#include "include.h"
#include "KmBox.h"
#include <random>

//#define KMBXBOX_B_API __declspec(dllimport)
//KMBXBOX_B_API std::string find_port(const std::string& targetDescription);
//KMBXBOX_B_API bool open_port(HANDLE& hSerial, const char* portName, DWORD baudRate);
//KMBXBOX_B_API void send_command(HANDLE hSerial, const std::string& command);
//
//class KMBXBOX_B_API KmBoxBManager {
//private:
//    HANDLE hSerial;
//
//public:
//
//    int init();
//    void km_move(int X, int Y);
//    int km_getpos(int& x, int& y);
//    void km_move_auto(int X, int Y, int runtime);
//    void km_click();
//    bool km_right(bool down);
//    void lock_mx();
//    void lock_my();
//    void lock_mr();
//    void unlock_mx();
//    void unlock_mr();
//    void unlock_my();
//
//
//    HANDLE getSerialHandle() const;
//
//};
//
//KMBXBOX_B_API KmBoxBManager kmBoxBMgr;

void KmboxController::InitializeFirst() {
    // 尝试加载配置
    if (LoadKmboxConfig()) {
        SetRememberConfig(true);
    }

    // 初始化设备
    Initialize();
}

void KmboxController::Initialize() {
    if (kmboxConfig.type == "Net") {
        std::cout << "[#] Init Kmbox Net..." << std::endl;
        kmboxConfig.minDelay = 1;

        auto start = std::chrono::steady_clock::now();
        char ipCopy[256] = { 0 };
        char portCopy[256] = { 0 };
        char uuidCopy[256] = { 0 };

        strcpy(ipCopy, kmboxConfig.ip.c_str());
        strcpy(portCopy, kmboxConfig.port.c_str());
        strcpy(uuidCopy, kmboxConfig.uuid.c_str());

        int result = kmNet_init(ipCopy, portCopy, uuidCopy);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        if (result == 0 && elapsed.count() < 5.0) {
            std::cout << "[#] Kmbox Net init success" << std::endl;
            kmboxConfig.initialized = true;
            if (GetRememberConfig()) {
                SaveKmboxConfig();
            }
        }
        else {
            std::cout << "[#] Kmbox Net init failed or timeout" << std::endl;
            kmboxConfig.initialized = false;
        }
    }
    else if (kmboxConfig.type == "BPro") {
        std::cout << "[#] Init Kmbox B+ Pro..." << std::endl;
        kmboxConfig.minDelay = 3;

        auto start = std::chrono::steady_clock::now();
        bool success = kmboxConfig._comPort.open(std::atoi(kmboxConfig.comPort.c_str()), std::atoi(kmboxConfig.baudRate.c_str()));
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        if (success && elapsed.count() < 5.0) {
            std::cout << "[#] Kmbox B+ Pro init success" << std::endl;
            kmboxConfig.initialized = true;
            if (GetRememberConfig()) {
                SaveKmboxConfig();
            }
        }
        else {
            std::cout << "[#] Kmbox B+ Pro init failed or timeout" << std::endl;
            kmboxConfig.initialized = false;
        }
    }
   /* else if (kmboxConfig.type == "MAKCU")
    {
        int kmResult = kmBoxBMgr.init();

        switch (kmResult) {
        case -1:
            std::cout << "MAKCU Failed!" << std::endl;
            return;
        case -2:
            std::cout << "MAKCU Port Not Open!" << std::endl;
            return;
        case 0:
            break;
        default:
            std::cout << "Initializing..." << std::endl;
            return;
        }

        HANDLE hSerial = kmBoxBMgr.getSerialHandle();
        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cout << "[#] MAKCU Failed!" << std::endl;
            kmboxConfig.initialized = false;
            return;
        }

        std::string MAKCUCHECK = "km.version()\r";
        send_command(hSerial, MAKCUCHECK);

        char buffer[256];
        DWORD bytesRead;
        std::string response;

        while (true) {
            if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                response += buffer;

                if (response.find(">>>") != std::string::npos) {
                    break;
                }
            }
            else {
                break;
            }
        }


        if (response.empty()) {
            std::cout << "[#] MAKCU Failed!" << std::endl;
            kmboxConfig.initialized = false;
            return;
        }
        else if (response.find("km.MAKCU") != std::string::npos) {
            std::cout << "[#] MAKCU Connected!" << std::endl;
            kmboxConfig.initialized = true;
            return;
        }
    }*/
}

