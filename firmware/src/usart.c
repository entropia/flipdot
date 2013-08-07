#include <stdint.h>
#include <stdbool.h>

#include "stm/stm32f0xx.h"
#include "stm/drv/rcc.h"
#include "stm/drv/usart.h"
#include "stm/drv/gpio.h"

#include "ringbuf.h"
#include "usart.h"
#include "systick.h"

#define USART_RX_BUF_LEN 1024
#define USART_RX_BUF_XOFF_AT 300
#define USART_RX_BUF_XON_AT 800

#if USART_RX_BUF_XOFF_AT > USART_RX_BUF_XON_AT
#error "XOFF threshold must be below XON threshold"
#endif

#define USART_TX_BUF_LEN 512

static uint8_t usart_rx_buf[USART_RX_BUF_LEN];
static uint8_t usart_tx_buf[USART_TX_BUF_LEN];

static struct rb rx_buf, tx_buf;

void init_usart(void) { // stmtaint
	rb_init(&rx_buf, USART_RX_BUF_LEN, usart_rx_buf);
	rb_init(&tx_buf, USART_TX_BUF_LEN, usart_tx_buf);

	// enable USART clock (PCLK, 24 MHz)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// TX/RX are PA9/10, enable GPIOA to configure them
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	// configure TX / RX
	GPIO_InitTypeDef gpio_init;

	gpio_init.GPIO_Mode  = GPIO_Mode_AF;
	gpio_init.GPIO_Pin   = GPIO_Pin_9;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio_init);

	gpio_init.GPIO_Pin   = GPIO_Pin_10;
	gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_init);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

	// initialize usart
	USART_InitTypeDef usart_init;
	USART_StructInit(&usart_init);
	usart_init.USART_BaudRate = 115200;

	USART_Init(USART1, &usart_init);

	NVIC_EnableIRQ(USART1_IRQn);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_NE, ENABLE);
	USART_ITConfig(USART1, USART_IT_FE, ENABLE);
	USART_ITConfig(USART1, USART_IT_PE, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

void usart_putc(uint8_t c) {
	while(rb_write_count(&tx_buf) == 0)
		;

	rb_produce(&tx_buf, c);

	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void usart_putnum(uint8_t n) {
	char buf[5];

	buf[4] = 0;

	for(int8_t idx = 3; idx >= 0; idx--) {
		buf[idx] = '0' + (n % 10);
		n /= 10;
	}

	usart_puts(buf);
}

void usart_puts(const char *s) {
	while(*s)
		usart_putc(*s++);
}

#define XON 0x11
#define XOFF 0x13

static volatile bool xoff = false;

static uint8_t _usart_getchar(void) {
	uint8_t c = 0;

	rb_consume(&rx_buf, &c);

	if(xoff && rb_write_count(&rx_buf) > USART_RX_BUF_XON_AT) {
		usart_putc(XON);
		xoff = false;
	}

	return c;
}

bool usart_haschar(void) {
	return rb_read_count(&rx_buf);
}

uint8_t usart_getchar(void) {
	while(!usart_haschar())
		;

	return _usart_getchar();
}

int16_t usart_getchar_timeout(uint32_t timeout) {
	uint32_t timeout_ticks = ticks + timeout;

	while(!usart_haschar() && time_before(ticks, timeout_ticks))
		;

	if(!time_before(ticks, timeout_ticks))
		return -1;

	return _usart_getchar();
}

static volatile uint8_t push = 0;
volatile uint8_t usart_error_state = 0;

void __attribute__((interrupt("IRQ"))) usart1_isr(void) {
	if(USART_GetITStatus(USART1, USART_IT_TXE)) {
		if(push) {
			USART_SendData(USART1, push);
			push = 0;
		} else {
			if(rb_read_count(&tx_buf)) {
				uint8_t c;

				rb_consume(&tx_buf, &c);
				USART_SendData(USART1, c);
			} else {
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
				USART_ClearITPendingBit(USART1, USART_IT_TXE);
			}
		}
	}

	if(USART_GetITStatus(USART1, USART_IT_RXNE)) {
		uint8_t c = USART_ReceiveData(USART1);

		rb_produce(&rx_buf, c);

		if(!xoff && rb_write_count(&rx_buf) <= USART_RX_BUF_XOFF_AT) {
			if(!push) {
				push = XOFF;
				xoff = true;
			}
		}
	}

	if(USART_GetITStatus(USART1, USART_IT_FE)) {
		usart_error_state |= USART_ERROR_FE;
		USART_ClearITPendingBit(USART1, USART_IT_FE);
	}

	if(USART_GetITStatus(USART1, USART_IT_NE)) {
		usart_error_state |= USART_ERROR_NE;
		USART_ClearITPendingBit(USART1, USART_IT_NE);
	}

	if(USART_GetITStatus(USART1, USART_IT_ORE)) {
		usart_error_state |= USART_ERROR_ORE;
		USART_ClearITPendingBit(USART1, USART_IT_ORE);
	}

	if(push)
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

}
