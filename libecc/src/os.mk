# Build rules for an OS specific library.

# The target library.
LIBNAME = libos.a

# The target processor.
TARGET = $(shell basename `cd ..; pwd`)
OS = $(shell basename `pwd`)

# The target library directory.
LIBDIR  = ../../../lib/$(TARGET)/$(OS)

# The build compiler for this OS.
CC = ../../../../bin/$(TARGET)-$(OS)-ecc
# The base compiler.
ECC = ../../../../bin/ecc

# The archiver.
AR = ../../../../bin/ecc-ar

CFLAGS = -g -Werror -MD -MP

.SUFFIXES: .c .S .o
.c.o:
	${CC} -c ${CFLAGS} $<
.S.o:
	${CC} -c ${CFLAGS} $<

VPATH := ../../../src/$(OS)

include $(VPATH)/sources

BASENAMES := $(basename $(filter %.c %.s %.S, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

CRTBASENAMES := $(basename $(filter %.c, $(CRTSRCS)))
CRTOBJS := $(CRTBASENAMES:%=%.o)

DEPENDSRCS := $(basename $(filter %.c %.S, $(SRCS) $(CRTSRCS)))
DEPENDFILES := $(DEPENDSRCS:%=%.d)

lib: $(CC) $(LIBNAME) $(CRTOBJS)

$(CC): $(ECC)
	ln -sf ecc $(CC)

$(LIBNAME): $(OBJS)
	$(AR) cr $(LIBNAME) $(OBJS)

install: lib
	cp $(LIBNAME) $(CRTOBJS) $(LIBDIR)

clean:
	rm -f *.o *.d

veryclean: clean
	rm -f $(LIBNAME)

-include $(DEPENDFILES) ""
