//RUN: %clang -c -Xclang -analyze -Xclang -analyzer-checker=chx -Xclang -verify %s -o /dev/null

#include <stdlib.h>

int local_zero(void) {
  int a = 0;
  return 3 / a;  // expected-warning{{chx.DBZ - DBZ}}
}

int rand_zero(void) {
  int a = rand();
  return 3 / a; // expected-warning{{chx.DBZ - tainted DBZ}}
}

static int ga = 0;
extern int gb;
int static_global_zero (int v) {
  int ra = v / ga;
  int rb = v / gb;
  return ra + rb;
}

int test4(int b) {
  return 1 / b; // no-warning
}
