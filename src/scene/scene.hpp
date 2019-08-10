#ifndef __SCENE_HPP_
#define __SCENE_HPP_

#include "camera.hpp"
#include "../model/model.hpp"
#include "light.hpp"
#include "glslprogram.hpp"

#include "../glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <string>

#include <map>
#include <forward_list>


class Scene {
    protected:
        // Attributes

        /** Window */
        GLFWwindow *window;

        /** Window title */
        std::string title;

        /** Frame buffer width */
        int width;

        /** Frame buffer height */
        int height;


        /** Background color */
        glm::vec3 background_color;


        /** Active camera */
        Camera *active_camera;


        /** Camera stock */
        std::map<std::size_t, Camera *> camera_stock;

        /** Model stock */
        std::map<std::size_t, std::pair<Model *, std::size_t> > model_stock;


        /** Light stock */
        std::map<std::size_t, Light *> light_stock;


        /** Lighting pass program ID */
        std::size_t lighting_program;

        /** Program stock */
        std::map<std::size_t, std::pair<GLSLProgram *, std::string> > program_stock;


        /** Thousands of frames */
        double kframes;


        // Constructors

        /** Disable the default constructor */
        Scene() = delete;

        /** Disable the default copy constructor */
        Scene(const Scene &) = delete;

        /** Disable the assignation operator */
        Scene &operator=(const Scene &) = delete;


        // Methods

        /** Draw the scene */
        void drawScene();


        // Static attributes

        /** Instances counter */
        static std::size_t instances;

        /** Elements unique ID's */
        static std::size_t element_id;

        /** Glad loaded flag */
        static bool initialized_glad;


        /** Screen width */
        static GLsizei screen_width;

        /** Screen height */
        static GLsizei screen_height;


        /** Square vertex array object */
        static GLuint square_vao;

        /** Square vertex buffer object */
        static GLuint square_vbo;


        /** Geometry frame buffer object */
        static GLuint fbo;

        /** Render buffer object */
        static GLuint rbo;

        /** Buffer textures */
        static GLuint buffer_texture[];


        /** OpenGL vendor */
        static const GLubyte *opengl_vendor;

        /** OpenGL renderer */
        static const GLubyte *opengl_renderer;

        /** OpenGL version */
        static const GLubyte *opengl_version;

        /** GLSL version */
        static const GLubyte *glsl_version;


        // Static methods

        /** Create the geometry frame buffer */
        static void createGeometryFrameBuffer();

        /** Create and attach texture to the frame buffer object */
        static void attachTextureToFrameBuffer(const GLenum &attachment, const GLint &internalFormat, const GLenum &format, const GLenum &type);


        /** Create a square fitted to the screen */
        static void createSquare();


        /** GLFW error callback */
        static void errorCallback(int error, const char *description);

        /** GLFW framebuffer size callback */
        static void framebufferSizeCallback(GLFWwindow *window, int width, int height);


    public:
        // Constructor

        /** Scene constructor */
        Scene(const std::string &title, const int &width = 800, const int &height = 600, const int &context_ver_maj = 3, const int &context_ver_min = 3);


        // Getters

        /** Get the valid status */
        bool isValid();


        /** Get resolution */
        std::string getTitle() const;

        /** Get resolution */
        glm::vec2 getResolution() const;

        /** Get the background color */
        glm::vec3 getBackgroundColor() const;


        /** Get the active camera */
        Camera *getActiveCamera() const;


        /** Get camera by ID */
        Camera *getCamera(const std::size_t &id = 0U) const;

        /** Get model by ID */
        Model *getModel(const std::size_t &id) const;

        /** Get light by ID */
        Light *getLight(const std::size_t &id) const;

        /** Get the program id of a model */
        std::size_t getModelProgram(const std::size_t &id) const;

        /** Get program by ID */
        GLSLProgram *getProgram(const std::size_t &id) const;

        /** Get program description by ID */
        std::string getProgramDescription(const std::size_t &id) const;


        /** Get the lighting pass program ID */
        std::size_t getLightingPassProgramID() const;


        /** Get the default geometry pass program */
        GLSLProgram *getDefaultGeometryPassProgram();

        /** Get the default geometry pass program description */
        std::string getDefaultGeometryPassProgramDescription();


        /** Get the default lighing pass program */
        GLSLProgram *getDefaultLightingPassProgram();

        /** Get the default lighing pass program description */
        std::string getDefaultLightingPassProgramDescription();


        /** Get frames */
        double getFrames() const;


        // Setters

        /** Set the background color */
        void setBackgroundColor(const glm::vec3 &color);


        /** Select the active camara */
        bool selectCamera(const std::size_t &id);

        /** Add camera */
        std::size_t addCamera(const bool &orthogonal = false);


        /** Add empty model */
        std::size_t addModel();

        /** Add model */
        std::size_t addModel(const std::string &path, const std::size_t &program_id = 0U);


        /** Add light */
        std::size_t addLight(const Light::Type &type = Light::DIRECTIONAL);


        /** Add empty GLSL program */
        std::size_t addProgram(const std::string &desc);

        /** Add GLSL program without geometry shader */
        std::size_t addProgram(const std::string &desc, const std::string &vert, const std::string &frag);

        /** Add GLSL program */
        std::size_t addProgram(const std::string &desc, const std::string &vert, const std::string &geom, const std::string &frag);

        /** Set program description */
        bool setProgramDescription(const std::string &desc, const std::size_t &id);


        /** Get the lighting pass program */
        void setLightingPassProgram(const std::size_t &id);


        /** Set the default geometry pass program */
        void setDefaultGeometryPassProgram(const std::string &desc, const std::string &vert, const std::string &frag);

        /** Se the default geometry pass program description */
        void setDefaultGeometryPassProgramDescription(const std::string &desc);


        /** Set the default lighing pass program */
        void setDefaultLightingPassProgram(const std::string &desc, const std::string &vert, const std::string &frag);

        /** Se the default lighing pass program description */
        void setDefaultLightingPassProgramDescription(const std::string &desc);


        /** Set title */
        void setTitle(const std::string &new_title);

        /** Set program to model */
        std::size_t setProgramToModel(const std::size_t &program_id, const std::size_t &model_id);


        // Methods

        /** Render main loop */
        virtual void mainLoop();


        /** Reload all programs */
        void reloadPrograms();


        /** Remove camera */
        bool removeCamera(const std::size_t &id);

        /** Remove camera */
        bool removeModel(const std::size_t &id);

        /** Remove light */
        bool removeLight(const std::size_t &id);

        /** Remove program */
        bool removeProgram(const std::size_t &id);


        /** Remove the default geometry pass program */
        void removeDefaultGeometryPassProgram();

        /** Remove the default lighting pass program */
        void removeDefaultLightingPassProgram();


        // Destructor

        /** Scene destructor */
        virtual ~Scene();


        // Static getters

        /** Get the OpenGL vendor */
        static const GLubyte *getOpenGLVendor();

        /** Get the OpenGL renderer */
        static const GLubyte *getOpenGLRenderer();

        /** Get the OpenGL version */
        static const GLubyte *getOpenGLVersion();

        /** Get the GLSL version */
        static const GLubyte *getGLSLVersion();
};

#endif // __SCENE_HPP_