void KmboxController::renderKmboxSettings() {
    // 标题
    ImGui::Text("%s", t_("KMBOX SETTINGS"));
    ImGui::Separator();
    ImGui::Spacing();

    // 设备类型选择
    const char* items[] = { "BPro", "Net","MAKCU" };
    const char* itemsShow[] = { "BPro", "Net","MAKCU"};
    int currentItem = (kmboxConfig.type == "BPro") ? 0 : (kmboxConfig.type == "MAKCU" ? 2 : 1);

    if (ImGui::Combo(t_("DeviceType"), &currentItem, itemsShow, IM_ARRAYSIZE(itemsShow))) {
        kmboxConfig.type = items[currentItem];
    }

    char tempBuffer[256] = { 0 };

    if (kmboxConfig.type == "BPro") {
        // 复制当前值到临时缓冲区
        strcpy(tempBuffer, kmboxConfig.comPort.c_str());
        if (ImGui::InputText(t_("ComPort"), tempBuffer, 256)) {
            kmboxConfig.comPort = tempBuffer;
        }

        memset(tempBuffer, 0, sizeof(tempBuffer));
        strcpy(tempBuffer, kmboxConfig.baudRate.c_str());
        if (ImGui::InputText(t_("BaudRate"), tempBuffer, 256)) {
            kmboxConfig.baudRate = tempBuffer;
        }
    }
    else if (kmboxConfig.type == "Net") {
        // Net设置
        memset(tempBuffer, 0, sizeof(tempBuffer));
        strcpy(tempBuffer, kmboxConfig.ip.c_str());
        if (ImGui::InputText(t_("KmboxIP"), tempBuffer, 256)) {
            kmboxConfig.ip = tempBuffer;
        }

        memset(tempBuffer, 0, sizeof(tempBuffer));
        strcpy(tempBuffer, kmboxConfig.port.c_str());
        if (ImGui::InputText(t_("KmboxPort"), tempBuffer, 256)) {
            kmboxConfig.port = tempBuffer;
        }

        memset(tempBuffer, 0, sizeof(tempBuffer));
        strcpy(tempBuffer, kmboxConfig.uuid.c_str());
        if (ImGui::InputText(t_("KmboxUUID"), tempBuffer, 256)) {
            kmboxConfig.uuid = tempBuffer;
        }
    }
    else if (kmboxConfig.type == "MAKCU")
    {
        ImGui::Text("Click Init below");
    }
    ImGui::Spacing();

    // 添加记住配置选项
    bool remember = GetRememberConfig();
    if (ImGui::Checkbox(t_("RememberKmboxInfo"), &remember)) {
        SetRememberConfig(remember);
    }

    ImGui::Spacing();

    // 初始化按钮
    if (ImGui::Button(t_("Initialize"))) {
        Initialize();
    }

    // 状态显示
    if (kmboxConfig.initialized) {
        ImGui::TextColored(ImVec4(0, 1, 0, 0.5), "%s", t_("Success"));
    }
    else {
        ImGui::TextColored(ImVec4(1, 0, 0, 0.5), "%s", t_("Failed or Timeout"));
    }
}

bool KmboxController::SaveKmboxConfig() {
    if (!rememberKmboxInfo) return false;

    try {
        json config;
        // 保存设备类型
        config["type"] = (kmboxConfig.type == "BPro") ? 0 : 1;

        // 根据设备类型保存对应配置
        if (kmboxConfig.type == "BPro") {
            config["comPort"] = kmboxConfig.comPort;
            config["baudRate"] = kmboxConfig.baudRate;
        }
        else {
            config["ip"] = kmboxConfig.ip;
            config["port"] = kmboxConfig.port;
            config["uuid"] = kmboxConfig.uuid;
        }

        std::ofstream file(CONFIG_FILE);
        if (!file.is_open()) return false;

        file << config.dump(4);
        file.close();
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool KmboxController::LoadKmboxConfig() {
    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) return false;

    try {
        json config = json::parse(file);

        // 设置设备类型
        int type = config["type"].get<int>();
        kmboxConfig.type = (type == 0) ? "BPro" : "Net";

        if (type == 0) { // BPro
            // 加载BPro配置
            kmboxConfig.comPort = config["comPort"].get<std::string>();
            kmboxConfig.baudRate = config["baudRate"].get<std::string>();
        }
        else { // Net
            // 加载Net配置
            kmboxConfig.ip = config["ip"].get<std::string>();
            kmboxConfig.port = config["port"].get<std::string>();
            kmboxConfig.uuid = config["uuid"].get<std::string>();
        }

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

void KmboxController::Move(int x, int y) {
    if (kmboxConfig.type == "Net") {
        kmNet_mouse_move(static_cast<short>(x), static_cast<short>(y));
    }
    else if (kmboxConfig.type == "BPro") {
        char cmd[1024];
        sprintf_s(cmd, "km.move(%d, %d)\r\n", x, y);
        kmboxConfig._comPort.write(cmd);
    }
    else if (kmboxConfig.type == "MAKCU")
    {
        //kmBoxBMgr.km_move(x, y);
    }
}

float RandomFloat() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

float RandomRange(float min, float max) {
    if (min > max) {
        std::swap(min, max);
    }
    return RandomFloat() * (max - min) + min;
}

void KmboxController::LeftClick() {
    if (aimbot->aimParams.aimType == 0) {
        mem.Write<int>(mem.OFF_BASE + OFF_INATTACK + 0x8, 5);
        Sleep(static_cast<int>(RandomRange(static_cast<float>(kmboxConfig.minDelay), 10)));
        mem.Write<int>(mem.OFF_BASE + OFF_INATTACK + 0x8, 4);
    }
    else {
        if (kmboxConfig.type == "Net") {
            kmNet_mouse_left(1);
            Sleep(static_cast<int>(RandomRange(static_cast<float>(kmboxConfig.minDelay), 10)));
            kmNet_mouse_left(0);
        }
        else if (kmboxConfig.type == "BPro") {
            char cmd[1024] = "km.click(0)\r\n";
            Sleep(static_cast<int>(RandomRange(static_cast<float>(kmboxConfig.minDelay), 10)));
            kmboxConfig._comPort.write(cmd);
        }
    }
}