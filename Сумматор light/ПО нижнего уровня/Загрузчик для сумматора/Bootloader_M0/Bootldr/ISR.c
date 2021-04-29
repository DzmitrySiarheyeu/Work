#include <stdint.h>

__asm void WAKEUP_IRQHandler(void)
{
	ldr r0, =0x1070
	ldr r0, [r0] 
	mov pc, r0
}

__asm void CAN_IRQHandler(void)
{
	ldr r0, =0x1074 
	ldr r0, [r0] 
	mov pc, r0
}

__asm void SSP1_IRQHandler(void)
{
	ldr r0, =0x1078 
	ldr r0, [r0] 
	mov pc, r0
}

__asm void I2C_IRQHandler(void)
{
	ldr r0, =0x107C 
	ldr r0, [r0] 
	mov pc, r0
}

__asm void TIMER16_0_IRQHandler(void)
{
	ldr r0, =0x1080 
	ldr r0, [r0] 
	mov pc, r0
}


__asm void TIMER16_1_IRQHandler(void)
{
	ldr r0, =0x1084
	ldr r0, [r0] 
	mov pc, r0
}

__asm void TIMER32_0_IRQHandler(void)
{
	ldr r0, =0x1088 
	ldr r0, [r0] 
	mov pc, r0
}

__asm void TIMER32_1_IRQHandler(void)
{
	ldr r0, =0x108C
	ldr r0, [r0] 
	mov pc, r0
}

__asm void SSP0_IRQHandler(void)
{
	ldr r0, =0x1090
	ldr r0, [r0] 
	mov pc, r0
}
__asm void UART_IRQHandler(void)
{
	ldr r0, =0x1094
	ldr r0, [r0] 
	mov pc, r0
}
__asm void USB_IRQHandler(void)
{
	ldr r0, =0x1098
	ldr r0, [r0] 
	mov pc, r0
}
__asm void USB_FIQHandler(void)
{
	ldr r0, =0x109C
	ldr r0, [r0] 
	mov pc, r0
}
__asm void ADC_IRQHandler(void)
{
	ldr r0, =0x10A0
	ldr r0, [r0] 
	mov pc, r0
}
__asm void WDT_IRQHandler(void)
{
	ldr r0, =0x10A4
	ldr r0, [r0] 
	mov pc, r0
}
__asm void BOD_IRQHandler(void)
{
	ldr r0, =0x10A8
	ldr r0, [r0] 
	mov pc, r0
}
__asm void FMC_IRQHandler(void)
{
	ldr r0, =0x10AC
	ldr r0, [r0] 
	mov pc, r0
}
__asm void PIOINT3_IRQHandler(void)
{
	ldr r0, =0x10B0
	ldr r0, [r0] 
	mov pc, r0
}
__asm void PIOINT2_IRQHandler(void)
{
	ldr r0, =0x10B4
	ldr r0, [r0] 
	mov pc, r0
}

__asm void PIOINT1_IRQHandler(void)
{
	ldr r0, =0x10B8
	ldr r0, [r0] 
	mov pc, r0
}

__asm void PIOINT0_IRQHandler(void)
{
	ldr r0, =0x10BC
	ldr r0, [r0] 
	mov pc, r0
}




