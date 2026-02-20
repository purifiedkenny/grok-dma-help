#include <algorithm>
#include <cmath>
#include "ESP.h"

#include "include.h"

#include "Skeleton.h"
#include "ItemList.h"
#include "imgui.h"



ESP::ESP() : AutoConfigurable("ESP") {
    // 注册配置参数
    // 敌人设置
    registerParam("Draw_FPS", drawFPS);
    registerParam("Enemy_Enabled", enemies.enabled);
    registerParam("Enemy_Distance", enemies.distance);
    registerParam("Enemy_ShowName", enemies.showName);
    registerParam("Enemy_ShowDistance", enemies.showDistance);
    registerParam("Enemy_ShowLevel", enemies.showLevel);
    registerParam("Enemy_ShowRank", enemies.showRank);
    registerParam("Enemy_ShowWeapon", enemies.showWeapon);
    registerParam("Enemy_ShowHealth", enemies.showHealth);
    registerParam("Enemy_ShowSkeleton", enemies.showSkeleton);
    registerParam("Enemy_ColorByHealth", enemies.colorByHealth);
    registerParam("Enemy_SkeletonThickness", enemies.skeletonThickness);
    registerParam("Enemy_EspThickness", enemies.espThickness);
    registerParam("Enemy_BoxStyle", reinterpret_cast<int&>(enemies.boxStyle));
    registerParam("Enemy_HealthBarStyle", reinterpret_cast<int&>(enemies.healthBarStyle));

    registerParam("Enemy_VisColor", enemies.visibleColor);
    registerParam("Enemy_InVisColor", enemies.invisibleColor);
    registerParam("Enemy_SkeInVisColor", enemies.invisibleSkeletonColor);
    registerParam("Enemy_SkeInVisColor", enemies.visibleSkeletonColor);

    // 队友设置
    teammates.enabled = false; // 默认为false
    registerParam("Teammate_Enabled", teammates.enabled);
    registerParam("Teammate_Distance", teammates.distance);
    registerParam("Teammate_ShowName", teammates.showName);
    registerParam("Teammate_ShowDistance", teammates.showDistance);
    registerParam("Teammate_ShowLevel", teammates.showLevel);
    registerParam("Teammate_ShowRank", teammates.showRank);
    registerParam("Teammate_ShowWeapon", teammates.showWeapon);
    registerParam("Teammate_ShowHealth", teammates.showHealth);
    registerParam("Teammate_ShowSkeleton", teammates.showSkeleton);
    registerParam("Teammate_ColorByHealth", teammates.colorByHealth);
    registerParam("Teammate_SkeletonThickness", teammates.skeletonThickness);
    registerParam("Teammate_EspThickness", teammates.espThickness);
    registerParam("Teammate_BoxStyle", reinterpret_cast<int&>(teammates.boxStyle));
    registerParam("Teammate_HealthBarStyle", reinterpret_cast<int&>(teammates.healthBarStyle));
    registerParam("Teammate_VisColor", teammates.visibleColor);
    registerParam("Teammate_InVisColor", teammates.invisibleColor);
    registerParam("Teammate_SkeInVisColor", teammates.invisibleSkeletonColor);
    registerParam("Teammate_SkeInVisColor", teammates.visibleSkeletonColor);

    // 机器人设置
    registerParam("Bot_Enabled", bots.enabled);
    registerParam("Bot_Distance", bots.distance);
    registerParam("Bot_ShowName", bots.showName);
    registerParam("Bot_ShowDistance", bots.showDistance);
    registerParam("Bot_ShowLevel", bots.showLevel);
    registerParam("Bot_ShowRank", bots.showRank);
    registerParam("Bot_ShowWeapon", bots.showWeapon);
    registerParam("Bot_ShowHealth", bots.showHealth);
    registerParam("Bot_ShowSkeleton", bots.showSkeleton);
    registerParam("Bot_ColorByHealth", bots.colorByHealth);
    registerParam("Bot_SkeletonThickness", bots.skeletonThickness);
    registerParam("Bot_EspThickness", bots.espThickness);
    registerParam("Bot_BoxStyle", reinterpret_cast<int&>(bots.boxStyle));
    registerParam("Bot_HealthBarStyle", reinterpret_cast<int&>(bots.healthBarStyle));
    registerParam("Bot_VisColor", bots.visibleColor);
    registerParam("Bot_InVisColor", bots.invisibleColor);
    registerParam("Bot_SkeInVisColor", bots.invisibleSkeletonColor);
    registerParam("Bot_SkeInVisColor", bots.visibleSkeletonColor);

    registerParam("Enemy_Animation", enemies.animation);
    registerParam("Enemy_AnimationMaxDistance", enemies.animationMaxDistance);
    registerParam("Enemy_AnimationMinDistance", enemies.animationMinDistance);
    registerParam("Enemy_AnimationFadeScale", enemies.animationFadeScale);

    // 为队友添加
    registerParam("Teammate_Animation", teammates.animation);
    registerParam("Teammate_AnimationMaxDistance", teammates.animationMaxDistance);
    registerParam("Teammate_AnimationMinDistance", teammates.animationMinDistance);
    registerParam("Teammate_AnimationFadeScale", teammates.animationFadeScale);

    // 为机器人添加
    registerParam("Bot_Animation", bots.animation);
    registerParam("Bot_AnimationMaxDistance", bots.animationMaxDistance);
    registerParam("Bot_AnimationMinDistance", bots.animationMinDistance);
    registerParam("Bot_AnimationFadeScale", bots.animationFadeScale);

    // 物品全局设置
    registerParam("Item_GlobalEnabled", items.globalEnabled);
    registerParam("Item_MaxDistance", items.maxDistance);
    registerParam("Item_Animation", items.animation);
    registerParam("Item_FOV", items.fov);
    registerParam("Item_Scale", items.scale);
    registerParam("Item_Style", items.style);
    registerParam("Item_ShowName", items.showName);
    registerParam("Item_ShowDistance", items.showDistance);
    registerParam("Item_ShowIcon", items.showIcon);

    // 屏幕外指示器设置
    registerParam("OffScreen_Enabled", offScreenIndicator.enabled);
    registerParam("OffScreen_MaxDistance", offScreenIndicator.maxDistance);
    registerParam("OffScreen_ShowDistance", offScreenIndicator.showDistance);
    registerParam("OffScreen_EnemyOnly", offScreenIndicator.enemyOnly);
    registerParam("OffScreen_IndicatorRadius", offScreenIndicator.indicatorRadius);
    registerParam("OffScreen_ArrowSize", offScreenIndicator.arrowSize);
    registerParam("OffScreen_ArrowAngleWidth", offScreenIndicator.arrowAngleWidth);

    // 键位设置
    registerParam("Toggle_Key", keys.toggleKey);
    registerParam("Toggle_KeyController", keys.toggleKeyController);
    registerParam("Hold_Key", keys.holdKeyController);
    registerParam("Hold_KeyController", keys.holdKey);
    registerParam("Hold_Mode", keys.holdMode);

    registerParam("MenuKey", uiSettings.menuKey);
    registerParam("EndKey", uiSettings.endKey);
    registerParam("FusionKey", uiSettings.fusionKey);
    registerParam("RadarKey", uiSettings.radarKey);

    registerParam("MenuKey_Controller", uiSettings.menuKeyController);
    registerParam("EndKey_Controller", uiSettings.endKeyController);
    registerParam("FusionKey_Controller", uiSettings.fusionKeyController);
    registerParam("RadarKey_Controller", uiSettings.radarKeyController);

    registerParam("Menu_TheSelected", uiSettings.themeSelected);

    // 全局设置
    registerParam("ESP_Enabled", espEnabled);
}

ESP::~ESP() {
    // 清理资源
    // 不需要特别的清理，因为这里没有动态分配内存
}

const ImVec4 ESP::defaultColors[] = {
    ImVec4(1.0f, 0.27f, 0.72f, 1.0f),  // pink - Medical items
    ImVec4(1.0f, 0.58f, 0.0f, 1.0f),   // orange - Light weapons
    ImVec4(1.0f, 0.25f, 0.25f, 1.0f),  // red - Shotguns
    ImVec4(0.28f, 1.0f, 0.35f, 1.0f),  // lime - Energy weapons
    ImVec4(0.0f, 0.77f, 0.64f, 1.0f),  // green - Heavy weapons
    ImVec4(0.41f, 0.16f, 1.0f, 1.0f),  // purple - Sniper weapons
    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),    // white - Level 1 attachments
    ImVec4(0.27f, 0.89f, 1.0f, 1.0f),  // blue - Level 2 attachments
    ImVec4(0.88f, 0.27f, 1.0f, 1.0f),  // purple - Level 3 attachments
    ImVec4(1.0f, 0.87f, 0.24f, 1.0f),  // gold - Level 4 attachments
    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),    // white - Level 1 armor
    ImVec4(0.27f, 0.89f, 1.0f, 1.0f),  // blue - Level 2 armor
    ImVec4(0.88f, 0.27f, 1.0f, 1.0f),  // purple - Level 3 armor
    ImVec4(1.0f, 0.87f, 0.24f, 1.0f),  // gold - Level 4 armor
    ImVec4(1.0f, 0.25f, 0.25f, 1.0f),  // red - Drop weapons
    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),    // white - Grenades
};

void ESP::initItemColors()
{
    initializeCategorySettings();

    // 注册分类设置参数
    for (size_t i = 0; i < categorySettings.size(); i++) {
        registerParam("Category_" + std::to_string(i) + "_Enabled", categorySettings[i].enabled);
        registerParam("Category_" + std::to_string(i) + "_Color", categorySettings[i].color);
    }
}

