#include <iostream>
#include <string>

#include "app.h"

// Example usage
int main(int argc, char* argv[]) {
    HTMLGUIApp app;
    
    if (!app.initialize()) {
        return -1;
    }
    
    // Sample HTML content
    std::string sampleHTML = R"(
        <html>
            <body>
                <div>
                    <label>Welcome to Micro HTML GUI!</label>
                    <div>
                        <button>Click Me</button>
                        <button>Another Button</button>
                    </div>
                    <div>
                        <label>This is a simple HTML renderer using Dear ImGui.</label>
                        <a href="https://github.com">Visit GitHub</a>
                        <a href="https://imgui.com">Dear ImGui</a>
                    </div>
                    <div>
                        <label>Supported elements:</label>
                        <button>Buttons</button>
                        Links like <a href="https://example.com">this one</a>
                        <label>Labels and text content</label>
                    </div>
                </div>
            </body>
        </html>
    )";
    
    app.loadHTML(sampleHTML);
    app.run();
    
    return 0;
}