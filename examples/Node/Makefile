CONTIKI_PROJECT = server client
all: $(CONTIKI_PROJECT)

TARGET ?= zoul
BOARD ?= remote-revb
MAKE_WITH_USER_SENSOR = 1

CONTIKI=../../

MODULES += os/services/shell

 # force Orchestra from command line
MAKE_WITH_ORCHESTRA ?= 0
# force Security from command line
MAKE_WITH_SECURITY ?= 0
 # print #routes periodically, used for regression tests
MAKE_WITH_PERIODIC_ROUTES_PRINT ?= 1

MAKE_MAC = MAKE_MAC_TSCH

ifeq ($(MAKE_WITH_ORCHESTRA),1)
MODULES += os/services/orchestra
endif

ifeq ($(MAKE_WITH_SECURITY),1)
CFLAGS += -DWITH_SECURITY=1
endif

ifeq ($(MAKE_WITH_PERIODIC_ROUTES_PRINT),1)
CFLAGS += -DWITH_PERIODIC_ROUTES_PRINT=1
endif

# To use math library
TARGET_LIBFILES+=-lm

# Project .c files
PROJECT_SOURCEFILES += decoder.c

include $(CONTIKI)/Makefile.include
