#include <iostream>
#include <map>

#include <instructions.hh>
#include <memory.hh>
#include <processor.hh>



#include <iomanip>
#include <cstdint>


using namespace instrs;
using namespace mem;

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
/*
       auto instr = mem.read<instrs::instruction>(pc);
       std::cout << std::setfill('0') << std::setw(8) << std::hex << instr.opcode() << '\n'; 
*/
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
       auto opcode = binaryInstr & 0x7F;
        switch (opcode)
        {
    
            
            //tipo I 0010011
        case 19 || 3:
            i_instruction* intruction = new i_instruction(binaryInstr);
            intruction->execute(proc, mem);
            break;
            //Tipo S 0100011
        case 35:    
            s_instruction* intruction2 = new s_instruction(binaryInstr);
            intruction2->execute(proc, mem);
            break;
        //0110111 tipo u lui
        case 55:
            u_instruction* intruction3 = new u_instruction(binaryInstr);
            intruction3->execute(proc, mem);
            break;        
            
     
        


/*        instr.execute(proc);
        exec_instrs++;
        next_pc = proc.next_pc();
  */
   } while (next_pc != pc); // look for while(1) in the code

   std::cout << "Number of executed instructions: " << exec_instrs << std::endl;
}
