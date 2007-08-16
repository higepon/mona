#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

enum AudioResult
{
	OK = 0,
	NG,
	UnSupportedFormat,
	DeviceIsNotFound,
};

typedef void* handle_t;
typedef int error_t;
typedef int codec_command_t;
typedef error_t (*audio_render_callback_t)(void*, void*, size_t, size_t*);
typedef error_t (*audio_stopped_callback_t)(void*);

struct audio_data_format
{
	int sample_rate;
	int bits;
	int channels;
};

struct audio_driver
{
	handle_t (*driver_new)();
	void     (*driver_delete)(handle_t);
	error_t	 (*driver_codec_command)(handle_t, codec_command_t, ...);
	error_t  (*driver_start)(handle_t);
	error_t  (*driver_stop)(handle_t);
	error_t (*driver_set_render_callback)(handle_t,audio_render_callback_t,void*);
	error_t (*driver_set_stopped_callback)(handle_t,audio_stopped_callback_t,void*);
	error_t (*driver_set_format)(handle_t, const struct audio_data_format*);
	error_t (*driver_get_format)(handle_t, struct audio_data_format*);
	size_t  (*driver_get_block_size)(handle_t);
	size_t  (*driver_write_block)(handle_t, void *);
	void *variables;
};

struct audio_driver *audio_driver_factory(const char *device_name);

#ifdef __cplusplus
}
#endif


