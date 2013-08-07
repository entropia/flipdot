#include "startup.h"

static void init_memory(void) {
	// copy data
	char *src = &_etext;
	char *dst = &_data;

	while(dst < &_edata) 
		*dst++ = *src++;

	// clear bss
	for(dst = &_bstart; dst < &_bend; dst++)
		*dst = 0;
}

void main(void);

void startup(void) {
	init_memory();
	
	main();

	while(1);
}
