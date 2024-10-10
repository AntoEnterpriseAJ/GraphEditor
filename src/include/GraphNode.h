#pragma once
#include "glm/glm.hpp"
#include "Shader.h"

class GraphNode
{
public:
	GraphNode(glm::vec2 position, glm::vec2 size = { 30.0f, 30.0f }, glm::vec3 color = { 1.0f, 0.0f, 0.0f });

	glm::vec2 getPosition() const;
	glm::vec2 getSize() const;

private:
	glm::vec2 m_position;
	glm::vec2 m_size;
	glm::vec3 m_color;
};	