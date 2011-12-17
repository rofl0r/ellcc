BASENAMES := $(basename $(filter %.c %.S, $(SRCS)))
OBJS := $(BASENAMES:%=%.o)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS)

clean:
	rm -fr *.o

install:
