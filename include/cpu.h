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
};