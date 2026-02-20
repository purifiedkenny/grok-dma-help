#include "ImGuiRender.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include "imgui_impl_win32.h"
#include "include.h"
#include <gl/GL.h>

ImGuiRenderer& ImGuiRenderer::getInstance() {
    static ImGuiRenderer instance;
    return instance;
}

ImGuiRenderer::ImGuiRenderer()
    : m_pd3dDevice(nullptr),
    m_fpsUpdateTime(0.0f),
    m_frameCount(0),
    m_currentFps(0.0f),
    m_currentFontName("default") {
    m_lastTime = std::chrono::steady_clock::now();
}

void ImGuiRenderer::newFrame() {
    // ����FPS����
    m_frameCount++;

    // ��ȡ��ǰʱ��
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastTime).count();

    // ÿ��һ��ʱ�����FPSֵ
    if (elapsed >= 1) {
        m_currentFps = static_cast<float>(m_frameCount) / static_cast<float>(elapsed);
        m_lastTime = currentTime;
        m_frameCount = 0;
    }
}

ImFont* ImGuiRenderer::addFont(const std::string& fontName, float fontSize) {
    if (fontName.empty() || fontSize <= 0) {
        return nullptr;
    }

    // ����Ƿ��Ѵ���ͬ������
    if (fontExists(fontName)) {
        return m_fonts[fontName].font;
    }

    // ��������
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("msyhbd.ttc", fontSize, nullptr, io.Fonts->GetGlyphRangesChineseFull());

    if (font) {
        ImFontInfo fontInfo(fontSize);
        fontInfo.font = font;
        fontInfo.loaded = true;
        fontInfo.path = "nullptr";
        m_fonts[fontName] = fontInfo;

        // ������ǵ�һ���Զ������壬��Ϊ��ǰ����
        if (m_currentFontName == "default" && fontName != "default") {
            m_currentFontName = fontName;
        }

        return font;
    }

    return nullptr;
}

ImFont* ImGuiRenderer::addFont(const std::string& fontName, float fontSize, ImFontConfig* fontConfig, const ImWchar* glyphRanges) {
    if (fontName.empty() || fontSize <= 0) {
        return nullptr;
    }

    // ����Ƿ��Ѵ���ͬ������
    if (fontExists(fontName)) {
        return m_fonts[fontName].font;
    }

    // �Ż���������
    ImFontConfig config;
    if (fontConfig) {
        config = *fontConfig;
    }

    // ��������
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = nullptr;

        // ��������
    font = io.Fonts->AddFontFromFileTTF("msyhbd.ttc", fontSize, &config, glyphRanges);

        if (font) {
            ImFontInfo fontInfo(fontSize);
            fontInfo.font = font;
            fontInfo.loaded = true;
            fontInfo.path = "nullptr";
            m_fonts[fontName] = fontInfo;

            // ������ǵ�һ���Զ������壬��Ϊ��ǰ����
            if (m_currentFontName == "default" && fontName != "default") {
                m_currentFontName = fontName;
            }
        }
    

    return font;
}

bool ImGuiRenderer::setCurrentFont(const std::string& fontName) {
    if (fontExists(fontName)) {
        m_currentFontName = fontName;
        return true;
    }
    return false;
}

bool ImGuiRenderer::fontExists(const std::string& fontName) const {
    return m_fonts.find(fontName) != m_fonts.end() && m_fonts.at(fontName).loaded;
}

ImFont* ImGuiRenderer::getFont(const std::string& fontName) {
    return getImGuiFont(fontName);
}

ImFont* ImGuiRenderer::getImGuiFont(const std::string& fontName) const {
    // ���ָ��������������������ڣ���ʹ��ָ������
    if (!fontName.empty() && fontExists(fontName)) {
        return m_fonts.at(fontName).font;
    }

    // ����ʹ�õ�ǰ����
    if (fontExists(m_currentFontName)) {
        return m_fonts.at(m_currentFontName).font;
    }

    // �����ǰ���岻���ڣ���ʹ��Ĭ������
    if (fontExists("default")) {
        return m_fonts.at("default").font;
    }

    // ���û�п������壬����nullptr
    return nullptr;
}

ImDrawList* ImGuiRenderer::getDrawList() const {
    return ImGui::GetBackgroundDrawList();
}

void ImGuiRenderer::drawText(const std::string& text, const ImVec2& position,
    const ImVec4& color, const std::string& fontName) {
    ImDrawList* drawList = getDrawList();
    ImFont* font = getImGuiFont(fontName);
    ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);

    // ֱ����AddText�д����������
    if (font) {
        drawList->AddText(font, font->FontSize, position, colorU32, text.c_str());
    }
    else {
        // ���û���ҵ����壬ʹ��Ĭ�����壨nullptr��
        drawList->AddText(position, colorU32, text.c_str());
    }
}

void ImGuiRenderer::drawText(const std::string& text, const ImVec2& position,
    const ImVec4& color, const float fontSize, const std::string& fontName) {
    ImDrawList* drawList = getDrawList();
    ImFont* font = getImGuiFont(fontName);
    ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);

    // ֱ����AddText�д����������
    if (font) {
        drawList->AddText(font, fontSize ? fontSize : font->FontSize, position, colorU32, text.c_str());
    }
    else {
        // ���û���ҵ����壬ʹ��Ĭ�����壨nullptr��
        drawList->AddText(position, colorU32, text.c_str());
    }
}

