#ifndef __LIGHT_HPP_
#define __LIGHT_HPP_

#include "glslprogram.hpp"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


class Light {
    public:
        // Enumerations
        enum Type : GLint {
            /** Directional light */
            DIRECTIONAL,

            /** Point light */
            POINT,

            /** Spotlight light */
            SPOTLIGHT
        };

    private:
        // Attributes

        /** Enabled status */
        bool enabled;


        /** Type */
        Light::Type type;


        /** Direction */
        glm::vec3 direction;

        /** Position */
        glm::vec3 position;

        /** Attenuation */
        glm::vec3 attenuation;

        /** Cutoff */
        glm::vec2 cutoff;


        /** Ambient color */
        glm::vec3 ambient_color;

        /** Diffuse color */
        glm::vec3 diffuse_color;

        /** Specular color */
        glm::vec3 specular_color;


        /** Ambient level */
        float ambient_level;

        /** Diffuse level */
        float diffuse_level;

        /** Specular level */
        float specular_level;

        /** Shininess */
        float shininess;


    public:
        // Constructors

        /** Light constructor */
        Light(const Light::Type &type = Light::DIRECTIONAL);


        // Getters

        /** Get the enabled status */
        bool isEnabled() const;

        /** Get the type */
        Light::Type getType() const;


        /** Get the direction */
        glm::vec3 getDirection() const;

        /** Get the position */
        glm::vec3 getPosition() const;

        /** Get attenuation */
        glm::vec3 getAttenuation() const;

        /** Get cutoff */
        glm::vec2 getCutoff() const;


        /** Get the ambient color */
        glm::vec3 getAmbientColor() const;

        /** Get the diffuse color */
        glm::vec3 getDiffuseColor() const;

        /** Get the specular color */
        glm::vec3 getSpecularColor() const;


        /** Get the ambient level */
        float getAmbientLevel() const;

        /** Get the ambient level */
        float getDiffuseLevel() const;

        /** Get the ambient level */
        float getSpecularLevel() const;

        /** Get the shininess */
        float getShininess() const;


        // Setters

        /** Set the enabled status */
        void setEnabled(const bool &status);

        /** Set the type */
        void setType(const Light::Type &new_type);


        /** Set the direction */
        void setDirection(const glm::vec3 &new_direction);

        /** Set the position */
        void setPosition(const glm::vec3 &new_position);

        /** Set attenuation */
        void setAttenuation(const glm::vec3 &new_attenuation);

        /** Set cutoff */
        void setCutoff(const glm::vec2 &new_cutoff);


        /** Set the ambient color */
        void setAmbientColor(const glm::vec3 &color);

        /** Set the diffuse color */
        void setDiffuseColor(const glm::vec3 &color);

        /** Set the specular color */
        void setSpecularColor(const glm::vec3 &color);


        /** Set the ambient level */
        void setAmbientLevel(const float &value);

        /** Set the ambient level */
        void setDiffuseLevel(const float &value);

        /** Set the ambient level */
        void setSpecularLevel(const float &value);

        /** Set the shininess */
        void setShininess(const float &value);


        // Methods

        /** Bind the light */
        void bind(GLSLProgram *const program) const;
};

#endif // __LIGHT_HPP_