# uchariot-base

Robot control software for MicroChariot.

To deploy the code to the robot, you can either crosscompile and deploy the executable, or clone the repo onto the jetson and build it there.  
Then just SSH onto the jetson and run the executable in ~/uchariot-base/build/ with `sudo ./uChariotBase`  

## Libraries

Minimum CMake version is 3.16.

| Name | Description | Install |
| --- | --- | --- |
| libeigen3 | linear algebra | sudo apt install libeigen3-dev | 
| libi2c | i2c interface | sudo apt install libi2c-dev |
| libgps | connecting to gpsd server | sudo apt install libgps-dev | 
| rapidjson | json parsing | header-only |
| tinyxml | XML parsing | header-only |

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
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
make
```

The only difference is the inclusion of `-DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake`.

### Deploy

Now you should have produced a binary called `uChariotBase`. Deploy this using `../deploy.sh`.
>You may want to set up an [SSH key](https://www.ssh.com/academy/ssh/copy-id) so that you don't have to keep entering the password.

### Running
Run the executable on the robot with  
```
cd ~/uchariot-base/build
./uchariotBase
```
It can also be started with `~/uchariot-base/start.sh` which is what is run by the start button on the [driver console](https://github.com/nasa-sra/uchariot-console).

### Simulation

You can also run the code on your local environment in simulation, this will disable peripherals like CAN, I2C, and serial connection. Just run  
```
mkdir build
cd build
cmake -DSIMULATION=true ..
make
sudo ./uChariotBase
```

## Documentation
Checkout the docs at ./docs/html/index.html  
Rebuild them with  
```
sudo apt install doxygen graphviz
cd docs
doxygen Doxyfile
```
