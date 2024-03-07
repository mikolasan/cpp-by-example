#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "raylib.h"

const int n_leds_per_universe = 170;
const int max_leds_per_row = 50;
const int led_size = 10;
std::map<int, std::vector<Color>> pixels;
std::string show_name;

void update() {}

void draw() {
    BeginDrawing();

    ClearBackground(BLACK);
    DrawText(show_name.c_str(), 20, 20, 20, LIGHTGRAY);

    int start_x = 10;
    int start_y = 100;
    int current_y = start_y;

    for (auto& [universe_id, colors] : pixels) {
        for (int i = 0; i < colors.size(); ++i) {
            int x = start_x + (i % max_leds_per_row) * led_size;
            int y = current_y + (i / max_leds_per_row) * led_size;
            DrawRectangle(x, y, led_size, led_size, colors[i]);
        }
        current_y += (1 + colors.size() / max_leds_per_row) * led_size;
    }

    EndDrawing();
}

int main(int argc, char const *argv[])
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Art Net viz");
    SetTargetFPS(60);

    show_name = "music of happy pigs";
    pixels[0] = std::vector<Color>(n_leds_per_universe, RED);
    pixels[1] = std::vector<Color>(n_leds_per_universe, ORANGE);
    pixels[2] = std::vector<Color>(n_leds_per_universe, YELLOW);
    pixels[3] = std::vector<Color>(n_leds_per_universe, WHITE);

    while (!WindowShouldClose()){
        update();
        draw();
    }

    CloseWindow();
    return 0;
}