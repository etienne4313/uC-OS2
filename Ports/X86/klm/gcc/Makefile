#
# RTOS files
#
UCOS_II_SRC = $(UCOS_II)/Source
UCOS_II_PORT = $(UCOS_II)/Ports/X86/klm/gcc
os_objects := ../../common/switch.o ../../common/os_cpu.o ../../common/time.o ../../../../Source/ucos_ii.o

MOD = /lib/modules/$(shell uname -r)/build
TT := $(shell pwd)

obj-m += ucos.o
#Below is a hack to remove the absolute path TODO  $(abspath $(lastword $(MAKEFILE_LIST)))
ucos-y := $(os_objects) ../../../../../../../../../../../../../$(common_objects)

ccflags-y := -I $(INCLUDE) -I $(UCOS_II_SRC) -I $(UCOS_II_PORT)

all:
	make V=1 -C $(MOD) M=$(TT) modules 
	cp ucos.ko $(PROG).ko

debug:

clean:
	make -C $(MOD) M=$(TT) clean
	rm -f $(os_objects)
	rm -f $(PROG).ko

flash:

