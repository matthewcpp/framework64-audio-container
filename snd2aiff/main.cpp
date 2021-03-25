#include <sndfile.h>

#include <array>
#include <experimental/filesystem>
#include <iostream>
#include <string>

namespace fs = std::experimental::filesystem;

#define WORKING_BUFFER_LEN 1024
std::array<double, WORKING_BUFFER_LEN> working_buffer;

bool check_paths(const fs::path& src_path, const fs::path& dest_dir);
fs::path get_dest_path(const fs::path& src_path);
void convert_to_aiff(SNDFILE* source_file, SNDFILE* dest_file);
void convert_stereo_to_mono_aiff(SNDFILE* source_file, SNDFILE* dest_file);

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: ./snd2aiff /path/to/src_path.snd /path/to/dest/dir" << std::endl;
        return 1;
    }

    fs::path src_path = argv[1];
    fs::path dest_dir = argv[2];

    if (!check_paths(src_path, dest_dir))
        return 1;

    const auto src_ext = src_path.extension().string();

    SF_INFO source_info = {0};
    SNDFILE* source_file = sf_open(src_path.c_str(), SFM_READ, &source_info);

    if (!source_file) {
        std::cout << "Error opening file: " << src_path << std::endl; 
        std::cout << sf_strerror(NULL) << std::endl;
        return 1;
    }

    if (source_info.channels > 2) {
        std::cout << "Input file contains " << source_info.channels << " channels.  Max supported channels is 2." << std::endl;
        return 1;
    }

    fs::path dest_path = dest_dir / src_path.stem().string().append(".aiff");

    if (src_ext == ".aiff" && source_info.channels == 1) {
        std::cout << "Copy " << src_path << " to " << dest_path << " (no processing necessary)" << std::endl;
        sf_close(source_file);
        fs::copy_file(src_path, dest_path);
        return 0;
    }

    SF_INFO dest_info = source_info;
    dest_info.channels = 1;
    dest_info.format = SF_FORMAT_AIFF | SF_FORMAT_PCM_16;
    SNDFILE* dest_file = sf_open(dest_path.c_str(), SFM_WRITE, &dest_info);

    if (!dest_file) {
        std::cout << "Unable to open output: " << dest_path << std::endl;
        std::cout << sf_strerror(NULL);
        return 1;
    }

    std::cout << "Converting " << src_path << " to aiff" << std::endl;

    if (source_info.channels == 1) {
        convert_to_aiff(source_file, dest_file);
    }
    else {
        convert_stereo_to_mono_aiff(source_file, dest_file);
    }

    sf_close(source_file);
    sf_close(dest_file);

    return 0;
}

bool check_paths(const fs::path& src_path, const fs::path& dest_dir) {
    if (!fs::exists(src_path)) {
        std::cout << "Input " << src_path << " does not exist" << std::endl;
        return false;
    }

    if (!fs::is_regular_file(src_path)) {
        std::cout << "Input " << src_path << " is not a regular file." << std::endl;
        return false;
    }

    if (!fs::is_directory(dest_dir)) {
        std::cout << "Destination " << dest_dir << " is not a directory" << std::endl;
        return false;
    }

    return true;
}

fs::path get_dest_path(const fs::path& src_path) {
    std::string filename = src_path.stem();
    filename.append(".aiff");
    return src_path.parent_path() / filename;
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