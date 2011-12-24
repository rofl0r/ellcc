SRCS := $(shell cd $(VPATH); echo *.c *.cxx *.y *.l) 
-include $(VPATH)/sources
SRCS := $(SRCS:*.%=)
BASENAMES := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)
DEPENDSRCS := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

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
	$(CC) $(MCPU) $(MFLOAT) -c $(CFLAGS) $(XCFLAGS) $<
.cxx.o:
	$(CXX) $(MCPU) -c $(CXXFLAGS) $(XCXXLAGS) $<

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(LDFLAGS) $(XLDFLAGS) $(OBJS) $(LDEXTRA) $(XLDEXTRA)

clean:
	rm -fr *.o *.d *.gcda *.gcno $(PROG)

install:

-include $(DEPENDFILES) ""
