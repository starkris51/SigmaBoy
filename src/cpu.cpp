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
    uint8_t correction;
    bool setC;
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
        correction = 0;
        setC = false;

        if (!registers->IsFlagSet(Flag::N))
        {
            if (registers->IsFlagSet(Flag::H) || (registers->a & 0x0F) > 0x09)
                correction |= 0x06;
            if (registers->IsFlagSet(Flag::C) || registers->a > 0x99)
            {
                correction |= 0x60;
                setC = true;
            }
            registers->a += correction;
        }
        else
        {
            if (registers->IsFlagSet(Flag::H))
                correction |= 0x06;
            if (registers->IsFlagSet(Flag::C))
                correction |= 0x60;
            registers->a -= correction;
        }

        registers->WriteFlag(Flag::Z, registers->a == 0);
        registers->WriteFlag(Flag::H, false);
        if (setC)
            registers->WriteFlag(Flag::C, true);

        return 4; // DAA
    case 0x28:
        offset = static_cast<int8_t>(memory->Read(registers->pc++));
        if (registers->IsFlagSet(Flag::Z))
        {
            registers->pc += offset;
            return 12;
        }
        return 8; // JR Z, e
    case 0x29:
        AddHL(registers->hl);
        return 8; // ADD HL, HL
    case 0x2A:
        registers->a = memory->Read(registers->hl);
        registers->hl++;
        return 8; // LD A, (HL+)
    case 0x2B:
        registers->hl--;
        return 8; // DEC HL
    case 0x2C:
        Inc(registers->l);
        return 4; // INC L
    case 0x2D:
        Dec(registers->l);
        return 4; // DEC L
    case 0x2E:
        registers->l = memory->Read(registers->pc++);
        return 8; // LD L, n8
    case 0x2F:
        registers->a = ~registers->a;
        registers->WriteFlag(Flag::N, true);
        registers->WriteFlag(Flag::H, true);
        return 4; // CPL
    case 0x30:
        offset = static_cast<int8_t>(memory->Read(registers->pc++));
        if (!registers->IsFlagSet(Flag::C))
        {
            registers->pc += offset;
            return 12;
        }
        return 8; // JR NC, e
    case 0x31:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);

        registers->sp = (high << 8) | low;
        return 12; // LD SP, n16
    case 0x32:
        memory->Write(registers->hl, registers->a);
        registers->hl--;
        return 8; // LD (HL-), A
    case 0x33:
        registers->sp++;
        return 8; // INC SP
    case 0x34:
        memory->Write(registers->hl, memory->Read(registers->hl) + 1);
        return 12; // INC (HL)
    case 0x35:
        memory->Write(registers->hl, memory->Read(registers->hl) - 1);
        return 12; // DEC (HL)
    case 0x36:
        memory->Write(registers->hl, memory->Read(registers->pc++));
        return 12; // LD (HL), n8
    case 0x37:
        registers->WriteFlag(Flag::N, false);
        registers->WriteFlag(Flag::H, false);
        registers->WriteFlag(Flag::C, true);
        return 4; // SCF
    case 0x38:
        offset = static_cast<int8_t>(memory->Read(registers->pc++));
        if (registers->IsFlagSet(Flag::C))
        {
            registers->pc += offset;
            return 12;
        }
        return 8; // JR C, e
    case 0x39:
        AddHL(registers->sp);
        return 8; // ADD HL, SP
    case 0x3A:
        registers->a = memory->Read(registers->hl);
        registers->hl--;
        return 8; // LD A, (HL-)
    case 0x3B:
        registers->sp--;
        return 8; // DEC SP
    case 0x3C:
        Inc(registers->a);
        return 4; // INC A
    case 0x3D:
        Dec(registers->a);
        return 4; // DEC A
    case 0x3E:
        registers->a = memory->Read(registers->pc++);
        return 8; // LD A, n8
    case 0x3F:
        registers->WriteFlag(Flag::N, false);
        registers->WriteFlag(Flag::H, false);
        registers->WriteFlag(Flag::C, !registers->IsFlagSet(Flag::C));
        return 4; // CCF
    case 0x40:
        // This instruction does nothing lmao
        return 4; // LD B, B
    case 0x41:
        registers->b = registers->c;
        return 4; // LD B, C
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

void CPU::Adc(uint8_t value)
{
    bool carry = registers->IsFlagSet(Flag::C);
    uint16_t result = registers->a + value + carry;

    registers->WriteFlag(Flag::Z, (result & 0xFF) == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, ((registers->a & 0xF) + (value & 0xF) + carry) > 0xF);
    registers->WriteFlag(Flag::C, result > 0xFF);

    registers->a = result & 0xFF;
}

void CPU::Sub(uint8_t value)
{
    uint16_t result = registers->a - value;

    registers->WriteFlag(Flag::Z, (result & 0xFF) == 0);
    registers->WriteFlag(Flag::N, true);
    registers->WriteFlag(Flag::H, (registers->a & 0xF) < (value & 0xF));
    registers->WriteFlag(Flag::C, value > registers->a);

    registers->a = result & 0xFF;
}

void CPU::Sbc(uint8_t value)
{
    bool carry = registers->IsFlagSet(Flag::C);
    uint8_t thatValueWithCarry = value + carry;
    uint16_t result = registers->a - value - carry;

    registers->WriteFlag(Flag::Z, (result & 0xFF) == 0);
    registers->WriteFlag(Flag::N, true);
    registers->WriteFlag(Flag::H, (registers->a & 0xF) < (thatValueWithCarry & 0xF));
    registers->WriteFlag(Flag::C, thatValueWithCarry > registers->a);

    registers->a = result & 0xFF;
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