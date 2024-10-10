#include "Graph.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include <thread>
#include <chrono>

Graph::Graph()
	: m_nodes{}, m_renderer{}, m_oriented{false}
{
	ResourceManager::loadShader("res/shaders/circle.vert", "res/shaders/circle.frag", "circle");
}

void Graph::render()
{
	this->handleInput(); // TODO: move this

	for (const auto& node : m_nodes)
	{
		m_renderer.render(node, ResourceManager::getShader("circle"));
	}

	for (const auto& edge : m_edges)
	{
		m_renderer.render(edge, ResourceManager::getShader("circle"));
	}
}

void Graph::addNode(const GraphNode& node)
{
	m_nodes.push_back(node);
}

static bool nodeSelected = false;

void Graph::handleInput()
{
	GLFWwindow* window = glfwGetCurrentContext();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		std::cout << "mouse button 1 pressed\n";
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		if (checkValidNodePosition(glm::vec2{xPos, yPos}))
		{
			this->addNode(GraphNode{ glm::vec2{ static_cast<float>(xPos), static_cast<float>(yPos) } });
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // TODO: remove this
			nodeSelected = false;
		}
		else
			checkNodeSelect(glm::vec2{xPos, yPos});
	}
}

static const GraphNode* edgeStart;

void Graph::checkNodeSelect(glm::vec2 position)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(200)); // TODO: remove this

	for (const auto& node : m_nodes)
	{
		if (glm::distance(node.getPosition(), position) >= node.getSize().x)
		{
			continue;
		}

		if (nodeSelected)
		{
			tryAddEdge(*edgeStart, node);
			nodeSelected = false;
			edgeStart = nullptr;
			return;
		}

		std::cout << "node selected\n";
		nodeSelected = true;
		edgeStart = &node;
		return;
	}
	nodeSelected = false;
}

void Graph::tryAddEdge(const GraphNode& edgeStart, const GraphNode& edgeEnd)
{
	if (edgeStart.getPosition() == edgeEnd.getPosition())
	{
		std::cout << "Cannot add edge to the same node\n";
		return;
	}

	for (const auto& edge : m_edges)
	{
		if (edge.getStartNode().getPosition() == edgeStart.getPosition() 
			&& edge.getEndNode().getPosition() == edgeEnd.getPosition())
		{
			std::cout << "Edge already exists\n";
			return;
		}
	}

	std::cout << "Edge added\n";
	m_edges.push_back(Edge{ edgeStart, edgeEnd });
	nodeSelected = false;

	for (const auto& edge : m_edges)
	{
		std::cout << "Current edges: \n";
		std::cout << "Start: " << edge.getStartNode().getPosition().x << " " << edge.getStartNode().getPosition().y << "\n";
		std::cout << "End: " << edge.getEndNode().getPosition().x << " " << edge.getEndNode().getPosition().y << "\n";
	}
}

bool Graph::checkValidNodePosition(glm::vec2 position) const
{
	for (const auto& node : m_nodes)
	{
		if (glm::distance(node.getPosition(), position) < node.getSize().x * 2.0f)
		{
			return false;
		}
	}
	return true;
}
