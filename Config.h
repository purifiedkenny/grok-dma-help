#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <filesystem>
#include <functional>
#include <nlohmann/json.hpp>
#include <imgui.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

// ImVec4 序列化支持
namespace nlohmann {
    template <>
    struct adl_serializer<ImVec4> {
        static void to_json(json& j, const ImVec4& vec) {
            j = json{
                {"x", vec.x},
                {"y", vec.y},
                {"z", vec.z},
                {"w", vec.w}
            };
        }

        static void from_json(const json& j, ImVec4& vec) {
            vec.x = j.value("x", 0.0f);
            vec.y = j.value("y", 0.0f);
            vec.z = j.value("z", 0.0f);
            vec.w = j.value("w", 0.0f);
        }
    };
}

// 前向声明
class AutoConfigurable;
inline std::string currentDefault;
//=============================================================================
// 配置管理器类
//=============================================================================
class ConfigManager {
private:
    json configData;
    std::string configFilePath;
    bool configLoaded = false;

    std::vector<AutoConfigurable*> components;

    ConfigManager() {
        ensureConfigDirectoryExists();
    }

    void ensureConfigDirectoryExists() {
        try {
            if (!fs::exists(configDirectory)) {
                fs::create_directories(configDirectory);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create config directory: " << e.what() << std::endl;
        }
    }

public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    ConfigManager(const ConfigManager&) = delete;
    void operator=(const ConfigManager&) = delete;
    std::string configDirectory = "configs/";
    std::string defaultConfigFile = "defaultCFG.json";
    // 静态方法声明
    static bool initialize();
    static bool deleteConfigFile(const std::string& filename);
    static bool loadFromFile(const std::string& filePath = "");
    static bool saveToFile(const std::string& filePath = "");
    static json getSection(const std::string& section);
    static void updateSection(const std::string& section, const json& sectionData);
    static void printConfig();
    static void registerComponent(AutoConfigurable* component);
    static const std::vector<AutoConfigurable*>& getComponents();
    static bool saveAllConfigs();
    static bool loadAllConfigs();
    static bool saveAsDefault(const std::string& currentConfigName);
    static bool loadDefaultConfig();
    static bool setConfigAsDefault(const std::string& configName);
    static std::vector<std::string> getAvailableConfigs();
    static bool saveConfigToFile(const std::string& filename);
    static bool loadConfigFromFile(const std::string& filename);
    static void setConfigDirectory(const std::string& directory);
    static std::string getConfigDirectory();
};

//=============================================================================
// 可自动配置类
//=============================================================================
class AutoConfigurable {
private:
    std::string configSection;

    // 用于存储参数的信息
    struct ParamInfo {
        void* valuePtr;
        std::function<void(const json&)> loadFunc;
        std::function<json()> saveFunc;
    };

    std::unordered_map<std::string, ParamInfo> params;

protected:
    // 构造函数 (接受配置部分名称)
    explicit AutoConfigurable(const std::string& section) : configSection(section) {
        // 注册到配置管理器
        ConfigManager::registerComponent(this);
    }

    // 注册参数模板函数 - 完整定义在头文件中
    template<typename T>
    void registerParam(const std::string& name, T& value) {
        ParamInfo info;
        info.valuePtr = &value;

        // 加载函数
        info.loadFunc = [&value, name](const json& j) {
            if (j.contains(name) && !j[name].is_null()) {
                try {
                    value = j[name].get<T>();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error loading parameter " << name
                        << ": " << e.what() << std::endl;
                }
            }
        };

        // 保存函数
        info.saveFunc = [&value, name]() -> json {
            return json{ {name, value} };
            };

        params[name] = info;
    }

    void registerParam(const std::string name, ImVec4& clr)
    {
        registerParam(name + "R", clr.x);
        registerParam(name + "G", clr.y);
        registerParam(name + "B", clr.z);
        registerParam(name + "A", clr.w);
    }

    // 注册参数的重载版本（带范围的float参数）
    void registerParam(const std::string& name, float& value, float min, float max, float step = 0.1f) {
        registerParam(name, value); // 调用基本版本
    }

    // 注册参数的重载版本（带范围的int参数）
    void registerParam(const std::string& name, int& value, int min, int max, int step = 1) {
        registerParam(name, value); // 调用基本版本
    }

