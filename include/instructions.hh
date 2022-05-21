#pragma once

#include <cstdint>
#include <functional>

#include <memory.hh>
#include <processor.hh>

// extracted from the riscv-spec-20191213.pdf

#include <iomanip>
#include <iostream>

namespace instrs {


enum class type {base, r, i, s, b, u, j};

class instruction {
    protected:
        instrs::type _type;
        uint32_t _bitstream;
    public:
        // check Figure 2.4 from the manual
        constexpr uint32_t bits(size_t lsb, size_t len) const
        {
            return (_bitstream >> lsb) & ((static_cast<uint32_t>(1) << len)-1);
        }
        instruction(uint32_t bitstream, instrs::type type=type::base) : _type(type),
        _bitstream(bitstream) {}
        //virtual ~instruction() {}	
        constexpr instrs::type type() const { return _type; }
        constexpr uint8_t opcode() const { return bits(0,7); }
       // virtual void execute(processor &proc);
       //void execute(processor &proc, memory &mem);
       
};

class r_instruction : public instruction {
    public:
        r_instruction(uint32_t bitstream) :
            instruction(bitstream, type::r) {
            }
        //~r_instruction() override{}    
        constexpr uint8_t rd() const { return bits(7, 5); }
        constexpr uint8_t funct3() const { return bits(12, 3); }
        constexpr uint8_t rs1() const { return bits(15, 5); }
        constexpr uint8_t rs2() const { return bits(20, 5); }
        constexpr uint32_t funct7() const { return bits(25, 7); }
        //virtual void execute(processor &proc, memory &mem) override;
        void execute(processor &proc, memory &mem);        
};

class i_instruction : public instruction {
    private:
      int32_t _imm;
    public:
        i_instruction(uint32_t bitstream) :
            instruction(bitstream, type::i) {
            _imm = bits(31, 1) ? 0xFFFFF800 : 0x0;
            _imm |= bits(20, 12);
        }
       // ~i_instruction() override {}	
        constexpr uint8_t rd() const { return bits(7, 5); }
        constexpr uint8_t funct3() const { return bits(12, 3); }
        constexpr uint8_t rs1() const { return bits(15, 5); }
        constexpr uint32_t imm() const { return bits(20, 12); } // imm[11:0]
        //virtual void execute(processor &proc) override;
        void execute(processor &proc, memory &mem);
};

class s_instruction : public instruction {
    private:
        int32_t _imm;
    public:
        s_instruction(uint32_t bitstream) :
        instruction(bitstream, type::s) {
            _imm = bits(31, 1) ? 0xFFFFF800 : 0x0;
/*            if(bits(31, 1) == 1){
                _imm = 0xFFFFF800;
            }
*/
            _imm |= bits(7,5);
            _imm |= (bits(25,7) << 5);

        }
        constexpr uint8_t funct3() const { return bits(12, 3); }
        constexpr uint8_t rs1() const { return bits(15, 5); }
        constexpr uint8_t rs2() const { return bits(20, 5); }
        constexpr uint32_t imm() const { return _imm; } // imm[11:5] 
        void execute(processor &proc, memory &mem);
};


class u_instruction : public instruction {
    private:
        int32_t _imm;
    public:
        u_instruction(uint32_t bitstream) :
            instruction(bitstream, type::u) {
            _imm = 0x0;
            _imm |= bits(12, 20) << 12;
        }
        constexpr uint8_t opcode() const { return bits(0, 7); }
        constexpr uint8_t rd() const { return bits(7, 5); }
        constexpr uint32_t imm() const { return bits(12, 20); } // imm[31:12]
        
        void execute(processor &proc, memory &mem);
};


class b_instruction : public instruction {
    private:
        int32_t _imm;    
    public:
        b_instruction(uint32_t bitstream) :
            instruction(bitstream, type::b) {
            _imm = bits(31, 1) ? 0xFFFFF800 : 0x0;
            //bits(7, 1);
            _imm |= bits(8, 4) << 1;
            _imm |= bits(25, 6) << 5;
            _imm |= bits(7, 1) << 11;
            _imm |= bits(31, 1) << 12;
        }
        //constexpr uint8_t imm() const { return bits(7, 5); } // imm[4:1|11] 
        constexpr uint8_t funct3() const { return bits(12, 3); }
        constexpr uint8_t rs1() const { return bits(15, 5); }
        constexpr uint8_t rs2() const { return bits(20, 5); }
        void execute(processor &proc, memory &mem);
        //constexpr uint32_t imm() const { return bits(25, 7); } // imm[12|10:5] 
};

/*

class j_instruction : public instruction {
    public:
        j_instruction(uint32_t bitstream) :
            instruction(bitstream, type::j) {}
        constexpr uint8_t rd() const { return bits(7, 5); }
        constexpr uint32_t imm() const { return bits(20, 12); } // imm[20|10:1|11|19:12]
};
*/

} // namespace instrs
