#include <monapi/io.h>
#include <monapi/syscall.h>
#include <monalibc/string.h>
#include "../debug.h"
#include "es1370.h"
#include "audiodriver.h"

extern "C" int32_t aud_es1370_init_driver(int16_t device_id);
extern "C" int32_t aud_es1370_stop_driver();
extern "C" int32_t aud_es1370_init_device();
extern "C" ch_t    aud_es1370_create_channel();
extern "C" int32_t aud_es1370_prepare_channel(ch_t ch, int32_t rate, int32_t bits, int32_t isStereo);
extern "C" int32_t aud_es1370_set_buffer(ch_t ch, void *buf, size_t size);
extern "C" int32_t aud_es1370_start_channel(ch_t ch, int32_t loop);
extern "C" int32_t aud_es1370_stop_channel(ch_t ch);
extern "C" int32_t aud_es1370_destroy_channel(ch_t ch);
extern "C" int32_t aud_es1370_emit_interrupted();
extern "C" int32_t aud_es1370_get_int_state();

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
	aud_es1370_stop_channel,	// stop_channel
	aud_es1370_destroy_channel,	// destroy_channel
	aud_es1370_emit_interrupted,	// emit_interrupted
	aud_es1370_get_int_state,	// int_state
};


extern "C" int32_t aud_es1370_init_driver(int16_t device_id)
{
	dputs("#Audio: aud_es1370_init_driver");
	if( dev != NULL ) return 0;
	desc.driver_id = device_id;
	syscall_get_io();
	dev = new es1370;
	return 0;
}

extern "C" int32_t aud_es1370_stop_driver()
{
	dputs("#Audio: aud_es1370_stop_driver");
	if( dev != NULL ) delete dev;
	return 0;
}

extern "C" int32_t aud_es1370_init_device()
{
	dputs("#Audio: aud_es1370_init_device");
	if( !dev->existDevice() ) return -1;
	dev->setIRQ();
	return 0;
}

extern "C" struct driver_desc *aud_es1370_get_desc()
{
	dputs("#Audio: aud_es1370_get_desc");
	return &desc;
}

extern "C" void aud_es1370_release_desc(struct driver_desc *desc)
{
	dputs("#Audio: aud_es1370_release_desc");
}

extern "C" ch_t aud_es1370_create_channel()
{
	dputs("#Audio: aud_es1370_create_channel");
	return dev->createChannel();
}

extern "C" int32_t aud_es1370_prepare_channel(ch_t ch, int32_t rate, int32_t bits, int32_t isStereo)
{
	dprintf("#Audio: aud_es1370_prepare_channel; rate = %d, bits = %d isStereo = %d\n", rate, bits, isStereo);
	if( ch == 1 )
	{
		dev->prepareChannelDAC1(rate, bits, isStereo);
	}
	return 0;
}

extern "C" int32_t aud_es1370_set_buffer(ch_t ch, void *buf, size_t size)
{
	dprintf("#Audio: aud_es1370_set_buffer; ch = %d, buf = %x, size = %d\n", ch, buf, size);
	if( ch == 1 )
	{
		dev->setBufferDAC1(buf, size);
	}
	return 0;
}

extern "C" int32_t aud_es1370_start_channel(ch_t ch, int32_t loop)
{
	dprintf("#Audio: aud_es1370_start_channel; ch = %d\n", ch);
	if( ch == 1 )
	{
		dev->startDAC1();
	}
	return 0;
}

extern "C" int32_t aud_es1370_stop_channel(ch_t ch)
{
	dprintf("#Audio: aud_es1370_stop_channel; ch = %d\n", ch);
	if( ch == 1 )
	{
		dev->stopDAC1();
	}
	return 0;
}

extern "C" int32_t aud_es1370_destroy_channel(ch_t ch)
{
	dprintf("#Audio: aud_es1370_destroy_channel; ch = %d\n", ch);
	dev->destroyChannel(ch);
	return 0;
}

extern "C" int32_t aud_es1370_emit_interrupted()
{
	dputs("#Audio: aud_es1370_emit_interrupted");
	monapi_set_irq(dev->pciinfo.IrqLine, MONAPI_TRUE, MONAPI_TRUE);
	dev->set_int_state();
	return 0;
}

