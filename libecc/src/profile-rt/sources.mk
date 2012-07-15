VPATH := $(VPATH):$(SRCPATH)/profile-rt/lib
CFLAGS += -D_XOPEN_SOURCE=700
SRCS = BasicBlockTracing.c CommonProfiling.c GCDAProfiling.c PathProfiling.c \
       EdgeProfiling.c OptimalEdgeProfiling.c
