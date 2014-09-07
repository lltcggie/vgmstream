#include "vgmstreamInputPluginDecoder.h"

double fade_seconds = 5.0;
double fade_delay_seconds = 5.0;
double loop_count = 3.0;
//bool ignore_loop = false;
bool loop_forever = false;


vgmstreamInputPluginDecoder::vgmstreamInputPluginDecoder(VGMSTREAM *stream, const INT64 FileSize)
	: stream(stream), FileSize(FileSize), stream_length_samples(-1), seek_needed_samples(-1), seek_needed_samples_position(-1), now_position_samples(0), fade_samples(0)
{
	stream_length_samples = get_vgmstream_play_samples(loop_count, fade_seconds, fade_delay_seconds, stream);
	fade_samples = (INT64)(fade_seconds * stream->sample_rate);
}

vgmstreamInputPluginDecoder::~vgmstreamInputPluginDecoder()
{
	close_vgmstream(stream);
}

// Read Info about stream, ABitDepth: See AIMP_INPUT_BITDEPTH_XXX flags
BOOL WINAPI vgmstreamInputPluginDecoder::DecoderGetInfo(int *ASampleRate, int *AChannels, int *ABitDepth)
{
	*ASampleRate = stream->sample_rate;
	*AChannels = stream->channels;

	// このライブラリは16bit固定
	*ABitDepth = AIMP_INPUT_BITDEPTH_16BIT;

	return TRUE;
}

// Uncompressed stream position in Bytes
INT64 WINAPI vgmstreamInputPluginDecoder::DecoderGetPosition()
{
	return SampleToByte(now_position_samples);
}

// Uncompressed stream size in Bytes
INT64 WINAPI vgmstreamInputPluginDecoder::DecoderGetSize()
{
	return SampleToByte(stream_length_samples);
}

// Read Info about the file
BOOL WINAPI vgmstreamInputPluginDecoder::DecoderGetTags(TAIMPFileInfo *AFileInfo)
{
	if (FileInfoIsValid(AFileInfo))
	{
		AFileInfo->SampleRate = stream->sample_rate;
		AFileInfo->Channels = stream->channels;
		AFileInfo->BitRate = 16; // このライブラリは16bit固定
		AFileInfo->FileSize = FileSize;
		AFileInfo->Duration = stream_length_samples * 1000UL / stream->sample_rate;

		if (strlen(stream->track) > 0)
		{
			const char *to = strchr(stream->track, '/');
			if (!to)
				AFileInfo->TrackNumber = atoi(stream->track);
			else
			{
				char buf[30];
				const int len = (int)(to - stream->track);
				if (len >= 1)
				{
					memcpy(buf, stream->track, len);
					buf[len] = '\0';

					AFileInfo->TrackNumber = atoi(buf);
				}
			}
		}

		if (strlen(stream->artist) > 0)
			CharToWchar(stream->artist, AFileInfo->ArtistBuffer, AFileInfo->ArtistBufferSizeInChars);

		if (strlen(stream->title) > 0)
			CharToWchar(stream->title, AFileInfo->TitleBuffer, AFileInfo->TitleBufferSizeInChars);

		if (strlen(stream->album) > 0)
			CharToWchar(stream->album, AFileInfo->AlbumBuffer, AFileInfo->AlbumBufferSizeInChars);

		if (stream->year > 0)
			_swprintf(AFileInfo->DateBuffer, L"%d", stream->year);

		if (strlen(stream->genre) > 0)
			CharToWchar(stream->genre, AFileInfo->GenreBuffer, AFileInfo->GenreBufferSizeInChars);


		return TRUE;
	}

	return FALSE;
}

