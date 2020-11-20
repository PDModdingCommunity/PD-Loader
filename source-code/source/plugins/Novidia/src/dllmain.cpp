#include "framework.h"
#include "glStuff.h"
#include <PluginConfigApi.h>
#include <GPUModel.h>

extern "C" {
// well, only good for 64 bit now, ont that is matters lol
#define SIZEOF_SIZE_T 8
#define SIZEOF_UNSIGNED_LONG_LONG 8
#include <xdelta3.h>
#include <xdelta3.c>
#include "crc/crc.h"
}


// upload to the SSBO instead after processing
// two versions because apparently TexSubImage can cause major stuttering
void h_uploadModelTransformBuf_TexImage(DWORD* a1, int a2)
{
	if (a2 > 0x100)
		return;

	uploadModelTransformBuf(a1, a2);
	
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_1D, buf_tex);
	// can't change tex size without making shader sad :(
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 0x3000 / sizeof(float) / 4, 0, tex_upload_format, GL_FLOAT, *(float**)0x1411a3330);
	glActiveTexture(GL_TEXTURE0);
}
void h_uploadModelTransformBuf_TexSubImage(DWORD* a1, int a2)
{
	if (a2 > 0x100)
		return;

	uploadModelTransformBuf(a1, a2);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_1D, buf_tex);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, a2 * 3, tex_upload_format, GL_FLOAT, *(float**)0x1411a3330);
	glActiveTexture(GL_TEXTURE0);
}

void h_uploadModelTransformBuf_NoUpload(DWORD* a1, int a2)
{
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_1D, buf_tex);
	glActiveTexture(GL_TEXTURE0);
}

// just a crash fix
void h_glBindBuffer(GLenum target, GLuint buffer)
{
	// if not okay, it's param buffer data
	if (!checkBufferTargetOk(target))
	{
		return;
	}

	glBindBuffer(target, buffer);
}

// just a crash fix
void h_glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
	// if not okay, it's param buffer data
	if (!checkBufferTargetOk(target))
	{
		return;
	}
	
	glBufferData(target, size, data, usage);
}

PROC h_wglGetProcAddress(LPCSTR Arg1)
{
	PROC res = wGlGetProcAddress_forhook(Arg1);
	if (res == nullptr)
		return (PROC)stub;
	return res;
}

// used as an entry point after GL context is created
void h_glutSetCursor(int cursor)
{
	if (!loadGlAddresses())
	{
		printf("[Novidia] Aborting loading\n");
		//MessageBoxA(NULL, "!", NULL, NULL);
	}
	else
	{

		struct {
			void* target;
			void* hook;
		} hookFuncs[] = {
			{&glBindBuffer, h_glBindBuffer},
			{&glBindBufferARB, h_glBindBuffer},
			{&glBufferData, h_glBufferData},
			{&glBufferDataARB, h_glBufferData},
			//{&glDrawElements, h_glDrawElements},
			//{&glDrawRangeElements, h_glDrawRangeElements},
		};

		for (auto fn : hookFuncs)
		{
			hookGlFunc((PROC*)fn.target, fn.hook);
		}

		//glActiveTexture(GL_TEXTURE8);
		glGenTextures(1, &buf_tex);
		printf("[Novidia] Buffer texture id: %d\n", buf_tex);
		glBindTexture(GL_TEXTURE_1D, buf_tex);
		if (enable_chara_skinning && use_TexSubImage)
			glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA32F, 0x3000 / sizeof(float) / 4);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		// BGRA texture uploads seem to run a little faster
		// ideally just asking the driver what to use would be fine,
		// but my driver says GL_RGBA is better (contrary to my testing)
		if (force_BGRA_upload)
		{
			tex_upload_format = GL_BGRA;
		}
		else
		{
			// ask the driver which image format to use for uploads
			glGetInternalformativ(GL_TEXTURE_1D, GL_RGBA32F, GL_TEXTURE_IMAGE_FORMAT, 1, &tex_upload_format);
			printf("[Novidia] Driver preferred texture upload format: %d\n", tex_upload_format);
		}

		if (tex_upload_format == GL_BGRA)
		{
			printf("[Novidia] Using BGRA texture uploads\n");
			// set swizzling to make shader read as if it's RGBA still
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		}
		else
		{
			tex_upload_format = GL_RGBA;
			printf("[Novidia] Using RGBA texture uploads\n");
		}

		if (!enable_chara_skinning)
		{
			struct vec4 {
				float x;
				float y;
				float z;
				float w;
			};

			struct mtx {
				vec4 xMul;
				vec4 yMul;
				vec4 zMul;
			};

			/*
			mtx no_anim_mtx = {
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 1, 0},
			};
			*/

			mtx no_anim_mtx = {
				{0, 0, 0, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0},
			};

			float uploadBuf[0x3000 / sizeof(float)];

			for (int i = 0; i < sizeof(uploadBuf) / sizeof(float); i += sizeof(no_anim_mtx) / sizeof(float))
			{
				// yes, I really could just memset it to all zeros lol
				memcpy(&uploadBuf[i], &no_anim_mtx, sizeof(no_anim_mtx));
			}
			
			glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 0x3000 / sizeof(float) / 4, 0, tex_upload_format, GL_FLOAT, uploadBuf);
		}
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	printf("[Novidia] Unhooking glutSetCursor\n");
	DetourDetach(&(PVOID&)glutSetCursor, h_glutSetCursor);
	DetourTransactionCommit();

	glutSetCursor(cursor);
}

