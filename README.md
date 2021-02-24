# Introduction

This is a simple implementation of a SurePet Feeder to MQTT broker.

# Acknowledgements

Thank you to Peter Lambrechtsen for reversing the MQTT protocol used by the SurePet system and finding the xor encryption key.

# Features

This is the list of features currently implemented:

* Reporting when the lid is open or closed by pet
* Reporting when the lid is open or closed by user
* Reporting the amount of food left in the left,right and both trays
* Reporting the Pet Chip ID that is feeding

These values can be consumed by Home Assistant to generate graphs and gauges:

![Tray Weight](images/Weight.png)


# Requirements

This requires the following:

* A Raspberry Pi (Tested on Pi3)
* Mikroe Pi3 Shield - https://www.mikroe.com/pi-3-click-shield
* BeeClick - https://www.mikroe.com/bee-click

# Pi setup

The following additional packages need to be installed onto the Pi:

* libmosquitto-dev
* libmosquitto1
* wiringpi
* libcjson-dev

Then the SPI needs to be enabled using the raspi-config command. 

1. Type <br><br> _sudo raspi-config_ <br><br>
1. Select "Interface Options"
1. Select "SPI"
1. Select "Yes" to enable SPI

By default, the application stores persistent state data into */var/run/catfeeder/state.json*. This means that the */var/run/catfeeder* directory needs to be created and writable by the application. 

# Usage

To start, type:

    ./CatFeeder -h <broker> -p <port> 

Once the application is running, press the "connect" button on the Feeder, this will pair the feeder with the application. 

The application can be added to supervisord (http://supervisord.org/) to allow it to be run in the background.     


# MQTT Topics

| Topic Name        | Type | Description |
| ------------- |:-------------:|:-------------:|
| petfeeder/[deviceMAC]/message | String | The raw MiWi payload, Only sent in debug mode |
| petfeeder/[deviceMAC]/battery      | Integer | Guess at what could be battery level |
| petfeeder/[deviceMAC]/lidOpen    | Boolean | true if the lid is open |
| petfeeder/[deviceMAC]/userOpen    | Boolean | true if the user opened the lid |
| petfeeder/[deviceMAC]/leftOpen_weight    | Float | Weight of food in the left tray in grams on opening |
| petfeeder/[deviceMAC]/leftClose_weight   | Float | Weight of food in the left tray in grams on closing |
| petfeeder/[deviceMAC]/rightOpen_weight   | Float | Weight of food in the right tray in grams on opening |
| petfeeder/[deviceMAC]/rightClose_weight  | Float | Weight of food in the right tray in grams on closing |
| petfeeder/[deviceMAC]/weight   | Float | Total Weight of food in both trays on closing |
| pet/[petChipID]/petFeeding | Boolean | true if the pet opened the lid |
| pet/[petChipID]/petFeedingTime | Integer | The amount of time (in seconds) the pet was feeding for in this session |
| pet/[petChipID]/eaten | Float | The amount of food eaten in by this pet in this session |
| pet/[petChipID]/petTotalDailyEaten | Float | The total daily amount of food (in grams) the pet has eaten, reset at midnight |
| pet/[petChipID]/petTotalDailyFeedingTime | Integer | The total daily amount of time (in seconds) the pet was feeding, reset at midnight  |

* deviceMAC: The MAC address of the device
* petChipID: The Pets Chip ID

# Protocol information

The SurePet feeder protocol is based on MiWi, this is a protocol made by Microchip. SurePet have changed a few MiWi fields to make it incompatible, for example the Protocol ID has been changed from 0x4D to 0x7E. 


# TODO

* More daily stats
* Handle multiple feeders/doors 
* Battery state, could be byte 3 in the beacon

