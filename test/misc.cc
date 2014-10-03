#include <assert.h>

int gint;
float gfloat;

float** gppfloat;

typedef union {
  double dnum;
  int inum;
  float* fptr;
} my_union;

int bar(my_union* mu) { return mu[4].inum; }

void foo(int* a, int* b) {
  if (a[0] > 1) {
    b[0] = 2;
  }
}

void bar(float x, float y) { x + y; }  // just a declaration

float baz() { return gfloat + **gppfloat; }

void bang(int* arr, int v) {
  for (int i = 0; i < v; ++i) {
    arr[i] -= i;
  }
}

template <typename T, int N>
T foo(T a, T b) {
  T k = a;
  for (int i = 0; i < N; ++i) {
    k = k * a + b;
  }
  return k;
}

int main(void) {
  int a = 1, b = 2, v = 3;
  int arr[10], brr[10];
  foo(arr, brr);
  bar(a, b);
  bang(arr, v);
}