int64_t hookedGetFileSize(MsString* path) {
	// store original size for use later or to return in case of error
	int64_t ogsize = divaGetFileSize(path);

	std::string pathStr = std::string(path->GetCharBuf());
	static std::string shdfarc = "shader.farc";


	// identify whether current file is shader or not
	bool fileIsShader = false;
	if (pathStr.length() >= shdfarc.length())
	{
		fileIsShader = (0 == pathStr.compare(pathStr.length() - shdfarc.length(), shdfarc.length(), shdfarc));
	}

	if (!fileIsShader)
	{
		return ogsize;
	}


	// reset some stuff when reprocessing shader farc (should never happen)
	if (shader_farc_data) {
		free(shader_farc_data);
		shader_farc_data = NULL;
		shader_farc_data_size = 0;
		shader_farc_path = "";
		shader_file_handle = NULL;
	}


	// local vars declared early to allow use of goto
	FILE* ogfile;
	void* ogdata = NULL;
	DWORD crc;
	wchar_t crcstr[9];
	std::wstring vcd_path;
	DWORD dwAttrib;
	FILE* vcdfile;
	long vcdsize;
	void* vcddata = NULL;
	void* outbuf = NULL;
	usize_t outbuf_size;
	usize_t outbuf_data_size;
	int xd3err;


	// if shader, open it and read data
	//if (fopen_s(&ogfile, path->GetCharBuf(), "rb") != 0)
	ogfile = divaFsopen(path->GetCharBuf(), "rb", _SH_DENYNO);
	if (ogfile == nullptr)
	{
		MessageBoxW(NULL, L"Error opening shader.farc.", L"Novidia", NULL);
		goto fail;
	}

	ogdata = malloc(ogsize);
	//if (fread(ogdata, 1, ogsize, ogfile) != ogsize)
	if (divaFread(ogdata, 1, ogsize, ogfile) != ogsize)
	{
		MessageBoxW(NULL, L"Error reading shader.farc.", L"Novidia", NULL);
		goto fail;
	}

	//fclose(ogfile);
	divaFclose(ogfile); // need to use diva's fclose if using diva's fsopen and fread


	// crc32 the shader data to find a vcdiff patch file
	crc = crc32buf((char*)ogdata, ogsize);
	swprintf_s(crcstr, L"%08x", crc);
	//MessageBoxW(NULL, crcstr, L"Novidia", NULL);

	vcd_path = DirPath() + L"\\plugins\\Novidia Shaders\\" + crcstr + L".vcdiff";
	
	dwAttrib = GetFileAttributesW(vcd_path.c_str());
	if (dwAttrib == INVALID_FILE_ATTRIBUTES || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		MessageBoxW(NULL, L"Unable to find shader vcdiff file.", L"Novidia", NULL);
		MessageBoxW(NULL, vcd_path.c_str(), L"Novidia", NULL);
		goto fail;
	}


	// try opening and reading said vcdiff patch
	if (_wfopen_s(&vcdfile, vcd_path.c_str(), L"rb") != 0)
	{
		MessageBoxW(NULL, L"Error opening shader vcdiff file.", L"Novidia", NULL);
		MessageBoxW(NULL, vcd_path.c_str(), L"Novidia", NULL);
		goto fail;
	}

	// get size of the file from having already opened it, skipping whatever the game does
	fseek(vcdfile, 0, SEEK_END); // seek to end of file
	vcdsize = ftell(vcdfile); // get current file pointer
	fseek(vcdfile, 0, SEEK_SET); // seek back to beginning of file

	vcddata = malloc(vcdsize);
	if (fread(vcddata, 1, vcdsize, vcdfile) != vcdsize)
	{
		MessageBoxW(NULL, L"Error reading shader vcdiff file.", L"Novidia", NULL);
		MessageBoxW(NULL, vcd_path.c_str(), L"Novidia", NULL);
		goto fail;
	}

	fclose(vcdfile);

	
	// allocate an output buffer and patch shader into it (needs to be done now to know correct size)
	outbuf_size = 64 * 1024 * 1024; // 64M should be enough
	outbuf = malloc(outbuf_size);
	outbuf_data_size;
	xd3err = xd3_decode_memory((uint8_t*)vcddata, vcdsize, (uint8_t*)ogdata, ogsize, (uint8_t*)outbuf, &outbuf_data_size, outbuf_size, 0);

	if (xd3err != 0)
	{
		MessageBoxW(NULL, L"Error applying shader vcdiff file.", L"Novidia", NULL);
		MessageBoxA(NULL, xd3_strerror(xd3err), "Novidia", NULL);
		goto fail;
	}

	wprintf(L"[Novidia] Patched shader.farc using %s.vcdiff\n", crcstr);

	// done successfully! cleanup and save results
	free(ogdata);
	free(vcddata);
	shader_farc_data = outbuf;
	shader_farc_data_size = outbuf_data_size;
	shader_farc_path = pathStr;

	return outbuf_data_size;

fail:
	if (ogdata) free(ogdata);
	if (vcddata) free(vcddata);
	if (outbuf) free(outbuf);
	return ogsize;
}
FILE* hookedFsopen(const char* path, const char* mode, int shflag)
{
	FILE* res = divaFsopen(path, mode, shflag);

	if (strcmp(path, shader_farc_path.c_str()) == 0)
	{
		shader_file_handle = res;
	}
	return res;
}
int64_t hookedFread(void* dst, int64_t size, int64_t count, FILE* file)
{
	if (file != shader_file_handle)
	{
		return divaFread(dst, size, count, file);
	}

	size_t size_bytes = size * count;

	if ((size_bytes > shader_farc_data_size) || !shader_farc_data)
	{
		return divaFread(dst, size, count, file); // this is an error -- the game wants more data than we have for some reason, or size has a value but not our buffer
	}

	memcpy(dst, shader_farc_data, size_bytes);

	shader_file_handle = NULL; // easy way to avoid risk of unexpected reuse

	// optionally, could free the stored farc data here, but it only uses 64MB so why bother?
	/*
	free(shader_farc_data); // no longer need to keep this around so free it and save the RAM
	shader_farc_data = NULL; // clear associated data too, because there's no longer a processed shader
	shader_farc_data_size = 0;
	shader_farc_path = "";
	*/

	printf("[Novidia] Shader patch applied\n");

	return size_bytes;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		loadConfig();

		if (!disable_amd_check)
		{
			std::string gpuName = GPUModel::getGpuName();
			if (gpuName != "AMD")
			{
				// detected Nvidia GPU
				printf("[Novidia] Detected Non-AMD GPU! Quitting!\n");
				return TRUE;
			}
		}

		DisableThreadLibraryCalls(hModule);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		printf("[Novidia] Hooking glutSetCursor\n");
		DetourAttach(&(PVOID&)glutSetCursor, h_glutSetCursor);
		printf("[Novidia] Hooking wglGetProcAddress\n");
		DetourAttach(&(PVOID&)wGlGetProcAddress_forhook, h_wglGetProcAddress);

		printf("[Novidia] Hooking uploadModelTransformBuf\n");
		if (enable_chara_skinning)
		{
			if (use_TexSubImage)
				DetourAttach(&(PVOID&)uploadModelTransformBuf, h_uploadModelTransformBuf_TexSubImage);
			else
				DetourAttach(&(PVOID&)uploadModelTransformBuf, h_uploadModelTransformBuf_TexImage);
		}
		else
		{
			DetourAttach(&(PVOID&)uploadModelTransformBuf, h_uploadModelTransformBuf_NoUpload);
		}

		if (enable_shader_deltas)
		{
			printf("[Novidia] Hooking divaGetFileSize\n");
			DetourAttach(&(PVOID&)divaGetFileSize, hookedGetFileSize);
			printf("[Novidia] Hooking divaFsopen\n");
			DetourAttach(&(PVOID&)divaFsopen, hookedFsopen);
			printf("[Novidia] Hooking divaFread\n");
			DetourAttach(&(PVOID&)divaFread, hookedFread);
		}

		DetourTransactionCommit();

		/*
		if (shader_amd_farc)
		{
			const char* shaderpath = "./rom/shader_amd.farc";
			InjectCode((void*)0x140a41018, std::vector<uint8_t>(shaderpath, shaderpath + strlen(shaderpath)));
		}
		*/
	}

	return TRUE;
}



