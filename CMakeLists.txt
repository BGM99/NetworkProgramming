cmake_minimum_required(VERSION 3.27)
project(NetworkProgramming C)

set(CMAKE_C_STANDARD 11)

add_executable(NetworkProgramming main.c
        daytimeprt.c
        daytimeprt.h
        echoprt.c
        echoprt.h
        webserver.c
        webserver.h
        chat.c
        chat.h)
