#include "Configuration.hpp"
#include "Variables.h"
#include <winerror.h>
#pragma warning( disable : 4091)
#include <ShlObj.h>
#include <string>
#include <sstream>
#include "CommonIncludes.h"
#include "ESP.h"
#include "Hooks.h"
#include "XorStr.hpp"

const char* GetWeaponName(int iWeaponID)
{
	switch (iWeaponID)
	{
	default:
		return "none";
	case WEAPON_DEAGLE:
		return "deagle";
	case WEAPON_DUALS:
		return "duals";
	case WEAPON_FIVE7:
		return "five7";
	case WEAPON_GLOCK:
		return "glock";
	case WEAPON_AK47:
		return "ak47";
	case WEAPON_AUG:
		return "aug";
	case WEAPON_AWP:
		return "awp";
	case WEAPON_FAMAS:
		return "famas";
	case WEAPON_G3SG1:
		return "g3sg1";
	case WEAPON_GALIL:
		return "galil";
	case WEAPON_M249:
		return "m249";
	case WEAPON_M4A1:
		return "m4a4";
	case WEAPON_MAC10:
		return "mac10";
	case WEAPON_P90:
		return "p90";
	case WEAPON_UMP45:
		return "ump45";
	case WEAPON_XM1014:
		return "xm1014";
	case WEAPON_BIZON:
		return "bizon";
	case WEAPON_MAG7:
		return "mag7";
	case WEAPON_NEGEV:
		return "negev";
	case WEAPON_SAWEDOFF:
		return "sawedoff";
	case WEAPON_TEC9:
		return "tec9";
	case WEAPON_TASER:
		return "taser";
	case WEAPON_USPS:
		return "usp-s";
	case WEAPON_MP7:
		return "mp7";
	case WEAPON_MP9:
		return "mp9";
	case WEAPON_NOVA:
		return "nova";
	case WEAPON_P250:
		return "p250";
	case WEAPON_SCAR20:
		return "scar20";
	case WEAPON_SG553:
		return "sg556";
	case WEAPON_SCOUT:
		return "ssg08";
	case WEAPON_M4A1S:
		return "m4a1-s";
	case WEAPON_P2000:
		return "p2000";
	case WEAPON_CZ75:
		return "cz75";
	case WEAPON_REVOLVER:
		return "revolver";
	case WEAPON_KNIFE_BAYONET:
		return "bayonet";
	case WEAPON_KNIFE_BUTTERFLY:
		return "butterfly";
	case WEAPON_KNIFE_FALCHION:
		return "falshion";
	case WEAPON_KNIFE_FLIP:
		return "flip";
	case WEAPON_KNIFE_GUT:
		return "gut";
	case WEAPON_KNIFE_KARAMBIT:
		return "karambit";
	case WEAPON_KNIFE_M9BAYONET:
		return "m9";
	case WEAPON_KNIFE_HUNTSMAN:
		return "huntsman";
	case WEAPON_KNIFE_BOWIE:
		return "bowie";
	case WEAPON_KNIFE_DAGGER:
		return "daggers";
	}
}

