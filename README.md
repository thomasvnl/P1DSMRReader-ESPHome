# P1 DSMR Reader with ESPHome

This is the documentation for configuring the P1 DSMR Reader board with ESPHome to receive DSMR messages from a Dutch Smart Meter (version 4.x and higher) and readings coming from a water meter via a proximity sensor. 

This repository holds a custom P1UARTReaderComponent for ESPHome and contains several (example) configurations that might suite your needs for monitoring your household power, gas, and water consumptions.

![Image of P1 DSMR Reader board](/docs/images/p1dsmrreader-fhd.jpg)

## 1. Prerequisites

* A P1 DSMR Reader board
* An LJ12A3-4-Z/BX-5V (or similar) proximity sensor (NPN, Normally Open). Needs to be 5V compatible.
* USB to UART programmer, like an FTDI or CP2102
* USB-C cable and USB power supply
* RJ12 6P6C cable that plugs into the P1 port of the Smart Meter and the P1 port of the P1 DSMR Reader board.  
  |  __N.B.__  the 5V power supply of the Smart Meter is not used for supplying power to the reader board so a 6P4C cable between meter and board should work for communication, but be aware that in that case the _P1 5V_ indicator LED stays off.

## 2. Getting Started

After cloning the repository to your machine you will need to do a little bit of setup to make the configuration(s) work for your device and environment.

### 2.1 The `secrets.yaml` File

In the cloned repository you will find a `secrets.yaml.example` file. Copy this file to `secrets.yaml`. 

The `secrets.yaml` file holds all the confidential parameters you might need to set, like your WiFi SSID/Password, or your MQTT broker settings, or an API key for calling an HTTP endpoint. 

You will only need to fill in secrets that are used by your chosen configuration, so if you are not using MQTT for instance you can ignore secrets that only apply to that component.

### 2.2 Installing ESPHome

The ESPHome website has a very straightforward guide on how to install the necessary packages. If you haven't installed ESPHome yet please navigate to [ESPHome/Getting Started#Installation](https://esphome.io/guides/getting_started_command_line.html#installation) and follow the instructons before proceeding.

### 2.3 Using or Creating a Configuration

The repository holds a few (example) configurations, some with an extension indicating which communication component they use for communicating the telemetry data. These configurations can be used and are compatible as-is, but can of course also be modified based on personal needs.

The following configurations are supplied and should be a good fit/good starting point for you to get going:

* `p1dsmrreader-all.yaml` Contains the full set of components to communicate telemetry via API/Hass, HTTP Request, MQTT, and Web/REST server
* `p1dsmrreader-hass.yaml` Contains only the API/Hass component
* `p1dsmrreader-http.yaml` Contains only the HTTP Request component
* `p1dsmrreader-mqtt.yaml` Contains only the MQTT component
* `p1dsmrreader-webserver.yaml` Contains only the Web Server component

Please do not modify the usage of the UARTP1ReaderComponent in the lambda and leave the GPIO switch for pin 4 (P1 Request To Send) as-is and internal to not disturb the decoding of DSMR messages. If doing so, you are doing this at your own risk.

## 3. Compiling & Uploading

## 4. Telemetry

### 4.1 Via API/Home Assistant

### 4.2 Via MQTT

### 4.3 Pushing Data to an HTTP endpoint

### 4.4 Direct REST calls

https://esphome.io/web-api/index.html#rest-api

## 5. FAQ

* _Sometimes after an OTA update the device stalls at shutdown/boot up_: You will need to manually reset the device when this happens. At present it is not clear why the firmware does not continue. After resetting the device it works fine, so for now this is a minor inconvenience.

## 6. Attribution & Thanks

* Thanks to [Koelie2](https://github.com/Koelie2) for aiding me with his knowledge of hardware and PCB manufacturing and supplying the tools and time to produce the P1 DSMR Reader boards.
* Thank to [Matthijs Kooijman](https://github.com/matthijskooijman) for creating the [arduino-dsmr](https://github.com/matthijskooijman/arduino-dsmr) library.
* Thanks to [AlexMekkering](https://github.com/AlexMekkering) for providing the [esphome-config](https://github.com/AlexMekkering/esphome-config) repository with example yaml structure and basic sensor configurations.