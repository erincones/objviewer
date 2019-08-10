#include "interactivescene.hpp"

#include "customwidgets.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>

#include <cstring>


// Private static const attributes

// Available texures
const std::map<Material::Attribute, std::string> InteractiveScene::AVAILABLE_TEXTURE = {
    {Material::AMBIENT,      "Ambient"},
    {Material::DIFFUSE,      "Diffuse"},
    {Material::SPECULAR,     "Specular"},
    {Material::SHININESS,    "Shininess"},
    {Material::NORMAL,       "Normal"},
    {Material::DISPLACEMENT, "Displacement"}
};

//Types of lights labels
const char *InteractiveScene::LIGHT_TYPE_LABEL[] = {"Directional", "Point", "Spotlight"};


// Private static attributes

// Repository URL
char InteractiveScene::repository_url[] = "https://github.com/Rebaya17/objviewer-gcc/";


// Private statics methods

// GLFW framebuffer size callback
void InteractiveScene::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // Execute the scene framebuffer size callback
    Scene::framebufferSizeCallback(window, width, height);

    // Resize the mouse resolution
    static_cast<InteractiveScene *>(glfwGetWindowUserPointer(window))->mouse->setResolution(width, height);
}

// GLFW mouse button callback
void InteractiveScene::mouseButtonCallback(GLFWwindow *window, int, int action, int) {
    // Get the ImGuiIO reference and the capture IO status
    ImGuiIO &io = ImGui::GetIO();
    const bool capture_io = io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput;

    // Disable the mouse if release a mouse button and the GUI don't want to capture IO
    if (!capture_io && (action == GLFW_RELEASE)) {
        static_cast<InteractiveScene *>(glfwGetWindowUserPointer(window))->setCursorEnabled(false);
    }
}

// GLFW cursor callback
void InteractiveScene::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    // Get the interactive scene
    InteractiveScene *const scene = static_cast<InteractiveScene *>(glfwGetWindowUserPointer(window));
    scene->cursor_position.x = static_cast<float>(xpos);
    scene->cursor_position.y = static_cast<float>(ypos);

    // Get the ImGuiIO reference and the capture IO status
    ImGuiIO &io = ImGui::GetIO();
    const bool capture_io = io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput;

    // Rotate the active camera if the cursor is disabled and the GUI don't want to capture IO or the main GUI window is not visible
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouse) && (!capture_io || !scene->show_main_gui)) {
        scene->active_camera->rotate(scene->mouse->translate(xpos, ypos));
    }
}

// GLFW scroll callback
void InteractiveScene::scrollCallback(GLFWwindow *window, double, double yoffset) {
    // Get the ImGuiIO reference and the capture IO status
    ImGuiIO &io = ImGui::GetIO();
    const bool capture_io = io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput;

    // Update the active camera zoom if the GUI don't want to capture IO
    if (!capture_io) {
        static_cast<InteractiveScene *>(glfwGetWindowUserPointer(window))->active_camera->zoom(yoffset);
    }
}

// GLFW key callback
void InteractiveScene::keyCallback(GLFWwindow *window, int key, int, int action, int modifier) {
    // Get the pressed status
    bool pressed = action != GLFW_RELEASE;

    // Get the interactive scene
    InteractiveScene *const scene = static_cast<InteractiveScene *>(glfwGetWindowUserPointer(window));

    // Get the ImGuiIO reference and the capture IO status
    ImGuiIO &io = ImGui::GetIO();
    const bool capture_io = io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput;

    switch (key) {
        // Toggle the main GUI window visibility
        case GLFW_KEY_ESCAPE:
            if (pressed) {
                scene->show_main_gui = !capture_io || !scene->show_main_gui;
                scene->setCursorEnabled(scene->show_main_gui);
            }
            return;

        // Toggle the about window
        case GLFW_KEY_F1:
            if (pressed) {
                scene->setAboutVisible(!scene->show_about);
            }
            return;

        // Toggle the about ImGui window
        case GLFW_KEY_F11:
            if (pressed) {
                scene->setAboutImGuiVisible(!scene->show_about_imgui);
            }
            return;

        // Toggle the metrics window
        case GLFW_KEY_F12:
            if (pressed) {
                scene->setMetricsVisible(!scene->show_metrics);
            }
            return;


        // Toggle the camera boost
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            Camera::setBoosted(pressed);
            return;

        // Reload shaders
        case GLFW_KEY_R:
            if (pressed && (modifier == GLFW_MOD_CONTROL)) {
                scene->reloadPrograms();
            }
    }
}


// Private methods

