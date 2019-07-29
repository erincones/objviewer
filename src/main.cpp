#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>


/** GLFW error callback */
void error_callback(int error, const char *description);


/** Main function */
int main (int argc, char **argv) {
    // Print arguments
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }

    // Setup error callback
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "error: cannot initialize GLFW" << std::endl;
        return 1;
    }

    // Set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the application window
    GLFWwindow *window = glfwCreateWindow(800, 600, "OBJViewer", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "error: cannot create the application window" << std::endl;

        // Terminate GLFW and exit
        glfwTerminate();
        return 1;
    }

    // Maximize window and set as the current context
    glfwMaximizeWindow(window);
    glfwMakeContextCurrent(window);

    // Initialize Glad
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        std::cerr << "error: cannot initialize glad" << std::endl;

        // Clean up
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Setup the swap interval
    glfwSwapInterval(1);

    // Create viewport
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Set the clear color
    glClearColor(0.45F, 0.55F, 0.6F, 1.00F);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);


    // Main loop
    while (glfwWindowShouldClose(window) != GLFW_TRUE) {
        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


// GLFW error callback
void error_callback(int error, const char *description) {
    std::cerr << "error " << error << ": " << description << std::endl;
}