#include <iostream>
/**
 * @file   comment.cc
 * @author Hongxu Chen <leftcopy.chx@gmail.com>
 * @date   Thu Oct  2 10:03:38 2014
 *
 * @brief Not much
 *
 *
 */

using std::cout;

/**
 * @brief add two objects
 *
 * @param t1 first parameter
 * @param T2 second parameter
 *
 * @return the added two objects
 */
template <typename T1, typename T2>
auto sum(T1 const &t1, T2 const &t2) -> decltype(t1 + t2) {
  return t1 + t2;
}

int main(void) {
  auto i = 1u;
  auto f = 1.0f;
  cout << sum(i, f) << '\n';
  return 0;
}
