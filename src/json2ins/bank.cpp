#include "bank.h"

#include "MidiFile.h"

#include <fstream>
#include <array>

void SequenceBank::loadSourceFile(const std::string &path) {
    std::ifstream file(path);

    if (!file)
        throw std::runtime_error("Unable to open file: " + path);

    file >> raw_data;
}

void SequenceBank::writeInstFile(const std::string& path) {
    InstWriter writer(raw_data);
    writer.filters = filters;

    writer.write(path);
}

void SequenceBank::filterMidiFile(const std::string& path) {
    smf::MidiFile midi_file;
    if (!midi_file.read(path))
        throw std::runtime_error("Unable to read midi file: " + path);

    if (midi_file.getTrackCount() > 1) {
        throw std::runtime_error("Midi file is not Type 0");
    }

    std::array<int, 16> midi_channels = {0};
    const auto& bank_json = raw_data["bank"];

    if (bank_json.contains("percussionDefault"))
        midi_channels[9] = bank_json["percussionDefault"].get<int>();

    const auto& track = midi_file[0];

    for (int i = 0; i < track.getEventCount(); i++) {
        const auto& event = track[i];

        int channel = event.getChannel();

        if (event.isNoteOn())
            addNote(midi_channels[channel], event.getKeyNumber(), event.getVelocity());

        if (event.isPatchChange()) {
            midi_channels[channel] = event.getP1();
        }
    }
}

int SequenceBank::findInstrument(int program) {
    const auto& bank = raw_data["bank"];
    const auto& instruments = bank["instruments"];

    for (size_t i = 0; i < instruments.size(); i++) {
        const auto& instrument = instruments[i];

        if (instrument["program"].get<int>() == program) {
            return instrument["instrument"].get<int>();
        }
    }

    if (bank.contains("percussionDefault")) {
        int percussion_index;
        bank["percussionDefault"].get_to(percussion_index);

        if (percussion_index == program)
            return percussion_index;
    }

    throw std::runtime_error("Instrument " + std::to_string(program) + " not present");
}

struct Keymap {
    Keymap(const json& j) {
        j["keyMin"].get_to(key_min);
        j["keyMax"].get_to(key_max);
        j["velocityMin"].get_to(vel_min);
        j["velocityMax"].get_to(vel_max);
    }

    bool acceptsNote(int note, int velocity) {
        return note >= key_min && note <= key_max &&
               velocity >= vel_min && velocity <= vel_max;
    }

    int key_min, key_max, vel_min, vel_max;
};

void SequenceBank::addNote(int program, int note, int velocity) {
    const int instrument_index = findInstrument(program);
    const auto& instrument = raw_data["instruments"][instrument_index];

    const auto& sounds = instrument["sounds"];
    for (size_t i = 0; i < sounds.size(); i++) {
        int sound_index = sounds[i].get<int>();
        const auto& sound = raw_data["sounds"][sound_index];

        int keymap_index = sound["keymap"].get<int>();
        Keymap keymap(raw_data["keymaps"][keymap_index]);

        if (keymap.acceptsNote(note, velocity)) {
            filters.instruments.insert(instrument_index);
            filters.sounds.insert(sound_index);
            filters.keymaps.insert(keymap_index);
            filters.envelopes.insert(sound["envelope"].get<int>());

            return;
        }

    }
}
