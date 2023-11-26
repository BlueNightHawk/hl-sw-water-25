#include "hud.h"
#include "cl_util.h"

cmd_function_s *GetCmdFunction(const char* pszName)
{
	auto pCmd = reinterpret_cast<cmd_function_s*>(gEngfuncs.GetFirstCmdFunctionHandle());

	// WHAT??!?!?
	if (!pCmd)
		return nullptr;

	while (pCmd)
	{
		if (!stricmp(pszName, pCmd->name))
		{
			return pCmd;
		}

		pCmd = pCmd->next;
	}
	return nullptr;
}