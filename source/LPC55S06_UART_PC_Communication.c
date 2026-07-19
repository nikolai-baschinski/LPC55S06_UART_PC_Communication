// (c) Nikolai Baschinski

#include "LPC55S06.h"
#include "UART.h"
#include "TIMER.h"
#include "GPIO.h"
#include "NVIC.h"

uint32_t cntr = 0;

int main(void)
{
  init_UART();
  init_TIMER();
  init_GPIO();
  init_NVIC();
  while(1);
  return 0;
}

void CTIMER0_IRQHandler(void)
{
  CTIMER0->IR = CTIMER_IR_MR0INT_MASK;

  if(cntr%100 == 0) {
    GPIO->NOT[0] = (1UL << 22);
  }

  if(cntr%200 == 0) {
    UART_write("Hello LPC55S06\r\n");
  }
  cntr++;
  GPIO->NOT[0] = (1UL << 9);
}
