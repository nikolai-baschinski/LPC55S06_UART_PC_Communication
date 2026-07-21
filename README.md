This is a bare-metal UART project for the NXP LPC55S06-EVK. The evaluation board is connected to a Windows 11 PC via a USB-to-UART adapter.

The firmware provides three functions:

1. It receives characters from the PC via UART and displays them on the Waveshare ILI9341 LCD.
2. It reads data from the BME280 sensor and displays it on the LCD.
3. It transmits the BME280 sensor data to the Windows 11 PC via UART.

The communication settings are: 115200 8N1

![](Doc/Setup.bmp)

![](Doc/HTerm.png)

| LCD      | Connector | Function     | Port | Peripheral |
|----------|---------- |------------- |------|------------|
| VCC      | J10.11    | Vcc (3.3V)   |      |            |
| GND      | J10.17    | GND          |      |            |
| DIN      | J12.1     | SPI3_MOSI    | P0_3 | SPI3       |
| CLK      | J10.16    | SPI3_CLK     | P0_6 | SPI3       |
| CS       | J10.14    | Chip Select  | P0_4 | GPIO       |
| DC       | J10.4     | Data/Command | P0_7 | GPIO       |
| RST      | J10.5     | Reset        | P0_8 | GPIO       |
| BL       | -         | Backlight    |      |            |


| BME280   | Connector | Function     | Port  | Peripheral |
|----------|-----------|--------------|-------|------------|
| VCC      | J10.11    | Vcc (3.3V)   |       |            |
| GND      | J10.17    | GND          |       |            |
| SCL      | J13.12    | Clock        | P0_14 | I2C        |
| SDA      | J13.10    | Data         | P0_13 | I2C        |
| ADDR     | J12.2     | Vcc (3.3V)   |       |            |
| CS       | J10.11    | Vcc (3.3V)   |       |            |

| Function | Connector | Connector Pin |
|----------|-----------|---------------|
| Rx       | J3        | 1             |
| Tx       | J3        | 2             |
| GND      | J3        | 3             |

Jumper JP9 must be closed.
Jumper JP12 must be open.


Hardware

LPC55S06-EVK: https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/lpcxpresso-boards/lpcxpresso-development-board-for-lpc55s0x-0x-family-of-mcus:LPC55S06-EVK

USB-UART-Adapter: https://www.az-delivery.de/en/products/ftdi-adapter-ft232rl

LCD: https://www.waveshare.com/wiki/2.4inch_LCD_Module?srsltid=AfmBOoqtv3bq-mZfPtsi2BxiewwQnIkomXrloIzpVwGw_HnrOcmvQZar

BME280: https://seengreat.com/product/207/bme280-environmental-sensor?srsltid=AfmBOorvlymsT9w0Ea-JBnftBbgADYcXMKpadnPHUyHl7X1wOO5TTgUa

Video: https://youtu.be/VZHyNFLlotg
