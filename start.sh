#!/bin/bash

# start gps dameon
sudo gpsd /dev/ttyACM0 -S 2024

# until /home/uchariot/uchariot-base/build/uchariot; do
#     echo "uchariot-base crashed with exit code $?.  Respawning.." >&2
#     sleep 1
# done