#ifndef UART_H_
#define UART_H_

#include "ProcessImage.h"

void init_UART();
void cyclic_UART(struct ProcessImage* p_pi);
uint8_t UART_read_char(uint8_t* p_byte);

#endif /* UART_H_ */
