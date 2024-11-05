#include "GraphEditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include <thread>
#include <sstream>

#ifdef _DEBUG
#include <iostream>
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) do {} while (0)
#endif

GraphEditor::GraphEditor()
    : m_graphData{}, m_renderer {}
{
    ResourceManager::loadShader("res/shaders/circle.vert", "res/shaders/circle.frag", "circle");
    ResourceManager::loadShader("res/shaders/edge.vert", "res/shaders/edge.frag", "edge");
    ResourceManager::loadShader("res/shaders/text.vert", "res/shaders/text.frag", "text");
    m_graphData.setLogAdjacency(true);
}

void GraphEditor::render()
{
    for (const auto& edge : m_graphData.getEdges())
    {
        m_renderer.render(edge, ResourceManager::getShader("edge"), m_graphData.isOriented());
    }

    for (const auto& node : m_graphData.getNodes())
    {
        m_renderer.render(node, ResourceManager::getShader("circle"));
        m_renderer.renderText(node->getText(), ResourceManager::getShader("text"), node->getPosition());
    }
}

static bool nodeSelected = false;
static bool pressed = false;
static bool longClick = false;
static float pressStartTime = 0.0f;
static float holdThreshold = 0.35f;
static GraphNode* nodeToDrag = nullptr;

void GraphEditor::handleInput()
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
    {
        return;
    }

    GLFWwindow* window = glfwGetCurrentContext();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!pressed)
        {
            pressed = true;
            pressStartTime = glfwGetTime();
        }

        float pressDuration = glfwGetTime() - pressStartTime;
        if (!longClick && pressDuration >= holdThreshold)
        {
            longClick = true;
            LOG("long click\n");
        }
    }

    if (longClick)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        if (nodeToDrag == nullptr)
        {
            for (auto& node : m_graphData.getNodes())
            {
                if (glm::distance(node->getPosition(), glm::vec2{ xPos, yPos }) <= node->getSize().x)
                {
                    nodeToDrag = node;
                    break;
                }
            }
        }

        if (nodeToDrag)
        {
            LOG("node with id: " << nodeToDrag->getInternalID() << " selected\n");
            nodeToDrag->setPosition(glm::vec2{ xPos, yPos });
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && pressed)
    {
        if (longClick && nodeToDrag)
        {
            for (const auto& node : m_graphData.getNodes())
            {
                if (node == nodeToDrag)
                {
                    continue;
                }

                if (glm::distance(node->getPosition(), nodeToDrag->getPosition()) < node->getSize().x * 2.0f)
                {
                    glm::vec2 offsetDir = glm::normalize(nodeToDrag->getPosition() - node->getPosition());
                    float offset = (node->getSize().x + nodeToDrag->getSize().x) - glm::distance(node->getPosition(), nodeToDrag->getPosition());

                    nodeToDrag->setPosition(nodeToDrag->getPosition() + offsetDir * offset);
                }
            }
        }

        if (!longClick)
        {
            LOG("short click\n");

            double xPos, yPos;
            glfwGetCursorPos(window, &xPos, &yPos);

            if (checkValidNodePosition(glm::vec2{ xPos, yPos }))
            {
                m_graphData.addNode(new GraphNode{
                    glm::vec2{xPos, yPos},
                    std::to_string(m_graphData.getNodes().size() + 1),
                    static_cast<unsigned int>(m_graphData.getNodes().size() + 1),
                    glm::vec2{GraphEditor::kNodeRadius, GraphEditor::kNodeRadius} });
                m_graphData.logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
                nodeSelected = false;
            }
            else
            {
                checkNodeSelect(glm::vec2{ xPos, yPos });
            }
        }

        nodeToDrag = nullptr;
        pressed = false;
        longClick = false;
    }
}

GraphData& GraphEditor::getGraphData()
{
    return m_graphData;
}

static GraphNode* edgeStart;

void GraphEditor::checkNodeSelect(glm::vec2 position)
{
    for (auto& node : m_graphData.getNodes())
    {
        if (glm::distance(node->getPosition(), position) >= node->getSize().x)
        {
            continue;
        }

        if (nodeSelected)
        {
            tryAddEdge(edgeStart, node);
            nodeSelected = false;
            edgeStart = nullptr;
            return;
        }

        LOG("node selected\n");
        nodeSelected = true;
        edgeStart = node;
        return;
    }
    nodeSelected = false;
}

void GraphEditor::tryAddEdge(GraphNode* edgeStart, GraphNode* edgeEnd)
{
    if (edgeStart->getPosition() == edgeEnd->getPosition())
    {
        LOG("Cannot add edge to the same node\n");
        return;
    }

    for (const auto& edge : m_graphData.getEdges())
    {
        const auto existingEdgeStart = edge.getStartNode();
        const auto existingEdgeEnd = edge.getEndNode();;

        bool isSameDirection = (existingEdgeStart->getPosition() == edgeStart->getPosition()
            && existingEdgeEnd->getPosition() == edgeEnd->getPosition());
        bool isOppositeDirection = (existingEdgeStart->getPosition() == edgeEnd->getPosition()
            && existingEdgeEnd->getPosition() == edgeStart->getPosition());

        if ((m_graphData.isOriented() && isSameDirection) || (!m_graphData.isOriented() && (isSameDirection || isOppositeDirection)))
        {
            LOG("Edge already exists\n");
            return;
        }
    }

    m_graphData.addEdge(edgeStart, edgeEnd);
    nodeSelected = false;

    for (const auto& edge : m_graphData.getEdges())
    {
        LOG("Current edges: \n"
            << "Start: " << edge.getStartNode()->getPosition().x
            << " " << edge.getStartNode()->getPosition().y << "\n"
            << "End: " << edge.getEndNode()->getPosition().x
            << " " << edge.getEndNode()->getPosition().y << "\n");
    }
}

bool GraphEditor::checkValidNodePosition(glm::vec2 position) 
{
    for (auto& node : m_graphData.getNodes())
    {
        if (glm::distance(node->getPosition(), position) < node->getSize().x * 2.0f)
        {
            return false;
        }
    }
    return true;
}