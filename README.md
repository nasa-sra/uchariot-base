# uchariot-base

Robot control software for MicroChariot.

To deploy the code to the robot, you can either crosscompile and deploy the executable, or clone the repo onto the jetson and build it there.  
Then just ssh onto the jetson and run the executable in ~/uchariot-base/build/ with `./uChariotBase`  

## Libraries

Minimum CMake version is 3.16.

| Name | Description | Install |
| --- | --- | --- |
| libeigen3 | linear algebra | sudo apt install libeigen3-dev | 
| libi2c | i2c interface | sudo apt install libi2c-dev |
| rapidjson | json parsing | n/a |
| libgps | connecting to gpsd server | sudo apt install libgps-dev | 

## Crosscompiling

### Sysroot 

To crosscompile for the Jetson, you first need to create a sysroot directory. The cmake points by default to `~/uchariot-sys/sysroot/`

You can use the following command to copy the jetsons's lib and usr directories to the sysroot in WSL or your VM.

`rsync -vR --progress -rl --delete-after --safe-links USERNAME@IP:/{lib,usr,opt/vc/lib} $HOME/uchariot-sys/sysroot`

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

NOTE: You MUST use Debian 11 or Ubuntu 20.04 LTS to crosscompile for the Jetson. The default WSL debian version is 12, so use these [instructions](https://stackoverflow.com/questions/77170725/how-to-install-debian-11-on-wsl-manually-i-am-trying-to-download-it-from-from) to get a Debian 11 disto.  

Once you have your VM, install the libraries (see table above) and the g++ 9 crosscompiling toolchain for aarch64, with

`sudo apt install g++-9-aarch64-linux-gnu`

Then to crosscompile, run 
```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
make
```

The only difference is the inclusion of `-DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake`.

> If you build without this flag on X86-Windows/Max/Linux then the project will run in simulation mode, with no connection to any peripherals like the CAN bus.

### Deploy

Now you should have produced a binary called `uChariotBase`. Deploy this using `../deploy.sh`.

### Simulation
