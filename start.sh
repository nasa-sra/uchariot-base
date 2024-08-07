#!/bin/bash

pgrep gpsd > /dev/null 2>&1
if [ $? -eq 1 ]; then
    gpsd -D -F /dev/ttyACM0
fi

# need to start this
# sudo slcand -o -s6 -S 1152000 /dev/ttyACM1

echo "Starting uchariotVision"
nexe=$(sudo /home/uchariot/uchariot-vision/build/uChariotVision)
$nexe

echo "Starting uChariotBase"
exe=$(sudo /home/uchariot/uchariot-base/build/uChariotBase)
$exe
# until $exe; do
#     echo "uchariot-base crashed with exit code $?.  Respawning.."
#     sleep 1
# done
