#include "Renderer.h"
#include <vector>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#include "ResourceManager.h"

#include <thread>

Renderer::Renderer()
    : m_circle{}, m_quad{}, m_textRenderer{ "res/fonts/Astron.otf", 40 }
{
    initPrimitivesData();
}

void Renderer::renderText(const std::string& text, const Shader& textShader, glm::vec2 pos
                         , bool centered, float scale, glm::vec3 color)
{
    if (centered)
    {
        float textWidth = m_textRenderer.calculateTextWidth(text, scale);
        float textHeight = m_textRenderer.calculateTextHeight(text, scale);

        float centeredX = pos.x - textWidth / 2.0f;
        float centeredY = pos.y - textHeight / 2.0f;
            
        m_textRenderer.RenderText(textShader, text, centeredX, centeredY, scale, color);
    }
    else
    {
        m_textRenderer.RenderText(textShader, text, pos.x, pos.y, scale, color);
    }
}

void Renderer::render(const Edge& edge, Shader& shader, bool oriented, bool weighted)
{
    shader.bind();

    glm::mat4 model(1.0f);

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    shader.setMat4("model", model);
    shader.setMat4("projection", projection);
    shader.setVec4("color", edge.getColor());

    glm::vec2 edgeStart{ edge.getStartNode()->getPosition().x, edge.getStartNode()->getPosition().y};
    glm::vec2 edgeEnd{ edge.getEndNode()->getPosition().x, edge.getEndNode()->getPosition().y };

    glm::vec2 dir = glm::normalize(edgeEnd - edgeStart);

    edgeStart += dir * edge.getStartNode()->getSize();
    edgeEnd   += -dir * edge.getStartNode()->getSize();

    std::vector<float> edgeVertices = {
        edgeStart.x, edgeStart.y,
        edgeEnd.x, edgeEnd.y,
    };

    GLuint edgeVAO, edgeVBO;
    glGenVertexArrays(1, &edgeVAO);
    glBindVertexArray(edgeVAO);

    glGenBuffers(1, &edgeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, edgeVBO);
    glBufferData(GL_ARRAY_BUFFER, edgeVertices.size() * sizeof(float), edgeVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(edgeVAO);
    glLineWidth(edge.getSize());
    glDrawArrays(GL_LINES, 0, 2);
    glLineWidth(1.0f);

    glDeleteBuffers(1, &edgeVBO);
    glDeleteVertexArrays(1, &edgeVAO);

    if (oriented)
    {
        glm::vec3 arrow1 = glm::vec3{ dir, 0.0f };
        glm::vec3 arrow2 = glm::vec3{ dir, 0.0f };

        float aSide = edge.getEndNode()->getPosition().x - edge.getStartNode()->getPosition().x;
        float bSide = edge.getEndNode()->getPosition().y - edge.getStartNode()->getPosition().y;
        float alpha = glm::atan(bSide / aSide);

        arrow1 = glm::rotate(arrow1, glm::radians(-135.0f + alpha), glm::vec3(0.0f, 0.0f, 1.0f));
        arrow2 = glm::rotate(arrow2, glm::radians(135.0f - alpha), glm::vec3(0.0f, 0.0f, 1.0f));

        constexpr float arrowLength = 20.0f;
        arrow1 *= arrowLength;
        arrow2 *= arrowLength;

        float arrowsVertices[] = {
            edgeEnd.x, edgeEnd.y,
            edgeEnd.x + arrow1.x, edgeEnd.y + arrow1.y,
            edgeEnd.x, edgeEnd.y,
            edgeEnd.x + arrow2.x, edgeEnd.y + arrow2.y
        };

        GLuint arrowsVAO, arrowsVBO;
        glGenVertexArrays(1, &arrowsVAO);
        glBindVertexArray(arrowsVAO);

        glGenBuffers(1, &arrowsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, arrowsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(arrowsVertices), arrowsVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(arrowsVAO);
        glLineWidth(edge.getSize());
        glDrawArrays(GL_LINES, 0, 4);
        glLineWidth(1.0f);
        glBindVertexArray(0);

        glDeleteBuffers(1, &arrowsVBO);
        glDeleteVertexArrays(1, &arrowsVAO);
    }

    if (weighted)
    {
        glm::vec2 textPos = (edgeStart + edgeEnd) / 2.0f;
        if (oriented)
        {
            float length = glm::distance(edgeStart, edgeEnd);
            textPos += dir * length / 4.0f;
        }

        renderText(std::to_string(edge.getWeight()), ResourceManager::getShader("text"), textPos);
    }
}

void Renderer::render(GraphNode* node, Shader& nodeShader, PrimitiveType primitive)
{
    nodeShader.bind();

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(node->getPosition(), 1.0f));
    model = glm::scale(model, glm::vec3(node->getSize(), 1.0f));

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    nodeShader.bind();
    nodeShader.setMat4("model", model);
    nodeShader.setMat4("projection", projection);

    if (primitive == PrimitiveType::circle)
    {
        nodeShader.setVec4("color", node->getColor());
        glBindVertexArray(m_circle.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 90);
    }
    else if (primitive == PrimitiveType::quad)
    {
        nodeShader.setVec4("color", node->getColor());
        glBindVertexArray(m_quad.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
}

void Renderer::addNodeToBatch(const GraphNode* node)
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(node->getPosition(), 1.0f));
    model = glm::scale(model, glm::vec3(node->getSize(), 1.0f));
    m_circleTranslations.push_back(model); // Use move semantics only if the data is expensive to copy.
}


void Renderer::clearNodeBatch()
{
    m_circleTranslations.clear();
}

void Renderer::nodeInstanceRender(Shader& shader)
{
    glBindVertexArray(m_circle.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_circleTranslationsVBO);
    glBufferData(GL_ARRAY_BUFFER, m_circleTranslations.size() * sizeof(glm::mat4), m_circleTranslations.data(), GL_DYNAMIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    shader.bind();
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    shader.setMat4("projection", projection);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 90, static_cast<GLsizei>(m_circleTranslations.size()));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::addEdgeToBatch(const Edge& edge)
{
    glm::vec2 startPos = edge.getStartNode()->getPosition();
    glm::vec2 endPos = edge.getEndNode()->getPosition();

    m_linesData.push_back(startPos.x);
    m_linesData.push_back(startPos.y);
    m_linesData.push_back(endPos.x);
    m_linesData.push_back(endPos.y);
}


void Renderer::clearEdgeBatch()
{
    m_linesData.clear();
}

void Renderer::edgeInstanceRender(Shader& shader)
{
    glBindVertexArray(m_line.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_line.VBO);
    glBufferData(GL_ARRAY_BUFFER, m_linesData.size() * sizeof(float), m_linesData.data(), GL_STATIC_DRAW);

    glm::mat4 model{1.0f};

    shader.bind();
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    shader.setMat4("projection", projection);
    shader.setMat4("model", model);

    glLineWidth(0.1f);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_linesData.size() / 2));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::initPrimitivesData()
{
    glGenBuffers(1, &m_circleTranslationsVBO);
    glGenBuffers(1, &m_linesDataVBO);

    initCircleData();
    initQuadData();
    initLineData();
}


void Renderer::initCircleData()
{
    std::vector<float> circleVertices;
    constexpr int circleVerticesCount = 30;
    constexpr float circleRadius = 1.0f;
    float angle = 0.0f;

    for (int i = 0; i < circleVerticesCount; i += 1) {
        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);
        circleVertices.push_back(circleRadius * glm::cos(glm::radians(angle)));
        circleVertices.push_back(circleRadius * glm::sin(glm::radians(angle)));

        angle += 360.0f / circleVerticesCount;

        circleVertices.push_back(circleRadius * glm::cos(glm::radians(angle)));
        circleVertices.push_back(circleRadius * glm::sin(glm::radians(angle)));
    }

    glGenVertexArrays(1, &m_circle.VAO);
    glBindVertexArray(m_circle.VAO);

    glGenBuffers(1, &m_circle.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_circle.VBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::initQuadData()
{
    float quadData[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0
    };

    glGenVertexArrays(1, &m_quad.VAO);
    glBindVertexArray(m_quad.VAO);

    glGenBuffers(1, &m_quad.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Renderer::initLineData()
{
    float lineData[] = {
        0.0f, 0.0f,
        1.0f, 1.0f 
    };

    glGenVertexArrays(1, &m_line.VAO);
    glBindVertexArray(m_line.VAO);

    glGenBuffers(1, &m_line.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_line.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineData), lineData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