using namespace PluginConfig;

PluginConfigOption config[] = {
	{ CONFIG_BOOLEAN, new PluginConfigBooleanData{ L"disable_amd_check", L"general", CONFIG_FILE, L"Disable AMD Check", L"On systems with both AMD and Nvidia graphics, the plugin may disable functionality due to detecting a non-AMD GPU.\nSet this to forcefully enable functionality.", false, false } },
	{ CONFIG_BOOLEAN, new PluginConfigBooleanData{ L"enable_chara_skinning", L"general", CONFIG_FILE, L"Enable Chara Skinning", L"If you really need to get extra performance, you can disable uploading skinning data. (character models will disappear)", true, false } },
	{ CONFIG_BOOLEAN, new PluginConfigBooleanData{ L"use_TexSubImage", L"general", CONFIG_FILE, L"Use glTexSubImage", L"glTexSubImage should offer higher performance, but stuttering has been reported when it is used.\nTry disabling this if you have issues.", true, false } },
	{ CONFIG_BOOLEAN, new PluginConfigBooleanData{ L"force_BGRA_upload", L"general", CONFIG_FILE, L"Force BGRA Texture Uploads", L"BGRA format uploads seem to run faster (on some hardware), but drivers may suggest RGBA instead.\nUsing this forces uploads to use the BGRA format.\n\nDisabling this may decrease or improve performance.", true, false } },
	{ CONFIG_BOOLEAN, new PluginConfigBooleanData{ L"shader_delta_patches", L"general", CONFIG_FILE, L"Apply shader vcdiff patches", L"Novidia can automatically apply pre-generated delta patch files to shaders.\nLeave this enabled for current and future versions of AMDPack.", true, false } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"Novidia";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Novidia by somewhatlurker\n\nPerforms some model skinning transformations in an alternate way to enable functionality on non-Nvidia hardware.\nAlso fixes crashing on non-Nvidia hardware.";
}

extern "C" __declspec(dllexport) PluginConfigArray GetPluginOptions(void)
{
	return PluginConfigArray{ _countof(config), config };
}