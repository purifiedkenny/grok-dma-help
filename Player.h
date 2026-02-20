#pragma once
#include "Entity.h"
#include <string>
#include <vector>

// Player 类表示在游戏世界中被解析出的一个实体角色
class Player : public Entity
{
public:
    // 内存指针字段
    uint64_t ModelPointer;       // 模型指针
    uint64_t BonePointer;        // 骨骼指针
    uint64_t NameList;           // 名称列表指针
    uint64_t weaponHandle;       // 当前武器句柄
    uint64_t NetVarGlobal;       // NetVar 全局指针
    uint64_t NetVarData;         // NetVar 数据指针
    uint64_t UserId;             // 用户 ID

    // 状态标记
    bool IsDead;                 // 是否已经死亡
    bool IsKnocked;              // 是否被击倒
    bool IsAimedAt;              // 是否被瞄准
    bool IsVisible;              // 是否可见
    bool needShieldCheck;        // 是否需要护盾检测
    bool shieldCheckResult;      // 护盾检测结果
    bool IsLocal;                // 是否为本地玩家
    bool IsTeamMate;             // 是否是队友
    bool IsHostile;              // 是否为敌对目标
    bool IsPro;                  // 是否为高玩/主播

    // 基础属性
    int Level;                   // 等级
    int Team;                    // 队伍编号
    int Health;                  // 生命值
    int Shield;                  // 护盾值
    int MaxShield;               // 最大护盾值

    // 高亮参数
    int GlowEnable;              // 高亮开关
    int GlowThroughWall;         // 透墙开关
    int HighlightID;             // 高亮模板 ID
    int weaponIndex;             // 武器索引

    // 战斗统计
    uint8_t Kills;               // 击杀数
    uint32_t Damages;            // 造成的伤害
    uint32_t Rank;               // 段位积分

    // 时间与可见性
    float OldVisTime;            // 上一次可见时间
    float LastTimeAimedAt;       // 最近一次被瞄准时间
    float LastVisibleTime;       // 最近一次可见时间
    float LastTimeAimedAtPrevious; // 记录更早一次被瞄准时间
    float WorldTime;             // 当前世界时间
    float lastWorldTime;
    float LastBoneUpdateTime;    // 最近一次骨骼更新时间
    float ViewYaw;               // 视角偏航
    float DistanceToLocalPlayer; // 与本地玩家距离
    float Distance2DToLocalPlayer; // 与本地玩家的水平距离

    // 空间/运动信息
    Vec2D ViewAngles;            // 视角角度
    Vec3D AbsoluteVelocity;      // 实际速度
    Vec3D VecMin;                // AABB 最小点
    Vec3D VecMax;                // AABB 最大点
    Vec4D Box;                   // 2D 包围盒

    // 轨迹缓存
    Vec3D lastOrigin;            // 上一帧位置
    Vec3D calculatedVelocity;    // 估算速度
    float lastUpdateTime;        // 上次刷新时间
    bool hasLastPosition;        // 是否拥有上一帧位置
    Vec3D BestBonePosition;      // 最佳瞄准骨骼坐标

    // 名称与展示
    char NameBuffer[128] = { 0 };  // 名称缓存
    char* PlayerName;            // 玩家名字
    std::string NickName;        // 玩家昵称
    std::string RankName;        // 段位名称

    // 构造与析构
    Player() : Entity(), ModelPointer(0), BonePointer(0), NameList(0),
        IsDead(false), IsKnocked(false), IsAimedAt(false), IsVisible(false),
        IsLocal(false), IsTeamMate(false), IsHostile(false),
        Level(0), Team(0), Health(0), Shield(0), MaxShield(0),
        lastUpdateTime(0), hasLastPosition(false), LastBoneUpdateTime(0) {
        PlayerName = new char[128];
        memset(PlayerName, 0, 128);
    }

    Player(int PlayerIndex) : Entity(PlayerIndex), ModelPointer(0), BonePointer(0), NameList(0),
        IsDead(false), IsKnocked(false), IsAimedAt(false), IsVisible(false),
        IsLocal(false), IsTeamMate(false), IsHostile(false),
        Level(0), Team(0), Health(0), Shield(0), MaxShield(0),
        lastUpdateTime(0), hasLastPosition(false), LastBoneUpdateTime(0) {
        PlayerName = new char[128];
        memset(PlayerName, 0, 128);
    }

