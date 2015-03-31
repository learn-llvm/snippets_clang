
warnglobals.so: WarnGlobals.cpp
	clang++ -std=c++14 $$(llvm-config --cflags) $$(llvm-config --ldflags) -shared -fPIC -o "$@" $^ -L/usr/lib/llvm/ -lclang

test.o: warnglobals.so
	clang++ -Xclang -load -Xclang ./warnglobals.so -Xclang -add-plugin -Xclang warn-globals -o "$@" -c test.cpp

test: warnglobals.so
	clang++ -Xclang -load -Xclang ./warnglobals.so -Xclang -add-plugin -Xclang warn-globals -o "$@" test.cpp

.PHONY: clean
clean:
	rm -f warnglobals.so test.o test
