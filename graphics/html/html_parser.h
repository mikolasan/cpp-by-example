#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <algorithm>
#include <memory>
#include <string>

#include "html_element.h"

// Simple HTML Parser
class HTMLParser {
private:
    std::string html;
    size_t pos;
    
    void skipWhitespace();
    std::string parseTagName();
    std::string parseAttribute(const std::string& attrName);
    std::string parseText();
    bool isClosingTag(const std::string& tagName);
    ElementType getElementType(const std::string& tagName);
    
public:
    HTMLParser(const std::string& htmlContent);
    
    HTMLElementPtr parse();
    HTMLElementPtr parseElement();
};

#endif