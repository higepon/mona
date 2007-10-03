#pragma once

extern "C" struct audio_driver *es1370_get_driver_desc();

struct audio_driver_dictionary_item
{
	char *device_name;
	struct audio_driver* (*audio_driver_getter)();
} audio_driver_dictionary[] = {
	{"es1370", es1370_get_driver_desc},
};

#define DIC_LENGTH (sizeof(audio_driver_dictionary)/sizeof(audio_driver_dictionary_item))
