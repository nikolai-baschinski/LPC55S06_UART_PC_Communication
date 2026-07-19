#include "LPC55S06.h"
#include "UART.h"

#define IOCON_PIO_FUNC1 0x01u        // Selects pin function 1
#define IOCON_PIO_SLEW_FAST 0x40u    // Fast slew rate
#define IOCON_PIO_DIGITAL_EN 0x0100u // Enables digital function
#define IOCON_PIO_MODE_PULLUP 0x20u   /*!<@brief Selects pull-up function */

void init_UART()
{
  SYSCON->FCCLKSELX[0] = SYSCON_FCCLKSEL0_SEL(2);        // clock source for UART: FRO 12 MHz
  SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL1_FC0_MASK; // enable clock for Flexcomm

  SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL1_FC0_RST(1); // set reset
  while (0u == (SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_FC0_RST_MASK));
  SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL1_FC0_RST(1); // clear reset
  while (SYSCON_PRESETCTRL1_FC0_RST_MASK == (SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_FC0_RST_MASK));

  FLEXCOMM0->PSELID = FLEXCOMM_PSELID_PERSEL(1); // choose function USART

  USART0->FIFOCFG |= USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK |
                     USART_FIFOCFG_EMPTYRX_MASK | USART_FIFOCFG_ENABLERX_MASK;

  IOCON->PIO[0][29] = IOCON_PIO_FUNC1 | IOCON_PIO_DIGITAL_EN;                         // Rx
  IOCON->PIO[0][30] = IOCON_PIO_FUNC1 | IOCON_PIO_DIGITAL_EN | IOCON_PIO_MODE_PULLUP; // Tx

  USART0->CFG = USART_CFG_ENABLE_MASK | USART_CFG_DATALEN(1); // 8N1, ENABLE first!
  USART0->OSR = 12;   // 12 MHz / (13 * 8) = 115384 Baud (+0,16 %)
  USART0->BRG = 7;
}

// non-blocking: if a byte was received, store it in *p_byte and return 1; else return 0
uint8_t UART_read_char(uint8_t* p_byte)
{
  if (USART0->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK) {
    *p_byte = (uint8_t)USART0->FIFORD; // low 8 bits = data; upper status flags are discarded
    return 1;
  }
  return 0;
}

// send a single byte, blocking until the TX FIFO has space
void UART_write_char(uint8_t byte)
{
  while (!(USART0->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK)); // wait while TX FIFO is full
  USART0->FIFOWR = byte;
}
