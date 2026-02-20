#pragma once

#include <vector>
#include <string>

inline std::vector<std::tuple<int, std::string, std::string>> MedList = {
	{209, "Phoenix Kit","f"},
	{210, "Med-Kit","g"},
	{211, "Syringe","h"},
	{212, "Shield-Cell","j"},
	{213, "Shield-Battery","i"},
};

inline std::vector<std::tuple<int, std::string, std::string>> LightGunList = {
	{126, "P2020","A"},
	{135, "RE-45","B"},
	{47, "Alternator","H"},
	{84, "R-301","M"},
	{79, "M600 Spit-fire","P"},
	{42, "G7 Scout","Q"},
	{160, "Light Ammo","a"},
	{52, "R-99","I"}
};

inline std::vector<std::tuple<int, std::string, std::string>> EnergyGunList = {
	{20, "Devotion LMG","O"},
	{8, "L-STAR","?"},

	{26, "Triple Take","S"},
	{155, "Nemesis Burst AR","3"},
	{161, "Energy Ammo","b"},
	{64, "VOLT","2"}
};

inline std::vector<std::tuple<int, std::string, std::string>> HeavyGunList = {
	{31, "VK47 FlatLine","K"},
	{36, "Hemlock","L"},
	{58, "Prowler","J"},
	{150, "30-30 Repeater","4"},
	{166, "Rampage","1"},
	{163, "Heavy Ammo","d"},
	{171, "C.A.R","0"}
};

inline std::vector<std::tuple<int, std::string, std::string>> SniperGunList = {
	{120, "Wing Man","C"},
	{69, "Long Bow","R"},
	{74, "Charge Rifle",">"},
	{141, "Sentinel",":"},
	{164, "Sniper Ammo","="},
};

inline std::vector<std::tuple<int, std::string, std::string>> ShotGunlist = {
	{104, "Mozambique","G"},
	{92, "EVA","D"},

	{162, "Shotgun Ammo","c"},
	{2, "Mastiff","F"}
};

inline std::vector<std::tuple<int, std::string, std::string>> GrenadeList = {
	{245, "Thermite","n"},
	{246, "Frag Grenade","p"},
	{247, "Arc_star","q"},
};

inline std::vector<std::tuple<int, std::string, std::string>> RedGunlist = {
	{148, "Bow","<"},
	{1, "Kraber","T"},
	{19, "Havoc","N"},
	{99, "PeaceKeeper","E"},
};

inline std::vector<std::tuple<int, std::string, std::string>> LvL4Bodylist = {
	{219, "Helmet5","k"},
	{243, "Backpack4","n"},
	{218, "Helmet4","k"},
	{400, "EVO Cache","n"},
};

inline std::vector<std::tuple<int, std::string, std::string>> LvL3Bodylist = {
	{233, "Knockdown Shield3","m"},
	{242, "Backpack3","n"},
	{238, "Shield3","l"},
};

inline std::vector<std::tuple<int, std::string, std::string>> LvL2Bodylist = {
	{232, "Knockdown Shield2","m"},
	{241, "Backpack2","}"},
	{237, "Shield2","l"},
};

inline std::vector<std::tuple<int, std::string, std::string>> LvL1Bodylist = {
	{231, "Knockdown Shield","m"},
	{240, "Backpack","}"},
	{236, "Shield","l"},
};

inline std::vector<std::tuple<int, std::string, std::string>> Partslvl4list = {
	{306, "Sniper Threat","X"},
	{310, "Suppressor","Y"},
	{267, "Light Mag","Z"},
	{271, "Heavy Mag","w"},
	{275, "Energy Mag","("},
	{279, "Sniper Mag",")"},
	{283, "Shotgun Bolt","x"},
	{304, "Boosted_Loader","7"},
	{305, "GunShield ","6"},
	{291, "SelectFire ","7"},
};

inline std::vector<std::tuple<int, std::string, std::string>> Partslvl3list = {
	{253, "Hcog-3x","v"},
	{254, "Holo2x-4x",";"},
	{256, "sniper4x8x","W"},
	{263, "Laser sight","8"},
	{260, "Suppressor","Y"},
	{266, "Light Mag","Z"},
	{270, "Heavy Mag","w"},
	{274, "Energy Mag","("},
	{278, "Sniper Mag",")"},
	{282, "Shotgun Bolt","x"},
	{286, "Stock","y"},
	{289, "Sniper Stock","z"},
	{208, "Accelerant","{"},
};

inline std::vector<std::tuple<int, std::string, std::string>> Partslvl2list = {
	{249, "Hcog-2x","."},
	{251, "Holo1x-2x","t"},
	{262, "Laser sight","8"},
	{255, "Sniper6x","v"},
	{259, "Suppressor","Y"},
	{265, "Light Mag","Z"},
	{269, "Heavy Mag","w"},
	{273, "Energy Mag","("},
	{277, "Sniper Mag",")"},
	{281, "Shotgun Bolt","x"},
	{285, "Stock","y"},
	{288, "Sniper Stock","z"},
	{317, "Evac Tower","*"}
};

inline std::vector<std::tuple<int, std::string, std::string>> Partslvl1list = {
	{248, "Hcog-1x","r"},
	{261, "Laser sight","8"},
	{258, "Suppressor","Y"},
	{264, "Light Mag","Z"},
	{268, "Heavy Mag","w"},
	{272, "Energy Mag","("},
	{276, "Sniper Mag",")"},
	{280, "Shotgun Bolt","x"},
	{284, "Stock","y"},
	{287, "Sniper Stock","z"}
};
inline auto Lists =
{
	MedList,
	LightGunList,
	ShotGunlist,
	EnergyGunList,
	HeavyGunList,
	SniperGunList,
	Partslvl1list,
	Partslvl2list,
	Partslvl3list,
	Partslvl4list,
	LvL1Bodylist,
	LvL2Bodylist,
	LvL3Bodylist,
	LvL4Bodylist,
	RedGunlist,
	GrenadeList,
};