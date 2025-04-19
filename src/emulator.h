#pragma once
#include <SDL3/SDL.h>

class Emulator
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *screen;
    bool running;

    static const int SCREEN_WIDTH = 160;
    static const int SCREEN_HEIGHT = 144;
    static const int SCALE = 4;

public:
    Emulator();
    ~Emulator();

    bool Initialize();
    void Run();
    void Cleanup();

private:
    void HandleEvents();
    void Update();
    void Render();
};