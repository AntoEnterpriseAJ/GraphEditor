#pragma once
#include "GraphNode.h"
#include "Shader.h"
#include "Edge.h"
#include "TextRenderer.h"

class Renderer
{
public:
	Renderer();

	void render(const GraphNode& node, Shader& nodeShader, Shader& textShader);
	void render(const Edge& edge, Shader& shader) const;
	
private:
	void initRenderData();
private:
	unsigned int m_circleVAO;
	unsigned int m_circleVBO;
	TextRenderer m_textRenderer;
};