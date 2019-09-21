#include "framework.h"
#include "PluginConfigApi.h"
#include <detours.h>
#pragma comment(lib, "detours.lib")

#include <windows.h>
#include <iostream>
#include <vector>
#include <strsafe.h>
#include <bassasio.h>
#include <shellapi.h>

void InjectCode(void* address, const std::vector<uint8_t> data)
{
	const size_t byteCount = data.size() * sizeof(uint8_t);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(address, data.data(), byteCount);
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}

void NopBytes(void* address, unsigned int num)
{
	std::vector<uint8_t> newbytes = {};

	for (unsigned int i = 0; i < num; ++i) newbytes.push_back(0x90);

	InjectCode(address, newbytes);
}

void resizeInternalBuffers(int frames)
{
	float* oldMixbuffer = divaAudioMixCls->mixbuffer;
	float* oldState2buffer = divaAudioMixCls->state2->buffer;

	divaBufSizeInFrames = frames;

	divaAudioMixCls->mixbuffer = (float*)malloc(divaBufSizeInFrames * 4 * 4);
	divaAudioMixCls->mixbuffer_size = divaBufSizeInFrames * 4 * 4;
	divaAudioMixCls->state2->buffer = (float*)malloc(divaBufSizeInFrames * 4 * 4);
	divaAudioMixCls->state2->buffer_size = divaBufSizeInFrames * 4 * 4;
	divaAudCls->buffer_size = divaBufSizeInFrames;

	free(oldMixbuffer);
	free(oldState2buffer);
	printf("[DivaSound] Resized internal buffers to %d frames\n", frames);
}

void resizeTestLoop()
{
	bool dir = true;
	while (true)
	{
		Sleep(5000);

		if (dir)
			resizeInternalBuffers(divaBufSizeInFrames + 3000);
		else
			resizeInternalBuffers(divaBufSizeInFrames - 3000);

		dir = !dir;
	}
}


void stopPlayback()
{
	if (useAsio)
	{
		if (BASS_ASIO_IsStarted()) BASS_ASIO_Stop();
	}
	else
	{
		if (ma_device_is_started(&device)) ma_device_stop(&device);
	}
}

void closeDevice()
{
	stopPlayback();

	if (useAsio)
	{
		BASS_ASIO_Free();
	}
	else
	{
		ma_device_uninit(&device);
		ma_context_uninit(&context);
	}
}


void audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	(void)pDevice;
	(void)pOutput;
	(void)pInput;
	(void)frameCount;

	if (divaAudCls->breakDword != 0 || divaAudioMixCls->mixbuffer == nullptr)
	{
		stopPlayback();
		return;
	}

	if (frameCount > divaBufSizeInFrames)
	{
		printf("[DivaSound] Warning: PDAFT buffer is too small\n");

		// allocate a larger buffer if necessary.
		// add 128 frames of padding because this isn't normal and we want to avoid it happening again if possible.
		resizeInternalBuffers(frameCount + 128);
	}
	
	divaAudioFillbuffer(divaAudioMixCls, (int16_t*)pOutput, frameCount, 0, 0);

	if (bitDepth > 16) // we should generate the output buffer ourselves
	{
		float volumes[4];
		for (int i = 0; i < 4; i++)
		{
			volumes[i] = divaAudioMixCls->volume_master * divaAudioMixCls->volume_channels[i];
		}


		int startChannel = 4 - nChannels;

		for (unsigned int i = 0; i < frameCount; i++)
		{
			for (int currentChannel = startChannel; currentChannel < 4; currentChannel++)
			{
				// note: currentChannel is for the game's buffer, outputChannel is for our output buffer.
				// outputChannel should also be used for volumes.
				int outputChannel = currentChannel - startChannel;

				if (bitDepth == 24) // 24 bit int output
				{
					int32_t out_val = (int32_t)(divaAudioMixCls->mixbuffer[i*4 + currentChannel] * volumes[outputChannel] * 8388607.0f);

					if (out_val > 8388607) out_val = 8388607;
					else if (out_val < -8388608) out_val = -8388608;

					uint64_t out_pos = (i*nChannels + outputChannel) * 3;

					((byte*)pOutput)[out_pos] = out_val;
					((byte*)pOutput)[out_pos + 1] = out_val >> 8;
					((byte*)pOutput)[out_pos + 2] = out_val >> 16;
				}

				else if (bitDepth == 32) // floating point output
				{
					((float*)pOutput)[i*nChannels + outputChannel] = divaAudioMixCls->mixbuffer[i*4 + currentChannel] * volumes[outputChannel];
				}
			}
		}
	}
}

