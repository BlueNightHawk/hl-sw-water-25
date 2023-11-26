#include "hud.h"
#include "cl_util.h"
#include "funchook.h"

#define pFunchook reinterpret_cast<funchook*>(pHook)

static bool CheckError(int code)
{
	if (code == FUNCHOOK_ERROR_SUCCESS)
		return true;

	Sys_Error("Funchook error code: %i", code);

	return false;
}

bool CFunchook::Init()
{
	pHook = funchook_create();

	assert(nullptr != pHook);

	return pHook != nullptr;
}

int CFunchook::Install()
{
	int code = funchook_install(pFunchook, 0);
	
	CheckError(code);

	return code;
}

int CFunchook::Prepare(void** target, void* hook)
{
	int code = funchook_prepare(pFunchook, target, hook);

	CheckError(code);

	return code;
}

int CFunchook::Uninstall()
{
	int code = funchook_uninstall(pFunchook, 0);

	CheckError(code);

	return code;
}

int CFunchook::Destroy()
{
	auto code = Uninstall();
	CheckError(code);
	if (code != FUNCHOOK_ERROR_SUCCESS)
	{
		return code;
	}

	code = funchook_destroy(pFunchook);
	CheckError(code);
	return code;
}

CFunchook* CFunchookMgr::CreateHook()
{
	CFunchook hook{};
	if (hook.Init())
	{
		hooks.emplace_back(hook);
		return &hooks.back();
	}
	return nullptr;
}

void CFunchookMgr::Shutdown()
{
	for (size_t i = 0; i < hooks.size(); i++)
	{
		hooks.at(i).Destroy();
	}
	hooks.clear();
}