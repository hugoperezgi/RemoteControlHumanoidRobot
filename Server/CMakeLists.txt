cmake_minimum_required(VERSION 3.10)
project(ControlServer)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


set(SOURCES
    Server.cpp
)

add_executable(ControlServer ${SOURCES})

add_subdirectory(controllers)
enable_testing()
add_subdirectory(test)

set_target_properties(ControlServer PROPERTIES WIN32_EXECUTABLE false)  # keep false to include console output
target_link_libraries(ControlServer ControllersLib)
target_link_libraries(ControlServer ws2_32)
target_link_libraries(ControlServer SQLite3)
