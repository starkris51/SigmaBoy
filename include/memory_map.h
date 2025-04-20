#pragma once
#include "cartridge.h"
#include <cstdint>
#include <array>

class MemoryMap
{
public:
    MemoryMap(Cartridge *cartridge);

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t value);

private:
    Cartridge *cartridge;

    uint8_t memory[0xFFFF];
};