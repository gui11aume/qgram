OBJECTS= qgram.o
SOURCES= main.c

CFLAGS= -std=c99 -g -Wall -O3
CC= gcc

all: qgram

qgram: $(OBJECTS) $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(OBJECTS) -lm -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJECTS) qgram
