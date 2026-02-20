#include "include.h"
#include "Glow.h"
#include "GlowMode.h"

Glow::Glow() : AutoConfigurable("Glow") {
    // ע�����ò��� - ��������
    registerParam("Global_Enable", enable);
    registerParam("Global_IgnoreTeammates", ignoreTeammates);
    registerParam("Global_IgnoreBots", ignoreBots);

    registerParam("Enemy_Enable", eneities.enable);
    registerParam("Enemy_Distance", eneities.distance);
    registerParam("Enemy_RainbowColor", eneities.RainBowColor);
    registerParam("Enemy_CheckShield", eneities.checkShield);
    registerParam("Enemy_OutlineSize", eneities.outlinesize);
    registerParam("Enemy_GlowSpeed", eneities.GlowAdminSpeed);
    registerParam("Enemy_HighlightScale", eneities.HighlightScale);
    registerParam("Enemy_KnockedColor", eneities.GlowKnocked);
    registerParam("Enemy_InvisibleColor", eneities.GlowInvisible);
    registerParam("Enemy_VisibleColor", eneities.GlowVisible);
    registerParam("Enemy_AdminColor", eneities.GlowAdmin);
    registerParam("Enemy_InsideType", eneities.insidetype);
    registerParam("Enemy_OutlineType", eneities.outlinetype);


    // ��Ʒ����
    registerParam("Item_Enable", itemSettings.enable);
    registerParam("Item_Ammo", itemSettings.Item_Ammo);
    registerParam("Item_Weapons", itemSettings.Item_Weapons);
    registerParam("Item_Grey", itemSettings.Item_Grey);
    registerParam("Item_Blue", itemSettings.Item_Blue);
    registerParam("Item_Purple", itemSettings.Item_Purple);
    registerParam("Item_Gold", itemSettings.Item_Gold);
    registerParam("Item_Red", itemSettings.Item_Red);
}

ImColor knocked = ImColor(17, 189, 3, 255);
ImColor whitebar = ImColor(255, 255, 255, 255);
ImColor bluebar = ImColor(0, 100, 255, 255);
ImColor purplebar = ImColor(226, 0, 255, 255);
ImColor redbar = ImColor(255, 0, 0, 255);
ImColor nonbar = ImColor(255, 165, 0, 255);

ImVec4 Glow::GetRainbowColor(float gameTime, float speed) {
    float hue = fmodf(gameTime * speed, 1.0f);

    float saturation = 1.0f;
    float value = 1.0f;

    float c = value * saturation;
    float x = c * (1 - fabsf(fmodf(hue * 6, 2) - 1));
    float m = value - c;

    float r, g, b;
    if (hue < 1.0f / 6.0f) {
        r = c; g = x; b = 0;
    }
    else if (hue < 2.0f / 6.0f) {
        r = x; g = c; b = 0;
    }
    else if (hue < 3.0f / 6.0f) {
        r = 0; g = c; b = x;
    }
    else if (hue < 4.0f / 6.0f) {
        r = 0; g = x; b = c;
    }
    else if (hue < 5.0f / 6.0f) {
        r = x; g = 0; b = c;
    }
    else {
        r = c; g = 0; b = x;
    }

    // Return ImVec4 (0-1 range)
    return ImVec4(
        r + m,
        g + m,
        b + m,
        1.0f
    );
}

