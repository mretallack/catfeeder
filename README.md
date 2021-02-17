# Introduction

This is a simple implementation of a SurePet Feeder to MQTT broker.

# Acknowledgements

Thank you to Peter Lambrechtsen for reversing the MQTT protocol used by the SurePet system and finding the xor encryption key.


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

# Usage

To start, type:

    ./CatFeeder -h <broker> -p <port> 

# TODO

* More daily stats
* Handle multiple feeders/doors 
* Battery state, could be byte 3 in the beacon
* Save the pet data to disk and load on startup
