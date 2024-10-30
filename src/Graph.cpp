#include "Graph.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include <thread>
#include <chrono>
#include <fstream>

#ifdef _DEBUG
#include <iostream>
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) do {} while (0)
#endif

Graph::Graph()
	: m_nodes{}, m_renderer{}, m_oriented{ true }, m_actions{}
{
	ResourceManager::loadShader("res/shaders/circle.vert", "res/shaders/circle.frag", "circle");
	ResourceManager::loadShader("res/shaders/edge.vert", "res/shaders/edge.frag", "edge");
	ResourceManager::loadShader("res/shaders/text.vert", "res/shaders/text.frag", "text");
}

Graph::~Graph()
{
	for (auto node : m_nodes)
	{
		delete node;
	}
}

void Graph::render(Renderer::Primitive nodePrimitive)
{
	for (const auto& edge : m_edges)
	{
		m_renderer.render(edge, ResourceManager::getShader("edge"), m_oriented);
	}

	for (const auto& node : m_nodes)
	{
		m_renderer.render(
			node, ResourceManager::getShader("circle"), ResourceManager::getShader("text"), nodePrimitive
		);
	}
}

void Graph::addNode(GraphNode* node)
{
	m_nodes.push_back(node);
	m_actions.push(Action::newNode);
}

void Graph::addEdge(GraphNode* edgeStart, GraphNode* edgeEnd)
{
	LOG("Edge added\n");
	m_edges.push_back(Edge{ edgeStart, edgeEnd });
	m_actions.push(Action::newEdge);
	logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
}

void Graph::logAdjacencyMatrix(const std::string& fileName) const
{
	std::ofstream file(fileName);
	if (!file.is_open())
	{
		std::cout << "Can't open file at: " << fileName << "\n";
		return;
	}

	LOG("Logging adjacency matrix\n");

	std::vector<std::vector<int>> adjMatrix(m_nodes.size(), std::vector<int>(m_nodes.size(), 0));
	for (const auto& edge : m_edges)
	{
		int startNodeID = edge.getStartNode()->getID();
		int endNodeID = edge.getEndNode()->getID();

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

void Graph::clear()
{
	m_nodes.clear();
	m_edges.clear();
	logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
}

void Graph::undo()
{
	if (!m_actions.empty() && !m_edges.empty()  && m_actions.top() == Action::newEdge)
	{
		logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
		m_edges.pop_back();
		m_actions.pop();
	}
	else if (!m_actions.empty() && !m_nodes.empty() && m_actions.top() == Action::newNode)
	{
		logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
		m_nodes.pop_back();
		m_actions.pop();
	}
	else
	{
		LOG("Nothing to undo\n");
	}
}

void Graph::setOriented(bool oriented)
{
	m_oriented = oriented;
}

bool Graph::isOriented() const
{
	return m_oriented;
}

static bool nodeSelected = false;
static bool pressed = false;
static bool longClick = false;
static float pressStartTime = 0.0f;
static float holdThreshold = 0.35f;
static GraphNode* nodeToDrag = nullptr;

void Graph::handleInput()
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
			for (auto& node : m_nodes)
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
			LOG("node with id: " << nodeToDrag->getID() << " selected\n");
			nodeToDrag->setPosition(glm::vec2{ xPos, yPos });
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && pressed)
	{
		if (longClick && nodeToDrag)
		{
			for (const auto& node : m_nodes)
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
				this->addNode(new GraphNode{ glm::vec2{xPos, yPos}, static_cast<unsigned int>(m_nodes.size() + 1) });
				logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
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

void Graph::tryAddEdge(GraphNode* edgeStart, GraphNode* edgeEnd)
{
	if (edgeStart->getPosition() == edgeEnd->getPosition())
	{
		LOG("Cannot add edge to the same node\n");
		return;
	}

	for (const auto& edge : m_edges)
	{
		const auto existingEdgeStart = edge.getStartNode();
		const auto existingEdgeEnd = edge.getEndNode();;

		bool isSameDirection = (existingEdgeStart->getPosition() == edgeStart->getPosition()
			&& existingEdgeEnd->getPosition() == edgeEnd->getPosition());
		bool isOppositeDirection = (existingEdgeStart->getPosition() == edgeEnd->getPosition()
			&& existingEdgeEnd->getPosition() == edgeStart->getPosition());

		if ((m_oriented && isSameDirection) || (!m_oriented && (isSameDirection || isOppositeDirection)))
		{
			LOG("Edge already exists\n");
			return;
		}
	}

	addEdge(edgeStart, edgeEnd);
	nodeSelected = false;

	for (const auto& edge : m_edges)
	{
		LOG("Current edges: \n"
			<< "Start: " << edge.getStartNode()->getPosition().x
			<< " " << edge.getStartNode()->getPosition().y << "\n"
			<< "End: " << edge.getEndNode()->getPosition().x
			<< " " << edge.getEndNode()->getPosition().y << "\n");
	}
}

bool Graph::checkValidNodePosition(glm::vec2 position) const
{
	for (const auto& node : m_nodes)
	{
		if (glm::distance(node->getPosition(), position) < node->getSize().x * 2.0f)
		{
			return false;
		}
	}
	return true;
}