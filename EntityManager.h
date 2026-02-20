#pragma once

#include <atomic>
#include <memory>
#include <vector>
#include <unordered_set>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <functional>
#include <chrono>
#include <boost/lockfree/spsc_queue.hpp>

#include "Player.h"
#include "Item.h"
#include "Shield.h"
#include "LocalPlayer.h"
#include "Map.h"
#include "VecDef.h"

// 护盾分类结构
struct CategorizedShields {
    std::vector<Shield*> gibraltarShields;
    std::vector<Shield*> rampartShields;
    std::vector<Shield*> newcastleShields;
    std::vector<Shield*> lifelineShields;
    std::chrono::steady_clock::time_point lastUpdateTime;
};

// 实体临时数据结构，用于批量处理
struct EntityTemp {
    int Index;
    int networkFlag;
    int teamNum;
    uint64_t BasePointer;
    uint64_t vaild;
    uint64_t nameAddress;
    Vec3D origin;
    char NameBuffer[128] = { 0 };
};

// 玩家批处理数据
struct PlayerBatchData {
    Vec3D origin;
    int health;
    int shield;
    bool isDead;
    bool isKnocked;
    int glowEnable;
    int glowThroughWall;
    int highlightID;
    float lastTimeAimedAt;
    float lastVisibleTime;
    float worldTime;
    float viewYaw;
    Vec2D viewAngles;
    Vec3D absoluteVelocity;
    int maxShield;
    uint64_t modelPointer;
    uint64_t bonePointer;
    uint64_t weaponHandle;
    uint64_t weaponHandleSecond;
    uint8_t kills;
    uint32_t damages;
    int weaponIndex;
    Vec3D vec_min;
    Vec3D vec_max;
};

// 通用批处理数据
struct EntityBatchData {
    Vec3D origin;
    uint64_t valid;
    int networkFlag;
    Vec3D rotation;
    int lootIndex;
};

// 无锁读写优化的多缓冲数据结构
template <typename T>
class ReadOptimizedBuffer {
private:
    // 使用指针数组存储实体 - 允许原子交换
    std::vector<T*>* m_data[3];  // 0: 读取, 1: 写入, 2: 预备缓冲
    std::atomic<int> m_readIndex{ 0 };
    std::atomic<int> m_writeIndex{ 1 };
    std::atomic<int> m_prepIndex{ 2 };
    mutable std::shared_mutex m_writeMutex;

public:

    ReadOptimizedBuffer() {
        for (int i = 0; i < 3; i++) {
            m_data[i] = new std::vector<T*>();
        }
    }

    ~ReadOptimizedBuffer() {
        for (int i = 0; i < 3; i++) {
            // 释放实体
            for (T* entity : *m_data[i]) {
                delete entity;
            }
            delete m_data[i];
        }
    }

    // 获取当前读取缓冲区 - 完全无锁
    std::shared_ptr<const std::vector<T*>> read() const {
        int currentReadIndex = m_readIndex.load(std::memory_order_acquire);
        return std::shared_ptr<const std::vector<T*>>(
            new std::vector<T*>(*m_data[currentReadIndex]),
            [](std::vector<T*>* ptr) { delete ptr; }
        );
    }

    // 开始写入过程 - 读写锁
    void beginUpdate() {
        m_writeMutex.lock();

        // 清理当前写入缓冲区
        int currentWriteIndex = m_writeIndex.load(std::memory_order_relaxed);
        auto& writeBuffer = *m_data[currentWriteIndex];

        // 复制当前读取缓冲区的内容
        int currentReadIndex = m_readIndex.load(std::memory_order_acquire);
        writeBuffer = *m_data[currentReadIndex];

        // 防止写缓冲无限增长导致 vector too long
        const size_t MAX_SAFE_SIZE = 5000;
        if (writeBuffer.size() > MAX_SAFE_SIZE) {
            std::cerr << "[Warning] write buffer size (" << writeBuffer.size() << ") exceeds safe threshold (" << MAX_SAFE_SIZE << "); trimming extra entities" << std::endl;
            size_t elementsToRemove = writeBuffer.size() - MAX_SAFE_SIZE;

            // 删除旧元素
            for (size_t i = 0; i < elementsToRemove; ++i) {
                delete writeBuffer[i];
            }

            writeBuffer.erase(writeBuffer.begin(), writeBuffer.begin() + elementsToRemove);
        }
    }

    // 获取当前写入缓冲区 - 必须在beginUpdate后调用
    std::vector<T*>& getWriteBuffer() {
        int currentWriteIndex = m_writeIndex.load(std::memory_order_relaxed);
        return *m_data[currentWriteIndex];
    }

    // 完成写入过程 - 将写入缓冲区转为可读
    void endUpdate() {
        // 交换读写指针
        int currentReadIndex = m_readIndex.load(std::memory_order_relaxed);
        int currentWriteIndex = m_writeIndex.load(std::memory_order_relaxed);
        int currentPrepIndex = m_prepIndex.load(std::memory_order_relaxed);

        // 循环切换：写入->读取->预备->写入
        m_readIndex.store(currentWriteIndex, std::memory_order_release);
        m_writeIndex.store(currentPrepIndex, std::memory_order_relaxed);
        m_prepIndex.store(currentReadIndex, std::memory_order_relaxed);

        m_writeMutex.unlock();
    }

