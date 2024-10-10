#include "Graph.h"
#include "ResourceManager.h"

Graph::Graph()
	: m_nodes{}, m_renderer{}
{
	ResourceManager::loadShader("res/shaders/circle.vert", "res/shaders/circle.frag", "circle");
}

void Graph::render()
{
	for (const auto& node : m_nodes)
	{
		m_renderer.render(node, ResourceManager::getShader("circle"));
	}
}

void Graph::addNode(const GraphNode& node)
{
	m_nodes.push_back(node);
}
