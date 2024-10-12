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
		m_renderer.render(edge, m_nodes, ResourceManager::getShader("circle"), m_oriented);
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
		int startNodeID = m_nodes[edge.getStartNodeID() - 1].getID();
		int endNodeID = m_nodes[edge.getEndNodeID() - 1].getID();

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
static bool pressed = false;
static bool longClick = false;
static float pressStartTime = 0.0f;
static float holdThreshold = 0.35f;
static GraphNode* nodeToDrag = nullptr;

void Graph::handleInput()
{
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
			std::cout << "long click\n";
		}
	}

	if (longClick)
	{
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		if (nodeToDrag == nullptr)
		{
			for (auto& node : m_nodes)
			{
				if (glm::distance(node.getPosition(), glm::vec2{ xPos, yPos }) <= node.getSize().x)
				{
					nodeToDrag = &node;
					break;
				}
			}
		}

		if (nodeToDrag)
		{
			std::cout << "node with id: " << nodeToDrag->getID() << " selected\n";
			nodeToDrag->setPosition(glm::vec2{ xPos, yPos });
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && pressed)
	{
		if (longClick && nodeToDrag)
		{
			for (const auto& node : m_nodes)
			{
				if (&node == nodeToDrag)
				{
					continue;
				}

				if (glm::distance(node.getPosition(), nodeToDrag->getPosition()) < node.getSize().x * 2.0f)
				{
					glm::vec2 offsetDir = glm::normalize(nodeToDrag->getPosition() - node.getPosition());
					float offset = (node.getSize().x + nodeToDrag->getSize().x) - glm::distance(node.getPosition(), nodeToDrag->getPosition());

					nodeToDrag->setPosition(nodeToDrag->getPosition() + offsetDir * offset);
				}
			}
		}

		if (!longClick)
		{
			std::cout << "short click\n";

			std::cout << "mouse button 1 pressed\n";
			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);

			if (checkValidNodePosition(glm::vec2{ xPos, yPos }))
			{
				this->addNode(GraphNode{ glm::vec2{xPos, yPos}, static_cast<unsigned int>(m_nodes.size() + 1) });
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

static GraphNode* edgeStart;

void Graph::checkNodeSelect(glm::vec2 position)
{
	for (auto& node : m_nodes)
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

void Graph::tryAddEdge(GraphNode& edgeStart, GraphNode& edgeEnd)
{
	if (edgeStart.getPosition() == edgeEnd.getPosition())
	{
		std::cout << "Cannot add edge to the same node\n";
		return;
	}

	for (const auto& edge : m_edges)
	{
		if (m_oriented && m_nodes[edge.getStartNodeID() - 1].getPosition() == edgeStart.getPosition()
			&& m_nodes[edge.getEndNodeID() - 1].getPosition() == edgeEnd.getPosition())
		{
			std::cout << "Edge already exists\n";
			return;
		}

        if (!m_oriented && ((m_nodes[edge.getStartNodeID() - 1].getPosition() == edgeStart.getPosition()
            && m_nodes[edge.getEndNodeID() - 1].getPosition() == edgeEnd.getPosition())
            || (m_nodes[edge.getStartNodeID() - 1].getPosition() == edgeEnd.getPosition()
                && m_nodes[edge.getEndNodeID() - 1].getPosition() == edgeStart.getPosition())))
        {
            std::cout << "Edge already exists\n";
            return;
        }
	}

	std::cout << "Edge added\n";
	m_edges.push_back(Edge{ edgeStart.getID(), edgeEnd.getID() });
	nodeSelected = false;

	for (const auto& edge : m_edges)
	{
		std::cout << "Current edges: \n";
		std::cout << "Start: " << m_nodes[edge.getStartNodeID() - 1].getPosition().x
			<< " " << m_nodes[edge.getStartNodeID() - 1].getPosition().y << "\n";
		std::cout << "End: " << m_nodes[edge.getEndNodeID() - 1].getPosition().x
			<< " " << m_nodes[edge.getEndNodeID() - 1].getPosition().y << "\n";
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