void ESP::initializeCategorySettings() {
    // 清除现有设置
    categorySettings.clear();

    // 获取分类总数
    size_t totalCategories = std::distance(Lists.begin(), Lists.end());
    categorySettings.reserve(totalCategories);
    // 为每个分类创建设置，直接使用静态数组
    for (size_t i = 0; i < totalCategories; i++) {
        ItemCategorySetting categorySetting;
        categorySetting.enabled = true;
        categorySetting.color = defaultColors[i];
        categorySettings.push_back(categorySetting);
    }
}

std::vector<std::string> ESP::GetItemCategoryNames() const {
    // 确保返回的名称列表与Lists容器中的分类数量一致
    std::vector<std::string> names = {
        "Medical Items",
        "Light Weapons",
        "Shotguns",
        "Energy Weapons",
        "Heavy Weapons",
        "Sniper Weapons",
        "Level 1 Attachments",
        "Level 2 Attachments",
        "Level 3 Attachments",
        "Level 4 Attachments",
        "Level 1 Armor",
        "Level 2 Armor",
        "Level 3 Armor",
        "Level 4 Armor",
        "Rare Weapons",
        "Grenades"
    };

    return names;
}

// 获取物品设置
const ESP::ItemSpecificSetting& ESP::GetItemSettings(int itemId, size_t categoryIndex) {
    // 如果有特定物品设置，则使用
    auto it = specificItemSettings.find(itemId);
    if (it != specificItemSettings.end()) {
        return it->second;
    }

    // 否则使用默认设置
    static ItemSpecificSetting defaultSetting;
    defaultSetting.enabled = true;
    defaultSetting.useCustomColor = false;
    defaultSetting.customColor = categoryIndex < categorySettings.size()
        ? categorySettings[categoryIndex].color
        : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    defaultSetting.showName = items.showName;
    defaultSetting.showDistance = items.showDistance;
    defaultSetting.showIcon = items.showIcon;

    return defaultSetting;
}

// 检查物品是否应该显示
bool ESP::ShouldDisplayItem(int itemId, size_t categoryIndex) {
    // 先检查全局设置
    if (!items.globalEnabled) {
        return false;
    }

    // 检查categorySettings是否为空
    if (categorySettings.empty()) {
        // 使用通用初始化函数而不是硬编码颜色
        initializeCategorySettings();
    }

    // 再检查分类设置
    if (categoryIndex >= categorySettings.size()) {
        return true; // 返回true而不是false，允许显示分类索引超出范围的物品
    }

    if (!categorySettings[categoryIndex].enabled) {
        return false;
    }

    // 获取物品特定设置
    auto it = specificItemSettings.find(itemId);
    if (it == specificItemSettings.end()) {
        // 如果没有特定设置，为该物品创建默认设置
        ItemSpecificSetting defaultSetting;
        defaultSetting.enabled = true;
        defaultSetting.useCustomColor = false;
        defaultSetting.customColor = (categoryIndex < categorySettings.size())
            ? categorySettings[categoryIndex].color
            : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        defaultSetting.showName = items.showName;
        defaultSetting.showDistance = items.showDistance;
        defaultSetting.showIcon = items.showIcon;
        specificItemSettings[itemId] = defaultSetting;

        return true; // 默认允许显示
    }

    return it->second.enabled;
}

// 获取物品颜色
ImVec4 ESP::GetItemColor(int itemId, size_t categoryIndex, float alphaMultiplier) {
    // 获取物品设置
    const auto& itemSetting = GetItemSettings(itemId, categoryIndex);

    // 确定使用哪个颜色
    ImVec4 colorVec;
    if (itemSetting.useCustomColor) {
        colorVec = itemSetting.customColor;
    }
    else if (categoryIndex < categorySettings.size()) {
        colorVec = categorySettings[categoryIndex].color;
    }
    else {
        colorVec = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // 默认白色
    }
    colorVec.w *= alphaMultiplier;
    return colorVec;
}

void ESP::updateKeys() {
    static bool init = false;
    if (!init) {
        std::unordered_map<std::string, int> espKeys = {
            {"toggle", keys.toggleKey},
            {"toggleController", keys.toggleKeyController},
            {"hold", keys.holdKey},
            {"holdController", keys.holdKeyController},
        };

        keyDetector->registerKeysForContext("ESP", espKeys);
        initItemColors();
        init = true;
    }

    keyDetector->updateKeyCode("ESP", "toggle", keys.toggleKey);
    keyDetector->updateKeyCode("ESP", "toggleController", keys.toggleKeyController);
    keyDetector->updateKeyCode("ESP", "hold", keys.holdKey);
    keyDetector->updateKeyCode("ESP", "holdController", keys.holdKeyController);
}

bool ESP::isKeyHeld(const std::string& keyName) {
    return IS_KEY_HELD("ESP", keyName);
}

bool ESP::isKeyToggled(const std::string& keyName) {
    if (IS_KEY_TRIGGERED("ESP", keyName))
    {
        CLEAR_KEY_TOGGLE_STATE("ESP", keyName);
        return true;
    }
    return false;
}

bool ESP::isPointOnScreen(const Vec2D& point) {
    return (point.x > 0 && point.x < gameView->ScreenSize.x &&
        point.y > 0 && point.y < gameView->ScreenSize.y);
}

std::pair<Vec2D, Vec2D> ESP::GetBounds(Vec3D min, Vec3D max) {
    Vec2D flb, brt, blb, frt, frb, brb, blt, flt;
    float left, top, right, bottom;

    Vec3D points[] = {
        Vec3D(min.x, min.y, min.z),
        Vec3D(min.x, max.y, min.z),
        Vec3D(max.x, max.y, min.z),
        Vec3D(max.x, min.y, min.z),
        Vec3D(max.x, max.y, max.z),
        Vec3D(min.x, max.y, max.z),
        Vec3D(min.x, min.y, max.z),
        Vec3D(max.x, min.y, max.z)
    };

    if (!gameView->WorldToScreenNoClip(points[3], flb) || !gameView->WorldToScreenNoClip(points[5], brt) ||
        !gameView->WorldToScreenNoClip(points[0], blb) || !gameView->WorldToScreenNoClip(points[4], frt) ||
        !gameView->WorldToScreenNoClip(points[2], frb) || !gameView->WorldToScreenNoClip(points[1], brb) ||
        !gameView->WorldToScreenNoClip(points[6], blt) || !gameView->WorldToScreenNoClip(points[7], flt))
        return std::make_pair(Vec2D(0, 0), Vec2D(0, 0));

    Vec2D arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
    left = flb.x;
    top = flb.y;
    right = flb.x;
    bottom = flb.y;

    if (left || top || right || bottom) {
        for (auto x = 1; x < 8; ++x) {
            if (left > arr[x].x)
                left = arr[x].x;
            if (bottom < arr[x].y)
                bottom = arr[x].y;
            if (right < arr[x].x)
                right = arr[x].x;
            if (top > arr[x].y)
                top = arr[x].y;
        }
        return std::make_pair(Vec2D(left, top), Vec2D(right, bottom));
    }

    return std::make_pair(Vec2D(0, 0), Vec2D(0, 0));
}

ImColor ESP::getPlayerColor(const Player* player, const PlayerESPSettings& settings) {
    if (!player) return ImColor(255, 255, 255, 255);

    // 基于距离的颜色渐变
    float distanceFactor = std::min(1.0f, player->DistanceToLocalPlayer / settings.distance);

    // 专业玩家特殊颜色
    if (player->IsPro) {
        ImVec4 proColor = settings.proPlayerColor;
        return ImColor(
            proColor.x * 255,
            proColor.y * 255,
            proColor.z * 255,
            (1.0f - distanceFactor * 0.5f) * 255
        );
    }

    // 根据可见性选择颜色
    ImVec4 baseColor = player->IsVisible ? settings.visibleColor : settings.invisibleColor;

    // 如果启用了根据血量渐变颜色
    if (settings.colorByHealth) {
        // 计算血量百分比
        float healthPercentage = player->Health / 100.0f;

        // 血量低时偏红，高时偏绿
        return ImColor(
            int((1.0f - healthPercentage) * 255),
            int(healthPercentage * 255),
            0,
            int((1.0f - distanceFactor * 0.5f) * 255)
        );
    }

    // 使用基础颜色
    return ImColor(
        baseColor.x * 255,
        baseColor.y * 255,
        baseColor.z * 255,
        (1.0f - distanceFactor * 0.5f) * 255
    );
}

