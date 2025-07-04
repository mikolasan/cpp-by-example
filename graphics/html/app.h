#ifndef APP_H
#define APP_H

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include "html_element.h"
#include "html_renderer.h"

// Main Application
class HTMLGUIApp {
private:
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool running;
    HTMLRenderer htmlRenderer;
    HTMLElementPtr rootElement;
    
public:
    HTMLGUIApp();
    ~HTMLGUIApp();

    bool initialize();
    void loadHTML(const std::string& htmlContent);
    void run();
    
private:
    void handleEvents();
    void render();
    void cleanup();
};

#endif