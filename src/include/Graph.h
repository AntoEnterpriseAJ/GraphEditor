#pragma once
#include "GraphNode.h"
#include "Edge.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Action.h"
#include <stack>
#include <vector>

class Graph
{
public:
    Graph();
    ~Graph();

    void render(Renderer::Primitive nodePrimitive = Renderer::Primitive::circle);
    void handleInput();

    void addNode(GraphNode* node);
	void addEdge(GraphNode* edgeStart, GraphNode* edgeEnd);
    void setOriented(bool oriented);
    void clear();
    void undo();

    void logAdjacencyMatrix(const std::string& fileName) const;
	bool isOriented() const;

private:
    void tryAddEdge(GraphNode* edgeStart, GraphNode* edgeEnd);
    void checkNodeSelect(glm::vec2 position);
    bool checkValidNodePosition(glm::vec2 position) const;
private:
    bool m_oriented;
    std::vector<GraphNode*> m_nodes;
    std::vector<Edge> m_edges;
    Renderer m_renderer;
    std::stack<Action> m_actions;
};