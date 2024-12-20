#include "MapEditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include "pugixml/pugixml.hpp"
#include <thread>
#include <sstream>

#ifdef _DEBUG
#include <iostream>
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) do {} while (0)
#endif

MapEditor::MapEditor()
    : m_graphData{}, m_renderer{}, m_selectedNode{nullptr}
{
    m_graphData.setLogAdjacency(true);
}

void MapEditor::render()
{
    for (const auto& edge : m_graphData.getEdges())
    {
        m_renderer.render(edge, ResourceManager::getShader("edge"), false, m_graphData.isWeighted());
    }

    //for (const auto& node : m_graphData.getNodes())
    //{
    //    m_renderer.render(node, ResourceManager::getShader("circle"));
    //}
}

static bool pressed = false;
static bool longClick = false;
static float pressStartTime = 0.0f;
static float holdThreshold = 0.35f;

void MapEditor::handleInput()
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
    {
        return;
    }

    GLFWwindow* window = glfwGetCurrentContext();
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        for (auto& node : m_graphData.getNodes())
        {
            if (glm::distance(node->getPosition(), glm::vec2{ xPos, yPos }) <= node->getSize().x)
            {
                m_selectedNode = node;
                break;
            }
        }
    }
}

static double minLatitude = std::numeric_limits<double>::max();
static double maxLatitude = std::numeric_limits<double>::lowest();
static double minLongitude = std::numeric_limits<double>::max();
static double maxLongitude = std::numeric_limits<double>::lowest();

void MapEditor::loadFromFile(const std::string& filePath)
{
    pugi::xml_document doc;
    if (!doc.load_file(filePath.c_str()))
    {
        std::cout << "Can't load map at path: " << filePath << "\n";
        return;
    }
    std::cout << "Succesfully loaded the map\n";

    unsigned int nodeCount = 0;
    pugi::xml_node nodes_node = doc.child("map").child("nodes");
    for (pugi::xml_node node = nodes_node.child("node"); node; node = node.next_sibling("node"))
    {
        double latitude = node.attribute("latitude").as_double();
        double longitude = node.attribute("longitude").as_double();

        if (latitude < minLatitude) minLatitude = latitude;
        if (latitude > maxLatitude) maxLatitude = latitude;
        if (longitude < minLongitude) minLongitude = longitude;
        if (longitude > maxLongitude) maxLongitude = longitude;

        nodeCount++;
    }

    std::vector<GraphNode*> nodes;
    nodes.reserve(nodeCount);

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    for (pugi::xml_node node = nodes_node.child("node"); node; node = node.next_sibling("node"))
    {
        unsigned int id = node.attribute("id").as_int();
        double latitude = node.attribute("latitude").as_double();
        double longitude = node.attribute("longitude").as_double();

        glm::vec2 coordinates{convertToScreenCoordinates(latitude, longitude, width, height)};
        nodes.push_back(new GraphNode{coordinates, "", static_cast<unsigned int>(nodes.size()), glm::vec2{1.0f, 1.0f}});
    }

    m_graphData.getNodes() = std::move(nodes);
    std::cout << "done with the nodes\n";

    std::vector<Edge> edges;
    edges.reserve(2 * nodeCount);

    pugi::xml_node arcs_node = doc.child("map").child("arcs");
    for (pugi::xml_node arc = arcs_node.child("arc"); arc; arc = arc.next_sibling("arc"))
    {
        unsigned int from = arc.attribute("from").as_int();
        unsigned int to = arc.attribute("to").as_int();
        int weight = arc.attribute("length").as_int();

        edges.emplace_back(m_graphData.getNode(from), m_graphData.getNode(to), weight, 0.1f);
    }

    m_graphData.getEdgesRef() = std::move(edges);
    std::cout << "done with the edges\n";
    m_graphData.updateAdjacencyList();

    std::cout << "done\n";
}

GraphData& MapEditor::getGraphData()
{
    return m_graphData;
}

const GraphNode* const MapEditor::getSelectedNode() const
{
    return m_selectedNode;
}

void MapEditor::setSelectedNode(GraphNode* node)
{
    m_selectedNode = node;
}

static GraphNode* edgeStart;

glm::vec2 MapEditor::convertToScreenCoordinates(double latitude, double longitude, int screenWidth, int screenHeight)
{
    double x = (longitude - minLongitude) / (maxLongitude - minLongitude) * screenWidth;
    double y = (latitude - minLatitude) / (maxLatitude - minLatitude) * screenHeight;

    return {x, y};
}

void MapEditor::checkNodeSelect(glm::vec2 position)
{
    for (auto& node : m_graphData.getNodes())
    {
        if (glm::distance(node->getPosition(), position) >= node->getSize().x)
        {
            continue;
        }

        LOG("node selected\n");
        m_selectedNode = node;
        edgeStart = node;
        return;
    }
    m_selectedNode = nullptr;
}