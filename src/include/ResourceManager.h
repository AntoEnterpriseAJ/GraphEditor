#pragma 
#include "Texture2D.h"
#include "Shader.h"
#include <unordered_map>
#include <string>

class ResourceManager
{
public:
    static void loadTexture(const char* texturePath, const std::string& textureName);
    static void loadShader(const char* vertexSourcePath, const char* fragmentSourcePath, const std::string& shaderName);

    static Texture2D& getTexture(const std::string& textureName);
    static Shader& getShader(const std::string& shaderName);

    static void clear();

private:
    ResourceManager();
    static std::string readFromPath(const char* path);
    static Texture2D loadTextureFromFile(const char* texturePath);
    static Shader loadShaderFromFile(const char* vertexSourcePath, const char* fragmentSourcePath);
private:
    static std::unordered_map<std::string, Texture2D> m_textures;
    static std::unordered_map<std::string, Shader> m_shaders;
};