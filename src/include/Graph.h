#pragma once
#include "GraphNode.h"
#include "Renderer.h"

class Graph
{
public:
	Graph();

	void render();

	void addNode(const GraphNode& node);
private:
	void handleInput();
	bool checkValidNodePosition(glm::vec2 position) const;
private:
	std::vector<GraphNode> m_nodes;
	Renderer m_renderer;
};