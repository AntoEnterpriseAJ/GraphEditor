#include "Graph.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include <thread>
#include <chrono>
#include <fstream>

Graph::Graph()
	: m_nodes{}, m_renderer{}, m_oriented{ true }
{
	ResourceManager::loadShader("res/shaders/circle.vert", "res/shaders/circle.frag", "circle");
	ResourceManager::loadShader("res/shaders/text.vert", "res/shaders/text.frag", "text");
}

void Graph::render()
{
	logAdjencyMatrix("res/adjMatrix/adjMatrix.txt");
	this->handleInput(); // TODO: move this

	for (const auto& edge : m_edges)
	{
		m_renderer.render(edge, ResourceManager::getShader("circle"), m_oriented);
	}

	for (const auto& node : m_nodes)
	{
		m_renderer.render(node, ResourceManager::getShader("circle"), ResourceManager::getShader("text"));
	}
}

void Graph::addNode(const GraphNode& node)
{
	m_nodes.push_back(node);
}

void Graph::logAdjencyMatrix(const std::string& fileName)
{
	std::ofstream file(fileName);
	if (!file.is_open())
	{
		std::cout << "Can't open file at: " << fileName << "\n";
		return;
	}

	std::vector<std::vector<int>> adjMatrix(m_nodes.size(), std::vector<int>(m_nodes.size(), 0));
	for (const auto& edge : m_edges)
	{
		int startNodeID = edge.getStartNode().getID();
		int endNodeID = edge.getEndNode().getID();

		if (m_oriented)
		{
			adjMatrix[startNodeID - 1][endNodeID - 1] = 1;
		}
		else
		{
			adjMatrix[startNodeID - 1][endNodeID - 1] = 1;
			adjMatrix[endNodeID - 1][startNodeID - 1] = 1;
		}
	}

	file << m_nodes.size() << "\n";
	for (const auto& row : adjMatrix)
	{
		for (const auto& elem : row)
		{
			file << elem << " ";
		}
		file << "\n";
	}
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
			this->addNode(GraphNode{glm::vec2{xPos, yPos}, static_cast<unsigned int>(m_nodes.size() + 1)});
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // TODO: remove this
			nodeSelected = false;
		}
		else
		{
			checkNodeSelect(glm::vec2{ xPos, yPos });
		}
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
