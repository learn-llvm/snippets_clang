#include <stdio.h>
#include <
#define myPrint printf
#define customPrint(st) myPrint("%d\n", st.f)

#line 100
struct Foo;
struct Foo;
typedef struct Foo *pFoo;

typedef struct Foo {
  int f;
} Foo;

int main(void) {
  Foo foo = {2};
  customPrint(foo);
  return 0;
}