void ESP::drawOffScreenIndicator(const Player* player, ImGuiRenderer& renderer) {
    if (!player || !offScreenIndicator.enabled) return;

    if (offScreenIndicator.enemyOnly && !player->IsHostile) return;
    if (player->DistanceToLocalPlayer > offScreenIndicator.maxDistance) return;

    // 检查玩家是否在屏幕上
    Vec2D screenPos;
    if (gameView->WorldToScreen(player->Origin, screenPos)) {
        // 检查点是否在屏幕内
        if (screenPos.x >= 0 && screenPos.x <= gameView->ScreenSize.x &&
            screenPos.y >= 0 && screenPos.y <= gameView->ScreenSize.y) {
            return; // 玩家在屏幕上，不需要指示器
        }
    }

    // 计算屏幕中心和尺寸
    float screenWidth = gameView->ScreenSize.x;
    float screenHeight = gameView->ScreenSize.y;
    ImVec2 screenCenter(screenWidth / 2.0f, screenHeight / 2.0f);

    // 获取本地玩家和敌人的位置差异
    Vec3D localOrigin = localPlayer->CameraOrigin;
    float yaw = localPlayer->ViewAngles.y * (3.14159f / 180.0f); // 转换为弧度

    // 计算从玩家到敌人的向量
    Vec3D positionDiff = player->Origin - localOrigin;

    // 使用视角旋转向量，将世界坐标转换为相对于玩家视角的坐标
    float x = std::cos(yaw) * positionDiff.y - std::sin(yaw) * positionDiff.x;
    float y = std::cos(yaw) * positionDiff.x + std::sin(yaw) * positionDiff.y;

    // 归一化向量
    float length = std::sqrt(x * x + y * y);
    if (length < 0.001f) return; // 避免除零错误
    x /= length;
    y /= length;
    x = -x;
    y = -y;

    // 计算指示器在圆/椭圆边缘的位置
    float baseRadius = std::min(screenWidth, screenHeight) * 0.4f * offScreenIndicator.indicatorRadius;
    float margin = 50.0f; // 边缘安全距离

    float radius = baseRadius;
    float distanceFactor = 1.0f;
    if (offScreenIndicator.dynamicRadius) {
        // 计算距离缩放因子，距离越近，半径越小
        distanceFactor = std::min(1.0f, player->DistanceToLocalPlayer / offScreenIndicator.maxDistance);
        float radiusScale = 0.6f + distanceFactor * 0.9f; // 在0.6到1.5倍之间变化，更明显的差异
        radius *= radiusScale;
    }

    // 计算指示器位置
    ImVec2 indicatorPos(screenCenter.x + x * radius, screenCenter.y + y * radius);

    // 确保指示器在屏幕边缘内
    float minX = margin;
    float maxX = screenWidth - margin;
    float minY = margin;
    float maxY = screenHeight - margin;

    indicatorPos.x = std::max(minX, std::min(maxX, indicatorPos.x));
    indicatorPos.y = std::max(minY, std::min(maxY, indicatorPos.y));

    // 获取指示器颜色
    ImColor indicatorColor;

    // 根据距离调整透明度
    float alphaFactor = 1.0f - (player->DistanceToLocalPlayer / offScreenIndicator.maxDistance);
    alphaFactor = std::max(0.5f, alphaFactor); // 最小透明度为50%

    // 如果是敌人，使用敌人的颜色
    if (player->IsHostile) {
        ImVec4 enemyColor = enemies.visibleColor;
        indicatorColor = ImColor(
            enemyColor.x * 255,
            enemyColor.y * 255,
            enemyColor.z * 255,
            alphaFactor * 255
        );
    }
    else {
        // 否则使用设置的指示器颜色
        ImVec4 baseColor = offScreenIndicator.indicatorColor;
        indicatorColor = ImColor(
            baseColor.x * 255,
            baseColor.y * 255,
            baseColor.z * 255,
            alphaFactor * 255
        );
    }

    // 箭头大小设置 - 也随距离变化
    float arrowSize = offScreenIndicator.arrowSize * 2.0f;
    // 如果启用动态半径，箭头大小也应该随距离变化
    if (offScreenIndicator.dynamicRadius) {
        // 距离越近，箭头越大（因为半径越小）
        arrowSize *= (1.0f - distanceFactor * 0.3f + 0.3f);
    }
    else {
        // 原有的箭头大小调整逻辑
        float arrowDistanceFactor = std::min(1.0f, player->DistanceToLocalPlayer / (offScreenIndicator.maxDistance * 0.5f));
        arrowSize = std::max(arrowSize * (1.0f - arrowDistanceFactor * 0.3f), arrowSize * 0.6f);
    }

    // 计算箭头方向 - 从指示器指向屏幕外（敌人方向）
    // 方向向量是从指示器到屏幕中心的方向的反方向
    float dirX = x; // 这里使用原始的x (已取反)
    float dirY = y; // 这里使用原始的y (已取反)

    // 箭头顶点 - 朝外
    ImVec2 arrowTip = ImVec2(
        indicatorPos.x + dirX * arrowSize,
        indicatorPos.y + dirY * arrowSize
    );

    // 计算垂直于箭头方向的向量
    float perpX = -dirY;
    float perpY = dirX;

    // 箭头底部两点
    float arrowWidth = arrowSize * 0.7f * sin(offScreenIndicator.arrowAngleWidth);

    ImVec2 arrowLeft = ImVec2(
        indicatorPos.x + perpX * arrowWidth,
        indicatorPos.y + perpY * arrowWidth
    );

    ImVec2 arrowRight = ImVec2(
        indicatorPos.x - perpX * arrowWidth,
        indicatorPos.y - perpY * arrowWidth
    );

    // 绘制箭头
    std::vector<ImVec2> arrowPoints = { arrowTip, arrowLeft, arrowRight };
    renderer.drawPolygonFilled(arrowPoints, indicatorColor);
    renderer.drawPolygon(arrowPoints, indicatorColor, 1.5f);

    // 显示距离信息
    if (offScreenIndicator.showDistance) {
        char distText[16];
        sprintf(distText, "%.0fm", player->DistanceToLocalPlayer);

        auto textSize = renderer.getTextSize(distText, "esp_small");
        float textX = indicatorPos.x - textSize.x * 0.5f;

        // 根据箭头方向调整文本位置
        float dot = dirY; // 检查垂直方向
        float textY;

        if (dot > 0) {
            // 箭头向下，文本放在上方
            textY = indicatorPos.y - textSize.y - arrowSize - 5.0f;
        }
        else {
            // 箭头向上或水平，文本放在下方
            textY = indicatorPos.y + arrowSize + 5.0f;
        }

        // 文本背景
        ImVec2 textBgMin(textX - 2, textY - 1);
        ImVec2 textBgMax(textX + textSize.x + 2, textY + textSize.y + 1);
        renderer.drawRectFilled(textBgMin, textBgMax, ImColor(20, 20, 20, 160), 2.0f);

        // 根据距离调整文本颜色
        ImColor textColor = ImColor(255, 255, 255, int(alphaFactor * 220));
        renderer.drawTextWithShadow(distText, ImVec2(textX, textY),
            textColor, ImColor(20, 20, 20, 180), 1.0f, "esp_small");
    }
}

