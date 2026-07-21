#ifndef LCD_H_
#define LCD_H_

#include "LPC55S06.h"
#include "GUI_Paint.h"

#define OFFSET_X 7
#define OFFSET_Y 12
#define CHAR_HIGH 24
#define CHAR_WIDTH 17

void init_LCD();
uint8_t LCD_get_X_position();
uint8_t LCD_get_Y_position();
void LCD_process_char(uint8_t c);

void lcd_CS_enable();
void lcd_CS_disable();

#endif /* LCD_H_ */
