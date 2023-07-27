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

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    std::unique_ptr <Shader> shader;
    std::unique_ptr<Shader> shader2;
public:
    sf::Clock timer;
    sf::Clock MapTimer;

    float SphereVelocityX = 0;
    float SpheredeltaVelocityX = 0.002;

    float SphereVelocityZ = 0;
    float SpherePushVelocityZ = 5;
    float SpheredeltaVelocityZ = 0.05;
    int GroundHeight = 100;

    int TimePenalty = 0;
    bool IsSphereColliding = false;

    void GenerateMap();
    void SphereJump();
    bool Collide(int FutureX, int FutureZ);

    std::shared_ptr<Object> sphere;
    std::deque<std::shared_ptr<Object>> cube_que;

    WorldManager();
    void Update(int sizeX, int sizeY);
    void Draw();
};

