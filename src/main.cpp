#include "scene/scene.hpp"

#include <iostream>


/** Main function */
int main (int argc, char **argv) {
    // Print arguments
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }

    // Initialize GLFW and exit with error if cannot initialize
    if (!Scene::initializeGLFW()) {
        return 1;
    }

    // Create the scene and execute the main loop
    Scene *scene = new Scene("OBJViewer");
    scene->mainLoop();

    // Clean up
    delete scene;
    Scene::terminateGLFW();

    // Normal exit
    return 0;
}