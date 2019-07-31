#include "scene/scene.hpp"

#include "dirsep.h"

#include <iostream>


/** Main function */
int main (int argc, char **argv) {
    // Print arguments
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }


    // Create the scene and check it
    Scene *scene = new Scene("OBJViewer");

    // Exit with error if the scene is not valid
    if (!scene->isValid()) {
        delete scene;
        return 1;
    }


    // Setup directories
    const std::string bin_path = argv[0];
    const std::string relative = bin_path.substr(0U, bin_path.find_last_of(DIR_SEP) + 1U);

    const std::string model_path  = relative + ".." + DIR_SEP + "model"  + DIR_SEP;
    const std::string shader_path = relative + ".." + DIR_SEP + "shader" + DIR_SEP;


    // Add the programs
    const std::string common_vert_shader = shader_path + "common.vert.glsl";
    Scene::setDefaultProgram(common_vert_shader, shader_path + "normals.frag.glsl");

    // Add the models
    scene->addModel(model_path + "suzanne" + DIR_SEP + "suzanne.obj");


    // Esecute the main loop
    scene->mainLoop();


    // Clean up and normal exit
    delete scene;
    return 0;
}