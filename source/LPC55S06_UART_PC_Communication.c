// (c) Nikolai Baschinski

#include "LPC55S06.h"
#include "UART.h"
#include "TIMER.h"
#include "GPIO.h"
#include "NVIC.h"
#include "I2C.h"
#include "bme.h"
#include "ProcessImage.h"

struct ProcessImage pi={0};

int main(void)
{
  init_UART();
  init_TIMER();
  init_I2C();
  init_GPIO();
  init_BME();
  init_NVIC();
  while(1) {
    cyclic_BME(&pi.bme280);
    cyclic_UART(&pi);
    delay(1000);
  }
  return 0;
}

void CTIMER0_IRQHandler(void)
{
  CTIMER0->IR = CTIMER_IR_MR0INT_MASK;

  if(pi.cntr_10ms%100 == 0) {
    GPIO->NOT[0] = (1UL << 22);
  }

  pi.cntr_10ms++;
  GPIO->NOT[0] = (1UL << 9);
}
