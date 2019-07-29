#include "objloader.hpp"

#include "../../glad/glad.h"

#include <glm/vec3.hpp>

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>


// Constructor

// OBJ loader constructor
OBJLoader::OBJLoader(const std::string &path) :
    ModelLoader(path) {}


// Methods

// Read data from file
bool OBJLoader::read() {
    // Open the model file and check it
    std::ifstream file(model_data.model_path);
    if (!file.is_open()) {
        std::cerr << "error: could not open the model `" << model_data.model_path << "'" << std::endl;
        return false;
    }

    // Model read variables
    std::vector<std::string> face;
    std::string token;
    std::string line;
    glm::vec3 data;
    GLuint count = 0U;

    // Read the file
    while(!file.eof()) {
        // Read line
        std::getline(file, line);

        // Skip comments
        if (line[0] == '#') {
            continue;
        }

        // Right trim line and skip empty line
        ModelLoader::rtrim(line);
        if (line.empty()) {
            continue;
        }

        // Create a string stream and read the first token
        std::istringstream stream(line);
        stream >> token;

        // Parse tokens
    }

    // Return true if not error has been found
    return true;
}