#pragma once
#include "MPList.h"

// ----------------------
// 基础武器类型定义
// ----------------------
namespace WeaponType
{
    // 突击步枪
    enum AssaultRifles
    {
        RSPN101 = mp_weapon_rspn101,             // R-301
        HEMLOK = mp_weapon_hemlok,               // 平行步枪
        HEMLOK_CRATE = mp_weapon_hemlok_crate,   // 空投平行步枪
        FLATLINE = mp_weapon_vinson,             // 平行机枪
        G2 = mp_weapon_g2,                       // G7侦查枪
        HAVOC = mp_weapon_energy_ar,             // 哈沃克
        HAVOC_CRATE = mp_weapon_energy_ar_crate, // 空投哈沃克
        NEMESIS = mp_weapon_nemesis,             // 复仇女神
        RIFLE_3030 = mp_weapon_3030              // 30-30
    };

    // 轻机枪
    enum LMGs
    {
        DRAGON_LMG = mp_weapon_dragon_lmg,     // 喷火轻机枪
        ESAW = mp_weapon_esaw,                 // 专注轻机枪
        ESAW_CRATE = mp_weapon_esaw_crate,     // 空投专注
        LSTAR = mp_weapon_lstar,               // L-Star
        LSTAR_CRATE = mp_weapon_lstar_crate,   // 空投L-Star
        RAMPAGE = mp_weapon_lmg                // 暴走轻机枪
    };

    // 冲锋枪
    enum SMGs
    {
        ALTERNATOR = mp_weapon_alternator_smg, // 转换者
        R99 = mp_weapon_r97,                   // R-99
        R99_CRATE = mp_weapon_r97_crate,       // 空投R-99
        PDW = mp_weapon_pdw,                   // 猎兽
        PDW_CRATE = mp_weapon_pdw_crate,       // 空投猎兽
        VOLT = mp_weapon_volt_smg,             // 电能冲锋枪
        CAR = mp_weapon_car                    // CAR冲锋枪
    };

    // 霰弹枪
    enum Shotguns
    {
        EVA8 = mp_weapon_shotgun,                // EVA-8自动霰弹枪
        EVA8_CRATE = mp_weapon_shotgun_crate,    // 空投EVA-8
        MASTIFF = mp_weapon_mastiff,             // 敖犬
        MASTIFF_CRATE = mp_weapon_mastiff_crate, // 空投敖犬
        PEACEKEEPER = mp_weapon_energy_shotgun,  // 和平捍卫者
        PK_CRATE = mp_weapon_energy_shotgun_crate // 空投和平捍卫者
    };

    // 狙击枪/精确武器
    enum SniperMarksman
    {
        SENTINEL = mp_weapon_sentinel,           // 哨兵
        BOW = mp_weapon_bow,                     // 波塞克复合弓
        KRABER = mp_weapon_sniper,               // 克莱伯
        CHARGE_RIFLE = mp_weapon_defender,       // 充能步枪
        LONGBOW = mp_weapon_dmr,                 // 长弓DMR
        TRIPLE_TAKE = mp_weapon_doubletake,      // 三重式狙击枪
        EPG = mp_weapon_epg,                     // 能源榴弹枪
        EPG_TETHERED = mp_weapon_epg_tethered    // 能源榴弹枪-锁定版
    };

    // 手枪
    enum Pistols
    {
        P2020 = mp_weapon_semipistol,             // P2020
        RE45 = mp_weapon_autopistol,              // RE-45自动手枪
        RE45_CRATE = mp_weapon_autopistol_crate,  // 空投RE-45
        WINGMAN = mp_weapon_wingman,              // 辅助手枪
        WINGMAN_CRATE = mp_weapon_wingman_crate,  // 空投辅助手枪
        MOZAMBIQUE = mp_weapon_shotgun_pistol     // 莫桑比克
    };

