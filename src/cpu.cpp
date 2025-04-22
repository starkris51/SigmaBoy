#include "cpu.h"

CPU::CPU(MMU *memory, Registers *registers) : memory(memory), registers(registers)
{
    registers->pc = 0x0100;
}

void CPU::Execute(uint8_t opcode)
{
    printf("PC: %04X, Opcode: %02X\n", registers->pc - 1, opcode);

    switch (opcode)
    {
    case 0x00:
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
    case 0x06:
        registers->b = memory->Read(registers->pc++);
        break; 
    case 0x07:
        registers->a = (registers->a << 1) | (registers->a >> 7);
        break;
    case 0x3E:
        registers->a = memory->Read(registers->pc++);
        break;
    default:
        printf("Unknown opcode: 0x%02X\n", opcode);
        break;
    }
}

void CPU::Step()
{
    uint8_t opcode = memory->Read(registers->pc++);
    Execute(opcode);
}