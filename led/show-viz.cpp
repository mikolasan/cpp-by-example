#include <array>
#include <chrono>
#include <cstdint>
#include <future>
#include <map>
#include <mutex>
#include <regex>
#include <stack>
#include <string>
#include <thread>
#include <vector>

#include <raylib.h>
#include <ryml_std.hpp>
#include <ryml.hpp>
#include <zmq.hpp>

#include "show.h"
#include "show_test.hpp"
#include "config.hpp"

using namespace std::chrono_literals;

const int n_leds_per_universe = 170;
const int max_leds_per_row = 50;
const int led_size = 10;

void draw(const std::string& show_name, const std::map<int, std::vector<Color>>& pixels, std::mutex& pixels_mutex) {
    BeginDrawing();

    ClearBackground(BLACK);
    DrawText(show_name.c_str(), 20, 20, 20, LIGHTGRAY);

    int start_x = 10;
    int start_y = 100;
    int current_y = start_y;

    {
        std::lock_guard<std::mutex> lock(pixels_mutex);
        for (auto& [universe_id, colors] : pixels) {
            for (int i = 0; i < colors.size(); ++i) {
                int x = start_x + (i % max_leds_per_row) * led_size;
                int y = current_y + (i / max_leds_per_row) * led_size;
                DrawRectangle(x, y, led_size, led_size, colors[i]);
            }
            current_y += (2 + colors.size() / max_leds_per_row) * led_size;
        }
    }

    EndDrawing();
}

// (select|play|stop):(game_name):(sound_name)
const std::regex message_regex("(\\w*)\\|([a-z_]*)\\|(.*)", std::regex_constants::ECMAScript);

template <typename R>
bool is_future_ready(std::future<R> const& f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

// (action, show_name)
using ParsedMessage = std::tuple<std::string, std::string>;
ParsedMessage receive_and_parse(zmq::socket_t& socket) {
    zmq::message_t request;
    // receive a request from client
    const zmq::recv_result_t res = socket.recv(request, zmq::recv_flags::none);
    std::string message = request.to_string();

    std::smatch match;
    if (!std::regex_match(message, match, message_regex)) {
        return {"", ""};
    }
    
    if (match.size() < 4) {
        return {"", ""};
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
    return {action, show_name};
}

int main(int argc, char const *argv[])
{
    std::string contents = get_file_contents("config.yml");
    std::map<std::string, TestShow*> show_buffer = read_show_mapping<TestShow>(contents);

    std::string show_file{"test.pcapng"};
    std::string show_name = "music of happy pigs";

    std::mutex pixels_mutex;
    std::map<int, std::vector<Color>> pixels;
    pixels[0] = std::vector<Color>(n_leds_per_universe, RED);
    pixels[1] = std::vector<Color>(n_leds_per_universe, ORANGE);
    pixels[2] = std::vector<Color>(n_leds_per_universe, YELLOW);
    pixels[3] = std::vector<Color>(n_leds_per_universe, WHITE);

    for (const auto&[show_name, show] : show_buffer) {
        show->pixels = &pixels;
        show->pixels_mutex = &pixels_mutex;
    }

    std::stack<AbstractShow*> background_shows;
    // std::atomic<bool> backend_listening = true;
    // std::mutex show_mutex;

    background_shows.push(show_buffer["background_show"]);

    // get signals from the game and create next show
    auto signal_listener = std::thread([&background_shows, &show_buffer](){
        // initialize the zmq context with a single IO thread
        zmq::context_t context{1};
        // construct a SUB socket and bind to interface
        zmq::socket_t socket{context, zmq::socket_type::sub};
        socket.set(zmq::sockopt::subscribe, "");

        std::cout << "Started listening on port 5555" << std::endl;
        socket.bind("tcp://*:5555");

        AbstractShow* show = nullptr;
        // AbstractShow* next_show = nullptr;
        TimePoint last_message_time = TimePoint();
        
        std::future<ParsedMessage> result;
        while (true) {
            if (!result.valid()) {
                result = \
                    std::async(std::launch::async, receive_and_parse, std::ref(socket));
                continue;
            }
            if (!is_future_ready(result)) {
                continue;
            }
            
            // std::packaged_task<ParsedMessage(zmq::socket_t&)> task(receive_and_parse);
            // const auto [action, show_name] = receive_and_parse(socket);
            // std::future<ParsedMessage> result = task.get_future();
            // block
            // task(socket);
            // std::thread task_td(std::move(task), std::ref(socket));
            // task_td.join();

            const auto [action, show_name] = result.get();

            if (!show_buffer.contains(show_name)) {
                // std::cout << "No mapping for '" << show_name << "'" << std::endl;
                continue;
            }

            TimePoint message_time = Clock::now();
            uint64_t time_past = std::chrono::duration_cast<std::chrono::milliseconds>(
                    message_time - last_message_time).count();
            last_message_time = message_time;
            std::cout << "[" 
                << time_past
                << "] Action: " << action
                << " show " << show_name
                << std::endl;

            // std::lock_guard<std::mutex> lock(show_mutex);
            
            if (action == "select" || action == "play") {
                // if (action == "select") {
                //     // clear the stack
                //     std::stack<AbstractShow*>().swap(background_shows);
                // }
                
                // background music repeats the "start" command
                if (show 
                        && show_name == show->get_codename()
                        && show->state.get_background_flag()) {
                    std::cout << "[STRANGE] NEXT SHOW Keep old show playing..." << std::endl;
                    continue;
                }
                
                if (show) {
                    std::cout << "Stop current show " << show->get_codename() << std::endl;
                    if (show->state.get_background_flag()) {
                        std::cout << "set background show " 
                            << show->get_codename() << " (visible to FALSE)" 
                            << std::endl;
                        show->state.set_visible(false);
                    }
                    show->stop();
                }
                
                std::cout << "Next show: " << show_name << std::endl;
                show = show_buffer[show_name];
                if (show->state.get_background_flag()) {
                    std::cout << "PUSH background show " << show->get_codename() << std::endl;
                    background_shows.push(show);
                }
                show->send();
            } else if (action == "stop") {
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
    signal_listener.detach();

    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Art Net viz");
    SetTargetFPS(30);

    while (!WindowShouldClose()){
        draw(show_name, pixels, pixels_mutex);
    }

    CloseWindow();
    return 0;
}