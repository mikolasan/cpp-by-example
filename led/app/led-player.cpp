#include <iostream>
#include <string>

#include "show_udp_cached.hpp"

using Show = ShowUdpCached;

int main(int argc, char const *argv[]) 
{
    if (argc != 3) {
        std::cerr << "this program needs (hostname, animation.pcap) as input arguments" << std::endl;
        return 1;
    }
    
    std::string hostname(argv[1]);
    std::string show_file(argv[2]);
    
    Show* show = new Show("loop test", show_file);
    show->hostname = hostname;
    show->state.set_loop(true);
    show->send();

    return 0;
}
