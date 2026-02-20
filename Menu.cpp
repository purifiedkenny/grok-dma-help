#include "include.h"
#include "Menu.h"
#include "rawData.h"
#include "Config.h"
#include "Misc.h"
#include "ImGuiRender.h"
#include <gl/GL.h>
#include <shellapi.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

void SetupImGuiStyle(int theme)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // 基础样式设置 (所有主题通用)
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(10, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.Alpha = 1.0f;

    // 根据选择的主题设置颜色
    switch (theme)
    {
    case 0: // Dark (现代深色主题)
    {
        // 背景颜色
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.14f, 0.95f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.95f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.50f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.80f);

        // 文本颜色
        colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        // 标题和边框
        colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.20f, 0.75f);
        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.27f, 0.60f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // 按钮
        colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.50f, 0.50f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.65f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.85f, 1.00f);

        // 框架元素
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.24f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.36f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.45f, 0.67f);

        // 选择元素
        colors[ImGuiCol_Header] = ImVec4(0.33f, 0.35f, 0.65f, 0.53f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.40f, 0.75f, 0.67f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.45f, 0.85f, 0.80f);

        // 滑块、复选框
        colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.45f, 0.80f, 0.30f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.55f, 0.94f, 0.50f);

        // 下拉列表
        colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.40f, 0.80f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.65f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.35f, 0.65f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.28f, 0.80f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.25f, 0.25f, 0.40f, 0.90f);

        // 其他元素
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.30f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 1.00f, 0.40f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.18f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.45f, 0.30f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.55f, 0.40f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.70f, 0.50f);
    }
    break;

    case 1: // Light (清新浅色主题)
    {
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.14f, 0.95f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.95f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.50f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.80f);

        // 文本颜色
        colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        // 标题和边框
        colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.20f, 0.75f);
        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.27f, 0.60f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // 按钮
        colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.50f, 0.50f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.65f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.85f, 1.00f);

        // 框架元素
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.24f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.36f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.45f, 0.67f);

        // 选择元素
        colors[ImGuiCol_Header] = ImVec4(0.33f, 0.35f, 0.65f, 0.53f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.40f, 0.75f, 0.67f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.45f, 0.85f, 0.80f);

        // 滑块、复选框
        colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.45f, 0.80f, 0.30f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.55f, 0.94f, 0.50f);

        // 下拉列表
        colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.40f, 0.80f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.65f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.35f, 0.65f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.28f, 0.80f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.25f, 0.25f, 0.40f, 0.90f);

        // 其他元素
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.30f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 1.00f, 0.40f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.18f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.45f, 0.30f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.55f, 0.40f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.70f, 0.50f);
    }
    break;

    case 2: // Classic (ImGui默认主题的改进版)
    {
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.14f, 0.95f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.95f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.50f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.17f, 0.80f);

        // 文本颜色
        colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        // 标题和边框
        colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.20f, 0.75f);
        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.27f, 0.60f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // 按钮
        colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.50f, 0.50f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.65f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.85f, 1.00f);

        // 框架元素
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.24f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.36f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.45f, 0.67f);

        // 选择元素
        colors[ImGuiCol_Header] = ImVec4(0.33f, 0.35f, 0.65f, 0.53f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.40f, 0.75f, 0.67f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.45f, 0.85f, 0.80f);

        // 滑块、复选框
        colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.45f, 0.80f, 0.30f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.55f, 0.94f, 0.50f);

        // 下拉列表
        colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.40f, 0.80f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.65f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.35f, 0.65f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.28f, 0.80f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.25f, 0.25f, 0.40f, 0.90f);

        // 其他元素
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.30f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 1.00f, 0.40f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.18f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.45f, 0.30f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.55f, 0.40f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.70f, 0.50f);
    }
    break;
    }
}






void Menu::updateAvailableConfigs() {
    availableConfigs = ConfigManager::getAvailableConfigs();
    selectedConfigIndex = -1;
}

