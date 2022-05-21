#include <iostream>
#include <map>

#include <instructions.hh>
#include <memory.hh>
#include <processor.hh>

#include <iomanip>
#include <cstdint>

//typedef uint32_t uint32_t;

/*Instrucciones
  0:   fe010113                addi    sp,sp,-32                   imm[11:0] rs1 000 rd 0010011 ADDI                 
   4:   00812e23                sw      s0,28(sp)                  imm[11:5] rs2 rs1 010 imm[4:0] 0100011 SW
  20:   fe842783                lw      a5,-24(s0)                 imm[11:0] rs1 010 rd 0000011 LW
  18:   000017b7                lui     a5,0x1                     imm[31:12] rd 0110111 LUI
  24:   00279793                slli    a5,a5,0x2                  0000000 shamt rs1 001 rd 0010011 SLLI
  28:   00f707b3                add     a5,a4,a5                   0000000 rs2 rs1 000 rd 0110011 ADD
  4c:   00300793                li      a5,3                       110000000001111 0010011     // ¿?¿? esto ni idea  010 imm[5] rd imm[4:0] 01  
  50:   fce7d4e3                bge     a5,a4,18 <main+0x18>       imm[12|10:5] rs2 rs1 101 imm[4:1|11] 1100011 BGE
  54:   0000006f                j       54 <main+0x54>             imm[20|10:1|11|19:12] rd 1101111 JAL
*/



using namespace instrs;
using namespace mem;


std::map<uint32_t, type> handlers = {
   {0b0000000, type::base},   // nop
   {0b0010011, type::i},      // addi
   {0b0100011, type::s},      // store word
   {0b0110111, type::u},      // lui Load Upper Immediate U
   {0b0000011, type::i},      // lw Load Word
   {0b0010011, type::i},      // slli
   {0b0110011, type::r},      // add
   {0b0010011, type::i},      // li
   {0b1100011, type::b},      // bge
   {0b1101111, type::j}       // j
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Invalid Syntax: hermes <program>" << std::endl;
        exit(1);
    }

   memory mem;
   processor proc;

   mem.load_binary(argv[1]);

   
   // read the entry point
   // ...

   proc.write_pc(mem.entry_point());

   // set the stack pointer
   // the stack grows downward with the stack pointer always being 16-byte aligned
   proc.write_reg(processor::sp, memory::stack_top);

   address_t pc =0xDEADBEEF, next_pc = 0xDEADBEEF;

   size_t exec_instrs = 0;
   do
   {
       pc = proc.read_pc();   


    //std::cout << std::setfill('0') << std::setw(8) << std::hex << instr.opcode() << '\n'; 


/*Instrucciones



  0:   fe010113                addi    sp,sp,-32                   imm[11:0] rs1 000 rd 0010011 ADDI                 
   4:   00812e23                sw      s0,28(sp)                  imm[11:5] rs2 rs1 010 imm[4:0] 0100011 SW
  20:   fe842783                lw      a5,-24(s0)                 imm[11:0] rs1 010 rd 0000011 LW
  18:   000017b7                lui     a5,0x1                     imm[31:12] rd 0110111 LUI
  24:   00279793                slli    a5,a5,0x2                  0000000 shamt rs1 001 rd 0010011 SLLI
  28:   00f707b3                add     a5,a4,a5                   0000000 rs2 rs1 000 rd 0110011 ADD

  4c:   00300793                li      a5,3                       110000000001111 0010011     // ¿?¿? esto ni idea  010 imm[5] rd imm[4:0] 01  
  50:   fce7d4e3                bge     a5,a4,18 <main+0x18>       imm[12|10:5] rs2 rs1 101 imm[4:1|11] 1100011 BGE
  54:   0000006f                j       54 <main+0x54>             imm[20|10:1|11|19:12] rd 1101111 JAL


*/

       
        uint32_t binaryInstr = mem.read<uint32_t>(pc);
        std::cout << std::setfill('0') << std::setw(8) << std::hex << binaryInstr << '\n'; 
        uint32_t opcode = binaryInstr & 0x7F;
        instruction* instr;
        switch (handlers[opcode])
        {
           // case type::base:
            //    instr = new intruction(binaryInstr);
                //ins.execute();
            case type::r:
                instr = new r_instruction(binaryInstr);
                //ins.execute(proc, mem);
                break;
            case type::i:
                instr = new i_instruction(binaryInstr);
                //ins.execute(proc, mem);
                break;
            case type::s:    
                instr = new s_instruction(binaryInstr);
                //ins.execute(proc, mem);
                break;
            case type::b:
                instr = new b_instruction(binaryInstr);
                //ins.execute(proc, mem);
                break;
            case type::u:
                instr = new u_instruction(binaryInstr);
                //ins.execute(proc, mem);
                break;
            case type::j:
                instr = new j_instruction(binaryInstr);
                //ins.execute(proc, mem);
                break;
            default:
                std::cerr << "Unexpected Opcode detected" << opcode << std::endl;
        }


        instr->execute(proc, mem);
        exec_instrs++;
        next_pc = proc.next_pc();
        
   } while (next_pc != pc); // look for while(1) in the code

    auto instr1 = new i_instruction(4274267907);
    instr1->execute(proc, mem);
    printf("%d\n", proc.read_reg(14)); 
   std::cout << "Number of executed instructions: " << exec_instrs << std::endl;
}
