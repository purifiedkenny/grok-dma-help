#pragma once
#include "Entity.h"
#include <cstring>

class Shield : public Entity
{
public:
    // 物品特有属性
    int networkFlag;             // 网络标志
    float distance_to_local;     // 到本地玩家的距离
    Vec3D rotation;              // 物品旋转信息

    // 构造函数
    Shield() : Entity(-1), networkFlag(0), distance_to_local(0) {
    }

    Shield(int ItemIndex) : Entity(ItemIndex), networkFlag(0), distance_to_local(0) {
    }

    // 覆盖基类的ValidCheck方法
    virtual void ValidCheck() override {
        Entity::ValidCheck(); // 调用基类的验证方法
    }

    // 更新物品信息
    bool UpdateShieldInfo() {
        if (!IsValid()) return false;

        // 这里应该包含从内存读取物品信息的代码
        // 例如: 
        // mem.Read(name_address, &NameBuffer, sizeof(NameBuffer));
        // mem.Read(BasePointer + OFFSET_ORIGIN, &Origin, sizeof(Vec3D));
        // mem.Read(BasePointer + OFFSET_ROTATION, &rotation, sizeof(Vec3D));

        // 计算与本地玩家的距离
        // distance_to_local = CalculateDistance(localPlayer->Origin, Origin);

        return true;
    }

    // 获取物品距离
    float GetDistance() const {
        return distance_to_local;
    }
};