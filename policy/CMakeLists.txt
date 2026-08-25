# Standalone-buildable per the extraction contract in docs/architecture.md.
if(NOT PROJECT_NAME)
    cmake_minimum_required(VERSION 3.25)
    project(policy CXX)
    enable_testing()
endif()

add_library(policy INTERFACE)
target_include_directories(policy INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(policy_test tests/test_policy.cpp)
target_link_libraries(policy_test PRIVATE policy)

add_test(NAME policy COMMAND policy_test)
