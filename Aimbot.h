#pragma once 
#include <chrono>
#include <imgui.h>
#include "Config.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "VecDef.h"
#include "AimAngle.h"
#include "HitBox.h"
#include "TriggerBot.h"
#include "Skynade.h" // SkyNade 功能接口

class Aimbot : public AutoConfigurable {
public:
    // 基础瞄准配置
    struct AimConfig {
        float smooth = 10.0f;
        float fov = 15.0f;
        float recoilX = 1.0f;
        float recoilY = 1.0f;
    };

    // 通用瞄准参数
    struct {
        AimConfig noScope;       // 未开镜配置
        AimConfig scoped{ 9.f, 12.f, 1.2f, 1.2f };  // 开镜配置
        float switchSmoothScale = 2.0f;  // 切换目标时的平滑缩放
        float shootingSmoothScale = 0.8f; // 射击时的平滑缩放
        bool autoAimBone = true;        // 是否自动选择骨骼
        bool showFOV = true;            // 是否显示 FOV 圆
        float fovThickness = 1.0f;      // FOV 圆线宽
        float maxDistance = 300.0f;     // 最大瞄准距离
        float minDistance = 1.0f;       // 最小瞄准距离
        bool enabled = true;            // 瞄准功能开关
        int aimBone = 0;                // 默认瞄准骨骼
        int aimType = 0;                // 0=Memory, 1=KMBOX
        bool shieldCheck = false;
    } aimParams;

    // SkyNade 设置
    struct {
        bool enabled = true;           // 是否启用
        float smooth = 10.f;            // 瞄准平滑
        float fov = 10.0f;              // FOV 范围
        int key = 0;                    // 键鼠热键
        int keyController = 0;          // 手柄热键
        ImVec4 indicatorColor = ImVec4(0.0f, 1.0f, 0.5f, 0.8f); // 指示器颜色
        bool showIndicator = true;      // 是否显示指示器
    } skyNadeParams;

    // 目标管理
    struct {
        Player* target;           // 当前目标
        float lastSwitchTime;     // 上次切换时间
        float switchCooldown;     // 切换冷却时间
        float switchSmoothDuration; // 切换平滑持续时长
    } targetSystem;

    // 快捷键绑定
    struct {
        int primary;      // 主瞄准键
        int primaryController;      // 手柄主瞄准键
        int secondary;    // 次级瞄准键
        int secondaryController;    // 手柄次级瞄准键
        int head;         // 强制头部键
        int headController;         // 手柄强制头部键
        int trigger;         // 触发键
        int triggerController;         // 手柄触发键
        int triggerAuto;         // 自动触发键
        int triggerAutoController;         // 手柄自动触发键
    } keys;

