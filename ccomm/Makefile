OBJS = message.o rhapbridge.o 

DEBUG = -g
CFLAGS = ${FLAGS} ${DEBUG} -std=c99
CXXFLAGS = ${FLAGS} ${DEBUG} -I${OMROOT}/LangCpp/oxf -I${OMROOT}/LangCpp/osconfig/Linux
LDFLAGS = ${DEBUG}

.SUFFIXES: .c .cc
.c.o:
	cc -c ${CFLAGS} $<

.cxx.o:
	c++ -c ${CXXFLAGS} $<

main: ${OBJS}
	c++ ${LDFLAGS} -o main ${OBJS} -lrt

test:  test.o ${OBJS}
	c++ ${LDFLAGS} -o test test.o ${OBJS} ${OMROOT}/LangCpp/lib/linuxoxf.a -lrt

clean:
	rm -f *.o
