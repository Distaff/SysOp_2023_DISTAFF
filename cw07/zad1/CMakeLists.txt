# CMakeList.txt : CMake project for PiotrKadziela-cw02
#
cmake_minimum_required (VERSION 3.8)

project ("cw07")

# Add source to this project's executable.
add_executable (cw07 "main.cpp" "queue.h" "pool.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cw07 PROPERTY CXX_STANDARD 20)
  target_compile_options(cw07 PUBLIC "-lrt")
	find_package(Threads REQUIRED)
	if(THREADS_HAVE_PTHREAD_ARG)
		target_compile_options(cw07 PUBLIC "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
		target_link_libraries(cw07 "${CMAKE_THREAD_LIBS_INIT}")
		target_link_libraries(cw07 "${CMAKE_THREAD_LIBS_INIT}" -lrt)
	endif()
endif()

# TODO: Add tests and install targets if needed.
