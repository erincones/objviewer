#include "scene/scene.hpp"

#include "dirsep.h"

#include <iostream>

#include <memory>


/** Main function */
int main (int argc, char **argv) {
    // Print arguments
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }

    // Create the scene and check it
    std::unique_ptr<Scene> scene = std::unique_ptr<Scene>(new Scene("OBJViewer"));

    // Exit with error if the scene is not valid
    if (!scene->isValid()) {
        return 1;
    }

    // Setup directories
    const std::string bin_path = argv[0];
    const std::string relative = bin_path.substr(0U, bin_path.find_last_of(DIR_SEP) + 1U);

    const std::string model_path  = relative + ".." + DIR_SEP + "model"  + DIR_SEP;
    const std::string shader_path = relative + ".." + DIR_SEP + "shader" + DIR_SEP;

    // Add GLSL program and model
    std::size_t normals_program = scene->addProgram(shader_path + "common.vert.glsl", shader_path + "normals.frag.glsl");
    scene->addModel(model_path + "suzanne" + DIR_SEP + "suzanne.obj", normals_program);
    

    // Esecute the main loop
    scene->mainLoop();

    // Normal exit
    return 0;
}