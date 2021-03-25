cmake_minimum_required(VERSION 3.16)
project(fw64_audio)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

add_subdirectory(json2ins)
add_subdirectory(snd2aiff)