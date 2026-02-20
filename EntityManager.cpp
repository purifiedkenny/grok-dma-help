#include <iostream>
#include <thread>
#include <algorithm>
#include "include.h"

#include "ShieldCheck.h"
#include "EntityManager.h"
#include "nlohmann/json.hpp"
#include <fstream>

// 外部全局变量引用
static uint64_t OFF_Kills_Ptr;
static uint64_t OFF_Damage_Ptr;
static uint64_t OFF_Rank_Ptr;

// 单例访问
EntityManager& EntityManager::getInstance() {
    static EntityManager instance;
    return instance;
}

// 构造函数
EntityManager::EntityManager() {
    // 初始化状态
    m_inGame = false;
    m_battleMode = false;
    m_loggedIn = false;
    m_running = true;
    m_clearingEntities = false;
    m_threadsSuspended = false;
    m_activeThreadCount = 0;
    m_adaptiveSleepTime = std::chrono::milliseconds(1);
    m_lastScanTime = std::chrono::steady_clock::now();
    m_scanCount = 0;
    m_lockContentionCount = 0;

    // 设置扫描参数
    m_scanParams.batchSize = 256;
    m_scanParams.interval = std::chrono::milliseconds(50);
    m_scanParams.playerScanEnd = 100;
    m_scanParams.itemShieldScanStart = 100;
    m_scanParams.itemShieldScanEnd = 10000;
}

// 析构函数
EntityManager::~EntityManager() {
    shutdown();
}

// 初始化
void EntityManager::initialize() {
    // 确保之前的资源已清理
    clearAllEntitiesInternal();

    // 重置状态
    m_inGame = false;
    m_battleMode = false;
    m_running = true;
    m_clearingEntities = false;
    m_threadsSuspended = false;
    m_adaptiveSleepTime = std::chrono::milliseconds(1);
}

// 关闭
void EntityManager::shutdown() {
    // 停止所有线程
    m_running = false;

    // 如果线程暂停中，先恢复它们以便能正常退出
    if (m_threadsSuspended) {
        resumeAllThreads();
    }

    // 清理所有实体
    clearAllEntitiesInternal();
}

void EntityManager::suspendAllThreads() {
    std::unique_lock<std::mutex> lock(m_threadSyncMutex);
    if (m_threadsSuspended) return; // 已经暂停

    // 设置暂停标志
    m_clearingEntities = true;

    // 记录初始活动线程数
    int initialActiveCount = m_activeThreadCount.load();

    lock.unlock();
    m_threadSyncCV.notify_all();  // 通知所有线程

    // 等待所有线程确认暂停
    const int MAX_RETRIES = 25;
    int retries = 0;
    bool allSuspended = false;

    while (!allSuspended && retries < MAX_RETRIES) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 给线程时间响应

        lock.lock();
        int currentActive = m_activeThreadCount.load();
        bool condition = (currentActive == 0);
        allSuspended = condition;
        lock.unlock();

        retries++;
        if (!allSuspended) {
            m_threadSyncCV.notify_all();  // 再次通知
        }
    }

    lock.lock();
    m_threadsSuspended = true;
}
// 恢复所有线程
void EntityManager::resumeAllThreads() {
    std::unique_lock<std::mutex> lock(m_threadSyncMutex);
    if (!m_threadsSuspended) return; // 已经运行中

    m_clearingEntities = false;
    m_threadsSuspended = false;
    lock.unlock(); // 在通知前释放锁

    m_threadSyncCV.notify_all();  // 通知所有线程可以继续运行
}

// 游戏状态控制方法
void EntityManager::setInGame(bool status) {
    m_inGame = status;
}

bool EntityManager::isInGame() const {
    return m_inGame;
}

void EntityManager::setBattleMode(bool status) {
    m_battleMode = status;
}

bool EntityManager::isInBattleMode() const {
    return m_battleMode;
}

void EntityManager::setLoggedIn(bool status) {
    m_loggedIn = status;
}

bool EntityManager::isLoggedIn() const {
    return m_loggedIn;
}

void EntityManager::setGameInput(bool status) {
    m_gameInput = status;
}

bool EntityManager::isGameInput() const {
    return m_gameInput;
}

// 实体数据访问方法
std::shared_ptr<const std::vector<Player*>> EntityManager::getPlayers() const {
    return m_players.read();
}

std::shared_ptr<const std::vector<Item*>> EntityManager::getItems() const {
    return m_items.read();
}

std::shared_ptr<const std::vector<Shield*>> EntityManager::getShields() const {
    return m_shields.read();
}

CategorizedShields EntityManager::getShieldCategories() const {
    auto categories = m_shieldCategories.read();
    if (categories && !categories->empty()) {
        return *(*categories)[0];
    }
    return CategorizedShields();
}

// 索引检查函数
bool EntityManager::checkPlayerExists(int index) const {
    std::shared_lock<std::shared_mutex> lock(m_indicesMutex);
    return m_playerIndices.count(index) > 0;
}

bool EntityManager::checkItemExists(int index) const {
    std::shared_lock<std::shared_mutex> lock(m_indicesMutex);
    return m_itemIndices.count(index) > 0;
}

bool EntityManager::checkShieldExists(int index) const {
    std::shared_lock<std::shared_mutex> lock(m_indicesMutex);
    return m_shieldIndices.count(index) > 0;
}

// 移除无效玩家
void EntityManager::removeInvalidPlayers(const std::vector<Player*>& playersToRemove) {
    if (playersToRemove.empty()) return;

    // 收集需要移除的索引
    std::vector<int> indicesToRemove;
    for (Player* player : playersToRemove) {
        if (player) {
            indicesToRemove.push_back(player->Index);
        }
    }

    // 更新索引集
    {
        std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
        for (int index : indicesToRemove) {
            m_playerIndices.erase(index);
        }
    }

    // 更新玩家列表
    m_players.beginUpdate();
    auto& players = m_players.getWriteBuffer();

    // 移除无效玩家
    auto newEnd = std::remove_if(players.begin(), players.end(),
        [&playersToRemove](Player* player) {
            auto it = std::find(playersToRemove.begin(), playersToRemove.end(), player);
            if (it != playersToRemove.end()) {
                delete player; // 释放内存
                return true;   // 移除此玩家
            }
            return false;      // 保留此玩家
        });

    players.erase(newEnd, players.end());
    m_players.endUpdate();
}

