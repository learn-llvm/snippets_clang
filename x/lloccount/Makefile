# -*- mode: makefile-gmake; -*-

CXX := clang++

LLVM_COMPONENTS := \
jit interpreter irreader nativecodegen codegen \
bitreader bitwriter ipo linker instrumentation core mc

CLANG_LIBS := \
-lclang -lclangFrontend -lclangFrontendTool -lclangDriver -lclangSerialization \
-lclangCodeGen -lclangParse -lclangSema -lclangEdit \
-lclangAnalysis -lclangAST -lclangLex -lclangBasic\
-lclangRewriteCore -lclangRewriteFrontend -lclangStaticAnalyzerFrontend \
-lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangARCMigrate

CPPFLAGS += $(shell llvm-config-3.4 --cflags) -std=c++11
LDFLAGS += $(shell llvm-config-3.4 --ldflags --libs $(LLVM_COMPONENTS))

SRCS := $(wildcard *.cc)
OBJS := $(SRCS:.cc=.o)
EXES := $(SRCS:.cc=.out)

all: $(EXES)

.cc.o:
	$(CXX) $(CPPFLAGS) -c $< -o $@

.o.out:
	$(CXX) $< $(CLANG_LIBS) $(LDFLAGS) -o $@

clean:
	$(RM) *.out *.o
