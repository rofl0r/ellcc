SRCS := $(shell cd $(VPATH); echo *.c *.cxx *.y *.l) 
-include $(VPATH)/sources
SRCS := $(SRCS:*.%=)
BASENAMES := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)
DEPENDSRCS := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

CFLAGS += -g -Werror -MD -MP
CXXFLAGS += -g -Werror -MD -MP

ifeq ($(XCC),)
  # The build compiler.
  CC = $(ELLCC)/bin/ecc -target $(TARGET)-ellcc-$(OS)-$(ABI)
  CXX = $(ELLCC)/bin/ecc++ -target $(TARGET)-ellcc-$(OS)-$(ABI)
else
  CC = $(XCC)
  CXX = $(XCXX)
endif

.SUFFIXES: .c .cxx .o
.c.o:
	$(CC) $(MCPU) $(MFLOAT) -c $(CFLAGS) $(XCFLAGS) $<
.cxx.o:
	$(CXX) $(MCPU) -c $(CXXFLAGS) $(XCXXFLAGS) $<

ifeq ($(EXCLUDE),)
$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(LDFLAGS) $(XLDFLAGS) $(OBJS) $(LDEXTRA) $(XLDEXTRA)
else
NOCHECK = 1
$(PROG):
	@echo $(PROG) for $(ARCH) is being skipped: $(EXCLUDE)
endif

clean:
	rm -fr *.o *.d *.gcda *.gcno $(PROG)

install: $(PROG)

ifneq ($(STDIN),)
INPUT := $(STDIN) |
endif

ifneq ($(STDOUT),)
OUTPUT := | $(STDOUT)
endif

ifeq ($(NOCHECK),)
check: clean $(PROG)
	$(INPUT) $(EXE)$(PROG) $(ARGS) $(OUTPUT) || exit 1
else
check:
endif

-include $(DEPENDFILES) ""
