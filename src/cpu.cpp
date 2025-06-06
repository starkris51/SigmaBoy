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
        registers->b = registers->b;
        return 4; // LD B, B
    case 0x41:
        registers->b = registers->c;
        return 4; // LD B, C
    case 0x42:
        registers->b = registers->d;
        return 4; // LD B, D
    case 0x43:
        registers->b = registers->e;
        return 4; // LD B, E
    case 0x44:
        registers->b = registers->h;
        return 4; // LD B, H
    case 0x45:
        registers->b = registers->l;
        return 4; // LD B, L
    case 0x46:
        registers->b = memory->Read(registers->hl);
        return 8; // LD B, (HL)
    case 0x47:
        registers->b = registers->a;
        return 4; // LD B, A
    case 0x48:
        registers->c = registers->b;
        return 4; // LD C, B
    case 0x49:
        registers->c = registers->c;
        return 4; // LD C, C
    case 0x4A:
        registers->c = registers->d;
        return 4; // LD C, D
    case 0x4B:
        registers->c = registers->e;
        return 4; // LD C, E
    case 0x4C:
        registers->c = registers->h;
        return 4; // LD C, H
    case 0x4D:
        registers->c = registers->l;
        return 4; // LD C, L
    case 0x4E:
        registers->c = memory->Read(registers->hl);
        return 8; // LD C, (HL)
    case 0x4F:
        registers->c = registers->a;
        return 4; // LD C, A
    case 0x50:
        registers->d = registers->b;
        return 4; // LD D, B
    case 0x51:
        registers->d = registers->c;
        return 4; // LD D, C
    case 0x52:
        registers->d = registers->d;
        return 4; // LD D, D
    case 0x53:
        registers->d = registers->e;
        return 4; // LD D, E
    case 0x54:
        registers->d = registers->h;
        return 4; // LD D, H
    case 0x55:
        registers->d = registers->l;
        return 4; // LD D, L
    case 0x56:
        registers->d = memory->Read(registers->hl);
        return 8; // LD D, (HL)
    case 0x57:
        registers->d = registers->a;
        return 4; // LD D, A
    case 0x58:
        registers->e = registers->b;
        return 4; // LD E, B
    case 0x59:
        registers->e = registers->c;
        return 4; // LD E, C
    case 0x5A:
        registers->e = registers->d;
        return 4; // LD E, D
    case 0x5B:
        registers->e = registers->e;
        return 4; // LD E, E
    case 0x5C:
        registers->e = registers->h;
        return 4; // LD E, H
    case 0x5D:
        registers->e = registers->l;
        return 4; // LD E, L
    case 0x5E:
        registers->e = memory->Read(registers->hl);
        return 8; // LD E, (HL)
    case 0x5F:
        registers->e = registers->a;
        return 4; // LD E, A
    case 0x60:
        registers->h = registers->b;
        return 4; // LD H, B
    case 0x61:
        registers->h = registers->c;
        return 4; // LD H, C
    case 0x62:
        registers->h = registers->d;
        return 4; // LD H, D
    case 0x63:
        registers->h = registers->e;
        return 4; // LD H, E
    case 0x64:
        registers->h = registers->h;
        return 4; // LD H, H
    case 0x65:
        registers->h = registers->l;
        return 4; // LD H, L
    case 0x66:
        registers->h = memory->Read(registers->hl);
        return 8; // LD H, (HL)
    case 0x67:
        registers->h = registers->a;
        return 4; // LD H, A
    case 0x68:
        registers->l = registers->b;
        return 4; // LD L, B
    case 0x69:
        registers->l = registers->c;
        return 4; // LD L, C
    case 0x6A:
        registers->l = registers->d;
        return 4; // LD L, D
    case 0x6B:
        registers->l = registers->e;
        return 4; // LD L, E
    case 0x6C:
        registers->l = registers->h;
        return 4; // LD L, H
    case 0x6D:
        registers->l = registers->l;
        return 4; // LD L, L
    case 0x6E:
        registers->l = memory->Read(registers->hl);
        return 8; // LD L, (HL)
    case 0x6F:
        registers->l = registers->a;
        return 4; // LD L, A
    case 0x70:
        memory->Write(registers->hl, registers->b);
        return 8; // LD (HL), B
    case 0x71:
        memory->Write(registers->hl, registers->c);
        return 8; // LD (HL), C
    case 0x72:
        memory->Write(registers->hl, registers->d);
        return 8; // LD (HL), D
    case 0x73:
        memory->Write(registers->hl, registers->e);
        return 8; // LD (HL), E
    case 0x74:
        memory->Write(registers->hl, registers->h);
        return 8; // LD (HL), H
    case 0x75:
        memory->Write(registers->hl, registers->l);
        return 8; // LD (HL), L
    case 0x76:
        isHalted = true;
        return 4; // HALT
    case 0x77:
        memory->Write(registers->hl, registers->a);
        return 8; // LD (HL), A
    case 0x78:
        registers->a = registers->b;
        return 4; // LD A, B
    case 0x79:
        registers->a = registers->c;
        return 4; // LD A, C
    case 0x7A:
        registers->a = registers->d;
        return 4; // LD A, D
    case 0x7B:
        registers->a = registers->e;
        return 4; // LD A, E
    case 0x7C:
        registers->a = registers->h;
        return 4; // LD A, H
    case 0x7D:
        registers->a = registers->l;
        return 4; // LD A, L
    case 0x7E:
        registers->a = memory->Read(registers->hl);
        return 8; // LD A, (HL)
    case 0x7F:
        registers->a = registers->a;
        return 4; // LD A, A
    case 0x80:
        Add(registers->b);
        return 4; // ADD A, B
    case 0x81:
        Add(registers->c);
        return 4; // ADD A, C
    case 0x82:
        Add(registers->d);
        return 4; // ADD A, D
    case 0x83:
        Add(registers->e);
        return 4; // ADD A, E
    case 0x84:
        Add(registers->h);
        return 4; // ADD A, H
    case 0x85:
        Add(registers->l);
        return 4; // ADD A, L
    case 0x86:
        Add(memory->Read(registers->hl));
        return 8; // ADD A, (HL)
    case 0x87:
        Add(registers->a);
        return 4; // ADD A, A
    case 0x88:
        Adc(registers->b);
        return 4; // ADC A, B
    case 0x89:
        Adc(registers->c);
        return 4; // ADC A, C
    case 0x8A:
        Adc(registers->d);
        return 4; // ADC A, D
    case 0x8B:
        Adc(registers->e);
        return 4; // ADC A, E
    case 0x8C:
        Adc(registers->h);
        return 4; // ADC A, H
    case 0x8D:
        Adc(registers->l);
        return 4; // ADC A, L
    case 0x8E:
        Adc(memory->Read(registers->hl));
        return 8; // ADC A, (HL)
    case 0x8F:
        Adc(registers->a);
        return 4; // ADC A, A
    case 0x90:
        Sub(registers->b);
        return 4; // SUB B
    case 0x91:
        Sub(registers->c);
        return 4; // SUB C
    case 0x92:
        Sub(registers->d);
        return 4; // SUB D
    case 0x93:
        Sub(registers->e);
        return 4; // SUB E
    case 0x94:
        Sub(registers->h);
        return 4; // SUB H
    case 0x95:
        Sub(registers->l);
        return 4; // SUB L
    case 0x96:
        Sub(memory->Read(registers->hl));
        return 8; // SUB (HL)
    case 0x97:
        Sub(registers->a);
        return 4; // SUB A
    case 0x98:
        Sbc(registers->b);
        return 4; // SBC A, B
    case 0x99:
        Sbc(registers->c);
        return 4; // SBC A, C
    case 0x9A:
        Sbc(registers->d);
        return 4; // SBC A, D
    case 0x9B:
        Sbc(registers->e);
        return 4; // SBC A, E
    case 0x9C:
        Sbc(registers->h);
        return 4; // SBC A, H
    case 0x9D:
        Sbc(registers->l);
        return 4; // SBC A, L
    case 0x9E:
        Sbc(memory->Read(registers->hl));
        return 8; // SBC A, (HL)
    case 0x9F:
        Sbc(registers->a);
        return 4; // SBC A, A
    case 0xA0:
        And(registers->b);
        return 4; // AND B
    case 0xA1:
        And(registers->c);
        return 4; // AND C
    case 0xA2:
        And(registers->d);
        return 4; // AND D
    case 0xA3:
        And(registers->e);
        return 4; // AND E
    case 0xA4:
        And(registers->h);
        return 4; // AND H
    case 0xA5:
        And(registers->l);
        return 4; // AND L
    case 0xA6:
        And(memory->Read(registers->hl));
        return 8; // AND (HL)
    case 0xA7:
        And(registers->a);
        return 4; // AND A
    case 0xA8:
        Xor(registers->b);
        return 4; // XOR B
    case 0xA9:
        Xor(registers->c);
        return 4; // XOR C
    case 0xAA:
        Xor(registers->d);
        return 4; // XOR D
    case 0xAB:
        Xor(registers->e);
        return 4; // XOR E
    case 0xAC:
        Xor(registers->h);
        return 4; // XOR H
    case 0xAD:
        Xor(registers->l);
        return 4; // XOR L
    case 0xAE:
        Xor(memory->Read(registers->hl));
        return 8; // XOR (HL)
    case 0xAF:
        Xor(registers->a);
        return 4; // XOR A
    case 0xB0:
        Or(registers->b);
        return 4; // OR B
    case 0xB1:
        Or(registers->c);
        return 4; // OR C
    case 0xB2:
        Or(registers->d);
        return 4; // OR D
    case 0xB3:
        Or(registers->e);
        return 4; // OR E
    case 0xB4:
        Or(registers->h);
        return 4; // OR H
    case 0xB5:
        Or(registers->l);
        return 4; // OR L
    case 0xB6:
        Or(memory->Read(registers->hl));
        return 8; // OR (HL)
    case 0xB7:
        Or(registers->a);
        return 4; // OR A
    case 0xB8:
        Cp(registers->b);
        return 4; // CP B
    case 0xB9:
        Cp(registers->c);
        return 4; // CP C
    case 0xBA:
        Cp(registers->d);
        return 4; // CP D
    case 0xBB:
        Cp(registers->e);
        return 4; // CP E
    case 0xBC:
        Cp(registers->h);
        return 4; // CP H
    case 0xBD:
        Cp(registers->l);
        return 4; // CP L
    case 0xBE:
        Cp(memory->Read(registers->hl));
        return 8; // CP (HL)
    case 0xBF:
        Cp(registers->a);
        return 4; // CP A
    case 0xC0:
        if (!registers->IsFlagSet(Flag::Z))
        {
            registers->pc = memory->Read(registers->sp++);
            registers->pc |= memory->Read(registers->sp++) << 8;
            return 20;
        }
        return 8; // RET NZ
    case 0xC1:
        registers->c = memory->Read(registers->sp++);
        registers->b = memory->Read(registers->sp++);
        return 12; // POP BC
    case 0xC2:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (!registers->IsFlagSet(Flag::Z))
        {
            registers->pc = address;
            return 16;
        }
        return 12; // JP NZ, a16
    case 0xC3:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;

        registers->pc = address;
        return 16; // JP a16
    case 0xC4:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (!registers->IsFlagSet(Flag::Z))
        {
            memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
            memory->Write(registers->sp--, registers->pc & 0xFF);
            registers->pc = address;
            return 24;
        }
        return 12; // CALL NZ, a16
    case 0xC5:
        memory->Write(registers->sp--, registers->b);
        memory->Write(registers->sp--, registers->c);
        return 16; // PUSH BC
    case 0xC6:
        Add(memory->Read(registers->pc++));
        return 8; // ADD A, d8
    case 0xC7:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);

        registers->pc = 0x0000;
        return 16; // RST 00H
    case 0xC8:
        if (registers->IsFlagSet(Flag::Z))
        {
            registers->pc = memory->Read(registers->sp++);
            registers->pc |= memory->Read(registers->sp++) << 8;
            return 20;
        }
        return 8; // RET Z
    case 0xC9:
        registers->pc = memory->Read(registers->sp++);
        registers->pc |= memory->Read(registers->sp++) << 8;
        return 16; // RET
    case 0xCA:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (registers->IsFlagSet(Flag::Z))
        {
            registers->pc = address;
            return 16;
        }
        return 12; // JP Z, a16
    case 0xCB:
        opcode = memory->Read(registers->pc++);
        return ExecuteCB(opcode); // PREFIX CB
    case 0xCC:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (registers->IsFlagSet(Flag::Z))
        {
            memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
            memory->Write(registers->sp--, registers->pc & 0xFF);
            registers->pc = address;
            return 24;
        }
        return 12; // CALL Z, a16
    case 0xCD:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = address;
        return 24; // CALL a16
    case 0xCE:
        Adc(memory->Read(registers->pc++));
        return 8; // ADC A, d8
    case 0xCF:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = 0x0008;
        return 16; // RST 08H
    case 0xD0:
        if (!registers->IsFlagSet(Flag::C))
        {
            registers->pc = memory->Read(registers->sp++);
            registers->pc |= memory->Read(registers->sp++) << 8;
            return 20;
        }
        return 8; // RET NC
    case 0xD1:
        registers->e = memory->Read(registers->sp++);
        registers->d = memory->Read(registers->sp++);
        return 12; // POP DE
    case 0xD2:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (!registers->IsFlagSet(Flag::C))
        {
            registers->pc = address;
            return 16;
        }
        return 12; // JP NC, a16
    case 0xD4:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (!registers->IsFlagSet(Flag::C))
        {
            memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
            memory->Write(registers->sp--, registers->pc & 0xFF);
            registers->pc = address;
            return 24;
        }
        return 12; // CALL NC, a16
    case 0xD5:
        memory->Write(registers->sp--, registers->d);
        memory->Write(registers->sp--, registers->e);
        return 16; // PUSH DE
    case 0xD6:
        Sub(memory->Read(registers->pc++));
        return 8; // SUB d8
    case 0xD7:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = 0x0010;
        return 16; // RST 10H
    case 0xD8:
        if (registers->IsFlagSet(Flag::C))
        {
            registers->pc = memory->Read(registers->sp++);
            registers->pc |= memory->Read(registers->sp++) << 8;
            return 20;
        }
        return 8; // RET C
    case 0xD9:
        registers->pc = memory->Read(registers->sp++);
        registers->pc |= memory->Read(registers->sp++) << 8;
        enableInterruptsNextInstruction = true;
        return 16; // RETI
    case 0xDA:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (registers->IsFlagSet(Flag::C))
        {
            registers->pc = address;
            return 16;
        }
        return 12; // JP C, a16
    case 0xDC:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        if (registers->IsFlagSet(Flag::C))
        {
            memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
            memory->Write(registers->sp--, registers->pc & 0xFF);
            registers->pc = address;
            return 24;
        }
        return 12; // CALL C, a16
    case 0xDE:
        Sbc(memory->Read(registers->pc++));
        return 8; // SBC A, d8
    case 0xDF:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = 0x0018;
        return 16; // RST 18H
    case 0xE0:
        address = 0xFF00 + memory->Read(registers->pc++);
        memory->Write(address, registers->a);
        return 12; // LDH (a8),A
    case 0xE1:
        registers->l = memory->Read(registers->sp++);
        registers->h = memory->Read(registers->sp++);
        return 12; // POP HL
    case 0xE2:
        memory->Write(0xFF00 + registers->c, registers->a);
        return 8; // LD (C),A
    case 0xE5:
        memory->Write(registers->sp--, registers->h);
        memory->Write(registers->sp--, registers->l);
        return 16; // PUSH HL
    case 0xE6:
        And(memory->Read(registers->pc++));
        return 8; // AND d8
    case 0xE7:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = 0x0020;
        return 16; // RST 20H
    case 0xE8:
        offset = static_cast<int8_t>(memory->Read(registers->pc++));
        registers->sp += offset;
        registers->WriteFlag(Flag::Z, false);
        registers->WriteFlag(Flag::N, false);
        registers->WriteFlag(Flag::H, ((registers->sp & 0xFFF) + (offset & 0xFFF)) > 0xFFF);
        registers->WriteFlag(Flag::C, (registers->sp & 0xFF00) != (registers->hl & 0xFF00));

        return 16; // ADD SP, r8
    case 0xE9:
        registers->pc = registers->hl;
        return 4; // JP (HL)
    case 0xEA:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        memory->Write(address, registers->a);
        return 16; // LD (a16),A
    case 0xEE:
        Xor(memory->Read(registers->pc++));
        return 8; // XOR d8
    case 0xEF:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = 0x0028;
        return 16; // RST 28H
    case 0xF0:
        address = 0xFF00 + memory->Read(registers->pc++);
        registers->a = memory->Read(address);
        return 12; // LDH A,(a8)
    case 0xF1:
        registers->f = memory->Read(registers->sp++);
        registers->a = memory->Read(registers->sp++);
        return 12; // POP AF
    case 0xF2:
        registers->a = memory->Read(0xFF00 + registers->c);
        return 8; // LD A,(C)
    case 0xF3:
        enableInterruptsNextInstruction = false;
        return 4; // DI
    case 0xF5:
        memory->Write(registers->sp--, registers->a);
        memory->Write(registers->sp--, registers->f);
        return 16; // PUSH AF
    case 0xF6:
        Or(memory->Read(registers->pc++));
        return 8; // OR d8
    case 0xF7:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = 0x0030;
        return 16; // RST 30H
    case 0xF8:
        offset = static_cast<int8_t>(memory->Read(registers->pc++));
        registers->hl = registers->sp + offset;
        registers->WriteFlag(Flag::N, false);
        registers->WriteFlag(Flag::H, ((registers->sp & 0xFFF) + (offset & 0xFFF)) > 0xFFF);
        registers->WriteFlag(Flag::C, (registers->sp & 0xFF00) != (registers->hl & 0xFF00));
        return 12; // LD HL,SP+r8
    case 0xF9:
        registers->sp = registers->hl;
        return 8; // LD SP,HL
    case 0xFA:
        low = memory->Read(registers->pc++);
        high = memory->Read(registers->pc++);
        address = (high << 8) | low;
        registers->a = memory->Read(address);
        return 16; // LD A,(a16)
    case 0xFB:
        enableInterruptsNextInstruction = true;
        return 4; // EI
    case 0xFE:
        Cp(memory->Read(registers->pc++));
        return 8; // CP d8
    case 0xFF:
        memory->Write(registers->sp--, (registers->pc >> 8) & 0xFF);
        memory->Write(registers->sp--, registers->pc & 0xFF);
        registers->pc = 0x0038;
        return 16; // RST 38H
    case 0xD3:
    case 0xDB:
    case 0xDD:
    case 0xE3:
    case 0xE4:
    case 0xEB:
    case 0xEC:
    case 0xED:
    case 0xF4:
    case 0xFC:
    case 0xFD:
        return 0;
    default:
        printf("Unknown opcode: 0x%02X\n", opcode);
        return 0; // Unknown opcode
    }
    return 0; // Default return value
}