void ImGuiRenderer::drawText(const std::string& text, float x, float y,
    const ImVec4& color, const std::string& fontName) {
    drawText(text, ImVec2(x, y), color, fontName);
}

void ImGuiRenderer::drawTextWithShadow(const std::string& text, const ImVec2& position,
    const ImVec4& textColor, const ImVec4& shadowColor,
    float shadowOffset, const std::string& fontName) {
    ImDrawList* drawList = getDrawList();
    ImFont* font = getImGuiFont(fontName);
    ImU32 textColorU32 = ImGui::ColorConvertFloat4ToU32(textColor);
    ImU32 shadowColorU32 = ImGui::ColorConvertFloat4ToU32(shadowColor);

    // �Ȼ�����Ӱ
    if (font) {
        drawList->AddText(font, font->FontSize,
            ImVec2(position.x + shadowOffset, position.y + shadowOffset),
            shadowColorU32, text.c_str());

        // �ٻ����ı�
        drawList->AddText(font, font->FontSize, position, textColorU32, text.c_str());
    }
    else {
        // ʹ��Ĭ������
        drawList->AddText(ImVec2(position.x + shadowOffset, position.y + shadowOffset),
            shadowColorU32, text.c_str());
        drawList->AddText(position, textColorU32, text.c_str());
    }
}

void ImGuiRenderer::drawTextWithShadow(const std::string& text, float x, float y,
    const ImVec4& textColor, const ImVec4& shadowColor,
    float shadowOffset, const std::string& fontName) {
    drawTextWithShadow(text, ImVec2(x, y), textColor, shadowColor, shadowOffset, fontName);
}

ImVec2 ImGuiRenderer::getTextSize(const std::string& text, const std::string& fontName) {
    ImFont* font = getImGuiFont(fontName);

    if (font && text.length() > 0) {
        return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, text.c_str());
    }

    return ImVec2(0.0f, 0.0f);
}

void ImGuiRenderer::drawLine(const ImVec2& p1, const ImVec2& p2,
    const ImVec4& color, float thickness) {
    getDrawList()->AddLine(p1, p2, ImGui::ColorConvertFloat4ToU32(color), thickness);
}

void ImGuiRenderer::drawLine(float x1, float y1, float x2, float y2,
    const ImVec4& color, float thickness) {
    drawLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
}

void ImGuiRenderer::drawRect(const ImVec2& min, const ImVec2& max,
    const ImVec4& color, float thickness, float rounding, ImDrawFlags flags) {
    getDrawList()->AddRect(min, max, ImGui::ColorConvertFloat4ToU32(color),
        rounding, flags, thickness);
}

void ImGuiRenderer::drawRect(float x, float y, float width, float height,
    const ImVec4& color, float thickness, float rounding, ImDrawFlags flags) {
    ImVec2 min1(x, y);
    ImVec2 max1(x + width, y + height);
    drawRect(min1, max1, color, thickness, rounding, flags);
}

void ImGuiRenderer::drawRectFilled(const ImVec2& min, const ImVec2& max,
    const ImVec4& color, float rounding, ImDrawFlags flags) {
    getDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(color),
        rounding, flags);
}

void ImGuiRenderer::drawRectFilled(float x, float y, float width, float height,
    const ImVec4& color, float rounding, ImDrawFlags flags) {
    ImVec2 min1(x, y);
    ImVec2 max1(x + width, y + height);
    drawRectFilled(min1, max1, color, rounding, flags);
}

void ImGuiRenderer::drawCircle(const ImVec2& center, float radius,
    const ImVec4& color, int numSegments, float thickness) {
    getDrawList()->AddCircle(center, radius, ImGui::ColorConvertFloat4ToU32(color),
        numSegments > 0 ? numSegments : 0, thickness);
}

void ImGuiRenderer::drawCircle(float x, float y, float radius,
    const ImVec4& color, int numSegments, float thickness) {
    drawCircle(ImVec2(x, y), radius, color, numSegments, thickness);
}

void ImGuiRenderer::drawCircleFilled(const ImVec2& center, float radius,
    const ImVec4& color, int numSegments) {
    getDrawList()->AddCircleFilled(center, radius, ImGui::ColorConvertFloat4ToU32(color),
        numSegments > 0 ? numSegments : 0);
}

void ImGuiRenderer::drawCircleFilled(float x, float y, float radius,
    const ImVec4& color, int numSegments) {
    drawCircleFilled(ImVec2(x, y), radius, color, numSegments);
}

