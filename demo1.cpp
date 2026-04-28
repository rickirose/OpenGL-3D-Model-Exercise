/******************************************************************************
 * This demo draws a triangle by defining its vertices in 3 dimensions
 * (the 3rd dimension is currently ignored and is just set to 0).
 *
 * The drawing is accomplished by:
 * - Uploading the vertices to the GPU using a Vertex Buffer Object (VBO).
 * - Specifying the vertices' format using a Vertex Array Object (VAO).
 * - Using a GLSL shader program (consisting of a simple vertex shader and a
 *   simple fragment shader) to actually draw the vertices as a triangle.
 *
 * Happy hacking! - eric
 * 
 * Camera controls: WASD for forward, left, back, right; QE for up and down
 *****************************************************************************/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <gdev.h>

// change this to your desired window attributes
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE  "Hello Teto!"
GLFWwindow *pWindow;

// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao[3];         // vertex array object (stores the render state for our vertex array)
GLuint vbo[3];         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shaderLeft;      // combined vertex and fragment shader
GLuint shaderMid;      // combined vertex and fragment shader
GLuint shaderRight;      // combined vertex and fragment shader
int totalVertexCount = 0; // global count > static array
GLuint hairTexture;
GLuint hairDisplacement;
GLuint skinTexture;
GLuint eyeTexture;

// define camera variables
glm::vec3 eyePosition = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::mat4 viewTransform;

std::vector<float> vertices;

