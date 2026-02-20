#pragma once
#include <windows.h>
#include <wininet.h>
#include <string>
#include <fstream>
#include <random>
#include <sstream>
#include <filesystem>
#include <urlmon.h>
#include <shellapi.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

using json = nlohmann::json;

class AutoUpdater {
private:
    // 镜像源列表
    const std::vector<std::string> VERSION_CHECK_URLS = {
        "https://gitee.com/keep1337/auto-updater-cloud/raw/master/version.json",
        "https://raw.githubusercontent.com/CNFDAzure/Apex-DMA-Cheat-Dustware/main/version.json",
    };

    const std::vector<std::string> DOWNLOAD_URLS = {
        "https://gitee.com/keep1337/auto-updater-cloud/releases/download/latest/Dust-Apex.vmp.exe",
        "https://github.com/CNFDAzure/Apex-DMA-Cheat-Dustware/releases/latest/download/Dust-Apex.exe"
    };

    const std::string CURRENT_VERSION = "1.1.4";
    std::ofstream logFile;

    void initLog() {
        if (!logFile.is_open()) {
            logFile.open("updater_debug.log", std::ios::app);
        }
    }

    void writeLog(const std::string& message) {
        //initLog();
        //auto now = std::chrono::system_clock::now();
        //auto now_c = std::chrono::system_clock::to_time_t(now);
        //std::stringstream ss;
        //ss << std::put_time(std::localtime(&now_c), "[%Y-%m-%d %H:%M:%S] ");
        //logFile << ss.str() << message << std::endl;
        ////std::cout << ss.str() << message << std::endl;
        //logFile.flush();
    }

    std::string generateRandomName() {
        const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, chars.length() - 1);

