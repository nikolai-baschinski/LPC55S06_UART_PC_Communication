This is a bare-metal USART project for the NXP LPC55S06-EVK. The evaluation board is connected to a Windows 11 PC via a USB-to-UART adapter. The firmware transmits BME280 sensor data (temperature, air pressure, and humidity) to the PC whenever the values change. On the PC, the terminal program HTerm receives and displays the data..

You can use any other terminal program. The communication settings are:

- 115200 baud
- 8 data bits
- 1 stop bit
- No parity

![](Doc/Setup.bmp)

![](Doc/Frame.bmp)

![](Doc/Baudrate.bmp)

![](Doc/HTerm.png)

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

UART-Adapter: https://www.az-delivery.de/en/products/ftdi-adapter-ft232rl

BME280: https://seengreat.com/product/207/bme280-environmental-sensor?srsltid=AfmBOorvlymsT9w0Ea-JBnftBbgADYcXMKpadnPHUyHl7X1wOO5TTgUa