// define all asymmetrical triangles
float asymmetrical[] = 
{
    // position (x, y, z) color (r, g, b) texture coordinates (s,t) partID normal (x,y,z)
    -0.168531f, 0.092728f,  0.649406f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.0f,  0.003457f,  0.543223f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // ahoge
     0.306651f,  0.053522f,  0.771129f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

     0.0f, -0.380293f, 0.327921f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.176f, -0.25f,  0.102765f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bangs L
     0.107574f, -0.441076f, -0.113183f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

     0.0f, -0.380293f, 0.327921f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.107574f, -0.441076f, -0.113183f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bangs R
     0.176f, -0.25f,  0.102765f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.176f, -0.25f,  0.102765f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.0f, -0.358676f, -0.042289f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.107574f, -0.441076f, -0.113183f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

     0.176f, -0.25f,  0.102765f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, -0.358676f, -0.042289f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.107574f, -0.441076f, -0.113183f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

float leftHead[] =
{
    // position (x, y, z) color (r, g, b)
    // HAIR COLOR:  0.878f, 0.184f, 0.184f
    // SHADE COLOR:  0.769f, 0.075f, 0.184f

     0.0f,  0.003457f,  0.543223f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.654628f, -0.247f, 0.186542f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, -0.380293f, 0.327921f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

     0.0f, -0.380293f, 0.327921f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.654628f, -0.247f, 0.186542f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.464363f, -0.147643f, 0.163961f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

     0.0f, -0.380293f, 0.327921f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.464363f, -0.147643f, 0.163961f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.523936f, 0.0f, -0.342026f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.263946f, -0.191512f, -0.005449f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.523936f, 0.0f, -0.342026f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.343384f,  0.0f, -0.163961f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

     0.0f,  0.003457f,  0.543223f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.654628f, -0.247f, 0.186542f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //backside
     0.0f, 0.435191f, 0.327921f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

     0.0f, 0.435191f, 0.327921f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.654628f, -0.247f, 0.186542f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.464363f, -0.147643f, 0.163961f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

     0.0f, 0.435191f, 0.327921f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.464363f, -0.147643f,  0.163961f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.523936f, 0.0f, -0.342026f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.263946f,  0.189836f, 0.0f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.523936f, 0.0f, -0.342026f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.343384f,  0.0f, -0.163961f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.263946f, 0.189836f, 0.0f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, 0.484842f, -0.348745f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, 0.435191f, 0.327921f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.263946f, 0.189836f, 0.0f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, 0.484842f, -0.348745f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, 0.326422f, -0.281858f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.672315f, -0.258871f, 0.353843f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.934572f, -0.00783f,  0.43522f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // twindrill top
    -0.674413f, -0.007531f, 0.019703f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    
    -0.414285f, -0.003489f, 0.43522f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.672315f, -0.258871f, 0.353843f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674413f, -0.007531f, 0.019703f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.676419f,  0.232991f, 0.397372f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674413f, -0.007531f, 0.019703f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.934572f, -0.00783f,  0.43522f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.676419f,  0.232991f, 0.397372f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674413f, -0.007531f, 0.019703f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.414285f, -0.003489f, 0.43522f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.676419f,  0.232991f, 0.397372f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.672315f, -0.258871f, 0.353843f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.934572f, -0.00783f,  0.43522f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.673125f, -0.161781f, 0.003175f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.834824f, -0.006998f, 0.053349f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // twindrill middle
    -0.674419f, -0.006813f, -0.202845f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.514032f, -0.004321f, 0.053349f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.673125f, -0.161781f, 0.003175f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674419f, -0.006813f, -0.202845f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.675656f,  0.141485f, 0.030013f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674419f, -0.006813f, -0.202845f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  //insert?
    -0.834824f, -0.006998f, 0.053349f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.675656f,  0.141485f, 0.030013f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.673125f, -0.161781f, 0.003175f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.834824f, -0.006998f, 0.053349f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.514032f, -0.004321f, 0.053349f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.675656f,  0.141485f, 0.030013f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674419f, -0.006813f, -0.202845f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.675656f,  0.141485f, 0.030013f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.673125f, -0.161781f, 0.003175f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.834824f, -0.006998f, 0.053349f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.675656f,  0.141485f, 0.030013f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.514032f, -0.004321f, 0.053349f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.673125f, -0.161781f, 0.003175f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.673766f, -0.084983f, -0.208296f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.755923f, -0.006339f, -0.182803f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // twindrill bottom
    -0.674423f, -0.006246f, -0.312972f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.592933f, -0.004979f, -0.182803f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.673766f, -0.084983f, -0.208296f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674423f, -0.006246f, -0.312972f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    
    -0.675052f,  0.069103f, -0.194659f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674423f, -0.006246f, -0.312972f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.755923f, -0.006339f, -0.182803f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.592933f, -0.004979f, -0.182803f, 0.769f, 0.075f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.675052f,  0.069103f, -0.194659f, 0.769f, 0.075f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.674423f, -0.006246f, -0.312972f, 0.769f, 0.075f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.675052f,  0.069103f, -0.194659f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.673766f, -0.084983f, -0.208296f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.755923f, -0.006339f, -0.182803f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

    -0.675052f,  0.069103f, -0.194659f, 0.878f, 0.184f, 0.184f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.592933f, -0.004979f, -0.182803f, 0.878f, 0.184f, 0.184f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    -0.673766f, -0.084983f, -0.208296f, 0.878f, 0.184f, 0.184f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 


    // SKIN COLOR:  1.0f, 0.933f, 0.867f
    // SHADE COLOR: 0.941f, 0.82f, 0.741f
    -0.170837f, -0.238198f, 0.10691f, 1.0f, 0.933f, 0.867f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
    -0.263946f, -0.191512f, -0.005449f, 1.0f, 0.933f, 0.867f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, -0.358676f, -0.042289f, 1.0f, 0.933f, 0.867f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 

     0.0f, -0.358676f, -0.042289f, 1.0f, 0.933f, 0.867f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
    -0.263946f, -0.191512f, -0.005449f, 1.0f, 0.933f, 0.867f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, -0.305175f, -0.351608f, 1.0f, 0.933f, 0.867f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 

    -0.263946f, -0.191512f, -0.005449f, 1.0f, 0.933f, 0.867f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
    -0.343384f,  0.0f, -0.163961f, 1.0f, 0.933f, 0.867f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, -0.305175f, -0.351608f, 1.0f, 0.933f, 0.867f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 

    -0.343384f,  0.0f, -0.163961f, 0.941f, 0.82f, 0.741f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
     0.0f,  0.003457f, -0.434348f, 0.941f, 0.82f, 0.741f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
     0.0f, -0.305175f, -0.351608f, 0.941f, 0.82f, 0.741f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 

    -0.263946f,  0.189836f, -0.005449f, 0.941f, 0.82f, 0.741f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
    -0.343384f,  0.0f, -0.163961f, 0.941f, 0.82f, 0.741f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
     0.0f,  0.326422f, -0.281858f, 0.941f, 0.82f, 0.741f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 

     0.0f,  0.326422f, -0.281858f, 1.0f, 0.933f, 0.867f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
    -0.343384f,  0.0f, -0.163961f, 1.0f, 0.933f, 0.867f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
     0.0f,  0.003457f, -0.434348f, 1.0f, 0.933f, 0.867f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
    

    // EYE COLOR: 0.329f, 0.027f, 0.09f
    -0.156485f, -0.256877f, -0.022829f, 0.329f, 0.027f, 0.09f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 
    -0.249028f, -0.185477f, -0.113493f, 0.329f, 0.027f, 0.09f, 1.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 
    -0.093429f, -0.297656f, -0.134674f, 0.329f, 0.027f, 0.09f, 0.5f, 1.0f, 2.0f, 0.0f, 0.0f, 0.0f
};

void symmetrize(std::vector<float>& vertices) 
{
    std::vector<float> mirror;
    size_t listSize = vertices.size();
    for (size_t i = 0; i < listSize; i += 36) // 3 vertices * 12 floats each
    { 
        // instantiate triangle points
        float* v1 = &vertices[i];
        float* v2 = &vertices[i + 12];
        float* v3 = &vertices[i + 24];

        // mirror math: mirror on the x axis (include normalx)
        auto pushMirror = [&](float* data)
        {
            mirror.push_back(-data[0]); // flip x
            // retain y,z,r,g,b,s,t,partID
            for (int j = 1; j < 9; j++) { mirror.push_back(data[j]); }
            mirror.push_back(-data[9]); //flip normal x
            mirror.push_back(data[10]); // retain norm y
            mirror.push_back(data[11]); // retain norm z
        };

        // push and swap winding order: z and y swap
        pushMirror(v1);
        pushMirror(v3);
        pushMirror(v2);
    }
    vertices.insert(vertices.end(), mirror.begin(), mirror.end());
}


void computeNormals(std::vector<float>& verts)
{
    const int STRIDE = 12;
    for (size_t i =0; i + 3 * STRIDE <= verts.size(); i += 3 * STRIDE) // per triangle
    {
        // get vertex coords of triangles
        glm::vec3 A(verts[i+0], verts[i+1], verts[i+2]);
        glm::vec3 B(verts[i+12], verts[i+13], verts[i+14]);
        glm::vec3 C(verts[i+24], verts[i+25], verts[i+26]);

        // normal = (B-A) x (C-A)
        glm::vec3 n = glm::normalize(glm::cross(B-A, C-A));

        // overwrite vertex normals
        for (int v=0; v<3; v++)
        {
            verts[i+v*STRIDE+9] = n.x;
            verts[i+v*STRIDE+10] = n.y;
            verts[i+v*STRIDE+11] = n.z;
        }
    }
}

// called by the main function to do initial setup, such as uploading vertex
// arrays, shader programs, etc.; returns true if successful, false otherwise
bool setup()
{
    // add left side to vertex array
    int leftHeadCount = sizeof(leftHead) / sizeof(float);
    vertices.insert(vertices.end(), leftHead, leftHead + leftHeadCount);

    // add right side to vertex array
    symmetrize(vertices);

    // add asymmetrical pieces to vertex array
    int asymCount = sizeof(asymmetrical) / sizeof(float);
    vertices.insert(vertices.end(), asymmetrical, asymmetrical + asymCount);

    // compute per-triangle normals for all vertices
    computeNormals(vertices);

    // update vertex count (stride=12)
    totalVertexCount = vertices.size() / 12;

    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(3, vao);
    glGenBuffers(3, vbo);

    for(int i = 0; i < 3; i++)
    {
        // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
        glBindVertexArray(vao[i]);

        // upload our vertex array data to the newly-created VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
            // on the VAO, register the current VBO with the following vertex attribute layout:
        // - layout location 0...
        // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
        // - ... its values will NOT be normalized (GL_FALSE)
        // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
        // - ... and we start at the beginning of the array (hence, (void*) 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*) 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*) (3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*) (6 * sizeof(float)));
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*) (8 * sizeof(float)));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*) (9 * sizeof(float))); //normal: 3floats, offset 9
        

        // enable the newly-created layout location 0;
        // this shall be used by our vertex shader to read the vertex's x, y, and z
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);
    }

    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    hairTexture = gdevLoadTexture("hair.png", GL_REPEAT, true, true);
    if(!hairTexture) return false;
    hairDisplacement = gdevLoadTexture("hairMap.png", GL_REPEAT, true, true);
    if(!hairDisplacement) return false;
    skinTexture = gdevLoadTexture("skin.png", GL_MIRRORED_REPEAT, true, true);
    if(!skinTexture) return false;
    eyeTexture = gdevLoadTexture("eye.png", GL_MIRRORED_REPEAT, true, true);
    if(!eyeTexture) return false;
    
    // load our shader program
    shaderLeft = gdevLoadShader("left.vs", "left.fs");
    if (! shaderLeft)
        return false;
    shaderMid = gdevLoadShader("mid.vs", "mid.fs");
    if (! shaderMid)
        return false;
    shaderRight = gdevLoadShader("right.vs", "right.fs");
    if (! shaderRight)
        return false;

    return true;
}

