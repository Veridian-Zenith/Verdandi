#include "arch/x86_64/serial.hpp"

namespace arch {

void SerialPort::outb(uint16_t off, uint8_t v) const {
    asm volatile("outb %0, %1" : : "a"(v), "Nd"(static_cast<uint16_t>(base_ + off)));
}

uint8_t SerialPort::inb(uint16_t off) const {
    uint8_t v;
    asm volatile("inb %1, %0" : "=a"(v) : "Nd"(static_cast<uint16_t>(base_ + off)));
    return v;
}

void SerialPort::init() {
    outb(1, 0x00); // IER: no interrupts yet
    outb(3, 0x80); // LCR: DLAB on
    outb(0, 0x01); // divisor low: 1 -> 115200 baud
    outb(1, 0x00); // divisor high
    outb(3, 0x03); // LCR: 8N1, DLAB off
    outb(2, 0xC7); // FCR: FIFO on, clear both
    outb(4, 0x0B); // MCR: DTR+RTS+OUT2
}

void SerialPort::putc(char c) {
    while (!(inb(5) & 0x20)) {
    } // LSR bit5: THR empty
    outb(0, static_cast<uint8_t>(c));
}

void SerialPort::write(const char* s) {
    while (*s) {
        putc(*s++);
    }
}

} // namespace arch