DWORD asioCallback(bool input, DWORD channel, void* buffer, DWORD length, void* user)
{
	ma_uint32 frames = length * 8 / bitDepth / nChannels;
	audioCallback(&device, buffer, NULL, frames);
	return length;
}


void asioNotifyProc(DWORD notify, void* user);

bool asioInit(bool enablePanel)
{
	if (BASS_ASIO_Init == NULL)
	{
		printf("[DivaSound] BASS ASIO not loaded...\n");
		return false;
	}

	if (HIWORD(BASS_ASIO_GetVersion()) != BASSASIOVERSION)
	{
		printf("[DivaSound] Incorrect BASS ASIO version. Use 1.4.0.0.\n");
		return false;
	}

	if (!BASS_ASIO_Init(asioDevice, BASS_ASIO_THREAD))
	{
		printf("[DivaSound] Failed to initialize device\n");
		return false;
	}

	if (enablePanel && showAsioPanel) BASS_ASIO_ControlPanel();

	if (!BASS_ASIO_ChannelEnable(false, 0, (ASIOPROC*)asioCallback, NULL))
	{
		printf("[DivaSound] Failed to enable channels\n");
		BASS_ASIO_Free();
		return false;
	}
	for (int i = 1; i < nChannels; i++)
	{
		if (!BASS_ASIO_ChannelJoin(false, i, 0))
		{
			printf("[DivaSound] Failed to enable channels\n");
			BASS_ASIO_Free();
			return false;
		}
	}

	if (BASS_ASIO_CheckRate(44100))
		BASS_ASIO_SetRate(44100);

	BASS_ASIO_ChannelSetRate(false, 0, 44100);

	if (bitDepth == 32)
		BASS_ASIO_ChannelSetFormat(false, 0, BASS_ASIO_FORMAT_FLOAT);
	else if (bitDepth == 24)
		BASS_ASIO_ChannelSetFormat(false, 0, BASS_ASIO_FORMAT_24BIT);
	else
		BASS_ASIO_ChannelSetFormat(false, 0, BASS_ASIO_FORMAT_16BIT);

	BASS_ASIO_SetNotify((ASIONOTIFYPROC*)asioNotifyProc, NULL);

	return true;
}

void asioReinit()
{
	printf("[DivaSound] Reinitialising ASIO device...\n");
	closeDevice();

	if (!asioInit(false)) return;

	BASS_ASIO_INFO asioInfo;
	BASS_ASIO_GetInfo(&asioInfo);
	double actualRate = BASS_ASIO_GetRate();

	printf("[DivaSound] Output buffer size: %d (%dms at %dHz)\n", asioInfo.bufpref, (int)(asioInfo.bufpref * 1000 / actualRate), (int)actualRate);

	divaBufSizeInFrames = (int)(asioInfo.bufpref * 44100 / actualRate);
	divaBufSizeInMilliseconds = divaBufSizeInFrames * 1000 / 44100;
	printf("[DivaSound] PDAFT buffer size: %d (%dms at %dHz)\n", divaBufSizeInFrames, divaBufSizeInMilliseconds, 44100);

	resizeInternalBuffers(divaBufSizeInFrames);

	if (!BASS_ASIO_Start(asioInfo.bufpref, 1))
	{
		printf("[DivaSound] Failed to start playback device\n");
		BASS_ASIO_Free();
		return;
	}

	printf("[DivaSound] Started playback\n");
}

