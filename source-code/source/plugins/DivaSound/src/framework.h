#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <vector>
#include <thread>
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include <bassasio.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

void (*divaAudioInit)(void* cls, uint64_t unk, uint64_t unk2) = (void(*)(void* cls, uint64_t unk, uint64_t unk2))0x1406269F0;

void (*divaAudioFillbuffer)(void* mixer, int16_t* buf, uint64_t nFrames, bool disableHpVol, bool invertPhase) = (void(*)(void* mixer, int16_t* buf, uint64_t nFrames, bool disableHpVol, bool invertPhase))0x140627370;
// mixer is a pointer to an audioMixer (from audio init's cls + 0x70)
// disableHpVol uses speaker volume for headphones (only works in 4ch mode)
// invertPhase inverts the output signal (only works if disableHpVol == false)

int (*divaAudioAllocMixer)(void* cls, uint64_t unk, uint64_t unk2, int64_t nFrames) = (int(*)(void* cls, uint64_t unk, uint64_t unk2, int64_t nFrames))0x140626710;
// unks are the same as from the init call. they seem to set the internal mixing channel counts(?) 
// cls is the same as mixer in divaAudioFillbuffer
// nFrames is number of audio frames to hold in the mixing buffers (only used when divaAudioFillbuffer is called). Internally this is multiplied by 16 (buffers are built using 32bit floats)

int(*mtx_init)(void* mutex, int type) = (int(*)(void* mutex, int type))0x14081DEF4;
int(*mtx_lock_0)(void* mutex) = (int(*)(void* mutex))0x14081DF50;
int(*mtx_unlock)(void* mutex) = (int(*)(void* mutex))0x14081DFD4;

typedef void streamingCallback(float* buffer, int64_t nFrames, void* userdata);

#pragma pack(push, 1)
struct initClass; // define these earlier so they can be used in other structs
struct audioMixer;

// known streaming channel variables
// + 0x00 = pointer to mixer class
// + 0x08 = buffer pointer (float)
// + 0x10 = buffer size (frame count * 16)
// + 0x18 = playing (32 bits, set to 0 for pause)
// + 0x1c = reset (32 bits, set to 0 to kill channel)
// + 0x20 = mutex pointer
// + 0x28 = master volume float (multiply other volumes by this)
// + 0x2c = ch1 volume
// + 0x30 = ch2 volume
// + 0x34 = ch3 volume
// + 0x38 = ch4 volume
//
// + 0x40 = callback func
// + 0x48 = callback userdata pointer
struct streamingChannel {
	audioMixer* mixer;

	float* buffer;
	uint64_t buffer_size; // in bytes

	uint32_t playing; // set to 0 for pause (usually ==1)
	uint32_t reset; // set to 1 to kill (usually ==0)

	void* mutex;

	float volume_master;
	float volume_channels[4];
	byte padding3c[0x4];

	streamingCallback callback;
	void* userdata;
};

// known SE channel variables
// + 0x00 = pointer to mixer class
// + 0x08 = some kind of toggle (is playing)?

// + 0x10 = mutex pointer
// + 0x18 = master volume float (multiply other volumes by this)
// + 0x1c = ch1 volume
// + 0x20 = ch2 volume
// + 0x24 = ch3 volume
// + 0x28 = ch4 volume

// + 0x30 = buffer pointer (float)
// + 0x38 = buffer size????? (frame count * 16)??? (buffers seem to hold whole sounds)
// + 0x40 = num channels
// + 0x48 = num frames
// + 0x50 = sample rate???
// + 0x58 = loop start frame???
// + 0x60 = loop end frame???
// + 0x68 = current playback time
struct seChannel {
	audioMixer* mixer;

	uint32_t unk1; // toggle on/off?
	byte padding0c[0x4];

	void* mutex;

	float volume_master;
	float volume_channels[4];
	byte padding2c[0x4];

	float* buffer;
	byte padding38[0x8]; // probably buffer size

	int64_t nChannels; // seems to affect mapping

