#include <chrono>
#include <algorithm>
#include <iostream>

#include <DMALibrary/Memory/Memory.h>
#include "KeyConverter.h"
#include "KeyDetector.h"

// 析构函数
KeyDetector::~KeyDetector() {
    shutdown();
}

void KeyDetector::shutdown() {
    // 清理所有按键
    
    keys.clear();
    keyMap.clear();
}

void KeyDetector::setDetectionMode(int mode) {
    if (mode >= 0 && mode <= 2) {
        detectionMode.store(mode, std::memory_order_release);
    }
}

int KeyDetector::getDetectionMode() const {
    return detectionMode.load(std::memory_order_acquire);
}

bool KeyDetector::registerKey(const std::string& contextId, const std::string& keyId, int keyCode) {
    // 参数验证
    if (contextId.empty() || keyId.empty() || keyCode == 0) {
        return false;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        // 检查是否已经注册
        auto it = keyMap.find(identifier);
        if (it != keyMap.end()) {
            // 已经注册，更新按键代码
            it->second->keyCode = keyCode;
            return true;
        }

        // 创建新的按键信息
        std::shared_ptr<KeyInfo> keyInfo;
        try {
            keyInfo = std::make_shared<KeyInfo>(contextId, keyId, keyCode);
        }
        catch (const std::exception& e) {
            std::cerr << "Error creating KeyInfo: " << e.what() << std::endl;
            return false;
        }

        // 添加到集合中
        keys.push_back(keyInfo);
        keyMap[identifier] = keyInfo;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in registerKey: " << e.what() << std::endl;
        return false;
    }
}

