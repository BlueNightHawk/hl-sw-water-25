#include "hooks.h"
#include "sigscanner.h" 
#include "SDL.h"

CFunchook* HwHooks;

#ifdef WIN32
#define LIBNAME "hw.dll"
#else
#define LIBNAME "hw.so"
#endif

void InitWaterHook()
{
	g_HwScan.FindFunction((DWORD*)&pEmitWaterPolys, patterns::EmitWaterPolys);

	g_HwScan.FindFunction((DWORD*)&pgl_texturemode_hook_callback, patterns::gl_texturemode_hook_callback);

	pglFogf = decltype(pglFogf)(SDL_GL_GetProcAddress("glFogf"));

	HwHooks->Prepare((void**)&pEmitWaterPolys, HOOKED_EmitWaterPolys);
	HwHooks->Prepare((void**)&pglFogf, HOOKED_glFogf);
	HwHooks->Prepare((void**)&pgl_texturemode_hook_callback, HOOKED_gl_texturemode_hook_callback);
}

void InitHooks()
{
	HwHooks = g_HookMgr.CreateHook();

	auto result = g_HwScan.GetModuleInfo(LIBNAME);

	if (!result)
		Sys_Error("Could not get " LIBNAME " module info!");

	InitWaterHook();

	HwHooks->Install();
}

void ShutdownHooks()
{
	g_HookMgr.Shutdown();
}