void EntityManager::clearAllEntities() {
    // 确保所有线程已暂停
    bool wasSuspended = m_threadsSuspended;
    if (!wasSuspended) {
        suspendAllThreads();
    }

    try {
        // 清空缓冲区
        m_players.clear();
        m_items.clear();
        m_shields.clear();
        m_shieldCategories.clear();

        // 清空索引集
        {
            std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
            m_playerIndices.clear();
            m_itemIndices.clear();
            m_shieldIndices.clear();
        }

        // 检查是否完全清空，必要时强制清理
        if (!m_players.read()->empty() || !m_items.read()->empty() ||
            !m_shields.read()->empty() || !m_shieldCategories.read()->empty()) {
            

            m_players.forceClear();
            m_items.forceClear();
            m_shields.forceClear();
            m_shieldCategories.forceClear();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error during clearAllEntities: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error during clearAllEntities" << std::endl;
    }

    // 如果我们是暂停线程的，则恢复线程
    if (!wasSuspended) {
        resumeAllThreads();
    }
}

void EntityManager::clearEntitiesOnly() {
    // 清空缓冲区
    m_players.clear();
    m_items.clear();
    m_shields.clear();
    m_shieldCategories.clear();

    // 清空索引集
    {
        std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
        m_playerIndices.clear();
        m_itemIndices.clear();
        m_shieldIndices.clear();
    }
}

// 清理实体内部方法
void EntityManager::clearAllEntitiesInternal() {
    clearEntitiesOnly();
}

// 主散射线程 (保持现有实现)
void EntityManager::mainScatterThread() {
    // 设置线程优先级和亲和性
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 2);

    // 性能监控变量
    std::chrono::steady_clock::time_point lastPerformanceLog = std::chrono::steady_clock::now();
    int frameCount = 0;

    while (m_running) {
        try {
            if (!m_loggedIn) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            // 性能监控 - 每5秒记录一次性能数据
            frameCount++;
            auto currentTime = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastPerformanceLog).count() >= 5) {
                fps = frameCount / 5.0f;

                frameCount = 0;
                lastPerformanceLog = currentTime;
            }

            char NameBufferTemp[64] = { 0 };
            mem.Read(mem.OFF_BASE + OFF_LEVEL, NameBufferTemp, sizeof(NameBufferTemp));
            map->MapName = std::string(NameBufferTemp);

            LocalPlayer temp;
            // 集中处理所有可能需要清理的情况
            if (map->MapName.empty() || !map->CheckMapData() || map->ReturnCR3Fix() ||
                !gatherGameData(&temp) || !temp.ValidPosition()) {

               

                // 暂停所有线程
                suspendAllThreads();

                // 设置游戏状态
                setInGame(false);

                try {
                    // 执行清理操作
                    clearAllEntities();

                    
                }
                catch (const std::exception& e) {
                    std::cerr << "清理过程中发生异常: " << e.what() << std::endl;
                }
                catch (...) {
                    std::cerr << "清理过程中发生未知异常" << std::endl;
                }

                // 无论清理是否成功，都恢复线程
                resumeAllThreads();

                // 特定情况的额外处理
                if (mem.FixCr3()) {
                    VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_ALL, 1);
                }

                // 增加延迟，确保系统稳定
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            scatterWeaponInfo(&temp);

            // 更新本地玩家数据
            *localPlayer = temp;

            // 标记游戏中状态
            setInGame(true);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        catch (const std::exception& e) {
            std::cerr << "Error in MainScatter thread: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void EntityManager::entityScatterThread() {
    try {
        // 设置线程属性
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
        SetThreadAffinityMask(GetCurrentThread(), 1ULL << 0);

        {
            std::lock_guard<std::mutex> lock(m_threadSyncMutex);
            m_activeThreadCount++;
        }

        // 上次扫描的位置记录
        size_t lastPlayerScanPos = 0;
        size_t lastItemShieldScanPos = m_scanParams.itemShieldScanStart;

        // 性能监控变量
        auto lastPlayerScanTime = std::chrono::steady_clock::now();
        auto lastItemShieldScanTime = std::chrono::steady_clock::now();

        while (m_running) {
            try {
                // 检查暂停状态
                {
                    std::unique_lock<std::mutex> lock(m_threadSyncMutex);
                    if (m_clearingEntities) {
                        m_activeThreadCount--;
                        m_threadSyncCV.notify_all();
                        m_threadSyncCV.wait(lock, [this]() {
                            return !m_clearingEntities;
                            });
                        m_activeThreadCount++;
                    }
                }

                if (!m_loggedIn || !m_inGame) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    continue;
                }

                // 限制扫描范围
                if (map->isinTraining) {
                    m_scanParams.playerScanEnd = 15000;
                }
                else {
                    m_scanParams.playerScanEnd = 100;
                }

                auto currentTime = std::chrono::steady_clock::now();

                // 玩家扫描 - 更高频率
                if (currentTime - lastPlayerScanTime >= std::chrono::milliseconds(20)) {
                    size_t startIdx = lastPlayerScanPos;
                    size_t endIdx = std::min(startIdx + m_scanParams.batchSize, m_scanParams.playerScanEnd);

                    try {
                        // 限制批次大小
                        if (endIdx - startIdx > 500) {
                            endIdx = startIdx + 500;
                        }

                        // 预读内存
                        std::vector<QWORD> prefetchAddresses;
                        prefetchAddresses.reserve(endIdx - startIdx);

                        for (size_t i = startIdx; i < endIdx; i++) {
                            prefetchAddresses.push_back(mem.OFF_BASE + OFF_ENTITY_LIST + ((i + 1) << 5));
                        }

                        VMMDLL_MemPrefetchPages(mem.vHandle, mem.current_process.PID,
                            prefetchAddresses.data(), static_cast<DWORD>(prefetchAddresses.size()));

                        // 读取实体指针
                        std::vector<uint64_t> basePointers(endIdx - startIdx);
                        auto handle = mem.CreateScatterHandle();
                        if (handle) {
                            for (size_t i = 0; i < basePointers.size(); i++) {
                                uint64_t address = mem.OFF_BASE + OFF_ENTITY_LIST + ((startIdx + i + 1) << 5);
                                mem.AddScatterReadRequest(handle, address, &basePointers[i], sizeof(uint64_t));
                            }
                            mem.ExecuteReadScatter(handle);
                            mem.CloseScatterHandle(handle);

                            // 处理有效指针
                            processEntityBatch(startIdx, basePointers);
                        }
                        else {
                            mem.CloseScatterHandle(handle);
                        }
                    }
                    catch (const std::exception&) {
                        // 忽略异常，继续下一个循环
                    }

                    // 更新下一次扫描位置
                    lastPlayerScanPos = endIdx;
                    if (lastPlayerScanPos >= m_scanParams.playerScanEnd) {
                        lastPlayerScanPos = 0;
                    }

                    lastPlayerScanTime = currentTime;
                }

                // 物品和护盾扫描 - 较低频率，并且在战斗模式下降低优先级
                if (!m_battleMode && currentTime - lastItemShieldScanTime >= m_scanParams.interval) {
                    size_t startIdx = lastItemShieldScanPos;
                    size_t endIdx = std::min(startIdx + m_scanParams.batchSize, m_scanParams.itemShieldScanEnd);

                    try {
                        // 限制批次大小
                        if (endIdx - startIdx > 500) {
                            endIdx = startIdx + 500;
                        }

                        // 预读内存
                        std::vector<QWORD> prefetchAddresses;
                        prefetchAddresses.reserve(endIdx - startIdx);

                        for (size_t i = startIdx; i < endIdx; i++) {
                            prefetchAddresses.push_back(mem.OFF_BASE + OFF_ENTITY_LIST + ((i + 1) << 5));
                        }

                        VMMDLL_MemPrefetchPages(mem.vHandle, mem.current_process.PID,
                            prefetchAddresses.data(), static_cast<DWORD>(prefetchAddresses.size()));

                        // 读取实体指针
                        std::vector<uint64_t> basePointers(endIdx - startIdx);
                        auto handle = mem.CreateScatterHandle();
                        if (handle) {
                            for (size_t i = 0; i < basePointers.size(); i++) {
                                uint64_t address = mem.OFF_BASE + OFF_ENTITY_LIST + ((startIdx + i + 1) << 5);
                                mem.AddScatterReadRequest(handle, address, &basePointers[i], sizeof(uint64_t));
                            }
                            mem.ExecuteReadScatter(handle);
                            mem.CloseScatterHandle(handle);

                            // 处理有效指针
                            processEntityBatch(startIdx, basePointers);
                        }
                        else {
                            mem.CloseScatterHandle(handle);
                        }
                    }
                    catch (const std::exception&) {
                        // 忽略异常，继续下一个循环
                    }

                    // 更新下一次扫描位置
                    lastItemShieldScanPos = endIdx;
                    if (lastItemShieldScanPos >= m_scanParams.itemShieldScanEnd) {
                        lastItemShieldScanPos = m_scanParams.itemShieldScanStart;
                    }

                    lastItemShieldScanTime = currentTime;
                }

                // 自适应睡眠
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            catch (const std::exception&) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    catch (const std::exception&) {
        // 忽略异常
    }

    {
        std::lock_guard<std::mutex> lock(m_threadSyncMutex);
        m_activeThreadCount--;
    }
    m_threadSyncCV.notify_all();
}
void EntityManager::processEntityBatch(size_t startIdx, const std::vector<uint64_t>& basePointers) {
    if (basePointers.empty()) return;

    // 限制处理大小
    const size_t MAX_PROCESS_SIZE = 256;
    if (basePointers.size() > MAX_PROCESS_SIZE) {
        // 只处理一部分
        std::vector<uint64_t> limitedPointers(basePointers.begin(), basePointers.begin() + MAX_PROCESS_SIZE);
        return processEntityBatch(startIdx, limitedPointers);
    }

    // 检查有效指针并过滤已知索引
    std::vector<std::pair<size_t, uint64_t>> validEntities;

    try {
        validEntities.reserve(basePointers.size());
    }
    catch (const std::exception&) {
        return;
    }

    {
        std::shared_lock<std::shared_mutex> lock(m_indicesMutex);

        for (size_t i = 0; i < basePointers.size(); i++) {
            size_t entityIdx = startIdx + i;
            uint64_t basePtr = basePointers[i];

            if (!mem.IsValidPointer(basePtr)) continue;

            // 检查是否已知
            bool isKnown = m_playerIndices.count(entityIdx) > 0 ||
                m_itemIndices.count(entityIdx) > 0 ||
                m_shieldIndices.count(entityIdx) > 0;

            if (isKnown) continue;

            try {
                validEntities.emplace_back(entityIdx, basePtr);
            }
            catch (const std::exception&) {
                break;  // 停止添加更多元素
            }
        }
    }

    if (validEntities.empty()) return;

    // 限制实体数据数组大小
    if (validEntities.size() > MAX_PROCESS_SIZE) {
        validEntities.resize(MAX_PROCESS_SIZE);
    }

    // 读取实体类型信息
    std::vector<EntityTemp> entityData;

    try {
        entityData.resize(validEntities.size());
    }
    catch (const std::exception&) {
        return;
    }

    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < validEntities.size(); i++) {
        size_t entityIdx = validEntities[i].first;
        uint64_t basePtr = validEntities[i].second;

        entityData[i].Index = entityIdx;
        entityData[i].BasePointer = basePtr;

        mem.AddScatterReadRequest(handle, basePtr + OFF_SignifierName,
            &entityData[i].nameAddress, sizeof(uint64_t));
        mem.AddScatterReadRequest(handle, basePtr + OFF_PROP_SCRIPT_NETWORKFLAG,
            &entityData[i].networkFlag, sizeof(int));
        mem.AddScatterReadRequest(handle, basePtr + OFF_LOCAL_ORIGIN,
            &entityData[i].origin, sizeof(Vec3D));
    }

    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);

    // 读取实体名称
    auto handle2 = mem.CreateScatterHandle();

    for (size_t i = 0; i < validEntities.size(); i++) {
        if (mem.IsValidPointer(entityData[i].nameAddress)) {
            mem.AddScatterReadRequest(handle2, entityData[i].nameAddress,
                entityData[i].NameBuffer, sizeof(entityData[i].NameBuffer));
        }
    }

    mem.ExecuteReadScatter(handle2, 0, false); // 直接忽略该问题
    mem.CloseScatterHandle(handle2);

    // 处理实体
    for (const auto& entity : entityData) {
        if (!mem.IsValidPointer(entity.nameAddress))
            continue;
        std::string name(entity.NameBuffer);

        try {
            // 玩家实体
            if (name == "player" || name == "npc_dummie") {
                Player* player = nullptr;
                try {
                    player = new Player();
                    player->BasePointer = entity.BasePointer;
                    player->Index = entity.Index;
                    player->signifierName = name;
                    processNewPlayer(player);
                }
                catch (const std::exception&) {
                    if (player) delete player;
                }
            }
            // 物品实体
            else if (name == "prop_survival") {
                // 距离检查
                float distanceSquared = entity.origin.distanceSquared2dTo(localPlayer->CameraOrigin);
                float maxDistanceSquared = (esp->items.maxDistance / 0.025f) * (esp->items.maxDistance / 0.025f);

                if (distanceSquared < maxDistanceSquared) {
                    Item* item = nullptr;
                    try {
                        item = new Item();
                        item->BasePointer = entity.BasePointer;
                        item->Index = entity.Index;
                        item->signifierName = name;
                        processNewItem(item);
                    }
                    catch (const std::exception&) {
                        if (item) delete item;
                    }
                }
            }
            // 护盾实体
            else if (name == "prop_script" &&
                (entity.networkFlag == 286269440 || entity.networkFlag == 286269441 ||
                    entity.networkFlag == 286261248 || entity.networkFlag == 286302208)) {

                float distanceSquared = entity.origin.distanceSquared2dTo(localPlayer->CameraOrigin);
                float maxDistanceSquared = (esp->enemies.distance / 0.025f) * (esp->enemies.distance / 0.025f);

                if (distanceSquared < maxDistanceSquared) {
                    Shield* shield = nullptr;
                    try {
                        shield = new Shield();
                        shield->BasePointer = entity.BasePointer;
                        shield->Index = entity.Index;
                        shield->networkFlag = entity.networkFlag;
                        shield->signifierName = name;
                        processNewShield(shield);
                    }
                    catch (const std::exception&) {
                        if (shield) delete shield;
                    }
                }
            }
        }
        catch (const std::exception&) {
            // 忽略异常，继续处理其他实体
        }
    }
}
// 处理新玩家
void EntityManager::processNewPlayer(Player* player) {
    static std::unordered_set<uint64_t> proPlayerUIDs;
    static bool prListLoaded = false;

    if (!prListLoaded) {
        try {
            std::ifstream prListFile("PRList.json");
            if (prListFile.is_open()) {
                // 读取整个文件内容到字符串
                std::string raw((std::istreambuf_iterator<char>(prListFile)),
                    std::istreambuf_iterator<char>());
                prListFile.close();

                // 手动解析 JSON，避免使用自动类型推导
                auto j = nlohmann::json::parse(raw);
                if (j.contains("Pro") && j["Pro"].is_array()) {
                    for (const auto& item : j["Pro"]) {
                        if (item.contains("uid") && item["uid"].is_number_unsigned()) {
                            uint64_t uid = item["uid"].get<uint64_t>();
                            proPlayerUIDs.insert(uid);
                        }
                    }
                }
                prListLoaded = true;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading PRList: " << e.what() << std::endl;
            prListLoaded = true; // 即使失败也标记为已加载，避免重复尝试
        }
    }

    if (!player || !mem.IsValidPointer(player->BasePointer)) {
        delete player;
        return;
    }

    // 检查是否已知
    {
        std::shared_lock<std::shared_mutex> lock(m_indicesMutex);
        if (m_playerIndices.count(player->Index) > 0) {
            delete player;
            return;
        }
    }

    // 读取玩家基本信息
    auto handle = mem.CreateScatterHandle();

    mem.AddScatterReadRequest(handle, player->BasePointer + OFF_NAME,
        player->NameBuffer, sizeof(player->NameBuffer));
    mem.AddScatterReadRequest(handle, player->BasePointer,
        &player->valid, sizeof(player->valid));
    mem.AddScatterReadRequest(handle, player->BasePointer + OFF_TEAM_NUMBER,
        &player->Team, sizeof(int));
    mem.AddScatterReadRequest(handle, player->BasePointer + OFF_XP,
        &player->Level, sizeof(int));
    mem.AddScatterReadRequest(handle, player->BasePointer + OFF_UID,
        &player->UserId, sizeof(uint64_t));
    mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_NAME_LIST + (player->Index) * 24,
        &player->NameList, sizeof(uint64_t));

    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);

    // 读取玩家名称
    if (!player->valid) {
        delete player;
        return;
    }

    handle = mem.CreateScatterHandle();
    if (mem.IsValidPointer(player->NameList) && !player->IsDummy()) {
        mem.AddScatterReadRequest(handle, player->NameList, player->PlayerName, 32);
        mem.ExecuteReadScatter(handle);
        mem.CloseScatterHandle(handle);
    }
        // 设置玩家属性
        const int localTeam = localPlayer->Team;

        if (map->m_mixtape && (localTeam & 1) == (player->Team & 1)) {
            player->Team = localTeam;
        }

        // 读取网络变量
        player->NetVarGlobal = mem.Read<uint64_t>(player->BasePointer + OFF_NETVAR_GLOBAL);
        player->NetVarData = mem.Read<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + ((player->NetVarGlobal & 0xffff) << 5));

        player->Rank = mem.Read<uint32_t>(player->NetVarData + OFF_NETVAR_INT32S + mem.Read<uint64_t>(mem.OFF_BASE + OFF_NETVAR_TABLE_PTR + static_cast<unsigned long long>(13) * 56 + 16) * 2);

        // 设置其他属性
        player->Level = player->GetPlayerLevel();
        player->RankName = player->getRankName();
        player->IsPro = proPlayerUIDs.find(player->UserId) != proPlayerUIDs.end();
        player->NickName = player->IsDummy() ? "Bot" : player->PlayerName;
        player->IsLocal = localPlayer->BasePointer == player->BasePointer;
        player->IsTeamMate = player->Team == localTeam;
        player->IsHostile = !player->IsTeamMate;

        // 添加到索引集和玩家列表
        {
            std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
            m_playerIndices.insert(player->Index);
        }

        // 添加到玩家列表
        m_players.beginUpdate();
        auto& players = m_players.getWriteBuffer();
        const size_t MAX_PLAYERS = 100; // 根据游戏特性调整
        if (players.size() >= MAX_PLAYERS) {
            std::cerr << "警告：玩家数量达到上限，忽略新玩家" << std::endl;
            delete player;
        }
        else {
            players.push_back(player);
        }
        m_players.endUpdate();
}