void ImGuiRenderer::drawCornerBox(const ImVec2& min, const ImVec2& max,
    const ImVec4& color, float thickness, float cornerSize) {
    ImDrawList* drawList = getDrawList();
    ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);

    // ������䳤��
    float width = max.x - min.x;
    float height = max.y - min.y;
    float cornerWidth = width * cornerSize;
    float cornerHeight = height * cornerSize;

    // ���Ͻ�
    drawList->AddLine(
        ImVec2(min.x, min.y),
        ImVec2(min.x + cornerWidth, min.y),
        colorU32, thickness);

    drawList->AddLine(
        ImVec2(min.x, min.y),
        ImVec2(min.x, min.y + cornerHeight),
        colorU32, thickness);

    // ���Ͻ�
    drawList->AddLine(
        ImVec2(max.x - cornerWidth, min.y),
        ImVec2(max.x, min.y),
        colorU32, thickness);

    drawList->AddLine(
        ImVec2(max.x, min.y),
        ImVec2(max.x, min.y + cornerHeight),
        colorU32, thickness);

    // ���½�
    drawList->AddLine(
        ImVec2(min.x, max.y - cornerHeight),
        ImVec2(min.x, max.y),
        colorU32, thickness);

    drawList->AddLine(
        ImVec2(min.x, max.y),
        ImVec2(min.x + cornerWidth, max.y),
        colorU32, thickness);

    // ���½�
    drawList->AddLine(
        ImVec2(max.x - cornerWidth, max.y),
        ImVec2(max.x, max.y),
        colorU32, thickness);

    drawList->AddLine(
        ImVec2(max.x, max.y - cornerHeight),
        ImVec2(max.x, max.y),
        colorU32, thickness);
}

void ImGuiRenderer::drawCornerBox(float x, float y, float width, float height,
    const ImVec4& color, float thickness, float cornerSize) {
    ImVec2 min1(x, y);
    ImVec2 max1(x + width, y + height);
    drawCornerBox(min1, max1, color, thickness, cornerSize);
}

void ImGuiRenderer::drawPolygon(const std::vector<ImVec2>& points,
    const ImVec4& color, float thickness) {
    if (points.size() < 2) return;

    ImDrawList* drawList = getDrawList();
    ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);

    for (size_t i = 0; i < points.size() - 1; i++) {
        drawList->AddLine(points[i], points[i + 1], colorU32, thickness);
    }

    // �պ϶����
    if (points.size() > 2) {
        drawList->AddLine(points.back(), points.front(), colorU32, thickness);
    }
}

void ImGuiRenderer::drawPolygonFilled(const std::vector<ImVec2>& points,
    const ImVec4& color) {
    if (points.size() < 3) return;

    ImDrawList* drawList = getDrawList();
    ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);

    drawList->AddConvexPolyFilled(points.data(), static_cast<int>(points.size()), colorU32);
}

void ImGuiRenderer::drawProgressBar(const ImVec2& pos, float width, float height,
    float progress, const ImVec4& bgColor, const ImVec4& fgColor) {
    progress = std::clamp(progress, 0.0f, 1.0f);

    ImDrawList* drawList = getDrawList();
    ImU32 bgColorU32 = ImGui::ColorConvertFloat4ToU32(bgColor);
    ImU32 fgColorU32 = ImGui::ColorConvertFloat4ToU32(fgColor);

    // ���Ʊ���
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + width, pos.y + height),
        bgColorU32);

    // ���ƽ�����
    if (progress > 0.0f) {
        drawList->AddRectFilled(
            pos,
            ImVec2(pos.x + width * progress, pos.y + height),
            fgColorU32);
    }
}

void ImGuiRenderer::drawFPS(const ImVec2& position, const ImVec4& color) {
    // ��ʽ��FPS�ı�
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << "FPS: Draw | " << m_currentFps << " | Internal" << entityManager.fps;

    // ����FPS�ı�
    drawText(ss.str(), position, color, 0);
}

void ImGuiRenderer::drawFPS(float x, float y, const ImVec4& color) {
    drawFPS(ImVec2(x, y), color);
}

bool ImGuiRenderer::NavButton(const char* label, bool selected) {
    ImGuiStyle& style = ImGui::GetStyle();
    float h = ImGui::GetFrameHeight();

    // ʹ��GetWindowContentRegionMax��GetWindowContentRegionMin�������
    float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;

    auto selectedColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
    auto normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);

    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, selectedColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selectedColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, selectedColor);
    }

    bool clicked = ImGui::Button(label, ImVec2(width * 0.9f, h * 1.5f));

    if (selected) {
        ImGui::PopStyleColor(3);
    }

    return clicked;
}

