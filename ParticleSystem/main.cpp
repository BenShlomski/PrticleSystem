#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdlib>
#include "Particle.h"


#define _CRT_SECURE_NO_WARNINGS

constexpr auto PARTICLE_COUNT = 200;

constexpr auto CIRCLE_VERTECIES = 32;
constexpr auto SCREEN_WIDTH = 1920;
constexpr auto SCREEN_HEIGHT = 1080;


void updateParticles(Particle* particles, size_t particleCount, float timeStep);

void defaultCollisionHandle(Particle* particles, size_t particleCount);

void drawParticles(Particle* particles, size_t particleCount);

void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides);

void draweRectangle(Coordinate a, Coordinate b, size_t scalingFactor);

void processInput(GLFWwindow* window);

bool LoadTGAFile(const char* filename, TGAFILE* tgaFile);


int main(void)
{
    GLFWwindow* window;
    Particle* particles = new Particle[PARTICLE_COUNT];

    // randomize particles
    for (size_t i = 0; i < PARTICLE_COUNT; i++)
    {
        particles[i].randomizeParticle(15, 30, {60, 60, 0}, {SCREEN_WIDTH - 60, SCREEN_HEIGHT - 60, 0}, {900, 900, 0}, {0, 0, 0});
    }

    // randomize seed
    srand(time(NULL));

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MainWindow", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    //Load GLAD so it configures OpenGL
    gladLoadGL();

    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity(); // same as above comment

    // disable v-sync
    glfwSwapInterval(0);

    // fps variables
    float lastTime = glfwGetTime();
    float timeStep = 0;
    float previousFrameTime = lastTime;
    int nbFrames = 0;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        // Measure speed
        float currentTime = glfwGetTime();
        float timeDifference = currentTime - lastTime;
        nbFrames++;

        if (timeDifference >= 1.0)
        {
            timeStep = 1000.0 / nbFrames;

            // display to window and reset timer
            std::string ms = std::to_string(timeStep);
            std::string fps = std::to_string((float)nbFrames / timeDifference);

            std::string windowTitle = "ms/frame: " + ms + "; fps: " + fps;
            glfwSetWindowTitle(window, windowTitle.c_str());

            nbFrames = 0;
            lastTime += 1.0;
        }

        // change particle position and velocity
        timeStep = currentTime - previousFrameTime;
        previousFrameTime = currentTime;
        updateParticles(particles, PARTICLE_COUNT, timeStep);

        // render
        drawParticles(particles, PARTICLE_COUNT);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }
    delete[] particles;
    glfwTerminate();

    return 0;
}

void updateParticles(Particle* particles, size_t particleCount, float timeStep)
{
    for (size_t i = 0; i < particleCount; i++)
    {
        particles[i].update(timeStep, { 0, 0, 0 }, {SCREEN_WIDTH, SCREEN_HEIGHT, 0});
    }
}

void defaultCollisionHandle(Particle* particles, size_t particleCount)
{
    for (size_t i = 0; i < particleCount; i++)
    {
        for (size_t j = 0; j < particleCount; j++) 
        {
            particles[i].handleCollision(particles[j]);
        }
    }
}

void drawParticles(Particle* particles, size_t particleCount)
{
    Particle particle;

    for (size_t i = 0; i < particleCount; i++)
    {
        particle = particles[i];

        glColor3ub(particle.getColor().x, particle.getColor().y, particle.getColor().z); // set circle color
        drawCircle(particle.getPosition().x, particle.getPosition().y, particle.getPosition().z, particle.getRadius(), CIRCLE_VERTECIES);
    }
}

void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides)
{
    int numberOfVertices = numberOfSides + 2;

    GLfloat twicePi = 2.0f * M_PI;

    GLfloat* circleVerticesX = new GLfloat[numberOfVertices];
    GLfloat* circleVerticesY = new GLfloat[numberOfVertices];
    GLfloat* circleVerticesZ = new GLfloat[numberOfVertices];

    circleVerticesX[0] = x;
    circleVerticesY[0] = y;
    circleVerticesZ[0] = z;

    for (int i = 1; i < numberOfVertices; i++)
    {
        circleVerticesX[i] = x + (radius * cos(i * twicePi / numberOfSides));
        circleVerticesY[i] = y + (radius * sin(i * twicePi / numberOfSides));
        circleVerticesZ[i] = z;
    }

    GLfloat* allCircleVertices = new GLfloat[(numberOfVertices) * 3];

    for (int i = 0; i < numberOfVertices; i++)
    {
        allCircleVertices[i * 3] = circleVerticesX[i];
        allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
        allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfVertices);
    glDisableClientState(GL_VERTEX_ARRAY);

    delete[] circleVerticesX;
    delete[] circleVerticesY;
    delete[] circleVerticesZ;

    delete[] allCircleVertices;
}

