cmake_minimum_required(VERSION 3.16)
project(HelloWorldCPU)

set(CMAKE_CXX_STANDARD 14)


add_executable(HelloWorldCPU main.cpp virtual_cpu.h)
