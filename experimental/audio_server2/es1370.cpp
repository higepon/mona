#include "es1370.h"
#include <stdlib.h>
#include <pci/pci.h>
#include <monapi/io.h>
#include <monapi/Thread.h>
#include <monalibc/stdio.h>

static handle_t instance = NULL;
const static char device_name[] = "es1370";

struct es1370_driver
{
	handle_t self;
	const char *devname;
	int state;
	uint32_t baseIO;
	uint32_t rate;
	uint32_t bits;
	PciInf pciinfo;
	Pci *pci;
	Thread *thread;
	error_t (*callback)(void* ref, void* buffer, size_t size);
	void *ref;
	size_t bufsize;
	void *dmabuf1;
	void *dmabuf2;
	int usingBuffer;
};

static error_t es1370_device_init(struct es1370_driver *d, const struct audio_data_format *f);
static void es1370_set_sample_rate(struct es1370_driver* d);
static void es1370_set_buffer(struct es1370_driver* d, void *p, size_t size);
static void es1370_set_bits(struct es1370_driver* d);
static void es1370_set_sample_count(struct es1370_driver* d, uint32_t count);
static void es1370_start_playback(struct es1370_driver *d);
static void es1370_stop_playback(struct es1370_driver *d);
static error_t es1370_buffer_setter(struct es1370_driver *d);
static void es1370_interrupt_catcher(void* a);
static void es1370_notifier(void* a);


handle_t es1370_new(const struct audio_data_format *f)
{
	puts(__func__);
	struct es1370_driver *d;
	int result;
	if( instance != NULL ) return (handle_t)NULL;
	d = (struct es1370_driver*)calloc(1, sizeof(struct es1370_driver));
	if( d == NULL ) return d;
	d->self = d;
	d->devname = device_name;

	d->bufsize = 1024;
	d->dmabuf1 = monapi_allocate_dma_memory(d->bufsize);
	d->dmabuf2 = monapi_allocate_dma_memory(d->bufsize);

	puts("init thread");
	d->thread = new MonAPI::Thread(&es1370_interrupt_catcher, d, &es1370_notifier);
	puts("init device");
	result = es1370_device_init(d, f);
	if( result != OK )
	{
		puts("Couldn't init device");
		free(d); return NULL;
	}
	d->state = PAUSE;
	instance = (handle_t)d;
	return (handle_t)d;
}

void es1370_delete(handle_t o)
{
	if( o == NULL ) return;
	delete ((struct es1370_driver*)o)->thread;
	free(o);
	instance = NULL;
	return;
}

error_t es1370_codec_command(handle_t o, codec_command_t c, ...)
{
	return NG;
}

error_t es1370_start(handle_t o)
{
	puts(__func__);
	struct es1370_driver *d = (struct es1370_driver*)o;

	if( es1370_buffer_setter(d) != OK )
	{
		return NG;
	}
	es1370_set_sample_count(d, 0);
	es1370_set_sample_rate(d);
	es1370_set_bits(d);
	es1370_start_playback(d);
	d->state = RUNNING;
	return OK;
}

error_t es1370_stop(handle_t o)
{
	es1370_stop_playback((struct es1370_driver*)o);
	return OK;
}

error_t es1370_set_callback(handle_t o,
	error_t (*callback)(void* ref, void* buffer, size_t size), void* ref)
{
	struct es1370_driver* d = (struct es1370_driver*)o;
	d->callback = callback;
	d->ref = ref;
	return OK;
}

error_t es1370_buffer_setter(struct es1370_driver *d)
{
//	puts(__func__);
//	printf("callback = %x\n", d->callback);
//	printf("buf1 = %x, buf2 = %x\n", d->dmabuf1, d->dmabuf2);
	error_t result;
	void *buf;
	buf = d->usingBuffer == 0 ? d->dmabuf1 : d->dmabuf2;
	d->usingBuffer = d->usingBuffer == 0 ? 1 : 0;
	result = d->callback(d->ref, buf, d->bufsize);
//	puts("set_buffer");
	es1370_set_buffer(d, buf, d->bufsize);
	return result;
}

