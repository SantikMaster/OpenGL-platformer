#pragma once
#include <vector>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "shader.h"
#include "vertex.h"

class Object;


class Object
{
public:
    const static int CUBE_EDGE = 100;

    Object(float radius = 50.0f, bool shapeType = false);
    void Render(Shader* shader);

    void SetRotationAngle(float angle);
    void SetPosition(const glm::vec3& position);
    void CreateIcosphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int recursionLevel, float radius);
    void Object::CreateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float sideLength);

    float RotationAngle;
    glm::vec3 Position;
    bool ShapeType = 0;
private:
    std::vector<Vertex> vertices;
    std::vector<Vertex> NewVertices;
    std::vector<unsigned int> indices;
    GLuint VBO, EBO;
    glm::mat4 modelMatrix;

    const int SPHERE_SUBDIVISIONS = 3; // Increase for higher resolution spheres
};

