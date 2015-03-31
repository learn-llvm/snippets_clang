def ops;
def GPR;
def Imm;
class inst<int opc, string asmstr, dag operandlist>;

multiclass ri_inst<int opc, string asmstr> {
  def _rr : inst<opc, !strconcat(asmstr, " $dst, $src1, $src2"),
                 (ops GPR:$dst, GPR:$src1, GPR:$src2)>;
  def _ri : inst<opc, !strconcat(asmstr, " $dst, $src1, $src2"),
                 (ops GPR:$dst, GPR:$src1, Imm:$src2)>;
}

// Instantiations of the ri_inst multiclass.
defm ADD : ri_inst<0b111, "add">;
defm SUB : ri_inst<0b101, "sub">;
defm MUL : ri_inst<0b100, "mul">;
