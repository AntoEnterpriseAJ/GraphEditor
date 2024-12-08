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

    void renderText(const std::string& text, const Shader& textShader, glm::vec2 pos, bool centered = true
                   , float scale = 1.0f, glm::vec3 color = glm::vec3{ 0.0f, 0.0f, 0.0f });
    void render(GraphNode* node, Shader& nodeShader, Primitive primitive = Primitive::circle);
    void render(const Edge& edge, Shader& shader, bool oriented, bool weighted);
    
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