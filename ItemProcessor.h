#pragma once
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <array>
#include <span>
#include "Item.h"

// 预处理后的物品数据结构
struct ProcessedItemData {
    int itemId;              // 物品ID
    int lootIndex;           // 物品类型索引
    size_t categoryIndex;    // 物品分类索引
    std::string name;        // 物品名称
    std::string icon;        // 物品图标字符
    Vec2D screenPos;         // 屏幕坐标
    float distance;          // 与玩家距离
    bool isOnScreen;         // 是否在屏幕上
    float distanceToCenter;  // 到屏幕中心的距离
};

// 存储所有处理过的物品和更新信息
struct ProcessedItemsData {
    // 使用固定大小数组避免动态分配
    static constexpr size_t MAX_ITEMS = 1024;
    ProcessedItemData items[MAX_ITEMS];
    size_t itemCount = 0;

    std::chrono::steady_clock::time_point lastUpdate;
    Vec3D cameraPosition;    // 处理时的相机位置
    Vec2D viewAngles;        // 处理时的视角
    Vec2D screenSize;        // 处理时的屏幕尺寸

    // 版本号，用于检测变化
    uint64_t version = 0;

    // 清空数据
    void clear() {
        itemCount = 0;
    }

    // 添加项目，并检查数组边界
    bool addItem(const ProcessedItemData& item) {
        if (itemCount < MAX_ITEMS) {
            items[itemCount++] = item;
            return true;
        }
        return false;
    }
};

class ItemScreenProcessor {
private:
    // 双缓冲区
    ProcessedItemsData m_buffers[2];

    // 活动缓冲区索引 - 使用原子操作无锁交换
    std::atomic<int> m_activeIndex{ 0 };

    // 版本计数器
    std::atomic<uint64_t> m_versionCounter{ 0 };

    // 处理线程控制
    std::atomic<bool> m_running{ true };

    // 屏幕更新标记
    std::atomic<bool> m_screenPosNeedsUpdate{ true };

    // 缓存数据
    Vec3D m_lastCameraPosition;
    Vec2D m_lastViewAngles;
    std::chrono::steady_clock::time_point m_lastCameraUpdate;

    // 处理缓冲区
    alignas(32) Vec3D m_worldPositionsBuffer[ProcessedItemsData::MAX_ITEMS];
    alignas(32) Vec2D m_screenPositionsBuffer[ProcessedItemsData::MAX_ITEMS];
    alignas(32) bool m_resultsBuffer[ProcessedItemsData::MAX_ITEMS];

    // 获取非活动缓冲区（用于写入新数据）
    ProcessedItemsData& getInactiveBuffer() {
        int currentActive = m_activeIndex.load(std::memory_order_relaxed);
        return m_buffers[1 - currentActive];
    }

    // 获取物品信息
    std::pair<size_t, std::string> GetItemInfo(int lootIndex);

public:
    ItemScreenProcessor();
    ~ItemScreenProcessor();

    // 处理线程函数
    void ProcessingThreadFunction();

    // 高性能无锁访问当前物品数据
    const ProcessedItemsData& getProcessedItems() const {
        return m_buffers[m_activeIndex.load(std::memory_order_acquire)];
    }

    // 复制当前物品数据（如需修改）
    ProcessedItemsData getProcessedItemsCopy() const {
        return m_buffers[m_activeIndex.load(std::memory_order_acquire)];
    }

    // 获取数据版本号 - 用于检测更新
    uint64_t getVersion() const {
        return m_buffers[m_activeIndex.load(std::memory_order_acquire)].version;
    }

    // 通知需要更新屏幕位置
    void notifyCameraChanged() {
        m_screenPosNeedsUpdate.store(true, std::memory_order_release);
    }

    // 停止处理线程
    void shutdown() {
        m_running.store(false, std::memory_order_release);
    }
};