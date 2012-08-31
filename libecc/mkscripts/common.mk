# The target processor.
TARGET := $(shell basename `cd $(LEVEL); pwd`)
# Check for code coverage.
COV := $(shell basename $(TARGET) .cov)
ifneq ($(TARGET),$(COV))
    TARGET := $(COV)
    CFLAGS += -fprofile-arcs -ftest-coverage
    CXXFLAGS += -fprofile-arcs -ftest-coverage
    LDFLAGS += -coverage
    COV := .cov
else
    COV :=
endif

# Determine the architecture.
ifneq ($(filter arm%, $(TARGET)),)
  ARCH := arm
else ifneq ($(filter i386%, $(TARGET)),)
  ARCH := i386
else ifneq ($(filter microblaze%, $(TARGET)),)
  ARCH := microblaze
else ifneq ($(filter mips%, $(TARGET)),)
  ARCH := mips
else ifneq ($(filter nios2%, $(TARGET)),)
  ARCH := nios2
else ifneq ($(filter ppc64%, $(TARGET)),)
  ARCH := ppc64
else ifneq ($(filter ppc%, $(TARGET)),)
  ARCH := ppc
else ifneq ($(filter sparc%, $(TARGET)),)
  ARCH := sparc
else ifneq ($(filter x86_64%, $(TARGET)),)
  ARCH := x86_64
endif
ARCH ?= $(TARGET)

# The base of the library name.
LIB = $(shell basename `pwd`)

# The target library.
LIBNAME = lib$(LIB).a

# The target library directory.
LIBDIR  = $(LEVEL)/../../lib/$(TARGET)$(COV)$(OSDIR)

ifeq ($(XCC),)
  # The build compiler.
  CC = $(LEVEL)/../../../bin/ecc -target $(TARGET)-$(OS)-ellcc

  # The archiver.
  AR = $(LEVEL)/../../../bin/ecc-ar
else
  CC = $(XCC)
endif

ifeq ($(XCXX),)
  # The build C++ compiler.
  CXX = $(LEVEL)/../../../bin/ecc++ -target $(TARGET)-$(OS)-ellcc
else
  CXX = $(XCXX)
endif

CFLAGS += -Werror -MD -MP -O1

ifndef NODEBUG
  CFLAGS += -g
  LDFLAGS += -g
endif

ifdef TARGET_CPU
    MCPU = -mcpu=$(TARGET_CPU)
endif

ifdef FLOAT
    MFLOAT = -m$(FLOAT)-float
endif

.SUFFIXES: .c .cpp .cxx .S .o
.c.o:
	${CC} $(MCPU) $(MFLOAT) -c ${CFLAGS} $<
.cpp.o:
	${CXX} $(MCPU) $(MFLOAT) -c ${CXXFLAGS} $<
.cxx.o:
	${CXX} $(MCPU) $(MFLOAT) -c ${CXXFLAGS} $<
.S.o:
	${CC} $(MCPU) -c ${CFLAGS} $<


# Build the library.
SRCPATH := $(LEVEL)/../../src
VPATH :=
include $(SRCPATH)/$(LIB)/sources.mk $(EXTRASRCS)

BASENAMES := $(basename $(filter %.c %.cxx %.cpp %.S, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

CRTBASENAMES := $(basename $(filter %.c, $(CRTSRCS)))
CRTOBJS := $(CRTBASENAMES:%=%.o)

DEPENDSRCS := $(basename $(filter %.c %.cxx %.cpp %.S, $(SRCS) $(CRTSRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

all: $(LIBNAME) $(CRTOBJS)

$(LIBNAME): $(OBJS)
	$(AR) cr $(LIBNAME) $(OBJS)

install: all
	mkdir -p $(LIBDIR)
	cp $(LIBNAME) $(CRTOBJS) $(LIBDIR)

clean:
	rm -f *.o *.d *.gcda *.gcno

veryclean: clean
	rm -f $(LIBNAME)

-include $(DEPENDFILES) ""