// Draw the GUI
void InteractiveScene::drawGUI() {
    // Check the visibility of all windows
    if (!show_main_gui && !show_metrics && !show_about && !show_about_imgui) {
        return;
    }

    // New ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Show the main GUI window
    if (show_main_gui) {
        showMainGUIWindow();
    }

    // Show the about window
    if (show_about) {
        InteractiveScene::showAboutWindow(show_about);
    }

    // Show the metrics built-in window
    if (show_metrics) {
        ImGui::ShowMetricsWindow(&show_metrics);
    }

    // Show the about ImGui built-in window
    if (show_about_imgui) {
        ImGui::ShowAboutWindow(&show_about_imgui);
    }

    // Update focus
    switch (focus) {
        // Set focus to the most front window
        case InteractiveScene::GUI:
            ImGui::SetWindowFocus();
            focus = InteractiveScene::NONE;
            break;

        // Set focus to the scene
        case InteractiveScene::SCENE:
            ImGui::SetWindowFocus(nullptr);
            focus = InteractiveScene::NONE;
            break;

        // Nothing for none
        case InteractiveScene::NONE: break;
    }

    // Render the GUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// Draw the main window GUI
void InteractiveScene::showMainGUIWindow() {
    // Setup style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(501.0F, static_cast<float>(height)), ImGuiCond_Always);

    // Create the main GUI window
    const bool open = ImGui::Begin("Settings", &show_main_gui, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PopStyleVar();

    // Abort if the window is not showing
    if (!show_main_gui || !open) {
        ImGui::End();
        return;
    }

    // About section
    if (ImGui::CollapsingHeader("User Guide", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Navigation mode
        ImGui::BulletText("ESCAPE to toggle the navigation mode.");
        ImGui::BulletText("Click in the scene to enter in the navigation mode.");
        ImGui::BulletText("F1 to toggle the about window.");
        ImGui::BulletText("F12 to toggle the Dear ImGui metrics window.");
        ImGui::BulletText("Double-click on title bar to collapse window.");

        // Windows manipulation
        if (ImGui::TreeNodeEx("Others than the settings window", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("Click and drag on lower right corner to resize window\n(double-click to auto fit window to its contents).");
            ImGui::BulletText("Click and drag on any empty space to move window.");
            ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            ImGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
            ImGui::TreePop();
            ImGui::Spacing();
        }

        // Keyboard input
        if (ImGui::TreeNodeEx("While editing text", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("Hold SHIFT or use mouse to select text.");
            ImGui::BulletText("CTRL+Left/Right to word jump.");
            ImGui::BulletText("CTRL+A or double-click to select all.");
            ImGui::BulletText("CTRL+X,CTRL+C,CTRL+V to use clipboard.");
            ImGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
            ImGui::BulletText("ESCAPE to revert.");
            ImGui::BulletText("You can apply arithmetic operators +,*,/ on numerical\nvalues. Use +- to subtract.");
            ImGui::TreePop();
            ImGui::Spacing();
        }

        // About and metrics buttons
        show_about |= ImGui::Button("About OBJViewer");

        ImGui::SameLine();
        show_about_imgui |= ImGui::Button("About Dear ImGui");

        ImGui::SameLine();
        show_metrics |= ImGui::Button("Metrics");
    }

    // Scene section
    if (ImGui::CollapsingHeader("Scene")) {
        // OpenGL info
        if (ImGui::TreeNodeEx("OpenGL", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Vendor: %s",       Scene::opengl_vendor);
            ImGui::Text("Renderer: %s",     Scene::opengl_renderer);
            ImGui::Text("Version: %s",      Scene::opengl_version);
            ImGui::Text("GLSL version: %s", Scene::glsl_version);
            ImGui::TreePop();
        }

        // Window information
        if (ImGui::TreeNodeEx("Window", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Width:  %d",  width);
            ImGui::SameLine(210.0F);
            ImGui::Text("Seconds: %.3fs", glfwGetTime());
            ImGui::Text("Height: %d", height);
            ImGui::SameLine(210.0F);
            ImGui::Text("Frames:  %.3fE3", kframes);
            ImGui::Text("Mouse: %.0f, %.0f", cursor_position.x, cursor_position.y);
            ImGui::HelpMarker("[x, y]");
            ImGui::Spacing();
            // Background color
            if (ImGui::ColorEdit3("Background", &clear_color.r)) {
                glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0F);
            }
            ImGui::TreePop();
        }

        // Scene statistics
        if (ImGui::TreeNode("Statistics*")) {
            // Models statistics variables
            std::size_t vertices  = 0U;
            std::size_t elements  = 0U;
            std::size_t triangles = 0U;
            std::size_t materials = 0U;
            std::size_t textures  = 0U;

            // Calculate models statistics
            for (const std::pair<const std::size_t, const std::pair<const Model *const, const std::size_t> > &program_data : model_stock) {
                vertices  += program_data.second.first->getNumberOfVertices();
                elements  += program_data.second.first->getNumberOfElements();
                triangles += program_data.second.first->getNumberOfTriangles();
                materials += program_data.second.first->getNumberOfMaterials();
                textures  += program_data.second.first->getNumberOfTextures();
            }

            // // Calculate program statistics
            std::size_t shaders = 0U;
            for (const std::pair<const std::size_t, std::pair<const GLSLProgram *const, const std::string> > &program_data : program_stock) {
                shaders += program_data.second.first->getNumberOfShaders();
            }

            // Cameras
            ImGui::BulletText("Cameras: %lu", camera_stock.size());

            // Models
            if (ImGui::TreeNodeEx("modelstats", ImGuiTreeNodeFlags_DefaultOpen, "Models: %lu", model_stock.size())) {
                ImGui::Text("Elements:  %lu", elements); ImGui::HelpMarker("Total of vertices");
                ImGui::SameLine(210.0F);
                ImGui::Text("Materials: %lu", materials);
                ImGui::Text("Vertices:  %lu", vertices); ImGui::HelpMarker("Unique vertices");
                ImGui::SameLine(210.0F);
                ImGui::Text("Textures:  %lu", textures);
                ImGui::Text("Triangles: %lu", triangles);
                ImGui::TreePop();
            }

            // Programs
            if (ImGui::TreeNodeEx("programsstats", ImGuiTreeNodeFlags_DefaultOpen, "GLSL programs: %lu", program_stock.size())) {
                ImGui::Text("Shaders: %lu", shaders);
                ImGui::TreePop();
            }

            // Note
            ImGui::Spacing();
            ImGui::TextDisabled("*Including the elements with errors.");

            // Pop statistics node
            ImGui::TreePop();
        }
    }

    // Cameras section
    if (ImGui::CollapsingHeader("Cameras")) {
        // Global settings
        if (ImGui::TreeNodeEx("Global", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Sensibility
            float value = Camera::getSensibility();
            if (ImGui::DragFloat("Sensibility", &value, 0.25F, 0.0F, 0.0F, "%.4f")) {
                Camera::setSensibility(value);
            }
            // Speed
            value = Camera::getSpeed();
            if (ImGui::DragFloat("Speed", &value, 0.005F, 0.0F, FLT_MAX, "%.4f")) {
                Camera::setSpeed(value);
            }
            // Boost speed
            value = Camera::getBoostedSpeed();
            if (ImGui::DragFloat("Boost speed", &value, 0.05F, 0.0F, FLT_MAX, "%.4f")) {
                Camera::setBoostedSpeed(value);
            }
            ImGui::HelpMarker("The boost speed is expected to be\ngreater than the normal speed.");
            ImGui::TreePop();
        }

        // Active camera
        if (ImGui::TreeNodeEx("Active", ImGuiTreeNodeFlags_DefaultOpen)) {
            cameraWidget(active_camera);
            ImGui::TreePop();
        }

        // Camera to remove ID
        std::size_t remove = 0U;

        // Draw each camera node
        for (const std::pair<const std::size_t, Camera *const> &camera_data : camera_stock) {
            // ID and title strings
            const std::string id = std::to_string(camera_data.first);
            const std::string camera_title = "Camera " + id;

            // Draw node and catch the selected to remove
            if (ImGui::TreeNode(id.c_str(), camera_title.c_str())) {
                if (!cameraWidget(camera_data.second, camera_data.first)) {
                    remove = camera_data.first;
                }
                ImGui::TreePop();
            }
        }

        // Remove camera
        if (remove != 0U) {
            removeCamera(remove);
        }

        // Add camera button
        ImGui::Spacing();
        if (ImGui::Button("Add camera", ImVec2(454.0F, 19.0F))) {
            addCamera();
        }
        ImGui::Spacing();
    }

    // Models section
    if (ImGui::CollapsingHeader("Models")) {
        // Model to remove ID
        std::size_t remove = 0U;

        // Draw each model node
        for (std::pair<const std::size_t, std::pair<Model *, std::size_t> > &program_data : model_stock) {
            // ID and title strings
            const std::string id = std::to_string(program_data.first);
            const std::string program_title = "Model " + id + ": " + program_data.second.first->getName();

            // Draw node and catch the selected to remove
            if (ImGui::TreeNode(id.c_str(), program_title.c_str())) {
                if (!modelWidget(program_data.second)) {
                    remove = program_data.first;
                }
                ImGui::TreePop();
            }
        }

        // Remove model
        if (remove != 0U) {
            removeModel(remove);
        }

        // Add model button
        ImGui::Spacing();
        if (ImGui::Button("Add model", ImVec2(454.0F, 19.0F))) {
            addModel();
        }
        ImGui::Spacing();
    }

    // Lights section
    if (ImGui::CollapsingHeader("Lights")) {
        // Program to remove ID
        std::size_t remove = 0U;

        // Draw each light node
        for (const std::pair<const std::size_t, Light *const> &light_data : light_stock) {
            // ID and title strings
            const std::string id = std::to_string(light_data.first);
            const std::string light_title = "Light " + id;

            // Draw node and catch the selected to remove
            if (ImGui::TreeNode(id.c_str(), light_title.c_str())) {
                if (!lightWidget(light_data.second)) {
                    remove = light_data.first;
                }
                ImGui::TreePop();
            }
        }

        // Remove light
        if (remove != 0U) {
            removeLight(remove);
        }

        // Add light button
        ImGui::Spacing();
        if (ImGui::Button("Add light", ImVec2(454.0F, 19.0F))) {
            addLight();
        }
        ImGui::Spacing();
    }

    // Programs section
    if (ImGui::CollapsingHeader("GLSL Programs")) {
        // Program to remove ID
        std::size_t remove = 0U;

        // Geometry pass program
        ImGui::BulletText("Lighting pass program");
        ImGui::Indent();
        // Program title with ID
        std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::const_iterator result = program_stock.find(lighting_program);
        std::string program_title = (result == program_stock.end() ? "NULL" : result->second.second);
        if ((lighting_program != 0U) && (lighting_program != 1U)) {
            program_title.append(" (").append(std::to_string(lighting_program)).append(")");
        }
        // Show the programs combo
        ImGui::PushItemWidth(-1.0F);
        if (ImGui::BeginCombo("###lighting_pass_program", program_title.c_str())) {
            // For each program in the stock show the item and make the selection
            size_t new_program = lighting_program;
            for (const std::pair<const std::size_t, std::pair<const GLSLProgram *const, const std::string> > &program_data : program_stock) {
                if (programComboItem(lighting_program, program_data.first)) {
                    new_program = program_data.first;
                }
            }
            lighting_program = new_program;
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::Unindent();

        // Draw each program node
        for (std::pair<const std::size_t, std::pair<GLSLProgram *, std::string> > &program_data : program_stock) {
            // ID string
            const std::string id = std::to_string(program_data.first);

            // Title string
            switch (program_data.first) {
                // Default geometry pass program
                case 0U:
                    program_title = "Default geometry pass";
                    break;

                // Default lighting pass program
                case 1U:
                    program_title = "Default lighting pass";
                    break;

                // Other programs
                default:
                    program_title = "Program " + id + ": " + program_data.second.second;
            }

            // Draw node and catch the selected to remove
            if (ImGui::TreeNode(id.c_str(), program_title.c_str())) {
                if (!programWidget(program_data.second)) {
                    remove = program_data.first;
                }
                ImGui::TreePop();
            }
        }

        // Remove program
        if (remove != 0U) {
            removeProgram(remove);
        }

        // Add program button
        ImGui::Spacing();
        if (ImGui::Button("Add GLSL program", ImVec2(454.0F, 19.0F))) {
            addProgram("Empty");
        }
        ImGui::Spacing();
    }

    // End main window
    ImGui::End();
}


// Draw the camera widget
bool InteractiveScene::cameraWidget(Camera *const camera, const std::size_t &id) {
    // Keep camera flag
    bool keep = true;

    // For non active camera
    if (id != 0U) {
        // Select active button
        bool active = camera == active_camera;
        if (ImGui::Checkbox("Active", &active)) {
            active_camera = camera;
        }
        // Remove button
        if (camera_stock.size() > 1U) {
            keep = !ImGui::RemoveButton();
        }
    }

    // Projection
    bool orthogonal = camera->isOrthogonal();
    if (ImGui::RadioButton("Perspective", !orthogonal)) {
        camera->setOrthogonal(false);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Orthogonal", orthogonal)) {
        camera->setOrthogonal(true);
    }
    ImGui::SameLine(338.0F);
    ImGui::Text("Projection");

    // Position
    glm::vec3 value = camera->getPosition();
    if (ImGui::DragFloat3("Position", &value.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
        camera->setPosition(value);
    }

    // Direction
    value = camera->getDirection();
    if (ImGui::DragFloat3("Direction", &value.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
        camera->setDirection(value);
    }

    // Clipping planes
    glm::vec2 clipping = camera->getClipping();
    if (ImGui::DragFloat2("Clipping", &clipping.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
        camera->setClipping(clipping);
    }
    ImGui::HelpMarker("(Near, Far)");

    // Field of view
    float fov = camera->getFOV();
    if (ImGui::DragFloat("FOV", &fov, 0.01F, 0.0F, 0.0F, "%.4f")) {
        camera->setFOV(fov);
    }

    // Separator for open nodes
    ImGui::Separator();

    // Return the keep camera value
    return keep;
}

// Draw the model widget
bool InteractiveScene::modelWidget(std::pair<Model *, std::size_t> &model_data) {
    // Keep model flag
    bool keep = true;

    // Get the model and program ID
    Model *const model = model_data.first;
    const std::size_t program = model_data.second;

    // Model path
    std::string str = model->getPath();
    if (ImGui::InputText("Path", &str, ImGuiInputTextFlags_EnterReturnsTrue)) {
        model->setPath(str);
    }
    // Enabled status
    bool enabled = model->isEnabled() && model->isOpen();
    if (ImGui::Checkbox("Enabled", &enabled)) {
        model->setEnabled(enabled && model->isOpen());
    }
    // Reload button
    ImGui::SameLine();
    if (ImGui::Button("Reload model")) {
        model->reload();
    }
    // Remove button
    keep = !ImGui::RemoveButton();

    // Check open status
    if (!model->isOpen()) {
        if (!model->Model::getPath().empty()) {
            ImGui::TextColored(ImVec4(0.80F, 0.16F, 0.16F, 1.00F), "Could not open the model");
        }
        return keep;
    }

    // Sumary
    if (ImGui::TreeNodeEx("Sumary", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Elements:  %lu", model->getNumberOfElements()); ImGui::HelpMarker("Total of vertices");
        ImGui::SameLine(210.0F);
        ImGui::Text("Materials: %lu", model->getNumberOfMaterials());
        ImGui::Text("Vertices:  %lu", model->getNumberOfVertices()); ImGui::HelpMarker("Unique vertices");
        ImGui::SameLine(210.0F);
        ImGui::Text("Textures:  %lu", model->getNumberOfTextures());
        ImGui::Text("Triangles: %lu", model->getNumberOfTriangles());
        ImGui::TreePop();
    }

    // Program title with the ID for non default programs
    std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::const_iterator result = program_stock.find(program);
    std::string program_title = (result == program_stock.end() ? "NULL" : result->second.second);
    if ((program != 0U) && (program != 1U)) {
        program_title.append(" (").append(std::to_string(program)).append(")");
    }

    // Program combo
    if (ImGui::BeginCombo("GLSL program", program_title.c_str())) {
        // For each program in the stock show the item and make the selection
        size_t new_program = model_data.second;
        for (const std::pair<const std::size_t, std::pair<const GLSLProgram *const, const std::string> > &program_data : program_stock) {
            if (programComboItem(model_data.second, program_data.first)) {
                new_program = program_data.first;
            }
        }
        model_data.second = new_program;
        ImGui::EndCombo();
    }

    // Geometry
    if (ImGui::TreeNodeEx("Geometry", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Position
        glm::vec3 value = model->Model::getPosition();
        if (ImGui::DragFloat3("Position", &value.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
            model->Model::setPosition(value);
        }

        // Rotation
        value = model->Model::getRotationAngles();
        if (ImGui::DragFloat3("Rotation", &value.x, 0.50F, 0.0F, 0.0F, "%.4f")) {
            model->Model::setRotation(value);
        }
        ImGui::HelpMarker("Angles in degrees");

        // Scale
        value = model->Model::getScale();
        if (ImGui::DragFloat3("Scale", &value.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
            model->setScale(value);
        }

        // Pop geometry node
        ImGui::TreePop();
    }

    // Materials
    if (ImGui::TreeNode("Materials")) {
        // Material path
        str = model->getMaterialPath();
        ImGui::InputText("Path", &str, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
        ImGui::HelpMarker("Read only");

        // Material material
        if (ImGui::Button("Reload material")) {
            model->reloadMaterial();
        }

        // Check material status
        if (!model->isMaterialOpen()) {
            ImGui::TextColored(ImVec4(0.80F, 0.16F, 0.16F, 1.00F), "Could not open the material file");
        }

        // Material variables
        float value;
        glm::vec3 color;
        Material *material;
        Material::Attribute material_attribute;
        const std::size_t materials = model->getNumberOfMaterials();

        // Global material
        if (ImGui::TreeNodeEx("Global", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::TextDisabled("Chages will be applied to all materials");

            // Get the material and number of materials
            material = model->getDefaultMaterial();

            // Ambient color
            material_attribute = Material::AMBIENT;
            color = material->getColor(material_attribute);
            if (ImGui::ColorEdit3("Ambient", &color.x)) {
                material->setColor(material_attribute, color);
                for (std::size_t i = 0U; i < materials; i++) {
                    model->getMaterial(i)->setColor(material_attribute, color);
                }
            }

            // Diffuse color
            material_attribute = Material::DIFFUSE;
            color = material->getColor(material_attribute);
            if (ImGui::ColorEdit3("Diffuse", &color.x)) {
                material->setColor(material_attribute, color);
                for (std::size_t i = 0U; i < materials; i++) {
                    model->getMaterial(i)->setColor(material_attribute, color);
                }
            }

            // Specular color
            material_attribute = Material::SPECULAR;
            color = material->getColor(material_attribute);
            if (ImGui::ColorEdit3("Specular", &color.x)) {
                material->setColor(material_attribute, color);
                for (std::size_t i = 0U; i < materials; i++) {
                    model->getMaterial(i)->setColor(material_attribute, color);
                }
            }

            // Shininess value
            material_attribute = Material::SHININESS;
            value = material->getValue(material_attribute);
            if (ImGui::DragFloat("Shininess", &value, 0.01F, 0.0F, FLT_MAX, "%.4f")) {
                material->setValue(material_attribute, value);
                for (std::size_t i = 0U; i < materials; i++) {
                    model->getMaterial(i)->setValue(material_attribute, value);
                }
            }

            // Roughness value
            material_attribute = Material::ROUGHNESS;
            value = material->getValue(material_attribute);
            if (ImGui::DragFloat("Roughness", &value, 0.01F, 0.0F, FLT_MAX, "%.4f")) {
                material->setValue(material_attribute, value);
                for (std::size_t i = 0U; i < materials; i++) {
                    model->getMaterial(i)->setValue(material_attribute, value);
                }
            }

            // Metalness value
            material_attribute = Material::METALNESS;
            value = material->getValue(material_attribute);
            if (ImGui::DragFloat("Metalness", &value, 0.01F, 0.0F, FLT_MAX, "%.4f")) {
                material->setValue(material_attribute, value);
                for (std::size_t i = 0U; i < materials; i++) {
                    model->getMaterial(i)->setValue(material_attribute, value);
                }
            }

            // Parallax value
            material_attribute = Material::DISPLACEMENT;
            value = material->getValue(material_attribute);
            if (ImGui::DragFloat("Parallax", &value, 0.01F, 0.0F, FLT_MAX, "%.4f")) {
                material->setValue(material_attribute, value);
                for (std::size_t i = 0U; i < materials; i++) {
                    model->getMaterial(i)->setValue(material_attribute, value);
                }
            }

            // Textures enabled status
            if (ImGui::TreeNodeEx("Textures", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::TextDisabled("Chages will be applied to all textures");

                // Ambient textures enabled status
                material_attribute = Material::AMBIENT;
                enabled = material->isTextureEnabled(material_attribute);
                if (ImGui::Checkbox("Ambient", &enabled)) {
                    material->setTextureEnabled(material_attribute, enabled);
                    for (std::size_t i = 0U; i < materials; i++) {
                        model->getMaterial(i)->setTextureEnabled(material_attribute, enabled);
                    }
                }

                // Shininess textures enabled status
                material_attribute = Material::SHININESS;
                enabled = material->isTextureEnabled(material_attribute);
                ImGui::SameLine(210.0F);
                if (ImGui::Checkbox("Shininess", &enabled)) {
                    material->setTextureEnabled(material_attribute, enabled);
                    for (std::size_t i = 0U; i < materials; i++) {
                        model->getMaterial(i)->setTextureEnabled(material_attribute, enabled);
                    }
                }

                // Diffuse textures enabled status
                material_attribute = Material::DIFFUSE;
                enabled = material->isTextureEnabled(material_attribute);
                if (ImGui::Checkbox("Diffuse", &enabled)) {
                    material->setTextureEnabled(material_attribute, enabled);
                    for (std::size_t i = 0U; i < materials; i++) {
                        model->getMaterial(i)->setTextureEnabled(material_attribute, enabled);
                    }
                }

                // Normal textures enabled status
                material_attribute = Material::NORMAL;
                enabled = material->isTextureEnabled(material_attribute);
                ImGui::SameLine(210.0F);
                if (ImGui::Checkbox("Normal", &enabled)) {
                    material->setTextureEnabled(material_attribute, enabled);
                    for (std::size_t i = 0U; i < materials; i++) {
                        model->getMaterial(i)->setTextureEnabled(material_attribute, enabled);
                    }
                }

                // Specular textures enabled status
                material_attribute = Material::SPECULAR;
                enabled = material->isTextureEnabled(material_attribute);
                if (ImGui::Checkbox("Specular", &enabled)) {
                    material->setTextureEnabled(material_attribute, enabled);
                    for (std::size_t i = 0U; i < materials; i++) {
                        model->getMaterial(i)->setTextureEnabled(material_attribute, enabled);
                    }
                }

                // Displacement textures enabled status
                material_attribute = Material::DISPLACEMENT;
                enabled = material->isTextureEnabled(material_attribute);
                ImGui::SameLine(210.0F);
                if (ImGui::Checkbox("Displacement", &enabled)) {
                    material->setTextureEnabled(material_attribute, enabled);
                    for (std::size_t i = 0U; i < materials; i++) {
                        model->getMaterial(i)->setTextureEnabled(material_attribute, enabled);
                    }
                }

                // Pop textures enabled status node
                ImGui::TreePop();
            }

            // Pop global material
            ImGui::TreePop();
        }

        // Material stock
        for (std::size_t i = 0; i < materials; i++) {
            // Get the material and name
            material = model->getMaterial(i);
            std::string name = material->getName();
            std::string id =  std::to_string(i);

            if (ImGui::TreeNode(id.c_str(), name.c_str())) {
                // Material name
                if (ImGui::InputText("Name", &name, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    material->setName(name);
                }

                // Ambient color
                material_attribute = Material::AMBIENT;
                color = material->getColor(material_attribute);
                if (ImGui::ColorEdit3("Ambient", &color.x)) {
                    material->setColor(material_attribute, color);
                }

                // Diffuse color
                material_attribute = Material::DIFFUSE;
                color = material->getColor(material_attribute);
                if (ImGui::ColorEdit3("Diffuse", &color.x)) {
                    material->setColor(material_attribute, color);
                }

                // Specular color
                material_attribute = Material::SPECULAR;
                color = material->getColor(material_attribute);
                if (ImGui::ColorEdit3("Specular", &color.x)) {
                    material->setColor(material_attribute, color);
                }

                // Shininess value
                material_attribute = Material::SHININESS;
                value = material->getValue(material_attribute);
                if (ImGui::DragFloat("Shininess", &value)) {
                    material->setValue(material_attribute, value);
                }

                // Roughness value
                material_attribute = Material::ROUGHNESS;
                value = material->getValue(material_attribute);
                if (ImGui::DragFloat("Roughness", &value)) {
                    material->setValue(material_attribute, value);
                }

                // Metalness value
                material_attribute = Material::METALNESS;
                value = material->getValue(material_attribute);
                if (ImGui::DragFloat("Metalness", &value)) {
                    material->setValue(material_attribute, value);
                }

                // Parallax value
                material_attribute = Material::DISPLACEMENT;
                value = material->getValue(material_attribute);
                if (ImGui::DragFloat("Parallax", &value)) {
                    material->setValue(material_attribute, value);
                }

                // Textures node
                if (ImGui::TreeNode("Textures")) {
                    // For each texture
                    for (const std::pair<const Material::Attribute, const std::string> &texture : InteractiveScene::AVAILABLE_TEXTURE) {
                        // Get the texture ID
                        id = std::to_string(texture.first);
                        if (ImGui::TreeNode(id.c_str(), texture.second.c_str())) {
                            // Texture path
                            str =  material->getTexturePath(texture.first);
                            if (ImGui::InputText("Path", &str, ImGuiInputTextFlags_EnterReturnsTrue)) {
                                material->setTexturePath(texture.first, str);
                            }

                            // Enabled status
                            enabled = material->isTextureEnabled(texture.first);
                            if (ImGui::Checkbox("Enabled", &enabled)) {
                                material->setTextureEnabled(texture.first, enabled);
                            }

                            // Reload buttons
                            ImGui::SameLine();
                            if (ImGui::Button("Reload texture")) {
                                material->reloadTexture(texture.first);
                            }

                            // Draw texture
                            ImGui::Image(reinterpret_cast<void *>(material->getTexture(texture.first)), ImVec2(300.0F, 300.0F), ImVec2(0.0F, 1.0F), ImVec2(1.0F, 0.0F));

                            // Separator for open nodes
                            ImGui::Separator();

                            // Pop the texture node
                            ImGui::TreePop();
                        }
                    }

                    // Pop the textures node
                    ImGui::TreePop();
                }

                // Separator for open nodes
                ImGui::Separator();

                // Pop the material node
                ImGui::TreePop();
            }
        }

        // Pop materials node
        ImGui::TreePop();
    }

    // Separator for open nodes
    ImGui::Separator();

    // Return the keep model value
    return keep;
}

// Draw the light widget
bool InteractiveScene::lightWidget(Light *const light) {
    // Keep light flag
    bool keep = true;

    // Type combo
    Light::Type type = light->getType();
    if (ImGui::BeginCombo("Type", InteractiveScene::LIGHT_TYPE_LABEL[type])) {
        for (GLint i = Light::DIRECTIONAL; i <= Light::SPOTLIGHT; i++) {
            // Selected status
            const Light::Type new_type = static_cast<Light::Type>(i);
            bool selected = type == new_type;

            // Show item and select
            if (ImGui::Selectable(InteractiveScene::LIGHT_TYPE_LABEL[new_type], selected)) {
                light->setType(new_type);
            }

            // Set default focus to selected
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Enabled checkbox
    bool status = light->isEnabled();
    if (ImGui::Checkbox("Enabled", &status)) {
        light->setEnabled(status);
    }
    // Grabed checkbox for spotlight lights
    if (type == Light::SPOTLIGHT) {
        status = light->isGrabbed();
        ImGui::SameLine();
        if (ImGui::Checkbox("Grabbed", &status)) {
            light->setGrabbed(status);
        }
    }
    // Remove button if there are more than one light in the stock
    if (light_stock.size() > 1U) {
        keep = !ImGui::RemoveButton();
    }


    // Space attributes
    ImGui::BulletText("Spacial attributes");
    ImGui::Indent();

    // Direction
    glm::vec3 vector = light->getDirection();
    if (ImGui::DragFloat3("Direction", &vector.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
        light->setDirection(vector);
    }

    // Position
    vector = light->getPosition();
    if (ImGui::DragFloat3("Position", &vector.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
        light->setPosition(vector);
    }

    // Attenuation for non directional lights
    if (type != Light::DIRECTIONAL) {
        vector = light->getAttenuation();
        if (ImGui::DragFloat3("Attenuation", &vector.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
            light->setAttenuation(vector);
        }
        ImGui::HelpMarker("[Constant, Linear, Quadratic]\nIf any value is negative rare\neffects may appear.");
    }

    // Cutoff for spotlight lights
    if (type == Light::SPOTLIGHT) {
        glm::vec2 cutoff = light->getCutoff();
        if (ImGui::DragFloat2("Cutoff", &cutoff.x, 0.01F, 0.0F, 0.0F, "%.4f")) {
            light->setCutoff(cutoff);
        }
        ImGui::HelpMarker("[Inner, Outter]\nIf the inner cutoff is greater than the\noutter cutoff rare effects may appear.");
    }
    ImGui::Unindent();


    // Color attributes
    ImGui::BulletText("Color attributes");
    ImGui::Indent();

    // Ambient color
    vector = light->getAmbientColor();
    if (ImGui::ColorEdit3("Ambient", &vector.r)) {
        light->setAmbientColor(vector);
    }

    // Diffuse color
    vector = light->getDiffuseColor();
    if (ImGui::ColorEdit3("Diffuse", &vector.r)) {
        light->setDiffuseColor(vector);
    }

    // Specular color
    vector = light->getSpecularColor();
    if (ImGui::ColorEdit3("Specular", &vector.r)) {
        light->setSpecularColor(vector);
    }
    ImGui::Unindent();


    // Colors levels
    ImGui::BulletText("Color values");
    ImGui::Indent();

    // Ambient color
    float value = light->getAmbientLevel();
    if (ImGui::DragFloat("Ambient level", &value, 0.0025F, 0.0F, 1.0F, "%.4f")) {
        light->setAmbientLevel(value);
    }

    // Diffuse color
    value = light->getDiffuseLevel();
    if (ImGui::DragFloat("Diffuse level", &value, 0.0025F, 0.0F, 1.0F, "%.4f")) {
        light->setDiffuseLevel(value);
    }

    // Specular color
    value = light->getSpecularLevel();
    if (ImGui::DragFloat("Specular level", &value, 0.0025F, 0.0F, 1.0F, "%.4f")) {
        light->setSpecularLevel(value);
    }

    // Shininess
    value = light->getShininess();
    if (ImGui::DragFloat("Shininess", &value, 0.0025F, 0.0F, 0.0F, "%.4f")) {
        light->setShininess(value);
    }
    ImGui::HelpMarker("If the shininess value negative\nrare effects may appear.");
    ImGui::Unindent();

    // Return the keep light value
    return keep;
}

// Draw the program widget
bool InteractiveScene::programWidget(std::pair<GLSLProgram *, std::string> &program_data) {
    // Keep program flag
    bool keep = true;

    // Get the program and shader paths
    GLSLProgram *const program = program_data.first;
    std::string vert = program->getShaderPath(GL_VERTEX_SHADER);
    std::string geom = program->getShaderPath(GL_GEOMETRY_SHADER);
    std::string frag = program->getShaderPath(GL_FRAGMENT_SHADER);

    // Default program flag
    const bool default_program = (program == program_stock[0U].first) || (program == program_stock[1U].first);

    // Program description
    std::string description = program_data.second;
    if (ImGui::InputText("Description", &description, ImGuiInputTextFlags_EnterReturnsTrue)) {
        program_data.second = description;
    }
    // Reload button
    if (ImGui::Button("Reload")) {
        program->link();
    }
    // Remove button for non default program
    if (!default_program) {
        keep = !ImGui::RemoveButton();
    }
    // Check the valid status
    if (!program->isValid()) {
        ImGui::TextColored(ImVec4(0.80F, 0.16F, 0.16F, 1.00F), "Could not link the program");
    }

    // Shaders sources paths
    ImGui::BulletText("Shaders");

    // Vertex shader source path
    bool link = ImGui::InputText("Vertex", &vert, ImGuiInputTextFlags_EnterReturnsTrue);

    // Geometry shader source path for non default programs
    if (!default_program) {
        link |= ImGui::InputText("Geometry", &geom, ImGuiInputTextFlags_EnterReturnsTrue);
    }

    // Fragment shader source path
    link |= ImGui::InputText("Fragment", &frag, ImGuiInputTextFlags_EnterReturnsTrue);

    // Relink if a path has been modified
    if (link) {
        // Program without geometry shader
        if (geom.empty()) {
            program->link(vert, frag);
        }
        // Program with geometry shader
        else {
            program->link(vert, geom, frag);
        }
    }

    // Separator for open nodes
    ImGui::Separator();

    // Return the keep program value
    return keep;
}

// Draw a program combo item
bool InteractiveScene::programComboItem(const std::size_t &current, const std::size_t &program) {
    // Selected status
    bool selected = current == program;

    // Get the program title and append the ID for non default programs
    std::string program_title = program_stock[program].second;
    if ((program != 0U) && (program != 1U)) {
        program_title.append(" (").append(std::to_string(program)).append(")");
    }

    // Show item and get the selection status
    const bool selection = ImGui::Selectable(program_title.c_str(), selected);

    // Set default focus to selected
    if (selected) {
        ImGui::SetItemDefaultFocus();
    }

    // Return the selection
    return selection;
}


// Process keyboard input
void InteractiveScene::processKeyboardInput() {
    // Return if the GUI wants the IO and is visible
    ImGuiIO &io = ImGui::GetIO();
    if ((io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput) && show_main_gui) {
        return;
    }

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W))                                           active_camera->travell(Camera::FRONT);
    if (glfwGetKey(window, GLFW_KEY_S))                                           active_camera->travell(Camera::BACK);
    if (glfwGetKey(window, GLFW_KEY_A)     || glfwGetKey(window, GLFW_KEY_LEFT))  active_camera->travell(Camera::LEFT);
    if (glfwGetKey(window, GLFW_KEY_D)     || glfwGetKey(window, GLFW_KEY_RIGHT)) active_camera->travell(Camera::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) || glfwGetKey(window, GLFW_KEY_UP))    active_camera->travell(Camera::UP);
    if (glfwGetKey(window, GLFW_KEY_C)     || glfwGetKey(window, GLFW_KEY_DOWN))  active_camera->travell(Camera::DOWN);

}



// Constructor

// Interactive scene constructor
InteractiveScene::InteractiveScene(const std::string &title, const int &width, const int &height, const int &context_ver_maj, const int &context_ver_min) :
    // Scene
    Scene(title, width, height, context_ver_maj, context_ver_min),

    // Mouse
    mouse(new Mouse(width, height)),

    // Cursor
    cursor_enabled(true),
    cursor_position(0.0F),

    // Focus
    focus(InteractiveScene::GUI),

    // Show default GUI windows
    show_main_gui(true),
    show_metrics(false),
    show_about(false),
    show_about_imgui(false),

    // Focus on GUI by default
    focus_gui(true) {
    // Load the GUI if is the first instance
    if ((Scene::instances == 1U) && Scene::initialized_glad) {
        // Set the user pointer to this scene and setup callbacks
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, InteractiveScene::framebufferSizeCallback);
        glfwSetMouseButtonCallback(window, InteractiveScene::mouseButtonCallback);
        glfwSetCursorPosCallback(window, InteractiveScene::cursorPosCallback);
        glfwSetScrollCallback(window, InteractiveScene::scrollCallback);
        glfwSetKeyCallback(window, InteractiveScene::keyCallback);


        // Setup the ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup platform and render
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        // Setup key navigation and disable ini file
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = NULL;

        // Setup the global style
        ImGui::StyleColorsDark();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 2.0F);
    }

    // Render the GUI before anything
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// Getters

// Get the main GUI visible status
bool InteractiveScene::isMainGUIVisible() const {
    return show_main_gui;
}

// Get the metrics window visible status
bool InteractiveScene::isMetricsVisible() const {
    return show_metrics;
}

// Get the about window visible status
bool InteractiveScene::isAboutVisible() const {
    return show_about;
}

// Get the about ImGui window visible status
bool InteractiveScene::isAboutImGuiVisible() const {
    return show_about_imgui;
}


// Get the cursor enabled status
bool InteractiveScene::isCursorEnabled() const {
    return ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NoMouse;
}

// Get the mouse
Mouse *InteractiveScene::getMouse() const {
    return mouse;
}


// Setters

// Set the main GUI visible status
void InteractiveScene::setMainGUIVisible(const bool &status) {
    show_main_gui = status;
    focus = InteractiveScene::GUI;
}

// Set the metrics window visible status
void InteractiveScene::setMetricsVisible(const bool &status) {
    // Update the visible status
    show_metrics = status;

    // Keep focus to scene if is not showing the main GUI window
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NoMouse) {
        focus = InteractiveScene::SCENE;
    }
}

// Set the about window visible status
void InteractiveScene::setAboutVisible(const bool &status) {
    // Update the visible status
    show_about = status;

    // Keep focus to scene if is not showing the main GUI window
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NoMouse) {
        focus = InteractiveScene::SCENE;
    }
}

// Set the about ImGui window visible status
void InteractiveScene::setAboutImGuiVisible(const bool &status) {
    // Update the visible status
    show_about_imgui = status;

    // Keep focus to scene if is not showing the main GUI window
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NoMouse) {
        focus = InteractiveScene::SCENE;
    }
}

// Set the mouse enabled status
void InteractiveScene::setCursorEnabled(const bool &status) {
    // Enable cursor
    if (status) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        focus = InteractiveScene::GUI;
    }

    // Disable cursor
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
        focus = InteractiveScene::SCENE;

        // Update position
        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        mouse->setTranslationPoint(xpos, ypos);
    }
}


// Methods

/** Render main loop */
void InteractiveScene::mainLoop() {
    // Check the window status
    if (window == nullptr) {
        std::cerr << "error: there is no window" << std::endl;
        return;
    }

    // Check the default geometry pass valid status
    if (!program_stock[0U].first->isValid()) {
        std::cerr << "warning: the default geometry pass program has not been set or is not valid" << std::endl;
    }

    // Check the default lighting pass valid status
    if (!program_stock[1U].first->isValid()) {
        std::cerr << "warning: the default lighting pass program has not been set or is not valid" << std::endl;
    }

    // The rendering main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the scene and GUI
        drawScene();
        drawGUI();

        // Process the keyboard input
        processKeyboardInput();

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);

        // Count frame
        kframes += 0.001;
    }
}


// Destructor

// Interactive scene destructor
InteractiveScene::~InteractiveScene() {
    // Delete the mouse
    delete mouse;

    // Terminate GUI if is the last instance
    if ((Scene::instances == 1U) && Scene::initialized_glad) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}


// Public static methods

// Draw the about window GUI
void InteractiveScene::showAboutWindow(bool &show) {
    // Creates the about window and abort if is not showing
    if (!ImGui::Begin("About OBJViewer", &show, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }

    // Title
    ImGui::Text("OBJViewer - Another OBJ models viewer");
    ImGui::Separator();

    // Signature
    ImGui::Text("By Erick Rincones 2019.");
    ImGui::Text("OBJViewer is licensed under the MIT License, see LICENSE for more information.");
    ImGui::Spacing();

    // Repository
    ImGui::Text("GitHub repository:"); ImGui::HelpMarker("Click to select all and press\nCTRL+V to copy to clipboard");

    ImGui::PushItemWidth(-1.0F);
    ImGui::InputText("###repourl", InteractiveScene::repository_url, sizeof(InteractiveScene::repository_url), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    // End window
    ImGui::End();
}