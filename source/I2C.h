#ifndef I2C_H_
#define I2C_H_

#define MAX_RECV_BURST 8

void init_I2C();
uint8_t I2C_write_read(uint8_t data);
void I2C_write(uint8_t* p_data, uint32_t length);
void I2C_read_burst(uint8_t *rx_buf, uint32_t length);

#endif /* I2C_H_ */