// 处理新物品
void EntityManager::processNewItem(Item* item) {
    if (!item || !mem.IsValidPointer(item->BasePointer)) {
        delete item;
        return;
    }

    // 检查是否已知
    {
        std::shared_lock<std::shared_mutex> lock(m_indicesMutex);
        if (m_itemIndices.count(item->Index) > 0) {
            delete item;
            return;
        }
    }

    // 读取物品基本信息
    auto handle = mem.CreateScatterHandle();
    if (!handle) {
        delete item;
        mem.CloseScatterHandle(handle);
        return;
    }

    mem.AddScatterReadRequest(handle, item->BasePointer,
        &item->valid, sizeof(uint64_t));
    mem.AddScatterReadRequest(handle, item->BasePointer + OFF_LOCAL_ORIGIN,
        &item->Origin, sizeof(Vec3D));
    mem.AddScatterReadRequest(handle, item->BasePointer + OFF_CUSTOM_SPIRIT,
        &item->LootIndex, sizeof(int));
    mem.AddScatterReadRequest(handle, item->BasePointer + OFF_SignifierName,
        &item->name_address, sizeof(uint64_t));

    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);

    // 读取物品名称
    if (mem.IsValidPointer(item->name_address)) {
        handle = mem.CreateScatterHandle();
        {
            mem.AddScatterReadRequest(handle, item->name_address,
                item->NameBuffer, sizeof(item->NameBuffer));
            mem.ExecuteReadScatter(handle);
            mem.CloseScatterHandle(handle);
        }
    }

    // 验证物品有效性
    item->ValidCheck();
    if (item->IsValid()) {
        item->distance_to_local = item->Origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;

        if (item->distance_to_local < esp->items.maxDistance) {
            // 添加到索引集和物品列表
            {
                std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                m_itemIndices.insert(item->Index);
            }

            m_items.beginUpdate();
            auto& items = m_items.getWriteBuffer();
            const size_t MAX_PLAYERS = 1000;
            if (items.size() >= MAX_PLAYERS) {
                std::cerr << "警告：物品数量达到上限，忽略新物品" << std::endl;
                delete item;
            }
            else {
                items.push_back(item);
            }
            m_items.endUpdate();
        }
        else {
            delete item;
        }
    }
    else {
        delete item;
    }
}

