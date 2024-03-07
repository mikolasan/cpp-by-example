#include <array>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <raylib.h>
#include <ryml_std.hpp>
#include <ryml.hpp>
#include <zmq.hpp>

#include "show.h"
#include "show_test.hpp"

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

    std::lock_guard<std::mutex> lock(pixels_mutex);
    for (auto& [universe_id, colors] : pixels) {
        for (int i = 0; i < colors.size(); ++i) {
            int x = start_x + (i % max_leds_per_row) * led_size;
            int y = current_y + (i / max_leds_per_row) * led_size;
            DrawRectangle(x, y, led_size, led_size, colors[i]);
        }
        current_y += (2 + colors.size() / max_leds_per_row) * led_size;
    }

    EndDrawing();
}

int main(int argc, char const *argv[])
{
    std::string show_file{"test.pcapng"};
    std::string show_name = "music of happy pigs";

    std::mutex pixels_mutex;
    std::map<int, std::vector<Color>> pixels;
    pixels[0] = std::vector<Color>(n_leds_per_universe, RED);
    pixels[1] = std::vector<Color>(n_leds_per_universe, ORANGE);
    pixels[2] = std::vector<Color>(n_leds_per_universe, YELLOW);
    pixels[3] = std::vector<Color>(n_leds_per_universe, WHITE);

    ShowTest* show = new ShowTest(show_name, show_file, &pixels, &pixels_mutex);
    show->send();

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