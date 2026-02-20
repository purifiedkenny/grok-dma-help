#pragma once
#include <string>
#include <unordered_map>
#include "DMALibrary/Memory/Memory.h"

// 地图类，用于管理当前游戏地图和游戏模式
class Map
{
public:
    // 地图信息
    char gameMode[64] = { 0 };        // 游戏模式名称
    bool isinMatch;                   // 是否在对战中
    bool isinTraining;                // 是否在训练模式
    bool m_mixtape;                   // 是否是混合模式
    std::string MapName;              // 当前地图名称
    std::string LastMapName;          // 上一张地图名称
    uint64_t gameModePtr;             // 游戏模式指针

    // 混合模式映射表
    std::unordered_map<std::string, bool> mixtape = {
        {"control", true},
        {"freedm", true},
        {"arenas", true}
    };

    // 构造函数
    Map() : isinMatch(false), isinTraining(false), m_mixtape(false), gameModePtr(0) {
        MapName = "";
        LastMapName = "";
    }

    // 析构函数
    ~Map() {}

    // 检查当前地图数据
    bool CheckMapData() {
        // 检查是否在比赛中（不是在大厅）
        isinMatch = MapName != "mp_lobby";

        // 检查是否在训练场
        isinTraining = (MapName == "mp_rr_canyonlands_staging_mu1");

        // 读取游戏模式
        if (gameModePtr) {
            mem.Read(gameModePtr, &gameMode, sizeof(gameMode));
            m_mixtape = mixtape[gameMode];
        }

        return isinMatch;
    }

    // 检查地图是否变更（用于CR3修复）
    bool ReturnCR3Fix() {
        if (MapName != LastMapName) {
            LastMapName = MapName;
            return true;
        }
        return false;
    }

    // 更新地图信息
    void UpdateMapInfo(const std::string& newMapName, uint64_t newGameModePtr = 0) {
        // 保存旧地图名称
        if (!MapName.empty() && MapName != newMapName) {
            LastMapName = MapName;
        }

        // 更新当前地图名称
        MapName = newMapName;

        // 更新游戏模式指针（如果提供）
        if (newGameModePtr != 0) {
            gameModePtr = newGameModePtr;
        }

        // 检查并更新地图状态
        CheckMapData();
    }

    // 获取地图名称
    const std::string& GetMapName() const {
        return MapName;
    }

    // 获取游戏模式
    const char* GetGameMode() const {
        return gameMode;
    }

    // 检查是否是指定游戏模式
    bool IsGameMode(const std::string& mode) const {
        return std::string(gameMode) == mode;
    }
};