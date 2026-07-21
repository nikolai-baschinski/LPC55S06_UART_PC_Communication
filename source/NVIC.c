#include "LPC55S06.h"

void init_NVIC(void)
{
  // Set timer interrupt priority
  NVIC_SetPriority(CTIMER0_IRQn, 5);
  NVIC_SetPriority(FLEXCOMM0_IRQn, 2); // higher priority

  // Clear a pending interrupt
  NVIC_ClearPendingIRQ(CTIMER0_IRQn);
  NVIC_ClearPendingIRQ(FLEXCOMM0_IRQn);

  // Enable interrupts
  NVIC->ISER[0] |= (1UL << (uint32_t)CTIMER0_IRQn); // Enable CTIMER0 interrupt
  NVIC->ISER[0] |= (1UL << (uint32_t)FLEXCOMM0_IRQn);// Enable USART interrupt
}
