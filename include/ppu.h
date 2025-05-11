#pragma once
#include <cstdint>
#include "mmu.h"
#include <SDL3/SDL_render.h>

class PPU
{
public:
    PPU(MMU *memory, SDL_Renderer *renderer);
    ~PPU();

    void Step(int cycles);
    void RenderScanline();

    uint32_t framebuffer[160 * 144];
    MMU *memory;
    SDL_Renderer *renderer;

    SDL_Texture *texture = nullptr;

private:
    uint8_t lcdc;
    uint8_t stat;
    uint16_t scy;
    uint16_t scx;
    uint16_t ly;
    uint16_t lyc;
    int mode = 2;
    int modeClock = 0;
    int line = 0;
};