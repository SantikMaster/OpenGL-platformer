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
    GroundHeight = Object::CUBE_EDGE;
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

    Sphere = std::make_shared<Object>(Object::SPHERE_RADIUS);

    Restart();
}

void WorldManager::Restart()
{
    SphereVelocityX = 0;
    SphereVelocityZ = 0;

    srand(time(0));
    if (!Font.loadFromFile(myfontFileName))
    {
        std::cout << "font not loaded\n";
    }
    if (!backgroundImage.loadFromFile(myImageFileName))
    {
        std::cout << "Image not loaded\n";
    }

    CubeQue.clear();
    int i = 0;
    for (i = 0; i < AmountOfStartCubes; i++)
    {
        std::shared_ptr<Object> obj = std::make_shared<Object>(Object::CUBE_EDGE, 1);
        obj->SetPosition(glm::vec3(Object::CUBE_EDGE * i - Object::CUBE_EDGE * AmountOfStartCubes/2, 0.0f, 0.0f));
        CubeQue.push_back(obj);
    }

    Sphere->SetPosition(glm::vec3(00.0f, 00.0f, Object::CUBE_EDGE + Object::CUBE_EDGE));
    Score = 0;
    EndingGame = false;
}

void WorldManager::GenerateMap()
{
    auto back_element = CubeQue.back();

    const float ProbapilityFlat = 0.5;
    const float ProbapilityStepUp = 0.2;
    const float ProbapilityStepDown = 0.2;


    float val = (float)rand() / RAND_MAX;
    std::shared_ptr<Object> obj = std::make_shared<Object>(Object::CUBE_EDGE, 1);

    if (val < ProbapilityFlat)
    {
        obj->SetPosition(glm::vec3(back_element->Position.x + Object::CUBE_EDGE, 0, back_element->Position.z));
    }
    else if (val < ProbapilityFlat+ProbapilityStepUp)
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
    else if (val < ProbapilityFlat + ProbapilityStepUp + ProbapilityStepDown)
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
        obj->SetPosition(glm::vec3(back_element->Position.x + 2*Object::CUBE_EDGE, 0, back_element->Position.z)); //Makeing a hole in the ground
    }
    CubeQue.push_back(obj);
    CubeQue.pop_front();
}

bool WorldManager::Collide(int FutureX, int FutureZ) 
{
    bool collide = false;
    for (auto it = CubeQue.begin(); it != CubeQue.end(); ++it)
    {       
        if ((FutureX >= it->get()->Position.x - Object::CUBE_EDGE) &&
            (FutureX < it->get()->Position.x) )
        {
            if ((FutureZ >= it->get()->Position.z) &&
                (FutureZ < it->get()->Position.z + Object::CUBE_EDGE - 1))
            {
                return true;
            }
        }
    }

    return collide;
}
void WorldManager::SphereJump()
{
    if (Collide(Sphere->Position.x, Sphere->Position.z - DistanceToTexture))
        SphereVelocityZ += SpherePushVelocityZ;
}
void WorldManager::Update(int sizeX, int sizeY) 
{
    float CameraStartPosX = -300;
    float CameraStartPosY = -400;
    float CameraStartPosZ = 200;
    float HowFar = 2000.f;

    if (Timer.getElapsedTime().asMilliseconds() >= DeltaTimeMS)
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
            SphereVelocityZ *= ReflectionZVelocityMultimplier;
        }

        if (!Collide(TempX, Sphere->Position.z + DistanceToTexture))
        {
            Sphere->Position.x = TempX;
        }
        else
        {
            IsSphereColliding = true;
            SphereVelocityX *= ReflectionXVelocityMultimplier;
        }

        if (Sphere->Position.z < YOutOfScreen) EndingGame = true;

        Timer.restart();
    }
    if (MapTimer.getElapsedTime().asMilliseconds() >= StartSpeed - TimePenalty)
    {
        GenerateMap();
        MapTimer.restart();
        TimePenalty++;
        Score++;
    }

    glm::vec3 cameraPos(CameraStartPosX + Sphere->Position.x , CameraStartPosY, CameraStartPosZ);
    glm::vec3 cameraTarget(Sphere->Position.x, 0.f, 0.f);
    glm::vec3 cameraUp(0.f, 0.1f, 0.8f);

    viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(sizeX) / sizeY, 1.f, HowFar);
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

    int ScorePositionX = 50;
    int ScorePositionY = 100;
    int RestartPositionX = 50;
    int RestartPositionY = 200;

    Text.setFont(Font);
    Text.setCharacterSize(36);
    Text.setColor(sf::Color::White);
    std::string Str = "Your score: " + std::to_string(Score);
    Text.setString(Str);
    Text.setPosition(ScorePositionX, ScorePositionY);

    Window->draw(Text);
    Str = "Press SPACE to restart";
    Text.setString(Str);
    Text.setPosition(RestartPositionX, RestartPositionY);
    Window->draw(Text);
}
void WorldManager::DrawBackground(sf::RenderWindow* Window)
{

}