#include "GraphNode.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"


GraphNode::GraphNode(glm::vec2 position, glm::vec2 size, glm::vec3 color)
	: m_position{ position }, m_color{ color }, m_size{ size }
{}

glm::vec2 GraphNode::getPosition() const
{
	return m_position;
}
