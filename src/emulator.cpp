#include "emulator.h"
#include <iostream>
#include <shobjidl.h>

Emulator::Emulator() : window(nullptr), renderer(nullptr), screen(nullptr), running(false) {}

Emulator::~Emulator()
{
    Cleanup();
}

bool Emulator::Initialize()
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

void Emulator::Run()
{
    running = true;
    while (running)
    {
        HandleEvents();
        Update();
        Render();
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
            running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            // Handle key presses
            break;
        }
    }
}

void Emulator::Update()
{
    // Update emulator state
}

void Emulator::Render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render GameBoy screen here

    SDL_RenderPresent(renderer);
}

void Emulator::Cleanup()
{
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
