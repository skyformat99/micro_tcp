###
## MIT License
##
## Copyright (c) 2017 Stefan Broekman. All rights reserved.
## https://stefanbroekman.nl
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.
###

cmake_minimum_required(VERSION 3.5)
project(micro_tcp)

###Options###
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_DEBUG OFF)

###Compiler options###
set(CMAKE_CXX_STANDARD 14)
if (MSVC)
    add_compile_options(/bigobj /W4 /MP)
else (MSVC)
    add_compile_options(-O2 -g3 -Wall -Wextra -Wconversion)
endif (MSVC)

###Output settings###
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
message(STATUS "Working directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Using build tool: ${CMAKE_BUILD_TOOL}")
message(STATUS "Using C++ version: ${CMAKE_CXX_STANDARD}")

###Copy example files to the binary directory###
file(COPY example/config.xml DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY example/secure DESTINATION ${CMAKE_BINARY_DIR}/bin)

###Required packages###
find_package(Boost REQUIRED COMPONENTS
        system
        program_options
        date_time
        filesystem)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

###Included directories###
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        SYSTEM ${Boost_INCLUDE_DIRS}
        SYSTEM ${OPENSSL_INCLUDE_DIR})

###Linked directories###
link_directories(
        ${CMAKE_BINARY_DIR}/bin
        ${CMAKE_BINARY_DIR}/lib)

###Definitions###
add_definitions(-DBOOST_ALL_STATIC_LINK=1)
add_definitions(-DBOOST_ASIO_NO_DEPRECATED=1)

###Operating system/compiler specific configuration###
message(STATUS "${CMAKE_SYSTEM} detected.")
if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    set(WINSOCK_API_LIBRARIES "-lws2_32 -lmswsock")
elseif (UNIX)
    #Do something for Unix
else (WIN32)
    message(FATAL_ERROR "It is not possible to build this application on this system! Currently the following systems \
    are supported: all Unix systems and Windows with MinGW(preferably mingw-w64) or MSVC.")
endif (WIN32)

#############################################
################# TARGET(S) ###################
file(GLOB PROJECT_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/*.hpp
        ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB MICRO_TCP_FILES
        ${PROJECT_SOURCE_DIR}/include/micro_tcp/*.hpp
        ${PROJECT_SOURCE_DIR}/include/micro_tcp/*.cpp)

add_executable(micro_tcp
        ${PROJECT_SOURCE_FILES}
        ${MICRO_TCP_FILES})

target_link_libraries(micro_tcp
        ${Boost_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${WINSOCK_API_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

#############################################
#############################################

###Print all values of a property###
function(print_property title property)
    get_property(properties DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY ${property})
    set(all_properties "")
    foreach (prop ${properties})
        set(all_properties "${all_properties} \n\t-- \"${prop}\"")
    endforeach ()
    message(STATUS "${title} ${all_properties}")
endfunction(print_property)

print_property("Compiler options" COMPILE_OPTIONS)
print_property("Included directories" INCLUDE_DIRECTORIES)
print_property("Linked directories" LINK_DIRECTORIES)
print_property("Definitions" COMPILE_DEFINITIONS)