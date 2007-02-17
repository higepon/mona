#include "audiodriver.h"


extern "C" struct driver_desc *aud_es1370_get_desc()
{
	return NULL;
}

extern "C" void aud_es1370_release_desc(struct driver_desc *desc)
{
}
