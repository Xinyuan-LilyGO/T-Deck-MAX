# ExternalGPS

为 T-Deck Pro MAX 板载独立 MIA-M10Q GNSS 接收机供电并解析数据，不使用 A7670G GNSS 命令。

- 硬件：T-Deck Pro MAX 和正确连接的 GNSS 天线；不需要 SIM 卡和 LTE 天线。
- 配置：默认 GNSS UART 为 9600 波特率，RX GPIO2/TX GPIO16。
- 编译：设置 `src_dir = examples/A7670G/ExternalGPS`。
- 预期：定位成功后输出坐标、卫星数量和 UTC 时间。
- 说明：首次定位需开阔天空并可能等待数分钟；室内或天线连接错误时，常见现象是有 NMEA 数据但位置无效。

改编自 `LilyGo-Modem-Series/examples/ExternalGPS_A7670G_Only`（MIT）；原 L76K 引脚已替换为本板 MIA-M10Q 引脚。
