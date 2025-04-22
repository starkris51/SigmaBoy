#pragma once
#include "cartridge.h"
#include <cstdint>
#include <array>

class MMU
{
public:
    MMU(Cartridge *cartridge);

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t value);

private:
    Cartridge *cartridge;

    uint8_t vram[0x2000];
    uint8_t eram[0x2000];
    uint8_t wram[0x2000];
    uint8_t oam[0xA0];
    uint8_t io[0x80];
    uint8_t hram[0x7F];
    uint8_t ie;
};
