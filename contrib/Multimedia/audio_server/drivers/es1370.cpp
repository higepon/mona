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
	dev->init();
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
	dev->samplerate = dev->makeSampleRateBit(rate);
	dev->bits = bits == 16 ? 1 << 3 : 0;
	dev->bits |= isStereo ? 1 << 2 : 0;
	return 0;
}

extern "C" int32_t aud_es1370_set_buffer(ch_t ch, void *buf, size_t size)
{
	dprintf("#Audio: aud_es1370_set_buffer; ch = %d, buf = %x, size = %d\n", ch, buf, size);
	dev->setBuffer(buf, size);
	return 0;
}

extern "C" int32_t aud_es1370_start_channel(ch_t ch, int32_t loop)
{
	dprintf("#Audio: aud_es1370_start_channel; ch = %d\n", ch);
	dev->start();
	return 0;
}

extern "C" int32_t aud_es1370_stop_channel(ch_t ch)
{
	dprintf("#Audio: aud_es1370_stop_channel; ch = %d\n", ch);
	dev->stop();
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
	dev->setFormat();
	return 0;
}

extern "C" int32_t aud_es1370_get_int_state()
{
	return dev->int_state;
}

es1370::es1370()
{
	this->pci = new Pci;
	usingchannel = false;
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

void es1370::init()
{
	uint32_t ctrl = 0;
	int i=0;
	uint32_t status;


	ctrl |= ES1370_XCTL1|ES1370_CDC_EN;;
	ctrl &= ~ES1370_SERR_DISABLE;
	outp32(ES1370_REG_CONTROL, ctrl);
	sleep(10);

	writeCodec(0x16, 0x02);
	sleep(20);
	writeCodec(0x16, 0x03);
	sleep(20);
	status = inp32(ES1370_REG_STATUS);
	while((i!=0xFFFF)&&(status & (1<<10)))
	{
		status = inp32(ES1370_REG_STATUS);
		i++;
	}

	writeCodec(0x00, 0x00);
	writeCodec(0x01, 0x00);

	ctrl = inp32(ES1370_REG_CONTROL);
	ctrl &= ~(ES1370_M_SBB|ES1370_DAC_SYNC|ES1370_DAC1_EN|ES1370_DAC2_EN
		 |ES1370_ADC_EN);
	outp32(ES1370_REG_CONTROL, ctrl);

	writeCodec(0x17, 0);
	sleep(20);
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
	return usingchannel ? -1 : 1;
}

void es1370::destroyChannel(ch_t ch)
{
	usingchannel = false;
}

uint32_t es1370::makeSampleRateBit(int rate)
{
	uint32_t bit=0;
	switch(rate)
	{
		case 5512: bit=0<<12;break;
		case 11025:bit=1<<12;break;
		case 22050:bit=2<<12;break;
		case 44100:bit=3<<12;break;
		default:break;
	}
	return bit;
}

void es1370::prepare(int rate, int bits, int isStereo)
{
	dprintf("#ES1370: prepareChannelDAC1; rate = %d, bits = %d, isStereo = %d\n", rate, bits, isStereo);
	uint32_t cnt, ser;
	setFormat();
	cnt = inp32(baseIO+ES1370_REG_CONTROL);
	ser = inp32(baseIO+ES1370_REG_SERIAL_CONTROL);
	//outp32(baseIO+ES1370_REG_CONTROL, cnt);
	// set loop mode
	ser &= ~ES1370_P1_LOOP_SEL;
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, ser);
//	outp32(baseIO+ES1370_REG_CONTROL, cnt);
//	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P1_LOOP_SEL);
}

void es1370::setBuffer(void *p, size_t size)
{
	dprintf("#ES1370: setBufferDAC1; p = %x, size = %d\n", p, size);
	this->prepare(0, 0, 0);
	outp32(baseIO+ES1370_REG_MEMPAGE, ES1370_PAGE_DAC);
	outp32(baseIO+ES1370_REG_DAC2_FRAMEADR, (uint32_t)p);
	outp32(baseIO+ES1370_REG_DAC2_FRAMECNT, (size>>2)-1);
}

void es1370::setFormat()
{
	uint32_t x;
	x = inp32(ES1370_REG_SERIAL_CONTROL);
	x &= ~ 3<<2;
	x |= bits;
	outp32(ES1370_REG_SERIAL_CONTROL, x);
	x = inp32(ES1370_REG_CONTROL);
	x &= ~ 0x3000;
	x |= samplerate;
	outp32(ES1370_REG_CONTROL, x);
}

void es1370::start()
{
	dputs("#ES1370: startDAC1");
	uint32_t reg = 0;
	reg = inp32(ES1370_REG_SERIAL_CONTROL);
	reg &= ~(7<<19);
	reg &= (reg & 8) ? (2<<19) : (1<<19);
	outp32(ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(ES1370_REG_SERIAL_CONTROL);
	reg &= ~(7<<19);
	outp32(ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(ES1370_REG_SERIAL_CONTROL);
	reg &= ~(ES1370_P2_LOOP_SEL|ES1370_P2_PAUSE|ES1370_P2_DAC_SEN);
	outp32(ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(ES1370_REG_SERIAL_CONTROL);
	reg |= ES1370_DAC2_EN;
	outp32(ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(ES1370_REG_SERIAL_CONTROL);
	reg &= ~ES1370_DAC2_EN;
	outp32(ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(ES1370_REG_SERIAL_CONTROL);
	reg &= ~ES1370_P2_INTR_EN;
	outp32(ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(ES1370_REG_SERIAL_CONTROL);
	reg |= ES1370_P2_INTR_EN;
	outp32(ES1370_REG_SERIAL_CONTROL, reg);
}

void es1370::stop()
{
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)|ES1370_P1_PAUSE);
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, inp32(baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P1_INTR_EN);
}

void es1370::set_int_state()
{
	uint32_t x;
	uint32_t ser;
	x = inp32(baseIO+ES1370_REG_STATUS);
	this->int_state = x;
	if( x & (1<<31) )
	{
		outp32(baseIO+ES1370_REG_STATUS, 0);
		if( x & (1<<1) )
		{
			dputs("DAC2 Interrupted.");
			ser = inp32(baseIO+ES1370_REG_SERIAL_CONTROL);
			ser &= ~ES1370_P2_INTR_EN;
			outp32(baseIO+ES1370_REG_SERIAL_CONTROL, ser);
			ser |= ES1370_P2_INTR_EN;
			outp32(baseIO+ES1370_REG_SERIAL_CONTROL, ser);
		}
	}
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

void es1370::writeCodec(uint8_t addr, uint8_t data)
{
	uint32_t x = 0;
	x = addr << 8;
	x |= data;
	outp32(baseIO+ES1370_REG_CODEC, x);
}


