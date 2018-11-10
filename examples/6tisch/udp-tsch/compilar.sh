#!/bin/bash

make clean
make PORT=/dev/ttyUSB0 server.upload
make clean
make PORT=/dev/ttyUSB1 client.upload
make PORT=/dev/ttyUSB0 login
#make TARGET=zoul BOARD=remote-revb PORT=/dev/ttyUSB1 login
