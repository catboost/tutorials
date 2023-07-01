cmake_minimum_required(VERSION 3.14)
project(apply-model)

include(cmake/Ccache.cmake)
include(cmake/CheckCompiler.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Development.cmake)

add_subdirectory(bin)
add_subdirectory(third_party)
