# ATDebug

提供 USB 串口与 A7670G AT 串口之间的双向透传。

- 硬件：T-Deck Pro MAX、A7670G、电池或大电流电源、LTE 天线；SIM 卡可选。
- 配置：无需修改代码。串口监视器使用 115200 波特率及“NL 和 CR”。
- 编译：在 `platformio.ini` 设置 `src_dir = examples/A7670G/ATDebug`。
- 预期：串口提示模组在线，输入 `AT` 后返回 `OK`。
- 注意：不熟悉的 AT 命令可能永久改变模组配置。模组已有响应时公共代码不会再次触发 PWRKEY。

改编自 `LilyGo-Modem-Series/examples/ATdebug`（MIT）。
