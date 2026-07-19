#include "LPC55S06.h"

void init_NVIC(void)
{
  // Set timer interrupt priority
  NVIC_SetPriority(CTIMER0_IRQn, 5);

  // Clear a pending interrupt
  NVIC_ClearPendingIRQ(CTIMER0_IRQn);

  // Enable timer interrupt
  NVIC->ISER[0] = (1UL << (uint32_t)CTIMER0_IRQn); // Enable CTIMER0 interrupt
}
