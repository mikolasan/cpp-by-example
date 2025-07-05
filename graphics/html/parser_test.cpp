#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "html_parser.h"

// Main CLI application
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <html_file> [--debug]" << std::endl;
        std::cout << "  html_file: Path to HTML file to parse" << std::endl;
        std::cout << "  --debug:   Enable debug output" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    bool debug = (argc > 2 && std::string(argv[2]) == "--debug");
    
    // Read HTML file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string htmlContent = buffer.str();
    
    std::cout << "=== HTML Parser CLI Debug Tool ===" << std::endl;
    std::cout << "File: " << filename << std::endl;
    std::cout << "Size: " << htmlContent.length() << " bytes" << std::endl;
    std::cout << "Debug: " << (debug ? "ON" : "OFF") << std::endl;
    std::cout << std::endl;
    
    if (debug) {
        std::cout << "=== RAW HTML (first 500 chars) ===" << std::endl;
        std::cout << htmlContent.substr(0, 500);
        if (htmlContent.length() > 500) {
            std::cout << "... (truncated)";
        }
        std::cout << std::endl << std::endl;
    }
    
    // Parse HTML
    HTMLParser parser(htmlContent, debug);
    auto root = parser.parse();
    
    std::cout << "=== PARSED TREE ===" << std::endl;
    if (root) {
        parser.printTree(root);
    } else {
        std::cout << "No elements parsed!" << std::endl;
    }
    
    return 0;
}