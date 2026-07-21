This is a bare-metal USART project for the NXP LPC55S06-EVK. The evaluation board is connected to a Windows 11 PC via a USB-to-UART adapter.
The firmware receives characters from the PC via the UART and displays them on the Waveshare ILI9341 LCD.

The communication settings are: 115200 8N1

![](Doc/Setup.bmp)

![](Doc/LCD.bmp)

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