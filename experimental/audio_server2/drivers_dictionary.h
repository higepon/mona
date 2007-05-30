#pragma once

extern "C" struct audio_driver *es1370_get_driver_desc();

struct audio_driver_dictionary_item
{
	char device_name[32];
	struct audio_driver* (*audio_driver_getter)();
};

#define DIC_LENGTH 1

struct audio_driver_dictionary_item audio_driver_dictionary[] =
{
	{"es1370", es1370_get_driver_desc},
};
