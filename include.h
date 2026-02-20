#ifndef INCLUDE_H
#define INCLUDE_H

#include <SFML/imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics.hpp>

#include <tchar.h>

#define IMGUI_USER_CONFIG <SFML/imconfig-SFML.h>
// ImGui 相关
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_impl_win32.h>

// 标准库
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <Windows.h>

// 项目头文件
#include "VecDef.h"
#include "ImGuiRender.h"
#include "Config.h"
#include "Offsets.hpp"

#include "MapList.h"
#include "Matrix.h"
#include "Entity.h"
#include "Player.h"
#include "Item.h"
#include "Shield.h"
#include "LocalPlayer.h"

#include "Map.h"

#include "Aimbot.h"
#include "Skynade.h"
#include "Triggerbot.h"
#include "Esp.h"
#include "Glow.h"
#include "Spectator.h"
#include "Misc.h"

#include "EntityManager.h"
#include "ItemProcessor.h"
#include "KeyDetector.h"
#include "KmBox.h"
#include "Translation.h"


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define entityManager EntityManager::getInstance()
inline Map* map = new Map;
inline LocalPlayer* localPlayer = new LocalPlayer;
inline Aimbot* aimbot = new Aimbot;
inline Skynade* skyNade = new Skynade;
inline TriggerBot* triggerbot = new TriggerBot;
inline ESP* esp = new ESP;
inline Glow* glow = new Glow;
inline Spectator* spectator = new Spectator;
inline Misc* misc = new Misc;
inline GameView* gameView = new GameView;
inline GameView* gameViewItems = new GameView;
inline KeyDetector* keyDetector = new KeyDetector;
inline KmboxController* kmbox = new KmboxController;
inline ItemScreenProcessor* itemProcessor = new ItemScreenProcessor;
//#define DEBUG_MODE

// 辅助宏，简化按键状态查询
#define IS_KEY_HELD(context, key) keyDetector->isKeyHeld(context, key)
#define IS_KEY_TRIGGERED(context, key) keyDetector->isKeyTriggered(context, key)
#define CLEAR_KEY_TOGGLE_STATE(context, key) keyDetector->clearTrigger(context, key)
#define REGISTER_KEY(context, key, code) keyDetector->registerKey(context, key, code)

#endif