    // 莫桑比克变种
    enum MozambiqueVariants
    {
        MOZAMBIQUE_LIGHT = mp_weapon_shotgun_pistol_light,    // 轻型弹莫桑比克
        MOZAMBIQUE_ENERGY = mp_weapon_shotgun_pistol_energy,  // 能量弹莫桑比克
        MOZAMBIQUE_SNIPER = mp_weapon_shotgun_pistol_sniper,  // 狙击弹莫桑比克
        MOZAMBIQUE_HEAVY = mp_weapon_shotgun_pistol_heavy,    // 重型弹莫桑比克
        MOZAMBIQUE_EPG = mp_weapon_shotgun_pistol_epg         // EPG莫桑比克
    };

    // 投掷武器
    enum Throwables
    {
        FRAG_GRENADE = mp_weapon_frag_grenade,           // 破片手雷
        ARC_STAR = mp_weapon_grenade_emp,                // 电弧星
        THERMITE = mp_weapon_thermite_grenade,           // 铝热剂手雷
        REV_SILENCE = mp_weapon_grenade_rev_shell,       // 亡灵沉默手雷
        THROWING_KNIFE = mp_weapon_throwingknife         // 投掷刀
    };

    // 近战武器-主要
    enum MeleePrimary
    {
        WRAITH_KUNAI_PRIMARY = mp_weapon_wraith_kunai_primary,              // 幻影苦无-主要
        BANGALORE_HEIRLOOM_PRIMARY = mp_weapon_bangalore_heirloom_primary,  // 班加罗尔传家宝-主要
        BLOODHOUND_AXE_PRIMARY = mp_weapon_bloodhound_axe_primary,          // 寻血猎犬战斧-主要
        CAUSTIC_HAMMER_PRIMARY = mp_weapon_caustic_hammer_primary,          // 侵蚀锤-主要
        GIBRALTAR_CLUB_PRIMARY = mp_weapon_gibraltar_club_primary,          // 直布罗陀战棍-主要
        LIFELINE_BATON_PRIMARY = mp_weapon_lifeline_baton_primary,          // 命脉短棍-主要
        MIRAGE_STATUE_PRIMARY = mp_weapon_mirage_statue_primary,            // 幻象雕像-主要
        OCTANE_KNIFE_PRIMARY = mp_weapon_octane_knife_primary,              // 动力刀-主要
        PATHFINDER_GLOVES_PRIMARY = mp_weapon_pathfinder_gloves_primary,    // 探路者拳套-主要
        WATTSON_GADGET_PRIMARY = mp_weapon_wattson_gadget_primary,          // 华森设备-主要
        CRYPTO_HEIRLOOM_PRIMARY = mp_weapon_crypto_heirloom_primary,        // 密客传家宝-主要
        REVENANT_SCYTHE_PRIMARY = mp_weapon_revenant_scythe_primary,        // 亡灵镰刀-主要
        LOBA_HEIRLOOM_PRIMARY = mp_weapon_loba_heirloom_primary,            // 罗芭传家宝-主要
        RAMPART_WRENCH_PRIMARY = mp_weapon_rampart_wrench_primary,          // 兰伯特扳手-主要
        HORIZON_HEIRLOOM_PRIMARY = mp_weapon_horizon_heirloom_primary,      // 地平线传家宝-主要
        VALKYRIE_SPEAR_PRIMARY = mp_weapon_valkyrie_spear_primary,          // 瓦尔基里长矛-主要
        ASH_HEIRLOOM_PRIMARY = mp_weapon_ash_heirloom_primary,              // 艾许传家宝-主要
        SEER_HEIRLOOM_PRIMARY = mp_weapon_seer_heirloom_primary,            // 希尔传家宝-主要
        WRAITH_KUNAI_RT01_PRIMARY = mp_weapon_wraith_kunai_rt01_primary,    // 幻影苦无RT01-主要
        FUSE_HEIRLOOM_PRIMARY = mp_weapon_fuse_heirloom_primary,            // 暴雷传家宝-主要
        ARTIFACT_DAGGER_PRIMARY = mp_weapon_artifact_dagger_primary,        // 神器匕首-主要
        NEWCASTLE_HEIRLOOM_PRIMARY = mp_weapon_newcastle_heirloom_primary,  // 纽卡斯尔传家宝-主要
        ARTIFACT_SWORD_PRIMARY = mp_weapon_artifact_sword_primary,          // 神器剑-主要
        HOOK_SWORD_PRIMARY = mp_weapon_hook_sword_primary,                  // 钩剑-主要
        KARAMBIT_PRIMARY = mp_weapon_agnostic_karambit_primary              // 爪刀-主要
    };

