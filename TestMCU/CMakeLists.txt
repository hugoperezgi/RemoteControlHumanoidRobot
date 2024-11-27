cmake_minimum_required(VERSION 3.0)
project(TestMCU)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


set(SOURCES
    CliSock.cpp
    main.cpp
)

add_executable(TestMCU ${SOURCES})
# target_link_libraries(SyssockTCPSockets ws2_32)