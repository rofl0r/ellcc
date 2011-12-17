SRCS := $(shell cd $(VPATH); echo *.c *.cxx *.y *.l) 
SRCS := $(SRCS:*.%=)
BASENAMES := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

CFLAGS += -Werror -MD -MP -O1

ifeq ($(XCC),)
  # The build compiler.
  CC = ../$(LEVEL)/../../../bin/$(TARGET)-$(OS)-ecc
else
  CC = $(XCC)
endif

.SUFFIXES: .c .S .o
.c.o:
	$(CC) $(MCPU) $(MFLOAT) -c $(CFLAGS) $<
.cxx.o:
	$(CC) $(MCPU) -c $(CFLAGS) $<

$(PROG): $(OBJS)
	echo LDFLAGS $(LDFLAGS)
	$(CC) -o $(PROG) $(LDFLAGS) $(OBJS) $(LDEXTRA)

clean:
	rm -fr *.o

install:
