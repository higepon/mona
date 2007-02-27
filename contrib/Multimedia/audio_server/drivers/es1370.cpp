#include <monapi/io.h>
#include <monapi/syscall.h>
#include "es1370.h"
#include "audiodriver.h"

extern "C" int32_t aud_es1370_init_driver(int16_t device_id);
extern "C" int32_t aud_es1370_stop_driver();
extern "C" int32_t aud_es1370_init_device();

static es1370 *dev = NULL;

struct driver_desc desc = {
	0,	// driver_type, but unused
	0,	// driver_id, but unused
	aud_es1370_init_driver,	// init_driver
	aud_es1370_init_device,	// init_device
	aud_es1370_stop_driver,	// stop_driver
	NULL,	// find_device
	NULL,	// maximum_channels
	NULL,	// using_channels
	NULL,	// create_channel
	NULL,	// prepare_channel
	NULL,	// set_buffer
	NULL,	// start_channel
	NULL,	// stop_channel
	NULL,	// destroy_channel
	NULL,	// emit_interrupted
};


extern "C" int32_t aud_es1370_init_driver(int16_t device_id)
{
	desc.driver_id = device_id;
	syscall_get_io();
	dev = new es1370;
	return 0;
}

extern "C" int32_t aud_es1370_stop_driver()
{
	delete dev;
	return 0;
}

extern "C" int32_t aud_es1370_init_device()
{
	if( !dev->existDevice() ) return -1;
	dev->setIRQ();
	return 0;
}

extern "C" struct driver_desc *aud_es1370_get_desc()
{
	return &desc;
}

extern "C" void aud_es1370_release_desc(struct driver_desc *desc)
{
}


es1370::es1370()
{
	this->pci = new Pci;
}

es1370::~es1370()
{
	delete this->pci;
}

bool es1370::existDevice()
{
	pci->CheckPciExist(ES1370_VENDOR_ID, ES1370_DEVICE_ID, &this->pciinfo);
	if( pciinfo.Exist != 0 ) return false;
	baseIO = pciinfo.BaseAd & ~1;
	return true;
}

void es1370::setIRQ()
{
	syscall_get_io();
	syscall_set_irq_receiver(this->pciinfo.IrqLine, SYS_MASK_INTERRUPT);
	monapi_set_irq(this->pciinfo.IrqLine, MONAPI_TRUE, MONAPI_TRUE);
}

void es1370::unsetIRQ()
{
	syscall_remove_irq_receiver(this->pciinfo.IrqLine);
}

void es1370::enableChannel(enum e_channel ch)
{
	uint32_t mask;
	switch(ch)
	{
		case DAC1: mask = ES1370_DAC1_EN;
		case DAC2: mask = ES1370_DAC2_EN;
		case ADC: mask = ES1370_ADC_EN;
		default: mask = ES1370_DAC1_EN;
	}
}

