#include "framework.h"
#include "PluginConfigApi.h"

#include <evr.h>

#include <d3d9.h>
#include <dvdmedia.h>
#include <dxva2api.h>

#include <mfapi.h>
#include <mferror.h>
#include <mftransform.h>
#include <SimpleIni.h>

bool forceSoftwareDecoding = false, forceHybridDecoding = false, debug = false;

IDirect3DDeviceManager9* deviceManager;

IMFTransform* mfTransform;
int width;
int height;
unsigned long samplesize;
unsigned long subtype;

IMFSample* dstSample;
IDirect3DSurface9* dstSurface;
IMFSample* srcSample;
IMFMediaBuffer* srcBuffer;

HRESULT initialize(IMFTransform* transform)
{
	HRESULT result = S_OK;

	IMFMediaType* type = NULL;
	IMFMediaBuffer* buffer = NULL;
	HANDLE hDevice = NULL;
	IDirectXVideoAccelerationService* service = NULL;

	AM_MEDIA_TYPE* format = NULL;
	VIDEOINFOHEADER2* header = NULL;

	// Release what we already had beforehand
	SAFE_RELEASE(mfTransform);
	SAFE_RELEASE(dstSample);
	SAFE_RELEASE(dstSurface);
	SAFE_RELEASE(srcSample);
	SAFE_RELEASE(srcBuffer);

	ASSERT(transform->GetOutputCurrentType(0, &type));
	ASSERT(type->GetRepresentation(AM_MEDIA_TYPE_REPRESENTATION, (void**)&format));

	header = (VIDEOINFOHEADER2*)format->pbFormat;

	width = header->bmiHeader.biWidth;
	height = header->bmiHeader.biHeight;
	samplesize = format->lSampleSize;
	subtype = format->subtype.Data1;

	// System memory sample
	ASSERT(MFCreateSample(&srcSample));
	ASSERT(MFCreateMemoryBuffer(format->lSampleSize, &buffer));
	ASSERT(srcSample->AddBuffer(buffer));
	ASSERT(buffer->QueryInterface(&srcBuffer));

	// Video memory sample
	ASSERT(deviceManager->OpenDeviceHandle(&hDevice));
	ASSERT(deviceManager->GetVideoService(hDevice, __uuidof(IDirectXVideoAccelerationService), (void**)& service));
	ASSERT(service->CreateSurface(width, height, 0, (D3DFORMAT)subtype, D3DPOOL_DEFAULT, 0, DXVA2_VideoSoftwareRenderTarget, &dstSurface, NULL));
	ASSERT(MFCreateVideoSampleFromSurface(dstSurface, &dstSample));
	ASSERT(deviceManager->CloseDeviceHandle(hDevice));

	mfTransform = transform;
	mfTransform->AddRef();

end:
	if (format)
		type->FreeRepresentation(AM_MEDIA_TYPE_REPRESENTATION, format);

	SAFE_RELEASE(service);
	SAFE_RELEASE(buffer);
	SAFE_RELEASE(type);

	return result;
}

VTABLE_HOOK(HRESULT, IMFTransform, ProcessOutput, DWORD dwFlags, DWORD cOutputBufferCount,
	MFT_OUTPUT_DATA_BUFFER* pOutputSamples, DWORD* pdwStatus)
{
	if (pOutputSamples->pSample != NULL || deviceManager == NULL || cOutputBufferCount != 1)
		return originalProcessOutput(This, dwFlags, cOutputBufferCount, pOutputSamples, pdwStatus);

	HRESULT result = S_OK;

	if (mfTransform != This)
		ASSERT(initialize(This));

	pOutputSamples->pSample = srcSample;
	pOutputSamples->pSample->AddRef();

	ASSERT(originalProcessOutput(This, dwFlags, cOutputBufferCount, pOutputSamples, pdwStatus));

	D3DLOCKED_RECT rect;
	BYTE* buf;
	ASSERT(srcBuffer->Lock(&buf, NULL, NULL));
	ASSERT(dstSurface->LockRect(&rect, NULL, NULL));
	ASSERT(MFCopyImage((BYTE*)rect.pBits, rect.Pitch, buf, samplesize, samplesize, 1));
	ASSERT(dstSurface->UnlockRect());
	ASSERT(srcBuffer->Unlock());

	LONGLONG duration;
	ASSERT(srcSample->GetSampleDuration(&duration));
	ASSERT(dstSample->SetSampleDuration(duration));

	LONGLONG time;
	ASSERT(srcSample->GetSampleTime(&time));
	ASSERT(dstSample->SetSampleTime(time));

	DWORD flags;
	ASSERT(srcSample->GetSampleFlags(&flags));
	ASSERT(dstSample->SetSampleFlags(flags));

	pOutputSamples->pSample->Release();
	pOutputSamples->pSample = dstSample;
	pOutputSamples->pSample->AddRef();
end:
	return result;
}

