#!/bin/bash

sudo pkill start.sh
sudo pkill --signal SIGINT uChariotBase
sudo pkill --signal SIGINT uChariotVision
sleep 2
sudo pkill uChariotBase
sudo pkill uChariotVision