#pragma once

#include <vector>

class CFunchook
{
public:
	bool Init();
	int Destroy();

	int Prepare(void** target, void* hook);
	int Install();
	int Uninstall();

private:
	void* pHook = nullptr;
};

class CFunchookMgr
{
public:
	CFunchook* CreateHook();

	void Shutdown();

private:
	std::vector<CFunchook> hooks;
};

inline CFunchookMgr g_HookMgr;