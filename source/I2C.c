#include "LPC55S06.h"
#include "I2C.h"

#define IOCON_PIO_DIGITAL_EN 0x0100u  /*!<@brief Enables digital function */
#define IOCON_PIO_FUNC1 0x01u         /*!<@brief Selects pin function 1 */

#define BME280_I2C_ADDRESS 0x77
#define BME280_BURST_REGISTER 0xF7

#define I2C_STAT_MSTST_IDLE (0x0 << 1)
#define I2C_STAT_MSTST_RX   (0x1 << 1)
#define I2C_STAT_MSTST_TX   (0x2 << 1)

void init_I2C()
{
  SYSCON->FCCLKSELX[1] = SYSCON_FCCLKSEL1_SEL(2); // clock source for I2C
  SYSCON->AHBCLKCTRLSET[1] = SYSCON_AHBCLKCTRL1_FC1_MASK; // enable clock for Flexcomm
  SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL1_FC1_RST(1); // set reset
  while (0u == (SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_FC1_RST_MASK));
  SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL1_FC1_RST(1); // clear reset
  while (SYSCON_PRESETCTRL1_FC1_RST_MASK == (SYSCON->PRESETCTRLX[1] & SYSCON_PRESETCTRL1_FC1_RST_MASK));

  FLEXCOMM1->PSELID = FLEXCOMM_PSELID_PERSEL(3); // choose function I2C

  IOCON->PIO[0][13] = IOCON_PIO_FUNC1 | IOCON_PIO_DIGITAL_EN; // SDA
  IOCON->PIO[0][14] = IOCON_PIO_FUNC1 | IOCON_PIO_DIGITAL_EN; // SCL

  I2C1->CLKDIV = 5;
  I2C1->MSTTIME = (1<<0) | // low
                  (0<<4);  // high

  I2C1->CFG = I2C_CFG_MSTEN_MASK;
}

void abort() {
  int val = 0;
  val = 78;
}

uint8_t I2C_write_read(uint8_t data)
{
  uint8_t rv;

  // write
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_IDLE) abort();
  I2C1->MSTDAT = (BME280_I2C_ADDRESS << 1) | 0; // address and 0 for RWn bit (write)
  I2C1->MSTCTL = I2C_MSTCTL_MSTSTART_MASK; // send start
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_TX) abort();
  I2C1->MSTDAT = data; // register address
  I2C1->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK; // continue transaction
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_TX) abort();

  // read
  I2C1->MSTDAT = (BME280_I2C_ADDRESS << 1) | 1; // address and 1 for RWn bit (read)
  I2C1->MSTCTL = I2C_MSTCTL_MSTSTART_MASK; // send repeated start
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_RX) abort();
  rv = I2C1->MSTDAT; // read data
  I2C1->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK; // send stop (NACKs the byte)
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_IDLE) abort();

  return rv;
}

void I2C_write(uint8_t* p_data, uint32_t length)
{
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_IDLE) abort();
  I2C1->MSTDAT = (BME280_I2C_ADDRESS << 1) | 0; // address and 0 for RWn bit
  I2C1->MSTCTL = I2C_MSTCTL_MSTSTART_MASK; // send start
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_TX) abort();

  for (int i = 0; i < length; i++) {
    I2C1->MSTDAT = p_data[i]; // send data
    I2C1->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK; // continue transaction
    while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
    if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_TX) abort();
  }

  I2C1->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK; // send stop
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_IDLE) abort();
}


void I2C_read_burst(uint8_t *rx_buf, uint32_t length)
{
  // write
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_IDLE) abort();
  I2C1->MSTDAT = (BME280_I2C_ADDRESS << 1) | 0; // address and 0 for RWn bit (write)
  I2C1->MSTCTL = I2C_MSTCTL_MSTSTART_MASK; // send start
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_TX) abort();
  I2C1->MSTDAT = BME280_BURST_REGISTER;
  I2C1->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK; // continue transaction
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_TX) abort();

  // read
  I2C1->MSTDAT = (BME280_I2C_ADDRESS << 1) | 1;
  I2C1->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;

  for(uint32_t i = 0; i < length; i++) {
    while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
    if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_RX) abort();
    rx_buf[i] = I2C1->MSTDAT;
    I2C1->MSTCTL = (i < length - 1) ? I2C_MSTCTL_MSTCONTINUE_MASK : I2C_MSTCTL_MSTSTOP_MASK;
  }
  while(!(I2C1->STAT & I2C_STAT_MSTPENDING_MASK));
  if((I2C1->STAT & I2C_STAT_MSTSTATE_MASK) != I2C_STAT_MSTST_IDLE) abort();
}
