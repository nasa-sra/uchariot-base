#!/bin/bash

sudo pkill start.sh
sudo pkill --signal SIGINT uChariotBase
sleep 2
sudo pkill uChariotBase