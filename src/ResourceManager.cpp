#include "ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

std::unordered_map<std::string, Texture2D> ResourceManager::m_textures;
std::unordered_map<std::string, Shader> ResourceManager::m_shaders;

Texture2D ResourceManager::loadTextureFromFile(const char* texturePath)
{
    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    unsigned char* imageData = stbi_load(texturePath, &width, &height, &channels, 0);

    GLenum format;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
    format = GL_RGBA;
    else
    {
        std::cout << "ERROR:TEXTURE2D: invalid texture format at path: " << texturePath << std::endl;
        exit(-1);
    }

    Texture2D texture;
    texture.setImageFormat(format);
    texture.setInternalFormat(format);

    texture.generate(width, height, imageData);

    return texture;
}

Shader ResourceManager::loadShaderFromFile(const char* vertexSourcePath, const char* fragmentSourcePath)
{
    const std::string& vs = readFromPath(vertexSourcePath);
    const std::string& fs = readFromPath(fragmentSourcePath);

    const char* vertexSource    = vs.c_str();
    const char* fragmentSource  = fs.c_str();

    return Shader{vertexSource, fragmentSource};
}

void ResourceManager::loadTexture(const char* texturePath, const std::string& textureName)
{
    if (!m_textures.contains(textureName))
    {
        m_textures[textureName] = loadTextureFromFile(texturePath);
    }
}

void ResourceManager::loadShader(const char* vertexSourcePath, const char* fragmentSourcePath, const std::string& shaderName)
{
    if (!m_shaders.contains(shaderName))
    {
        m_shaders[shaderName] = loadShaderFromFile(vertexSourcePath, fragmentSourcePath);
    }
}

Texture2D& ResourceManager::getTexture(const std::string& textureName)
{
    if (m_textures.contains(textureName))
    {
        return m_textures[textureName];
    }

    std::cout << "ERROR::RESOURCE_MANAGER: no texture with name: " << textureName << std::endl;
    static Texture2D emptyTexture; // Create a static empty texture
    return emptyTexture;
}

Shader& ResourceManager::getShader(const std::string& shaderName)
{
    if (m_shaders.contains(shaderName))
    {
        return m_shaders[shaderName];
    }

    std::cout << "ERROR::RESOURCE_MANAGER: no shader with name: " << shaderName << std::endl;

    static Shader emptyShader; // Create a static empty shader
    return emptyShader;
}

void ResourceManager::clear()
{
    for (const auto& [name, texture] : m_textures)
    {
        unsigned int id = texture.getID();
        glDeleteTextures(1, &id);
        m_textures.erase(name);
    }

    for (const auto& [name, shader] : m_shaders)
    {
        glDeleteProgram(shader.getID());
        m_shaders.erase(name);
    }
}

std::string ResourceManager::readFromPath(const char* path)
{
    std::ifstream fin(path);
    if (!fin.is_open())
    {
        std::cout << "can't open file at: " << path << std::endl;
        exit(-1);
    }

    std::stringstream ss;
    ss << fin.rdbuf();

    return ss.str();
}
