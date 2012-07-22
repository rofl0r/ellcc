# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
XCC = gcc
XCFLAGS = -std=gnu99 -D__host__
DOFF = ../../../../../../libecc
XCFLAGS += -I../../../../../../test/include
XLDEXTRA = -lm
EXE = ./
