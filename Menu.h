#pragma once
#include <vector>
#include <string>

namespace Menu
{
	inline bool isMenuVisible = true;
    inline char newConfigName[128] = "";
    inline int selectedConfigIndex = -1;
    inline std::vector<std::string> availableConfigs;
    inline bool fusionMode = false;

    // 更新可用配置列表
    void updateAvailableConfigs();
    void renderConfigManager();
    void renderMiscPage();
	void RenderMenu();
	void run();
}