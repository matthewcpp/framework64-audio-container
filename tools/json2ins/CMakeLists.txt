FetchContent_Declare(midifile
  URL https://github.com/craigsapp/midifile/archive/refs/heads/master.zip
)

if(NOT midifile_POPULATED)
    FetchContent_Populate(midifile)
    add_subdirectory(${midifile_SOURCE_DIR} ${midifile_BINARY_DIR})
endif()

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    GIT_TAG v3.9.1)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


add_executable(json2ins main.cpp bank.cpp bank.h ins_writer.cpp ins_writer.h)

target_link_libraries(json2ins PRIVATE midifile nlohmann_json::nlohmann_json)
target_include_directories(json2ins PRIVATE ${midifile_SOURCE_DIR}/include)