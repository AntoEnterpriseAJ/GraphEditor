#pragma once
#include "GraphNode.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer();

	void render(const GraphNode& node, Shader& shader) const;
	
private:
	void initRenderData();
private:
	unsigned int m_circleVAO;
	unsigned int m_circleVBO;
};