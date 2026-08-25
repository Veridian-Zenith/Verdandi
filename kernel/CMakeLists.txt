set(KERNEL_ELF verdandi.elf)

add_executable(${KERNEL_ELF}
    src/arch/x86_64/entry.S
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
