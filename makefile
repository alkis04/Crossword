.SILENT: 
CC=gcc
CFLAGS=-c 
PNAME=crossw
SOURCES= main.c structures.c dictionary.c solve.c bitarrays.c draw.c
OBJECTS= $(SOURCES:.c=.o)
all: program
program: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(PNAME)
.c.o:
	$(CC) $(CFLAGS) $<
clean:
	rm -f *.o
	rm -f $(PNAME)