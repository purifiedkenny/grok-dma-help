#pragma once
#include <atomic>
#include <thread>
#include <vector>
#include "Config.h"
#include "VecDef.h"
#include "Player.h"

class Glow : public AutoConfigurable {
public:
    struct GlowSettings {
        bool enable = false;
        bool RainBowColor = false;
        bool checkShield = false;
        int outlinesize{ 50 };
        float distance = 200.f;
        float GlowAdminSpeed = 1.0f;
        float HighlightScale{ 50.f };
        ImVec4 GlowKnocked{ 0.f,0.f,1.f,1.f };
        ImVec4 GlowInvisible{ 1.f,1.f,1.f,1.f };
        ImVec4 GlowVisible{ 1.f,0.f,0.f,1.f };
        ImVec4 GlowAdmin{ 1.f,0.f,0.f,1.f };
        int insidetype = 101;
        int outlinetype = 125;
    };

    struct ItemSettings {
        bool enable = false;
        bool Item_Ammo{ true };
        bool Item_Weapons{ true };
        bool Item_Grey{ true };
        bool Item_Blue{ true };
        bool Item_Purple{ true };
        bool Item_Gold{ true };
        bool Item_Red{ true };
    };

    bool enable = true;
    bool ignoreTeammates = true;
    bool ignoreBots = false;

    GlowSettings eneities;       // ��ҷ�������
    ItemSettings itemSettings;  // ��Ʒ��������

    // UI��Ⱦ����
    void renderMenu();
    void renderEnemyTab();
    void renderItemTab();
    void renderGeneralTab();

    // �����к���
    void run();


    Glow();

private:
    uint64_t HighlightSettingsPointer;
    ImVec4 GetRainbowColor(float gameTime, float speed = 1.0f);
    void setCustomGlow(Player* Target, int enable, int wall, bool isVisible);
};