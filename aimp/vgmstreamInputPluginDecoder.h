#pragma once

#include <Helpers/AIMPSDKHelpers.h>
#include <AIMPSDKInput.h>
#include "../src/vgmstream.h"


class vgmstreamInputPluginDecoder : public IUnknownInterfaceImpl < IAIMPInputPluginDecoder >
{
private:
	VGMSTREAM *stream;
	INT64 FileSize;

	INT64 stream_length_samples;
	INT64 seek_needed_samples;
	INT64 seek_needed_samples_position;
	INT64 now_position_samples;

	INT64 fade_samples;

private:
	INT64 ByteToSample(const INT64 byte);
	INT64 SampleToByte(const INT64 sample);

	INT64 SampleAdjustLoop(const INT64 sample);

	static bool CharToWchar(const char *str, const PWCHAR wstr, const DWORD size);

public:
	vgmstreamInputPluginDecoder(VGMSTREAM *stream, const INT64 FileSize);
	~vgmstreamInputPluginDecoder();

	// Read Info about stream, ABitDepth: See AIMP_INPUT_BITDEPTH_XXX flags
	virtual BOOL WINAPI DecoderGetInfo(int *ASampleRate, int *AChannels, int *ABitDepth);
	// Uncompressed stream position in Bytes
	virtual INT64 WINAPI DecoderGetPosition();
	// Uncompressed stream size in Bytes
	virtual INT64 WINAPI DecoderGetSize();
	// Read Info about the file
	virtual BOOL WINAPI DecoderGetTags(TAIMPFileInfo *AFileInfo);
	// Size of Buffer in Bytes
	virtual int  WINAPI DecoderRead(unsigned char *Buffer, int Size);
	// Uncompressed stream position in Bytes
	virtual BOOL WINAPI DecoderSetPosition(const INT64 AValue);
	// Is DecoderSetPosition supports?
	virtual BOOL WINAPI DecoderIsSeekable();
	// Is speed, tempo and etc supports?
	// RealTime streams doesn't supports speed control
	virtual BOOL WINAPI DecoderIsRealTimeStream();
	// Return format type for current file, (MP3, OGG, AAC+, FLAC and etc)
	virtual PWCHAR WINAPI DecoderGetFormatType();
};