void ESP::drawPlayerESP(const Player* player, ImGuiRenderer& renderer, const PlayerESPSettings& settings) {
    if (!player || !settings.enabled) return;

    // 跳过距离过远的玩家
    if (player->DistanceToLocalPlayer > settings.distance) return;

    // 获取玩家包围盒坐标
    std::pair<Vec2D, Vec2D> Bounds = GetBounds(player->VecMin, player->VecMax);
    ImVec2 TopLeft(Bounds.first.x, Bounds.first.y);
    ImVec2 BottomRight(Bounds.second.x, Bounds.second.y);

    // 检查坐标有效性
    if (TopLeft.x == 0 || TopLeft.y == 0 || BottomRight.x == 0 || BottomRight.y == 0) {
        // 玩家在屏幕外，绘制指示器
        if (offScreenIndicator.enabled) {
            drawOffScreenIndicator(player, renderer);
        }
        return;
    }

    // 计算玩家框中心点
    ImVec2 playerBoxCenter(
        (TopLeft.x + BottomRight.x) / 2.0f,
        (TopLeft.y + BottomRight.y) / 2.0f
    );

    // 计算屏幕中心
    ImVec2 screenCenter(
        gameView->ScreenSize.x / 2.0f,
        gameView->ScreenSize.y / 2.0f
    );

    // 计算到屏幕中心的距离
    float distanceToCenter = std::sqrt(
        std::pow(playerBoxCenter.x - screenCenter.x, 2) +
        std::pow(playerBoxCenter.y - screenCenter.y, 2)
    );

    // 基于距离计算透明度系数
    float alphaMult = 1.0f;

    // 如果启用了动画效果，则根据距离调整透明度
    if (settings.animation) {
        if (distanceToCenter < settings.animationMinDistance) {
            // 距离小于最小阈值，完全透明
            alphaMult = 0.0f;
        }
        else if (distanceToCenter < settings.animationMaxDistance) {
            // 在最小和最大阈值之间，线性插值
            alphaMult = (distanceToCenter - settings.animationMinDistance) /
                (settings.animationMaxDistance - settings.animationMinDistance);

            // 应用自定义缩放因子
            alphaMult = std::min(1.0f, alphaMult * (settings.animationFadeScale / 10));
        }
            // 距离大于等于最大阈值，保持完全不透明(alphaMult = 1.0f)
    }

    alphaMult = std::max(0.2f, alphaMult);

    ImColor playerColor = getPlayerColor(player, settings);

    // 绘制边框
    switch (settings.boxStyle) {
    case PlayerESPSettings::BoxStyle::Box2D:
        renderer.drawRect(TopLeft, BottomRight, playerColor, settings.espThickness);
        break;
    case PlayerESPSettings::BoxStyle::Corner:
        renderer.drawCornerBox(TopLeft, BottomRight, playerColor, settings.espThickness);
        break;
    case PlayerESPSettings::BoxStyle::None:
    default:
        // 不绘制边框
        break;
    }

    playerColor.Value.w *= alphaMult; // 应用透明度

    // 准备玩家信息文本
    std::string playerInfo;

    // 玩家名称
    if (settings.showName) {
        playerInfo = player->NickName;
        if (player->IsPro) {
            playerInfo = "[PRO] " + playerInfo;
        }
    }

    // 玩家距离
    if (settings.showDistance) {
        char distText[16];
        sprintf(distText, " [%.0fm]", player->DistanceToLocalPlayer);
        playerInfo += distText;
    }

    // 玩家等级
    if (settings.showLevel && player->Level > 0) {
        char levelText[16];
        sprintf(levelText, " [Lv%d]", player->Level);
        playerInfo += levelText;
    }

    // 玩家段位
    if (settings.showRank && !player->RankName.empty()) {
        playerInfo += " [" + player->RankName + "]";
    }

    if (!playerInfo.empty()) {
        auto textSize = renderer.getTextSize(playerInfo, "esp");
        float textX = (TopLeft.x + BottomRight.x) / 2 - textSize.x / 2;

        // 根据血条样式调整文本Y位置，避免被血条覆盖
        float textY = TopLeft.y - 5.f - textSize.y;

        // 根据血条样式调整文本位置
        if (settings.showHealth && player->Health > 0 && !player->IsKnocked) {
            if (settings.healthBarStyle == PlayerESPSettings::HealthBarStyle::Seer) {
                // Seer血条在顶部，需要更高位置
                textY = TopLeft.y - (40.f + textSize.y);
            }
            else if (settings.healthBarStyle == PlayerESPSettings::HealthBarStyle::Custom) {
                // Custom血条也需要更高位置
                textY = TopLeft.y - (40.f + textSize.y);
            }
        }

        // 显示击杀数和伤害 (放在玩家名称上方)
        if (player->IsBot && player->Kills > 0 || player->Damages > 0) {
            char statsText[64];
            sprintf(statsText, "K: %d | DMG: %u", player->Kills, player->Damages);

            auto statsSize = renderer.getTextSize(statsText, "esp");
            float statsX = (TopLeft.x + BottomRight.x) / 2 - statsSize.x / 2;
            float statsY = textY - statsSize.y - 2; // 在名字文本上方

            // 统计信息背景 - 应用透明度
            ImVec2 statsBgMin(statsX - 5, statsY - 2);
            ImVec2 statsBgMax(statsX + statsSize.x + 5, statsY + statsSize.y + 2);
            ImColor bgColor = ImColor(20, 20, 20, int(150 * alphaMult));
            renderer.drawRectFilled(statsBgMin, statsBgMax, bgColor, 3.0f);

            // 绘制统计信息文本 - 应用透明度
            ImColor statsColor = ImColor(255, 255, 0, int(255 * alphaMult));
            ImColor shadowColor = ImColor(20, 20, 20, int(255 * alphaMult));
            renderer.drawTextWithShadow(statsText, ImVec2(statsX, statsY),
                statsColor, shadowColor, 1.5f, "esp");
        }

        // 文本背景 - 应用透明度
        ImVec2 textBgMin(textX - 5, textY - 2);
        ImVec2 textBgMax(textX + textSize.x + 5, textY + textSize.y + 2);
        ImColor bgColor = ImColor(20, 20, 20, int(150 * alphaMult));
        renderer.drawRectFilled(textBgMin, textBgMax, bgColor, 3.0f);

        // 绘制文本 - 应用透明度
        ImColor textColor = ImColor(
            settings.textColor.x * 255,
            settings.textColor.y * 255,
            settings.textColor.z * 255,
            settings.textColor.w * 255 * alphaMult
        );
        ImColor shadowColor = ImColor(20, 20, 20, int(180 * alphaMult));
        renderer.drawTextWithShadow(playerInfo, ImVec2(textX, textY),
            textColor, shadowColor, 1.5f, "esp");
    }

    // 绘制生命值和护甲条 - 传递透明度
    if (settings.showHealth && player->Health > 0 && !player->IsKnocked) {
        drawHealthAndShield(player, TopLeft, BottomRight, settings);
    }

    // 显示武器信息 - 应用透明度
    if (settings.showWeapon && player->weaponIndex > 0) {
        std::string weaponName = getWeaponNameById(player->weaponIndex);
        if (!weaponName.empty()) {
            auto textSize = renderer.getTextSize(weaponName, "esp_small");
            float textX = (TopLeft.x + BottomRight.x) / 2 - textSize.x / 2;
            float textY = BottomRight.y + (settings.showHealth ? 15.0f : 5.0f);

            // 武器名称文本背景
            ImVec2 weaponBgMin(textX - 3, textY - 1);
            ImVec2 weaponBgMax(textX + textSize.x + 3, textY + textSize.y + 1);
            ImColor weaponBgColor = ImColor(20, 20, 20, int(120 * alphaMult));
            renderer.drawRectFilled(weaponBgMin, weaponBgMax, weaponBgColor, 2.0f);

            // 武器名称文本
            ImColor weaponColor = ImColor(255, 255, 0, int(255 * alphaMult));
            ImColor weaponShadow = ImColor(20, 20, 20, int(255 * alphaMult));
            renderer.drawTextWithShadow(weaponName, ImVec2(textX, textY),
                weaponColor, weaponShadow, 1.5f, "esp_small");
        }
    }
}
// 调用渲染器中实现的血条绘制函数
void ESP::drawHealthAndShield(const Player* player, ImVec2 topLeft, ImVec2 bottomRight, const PlayerESPSettings& settings) {
    auto& renderer = ImGuiRenderer::getInstance();

    // 创建框结构，用于传递给绘制函数
    Vec4D box;
    box.x = topLeft.x;                   // 左
    box.y = topLeft.y;                   // 上
    box.z = bottomRight.y - topLeft.y;   // 高度
    box.w = bottomRight.x - topLeft.x;   // 宽度

    // 根据健康条风格选择不同绘制函数
    switch (settings.healthBarStyle) {
    case PlayerESPSettings::HealthBarStyle::Seer:
        renderer.DrawHealth(
            box.x + box.w / 2,      // 中心X坐标
            box.y - 14,             // 顶部上方位置
            player->Shield,         // 护甲值
            player->MaxShield,      // 最大护甲值
            0,                      // 附加参数
            player->Health          // 生命值
        );
        break;
    case PlayerESPSettings::HealthBarStyle::Classic:
        renderer.HealthBarSide(
            { box.x, box.y },                // 左上角
            { box.x + box.w, box.y + box.z },// 右下角
            player->Health                   // 生命值
        );
        renderer.ShiledBarSide(
            { box.x + box.w, box.y },        // 右上角
            { box.x + box.w, box.y + box.z },// 右下角
            player->Shield,                  // 护甲值
            player->MaxShield                // 最大护甲值
        );
        break;
    case PlayerESPSettings::HealthBarStyle::Custom:
        renderer.HealthBar(box, player->Health);
        renderer.ShieldBar(box, player->Shield, player->MaxShield);
        break;
    }
}

class BatchRoundedDrawer {
private:
    static constexpr int NUM_SEGMENTS = 6;

public:
    // Single background immediate drawing function
    void DrawRoundedParallelogram(ImDrawList* drawList, const ImVec2& pos, const ImVec2& size,
        float skewFactor, float cornerRadius, float padding, ImU32 color) {

        std::vector<ImVec2> points;
        points.reserve((NUM_SEGMENTS + 1) * 4);

        // Calculate base vertices with text padding
        const float textPaddingX = padding * 1.5f;
        const float textPaddingY = padding * 1.2f;

        ImVec2 basePoints[4] = {
            ImVec2(pos.x - textPaddingX + skewFactor, pos.y - textPaddingY),         // Top left
            ImVec2(pos.x + size.x + textPaddingX + skewFactor, pos.y - textPaddingY),   // Top right
            ImVec2(pos.x + size.x + textPaddingX - skewFactor, pos.y + size.y + textPaddingY), // Bottom right
            ImVec2(pos.x - textPaddingX - skewFactor, pos.y + size.y + textPaddingY)    // Bottom left
        };

        // Add vertices for each corner
        for (int corner = 0; corner < 4; corner++) {
            const ImVec2& base = basePoints[corner];
            for (int i = 0; i <= NUM_SEGMENTS; i++) {
                float angle = M_PI / 2 * i / NUM_SEGMENTS;
                ImVec2 p;

                switch (corner) {
                case 0: // Top left
                    p = ImVec2(
                        base.x + cornerRadius * (1 - cosf(angle)),
                        base.y + cornerRadius * (1 - sinf(angle))
                    );
                    break;
                case 1: // Top right
                    p = ImVec2(
                        base.x - cornerRadius * (1 - sinf(angle)),
                        base.y + cornerRadius * (1 - cosf(angle))
                    );
                    break;
                case 2: // Bottom right
                    p = ImVec2(
                        base.x - cornerRadius * (1 - cosf(angle)),
                        base.y - cornerRadius * (1 - sinf(angle))
                    );
                    break;
                case 3: // Bottom left
                    p = ImVec2(
                        base.x + cornerRadius * (1 - sinf(angle)),
                        base.y - cornerRadius * (1 - cosf(angle))
                    );
                    break;
                }
                points.push_back(p);
            }
        }

        drawList->AddConvexPolyFilled(points.data(), points.size(), color);
        drawList->AddPolyline(points.data(), points.size(), color, true, 1.0f);
    }
};

