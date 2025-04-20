#include "registers.h"
#include "memory_map.h"

class CPU
{
public:
    CPU(MemoryMap *memory, Registers *registers);

    MemoryMap *memory;
    Registers *registers;

    void Execute(uint8_t opcode);
    int Step();
};