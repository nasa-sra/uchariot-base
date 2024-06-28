# uchariot-base

Robot control software for MicroChariot.

To deploy the code to the robot, you can either crosscompile and deploy the executable, or clone the repo onto the pi and build it there.  
Then just ssh onto the pi and run the executable in ~/uchariot-base/build/ with `./uChariotBase`  

## Building
You can test the robot code by building and running it on Linux, like with WSL or a VM. The code will run in simulation, meaning there will be no connections to the CAN bus or other perphials.  
Install the build tools with `sudo apt install g++ cmake`  
Then build with  
```
mkdir build  
cd build  
cmake ..  
make
```
and run with `./uChariotBase`  

## Crosscompiling
To crosscompile for the raspberry pi, you can use WSL or a VM running Debian or Ubuntu. First, you need to create a sysroot directory. The cmake points by default to "~/uchariot-sys/sysroot/". You can use  
`rsync -vR --progress -rl --delete-after --safe-links USERNAME@RPI_IP:/{lib,usr,opt/vc/lib} $HOME/uchariot-sys/sysroot`  
to copy the pi's lib and usr directories to the sysroot. You then need to install the cross compiler for aarch64.  
`sudo apt install g++-aarch64-linux-gnu`  
Then to cross compile run
```
mkdir pibuild
cmake -DCMAKE_TOOLCHAIN_FILE=../piToolchain.cmake ..
make
```
This should generate an executable for aarch which you can deploy to the pi using `../deploy.sh`  
