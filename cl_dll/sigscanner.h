#pragma once
#include "PlatformHeaders.h"
#include <string>
#ifdef WIN32
#include <Psapi.h>
#endif
#include "patterns.h"
#include <assert.h>

class SigScan
{
private:
#ifdef WIN32
	DWORD_PTR base;
	DWORD size;
#else
	void* base;
#endif
public:
	// For getting information about the executing module
	bool GetModuleInfo(char* szModule)
	{
#ifdef WIN32
		MODULEINFO modinfo = {nullptr};
		HMODULE hModule = GetModuleHandle(szModule);
		if (hModule == nullptr)
		{
			return false;
		}
		GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));

		base = reinterpret_cast<DWORD_PTR>(modinfo.lpBaseOfDll);
		size = (DWORD)modinfo.SizeOfImage;
#else
		base = (void*)dlopen(szModule, RTLD_GLOBAL);
		if (base == nullptr)
		{
			return false;
		}
#endif

		return true;
	}
#ifdef WIN32
	void FindFunction(DWORD_PTR *result, Pattern pattern)
	{
		const char* version = gEngfuncs.pfnGetCvarString("sv_version");

		if (!strstr(version, pattern.build))
		{
			Sys_Error("Engine build version mismatch for pattern : %s!\n"
				"Update to newer version of the mod if available.", pattern.name);
			return;
		}

		auto patternLength = strlen(pattern.mask);

		for (DWORD i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD j = 0; j < patternLength; j++)
			{
				found &= pattern.mask[j] == '?' || pattern.pattern[j] == *(char*)(base + i + j);
			}
			if (found)
			{
				*result = base + i;
				return;
			}
		}
		Sys_Error("Could not find function : %s", pattern.name);
	}
#else
	bool FindFunction(DWORD_PTR* result, Pattern pattern)
	{
		result = (void*)dlsym(base, pattern.name);

		return result != nullptr;
	}
#endif
};

inline SigScan g_HwScan;