// 处理新护盾
void EntityManager::processNewShield(Shield* shield) {
    if (!shield || !mem.IsValidPointer(shield->BasePointer)) {
        delete shield;
        return;
    }

    // 检查是否已知
    {
        std::shared_lock<std::shared_mutex> lock(m_indicesMutex);
        if (m_shieldIndices.count(shield->Index) > 0) {
            delete shield;
            return;
        }
    }

    // 读取护盾基本信息
    auto handle = mem.CreateScatterHandle();
    if (!handle) {
        delete shield;
        mem.CloseScatterHandle(handle);
        return;
    }

    mem.AddScatterReadRequest(handle, shield->BasePointer,
        &shield->valid, sizeof(uint64_t));
    mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_LOCAL_ORIGIN,
        &shield->Origin, sizeof(Vec3D));
    mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_ENTITY_ROTATION,
        &shield->rotation, sizeof(Vec3D));

    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);

    // 验证护盾有效性
    shield->ValidCheck();
    if (shield->valid) {
        shield->distance_to_local = shield->Origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;

        if (shield->distance_to_local < esp->enemies.distance) {
            // 添加到索引集和护盾列表
            {
                std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                m_shieldIndices.insert(shield->Index);
            }

            m_shields.beginUpdate();
            auto& shields = m_shields.getWriteBuffer();
            const size_t MAX_PLAYERS = 20; // 根据游戏特性调整
            if (shields.size() >= MAX_PLAYERS) {
                std::cerr << "警告：护盾数量达到上限，忽略新护盾" << std::endl;
                delete shield;
            }
            else {
                shields.push_back(shield);
            }
            m_shields.endUpdate();

            // 更新护盾分类
            updateShieldClassification();
        }
        else {
            delete shield;
        }
    }
    else {
        delete shield;
    }
}

