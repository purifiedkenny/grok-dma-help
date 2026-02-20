#pragma once
#include <string>
#include <Kmbox/KmboxB.h>
#include <Kmbox/KmboxNet.hpp>

class KmboxController
{
public:
    KmboxController() {};

    bool rememberKmboxInfo = false;
    const std::string CONFIG_FILE = "kmbox_config.json";
    bool SaveKmboxConfig();
    // 加载配置文件
    bool LoadKmboxConfig();
    // 获取和设置是否记住配置
    bool GetRememberConfig() const { return rememberKmboxInfo; }
    void SetRememberConfig(bool remember) { rememberKmboxInfo = remember; }
    
    struct KMBoxConfig {
        std::string type = "Net";
        std::string comPort = "4";
        std::string baudRate = "115200";
        std::string ip = "192.168.2.188";
        std::string port = "9742";
        std::string uuid;
        int minDelay{ 1 };
        bool initialized{ false };
        _com _comPort; // 修正为ComPort类型
    } kmboxConfig;
    
    void InitializeFirst();
    void Initialize();
    void renderKmboxSettings();

    //Tools

    void Move(int x, int y);
    void LeftClick();
};