void asioNotifyProc(DWORD notify, void* user)
{
	if (notify == BASS_ASIO_NOTIFY_RESET)
	{
		std::thread asioReinitThread(asioReinit);
		asioReinitThread.join();
	}
}


bool maInit()
{
	ma_backend backends[] = { maBackend };

	contextConfig = ma_context_config_init();
	contextConfig.threadPriority = ma_thread_priority_highest;

	if (ma_context_init(backends, sizeof(backends) / sizeof(backends[0]), &contextConfig, &context) != MA_SUCCESS) {
		printf("[DivaSound] Failed to initialize context\n");
		return false;
	}


	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.channels = nChannels;
	deviceConfig.sampleRate = 44100;
	deviceConfig.bufferSizeInMilliseconds = requestBuffer; // actual result may be larger
	deviceConfig.periods = nPeriods;
	deviceConfig.dataCallback = audioCallback;
	deviceConfig.pUserData = NULL;
	deviceConfig.playback.shareMode = maSharemode;

	if (bitDepth == 32)
		deviceConfig.playback.format = ma_format_f32;
	else if (bitDepth == 24)
		deviceConfig.playback.format = ma_format_s24;
	else
		deviceConfig.playback.format = ma_format_s16;

	if (nChannels == 4)
	{
		deviceConfig.playback.channelMap[0] = MA_CHANNEL_FRONT_LEFT;
		deviceConfig.playback.channelMap[1] = MA_CHANNEL_FRONT_RIGHT;
		deviceConfig.playback.channelMap[2] = MA_CHANNEL_BACK_LEFT;
		deviceConfig.playback.channelMap[3] = MA_CHANNEL_BACK_RIGHT;
	}


	if (ma_device_init(&context, &deviceConfig, &device) != MA_SUCCESS) {
		printf("[DivaSound] Failed to open playback device\n");
		ma_context_uninit(&context);
		return false;
	}
	//printf("[DivaSound] Opened playback device\n");

	return true;
}


void loadConfig()
{
	nChannels = GetPrivateProfileIntW(L"general", L"channels", 2, CONFIG_FILE);
	if (nChannels != 4) nChannels = 2;


	bitDepth = GetPrivateProfileIntW(L"general", L"bit_depth", 16, CONFIG_FILE);
	if (bitDepth != 32 && bitDepth != 24) bitDepth = 16;


	requestBuffer = GetPrivateProfileIntW(L"buffer", L"buffer_size", 10, CONFIG_FILE);
	if (requestBuffer < 0) requestBuffer = 0;


	nPeriods = GetPrivateProfileIntW(L"buffer", L"periods", 2, CONFIG_FILE);
	if (nPeriods < 1) nPeriods = 1;


	useOldInit = GetPrivateProfileIntW(L"general", L"alternate_init", 1, CONFIG_FILE) > 0 ? false : true;


	GetPrivateProfileStringW(L"general", L"backend", L"wasapi", backendName, 32, CONFIG_FILE);
	for (wchar_t& chr : backendName)
		chr = towlower(chr);

	useAsio = false;
	maSharemode = ma_share_mode_shared;

	if (lstrcmpW(backendName, L"asio") == 0)
	{
		StringCchCopyW(backendName, 32, L"ASIO");
		useAsio = true;
	}
	else if (lstrcmpW(backendName, L"directsound") == 0)
	{
		StringCchCopyW(backendName, 32, L"DirectSound");
		maBackend = ma_backend_dsound;
	}
	else if (lstrcmpW(backendName, L"wasapi_exclusive") == 0)
	{
		StringCchCopyW(backendName, 32, L"WASAPI (Exclusive mode)");
		maBackend = ma_backend_wasapi;
		maSharemode = ma_share_mode_exclusive;
	}
	else
	{
		StringCchCopyW(backendName, 32, L"WASAPI");
		maBackend = ma_backend_wasapi;
	}


	asioDevice = GetPrivateProfileIntW(L"asio", L"device", -1, CONFIG_FILE);

	showAsioPanel = GetPrivateProfileIntW(L"asio", L"show_config", 0, CONFIG_FILE) > 0 ? true : false;
}

