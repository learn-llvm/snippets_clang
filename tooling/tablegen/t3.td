class ModRefVal<bits<2> val> {
  bits<2> Value = val;
}

def None   : ModRefVal<0>;
def Mod    : ModRefVal<1>;
def Ref    : ModRefVal<2>;
def ModRef : ModRefVal<3>;

class Value<ModRefVal MR> {
  // Decode some information into a more convenient format, while providing
  // a nice interface to the user of the "Value" class.
  bit isMod = MR.Value{0};
  bit isRef = MR.Value{1};

  // other stuff...
}

// Example uses
def bork : Value<Mod>;
def zork : Value<Ref>;
def hork : Value<ModRef>;
