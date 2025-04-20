#include "emulator.h"

int main(int argc, char *argv[])
{
    Emulator emulator;

    if (!emulator.ConfigureWindow())
    {
        return 1;
    }

    emulator.Run();
    return 0;
}