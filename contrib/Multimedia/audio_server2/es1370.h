#pragma once

#include <stdint.h>
#include <pci/Pci.h>
#include "audio_driver.h"

enum
{
	ES1370_VENDOR_ID	= 0x1274,
	ES1370_DEVICE_ID	= 0x5000,

	ES1370_REG_CONTROL	= 0x00,
	ES1370_REG_STATUS	= 0x04,
	ES1370_REG_UART_DATA	= 0x08,
	ES1370_REG_UART_STATUS	= 0x09,
	ES1370_REG_UART_CONTROL	= 0x09,
	ES1370_REG_UART_TEST	= 0x0a,
	ES1370_REG_MEMPAGE	= 0x0c,
	ES1370_REG_CODEC	= 0x10,
	ES1370_REG_SERIAL_CONTROL = 0x20,
	ES1370_REG_DAC1_SCOUNT	= 0x24,
	ES1370_REG_DAC2_SCOUNT	= 0x28,
	ES1370_REG_ADC_SCOUNT	= 0x2c,
	ES1370_REG_DAC1_FRAMEADR = 0x30,
	ES1370_REG_DAC1_FRAMECNT = 0x34,
	ES1370_REG_DAC2_FRAMEADR = 0x38,
	ES1370_REG_DAC2_FRAMECNT = 0x3c,
	ES1370_REG_ADC_FRAMEADR = 0x30,
	ES1370_REG_ADC_FRAMECNT = 0x34,
	ES1370_REG_PHANTOM_FRAMEADR = 0x38,
	ES1370_REG_PHANTOM_FRAMECNT = 0x3c,

	ES1370_SERR_DISABLE = (1<<0),
	ES1370_CDC_EN = (1<<1),
	ES1370_JYSTK_EN = (1<<2),
	ES1370_UART_EN = (1<<3),
	ES1370_ADC_EN = (1<<4),
	ES1370_DAC2_EN = (1<<5),
	ES1370_DAC1_EN = (1<<6),
	ES1370_BREQ = (1<<7),
	ES1370_XCTL0 = (1<<8),
	ES1370_M_CB = (1<<9),
	ES1370_CCB_INTRM = (1<<10),
	ES1370_DAC_SYNC = (1<<11),
	ES1370_WTSRSEL = (1<<12),
	ES1370_M_SBB = (1<<14),
	ES1370_MSFMTSEL = (1<<15),
	ES1370_PCLKDIV = (1<<16),
	ES1370_OPEN = (1<<29),
	ES1370_XCTL1 = (1<<30),
	ES1370_ADC_STOP = (1<<31),

	ES1370_R1_LOOP_SEL = (1<<15),
	ES1370_P2_LOOP_SEL = (1<<14),
	ES1370_P1_LOOP_SEL = (1<<13),
	ES1370_P2_PAUSE = (1<<12),
	ES1370_P1_PAUSE = (1<<11),
	ES1370_R1_INT_EN = (1<<10),
	ES1370_P2_INTR_EN = (1<<9),
	ES1370_P1_INTR_EN = (1<<8),
	ES1370_P1_SCT_RLD = (1<<7),
	ES1370_P2_DAC_SEN = (1<<6),
	ES1370_R1_S_EB = (1<<5),
	ES1370_R1_S_MB = (1<<4),
	ES1370_P2_S_EB = (1<<3),
	ES1370_P2_S_MB = (1<<2),
	ES1370_P1_S_EB = (1<<1),
	ES1370_P1_S_MB = (1<<0),

	ES1370_PAGE_DAC	= 0x0c,
	ES1370_PAGE_ADC	= 0x0d,
	ES1370_PAGE_UART= 0x0e,
	ES1370_PAGE_UART1=0x0f,
};

enum STATE
{
	PAUSE = 0,
	RUNNING,
};

extern "C" struct audio_driver *es1370_get_driver_desc();
handle_t	es1370_new(const struct audio_data_format *f);
void		es1370_delete(handle_t o);
error_t		es1370_codec_command(handle_t o, codec_command_t c, ...);
error_t		es1370_start(handle_t o);
error_t		es1370_stop(handle_t o);
error_t         es1370_regist_int_handler(handle_t o);
error_t         es1370_do_int_proc(handle_t o, MessageInfo *msg);
error_t		es1370_set_callback(handle_t o,
	error_t (*callback)(void* ref, void* buffer, size_t size, size_t *wrote), void* ref);
error_t		es1370_set_stopped_callback(handle_t o, audio_stopped_callback_t, void *ref);

