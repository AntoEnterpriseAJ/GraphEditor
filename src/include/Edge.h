#pragma once
#include "glm/glm.hpp"
#include "GraphNode.h"

class Edge
{
public:
	Edge(const GraphNode& nodeStart, const GraphNode& nodeEnd);

	const GraphNode& getStartNode() const;
	const GraphNode& getEndNode() const;

private:
	GraphNode m_nodeStart;
	GraphNode m_nodeEnd;
	bool m_oriented;
};