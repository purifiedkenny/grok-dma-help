#include "Skeleton.h"
#include <unordered_set>


int GetBoneFromHitbox2(Player* p, HitboxType HitBox) {
    // 首先检查模型指针是否有效
    if (!mem.IsValidPointer(p->ModelPointer))
        return -1; // 如果模型指针无效，返回-1

    // 读取StudioHDR，这是模型数据的一个部分，包含了重要的模型信息
    uint64_t StudioHDR = mem.ReadCache<uint64_t>(p->ModelPointer + 0x8);
    // 检查StudioHDR指针的有效性
    if (!mem.IsValidPointer(StudioHDR + 0x34))
        return -1; // 如果无效，返回-1

    // 读取击中盒缓存信息
    auto HitboxCache = mem.ReadCache<uint16_t>(StudioHDR + 0x34);
    // 通过缓存信息计算出击中盒数组的地址
    auto HitboxArray = StudioHDR + (static_cast<uint64_t>(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
    // 检查击中盒数组指针的有效性
    if (!mem.IsValidPointer(HitboxArray + 0x4))
        return -1; // 如果无效，返回-1

    // 读取索引缓存信息
    auto IndexCache = mem.ReadCache<uint16_t>(HitboxArray + 0x4);
    // 通过索引缓存信息计算出击中盒索引的地址
    auto HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
    // 根据给定的HitBox类型计算出对应骨骼的指针
    auto BonePtr = HitboxIndex + HitboxArray + (static_cast<int>(HitBox) * 0x20);
    // 检查骨骼指针的有效性
    if (!mem.IsValidPointer(BonePtr))
        return -1; // 如果无效，返回-1

    // 读取并返回骨骼索引
    return mem.ReadCache<uint16_t>(BonePtr);
}

std::array<std::pair<HitboxType, HitboxType>, 15> boneConnections = { {
    {HitboxType::Head, HitboxType::Neck},         // Head to Neck
    {HitboxType::Neck, HitboxType::UpperChest},         // Head to Neck
    {HitboxType::UpperChest, HitboxType::Hip},          // Neck to Bottom
    {HitboxType::Neck, HitboxType::MidSpine},     // Neck to Left Shoulder
    {HitboxType::MidSpine, HitboxType::LowerSpine}, // Left Shoulder to Left Elbow
    {HitboxType::LowerSpine, HitboxType::PropGun},  // Left Elbow to Left Hand
    {HitboxType::Neck, HitboxType::NeckA},        // Neck to Right Shoulder
    {HitboxType::NeckA, HitboxType::NeckB},       // Right Shoulder to Right Elbow
    {HitboxType::NeckB, HitboxType::NeckC},       // Right Elbow to Right Hand
    {HitboxType::Hip, HitboxType::HeadB},         // Bottom to Left Thigh
    {HitboxType::HeadB, HitboxType::LeftClavicle}, // Left Thigh to Left Knee
    {HitboxType::LeftClavicle, HitboxType::LeftShoulder}, // Left Knee to Left Leg
    {HitboxType::Hip, HitboxType::LeftShoulderMid},  // Bottom to Right Thigh
    {HitboxType::LeftShoulderMid, HitboxType::LeftElbowB}, // Right Thigh to Right Knee
    {HitboxType::LeftElbowB, HitboxType::LeftElbow}  // Right Knee to Right Leg
} };

std::array<HitboxType, 16> criticalHitboxes = {
    HitboxType::Head,  HitboxType::Neck, HitboxType::UpperChest, HitboxType::Hip,
    HitboxType::MidSpine, HitboxType::LowerSpine, HitboxType::PropGun,
    HitboxType::NeckA, HitboxType::NeckB, HitboxType::NeckC,
    HitboxType::HeadB, HitboxType::LeftClavicle, HitboxType::LeftShoulder,
    HitboxType::LeftShoulderMid, HitboxType::LeftElbowB, HitboxType::LeftElbow
};

void SkeletonSystem::BatchUpdateBones() {


    auto& buffer = GetInactiveBuffer();

    // 记录本次处理的玩家索引，避免重复处理
    std::unordered_set<int> processedIndices;

    // 重置所有玩家缓存状态
    for (size_t i = 0; i < MAX_PLAYERS; ++i) {
        buffer.cache[i].valid = false;
    }

    const auto& players = entityManager.getPlayers();
    if (players->empty()) return;

    size_t cacheIndex = 0;
    for (const auto & player : *players) {
        if (player == nullptr || player->IsLocal || processedIndices.find(player->Index) != processedIndices.end()) {
            continue;
        }

        auto cfgUse = player->IsDummy() ? esp->bots : player->IsTeamMate ? esp->teammates : esp->enemies;
        float maxDistance = cfgUse.distance > aimbot->aimParams.maxDistance ? cfgUse.distance : aimbot->aimParams.maxDistance;
        if (player->DistanceToLocalPlayer > maxDistance) {
            continue;
        }

        // 记录已处理的玩家索引
        processedIndices.insert(player->Index);

        // 获取缓存位置
        auto& playerCache = buffer.cache[cacheIndex];
        playerCache.valid = false;
        playerCache.playerIndex = player->Index;
        playerCache.distance = player->DistanceToLocalPlayer;

        if (!cfgUse.showSkeleton && player->DistanceToLocalPlayer > aimbot->aimParams.maxDistance) {
            continue;
        }

        bool allValid = true;

        // 预分配固定大小的矩阵数组，避免动态分配
        std::array<Matrix3x4, MAX_BONES> boneMatrices;
        auto handle = mem.CreateScatterHandle();
        std::vector<size_t> validBoneIndices;
        validBoneIndices.reserve(criticalHitboxes.size());

        // 第一步：收集所有需要读取的骨骼位置
        for (const auto& hitbox : criticalHitboxes) {
            int Bone = GetBoneFromHitbox2(player, hitbox);
            if (Bone < 0 || Bone > 255) continue;

            uint64_t TempBonePointer = player->BonePointer + (Bone * sizeof(Matrix3x4));
            if (!mem.IsValidPointer(TempBonePointer)) continue;

            validBoneIndices.push_back(static_cast<size_t>(hitbox));
            mem.AddScatterReadRequest(handle, TempBonePointer, &boneMatrices[static_cast<size_t>(hitbox)], sizeof(Matrix3x4));
        }

        // 检查是否有有效的骨骼需要读取
        if (validBoneIndices.empty()) {
            mem.CloseScatterHandle(handle);
            continue;
        }

        // 执行批量读取
        mem.ExecuteReadScatter(handle);
        mem.CloseScatterHandle(handle);


        // 处理读取到的数据
        for (size_t boneIdx : validBoneIndices) {
            const Matrix3x4& mat3x4 = boneMatrices[boneIdx];
            Vec3D worldPos = mat3x4.GetPosition();

            if (worldPos.empty()) {
                allValid = false;
                continue;
            }

            // 计算相对位置并存储
            playerCache.bones[boneIdx].relativePos = Vec3D(
                worldPos.x,
                worldPos.y,
                worldPos.z
            );
            playerCache.bones[boneIdx].valid = true;
        }

        playerCache.valid = allValid;

        // 更新缓存索引，确保不超出数组范围
        if (++cacheIndex >= MAX_PLAYERS) break;
    }

    buffer.updateCounter.fetch_add(1, std::memory_order_release);
    SwapBuffers();
}

void SkeletonSystem::Render(ImDrawList* drawList) {
    const auto& players = entityManager.getPlayers();
    if (!drawList || players->empty()) return;

    const auto& buffer = GetActiveBuffer();

    struct ScreenPos {
        Vec2D pos;
        bool valid;
    };
    std::array<ScreenPos, MAX_BONES> screenPositions;

    // 记录已经渲染过的玩家索引
    std::unordered_set<int> renderedIndices;

    for (Player* player : *players) {
        // 跳过无效玩家或已渲染的玩家
        if (!player || !player->IsValid() || player->IsTeamMate || player->IsLocal ||
            renderedIndices.find(player->Index) != renderedIndices.end()) {
            continue;
        }

        // 记录已渲染的玩家
        renderedIndices.insert(player->Index);

        // 查找对应的缓存数据
        const PlayerBoneCache* playerCache = nullptr;
        for (size_t i = 0; i < MAX_PLAYERS; ++i) {
            if (buffer.cache[i].valid && buffer.cache[i].playerIndex == player->Index) {
                playerCache = &buffer.cache[i];
                break;
            }
        }

        // 如果没有找到缓存，或者距离超出限制，则跳过
        auto cfgUse = player->IsDummy() ? esp->bots : player->IsTeamMate ? esp->teammates : esp->enemies;
        if (!playerCache || !playerCache->valid || playerCache->distance > cfgUse.distance || !cfgUse.showSkeleton) {
            continue;
        }

        Vec3D currentOrigin = player->Origin;
        bool hasValidBones = true;

        // 转换骨骼到屏幕坐标
        for (const auto& hitbox : criticalHitboxes) {
            int idx = static_cast<int>(hitbox);
            const auto& bone = playerCache->bones[idx];

            if (!bone.valid) {
                screenPositions[idx].valid = false;
                hasValidBones = false;
                continue;
            }

            Vec3D worldPos = Vec3D(
                currentOrigin.x + bone.relativePos.x,
                currentOrigin.y + bone.relativePos.y,
                currentOrigin.z + bone.relativePos.z
            );

            screenPositions[idx].valid = gameView->WorldToScreen(
                worldPos,
                screenPositions[idx].pos
            );

            if (!screenPositions[idx].valid) {
                hasValidBones = false;
            }
        }

        if (!hasValidBones) continue;

        // 绘制骨骼线
        ImU32 color = ImGui::ColorConvertFloat4ToU32(
            player->IsVisible ?
            cfgUse.visibleSkeletonColor :
            cfgUse.invisibleSkeletonColor
        );

        for (const auto& [start, end] : boneConnections) {
            int startIdx = static_cast<int>(start);
            int endIdx = static_cast<int>(end);

            if (!screenPositions[startIdx].valid || !screenPositions[endIdx].valid) continue;

            drawList->AddLine(
                ImVec2(screenPositions[startIdx].pos.x, screenPositions[startIdx].pos.y),
                ImVec2(screenPositions[endIdx].pos.x, screenPositions[endIdx].pos.y),
                color,
                cfgUse.skeletonThickness
            );
        }
    }
}

SkeletonSystem::SkeletonSystem()  {
}

SkeletonSystem::~SkeletonSystem() {
}

bool SkeletonSystem::IsValidBonePosition(const Vec3D& bonePos, const Vec3D& origin) const {
    static constexpr float MAX_BONE_DISTANCE = 100.0f;
    return std::abs(bonePos.x - origin.x) <= MAX_BONE_DISTANCE &&
        std::abs(bonePos.y - origin.y) <= MAX_BONE_DISTANCE &&
        std::abs(bonePos.z - origin.z) <= MAX_BONE_DISTANCE;
}

void SkeletonSystem::UpdateThread() {
    while (true) {
        if (!entityManager.isLoggedIn())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        if (!entityManager.isInGame() || !localPlayer || !localPlayer->IsValid()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        try {
            BatchUpdateBones();
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in bone update thread: " << e.what() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

// 修改GetBoneWorldPosition方法，使用索引匹配玩家
bool SkeletonSystem::GetBoneWorldPosition(Player* player, HitboxType hitbox, Vec3D& outPosition) {
    if (!player || !player->IsDataValid()) {
        return false;
    }

    const auto& buffer = GetActiveBuffer();
    Vec3D playerOrigin = player->Origin;

    // 根据索引查找玩家
    for (size_t i = 0; i < MAX_PLAYERS; ++i) {
        const auto& playerCache = buffer.cache[i];
        if (!playerCache.valid) continue;

        // 优先使用索引匹配
        if (playerCache.playerIndex == player->Index) {
            // 找到玩家后，获取骨骼
            int idx = static_cast<int>(hitbox);
            const auto& bone = playerCache.bones[idx];

            if (!bone.valid) {
                return false;
            }

            outPosition = Vec3D(
                playerOrigin.x + bone.relativePos.x,
                playerOrigin.y + bone.relativePos.y,
                playerOrigin.z + bone.relativePos.z
            );

            return true;
        }
    }

    // 回退到距离匹配（兼容性）
    for (size_t i = 0; i < MAX_PLAYERS; ++i) {
        const auto& playerCache = buffer.cache[i];
        if (!playerCache.valid) continue;

        if (std::abs(playerCache.distance - player->DistanceToLocalPlayer) < 0.1f) {
            int idx = static_cast<int>(hitbox);
            const auto& bone = playerCache.bones[idx];

            if (!bone.valid) {
                return false;
            }

            outPosition = Vec3D(
                playerOrigin.x + bone.relativePos.x,
                playerOrigin.y + bone.relativePos.y,
                playerOrigin.z + bone.relativePos.z
            );

            return true;
        }
    }

    return false;
}

bool SkeletonSystem::GetMultipleBonePositions(Player* player, const std::vector<HitboxType>& hitboxes,
    std::unordered_map<HitboxType, Vec3D>& outPositions) {
    if (!player || !player->IsDataValid() || hitboxes.empty()) {
        return false;
    }

    const auto& buffer = GetActiveBuffer();
    Vec3D playerOrigin = player->Origin;

    // 在缓存中查找玩家（优先使用索引）
    size_t playerIndex = SIZE_MAX;
    for (size_t i = 0; i < MAX_PLAYERS; ++i) {
        const auto& playerCache = buffer.cache[i];
        if (!playerCache.valid) continue;

        if (playerCache.playerIndex == player->Index) {
            playerIndex = i;
            break;
        }
    }

    // 回退到距离匹配
    if (playerIndex == SIZE_MAX) {
        for (size_t i = 0; i < MAX_PLAYERS; ++i) {
            const auto& playerCache = buffer.cache[i];
            if (!playerCache.valid) continue;

            if (std::abs(playerCache.distance - player->DistanceToLocalPlayer) < 0.1f) {
                playerIndex = i;
                break;
            }
        }
    }

    if (playerIndex == SIZE_MAX) {
        return false; // 在缓存中没有找到玩家
    }

    bool anyValid = false;

    // 获取所有请求的骨骼位置
    for (const auto& hitbox : hitboxes) {
        int idx = static_cast<int>(hitbox);
        const auto& bone = buffer.cache[playerIndex].bones[idx];

        if (!bone.valid) continue;

        outPositions[hitbox] = Vec3D(
            playerOrigin.x + bone.relativePos.x,
            playerOrigin.y + bone.relativePos.y,
            playerOrigin.z + bone.relativePos.z
        );

        anyValid = true;
    }

    return anyValid;
}

// 实现IsBoneVisible方法
bool SkeletonSystem::IsBoneVisible(Player* player, HitboxType hitbox) {
    if (!player || !player->IsDataValid()) {
        return false;
    }

    Vec3D bonePos;
    if (!GetBoneWorldPosition(player, hitbox, bonePos)) {
        return false;
    }

    // 简单的可见性检查 - 在实际实现中可能会更复杂
    return player->IsVisible;
}

// 实现GetBestAimBone方法
HitboxType SkeletonSystem::GetBestAimBone(Player* player, const std::vector<HitboxType>& preferredHitboxes) {
    if (!player || !player->IsDataValid() || preferredHitboxes.empty() || !localPlayer || !localPlayer->IsValid()) {
        return HitboxType::Head; // 如果数据无效，默认瞄准头部
    }

    const auto& buffer = GetActiveBuffer();

    // 在缓存中查找玩家（优先使用索引）
    size_t playerIndex = SIZE_MAX;
    for (size_t i = 0; i < MAX_PLAYERS; ++i) {
        const auto& playerCache = buffer.cache[i];
        if (!playerCache.valid) continue;

        if (playerCache.playerIndex == player->Index) {
            playerIndex = i;
            break;
        }
    }

    // 回退到距离匹配
    if (playerIndex == SIZE_MAX) {
        for (size_t i = 0; i < MAX_PLAYERS; ++i) {
            const auto& playerCache = buffer.cache[i];
            if (!playerCache.valid) continue;

            if (std::abs(playerCache.distance - player->DistanceToLocalPlayer) < 0.1f) {
                playerIndex = i;
                break;
            }
        }
    }

    if (playerIndex == SIZE_MAX) {
        return HitboxType::Head; // 如果在缓存中没有找到玩家，默认瞄准头部
    }

    // 获取视角以计算最接近准星的骨骼
    AimAngle viewAngles = { localPlayer->ViewAngles.x, localPlayer->ViewAngles.y };
    Vec3D cameraOrigin = localPlayer->CameraOrigin;
    Vec3D playerOrigin = player->Origin;

    float closestDistance = 999.0f;
    HitboxType bestHitbox = preferredHitboxes[0];

    for (const auto& hitbox : preferredHitboxes) {
        int boneIdx = static_cast<int>(hitbox);
        const auto& boneData = buffer.cache[playerIndex].bones[boneIdx];

        if (!boneData.valid) continue;

        Vec3D worldPos = Vec3D(
            playerOrigin.x + boneData.relativePos.x,
            playerOrigin.y + boneData.relativePos.y,
            playerOrigin.z + boneData.relativePos.z
        );

        // 计算到这个骨骼的角度
        Vec3D delta = worldPos - cameraOrigin;
        float length = delta.length();

        if (length < 0.0001f) continue;

        float pitch = -asinf(delta.z / length) * RAD2DEG_CONST;
        float yaw = atan2f(delta.y, delta.x) * RAD2DEG_CONST;

        AimAngle boneAngle(pitch, yaw);

        // 计算与当前视角的角度差距
        float distance = std::abs(viewAngles.pitch - boneAngle.pitch) +
            std::abs(viewAngles.yaw - boneAngle.yaw);

        if (distance < closestDistance) {
            closestDistance = distance;
            bestHitbox = hitbox;
        }
    }

    return bestHitbox;
}

bool SkeletonSystem::GetBestBonePositionForAiming(Player* player, Vec3D& outPosition, bool forceHead, int forceBone) {
    if (!player || !player->IsDataValid()) {
        return false;
    }

    // 如果指定了强制骨骼，则直接使用该骨骼
    if (forceBone >= 0) {
        return GetBoneWorldPosition(player, static_cast<HitboxType>(forceBone), outPosition);
    }

    // 如果指定了强制瞄头
    if (forceHead) {
        return GetBoneWorldPosition(player, HitboxType::Head, outPosition);
    }

    // 否则，使用优先级列表进行自动选择
    std::vector<HitboxType> bonePriority = {
        HitboxType::Head,
        HitboxType::Neck,
        HitboxType::UpperChest,
        HitboxType::LowerSpine
    };

    // 从这些选项中获取最佳骨骼
    HitboxType bestBone = GetBestAimBone(player, bonePriority);

    // 获取最佳骨骼的位置
    return GetBoneWorldPosition(player, bestBone, outPosition);
}