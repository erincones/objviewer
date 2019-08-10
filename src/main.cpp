#include "scene/gui/interactivescene.hpp"

#include "dirsep.h"

#include <glm/vec3.hpp>

#include <iostream>


/** Main function */
int main (int argc, char **argv) {
    // Print arguments
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }


    // Create the scene and check it
    InteractiveScene *scene = new InteractiveScene("OBJViewer");

    // Exit with error if the scene is not valid
    if (!scene->isValid()) {
        delete scene;
        return 1;
    }

    // Set the background color
    scene->setBackgroundColor(glm::vec3(0.45F, 0.55F, 0.60F));


    // Setup directories
    const std::string bin_path = argv[0];
    const std::string relative = bin_path.substr(0U, bin_path.find_last_of(DIR_SEP) + 1U);

    const std::string model_path  = relative + ".." + DIR_SEP + "model"  + DIR_SEP;
    const std::string shader_path = relative + ".." + DIR_SEP + "shader" + DIR_SEP;


    // Add the programs
    const std::string common_lp_vert_path = shader_path + "light_pass_common.vert.glsl";
    scene->setDefaultGeometryPassProgram("Deferred shading", shader_path + "geom_pass.vert.glsl", shader_path + "geom_pass.frag.glsl");
    scene->setDefaultLightingPassProgram("Surface normals",  common_lp_vert_path,                 shader_path + "lp_normals.frag.glsl");

    scene->addProgram("Positions", common_lp_vert_path, shader_path + "lp_positions.frag.glsl");

    std::size_t program_0 = scene->addProgram("Blinn-Phong", common_lp_vert_path, shader_path + "lp_blinn_phong.frag.glsl");
    scene->setLightingPassProgram(program_0);

    // Add the models
    std::size_t model_id_0 = scene->addModel(model_path + "nanosuit" + DIR_SEP + "nanosuit.obj");
    std::size_t model_id_1 = scene->addModel(model_path + "suzanne"  + DIR_SEP + "suzanne.obj");
    std::size_t model_id_2 = scene->addModel(model_path + "box"      + DIR_SEP + "box.obj");

    // Setup models
    Model *model = scene->getModel(model_id_0);

    model = scene->getModel(model_id_1);
    model->setScale(glm::vec3(0.45F));
    model->setPosition(glm::vec3(0.6F, 0.225F, 0.0F));

    model = scene->getModel(model_id_2);
    model->setScale(glm::vec3(0.45F));
    model->setPosition(glm::vec3(0.6F, -0.225F, 0.0F));


    // Esecute the main loop
    scene->mainLoop();


    // Clean up and normal exit
    delete scene;
    return 0;
}