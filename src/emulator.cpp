#include "emulator.h"
#include <iostream>
#include <string>

Emulator::Emulator() : window(nullptr), renderer(nullptr), screen(nullptr), isEmulatorWindowOpen(false), cartridge(nullptr), memory(nullptr), cpu(nullptr), ppu(nullptr) {}

Emulator::~Emulator()
{
    Cleanup();
}

bool Emulator::ConfigureWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("SigmaBoy",
                              SCREEN_WIDTH * SCALE,
                              SCREEN_HEIGHT * SCALE,
                              SDL_WINDOW_OPENGL);

    if (!window)
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Emulator::OnFileAdded(void *userdata, const char *const *filelist, int filter)
{
    if (filelist && filelist[0])
    {
        std::string rom = filelist[0];
        std::cout << "Loading ROM from: " << rom << std::endl;

        Emulator *emulator = static_cast<Emulator *>(userdata);
        Cartridge *cartridge = new Cartridge(rom);
        emulator->LoadCartridge(cartridge);
    }
}

void Emulator::LoadCartridge(Cartridge *cartridge)
{
    // Cleanup previous components
    if (cartridge)
    {
        delete this->cartridge;
        this->cartridge = nullptr;
    }
    if (memory)
    {
        delete memory;
        memory = nullptr;
    }
    if (cpu)
    {
        delete cpu;
        cpu = nullptr;
    }
    if (ppu)
    {
        delete ppu;
        ppu = nullptr;
    }

    this->cartridge = cartridge;
    memory = new MMU(cartridge);
    cpu = new CPU(memory, &registers);
    ppu = new PPU(memory, renderer);

    SDL_SetWindowTitle(window, cartridge->GetTitle().c_str());

    status.isRunning = true;
}

void Emulator::RunStep()
{
    if (!status.isPaused || status.doStep)
    {
        int cycle = cpu->Step();
        cpu->CheckInterrupts();
    }

    ppu->Step(cpu->cycles);
}

void Emulator::Run()
{
    isEmulatorWindowOpen = true;
    while (isEmulatorWindowOpen)
    {
        uint32_t frameStart = SDL_GetTicks();

        HandleEvents(); // Window inputs

        if (status.isRunning) // Emulator Running
        {
            this->RunStep();
        }

        uint32_t frameEnd = SDL_GetTicks();
        uint32_t elapsed = frameEnd - frameStart;
        if (elapsed < frameDurationMs)
        {
            uint32_t remaining = frameDurationMs - elapsed;
            SDL_Delay(remaining);
        }
    }
}

void Emulator::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            isEmulatorWindowOpen = false;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_RIGHT) // Right-click
            {
                SDL_ShowOpenFileDialog(OnFileAdded,
                                       this, window, filters, 1, 0, 0);
            }
            break;
        }
    }
}

void Emulator::Cleanup()
{
    if (this->cartridge)
    {
        delete this->cartridge;
        this->cartridge = nullptr;
    }
    if (cpu)
    {
        delete cpu;
        cpu = nullptr;
    }
    if (memory)
    {
        delete memory;
        memory = nullptr;
    }
    if (ppu)
    {
        delete ppu;
        ppu = nullptr;
    }

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}