#include "Graph.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include <thread>
#include <chrono>

Graph::Graph()
	: m_nodes{}, m_renderer{}
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
}

void Graph::addNode(const GraphNode& node)
{
	m_nodes.push_back(node);
}

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
		}
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
