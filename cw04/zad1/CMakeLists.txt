cmake_minimum_required (VERSION 3.8)

project ("cw04")

# Add source to this project's executable.
add_executable (cw04 "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cw04 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
