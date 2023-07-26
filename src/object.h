#pragma once
#include <vector>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "shader.h"
#include "vertex.h"

//class Vertex;
class Object;
//class Shader;

class Object
{
public:
    const int SPHERE_SUBDIVISIONS = 3; // Increase for higher resolution spheres
    Object(float radius = 50.0f, bool shapeType = false);
    void render(Shader* shader);

    void setRotationAngle(float angle);
    void setPosition(const glm::vec3& position);
    void createIcosphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int recursionLevel, float radius);
    void Object::createCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float sideLength);

    float rotationAngle;
    glm::vec3 position;
    bool ShapeType = 0;
private:
    std::vector<Vertex> vertices;
    std::vector<Vertex> NewVertices;
    std::vector<unsigned int> indices;
    GLuint VBO, EBO;
    glm::mat4 modelMatrix;
};

