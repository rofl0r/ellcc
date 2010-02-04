OBJS = message.o

FLAGS = -DTEST
DEBUG = -g
CFLAGS = ${FLAGS} ${DEBUG} -std=c99
CXXFLAGS = ${FLAGS} ${DEBUG}
LDFLAGS = ${DEBUG}

.SUFFIXES: .c .cxx
.c.o:
	cc -c ${CFLAGS} $<

.cxx.o:
	c++ -c ${CXXFLAGS} $<

main: ${OBJS}
	c++ ${LDFLAGS} -o main ${OBJS} -lrt

clean:
	rm -f *.o