void hookedAudioInit(initClass *cls, uint64_t unk, uint64_t unk2)
{
	//printf("[DivaSound] Loaded\n");
	printf("[DivaSound] Output config: %S %dch %dbit\n", backendName, nChannels, bitDepth);

	divaAudCls = cls;
	divaAudCls->breakDword = 0;

	if (useOldInit)
	{
		printf("[DivaSound] Using old initialisation method\n");
		// let the game set some stuff up
		// might be necessary sometimes
		divaAudioInit(divaAudCls, unk, unk2);
		//printf("[DivaSound] Game audio initialised\n");
	}
	else
	{
		// setup some variables instead of using the original init function
		divaAudCls->mixer = new audioMixer();
		divaAudCls->mixer->volume_master = 1.0f;
		divaAudCls->mixer->volume_channels[0] = 1.0f;
		divaAudCls->mixer->volume_channels[1] = 1.0f;
		divaAudCls->mixer->volume_channels[2] = 1.0f;
		divaAudCls->mixer->volume_channels[3] = 1.0f;
		divaAudCls->mixer->audioClass = divaAudCls;

		mtx_init(&divaAudCls->mixer->volume_mutex, 2);
	}

	divaAudioMixCls = divaAudCls->mixer;
	
	divaAudCls->channels = nChannels; // this could replace stereo patch
	divaAudCls->rate = 44100; // really does nothing
	divaAudCls->depth = bitDepth; // setting this to something other than 16 just removes output


	if (useAsio)
	{
		if (!asioInit(true)) return;

		BASS_ASIO_INFO asioInfo;
		BASS_ASIO_GetInfo(&asioInfo);
		double actualRate = BASS_ASIO_GetRate();

		printf("[DivaSound] Output buffer size: %d (%dms at %dHz)\n", asioInfo.bufpref, (int)(asioInfo.bufpref * 1000 / actualRate), (int)actualRate);

		divaBufSizeInFrames = (int)(asioInfo.bufpref * 44100 / actualRate);
		divaBufSizeInMilliseconds = divaBufSizeInFrames * 1000 / 44100;
		printf("[DivaSound] PDAFT buffer size: %d (%dms at %dHz)\n", divaBufSizeInFrames, divaBufSizeInMilliseconds, 44100);


		divaAudioAllocMixer(divaAudioMixCls, unk, unk2, divaBufSizeInFrames);
		printf("[DivaSound] Created internal audio mixer\n");

		if (!BASS_ASIO_Start(asioInfo.bufpref, 1))
		{
			printf("[DivaSound] Failed to start playback device\n");
			BASS_ASIO_Free();
			return;
		}

		printf("[DivaSound] Started playback\n");
	}
	else
	{
		if (!maInit()) return;

		maInternalBufferSizeInMilliseconds = device.playback.internalBufferSizeInFrames * 1000 / device.playback.internalSampleRate; // because miniaudio doesn't seem to have this
		printf("[DivaSound] Output buffer size: %d (%dms at %dHz)\n", device.playback.internalBufferSizeInFrames, maInternalBufferSizeInMilliseconds, device.playback.internalSampleRate);
		printf("[DivaSound] Buffer periods: %d\n", device.playback.internalPeriods);

		divaBufSizeInFrames = device.playback.internalBufferSizeInFrames * device.sampleRate / device.playback.internalSampleRate; // +128; // 128 is just a bit extra in case resampling needs it or something. idk
		divaBufSizeInMilliseconds = divaBufSizeInFrames * 1000 / device.sampleRate;
		printf("[DivaSound] PDAFT buffer size: %d (%dms at %dHz)\n", divaBufSizeInFrames, divaBufSizeInMilliseconds, device.sampleRate);


		divaAudioAllocMixer(divaAudioMixCls, unk, unk2, divaBufSizeInFrames);
		printf("[DivaSound] Created internal audio mixer\n");


		if (ma_device_start(&device) != MA_SUCCESS) {
			printf("[DivaSound] Failed to start playback device\n");
			ma_device_uninit(&device);
			ma_context_uninit(&context);
			return;
		}
		printf("[DivaSound] Started playback\n");
	}

	divaAudCls->hCallback = new HANDLE();
	*divaAudCls->hCallback = (HANDLE)1; // this *should* be an actual thread handle, but I bypassed the destruction stuff anyway lol

	divaAudCls->buffer_size = divaBufSizeInFrames;
	//loopThread = std::thread(resizeTestLoop);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		loadConfig();

		if (useOldInit)
		{
			 // these patches are only needed if calling the game's own init function
			 // they shouldn't be necessary anymore

			// force stereo mode
			InjectCode((void*)0x0000000140A860C0, { 0x02 });

			// remove a call to get device info and skip the check for it
			NopBytes((void*)0x140626b56, 7);
			NopBytes((void*)0x140626b8a, 8);
			InjectCode((void*)0x140626b8a, { 0x48, 0x83, 0xEF, 0x18 }); // fix value of RDI because I changed the flow here
			NopBytes((void*)0x140626ba9, 11);

			// return from the original init early
			InjectCode((void*)0x0000000140626C29, { 0x48, 0xE9 });
		}

		// skip some thread deinitialisation stuff I don't care about
		InjectCode((void*)0x0000000140625F42, { 0xEB, 0x29 });
    
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaAudioInit, (PVOID)hookedAudioInit);
		DetourTransactionCommit();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		closeDevice();
	}
	return TRUE;
}


