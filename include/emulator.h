#pragma once
#include <SDL3/SDL.h>
#include "cartridge.h"
#include "cpu.h"
#include "mmu.h"
#include "status.h"
#include "registers.h"
#include "ppu.h"

class Emulator
{
private:
    // SDL
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *screen;
    const SDL_DialogFileFilter filters[1] = {{"Gameboy File", "*"}};
    bool isEmulatorWindowOpen;
    const double frameDurationMs = 1000.0 / 59.7275;

    static const int SCREEN_WIDTH = 160;
    static const int SCREEN_HEIGHT = 144;
    static const int SCALE = 4;

public:
    Emulator();
    ~Emulator();

    // SDL
    bool ConfigureWindow();
    void Cleanup();

    // Emulator Functions
    void LoadCartridge(Cartridge *cartridge);
    static void OnFileAdded(void *userdata, const char *const *filelist, int filter);
    void Run();

    // Emulator Hardware
    Status status;

    Registers registers;
    Cartridge *cartridge;
    MMU *memory;
    CPU *cpu;
    PPU *ppu;

private:
    void HandleEvents();
    void RunStep();
};