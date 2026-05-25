# T-Deck Pro Max Pin Map (V0.1 / 2025-09-11)
> This document is organized from `hardware/T-Deck Pro Max V0.1 25-09-11/T-Deck Pro Max V0.1 25-09-11.pdf`, and cross-checked against the current repository macros in `lib/TDeckMaxBoard/src/TDeckMaxBoard.h`, `readme.md`, and `readme_cn.md`.
> It is grouped by module. Some schematic net names are labeled from the peripheral's point of view, such as `GPS_TX`, `7682_RXD`, and `I2S_DSDIN`. Where that matters, both the schematic name and the repo macro are noted.

## 1. MCU And Shared Buses

| Function | Current Macro / Preferred Name | GPIO | Source | Notes |
| --- | --- | --- | --- | --- |
| Shared I2C SDA | `BOARD_I2C_SDA` | 13 | Schematic Page 1/4/7/8 | Shared by the main board I2C bus |
| Shared I2C SCL | `BOARD_I2C_SCL` | 14 | Schematic Page 1/4/7/8 | Shared by the main board I2C bus |
| Shared SPI SCK | `BOARD_SPI_SCK` | 36 | Schematic Page 3/4/6 | Shared by EPD, TF, and SX1262 |
| Shared SPI MOSI | `BOARD_SPI_MOSI` | 33 | Schematic Page 3/4/6 | Shared by EPD, TF, and SX1262 |
| Shared SPI MISO | `BOARD_SPI_MISO` | 47 | Schematic Page 3/6 | Shared by TF and SX1262 |
| BOOT | `BOARD_BOOT_PIN` | 0 | Schematic Page 2 | Boot button |

## 2. XL9555 Expansion IO

I2C address: `0x20`.

`BOARD_XL9555_INT` is currently fixed to `-1`, because the XL9555 `INT` pin is not connected to any ESP32-S3 pin.

| XL9555 | Current Macro / Preferred Name | Board Net | Direction | Source | Notes |
| --- | --- | --- | --- | --- | --- |
| P00 | `BOARD_XL9555_00_6609_EN` | `6609_EN` | Output | Schematic Page 1/7 | Enables A7682E power |
| P01 | `BOARD_XL9555_01_LORA_EN` | `LORA_EN` | Output | Schematic Page 3/7 | Enables SX1262 power |
| P02 | `BOARD_XL9555_02_GPS_EN` | `GPS_EN` | Output | Schematic Page 3/7 | Enables MIA-M10Q power |
| P03 | `BOARD_XL9555_03_1V8_EN` | `1V8_EN` | Output | Schematic Page 4/7 | Enables BHI260AP 1.8 V rail |
| P04 | `BOARD_XL9555_04_LORA_SEL` | `LORA_SEL` | Output | Schematic Page 3/7 | `HIGH` = internal antenna, `LOW` = external antenna |
| P05 | `BOARD_XL9555_05_MOTOR_EN` | `M_EN` | Output | Schematic Page 4/7 | DRV2605 power / enable |
| P06 | `BOARD_XL9555_06_AMPLIFIER` | `SHUTDOWM` | Output | Schematic Page 7/8 | Power amplifier enable; the schematic net is spelled `SHUTDOWM` |
| P07 | `BOARD_XL9555_07_TOUCH_RST` | `T_RST` | Output | Schematic Page 4/7 | Touch reset, active low |
| P10 | `BOARD_XL9555_10_PWRKEY_EN` | `PWRKEY_EN` | Output | Schematic Page 5/7 | A7682E `PWRKEY` control |
| P11 | `BOARD_XL9555_11_KEY_RST` | `KEY_RST` | Output | Schematic Page 7 | Keyboard reset, active low |
| P12 | `BOARD_XL9555_12_AUDIO_SEL` | `AUDIO_SEL` | Output | Schematic Page 7/8 | `HIGH = A7682E`, `LOW = ES8311` |
| P13 | `BOARD_XL9555_13` | NC | - | Schematic Page 7 | Reserved in the current repo |
| P14 | `BOARD_XL9555_14` | NC | - | Schematic Page 7 | Reserved in the current repo |
| P15 | `BOARD_XL9555_15` | NC | - | Schematic Page 7 | Reserved in the current repo |
| P16 | `BOARD_XL9555_16` | NC | - | Schematic Page 7 | Reserved in the current repo |
| P17 | `BOARD_XL9555_17` | NC | - | Schematic Page 7 | Reserved in the current repo |

