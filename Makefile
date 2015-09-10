#
# Makefile for linenoise
#

.PHONY: default
default: all

.PHONY: all
all: linenoise_example
all: linenoise_example_server

linenoise.o: linenoise.c linenoise.h

linenoise_example: $(patsubst %.c,%.o, linenoise.c example.c)
	$(CC) -Wall -W -Os -g -o $(@) $(^)

linenoise_example_server: $(patsubst %.c,%.o, linenoise.c example_server.c)
	$(CC) -Wall -W -Os -g -o $(@) $(^)

%.o: %.c
	$(CC) $(CFLAGS) -o $(@) -c $(<)

.PHONY: clean
clean:
	rm -f linenoise_example
	rm -f linenoise_example_server
	rm -f *.o


#
# EOF
#
