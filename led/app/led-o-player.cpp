#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>

#include <ryml_std.hpp>
#include <ryml.hpp>
#include <zmq.hpp>

#include "show.h"
#include "show_abstract.hpp"

int main(int argc, char const *argv[]) 
{
    if (argc != 2) {
        std::cerr << "this program needs a (hostname) as its input argument" << std::endl;
        return 1;
    }
    
    std::string hostname(argv[1]);
    
    std::string contents = get_file_contents("config.yml");
    ryml::Tree tree = ryml::parse(ryml::to_csubstr(contents));
    ryml::NodeRef colors = tree["o_colors"];
    std::map<std::string, std::tuple<int, int, int>> color_map;

    for (ryml::NodeRef const& child : colors.children()) {
        std::string color;
        ryml::from_chars(child.key(), &color);
        
        int red, green, blue;
        if (child.is_seq() && child.num_children() == 3) {
            child[0] >> red;
            child[1] >> green;
            child[2] >> blue;
            std::cout << "color: " << color
                << " values: " 
                << red << " " 
                << green << " "
                << blue << std::endl;
        }
        color_map[color] = std::make_tuple(red, green, blue);
    }


    Show* show = nullptr;
    Show* next_show = nullptr;
    std::atomic<bool> backend_listening = true;
    std::mutex show_mutex;

    // detect new show (check every 10ms), start playing it
    // send is blocking, but can be interrupted from another thread
    auto producer = std::thread([&show, &next_show, &show_mutex, &hostname](){
        using namespace std::chrono_literals;
        while(true) {
            {
                std::lock_guard<std::mutex> lock(show_mutex);
                if (next_show) {
                    show = next_show;
                    next_show = nullptr;
                }
            }
            if (show) {
                std::cout << "start show '" << show->get_codename() << "'" << std::endl;
                show->send_to(hostname);
                {
                    std::lock_guard<std::mutex> lock(show_mutex);
                    std::cout << "delete show '" << show->get_codename() << "'" << std::endl;
                    delete show;
                    show = nullptr;
                }
            } else {
                std::this_thread::sleep_for(50ms);
            }
        }
    });
    producer.detach();

    // get signals from the game and create next show
    auto consumer = std::thread([&backend_listening, &show, &next_show, &show_mutex, &color_map](){
        // initialize the zmq context with a single IO thread
        zmq::context_t context{1};
        // construct a SUB socket and bind to interface
        zmq::socket_t socket{context, zmq::socket_type::sub};
        socket.set(zmq::sockopt::subscribe, "");
        socket.bind("tcp://*:5555");
        
        while (backend_listening) {
            zmq::message_t request;
            // receive a request from client
            const zmq::recv_result_t res = socket.recv(request, zmq::recv_flags::none);
            std::string message = request.to_string();
            std::cout << "Message: " << message << std::endl;

            // candle:(top|main|bottom):(red|green|blue|white|yellow|magenta|cyan):(constant|fast|slow)
            const std::regex candle_regex("candle:(\\w+):(\\w+):(\\w+)");
            std::smatch match;
            if (!std::regex_match(message, match, candle_regex)) {
                continue;
            }
            
            if (match.size() < 4) {
                continue;
            }
            
            std::ssub_match section_sub_match = match[1];
            std::string section = section_sub_match.str();
            
            std::ssub_match color_sub_match = match[2];
            std::string color = color_sub_match.str();

            std::ssub_match frequency_sub_match = match[3];
            std::string frequency = frequency_sub_match.str();
            
            if (!color_map.contains(color)) {
                std::cout << "No color" << std::endl;
                continue;
            }

            if (section != "top") {
                continue;
            }
            
            std::cout << "Parsed [section: " << section
                << " | color: " << color
                << " | frequency: " << frequency 
                << "]" << std::endl;
            
            std::lock_guard<std::mutex> lock(show_mutex);
            if (show && message == show->get_codename()) {
                std::cout << "Keep the same show playing..." << std::endl;
            } else {
                if (show) {
                    show->stop();
                }
                if (next_show) {
                    std::cerr << "OVERRIDING next show" << std::endl;
                    delete next_show;
                }
                std::cout << "next show '" << message << "'" << std::endl;
                next_show = new Show(message, section, color_map[color], frequency);
            }
        }
    });
    consumer.join();

    return 0;
}

