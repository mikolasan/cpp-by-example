#include <algorithm>
#include <memory>
#include <string>

#include "html_parser.h"

HTMLParser::HTMLParser(const std::string& htmlContent) : 
    html(htmlContent), pos(0)
{}

HTMLElementPtr HTMLParser::parse() {
    return parseElement();
}

HTMLElementPtr HTMLParser::parseElement() {
    skipWhitespace();

    if (pos >= html.length()) return nullptr;

    if (html[pos] != '<') {
        // Parse text content
        std::string text = parseText();
        if (!text.empty()) {
            auto textElement = std::make_shared<HTMLElement>(ElementType::TEXT);
            textElement->text = text;
            return textElement;
        }
        return nullptr;
    }

    std::string tagName = parseTagName();
    if (tagName.empty()) return nullptr;

    // Handle self-closing or void elements
    if (tagName.back() == '/') {
        tagName.pop_back();
    }

    auto element = std::make_shared<HTMLElement>(getElementType(tagName), tagName);

    // Parse href attribute for anchor tags
    if (element->type == ElementType::ANCHOR) {
        element->href = parseAttribute("href");
    }

    // Parse children until closing tag
    while (pos < html.length() && !isClosingTag(tagName)) {
        auto child = parseElement();
        if (child) {
            element->children.push_back(child);
        }
    }

    // Skip closing tag
    if (isClosingTag(tagName)) {
        while (pos < html.length() && html[pos] != '>') {
            pos++;
        }
        if (pos < html.length()) pos++; // skip '>'
    }

    return element;
}

void HTMLParser::skipWhitespace() {
    while (pos < html.length() && std::isspace(html[pos])) {
        pos++;
    }
}

std::string HTMLParser::parseTagName() {
    skipWhitespace();
    if (pos >= html.length() || html[pos] != '<') return "";

    pos++; // skip '<'
    size_t start = pos;

    while (pos < html.length() && html[pos] != '>' && html[pos] != ' ') {
        pos++;
    }

    std::string tagName = html.substr(start, pos - start);

    // Skip to end of tag
    while (pos < html.length() && html[pos] != '>') {
        pos++;
    }
    if (pos < html.length()) pos++; // skip '>'

    return tagName;
}

std::string HTMLParser::parseAttribute(const std::string& attrName) {
    size_t savedPos = pos;
    pos -= 1; // Go back to before '>'

    // Find the attribute in the current tag
    while (pos > 0 && html[pos] != '<') {
        pos--;
    }

    std::string tagContent = html.substr(pos, savedPos - pos);
    size_t attrPos = tagContent.find(attrName + "=");

    std::string result;
    if (attrPos != std::string::npos) {
        attrPos += attrName.length() + 2; // Skip 'attr="'
        size_t endPos = tagContent.find('"', attrPos);
        if (endPos != std::string::npos) {
            result = tagContent.substr(attrPos, endPos - attrPos);
        }
    }

    pos = savedPos; // Restore position
    return result;
}

std::string HTMLParser::parseText() {
    skipWhitespace();
    size_t start = pos;

    while (pos < html.length() && html[pos] != '<') {
        pos++;
    }

    std::string text = html.substr(start, pos - start);

    // Trim whitespace
    text.erase(0, text.find_first_not_of(" \t\n\r"));
    text.erase(text.find_last_not_of(" \t\n\r") + 1);

    return text;
}

bool HTMLParser::isClosingTag(const std::string& tagName) {
    skipWhitespace();
    if (pos >= html.length() || html[pos] != '<') return false;

    size_t checkPos = pos + 1;
    if (checkPos >= html.length() || html[checkPos] != '/') return false;

    checkPos++;
    std::string closingTag = html.substr(checkPos, tagName.length());

    return closingTag == tagName;
}

ElementType HTMLParser::getElementType(const std::string& tagName) {
    std::string lower = tagName;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "html") return ElementType::HTML;
    if (lower == "body") return ElementType::BODY;
    if (lower == "div") return ElementType::DIV;
    if (lower == "label") return ElementType::LABEL;
    if (lower == "button") return ElementType::BUTTON;
    if (lower == "a") return ElementType::ANCHOR;

    return ElementType::DIV; // default
}
