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

    void render();

    void addNode(GraphNode* node);
	void addEdge(GraphNode* edgeStart, GraphNode* edgeEnd);
    void logAdjacencyMatrix(const std::string& fileName);
    void clear();
    void undo();

private:
    void handleInput();
    void tryAddEdge(GraphNode* edgeStart, GraphNode* edgeEnd);
    void checkNodeSelect(glm::vec2 position);
    bool checkValidNodePosition(glm::vec2 position) const;
    void renderUI();
private:
    bool m_oriented;
    std::vector<GraphNode*> m_nodes;
    std::vector<Edge> m_edges;
    Renderer m_renderer;
    std::stack<Action> m_actions;
};