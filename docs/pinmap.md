# T-Deck Pro Max 引脚映射（V0.1 / 2025-09-11）

> 本文按 `hardware/T-Deck Pro Max V0.1 25-09-11/T-Deck Pro Max V0.1 25-09-11.pdf` 整理，并交叉核对了仓库当前 `readme.md`、`readme_cn.md` 与 `examples/factory/utilities.h`。

> 本文按模块分类整理。原理图里部分网络名按外设视角命名，例如 `GPS_TX`、`7682_RXD`、`I2S_DSDIN`；文中会同时注明仓库里常见的板级宏名。

## 1. 主控与公共总线

| 功能 | 当前宏/建议名 | GPIO | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| 主 I2C SDA | `BOARD_I2C_SDA` | 13 | 原理图 Page 1/4/7/8 | 全板共享 I2C |
| 主 I2C SCL | `BOARD_I2C_SCL` | 14 | 原理图 Page 1/4/7/8 | 全板共享 I2C |
| 主 SPI SCK | `BOARD_SPI_SCK` | 36 | 原理图 Page 3/4/6 | EPD、TF、SX1262 共用 |
| 主 SPI MOSI | `BOARD_SPI_MOSI` | 33 | 原理图 Page 3/4/6 | EPD、TF、SX1262 共用 |
| 主 SPI MISO | `BOARD_SPI_MISO` | 47 | 原理图 Page 3/6 | TF、SX1262 共用 |
| BOOT | `BOARD_BOOT_PIN` | 0 | 原理图 Page 2 | 启动按键 |

## 2. XL9555 扩展 IO

I2C 地址：`0x20`。

`BOARD_XL9555_INT` 当前固定为 `-1`，因为 XL9555 的 `INT` 没有接入任何 ESP32 引脚。

| XL9555 | 当前宏/建议名 | 板级网络 | 方向 | 来源 | 备注 |
| --- | --- | --- | --- | --- | --- |
| P00 | `BOARD_XL9555_00_6609_EN` | `6609_EN` | 输出 | 原理图 Page 1/7 | A7682E 电源使能 |
| P01 | `BOARD_XL9555_01_LORA_EN` | `LORA_EN` | 输出 | 原理图 Page 3/7 | SX1262 电源使能 |
| P02 | `BOARD_XL9555_02_GPS_EN` | `GPS_EN` | 输出 | 原理图 Page 3/7 | MIA-M10Q 电源使能 |
| P03 | `BOARD_XL9555_03_1V8_EN` | `1V8_EN` | 输出 | 原理图 Page 4/7 | BHI260AP 1.8 V 电源使能 |
| P04 | `BOARD_XL9555_04_LORA_SEL` | `LORA_SEL` | 输出 | 原理图 Page 3/7 | `HIGH` 内置天线，`LOW` 外置天线 |
| P05 | `BOARD_XL9555_05_MOTOR_EN` | `M_EN` | 输出 | 原理图 Page 4/7 | DRV2605 供电/使能 |
| P06 | `BOARD_XL9555_06_AMPLIFIER` | `SHUTDOWM` | 输出 | 原理图 Page 7/8 | 功放使能；原理图网名拼写就是 `SHUTDOWM` |
| P07 | `BOARD_XL9555_07_TOUCH_RST` | `T_RST` | 输出 | 原理图 Page 4/7 | 触摸复位，低有效 |
| P10 | `BOARD_XL9555_10_PWRKEY_EN` | `PWRKEY_EN` | 输出 | 原理图 Page 5/7 | A7682E `PWRKEY` 控制 |
| P11 | `BOARD_XL9555_11_KEY_RST` | `KEY_RST` | 输出 | 原理图 Page 7 | 键盘复位，低有效 |
| P12 | `BOARD_XL9555_12_AUDIO_SEL` | `AUDIO_SEL` | 输出 | 原理图 Page 7/8 | `HIGH = A7682E`，`LOW = ES8311` |
| P13 | `BOARD_XL9555_13` | NC | - | 原理图 Page 7 | 当前仓库保留 |
| P14 | `BOARD_XL9555_14` | NC | - | 原理图 Page 7 | 当前仓库保留 |
| P15 | `BOARD_XL9555_15` | NC | - | 原理图 Page 7 | 当前仓库保留 |
| P16 | `BOARD_XL9555_16` | NC | - | 原理图 Page 7 | 当前仓库保留 |
| P17 | `BOARD_XL9555_17` | NC | - | 原理图 Page 7 | 当前仓库保留 |

## 3. EPD 显示

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| EPD DC | `BOARD_EPD_DC` | GPIO35 | 原理图 Page 4 | 原理图网络名 `LCD_D/C` |
| EPD CS | `BOARD_EPD_CS` | GPIO34 | 原理图 Page 4 | 原理图网络名 `LCD_CS` |
| EPD BUSY | `BOARD_EPD_BUSY` | GPIO37 | 原理图 Page 4 | 原理图网络名 `LCD_BUSY` |
| EPD RST | `BOARD_EPD_RST` | GPIO9 | 原理图 Page 4 | 原理图网络名 `LCD_RST` |
| EPD SCK | `BOARD_EPD_SCK` | GPIO36 | 原理图 Page 4 | 共用主 SPI |
| EPD MOSI | `BOARD_EPD_MOSI` | GPIO33 | 原理图 Page 4 | 共用主 SPI |
| EPD 前光 PWM | `BOARD_EPD_BL` | GPIO41 | 原理图 Page 4 | 原理图网络名 `BL_PWM` |