// 玩家更新线程
void EntityManager::playerScatterThread() {
    // 设置线程属性
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 1);

    {
        std::lock_guard<std::mutex> lock(m_threadSyncMutex);
        m_activeThreadCount++;
    }

    while (m_running) {
        try {
            {
                std::unique_lock<std::mutex> lock(m_threadSyncMutex);
                if (m_clearingEntities) {

                    m_activeThreadCount--;

                    // 通知主线程此线程已暂停
                    m_threadSyncCV.notify_all();

                    // 等待恢复信号，添加超时避免死锁
                    auto waitResult = m_threadSyncCV.wait_for(lock, std::chrono::seconds(10), [this]() {
                        return !m_clearingEntities;
                        });

                    m_activeThreadCount++;
                }
            }
            if (!m_loggedIn || !m_inGame) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            // 获取当前玩家列表
            auto players = getPlayers();
            if (players->empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            // 开始更新
            m_players.beginUpdate();
            auto& workingPlayers = m_players.getWriteBuffer();

            // 准备数据结构
            std::vector<PlayerBatchData> playerBatchData(workingPlayers.size());

            // 创建批处理句柄
            auto handle = mem.CreateScatterHandle();
            if (!handle) {
                m_players.endUpdate();
                mem.CloseScatterHandle(handle);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            // 批量读取玩家数据
            for (size_t i = 0; i < workingPlayers.size(); i++) {
                Player* player = workingPlayers[i];
                uint64_t base = player->BasePointer;

                if (!mem.IsValidPointer(base)) continue;

                auto& data = playerBatchData[i];

                mem.AddScatterReadRequest(handle, base + OFF_LOCAL_ORIGIN, &data.origin, sizeof(Vec3D));
                mem.AddScatterReadRequest(handle, base + OFF_HEALTH, &data.health, sizeof(int));
                mem.AddScatterReadRequest(handle, base + OFF_SHIELD, &data.shield, sizeof(int));
                mem.AddScatterReadRequest(handle, base + OFF_LIFE_STATE, &data.isDead, sizeof(bool));
                mem.AddScatterReadRequest(handle, base + OFF_BLEEDOUT_STATE, &data.isKnocked, sizeof(bool));
                mem.AddScatterReadRequest(handle, base + OFF_GLOW_ENABLE, &data.glowEnable, sizeof(int));
                mem.AddScatterReadRequest(handle, base + OFF_GLOW_THROUGH_WALL, &data.glowThroughWall, sizeof(int));
                mem.AddScatterReadRequest(handle, base + OFF_GLOW_HIGHLIGHT_ID, &data.highlightID, sizeof(int));
                mem.AddScatterReadRequest(handle, base + OFF_LAST_AIMEDAT_TIME, &data.lastTimeAimedAt, sizeof(float));
                mem.AddScatterReadRequest(handle, base + OFF_LAST_VISIBLE_TIME, &data.lastVisibleTime, sizeof(float));
                mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_TIME_BASE, &data.worldTime, sizeof(float));
                mem.AddScatterReadRequest(handle, base + OFF_YAW, &data.viewYaw, sizeof(float));
                mem.AddScatterReadRequest(handle, base + OFF_VIEW_ANGLES, &data.viewAngles, sizeof(Vec2D));
                mem.AddScatterReadRequest(handle, base + OFF_ABSVELOCITY, &data.absoluteVelocity, sizeof(Vec3D));
                mem.AddScatterReadRequest(handle, base + OFF_MAXSHIELD, &data.maxShield, sizeof(int));
                mem.AddScatterReadRequest(handle, base + OFF_STUDIOHDR, &data.modelPointer, sizeof(uint64_t));
                mem.AddScatterReadRequest(handle, base + OFF_BONES, &data.bonePointer, sizeof(uint64_t));
                mem.AddScatterReadRequest(handle, base + OFF_ACTIVE_WEAPON, &data.weaponHandle, sizeof(uint64_t));
                mem.AddScatterReadRequest(handle, base + OFF_Collision + OFF_Collision_m_vecMins, &data.vec_min, sizeof(Vec3D));
                mem.AddScatterReadRequest(handle, base + OFF_Collision + OFF_Collision_m_vecMaxs, &data.vec_max, sizeof(Vec3D));

                if (mem.IsValidPointer(player->NetVarData)) {
                    mem.AddScatterReadRequest(handle, player->NetVarData + OFF_NETVAR_RANGES + OFF_Kills_Ptr, &data.kills, sizeof(uint8_t));
                    mem.AddScatterReadRequest(handle, player->NetVarData + OFF_NETVAR_INT32S + OFF_Damage_Ptr, &data.damages, sizeof(uint32_t));
                }
            }

            mem.ExecuteReadScatter(handle);
            mem.CloseScatterHandle(handle);

            // 武器数据读取
            if (true) {
                auto weaponHandle = mem.CreateScatterHandle();
                if (weaponHandle) {
                    for (size_t i = 0; i < workingPlayers.size(); i++) {
                        if (!playerBatchData[i].weaponHandle) continue;

                        uint64_t WeaponHandleMasked = playerBatchData[i].weaponHandle & 0xffff;
                        uint64_t weaponEntityAddress = mem.OFF_BASE + OFF_ENTITY_LIST + (WeaponHandleMasked << 5);
                        mem.AddScatterReadRequest(weaponHandle, weaponEntityAddress,
                            &playerBatchData[i].weaponHandleSecond, sizeof(uint64_t));
                    }

                    mem.ExecuteReadScatter(weaponHandle);
                    mem.CloseScatterHandle(weaponHandle);

                    auto weaponIndexHandle = mem.CreateScatterHandle();
                    if (weaponIndexHandle) {
                        for (size_t i = 0; i < workingPlayers.size(); i++) {
                            if (!playerBatchData[i].weaponHandleSecond ||
                                !mem.IsValidPointer(playerBatchData[i].weaponHandleSecond)) continue;

                            mem.AddScatterReadRequest(weaponIndexHandle,
                                playerBatchData[i].weaponHandleSecond + OFF_WEAPON_INDEX,
                                &playerBatchData[i].weaponIndex, sizeof(int));
                        }

                        mem.ExecuteReadScatter(weaponIndexHandle, 0, false);
                        mem.CloseScatterHandle(weaponIndexHandle);
                    }
                    else
                        mem.CloseScatterHandle(weaponIndexHandle);
                }
                else
                {
                    mem.CloseScatterHandle(weaponHandle);
                }
            }

            // 更新玩家数据并移除无效玩家
            std::vector<Player*> invalidPlayers;
            for (size_t i = 0; i < workingPlayers.size(); i++) {
                Player* player = workingPlayers[i];
                const auto& data = playerBatchData[i];

                if (!mem.IsValidPointer(player->BasePointer) || data.health > 100 || data.health <= 0 || data.origin.empty()) {
                    invalidPlayers.push_back(player);
                    continue;
                }

                // 更新玩家属性
                player->Health = data.health;
                player->Shield = data.shield;
                player->IsDead = data.isDead;
                player->IsKnocked = data.isKnocked;
                player->GlowEnable = data.glowEnable;
                player->GlowThroughWall = data.glowThroughWall;
                player->HighlightID = data.highlightID;
                player->LastTimeAimedAt = data.lastTimeAimedAt;
                player->LastVisibleTime = data.lastVisibleTime;
                player->ViewYaw = data.viewYaw;
                player->ViewAngles = data.viewAngles;
                player->AbsoluteVelocity = data.absoluteVelocity;
                player->calculatedVelocity = data.absoluteVelocity;
                player->MaxShield = data.maxShield;
                player->ModelPointer = data.modelPointer;
                player->BonePointer = data.bonePointer;
                player->Origin = data.origin;
                player->VecMin = data.vec_min + data.origin;
                player->VecMax = data.vec_max + data.origin;
                player->DistanceToLocalPlayer = localPlayer->Origin.distance2dTo(player->Origin) * 0.025F;
                player->Distance2DToLocalPlayer = localPlayer->Origin.distance2dTo(player->Origin) * 0.025F;

                player->AimedAtCheck();

                player->IsKnocked = player->IsDummy() ? false : player->IsKnocked;
                player->IsVisible = player->VisCheck() || player->IsAimedAt;
                player->needShieldCheck = player->IsVisible;
                player->weaponIndex = data.weaponIndex;
                player->Kills = data.kills;
                player->Damages = data.damages;
                player->IsBot = player->IsDummy();

                // 假人处理
                if (player->IsDummy()) {
                    float currentTime = data.worldTime;
                    if (!player->hasLastPosition) {
                        player->lastOrigin = data.origin;
                        player->lastUpdateTime = currentTime;
                        player->hasLastPosition = true;
                        player->calculatedVelocity = Vec3D();
                    }
                    else {
                        float deltaTime = currentTime - player->lastUpdateTime;
                        if (deltaTime > 0) {
                            Vec3D deltaPosition = {
                                data.origin.x - player->lastOrigin.x,
                                data.origin.y - player->lastOrigin.y,
                                data.origin.z - player->lastOrigin.z
                            };

                            player->calculatedVelocity = {
                                deltaPosition.x / deltaTime,
                                deltaPosition.y / deltaTime,
                                deltaPosition.z / deltaTime
                            };

                            player->lastOrigin = data.origin;
                            player->lastUpdateTime = currentTime;
                        }
                    }
                }
            }

            // 移除无效玩家
            if (!invalidPlayers.empty()) {
                for (auto player : invalidPlayers) {
                    auto it = std::find(workingPlayers.begin(), workingPlayers.end(), player);
                    if (it != workingPlayers.end()) {
                        {
                            std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                            m_playerIndices.erase(player->Index);
                        }
                        workingPlayers.erase(it);
                        delete player;
                    }
                }
            }

            // 护盾阻挡检查

            if (aimbot->aimParams.shieldCheck)
            {
                bool needsShieldCheck = false;
                for (auto* player : workingPlayers) {
                    if (player && player->needShieldCheck) {
                        needsShieldCheck = true;
                        break;
                    }
                }

                if (needsShieldCheck) {
                    for (auto* player : workingPlayers) {
                        if (!player || !player->needShieldCheck) continue;
                        if (isPlayerBlockedByShields(player)) {
                            player->IsVisible = false;
                        }
                    }
                }
            }

            // 完成更新
            m_players.endUpdate();

            // 自适应休眠
            auto sleepTime = m_battleMode ?
                std::chrono::milliseconds(5) : std::chrono::milliseconds(10);
            std::this_thread::sleep_for(sleepTime);
        }
        catch (const std::exception& e) {
            std::cerr << "Error in PlayerScatter thread: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_threadSyncMutex);
        m_activeThreadCount--;
    }
    m_threadSyncCV.notify_all();
}


void EntityManager::itemShieldScatterThread() {
    // 设置线程属性
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 2);

    {
        std::lock_guard<std::mutex> lock(m_threadSyncMutex);
        m_activeThreadCount++;
    }

    while (m_running) {
        try {
            // 检查暂停状态 - 修改版本
            {
                std::unique_lock<std::mutex> lock(m_threadSyncMutex);
                if (m_clearingEntities) {
                    

                    // 减少活跃线程计数
                    m_activeThreadCount--;

                    // 通知主线程此线程已暂停
                    m_threadSyncCV.notify_all();

                    // 等待恢复信号，带超时机制防止无限等待
                    auto waitResult = m_threadSyncCV.wait_for(lock, std::chrono::seconds(10), [this]() {
                        return !m_clearingEntities;
                        });

                    m_activeThreadCount++;
                   
                }
            }

            if (!m_loggedIn || !m_inGame) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            // 在战斗模式下清理物品
            if (m_battleMode && esp->items.globalEnabled == false) {
                m_items.beginUpdate();
                auto& workingItems = m_items.getWriteBuffer();

                // 清理物品列表
                for (auto* item : workingItems) {
                    delete item;
                }
                workingItems.clear();

                {
                    std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                    m_itemIndices.clear();
                }

                m_items.endUpdate();
            }
            // 更新物品
            else if (!m_battleMode && esp->items.globalEnabled) {
                // 开始更新物品
                m_items.beginUpdate();
                auto& workingItems = m_items.getWriteBuffer();

                if (!workingItems.empty()) {
                    std::vector<EntityBatchData> itemData(workingItems.size());
                    auto handle = mem.CreateScatterHandle();
                    {
                        // 批量读取物品数据
                        for (size_t i = 0; i < workingItems.size(); i++) {
                            Item* item = workingItems[i];

                            if (!mem.IsValidPointer(item->BasePointer)) continue;

                            mem.AddScatterReadRequest(handle, item->BasePointer + OFF_LOCAL_ORIGIN,
                                &itemData[i].origin, sizeof(Vec3D));
                            mem.AddScatterReadRequest(handle, item->BasePointer + OFF_CUSTOM_SPIRIT,
                                &itemData[i].lootIndex, sizeof(int));
                            mem.AddScatterReadRequest(handle, item->BasePointer,
                                &itemData[i].valid, sizeof(uint64_t));
                        }

                        mem.ExecuteReadScatter(handle);
                        mem.CloseScatterHandle(handle);

                        // 更新物品并移除无效物品
                        std::vector<Item*> validItems;
                        validItems.reserve(workingItems.size());

                        for (size_t i = 0; i < workingItems.size(); i++) {
                            Item* item = workingItems[i];
                            auto& data = itemData[i];

                            // 验证物品有效性
                            if (!mem.IsValidPointer(data.valid)) {
                                {
                                    std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                                    m_itemIndices.erase(item->Index);
                                }
                                delete item;
                                continue;
                            }

                            // 更新物品属性
                            item->Origin = data.origin;
                            item->LootIndex = data.lootIndex;
                            item->valid = data.valid;

                            // 更新距离并检查是否超出范围
                            item->distance_to_local = item->Origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;
                            if (item->distance_to_local >= esp->items.maxDistance) {
                                {
                                    std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                                    m_itemIndices.erase(item->Index);
                                }
                                delete item;
                                continue;
                            }

                            validItems.push_back(item);
                        }

                        // 更新物品列表
                        workingItems = validItems;
                    }
                }

                m_items.endUpdate();
            }

            // 更新护盾 - 即使在战斗模式下也更新护盾
            m_shields.beginUpdate();
            auto& workingShields = m_shields.getWriteBuffer();

            if (!workingShields.empty()) {
                std::vector<EntityBatchData> shieldData(workingShields.size());
                auto handle = mem.CreateScatterHandle();
                {
                    // 批量读取护盾数据
                    for (size_t i = 0; i < workingShields.size(); i++) {
                        Shield* shield = workingShields[i];

                        if (!mem.IsValidPointer(shield->BasePointer)) continue;

                        mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_LOCAL_ORIGIN,
                            &shieldData[i].origin, sizeof(Vec3D));
                        mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_ENTITY_ROTATION,
                            &shieldData[i].rotation, sizeof(Vec3D));
                        mem.AddScatterReadRequest(handle, shield->BasePointer,
                            &shieldData[i].valid, sizeof(uint64_t));
                        mem.AddScatterReadRequest(handle, shield->BasePointer + OFF_PROP_SCRIPT_NETWORKFLAG,
                            &shieldData[i].networkFlag, sizeof(int));
                    }

                    mem.ExecuteReadScatter(handle);
                    mem.CloseScatterHandle(handle);

                    // 更新护盾并移除无效护盾
                    std::vector<Shield*> validShields;
                    validShields.reserve(workingShields.size());
                    bool shieldDataChanged = false;

                    for (size_t i = 0; i < workingShields.size(); i++) {
                        Shield* shield = workingShields[i];
                        auto& data = shieldData[i];

                        // 验证护盾有效性
                        if (!mem.IsValidPointer(data.valid)) {
                            {
                                std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                                m_shieldIndices.erase(shield->Index);
                            }
                            delete shield;
                            shieldDataChanged = true;
                            continue;
                        }

                        float distance = data.origin.distance2dTo(localPlayer->CameraOrigin) * 0.025f;
                        if (distance >= esp->enemies.distance) {
                            {
                                std::unique_lock<std::shared_mutex> lock(m_indicesMutex);
                                m_shieldIndices.erase(shield->Index);
                            }
                            delete shield;
                            shieldDataChanged = true;
                            continue;
                        }

                        // 检查是否有数据变化
                        if (shield->Origin.x != data.origin.x || shield->Origin.y != data.origin.y ||
                            shield->Origin.z != data.origin.z || shield->rotation.y != data.rotation.y ||
                            shield->networkFlag != data.networkFlag) {
                            shieldDataChanged = true;
                        }

                        // 更新护盾属性
                        shield->Origin = data.origin;
                        shield->rotation = data.rotation;
                        shield->networkFlag = data.networkFlag;
                        shield->distance_to_local = distance;
                        shield->valid = data.valid;

                        validShields.push_back(shield);
                    }

                    // 更新护盾列表
                    workingShields = validShields;

                    // 如果护盾数据有变化，更新护盾分类
                    if (shieldDataChanged) {
                        updateShieldClassification();
                    }
                }
            }

            m_shields.endUpdate();

            // 自适应休眠
            auto sleepTime = m_battleMode ?
                std::chrono::milliseconds(30) : std::chrono::milliseconds(20);
            std::this_thread::sleep_for(sleepTime);
        }
        catch (const std::exception& e) {
            std::cerr << "Error in ItemShieldScatter thread: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_threadSyncMutex);
        m_activeThreadCount--;
    }
    m_threadSyncCV.notify_all();
}

