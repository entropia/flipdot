#ifndef _USART_H_
#define _USART_H_

#include <stdint.h>
#include <stdbool.h>

void init_usart(void);
uint8_t usart_getchar(void);
int16_t usart_getchar_timeout(uint32_t timeout);
bool usart_haschar(void);
void usart_putc(uint8_t c);
void usart_puts(const char *s);
void usart_putnum(uint8_t num);

#define USART_ERROR_FE (1<<0)
#define USART_ERROR_NE (1<<1)
#define USART_ERROR_ORE (1<<2)
extern volatile uint8_t usart_error_state;

#endif
