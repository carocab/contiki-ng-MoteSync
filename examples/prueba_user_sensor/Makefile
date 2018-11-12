CONTIKI_PROJECT = test_sensor

TARGET ?= zoul
BOARD ?= remote-revb
PORT ?= /dev/ttyUSB0

all: $(CONTIKI_PROJECT)

CONTIKI = ../../
MAKE_MAC = MAKE_MAC_TSCH
include $(CONTIKI)/Makefile.include
