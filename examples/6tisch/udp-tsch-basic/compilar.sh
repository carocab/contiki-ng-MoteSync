#!/bin/bash

make clean
make PORT=/dev/ttyUSB0 server.upload
make clean
make PORT=/dev/ttyUSB1 NODEID=0x0013 client.upload
make PORT=/dev/ttyUSB0 login
#make PORT=/dev/ttyUSB1 login
