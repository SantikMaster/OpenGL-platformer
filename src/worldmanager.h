#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include <deque>

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

 //   glm::vec3 cameraPos;
 //   glm::vec3 cameraTarget;
 //   glm::vec3 cameraUp;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    std::unique_ptr <Shader> shader;
    std::unique_ptr<Shader> shader2;
public:
    std::shared_ptr<Object> sphere;
  //  std::shared_ptr<Object> cube;
    std::deque<std::shared_ptr<Object>> cube_que;

    WorldManager();
    void Update(int sizeX, int sizeY);
    void Draw();
};

