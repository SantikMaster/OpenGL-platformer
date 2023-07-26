#include "shader.h"

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    GLuint vertexShader, fragmentShader;
    GLenum error;



    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation status (omitted for brevity)

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation status (omitted for brevity)

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);

    glLinkProgram(programID);

    // Check for shader program linking status (omitted for brevity)
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    glDetachShader(programID, vertexShader);
    glDetachShader(programID, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(programID);
    // Check for shader program linking status (omitted for brevity)

}

void Shader::use()
{
    glUseProgram(programID);
}

void Shader::setMat4(const char* name, const glm::mat4& matrix) {
    glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const char* name, const glm::vec3& vector) {
    glUniform3fv(glGetUniformLocation(programID, name), 1, glm::value_ptr(vector));
}

void Shader::setFloat(const char* name, float value) {
    glUniform1f(glGetUniformLocation(programID, name), value);
}

GLuint Shader::getProgramID() const
{
    return programID;
}
