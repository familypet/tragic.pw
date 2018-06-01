
#include "ESP.h"
#include "Interfaces.h"
#include "Render.h"
#include <ctime>
#include <iostream>
#include <algorithm>
#include "GrenadePrediction.h"
#include "LagComp.h"
#include "Autowall.h"


visuals::visuals()
{
	BombCarrier = nullptr;
}

int width = 0;
int height = 0;
bool done = false;
void visuals::OnPaintTraverse(C_BaseEntity* local)
{
	CViewSetup* g_ViewSetup;

	CInput::CUserCmd* pCmd;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->GetActiveWeaponHandle());

	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	NightMode();
	DoAsusWalls();

	SkyChanger();

	if (g_Options.Visuals.angleLines && pLocal->IsAlive())
		DrawAngles();

	if (g_Options.Visuals.DrawAwall && pLocal->IsAlive())
		DrawAwall();
    for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
    {

        C_BaseEntity *entity = g_EntityList->GetClientEntity(i);
        player_info_t pinfo;
        if (entity == local && local->IsAlive() && g_Engine->GetPlayerInfo(g_Engine->GetLocalPlayer(), &pinfo))
        {
            if (g_Input->m_fCameraInThirdPerson && g_Options.Visuals.Enabled)
            {
                Vector max = entity->GetCollideable()->OBBMaxs();
                Vector pos, pos3D;
                Vector top, top3D;
                pos3D = entity->GetOrigin();
                top3D = pos3D + Vector(0, 0, max.z);

                if (!g_Render->WorldToScreen(pos3D, pos) || !g_Render->WorldToScreen(top3D, top))
                    return;

                float height = (pos.y - top.y);
                float width = height / 4.f;
                if (g_Options.Visuals.Box)
                {
                    Color color;
                    color = GetPlayerColor(entity, local);
                    PlayerBox(top.x, top.y, width, height, color);
                }
                if (g_Options.Visuals.HP)
                    DrawHealth(pos, top, local->GetHealth());

                if (g_Options.Visuals.Name)
                    g_Render->DrawString2(g_Render->font.ESP, (int)top.x, (int)top.y - 6, Color::White(), FONT_CENTER, pinfo.name);

            }
        }
		if (g_Options.Misc.Radar)
		{
			DWORD m_bSpotted = offsetz.DT_BaseEntity.m_bSpotted;
			*(char*)((DWORD)(entity)+m_bSpotted) = 1; //error 
		}

        if (entity && entity != local && !entity->IsDormant())
        {
            if (g_Engine->GetPlayerInfo(i, &pinfo) && entity->IsAlive())
            {
                if (g_Options.Visuals.backtrackline)
				{
					if (local->IsAlive())
					{
						for (int t = 0; t < g_Options.LegitBot.backtrackTicks; ++t)
						{
							Vector screenbacktrack[64][12];

							if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > local->GetSimulationTime())
							{
								if (g_Render->WorldToScreen(headPositions[i][t].hitboxPos, screenbacktrack[i][t]))
								{

									g_Surface->DrawSetColor(Color(int(g_Options.Colors.backtrackdots_color[0] * 255.f), int(g_Options.Colors.backtrackdots_color[1] * 255.f), int(g_Options.Colors.backtrackdots_color[2] * 255.f)));
									g_Surface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);

								}
							}
						}
					}
                    else
                    {
                        memset(&headPositions[0][0], 0, sizeof(headPositions));
                    }
                }
				if (g_Options.Ragebot.PosAdjust)
				{
					if (local->IsAlive())
					{
						for (int t = 0; t < 12; ++t)
						{
							Vector screenbacktrack[64][12];

							if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > local->GetSimulationTime())
							{
								if (g_Render->WorldToScreen(headPositions[i][t].hitboxPos, screenbacktrack[i][t]))
								{

									g_Surface->DrawSetColor(Color::Green());
									g_Surface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);

								}
							}
						}
					}
					else
					{
						memset(&headPositions[0][0], 0, sizeof(headPositions));
					}
				}
				if (g_Options.Ragebot.FakeLagFix)
				{
					if (local->IsAlive())
					{
						Vector screenbacktrack[64];

						if (backtracking->records[i].tick_count + 12 > g_Globals->tickcount)
						{
							if (g_Render->WorldToScreen(backtracking->records[i].headPosition, screenbacktrack[i]))
							{

								g_Surface->DrawSetColor(Color::Black());
								g_Surface->DrawLine(screenbacktrack[i].x, screenbacktrack[i].y, screenbacktrack[i].x + 2, screenbacktrack[i].y + 2);

							}
						}
					}
					else
					{
						memset(&backtracking->records[0], 0, sizeof(backtracking->records));
					}
				}

                if (g_Options.Visuals.Enabled && g_Options.Visuals.Filter.Players)
                {
                    if (g_Options.Visuals.DLight)
                        DLight(local, entity);

                    DrawPlayer(entity, pinfo, local);

                }
				if (g_Options.Visuals.DrawAwall && local->IsAlive())
					DrawAwall();

            }
            if (g_Options.Visuals.Enabled)
            {
                ClientClass* cClass = (ClientClass*)entity->GetClientClass();
                if (g_Options.Visuals.WeaponsWorld && cClass->m_ClassID != (int)ClassID::CBaseWeaponWorldModel && ((strstr(cClass->m_pNetworkName, "Weapon") || cClass->m_ClassID == (int)ClassID::CDEagle || cClass->m_ClassID == (int)ClassID::CAK47)))
                {
                    DrawDrop(entity);
                }
                if (g_Options.Visuals.C4World)
                {
                    if (cClass->m_ClassID == (int)ClassID::CPlantedC4)
                        DrawBombPlanted(entity, local);
                }

                if (cClass->m_ClassID == (int)ClassID::CC4)
                    DrawBomb(entity, cClass);
                if (g_Options.Visuals.GrenadeESP && strstr(cClass->m_pNetworkName, "Projectile"))
                {
                    DrawThrowable(entity);
                }
            }
        }
    }
    if (g_Options.Misc.SpecList) SpecList(local);
    
	if (g_Options.Misc.Hitmarker)
		Hitmarker();

	if (g_Options.Visuals.GhostChams && g_Input->m_fCameraInThirdPerson)
    NightMode();
	grenade_prediction::instance().Paint();

    if (g_Options.Visuals.SpreadCrosshair)
    {
        g_Engine->GetScreenSize(width, height);
        if (local  && local->IsAlive())
        {
            static Vector ViewAngles;
            g_Engine->GetViewAngles(ViewAngles);
            ViewAngles += local->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

            static Vector fowardVec;
            AngleVectors(ViewAngles, &fowardVec);
            fowardVec *= 10000;

            // Get ray start / end
            Vector start = local->GetOrigin() + local->GetViewOffset();
            Vector end = start + fowardVec, endScreen;

            CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->GetActiveWeaponHandle());
            float cone = pWeapon->GetSpread() + pWeapon->GetInaccuracy();
            if (cone > 0.0f)
            {
                if (cone < 0.01f) cone = 0.01f;
                float size = (cone * height) * 0.7f;
                Color color(int(g_Options.Colors.color_spread[0] * 255.f), int(g_Options.Colors.color_spread[1] * 255.f), int(g_Options.Colors.color_spread[2] * 255.f));

                if (g_Render->WorldToScreen(end, endScreen))
                {
                    g_Render->OutlineCircle(endScreen.x, endScreen.y, (int)size, 48, color);
                }
            }
        }
    }

	if (g_Options.Visuals.RecoilCrosshair)
	{
		g_Engine->GetScreenSize(width, height);
		if (local && local->IsAlive())
		{
			static Vector ViewAngles;
			g_Engine->GetViewAngles(ViewAngles);
			ViewAngles += local->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

			Vector fowardVec;
			AngleVectors(ViewAngles, &fowardVec);
			fowardVec *= 10000;

			Vector start = local->GetOrigin() + local->GetViewOffset();
			Vector end = start + fowardVec, endScreen;

			if (g_Render->WorldToScreen(end, endScreen) && local->IsAlive())
			{
				g_Render->Line(endScreen.x - 4, endScreen.y, endScreen.x + 4, endScreen.y, Color(int(g_Options.Colors.color_recoil[0] * 255.f), int(g_Options.Colors.color_recoil[1] * 255.f), int(g_Options.Colors.color_recoil[2] * 255.f)));
				g_Render->Line(endScreen.x, endScreen.y - 4, endScreen.x, endScreen.y + 4, Color(int(g_Options.Colors.color_recoil[0] * 255.f), int(g_Options.Colors.color_recoil[1] * 255.f), int(g_Options.Colors.color_recoil[2] * 255.f)));
			}
		}
	}
}

std::string CleanItemName(std::string name)
{
	std::string Name = name;
	// Tidy up the weapon Name
	if (Name[0] == 'C')
		Name.erase(Name.begin());

	// Remove the word Weapon
	auto startOfWeap = Name.find("Weapon");
	if (startOfWeap != std::string::npos)
		Name.erase(Name.begin() + startOfWeap, Name.begin() + startOfWeap + 6);

	return Name;
}

wchar_t* CharToWideChar(const char* text)
{
	size_t size = strlen(text) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs_s(NULL, wa, size / 4, text, size);
	return wa;
}
void visuals::SkyChanger()
{
	static auto sv_skyname = g_CVar->FindVar("sv_skyname");

	switch (g_Options.Visuals.SkyboxChanger)
	{
	case 1: //Baggage
		sv_skyname->SetValue("cs_baggage_skybox_");
		break;
	case 2: //Tibet
		sv_skyname->SetValue("cs_tibet");
		break;
	case 3: //Clear Sky
		sv_skyname->SetValue("clearsky");
		break;
	case 4: //Clear Sky HD
		sv_skyname->SetValue("clearsky_hdr");
		break;
	case 5: //Embassy
		sv_skyname->SetValue("embassy");
		break;
	case 6: //Italy
		sv_skyname->SetValue("italy");
		break;
	case 7: //Daylight 1
		sv_skyname->SetValue("sky_cs15_daylight01_hdr");
		break;
	case 8: //Daylight 2
		sv_skyname->SetValue("sky_cs15_daylight02_hdr");
		break;
	case 9: //Daylight 3
		sv_skyname->SetValue("sky_cs15_daylight03_hdr");
		break;
	case 10: //Daylight 4
		sv_skyname->SetValue("sky_cs15_daylight04_hdr");
		break;
	case 11: //Cloudy
		sv_skyname->SetValue("sky_csgo_cloudy01");
		break;
	case 12: //Night 1
		sv_skyname->SetValue("sky_csgo_night02");
		break;
	case 13: //Night 2
		sv_skyname->SetValue("sky_csgo_night02b");
		break;
	case 14: //Night Flat
		sv_skyname->SetValue("sky_csgo_night_flat");
		break;
	case 15: //Day HD
		sv_skyname->SetValue("sky_day02_05_hdr");
		break;
	case 16: //Day
		sv_skyname->SetValue("sky_day02_05");
		break;
	case 17: //Black
		sv_skyname->SetValue("sky_l4d_rural02_ldr");
		break;
	case 18: //Vertigo HD
		sv_skyname->SetValue("vertigo_hdr");
		break;
	case 19: //Vertigo Blue HD
		sv_skyname->SetValue("vertigoblue_hdr");
		break;
	case 20: //Vertigo
		sv_skyname->SetValue("vertigo");
		break;
	case 21: //Vietnam
		sv_skyname->SetValue("vietnam");
		break;
	case 22: //Dusty Sky
		sv_skyname->SetValue("sky_dust");
		break;
	case 23: //Jungle
		sv_skyname->SetValue("jungle");
		break;
	case 24: //Nuke
		sv_skyname->SetValue("nukeblank");
		break;
	case 25: //Office
		sv_skyname->SetValue("office");
		break;
	}
}

C_CSPlayerResource* playerresources;
void visuals::DrawPlayer(C_BaseEntity* entity, player_info_t pinfo, C_BaseEntity* local)
{
	ESPBox Box;
	ESPBox size;
	Vector max = entity->GetCollideable()->OBBMaxs();
	Vector pos, pos3D;
	Vector top, top3D;
	pos3D = entity->GetOrigin();
	top3D = pos3D + Vector(0, 0, max.z);

	if (!g_Render->WorldToScreen(pos3D, pos) || !g_Render->WorldToScreen(top3D, top))
		return;

	float height = (pos.y - top.y);
	float width = height / 4.f;

	Color color;

	if (g_Options.Visuals.Filter.EnemyOnly && (entity->GetTeamNum() == local->GetTeamNum()))
		return;
	color = GetPlayerColor(entity, local);

	if (g_Options.Visuals.Box)
		PlayerBox(top.x, top.y, width, height, color);

	if (g_Options.Visuals.HP)
		DrawHealth(pos, top, entity->GetHealth());

	if (g_Options.Visuals.Name)
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x, (int)top.y - 6, Color::White(), FONT_CENTER, pinfo.name);

	if (g_Options.Visuals.Skeleton)
		DrawSkeleton(entity);

	if (g_Options.Visuals.Snaplines)
		Snaplines(Box, Color::Red(), entity);
	
	int bottom = 0;

    /*auto m_AnimOverlay = entity->GetAnimationOverlay();

    for (int i = 0; i < 14; i++)
    {
        char sequence_string[64];
        sprintf_s(sequence_string, sizeof(sequence_string) - 1, "%d | %d", i, m_AnimOverlay.m_pElements[i].Sequence);
        g_Render->DrawString2(g_Render->font.ESP, top.x + 20 + height, top.y + 10 * i, Color::White(), FONT_LEFT, sequence_string);
    }*/

	std::vector<std::string> weapon;
	std::vector<std::string> bomb;
	std::vector<std::string> rank;
	std::vector<std::string> wins;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(entity->GetActiveWeaponHandle());
    if (g_Options.Visuals.Weapon && pWeapon)
    {
        int weapon_id = pWeapon->m_AttributeManager()->m_Item()->GetItemDefinitionIndex();

        auto weapon_name = ItemDefinitionIndexToString(weapon_id);
        weapon.push_back(weapon_name);
    }



	if (g_Options.Visuals.C4 && entity == BombCarrier)
	{
		bomb.push_back("Bomb");
	}

	int i = 0;
	if (g_Options.Visuals.Weapon)
	{

		for (auto Text : weapon)
		{
			g_Render->DrawString2(g_Render->font.Defuse, (int)top.x, int(top.y + height + 8 + (10 * bottom++)), Color::White(), FONT_CENTER, "%s", Text.c_str());
			i++;
		}
	}
	if (g_Options.Visuals.C4)
	{
		for (auto Text : bomb)
		{
			g_Render->DrawString2(g_Render->font.Defuse, (int)top.x, int(top.y + height + 8 + (10 * bottom++)), Color::Red(), FONT_CENTER, Text.c_str());
			i++;
		}
	}

	if (g_Options.Visuals.money)
	{
		g_Render->Textf(int(top.x + width + 3), int(top.y + 12), Color(50, 226, 120, 255), g_Render->font.ESP, "%i", entity->GetMoney());
	}

	if (g_Options.Visuals.IsScoped && entity->IsScoped())
	{
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x + width + 3, int(top.y + 7), Color::Red(), FONT_CENTER, "*Scoped*");
		i++;
	}

	if (g_Options.Visuals.HasDefuser && entity->hasDefuser())
	{
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x, int(top.y + height + 8 + (14 * bottom++)), Color::LightBlue(), FONT_CENTER, "Defuser");
		i++;
	}
	if (g_Options.Visuals.Flashed && entity->IsFlashed())
	{
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x, int(top.y + height + 15 + (18 * bottom++)), Color::White(), FONT_CENTER, "*Flashed*");
		i++;
	}



}

void visuals::DrawAngles()
{
	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = pLocal;

	/*AngleVectors(QAngle(0, pLocal->GetLowerBodyYaw(), 0), &forward);
	src3D = pLocal->GetOrigin();
	dst3D = src3D + (forward * 35.f); //replace 50 with the length you want the line to have

	ray.Init(src3D, dst3D);

	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!g_Render->WorldToScreen(src3D, src) || !g_Render->WorldToScreen(tr.endpos, dst))
	return;

	g_Render->Line(src.x, src.y, dst.x, dst.y, Color(255, 165, 0, 255));
	g_Render->Text(dst.x, dst.y, Color(255, 165.f, 0, 255), g_Render->font.ESP, "lby");*/

	AngleVectors(QAngle(0, Globals::RealAngle, 0), &forward);
	src3D = pLocal->GetOrigin();
	dst3D = src3D + (forward * 45.f);

	ray.Init(src3D, dst3D);

	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!g_Render->WorldToScreen(src3D, src) || !g_Render->WorldToScreen(tr.endpos, dst))
		return;

	g_Render->Line(src.x, src.y, dst.x, dst.y, Color(0, 255, 0, 255));
	g_Render->Text(dst.x, dst.y, Color(0, 255, 0, 255), g_Render->font.ESP, "REAL");

	AngleVectors(QAngle(0, Globals::FakeAngle, 0), &forward);
	dst3D = src3D + (forward * 45.f);

	ray.Init(src3D, dst3D);

	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!g_Render->WorldToScreen(src3D, src) || !g_Render->WorldToScreen(tr.endpos, dst))
		return;

	g_Render->Line(src.x, src.y, dst.x, dst.y, Color(255, 0, 0, 255));
	g_Render->Text(dst.x, dst.y, Color(255, 0, 0, 255), g_Render->font.ESP, "FAKE");
}
bool Snaplines;

void visuals::Snaplines(visuals::ESPBox size, Color color, C_BaseEntity* entity)
{
	C_BaseEntity* local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	Vector src3D, src;
	src3D = entity->GetOrigin() - Vector(0, 0, 0);

	if (!g_Render->WorldToScreen(src3D, src))
		return;

	int ScreenWidth, ScreenHeight;
	g_Engine->GetScreenSize(ScreenWidth, ScreenHeight);

	int x = (int)(ScreenWidth * 0.5f);
	int y = 1; //0

	y = ScreenHeight;

	if (local->IsAlive() && !g_Input->m_fCameraInThirdPerson)
	{
		g_Render->Line((int)(src.x), (int)(src.y), x, y, Color(255, 255, 255, 255));
	}
}


void visuals::Hitmarker()
{
	if (G::hitmarkeralpha < 0.f)
		G::hitmarkeralpha = 0.f;
	else if (G::hitmarkeralpha > 0.f)
		G::hitmarkeralpha -= 0.01f;

	int W, H;
	g_Engine->GetScreenSize(W, H);

	if (G::hitmarkeralpha > 0.f)
	{
		g_Render->Line(W / 2 - 10, H / 2 - 10, W / 2 - 5, H / 2 - 5, Color(int(g_Options.Colors.hitmark_color[0] * 255.f), int(g_Options.Colors.hitmark_color[1] * 255.f), int(g_Options.Colors.hitmark_color[2] * 255.f), (G::hitmarkeralpha * 255.f)));
		g_Render->Line(W / 2 - 10, H / 2 + 10, W / 2 - 5, H / 2 + 5, Color(int(g_Options.Colors.hitmark_color[0] * 255.f), int(g_Options.Colors.hitmark_color[1] * 255.f), int(g_Options.Colors.hitmark_color[2] * 255.f), (G::hitmarkeralpha * 255.f)));
		g_Render->Line(W / 2 + 10, H / 2 - 10, W / 2 + 5, H / 2 - 5, Color(int(g_Options.Colors.hitmark_color[0] * 255.f), int(g_Options.Colors.hitmark_color[1] * 255.f), int(g_Options.Colors.hitmark_color[2] * 255.f), (G::hitmarkeralpha * 255.f)));
		g_Render->Line(W / 2 + 10, H / 2 + 10, W / 2 + 5, H / 2 + 5, Color(int(g_Options.Colors.hitmark_color[0] * 255.f), int(g_Options.Colors.hitmark_color[1] * 255.f), int(g_Options.Colors.hitmark_color[2] * 255.f), (G::hitmarkeralpha * 255.f)));

	}
}
float wallAlpha;
float modelalpha;
float propAlpha;
bool prepared;

void visuals::DoAsusWalls()
{

	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());


	if (!pLocal || !g_Engine->IsConnected() || !g_Engine->IsInGame())
		return;

	if (g_Options.Visuals.propalpha != propAlpha) {
		for (MaterialHandle_t i = g_MaterialSystem->FirstMaterial(); i != g_MaterialSystem->InvalidMaterial(); i = g_MaterialSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = g_MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "StaticProp textures")) {
				pMaterial->AlphaModulate(g_Options.Visuals.propalpha);
			}
		}
		propAlpha = g_Options.Visuals.propalpha;
	}

	if (g_Options.Visuals.wallalpha != wallAlpha) {
		for (MaterialHandle_t i = g_MaterialSystem->FirstMaterial(); i != g_MaterialSystem->InvalidMaterial(); i = g_MaterialSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = g_MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World textures"))
				pMaterial->AlphaModulate(g_Options.Visuals.wallalpha);
		}
		wallAlpha = g_Options.Visuals.wallalpha;
	}



	if (g_Options.Visuals.modelalpha != modelalpha) {
		for (MaterialHandle_t i = g_MaterialSystem->FirstMaterial(); i != g_MaterialSystem->InvalidMaterial(); i = g_MaterialSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = g_MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "Model textures"))
				pMaterial->AlphaModulate(g_Options.Visuals.modelalpha);
		}
		modelalpha = g_Options.Visuals.modelalpha;
	}

}

void visuals::PlayerBox(float x, float y, float w, float h, Color clr)
{
	g_Surface->DrawSetColor(clr);
	g_Surface->DrawOutlinedRect(int(x - w), int(y), int(x + w), int(y + h));
	g_Surface->DrawSetColor(Color::Black());
	g_Surface->DrawOutlinedRect(int(x - w - 1), int(y - 1), int(x + w + 1), int(y + h + 1));
	g_Surface->DrawOutlinedRect(int(x - w + 1), int(y + 1), int(x + w - 1), int(y + h - 1));
}

Color visuals::GetPlayerColor(C_BaseEntity* entity, C_BaseEntity* local)
{
	int TeamNum = entity->GetTeamNum();
	bool IsVis = MiscFunctions::IsVisible(local, entity, Head);

	Color color;
	static float rainbow;
	rainbow += 0.005f;
	if (rainbow > 1.f)
		rainbow = 0.f;
	if (TeamNum == TEAM_CS_T)
	{

		if (IsVis)
			color = Color(32, 130, 43);
		else
			color = Color(int(g_Options.Colors.box_color_t[0] * 255.f), int(g_Options.Colors.box_color_t[1] * 255.f), int(g_Options.Colors.box_color_t[2] * 255.f));
	}
	else
	{
		if (IsVis)
			color = Color(32, 130, 43);
		else
			color = Color(int(g_Options.Colors.box_color_ct[0] * 255.f), int(g_Options.Colors.box_color_ct[1] * 255.f), int(g_Options.Colors.box_color_ct[2] * 255.f));
	}


	return color;
}

void visuals::DrawSkeleton(C_BaseEntity* entity)
{
	studiohdr_t* pStudioHdr = g_ModelInfo->GetStudiomodel(entity->GetModel());

	if (!pStudioHdr)
		return;

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);

		if (pBone && (pBone->flags & 0x100) && (pBone->parent != -1))
		{
			vChild = entity->GetBonePos(j);
			vParent = entity->GetBonePos(pBone->parent);

			int iChestBone = 6;  // Parameter of relevant Bone number
			Vector vBreastBone; // New reference Point for connecting many bones
			Vector vUpperDirection = entity->GetBonePos(iChestBone + 1) - entity->GetBonePos(iChestBone); // direction vector from chest to neck
			vBreastBone = entity->GetBonePos(iChestBone) + vUpperDirection / 2;
			Vector vDeltaChild = vChild - vBreastBone; // Used to determine close bones to the reference point
			Vector vDeltaParent = vParent - vBreastBone;

			// Eliminating / Converting all disturbing bone positions in three steps.
			if ((vDeltaParent.Length() < 9 && vDeltaChild.Length() < 9))
				vParent = vBreastBone;

			if (j == iChestBone - 1)
				vChild = vBreastBone;

			if (abs(vDeltaChild.z) < 5 && (vDeltaParent.Length() < 5 && vDeltaChild.Length() < 5) || j == iChestBone)
				continue;

			if (g_Render->WorldToScreen(vParent, sParent) && g_Render->WorldToScreen(vChild, sChild)) {
				g_Render->Line(sParent.x, sParent.y, sChild.x, sChild.y, Color(int(g_Options.Colors.color_skeleton[0] * 255.f), int(g_Options.Colors.color_skeleton[1] * 255.f), int(g_Options.Colors.color_skeleton[2] * 255.f)));
			}
		}
	}
}

void visuals::DrawHealth(C_BaseEntity* entity, visuals::ESPBox size)
{
	int health = entity->GetHealth();
	int HP = health;
	if (HP > 100)
		HP = 100;
	int hp = health;
	float r = float(255 - health * 2.55);
	float g = float(health * 2.55);
	hp = (size.h - ((size.h * hp) / 100));

	g_Render->Outline(int(size.x - 4), int(size.y + hp), (int)2, int(size.h - hp + 1), Color((int)r, (int)g, 0));
	g_Render->Outline(int(size.x - 5), int(size.y - 1), (int)3, int(size.h + 2), Color(0, 0, 0, 150));
}



void visuals::DrawHealth(Vector bot, Vector top, float health)
{
	float h = (bot.y - top.y);
	float offset = (h / 4.f) + 5;
	float w = h / 64.f;

	UINT hp = UINT(h - (UINT)((h * health) / 100)); // Percentage

	int Red = int(255 - (health*2.55));
	int Green = int(health*2.55);

	g_Render->DrawOutlinedRect(int((top.x - offset) - 1), int(top.y - 1), 3, int(h + 2), Color::Black());

	g_Render->Line(int((top.x - offset)), int(top.y + hp), int((top.x - offset)), int(top.y + h), Color(Red, Green, 0, 255));
}

void visuals::DrawDrop(C_BaseEntity* entity)
{
    if (entity)
    {
        CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)entity;

        auto owner = pWeapon->GetOwnerHandle();

        if (owner > -1)
            return;

        Vector pos3D = entity->GetAbsOrigin2();

        if (pos3D.x == 0.0f && pos3D.y == 0.0f && pos3D.z == 0.0f)
            return;

        Vector pos;

        if (!g_Render->WorldToScreen(pos3D, pos))
            return;

        int weaponID = pWeapon->m_AttributeManager()->m_Item()->GetItemDefinitionIndex();
        auto weaponName = ItemDefinitionIndexToString(weaponID);


        g_Render->Text(pos.x, pos.y, Color(255, 255, 255, 255), g_Render->font.DroppedGuns, weaponName);
    }
}
float damage;
char bombdamagestringdead[24];
char bombdamagestringalive[24];
void visuals::DrawBombPlanted(C_BaseEntity* entity, C_BaseEntity* local)
{
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = entity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)entity;
	float flBlow = Bomb->GetC4BlowTime();
	float lifetime = flBlow - (g_Globals->interval_per_tick * local->GetTickBase());
	if (g_Render->WorldToScreen(vOrig, vScreen))
	{
		if (local->IsAlive())
		{
			float flDistance = local->GetEyePosition().DistTo(entity->GetEyePosition());
			float a = 450.7f;
			float b = 75.68f;
			float c = 789.2f;
			float d = ((flDistance - b) / c);
			float flDamage = a*exp(-d * d);

			damage = float((std::max)((int)ceilf(CSGO_Armor(flDamage, local->ArmorValue())), 0));

			sprintf_s(bombdamagestringdead, sizeof(bombdamagestringdead) - 1, "DEAD");
			sprintf_s(bombdamagestringalive, sizeof(bombdamagestringalive) - 1, "Health left: %.0f", local->GetHealth() - damage);
            if (lifetime > -2.f)
            {
                if (damage >= local->GetHealth())
                {
                    g_Render->Text((int)vScreen.x, int(vScreen.y + 10), Color(250, 42, 42, 255), g_Render->font.Defuse, bombdamagestringdead);
                }
                else if (local->GetHealth() > damage)
                {
                    g_Render->Text((int)vScreen.x, int(vScreen.y + 10), Color(0, 255, 0, 255), g_Render->font.Defuse, bombdamagestringalive);
                }
            }
		}
		char buffer[64];
		if (lifetime > 0.01f && !Bomb->IsBombDefused())
		{
			sprintf_s(buffer, "Bomb: %.1f", lifetime);
			g_Render->Text((int)vScreen.x, (int)vScreen.y, Color(250, 42, 42, 255), g_Render->font.ESP, buffer);
		}

	}

	g_Engine->GetScreenSize(width, height);
	int halfX = width / 2;
	int halfY = height / 2;


	if (Bomb->GetBombDefuser() > 0)
	{
		float countdown = Bomb->GetC4DefuseCountDown() - (local->GetTickBase() * g_Globals->interval_per_tick);
		if (countdown > 0.01f)
		{
			if (lifetime > countdown)
			{
				char defuseTimeString[24];
				sprintf_s(defuseTimeString, sizeof(defuseTimeString) - 1, "Defusing: %.1f", countdown);
				g_Render->Text(halfX - 50, halfY + 200, Color(0, 255, 0, 255), g_Render->font.Defuse, defuseTimeString);
			}
			else
			{
				g_Render->Text(halfX - 50, halfY + 200, Color(255, 0, 0, 255), g_Render->font.Defuse, "No More Time, Get Out!");
			}
		}
	}
}

void visuals::DrawAwall()
{
	int MidX;
	int MidY;
	g_Engine->GetScreenSize(MidX, MidY);

	int damage;
	if (CanWallbang(damage))
	{
		g_Render->OutlineCircle(MidX / 2, MidY / 2, 10, 10, Color(0, 255, 0));
		g_Render->Textf(MidX / 2, MidY / 2 + 6, Color(255, 255, 255, 255), g_Render->font.ESP, "DMG: %1i", damage);
	}
	else
	{
		g_Render->OutlineCircle(MidX / 2, MidY / 2, 10, 10, Color(255, 0, 0));
		g_Render->Textf(MidX / 2, MidY / 2 + 6, Color(255, 255, 255, 255), g_Render->font.ESP, "DMG: 0");
	}
}

