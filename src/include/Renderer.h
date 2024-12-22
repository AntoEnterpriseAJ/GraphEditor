#pragma once
#include "GraphNode.h"
#include "Shader.h"
#include "Edge.h"
#include "TextRenderer.h"

class Renderer
{
public:
    enum class PrimitiveType
    {
        circle,
        quad,
    };

    struct Primitive
    {
        unsigned int  VAO;
        unsigned int  VBO;

        Primitive() : VAO{0}, VBO{0} {};
    };

public:

    Renderer();

    void renderText(const std::string& text, const Shader& textShader, glm::vec2 pos, bool centered = true
                   , float scale = 1.0f, glm::vec3 color = glm::vec3{ 0.0f, 0.0f, 0.0f });
    void render(GraphNode* node, Shader& nodeShader, PrimitiveType primitive = PrimitiveType::circle);
    void render(const Edge& edge, Shader& shader, bool oriented, bool weighted);

    void addNodeToBatch(const GraphNode* node);
    void clearNodeBatch();
    void nodeInstanceRender(Shader& shader);

    void addEdgeToBatch(const Edge& edge);
    void clearEdgeBatch();
    void edgeInstanceRender(Shader& shader);

private:
    void initPrimitivesData();
    void initCircleData();
    void initQuadData();
    void initLineData();
private:
    TextRenderer m_textRenderer;
    Primitive    m_circle;
    Primitive    m_quad;
    Primitive    m_line;

    unsigned int m_linesDataVBO;
    std::vector<float> m_linesData;

    unsigned int m_linesColorVBO;
    std::vector<float> m_linesColor;

    unsigned int m_circleTranslationsVBO;
    std::vector<glm::mat4> m_circleTranslations;
};