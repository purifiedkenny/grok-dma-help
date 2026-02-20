#pragma once
#include <Windows.h>
#include <vector>
#include <imgui.h>
#include "HitBox.h"
#include "Config.h"
#include "Player.h"
#include "VecDef.h"
#include <float.h>

class TriggerBot : public AutoConfigurable {
public:
    TriggerBot();

    // 简化后的边界框触发器配置
    struct {
        bool enable = true;              // 总开关
        int type = 1;                    // 0 = isAimedat check, 1 = 边界框判定
        int delay = 50;                  // 点击延迟(毫秒)
        bool showHitboxes = true;        // 显示判定区域
        bool showPrediction = true;      // 显示预测位置
        ImColor hitboxColor = ImColor(0, 255, 255, 180);  // 边界框颜色
        ImColor hitboxActiveColor = ImColor(255, 0, 0, 180);  // 触发时边界框颜色
        float extraPadding = 0.1f;       // 边界框额外扩展比例
        float closeRangeThreshold = 4.0f; // 近距离阈值
        float lineThickness = 1.0f;      // 线条粗细
    } triggerParams;

    // 检查是否应该触发扳机
    bool ShouldTrigger(Player* target);

    // 渲染触发框
    void RenderHitboxes(Player* currentTarget, bool isTriggering);

    // 更新最后点击时间
    void UpdateLastClickTime();

    // UI渲染
    void renderUI();
    void renderBasicSettings();
    void renderHitboxSettings();
    void renderKeyBindings();
    // 帮助标记函数
    void HelpMarker(const char* desc);
private:
    static DWORD lastClickTime;    // 上次点击时间

    Vec3D GetBoxDimensionsForBone(int boneIndex);

    // 根据骨骼位置计算碰撞箱角点
    std::vector<Vec3D> CalculateBoxCorners(const Vec3D& bonePos, const Vec3D& dimensions);

    // 将3D角点投影到屏幕并检查准星是否在其中
    bool IsPointInScreenBox(const std::vector<Vec3D>& corners);

};