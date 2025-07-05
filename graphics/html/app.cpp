#include <iomanip>
#include <iostream>
#include <sstream>

#include "app.h"
#include "html_parser.h"

HTMLGUIApp::HTMLGUIApp()
    : window(nullptr), glContext(nullptr), running(false)
    , requestState(RequestState::IDLE), showStatusBar(true), statusBarHeight(60.0f) {

    // Initialize address bar with default URL
    addressBarText.resize(256);
    strcpy(addressBarText.data(), "https://google.com");

    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HTMLGUIApp::~HTMLGUIApp() {
    cleanup();
    curl_global_cleanup();
}

bool HTMLGUIApp::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("Micro HTML GUI Renderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        window_flags);

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void HTMLGUIApp::loadHTML(const std::string& htmlContent) {
    errorMessage = "";
    try {
        HTMLParser parser(htmlContent);
        rootElement = parser.parse();
    } catch (const std::exception& e) {
        errorMessage = e.what();
        rootElement = nullptr;
    }
}

void HTMLGUIApp::run() {
    running = true;

    while (running) {
        handleEvents();
        checkHTTPRequest();
        render();
    }
}

void HTMLGUIApp::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
}

void HTMLGUIApp::render() {
    // Start Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Get window size
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Set up main window to fill entire screen
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

    ImGui::Begin("Browser", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    // Render address bar
    renderAddressBar();

    // Render status bar if needed
    if (showStatusBar) {
        renderStatusBar();
    }

    // Render main content
    renderContent();

    ImGui::End();

    // Render Dear ImGui
    ImGui::Render();
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

void HTMLGUIApp::renderAddressBar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));

    // Address bar with Go button
    ImGui::Text("Address:");
    ImGui::SameLine();

    float buttonWidth = 80.0f;
    float availableWidth = ImGui::GetContentRegionAvail().x - buttonWidth - ImGui::GetStyle().ItemSpacing.x;

    ImGui::PushItemWidth(availableWidth);
    bool enterPressed = ImGui::InputText("##address",
        addressBarText.data(),
        addressBarText.size(),
        ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();

    ImGui::SameLine();
    bool goButtonPressed = ImGui::Button("Go", ImVec2(buttonWidth, 0));

    // Handle Go button or Enter key
    if ((goButtonPressed || enterPressed) && requestState == RequestState::IDLE) {
        std::string url = addressBarText.data();
        if (!url.empty()) {
            startHTTPRequest(url);
        }
    }

    ImGui::PopStyleVar();
    ImGui::Separator();
}

void HTMLGUIApp::renderStatusBar() {
    ImGui::BeginChild("StatusBar", ImVec2(0, statusBarHeight), true);

    switch (requestState) {
    case RequestState::IDLE:
        ImGui::Text("Ready");
        break;

    case RequestState::SENDING:
        ImGui::Text("Sending request...");
        break;

    case RequestState::WAITING: {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - requestStartTime);
        ImGui::Text("Waiting for response... (%.1fs)", elapsed.count() / 1000.0f);

        // Show progress indicator
        ImGui::SameLine();
        const char* spinner = "|/-\\";
        static int spinnerIndex = 0;
        ImGui::Text("%c", spinner[spinnerIndex]);
        spinnerIndex = (spinnerIndex + 1) % 4;
        break;
    }

    case RequestState::COMPLETED: {
        // Show status code with color
        ImVec4 statusColor = getStatusCodeColor(lastResponse.statusCode);
        ImGui::TextColored(statusColor, "Status: %ld", lastResponse.statusCode);

        ImGui::SameLine();
        ImGui::Text("| Time: %.2fs", lastResponse.requestTime);

        ImGui::SameLine();
        ImGui::Text("| Size: %zu bytes", lastResponse.content.size());

        break;
    }

    case RequestState::ERROR:
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: %s", lastResponse.errorMessage.c_str());
        break;
    }

    ImGui::EndChild();
}

void HTMLGUIApp::renderContent() {
    ImGui::BeginChild("Content", ImVec2(0, 0), false);

    if (rootElement) {
        htmlRenderer.render(rootElement);
    }
    else if (!errorMessage.empty()) {
        ImGui::TextWrapped(errorMessage.data());
    }
    else {
        ImGui::TextWrapped("No content loaded. Enter a URL in the address bar and click Go to fetch a web page.");
    }

    ImGui::EndChild();
}

ImVec4 HTMLGUIApp::getStatusCodeColor(long statusCode) {
    if (statusCode >= 200 && statusCode < 300) {
        return ImVec4(0.2f, 0.8f, 0.2f, 1.0f); // Green for success
    }
    else if (statusCode >= 300 && statusCode < 400) {
        return ImVec4(0.8f, 0.6f, 0.2f, 1.0f); // Orange for redirects
    }
    else if (statusCode >= 400 && statusCode < 500) {
        return ImVec4(0.8f, 0.4f, 0.2f, 1.0f); // Red-orange for client errors
    }
    else if (statusCode >= 500) {
        return ImVec4(0.8f, 0.2f, 0.2f, 1.0f); // Red for server errors
    }
    else {
        return ImVec4(0.6f, 0.6f, 0.6f, 1.0f); // Gray for unknown
    }
}

size_t HTMLGUIApp::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

HTTPResponse HTMLGUIApp::performHTTPRequest(const std::string& url) {
    HTTPResponse response;
    CURL* curl;
    CURLcode res;

    auto startTime = std::chrono::steady_clock::now();

    curl = curl_easy_init();
    if (curl) {
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set callback function to write response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.content);

        // Follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Set timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

        // Set User-Agent
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "MicroHTMLGUI/1.0");

        // Perform the request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            // Get response code
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);
        }
        else {
            response.errorMessage = curl_easy_strerror(res);
            response.statusCode = 0;
        }

        curl_easy_cleanup(curl);
    }
    else {
        response.errorMessage = "Failed to initialize CURL";
        response.statusCode = 0;
    }

    auto endTime = std::chrono::steady_clock::now();
    response.requestTime = std::chrono::duration<double>(endTime - startTime).count();

    return response;
}

void HTMLGUIApp::startHTTPRequest(const std::string& url) {
    requestState = RequestState::SENDING;
    requestStartTime = std::chrono::steady_clock::now();

    // Start async HTTP request
    httpFuture = std::async(std::launch::async, [this, url]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Brief delay to show "sending" state
        return performHTTPRequest(url);
        });

    requestState = RequestState::WAITING;
}

void HTMLGUIApp::checkHTTPRequest() {
    if (requestState == RequestState::WAITING && httpFuture.valid()) {
        if (httpFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            lastResponse = httpFuture.get();

            if (lastResponse.statusCode == 0) {
                requestState = RequestState::ERROR;
            }
            else {
                requestState = RequestState::COMPLETED;

                // Parse and load the HTML content
                if (!lastResponse.content.empty()) {
                    loadHTML(lastResponse.content);
                }
            }
        }
    }
}

void HTMLGUIApp::cleanup() {
    // Wait for any pending HTTP request
    if (httpFuture.valid()) {
        httpFuture.wait();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}