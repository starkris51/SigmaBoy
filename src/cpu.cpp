#include "cpu.h"

CPU::CPU(MMU *memory, Registers *registers) : memory(memory), registers(registers)
{
    registers->pc = 0x0100;
    registers->sp = 0xFFFE;
}

int CPU::Execute(uint8_t opcode)
{
    uint8_t low, high;
    uint16_t address;
    printf("PC: %04X, Opcode: %02X\n", registers->pc - 1, opcode);

    switch (opcode)
    {
    case 0x00: // NOP
        return 4;
    case 0x01:
        registers->c = memory->Read(registers->pc++);
        registers->b = memory->Read(registers->pc++);
        return 12; // LD BC,d16
    case 0x02:
        memory->Write(registers->bc, registers->a);
        return 8; // LD (BC),A
    case 0x03:
        registers->bc++;
        return 8; // INC BC
    case 0x04:
        Inc(registers->b);
        return 4; // INC B
    case 0x05:
        Dec(registers->b);
        return 4; // DEC B
    case 0x06:
        registers->b = memory->Read(registers->pc++);
        return 8; // LD B,d8
    case 0x07:
        Rlc(registers->a, false);
        return 4; // RLCA
    case 0x08:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;

        memory->Write(address, registers->sp & 0xFF);
        memory->Write(address + 1, (registers->sp >> 8) & 0xFF);
        return 20; // LD (a16),SP
    case 0x09:
        AddHL(registers->bc);
        return 8; // ADD HL,BC
    case 0xA:
        registers->a = memory->Read(registers->bc);
        return 8; // LD A,(BC)
    case 0xB:
        registers->bc--;
        return 8; // DEC BC
    case 0xC:
        Inc(registers->c);
        return 4; // INC C
    case 0xD:
        Dec(registers->c);
        return 4; // DEC C
    case 0xE:
        registers->c = memory->Read(registers->pc++);
        return 8; // LD C,d8
    case 0xF:
        Rrc(registers->a, false);
        return 4; // RRCA
    default:
        printf("Unknown opcode: 0x%02X\n", opcode);
        return 0; // Unknown opcode
    }
    return 0; // Default return value
}

void CPU::Add(uint8_t value)
{
    uint16_t result = registers->a + value;

    registers->WriteFlag(Flag::Z, (result & 0xFF) == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, ((registers->a & 0xF) + (value & 0xF)) > 0xF);
    registers->WriteFlag(Flag::C, result > 0xFF);

    registers->a = result & 0xFF;
}

void CPU::AddHL(uint16_t value)
{
    uint32_t result = registers->hl + value;

    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, ((registers->hl & 0xFFF) + (value & 0xFFF)) > 0xFFF);
    registers->WriteFlag(Flag::C, result > 0xFFFF);

    registers->hl = result & 0xFFFF;
}

void CPU::Inc(uint8_t &value)
{
    value++;
    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, ((value & 0x0F) == 0x0F));
}

void CPU::Dec(uint8_t &value)
{
    value--;
    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, true);
    registers->WriteFlag(Flag::H, (value & 0xF) == 0xF);
}

void CPU::Rl(uint8_t &value, bool isPrefixCB)
{
    bool isLastBit = (value & 0x80) != 0;
    value <<= 1;
    value |= registers->IsFlagSet(Flag::C);

    if (isPrefixCB)
    {
        registers->WriteFlag(Flag::Z, value == 0);
    }

    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, isLastBit);
}

void CPU::Rlc(uint8_t &value, bool isPrefixCB)
{
    bool isLastBit = (value & 0x80) != 0;
    value = (value << 1) | (value >> 7);

    if (isPrefixCB)
    {
        registers->WriteFlag(Flag::Z, value == 0);
    }

    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, isLastBit);
}

void CPU::Rr(uint8_t &value, bool isPrefixCB)
{
    bool isLastBit = (value & 0x1) != 0;
    value >>= 1;
    value |= registers->IsFlagSet(Flag::C);

    if (isPrefixCB)
    {
        registers->WriteFlag(Flag::Z, value == 0);
    }

    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, isLastBit);
}

void CPU::Rrc(uint8_t &value, bool isPrefixCB)
{
    bool isLastBit = (value & 0x1) != 0;
    value = (value >> 1) | (value << 7);

    if (isPrefixCB)
    {
        registers->WriteFlag(Flag::Z, value == 0);
    }

    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, isLastBit);
}

void CPU::Step()
{
    uint8_t opcode = memory->Read(registers->pc++);
    cycles += Execute(opcode);

    double timePerCycle = 1.0 / CPU_CLOCK_SPEED;
    double elapsedTime = cycles * timePerCycle;
    double targetTime = (cycles / CYCLES_PER_FRAME) * timePerCycle;

    if (elapsedTime < targetTime)
    {
        SDL_Delay((targetTime - elapsedTime) * 1000.0);
    }
}