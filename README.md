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

## Crosscompiling
To crosscompile for the raspberry pi, you first need to create a sysroot directory. The cmake points by default to ~/uchariot-sys/sysroot/
You can use
`rsync -vR --progress -rl --delete-after --safe-links uchariot@10.93.24.4:/{lib,usr,opt/vc/lib} $HOME/uchariot-sys/sysroot`
to copy the pi's lib and usr directories to the sysroot. You then need to install the cross compiler for aarch64. 
`sudo apt install g++-aarch64-linux-gnu`
Then create a pibuild directory and run
`cmake -DCMAKE_TOOLCHAIN_FILE=../piToolchain.cmake ..`
and 
`make`
This should generate an executable for aarch which you can deploy to the pi using
`../deploy.sh`
