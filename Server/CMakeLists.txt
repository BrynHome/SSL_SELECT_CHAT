cmake_minimum_required(VERSION 3.21)
project(Server C)

set(CMAKE_C_STANDARD 11)

add_executable(Server
        svr.c common.c common.h)
