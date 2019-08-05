#include "imgui/imgui.h"

#include <string>


// Custom ImGui widgets
namespace ImGui {
    // Helper to display a little (?) mark which shows a tooltip when hovered.
    // In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
    void HelpMarker(const char *desc, const bool &same_line = true);

    // A red button thats returns true if is pressed
    bool RemoveButton(const bool &same_line = true);

    // ImGui::InputText() with std::string
    bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}