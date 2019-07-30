#ifndef __MODEL_HPP_
#define __MODEL_HPP_

#include "loader/modelloader.hpp"
#include "loader/modeldata.hpp"
#include "material.hpp"

#include "../scene/glslprogram.hpp"

#include "../glad/glad.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <string>

#include <vector>

#include <memory>


/** 3D model */
class Model : private ModelData {
    private:
        // Attributes

        /** GLSL program */
        std::shared_ptr<GLSLProgram> program;


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


        // Getters

        /** Get the open status */
        bool isOpen() const;

        /** Get the open material status */
        bool isMaterialOpen() const;


        /** Get the model file path */
        std::string getPath() const;

        /** Get the material file path */
        std::string getMaterialPath() const;


        /** Get the GLSL program */
        std::shared_ptr<GLSLProgram> getProgram() const;


        /** Get the material stock */
        std::vector<std::shared_ptr<Material> > getMaterialStock() const;

        /** Get a material by name */
        std::shared_ptr<Material> getMaterial(const std::string &material_name) const;


        /** Get the origin matrix */
        glm::mat4 getOriginMatrix() const;

        /** Get the model matrix */
        glm::mat4 getModelMatrix() const;


        /** Get the position */
        glm::vec3 getPosition() const;

        /** Get the rotation quaternion */
        glm::quat getRotation() const;

        /** Get the rotation angles */
        glm::vec3 getRotationAngles() const;

        /** Get the scale */
        glm::vec3 getScale() const;

        /** Get the maximum position values */
        glm::vec3 getMax() const;

        /** Get the minimum position values */
        glm::vec3 getMin() const;


        // Setters

        /** Set the new path */
        void setPath(const std::string &new_path);

        /** Set the GLSL program */
        void setProgram(const std::shared_ptr<GLSLProgram> &new_program);


        /** Set the new position */
        void setPosition(const glm::vec3 &new_position);

        /** Set the new rotation angles in degrees */
        void setRotation(const glm::vec3 &new_rotation);

        /** Set the new rotation quaternion */
        void setRotation(const glm::quat &new_rotation);

        /** Set the new scale */
        void setScale(const glm::vec3 &new_scale);


        // Methods

        /** Reload model */
        void reload();

        /** Reload material */
        void reloadMaterial();

        /** Reset geometry */
        void resetGeometry();


        /** Draw the model */
        void draw();


        /** Translate the model */
        void translate(const glm::vec3 &delta);

        /** Rotate certain degrees */
        void rotate(const glm::vec3 &delta);

        /** Rotate applying the quaternion */
        void rotate(const glm::quat &factor);

        /** Scale the given factor */
        void scale(const glm::vec3 &factor);


        // Destructor

        /** Model destructor */
        ~Model();
};

#endif // __MODEL_HPP_