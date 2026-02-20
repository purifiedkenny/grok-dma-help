#pragma once
#include "Entity.h"

// 本地玩家类，继承自Entity，表示玩家控制的主角
class LocalPlayer : public Entity
{
public:
    // 武器相关
    uint64_t WeaponHandle;       // 当前武器句柄
    uint64_t WeaponEntity;       // 武器实体指针

    // 状态标志
    bool IsDead;                 // 是否死亡
    bool IsKnocked;              // 是否被击倒
    bool IsZooming;              // 是否在缩放（瞄准）
    bool InAttack;               // 是否在攻击
    bool isGrppleActived;        // 是否激活钩爪
    bool IsHoldingGrenade;       // 是否持有手雷
    bool IsReloading;            // 是否在装填
    bool IsEmptyHand;            // 是否空手

    // 基本属性
    int Team;                    // 队伍
    int Flags;                   // 标志位
    int isGRAPPLE;               // 钩爪状态
    int WeaponIndex;             // 武器索引
    int weapon_mod_bitfield;     // 武器模组位域
    int Ammo;                    // 弹药
    int skyDriveState;
    int backWardState;
    int duckState;
    int forewardState;
    int forceForeward;

    // 视角和武器相关属性
    float ViewYaw;               // 视角偏航
    float ZoomFOV;               // 缩放视场角
    float TargetZoomFOV;         // 目标缩放视场角
    float WeaponProjectileSpeed; // 武器投射物速度
    float WeaponProjectileScale; // 武器投射物比例
    float mouseSensitive;
    float wallrunStart;
    float wallrunClear;
    float TimeBase;
    float TravelStart;
    float TravelProgress;

    // 位置和移动相关
    Vec3D CameraOrigin;          // 相机原点
    Vec3D AbsoluteVelocity;      // 绝对速度
    Vec3D AimPunch;              // 瞄准冲击

    Vec2D ViewAngles;            // 视角角度
    Vec2D SwayAngles;            // 摇摆角度

    // 构造函数
    LocalPlayer() : Entity(),
        WeaponHandle(0), WeaponEntity(0),
        IsDead(false), IsKnocked(false), IsZooming(false),
        InAttack(false), isGrppleActived(false), IsHoldingGrenade(false),
        IsReloading(false),
        Team(0), Flags(0), isGRAPPLE(0), WeaponIndex(0),
        weapon_mod_bitfield(0), Ammo(0),
        ViewYaw(0), ZoomFOV(0), TargetZoomFOV(0),
        WeaponProjectileSpeed(0), WeaponProjectileScale(0) {
    }

    // 检查位置是否有效
    bool ValidPosition() {
        // 检查玩家位置是否在有效范围内
        return !Origin.empty();
    }

    // 覆盖基类的IsValid方法
    virtual bool IsValid() override {
        return BasePointer != 0 &&
            BasePointer != 0xCCCCCCCCCCCCCCCC &&
            BasePointer != 0xCDCDCDCDCDCDCDCD &&
            BasePointer < 0x7FFFFFFFFFFF &&
            BasePointer > 0x1000;
    }
};