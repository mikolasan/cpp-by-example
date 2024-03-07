#include <iostream>
#include <string>

#include "show_simple.hpp"


int main(int argc, char const *argv[]) 
{
    if (argc != 3) {
        std::cerr << "this program needs (hostname, animation.pcap) as input arguments" << std::endl;
        return 1;
    }
    
    std::string hostname(argv[1]);
    std::string show_file(argv[2]);
    
    Show* show = new Show("loop", show_file, hostname);
    show->cache();
    show->set_background(true);
    show->send_cached();

    return 0;
}
