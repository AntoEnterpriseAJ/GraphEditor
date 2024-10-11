#pragma once
#include "GraphNode.h"
#include "Edge.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include <vector>

class Graph
{
public:
	Graph();

	void render();

	void addNode(const GraphNode& node);
	void logAdjencyMatrix(const std::string& fileName);
private:
	void handleInput();
	void tryAddEdge(GraphNode& edgeStart, GraphNode& edgeEnd);
	void checkNodeSelect(glm::vec2 position);
	bool checkValidNodePosition(glm::vec2 position) const;
private:
	bool m_oriented;
	std::vector<GraphNode> m_nodes;
	std::vector<Edge> m_edges;
	Renderer m_renderer;
};