
#include "object.h"
#include "vertex.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>


void Object::SetRotationAngle(float angle)
{
    RotationAngle = angle;
}

void Object::SetPosition(const glm::vec3& pos)
{
    Position = pos;
}

Object::Object(float radius, bool shapeType)
{
    ShapeType = shapeType;
    modelMatrix = glm::mat4(1.0f);
    RotationAngle = 0;
    Position = glm::vec3(0, 0, 0);

    if (shapeType == 0)
        CreateIcosphere(vertices, indices, SPHERE_SUBDIVISIONS, radius);
    else
        CreateCube(vertices, indices, radius);

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        Vertex ver;
        NewVertices.push_back(ver);

    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

}

void Object::Render(Shader* shader)
{
    modelMatrix = glm::mat4(1.0f);

    if (ShapeType == 1)
        modelMatrix = glm::translate(modelMatrix, Position); // Apply translation if needed
    else
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, Position.y, Position.z));
   
    modelMatrix = glm::rotate(modelMatrix, RotationAngle, glm::vec3(0.0f, -1.0f, 0.0f));
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    //   glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
 


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "uRotationAngle"), RotationAngle);
    if (ShapeType == 1)
        glUniform3fv(glGetUniformLocation(shader->getProgramID(), "uSphereCenter"), 1, glm::value_ptr(Position));
    else
    glUniform3fv(glGetUniformLocation(shader->getProgramID(), "uSphereCenter"), 1, glm::value_ptr(glm::vec3(0, Position.y, Position.z)));
    glUniform3fv(glGetUniformLocation(shader->getProgramID(), "uSphereRotation"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, +10.0f)));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normalX)));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Object::CreateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float sideLength)
{
    // Vertices for a cube
    vertices = {
        // Front face
       {-sideLength / 2, -sideLength / 2, sideLength / 2},
        {sideLength / 2, -sideLength / 2, sideLength / 2},
        {sideLength / 2, sideLength / 2, sideLength / 2},
        {-sideLength / 2, sideLength / 2, sideLength / 2},

        // Back face
       {-sideLength / 2, -sideLength / 2, -sideLength / 2},
        {sideLength / 2, -sideLength / 2, -sideLength / 2},
        {sideLength / 2, sideLength / 2, -sideLength / 2},
        {-sideLength / 2, sideLength / 2, -sideLength / 2},/**/

        // Left face
        {-sideLength / 2, -sideLength / 2, -sideLength / 2},
        {-sideLength / 2, -sideLength / 2, sideLength / 2},
        {-sideLength / 2, sideLength / 2, sideLength / 2},
        {-sideLength / 2, sideLength / 2, -sideLength / 2},

        // Right face
        {sideLength / 2, -sideLength / 2, sideLength / 2},
        {sideLength / 2, -sideLength / 2, -sideLength / 2},
        {sideLength / 2, sideLength / 2, -sideLength / 2},
        {sideLength / 2, sideLength / 2, sideLength / 2},

        // Top face
         {-sideLength / 2, sideLength / 2, sideLength / 2},
        {sideLength / 2, sideLength / 2, sideLength / 2},
        {sideLength / 2, sideLength / 2, -sideLength / 2},
        {-sideLength / 2, sideLength / 2, -sideLength / 2},

        // Bottom face
        {-sideLength / 2, -sideLength / 2, -sideLength / 2},
        {sideLength / 2, -sideLength / 2, -sideLength / 2},
        {sideLength / 2, -sideLength / 2, sideLength / 2},
        {-sideLength / 2, -sideLength / 2, sideLength / 2}
    };
    // Indices for a cube (using triangle strip)
    indices = {
        0, 1, 2,  // Front face
        2, 3, 0,
        4, 5, 6,  // Right face
        6, 7, 4,
        8, 9, 10, // Back face
        10, 11, 8,
        12, 13, 14, // Left face
        14, 15, 12,
        16, 17, 18, // Bottom face
        18, 19, 16,
        20, 21, 22, // Top face
        22, 23, 20
    };
}

void Object::CreateIcosphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int recursionLevel, float radius)
{
    // Create an icosahedron
    const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

    vertices = {
        {-1, t, 0}, {1, t, 0}, {-1, -t, 0}, {1, -t, 0},
        {0, -1, t}, {0, 1, t}, {0, -1, -t}, {0, 1, -t},
        {t, 0, -1}, {t, 0, 1}, {-t, 0, -1}, {-t, 0, 1}
    };

    indices = {
        0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
        1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
        3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
        4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
    };

    // Subdivide triangles to create a sphere-like shape
    for (int i = 0; i < recursionLevel; ++i) {
        std::vector<unsigned int> newIndices;

        for (size_t j = 0; j < indices.size(); j += 3) {
            unsigned int v1 = indices[j];
            unsigned int v2 = indices[j + 1];
            unsigned int v3 = indices[j + 2];

            Vertex v1pos = vertices[v1];
            Vertex v2pos = vertices[v2];
            Vertex v3pos = vertices[v3];

            // Create three new vertices at the midpoints of each triangle edge
            Vertex v12 = Vertex(glm::vec3((v1pos.x + v2pos.x) / 2.0f, (v1pos.y + v2pos.y) / 2.0f, (v1pos.z + v2pos.z) / 2.0f));
            Vertex v23 = Vertex(glm::vec3((v2pos.x + v3pos.x) / 2.0f, (v2pos.y + v3pos.y) / 2.0f, (v2pos.z + v3pos.z) / 2.0f));
            Vertex v31 = Vertex(glm::vec3((v3pos.x + v1pos.x) / 2.0f, (v3pos.y + v1pos.y) / 2.0f, (v3pos.z + v1pos.z) / 2.0f));

            // Normalize the new vertices to lie on the sphere's surface
            float len = std::sqrt(v12.x * v12.x + v12.y * v12.y + v12.z * v12.z);
            v12.x /= len; v12.y /= len; v12.z /= len;

            len = std::sqrt(v23.x * v23.x + v23.y * v23.y + v23.z * v23.z);
            v23.x /= len; v23.y /= len; v23.z /= len;

            len = std::sqrt(v31.x * v31.x + v31.y * v31.y + v31.z * v31.z);
            v31.x /= len; v31.y /= len; v31.z /= len;

            // Add new vertices and update indices
            unsigned int v12Idx = static_cast<unsigned int>(vertices.size());
            unsigned int v23Idx = v12Idx + 1;
            unsigned int v31Idx = v12Idx + 2;

            vertices.push_back(v12);
            vertices.push_back(v23);
            vertices.push_back(v31);

            newIndices.push_back(v1); newIndices.push_back(v12Idx); newIndices.push_back(v31Idx);
            newIndices.push_back(v2); newIndices.push_back(v23Idx); newIndices.push_back(v12Idx);
            newIndices.push_back(v3); newIndices.push_back(v31Idx); newIndices.push_back(v23Idx);
            newIndices.push_back(v12Idx); newIndices.push_back(v23Idx); newIndices.push_back(v31Idx);
        }
        for (auto& vertex : vertices) {
            vertex.normalize();
        }
        for (auto& vertex : vertices) {
            vertex.x *= radius;
            vertex.y *= radius;
            vertex.z *= radius;
        }
        indices = newIndices;
    }
}