#pragma once
#include "glm/glm.hpp"
#include "Shader.h"

class GraphNode
{
public:
    GraphNode();
    GraphNode(glm::vec2 position, const std::string& text, unsigned int internalID = 0
             , glm::vec2 size = { 30.0f, 30.0f }, glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f});

    void setPosition(glm::vec2 position);

    std::string getText() const;
    glm::vec2 getPosition() const;
    glm::vec2 getSize() const;
    glm::vec4 getColor() const;
    unsigned int getInternalID() const;

private:
    std::string m_text;
    unsigned int m_internalID;
    glm::vec2 m_position;
    glm::vec2 m_size;
    glm::vec4 m_color;
};	