#pragma once
#include <monalibc/stdint.h>

typedef int32_t ch_t;
struct driver_desc
{
	int32_t (*init_drive)(int16_t device_id);
	int32_t (*init_device)();
	int32_t (*stop_driver)();
	int32_t (*find_device)();
	int32_t (*maximum_channels)();
	ch_t (*create_channel)();
	int32_t (*prepare_channel)(ch_t ch, int32_t rate, int32_t isStereo);
	int32_t (*play_channel)(ch_t ch, int32_t loop);
	int32_t (*stop_channel)(ch_t ch);
	int32_t (*destroy_channel)(ch_t ch);
};

