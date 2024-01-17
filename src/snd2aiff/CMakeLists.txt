cmake_minimum_required(VERSION 3.16)
project(snd2aiff)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SndFile CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(snd2aiff main.cpp)

target_link_libraries(snd2aiff PRIVATE SndFile::sndfile nlohmann_json::nlohmann_json)