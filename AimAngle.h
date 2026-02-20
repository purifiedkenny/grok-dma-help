#pragma once
#include <corecrt_math.h>

class AimAngle {
public:
    float pitch;  // x轴角度
    float yaw;    // y轴角度

    AimAngle() : pitch(0), yaw(0) {}
    AimAngle(float _pitch, float _yaw) : pitch(_pitch), yaw(_yaw) {}

    // 角度归一化
    void Normalize() {
        // 限制pitch在-89到89度之间
        while (pitch > 89.0f) pitch -= 180.0f;
        while (pitch < -89.0f) pitch += 180.0f;

        // 限制yaw在-180到180度之间
        while (yaw > 180.0f) yaw -= 360.0f;
        while (yaw < -180.0f) yaw += 360.0f;
    }

    // 计算与另一个角度的距离
    float DistanceTo(const AimAngle& other) const {
        float pitchDiff = pitch - other.pitch;
        float yawDiff = yaw - other.yaw;

        // 确保yaw差距不超过180度
        if (yawDiff > 180.0f) yawDiff -= 360.0f;
        else if (yawDiff < -180.0f) yawDiff += 360.0f;

        return sqrtf(pitchDiff * pitchDiff + yawDiff * yawDiff);
    }

    // 运算符重载
    AimAngle operator-(const AimAngle& other) const {
        return AimAngle(pitch - other.pitch, yaw - other.yaw);
    }

    AimAngle operator+(const AimAngle& other) const {
        return AimAngle(pitch + other.pitch, yaw + other.yaw);
    }

    AimAngle operator/(float divisor) const {
        return AimAngle(pitch / divisor, yaw / divisor);
    }
};
