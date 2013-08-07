OBJS = src/startup.o \
       src/isr.o \
       src/main.o\
       src/usart.o\
       src/shift.o\
       src/fp2800.o\
       src/systick.o\
       src/display.o\
       src/ringbuf.o\
       src/idle.o\
       src/drv/rcc.o\
       src/drv/usart.o\
       src/drv/gpio.o\
       src/drv/spi.o\
       src/drv/misc.o

COMPILER_PREFIX=arm-none-eabi

CFLAGS=-mthumb -mcpu=cortex-m0 -Os -g -flto -fno-common -ffreestanding -Wall -Wno-unused-function -Wextra -Werror -std=c99
LDFLAGS=-Wl,--gc-sections -flto -g -nostartfiles -mthumb -mcpu=cortex-m0

CC=$(COMPILER_PREFIX)-gcc
LD=$(COMPILER_PREFIX)-ld
OBJCOPY=$(COMPILER_PREFIX)-objcopy
GDB=$(COMPILER_PREFIX)-gdb

all: ctrl.bin

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ -Iinclude/ $<

ctrl.elf: $(OBJS)
	$(CC) -o $@ -T linker.ld $(LDFLAGS) $(OBJS) -lgcc

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJS) ctrl.elf ctrl.bin

debug: ctrl.elf
	$(GDB) -ex 'target extended localhost:4242' ctrl.elf

flash: ctrl.bin
	-st-flash write ctrl.bin 0x8000000
	st-flash write ctrl.bin 0x8000000
