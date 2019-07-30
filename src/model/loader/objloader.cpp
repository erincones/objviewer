#include "objloader.hpp"

#include "../../dirsep.h"

#include "../../glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <algorithm>
#include <vector>

#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>

#include <memory>


// Private methods

// Parse, store the vertex and returns its index
GLsizei OBJLoader::storeVertex(const std::string &vertex_str) {
    // Search the vertex
    std::map<std::string, GLsizei>::iterator result = parsed_vertex.find(vertex_str);

    // Return the index of already parsed vertex
    if (result != parsed_vertex.end()) {
        index_stock.push_back(result->second);
        return result->second;
    }
    

    // Create a new vertex
    ModelLoader::Vertex vertex;

    // Parse vertex indices data
    for (std::size_t i = 0, begin = 0, end = 0; (i < 3) && (end != std::string::npos); i++, begin = end + 1) {
        // Find next attribute position
        end = vertex_str.find('/', begin);

        // Parse index and get attribute
        if ((begin < vertex_str.size()) && (begin < end)) {
            switch (i) {
                case 0: vertex.position = position_stock[std::stoi(vertex_str.substr(begin)) - 1]; break;
                case 1: vertex.uv_coord = uv_coord_stock[std::stoi(vertex_str.substr(begin)) - 1]; break;
                case 2: vertex.normal   = normal_stock[std::stoi(vertex_str.substr(begin)) - 1];
            }
        }
    }
    
    // Add vertex
    GLsizei index = static_cast<GLsizei>(vertex_stock.size());
    parsed_vertex[vertex_str] = index;
    index_stock.push_back(index);
    vertex_stock.push_back(vertex);

    // Return the index
    return index;
}

