#pragma once

#include <windows.h>
#include <iostream>
#include <detours.h>

HMODULE ogl32 = GetModuleHandle(L"opengl32.dll");

PROC(*wGlGetProcAddress_forhook)(LPCSTR) = (PROC(*)(LPCSTR))(GetProcAddress(ogl32, "wglGetProcAddress"));


// should probably just use GL.h, but I'm avoiding it to let me call hooked stuff as normal
typedef uint32_t GLenum, GLuint, GLsizei, GLbitfield;
typedef int32_t GLint;
typedef uint64_t GLsizeiptr;
typedef int64_t GLintptr, GLint64;
typedef bool GLboolean;
typedef char GLchar;

#define GL_FALSE 0
#define GL_NO_ERROR 0
#define GL_TEXTURE_1D 0x0de0
#define GL_TEXTURE_2D 0x0de1
#define GL_FLOAT 0x1406
#define GL_RED 0x1903
#define GL_GREEN 0x1904
#define GL_BLUE 0x1905
#define GL_ALPHA	 0x1906
#define GL_RGBA 0x1908
#define GL_NEAREST 0x2600
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_BGRA 0x80e1
#define GL_CLAMP_TO_BORDER 0x812d
#define GL_CLAMP_TO_EDGE 0x812f
#define GL_TEXTURE_IMAGE_FORMAT 0x828f
#define GL_TEXTURE0 0x84c0
#define GL_TEXTURE8 0x84c8
#define GL_BUFFER_SIZE 0x8764
#define GL_RGBA32F 0x8814
#define GL_RGBA16F 0x881a
#define GL_ARRAY_BUFFER 0x8892
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_READ_ONLY 0x88b8
#define GL_STREAM_DRAW 0x88e0
#define GL_STREAM_COPY 0x88e2
#define GL_COMPILE_STATUS 0x8b81
#define GL_LINK_STATUS 0x8b82
#define GL_CURRENT_PROGRAM 0x8b8d
#define GL_VERTEX_PROGRAM_PARAMETER_BUFFER_NV 0x8da2
#define GL_FRAGMENT_PROGRAM_PARAMETER_BUFFER_NV 0x8da4
#define GL_TEXTURE_SWIZZLE_R 0x8e42
#define GL_TEXTURE_SWIZZLE_G 0x8e43
#define GL_TEXTURE_SWIZZLE_B 0x8e44
#define GL_TEXTURE_SWIZZLE_A 0x8e45
#define GL_TEXTURE_SWIZZLE_RGBA 0x8e46
#define GL_SHADER_STORAGE_BUFFER 0x90d2
#define GL_COMPUTE_SHADER 0x91b9
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000


//void(__cdecl** p_glActiveTexture)(GLenum texture) = (void(__cdecl**)(GLenum texture))0x1411a3db8;
void(__cdecl* glGenTextures)(GLsizei n, GLuint* textures) = *(void(__cdecl**)(GLsizei, GLuint*))0x140965ad8;
void(__cdecl* glBindTexture)(GLenum target, GLuint texture) = *(void(__cdecl**)(GLenum, GLuint))0x140965bf8;
void(__cdecl* glTexParameteri)(GLenum target, GLenum pname, GLint param) = *(void(__cdecl**)(GLenum, GLenum, GLint))0x140965a18;
void(__cdecl* glTexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* data) = *(void(__cdecl**)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const void*))0x140965b58;
void(__cdecl* glTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels) = (void(__cdecl*)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void*))GetProcAddress(ogl32, "glTexSubImage1D");

// these are all imported using wglGetProcAddress and can't be initialised until after the OpenGL context is created
void(__cdecl* glActiveTexture)(GLenum texture);
void(__cdecl* glTexStorage1D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
void(__cdecl* glGetInternalformativ)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params);
void(__cdecl* glBindBuffer)(GLenum target, GLuint buffer);
void(__cdecl* glBindBufferARB)(GLenum target, GLuint buffer);
void(__cdecl* glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
void(__cdecl* glBufferDataARB)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);

// just used for a nicer time getting addresses and hooking
struct {
	void* ptr;
	const char* name;
	bool dynamic;
} glFuncs[] = {
	{&glGenTextures, "glGenTextures", false},
	{&glBindTexture, "glBindTexture", false},
	{&glTexParameteri, "glTexParameteri", false},
	{&glTexImage1D, "glTexImage1D", false},
	{&glTexSubImage1D, "glTexSubImage1D", false},
	{&glActiveTexture, "glActiveTexture", true},
	{&glTexStorage1D, "glTexStorage1D", true},
	{&glGetInternalformativ, "glGetInternalformativ", true},
	{&glBindBuffer, "glBindBuffer", true},
	{&glBindBufferARB, "glBindBufferARB", true},
	{&glBufferData, "glBufferData", true},
	{&glBufferDataARB, "glBufferDataARB", true},
};

bool loadGlAddresses()
{
	bool ret = true;
	for (auto fn : glFuncs)
	{
		if (fn.dynamic)
		{
			*(PROC*)(fn.ptr) = wGlGetProcAddress_forhook(fn.name);
			if (*(PROC*)(fn.ptr) == nullptr)
			{
				printf("[Novidia] Failed to load address of %s\n", fn.name);
				ret = false;
				MessageBoxA(NULL, fn.name, NULL, NULL);
			}
		}
	}
	return ret;
}

const char* getGlFuncName(void* func)
{
	const char* name = NULL;
	for (auto fn : glFuncs)
	{
		if (func == *(PROC*)(fn.ptr))
			name = fn.name;
	}

	return name;
}

bool hookGlFunc(PROC* target, void* hook)
{
	const char* name = getGlFuncName(*target);

	if (name == nullptr)
	{
		printf("[Novidia] Invalid hook target at %p\n", *target);
		return false;
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	printf("[Novidia] Hooking %s\n", name);
	DetourAttach(&(PVOID&)*target, hook);
	DetourTransactionCommit();

	return true;
}