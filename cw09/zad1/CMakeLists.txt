# CMakeList.txt : CMake project for PiotrKadziela-cw02
#
cmake_minimum_required (VERSION 3.8)

project ("cw09")

# Add source to this project's executable.
add_executable (cw09 "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cw09 PROPERTY CXX_STANDARD 20)
  



	find_package(Threads REQUIRED)
	if(THREADS_HAVE_PTHREAD_ARG)
	  target_compile_options(cw09 PUBLIC "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
	  target_link_libraries(cw09 "${CMAKE_THREAD_LIBS_INIT}")
	endif()


endif()

# TODO: Add tests and install targets if needed.
