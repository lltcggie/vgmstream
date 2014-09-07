#include "vgmstreamInputPlugin.h"
#include "vgmstreamInputPluginDecoder.h"
#include <AIMPSDKAddons.h>
#include <Windows.h>


const WCHAR *Extensions = {
	L"*.2dx9;"
	L"*.2pfs;"

	L"*.aax;"
	L"*.acm;"
	L"*.adm;"
	L"*.adp;"
	L"*.adpcm;"
	L"*.ads;"
	L"*.adx;"
	L"*.afc;"
	L"*.agsc;"
	L"*.ahx;"
	L"*.aifc;"
	L"*.aix;"
	L"*.amts;"
	L"*.as4;"
	L"*.asd;"
	L"*.asf;"
	L"*.asr;"
	L"*.ass;"
	L"*.ast;"
	L"*.aud;"
	L"*.aus;"

	L"*.baka;"
	L"*.baf;"
	L"*.bar;"
	L"*.bcstm;"
	L"*.bcwav;"
	L"*.bfwav;"
	L"*.bg00;"
	L"*.bgw;"
	L"*.bh2pcm;"
	L"*.bmdx;"
	L"*.bms;"
	L"*.bns;"
	L"*.bnsf;"
	L"*.bo2;"
	L"*.brstm;"
	L"*.brstmspm;"
	L"*.bvg;"

	L"*.caf;"
	L"*.capdsp;"
	L"*.cbd2;"
	L"*.ccc;"
	L"*.cfn;"
	L"*.ckd;"
	L"*.cnk;"
	L"*.cps;"

	L"*.dcs;"
	L"*.de2;"
	L"*.ddsp;"
	L"*.dmsg;"
	L"*.dsp;"
	L"*.dspw;"
	L"*.dtk;"
	L"*.dvi;"
	L"*.dxh;"

	L"*.eam;"
	L"*.emff;"
	L"*.enth;"

	L"*.fag;"
	L"*.filp;"
	L"*.fsb;"

	L"*.gbts;"
	L"*.gca;"
	L"*.gcm;"
	L"*.gcub;"
	L"*.gcw;"
	L"*.genh;"
	L"*.gms;"
	L"*.gsb;"

	L"*.hgc1;"
	L"*.his;"
	L"*.hlwav;"
	L"*.hps;"
	L"*.hsf;"
	L"*.hwas;"

	L"*.iab;"
	L"*.idsp;"
	L"*.idvi;"
	L"*.ikm;"
	L"*.ild;"
	L"*.int;"
	L"*.isd;"
	L"*.ivaud;"
	L"*.ivag;"
	L"*.ivb;"

	L"*.joe;"
	L"*.jstm;"

	L"*.kces;"
	L"*.kcey;"
	L"*.khv;"
	L"*.klbs;"
	L"*.kovs;"
	L"*.kraw;"

	L"*.leg;"
	L"*.logg;"
	L"*.lpcm;"
	L"*.lps;"
	L"*.lsf;"
	L"*.lwav;"

	L"*.matx;"
	L"*.mcg;"
	L"*.mi4;"
	L"*.mib;"
	L"*.mic;"
	L"*.mihb;"
	L"*.mpdsp;"
	L"*.mpds;"
	L"*.msa;"
	L"*.msf;"
	L"*.mss;"
	L"*.msvp;"
	L"*.mtaf;"
	L"*.mus;"
	L"*.musc;"
	L"*.musx;"
	L"*.mwv;"
	L"*.mxst;"
	L"*.myspd;"

	L"*.ndp;"
	L"*.ngca;"
	L"*.npsf;"
	L"*.nwa;"

	L"*.omu;"
	L"*.otm;"

	L"*.p2bt;"
	L"*.p3d;"
	L"*.past;"
	L"*.pcm;"
	L"*.pdt;"
	L"*.pnb;"
	L"*.pos;"
	L"*.ps2stm;"
	L"*.psh;"
	L"*.psnd;"
	L"*.psw;"

	L"*.ras;"
	L"*.raw;"
	L"*.rkv;"
	L"*.rnd;"
	L"*.rrds;"
	L"*.rsd;"
	L"*.rsf;"
	L"*.rstm;"
	L"*.rvws;"
	L"*.rwar;"
	L"*.rwav;"
	L"*.rws;"
	L"*.rwsd;"
	L"*.rwx;"
	L"*.rxw;"

	L"*.s14;"
	L"*.sab;"
	L"*.sad;"
	L"*.sap;"
	L"*.sc;"
	L"*.scd;"
	L"*.sck;"
	L"*.sd9;"
	L"*.sdt;"
	L"*.seg;"
	L"*.sf0;"
	L"*.sfl;"
	L"*.sfs;"
	L"*.sfx;"
	L"*.sgb;"
	L"*.sgd;"
	L"*.sl3;"
	L"*.sli;"
	L"*.smp;"
	L"*.smpl;"
	L"*.snd;"
	L"*.snds;"
	L"*.sng;"
	L"*.sns;"
	L"*.spd;"
	L"*.spm;"
	L"*.sps;"
	L"*.spsd;"
	L"*.spw;"
	L"*.ss2;"
	L"*.ss3;"
	L"*.ss7;"
	L"*.ssm;"
	L"*.sss;"
	L"*.ster;"
	L"*.stma;"
	L"*.str;"
	L"*.strm;"
	L"*.sts;"
	L"*.stx;"
	L"*.svag;"
	L"*.svs;"
	L"*.swav;"
	L"*.swd;"

	L"*.tec;"
	L"*.thp;"
	L"*.tk1;"
	L"*.tk5;"
	L"*.tra;"
	L"*.tun;"
	L"*.tydsp;"

	L"*.um3;"

	L"*.vag;"
	L"*.vas;"
	L"*.vawx;"
	L"*.vb;"
	L"*.vbk;"
	L"*.vgs;"
	L"*.vgv;"
	L"*.vig;"
	L"*.vms;"
	L"*.voi;"
	L"*.vpk;"
	L"*.vs;"
	L"*.vsf;"

	L"*.waa;"
	L"*.wac;"
	L"*.wad;"
	L"*.wam;"
	L"*.wavm;"
	L"*.wb;"
	L"*.wii;"
	L"*.wmus;"
	L"*.wp2;"
	L"*.wpd;"
	L"*.wsd;"
	L"*.wsi;"
	L"*.wvs;"

	L"*.xa;"
	L"*.xa2;"
	L"*.xa30;"
	L"*.xau;"
	L"*.xmu;"
	L"*.xnb;"
	L"*.xsf;"
	L"*.xss;"
	L"*.xvag;"
	L"*.xvas;"
	L"*.xwav;"
	L"*.xwb;"

	L"*.ydsp;"
	L"*.ymf;"

	L"*.zsd;"
	L"*.zwdsp;"
};

