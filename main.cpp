#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <functional>

#include "include.h"
#include "Menu.h"
#include "Updater.h"
#include "KeyCheck.h"

#include "Esp.h"
#include "Aimbot.h"
#include "Misc.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>
#include "TriggerBot.h"


#include <DbgHelp.h>
#include "KeyDetector.h"
#include "Skeleton.h"
#include "skStr.h"
#include "Webradar.h"
#pragma comment(lib, "dbghelp.lib")




std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);



size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}


std::string ReadFromJson(std::string path, std::string section)
{
    if (!std::filesystem::exists(path))
        return skCrypt("File Not Found").decrypt();
    std::ifstream file(path);
    json data = json::parse(file);
    return data[section];
}





// 全局变量
std::atomic<bool> g_running{ true }; // 控制线程运行
std::vector<std::thread> g_threads; // 存储所有线程

// 线程包装函数，增加异常处理和正常结束功能
template<typename Func>
void ThreadWrapper(const char* name, Func&& func) {
    try {
        //std::cout << "Thread started: " << name << std::endl;

        {
            try {
                func();
            }
            catch (const std::exception& e) {
                std::cerr << "Exception in thread " << name << ": " << e.what() << std::endl;
                // 继续运行，不终止线程
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            catch (...) {
                std::cerr << "Unknown exception in thread " << name << std::endl;
                // 继续运行，不终止线程
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        std::cout << "Thread ended normally: " << name << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal exception in thread " << name << ": " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown fatal exception in thread " << name << std::endl;
    }
}

// 安全创建线程函数
void SafeCreateThread(const char* name, std::function<void()> func) {
    try {
        // 给一些时间让其他系统初始化
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 创建线程并添加到管理容器
        g_threads.emplace_back(ThreadWrapper<std::function<void()>>, name, func);

        // 将线程设为分离状态
        g_threads.back().detach();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create thread " << name << ": " << e.what() << std::endl;
    }
}

// 清理所有线程
void CleanupThreads() {
    std::cout << "Stopping all threads..." << std::endl;

    // 设置停止标志
    g_running = false;

    // 等待线程正常结束
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 清空线程容器
    g_threads.clear();

    std::cout << "All threads stopped" << std::endl;
}

void StartWebRadar() {
    static WebRadar* radar = new WebRadar();
    radar->Start();
}

void MiscThread()
{
    while (true)
    {
        if (!entityManager.isInGame() || !entityManager.isLoggedIn()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        spectator->run();
        misc->run();
        glow->run();
        

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


void printColoredText(int color, const std::string& text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << text;
    SetConsoleTextAttribute(hConsole, 7); // 恢复白色
}
void printLogo(std::string version) {
    const char* logo[] = {
        " ██████╗ ██╗   ██╗███████╗████████╗██╗    ██╗ █████╗ ██████╗ ███████╗",
        "██╔═══██╗██║   ██║██╔════╝╚══██╔══╝██║    ██║██╔══██╗██╔══██╗██╔════╝",
        "██║   ██║██║   ██║███████╗   ██║   ██║ █╗ ██║███████║██████╔╝█████╗  ",
        "██║   ██║██║   ██║╚════██║   ██║   ██║███╗██║██╔══██║██╔══██╗██╔══╝  ",
        "╚██████╔╝╚██████╔╝███████║   ██║   ╚███╔███╔╝██║  ██║██║  ██║███████╗",
        " ╚═════╝  ╚═════╝ ╚══════╝   ╚═╝    ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝"
    };
    // 霓虹渐变配色方案 (Windows 颜色码)
    int colors[] = { 9, 11, 3, 10, 12, 13 }; // 蓝->浅蓝->黄->绿->红->紫

    // 上边框
    printColoredText(9, "\n★≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡★\n");
    // 输出 ASCII 艺术
    for (int i = 0; i < 6; ++i) {
        printColoredText(colors[i], std::string(8, ' '));
        printColoredText(colors[i], logo[i]);
        std::cout << std::endl;
    }
    // 下边框 + 版本信息
    printColoredText(12, "\n≡≡≡ ");
    printColoredText(14, "Xerus Apex Legends Special Edition");
    printColoredText(12, " ≡≡≡≡≡≡≡≡≡≡≡ ");
    printColoredText(11, version);
    printColoredText(12, " ≡≡≡\n\n");
}

bool initializeTargetProcess(bool memmap)
{
	while (!mem.Init("r5apex_dx12.exe", memmap, false))
	{
		std::cout << "Failed to initialize memory read. Retrying...\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	Beep(500, 500);
	return true;
}

std::string getLanguageName(Translation::Language lang) {
    switch (lang) {
    case Translation::Language::English: return "English";
    case Translation::Language::SimplifiedChinese: return "简体中文";
    case Translation::Language::TraditionalChinese: return "繁體中文";
    case Translation::Language::Japanese: return "日本語";
    default: return "未知语言";
    }
}

Translation::Language checkAndSetDefaultLanguage() {
    const std::string configFile = "defaultLanguage.cfg";
    Translation::Language defaultLanguage = Translation::Language::English; // 默认为英文
    bool fileValid = false;

    // 检查配置文件是否存在
    if (fs::exists(configFile)) {
        std::ifstream file(configFile);
        if (file.is_open()) {
            std::string line;
            if (std::getline(file, line)) {
                try {
                    // 尝试将文件内容转换为整数
                    int langCode = std::stoi(line);
                    // 验证语言代码是否有效
                    if (langCode >= 0 && langCode <= 3) {
                        defaultLanguage = static_cast<Translation::Language>(langCode);
                        fileValid = true;
                        std::cout << "已从配置文件加载默认语言: "
                            << getLanguageName(defaultLanguage) << std::endl;
                    }
                }
                catch (const std::exception& e) {
                    // 转换失败，文件格式不正确
                }
            }
            file.close();
        }
    }
    if (!fileValid) {
       
        std::cout << "No Default Language Found,select here:" << std::endl;
        std::cout << "0 - English" << std::endl;
        std::cout << "1 - 简体中文" << std::endl;
        std::cout << "2 - 繁體中文" << std::endl;
        std::cout << "3 - 日本語" << std::endl;

        int userChoice = -1;
        while (userChoice < 0 || userChoice > 3) {
            std::cout << "Enter option (0-3): ";
            std::cin >> userChoice;

            if (std::cin.fail() || userChoice < 0 || userChoice > 3) {
                std::cin.clear(); // 清除错误状态
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略无效输入
                std::cout << "Not valid,please retry." << std::endl;
                std::cout << "无效输入，请重新输入." << std::endl;
                userChoice = -1;
            }
        }

        defaultLanguage = static_cast<Translation::Language> (userChoice);

        // 将用户选择保存到配置文件
        std::ofstream outFile(configFile);
        if (outFile.is_open()) {
            outFile << userChoice;
            outFile.close();
            std::cout << "Saved Default Language." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            std::cerr << "Cant save default language." << std::endl;
        }
    }

    return defaultLanguage;
}



//web menu bullshit




// 游戏主逻辑函数
void GameMainLogic() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try {
        Translation::Language currentLanguage = checkAndSetDefaultLanguage();

        switch (currentLanguage) {
        case Translation::English:
            std::cout << "Welcome!" << std::endl;
            Translation::setLanguage(currentLanguage);
            break;
        case Translation::SimplifiedChinese:
            std::cout << "欢迎使用！" << std::endl;
            Translation::setLanguage(currentLanguage);
            break;
        case Translation::TraditionalChinese:
            std::cout << "歡迎使用！" << std::endl;
            Translation::setLanguage(currentLanguage);
            break;
        case Translation::Japanese:
            std::cout << "私たちのアプリケーションへようこそ！" << std::endl;
            Translation::setLanguage(currentLanguage);
            break;
        }

        Sleep(2000);
        system("cls");



        g_running = true;
        Translation::init();
        entityManager.initialize();
        
        AutoUpdater updater;
        if (updater.checkAndUpdate()) {
            return;
        }
        
       // printLogo(updater.getVersion());
        ConfigManager::initialize();
       
        // 初始化键盘
        try {
            mem.GetKeyboard()->InitKeyboard();
            std::cout << "Keyboard initialized" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Keyboard initialization error: " << e.what() << std::endl;
        }
        entityManager.setGameInput(true);

        // 等待Apex进程
        bool memmap = true;
        std::string input;

        std::cout << "some 75t cards may not support mem mapping. I would try ->y<- first. " << std::endl;
        std::cout << "if it fails just do it manually. Staff can help if needed. " << std::endl;
        std::cout << "memory mapping?(Enter->y or y/n): ";
        std::getline(std::cin, input);

        if (input == "y" || input == "Y") {
            memmap = true;
        }
        else if (input == "n" || input == "N") {
            memmap = false;
        }
        else {
            std::cout << "Invalid input. Defaulting to true.\n";
        }

        /*if (!initializeTargetProcess(memmap)) {
            return;
        }*/

        // 依次创建线程，使用包装函数增强稳定性
        SafeCreateThread("MainScatter", std::bind(&EntityManager::mainScatterThread, &entityManager));
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 给主散射器线程时间初始化

        SafeCreateThread("EntityScatter", std::bind(&EntityManager::entityScatterThread, &entityManager));
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 给实体散射器线程时间初始化

        SafeCreateThread("PlayerScatter", std::bind(&EntityManager::playerScatterThread, &entityManager));
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 确保玩家散射器正确启动

        SafeCreateThread("ItemScatter", std::bind(&EntityManager::itemShieldScatterThread, &entityManager));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SafeCreateThread("AimThread", std::bind(&Aimbot::run, aimbot));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SafeCreateThread("SkeletonThread", std::bind(&SkeletonSystem::UpdateThread, skeletonSystem));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SafeCreateThread("MiscThread", MiscThread);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SafeCreateThread("KeyCheckThread", KeyCheck::run);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SafeCreateThread("W2SThread", std::bind(&ItemScreenProcessor::ProcessingThreadFunction, itemProcessor));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SafeCreateThread("MenuThread", Menu::run);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        kmbox->InitializeFirst();
        StartWebRadar();

        /*SafeCreateThread("MenuThread", Menu::run);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));*/

        while (true) {
            Sleep(2000);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in GameMainLogic: " << e.what() << std::endl;
        MessageBoxA(NULL, e.what(), "GameMainLogic Error", MB_OK | MB_ICONERROR);
    }
    catch (...) {
        std::cerr << "Unknown exception in GameMainLogic" << std::endl;
        MessageBoxA(NULL, "GameMainLogic Unknown Error", "错误", MB_OK | MB_ICONERROR);
    }

    // 确保清理所有线程
    CleanupThreads();
}

std::string GetDetailedCallStack(CONTEXT* context)
{
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    // 初始化符号处理
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
    if (!SymInitialize(process, NULL, TRUE)) {
        return "Failed to initialize symbol handler";
    }

    STACKFRAME64 stack = {};
    stack.AddrPC.Offset = context->Rip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = context->Rbp;
    stack.AddrFrame.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Rsp;
    stack.AddrStack.Mode = AddrModeFlat;

    std::stringstream callstack;
    callstack << "Crash Call Stack:\n\n";

    for (int frameNum = 0; frameNum < 25; frameNum++) // 限制栈帧数量
    {
        // 获取下一个栈帧
        if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64,
            process,
            thread,
            &stack,
            context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL))
        {
            break;
        }

        if (stack.AddrPC.Offset == 0) {
            break;
        }

        // 获取符号信息
        char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbol_buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        DWORD64 displacement = 0;
        if (SymFromAddr(process, stack.AddrPC.Offset, &displacement, symbol))
        {
            callstack << "Function: " << symbol->Name << "\n";

            // 获取行号信息
            IMAGEHLP_LINE64 line = { sizeof(IMAGEHLP_LINE64) };
            DWORD lineDisplacement;
            if (SymGetLineFromAddr64(process, stack.AddrPC.Offset, &lineDisplacement, &line))
            {
                callstack << "File: " << line.FileName << "\n";
                callstack << "Line: " << line.LineNumber << "\n";
            }

            callstack << "Address: 0x" << std::hex << stack.AddrPC.Offset << std::dec << "\n\n";
        }
        else
        {
            callstack << "Failed to get symbol. Error: " << GetLastError() << "\n";
        }
    }

    SymCleanup(process);
    return callstack.str();
}

LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* ex)
{
    try {
        // 获取异常的基本信息
        std::stringstream error_info;
        error_info << "Crashed!\n\n";
        error_info << "Code: 0x" << std::hex << ex->ExceptionRecord->ExceptionCode << std::dec << "\n";
        error_info << "Address: 0x" << std::hex << (uintptr_t)ex->ExceptionRecord->ExceptionAddress << std::dec << "\n\n";

        // 获取详细的调用栈
        std::string callstack = GetDetailedCallStack(ex->ContextRecord);
        error_info << callstack;

        // 显示错误信息
        MessageBoxA(NULL, error_info.str().c_str(), "Crashed", MB_OK | MB_ICONERROR);
    }
    catch (...) {
        MessageBoxA(NULL, "Error on getting crash information", "Error", MB_OK | MB_ICONERROR);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}


// Windows应用程序入口点
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    SetUnhandledExceptionFilter(UnhandledExceptionHandler);

    // 分配控制台窗口
    if (AllocConsole()) {
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);  // 重定向标准输出
        freopen_s(&dummy, "CONOUT$", "w", stderr);  // 重定向标准错误
        freopen_s(&dummy, "CONIN$", "r", stdin);    // 重定向标准输入

        // 设置控制台窗口标题
        SetConsoleTitle(TEXT("Xerus-ApexLegends"));

        // 设置控制台窗口位置到右侧（不遮挡游戏窗口）
        HWND consoleHwnd = GetConsoleWindow();
        if (consoleHwnd) {
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);

            RECT consoleRect;
            GetWindowRect(consoleHwnd, &consoleRect);
            int consoleWidth = consoleRect.right - consoleRect.left;
            int consoleHeight = consoleRect.bottom - consoleRect.top;

            // 将控制台窗口移动到屏幕右侧
            MoveWindow(consoleHwnd,
                screenWidth - consoleWidth,
                0,
                consoleWidth,
                consoleHeight,
                TRUE);
        }
    }

    // 运行游戏主逻辑
    GameMainLogic();

    // 释放控制台
    FreeConsole();

    return 0;
}