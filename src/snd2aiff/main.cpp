#include <sndfile.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

#define WORKING_BUFFER_LEN 1024
std::array<double, WORKING_BUFFER_LEN> working_buffer;

bool check_paths(const std::filesystem::path& src_path, const std::filesystem::path& dest_dir);
bool process_file(const std::filesystem::path& src_path, const std::filesystem::path& dest_audio_path);
bool write_info_file(const std::filesystem::path& src_path, const std::filesystem::path& dest_path);

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: ./snd2aiff /path/to/src_path.snd /path/to/dest/dir" << std::endl;
        return 1;
    }

    std::filesystem::path src_path = argv[1];
    std::filesystem::path dest_dir = argv[2];

    if (!check_paths(src_path, dest_dir)) {
        return 1;
    }

    auto dest_audio_path = (dest_dir / src_path.filename()).replace_extension(".aiff");

    if (!process_file(src_path, dest_audio_path)) {
        return 1;
    }

    auto dest_info_path = dest_audio_path;
    dest_info_path.replace_extension(".json");

    if (!write_info_file(dest_audio_path, dest_info_path)) {
        return 1;
    }

    return 0;
}

bool check_paths(const std::filesystem::path& src_path, const std::filesystem::path& dest_dir) {
    if (!std::filesystem::exists(src_path)) {
        std::cout << "Input " << src_path << " does not exist" << std::endl;
        return false;
    }

    if (!std::filesystem::is_regular_file(src_path)) {
        std::cout << "Input " << src_path << " is not a regular file." << std::endl;
        return false;
    }

    if (!std::filesystem::is_directory(dest_dir)) {
        std::cout << "Destination " << dest_dir << " is not a directory" << std::endl;
        return false;
    }

    return true;
}

void convert_to_aiff(SNDFILE* source_file, SNDFILE* dest_file) {
    int read_count;

    while ((read_count = sf_read_double (source_file, working_buffer.data(), WORKING_BUFFER_LEN))) {
        sf_write_double (dest_file, working_buffer.data(), read_count) ;
    }
}

void convert_stereo_to_mono_aiff(SNDFILE* source_file, SNDFILE* dest_file) {
    int read_count;

    while ((read_count = sf_read_double (source_file, working_buffer.data(), WORKING_BUFFER_LEN))) {
        int actual_buffer_count = 0;

        for (int i = 0; i < read_count; i += 2) {
            double value = working_buffer[i] + working_buffer[i + 1];
            working_buffer[i / 2] = value * 0.5;
        }

        sf_write_double (dest_file, working_buffer.data(), read_count / 2) ;
    }
}

bool process_file(const std::filesystem::path& src_path, const std::filesystem::path& dest_audio_path) {
    SF_INFO source_info = {0};
    SNDFILE* source_file = sf_open(src_path.c_str(), SFM_READ, &source_info);

    if (!source_file) {
        std::cout << "Error opening file: " << src_path << std::endl; 
        std::cout << sf_strerror(NULL) << std::endl;
        return false;
    }

    if (source_info.channels > 2) {
        std::cout << "Input file contains " << source_info.channels << " channels.  Max supported channels is 2." << std::endl;
        return false;
    }

    if (src_path.extension() == ".aiff" && source_info.channels == 1) {
        std::cout << "Copy " << src_path << " to " << dest_audio_path << " (no processing necessary)" << std::endl;
        sf_close(source_file);
        std::filesystem::copy_file(src_path, dest_audio_path);
        return true;
    }

    SF_INFO dest_info = {0};
    dest_info.channels = 1;
    dest_info.samplerate = source_info.samplerate;
    dest_info.format = SF_FORMAT_AIFF | SF_FORMAT_PCM_16;
    SNDFILE* dest_file = sf_open(dest_audio_path.c_str(), SFM_WRITE, &dest_info);

    if (!dest_file) {
        std::cout << "Unable to open output: " << dest_audio_path << std::endl;
        std::cout << sf_strerror(NULL);
        return 1;
    }

    std::cout << "Converting " << src_path << " to aiff" << std::endl;

    if (source_info.channels == 1) {
        convert_to_aiff(source_file, dest_file);
    }
    else {
        std::cout << "Converting stereo to mono" << std::endl;
        convert_stereo_to_mono_aiff(source_file, dest_file);
    }

    sf_close(source_file);
    sf_close(dest_file);

    return true;
}

bool write_info_file(const std::filesystem::path& src_path, const std::filesystem::path& dest_path) {
    SF_INFO source_info = {0};
    SNDFILE* source_file = sf_open(src_path.c_str(), SFM_READ, &source_info);

    if (!source_file) {
        std::cout << "Failed to open converted sound file for writing: " << src_path << std::endl;
        return false;
    }

    sf_close(source_file);

    std::ofstream json_file(dest_path);

    if (!json_file) {
        std::cout << "Failed to open info file for writing: " << dest_path << std::endl;
        return false;
    }

    double seconds = static_cast<double>(source_info.frames) / static_cast<double>(source_info.samplerate); 

    nlohmann::json info_json = {
        {"duration", seconds}
    };

    json_file << info_json.dump(4) << std::endl;

    return true;
}
