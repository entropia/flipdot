#include <stdint.h>
#include <stdbool.h>

#include "ringbuf.h"

void rb_init(struct rb *buf, uint32_t size, uint8_t *storage) {
	buf->data = storage;
	buf->size = size;

	buf->r = buf->w = 0;
}

uint32_t rb_read_count(struct rb *buf) {
	uint32_t w, r;

	w = buf->w;
	r = buf->r;

	if(w > r)
		return w - r;
	else
		return (w - r + buf->size) % buf->size;
}

uint32_t rb_write_count(struct rb *buf) {
	uint32_t w, r;

	w = buf->w;
	r = buf->r;

	if(w > r)
		return ((r - w + buf->size) % buf->size) - 1;
	else if(w < r)
		return r - w - 1;
	else
		return buf->size - 1;
}

bool rb_produce(struct rb *buf, uint8_t data) {
	if(rb_write_count(buf) == 0)
		return false;

	buf->data[buf->w] = data;
	buf->w = (buf->w + 1) % buf->size;

	return true;
}

bool rb_consume(struct rb *buf, uint8_t *data) {
	if(rb_read_count(buf) == 0)
		return false;

	*data = buf->data[buf->r];
	buf->r = (buf->r + 1) % buf->size;

	return true;
}
