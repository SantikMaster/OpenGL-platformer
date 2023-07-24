#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>

const int SPHERE_SUBDIVISIONS = 3; // Increase for higher resolution spheres

class Shader;
struct Vertex {
    float x, y, z;
    float normalX, normalY, normalZ;
};


void normalize(Vertex& vertex) {
    float length = std::sqrt(vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z);
    if (length != 0.0f) {
        vertex.x /= length;
        vertex.y /= length;
        vertex.z /= length;

        vertex.normalX = vertex.x;
        vertex.normalY = vertex.y;
        vertex.normalZ = vertex.z;
    }
}

void createIcosphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int recursionLevel, float radius)
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
            Vertex v12 = { (v1pos.x + v2pos.x) / 2.0f, (v1pos.y + v2pos.y) / 2.0f, (v1pos.z + v2pos.z) / 2.0f };
            Vertex v23 = { (v2pos.x + v3pos.x) / 2.0f, (v2pos.y + v3pos.y) / 2.0f, (v2pos.z + v3pos.z) / 2.0f };
            Vertex v31 = { (v3pos.x + v1pos.x) / 2.0f, (v3pos.y + v1pos.y) / 2.0f, (v3pos.z + v1pos.z) / 2.0f };

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
            normalize(vertex);
        }
        for (auto& vertex : vertices) {
            vertex.x *= radius;
            vertex.y *= radius;
            vertex.z *= radius;
        }
        indices = newIndices;
    }
}

const char* vertexShaderSource = R"(
    #version 440 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    
    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec3 FragNormal;
    out vec3 FragPos;

    void main() {
        FragNormal = mat3(transpose(inverse(uModel))) * aNormal;
        FragPos = vec3(uModel * vec4(aPos, 1.0));
        gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    }
)";


const char* fragmentShaderSource = R"(
#version 440 core

in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uAmbientStrength;
uniform float uDiffuseStrength;

void main() {
    vec3 ambient = uAmbientStrength * uLightColor;

    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(uLightDirection);
    lightDir = vec3(0, 0, 1);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Increase the blue component of the light color to make it more blue
    vec3 blueLightColor = vec3(uLightColor.x * 0.2, uLightColor.y * 0.2, uLightColor.z * 1.0);
    
    vec3 diffuse = uDiffuseStrength * diff * blueLightColor*1.5;
    
    // Define the range for the white stripe along the Y-axis
    float startY = -10.0; // Adjust the value based on the size of your sphere
    float endY = 10.0;   // Adjust the value based on the size of your sphere

    // Check if the fragment's position is within the range of the white stripe
    if (FragPos.y >= startY && FragPos.y <= endY) {
        vec3 norm = normalize(FragNormal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = uDiffuseStrength * diff * uLightColor;
            FragColor = vec4(ambient + diffuse, 1.0); // White color
    } else {
        FragColor = vec4(ambient + diffuse, 1.0);
    }
})";

class Sphere {
public:
    Sphere(float radius = 50.0f);
    void render();

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VBO, EBO;
};

class Shader 
{
public:
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader();
    void use();
    void setMat4(const char* name, const glm::mat4& matrix);
    void setVec3(const char* name, const glm::vec3& vector);
    void setFloat(const char* name, float value);
    GLuint getProgramID() const;

private:
    GLuint programID;
};


Sphere::Sphere(float radius)
{
    createIcosphere(vertices, indices, SPHERE_SUBDIVISIONS, radius);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void Sphere::render() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normalX)));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
}

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader, fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation status (omitted for brevity)

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation status (omitted for brevity)

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);

    glLinkProgram(programID);

    // Check for shader program linking status (omitted for brevity)

    glDetachShader(programID, vertexShader);
    glDetachShader(programID, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(programID);
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::setMat4(const char* name, const glm::mat4& matrix) {
    glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const char* name, const glm::vec3& vector) {
    glUniform3fv(glGetUniformLocation(programID, name), 1, glm::value_ptr(vector));
}

void Shader::setFloat(const char* name, float value) {
    glUniform1f(glGetUniformLocation(programID, name), value);
}

GLuint Shader::getProgramID() const {
    return programID;
}


class WorldManager
{
    std::shared_ptr<Sphere> sphere;

public:
    WorldManager();
    void Draw();
};

WorldManager::WorldManager()
{
     sphere = std::make_shared<Sphere>(50.0f);
}

void WorldManager::Draw()
{
    sphere->render();
}

class Engine
{
    void Init();


    sf::ContextSettings settings;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    

    GLint modelLoc;
    GLint viewLoc;
    GLint projectionLoc;
    GLint lightDirLoc;
    GLint lightColorLoc;
    GLint ambientStrengthLoc;
    GLint diffuseStrengthLoc;
public:
    Engine();
    ~Engine();
    void Update();
    void Run();

    std::unique_ptr<sf::RenderWindow> App;
    std::unique_ptr <Shader> shader;
    std::unique_ptr<WorldManager> World;
 
};
void Engine::Init()
{

    settings.depthBits = 24; // Set the depth buffer bits to 24
    settings.majorVersion = 4; // Request OpenGL 3.0 or above
    settings.minorVersion = 4;

    App = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600, 32), "SFML OpenGL", sf::Style::Default, settings);
   // sf::Clock Clock;

    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(glewInitResult) << std::endl;
    }

    // Check OpenGL version
    if (!GLEW_VERSION_3_3) {
        std::cerr << "OpenGL 3.3 is not supported by your graphics card or driver." << std::endl;
    }

    // OpenGL setup
    glClearDepth(1.f);
    glClearColor(0.3f, 0.3f, 0.3f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);


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
    projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(App->getSize().x) / App->getSize().y, 1.f, 300.f);
    glLoadMatrixf(glm::value_ptr(projectionMatrix));
    shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);

    modelLoc = glGetUniformLocation(shader->getProgramID(), "uModel");
    viewLoc = glGetUniformLocation(shader->getProgramID(), "uView");
    projectionLoc = glGetUniformLocation(shader->getProgramID(), "uProjection");
    lightDirLoc = glGetUniformLocation(shader->getProgramID(), "uLightDirection");
    lightColorLoc = glGetUniformLocation(shader->getProgramID(), "uLightColor");
    ambientStrengthLoc = glGetUniformLocation(shader->getProgramID(), "uAmbientStrength");
    diffuseStrengthLoc = glGetUniformLocation(shader->getProgramID(), "uDiffuseStrength");

    World = std::make_unique<WorldManager>();
}
void Engine::Update()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix (no transformation for now)
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glm::vec3 lightDirection(0.f, 1.f, 0.f); // Adjust light direction as needed
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirection));

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // White light color
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glUniform1f(ambientStrengthLoc, 0.2f);
    glUniform1f(diffuseStrengthLoc, 0.7f);
}
Engine::Engine()
{
    Init();
}
Engine::~Engine()
{

}
void Engine::Run()
{
    while (App->isOpen()) 
    {
        sf::Event Event;
        while (App->pollEvent(Event)) {
            if (Event.type == sf::Event::Closed)
                App->close();

            if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
                App->close();
        }

        // Rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();
        Update();
        World->Draw();

        App->display();
    }
}
// ... (Rest of the function prototypes)

int main()
{
    Engine SFMLEngine;
    SFMLEngine.Run();

    return EXIT_SUCCESS;
}


