---
layout: post
title:  "Thermostat for pic12f683"
date:   2017-01-31 12:36:05 +0200
category: Devices
featured: yes
tags: [Devices]
---

## Firmware Thermostat for pic12f683
Nevertheless device called "thermostat" it can regulate any analog demension like soil humidity, light intensivity and others. Device can operate in two modes:


1.	__Automatic value control__

	In this mode device histeresis is set by sending command COMMAND\_DATA . When COMMAND\_SWITCH(ON) command received the device will automaticly switch load on and off when  histeresis levels reached (low and high accordingly) 


2. 	__Manual value control__

	In this mode device working exactly as analog-reader/timer pair. It means your host software should read the analog value and send back amount of seconds (up to 2 min)  to switch load on.
	This mode very usefull for implementing PID control of messured value.

## Supported commands


1. COMMAND\_SWITCH(ON\|OFF)

   Activates or deactivates device. Histeresis should be defined before using this command, when automatic mode used. (see below)

2. COMMAND\_DATA

   Sets "thermostat" parameters.


## Setup device for work

 As we don't know real demensions of value, messurment never a wast. Setup your system for low value and see how it will be changed during the time. So you can know average value changing. Do the same when load is on and value grows. You can check analog value  by sending command WIREBUS\_CMD\_GETSTATE. Device will replay WIREBUS\_INFO\_STATE with messured analog value (1-128) 

After you will know approximately estimated thresholds setup your device as follow:

### __Automatic value control__
	
In *automatic value control mode* top and low thresholds should be defined. Messure it using methodics described above.
Setup device by sending CMD\_DATA with the followint data payload :

__Payload data__


| Byte0	| Byte1	| Byte2	| Description	|
|---|:-------------|:-----|:-----|
0x01 | minValue | maxValue  | Setup min/max thresholds and switch to automatic mode 

<br />

After device setup you may use COMMAND\_SWITCH(ON\|OFF) to start or stop automatic process. Mode and thresholds saved in non-volatile memory and restored during boot process or switching device on/off



###  __Manual value control__ 

In *manual value control mode* you must implement external regulator or PID controller. Regulator should periodically ask device for analog value.
According to value changing trend you can shedule load switching. Internal timer implemented to switch device off automaticly. 
Typycal PID controller may adjust this timer duration (time during load is on) to regulate analog value. In this mode you don't have setup thresholds cause your 
external controller will worry about it.

__Commands to use__

| Command | Byte0 Payload | Description
|---|---|---|
WIREBUS\_CMD\_GETSTATE | Not used | Get analog value from device (1-128). Device responds with WIREBUS\_INFO\_STATE. topmost bit is 0
WIREBUS\_DATA\_SWITCH  | 0/1 or Timer delay in sec | Switch load on/off.See bellow* 


\**If Byte0 > 1 top 7 bits defines timer in seconds (up to 128 sec ~2 min). If value == 1 timer disabled and load switched on.*
	


## DIY resources

Sorry, this part under construction yet. Here we want to post BOM, links to order parts or download project files 

For now you can see latest updates on  [Wirebus github repository](https://github.com/vt77/wirebus)


