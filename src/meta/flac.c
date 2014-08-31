#include "../vgmstream.h"

#ifdef VGM_USE_FLAC

#include <stdio.h>
#include <string.h>
#include "meta.h"
#include "../util.h"
#include <FLAC/stream_decoder.h>


static FLAC__StreamDecoderReadStatus read_func(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
	flac_codec_data * const f_data = client_data;
    size_t bytes_read;
   
	bytes_read = read_streamfile(buffer, f_data->f_streamfile.offset, *bytes, f_data->f_streamfile.streamfile);

	*bytes = bytes_read;

	f_data->f_streamfile.offset += bytes_read;

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

static FLAC__StreamDecoderSeekStatus seek_func(const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset, void *client_data)
{
	flac_codec_data * const f_data = client_data;

	f_data->f_streamfile.offset = absolute_byte_offset;
	
    return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

static FLAC__StreamDecoderTellStatus tell_func(const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset, void *client_data)
{
	flac_codec_data * const f_data = client_data;

	*absolute_byte_offset = f_data->f_streamfile.offset;

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

static FLAC__StreamDecoderLengthStatus length_func(const FLAC__StreamDecoder *decoder, FLAC__uint64 *stream_length, void *client_data)
{
	flac_codec_data * const f_data = client_data;

	*stream_length = f_data->f_streamfile.size;

	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

static FLAC__bool eof_func(const FLAC__StreamDecoder *decoder, void *client_data)
{
	flac_codec_data * const f_data = client_data;

	if (f_data->f_streamfile.size <= f_data->f_streamfile.offset)
		return 1;

	return 0;
}

static FLAC__StreamDecoderWriteStatus write_func(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{
	flac_codec_data * f_data = client_data;
	size_t write_size = frame->header.blocksize * frame->header.channels * (frame->header.bits_per_sample / 8);
	int i, j;

	if (write_size > f_data->f_buffer.max_size)
	{
		free(f_data->f_buffer.buf);
		f_data->f_buffer.buf = (char *)malloc(write_size);
	}

	for (i = 0; i < frame->header.blocksize; i++)
	{
		for(j = 0; j < frame->header.channels; j++)
			f_data->f_buffer.buf[i*frame->header.channels + j] = (FLAC__int16)buffer[j][i];
	}

	f_data->f_buffer.offset = 0;
	f_data->f_buffer.remaining_sample = frame->header.blocksize;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void meta_func(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		flac_codec_data * f_data = client_data;

		f_data->total_samples = metadata->data.stream_info.total_samples;
		f_data->channels = metadata->data.stream_info.channels;
		f_data->bps = metadata->data.stream_info.bits_per_sample;
		f_data->sample_rate = metadata->data.stream_info.sample_rate;
		f_data->blocksize = metadata->data.stream_info.max_blocksize;
	}
}

static void error_func(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
}

/* Ogg Vorbis, by way of libvorbisfile */

VGMSTREAM * init_vgmstream_flac(STREAMFILE *streamFile) {
    char filename[PATH_LIMIT];

	FLAC__StreamDecoder *decoder = NULL;
	VGMSTREAM * vgmstream = NULL;
	flac_codec_data *data;

    /* check extension, case insensitive */
    streamFile->get_name(streamFile,filename,sizeof(filename));
	if (strcasecmp("flac", filename_extension(filename))) goto fail;

	if ((decoder = FLAC__stream_decoder_new()) == 0) goto fail;

	data = (flac_codec_data *)calloc(1, sizeof(flac_codec_data));
	if (!data) goto fail;

	data->decoder = decoder;
	data->f_streamfile.streamfile = streamFile->open(streamFile, filename, STREAMFILE_DEFAULT_BUFFER_SIZE);
	if (!data->f_streamfile.streamfile) goto fail;
	data->f_streamfile.offset = 0;
	data->f_streamfile.size = get_streamfile_size(data->f_streamfile.streamfile);

	FLAC__stream_decoder_set_md5_checking(decoder, true);

	if (FLAC__stream_decoder_init_stream(decoder, read_func, seek_func, tell_func, length_func, eof_func, write_func, meta_func, error_func, data) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
		goto fail;

	// メタデータまで読み込み
	if(!FLAC__stream_decoder_process_until_end_of_metadata(decoder))
		goto fail;

	// TODO: 他のbpsのサポート(変換を実装してデコーダーを16bit以外もサポートできるようにしないといけない)
	if (data->bps != 16) goto fail;

	/* build the VGMSTREAM */
	vgmstream = allocate_vgmstream(data->channels, 0);
	if (!vgmstream) goto fail;

	/* store our fun extra datas */
	vgmstream->codec_data = data;

	/* fill in the vital statistics */
	vgmstream->channels = data->channels;
	vgmstream->sample_rate = data->sample_rate;

	/* let's play the whole file */
	vgmstream->num_samples = data->total_samples;

	vgmstream->coding_type = coding_flac;
	vgmstream->layout_type = layout_flac;
	vgmstream->meta_type = meta_flac;
	
	return vgmstream;
	/* clean up anything we may have opened */
fail:
	if (data) {
		if (data->f_streamfile.streamfile)
			close_streamfile(data->f_streamfile.streamfile);
		free(data);
	}
	if (vgmstream) {
		vgmstream->codec_data = NULL;
		close_vgmstream(vgmstream);
	}
	if (decoder) {
		FLAC__stream_decoder_finish(decoder);
		FLAC__stream_decoder_delete(decoder);
	}
	return NULL;
}

#endif
