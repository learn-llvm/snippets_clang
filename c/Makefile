# -*- mode: makefile-gmake; -*-
LLVM_COMPONENTS := \
jit interpreter irreader nativecodegen codegen \
bitreader bitwriter ipo linker instrumentation core mc

CLANG_LIBS := \
-lclang -lclangFrontend -lclangFrontendTool -lclangDriver -lclangSerialization \
-lclangCodeGen -lclangParse -lclangSema -lclangEdit \
-lclangAnalysis -lclangAST -lclangLex -lclangBasic\
-lclangRewriteCore -lclangRewriteFrontend -lclangStaticAnalyzerFrontend \
-lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangARCMigrate 

CFLAGS += $(shell llvm-config-3.4 --cflags) -std=c99
LDFLAGS += $(shell llvm-config-3.4 --ldflags --libs $(LLVM_COMPONENTS))

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
EXES := $(SRCS:.c=.out)

all: $(EXES)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.o.out:
	$(CC) $< $(CLANG_LIBS) $(LDFLAGS) -o $@

clean:
	$(RM) *.out *.o
