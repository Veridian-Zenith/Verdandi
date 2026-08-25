#pragma once
#include <stdint.h>

#include "vz/types.hpp"

namespace arch {

/* Stack frame laid out by the common interrupt stub; field order is ABI
   and must match the push sequence in interrupts.S exactly. */
struct Registers {
    vz::u64 r15;
    vz::u64 r14;
    vz::u64 r13;
    vz::u64 r12;
    vz::u64 r11;
    vz::u64 r10;
    vz::u64 r9;
    vz::u64 r8;
    vz::u64 rdi;
    vz::u64 rsi;
    vz::u64 rbp;
    vz::u64 orig_rsp;
    vz::u64 rbx;
    vz::u64 rdx;
    vz::u64 rcx;
    vz::u64 rax;
    vz::u64 vector;
    vz::u64 error;
    /* iretq frame: in long mode SS:RSP are always pushed */
    vz::u64 rip;
    vz::u64 cs;
    vz::u64 rflags;
    vz::u64 rsp;
    vz::u64 ss;
};

using IrqHandler = void (*)(void* ctx, Registers& r);

class Idt {
  public:
    void reset();
    void set_exception_gates(); /* vectors 0..47: CPU faults + external IRQs */
    void bind(vz::u8 vector, IrqHandler fn, void* ctx);
    void unbind(vz::u8 vector);
    void install();
    /* Called by the common stub; the active instance was chosen at install(). */
    void dispatch(Registers& r);

  private:
    struct Binding {
        IrqHandler fn = nullptr;
        void* ctx = nullptr;
    };

    /* Long-mode gates are 16 bytes each; stored as two u64 slots. */
    vz::u64 gates_[512] = {};
    Binding bindings_[256] = {};
};

void print_registers(const Registers& r);
[[noreturn]] void halt_forever();

} // namespace arch
