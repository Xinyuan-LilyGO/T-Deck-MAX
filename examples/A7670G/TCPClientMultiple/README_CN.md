# TCPClientMultiple

同时打开两个独立的 A7670G TCP Socket 并读取 HTTP 响应。

- 硬件：开发板、A7670G、LTE 天线、足够的电源和已开通流量的 SIM 卡。
- 配置：替换 `APN`；需要时修改 `setup()` 中的两个主机和资源路径。
- 编译：设置 `src_dir = examples/A7670G/TCPClientMultiple`。
- 预期：串口交替输出 mux 0 和 mux 1 的响应，最后关闭两个连接。
- 说明：两个 Socket 的输出可能交错，这是预期行为；测试请求均为未加密 HTTP。

改编自 `LilyGo-Modem-Series/examples/TCPClientMultiple`（MIT）。
