# this is the Makefile for ecc, the Embedded Compiler Collection driver

# Get the target compiler names.
CCS=$(foreach target, $(shell cat ../targets),$(target)-ecc)
CXXS=$(foreach target, $(shell cat ../targets),$(target)-e++)
CXXS+=e++

# C++ preprocessor, compiler and linker
CXX := g++
CCFLAGS = -I../include
LDFLAGS := -L../lib
LLVMCCFLAGS := $(shell ../bin/llvm-config --cxxflags) -fexceptions
LLVMTARGETS := $(shell ../bin/llvm-config --targets-built)
LLVMLDFLAGS := -lclangFrontend -lclangDriver -lclangCodeGen -lclangSema \
	       -lclangSerialization -lclangIndex \
	       -lclangChecker -lclangAnalysis \
	       -lclangRewrite -lclangAST -lclangParse -lclangLex -lclangBasic \
	       $(shell ../bin/llvm-config --ldflags --libs \
	       $(LLVMTARGETS) bitreader bitwriter instrumentation scalaropts ipo linker)

# list of files to clean in 'clean' (etc.) targets
# (these get added to below)
TOCLEAN =
TOTOOLCLEAN =
TODISTCLEAN =

# compile .cc in this directory to a .o
TOCLEAN += *.o *.d
%.o: %.cpp
	$(CXX) -c -o $@ $< $(if $(findstring $*,$(GCOV_MODS)),$(GCOV_OPTS) )$(CCFLAGS) $(LLVMCCFLAGS)
	@$(CXX) -c -MM -o $*.d $< $(if $(findstring $*,$(GCOV_MODS)),$(GCOV_OPTS) )$(CCFLAGS) $(LLVMCCFLAGS)

# ------------------------- ecc ---------------------

# list of modules needed for the parser; ideally they're in an order
# that finds serious compilation problems earliest (it's ok to
# rearrange as different parts of the code are in flux)
ECC_OBJS := ecc.o

-include $(ECC_OBJS:.o=.d)

# ecc binary
TOCLEAN += ecc
ecc: $(ECC_OBJS) $(libraries)
	$(CXX) -o $@ $(ECC_OBJS) $(LDFLAGS) $(LLVMLDFLAGS)
	@cp ecc ../bin

$(CCS) $(CXXS): ecc
	@cd ../bin; ln -fs ecc $@
