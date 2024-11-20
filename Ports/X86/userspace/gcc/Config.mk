#
# RTOS files
#
UCOS_II_SRC = $(UCOS_II)Source
UCOS_II_PORT = $(UCOS_II)Ports/X86/userspace/gcc
os_objects := $(UCOS_II_SRC)/ucos_ii.o $(patsubst %.c,%.o,$(wildcard $(UCOS_II_PORT)/*.c)) $(patsubst %.c,%.o,$(wildcard $(UCOS_II_PORT)/library/*.c))
asm_objects := $(UCOS_II_PORT)/switch.S

#
# Cross Compiler is native gcc
#
CROSS_COMPILE =
CC = $(CROSS_COMPILE)gcc
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

# -I $(IINCLUDE)
#
CFLAGS = -c -Wall -I $(UCOS_II_SRC) -I $(UCOS_II_PORT)
LDFLAGS = -lrt -lpthread

