# Build rules for an OS specific library.
LEVEL=../..
OS = $(shell basename `cd ../; pwd`)
OSDIR = /$(OS)
CFLAGS += -fno-builtin
include $(LEVEL)/../../src/common.mk
