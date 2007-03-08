#include <monapi/io.h>
#include <monapi/syscall.h>
#include "es1370.h"
#include "audiodriver.h"

extern "C" int32_t aud_es1370_init_driver(int16_t device_id);
extern "C" int32_t aud_es1370_stop_driver();
extern "C" int32_t aud_es1370_init_device();
extern "C" ch_t    aud_es1370_create_channel();
extern "C" int32_t aud_es1370_prepare_channel(ch_t ch, int32_t rate, int32_t bits, int32_t isStereo);
extern "C" int32_t aud_es1370_set_buffer(ch_t ch, void *buf, size_t size);
extern "C" int32_t aud_es1370_start_channel(ch_t ch, int32_t loop);
extern "C" int32_t aud_es1370_emit_interrupted();

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
	aud_es1370_create_channel,	// create_channel
	aud_es1370_prepare_channel,	// prepare_channel
	aud_es1370_set_buffer,	// set_buffer
	aud_es1370_start_channel,	// start_channel
	NULL,	// stop_channel
	NULL,	// destroy_channel
	aud_es1370_emit_interrupted,	// emit_interrupted
};


extern "C" int32_t aud_es1370_init_driver(int16_t device_id)
{
	if( dev != NULL ) return 0;
	desc.driver_id = device_id;
	syscall_get_io();
	dev = new es1370;
	return 0;
}

extern "C" int32_t aud_es1370_stop_driver()
{
	if( dev != NULL ) delete dev;
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

extern "C" ch_t aud_es1370_create_channel()
{
	return dev->createChannel();
}

extern "C" int32_t aud_es1370_prepare_channel(ch_t ch, int32_t rate, int32_t bits, int32_t isStereo)
{
	if( ch == 1 )
	{
		dev->prepareChannelDAC1(rate, bits, isStereo);
	}
	return 0;
}

extern "C" int32_t aud_es1370_set_buffer(ch_t ch, void *buf, size_t size)
{
	if( ch == 1 )
	{
		dev->setBufferDAC1(buf, size);
	}
	return 0;
}

extern "C" int32_t aud_es1370_start_channel(ch_t ch, int32_t loop)
{
	if( ch == 1 )
	{
		dev->startDAC1();
	}
	return 0;
}

extern "C" int32_t aud_es1370_emit_interrupted()
{
	monapi_set_irq(dev->pciinfo.IrqLine, MONAPI_TRUE, MONAPI_TRUE);
}

es1370::es1370()
{
	this->pci = new Pci;
	usingchannels[0] = false;
	usingchannels[1] = false;
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

ch_t es1370::createChannel()
{
	for( int i = 0; i < 2 ; i++ )
	{
		if( !usingchannels[i] ) return (ch_t)i+1;
	}
	return (ch_t)-1;
}

void es1370::setSampleRate(int rate)
{
	int bit = 0;
	uint32_t n;
	switch(rate)
	{
		case 5512: bit = 0; break;
		case 11025:bit = 1; break;
		case 22050:bit = 2; break;
		case 44100:bit = 3; break;
		default: break;
	}
	n = inp32(baseIO+ES1370_REG_CONTROL);
	n &= ~0x3000;
	n |= bit;
	outp32(baseIO+ES1370_REG_CONTROL, n);
}

void es1370::prepareChannelDAC1(int rate, int bits, int isStereo)
{
	// Disable DAC1
	outp32(baseIO+ES1370_REG_CONTROL, inp32(baseIO+ES1370_REG_CONTROL)&~ES1370_DAC1_EN);
	setSampleRate(rate);
	// set stereo mode & bits
	if( bits == 16 )
		if( isStereo != 0 ) outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_S_EB|ES1370_P1_S_MB);
		else outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_S_EB);
	else if( bits == 8 )
		if( isStereo != 0 ) outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_S_MB);
		else outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~(ES1370_P1_S_MB|ES1370_P1_S_EB));
	// enable ccb interrupt
	outp32(ES1370_REG_SERIAL_CONTROL, inp32(ES1370_REG_SERIAL_CONTROL)|ES1370_CCB_INTRM);
	// enable SERR
	outp32(ES1370_REG_SERIAL_CONTROL, inp32(ES1370_REG_SERIAL_CONTROL)&~ES1370_SERR_DISABLE);
	// enable interrupt
	outp32(ES1370_REG_SERIAL_CONTROL, inp32(ES1370_REG_SERIAL_CONTROL)|ES1370_P1_INTR_EN);
}

typedef union _fr
{
	uint32_t n;
	struct s
	{
		uint16_t currentcount;
		uint16_t buffersize;
	}s;
}FrameRegister;

void es1370::setBufferDAC1(void *p, size_t size)
{
	outp32(baseIO+ES1370_REG_MEMPAGE, ES1370_PAGE_DAC&0xf);
	FrameRegister fr;
	fr.s.currentcount = 0;
	fr.s.buffersize = (unsigned short)size;
	outp32(baseIO+ES1370_REG_DAC1_FRAMEADR, (uint32_t)p);
	outp32(baseIO+ES1370_REG_DAC1_FRAMECNT, (uint32_t)fr.n);
}

void es1370::startDAC1()
{
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P1_PAUSE);
	outp32(baseIO+ES1370_REG_CONTROL, inp32(baseIO+ES1370_REG_CONTROL)|ES1370_DAC1_EN);
}

