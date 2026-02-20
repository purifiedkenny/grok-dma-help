#include "TriggerBot.h"
#include "include.h"
#include <algorithm>
#include <chrono>
#include "Skeleton.h"

// 静态变量初始化
DWORD TriggerBot::lastClickTime = 0;

TriggerBot::TriggerBot() : AutoConfigurable("TriggerBot") {
    // 初始化触发器配置参数
    triggerParams.enable = true;
    triggerParams.type = 1;  // 默认使用边界框判定
    triggerParams.delay = 50;
    triggerParams.showHitboxes = true;
    triggerParams.showPrediction = true;
    triggerParams.hitboxColor = ImColor(0, 255, 255, 180);
    triggerParams.hitboxActiveColor = ImColor(255, 0, 0, 180);
    triggerParams.extraPadding = 0.1f;
    triggerParams.closeRangeThreshold = 4.0f;
    triggerParams.lineThickness = 1.0f;

    // 注册配置参数
    registerParam("Enable", triggerParams.enable);
    registerParam("Type", triggerParams.type);
    registerParam("Delay", triggerParams.delay);
    registerParam("ShowHitboxes", triggerParams.showHitboxes);
    registerParam("ShowPrediction", triggerParams.showPrediction);
    registerParam("HitboxColorR", triggerParams.hitboxColor.Value.x);
    registerParam("HitboxColorG", triggerParams.hitboxColor.Value.y);
    registerParam("HitboxColorB", triggerParams.hitboxColor.Value.z);
    registerParam("HitboxColorA", triggerParams.hitboxColor.Value.w);
    registerParam("HitboxActiveColorR", triggerParams.hitboxActiveColor.Value.x);
    registerParam("HitboxActiveColorG", triggerParams.hitboxActiveColor.Value.y);
    registerParam("HitboxActiveColorB", triggerParams.hitboxActiveColor.Value.z);
    registerParam("HitboxActiveColorA", triggerParams.hitboxActiveColor.Value.w);
    registerParam("ExtraPadding", triggerParams.extraPadding);
    registerParam("CloseRangeThreshold", triggerParams.closeRangeThreshold);
    registerParam("LineThickness", triggerParams.lineThickness);
}

bool TriggerBot::ShouldTrigger(Player* target) {
    // 基础验证
    if (!target || !target->IsValid() || !localPlayer || !localPlayer->IsValid()) {
        return false;
    }

    // 延迟检查
    if (GetTickCount64() - lastClickTime < triggerParams.delay) {
        return false;
    }

    // 类型0: 仅检查isAimedAt
    if (triggerParams.type == 0) {
        return target->IsAimedAt;
    }

    // 类型1: 使用骨骼系统判定
    std::vector<int> bones = { 0, 1, 2, 3 }; // 检查多个骨骼点: 头、颈、上胸、下胸

    for (int boneIndex : bones) {
        // 获取骨骼位置 - 优先使用骨骼系统
        Vec3D bonePosition;
        bool bonePositionValid = false;

        // 首先尝试从skeleton系统获取骨骼位置
        if (skeletonSystem) {
            bonePositionValid = skeletonSystem->GetBestBonePositionForAiming(
                target, bonePosition, false, boneIndex);
        }

        // 如果骨骼系统未获取到位置，使用原始方法
        if (!bonePositionValid) {
            HitboxType hitboxType = static_cast<HitboxType>(boneIndex);
            bonePosition = aimbot->GetBonePosition(target, hitboxType);

            // 如果位置仍然无效，跳过当前骨骼
            if (bonePosition.empty()) {
                continue;
            }
        }

        // 应用预测
        if (triggerParams.showPrediction) {
            bonePosition = aimbot->CalculatePredictedPosition(
                bonePosition,
                target->AbsoluteVelocity,
                localPlayer->WeaponProjectileSpeed,
                localPlayer->WeaponProjectileScale
            );
        }

        // 获取适合该骨骼的碰撞箱尺寸
        Vec3D boxDimensions = GetBoxDimensionsForBone(boneIndex);

        // 添加额外的padding
        boxDimensions.x *= (1.0f + triggerParams.extraPadding);
        boxDimensions.y *= (1.0f + triggerParams.extraPadding);
        boxDimensions.z *= (1.0f + triggerParams.extraPadding);

        // 计算碰撞箱角点
        std::vector<Vec3D> corners = CalculateBoxCorners(bonePosition, boxDimensions);

        // 计算屏幕上的边界框
        float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;
        bool anyPointVisible = false;

        for (const auto& corner : corners) {
            Vec2D screenPos;
            if (gameView->WorldToScreen(corner, screenPos)) {
                anyPointVisible = true;
                minX = std::min(minX, screenPos.x);
                maxX = std::max(maxX, screenPos.x);
                minY = std::min(minY, screenPos.y);
                maxY = std::max(maxY, screenPos.y);
            }
        }

        // 如果没有角点可见，继续检查下一个骨骼
        if (!anyPointVisible) continue;

        // 近距离目标特殊处理
        float distance = target->DistanceToLocalPlayer;
        if (distance <= triggerParams.closeRangeThreshold) {
            // 近距离扩大碰撞箱
            float expansionFactor = 1.0f + (triggerParams.closeRangeThreshold - distance) / triggerParams.closeRangeThreshold;
            float width = maxX - minX;
            float height = maxY - minY;
            float centerX = (minX + maxX) * 0.5f;
            float centerY = (minY + maxY) * 0.5f;

            minX = centerX - width * 0.5f * expansionFactor;
            maxX = centerX + width * 0.5f * expansionFactor;
            minY = centerY - height * 0.5f * expansionFactor;
            maxY = centerY + height * 0.5f * expansionFactor;

            // 近距离时，如果被瞄准直接触发
            if (target->IsAimedAt) {
                return true;
            }
        }

        // 获取屏幕中心点(准星位置)
        const Vec2D& screenCenter = gameView->ScreenCenter;

        // 检查准星是否在边界框内
        if (screenCenter.x >= minX && screenCenter.x <= maxX &&
            screenCenter.y >= minY && screenCenter.y <= maxY) {
            return true;
        }
    }

    return false;
}

