#include "Edge.h"

Edge::Edge(GraphNode* nodeStart, GraphNode* nodeEnd)
    : m_nodeEnd{ nodeEnd }, m_nodeStart{ nodeStart }
{}

GraphNode* Edge::getStartNode() const
{
    return m_nodeStart;
}

GraphNode* Edge::getEndNode() const
{
    return m_nodeEnd;
}

bool Edge::operator==(const Edge& other) const
{
    return (m_nodeStart == other.m_nodeStart && m_nodeEnd == other.m_nodeEnd)
        || (m_nodeStart == other.m_nodeEnd && m_nodeEnd == other.m_nodeStart);
}
