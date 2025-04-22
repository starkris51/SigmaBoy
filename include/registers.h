#pragma once

#include <stdint.h>
#include <fstream>
#include <iostream>

enum Flag : uint8_t {
    Z = 1 << 7, // Zero
    N = 1 << 6, // Subtract
    H = 1 << 5, // Half Carry
    C = 1 << 4  // Carry
};

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