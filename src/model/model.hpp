#ifndef __MODEL_HPP_
#define __MODEL_HPP_

#include "loader/modelloader.hpp"
#include "loader/modeldata.hpp"
#include "material.hpp"

#include "../glad/glad.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>


/** 3D model */
class Model : private ModelData {
    private:
        // Attributes

        /** Position */
        glm::vec3 position;

        /** Rotation */
        glm::quat rotation;

        /** Dimension */
        glm::quat dimension;


        // Constructors

        /** Disable the default copy constructor */
        Model(const Model &) = delete;

        /** Disable the assignation operator */
        Model &operator=(const Model &) = delete;


    public:
        // Constructor

        /** Empty model constructor */
        Model();

        /** Model constructor */
        Model(const std::string &path);

};

#endif // __MODEL_HPP_