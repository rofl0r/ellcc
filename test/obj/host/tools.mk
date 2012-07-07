# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
XCC = gcc
# ARCH should be one of arm, i386, microblaze, mips, nios2, ppc64, ppc, sparc, or x86_64.
ARCH := $(shell uname -m)
XCFLAGS = -std=gnu99 -D__host__
DOFF = ../../../../../../libecc
XCFLAGS += -I../../../../../../test/include
XLDEXTRA = -lm
EXE = ./
