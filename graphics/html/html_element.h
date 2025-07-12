#ifndef HTML_ELEMENT_H
#define HTML_ELEMENT_H

#include <list>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
struct HTMLElement;

using HTMLElementPtr = std::shared_ptr<HTMLElement>;

using DOM = std::list<HTMLElement>;

// HTML Element types
enum class ElementType {
    HTML,
    BODY,
    DIV,
    LABEL,
    BUTTON,
    ANCHOR,
    TEXT,
    UNKNOWN
};

// HTML Element structure
struct HTMLElement {
    ElementType type;
    std::string tag;
    std::string text;
    std::string href; // for anchor tags
    std::vector<HTMLElementPtr> children;
    
    HTMLElement(ElementType t, const std::string& tag_name = "") 
        : type(t), tag(tag_name) {}
};

#endif