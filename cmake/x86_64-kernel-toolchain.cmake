# Freestanding 64-bit toolchain for the Verdandi kernel.
# entry.S runs 32-bit protected mode just long enough to page-table itself
# into long mode (PAE + EFER.LME + far jump); everything else is x86_64.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)

# FORCE here (not FLAGS_INIT + preset overrides): toolchain files are
# re-included on every configure, so this pins flags deterministically and
# makes shell CFLAGS/CPPFLAGS/LDFLAGS irrelevant without preset gymnastics.
set(KERNEL_LANG_FLAGS "-target x86_64-pc-none-elf -mno-red-zone -mno-mmx -mno-sse -mno-sse2")

set(CMAKE_C_FLAGS "${KERNEL_LANG_FLAGS} -ffreestanding -fno-builtin -Wall -Wextra"
    CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${KERNEL_LANG_FLAGS} -ffreestanding -fno-builtin -nostdinc++ -std=c++26 -fno-exceptions -fno-rtti -fno-stack-protector -fno-pic -fno-pie -Wall -Wextra"
    CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${KERNEL_LANG_FLAGS}" CACHE STRING "" FORCE)

set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -static -fuse-ld=lld -Wl,--build-id=none"
    CACHE STRING "" FORCE)
