#include <array>
#include <chrono>
#include <cstdint>
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

    AbstractShow* show = nullptr;
    AbstractShow* next_show = nullptr;
    std::stack<AbstractShow*> background_shows;
    // std::atomic<bool> backend_listening = true;
    std::mutex show_mutex;

    background_shows.push(show_buffer["background_show"]);

    // detect new show (check every 10ms), start playing it
    // send is blocking, but can be interrupted from another thread
    auto show_player = std::thread([&show, &next_show, &background_shows, &show_mutex](){
        using namespace std::chrono_literals;
        while(true) {
            {
                std::lock_guard<std::mutex> lock(show_mutex);
                if (background_shows.size() > 1) {
                    AbstractShow* background_show = background_shows.top();
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
                    AbstractShow* background_show = background_shows.top();
                    std::cout << "Return to background show " << background_show->get_codename() << " (visible to TRUE)" << std::endl;
                    show = background_show;
                    show->state.set_visible(true);
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
    show_player.detach();

    // get signals from the game and create next show
    auto signal_listener = std::thread([&show, &next_show, &background_shows, &show_mutex, &show_buffer](){
        // initialize the zmq context with a single IO thread
        zmq::context_t context{1};
        // construct a SUB socket and bind to interface
        zmq::socket_t socket{context, zmq::socket_type::sub};
        socket.set(zmq::sockopt::subscribe, "");

        std::cout << "Started listening on port 5555" << std::endl;
        socket.bind("tcp://*:5555");

        // (select|play|stop):(game_name):(sound_name)
        const std::regex frontend_sound_regex("(\\w*)\\|([a-z_]*)\\|(.*)", std::regex_constants::ECMAScript);

        TimePoint last_message_time = TimePoint();
        while (true) {
            zmq::message_t request;
            // receive a request from client
            const zmq::recv_result_t res = socket.recv(request, zmq::recv_flags::none);
            std::string message = request.to_string();

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
                << "] Message: " << message << std::endl;

            std::lock_guard<std::mutex> lock(show_mutex);
            
            if (action == "select" || action == "play") {
                if (action == "select") {
                    // clear the stack
                    std::stack<AbstractShow*>().swap(background_shows);
                }
                
                if (show && show_name == show->get_codename() && show->state.get_background_flag()) {
                    std::cout << "NEXT SHOW Keep old show playing..." << std::endl;
                } else {
                    if (show) {
                        std::cout << "Stop current show " << show->get_codename() << std::endl;
                        if (show->state.get_background_flag()) {
                            std::cout << "set background show " << show->get_codename() << " (visible to FALSE)" << std::endl;
                            show->state.set_visible(false);
                        }
                        show->stop();
                    }
                    if (next_show) {
                        std::cerr << "NEXT SHOW - OVERRIDING" << std::endl;
                    }
                    std::cout << "New sound: " << sound_name << std::endl;
                    next_show = show_buffer[show_name];
                }
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
    SetTargetFPS(60);

    while (!WindowShouldClose()){
        draw(show_name, pixels, pixels_mutex);
    }

    CloseWindow();
    return 0;
}