#include <instructions.hh>
#include <memory.hh>

using namespace instrs;
using namespace mem;

using namespace std;


void i_instruction::execute(processor &proc)
{
    uint8_t _funct = this->funct3();
    uint8_t _rd = this->rd();
    uint8_t _rs = this->rs1();

    uint32_t _valueRS = proc.read_reg(_rs);

    switch (_funct)
    {
        case 0:            
            proc.write_reg(_rd, _valueRS + _imm);
            break;
        case 1:
            // slli
            break;
    }
}

void s_instruction::execute(processor &proc, memory &mem)
{
    uint8_t _funct = this->funct3();
    uint8_t _rd = this->rd();
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
    
// ...

