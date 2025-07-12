#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include "html_element.h"

class HTMLParser;

enum class ParserState {
    DataState,
    TagOpenState,
};

class StateParser {
public:
    virtual void parse(HTMLParser& parser) = 0;
};

class DataStateParser : public StateParser {
public:
    void parse(HTMLParser& parser);
};

class TagOpenStateParser : public StateParser {
public:
    void parse(HTMLParser& parser);
};

// Simple HTML Parser
class HTMLParser {
private:
    std::istringstream input_stream;
    DOM dom;
    ParserState parser_state;
    StateParser& current_parser;
    std::string html;
    size_t pos;
    int depth;
    bool debug;
    
    friend class DataStateParser;
    friend class TagOpenStateParser;

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
    
    DOM parse();
    // HTMLElementPtr parse();
    HTMLElementPtr parseElement();
    void printTree(const DOM& element, int indentLevel = 0);
};

#endif