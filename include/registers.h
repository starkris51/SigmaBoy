#pragma once

#include <stdint.h>
#include <fstream>
#include <iostream>

class Registers
{
public:
    uint16_t pc = 0;
    uint16_t sp;

    struct
    {
        union
        {
            struct
            {
                uint8_t f;
                uint8_t a;
            };
            uint16_t af;
        };
    };

    struct
    {
        union
        {
            struct
            {
                uint8_t c;
                uint8_t b;
            };
            uint16_t bc;
        };
    };

    struct
    {
        union
        {
            struct
            {
                uint8_t e;
                uint8_t d;
            };
            uint16_t de;
        };
    };

    struct
    {
        union
        {
            struct
            {
                uint8_t l;
                uint8_t h;
            };
            uint16_t hl;
        };
    };
};