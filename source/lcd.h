#ifndef LCD_H_
#define LCD_H_

#include "LPC55S06.h"
#include "ProcessImage.h"

void init_LCD();
void cyclic_LCD(struct ProcessImage* p_pi);

#endif /* LCD_H_ */
