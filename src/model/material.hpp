#ifndef __MATERIAL_HPP_
#define __MATERIAL_HPP_

#include "../scene/glslprogram.hpp"

#include "../glad/glad.h"

#include <glm/vec3.hpp>

#include <string>


/** Material with textures */
class Material {
    public:
        // Enumerations

        /** Attributes */
        enum Attribute : GLenum {
            /** Ambient component */
            AMBIENT = 0x0001,

            /** Diffuse component */
            DIFFUSE = 0x0002,

            /** Specular component */
            SPECULAR = 0x0004,

            /** Shininess texture */
            SHININESS = 0x0008,

            /** Roughness value */
            ROUGHNESS = 0x0010,

            /** Metalness value */
            METALNESS = 0x0020,

            /** Transparency component */
            TRANSPARENCY = 0x0040,

            /** Normal texture */
            NORMAL = 0x0080,

            /** Displacement component */
            DISPLACEMENT = 0x0100,

            /** Refractive index value */
            REFRACTIVE_INDEX = 0x0200,

            /** Cube map texture */
            CUBE_MAP = 0x8000,

            /** Cube map right side texture */
            CUBE_MAP_RIGHT = GL_TEXTURE_CUBE_MAP_POSITIVE_X,

            /** Cube map left side texture */
            CUBE_MAP_LEFT = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,

            /** Cube map top side texture */
            CUBE_MAP_TOP = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,

            /** Cube map down side texture */
            CUBE_MAP_BOTTOM = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,

            /** Cube map front side texture */
            CUBE_MAP_FRONT = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,

            /** Cube map back side texture */
            CUBE_MAP_BACK = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,

            /** All textures */
            ALL_TEXTURES = 0x818F
        };

    private:
        // Attributes

        /** Material name */
        std::string name;


        /** Colors */
        glm::vec3 color[4];


        /** Value */
        float value[6];


        /** Texture */
        GLuint texture[7];

        /** Texture enabled status */
        bool texture_enabled[7];

        /** Texture path */
        std::string texture_path[12];


        // Constructors

        /** Disable the default constructor */
        Material() = delete;

        /** Disable the default copy constructor */
        Material(const Material &) = delete;

        /** Disable the assignation operator */
        Material &operator=(const Material &) = delete;


        // Static const attributes

        /** Texture attributes */
        static const Material::Attribute TEXTURE_ATTRIBUTE[];


        // Static attributes

        /** Default texture */
        static GLuint default_texture[3];


        // Static methods

        /** Create a default texture */
        static GLuint createDefaultTexture(const GLubyte *const color);

        /** Bind texture */
        static void bindTexture(const GLenum &index, const GLuint &texture);

        /** Load a 2D texture */
        static GLuint load2DTexture(const std::string &path);

        /** Load a cube map texture */
        static GLuint loadCubeMapTexture(const std::string (&path)[6]);

    public:
        // Constructor

        /** Material constructor */
        Material(const std::string &name);


        // Getters

        /** Get the material name */
        std::string getName() const;

        /** Get the color of the given attribute */
        glm::vec3 getColor(const Material::Attribute &attrib) const;

        /** Get the value of the given attribute */
        float getValue(const Material::Attribute &attrib) const;

        /** Get the texture of the given attribute */
        GLuint getTexture(const Material::Attribute &attrib) const;

        /** Get the texture enabled status */
        bool isTextureEnabled(const Material::Attribute &attrib) const;

        /** Get the texture path of the given attribute */
        std::string getTexturePath(const Material::Attribute &attrib) const;


        // Setters

        /** Set the material name */
        void setName(const std::string &new_name);

        /** Set the color of the given attribute */
        void setColor(const Material::Attribute &attrib, const glm::vec3 &new_color);

        /** Set the value of the given attribute */
        void setValue(const Material::Attribute &attrib, const float &new_value);

        /** Get the texture enabled status */
        void setTextureEnabled(const Material::Attribute &attrib, const bool &status);

        /** Set the texture path of the given attribute */
        void setTexturePath(const Material::Attribute &attrib, const std::string &path);

        /** Set the cube map texture path */
        void setCubeMapTexturePath(const std::string (&path)[6]);


        // Methods

        /** Reload texture */
        void reloadTexture(const Material::Attribute &attrib);

        /** Bind material */
        void bind(GLSLProgram *const program) const;


        // Destructor

        /** Material destructor */
        virtual ~Material();


        // Static methods

        /** Create the default textures */
        static void createDefaultTextures();

        /** Delete the default textures */
        static void deleteDefaultTextures();
};

#endif // __MATERIAL_HPP_