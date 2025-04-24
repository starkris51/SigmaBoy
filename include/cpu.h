#include "registers.h"
#include "mmu.h"

class CPU
{
public:
    CPU(MMU *memory, Registers *registers);

    MMU *memory;
    Registers *registers;

    void Execute(uint8_t opcode);
    void Step();

    // CPU Instructions
    void Add(uint8_t value);
    void AddHL(uint16_t value);
    void Sub(uint8_t value);
    void Inc(uint8_t &value);
    void Dec(uint8_t &value);
    void And(uint8_t value);
    void Or(uint8_t value);
    void Xor(uint8_t value);
    void Cmp(uint8_t value);
    void Jump(uint16_t address);
    void Call(uint16_t address);
    void Ret();
    void Rst(uint8_t vector);
    void Push(uint16_t value);
    void Pop(uint16_t &value);
    void Rlc(uint8_t &value);
    void Rl(uint8_t &value);
};