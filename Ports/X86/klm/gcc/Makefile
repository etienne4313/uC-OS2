MOD = /lib/modules/$(shell uname -r)/build
CURR := $(shell pwd)
KLM := $(notdir $(PROG))

#
# RTOS files
#
UCOS_II_SRC = $(UCOS_II)/Source
UCOS_II_PORT = $(UCOS_II)/Ports/X86/klm/gcc
os_objects := ../../common/switch.o ../../common/os_cpu.o ../../common/time.o ../../../../Source/ucos_ii.o

obj-m += $(KLM).o
# Below is a hack to remove the absolute path TODO  $(abspath $(lastword $(MAKEFILE_LIST)))
# https://stackoverflow.com/questions/15340725/how-i-strip-path-of-source-file-while-writing-a-compilation-rule-in-makefile
$(KLM)-y := $(os_objects) ../../../../../../../../../../../../../$(common_objects)

ccflags-y := -I $(INCLUDE) -I $(UCOS_II_SRC) -I $(UCOS_II_PORT)

all:
	make V=1 -C $(MOD) M=$(CURR) modules
	mv $(KLM).ko $(PROG).ko

debug:

clean:
	make -C $(MOD) M=$(CURR) clean
	rm -f $(os_objects)
	rm -f $(PROG).ko

load:
	sudo dmesg -c > /dev/null
	sudo insmod $(PROG).ko
	sudo sh -c "echo 0 >/sys/devices/system/cpu/cpu2/online"

unload:
	sudo cat /proc/rtos
	sleep 1
	sudo sh -c "echo 1 >/sys/devices/system/cpu/cpu2/online"
	sudo rmmod $(PROG).ko
	sudo dmesg -c 

