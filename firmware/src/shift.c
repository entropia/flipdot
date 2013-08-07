#include <stdint.h>
#include <stdbool.h>

#include "stm/stm32f0xx.h"
#include "stm/drv/rcc.h"
#include "stm/drv/spi.h"
#include "stm/drv/gpio.h"

#include "shift.h"

// configure SPI2 for shift registers
static void init_spi(void) { // stmtaint
	// enable SPI clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	// MOSI/SCK are PB15/PB13, enable GPIOB to configure them
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	// configure SCK / MOSI
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Mode  = GPIO_Mode_AF;
	gpio_init.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
	gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOB, &gpio_init);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_0);

	// initialize spi
	SPI_InitTypeDef spi_init;
	SPI_StructInit(&spi_init);

	spi_init.SPI_Direction         = SPI_Direction_1Line_Tx;
	spi_init.SPI_Mode              = SPI_Mode_Master;
	spi_init.SPI_DataSize          = SPI_DataSize_16b;
	spi_init.SPI_CPOL              = SPI_CPOL_Low;
	spi_init.SPI_CPHA              = SPI_CPHA_1Edge;
	spi_init.SPI_NSS               = SPI_NSS_Soft;
	spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // 1.5 MHz
	spi_init.SPI_FirstBit          = SPI_FirstBit_MSB;

	SPI_Init(SPI2, &spi_init);
	SPI_Cmd(SPI2, ENABLE);
}

// strobe is PA12
static void init_strobe(void) {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpio_init;

	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
	gpio_init.GPIO_Pin   = GPIO_Pin_12;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &gpio_init);
}

void init_shift(void) {
	init_spi();
	init_strobe();

	shift_clear();
}

static void shift_strobe(void) {
	GPIO_SetBits(GPIOA, GPIO_Pin_12);

	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

static void shift_wait_spi_avail(void) {
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
		;
}

static void shift_transmit(uint16_t upper, uint16_t lower) {
	shift_wait_spi_avail();
	SPI_I2S_SendData16(SPI2, upper);

	shift_wait_spi_avail();
	SPI_I2S_SendData16(SPI2, lower);

	shift_wait_spi_avail();
	shift_strobe();
}

void shift_clear(void) {
	shift_transmit(0x0000, 0x0000);
}

void shift_set(uint8_t row, bool high) {
	uint16_t upper, lower;

	upper = lower = 0x0000;

	if(high)
		upper = 1 << (15 - row);
	else
		lower = 1 << row;

	shift_transmit(upper, lower);
}
