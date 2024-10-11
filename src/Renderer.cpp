#include "Renderer.h"
#include <vector>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

Renderer::Renderer()
	: m_circleVAO{ 0 }, m_circleVBO{ 0 }, m_textRenderer{ "res/fonts/Astron.otf", 40 }
{
    initRenderData();
}

void Renderer::render(const GraphNode& node, Shader& nodeShader, Shader& textShader)
{
    nodeShader.bind();

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(node.getPosition(), 1.0f));
	model = glm::scale(model, glm::vec3(node.getSize(), 1.0f));

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    nodeShader.bind();
    nodeShader.setMat4("model", model);
    nodeShader.setMat4("projection", projection);

	glBindVertexArray(m_circleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 90);

	std::string nodeID = std::to_string(node.getID());
	float textWidth = m_textRenderer.calculateTextWidth(nodeID);
	float textHeight = m_textRenderer.calculateTextHeight(nodeID);

    float centeredX = node.getPosition().x - (textWidth / 2.0f);
    float centeredY = node.getPosition().y - (textHeight / 2.0f);

	m_textRenderer.RenderText(textShader, std::to_string(node.getID()), centeredX, centeredY, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	glBindVertexArray(0);
}

void Renderer::render(const Edge& edge, Shader& shader) const
{
    shader.bind();

    glm::mat4 model(1.0f);

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    shader.setMat4("model", model);
    shader.setMat4("projection", projection);

    glm::vec2 edgeStart{ edge.getStartNode().getPosition().x, edge.getStartNode().getPosition().y};
    glm::vec2 edgeEnd{ edge.getEndNode().getPosition().x, edge.getEndNode().getPosition().y };

    glm::vec2 dir = glm::normalize(edgeEnd - edgeStart);

    edgeStart += dir * edge.getStartNode().getSize();
    edgeEnd   += -dir * edge.getStartNode().getSize();

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

    glDeleteBuffers(1, &edgeVBO);
    glDeleteVertexArrays(1, &edgeVAO);

    glm::vec3 arrow1 = glm::vec3{ dir, 0.0f };
    glm::vec3 arrow2 = glm::vec3{ dir, 0.0f };

    float aSide = edge.getEndNode().getPosition().x - edge.getStartNode().getPosition().x;
    float bSide = edge.getEndNode().getPosition().y - edge.getStartNode().getPosition().y;
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
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);

    glDeleteBuffers(1, &arrowsVBO);
    glDeleteVertexArrays(1, &arrowsVAO);
    glLineWidth(1.0f);
}

void Renderer::initRenderData()
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