void Glow::run() {
    static const std::vector<uint8_t> ItemHighlightID = { 15, 42, 47, 54, 65, 9, 58 };
    {

        if (!localPlayer || localPlayer->IsDead || !enable) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            return;
        }

        mem.Read(mem.OFF_BASE + OFF_GLOW_HIGHLIGHTS, &HighlightSettingsPointer, sizeof(uint64_t));
        if (!mem.IsValidPointer(HighlightSettingsPointer)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            return;
        }

        // ������Ʒ����
        if (itemSettings.enable) {
            uint64_t highlightSize = OFF_GLOW_HIGHLIGHT_TYPE_SIZE;
            const GlowMode newGlowMode = { 137, 138, 35, 127 };

            const std::vector<bool> itemFlags = {
                itemSettings.Item_Gold,
                itemSettings.Item_Red,
                itemSettings.Item_Purple,
                itemSettings.Item_Blue,
                itemSettings.Item_Grey,
                itemSettings.Item_Weapons,
                itemSettings.Item_Ammo
            };

            for (size_t i = 0; i < ItemHighlightID.size(); ++i) {
                if (itemFlags[i]) {
                    uint8_t highlightId = ItemHighlightID[i];
                    uint64_t highlightAddress = HighlightSettingsPointer + (highlightSize * highlightId) + 0;
                    if (!mem.IsValidPointer(highlightAddress))
                        continue;

                    const GlowMode oldGlowMode = mem.Read<GlowMode>(highlightAddress);
                    if (newGlowMode != oldGlowMode) {
                        mem.Write<GlowMode>(
                            HighlightSettingsPointer + (highlightSize * highlightId) + 0, newGlowMode);
                    }
                }
            }
        }

        if (!eneities.enable)
            return;

        const auto& playersCopy = entityManager.getPlayers();
        //���Ǵ����߼���ͬ
        if (!playersCopy->empty()) {
            for (const auto& player : *playersCopy) {
                if (player == nullptr || player->IsDead || player->Health <= 0 || player->IsLocal)
                    continue;

                // �����������ѡ���Ӧ�ķ�������
                if ((player->IsHostile) || (player->IsTeamMate && !ignoreTeammates) || (player->IsDummy() && !ignoreBots)) {
                    setCustomGlow(player, 1, 1, player->IsVisible);
                }
            }
        }
    }
}

