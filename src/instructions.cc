#include <instructions.hh>
#include <memory.hh>

using namespace instrs;
using namespace mem;

using namespace std;


void i_instruction::execute(processor &proc, memory &mem)
{
    uint8_t _funct = this->funct3();
    uint8_t _rd = this->rd();
    uint8_t _rs = this->rs1();

    uint32_t _valueRS = proc.read_reg(_rs);

    switch (_funct)
    {
        case 0:
            //ADDI            
            proc.write_reg(_rd, _valueRS + _imm);
            break;
        case 1:
        //SLLI //Desplaza el valor de RS con los 4 bits
            proc.write_reg(_rd, _valueRS << bits(20, 5));
            break;

        case 2:
            // LW
            proc.write_reg(_rd, mem.read<uint32_t>(static_cast<uint32_t>(_valueRS + _imm)));
            break;
    }
}

void s_instruction::execute(processor &proc, memory &mem)
{
    uint8_t _funct = this->funct3();
    uint8_t _rs = this->rs1();
    uint8_t _rs2 = this->rs2();

    uint32_t _valueRS = proc.read_reg(_rs);
    uint32_t _valueRS2 = proc.read_reg(_rs2);

    switch (_funct)
    {
        case 2:
            // SW
            mem.write(_valueRS + _imm, _valueRS2);
            break;
    }
}


 void u_instruction::execute(processor &proc, memory &mem){
        mem.read<uint32_t>(0);
        uint8_t _funct = this->opcode();
        uint8_t _rd = this->rd();    
        switch (_funct)
        {
            case 55:
                //LUI
                proc.write_reg(_rd, _imm);
                break;
        }

 }

 void r_instruction::execute(processor &proc, memory &mem){
        mem.read<uint32_t>(0);
        uint8_t _funct = this->funct3();
        uint8_t _rd = this->rd();
        uint8_t _rs = this->rs1();
        uint8_t _rs2 = this->rs2();
        //uint8_t _funct7 = this->funct7();

        switch (_funct)
        {
        case 0:
            //ADD
            proc.write_reg(_rd, proc.read_reg(_rs) + proc.read_reg(_rs2));
            break;
        
        default:
            break;
        }

 }
 void b_instruction::execute(processor &proc, memory &mem){
        mem.read<uint32_t>(0);
        uint8_t _funct = this->funct3();
        uint8_t _rs = this->rs1();
        uint8_t _rs2 = this->rs2();

        switch (_funct)
        {
        case 5:
            //BGE
            if(proc.read_reg(_rs) >= proc.read_reg(_rs2)){
                proc.write_pc(proc.read_pc() + _imm);
            }
            break;
        }
 }
// ...

void j_instruction::execute(processor &proc, memory &mem)   {
        mem.read<uint32_t>(0);
        uint8_t _funct = this->opcode();
        //uint8_t _rd = this->rd();
        switch (_funct)
        {
            case 0b1101111:     // JAL 
                int32_t destination = static_cast<int32_t>(proc.read_pc()) + this->offset();
                proc.write_pc( static_cast<uint32_t>( destination ) );
                break;
        }
}

