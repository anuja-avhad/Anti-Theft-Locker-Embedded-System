# Circuit Wiring Pinout & Connections

This document details the exact physical and virtual pin connections for the Anti-Theft Locker System built on an Arduino UNO (ATmega328P).

--------------------------------------------------
Pin Mapping Table
--------------------------------------------------

Component         | Pin Function | Arduino UNO Pin | Notes / Operating Details
------------------|--------------|-----------------|---------------------------------------------------
4x4 Keypad        | Row 1 (R1)   | Digital Pin 2   | Matrix Row Output Line 1
4x4 Keypad        | Row 2 (R2)   | Digital Pin 3   | Matrix Row Output Line 2
4x4 Keypad        | Row 3 (R3)   | Digital Pin 4   | Matrix Row Output Line 3
4x4 Keypad        | Row 4 (R4)   | Digital Pin 5   | Matrix Row Output Line 4
4x4 Keypad        | Col 1 (C1)   | Digital Pin 6   | Matrix Column Input Line 1 (Pull-up)
4x4 Keypad        | Col 2 (C2)   | Digital Pin 7   | Matrix Column Input Line 2 (Pull-up)
4x4 Keypad        | Col 3 (C3)   | Digital Pin 8   | Matrix Column Input Line 3 (Pull-up)
4x4 Keypad        | Col 4 (C4)   | Digital Pin 9   | Matrix Column Input Line 4 (Pull-up)
Green LED         | Anode (+)    | Digital Pin 10  | Connected via 220 Ohm resistor (Access Granted)
Red LED           | Anode (+)    | Digital Pin 11  | Connected via 220 Ohm resistor (Access Denied / Alarm)
Active Buzzer     | Positive (+) | Digital Pin 12  | Keypress chirp & alarm tone generator
SG90 Servo        | PWM Signal   | Digital Pin 13  | Actuator control (0 deg Locked, 90 deg Unlocked)
16x2 LCD          | SDA (Data)   | Analog Pin A4   | I2C Serial Data (0x27 bus address)
16x2 LCD          | SCL (Clock)  | Analog Pin A5   | I2C Serial Clock

--------------------------------------------------
Power Connections
--------------------------------------------------

* 5V Bus: Connected to Arduino 5V Pin -> Powers LCD VCC, Servo VCC (Red wire), and key board rails.
* Ground Bus: Connected to Arduino GND Pin -> Shared Ground line for LCD GND, Servo GND (Black/Brown wire), Buzzer (-), and LED Cathodes.
