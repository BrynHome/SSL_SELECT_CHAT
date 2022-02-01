cmake_minimum_required(VERSION 3.21)
project(Server C)

set(CMAKE_C_STANDARD 11)

add_executable(Server
        svr.c common.c common.h)
find_package(OpenSSL REQUIRED)
target_link_libraries(Server PUBLIC OpenSSL::SSL OpenSSL::Crypto)