Vec3D TriggerBot::GetBoxDimensionsForBone(int boneIndex) {
    switch (boneIndex) {
    case 0: // 头部
        return { 5.0f, 5.0f, 5.0f };
    case 1: // 颈部
        return { 4.0f, 4.0f, 4.0f };
    case 2: // 上胸
        return { 7.0f, 7.0f, 10.0f };
    case 3: // 下胸/腹部
        return { 8.0f, 8.0f, 12.0f };
    default:
        return { 6.0f, 6.0f, 8.0f };
    }
}

std::vector<Vec3D> TriggerBot::CalculateBoxCorners(const Vec3D& bonePos, const Vec3D& dimensions) {
    return {
        // 前面四个角点
        {bonePos.x + dimensions.x, bonePos.y + dimensions.y, bonePos.z + dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y + dimensions.y, bonePos.z + dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y - dimensions.y, bonePos.z + dimensions.z},
        {bonePos.x + dimensions.x, bonePos.y - dimensions.y, bonePos.z + dimensions.z},
        // 后面四个角点
        {bonePos.x + dimensions.x, bonePos.y + dimensions.y, bonePos.z - dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y + dimensions.y, bonePos.z - dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y - dimensions.y, bonePos.z - dimensions.z},
        {bonePos.x + dimensions.x, bonePos.y - dimensions.y, bonePos.z - dimensions.z}
    };
}

