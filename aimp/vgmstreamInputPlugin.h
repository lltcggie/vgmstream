#pragma once

#include <Helpers/AIMPSDKHelpers.h>
#include <AIMPSDKInput.h>


class vgmstreamInputPlugin : public IUnknownInterfaceImpl < IAIMPInputPluginHeader >
{
private:
	HMODULE FPluginInstance;

	static INT64 GetFileSize(PWCHAR AFileName);

public:
	vgmstreamInputPlugin(HMODULE PluginInstance)
	{
		FPluginInstance = PluginInstance;
	}

	// IAIMPAddonPlugin
	virtual PWCHAR WINAPI GetPluginAuthor()
	{
		return L"vgmstream plugin";
	}

	virtual PWCHAR WINAPI GetPluginInfo()
	{
		return L"vgmstream plugin";
	}

	virtual PWCHAR WINAPI GetPluginName()
	{
		return L"vgmstream plugin";
	}

	virtual DWORD  WINAPI GetPluginFlags()
	{
		// return AIMP_INPUT_FLAG_FILE | AIMP_INPUT_FLAG_ISTREAM;
		return AIMP_INPUT_FLAG_FILE;
	}

	virtual BOOL WINAPI Initialize();
	virtual BOOL WINAPI Finalize();

	virtual BOOL WINAPI CreateDecoder(PWCHAR AFileName, IAIMPInputPluginDecoder **ADecoder);
	virtual BOOL WINAPI CreateDecoderEx(IAIMPInputStream *AStream, IAIMPInputPluginDecoder **ADecoder);

	virtual BOOL WINAPI GetFileInfo(PWCHAR AFileName, TAIMPFileInfo *AFileInfo);

	// Return string format: "My Custom Format1|*.fmt1;*.fmt2;|"
	virtual PWCHAR WINAPI GetSupportsFormats();
};
