#pragma once
#include "glm/glm.hpp"
#include "GraphNode.h"

class Edge
{
public:
    Edge(GraphNode* nodeStart, GraphNode* nodeEnd);

    GraphNode* getStartNode() const;
    GraphNode* getEndNode() const;
    glm::vec4  getColor() const;

    void setColor(const glm::vec4& color);
    void setWeight(int weight);

    int  getWeight() const;
    bool operator==(const Edge& other) const;

private:
    GraphNode* m_nodeStart;
    GraphNode* m_nodeEnd;
    glm::vec4  m_color;
    int        m_weight;
};