#ifndef UART_H_
#define UART_H_

void init_UART();
void    UART_write_char(uint8_t byte);
uint8_t UART_read_char(uint8_t* p_byte);

#endif /* UART_H_ */
