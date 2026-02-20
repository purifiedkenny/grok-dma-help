#pragma once
#include <cstdint>
constexpr uint64_t OFF_LEVEL = 0x1f5a144; //[Miscellaneous].LevelName updated 2025/11/13
constexpr uint64_t OFF_LOCAL_PLAYER = 0x2a7be28; //[Miscellaneous].LocalPlayer updated 2025/11/13
constexpr uint64_t OFF_ENTITY_LIST = 0x65fe6d8; //[Miscellaneous].EntityList updated 2025/11/13
constexpr uint64_t OFF_NAME_LIST = 0x90390c0; //[Miscellaneous].NameList updated 2025/11/13
constexpr uint64_t OFF_NAME_INDEX = 0x38; //nameIndex
constexpr uint64_t OFF_INPUT_SYSTEM = 0x200b800; //[Miscellaneous].InputSystem updated 2025/11/13
constexpr uint64_t OFF_MOUSE_SENSITIVITY = 0x02947730;                   //BASE + mouse_sensitivity

constexpr uint64_t OFF_OBSERVER_LIST = 0x66006f8;  //[Miscellaneous].observerList updated 2025/11/13
constexpr uint64_t OFF_OBSERVER_ARRAY = 0x0958 - 0x0004;      //OBSERVER_INDEX signature

constexpr uint64_t OFF_GAMEMODE = 0x02ab5ae0; //[ConVars].mp_gamemode updated 2025/11/13
constexpr uint64_t OFF_MatrixLevel = 0x3410C40;                   // MatrixLevel  updated 2025/03/07   41 3B 14 80 7D 15
// HUD
constexpr uint64_t OFF_VIEWRENDER = 0x40d8a38; //[Miscellaneous].ViewRender updated 2025/11/13
constexpr uint64_t OFF_VIEWMATRIX = 0x11a350; //[Miscellaneous]->ViewMatrix

// Player
constexpr uint64_t OFF_XP = 0x3984; //[RecvTable.DT_Player].m_xp updated 2025/11/13
constexpr uint64_t OFF_HEALTH = 0x0324; //[RecvTable.DT_Player].m_iHealth updated updated 2025/11/13
constexpr uint64_t OFF_MAXHEALTH = 0x0468; //[RecvTable.DT_Player]->m_iMaxHealth
constexpr uint64_t OFF_SHIELD = 0x01a0; //[RecvTable.DT_BaseEntity].m_shieldHealth updated 2025/08/28
constexpr uint64_t OFF_MAXSHIELD = 0x01a4; //[RecvTable.DT_BaseEntity].m_shieldHealthMax updated 2025/08/28
constexpr uint64_t OFF_ARMORTYPE = 0x4974; //[RecvTable.DT_Player]->m_armorType

constexpr uint64_t OFF_CAMERAORIGIN = 0x1fb4; //[Miscellaneous]->CPlayer!camera_origin
constexpr uint64_t OFF_CUSTOM_SPIRIT = 0x15d4; //[Miscellaneous]m_customScriptInt
constexpr uint64_t OFF_STUDIOHDR = 0xff0; //[Miscellaneous]->CBaseAnimating!m_pStudioHdr 
constexpr uint64_t OFF_BONES = 0x0da8 + 0x48; //[RecvTable.DT_BaseAnimating]->m_nForceBone + 0x48

constexpr uint64_t OFF_LOCAL_ORIGIN = 0x0004; //[DataMap.C_BaseEntity]->m_vecAbsOrigin
constexpr uint64_t OFF_ENTITY_ROTATION = 0x164; //[DataMap.C_BaseEntity]m_angAbsRotation
constexpr uint64_t OFF_ABSVELOCITY = 0x0170; //[DataMap.C_BaseEntity]->m_vecAbsVelocity
constexpr uint64_t OFF_Collision = 0x3B0; //[DataMap.C_BaseEntity]->m_Collision
constexpr uint64_t OFF_Collision_m_vecMins = 0x10; //[DataMap.C_BaseEntity]->m_Collision ->m_vecMins
constexpr uint64_t OFF_Collision_m_vecMaxs = 0x1C; //[DataMap.C_BaseEntity]->m_Collision -> m_vecMaxs
constexpr uint64_t OFF_PROP_SCRIPT_NETWORKFLAG = 0x02d8; //[DataMap.C_BaseEntity]m_networkedFlags

