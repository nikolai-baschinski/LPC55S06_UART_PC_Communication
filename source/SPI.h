#ifndef SPI_H_
#define SPI_H_

#define BME280_BURST_BUFFER_SIZE 9U

void init_SPI();
void SPI_send_byte(uint8_t byte);

#endif /* SPI_H_ */
