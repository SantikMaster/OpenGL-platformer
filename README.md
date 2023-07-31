# OpenGL-platformer 
OpenGL+SFML platformer

This is a platformer that uses OpenGL and SFML. The ball is bouncing among the cube platforms.

# Installation 

$git clone https://github.com/SantikMaster/OpenGL-platformer.git

$cd OpenGL-platformer

$mkdir build

$cd build

$cmake ..

# Description

Graphics Pipeline: The program sets up the graphics pipeline using vertex and fragment shaders. The vertex shader processes the vertices of the objects, while the fragment shader determines the final color of each pixel.

Sphere Generation: The application generates a sphere using an icosahedron-based subdivision algorithm. The sphere is refined by recursively dividing its triangles to create a smoother and more spherical shape.

Cube Rendering: The cube is created by defining its vertices and indices and then rendering it with the appropriate transformations.

Lighting and Shading: The scene implements lighting and shading to create a realistic appearance for the objects. The shader calculates ambient and diffuse lighting based on the position of the light source and the surface normal of the objects.

Simple OpenGL+SFML Engine:  The engine handles user input events (such as key presses and window close events) and updates the game state accordingly. For example, it processes player movement (A/D keys for left/right) and jumping (space key).

Game World Management (WorldManager): The WorldManager class is responsible for managing the game world, including handling game objects (such as a sphere), updating their positions and velocities, and rendering the scene.

Overall, the project serves as a basic 3D graphics application, showcasing the use of OpenGL for rendering 3D objects, implementing lighting and shading, and providing an interactive environment for visualizing the objects from different angles and positions. 