void KeyDetector::registerKeysForContext(const std::string& contextId,
    const std::unordered_map<std::string, int>& keysToRegister) {
    if (contextId.empty() || keysToRegister.empty()) {
        return;
    }

    try {
        for (const auto& [keyId, keyCode] : keysToRegister) {
            if (keyCode != 0) {
                registerKey(contextId, keyId, keyCode);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in registerKeysForContext: " << e.what() << std::endl;
    }
}

bool KeyDetector::updateKeyCode(const std::string& contextId, const std::string& keyId, int keyCode) {
    if (contextId.empty() || keyId.empty() || keyCode == 0) {
        return false;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        auto it = keyMap.find(identifier);
        if (it == keyMap.end()) {
            return false;
        }

        it->second->keyCode = keyCode;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in updateKeyCode: " << e.what() << std::endl;
        return false;
    }
}

bool KeyDetector::unregisterKey(const std::string& contextId, const std::string& keyId) {
    if (contextId.empty() || keyId.empty()) {
        return false;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        auto it = keyMap.find(identifier);
        if (it == keyMap.end()) {
            return false;
        }

        // 从映射表中移除
        auto keyInfo = it->second;
        keyMap.erase(it);

        // 从列表中移除
        keys.erase(std::remove(keys.begin(), keys.end(), keyInfo), keys.end());

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in unregisterKey: " << e.what() << std::endl;
        return false;
    }
}

void KeyDetector::clearContextKeys(const std::string& contextId) {
    if (contextId.empty()) {
        return;
    }

    try {
        

        // 找到所有属于该上下文的按键
        std::vector<std::shared_ptr<KeyInfo>> keysToRemove;
        std::vector<std::string> identifiersToRemove;

        for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
            if (it->second->contextId == contextId) {
                keysToRemove.push_back(it->second);
                identifiersToRemove.push_back(it->first);
            }
        }

        // 从映射表中移除
        for (const auto& identifier : identifiersToRemove) {
            keyMap.erase(identifier);
        }

        // 从列表中移除
        for (const auto& keyInfo : keysToRemove) {
            keys.erase(std::remove(keys.begin(), keys.end(), keyInfo), keys.end());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in clearContextKeys: " << e.what() << std::endl;
    }
}

bool KeyDetector::enableKey(const std::string& contextId, const std::string& keyId, bool enable) {
    if (contextId.empty() || keyId.empty()) {
        return false;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        auto it = keyMap.find(identifier);
        if (it == keyMap.end()) {
            return false;
        }

        it->second->enabled = enable;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in enableKey: " << e.what() << std::endl;
        return false;
    }
}

bool KeyDetector::isKeyHeld(const std::string& contextId, const std::string& keyId) {
    if (contextId.empty() || keyId.empty()) {
        return false;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        auto it = keyMap.find(identifier);
        if (it == keyMap.end() || !it->second->enabled) {
            return false;
        }

        return it->second->currentState.load(std::memory_order_acquire);
    }
    catch (const std::exception& e) {
        std::cerr << "Error in isKeyHeld: " << e.what() << std::endl;
        return false;
    }
}

bool KeyDetector::isKeyTriggered(const std::string& contextId, const std::string& keyId) {
    if (contextId.empty() || keyId.empty()) {
        return false;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        auto it = keyMap.find(identifier);
        if (it == keyMap.end() || !it->second->enabled) {
            return false;
        }

        return it->second->triggered.load(std::memory_order_acquire);
    }
    catch (const std::exception& e) {
        std::cerr << "Error in isKeyTriggered: " << e.what() << std::endl;
        return false;
    }
}

void KeyDetector::clearTrigger(const std::string& contextId, const std::string& keyId) {
    if (contextId.empty() || keyId.empty()) {
        return;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        auto it = keyMap.find(identifier);
        if (it == keyMap.end()) {
            return;
        }

        it->second->triggered.store(false, std::memory_order_release);
    }
    catch (const std::exception& e) {
        std::cerr << "Error in clearTrigger: " << e.what() << std::endl;
    }
}

uint64_t KeyDetector::getLastPressTime(const std::string& contextId, const std::string& keyId) {
    if (contextId.empty() || keyId.empty()) {
        return 0;
    }

    try {
        std::string identifier = generateKeyIdentifier(contextId, keyId);
        

        auto it = keyMap.find(identifier);
        if (it == keyMap.end()) {
            return 0;
        }

        return it->second->lastPressTime.load(std::memory_order_acquire);
    }
    catch (const std::exception& e) {
        std::cerr << "Error in getLastPressTime: " << e.what() << std::endl;
        return 0;
    }
}

void KeyDetector::detectionThreadFunc() {
    

    // 如果没有注册按键，不需要处理
    if (keys.empty()) {
        return;
    }

    // 遍历所有按键
    for (auto& keyInfo : keys) {
        if (!keyInfo || !keyInfo->enabled || keyInfo->keyCode == 0) {
            continue;
        }

        // 更新状态
        bool lastState = keyInfo->currentState.load(std::memory_order_acquire);
        keyInfo->lastState.store(lastState, std::memory_order_release);

        bool currentState = false;
        try {
            currentState = detectKeyState(keyInfo->keyCode);
            keyInfo->currentState.store(currentState, std::memory_order_release);

            // 检测触发
            if (currentState && !lastState) {
                keyInfo->triggered.store(true, std::memory_order_release);
                keyInfo->lastPressTime.store(getCurrentTimeMs(), std::memory_order_release);
            }
        }
        catch (...) {
            // 跳过出错的按键
        }
    }
}

std::string KeyDetector::generateKeyIdentifier(const std::string& contextId, const std::string& keyId) const {
    return contextId + "_" + keyId;
}

bool KeyDetector::detectKeyState(int keyCode) {
    try {
        int mode = detectionMode.load(std::memory_order_acquire);

        switch (mode) {
        case 0: return detectWinAPIKeyState(keyCode);
        case 1: return detectCustom1KeyState(keyCode);
        case 2: return detectCustom2KeyState(keyCode);
        default: return detectWinAPIKeyState(keyCode);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error detecting key state: " << e.what() << std::endl;
        return false;
    }
}

bool KeyDetector::detectWinAPIKeyState(int keyCode) {
    try {
        return (GetAsyncKeyState(keyCode) & 0x8000) != 0;
    }
    catch (...) {
        return false;
    }
}

bool KeyDetector::detectCustom1KeyState(int keyCode) {
    try {

        if (mem.GetKeyboard()->IsKeyDown(keyCode) || detectWinAPIKeyState(keyCode))
            return true;
        int inputKey = virkey_to_inputsystem(keyCode);
        if (inputKey != 0) {
            return mem.isPressed(inputKey); 
        }
        return false;
    }
    catch (...) {
        return detectWinAPIKeyState(keyCode); // 出错时回退到WinAPI
    }
}

bool KeyDetector::detectCustom2KeyState(int keyCode) {
    try {
        // 添加安全检查
        if (!mem.GetKeyboard()) {
            return detectWinAPIKeyState(keyCode); // 回退到WinAPI
        }

        return mem.GetKeyboard()->IsKeyDown(keyCode) || detectWinAPIKeyState(keyCode);
    }
    catch (...) {
        return detectWinAPIKeyState(keyCode); // 出错时回退到WinAPI
    }
}

uint64_t KeyDetector::getCurrentTimeMs() const {
    try {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    catch (...) {
        return 0;
    }
}