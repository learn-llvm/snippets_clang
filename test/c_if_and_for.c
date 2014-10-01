#include <assert.h>
void foo(int* a, int* b) {
  if (a[0] > 1) {
    b[0] = 2;
  }
}

void bar(float x, float y);  // just a declaration

void bang(int* a, int v) {
  for (int i = 0; i < v; ++i) {
    a[i] -= i;
  }
}

int main(void) {
  int a, b, v;
  int arr[10];
  foo(arr, v);
  bar(a, b);
  bang(arr, v);
}
