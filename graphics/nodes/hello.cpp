#include "node_editor.h"
#include "graph.h"

#include <imnodes.h>
#include <imgui.h>

#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <vector>

/*

http://multiversesocial.com/web4.html

ssd data -> device_memory_channel ->
        -> data vlc_channel -> audio, video, subtitles
        -> audio_channel- > speakers
        -> video_channel ->
        -> gpu_channel ->
        -> screen_channel -> monitor
*/

namespace example
{
namespace
{
enum class NodeType
{
    add,
    multiply,
    output,
    sine,
    time,
    device_memory_channel,
    vlc_channel,
    vlc_channel_2,
    vlc_channel_3,
    audio_channel,
    video_channel,
    gpu_channel,
    screen_channel,
    monitor,
    speakers,
    ssd,
    value
};

struct Node
{
    NodeType type;
    float    value;

    explicit Node(const NodeType t) : type(t), value(0.f) {}

    Node(const NodeType t, const float v) : type(t), value(v) {}
};

template<class T>
T clamp(T x, T a, T b)
{
    return std::min(b, std::max(x, a));
}

static float current_time_seconds = 0.f;

ImU32 evaluate(const Graph<Node>& graph, const int root_node)
{
    std::stack<int> postorder;
    dfs_traverse(
        graph, root_node, [&postorder](const int node_id) -> void { postorder.push(node_id); });

    std::stack<float> value_stack;
    while (!postorder.empty())
    {
        const int id = postorder.top();
        postorder.pop();
        const Node node = graph.node(id);

        switch (node.type)
        {
        case NodeType::add:
        {
            const float rhs = value_stack.top();
            value_stack.pop();
            const float lhs = value_stack.top();
            value_stack.pop();
            value_stack.push(lhs + rhs);
        }
        break;
        case NodeType::multiply:
        {
            const float rhs = value_stack.top();
            value_stack.pop();
            const float lhs = value_stack.top();
            value_stack.pop();
            value_stack.push(rhs * lhs);
        }
        break;
        case NodeType::sine:
        {
            const float x = value_stack.top();
            value_stack.pop();
            const float res = std::abs(std::sin(x));
            value_stack.push(res);
        }
        break;
        case NodeType::time:
        {
            value_stack.push(current_time_seconds);
        }
        break;
        case NodeType::value:
        {
            // If the edge does not have an edge connecting to another node, then just use the value
            // at this node. It means the node's input pin has not been connected to anything and
            // the value comes from the node's UI.
            if (graph.num_edges_from_node(id) == 0ull)
            {
                value_stack.push(node.value);
            }
        }
        break;
        default:
            break;
        }
    }

    // The final output node isn't evaluated in the loop -- instead we just pop
    // the three values which should be in the stack.
    assert(value_stack.size() == 3ull);
    const int b = static_cast<int>(255.f * clamp(value_stack.top(), 0.f, 1.f) + 0.5f);
    value_stack.pop();
    const int g = static_cast<int>(255.f * clamp(value_stack.top(), 0.f, 1.f) + 0.5f);
    value_stack.pop();
    const int r = static_cast<int>(255.f * clamp(value_stack.top(), 0.f, 1.f) + 0.5f);
    value_stack.pop();

    return IM_COL32(r, g, b, 255);
}

class ColorNodeEditor
{
public:
    ColorNodeEditor()
        : graph_(), nodes_(), root_node_id_(-1),
          minimap_location_(ImNodesMiniMapLocation_BottomRight)
    {
    }

