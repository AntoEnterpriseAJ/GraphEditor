#include "GraphNode.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"

GraphNode::GraphNode()
	: m_position{ 0.0f, 0.0f }, m_ID{0}, m_color{ 1.0f, 0.0f, 0.0f }, m_size{ 30.0f, 30.0f }
{}

GraphNode::GraphNode(glm::vec2 position, unsigned int ID, glm::vec2 size, glm::vec3 color)
	: m_position{ position }, m_ID{ID}, m_color{ color }, m_size{ size }
{}

void GraphNode::setPosition(glm::vec2 position)
{
	m_position = position;
}

glm::vec2 GraphNode::getPosition() const
{
	return m_position;
}

glm::vec2 GraphNode::getSize() const
{
	return m_size;
}

unsigned int GraphNode::getID() const
{
	return m_ID;
}
