#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
//#include <string>
#include <fstream>
#include <sstream>
#include <vector>

const float X = 0.525731112119133606f;
const float Z = 0.850650808352039932f;

static const float points[] =
{
    -X, 0.0f, Z, X, 0.0f, Z, -X, 0.0f, -Z, X, 0.0f, -Z,
    0.0f, Z, X, 0.0f, Z, -X, 0.0f, -Z, X, 0.0f, -Z, -X,
    Z, X, 0.0f, -Z, X, 0.0f, Z, -X, 0.0f, -Z, -X, 0.0f
};

glm::vec3 normalize(const glm::vec3& v)
{
    float length = glm::length(v);
    return length > 0.0f ? v / length : v;
}

// Function to create an icosphere
void createIcosphere(int recursionLevel, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLuint>& indices)
{
    const float X = 0.525731112119133606f;
    const float Z = 0.850650808352039932f;

  /*  static const float points[] =
    {
        -X, 0.0f, Z, X, 0.0f, Z, -X, 0.0f, -Z, X, 0.0f, -Z,
        0.0f, Z, X, 0.0f, Z, -X, 0.0f, -Z, X, 0.0f, -Z, -X,
        Z, X, 0.0f, -Z, X, 0.0f, Z, -X, 0.0f, -Z, -X, 0.0f
    };*/

    static const GLuint triangles[] =
    {
        0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
        1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
        3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
        4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
    };

    vertices.clear();
    normals.clear();
    indices.clear();

    // Add the initial icosahedron vertices
    for (int i = 0; i < 12; i++)
    {
        vertices.push_back(points[i * 3]);
        vertices.push_back(points[i * 3 + 1]);
        vertices.push_back(points[i * 3 + 2]);
    }

    // Subdivide the triangles
    for (int i = 0; i < 60; i += 3)
    {
        glm::vec3 v1(vertices[triangles[i] * 3], vertices[triangles[i] * 3 + 1], vertices[triangles[i] * 3 + 2]);
        glm::vec3 v2(vertices[triangles[i + 1] * 3], vertices[triangles[i + 1] * 3 + 1], vertices[triangles[i + 1] * 3 + 2]);
        glm::vec3 v3(vertices[triangles[i + 2] * 3], vertices[triangles[i + 2] * 3 + 1], vertices[triangles[i + 2] * 3 + 2]);

        glm::vec3 a = normalize((v1 + v2) * 0.5f);
        glm::vec3 b = normalize((v2 + v3) * 0.5f);
        glm::vec3 c = normalize((v3 + v1) * 0.5f);

        vertices.push_back(a.x);
        vertices.push_back(a.y);
        vertices.push_back(a.z);

        vertices.push_back(b.x);
        vertices.push_back(b.y);
        vertices.push_back(b.z);

        vertices.push_back(c.x);
        vertices.push_back(c.y);
        vertices.push_back(c.z);
    }

    // Normalize all the vertices
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        glm::vec3 v(vertices[i], vertices[i + 1], vertices[i + 2]);
        v = normalize(v);
        normals.push_back(v.x);
        normals.push_back(v.y);
        normals.push_back(v.z);
    }

    // Initialize indices
    for (GLuint i = 0; i < vertices.size() / 3; i += 3)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    // Subdivide the triangles recursively to increase the resolution
    for (int i = 0; i < recursionLevel; ++i)
    {
        std::cout << "here";
        std::vector<GLuint> newIndices;
        newIndices.reserve(indices.size() * 4);
        for (size_t j = 0; j < indices.size(); j += 3)
        {
            GLuint ia = indices[j];
            GLuint ib = indices[j + 1];
            GLuint ic = indices[j + 2];

            GLuint iab = vertices.size() / 3 + (ia + ib) / 2;
            GLuint ibc = vertices.size() / 3 + (ib + ic) / 2;
            GLuint ica = vertices.size() / 3 + (ic + ia) / 2;

            vertices.push_back(vertices[ia * 3]);
            vertices.push_back(vertices[ia * 3 + 1]);
            vertices.push_back(vertices[ia * 3 + 2]);

            vertices.push_back(vertices[ib * 3]);
            vertices.push_back(vertices[ib * 3 + 1]);
            vertices.push_back(vertices[ib * 3 + 2]);

            vertices.push_back(vertices[ic * 3]);
            vertices.push_back(vertices[ic * 3 + 1]);
            vertices.push_back(vertices[ic * 3 + 2]);

            vertices.push_back(vertices[iab * 3]);
            vertices.push_back(vertices[iab * 3 + 1]);
            vertices.push_back(vertices[iab * 3 + 2]);

            vertices.push_back(vertices[ibc * 3]);
            vertices.push_back(vertices[ibc * 3 + 1]);
            vertices.push_back(vertices[ibc * 3 + 2]);

            vertices.push_back(vertices[ica * 3]);
            vertices.push_back(vertices[ica * 3 + 1]);
            vertices.push_back(vertices[ica * 3 + 2]);

            newIndices.push_back(ia);
            newIndices.push_back(iab);
            newIndices.push_back(ica);

            newIndices.push_back(ib);
            newIndices.push_back(ibc);
            newIndices.push_back(iab);

            newIndices.push_back(ic);
            newIndices.push_back(ica);
            newIndices.push_back(ibc);

            newIndices.push_back(iab);
            newIndices.push_back(ibc);
            newIndices.push_back(ica);
        }
        indices = newIndices;
    }
}
int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24; // Set the depth buffer bits to 24
    settings.majorVersion = 3; // Request OpenGL 3.0 or above
    settings.minorVersion = 3;



    std::vector<GLfloat> sphereVertices;
    std::vector<GLfloat> sphereColors;
   
    /*std::vector<GLfloat> icosphereVertices;
    std::vector<GLfloat> icosphereVertices(points, points + sizeof(points) / sizeof(points[0]));

    std::vector<GLfloat> icosphereNormals;
    std::vector<GLuint> icosphereIndices;

    // Define the recursion level for the icosphere (higher level for more detail)
    int recursionLevel = 3;

    // Create the icosphere using the createIcosphere function
    createIcosphere(recursionLevel, icosphereVertices, icosphereNormals, icosphereIndices);*/

    const int stacks = 20; // Number of latitude lines
    const int slices = 20; // Number of longitude lines
    const float radius = 50.f;

    for (int i = 0; i <= stacks; ++i)
    {
        float phi = glm::pi<float>() * static_cast<float>(i) / static_cast<float>(stacks);
        float y = cos(phi);
        float sinPhi = sin(phi);

        for (int j = 0; j <= slices; ++j)
        {
            float theta = 2.f * glm::pi<float>() * static_cast<float>(j) / static_cast<float>(slices);
            float x = sinPhi * cos(theta);
            float z = sinPhi * sin(theta);

            // Vertex position
            sphereVertices.push_back(radius * x);
            sphereVertices.push_back(radius * y);
            sphereVertices.push_back(radius * z);

            // Vertex color (you can modify colors here if needed)
            sphereColors.push_back(1.f);
            sphereColors.push_back(0.f);
            sphereColors.push_back(0.f);
            sphereColors.push_back(0.5f); // Alpha (transparency) of 0.5
        }
    }

    sf::RenderWindow App(sf::VideoMode(800, 600, 32), "SFML OpenGL", sf::Style::Default, settings);
    sf::Clock Clock;

    // OpenGL setup
    glClearDepth(1.f);
    glClearColor(0.3f, 0.3f, 0.3f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Projection setup
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(App.getSize().x) / App.getSize().y, 1.f, 300.f);
    glLoadMatrixf(glm::value_ptr(projectionMatrix));

    // Camera setup
    glm::vec3 cameraPos(0.f, 0.f, 200.f);
    glm::vec3 cameraTarget(0.f, 0.f, 0.f);
    glm::vec3 cameraUp(0.f, 1.f, 0.f);
    glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(viewMatrix));

    bool rotate = true;
    float angle;
    // Enable color material
   // glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);




    while (App.isOpen())
    {
        sf::Event Event;
        while (App.pollEvent(Event))
        {
            if (Event.type == sf::Event::Closed)
                App.close();

            if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
                App.close();

            if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::A))
            {
                rotate = !rotate;
            }
        }
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
         //   std::cerr << "OpenGL Error: " << gluErrorString(error) << std::endl;
        }
        // Drawing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glPushMatrix();
        if (rotate)
        {
            angle = Clock.getElapsedTime().asSeconds();
        }
        glRotatef(angle * 50, 1.f, 0.f, 0.f);
        glRotatef(angle * 30, 0.f, 1.f, 0.f);
        glRotatef(angle * 90, 0.f, 0.f, 1.f);

        glBegin(GL_QUADS);
        glBegin(GL_QUADS);
        // Cyan color with alpha (transparency) of 0.5
        glColor4f(0.f, 1.f, 1.f, 0.5f);
        glVertex3f(-50.f, -50.f, -50.f);
        glVertex3f(-50.f, 50.f, -50.f);
        glVertex3f(50.f, 50.f, -50.f);
        glVertex3f(50.f, -50.f, -50.f);

        // Blue color with alpha (transparency) of 0.5
        glColor4f(0.f, 0.f, 1.f, 0.5f);
        glVertex3f(-50.f, -50.f, 50.f);
        glVertex3f(-50.f, 50.f, 50.f);
        glVertex3f(50.f, 50.f, 50.f);
        glVertex3f(50.f, -50.f, 50.f);

        // Magenta color with alpha (transparency) of 0.5
        glColor4f(1.f, 0.f, 1.f, 0.5f);
        glVertex3f(-50.f, -50.f, -50.f);
        glVertex3f(-50.f, 50.f, -50.f);
        glVertex3f(-50.f, 50.f, 50.f);
        glVertex3f(-50.f, -50.f, 50.f);

        // Green color with alpha (transparency) of 0.5
        glColor4f(0.f, 1.f, 0.f, 0.5f);
        glVertex3f(50.f, -50.f, -50.f);
        glVertex3f(50.f, 50.f, -50.f);
        glVertex3f(50.f, 50.f, 50.f);
        glVertex3f(50.f, -50.f, 50.f);

        // Yellow color with alpha (transparency) of 0.5
        glColor4f(1.f, 1.f, 0.f, 0.5f);
        glVertex3f(-50.f, -50.f, 50.f);
        glVertex3f(-50.f, -50.f, -50.f);
        glVertex3f(50.f, -50.f, -50.f);
        glVertex3f(50.f, -50.f, 50.f);

        // Red color with alpha (transparency) of 0.5
        glColor4f(1.f, 0.f, 0.f, 0.5f);
        glVertex3f(-50.f, 50.f, 50.f);
        glVertex3f(-50.f, 50.f, -50.f);
        glVertex3f(50.f, 50.f, -50.f);
        glVertex3f(50.f, 50.f, 50.f);

        glEnd();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, sphereVertices.data());
        glColorPointer(4, GL_FLOAT, 0, sphereColors.data());
        glDrawArrays(GL_QUAD_STRIP, 0, (stacks + 1) * (slices + 1));
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);/**/

        /*glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glColor3f(0.0f, 0.0f, 1.0f); // Blue color (R, G, B)
        glVertexPointer(3, GL_FLOAT, 0, icosphereVertices.data());
        glDrawElements(GL_TRIANGLES, icosphereIndices.size(), GL_UNSIGNED_INT, icosphereIndices.data());
        glDisableClientState(GL_VERTEX_ARRAY);*/

        glPopMatrix();

        // Display
        App.display();
    }

    return EXIT_SUCCESS;
}