    // 近战武器-次要
    enum MeleeSecondary
    {
        EMPTY_HANDED = melee_pilot_emptyhanded,              // 空手
        SURVIVAL_MELEE = mp_weapon_melee_survival,           // 生存近战
        WRAITH_KUNAI = melee_wraith_kunai,                   // 幻影苦无
        BANGALORE_HEIRLOOM = melee_bangalore_heirloom,       // 班加罗尔传家宝
        BLOODHOUND_AXE = melee_bloodhound_axe,               // 寻血猎犬战斧
        CAUSTIC_HAMMER = melee_caustic_hammer,               // 侵蚀锤
        GIBRALTAR_CLUB = melee_gibraltar_club,               // 直布罗陀战棍
        LIFELINE_BATON = melee_lifeline_baton,               // 命脉短棍
        MIRAGE_STATUE = melee_mirage_statue,                 // 幻象雕像
        OCTANE_KNIFE = melee_octane_knife,                   // 动力刀
        PATHFINDER_GLOVES = melee_pathfinder_gloves,         // 探路者拳套
        WATTSON_GADGET = melee_wattson_gadget,               // 华森设备
        CRYPTO_HEIRLOOM = melee_crypto_heirloom,             // 密客传家宝
        REVENANT_SCYTHE = melee_revenant_scythe,             // 亡灵镰刀
        LOBA_HEIRLOOM = melee_loba_heirloom,                 // 罗芭传家宝
        RAMPART_WRENCH = melee_rampart_wrench,               // 兰伯特扳手
        HORIZON_HEIRLOOM = melee_horizon_heirloom,           // 地平线传家宝
        VALKYRIE_SPEAR = melee_valkyrie_spear,               // 瓦尔基里长矛
        ASH_HEIRLOOM = melee_ash_heirloom,                   // 艾许传家宝
        SEER_HEIRLOOM = melee_seer_heirloom,                 // 希尔传家宝
        WRAITH_KUNAI_RT01 = melee_wraith_kunai_rt01,         // 幻影苦无RT01
        FUSE_HEIRLOOM = melee_fuse_heirloom,                 // 暴雷传家宝
        ARTIFACT_DAGGER = melee_artifact_dagger,             // 神器匕首
        NEWCASTLE_HEIRLOOM = melee_newcastle_heirloom,       // 纽卡斯尔传家宝
        ARTIFACT_SWORD = melee_artifact_sword,               // 神器剑
        HOOK_SWORD = melee_hook_sword,                       // 钩剑
        KARAMBIT = melee_agnostic_karambit                   // 爪刀
    };

    // 特殊武器
    enum SpecialWeapons
    {
        TITAN_SWORD = mp_weapon_titan_sword,                 // 泰坦剑
        MOUNTED_TURRET = mp_weapon_mounted_turret_weapon,    // 安装炮塔
        TURRET_PLACEABLE = mp_weapon_mounted_turret_placeable, // 可放置炮塔
        POWER_SWORD = mp_weapon_power_sword                  // 能量剑
    };

