#include "lcd.h"
#include "TIMER.h"
#include "GPIO.h"
#include "SPI.h"
#include "GUI_Paint.h"
#include "ProcessImage.h"
#include "UART.h"

#define CR 0x0D
#define LF 0x0A

#define COLS 18   // characters per line   (x: 0..17)
#define ROWS 7    // lines                 (y: 0..6)

#define OFFSET_X 7
#define OFFSET_Y 12
#define CHAR_HIGH 24  // font 24 pixels
#define CHAR_WIDTH 17 // font 24 pixels

struct Cursor {
  uint8_t x;
  uint8_t y;
};

struct Cursor char_cursor = {0};

uint8_t LCD_get_X_position()
{
  return char_cursor.x;
}

uint8_t LCD_get_Y_position()
{
  return char_cursor.y;
}

static void cursor_line_down(void)
{
  char_cursor.y = (char_cursor.y + 1u == ROWS) ? 0u : char_cursor.y + 1u;
}

void LCD_process_char(uint8_t c)
{
  switch (c) {
    case CR:
      char_cursor.x = 0;
      break;

    case LF:
      cursor_line_down();
      break;

    default:
      char_cursor.x++;
      if (char_cursor.x == COLS) {   // past last column -> wrap to next line
        char_cursor.x = 0;
        cursor_line_down();
      }
      break;
  }
}

void lcd_CS_enable()
{
  GPIO_set_P0_4(0);
}

void lcd_CS_disable()
{
  GPIO_set_P0_4(1);
}

void lcd_RST_set()
{
  GPIO_set_P0_8(1);
}

void lcd_RST_reset()
{
  GPIO_set_P0_8(0);
}

void lcd_DC_set_data()
{
  GPIO_set_P0_7(1);
}

void lcd_DC_set_command()
{
  GPIO_set_P0_7(0);
}

void spi_send(uint8_t byte)
{
  SPI_send_byte(byte);
}

void lcd_send_c(uint8_t byte)
{
  lcd_DC_set_command();
  spi_send(byte);
}

void lcd_send_d(uint8_t byte)
{
  lcd_DC_set_data();
  spi_send(byte);
}

void lcd_set_window(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
  lcd_send_c(0x2A);
  lcd_send_d(Xstart >> 8);
  lcd_send_d(Xstart & 0xFF);
  lcd_send_d((Xend - 1) >> 8);
  lcd_send_d((Xend - 1) & 0xFF);

  lcd_send_c(0x2B);
  lcd_send_d(Ystart >> 8);
  lcd_send_d(Ystart & 0xFF);
  lcd_send_d((Yend - 1) >> 8);
  lcd_send_d((Yend - 1) & 0xFF);

  lcd_send_c(0x2C);
}

void lcd_send_d_word(uint16_t data)
{
  lcd_DC_set_data();
  spi_send((data>>8) & 0xff);
  spi_send(data);
}

void lcd_clear_display(uint16_t color)
{
  lcd_set_window(0, 0, LCD_2IN4_WIDTH, LCD_2IN4_HEIGHT);

  for(int i = 0; i < LCD_2IN4_WIDTH; i++) {
    for(int j = 0; j < LCD_2IN4_HEIGHT; j++) {
      lcd_send_d_word(color);
    }
  }
}

void lcd_display_image(uint8_t *image)
{
  lcd_set_window(0, 0, LCD_2IN4_WIDTH, LCD_2IN4_HEIGHT);

  for(int i = 0; i < LCD_2IN4_WIDTH; i++) {
    for(int j = 0; j < LCD_2IN4_HEIGHT; j++) {
      lcd_send_d_word(*(image+i*LCD_2IN4_WIDTH+j));
    }
  }
}

void lcd_set_cursor(uint16_t X, uint16_t Y)
{
  lcd_send_c(0x2A);
  lcd_send_d(X >> 8);
  lcd_send_d(X);
  lcd_send_d(X >> 8);
  lcd_send_d(X);

  lcd_send_c(0x2B);
  lcd_send_d(Y >> 8);
  lcd_send_d(Y);
  lcd_send_d(Y >> 8);
  lcd_send_d(Y);

  lcd_send_c(0x2C);
}

void lcd_draw_paint(uint16_t x, uint16_t y, uint16_t color)
{
  lcd_set_cursor(x, y);
  lcd_send_d_word(color);
}

void lcd_clear_window(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t color)
{
  lcd_set_window(Xstart, Ystart, Xend, Yend);
  for(int i = Ystart; i <= Yend; i++) {
    for(int j = Xstart; j <= Xend; j++) {
      lcd_send_d_word(color);
    }
  }
}

void lcd_reset()
{
  lcd_RST_set();
  delay(100);
  lcd_RST_reset();
  delay(100);
  lcd_RST_set();
  delay(100);
}