int CPU::ExecuteCB(uint8_t opcode)
{
    uint8_t value;

    switch (opcode)
    {
    case 0x00:
        Rlc(registers->b, true);
        return 8; // RLC B
    case 0x01:
        Rlc(registers->c, true);
        return 8; // RLC C
    case 0x02:
        Rlc(registers->d, true);
        return 8; // RLC D
    case 0x03:
        Rlc(registers->e, true);
        return 8; // RLC E
    case 0x04:
        Rlc(registers->h, true);
        return 8; // RLC H
    case 0x05:
        Rlc(registers->l, true);
        return 8; // RLC L
    case 0x06:
        value = memory->Read(registers->hl);
        Rlc(value, true);
        memory->Write(registers->hl, value);
        return 16; // RLC (HL)
    case 0x07:
        Rlc(registers->a, true);
        return 8; // RLC A
    case 0x08:
        Rrc(registers->b, true);
        return 8; // RRC B
    case 0x09:
        Rrc(registers->c, true);
        return 8; // RRC C
    case 0x0A:
        Rrc(registers->d, true);
        return 8; // RRC D
    case 0x0B:
        Rrc(registers->e, true);
        return 8; // RRC E
    case 0x0C:
        Rrc(registers->h, true);
        return 8; // RRC H
    case 0x0D:
        Rrc(registers->l, true);
        return 8; // RRC L
    case 0x0E:
        value = memory->Read(registers->hl);
        Rrc(value, true);
        memory->Write(registers->hl, value);
        return 16; // RRC (HL)
    case 0x0F:
        Rrc(registers->a, true);
        return 8; // RRC A
    case 0x10:
        Rl(registers->b, true);
        return 8; // RL B
    case 0x11:
        Rl(registers->c, true);
        return 8; // RL C
    case 0x12:
        Rl(registers->d, true);
        return 8; // RL D
    case 0x13:
        Rl(registers->e, true);
        return 8; // RL E
    case 0x14:
        Rl(registers->h, true);
        return 8; // RL H
    case 0x15:
        Rl(registers->l, true);
        return 8; // RL L
    case 0x16:
        value = memory->Read(registers->hl);
        Rl(value, true);
        memory->Write(registers->hl, value);
        return 16; // RL (HL)
    case 0x17:
        Rl(registers->a, true);
        return 8; // RL A
    case 0x18:
        Rr(registers->b, true);
        return 8; // RR B
    case 0x19:
        Rr(registers->c, true);
        return 8; // RR C
    case 0x1A:
        Rr(registers->d, true);
        return 8; // RR D
    case 0x1B:
        Rr(registers->e, true);
        return 8; // RR E
    case 0x1C:
        Rr(registers->h, true);
        return 8; // RR H
    case 0x1D:
        Rr(registers->l, true);
        return 8; // RR L
    case 0x1E:
        value = memory->Read(registers->hl);
        Rr(value, true);
        memory->Write(registers->hl, value);
        return 16; // RR (HL)
    case 0x1F:
        Rr(registers->a, true);
        return 8; // RR A
    case 0x20:
        Sla(registers->b);
        return 8; // SLA B
    case 0x21:
        Sla(registers->c);
        return 8; // SLA C
    case 0x22:
        Sla(registers->d);
        return 8; // SLA D
    case 0x23:
        Sla(registers->e);
        return 8; // SLA E
    case 0x24:
        Sla(registers->h);
        return 8; // SLA H
    case 0x25:
        Sla(registers->l);
        return 8; // SLA L
    case 0x26:
        value = memory->Read(registers->hl);
        Sla(value);
        memory->Write(registers->hl, value);
        return 16; // SLA (HL)
    case 0x27:
        Sla(registers->a);
        return 8; // SLA A
    case 0x28:
        Sra(registers->b);
        return 8; // SRA B
    case 0x29:
        Sra(registers->c);
        return 8; // SRA C
    case 0x2A:
        Sra(registers->d);
        return 8; // SRA D
    case 0x2B:
        Sra(registers->e);
        return 8; // SRA E
    case 0x2C:
        Sra(registers->h);
        return 8; // SRA H
    case 0x2D:
        Sra(registers->l);
        return 8; // SRA L
    case 0x2E:
        value = memory->Read(registers->hl);
        Sra(value);
        memory->Write(registers->hl, value);
        return 16; // SRA (HL)
    case 0x2F:
        Sra(registers->a);
        return 8; // SRA A
    case 0x30:
        Swap(registers->b);
        return 8; // SWAP B
    case 0x31:
        Swap(registers->c);
        return 8; // SWAP C
    case 0x32:
        Swap(registers->d);
        return 8; // SWAP D
    case 0x33:
        Swap(registers->e);
        return 8; // SWAP E
    case 0x34:
        Swap(registers->h);
        return 8; // SWAP H
    case 0x35:
        Swap(registers->l);
        return 8; // SWAP L
    case 0x36:
        value = memory->Read(registers->hl);
        Swap(value);
        memory->Write(registers->hl, value);
        return 16; // SWAP (HL)
    case 0x37:
        Swap(registers->a);
        return 8; // SWAP A
    case 0x38:
        Srl(registers->b);
        return 8; // SRL B
    case 0x39:
        Srl(registers->c);
        return 8; // SRL C
    case 0x3A:
        Srl(registers->d);
        return 8; // SRL D
    case 0x3B:
        Srl(registers->e);
        return 8; // SRL E
    case 0x3C:
        Srl(registers->h);
        return 8; // SRL H
    case 0x3D:
        Srl(registers->l);
        return 8; // SRL L
    case 0x3E:
        value = memory->Read(registers->hl);
        Srl(value);
        memory->Write(registers->hl, value);
        return 16; // SRL (HL)
    case 0x3F:
        Srl(registers->a);
        return 8; // SRL A
    case 0x40:
        Bit(registers->b, 0x01);
        return 8; // BIT 0,B
    case 0x41:
        Bit(registers->c, 0x01);
        return 8; // BIT 0,C
    case 0x42:
        Bit(registers->d, 0x01);
        return 8; // BIT 0,D
    case 0x43:
        Bit(registers->e, 0x01);
        return 8; // BIT 0,E
    case 0x44:
        Bit(registers->h, 0x01);
        return 8; // BIT 0,H
    case 0x45:
        Bit(registers->l, 0x01);
        return 8; // BIT 0,L
    case 0x46:
        value = memory->Read(registers->hl);
        Bit(value, 0x01);
        return 12; // BIT 0,(HL)
    case 0x47:
        Bit(registers->a, 0x01);
        return 8; // BIT 0,A
    case 0x48:
        Bit(registers->b, 0x02);
        return 8; // BIT 1,B
    case 0x49:
        Bit(registers->c, 0x02);
        return 8; // BIT 1,C
    case 0x4A:
        Bit(registers->d, 0x02);
        return 8; // BIT 1,D
    case 0x4B:
        Bit(registers->e, 0x02);
        return 8; // BIT 1,E
    case 0x4C:
        Bit(registers->h, 0x02);
        return 8; // BIT 1,H
    case 0x4D:
        Bit(registers->l, 0x02);
        return 8; // BIT 1,L
    case 0x4E:
        value = memory->Read(registers->hl);
        Bit(value, 0x02);
        return 12; // BIT 1,(HL)
    case 0x4F:
        Bit(registers->a, 0x02);
        return 8; // BIT 1,A
    case 0x50:
        Bit(registers->b, 0x04);
        return 8; // BIT 2,B
    case 0x51:
        Bit(registers->c, 0x04);
        return 8; // BIT 2,C
    case 0x52:
        Bit(registers->d, 0x04);
        return 8; // BIT 2,D
    case 0x53:
        Bit(registers->e, 0x04);
        return 8; // BIT 2,E
    case 0x54:
        Bit(registers->h, 0x04);
        return 8; // BIT 2,H
    case 0x55:
        Bit(registers->l, 0x04);
        return 8; // BIT 2,L
    case 0x56:
        value = memory->Read(registers->hl);
        Bit(value, 0x04);
        return 12; // BIT 2,(HL)
    case 0x57:
        Bit(registers->a, 0x04);
        return 8; // BIT 2,A
    case 0x58:
        Bit(registers->b, 0x08);
        return 8; // BIT 3,B
    case 0x59:
        Bit(registers->c, 0x08);
        return 8; // BIT 3,C
    case 0x5A:
        Bit(registers->d, 0x08);
        return 8; // BIT 3,D
    case 0x5B:
        Bit(registers->e, 0x08);
        return 8; // BIT 3,E
    case 0x5C:
        Bit(registers->h, 0x08);
        return 8; // BIT 3,H
    case 0x5D:
        Bit(registers->l, 0x08);
        return 8; // BIT 3,L
    case 0x5E:
        value = memory->Read(registers->hl);
        Bit(value, 0x08);
        return 12; // BIT 3,(HL)
    case 0x5F:
        Bit(registers->a, 0x08);
        return 8; // BIT 3,A
    case 0x60:
        Bit(registers->b, 0x10);
        return 8; // BIT 4,B
    case 0x61:
        Bit(registers->c, 0x10);
        return 8; // BIT 4,C
    case 0x62:
        Bit(registers->d, 0x10);
        return 8; // BIT 4,D
    case 0x63:
        Bit(registers->e, 0x10);
        return 8; // BIT 4,E
    case 0x64:
        Bit(registers->h, 0x10);
        return 8; // BIT 4,H
    case 0x65:
        Bit(registers->l, 0x10);
        return 8; // BIT 4,L
    case 0x66:
        value = memory->Read(registers->hl);
        Bit(value, 0x10);
        return 12; // BIT 4,(HL)
    case 0x67:
        Bit(registers->a, 0x10);
        return 8; // BIT 4,A
    case 0x68:
        Bit(registers->b, 0x20);
        return 8; // BIT 5,B
    case 0x69:
        Bit(registers->c, 0x20);
        return 8; // BIT 5,C
    case 0x6A:
        Bit(registers->d, 0x20);
        return 8; // BIT 5,D
    case 0x6B:
        Bit(registers->e, 0x20);
        return 8; // BIT 5,E
    case 0x6C:
        Bit(registers->h, 0x20);
        return 8; // BIT 5,H
    case 0x6D:
        Bit(registers->l, 0x20);
        return 8; // BIT 5,L
    case 0x6E:
        value = memory->Read(registers->hl);
        Bit(value, 0x20);
        return 12; // BIT 5,(HL)
    case 0x6F:
        Bit(registers->a, 0x20);
        return 8; // BIT 5,A
    case 0x70:
        Bit(registers->b, 0x40);
        return 8; // BIT 6,B
    case 0x71:
        Bit(registers->c, 0x40);
        return 8; // BIT 6,C
    case 0x72:
        Bit(registers->d, 0x40);
        return 8; // BIT 6,D
    case 0x73:
        Bit(registers->e, 0x40);
        return 8; // BIT 6,E
    case 0x74:
        Bit(registers->h, 0x40);
        return 8; // BIT 6,H
    case 0x75:
        Bit(registers->l, 0x40);
        return 8; // BIT 6,L
    case 0x76:
        value = memory->Read(registers->hl);
        Bit(value, 0x40);
        return 12; // BIT 6,(HL)
    case 0x77:
        Bit(registers->a, 0x40);
        return 8; // BIT 6,A
    case 0x78:
        Bit(registers->b, 0x80);
        return 8; // BIT 7,B
    case 0x79:
        Bit(registers->c, 0x80);
        return 8; // BIT 7,C
    case 0x7A:
        Bit(registers->d, 0x80);
        return 8; // BIT 7,D
    case 0x7B:
        Bit(registers->e, 0x80);
        return 8; // BIT 7,E
    case 0x7C:
        Bit(registers->h, 0x80);
        return 8; // BIT 7,H
    case 0x7D:
        Bit(registers->l, 0x80);
        return 8; // BIT 7,L
    case 0x7E:
        value = memory->Read(registers->hl);
        Bit(value, 0x80);
        return 12; // BIT 7,(HL)
    case 0x7F:
        Bit(registers->a, 0x80);
        return 8; // BIT 7,A
    case 0x80:
        Res(registers->b, 0x01);
        return 8; // RES 0,B
    case 0x81:
        Res(registers->c, 0x01);
        return 8; // RES 0,C
    case 0x82:
        Res(registers->d, 0x01);
        return 8; // RES 0,D
    case 0x83:
        Res(registers->e, 0x01);
        return 8; // RES 0,E
    case 0x84:
        Res(registers->h, 0x01);
        return 8; // RES 0,H
    case 0x85:
        Res(registers->l, 0x01);
        return 8; // RES 0,L
    case 0x86:
        value = memory->Read(registers->hl);
        Res(value, 0x01);
        memory->Write(registers->hl, value);
        return 16; // RES 0,(HL)
    case 0x87:
        Res(registers->a, 0x01);
        return 8; // RES 0,A
    case 0x88:
        Res(registers->b, 0x02);
        return 8; // RES 1,B
    case 0x89:
        Res(registers->c, 0x02);
        return 8; // RES 1,C
    case 0x8A:
        Res(registers->d, 0x02);
        return 8; // RES 1,D
    case 0x8B:
        Res(registers->e, 0x02);
        return 8; // RES 1,E
    case 0x8C:
        Res(registers->h, 0x02);
        return 8; // RES 1,H
    case 0x8D:
        Res(registers->l, 0x02);
        return 8; // RES 1,L
    case 0x8E:
        value = memory->Read(registers->hl);
        Res(value, 0x02);
        memory->Write(registers->hl, value);
        return 16; // RES 1,(HL)
    case 0x8F:
        Res(registers->a, 0x02);
        return 8; // RES 1,A
    case 0x90:
        Res(registers->b, 0x04);
        return 8; // RES 2,B
    case 0x91:
        Res(registers->c, 0x04);
        return 8; // RES 2,C
    case 0x92:
        Res(registers->d, 0x04);
        return 8; // RES 2,D
    case 0x93:
        Res(registers->e, 0x04);
        return 8; // RES 2,E
    case 0x94:
        Res(registers->h, 0x04);
        return 8; // RES 2,H
    case 0x95:
        Res(registers->l, 0x04);
        return 8; // RES 2,L
    case 0x96:
        value = memory->Read(registers->hl);
        Res(value, 0x04);
        memory->Write(registers->hl, value);
        return 16; // RES 2,(HL)
    case 0x97:
        Res(registers->a, 0x04);
        return 8; // RES 2,A
    case 0x98:
        Res(registers->b, 0x08);
        return 8; // RES 3,B
    case 0x99:
        Res(registers->c, 0x08);
        return 8; // RES 3,C
    case 0x9A:
        Res(registers->d, 0x08);
        return 8; // RES 3,D
    case 0x9B:
        Res(registers->e, 0x08);
        return 8; // RES 3,E
    case 0x9C:
        Res(registers->h, 0x08);
        return 8; // RES 3,H
    case 0x9D:
        Res(registers->l, 0x08);
        return 8; // RES 3,L
    case 0x9E:
        value = memory->Read(registers->hl);
        Res(value, 0x08);
        memory->Write(registers->hl, value);
        return 16; // RES 3,(HL)
    case 0x9F:
        Res(registers->a, 0x08);
        return 8; // RES 3,A
    case 0xA0:
        Res(registers->b, 0x10);
        return 8; // RES 4,B
    case 0xA1:
        Res(registers->c, 0x10);
        return 8; // RES 4,C
    case 0xA2:
        Res(registers->d, 0x10);
        return 8; // RES 4,D
    case 0xA3:
        Res(registers->e, 0x10);
        return 8; // RES 4,E
    case 0xA4:
        Res(registers->h, 0x10);
        return 8; // RES 4,H
    case 0xA5:
        Res(registers->l, 0x10);
        return 8; // RES 4,L
    case 0xA6:
        value = memory->Read(registers->hl);
        Res(value, 0x10);
        memory->Write(registers->hl, value);
        return 16; // RES 4,(HL)
    case 0xA7:
        Res(registers->a, 0x10);
        return 8; // RES 4,A
    case 0xA8:
        Res(registers->b, 0x20);
        return 8; // RES 5,B
    case 0xA9:
        Res(registers->c, 0x20);
        return 8; // RES 5,C
    case 0xAA:
        Res(registers->d, 0x20);
        return 8; // RES 5,D
    case 0xAB:
        Res(registers->e, 0x20);
        return 8; // RES 5,E
    case 0xAC:
        Res(registers->h, 0x20);
        return 8; // RES 5,H
    case 0xAD:
        Res(registers->l, 0x20);
        return 8; // RES 5,L
    case 0xAE:
        value = memory->Read(registers->hl);
        Res(value, 0x20);
        memory->Write(registers->hl, value);
        return 16; // RES 5,(HL)
    case 0xAF:
        Res(registers->a, 0x20);
        return 8; // RES 5,A
    case 0xB0:
        Res(registers->b, 0x40);
        return 8; // RES 6,B
    case 0xB1:
        Res(registers->c, 0x40);
        return 8; // RES 6,C
    case 0xB2:
        Res(registers->d, 0x40);
        return 8; // RES 6,D
    case 0xB3:
        Res(registers->e, 0x40);
        return 8; // RES 6,E
    case 0xB4:
        Res(registers->h, 0x40);
        return 8; // RES 6,H
    case 0xB5:
        Res(registers->l, 0x40);
        return 8; // RES 6,L
    case 0xB6:
        value = memory->Read(registers->hl);
        Res(value, 0x40);
        memory->Write(registers->hl, value);
        return 16; // RES 6,(HL)
    case 0xB7:
        Res(registers->a, 0x40);
        return 8; // RES 6,A
    case 0xB8:
        Res(registers->b, 0x80);
        return 8; // RES 7,B
    case 0xB9:
        Res(registers->c, 0x80);
        return 8; // RES 7,C
    case 0xBA:
        Res(registers->d, 0x80);
        return 8; // RES 7,D
    case 0xBB:
        Res(registers->e, 0x80);
        return 8; // RES 7,E
    case 0xBC:
        Res(registers->h, 0x80);
        return 8; // RES 7,H
    case 0xBD:
        Res(registers->l, 0x80);
        return 8; // RES 7,L
    case 0xBE:
        value = memory->Read(registers->hl);
        Res(value, 0x80);
        memory->Write(registers->hl, value);
        return 16; // RES 7,(HL)
    case 0xBF:
        Res(registers->a, 0x80);
        return 8; // RES 7,A
    case 0xC0:
        Set(registers->b, 0x01);
        return 8; // SET 0,B
    case 0xC1:
        Set(registers->c, 0x01);
        return 8; // SET 0,C
    case 0xC2:
        Set(registers->d, 0x01);
        return 8; // SET 0,D
    case 0xC3:
        Set(registers->e, 0x01);
        return 8; // SET 0,E
    case 0xC4:
        Set(registers->h, 0x01);
        return 8; // SET 0,H
    case 0xC5:
        Set(registers->l, 0x01);
        return 8; // SET 0,L
    case 0xC6:
        value = memory->Read(registers->hl);
        Set(value, 0x01);
        memory->Write(registers->hl, value);
        return 16; // SET 0,(HL)
    case 0xC7:
        Set(registers->a, 0x01);
        return 8; // SET 0,A
    case 0xC8:
        Set(registers->b, 0x02);
        return 8; // SET 1,B
    case 0xC9:
        Set(registers->c, 0x02);
        return 8; // SET 1,C
    case 0xCA:
        Set(registers->d, 0x02);
        return 8; // SET 1,D
    case 0xCB:
        Set(registers->e, 0x02);
        return 8; // SET 1,E
    case 0xCC:
        Set(registers->h, 0x02);
        return 8; // SET 1,H
    case 0xCD:
        Set(registers->l, 0x02);
        return 8; // SET 1,L
    case 0xCE:
        value = memory->Read(registers->hl);
        Set(value, 0x02);
        memory->Write(registers->hl, value);
        return 16; // SET 1,(HL)
    case 0xCF:
        Set(registers->a, 0x02);
        return 8; // SET 1,A
    case 0xD0:
        Set(registers->b, 0x04);
        return 8; // SET 2,B
    case 0xD1:
        Set(registers->c, 0x04);
        return 8; // SET 2,C
    case 0xD2:
        Set(registers->d, 0x04);
        return 8; // SET 2,D
    case 0xD3:
        Set(registers->e, 0x04);
        return 8; // SET 2,E
    case 0xD4:
        Set(registers->h, 0x04);
        return 8; // SET 2,H
    case 0xD5:
        Set(registers->l, 0x04);
        return 8; // SET 2,L
    case 0xD6:
        value = memory->Read(registers->hl);
        Set(value, 0x04);
        memory->Write(registers->hl, value);
        return 16; // SET 2,(HL)
    case 0xD7:
        Set(registers->a, 0x04);
        return 8; // SET 2,A
    case 0xD8:
        Set(registers->b, 0x08);
        return 8; // SET 3,B
    case 0xD9:
        Set(registers->c, 0x08);
        return 8; // SET 3,C
    case 0xDA:
        Set(registers->d, 0x08);
        return 8; // SET 3,D
    case 0xDB:
        Set(registers->e, 0x08);
        return 8; // SET 3,E
    case 0xDC:
        Set(registers->h, 0x08);
        return 8; // SET 3,H
    case 0xDD:
        Set(registers->l, 0x08);
        return 8; // SET 3,L
    case 0xDE:
        value = memory->Read(registers->hl);
        Set(value, 0x08);
        memory->Write(registers->hl, value);
        return 16; // SET 3,(HL)
    case 0xDF:
        Set(registers->a, 0x08);
        return 8; // SET 3,A
    case 0xE0:
        Set(registers->b, 0x10);
        return 8; // SET 4,B
    case 0xE1:
        Set(registers->c, 0x10);
        return 8; // SET 4,C
    case 0xE2:
        Set(registers->d, 0x10);
        return 8; // SET 4,D
    case 0xE3:
        Set(registers->e, 0x10);
        return 8; // SET 4,E
    case 0xE4:
        Set(registers->h, 0x10);
        return 8; // SET 4,H
    case 0xE5:
        Set(registers->l, 0x10);
        return 8; // SET 4,L
    case 0xE6:
        value = memory->Read(registers->hl);
        Set(value, 0x10);
        memory->Write(registers->hl, value);
        return 16; // SET 4,(HL)
    case 0xE7:
        Set(registers->a, 0x10);
        return 8; // SET 4,A
    case 0xE8:
        Set(registers->b, 0x20);
        return 8; // SET 5,B
    case 0xE9:
        Set(registers->c, 0x20);
        return 8; // SET 5,C
    case 0xEA:
        Set(registers->d, 0x20);
        return 8; // SET 5,D
    case 0xEB:
        Set(registers->e, 0x20);
        return 8; // SET 5,E
    case 0xEC:
        Set(registers->h, 0x20);
        return 8; // SET 5,H
    case 0xED:
        Set(registers->l, 0x20);
        return 8; // SET 5,L
    case 0xEE:
        value = memory->Read(registers->hl);
        Set(value, 0x20);
        memory->Write(registers->hl, value);
        return 16; // SET 5,(HL)
    case 0xEF:
        Set(registers->a, 0x20);
        return 8; // SET 5,A
    case 0xF0:
        Set(registers->b, 0x40);
        return 8; // SET 6,B
    case 0xF1:
        Set(registers->c, 0x40);
        return 8; // SET 6,C
    case 0xF2:
        Set(registers->d, 0x40);
        return 8; // SET 6,D
    case 0xF3:
        Set(registers->e, 0x40);
        return 8; // SET 6,E
    case 0xF4:
        Set(registers->h, 0x40);
        return 8; // SET 6,H
    case 0xF5:
        Set(registers->l, 0x40);
        return 8; // SET 6,L
    case 0xF6:
        value = memory->Read(registers->hl);
        Set(value, 0x40);
        memory->Write(registers->hl, value);
        return 16; // SET 6,(HL)
    case 0xF7:
        Set(registers->a, 0x40);
        return 8; // SET 6,A
    case 0xF8:
        Set(registers->b, 0x80);
        return 8; // SET 7,B
    case 0xF9:
        Set(registers->c, 0x80);
        return 8; // SET 7,C
    case 0xFA:
        Set(registers->d, 0x80);
        return 8; // SET 7,D
    case 0xFB:
        Set(registers->e, 0x80);
        return 8; // SET 7,E
    case 0xFC:
        Set(registers->h, 0x80);
        return 8; // SET 7,H
    case 0xFD:
        Set(registers->l, 0x80);
        return 8; // SET 7,L
    case 0xFE:
        value = memory->Read(registers->hl);
        Set(value, 0x80);
        memory->Write(registers->hl, value);
        return 16; // SET 7,(HL)
    case 0xFF:
        Set(registers->a, 0x80);
        return 8; // SET 7,A

    default:
        printf("Unknown CB opcode: 0x%02X\n", opcode);
        return 0; // Unknown CB opcode
    }
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
    registers->WriteFlag(Flag::H, ((value & 0x0F) == 0x0F));

    value++;
    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, false);
}

