#include "include.h"
#include "Spectator.h"

Spectator::Spectator() : AutoConfigurable("Spectator")
{
	TotalSpectators = 0;
	Spectators.clear();
	LastUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	registerParam("Enabled", uiParams.enabled);
}

void Spectator::run()
{
	std::chrono::milliseconds Now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	if (Now >= LastUpdateTime + std::chrono::milliseconds(1000))
	{
		int TempTotalSpectators = 0;
		std::vector<std::string> TempSpectators;
		const auto& playersCopy = entityManager.getPlayers();
		for (const auto &player : *playersCopy)
		{
			if (IsSpectating(player)) {
				TempSpectators.push_back(player->PlayerName);
				TempTotalSpectators++;
			}
		}

		TotalSpectators = TempTotalSpectators / 2;
		Spectators = TempSpectators;
		LastUpdateTime = Now;
	}
}

bool Spectator::IsSpectating(Player* player) {
	if (!player || !player->IsDead)
		return false;
	uint64_t SpectatorList = mem.Read<uint64_t>(mem.OFF_BASE + OFF_OBSERVER_LIST);
	int PlayerData = mem.Read<int>(player->BasePointer + 0x38);
	int SpecIndex = mem.Read<int>(SpectatorList + PlayerData * 8 + OFF_OBSERVER_ARRAY);//0x0964
	uint64_t SpectatorAddr = mem.Read<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + ((static_cast<uint64_t>(SpecIndex & 0xFFFF)) << 5));
	if (SpectatorAddr == localPlayer->BasePointer)
		return true;
	return false;
}

void Spectator::drawMenu()
{
	if (!uiParams.enabled)
		return;
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Once);
	ImGui::Begin(t_("Spectator"), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("%s %d", t_("Total Spectators:"), TotalSpectators);

	ImGui::Separator();
	for (auto& Spectator : Spectators)
	{
		ImGui::Text(Spectator.c_str());
	}
	ImGui::End();
	ImGui::PopFont();
}