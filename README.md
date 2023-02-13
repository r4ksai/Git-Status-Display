# Git Status Display

![Background](Images/background.jpg)

This is a display for showing GitHub Collaborations.

## Setup Device

To install the firmware, use platformio and upload the sketch directly to the ESP8266.

Once installed, The device will act as an access point.

To access the access point user the password **0123456789**

browse to **192.168.0.1** and add in your wifi credentials.

Once you have entered the credentials the device will restart and connect to the wifi.

Now find the IP of the device using your prefered IP scanner and access the IP on the website with an address of `/token`
Here you can add your token and display username

![Git Status Display](Images/angled.jpg)

## Getting a token

The token can be obtained from github
[Token Generation](https://github.com/settings/tokens)

While generating the token keep in mind that you only need **_read:user_** permission

## Changing username

To change the username you can go to `/username` and update the username

## Reseting the device

To reset the device you can go to `/reset` and this automatically removes the wifi credentials.
For removing the token you'll have to manually go and change the token to blank space

![Git Status Display Loading](Images/zoomed.jpg)

## Wiring

![Wiring Diagram](Images/wiring_diagram.png)

## Enclosure

![Enclosure](Images/enclosure.png)

There are files in enclosure folder that you can modify and slice with your prefered slicer and print.

## Flow Chart

```mermaid
flowchart TD

Start --> B[Clear Display]
B --> C[Splash Screen]
C --> D[Loading Screen]
D --> F[Check EEPROM for WiFi Creds]
F --> G{WiFi Creds Found}
G --> |No| H[Start Access Point]
G --> |Yes| I[Connect to WiFi]
I --> J{Connected to WiFi}
J --> |No| H
J --> |Yes| K[Get Git Status Dots]
K --> L[Setup Webserver]
H --> L[Setup Webserver]
K --> E[Status Dots Screen]
E --> |Wait 10 Mins| D
E --> End
L --> End
```
