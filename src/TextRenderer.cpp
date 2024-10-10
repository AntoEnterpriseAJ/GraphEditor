#include "TextRenderer.h"

TextRenderer::TextRenderer(const char* fontPath, unsigned int fontSize)
    : m_quadVAO{0}, m_quadVBO{0}
{
    initRenderData();
    
    if (FT_Init_FreeType(&m_FT))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        exit(-1);
    }

    if (FT_New_Face(m_FT, fontPath, 0, &m_Face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        exit(-1);
    }
    else
    {
        FT_Set_Pixel_Sizes(m_Face, 0, fontSize);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char character = 0; character < 128; character++)
        {
            if (FT_Load_Char(m_Face, character, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                m_Face->glyph->bitmap.width,
                m_Face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                m_Face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            m_characterData[character] = CharacterData{
                texture,
                static_cast<unsigned int>(m_Face->glyph->advance.x),
                glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
                glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
            };
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    FT_Done_Face(m_Face);
    FT_Done_FreeType(m_FT);
}

TextRenderer::~TextRenderer()
{
    for (const auto& [character, charData] : m_characterData)
    {
        glDeleteTextures(1, &charData.texture);
    }
}

void TextRenderer::RenderText(const Shader& shader, const std::string& text, float x, float y, float scale, glm::vec3 color)
{
    shader.bind();
    shader.setVec3("textColor", color);
    
    glm::mat4 orthographicProjection(float l, float r, float b, float t, float n, float f);
    glm::mat4 ortho = orthographicProjection(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    shader.setMat4("projection", ortho);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_quadVAO);

    for (const auto& character : text)
    {
        CharacterData charData = m_characterData[character];

        float xpos = x + charData.bearing.x * scale;
        float ypos = y - (charData.size.y - charData.bearing.y) * scale;

        float w = charData.size.x * scale;
        float h = charData.size.y * scale;

        glm::mat4 model{ 1.0f };
        model = glm::translate(model, glm::vec3{ xpos, ypos, 0.0f });
        model = glm::scale(model, glm::vec3{ w, h, 0.0f });
        shader.setMat4("model", model);

        glBindTexture(GL_TEXTURE_2D, charData.texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (charData.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::initRenderData()
{
    float quadData[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glBindVertexArray(m_quadVAO);

    glGenBuffers(1, &m_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}
