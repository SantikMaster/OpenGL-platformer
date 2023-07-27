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
uniform vec3 uSphereCenter;     // Uniform to pass the center position of the sphere
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

/*
const char* FragmentShaderSource_sphere = R"(
#version 440 core

in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 uLightColor;
uniform vec3 uLightDirection;
uniform float uAmbientStrength;
uniform float uDiffuseStrength;

void main() {
    vec3 ambient = uAmbientStrength * uLightColor;
    vec3 lightDir = normalize(uLightDirection);
    float diff = max(dot(normalize(FragNormal), lightDir), 0.0);
    vec3 diffuse = uDiffuseStrength * diff * uLightColor;

    FragColor = vec4(ambient + diffuse, 1.0);
}
)";*/

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

    shader = std::make_unique<Shader>(VertexShaderSource_sphere, FragmentShaderSource_sphere);
    shader2 = std::make_unique<Shader>(VertexShaderSource, FragmentShaderSource);

    modelLoc = glGetUniformLocation(shader->getProgramID(), "uModel");
    viewLoc = glGetUniformLocation(shader->getProgramID(), "uView");
    projectionLoc = glGetUniformLocation(shader->getProgramID(), "uProjection");
    lightDirLoc = glGetUniformLocation(shader->getProgramID(), "uLightDirection");
    lightColorLoc = glGetUniformLocation(shader->getProgramID(), "uLightColor");
    ambientStrengthLoc = glGetUniformLocation(shader->getProgramID(), "uAmbientStrength");
    diffuseStrengthLoc = glGetUniformLocation(shader->getProgramID(), "uDiffuseStrength");

    // Camera setup

    shader->use();


    sphere = std::make_shared<Object>(50.0f);

    int i = 0;
    for (i = 0; i < 20; i++)
    {
        std::shared_ptr<Object> obj = std::make_shared<Object>(100.0f, 1);
        obj->setPosition(glm::vec3(100.0f*i-400, 0.0f, 0.0f));
        cube_que.push_back(obj);
    }

    sphere->setPosition(glm::vec3(00.0f, 00.0f, 150.0f));
}

void WorldManager::GenerateMap()
{
    auto back_element = cube_que.back();

   // int RANDMAX = 32767
    int val = 110*::rand() / RAND_MAX;
    std::cout << val << '\n';
    std::shared_ptr<Object> obj = std::make_shared<Object>(100.0f, 1);

    if (val < 50)
    {

        obj->setPosition(glm::vec3(back_element->position.x + 100, 0, back_element->position.z));
    }
    else if (val < 75)
    {
        if (back_element->position.z > -300)
        {
            obj->setPosition(glm::vec3(back_element->position.x + 100, 0, back_element->position.z-100));
        }
        else
        {
            obj->setPosition(glm::vec3(back_element->position.x + 100, 0, -300.0f));
        }
    }
    else if (val < 100)
    {
        if (back_element->position.z < 200)
        {
            obj->setPosition(glm::vec3(back_element->position.x + 100, 0, back_element->position.z + 100));
        }
        else
        {
            obj->setPosition(glm::vec3(back_element->position.x + 100, 0, 200.0f));
        }
    }
    else 
    {
        obj->setPosition(glm::vec3(back_element->position.x + 200, 0, back_element->position.z));
    }
    cube_que.push_back(obj);
    cube_que.pop_front();

}

bool WorldManager::Collide(int FutureX, int FutureZ) // 
{
    bool collide = false;
    for (auto it = cube_que.begin(); it != cube_que.end(); ++it)
    {       

        if ((FutureX >= it->get()->position.x - 100) &&
            (FutureX < it->get()->position.x) )
        {
            if ((FutureZ >= it->get()->position.z) &&
                (FutureZ < it->get()->position.z + 99))
            {
                return true;
            }
        }
    }

    return collide;

}
void WorldManager::SphereJump() // 
{
    if (Collide(sphere->position.x, sphere->position.z - 10))
        SphereVelocityZ += SpherePushVelocityZ;
}
void WorldManager::Update(int sizeX, int sizeY) // 
{
    if (timer.getElapsedTime().asMilliseconds() >= 10)
    {
        
        sphere->setRotationAngle(sphere->rotationAngle + SphereVelocityX);
    
        int TempX = sphere->position.x + SphereVelocityX * 500;
        int TempZ = sphere->position.z + SphereVelocityZ;

        if (!Collide(sphere->position.x, TempZ))
        {
          
            sphere->position.z = TempZ;
            SphereVelocityZ -= SpheredeltaVelocityZ;
            IsSphereColliding = false;
        }
        else
        {
            IsSphereColliding = true;
            std::cout << "helloz\n";
            SphereVelocityZ *= -0.1;
        }

        if (!Collide(TempX, sphere->position.z+10))
        {
            sphere->position.x = TempX;
        }
        else
        {
            std::cout << "hellox\n";
            IsSphereColliding = true;
            SphereVelocityX *= -0.3;
        }

        timer.restart();
    }
    if (MapTimer.getElapsedTime().asMilliseconds() >= 700- TimePenalty)
    {
        GenerateMap();
        MapTimer.restart();
        TimePenalty++;
    }

    //  glm::vec3 lightDirection(0.f, 1.f, 0.f);
    glm::vec3 cameraPos(-300.0f+ sphere->position.x , -400.0f, 200.f);
    glm::vec3 cameraTarget(sphere->position.x, 0.f, 0.f);
    glm::vec3 cameraUp(0.f, 0.1f, 0.8f);

    viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    // Update the projection matrix
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
    shader->use();

    sphere->render(shader.get());

    shader2->use();
    shader->setMat4("uView", viewMatrix);
    shader->setMat4("uProjection", projectionMatrix);

    for (auto i = cube_que.begin(); i != cube_que.end(); i++)
    {
        i->get()->render(shader.get());
    }
}