    virtual ~Player() {
        delete[] PlayerName;
    }

    // 判断是否为训练假人
    bool IsDummy() {
        return  signifierName == "npc_dummie";
    }

    bool IsBot;

    // 进行有效性校验
    bool IsDataValid() {
        if (BasePointer == 0 || ModelPointer == 0 || BonePointer == 0) {
            return false;
        }

        if (Origin.empty()) {
            return false;
        }

        if (Distance2DToLocalPlayer <= 0 || Distance2DToLocalPlayer > 300) {
            return false;
        }

        return true;
    }

    // 检查 valid 字段是否在正常范围
    virtual void ValidCheck() override {
        if (valid) {
            if (valid > 0x7FFFFFFEFFFF || valid < 0x7FF000000000) {
                BasePointer = 0;
                valid = 0;
            }
        }
    }

    // 可见性检测
    bool VisCheck() {
        bool Visibility = false;
        if (LastVisibleTime > OldVisTime || LastVisibleTime < 0.f && OldVisTime > 0.f) {
            Visibility = true;
        }
        OldVisTime = LastVisibleTime;
        return Visibility;
    }

    // 更新被瞄准状态
    void AimedAtCheck() {
        if (LastTimeAimedAt > LastTimeAimedAtPrevious) {
            IsAimedAt = true;
            LastTimeAimedAtPrevious = LastTimeAimedAt;
            return;
        }

        IsAimedAt = false;
    }

    struct LevelInfo {
        std::string rankName; // 段位名称
        uint32_t minScore;    // 对应的最小积分
    };

    std::vector<struct LevelInfo> rankInfo = {
            {"Rookie IV", 0}, {"Rookie III", 250}, {"Rookie II", 500}, {"Rookie I", 750},
            {"Bronze IV", 1000}, {"Bronze III", 1500}, {"Bronze II", 2000}, {"Bronze I", 2500},
            {"Silver IV", 3000}, {"Silver III", 3600}, {"Silver II", 4200}, {"Silver I", 4800},
            {"Gold IV", 5400}, {"Gold III", 6100}, {"Gold II", 6800}, {"Gold I", 7500},
            {"Platinum IV", 8200}, {"Platinum III", 9000}, {"Platinum II", 9800}, {"Platinum I", 10600},
            {"Diamond IV", 11400}, {"Diamond III", 12300}, {"Diamond II", 13200}, {"Diamond I", 14100},
            {"Master", 15000}, {"Predator", 999999}
    };

    // 根据积分查找对应段位
    std::string findRankByScore(uint32_t score) {
        if (rankInfo.empty()) {
            return "Unranked";
        }
        for (size_t i = 0; i < rankInfo.size() - 1; ++i) {
            if (score >= rankInfo[i].minScore && score < rankInfo[i + 1].minScore) {
                return rankInfo[i].rankName;
            }
        }
        return "Unranked";
    }

    std::string getRankName() {
        return findRankByScore(Rank);
    }

    // 通过经验值换算等级
    inline int GetPlayerLevel() {
        int m_xp = Level; // 经验值
        if (m_xp < 0) return 0;
        if (m_xp < 100) return 1;

        int levels[] = { 2750, 6650, 11400, 17000, 23350, 30450, 38300, 46450, 55050, 64100, 73600, 83550, 93950, 104800, 116100, 127850, 140050, 152400, 164900, 177550, 190350, 203300, 216400, 229650, 243050, 256600, 270300, 284150, 298150, 312300, 326600, 341050, 355650, 370400, 385300, 400350, 415550, 430900, 446400, 462050, 477850, 493800, 509900, 526150, 542550, 559100, 575800, 592650, 609650, 626800, 644100, 661550, 679150, 696900, 714800 };

        int level = 56;
        int arraySize = sizeof(levels) / sizeof(levels[0]);

        for (int i = 0; i < arraySize; i++)
        {
            if (m_xp < levels[i])
            {
                return i + 1;
            }
        }

        return level + ((m_xp - levels[arraySize - 1] + 1) / 18000);
    }
};
