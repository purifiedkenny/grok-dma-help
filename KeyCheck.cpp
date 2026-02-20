#include "include.h"
#include "KeyCheck.h"
#include "Menu.h"

//TODO: �����ƶ���Misc
void toggleRadar() {
    // ȷ��localPlayer��Ч
    if (!localPlayer) {
        return;
    }

    int localPlayerTeamID = localPlayer->Team;

    if (localPlayerTeamID != 1) {
        try {
            double curTime = mem.Read<float>(localPlayer->BasePointer + OFF_TIME_BASE);
            double continueTime = 0.2;
            double endTime = curTime + continueTime;

            while (curTime < endTime) {
                mem.Write<int>(localPlayer->BasePointer + OFF_TEAM_NUMBER, 1);
                curTime = mem.Read<float>(localPlayer->BasePointer + OFF_TIME_BASE);
            }

            curTime = mem.Read<float>(localPlayer->BasePointer + OFF_TIME_BASE);
            endTime = curTime + continueTime;

            while (curTime < endTime) {
                mem.Write<int>(localPlayer->BasePointer + OFF_TEAM_NUMBER, localPlayerTeamID);
                curTime = mem.Read<float>(localPlayer->BasePointer + OFF_TIME_BASE);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error in toggleRadar: " << e.what() << std::endl;
        }
    }
}

void KeyCheck::run() {
    try {
        // �ȴ�ȷ����ʼ�����
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // ע�ᰴ��
        bool success = true;
        success &= keyDetector->registerKey("GLOBAL", "toggle_gui", esp->uiSettings.menuKey);
        success &= keyDetector->registerKey("GLOBAL", "toggle_radar", esp->uiSettings.radarKey);
        success &= keyDetector->registerKey("GLOBAL", "end_app", esp->uiSettings.endKey);
        success &= keyDetector->registerKey("GLOBAL", "fusion_background", esp->uiSettings.fusionKey);

        success &= keyDetector->registerKey("GLOBAL", "toggle_gui_controller", esp->uiSettings.menuKeyController);
        success &= keyDetector->registerKey("GLOBAL", "end_app_controller", esp->uiSettings.endKeyController);
        success &= keyDetector->registerKey("GLOBAL", "fusion_background_controller", esp->uiSettings.fusionKeyController);
        success &= keyDetector->registerKey("GLOBAL", "toggle_radar_controller", esp->uiSettings.radarKeyController);

        aimbot->registerHotKeys();

        while (true) {
            try {
                if (entityManager.isGameInput()) {
                    int mode = 1;
                    uint64_t inputAddress = mem.OFF_BASE + OFF_INPUT_SYSTEM + 0xb0;
                    if (mem.IsValidPointer(inputAddress))
                    {
                        mem.Read(inputAddress, &mem.InputButton, sizeof(Memory::InputButton));
                    }
                    else
                        mode = 2;

                    keyDetector->setDetectionMode(mode);
                }
                else if (!entityManager.isGameInput()) {
                    keyDetector->setDetectionMode(2);
                }
                else {
                    keyDetector->setDetectionMode(0);
                }

                keyDetector->detectionThreadFunc();

                keyDetector->updateKeyCode("GLOBAL", "toggle_gui", esp->uiSettings.menuKey);
                keyDetector->updateKeyCode("GLOBAL", "end_app", esp->uiSettings.endKey);
                keyDetector->updateKeyCode("GLOBAL", "fusion_background", esp->uiSettings.fusionKey);
                keyDetector->updateKeyCode("GLOBAL", "toggle_radar", esp->uiSettings.radarKey);

                keyDetector->updateKeyCode("GLOBAL", "toggle_gui_controller", esp->uiSettings.menuKeyController);
                keyDetector->updateKeyCode("GLOBAL", "fusion_background_controller", esp->uiSettings.fusionKeyController);
                keyDetector->updateKeyCode("GLOBAL", "toggle_radar_controller", esp->uiSettings.radarKeyController);
                keyDetector->updateKeyCode("GLOBAL", "end_app_controller", esp->uiSettings.endKeyController);

                if (!misc->uiParams.useController)
                {
                    if (keyDetector->isKeyTriggered("GLOBAL", "toggle_gui")) {
                        Menu::isMenuVisible = !Menu::isMenuVisible;
                        keyDetector->clearTrigger("GLOBAL", "toggle_gui");
                    }

                    // ����״��л�
                    if (keyDetector->isKeyTriggered("GLOBAL", "toggle_radar")) {
                        toggleRadar();
                        keyDetector->clearTrigger("GLOBAL", "toggle_radar");
                    }

                    // ����˳���
                    if (keyDetector->isKeyTriggered("GLOBAL", "end_app")) {
                        // ���ŵعر�
                        keyDetector->shutdown();
                        ExitProcess(-1);
                    }

                    if (keyDetector->isKeyTriggered("GLOBAL", "fusion_background")) {
                        Menu::fusionMode = !Menu::fusionMode;
                        keyDetector->clearTrigger("GLOBAL", "fusion_background");
                    }
                }
                else
                {
                    if (keyDetector->isKeyTriggered("GLOBAL", "toggle_gui_controller")) {
                        Menu::isMenuVisible = !Menu::isMenuVisible;
                        keyDetector->clearTrigger("GLOBAL", "toggle_gui_controller");
                    }

                    // ����״��л�
                    if (keyDetector->isKeyTriggered("GLOBAL", "toggle_radar_controller")) {
                        toggleRadar();
                        keyDetector->clearTrigger("GLOBAL", "toggle_radar_controller");
                    }

                    // ����˳���
                    if (keyDetector->isKeyTriggered("GLOBAL", "end_app_controller")) {
                        // ���ŵعر�
                        keyDetector->shutdown();
                        ExitProcess(-1);
                    }

                    if (keyDetector->isKeyTriggered("GLOBAL", "fusion_background_controller")) {
                        Menu::fusionMode = !Menu::fusionMode;
                        keyDetector->clearTrigger("GLOBAL", "fusion_background_controller");
                    }
                }
                

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            catch (const std::exception& e) {
                std::cerr << "Error in KeyCheck loop: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error in KeyCheck::run: " << e.what() << std::endl;
    }

}