void TriggerBot::RenderHitboxes(Player* currentTarget, bool isTriggering) {
    if (!triggerParams.showHitboxes || !localPlayer || !localPlayer->IsValid()) {
        return;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    const auto& players = entityManager.getPlayers();
    for (auto player : *players) {
        if (!aimbot->IsValidTarget(player)) {
            continue;
        }

        // 检查多个骨骼点
        std::vector<int> bones = { 0, 1, 2, 3 };

        for (int boneIndex : bones) {
            // 获取骨骼位置 - 优先使用骨骼系统
            Vec3D bonePosition;
            bool bonePositionValid = false;

            // 尝试从skeleton系统获取骨骼位置
            if (skeletonSystem) {
                bonePositionValid = skeletonSystem->GetBestBonePositionForAiming(
                    player, bonePosition, false, boneIndex);
            }

            // 如果未获取到，使用原始方法
            if (!bonePositionValid) {
                HitboxType hitboxType = static_cast<HitboxType>(boneIndex);
                bonePosition = aimbot->GetBonePosition(player, hitboxType);

                // 如果位置无效，跳过当前骨骼
                if (bonePosition.empty()) {
                    continue;
                }
            }

            // 应用预测
            if (triggerParams.showPrediction) {
                bonePosition = aimbot->CalculatePredictedPosition(
                    bonePosition,
                    player->AbsoluteVelocity,
                    localPlayer->WeaponProjectileSpeed,
                    localPlayer->WeaponProjectileScale
                );
            }

            // 获取碰撞箱尺寸
            Vec3D boxDimensions = GetBoxDimensionsForBone(boneIndex);

            // 添加额外的padding
            boxDimensions.x *= (1.0f + triggerParams.extraPadding);
            boxDimensions.y *= (1.0f + triggerParams.extraPadding);
            boxDimensions.z *= (1.0f + triggerParams.extraPadding);

            // 计算碰撞箱角点
            std::vector<Vec3D> corners = CalculateBoxCorners(bonePosition, boxDimensions);

            // 将3D角点转换为2D屏幕坐标
            std::vector<Vec2D> screenCorners;
            for (const auto& corner : corners) {
                Vec2D screenPos;
                if (gameView->WorldToScreen(corner, screenPos)) {
                    screenCorners.push_back(screenPos);
                }
            }

            // 选择颜色 - 当前目标使用高亮色
            ImColor boxColor = (player == currentTarget && isTriggering)
                ? triggerParams.hitboxActiveColor
                : triggerParams.hitboxColor;

            // 绘制边界框
            if (screenCorners.size() >= 8) {
                // 底部矩形
                for (int i = 0; i < 4; i++) {
                    drawList->AddLine(
                        ImVec2(screenCorners[i].x, screenCorners[i].y),
                        ImVec2(screenCorners[(i + 1) % 4].x, screenCorners[(i + 1) % 4].y),
                        boxColor, triggerParams.lineThickness
                    );
                }

                // 顶部矩形
                for (int i = 0; i < 4; i++) {
                    drawList->AddLine(
                        ImVec2(screenCorners[i + 4].x, screenCorners[i + 4].y),
                        ImVec2(screenCorners[((i + 1) % 4) + 4].x, screenCorners[((i + 1) % 4) + 4].y),
                        boxColor, triggerParams.lineThickness
                    );
                }

                // 连接顶部和底部
                for (int i = 0; i < 4; i++) {
                    drawList->AddLine(
                        ImVec2(screenCorners[i].x, screenCorners[i].y),
                        ImVec2(screenCorners[i + 4].x, screenCorners[i + 4].y),
                        boxColor, triggerParams.lineThickness
                    );
                }
            }
        }
    }
}

void TriggerBot::UpdateLastClickTime() {
    lastClickTime = GetTickCount64();
}

void TriggerBot::renderUI() {
    // 标题和分隔线
    ImGui::Text("%s", t_("TRIGGERBOT SETTINGS"));
    ImGui::Separator();
    ImGui::Spacing();

    // 添加顶部水平导航 - 简化版本
    static int triggerTab = 0; // 0=Basic, 1=Hitboxes, 2=Keys

    // 创建简洁的标签
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.18f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.30f, 0.40f, 1.0f));

    // 水平排列标签
    if (ImGui::Button(t_("Basic"), ImVec2(120, 36))) triggerTab = 0;
    ImGui::SameLine();
    if (ImGui::Button(t_("Hitboxes"), ImVec2(120, 36))) triggerTab = 1;
    ImGui::SameLine();
    if (ImGui::Button(t_("Keys"), ImVec2(120, 36))) triggerTab = 2;

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Spacing();

    // 根据选择的标签页显示不同内容
    switch (triggerTab) {
    case 0: // 基本设置
        renderBasicSettings();
        break;
    case 1: // Hitbox设置
        renderHitboxSettings();
        break;
    case 2: // 按键设置
        renderKeyBindings();
        break;
    }
}

