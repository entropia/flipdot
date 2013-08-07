#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include <stdint.h>
#include <stdbool.h>

struct rb {
	volatile uint8_t *data; //FIXME
	uint32_t size;

	volatile uint32_t r, w;
};

void rb_init(struct rb *buf, uint32_t size, uint8_t *storage);
uint32_t rb_read_count(struct rb *buf);
uint32_t rb_write_count(struct rb *buf);
bool rb_produce(struct rb *buf, uint8_t data);
bool rb_consume(struct rb *buf, uint8_t *data);

#endif
