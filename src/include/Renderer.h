#pragma once
#include "GraphNode.h"
#include "Shader.h"
#include "Edge.h"
#include "TextRenderer.h"

class Renderer
{
public:
    enum class Primitive
    {
        circle,
        quad,
    };

    Renderer();

    void render(GraphNode* node, Shader& nodeShader, Shader& textShader, Primitive primitive = Primitive::circle);
    void render(const Edge& edge, Shader& shader, bool oriented) const;
    
private:
    void initPrimitivesData();
    void initCircleData();
    void initQuadData();
private:
    TextRenderer m_textRenderer;
    unsigned int m_circleVAO;
    unsigned int m_circleVBO;
    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
};