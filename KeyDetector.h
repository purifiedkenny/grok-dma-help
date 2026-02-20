#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <Windows.h>

// 现代化的按键检测系统，不依赖旧的Singleton和Module类
class KeyDetector {
private:
    // 按键信息结构
    struct KeyInfo {
        int keyCode;                      // 按键代码
        std::string contextId;            // 所属上下文ID (比如类名)
        std::string keyId;                // 按键ID
        bool enabled;                     // 是否启用检测
        std::atomic<bool> currentState;   // 当前状态
        std::atomic<bool> lastState;      // 上一次状态
        std::atomic<bool> triggered;      // 触发状态
        std::atomic<uint64_t> lastPressTime; // 上次按下时间

        KeyInfo(const std::string& context, const std::string& key, int code)
            : keyCode(code), contextId(context), keyId(key), enabled(true),
            currentState(false), lastState(false), triggered(false), lastPressTime(0) {
        }
    };

    // 内部状态
    std::vector<std::shared_ptr<KeyInfo>> keys;
    std::unordered_map<std::string, std::shared_ptr<KeyInfo>> keyMap;
    //std::thread detectionThread;
    std::atomic<int> detectionMode{ 0 }; // 0=WinAPI, 1=Custom1, 2=Custom2


private:

    // 生成唯一按键标识符
    std::string generateKeyIdentifier(const std::string& contextId, const std::string& keyId) const;

    // 检测具体按键状态
    bool detectKeyState(int keyCode);

    // 检测WinAPI按键状态
    bool detectWinAPIKeyState(int keyCode);

    // 检测自定义输入方式1的按键状态
    bool detectCustom1KeyState(int keyCode);

    // 检测自定义输入方式2的按键状态
    bool detectCustom2KeyState(int keyCode);

    // 获取当前时间戳(毫秒)
    uint64_t getCurrentTimeMs() const;

public:

    KeyDetector() {};
    ~KeyDetector();

    // 按键检测线程函数
    void detectionThreadFunc();

    void shutdown();

    // 设置检测模式
    void setDetectionMode(int mode);
    int getDetectionMode() const;

    // 注册按键
    bool registerKey(const std::string& contextId, const std::string& keyId, int keyCode);

    // 批量注册键
    void registerKeysForContext(const std::string& contextId,
        const std::unordered_map<std::string, int>& keys);

    // 更新按键代码
    bool updateKeyCode(const std::string& contextId, const std::string& keyId, int keyCode);

    // 注销按键
    bool unregisterKey(const std::string& contextId, const std::string& keyId);

    // 清除上下文所有按键
    void clearContextKeys(const std::string& contextId);

    // 启用/禁用按键检测
    bool enableKey(const std::string& contextId, const std::string& keyId, bool enable = true);

    // 检查按键状态
    bool isKeyHeld(const std::string& contextId, const std::string& keyId);
    bool isKeyTriggered(const std::string& contextId, const std::string& keyId);

    // 清除按键触发状态
    void clearTrigger(const std::string& contextId, const std::string& keyId);

    // 获取按键上次按下的时间
    uint64_t getLastPressTime(const std::string& contextId, const std::string& keyId);
};