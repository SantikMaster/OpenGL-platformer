#include "worldmanager.h"
#include "shader.h"
#include "object.h"

#include <iostream>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



const char* VertexShaderSource_sphere = R"(
    #version 440 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    
    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform float uRotationAngle;



    out vec3 FragNormal;
    out vec3 FragPos;

    void main() 
{

        FragNormal = mat3(transpose(inverse(uModel))) * aNormal;
        FragPos = vec3(uModel * vec4(aPos, 1.0));
        gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    }
)";


const char* FragmentShaderSource_sphere = R"(
#version 440 core

in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 uLightColor;
uniform vec3 uLightDirection;  // Added light direction uniform
uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uRotationAngle;   // Uniform to pass the rotation angle from the CPU
uniform vec3 uSphereCenter;     // Uniform to pass the center Position of the sphere
uniform vec3 uSphereRotation;   // Uniform to pass the axis of rotation for the sphere

void main() {
    vec3 ambient = uAmbientStrength * uLightColor;

    // Calculate the light direction in view space
    vec3 lightDir = normalize(uLightDirection);  // Use the light direction uniform
//    vec3 lightDir2 = normalize(vec3(1,0,0));  // U

    float diff = max(dot(normalize(FragNormal), lightDir), 0.0);

    // Increase the blue component of the light color to make it more blue
    vec3 blueLightColor = vec3(uLightColor.x * 0.2, uLightColor.y * 0.2, uLightColor.z * 1.0);

    vec3 diffuse = uDiffuseStrength * diff * blueLightColor * 1.5;

    // Calculate the uniform rotation angle using the absolute value of uSphereRotation
    float uniformRotationAngle = uRotationAngle * length(uSphereRotation);

    // Calculate the vector from the fragment's position to the sphere's center
    vec3 centerToFrag = FragPos - uSphereCenter;

    // Rotate the vector representing the fragment's position around the Y-axis
    float cosAngle = cos(uniformRotationAngle);
    float sinAngle = sin(uniformRotationAngle);
    vec3 rotatedPos = vec3(
        cosAngle * centerToFrag.x - sinAngle * centerToFrag.z + uSphereCenter.x,
        FragPos.y,
        sinAngle * centerToFrag.x + cosAngle * centerToFrag.z + uSphereCenter.z
    );

    // Calculate the distance of the rotated position from the Y-axis (to check for rotation)
    float rotatedDistanceFromYAxis = abs(rotatedPos.z - uSphereCenter.z);

    // Define the width of the stripe
    float stripeWidth = 10.0; // Adjust the value based on your desired width

    // Check if the distance from the Z-axis is within the stripe width
    if (rotatedDistanceFromYAxis <= stripeWidth * 0.5) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color for the rotating stripe
    } else {
        FragColor = vec4(ambient + diffuse, 1.0);
    }
}



)";


const char* VertexShaderSource = R"(
#version 440 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    
    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform float uRotationAngle;



    out vec3 FragNormal;
    out vec3 FragPos;

    void main() 
{

        FragNormal = mat3(transpose(inverse(uModel))) * aNormal;
        FragPos = vec3(uModel * vec4(aPos, 1.0));
        gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    }
)";


// Fragment Shader Source
const char* FragmentShaderSource = R"(
#version 440 core

in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 uLightColor;
uniform vec3 uLightDirection;  // Added light direction uniform
uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uRotationAngle;   // Uniform to pass the rotation angle from the CPU
uniform vec3 uSphereCenter;     // Uniform to pass the center position of the sphere
uniform vec3 uSphereRotation;   // Uniform to pass the axis of rotation for the sphere

void main() 
{

    vec3 greenColor = vec3(0.1, 1, 0.1);
    // Calculate the Lambertian lighting intensity
    float diffuseStrength = max(dot(normalize(FragNormal), vec3(0,-2,2)), 0.0);

    // Set the final color as the diffuse color of the cube multiplied by the lighting intensity
    vec3 diffuse = greenColor * diffuseStrength*4;

    vec3 ambient = uAmbientStrength * vec3(0.3, 0.8, 0.8)*4;
    vec3 finalColor = ambient + diffuse;


    // Set the alpha component to 1.0 for full opacity
    FragColor = vec4(finalColor, 0.5);
}
)";
WorldManager::WorldManager()
{
    ShaderSphere = std::make_unique<Shader>(VertexShaderSource_sphere, FragmentShaderSource_sphere);
    ShaderCube = std::make_unique<Shader>(VertexShaderSource, FragmentShaderSource);

    modelLoc = glGetUniformLocation(ShaderSphere->getProgramID(), "uModel");
    viewLoc = glGetUniformLocation(ShaderSphere->getProgramID(), "uView");
    projectionLoc = glGetUniformLocation(ShaderSphere->getProgramID(), "uProjection");
    lightDirLoc = glGetUniformLocation(ShaderSphere->getProgramID(), "uLightDirection");
    lightColorLoc = glGetUniformLocation(ShaderSphere->getProgramID(), "uLightColor");
    ambientStrengthLoc = glGetUniformLocation(ShaderSphere->getProgramID(), "uAmbientStrength");
    diffuseStrengthLoc = glGetUniformLocation(ShaderSphere->getProgramID(), "uDiffuseStrength");

    ShaderSphere->use();

    Sphere = std::make_shared<Object>(50.0f);

    Restart();
}

