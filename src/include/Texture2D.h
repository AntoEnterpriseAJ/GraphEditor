#pragma once
#include <string>

class Texture2D
{
public:
    Texture2D();

    void generate(unsigned int width, unsigned int height, unsigned char* imageData);

    unsigned int getID() const;
    void bind() const;
    void unbind() const;

    void setImageFormat(unsigned int imageFormat);
    void setInternalFormat(unsigned int internalFormat);

private:
    unsigned int m_ID;
    unsigned int m_width, m_height;
    unsigned int m_imageFormat, m_internalFormat;
    unsigned int m_wrapS, m_wrapT;
    unsigned int m_filterMin, m_filterMag;
};