# uchariot-base

Avionics software for MicroChariot.

## Avionics Hardware

The main processor is a Raspberry PI 4. Each of the four wheel
motors is contained VESC 6 MKV motor controllers connected to 
a CAN bus. 

The RPI communicates over SPI to to an RS485 CAN hat
which connects the RPI to the CAN bus. This SPI connection is 
handled by Linux, which exposes the CAN connection as a network
interface.

[More information on the SPI-CAN network interface](https://www.waveshare.com/wiki/RS485_CAN_HAT).
[More information on the VESC 6 MKV controller CAN interface](https://trampaboards.com/vesc-6-mkv--the-next-generation--benjamin-vedder-electronic-speed-controller-p-27517.html).
[CAN Bus Protocol and Frame Data](https://vesc-project.com/sites/default/files/imce/u15301/VESC6_CAN_CommandsTelemetry.pdf)
