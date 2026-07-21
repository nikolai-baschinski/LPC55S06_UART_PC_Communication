#include "LPC55S06.h"
#include "SPI.h"

#define IOCON_PIO_DIGITAL_EN 0x0100u  /*!<@brief Enables digital function */
#define IOCON_PIO_FUNC1 0x01u         /*!<@brief Selects pin function 1 */
#define IOCON_PIO_FUNC8 0x08u         /*!<@brief Selects pin function 8 */
#define IOCON_PIO_INV_DISABLED 0x00u  /*!<@brief Input function is not inverted */
#define IOCON_PIO_MODE_INACT 0x00u    /*!<@brief No addition pin function */
#define IOCON_PIO_MODE_PULLUP 0x20u   /*!<@brief Selects pull-up function */
#define IOCON_PIO_OPENDRAIN_DI 0x00u  /*!<@brief Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD 0x00u /*!<@brief Standard mode, output slew rate control is enabled */
#define IOCON_PIO_SLEW_FAST 0x40u
#define IOCON_PIO_OPEN_DRAIN_ENABLED 0x200u

uint8_t burst_rcv_buffer[BME280_BURST_BUFFER_SIZE]= {0};

#define SPI_FIFO_DEPTH 8U // LPC55S06 FLEXCOMM SPI, see SPI3->FIFOSIZE / UM11424

void init_SPI()
{
  SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL1_FC3_MASK; // enable clock for Flexcomm
  SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL1_FC3_RST(1); // set reset
  while (0u == (SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_FC3_RST_MASK));
  SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL1_FC3_RST(1); // clear reset
  while (SYSCON_PRESETCTRL1_FC3_RST_MASK == (SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_FC3_RST_MASK));

  FLEXCOMM3->PSELID = FLEXCOMM_PSELID_PERSEL(2); // choose function SPI
  SYSCON->FCCLKSELX[3] = SYSCON_FCCLKSEL3_SEL(2); // clock source for SPI

  // MISO
  IOCON->PIO[0][2] = IOCON_PIO_FUNC1 | IOCON_PIO_SLEW_FAST | IOCON_PIO_DIGITAL_EN;
  // MOSI
  IOCON->PIO[0][3] = IOCON_PIO_FUNC1 | IOCON_PIO_SLEW_FAST | IOCON_PIO_DIGITAL_EN;
  // CLK
  IOCON->PIO[0][6] = IOCON_PIO_FUNC1 | IOCON_PIO_SLEW_FAST | IOCON_PIO_DIGITAL_EN;

  SPI3->FIFOCFG |= SPI_FIFOCFG_ENABLETX(0) | SPI_FIFOCFG_ENABLERX(0); // Disable FIFO TX/RX
  SPI3->DIV = 2; // SPI prescalor
  SPI3->FIFOCFG |= SPI_FIFOCFG_EMPTYTX_MASK | SPI_FIFOCFG_EMPTYRX_MASK; // Empty the buffers
  SPI3->FIFOCFG |= SPI_FIFOCFG_ENABLETX(1) | SPI_FIFOCFG_ENABLERX(1); // Enable FIFO TX/RX
  SPI3->CFG = SPI_CFG_MASTER(1) | SPI_CFG_ENABLE(1);
}

void SPI_send_byte(uint8_t byte)
{
  while ((SPI3->FIFOSTAT & SPI_FIFOSTAT_TXNOTFULL_MASK) == 0);
  SPI3->FIFOWR = SPI_FIFOWR_LEN(7)
               | SPI_FIFOWR_EOT(1)
               | SPI_FIFOWR_RXIGNORE(1)
               | byte;
  while ((SPI3->STAT & SPI_STAT_MSTIDLE_MASK) == 0);
}
