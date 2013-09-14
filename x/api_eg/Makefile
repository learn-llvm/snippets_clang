

all : build_executable compile_to_module compile_to_module-return


build_executable :
	g++ -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -I/usr/lib/llvm-3.4/include  -L/usr/lib/llvm-3.4/lib -fvisibility-inlines-hidden -lclangAnalysis -lclangAST -lclangBasic -lclangCodeGen -lclangDriver -lclangEdit -lclangFrontend -lclangLex -lclangParse -lclangRewrite -lclangSema -lclangSerialization -lclangTooling -lLLVMSupport -lLLVMMC -o build_executable build_executable.cpp


compile_to_module : 
	g++ -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS  -I/usr/lib/llvm-3.4/include  -L/usr/lib/llvm-3.4/lib -fvisibility-inlines-hidden -lclangAnalysis -lclangAST -lclangBasic -lclangCodeGen -lclangDriver -lclangEdit -lclangFrontend -lclangLex -lclangParse -lclangRewrite -lclangSema -lclangSerialization -lclangTooling -lLLVMAnalysis -lLLVMArchive -lLLVMAsmParser -lLLVMAsmPrinter -lLLVMBitReader -lLLVMBitWriter -lLLVMCodeGen -lLLVMCore -lLLVMExecutionEngine -lLLVMInstCombine -lLLVMInstrumentation -lLLVMInterpreter -lLLVMJIT -lLLVMLinker -lLLVMMC -lLLVMMCParser -lLLVMScalarOpts -lLLVMSelectionDAG -lLLVMSupport -lLLVMTarget -lLLVMTransformUtils -lLLVMVectorize -lLLVMX86AsmParser -lLLVMX86AsmPrinter -lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMX86Utils -lLLVMipa -lLLVMipo -o compile_to_module compile_to_module.cpp


compile_to_module-return :
	g++ -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -I/usr/lib/llvm-3.4/include  -L/usr/lib/llvm-3.4/lib -fvisibility-inlines-hidden -lclangAnalysis -lclangAST -lclangBasic -lclangCodeGen -lclangDriver -lclangEdit -lclangFrontend -lclangLex -lclangParse -lclangRewrite -lclangSema -lclangSerialization -lclangTooling -lLLVMAnalysis -lLLVMArchive -lLLVMAsmParser -lLLVMAsmPrinter -lLLVMBitReader -lLLVMBitWriter -lLLVMCodeGen -lLLVMCore -lLLVMExecutionEngine -lLLVMInstCombine -lLLVMInstrumentation -lLLVMInterpreter -lLLVMJIT -lLLVMLinker -lLLVMMC -lLLVMMCParser -lLLVMScalarOpts -lLLVMSelectionDAG -lLLVMSupport -lLLVMTarget -lLLVMTransformUtils -lLLVMVectorize -lLLVMX85AsmParser -lLLVMX86AsmPrinter -lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMX86Utils -lLLVMipa -lLLVMipo -o compile_to_module-return compile_to_module-return.cpp

