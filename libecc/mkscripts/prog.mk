SRCS := $(shell cd $(VPATH); echo *.c *.cxx *.y *.l) 
SRCS := $(SRCS:*.%=)
BASENAMES := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

CFLAGS += -Werror -MD -MP -O1

ifeq ($(XCC),)
  # The build compiler.
  CC = $(ELLCC)/bin/$(TARGET)-$(OS)-ecc
  CXX = $(ELLCC)/bin/$(TARGET)-$(OS)-ecc++
else
  CC = $(XCC)
  CXX = $(XCXX)
endif

.SUFFIXES: .c .cxx .o
.c.o:
	$(CC) $(MCPU) $(MFLOAT) -c $(CFLAGS) $<
.cxx.o:
	$(CXX) $(MCPU) -c $(CFLAGS) $<

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(LDFLAGS) $(OBJS) $(LDEXTRA)

clean:
	rm -fr *.o

install:
