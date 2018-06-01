

#pragma once

#include "entities.h"
#include "SDK.h"
#include "singleton.hpp"

Vector GetAutostrafeView();

class misc
    : public singleton<misc>
{
public:
	void OnCreateMove(CInput::CUserCmd *cmd, C_BaseEntity *local);

	static void misc::FakeWalk(CInput::CUserCmd* pCmd, bool &bSendPacket, C_BaseEntity *local);
	void do_aimbot(C_BaseEntity *local, CBaseCombatWeapon *weapon, CInput::CUserCmd *cmd);
	void logsrun();
private:
	void AutoStrafe(CInput::CUserCmd *cmd, C_BaseEntity *local);
};
template<class T, class U>
inline T clamp(T in, U low, U high)
{
    if (in <= low)
        return low;
    else if (in >= high)
        return high;
    else
        return in;
}

inline float bitsToFloat(unsigned long i)
{
    return *reinterpret_cast<float*>(&i);
}


inline float FloatNegate(float f)
{
    return bitsToFloat(FloatBits(f) ^ 0x80000000);
}

char* const skybox_models[] =
{
	"cs_baggage_skybox_",
	"cs_tibet",
	"embassy",
	"italy",
	"jungle",
	"nukeblank",
	"office",
	"sky_cs15_daylight01_hdr",
	"sky_cs15_daylight02_hdr",
	"sky_cs15_daylight03_hdr",
	"sky_cs15_daylight04_hdr",
	"sky_csgo_cloudy01",
	"sky_csgo_night02",
	"sky_csgo_night02b",
	"sky_csgo_night_flat",
	"sky_day02_05_hdr",
	"sky_day02_05",
	"sky_venice",
	"sky_dust",
	"sky_l4d_rural02_ldr",
	"vertigo_hdr",
	"vertigoblue_hdr",
	"vertigo",
	"vietnam"
};

char* const tuxlist[] =
{
	"AimTux owns me and all",
	"Your Windows p2c sucks my AimTux dry",
	"It's free as in FREEDOM!",
	"Tux only let me out so I could play this game, please be nice!",
	"Tux nutted but you keep sucken",
	">tfw no vac on Linux",
};
char* const ezfragslist[] =
{
	"Think you could do better? Not without www.EZFrags.co.uk",
	"If I was cheating, I'd use www.EZFrags.co.uk",
	"I'm not using www.EZFrags.co.uk, you're just bad",
	"Visit www.EZFrags.co.uk for the finest public & private CS:GO cheats",
	"Stop being a noob! Get good with www.EZFrags.co.uk",
	"You just got pwned by www.EZFrags.co.uk, the #1 CS:GO cheat",
};
char* const phooklist[] =
{
	"tragic.pw",
	"Get Good! Get tragic!",
	"I'm not cheating, but If I was I would recommend tragic.pw",
	"tragic > all",
	"tragic owns me and all",
	"tragic #1"
};
char* const hooktroniclist[] =
{
	"Get HookTronic.TOP and never fear VAC.",
	"HookTronic.TOP, simply untouchable",
	"HookTronic owns me and all",
	"Did you seriously pay for that shit?",
	"Refund and buy HookTronic",
	"Enjoy your VAC wih your shitty provider"
};
char* const aimwarelist[] =
{
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats"
};
char* const huecheatslist[] =
{
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats",
	"www.AIMWARE.net | CSGO Premium Cheats"
};
inline void SetName(const char* new_name)
{
    static auto name = g_CVar->FindVar("name");

    name->SetValue(new_name);
    *reinterpret_cast<int*>(uintptr_t(&name->fnChangeCallback) + 0xC) = 0;
}
inline void setclantag(const char* tag)
{
    static auto ClanTagOffset = U::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");
    if (ClanTagOffset)
    {
        auto tag_ = std::string(tag);
        if (strlen(tag) > 0) {
            auto newline = tag_.find("\\n");
            auto tab = tag_.find("\\t");
            if (newline != std::string::npos) {
                tag_.replace(newline, newline + 2, "\n");
            }
            if (tab != std::string::npos) {
                tag_.replace(tab, tab + 2, "\t");
            }
        }
        static auto dankesttSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(ClanTagOffset);
        dankesttSetClanTag(tag_.data(), tag_.data());
    }
}


inline void RankReveal(CInput::CUserCmd* cmd)
{
	if (cmd->buttons & IN_SCORE && g_Options.Misc.ServerRankRevealAll)
	{
		static auto ServerRankRevealAll = (bool(__cdecl*)(int*))((PDWORD)U::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????x????"));
		if (g_Options.Misc.ServerRankRevealAll)
		{
			static int fArray[3] = { 0,0,0 };
			ServerRankRevealAll(fArray);
		}
	}
}
