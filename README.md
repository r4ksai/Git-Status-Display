# Git Status Display

This is a display for showing GitHub Collaborations.

## Setup Device

To install the firmware, use platformio and upload the sketch directly to the ESP32.

## Wiring

|ESP32 |DOT MATRIX|
|------|----------|
|5V    |VCC       |
|PIN 18|CLK       |
|PIN 23|DATA      |
|PIN 5 |CS        |
|GND   |GND       |

![Wiring Diagram](wiring_diagram.jpeg)

## Enclosure

There is a file called enclosure.stl that you can slice with your prefered slicer and print.

## TODO

1. Add a default username
2. Seperate username from token
3. Auto dim according to the time
4. Show loading animation on start 
5. Use freertos to make display animation async
6. Fix Auto reloading the form pages