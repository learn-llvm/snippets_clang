def ops;
def GPR;
def Imm;
class inst<int opc, string asmstr, dag operandlist>;

class rrinst<int opc, string asmstr>
  : inst<opc, !strconcat(asmstr, " $dst, $src1, $src2"),
         (ops GPR:$dst, GPR:$src1, GPR:$src2)>;

class riinst<int opc, string asmstr>
  : inst<opc, !strconcat(asmstr, " $dst, $src1, $src2"),
         (ops GPR:$dst, GPR:$src1, Imm:$src2)>;

// Instantiations of the ri_inst multiclass.
def ADD_rr : rrinst<0b111, "add">;
def ADD_ri : riinst<0b111, "add">;
def SUB_rr : rrinst<0b101, "sub">;
def SUB_ri : riinst<0b101, "sub">;
def MUL_rr : rrinst<0b100, "mul">;
def MUL_ri : riinst<0b100, "mul">;
