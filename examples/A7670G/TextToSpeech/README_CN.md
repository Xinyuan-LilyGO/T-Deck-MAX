# TextToSpeech

把音频切换到模组功放通道，并通过 A76XX `AT+CTTS` 播报一段 ASCII 文本。

- 硬件：支持 TTS 的 A7670G 固件、扬声器和足够的电源；SIM 卡可选。
- 配置：替换 `TEXT_TO_SPEAK`；示例使用模式 2 播报 ASCII 文本。
- 编译：设置 `src_dir = examples/A7670G/TextToSpeech`。
- 预期：扬声器播放文本，串口输出 `TTS complete.`。
- 说明：TTS 语言包和 `AT+CTTS` 支持情况随模组后缀/固件而异；请使用安全音量。

改编自 `LilyGo-Modem-Series/examples/TextToSpeech`（MIT）。
