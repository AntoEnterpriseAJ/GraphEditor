#include "Object.h"

Object::Object(const glm::vec2& position, const glm::vec2& size)
	: m_position{position}, m_size{size}
{}

const glm::vec2& Object::getPosition()
{
	return m_position;
}

const glm::vec2& Object::getSize()
{
	return m_size;
}