constexpr uint64_t OFF_ZOOMING = 0x1ca1; //[RecvTable.DT_Player]->m_bZooming 0x1c41
constexpr uint64_t OFF_TEAM_NUMBER = 0x0334; //[RecvTable.DT_BaseEntity]->m_iTeamNum
constexpr uint64_t OFF_TEAM_SQUADID = 0x0340; //m_squadID
constexpr uint64_t OFF_NAME = 0x0479; //[RecvTable.DT_BaseEntity]->m_iName
constexpr uint64_t OFF_LIFE_STATE = 0x0690; //[RecvTable.DT_Player]->m_lifeState
constexpr uint64_t OFF_BLEEDOUT_STATE = 0x2920; //[RecvTable.DT_Player]->m_bleedoutState
constexpr uint64_t OFF_LAST_VISIBLE_TIME = 0x1a54; //[Miscellaneous]->CPlayer!lastVisibleTime or [RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x2
constexpr uint64_t OFF_LAST_AIMEDAT_TIME = 0x1a58; //CWeaponX!lastCrosshairTargetTime or [RecvTable.DT_BaseCombatCharacter]->m_hudInfo_visibilityTestAlwaysPasses + 0x2 + 0x8
constexpr uint64_t OFF_VIEW_ANGLES = 0x26f4 - 0x104; //[RecvTable.DT_Player].m_ammoPoolCapacity - 0x14 updated 2025/11/13
constexpr uint64_t OFF_BREATH_ANGLES = OFF_VIEW_ANGLES - 0x10; //updated 2024/05/23
constexpr uint64_t OFF_PUNCH_ANGLES = 0x2508; //[DataMap.C_Player].m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle updated 2025/11/13
constexpr uint64_t OFF_YAW = 0x230c - 0x8; //[DataMap.C_Player].m_currentFramePlayer.m_ammoPoolCount - 0x8 updated 2025/11/13
// Weapon
constexpr uint64_t OFF_WEAPON_HANDLE = 0x19b4; //[RecvTable.DT_BaseCombatCharacter].m_latestPrimaryWeapons
constexpr uint64_t OFF_WEAPON_INDEX = 0x1818; //[RecvTable.DT_WeaponX].m_weaponNameIndex updated 2025/11/13
constexpr uint64_t OFF_PROJECTILESPEED = 0x1aa0 + 0xd78; //[Miscellaneous].WeaponSettingsMeta_base + [WeaponSettings].projectile_launch_speed updated 2025/11/05
constexpr uint64_t OFF_PROJECTILESCALE = OFF_PROJECTILESPEED + 0x8; //[WeaponSettingsMeta].base + [WeaponSettings].projectile_gravity_scale
constexpr uint64_t OFF_OFFHAND_WEAPON = 0x19c4; //[DataMap.C_BaseCombatCharacter].m_latestNonOffhandWeapons updated 2025/11/13
constexpr uint64_t OFF_CURRENTZOOMFOV = 0x1650 + 0x00c0; //[RecvTable.DT_WeaponX].m_playerData + m_curZoomFOV updated 2025/11/13
constexpr uint64_t OFF_TARGETZOOMFOV = OFF_CURRENTZOOMFOV + 0x4; // Changes according to magnification
constexpr uint64_t OFF_WEAPON_AMMO = 0x1600; //[RecvTable.DT_WeaponX_LocalWeaponData].m_ammoInClip updated 2025/11/13
constexpr uint64_t OFF_RELOADING = 0x161a; //[RecvTable.DT_WeaponX_LocalWeaponData]->m_bInReload

constexpr uint64_t OFF_ACTIVE_WEAPON = 0x1948 + 0x0058; //[RecvTable.DT_Player].m_inventory + WeaponInventory_Client>activeWeapons updated 2025/11/13
constexpr uint64_t OFF_WEAPON_BITFIELD = 0x1790;

// Glow
constexpr uint64_t OFF_GLOW_HIGHLIGHTS = 0x6d48eb0; //HighlightSettings 0x68E5030 0x68e6930
constexpr uint64_t OFF_GLOW_ENABLE = 0x28C; //updated OFF_GLOW_CONTEXT_ID 7 = enabled, 2 = disabled
constexpr uint64_t OFF_GLOW_THROUGH_WALL = 0x26C; //Script_Highlight_SetVisibilityType updated 2 = enabled, 5 = disabled
constexpr uint64_t OFF_GLOW_FIX = 0x278; //updated 2024/04/12
constexpr uint64_t OFF_GLOW_HIGHLIGHT_ID = 0x29C; //[RecvTable.DT_HighlightSettings].m_highlightGenericContexts updated 2024/11/06 [DT_HighlightSettings].m_highlightServerActiveStates
constexpr uint64_t OFF_GLOW_HIGHLIGHT_TYPE_SIZE = 0x34; //updated 2024/04/12
constexpr uint64_t OFF_GLOW_DISTANCE = 0x264; //Highlight_SetFarFadeDist [ 8B 81 ? ? ? ? 85 C0 75 ? 48 8D 0D ? ? ? ? E9 ] updated 2024/08/22

