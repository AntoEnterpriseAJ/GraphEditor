#include "Edge.h"

Edge::Edge(GraphNode* nodeStart, GraphNode* nodeEnd)
    : m_nodeEnd{ nodeEnd }, m_nodeStart{ nodeStart }, m_weight{ 0 }
{}

GraphNode* Edge::getStartNode() const
{
    return m_nodeStart;
}

GraphNode* Edge::getEndNode() const
{
    return m_nodeEnd;
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
