#pragma once
#include <array>
#include <vector>
#include <atomic>
#include <thread>
#include "include.h"


class SkeletonSystem {
public:
    SkeletonSystem();
    ~SkeletonSystem();
    void BatchUpdateBones();
    void Render(ImDrawList* drawList);

    bool GetBoneWorldPosition(Player* player, HitboxType hitbox, Vec3D& outPosition);

    // 从缓存中获取多个骨骼位置
    bool GetMultipleBonePositions(Player* player, const std::vector<HitboxType>& hitboxes,
        std::unordered_map<HitboxType, Vec3D>& outPositions);

    // 从缓存中检查骨骼是否可见
    bool IsBoneVisible(Player* player, HitboxType hitbox);

    // 从缓存中获取最佳瞄准骨骼
    HitboxType GetBestAimBone(Player* player, const std::vector<HitboxType>& preferredHitboxes);

    bool GetBestBonePositionForAiming(Player* player, Vec3D& outPosition, bool forceHead, int forceBone);

private:
    static constexpr size_t MAX_PLAYERS = 100;
    static constexpr size_t MAX_BONES = 65;
    static constexpr size_t BUFFER_COUNT = 2;
    // 简化的骨骼数据结构 - 只存储相对位置
    struct BoneData {
        Vec3D relativePos;  // 相对于origin的位置
        bool valid;
        BoneData() : valid(false) {}
    };
    struct alignas(64) PlayerBoneCache {
        std::array<BoneData, MAX_BONES> bones;
        float distance;
        bool valid;
        int playerIndex;
        PlayerBoneCache() : distance(0.0f), valid(false), playerIndex(-1) {}
    };
    struct alignas(64) BufferData {
        std::array<PlayerBoneCache, MAX_PLAYERS> cache;
        std::atomic<uint64_t> updateCounter{ 0 };
        BufferData() = default;
    };
    // 其他辅助结构保持不变
    struct BoneAddressCache {
        uint64_t boneAddress;
        Vec3D origin;
        bool valid;
        BoneAddressCache() : boneAddress(0), valid(false) {}
    };
    struct bone_t {
        BYTE pad[0xCC];
        float x;
        BYTE pad2[0xC];
        float y;
        BYTE pad3[0xC];
        float z;
    };
    struct TempBoneData {
        std::array<bone_t, MAX_BONES> positions;
    };
private:
    alignas(64) BufferData buffers[BUFFER_COUNT];
    std::atomic<int> activeIndex{ 0 };
    bool IsValidBonePosition(const Vec3D& bonePos, const Vec3D& origin) const;
   
    BufferData& GetInactiveBuffer() {
        return buffers[1 - activeIndex.load(std::memory_order_acquire)];
    }
    void SwapBuffers() {
        int current = activeIndex.load(std::memory_order_acquire);
        activeIndex.store(1 - current, std::memory_order_release);
    }
public:
    void UpdateThread();

    BufferData& GetActiveBuffer() {
        return buffers[activeIndex.load(std::memory_order_acquire)];
    }
};
inline SkeletonSystem* skeletonSystem = new SkeletonSystem;