    // NPC武器
    enum NPCWeapons
    {
        NPC_HEMLOK = npc_weapon_hemlok,                    // NPC平行步枪
        NPC_ENERGY_SHOTGUN = npc_weapon_energy_shotgun,    // NPC能量霰弹枪
        NPC_LSTAR = npc_weapon_lstar,                      // NPC L-Star
        NPC_THERMITE = npc_weapon_thermite_grenade,        // NPC铝热剂
        NPC_SPIDER = npc_weapon_spider_jungle,             // NPC丛林蜘蛛
        NPC_NESSIE = npc_weapon_nessie                     // NPC尼斯湖水怪
    };
}

// ----------------------
// 传奇技能类型定义
// ----------------------
namespace LegendAbility
{
    // 班加罗尔技能
    enum Bangalore
    {
        SMOKE_LAUNCHER = mp_weapon_grenade_bangalore,       // 烟雾发射器
        ROLLING_THUNDER = mp_ability_rolling_thunder_piece, // 滚雷
        CREEPING_BARRAGE = mp_weapon_grenade_creeping_bombardment // 蠕行轰炸
    };

    // 寻血猎犬技能
    enum Bloodhound
    {
        EYE_OF_THE_HUNT = mp_ability_hunt_mode,     // 猎人之眼
        AREA_SCAN = mp_ability_area_sonar_scan      // 区域扫描
    };

    // 直布罗陀技能
    enum Gibraltar
    {
        DOME_SHIELD = mp_weapon_bubble_bunker,        // 圆顶护罩
        GUN_SHIELD = mp_ability_gibraltar_shield,     // 枪盾
        DEFENSIVE_BOMBARDMENT = mp_weapon_grenade_defensive_bombardment // 防御轰炸
    };

    // 侵蚀技能
    enum Caustic
    {
        GAS_GRENADE = mp_weapon_grenade_gas,      // 毒气手雷
        GAS_TRAP = mp_weapon_dirty_bomb           // 毒气陷阱
    };

    // 命脉技能
    enum Lifeline
    {
        HEALTH_DRONE = mp_ability_lifeline_halo,           // 治疗无人机
        FOLLOW_MEDIC = mp_ability_lifeline_follow_medic,   // 跟随医疗
        CARE_PACKAGE = mp_weapon_following_medic_orders    // 护理包
    };

    // 探路者技能
    enum Pathfinder
    {
        GRAPPLE = mp_ability_grapple,   // 抓钩
        ZIPLINE = mp_weapon_zipline     // 滑索
    };

    // 幻影技能
    enum Mirage
    {
        HOLOPILOT = mp_ability_holopilot,        // 全息诱饵
        ULTIMATE_DECOY = mp_ability_mirage_ultimate // 全息宴会
    };

    // 幻影技能
    enum Wraith
    {
        INTO_THE_VOID = mp_ability_phase_walk,  // 进入虚空
        DIMENSIONAL_RIFT = mp_weapon_phase_tunnel // 次元裂缝
    };

    // 奥克坦技能
    enum Octane
    {
        STIM = mp_ability_octane_stim,  // 兴奋剂
        LAUNCH_PAD = mp_weapon_jump_pad // 弹跳垫
    };

    // 华森技能
    enum Wattson
    {
        PERIMETER_SECURITY = mp_weapon_tesla_trap,        // 周界安全
        INTERCEPTION_PYLON = mp_weapon_trophy_defense_system // 拦截塔
    };

    // 密客技能
    enum Crypto
    {
        DRONE = mp_ability_crypto_drone,     // 侦察无人机
        DRONE_EMP = mp_ability_crypto_drone_emp // 无人机EMP
    };

    // 亡灵技能
    enum Revenant
    {
        SHADOW_POUNCE = mp_ability_revenant_shadow_pounce_free, // 暗影扑击
        DEATH_TOTEM = mp_ability_shadow_form                    // 死亡图腾
    };

