#ifndef HTML_GUI_APP_H
#define HTML_GUI_APP_H

#include <chrono>
#include <thread>
#include <future>

#include <curl/curl.h>
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

#undef ERROR

enum class RequestState {
    IDLE,
    SENDING,
    WAITING,
    COMPLETED,
    ERROR
};

struct HTTPResponse {
    std::string content;
    long statusCode;
    std::string errorMessage;
    double requestTime;
};

class HTMLGUIApp {
private:
    SDL_Window* window;
    SDL_GLContext glContext;
    bool running;
    HTMLRenderer htmlRenderer;
    HTMLElementPtr rootElement;
    std::string errorMessage;
    
    // HTTP request state
    RequestState requestState;
    std::string addressBarText;
    std::future<HTTPResponse> httpFuture;
    std::chrono::steady_clock::time_point requestStartTime;
    HTTPResponse lastResponse;
    
    // UI state
    bool showStatusBar;
    float statusBarHeight;
    
    // HTTP client methods
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    HTTPResponse performHTTPRequest(const std::string& url);
    void startHTTPRequest(const std::string& url);
    void checkHTTPRequest();
    
    // UI rendering methods
    void renderAddressBar();
    void renderStatusBar();
    void renderContent();
    ImVec4 getStatusCodeColor(long statusCode);
    
    // Event handling
    void handleEvents();
    void render();
    void cleanup();
    
public:
    HTMLGUIApp();
    ~HTMLGUIApp();
    
    bool initialize();
    void loadHTML(const std::string& htmlContent);
    void run();
};

#endif