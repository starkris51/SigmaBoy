#include "ppu.h"
#include "mmu.h"

PPU::PPU(MMU *memory, SDL_Renderer *renderer) : memory(memory), renderer(renderer)
{
    lcdc = 0x00;
    stat = 0x00;
    scy = 0x00;
    scx = 0x00;
    ly = 0x00;
    lyc = 0x00;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);

    for (int i = 0; i < 160 * 144; i++)
    {
        framebuffer[i] = 0xFF000000;
    }
}

PPU::~PPU()
{
    if (texture)
        SDL_DestroyTexture(texture);
}

void PPU::Step(int cycles)
{
    modeClock += cycles;

    switch (mode)
    {
    case 2:
        if (modeClock >= 80)
        {
            mode = 3;
            modeClock = 0;
        }
        break;

    case 3:
        if (modeClock >= 172)
        {
            RenderScanline();
            mode = 0;
            modeClock = 0;
        }
        break;

    case 0:
        if (modeClock >= 204)
        {
            line++;
            modeClock = 0;

            if (line == 144)
            {
                mode = 1;
                uint8_t iflag = memory->Read(0xFF0F);
                memory->Write(0xFF0F, iflag | 0x01);
            }
            else
            {
                mode = 2;
            }
        }
        break;

    case 1:
        if (modeClock >= 456)
        {
            line++;
            modeClock = 0;

            if (line > 153)
            {
                line = 0;
                mode = 2;
            }
        }
        break;
    }

    memory->Write(0xFF44, line);
}

void PPU::RenderScanline()
{
    for (int x = 0; x < 160; x++)
    {
        uint32_t color = (line % 2 == 0) ? 0xFFFFFFFF : 0xFFAAAAAA;
        framebuffer[line * 160 + x] = color;
    }

    if (line == 143)
    {
        SDL_UpdateTexture(texture, nullptr, framebuffer, 160 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}