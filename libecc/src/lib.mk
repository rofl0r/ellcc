# Build rules for an non-OS specific library.

# The target processor.
TARGET = $(shell basename `cd ..; pwd`)
ARCH ?= $(TARGET)
# The base of the library name.
LIB = $(shell basename `pwd`)

# The target library.
LIBNAME = lib$(LIB).a

# The target library directory.
LIBDIR  = ../../../lib/$(TARGET)

# The build compiler.
CC = ../../../../bin/$(TARGET)-elf-ecc
# The base compiler.
ECC = ../../../../bin/ecc

# The archiver.
AR = ../../../../bin/ecc-ar

CFLAGS = -Werror -MD -MP -O1

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

$(CC): $(ECC)
	ln -sf ecc $(CC)

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
