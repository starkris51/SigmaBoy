#pragma once
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

enum class MBCType
{
    NONE,
    MBC1,
    MBC2,
    MBC3,
    MBC5
};

class Cartridge
{
public:
    Cartridge(std::string rom);

    uint8_t ReadROM(uint16_t address) const;
    void SwitchBank(uint8_t bank);

private:
    std::vector<uint8_t> romData;
    MBCType mbcType;
    uint8_t currentBank;

    std::string rom_title;

    void DetectMBC();
};
