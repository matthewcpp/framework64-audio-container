find_package(nlohmann_json CONFIG REQUIRED)

set(midifile_dir ${CMAKE_CURRENT_SOURCE_DIR}/midifile)
set(midifile_include_dir ${midifile_dir}/include)
set(midifile_src_dir ${midifile_dir}/src)
set(midifile_src
    ${midifile_src_dir}/MidiFile.cpp
    ${midifile_src_dir}/MidiEvent.cpp
    ${midifile_src_dir}/MidiEventList.cpp
    ${midifile_src_dir}/MidiMessage.cpp
    ${midifile_src_dir}/Options.cpp
    ${midifile_src_dir}/Binasc.cpp
)

set (json2ins_src
    main.cpp bank.cpp bank.h ins_writer.cpp ins_writer.h
)

add_executable(json2ins ${json2ins_src} ${midifile_src})

target_include_directories(json2ins PRIVATE ${midifile_include_dir})
target_link_libraries(json2ins PRIVATE nlohmann_json::nlohmann_json)
