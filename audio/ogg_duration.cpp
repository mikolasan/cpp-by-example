
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "stb_vorbis.h"

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        std::cerr << "this program needs an ogg filename as its input argument" << std::endl;
        return 1;
    }
    std::string filename(argv[1]);

    // Read the OGG file into memory
    // FILE* ogg_file = fopen(filename.c_str(), "rb");
    // if (!ogg_file) {
    //     std::cerr << "Failed to open OGG file\n";
    //     return 1;
    // }
    // fseek(ogg_file, 0, SEEK_END);
    // long ogg_size = ftell(ogg_file);
    // fseek(ogg_file, 0, SEEK_SET);
    // std::vector<unsigned char> ogg_data(ogg_size);
    // fread(&ogg_data[0], 1, ogg_size, ogg_file);
    // fclose(ogg_file);
    
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "could not open " << filename << std::endl;
        return 1;
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    std::string ogg_data = contents.str();

    // Initialize the Vorbis decoder with the in-memory data
    int error;
    stb_vorbis* vorbis = stb_vorbis_open_memory(
        reinterpret_cast<const unsigned char*>(ogg_data.data()),
        ogg_data.size(),
        &error,
        NULL);

    if (!vorbis) {
        std::cerr << "Failed to open OGG file with stb_vorbis\n";
        return 1;
    }

    // Get the total length of the audio in seconds
    double total_seconds = stb_vorbis_stream_length_in_seconds(vorbis);

    std::cout << "Duration of the audio: " << total_seconds << " seconds\n";

    // Clean up
    stb_vorbis_close(vorbis);

    return 0;
}

