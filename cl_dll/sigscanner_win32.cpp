#include "hooks.h"
#include "sigscanner.h"

// For getting information about the executing module
bool SigScan::GetModuleInfo(char* szModule)
{
	MODULEINFO modinfo = {nullptr};
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == nullptr)
	{
		return false;
	}
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));

	base = reinterpret_cast<DWORD_PTR>(modinfo.lpBaseOfDll);
	size = (DWORD)modinfo.SizeOfImage;

	return true;
}

void SigScan::FindFunction(DWORD* result, Pattern pattern)
{
	const char* version = gEngfuncs.pfnGetCvarString("sv_version");

	if (!strstr(version, pattern.build))
	{
		Sys_Error("Engine build version mismatch for pattern : %s!\n"
				  "Update to newer version of the mod if available.",
			pattern.name);
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