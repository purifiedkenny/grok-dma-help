#pragma once
#include <cstdint>
#include <immintrin.h> // SIMD指令集
#include "VecDef.h"

// 3x4矩阵类
class Matrix3x4 {
public:
    float matrix[3][4];

    Matrix3x4() {
        // 初始化为单位矩阵
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                matrix[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    // 获取位置信息
    Vec3D GetPosition() const {
        return Vec3D(matrix[0][3], matrix[1][3], matrix[2][3]);
    }
};

struct ViewMatrix {
public:
    float matrix[4][4];

    std::array<std::array<double, 4>, 4> M;

    ViewMatrix()
    {
        M = { { { 0, 0, 0, 0 },
                { 0, 0, 0, 0 },
                { 0, 0, 0, 0 },
                { 0, 0, 0, 0 } } };
    }

    Vector3D Transform(const Vector3D vector) const {
        Vector3D transformed;


        // 使用SIMD加速计算
#ifdef __AVX__
        __m256 vec = _mm256_set_ps(1.0f, vector.z, vector.y, vector.x,
            vector.z, vector.y, vector.x, 0.0f);

        // 优化矩阵行访问
        __m128 row0 = _mm_loadu_ps(&matrix[0][0]);
        __m128 row1 = _mm_loadu_ps(&matrix[1][0]);
        __m128 row3 = _mm_loadu_ps(&matrix[3][0]);

        // 扩展到256位寄存器
        __m256 row0_256 = _mm256_insertf128_ps(_mm256_castps128_ps256(row0), row0, 1);
        __m256 row1_256 = _mm256_insertf128_ps(_mm256_castps128_ps256(row1), row1, 1);
        __m256 row3_256 = _mm256_insertf128_ps(_mm256_castps128_ps256(row3), row3, 1);

        // 计算点积
        __m256 result0 = _mm256_dp_ps(vec, row0_256, 0xF1);
        __m256 result1 = _mm256_dp_ps(vec, row1_256, 0xF2);
        __m256 result3 = _mm256_dp_ps(vec, row3_256, 0xF4);

        // 合并结果
        __m256 result = _mm256_or_ps(_mm256_or_ps(result0, result1), result3);

        // 存储结果
        float resultArr[8];
        _mm256_storeu_ps(resultArr, result);

        transformed.x = resultArr[0];
        transformed.y = resultArr[1];
        transformed.z = resultArr[2];
#else
// 非AVX版本回退到标准计算
        transformed.x = vector.y * matrix[0][1] + vector.x * matrix[0][0] + vector.z * matrix[0][2] + matrix[0][3];
        transformed.y = vector.y * matrix[1][1] + vector.x * matrix[1][0] + vector.z * matrix[1][2] + matrix[1][3];
        transformed.z = vector.y * matrix[3][1] + vector.x * matrix[3][0] + vector.z * matrix[3][2] + matrix[3][3];
#endif

        return transformed;
    }


    Vec4D Transform4D(const Vector3D vector, float px, float py) const {

        Vec4D Box; // 存储变换后的VectorQuantity。
        float camera = matrix[3][0] * vector.x + matrix[3][1] * vector.y + matrix[3][2] * vector.z + matrix[3][3];

        float r_x = px + (matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2] * vector.z + matrix[0][3]) / camera * px;
        float r_y = py - (matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * (vector.z + 80) + matrix[1][3]) / camera * py;
        float r_w = py - (matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * (vector.z + 160) + matrix[1][3]) / camera * py;

        Box.x = (r_x - (r_y - r_w) / 4);
        Box.y = r_y;
        Box.w = (r_y - r_w) / 2;
        Box.z = r_y - r_w;

        return Box;
    }
};

class GameView
{
public:
    uint64_t RenderPointer;
    ViewMatrix GameViewMatrix;
    Vec2D ScreenSize;
    Vec2D ScreenCenter;
    void UpdateScreenSize(float width, float height) {

        ScreenSize = { width,height };
        ScreenCenter = ScreenSize / 2.f;
    }

