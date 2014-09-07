#include <AIMPSDKAddons.h>
#include "vgmstreamInputPlugin.h"

HMODULE PluginInstance = 0;


// TODO: AIMP_QueryInput ‚©‚à
BOOL WINAPI AIMP_QueryInput(IAIMPInputPluginHeader **AHeader)
{
	vgmstreamInputPlugin *Plugin = new vgmstreamInputPlugin(PluginInstance);
	Plugin->AddRef();
	*AHeader = Plugin;
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		PluginInstance = hModule;
		break;
	}
	return TRUE;
}
