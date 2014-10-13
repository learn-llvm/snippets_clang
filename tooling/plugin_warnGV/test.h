#ifndef TEST_H
#define TEST_H

namespace some_namespace {
namespace some_sub_namespace {
int public_global = 42;
}

class some_class {
  static const int static_const_member = 42;
  static int static_member;
};
}

#endif
