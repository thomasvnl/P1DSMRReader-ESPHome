# P1 DSMR Reader with ESPHome

This is the documentation for configuring the P1 DSMR Reader board with ESPHome to receive DSMR messages from a Dutch Smart Meter (version 4.x and higher) and readings coming from a water meter via a proximity sensor. 

This repository holds a custom P1UARTReaderComponent for ESPHome and contains several (example) configurations that might suite your needs for monitoring your household power, gas, and water consumptions.

## 1. Prerequisites

* A P1 DSMR Reader board
* An LJ12A3-4-Z/BX-5V (or similar) proximity sensor (NPN, Normally Open). Needs to be 5V compatible.
* USB to UART programmer, like an FTDI or CP2102
* USB-C cable and USB power supply
* RJ12 6P6C cable that plugs into the P1 port of the Smart Meter and the P1 port of the P1 DSMR Reader board.    
	<sub><sup>__N.B.__ the 5V power supply of the Smart Meter is __not used__ so a 6P4C cable between meter and reader board should work as well. Do note that the P1 5V LED will remain off if you're not using a 6P6C cable.</sup></sub>

## 2. Getting Started

After cloning the repository to your machine you will need to do a little bit of setup to make the configuration(s) work for your device and environment.

### 2.1 The `secrets.yaml` File

In the cloned repository you will find a `secrets.yaml.example` file. Copy this file to `secrets.yaml`. 

The `secrets.yaml` file holds all the confidential parameters you might need to set, like your WiFi SSID/Password, or your MQTT broker settings, or an API key for calling an HTTP endpoint. 

You will only need to fill in secrets that are used by your chosen configuration, so if you are not using MQTT for instance you can ignore secrets that only apply to that component.

### 2.2 Installing ESPHome

The ESPHome website has a very straightforward guide on how to install the necessary packages. If you haven't installed ESPHome yet please navigate to [ESPHome/Getting Started#Installation](https://esphome.io/guides/getting_started_command_line.html#installation) and follow the instructons before proceeding.

### 2.3 Creating a Configuration



## 3. Compiling & Uploading

## 4. Telemetry

### 4.1 Via API/Home Assistant

### 4.2 Via MQTT

### 4.3 Pushing Data to an HTTP endpoint

### 4.4 Direct REST calls