void ESP::RenderItemESP() {
    // 直接返回，如果物品ESP未启用
    if (!this->items.globalEnabled) {
        return;
    }

    const auto& processedData = itemProcessor->getProcessedItems();

    // 检查是否有物品
    if (processedData.itemCount == 0) {
        return;
    }

    // 检查和确保categorySettings已正确初始化
    if (categorySettings.empty() || categorySettings.size() < std::distance(Lists.begin(), Lists.end())) {
        initializeCategorySettings();
    }

    static BatchRoundedDrawer batchDrawer;
    auto* drawList = ImGui::GetBackgroundDrawList();
    ImGuiRenderer& renderer = ImGuiRenderer::getInstance();

    // 缓存频繁使用的设置
    const float fontSize = 15.0f;  // 固定字体大小
    const float iconSize = 30.0f;  // 固定图标大小
    const bool showName = this->items.showName;
    const bool showIcon = this->items.showIcon;
    const bool showDistance = this->items.showDistance;
    const bool useAnimation = this->items.animation;
    const float fovValue = this->items.fov;
    const float scaleValue = this->items.scale;
    const int styleValue = this->items.style;

    // 直接遍历已经处理好的物品数据
    for (size_t i = 0; i < processedData.itemCount; i++) {
        const auto& processed = processedData.items[i];
        // 检查物品是否应该显示
        if (!ShouldDisplayItem(processed.itemId, processed.categoryIndex)) {
            continue;
        }

        // 如果物品不在屏幕上，跳过
        if (!processed.isOnScreen) {
            continue;
        }

        Vec2D screenPos = processed.screenPos;

        // 计算Alpha基于屏幕中心距离
        float alphaMultiplier = 1.0f;
        if (useAnimation && processed.distanceToCenter <= fovValue) {
            alphaMultiplier = std::min(1.0f, std::max(0.0f,
                (1.0f / fovValue) * processed.distanceToCenter));
            if (alphaMultiplier <= scaleValue) alphaMultiplier = 0.0f;
        }

        // 跳过如果完全透明
        if (alphaMultiplier <= 0.0f) continue;

        // 获取物品设置
        const auto& itemSettings = GetItemSettings(processed.itemId, processed.categoryIndex);

        // 获取带应用alpha的物品颜色
        ImVec4 itemColor = GetItemColor(processed.itemId, processed.categoryIndex, alphaMultiplier);

        // 计算文本尺寸和位置
        ImVec2 textSize = ImGui::CalcTextSize(processed.name.c_str());
        float x = screenPos.x - (textSize.x / 2.0f);
        float y = screenPos.y - (textSize.y / 2.0f);

        // 计算绘制参数基于距离
        float distanceScale = std::min(1.0f, 30.0f / processed.distance);
        float padding = 5.0f * distanceScale;
        float skewFactor = 5.0f * distanceScale;
        float cornerRadius = 3.0f * distanceScale;

        // 基于风格渲染
        switch (styleValue) {
        case 1: // 仅文本
            if (itemSettings.showName && showName) {
                ImU32 bgColor = ImColor(20, 20, 20, int(120 * alphaMultiplier));
                batchDrawer.DrawRoundedParallelogram(
                    drawList,
                    ImVec2(x, y),
                    textSize,
                    skewFactor,
                    cornerRadius,
                    padding,
                    bgColor
                );

                renderer.drawTextWithShadow(processed.name, ImVec2(x, y),
                    itemColor, ImColor(20, 20, 20, int(200 * alphaMultiplier)),
                    1.0f, "esp_item");
            }
            break;

        case 2: // 仅图标
            if (itemSettings.showIcon && showIcon) {
                // 绘制图标 - 使用字形或圆圈作为占位符
                if (!processed.icon.empty()) {
                    // 如果图标是字体图标(字形)
                    renderer.drawText(processed.icon,
                        ImVec2(screenPos.x - iconSize / 4, screenPos.y - iconSize / 4),
                        itemColor, iconSize * distanceScale, "esp_item_icon");
                }
                else {
                    // 回退到绘制一个圆
                    renderer.drawCircleFilled(
                        ImVec2(screenPos.x, screenPos.y),
                        8.0f * distanceScale,
                        itemColor
                    );
                    renderer.drawCircle(
                        ImVec2(screenPos.x, screenPos.y),
                        8.0f * distanceScale + 1.0f,
                        ImColor(20, 20, 20, int(150 * alphaMultiplier)),
                        12, 1.0f
                    );
                }
            }
            break;

        case 3: // 文本和图标
            if (itemSettings.showName && showName) {
                ImU32 bgColor = ImColor(20, 20, 20, int(120 * alphaMultiplier));
                float yOffset = iconSize * distanceScale + 5;

                batchDrawer.DrawRoundedParallelogram(
                    drawList,
                    ImVec2(x, y + yOffset),
                    textSize,
                    skewFactor,
                    cornerRadius,
                    padding,
                    bgColor
                );

                renderer.drawTextWithShadow(processed.name,
                    ImVec2(x, y + yOffset),
                    itemColor, ImColor(20, 20, 20, int(200 * alphaMultiplier)),
                    1.0f, "esp_item");
            }

            if (itemSettings.showIcon && showIcon) {
                if (!processed.icon.empty()) {
                    // 如果图标是字体图标(字形)
                    renderer.drawText(processed.icon,
                        ImVec2(screenPos.x - iconSize / 4, screenPos.y - iconSize / 4),
                        itemColor, iconSize * distanceScale, "esp_item_icon");
                }
                else {
                    // 回退到绘制一个圆
                    renderer.drawCircleFilled(
                        ImVec2(screenPos.x, screenPos.y),
                        8.0f * distanceScale,
                        itemColor
                    );
                    renderer.drawCircle(
                        ImVec2(screenPos.x, screenPos.y),
                        8.0f * distanceScale + 1.0f,
                        ImColor(20, 20, 20, int(150 * alphaMultiplier)),
                        12, 1.0f
                    );
                }
            }
            break;
        }

        // 显示距离信息
        if (itemSettings.showDistance && showDistance) {
            char distText[16];
            sprintf(distText, "%.0fm", processed.distance);

            auto distTextSize = renderer.getTextSize(distText, "esp_item_small");
            ImVec2 distTextPos = ImVec2(
                screenPos.x - distTextSize.x * 0.5f,
                screenPos.y + 20.0f
            );

            // 可选：为距离文本添加背景
            ImVec2 distBgMin(distTextPos.x - 3, distTextPos.y - 1);
            ImVec2 distBgMax(distTextPos.x + distTextSize.x + 3, distTextPos.y + distTextSize.y + 1);
            renderer.drawRectFilled(distBgMin, distBgMax, ImColor(20, 20, 20, int(100 * alphaMultiplier)), 2.0f);

            // 绘制距离文本
            renderer.drawTextWithShadow(distText, distTextPos,
                ImColor(180, 180, 180, int(200 * alphaMultiplier)),
                ImColor(20, 20, 20, int(200 * alphaMultiplier)),
                1.0f, "esp_item_small");
        }
        /*
		char idText[16];
		sprintf(idText, "ID: %d", processed.itemId);
		auto idTextSize = renderer.getTextSize(idText, "esp_item_small");
		ImVec2 idTextPos = ImVec2(
			screenPos.x - idTextSize.x * 0.5f,
			screenPos.y - 20.0f - idTextSize.y
		);

		ImVec2 idBgMin(idTextPos.x - 3, idTextPos.y - 1);
		ImVec2 idBgMax(idTextPos.x + idTextSize.x + 3, idTextPos.y + idTextSize.y + 1);
		renderer.drawRectFilled(idBgMin, idBgMax, ImColor(20, 20, 20, int(100 * alphaMultiplier)), 2.0f);

		renderer.drawTextWithShadow(idText, idTextPos,
			ImColor(255, 255, 255, int(200 * alphaMultiplier)),
			ImColor(20, 20, 20, int(200 * alphaMultiplier)),
			1.0f, "esp_item_small");
            */
    }
}

