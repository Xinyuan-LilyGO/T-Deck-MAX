# SendSMS

Registers on the mobile network and sends one text SMS.

- Hardware: board, A7670G, LTE antenna, adequate power and an SMS-capable SIM/plan.
- Configuration: replace `SMS_TARGET`; include the international country code. Set `SIM_PIN` if required.
- Build: set `src_dir = examples/A7670G/SendSMS`.
- Expected result: `SMS sent.` appears and the destination receives one message.
- Caution: flashing/resetting sends another billable SMS. SMS support depends on the A7670G firmware and operator IMS/SMS service.

Adapted from `LilyGo-Modem-Series/examples/SendSMS` (MIT).