    bool WorldToScreen(const Vec3D& worldPosition, Vec2D& screenPosition) const {
        // 使用矩阵变换将世界坐标转换为屏幕坐标
        Vec3D transformed = GameViewMatrix.Transform(worldPosition);

        // W除法（透视除法）
        if (transformed.z < 0.001f)
            return false;

        // 计算屏幕坐标
        float invZ = 1.0f / transformed.z;
        screenPosition.x = (ScreenSize.x * 0.5f) + (transformed.x * invZ * ScreenSize.x * 0.5f);
        screenPosition.y = (ScreenSize.y * 0.5f) - (transformed.y * invZ * ScreenSize.y * 0.5f);

        // 检查坐标是否在屏幕范围内
        return (screenPosition.x >= 0 && screenPosition.x <= ScreenSize.x &&
            screenPosition.y >= 0 && screenPosition.y <= ScreenSize.y);
    }

    // 世界坐标到屏幕坐标的转换，无裁剪检查版本
    bool WorldToScreenNoClip(const Vec3D& worldPosition, Vec2D& screenPosition) const {
        // 使用矩阵变换将世界坐标转换为屏幕坐标
        Vec3D transformed = GameViewMatrix.Transform(worldPosition);

        // W除法（透视除法）
        if (transformed.z < 0.001f)
            return false;

        // 计算屏幕坐标
        float invZ = 1.0f / transformed.z;
        screenPosition.x = (ScreenSize.x * 0.5f) + (transformed.x * invZ * ScreenSize.x * 0.5f);
        screenPosition.y = (ScreenSize.y * 0.5f) - (transformed.y * invZ * ScreenSize.y * 0.5f);

        // 不进行裁剪检查，总是返回true
        return true;
    }

    bool WorldToScreen4D(const Vec3D& worldPosition, Vec4D& screenBox) {
        // 首先检查点是否可投影
        Vec2D temp;
        if (!WorldToScreenNoClip(worldPosition, temp))
            return false;

        // 计算边界框
        screenBox = GameViewMatrix.Transform4D(worldPosition, ScreenSize.x, ScreenSize.y);
        return true;
    }

