#include "LPC55S06.h"
#include "GPIO.h"

void init_GPIO()
{
  SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL0_GPIO0_MASK | SYSCON_AHBCLKCTRL0_IOCON_MASK;

  // GPIO Port 0, Pin 22 (LED D4)
  IOCON->PIO[0][22] = IOCON_PIO_DIGIMODE(1);
  GPIO->DIRSET[0] = (1UL << 22);

  // GPIO Port 0, Pin 9 (test pin)
  IOCON->PIO[0][9] = IOCON_PIO_DIGIMODE(1);
  GPIO->DIRSET[0] = (1UL << 9);

  // GPIO Port 0 , Pin 4 (CS - Chip Select for LCD)
  IOCON->PIO[0][4] = IOCON_PIO_DIGIMODE(1);
  GPIO->SET[0] = (1UL << 4);
  GPIO->DIRSET[0] = (1UL << 4);

  // GPIO Port 0, Pin 7 (DC data/command for LCD)
  IOCON->PIO[0][7] = IOCON_PIO_DIGIMODE(1);
  GPIO->DIRSET[0] = (1UL << 7);

  // GPIO Port 0, Pin 8 (RST - Rest for LCD)
  IOCON->PIO[0][8] = IOCON_PIO_DIGIMODE(1);
  GPIO->SET[0] = (1UL << 8);
  GPIO->DIRSET[0] = (1UL << 8);
}

void GPIO_set_P0_7(uint32_t value)
{
  if(value)
    GPIO->SET[0] = (1UL << 7);
  else
    GPIO->CLR[0] = (1UL << 7);
}

void GPIO_set_P0_8(uint32_t value)
{
  if(value)
    GPIO->SET[0] = (1UL << 8);
  else
    GPIO->CLR[0] = (1UL << 8);
}

void GPIO_set_P0_4(uint32_t value)
{
  if (value)
    GPIO->SET[0] = (1UL << 4);
  else
    GPIO->CLR[0] = (1UL << 4);
}
