#include <iostream>
#include <string>

#include "show_uart_cached.hpp"


int main(int argc, char const *argv[]) 
{
    if (argc != 3) {
        std::cerr << "this program needs (device, animation.pcap) as input arguments" << std::endl;
        return 1;
    }
    
    std::string device(argv[1]);
    std::string show_file(argv[2]);
    
    ShowUartCached* show = new ShowUartCached("test", show_file, device);
    // show->state.set_background(true);
    show->send();

    return 0;
}
