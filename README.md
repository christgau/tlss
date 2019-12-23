# tlss - Traffic Light Start System

A simple radio-based start system using traffic lights for rowing competitions.

# Background

Devices control traffic lights (red and green) conforming to the regulations
for rowing competitions. The starting umpire (starter) switches the lights from
neutral (all off) to an active red light when competitors are called to be
"ready". When the race is started, the red light switches off and the green one
turns on ("go"). At the same time an acoustic signal is emitted. If a false
start was detected (by whichever mechanism), the lights alternately turn on and
off with along an acoustic signal which indicates the false start to the crews.

# Device Concept

There is a device (starter unit) that is under the control of the starter and
manages all traffic lights. The starter unit uses the same hard- and software
as the traffic light units on the individual lanes (lane units). They only
differ in configuration (starter vs lane unit). The lane units can be
configured with their lane number.

When the starter unit receives a ready signal, it broadcasts an according
command via radio. If the starter unit receives a ready signal again, the
lights are turned off (neutral position). When lights are red (ready) and the
start command is received, again a command is broadcast to turn the red light
off, the green one on, and start the audio. The lane unit reset themselves into
neutral state again after a certain timeout. A separate false start signal can
trigger an according message from the starter unit to the lane units.

For each lane a timeout value can be configured via radio in order to have
individual lanes switch to the green lights when that timeout is expired.

A LoRA gateway-like device can forward the commands from the starter unit to
the timing system.

All radio messages are encrypted using AES and should be protected against
replay attacks.

# Target Platform

Microship ATtinyX1Y (1-series). Currently, the ATtiny816 is targeted.

# Build

In order to build the package, you need the current libc/m for the ATtiny
1-series. You can download them from Microchip website:
[https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers].
Extract `libm.a` and `libc.a` from
`avr8-gnu-toolchain-linux_x86_64/avr/lib/avrxmega3/short-calls/`.

In addition, you need the `libattiny816.a` and `crtattiny816.o` files from the
_Microchip ATtiny Series Device Support_ pack
([https://packs.download.microchip.com/])

Place those files under `build/dev/attiny816/avrxmega3/short-calls/`

# Remarks

Not tested with actual hardware, just software implementation for proof of
concept and resource usage exploration.
