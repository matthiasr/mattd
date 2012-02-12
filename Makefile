INDENT= indent
INDENTFLAGS= -i4 -di1 -npsl
all: mattd

mattd: mattd.c

.PHONY: clean indent

indent: mattd.c
	$(INDENT) $(INDENTFLAGS) mattd.c

clean:
	rm -f *.o mattd
