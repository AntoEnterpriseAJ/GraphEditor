#pragma once
#include "glm/glm.hpp"
#include "GraphNode.h"

class Edge
{
public:
	Edge(unsigned int nodeStartID, unsigned int nodeEndID);

	unsigned int getStartNodeID() const;
	unsigned int getEndNodeID() const;

private:
	unsigned int m_nodeStartID;
	unsigned int m_nodeEndID;
};