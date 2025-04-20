#pragma once

enum ColorModes
{
    NORMAL = 0,
    RETRO = 1,
    GRAY = 2,
    SIGMA = 3
};

struct Status
{
    bool isRunning = false;
    bool isPaused = false;
    bool doStep = false;
    bool soundEnabled = true;

    int colorMode = NORMAL;
};