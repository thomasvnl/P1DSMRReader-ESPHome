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

More recent versions of ESPHome [changed something](https://github.com/oxan/esphome-stream-server/issues/6) that made ESPHome no longer backwards compatible. You need to install any version lower than 2021.10.0. For example: `pip3 install esphome==2021.9.3`

### 2.3 Using or Creating a Configuration

The repository holds a few (example) configurations, some with an extension indicating which communication component they use for communicating the telemetry data. These configurations can be used and are compatible as-is, but can of course also be modified based on personal needs.

The following configurations are supplied and should be a good fit/good starting point for you to get going:

* `p1dsmrreader-all.yaml` Contains the full set of components to communicate telemetry via API/Hass, HTTP Request, MQTT, and Web/REST server. __For demonstration purposes only__, using different ways of communication could bite each other.
* `p1dsmrreader-hass.yaml` Contains only the API/Hass component
* `p1dsmrreader-http.yaml` Contains only the HTTP Request component
* `p1dsmrreader-mqtt.yaml` Contains only the MQTT component
* `p1dsmrreader-webserver.yaml` Contains only the Web Server component

Please do not modify the usage of the UARTP1ReaderComponent in the lambda and leave the GPIO switch for pin 4 (P1 Request To Send) as-is and internal to not disturb the decoding of DSMR messages. If you do modify this functionality you do so at your own risk.

## 3. Compiling & Uploading

In the same folder as the configurations, execute `esphome run` followed by the filename of the chosen configuration. For example: `esphome run p1dsmrreader-mqtt.yaml`. After the build process is done you get to choose the com port to upload to. 

If you get: `A fatal error occurred: Failed to connect to ESP32: Invalid head of packet (0x1B)` then you need to press the flash button when the uploading process starts. This is indicated by some of the lights flashing on the ftdi programmer.

## 4. Telemetry

### 4.1 Via API/Home Assistant

-- Needs example from someone using Home Assistant with this device --

### 4.2 Via MQTT

Each available value will be published to its own topic.

`<TOPIC_PREFIX>` = name of device (p1dsmrreader) unless otherwise specified.

All topics use the format `<TOPIC_PREFIX>/sensor/<SENSOR_NAME>/state` to publish readings.

`<SENSOR_NAME>` values:

* `actual_current_l1`: The amperage on phase 1 (A)
* `actual_current_l2`: The amperage on phase 2 (A)<sup>1</sup>
* `actual_current_l3`: The amperage on phase 3 (A)<sup>1</sup>
* `actual_power_consumed`: The current/actual power the household consumes from the grid (W)
* `actual_power_produced`: The current/actual power the household produces to the grid (W)
* `actual_voltage_l1`: The voltage on phase 1 (V)
* `actual_voltage_l2`: The voltage on phase 2 (V)<sup>1</sup> 
* `actual_voltage_l3`: The voltage on phase 3 (V)<sup>1</sup>
* `electricity_consumed_high_tariff`: The amount of normal/high tariff electricity consumed from the grid (Wh)
* `electricity_consumed_low_tariff`: The amount of low tariff electricity consumed from the grid (Wh)<sup>2</sup>
* `electricity_produced_high_tariff`: The amount of normal/high tariff electricity produced to the grid (Wh)
* `electricity_produced_low_tariff`: The amount of low tariff electricity produced to the grid (Wh)<sup>2</sup>
* `esphome_version`: Version of ESPHome the firmware was build with
* `gas_consumed`: The amount of gas consumed (dm³/litre)
* `ip_address`: The IP address of the device
* `total_water_consumption`: The total amount of water consumed since boot (dm³/litre)
* `uptime`: Uptime in hours or seconds (depending on settings)
* `water_consumption_per_minute`: The amount of water consumed last minunte (dm³/litre)
* `wifi_signal`: The wifi signal dBm

![Animation of MQTT topics](/docs/images/p1dsmrreader-mqtt-topics.gif)

<sup>1</sup>Only available when connected to a three-phase smart meter.  
<sup>2</sup>Only available if your contract has a normal and low tariff.

### 4.3 Pushing Data to an HTTP endpoint




```
{
	"actualPower": {
		"consumed": int,
		"produced": int
	},
	"electricity": {
		"consumed": {
			"lowTarrif": int,
			"highTariff": int
		},
		"produced": {
			"lowTariff": int,
			"highTariff": int
		}
	},
	"water": {
		"lastMinute": int,
		"total": int
	}
	"gas": int,
	"uptime": int/float
}
```

Units are the same as for the [MQTT payload in section 4.2](#42-via-mqtt)

![JSON Payload](/docs/images/p1dsmrreader-http-json-msg.png)

### 4.4 Direct REST calls

See https://esphome.io/web-api/index.html#rest-api how to make direct REST calls when using the web-server configuration.

## 5. FAQ

* _Sometimes after an OTA update the device stalls at shutdown/boot up_: You will need to manually reset the device when this happens (i.e. push the RESET button). At present it is not clear why the firmware does not continue its operation, and unfortunately there's no watchdog that resets the MCU. After resetting the device it works fine, so for now this is a minor inconvenience that will hopefully be resolved in the future.

## 6. Attribution & Thanks

* Thanks to [The Flamingo](https://github.com/Flamingo-tech) for aiding me with his knowledge of hardware and PCB manufacturing and supplying the tools and time to produce the P1 DSMR Reader boards.
* Thanks to [Matthijs Kooijman](https://github.com/matthijskooijman) for creating the [arduino-dsmr](https://github.com/matthijskooijman/arduino-dsmr) library.
* Thanks to [AlexMekkering](https://github.com/AlexMekkering) for providing the [esphome-config](https://github.com/AlexMekkering/esphome-config) repository with example yaml structure and basic sensor configurations.
