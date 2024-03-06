#include <array>
#include <cstdint>
#include <map>

#include "raylib.h"


struct Block {
    enum struct Fade {
        IN,
        OUT
    };
    Block(int _size, Color _color, float _alpha, float _fade_max) :
        size(_size),
        color(_color),
        alpha(_alpha),
        fade(Fade::IN),
        fade_max(_fade_max)
    {}
    int size;
    Color color;
    float alpha;
    Fade fade;
    float fade_max;
};

const int num_leds = 50;
const int led_size = 15;
std::map<int, Block> blocks;

void update() {}

void draw() {
    BeginDrawing();

    ClearBackground(DARKGRAY);
    DrawText("test moving squares like in music light box", 20, 20, 20, LIGHTGRAY);

    int x = 10;
    int y = 100;
    const float delta = 0.001;
    
    for (auto& [start_position, block] : blocks) {
        for (int i = 0; i < block.size; ++i) {
            float alpha = block.alpha;
            if (block.fade == Block::Fade::IN) {
                alpha += delta;
                if (alpha > block.fade_max) {
                    alpha = block.fade_max;
                    block.fade = Block::Fade::OUT;
                }
            } else {
                alpha -= delta;
                if (alpha < 0.0) {
                    alpha = 0.0;
                    block.fade = Block::Fade::IN;
                }
            }
            block.alpha = alpha;
            DrawRectangle(x + (start_position + i) * led_size, y, led_size, led_size, ColorAlpha(block.color, alpha));
        }
    }

    EndDrawing();
}

int main(int argc, char const *argv[])
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Light Squares Test");
    SetTargetFPS(60);

    int j = 0;
    while (j < num_leds) {
        int block_size = GetRandomValue(1, 4);
        if (j + block_size > num_leds) {
            block_size = num_leds - j;
        }
        Color base_color = GOLD;
        float alpha = GetRandomValue(0, 100) / 100.;
        float fade_max = GetRandomValue(20, 100) / 100.;
        blocks.emplace(j, Block(block_size, ColorAlpha(base_color, alpha), alpha, fade_max));
        j += block_size;
    }

    while (!WindowShouldClose()){
        update();
        draw();
    }

    CloseWindow();
    return 0;
}