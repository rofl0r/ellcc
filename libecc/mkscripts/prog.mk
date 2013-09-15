SRCS := $(shell cd $(VPATH); echo *.c *.cxx *.y *.l) 
-include $(VPATH)/sources.mk
SRCS := $(SRCS:*.%=)
BASENAMES := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)
DEPENDSRCS := $(basename $(filter %.c %.cxx %.y %.l, $(SRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

CFLAGS += -g -MD -MP
CXXFLAGS += -g -MD -MP
-include $(ELLCC)/libecc/mkscripts/targets/$(TARGET)/setup.mk

ifeq ($(XCC),)
  # The build compiler.
  CC = $(ELLCC)/bin/ecc
  CXX = $(ELLCC)/bin/ecc++
else
  CC = $(XCC)
  CXX = $(XCXX)
endif

.SUFFIXES: .c .cxx.s .S .o
.s.o:
	$(CC) $(MCPU) $(MFLOAT) -c $(CFLAGS.$(TARGET)) $(XCFLAGS) $<
.S.o:
	$(CC) $(MCPU) $(MFLOAT) -c $(CFLAGS.$(TARGET)) $(XCFLAGS) $<
.c.o:
	$(CC) $(MCPU) $(MFLOAT) -c $(CFLAGS.$(TARGET)) $(XCFLAGS) $<
.cxx.o:
	$(CXX) $(MCPU) -c $(CXXFLAGS.$(TARGET)) $(XCXXFLAGS) $<

ifeq ($(EXCLUDE),)
$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(LDFLAGS) $(XLDFLAGS) $(OBJS) $(LDEXTRA) $(XLDEXTRA)
else
NOCHECK = 1
$(PROG):
	@echo $(PROG) for $(TARGET) is being skipped: $(EXCLUDE)
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
