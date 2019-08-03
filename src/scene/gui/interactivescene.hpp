#ifndef __INTERACTIVE_SCENE_HPP_
#define __INTERACTIVE_SCENE_HPP_

#include "../scene.hpp"


class InteractiveScene : public Scene {
    private:
        // Attributes

        /** Draw the main GUI window flag */
        bool show_main_gui_win;

        /** Show metrics window flag */
        bool show_metrics_win;
        
        /** Show about window flag */
        bool show_about_win;

        /** Show about ImGui window flag */
		bool show_about_imgui_win;


        /** Focus GUI flag */
        bool focus_gui;


        // Methods

        /** Draw the scene */
        void drawScene();

        /** Draw the GUI */
        void drawGUI();


        /** Draw the main window GUI */
        void showMainGUIWindow();


        /** Draw the camera widget */
        bool cameraWidget(Camera *const camera, const std::size_t &id = 0U);

        /** Draw the model widget */
        bool modelWidget(std::pair<Model *const, std::size_t> &model_data, const std::size_t &id = 0U);

        /** Draw the program widget */
        bool programWidget(GLSLProgram *const program, const std::size_t &id = 0U);


        // Static attributes

        /** Repository URL */
        static char repository_url[];


        // Static methods

        /** GLFW framebuffer size callback */
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);


    public:
        // Constructor

        /** Interactive scene constructor */
        InteractiveScene(const std::string &title, const int &width = 800, const int &height = 600, const int &context_ver_maj = 3, const int &context_ver_min = 3);


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