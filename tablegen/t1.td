class C {
  bit V = 1;
  /* string NAME = "CC"; */
}

def X : C;

def Y : C {
  string Greeting = "hello";
}

def Z : C{
  bits<4> bs = {?, 1, ?, ?} ;
  bit b = bs{2};
}

def W : C{
  list<string> lb = [];
}

class D : C {let V = 0;}
