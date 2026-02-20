#pragma once
#include "Config.h"
#include "Player.h"
#include "Offsets.hpp"
#include <DMALibrary/Memory/Memory.h>
#include "LocalPlayer.h"


class Spectator : public AutoConfigurable {
public:

	void run();
	void drawMenu();

	auto GetTotalSpectators() const -> int {
		return TotalSpectators;
	}

	auto GetSpectators() const -> std::vector<std::string> {
		return Spectators;
	}

	Spectator();

	struct UIParams
	{
		bool enabled = true;
		
	} uiParams;;

private:
	int TotalSpectators = 0;
	std::vector<std::string> Spectators;
	std::chrono::milliseconds LastUpdateTime;

	bool IsSpectating(Player* player); 
};