## 4. 触摸 CST328

I2C 地址：`0x1A`。

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_TOUCH_SDA` | GPIO13 | 原理图 Page 4 | 共用主 I2C |
| I2C SCL | `BOARD_TOUCH_SCL` | GPIO14 | 原理图 Page 4 | 共用主 I2C |
| INT | `BOARD_TOUCH_INT` | GPIO12 | 原理图 Page 4 | CST328 中断 |
| RST | `BOARD_TOUCH_RST` | `XL9555 P07` | 原理图 Page 4/7 | 板级网络 `T_RST`，不是 ESP32 直连 GPIO |

## 5. 键盘 TCA8418

I2C 地址：`0x34`。

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_KEYBOARD_SDA` | GPIO13 | 原理图 Page 7 | 共用主 I2C |
| I2C SCL | `BOARD_KEYBOARD_SCL` | GPIO14 | 原理图 Page 7 | 共用主 I2C |
| INT | `BOARD_KEYBOARD_INT` | GPIO15 | 原理图 Page 7 | TCA8418 中断 |
| 背光 PWM | `BOARD_KEYBOARD_LED` | GPIO42 | 原理图 Page 7 | 原理图网络名 `LED_PWM` |
| RST | `BOARD_KEYBOARD_RST` | `XL9555 P11` | 原理图 Page 7 | 板级网络 `KEY_RST`，低有效 |

## 6. IMU BHI260AP

I2C 地址：`0x28`。

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_GYROSCOPDE_SDA` | GPIO13 | 原理图 Page 4 | 共用主 I2C |
| I2C SCL | `BOARD_GYROSCOPDE_SCL` | GPIO14 | 原理图 Page 4 | 共用主 I2C |
| INT | `BOARD_GYROSCOPDE_INT` | GPIO21 | 原理图 Page 4 | 原理图网络名 `HIRQ` |
| RST | `BOARD_GYROSCOPDE_RST` | `-1` | 原理图 Page 4 | 当前未接复位脚 |
| 电源使能 | `BOARD_XL9555_03_1V8_EN` | `XL9555 P03` | 原理图 Page 4/7 | `HIGH` 上电 |

## 7. 马达 DRV2605

I2C 地址：`0x5A`。

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_MOTOR_SDA` | GPIO13 | 原理图 Page 4 | 共用主 I2C |
| I2C SCL | `BOARD_MOTOR_SCL` | GPIO14 | 原理图 Page 4 | 共用主 I2C |
| 供电/使能 | `BOARD_MOTOR_EN` | `XL9555 P05` | 原理图 Page 4/7 | 板级网络 `M_EN` |

## 8. LoRa SX1262

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| SCK | `BOARD_LORA_SCK` | GPIO36 | 原理图 Page 3 | 共用主 SPI |
| MOSI | `BOARD_LORA_MOSI` | GPIO33 | 原理图 Page 3 | 共用主 SPI |
| MISO | `BOARD_LORA_MISO` | GPIO47 | 原理图 Page 3 | 共用主 SPI |
| CS | `BOARD_LORA_CS` | GPIO3 | 原理图 Page 3 | 模块 `NSS` |
| RST | `BOARD_LORA_RST` | GPIO4 | 原理图 Page 3 | 模块 `NRESET` |
| IRQ | `BOARD_LORA_INT` | GPIO5 | 原理图 Page 3 | 模块 `DIO1` |
| BUSY | `BOARD_LORA_BUSY` | GPIO6 | 原理图 Page 3 | 模块 `BUSY` |
| 电源使能 | `BOARD_XL9555_01_LORA_EN` | `XL9555 P01` | 原理图 Page 3/7 | `HIGH` 上电 |
| 天线切换 | `BOARD_XL9555_04_LORA_SEL` | `XL9555 P04` | 原理图 Page 3/7 | 当前仓库以 `HIGH` 内置、`LOW` 外置 为准 |

## 9. GPS MIA-M10Q

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| PPS | `BOARD_GPS_PPS` | GPIO1 | 原理图 Page 3 | 原理图网络名 `PPS` |
| 模块 TX -> 主控 RX | `GPS_TX` / `BOARD_GPS_RXD` | GPIO2 | 原理图 Page 3 | 原理图网名按 GPS 模块视角命名 |
| 模块 RX <- 主控 TX | `GPS_RX` / `BOARD_GPS_TXD` | GPIO16 | 原理图 Page 3 | 原理图网名按 GPS 模块视角命名 |
| 电源使能 | `BOARD_XL9555_02_GPS_EN` | `XL9555 P02` | 原理图 Page 3/7 | `HIGH` 上电 |