using namespace PluginConfig;

void OpenWiki()
{
	ShellExecuteW(NULL, L"open", L"https://github.com/somewhatlurker/DivaSound/wiki", NULL, NULL, SW_SHOW);
}

PluginConfigOption config[] = {
	{ CONFIG_DROPDOWN_TEXT, new PluginConfigDropdownTextData{L"backend", L"general", CONFIG_FILE, L"Backend:", L"Sets the audio output protocol.", L"WASAPI", std::vector<LPCWSTR>({ L"WASAPI", L"WASAPI_Exclusive" }), true, false } },
	{ CONFIG_DROPDOWN_NUMBER, new PluginConfigDropdownNumberData{ L"channels", L"general", CONFIG_FILE, L"Channels:", L"Sets the number of channels.", 2, std::vector<int>({ 2, 4 }), false } },
	{ CONFIG_DROPDOWN_NUMBER, new PluginConfigDropdownNumberData{ L"bit_depth", L"general", CONFIG_FILE, L"Bit Depth:", L"Sets the audio sample format.\n(32 uses floating point samples)", 16, std::vector<int>({ 16, 24, 32 }), false } },
	{ CONFIG_NUMERIC, new PluginConfigNumericData{ L"buffer_size", L"buffer", CONFIG_FILE, L"Target Buffer Size:", L"Sets the target buffer size in ms.\nWASAPI will often ignore this and adapt to your hardware config automatically.", 10, 1, 100 } },
	{ CONFIG_NUMERIC, new PluginConfigNumericData{ L"periods", L"buffer", CONFIG_FILE, L"Buffer Periods:", L"Sets how often the buffer should be filled.\nFewer periods usually allows for lower latency, but lowering this may cause issues.", 2, 1, 8 } },
	{ CONFIG_BOOLEAN, new PluginConfigBooleanData{ L"alternate_init", L"general", CONFIG_FILE, L"Use new init", L"Use the full initialisation replacement.\nTry unchecking this if DivaSound seems to cause crashes.", true } },
	{ CONFIG_SPACER, new PluginConfigSpacerData{ 8 } },
	{ CONFIG_BUTTON, new PluginConfigButtonData{ L"Help", L"Get help on the DivaSound wiki.", OpenWiki } },
	{ CONFIG_SPACER, new PluginConfigSpacerData{ 8 } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"DivaSound";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"DivaSound Plugin by somewhatlurker\n\nDivaSound replaces the game's original audio output,\nimproving device support and adding configurable options.";
}

extern "C" __declspec(dllexport) PluginConfigArray GetPluginOptions(void)
{
	return PluginConfigArray{ _countof(config), config };
}