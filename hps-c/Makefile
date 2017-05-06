#
TARGET = can_control
ALT_DEVICE_FAMILY ?= soc_cv_av
HWLIBS_ROOT = $(SOCEDS_DEST_ROOT)/ip/altera/hps/altera_hps/hwlib

#
COMPILER_PATH = /home/roboy/Downloads/gcc-linaro-arm-linux-gnueabihf-4.7-2012.11-20121123_linux/bin/
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -static -g -Wall -std=c++11 -I${HWLIBS_ROOT}/include -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY) -D$(ALT_DEVICE_FAMILY) -Iinclude/ -Iusr/include -Incurses_arm/include
LDFLAGS =  -g -Wall -Lncurses_arm/lib -lncurses -Lusr/lib -ltinyxml
CC = ${COMPILER_PATH}$(CROSS_COMPILE)gcc
CPP = ${COMPILER_PATH}$(CROSS_COMPILE)g++
ARCH= arm

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.o)))

.PHONY : all
all : $(TARGET)

build: $(TARGET)
$(TARGET): $(OBJ_FILES)
	$(CPP) $(LDFLAGS) -o $@ $^
$(OBJ_FILES): build/%.o : src/%.cpp
	$(CPP) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGET) build/*.a build/*.o build/*~ 