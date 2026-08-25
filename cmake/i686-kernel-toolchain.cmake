# Freestanding 32-bit toolchain for the Verdandi kernel.
# M0 runs in protected mode; the long-mode trampoline lands in M1 and will
# extend this file rather than replace it.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang)

# FORCE here (not FLAGS_INIT + preset overrides): toolchain files are
# re-included on every configure, so this pins flags deterministically and
# makes shell CFLAGS/CPPFLAGS/LDFLAGS irrelevant without preset gymnastics.
set(KERNEL_LANG_FLAGS "-target i386-pc-none-elf -march=i686 -mno-mmx -mno-sse -mno-sse2")

set(CMAKE_C_FLAGS "${KERNEL_LANG_FLAGS} -ffreestanding -fno-builtin -Wall -Wextra"
    CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${KERNEL_LANG_FLAGS} -ffreestanding -fno-builtin -nostdinc++ -std=c++26 -fno-exceptions -fno-rtti -fno-stack-protector -fno-pic -fno-pie -Wall -Wextra"
    CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${KERNEL_LANG_FLAGS}" CACHE STRING "" FORCE)

set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -static -fuse-ld=lld -Wl,--build-id=none"
    CACHE STRING "" FORCE)
