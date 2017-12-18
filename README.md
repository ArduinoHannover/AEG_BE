# AEG BE LCD Driver

This is an Adafruit GFX based LCD driver for AEG (BMG|MIS) LCDs containing M7005 or M6007 driver ICs, primary for BE10 and BE11. Other display types are not tested.

## Wiring

The displays have two connectors at each side. On on the top left and one on the bottom right. The following pinout starts from the inner (LCD) to outer:

| Pin | Direction | Function         | Ratings
| --- | --------- | ---------------- | ---
| 1   | in        | Data             |
| 2   | pwr       | V<sub>CC</sub>   | 4.5 - 5.5 V
| 3   | pwr       | V<sub>DD</sub>   | 4.5 - 12 V
| 4   | pwr       | GND              |
| 5   | in        | Reset (Enable)   |
| 6   | in        | Switch frequency | 30 - 100 Hz
| 7   | in        | Load (Latch)     |
| 8   | in        | Clock            | ≤ 300 kHz
| 9   | out       | Data             |

3.6 V ≤ HIGH ≤ 5.8 V

-0.3 V ≤ LOW ≤ 0.8 V

### Arduino

This library is mainly for AVRs with 3 Timers (ATmega168/328) of which PD3 (D3) is used for the 61 Hz LCD frequency. The MOSI (11) and SCK (13) Pins are used by the SPI library. Enable and latch can be chosen freely.

### Subclasses

There are the classes `AEG_BE10` and `AEG_BE11` which have the appropriate params for the constructors set. 