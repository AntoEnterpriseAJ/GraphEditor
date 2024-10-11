#include "Edge.h"

Edge::Edge(const GraphNode& nodeStart, const GraphNode& nodeEnd)
	: m_nodeStart{ nodeStart }, m_nodeEnd{ nodeEnd }
{}

const GraphNode& Edge::getStartNode() const
{
	return m_nodeStart;
}

const GraphNode& Edge::getEndNode() const
{
	return m_nodeEnd;
}
