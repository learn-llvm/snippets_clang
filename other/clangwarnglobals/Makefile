
warnglobals.so: WarnGlobals.cpp
	clang++ -std=c++98 $$(llvm-config --cflags --ldflags) -shared -fPIC -o "$@" $^ -L/usr/lib/llvm/ -lclang

test.o: warnglobals.so
	clang++ -Xclang -load -Xclang ./warnglobals.so -Xclang -add-plugin -Xclang warn-globals -o "$@" -c test.cpp

test: warnglobals.so
	clang++ -Xclang -load -Xclang ./warnglobals.so -Xclang -add-plugin -Xclang warn-globals -o "$@" test.cpp

.PHONY: clean
clean:
	rm -f warnglobals.so test.o test
