#include "es1370.h"
#include <stdlib.h>
#include <pci/Pci.h>
#include <monapi/io.h>
#include <monapi/Thread.h>
#include <monalibc/stdio.h>

static handle_t instance = NULL;
const static char device_name[] = "es1370";
struct audio_driver es1370_audio_driver_desc = 
{
	es1370_new,
	es1370_delete,
	es1370_codec_command,
	es1370_start,
	es1370_stop,
	es1370_set_callback,
	es1370_set_stopped_callback,
};


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
	error_t (*callback)(void* ref, void* buffer, size_t size, size_t *wrote);
	error_t (*stopped_callback)(void* ref);
	void *ref;
	void *stopped_ref;
	size_t bufsize;
	void *dmabuf1;
	void *dmabuf2;
	int usingBuffer;
};

int check_driver_desc(const struct es1370_driver *d)
{
	puts("check_driver_desc");
	if( d->callback == NULL ) return puts("callback is null");
	if( d->ref == NULL ) return puts("ref is null");
	if( d->dmabuf1 == NULL ) return puts("dmabuf1 is null");
	if( d->dmabuf2 == NULL ) return puts("dmabuf2 is null");
	return 0;
}

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

struct audio_driver* es1370_get_driver_desc()
{
	return &es1370_audio_driver_desc;
}

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

	d->bufsize = 2048;
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
	syscall_get_io();
	puts(__func__);
	return (handle_t)d;
}

void es1370_delete(handle_t o)
{
	if( o == NULL ) return;
	struct es1370_driver *d = (struct es1370_driver*)o;
	delete d->thread;
	monapi_deallocate_dma_memory(d->dmabuf1, d->bufsize);
	monapi_deallocate_dma_memory(d->dmabuf2, d->bufsize);
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
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	struct es1370_driver *d = (struct es1370_driver*)o;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	if( es1370_buffer_setter(d) != OK )
	{
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
		return NG;
	}

    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	es1370_set_sample_rate(d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	es1370_set_bits(d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	es1370_start_playback(d);

    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	d->state = RUNNING;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

	es1370_buffer_setter(d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	return OK;
}

error_t es1370_stop(handle_t o)
{
	es1370_stop_playback((struct es1370_driver*)o);
	return OK;
}

error_t es1370_set_callback(handle_t o,
	error_t (*callback)(void* ref, void* buffer, size_t size, size_t*), void* ref)
{
	struct es1370_driver* d = (struct es1370_driver*)o;
	d->callback = callback;
	d->ref = ref;
	return OK;
}

error_t es1370_set_stopped_callback(handle_t o, error_t (*callback)(void* ref), void *ref)
{
	struct es1370_driver *d = (struct es1370_driver*)o;
	d->stopped_callback = callback;
	d->stopped_ref = ref;
	return OK;
}

error_t es1370_buffer_setter(struct es1370_driver *d)
{
//	check_driver_desc(d);
    _logprintf("callback = %x\n", d->callback);
	puts(__func__);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//	printf("buf1 = %x, buf2 = %x\n", d->dmabuf1, d->dmabuf2);
	error_t result;
	size_t wrote;
	void *buf;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	buf = d->usingBuffer == 0 ? d->dmabuf1 : d->dmabuf2;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	d->usingBuffer = d->usingBuffer == 0 ? 1 : 0;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	result = d->callback(d->ref, buf, d->bufsize, &wrote);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	puts("set_buffer");
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	es1370_set_buffer(d, buf, wrote);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	es1370_set_sample_count(d, wrote/2-1);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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

union frame_reg
{
	uint32_t x;
	struct
	{
		uint16_t size;
		uint16_t count;
	}s;
};

static void es1370_set_buffer(struct es1370_driver* d, void *p, size_t size)
{
//	puts(__func__);
//	printf("p = %x\n", p);
/*
	struct frame_reg fr;
	fr.s.count = 0;
	fr.s.size = size-1;
*/
    _logprintf("ES1370 DRI: d = %x\n", d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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
	_logprintf("d = %x\n", d);

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

void rdtsc(uint32_t* timeL, uint32_t* timeH) {

    uint32_t l,h;
    asm volatile("rdtsc           \n"
                 "mov   %%eax, %0 \n"
                 "mov   %%edx, %1 \n"
                 : "=m"(l), "=m"(h)
                 : /* no */
                 : "eax", "edx");
    *timeL = l;
    *timeH = h;
}


static void es1370_interrupt_catcher(void* a)
{
	struct es1370_driver* d = (struct es1370_driver*)a;
    _printf("irq=%x\n", d->pciinfo.IrqLine);
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
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
			uint32_t stat, result;
			stat = inp32(d->baseIO+ES1370_REG_STATUS);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
			if( stat & 4 )
			{
				if( d->state == RUNNING )
				{
			//		puts("INTERRUPTED");
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

					es1370_stop_playback(d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

					result = es1370_buffer_setter(d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

					es1370_start_playback(d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

					if( result != OK )
					{
						d->state = PAUSE;
						return;
					}
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
					result = es1370_buffer_setter(d);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
					/*
					result = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
					result &= ~ES1370_P1_INTR_EN;
					outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, result);
					result |= ES1370_P1_INTR_EN;
					outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, result);
					*/
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
	struct es1370_driver *d = (struct es1370_driver*)a;
	d->stopped_callback(d->stopped_ref);
}
