# Arduino rangefinder

This device rotates a HC-SR04 ultrasound rangefinder using a PWM-controlled servo motor and surveys surroundings, 
writing data to 128x64 OLED display GUI.

## Components
- Arduino UNO
- Rangefinder: HC-SR04
- OLED display, monochrome 128x64 (Troyka module)
- FS90 Micro servo

## Assembly
- Refer to pin number macros defined in program.
- I2C is enabled via A4 (Data) and A5 (Clock).
- Attach all VCC and GND pins to respective buses and attach rangefinder to servo arm.

## Operation
- Measurements are saved and overwritten automatically as the device rotates.
- Press "TOGGLE" button to lock servo in position.
