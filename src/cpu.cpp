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
    int8_t offset;
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
    case 0x10:
        registers->pc++; // just skip the byte its not being used
        isStopped = true;
        return 4; // STOP with a dummy byte??
    case 0x11:
        registers->e = memory->Read(registers->pc++);
        registers->d = memory->Read(registers->pc++);
        return 12; // LD DE, n16
    case 0x12:
        memory->Write(registers->de, registers->a);
        return 8; // LD (DE),A
    case 0x13:
        registers->de++;
        return 8; // INC DE
    case 0x14:
        Inc(registers->d);
        return 4; // INC D
    case 0x15:
        Dec(registers->d);
        return 4; // DEC D
    case 0x16:
        registers->d = memory->Read(registers->pc++);
        return 8; // LD D,n8
    case 0x17:
        Rl(registers->a, false);
        return 4; // RLA
    case 0x18:
        offset = static_cast<int8_t>(memory->Read(registers->pc++));
        registers->pc += offset;
        return 12; // JR e8
    case 0x19:
        AddHL(registers->de);
        return 8; // ADD HL, DE
    case 0x1A:
        registers->a = memory->Read(registers->de);
        return 8; // LD A, (DE)
    case 0x1B:
        registers->de--;
        return 8; // DEC DE
    case 0x1C:
        Inc(registers->e);
        return 4; // INC E
    case 0x1D:
        Dec(registers->e);
        return 4; // DEC E
    case 0x1E:
        registers->e = memory->Read(registers->pc++);
        return 8; // LD E, n8
    case 0x1F:
        Rr(registers->a, false);
        return 4; // RRA
    case 0x20:
        offset = static_cast<int8_t>(memory->Read(registers->pc++));
        if (!registers->IsFlagSet(Flag::Z))
        {
            registers->pc += offset;
            return 12;
        }
        return 8; // JR NZ, e
    case 0x21:
        registers->l = memory->Read(registers->pc++);
        registers->h = memory->Read(registers->pc++);
        return 12; // LD HL, n16
    case 0x22:
        memory->Write(registers->hl, registers->a);
        registers->hl++;
        return 8; // LD (HL+), A
    case 0x23:
        registers->hl++;
        return 8; // INC HL
    case 0x24:
        Inc(registers->h);
        return 4; // INC H
    case 0x25:
        Dec(registers->h);
        return 4; // DEC H
    case 0x26:
        registers->h = memory->Read(registers->pc++);
        return 8; // LD H, n8
    case 0x27:
        if (registers->IsFlagSet(Flag::N))
        {
            if (registers->IsFlagSet(Flag::C) || registers->a > 0x99)
            {
                registers->a -= 0x60;
                registers->WriteFlag(Flag::C, true);
            }
            if (registers->IsFlagSet(Flag::H) || (registers->a & 0x0F) > 0x09)
            {
                registers->a -= 0x06;
            }
        }
        else
        {
            if (registers->IsFlagSet(Flag::C) || registers->a > 0x99)
            {
                registers->a += 0x60;
                registers->WriteFlag(Flag::C, true);
            }
            if (registers->IsFlagSet(Flag::H) || (registers->a & 0x0F) > 0x09)
            {
                registers->a += 0x06;
            }
        }
        return 4; // DAA
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

int CPU::Step()
{
    uint8_t opcode = memory->Read(registers->pc++);
    int instructionCycles = Execute(opcode);
    cycles += instructionCycles;
    return instructionCycles;
}