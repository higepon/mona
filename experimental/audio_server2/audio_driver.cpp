#include "audio_driver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "drivers_dictionary.h"

int audio_dic_item_compar(const void* a, const void *b)
{
	puts(__func__);
	struct audio_driver_dictionary_item *item1;
	struct audio_driver_dictionary_item *item2;
	item1 = (struct audio_driver_dictionary_item*)a;
	item2 = (struct audio_driver_dictionary_item*)b;

	puts("strncmp");
	printf("item1 = %x, item2 = %x\n", item1, item2);
	printf("item1.name = %x, item2.name = %x\n", item1->device_name, item2->device_name);
	return strcmp(item1->device_name, item2->device_name);
}

extern "C" struct audio_driver *es1370_get_driver_desc();

extern "C" struct audio_driver *audio_driver_factory(const char *device_name)
{
/*
	void *p;
	struct audio_driver_dictionary_item key;
	puts(__func__);
	puts("strncpy");
	strncpy(key.device_name, device_name, 32);
	puts("bsearch");
	p = bsearch(&key, audio_driver_dictionary, DIC_LENGTH,
		sizeof(struct audio_driver_dictionary_item),
			audio_dic_item_compar);
	return (struct audio_driver*)p;
*/
	return es1370_get_driver_desc();
}
