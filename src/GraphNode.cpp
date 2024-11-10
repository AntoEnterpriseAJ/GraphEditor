#include "GraphNode.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"

GraphNode::GraphNode(glm::vec2 position, const std::string& text, unsigned int ID, glm::vec2 size, glm::vec4 color)
    : m_position{ position }, m_label{text}, m_internalID{ID}, m_color{color}, m_size{size}
{}

void GraphNode::setPosition(glm::vec2 position)
{
    m_position = position;
}

void GraphNode::setColor(glm::vec4 color)
{
    m_color = color;
}

std::string GraphNode::getLabel() const
{
    return m_label;
}

glm::vec2 GraphNode::getPosition() const
{
    return m_position;
}

glm::vec2 GraphNode::getSize() const
{
    return m_size;
}

glm::vec4 GraphNode::getColor() const
{
    return m_color;
}

unsigned int GraphNode::getInternalID() const
{
    return m_internalID;
}