// called by the main function to do rendering per frame
void render()
{
    float time = (float)glfwGetTime();
    // clear the whole frame
    glClearColor(1.0f*cos(time), 0.3f*sin(time), 0.3f*cos(time), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable OpenGL's hidden surface removal
    glEnable(GL_DEPTH_TEST);

    // camera controls
    float moveSpeed = 0.05f;
    
    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
        eyePosition += moveSpeed * cameraFront;
    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
        eyePosition -= moveSpeed * cameraFront;
    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
        eyePosition -= moveSpeed * glm::normalize(glm::cross(cameraFront, upVector));
    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
        eyePosition += moveSpeed * glm::normalize(glm::cross(cameraFront, upVector));
    if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS)
        eyePosition += moveSpeed * upVector;
    if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS)
        eyePosition -= moveSpeed * upVector;
    
    viewTransform = glm::lookAt(eyePosition, eyePosition + cameraFront, upVector);

    //calculate projection matrix
    float fov = glm::radians(60.0f);
    float aspect = (float) WINDOW_WIDTH / WINDOW_HEIGHT;
    float near = 0.1f, far = 100.0f;

    glm::mat4 projectionViewMatrix;
    projectionViewMatrix = glm::perspective(fov, aspect, near, far); 
    projectionViewMatrix *= glm::lookAt(eyePosition, eyePosition + cameraFront, upVector);

    // lil bounce for kasane teto!
    float bounce = sin(time * 3.0f) * 0.2f;

    // ... normal and model matrices for the 3 instances
    // middle model
    glm::mat4 modelMatrixM = glm::mat4(1.0f);
    modelMatrixM = glm::translate(modelMatrixM, glm::vec3(0.0f, bounce, -3.0f));
    modelMatrixM = glm::rotate(modelMatrixM, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrixM = glm::rotate(modelMatrixM, time*2, glm::vec3(0.0f, 0.0f, 1.0f));
    
    glm::mat4 normalMatrixM = glm::transpose(glm::inverse(modelMatrixM));

    // left model
    glm::mat4 modelMatrixL = glm::mat4(1.0f);
    modelMatrixL = glm::translate(modelMatrixL, glm::vec3(-2.5f, -1 + bounce, -6.0f));
    modelMatrixL = glm::rotate(modelMatrixL, glm::radians(45.0f*time), glm::vec3(1.0f, 1.0f, 0.0f));
    modelMatrixL = glm::rotate(modelMatrixL, time*2, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrixL = glm::scale(modelMatrixL, glm::vec3(3.0f * sin(time), 2.0f*sin(time), 2.0f));

    glm::mat4 normalMatrixL = glm::transpose(glm::inverse(modelMatrixL));

    // right model
    glm::mat4 modelMatrixR = glm::mat4(1.0f);
    modelMatrixR = glm::translate(modelMatrixR, glm::vec3(2.0f, 1.0 + bounce, -3.0f));
    modelMatrixR = glm::rotate(modelMatrixR, glm::radians(-90.0f*time), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrixR = glm::rotate(modelMatrixR, time/2, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrixR = glm::scale(modelMatrixR, glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 normalMatrixR = glm::transpose(glm::inverse(modelMatrixR));

    // ... draw our triangles
    // mid shader
    glUseProgram(shaderMid);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hairTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, hairDisplacement);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, skinTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, eyeTexture);

    glUniform1f(glGetUniformLocation(shaderMid, "time"), time);
    glUniform1i(glGetUniformLocation(shaderMid, "hair"), 0);
    glUniform1i(glGetUniformLocation(shaderMid, "hairD"), 1);
    glUniform1i(glGetUniformLocation(shaderMid, "skin"), 2);
    glUniform1i(glGetUniformLocation(shaderMid, "eye"), 3);

    glUniformMatrix4fv(glGetUniformLocation(shaderMid, "projectionViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrixM));
    glUniformMatrix4fv(glGetUniformLocation(shaderMid, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrixM));
    glUniformMatrix4fv(glGetUniformLocation(shaderMid, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrixM));

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, totalVertexCount);
    glBindVertexArray(0);

    // left shader
    glUseProgram(shaderLeft);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hairTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, hairDisplacement);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, skinTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, eyeTexture);

    glUniform1f(glGetUniformLocation(shaderLeft, "time"), time);
    glUniform1i(glGetUniformLocation(shaderLeft, "hair"), 0);
    glUniform1i(glGetUniformLocation(shaderLeft, "hairD"), 1);
    glUniform1i(glGetUniformLocation(shaderLeft, "skin"), 2);
    glUniform1i(glGetUniformLocation(shaderLeft, "eye"), 3);

    glUniformMatrix4fv(glGetUniformLocation(shaderLeft, "projectionViewMatrix"), 1, GL_FALSE, glm::value_ptr(projectionViewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderLeft, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrixL));
    glUniformMatrix4fv(glGetUniformLocation(shaderLeft, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrixL));

    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, totalVertexCount);
    glBindVertexArray(0);

    // right shader
    glUseProgram(shaderRight);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hairTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, hairDisplacement);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, skinTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, eyeTexture);
    glUniform1f(glGetUniformLocation(shaderRight, "time"), time);
    glUniform1i(glGetUniformLocation(shaderRight, "hair"), 0);
    glUniform1i(glGetUniformLocation(shaderRight, "hairD"), 1);
    glUniform1i(glGetUniformLocation(shaderRight, "skin"), 2);
    glUniform1i(glGetUniformLocation(shaderRight, "eye"), 3);

    glUniformMatrix4fv(glGetUniformLocation(shaderRight, "projectionViewMatrix"), 1, GL_FALSE, glm::value_ptr(projectionViewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderRight, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrixR));
    glUniformMatrix4fv(glGetUniformLocation(shaderRight, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrixR));

    glBindVertexArray(vao[2]);
    glDrawArrays(GL_TRIANGLES, 0, totalVertexCount);
    glBindVertexArray(0);
}