// ������UI��Ⱦ����
void ImGuiRenderer::renderKeyBinding(const char* label, int* keyPtr) {
    ImGui::PushID(label);

    // ��ʾ��ǰ�󶨵İ���
    char keyName[64] = "None";
    if (*keyPtr > 0) {
        // ��ȡ��������
        getKeyName(*keyPtr, keyName, sizeof(keyName));
    }

    // �̶����ȵİ�ť��ʽ
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));

    // ��ť����ʾborder��background��ֻ��ʾ�ı�
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.30f, 0.30f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));

    ImGui::PushItemWidth(100);

    // ������ť
    if (ImGui::Button(keyName, ImVec2(100, 0))) {
        ImGui::OpenPopup("BindKey");
    }

    ImGui::PopItemWidth();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // ������ǩ
    ImGui::SameLine(120);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);

    // �����󶨵�������
    if (ImGui::BeginPopup("BindKey")) {
        ImGui::Text("Press any key to bind...");
        ImGui::Text("ESC to cancel");

        // ��ⰴ������
        for (int i = 1; i < 256; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                if (i == VK_ESCAPE) {
                    // ȡ����
                    *keyPtr = 0;
                    ImGui::CloseCurrentPopup();
                }
                else {
                    // �����µİ�����
                    *keyPtr = i;
                    // ����KeyDetector
                    ImGui::CloseCurrentPopup();
                }
                break;
            }
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void ImGuiRenderer::getKeyName(int keyCode, char* buffer, size_t bufferSize) {
    // ����������ӳ��
    switch (keyCode) {
    case VK_LBUTTON: strcpy_s(buffer, bufferSize, "Mouse1"); break;
    case VK_RBUTTON: strcpy_s(buffer, bufferSize, "Mouse2"); break;
    case VK_MBUTTON: strcpy_s(buffer, bufferSize, "Mouse3"); break;
    case VK_XBUTTON1: strcpy_s(buffer, bufferSize, "Mouse4"); break;
    case VK_XBUTTON2: strcpy_s(buffer, bufferSize, "Mouse5"); break;
    case VK_LSHIFT: strcpy_s(buffer, bufferSize, "L-Shift"); break;
    case VK_RSHIFT: strcpy_s(buffer, bufferSize, "R-Shift"); break;
    case VK_LCONTROL: strcpy_s(buffer, bufferSize, "L-Ctrl"); break;
    case VK_RCONTROL: strcpy_s(buffer, bufferSize, "R-Ctrl"); break;
    case VK_LMENU: strcpy_s(buffer, bufferSize, "L-Alt"); break;
    case VK_RMENU: strcpy_s(buffer, bufferSize, "R-Alt"); break;
    case VK_SPACE: strcpy_s(buffer, bufferSize, "Space"); break;
    case VK_RETURN: strcpy_s(buffer, bufferSize, "Enter"); break;
    case VK_BACK: strcpy_s(buffer, bufferSize, "Backspace"); break;
    case VK_TAB: strcpy_s(buffer, bufferSize, "Tab"); break;
    case VK_ESCAPE: strcpy_s(buffer, bufferSize, "Escape"); break;

        // �������³��ü�λ
    case VK_INSERT: strcpy_s(buffer, bufferSize, "Insert"); break;
    case VK_DELETE: strcpy_s(buffer, bufferSize, "Delete"); break;
    case VK_HOME: strcpy_s(buffer, bufferSize, "Home"); break;
    case VK_END: strcpy_s(buffer, bufferSize, "End"); break;
    case VK_PRIOR: strcpy_s(buffer, bufferSize, "Page Up"); break;
    case VK_NEXT: strcpy_s(buffer, bufferSize, "Page Down"); break;
    case VK_SNAPSHOT: strcpy_s(buffer, bufferSize, "Print Screen"); break;
    case VK_SCROLL: strcpy_s(buffer, bufferSize, "Scroll Lock"); break;
    case VK_PAUSE: strcpy_s(buffer, bufferSize, "Pause"); break;
    case VK_CAPITAL: strcpy_s(buffer, bufferSize, "Caps Lock"); break;
    case VK_NUMLOCK: strcpy_s(buffer, bufferSize, "Num Lock"); break;

        // �����
    case VK_UP: strcpy_s(buffer, bufferSize, "Up"); break;
    case VK_DOWN: strcpy_s(buffer, bufferSize, "Down"); break;
    case VK_LEFT: strcpy_s(buffer, bufferSize, "Left"); break;
    case VK_RIGHT: strcpy_s(buffer, bufferSize, "Right"); break;

        // С���̰���
    case VK_NUMPAD0: strcpy_s(buffer, bufferSize, "Num 0"); break;
    case VK_NUMPAD1: strcpy_s(buffer, bufferSize, "Num 1"); break;
    case VK_NUMPAD2: strcpy_s(buffer, bufferSize, "Num 2"); break;
    case VK_NUMPAD3: strcpy_s(buffer, bufferSize, "Num 3"); break;
    case VK_NUMPAD4: strcpy_s(buffer, bufferSize, "Num 4"); break;
    case VK_NUMPAD5: strcpy_s(buffer, bufferSize, "Num 5"); break;
    case VK_NUMPAD6: strcpy_s(buffer, bufferSize, "Num 6"); break;
    case VK_NUMPAD7: strcpy_s(buffer, bufferSize, "Num 7"); break;
    case VK_NUMPAD8: strcpy_s(buffer, bufferSize, "Num 8"); break;
    case VK_NUMPAD9: strcpy_s(buffer, bufferSize, "Num 9"); break;
    case VK_MULTIPLY: strcpy_s(buffer, bufferSize, "Num *"); break;
    case VK_ADD: strcpy_s(buffer, bufferSize, "Num +"); break;
    case VK_SUBTRACT: strcpy_s(buffer, bufferSize, "Num -"); break;
    case VK_DECIMAL: strcpy_s(buffer, bufferSize, "Num ."); break;
    case VK_DIVIDE: strcpy_s(buffer, bufferSize, "Num /"); break;

        // ������ż�
    case VK_OEM_1: strcpy_s(buffer, bufferSize, ";"); break;        // ;:
    case VK_OEM_PLUS: strcpy_s(buffer, bufferSize, "="); break;     // =+
    case VK_OEM_COMMA: strcpy_s(buffer, bufferSize, ","); break;    // ,
    case VK_OEM_MINUS: strcpy_s(buffer, bufferSize, "-"); break;    // -_
    case VK_OEM_PERIOD: strcpy_s(buffer, bufferSize, "."); break;   // .>
    case VK_OEM_2: strcpy_s(buffer, bufferSize, "/"); break;        // /?
    case VK_OEM_3: strcpy_s(buffer, bufferSize, "`"); break;        // `~
    case VK_OEM_4: strcpy_s(buffer, bufferSize, "["); break;        // [{
    case VK_OEM_5: strcpy_s(buffer, bufferSize, "\\"); break;       // \|
    case VK_OEM_6: strcpy_s(buffer, bufferSize, "]"); break;        // ]}
    case VK_OEM_7: strcpy_s(buffer, bufferSize, "'"); break;        // '"

    default:
        // ������ĸ�����ֺ͹��ܼ�����������
        if (keyCode >= 'A' && keyCode <= 'Z') {
            sprintf_s(buffer, bufferSize, "%c", keyCode);
        }
        else if (keyCode >= '0' && keyCode <= '9') {
            sprintf_s(buffer, bufferSize, "%c", keyCode);
        }
        else if (keyCode >= VK_F1 && keyCode <= VK_F24) {
            sprintf_s(buffer, bufferSize, "F%d", keyCode - VK_F1 + 1);
        }
        else {
            sprintf_s(buffer, bufferSize, "Key %d", keyCode);
        }
        break;
    }
}

void ImGuiRenderer::renderGamepadBinding(const char* label, int* buttonPtr) {
    ImGui::PushID(label);

    // ��ʾ��ǰ�󶨵İ���
    char buttonName[64] = "";
    if (*buttonPtr > 0) {
        // ��ȡ��������
        getGamepadButtonName(*buttonPtr, buttonName, sizeof(buttonName));
    }
    else {
        strcpy_s(buttonName, sizeof(buttonName), t_("None"));
    }

    // ������ʽ
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.30f, 0.30f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));

    ImGui::PushItemWidth(100);

    // ���������˵�
    if (ImGui::BeginCombo("##gamepadButton", buttonName)) {
        // ����Ҫ��ʾ���ֱ�����
        struct GamepadButton {
            const char* translationKey;
            int code;
        };

        GamepadButton buttons[] = {
            {"None", 0},
            {"Square Button", 117},
            {"Triangle Button", 118},
            {"Circle Button", 116},
            {"Cross Button", 115},
            {"Left Shoulder", 119},
            {"Right Shoulder", 120},
            {"Map Button", 121},
            {"Settings Button", 122},
            {"Left Trigger", 126},
            {"Right Trigger", 127},
            {"D-Pad Up", 243},
            {"D-Pad Left", 246},
            {"D-Pad Right", 244},
            {"D-Pad Down", 245}
        };

        for (const GamepadButton& button : buttons) {
            const bool is_selected = (*buttonPtr == button.code);
            if (ImGui::Selectable(t_(button.translationKey), is_selected)) {
                *buttonPtr = button.code;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // ������ǩ
    ImGui::SameLine(120);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);

    ImGui::PopID();
}

// ������������ȡ�ֱ��������ƣ�ʹ�÷���ϵͳ��
void ImGuiRenderer::getGamepadButtonName(int buttonCode, char* buffer, size_t bufferSize) {
    switch (buttonCode) {
    case 0:
        strcpy_s(buffer, bufferSize, t_("None"));
        break;
    case 115:
        strcpy_s(buffer, bufferSize, t_("Cross Button"));
        break;
    case 116:
        strcpy_s(buffer, bufferSize, t_("Circle Button"));
        break;
    case 117:
        strcpy_s(buffer, bufferSize, t_("Square Button"));
        break;
    case 118:
        strcpy_s(buffer, bufferSize, t_("Triangle Button"));
        break;
    case 119:
        strcpy_s(buffer, bufferSize, t_("Left Shoulder"));
        break;
    case 120:
        strcpy_s(buffer, bufferSize, t_("Right Shoulder"));
        break;
    case 121:
        strcpy_s(buffer, bufferSize, t_("Map Button"));
        break;
    case 122:
        strcpy_s(buffer, bufferSize, t_("Settings Button"));
        break;
    case 126:
        strcpy_s(buffer, bufferSize, t_("Left Trigger"));
        break;
    case 127:
        strcpy_s(buffer, bufferSize, t_("Right Trigger"));
        break;
    case 243:
        strcpy_s(buffer, bufferSize, t_("D-Pad Up"));
        break;
    case 246:
        strcpy_s(buffer, bufferSize, t_("D-Pad Left"));
        break;
    case 244:
        strcpy_s(buffer, bufferSize, t_("D-Pad Right"));
        break;
    case 245:
        strcpy_s(buffer, bufferSize, t_("D-Pad Down"));
        break;
    default:
        sprintf_s(buffer, bufferSize, "Button %d", buttonCode);
        break;
    }
}

// ����ΪѪ��/������Ⱦ��������Щ���ֲ���

void ImGuiRenderer::DrawHealth(float x, float y, int shield, int max_shield, int armorType, int health) {

    auto drawlist = ImGui::GetBackgroundDrawList();
        int bg_offset = 3;
        int bar_width = 105; //158
        // 4steps...2*3=6
        // 38*4=152 152+6 = 158
        // 5steps...2*4=8
        // 30*5=150 150+8 = 158
        float max_health = 100.0f;//100
        float shield_step = 25.0f; //25

        float shield_25 = 19.4f; //30
        int steps = 5;


        ImVec2 bg1(x - bar_width / 2 - bg_offset, y);
        ImVec2 bg2(bg1.x - 10, bg1.y - 16);
        ImVec2 bg3(bg2.x + 5, bg2.y - 7);
        ImVec2 bg4(bg3.x + bar_width + bg_offset, bg3.y);
        ImVec2 bg5(bg4.x + 11, bg4.y + 18);
        ImVec2 bg6(x + bar_width / 2 + bg_offset, y);
        drawlist->AddHexagonFilled(bg1, bg2, bg3, bg4, bg5, bg6, ImColor(0, 0, 0, 120));


        ImVec2 h1(bg1.x + 3, bg1.y - 4);
        ImVec2 h2(h1.x - 5, h1.y - 8);
        ImVec2 h3(h2.x + (float)health / max_health * bar_width, h2.y);
        ImVec2 h4(h1.x + (float)health / max_health * bar_width, h1.y);
        ImVec2 h3m(h2.x + bar_width, h2.y);
        ImVec2 h4m(h1.x + bar_width, h1.y);
        drawlist->AddQuadFilled(h1, h2, h3m, h4m, ImColor(10, 10, 30, 60));
        drawlist->AddQuadFilled(h1, h2, h3, h4, ImColor(255,255,255,255));


        ImColor shieldCracked(97, 97, 97);
        ImColor shieldCrackedDark(67, 67, 67);

        ImColor shieldCol;
        ImColor shieldColDark; //not used, but the real seer q has shadow inside
        if (max_shield == 50) { //white
            shieldCol = ImColor(247, 247, 247);
            shieldColDark = ImColor(164, 164, 164);
        }
        else if (max_shield == 75) { //blue
            shieldCol = ImColor(39, 178, 255);
            shieldColDark = ImColor(27, 120, 210);
        }
        else if (max_shield == 100) { //purple
            shieldCol = ImColor(206, 59, 255);
            shieldColDark = ImColor(136, 36, 220);
        }
        else if (max_shield == 100) { //gold
            shieldCol = ImColor(255, 255, 79);
            shieldColDark = ImColor(218, 175, 49);
        }
        else if (max_shield == 125) { //red
            shieldCol = ImColor(219, 2, 2);
            shieldColDark = ImColor(219, 2, 2);
        }
        else {
            shieldCol = ImColor(247, 247, 247);
            shieldColDark = ImColor(164, 164, 164);
        }
        int shield_tmp = shield;
        int shield1 = 0;
        int shield2 = 0;
        int shield3 = 0;
        int shield4 = 0;
        int shield5 = 0;
        if (shield_tmp > 25) {
            shield1 = 25;
            shield_tmp -= 25;
            if (shield_tmp > 25) {
                shield2 = 25;
                shield_tmp -= 25;
                if (shield_tmp > 25) {
                    shield3 = 25;
                    shield_tmp -= 25;
                    if (shield_tmp > 25) {
                        shield4 = 25;
                        shield_tmp -= 25;
                        shield5 = shield_tmp;
                    }
                    else {
                        shield4 = shield_tmp;
                    }
                }
                else {
                    shield3 = shield_tmp;
                }
            }
            else {
                shield2 = shield_tmp;
            }
        }
        else {
            shield1 = shield_tmp;
        }
        ImVec2 s1(h2.x - 1, h2.y - 2);
        ImVec2 s2(s1.x - 3, s1.y - 5);
        ImVec2 s3(s2.x + shield1 / shield_step * shield_25, s2.y);
        ImVec2 s4(s1.x + shield1 / shield_step * shield_25, s1.y);
        ImVec2 s3m(s2.x + shield_25, s2.y);
        ImVec2 s4m(s1.x + shield_25, s1.y);

        ImVec2 ss1(s4m.x + 2, s1.y);
        ImVec2 ss2(s3m.x + 2, s2.y);
        ImVec2 ss3(ss2.x + shield2 / shield_step * shield_25, s2.y);
        ImVec2 ss4(ss1.x + shield2 / shield_step * shield_25, s1.y);
        ImVec2 ss3m(ss2.x + shield_25, s2.y);
        ImVec2 ss4m(ss1.x + shield_25, s1.y);

        ImVec2 sss1(ss4m.x + 2, s1.y);
        ImVec2 sss2(ss3m.x + 2, s2.y);
        ImVec2 sss3(sss2.x + shield3 / shield_step * shield_25, s2.y);
        ImVec2 sss4(sss1.x + shield3 / shield_step * shield_25, s1.y);
        ImVec2 sss3m(sss2.x + shield_25, s2.y);
        ImVec2 sss4m(sss1.x + shield_25, s1.y);

        ImVec2 ssss1(sss4m.x + 2, s1.y);
        ImVec2 ssss2(sss3m.x + 2, s2.y);
        ImVec2 ssss3(ssss2.x + shield4 / shield_step * shield_25, s2.y);
        ImVec2 ssss4(ssss1.x + shield4 / shield_step * shield_25, s1.y);
        ImVec2 ssss3m(ssss2.x + shield_25, s2.y);
        ImVec2 ssss4m(ssss1.x + shield_25, s1.y);

        ImVec2 sssss1(ssss4m.x + 2, s1.y);
        ImVec2 sssss2(ssss3m.x + 2, s2.y);
        ImVec2 sssss3(sssss2.x + shield5 / shield_step * shield_25, s2.y);
        ImVec2 sssss4(sssss1.x + shield5 / shield_step * shield_25, s1.y);
        ImVec2 sssss3m(sssss2.x + shield_25, s2.y);
        ImVec2 sssss4m(sssss1.x + shield_25, s1.y);
        if (max_shield == 50) {
            if (shield <= 25) {
                if (shield < 25) {
                    drawlist->AddQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);

            }
            else if (shield <= 50) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                if (shield != 50) {
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            }
        }
        else if (max_shield == 75) {
            if (shield <= 25) {
                if (shield < 25) {
                    drawlist->AddQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);

            }
            else if (shield <= 50) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                if (shield < 50) {
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            }
            else if (shield <= 75) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
                if (shield < 75) {
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            }
        }
        else if (max_shield == 100) {
            if (shield <= 25) {
                if (shield < 25) {
                    drawlist->AddQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);

            }
            else if (shield <= 50) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                if (shield < 50) {
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            }
            else if (shield <= 75) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
                if (shield < 75) {
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            }
            else if (shield <= 100) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
                drawlist->AddQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
                if (shield < 100) {
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
            }
        }
        else if (max_shield == 125) {
            if (shield <= 25) {
                if (shield < 25) {
                    drawlist->AddQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                    drawlist->AddQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);

            }
            else if (shield <= 50) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                if (shield < 50) {
                    drawlist->AddQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                    drawlist->AddQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            }
            else if (shield <= 75) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
                if (shield < 75) {
                    drawlist->AddQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                    drawlist->AddQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            }
            else if (shield <= 100) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
                drawlist->AddQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
                if (shield < 100) {
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                    drawlist->AddQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
            }
            else if (shield <= 125) {
                drawlist->AddQuadFilled(s1, s2, s3, s4, shieldCol);
                drawlist->AddQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
                drawlist->AddQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
                drawlist->AddQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
                if (shield < 125) {
                    drawlist->AddQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
                }
                if (shield != 0)
                    drawlist->AddQuadFilled(sssss1, sssss2, sssss3, sssss4, shieldCol);
            }
        }
    }

void ImGuiRenderer::HealthBar(Vec4D Head, int health) {
    ImVec2 p1(Head.x - Head.w * 2, Head.y - 12);
    ImVec2 p2(Head.x - Head.w * 2, Head.y - 16);

    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Ĭ����ɫ (��ɫ)

    if (health > 100) {
        health = 100;
    }

    // ���ݽ���ֵ������ɫ
    if (health >= 90) {
        color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);      // ��ɫ
    }
    else if (health >= 80) {
        color = ImVec4(0.0f, 0.8f, 0.8f, 1.0f);      // ����
    }
    else if (health >= 70) {
        color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);      // ����
    }
    else if (health >= 60) {
        color = ImVec4(0.8f, 1.0f, 0.0f, 1.0f);      // �ۻ�
    }
    else if (health >= 40) {
        color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);      // ��ɫ
    }
    else if (health >= 5) {
        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);      // ��ɫ
    }

    // �����ⲿ��ɫ���α���
    ImGui::GetBackgroundDrawList()->AddRectFilled(p1, p2, ImColor(0, 0, 0, 100));

    // ���ݽ���ֵ��̬����Ѫ���Ŀ���
    float healthWidth = ((Head.w * 5) * health) / 100.0f; // Ѫ����ǰ����
    float MaxhealthWidth = Head.w * 5; // ��ѪѪ������
    ImVec2 healthBarTopRight(p1.x + healthWidth, p2.y); // Ѫ���ұߵĵ㣬��̬����
    ImVec2 MaxhealthBarTopRight(p1.x + MaxhealthWidth, p2.y); // Ѫ���ұߵĵ㣬��̬����

    // �����ڲ�������
    ImVec4 segmentColor = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
    ImGui::GetBackgroundDrawList()->AddRectFilled(p1, MaxhealthBarTopRight, ImColor(segmentColor));
    ImGui::GetBackgroundDrawList()->AddRectFilled(p1, healthBarTopRight, ImColor(color));
}

