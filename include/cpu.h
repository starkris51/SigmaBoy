#include <SDL3/SDL_timer.h>
#include "registers.h"
#include "mmu.h"

class CPU
{
public:
    CPU(MMU *memory, Registers *registers);

    MMU *memory;
    Registers *registers;

    uint8_t cycles = 0;

    const double CPU_CLOCK_SPEED = 4194304.0; // in Hz
    const double TARGET_FRAMERATE = 59.7275;  // Game Boy frame rate (in Hz)
    const double CYCLES_PER_FRAME = CPU_CLOCK_SPEED / TARGET_FRAMERATE;

    int Execute(uint8_t opcode);
    void Step();

    // CPU Instructions
    void Add(uint8_t value);
    void AddHL(uint16_t value);
    void Sub(uint8_t value);
    void Inc(uint8_t &value);
    void Dec(uint8_t &value);
    void And(uint8_t value);
    void Or(uint8_t value);
    void Xor(uint8_t value);
    void Cmp(uint8_t value);
    void Jump(uint16_t address);
    void Call(uint16_t address);
    void Ret();
    void Rst(uint8_t vector);
    void Push(uint16_t value);
    void Pop(uint16_t &value);
    void Rl(uint8_t &value, bool isPrefixCB);
    void Rlc(uint8_t &value, bool isPrefixCB);
    void Rr(uint8_t &value, bool isPrefixCB);
    void Rrc(uint8_t &value, bool isPrefixCB);
};