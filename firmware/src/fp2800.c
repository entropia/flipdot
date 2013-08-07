#include "stm/stm32f0xx.h"
#include "stm/drv/rcc.h"
#include "stm/drv/gpio.h"

#include "systick.h"
#include "fp2800.h"

void init_fp2800(void) { // stmtaint
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

	GPIO_InitTypeDef gpio_init;

	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
	gpio_init.GPIO_Pin   = GPIO_Pin_4 |
	                       GPIO_Pin_5 |
	                       GPIO_Pin_6 |
	                       GPIO_Pin_7 |
	                       GPIO_Pin_8 |
	                       GPIO_Pin_9;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOB, &gpio_init);

	gpio_init.GPIO_Pin   = GPIO_Pin_10 |
		               GPIO_Pin_11 |
			       GPIO_Pin_12;
	GPIO_Init(GPIOC, &gpio_init);

	gpio_init.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOD, &gpio_init);
}

void fp2800_select_col(uint8_t col) {
	col = 27 - col;

	uint8_t digit = col / 7;

	GPIO_WriteBit(GPIOB, GPIO_Pin_5, digit & 2);
	GPIO_WriteBit(GPIOB, GPIO_Pin_4, digit & 1);

	uint8_t segment = (col % 7) + 1;
	GPIO_WriteBit(GPIOC, GPIO_Pin_12, segment & 4);
	GPIO_WriteBit(GPIOC, GPIO_Pin_11, segment & 2);
	GPIO_WriteBit(GPIOC, GPIO_Pin_10, segment & 1);
}

void fp2800_set_polarity(bool high) {
	GPIO_WriteBit(GPIOD, GPIO_Pin_2, high ? Bit_SET : Bit_RESET);
}

void fp2800_strobe(uint8_t speed, uint8_t module) {
	if(speed > 2)
		speed = 2;

	delay_ticks(8 >> speed);

	uint16_t pin;
	switch(module) {
		case 0:
			pin = GPIO_Pin_9;
			break;
		case 1:
			pin = GPIO_Pin_8;
			break;
		case 2:
			pin = GPIO_Pin_7;
			break;
		case 3:
			pin = GPIO_Pin_6;
			break;
		default:
			return;
	}

	GPIO_WriteBit(GPIOB, pin, Bit_SET);

	delay_ticks(4 >> speed);

	GPIO_WriteBit(GPIOB, pin, Bit_RESET);
}
