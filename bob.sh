modprobe can
modprobe can_raw
modprobe mttcan

sudo ip link set can0 type can bitrate 500000
sudo ifconfig can0 up

cansend 00000001#000186A0