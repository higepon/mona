#pragma once
#include <monalibc/stdint.h>

typedef int32_t ch_t;
struct driver_desc
{
	int32_t driver_type;
	int32_t driver_id;
	int32_t (*init_driver)(int16_t device_id);
	int32_t (*init_device)();
	int32_t (*stop_driver)();
	int32_t (*find_device)();
	int32_t (*maximum_channels)();
	int32_t (*using_channels)();
	ch_t (*create_channel)(); /* if returning value is -1, it is failed. */
	int32_t (*prepare_channel)(ch_t ch, int32_t rate, int32_t bits, int32_t isStereo);
	int32_t (*set_buffer)(ch_t ch, void* buf, size_t size);
	int32_t (*start_channel)(ch_t ch, int32_t loop);
	int32_t (*stop_channel)(ch_t ch);
	int32_t (*destroy_channel)(ch_t ch);
	int32_t (*emit_interrupted)();
};

