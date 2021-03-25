#pragma once

#include "nlohmann/json.hpp"
#include "ins_writer.h"

#include <map>
#include <memory>

using nlohmann::json;

class SequenceBank {
public:
    void loadSourceFile(const std::string& path);
    void writeInstFile(const std::string& path);

    void filterMidiFile(const std::string& path);

private:
    void addNote(int program, int note, int velocity);
    int findInstrument(int program);

private:
    InstWriter::Indices filters;
    json raw_data;
};