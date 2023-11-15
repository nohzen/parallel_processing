CC = g++
CFLAGS = -Wall -O0 `pkg-config opencv4 --cflags` -fopenmp
LDLIBS = `pkg-config opencv4 --libs`

SOURCES = $(wildcard *.cpp)
BINARIES = $(SOURCES:.cpp=)

all: $(BINARIES)

%: %.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	$(RM) $(BINARIES)
