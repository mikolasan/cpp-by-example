#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <regex>
#include <stack>
#include <string>
#include <thread>

#include <zmq.hpp>

#include "show_uart_cached.hpp"
#include "show_abstract.h"

int main(int argc, char const *argv[]) 
{
    std::cout << "LED serial has started" << std::endl;

    std::string contents = get_file_contents("config.yml");
    std::map<std::string, ShowUartCached*> show_buffer = read_show_mapping<ShowUartCached>(contents);

    ShowUartCached* show = nullptr;
    ShowUartCached* next_show = nullptr;
    std::stack<ShowUartCached*> background_shows;
    std::atomic<bool> backend_listening = true;
    std::mutex show_mutex;

    // detect new show (check every 10ms), start playing it
    // send is blocking, but can be interrupted from another thread
    auto producer = std::thread([&show, &next_show, &background_shows, &show_mutex](){
        using namespace std::chrono_literals;
        while(true) {
            {
                std::lock_guard<std::mutex> lock(show_mutex);
                if (background_shows.size() > 1) {
                    ShowUartCached* background_show = background_shows.top();
                    if (background_show->state.is_force_stopped()) {
                        std::cout << "POP background show " << background_show->get_codename() << std::endl;
                        background_shows.pop();
                    }
                }
                if (next_show) {
                    show = next_show;
                    next_show = nullptr;
                    if (show->state.get_background_flag()) {
                        std::cout << "PUSH background show " << show->get_codename() << std::endl;
                        background_shows.push(show);
                    }
                } else if (!background_shows.empty()) {
                    ShowUartCached* background_show = background_shows.top();
                    std::cout << "Return to background show " << background_show->get_codename() << std::endl;
                    show = background_show;
                }
            }
            if (show) {
                show->send();
                {
                    std::lock_guard<std::mutex> lock(show_mutex);
                    show = nullptr;
                }
            } else {
                std::this_thread::sleep_for(10ms);
            }
        }
    });
    producer.detach();

    // get signals from the game and create next show
    auto consumer = std::thread([&backend_listening, &show, &next_show, &background_shows, &show_mutex, &show_buffer](){
        // initialize the zmq context with a single IO thread
        zmq::context_t context{1};
        // construct a SUB socket and bind to interface
        zmq::socket_t socket{context, zmq::socket_type::sub};
        socket.set(zmq::sockopt::subscribe, "");

        std::cout << "Started listening on port 5555" << std::endl;
        socket.bind("tcp://*:5555");

        // (select|play|stop):(game_name):(sound_name)
        const std::regex frontend_sound_regex("(\\w*)\\|([a-z_]*)\\|(.*)", std::regex_constants::ECMAScript);

        while (backend_listening) {
            zmq::message_t request;
            // receive a request from client
            const zmq::recv_result_t res = socket.recv(request, zmq::recv_flags::none);
            std::string message = request.to_string();
            std::cout << "Message: " << message << std::endl;

            std::smatch match;
            if (!std::regex_match(message, match, frontend_sound_regex)) {
                continue;
            }
            
            if (match.size() < 4) {
                continue;
            }

            std::ssub_match action_sub_match = match[1];
            std::string action = action_sub_match.str();
            
            std::ssub_match game_name_sub_match = match[2];
            std::string game_name = game_name_sub_match.str();

            std::ssub_match sound_name_sub_match = match[3];
            std::string sound_name = sound_name_sub_match.str();

            // std::cout << "Parsed [action: " << action
            //     << " | game: " << game_name
            //     << " | sound: " << sound_name 
            //     << "]" << std::endl;

            std::string show_name = game_name + "|" + sound_name;
            
            if (action == "select" || action == "play") {
                if (!show_buffer.contains(show_name)) {
                    // std::cout << "No mapping for '" << show_name << "'" << std::endl;
                    continue;
                }

                std::lock_guard<std::mutex> lock(show_mutex);
                if (action == "select") {
                    // clear the stack
                    std::stack<ShowUartCached*>().swap(background_shows);
                }
                
                if (show && show_name == show->get_codename() && show->state.get_background_flag()) {
                    std::cout << "NEXT SHOW Keep old show playing..." << std::endl;
                } else {
                    if (show) {
                        std::cout << "Stop current show " << show->get_codename() << std::endl;
                        show->stop();
                    }
                    if (next_show) {
                        std::cerr << "NEXT SHOW - OVERRIDING" << std::endl;
                    }
                    std::cout << "New sound: " << sound_name << std::endl;
                    next_show = show_buffer[show_name];
                }
            } else if (action == "stop") {
                if (!show_buffer.contains(show_name)) {
                    continue;
                }

                std::lock_guard<std::mutex> lock(show_mutex);
                if (show && show_name == show->get_codename()) {
                    std::cout << "FORCE STOP show " << show_name << std::endl;
                    show->state.force_stop();
                } else if (!background_shows.empty() && show_name == background_shows.top()->get_codename()) {
                    std::cout << "FORCE STOP background show " << show_name << std::endl;
                    background_shows.top()->state.force_stop();
                }
            } else {
                std::cout << "Unknown action " << action << std::endl;
            }
        }
    });
    consumer.join();

    return 0;
}
