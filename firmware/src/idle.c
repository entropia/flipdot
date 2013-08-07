#include <stdint.h>

#include "display.h"
#include "usart.h"

#include "img/flipdot_url.h"
#include "idle.h"

static void display_url(void) {
	for(uint8_t x = 0; x < 112; x++)
		for(uint8_t y = 0; y < 16; y++)
			set_dot(x, y, flipdot_url[y * 112 + x]);
}

void idle(void) {
	synchronize();
	clear();

	display_url();

	while(!usart_haschar()) {
	}

	clear();
}
