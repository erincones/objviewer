#include "scene/scene.hpp"

#include <iostream>

#include <memory>


/** Main function */
int main (int argc, char **argv) {
    // Print arguments
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }

    // Create the scene and execute the main loop
    std::unique_ptr<Scene> scene = std::unique_ptr<Scene>(new Scene("OBJViewer"));
    scene->mainLoop();

    // Normal exit
    return 0;
}