void ImGuiRenderer::ShieldBar(Vec4D Head, int ShieldHeal, int ShieldHealthMax) {
    ImVec2 p1(Head.x - Head.w * 2, Head.y - 18);
    ImVec2 p2(Head.x - Head.w * 2, Head.y - 22);

    int numSegments = ShieldHealthMax / 25; // �ܹ��ĸ�����������ShieldHealthMax��̬����
    float gapWidth = (Head.w * 5) / 20; // ÿ������֮��ļ������
    float totalGapWidth = gapWidth * (numSegments - 1); // �ܼ������
    float segmentWidth = ((Head.w * 5) - totalGapWidth) / numSegments; // ÿ�����ӵĿ��ȣ����Ǽ��

    ImVec4 color; // ���� ShieldHealthMax ������ɫ

    // ���� ShieldHealthMax ������ɫ
    if (ShieldHealthMax == 50) {
        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // ��ɫ
    }
    else if (ShieldHealthMax == 75) {
        color = ImColor(0, 161, 255, 255); // ��ɫ
    }
    else if (ShieldHealthMax == 100) {
        color = ImColor(178, 58, 247, 255); // ��ɫ
    }
    else if (ShieldHealthMax == 125) {
        color = ImVec4(2.0f, 0.0f, 0.0f, 1.0f); // ��ɫ
    }
    else {
        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Ĭ����ɫ (��ɫ)
    }

    // ��֤����ֵ������ ShieldHealthMax
    if (ShieldHeal > ShieldHealthMax) {
        ShieldHeal = ShieldHealthMax;
    }

    // �����ⲿ��ɫ���α���
    ImGui::GetBackgroundDrawList()->AddRectFilled(p1, p2, ImColor(0, 0, 0, 100));

    // ���㲢����ÿ������
    for (int i = 0; i < numSegments; ++i) {
        ImVec2 segmentTopLeft(p1.x + i * (segmentWidth + gapWidth), p1.y);
        ImVec2 segmentBottomRight(segmentTopLeft.x + segmentWidth, p2.y);

        // �����ǰ���Ӵ����Ļ���ֵ��ShieldHeal�ڣ��ͻ�����ɫ��������ƻ�ɫ
        ImVec4 segmentColor = ((i * 25) < ShieldHeal) ? color : ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
        ImGui::GetBackgroundDrawList()->AddRectFilled(segmentTopLeft, segmentBottomRight, ImColor(segmentColor));
    }
}

