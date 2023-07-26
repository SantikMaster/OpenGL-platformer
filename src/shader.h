#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader
{
public:
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader();
    void use();
    void setMat4(const char* name, const glm::mat4& matrix);
    void setVec3(const char* name, const glm::vec3& vector);
    void setFloat(const char* name, float value);
    GLuint getProgramID() const;

private:
    GLuint programID;
};

