#ifndef __INTERACTIVE_SCENE_HPP_
#define __INTERACTIVE_SCENE_HPP_

#include "../scene.hpp"

#include "mouse.hpp"


class InteractiveScene : public Scene {
    private:
        // Enumerations
        enum Focus {
            /** Focus to none */
            NONE,

            /** Focus to the scene */
            SCENE,

            /** Focus to the GUI */
            GUI
        };


        // Attributes

        /** Mouse */
        Mouse *const mouse;


        /** Cursor enabled status */
        bool cursor_enabled;

        /** Cursor position */
        glm::vec2 cursor_position;


        /** Focus */
        InteractiveScene::Focus focus;


        /** Draw the main GUI window flag */
        bool show_main_gui;

        /** Show metrics window flag */
        bool show_metrics;

        /** Show about window flag */
        bool show_about;

        /** Show about ImGui window flag */
        bool show_about_imgui;


        /** Focus GUI flag */
        bool focus_gui;


        // Methods

        /** Draw the GUI */
        void drawGUI();


        /** Draw the main window GUI */
        void showMainGUIWindow();


        /** Draw the camera widget */
        bool cameraWidget(Camera *const camera, const std::size_t &id = 0U);

        /** Draw the model widget */
        bool modelWidget(std::pair<Model *, std::size_t> &model_data);

        /** Draw the light widget */
        bool lightWidget(Light *const light);

        /** Draw the program widget */
        bool programWidget(std::pair<GLSLProgram *, std::string> &program_data);

        /** Draw a program combo item */
        bool programComboItem(const std::size_t &current, const std::size_t &program);


        /** Process keyboard input */
        void processKeyboardInput();


        // Static const attributes

        /** Available texures */
        static const std::map<Material::Attribute, std::string> AVAILABLE_TEXTURE;

        /** Types of lights labels */
        static const char *LIGHT_TYPE_LABEL[];


        // Static attributes

        /** Repository URL */
        static char repository_url[];


        // Static methods

        /** GLFW framebuffer size callback */
        static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

        /** GLFW mouse button callback */
        static void mouseButtonCallback(GLFWwindow *window, int, int action, int);

        /** GLFW cursor callback */
        static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

        /** GLFW scroll callback */
        static void scrollCallback(GLFWwindow *window, double, double yoffset);

        /** GLFW key callback */
        static void keyCallback(GLFWwindow *window, int key, int, int action, int modifier);


    public:
        // Constructor

        /** Interactive scene constructor */
        InteractiveScene(const std::string &title, const int &width = 800, const int &height = 600, const int &context_ver_maj = 3, const int &context_ver_min = 3);


        // Getters

        /** Get the main GUI visible status */
        bool isMainGUIVisible() const;

        /** Get the metrics window visible status */
        bool isMetricsVisible() const;

        /** Get the about window visible status */
        bool isAboutVisible() const;

        /** Get the about ImGui window visible status */
        bool isAboutImGuiVisible() const;


        /** Get the cursor enabled status */
        bool isCursorEnabled() const;

        /** Get the mouse */
        Mouse *getMouse() const;


        // Setters

        /** Set the main GUI visible status */
        void setMainGUIVisible(const bool &status);

        /** Set the metrics window visible */
        void setMetricsVisible(const bool &status);

        /** Set the about window visible */
        void setAboutVisible(const bool &status);

        /** Set the about ImGui window visible */
        void setAboutImGuiVisible(const bool &status);


        /** Set the cursor enabled status */
        void setCursorEnabled(const bool &status);


        // Methods

        /** Render main loop */
        void mainLoop();


        // Destructor

        /** Interactive scene destructor */
        ~InteractiveScene();


        // Static methods

        /** Draw the about window GUI */
        static void showAboutWindow(bool &show);
};

#endif // __INTERACTIVE_SCENE_HPP_