	uint64_t length; // in frames
	byte padding50[0x8]; // sample rate?
	uint64_t loop_start; // ?
	uint64_t loop_end; // ?
	uint64_t curTime;
};

// known internal audio mixer variables (used by divaAudioFillbuffer and divaAudioAllocMixer)
// cls + 0x00 = pointer to main audio/init class
// cls + 0x08 = SE channels pointer
// cls + 0x10 = SE channels count
// cls + 0x18 = streaming channels pointer
// cls + 0x20 = streaming channels count
// cls + 0x28 = mixing buffer pointer (when a buffer is generated, this is filled with 32bit floats (four per frame)
// cls + 0x30 = mixing buffer size (frame count * 16)
//
// cls + 0x38 = mutex lock for volume
// cls + 0x40 = master volume float (multiply other volumes by this)
// cls + 0x44 = ch1 volume
// cls + 0x48 = ch2 volume
// cls + 0x4c = ch3 volume
// cls + 0x50 = ch4 volume
struct audioMixer {
	initClass* audioClass;

	seChannel* seChannels;
	uint64_t seChannels_len;
	streamingChannel* streamingChannels;
	uint64_t streamingChannels_len;

	float* mixbuffer;
	uint64_t mixbuffer_size;

	void* volume_mutex; // not sure if this is pointer or not
	float volume_master;
	float volume_channels[4];

	byte padding54[4];
};

// known main/init audio class variables (used by divaAudioInit)
// cls + 0x0 = WAVEFORMATEXTENSIBLE wave_format (null with old method) (length 0x28)
// cls + 0x28 = IMMDeviceEnumerator *pEnumerator
// cls + 0x30 = IMMDevice *pDevice (default device)
// cls + 0x38 = IAudioClient *pAudioClient
//
// cls + 0x40 = buffer size in frames
//
// cls + 0x48 = IAudioRenderClient *pRenderClient
//
// cls + 0x50 = HANDLE hEvent (for original WASAPI buffer timing)
//
// cls + 0x58 = number of output channels
// cls + 0x60 = audio sample rate
// cls + 0x68 = audio bit depth (only 16bit works)
//
// cls + 0x70 = pointer to mixer class
//
// cls + 0x78 = HANDLE* hCallback (handle to original callback thread)
//
// cls + 0x80 = break flag dword (becomes 1 on exit)
struct initClass {
	WAVEFORMATEXTENSIBLE wave_format;
	IMMDeviceEnumerator *pEnumerator;
	IMMDevice *pDevice;
	IAudioClient *pAudioClient;


	uint64_t buffer_size;

	IAudioRenderClient *pRenderClient;
	HANDLE hEvent;

	uint64_t channels;
	uint64_t rate; // should always =44100
	uint64_t depth; // should always =16

	audioMixer* mixer;
	
	HANDLE* hCallback;
	int32_t breakDword;
};
#pragma pack(pop)


initClass *divaAudCls;
audioMixer *divaAudioMixCls;

std::thread loopThread;

ma_context_config contextConfig;
ma_context context;
ma_device_config deviceConfig;
ma_device device;

int maInternalBufferSizeInMilliseconds;
int divaBufSizeInFrames;
int divaBufSizeInMilliseconds;

int nChannels; // this can only be 2 or 4
int bitDepth; // signed 16/24 bit integer or 32 bit float
int requestBuffer;
int nPeriods;
wchar_t backendName[32]; // wasapi or directsound
ma_backend maBackend;
ma_share_mode maSharemode;
bool useAsio;
int asioDevice;
bool showAsioPanel;

bool useOldInit;

std::wstring ExePath() {
	WCHAR buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return std::wstring(buffer);
}

std::wstring DirPath() {
	std::wstring exepath = ExePath();
	std::wstring::size_type pos = exepath.find_last_of(L"\\/");
	return exepath.substr(0, pos);
}

std::wstring CONFIG_FILE_STRING = DirPath() + L"\\plugins\\DivaSound.ini";
LPCWSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();