    void show()
    {
        // Update timer context
        current_time_seconds = 0.001f * SDL_GetTicks();

        auto flags = ImGuiWindowFlags_MenuBar;

        // The node editor window
        ImGui::Begin("channels node editor", NULL, flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Mini-map"))
            {
                const char* names[] = {
                    "Top Left",
                    "Top Right",
                    "Bottom Left",
                    "Bottom Right",
                };
                int locations[] = {
                    ImNodesMiniMapLocation_TopLeft,
                    ImNodesMiniMapLocation_TopRight,
                    ImNodesMiniMapLocation_BottomLeft,
                    ImNodesMiniMapLocation_BottomRight,
                };

                for (int i = 0; i < 4; i++)
                {
                    bool selected = minimap_location_ == locations[i];
                    if (ImGui::MenuItem(names[i], NULL, &selected))
                        minimap_location_ = locations[i];
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Style"))
            {
                if (ImGui::MenuItem("Classic"))
                {
                    ImGui::StyleColorsClassic();
                    ImNodes::StyleColorsClassic();
                }
                if (ImGui::MenuItem("Dark"))
                {
                    ImGui::StyleColorsDark();
                    ImNodes::StyleColorsDark();
                }
                if (ImGui::MenuItem("Light"))
                {
                    ImGui::StyleColorsLight();
                    ImNodes::StyleColorsLight();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::TextUnformatted("Edit the color of the output color window using nodes.");
        ImGui::Columns(1);
        ImGui::TextUnformatted("A -- add node");
        ImGui::TextUnformatted("X -- delete selected node or link");
        ImGui::Columns(1);

        ImNodes::BeginNodeEditor();

        // Handle new nodes
        // These are driven by the user, so we place this code before rendering the nodes
        {
            const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                    ImNodes::IsEditorHovered() && ImGui::IsKeyReleased(ImGuiKey_A);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
            if (!ImGui::IsAnyItemHovered() && open_popup)
            {
                ImGui::OpenPopup("add node");
            }

            if (ImGui::BeginPopup("add node"))
            {
                const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

                if (ImGui::MenuItem("VLC channel"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::vlc_channel);
                    const Node op2(NodeType::vlc_channel);
                    const Node op3(NodeType::vlc_channel);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::vlc_channel;
                    ui_node.ui.vlc_channel.buffer = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);
                    ui_node.id2 = graph_.insert_node(op2);
                    ui_node.id3 = graph_.insert_node(op3);

                    graph_.insert_edge(ui_node.id, ui_node.ui.vlc_channel.buffer);
                    graph_.insert_edge(ui_node.id2, ui_node.ui.vlc_channel.buffer);
                    graph_.insert_edge(ui_node.id3, ui_node.ui.vlc_channel.buffer);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("device memory channel"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::device_memory_channel);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::device_memory_channel;
                    ui_node.ui.device_memory_channel.data = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.device_memory_channel.data);
                    
                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("audio channel"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::audio_channel);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::audio_channel;
                    ui_node.ui.audio_channel.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.audio_channel.input);
                    
                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("video channel"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::video_channel);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::video_channel;
                    ui_node.ui.video_channel.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.video_channel.input);
                    
                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("GPU channel"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::gpu_channel);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::gpu_channel;
                    ui_node.ui.gpu_channel.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.gpu_channel.input);
                    
                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("screen channel"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::screen_channel);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::screen_channel;
                    ui_node.ui.screen_channel.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.screen_channel.input);
                    
                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("multiply"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::multiply);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::multiply;
                    ui_node.ui.multiply.lhs = graph_.insert_node(value);
                    ui_node.ui.multiply.rhs = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.multiply.lhs);
                    graph_.insert_edge(ui_node.id, ui_node.ui.multiply.rhs);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("speakers") && root_node_id_ == -1)
                {
                    const Node value(NodeType::value, 0.f);
                    const Node out(NodeType::speakers);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::speakers;
                    ui_node.ui.speakers.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(out);

                    graph_.insert_edge(ui_node.id, ui_node.ui.speakers.input);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("monitor") && root_node_id_ == -1)
                {
                    const Node value(NodeType::value, 0.f);
                    const Node out(NodeType::monitor);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::monitor;
                    ui_node.ui.monitor.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(out);

                    graph_.insert_edge(ui_node.id, ui_node.ui.monitor.input);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("sine"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::sine);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::sine;
                    ui_node.ui.sine.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.sine.input);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("time"))
                {
                    UiNode ui_node;
                    ui_node.type = UiNodeType::time;
                    ui_node.id = graph_.insert_node(Node(NodeType::time));

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("SSD"))
                {
                    UiNode ui_node;
                    ui_node.type = UiNodeType::ssd;
                    ui_node.id = graph_.insert_node(Node(NodeType::ssd));

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
        }