    // 罗芭技能
    enum Loba
    {
        BURGLAR_BEST_FRIEND = mp_ability_translocation, // 跃迁手环
        BLACK_MARKET = mp_ability_black_market          // 黑市精品
    };

    // 兰伯特技能
    enum Rampart
    {
        AMPED_WALL = mp_weapon_cover_wall,  // 加强掩体
        SHEILA = mp_weapon_mobile_hmg       // 希拉重机枪
    };

    // 地平线技能
    enum Horizon
    {
        GRAVITY_LIFT = mp_ability_horizon_tac_space_elevator, // 重力电梯
        BLACK_HOLE = mp_ability_horizon_black_hole            // 黑洞
    };

    // 暴雷技能
    enum Fuse
    {
        KNUCKLE_CLUSTER = mp_weapon_cluster_bomb,         // 指节集束炸弹
        THE_MOTHERLODE = mp_weapon_cluster_bomb_launcher  // 巨型集束炸弹
    };

    // 瓦尔基里技能
    enum Valkyrie
    {
        VTOL_JETS = mp_ability_valk_jets,              // 垂直起降喷射器
        MISSILE_SWARM = mp_ability_valk_cluster_missile, // 导弹群
        SKYWARD_DIVE = mp_ability_valk_skyward          // 苍穹俯冲
    };

    // 希尔技能
    enum Seer
    {
        FOCUS_OF_ATTENTION = mp_ability_heartbeat_sensor, // 注目
        ECHO_LOCATE = mp_ability_echo_locator,           // 回声定位
        EXHIBIT = mp_ability_sniper_ult                  // 微型无人机展示
    };

    // 艾许技能
    enum Ash
    {
        ARC_SNARE = mp_weapon_arc_bolt,       // 电弧陷阱
        PHASE_BREACH = mp_weapon_phase_breach  // 相位突破
    };

    // 疯玛吉技能
    enum MadMaggie
    {
        RIOT_DRILL = mp_weapon_riot_drill,     // 暴乱钻头
        WRECKING_BALL = mp_ability_wrecking_ball // 破坏球
    };

    // 纽卡斯尔技能
    enum Newcastle
    {
        MOBILE_SHIELD = mp_ability_shield_throw,  // 移动护盾
        CASTLE_WALL = mp_weapon_ferro_wall,       // 城堡墙
        RETRIEVE_THE_WOUNDED = mp_ability_armored_leap // 救治伤员
    };

    // 万蒂奇技能
    enum Vantage
    {
        ECHO_RELOCATION = mp_weapon_vantage_recall,     // 埃科重新定位
        SNIPER_MARK = mp_ability_companion_launch,      // 狙击标记
        COMPANION_LAUNCH = mp_ability_companion_launch_entry // 伙伴发射
    };

    // 催化剂技能
    enum Catalyst
    {
        PIERCING_SPIKES = mp_ability_spike_strip,  // 尖刺阵
        DARK_VEIL = mp_ability_phase_door          // 暗幕
    };

    // 巴拉尔技能
    enum Ballistic
    {
        WHISTLER = mp_ability_portable_auto_loader,  // 口哨手枪
        SLING = mp_ability_debuff_zone               // 减益区域
    };

    // 电导体技能
    enum Conduit
    {
        RADIANT_PULSE = mp_ability_conduit_arc_flash,       // 辐射脉冲
        SHIELD_MINES = mp_ability_conduit_shield_mines,     // 护盾地雷
        SHIELD_MINE_LINE = mp_ability_conduit_shield_mines_line // 护盾地雷线
    };

    // 霍里森技能
    enum Horizon1
    {
        SONIC_BOOM = mp_ability_sonic_blast        // 音速爆炸
    };

    // 伊莫茨技能
    enum Emote
    {
        EMOTE_PROJECTOR = mp_ability_emote_projector  // 表情投影仪
    };

    // 欧弗德莱夫技能
    enum Override
    {
        DRONE_RIDER = mp_ability_drone_rider  // 无人机骑手
    };

