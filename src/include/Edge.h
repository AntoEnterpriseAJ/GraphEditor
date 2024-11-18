#pragma once
#include "glm/glm.hpp"
#include "GraphNode.h"

class Edge
{
public:
    Edge(GraphNode* nodeStart, GraphNode* nodeEnd);

    GraphNode* getStartNode() const;
    GraphNode* getEndNode() const;

    bool operator==(const Edge& other) const;

private:
    GraphNode* m_nodeStart;
    GraphNode* m_nodeEnd;
};