void visuals::DrawBomb(C_BaseEntity* entity, ClientClass* cClass)
{
	// Null it out incase bomb has been dropped or planted
	BombCarrier = nullptr;
	CBaseCombatWeapon *BombWeapon = (CBaseCombatWeapon *)entity;
	Vector vOrig; Vector vScreen;
	vOrig = entity->GetOrigin();
	bool adopted = true;
	auto parent = BombWeapon->GetOwnerHandle();
	if (parent || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
	{
		C_BaseEntity* pParentEnt = (g_EntityList->GetClientEntityFromHandle(parent));
		if (pParentEnt && pParentEnt->IsAlive())
		{
			BombCarrier = pParentEnt;
			adopted = false;
		}
	}
	if (g_Options.Visuals.C4World)
	{
		if (adopted)
		{
			if (g_Render->WorldToScreen(vOrig, vScreen))
			{
				g_Render->Text((int)vScreen.x, (int)vScreen.y, Color(112, 20, 20, 255), g_Render->font.ESP, "Bomb");
			}
		}
	}
}

void visuals::DrawBox(visuals::ESPBox size, Color color)
{
	g_Render->Outline(size.x, size.y, size.w, size.h, color);
	g_Render->Outline(size.x - 1, size.y - 1, size.w + 2, size.h + 2, Color(10, 10, 10, 150));
	g_Render->Outline(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(10, 10, 10, 150));
}
bool visuals::GetBox(C_BaseEntity* entity, visuals::ESPBox &result)
{
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = entity->GetOrigin();
	min = entity->collisionProperty()->GetMins() + vOrigin;
	max = entity->collisionProperty()->GetMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	// Get screen positions
	if (!g_Render->WorldToScreen(points[3], flb) || !g_Render->WorldToScreen(points[5], brt)
		|| !g_Render->WorldToScreen(points[0], blb) || !g_Render->WorldToScreen(points[4], frt)
		|| !g_Render->WorldToScreen(points[2], frb) || !g_Render->WorldToScreen(points[1], brb)
		|| !g_Render->WorldToScreen(points[6], blt) || !g_Render->WorldToScreen(points[7], flt))
		return false;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	result.x = (int)left;
	result.y = (int)top;
	result.w = int(right - left);
	result.h = int(bottom - top);
	result.gay = (int)top;

	return true;
}
void visuals::BoxAndText(C_BaseEntity* entity, std::string text)
{
	ESPBox Box;
	std::vector<std::string> Info;
	if (GetBox(entity, Box))
	{
		Info.push_back(text);
		if (g_Options.Visuals.GrenadeESP)
		{
			DrawBox(Box, Color(255, 255, 255, 255));
			int i = 0;
			for (auto kek : Info)
			{
				g_Render->Text(Box.x + 1, Box.y + 1, Color(255, 255, 255, 255), g_Render->font.ESP, kek.c_str());
				i++;
			}
		}
	}
}

void visuals::DrawThrowable(C_BaseEntity* throwable)
{
	model_t* nadeModel = (model_t*)throwable->GetModel();

	if (!nadeModel)
		return;

	studiohdr_t* hdr = g_ModelInfo->GetStudiomodel(nadeModel);

	if (!hdr)
		return;

	if (!strstr(hdr->name, "thrown") && !strstr(hdr->name, "dropped"))
		return;

	std::string nadeName = "Unknown Grenade";

	IMaterial* mats[32];
	g_ModelInfo->GetModelMaterials(nadeModel, hdr->numtextures, mats);

	for (int i = 0; i < hdr->numtextures; i++)
	{
		IMaterial* mat = mats[i];
		if (!mat)
			continue;

		if (strstr(mat->GetName(), "flashbang"))
		{
			nadeName = "Flashbang";
			break;
		}
		else if (strstr(mat->GetName(), "m67_grenade") || strstr(mat->GetName(), "hegrenade"))
		{
			nadeName = "HE";
			break;
		}
		else if (strstr(mat->GetName(), "smoke"))
		{
			nadeName = "Smoke";
			break;
		}
		else if (strstr(mat->GetName(), "decoy"))
		{
			nadeName = "Decoy";
			break;
		}
		else if (strstr(mat->GetName(), "incendiary") || strstr(mat->GetName(), "molotov"))
		{
			nadeName = "Molotov";
			break;
		}
	}

	BoxAndText(throwable, nadeName);
}

void visuals::DLight(C_BaseEntity *local, C_BaseEntity* entity)
{
	player_info_t pinfo;
	if (local && entity && entity != local)
	{
		if (g_Engine->GetPlayerInfo(entity->GetIndex(), &pinfo) && entity->IsAlive() && !entity->IsDormant())
		{
			if (local->GetTeamNum() != entity->GetTeamNum())
			{
				dlight_t* pElight = g_Dlight->CL_AllocElight(entity->GetIndex());
				pElight->origin = entity->GetOrigin() + Vector(0.0f, 0.0f, 35.0f);
				pElight->radius = 400.0f;
				pElight->color.b = int(g_Options.Colors.dlight_color[0] * 255);
				pElight->color.g = int(g_Options.Colors.dlight_color[1] * 255);
				pElight->color.r = int(g_Options.Colors.dlight_color[2] * 255);
				pElight->die = g_Globals->curtime + 0.05f;
				pElight->decay = pElight->radius; // / 5.0f;
				pElight->key = entity->GetIndex();

				dlight_t* pDlight = g_Dlight->CL_AllocDlight(entity->GetIndex());
				pDlight->origin = entity->GetOrigin();
				pDlight->radius = 400.0f;
				pDlight->color.b = int(g_Options.Colors.dlight_color[0] * 255);
				pDlight->color.g = int(g_Options.Colors.dlight_color[1] * 255);
				pDlight->color.r = int(g_Options.Colors.dlight_color[2] * 255);
				pDlight->die = g_Globals->curtime + 0.05f;
				pDlight->decay = pDlight->radius; // / 5.0f;
				pDlight->key = entity->GetIndex();
			}
		}
	}
}

void visuals::NightMode()
{
    if (g_Options.Misc.nightMode)
    {
        if (!done)
        {


            static auto sv_skyname = g_CVar->FindVar("sv_skyname");
            static auto r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
            r_DrawSpecificStaticProp->SetValue(1);
            sv_skyname->SetValue("sky_csgo_night02");

            for (MaterialHandle_t i = g_MaterialSystem->FirstMaterial(); i != g_MaterialSystem->InvalidMaterial(); i = g_MaterialSystem->NextMaterial(i))
            {
                IMaterial *pMaterial = g_MaterialSystem->GetMaterial(i);

                if (!pMaterial)
                    continue;

                const char* group = pMaterial->GetTextureGroupName();
                const char* name = pMaterial->GetName();

                if (strstr(group, "World textures"))
                {
                    pMaterial->ColorModulate(0.10, 0.10, 0.10);
                }
                if (strstr(group, "StaticProp"))
                {
                    pMaterial->ColorModulate(0.30, 0.30, 0.30);
                }
                if (strstr(name, "models/props/de_dust/palace_bigdome"))
                {
                    pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
                }
                if (strstr(name, "models/props/de_dust/palace_pillars"))
                {
                    pMaterial->ColorModulate(0.30, 0.30, 0.30);
                }

                if (strstr(group, "Particle textures"))
                {
                    pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
                }
                done = true;
            }

        }
    }
    else
    {
        if (done)
        {
            for (MaterialHandle_t i = g_MaterialSystem->FirstMaterial(); i != g_MaterialSystem->InvalidMaterial(); i = g_MaterialSystem->NextMaterial(i))
            {
                IMaterial *pMaterial = g_MaterialSystem->GetMaterial(i);

                if (!pMaterial)
                    continue;

                const char* group = pMaterial->GetTextureGroupName();
                const char* name = pMaterial->GetName();

                if (strstr(group, "World textures"))
                {

                    pMaterial->ColorModulate(1, 1, 1);
                }
                if (strstr(group, "StaticProp"))
                {

                    pMaterial->ColorModulate(1, 1, 1);
                }
                if (strstr(name, "models/props/de_dust/palace_bigdome"))
                {
                    pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
                }
                if (strstr(name, "models/props/de_dust/palace_pillars"))
                {

                    pMaterial->ColorModulate(1, 1, 1);
                }
                if (strstr(group, "Particle textures"))
                {
                    pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
                }
            }
            done = false;
        }
    }
}

void visuals::SpecList(C_BaseEntity *local)
{

	RECT scrn = g_Render->GetViewport();
	int kapi = 0;

	if (local)
	{
		for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
		{
			// Get the entity
			C_BaseEntity *pEntity = g_EntityList->GetClientEntity(i);
			player_info_t pinfo;
			if (pEntity && pEntity != local)
			{
				if (g_Engine->GetPlayerInfo(i, &pinfo) && !pEntity->IsAlive() && !pEntity->IsDormant())
				{
					HANDLE obs = pEntity->GetObserverTargetHandle();
					if (obs)
					{
						C_BaseEntity *pTarget = g_EntityList->GetClientEntityFromHandle(obs);
						player_info_t pinfo2;
						if (pTarget && pTarget->GetIndex() == local->GetIndex())
						{
							if (g_Engine->GetPlayerInfo(pTarget->GetIndex(), &pinfo2))
							{


								g_Render->DrawString2(g_Render->font.ESP, scrn.left + 7, (scrn.top) + (16 * kapi) + 425, Color(255, 0, 0, 255), FONT_LEFT, "%s", pinfo.name);
								kapi++;
							}
						}
					}
				}
			}
		}
	}
	g_Render->DrawString2(g_Render->font.ESP, scrn.left + 7, (scrn.top) + 410, Color(255, 255, 255, 255), FONT_LEFT, "Spectators:");
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class besjtmp {
public:
	string rbdlnvnsbxcc;
	double cgaktosd;
	string igxkdzao;
	besjtmp();
	void cqpydpqxibp();

protected:
	double svdxzbjz;
	int pqhho;

	bool ovaaoovvhxxtsnjjrmker(bool jvuagtkpcnsjt, bool xkgbzj, double zehjoro, int ghqiczzx, double lenmljpxqhpxkkb, bool qgjxcsfcp, int tbxuxidkgrbq, string xgdis);
	double eeqytbzssrnbc(int mdrllhthorfxdwh, double szfttyeodpdam, bool iftkq, bool pdxjgb, string ldpslqbdrwt, string xqfzhwadl, string qmejcug, double joqrvn);
	double jhilgqulht(bool rvthvhxja, string nokozeaqp, string cervjh, string ypgseoosfkavzog, bool wdbgggiakjenlsk, double gizjujtteuaj, double jxksglg);
	int dodfuxzeehkfuwwfqrkj(bool kfjddmptdhprymp, int lijeobjxxxlvmg, bool jntfd, string agakncnxczek, double qzwzwd, int lnmbnacm, int rlduxdohycgsu, int oqbglmgi, bool ygxcoznb);
	double frnkyqpmjuksgtdgsxvcrqhd(string pgbyba, double moryovbxygdii, bool dvkhhadmecav, bool kxjlh, double xjhduzlemxhaz, int tsoenbjj, string reitrlk, int pjgvyaiebv);
	string ztipbbpxetgnk(double udyyc, double kyybon, int bxnpuxj, string mdbdgyckhghucog);

private:
	string qwmxmafhm;
	bool qlwzldlapsm;
	int yeqiw;

	int nbqduytzzmrqblowuxbv(double vqzyyvtqvsk, int efchbiesjgfwx);
	int zryexxxaytdktfm(bool uclaytua, string ihxxfecacku, bool bkwtmsix);
	void givrtzktmsgu(string lxcvybpummx, string qvrrjdadcb, double pqtoblrswpuefs, string bsjtosohjv, string uflutml, int udiorhlriczyxg);
	void tthyvvydfu(bool gdpkinrwyemb, bool rkgeolpeli, int rpagout, double adidahahpvjvt);
	void mxmorglkalyqk(int cfifkjbwdve, string dtalxpluxyk, double opcdpjkomlyu, double dyfbuqpreumypnt, int shthehswq, string cldshyd, double zedjlhpzlwv, string uyxhaapfq, string cidhy);
	int duuqxmiglbznpi(bool oayshhxgxxjt);

};


int besjtmp::nbqduytzzmrqblowuxbv(double vqzyyvtqvsk, int efchbiesjgfwx) {
	int ipihyoaba = 1131;
	double czhsqnmitxrxvd = 36277;
	bool qsxummbuxc = false;
	bool uluvlpohr = false;
	string yeolt = "jkpcflwhrjwjhljnjopyllvmtthzkcoalhvebyuetetlpqoygf";
	string nknoljzbqpur = "algvogluwnrqhycbujdkvtdyrtyyuleonpyeqbfundzqlmcjasiodneyjqonybokvscpfbo";
	int filznjfpvcewomp = 596;
	string njihbjiw = "yeclctrgggkoumlhvsbmxrxtsaatpkqbityhfpfbqvvmgcptgaljjawypcipwocecqekupethzcctsvjoseeaslvyqk";
	bool lbffdu = true;
	if (string("algvogluwnrqhycbujdkvtdyrtyyuleonpyeqbfundzqlmcjasiodneyjqonybokvscpfbo") != string("algvogluwnrqhycbujdkvtdyrtyyuleonpyeqbfundzqlmcjasiodneyjqonybokvscpfbo")) {
		int mtj;
		for (mtj = 98; mtj > 0; mtj--) {
			continue;
		}
	}
	if (string("algvogluwnrqhycbujdkvtdyrtyyuleonpyeqbfundzqlmcjasiodneyjqonybokvscpfbo") != string("algvogluwnrqhycbujdkvtdyrtyyuleonpyeqbfundzqlmcjasiodneyjqonybokvscpfbo")) {
		int rto;
		for (rto = 56; rto > 0; rto--) {
			continue;
		}
	}
	if (36277 == 36277) {
		int xvibp;
		for (xvibp = 88; xvibp > 0; xvibp--) {
			continue;
		}
	}
	return 8337;
}

int besjtmp::zryexxxaytdktfm(bool uclaytua, string ihxxfecacku, bool bkwtmsix) {
	int hlpotetwqgjpw = 4259;
	int ycfilryhrsn = 5336;
	int jgxjbfkhabjqii = 2243;
	int solagjauv = 704;
	bool qcnqfkzhpqdtlue = true;
	return 32912;
}

void besjtmp::givrtzktmsgu(string lxcvybpummx, string qvrrjdadcb, double pqtoblrswpuefs, string bsjtosohjv, string uflutml, int udiorhlriczyxg) {
	double wntiies = 14259;
	bool gaobicvvqpoynxo = true;
	string cqwuzfseojnj = "apwixsvqgmzktdycapnbswqdbrdcngahetbubopjpsjyz";
	double dizcucvts = 15354;
	string oiugludanxod = "tdzsrsumthamublqzq";
	int vojqna = 874;
	string qifirteozypk = "trnjoactxvqscfqpkkkdtlcrlfihkfkznmizgichwtjbprdncyteajjvlpv";
	int gmkocgqw = 8322;
	if (string("trnjoactxvqscfqpkkkdtlcrlfihkfkznmizgichwtjbprdncyteajjvlpv") != string("trnjoactxvqscfqpkkkdtlcrlfihkfkznmizgichwtjbprdncyteajjvlpv")) {
		int gysjghd;
		for (gysjghd = 26; gysjghd > 0; gysjghd--) {
			continue;
		}
	}
	if (string("trnjoactxvqscfqpkkkdtlcrlfihkfkznmizgichwtjbprdncyteajjvlpv") == string("trnjoactxvqscfqpkkkdtlcrlfihkfkznmizgichwtjbprdncyteajjvlpv")) {
		int exf;
		for (exf = 94; exf > 0; exf--) {
			continue;
		}
	}
	if (string("trnjoactxvqscfqpkkkdtlcrlfihkfkznmizgichwtjbprdncyteajjvlpv") != string("trnjoactxvqscfqpkkkdtlcrlfihkfkznmizgichwtjbprdncyteajjvlpv")) {
		int vkj;
		for (vkj = 64; vkj > 0; vkj--) {
			continue;
		}
	}

}

void besjtmp::tthyvvydfu(bool gdpkinrwyemb, bool rkgeolpeli, int rpagout, double adidahahpvjvt) {
	bool fspswqoxqa = true;
	string raqikhxrq = "mwo";
	int keqkclbpslui = 1340;
	double mtekn = 33862;
	if (true == true) {
		int tbyaujj;
		for (tbyaujj = 85; tbyaujj > 0; tbyaujj--) {
			continue;
		}
	}
	if (true != true) {
		int wupqlva;
		for (wupqlva = 30; wupqlva > 0; wupqlva--) {
			continue;
		}
	}
	if (true == true) {
		int dgxfeei;
		for (dgxfeei = 49; dgxfeei > 0; dgxfeei--) {
			continue;
		}
	}

}

void besjtmp::mxmorglkalyqk(int cfifkjbwdve, string dtalxpluxyk, double opcdpjkomlyu, double dyfbuqpreumypnt, int shthehswq, string cldshyd, double zedjlhpzlwv, string uyxhaapfq, string cidhy) {
	string rihgdvspnabooia = "uriojfcmajjugqvsfiaylyniypvknoldlgyuoavfgibqyvsgacicszuuaiwgqflvfihzugajkmqqowqlayfjvdycdyrbdpdfta";
	bool iiavovegf = true;
	int andhv = 327;
	int plnnvmlvyl = 4002;
	string gvxbioeoiob = "gfziunozaoiokhqckglysxweabqwzdemvmgvzqqyjvnzclafieudfvvjwgedfxsiqahxdopqjcajantuzazjsopdpipaoprccjq";
	if (327 == 327) {
		int drwavvdm;
		for (drwavvdm = 54; drwavvdm > 0; drwavvdm--) {
			continue;
		}
	}
	if (327 == 327) {
		int tzxlw;
		for (tzxlw = 70; tzxlw > 0; tzxlw--) {
			continue;
		}
	}
	if (true == true) {
		int hgz;
		for (hgz = 76; hgz > 0; hgz--) {
			continue;
		}
	}
	if (string("gfziunozaoiokhqckglysxweabqwzdemvmgvzqqyjvnzclafieudfvvjwgedfxsiqahxdopqjcajantuzazjsopdpipaoprccjq") == string("gfziunozaoiokhqckglysxweabqwzdemvmgvzqqyjvnzclafieudfvvjwgedfxsiqahxdopqjcajantuzazjsopdpipaoprccjq")) {
		int yky;
		for (yky = 16; yky > 0; yky--) {
			continue;
		}
	}

}

int besjtmp::duuqxmiglbznpi(bool oayshhxgxxjt) {
	int ihdpq = 1309;
	if (1309 == 1309) {
		int ybdmvgal;
		for (ybdmvgal = 36; ybdmvgal > 0; ybdmvgal--) {
			continue;
		}
	}
	if (1309 != 1309) {
		int szplkhch;
		for (szplkhch = 7; szplkhch > 0; szplkhch--) {
			continue;
		}
	}
	if (1309 == 1309) {
		int zvoblzd;
		for (zvoblzd = 37; zvoblzd > 0; zvoblzd--) {
			continue;
		}
	}
	return 64695;
}

bool besjtmp::ovaaoovvhxxtsnjjrmker(bool jvuagtkpcnsjt, bool xkgbzj, double zehjoro, int ghqiczzx, double lenmljpxqhpxkkb, bool qgjxcsfcp, int tbxuxidkgrbq, string xgdis) {
	bool xpkngwctn = false;
	string zqrzt = "vyrjtkcrhjmvjlebmtrcxanvn";
	int xgoqukabvksnyen = 4053;
	string xitpokn = "lsywrjxzvudlntygtrttklocbnzuzfjxvniwyicsugepqqlvcujfxdwymfirqoujxhdnmejwuczwgbhfis";
	bool mberorqeayblz = false;
	if (4053 != 4053) {
		int isoi;
		for (isoi = 72; isoi > 0; isoi--) {
			continue;
		}
	}
	if (string("lsywrjxzvudlntygtrttklocbnzuzfjxvniwyicsugepqqlvcujfxdwymfirqoujxhdnmejwuczwgbhfis") == string("lsywrjxzvudlntygtrttklocbnzuzfjxvniwyicsugepqqlvcujfxdwymfirqoujxhdnmejwuczwgbhfis")) {
		int xxqiwvna;
		for (xxqiwvna = 64; xxqiwvna > 0; xxqiwvna--) {
			continue;
		}
	}
	if (false != false) {
		int jpunv;
		for (jpunv = 28; jpunv > 0; jpunv--) {
			continue;
		}
	}
	return true;
}

double besjtmp::eeqytbzssrnbc(int mdrllhthorfxdwh, double szfttyeodpdam, bool iftkq, bool pdxjgb, string ldpslqbdrwt, string xqfzhwadl, string qmejcug, double joqrvn) {
	bool nlgxqtft = false;
	if (false == false) {
		int ic;
		for (ic = 28; ic > 0; ic--) {
			continue;
		}
	}
	if (false != false) {
		int mu;
		for (mu = 34; mu > 0; mu--) {
			continue;
		}
	}
	if (false == false) {
		int ixf;
		for (ixf = 71; ixf > 0; ixf--) {
			continue;
		}
	}
	if (false != false) {
		int bjmyqvl;
		for (bjmyqvl = 85; bjmyqvl > 0; bjmyqvl--) {
			continue;
		}
	}
	return 96767;
}

double besjtmp::jhilgqulht(bool rvthvhxja, string nokozeaqp, string cervjh, string ypgseoosfkavzog, bool wdbgggiakjenlsk, double gizjujtteuaj, double jxksglg) {
	int owbifpbxgy = 1558;
	if (1558 == 1558) {
		int hcakvmw;
		for (hcakvmw = 37; hcakvmw > 0; hcakvmw--) {
			continue;
		}
	}
	return 31983;
}

int besjtmp::dodfuxzeehkfuwwfqrkj(bool kfjddmptdhprymp, int lijeobjxxxlvmg, bool jntfd, string agakncnxczek, double qzwzwd, int lnmbnacm, int rlduxdohycgsu, int oqbglmgi, bool ygxcoznb) {
	string eldutbjog = "prsyqd";
	return 38503;
}

double besjtmp::frnkyqpmjuksgtdgsxvcrqhd(string pgbyba, double moryovbxygdii, bool dvkhhadmecav, bool kxjlh, double xjhduzlemxhaz, int tsoenbjj, string reitrlk, int pjgvyaiebv) {
	bool piompldfhlxj = true;
	int argbnyfq = 7691;
	double oulrekgg = 92518;
	double rncffwvpxtts = 22990;
	double bohuquklqjqv = 50046;
	if (22990 == 22990) {
		int ibbycfyic;
		for (ibbycfyic = 60; ibbycfyic > 0; ibbycfyic--) {
			continue;
		}
	}
	return 11826;
}

string besjtmp::ztipbbpxetgnk(double udyyc, double kyybon, int bxnpuxj, string mdbdgyckhghucog) {
	bool qscyu = false;
	double nbjlips = 61792;
	int bcfdoxatpcqgugw = 1301;
	int slrpp = 8162;
	int quixgnejjvl = 2666;
	bool mnrrne = true;
	string xnzxdouffmlvd = "fwzsbkhabobqyddu";
	if (2666 == 2666) {
		int exnfs;
		for (exnfs = 93; exnfs > 0; exnfs--) {
			continue;
		}
	}
	if (61792 != 61792) {
		int huc;
		for (huc = 81; huc > 0; huc--) {
			continue;
		}
	}
	return string("mjzmokzhxainzkthqeo");
}

void besjtmp::cqpydpqxibp() {
	int jlyfpllaxikxjx = 1346;
	double votvicj = 39473;
	bool mfbfagxj = true;
	double teerndhweyp = 6902;
	string xfrztpctpu = "ixjvwkfkyqnhtzaapzlqxoaqwjvhhotlkrepkcmpuabqrczmqmxerszbwmkbmargkhsmulx";
	if (6902 == 6902) {
		int rzkalzhlvc;
		for (rzkalzhlvc = 98; rzkalzhlvc > 0; rzkalzhlvc--) {
			continue;
		}
	}
	if (1346 == 1346) {
		int xe;
		for (xe = 35; xe > 0; xe--) {
			continue;
		}
	}
	if (true == true) {
		int qm;
		for (qm = 73; qm > 0; qm--) {
			continue;
		}
	}
	if (1346 == 1346) {
		int ojmvfcahby;
		for (ojmvfcahby = 34; ojmvfcahby > 0; ojmvfcahby--) {
			continue;
		}
	}
	if (string("ixjvwkfkyqnhtzaapzlqxoaqwjvhhotlkrepkcmpuabqrczmqmxerszbwmkbmargkhsmulx") != string("ixjvwkfkyqnhtzaapzlqxoaqwjvhhotlkrepkcmpuabqrczmqmxerszbwmkbmargkhsmulx")) {
		int vhwqfxxoj;
		for (vhwqfxxoj = 86; vhwqfxxoj > 0; vhwqfxxoj--) {
			continue;
		}
	}

}

besjtmp::besjtmp() {
	this->cqpydpqxibp();
	this->ovaaoovvhxxtsnjjrmker(true, true, 23163, 925, 3891, true, 3228, string("gjgnhqkmgncljnheqdkwagprwbnwtspglsunlbstjcdmudq"));
	this->eeqytbzssrnbc(2914, 28843, true, true, string(""), string("naflhuizwjiyicuwazoikwefpioaeynuywmnpcadqrfufablzflvdelxpszbrhsrxupyfey"), string("xupclpvgxvpvshugadmcigxoxonj"), 4779);
	this->jhilgqulht(false, string("dvwinksffjwpybiowjfbvophrhcnepvncclsidqlw"), string("gdkshuzwqzagrmgjtmgxikvcrsjhnajwjbbclslfvcrumryaevtfehwnmtsqzyrduzljniivj"), string("akvyolqatusmqllvnsdhqlkbievfzbgxsqagkcyziobye"), true, 11321, 15597);
	this->dodfuxzeehkfuwwfqrkj(false, 1781, false, string("opteeprpuzqjj"), 36288, 1711, 697, 4127, false);
	this->frnkyqpmjuksgtdgsxvcrqhd(string("ukcwtktrmmjgmfhkyqpvbpfjfwbpjbsoumbshbanswdaypvgzbjgpezcm"), 60441, true, false, 7210, 1329, string("waitvbofzigplsfsgonaebmtizpqntxjutgcwjy"), 3621);
	this->ztipbbpxetgnk(2176, 23437, 2190, string("sbitxnbzmibpyheuuvvzenxrrjiykbtuzdlxzlbykiynmhonvsqbxwoscrxgwwokdx"));
	this->nbqduytzzmrqblowuxbv(9891, 599);
	this->zryexxxaytdktfm(true, string("mlrs"), true);
	this->givrtzktmsgu(string("abiozepnnjespuwjlxbajoxzwrsdtrmtijcfbusceyqqooboathn"), string("btsyhcdcjccogleptcyhpeiopaprxapxqpcdynnbkqsddevrdpgxydoeilfkunrcblwkosqlfaonbiqvcydgmhnezqdpc"), 4415, string("lsylatgnedzzjvboixvisd"), string("vysvutxcckkxstforqlxlfqchgwmlclspxxrfnbcsortbbhigprwuwdzvuabufyuokhbonlzuvbxlefq"), 2339);
	this->tthyvvydfu(true, false, 1528, 32786);
	this->mxmorglkalyqk(5808, string("wwibtpqklzczjzmrqhjlhwvevmxpaccflgmmmtrooyrawtdxjzptdmwrmhtjwluybeifywesdwsgssrntdbw"), 3599, 7455, 1238, string("zyaceqdaiancoldlkbjmvrjncntytkprhonuzbeyjrcgyejzivfgyfiwukvjvteiohmcyciayjqagshaseqyvptxexjvmzf"), 14779, string("djqgezvqtjsgufcmscewvcdzmqwzaskjedmpqaewjetxvdqgjeqddvvuzakvauymhhfihoux"), string("fwjyazman"));
	this->duuqxmiglbznpi(false);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class hvwkgye {
public:
	int inlrzabin;
	int gjrzratoqm;
	double iphlynbojuibzpd;
	string fxsyuonsspmw;
	hvwkgye();
	double llgnkluvwjbegrrjlcuebdac(double tkpdz, double nypnecffxhots, bool ilzwf, double wizuz, double bzysmxnm, double jskzfomkqn, string fksmf, int wsighyo, bool rxgvgvqreqi, string zyawrydihklg);
	double hbaazvxqyhuhqb(string xveueppob, bool vtiwzmtqal, int vbcwdq, double svfqdcheze, double cbafqevyz);
	bool nmxolpexlzvyjqvj(string wmsoqc, int owejgycxnhodap, int byyng, double swxrubxvspz, bool qndnldrvl);
	int eucxvlhktjp(bool dxwndiltkbn, int ksvnpckvgozxpze, int ykrua, string ouhrtspmkcl, int tpmruqqkaraugtq, bool nwwwpknbs, int ezfijwkitcmq);
	void bpmmyxkszpycbals(double spvwxj, bool rhztqqdbbhnnetc, double eiofoukgwzdvl);
	string lsfmltxzbhovkexaiiqyhpjw(string dowuy, int gwowyman, string ighze, bool jduaaodgrhchixk, string bovyapdcj, double arzksczidlaqkru, double smzetj, double vsfdbnicvsgiqzo, string frcibvrylkbvic);
	string nwowujgkmuttudmezi(int rduzno, int unmwzpf, int bymxz, double pqooteuysqzmz, string bdfjohakhy);
	bool jqkxunufuffxco(double szatm);

protected:
	string nzqrnl;
	string ajxobpbi;
	string chwig;

	void webwamanxklkl(int kswncccx, int smbedxvccepmkdc, double cmtvlneuomxq);

private:
	string mpggzonx;
	double cjjdja;
	string qwqmeskxooyjbjs;
	bool rfexglw;
	int nnaov;

	double ydowgpirurtebubtu(int atiomaxbci);
	string fxqowuymnbuukedmwfzovart(double lcgjrtivlayn, int qsiyebn, string keyrnepiqqo, int djfvzdrk, bool aqpfsagkdksfbi, double cweyr, int wurctebjpf);
	void mobeyllxfascfnhsxpu(string hkjgzm, double zxyoygjzfybmkk, bool nbywhgbqj, bool ubsziogpcgzcq, int cqjmxczxtq, bool fgrtvh, string ydgpktnrzfnxvec);
	bool ltrcvsgpqutmakxzpizfq(bool wlgldyxo, int hlqbze, int yqyftjfsblcgmkn, string nonpmemjkvdn, double vusfeamhpsdx);
	int qnaqvjqeutzfykgusw(int lzodmayb, string yiocfwxlryxit, bool mckrfwpm, string qxkumqrps, int urkfpvoduihoge, double imhufxfglomzxck, int yuhhjuaxnn, bool wviaaqpaquq, int brapkspsrzk, string dqkfihwezrmjhu);
	int vvjyduohqdx(string bhumosbqerf, string hhdff, bool untvvqcneeiie);
	int tdqjlxuufrgzpbighnyd(int fqqsmxvpmpbbj);

};


double hvwkgye::ydowgpirurtebubtu(int atiomaxbci) {
	bool cvtnpmte = true;
	int uvjqrubxmqvxlwv = 1807;
	bool xpqesnxsw = false;
	bool bilulfaxoudmx = true;
	int tijwqnswhgich = 1529;
	bool jluqzgaiqjbx = false;
	if (true != true) {
		int zul;
		for (zul = 91; zul > 0; zul--) {
			continue;
		}
	}
	if (1807 == 1807) {
		int dtzmrx;
		for (dtzmrx = 72; dtzmrx > 0; dtzmrx--) {
			continue;
		}
	}
	if (true != true) {
		int md;
		for (md = 86; md > 0; md--) {
			continue;
		}
	}
	if (false == false) {
		int ktisakq;
		for (ktisakq = 54; ktisakq > 0; ktisakq--) {
			continue;
		}
	}
	return 25262;
}

string hvwkgye::fxqowuymnbuukedmwfzovart(double lcgjrtivlayn, int qsiyebn, string keyrnepiqqo, int djfvzdrk, bool aqpfsagkdksfbi, double cweyr, int wurctebjpf) {
	int icnpciijaezwzyb = 3090;
	double tssyivg = 15737;
	bool huilstui = false;
	bool yreshvmzwtyv = false;
	string jzbkquepn = "ckrkxjoowfqbvhjoazruhrzopkklt";
	string tlosoacsklfovb = "jbjnxqykwhwnshyuxvqlmzkgcubqilabc";
	int orzwmocoi = 3332;
	string ksaqxsjve = "rhzyutlodwfawaspjawjfipcmzadfofsakocpdhilkonbjfffxpffwhtxgcbmhmisjqshbowtrkzzmjgqlzsvtifjxieecdve";
	string earaai = "fcbhxfibseqlmuyxodltgkbfavdxonzcmi";
	if (string("fcbhxfibseqlmuyxodltgkbfavdxonzcmi") == string("fcbhxfibseqlmuyxodltgkbfavdxonzcmi")) {
		int nnamivkzaj;
		for (nnamivkzaj = 55; nnamivkzaj > 0; nnamivkzaj--) {
			continue;
		}
	}
	if (false != false) {
		int bdptbft;
		for (bdptbft = 21; bdptbft > 0; bdptbft--) {
			continue;
		}
	}
	return string("fg");
}

void hvwkgye::mobeyllxfascfnhsxpu(string hkjgzm, double zxyoygjzfybmkk, bool nbywhgbqj, bool ubsziogpcgzcq, int cqjmxczxtq, bool fgrtvh, string ydgpktnrzfnxvec) {
	bool lxsifyiumevsfl = true;
	if (true != true) {
		int rvikjknac;
		for (rvikjknac = 26; rvikjknac > 0; rvikjknac--) {
			continue;
		}
	}

}

bool hvwkgye::ltrcvsgpqutmakxzpizfq(bool wlgldyxo, int hlqbze, int yqyftjfsblcgmkn, string nonpmemjkvdn, double vusfeamhpsdx) {
	double unnnmvgckindhrf = 20446;
	string maphizepbpelp = "fncullsxpuuxywytgnanymtblbczafjkmpkungquzejhfsw";
	if (20446 != 20446) {
		int sixt;
		for (sixt = 39; sixt > 0; sixt--) {
			continue;
		}
	}
	if (20446 == 20446) {
		int ijvejerfey;
		for (ijvejerfey = 55; ijvejerfey > 0; ijvejerfey--) {
			continue;
		}
	}
	if (20446 != 20446) {
		int wv;
		for (wv = 16; wv > 0; wv--) {
			continue;
		}
	}
	return true;
}

int hvwkgye::qnaqvjqeutzfykgusw(int lzodmayb, string yiocfwxlryxit, bool mckrfwpm, string qxkumqrps, int urkfpvoduihoge, double imhufxfglomzxck, int yuhhjuaxnn, bool wviaaqpaquq, int brapkspsrzk, string dqkfihwezrmjhu) {
	double opjypkkbqnyz = 19096;
	double lcdmmhkxtfxznr = 6429;
	int qsggqemcjcnyntc = 3706;
	double hlvxtdlpjubkge = 4125;
	if (3706 == 3706) {
		int weynio;
		for (weynio = 91; weynio > 0; weynio--) {
			continue;
		}
	}
	if (6429 != 6429) {
		int kcl;
		for (kcl = 89; kcl > 0; kcl--) {
			continue;
		}
	}
	if (4125 == 4125) {
		int zolwrj;
		for (zolwrj = 99; zolwrj > 0; zolwrj--) {
			continue;
		}
	}
	return 4935;
}

int hvwkgye::vvjyduohqdx(string bhumosbqerf, string hhdff, bool untvvqcneeiie) {
	int fpswbwqwbixgor = 723;
	if (723 != 723) {
		int sgc;
		for (sgc = 40; sgc > 0; sgc--) {
			continue;
		}
	}
	if (723 != 723) {
		int xdctfyie;
		for (xdctfyie = 66; xdctfyie > 0; xdctfyie--) {
			continue;
		}
	}
	if (723 != 723) {
		int sytmkt;
		for (sytmkt = 67; sytmkt > 0; sytmkt--) {
			continue;
		}
	}
	if (723 == 723) {
		int ugyhhndil;
		for (ugyhhndil = 40; ugyhhndil > 0; ugyhhndil--) {
			continue;
		}
	}
	if (723 != 723) {
		int tyagn;
		for (tyagn = 2; tyagn > 0; tyagn--) {
			continue;
		}
	}
	return 43495;
}

int hvwkgye::tdqjlxuufrgzpbighnyd(int fqqsmxvpmpbbj) {
	double hvrfmizao = 31350;
	int oxwikip = 3661;
	string qqdsnbpsbautwvx = "mniooxvwifqyncbvjajwbgsvuezhwalgkejjomqghunxuwbvrxbbdysjfnjk";
	string tkbklrfiitkrf = "zruouquwtbihzzxzsnxsbp";
	bool nmtps = true;
	int sxbizsfetiggt = 2850;
	double dlwqqp = 65448;
	string qhzthdyzh = "xqotphnvmnsjosbbaczqzhrkqkjirbmtsrnjjjtyfxtccwjyvcpqjkfdfagjozlmrlefroddkzrhddrtdwqvsxtlh";
	int odxcdiq = 44;
	bool wykca = false;
	if (2850 != 2850) {
		int qy;
		for (qy = 38; qy > 0; qy--) {
			continue;
		}
	}
	if (3661 == 3661) {
		int edxqaonlya;
		for (edxqaonlya = 81; edxqaonlya > 0; edxqaonlya--) {
			continue;
		}
	}
	if (65448 == 65448) {
		int tolpob;
		for (tolpob = 53; tolpob > 0; tolpob--) {
			continue;
		}
	}
	if (44 != 44) {
		int lbuqhipo;
		for (lbuqhipo = 60; lbuqhipo > 0; lbuqhipo--) {
			continue;
		}
	}
	return 98913;
}

void hvwkgye::webwamanxklkl(int kswncccx, int smbedxvccepmkdc, double cmtvlneuomxq) {
	int neeldxkjqas = 122;
	string zgolbblzz = "fbdjejtnorexbcwaypsquqhsdsdcknwmugsexjlvgqtfl";
	double oaqxbc = 78803;
	int dekbmbedsczj = 5405;
	string yfhsgahcm = "yfctdimmiooflrhqxrqpjasmrzvvvsqfclcvxellnoyfgzu";
	string epyuuiixv = "zcxadojzhgiamagobdehzfstrwbjrfikogdoixvkioipmrpnpfrgcbgkjmqfuhdlheteqzegmvrremxyen";
	if (5405 != 5405) {
		int ixvxi;
		for (ixvxi = 20; ixvxi > 0; ixvxi--) {
			continue;
		}
	}
	if (string("yfctdimmiooflrhqxrqpjasmrzvvvsqfclcvxellnoyfgzu") != string("yfctdimmiooflrhqxrqpjasmrzvvvsqfclcvxellnoyfgzu")) {
		int viqlcv;
		for (viqlcv = 45; viqlcv > 0; viqlcv--) {
			continue;
		}
	}
	if (122 == 122) {
		int mr;
		for (mr = 5; mr > 0; mr--) {
			continue;
		}
	}
	if (78803 == 78803) {
		int njyhgelgc;
		for (njyhgelgc = 53; njyhgelgc > 0; njyhgelgc--) {
			continue;
		}
	}
	if (string("zcxadojzhgiamagobdehzfstrwbjrfikogdoixvkioipmrpnpfrgcbgkjmqfuhdlheteqzegmvrremxyen") == string("zcxadojzhgiamagobdehzfstrwbjrfikogdoixvkioipmrpnpfrgcbgkjmqfuhdlheteqzegmvrremxyen")) {
		int fsiou;
		for (fsiou = 55; fsiou > 0; fsiou--) {
			continue;
		}
	}

}

double hvwkgye::llgnkluvwjbegrrjlcuebdac(double tkpdz, double nypnecffxhots, bool ilzwf, double wizuz, double bzysmxnm, double jskzfomkqn, string fksmf, int wsighyo, bool rxgvgvqreqi, string zyawrydihklg) {
	return 869;
}

double hvwkgye::hbaazvxqyhuhqb(string xveueppob, bool vtiwzmtqal, int vbcwdq, double svfqdcheze, double cbafqevyz) {
	int jejgjkxtnztmivr = 4841;
	double ziegvhwfcn = 4767;
	bool sinbdukmhbxb = false;
	bool hypglwjj = false;
	string honatldo = "";
	double sjlbvzqpx = 18332;
	bool bngaymcyjdy = true;
	int hyyxfe = 1974;
	int hgrpgnbdwyat = 5210;
	int fwpwbzalykyi = 320;
	if (true == true) {
		int nuzcs;
		for (nuzcs = 61; nuzcs > 0; nuzcs--) {
			continue;
		}
	}
	if (true != true) {
		int stjtjtp;
		for (stjtjtp = 70; stjtjtp > 0; stjtjtp--) {
			continue;
		}
	}
	if (false == false) {
		int yzbpsxkpr;
		for (yzbpsxkpr = 50; yzbpsxkpr > 0; yzbpsxkpr--) {
			continue;
		}
	}
	if (4841 == 4841) {
		int gfdzlylx;
		for (gfdzlylx = 96; gfdzlylx > 0; gfdzlylx--) {
			continue;
		}
	}
	return 61006;
}

bool hvwkgye::nmxolpexlzvyjqvj(string wmsoqc, int owejgycxnhodap, int byyng, double swxrubxvspz, bool qndnldrvl) {
	return false;
}

int hvwkgye::eucxvlhktjp(bool dxwndiltkbn, int ksvnpckvgozxpze, int ykrua, string ouhrtspmkcl, int tpmruqqkaraugtq, bool nwwwpknbs, int ezfijwkitcmq) {
	int jyksvv = 4283;
	int kmrgmkdjmqwb = 1065;
	string ngnytdjl = "dgaeqojdenkzgicchmaxgcuetgicbecknohanhctpekwlmttckbcxqpscqqmqpzslyexdvsesdzl";
	string ucljpxduqn = "slwvwufhgzvhypgwlrjfaaydrcmjrhbirpncyeewwvdeclomabvbsltdirwrkomzdippbxqeqbnihzgolijbaaqjt";
	if (4283 == 4283) {
		int wgxjqa;
		for (wgxjqa = 80; wgxjqa > 0; wgxjqa--) {
			continue;
		}
	}
	if (4283 == 4283) {
		int jwb;
		for (jwb = 12; jwb > 0; jwb--) {
			continue;
		}
	}
	return 88113;
}

void hvwkgye::bpmmyxkszpycbals(double spvwxj, bool rhztqqdbbhnnetc, double eiofoukgwzdvl) {
	string kfzdntydglhhzar = "wloivfhaqfohmkcwxqnvsmnbsccodv";
	bool tomxni = false;
	string ghepvpetgmlt = "mlrpkzirsxvuafvqmzk";
	if (string("mlrpkzirsxvuafvqmzk") != string("mlrpkzirsxvuafvqmzk")) {
		int vdag;
		for (vdag = 38; vdag > 0; vdag--) {
			continue;
		}
	}

}

string hvwkgye::lsfmltxzbhovkexaiiqyhpjw(string dowuy, int gwowyman, string ighze, bool jduaaodgrhchixk, string bovyapdcj, double arzksczidlaqkru, double smzetj, double vsfdbnicvsgiqzo, string frcibvrylkbvic) {
	return string("");
}

string hvwkgye::nwowujgkmuttudmezi(int rduzno, int unmwzpf, int bymxz, double pqooteuysqzmz, string bdfjohakhy) {
	string fhgxs = "pxfwnrjnrvfyswehbsimrmxijkbptnzqdteibhmqtsepq";
	int cfogplxat = 3642;
	double pffxp = 54733;
	double ekvxp = 56960;
	string ijpmbwsmrvltptx = "olhmvegdrkubzsyiukhod";
	double dthmgmuhwtsbkgy = 978;
	double pahyzeaxvnkfv = 36096;
	if (string("olhmvegdrkubzsyiukhod") == string("olhmvegdrkubzsyiukhod")) {
		int toweostxn;
		for (toweostxn = 37; toweostxn > 0; toweostxn--) {
			continue;
		}
	}
	if (36096 == 36096) {
		int jzzfx;
		for (jzzfx = 95; jzzfx > 0; jzzfx--) {
			continue;
		}
	}
	return string("haoneopijc");
}

bool hvwkgye::jqkxunufuffxco(double szatm) {
	bool wplcqtlklneio = false;
	bool cdziykvesfi = false;
	if (false != false) {
		int nsa;
		for (nsa = 19; nsa > 0; nsa--) {
			continue;
		}
	}
	if (false == false) {
		int uxqmcazf;
		for (uxqmcazf = 19; uxqmcazf > 0; uxqmcazf--) {
			continue;
		}
	}
	if (false != false) {
		int rr;
		for (rr = 20; rr > 0; rr--) {
			continue;
		}
	}
	if (false != false) {
		int xhocf;
		for (xhocf = 58; xhocf > 0; xhocf--) {
			continue;
		}
	}
	return false;
}

hvwkgye::hvwkgye() {
	this->llgnkluvwjbegrrjlcuebdac(1889, 7393, true, 30006, 91081, 50984, string("esjwrmrmtpfnvocqbddvxtczrfwnbupcnzikhopqjbjlpy"), 1094, false, string("elromklquoxnscxduurrclnhgcrnnfalqmbezfvkmljaylfouyeeqf"));
	this->hbaazvxqyhuhqb(string("tsimvlgrbzwjfdzepwupyzttflwvyvhbocecjghmximdkjftavx"), false, 1423, 37905, 13192);
	this->nmxolpexlzvyjqvj(string("tvgcfffkxueqitlwynncxbzbfbckwatgxc"), 1893, 1191, 47083, false);
	this->eucxvlhktjp(true, 2891, 4200, string("jujzkquausrmuohcezchwxyyqdmlnylhpaudzojahcisqmevqxlbu"), 202, true, 2494);
	this->bpmmyxkszpycbals(74081, true, 6525);
	this->lsfmltxzbhovkexaiiqyhpjw(string("gstpyamyygvnngrgvyaenuyazxpufkemnzowjvhqcvqosaklbpugsfdljgxuvcvpthrgpkkoeujptgoax"), 5186, string("jnjzinenfuzorgrhnthiayqudpfxwekaspkkoofplqsagmtmhiswffawcvjkamibakfnvjasucvmkfkmezpijmwneyduntad"), false, string("cfqoxrxgmzgvwboauekdzmrsxafyownz"), 3882, 64867, 4601, string("ofsgeysjkfayufvhbnmnqhzwt"));
	this->nwowujgkmuttudmezi(933, 4183, 5520, 17857, string("lkcgsfrq"));
	this->jqkxunufuffxco(29534);
	this->webwamanxklkl(2545, 495, 970);
	this->ydowgpirurtebubtu(229);
	this->fxqowuymnbuukedmwfzovart(48948, 888, string("ubxnjniqhtmk"), 885, true, 8682, 1629);
	this->mobeyllxfascfnhsxpu(string("zlpduztyjzwwnzcohtflcyysfsfytuqnovdvhgfwmdovyyankgfw"), 3554, false, false, 681, false, string("ofozcnviaabtxnxyynnyuniputogymfspqrytsyyjprlfhttuxjkyorkearkaczti"));
	this->ltrcvsgpqutmakxzpizfq(false, 1178, 4264, string("olybmiaalwlzj"), 31137);
	this->qnaqvjqeutzfykgusw(2103, string("odpyddgizinpi"), false, string("owyfeitbdtpiros"), 172, 2074, 296, false, 4384, string("dkzeajulqobbvjdflxgtsqlntqrrwqxaycxztkvpgwbhuzkebtwhenaearywvtwicgwjznpqdf"));
	this->vvjyduohqdx(string("oadinhkjvkocraxptxqqqxvghibqlgadzmbkrnuddklhdxrmewrkaejozvwzfjfludjyqv"), string("ihjqthyqnugvoqgtx"), false);
	this->tdqjlxuufrgzpbighnyd(429);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class nkdzalm {
public:
	string xwuuywqyxhy;
	double yzthhsttchbnl;
	nkdzalm();
	bool svrwxyhdlljruwcnd(string oztqaxxei, bool lwirud, bool havxvuw, double zryrule, bool vtzsmirf, bool ohzqxghog, int ckepudcfr, bool swcoqay);
	int usfvtndazkhmz(double xkycaqbkorbab, double jcmzvsmnuu, bool cbvxzatdtgw, bool yybqype);
	double zsihgdrbsfqxo();
	string wlwaswhofnpi(int aujwgpajghovup, bool cwpiqxx, int rlxvuvpmakcct);
	void ovivvfqaxmzzzatklx(int ctlmirvivxaylg, int vhuus, string qummngk, string hrwieqnemh, double hxshp, int eykjvglmcioao, int hsqkavbawx);
	int yaqmygajlfd(int mwhuqdphrwxdhtz, int owxhahjzb, string efwpoafa, string krgnxocii);
	string fgmceualrhqhzjbhuxmuf(double tzenfwhpzc, int oqsubsy);
	double lbdanzlpoborj(double unzohshd, string jfamxsihhloim, bool tmunafxsz, bool vagcvm, double hfvjkyvce, bool oogvydkjzmpepr, string nyzpgxn, int drvhjeyth, string xoivakepwrwud);
	void ugpnzpzutoqwmnsjdt(int hwinm);

protected:
	double hxbtvnuccojdb;
	string vdpcq;
	string zrvqctkldwwuu;
	bool lhulz;
	double qzeroqla;

	void dzhlmejyexshnkgmn();
	string hbmsriyntcdejjp(int lfatccnjwuxcg);
	bool veslliujjpallmgtllzd(int rhlodxwwaprc, string sehzdeatoh, bool mqcifckjl, string wcohv, bool ixdyy);

private:
	string nhvnqkkvkc;
	bool jxvwqnytbep;

	double fhxlpbecdfbjnzqfnxe(bool vzdxgvewqsomzy, string xswrbsc, double crmobggewhvvsk, bool yhglbogrc, int lffesrj, string jnwhqbm);
	int knsgpdwqvuik(double umbkykmwhazcdnp, bool ixaqh, bool uiiiigrzrgjzhl, string fqpcediq, double catsd, bool oermexkcubab, string axgyu, string bxlapdwcfb, string ujjarejx, bool chgguypzdlgas);
	int dxyepfpooqvawymequocbqa(double jlpasxlsey, int acpsjkr, int knhydn);
	double xgmniajakncuhshwif(int jkfgerth, int zgzkszz, string pvqieffvd, string gqyvzwsdlpeav, int qgwsuxl);

};


double nkdzalm::fhxlpbecdfbjnzqfnxe(bool vzdxgvewqsomzy, string xswrbsc, double crmobggewhvvsk, bool yhglbogrc, int lffesrj, string jnwhqbm) {
	string uiietxkpv = "ktlfgacsivkioqnkmcwyrsxzikmltuokkcokdrxggyrrrobigptxglarh";
	bool fiubryfggj = false;
	if (false != false) {
		int iqafwejdhl;
		for (iqafwejdhl = 47; iqafwejdhl > 0; iqafwejdhl--) {
			continue;
		}
	}
	if (false != false) {
		int exhj;
		for (exhj = 66; exhj > 0; exhj--) {
			continue;
		}
	}
	if (string("ktlfgacsivkioqnkmcwyrsxzikmltuokkcokdrxggyrrrobigptxglarh") != string("ktlfgacsivkioqnkmcwyrsxzikmltuokkcokdrxggyrrrobigptxglarh")) {
		int puzqqx;
		for (puzqqx = 6; puzqqx > 0; puzqqx--) {
			continue;
		}
	}
	return 29926;
}

int nkdzalm::knsgpdwqvuik(double umbkykmwhazcdnp, bool ixaqh, bool uiiiigrzrgjzhl, string fqpcediq, double catsd, bool oermexkcubab, string axgyu, string bxlapdwcfb, string ujjarejx, bool chgguypzdlgas) {
	double xjngtjkgwivht = 29884;
	bool oimkrbclp = false;
	bool vjpjedunghxwz = false;
	string vrlho = "ncdqehhpjweumszryyxsnbojhyzgwvogbfucdusvyktluikyuubvzjubjyaxqpqpjriyxuufyneuehnimdte";
	double yvdclm = 10329;
	bool vgqhj = false;
	bool ntzkcrnkt = false;
	if (10329 == 10329) {
		int ujrl;
		for (ujrl = 38; ujrl > 0; ujrl--) {
			continue;
		}
	}
	if (false == false) {
		int flrqeg;
		for (flrqeg = 85; flrqeg > 0; flrqeg--) {
			continue;
		}
	}
	if (string("ncdqehhpjweumszryyxsnbojhyzgwvogbfucdusvyktluikyuubvzjubjyaxqpqpjriyxuufyneuehnimdte") == string("ncdqehhpjweumszryyxsnbojhyzgwvogbfucdusvyktluikyuubvzjubjyaxqpqpjriyxuufyneuehnimdte")) {
		int fksfrwrvb;
		for (fksfrwrvb = 47; fksfrwrvb > 0; fksfrwrvb--) {
			continue;
		}
	}
	if (false != false) {
		int zqgfobt;
		for (zqgfobt = 100; zqgfobt > 0; zqgfobt--) {
			continue;
		}
	}
	return 72691;
}

int nkdzalm::dxyepfpooqvawymequocbqa(double jlpasxlsey, int acpsjkr, int knhydn) {
	string rnsurntajltcn = "rlvba";
	string xtiafyuvb = "rfrmdtciegiwjrsnvgftdhiadqzpppxwaglpmfgavabpwtlxqkxiqdehmylrhvjsqufuijvoledqgultpdffp";
	int abkpxoskz = 1693;
	string uphicgpbjnm = "lhgatqexnqhzmkuez";
	if (string("rfrmdtciegiwjrsnvgftdhiadqzpppxwaglpmfgavabpwtlxqkxiqdehmylrhvjsqufuijvoledqgultpdffp") == string("rfrmdtciegiwjrsnvgftdhiadqzpppxwaglpmfgavabpwtlxqkxiqdehmylrhvjsqufuijvoledqgultpdffp")) {
		int hrer;
		for (hrer = 41; hrer > 0; hrer--) {
			continue;
		}
	}
	if (string("rlvba") != string("rlvba")) {
		int mhe;
		for (mhe = 13; mhe > 0; mhe--) {
			continue;
		}
	}
	return 8014;
}

double nkdzalm::xgmniajakncuhshwif(int jkfgerth, int zgzkszz, string pvqieffvd, string gqyvzwsdlpeav, int qgwsuxl) {
	int dglzdg = 4601;
	int ueigfqyetrz = 1044;
	int esvclpohpmgbsll = 4699;
	if (1044 == 1044) {
		int xeb;
		for (xeb = 48; xeb > 0; xeb--) {
			continue;
		}
	}
	return 70951;
}

void nkdzalm::dzhlmejyexshnkgmn() {
	double arhmiymqntzjwo = 3741;
	int eurcddrr = 416;
	bool jryiurn = true;
	double dbtjysfzizfrli = 21874;
	int tnxar = 4144;
	string yfzrpk = "zrshvhljhynmbebqyetfrxzmmkdfsmjyjgaiusbfyczbgbloyvyufnspcjiqywrfavvjguwvtazhuplrvqe";
	bool itqtbkqbavgt = true;
	if (true == true) {
		int pbkwgdzirn;
		for (pbkwgdzirn = 44; pbkwgdzirn > 0; pbkwgdzirn--) {
			continue;
		}
	}
	if (true == true) {
		int be;
		for (be = 21; be > 0; be--) {
			continue;
		}
	}
	if (true == true) {
		int jf;
		for (jf = 23; jf > 0; jf--) {
			continue;
		}
	}

}

string nkdzalm::hbmsriyntcdejjp(int lfatccnjwuxcg) {
	bool cegeqomxzna = true;
	double rmzqibkhzfrykj = 36095;
	return string("foyfba");
}

bool nkdzalm::veslliujjpallmgtllzd(int rhlodxwwaprc, string sehzdeatoh, bool mqcifckjl, string wcohv, bool ixdyy) {
	int ifysjdimth = 2421;
	string uinnpgegjvcw = "jlcfovjsznlxveakglfevlhpivaggkvkhmuevkgqfmlrtokfrcnuxrzlaawwqhaqklldkphflxvyavdjjyoxcxrqlde";
	double afokeecrgasntst = 12538;
	double pxoamko = 5909;
	string iydbojasvoirkv = "ektywqtresfhfqrigsyfpuupctoupzzpbzokgsnpemeayywytwjogqvjhutazpvukgzeoioavsz";
	int twigdy = 806;
	string yivsnig = "hgxbydbhflxvfvkdnzpyjgwjzypkkovncqpmqmrmwypiomaqau";
	int jimklhnmmt = 878;
	if (2421 != 2421) {
		int fvanqw;
		for (fvanqw = 56; fvanqw > 0; fvanqw--) {
			continue;
		}
	}
	if (string("hgxbydbhflxvfvkdnzpyjgwjzypkkovncqpmqmrmwypiomaqau") != string("hgxbydbhflxvfvkdnzpyjgwjzypkkovncqpmqmrmwypiomaqau")) {
		int gmlfdp;
		for (gmlfdp = 81; gmlfdp > 0; gmlfdp--) {
			continue;
		}
	}
	if (12538 != 12538) {
		int zym;
		for (zym = 46; zym > 0; zym--) {
			continue;
		}
	}
	return true;
}

bool nkdzalm::svrwxyhdlljruwcnd(string oztqaxxei, bool lwirud, bool havxvuw, double zryrule, bool vtzsmirf, bool ohzqxghog, int ckepudcfr, bool swcoqay) {
	int htzclyaoeebung = 5417;
	int zjcrxsf = 865;
	int rtxpfcogjgkvbiy = 1301;
	string apuqeon = "pkmhmgobvvzogvtmjpxnvtjafgbbilgrvhznoxjxjiarmnfyws";
	string szgjhuyjerjw = "xozjgsgeevneqieapyjtthcxxgufwtwxznijtpeaoxtuakmctkwmdbwsmbcupayjsbu";
	int eebswpfu = 6165;
	string bswfjrku = "ayzexebohtdtfokfuomyjbitauwprcrhogsauydbiglowojitxlifjnbonynbphjjdjbrwrsdvpeqixa";
	int ikasgdkqqcpduvj = 1168;
	string cpclfmtblmuzq = "flumsotfijftdzjdfxwlq";
	if (string("pkmhmgobvvzogvtmjpxnvtjafgbbilgrvhznoxjxjiarmnfyws") == string("pkmhmgobvvzogvtmjpxnvtjafgbbilgrvhznoxjxjiarmnfyws")) {
		int undiwapy;
		for (undiwapy = 15; undiwapy > 0; undiwapy--) {
			continue;
		}
	}
	if (5417 == 5417) {
		int ctgyzsdsbr;
		for (ctgyzsdsbr = 21; ctgyzsdsbr > 0; ctgyzsdsbr--) {
			continue;
		}
	}
	if (865 == 865) {
		int apwov;
		for (apwov = 84; apwov > 0; apwov--) {
			continue;
		}
	}
	if (5417 == 5417) {
		int vyesag;
		for (vyesag = 16; vyesag > 0; vyesag--) {
			continue;
		}
	}
	if (string("ayzexebohtdtfokfuomyjbitauwprcrhogsauydbiglowojitxlifjnbonynbphjjdjbrwrsdvpeqixa") == string("ayzexebohtdtfokfuomyjbitauwprcrhogsauydbiglowojitxlifjnbonynbphjjdjbrwrsdvpeqixa")) {
		int ishgrkaj;
		for (ishgrkaj = 60; ishgrkaj > 0; ishgrkaj--) {
			continue;
		}
	}
	return true;
}

int nkdzalm::usfvtndazkhmz(double xkycaqbkorbab, double jcmzvsmnuu, bool cbvxzatdtgw, bool yybqype) {
	int evohxp = 6193;
	int mmtszobawq = 1297;
	if (1297 == 1297) {
		int hczqtfodt;
		for (hczqtfodt = 59; hczqtfodt > 0; hczqtfodt--) {
			continue;
		}
	}
	if (6193 != 6193) {
		int mkdlrckb;
		for (mkdlrckb = 47; mkdlrckb > 0; mkdlrckb--) {
			continue;
		}
	}
	if (6193 != 6193) {
		int nco;
		for (nco = 26; nco > 0; nco--) {
			continue;
		}
	}
	return 37542;
}

double nkdzalm::zsihgdrbsfqxo() {
	double nrbpvomngpf = 57031;
	string fwghvsax = "cdldbknmqbthbplfpdxmaqtrdrjincfflykshktxvuvnoxuxrimjfqiszwaeqnhkkbxacltqvywmde";
	bool dwokevu = false;
	string tqngypmpvnwvvpu = "twqmgcyyxvxfebvjrisofxwkhidnhoeskmpfsjpkejtetghyswbecyxmovbkmsnonmkcpiuvobvrqkmngdwfsh";
	string qnxrh = "zzgrtktkdhsnuchfhhspkbaadavtyamgjdrlrsijhprdnxdfdtdpfseozxlkpmemxqaqqwts";
	if (string("zzgrtktkdhsnuchfhhspkbaadavtyamgjdrlrsijhprdnxdfdtdpfseozxlkpmemxqaqqwts") == string("zzgrtktkdhsnuchfhhspkbaadavtyamgjdrlrsijhprdnxdfdtdpfseozxlkpmemxqaqqwts")) {
		int xiihgyix;
		for (xiihgyix = 80; xiihgyix > 0; xiihgyix--) {
			continue;
		}
	}
	if (string("zzgrtktkdhsnuchfhhspkbaadavtyamgjdrlrsijhprdnxdfdtdpfseozxlkpmemxqaqqwts") != string("zzgrtktkdhsnuchfhhspkbaadavtyamgjdrlrsijhprdnxdfdtdpfseozxlkpmemxqaqqwts")) {
		int tbo;
		for (tbo = 8; tbo > 0; tbo--) {
			continue;
		}
	}
	if (false == false) {
		int fixngk;
		for (fixngk = 58; fixngk > 0; fixngk--) {
			continue;
		}
	}
	return 98185;
}

string nkdzalm::wlwaswhofnpi(int aujwgpajghovup, bool cwpiqxx, int rlxvuvpmakcct) {
	return string("vkobmei");
}

void nkdzalm::ovivvfqaxmzzzatklx(int ctlmirvivxaylg, int vhuus, string qummngk, string hrwieqnemh, double hxshp, int eykjvglmcioao, int hsqkavbawx) {
	bool tgpsgbdryg = true;
	int gzixf = 1191;
	string mszzw = "gusygejcjnfxbiboskkqortmynzexr";
	string vhxlvqesunihhq = "fmdmeaycfxqvrfgcedavfablefkyzjtchqphfdvmuhtdgynavynrxvxafviulghfldsdhedvpplafxfsliicxrmbnyvfy";
	int ogthhyiitxh = 7401;
	double ikhllskdxrzrnk = 31264;
	if (31264 != 31264) {
		int ea;
		for (ea = 87; ea > 0; ea--) {
			continue;
		}
	}
	if (7401 != 7401) {
		int fpon;
		for (fpon = 83; fpon > 0; fpon--) {
			continue;
		}
	}

}

int nkdzalm::yaqmygajlfd(int mwhuqdphrwxdhtz, int owxhahjzb, string efwpoafa, string krgnxocii) {
	int uwkihkq = 277;
	string fgywepyxla = "zpocrpfkklrcrrnypkdofynlgjvzmucpmjetvkvsphdczlgxamrpakbveszkgutvtvebjrkbwyt";
	bool zkhpxykgdbma = false;
	int dwjavzplbunc = 4364;
	string goiooqvqs = "psvqnvxsvrixbycunpxqqeznzoianavjzhlnudnbrggtyuqtzhaozdrrklegsjytpujajyy";
	int ddtnytf = 3210;
	double qiqkzdwne = 4611;
	string qdaoizgzuefhy = "sbrzxxlfwsyznsrsqxklgcrkju";
	string mpsarc = "yewsvwbcrvlvwfbkwaqindnvjkawchrrlmgypdwegozdsfmykdpshgrjdkrmndghujmemsdhknulqsyj";
	string bnrfogompipfamg = "trryldkj";
	if (string("psvqnvxsvrixbycunpxqqeznzoianavjzhlnudnbrggtyuqtzhaozdrrklegsjytpujajyy") != string("psvqnvxsvrixbycunpxqqeznzoianavjzhlnudnbrggtyuqtzhaozdrrklegsjytpujajyy")) {
		int ybkagt;
		for (ybkagt = 30; ybkagt > 0; ybkagt--) {
			continue;
		}
	}
	if (string("yewsvwbcrvlvwfbkwaqindnvjkawchrrlmgypdwegozdsfmykdpshgrjdkrmndghujmemsdhknulqsyj") == string("yewsvwbcrvlvwfbkwaqindnvjkawchrrlmgypdwegozdsfmykdpshgrjdkrmndghujmemsdhknulqsyj")) {
		int amkc;
		for (amkc = 93; amkc > 0; amkc--) {
			continue;
		}
	}
	return 54754;
}

string nkdzalm::fgmceualrhqhzjbhuxmuf(double tzenfwhpzc, int oqsubsy) {
	double hdmqpjnjxoejlu = 17641;
	bool yfrgyur = false;
	bool owaiviqgsmbj = false;
	if (17641 == 17641) {
		int pciiwv;
		for (pciiwv = 97; pciiwv > 0; pciiwv--) {
			continue;
		}
	}
	if (17641 != 17641) {
		int isoob;
		for (isoob = 19; isoob > 0; isoob--) {
			continue;
		}
	}
	return string("lovpcaqfuqgpg");
}

double nkdzalm::lbdanzlpoborj(double unzohshd, string jfamxsihhloim, bool tmunafxsz, bool vagcvm, double hfvjkyvce, bool oogvydkjzmpepr, string nyzpgxn, int drvhjeyth, string xoivakepwrwud) {
	bool zmgnpxgriqcc = true;
	double cgojcffhpqpfr = 1847;
	double blkagh = 31939;
	double pdvpuizfk = 28896;
	string rpodkiakgelpen = "lrkfwhrkthwemqpvhgrkkgqgoyqcuuzdzrcdsgjzzicugrkknbcwjyiajlinzpdwhjoyhermetev";
	string yuegxhkedyndpfi = "fqnd";
	string dwdvitqunzumpsh = "xgsjnsimhlziotmcqvojwywbltcukwdbjbsidoimbzcdnekh";
	string jnwkewxj = "ljlnlensmodlksgwdaeuodiyjcvvlmtqissbxy";
	if (string("fqnd") != string("fqnd")) {
		int sjobti;
		for (sjobti = 58; sjobti > 0; sjobti--) {
			continue;
		}
	}
	if (string("ljlnlensmodlksgwdaeuodiyjcvvlmtqissbxy") != string("ljlnlensmodlksgwdaeuodiyjcvvlmtqissbxy")) {
		int dchejttyq;
		for (dchejttyq = 4; dchejttyq > 0; dchejttyq--) {
			continue;
		}
	}
	return 94986;
}

void nkdzalm::ugpnzpzutoqwmnsjdt(int hwinm) {
	int cuiyin = 4073;
	int mdoodyqzgb = 1410;
	string qiwnhxxaqoslj = "tglzlbgxyqfxyyguwjyqxulunectlgiogasijzppfajsiqjwqwiyxvvbxudpjt";
	if (4073 == 4073) {
		int ioqad;
		for (ioqad = 77; ioqad > 0; ioqad--) {
			continue;
		}
	}
	if (4073 != 4073) {
		int ydxv;
		for (ydxv = 4; ydxv > 0; ydxv--) {
			continue;
		}
	}

}

nkdzalm::nkdzalm() {
	this->svrwxyhdlljruwcnd(string("ngdkulaugyvsetxnzpfrxtcnitmcvbervef"), true, true, 2432, true, false, 4308, true);
	this->usfvtndazkhmz(49304, 47834, false, true);
	this->zsihgdrbsfqxo();
	this->wlwaswhofnpi(2479, false, 8729);
	this->ovivvfqaxmzzzatklx(6846, 82, string("gtcagftklslljpaajqwadusqahcsarjdiqaagwpagmneubqnftqcjbcdfvnfrqaibeidjttvygdzfjkqsjhzmunqv"), string("apahrfzaqaczbhedmpqnpbvjzvpeheznpumopndabbwzujwzooshbjudzfcgboqtrwhnwads"), 46551, 1732, 7677);
	this->yaqmygajlfd(2620, 2975, string("aajqsarlnlrbecfktuxvwvxbdfpwlekeauswme"), string("jmikanajteruyst"));
	this->fgmceualrhqhzjbhuxmuf(7290, 6765);
	this->lbdanzlpoborj(14777, string("hqchqaurvgltmoedaxqfcneywsotmqzyxplqwayujjoiberqpoetlkinazxfcnhdawbecielilcczkqgqwlafxqcelnbvwcbai"), true, true, 29503, true, string("nte"), 297, string("oniyprgiikahkcnlrqkxpeuwpeiootffqwekkdzf"));
	this->ugpnzpzutoqwmnsjdt(3633);
	this->dzhlmejyexshnkgmn();
	this->hbmsriyntcdejjp(3315);
	this->veslliujjpallmgtllzd(3048, string("uzfhxxyxaktjwgloiwnejvlaotxqahoppsgvnkddloklqgfxtjmbkzufwzpotdqomtjrcw"), true, string("wqxdsblmzthdhtttkgujiyycohskckhh"), false);
	this->fhxlpbecdfbjnzqfnxe(true, string("itmiqptzxzvluzlosgxylboyuxuyepvrpububqwanuqftmwbjbdqgugoabaigvmtdnhj"), 19258, true, 3074, string("tuuvzqtfsolbwqtoikobstbrwwjhikdbhdecuqbdmecslfoevzrsvaavcjlmlvxemnfimmpkjhbmcjztv"));
	this->knsgpdwqvuik(31056, false, false, string("oiojxtlniitleirdkrqztozpninqojbumhzncdltfpyhzpdnoxuaixsjfqywzoqitflylhskcnabghvhrxmqz"), 32123, false, string("m"), string("a"), string("egnvhtglcvkvhlhddjwdcqtzgnjgegwkrbhpnqsohhdmxhgxdcnvjbwpozaaqaryjhcjkuxzujjdl"), true);
	this->dxyepfpooqvawymequocbqa(8975, 4320, 2423);
	this->xgmniajakncuhshwif(363, 4257, string("ptqeyuwdjepvmeiytoxxsmjlcdtfzyzrtsuaesryutybapgmrcpobmcoyptbrxgsmnhcbstsjowmimwlqsaozgp"), string("joysgzrtctxxfornbzzifzazhipqnmowaylyotdyokyzdwdrvixliexxkeysuioqcquhzjxsqttdkeruufxt"), 4187);
}



















































































































































































































































































