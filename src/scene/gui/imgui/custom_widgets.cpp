#include "imgui.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
void ImGui::HelpMarker(const char *desc, const bool &same_line) {
    if (same_line) {
        ImGui::SameLine();
    }
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// A red button thats returns true if is pressed
bool ImGui::RemoveButton(const bool &same_line) {
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.60F, 0.24F, 0.24F, 1.00F));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.80F, 0.16F, 0.16F, 1.00F));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70F, 0.21F, 0.21F, 1.00F));
    if (same_line) {
        ImGui::SameLine();
    }
    bool pressed = ImGui::Button("Remove");
    ImGui::PopStyleColor(3);
    return pressed;
}