#include <stdint.h>

#include "startup.h"
#include "systick.h"

static void unexpected_interrupt(void);
void systick(void);
void usart1_isr(void);

void (*nvic_table[])() __attribute__((section(".nvic_table"))) = {
	(void (*)()) &_estack,
        startup,		//-6: Reset (0x00000004),
        unexpected_interrupt,	//-5: NMI (0x00000008),
        unexpected_interrupt,	//-4: HardFault (0x0000000C),
        unexpected_interrupt,	//XX: unused (0x00000010),
        unexpected_interrupt,	//XX: unused (0x00000014),
        unexpected_interrupt,	//XX: unused (0x00000018),
        unexpected_interrupt,	//XX: unused (0x0000001C),
        unexpected_interrupt,	//XX: unused (0x00000020),
        unexpected_interrupt,	//XX: unused (0x00000024),
        unexpected_interrupt,	//XX: unused (0x00000028),
        unexpected_interrupt,	//-3: SVCall (0x0000002C),
        unexpected_interrupt,	//XX: unused (0x00000030),
        unexpected_interrupt,	//XX: unused (0x00000034),
        unexpected_interrupt,	//-2: PendSV (0x00000038),
        systick,		//-1: SysTick (0x0000003C),
        unexpected_interrupt,	// 0: WWDG (0x00000040),
        unexpected_interrupt,	// 1: PVD (0x00000044),
        unexpected_interrupt,	// 2: RTC (0x00000048),
        unexpected_interrupt,	// 3: FLASH (0x0000004C),
        unexpected_interrupt,	// 4: RCC (0x00000050),
        unexpected_interrupt,	// 5: EXTI0_1 (0x00000054),
        unexpected_interrupt,	// 6: EXTI2_3 (0x00000058),
        unexpected_interrupt,	// 7: EXTI4_15 (0x0000005C),
        unexpected_interrupt,	// 8: TSC (0x00000060),
        unexpected_interrupt,	// 9: DMA_CH1 (0x00000064),
        unexpected_interrupt,	//10: DMA_CH2_3 (0x00000068),
        unexpected_interrupt,	//11: DMA_CH4_5 (0x0000006C),
        unexpected_interrupt,	//12: ADC_COMP (0x00000070),
        unexpected_interrupt,	//13: TIM1_BRK_UP_TRG_COM (0x00000074),
        unexpected_interrupt,	//14: TIM1_CC (0x00000078),
        unexpected_interrupt,	//15: TIM2 (0x0000007C),
        unexpected_interrupt,	//16: TIM3 (0x00000080),
        unexpected_interrupt,	//17: TIM6_DAC (0x00000084),
        unexpected_interrupt,	//18: reserved (0x00000088),
        unexpected_interrupt,	//19: TIM14 (0x0000008C),
        unexpected_interrupt,	//20: TIM15 (0x00000090),
        unexpected_interrupt,	//21: TIM16 (0x00000094),
        unexpected_interrupt,	//22: TIM17 (0x00000098),
        unexpected_interrupt,	//23: I2C1 (0x0000009C),
        unexpected_interrupt,	//24: I2C2 (0x000000A0),
        unexpected_interrupt,	//25: SPI1 (0x000000A4),
        unexpected_interrupt,	//26: SPI2 (0x000000A8),
        usart1_isr,		//27: USART1 (0x000000AC),
        unexpected_interrupt,	//28: USART2 (0x000000B0),
        unexpected_interrupt,	//29: reserved (0x000000B4),
        unexpected_interrupt,	//30: CEC (0x000000B8),
        unexpected_interrupt,	//31: reserved (0x000000BC),
};

static void __attribute__((interrupt("IRQ"))) unexpected_interrupt(void) {
	while(1);
}
