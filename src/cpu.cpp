#include "cpu.h"

CPU::CPU(MemoryMap *memory, Registers *registers) : memory(memory), registers(registers)
{
    registers->pc = 0x0100;
}

void CPU::Execute(uint8_t opcode)
{
    printf("PC: %04X, Opcode: %02X\n", registers->pc - 1, opcode);

    switch (opcode)
    {
    case 0x00: // NOP
        break;
    case 0x01:
        registers->c = memory->Read(registers->pc++);
        registers->b = memory->Read(registers->pc++);
        break;
    case 0x02:
        memory->Write(registers->bc, registers->a);
        break;
    case 0x03:
        registers->bc++;
        break;
    case 0x04:
        registers->b++;
        break;
    case 0x05:
        registers->b--;
        break;
    case 0x3E: // Load data to that fresh Accumulator
        registers->a = memory->Read(registers->pc++);
        break;
    case 0x06: // Load that data to B register
        registers->b = memory->Read(registers->pc++);
        break;

    default:
        printf("Unknown opcode: 0x%02X\n", opcode);
        break;
    }
}

int CPU::Step()
{
    uint8_t opcode = memory->Read(registers->pc++);
    Execute(opcode);

    return 4;
}