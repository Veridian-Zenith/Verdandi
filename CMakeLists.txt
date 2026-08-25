cmake_minimum_required(VERSION 3.25)

project(verdandi
    VERSION 0.1.0
    DESCRIPTION "Capability-policy-native operating system"
    LANGUAGES CXX ASM)

set(VERDANDI_VARIANT "host" CACHE STRING "kernel | host")

if(VERDANDI_VARIANT STREQUAL "host")
    # Reset inherited shell flags here, then pin host-native ones. Env vars
    # (CFLAGS/CXXFLAGS/CPPFLAGS/LDFLAGS) are deliberately ignored everywhere
    # in this repo; each build entry point re-pins what it needs.
    # VERDANDI_SANITIZERS lets CI add e.g. -fsanitize=address,undefined.
    set(VERDANDI_SANITIZERS "" CACHE STRING "host-only sanitizer flags")
    set(CMAKE_C_FLAGS "" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "-march=native -mtune=native ${VERDANDI_SANITIZERS}" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld ${VERDANDI_SANITIZERS}" CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(VERDANDI_VARIANT STREQUAL "kernel")
    enable_language(ASM)
    add_subdirectory(kernel)
else()
    enable_testing()
    add_subdirectory(policy)
    # libvz gains host tests here as it grows; it is header-mostly today.
endif()
