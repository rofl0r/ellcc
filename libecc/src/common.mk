# The target processor.
TARGET = $(shell basename `cd $(LEVEL); pwd`)

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
LIBDIR  = $(LEVEL)/../../lib/$(TARGET)$(OSDIR)

ifeq ($(XCC),)
  # The build compiler.
  CC = $(LEVEL)/../../../bin/$(TARGET)-$(OS)-ecc
  # The base compiler.
  ECC = $(LEVEL)/../../../bin/ecc

  # The archiver.
  AR = $(LEVEL)/../../../bin/ecc-ar
else
  CC = $(XCC)
endif

CFLAGS += -Werror -MD -MP -O1

ifdef CPU
    MCPU = -mcpu=$(CPU)
endif

ifdef FLOAT
    MFLOAT = -m$(FLOAT)-float
endif

.SUFFIXES: .c .S .o
.c.o:
	${CC} $(MCPU) $(MFLOAT) -c ${CFLAGS} $<
.S.o:
	${CC} $(MCPU) -c ${CFLAGS} $<


# Build the library.
SRCPATH := $(LEVEL)/../../src
VPATH :=
include $(SRCPATH)/$(LIB)/sources $(EXTRASRCS)

BASENAMES := $(basename $(filter %.c %.S, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

CRTBASENAMES := $(basename $(filter %.c, $(CRTSRCS)))
CRTOBJS := $(CRTBASENAMES:%=%.o)

DEPENDSRCS := $(basename $(filter %.c %.S, $(SRCS) $(CRTSRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

all: $(CC) $(LIBNAME) $(CRTOBJS)

ifneq ($(ECC),)
$(CC): $(ECC)
	ln -sf ecc $(CC)
else
$(CC):
endif

$(LIBNAME): $(OBJS)
	$(AR) cr $(LIBNAME) $(OBJS)

install: all
	mkdir -p $(LIBDIR)
	cp $(LIBNAME) $(CRTOBJS) $(LIBDIR)

clean:
	rm -f *.o *.d

veryclean: clean
	rm -f $(LIBNAME)

-include $(DEPENDFILES) ""

