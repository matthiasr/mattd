all: daemon

daemon: daemon.c

.PHONY: clean

clean:
	rm -f *.o daemon
