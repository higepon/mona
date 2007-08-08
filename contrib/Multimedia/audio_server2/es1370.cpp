#include "es1370.h"
#include <stdlib.h>
#include <pci/Pci.h>
#include <monapi/io.h>
#include <monapi/Thread.h>
#include <monalibc/stdio.h>
#include <monapi/syscall.h>
#include <monapi/Message.h>

#define PCLKDIV(x) (1411200/(x)-2)
#define f_FMT(b,c) ((b==8?0:2)|(c==2?1:0))
#define sh_FMT 2

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
	es1370_set_format,
	es1370_get_format,
	NULL,
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

static error_t es1370_device_init(struct es1370_driver *d);
inline static void es1370_set_sample_rate(struct es1370_driver* d, uint32_t);
inline static void es1370_set_buffer(struct es1370_driver* d, void *p, size_t size);
inline static void es1370_set_bits(struct es1370_driver* d, uint32_t);
inline static void es1370_set_channels(struct es1370_driver* d, uint32_t);
inline static void es1370_set_sample_count(struct es1370_driver* d, uint32_t count);
inline static void es1370_start_playback(struct es1370_driver *d);
inline static void es1370_stop_playback(struct es1370_driver *d);
static error_t es1370_buffer_setter(struct es1370_driver *d);
static void es1370_interrupt_catcher(void* a);
static void es1370_notifier(void* a);

struct audio_driver* es1370_get_driver_desc()
{
	return &es1370_audio_driver_desc;
}

handle_t es1370_new()
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
	result = es1370_device_init(d);
	if( result != OK )
	{
		puts("Couldn't init device");
		free(d); return NULL;
	}
	d->state = PAUSE;
	instance = (handle_t)d;

	es1370_set_buffer(d, d->dmabuf1, d->bufsize);
	es1370_set_sample_count(d, d->bufsize);
//	syscall_get_io();
//	puts(__func__);
	return (handle_t)d;
}

void es1370_delete(handle_t o)
{
	if( o == NULL ) return;
	struct es1370_driver *d = (struct es1370_driver*)o;
	d->thread->stop();
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

error_t es1370_set_format(handle_t o, const struct audio_data_format *f)
{
	if( o == NULL || f == NULL ) return NG;
	struct es1370_driver *d = (struct es1370_driver*)o;
	uint32_t fmt, pclkdiv, reg;
	pclkdiv = PCLKDIV(f->sample_rate);
	fmt = f_FMT(f->bits, f->channels);
	reg = inp32(d->baseIO+ES1370_REG_CONTROL);
	reg |= pclkdiv<<ES1370_PCLKDIV_SH;
	reg &= ~(ES1370_ADC_EN|ES1370_UART_EN|ES1370_JYSTK_EN);
	outp32(d->baseIO+ES1370_REG_CONTROL, reg);
	reg = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	reg &= ~(7<<19); // end_inc
	reg |= 2<<19;
	reg |= fmt<<sh_FMT;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);
	return OK;
}

error_t es1370_get_format(handle_t o, struct audio_data_format *f)
{
	if( o == NULL || f == NULL ) return NG;
	//struct es1370_driver *d = (struct es1370_driver*)o;
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
	error_t result;
	size_t wrote;
	void *buf;
    	buf = d->dmabuf1;
//	buf = d->usingBuffer == 0 ? d->dmabuf1 : d->dmabuf2;
//	d->usingBuffer = d->usingBuffer == 0 ? 1 : 0;
	result = d->callback(d->ref, buf, d->bufsize, &wrote);
//	es1370_set_buffer(d, buf, d->bufsize);
	return result;
}

static error_t es1370_device_init(struct es1370_driver *d)
{
	uint32_t ctrl, pclkdiv, fmt;
	puts(__func__);

	puts("init pci");
	d->pci = new Pci;
	d->pci->CheckPciExist(ES1370_VENDOR_ID, ES1370_DEVICE_ID, &d->pciinfo);
	if( d->pciinfo.Exist != 0 ) return NG;
	d->baseIO = d->pciinfo.BaseAd & ~1;

	pclkdiv = PCLKDIV(44100);

	ctrl = inp32(d->baseIO+ES1370_REG_CONTROL);
	ctrl |= ES1370_CDC_EN;
	ctrl &= ~ES1370_SERR_DISABLE;
	ctrl |= pclkdiv;
	outp32(d->baseIO+ES1370_REG_CONTROL, ctrl);
	sleep(10);

	fmt = f_FMT(16, 2);

	ctrl = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	ctrl |= fmt<<sh_FMT;
	ctrl &= ~(ES1370_R1_LOOP_SEL|ES1370_P1_LOOP_SEL|ES1370_P2_LOOP_SEL);
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, ctrl);

	outp32(ES1370_REG_CODEC, 0);
	outp32(ES1370_REG_CODEC, (1<<8));

