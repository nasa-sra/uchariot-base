#!/bin/bash

pgrep gpsd > /dev/null 2>&1
if [ $? -eq 1 ]; then
    gpsd -D -F /dev/ttyACM0
fi

echo "Starting uChariotBase"
exe=$(sudo /home/uchariot/uchariot-base/build/uChariotBase)
$exe
# until $exe; do
#     echo "uchariot-base crashed with exit code $?.  Respawning.."
#     sleep 1
# done