## 3. EPD Display

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| EPD DC | `BOARD_EPD_DC` | GPIO35 | Schematic Page 4 | Schematic net name: `LCD_D/C` |
| EPD CS | `BOARD_EPD_CS` | GPIO34 | Schematic Page 4 | Schematic net name: `LCD_CS` |
| EPD BUSY | `BOARD_EPD_BUSY` | GPIO37 | Schematic Page 4 | Schematic net name: `LCD_BUSY` |
| EPD RST | `BOARD_EPD_RST` | GPIO9 | Schematic Page 4 | Schematic net name: `LCD_RST` |
| EPD SCK | `BOARD_EPD_SCK` | GPIO36 | Schematic Page 4 | Shared SPI bus |
| EPD MOSI | `BOARD_EPD_MOSI` | GPIO33 | Schematic Page 4 | Shared SPI bus |
| EPD frontlight PWM | `BOARD_EPD_BL` | GPIO41 | Schematic Page 4 | Schematic net name: `BL_PWM` |

## 4. Touch CST328

I2C address: `0x1A`.

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_TOUCH_SDA` | GPIO13 | Schematic Page 4 | Shared I2C bus |
| I2C SCL | `BOARD_TOUCH_SCL` | GPIO14 | Schematic Page 4 | Shared I2C bus |
| INT | `BOARD_TOUCH_INT` | GPIO12 | Schematic Page 4 | CST328 interrupt |
| RST | `BOARD_TOUCH_RST` | `XL9555 P07` | Schematic Page 4/7 | Board net `T_RST`; not a direct ESP32 GPIO |

## 5. Keyboard TCA8418

I2C address: `0x34`.

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_KEYBOARD_SDA` | GPIO13 | Schematic Page 7 | Shared I2C bus |
| I2C SCL | `BOARD_KEYBOARD_SCL` | GPIO14 | Schematic Page 7 | Shared I2C bus |
| INT | `BOARD_KEYBOARD_INT` | GPIO15 | Schematic Page 7 | TCA8418 interrupt |
| Backlight PWM | `BOARD_KEYBOARD_LED` | GPIO42 | Schematic Page 7 | Schematic net name: `LED_PWM` |
| RST | `BOARD_KEYBOARD_RST` | `XL9555 P11` | Schematic Page 7 | Board net `KEY_RST`, active low |

## 6. IMU BHI260AP

I2C address: `0x28`.

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_GYROSCOPDE_SDA` | GPIO13 | Schematic Page 4 | Shared I2C bus |
| I2C SCL | `BOARD_GYROSCOPDE_SCL` | GPIO14 | Schematic Page 4 | Shared I2C bus |
| INT | `BOARD_GYROSCOPDE_INT` | GPIO21 | Schematic Page 4 | Schematic net name: `HIRQ` |
| RST | `BOARD_GYROSCOPDE_RST` | `-1` | Schematic Page 4 | Reset is not wired |
| Power enable | `BOARD_XL9555_03_1V8_EN` | `XL9555 P03` | Schematic Page 4/7 | `HIGH` powers the IMU rail |

## 7. Motor DRV2605

I2C address: `0x5A`.

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_MOTOR_SDA` | GPIO13 | Schematic Page 4 | Shared I2C bus |
| I2C SCL | `BOARD_MOTOR_SCL` | GPIO14 | Schematic Page 4 | Shared I2C bus |
| Power / enable | `BOARD_MOTOR_EN` | `XL9555 P05` | Schematic Page 4/7 | Board net `M_EN` |

## 8. LoRa SX1262

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| SCK | `BOARD_LORA_SCK` | GPIO36 | Schematic Page 3 | Shared SPI bus |
| MOSI | `BOARD_LORA_MOSI` | GPIO33 | Schematic Page 3 | Shared SPI bus |
| MISO | `BOARD_LORA_MISO` | GPIO47 | Schematic Page 3 | Shared SPI bus |
| CS | `BOARD_LORA_CS` | GPIO3 | Schematic Page 3 | Module `NSS` |
| RST | `BOARD_LORA_RST` | GPIO4 | Schematic Page 3 | Module `NRESET` |
| IRQ | `BOARD_LORA_INT` | GPIO5 | Schematic Page 3 | Module `DIO1` |
| BUSY | `BOARD_LORA_BUSY` | GPIO6 | Schematic Page 3 | Module `BUSY` |
| Power enable | `BOARD_XL9555_01_LORA_EN` | `XL9555 P01` | Schematic Page 3/7 | `HIGH` powers the module |
| Antenna select | `BOARD_XL9555_04_LORA_SEL` | `XL9555 P04` | Schematic Page 3/7 | Current repo convention: `HIGH` = internal, `LOW` = external |

## 9. GPS MIA-M10Q

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| PPS | `BOARD_GPS_PPS` | GPIO1 | Schematic Page 3 | Schematic net name: `PPS` |
| Module TX -> MCU RX | `GPS_TX` / `BOARD_GPS_RXD` | GPIO2 | Schematic Page 3 | Schematic net names are from the GPS module point of view |
| Module RX <- MCU TX | `GPS_RX` / `BOARD_GPS_TXD` | GPIO16 | Schematic Page 3 | Schematic net names are from the GPS module point of view |
| Power enable | `BOARD_XL9555_02_GPS_EN` | `XL9555 P02` | Schematic Page 3/7 | `HIGH` powers the module |

