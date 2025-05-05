#include <SDL3/SDL_timer.h>
#include "registers.h"
#include "mmu.h"

class CPU
{
public:
    CPU(MMU *memory, Registers *registers);

    MMU *memory;
    Registers *registers;

    bool enableInterruptsNextInstruction = false;

    bool ime = false;

    uint8_t cycles = 0;
    bool isStopped = false;
    bool isHalted = false;

    int Execute(uint8_t opcode);
    void CheckInterrupts();
    int Step();

    // CPU Instructions
    void Add(uint8_t value);
    void AddHL(uint16_t value);
    void Adc(uint8_t value);
    void Sub(uint8_t value);
    void Sbc(uint8_t value);
    void Inc(uint8_t &value);
    void Dec(uint8_t &value);
    void And(uint8_t value);
    void Or(uint8_t value);
    void Xor(uint8_t value);
    void Cp(uint8_t value);
    void Rl(uint8_t &value, bool isPrefixCB);
    void Rlc(uint8_t &value, bool isPrefixCB);
    void Rr(uint8_t &value, bool isPrefixCB);
    void Rrc(uint8_t &value, bool isPrefixCB);
};