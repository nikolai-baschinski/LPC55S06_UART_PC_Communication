// (c) Nikolai Baschinski

#include "LPC55S06.h"
#include "UART.h"
#include "TIMER.h"
#include "GPIO.h"
#include "NVIC.h"
#include "SPI.h"
#include "lcd.h"
#include "fonts.h"

uint32_t cntr = 0;

int main(void)
{
  init_UART();
  init_TIMER();
  init_GPIO();
  init_SPI();
  init_LCD();
  init_NVIC();

  while (1) {
    uint8_t c;
    if (UART_read_char(&c)) {
      uint8_t x = LCD_get_X_position();
      uint8_t y = LCD_get_Y_position();
      LCD_process_char(c);
      if (c >= 32 && c <= 126) {
        lcd_CS_enable();
        Paint_ClearWindows(OFFSET_X + x*CHAR_WIDTH,
                           OFFSET_Y + y*CHAR_HIGH,
                           OFFSET_X + (x+1)*CHAR_WIDTH,
                           OFFSET_Y + (y+1)*CHAR_HIGH, WHITE);
        Paint_DrawChar(    OFFSET_X + x*CHAR_WIDTH,
                           OFFSET_Y + y*CHAR_HIGH, c, &Font24, WHITE, BLACK);
        lcd_CS_disable();
      }
    }
  }
  return 0;
}

void CTIMER0_IRQHandler(void)
{
  CTIMER0->IR = CTIMER_IR_MR0INT_MASK;

  if(cntr % 100 == 0) {
    GPIO->NOT[0] = (1UL << 22);
  }

  cntr++;
  GPIO->NOT[0] = (1UL << 9);
}
