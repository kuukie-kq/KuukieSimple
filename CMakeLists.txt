cmake_minimum_required(VERSION 3.17)
project(KuukieSimple)

set(CMAKE_CXX_STANDARD 14)

add_executable(KuukieSimple main.cpp testcpu/version/one/cpu.cpp testcpu/version/two/cpu.cpp)