std::string ESP::getWeaponNameById(int weaponId) {
    // Weapon ID to name mapping based on WeaponType namespace
    static std::unordered_map<int, std::string> weaponNames = {
        {WeaponType::MeleeSecondary::EMPTY_HANDED, "Empty Handed"},
        {WeaponType::MeleeSecondary::SURVIVAL_MELEE, "Empty Handed"},
        // Assault Rifles
        {WeaponType::AssaultRifles::RSPN101, "R-301"},
        {WeaponType::AssaultRifles::HEMLOK, "Hemlok"},
        {WeaponType::AssaultRifles::HEMLOK_CRATE, "Hemlok (Crate)"},
        {WeaponType::AssaultRifles::FLATLINE, "Flatline"},
        {WeaponType::AssaultRifles::G2, "G7 Scout"},
        {WeaponType::AssaultRifles::HAVOC, "Havoc"},
        {WeaponType::AssaultRifles::HAVOC_CRATE, "Havoc (Crate)"},
        {WeaponType::AssaultRifles::NEMESIS, "Nemesis"},
        {WeaponType::AssaultRifles::RIFLE_3030, "30-30 Repeater"},

        // LMGs
        {WeaponType::LMGs::DRAGON_LMG, "Spitfire"},
        {WeaponType::LMGs::ESAW, "Devotion"},
        {WeaponType::LMGs::ESAW_CRATE, "Devotion (Crate)"},
        {WeaponType::LMGs::LSTAR, "L-STAR"},
        {WeaponType::LMGs::LSTAR_CRATE, "L-STAR (Crate)"},
        {WeaponType::LMGs::RAMPAGE, "Rampage"},

        // SMGs
        {WeaponType::SMGs::ALTERNATOR, "Alternator"},
        {WeaponType::SMGs::R99, "R-99"},
        {WeaponType::SMGs::R99_CRATE, "R-99 (Crate)"},
        {WeaponType::SMGs::PDW, "Prowler"},
        {WeaponType::SMGs::PDW_CRATE, "Prowler (Crate)"},
        {WeaponType::SMGs::VOLT, "Volt"},
        {WeaponType::SMGs::CAR, "C.A.R."},

        // Shotguns
        {WeaponType::Shotguns::EVA8, "EVA-8"},
        {WeaponType::Shotguns::EVA8_CRATE, "EVA-8 (Crate)"},
        {WeaponType::Shotguns::MASTIFF, "Mastiff"},
        {WeaponType::Shotguns::MASTIFF_CRATE, "Mastiff (Crate)"},
        {WeaponType::Shotguns::PEACEKEEPER, "Peacekeeper"},
        {WeaponType::Shotguns::PK_CRATE, "Peacekeeper (Crate)"},

        // Sniper/Marksman
        {WeaponType::SniperMarksman::SENTINEL, "Sentinel"},
        {WeaponType::SniperMarksman::BOW, "Bocek Bow"},
        {WeaponType::SniperMarksman::KRABER, "Kraber"},
        {WeaponType::SniperMarksman::CHARGE_RIFLE, "Charge Rifle"},
        {WeaponType::SniperMarksman::LONGBOW, "Longbow DMR"},
        {WeaponType::SniperMarksman::TRIPLE_TAKE, "Triple Take"},
        {WeaponType::SniperMarksman::EPG, "EPG"},
        {WeaponType::SniperMarksman::EPG_TETHERED, "EPG (Tethered)"},

        // Pistols
        {WeaponType::Pistols::P2020, "P2020"},
        {WeaponType::Pistols::RE45, "RE-45"},
        {WeaponType::Pistols::RE45_CRATE, "RE-45 (Crate)"},
        {WeaponType::Pistols::WINGMAN, "Wingman"},
        {WeaponType::Pistols::WINGMAN_CRATE, "Wingman (Crate)"},
        {WeaponType::Pistols::MOZAMBIQUE, "Mozambique"},

        // Mozambique variants
        {WeaponType::MozambiqueVariants::MOZAMBIQUE_LIGHT, "Mozambique (Light)"},
        {WeaponType::MozambiqueVariants::MOZAMBIQUE_ENERGY, "Mozambique (Energy)"},
        {WeaponType::MozambiqueVariants::MOZAMBIQUE_SNIPER, "Mozambique (Sniper)"},
        {WeaponType::MozambiqueVariants::MOZAMBIQUE_HEAVY, "Mozambique (Heavy)"},
        {WeaponType::MozambiqueVariants::MOZAMBIQUE_EPG, "Mozambique (EPG)"},

        // Throwables
        {WeaponType::Throwables::FRAG_GRENADE, "Frag Grenade"},
        {WeaponType::Throwables::ARC_STAR, "Arc Star"},
        {WeaponType::Throwables::THERMITE, "Thermite Grenade"},
        {WeaponType::Throwables::REV_SILENCE, "Silence"},
        {WeaponType::Throwables::THROWING_KNIFE, "Throwing Knife"},

        // Special Weapons
        {WeaponType::SpecialWeapons::TITAN_SWORD, "Titan Sword"},
        {WeaponType::SpecialWeapons::MOUNTED_TURRET, "Mounted Turret"},
        {WeaponType::SpecialWeapons::TURRET_PLACEABLE, "Placeable Turret"},
        {WeaponType::SpecialWeapons::POWER_SWORD, "Power Sword"}
    };

    auto it = weaponNames.find(weaponId);
    if (it != weaponNames.end()) {
        // Return the translated weapon name if available, otherwise return the English name
        return t_(it->second);
    }
    return t_("None");
}

void ESP::run() {
    // 检查按键状态
    if (keys.holdMode) {
		espEnabled = misc->uiParams.useController ? isKeyHeld("holdController") : isKeyHeld("hold");
    }
    else {
		if (misc->uiParams.useController && isKeyToggled("toggleController")) {
			// 控制器模式
			espEnabled = !espEnabled;
		}
        else {
            // 切换模式
            if (isKeyToggled("toggle")) {
                espEnabled = !espEnabled;
            }
        }
    }

    // 检查总开关
    if (!espEnabled) return;

    ImGuiRenderer& renderer = ImGuiRenderer::getInstance();

    // 处理玩家ESP
    const auto& entities = entityManager.getPlayers();

    if (!entities->empty()) {
        for (const auto& player : *entities) {
            // 跳过无效玩家
            if (!player || !player->valid || player->IsDead || player->IsKnocked) {
                continue;
            }

            // 跳过本地玩家
            if (player->IsLocal || player->Distance2DToLocalPlayer < 1.f) {
                continue;
            }

            // 根据玩家类型选择对应设置
            if (player->IsDummy()) {
                // 机器人
                drawPlayerESP(player, renderer, bots);
            }
            else if (player->IsTeamMate) {
                // 队友
                drawPlayerESP(player, renderer, teammates);
            }
            else {
                // 敌人
                drawPlayerESP(player, renderer, enemies);
            }
        }

        skeletonSystem->Render(ImGui::GetBackgroundDrawList());
    }


    RenderItemESP();
}

void ESP::renderMenu() {
    ImGui::Text("%s", t_("ESP SETTINGS"));

    ImGui::Separator();
    ImGui::Spacing();

    // 添加顶部水平导航 - 简化版本
    static int espTab = 0; // 0=General, 1=Enemy, 2=Teammate, 3=Bot, 4=Item, 5=Offscreen

    // 创建简洁的标签
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.18f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.30f, 0.40f, 1.0f));

    // Horizontal tab arrangement
    if (ImGui::Button(t_("Global"), ImVec2(70, 36))) espTab = 0;
    ImGui::SameLine();
    if (ImGui::Button(t_("Enemy"), ImVec2(70, 36))) espTab = 1;
    ImGui::SameLine();
    if (ImGui::Button(t_("Teammate"), ImVec2(70, 36))) espTab = 2;
    ImGui::SameLine();
    if (ImGui::Button(t_("Bot"), ImVec2(70, 36))) espTab = 3;
    ImGui::SameLine();
    if (ImGui::Button(t_("Item"), ImVec2(70, 36))) espTab = 4;
    ImGui::SameLine();
    if (ImGui::Button(t_("Categories"), ImVec2(70, 36))) espTab = 5;
    ImGui::SameLine();
    if (ImGui::Button(t_("Offscreen"), ImVec2(70, 36))) espTab = 6;

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // 根据选择的标签页显示不同内容
    switch (espTab) {
    case 0: // General
        renderGeneralTab();
        break;
    case 1: // Enemy
        renderEnemyTab();
        break;
    case 2: // Teammate
        renderTeammateTab();
        break;
    case 3: // Bot
        renderBotTab();
        break;
    case 4: // Item
        renderItemTab();
        break;
    case 5: // Item Categories
        renderItemCategoriesTab();
        break;
    case 6: // Offscreen
        renderOffscreenTab();
        break;
    }
}

