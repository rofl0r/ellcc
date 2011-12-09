# Build rules for an non-OS specific library.

# The target processor.
TARGET = $(shell basename `cd ..; pwd`)

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
LIBDIR  = ../../../lib/$(TARGET)

ifeq ($(XCC),)
  # The build compiler.
  CC = ../../../../bin/$(TARGET)-elf-ecc
  # The base compiler.
  ECC = ../../../../bin/ecc

  # The archiver.
  AR = ../../../../bin/ecc-ar
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

.SUFFIXES: .c .o
.c.o:
	${CC} $(MCPU) $(MFLOAT) -g -c ${CFLAGS} $<

VPATH := ../../../src/$(LIB)

include $(VPATH)/sources

BASENAMES := $(basename $(filter %.c, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

DEPENDSRCS := $(basename $(filter %.c, $(SRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

all: $(CC) $(LIBNAME)

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
	cp $(LIBNAME) $(LIBDIR)

clean:
	rm -f *.o *.d

veryclean: clean
	rm -f $(LIBNAME)

-include $(DEPENDFILES) ""
