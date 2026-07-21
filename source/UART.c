#include "LPC55S06.h"
#include "UART.h"
#include "stdio.h"

#define IOCON_PIO_FUNC1 0x01u        // Selects pin function 1
#define IOCON_PIO_SLEW_FAST 0x40u    // Fast slew rate
#define IOCON_PIO_DIGITAL_EN 0x0100u // Enables digital function
#define IOCON_PIO_MODE_PULLUP 0x20u   /*!<@brief Selects pull-up function */

#define RX_BUF_SIZE 256                       // must be a power of two
static volatile uint8_t  rx_buf[RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;         // written by the ISR only
static volatile uint16_t rx_tail = 0;         // written by the main loop only

void FLEXCOMM0_IRQHandler(void)
{
  while (USART0->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK) {
    uint8_t b = (uint8_t)USART0->FIFORD;

    uint16_t next = rx_head + 1u;
    if (next >= RX_BUF_SIZE) {
      next = 0;
    }

    if (next != rx_tail) {    // drop the byte if the ring buffer is full
      rx_buf[rx_head] = b;
      rx_head = next;
    }
  }
}

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

  // RX interrupt: fire as soon as a single byte sits in the RX FIFO
  USART0->FIFOTRIG = (USART0->FIFOTRIG & ~USART_FIFOTRIG_RXLVL_MASK) // RXLVL = 0 -> one entry
                     | USART_FIFOTRIG_RXLVLENA_MASK;
  USART0->FIFOINTENSET = USART_FIFOINTENSET_RXLVL_MASK;
}

uint8_t UART_read_char(uint8_t* p_byte)
{
  if (rx_head == rx_tail) {
    return 0;                 // ring buffer empty
  }

  *p_byte = rx_buf[rx_tail];

  uint16_t next = rx_tail + 1u;
  if (next >= RX_BUF_SIZE) {
    next = 0;
  }
  rx_tail = next;

  return 1;
}

// send a single byte, blocking until the TX FIFO has space
void UART_write_char(uint8_t byte)
{
  while (!(USART0->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK)); // wait while TX FIFO is full
  USART0->FIFOWR = byte;
}

void UART_write(const char* p_text)
{
  while (*p_text != '\0') {
    while ((USART0->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK) == 0); // wait for space in TX FIFO
    USART0->FIFOWR = *p_text++;
  }
}

void cyclic_UART(struct ProcessImage* p_pi)
{
  char str[32];

  if(p_pi->bme280.temperature != p_pi->bme280_memory.temperature ||
     p_pi->bme280.pressure != p_pi->bme280_memory.pressure ||
     p_pi->bme280.humidity != p_pi->bme280_memory.humidity) {
    sprintf(str, "T=%.1f;P=%u;H=%u\r\n", p_pi->bme280.temperature, p_pi->bme280.pressure, p_pi->bme280.humidity);
    UART_write(str);
  }
}
