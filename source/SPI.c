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

void SPI_transmit(unsigned char* data, int size)
{
  while (size > 0) {

    // wait till transmit FIFO is not empty
    while ((SPI3->FIFOSTAT & SPI_FIFOSTAT_TXNOTFULL_MASK) == 0);

    uint32_t ctrl = SPI_FIFOWR_LEN(7) | SPI_FIFOWR_RXIGNORE(1);

    // last byte?
    if (size == 1) {
      ctrl |= SPI_FIFOWR_EOT(1); // set "End Of Transfer
    }

    SPI3->FIFOWR = ctrl | *data;

    data++;
    size--;
  }

  // wait will everything is sent
  while ((SPI3->STAT & SPI_STAT_MSTIDLE_MASK) == 0);
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

uint8_t SPI_send_byte_receive_byte(uint8_t byte)
{
  uint32_t rx;

  // Empty RX FIFO
  SPI3->FIFOCFG |= SPI_FIFOCFG_EMPTYRX_MASK;

  // Wait until TX FIFO has space
  while (!(SPI3->FIFOSTAT & SPI_FIFOSTAT_TXNOTFULL_MASK));

  SPI3->FIFOWR = SPI_FIFOWR_LEN(15) | SPI_FIFOWR_EOT_MASK | (uint16_t)byte<<8;

  // Wait until one byte has been received
  while (!(SPI3->FIFOSTAT & SPI_FIFOSTAT_RXNOTEMPTY_MASK));

  // Read received byte
  rx = SPI3->FIFORD;

  // Wait until the SPI transfer has completed
  while (!(SPI3->STAT & SPI_STAT_MSTIDLE_MASK));

  return (uint8_t)rx;
}

uint8_t* SPI_tranceive_burst(uint8_t data)
{
  for(int i = 0; i < BME280_BURST_BUFFER_SIZE; i++) {

    // Wait until TX FIFO has space
    while (!(SPI3->FIFOSTAT & SPI_FIFOSTAT_TXNOTFULL_MASK));

    if(i == 0) {
      SPI3->FIFOWR = SPI_FIFOWR_LEN(7) | SPI_FIFOWR_EOT(1) | data;
    } else {
      SPI3->FIFOWR = SPI_FIFOWR_LEN(7) | SPI_FIFOWR_EOT(1) | 0xFF;
    }

    // Wait until one byte has been received
    while (!(SPI3->FIFOSTAT & SPI_FIFOSTAT_RXNOTEMPTY_MASK));

    // Store received byte
    burst_rcv_buffer[i] = (uint8_t)SPI3->FIFORD;
  }

  return burst_rcv_buffer;
}

void SPI_BME280_burst(uint8_t* rx_buffer)
{
  uint32_t rxRemainingBytes = BME280_BURST_BUFFER_SIZE;
  uint32_t txRemainingBytes = BME280_BURST_BUFFER_SIZE;
  uint32_t toReceiveCount = 0;
  uint32_t tmp32;

  // clear tx/rx errors and empty FIFOs
  SPI3->FIFOCFG  |= SPI_FIFOCFG_EMPTYTX_MASK | SPI_FIFOCFG_EMPTYRX_MASK;
  SPI3->FIFOSTAT |= SPI_FIFOSTAT_TXERR_MASK | SPI_FIFOSTAT_RXERR_MASK;

  while ((txRemainingBytes != 0U) || (rxRemainingBytes != 0U)) {

    // receive
    if ( ((SPI3->FIFOSTAT & SPI_FIFOSTAT_RXNOTEMPTY_MASK) != 0U) &&  // if rxFIFO is not empty
         (rxRemainingBytes != 0U) ) {                                // not all the bytes have been received yet

      *(rx_buffer++) = (uint8_t)SPI3->FIFORD;
      rxRemainingBytes--;
      toReceiveCount -= 1U;
    }

    // transmit
    if ( ((SPI3->FIFOSTAT & SPI_FIFOSTAT_TXNOTFULL_MASK) != 0U) && // txFIFO is not full
         (toReceiveCount < SPI_FIFO_DEPTH) &&                      // data to receive does not exceed FIFO depth
         (txRemainingBytes != 0U) ) {                              // not all the bytes have been sent yet

      if (txRemainingBytes == BME280_BURST_BUFFER_SIZE) {
        tmp32 = 0xF7U; // burst start
      } else {
        tmp32 = 0xFFU;
      }
      tmp32 |= SPI_FIFOWR_LEN(7); // 8-bit frame - required in EVERY word
      txRemainingBytes--;
      if (txRemainingBytes == 0U) {
        tmp32 |= SPI_FIFOWR_EOT(1);
      }
      SPI3->FIFOWR = tmp32;
      toReceiveCount += 1U;
    }
  }
}
