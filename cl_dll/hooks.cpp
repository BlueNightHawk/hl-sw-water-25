#include "hooks.h"
#include "sigscanner.h" 
#include "SDL.h"

CFunchook* HwHooks;

void InitWaterHook()
{
	g_HwScan.FindFunction((DWORD_PTR*)&pEmitWaterPolys, patterns::EmitWaterPolys);

	g_HwScan.FindFunction((DWORD_PTR*)&pgl_texturemode_hook_callback, patterns::gl_texturemode_hook_callback);

	pglFogf = decltype(pglFogf)(SDL_GL_GetProcAddress("glFogf"));

	HwHooks->Prepare((void**)&pEmitWaterPolys, HOOKED_EmitWaterPolys);
	HwHooks->Prepare((void**)&pglFogf, HOOKED_glFogf);
	HwHooks->Prepare((void**)&pgl_texturemode_hook_callback, HOOKED_gl_texturemode_hook_callback);
}

void InitHooks()
{
	HwHooks = g_HookMgr.CreateHook();

	auto result = g_HwScan.GetModuleInfo("hw.dll");

	if (!result)
		Sys_Error("Could not get hw.dll module info!");

	InitWaterHook();

	HwHooks->Install();
}

void ShutdownHooks()
{
	g_HookMgr.Shutdown();
}