void ImGuiRenderer::HealthBarSide(Vec2D BoxStart, Vec2D BoxEnd, int health) {
    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Ĭ����ɫ (��ɫ)

    if (health > 100) {
        health = 100;
    }
    if (health >= 90) {
        color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);      // ��ɫ
    }
    else if (health >= 80) {
        color = ImVec4(0.0f, 0.8f, 0.8f, 1.0f);      // ����
    }
    else if (health >= 70) {
        color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);      // ����
    }
    else if (health >= 60) {
        color = ImVec4(0.8f, 1.0f, 0.0f, 1.0f);      // �ۻ�
    }
    else if (health >= 40) {
        color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);      // ��ɫ
    }
    else if (health >= 5) {
        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);      // ��ɫ
    }

    float width = BoxEnd.y - BoxStart.y;
    static float gap = 5.f;
    // ���ݽ���ֵ��̬����Ѫ���Ŀ���
    float scale = width / 100.f;
    float healthHeight = scale * health; // Ѫ����ǰ����
    ImVec2 p1(BoxStart.x - gap, BoxEnd.y);

    ImVec2 PointBottom(p1.x - gap, p1.y);
    ImVec2 PointToUP(p1.x, p1.y - healthHeight);

    ImGui::GetBackgroundDrawList()->AddRectFilled(PointBottom, ImVec2(p1.x + gap, BoxStart.y), ImColor(20, 20, 20, 100));
    ImGui::GetBackgroundDrawList()->AddRectFilled(PointBottom, PointToUP, ImColor(color));
}

