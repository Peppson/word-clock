# WordClock [![C++](https://img.shields.io/badge/C++-00599C?logo=c%2b%2b&logoColor=white)](https://isocpp.org/) [![PlatformIO](https://img.shields.io/badge/PlatformIO-FF6600?logo=platformio&logoColor=white)](https://platformio.org/) 

> **Note:** This is a personal hobby project.  

![wordClock](/images/wordClock.jpg) 

## Overview
- Machine-cut 45x45cm metal faceplate *(Swedish time notation)*
- Web Interface for configuration and playing games on the 11x10 grid! 
- Wi-Fi time synchronization with optional manual time setting
- Ambient light sensor for automatic brightness control
- Physical buttons for direct control
- NeoPixel LEDs with full RGB color

## Web Interface
A simple accesspoint that is activated using a physical button.
- Configure Wi-Fi credentials for NTP time synchronization
- Manually set the time *(alternative to NTP)*
- Adjust LED colors
- Play Snake *(why not?)*

![webPage](/images/webPage/CombinedWebPage.png) 

## Hardware/Build

#### Parts used:
- ESP32 microcontroller
- WS2812B (NeoPixel) LEDs ×110
- TSL2591 lux sensor
- Basic electronic components

Schematic:
![Schematic](/images/Schematic.png) 

Cross section *(thanks Chat)*
![Schematic](/images/build/sideView.png) 

MDF backside:
![build](/images/build/IMG_1671.jpg) 

LED panel:  
*Lesson learned. Check LED strip spacing first. Ended up soldering every LED individually...*
![build](/images/build/IMG_1678.jpg) 

LED diffusors:  
*Simple frosted plastic sheet cut into circles.*
![build](/images/build/IMG_1818.jpg) 

<details>
    <summary>More build images</summary>

<br>

Top: light sensor + status LED
![build](/images/build/IMG_1662.jpg)   

Right side: Buttons
![build](/images/build/IMG_1664.jpg)   

Without LED diffusors
![build](/images/build/IMG_1814.jpg)   

With LED diffusors
![build](/images/build/IMG_1819.jpg) 

Backside with LED panel
![build](/images/build/IMG_1816.jpg) 
 
</details>

<br>

## LEDs brightness correction

Brightness is mapped through a custom correction curve to better match perceived light levels from the lux sensor.  
It took some trial and error to calibrate, but this curve works well for the current placement of the clock.  
In total darkness, the LEDs are turned off.  

![curve](/images/luxCorrectionCurve.png) 

Example code:
```c++
float normalizedAverage = luxAverage / LUX_THRESHOLD_MAX;
float mappedAverage = applyCorrectionCurve(normalizedAverage);
float scaledAverage = scaleLedBrightness(mappedAverage); // Hardware btn scaler in 0.2f steps
```

More images in [📂 images/](./images/) 
