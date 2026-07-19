#include "LPC55S06.h"
#include "TIMER.h"

void init_CTIMER1(void)
{
  // Clock for CTIMER1
  SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL1_TIMER1_MASK;

  // Reset
  SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL1_TIMER1_RST_MASK;
  while ((SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_TIMER1_RST_MASK) == 0);

  SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL1_TIMER1_RST_MASK;
  while (SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_TIMER1_RST_MASK);

  // FRO96 as Timer-Clock
  SYSCON->CTIMERCLKSELX[1] = SYSCON_CTIMERCLKSEL1_SEL(3);

  CTIMER1->PR = 959; // 96 MHz / (959 + 1) = 100 kHz
  CTIMER1->TC = 0;
  CTIMER1->PC = 0;
  CTIMER1->TCR = CTIMER_TCR_CEN_MASK; // enable timer 1
}

void init_CTIMER0(void)
{
  SYSCON->CTIMERCLKSELX[0] = SYSCON_CTIMERCLKSEL0_SEL(3); // select FRO 96 MHz as source for timer 0
  SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL1_TIMER0_MASK; // enable clock for timer 0
  SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL1_TIMER0_RST(1); // reset peripheral

  CTIMER0->PR = 95; // prescaler
  CTIMER0->MR[0] = 10000; // set match value for 10 ms
  CTIMER0->MCR = CTIMER_MCR_MR0I_MASK | CTIMER_MCR_MR0R_MASK; // interrupt is generated when MR0 matches the value in the TC
  CTIMER0->TC = 0; // set timer counter to 0
  CTIMER0->PC = 0; // set prescale counter to 0
  CTIMER0->IR = CTIMER_IR_MR0INT_MASK;

  CTIMER0->TCR = 1; // enable timer 0
}

void init_TIMER()
{
  init_CTIMER0();
  init_CTIMER1();
}

void delay(uint32_t ms)
{
  CTIMER1->TC = 0;
  while (CTIMER1->TC < (ms * 100));
}
