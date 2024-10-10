#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Texture2D.h"
#include "glm/glm.hpp"
#include "Shader.h"
#include <unordered_map>

class TextRenderer
{
public:
    struct CharacterData
    {
        unsigned int texture;
        unsigned int advance;
        glm::ivec2   size;
        glm::ivec2   bearing;
    };

    TextRenderer(const char* fontPath, unsigned int fontSize);
    ~TextRenderer();

    void RenderText(const Shader& shader, const std::string& text, float x, float y, float scale = 1.0f, glm::vec3 color = {1.0f, 1.0f, 1.0f});

private:
    void initRenderData();
private:
    FT_Library m_FT;
    FT_Face    m_Face;

    unsigned int m_quadVAO;
    unsigned int m_quadVBO;

    std::unordered_map<char, CharacterData> m_characterData;
};