// 更新护盾分类
void EntityManager::updateShieldClassification() {
    CategorizedShields categories;

    // 预分配容器大小避免运行时扩容
    categories.gibraltarShields.reserve(20);
    categories.rampartShields.reserve(20);
    categories.newcastleShields.reserve(20);
    categories.lifelineShields.reserve(20);

    auto shields = getShields();
    for (const auto& shield : *shields) {
        if (!shield || !shield->valid) continue;

        if (shield->networkFlag == 286269440) {
            categories.gibraltarShields.push_back(shield);
        }
        else if (shield->networkFlag == 286269441) {
            categories.rampartShields.push_back(shield);
        }
        else if (shield->networkFlag == 286302208) {
            categories.newcastleShields.push_back(shield);
        }
        else if (shield->networkFlag == 286261248) {
            categories.lifelineShields.push_back(shield);
        }
    }

    categories.lastUpdateTime = std::chrono::steady_clock::now();

    // 更新护盾分类
    m_shieldCategories.beginUpdate();
    auto& categoriesBuffer = m_shieldCategories.getWriteBuffer();

    // 清理旧分类
    for (auto* cat : categoriesBuffer) {
        delete cat;
    }
    categoriesBuffer.clear();

    // 添加新分类
    categoriesBuffer.push_back(new CategorizedShields(categories));

    m_shieldCategories.endUpdate();
}