void CConfig::Setup()
{


	SetupValue(g_Options.Ragebot.MainSwitch, false, ("Ragebot"), ("RageToggle"));
	SetupValue(g_Options.Ragebot.Enabled, false, ("Ragebot"), ("Enabled"));
	SetupValue(g_Options.Ragebot.AutoFire, false, ("Ragebot"), ("AutoFire"));
	SetupValue(g_Options.Ragebot.FOV, 0.f, ("Ragebot"), ("FOV"));
	SetupValue(g_Options.Ragebot.Silent, false, ("Ragebot"), ("Silent"));
	SetupValue(g_Options.Ragebot.AutoPistol, false, ("Ragebot"), ("AutoPistol"));
	SetupValue(g_Options.Ragebot.KeyPress, 0, ("Ragebot"), ("Key"));
	SetupValue(g_Options.Visuals.ToggleKey, 0, ("Visuals"), ("ToggleKey"));
	SetupValue(g_Options.Visuals.SniperCrosshair, false, ("Visuals"), ("Sniper_CrosshairVisible"));

	SetupValue(g_Options.Ragebot.flipkey, 1, ("Ragebot"), ("FlipKey"));
	SetupValue(g_Options.Ragebot.EnabledAntiAim, false, ("Ragebot"), ("AntiaimEnabled"));
	SetupValue(g_Options.Ragebot.PreAAs, false, ("Ragebot"), ("Pre-set-AAs"));
	SetupValue(g_Options.Ragebot.Pitch, 0, ("Ragebot"), ("AntiaimPitch"));
	SetupValue(g_Options.Ragebot.YawTrue, 0, ("Ragebot"), ("AntiaimYaw-true"));
	SetupValue(g_Options.Ragebot.YawFake, 0, ("Ragebot"), ("AntiaimYaw-fake"));
	SetupValue(g_Options.Ragebot.AtTarget, false, ("Ragebot"), ("attargets"));
	SetupValue(g_Options.Ragebot.Edge, false, ("Ragebot"), ("edge"));
	SetupValue(g_Options.Ragebot.KnifeAA, false, ("Ragebot"), ("KnifeAA"));
	SetupValue(g_Options.Ragebot.BuilderAAs, false, ("Ragebot"), ("Builder-AAs"));
	SetupValue(g_Options.Ragebot.BuilderPitch, 0, ("Ragebot"), ("Builder-Pitch"));
	SetupValue(g_Options.Ragebot.BuilderReal, 0, ("Ragebot"), ("Builder-Yaw"));
	SetupValue(g_Options.Ragebot.BuilderFake, 0, ("Ragebot"), ("Builder-Fake"));
	SetupValue(g_Options.Ragebot.Jitter, false, ("Ragebot"), ("Builder-Jitter"));
	SetupValue(g_Options.Ragebot.JitterRange, 0, ("Ragebot"), ("Builder-Jitter-Range"));
	SetupValue(g_Options.Ragebot.FJitter, false, ("Ragebot"), ("Builder-fJitter"));
	SetupValue(g_Options.Ragebot.FJitterRange, 0, ("Ragebot"), ("Builder-fJitter-Range"));
	SetupValue(g_Options.Ragebot.LBYBreaker, false, ("Ragebot"), ("Builder-LBY-Breaker"));
	SetupValue(g_Options.Ragebot.FakeLag, false, ("Ragebot"), ("Fakelag"));
	SetupValue(g_Options.Ragebot.FakeLagAmt, 0, ("Ragebot"), ("Fakelag Amount"));

	SetupValue(g_Options.Ragebot.FriendlyFire, false, ("Ragebot"), ("FriendlyFire"));
	SetupValue(g_Options.Ragebot.Hitbox, 0, ("Ragebot"), ("Hitbox"));
	SetupValue(g_Options.Ragebot.Hitscan, 0, ("Ragebot"), ("Hitscan"));

	SetupValue(g_Options.Ragebot.AntiRecoil, false, ("Ragebot"), ("AntiRecoil"));
	SetupValue(g_Options.Ragebot.AutoStop, false, ("Ragebot"), ("AutoStop"));
	SetupValue(g_Options.Ragebot.AutoCrouch, false, ("Ragebot"), ("AutoCrouch"));
	SetupValue(g_Options.Ragebot.AutoScope, false, ("Ragebot"), ("AutoScope"));
	SetupValue(g_Options.Ragebot.MinimumDamageSniper, 0.f, ("Ragebot"), ("AutoWallDamageSniper"));
	SetupValue(g_Options.Ragebot.MinimumDamagePistol, 0.f, ("Ragebot"), ("AutoWallDamagePistol"));
	SetupValue(g_Options.Ragebot.MinimumDamageRifle, 0.f, ("Ragebot"), ("AutoWallDamageRifle"));
	SetupValue(g_Options.Ragebot.MinimumDamageHeavy, 0.f, ("Ragebot"), ("AutoWallDamageHeavy"));
	SetupValue(g_Options.Ragebot.MinimumDamageSmg, 0.f, ("Ragebot"), ("AutoWallDamageSmgs"));
	SetupValue(g_Options.Ragebot.MinimumDamageRevolver, 0.f, ("Ragebot"), ("AutoWallDamageRevolver"));
	SetupValue(g_Options.Ragebot.Hitchance, false, ("Ragebot"), ("HitChance"));
	SetupValue(g_Options.Ragebot.HitchanceSniper, 0.f, ("Ragebot"), ("HitChanceSniper"));
	SetupValue(g_Options.Ragebot.HitchancePistol, 0.f, ("Ragebot"), ("HitChancePistol"));
	SetupValue(g_Options.Ragebot.HitchanceHeavy, 0.f, ("Ragebot"), ("HitChanceHeavy"));
	SetupValue(g_Options.Ragebot.HitchanceSmgs, 0.f, ("Ragebot"), ("HitChanceSmgs"));
	SetupValue(g_Options.Ragebot.HitchanceRifle, 0.f, ("Ragebot"), ("HitChanceRifle"));
	SetupValue(g_Options.Ragebot.HitchanceRevolver, 0.f, ("Ragebot"), ("HitChanceRevolver"));
	SetupValue(g_Options.Ragebot.Resolver, false, ("Ragebot"), ("Resolver"));
	SetupValue(g_Options.LegitBot.legitresolver, false, ("LegitBot"), ("legitresolver"));
	SetupValue(g_Options.Ragebot.FakeLagFix, false, ("Ragebot"), ("Fakelag Fix"));
	SetupValue(g_Options.Ragebot.PosAdjust, false, ("Ragebot"), ("Position Adjustment"));
	SetupValue(g_Options.Ragebot.BAIMkey, 0, ("Ragebot"), ("BAIMKey"));


	//new aa stuff going here
	SetupValue(g_Options.Ragebot.AA_onCrouch, false, ("RageBot"), ("AACrouch"));
	SetupValue(g_Options.Ragebot.AA_onFakeWalk, false, ("RageBot"), ("AAFakeWalk"));
	SetupValue(g_Options.Ragebot.AA_onRun, false, ("RageBot"), ("AARun"));
	SetupValue(g_Options.Ragebot.AA_onWalk, false, ("RageBot"), ("AAWalk"));
	SetupValue(g_Options.Ragebot.AA_onStand, false, ("RageBot"), ("AAStand"));


	//walkiing builder aa's
	SetupValue(g_Options.Ragebot.walk_BuilderAAs, false, ("RageBot"), ("walk_BuilderAAs"));
	SetupValue(g_Options.Ragebot.walk_Jitter, false, ("RageBot"), ("walk_Jitter"));
	SetupValue(g_Options.Ragebot.walk_FJitter, false, ("RageBot"), ("walk_FJitter"));
	SetupValue(g_Options.Ragebot.walk_LBYBreaker, false, ("RageBot"), ("walk_LBYBreaker"));
	SetupValue(g_Options.Ragebot.walk_BuilderReal, 0, ("RageBot"), ("walk_BuilderReal"));
	SetupValue(g_Options.Ragebot.walk_BuilderFake, 0, ("RageBot"), ("walk_BuilderFake"));
	SetupValue(g_Options.Ragebot.walk_BuilderPitch, 0, ("RageBot"), ("walk_BuilderPitch"));
	SetupValue(g_Options.Ragebot.walk_JitterRange, 0, ("RageBot"), ("walk_JitterRange"));
	SetupValue(g_Options.Ragebot.walk_FJitterRange, 0, ("RageBot"), ("walk_FJitterRange"));

	//ruinning builder aa's
	SetupValue(g_Options.Ragebot.run_BuilderAAs, false, ("RageBot"), ("run_BuilderAAs"));
	SetupValue(g_Options.Ragebot.run_Jitter, false, ("RageBot"), ("run_Jitter"));
	SetupValue(g_Options.Ragebot.run_FJitter, false, ("RageBot"), ("run_FJitter"));
	SetupValue(g_Options.Ragebot.run_LBYBreaker, false, ("RageBot"), ("run_LBYBreaker"));
	SetupValue(g_Options.Ragebot.run_BuilderReal, 0, ("RageBot"), ("run_BuilderReal"));
	SetupValue(g_Options.Ragebot.run_BuilderFake, 0, ("RageBot"), ("run_BuilderFake"));
	SetupValue(g_Options.Ragebot.run_BuilderPitch, 0, ("RageBot"), ("run_BuilderPitch"));
	SetupValue(g_Options.Ragebot.run_JitterRange, 0, ("RageBot"), ("run_JitterRange"));
	SetupValue(g_Options.Ragebot.run_FJitterRange, 0, ("RageBot"), ("run_FJitterRange"));

	//crouching builder aa's
	SetupValue(g_Options.Ragebot.crouch_BuilderAAs, false, ("RageBot"), ("crouch_BuilderAAs"));
	SetupValue(g_Options.Ragebot.crouch_Jitter, false, ("RageBot"), ("crouch_Jitter"));
	SetupValue(g_Options.Ragebot.crouch_FJitter, false, ("RageBot"), ("crouch_FJitter"));
	SetupValue(g_Options.Ragebot.crouch_LBYBreaker, false, ("RageBot"), ("crouch_LBYBreaker"));
	SetupValue(g_Options.Ragebot.crouch_BuilderReal, 0, ("RageBot"), ("crouch_BuilderReal"));
	SetupValue(g_Options.Ragebot.crouch_BuilderFake, 0, ("RageBot"), ("crouch_BuilderFake"));
	SetupValue(g_Options.Ragebot.crouch_BuilderPitch, 0, ("RageBot"), ("crouch_BuilderPitch"));
	SetupValue(g_Options.Ragebot.crouch_JitterRange, 0, ("RageBot"), ("crouch_JitterRange"));
	SetupValue(g_Options.Ragebot.crouch_FJitterRange, 0, ("RageBot"), ("crouch_FJitterRange"));

	//fakewalk builder aa's
	SetupValue(g_Options.Ragebot.fwalk_BuilderAAs, false, ("RageBot"), ("fwalk_BuilderAAs"));
	SetupValue(g_Options.Ragebot.fwalk_Jitter, false, ("RageBot"), ("fwalk_Jitter"));
	SetupValue(g_Options.Ragebot.fwalk_FJitter, false, ("RageBot"), ("fwalk_FJitter"));
	SetupValue(g_Options.Ragebot.fwalk_LBYBreaker, false, ("RageBot"), ("fwalk_LBYBreaker"));
	SetupValue(g_Options.Ragebot.fwalk_BuilderReal, 0, ("RageBot"), ("fwalk_BuilderReal"));
	SetupValue(g_Options.Ragebot.fwalk_BuilderFake, 0, ("RageBot"), ("fwalk_BuilderFake"));
	SetupValue(g_Options.Ragebot.fwalk_BuilderPitch, 0, ("RageBot"), ("fwalk_BuilderPitch"));
	SetupValue(g_Options.Ragebot.fwalk_JitterRange, 0, ("RageBot"), ("fwalk_JitterRange"));
	SetupValue(g_Options.Ragebot.fwalk_FJitterRange, 0, ("RageBot"), ("fwalk_FJitterRange"));

	//standing builder aa's
	SetupValue(g_Options.Ragebot.stand_BuilderAAs, false, ("RageBot"), ("stand_BuilderAAs"));
	SetupValue(g_Options.Ragebot.stand_Jitter, false, ("RageBot"), ("stand_Jitter"));
	SetupValue(g_Options.Ragebot.stand_FJitter, false, ("RageBot"), ("stand_FJitter"));
	SetupValue(g_Options.Ragebot.stand_LBYBreaker, false, ("RageBot"), ("stand_LBYBreaker"));
	SetupValue(g_Options.Ragebot.stand_BuilderReal, 0, ("RageBot"), ("stand_BuilderReal"));
	SetupValue(g_Options.Ragebot.stand_BuilderFake, 0, ("RageBot"), ("stand_BuilderFake"));
	SetupValue(g_Options.Ragebot.stand_BuilderPitch, 0, ("RageBot"), ("stand_BuilderPitch"));
	SetupValue(g_Options.Ragebot.stand_JitterRange, 0, ("RageBot"), ("stand_JitterRange"));
	SetupValue(g_Options.Ragebot.stand_FJitterRange, 0, ("RageBot"), ("stand_FJitterRange"));


	//BEGINNING OF PREBUILT AA'S

	//walking prebuild aa's  walk
	SetupValue(g_Options.Ragebot.walk_PreAAs, false, ("RageBot"), ("walk_PreAAs"));
	SetupValue(g_Options.Ragebot.walk_Pitch, false, ("RageBot"), ("walk_Pitch"));
	SetupValue(g_Options.Ragebot.walk_PitchAdder, 0, ("RageBot"), ("walk_PitchAdder"));
	SetupValue(g_Options.Ragebot.walk_YawTrue, false, ("RageBot"), ("walk_YawTrue"));
	SetupValue(g_Options.Ragebot.walk_YawTrueAdder, 0, ("RageBot"), ("walk_YawTrueAdder"));
	SetupValue(g_Options.Ragebot.walk_YawFake, false, ("RageBot"), ("walk_YawFake"));
	SetupValue(g_Options.Ragebot.walk_YawFakeAdder, 0, ("RageBot"), ("walk_YawFakeAdder"));

	//running prebuild aa's
	SetupValue(g_Options.Ragebot.run_PreAAs, false, ("RageBot"), ("run_PreAAs"));
	SetupValue(g_Options.Ragebot.run_Pitch, false, ("RageBot"), ("run_Pitch"));
	SetupValue(g_Options.Ragebot.run_PitchAdder, 0, ("RageBot"), ("run_PitchAdder"));
	SetupValue(g_Options.Ragebot.run_YawTrue, false, ("RageBot"), ("run_YawTrue"));
	SetupValue(g_Options.Ragebot.run_YawTrueAdder, 0, ("RageBot"), ("run_YawTrueAdder"));
	SetupValue(g_Options.Ragebot.run_YawFake, false, ("RageBot"), ("run_YawFake"));
	SetupValue(g_Options.Ragebot.run_YawFakeAdder, 0, ("RageBot"), ("run_YawFakeAdder"));

	//crouching prebuild aa's crouch
	SetupValue(g_Options.Ragebot.crouch_PreAAs, false, ("RageBot"), ("_PreAAs"));
	SetupValue(g_Options.Ragebot.crouch_Pitch, false, ("RageBot"), ("_Pitch"));
	SetupValue(g_Options.Ragebot.crouch_PitchAdder, 0, ("RageBot"), ("_PitchAdder"));
	SetupValue(g_Options.Ragebot.crouch_YawTrue, false, ("RageBot"), ("_YawTrue"));
	SetupValue(g_Options.Ragebot.crouch_YawTrueAdder, 0, ("RageBot"), ("_YawTrueAdder"));
	SetupValue(g_Options.Ragebot.crouch_YawFake, false, ("RageBot"), ("_YawFake"));
	SetupValue(g_Options.Ragebot.crouch_YawFakeAdder, 0, ("RageBot"), ("_YawFakeAdder"));

	//fakewalking prebuild aa's fwalk
	SetupValue(g_Options.Ragebot.fwalk_PreAAs, false, ("RageBot"), ("fwalk_PreAAs"));
	SetupValue(g_Options.Ragebot.fwalk_Pitch, false, ("RageBot"), ("fwalk_Pitch"));
	SetupValue(g_Options.Ragebot.fwalk_PitchAdder, 0, ("RageBot"), ("fwalk_PitchAdder"));
	SetupValue(g_Options.Ragebot.fwalk_YawTrue, false, ("RageBot"), ("fwalk_YawTrue"));
	SetupValue(g_Options.Ragebot.fwalk_YawTrueAdder, 0, ("RageBot"), ("fwalk_YawTrueAdder"));
	SetupValue(g_Options.Ragebot.fwalk_YawFake, false, ("RageBot"), ("fwalk_YawFake"));
	SetupValue(g_Options.Ragebot.fwalk_YawFakeAdder, 0, ("RageBot"), ("fwalk_YawFakeAdder"));

	//standing prebuild aa's  stand
	SetupValue(g_Options.Ragebot.stand_PreAAs, false, ("RageBot"), ("stand_PreAAs"));
	SetupValue(g_Options.Ragebot.stand_Pitch, false, ("RageBot"), ("stand_Pitch"));
	SetupValue(g_Options.Ragebot.stand_PitchAdder, 0, ("RageBot"), ("stand_PitchAdder"));
	SetupValue(g_Options.Ragebot.stand_YawTrue, false, ("RageBot"), ("stand_YawTrue"));
	SetupValue(g_Options.Ragebot.stand_YawTrueAdder, 0, ("RageBot"), ("stand_YawTrueAdder"));
	SetupValue(g_Options.Ragebot.stand_YawFake, false, ("RageBot"), ("stand_YawFake"));
	SetupValue(g_Options.Ragebot.stand_YawFakeAdder, 0, ("RageBot"), ("stand_YawFakeAdder"));

	SetupValue(g_Options.Ragebot.walk_allowflip, false, ("RageBot"), ("walk_allowflip"));
	SetupValue(g_Options.Ragebot.run_allowflip, false, ("RageBot"), ("run_allowflip"));
	SetupValue(g_Options.Ragebot.crouch_allowflip, false, ("RageBot"), ("crouch_allowflip"));
	SetupValue(g_Options.Ragebot.fwalk_allowflip, false, ("RageBot"), ("fwalk_allowflip"));
	SetupValue(g_Options.Ragebot.stand_allowflip, false, ("RageBot"), ("stand_allowflip"));

	//SetupValue(g_Options.Visuals.SniperCrosshair, false, ("Visuals"), ("Sniper_CrosshairVisible"));

	SetupValue(g_Options.Misc.Bhop, 0, ("Misc"), ("Bhop"));
	SetupValue(g_Options.Visuals.SniperCrosshair, false, ("Visuals"), ("Sniper_CrosshairVisible"));

	// legitbot

	SetupValue(g_Options.LegitBot.SmokeCheck, false, ("Legitbot"), ("SmokeCheck"));
	SetupValue(g_Options.LegitBot.AutoPistol, false, ("Legitbot"), ("Auto_Pistol"));
	//LEGITBOT
	SetupValue(g_Options.Backtrack.backtrackenable, false, ("Legitbot"), ("Backtrack"));
	SetupValue(g_Options.Backtrack.backtrackhistory, false, ("Legitbot"), ("Backtrackhistory"));
	SetupValue(g_Options.Backtrack.backtrackticks, 0, ("Legitbot"), ("BacktrackTicks"));


	SetupValue(g_Options.LegitBot.MainKey, 1, ("Legitbot"), ("Key-Rifles"));
	SetupValue(g_Options.LegitBot.Mainfov, 0.f, ("Legitbot"), ("FOV-Rifles"));
	SetupValue(g_Options.LegitBot.MainSmooth, 1.f, ("Legitbot"), ("Smooth-Rifles"));
	SetupValue(g_Options.LegitBot.main_recoil_min, 0, ("Legitbot"), ("RCS-min-Rifles"));
	SetupValue(g_Options.LegitBot.main_recoil_max, 0, ("Legitbot"), ("RCS-max-Rifles"));

	SetupValue(g_Options.LegitBot.PistolKey, 6, ("Legitbot"), ("Key-Pistol"));
	SetupValue(g_Options.LegitBot.Pistolfov, 0.f, ("Legitbot"), ("FOV-Pistol"));
	SetupValue(g_Options.LegitBot.PistolSmooth, 1.f, ("Legitbot"), ("Smooth-Pistol"));
	SetupValue(g_Options.LegitBot.pistol_recoil_min, 0, ("Legitbot"), ("RCS-min-pistol"));
	SetupValue(g_Options.LegitBot.pistol_recoil_max, 0, ("Legitbot"), ("RCS-max-pistol"));

	SetupValue(g_Options.LegitBot.SniperKey, 6, ("Legitbot"), ("Key-Sniper"));
	SetupValue(g_Options.LegitBot.Sniperfov, 0.f, ("Legitbot"), ("FOV-Sniper"));
	SetupValue(g_Options.LegitBot.SniperSmooth, 0.f, ("Legitbot"), ("Smooth-Sniper"));
	SetupValue(g_Options.LegitBot.sniper_recoil_min, 0, ("Legitbot"), ("RCS-min-sniper"));
	SetupValue(g_Options.LegitBot.sniper_recoil_max, 0, ("Legitbot"), ("RCS-max-sniper"));


	SetupValue(g_Options.LegitBot.DeagleKey, 6, ("Legitbot"), ("Key-Deagle"));
	SetupValue(g_Options.LegitBot.Deaglefov, 0.f, ("Legitbot"), ("FOV-Deagle"));
	SetupValue(g_Options.LegitBot.DeagleSmooth, 0.f, ("Legitbot"), ("Smooth-Deagle"));
	SetupValue(g_Options.LegitBot.Deagle_recoil_min, 0, ("Legitbot"), ("RCS-min-Deagle"));
	SetupValue(g_Options.LegitBot.Deagle_recoil_max, 0, ("Legitbot"), ("RCS-max-Deagle"));


	SetupValue(g_Options.LegitBot.scoutKey, 6, ("Legitbot"), ("Key-Scout"));
	SetupValue(g_Options.LegitBot.scoutfov, 0.f, ("Legitbot"), ("FOV-Scout"));
	SetupValue(g_Options.LegitBot.scoutSmooth, 0.f, ("Legitbot"), ("Smooth-Scout"));
	//WEAPONS SETTS
	SetupValue(g_Options.LegitBot.FilterMainWeapons.Headwpmain, false, ("Legitbot"), ("Headwpmain"));
	SetupValue(g_Options.LegitBot.FilterMainWeapons.Neckwpmain, false, ("Legitbot"), ("Neckwpmain"));
	SetupValue(g_Options.LegitBot.FilterMainWeapons.Chestwpmain, false, ("Legitbot"), ("Chestwpmain"));
	SetupValue(g_Options.LegitBot.FilterMainWeapons.Stomachwpmain, false, ("Legitbot"), ("Stomachwpmain"));
	SetupValue(g_Options.LegitBot.FilterMainWeapons.multiboneswpmain, false, ("Legitbot"), ("multiboneswpmain"));
	//PISTOLS
	SetupValue(g_Options.LegitBot.FilterPistolWeapons.Headwppis, false, ("Legitbot"), ("Headwppis"));
	SetupValue(g_Options.LegitBot.FilterPistolWeapons.Neckwppis, false, ("Legitbot"), ("Neckwppis"));
	SetupValue(g_Options.LegitBot.FilterPistolWeapons.Chestwppis, false, ("Legitbot"), ("Chestwppis"));
	SetupValue(g_Options.LegitBot.FilterPistolWeapons.Stomachwppis, false, ("Legitbot"), ("Stomachwppis"));
	SetupValue(g_Options.LegitBot.FilterPistolWeapons.multiboneswppis, false, ("Legitbot"), ("multiboneswppis"));

	//SNIPER
	SetupValue(g_Options.LegitBot.FilterSniperWeapons.HeadwpSnip, false, ("Legitbot"), ("HeadwpSnip"));
	SetupValue(g_Options.LegitBot.FilterSniperWeapons.NeckwpSnip, false, ("Legitbot"), ("NeckwpSnip"));
	SetupValue(g_Options.LegitBot.FilterSniperWeapons.ChestwpSnip, false, ("Legitbot"), ("ChestwpSnip"));
	SetupValue(g_Options.LegitBot.FilterSniperWeapons.StomachwpSnip, false, ("Legitbot"), ("StomachwpSnip"));
	SetupValue(g_Options.LegitBot.FilterSniperWeapons.multiboneswpSnip, false, ("Legitbot"), ("multiboneswpSnip"));
	SetupValue(g_Options.Misc.Radar, false, ("Misc"), ("Radar"));

	SetupValue(g_Options.LegitBot.FilterScoutWeapons.HeadwpScout, false, ("Legitbot"), ("HeadwpScout"));
	SetupValue(g_Options.LegitBot.FilterScoutWeapons.NeckwpScout, false, ("Legitbot"), ("NeckwpScout"));
	SetupValue(g_Options.LegitBot.FilterScoutWeapons.ChestwpScout, false, ("Legitbot"), ("ChestwpScout"));
	SetupValue(g_Options.LegitBot.FilterScoutWeapons.StomachwpScout, false, ("Legitbot"), ("StomachwpScout"));
	SetupValue(g_Options.LegitBot.FilterScoutWeapons.multiboneswpScout, false, ("Legitbot"), ("multiboneswpScout"));
	//SMGWEAPONS

	SetupValue(g_Options.Misc.fakewalk, false, ("Misc"), ("fakewalk"));

	SetupValue(g_Options.LegitBot.FilterDeagleWeapons.HeadwpDeagle, false, ("Legitbot"), ("HeadwpDeagle"));
	SetupValue(g_Options.LegitBot.FilterDeagleWeapons.NeckwpDeagle, false, ("Legitbot"), ("NeckwpDeagle"));
	SetupValue(g_Options.LegitBot.FilterDeagleWeapons.ChestwpDeagle, false, ("Legitbot"), ("ChestwpDeagle"));
	SetupValue(g_Options.LegitBot.FilterDeagleWeapons.StomachwpDeagle, false, ("Legitbot"), ("StomachwpDeagle"));
	SetupValue(g_Options.LegitBot.FilterDeagleWeapons.multiboneswpDeagle, false, ("Legitbot"), ("multiboneswpDeagle"));

	SetupValue(g_Options.LegitBot.FiltersmgWeapons.Headwpsmg, false, ("Legitbot"), ("Headwpsmg"));
	SetupValue(g_Options.LegitBot.FiltersmgWeapons.Neckwpsmg, false, ("Legitbot"), ("Neckwpsmg"));
	SetupValue(g_Options.LegitBot.FiltersmgWeapons.Chestwpsmg, false, ("Legitbot"), ("Chestwpsmg"));
	SetupValue(g_Options.LegitBot.FiltersmgWeapons.Stomachwpsmg, false, ("Legitbot"), ("Stomachwpsmg"));
	SetupValue(g_Options.LegitBot.FiltersmgWeapons.multiboneswpsmg, false, ("Legitbot"), ("multiboneswpsmg"));
	SetupValue(g_Options.Misc.bFreeCam, false, ("Misc"), ("FreeCam-Enabled"));
	SetupValue(g_Options.Misc.iFreeCamKey, 0, ("Visuals"), ("FreeCam-Key"));
	SetupValue(g_Options.Misc.flFreeCamSpeed, 0.f, ("Visuals"), ("FreeCam-Speed"));

	//HEABYWEAPONS
	SetupValue(g_Options.LegitBot.FilterheavyWeapons.Headwphv, false, ("Legitbot"), ("Headwphv"));
	SetupValue(g_Options.LegitBot.FilterheavyWeapons.Neckwphv, false, ("Legitbot"), ("Neckwphv"));
	SetupValue(g_Options.LegitBot.FilterheavyWeapons.Chestwphv, false, ("Legitbot"), ("Chestwphv"));
	SetupValue(g_Options.LegitBot.FilterheavyWeapons.Stomachwphv, false, ("Legitbot"), ("Stomachwphv"));
	SetupValue(g_Options.LegitBot.FilterheavyWeapons.multibonewphv, false, ("Legitbot"), ("multibonewphv"));

	SetupValue(g_Options.Visuals.chamsHp, false, ("Visuals"), ("ChamsHp"));




	SetupValue(g_Options.Backtrack.backtrackenable, false, ("Legitbot"), ("Backtrack"));
	SetupValue(g_Options.Backtrack.backtrackhistory, false, ("Legitbot"), ("Backtrackhistory"));
	SetupValue(g_Options.Backtrack.backtrackticks, 0, ("Legitbot"), ("BacktrackTicks"));
	SetupValue(g_Options.Colors.backtrackdots_color[0], 0.0f, "Colors", "backtrackdots_color1");
	SetupValue(g_Options.Colors.backtrackdots_color[1], 0.0f, "Colors", "backtrackdots_color2");
	SetupValue(g_Options.Colors.backtrackdots_color[2], 0.0f, "Colors", "backtrackdots_color3");

	SetupValue(g_Options.LegitBot.backtrack, false, ("Legitbot"), ("Backtrack"));
	SetupValue(g_Options.LegitBot.backtrackTicks, 0, ("Legitbot"), ("BacktrackTicks"));

	SetupValue(g_Options.Visuals.backtrackline, false, ("Visuals"), ("backtrackline"));

	//TRIGGERBOT
	SetupValue(g_Options.LegitBot.Triggerbot.Enabled, false, ("Triggerbot"), ("Enabled"));
	SetupValue(g_Options.LegitBot.Triggerbot.Key, 6, ("Triggerbot"), ("Key"));
	SetupValue(g_Options.LegitBot.Triggerbot.Delay, 0.f, ("Triggerbot"), ("Delay"));
	SetupValue(g_Options.LegitBot.Triggerbot.hitchance, 0.f, ("Triggerbot"), ("Hitchance"));

	SetupValue(g_Options.LegitBot.Triggerbot.Filter.Head, false, ("Triggerbot"), ("FilterHead"));
	SetupValue(g_Options.LegitBot.Triggerbot.Filter.Chest, false, ("Triggerbot"), ("FilterChest"));
	SetupValue(g_Options.LegitBot.Triggerbot.Filter.Stomach, false, ("Triggerbot"), ("FilterStomach"));
	SetupValue(g_Options.LegitBot.Triggerbot.Filter.Arms, false, ("Triggerbot"), ("FilterArms"));
	SetupValue(g_Options.LegitBot.Triggerbot.Filter.Legs, false, ("Triggerbot"), ("FilterLegs"));

	SetupValue(g_Options.Visuals.Skeleton, false, ("Visuals"), ("Skeleton"));
	SetupValue(g_Options.Visuals.HasDefuser, false, ("Visuals"), ("HasDefuser"));
	SetupValue(g_Options.Visuals.IsScoped, false, ("Visuals"), ("IsScoped"));
	SetupValue(g_Options.Visuals.Enabled, false, ("Visuals"), ("VisualsEnabled"));
	SetupValue(g_Options.Visuals.Box, false, ("Visuals"), ("Box"));
	SetupValue(g_Options.Visuals.Name, false, ("Visuals"), ("Name"));
	SetupValue(g_Options.Visuals.HP, false, ("Visuals"), ("HP"));
	SetupValue(g_Options.Visuals.Weapon, false, ("Visuals"), ("Weapon"));
	SetupValue(g_Options.Visuals.LBYIndicator, false, ("Visuals"), ("LBY Indicator"));
	SetupValue(g_Options.Misc.FakePing2, false, ("Misc"), ("FakePing Amount"));

	SetupValue(g_Options.Visuals.backtrackline, false, ("Visuals"), ("backtrackline"));
	SetupValue(g_Options.Visuals.Glow, false, ("Visuals"), ("Glow"));
	SetupValue(g_Options.Visuals.GrenadeESP, false, ("Visuals"), ("GranadeESP"));
	SetupValue(g_Options.Visuals.GrenadePrediction, false, ("Visuals"), ("Granade Prediction"));
	SetupValue(g_Options.Visuals.RecoilCrosshair, false, ("Visuals"), ("RecoilCrosshair"));
	SetupValue(g_Options.Visuals.SpreadCrosshair, false, ("Visuals"), ("SpreadCrosshair"));
	SetupValue(g_Options.Visuals.NoVisualRecoil, false, ("Visuals"), ("NoVisualRecoil"));
	SetupValue(g_Options.Visuals.FOVChanger, 0.f, ("Visuals"), ("fovchanger"));
	SetupValue(g_Options.Visuals.viewmodelChanger, 68.f, ("Visuals"), ("viewmodel_fov"));
	SetupValue(g_Options.Visuals.Time, false, ("Visuals"), ("Time"));
	SetupValue(g_Options.Visuals.DLight, false, ("Visuals"), ("DLight"));
	SetupValue(g_Options.Visuals.C4, false, ("Visuals"), ("C4"));
	SetupValue(g_Options.Visuals.money, false, ("Visuals"), ("Money"));
	SetupValue(g_Options.Visuals.NoFlash, false, ("Visuals"), ("NoFlash"));
	SetupValue(g_Options.Visuals.NoSmoke, false, ("Visuals"), ("NoSmoke"));
	SetupValue(g_Options.Visuals.noscopeborder, false, ("Visuals"), ("1tapNoScope360"));
	SetupValue(g_Options.Visuals.Smoke, 0, ("Visuals"), ("Smoke"));
	SetupValue(g_Options.Visuals.angleLines, false, ("Visuals"), ("AngleLines"));

	SetupValue(g_Options.Colors.hands_alpha, 1.f, ("Visuals"), ("HandsAlpha"));
	SetupValue(g_Options.Visuals.Hands, 0, ("Visuals"), ("Hands"));

	SetupValue(g_Options.Colors.hands_color[0], 1.f, ("Colors"), ("HandsChams1Color"));
	SetupValue(g_Options.Colors.hands_color[1], 1.f, ("Colors"), ("HandsChams2Color"));
	SetupValue(g_Options.Colors.hands_color[2], 1.f, ("Colors"), ("HandsChams3Color"));

	SetupValue(g_Options.Visuals.Chams, false, ("Visuals"), ("PlayerChams"));
	SetupValue(g_Options.Visuals.Teamchams, false, ("Visuals"), ("Teamchams"));
	SetupValue(g_Options.Visuals.Chamweapon, false, ("Visuals"), ("WeaponChams"));
	SetupValue(g_Options.Visuals.XQZ, false, ("Visuals"), ("XQZ Chams"));
	SetupValue(g_Options.Visuals.champlayeralpha, 0, ("Visuals"), ("PlayerCham Alpha"));
	SetupValue(g_Options.Visuals.matierial, 0, ("Visuals"), ("PlayerCham Material"));
	SetupValue(g_Options.Visuals.weaponviewcham, false, ("Visuals"), ("Weapon Chams"));
	SetupValue(g_Options.Visuals.weaponhandalpha, 0, ("Visuals"), ("Weapon Chams Alpha"));

	SetupValue(g_Options.Colors.EnemyChamsNVis[0], 1.f, ("Colors"), ("EnemyChamsNVisRed"));
	SetupValue(g_Options.Colors.EnemyChamsNVis[1], 0.f, ("Colors"), ("EnemyChamsNVisGreen"));
	SetupValue(g_Options.Colors.EnemyChamsNVis[2], 0.f, ("Colors"), ("EnemyChamsNVisBlue"));

	SetupValue(g_Options.Colors.EnemyChamsVis[0], 0.f, ("Colors"), ("EnemyChamsVisRed"));
	SetupValue(g_Options.Colors.EnemyChamsVis[1], 1.f, ("Colors"), ("EnemyChamsVisGreen"));
	SetupValue(g_Options.Colors.EnemyChamsVis[2], 0.f, ("Colors"), ("EnemyChamsVisBlue"));

	SetupValue(g_Options.Colors.color_skeleton[0], 1.f, ("Colors"), ("Skeleton1Color"));
	SetupValue(g_Options.Colors.color_skeleton[1], 1.f, ("Colors"), ("Skeleton2Color"));
	SetupValue(g_Options.Colors.color_skeleton[2], 1.f, ("Colors"), ("Skeleton3Color"));

	SetupValue(g_Options.Colors.tracer_color[0], 0.f, ("Colors"), ("Tracer1Color"));
	SetupValue(g_Options.Colors.tracer_color[1], 1.f, ("Colors"), ("Tracer2Color"));
	SetupValue(g_Options.Colors.tracer_color[2], 0.f, ("Colors"), ("Tracer3Color"));

	SetupValue(g_Options.Colors.box_color_t[0], 1.f, ("Colors"), ("ColorBoxTR1Color"));
	SetupValue(g_Options.Colors.box_color_t[1], 0.f, ("Colors"), ("ColorBoxTR2Color"));
	SetupValue(g_Options.Colors.box_color_t[2], 0.f, ("Colors"), ("ColorBoxTR3Color"));

	SetupValue(g_Options.Colors.box_color_ct[0], 0.f, ("Colors"), ("ColorBoxCT1Color"));
	SetupValue(g_Options.Colors.box_color_ct[1], 0.f, ("Colors"), ("ColorBoxCT2Color"));
	SetupValue(g_Options.Colors.box_color_ct[2], 1.f, ("Colors"), ("ColorBoxCT3Color"));

	SetupValue(g_Options.Colors.color_spread[0], 0.f, ("Colors"), ("SpreadCrosshair1Color"));
	SetupValue(g_Options.Colors.color_spread[1], 1.f, ("Colors"), ("SpreadCrosshair2Color"));
	SetupValue(g_Options.Colors.color_spread[2], 0.f, ("Colors"), ("SpreadCrosshair3Color"));

	SetupValue(g_Options.Colors.color_recoil[0], 1.f, ("Colors"), ("RecoilCrosshair1Color"));
	SetupValue(g_Options.Colors.color_recoil[1], 0.f, ("Colors"), ("RecoilCrosshair2Color"));
	SetupValue(g_Options.Colors.color_recoil[2], 0.f, ("Colors"), ("RecoilCrosshair3Color"));

	SetupValue(g_Options.Colors.dlight_color[0], 0.f, ("Colors"), ("DynamicLight1Color"));
	SetupValue(g_Options.Colors.dlight_color[1], 1.f, ("Colors"), ("DynamicLight2Color"));
	SetupValue(g_Options.Colors.dlight_color[2], 0.f, ("Colors"), ("DynamicLight3Color"));

	SetupValue(g_Options.Colors.hitmark_color[0], 0.f, ("Colors"), ("hitmarker1Color"));
	SetupValue(g_Options.Colors.hitmark_color[1], 1.f, ("Colors"), ("hitmarker2Color"));
	SetupValue(g_Options.Colors.hitmark_color[2], 0.f, ("Colors"), ("hitmarker3Color"));

	SetupValue(g_Options.Colors.backtrackdots_color[0], 1.f, ("Colors"), ("backtrackdots1Color"));
	SetupValue(g_Options.Colors.backtrackdots_color[1], 0.f, ("Colors"), ("backtrackdots2Color"));
	SetupValue(g_Options.Colors.backtrackdots_color[2], 0.f, ("Colors"), ("backtrackdots3Color"));

	SetupValue(g_Options.Visuals.tpdist, false, ("Visuals"), ("thirdperson distance"));
	SetupValue(g_Options.Visuals.TPKey, false, ("Visuals"), ("thirdperson key"));
	SetupValue(g_Options.Visuals.ThirdPerson, false, ("Visuals"), ("thirdperson switch"));

	SetupValue(g_Options.Misc.fps, false, ("Visuals"), ("fpsboost"));
	SetupValue(g_Options.Misc.afkbot, false, ("Misc"), ("AFK-BOT"));


	SetupValue(g_Options.Visuals.Filter.Players, false, ("Visuals"), ("Players"));
	SetupValue(g_Options.Visuals.Filter.EnemyOnly, false, ("Visuals"), ("EnemyOnly"));
	SetupValue(g_Options.Visuals.WeaponsWorld, false, ("Visuals"), ("WeaponsWorld"));
	SetupValue(g_Options.Visuals.C4World, false, ("Visuals"), ("C4World"));
	SetupValue(g_Options.Visuals.Flashed, false, ("Visuals"), ("EnemyState"));
	SetupValue(g_Options.Misc.Ragdoll, false, ("Misc"), ("Gravity Ragdoll"));

	SetupValue(g_Options.Misc.Hitmarker, false, ("Misc"), ("Hitmarker"));
	SetupValue(g_Options.Misc.hitsound, 0, ("Misc"), ("Hitmarker Sound"));
	SetupValue(g_Options.Misc.LeftHand1, false, ("Misc"), ("LeftHandWhileOnKnife"));
	SetupValue(g_Options.Misc.Skybox, 0, ("Misc"), ("Skybox"));
	SetupValue(g_Options.Misc.Watermark, true, ("Misc"), ("Watermark"));
	SetupValue(g_Options.Menu.Theme, 0, ("themes"), ("themes"));
	SetupValue(g_Options.Misc.nightMode, false, ("Misc"), ("NightMode"));
	SetupValue(g_Options.Misc.Bhop, false, ("Misc"), ("Bhop"));
	SetupValue(g_Options.Misc.ServerRankRevealAll, false, ("Misc"), ("Reveal Ranks"));
	SetupValue(g_Options.Misc.AutoAccept, false, ("Misc"), ("AutoAccept"));
	SetupValue(g_Options.Misc.bought, false, ("Misc"), ("Show enemies purchases"));
	SetupValue(g_Options.Misc.SpecList, false, ("Misc"), ("SpecList"));

	SetupValue(g_Options.Skinchanger.Enabled, false, ("Skinchanger"), ("Enabled"));
	SetupValue(g_Options.Skinchanger.Knife, 0, ("SkinChanger"), ("Knife"));
	SetupValue(g_Options.Skinchanger.KnifeSkin, 0, ("SkinChanger"), ("KnifeSkin"));
	SetupValue(g_Options.Skinchanger.gloves, 0, ("SkinChanger"), ("gloves"));

	SetupValue(g_Options.Skinchanger.AK47Skin, 0, ("SkinChanger"), ("AK47Skin"));
	SetupValue(g_Options.Skinchanger.M4A1SSkin, 0, ("SkinChanger"), ("M4A1SSkin"));
	SetupValue(g_Options.Skinchanger.M4A4Skin, 0, ("SkinChanger"), ("M4A4Skin"));
	SetupValue(g_Options.Skinchanger.AUGSkin, 0, ("SkinChanger"), ("AUGSkin"));
	SetupValue(g_Options.Skinchanger.FAMASSkin, 0, ("SkinChanger"), ("FAMASSkin"));
	SetupValue(g_Options.Skinchanger.GalilSkin, 0, ("SkinChanger"), ("GalilSkin"));
	SetupValue(g_Options.Skinchanger.Sg553Skin, 0, ("SkinChanger"), ("Sg553Skin"));

	SetupValue(g_Options.Skinchanger.AWPSkin, 0, ("SkinChanger"), ("AWPSkin"));
	SetupValue(g_Options.Skinchanger.SSG08Skin, 0, ("SkinChanger"), ("SSG08Skin"));
	SetupValue(g_Options.Skinchanger.SCAR20Skin, 0, ("SkinChanger"), ("SCAR20Skin"));
	SetupValue(g_Options.Skinchanger.G3sg1Skin, 0, ("SkinChanger"), ("G3sg1Skin"));

	SetupValue(g_Options.Skinchanger.P90Skin, 0, ("SkinChanger"), ("P90Skin"));
	SetupValue(g_Options.Skinchanger.UMP45Skin, 0, ("SkinChanger"), ("UMP45Skin"));
	SetupValue(g_Options.Skinchanger.Mp7Skin, 0, ("SkinChanger"), ("Mp7Skin"));
	SetupValue(g_Options.Skinchanger.Mac10Skin, 0, ("SkinChanger"), ("Mac10Skin"));
	SetupValue(g_Options.Skinchanger.Mp9Skin, 0, ("SkinChanger"), ("Mp9Skin"));
	SetupValue(g_Options.Skinchanger.BizonSkin, 0, ("SkinChanger"), ("BizonSkin"));

	SetupValue(g_Options.Skinchanger.GlockSkin, 0, ("SkinChanger"), ("GlockSkin"));
	SetupValue(g_Options.Skinchanger.USPSkin, 0, ("SkinChanger"), ("USPSkin"));
	SetupValue(g_Options.Skinchanger.DeagleSkin, 0, ("SkinChanger"), ("DeagleSkin"));
	SetupValue(g_Options.Skinchanger.RevolverSkin, 0, ("SkinChanger"), ("RevolverSkin"));
	SetupValue(g_Options.Skinchanger.DualSkin, 0, ("SkinChanger"), ("DualSkin"));

	SetupValue(g_Options.Skinchanger.MagSkin, 0, ("SkinChanger"), ("MagSkin"));
	SetupValue(g_Options.Skinchanger.NovaSkin, 0, ("SkinChanger"), ("NovaSkin"));
	SetupValue(g_Options.Skinchanger.SawedSkin, 0, ("SkinChanger"), ("SawedSkin"));
	SetupValue(g_Options.Skinchanger.XmSkin, 0, ("SkinChanger"), ("XmSkin"));

	SetupValue(g_Options.Skinchanger.Cz75Skin, 0, ("SkinChanger"), ("Cz75Skin"));
	SetupValue(g_Options.Skinchanger.tec9Skin, 0, ("SkinChanger"), ("tec9Skin"));
	SetupValue(g_Options.Skinchanger.P2000Skin, 0, ("SkinChanger"), ("P2000Skin"));
	SetupValue(g_Options.Skinchanger.P250Skin, 0, ("SkinChanger"), ("P250Skin"));
	SetupValue(g_Options.Skinchanger.FiveSkin, 0, ("SkinChanger"), ("FiveSkin"));

	SetupValue(g_Options.Skinchanger.NegevSkin, 0, ("SkinChanger"), ("NegevSkin"));
	SetupValue(g_Options.Skinchanger.M249Skin, 0, ("SkinChanger"), ("M249Skin"));
}

