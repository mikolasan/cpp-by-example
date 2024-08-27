#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "MotherboardRTC.h"
#include "SystemTime.h"
#include "TimeFactory.h"


void print_usage() {
    std::cout << "Wrong usage" << std::endl;
}

std::string convert_time(time_t time) {
    // std::cout << "UTC:   " << std::put_time(std::gmtime(&time), "%c %Z") << '\n';
    // std::cout << "local: " << std::put_time(std::localtime(&time), "%c %Z") << '\n';
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%c %Z");
    return ss.str();
}

int main(int argc, char const *argv[])
{
    std::map<std::string, AbstractTime*> loaded_time;
    for (auto i = TimeFactory::iterator_begin(); i != TimeFactory::iterator_end(); ++i) {
        auto build_function = *i;
        auto time = build_function();
        loaded_time[time->name()] = time;
    }

    if (argc == 2) {
        std::string command(argv[1]);
        if (command == "list") {
            for (const auto& [name, time] : loaded_time) {
                std::cout << name << std::endl;
            }
        } else {
            print_usage();
        }
    } else if (argc == 3) {
        std::string command(argv[1]);
        std::string arg(argv[2]);
        if (command == "get" 
            && loaded_time.count(arg) > 0) {
                time_t from_time = loaded_time[arg]->get();
                std::cout << arg << ": " << convert_time(from_time) << std::endl;
        }
    } else if (argc == 5) {
        std::string command_1(argv[1]);
        std::string from(argv[2]);
        std::string command_2(argv[3]);
        std::string to(argv[4]);
        if (command_1 == "from" 
            && loaded_time.count(from) > 0
            && command_2 == "to"
            && loaded_time.count(to) > 0
            && from != to) {
                time_t from_time = loaded_time[from]->get();
                std::cout << from << ": " << convert_time(from_time) << std::endl;
                time_t to_time = loaded_time[to]->get();
                std::cout << to << ": " << convert_time(to_time) << std::endl;
                
                loaded_time[to]->set(from_time);
        } else {
            print_usage();
        }
    } else {
        print_usage();
    }
    return 0;
}