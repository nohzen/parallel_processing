CC = g++
CFLAGS = -fopenmp

SOURCES = $(wildcard *.c)
BINARIES = $(SOURCES:.c=)

all: $(BINARIES)

%: %.cpp
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(BINARIES)
