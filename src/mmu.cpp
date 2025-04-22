#include "mmu.h"

MMU::MMU(Cartridge *cartridge) : cartridge(cartridge) {}

uint8_t MMU::Read(uint16_t address)
{
    if (address <= 0x7FFF)
        return cartridge->ReadROM(address);
    else if (address >= 0x8000 && address <= 0x9FFF)
        return vram[address - 0x8000];
    else if (address >= 0xA000 && address <= 0xBFFF)
        return eram[address - 0xA000];
    else if (address >= 0xC000 && address <= 0xDFFF)
        return wram[address - 0xC000];
    else if (address >= 0xE000 && address <= 0xFDFF)
        return wram[address - 0xE000];
    else if (address >= 0xFE00 && address <= 0xFE9F)
        return oam[address - 0xFE00];
    else if (address >= 0xFF00 && address <= 0xFF7F)
        return io[address - 0xFF00];
    else if (address >= 0xFF80 && address <= 0xFFFE)
        return hram[address - 0xFF80];
    else if (address == 0xFFFF)
        return ie;
    return 0xFF;
}

void MMU::Write(uint16_t address, uint8_t value)
{
    if (address <= 0x7FFF)
    {
        // cartridge->WriteROM(address, value); // for MBC
    }
    else if (address >= 0x8000 && address <= 0x9FFF)
        vram[address - 0x8000] = value;
    else if (address >= 0xA000 && address <= 0xBFFF)
        eram[address - 0xA000] = value;
    else if (address >= 0xC000 && address <= 0xDFFF)
        wram[address - 0xC000] = value;
    else if (address >= 0xE000 && address <= 0xFDFF)
        wram[address - 0xE000] = value;
    else if (address >= 0xFE00 && address <= 0xFE9F)
        oam[address - 0xFE00] = value;
    else if (address >= 0xFF00 && address <= 0xFF7F)
        io[address - 0xFF00] = value;
    else if (address >= 0xFF80 && address <= 0xFFFE)
        hram[address - 0xFF80] = value;
    else if (address == 0xFFFF)
        ie = value;
}