/*
	ctrl = inp32(d->baseIO+ES1370_REG_CONTROL);
	ctrl &= ~(ES1370_DAC1_EN|ES1370_DAC2_EN|ES1370_ADC_EN);
	outp32(d->baseIO+ES1370_REG_CONTROL, ctrl);
*/

	outp32(d->baseIO+ES1370_REG_MEMPAGE, ES1370_PAGE_DAC);

	d->thread->start();

	return OK;
}

inline static void es1370_set_sample_rate(struct es1370_driver* d, uint32_t rate)
{
	uint32_t ctrl = inp32(d->baseIO+ES1370_REG_CONTROL);
	uint32_t pclkdiv = 1411200/(rate+2);
	pclkdiv <<= 16;
	ctrl |= rate;
	outp32(d->baseIO+ES1370_REG_CONTROL, ctrl);
}

inline static void es1370_set_channels(struct es1370_driver* d, uint32_t n)
{
	uint32_t ctrl = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	uint32_t fmt;
	fmt |= n == 2 ? 1 : 0;
	ctrl |= fmt<<2;
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

inline static void es1370_set_buffer(struct es1370_driver* d, void *p, size_t size)
{
//	puts(__func__);
//	printf("p = %x\n", p);
/*
	struct frame_reg fr;
	fr.s.count = 0;
	fr.s.size = size-1;
*/
    //_logprintf("ES1370 DRI: d = %x\n", d);
    //_logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//	outp32(d->baseIO+ES1370_REG_MEMPAGE, ES1370_PAGE_DAC);
	outp32(d->baseIO+ES1370_REG_DAC2_FRAMEADR, (uint32_t)p);
	outp32(d->baseIO+ES1370_REG_DAC2_FRAMECNT, size-1);
    //_logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return;
}

inline static void es1370_set_bits(struct es1370_driver* d, uint32_t bits)
{
	uint32_t ctrl = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	uint32_t fmt = bits == 8 ? 0 : 2;
	ctrl |= fmt<<2;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, ctrl);
}

inline static void es1370_set_sample_count(struct es1370_driver* d, uint32_t count)
{
	outp32(d->baseIO+ES1370_REG_DAC2_SCOUNT, count-1);
}

inline static void es1370_start_playback(struct es1370_driver *d)
{
	uint32_t reg;
	//_logprintf("d = %x\n", d);

	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL,
		inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL)&~ES1370_P2_PAUSE);

	reg = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	reg |= ES1370_P2_INTR_EN;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(d->baseIO+ES1370_REG_CONTROL);
	reg |= ES1370_DAC2_EN;
	outp32(d->baseIO+ES1370_REG_CONTROL, reg);
}

inline static void es1370_stop_playback(struct es1370_driver *d)
{
	uint32_t reg;

	reg = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	reg |= ES1370_P2_PAUSE;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);

	reg = inp32(d->baseIO+ES1370_REG_CONTROL);
	reg &= ~ES1370_DAC2_EN;
	outp32(d->baseIO+ES1370_REG_CONTROL, reg);

	reg = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
	reg &= ~ES1370_P2_INTR_EN;
	outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, reg);
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
//    _printf("irq=%x\n", d->pciinfo.IrqLine);
    	syscall_get_io();
	syscall_set_irq_receiver(d->pciinfo.IrqLine, SYS_MASK_INTERRUPT);
	monapi_set_irq(d->pciinfo.IrqLine, MONAPI_TRUE, MONAPI_TRUE);

	MessageInfo msg;
	uint32_t stat, result;
	while(1)
	{
		if( MonAPI::Message::receive(&msg) )
		{
			continue;
		}
		if( msg.header == MSG_INTERRUPTED )
		{
			stat = inp32(d->baseIO+ES1370_REG_STATUS);
			if( stat & 2 )
			{
				if( d->state == RUNNING )
				{
//					puts("INTERRUPTED");
//					es1370_stop_playback(d);
					result = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
					result &= ~ES1370_P2_INTR_EN;
					outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, result);
					result = es1370_buffer_setter(d);
					if( result != OK )
					{
						d->state = PAUSE;
						return;
					}
					result = inp32(d->baseIO+ES1370_REG_SERIAL_CONTROL);
					result |= ES1370_P2_INTR_EN;
					outp32(d->baseIO+ES1370_REG_SERIAL_CONTROL, result);
//					es1370_start_playback(d);

					monapi_set_irq(d->pciinfo.IrqLine, MONAPI_TRUE, MONAPI_TRUE);
				}
			}
		}
	}
}

static void es1370_notifier(void* a)
{
	struct es1370_driver *d = (struct es1370_driver*)a;
	d->stopped_callback(d->stopped_ref);
}
