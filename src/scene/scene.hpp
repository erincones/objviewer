#ifndef __SCENE_HPP_
#define __SCENE_HPP_

#include "../glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <string>


class Scene {
    private:
        // Attributes

        /** Window */
        GLFWwindow *window;

        /** Window title */
        std::string window_title;

        /** Window width */
        int window_width;

        /** Window height */
        int window_height;


        /** Clear color */
        glm::vec3 clear_color;


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

        /** Glad loaded flag */
        static bool initialized_glad;


        // Static methods

        /** GLFW error callback */
        static void error_callback(int error, const char *description);

        /** GLFW framebuffer size callback */
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);


    public:
        // Constructor

        /** Scene constructor */
        Scene(const std::string &title, const int &width = 800, const int &height = 600, const int &context_ver_maj = 3, const int &context_ver_min = 3);


        // Methods

        /** Render main loop */
        void mainLoop();
        

        // Destructor

        /** Scene destructor */
        ~Scene();
};

#endif // __SCENE_HPP_