// 检测玩家是否被护盾阻挡
bool EntityManager::isPlayerBlockedByShields(Player* player) {
    if (!player->IsVisible) {
        return true;
    }

    if (!player->valid || player->IsLocal ||
        player->IsDead || player->Health <= 0 || player->IsTeamMate) {
        return false;
    }

    if (player->DistanceToLocalPlayer > esp->enemies.distance) {
        return false;
    }

    auto shieldCats = m_shieldCategories.read();
    if (shieldCats->empty()) {
        return false;
    }

    CategorizedShields categories = *(*shieldCats)[0];

    bool hasShields = !categories.newcastleShields.empty() ||
        !categories.gibraltarShields.empty() ||
        !categories.rampartShields.empty() ||
        !categories.lifelineShields.empty();

    if (!hasShields) {
        return false;
    }

    //// 护盾碰撞检测...
    const Vec3D& playerOrigin = player->Origin;
    const Vec3D& cameraOrigin = localPlayer->CameraOrigin;
    const Vec2D& viewAngles = localPlayer->ViewAngles;

    //// 计算玩家到本地玩家的方向向量
    //Vec3D directionToPlayer = {
    //    playerOrigin.x - cameraOrigin.x,
    //    playerOrigin.y - cameraOrigin.y,
    //    playerOrigin.z - cameraOrigin.z
    //};
    //float distanceToPlayer = std::sqrt(
    //    directionToPlayer.x * directionToPlayer.x +
    //    directionToPlayer.y * directionToPlayer.y +
    //    directionToPlayer.z * directionToPlayer.z
    //);

    //// 归一化方向向量
    //if (distanceToPlayer > 0.0001f) {
    //    directionToPlayer.x /= distanceToPlayer;
    //    directionToPlayer.y /= distanceToPlayer;
    //    directionToPlayer.z /= distanceToPlayer;
    //}

    // 先检查直布罗陀护盾(最快速检测)
    for (auto shield : categories.gibraltarShields) {
        if (ShieldTool::IsLineThroughSphere(cameraOrigin, playerOrigin, shield->Origin)) {
            return true;
        }
    }

    // 再检查生命线护盾
    for (auto shield : categories.lifelineShields) {
        if (ShieldTool::IsBlockedByLifeLineShield(cameraOrigin, playerOrigin, shield->Origin)) {
            return true;
        }
    }

    //for (auto shield : categories.newcastleShields) {
    //    if (ShieldTool::IsBlockedByNewcastleShield(shield, cameraOrigin, playerOrigin)) {
    //        return true;
    //    }
    //}

    //// 最后检查兰伯特护盾(最复杂)
    //for (auto shield : categories.rampartShields) {
    //    if (ShieldTool::IsInRampartShieldProtection(cameraOrigin, viewAngles,
    //        playerOrigin, shield->Origin, shield->rotation)) {
    //        return true;
    //    }
    //}

    return false;
}

bool EntityManager::gatherGameData(LocalPlayer* localPlayer) {
    auto handle = mem.CreateScatterHandle();
    if (!handle) { mem.CloseScatterHandle(handle); return false; }

    uint64_t gameModeAddress = mem.OFF_BASE + OFF_GAMEMODE;
    mem.AddScatterReadRequest(handle, gameModeAddress, &map->gameModePtr, sizeof(uint64_t));

    uint64_t localPlayerAddress = mem.OFF_BASE + OFF_LOCAL_PLAYER;
    mem.AddScatterReadRequest(handle, localPlayerAddress, &localPlayer->BasePointer, sizeof(uint64_t));

    uint64_t inAttackAddress = mem.OFF_BASE + OFF_INATTACK;
    mem.AddScatterReadRequest(handle, inAttackAddress, &localPlayer->InAttack, sizeof(uint32_t));

    // 靈敏度 默認 5.0
    mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_MOUSE_SENSITIVITY + 0x60, &localPlayer->mouseSensitive, sizeof(float));

    // 如果偏移还未初始化，获取偏移
    if (OFF_Kills_Ptr == 0) {
        uint64_t KillNetVarTableAddr = mem.OFF_BASE + OFF_NETVAR_TABLE_PTR + static_cast<unsigned long long>(153) * 56 + 16;
        mem.AddScatterReadRequest(handle, KillNetVarTableAddr, &OFF_Kills_Ptr, sizeof(OFF_Kills_Ptr));
    }

    if (OFF_Damage_Ptr == 0) {
        uint64_t DamageNetVarTableAddr = mem.OFF_BASE + OFF_NETVAR_TABLE_PTR + static_cast<unsigned long long>(9) * 56 + 16;
        mem.AddScatterReadRequest(handle, DamageNetVarTableAddr, &OFF_Damage_Ptr, sizeof(OFF_Damage_Ptr));
    }

    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);

    // 如果是首次读取，处理偏移
    if (OFF_Kills_Ptr != 0 && OFF_Damage_Ptr != 0) {
        OFF_Kills_Ptr *= 2;
        OFF_Damage_Ptr *= 2;
    }

    return scatterLocalPlayer(localPlayer);
}

