#include <stdio.h>
int sum(int a, int b) {
  return a + b;
}

int main(void) {
  printf("sum: %d\n", sum(2, 3) + sum(3, 4));
  return 0;
}
