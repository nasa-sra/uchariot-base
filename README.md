# uchariot-base

Robot control software for MicroChariot.

To deploy the code to the robot, you can either crosscompile and deploy the executable, or clone the repo onto the pi and build it there.  
Then just ssh onto the pi and run the executable in ~/uchariot-base/build/ with `./uChariotBase`  

## Libraries

| Name | Desc | Install |
| --- | --- | --- |
| libeigen3 | linear algebra | sudo apt install libeigen3-dev | 
| wiringPi | peripheral interface | ??? |
| rapidjson | json parsing | n/a |
| nmea | gps parsing | n/a | 

## Crosscompiling

### Sysroot 

To crosscompile for the raspberry pi, you first need to create a sysroot directory. The cmake points by default to `~/uchariot-sys/sysroot/`

You can use the following command to copy the pi's lib and usr directories to the sysroot.

`rsync -vR --progress -rl --delete-after --safe-links USERNAME@RPI_IP:/{lib,usr,opt/vc/lib} $HOME/uchariot-sys/sysroot`


### Native Compiling (ARM-Linux or M-series Mac)
 
>If you are using an M-series Mac, use a Linux VM (Ubuntu or Debian recomended) with UTM. This basically turns your computer into a ARM-Linux box so you can build without crosscompiling.

Then to compile, run 
```
mkdir build
cd build
cmake ..
make
```

### Crosscompiling (X86-Windows/Mac/Linux)

> If you are using X86 Windows, use WSL Debian or Ubuntu.
> If you are using an Intel Mac, use a VM with Debian or Ubuntu.

Then install the crosscompiling toolchain for aarch64, and run

`sudo apt install g++-aarch64-linux-gnu`

Then to crosscompile, run 
```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../piToolchain.cmake ..
make
```

The only difference is the inclusion of `-DCMAKE_TOOLCHAIN_FILE=../piToolchain.cmake`.

> If you build without this flag on X86-Windows/Max/Linux then the project will run in simulation mode, with no connection to any peripherals like the CAN bus.

### Deploy

Now you schould have produced a binary called ` uChariotBase`. Deploy this using `../deploy.sh`.

### Simulation