    // 注册枚举参数
    void registerEnumParam(const std::string& name, int& value, const char** enumValues, int enumCount) {
        registerParam(name, value); // 调用基本版本
    }

public:
    // 禁止拷贝
    AutoConfigurable(const AutoConfigurable&) = delete;
    AutoConfigurable& operator=(const AutoConfigurable&) = delete;

    virtual ~AutoConfigurable() = default;

    // 加载配置
    void loadConfig() {
        // 获取当前部分的配置
        json sectionData = ConfigManager::getSection(configSection);

        // 应用到所有参数
        for (auto& pair : params) {
            auto& info = pair.second;
            info.loadFunc(sectionData);
        }
    }

    // 保存配置
    void saveConfig() {
        // 获取现有部分的配置
        json sectionData = ConfigManager::getSection(configSection);

        // 更新数据
        for (auto& pair : params) {
            auto& name = pair.first;
            auto& info = pair.second;
            json paramData = info.saveFunc();
            for (auto it = paramData.begin(); it != paramData.end(); ++it) {
                sectionData[it.key()] = it.value();
            }
        }

        // 更新部分
        ConfigManager::updateSection(configSection, sectionData);

        // 保存到文件
        //ConfigManager::saveToFile();
    }

    // 获取配置部分名称
    std::string getConfigSection() const {
        return configSection;
    }
};

//=============================================================================
// 配置管理器静态方法实现
//=============================================================================

// 初始化并从文件加载配置
inline bool ConfigManager::initialize() {
    auto& instance = getInstance();
    // 否则，尝试加载默认配置，但不创建新文件
    if (fs::exists(instance.configDirectory + instance.defaultConfigFile)) {
        return loadDefaultConfig();
    }

    // 如果没有默认配置，只初始化内存中的配置，不创建文件
    instance.configData = json::object();
    instance.configLoaded = true;
    return true;
}

inline bool ConfigManager::deleteConfigFile(const std::string& filename) {
    auto& instance = getInstance();
    std::string configPath = instance.configDirectory + filename + ".json";

    // 检查文件是否存在
    if (!fs::exists(configPath)) {
        std::cerr << "Config file does not exist: " << configPath << std::endl;
        return false;
    }

    try {
        // 删除文件
        fs::remove(configPath);
        std::cout << "Deleted config file: " << configPath << std::endl;

        // 检查是否为默认配置，如果是则清除默认配置引用
        std::string defaultConfigPath = instance.configDirectory + instance.defaultConfigFile;
        if (fs::exists(defaultConfigPath)) {
            try {
                std::ifstream file(defaultConfigPath);
                if (file.is_open()) {
                    json defaultConfig;
                    file >> defaultConfig;
                    file.close();

                    // 检查是否有任何组件引用了被删除的配置
                    bool isDefault = false;
                    for (auto* component : instance.components) {
                        if (component && defaultConfig.contains(component->getConfigSection()) &&
                            defaultConfig[component->getConfigSection()].get<std::string>() == filename) {
                            isDefault = true;
                            break;
                        }
                    }

                    // 如果是默认配置，则清除引用
                    if (isDefault) {
                        json emptyConfig;
                        std::ofstream outFile(defaultConfigPath);
                        if (outFile.is_open()) {
                            outFile << emptyConfig.dump(4);
                            std::cout << "Cleared default config reference to deleted file" << std::endl;
                        }
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error checking default config: " << e.what() << std::endl;
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error deleting config file: " << e.what() << std::endl;
        return false;
    }
}

// 从文件加载配置
inline bool ConfigManager::loadFromFile(const std::string& filePath) {
    auto& instance = getInstance();

    // 如果提供了新路径，则更新
    if (!filePath.empty()) {
        instance.configFilePath = filePath;
    }

    // 确保我们有一个文件路径
    if (instance.configFilePath.empty()) {
        std::cerr << "Error: No config file path specified" << std::endl;
        return false;
    }

    try {
        std::ifstream file(instance.configFilePath);
        if (file.is_open()) {
            file >> instance.configData;
            instance.configLoaded = true;
            return true;
        }
        else {
            // 如果文件不存在，创建一个空的配置
            instance.configData = json::object();
            instance.configLoaded = true;
            return true;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

// 保存配置到文件
inline bool ConfigManager::saveToFile(const std::string& filePath) {
    auto& instance = getInstance();

    // 如果提供了新路径，则更新
    if (!filePath.empty()) {
        instance.configFilePath = filePath;
    }

    // 确保我们有一个文件路径，如果没有就不保存
    if (instance.configFilePath.empty()) {
        std::cerr << "Error: No config file path specified" << std::endl;
        return false;
    }

    try {
        std::ofstream file(instance.configFilePath);
        if (file.is_open()) {
            file << instance.configData.dump(4); // 美化输出
            return true;
        }
        else {
            std::cerr << "Error: Could not open file for writing: "
                << instance.configFilePath << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

// 获取某个部分的配置
inline json ConfigManager::getSection(const std::string& section) {
    auto& instance = getInstance();

    if (!instance.configLoaded) {
        if (!instance.loadFromFile()) {
            // 如果加载失败，返回空对象
            return json::object();
        }
    }

    // 如果部分不存在，创建它
    if (!instance.configData.contains(section)) {
        instance.configData[section] = json::object();
    }

    return instance.configData[section];
}

// 更新某个部分的配置
inline void ConfigManager::updateSection(const std::string& section, const json& sectionData) {
    auto& instance = getInstance();

    if (!instance.configLoaded) {
        if (!instance.loadFromFile()) {
            // 如果加载失败，创建一个新的配置
            instance.configData = json::object();
            instance.configLoaded = true;
        }
    }

    instance.configData[section] = sectionData;
}

// 打印配置内容 (调试用)
inline void ConfigManager::printConfig() {
    auto& instance = getInstance();

    if (instance.configLoaded) {
        std::cout << "Current config:" << std::endl;
        std::cout << instance.configData.dump(4) << std::endl;
    }
    else {
        std::cout << "Config not loaded yet" << std::endl;
    }
}

// 注册组件
inline void ConfigManager::registerComponent(AutoConfigurable* component) {
    if (component) {
        auto& instance = getInstance();
        // 检查组件是否已注册
        for (auto* comp : instance.components) {
            if (comp == component) return;
        }
        instance.components.push_back(component);
    }
}

// 获取所有已注册组件
inline const std::vector<AutoConfigurable*>& ConfigManager::getComponents() {
    auto& instance = getInstance();
    return instance.components;
}

// 保存所有组件配置
inline bool ConfigManager::saveAllConfigs() {
    auto& instance = getInstance();
    bool success = true;

    for (auto* component : instance.components) {
        if (component) {
            try {
                // 注意：这里调用组件的 saveConfig，但它已被修改为不自动保存到文件
                component->saveConfig();
            }
            catch (const std::exception& e) {
                std::cerr << "Error saving config for component "
                    << component->getConfigSection()
                    << ": " << e.what() << std::endl;
                success = false;
            }
        }
    }

    return success;
}

// 加载所有组件配置
inline bool ConfigManager::loadAllConfigs() {
    auto& instance = getInstance();
    bool success = true;

    for (auto* component : instance.components) {
        if (component) {
            try {
                component->loadConfig();
            }
            catch (const std::exception& e) {
                std::cerr << "Error loading config for component "
                    << component->getConfigSection()
                    << ": " << e.what() << std::endl;
                success = false;
            }
        }
    }

    return success;
}

inline bool ConfigManager::saveAsDefault(const std::string& currentConfigName) {
    // 必须提供当前配置名称
    if (currentConfigName.empty()) {
        std::cerr << "Cannot set default: current config name is empty" << std::endl;
        return false;
    }

    return setConfigAsDefault(currentConfigName);
}


inline bool ConfigManager::loadDefaultConfig() {
    auto& instance = getInstance();
    std::string defaultConfigPath = instance.configDirectory + instance.defaultConfigFile;

    // 检查默认配置文件是否存在
    if (!fs::exists(defaultConfigPath)) {
        std::cerr << "Default config file does not exist" << std::endl;
        return false;
    }

    try {
        // 读取默认配置引用文件
        std::ifstream file(defaultConfigPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open default config file" << std::endl;
            return false;
        }

        json defaultConfig;
        file >> defaultConfig;

        std::string configName;
        if (!instance.components.empty() && instance.components[0] != nullptr) {
            auto* firstComponent = instance.components[0];
            if (defaultConfig.contains(firstComponent->getConfigSection())) {
                configName = defaultConfig[firstComponent->getConfigSection()].get<std::string>();
            }
        }

        // 如果没有找到配置名，检查任何组件
        if (configName.empty()) {
            for (auto* component : instance.components) {
                if (component && defaultConfig.contains(component->getConfigSection())) {
                    configName = defaultConfig[component->getConfigSection()].get<std::string>();
                    break;
                }
            }
        }

        // 如果仍然没有找到配置名，返回失败
        if (configName.empty()) {
            std::cerr << "No valid config name found in default config" << std::endl;
            return false;
        }
        currentDefault = configName;
        // 加载指定的配置文件
        return loadConfigFromFile(configName);
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading default config: " << e.what() << std::endl;
        return false;
    }
}

inline bool ConfigManager::setConfigAsDefault(const std::string& configName) {
    auto& instance = getInstance();

    // 检查配置文件是否存在
    std::string configPath = instance.configDirectory + configName + ".json";
    if (!fs::exists(configPath)) {
        std::cerr << "Config file does not exist: " << configPath << std::endl;
        return false;
    }

    // 创建默认配置引用文件，只包含配置名称
    json defaultConfig;
    for (auto* component : instance.components) {
        if (component) {
            // 所有组件指向同一个配置文件
            defaultConfig[component->getConfigSection()] = configName;
        }
    }

    // 保存默认配置引用
    std::string defaultConfigPath = instance.configDirectory + instance.defaultConfigFile;
    try {
        std::ofstream file(defaultConfigPath);
        if (file.is_open()) {
            file << defaultConfig.dump(4);
            currentDefault = configName;
            return true;
        }
        else {
            std::cerr << "Failed to open default config file for writing" << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving default config: " << e.what() << std::endl;
        return false;
    }
}

inline std::vector<std::string> ConfigManager::getAvailableConfigs() {
    auto& instance = getInstance();
    std::vector<std::string> configs;

    try {
        for (const auto& entry : fs::directory_iterator(instance.configDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                // 排除默认配置文件
                std::string filename = entry.path().filename().string();
                if (filename != instance.defaultConfigFile) {
                    // 移除扩展名
                    size_t lastDot = filename.find_last_of(".");
                    if (lastDot != std::string::npos) {
                        filename = filename.substr(0, lastDot);
                    }
                    configs.push_back(filename);
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading config directory: " << e.what() << std::endl;
    }

    return configs;
}

inline bool ConfigManager::saveConfigToFile(const std::string& filename) {
    auto& instance = getInstance();

    // 首先保存所有组件配置到内存中的configData
    if (!saveAllConfigs()) {
        return false;
    }

    // 直接保存完整的configData到指定文件
    std::string configPath = instance.configDirectory + filename + ".json";
    try {
        std::ofstream file(configPath);
        if (file.is_open()) {
            file << instance.configData.dump(4); // 保存完整配置
            return true;
        }
        else {
            std::cerr << "Failed to open config file for writing: " << configPath << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving config to file: " << e.what() << std::endl;
        return false;
    }
}

inline bool ConfigManager::loadConfigFromFile(const std::string& filename) {
    auto& instance = getInstance();
    std::string configPath = instance.configDirectory + filename + ".json";

    if (!fs::exists(configPath)) {
        std::cerr << "Config file does not exist: " << configPath << std::endl;
        return false;
    }

    try {
        // 读取配置文件
        std::ifstream file(configPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file: " << configPath << std::endl;
            return false;
        }

        // 加载配置到内存
        file >> instance.configData;
        instance.configLoaded = true;

        // 应用配置到各组件
        return loadAllConfigs();
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config from file: " << e.what() << std::endl;
        return false;
    }
}
inline void ConfigManager::setConfigDirectory(const std::string& directory) {
    auto& instance = getInstance();
    instance.configDirectory = directory;
    instance.ensureConfigDirectoryExists();
}

inline std::string ConfigManager::getConfigDirectory() {
    auto& instance = getInstance();
    return instance.configDirectory;
}