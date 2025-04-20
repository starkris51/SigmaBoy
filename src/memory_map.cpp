#include "memory_map.h"

MemoryMap::MemoryMap(Cartridge *cartridge) : cartridge(cartridge) {}

uint8_t MemoryMap::Read(uint16_t address)
{
    if (address <= 0x7FFF)
    {
        return cartridge->ReadROM(address);
    }

    if (address >= 0xFEA0 && address <= 0xFEFF)
    {
        return 0xFF;
    }

    return memory[address];
}

void MemoryMap::Write(uint16_t address, uint8_t value)
{
    if (address <= 0x7FFF)
    {
        // cartridge->WriteROM(address, value);
        return;
    }

    if (address >= 0xFEA0 && address <= 0xFEFF)
    {
        return;
    }

    memory[address] = value;
}