void CPU::Dec(uint8_t &value)
{
    registers->WriteFlag(Flag::H, (value & 0xF) == 0);

    value--;
    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, true);
}

void CPU::And(uint8_t value)
{
    registers->a &= value;
    registers->WriteFlag(Flag::Z, registers->a == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, true);
    registers->WriteFlag(Flag::C, false);
}

void CPU::Or(uint8_t value)
{
    registers->a |= value;
    registers->WriteFlag(Flag::Z, registers->a == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, false);
}

void CPU::Xor(uint8_t value)
{
    registers->a ^= value;
    registers->WriteFlag(Flag::Z, registers->a == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, false);
}

void CPU::Cp(uint8_t value)
{
    uint8_t a = registers->a;

    registers->WriteFlag(Flag::Z, a == value);
    registers->WriteFlag(Flag::N, true);
    registers->WriteFlag(Flag::H, (a & 0xF) < (value & 0xF));
    registers->WriteFlag(Flag::C, a < value);
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

void CPU::Sla(uint8_t &value)
{
    bool isLastBit = (value & 0x80) != 0;
    value <<= 1;

    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, isLastBit);
}

void CPU::Sra(uint8_t &value)
{
    bool bit0 = value & 0x01;
    bool bit7 = value & 0x80;

    value >>= 1;
    value |= bit7;

    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, bit0);
}

