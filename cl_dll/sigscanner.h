#pragma once
#include "PlatformHeaders.h"
#include <string>
#ifdef WIN32
#include <Psapi.h>
#endif
#include "patterns.h"
#include <assert.h>

#ifndef WIN32
typedef unsigned long DWORD;
#endif

class SigScan
{
private:
#ifdef WIN32
	DWORD base;
	DWORD size;
#else
	void* base;
#endif

public:
	bool GetModuleInfo(char* szModule);
	void FindFunction(DWORD* result, Pattern pattern);
};

inline SigScan g_HwScan;