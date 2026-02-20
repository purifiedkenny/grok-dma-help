#pragma once
#include <cstdint>
#include "VecDef.h"

// 基础实体类，用作所有游戏中可交互对象的基类
class Entity
{
public:
    // 基础属性
    int Index;                  // 实体在实体列表中的索引
    uint64_t BasePointer;       // 内存中的基础指针
    uint64_t valid;             // 用于验证实体是否有效
    Vec3D Origin;               // 世界空间中的位置
    Vec2D ScreenPosition;       // 屏幕空间中的位置

    std::string signifierName;
    // 构造函数
    Entity() : Index(-1), BasePointer(0), valid(0) {}
    Entity(int index) : Index(index), BasePointer(0), valid(0) {}

    // 虚析构函数确保正确的多态行为
    virtual ~Entity() {}

    // 验证实体是否有效 - 基础实现
    virtual void ValidCheck() {
        if (valid) {
            // 检查内存地址是否在有效范围内
            if (valid > 0x7FFFFFFEFFFF || valid < 0x7FF000000000) {
                BasePointer = 0;
                valid = 0;
            }
        }
    }

    // 检查实体是否有效
    virtual bool IsValid() {
        return BasePointer != 0 &&
            BasePointer != 0xCCCCCCCCCCCCCCCC &&
            BasePointer != 0xCDCDCDCDCDCDCDCD &&
            BasePointer < 0x7FFFFFFFFFFF &&
            BasePointer > 0x1000;
    }
};