void ESP::renderGeneralTab() {
    ImGui::BeginChild("GeneralContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 主开关 - 大号开关
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable ESP"), &espEnabled);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 基本设置组
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("ESP Categories"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Enable Enemy ESP"), &enemies.enabled);
    ImGui::Checkbox(t_("Enable Teammate ESP"), &teammates.enabled);
    ImGui::Checkbox(t_("Enable Bot ESP"), &bots.enabled);
    ImGui::Checkbox(t_("Show Spectator"), &spectator->uiParams.enabled);

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // 渲染按键绑定设置
    renderKeyBindings();

    ImGui::EndChild();

    ImGui::EndChild(); // GeneralContent
}

void ESP::renderKeyBindings() {
    // 按键绑定组
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Key Bindings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    auto& renderer = ImGuiRenderer::getInstance();

    // 按键模式
    ImGui::Checkbox(t_("Hold Mode"), &keys.holdMode);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Hold Mode: ESP is active only while the key is held down"));
        ImGui::Text("%s", t_("Toggle Mode: Press the key to toggle ESP on/off"));
        ImGui::EndTooltip();
    }

    ImGui::Spacing();

    if (keys.holdMode) {
        if(!misc->uiParams.useController)
			renderer.renderKeyBinding(t_("Hold Key"), &keys.holdKey);
        else
			renderer.renderGamepadBinding(t_("Hold Key"), &keys.holdKeyController);
    }
    else {
        if (!misc->uiParams.useController)
            renderer.renderKeyBinding(t_("Toggle Key"), &keys.toggleKey);
        else
            renderer.renderGamepadBinding(t_("Toggle Key"), &keys.toggleKeyController);
    }

    ImGui::Spacing();

    // 更新按键
    updateKeys();
}

void ESP::renderEnemyTab() {
    ImGui::BeginChild("EnemyContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 敌人ESP - 大号开关
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Enemy ESP"), &enemies.enabled);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 敌人显示设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Display Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Show Name"), &enemies.showName);
    ImGui::Checkbox(t_("Show Distance"), &enemies.showDistance);
    ImGui::Checkbox(t_("Show Level"), &enemies.showLevel);
    ImGui::Checkbox(t_("Show Rank"), &enemies.showRank);
    ImGui::Checkbox(t_("Show Weapon"), &enemies.showWeapon);
    ImGui::Checkbox(t_("Show Health Bar"), &enemies.showHealth);
    ImGui::Checkbox(t_("Show Skeleton"), &enemies.showSkeleton);
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Animation Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Enable Animation"), &enemies.animation);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("When enabled, ESP elements fade out when close to screen center"));
        ImGui::Text("%s", t_("This helps prevent blocking your view during combat"));
        ImGui::EndTooltip();
    }

    if (enemies.animation) {
        ImGui::SliderFloat(t_("Min Distance"), &enemies.animationMinDistance, 10.0f, 200.0f, "%.0f px");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("ESP elements closer than this to screen center become completely transparent"));
            ImGui::EndTooltip();
        }

        ImGui::SliderFloat(t_("Max Distance"), &enemies.animationMaxDistance, 100.0f, 200.f, "%.0f px");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("ESP elements further than this from screen center are fully visible"));
            ImGui::EndTooltip();
        }

        ImGui::SliderFloat(t_("Fade Scale"), &enemies.animationFadeScale, 0.5f, 2.0f, "%.2f");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("Adjusts how quickly ESP fades in based on distance"));
            ImGui::Text("%s", t_("Higher values = faster fade in"));
            ImGui::EndTooltip();
        }
    }

    ImGui::Spacing();

    // 边框风格
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Box Style"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* boxStyles[] = { t_("None"), t_("2D Box"), t_("Corner Box") };
    int currentStyle = static_cast<int>(enemies.boxStyle);
    if (ImGui::Combo(t_("Box Style"), &currentStyle, boxStyles, IM_ARRAYSIZE(boxStyles))) {
        enemies.boxStyle = static_cast<PlayerESPSettings::BoxStyle>(currentStyle);
    }

    // 健康条样式
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Health Bar Style"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* healthBarStyles[] = { t_("Seer"), t_("Classic"), t_("Custom") };
    int currentHealthBarStyle = static_cast<int>(enemies.healthBarStyle);
    if (ImGui::Combo(t_("Health Bar Style"), &currentHealthBarStyle, healthBarStyles, IM_ARRAYSIZE(healthBarStyles))) {
        enemies.healthBarStyle = static_cast<PlayerESPSettings::HealthBarStyle>(currentHealthBarStyle);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // 距离设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Distance Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::SliderFloat(t_("Max Distance"), &enemies.distance, 50.0f, 500.0f, "%.0f meters");
    ImGui::SliderFloat(t_("Near Distance"), &enemies.nearDistance, 10.0f, 100.0f, "%.0f meters");

    ImGui::Spacing();
    ImGui::Spacing();

    // 颜色设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Color Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Color By Health"), &enemies.colorByHealth);

    if (!enemies.colorByHealth) {
        ImGui::Text("%s:", t_("Visible Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##VisibleColor", (float*)&enemies.visibleColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

        ImGui::Text("%s:", t_("Invisible Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##InvisibleColor", (float*)&enemies.invisibleColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    }

    ImGui::Text("%s:", t_("Skeleton Visible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##SkeVisibleColor", (float*)&enemies.visibleSkeletonColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Skeleton Invisible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##SkeInvisibleColor", (float*)&enemies.invisibleSkeletonColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Pro Player Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##ProPlayerColor", (float*)&enemies.proPlayerColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Text Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##TextColor", (float*)&enemies.textColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::SliderFloat(t_("ESP Thickness"), &enemies.espThickness, 1.f, 5.f, "%.0f px");
    ImGui::SliderFloat(t_("Skeleton Thickness"), &enemies.skeletonThickness, 1.f, 5.f, "%.0f px");
    ImGui::EndChild();

    ImGui::EndChild(); // EnemyContent
}

void ESP::renderItemTab() {
    ImGui::BeginChild("ItemContent", ImVec2(0, 0), false);

    // Left and right panel layout
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // Left panel
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // Main Item ESP toggle with larger style
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Item ESP"), &items.globalEnabled);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // Item display settings
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Global Display Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Show All Names"), &items.showName);
    ImGui::Checkbox(t_("Show All Distances"), &items.showDistance);
    ImGui::Checkbox(t_("Show All Icons"), &items.showIcon);
    ImGui::Checkbox(t_("Enable Animation"), &items.animation);
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Animation helps prevent screen clutter near crosshair"));
        ImGui::Text("%s", t_("Items fade in based on distance from screen center"));
        ImGui::Text("%s", t_("This prevents cluttering around your aim point"));
        ImGui::EndTooltip();
    }

    // Item style
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Display Style"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* itemStyles[] = { t_("None"), t_("Text Only"), t_("Icon Only"), t_("Text and Icon") };
    ImGui::Combo(t_("Style"), &items.style, itemStyles, IM_ARRAYSIZE(itemStyles));

    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // Distance and animation settings
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Distance Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::SliderFloat(t_("Maximum Distance"), &items.maxDistance, 10.0f, 500.0f, "%.0f meters");

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Animation Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::SliderFloat(t_("FOV"), &items.fov, 10.0f, 200.0f, "%.0f");
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Controls fade-in area size around crosshair"));
        ImGui::EndTooltip();
    }

    ImGui::SliderFloat(t_("Scale"), &items.scale, 0.1f, 1.0f, "%.2f");
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Controls fade threshold - lower values show more items near crosshair"));
        ImGui::EndTooltip();
    }

    ImGui::EndChild();

    ImGui::EndChild(); // ItemContent
}

void ESP::renderOffscreenTab() {
    ImGui::BeginChild("OffscreenContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 屏幕外指示器 - 大号开关
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Offscreen Indicators"), &offScreenIndicator.enabled);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 屏幕外指示器设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Indicator Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Show Distance"), &offScreenIndicator.showDistance);
    ImGui::Checkbox(t_("Only Enemies"), &offScreenIndicator.enemyOnly);

    // 新增设置
    ImGui::SliderFloat(t_("Indicator Radius"), &offScreenIndicator.indicatorRadius, 0.1f, 0.9f, "%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Controls how far from screen center the indicators appear"));
        ImGui::Text("%s", t_("Value is percentage of screen size"));
        ImGui::EndTooltip();
    }

    ImGui::SliderFloat(t_("Arrow Size"), &offScreenIndicator.arrowSize, 5.0f, 20.0f, "%.1f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Size of the triangle indicator"));
        ImGui::EndTooltip();
    }

    ImGui::SliderFloat(t_("Arrow Angle"), &offScreenIndicator.arrowAngleWidth, 0.5f, 3.0f, "%.1f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Controls how wide the arrow appears"));
        ImGui::Text("%s", t_("Higher values = wider arrow"));
        ImGui::EndTooltip();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // 距离设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Distance Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::SliderFloat(t_("Max Distance"), &offScreenIndicator.maxDistance, 20.0f, 300.0f, "%.0f meters");

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Checkbox(t_("Dynamic Radius"), &offScreenIndicator.dynamicRadius);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("Indicator radius changes based on enemy distance"));
        ImGui::Text("%s", t_("Closer enemies have larger radius indicators"));
        ImGui::EndTooltip();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 颜色设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Color Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("%s:", t_("Indicator Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##IndicatorColor", (float*)&offScreenIndicator.indicatorColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::EndChild();

    ImGui::EndChild(); // OffscreenContent
}

void ESP::renderItemCategoriesTab() {
    ImGui::BeginChild("ItemCategoriesContent", ImVec2(0, 0), false);

    auto categoryNames = GetItemCategoryNames();

    // Help text
    ImGui::TextWrapped("%s", t_("Configure what items to display and their colors. You can enable/disable entire categories or individual items."));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Category list
    int categoryIndex = 0;
    auto listsIter = Lists.begin();

    // 确保我们有正确数量的分类名称
    if (categoryNames.size() != std::distance(Lists.begin(), Lists.end())) {
        ImGui::EndChild();
        return;
    }

    for (const auto& categoryName : categoryNames) {
        if (categoryIndex >= categorySettings.size() || listsIter == Lists.end()) {
            break;
        }

        const auto& categoryItems = *listsIter;

        // Category header with color and enable checkbox
        ImGui::PushID(categoryIndex);

        // Use the category color for the tree node text
        ImGui::PushStyleColor(ImGuiCol_Text,
            categorySettings[categoryIndex].color
        );

        bool categoryOpen = ImGui::TreeNodeEx(
            t_(categoryName),
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
        );

        ImGui::PopStyleColor();

        // Category controls on same line
        ImGui::Checkbox(t_("Enable"), &categorySettings[categoryIndex].enabled);

        ImGui::SameLine();
        ImGui::ColorEdit4("##CategoryColor", (float*)&categorySettings[categoryIndex].color,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

        if (categoryOpen) {
            // Display items in this category
            ImGui::Separator();

            ImGui::BeginChild(("Category_" + std::to_string(categoryIndex)).c_str(),
                ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 16), true);

            int itemIndex = 0;
            int columnCount = 0;
            const int MAX_COLUMNS = 2; // 减少列数，从3列改为2列，让物品显示更清晰

            for (const auto& item : categoryItems) {
                const auto& [id, name, icon] = item;

                // Get or create item settings
                bool settingExists = specificItemSettings.find(id) != specificItemSettings.end();
                if (!settingExists) {
                    ItemSpecificSetting defaultSetting;
                    defaultSetting.enabled = true;
                    defaultSetting.useCustomColor = false;
                    defaultSetting.customColor = categorySettings[categoryIndex].color;
                    defaultSetting.showName = items.showName;
                    defaultSetting.showDistance = items.showDistance;
                    defaultSetting.showIcon = items.showIcon;
                    specificItemSettings[id] = defaultSetting;
                }

                auto& itemSetting = specificItemSettings[id];

                // 修改列布局逻辑，使其更清晰明确
                if (columnCount > 0) {
                    ImGui::SameLine(columnCount * (ImGui::GetContentRegionAvail().x / MAX_COLUMNS));
                }

                // Item controls
                ImGui::PushID(id);

                // Enable checkbox
                ImGui::Checkbox(t_(name), &itemSetting.enabled);

                // Custom color checkbox (icon version)
                ImGui::SameLine();
                ImGui::Checkbox("##CustomColor", &itemSetting.useCustomColor);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(t_("Use custom color"));
                }

                // Color picker (only visible if custom color is enabled)
                if (itemSetting.useCustomColor) {
                    ImGui::SameLine();
                    ImGui::ColorEdit4("##ItemColor", (float*)&itemSetting.customColor,
                        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                }

                ImGui::PopID();

                itemIndex++;
                columnCount = (columnCount + 1) % MAX_COLUMNS;
                if (columnCount == 0) {
                    ImGui::Separator();
                }
            }

            ImGui::EndChild();
            ImGui::TreePop();
        }

        ImGui::PopID();
        ImGui::Spacing();

        // 移动到下一个分类
        categoryIndex++;
        ++listsIter;
    }

    ImGui::EndChild(); // ItemCategoriesContent
}
void ESP::renderTeammateTab() {
    ImGui::BeginChild("TeammateContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 队友ESP - 大号开关
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Teammate ESP"), &teammates.enabled);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 队友显示设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Display Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Show Name"), &teammates.showName);
    ImGui::Checkbox(t_("Show Distance"), &teammates.showDistance);
    ImGui::Checkbox(t_("Show Level"), &teammates.showLevel);
    ImGui::Checkbox(t_("Show Rank"), &teammates.showRank);
    ImGui::Checkbox(t_("Show Weapon"), &teammates.showWeapon);
    ImGui::Checkbox(t_("Show Health Bar"), &teammates.showHealth);
    ImGui::Checkbox(t_("Show Skeleton"), &teammates.showSkeleton);
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Animation Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Enable Animation"), &teammates.animation);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("When enabled, ESP elements fade out when close to screen center"));
        ImGui::Text("%s", t_("This helps prevent blocking your view during combat"));
        ImGui::EndTooltip();
    }

    if (teammates.animation) {
        ImGui::SliderFloat(t_("Min Distance"), &teammates.animationMinDistance, 10.0f, 200.0f, "%.0f px");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("ESP elements closer than this to screen center become completely transparent"));
            ImGui::EndTooltip();
        }

        ImGui::SliderFloat(t_("Max Distance"), &teammates.animationMaxDistance, 100.0f, 200.f, "%.0f px");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("ESP elements further than this from screen center are fully visible"));
            ImGui::EndTooltip();
        }

        ImGui::SliderFloat(t_("Fade Scale"), &teammates.animationFadeScale, 0.5f, 2.0f, "%.2f");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("Adjusts how quickly ESP fades in based on distance"));
            ImGui::Text("%s", t_("Higher values = faster fade in"));
            ImGui::EndTooltip();
        }
    }

    ImGui::Spacing();

    // 边框风格
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Box Style"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* boxStyles[] = { t_("None"), t_("2D Box"), t_("Corner Box") };
    int currentStyle = static_cast<int>(teammates.boxStyle);
    if (ImGui::Combo(t_("Box Style"), &currentStyle, boxStyles, IM_ARRAYSIZE(boxStyles))) {
        teammates.boxStyle = static_cast<PlayerESPSettings::BoxStyle>(currentStyle);
    }

    // 健康条样式
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Health Bar Style"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* healthBarStyles[] = { t_("Seer"), t_("Classic"), t_("Custom") };
    int currentHealthBarStyle = static_cast<int>(teammates.healthBarStyle);
    if (ImGui::Combo(t_("Health Bar Style"), &currentHealthBarStyle, healthBarStyles, IM_ARRAYSIZE(healthBarStyles))) {
        teammates.healthBarStyle = static_cast<PlayerESPSettings::HealthBarStyle>(currentHealthBarStyle);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // 距离设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Distance Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::SliderFloat(t_("Max Distance"), &teammates.distance, 50.0f, 500.0f, "%.0f meters");
    ImGui::SliderFloat(t_("Near Distance"), &teammates.nearDistance, 10.0f, 100.0f, "%.0f meters");

    ImGui::Spacing();
    ImGui::Spacing();

    // 颜色设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Color Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Color By Health"), &teammates.colorByHealth);

    if (!teammates.colorByHealth) {
        ImGui::Text("%s:", t_("Visible Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##VisibleColor", (float*)&teammates.visibleColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

        ImGui::Text("%s:", t_("Invisible Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##InvisibleColor", (float*)&teammates.invisibleColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    }
    ImGui::Text("%s:", t_("Skeleton Visible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##SkeVisibleColor", (float*)&teammates.visibleSkeletonColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Skeleton Invisible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##SkeInvisibleColor", (float*)&teammates.invisibleSkeletonColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    ImGui::Text("%s:", t_("Pro Player Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##ProPlayerColor", (float*)&teammates.proPlayerColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Text Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##TextColor", (float*)&teammates.textColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::SliderFloat(t_("ESP Thickness"), &teammates.espThickness, 1.f, 5.f, "%.0f px");
    ImGui::SliderFloat(t_("Skeleton Thickness"), &teammates.skeletonThickness, 1.0f, 5.f, "%.0f px");
    ImGui::EndChild();

    ImGui::EndChild(); // TeammateContent
}

void ESP::renderBotTab() {
    ImGui::BeginChild("BotContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 机器人ESP - 大号开关
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Bot ESP"), &bots.enabled);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 机器人显示设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Display Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Show Name"), &bots.showName);
    ImGui::Checkbox(t_("Show Distance"), &bots.showDistance);
    ImGui::Checkbox(t_("Show Level"), &bots.showLevel);
    ImGui::Checkbox(t_("Show Rank"), &bots.showRank);
    ImGui::Checkbox(t_("Show Weapon"), &bots.showWeapon);
    ImGui::Checkbox(t_("Show Health Bar"), &bots.showHealth);
    ImGui::Checkbox(t_("Show Skeleton"), &bots.showSkeleton);
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Animation Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Enable Animation"), &bots.animation);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", t_("When enabled, ESP elements fade out when close to screen center"));
        ImGui::Text("%s", t_("This helps prevent blocking your view during combat"));
        ImGui::EndTooltip();
    }

    if (bots.animation) {
        ImGui::SliderFloat(t_("Min Distance"), &bots.animationMinDistance, 10.0f, 200.0f, "%.0f px");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("ESP elements closer than this to screen center become completely transparent"));
            ImGui::EndTooltip();
        }

        ImGui::SliderFloat(t_("Max Distance"), &bots.animationMaxDistance, 100.0f, 200.f, "%.0f px");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("ESP elements further than this from screen center are fully visible"));
            ImGui::EndTooltip();
        }

        ImGui::SliderFloat(t_("Fade Scale"), &bots.animationFadeScale, 0.5f, 2.0f, "%.2f");
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", t_("Adjusts how quickly ESP fades in based on distance"));
            ImGui::Text("%s", t_("Higher values = faster fade in"));
            ImGui::EndTooltip();
        }
    }

    ImGui::Spacing();

    // 边框风格
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Box Style"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* boxStyles[] = { t_("None"), t_("2D Box"), t_("Corner Box") };
    int currentStyle = static_cast<int>(bots.boxStyle);
    if (ImGui::Combo(t_("Box Style"), &currentStyle, boxStyles, IM_ARRAYSIZE(boxStyles))) {
        bots.boxStyle = static_cast<PlayerESPSettings::BoxStyle>(currentStyle);
    }

    // 健康条样式
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Health Bar Style"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* healthBarStyles[] = { t_("Seer"), t_("Classic"), t_("Custom") };
    int currentHealthBarStyle = static_cast<int>(bots.healthBarStyle);
    if (ImGui::Combo(t_("Health Bar Style"), &currentHealthBarStyle, healthBarStyles, IM_ARRAYSIZE(healthBarStyles))) {
        bots.healthBarStyle = static_cast<PlayerESPSettings::HealthBarStyle>(currentHealthBarStyle);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // 距离设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Distance Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::SliderFloat(t_("Max Distance"), &bots.distance, 50.0f, 500.0f, "%.0f meters");
    ImGui::SliderFloat(t_("Near Distance"), &bots.nearDistance, 10.0f, 100.0f, "%.0f meters");

    ImGui::Spacing();
    ImGui::Spacing();

    // 颜色设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Color Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox(t_("Color By Health"), &bots.colorByHealth);

    if (!bots.colorByHealth) {
        ImGui::Text("%s:", t_("Visible Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##VisibleColor", (float*)&bots.visibleColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

        ImGui::Text("%s:", t_("Invisible Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##InvisibleColor", (float*)&bots.invisibleColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    }
    ImGui::Text("%s:", t_("Skeleton Visible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##SkeVisibleColor", (float*)&bots.visibleSkeletonColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("%s:", t_("Skeleton Invisible Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##SkeInvisibleColor", (float*)&bots.invisibleSkeletonColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    ImGui::Text("%s:", t_("Text Color"));
    ImGui::SameLine();
    ImGui::ColorEdit4("##TextColor", (float*)&bots.textColor,
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::SliderFloat(t_("ESP Thickness"), &bots.espThickness, 1.f, 5.f, "%.0f px");
    ImGui::SliderFloat(t_("Skeleton Thickness"), &bots.skeletonThickness, 1.0f, 5.f, "%.0f px");

    ImGui::EndChild();

    ImGui::EndChild(); // BotContent
}