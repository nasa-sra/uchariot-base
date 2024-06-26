#!/bin/bash

until /home/uchariot/uchariot-base/build/uchariot; do
    echo "uchariot-base crashed with exit code $?.  Respawning.." >&2
    sleep 1
done