// Calculate the tangent vector for each vertex of a triangle
void OBJLoader::calcTangent(const GLsizei &ind_0, const GLsizei &ind_1, const GLsizei &ind_2) {
    // Get vertices
    ModelLoader::Vertex &vertex_0 = vertex_stock.at(ind_0);
    ModelLoader::Vertex &vertex_1 = vertex_stock.at(ind_1);
    ModelLoader::Vertex &vertex_2 = vertex_stock.at(ind_2);

    // Get position triangle edges
    const glm::vec3 l0(vertex_1.position - vertex_0.position);
    const glm::vec3 l1(vertex_2.position - vertex_0.position);

    // Get texture triangle edges
    const glm::vec2 d0(vertex_1.uv_coord - vertex_0.uv_coord);
    const glm::vec2 d1(vertex_2.uv_coord - vertex_0.uv_coord);

    // Calculate tangent
    const glm::vec3 tangent((l0 * d1.t - l1 * d0.t) / glm::abs(d0.s * d1.t - d1.s * d0.t));

    // Accumulate tangent
    vertex_0.tangent += tangent;
    vertex_1.tangent += tangent;
    vertex_2.tangent += tangent;
}

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
    GLsizei count = 0U;

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

        // Load material file
        if (token == "mtllib") {
            // Get the relative path to the material file
            stream >> std::ws;
            std::getline(stream, token);
            
            // Read mtl file
            readMTL(token);
        }

        // Use material for the next vertices
        else if ((token == "usemtl") && model_data.material_open) {
            // Set count to the previous object
            if (!model_data.object_stock.empty()) {
                model_data.object_stock.back()->count = static_cast<GLsizei>(index_stock.size()) - count;
                count = static_cast<GLsizei>(index_stock.size());
            }

            // Read material name
			stream >> std::ws;
			std::getline(stream, token);

            // Search in the stock
			std::shared_ptr<Material> material = nullptr;
            for (const std::shared_ptr<Material> &material_it : model_data.material_stock) {
                if (material_it->getName() == token) {
                    material = material_it;
                    break;
                }
            }

            // Create object and bind the material
            model_data.object_stock.emplace_back(std::make_shared<ModelData::Object>(0, count, material));
        }

        // Store vertex position
        else if (token == "v") {
            stream >> data.x >> data.y >> data.z;
            position_stock.push_back(data);

            // Update the position limits
            if (data.x < model_data.min.x) model_data.min.x = data.x;
            if (data.y < model_data.min.y) model_data.min.y = data.y;
            if (data.z < model_data.min.z) model_data.min.z = data.z;
            if (data.x > model_data.max.x) model_data.max.x = data.x;
            if (data.y > model_data.max.y) model_data.max.y = data.y;
            if (data.z > model_data.max.z) model_data.max.z = data.z;
        }

        // Store normal
        else if (token == "vn") {
            stream >> data.x >> data.y >> data.z;
            normal_stock.push_back(data);
        }

        // Store texture coordinate
        else if (token == "vt") {
            stream >> data.x >> data.y;
            uv_coord_stock.push_back(glm::vec2(data));
        }

        // Store face
        else if (token == "f") {
            // Read the face vertex data
            while (stream >> token) {
                face.push_back(token);
            }

            // First vertex
            const std::string &first = *face.begin();

            // Triangulate polygon
            for (std::vector<std::string>::iterator it = face.begin() + 2; it != face.end(); it++) {
                // Store the first and previous vertex
                const GLsizei ind_0 = storeVertex(first);
                const GLsizei ind_1 = storeVertex(*(it - 1));

                // Store the current vertex
                const GLsizei ind_2 = storeVertex(*it);

                // Calculate the tangent of the triangle
                calcTangent(ind_0, ind_1, ind_2);
            }

            // Clear faces vector
            face.clear();
        }
    }

    // Close file
    file.close();

    // Set count to the last object
    if (model_data.material_open) {
        model_data.object_stock.back()->count = static_cast<GLsizei>(index_stock.size()) - count;
    }

    // Create a default material and associate all vertices to it if the material file could not be open
	else {
        std::shared_ptr<Material> material = std::make_shared<Material>("default");
        model_data.material_stock.emplace_back(material);
        model_data.object_stock.emplace_back(std::make_shared<ModelData::Object>(static_cast<GLsizei>(index_stock.size()), 0, material));
    }

    // Orthogonalize tangents
    for (ModelLoader::Vertex &vertex : vertex_stock) {
        vertex.tangent = glm::normalize(vertex.tangent - vertex.normal * glm::dot(vertex.normal, vertex.tangent));
    }

    // Setup origin matrix
    glm::vec3 dim = model_data.max - model_data.min;
    float min_dim = 1.0F / glm::max(glm::max(dim.x, dim.y), dim.z);
    model_data.origin_mat = glm::translate(glm::scale(glm::mat4(1.0F), glm::vec3(min_dim)), (model_data.min + model_data.max) / -2.0F) ;

    // Save statistics
    model_data.vertices = position_stock.size();
    model_data.elements = vertex_stock.size();
    model_data.triangles = index_stock.size() / 3U;

    // Free memory
    parsed_vertex.clear();
    position_stock.clear();
    uv_coord_stock.clear();
    normal_stock.clear();

    // Return true if not error has been found
    model_data.model_open = true;
    return true;
}