/*****************************************************************************/

// handler called by GLFW when there is a keyboard event
void handleKeys(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // pressing Esc closes the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
}

// handler called by GLFW when the window is resized
void handleResize(GLFWwindow* pWindow, int width, int height)
{
    // tell OpenGL to do its drawing within the entire "client area" (area within the borders) of the window
    glViewport(0, 0, width, height);
}

// main function
int main(int argc, char** argv)
{
    // initialize GLFW and ask for OpenGL 3.3 core
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create a GLFW window with the specified width, height, and title
    pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (! pWindow)
    {
        // gracefully terminate if we cannot create the window
        std::cout << "Cannot create the GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    // make the window the current context of subsequent OpenGL commands,
    // and enable vertical sync and aspect-ratio correction on the GLFW window
    glfwMakeContextCurrent(pWindow);
    glfwSwapInterval(1);
    glfwSetWindowAspectRatio(pWindow, WINDOW_WIDTH, WINDOW_HEIGHT);

    // set up callback functions to handle window system events
    glfwSetKeyCallback(pWindow, handleKeys);
    glfwSetFramebufferSizeCallback(pWindow, handleResize);

    // don't miss any momentary keypresses
    glfwSetInputMode(pWindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    // initialize GLAD, which acts as a library loader for the current OS's native OpenGL library
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // if our initial setup is successful...
    if (setup())
    {
        // do rendering in a loop until the user closes the window
        while (! glfwWindowShouldClose(pWindow))
        {
            // render our next frame
            // (by default, GLFW uses double-buffering with a front and back buffer;
            // all drawing goes to the back buffer, so the frame does not get shown yet)
            render();

            // swap the GLFW front and back buffers to show the next frame
            glfwSwapBuffers(pWindow);

            // process any window events (such as moving, resizing, keyboard presses, etc.)
            glfwPollEvents();
        }
    }

    // gracefully terminate the program
    glfwTerminate();
    return 0;
}