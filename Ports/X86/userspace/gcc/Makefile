include Config.mk

all_objects := $(os_objects) $(common_objects)

all: $(all_objects)
	as $(asm_objects) -o /tmp/switch.out
	$(CC) -o $(PROG) $(all_objects) /tmp/switch.out $(LDFLAGS)

debug:
	$(OBJDUMP) -D $(PROG) >$(PROG).diss

clean:
	rm -f $(all_objects) $(PROG) $(PROG).hex $(PROG).diss

