#include "ItemProcessor.h"
#include "include.h"
#include "ItemList.h"

ItemScreenProcessor::ItemScreenProcessor()
    : m_lastCameraUpdate(std::chrono::steady_clock::now()) {
}

ItemScreenProcessor::~ItemScreenProcessor() {
    shutdown();
}

void ItemScreenProcessor::ProcessingThreadFunction() {
    // 物品信息缓存使用线程本地存储
    static thread_local std::unordered_map<int, std::pair<size_t, std::string>> itemInfoCache;

    // 设置线程优先级为高优先级
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    
    while (m_running.load(std::memory_order_relaxed)) {
        try {
            if (!entityManager.isLoggedIn()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            // 检查游戏状态
            if (!entityManager.isInGame()) {
                // 获取非活动缓冲区进行修改
                ProcessedItemsData& newData = getInactiveBuffer();

                // 清空物品列表
                newData.clear();
                newData.lastUpdate = std::chrono::steady_clock::now();
                newData.version = m_versionCounter.fetch_add(1, std::memory_order_relaxed);

                // 原子交换缓冲区
                m_activeIndex.store(1 - m_activeIndex.load(std::memory_order_relaxed),
                    std::memory_order_release);

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            gameViewItems->RenderPointer = mem.ReadCache<uint64_t>(mem.OFF_BASE + OFF_VIEWRENDER);
            uint64_t MatrixPtr = mem.Read<uint64_t>(gameViewItems->RenderPointer + OFF_VIEWMATRIX);
            mem.Read(MatrixPtr, &gameViewItems->GameViewMatrix, sizeof(ViewMatrix));

            // 获取当前摄像机位置和视角 - 不检查是否移动
            Vec3D currentCameraPosition = localPlayer->CameraOrigin;
            Vec2D currentViewAngles = localPlayer->ViewAngles;

            // 直接更新摄像机记录
            m_lastCameraPosition = currentCameraPosition;
            m_lastViewAngles = currentViewAngles;
            m_lastCameraUpdate = std::chrono::steady_clock::now();
            m_screenPosNeedsUpdate.store(false, std::memory_order_relaxed);

            // 获取非活动缓冲区用于写入
            ProcessedItemsData& newProcessed = getInactiveBuffer();

            // 初始化新数据
            newProcessed.clear();
            newProcessed.lastUpdate = std::chrono::steady_clock::now();
            newProcessed.cameraPosition = currentCameraPosition;
            newProcessed.viewAngles = currentViewAngles;
            newProcessed.screenSize = Vec2D(gameViewItems->ScreenSize.x, gameViewItems->ScreenSize.y);

            // 无锁获取物品列表
            const auto& items = entityManager.getItems();
            size_t batchSize = 0;

            // 批处理物品 - 收集世界坐标
            for (const auto* item : *items) {
                if (!item) continue;

                // 距离过滤
                if (item->distance_to_local > esp->items.maxDistance) {
                    continue;
                }

                // 添加到批处理队列
                m_worldPositionsBuffer[batchSize++] = item->Origin;

                // 当批处理队列满时中断
                if (batchSize == ProcessedItemsData::MAX_ITEMS) {
                    break;
                }
            }

            // 批量处理世界到屏幕坐标
            if (batchSize > 0) {
                gameViewItems->W2S_Batch(m_worldPositionsBuffer, m_screenPositionsBuffer,
                    m_resultsBuffer, batchSize);

                // 重新遍历物品，使用预计算的屏幕坐标
                size_t processedCount = 0;
                size_t itemIdx = 0;

                for (const auto* item : *items) {
                    if (!item || itemIdx >= batchSize) continue;

                    // 距离过滤
                    if (item->distance_to_local > esp->items.maxDistance) {
                        itemIdx++;
                        continue;
                    }

                    // 如果在屏幕上
                    if (m_resultsBuffer[itemIdx]) {
                        // 从缓存获取物品信息，或计算并缓存
                        auto infoIter = itemInfoCache.find(item->LootIndex);
                        std::pair<size_t, std::string> itemInfo;

                        if (infoIter != itemInfoCache.end()) {
                            itemInfo = infoIter->second;
                        }
                        else {
                            itemInfo = GetItemInfo(item->LootIndex);
                            itemInfoCache[item->LootIndex] = itemInfo;
                        }

                        // 构造处理数据
                        ProcessedItemData processedItem;
                        processedItem.itemId = item->LootIndex;
                        processedItem.lootIndex = item->LootIndex;
                        processedItem.categoryIndex = itemInfo.first;
                        processedItem.name = itemInfo.second;
                        processedItem.screenPos = m_screenPositionsBuffer[itemIdx];
                        processedItem.distance = item->distance_to_local;
                        processedItem.isOnScreen = true;

                        // 计算到屏幕中心的距离
                        processedItem.distanceToCenter = processedItem.screenPos.distanceTo(Vec2D(
                            gameViewItems->ScreenSize.x / 2,
                            gameViewItems->ScreenSize.y / 2
                        ));

                        // 查找图标
                        size_t categoryIndex = itemInfo.first;
                        if (categoryIndex < Lists.size()) {
                            auto categoryIter = Lists.begin();
                            std::advance(categoryIter, categoryIndex);

                            for (const auto& listItem : *categoryIter) {
                                if (std::get<0>(listItem) == item->LootIndex) {
                                    processedItem.icon = std::get<2>(listItem);
                                    break;
                                }
                            }
                        }

                        // 添加到结果
                        newProcessed.addItem(processedItem);
                        processedCount++;
                    }

                    itemIdx++;
                }

                // 距离排序 - 远到近
                if (processedCount > 1) {
                    std::sort(newProcessed.items, newProcessed.items + processedCount,
                        [](const ProcessedItemData& a, const ProcessedItemData& b) {
                            return a.distance > b.distance;
                        });
                }

                // 更新物品计数
                newProcessed.itemCount = processedCount;
            }

            // 更新版本号
            newProcessed.version = m_versionCounter.fetch_add(1, std::memory_order_relaxed);

            // 原子交换缓冲区 - 一次操作完成所有更新
            m_activeIndex.store(1 - m_activeIndex.load(std::memory_order_relaxed),
                std::memory_order_release);

            // 固定更新频率 - 降低CPU使用率
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
        catch (const std::exception& e) {
            std::cerr << "Error in ItemScreenProcessor: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

std::pair<size_t, std::string> ItemScreenProcessor::GetItemInfo(int lootIndex) {
    size_t categoryIndex = 0;
    std::string itemName = "Unknown";

    // 遍历物品列表查找分类和名称
    for (auto categoryIter = Lists.begin(); categoryIter != Lists.end(); ++categoryIter, ++categoryIndex) {
        const auto& category = *categoryIter;

        for (const auto& item : category) {
            if (std::get<0>(item) == lootIndex) {
                return { categoryIndex, std::get<1>(item) };
            }
        }
    }

    return { 0, itemName };
}