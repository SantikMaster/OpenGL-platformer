#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Vertex
{
public:
    float x, y, z;
    float normalX, normalY, normalZ;

    Vertex()
    {
        x = 0;
        y = 0;
        z = 0;


        normalX = 0;
        normalY = 0;
        normalZ = 0;
    }

    Vertex(glm::vec3 vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;

        float length = std::sqrt(x * x + y * y + z * z);

        normalX = x;
        normalY = y;
        normalZ = z;

        normalX /= length;
        normalY /= length;
        normalZ /= length;
    }
    Vertex(float xVal, float yVal, float zVal)
    {
        x = xVal;
        y = yVal;
        z = zVal;

        float length = std::sqrt(x * x + y * y + z * z);

        normalX = x;
        normalY = y;
        normalZ = z;

        normalX /= length;
        normalY /= length;
        normalZ /= length;
    }
    void normalize() 
    {
        float length = std::sqrt(x * x + y * y + z * z);
        if (length != 0.0f)
        {
             x /= length;
             y /= length;
             z /= length;
             normalX = x;
             normalY = y;
             normalZ = z;
        }
    }
};