        std::string result;
        for (int i = 0; i < 10; i++) {
            result += chars[dis(gen)];
        }
        writeLog("Generated random name: " + result + ".exe");
        return result + ".exe";
    }

    std::string getCurrentPath() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string path(buffer);
        writeLog("Current path: " + path);
        return path;
    }

    std::string getCurrentExeName() {
        std::string path = getCurrentPath();
        std::string name = path.substr(path.find_last_of("\\") + 1);
        writeLog("Current exe name: " + name);
        return name;
    }

    bool downloadFile(const std::string& url, const std::string& filename) {
        writeLog("Attempting to download from: " + url);
        writeLog("Target filename: " + filename);

        HINTERNET hInternet = InternetOpenA("AutoUpdater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            writeLog("InternetOpen failed with error: " + std::to_string(GetLastError()));
            return false;
        }

        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0,
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, 0);
        if (!hConnect) {
            writeLog("InternetOpenUrl failed with error: " + std::to_string(GetLastError()));
            InternetCloseHandle(hInternet);
            return false;
        }

        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            writeLog("Failed to create output file: " + filename);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return false;
        }

        char buffer[8192];
        DWORD bytesRead;
        size_t totalBytes = 0;

        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            outFile.write(buffer, bytesRead);
            totalBytes += bytesRead;
            writeLog("Downloaded " + std::to_string(totalBytes) + " bytes");
        }

        outFile.close();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        // 验证文件
        std::ifstream testFile(filename, std::ios::binary);
        if (!testFile.good()) {
            writeLog("File not found after download");
            return false;
        }

        testFile.seekg(0, std::ios::end);
        size_t fileSize = testFile.tellg();
        testFile.close();

        if (fileSize == 0) {
            writeLog("Downloaded file is empty");
            std::filesystem::remove(filename);
            return false;
        }

        writeLog("Download successful. File size: " + std::to_string(fileSize) + " bytes");
        return true;
    }

    json getVersionInfo() {
        std::string tempFile = "temp_version.json";

        for (const auto& url : VERSION_CHECK_URLS) {
            writeLog("Trying version check URL: " + url);
            try {
                if (downloadFile(url, tempFile)) {
                    std::ifstream file(tempFile);
                    if (file.is_open()) {
                        std::string content;
                        try {
                            content.assign((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
                        }
                        catch (const std::exception& e) {
                            writeLog("Error reading file content: " + std::string(e.what()));
                            MessageBoxA(NULL, ("Error reading file: " + std::string(e.what())).c_str(),
                                "Error", MB_OK | MB_ICONERROR);
                            file.close();
                            std::filesystem::remove(tempFile);
                            continue;
                        }
                        file.close();
                        std::filesystem::remove(tempFile);

                        writeLog("Raw JSON content: " + content);

                        if (content.empty()) {
                            writeLog("Empty content received");
                            continue;
                        }

                        try {
                            writeLog("Starting safe JSON parse");
                            // 使用手动解析的方式
                            size_t versionStart = content.find("\"version\"");
                            size_t versionValueStart = content.find(":", versionStart);
                            size_t versionValueEnd = content.find(",", versionValueStart);

                            if (versionStart == std::string::npos || versionValueStart == std::string::npos) {
                                writeLog("Failed to find version field");
                                continue;
                            }

                            // 提取版本字符串
                            std::string versionStr = content.substr(
                                versionValueStart + 1,
                                (versionValueEnd != std::string::npos ?
                                    versionValueEnd - versionValueStart - 1 :
                                    std::string::npos)
                            );

                            // 清理引号和空格
                            versionStr.erase(
                                std::remove_if(versionStr.begin(), versionStr.end(),
                                    [](char c) { return c == '"' || c == ' '; }),
                                versionStr.end()
                            );

                            writeLog("Extracted version: " + versionStr);

                            // 创建返回的JSON对象
                            json result;
                            result["version"] = versionStr;
                            result["force_update"] = content.find("\"force_update\": true") != std::string::npos;

                            writeLog("Successfully created version info");
                            return result;
                        }
                        catch (const std::exception& e) {
                            std::string errorMsg = "JSON parse error: " + std::string(e.what());
                            writeLog(errorMsg);
                            MessageBoxA(NULL, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
                            continue;
                        }
                    }
                }
            }
            catch (const std::exception& e) {
                std::string errorMsg = "Exception during version check: " + std::string(e.what());
                writeLog(errorMsg);
                MessageBoxA(NULL, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
            }
        }
        writeLog("All version check attempts failed");
        return json::object();
    }

    bool isNewer(const std::string& newVersion, const std::string& currentVersion) {
        writeLog("Comparing versions - New: " + newVersion + ", Current: " + currentVersion);

        std::vector<int> newParts;
        std::vector<int> currentParts;

        std::stringstream ss(newVersion);
        std::string item;
        while (std::getline(ss, item, '.')) {
            newParts.push_back(std::stoi(item));
        }

        ss.clear();
        ss.str(currentVersion);
        while (std::getline(ss, item, '.')) {
            currentParts.push_back(std::stoi(item));
        }

        for (size_t i = 0; i < std::min(newParts.size(), currentParts.size()); i++) {
            if (newParts[i] != currentParts[i]) {
                bool result = newParts[i] > currentParts[i];
                writeLog("Version comparison result: " + std::string(result ? "newer" : "not newer"));
                return result;
            }
        }
        bool result = newParts.size() > currentParts.size();
        writeLog("Version comparison result: " + std::string(result ? "newer" : "not newer"));
        return result;
    }

    void createUpdateBat(const std::string& newExeName) {
        writeLog("Creating update batch file for: " + newExeName);

        std::string batFileName = generateRandomName() + ".bat";
        std::string currentExe = getCurrentExeName();
        std::string currentPath = getCurrentPath();
        std::string currentDir = currentPath.substr(0, currentPath.find_last_of("\\") + 1);

        writeLog("Batch file name: " + batFileName);
        writeLog("Current directory: " + currentDir);

        std::ofstream bat(batFileName);
        bat << "@echo off\n"
            << "echo Starting update process... >> update_log.txt\n"
            << "ping -n 2 127.0.0.1 > nul\n"
            << "cd /d \"" << currentDir << "\"\n"
            << "echo Attempting to delete " << currentExe << " >> update_log.txt\n"
            << "del \"" << currentExe << "\"\n"
            << "if exist \"" << currentExe << "\" (\n"
            << "    echo Retrying delete after delay >> update_log.txt\n"
            << "    ping -n 2 127.0.0.1 > nul\n"
            << "    del \"" << currentExe << "\"\n"
            << ")\n"
            << "echo Moving new file >> update_log.txt\n"
            << "move \"" << newExeName << "\" \"" << currentExe << "\"\n"
            << "echo Starting new version >> update_log.txt\n"
            << "start \"\" \"" << currentDir << currentExe << "\"\n"
            << "echo Cleaning up >> update_log.txt\n"
            << "del \"" << batFileName << "\"\n";

        bat.close();
        writeLog("Update batch file created successfully");

        Sleep(100);
        writeLog("Executing update batch file");
        ShellExecuteA(NULL, "open", (currentDir + batFileName).c_str(), NULL, currentDir.c_str(), SW_HIDE);
    }

public:
    AutoUpdater() {
        initLog();
        writeLog("AutoUpdater initialized");
        writeLog("Current version: " + CURRENT_VERSION);
    }

    ~AutoUpdater() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    bool downloadWithRetry(const std::string& filename, int maxRetries = 3) {
        writeLog("Starting download with retry for: " + filename);
        writeLog("Max retries: " + std::to_string(maxRetries));

        for (const auto& url : DOWNLOAD_URLS) {
            writeLog("Trying download URL: " + url);
            for (int i = 0; i < maxRetries; i++) {
                writeLog("Attempt " + std::to_string(i + 1) + " of " + std::to_string(maxRetries));
                try {
                    if (downloadFile(url, filename)) {
                        writeLog("Download successful");
                        return true;
                    }
                    if (i < maxRetries - 1) {
                        writeLog("Download failed, waiting 2 seconds before retry");
                        Sleep(2000);
                    }
                }
                catch (const std::exception& e) {
                    writeLog("Exception during download: " + std::string(e.what()));
                }
            }
            writeLog("All retry attempts failed for URL: " + url);
        }
        writeLog("All download URLs failed");
        return false;
    }

    bool checkAndUpdate() {
        writeLog("Starting update check");
        const int MAX_RETRIES = 3;

        for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
            writeLog("Update check attempt " + std::to_string(attempt + 1) + " of " + std::to_string(MAX_RETRIES));
            try {
                json versionInfo = getVersionInfo();
                if (versionInfo.empty()) {
                    writeLog("Failed to get version info");
                    if (attempt < MAX_RETRIES - 1) {
                        writeLog("Waiting 2 seconds before retry");
                        Sleep(2000);
                        continue;
                    }
                    return false;
                }

                std::string latestVersion = versionInfo["version"];
                bool forceUpdate = versionInfo["force_update"];

                writeLog("Version check successful:");
                writeLog("Latest version: " + latestVersion);
                writeLog("Force update: " + std::string(forceUpdate ? "yes" : "no"));

                if (forceUpdate || isNewer(latestVersion, CURRENT_VERSION)) {
                    writeLog("Update needed, starting download process");
                    std::string newFileName = generateRandomName();
                    std::string currentPath = getCurrentPath();
                    std::string currentDir = currentPath.substr(0, currentPath.find_last_of("\\") + 1);
                    std::string fullNewPath = currentDir + newFileName;

                    writeLog("New file path: " + fullNewPath);

                    if (downloadWithRetry(fullNewPath)) {
                        writeLog("Download completed, verifying file");

                        if (std::filesystem::exists(fullNewPath)) {
                            auto fileSize = std::filesystem::file_size(fullNewPath);
                            writeLog("Downloaded file size: " + std::to_string(fileSize));

                            if (fileSize > 0) {
                                writeLog("File verification successful, starting update process");
                                createUpdateBat(newFileName);
                                writeLog("Update process initiated, exiting current instance");
                                Sleep(500);
                                exit(0);
                                return true;
                            }
                            else {
                                writeLog("Downloaded file is empty");
                                std::filesystem::remove(fullNewPath);
                                if (attempt < MAX_RETRIES - 1) continue;
                            }
                        }
                        else {
                            writeLog("Downloaded file not found");
                            if (attempt < MAX_RETRIES - 1) continue;
                        }
                    }
                    else {
                        writeLog("Download failed after all retries");
                        if (attempt < MAX_RETRIES - 1) continue;
                    }
                }
                else {
                    writeLog("No update needed");
                    return false;
                }
            }
            catch (const std::exception& e) {
                writeLog("Exception during update check: " + std::string(e.what()));
                if (attempt < MAX_RETRIES - 1) {
                    Sleep(2000);
                    continue;
                }
                return false;
            }
        }
        writeLog("Update process failed after all attempts");
        return false;
    }

    std::string getVersion() const {
        return CURRENT_VERSION;
    }
};