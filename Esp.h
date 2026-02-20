#pragma once
#include <atomic>
#include <thread>
#include <vector>
#include <unordered_map>
#include "Config.h"
#include "VecDef.h"
#include "Player.h"
#include "Item.h"
#include "ImGuiRender.h"

class ESP : public AutoConfigurable {
public:

    bool drawFPS = true;

    struct UISettings
    {
        int menuKey = VK_INSERT;
		int endKey = VK_END;
		int fusionKey = VK_HOME;
		int radarKey = VK_F10;
        int menuKeyController = 0;
		int endKeyController = 0;
		int fusionKeyController = 0;
		int radarKeyController = 0;
		int languageSelected = 0; //0 = English, 1 = Simplified Chinese, 2 = Traditional Chinese, 3 = Japanese
		int themeSelected = 2; 
    } uiSettings;

    // 玩家ESP设置结构
    struct PlayerESPSettings {
        bool enabled = true;          // 该分类是否启用
        float distance = 200.0f;      // 最大显示距离
        bool showName = true;         // 显示姓名
        bool showDistance = true;     // 显示距离
        bool showLevel = true;        // 显示等级
        bool showRank = true;         // 显示段位
        bool showWeapon = true;       // 显示武器
        bool showHealth = true;       // 显示生命值
        bool showSkeleton = true;     // 显示骨骼
        bool colorByHealth = false;    // 根据血量渐变颜色
        ImVec4 boxColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // 边框颜色
        ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 文字颜色
        ImVec4 visibleColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);     // 可见时颜色
        ImVec4 visibleSkeletonColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);     // 可见时骨骼颜色
        ImVec4 invisibleColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);   // 不可见时颜色
        ImVec4 invisibleSkeletonColor = ImVec4(1.0f, 1.f, 1.f, 1.0f);   // 不可见时骨骼颜色
        ImVec4 proPlayerColor = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);   // 专业玩家颜色
        float nearDistance = 50.0f;   // 近距离阈值

        float espThickness = 1.f;
        float skeletonThickness = 1.f;

        bool animation = true;          // 开启动画效果（基于屏幕中心距离的透明度）
        float animationMaxDistance = 100.f; // 最大距离阈值，超过此距离显示完全不透明
        float animationMinDistance = 20.f;  // 最小距离阈值，小于此距离完全透明
        float animationFadeScale = 1.0f;     // 淡入淡出效果的缩放因子

        // 边框样式
        enum class BoxStyle {
            None = 0,
            Box2D = 1,
            Corner = 2
        } boxStyle = BoxStyle::Corner;

        // 血条样式
        enum class HealthBarStyle {
            Seer = 0,      // Seer风格
            Classic = 1,   // 经典风格
            Custom = 2     // 自定义风格
        } healthBarStyle = HealthBarStyle::Seer;
    };

    // 物品分类设置结构 (新增)
    struct ItemCategorySetting {
        bool enabled = true;                // 启用状态
        ImVec4 color;                       // 分类颜色
    };

    // 物品特定设置结构 (新增)
    struct ItemSpecificSetting {
        bool enabled = true;                // 启用状态
        bool useCustomColor = false;        // 是否使用自定义颜色
        ImVec4 customColor;                 // 自定义颜色
        bool showName = true;               // 显示名称
        bool showDistance = true;           // 显示距离
        bool showIcon = true;               // 显示图标
    };

    // 物品ESP设置结构 (修改)
    struct ItemESPSettings {
        bool globalEnabled = true;          // 全局开关
        float maxDistance = 100.f;          // 最大显示距离
        bool animation = true;              // 动画效果(防止遮挡准心)
        float fov = 100.0f;                 // 视野范围
        float scale = 0.6f;                 // 缩放比例
        int style = 2;                      // 显示样式 (0=无, 1=仅文本, 2=仅图标, 3=文本和图标)
        bool showName = true;               // 全局显示名称设置
        bool showDistance = true;           // 全局显示距离设置
        bool showIcon = true;               // 全局显示图标设置
    };

    // 屏幕外指示器设置
    struct OffScreenIndicatorSettings {
        bool enabled = true;          // 是否启用
        float maxDistance = 100.0f;   // 最大显示距离
        bool showDistance = true;     // 显示距离
        bool enemyOnly = true;        // 仅显示敌人
        float indicatorRadius = 0.4f; // 指示器半径（屏幕大小的百分比）
        float arrowSize = 10.0f;      // 箭头大小
        float arrowAngleWidth = 2.5f; // 箭头展开角度
        bool dynamicRadius = true;    // 动态距离
        ImVec4 indicatorColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // 指示器颜色
    };

    // 键位设置
    struct KeySettings {
        int toggleKey = 0;        // 切换ESP开关的按键
        int toggleKeyController = 0;        // 切换ESP开关的按键
        int holdKey = 0;    // 按住启用ESP的按键
        int holdKeyController = 0;    // 按住启用ESP的按键
        bool holdMode = false;        // 是否为按住模式
    };

    // 主要设置组
    PlayerESPSettings enemies;        // 敌人ESP设置
    PlayerESPSettings teammates;      // 队友ESP设置
    PlayerESPSettings bots;           // 机器人ESP设置
    ItemESPSettings items;            // 物品ESP全局设置
    OffScreenIndicatorSettings offScreenIndicator; // 屏幕外指示器设置
    KeySettings keys;                 // 按键设置
    bool espEnabled = true;           // ESP总开关

    // 物品分类设置 (新增)
    std::vector<ItemCategorySetting> categorySettings;

    // 物品特定设置 (新增) - 物品ID到设置的映射
    std::unordered_map<int, ItemSpecificSetting> specificItemSettings;

    // 功能方法
    // 获取屏幕边框
    std::pair<Vec2D, Vec2D> GetBounds(Vec3D min, Vec3D max);

    // 主ESP绘制函数
    void run();

    // 渲染菜单
    void renderMenu();

    // 按键更新
    void updateKeys();

    // 获取玩家颜色
    ImColor getPlayerColor(const Player* player, const PlayerESPSettings& settings);

    // 绘制屏幕外指示器
    void drawOffScreenIndicator(const Player* player, ImGuiRenderer& renderer);

    // 绘制玩家ESP
    void drawPlayerESP(const Player* player, ImGuiRenderer& renderer, const PlayerESPSettings& settings);

    // 绘制健康和护甲条
    void drawHealthAndShield(const Player* player, ImVec2 topLeft, ImVec2 bottomRight, const PlayerESPSettings& settings);

    // 物品ESP方法 (修改)
    void RenderItemESP();
    std::vector<std::string> GetItemCategoryNames() const;

    void renderItemCategoriesTab();

    // 获取物品设置
    const ItemSpecificSetting& GetItemSettings(int itemId, size_t categoryIndex);

    // 检查物品是否应该显示
    bool ShouldDisplayItem(int itemId, size_t categoryIndex);

    // 获取物品颜色
    ImVec4 GetItemColor(int itemId, size_t categoryIndex, float alphaMultiplier = 1.0f);

    // 构造和析构函数
    ESP();
    ~ESP();

private:
    // UI相关方法
    void renderGeneralTab();
    void renderEnemyTab();
    void renderTeammateTab();
    void renderBotTab();
    void renderItemTab();
    void renderOffscreenTab();
    void renderKeyBindings();

    // 内部辅助方法
    bool isKeyHeld(const std::string& keyName);
    bool isKeyToggled(const std::string& keyName);
    bool isPointOnScreen(const Vec2D& point);
    std::string getWeaponNameById(int weaponId);

    static const ImVec4 defaultColors[];
    void initItemColors();
    void initializeCategorySettings();
};