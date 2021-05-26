# P1 DSMR Reader with ESPHome

This is a custom component for ESPHome and the P1 DSMR Reader board to receive DSMR messages from a Dutch Smart Meter (version 4.x and higher) and pulses coming from a water meter via a proximity sensor. This repository holds a custom P1 UART Reader Component for ESPHome and contains several configurations that might suite your needs for monitoring your household power, gas, and water consumptions.

### Prerequisites

* A P1 DSMR Reader board
* An LJ12A3-4-Z/BX-5V proximity sensor (NPN Normally Open)
* USB to UART programmer, like an FTDI or CP2102
* USB-C cable and usb power supply
* RJ12 6P6C cable for communications between the Smart Meter and the P1 DSMR Reader

