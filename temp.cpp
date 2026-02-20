//#include <iostream>  
//#include <thread>
//#include <algorithm>
//#include "include.h"
//
//#include "ShieldCheck.h"
//#include "EntityManager.h"
//
//// 对外部的全局变量的引用
//static uint64_t OFF_Kills_Ptr; // 杀敌数偏移
//static uint64_t OFF_Damage_Ptr; // 伤害偏移
//static uint64_t OFF_Rank_Ptr;   // 等级偏移
//
//// 构造函数
//EntityManager::EntityManager() {
//    // 初始化状态
//    m_inGame = false;
//    m_battleMode = false;
//    m_loggedIn = false;
//    m_running = true;
//    m_clearingEntities = false;
//    m_threadsSuspended = false;
//    m_activeThreadCount = 0;
//    m_adaptiveSleepTime = std::chrono::milliseconds(1);
//    m_lastScanTime = std::chrono::steady_clock::now();
//    m_scanCount = 0;
//    m_lockContentionCount = 0;
//
//    // 启动队列处理线程
//    m_processingThread = std::thread(&EntityManager::queueProcessingThread, this);
//}
//
//// 析构函数
//EntityManager::~EntityManager() {
//    shutdown();
//
//    // 等待处理线程退出
//    if (m_processingThread.joinable()) {
//        m_processingThread.join();
//    }
//}
//
//// 初始化
//void EntityManager::initialize() {
//    // 确保之前的资源已清理
//    clearAllEntitiesInternal();
//
//    // 重置状态
//    m_inGame = false;
//    m_battleMode = false;
//    m_running = true;
//    m_clearingEntities = false;
//    m_threadsSuspended = false;
//    m_adaptiveSleepTime = std::chrono::milliseconds(1);
//}
//
//// 关闭
//void EntityManager::shutdown() {
//    // 停止所有线程
//    m_running = false;
//
//    // 如果线程暂停中，先恢复它们以便能正常退出
//    if (m_threadsSuspended) {
//        resumeAllThreads();
//    }
//
//    // 通知队列处理线程
//    m_queueCV.notify_all();
//
//    // 清理所有实体
//    clearAllEntitiesInternal();
//}
//
//// 启动所有线程
//void EntityManager::startThreads() {
//    // 确保先停止现有线程
//    m_running = false;
//    m_queueCV.notify_all();
//
//    // 等待处理线程退出
//    if (m_processingThread.joinable()) {
//        m_processingThread.join();
//    }
//
//    // 重置运行标志
//    m_running = true;
//
//    // 重新启动队列处理线程
//    m_processingThread = std::thread(&EntityManager::queueProcessingThread, this);
//}
//
//// 优化的线程暂停函数
//void EntityManager::suspendAllThreads() {
//    boost::unique_lock<boost::mutex> lock(m_threadSyncMutex);
//    if (m_threadsSuspended) return; // 已经暂停
//
//    m_clearingEntities = true;
//    lock.unlock(); // 在通知前释放锁，减少争用
//
//    m_threadSyncCV.notify_all();  // 通知所有线程检查状态
//
//    // 等待所有线程进入等待状态，使用超时防止死锁
//    lock.lock();
//    bool allSuspended = m_threadSyncCV.wait_for(lock, boost::chrono::milliseconds(100), [this]() {
//        return m_activeThreadCount == 0;
//        });
//
//    m_threadsSuspended = true;
//
//    // 超时情况处理
//    if (!allSuspended) {
//        std::cerr << "警告：线程暂停超时，可能有线程未响应" << std::endl;
//    }
//}
//
//// 优化的线程恢复函数
//void EntityManager::resumeAllThreads() {
//    boost::unique_lock<boost::mutex> lock(m_threadSyncMutex);
//    if (!m_threadsSuspended) return; // 已经运行中
//
//    m_clearingEntities = false;
//    m_threadsSuspended = false;
//    lock.unlock(); // 在通知前释放锁
//
//    m_threadSyncCV.notify_all();  // 通知所有线程可以继续运行
//}
//
//// 设置游戏状态
//void EntityManager::setInGame(bool status) {
//    m_inGame = status;
//}
//
//bool EntityManager::isInGame() const {
//    return m_inGame;
//}
//
//void EntityManager::setBattleMode(bool status) {
//    m_battleMode = status;
//}
//
//bool EntityManager::isInBattleMode() const {
//    return m_battleMode;
//}
//
//void EntityManager::setLoggedIn(bool status) {
//    m_loggedIn = status;
//}
//
//bool EntityManager::isLoggedIn() const {
//    return m_loggedIn;
//}
//
//void EntityManager::setGameInput(bool status) {
//    m_gameInput = status;
//}
//
//bool EntityManager::isGameInput() const {
//    return m_gameInput;
//}
//
//// 索引检查函数 - 使用共享锁进行只读访问
//bool EntityManager::checkPlayerExists(int index) const {
//    boost::shared_lock<boost::shared_mutex> lock(m_indicesMutex);
//    return m_playerIndices.count(index) > 0;
//}
//
//bool EntityManager::checkItemExists(int index) const {
//    boost::shared_lock<boost::shared_mutex> lock(m_indicesMutex);
//    return m_itemIndices.count(index) > 0;
//}
//
//bool EntityManager::checkShieldExists(int index) const {
//    boost::shared_lock<boost::shared_mutex> lock(m_indicesMutex);
//    return m_shieldIndices.count(index) > 0;
//}
//
//// 获取数据接口 - 无锁设计
//std::shared_ptr<const std::vector<Player*>> EntityManager::getPlayers() const {
//    return m_players.getItems();
//}
//
//std::shared_ptr<const std::vector<Item*>> EntityManager::getItems() const {
//    return m_items.getItems();
//}
//
//std::shared_ptr<const std::vector<Shield*>> EntityManager::getShields() const {
//    return m_shields.getItems();
//}
//
//CategorizedShields EntityManager::getShieldCategories() const {
//    auto categories = m_shieldCategories.getItems();
//    if (categories && !categories->empty()) {
//        // 直接返回第一个元素的复制品，而不是引用
//        return *(*categories)[0];
//    }
//    return CategorizedShields(); // 返回默认实例
//}
//
//// 更新接口 - 使用优化的三缓冲
//void EntityManager::updatePlayers(const std::function<void(std::vector<Player*>&)>& updateFunc) {
//    m_players.updateItems(updateFunc);
//}
//
//void EntityManager::updateItems(const std::function<void(std::vector<Item*>&)>& updateFunc) {
//    m_items.updateItems(updateFunc);
//}
//
//void EntityManager::updateShields(const std::function<void(std::vector<Shield*>&)>& updateFunc) {
//    m_shields.updateItems(updateFunc);
//}
//
//void EntityManager::updateShieldCategories(const CategorizedShields& categories) {
//    // 添加到三缓冲
//    m_shieldCategories.updateItems([&categories](std::vector<CategorizedShields*>& items) {
//        // 清除旧分类
//        for (auto* item : items) {
//            delete item;
//        }
//        items.clear();
//
//        // 添加新分类（复制一份）
//        CategorizedShields* newCategories = new CategorizedShields(categories);
//        items.push_back(newCategories);
//        });
//}
//
//// 添加实体接口 - 使用读写锁优化
//void EntityManager::addPlayers(const std::vector<Player*>& players) {
//    if (players.empty()) return;
//
//    // 使用互斥锁保护索引集
//    {
//        boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//        for (auto player : players) {
//            if (player) {
//                m_playerIndices.insert(player->Index);
//            }
//        }
//    }
//
//    // 添加到三缓冲
//    m_players.addItems(players);
//}
//
//void EntityManager::addItems(const std::vector<Item*>& items) {
//    if (items.empty()) return;
//
//    // 使用互斥锁保护索引集
//    {
//        boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//        for (auto item : items) {
//            if (item) {
//                m_itemIndices.insert(item->Index);
//            }
//        }
//    }
//
//    // 添加到三缓冲
//    m_items.addItems(items);
//}
//
//void EntityManager::addShields(const std::vector<Shield*>& shields) {
//    if (shields.empty()) return;
//
//    // 使用互斥锁保护索引集
//    {
//        boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//        for (auto shield : shields) {
//            if (shield) {
//                m_shieldIndices.insert(shield->Index);
//            }
//        }
//    }
//
//    // 添加到三缓冲
//    m_shields.addItems(shields);
//}
//
//// 添加新发现的实体 - 使用无锁队列
//void EntityManager::addNewPlayer(Player* player) {
//    if (!player) return;
//
//    // 尝试添加到无锁队列
//    if (!m_newPlayerQueue.push(player)) {
//        // 队列满，直接处理
//        std::vector<Player*> players{ player };
//        processNewPlayers(players);
//    }
//
//    // 通知处理线程
//    m_queueCV.notify_one();
//}
//
//void EntityManager::addNewItem(Item* item) {
//    if (!item) return;
//
//    // 尝试添加到无锁队列
//    if (!m_newItemQueue.push(item)) {
//        // 队列满，直接处理
//        std::vector<Item*> items{ item };
//        processNewItems(items);
//    }
//
//    // 通知处理线程
//    m_queueCV.notify_one();
//}
//
//void EntityManager::addNewShield(Shield* shield) {
//    if (!shield) return;
//
//    // 尝试添加到无锁队列
//    if (!m_newShieldQueue.push(shield)) {
//        // 队列满，直接处理
//        std::vector<Shield*> shields{ shield };
//        processNewShields(shields);
//    }
//
//    // 通知处理线程
//    m_queueCV.notify_one();
//}
//
//// 批量添加新实体 - 使用无锁队列
//void EntityManager::addNewPlayers(const std::vector<Player*>& players) {
//    if (players.empty()) return;
//
//    // 尝试逐个添加到无锁队列
//    std::vector<Player*> failedToQueue;
//    for (Player* player : players) {
//        if (player && !m_newPlayerQueue.push(player)) {
//            failedToQueue.push_back(player);
//        }
//    }
//
//    // 处理无法添加到队列的实体
//    if (!failedToQueue.empty()) {
//        processNewPlayers(failedToQueue);
//    }
//
//    // 通知处理线程
//    m_queueCV.notify_one();
//}
//
//void EntityManager::addNewItems(const std::vector<Item*>& items) {
//    if (items.empty()) return;
//
//    // 尝试逐个添加到无锁队列
//    std::vector<Item*> failedToQueue;
//    for (Item* item : items) {
//        if (item && !m_newItemQueue.push(item)) {
//            failedToQueue.push_back(item);
//        }
//    }
//
//    // 处理无法添加到队列的实体
//    if (!failedToQueue.empty()) {
//        processNewItems(failedToQueue);
//    }
//
//    // 通知处理线程
//    m_queueCV.notify_one();
//}
//
//void EntityManager::addNewShields(const std::vector<Shield*>& shields) {
//    if (shields.empty()) return;
//
//    // 尝试逐个添加到无锁队列
//    std::vector<Shield*> failedToQueue;
//    for (Shield* shield : shields) {
//        if (shield && !m_newShieldQueue.push(shield)) {
//            failedToQueue.push_back(shield);
//        }
//    }
//
//    // 处理无法添加到队列的实体
//    if (!failedToQueue.empty()) {
//        processNewShields(failedToQueue);
//    }
//
//    // 通知处理线程
//    m_queueCV.notify_one();
//}
//
//// 优化的队列处理线程
//void EntityManager::queueProcessingThread() {
//    // 设置适当的线程优先级
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
//    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 3);
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount++;
//    }
//
//    constexpr size_t BATCH_SIZE = 10;
//    std::chrono::milliseconds adaptiveSleepTime(1); // 自适应睡眠时间
//
//    while (m_running) {
//        try {
//            {
//                boost::unique_lock<boost::mutex> lock(m_threadSyncMutex);
//                if (m_clearingEntities) {
//                    // 暂停时减少活动线程计数
//                    m_activeThreadCount--;
//
//                    // 通知主线程此线程已暂停
//                    m_threadSyncCV.notify_all();
//
//                    // 等待清理完成信号
//                    m_threadSyncCV.wait(lock, [this]() {
//                        return !m_clearingEntities;
//                        });
//
//                    // 恢复时增加活动线程计数
//                    m_activeThreadCount++;
//                }
//            }
//
//            if (!m_loggedIn) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 跟踪是否处理了任何元素
//            bool processedAny = false;
//
//            // 处理玩家队列
//            std::vector<Player*> playerBatch;
//            playerBatch.reserve(BATCH_SIZE);
//
//            Player* player;
//            for (size_t i = 0; i < BATCH_SIZE && m_newPlayerQueue.pop(player); ++i) {
//                playerBatch.push_back(player);
//                processedAny = true;
//            }
//
//            if (!playerBatch.empty()) {
//                processNewPlayers(playerBatch);
//            }
//
//            // 处理物品队列
//            std::vector<Item*> itemBatch;
//            itemBatch.reserve(BATCH_SIZE);
//
//            Item* item;
//            for (size_t i = 0; i < BATCH_SIZE && m_newItemQueue.pop(item); ++i) {
//                itemBatch.push_back(item);
//                processedAny = true;
//            }
//
//            if (!itemBatch.empty()) {
//                processNewItems(itemBatch);
//            }
//
//            // 处理护盾队列
//            std::vector<Shield*> shieldBatch;
//            shieldBatch.reserve(BATCH_SIZE);
//
//            Shield* shield;
//            for (size_t i = 0; i < BATCH_SIZE && m_newShieldQueue.pop(shield); ++i) {
//                shieldBatch.push_back(shield);
//                processedAny = true;
//            }
//
//            if (!shieldBatch.empty()) {
//                processNewShields(shieldBatch);
//
//                // 更新护盾分类
//                updateShieldClassification();
//            }
//
//            // 自适应睡眠 - 如果没有处理任何元素，增加睡眠时间
//            if (!processedAny) {
//                adaptiveSleepTime = std::min(std::chrono::milliseconds(adaptiveSleepTime.count() * 2),
//                    std::chrono::milliseconds(20));
//
//                {
//                    boost::unique_lock<boost::mutex> lock(m_queueMutex);
//                    m_queueCV.wait_for(lock, boost::chrono::milliseconds(adaptiveSleepTime.count()), [this]() -> bool {
//                        return !m_running || !m_newPlayerQueue.empty() ||
//                            !m_newItemQueue.empty() || !m_newShieldQueue.empty();
//                        });
//                }
//            }
//            else {
//                // 如果处理了元素，重置睡眠时间
//                adaptiveSleepTime = std::chrono::milliseconds(1);
//            }
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error in queue processing thread: " << e.what() << std::endl;
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//        }
//    }
//
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount--;
//    }
//    m_threadSyncCV.notify_all();
//}
//
//// 移除无效实体
//void EntityManager::removeInvalidPlayers(const std::vector<Player*>& playersToRemove) {
//    if (playersToRemove.empty()) return;
//
//    // 收集需要移除的索引
//    std::vector<int> indicesToRemove;
//    for (Player* player : playersToRemove) {
//        if (player) {
//            indicesToRemove.push_back(player->Index);
//        }
//    }
//
//    // 更新索引集
//    {
//        boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//        for (int index : indicesToRemove) {
//            m_playerIndices.erase(index);
//        }
//    }
//
//    // 移除玩家并释放内存
//    m_players.removeItems(playersToRemove);
//}
//
//// 清理方法
//void EntityManager::clearAllEntities() {
//    clearAllEntitiesInternal();
//}
//
//void EntityManager::clearEntitiesOnly() {
//    // 清空三缓冲
//    m_players.updateItems([](std::vector<Player*>& players) {
//        players.clear();
//        });
//
//    m_items.updateItems([](std::vector<Item*>& items) {
//        items.clear();
//        });
//
//    m_shields.updateItems([](std::vector<Shield*>& shields) {
//        shields.clear();
//        });
//
//    // 清空索引集
//    {
//        boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//        m_playerIndices.clear();
//        m_itemIndices.clear();
//        m_shieldIndices.clear();
//    }
//}
//
//void EntityManager::mainScatterThread() {
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
//    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 2);
//
//    // 性能监控变量
//    std::chrono::steady_clock::time_point lastPerformanceLog = std::chrono::steady_clock::now();
//    int frameCount = 0;
//
//    while (m_running) {
//        try {
//            if (!m_loggedIn) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                continue;
//            }
//
//            // 性能监控 - 每5秒记录一次性能数据
//            frameCount++;
//            auto currentTime = std::chrono::steady_clock::now();
//            if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastPerformanceLog).count() >= 5) {
//                float fps = frameCount / 5.0f;
//                int lockContentions = m_lockContentionCount.exchange(0);
//                int scans = m_scanCount.exchange(0);
//
//                // 记录性能指标
//                if (fps < 200.0f) {  // 如果FPS低于期望阈值
//                    std::cerr << "Performance warning: FPS=" << fps
//                        << ", Lock contentions=" << lockContentions
//                        << ", Scans=" << scans << std::endl;
//                }
//
//                frameCount = 0;
//                lastPerformanceLog = currentTime;
//            }
//
//            char NameBufferTemp[64] = { 0 };
//            mem.Read(mem.OFF_BASE + OFF_LEVEL, NameBufferTemp, sizeof(NameBufferTemp));
//            map->MapName = std::string(NameBufferTemp);
//
//            if (map->MapName.empty()) {
//                // 暂停其他线程
//                suspendAllThreads();
//
//                // 安全清理
//                setInGame(false);
//                clearAllEntities();
//
//                // 恢复线程
//                resumeAllThreads();
//
//                if (mem.FixCr3())
//                    VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_ALL, 1);
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                continue;
//            }
//
//            if (!map->CheckMapData()) {
//                // 暂停其他线程
//                suspendAllThreads();
//
//                // 安全清理
//                setInGame(false);
//                clearAllEntities();
//
//                // 恢复线程
//                resumeAllThreads();
//                continue;
//            }
//
//            if (map->ReturnCR3Fix()) {
//                // 暂停其他线程
//                suspendAllThreads();
//
//                // 安全清理
//                setInGame(false);
//                clearAllEntities();
//
//                // 恢复线程
//                resumeAllThreads();
//
//                if (mem.FixCr3())
//                    VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_ALL, 1);
//                continue;
//            }
//
//            // 使用本地临时变量避免对象修改冲突
//            LocalPlayer temp;
//            if (!gatherGameData(&temp) || !temp.ValidPosition()) {
//                // 暂停其他线程
//                suspendAllThreads();
//
//                // 安全清理
//                setInGame(false);
//                clearAllEntities();
//
//                // 恢复线程
//                resumeAllThreads();
//                continue;
//            }
//
//            // 收集武器信息
//            scatterWeaponInfo(&temp);
//
//            // 更新本地玩家数据
//            *localPlayer = temp;
//
//            // 标记游戏中状态
//            setInGame(true);
//
//            glow->run();
//            spectator->run();
//
//            std::this_thread::sleep_for(std::chrono::milliseconds(1));
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error in MainScatter thread: " << e.what() << std::endl;
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//        }
//    }
//}
//
//// 实体散射线程 - 优化版本
//void EntityManager::entityScatterThread() {
//    // 设置适当的线程优先级
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
//    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 0);  // 绑定到核心0
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount++;
//    }
//
//    constexpr size_t BATCH_SIZE = 2048;
//    constexpr size_t MAX_ENTITIES = 10000;
//
//    // 使用稀疏表示替代固定大小位图
//    std::unordered_set<size_t> existingIndices;
//    existingIndices.reserve(8000); // 预留空间避免频繁重新分配
//
//    // 实体数据池
//    std::vector<EntityTemp> entityPool;
//    entityPool.resize(MAX_ENTITIES); // 预分配最大大小避免溢出
//
//    // 基础指针数组，用于批量读取
//    std::vector<uint64_t> basePointers;
//    basePointers.resize(BATCH_SIZE);
//
//    // 有效索引容器，预分配避免重新分配
//    std::vector<size_t> validIndices;
//    validIndices.reserve(BATCH_SIZE);
//
//    // 批处理更新周期控制
//    constexpr int UPDATE_FREQUENCY = 10; // 每10次迭代更新一次索引集
//    int updateCounter = 0;
//
//    // 性能相关变量
//    auto lastUpdateTime = std::chrono::steady_clock::now();
//    std::chrono::milliseconds scanInterval(5); // 自适应扫描间隔
//
//    while (m_running) {
//        try {
//            {
//                boost::unique_lock<boost::mutex> lock(m_threadSyncMutex);
//                if (m_clearingEntities) {
//                    // 暂停时减少活动线程计数
//                    m_activeThreadCount--;
//
//                    // 通知主线程此线程已暂停
//                    m_threadSyncCV.notify_all();
//
//                    // 等待清理完成信号
//                    m_threadSyncCV.wait(lock, [this]() {
//                        return !m_clearingEntities;
//                        });
//
//                    // 恢复时增加活动线程计数
//                    m_activeThreadCount++;
//                }
//            }
//
//            auto scanStartTime = std::chrono::steady_clock::now();
//
//            if (!m_loggedIn) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                continue;
//            }
//
//            // 检查游戏状态并清理
//            if (!m_inGame) {
//                existingIndices.clear();
//                clearEntitiesOnly();
//                // 重置扫描策略
//                std::this_thread::sleep_for(std::chrono::milliseconds(50));
//                continue;
//            }
//
//            // 优化: 周期性更新索引集合而不是每次迭代
//            if (updateCounter == 0) {
//                // 检查队列是否有待处理的新实体
//                bool hasNewEntities = !m_newPlayerQueue.empty() || !m_newItemQueue.empty() || !m_newShieldQueue.empty();
//
//                existingIndices.clear();
//
//                // 收集现有实体索引 - 使用共享锁进行只读访问
//                {
//                    boost::shared_lock<boost::shared_mutex> lock(m_indicesMutex);
//                    // 确保有足够空间添加所有索引
//                    existingIndices.reserve(m_playerIndices.size() + m_itemIndices.size() + m_shieldIndices.size());
//                    existingIndices.insert(m_playerIndices.begin(), m_playerIndices.end());
//                    existingIndices.insert(m_itemIndices.begin(), m_itemIndices.end());
//                    existingIndices.insert(m_shieldIndices.begin(), m_shieldIndices.end());
//                }
//            }
//            updateCounter = (updateCounter + 1) % UPDATE_FREQUENCY;
//
//            validIndices.clear();
//            const float ItemDistance = esp->items.maxDistance;
//            const float ESPDistance = esp->enemies.distance;
//
//            // 分批处理实体 - 优先处理可能活跃的区域
//            size_t entitiesFound = 0;
//
//            // 更智能的扫描序列，优先扫描活跃区域
//            std::vector<size_t> scanSequence;
//            scanSequence.reserve(MAX_ENTITIES / BATCH_SIZE + 1);
//
//            // 如果在战斗模式下，优先扫描0-1000的实体ID，它们通常是玩家
//            if (m_battleMode) {
//                for (size_t i = 0; i < 100; i += BATCH_SIZE) {
//                    scanSequence.push_back(i);
//                }
//
//                // 然后扫描其余区域
//                for (size_t i = 100; i < MAX_ENTITIES; i += BATCH_SIZE) {
//                    scanSequence.push_back(i);
//                }
//            }
//            else {
//                // 正常模式下顺序扫描
//                for (size_t i = 0; i < MAX_ENTITIES; i += BATCH_SIZE) {
//                    scanSequence.push_back(i);
//                }
//            }
//
//            if (!scanSequence.empty()) {
//                std::vector<QWORD> prefetchAddresses;
//                prefetchAddresses.reserve(std::min(scanSequence.size(), size_t(250)));
//
//                // 选择前250个区域进行预读 (避免一次预读过多)
//                size_t prefetchCount = std::min(scanSequence.size(), size_t(250));
//                for (size_t i = 0; i < prefetchCount; i++) {
//                    size_t batchStart = scanSequence[i];
//                    // 预读每批次的第一个实体
//                    prefetchAddresses.push_back(mem.OFF_BASE + OFF_ENTITY_LIST +
//                        ((static_cast<unsigned long long>(batchStart) + 1) << 5));
//                }
//
//                // 执行预读
//                if (!prefetchAddresses.empty()) {
//                    VMMDLL_MemPrefetchPages(mem.vHandle, mem.current_process.PID,
//                        prefetchAddresses.data(),
//                        static_cast<DWORD>(prefetchAddresses.size()));
//                }
//            }
//            // 为批量添加实体准备容器
//            std::vector<Item*> newItemsToAdd;
//            std::vector<Shield*> newShieldsToAdd;
//            std::vector<Player*> newPlayersToAdd;
//
//            // 预分配空间避免重新分配
//            newItemsToAdd.reserve(100);
//            newShieldsToAdd.reserve(100);
//            newPlayersToAdd.reserve(100);
//
//            // 主扫描循环
//            for (size_t batchStart : scanSequence) {
//                if (!m_inGame) break;  // 中断检查
//
//                size_t batchEnd = std::min(batchStart + BATCH_SIZE, MAX_ENTITIES);
//
//                // 读取基础指针
//                {
//                    auto handle = mem.CreateScatterHandle();
//                    if (!handle) { mem.CloseScatterHandle(handle); continue; }
//
//                    for (size_t i = batchStart, idx = 0; i < batchEnd; i++, idx++) {
//                        uint64_t address = mem.OFF_BASE + OFF_ENTITY_LIST + ((static_cast<unsigned long long>(i) + 1) << 5);
//                        mem.AddScatterReadRequest(handle, address, &basePointers[idx], sizeof(uint64_t));
//                    }
//
//                    mem.ExecuteReadScatter(handle);
//                    mem.CloseScatterHandle(handle);
//                }
//
//                // 验证实体并收集有效索引
//                std::vector<size_t> tempIndices;
//                tempIndices.reserve(BATCH_SIZE); // 预分配避免重新分配
//
//                {
//                    auto handle = mem.CreateScatterHandle();
//                    if (!handle) { mem.CloseScatterHandle(handle); continue; }
//
//                    for (size_t i = batchStart, idx = 0; i < batchEnd; i++, idx++) {
//                        // 跳过无效指针和已知实体
//                        if (!mem.IsValidPointer(basePointers[idx]) || existingIndices.count(i) > 0) {
//                            continue;
//                        }
//                        mem.AddScatterReadRequest(handle, basePointers[idx],
//                            &entityPool[i].vaild, sizeof(uint64_t));
//                        tempIndices.push_back(i);
//                    }
//
//                    if (!tempIndices.empty()) {
//                        mem.ExecuteReadScatter(handle);
//                        for (const auto& idx : tempIndices) {
//                            if (mem.IsValidPointer(entityPool[idx].vaild)) {
//                                validIndices.push_back(idx);
//                                entityPool[idx].BasePointer = basePointers[idx - batchStart];
//                                entityPool[idx].Index = idx;
//                                entitiesFound++;
//                            }
//                        }
//                    }
//                    mem.CloseScatterHandle(handle);
//                }
//
//                if (!validIndices.empty()) {
//                    // 读取实体数据
//                    {
//                        auto handle = mem.CreateScatterHandle();
//                        if (!handle) { mem.CloseScatterHandle(handle); continue; }
//
//                        for (const auto& idx : validIndices) {
//                            mem.AddScatterReadRequest(handle, entityPool[idx].BasePointer + OFF_SignifierName,
//                                &entityPool[idx].nameAddress, sizeof(uint64_t));
//                            mem.AddScatterReadRequest(handle, entityPool[idx].BasePointer + OFF_PROP_SCRIPT_NETWORKFLAG,
//                                &entityPool[idx].networkFlag, sizeof(int));
//                            mem.AddScatterReadRequest(handle, entityPool[idx].BasePointer + OFF_TEAM_NUMBER,
//                                &entityPool[idx].teamNum, sizeof(int));
//                            mem.AddScatterReadRequest(handle, entityPool[idx].BasePointer + OFF_LOCAL_ORIGIN,
//                                &entityPool[idx].origin, sizeof(Vec3D));
//                        }
//
//                        mem.ExecuteReadScatter(handle);
//                        mem.CloseScatterHandle(handle);
//                    }
//
//                    // 读取实体名称
//                    {
//                        auto handle = mem.CreateScatterHandle();
//                        if (!handle) { mem.CloseScatterHandle(handle); continue; }
//
//                        for (const auto& idx : validIndices) {
//                            if (mem.IsValidPointer(entityPool[idx].nameAddress)) {
//                                mem.AddScatterReadRequest(handle, entityPool[idx].nameAddress,
//                                    entityPool[idx].NameBuffer, sizeof(entityPool[idx].NameBuffer));
//                            }
//                        }
//
//                        mem.ExecuteReadScatter(handle);
//                        mem.CloseScatterHandle(handle);
//                    }
//                    // 分类并存储新实体
//                    for (const auto& idx : validIndices) {
//                        const auto& entity = entityPool[idx];
//                        if (!mem.IsValidPointer(entity.BasePointer)) continue;
//
//                        std::string name(entity.NameBuffer);
//
//                        try {
//
//                            // 分类并按距离过滤
//                            if (name == "prop_survival") {
//                                float distanceSquared = entity.origin.distanceSquared2dTo(localPlayer->CameraOrigin);
//                                float maxDistanceSquared = (ItemDistance / 0.025f) * (ItemDistance / 0.025f);
//                                if (distanceSquared < maxDistanceSquared)
//                                {
//                                    Item* item = new Item();
//                                    item->BasePointer = entity.BasePointer;
//                                    item->Index = entity.Index;
//                                    newItemsToAdd.push_back(item);
//                                }
//                            }
//                            else if (name == "prop_script" &&
//                                (entity.networkFlag == 286269440 || entity.networkFlag == 286269441 ||
//                                    entity.networkFlag == 286261248 || entity.networkFlag == 286302208)) {
//                                float distanceSquared = entity.origin.distanceSquared2dTo(localPlayer->CameraOrigin);
//                                float maxDistanceSquared = (ESPDistance / 0.025f) * (ESPDistance / 0.025f);
//                                if (distanceSquared < maxDistanceSquared)
//                                {
//                                    Shield* shield = new Shield();
//                                    shield->BasePointer = entity.BasePointer;
//                                    shield->Index = entity.Index;
//                                    shield->networkFlag = entity.networkFlag;
//                                    newShieldsToAdd.push_back(shield);
//                                }
//                            }
//                            else if (name == "player" || name == "npc_dummie") {
//                                Player* player = new Player();
//                                player->BasePointer = entity.BasePointer;
//                                player->Index = entity.Index;
//                                newPlayersToAdd.push_back(player);
//                            }
//                        }
//                        catch (...) {
//                            // 清理已分配的资源
//                            for (auto* item : newItemsToAdd) delete item;
//                            for (auto* shield : newShieldsToAdd) delete shield;
//                            for (auto* player : newPlayersToAdd) delete player;
//                            throw;
//                        }
//                    }
//                }
//
//                auto currentIt = std::find(scanSequence.begin(), scanSequence.end(), batchStart);
//                // 预读下2-3个批次的数据
//                if (currentIt != scanSequence.end() && std::distance(currentIt, scanSequence.end()) > 3) {
//                    std::vector<QWORD> nextPrefetchAddresses;
//                    nextPrefetchAddresses.reserve(BATCH_SIZE * 3);
//
//                    // 预读未来3个批次的起始地址
//                    for (int ahead = 1; ahead <= 3; ahead++) {
//                        if (currentIt + ahead >= scanSequence.end()) break;
//
//                        size_t futureBatchStart = *(currentIt + ahead);
//                        for (size_t i = 0; i < BATCH_SIZE; i += 16) { // 每16个实体取样一个
//                            nextPrefetchAddresses.push_back(mem.OFF_BASE + OFF_ENTITY_LIST +
//                                ((static_cast<unsigned long long>(futureBatchStart + i) + 1) << 5));
//                        }
//                    }
//
//                    // 异步执行预读
//                    if (!nextPrefetchAddresses.empty()) {
//                        VMMDLL_MemPrefetchPages(mem.vHandle, mem.current_process.PID,
//                            nextPrefetchAddresses.data(),
//                            static_cast<DWORD>(nextPrefetchAddresses.size()));
//                    }
//                }
//
//                // 更轻量的yield，让UI线程有机会执行
//                std::this_thread::sleep_for(std::chrono::microseconds(1));
//            }
//
//            // 批量添加实体到队列，减少锁竞争
//            if (!newItemsToAdd.empty()) {
//                addNewItems(newItemsToAdd);
//            }
//
//            if (!newShieldsToAdd.empty()) {
//                addNewShields(newShieldsToAdd);
//            }
//
//            if (!newPlayersToAdd.empty()) {
//                addNewPlayers(newPlayersToAdd);
//            }
//
//            // 增加扫描计数
//            m_scanCount++;
//
//            // 自适应睡眠基于处理时间和实体数量
//            auto scanEndTime = std::chrono::steady_clock::now();
//            auto scanDuration = std::chrono::duration_cast<std::chrono::milliseconds>(scanEndTime - scanStartTime);
//
//            // 在战斗模式下更积极的扫描，否则协作式睡眠
//            if (m_battleMode) {
//                std::this_thread::sleep_for(std::chrono::microseconds(100));
//            }
//            else {
//                // 如果扫描快速完成且找到实体少，适当增加睡眠时间
//                scanInterval = std::min(
//                    std::max(std::chrono::milliseconds(1),
//                        std::chrono::milliseconds(5 - scanDuration.count())),
//                    std::chrono::milliseconds(5));
//
//                std::this_thread::sleep_for(scanInterval);
//            }
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error in EntityScatter thread: " << e.what() << std::endl;
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//        }
//    }
//
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount--;
//    }
//    m_threadSyncCV.notify_all();
//}
//
//// 玩家处理线程 - 优化版本
//void EntityManager::playerScatterThread() {
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
//    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 1);  // 绑定到核心1
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount++;
//    }
//
//    // 预分配批数据容器
//    std::vector<PlayerBatchData> playerBatchData;
//    playerBatchData.reserve(100);
//
//    // 加载PR玩家列表
//    static std::unordered_set<uint64_t> proPlayerUIDs;
//    static bool prListLoaded = false;
//
//    if (!prListLoaded) {
//        try {
//            std::ifstream prListFile("PRList.json");
//            if (prListFile.is_open()) {
//                // 读取整个文件内容到字符串
//                std::string raw((std::istreambuf_iterator<char>(prListFile)),
//                    std::istreambuf_iterator<char>());
//                prListFile.close();
//
//                // 手动解析 JSON，避免使用自动类型推导
//                auto j = nlohmann::json::parse(raw);
//                if (j.contains("Pro") && j["Pro"].is_array()) {
//                    // 预分配空间避免重新分配
//                    proPlayerUIDs.reserve(j["Pro"].size());
//                    for (const auto& item : j["Pro"]) {
//                        if (item.contains("uid") && item["uid"].is_number_unsigned()) {
//                            uint64_t uid = item["uid"].get<uint64_t>();
//                            proPlayerUIDs.insert(uid);
//                        }
//                    }
//                }
//                prListLoaded = true;
//            }
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error loading PRList: " << e.what() << std::endl;
//            prListLoaded = true; // 即使失败也标记为已加载，避免重复尝试
//        }
//    }
//
//    // 性能变量
//    auto lastUpdateTime = std::chrono::steady_clock::now();
//    int frameCounter = 0;
//    std::chrono::milliseconds adaptiveSleepTime(1);
//
//    while (m_running) {
//        try {
//            auto frameStartTime = std::chrono::steady_clock::now();
//
//            {
//                boost::unique_lock<boost::mutex> lock(m_threadSyncMutex);
//                if (m_clearingEntities) {
//                    // 暂停时减少活动线程计数
//                    m_activeThreadCount--;
//
//                    // 通知主线程此线程已暂停
//                    m_threadSyncCV.notify_all();
//
//                    // 等待清理完成信号
//                    m_threadSyncCV.wait(lock, [this]() {
//                        return !m_clearingEntities;
//                        });
//
//                    // 恢复时增加活动线程计数
//                    m_activeThreadCount++;
//                }
//            }
//
//            if (!m_loggedIn || !m_inGame) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 更新现有玩家
//            auto players = getPlayers();
//            if (players->empty()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 复制需要处理的玩家
//            std::vector<Player*> playersCopy;
//            playersCopy.reserve(players->size());
//
//            for (const auto& player : *players) {
//                if (player && mem.IsValidPointer(player->BasePointer)) {
//                    playersCopy.push_back(player);
//                }
//            }
//
//            if (playersCopy.empty()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            if (!playersCopy.empty()) {
//                std::vector<QWORD> prefetchAddresses;
//                prefetchAddresses.reserve(playersCopy.size() * 3); // 每个玩家预读3个关键地址
//
//                for (Player* player : playersCopy) {
//                    if (!player || !mem.IsValidPointer(player->BasePointer) || !mem.IsValidPointer(player->valid)) continue;
//
//                    // 预读玩家位置
//                    prefetchAddresses.push_back(player->BasePointer + OFF_LOCAL_ORIGIN);
//                    prefetchAddresses.push_back(player->BasePointer + OFF_HEALTH);
//                    prefetchAddresses.push_back(player->BasePointer + OFF_SHIELD);
//                }
//
//                // 执行预读
//                if (!prefetchAddresses.empty()) {
//                    VMMDLL_MemPrefetchPages(mem.vHandle, mem.current_process.PID,
//                        prefetchAddresses.data(),
//                        static_cast<DWORD>(prefetchAddresses.size()));
//                }
//            }
//            // 准备数据结构
//            playerBatchData.resize(playersCopy.size());
//
//            // 创建散射句柄批量读取数据
//            auto handle = mem.CreateScatterHandle();
//            if (!handle) {
//                mem.CloseScatterHandle(handle);
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                continue;
//            }
//
//            // 为所有玩家添加散射读取请求
//            for (size_t i = 0; i < playersCopy.size(); i++) {
//                Player* player = playersCopy[i];
//                uint64_t base = player->BasePointer;
//                auto& data = playerBatchData[i];
//
//                mem.AddScatterReadRequest(handle, base + OFF_LOCAL_ORIGIN, &data.origin, sizeof(Vec3D));
//                mem.AddScatterReadRequest(handle, base + OFF_HEALTH, &data.health, sizeof(int));
//                mem.AddScatterReadRequest(handle, base + OFF_SHIELD, &data.shield, sizeof(int));
//                mem.AddScatterReadRequest(handle, base + OFF_LIFE_STATE, &data.isDead, sizeof(bool));
//                mem.AddScatterReadRequest(handle, base + OFF_BLEEDOUT_STATE, &data.isKnocked, sizeof(bool));
//                mem.AddScatterReadRequest(handle, base + OFF_GLOW_ENABLE, &data.glowEnable, sizeof(int));
//                mem.AddScatterReadRequest(handle, base + OFF_GLOW_THROUGH_WALL, &data.glowThroughWall, sizeof(int));
//                mem.AddScatterReadRequest(handle, base + OFF_GLOW_HIGHLIGHT_ID, &data.highlightID, sizeof(int));
//                mem.AddScatterReadRequest(handle, base + OFF_LAST_AIMEDAT_TIME, &data.lastTimeAimedAt, sizeof(float));
//                mem.AddScatterReadRequest(handle, base + OFF_LAST_VISIBLE_TIME, &data.lastVisibleTime, sizeof(float));
//                mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_TIME_BASE, &data.worldTime, sizeof(float));
//                mem.AddScatterReadRequest(handle, base + OFF_YAW, &data.viewYaw, sizeof(float));
//                mem.AddScatterReadRequest(handle, base + OFF_VIEW_ANGLES, &data.viewAngles, sizeof(Vec2D));
//                mem.AddScatterReadRequest(handle, base + OFF_ABSVELOCITY, &data.absoluteVelocity, sizeof(Vec3D));
//                mem.AddScatterReadRequest(handle, base + OFF_MAXSHIELD, &data.maxShield, sizeof(int));
//                mem.AddScatterReadRequest(handle, base + OFF_STUDIOHDR, &data.modelPointer, sizeof(uint64_t));
//                mem.AddScatterReadRequest(handle, base + OFF_BONES, &data.bonePointer, sizeof(uint64_t));
//                mem.AddScatterReadRequest(handle, base + OFF_ACTIVE_WEAPON, &data.weaponHandle, sizeof(uint64_t));
//                mem.AddScatterReadRequest(handle, base + OFF_Collision + OFF_Collision_m_vecMins, &data.vec_min, sizeof(Vec3D));
//                mem.AddScatterReadRequest(handle, base + OFF_Collision + OFF_Collision_m_vecMaxs, &data.vec_max, sizeof(Vec3D));
//                mem.AddScatterReadRequest(handle, player->NetVarData + OFF_NETVAR_RANGES + OFF_Kills_Ptr, &data.kills, sizeof(uint8_t));
//                mem.AddScatterReadRequest(handle, player->NetVarData + OFF_NETVAR_INT32S + OFF_Damage_Ptr, &data.damages, sizeof(uint32_t));
//            }
//
//            mem.ExecuteReadScatter(handle);
//            mem.CloseScatterHandle(handle);
//
//            // 武器数据读取
//            bool enableWeaponInfo = true;
//            if (enableWeaponInfo) {
//                auto weaponHandle = mem.CreateScatterHandle();
//                if (weaponHandle) {
//                    for (size_t i = 0; i < playersCopy.size(); i++) {
//                        if (!playerBatchData[i].weaponHandle) continue;
//
//                        uint64_t WeaponHandleMasked = playerBatchData[i].weaponHandle & 0xffff;
//                        uint64_t weaponEntityAddress = mem.OFF_BASE + OFF_ENTITY_LIST + (WeaponHandleMasked << 5);
//                        mem.AddScatterReadRequest(weaponHandle, weaponEntityAddress,
//                            &playerBatchData[i].weaponHandleSecond, sizeof(uint64_t));
//                    }
//
//                    mem.ExecuteReadScatter(weaponHandle);
//                    mem.CloseScatterHandle(weaponHandle);
//
//                    auto weaponIndexHandle = mem.CreateScatterHandle();
//                    if (weaponIndexHandle) {
//                        for (size_t i = 0; i < playersCopy.size(); i++) {
//                            if (!playerBatchData[i].weaponHandleSecond ||
//                                !mem.IsValidPointer(playerBatchData[i].weaponHandleSecond)) continue;
//
//                            mem.AddScatterReadRequest(weaponIndexHandle,
//                                playerBatchData[i].weaponHandleSecond + OFF_WEAPON_INDEX,
//                                &playerBatchData[i].weaponIndex, sizeof(int));
//                        }
//
//                        mem.ExecuteReadScatter(weaponIndexHandle);
//                        mem.CloseScatterHandle(weaponIndexHandle);
//                    }
//                }
//            }
//            // 更新玩家数据
//            std::vector<Player*> invalidPlayers;
//            invalidPlayers.reserve(playersCopy.size());
//
//            for (size_t i = 0; i < playersCopy.size(); i++) {
//                Player* player = playersCopy[i];
//                const auto& data = playerBatchData[i];
//
//                if (data.health > 100 || data.health <= 0 || data.origin.empty()) {
//                    invalidPlayers.push_back(player);
//                    continue;
//                }
//
//                player->Health = data.health;
//                player->Shield = data.shield;
//                player->IsDead = data.isDead;
//                player->IsKnocked = data.isKnocked;
//                player->GlowEnable = data.glowEnable;
//                player->GlowThroughWall = data.glowThroughWall;
//                player->HighlightID = data.highlightID;
//                player->LastTimeAimedAt = data.lastTimeAimedAt;
//                player->LastVisibleTime = data.lastVisibleTime;
//                player->ViewYaw = data.viewYaw;
//                player->ViewAngles = data.viewAngles;
//                player->AbsoluteVelocity = data.absoluteVelocity;
//                player->calculatedVelocity = data.absoluteVelocity;
//                player->MaxShield = data.maxShield;
//                player->ModelPointer = data.modelPointer;
//                player->BonePointer = data.bonePointer;
//                player->Origin = data.origin;
//                player->VecMin = data.vec_min + data.origin;
//                player->VecMax = data.vec_max + data.origin;
//                player->DistanceToLocalPlayer = localPlayer->Origin.distance2dTo(player->Origin) * 0.025F;
//                player->Distance2DToLocalPlayer = localPlayer->Origin.distance2dTo(player->Origin) * 0.025F;
//
//                player->AimedAtCheck();
//
//                player->IsKnocked = player->IsDummy() ? false : player->IsKnocked;
//                player->IsVisible = player->VisCheck() || player->IsAimedAt;
//                player->needShieldCheck = player->IsVisible;
//                player->weaponIndex = data.weaponIndex;
//                player->Kills = data.kills;
//                player->Damages = data.damages;
//                player->IsBot = player->IsDummy();
//
//                // 处理假人特殊逻辑 
//                if (player->IsDummy()) {
//                    float currentTime = data.worldTime;
//                    if (!player->hasLastPosition) {
//                        player->lastOrigin = data.origin;
//                        player->lastUpdateTime = currentTime;
//                        player->hasLastPosition = true;
//                        player->calculatedVelocity = Vec3D();
//                    }
//                    else {
//                        float deltaTime = currentTime - player->lastUpdateTime;
//                        if (deltaTime > 0) {
//                            Vec3D deltaPosition = {
//                                data.origin.x - player->lastOrigin.x,
//                                data.origin.y - player->lastOrigin.y,
//                                data.origin.z - player->lastOrigin.z
//                            };
//
//                            player->calculatedVelocity = {
//                                deltaPosition.x / deltaTime,
//                                deltaPosition.y / deltaTime,
//                                deltaPosition.z / deltaTime
//                            };
//
//                            player->lastOrigin = data.origin;
//                            player->lastUpdateTime = currentTime;
//                        }
//                    }
//                }
//            }
//
//            // 移除无效玩家
//            if (!invalidPlayers.empty()) {
//                removeInvalidPlayers(invalidPlayers);
//            }
//
//            // 检查护盾阻挡 - 只在需要时执行
//            bool needsShieldCheck = false;
//            for (auto* player : playersCopy) {
//                if (player && player->needShieldCheck) {
//                    needsShieldCheck = true;
//                    break;
//                }
//            }
//
//            if (needsShieldCheck) {
//                updatePlayers([this](std::vector<Player*>& players) {
//                    for (auto* player : players) {
//                        if (!player || !player->needShieldCheck) continue;
//                        if (isPlayerBlockedByShields(player)) {
//                            player->IsVisible = false;
//                        }
//                    }
//                    });
//            }
//            // 计算帧处理时间和自适应睡眠
//            frameCounter++;
//            auto frameEndTime = std::chrono::steady_clock::now();
//            auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime);
//
//            // 自适应睡眠
//            if (m_battleMode) {
//                // 在战斗模式下使用最小睡眠时间
//                std::this_thread::sleep_for(std::chrono::microseconds(100));
//            }
//            else {
//                // 在非战斗模式下自适应调整
//                adaptiveSleepTime = std::chrono::milliseconds(
//                    std::min(10, std::max(1, 5 - static_cast<int>(frameDuration.count() / 1000))));
//
//                std::this_thread::sleep_for(adaptiveSleepTime);
//            }
//
//            // 每秒记录一次性能数据
//            if (std::chrono::duration_cast<std::chrono::seconds>(frameEndTime - lastUpdateTime).count() >= 1) {
//                lastUpdateTime = frameEndTime;
//
//                // 重置计数器
//                frameCounter = 0;
//            }
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error in PlayerScatter thread: " << e.what() << std::endl;
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//        }
//    }
//
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount--;
//    }
//    m_threadSyncCV.notify_all();
//}
//
//// 物品处理线程 - 优化版本
//void EntityManager::itemScatterThread() {
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
//    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 1);
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount++;
//    }
//
//    // 物品数据池
//    std::vector<ItemBatchData> itemBatchData;
//    itemBatchData.reserve(1000);
//
//    // 自适应睡眠时间
//    std::chrono::milliseconds adaptiveSleepTime(1);
//
//    while (m_running) {
//        try {
//            auto frameStartTime = std::chrono::steady_clock::now();
//
//            {
//                boost::unique_lock<boost::mutex> lock(m_threadSyncMutex);
//                if (m_clearingEntities) {
//                    // 暂停时减少活动线程计数
//                    m_activeThreadCount--;
//
//                    // 通知主线程此线程已暂停
//                    m_threadSyncCV.notify_all();
//
//                    // 等待清理完成信号
//                    m_threadSyncCV.wait(lock, [this]() {
//                        return !m_clearingEntities;
//                        });
//
//                    // 恢复时增加活动线程计数
//                    m_activeThreadCount++;
//                }
//            }
//
//            if (!m_loggedIn) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 基本状态检查 - 如果不在游戏中或处于战斗模式，则不扫描物品
//            if (!m_inGame || m_battleMode || !esp->items.globalEnabled) {
//                // 在战斗模式下清理物品列表以提高性能
//                if (m_battleMode) {
//                    updateItems([](std::vector<Item*>& items) {
//                        for (auto* item : items) delete item;
//                        items.clear();
//                        });
//
//                    // 清空队列中的物品
//                    Item* item;
//                    while (m_newItemQueue.pop(item)) {
//                        delete item;
//                    }
//                }
//                std::this_thread::sleep_for(std::chrono::milliseconds(50));
//                continue;
//            }
//
//            // 更新现有物品
//            auto items = getItems();
//            if (items->empty()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 收集有效物品
//            std::vector<Item*> itemsCopy;
//            itemsCopy.reserve(items->size());
//
//            for (const auto& item : *items) {
//                if (item && mem.IsValidPointer(item->BasePointer) && item->IsValid()) {
//                    itemsCopy.push_back(item);
//                }
//            }
//
//            if (itemsCopy.empty()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 调整批处理数据容器大小
//            itemBatchData.resize(itemsCopy.size());
//            auto handle = mem.CreateScatterHandle();
//            if (!handle) {
//                mem.CloseScatterHandle(handle);
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                continue;
//            }
//
//            // 批量读取物品数据
//            for (size_t i = 0; i < itemsCopy.size(); i++) {
//                Item* item = itemsCopy[i];
//                auto& data = itemBatchData[i];
//
//                mem.AddScatterReadRequest(handle, item->BasePointer + OFF_LOCAL_ORIGIN,
//                    &data.origin, sizeof(Vec3D));
//                mem.AddScatterReadRequest(handle, item->BasePointer + OFF_CUSTOM_SPIRIT,
//                    &data.lootIndex, sizeof(int));
//                mem.AddScatterReadRequest(handle, item->BasePointer,
//                    &data.valid, sizeof(uint64_t));
//            }
//
//            mem.ExecuteReadScatter(handle);
//            mem.CloseScatterHandle(handle);
//            // 更新物品并标记无效物品
//            std::vector<Item*> itemsToRemove;
//            itemsToRemove.reserve(itemsCopy.size() / 4);
//
//            const float ItemDistance = esp->items.maxDistance;
//
//            for (size_t i = 0; i < itemsCopy.size(); i++) {
//                Item* item = itemsCopy[i];
//                const auto& data = itemBatchData[i];
//
//                // 验证物品有效性
//                if (!mem.IsValidPointer(data.valid) || !item->LootIndex) {
//                    itemsToRemove.push_back(item);
//                    continue;
//                }
//
//                // 更新物品属性
//                item->Origin = data.origin;
//                item->LootIndex = data.lootIndex;
//                item->valid = data.valid;
//
//                // 更新距离并检查是否超出范围
//                item->distance_to_local = item->Origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;
//                if (item->distance_to_local >= ItemDistance) {
//                    itemsToRemove.push_back(item);
//                    continue;
//                }
//            }
//
//            // 从全局列表中移除无效物品
//            if (!itemsToRemove.empty()) {
//                // 收集需要删除的索引
//                std::vector<int> indicesToRemove;
//                indicesToRemove.reserve(itemsToRemove.size());
//
//                for (Item* item : itemsToRemove) {
//                    if (item) {
//                        indicesToRemove.push_back(item->Index);
//                    }
//                }
//
//                // 更新索引集
//                {
//                    boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//                    for (int index : indicesToRemove) {
//                        m_itemIndices.erase(index);
//                    }
//                }
//
//                // 更新物品列表，删除无效项
//                m_items.removeItems(itemsToRemove);
//
//                // 删除无效物品释放内存
//                for (auto* item : itemsToRemove) {
//                    delete item;
//                }
//            }
//
//            // 自适应睡眠时间
//            auto frameEndTime = std::chrono::steady_clock::now();
//            auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime);
//
//            // 根据处理时间调整睡眠时间
//            adaptiveSleepTime = std::chrono::milliseconds(
//                std::min(10, std::max(1, 2 - static_cast<int>(frameDuration.count() / 1000))));
//
//            std::this_thread::sleep_for(adaptiveSleepTime);
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error in ItemScatter thread: " << e.what() << std::endl;
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//        }
//    }
//
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount--;
//    }
//    m_threadSyncCV.notify_all();
//}
//
//// 护盾处理线程 - 优化版本
//void EntityManager::shieldScatterThread() {
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
//    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 2);
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount++;
//    }
//
//    // 护盾数据池
//    std::vector<ShieldBatchData> shieldData;
//    shieldData.reserve(100);
//
//    // 本地分类护盾容器
//    CategorizedShields localCategories;
//    localCategories.gibraltarShields.reserve(20);
//    localCategories.rampartShields.reserve(20);
//    localCategories.newcastleShields.reserve(20);
//    localCategories.lifelineShields.reserve(20);
//
//    bool needUpdateCategories = false;
//    std::chrono::milliseconds adaptiveSleepTime(1);
//
//    while (m_running) {
//        try {
//            auto frameStartTime = std::chrono::steady_clock::now();
//
//            {
//                boost::unique_lock<boost::mutex> lock(m_threadSyncMutex);
//                if (m_clearingEntities) {
//                    // 暂停时减少活动线程计数
//                    m_activeThreadCount--;
//
//                    // 通知主线程此线程已暂停
//                    m_threadSyncCV.notify_all();
//
//                    // 等待清理完成信号
//                    m_threadSyncCV.wait(lock, [this]() {
//                        return !m_clearingEntities;
//                        });
//
//                    // 恢复时增加活动线程计数
//                    m_activeThreadCount++;
//                }
//            }
//
//            if (!m_loggedIn || !m_inGame) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 更新现有护盾
//            auto shields = getShields();
//            if (shields->empty()) {
//                // 如果没有护盾，清空分类护盾
//                if (!localCategories.gibraltarShields.empty() || !localCategories.rampartShields.empty() ||
//                    !localCategories.newcastleShields.empty() || !localCategories.lifelineShields.empty()) {
//                    localCategories.gibraltarShields.clear();
//                    localCategories.rampartShields.clear();
//                    localCategories.newcastleShields.clear();
//                    localCategories.lifelineShields.clear();
//
//                    // 更新护盾分类
//                    updateShieldCategories(localCategories);
//                }
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 收集有效护盾
//            std::vector<Shield*> shieldsCopy;
//            shieldsCopy.reserve(shields->size());
//
//            for (const auto& shield : *shields) {
//                if (shield && mem.IsValidPointer(shield->BasePointer)) {
//                    shieldsCopy.push_back(shield);
//                }
//            }
//
//            if (shieldsCopy.empty()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                continue;
//            }
//
//            // 调整数据容器大小
//            shieldData.resize(shieldsCopy.size());
//            auto handle = mem.CreateScatterHandle();
//            if (!handle) {
//                mem.CloseScatterHandle(handle);
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                continue;
//            }
//
//            // 批量读取护盾数据
//            for (size_t i = 0; i < shieldsCopy.size(); i++) {
//                Shield* shield = shieldsCopy[i];
//                auto& data = shieldData[i];
//
//                mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_LOCAL_ORIGIN,
//                    &data.origin, sizeof(Vec3D));
//                mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_ENTITY_ROTATION,
//                    &data.rotation, sizeof(Vec3D));
//                mem.AddScatterReadRequest(handle, shield->BasePointer,
//                    &data.valid, sizeof(uint64_t));
//                mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_PROP_SCRIPT_NETWORKFLAG,
//                    &data.networkFlag, sizeof(int));
//            }
//
//            mem.ExecuteReadScatter(handle);
//            mem.CloseScatterHandle(handle);
//
//            // 更新护盾状态和标记无效护盾
//            const float ESPDistance = esp->enemies.distance;
//            std::vector<Shield*> shieldsToRemove;
//            shieldsToRemove.reserve(shieldsCopy.size() / 4);
//
//            bool shieldDataChanged = false;  // 跟踪是否有护盾数据变化
//
//            for (size_t i = 0; i < shieldsCopy.size(); i++) {
//                Shield* shield = shieldsCopy[i];
//                const auto& data = shieldData[i];
//
//                if (!mem.IsValidPointer(data.valid)) {
//                    shieldsToRemove.push_back(shield);
//                    shieldDataChanged = true;
//                    continue;
//                }
//
//                float distance = data.origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;
//                if (distance >= ESPDistance) {
//                    shieldsToRemove.push_back(shield);
//                    shieldDataChanged = true;
//                    continue;
//                }
//
//                // 检查是否有数据变化
//                if (shield->Origin.x != data.origin.x || shield->Origin.y != data.origin.y ||
//                    shield->Origin.z != data.origin.z || shield->rotation.y != data.rotation.y ||
//                    shield->networkFlag != data.networkFlag) {
//                    shieldDataChanged = true;
//                }
//
//                shield->Origin = data.origin;
//                shield->rotation = data.rotation;
//                shield->networkFlag = data.networkFlag;
//                shield->distance_to_local = distance;
//                shield->valid = data.valid;
//            }
//
//            // 删除无效护盾
//            if (!shieldsToRemove.empty()) {
//                // 收集需要删除的索引
//                std::vector<int> indicesToRemove;
//                indicesToRemove.reserve(shieldsToRemove.size());
//
//                for (Shield* shield : shieldsToRemove) {
//                    if (shield) {
//                        indicesToRemove.push_back(shield->Index);
//                    }
//                }
//
//                // 更新索引集
//                {
//                    boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//                    for (int index : indicesToRemove) {
//                        m_shieldIndices.erase(index);
//                    }
//                }
//
//                // 更新护盾列表
//                m_shields.removeItems(shieldsToRemove);
//
//                // 删除无效护盾释放内存
//                for (auto* shield : shieldsToRemove) {
//                    delete shield;
//                }
//
//                shieldDataChanged = true;
//            }
//
//            // 更新护盾分类（如果需要）
//            if (needUpdateCategories || shieldDataChanged) {
//                updateShieldClassification();
//                needUpdateCategories = false;
//            }
//
//            // 自适应睡眠
//            auto frameEndTime = std::chrono::steady_clock::now();
//            auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime);
//
//            adaptiveSleepTime = std::chrono::milliseconds(
//                std::min(10, std::max(1, 2 - static_cast<int>(frameDuration.count() / 1000))));
//
//            std::this_thread::sleep_for(adaptiveSleepTime);
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Error in ShieldScatter thread: " << e.what() << std::endl;
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//        }
//    }
//
//    {
//        boost::lock_guard<boost::mutex> lock(m_threadSyncMutex);
//        m_activeThreadCount--;
//    }
//    m_threadSyncCV.notify_all();
//}
//
//// 处理新实体 - 优化版本
//void EntityManager::processNewPlayers(std::vector<Player*>& newPlayers) {
//    if (newPlayers.empty()) return;
//
//    auto handle = mem.CreateScatterHandle();
//    if (!handle) {
//        mem.CloseScatterHandle(handle);
//        for (auto* player : newPlayers) delete player;
//        return;
//    }
//
//    // 收集需要检查的索引
//    std::vector<Player*> uniquePlayers;
//    uniquePlayers.reserve(newPlayers.size());
//
//    std::vector<int> playerIndicesToCheck;
//    playerIndicesToCheck.reserve(newPlayers.size());
//
//    for (Player* player : newPlayers) {
//        if (!player || !mem.IsValidPointer(player->BasePointer)) {
//            delete player;
//            continue;
//        }
//        playerIndicesToCheck.push_back(player->Index);
//        uniquePlayers.push_back(player);
//    }
//
//    if (uniquePlayers.empty()) {
//        mem.CloseScatterHandle(handle);
//        return;
//    }
//
//    // 检查重复索引并移除重复玩家
//    {
//        boost::shared_lock<boost::shared_mutex> lock(m_indicesMutex);
//
//        // 找出已存在的索引对应的位置
//        std::vector<size_t> duplicateIndices;
//        for (size_t i = 0; i < playerIndicesToCheck.size(); i++) {
//            if (m_playerIndices.count(playerIndicesToCheck[i]) > 0) {
//                duplicateIndices.push_back(i);
//            }
//        }
//
//        // 如果没有找到重复项，跳过锁升级
//        if (!duplicateIndices.empty()) {
//            // 释放共享锁，准备获取排他锁
//            lock.unlock();
//
//            // 获取排他锁，用于精确检查和更新操作
//            boost::unique_lock<boost::shared_mutex> writeLock(m_indicesMutex);
//
//            // 再次检查避免条件竞争
//            duplicateIndices.clear();
//            for (size_t i = 0; i < playerIndicesToCheck.size(); i++) {
//                if (m_playerIndices.count(playerIndicesToCheck[i]) > 0) {
//                    duplicateIndices.push_back(i);
//                }
//            }
//
//            // 移除重复的玩家（从后向前删除避免影响索引）
//            for (auto it = duplicateIndices.rbegin(); it != duplicateIndices.rend(); ++it) {
//                delete uniquePlayers[*it];
//                uniquePlayers.erase(uniquePlayers.begin() + *it);
//                playerIndicesToCheck.erase(playerIndicesToCheck.begin() + *it);
//            }
//        }
//    }
//    // 如果没有唯一玩家剩余，直接返回
//    if (uniquePlayers.empty()) {
//        mem.CloseScatterHandle(handle);
//        return;
//    }
//
//    // 添加读取请求
//    for (Player* player : uniquePlayers) {
//        mem.AddScatterReadRequest(handle, player->BasePointer + OFF_NAME,
//            player->NameBuffer, sizeof(player->NameBuffer));
//        mem.AddScatterReadRequest(handle, player->BasePointer,
//            &player->valid, sizeof(player->valid));
//        mem.AddScatterReadRequest(handle, player->BasePointer + OFF_TEAM_NUMBER,
//            &player->Team, sizeof(int));
//        mem.AddScatterReadRequest(handle, player->BasePointer + OFF_XP,
//            &player->Level, sizeof(int));
//        mem.AddScatterReadRequest(handle, player->BasePointer + OFF_UID,
//            &player->UserId, sizeof(uint64_t));
//        mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_NAME_LIST + (player->Index) * 24,
//            &player->NameList, sizeof(uint64_t));
//    }
//
//    mem.ExecuteReadScatter(handle);
//    mem.CloseScatterHandle(handle);
//
//    // 读取玩家名称
//    auto nameHandle = mem.CreateScatterHandle();
//    if (nameHandle) {
//        for (Player* player : uniquePlayers) {
//            if (!player || !mem.IsValidPointer(player->NameList)) continue;
//            mem.AddScatterReadRequest(nameHandle, player->NameList, player->PlayerName, 32);
//        }
//
//        mem.ExecuteReadScatter(nameHandle);
//        mem.CloseScatterHandle(nameHandle);
//
//        const int localTeam = localPlayer->Team;
//        std::vector<Player*> validPlayers;
//        validPlayers.reserve(uniquePlayers.size());
//
//        // 收集有效索引
//        std::vector<int> validIndices;
//        validIndices.reserve(uniquePlayers.size());
//
//        for (Player* player : uniquePlayers) {
//            if (!player) continue;
//
//            if (!mem.IsValidPointer(player->BasePointer)) {
//                delete player;
//                continue;
//            }
//
//            if (player->valid) {
//                if (map->m_mixtape && (localTeam & 1) == (player->Team & 1)) {
//                    player->Team = localTeam;
//                }
//
//                // 读取网络变量
//                player->NetVarGlobal = mem.ReadCache<uint64_t>(player->BasePointer + OFF_NETVAR_GLOBAL);
//                player->NetVarData = mem.ReadCache<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + ((player->NetVarGlobal & 0xffff) << 5));
//
//                static uint64_t OFF_Rank_Ptr = 0;
//                if (OFF_Rank_Ptr == 0) {
//                    uint64_t RankNetVarTableAddr = mem.OFF_BASE + OFF_NETVAR_TABLE_PTR + static_cast<unsigned long long>(13) * 56 + 16;
//                    OFF_Rank_Ptr = mem.Read<uint64_t>(RankNetVarTableAddr) * 2;
//                }
//
//                player->Rank = mem.ReadCache<uint32_t>(player->NetVarData + OFF_NETVAR_INT32S + OFF_Rank_Ptr);
//
//                player->Level = player->GetPlayerLevel();
//                player->RankName = player->getRankName();
//                player->IsPro = player->UserId > 0 && player->UserId < 1000000000;  // 简化的PR检测
//                player->NickName = player->IsDummy() ? "Bot" : player->PlayerName;
//                player->IsLocal = localPlayer->BasePointer == player->BasePointer;
//                player->IsTeamMate = player->Team == localTeam;
//                player->IsHostile = !player->IsTeamMate;
//
//                validPlayers.push_back(player);
//                validIndices.push_back(player->Index);
//            }
//            else {
//                delete player;
//            }
//        }
//        // 使用写锁更新索引集
//        if (!validIndices.empty()) {
//            boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//            for (int index : validIndices) {
//                m_playerIndices.insert(index);
//            }
//        }
//
//        if (!validPlayers.empty()) {
//            addPlayers(validPlayers);
//        }
//    }
//    else {
//        // 清理资源
//        for (auto* player : uniquePlayers) {
//            delete player;
//        }
//    }
//}
//
//// 处理新物品 - 优化版本
//void EntityManager::processNewItems(std::vector<Item*>& newItems) {
//    if (newItems.empty()) return;
//
//    auto handle = mem.CreateScatterHandle();
//    if (!handle) {
//        mem.CloseScatterHandle(handle);
//        for (auto* item : newItems) delete item;
//        return;
//    }
//
//    // 收集需要检查的索引
//    std::vector<Item*> uniqueItems;
//    uniqueItems.reserve(newItems.size());
//
//    std::vector<int> itemIndicesToCheck;
//    itemIndicesToCheck.reserve(newItems.size());
//
//    for (Item* item : newItems) {
//        if (!item || !mem.IsValidPointer(item->BasePointer)) {
//            delete item;
//            continue;
//        }
//        itemIndicesToCheck.push_back(item->Index);
//        uniqueItems.push_back(item);
//    }
//
//    if (uniqueItems.empty()) {
//        mem.CloseScatterHandle(handle);
//        return;
//    }
//
//    // 检查重复索引并移除重复物品 - 使用读写锁优化
//    {
//        boost::shared_lock<boost::shared_mutex> readLock(m_indicesMutex);
//
//        // 检查是否有重复
//        bool hasDuplicates = false;
//        for (int index : itemIndicesToCheck) {
//            if (m_itemIndices.count(index) > 0) {
//                hasDuplicates = true;
//                break;
//            }
//        }
//
//        if (hasDuplicates) {
//            // 升级到写锁
//            readLock.unlock();
//            boost::unique_lock<boost::shared_mutex> writeLock(m_indicesMutex);
//
//            // 找出已存在的索引对应的位置
//            std::vector<size_t> duplicateIndices;
//            for (size_t i = 0; i < itemIndicesToCheck.size(); i++) {
//                if (m_itemIndices.count(itemIndicesToCheck[i]) > 0) {
//                    duplicateIndices.push_back(i);
//                }
//            }
//
//            // 移除重复的物品（从后向前删除避免影响索引）
//            for (auto it = duplicateIndices.rbegin(); it != duplicateIndices.rend(); ++it) {
//                delete uniqueItems[*it];
//                uniqueItems.erase(uniqueItems.begin() + *it);
//                itemIndicesToCheck.erase(itemIndicesToCheck.begin() + *it);
//            }
//        }
//    }
//    // 如果没有唯一物品剩余，直接返回
//    if (uniqueItems.empty()) {
//        mem.CloseScatterHandle(handle);
//        return;
//    }
//
//    // 添加读取请求
//    for (Item* item : uniqueItems) {
//        mem.AddScatterReadRequest(handle, item->BasePointer,
//            &item->valid, sizeof(uint64_t));
//        mem.AddScatterReadRequest(handle, item->BasePointer + OFF_LOCAL_ORIGIN,
//            &item->Origin, sizeof(Vec3D));
//        mem.AddScatterReadRequest(handle, item->BasePointer + OFF_CUSTOM_SPIRIT,
//            &item->LootIndex, sizeof(int));
//        mem.AddScatterReadRequest(handle, item->BasePointer + OFF_SignifierName,
//            &item->name_address, sizeof(uint64_t));
//    }
//
//    mem.ExecuteReadScatter(handle);
//    mem.CloseScatterHandle(handle);
//
//    // 读取物品名称
//    auto nameHandle = mem.CreateScatterHandle();
//    if (nameHandle) {
//        for (Item* item : uniqueItems) {
//            if (mem.IsValidPointer(item->name_address)) {
//                mem.AddScatterReadRequest(nameHandle, item->name_address,
//                    item->NameBuffer, sizeof(item->NameBuffer));
//            }
//        }
//
//        mem.ExecuteReadScatter(nameHandle);
//        mem.CloseScatterHandle(nameHandle);
//
//        const float ItemDistance = esp->items.maxDistance;
//        std::vector<Item*> validItems;
//        validItems.reserve(uniqueItems.size());
//
//        // 收集有效索引
//        std::vector<int> validIndices;
//        validIndices.reserve(uniqueItems.size());
//
//        for (Item* item : uniqueItems) {
//            if (!item) continue;
//
//            item->ValidCheck();
//            if (item->IsValid()) {
//                item->distance_to_local = item->Origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;
//                if (item->distance_to_local < ItemDistance) {
//                    validItems.push_back(item);
//                    validIndices.push_back(item->Index);
//                }
//                else {
//                    delete item;
//                }
//            }
//            else {
//                delete item;
//            }
//        }
//
//        // 使用写锁更新索引集
//        if (!validIndices.empty()) {
//            boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//            for (int index : validIndices) {
//                m_itemIndices.insert(index);
//            }
//        }
//
//        if (!validItems.empty()) {
//            addItems(validItems);
//        }
//    }
//    else {
//        // 清理资源
//        for (auto* item : uniqueItems) {
//            delete item;
//        }
//    }
//}
//
//// 处理新护盾 - 优化版本
//void EntityManager::processNewShields(std::vector<Shield*>& newShields) {
//    if (newShields.empty()) return;
//
//    auto handle = mem.CreateScatterHandle();
//    if (!handle) {
//        mem.CloseScatterHandle(handle);
//        for (auto* shield : newShields) delete shield;
//        return;
//    }
//
//    // 收集需要检查的索引
//    std::vector<Shield*> uniqueShields;
//    uniqueShields.reserve(newShields.size());
//
//    std::vector<int> shieldIndicesToCheck;
//    shieldIndicesToCheck.reserve(newShields.size());
//
//    for (Shield* shield : newShields) {
//        if (!shield || !mem.IsValidPointer(shield->BasePointer)) {
//            delete shield;
//            continue;
//        }
//        shieldIndicesToCheck.push_back(shield->Index);
//        uniqueShields.push_back(shield);
//    }
//
//    if (uniqueShields.empty()) {
//        mem.CloseScatterHandle(handle);
//        return;
//    }
//
//    // 检查重复索引并移除重复护盾 - 使用读写锁优化
//    {
//        boost::shared_lock<boost::shared_mutex> readLock(m_indicesMutex);
//
//        // 检查是否有重复
//        bool hasDuplicates = false;
//        for (int index : shieldIndicesToCheck) {
//            if (m_shieldIndices.count(index) > 0) {
//                hasDuplicates = true;
//                break;
//            }
//        }
//
//        if (hasDuplicates) {
//            // 升级到写锁
//            readLock.unlock();
//            boost::unique_lock<boost::shared_mutex> writeLock(m_indicesMutex);
//
//            // 找出已存在的索引对应的位置
//            std::vector<size_t> duplicateIndices;
//            for (size_t i = 0; i < shieldIndicesToCheck.size(); i++) {
//                if (m_shieldIndices.count(shieldIndicesToCheck[i]) > 0) {
//                    duplicateIndices.push_back(i);
//                }
//            }
//
//            // 移除重复的护盾（从后向前删除避免影响索引）
//            for (auto it = duplicateIndices.rbegin(); it != duplicateIndices.rend(); ++it) {
//                delete uniqueShields[*it];
//                uniqueShields.erase(uniqueShields.begin() + *it);
//                shieldIndicesToCheck.erase(shieldIndicesToCheck.begin() + *it);
//            }
//        }
//    }
//
//    // 如果没有唯一护盾剩余，直接返回
//    if (uniqueShields.empty()) {
//        mem.CloseScatterHandle(handle);
//        return;
//    }
//
//    // 添加读取请求
//    for (Shield* shield : uniqueShields) {
//        mem.AddScatterReadRequest(handle, shield->BasePointer,
//            &shield->valid, sizeof(uint64_t));
//        mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_LOCAL_ORIGIN,
//            &shield->Origin, sizeof(Vec3D));
//        mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_ENTITY_ROTATION,
//            &shield->rotation, sizeof(Vec3D));
//        mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_PROP_SCRIPT_NETWORKFLAG,
//            &shield->networkFlag, sizeof(int));
//    }
//
//    mem.ExecuteReadScatter(handle);
//    mem.CloseScatterHandle(handle);
//
//    const float ESPDistance = esp->enemies.distance;
//    std::vector<Shield*> validShields;
//    validShields.reserve(uniqueShields.size());
//
//    // 收集有效索引
//    std::vector<int> validIndices;
//    validIndices.reserve(uniqueShields.size());
//
//    for (Shield* shield : uniqueShields) {
//        if (!shield) continue;
//
//        shield->ValidCheck();
//        if (shield->valid &&
//            shield->Origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f < ESPDistance) {
//
//            shield->distance_to_local = shield->Origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;
//            validShields.push_back(shield);
//            validIndices.push_back(shield->Index);
//        }
//        else {
//            delete shield;
//        }
//    }
//
//    // 使用写锁更新索引集
//    if (!validIndices.empty()) {
//        boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//        for (int index : validIndices) {
//            m_shieldIndices.insert(index);
//        }
//    }
//
//    if (!validShields.empty()) {
//        addShields(validShields);
//    }
//}
//
//
//bool EntityManager::gatherGameData(LocalPlayer* localPlayer) {
//    auto handle = mem.CreateScatterHandle();
//    if (!handle) { mem.CloseScatterHandle(handle); return false; }
//
//    uint64_t gameModeAddress = mem.OFF_BASE + OFF_GAMEMODE;
//    mem.AddScatterReadRequest(handle, gameModeAddress, &map->gameModePtr, sizeof(uint64_t));
//
//    uint64_t localPlayerAddress = mem.OFF_BASE + OFF_LOCAL_PLAYER;
//    mem.AddScatterReadRequest(handle, localPlayerAddress, &localPlayer->BasePointer, sizeof(uint64_t));
//
//    uint64_t inAttackAddress = mem.OFF_BASE + OFF_INATTACK;
//    mem.AddScatterReadRequest(handle, inAttackAddress, &localPlayer->InAttack, sizeof(uint32_t));
//
//    // 靈敏度 默認 5.0
//    mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_MOUSE_SENSITIVITY + 0x60, &localPlayer->mouseSensitive, sizeof(float));
//
//    // 如果偏移还未初始化，获取偏移
//    if (OFF_Kills_Ptr == 0) {
//        uint64_t KillNetVarTableAddr = mem.OFF_BASE + OFF_NETVAR_TABLE_PTR + static_cast<unsigned long long>(153) * 56 + 16;
//        mem.AddScatterReadRequest(handle, KillNetVarTableAddr, &OFF_Kills_Ptr, sizeof(OFF_Kills_Ptr));
//    }
//
//    if (OFF_Damage_Ptr == 0) {
//        uint64_t DamageNetVarTableAddr = mem.OFF_BASE + OFF_NETVAR_TABLE_PTR + static_cast<unsigned long long>(9) * 56 + 16;
//        mem.AddScatterReadRequest(handle, DamageNetVarTableAddr, &OFF_Damage_Ptr, sizeof(OFF_Damage_Ptr));
//    }
//
//    mem.ExecuteReadScatter(handle);
//    mem.CloseScatterHandle(handle);
//
//    // 如果是首次读取，处理偏移
//    if (OFF_Kills_Ptr != 0 && OFF_Damage_Ptr != 0) {
//        OFF_Kills_Ptr *= 2;
//        OFF_Damage_Ptr *= 2;
//    }
//
//    return scatterLocalPlayer(localPlayer);
//}
//
//// scatterLocalPlayer 方法 - 优化版本
//bool EntityManager::scatterLocalPlayer(LocalPlayer* localPlayer) {
//    if (!mem.IsValidPointer(localPlayer->BasePointer)) return false;
//
//    auto handle = mem.CreateScatterHandle();
//    if (!handle) { mem.CloseScatterHandle(handle); return false; }
//
//    // 散射读取本地玩家基本数据
//    uint64_t isDeadAddress = localPlayer->BasePointer + OFF_LIFE_STATE;
//    mem.AddScatterReadRequest(handle, isDeadAddress, &localPlayer->IsDead, sizeof(bool));
//
//    uint64_t isKnockedAddress = localPlayer->BasePointer + OFF_BLEEDOUT_STATE;
//    mem.AddScatterReadRequest(handle, isKnockedAddress, &localPlayer->IsKnocked, sizeof(bool));
//
//    uint64_t isZoomingAddress = localPlayer->BasePointer + OFF_ZOOMING;
//    mem.AddScatterReadRequest(handle, isZoomingAddress, &localPlayer->IsZooming, sizeof(bool));
//
//    uint64_t teamAddress = localPlayer->BasePointer + OFF_TEAM_NUMBER;
//    mem.AddScatterReadRequest(handle, teamAddress, &localPlayer->Team, sizeof(int));
//
//    uint64_t localOriginAddress = localPlayer->BasePointer + OFF_LOCAL_ORIGIN;
//    mem.AddScatterReadRequest(handle, localOriginAddress, &localPlayer->Origin, sizeof(Vec3D));
//
//    uint64_t cameraPositionAddress = localPlayer->BasePointer + OFF_CAMERAORIGIN;
//    mem.AddScatterReadRequest(handle, cameraPositionAddress, &localPlayer->CameraOrigin, sizeof(Vec3D));
//
//    uint64_t viewAnglesAddress = localPlayer->BasePointer + OFF_VIEW_ANGLES;
//    mem.AddScatterReadRequest(handle, viewAnglesAddress, &localPlayer->ViewAngles, sizeof(Vec2D));
//
//    uint64_t breathAddress = localPlayer->BasePointer + OFF_BREATH_ANGLES;
//    mem.AddScatterReadRequest(handle, breathAddress, &localPlayer->SwayAngles, sizeof(Vec2D));
//
//    uint64_t weaponHandleAddress = localPlayer->BasePointer + OFF_ACTIVE_WEAPON;
//    mem.AddScatterReadRequest(handle, weaponHandleAddress, &localPlayer->WeaponHandle, sizeof(uint64_t));
//
//    uint64_t FlagsAddress = localPlayer->BasePointer + OFF_FLAGS;
//    mem.AddScatterReadRequest(handle, FlagsAddress, &localPlayer->Flags, sizeof(int));
//
//    uint64_t GrappleActived = localPlayer->BasePointer + OFF_GRAPPLEACTIVED;
//    mem.AddScatterReadRequest(handle, GrappleActived, &localPlayer->isGrppleActived, sizeof(bool));
//
//    uint64_t isGRAPPLEAddress = localPlayer->BasePointer + OFF_GRAPPLE + OFF_GRAPPLEATTACHED;
//    mem.AddScatterReadRequest(handle, isGRAPPLEAddress, &localPlayer->isGRAPPLE, sizeof(int));
//
//    uint64_t PuchAngles = localPlayer->BasePointer + OFF_PUNCH_ANGLES;
//    mem.AddScatterReadRequest(handle, PuchAngles, &localPlayer->AimPunch, sizeof(Vec3D));
//
//    mem.ExecuteReadScatter(handle);
//    mem.CloseScatterHandle(handle);
//
//    return true;
//}
//
//// 收集武器信息 - 优化版本
//void EntityManager::scatterWeaponInfo(LocalPlayer* localPlayer) {
//    if (localPlayer->IsDead || localPlayer->IsKnocked || !localPlayer->WeaponHandle)
//        return;
//
//    uint64_t WeaponHandleMasked = localPlayer->WeaponHandle & 0xffff;
//    localPlayer->WeaponEntity = mem.Read<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + (WeaponHandleMasked << 5));
//
//    auto handle = mem.CreateScatterHandle();
//    if (!handle) { mem.CloseScatterHandle(handle); return; }
//
//    // 散射读取武器数据
//    uint64_t zoomFOVAddress = localPlayer->WeaponEntity + OFF_CURRENTZOOMFOV;
//    mem.AddScatterReadRequest(handle, zoomFOVAddress, &localPlayer->ZoomFOV, sizeof(float));
//
//    uint64_t targetZoomFOVAddress = localPlayer->WeaponEntity + OFF_TARGETZOOMFOV;
//    mem.AddScatterReadRequest(handle, targetZoomFOVAddress, &localPlayer->TargetZoomFOV, sizeof(float));
//
//    uint64_t weaponIndexAddress = localPlayer->WeaponEntity + OFF_WEAPON_INDEX;
//    mem.AddScatterReadRequest(handle, weaponIndexAddress, &localPlayer->WeaponIndex, sizeof(int));
//
//    uint64_t mod_bitfieldAddress = localPlayer->WeaponEntity + OFF_WEAPON_BITFIELD;
//    mem.AddScatterReadRequest(handle, mod_bitfieldAddress, &localPlayer->weapon_mod_bitfield, sizeof(int));
//
//    uint64_t weaponProjectileSpeedAddress = localPlayer->WeaponEntity + OFF_PROJECTILESPEED;
//    mem.AddScatterReadRequest(handle, weaponProjectileSpeedAddress, &localPlayer->WeaponProjectileSpeed, sizeof(float));
//
//    uint64_t weaponProjectileScaleAddress = localPlayer->WeaponEntity + OFF_PROJECTILESCALE;
//    mem.AddScatterReadRequest(handle, weaponProjectileScaleAddress, &localPlayer->WeaponProjectileScale, sizeof(float));
//
//    uint64_t playerAbsoluteVelocityAddress = localPlayer->BasePointer + OFF_ABSVELOCITY;
//    mem.AddScatterReadRequest(handle, playerAbsoluteVelocityAddress, &localPlayer->AbsoluteVelocity, sizeof(Vec3D));
//
//    uint64_t weaponAmmoAddress = localPlayer->WeaponEntity + OFF_WEAPON_AMMO;
//    mem.AddScatterReadRequest(handle, weaponAmmoAddress, &localPlayer->Ammo, sizeof(int));
//
//    uint64_t isReloadingAddress = localPlayer->WeaponEntity + OFF_RELOADING;
//    mem.AddScatterReadRequest(handle, isReloadingAddress, &localPlayer->IsReloading, sizeof(bool));
//
//    mem.ExecuteReadScatter(handle);
//    mem.CloseScatterHandle(handle);
//
//    localPlayer->IsHoldingGrenade = localPlayer->WeaponIndex == 146 || localPlayer->WeaponIndex == 147 || localPlayer->WeaponIndex == 148;
//    localPlayer->IsEmptyHand = localPlayer->WeaponIndex == 149 || localPlayer->WeaponIndex == 150;
//}
//
//// 清理所有实体内部方法 - 优化版本
//void EntityManager::clearAllEntitiesInternal() {
//    // 确保所有线程暂停
//    suspendAllThreads();
//
//    // 清空无锁队列
//    Player* player;
//    Item* item;
//    Shield* shield;
//
//    // 清空队列并释放内存
//    while (m_newPlayerQueue.pop(player)) {
//        delete player;
//    }
//
//    while (m_newItemQueue.pop(item)) {
//        delete item;
//    }
//
//    while (m_newShieldQueue.pop(shield)) {
//        delete shield;
//    }
//
//    // 清理玩家列表
//    m_players.updateItems([](std::vector<Player*>& players) {
//        for (auto* player : players) {
//            delete player;
//        }
//        players.clear();
//        });
//
//    // 清理物品列表
//    m_items.updateItems([](std::vector<Item*>& items) {
//        for (auto* item : items) {
//            delete item;
//        }
//        items.clear();
//        });
//
//    // 清理护盾列表
//    m_shields.updateItems([](std::vector<Shield*>& shields) {
//        for (auto* shield : shields) {
//            delete shield;
//        }
//        shields.clear();
//        });
//
//    // 清理护盾分类
//    m_shieldCategories.updateItems([](std::vector<CategorizedShields*>& categories) {
//        for (auto* category : categories) {
//            delete category;
//        }
//        categories.clear();
//        });
//
//    // 清空索引集
//    {
//        boost::unique_lock<boost::shared_mutex> lock(m_indicesMutex);
//        m_playerIndices.clear();
//        m_itemIndices.clear();
//        m_shieldIndices.clear();
//    }
//
//    // 恢复所有线程
//    resumeAllThreads();
//}
//
//// 检测玩家是否被护盾阻挡 - 优化版本
//bool EntityManager::isPlayerBlockedByShields(Player* player) {
//    // 如果玩家已经不可见，无需进一步检查
//    if (!player->IsVisible) {
//        return true;
//    }
//
//    // 过滤不需要检测的玩家
//    if (!player->valid || player->IsLocal ||
//        player->IsDead || player->Health <= 0 || player->IsTeamMate) {
//        return false;
//    }
//
//    // 距离检查
//    if (player->DistanceToLocalPlayer > esp->enemies.distance) {
//        return false;
//    }
//
//    // 获取分类护盾列表
//    auto categoriesVector = m_shieldCategories.getItems();
//    if (!categoriesVector || categoriesVector->empty()) {
//        return false;
//    }
//
//    // 获取并解引用
//    CategorizedShields categories = *(*categoriesVector)[0];
//
//    // 优化1：检查护盾数量，快速返回
//    bool hasShields = !categories.newcastleShields.empty() ||
//        !categories.gibraltarShields.empty() ||
//        !categories.rampartShields.empty() ||
//        !categories.lifelineShields.empty();
//
//    if (!hasShields) {
//        return false;
//    }
//
//    // 在锁外进行护盾碰撞检测
//    const Vec3D& playerOrigin = player->Origin;
//    const Vec3D& cameraOrigin = localPlayer->CameraOrigin;
//    const Vec2D& viewAngles = localPlayer->ViewAngles;
//
//    // 优化2：按性能成本从低到高排序碰撞检测
//
//    // 计算玩家到本地玩家的方向向量，用于所有护盾检测
//    Vec3D directionToPlayer = {
//        playerOrigin.x - cameraOrigin.x,
//        playerOrigin.y - cameraOrigin.y,
//        playerOrigin.z - cameraOrigin.z
//    };
//    float distanceToPlayer = std::sqrt(
//        directionToPlayer.x * directionToPlayer.x +
//        directionToPlayer.y * directionToPlayer.y +
//        directionToPlayer.z * directionToPlayer.z
//    );
//
//    // 归一化方向向量
//    if (distanceToPlayer > 0.0001f) {
//        directionToPlayer.x /= distanceToPlayer;
//        directionToPlayer.y /= distanceToPlayer;
//        directionToPlayer.z /= distanceToPlayer;
//    }
//
//    // 1. 首先进行快速的AABB检测
//    for (auto shield : categories.gibraltarShields) {
//        // AABB检测比球体检测更快
//        if (ShieldTool::IsLineThroughSphere(cameraOrigin, playerOrigin, shield->Origin)) {
//            return true;
//        }
//    }
//
//    // 2. 然后进行LifeLine护盾检测
//    for (auto shield : categories.lifelineShields) {
//        if (ShieldTool::IsBlockedByLifeLineShield(cameraOrigin, playerOrigin, shield->Origin)) {
//            return true;
//        }
//    }
//
//    // 3. 接着进行NewCastle护盾检测
//    for (auto shield : categories.newcastleShields) {
//        if (ShieldTool::IsBlockedByNewcastleShield(shield, cameraOrigin, playerOrigin)) {
//            return true;
//        }
//    }
//
//    // 4. 最后进行Rampart护盾检测(最复杂)
//    for (auto shield : categories.rampartShields) {
//        if (ShieldTool::IsInRampartShieldProtection(cameraOrigin, viewAngles,
//            playerOrigin, shield->Origin, shield->rotation)) {
//            return true;
//        }
//    }
//
//    return false;
//}
//
//// 更新护盾分类 - 优化版本
//void EntityManager::updateShieldClassification() {
//    CategorizedShields categories;
//    // 预分配容器大小避免运行时扩容
//    categories.gibraltarShields.reserve(20);
//    categories.rampartShields.reserve(20);
//    categories.newcastleShields.reserve(20);
//    categories.lifelineShields.reserve(20);
//
//    auto shields = getShields();
//    for (const auto& shield : *shields) {
//        if (!shield || !shield->valid) continue;
//
//        if (shield->networkFlag == 286269440) {
//            categories.gibraltarShields.push_back(shield);
//        }
//        else if (shield->networkFlag == 286269441) {
//            categories.rampartShields.push_back(shield);
//        }
//        else if (shield->networkFlag == 286302208) {
//            categories.newcastleShields.push_back(shield);
//        }
//        else if (shield->networkFlag == 286261248) {
//            categories.lifelineShields.push_back(shield);
//        }
//    }
//
//    categories.lastUpdateTime = std::chrono::steady_clock::now();
//    updateShieldCategories(categories);
//}