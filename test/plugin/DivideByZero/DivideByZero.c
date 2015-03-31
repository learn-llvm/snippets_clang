//RUN: %clang_cc1 -analyze -analyzer-checker=chx -verify %s

int main(void){
  int a = 0;

 return 3 / a;  // expected-warning{{my-divide-by-zero}}
}
