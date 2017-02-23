# wirebus - Easy IoT network
---------------------------------
## Introduction

Simple one wire protocol for microcontrollers communication. Protocol was designed for avr tiny in mind, but can be used for any microcontroller like pic16 or stm . Only one 8bit timer and one on-change pin IRQ required.

__Features:__

* No master required. All devices may talk each other
* Only one wire needed in simple mode. LVDS may be used for long lines
* Small footprint as about 700k
* Collision resolving 
* ISO/IEC CD 30141 compatible

------------------------------------
## Licensing

MIT License . See the file 'LICENSE' for licensing terms.

-------------------------------------
## Getting started

### Structure tree
__src__ and __include__ directories - no surprise,  precisely what its name suggests. source and include files for wirebus project

__devices__  - directory where units projects placed. Each project has mcu and architecture in its name. __noarc__ says device may be compiled for any supported architecture (avr,pic,stm)

__connectors__  - Devices to connect wirebus network to external. For now __uart__ to connect to host devices like Raspberry, or PC using TTL to USB converter. __esp8266__ - connects to WiFi network (under havy development)

__arduino__  - exactly what it says. Wirebus port for Arduino

### Supported architectures

See scripts/platforms.dat for supported MCUs 


### Creating new project

You may use simple bash script to create new project 

Useage : __./createproject.sh --mcu=stm8s --name test1234__

After project created examine __devices/firmware-test1234-stm__ directory to add your code.


--------------------------------------
## Register UUID for your device
### Why I should register UUID ?
UUID_MAJOR/UUID_MINOR pair my be requested from device using WIREBUS_CMD_GETDEVICEINFO command. By this pair maintainer knows Company/Type/Special needs for specified device. You don't need register this pair if you not plaining to release device to public.

### How do I get UUID_MAJOR/UUID_MINOR pair for my device ?
Just pull request for your project to be included in device tree. We will check the code and send you pair or range if your device will be accepted.

--------------------------------------
## Issues
1. In some cases packet buffer overflow may be possible. Controller will be resetted by watchdog
2. Attiny13 transport not completly tested
3. STM8 support not complete yet

------------------------------------
## History
23.02.17  Version 2.1b. Full code refactoring  

23.01.17  STM8 support added, not tested 

22.12.16  PIC12 support added

12.11.16  Attiny85 support added 

12.09.16  Draft version released on the wild 



