include Config.mk

all_objects := $(os_objects) $(common_objects)

all: $(all_objects)
	$(CC) -o $(PROG) $(all_objects) $(LDFLAGS)
	$(OBJCOPY) -O ihex -R .eeprom $(PROG) $(PROG).hex

debug:
	$(OBJDUMP) -D $(PROG) > $(PROG).diss
	$(OBJDUMP) -x $(PROG) |grep -A6 "Idx Name"
	$(OBJDUMP) -x $(PROG) |grep " .bss" |grep -v "*" |sort -nk 5  |tail -20
	$(OBJDUMP) -x $(PROG) |grep " .data" |grep -v "*" |sort -nk 5  |tail -20

clean:
	rm -f $(all_objects) $(PROG) $(PROG).hex $(PROG).diss

flash:
	$(FLASH)
