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

constexpr auto PARTICLE_COUNT = 9;
constexpr auto FULL_SCREEN = true;

constexpr auto CIRCLE_VERTECIES = 32;
constexpr auto SCREEN_WIDTH = 1920;
constexpr auto SCREEN_HEIGHT = 1080;


void updateParticles(Particle* particles, size_t particleCount, double timeStep);

void drawParticles(Particle* particles, size_t particleCount);

void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides);

void processInput(GLFWwindow* window);


int main(void)
{
    GLFWwindow* window;
    Particle* particles = new Particle[PARTICLE_COUNT];

    // randomize particles
    for (size_t i = 0; i < PARTICLE_COUNT; i++)
    {
        particles[i].randomizeParticle(70, 100, {60, 60, 0}, {SCREEN_WIDTH - 60, SCREEN_HEIGHT - 60, 0}, {900, 90, 0}, {0, 0, 0});
    }

    // randomize seed
    srand(time(NULL));

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWmonitor* monitor = NULL;
    if (FULL_SCREEN)
    {
        monitor = glfwGetPrimaryMonitor();
    }

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MainWindow", monitor, NULL);

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
    double lastTime = glfwGetTime();
    double timeStep = 0;
    double previousFrameTime = lastTime;
    int nbFrames = 0;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        // Measure speed
        double currentTime = glfwGetTime();
        double timeDifference = currentTime - lastTime;
        nbFrames++;

        if (timeDifference >= 1.0)
        {
            timeStep = 1000.0 / nbFrames;

            // display to window and reset timer
            std::string ms = std::to_string(timeStep);
            std::string fps = std::to_string((double)nbFrames / timeDifference);

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

void updateParticles(Particle* particles, size_t particleCount, double timeStep)
{
    for (size_t i = 0; i < particleCount; i++)
    {
        particles[i].update(timeStep, { 0, 0, 0 }, {SCREEN_WIDTH, SCREEN_HEIGHT, 0});
    }

    for (size_t i = 0; i < particleCount; i++)
    {
        for (size_t j = 0; j < particleCount; j++)
        {
            particles[i].handleCollision(particles[j]);
        }
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

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}