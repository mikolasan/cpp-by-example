#ifndef HTML_RENDERER_H
#define HTML_RENDERER_H

#include <string>

#include "html_element.h"


// HTML Renderer using Dear ImGui
class HTMLRenderer {
private:
    int buttonId = 0;
    
public:
    void render(const HTMLElementPtr& element);
    
private:
    void renderChildren(const HTMLElementPtr& element);
    std::string getTextContent(const HTMLElementPtr& element);
};

#endif