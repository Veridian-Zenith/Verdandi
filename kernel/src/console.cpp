#include "kernel/console.hpp"

namespace console {

namespace {
Sink g_sink{nullptr, nullptr};
}

void set_sink(Sink s) { g_sink = s; }

void putc(char c) {
    if (g_sink.putc) {
        g_sink.putc(g_sink.ctx, c);
    }
}

void write(const char* s) {
    while (*s) {
        putc(*s++);
    }
}

void write(const char* data, unsigned long len) {
    for (unsigned long i = 0; i < len; ++i) {
        putc(data[i]);
    }
}

} // namespace console