void draweRectangle(Coordinate a, Coordinate b, size_t scalingFactor)
{
    GLfloat rectangleVertices[4 * 2];

    rectangleVertices[2 * 0 + 0] = a.x - scalingFactor;
    rectangleVertices[2 * 0 + 1] = b.y + scalingFactor;
    rectangleVertices[2 * 1 + 0] = a.x - scalingFactor;
    rectangleVertices[2 * 1 + 1] = a.y - scalingFactor;
    rectangleVertices[2 * 2 + 0] = b.x + scalingFactor;
    rectangleVertices[2 * 2 + 1] = a.y - scalingFactor;
    rectangleVertices[2 * 3 + 0] = b.x + scalingFactor;
    rectangleVertices[2 * 3 + 1] = b.y + scalingFactor;

    glColor3ub(255, 255, 255);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), rectangleVertices);
    glVertexPointer(2, GL_FLOAT, 0, rectangleVertices);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

bool LoadTGAFile(const char* filename, TGAFILE* tgaFile)
{
    FILE* filePtr;
    unsigned char ucharBad;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;

    // Open the TGA file.
    fopen_s(&filePtr, filename, "rb");
    if (filePtr == NULL)
    {
        return false;
    }

    // Read the two first bytes we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Which type of image gets stored in imageTypeCode.
    fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tgaFile->imageTypeCode != 2 && tgaFile->imageTypeCode != 3)
    {
        fclose(filePtr);
        return false;
    }

    // Read 13 bytes of data we don't need.
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);

    // Read the image's width and height.
    fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr);
    fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr);

    // Read the bit depth.
    fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr);

    // Read one byte of data we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Color mode -> 3 = BGR, 4 = BGRA.
    colorMode = tgaFile->bitCount / 8;
    imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;

    // Allocate memory for the image data.
    tgaFile->imageData = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);

    // Read the image data.
    fread(tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr);

    // Change from BGR to RGB so OpenGL can read the image data.
    for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        colorSwap = tgaFile->imageData[imageIdx];
        tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
        tgaFile->imageData[imageIdx + 2] = colorSwap;
    }

    fclose(filePtr);
    return true;
}
//
//
//int main(int argc, char** argv)
//{
//    //create GL context
//    GLFWwindow* window;
//    
//    // Initialize the library
//    if (!glfwInit())
//    {
//        return -1;
//    }
//    
//    // Create a windowed mode window and its OpenGL context
//    window = glfwCreateWindow(1920, 1080, "MainWindow", NULL, NULL);
//    
//    if (!window)
//    {
//        glfwTerminate();
//        return -1;
//    }
//    
//    // Make the window's context current
//    glfwMakeContextCurrent(window);
//    
//    //Load GLAD so it configures OpenGL
//    gladLoadGL();
//
//    TGAFILE image;
//    LoadTGAFile("C:/Users/Yonatan/source/repos/ParticleSystem/flipbooks/Explosion02_5x5.tga", &image);
//
//    //upload to GPU texture
//    GLuint tex;
//    glGenTextures(1, &tex);
//    glBindTexture(GL_TEXTURE_2D, tex);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 8, 8, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image.imageData);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    //match projection to window resolution (could be in reshape callback)
//    glMatrixMode(GL_PROJECTION);
//    glOrtho(0, 1920, 0, 1080, -1, 1);
//    glMatrixMode(GL_MODELVIEW);
//
//    //clear and draw quad with texture (could be in display callback)
//    glClear(GL_COLOR_BUFFER_BIT);
//    glBindTexture(GL_TEXTURE_2D, tex);
//    glEnable(GL_TEXTURE_2D);
//    glBegin(GL_QUADS);
//    glTexCoord2i(0, 0); glVertex2i(100, 100);
//    glTexCoord2i(0, 1); glVertex2i(100, 500);
//    glTexCoord2i(1, 1); glVertex2i(500, 500);
//    glTexCoord2i(1, 0); glVertex2i(500, 100);
//    glEnd();
//    glDisable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    
//    glfwSwapBuffers(window);
//    free(image.imageData);
//
//    getchar(); //pause so you can see what just happened
//    //System("pause"); //I think this works on windows
//
//    return 0;
//}