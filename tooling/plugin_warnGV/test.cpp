#include "test.h"
#include <iostream>

static int s_some_global = 42;
static const int s_some_const_global = 42;

static void some_function_with_a_static() { static int s_static_local = 42; }

int some_namespace::some_class::static_member = 42;

int main(int argc, char **argv) {
  std::cout << "global value: " << s_some_global << "\n";
  return 0;
}