    void clear() {
        std::unique_lock<std::shared_mutex> lock(m_writeMutex);

        // 统计一下要丢弃的对象数量
        int totalObjects = 0;
        for (int i = 0; i < 3; i++) {
            totalObjects += m_data[i]->size();
        }

        if (totalObjects > 0) {
            std::cout << "[Warning] performing safety cleanup; skipping deletion of " << totalObjects << " objects for now" << std::endl;
            // 仅清空容器指针数组，不删除对象
            for (int i = 0; i < 3; i++) {
                m_data[i]->clear();
            }
        }
    }
    

    void forceClear() {
        std::unique_lock<std::shared_mutex> lock(m_writeMutex);

        // 清理所有缓冲区
        for (int i = 0; i < 3; i++) {
            auto& buffer = *m_data[i];
            for (size_t j = 0; j < buffer.size(); j++) {
                T* entity = buffer[j];
                if (entity) {
                    try {
                        delete entity;
                    }
                    catch (...) {
                        // 忽略异常，确保继续处理
                    }
                }
            }
            buffer.clear();
            buffer.shrink_to_fit(); // 释放多余内存
        }

        // 重置索引
        m_readIndex.store(0, std::memory_order_release);
        m_writeIndex.store(1, std::memory_order_relaxed);
        m_prepIndex.store(2, std::memory_order_relaxed);
    }
};

class EntityManager {
public:

    float fps = 0;
    // 单例访问
    static EntityManager& getInstance();

    // 初始化和清理
    void initialize();
    void shutdown();

    // 暂停和恢复线程
    void suspendAllThreads();
    void resumeAllThreads();

    // 状态设置
    void setInGame(bool status);
    bool isInGame() const;

    void setBattleMode(bool status);
    bool isInBattleMode() const;

    void setLoggedIn(bool status);
    bool isLoggedIn() const;

    void setGameInput(bool status);
    bool isGameInput() const;

    // 实体访问接口 - 无锁读取
    std::shared_ptr<const std::vector<Player*>> getPlayers() const;
    std::shared_ptr<const std::vector<Item*>> getItems() const;
    std::shared_ptr<const std::vector<Shield*>> getShields() const;
    CategorizedShields getShieldCategories() const;

    // 实体移除接口
    void removeInvalidPlayers(const std::vector<Player*>& playersToRemove);

    // 存在性检查接口
    bool checkPlayerExists(int index) const;
    bool checkItemExists(int index) const;
    bool checkShieldExists(int index) const;

    // 清理方法
    void clearAllEntities();
    void clearEntitiesOnly();

    // 线程入口方法
    void mainScatterThread();
    void entityScatterThread();
    void playerScatterThread();
    void itemShieldScatterThread();

    // 护盾相关方法
    bool isPlayerBlockedByShields(Player* player);
    std::condition_variable_any m_threadSyncCV;
    std::mutex m_threadSyncMutex;
    std::atomic<int> m_activeThreadCount{ 0 };
    std::atomic<bool> m_clearingEntities{ false };
    std::atomic<bool> m_threadsSuspended{ false };
private:
    // 私有构造函数，确保单例模式
    EntityManager();
    ~EntityManager();

    // 禁止复制
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    // 内部实现方法
    void clearAllEntitiesInternal();
    void processEntityBatch(size_t startIdx, const std::vector<uint64_t>& basePointers);
    void processNewPlayer(Player* player);
    void processNewItem(Item* item);
    void processNewShield(Shield* shield);
    void updateShieldClassification();
    bool gatherGameData(LocalPlayer* localPlayer);
    bool scatterLocalPlayer(LocalPlayer* localPlayer);
    void scatterWeaponInfo(LocalPlayer* localPlayer);

    // 线程同步


    // 状态标志
    std::atomic<bool> m_inGame{ false };
    std::atomic<bool> m_battleMode{ false };
    std::atomic<bool> m_loggedIn{ false };
    std::atomic<bool> m_running{ true };
    std::atomic<bool> m_gameInput{ true };

    // 高效率读取的多缓冲区
    ReadOptimizedBuffer<Player> m_players;
    ReadOptimizedBuffer<Item> m_items;
    ReadOptimizedBuffer<Shield> m_shields;
    ReadOptimizedBuffer<CategorizedShields> m_shieldCategories;

    // 索引管理
    mutable std::shared_mutex m_indicesMutex;
    std::unordered_set<int> m_playerIndices;
    std::unordered_set<int> m_itemIndices;
    std::unordered_set<int> m_shieldIndices;


    // 性能监控
    std::chrono::steady_clock::time_point m_lastScanTime;
    std::atomic<int> m_scanCount{ 0 };
    std::atomic<int> m_lockContentionCount{ 0 };

    // 自适应控制
    std::chrono::milliseconds m_adaptiveSleepTime{ 1 };

    // 扫描参数
    struct ScanParameters {
        size_t batchSize{ 128 };              // 每批扫描的实体数量
        std::chrono::milliseconds interval{ 50 };  // 扫描间隔
        size_t playerScanEnd{ 200 };          // 玩家扫描结束索引
        size_t itemShieldScanStart{ 1000 };   // 物品和护盾扫描开始索引
        size_t itemShieldScanEnd{ 10000 };    // 物品和护盾扫描结束索引
    };

    ScanParameters m_scanParams;
};