## 10. 4G 模块 A7682E

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| RI | `BOARD_A7682E_RI` | GPIO7 | 原理图 Page 2/5 | 模块 `RI` |
| DTR | `BOARD_A7682E_ITR` | GPIO8 | 原理图 Page 2/5 | 当前仓库把这个宏理解为 `DTR` |
| 模块 RXD | `7682_RXD` / `BOARD_A7682E_RXD` | GPIO10 | 原理图 Page 2/5 | 网名按模块视角命名 |
| 模块 TXD | `7682_TXD` / `BOARD_A7682E_TXD` | GPIO11 | 原理图 Page 2/5 | 网名按模块视角命名 |
| 电源使能 | `BOARD_XL9555_00_6609_EN` | `XL9555 P00` | 原理图 Page 1/7 | A7682E 电源域 |
| `PWRKEY` 控制 | `BOARD_A7682E_PWRKEY` / `BOARD_XL9555_10_PWRKEY_EN` | `XL9555 P10` | 原理图 Page 5/7 | 板级网络 `PWRKEY_EN` |

## 11. TF 卡

| 功能 | 当前宏/建议名 | GPIO | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| SD CS | `BOARD_SD_CS` | 48 | 原理图 Page 2/6 | 与 EPD、LoRa 共用 SPI |
| SD SCK | `BOARD_SD_SCK` | 36 | 原理图 Page 3/6 | 共用主 SPI |
| SD MOSI | `BOARD_SD_MOSI` | 33 | 原理图 Page 3/6 | 共用主 SPI |
| SD MISO | `BOARD_SD_MISO` | 47 | 原理图 Page 3/6 | 共用主 SPI |

## 12. 音频 ES8311

I2C 地址：`0x18`。

| 功能 | 当前宏/建议名 | GPIO/映射 | 来源 | 备注 |
| --- | --- | --- | --- | --- |
| I2C SDA | `BOARD_ES8311_SDA` | GPIO13 | 原理图 Page 8 | 共用主 I2C |
| I2C SCL | `BOARD_ES8311_SCL` | GPIO14 | 原理图 Page 8 | 共用主 I2C |
| I2S MCLK | `BOARD_ES8311_MCLK` | GPIO38 | 原理图 Page 8 | `I2S_MCLK` |
| I2S BCLK/SCLK | `BOARD_ES8311_SCLK` | GPIO39 | 原理图 Page 8 | `I2S_SCLK` |
| I2S LRCK | `BOARD_ES8311_LRCK` | GPIO18 | 原理图 Page 8 | `I2S_LRCK` |
| ES8311 `ASDOUT` -> ESP32 DIN | 原理图网名 `I2S_ASDOUT` | GPIO40 | 原理图 Page 8 | 原理图按 esp32 视角命名 |
| ESP32 DOUT -> ES8311 `DSDIN` | 原理图网名 `I2S_DSDIN` | GPIO17 | 原理图 Page 8 | 原理图按 esp32 视角命名 |
| 功放使能 | `BOARD_XL9555_06_AMPLIFIER` | `XL9555 P06` | 原理图 Page 7/8 | `HIGH` 使能 |
| 音频路由选择 | `BOARD_XL9555_12_AUDIO_SEL` | `XL9555 P12` | 原理图 Page 7/8 | `HIGH = A7682E`，`LOW = ES8311` |

> 说明：`readme*.md` 与 `examples/factory/utilities.h` 中的 `BOARD_ES8311_ASDOUT/DSDIN` 用的是 ESP32 I2S 方向命名；如果按原理图网名理解，则应对应 `ASDOUT = GPIO17`、`DSDIN = GPIO40`。

## 13. I2C 地址表

| 设备 | 地址 | 来源 | 备注 |
| --- | --- | --- | --- |
| ES8311 | `0x18` | 原理图 Page 8 | 音频 codec |
| CST328 | `0x1A` | `readme*.md` | 触摸控制器 |
| XL9555 | `0x20` | 原理图 Page 7 | IO 扩展 |
| BHI260AP | `0x28` | 原理图 Page 4 | IMU |
| TCA8418 | `0x34` | 原理图 Page 7 | 键盘矩阵控制器 |
| BQ27220 | `0x55` | 原理图 Page 1 | 电量计 |
| DRV2605 | `0x5A` | 原理图 Page 4 | 振动马达驱动 |
| BQ25896 | `0x6B` | 原理图 Page 1 | 充电管理(已停用) |
| SY6970 | `0x6A` | 原理图 Page 1 | 充电管理 |

说明：`BQ25896` 因为一些原因停用，后续的 `T-Deck-MAX` 都将使用 `SY6970`。

## 14. 代码现状提示

- `BOARD_A7682E_ITR` 这个宏名在当前仓库里应理解为 `DTR`。
- GPS、A7682E UART、ES8311 I2S 这三组信号，代码里同时存在“外设网名视角”和“ESP32 侧视角”两套叫法，写新代码时最好把方向写清楚。
- 当前仓库的主板宏主要散落在 `readme*.md`、`examples/factory/utilities.h` 和若干示例各自的 `utilities.h`，建议后续收敛成单一公共头文件。
