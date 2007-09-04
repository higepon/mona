#ifndef _SERVER_AUDIO_H_
#define _SERVER_AUDIO_H_

#include <stdint.h>

enum
{
	MSG_AUDIO_CREATE_CHANNEL = 0x506211e, /* AUD:CCH */
	MSG_AUDIO_DESTROY_CHANNEL= 0x506311e, /* AUD:DCH */
	MSG_AUDIO_SET_FORMAT     = 0x50722b2, /* AUD:SFM */
	MSG_AUDIO_GET_FORMAT     = 0x50662b2, /* AUD:GFM */
	MSG_AUDIO_SET_STREAM     = 0x507294e, /* AUD:SST */
	MSG_AUDIO_GET_STREAM     = 0x506694e, /* AUD:GST */
	MSG_AUDIO_SET_BLOCKSIZE  = 0x50720ca, /* AUD:SBS */
	MSG_AUDIO_GET_BLOCKSIZE  = 0x50660ca, /* AUD:GBS */
	MSG_AUDIO_START          = 0x50729ce, /* AUD:STT */
	MSG_AUDIO_STOP           = 0x50729be, /* AUD:STP */
	MSG_AUDIO_SET_VOLUME     = 0x5072aba, /* AUD:SVO */
	MSG_AUDIO_GET_VOLUME     = 0x5066aba, /* AUD:GVO */
	MSG_AUDIO_GET_CHANNELS_LIST = 0x506612e, /* AUD:GCL */

	AUDIO_INPUT = 1,
	AUDIO_OUTPUT= 2,


	MSG_AUDIO_NEW_CHANNEL	 = 0x506d11e, /* AUD:NCH */
#if 0
	MSG_AUDIO_DELETE_CHANNEL = 0x506311e, /* AUD:DCH */
	MSG_AUDIO_START          = 0x5072982, /* AUD:STA */
	MSG_AUDIO_STOP           = 0x50729be, /* AUD:STP */
	MSG_AUDIO_SET_FORMAT     = 0x506564e, /* AUD:FMT */
	MSG_AUDIO_SERVER_VERSION = 0x5075246, /* AUD:VER */
	MSG_AUDIO_KILL           = 0x506a42e, /* AUD:KIL */
	MSG_AUDIO_RESET          = 0x507194e, /* AUD:RST */
#endif
};

struct audio_data_format
{
	uint32_t format;
	uint32_t channels;
	uint32_t bits;
	uint32_t rate;
};

struct audio_channel_description
{
	uint32_t id;
	uint32_t owner;
	uint16_t volume;
	uint16_t state;
	struct audio_data_format format;
};

/*
 * First return number are stored msg.arg2.
 * I can't recommend amateurs to use these commands.
 */

/* Function: MSG_AUDIO_CREATE_CHANNEL
 * To create a new channel.
 * Params:
 * 	arg1(direction): A type of the channel. It's described by the macros.
 * Returns: Channel ID.
 */

/* Function: MSG_AUDIO_DESTROY_CHANNEL
 * To destroy a channel.
 * Params:
 * 	arg1(channel): Channel ID.
 * Retruns: None
 */

/* Function: MSG_AUDIO_SET_FORMAT
 * To set a format.
 * Params:
 * 	arg1(channel): Channel ID.
 *	str(format): A format.
 * Retruns: if it isn't MONA_SUCESS, it was failure.
 */

/* Function: MSG_AUDIO_GET_FORMAT
 * To get a format.
 * Params:
 * 	arg1(channel): Channel ID.
 * Retruns: arg2: if it isn't MONA_SUCESS, it was failure.
 *	    str: format
 */

/* Function: MSG_AUDIO_SET_STREAM
 * To set a stream.
 * Params:
 * 	arg1(channel): Channel ID.
 *	arg2(stream): A stream.
 * Retruns: if it isn't MONA_SUCESS, it was failure.
 */

/* Function: MSG_AUDIO_GET_STREAM
 * To get a stream.
 * Params:
 * 	arg1(channel): Channel ID.
 * Retruns: arg2: if it is 0, it was failure. if it isn't 0, it is stream handle.
 */

/* Function: MSG_AUDIO_START
 * To start playing or captureing.
 * Params:
 *	channel: number of a channel.
 * Returns: None
 */

/* Function: MSG_AUDIO_STOP
 * To stop playing or captureing.
 * Params:
 * 	channel: number of a channel.
 * Returns: None
 */

/* Function: MSG_AUDIO_SERVER_VERSION
 * To get a version number of this server.
 * Prams: None
 * Retruns: version number
 */

/* Function: MSG_AUDIO_KILL
 * To kill this server.
 * Params: None
 * Returns: None
 */

/* Function: MSG_AUDIO_RESET
 * To reest this server.
 * Params: None
 * Returns: None
 */

#endif

