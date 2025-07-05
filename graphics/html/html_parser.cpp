#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

#include "html_parser.h"

HTMLParser::HTMLParser(const std::string& htmlContent, bool enableDebug) :
    html(htmlContent), pos(0), depth(0), debug(enableDebug)
{
}

HTMLElementPtr HTMLParser::parse() {
    debugLog("Starting parse...");
    return parseElement();
}

HTMLElementPtr HTMLParser::parseElement() {
    depth++;
    skipWhitespace();

    if (pos >= html.length()) {
        depth--;
        return nullptr;
    }

    if (html[pos] != '<') {
        // Parse text content
        std::string text = parseText();
        if (!text.empty()) {
            auto textElement = std::make_shared<HTMLElement>(ElementType::TEXT);
            textElement->text = text;
            depth--;
            return textElement;
        }
        depth--;
        return nullptr;
    }

    std::string tagName = parseTagName();
    if (tagName.empty()) {
        depth--;
        return nullptr;
    }

    // Handle closing tags
    if (tagName[0] == '/') {
        depth--;
        return nullptr;
    }

    // Handle self-closing tags
    if (tagName.back() == '/') {
        tagName.pop_back();
    }

    auto element = std::make_shared<HTMLElement>(getElementType(tagName), tagName);
    debugLog("Created element: " + tagName + " (type: " + getElementTypeName(element->type) + ")");

    // Parse href attribute for anchor tags
    if (element->type == ElementType::ANCHOR) {
        element->href = parseAttribute("href");
        if (!element->href.empty()) {
            debugLog("Found href: " + element->href);
        }
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
        debugLog("Skipping closing tag for: " + tagName);
        while (pos < html.length() && html[pos] != '>') {
            pos++;
        }
        if (pos < html.length()) pos++; // skip '>'
    }

    depth--;
    return element;
}

void HTMLParser::printTree(const HTMLElementPtr& element, int indentLevel) {
    if (!element) return;

    std::string indent(indentLevel * 2, ' ');

    if (element->type == ElementType::TEXT) {
        std::cout << indent << "[TEXT] \"" << element->text << "\"" << std::endl;
    }
    else {
        std::cout << indent << "[" << getElementTypeName(element->type) << "] ";
        if (!element->tag.empty()) {
            std::cout << "<" << element->tag << ">";
        }
        if (!element->href.empty()) {
            std::cout << " href=\"" << element->href << "\"";
        }
        std::cout << std::endl;

        for (const auto& child : element->children) {
            printTree(child, indentLevel + 1);
        }
    }
}

void HTMLParser::skipWhitespace() {
    while (pos < html.length() && std::isspace(html[pos])) {
        pos++;
    }
}

void HTMLParser::skipComment() {
    // Skip <!--
    pos += 3;
    while (pos < html.length() - 2) {
        if (html.substr(pos, 3) == "-->") {
            pos += 3;
            break;
        }
        pos++;
    }
}

void HTMLParser::skipDoctype() {
    while (pos < html.length() && html[pos] != '>') {
        pos++;
    }
    if (pos < html.length()) pos++; // skip '>'
}

void HTMLParser::debugLog(const std::string& msg) {
    if (debug) {
        std::cout << "[DEBUG] " << std::string(depth * 2, ' ') << msg << std::endl;
    }
}

std::string HTMLParser::parseTagName() {
    skipWhitespace();
    if (pos >= html.length() || html[pos] != '<') {
        debugLog("No opening bracket found");
        return "";
    }

    pos++; // skip '<'

    // Handle comments
    if (pos < html.length() - 2 && html.substr(pos, 3) == "!--") {
        debugLog("Found comment, skipping");
        skipComment();
        return "";
    }

    // Handle doctype
    if (pos < html.length() - 1 && html.substr(pos, 1) == "!") {
        debugLog("Found doctype, skipping");
        skipDoctype();
        return "";
    }

    size_t start = pos;

    // Handle closing tags
    bool isClosing = false;
    if (pos < html.length() && html[pos] == '/') {
        isClosing = true;
        pos++;
        start = pos;
    }

    while (pos < html.length() && html[pos] != '>' && html[pos] != ' ' && html[pos] != '\t' && html[pos] != '\n') {
        pos++;
    }

    std::string tagName = html.substr(start, pos - start);

    // Skip to end of tag
    while (pos < html.length() && html[pos] != '>') {
        pos++;
    }
    if (pos < html.length()) pos++; // skip '>'

    if (isClosing) {
        debugLog("Found closing tag: " + tagName);
        return "/" + tagName;
    }
    else {
        debugLog("Found opening tag: " + tagName);
        return tagName;
    }
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
        attrPos += attrName.length() + 1; // Skip 'attr='

        // Handle quoted attributes
        if (attrPos < tagContent.length() && tagContent[attrPos] == '"') {
            attrPos++; // Skip opening quote
            size_t endPos = tagContent.find('"', attrPos);
            if (endPos != std::string::npos) {
                result = tagContent.substr(attrPos, endPos - attrPos);
            }
        }
        else if (attrPos < tagContent.length() && tagContent[attrPos] == '\'') {
            attrPos++; // Skip opening quote
            size_t endPos = tagContent.find('\'', attrPos);
            if (endPos != std::string::npos) {
                result = tagContent.substr(attrPos, endPos - attrPos);
            }
        }
        else {
            // Unquoted attribute
            size_t endPos = tagContent.find(' ', attrPos);
            if (endPos == std::string::npos) {
                endPos = tagContent.find('>', attrPos);
            }
            if (endPos != std::string::npos) {
                result = tagContent.substr(attrPos, endPos - attrPos);
            }
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

    if (!text.empty()) {
        debugLog("Found text: \"" + text + "\"");
    }

    return text;
}

bool HTMLParser::isClosingTag(const std::string& tagName) {
    size_t savedPos = pos;
    skipWhitespace();

    if (pos >= html.length() || html[pos] != '<') {
        pos = savedPos;
        return false;
    }

    size_t checkPos = pos + 1;
    if (checkPos >= html.length() || html[checkPos] != '/') {
        pos = savedPos;
        return false;
    }

    checkPos++;
    std::string closingTag = html.substr(checkPos, tagName.length());

    bool isClosing = (closingTag == tagName);
    pos = savedPos;
    return isClosing;
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

    return ElementType::UNKNOWN;
}

std::string HTMLParser::getElementTypeName(ElementType type) {
    switch (type) {
    case ElementType::HTML: return "HTML";
    case ElementType::BODY: return "BODY";
    case ElementType::DIV: return "DIV";
    case ElementType::LABEL: return "LABEL";
    case ElementType::BUTTON: return "BUTTON";
    case ElementType::ANCHOR: return "ANCHOR";
    case ElementType::TEXT: return "TEXT";
    case ElementType::UNKNOWN: return "UNKNOWN";
    }
    return "UNKNOWN";
}