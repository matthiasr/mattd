all: mattd

mattd: mattd.c

.PHONY: clean

clean:
	rm -f *.o mattd