## 10. 4G Modem A7682E

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| RI | `BOARD_A7682E_RI` | GPIO7 | Schematic Page 2/5 | Module `RI` |
| DTR | `BOARD_A7682E_ITR` / `BOARD_A7682E_DTR` | GPIO8 | Schematic Page 2/5 | The current repo interprets this signal as `DTR` |
| Module RXD | `7682_RXD` / `BOARD_A7682E_RXD` | GPIO10 | Schematic Page 2/5 | Net name is labeled from the modem side |
| Module TXD | `7682_TXD` / `BOARD_A7682E_TXD` | GPIO11 | Schematic Page 2/5 | Net name is labeled from the modem side |
| Power enable | `BOARD_XL9555_00_6609_EN` | `XL9555 P00` | Schematic Page 1/7 | A7682E main power rail enable |
| `PWRKEY` control | `BOARD_A7682E_PWRKEY` / `BOARD_XL9555_10_PWRKEY_EN` | `XL9555 P10` | Schematic Page 5/7 | Board net `PWRKEY_EN` |

## 11. TF Card

| Function | Current Macro / Preferred Name | GPIO | Source | Notes |
| --- | --- | --- | --- | --- |
| SD CS | `BOARD_SD_CS` | 48 | Schematic Page 2/6 | Shares SPI with EPD and LoRa |
| SD SCK | `BOARD_SD_SCK` | 36 | Schematic Page 3/6 | Shared SPI bus |
| SD MOSI | `BOARD_SD_MOSI` | 33 | Schematic Page 3/6 | Shared SPI bus |
| SD MISO | `BOARD_SD_MISO` | 47 | Schematic Page 3/6 | Shared SPI bus |

## 12. Audio ES8311

I2C address: `0x18`.

| Function | Current Macro / Preferred Name | GPIO / Mapping | Source | Notes |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_ES8311_SDA` | GPIO13 | Schematic Page 8 | Shared I2C bus |
| I2C SCL | `BOARD_ES8311_SCL` | GPIO14 | Schematic Page 8 | Shared I2C bus |
| I2S MCLK | `BOARD_ES8311_MCLK` | GPIO38 | Schematic Page 8 | `I2S_MCLK` |
| I2S BCLK / SCLK | `BOARD_ES8311_SCLK` | GPIO39 | Schematic Page 8 | `I2S_SCLK` |
| I2S LRCK | `BOARD_ES8311_LRCK` | GPIO18 | Schematic Page 8 | `I2S_LRCK` |
| ES8311 `ASDOUT` -> ESP32 DIN | `BOARD_ES8311_ASDOUT` | GPIO40 | Schematic Page 8 | Current repo macro naming follows the existing code convention |
| ESP32 DOUT -> ES8311 `DSDIN` | `BOARD_ES8311_DSDIN` | GPIO17 | Schematic Page 8 | Current repo macro naming follows the existing code convention |
| Power amp enable | `BOARD_XL9555_06_AMPLIFIER` | `XL9555 P06` | Schematic Page 7/8 | `HIGH` enables the amplifier |
| Audio route select | `BOARD_XL9555_12_AUDIO_SEL` | `XL9555 P12` | Schematic Page 7/8 | `HIGH = A7682E`, `LOW = ES8311` |

> Note: the current repo uses the long-standing `BOARD_ES8311_ASDOUT/DSDIN` macro names from the code side. If you read the ES8311 pin names strictly from the codec side, the apparent data directions can look reversed, so always verify the signal direction when refactoring or wiring new code.

## 13. I2C Address Table

| Device | Address | Source | Notes |
| --- | --- | --- | --- |
| ES8311 | `0x18` | Schematic Page 8 | Audio codec |
| CST328 | `0x1A` | `readme*.md` | Touch controller |
| XL9555 | `0x20` | Schematic Page 7 | IO expander |
| BHI260AP | `0x28` | Schematic Page 4 | IMU |
| TCA8418 | `0x34` | Schematic Page 7 | Keyboard matrix controller |
| BQ27220 | `0x55` | Schematic Page 1 | Fuel gauge |
| DRV2605 | `0x5A` | Schematic Page 4 | Vibration motor driver |
| BQ25896 | `0x6B` | Schematic Page 1 | Charger, now deprecated |
| SY6970 | `0x6A` | Schematic Page 1 | Charger |

Note: `BQ25896` was later replaced. Newer `T-Deck-MAX` boards use `SY6970`.

## 14. Current Codebase Notes

- `BOARD_A7682E_ITR` should be understood as `DTR`. The current public header also provides the alias `BOARD_A7682E_DTR`.
- GPS UART, A7682E UART, and ES8311 I2S naming still mix peripheral-side and ESP32-side viewpoints in different places. When writing new code, spell out the direction explicitly.
- Board-level macros are now centralized in `lib/TDeckMaxBoard/src/TDeckMaxBoard.h`.
