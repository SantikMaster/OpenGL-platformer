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

    std::unique_ptr <Shader> ShaderSphere;
    std::unique_ptr<Shader> ShaderCube;
public:
    sf::Clock Timer;
    sf::Clock MapTimer;

    bool EndingGame = false;
    const int MaxGroundHeight = 200;
    const int MinGroundHeight = -300;

    float SphereVelocityX = 0;
    float SpheredeltaVelocityX = 0.002;

    float SphereVelocityZ = 0;
    float SpherePushVelocityZ = 5;
    float SpheredeltaVelocityZ = 0.05;
    int GroundHeight = 100;
    int Score = 0;

    int TimePenalty = 0;
    bool IsSphereColliding = false;

    std::string myfontFileName = "../Textures/Arial.ttf";
    std::string myImageFileName = "../Textures/Background.jpg";
    sf::Font Font;
    sf::Text Text;
    sf::Image backgroundImage;

    void GenerateMap();
    void SphereJump();
    bool Collide(int FutureX, int FutureZ);
    void DrawEndBanner(sf::RenderWindow *Window);
    void DrawBackground(sf::RenderWindow* Window);
    void Restart();

    std::shared_ptr<Object> Sphere;
    std::deque<std::shared_ptr<Object>> CubeQue;

    WorldManager();
    void Update(int sizeX, int sizeY);
    void Draw();
};

