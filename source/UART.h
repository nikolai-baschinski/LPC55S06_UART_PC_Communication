#ifndef UART_H_
#define UART_H_

#include "ProcessImage.h"

void init_UART();
void cyclic_UART(struct ProcessImage* p_pi);

#endif /* UART_H_ */