// Size of Buffer in Bytes
int  WINAPI vgmstreamInputPluginDecoder::DecoderRead(unsigned char *Buffer, int Size)
{
	// TODO: Loop回数やフェードなど

	const int32_t samples_to_do = (int32_t)ByteToSample(Size);
	const int32_t do_size = (int32_t)SampleToByte(samples_to_do);

	INT64 seek_pos_samples = 0;
	if (seek_needed_samples != -1)
	{
		const INT64 pos = SampleAdjustLoop(seek_needed_samples_position);
		const INT64 to = SampleAdjustLoop(seek_needed_samples);

		seek_pos_samples = to - pos;
	}

	if (seek_pos_samples < 0) // 戻る
	{
		// 0に戻してから進めさせる
		reset_vgmstream(stream);

		seek_pos_samples = seek_needed_samples_position + seek_pos_samples;
	}

	if (seek_pos_samples > 0) // 進む
	{
		const INT64 LoopNum = seek_pos_samples / (INT64)samples_to_do;
		for (INT64 i = 0; i < LoopNum; i++)
			render_vgmstream((sample *)Buffer, samples_to_do, stream);

		const INT64 ModNum = seek_pos_samples % (INT64)samples_to_do;
		if (ModNum > 0)
			render_vgmstream((sample *)Buffer, (int32_t)ModNum, stream);
	}

	if (seek_needed_samples != -1)
	{
		seek_needed_samples = -1;
		seek_needed_samples_position = -1;
	}

	render_vgmstream((sample *)Buffer, samples_to_do, stream);

	if (stream->loop_flag && fade_samples > 0 && !loop_forever)
	{
		INT64 samples_into_fade = now_position_samples - (stream_length_samples - fade_samples);
		if (samples_into_fade + samples_to_do > 0)
		{
			sample *sample_buffer = (sample *)Buffer;

			for (int j = 0; j < samples_to_do; j++, samples_into_fade++)
			{
				if (samples_into_fade > 0)
				{
					const double fadedness = (double)(fade_samples - samples_into_fade) / (double)fade_samples;
					for (int k = 0; k < stream->channels; k++)
						sample_buffer[j*stream->channels + k] = (short)(sample_buffer[j*stream->channels + k] * fadedness);
				}
			}
		}
	}

	now_position_samples += samples_to_do;

	return do_size;
}

// Uncompressed stream position in Bytes
BOOL WINAPI vgmstreamInputPluginDecoder::DecoderSetPosition(const INT64 AValue)
{
	seek_needed_samples = ByteToSample(AValue);
	seek_needed_samples_position = now_position_samples;

	now_position_samples = seek_needed_samples;

	return TRUE;
}

// Is DecoderSetPosition supports?
BOOL WINAPI vgmstreamInputPluginDecoder::DecoderIsSeekable()
{
	return TRUE;
}

// Is speed, tempo and etc supports?
// RealTime streams doesn't supports speed control
BOOL WINAPI vgmstreamInputPluginDecoder::DecoderIsRealTimeStream()
{
	return FALSE;
}

// Return format type for current file, (MP3, OGG, AAC+, FLAC and etc)
PWCHAR WINAPI vgmstreamInputPluginDecoder::DecoderGetFormatType()
{
	// TODO: 

	static WCHAR buf[1000];

	wcscpy(buf, L"vgmstream");

	return buf;
}

INT64 vgmstreamInputPluginDecoder::ByteToSample(const INT64 byte)
{
	return byte / (stream->channels * (16 / 8));
}

INT64 vgmstreamInputPluginDecoder::SampleToByte(const INT64 sample)
{
	return sample * stream->channels * (16 / 8);
}

INT64 vgmstreamInputPluginDecoder::SampleAdjustLoop(const INT64 sample)
{
	INT64 sampleLoop = sample;

	// adjust for correct position within loop
	if (stream->loop_flag && (stream->loop_end_sample - stream->loop_start_sample) && sampleLoop >= stream->loop_end_sample) {
		sampleLoop -= stream->loop_start_sample;
		sampleLoop %= (stream->loop_end_sample - stream->loop_start_sample);
		sampleLoop += stream->loop_start_sample;
	}

	return sampleLoop;
}

bool vgmstreamInputPluginDecoder::CharToWchar(const char *str, const PWCHAR wstr, const DWORD size)
{
	MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, size);
	wstr[size - 1] = L'\0';

	return true;
}
