# Build rules for an OS specific library.
LEVEL=../..
OS = $(shell basename `cd ../; pwd`)
OSDIR = /$(OS)
CFLAGS += -fno-builtin
include $(LEVEL)/../../mkscripts/common.mk
