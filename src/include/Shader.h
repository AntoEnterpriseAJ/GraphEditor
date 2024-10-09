#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>


class Shader
{
public:
    Shader();
    Shader(const char* vertexSource, const char* fragmentSource);


    int getUniformLocation(const std::string& name) const;
    unsigned int getID() const;
    void bind() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec4(const std::string& name, glm::vec4 vec) const;
    void setVec3(const std::string& name, glm::vec3 vec) const;
    void setVec2(const std::string& name, glm::vec2 vec) const;
    void setVec4(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    const char* readFromPath(const char* path);
private:
    unsigned int m_ID;
    std::unordered_map<std::string, int> m_locationCache;
};
