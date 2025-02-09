#pragma once
#include "glm/glm.hpp"
#include "GraphNode.h"

class Edge
{
public:
    Edge(GraphNode* nodeStart, GraphNode* nodeEnd, int weight = 0, float size = 6.0f);

    GraphNode* getStartNode() const;
    GraphNode* getEndNode()   const;
    glm::vec4  getColor()     const;
    float      getSize()      const;

    void setColor(const glm::vec4& color);
    void setWeight(int weight);
    void setDepth(float depth);

    float getDepth()  const;
    int   getWeight() const;
    bool  operator==(const Edge& other) const;

public:
    static constexpr float kDefaultDepth = 0.9f;
private:
    GraphNode* m_nodeStart;
    GraphNode* m_nodeEnd;
    glm::vec4  m_color;
    float      m_size;
    int        m_weight;
    float      m_depth;
};