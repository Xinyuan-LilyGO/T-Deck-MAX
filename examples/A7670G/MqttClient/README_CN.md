# MqttClient

使用 A7670G 内置 MQTT 命令连接、订阅与发布，无需 PubSubClient。

- 硬件：开发板、A7670G、LTE 天线、足够的电源和已开通流量的 SIM 卡。
- 配置：替换 `APN`、Broker、客户端 ID 和主题；客户端 ID 应保持唯一。
- 编译：设置 `src_dir = examples/A7670G/MqttClient`。
- 预期：连接后发布一条消息，并输出 `TOPIC_RX` 收到的内容。
- 安全：默认公共 Broker 和 1883 端口不加密，不适合发送秘密数据或用于生产控制。

改编自 `LilyGo-Modem-Series/examples/MqttClient`（MIT）。
