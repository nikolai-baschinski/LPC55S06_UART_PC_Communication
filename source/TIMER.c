#include "LPC55S06.h"
#include "TIMER.h"

void init_TIMER()
{
  SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL1_TIMER0_MASK; // enable clock for timer 0
  SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL1_TIMER0_RST_MASK; // reset peripheral
  SYSCON->CTIMERCLKSELX[0] = 3; // select FRO 96 MHz as source for timer 0
  CTIMER0->PR = 95; // prescaler
  CTIMER0->MR[0] = 10000; // set match value for 10 ms
  CTIMER0->MCR = (1 << 0) | (1 << 1); // interrupt is generated when MR0 matches the value in the TC
  NVIC->ISER[0] |= (1UL << (uint32_t)CTIMER0_IRQn); // Enable CTIMER0 interrupt
  CTIMER0->TC = 0; // set timer counter to 0
  CTIMER0->PC = 0; // set prescale counter to 0
  CTIMER0->TCR = 1; // enable timer 0
}
