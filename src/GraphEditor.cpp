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
    : m_graphData{}, m_renderer{}, m_selectedNode{nullptr}
{
    m_graphData.setLogAdjacency(true);
}

void GraphEditor::render()
{
    for (const auto& edge : m_graphData.getEdges())
    {
        m_renderer.render(edge, ResourceManager::getShader("edge"), m_graphData.isOriented(), m_graphData.isWeighted());
    }

    for (const auto& node : m_graphData.getNodes())
    {
        m_renderer.render(node, ResourceManager::getShader("circle"));
        m_renderer.renderText(node->getLabel(), ResourceManager::getShader("text"), node->getPosition());
    }
}

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

            if (!checkEdgeSelect(glm::vec2{ xPos, yPos }))
            {
                if (checkValidNodePosition(glm::vec2{ xPos, yPos }))
                {
                    m_graphData.addNode(new GraphNode{
                        glm::vec2{xPos, yPos},
                        std::to_string(m_graphData.getNodes().size()),
                        static_cast<unsigned int>(m_graphData.getNodes().size()),
                        glm::vec2{GraphEditor::kNodeRadius, GraphEditor::kNodeRadius} });
                    m_graphData.logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
                    m_selectedNode = nullptr;
                }
                else
                {
                    checkNodeSelect(glm::vec2{ xPos, yPos });
                }
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

const GraphNode* const GraphEditor::getSelectedNode() const
{
    return m_selectedNode;
}

void GraphEditor::setSelectedNode(GraphNode* node)
{
    m_selectedNode = node;
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

        if (m_selectedNode != nullptr)
        {
            tryAddEdge(edgeStart, node);
            m_selectedNode = nullptr;
            edgeStart = nullptr;
            return;
        }

        LOG("node selected\n");
        m_selectedNode = node;
        edgeStart = node;
        return;
    }
    m_selectedNode = nullptr;
}

bool GraphEditor::checkEdgeSelect(glm::vec2 position)
{
    if (!m_graphData.isWeighted())
    {
        return false;
    }

    constexpr float selectionThreshold = 10.0f;

    for (auto& edge : m_graphData.getEdgesRef())
    {
        glm::vec2 edgeStart{ edge.getStartNode()->getPosition().x, edge.getStartNode()->getPosition().y };
        glm::vec2 edgeEnd{ edge.getEndNode()->getPosition().x, edge.getEndNode()->getPosition().y };
        float tipThreshold = glm::distance(edgeStart, edgeEnd) / 2.0f;

        glm::vec2 dir = glm::normalize(edgeEnd - edgeStart);

        edgeStart += dir * edge.getStartNode()->getSize().x;
        edgeEnd += -dir * edge.getStartNode()->getSize().x;

        float t = glm::dot(position - edgeStart, dir);
        glm::vec2 closestPoint = edgeStart + glm::clamp(t, 0.0f, glm::distance(edgeStart, edgeEnd)) * dir;

        if (glm::distance(position, closestPoint) < selectionThreshold)
        {
            if (m_graphData.isOriented() && glm::distance(position, edgeEnd) < tipThreshold)
            {
                std::cout << "Edge end selected, enter a weight:\n";
                int weight = 0;
                std::cin >> weight;

                edge.setWeight(weight);
                //TODO: undo, clear, ...
                m_graphData.getEdgeWeights()[{edge.getStartNode()->getInternalID(), edge.getEndNode()->getInternalID()}] = weight;
                return true;
            }

            if (!m_graphData.isOriented())
            {
                std::cout << "Edge selected, enter a weight:\n";
                int weight = 0;
                std::cin >> weight;

                edge.setWeight(weight);
                m_graphData.getEdgeWeights()[{edge.getStartNode()->getInternalID(), edge.getEndNode()->getInternalID()}] = weight;
                m_graphData.getEdgeWeights()[{edge.getEndNode()->getInternalID(), edge.getStartNode()->getInternalID()}] = weight;
                return true;
            }
        }
    }
    return false;
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
        const auto existingEdgeEnd = edge.getEndNode();

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
    m_selectedNode = nullptr;

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