void Menu::renderConfigManager() {
    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Using larger font
    ImGui::Text("%s", t_("Configuration Management"));
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();

    // Layout settings
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.6f;
    float rightWidth = width * 0.4f;

    // Left panel - Config list
    ImGui::BeginChild("ConfigListPanel", ImVec2(leftWidth, 0), true);

    // Config list title
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Available Configurations"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Refresh list button
    if (ImGui::Button(t_("Refresh List"), ImVec2(100, 25))) {
        updateAvailableConfigs();
    }

    ImGui::Spacing();

    // Configuration list
    ImGui::BeginChild("ConfigList", ImVec2(0, 250), true);
    if (availableConfigs.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), t_("No configuration files found"));
    }
    else {
        for (int i = 0; i < availableConfigs.size(); i++) {
            if (ImGui::Selectable(availableConfigs[i].c_str(), selectedConfigIndex == i)) {
                selectedConfigIndex = i;
            }
        }
    }
    ImGui::EndChild();

    // New configuration name input
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Create New Configuration"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("%s", t_("Configuration Name"));
    ImGui::SameLine();
    ImGui::InputText("##ConfigName", newConfigName, sizeof(newConfigName));

    if (ImGui::Button(t_("Save as New Configuration"), ImVec2(180, 30))) {
        if (strlen(newConfigName) > 0) {
            if (ConfigManager::saveConfigToFile(newConfigName)) {
                updateAvailableConfigs();
                ImGui::OpenPopup("Success");
                memset(newConfigName, 0, sizeof(newConfigName));
            }
            else {
                ImGui::OpenPopup("Failed");
            }
        }
        else {
            ImGui::OpenPopup("Error");
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel - Action buttons
    ImGui::BeginChild("ConfigActionsPanel", ImVec2(rightWidth, 0), true);

    // Actions title
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Configuration Actions"));
    ImGui::PopStyleColor();
    ImGui::Separator();


    std::string defaultConfigPath = ConfigManager::getInstance().configDirectory + ConfigManager::getInstance().defaultConfigFile;

    if (fs::exists(defaultConfigPath)) {

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.4f, 0.6f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.5f, 0.7f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.6f, 0.8f, 1.0f));

        ImGui::Text((std::string(t_("Current Default Config:")) + currentDefault + ".json").c_str());

        if (ImGui::Button(t_("Load Default"), ImVec2(rightWidth - 20, 40))) {
            if (ConfigManager::loadDefaultConfig()) {
                SetupImGuiStyle(esp->uiSettings.themeSelected);
                ImGui::OpenPopup("Success");
            }
            else {
                ImGui::OpenPopup("Failed");
            }
        }

        if (ImGui::Button(t_("Delete Default"), ImVec2(rightWidth - 20, 40))) {
            fs::remove(defaultConfigPath);
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        ImGui::Spacing();
        ImGui::Separator();
    }

    // Selected configuration actions
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Selected Configuration Actions"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Disable buttons when no config is selected
    ImGui::BeginDisabled(selectedConfigIndex < 0 || selectedConfigIndex >= availableConfigs.size());

    if (ImGui::Button(t_("Save Configuration"), ImVec2(rightWidth - 20, 40))) {
        if (selectedConfigIndex >= 0 && selectedConfigIndex < availableConfigs.size())
        {
            if (ConfigManager::saveConfigToFile(availableConfigs[selectedConfigIndex])) {
                ImGui::OpenPopup("Success");
            }
            else {
                ImGui::OpenPopup("Failed");
            }
        }
    }

    ImGui::Spacing();

    if (ImGui::Button(t_("Load Configuration"), ImVec2(rightWidth - 20, 40))) {
        if (selectedConfigIndex >= 0 && selectedConfigIndex < availableConfigs.size()) {
            if (ConfigManager::loadConfigFromFile(availableConfigs[selectedConfigIndex])) {
                ImGui::OpenPopup("Success");
            }
            else {
                ImGui::OpenPopup("Failed");
            }
        }
    }

    ImGui::Spacing();

    if (ImGui::Button(t_("Save as Default"), ImVec2(rightWidth - 20, 40))) {
        if (selectedConfigIndex >= 0 && selectedConfigIndex < availableConfigs.size()) {
            if (ConfigManager::saveAsDefault(availableConfigs[selectedConfigIndex])) {
                ImGui::OpenPopup("Success");
            }
            else {
                ImGui::OpenPopup("Failed");
            }
        }
    }

    ImGui::Spacing();

    if (ImGui::Button(t_("Delete Configuration"), ImVec2(rightWidth - 20, 40))) {
        if (selectedConfigIndex >= 0 && selectedConfigIndex < availableConfigs.size()) {
            if (ConfigManager::deleteConfigFile(availableConfigs[selectedConfigIndex])) {
                updateAvailableConfigs();
                ImGui::OpenPopup("Success");
            }
            else {
                ImGui::OpenPopup("Failed");
            }
        }
    }

    ImGui::EndDisabled();

    ImGui::EndChild();

    // Popup windows
    if (ImGui::BeginPopupModal(t_("Success"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", t_("Operation Completed Successfully!"));
        ImGui::Separator();

        if (ImGui::Button(t_("OK"), ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal(t_("Failed"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", t_("Operation Failed. Please Check Error Logs!"));
        ImGui::Separator();

        if (ImGui::Button(t_("OK"), ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal(t_("Error"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", t_("Please Enter a Configuration Name!"));
        ImGui::Separator();

        if (ImGui::Button(t_("OK"), ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Menu::renderMiscPage() {
    // 标题
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // 使用较大字体
    ImGui::Text("%s", t_("Settings"));
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.48f;
    float rightWidth = width * 0.48f;

    // 左侧面板
    ImGui::BeginChild("MiscLeftPanel", ImVec2(leftWidth, 0), true);

    // 语言设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Language Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // 语言选择
    ImGui::Text("%s", t_("Selected Language: "));

    // 语言下拉选择框
    const char* languages[] = { "English", "简体中文", "繁體中文", "日本語" };
    if (ImGui::BeginCombo("##LanguageSelector", languages[esp->uiSettings.languageSelected])) {
        for (int i = 0; i < IM_ARRAYSIZE(languages); i++) {
            bool isSelected = (esp->uiSettings.languageSelected == i);
            if (ImGui::Selectable(languages[i], isSelected)) {
                esp->uiSettings.languageSelected = i;
                Translation::setLanguage(static_cast<Translation::Language>(esp->uiSettings.languageSelected));
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 界面设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Interface Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox(t_("Show FPS"), &esp->drawFPS);

   

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("MiscRightPanel", ImVec2(rightWidth, 0), true);

    // 其他设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Other Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

	ImGui::Checkbox(t_("UseController"), &misc->uiParams.useController);
	auto& renderer = ImGuiRenderer::getInstance();
	if (misc->uiParams.useController) {
		renderer.renderGamepadBinding(t_("Menu Key"), &esp->uiSettings.menuKeyController);
		renderer.renderGamepadBinding(t_("End Key"), &esp->uiSettings.endKeyController);
		renderer.renderGamepadBinding(t_("Fusion Key"), &esp->uiSettings.fusionKeyController);
		renderer.renderGamepadBinding(t_("Radar Key"), &esp->uiSettings.radarKeyController);
	}
    else
    {
        renderer.renderKeyBinding(t_("Menu Key"), &esp->uiSettings.menuKey);
        renderer.renderKeyBinding(t_("End Key"), &esp->uiSettings.endKey);
        renderer.renderKeyBinding(t_("Fusion Key"), &esp->uiSettings.fusionKey);
        renderer.renderKeyBinding(t_("Radar Key"), &esp->uiSettings.radarKey);
    }
    if (ImGui::Button(t_("OpenWebRadar")))
        ShellExecute(NULL, L"open", L"http://localhost:8080", NULL, NULL, SW_SHOWNORMAL);

    misc->renderMenu();


    ImGui::EndChild();
}

void Menu::RenderMenu() {
    if (!isMenuVisible) {
        return;
    }

    // 设置窗口属性 - 800x800大小
    ImGui::SetNextWindowSize(ImVec2(800, 650), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

    // 设置样式
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::Begin("Xerus Free Apex DMA", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    // 分割布局：左侧导航栏(180px宽) + 右侧内容
    float navigationWidth = 180.0f;

    // 左侧导航区域
    ImGui::BeginChild("LeftPanel", ImVec2(navigationWidth, 0), true);

    // 导航按钮
    static int currentNav = 0; // 0=AIMBOT, 1=ESP, 2=GLOW 等
    const char* navItems[] = { "Aimbot", "ESP", "Glow", "Triggerbot", "Misc", "Config" };

    for (int i = 0; i < IM_ARRAYSIZE(navItems); i++) {
        // 设置当前项样式
        if (currentNav == i) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.45f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.35f, 0.45f, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        }

        // 按钮 - 使用翻译
        
        if (ImGui::NavButton(t_(navItems[i]), currentNav == i, {160, 90})) {
            currentNav = i;
        }

        ImGui::PopStyleColor(4);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧内容区域
    ImGui::BeginChild("ContentRegion", ImVec2(0, 0), true);

    // 根据当前选择的导航选项显示相应的内容
    switch (currentNav) {
    case 0: // AIMBOT
    {
        aimbot->renderUI();
        break;
    }
    case 1: // ESP
        esp->renderMenu();
        break;

    case 2: // GLOW
        glow->renderMenu();
        break;

    case 3: // TRIGGERBOT
        triggerbot->renderUI();
        break;

    case 4: // MISC
        renderMiscPage();
        break;

    case 5: // CONFIG
        renderConfigManager();
        break;
    }

    ImGui::EndChild();

    ImGui::End();
    ImGui::PopFont();
    ImGui::PopStyleVar(5);
}

void SetWindowTransparent(HWND hwnd, bool transparent) {
    if (transparent) {
        // 1. 设置窗口为分层窗口
        LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
        ex_style |= WS_EX_LAYERED | WS_EX_NOACTIVATE; // 添加透明属性
        SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);

        // 2. 设置透明色(黑色)
        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

        // 3. 设置DWM扩展边框
        MARGINS margins = { -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        
    }
    else {
        // 恢复正常窗口样式
        LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
        ex_style &= ~(WS_EX_LAYERED | WS_EX_NOACTIVATE);
        SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);

        // 重置DWM设置
        MARGINS margins = { 0 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}


void Menu::run() {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 3);

    int detectedMajor = 1;
    int detectedMinor = 1;
    const char* vendorString = "Unknown";
    const char* rendererString = "Unknown";

    // 使用Windows API创建临时OpenGL上下文进行检测
    {
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        // 创建一个隐藏窗口用于检测OpenGL
        HWND hwnd = CreateWindowExA(0, "STATIC", "OpenGL Detection",
            WS_POPUP, 0, 0, 1, 1,
            NULL, NULL, GetModuleHandle(NULL), NULL);

        HDC hdc = GetDC(hwnd);
        int pixelFormat = ChoosePixelFormat(hdc, &pfd);
        SetPixelFormat(hdc, pixelFormat, &pfd);

        // 创建OpenGL上下文
        HGLRC hglrc = wglCreateContext(hdc);
        wglMakeCurrent(hdc, hglrc);

        // 获取版本信息
        const GLubyte* version = glGetString(GL_VERSION);
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* renderer = glGetString(GL_RENDERER);

        if (version) {
            std::string versionStr((const char*)version);
            std::cout << "OpenGL Version String: " << versionStr << std::endl;

            // 解析版本字符串 (格式通常是: "主版本.次版本.修订版本")
            size_t dotPos = versionStr.find('.');
            if (dotPos != std::string::npos && dotPos > 0) {
                detectedMajor = versionStr[dotPos - 1] - '0';
                if (dotPos + 1 < versionStr.length()) {
                    detectedMinor = versionStr[dotPos + 1] - '0';
                }
            }
        }

        if (vendor) vendorString = (const char*)vendor;
        if (renderer) rendererString = (const char*)renderer;

        std::cout << "Detected OpenGL Version: " << detectedMajor << "." << detectedMinor << std::endl;
        std::cout << "OpenGL Vendor: " << vendorString << std::endl;
        std::cout << "OpenGL Renderer: " << rendererString << std::endl;

        // 清理OpenGL上下文
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hglrc);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
    }

    // 根据检测到的版本信息，设置SFML上下文
    sf::ContextSettings settings;
    settings.majorVersion = detectedMajor;
    settings.minorVersion = detectedMinor;
    settings.depthBits = 0;           // 2D应用设为0，3D应用设为24
    settings.stencilBits = 0;         // 除非需要特殊效果，否则设为0
    settings.antiAliasingLevel = 0;   // 禁用抗锯齿以获取最高性能
    settings.sRgbCapable = false;     // 禁用sRGB以获取最高性能

    // 尝试不同的OpenGL版本
    std::vector<std::pair<int, int>> glVersions = {
        {detectedMajor, detectedMinor},  // 首选检测到的版本
        {3, 3},                          // 常见的最低支持版本
        {2, 1},                          // 更老的版本
        {1, 1}                           // 最基础的版本
    };

    sf::RenderWindow window;
    bool windowCreated = false;

    for (const auto& [major, minor] : glVersions) {
        settings.majorVersion = major;
        settings.minorVersion = minor;
        
        try {
            window.create(sf::VideoMode::getDesktopMode(), "Xerus-ApexLegends", 
                         sf::Style::None, sf::State::Fullscreen, settings);
            
            if (window.isOpen()) {
                windowCreated = true;
                std::cout << "Successfully created window with OpenGL " << major << "." << minor << std::endl;
                break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to create window with OpenGL " << major << "." << minor 
                      << ": " << e.what() << std::endl;
        }
    }

    if (!windowCreated) {
        MessageBoxA(NULL, "Failed to create window with any OpenGL version", "SFML Error", MB_ICONERROR);
        return;
    }
    bool imguiInitSuccess = ImGui::SFML::Init(window);

    if (!imguiInitSuccess) {
        std::cerr << "ImGui initialization failed!" << std::endl;
        window.close();
        MessageBoxA(NULL, "Failed to initialize ImGui", "SFML Error", MB_ICONERROR);
        return;
    }

    HWND hwnd = window.getNativeHandle();
    SetForegroundWindow(hwnd);
    SetActiveWindow(hwnd);

    window.setFramerateLimit(0);
    window.setVerticalSyncEnabled(false);

    // 确保窗口可见
    window.setPosition(sf::Vector2i(0, 0));
    window.requestFocus();
    window.setActive(true);

    // 配置ImGui
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // 启用键盘控制
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // 启用游戏手柄控制
	io.IniFilename = NULL; // 禁用配置文件

    auto& rendererIMGUI = ImGuiRenderer::getInstance();
    // 加载字体，手动指定字符范围
    ImFont* espFont = rendererIMGUI.addFont("esp", 15.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
    ImFont* normalFont = rendererIMGUI.addFont("normal",  20.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
    ImFont* espSmallFont = rendererIMGUI.addFont("esp_small",  12.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
    ImFont* espItemFont = rendererIMGUI.addFont("esp_item",  14.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
    ImFont* espItemSmallFont = rendererIMGUI.addFont("esp_item_small", 11.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());

    // 图标字体配置
    ImFontConfig iconConfig;
    iconConfig.MergeMode = false;
    iconConfig.PixelSnapH = true;
    iconConfig.RasterizerMultiply = 0.7f;
    float iconSize = 24.f;

    auto fontIcon = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(rawData, sizeof(rawData), iconSize, &iconConfig);

    // 添加菜单字体
    ImFont* menuFont = rendererIMGUI.addFont("menu", 18.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());

    // 记录图标字体
    ImFontInfo fontInfo(iconSize);
    fontInfo.font = fontIcon;
    fontInfo.loaded = true;
    fontInfo.path = "nullptr";
    rendererIMGUI.m_fonts["esp_item_icon"] = fontInfo;

    // 更新字体纹理
    ImGui::SFML::UpdateFontTexture();
    updateAvailableConfigs();
    SetupImGuiStyle(esp->uiSettings.themeSelected);

    auto newTime = std::chrono::steady_clock::now();
    auto oldTime1 = std::chrono::steady_clock::now();


    // 主循环
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        static bool old = fusionMode;
        if (fusionMode != old)
        {
            if (fusionMode)
            {

                window.close();

                // 创建覆盖整个屏幕的无边框窗口
                window.create(
                    sf::VideoMode::getDesktopMode(),
                    "Fusion-Off-Xerus-Apex",
                    sf::Style::None,
                    sf::State::Windowed,  // 无边框
                    settings
                );

                // 设置窗口透明属性
                HWND hwnd = window.getNativeHandle();
                SetWindowTransparent(hwnd, true);

                HWND desktopHwnd = FindWindowA(NULL, "Xerus-ApexLegends");
            
                SetForegroundWindow(desktopHwnd);
                SetFocus(desktopHwnd);

                UpdateWindow(desktopHwnd);
                UpdateWindow(hwnd);
            }
            else
            {
                window.close();

  
                window.create(sf::VideoMode::getDesktopMode(), "Xerus-ApexLegends", sf::Style::None, sf::State::Fullscreen, settings);
                

                // 禁用窗口透明属性
                HWND hwnd = window.getNativeHandle();
                SetWindowTransparent(hwnd, false);
                SetForegroundWindow(hwnd);
                SetActiveWindow(hwnd);

                window.requestFocus();
                window.setActive(true);
            }

            window.setFramerateLimit(0);
            window.setVerticalSyncEnabled(false);
            window.setPosition(sf::Vector2i(0, 0));


            old = fusionMode;
        }

        if (fusionMode) {
            window.clear(sf::Color::Transparent);
        }
        else {
            window.clear(sf::Color::Black);
        }

        newTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_second1 = newTime - oldTime1;
        if (elapsed_second1.count() >= 8) {
            VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_MEM, 1);
            VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_TLB, 1);
            oldTime1 = newTime;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        rendererIMGUI.newFrame();

        {
            entityManager.setLoggedIn(true);

            gameView->UpdateScreenSize(window.getSize().x, window.getSize().y);
            gameViewItems->UpdateScreenSize(window.getSize().x, window.getSize().y);

            gameView->RenderPointer = mem.ReadCache<uint64_t>(mem.OFF_BASE + OFF_VIEWRENDER);

            uint64_t MatrixPtr = mem.Read<uint64_t>(gameView->RenderPointer + OFF_VIEWMATRIX);
            mem.Read(MatrixPtr, &gameView->GameViewMatrix, sizeof(ViewMatrix));

            {
                if (isMenuVisible)
                    RenderMenu();
				if (esp->drawFPS)
                    rendererIMGUI.drawFPS({ 0,0 }, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                spectator->drawMenu();
            }



            esp->run();
            aimbot->RenderDraw();
            misc->renderSwitcher();

            if (aimbot->predictionSettings.enabled && aimbot->predictionSettings.showDebug) {
                aimbot->RenderPredictionDebug();
            }
        }



        ImGui::SFML::Render(window);
        window.display();
    }
   
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    ImGui::SFML::Shutdown();
#ifndef DEBUG_MODE
#endif
}