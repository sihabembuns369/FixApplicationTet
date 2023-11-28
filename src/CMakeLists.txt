# Jika menggunakan CMake
cmake_minimum_required(VERSION 3.0)
project(MyQuickFIXProject)

set(CMAKE_CXX_STANDARD 11)

add_executable(myfixapp main.cpp MyConcreteApplication.cpp MyApplication.h MyConcreteApplication.h)

target_link_libraries(myfixapp quickfix)
