cmake_minimum_required(VERSION 3.11)
project(MySFMLProject)

# Set C++ standard (change if needed)
set(CMAKE_CXX_STANDARD 17)

# Check if SFML is already installed on the system
find_package(SFML 2.5 COMPONENTS graphics window system)
find_package(OpenGL REQUIRED)

if(NOT SFML_FOUND)
    # Add the FetchContent module (introduced in CMake 3.11)
    include(FetchContent)

    # Set the path where the external libraries will be downloaded and built
    set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/external)

    # SFML options
    option(SFML_USE_STATIC_STD_LIBS "Use static standard libraries with SFML" FALSE)
    set(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Link SFML statically")

    # Fetch and build SFML using FetchContent
    FetchContent_Declare(
        SFML
        URL https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip
    )
    FetchContent_GetProperties(SFML)
    if(NOT SFML_POPULATED)
        FetchContent_Populate(SFML)
        add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
    endif()

endif()

# Fetch and add glm using FetchContent
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/archive/0.9.9.8.tar.gz
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()


# Add your project's source files here
set(SOURCES
    src/cube.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories for your project
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glew/include
    path/to/glew/include
)

# Link SFML and GLEW libraries to your project
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    sfml-graphics
    sfml-window
    #sfml-system
    glm
    OpenGL32
    #glew
    ${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/Release/x64/glew32.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/Release/x64/glew32s.lib
)
