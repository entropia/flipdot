#include <stdint.h>

#include "stm/stm32f0xx.h"
#include "stm/drv/rcc.h"

#include "config.h"

#include "usart.h"
#include "shift.h"
#include "fp2800.h"
#include "systick.h"
#include "display.h"
#include "idle.h"

static void init_clock(void) { // stmtaint
	// initial HSE with signal from ST-LINK (fixed to 8 MHz)
	RCC_HSEConfig(RCC_HSE_Bypass);
	RCC_WaitForHSEStartUp();

	// use HSE as source for PLL
	RCC_PREDIV1Config(RCC_PREDIV1_Div1); // no prediv
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_3); // x3 (-> 24 MHz)
	RCC_PLLCmd(ENABLE);

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		;

	// use it as the system clock
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

#define IDLE_TIMEOUT (10 * HZ)

enum cmd_state {
	INIT,
	CMD_SET, CMD_SET_GOT_X,
};

void main(void) {
	init_clock();

	init_shift();
	init_fp2800();

	init_systick();

	init_usart();

	synchronize();

	enum cmd_state state = INIT;
	uint8_t set_cmd = 0, set_x = 0, set_y = 0;
	bool verbose = false;

	while(1) {
		int16_t result = usart_getchar_timeout(IDLE_TIMEOUT);
		if(result < 0) {
			state = INIT;

			idle();
			continue;
		}

		uint8_t cmd = result;

		switch(state) {
			case INIT:
				if(cmd == '1' || cmd == '0') {
					state = CMD_SET;
					set_cmd = cmd;
				} else if(cmd == 'c')
					clear();
				else if(cmd == 's')
					synchronize();
				else if(cmd == 'f')
					usart_putc('F');
				else if(cmd == 'v')
					verbose = true;
				else if(cmd == 'q')
					verbose = false;
				else if(verbose) {
					usart_putc('U');
					usart_putnum(cmd);
					usart_putc('\n');
				}
				break;
			case CMD_SET:
				set_x = cmd;
				state = CMD_SET_GOT_X;
				break;
			case CMD_SET_GOT_X:
				set_y = cmd;
				state = INIT;

				if(set_x > WIDTH || set_y > HEIGHT) {
					if(verbose) {
						usart_putc('E');
						usart_putnum(set_x);
						usart_putc(',');
						usart_putnum(set_y);
						usart_putc('\n');
					}

					continue;
				}

				set_dot(set_x, set_y, (set_cmd == '1') ? true : false);
				break;
		}

		if(usart_error_state) {
			if(verbose) {
				usart_putc('Z');
				usart_putnum(usart_error_state);
				usart_putc('\n');
			}

			usart_error_state = 0;
		}
	}
}
