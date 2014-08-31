#include "../vgmstream.h"

#ifdef VGM_USE_FLAC
#include <FLAC/stream_decoder.h>
#include "coding.h"
#include "../util.h"

void decode_flac(flac_codec_data * data, sample * outbuf, int32_t samples_to_do, int channels)
{
	int32_t samples_done = 0;
	FLAC__StreamDecoder *decoder = data->decoder;

    do {
		if (data->f_buffer.remaining_sample == 0)
		{
			FLAC__StreamDecoderState state;

			if (!FLAC__stream_decoder_process_single(decoder))
				return;
			state = FLAC__stream_decoder_get_state(decoder);
			if (!(
				state == FLAC__STREAM_DECODER_SEARCH_FOR_METADATA ||
				state == FLAC__STREAM_DECODER_READ_METADATA ||
				state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC ||
				state == FLAC__STREAM_DECODER_READ_FRAME))
				return;
		}

		if (data->f_buffer.remaining_sample > 0)
		{
			size_t sample_num;
			size_t rem = (size_t)(samples_to_do - samples_done);
			if (rem > data->f_buffer.remaining_sample)
				sample_num = data->f_buffer.remaining_sample;
			else
				sample_num = rem;

			memcpy((char *)(outbuf + samples_done*channels), data->f_buffer.buf + data->f_buffer.offset, sizeof(sample)*sample_num*channels);
			data->f_buffer.offset += sample_num*channels;
			data->f_buffer.remaining_sample -= sample_num;

			samples_done += sample_num;
		}
    } while (samples_done < samples_to_do);
}

#endif
