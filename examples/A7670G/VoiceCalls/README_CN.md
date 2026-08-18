# VoiceCalls

把音频切换到模组功放通道，拨打一次电话并在 20 秒后挂机。

- 硬件：支持语音的 A7670G 型号/固件、扬声器、LTE 天线、足够的电源和语音 SIM 卡。
- 配置：替换 `PHONE_NUMBER`；需要时调整 `CALL_DURATION_MS`。
- 编译：设置 `src_dir = examples/A7670G/VoiceCalls`。
- 预期：目标号码响铃，音频走模组通道，随后自动挂机。
- 注意：通话可能计费；部分 A7670G 后缀、固件或运营商 VoLTE 配置不支持语音。

改编自 `LilyGo-Modem-Series/examples/VoiceCalls`（MIT）。
