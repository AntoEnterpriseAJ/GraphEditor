#include "Edge.h"

Edge::Edge(GraphNode* nodeStart, GraphNode* nodeEnd, int weight, float size)
    : m_nodeEnd{ nodeEnd }, m_nodeStart{ nodeStart }, m_weight{ weight }
    , m_color{ 1.0f, 0.0f, 0.0f, 1.0f }, m_size{size}
{}

GraphNode* Edge::getStartNode() const
{
    return m_nodeStart;
}

GraphNode* Edge::getEndNode() const
{
    return m_nodeEnd;
}

glm::vec4 Edge::getColor() const
{
    return m_color;
}

float Edge::getSize() const
{
    return m_size;
}

void Edge::setColor(const glm::vec4& color)
{
    m_color = color;
}

void Edge::setWeight(int weight)
{
    m_weight = weight;
}

int Edge::getWeight() const
{
    return m_weight;
}

bool Edge::operator==(const Edge& other) const
{
    return (m_nodeStart == other.m_nodeStart && m_nodeEnd == other.m_nodeEnd)
        || (m_nodeStart == other.m_nodeEnd && m_nodeEnd == other.m_nodeStart);
}
