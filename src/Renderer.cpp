#include "Renderer.h"
#include <vector>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

Renderer::Renderer()
	: m_circleVAO{ 0 }, m_circleVBO{ 0 }
{
    initRenderData();
}

void Renderer::render(const GraphNode& node, Shader& shader) const
{
    shader.bind();

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(node.getPosition(), 1.0f));
	model = glm::scale(model, glm::vec3(node.getSize(), 1.0f));

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    shader.bind();
    shader.setMat4("model", model);
    shader.setMat4("projection", projection);

	glBindVertexArray(m_circleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 90);
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
