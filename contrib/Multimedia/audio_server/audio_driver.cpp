#include "audio_driver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "drivers_dictionary.h"

static int audio_dic_item_compar(const void* a, const void *b)
{
    struct audio_driver_dictionary_item *item1;
    struct audio_driver_dictionary_item *item2;
    item1 = (struct audio_driver_dictionary_item*)a;
    item2 = (struct audio_driver_dictionary_item*)b;

    return strcmp(item1->device_name, item2->device_name);
}

extern "C" struct audio_driver *audio_driver_factory(const char *device_name)
{
    void *p;
    struct audio_driver_dictionary_item key;
    struct audio_driver_dictionary_item *item;
    key.device_name = device_name;
    p = bsearch(&key, audio_driver_dictionary, DIC_LENGTH,
        sizeof(struct audio_driver_dictionary_item),
            audio_dic_item_compar);
    if( p == NULL ) return NULL;
    item = (struct audio_driver_dictionary_item*)p;
    return item->audio_driver_getter();
}
