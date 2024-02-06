#include <iostream>
#include <fstream>
#include <zlib.h>

constexpr int BUFFER_SIZE = 128;

const char* filename_1 = "arch.gz";

int main(int argc, char const *argv[])
{
    gzFile inputFile = gzopen(filename_1, "rb");
    if (inputFile == nullptr) {
        std::cerr << "Error opening input file: " << filename_1 << std::endl;
        return 1;
    }

    std::string outputFilename = "test";
    std::ofstream outputFile(outputFilename.c_str(), std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error creating output file: " << outputFilename << std::endl;
        gzclose(inputFile);
        return 1;
    }
    
    char buffer[BUFFER_SIZE];
    int bytesRead;
    
    for (int i = 0; i < 10; ++i) {
        bytesRead = gzread(inputFile, buffer, BUFFER_SIZE);
        if (bytesRead > 0) {
           outputFile.write(buffer, bytesRead); 
        } else {
            break;
        }
    }
    // while ((bytesRead = gzread(inputFile, buffer, BUFFER_SIZE)) > 0) {
    //     outputFile.write(buffer, bytesRead);
    // }

    outputFile.close();

    if (bytesRead < 0) {
        std::cerr << "Error extracting file: " << gzerror(inputFile, nullptr) << std::endl;
        gzclose(inputFile);
        return 1;
    }

    gzclose(inputFile);
    std::cout << "Extraction completed successfully. Output file: " << outputFilename << std::endl;
    return 0;
}