void Glow::setCustomGlow(Player* Target, int enable, int wall, bool isVisible) {
    if (Target->GlowEnable == 0 && Target->GlowThroughWall == 0 && Target->HighlightID == 0) {
        return;
    }

    uint64_t basePointer = Target->BasePointer;

    if (!mem.IsValidPointer(mem.Read<uint64_t>(basePointer)))
        return;

    int settingIndex = 0;
    std::array<float, 3> glowColorRGB = { 0, 0, 0 };

    // Determine state and set color accordingly
    if (Target->IsKnocked) {
        settingIndex = 61;
        glowColorRGB = {
            eneities.GlowKnocked.x * eneities.HighlightScale,
            eneities.GlowKnocked.y * eneities.HighlightScale,
            eneities.GlowKnocked.z * eneities.HighlightScale
        };
    }
    else if (Target->IsVisible) {
        settingIndex = 62;
        glowColorRGB = {
            eneities.GlowVisible.x * eneities.HighlightScale,
            eneities.GlowVisible.y * eneities.HighlightScale,
            eneities.GlowVisible.z * eneities.HighlightScale
        };
    }
    else {
        if (eneities.checkShield && Target->IsHostile) {
            if (Target->Shield + Target->Health <= 100) {
                settingIndex = 63;
                glowColorRGB = { nonbar.Value.x, nonbar.Value.y, nonbar.Value.z };
            }
            else if (Target->Shield + Target->Health <= 150) {
                settingIndex = 64;
                glowColorRGB = { whitebar.Value.x, whitebar.Value.y, whitebar.Value.z };
            }
            else if (Target->Shield + Target->Health <= 175) {
                settingIndex = 66;
                glowColorRGB = { bluebar.Value.x, bluebar.Value.y, bluebar.Value.z };
            }
            else if (Target->Shield + Target->Health <= 200) {
                settingIndex = 67;
                glowColorRGB = { purplebar.Value.x, purplebar.Value.y, purplebar.Value.z };
            }
            else if (Target->Shield + Target->Health <= 225) {
                settingIndex = 68;
                glowColorRGB = { redbar.Value.x, redbar.Value.y, redbar.Value.z };
            }
            else {
                settingIndex = 69;
                glowColorRGB = { 255.0 / 2, 255.0 / 2, 255.0 / 2 };
            }
        }
        else if (Target->IsPro && Target->IsHostile) {
            settingIndex = 60;
            ImVec4 color = eneities.RainBowColor ?
                GetRainbowColor((float)ImGui::GetTime(), eneities.GlowAdminSpeed / 10) :
                eneities.GlowAdmin;

            glowColorRGB = {
                color.x * eneities.HighlightScale,
                color.y * eneities.HighlightScale,
                color.z * eneities.HighlightScale
            };
        }
        else {
            settingIndex = 69;
            glowColorRGB = {
                eneities.GlowInvisible.x * eneities.HighlightScale,
                eneities.GlowInvisible.y * eneities.HighlightScale,
                eneities.GlowInvisible.z * eneities.HighlightScale
            };
        }
    }

    // Prepare all values before writing
    uint8_t enableValue = static_cast<uint8_t>(enable);
    uint8_t wallValue = static_cast<uint8_t>(wall);
    uint8_t highlightValue = static_cast<uint8_t>(settingIndex);
    uint8_t fixValue = 0;

    // Prepare highlight function bits
    std::array<uint8_t, 4> highlightFunctionBits = {
        static_cast<uint8_t>(eneities.insidetype),
        static_cast<uint8_t>(eneities.outlinetype),
        static_cast<uint8_t>(eneities.outlinesize),
        64
    };

    // Calculate base addresses
    uint64_t glowEnableAddress = basePointer + OFF_GLOW_ENABLE;
    uint64_t glowThroughWallAddress = basePointer + OFF_GLOW_THROUGH_WALL;
    uint64_t highlightIdAddress = basePointer + OFF_GLOW_HIGHLIGHT_ID;
    uint64_t glowFixAddress = basePointer + OFF_GLOW_FIX;
    uint64_t highlightOffset = OFF_GLOW_HIGHLIGHT_TYPE_SIZE * settingIndex;

    // Change memory protection
    DWORD oldProtect;
    VirtualProtect((LPVOID)HighlightSettingsPointer, sizeof(highlightFunctionBits) + sizeof(glowColorRGB),
        PAGE_EXECUTE_READWRITE, &oldProtect);

    // Synchronized writes for glow settings
    if (Target->GlowEnable != enable) {
        mem.Write<uint8_t>(glowEnableAddress, enableValue);
    }

    if (Target->GlowThroughWall != wall) {
        mem.Write<uint8_t>(glowThroughWallAddress, wallValue);
    }

    // Write highlight ID and fix value
    mem.Write<uint8_t>(highlightIdAddress, highlightValue);
    mem.Write<uint8_t>(glowFixAddress, fixValue);

    // Write highlight function bits in a synchronized manner
    uint64_t highlightBaseAddress = HighlightSettingsPointer + highlightOffset;
    for (size_t i = 0; i < highlightFunctionBits.size(); ++i) {
        mem.Write<uint8_t>(highlightBaseAddress + i, highlightFunctionBits[i]);
    }

    // Write color values in a synchronized manner
    uint64_t colorBaseAddress = highlightBaseAddress + 4;
    for (size_t i = 0; i < glowColorRGB.size(); ++i) {
        mem.Write<float>(colorBaseAddress + (i * sizeof(float)), glowColorRGB[i]);
    }

    // Restore memory protection
    VirtualProtect((LPVOID)HighlightSettingsPointer, sizeof(highlightFunctionBits) + sizeof(glowColorRGB),
        oldProtect, &oldProtect);
}

