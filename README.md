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

* [More information on the SPI-CAN network interface](https://www.waveshare.com/wiki/RS485_CAN_HAT).
* [More information on the VESC 6 MKV controller CAN interface](https://trampaboards.com/vesc-6-mkv--the-next-generation--benjamin-vedder-electronic-speed-controller-p-27517.html).
* [CAN Bus Protocol and Frame Data](https://vesc-project.com/sites/default/files/imce/u15301/VESC6_CAN_CommandsTelemetry.pdf)

## Libraries

| Name | Desc | Where |
| --- | --- | --- |
| wiringPi | peripheral interface | linked externally |
| rapidjson | json parsing | header only |
| nmea | gps parsing | header and source | 

## Cross-Compiling

### Sysroot 

To crosscompile for the raspberry pi, you first need to create a sysroot directory. The cmake points by default to `~/uchariot-sys/sysroot/`

You can use the following command to copy the pi's lib and usr directories to the sysroot.

`rsync -vR --progress -rl --delete-after --safe-links uchariot@10.93.24.4:/{lib,usr,opt/vc/lib} $HOME/uchariot-sys/sysroot`


### Native Compiling (ARM-Linux or M-series Mac)
 
 
>If you are using an M-series Mac, use a Linux VM (Ubuntu or Debian recomended) with UTM. This basically turns your computer into a ARM-Linux box so you can build without cross compiling.

Then to compile, run 
```
mkdir build
cd build
cmake ..
make
sh ../deploy.sh
```


### Cross-Compiling (X86-Windows/Mac/Linux)

> If you are using X86 Windows, use WSL Debian or Ubuntu.
> If you are using an Intel Mac, use a VM with Debian or Ubuntu.

Then install the cross compiling toolchain for aarch64, and run

`sudo apt install g++-aarch64-linux-gnu`

Then to cross-compile, run 
```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../piToolchain.cmake ..
make
sh ../deploy.sh
```

The only difference is the inclusion of `-DCMAKE_TOOLCHAIN_FILE=../piToolchain.cmake`.