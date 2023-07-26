#include "worldmanager.h"
#include "shader.h"
#include "object.h"

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
uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uRotationAngle;   // Uniform to pass the rotation angle from the CPU
uniform vec3 uSphereCenter;     // Uniform to pass the center position of the sphere
uniform vec3 uSphereRotation;   // Uniform to pass the axis of rotation for the sphere

void main() {
    vec3 ambient = uAmbientStrength * uLightColor;

    // Calculate the light direction in view space
    vec3 lightDir = normalize(vec3(0, 0, 1)); // Assuming the light is facing in the negative Z direction
    lightDir = normalize(FragPos - lightDir);

    float diff = max(dot(normalize(FragNormal), lightDir), 0.0);

    // Increase the blue component of the light color to make it more blue
    vec3 blueLightColor = vec3(uLightColor.x * 0.2, uLightColor.y * 0.2, uLightColor.z * 1.0);

    vec3 diffuse = uDiffuseStrength * diff * blueLightColor * 1.5;

    // Calculate the uniform rotation angle using the absolute value of uSphereRotation
    float uniformRotationAngle = uRotationAngle * length(uSphereRotation);

    // Calculate the vector from the fragment's position to the sphere's center
    vec3 centerToFrag = FragPos - uSphereCenter;

    // Calculate the dot product between uSphereRotation and the vector to determine the rotation direction
    float rotationDot = dot(normalize(uSphereRotation), normalize(centerToFrag));

    // Adjust the rotation angle based on the rotation direction
 
    float finalRotationAngle = uniformRotationAngle*sign(rotationDot);

    // Rotate the fragment's position based on the sphere's rotation angle and axis
    float cosAngle = cos(finalRotationAngle);
    float sinAngle = sin(finalRotationAngle);
/*    vec3 rotatedPos = vec3(
        cosAngle * centerToFrag.x - sinAngle * centerToFrag.z + uSphereCenter.x,
        FragPos.y,
        sinAngle * centerToFrag.x + cosAngle * centerToFrag.z + uSphereCenter.z
    );*/

    vec3 rotatedPos = vec3(
        cosAngle * centerToFrag.x - sinAngle * centerToFrag.y + uSphereCenter.x,
        cosAngle * centerToFrag.y + sinAngle * centerToFrag.x + uSphereCenter.y,
        centerToFrag.z + uSphereCenter.z
    );

    // Calculate the distance of the rotated position from the Y-axis
    float distanceFromYAxis = abs(rotatedPos.x - uSphereCenter.x);

    // Define the width of the stripe
    float stripeWidth = 10.0; // Adjust the value based on your desired width

    // Check if the distance from the Y-axis is less than or equal to the stripe width
    if (distanceFromYAxis <= stripeWidth * 0.5) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
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
uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uRotationAngle;   // Uniform to pass the rotation angle from the CPU
uniform vec3 uSphereCenter;     // Uniform to pass the center position of the sphere
uniform vec3 uSphereRotation;   // Uniform to pass the axis of rotation for the sphere

void main() {
    vec3 ambient = uAmbientStrength * uLightColor;

    // Calculate the light direction in view space
    vec3 lightDir = normalize(vec3(0, 0, 1)); // Assuming the light is facing in the negative Z direction
    lightDir = normalize(FragPos - lightDir);

    float diff = max(dot(normalize(FragNormal), lightDir), 0.0);

    // Increase the green component of the light 
    vec3 greenLightColor = vec3(uLightColor.x * 0.2, uLightColor.y * 1, uLightColor.z * 0.2);

    vec3 diffuse = uDiffuseStrength * diff * greenLightColor * 3.5;

    // Set the alpha component to 0.5 for half-transparency
    FragColor = vec4(ambient + diffuse, 0.7);
}
)";
WorldManager::WorldManager(int sizeX, int sizeY)
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
    glm::vec3 lightDirection(0.f, 1.f, 0.f);
    glm::vec3 cameraPos(0.f, 0.f, 200.f);
    glm::vec3 cameraTarget(0.f, 0.f, 0.f);
    glm::vec3 cameraUp(0.f, 1.f, 0.f);
    viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(viewMatrix));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(sizeX) / sizeY, 1.f, 300.f);
    glLoadMatrixf(glm::value_ptr(projectionMatrix));

    shader->use();


    sphere = std::make_shared<Object>(50.0f);
    cube = std::make_shared<Object>(100.0f, 1);


    sphere->setPosition(glm::vec3(-50.0f, 0.0f, 0.0f));
    cube->setPosition(glm::vec3(100.0f, 0.0f, 0.0f));
}

void WorldManager::Update()
{
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glm::vec3 lightDirection(0.f, 1.f, 0.f);
    lightDirection = glm::mat3(viewMatrix) * lightDirection;
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirection));

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // White light color
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glUniform1f(ambientStrengthLoc, 0.2f);
    glUniform1f(diffuseStrengthLoc, 0.7f);
}

void WorldManager::Draw()
{
    sphere->setRotationAngle(sphere->rotationAngle + 0.0001f);
    cube->setRotationAngle(sphere->rotationAngle + 0.0013f);

    shader->use();

    sphere->render(shader.get());

    shader2->use();
    cube->render(shader.get());
}