void WorldManager::Restart()
{
    srand(time(0));
    if (Font.loadFromFile(myfontFileName))
    {
        std::cout << "font loaded";
    }

    CubeQue.clear();
    int i = 0;
    for (i = 0; i < 20; i++)
    {
        std::shared_ptr<Object> obj = std::make_shared<Object>(100.0f, 1);
        obj->SetPosition(glm::vec3(100.0f * i - 400, 0.0f, 0.0f));
        CubeQue.push_back(obj);
    }

    Sphere->SetPosition(glm::vec3(00.0f, 00.0f, 150.0f));
    Score = 0;
    EndingGame = false;
}

void WorldManager::GenerateMap()
{
    auto back_element = CubeQue.back();

    int val = 110*::rand() / RAND_MAX;
    std::shared_ptr<Object> obj = std::make_shared<Object>(100.0f, 1);

    if (val < 50)
    {

        obj->SetPosition(glm::vec3(back_element->Position.x + Object::CUBE_EDGE, 0, back_element->Position.z));
    }
    else if (val < 75)
    {
        if (back_element->Position.z > MinGroundHeight)
        {
            obj->SetPosition(glm::vec3(back_element->Position.x + Object::CUBE_EDGE, 0, back_element->Position.z - Object::CUBE_EDGE));
        }
        else
        {
            obj->SetPosition(glm::vec3(back_element->Position.x + Object::CUBE_EDGE, 0, MinGroundHeight));
        }
    }
    else if (val < 100)
    {
        if (back_element->Position.z < MaxGroundHeight)
        {
            obj->SetPosition(glm::vec3(back_element->Position.x + Object::CUBE_EDGE, 0, back_element->Position.z + Object::CUBE_EDGE));
        }
        else
        {
            obj->SetPosition(glm::vec3(back_element->Position.x + Object::CUBE_EDGE, 0, MaxGroundHeight));
        }
    }
    else
    {
        obj->SetPosition(glm::vec3(back_element->Position.x + 200, 0, back_element->Position.z));
    }
    CubeQue.push_back(obj);
    CubeQue.pop_front();
}

bool WorldManager::Collide(int FutureX, int FutureZ) // 
{
    bool collide = false;
    for (auto it = CubeQue.begin(); it != CubeQue.end(); ++it)
    {       
        if ((FutureX >= it->get()->Position.x - 100) &&
            (FutureX < it->get()->Position.x) )
        {
            if ((FutureZ >= it->get()->Position.z) &&
                (FutureZ < it->get()->Position.z + 99))
            {
                return true;
            }
        }
    }

    return collide;
}
void WorldManager::SphereJump() // 
{
    if (Collide(Sphere->Position.x, Sphere->Position.z - 10))
        SphereVelocityZ += SpherePushVelocityZ;
}
void WorldManager::Update(int sizeX, int sizeY) // 
{
    if (Timer.getElapsedTime().asMilliseconds() >= 10)
    {
        
        Sphere->SetRotationAngle(Sphere->RotationAngle + SphereVelocityX);
    
        int TempX = Sphere->Position.x + SphereVelocityX * 500;
        int TempZ = Sphere->Position.z + SphereVelocityZ;

        if (!Collide(Sphere->Position.x, TempZ))
        {
          
            Sphere->Position.z = TempZ;
            SphereVelocityZ -= SpheredeltaVelocityZ;
            IsSphereColliding = false;
        }
        else
        {
            IsSphereColliding = true;
            SphereVelocityZ *= -0.1;
        }

        if (!Collide(TempX, Sphere->Position.z+10))
        {
            Sphere->Position.x = TempX;
        }
        else
        {
            IsSphereColliding = true;
            SphereVelocityX *= -0.3;
        }

        if (Sphere->Position.z < -500) EndingGame = true;

        Timer.restart();
    }
    if (MapTimer.getElapsedTime().asMilliseconds() >= 700- TimePenalty)
    {
        GenerateMap();
        MapTimer.restart();
        TimePenalty++;
        Score++;
    }

    glm::vec3 cameraPos(-300.0f+ Sphere->Position.x , -400.0f, 200.f);
    glm::vec3 cameraTarget(Sphere->Position.x, 0.f, 0.f);
    glm::vec3 cameraUp(0.f, 0.1f, 0.8f);

    viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(sizeX) / sizeY, 1.f, 2000.f);
    glLoadMatrixf(glm::value_ptr(projectionMatrix));

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glm::vec3 lightDirection(-1.f, -1.f, 0.f);
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirection));

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // White light color
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glUniform1f(ambientStrengthLoc, 0.2f);
    glUniform1f(diffuseStrengthLoc, 0.7f);
}

void WorldManager::Draw()
{
    ShaderSphere->use();

    Sphere->Render(ShaderSphere.get());

    ShaderCube->use();
    ShaderSphere->setMat4("uView", viewMatrix);
    ShaderSphere->setMat4("uProjection", projectionMatrix);

    for (auto i = CubeQue.begin(); i != CubeQue.end(); i++)
    {
        i->get()->Render(ShaderSphere.get());
    }
}
void  WorldManager::DrawEndBanner(sf::RenderWindow *Window)
{
    Window->clear();

    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(100, 50));
    rectangle.setPosition(0, 0);
    rectangle.setFillColor(sf::Color(sf::Color::White));
    Window->draw(rectangle);

    Text.setFont(Font);
    Text.setCharacterSize(36);
    Text.setColor(sf::Color::White);
    std::string ScoreTxt = "Your score: " + std::to_string(Score);
    Text.setString(ScoreTxt);
    Text.setPosition(50, 100);

    Window->draw(Text);
    ScoreTxt = "Press SPACE to restart";
    Text.setString(ScoreTxt);
    Text.setPosition(50, 200);
    Window->draw(Text);
}