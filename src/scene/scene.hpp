#ifndef __SCENE_HPP_
#define __SCENE_HPP_

#include "camera.hpp"
#include "../model/model.hpp"
#include "glslprogram.hpp"

#include "../glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <string>

#include <map>
#include <forward_list>


class Scene {
    private:
        // Attributes

        /** Window */
        GLFWwindow *window;

        /** Window title */
        std::string title;

        /** Frame buffer width */
        int width;

        /** Frame buffer height */
        int height;


        /** Clear color */
        glm::vec3 clear_color;


        /** Current camera */
        Camera *current_camera;


        /** Camera stock */
        std::map<unsigned int, Camera *> camera_stock;

        /** Model stock */
        std::map<unsigned int, Model *> model_stock;

        /** Program stock */
        std::map<unsigned int, GLSLProgram *> program_stock;


        // Constructors

        /** Disable the default constructor */
        Scene() = delete;

        /** Disable the default copy constructor */
        Scene(const Scene &) = delete;

        /** Disable the assignation operator */
        Scene &operator=(const Scene &) = delete;


        // Static attributes

        /** Instances counter */
        static unsigned int instances;

        /** Elements unique ID's */
        static unsigned int element_id;

        /** Glad loaded flag */
        static bool initialized_glad;


        /** Default program */
        static GLSLProgram *default_program;


        // Static methods

        /** GLFW error callback */
        static void error_callback(int error, const char *description);

        /** GLFW framebuffer size callback */
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);


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


        /** Get the current camera */
        Camera *getCurrentCamera() const;


        /** Get camera by ID */
        Camera *getCamera(const unsigned int &id = 0U) const;

        /** Get model by ID */
        Model *getModel(const unsigned int &id) const;

        /** Get program by ID */
        GLSLProgram *getProgram(const unsigned int &id) const;


        // Setters

        /** Set title */
        void setTitle(const std::string &new_title);


        /** Add camera */
        unsigned int addCamera(const bool &orthogonal = false);

        /** Select current camara */
        bool selectCamera(const unsigned int &id);


        /** Add empty model */
        unsigned int addModel();

        /** Add model */
        unsigned int addModel(const std::string &path, const unsigned int &program_id = 0U);


        /** Add empty GLSL program */
        unsigned int addProgram();

        /** Add GLSL program without geometry shader */
        unsigned int addProgram(const std::string &vert, const std::string &frag);

        /** Add GLSL program */
        unsigned int addProgram(const std::string &vert, const std::string &geom, const std::string &frag);


        // Methods

        /** Render main loop */
        void mainLoop();


        /** Remove camera */
        bool removeCamera(const unsigned int &id);

        /** Remove camera */
        bool removeModel(const unsigned int &id);

        /** Remove program */
        bool removeProgram(const unsigned int &id);


        // Destructor

        /** Scene destructor */
        ~Scene();


        // Static getters

        /** Get the default program */
        static GLSLProgram *getDefaultProgram();


        // Static setters

        /** Set the default program without geometry shader */
        static GLSLProgram *setDefaultProgram(const std::string &vert, const std::string &frag);

        /** Set the default program */
        static GLSLProgram *setDefaultProgram(const std::string &vert, const std::string &geom, const std::string &frag);
};

#endif // __SCENE_HPP_