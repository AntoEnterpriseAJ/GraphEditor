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