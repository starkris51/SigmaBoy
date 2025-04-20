#include "cartridge.h"

Cartridge::Cartridge(std::string rom)
{
    std::ifstream file(rom, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Unable to open ROM file: " << rom << std::endl;
    }
    else
    {
        std::cerr << "Test" << rom << std::endl;
    }
}

Cartridge::~Cartridge()
{
}
