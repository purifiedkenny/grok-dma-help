#include "include.h"
#include "Misc.h"
#include "Menu.h"

Misc::Misc() : AutoConfigurable("Misc")
{
    registerParam("Misc_BHOP", uiParams.enableBhop);
    registerParam("Misc_TS", uiParams.enableTS);
    registerParam("Misc_SG", uiParams.enableSG);
    registerParam("Misc_GP", uiParams.enableGrapple);
    registerParam("MISC_UseController", uiParams.useController);
}
//附加到主线程
void Misc::run()
{

    static bool startSg = false;
    if (uiParams.enableSG)
    {
		float worldtime = mem.Read<float>(localPlayer->BasePointer + OFF_TIME_BASE);						   // Current time
		float traversalStartTime = mem.Read<float>(localPlayer->BasePointer + OFF_TRAVERSAL_START_TIME); // Time to start wall climbing
		float traversalProgress = mem.Read<float>(localPlayer->BasePointer + OFF_TRAVERSAL_PROGRESS);	   // Wall climbing, if > 0.87 it is almost over.
        static bool startSg = false;
        static float superGlideTimer = 0;
        static float fpsrate = 120;
        auto HangOnWall = -(traversalStartTime - worldtime);
        if (HangOnWall > 0.1 * 60 / fpsrate && HangOnWall < 0.12 * 60 / fpsrate) {
            mem.Write<int>(mem.OFF_BASE + OFF_IN_JUMP + 0x8, 4);
        }
        //(jump_state == 65 || jump_state == 115) 跳跃
        if (traversalProgress > 0.87 + 0.02 * fpsrate / 60 && !startSg && HangOnWall > 0.1 * 60 / fpsrate && HangOnWall < 1.5 * 60 / fpsrate && (mem.GetKeyboard()->IsKeyDown(VK_SPACE) || mem.isPressed(65) || mem.isPressed(115))) {
            superGlideTimer = worldtime;
            startSg = true;
        }
        if (startSg)
        {
            mem.Write<int>(mem.OFF_BASE + OFF_IN_JUMP + 0x8, 5);

            float currentTime;
            while (true) {
                if (currentTime = mem.Read<float>(localPlayer->BasePointer + OFF_TIME_BASE)) {
                    if (currentTime - superGlideTimer < 0.004 * 60 / fpsrate) {
                    }
                    else {
                        break;
                    }
                }
            }
            mem.Write<int>(mem.OFF_BASE + OFF_IN_DUCK + 0x8, 6);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            mem.Write<int>(mem.OFF_BASE + OFF_IN_JUMP + 0x8, 4);
            std::this_thread::sleep_for(std::chrono::milliseconds(320));

            startSg = false;
        }
    }

    if (uiParams.enableTS)
    {
        bool ts_start = true;
        bool longclimb = false;
        if (localPlayer->wallrunStart > localPlayer->wallrunClear) {  // 在爬墙
            float climbTime = localPlayer->TimeBase - localPlayer->wallrunStart;
            if (climbTime > 0.8) {
                longclimb = true;
                ts_start = false;
            }
            else
            {
                ts_start = true;
            }
        }
        if (ts_start) {
            if (longclimb) {
                if (localPlayer->TimeBase > localPlayer->wallrunClear + 0.1)
                    longclimb = false;
            }
            // when player is in air  and                       not skydrive    and  not longclimb and   not backward
            if (((localPlayer->Flags & 0x1) == 0) && !(localPlayer->skyDriveState > 0) && !longclimb && !(localPlayer->backWardState > 0))
            {
                if (((localPlayer->duckState > 0) && (localPlayer->forewardState == 33))) { //previously 33
                    if (localPlayer->forceForeward == 0) {
                        mem.Write<int>(mem.OFF_BASE + OFF_IN_FORWARD + 0x8, 1);
                    }
                    else {
                        mem.Write<int>(mem.OFF_BASE + OFF_IN_FORWARD + 0x8, 0);
                    }
                }
            }
            else if ((localPlayer->Flags & 0x1) != 0) {
                if (localPlayer->forewardState == 0) {
                    mem.Write<int>(mem.OFF_BASE + OFF_IN_FORWARD + 0x8, 0);
                }
                else if (localPlayer->forewardState == 33) {
                    mem.Write<int>(mem.OFF_BASE + OFF_IN_FORWARD + 0x8, 1);
                }
            }
        }
    }

    //兔子跳
    if (uiParams.enableBhop && !startSg)
    {
        static bool lstateOfForward = false;
        if ((mem.GetKeyboard()->IsKeyDown(VK_SPACE) || mem.isPressed(65)) && (mem.GetKeyboard()->IsKeyDown(VK_LCONTROL) || mem.isPressed(83)))
        {
            mem.Write<int>(mem.OFF_BASE + OFF_IN_JUMP + 0x8, 5);
            mem.Write<int>(mem.OFF_BASE + OFF_IN_FORWARD + 0x8, 5);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            mem.Write<int>(mem.OFF_BASE + OFF_IN_JUMP + 0x8, 4);
            mem.Write<int>(mem.OFF_BASE + OFF_IN_FORWARD + 0x8, 4);
            lstateOfForward = true;   
        }

		if (lstateOfForward)
		{
			if (mem.isPressed(33))
			{
				mem.Write<int>(mem.OFF_BASE + OFF_IN_FORWARD + 0x8, 5);
			}
			lstateOfForward = false;
		}
    }

    //自动钩
    if (uiParams.enableGrapple)
    {
        if (localPlayer->isGrppleActived)
        {
            if (localPlayer->isGRAPPLE == 1)
            {
                mem.Write<int>(mem.OFF_BASE + OFF_IN_JUMP + 0x8, 5);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                mem.Write<int>(mem.OFF_BASE + OFF_IN_JUMP + 0x8, 4);
            }

        }
    }
}

