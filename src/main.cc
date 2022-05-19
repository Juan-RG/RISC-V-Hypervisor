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
       
        uint32_t binaryInstr = mem.read<uint32_t>(pc);
        auto opcode = binaryInstr & 0x7F;
        switch (opcode)
        {
            //tipo R 0110011
        case 19:
            i_instruction* intruction = new i_instruction(binaryInstr);
            intruction->execute(proc);
            break;

        }
        


/*        instr.execute(proc);
        exec_instrs++;
        next_pc = proc.next_pc();
  */
   } while (next_pc != pc); // look for while(1) in the code

   std::cout << "Number of executed instructions: " << exec_instrs << std::endl;
}
