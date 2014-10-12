CXX=clang++
LLVM_VERSION = -3.4
LLVM_CONFIG = llvm-config$(LLVM_VERSION)
LLVM_AS = llvm-as$(LLVM_VERSION)
LLVM_LIBDIR = `$(LLVM_CONFIG) --libdir`
CXXFLAGS = `$(LLVM_CONFIG) --cxxflags`
LDFLAGS = `$(LLVM_CONFIG) --ldflags`
CLANG_LIBS = -lclang -lclangLex -lclangAST -lclangParse -lclangAnalysis -lclangRewriteCore -lclangRewriteFrontend  -lclangBasic -lclangSema -lclangSerialization -lclangDriver -lclangStaticAnalyzerCheckers -lclangFrontend -lclangStaticAnalyzerCore -lclangFrontendTool -lclangStaticAnalyzerFrontend -lclangARCMigrate -lclangASTMatchers #-lclangIndex
MY_BIN = $(HOME)/.bin
EXECUTABLE = ast

all: $(EXECUTABLE)

test: all
	@echo Running test suite ...
	cd tests && ./test.sh

clean:
	@echo Cleaning target dirs ...
	-rm -rf *.o $(EXECUTABLE)

%.o: %.cpp check_libs
	@echo Compiling $< ...
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(EXECUTABLE): $(EXECUTABLE).o
	@echo Linking $@ ...
	$(CXX) $(LDFLAGS) $(CLANG_LIBS) $< -o $@

check_libs:
	@echo Checking if LLVM exists ...
	which $(LLVM_AS)|| exit 1
	@echo Checking if clang exists ...
	which clang || exit 1
	@echo Checking if libclang.so exists ...
	[ -e $(LLVM_LIBDIR)/libclang.so ] || exit 1