    // 超新星技能
    enum Nova
    {
        REINFORCE = mp_ability_reinforce  // 增强
    };

    // 抄袭套件 - 复制能力
    enum CopycatKit
    {
        BLOODHOUND_SONAR = mp_ability_copycat_kit_bloodhound_sonar,  // 复制寻血猎犬扫描
        HORIZON_ELEVATOR = mp_ability_copycat_kit_horizon_elevator,   // 复制地平线电梯
        MIRAGE_HOLOPILOT = mp_ability_copycat_kit_mirage_holopilot,   // 复制幻象诱饵
        WRAITH_PHASE = mp_ability_copycat_kit_wraith_phase_walk,      // 复制幻影相位
        CATALYST_WALL = mp_ability_copycat_kit_catalyst_wall,         // 复制催化剂墙
        OCTANE_PAD = mp_ability_copycat_kit_octane_pad,               // 复制奥克坦跳垫
        ASH_PHASE_BLADE = mp_ability_copycat_kit_ash_phase_blade,     // 复制艾许相位刀
        CAUSTIC_GAS = mp_ability_copycat_kit_caustic_grenade_gas      // 复制侵蚀毒气
    };

    // 增强套件
    enum BoostKit
    {
        MAGIC_PIXEL = mp_ability_boost_kit_magic_pixel,       // 魔法像素
        PILOT_HOVER = mp_ability_boost_kit_pilot_hover,       // 飞行员悬浮
        LIFESTEAL = mp_ability_boost_kit_lifesteal_kit,       // 生命窃取
        NESSIE_SUMMON = mp_ability_boost_kit_nessie_summon    // 尼斯湖水怪召唤
    };

    // 通用能力
    enum GenericAbilities
    {
        GENERIC_OFFHAND = mp_ability_generic_offhand,         // 通用副手
        REVIVE_SHIELD = mp_weapon_revive_shield,              // 复活护盾
        INCAP_SHIELD = mp_weapon_incap_shield,                // 倒地护盾
        PANIC_BUTTON = mp_ability_panic_button,               // 应急按钮
        ITEM_SPAWNER = mp_ability_item_spawner,               // 物品生成器
        CONSUMABLE = mp_ability_consumable,                   // 消耗品
        SECOND_CHANCE = mp_ability_second_chance_token,       // 第二次机会
        TRANSPORT_PORTAL = mp_ability_transport_portal,       // 传送门
        PORTAL_DATAPAD = mp_ability_transport_portal_datapad, // 传送门数据板
        VOID_RING = mp_ability_void_ring,                     // 虚空环
        RESPAWN_BEACON = mp_ability_mobile_respawn_beacon,    // 移动重生信标
        REDEPLOY_BALLOON = mp_ability_redeploy_balloon        // 重新部署气球
    };

    // 其他特殊武器
    enum SpecialAbilities
    {
        CUBEMAP = weapon_cubemap,                                // 立方体贴图
        MORTAR_RING = mp_weapon_mortar_ring,                     // 迫击炮环
        MORTAR_MISSILE = mp_weapon_mortar_ring_missle,           // 迫击炮导弹
        BOMBARDMENT_WEAPON = mp_weapon_defensive_bombardment_weapon, // 轰炸武器
        CREEPING_BOMBARDMENT = mp_weapon_creeping_bombardment_weapon // 蠕行轰炸武器
    };
}

// ----------------------
// 其他分类定义
// ----------------------

// 特殊物品和事件
namespace SpecialItems
{
    enum Items
    {
        // 特殊物品
        MOBILE_RESPAWN = mp_ability_mobile_respawn_beacon,  // 移动重生信标
        SECOND_CHANCE_TOKEN = mp_ability_second_chance_token, // 第二次机会令牌
        CONSUMABLE_ITEM = mp_ability_consumable           // 可消耗物品
    };
}