void lcd_init()
{
  lcd_send_c(0x11); // SLEEP OUT: COMMAND
  delay(120);
  lcd_send_c(0xCF); // POWER CONTROL B: COMMAND
  lcd_send_d(0x00); // POWER CONTROL B: DATA
  lcd_send_d(0xC1); // POWER CONTROL B: DATA
  lcd_send_d(0x30); // POWER CONTROL B: DATA
  lcd_send_c(0xED); // POWER-ON SEQUENCE CONTROL: COMMAND
  lcd_send_d(0x64); // SOFT START CONTROL: DATA
  lcd_send_d(0x03); // POWER-ON SEQUENCE CONTROL: DATA
  lcd_send_d(0x12); // POWER-ON SEQUENCE CONTROL: DATA
  lcd_send_d(0x81); // DDVDH ENHANCE MODE: DATA
  lcd_send_c(0xE8);
  lcd_send_d(0x85);
  lcd_send_d(0x00);
  lcd_send_d(0x79);
  lcd_send_c(0xCB);
  lcd_send_d(0x39);
  lcd_send_d(0x2C);
  lcd_send_d(0x00);
  lcd_send_d(0x34);
  lcd_send_d(0x02);
  lcd_send_c(0xF7);
  lcd_send_d(0x20);
  lcd_send_c(0xEA);
  lcd_send_d(0x00);
  lcd_send_d(0x00);
  lcd_send_c(0xC0);
  lcd_send_d(0x1D);
  lcd_send_c(0xC1);
  lcd_send_d(0x12);
  lcd_send_c(0xC5);
  lcd_send_d(0x33);
  lcd_send_d(0x3F);
  lcd_send_c(0xC7);
  lcd_send_d(0x92);
  lcd_send_c(0x3A);
  lcd_send_d(0x55);
  lcd_send_c(0x36);
  lcd_send_d(0x08);
  lcd_send_c(0xB1);
  lcd_send_d(0x00);
  lcd_send_d(0x12);
  lcd_send_c(0xB6);
  lcd_send_d(0x0A);
  lcd_send_d(0xA2);
  lcd_send_c(0x44);
  lcd_send_d(0x02);
  lcd_send_c(0xF2);
  lcd_send_d(0x00);
  lcd_send_c(0x26);
  lcd_send_d(0x01);
  lcd_send_c(0xE0);
  lcd_send_d(0x0F);
  lcd_send_d(0x22);
  lcd_send_d(0x1C);
  lcd_send_d(0x1B);
  lcd_send_d(0x08);
  lcd_send_d(0x0F);
  lcd_send_d(0x48);
  lcd_send_d(0xB8);
  lcd_send_d(0x34);
  lcd_send_d(0x05);
  lcd_send_d(0x0C);
  lcd_send_d(0x09);
  lcd_send_d(0x0F);
  lcd_send_d(0x07);
  lcd_send_d(0x00);
  lcd_send_c(0xE1);
  lcd_send_d(0x00);
  lcd_send_d(0x23);
  lcd_send_d(0x24);
  lcd_send_d(0x07);
  lcd_send_d(0x10);
  lcd_send_d(0x07);
  lcd_send_d(0x38);
  lcd_send_d(0x47);
  lcd_send_d(0x4B);
  lcd_send_d(0x0A);
  lcd_send_d(0x13);
  lcd_send_d(0x06);
  lcd_send_d(0x30);
  lcd_send_d(0x38);
  lcd_send_d(0x0F);
  lcd_send_c(0x29);
}

void init_LCD()
{
  lcd_reset();
  lcd_CS_enable();
  lcd_init();
  lcd_clear_display(WHITE);

  Paint_NewImage(LCD_2IN4_WIDTH, LCD_2IN4_HEIGHT, ROTATE_270, WHITE);
  Paint_SetClearFuntion(lcd_clear_display);
  Paint_SetDisplayFuntion(lcd_draw_paint);

  Paint_DrawLine(0, 192, 320, 192, BLACK, 2, LINE_STYLE_SOLID);

  Paint_DrawString_EN(10, 210, "T:", &Font20, WHITE, BLACK);
  Paint_DrawString_EN(120, 210, "P:", &Font20, WHITE, BLACK);
  Paint_DrawString_EN(230, 210, "H:", &Font20, WHITE, BLACK);


  lcd_CS_disable();
}

void cyclic_LCD(struct ProcessImage* p_pi)
{
  if(p_pi->bme280.temperature != p_pi->bme280_memory.temperature) {
    lcd_CS_enable();
    Paint_ClearWindows(38, 210, 38+14*4, 230, WHITE);
    Paint_DrawFloatNum(38, 210, p_pi->bme280.temperature, 1, &Font20, WHITE, BLACK);
    lcd_CS_disable();
  }

  if(p_pi->bme280.pressure != p_pi->bme280_memory.pressure) {
    lcd_CS_enable();
    Paint_ClearWindows(148, 210, 148+14*4, 230, WHITE);
    Paint_DrawNum(148, 210, p_pi->bme280.pressure, &Font20, WHITE, BLACK);
    lcd_CS_disable();
  }

  if(p_pi->bme280.humidity != p_pi->bme280_memory.humidity) {
    lcd_CS_enable();
    Paint_ClearWindows(258, 210, 258+14*3, 230, WHITE);
    Paint_DrawNum(258, 210, p_pi->bme280.humidity, &Font20, WHITE, BLACK);
    lcd_CS_disable();
  }

  p_pi->bme280_memory.temperature = p_pi->bme280.temperature;
  p_pi->bme280_memory.pressure = p_pi->bme280.pressure;
  p_pi->bme280_memory.humidity = p_pi->bme280.humidity;


  uint8_t c;
  if (UART_read_char(&c)) {
    uint8_t y = LCD_get_Y_position();
    uint8_t x = LCD_get_X_position();
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