VTABLE_HOOK(HRESULT, IMFTransform, ProcessMessage, MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
{
	if (forceSoftwareDecoding)
	{
		if (eMessage == MFT_MESSAGE_SET_D3D_MANAGER) {
			PRINT("[DivaMovie] Force Software Decoding enabled\n");
			INSTALL_VTABLE_HOOK(This, ProcessOutput, 25);
			return S_OK;
		}
		return originalProcessMessage(This, eMessage, ulParam);
	}
	else
	{
		HRESULT result = originalProcessMessage(This, eMessage, ulParam);
		if (eMessage == MFT_MESSAGE_SET_D3D_MANAGER && result == MF_E_UNSUPPORTED_D3D_TYPE)
		{
			PRINT("[DivaMovie] This system does not support DXVA hardware decoding\n");
			result = originalProcessMessage(This, eMessage, NULL);
			if (SUCCEEDED(result))
				INSTALL_VTABLE_HOOK(This, ProcessOutput, 25);
		}
		else if (!forceHybridDecoding && eMessage == MFT_MESSAGE_SET_D3D_MANAGER && result == 0)
		{
			static bool warn = false;
			if (!warn)
			{
				MessageBoxW(0, L"Your system supports DXVA hardware decoding.\nDivaMovie will be disabled.\n\nIt is highly recommended to restart the game now.", L"DivaMovie", MB_ICONWARNING);

				CSimpleIniW ini_reader;
				ini_reader.SetUnicode(false);
				ini_reader.LoadFile(MASTER_CONFIG_FILE);
				ini_reader.SetValue(L"plugins", L"DivaMovie.dva", L"0");
				ini_reader.SaveFile(MASTER_CONFIG_FILE);

				warn = true;
			}
		}
		return result;
	}
	
}

HOOK(void*, IMFTransformInitializer, 0x140420B90, void* a1, void* a2, IMFTransform** transform)
{
	void* result = originalIMFTransformInitializer(a1, a2, transform);
	INSTALL_VTABLE_HOOK(*transform, ProcessMessage, 23);
	return result;
}

HOOK(HRESULT, DXVA2CreateDirect3DDeviceManager, PROC_ADDRESS("dxva2.dll", "DXVA2CreateDirect3DDeviceManager9"),
	UINT* pResetToken, IDirect3DDeviceManager9** ppDeviceManager)
{
	HRESULT result = originalDXVA2CreateDirect3DDeviceManager(pResetToken, ppDeviceManager);

	if (FAILED(result))
		return result;

	if (deviceManager)
		deviceManager->Release();

	deviceManager = *ppDeviceManager;
	deviceManager->AddRef();

	return result;
}

void loadConfig() {
	forceSoftwareDecoding = GetPrivateProfileIntW(L"general", L"force_software_decoding", 0, CONFIG_FILE) > 0 ? true : false;
	forceHybridDecoding = GetPrivateProfileIntW(L"general", L"force_hybrid_decoding", 0, CONFIG_FILE) > 0 ? true : false;
	debug = GetPrivateProfileIntW(L"general", L"debug", 0, CONFIG_FILE) > 0 ? true : false;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		loadConfig();
		INSTALL_HOOK(DXVA2CreateDirect3DDeviceManager);
		INSTALL_HOOK(IMFTransformInitializer);
	}

	return TRUE;
}

PluginConfig::PluginConfigOption config[] = {
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"force_software_decoding", L"general", CONFIG_FILE, L"Force Software Decoding", L"Use software decoding even on systems that support DXVA hardware decoding.", false } },
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"force_hybrid_decoding", L"general", CONFIG_FILE, L"Force Hybrid Decoding", L"Do not disable DivaMovie on systems that support DXVA hardware decoding.\nCan cause issues on recent drivers.", false } },
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"debug", L"general", CONFIG_FILE, L"Debug (slow!)", L"Enable PRINT (possibly at the cost of performance).", false } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"DivaMovie";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"DivaMovie Plugin by Skyth\n\nDivaMovie enables movies on systems that do not support DXVA hardware decoding.";
}

extern "C" __declspec(dllexport) PluginConfig::PluginConfigArray GetPluginOptions(void)
{
	return PluginConfig::PluginConfigArray{ _countof(config), config };
}