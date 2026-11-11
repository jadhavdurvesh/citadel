cmake_minimum_required(VERSION 3.16)
project(Citadel VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(CITADEL_BUILD_TESTS "Build Citadel tests" ON)
add_library(citadel_core STATIC
    src/core/parser.c
    src/core/registry.c
    src/core/dispatcher.c
    src/connectors/provider.c
    src/connectors/octet.c
    src/connectors/contract.c)
target_include_directories(citadel_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
  target_compile_options(citadel_core PRIVATE /W4 /WX)
else()
  target_compile_options(citadel_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(citadel src/host/main.c src/host/demo_adapter.c)
target_link_libraries(citadel PRIVATE citadel_core)

if(CITADEL_BUILD_TESTS)
  enable_testing()
  add_executable(citadel_tests
    tests/test_main.c tests/test_parser.c tests/test_registry.c
    tests/test_dispatcher.c tests/test_provider.c
    tests/test_octet.c tests/test_contract.c)
  target_link_libraries(citadel_tests PRIVATE citadel_core)
  add_test(NAME citadel_tests COMMAND citadel_tests)
endif()
