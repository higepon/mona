#ifndef _SERVER_AUDIO_H_
#define _SERVER_AUDIO_H_

enum
{
	MSG_AUDIO_NEW_CHANNEL	 = 0x506d11e, /* AUD:NCH */
	MSG_AUDIO_DELETE_CHANNEL = 0x506311e, /* AUD:DCH */
	MSG_AUDIO_START          = 0x5072982, /* AUD:STA */
	MSG_AUDIO_STOP           = 0x50729be, /* AUD:STP */
	MSG_AUDIO_SET_FORMAT     = 0x506564e, /* AUD:FMT */
	MSG_AUDIO_SERVER_VERSION = 0x5075246, /* AUD:VER */
	MSG_AUDIO_KILL           = 0x506a42e, /* AUD:KIL */
	MSG_AUDIO_RESET          = 0x507194e, /* AUD:RST */
};

/* Function: MSG_AUDIO_NEW_CHANNEL
 * To create a new channel.
 * Params:
 * 	type: A type of the channel. It's described by the enum `ChannelType`.
 * Returns: number of the channel, handle of the stream.
 */

/* Function: MSG_AUDIO_DELETE_CHANNEL
 * To delete a channel.
 * Params:
 * 	channel: number of a channel.
 * Retruns: None
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

/* Function: MSG_AUDIO_SET_FORMAT
 * To set a audio data format.
 * Params:
 * 	channels: number of audio channels
 * 	bits: bits per sample
 * 	rate: samples per second
 * 	byteorder: Byte ordering in sample. But now, It's a little endian only.
 * 	           Therefore you don't have to set this param.
 * Returns: Error Status
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

