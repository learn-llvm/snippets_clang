#include "stdio.h"
unsigned char c1;
unsigned char c2 = 1;
int main() {
  unsigned int add(int, int);
  int a = 0, b = 1;
  int c;
  c = add(a, b);
  printf("%d\n", c);
  return 0;
}

unsigned int add(int a, int b) { return a + b; }