    // 高性能版本 - 使用SIMD进行矩阵变换
    __forceinline bool W2S_HighPerformance(const Vec3D& worldPosition, Vec2D& screenPosition) const {
        // 使用SSE指令加速矩阵乘法
#ifdef __AVX__
// AVX版本（如果可用）
        __m256 worldVec = _mm256_set_ps(1.0f, worldPosition.z, worldPosition.y, worldPosition.x,
            worldPosition.z, worldPosition.y, worldPosition.x, 0.0f);

        // 转置矩阵的前两行和最后一行加载到AVX寄存器
        __m256 row0 = _mm256_set_ps(GameViewMatrix.matrix[0][3], GameViewMatrix.matrix[0][2],
            GameViewMatrix.matrix[0][1], GameViewMatrix.matrix[0][0], 0, 0, 0, 0);
        __m256 row1 = _mm256_set_ps(GameViewMatrix.matrix[1][3], GameViewMatrix.matrix[1][2],
            GameViewMatrix.matrix[1][1], GameViewMatrix.matrix[1][0], 0, 0, 0, 0);
        __m256 row3 = _mm256_set_ps(GameViewMatrix.matrix[3][3], GameViewMatrix.matrix[3][2],
            GameViewMatrix.matrix[3][1], GameViewMatrix.matrix[3][0], 0, 0, 0, 0);

        // 计算x,y,z分量
        __m256 mulResult = _mm256_dp_ps(worldVec, row0, 0xF1);
        __m256 mulResult2 = _mm256_dp_ps(worldVec, row1, 0xF2);
        __m256 mulResult3 = _mm256_dp_ps(worldVec, row3, 0xF4);

        __m256 result = _mm256_or_ps(_mm256_or_ps(mulResult, mulResult2), mulResult3);

        // 提取结果
        float transformed[8];
        _mm256_storeu_ps(transformed, result);

        float transformedX = transformed[0];
        float transformedY = transformed[1];
        float transformedZ = transformed[2];

#else
// SSE版本（更广泛支持）
        __m128 worldVec = _mm_set_ps(1.0f, worldPosition.z, worldPosition.y, worldPosition.x);

        // 加载矩阵行
        __m128 row0 = _mm_set_ps(GameViewMatrix.matrix[0][3], GameViewMatrix.matrix[0][2],
            GameViewMatrix.matrix[0][1], GameViewMatrix.matrix[0][0]);
        __m128 row1 = _mm_set_ps(GameViewMatrix.matrix[1][3], GameViewMatrix.matrix[1][2],
            GameViewMatrix.matrix[1][1], GameViewMatrix.matrix[1][0]);
        __m128 row3 = _mm_set_ps(GameViewMatrix.matrix[3][3], GameViewMatrix.matrix[3][2],
            GameViewMatrix.matrix[3][1], GameViewMatrix.matrix[3][0]);

        // 点积计算
        __m128 mulResult0 = _mm_mul_ps(worldVec, row0);
        __m128 mulResult1 = _mm_mul_ps(worldVec, row1);
        __m128 mulResult3 = _mm_mul_ps(worldVec, row3);

        // 水平相加
        mulResult0 = _mm_hadd_ps(mulResult0, mulResult0);
        mulResult0 = _mm_hadd_ps(mulResult0, mulResult0);

        mulResult1 = _mm_hadd_ps(mulResult1, mulResult1);
        mulResult1 = _mm_hadd_ps(mulResult1, mulResult1);

        mulResult3 = _mm_hadd_ps(mulResult3, mulResult3);
        mulResult3 = _mm_hadd_ps(mulResult3, mulResult3);

        float transformedX, transformedY, transformedZ;
        _mm_store_ss(&transformedX, mulResult0);
        _mm_store_ss(&transformedY, mulResult1);
        _mm_store_ss(&transformedZ, mulResult3);
#endif

        // 快速剔除检查
        if (transformedZ < 0.001f)
            return false;

        // 避免除法操作，使用乘法代替
        float invZ = _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ss(transformedZ)));

        // 预乘以屏幕尺寸常量
        screenPosition.x = ScreenCenter.x + (transformedX * invZ * ScreenCenter.x);
        screenPosition.y = ScreenCenter.y - (transformedY * invZ * ScreenCenter.y);

        // 快速边界检查 (可选，如果后续代码会处理屏幕外的点，可以移除)
        return (screenPosition.x >= 0 && screenPosition.x <= ScreenSize.x &&
            screenPosition.y >= 0 && screenPosition.y <= ScreenSize.y);
    }

    // 批量处理多个坐标的高性能版本
    __forceinline int W2S_Batch(const Vec3D* worldPositions, Vec2D* screenPositions,
        bool* results, int count) const {
        int validCount = 0;

        // 预加载常量到寄存器
#ifdef __AVX__
        __m256 halfWidthReg = _mm256_set1_ps(halfWidth);
        __m256 halfHeightReg = _mm256_set1_ps(halfHeight);
        __m256 screenWidthReg = _mm256_set1_ps(ScreenSize.x);
        __m256 screenHeightReg = _mm256_set1_ps(ScreenSize.y);
        __m256 zeroReg = _mm256_setzero_ps();
        __m256 smallValueReg = _mm256_set1_ps(0.001f);
#else
        __m128 halfWidthReg = _mm_set1_ps(ScreenCenter.x);
        __m128 halfHeightReg = _mm_set1_ps(ScreenCenter.y);
        __m128 screenWidthReg = _mm_set1_ps(ScreenSize.x);
        __m128 screenHeightReg = _mm_set1_ps(ScreenSize.y);
        __m128 zeroReg = _mm_setzero_ps();
        __m128 smallValueReg = _mm_set1_ps(0.001f);
#endif

        // 批量处理
        for (int i = 0; i < count; i++) {
            results[i] = W2S_HighPerformance(worldPositions[i], screenPositions[i]);
            validCount += results[i] ? 1 : 0;
        }

        return validCount;
    }
};