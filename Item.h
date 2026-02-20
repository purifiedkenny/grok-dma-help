#pragma once
#include "Entity.h"
#include <cstring>

// 物品类，代表游戏中的物品，如武器、医疗物品、弹药等
class Item : public Entity
{
public:
    // 物品特有属性
    uint64_t name_address;       // 物品名称在内存中的地址
    int LootIndex;               // 物品的战利品索引
    float distance_to_local;     // 到本地玩家的距离

    char NameBuffer[64] = { 0 }; // 物品名称缓冲区


    // 构造函数
    Item() : Entity(-1), name_address(0), LootIndex(0), distance_to_local(0) {
        memset(NameBuffer, 0, sizeof(NameBuffer));
    }

    Item(int ItemIndex) : Entity(ItemIndex), name_address(0), LootIndex(0), distance_to_local(0) {
        memset(NameBuffer, 0, sizeof(NameBuffer));
    }

    // 覆盖基类的ValidCheck方法
    virtual void ValidCheck() override {
        Entity::ValidCheck(); // 调用基类的验证方法
    }

    // 更新物品信息
    bool UpdateItemInfo() {
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

    // 获取物品名称
    const char* GetItemName() const {
        return NameBuffer;
    }

    // 获取物品距离
    float GetDistance() const {
        return distance_to_local;
    }
};