// Read material data from file
bool OBJLoader::readMTL(const std::string &mtl) {
    // Get the relative directory and set the material file path
    std::string dir = model_data.model_path.substr(0, model_data.model_path.find_last_of(DIR_SEP) + 1);
    model_data.material_path = dir + mtl;

    // Open the material file and check it
    std::ifstream file(model_data.material_path);
    if (!file.is_open()) {
        std::cerr << "error: could not open the material file `" << model_data.material_path << "'" << std::endl;
        return false;
    }

    // Material description read variables
    std::shared_ptr<Material> material = nullptr;
    bool load_cube_map = false;
    std::string cube_map_path[6];
	std::string token;
	std::string line;
	glm::vec3 data;
	float value;

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

        // Convert token to lower case
		std::transform(token.begin(), token.end(), token.begin(), ::tolower);

        // New material
        if (token == "newmtl") {
            // Set cube map paths to the previous material
            if (load_cube_map) {
                material->setCubeMapTexturePath(cube_map_path);

                // Clear cube map paths
                load_cube_map = false;
                for (int i = 0; i < 6; i++) {
                    cube_map_path[i].clear();
                }
            }

            // Get left trimed full name
            stream >> std::ws;
			std::getline(stream, token);

            // Create the new material
            material = std::make_shared<Material>(token);
			model_data.material_stock.emplace_back(material);
        }


        // Colors

        // Ambient color
        else if (token == "ka") {
            stream >> data.x >> data.y >> data.z;
            material->setColor(data, Material::AMBIENT);
        }

        // Diffuse color
        else if (token == "kd") {
            stream >> data.x >> data.y >> data.z;
            material->setColor(data, Material::DIFFUSE);
        }

        // Specular color
        else if (token == "ks") {
            stream >> data.x >> data.y >> data.z;
            material->setColor(data, Material::SPECULAR);
        }

        // Transparency color
        else if (token == "tf") {
            stream >> data.x >> data.y >> data.z;
            material->setColor(data, Material::TRANSPARENCY);
        }


        // Values

        // Shininess
		else if (token == "ns") {
			stream >> value;
			material->setValue(value, Material::SHININESS);
		}

        // Disolve
		else if (token == "d") {
			stream >> value;
			material->setValue(1.0F - value, Material::TRANSPARENCY);
		}

		// Transparency
		else if (token == "tr") {
			stream >> value;
			material->setValue(value, Material::TRANSPARENCY);
		}

		// Refractive index
		else if (token == "ni") {
			stream >> value;
			material->setValue(value, Material::REFRACTIVE_INDEX);
		}


        // Textures

        // Ambient texture
        else if (token == "map_ka") {
            stream >> std::ws;
			std::getline(stream, token);
            material->setTexturePath(dir + token, Material::AMBIENT);
            model_data.textures++;
        }

        // Diffuse texture
        else if (token == "map_kd") {
            stream >> std::ws;
			std::getline(stream, token);
            material->setTexturePath(dir + token, Material::DIFFUSE);
            model_data.textures++;
        }

        // Specular texture
        else if (token == "map_ks") {
            stream >> std::ws;
			std::getline(stream, token);
            material->setTexturePath(dir + token, Material::SPECULAR);
            model_data.textures++;
        }

        // Shininess texture
        else if (token == "map_ns") {
            stream >> std::ws;
			std::getline(stream, token);
            material->setTexturePath(dir + token, Material::SHININESS);
            model_data.textures++;
        }

        // Normal texture
		else if ((token == "map_bump") || (token == "bump") || (token == "kn")) {
            stream >> std::ws;
			std::getline(stream, token);
            material->setTexturePath(dir + token, Material::NORMAL);
            model_data.textures++;
        }

        // Displacement texture
        else if (token == "disp") {
            stream >> std::ws;
			std::getline(stream, token);
            material->setTexturePath(dir + token, Material::DISPLACEMENT);
            model_data.textures++;
        }

        // Cube map texture
        else if (token == "refl") {
            // Is a cube map flag
            bool is_cube = false;

            // Skip the "-type" token and get the cube side
            stream >> token;
            stream >> token;

            // Right side
            if (token == "cube_right") {
                stream >> std::ws;
                std::getline(stream, token);
                cube_map_path[0] = dir + token;
                is_cube = true;
            }

            // Left side
            else if (token == "cube_left") {
                stream >> std::ws;
                std::getline(stream, token);
                cube_map_path[1] = dir + token;
                is_cube = true;
            }

            // Top side
            else if (token == "cube_top") {
                stream >> std::ws;
                std::getline(stream, token);
                cube_map_path[2] = dir + token;
                is_cube = true;
            }

            // Bottom side
            else if (token == "cube_bottom") {
                stream >> std::ws;
                std::getline(stream, token);
                cube_map_path[3] = dir + token;
                is_cube = true;
            }

            // Front side
            else if (token == "cube_front") {
                stream >> std::ws;
                std::getline(stream, token);
                cube_map_path[4] = dir + token;
                is_cube = true;
            }

            // Back side
            else if (token == "cube_back") {
                stream >> std::ws;
                std::getline(stream, token);
                cube_map_path[5] = dir + token;
                is_cube = true;
            }

            // Count texture set load cube map true
            if (is_cube) {
                model_data.textures++;
                load_cube_map = true;
            }
        }
    }

    // Close file
    file.close();

    // Return true if not error has been found
    model_data.material_open = true;
    return true;
}


// Constructor

// OBJ loader constructor
OBJLoader::OBJLoader(const std::string &path) :
    ModelLoader(path) {}