void CPU::Srl(uint8_t &value)
{
    bool bit0 = value & 0x01;

    value >>= 1;

    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, bit0);
}

void CPU::Swap(uint8_t &value)
{
    value = (value << 4) | (value >> 4);

    registers->WriteFlag(Flag::Z, value == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, false);
    registers->WriteFlag(Flag::C, false);
}

void CPU::Bit(uint8_t &value, uint8_t bit)
{
    registers->WriteFlag(Flag::Z, (value & bit) == 0);
    registers->WriteFlag(Flag::N, false);
    registers->WriteFlag(Flag::H, true);
}

void CPU::Res(uint8_t &value, uint8_t bit)
{
    value &= ~bit;
}

void CPU::Set(uint8_t &value, uint8_t bit)
{
    value |= bit;
}

void CPU::CheckInterrupts()
{
    if (!ime)
        return;

    uint8_t ie = memory->Read(0xFFFF);
    uint8_t iflag = memory->Read(0xFF0F);

    uint8_t interrupts = ie & iflag & 0x1F;

    if (interrupts == 0)
        return;

    ime = false;
    isHalted = false;

    memory->Write(--registers->sp, (registers->pc >> 8) & 0xFF);
    memory->Write(--registers->sp, registers->pc & 0xFF);

    if (interrupts & 0x01)
    {
        registers->pc = 0x0040;
        memory->Write(0xFF0F, iflag & ~0x01);
    }
    else if (interrupts & 0x02)
    {
        registers->pc = 0x0048;
        memory->Write(0xFF0F, iflag & ~0x02);
    }
    else if (interrupts & 0x04)
    {
        registers->pc = 0x0050;
        memory->Write(0xFF0F, iflag & ~0x04);
    }
    else if (interrupts & 0x08)
    {
        registers->pc = 0x0058;
        memory->Write(0xFF0F, iflag & ~0x08);
    }
    else if (interrupts & 0x10)
    {
        registers->pc = 0x0060;
        memory->Write(0xFF0F, iflag & ~0x10);
    }
}

int CPU::Step()
{
    if (isHalted)
    {
        uint8_t ie = memory->Read(0xFFFF);
        uint8_t iflag = memory->Read(0xFF0F);
        if (ie & iflag & 0x1F)
        {
            isHalted = false;
        }
        else
        {
            cycles += 4;
            return 4;
        }
    }

    uint8_t opcode = memory->Read(registers->pc++);
    int instructionCycles = Execute(opcode);
    cycles += instructionCycles;

    if (enableInterruptsNextInstruction)
    {
        ime = true;
        enableInterruptsNextInstruction = false;
    }

    return instructionCycles;
}