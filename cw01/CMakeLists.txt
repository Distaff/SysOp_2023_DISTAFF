# CMakeList.txt : CMake project for PiotrKadziela-cw01, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("PiotrKadziela-cw01")

# Add source to this project's executable.
add_executable (PiotrKadziela-cw01 "fileData.cpp" "fileData.h" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PiotrKadziela-cw01 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
