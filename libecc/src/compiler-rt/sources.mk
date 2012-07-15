VPATH := $(VPATH):$(SRCPATH)/compiler-rt/lib
SRCS += umodsi3.c udivsi3.c comparedf2.c floatunsidf.c adddf3.c muldf3.c floatsidf.c \
        fixdfsi.c divdf3.c moddi3.c divdi3.c umoddi3.c udivdi3.c divsi3.c floatsisf.c \
        extendsfdf2.c extendsfdf2.c udivmoddi4.c divsf3.c mulsi3.c modsi3.c truncdfsf2.c \
        mulsf3.c fixunsdfdi.c addsf3.c floatdidf.c fixunssfdi.c fixunsdfsi.c comparedf2.c \
        comparesf2.c muldi3.c ashldi3.c lshrdi3.c subdf3.c fixdfdi.c muldc3.c mulsc3.c \
        subsf3.c fixsfsi.c floatunsisf.c fixsfdi.c
       
VPATH := $(VPATH):$(SRCPATH)/compiler-rt/lib/ppc
SRCS += gcc_qmul.c gcc_qdiv.c gcc_qsub.c gcc_qadd.c
