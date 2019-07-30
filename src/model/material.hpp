#ifndef __MATERIAL_HPP_
#define __MATERIAL_HPP_

#include "../scene/glslprogram.hpp"

#include "../glad/glad.h"

#include <glm/vec3.hpp>

#include <string>

#include <memory>


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


        /** Ambient color */
		glm::vec3 ambient_color;

        /** Diffuse color */
		glm::vec3 diffuse_color;

        /** Specular color */
		glm::vec3 specular_color;

        /** Transparency color */
        glm::vec3 transparency_color;


        /** Shininess value */
		float shininess;

        /** Roughness value */
		float roughness;

        /** Metalness value */
		float metalness;

        /** Transparency value */
        float transparency;

        /** Displacement value */
        float displacement;

        /** Refractive index value */
		float refractive_index;


        /** Ambient texture */
		GLuint ambient_tex;

        /** Diffuse texture */
		GLuint diffuse_tex;

        /** Specular texture */
		GLuint specular_tex;

        /** Shininess texture */
		GLuint shininess_tex;

        /** Normal texture */
		GLuint normal_tex;

        /** Ambient texture */
		GLuint displacement_tex;

        /** Cube map texture */
		GLuint cube_map_tex;


        /** Ambient texture path */
		std::string ambient_tex_path;

        /** Diffuse texture path */
		std::string diffuse_tex_path;

        /** Specular texture path */
		std::string specular_tex_path;

        /** Shininess texture path */
		std::string shininess_tex_path;

        /** Normal texture path */
		std::string normal_tex_path;

        /** Displacement texture */
		std::string displacement_tex_path;

        /** Cube map texture paths */
		std::string cube_map_tex_path[6];


        // Constructors

        /** Disable the default constructor */
        Material() = delete;

        /** Disable the default copy constructor */
        Material(const Material &) = delete;

        /** Disable the assignation operator */
        Material &operator=(const Material &) = delete;



        // Static attributes

        /** Default white texture */
		static GLuint white_tex;

        /** Default black texture */
        static GLuint black_tex;

        /** Default blue texture */
        static GLuint blue_tex;


        // Static methods

        /** Create a default texture */
        static GLuint createDefaultTexture(const glm::vec3 &color);

        /** Bind texture */
        static void bindTexture(const GLenum &index, const GLuint &texture, const GLuint &default_texture);

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

        /** Get the texture path of the given attribute */
        std::string getTexturePath(const Material::Attribute &attrib) const;


        // Setters

        /** Set the material name */
        void setName(const std::string &new_name);

        /** Set the color of the given attribute */
        void setColor(const glm::vec3 &color, const Material::Attribute &attrib);

        /** Set the value of the given attribute */
        void setValue(const float &value, const Material::Attribute &attrib);

        /** Set the texture path of the given attribute */
        void setTexturePath(const std::string &path, const Material::Attribute &attrib);

        /** Set the cube map texture path */
        void setCubeMapTexturePath(const std::string (&path)[6]);


        // Methods

        /** Reload texture */
        void reloadTexture(const Material::Attribute &attrib);

        /** Bind material */
        void bind(const std::shared_ptr<GLSLProgram> &program) const;


        // Destructor

        /** Material destructor */
        ~Material();


        // Static methods

        /** Create the default textures */
        static void createDefaultTextures();

        /** Delete the default textures */
        static void deleteDefaultTextures();
};

#endif // __MATERIAL_HPP_