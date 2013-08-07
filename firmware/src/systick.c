#include <stdint.h>

#include "stm/drv/misc.h"

#include "systick.h"

volatile uint32_t ticks = 0;

void init_systick(void) {
	SysTick_Config(24 * 1000 * 1000 / HZ); // Tick every 250us
}

void delay_ms(uint16_t ms) {
	delay_ticks(ms * (HZ / 1000));
}

void delay_ticks(uint16_t cnt) {
	uint32_t timeout = ticks + cnt;

	while(time_before(ticks, timeout))
		;
}

void __attribute__((interrupt("IRQ"))) systick(void) {
	ticks++;
}
