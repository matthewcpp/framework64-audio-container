#include "ins_writer.h"

#include <fstream>

void InstWriter::write(const std::string& path) {
    std::ofstream file(path);

    filters_enabled = (filters.envelopes.size() | filters.keymaps.size() | filters.sounds.size() |filters.instruments.size()) != 0;

    writeBank(json_data["bank"], file);
}

void objectStart(const char* type, const std::string& name, std::ostream& stream) {
    stream << type << ' ' << name << '\n';
    stream << "{\n";
}

void objectEnd(std::ostream& stream) {
    stream << "}\n\n";
}

template <typename T>
void writeItemLine(const std::string& key, const T& value, std::ostream& stream) {
    stream << "    " << key << " = " << value << ";\n";
}

const std::unordered_set<std::string> envelope_skip_items = {"name"};

void InstWriter::writeEnvelope(int envelope_index, std::ostream& stream) {
    if (!shouldWrite(envelope_index, filters.envelopes, serialized.envelopes))
        return;

    const auto& envelope_json = json_data["envelopes"][envelope_index];

    objectStart("envelope", envelope_json["name"].get<std::string>(), stream);

    for (const auto& item : envelope_json.items()) {
        if (envelope_skip_items.count(item.key()))
            continue;

        writeItemLine(item.key(), item.value().get<int64_t>(), stream);
    }

    objectEnd(stream);

    serialized.envelopes.insert(envelope_index);
}

const std::unordered_set<std::string> keymap_skip_items = {"name"};

void InstWriter::writeKeymap(int keymap_index, std::ostream& stream){
    if (!shouldWrite(keymap_index, filters.keymaps, serialized.keymaps))
        return;

    const auto& keymap_json = json_data["keymaps"][keymap_index];

    objectStart("keymap", keymap_json["name"].get<std::string>(), stream);

    for (const auto& item : keymap_json.items()) {
        if (keymap_skip_items.count(item.key()))
            continue;

        writeItemLine(item.key(), item.value().get<int64_t>(), stream);
    }

    objectEnd(stream);

    serialized.keymaps.insert(keymap_index);
}

const std::unordered_set<std::string> sound_skip_items = {"name"};

void InstWriter::writeSound(int sound_index, std::ostream& stream){
    if (!shouldWrite(sound_index, filters.sounds, serialized.sounds))
        return;

    const auto& sound_json = json_data["sounds"][sound_index];
    const int envelope_index = sound_json["envelope"].get<int>();
    const int keymap_index = sound_json["keymap"].get<int>();

    writeEnvelope(envelope_index, stream);
    writeKeymap(keymap_index, stream);

    objectStart("sound", sound_json["name"].get<std::string>(), stream);
    for (const auto& item : sound_json.items()) {
        if (sound_skip_items.count(item.key())) {
            continue;
        }
        if (item.key() == "use"){
            stream << "    use (\"" << item.value().get<std::string>() << "\");\n";
        }
        else if (item.key() == "keymap") {
            const auto& keymap = json_data["keymaps"][keymap_index];
            writeItemLine(item.key(), keymap["name"].get<std::string>(), stream);
        }
        else if (item.key() == "envelope") {
            const auto& envelope = json_data["envelopes"][envelope_index];
            writeItemLine(item.key(), envelope["name"].get<std::string>(), stream);
        }
        else {
            writeItemLine(item.key(), item.value().get<uint64_t>(), stream);
        }

    }
    objectEnd(stream);

    serialized.sounds.insert(sound_index);
}

const std::unordered_set<std::string> instrument_skip_items = {"name", "sounds"};

void InstWriter::writeInstrument(int instrument_index, std::ostream& stream){
    if (!shouldWrite(instrument_index, filters.instruments, serialized.instruments))
        return;

    const auto& instrument_json = json_data["instruments"][instrument_index];
    const auto& sounds = instrument_json["sounds"];

    for (size_t i = 0; i < sounds.size(); i++) {
        const int sound_index = sounds[i].get<int>();
        writeSound(sound_index, stream);
    }

    objectStart("instrument", instrument_json["name"], stream);
    for (const auto& item : instrument_json.items()) {
        if (instrument_skip_items.count(item.key()))
            continue;

        writeItemLine(item.key(), item.value().get<int64_t>(), stream);
    }

    for (size_t i = 0; i < sounds.size(); i++) {
        int sound_index = sounds[i].get<int>();

        if (serialized.sounds.count(sound_index)) {
            const auto& sound = json_data["sounds"][sound_index];
            writeItemLine("sound", sound["name"].get<std::string>(), stream);
        }
    }
    objectEnd(stream);

    serialized.instruments.insert(instrument_index);
}

const std::unordered_set<std::string> bank_skip_items = {"name", "instruments"};

void InstWriter::writeBank(const json& bank_json, std::ostream& stream){
    const auto& instruments = bank_json["instruments"];

    for (size_t i = 0; i < instruments.size(); i++) {
        const int instrument_index = instruments[i]["instrument"].get<int>();
        writeInstrument(instrument_index, stream);
    }

    if (bank_json.contains("percussionDefault"))
        writeInstrument(bank_json["percussionDefault"].get<int>(), stream);

    objectStart("bank", bank_json["name"].get<std::string>(), stream);

    for (const auto& item : bank_json.items()) {
        if (bank_skip_items.count(item.key()))
            continue;

        if (item.key() == "percussionDefault") {
            const int percussion_index = bank_json["percussionDefault"].get<int>();

            if (serialized.instruments.count(percussion_index)) {
                const auto& instrument = json_data["instruments"][percussion_index];
                writeItemLine("percussionDefault", instrument["name"].get<std::string>(), stream);
            }
        }
        else {
            writeItemLine(item.key(), item.value().get<int64_t>(), stream);
        }
    }

    for (size_t i = 0; i < instruments.size(); i++) {
        const int instrument_index = instruments[i]["instrument"].get<int>();

        if (serialized.instruments.count(instrument_index)) {
            int program = instruments[i]["program"].get<int>();

            const auto& instrument = json_data["instruments"][instrument_index];
            stream << "    instrument [" << program << "] = " << instrument["name"].get<std::string>() << ";\n";
        }
    }

    objectEnd(stream);
}

bool InstWriter::shouldWrite(int index, const std::unordered_set<int>& filter_set, const std::unordered_set<int>& serialized_set) const {
    if (serialized_set.count(index))
        return false;

    if (filters_enabled)
        return filter_set.count(index);
    else
        return true;
}