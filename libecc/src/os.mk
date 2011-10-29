# Build rules for an OS specific library.

# The target processor.
TARGET = $(shell basename `cd ../..; pwd`)
# The target operating system.
OS = $(shell basename `cd ../; pwd`)
# The base of the library name.
LIB = $(shell basename `pwd`)

# The target library.
LIBNAME = lib$(LIB).a

# The target library directory.
LIBDIR  = ../../../../lib/$(TARGET)/$(OS)

# The build compiler for this OS.
CC = ../../../../../bin/$(TARGET)-$(OS)-ecc
# The base compiler.
ECC = ../../../../../bin/ecc

# The archiver.
AR = ../../../../../bin/ecc-ar

CFLAGS = -Werror -MD -MP -O1 -fno-builtin

.SUFFIXES: .c .S .o
.c.o:
	${CC} -g -c ${CFLAGS} $<
.S.o:
	${CC} -c ${CFLAGS} $<

SRCPATH := ../../../../src

# Build the library.
VPATH :=
include $(SRCPATH)/$(LIB)/sources $(EXTRASRCS)

BASENAMES := $(basename $(filter %.c %.s %.S, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

CRTBASENAMES := $(basename $(filter %.c, $(CRTSRCS)))
CRTOBJS := $(CRTBASENAMES:%=%.o)

DEPENDSRCS := $(basename $(filter %.c %.S, $(SRCS) $(CRTSRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

all: $(CC) $(LIBNAME) $(CRTOBJS)

$(CC): $(ECC)
	ln -sf ecc $(CC)

$(LIBNAME): $(OBJS)
	$(AR) cr $(LIBNAME) $(OBJS)

install: all
	cp $(LIBNAME) $(CRTOBJS) $(LIBDIR)

clean:
	rm -f *.o *.d

veryclean: clean
	rm -f $(LIBNAME)

-include $(DEPENDFILES) ""
