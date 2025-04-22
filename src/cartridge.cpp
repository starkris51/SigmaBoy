#include "cartridge.h"

Cartridge::Cartridge(std::string rom)
{
    std::ifstream ROM(rom, std::ios::binary);
    if (!ROM)
    {
        throw std::runtime_error("Failed to open ROM file.");
    }

    ROM.seekg(0, std::ios::end);
    long size = ROM.tellg();
    ROM.seekg(0, std::ios::beg);

    if (size < 0x150)
    {
        throw std::runtime_error("Invalid ROM file size.");
    }

    romData = std::vector<uint8_t>(size);
    ROM.read(reinterpret_cast<char *>(romData.data()), size);

    const std::vector<uint8_t> nintendoLogo = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

    if (!std::equal(nintendoLogo.begin(), nintendoLogo.end(), romData.begin() + 0x104))
    {
        throw std::runtime_error("Invalid Game Boy ROM: Nintendo logo mismatch.");
    }

    if (size % (16 * 1024) != 0)
    {
        std::cout << "Size must be a multiple of 16 KB" << std::endl;
        return;
    }

    rom_title = std::string(romData.begin() + 0x134, romData.begin() + 0x143);

    std::cout << "Rom Title: " << rom_title << std::endl;

    DetectMBC();
}

void Cartridge::DetectMBC()
{
    uint8_t mbcByte = romData[0x147];
    switch (mbcByte)
    {
    case 0x00:
        mbcType = MBCType::NONE;
        break;
    case 0x01:
    case 0x02:
    case 0x03:
        mbcType = MBCType::MBC1;
        break;
    case 0x05:
    case 0x06:
        mbcType = MBCType::MBC2;
        break;
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
        mbcType = MBCType::MBC3;
        break;
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
        mbcType = MBCType::MBC5;
        break;
    default:
        throw std::runtime_error("Unknown MBC type.");
    }
}

uint8_t Cartridge::ReadROM(uint16_t address) const
{
    if (address < 0x4000)
    {
        return romData[address];
    }
    else if (address < 0x8000)
    {
        uint16_t bankOffset = currentBank * 0x4000;
        return romData[bankOffset + (address - 0x4000)];
    }
    return 0xFF;
}

void Cartridge::SwitchBank(uint8_t bank)
{
    if (mbcType == MBCType::MBC1)
    {
        currentBank = bank % (romData.size() / 0x4000);
    }
}