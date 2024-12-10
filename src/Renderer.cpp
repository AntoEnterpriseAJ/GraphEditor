#include "Renderer.h"
#include <vector>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#include "ResourceManager.h"

Renderer::Renderer()
    : m_circleVAO{ 0 }, m_circleVBO{ 0 }, m_textRenderer{ "res/fonts/Astron.otf", 40 }
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

void Renderer::render(GraphNode* node, Shader& nodeShader, Primitive primitive)
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

    if (primitive == Primitive::circle)
    {
        nodeShader.setVec4("color", node->getColor());
        glBindVertexArray(m_circleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 90);
    }
    else if (primitive == Primitive::quad)
    {
        nodeShader.setVec4("color", node->getColor());
        glBindVertexArray(m_quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
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
    glLineWidth(6.0f);
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
        glLineWidth(6.0f);
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

void Renderer::initPrimitivesData()
{
    initCircleData();
    initQuadData();
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

    glGenVertexArrays(1, &m_circleVAO);
    glBindVertexArray(m_circleVAO);

    glGenBuffers(1, &m_circleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_circleVBO);
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

    glGenVertexArrays(1, &m_quadVAO);
    glBindVertexArray(m_quadVAO);

    glGenBuffers(1, &m_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}
