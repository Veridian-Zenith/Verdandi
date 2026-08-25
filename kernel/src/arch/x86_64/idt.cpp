#include "arch/x86_64/idt.hpp"

#include "arch/x86_64/gdt.hpp"
#include "kernel/console.hpp"
#include "vz/fmt.hpp"

constexpr unsigned kStubCount = 48;

extern "C" const void* isr_stub_table[kStubCount];

namespace arch {

namespace {

/* Second deliberate runtime-registration root (see console sink): the stub
   entry has no instance context, so install() publishes the dispatcher here. */
Idt* g_active_idt = nullptr;

constexpr const char* kMnemonics[20] = {
    "#DE", "#DB", "NMI", "#BP", "#OF", "#BR", "#UD", "#NM", "#DF", "CSO",
    "#TS", "#NP", "#SS", "#GP", "#PF", "RSV", "#MF", "#AC", "#MC", "#XF"};

/* Long-mode gate, 16 bytes: off[15:0] sel[31:16] ist[34:32] attr[39:32]
   off[47:32] | off[63:48] in the second word. */
void make_gate(vz::u64* slot, vz::u64 offset, vz::u16 selector, vz::u8 type_attr) {
    slot[0] = (offset & 0xFFFFu) | static_cast<vz::u64>(selector) << 16 |
              static_cast<vz::u64>(type_attr) << 40 |
              ((offset >> 16) & 0xFFFFull) << 48;
    slot[1] = offset >> 32;
}

vz::u64 read_cr(vz::u8 n) {
    switch (n) {
        case 0: {
            vz::u64 v;
            asm volatile("movq %%cr0, %0" : "=r"(v));
            return v;
        }
        case 2: {
            vz::u64 v;
            asm volatile("movq %%cr2, %0" : "=r"(v));
            return v;
        }
        default: {
            vz::u64 v;
            asm volatile("movq %%cr3, %0" : "=r"(v));
            return v;
        }
    }
}

void put_hex(vz::u64 v, unsigned width) {
    char buf[16];
    vz::fmt::to_hex(buf, v, width);
    console::write(buf, width);
}

} // namespace

void Idt::reset() {
    for (auto& g : gates_) {
        g = 0;
    }
    for (auto& b : bindings_) {
        b = {};
    }
}

void Idt::set_exception_gates() {
    for (unsigned i = 0; i < kStubCount; ++i) {
        auto offset = reinterpret_cast<vz::u64>(isr_stub_table[i]);
        make_gate(&gates_[2 * i], offset, Gdt::kSelCode, 0x8E);
    }
}

void Idt::bind(vz::u8 vector, IrqHandler fn, void* ctx) {
    bindings_[vector] = {fn, ctx};
}

void Idt::unbind(vz::u8 vector) {
    bindings_[vector] = {};
}

void Idt::install() {
    struct {
        vz::u16 limit;
        vz::u64 base;
    } __attribute__((packed)) idtr{sizeof(gates_) - 1,
                                   reinterpret_cast<vz::u64>(gates_)};
    asm volatile("lidt %0" : : "m"(idtr));
    g_active_idt = this;
}

void Idt::dispatch(Registers& r) {
    const Binding& b = bindings_[r.vector];
    if (b.fn != nullptr) {
        b.fn(b.ctx, r);
        return;
    }
    if (r.vector < 32) {
        print_registers(r);
        halt_forever();
    }
}

void print_registers(const Registers& r) {
    const char* name = r.vector < 20 ? kMnemonics[r.vector] : "???";

    console::write("\n[verdandi] cpu fault v=");
    put_hex(r.vector, 4);
    console::write(" ");
    console::write(name);
    console::write(" err=");
    put_hex(r.error, 16);
    console::write("\n");

    console::write("  rax="), put_hex(r.rax, 16);
    console::write(" rbx="), put_hex(r.rbx, 16);
    console::write(" rcx="), put_hex(r.rcx, 16);
    console::write(" rdx="), put_hex(r.rdx, 16);
    console::write("\n");
    console::write("  rsi="), put_hex(r.rsi, 16);
    console::write(" rdi="), put_hex(r.rdi, 16);
    console::write(" rbp="), put_hex(r.rbp, 16);
    console::write(" rsp="), put_hex(r.orig_rsp, 16);
    console::write("\n");
    console::write("  r08="), put_hex(r.r8, 16);
    console::write(" r09="), put_hex(r.r9, 16);
    console::write(" r10="), put_hex(r.r10, 16);
    console::write(" r11="), put_hex(r.r11, 16);
    console::write("\n");
    console::write("  r12="), put_hex(r.r12, 16);
    console::write(" r13="), put_hex(r.r13, 16);
    console::write(" r14="), put_hex(r.r14, 16);
    console::write(" r15="), put_hex(r.r15, 16);
    console::write("\n");
    console::write("  rip="), put_hex(r.rip, 16);
    console::write(" cs="), put_hex(r.cs, 4);
    console::write(" rfl="), put_hex(r.rflags, 16);
    console::write("\n");
    console::write("  cr0="), put_hex(read_cr(0), 16);
    console::write(" cr2="), put_hex(read_cr(2), 16);
    console::write(" cr3="), put_hex(read_cr(3), 16);
    console::write("\n");
}

[[noreturn]] void halt_forever() {
    for (;;) {
        asm volatile("cli\n\thlt");
    }
}

} // namespace arch

extern "C" void idt_common(arch::Registers* r) {
    if (arch::g_active_idt == nullptr) {
        arch::halt_forever();
    }
    arch::g_active_idt->dispatch(*r);
}