void TriggerBot::renderBasicSettings() {
    ImGui::BeginChild("BasicSettings", ImVec2(0, 0), false);

    // 主开关
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable TriggerBot"), &triggerParams.enable);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 基本设置组
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Basic Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    // 触发器类型选择
    const char* triggerTypes[] = { t_("Auto Fire when Aimed At"), t_("Calculate Bone Position") };
    ImGui::Text("%s", t_("Trigger Mode"));
    ImGui::PushItemWidth(300.0f);
    ImGui::Combo("##TriggerType", &triggerParams.type, triggerTypes, IM_ARRAYSIZE(triggerTypes));
    ImGui::PopItemWidth();

    // 延迟设置
    ImGui::Text("%s", t_("Trigger Delay (ms)"));
    ImGui::PushItemWidth(300.0f);
    ImGui::SliderInt("##TriggerDelay", &triggerParams.delay, 0, 200);
    ImGui::PopItemWidth();

    // 可视化设置
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Visual Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Show Hitboxes"), &triggerParams.showHitboxes);
    ImGui::Checkbox(t_("Show Prediction"), &triggerParams.showPrediction);

    // 线条粗细
    if (triggerParams.showHitboxes) {
        ImGui::Text("%s", t_("Line Thickness"));
        ImGui::PushItemWidth(300.0f);
        ImGui::SliderFloat("##LineThickness", &triggerParams.lineThickness, 0.5f, 3.0f, "%.1f");
        ImGui::PopItemWidth();
    }

    // 颜色设置
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Color Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("%s", t_("Normal Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##NormalColor", (float*)&triggerParams.hitboxColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s", t_("Active Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##ActiveColor", (float*)&triggerParams.hitboxActiveColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::EndChild();
}

void TriggerBot::renderHitboxSettings() {
    ImGui::BeginChild("HitboxSettings", ImVec2(0, 0), false);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    if (triggerParams.type == 0) {
        ImGui::TextWrapped("%s", t_("Note: Your TriggerType Settings is AimeAt only! So the Hitbox Settings will not use!"));
    }
    ImGui::Text("%s", t_("Bounding Box Configuration"));
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // 边界框判定说明
    ImGui::TextWrapped("%s", t_("Uses 3D bounding boxes for more accurate detection"));
    ImGui::Spacing();

    // 显示设置区域
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Display Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Show Hitboxes"), &triggerParams.showHitboxes);
    ImGui::SameLine(200);
    ImGui::Checkbox(t_("Show Prediction"), &triggerParams.showPrediction);

    if (triggerParams.showHitboxes) {
        ImGui::Text("%s", t_("Line Thickness"));
        ImGui::PushItemWidth(300.0f);
        ImGui::SliderFloat("##Thickness", &triggerParams.lineThickness, 0.5f, 3.0f, "%.1f");
        ImGui::PopItemWidth();

        ImGui::Text("%s", t_("Normal Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##NormalColor", (float*)&triggerParams.hitboxColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

        ImGui::Text("%s", t_("Active Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##ActiveColor", (float*)&triggerParams.hitboxActiveColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 尺寸设置区域
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Size Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    // 边界框扩展设置
    ImGui::Text("%s", t_("Hitbox Expansion"));
    ImGui::PushItemWidth(300.0f);
    ImGui::SliderFloat("##Expansion", &triggerParams.extraPadding, 0.0f, 0.5f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", t_("Expands hitbox for easier triggering"));

    ImGui::Spacing();

    // 近距离设置
    ImGui::Text("%s", t_("Close Range Threshold"));
    ImGui::PushItemWidth(300.0f);
    ImGui::SliderFloat("##CloseRange", &triggerParams.closeRangeThreshold, 1.0f, 10.0f, "%.1f");
    ImGui::PopItemWidth();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", t_("Automatically expands at close range"));

    // 额外的调试选项
    ImGui::Spacing();
    ImGui::Spacing();

    // 高级选项区域
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Advanced Options"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    static bool showDebugInfo = false;
    ImGui::Checkbox(t_("Show Debug Info"), &showDebugInfo);
    if (showDebugInfo) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Min/Max: %.2f, %.2f, %.2f / %.2f, %.2f, %.2f",
            -10.0f, -10.0f, -20.0f, 10.0f, 10.0f, 50.0f);  // 示例值
    }

    ImGui::Spacing();
    ImGui::EndChild();
}

void TriggerBot::renderKeyBindings() {
    ImGui::BeginChild("KeyBindings", ImVec2(0, 0), false);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Triggerbot Key Bindings"));
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    auto& renderer = ImGuiRenderer::getInstance();

    // 从aimbot获取键位引用
    int& triggerKey = aimbot->keys.trigger;
    int& triggerKeyController = aimbot->keys.triggerController;
    int& triggerAutoKey = aimbot->keys.triggerAuto;
    int& triggerAutoKeyController = aimbot->keys.triggerAutoController;

    // 标准触发键
    if (!misc->uiParams.useController) {
        renderer.renderKeyBinding(t_("Standard Trigger"), &triggerKey);
    }
    else {
        renderer.renderGamepadBinding(t_("Standard Trigger"), &triggerKeyController);
    }
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", t_("Activates trigger without aiming"));

    ImGui::Spacing();
    ImGui::Spacing();

    // 磁性触发键
    if (!misc->uiParams.useController) {
        renderer.renderKeyBinding(t_("Magnetic Trigger"), &triggerAutoKey);
    }
    else {
        renderer.renderGamepadBinding(t_("Magnetic Trigger"), &triggerAutoKeyController);
    }
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", t_("Aims at target while triggering"));

    ImGui::Spacing();
    ImGui::EndChild();
}

void TriggerBot::HelpMarker(const char* desc) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}