void ImGuiRenderer::ShiledBarSide(Vec2D BoxStart, Vec2D BoxEnd, int ShieldHeal, int ShieldHealthMax) {
    ImVec4 color; // ���� ShieldHealthMax ������ɫ

    if (ShieldHealthMax == 50) {
        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // ��ɫ
    }
    else if (ShieldHealthMax == 75) {
        color = ImColor(0, 161, 255, 255); // ��ɫ
    }
    else if (ShieldHealthMax == 100) {
        color = ImColor(178, 58, 247, 255); // ��ɫ
    }
    else if (ShieldHealthMax == 125) {
        color = ImVec4(2.0f, 0.0f, 0.0f, 1.0f); // ��ɫ
    }
    else {
        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Ĭ����ɫ (��ɫ)
    }

    if (ShieldHeal > ShieldHealthMax) {
        ShieldHeal = ShieldHealthMax;
    }

    float width = BoxEnd.y - BoxStart.y;
    static float gap = 5.f;

    float scale = width / ShieldHealthMax;
    float healthHeight = scale * ShieldHeal;
    ImVec2 p1(BoxStart.x + gap, BoxEnd.y);

    ImVec2 PointBottom(p1.x + gap, p1.y);
    ImVec2 PointToUP(p1.x, p1.y - healthHeight);

    ImGui::GetBackgroundDrawList()->AddRectFilled(PointBottom, ImVec2(p1.x + gap, BoxStart.y), ImColor(20, 20, 20, 100));
    ImGui::GetBackgroundDrawList()->AddRectFilled(PointBottom, PointToUP, ImColor(color));
}