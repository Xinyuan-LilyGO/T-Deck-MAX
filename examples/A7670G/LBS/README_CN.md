# LBS

使用 A7670G 基站定位服务，输出坐标、精度与 UTC 时间。

- 硬件：开发板、A7670G、LTE 天线、足够的电源、已开通流量的 SIM 卡。
- 配置：替换 `APN`；运营商要求认证时填写用户名和密码。
- 编译：设置 `src_dir = examples/A7670G/LBS`。
- 预期：每 30 秒输出一次近似位置。
- 说明：LBS 来自网络，精度低于 GNSS，可能向服务方暴露位置，并受固件/运营商支持情况影响。

改编自 `LilyGo-Modem-Series/examples/LBSExample`（MIT）。
