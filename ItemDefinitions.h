#pragma once

#include <unordered_map>

struct Item_t {
	Item_t(const char* display_name, const char* entity_name, const char* model, const char* killicon = nullptr) {
		this->display_name = display_name;
		this->entity_name = entity_name;
		this->model = model;
		this->killicon = killicon;
	}

	const char* display_name = nullptr;
	const char* entity_name = nullptr;
	const char* model = nullptr;
	const char* killicon = nullptr;
};

const std::unordered_map<size_t, Item_t> ItemDefinitionIndex = {
	{ 1,{ "Desert Eagle", "WEAPON_DEAGLE", "models/weapons/v_pist_deagle.mdl", "deagle" } },
	{ 2,{ "Dual Berettas", "WEAPON_ELITE", "models/weapons/v_pist_elite.mdl", "elite" } },
	{ 3,{ "Five-SeveN", "WEAPON_FIVESEVEN", "models/weapons/v_pist_fiveseven.mdl", "fiveseven" } },
	{ 4,{ "Glock-18", "WEAPON_GLOCK", "models/weapons/v_pist_glock18.mdl", "glock" } },
	{ 7,{ "AK-47", "WEAPON_AK47", "models/weapons/v_rif_ak47.mdl", "ak47" } },
	{ 8,{ "AUG", "WEAPON_AUG", "models/weapons/v_rif_aug.mdl", "aug" } },
	{ 9,{ "AWP", "WEAPON_AWP", "models/weapons/v_snip_awp.mdl", "awp" } },
	{ 10,{ "FAMAS", "WEAPON_FAMAS", "models/weapons/v_rif_famas.mdl", "famas" } },
	{ 11,{ "G3SG1", "WEAPON_G3SG1", "models/weapons/v_snip_g3sg1.mdl", "g3sg1" } },
	{ 13,{ "Galil AR", "WEAPON_GALILAR", "models/weapons/v_rif_galilar.mdl", "galilar" } },
	{ 14,{ "M249", "WEAPON_M249", "models/weapons/v_mach_m249para.mdl", "m249" } },
	{ 16,{ "M4A4", "WEAPON_M4A1", "models/weapons/v_rif_m4a1.mdl", "m4a1" } },
	{ 17,{ "MAC-10", "WEAPON_MAC10", "models/weapons/v_smg_mac10.mdl", "mac10" } },
	{ 19,{ "P90", "WEAPON_P90", "models/weapons/v_smg_p90.mdl", "p90" } },
	{ 24,{ "UMP-45", "WEAPON_UMP45", "models/weapons/v_smg_ump45.mdl", "ump45" } },
	{ 25,{ "XM1014", "WEAPON_XM1014", "models/weapons/v_shot_xm1014.mdl", "xm1014" } },
	{ 26,{ "PP-Bizon", "WEAPON_BIZON", "models/weapons/v_smg_bizon.mdl", "bizon" } },
	{ 27,{ "MAG-7", "WEAPON_MAG7", "models/weapons/v_shot_mag7.mdl", "mag7" } },
	{ 28,{ "Negev", "WEAPON_NEGEV", "models/weapons/v_mach_negev.mdl", "negev" } },
	{ 29,{ "Sawed-Off", "WEAPON_SAWEDOFF", "models/weapons/v_shot_sawedoff.mdl", "sawedoff" } },
	{ 30,{ "Tec-9", "WEAPON_TEC9", "models/weapons/v_pist_tec9.mdl", "tec9" } },
	{ 31,{ "Zeus x27", "WEAPON_TASER", "models/weapons/v_eq_taser.mdl", "taser" } },
	{ 32,{ "P2000", "WEAPON_HKP2000", "models/weapons/v_pist_hkp2000.mdl", "hkp2000" } },
	{ 33,{ "MP7", "WEAPON_MP7", "models/weapons/v_smg_mp7.mdl", "mp7" } },
	{ 34,{ "MP9", "WEAPON_MP9", "models/weapons/v_smg_mp9.mdl", "mp9" } },
	{ 35,{ "Nova", "WEAPON_NOVA", "models/weapons/v_shot_nova.mdl", "nova" } },
	{ 36,{ "P250", "WEAPON_P250", "models/weapons/v_pist_p250.mdl", "p250" } },
	{ 38,{ "SCAR-20", "WEAPON_SCAR20", "models/weapons/v_snip_scar20.mdl", "scar20" } },
	{ 39,{ "SG 553", "WEAPON_SG556", "models/weapons/v_rif_sg556.mdl", "sg556" } },
	{ 40,{ "SSG 08", "WEAPON_SSG08", "models/weapons/v_snip_ssg08.mdl", "ssg08" } },
	{ 42,{ "Knife (Counter-Terrorists)", "WEAPON_KNIFE", "models/weapons/v_knife_default_ct.mdl", "knife_default_ct" } },
	{ 43,{ "Flashbang", "WEAPON_FLASHBANG", "models/weapons/v_eq_flashbang.mdl", "flashbang" } },
	{ 44,{ "HE Grenade", "WEAPON_HEGRENADE", "models/weapons/v_eq_fraggrenade.mdl", "hegrenade" } },
	{ 45,{ "Smoke Grenade", "WEAPON_SMOKEGRENADE", "models/weapons/v_eq_smokegrenade.mdl", "smokegrenade" } },
	{ 46,{ "Molotov", "WEAPON_MOLOTOV", "models/weapons/v_eq_molotov.mdl", "inferno" } },
	{ 47,{ "Decoy Grenade", "WEAPON_DECOY", "models/weapons/v_eq_decoy.mdl", "decoy" } },
	{ 48,{ "Incendiary Grenade", "WEAPON_INCGRENADE", "models/weapons/v_eq_incendiarygrenade.mdl", "inferno" } },
	{ 49,{ "C4 Explosive", "WEAPON_C4", "models/weapons/v_ied.mdl" } },
	{ 59,{ "Knife (Terrorists)", "WEAPON_KNIFE_T", "models/weapons/v_knife_default_t.mdl", "knife_t" } },
	{ 60,{ "M4A1-S", "WEAPON_M4A1_SILENCER", "models/weapons/v_rif_m4a1_s.mdl", "m4a1_silencer" } },
	{ 61,{ "USP-S", "WEAPON_USP_SILENCER", "models/weapons/v_pist_223.mdl", "usp_silencer" } },
	{ 63,{ "CZ75 Auto", "WEAPON_CZ75A", "models/weapons/v_pist_cz_75.mdl", "cz75a" } },
	{ 64,{ "R8 Revolver", "WEAPON_REVOLVER", "models/weapons/v_pist_revolver.mdl", "revolver" } },
	{ 500,{ "Bayonet", "WEAPON_KNIFE_BAYONET", "models/weapons/v_knife_bayonet.mdl", "bayonet" } },
	{ 505,{ "Flip Knife", "WEAPON_KNIFE_FLIP", "models/weapons/v_knife_flip.mdl", "knife_flip" } },
	{ 506,{ "Gut Knife", "WEAPON_KNIFE_GUT", "models/weapons/v_knife_gut.mdl", "knife_gut" } },
	{ 507,{ "Karambit", "WEAPON_KNIFE_KARAMBIT", "models/weapons/v_knife_karam.mdl", "knife_karambit" } },
	{ 508,{ "M9 Bayonet", "WEAPON_KNIFE_M9_BAYONET", "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet" } },
	{ 509,{ "Huntsman Knife", "WEAPON_KNIFE_TACTICAL", "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
	{ 512,{ "Falchion Knife", "WEAPON_KNIFE_FALCHION", "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion" } },
	{ 514,{ "Bowie Knife", "WEAPON_KNIFE_SURVIVAL_BOWIE", "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie" } },
	{ 515,{ "Butterfly Knife", "WEAPON_KNIFE_BUTTERFLY", "models/weapons/v_knife_butterfly.mdl", "knife_butterfly" } },
	{ 516,{ "Shadow Daggers", "WEAPON_KNIFE_PUSH", "models/weapons/v_knife_push.mdl", "knife_push" } }
};

// Currently unused -- only here for reference.
const std::unordered_map<size_t, std::string> EntityQuality = {
	{ 0, "Normal" },
	{ 1, "Genuine" },
	{ 2, "Vintage" },
	{ 3, "Unusual" },
	{ 5, "Community" },
	{ 6, "Developer" },
	{ 7, "Self-Made" },
	{ 8, "Customized" },
	{ 9, "Strange" },
	{ 10, "Completed" },
	{ 12, "Tournament" }
};