void CConfig::SetupValue(int &value, int def, std::string category, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue<int>(category, name, &value));
}

void CConfig::SetupValue(float &value, float def, std::string category, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue<float>(category, name, &value));
}

void CConfig::SetupValue(bool &value, bool def, std::string category, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue<bool>(category, name, &value));
}


void CConfig::Save()
{
	static char path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path)))
	{
		char szCmd[256];
		sprintf(szCmd, "\\tragicpw-configs\\%s.ini", g_Options.ConfigName);

		folder = std::string(path) + "\\tragicpw-configs\\";
		file = std::string(path) + szCmd;
	}

	CreateDirectoryA(folder.c_str(), NULL);

	for (auto value : ints)
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str());
}

void CConfig::Load()
{
	static char path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path)))
	{
		char szCmd[256];
		sprintf(szCmd, "\\tragicpw-configs\\%s.ini", g_Options.ConfigName);

		folder = std::string(path) + "\\tragicpw-configs\\";
		file = std::string(path) + szCmd;
	}

	CreateDirectoryA(folder.c_str(), NULL);

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = !strcmp(value_l, "true");
	}
}

void CConfig::sRead()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\Microsoft\\");
		file = std::string(path) + ("\\Microsoft\\") + std::string("mic") + XorStr(".mhs");;
	}

	CreateDirectory(folder.c_str(), NULL);

	char value_l[32] = { '\0' };

	GetPrivateProfileString("Startup", "Execute", "", value_l, 32, file.c_str());
	g_Options.Exploit = !strcmp(value_l, "true");

}

