#pragma once

#include <windows.h>
#include <iostream>
#include <detours.h>


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
#define GL_FLOAT 0x1406
#define GL_RGBA 0x1908
#define GL_NEAREST 0x2600
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_CLAMP_TO_BORDER 0x812d
#define GL_CLAMP_TO_EDGE 0x812f
#define GL_TEXTURE0 0x84c0
#define GL_TEXTURE8 0x84c8
#define GL_BUFFER_SIZE 0x8764
#define GL_RGBA32F 0x8814
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
#define GL_SHADER_STORAGE_BUFFER 0x90d2
#define GL_COMPUTE_SHADER 0x91b9
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000


//void(__cdecl** p_glActiveTexture)(GLenum texture) = (void(__cdecl**)(GLenum texture))0x1411a3db8;
//void(__cdecl* glGenTextures)(GLsizei n, GLuint* textures) = *(void(__cdecl**)(GLsizei n, GLuint * textures))0x140965ad8;
//void(__cdecl* glBindTexture)(GLenum target, GLuint texture) = *(void(__cdecl**)(GLenum target, GLuint texture))0x140965bf8;
//void(__cdecl* glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void* indices) = *(void(__cdecl**)(GLenum mode, GLsizei count, GLenum type, const void* indices))0x140965a48;

// these are all dynamic imports and can't be initialised until after the OpenGL context is created
void(__cdecl* glActiveTexture)(GLenum texture);
void(__cdecl* glGenTextures)(GLsizei n, GLuint* textures);
void(__cdecl* glBindTexture)(GLenum target, GLuint texture);
void(__cdecl* glTexStorage1D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
//void(__cdecl* glTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels);
void(__cdecl* glTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
void(__cdecl* glTexParameteri)(GLenum target, GLenum pname, GLint param);
//void(__cdecl* glDrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
void(__cdecl* glBindBuffer)(GLenum target, GLuint buffer);
void(__cdecl* glBindBufferARB)(GLenum target, GLuint buffer);
void(__cdecl* glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
void(__cdecl* glBufferDataARB)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
void(__cdecl* glFinish)();

// just used for a nicer time getting addresses and hooking
struct {
	void* ptr;
	const char* name;
	bool dynamic;
} glFuncs[] = {
	//{&glDrawElements, "glDrawElements", false},
	{&glActiveTexture, "glActiveTexture", true},
	{&glGenTextures, "glGenTextures", true},
	{&glBindTexture, "glBindTexture", true},
	{&glTexStorage1D, "glTexStorage1D", true},
	//{&glTexImage1D, "glTexImage1D", true},
	{&glTexSubImage1D, "glTexSubImage1D", true},
	{&glTexParameteri, "glTexParameteri", true},
	//{&glDrawRangeElements, "glDrawRangeElements", true},
	{&glBindBuffer, "glBindBuffer", true},
	{&glBindBufferARB, "glBindBufferARB", true},
	{&glBufferData, "glBufferData", true},
	{&glBufferDataARB, "glBufferDataARB", true},
	{&glFinish, "glFinish", true},
};

bool loadGlAddresses()
{
	bool ret = true;
	for (auto fn : glFuncs)
	{
		if (fn.dynamic)
		{
			*(PROC*)(fn.ptr) = wglGetProcAddress(fn.name);
			if (*(PROC*)(fn.ptr) == nullptr)
			{
				printf("[Novidia] Failed to load address of %s\n", fn.name);
				ret = false;
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