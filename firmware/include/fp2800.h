#ifndef _FP2800_H_
#define _FP2800_H_

#include <stdint.h>
#include <stdbool.h>

void init_fp2800(void);

void fp2800_select_col(uint8_t col);
void fp2800_set_polarity(bool high);
void fp2800_strobe(uint8_t speed, uint8_t module);

#endif