// scatterLocalPlayer 方法 - 优化版本
bool EntityManager::scatterLocalPlayer(LocalPlayer* localPlayer) {
    if (!mem.IsValidPointer(localPlayer->BasePointer)) return false;

    auto handle = mem.CreateScatterHandle();
    if (!handle) { mem.CloseScatterHandle(handle); return false; }

    // 散射读取本地玩家基本数据
    uint64_t isDeadAddress = localPlayer->BasePointer + OFF_LIFE_STATE;
    mem.AddScatterReadRequest(handle, isDeadAddress, &localPlayer->IsDead, sizeof(bool));

    uint64_t isKnockedAddress = localPlayer->BasePointer + OFF_BLEEDOUT_STATE;
    mem.AddScatterReadRequest(handle, isKnockedAddress, &localPlayer->IsKnocked, sizeof(bool));

    uint64_t isZoomingAddress = localPlayer->BasePointer + OFF_ZOOMING;
    mem.AddScatterReadRequest(handle, isZoomingAddress, &localPlayer->IsZooming, sizeof(bool));

    uint64_t teamAddress = localPlayer->BasePointer + OFF_TEAM_NUMBER;
    mem.AddScatterReadRequest(handle, teamAddress, &localPlayer->Team, sizeof(int));

    uint64_t localOriginAddress = localPlayer->BasePointer + OFF_LOCAL_ORIGIN;
    mem.AddScatterReadRequest(handle, localOriginAddress, &localPlayer->Origin, sizeof(Vec3D));

    uint64_t cameraPositionAddress = localPlayer->BasePointer + OFF_CAMERAORIGIN;
    mem.AddScatterReadRequest(handle, cameraPositionAddress, &localPlayer->CameraOrigin, sizeof(Vec3D));

    uint64_t viewAnglesAddress = localPlayer->BasePointer + OFF_VIEW_ANGLES;
    mem.AddScatterReadRequest(handle, viewAnglesAddress, &localPlayer->ViewAngles, sizeof(Vec2D));

    uint64_t breathAddress = localPlayer->BasePointer + OFF_BREATH_ANGLES;
    mem.AddScatterReadRequest(handle, breathAddress, &localPlayer->SwayAngles, sizeof(Vec2D));

    uint64_t weaponHandleAddress = localPlayer->BasePointer + OFF_ACTIVE_WEAPON;
    mem.AddScatterReadRequest(handle, weaponHandleAddress, &localPlayer->WeaponHandle, sizeof(uint64_t));

    uint64_t FlagsAddress = localPlayer->BasePointer + OFF_FLAGS;
    mem.AddScatterReadRequest(handle, FlagsAddress, &localPlayer->Flags, sizeof(int));

    uint64_t GrappleActived = localPlayer->BasePointer + OFF_GRAPPLEACTIVED;
    mem.AddScatterReadRequest(handle, GrappleActived, &localPlayer->isGrppleActived, sizeof(bool));

    uint64_t isGRAPPLEAddress = localPlayer->BasePointer + OFF_GRAPPLE + OFF_GRAPPLEATTACHED;
    mem.AddScatterReadRequest(handle, isGRAPPLEAddress, &localPlayer->isGRAPPLE, sizeof(int));

    uint64_t PuchAngles = localPlayer->BasePointer + OFF_PUNCH_ANGLES;
    mem.AddScatterReadRequest(handle, PuchAngles, &localPlayer->AimPunch, sizeof(Vec3D));

    mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_TIME_BASE, &localPlayer->TimeBase, sizeof(float));
    mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_IN_BACKWARD, &localPlayer->backWardState, sizeof(int));
    mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_IN_FORWARD, &localPlayer->forewardState, sizeof(int));
    mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_IN_FORWARD + 0x8, &localPlayer->forceForeward, sizeof(int));
    
    mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_Wall_RunStart_TIME, &localPlayer->wallrunStart, sizeof(float));
    mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_Wall_RunClear_TIME, &localPlayer->wallrunClear, sizeof(float));
    mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_SET_SKYDRIVESTATE, &localPlayer->skyDriveState, sizeof(int));
    mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_SET_IN_DUCKSTATE, &localPlayer->duckState, sizeof(int));
    
    mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_TRAVERSAL_START_TIME, &localPlayer->TravelStart, sizeof(float));
    mem.AddScatterReadRequest(handle, localPlayer->BasePointer + OFF_TRAVERSAL_PROGRESS, &localPlayer->TravelProgress, sizeof(float));
    
    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);

    return true;
}

// 收集武器信息 - 优化版本
void EntityManager::scatterWeaponInfo(LocalPlayer* localPlayer) {
    if (localPlayer->IsDead || localPlayer->IsKnocked || !localPlayer->WeaponHandle)
        return;

    uint64_t WeaponHandleMasked = localPlayer->WeaponHandle & 0xffff;
    localPlayer->WeaponEntity = mem.Read<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + (WeaponHandleMasked << 5));

    auto handle = mem.CreateScatterHandle();
    if (!handle) { mem.CloseScatterHandle(handle); return; }

    // 散射读取武器数据
    uint64_t zoomFOVAddress = localPlayer->WeaponEntity + OFF_CURRENTZOOMFOV;
    mem.AddScatterReadRequest(handle, zoomFOVAddress, &localPlayer->ZoomFOV, sizeof(float));

    uint64_t targetZoomFOVAddress = localPlayer->WeaponEntity + OFF_TARGETZOOMFOV;
    mem.AddScatterReadRequest(handle, targetZoomFOVAddress, &localPlayer->TargetZoomFOV, sizeof(float));

    uint64_t weaponIndexAddress = localPlayer->WeaponEntity + OFF_WEAPON_INDEX;
    mem.AddScatterReadRequest(handle, weaponIndexAddress, &localPlayer->WeaponIndex, sizeof(int));

    uint64_t mod_bitfieldAddress = localPlayer->WeaponEntity + OFF_WEAPON_BITFIELD;
    mem.AddScatterReadRequest(handle, mod_bitfieldAddress, &localPlayer->weapon_mod_bitfield, sizeof(int));

    uint64_t weaponProjectileSpeedAddress = localPlayer->WeaponEntity + OFF_PROJECTILESPEED;
    mem.AddScatterReadRequest(handle, weaponProjectileSpeedAddress, &localPlayer->WeaponProjectileSpeed, sizeof(float));

    uint64_t weaponProjectileScaleAddress = localPlayer->WeaponEntity + OFF_PROJECTILESCALE;
    mem.AddScatterReadRequest(handle, weaponProjectileScaleAddress, &localPlayer->WeaponProjectileScale, sizeof(float));

    uint64_t playerAbsoluteVelocityAddress = localPlayer->BasePointer + OFF_ABSVELOCITY;
    mem.AddScatterReadRequest(handle, playerAbsoluteVelocityAddress, &localPlayer->AbsoluteVelocity, sizeof(Vec3D));

    uint64_t weaponAmmoAddress = localPlayer->WeaponEntity + OFF_WEAPON_AMMO;
    mem.AddScatterReadRequest(handle, weaponAmmoAddress, &localPlayer->Ammo, sizeof(int));

    uint64_t isReloadingAddress = localPlayer->WeaponEntity + OFF_RELOADING;
    mem.AddScatterReadRequest(handle, isReloadingAddress, &localPlayer->IsReloading, sizeof(bool));


    mem.ExecuteReadScatter(handle);
    mem.CloseScatterHandle(handle);
 
    localPlayer->IsHoldingGrenade = localPlayer->WeaponIndex == WeaponType::Throwables::ARC_STAR || localPlayer->WeaponIndex == WeaponType::Throwables::FRAG_GRENADE || localPlayer->WeaponIndex == WeaponType::Throwables::THERMITE;
    localPlayer->IsEmptyHand = localPlayer->WeaponIndex == WeaponType::MeleeSecondary::EMPTY_HANDED || localPlayer->WeaponIndex == WeaponType::MeleeSecondary::SURVIVAL_MELEE;
}
