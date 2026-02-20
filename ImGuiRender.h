#pragma once
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <imgui.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "VecDef.h"

// ������Ϣ�ṹ
struct ImFontInfo {
    ImFont* font;
    std::string path;
    float size;
    bool loaded;

    ImFontInfo() : font(nullptr), size(24.0f), loaded(false) {}
    ImFontInfo(float fontSize) : font(nullptr), size(fontSize), loaded(false) {}
};

class ImGuiRenderer {
private:
    // ˽�й��캯��������ģʽ��
    ImGuiRenderer();

    // ��ֹ����
    ImGuiRenderer(const ImGuiRenderer&) = delete;
    ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;

    // �������
    std::string m_currentFontName;

    // ��ȡ����
    ImFont* getImGuiFont(const std::string& fontName) const;

    // FPS������س�Ա
    std::chrono::steady_clock::time_point m_lastTime;
    float m_fpsUpdateTime;
    int m_frameCount;
    float m_currentFps;
    static constexpr float FPS_UPDATE_INTERVAL = 0.1f; // FPS���¼������

    // ���ر��������б�
    ImDrawList* getDrawList() const;

    // DirectX 12 ���
    ID3D12Device* m_pd3dDevice;
    HWND m_hwnd;

public:
    // ����ģʽ��ȡʵ��
    static ImGuiRenderer& getInstance();
    std::map<std::string, ImFontInfo> m_fonts;

    void newFrame();

    // �����������
    ImFont* addFont(const std::string& fontName, float fontSize, ImFontConfig* fontConfig, const ImWchar* glyphRanges);
    ImFont* addFont(const std::string& fontName, float fontSize = 24.0f);
    bool setCurrentFont(const std::string& fontName);
    bool fontExists(const std::string& fontName) const;
    ImFont* getFont(const std::string& fontName = "");

    //--------- �ı���Ⱦ���� -----------//
    // �����ı�
    void drawText(const std::string& text, const ImVec2& position,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        const float fontSize = 0,
        const std::string& fontName = "");

    void drawText(const std::string& text, const ImVec2& position,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        const std::string& fontName = "");

    // ���ƴ���Ӱ���ı�
    void drawTextWithShadow(const std::string& text, const ImVec2& position,
        const ImVec4& textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        const ImVec4& shadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.7f),
        float shadowOffset = 2.0f,
        const std::string& fontName = "");

    // ����汾
    void drawText(const std::string& text, float x, float y,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        const std::string& fontName = "");

    void drawTextWithShadow(const std::string& text, float x, float y,
        const ImVec4& textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        const ImVec4& shadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.7f),
        float shadowOffset = 2.0f,
        const std::string& fontName = "");

    // ��ȡ�ı��ߴ�
    ImVec2 getTextSize(const std::string& text, const std::string& fontName = "");

    //--------- ��״��Ⱦ���� -----------//
    // ��������
    void drawLine(const ImVec2& p1, const ImVec2& p2,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float thickness = 1.0f);

    void drawLine(float x1, float y1, float x2, float y2,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float thickness = 1.0f);

    // ���ƾ���
    void drawRect(const ImVec2& min, const ImVec2& max,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float thickness = 1.0f, float rounding = 0.0f, ImDrawFlags f = 0);

    void drawRect(float x, float y, float width, float height,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float thickness = 1.0f, float rounding = 0.0f, ImDrawFlags f = 0);

    // ����ʵ�ľ���
    void drawRectFilled(const ImVec2& min, const ImVec2& max,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float rounding = 0.0f, ImDrawFlags f = 0);

    void drawRectFilled(float x, float y, float width, float height,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float rounding = 0.0f, ImDrawFlags f = 0);

    // ����Բ��
    void drawCircle(const ImVec2& center, float radius,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        int numSegments = 0, float thickness = 1.0f);

    void drawCircle(float x, float y, float radius,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        int numSegments = 0, float thickness = 1.0f);

    // ����ʵ��Բ��
    void drawCircleFilled(const ImVec2& center, float radius,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        int numSegments = 0);

    void drawCircleFilled(float x, float y, float radius,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        int numSegments = 0);

    // ���ƽ������ (Corner Box)
    void drawCornerBox(const ImVec2& min, const ImVec2& max,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float thickness = 1.0f, float cornerSize = 0.25f);

    void drawCornerBox(float x, float y, float width, float height,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float thickness = 1.0f, float cornerSize = 0.25f);

    // ���ƶ����
    void drawPolygon(const std::vector<ImVec2>& points,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        float thickness = 1.0f);

    // ����ʵ�Ķ����
    void drawPolygonFilled(const std::vector<ImVec2>& points,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    // ���ƽ�����
    void drawProgressBar(const ImVec2& pos, float width, float height,
        float progress, const ImVec4& bgColor = ImVec4(0.2f, 0.2f, 0.2f, 0.8f),
        const ImVec4& fgColor = ImVec4(0.0f, 0.8f, 0.0f, 1.0f));

    // ����FPS����
    void drawFPS(const ImVec2& position,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    void drawFPS(float x, float y,
        const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    //------------�Զ���Ѫ��/���׻���-------------//
    void DrawHealth(float x, float y, int shield, int max_shield, int armorType, int health);
    void HealthBarSide(Vec2D BoxStart, Vec2D BoxEnd, int health);
    void ShiledBarSide(Vec2D BoxStart, Vec2D BoxEnd, int ShieldHeal, int ShieldHealthMax);
    void HealthBar(Vec4D Head, int health);
    void ShieldBar(Vec4D Head, int ShieldHeal, int ShieldHealthMax);

    void renderGamepadBinding(const char* label, int* buttonPtr);
    void getGamepadButtonName(int buttonCode, char* buffer, size_t bufferSize);

    // �Զ���UIģ�巽��
    bool NavButton(const char* label, bool selected);
    void renderKeyBinding(const char* label, int* keyPtr);
    void getKeyName(int keyCode, char* buffer, size_t bufferSize);
};

// ��ɫת����������
namespace ImColors {
    // RGBA��ɫת��ΪImVec4
    inline ImVec4 RGBA(int r, int g, int b, int a = 255) {
        return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
    }

    // RGB��ɫת��ΪImU32
    inline ImU32 RGB2u32(int r, int g, int b) {
        return IM_COL32(r, g, b, 255);
    }

    // RGBA��ɫת��ΪImU32
    inline ImU32 RGBA32(int r, int g, int b, int a = 255) {
        return IM_COL32(r, g, b, a);
    }

    // Ԥ�������ɫ����
    const ImVec4 White = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 Black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4 Red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4 Green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    const ImVec4 Blue = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    const ImVec4 Yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    const ImVec4 Cyan = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 Magenta = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
    const ImVec4 Orange = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    const ImVec4 Purple = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);
}