extern "C" int32_t aud_es1370_get_int_state()
{
	return dev->int_state;
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
	dputs("#ES1370: setIRQ");
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

void es1370::destroyChannel(ch_t ch)
{
	if( ch > 2 && ch < 1 ) return;
	usingchannels[ch] = false;
}

uint32_t es1370::makeSampleRateBit(int rate)
{
	uint32_t bit;
	switch(rate)
	{
		case 5512: bit=0;break;
		case 11025:bit=1;break;
		case 22050:bit=2;break;
		case 44100:bit=3;break;
		default:break;
	}
	return bit;
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
	dprintf("#ES1370: prepareChannelDAC1; rate = %d, bits = %d, isStereo = %d\n", rate, bits, isStereo);
	uint32_t cnt, ser;
	cnt = inp32(baseIO+ES1370_REG_CONTROL);
	ser = inp32(baseIO+ES1370_REG_SERIAL_CONTROL);
	// Disable DAC1
	cnt &= ~ES1370_DAC1_EN;
	outp32(baseIO+ES1370_REG_CONTROL, cnt);
	// set mempage
	outp32(baseIO+ES1370_REG_MEMPAGE, ES1370_PAGE_DAC&0xf);
	// enable ccb interrupt
//	cnt |= ES1370_CCB_INTRM;
//	outp32(baseIO+ES1370_REG_CONTROL, inp32(baseIO+ES1370_REG_CONTROL)|ES1370_CCB_INTRM);
	// enable SERR
//	cnt &= ~ES1370_SERR_DISABLE;
//	outp32(baseIO+ES1370_REG_CONTROL, inp32(baseIO+ES1370_REG_CONTROL)&~ES1370_SERR_DISABLE);
	// hoge
//	ser &= ~ES1370_P1_INTR_EN;
//	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P1_INTR_EN);
//	outp32(baseIO+ES1370_REG_CONTROL, inp32(baseIO+ES1370_REG_CONTROL)&~1);
	// set sample rate
	cnt &= ~0x3000;
	cnt |= makeSampleRateBit(rate)<<12;
//	setSampleRate(rate);
	// set stereo mode & bits
	if( bits == 16 )
		if( isStereo != 0 ) ser |= ES1370_P1_S_EB|ES1370_P1_S_MB;
		else ser |= ES1370_P1_S_EB;
	else if( bits == 8 )
		if( isStereo != 0 ) ser |= ES1370_P1_S_MB;
		else ser &= ~(ES1370_P1_S_MB|ES1370_P1_S_EB);
/*
	if( bits == 16 )
		if( isStereo != 0 ) outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_S_EB|ES1370_P1_S_MB);
		else outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_S_EB);
	else if( bits == 8 )
		if( isStereo != 0 ) outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_S_MB);
		else outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~(ES1370_P1_S_MB|ES1370_P1_S_EB));
*/

	// set loop mode
	ser &= ~ES1370_P1_LOOP_SEL;
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, ser);
	outp32(baseIO+ES1370_REG_CONTROL, cnt);
//	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P1_LOOP_SEL);
}

typedef struct _fr
{
	uint16_t buffersize;
	uint16_t currentcount;
}FrameRegister;

void es1370::setBufferDAC1(void *p, size_t size)
{
	uint32_t n;
	dprintf("#ES1370: setBufferDAC1; p = %x, size = %d\n", p, size);
	outp32(baseIO+ES1370_REG_MEMPAGE, ES1370_PAGE_DAC&0xf);
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_PAUSE);
	FrameRegister fr;
//	fr.currentcount = 0;
//	fr.buffersize = (unsigned short)(size-1);
//	memcpy(&n, &fr, sizeof(int));
//	dprintf("#ES1370: n = %x\n", n);
	outp32(baseIO+ES1370_REG_DAC1_FRAMEADR, (uint32_t)p);
//	outp32(baseIO+ES1370_REG_DAC1_FRAMECNT, (uint32_t)n);
	outp32(baseIO+ES1370_REG_DAC1_FRAMECNT, (size>>2)-1);

	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~(ES1370_P1_LOOP_SEL|ES1370_P1_PAUSE|ES1370_P1_SCT_RLD|ES1370_P1_S_EB|ES1370_P1_S_MB));
}

void es1370::startDAC1()
{
	dputs("#ES1370: startDAC1");
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P1_PAUSE);
	// enable interrupt
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_INTR_EN);
	outp32(baseIO+ES1370_REG_CONTROL, inp32(baseIO+ES1370_REG_CONTROL)|ES1370_DAC1_EN);
	dumpRegisters();
}

void es1370::stopDAC1()
{
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_PAUSE);
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P1_INTR_EN);
}

void es1370::set_int_state()
{
	uint32_t x;
	x = inp32(baseIO+ES1370_REG_STATUS);
	this->int_state = x;
	if( x & (1<<31) )
	{
		outp32(baseIO+ES1370_REG_STATUS, 0);
	}
}

inline void rgor(int rA, int rS, int rB)
{
}

inline void rgori(int rA, int rS, int imm)
{
}

void es1370::dumpRegisters()
{
	uint32_t result;
	return;

	result = inp32(baseIO+ES1370_REG_CONTROL);
	dprintf("Control: %x\n", result);

	result = inp32(baseIO+ES1370_REG_SERIAL_CONTROL);
	dprintf("Serial Control: %x\n", result);

	result = inp32(baseIO+ES1370_REG_STATUS);
	dprintf("Status: %x\n", result);

	result = inp8(baseIO+ES1370_REG_UART_DATA);
	dprintf("UART Data: %x\n", result);

	result = inp8(baseIO+ES1370_REG_UART_STATUS);
	dprintf("UART Status: %x\n", result);

	result = inp32(baseIO+ES1370_REG_MEMPAGE);
	dprintf("Memory Page: %x\n", result);

	result = inp32(baseIO+ES1370_REG_SERIAL_CONTROL);
	dprintf("Serial Interface Control: %x\n", result);

	result = inp32(baseIO+ES1370_REG_DAC1_FRAMEADR);
	dprintf("DAC1 Frame Address: %x\n", result);

	result = inp32(baseIO+ES1370_REG_DAC1_FRAMECNT);
	dprintf("DAC1 Frame Count & Size: %x\n", result);
}



