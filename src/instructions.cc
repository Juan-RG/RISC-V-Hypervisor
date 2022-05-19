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

    
// ...

