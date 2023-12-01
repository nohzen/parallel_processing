CC = g++
CPPFLAGS= -DCL_TARGET_OPENCL_VERSION=300
CFLAGS = -O0 `pkg-config opencv4 --cflags`
#  -Wall
LDLIBS = `pkg-config opencv4 --libs` -lOpenCL

OBJS_MAIN=main.o cl_util.o

main: $(OBJS_MAIN)
	${CC} -o $@ $(OBJS_MAIN) $(LDLIBS)

%.o: %.cpp
	${CC} -c -o $@ $^ $(CPPFLAGS) $(CFLAGS)

clean:
	$(RM) $(BINARIES) $(OBJS_MAIN) main