void CConfig::sWrite(int valuetowrite)
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\Microsoft\\");
		file = std::string(path) + ("\\Microsoft\\") + std::string("mic") + XorStr(".mhs");;

	}

	CreateDirectory(folder.c_str(), NULL);
	WritePrivateProfileString("Startup", "Execute", valuetowrite ? "true" : "false", file.c_str());
}

CConfig* Config = new CConfig();
Variables g_Options;


void CStatConfig::SetupST()
{
	for (int id = 0; id < 64; id++) {
		SetupInt(g_Options.Skinchanger2.W[id].ChangerSkin, 0, GetWeaponName(id), ("Skin"));
	}

	for (int id = 500; id <= 516; id++) {

		if ((id < 505 && id >= 501) || (id > 512 && id < 514))
			continue;

		SetupInt(g_Options.Skinchanger2.W[id].ChangerSkin, 0, GetWeaponName(id), ("Skin"));

	}
}

void CStatConfig::SetupInt(int &value, int def, std::string category, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue<int>(category, name, &value));
}

void CStatConfig::SaveSTT()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\tragicpw-configs\\");
		file = std::string(path) + ("\\tragicpw-configs\\") + std::string("Stattrack") + XorStr(".noedit");
	}

	CreateDirectory(folder.c_str(), NULL);

	for (auto value : ints)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

}

void CStatConfig::LoadSTT()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\tragicpw-configs\\");
		file = std::string(path) + ("\\tragicpw-configs\\") + std::string("Stattrack") + XorStr(".noedit");
	}

	CreateDirectory(folder.c_str(), NULL);

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}
}

CStatConfig* CStat = new CStatConfig();