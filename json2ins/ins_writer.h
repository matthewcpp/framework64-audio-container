#pragma once

#include "nlohmann/json.hpp"

#include <string>
#include <ostream>
#include <unordered_set>

using nlohmann::json;

class InstWriter {
public:
    struct Indices {
        std::unordered_set<int> keymaps;
        std::unordered_set<int> envelopes;
        std::unordered_set<int> sounds;
        std::unordered_set<int> instruments;
    };

public:
    InstWriter(const json& data) : json_data(data) {}

    void write(const std::string& path);

public:
    Indices filters;

private:
    void writeEnvelope(int envelope_index, std::ostream& stream);
    void writeKeymap(int keymap_index, std::ostream& stream);
    void writeSound(int sound_index, std::ostream& stream);
    void writeInstrument(int instrument_index, std::ostream& stream);
    void writeBank(const json& bank_json, std::ostream& stream);

    bool shouldWrite(int index, const std::unordered_set<int>& filter_set, const std::unordered_set<int>& serialized_set) const;

    bool filters_enabled;
    Indices serialized;
    const json& json_data;
};