static error_t es1370_device_init(struct es1370_driver *d, const struct audio_data_format *f)
{
	uint32_t ctrl;
	puts(__func__);

	if( f->sample_rate != 44100 || f->bits != 16 || f->channels != 2 )
		return NG;

	puts("init pci");
	d->pci = new Pci;
	d->pci->CheckPciExist(ES1370_VENDOR_ID, ES1370_DEVICE_ID, &d->pciinfo);
	if( d->pciinfo.Exist != 0 ) return NG;
	d->baseIO = d->pciinfo.BaseAd & ~1;

	ctrl = inp32(d->baseIO+ES1370_REG_CONTROL);
	ctrl |= ES1370_XCTL1|ES1370_CDC_EN;
	ctrl &= ~ES1370_SERR_DISABLE;
	outp32(d->baseIO+ES1370_REG_CONTROL, ctrl);
	sleep(10);

	outp32(ES1370_REG_CODEC, 0);
	outp32(ES1370_REG_CODEC, (1<<8));

	ctrl = inp32(d->baseIO+ES1370_REG_CONTROL);
	ctrl &= ~(ES1370_DAC1_EN|ES1370_DAC2_EN|ES1370_ADC_EN);
	outp32(d->baseIO+ES1370_REG_CONTROL, ctrl);

	d->thread->start();

	d->rate = 3<<12;
	d->bits = 3;

	return OK;
}

static void es1370_set_sample_rate(struct es1370_driver* d)
{
	uint32_t ctrl = inp32(d->baseIO+ES1370_REG_CONTROL);
	ctrl |= d->rate;
	outp32(d->baseIO+ES1370_REG_CONTROL, ctrl);
}

static void es1370_set_buffer(struct es1370_driver* d, void *p, size_t size)
{
//	puts(__func__);
//	printf("p = %x\n", p);
	outp32(d->baseIO+ES1370_REG_MEMPAGE, ES1370_PAGE_DAC);
	outp32(d->baseIO+ES1370_REG_DAC1_FRAMEADR, (uint32_t)p);
	outp32(d->baseIO+ES1370_REG_DAC1_FRAMECNT, (size>>2)-1);
}

static void es1370_set_bits(struct es1370_driver* d)
{
	uint32_t ctrl = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	ctrl |= d->bits;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, ctrl);
}

static void es1370_set_sample_count(struct es1370_driver* d, uint32_t count)
{
	outp32(d->baseIO+ES1370_REG_DAC1_SCOUNT, count);
}

static void es1370_start_playback(struct es1370_driver *d)
{
	uint32_t reg;

	reg = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	reg &= ~(ES1370_P1_LOOP_SEL|ES1370_P1_PAUSE);
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	reg &= ~ES1370_P1_INTR_EN;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);
	reg |= ES1370_P1_INTR_EN;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(d->baseIO+ES1370_REG_CONTROL);
	reg &= ~ES1370_DAC1_EN;
	outp32(d->baseIO+ES1370_REG_CONTROL, reg);
	reg |= ES1370_DAC1_EN;
	outp32(d->baseIO+ES1370_REG_CONTROL, reg);
}

static void es1370_stop_playback(struct es1370_driver *d)
{
	uint32_t reg;

	reg = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	reg |= ES1370_P1_PAUSE;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(d->baseIO+ES1370_REG_CONTROL);
	reg &= ~ES1370_DAC1_EN;
	outp32(d->baseIO+ES1370_REG_CONTROL, reg);
}

static void es1370_interrupt_catcher(void* a)
{
	struct es1370_driver* d = (struct es1370_driver*)a;
	syscall_set_irq_receiver(d->pciinfo.IrqLine, SYS_MASK_INTERRUPT);
	monapi_set_irq(d->pciinfo.IrqLine, MONAPI_TRUE, MONAPI_TRUE);

	MessageInfo msg;
	while(1)
	{
		if( MonAPI::Message::receive(&msg) ) continue;
		switch(msg.header)
		{
		case MSG_INTERRUPTED:
		{
			uint32_t stat, result;
			stat = inp32(d->baseIO+ES1370_REG_STATUS);
			if( stat & 4 )
			{
				if( d->state == RUNNING )
				{
					puts("INTERRUPTED");
					result = es1370_buffer_setter(d);
					if( result != OK ) d->state = PAUSE;
					monapi_set_irq(d->pciinfo.IrqLine, MONAPI_TRUE, MONAPI_TRUE);
				}
			}
			break;
		}
			default: break;
		}
	}
}

static void es1370_notifier(void* a)
{
}
