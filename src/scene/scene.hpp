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
        std::map<std::size_t, Camera *> camera_stock;

        /** Model stock */
        std::map<std::size_t, std::pair<Model *, std::size_t> > model_stock;

        /** Program stock */
        std::map<std::size_t, GLSLProgram *> program_stock;


        // Constructors

        /** Disable the default constructor */
        Scene() = delete;

        /** Disable the default copy constructor */
        Scene(const Scene &) = delete;

        /** Disable the assignation operator */
        Scene &operator=(const Scene &) = delete;


        // Static attributes

        /** Instances counter */
        static std::size_t instances;

        /** Elements unique ID's */
        static std::size_t element_id;

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
        Camera *getCamera(const std::size_t &id = 0U) const;

        /** Get model by ID */
        Model *getModel(const std::size_t &id) const;

        /** Get program by ID */
        GLSLProgram *getProgram(const std::size_t &id) const;


        // Setters

        /** Select current camara */
        bool selectCamera(const std::size_t &id);

        /** Add camera */
        std::size_t addCamera(const bool &orthogonal = false);


        /** Add empty model */
        std::size_t addModel();

        /** Add model */
        std::size_t addModel(const std::string &path, const std::size_t &program_id = 0U);


        /** Add empty GLSL program */
        std::size_t addProgram();

        /** Add GLSL program without geometry shader */
        std::size_t addProgram(const std::string &vert, const std::string &frag);

        /** Add GLSL program */
        std::size_t addProgram(const std::string &vert, const std::string &geom, const std::string &frag);


        /** Set title */
        void setTitle(const std::string &new_title);

        /** Set program to model */
        std::size_t setProgramToModel(const std::size_t &program_id, const std::size_t &model_id);


        // Methods

        /** Render main loop */
        void mainLoop();


        /** Remove camera */
        bool removeCamera(const std::size_t &id);

        /** Remove camera */
        bool removeModel(const std::size_t &id);

        /** Remove program */
        bool removeProgram(const std::size_t &id);


        // Destructor

        /** Scene destructor */
        ~Scene();


        // Static getters

        /** Get the default program */
        static GLSLProgram *getDefaultProgram();


        // Static setters

        /** Set the default program without geometry shader */
        static void setDefaultProgram(const std::string &vert, const std::string &frag);

        /** Set the default program */
        static void setDefaultProgram(const std::string &vert, const std::string &geom, const std::string &frag);
};

#endif // __SCENE_HPP_