    // 视觉设置
    struct {
        ImVec4 normalColor = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);  // 普通状态 FOV 颜色
        ImVec4 targetColor = ImVec4(1.0f, 0.2f, 0.2f, 0.7f);  // 锁定目标 FOV 颜色
    } visualSettings;

    // 预测配置
    struct {
        bool enabled = true;        // 是否启用预测
        float scale = 1.0f;         // 修正倍率
        bool showDebug = false;     // 显示调试信息
        int predictionSystem = 0;   // 预测系统：0=AimCorrection，1=自定义
        float cacheTimeout = 0.016f; // 缓存失效时间（秒）
    } predictionSettings;

    // 目标筛选与优先级
    struct {
        int priorityMode = 1;       // 0=距离, 1=FOV, 2=骨骼
        bool targetEnemies = true;  // 锁定敌人
        bool targetTeammates = false; // 锁定队友
        bool targetBots = true;     // 包含 AI
        bool visibleOnly = true;    // 仅瞄准可见目标
        bool ignoreKnockedDown = true; // 忽略倒地目标
    } targetSettings;

    // 目标类型设置
    struct {
        int defaultBones[4] = { 0, 0, 1 }; // 各目标类型的默认骨骼配置
        int currentTargetType = 0;  // 当前目标类型
    } targetTypeSettings;

    struct {
        bool enabled = true;              // 是否启用人性化处理
        int type = 2;
        float microAdjustScale = 0.05f;    // 微调幅度 (0.0-1.0)
        float nonLinearPathStrength = 0.15F; // 非线性路径强度 (0.0-1.0)
        float timeVariationMin = 0.8f;    // 更新间隔下限
        float timeVariationMax = 1.2f;    // 更新间隔上限

        bool enablePitchAdjust = false;    // 是否允许俯仰轴调整
        bool enableYawAdjust = true;      // 是否允许偏航轴调整

        int humanizeMode = 0;
        float curveOffsetMin = 5.0f;
        float curveOffsetMax = 15.0f;

        // 贝塞尔曲线状态
        float bezierProgress = 0.0f;      // 当前贝塞尔进度
        float bezierSpeed = 0.8f;        // 贝塞尔推进速度
        AimAngle lastTargetAngle;         // 上一次目标角度

        // 锁定提示
        bool showLockIndicator = true;   // 是否显示锁定指示
        float indicatorSize = 8.0f;      // 指示器尺寸
        ImVec4 indicatorColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 指示器颜色

        // 更新时间控制
        float lastUpdateTime = 0.0f;     // 上次更新时间
        float nextUpdateDelay = 0.0f;    // 下一次更新延迟
    } humanParams;

    Vec3D skyNadePointCache;
    // TriggerBot 状态标记
    bool isTriggering = false;

    bool isSkyNadeActive = false;

    // 构造函数
    Aimbot();

    // 核心接口
    void registerHotKeys();
    void updateKeys();
    void run();
    void RunAimbot();

    Vec2D ApplySensitivityCorrection(Vec2D step);

    Vec3D AimBotHumanizeMode1(Vec3D EnemyDelta, float AccuracyMin, float AccuracyMax);
    Vec3D ApplyHumanizationToAim(Vec3D targetPos, Vec3D currentPos, int humanizeMode);
    AimAngle CalculateBezierPoint(const AimAngle& start, const AimAngle& control, const AimAngle& end, float t);
    // 对瞄准角度做微调
    void ApplyMicroAdjustments(AimAngle& angle);
    // 判断是否需要刷新瞄准
    bool ShouldUpdateAim();

    // 目标搜索
    Player* FindBestTarget();
    float CalculateTargetScore(Player* player, float bestFov);
    bool IsValidTarget(Player* player);

    // 锁定管理
    void StartAiming(Player* target);
    void ReleaseTarget();

    void AimCorrection(Vec3D LocalPos, Vec3D* InVecArg, Vec3D currVelocity, float Bulletspeed, float Bulletgrav);

    // TriggerBot 相关
    void CheckAndTriggerTarget(Player* target);
    void RenderTriggerFrame();

    void StartSkyNade(Player * target);

    // 渲染接口
    void RenderDraw();
    void RenderPredictionDebug();
    void renderUI();
    void renderGlobalTabSimplified();
    void renderSettingsTabSimplified();
    void renderHitboxesTabSimplified();
    void renderSkyNadeSettings();

    // 计算与辅助函数
    AimAngle CalculateAngle(Vec3D from, Vec3D to);
    Vec3D CalculatePredictedPosition(Vec3D targetPos, Vec3D targetVel, float bulletSpeed, float bulletGrav);
    float GetFOVScale() const;
    int GetBestBone(Player* target);
    int GetBoneFromHitbox(Player* p, HitboxType HitBox);
    float CalculateDistanceFromCrosshair(Vec3D targetPos);
    bool isKeyHeld(const std::string& keyName);
    Vec3D GetBonePosition(Player* player, HitboxType hitbox);
    int DetermineTargetType(Player* target);

    // UI 中的预测设置
    void renderPredictionSettings();
};
