set(KERNEL_ELF verdandi.elf)

add_executable(${KERNEL_ELF}
    src/arch/x86_64/entry.S
    src/arch/x86_64/interrupts.S
    src/arch/x86_64/apic.cpp
    src/arch/x86_64/gdt.cpp
    src/arch/x86_64/idt.cpp
    src/arch/x86_64/serial.cpp
    src/console.cpp
    src/main.cpp)

target_include_directories(${KERNEL_ELF} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libvz/include)

set_target_properties(${KERNEL_ELF} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

target_link_options(${KERNEL_ELF} PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

# QEMU -kernel (Multiboot1) only loads ELF32 containers; the kernel is linked
# as ELF64. The converted copy is what boots; verdandi.elf keeps symbols.
find_program(LLVM_OBJCOPY_EXE llvm-objcopy REQUIRED)
add_custom_command(TARGET ${KERNEL_ELF} POST_BUILD
    COMMAND ${LLVM_OBJCOPY_EXE} -O elf32-i386
            $<TARGET_FILE:${KERNEL_ELF}>
            ${CMAKE_BINARY_DIR}/verdandi.boot.elf
    VERBATIM)
