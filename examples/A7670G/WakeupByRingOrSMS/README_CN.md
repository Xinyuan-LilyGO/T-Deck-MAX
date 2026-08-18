# WakeupByRingOrSMS

ESP32-S3 深睡期间保持 A7670G 供电，并在模组 RI 拉低时唤醒。

- 硬件：开发板、A7670G、LTE 天线、足够的电池供电，以及支持电话或短信的 SIM 卡。
- 配置：无需修改；出现睡眠提示后向该 SIM 拨号或发送短信。
- 编译：设置 `src_dir = examples/A7670G/WakeupByRingOrSMS`。
- 预期：GPIO7 上的 RI 唤醒 ESP32-S3，串口显示模组 URC 5 秒。
- 说明：RI、语音和短信行为取决于固件/运营商配置；RI 噪声或持续为低会导致立即唤醒。

改编自 `LilyGo-Modem-Series/examples/WakeupByRingOrSMS`（MIT）。
