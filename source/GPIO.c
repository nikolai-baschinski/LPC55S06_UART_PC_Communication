#include "LPC55S06.h"
#include "GPIO.h"

void init_GPIO()
{
  SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL0_GPIO0_MASK | SYSCON_AHBCLKCTRL0_IOCON_MASK;

  // GPIO Port 0, Pin 22 (LED D4)
  IOCON->PIO[0][22] = IOCON_PIO_DIGIMODE(1);
  GPIO->DIRSET[0] = (1UL << 22);

  // GPIO Port 0, Pin 9
  IOCON->PIO[0][9] = 0;
  GPIO->DIRSET[0] = (1UL << 9);
  IOCON->PIO[0][9] = IOCON_PIO_DIGIMODE(1);

  // UM: Once the pins are configured, the IOCON clock can be disabled in order to conserve power.
  SYSCON->AHBCLKCTRLCLR[0] = SYSCON_AHBCLKCTRL0_IOCON_MASK;
}
