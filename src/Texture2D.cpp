#include "include/Texture2D.h"
#include <iostream>
#include "glad/glad.h"

#include "stb/stb_image.h"

Texture2D::Texture2D()
    : m_ID{0}, m_width{0}, m_height{0}, m_imageFormat{GL_RGB}, m_internalFormat{GL_RGB},
      m_wrapS{GL_REPEAT}, m_wrapT{GL_REPEAT}, m_filterMin{GL_LINEAR}, m_filterMag{GL_LINEAR}
{}

void Texture2D::generate(unsigned int width, unsigned int height, unsigned char* imageData)
{
    m_width = width, m_height = height;

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMag);

    glTexImage2D(GL_TEXTURE_2D, 0, m_imageFormat, width, height, 0, m_internalFormat, GL_UNSIGNED_BYTE, imageData);
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture2D::getID() const
{
    return m_ID;
}

void Texture2D::setImageFormat(unsigned int imageFormat)
{
    m_imageFormat = imageFormat;
}

void Texture2D::setInternalFormat(unsigned int internalFormat)
{
    m_internalFormat = internalFormat;
}
