#pragma once
#include <vector>
#include "Config.h"
#include "VecDef.h"
#include "Player.h"

class Misc : public AutoConfigurable {
public:

	struct {
		bool enableSG = false;
		bool enableBhop = false;
		bool enableGrapple = false;
		bool enableTS = false;
		bool useController = false;
	} uiParams;

	struct {
		bool enable = false;
		std::vector<std::string> Lists;
		int activeIndex = 0;
		int switchKeyNext = VK_RIGHT;
		int switchKeyNextController = 0;
		int switchKeyUp = VK_LEFT;
		int switchKeyUpController = 0;
	} cfgSwitcher;

	Misc();

	void renderSwitcher();
	void run();

	void renderMenu();
	void updateKeys();
	bool isKeyHeld(const std::string& keyName);
	bool isKeyToggled(const std::string& keyName);
};