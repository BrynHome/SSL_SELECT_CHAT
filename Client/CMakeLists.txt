cmake_minimum_required(VERSION 3.21)
project(Client C)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
set(CMAKE_C_STANDARD 11)

add_executable(Client
        clnt.c common.c)
find_package(OpenSSL REQUIRED)
target_link_libraries(Client PUBLIC OpenSSL::SSL OpenSSL::Crypto Threads::Threads)
