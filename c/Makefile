# -*- mode: makefile-gmake; -*-
LLVM_COMPONENTS := \
jit interpreter irreader nativecodegen codegen \
bitreader bitwriter ipo linker instrumentation core mc

CLANG_LIBS := \
-lclang -lclangFrontend -lclangFrontendTool -lclangDriver -lclangSerialization \
-lclangCodeGen -lclangParse -lclangSema -lclangEdit \
-lclangAnalysis -lclangAST -lclangLex -lclangBasic\
-lclangRewriteCore -lclangRewriteFrontend -lclangStaticAnalyzerFrontend \
-lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangARCMigrate -lpthread -shared

LLVM_CONFIG = llvm-config-3.4

INC_DIR := -I/usr/include/libxml2 -I/usr/lib/llvm-3.4/include

CFLAGS += $(shell $(LLVM_CONFIG) --cflags) -std=c99 $(INC_DIR) -g
LDFLAGS += $(shell $(LLVM_CONFIG) --ldflags --libs $(LLVM_COMPONENTS))

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
EXES := $(SRCS:.c=.out)

CC = clang

all: $(EXES)

msg:
	echo $(CC)
	echo $(CFLAGS)
	echo $(LDFLAGS)


.c.o:
	echo $(CFLAGS)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

.o.out:
	$(CC) $< $(CLANG_LIBS) $(LDFLAGS) -o $@

clean:
	$(RM) *.out *.o


.PHONY:
	all msg