// Misc
constexpr uint64_t OFF_TIME_BASE = 0x2158; //[RecvTable.DT_Player].m_currentFramePlayer updated 2025/11/13
constexpr uint64_t OFF_TRAVERSAL_START_TIME = 0x2d3c; //[DataMap.C_Player].m_traversalStartTime updated 2025/11/13
constexpr uint64_t OFF_TRAVERSAL_PROGRESS = 0x2d34; //[DataMap.C_Player].m_traversalProgress updated 2025/11/13
constexpr uint64_t OFF_UID = 0x2708; //[RecvTable.DT_Player].m_platformUserId updated 2025/11/13
constexpr uint64_t OFF_NucleusId = 0x2710;   //[RecvTable.DT_Player].m_nucleusId updated 2025/11/13
constexpr uint64_t OFF_VIEWMODEL = 0x2f60; //[RecvTable.DT_Player].m_hViewModels updated 2025/11/13

constexpr uint64_t OFF_MODELNAME = 0x0030; //m_ModelName
constexpr uint64_t OFF_Wall_RunStart_TIME = 0x386c; //[DataMap.C_Player].m_wallRunStartTime updated 2025/11/13
constexpr uint64_t OFF_Wall_RunClear_TIME = 0x3870; //[DataMap.C_Player].m_wallRunClearTime float updated 2025/11/13
constexpr uint64_t OFF_SET_FLAGS = 0x00c8;//[DataMap.C_Player]->m_fFlags
constexpr uint64_t OFF_SignifierName = 0x0470; //[RecvTable.DT_BaseEntity].m_iSignifierName updated 2025/11/13

constexpr uint64_t OFF_SET_SKYDRIVESTATE = 0x49d4; //[RecvTable.DT_Player].m_skydiveState updated 2025/11/13
constexpr uint64_t OFF_SET_IN_DUCKSTATE = 0x2c24; //[DataMap.C_Player].m_duckState updated 2025/11/13

constexpr uint64_t OFF_INATTACK = 0x040d8b30; //[Buttons].in_attack updated 2025/11/13
constexpr uint64_t OFF_IN_JUMP = 0x40d94e0; //[Buttons].in_jump updated 2025/11/13
constexpr uint64_t OFF_IN_DUCK = 0x040d95d8; //[Buttons].in_duck updated 2025/11/13
constexpr uint64_t OFF_IN_FORWARD = 0x040d8a60; //[Buttons].in_forward updated 2025/11/13
constexpr uint64_t OFF_IN_BACKWARD = 0x040d8a88; //[Buttons].in_backward updated 2025/11/13
constexpr uint64_t OFF_IN_MOVELEFT = 0x040d8a50; //[Buttons].in_moveleft updated 2025/11/13
constexpr uint64_t OFF_IN_MOVERIGHT = 0x040d8a78; //[Buttons].in_moveright updated 2025/11/13
constexpr uint64_t OFF_IN_RELOAD = 0x040d9448; //[Buttons].in_reload updated 2025/11/13
constexpr uint64_t OFF_IN_ZOOM = 0x040d9560; //[Buttons].in_zoom updated 2025/11/13

constexpr uint64_t OFF_GRAPPLE = 0x2e88; //[RecvTable.DT_Player].m_grapple updated 2025/11/13
constexpr uint64_t OFF_GRAPPLEACTIVED = 0x2f10; //[RecvTable.DT_Player].m_grappleActive updated 2025/11/13
constexpr uint64_t OFF_GRAPPLEATTACHED = 0x0048;					  //[DataMap.GrappleData]->m_grappleAttached
constexpr uint64_t OFF_FLAGS = 0x00c8;							  //[RecvTable.DT_Player]->m_fFlags

constexpr uint64_t OFF_NETVAR_TABLE_PTR = 0x92afbb0; //[Miscellaneous].NetworkVarTablePtr updated 2025/11/13
constexpr uint64_t OFF_NETVAR_GLOBAL = 0x4968;                    // m_playerScriptNetDataGlobal    updated  2025/01/08
constexpr uint64_t OFF_NETVAR_RANGES = 0x0b72;                    //[RecvTable.DT_ScriptNetData_SNDC_GLOBAL_NON_REWIND].m_ranges[0]
constexpr uint64_t OFF_NETVAR_INT32S = 0x0bc0;                    //[RecvTable.DT_ScriptNetData_SNDC_GLOBAL] ->m_int32s[0]