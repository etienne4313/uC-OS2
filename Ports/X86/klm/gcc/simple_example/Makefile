#
# Run this example on CPU 2
#  make load CPU=2; make unload CPU=2
#
PWD := $(shell pwd)
PROG := $(PWD)/$(shell basename `pwd`)

#
# Project files
#
INCLUDE = $(PWD)
common_objects := $(patsubst %.c,%.o,$(wildcard $(PWD)/*.c))

export PWD PROG INCLUDE common_objects

all:
	$(MAKE) -C $(KDIR)

debug:
	$(MAKE) -C $(KDIR) debug

clean:
	$(MAKE) -C $(KDIR) clean

load:
	$(MAKE) -C $(KDIR) load

unload:
	$(MAKE) -C $(KDIR) unload
