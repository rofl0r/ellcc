VPATH := $(VPATH):$(SRCPATH)/profile-rt/lib
CFLAGS += -D_XOPEN_SOURCE=700
SRCS = BasicBlockTracing.c CommonProfiling.c EdgeProfiling.c OptimalEdgeProfiling.c \
       PathProfiling.c
