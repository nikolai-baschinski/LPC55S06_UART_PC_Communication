This is a bare-metal USART project for the NXP LPC55S06-EVK. The evaluation board is connected to a Windows 11 PC via a USB-to-UART adapter.
The firmware receives characters from the PC via the UART and echoes them back to the PC.

You can use any other terminal program. The communication settings are:

- 115200 baud
- 8 data bits
- 1 stop bit
- No parity

No RX overflow can occur in echo mode

In echo mode the MCU transmits exactly one byte for every byte it receives.
Because RX and TX run at the same baud rate, the receive rate and the send rate are identical by construction, so no backlog can build up.

Timing at 115200 baud, 8N1 (10 bits per character):
  - Character period : 10 / 115200 = 86.8 us per character
  - Echo loop body   : a few register accesses (read RXNOTEMPTY, read FIFORD, wait TXNOTFULL, write FIFOWR)  ~1-2 us  <<  86.8 us
  - RX FIFO depth    : 16 entries -> 16 x 86.8 us = 1.39 ms of slack

The loop services a character roughly 50x faster than a new one can arrive, and the 16-entry hardware FIFO absorbs any short jitter. A byte would only be lost if the loop stalled longer than 1.39 ms - which cannot happen as long as no slow processing (e.g. an LCD update) is inserted between receive and echo.

![](/Doc/Setup.bmp)

![](/Doc/HTerm.png)

![](/Doc/Tx_Rx.bmp)


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