BOOL WINAPI vgmstreamInputPlugin::Initialize()
{
	return TRUE;
}

BOOL WINAPI vgmstreamInputPlugin::Finalize()
{
	return true;
}

BOOL WINAPI vgmstreamInputPlugin::CreateDecoder(PWCHAR AFileName, IAIMPInputPluginDecoder **ADecoder)
{
	char path[1024];
	int ret = WideCharToMultiByte(CP_ACP, 0, AFileName, -1, path, sizeof(path), nullptr, nullptr);
	path[sizeof(path) - 1] = '\0';

	VGMSTREAM *vgmstream = init_vgmstream(path);

	if (!vgmstream)
		return FALSE;

	*ADecoder = new vgmstreamInputPluginDecoder(vgmstream, GetFileSize(AFileName));

	return TRUE;
}

BOOL WINAPI vgmstreamInputPlugin::CreateDecoderEx(IAIMPInputStream *AStream, IAIMPInputPluginDecoder **ADecoder)
{
	// TODO: 

	return FALSE;
}

BOOL WINAPI vgmstreamInputPlugin::GetFileInfo(PWCHAR AFileName, TAIMPFileInfo *AFileInfo)
{
	if (FileInfoIsValid(AFileInfo))
	{
		char path[1024];
		int ret = WideCharToMultiByte(CP_ACP, 0, AFileName, -1, path, sizeof(path), nullptr, nullptr);
		path[sizeof(path) - 1] = '\0';

		VGMSTREAM *vgmstream = init_vgmstream(path);

		if (!vgmstream)
			return FALSE;

		vgmstreamInputPluginDecoder* ptr = new vgmstreamInputPluginDecoder(vgmstream, GetFileSize(AFileName));
		const BOOL bret = ptr->DecoderGetTags(AFileInfo);
		delete ptr;

		return bret;
	}

	return FALSE;
}

// Return string format: "My Custom Format1|*.fmt1;*.fmt2;|"
PWCHAR WINAPI vgmstreamInputPlugin::GetSupportsFormats()
{
	static WCHAR buf[10000] = L"\0";

	if (buf[0] == '\0')
	{
		wcscpy(buf, L"vgmstream|");
		wcscat(buf, Extensions);
		wcscat(buf, L"|");
	}

	return buf;
}

INT64 vgmstreamInputPlugin::GetFileSize(PWCHAR AFileName)
{
	INT64 ret = 0;

	FILE *fp = _wfopen(AFileName, L"rb");
	if (fp)
	{
		_fseeki64(fp, 0, SEEK_END);
		ret = _ftelli64(fp);
		fclose(fp);
	}

	return ret;
}
