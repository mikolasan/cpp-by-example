#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <zlib.h>

constexpr int BUFFER_SIZE = 128;

const char* filename_1 = "arch1.gz";
const char* filename_2 = "arch2.gz";

void printBuffers(int64_t offset, const char* buffer1, const char* buffer2, int bytesRead) {
    constexpr int bytesPerLine = 16;

    for (int i = 0; i < bytesRead; i += bytesPerLine) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << (offset + i) << ": ";

        for (int j = i; j < i + bytesPerLine; j++) {
            if (j < bytesRead) {
                std::cout << std::setw(2) << std::hex << static_cast<unsigned int>(buffer1[j] & 0xFF) << " ";
            } else {
                std::cout << "     ";
            }
        }
        std::cout << "| ";
        for (int j = i; j < i + bytesPerLine; j++) {
            if (j < bytesRead) {
                std::cout << std::setw(2) << std::hex << static_cast<unsigned int>(buffer2[j] & 0xFF) << " ";
            } else {
                std::cout << "     ";
            }
        }

        std::cout << "|";
        for (int j = i; j < i + bytesPerLine; j++) {
            if (j < bytesRead) {
                char c = buffer1[j];
                if (c < 32 || c > 126) {
                    c = '.';
                }
                std::cout << c;
            }
        }
        std::cout << "| |";
        for (int j = i; j < i + bytesPerLine; j++) {
            if (j < bytesRead) {
                char c = buffer2[j];
                if (c < 32 || c > 126) {
                    c = '.';
                }
                std::cout << c;
            }
        }
        std::cout << "|" << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    gzFile inputFile1 = gzopen(filename_1, "rb");
    if (inputFile1 == nullptr) {
        std::cerr << "Error opening input file: " << filename_1 << std::endl;
        return 1;
    }

    gzFile inputFile2 = gzopen(filename_2, "rb");
    if (inputFile2 == nullptr) {
        std::cerr << "Error opening input file: " << filename_2 << std::endl;
        return 1;
    }

    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];

    int i = 0;
    for (;;) {
    // for (; i < 10;) {
        int bytesRead1 = gzread(inputFile1, buffer1, BUFFER_SIZE);
        int bytesRead2 = gzread(inputFile2, buffer2, BUFFER_SIZE);
        if (bytesRead1 < 1 || bytesRead2 < 1 || bytesRead1 != bytesRead2) {
            std::cout << "Brr, length is not good" << std::endl;
            std::cerr << "Finished reading 1: " << gzerror(inputFile1, nullptr) << std::endl;
            std::cerr << "Finished reading 2: " << gzerror(inputFile2, nullptr) << std::endl;
            return 2;
        } else if (bytesRead1 > 0 && bytesRead2 > 0) {
            if (memcmp(buffer1, buffer2, bytesRead1) != 0) {
                std::cout << "Buffers differ:" << std::endl;
                printBuffers(i * BUFFER_SIZE, buffer1, buffer2, bytesRead1);
            }
        }

        ++i;
    }

    gzclose(inputFile1);
    gzclose(inputFile2);
    
    std::cout << "Extraction completed successfully" << std::endl;
    return 0;
}
