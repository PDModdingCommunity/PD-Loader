#pragma once

#include "glStuff.h"
#include <vector>

// use this as a hook to perform initialisation
void(__cdecl* glutSetCursor)(int cursor) = *(void(__cdecl**)(int))0x140966068;
void(__cdecl* uploadModelTransformBuf)(DWORD* a1, int a2) = (void(__cdecl*)(DWORD * a1, int a2))0x140444fa0;

bool& performTransform = *(bool*)0x14cc57bb0;

// vars that will contain the requiered info about GL buffers to perform the transform
GLuint buf_tex;


// check if a buffer target is okay to allow on non-Nvidia platforms
// (used to prevent crashes in glBindBuffer and glBufferData)
// only parameter buffer NV targets cause issues
bool checkBufferTargetOk(GLenum target)
{
	return !(target == GL_VERTEX_PROGRAM_PARAMETER_BUFFER_NV || target == GL_FRAGMENT_PROGRAM_PARAMETER_BUFFER_NV);
}


int __stdcall stub() { return 1; }

bool disable_amd_check;
bool enable_chara_skinning;
bool use_TexSubImage;
bool force_BGRA_upload;
GLint tex_upload_format;

struct MsString {
	union {
		char* string_ptr;
		char string_buf[16];
	};
	uint64_t len;
	uint64_t bufsize;

	char* GetCharBuf()
	{
		if (bufsize > 0xf && string_ptr != nullptr)
			return string_ptr;
		else
			return string_buf;
	};

	void SetCharBuf(char* newcontent)
	{
		len = strlen(newcontent);
		bufsize = len;
		if (len > 0xf)
		{
			string_ptr = _strdup(newcontent);
		}
		else
		{
			strcpy_s(string_buf, newcontent);
		}
	}
};

std::string shader_farc_path = "";
void* shader_farc_data = NULL;
int64_t shader_farc_data_size;
FILE* shader_file_handle = NULL;
bool enable_shader_deltas;

int64_t(*divaGetFileSize)(MsString* path) = (int64_t(*)(MsString* path))0x1400abb20;
FILE* (*divaFsopen)(const char* path, const char* mode, int shflag) = (FILE* (*)(const char* path, const char* mode, int shflag))0x14085a17c;
int64_t(*divaFread)(void* dst, int64_t size, int64_t count, FILE* file) = (int64_t(*)(void* dst, int64_t size, int64_t count, FILE* file))0x14085a6c4;
int(*divaFclose)(FILE* file) = (int(*)(FILE* file))0x140846e9c; // not hooked, just so it can be called

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

std::wstring CONFIG_FILE_STRING = DirPath() + L"\\plugins\\Novidia.ini";
LPCWSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();


void loadConfig()
{
	disable_amd_check = GetPrivateProfileIntW(L"general", L"disable_amd_check", 0, CONFIG_FILE) > 0 ? true : false;
	enable_chara_skinning = GetPrivateProfileIntW(L"general", L"enable_chara_skinning", 1, CONFIG_FILE) > 0 ? true : false;
	use_TexSubImage = GetPrivateProfileIntW(L"general", L"use_TexSubImage", 1, CONFIG_FILE) > 0 ? true : false;
	force_BGRA_upload = GetPrivateProfileIntW(L"general", L"force_BGRA_upload", 1, CONFIG_FILE) > 0 ? true : false;
	enable_shader_deltas = GetPrivateProfileIntW(L"general", L"shader_delta_patches", 1, CONFIG_FILE) > 0 ? true : false;
	
	if (GetPrivateProfileIntW(L"general", L"shader_amd_farc", 0, CONFIG_FILE) > 0)
	{
		MessageBoxA(NULL, "Attention: Use of shader_amd.farc has been discontinued and will no longer work. MAMD mdata is not required and can be deleted.\n\nIf you use modded shaders, amd-tools has been updated so it can generate new patches. Please check the readme for more information.", "Novidia", 0);
		WritePrivateProfileStringW(L"general", L"shader_amd_farc", L"0", CONFIG_FILE);
	}
}


void InjectCode(void* address, const std::vector<uint8_t> data)
{
	const size_t byteCount = data.size() * sizeof(uint8_t);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(address, data.data(), byteCount);
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}