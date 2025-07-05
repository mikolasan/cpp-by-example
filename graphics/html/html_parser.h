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
    int depth;
    bool debug;
    
    void skipWhitespace();
    void skipComment();
    void skipDoctype();
    void debugLog(const std::string& msg);
    std::string parseTagName();
    std::string parseAttribute(const std::string& attrName);
    std::string parseText();
    bool isClosingTag(const std::string& tagName);
    ElementType getElementType(const std::string& tagName);
    std::string getElementTypeName(ElementType type);
    
public:
    HTMLParser(const std::string& htmlContent, bool enableDebug = false);
    
    HTMLElementPtr parse();
    HTMLElementPtr parseElement();
    void printTree(const HTMLElementPtr& element, int indentLevel = 0);
};

#endif