void Misc::renderMenu()
{
    // 现有的复选框
    ImGui::Checkbox(t_("SuperGlide"), &uiParams.enableSG);
    ImGui::Checkbox(t_("TapStrafe"), &uiParams.enableTS);
    ImGui::Checkbox(t_("BunnyHop"), &uiParams.enableBhop);
    ImGui::Checkbox(t_("AutoGrapple"), &uiParams.enableGrapple);

    // 参数切换器部分
    ImGui::Separator();
    ImGui::Text(t_("Config Switcher"));
    ImGui::Checkbox(t_("Enable Switcher"), &cfgSwitcher.enable);

    if (cfgSwitcher.enable)
    {

        auto& renderer = ImGuiRenderer::getInstance();

        if (uiParams.useController)
        {
            renderer.renderGamepadBinding(t_("Previous Config"), &cfgSwitcher.switchKeyUpController);
            renderer.renderGamepadBinding(t_("Next Config"), &cfgSwitcher.switchKeyNextController);
        }
        else
        {
            renderer.renderKeyBinding(t_("Previous Config"), &cfgSwitcher.switchKeyUp);
            renderer.renderKeyBinding(t_("Next Config"), &cfgSwitcher.switchKeyNext);
        }

        // 参数列表
        ImGui::Separator();
        ImGui::Text(t_("Config List"));

        // 显示当前参数列表及删除按钮
        for (int i = 0; i < cfgSwitcher.Lists.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::Text("%s", cfgSwitcher.Lists[i].c_str());
            ImGui::SameLine();
            if (ImGui::Button("X"))
            {
                cfgSwitcher.Lists.erase(cfgSwitcher.Lists.begin() + i);
                if (cfgSwitcher.activeIndex >= cfgSwitcher.Lists.size() && !cfgSwitcher.Lists.empty())
                {
                    cfgSwitcher.activeIndex = cfgSwitcher.Lists.size() - 1;
                }
                i--;
            }
            ImGui::PopID();
        }

        // 添加参数按钮
        if (ImGui::Button(t_("Add Config")))
        {
            ImGui::OpenPopup("Select Config");
        }

        // 参数选择弹窗
        if (ImGui::BeginPopup("Select Config"))
        {
            ImGui::Text(t_("Select a Config to add:"));
            for (auto cfgName : Menu::availableConfigs)
                if (ImGui::Selectable(cfgName.c_str()))
                {
                    // 检查是否已存在
                    bool exists = false;
                    for (const auto& existing : cfgSwitcher.Lists)
                    {
                        if (existing == cfgName)
                        {
                            exists = true;
                            break;
                        }
                    }

                    if (!exists)
                    {
                        cfgSwitcher.Lists.push_back(cfgName);
                    }

                    ImGui::CloseCurrentPopup();
                }
            ImGui::EndPopup();
        }
    }
}

void Misc::updateKeys() {
    static bool init = false;
    if (!init) {
        std::unordered_map<std::string, int> espKeys = {
            {"nextKey", cfgSwitcher.switchKeyNext},
            {"upKey", cfgSwitcher.switchKeyUp},
            {"nextKeyController", cfgSwitcher.switchKeyNextController},
            {"upKeyController", cfgSwitcher.switchKeyUpController},
        };

        keyDetector->registerKeysForContext("Misc", espKeys);
        init = true;
    }

    keyDetector->updateKeyCode("Misc", "nextKey", cfgSwitcher.switchKeyNext);
    keyDetector->updateKeyCode("Misc", "upKey", cfgSwitcher.switchKeyUp);
    keyDetector->updateKeyCode("Misc", "nextKeyController", cfgSwitcher.switchKeyNextController);
    keyDetector->updateKeyCode("Misc", "upKeyController", cfgSwitcher.switchKeyUpController);
}

bool Misc::isKeyHeld(const std::string& keyName) {
    return IS_KEY_HELD("Misc", keyName);
}

bool Misc::isKeyToggled(const std::string& keyName) {
    if (IS_KEY_TRIGGERED("Misc", keyName))
    {
        CLEAR_KEY_TOGGLE_STATE("Misc", keyName);
        return true;
    }
    return false;
}

void Misc::renderSwitcher()
{
    if (!cfgSwitcher.enable || cfgSwitcher.Lists.empty())
        return;

    updateKeys();

    if (isKeyToggled("nextKey") || (misc->uiParams.useController && isKeyToggled("nextKeyController")))
    {
        cfgSwitcher.activeIndex = (cfgSwitcher.activeIndex + 1) % cfgSwitcher.Lists.size();
        ConfigManager::loadConfigFromFile(cfgSwitcher.Lists[cfgSwitcher.activeIndex]);
    }
    else if (isKeyToggled("upKey") || (misc->uiParams.useController && isKeyToggled("upKeyController")))
    {
        cfgSwitcher.activeIndex = (cfgSwitcher.activeIndex - 1 + cfgSwitcher.Lists.size()) % cfgSwitcher.Lists.size();
        ConfigManager::loadConfigFromFile(cfgSwitcher.Lists[cfgSwitcher.activeIndex]);
    }

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Once);
    ImGui::Begin(t_("Configs"), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text(t_("Config List:"));
    for (int i = 0; i < cfgSwitcher.Lists.size(); i++)
    {
        ImGui::TextColored(i == cfgSwitcher.activeIndex ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (cfgSwitcher.Lists[i] + ".cfg").c_str());
    }
    ImGui::End();
    ImGui::PopFont();
}