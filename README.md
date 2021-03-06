# smarthouse-arduino

## Description
Arduino Nano (or compatible device) firmware that plays with the following set of hardware:
- DHT-11 humidity and temperature sensor
- PCD8544 (or compatible) LCD
- Photoresistor
- HC-06 bluetooth module 

## Features
- communicating
  - with PC desktop through UART
    - [desktop app] (https://github.com/konstunn/smarthouse-server)
  - with Android-based mobile device through bluetooth with [client app] (https://github.com/konstunn/smarthouse-android-bluetooth-client)
    - PCD8544 LCD backlight remote control from mobile device
    - display temperature, humidity, illumination on mobile device screen
- display humidity, temperature, illumination on local LCD
