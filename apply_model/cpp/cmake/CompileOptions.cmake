# Common compile options for C++

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party)

# https://clang.llvm.org/docs/DiagnosticsReference.html
add_compile_options(-Wall -Wextra -Wpedantic -fno-omit-frame-pointer)

# Turn warnings into errors
add_compile_options(-Werror -Wno-language-extension-token)

add_compile_options(-Wno-error=unused-command-line-argument)
add_compile_options(-Wno-error=unused-but-set-variable)

message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
