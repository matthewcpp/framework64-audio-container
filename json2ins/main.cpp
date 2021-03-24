#include "bank.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: ./json2ins source_file out_file [midifiles]" << std::endl;
        return 1;
    }

    std::string src_json = argv[1];
    std::string out_inst = argv[2];

    SequenceBank sequence_bank;
    sequence_bank.loadSourceFile(src_json);

    for (int i = 3; i < argc; i++) {
        sequence_bank.filterMidiFile(argv[i]);
    }

    sequence_bank.writeInstFile(out_inst);

    return 0;
}
