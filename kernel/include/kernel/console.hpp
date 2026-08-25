#pragma once
#include <stdint.h>

namespace console {

/* Output sink indirection: early boot writes go wherever policy/discovery
   says they should (UART, GOP text, later network). No call site changes. */
struct Sink {
    void (*putc)(void* ctx, char c);
    void* ctx;
};

/* The single deliberate root of dynamicity: one registration point, chosen
   at runtime. Everything else in the tree receives dependencies instead of
   owning globals. */
void set_sink(Sink s);

void putc(char c);
void write(const char* s);
void write(const char* data, unsigned long len);

} // namespace console