void Glow::renderMenu() {
    // ʹ�ô������
    ImGui::Text("%s", t_("GLOW SETTINGS"));

    ImGui::Separator();
    ImGui::Spacing();

    // ���Ӷ���ˮƽ���� - �򻯰汾
    static int glowTab = 0; // 0=General, 1=Enemy, 2=Teammate, 3=Bot, 4=Item

    // �������ı�ǩ
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.18f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.30f, 0.40f, 1.0f));

    // ˮƽ���еı�ǩ
    if (ImGui::Button(t_("General"), ImVec2(70, 36))) glowTab = 0;
    ImGui::SameLine();
    if (ImGui::Button(t_("Player Setting"), ImVec2(70, 36))) glowTab = 1;
    ImGui::SameLine();
    if (ImGui::Button(t_("Items"), ImVec2(70, 36))) glowTab = 2;

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // ����ѡ��ı�ǩҳ��ʾ��ͬ����
    switch (glowTab) {
    case 0: // General
        renderGeneralTab();
        break;
    case 1: // Enemy
        renderEnemyTab();
        break;
    case 2: // Item
        renderItemTab();
        break;
    }
}

void Glow::renderGeneralTab() {
    ImGui::BeginChild("GeneralContent", ImVec2(0, 0), false);

    // �������в���
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // ������
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // ������ - ��ſ���
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Global Enable"), &enable);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // ����������
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Glow Categories"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Ignore Teammates"), &ignoreTeammates);
    ImGui::Checkbox(t_("Ignore Bots"), &ignoreBots);

    ImGui::EndChild();

    ImGui::EndChild(); // GeneralContent
}

void Glow::renderEnemyTab() {
    ImGui::BeginChild("EnemyContent", ImVec2(0, 0), false);

    // �������в���
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // ������
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // ���˷��� - ��ſ���
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Player Glow"), &eneities.enable);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // ������ʾ����
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Glow Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Based On Shield"), &eneities.checkShield);
    ImGui::Checkbox(t_("Rainbow Admin Color"), &eneities.RainBowColor);
    if (eneities.RainBowColor) {
        ImGui::SliderFloat(t_("Rainbow Speed"), &eneities.GlowAdminSpeed, 1.0f, 5.0f, "%.1f");
    }

    ImGui::SliderInt(t_("Outline Size"), &eneities.outlinesize, 0, 255);
    ImGui::SliderFloat(t_("Highlight Scale"), &eneities.HighlightScale, 1.0f, 255.0f, "%.0f");

    ImGui::EndChild();

    ImGui::SameLine();

    // �Ҳ����
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // ��������
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Distance Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::SliderFloat(t_("Max Distance"), &eneities.distance, 50.0f, 500.0f, "%.0f meters");

    ImGui::Spacing();
    ImGui::Spacing();

    // ��ɫ����
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Color Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("%s:", t_("Visible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##VisibleColor", (float*)&eneities.GlowVisible,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Invisible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##InvisibleColor", (float*)&eneities.GlowInvisible,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Knocked Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##KnockedColor", (float*)&eneities.GlowKnocked,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Admin Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##AdminColor", (float*)&eneities.GlowAdmin,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::EndChild();

    ImGui::EndChild(); // EnemyContent
}

void Glow::renderItemTab() {
    ImGui::BeginChild("ItemContent", ImVec2(0, 0), false);

    // �������в���
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // ������
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // ��Ʒ���� - ��ſ���
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Item Glow"), &itemSettings.enable);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // ��Ʒ��������
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Item Types"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Ammo"), &itemSettings.Item_Ammo);
    ImGui::SameLine();
    ImGui::Checkbox(t_("Weapons"), &itemSettings.Item_Weapons);

    ImGui::EndChild();

    ImGui::SameLine();

    // �Ҳ����
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // ��Ʒ�ȼ�����
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Item Rarity"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Grey"), &itemSettings.Item_Grey);
    ImGui::SameLine();
    ImGui::Checkbox(t_("Blue"), &itemSettings.Item_Blue);

    ImGui::Checkbox(t_("Purple"), &itemSettings.Item_Purple);
    ImGui::SameLine();
    ImGui::Checkbox(t_("Gold"), &itemSettings.Item_Gold);

    ImGui::Checkbox(t_("Red"), &itemSettings.Item_Red);

    ImGui::EndChild();

    ImGui::EndChild(); // ItemContent
}