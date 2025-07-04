#include <iostream>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include "html_renderer.h"

void HTMLRenderer::render(const HTMLElementPtr& element) {
    if (!element) return;

    switch (element->type) {
    case ElementType::HTML:
    case ElementType::BODY:
        renderChildren(element);
        break;

    case ElementType::DIV:
        ImGui::BeginGroup();
        renderChildren(element);
        ImGui::EndGroup();
        ImGui::Spacing();
        break;

    case ElementType::LABEL:
        if (!element->children.empty()) {
            std::string labelText = getTextContent(element);
            ImGui::Text("%s", labelText.c_str());
        }
        break;

    case ElementType::BUTTON:
    {
        std::string buttonText = getTextContent(element);
        if (buttonText.empty()) buttonText = "Button";

        ImGui::PushID(buttonId++);
        if (ImGui::Button(buttonText.c_str())) {
            std::cout << "Button clicked: " << buttonText << std::endl;
        }
        ImGui::PopID();
    }
    break;

    case ElementType::ANCHOR:
    {
        std::string linkText = getTextContent(element);
        if (linkText.empty()) linkText = element->href;

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.5f, 0.9f, 0.5f));

        ImGui::PushID(buttonId++);
        if (ImGui::SmallButton(linkText.c_str())) {
            std::cout << "Link clicked: " << element->href << std::endl;
        }
        ImGui::PopID();

        ImGui::PopStyleColor(3);
    }
    break;

    case ElementType::TEXT:
        if (!element->text.empty()) {
            ImGui::TextWrapped("%s", element->text.c_str());
        }
        break;
    }
}

void HTMLRenderer::renderChildren(const HTMLElementPtr& element) {
    for (const auto& child : element->children) {
        render(child);
    }
}

std::string HTMLRenderer::getTextContent(const HTMLElementPtr& element) {
    std::string result;

    if (element->type == ElementType::TEXT) {
        return element->text;
    }

    for (const auto& child : element->children) {
        result += getTextContent(child);
    }

    return result;
}