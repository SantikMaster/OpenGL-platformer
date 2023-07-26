#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"



class Object;
class Shader;

class WorldManager
{
    GLint modelLoc;
    GLint viewLoc;
    GLint projectionLoc;
    GLint lightDirLoc;
    GLint lightColorLoc;
    GLint ambientStrengthLoc;
    GLint diffuseStrengthLoc;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    std::unique_ptr <Shader> shader;
    std::unique_ptr<Shader> shader2;
public:
    std::shared_ptr<Object> sphere;
    std::shared_ptr<Object> cube;

    WorldManager(int, int);
    void Update();
    void Draw();

  //  char* vertexShaderSource = VertexShaderSource;
  //  char* fragmentShaderSource = FragmentShaderSource;
   // char* vertexShaderSource_sphere = VertexShaderSource_sphere;
   // char* fragmentShaderSource_sphere = FragmentShaderSource_sphere;
};

