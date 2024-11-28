cmake_minimum_required(VERSION 3.0)
project(TestClient)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


set(SOURCES
    Client.cpp
    CliSock.cpp
    main.cpp
)

add_executable(TestClient ${SOURCES})
target_link_libraries(TestClient ws2_32)