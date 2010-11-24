# Build rules for an OS specific library.

# The target library.
LIBNAME = libos.a

# The target processor.
TARGET = $(shell basename `cd ..; pwd`)
OS = $(shell basename `pwd`)

# The target library directory.
LIBDIR  = ../../../lib/$(TARGET)/$(OS)

# The build compiler.
CC = ../../../../bin/$(TARGET)-$(OS)-ecc
# The archiver.
AR = ../../../../bin/ecc-ar

CFLAGS = -Werror

.SUFFIXES: .c .o
.c.o:
	${CC} -c ${CFLAGS} $<

# Startup code.
CRTSRCS = ../../../src/$(OS)/crt/crt0.c

VPATH = ../../../src/$(OS)

include $(VPATH)/sources

BASENAMES = $(basename $(filter %.c, $(SRCS)))
OBJS = $(BASENAMES:%=%.o)

$(LIBNAME): $(OBJS)
	$(AR) cr $(LIBNAME) $(OBJS)

clean:
	rm -f $(OBJS)

veryclean: clean
	rm -f $(LIBNAME)
