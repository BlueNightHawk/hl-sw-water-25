#pragma once
#include "PlatformHeaders.h"
#include <string>
#ifdef WIN32
#include <Psapi.h>
#else
#include <unordered_map>
#include <string_view>
#endif
#include "patterns.h"
#include <assert.h>

#ifndef WIN32
typedef unsigned long DWORD;
typedef DWORD u32;
#endif

class SigScan
{
private:
#ifdef WIN32
	DWORD base;
	DWORD size;
#else
	void* base;
	u32 engineAddr = 0;
	std::unordered_map<std::string_view, u32> engineSymbols;

	u32 getEngineSymbol(const char* symbol);
#endif

public:
	bool GetModuleInfo(char* szModule);
	void FindFunction(DWORD* result, Pattern pattern);
};

inline SigScan g_HwScan;