        for (const UiNode& node : nodes_)
        {
            switch (node.type)
            {
            case UiNodeType::add:
            {
                const float node_width = 100.f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("add");
                ImNodes::EndNodeTitleBar();
                {
                    ImNodes::BeginInputAttribute(node.ui.add.lhs);
                    const float label_width = ImGui::CalcTextSize("left").x;
                    ImGui::TextUnformatted("left");
                    if (graph_.num_edges_from_node(node.ui.add.lhs) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat("##hidelabel", &graph_.node(node.ui.add.lhs).value, 0.01f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }

                {
                    ImNodes::BeginInputAttribute(node.ui.add.rhs);
                    const float label_width = ImGui::CalcTextSize("right").x;
                    ImGui::TextUnformatted("right");
                    if (graph_.num_edges_from_node(node.ui.add.rhs) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat("##hidelabel", &graph_.node(node.ui.add.rhs).value, 0.01f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("result").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("result");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::multiply:
            {
                const float node_width = 100.0f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("multiply");
                ImNodes::EndNodeTitleBar();

                {
                    ImNodes::BeginInputAttribute(node.ui.multiply.lhs);
                    const float label_width = ImGui::CalcTextSize("left").x;
                    ImGui::TextUnformatted("left");
                    if (graph_.num_edges_from_node(node.ui.multiply.lhs) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat(
                            "##hidelabel", &graph_.node(node.ui.multiply.lhs).value, 0.01f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }

                {
                    ImNodes::BeginInputAttribute(node.ui.multiply.rhs);
                    const float label_width = ImGui::CalcTextSize("right").x;
                    ImGui::TextUnformatted("right");
                    if (graph_.num_edges_from_node(node.ui.multiply.rhs) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat(
                            "##hidelabel", &graph_.node(node.ui.multiply.rhs).value, 0.01f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("result").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("result");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::output:
            {
                const float node_width = 100.0f;
                ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(11, 109, 191, 255));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(45, 126, 194, 255));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(81, 148, 204, 255));
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("output");
                ImNodes::EndNodeTitleBar();

                ImGui::Dummy(ImVec2(node_width, 0.f));
                {
                    ImNodes::BeginInputAttribute(node.ui.output.r);
                    const float label_width = ImGui::CalcTextSize("r").x;
                    ImGui::TextUnformatted("r");
                    if (graph_.num_edges_from_node(node.ui.output.r) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat(
                            "##hidelabel", &graph_.node(node.ui.output.r).value, 0.01f, 0.f, 1.0f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginInputAttribute(node.ui.output.g);
                    const float label_width = ImGui::CalcTextSize("g").x;
                    ImGui::TextUnformatted("g");
                    if (graph_.num_edges_from_node(node.ui.output.g) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat(
                            "##hidelabel", &graph_.node(node.ui.output.g).value, 0.01f, 0.f, 1.f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginInputAttribute(node.ui.output.b);
                    const float label_width = ImGui::CalcTextSize("b").x;
                    ImGui::TextUnformatted("b");
                    if (graph_.num_edges_from_node(node.ui.output.b) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat(
                            "##hidelabel", &graph_.node(node.ui.output.b).value, 0.01f, 0.f, 1.0f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }
                ImNodes::EndNode();
                ImNodes::PopColorStyle();
                ImNodes::PopColorStyle();
                ImNodes::PopColorStyle();
            }
            break;
            case UiNodeType::sine:
            {
                const float node_width = 100.0f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("sine");
                ImNodes::EndNodeTitleBar();

                {
                    ImNodes::BeginInputAttribute(node.ui.sine.input);
                    const float label_width = ImGui::CalcTextSize("number").x;
                    ImGui::TextUnformatted("number");
                    if (graph_.num_edges_from_node(node.ui.sine.input) == 0ull)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(node_width - label_width);
                        ImGui::DragFloat(
                            "##hidelabel",
                            &graph_.node(node.ui.sine.input).value,
                            0.01f,
                            0.f,
                            1.0f);
                        ImGui::PopItemWidth();
                    }
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("output").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("output");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::time:
            {
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("time");
                ImNodes::EndNodeTitleBar();

                ImNodes::BeginOutputAttribute(node.id);
                ImGui::Text("output");
                ImNodes::EndOutputAttribute();

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::ssd:
            {
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("SSD");
                ImNodes::EndNodeTitleBar();

                ImNodes::BeginOutputAttribute(node.id);
                ImGui::Text("data");
                ImNodes::EndOutputAttribute();

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::monitor:
            {
                // ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(11, 109, 191, 255));
                // ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(45, 126, 194, 255));
                // ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(81, 148, 204, 255));
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("monitor");
                ImNodes::EndNodeTitleBar();

                ImNodes::BeginInputAttribute(node.ui.output.r);
                ImGui::Text("screen");
                ImNodes::EndInputAttribute();

                ImNodes::EndNode();
                // ImNodes::PopColorStyle();
                // ImNodes::PopColorStyle();
                // ImNodes::PopColorStyle();
            }
            break;
            case UiNodeType::speakers:
            {
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("speakers");
                ImNodes::EndNodeTitleBar();

                ImNodes::BeginInputAttribute(node.ui.speakers.input);
                ImGui::Text("sound");
                ImNodes::EndInputAttribute();

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::device_memory_channel:
            {
                const float node_width = 140.f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("device memory channel");
                ImNodes::EndNodeTitleBar();
                {
                    ImNodes::BeginInputAttribute(node.ui.device_memory_channel.data);
                    ImGui::TextUnformatted("data");
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("buffer").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("buffer");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::vlc_channel:
            {
                const float node_width = 100.f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("VLC channel");
                ImNodes::EndNodeTitleBar();
                {
                    ImNodes::BeginInputAttribute(node.ui.vlc_channel.buffer);
                    ImGui::TextUnformatted("buffer");
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("audio buffer").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("audio buffer");
                    ImNodes::EndOutputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id2);
                    const float label_width = ImGui::CalcTextSize("video buffer").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("video buffer");
                    ImNodes::EndOutputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id3);
                    const float label_width = ImGui::CalcTextSize("subtitles buffer").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("subtitles buffer");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::audio_channel:
            {
                const float node_width = 100.f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("audio channel");
                ImNodes::EndNodeTitleBar();
                {
                    ImNodes::BeginInputAttribute(node.ui.audio_channel.input);
                    ImGui::TextUnformatted("audio buffer");
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("sound").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("sound");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::video_channel:
            {
                const float node_width = 100.f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("video channel");
                ImNodes::EndNodeTitleBar();
                {
                    ImNodes::BeginInputAttribute(node.ui.add.lhs);
                    ImGui::TextUnformatted("video buffer");
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("buffer").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("buffer");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::gpu_channel:
            {
                const float node_width = 100.f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("GPU channel");
                ImNodes::EndNodeTitleBar();
                {
                    ImNodes::BeginInputAttribute(node.ui.add.lhs);
                    ImGui::TextUnformatted("buffer");
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("framebuffer").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("framebuffer");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            case UiNodeType::screen_channel:
            {
                const float node_width = 100.f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("screen channel");
                ImNodes::EndNodeTitleBar();
                {
                    ImNodes::BeginInputAttribute(node.ui.add.lhs);
                    ImGui::TextUnformatted("framebuffer");
                    ImNodes::EndInputAttribute();
                }

                ImGui::Spacing();

                {
                    ImNodes::BeginOutputAttribute(node.id);
                    const float label_width = ImGui::CalcTextSize("monitor").x;
                    ImGui::Indent(node_width - label_width);
                    ImGui::TextUnformatted("monitor");
                    ImNodes::EndOutputAttribute();
                }

                ImNodes::EndNode();
            }
            break;
            }
        }

        for (const auto& edge : graph_.edges())
        {
            // If edge doesn't start at value, then it's an internal edge, i.e.
            // an edge which links a node's operation to its input. We don't
            // want to render node internals with visible links.
            if (graph_.node(edge.from).type != NodeType::value)
                continue;

            ImNodes::Link(edge.id, edge.from, edge.to);
        }

        ImNodes::MiniMap(0.2f, minimap_location_);
        ImNodes::EndNodeEditor();

        // Handle new links
        // These are driven by Imnodes, so we place the code after EndNodeEditor().

        {
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                const NodeType start_type = graph_.node(start_attr).type;
                const NodeType end_type = graph_.node(end_attr).type;

                const bool valid_link = start_type != end_type;
                if (valid_link)
                {
                    // Ensure the edge is always directed from the value to
                    // whatever produces the value
                    if (start_type != NodeType::value)
                    {
                        std::swap(start_attr, end_attr);
                    }
                    graph_.insert_edge(start_attr, end_attr);
                }
            }
        }

        // Handle deleted links

        {
            int link_id;
            if (ImNodes::IsLinkDestroyed(&link_id))
            {
                graph_.erase_edge(link_id);
            }
        }

        // delete edge action
        {
            const int num_selected = ImNodes::NumSelectedLinks();
            if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_X))
            {
                static std::vector<int> selected_links;
                selected_links.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedLinks(selected_links.data());
                for (const int edge_id : selected_links)
                {
                    graph_.erase_edge(edge_id);
                }
            }
        }

        // delete node action
        {
            const int num_selected = ImNodes::NumSelectedNodes();
            if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_X))
            {
                static std::vector<int> selected_nodes;
                selected_nodes.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedNodes(selected_nodes.data());
                for (const int node_id : selected_nodes)
                {
                    graph_.erase_node(node_id);
                    auto iter = std::find_if(
                        nodes_.begin(), nodes_.end(), [node_id](const UiNode& node) -> bool {
                            return node.id == node_id;
                        });
                    // Erase any additional internal nodes
                    switch (iter->type)
                    {
                    case UiNodeType::add:
                        graph_.erase_node(iter->ui.add.lhs);
                        graph_.erase_node(iter->ui.add.rhs);
                        break;
                    case UiNodeType::multiply:
                        graph_.erase_node(iter->ui.multiply.lhs);
                        graph_.erase_node(iter->ui.multiply.rhs);
                        break;
                    case UiNodeType::output:
                        graph_.erase_node(iter->ui.output.r);
                        graph_.erase_node(iter->ui.output.g);
                        graph_.erase_node(iter->ui.output.b);
                        root_node_id_ = -1;
                        break;
                    case UiNodeType::sine:
                        graph_.erase_node(iter->ui.sine.input);
                        break;
                    default:
                        break;
                    }
                    nodes_.erase(iter);
                }
            }
        }

        ImGui::End();

        // The color output window

        const ImU32 color =
            root_node_id_ != -1 ? evaluate(graph_, root_node_id_) : IM_COL32(255, 20, 147, 255);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, color);
        ImGui::Begin("output color");
        ImGui::End();
        ImGui::PopStyleColor();
    }

private:
    enum class UiNodeType
    {
        add,
        multiply,
        output,
        sine,
        time,
        device_memory_channel,
        vlc_channel,
        audio_channel,
        video_channel,
        gpu_channel,
        screen_channel,
        monitor,
        speakers,
        ssd,
    };

    struct UiNode
    {
        UiNodeType type;
        // The identifying id of the ui node. For add, multiply, sine, and time
        // this is the "operation" node id. The additional input nodes are
        // stored in the structs.
        int id;
        int id2;
        int id3;

        union
        {
            struct
            {
                int lhs, rhs;
            } add;

            struct
            {
                int lhs, rhs;
            } multiply;

            struct
            {
                int r, g, b;
            } output;

            struct
            {
                int input;
            } sine;

            struct
            {
                int data;
            } device_memory_channel;

            struct
            {
                int buffer;
            } vlc_channel;

            struct
            {
                int input;
            } audio_channel;

            struct
            {
                int input;
            } video_channel;

            struct
            {
                int input;
            } gpu_channel;

            struct
            {
                int input;
            } screen_channel;

            struct
            {
                int input;
            } monitor;

            struct
            {
                int input;
            } speakers;

            struct
            {
                int input;
            } ssd;
        } ui;
    };

    Graph<Node>            graph_;
    std::vector<UiNode>    nodes_;
    int                    root_node_id_;
    ImNodesMiniMapLocation minimap_location_;
};

static ColorNodeEditor color_editor;
} // namespace

void NodeEditorInitialize()
{
    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
}

void NodeEditorShow() { color_editor.show(); }

void NodeEditorShutdown() {}
} // namespace example