#pragma once
#include "glm/glm.hpp"

class Object
{
public:
	Object(const glm::vec2& position, const glm::vec2& size);

	const glm::vec2& getPosition();
	const glm::vec2& getSize();

private:
	glm::vec2 m_position;
	glm::vec2 m_size;
};