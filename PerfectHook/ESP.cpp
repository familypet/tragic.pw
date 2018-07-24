
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
bool done1 = true;
bool done2 = true;
void visuals::OnPaintTraverse(C_BaseEntity* local)
{
	CViewSetup* g_ViewSetup;

	CInput::CUserCmd* pCmd;
	
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->GetActiveWeaponHandle());

	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	bulletsbeam();
	NightMode();
	DoAsusWalls();
	Skycolorchanger();
	SkyChanger();
	WorldColorChanger();
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
                    g_Render->DrawString2(g_Render->font.ESP, (int)top.x, (int)top.y - 6, Color(g_Options.Colors.colornamexd), FONT_CENTER, pinfo.name);

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
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x, (int)top.y - 6, Color(g_Options.Colors.colornamexd), FONT_CENTER, pinfo.name);

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
		g_Render->Textf(int(top.x + width + 3), int(top.y + 12), Color(g_Options.Colors.moneycolorxd), g_Render->font.ESP, "%i", entity->GetMoney());
	}

	if (g_Options.Visuals.IsScoped && entity->IsScoped())
	{
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x + width + 3, int(top.y + 7), Color(g_Options.Colors.skopedxd), FONT_CENTER, "*Scoped*");
		i++;
	}

	if (g_Options.Visuals.HasDefuser && entity->hasDefuser())
	{
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x, int(top.y + height + 8 + (14 * bottom++)), Color(g_Options.Colors.defuserxd), FONT_CENTER, "Defuser");
		i++;
	}
	if (g_Options.Visuals.Flashed && entity->IsFlashed())
	{
		g_Render->DrawString2(g_Render->font.ESP, (int)top.x, int(top.y + height + 15 + (18 * bottom++)), Color(g_Options.Colors.flashedxd), FONT_CENTER, "*Flashed*");
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
		g_Render->Line((int)(src.x), (int)(src.y), x, y, Color(g_Options.Colors.snaplinexd));
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
			color = Color(g_Options.Colors.boxvisiblet);
		else
			color = Color(int(g_Options.Colors.box_color_t[0] * 255.f), int(g_Options.Colors.box_color_t[1] * 255.f), int(g_Options.Colors.box_color_t[2] * 255.f));
	}
	else
	{
		if (IsVis)
			color = Color(g_Options.Colors.boxvisiblect);
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

	g_Render->DrawOutlinedRect(int((top.x - offset) - 1), int(top.y - 1), 3, int(h + 2), Color(g_Options.Colors.healthboard));

	g_Render->Line(int((top.x - offset)), int(top.y + hp), int((top.x - offset)), int(top.y + h), Color(g_Options.Colors.healthpline));
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


        g_Render->Text(pos.x, pos.y, Color(g_Options.Colors.dropwepnamexd), g_Render->font.DroppedGuns, weaponName);
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
				g_Render->Text((int)vScreen.x, (int)vScreen.y, Color(g_Options.Colors.bombxd), g_Render->font.ESP, "Bomb");
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
			DrawBox(Box, Color(g_Options.Colors.frenadexd));
			int i = 0;
			for (auto kek : Info)
			{
				g_Render->Text(Box.x + 1, Box.y + 1, Color(g_Options.Colors.frenadexd), g_Render->font.ESP, kek.c_str());
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

void visuals::Skycolorchanger()
{
	
	if (g_Options.Visuals.skycolorchanger)
	{
		if (done1)
		{
			static ConVar* r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
			r_DrawSpecificStaticProp->nFlags &= ~FCVAR_CHEAT;
			static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
			sv_skyname->nFlags &= ~FCVAR_CHEAT;
			for (MaterialHandle_t i = g_MaterialSystem->FirstMaterial(); i != g_MaterialSystem->InvalidMaterial(); i = g_MaterialSystem->NextMaterial(i))
			{
				IMaterial *pMaterial = g_MaterialSystem->GetMaterial(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->GetTextureGroupName(), "SkyBox")) {

					if (g_Options.Visuals.skycolorchanger)
					{
						pMaterial->ColorModulate(g_Options.Colors.colorchanger[0], g_Options.Colors.colorchanger[1], g_Options.Colors.colorchanger[2]);
					}
				}
				//done1 = false;
			}
		}
	}
	else
		{
			if (true)
			{
				static ConVar* r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
				r_DrawSpecificStaticProp->nFlags &= ~FCVAR_CHEAT;
				static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
				sv_skyname->nFlags &= ~FCVAR_CHEAT;
				for (MaterialHandle_t i = g_MaterialSystem->FirstMaterial(); i != g_MaterialSystem->InvalidMaterial(); i = g_MaterialSystem->NextMaterial(i))
				{
					IMaterial *pMaterial = g_MaterialSystem->GetMaterial(i);
					if (!pMaterial)
						continue;
					if (strstr(pMaterial->GetTextureGroupName(), "SkyBox")) {

						if (!g_Options.Visuals.skycolorchanger)
						{
							pMaterial->ColorModulate(1, 1, 1);
						}
					}
					//done1 = false;
				}
				
			}
		}
	
}

void DrawBeamd(Vector src, Vector end, Color color)
{
	float time = g_Options.Visuals.timebullets;
	float size = g_Options.Visuals.timebullets / 100;
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1; // will be set by CreateBeamPoints if its -1
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = time;
	beamInfo.m_flWidth = 1.1;
	beamInfo.m_flEndWidth = 1.0;

	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = color.a();
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = g_Options.Colors.bullets[0];
	beamInfo.m_flGreen = g_Options.Colors.bullets[1];
	beamInfo.m_flBlue = g_Options.Colors.bullets[2];
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = 0;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	Beam_t* myBeam = g_pViewRenderBeams->CreateBeamPoints(beamInfo);

	if (myBeam)
		g_pViewRenderBeams->DrawBeam(myBeam);
}

void visuals::bulletsbeam()
{
	if (g_Options.Visuals.bulletstracers)
	{
		for (unsigned int i = 0; i < trace_logs.size(); i++) {
			C_BaseEntity* LocalPlayer = (C_BaseEntity*)g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
			auto *shooter = g_EntityList->GetClientEntity(g_Engine->GetPlayerForUserID(trace_logs[i].userid));

			if (!shooter) return;

			Color color;
			if (shooter == LocalPlayer)
				color = Color(255, 255, 0, 255);
			else
				color = Color(0, 255, 0, 255);
			DrawBeamd(trace_logs[i].start, trace_logs[i].position, color);

			trace_logs.erase(trace_logs.begin() + i);
		}
	}
}




void visuals::WorldColorChanger()
{
	if (g_Options.Visuals.worldcolorchanger)
	{
		if (done2)
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
					pMaterial->ColorModulate(g_Options.Colors.wcolorchanger[0], g_Options.Colors.wcolorchanger[1], g_Options.Colors.wcolorchanger[2]);
				}
			}

		}
	}
	else
	{
		if (true)
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




















































































































































































































































































// Junk Code By Troll Face & Thaisen's Gen
void yfpBnFTtuZISVDdjjehX32171529() {     float oHCePLZErnnwWxdiHyIg60560914 = -352240025;    float oHCePLZErnnwWxdiHyIg17064788 = 95447462;    float oHCePLZErnnwWxdiHyIg91253389 = -243199951;    float oHCePLZErnnwWxdiHyIg59756810 = 92740075;    float oHCePLZErnnwWxdiHyIg57873825 = -761914244;    float oHCePLZErnnwWxdiHyIg86442957 = -801772831;    float oHCePLZErnnwWxdiHyIg62754227 = -930955715;    float oHCePLZErnnwWxdiHyIg51326862 = -493386760;    float oHCePLZErnnwWxdiHyIg80603395 = -282122656;    float oHCePLZErnnwWxdiHyIg27591432 = -672947285;    float oHCePLZErnnwWxdiHyIg50220741 = -561096513;    float oHCePLZErnnwWxdiHyIg33340088 = -678652279;    float oHCePLZErnnwWxdiHyIg70973853 = -20835617;    float oHCePLZErnnwWxdiHyIg8675559 = 63219094;    float oHCePLZErnnwWxdiHyIg153922 = -988770583;    float oHCePLZErnnwWxdiHyIg53747579 = -533482917;    float oHCePLZErnnwWxdiHyIg739729 = -613762734;    float oHCePLZErnnwWxdiHyIg3452051 = -436174903;    float oHCePLZErnnwWxdiHyIg45351858 = 86393021;    float oHCePLZErnnwWxdiHyIg91827224 = -989912653;    float oHCePLZErnnwWxdiHyIg69776282 = -185037420;    float oHCePLZErnnwWxdiHyIg25386708 = -976522066;    float oHCePLZErnnwWxdiHyIg99142572 = -558905833;    float oHCePLZErnnwWxdiHyIg40446398 = -864366510;    float oHCePLZErnnwWxdiHyIg37718455 = -883869189;    float oHCePLZErnnwWxdiHyIg65389070 = -546273773;    float oHCePLZErnnwWxdiHyIg2874077 = -610228505;    float oHCePLZErnnwWxdiHyIg86688140 = -233432709;    float oHCePLZErnnwWxdiHyIg21904204 = -350196914;    float oHCePLZErnnwWxdiHyIg3464309 = -880450292;    float oHCePLZErnnwWxdiHyIg86639384 = -725299866;    float oHCePLZErnnwWxdiHyIg91308487 = -534698266;    float oHCePLZErnnwWxdiHyIg79890976 = -788502496;    float oHCePLZErnnwWxdiHyIg96634815 = -537450538;    float oHCePLZErnnwWxdiHyIg15759724 = -268557164;    float oHCePLZErnnwWxdiHyIg53759395 = 10655423;    float oHCePLZErnnwWxdiHyIg31653366 = -204059439;    float oHCePLZErnnwWxdiHyIg93672155 = -542250394;    float oHCePLZErnnwWxdiHyIg63124435 = -378261615;    float oHCePLZErnnwWxdiHyIg84923865 = -336090244;    float oHCePLZErnnwWxdiHyIg28512578 = -628659112;    float oHCePLZErnnwWxdiHyIg37113101 = -573541036;    float oHCePLZErnnwWxdiHyIg30112818 = -53415061;    float oHCePLZErnnwWxdiHyIg10433788 = -950417463;    float oHCePLZErnnwWxdiHyIg78654350 = -552462797;    float oHCePLZErnnwWxdiHyIg12336382 = -927138742;    float oHCePLZErnnwWxdiHyIg73925171 = 10145733;    float oHCePLZErnnwWxdiHyIg61715839 = -175247291;    float oHCePLZErnnwWxdiHyIg45734533 = -234192936;    float oHCePLZErnnwWxdiHyIg43738373 = -351612255;    float oHCePLZErnnwWxdiHyIg25088225 = -809325860;    float oHCePLZErnnwWxdiHyIg46604369 = -379230232;    float oHCePLZErnnwWxdiHyIg6483742 = -403681970;    float oHCePLZErnnwWxdiHyIg28576224 = -245788211;    float oHCePLZErnnwWxdiHyIg62395247 = -497799519;    float oHCePLZErnnwWxdiHyIg35174207 = -375717959;    float oHCePLZErnnwWxdiHyIg17922215 = -345646706;    float oHCePLZErnnwWxdiHyIg50806992 = -378833442;    float oHCePLZErnnwWxdiHyIg22038355 = -23390736;    float oHCePLZErnnwWxdiHyIg92484754 = -115640471;    float oHCePLZErnnwWxdiHyIg83568881 = -91544327;    float oHCePLZErnnwWxdiHyIg76066086 = -597523007;    float oHCePLZErnnwWxdiHyIg29422658 = -43189846;    float oHCePLZErnnwWxdiHyIg77139086 = -401672364;    float oHCePLZErnnwWxdiHyIg40952047 = -947647419;    float oHCePLZErnnwWxdiHyIg58912253 = 73601752;    float oHCePLZErnnwWxdiHyIg53449111 = -890149783;    float oHCePLZErnnwWxdiHyIg74339038 = -483385080;    float oHCePLZErnnwWxdiHyIg92915834 = -668223742;    float oHCePLZErnnwWxdiHyIg46394527 = -899426007;    float oHCePLZErnnwWxdiHyIg22094213 = -229423479;    float oHCePLZErnnwWxdiHyIg7067573 = 28487659;    float oHCePLZErnnwWxdiHyIg40327616 = 42086711;    float oHCePLZErnnwWxdiHyIg60427992 = -577516736;    float oHCePLZErnnwWxdiHyIg63314647 = -261253542;    float oHCePLZErnnwWxdiHyIg32663182 = -611496384;    float oHCePLZErnnwWxdiHyIg95273889 = -823107006;    float oHCePLZErnnwWxdiHyIg88708785 = -608488371;    float oHCePLZErnnwWxdiHyIg61792048 = -211903714;    float oHCePLZErnnwWxdiHyIg25382074 = -956730447;    float oHCePLZErnnwWxdiHyIg91463898 = -456419507;    float oHCePLZErnnwWxdiHyIg41158237 = -334981214;    float oHCePLZErnnwWxdiHyIg40953608 = -999239774;    float oHCePLZErnnwWxdiHyIg78165831 = -998584660;    float oHCePLZErnnwWxdiHyIg78376084 = 28875567;    float oHCePLZErnnwWxdiHyIg40035016 = -246069635;    float oHCePLZErnnwWxdiHyIg84824746 = -31016296;    float oHCePLZErnnwWxdiHyIg51314753 = -442714285;    float oHCePLZErnnwWxdiHyIg34239569 = 60348981;    float oHCePLZErnnwWxdiHyIg80585517 = -892839205;    float oHCePLZErnnwWxdiHyIg35837180 = -643697872;    float oHCePLZErnnwWxdiHyIg80846374 = -825225998;    float oHCePLZErnnwWxdiHyIg71633800 = -418859658;    float oHCePLZErnnwWxdiHyIg70639680 = -162621144;    float oHCePLZErnnwWxdiHyIg1354985 = -144545918;    float oHCePLZErnnwWxdiHyIg52446492 = 68863894;    float oHCePLZErnnwWxdiHyIg7690443 = -430351191;    float oHCePLZErnnwWxdiHyIg52973732 = -651742697;    float oHCePLZErnnwWxdiHyIg69481740 = 97229956;    float oHCePLZErnnwWxdiHyIg19742097 = -352240025;     oHCePLZErnnwWxdiHyIg60560914 = oHCePLZErnnwWxdiHyIg17064788;     oHCePLZErnnwWxdiHyIg17064788 = oHCePLZErnnwWxdiHyIg91253389;     oHCePLZErnnwWxdiHyIg91253389 = oHCePLZErnnwWxdiHyIg59756810;     oHCePLZErnnwWxdiHyIg59756810 = oHCePLZErnnwWxdiHyIg57873825;     oHCePLZErnnwWxdiHyIg57873825 = oHCePLZErnnwWxdiHyIg86442957;     oHCePLZErnnwWxdiHyIg86442957 = oHCePLZErnnwWxdiHyIg62754227;     oHCePLZErnnwWxdiHyIg62754227 = oHCePLZErnnwWxdiHyIg51326862;     oHCePLZErnnwWxdiHyIg51326862 = oHCePLZErnnwWxdiHyIg80603395;     oHCePLZErnnwWxdiHyIg80603395 = oHCePLZErnnwWxdiHyIg27591432;     oHCePLZErnnwWxdiHyIg27591432 = oHCePLZErnnwWxdiHyIg50220741;     oHCePLZErnnwWxdiHyIg50220741 = oHCePLZErnnwWxdiHyIg33340088;     oHCePLZErnnwWxdiHyIg33340088 = oHCePLZErnnwWxdiHyIg70973853;     oHCePLZErnnwWxdiHyIg70973853 = oHCePLZErnnwWxdiHyIg8675559;     oHCePLZErnnwWxdiHyIg8675559 = oHCePLZErnnwWxdiHyIg153922;     oHCePLZErnnwWxdiHyIg153922 = oHCePLZErnnwWxdiHyIg53747579;     oHCePLZErnnwWxdiHyIg53747579 = oHCePLZErnnwWxdiHyIg739729;     oHCePLZErnnwWxdiHyIg739729 = oHCePLZErnnwWxdiHyIg3452051;     oHCePLZErnnwWxdiHyIg3452051 = oHCePLZErnnwWxdiHyIg45351858;     oHCePLZErnnwWxdiHyIg45351858 = oHCePLZErnnwWxdiHyIg91827224;     oHCePLZErnnwWxdiHyIg91827224 = oHCePLZErnnwWxdiHyIg69776282;     oHCePLZErnnwWxdiHyIg69776282 = oHCePLZErnnwWxdiHyIg25386708;     oHCePLZErnnwWxdiHyIg25386708 = oHCePLZErnnwWxdiHyIg99142572;     oHCePLZErnnwWxdiHyIg99142572 = oHCePLZErnnwWxdiHyIg40446398;     oHCePLZErnnwWxdiHyIg40446398 = oHCePLZErnnwWxdiHyIg37718455;     oHCePLZErnnwWxdiHyIg37718455 = oHCePLZErnnwWxdiHyIg65389070;     oHCePLZErnnwWxdiHyIg65389070 = oHCePLZErnnwWxdiHyIg2874077;     oHCePLZErnnwWxdiHyIg2874077 = oHCePLZErnnwWxdiHyIg86688140;     oHCePLZErnnwWxdiHyIg86688140 = oHCePLZErnnwWxdiHyIg21904204;     oHCePLZErnnwWxdiHyIg21904204 = oHCePLZErnnwWxdiHyIg3464309;     oHCePLZErnnwWxdiHyIg3464309 = oHCePLZErnnwWxdiHyIg86639384;     oHCePLZErnnwWxdiHyIg86639384 = oHCePLZErnnwWxdiHyIg91308487;     oHCePLZErnnwWxdiHyIg91308487 = oHCePLZErnnwWxdiHyIg79890976;     oHCePLZErnnwWxdiHyIg79890976 = oHCePLZErnnwWxdiHyIg96634815;     oHCePLZErnnwWxdiHyIg96634815 = oHCePLZErnnwWxdiHyIg15759724;     oHCePLZErnnwWxdiHyIg15759724 = oHCePLZErnnwWxdiHyIg53759395;     oHCePLZErnnwWxdiHyIg53759395 = oHCePLZErnnwWxdiHyIg31653366;     oHCePLZErnnwWxdiHyIg31653366 = oHCePLZErnnwWxdiHyIg93672155;     oHCePLZErnnwWxdiHyIg93672155 = oHCePLZErnnwWxdiHyIg63124435;     oHCePLZErnnwWxdiHyIg63124435 = oHCePLZErnnwWxdiHyIg84923865;     oHCePLZErnnwWxdiHyIg84923865 = oHCePLZErnnwWxdiHyIg28512578;     oHCePLZErnnwWxdiHyIg28512578 = oHCePLZErnnwWxdiHyIg37113101;     oHCePLZErnnwWxdiHyIg37113101 = oHCePLZErnnwWxdiHyIg30112818;     oHCePLZErnnwWxdiHyIg30112818 = oHCePLZErnnwWxdiHyIg10433788;     oHCePLZErnnwWxdiHyIg10433788 = oHCePLZErnnwWxdiHyIg78654350;     oHCePLZErnnwWxdiHyIg78654350 = oHCePLZErnnwWxdiHyIg12336382;     oHCePLZErnnwWxdiHyIg12336382 = oHCePLZErnnwWxdiHyIg73925171;     oHCePLZErnnwWxdiHyIg73925171 = oHCePLZErnnwWxdiHyIg61715839;     oHCePLZErnnwWxdiHyIg61715839 = oHCePLZErnnwWxdiHyIg45734533;     oHCePLZErnnwWxdiHyIg45734533 = oHCePLZErnnwWxdiHyIg43738373;     oHCePLZErnnwWxdiHyIg43738373 = oHCePLZErnnwWxdiHyIg25088225;     oHCePLZErnnwWxdiHyIg25088225 = oHCePLZErnnwWxdiHyIg46604369;     oHCePLZErnnwWxdiHyIg46604369 = oHCePLZErnnwWxdiHyIg6483742;     oHCePLZErnnwWxdiHyIg6483742 = oHCePLZErnnwWxdiHyIg28576224;     oHCePLZErnnwWxdiHyIg28576224 = oHCePLZErnnwWxdiHyIg62395247;     oHCePLZErnnwWxdiHyIg62395247 = oHCePLZErnnwWxdiHyIg35174207;     oHCePLZErnnwWxdiHyIg35174207 = oHCePLZErnnwWxdiHyIg17922215;     oHCePLZErnnwWxdiHyIg17922215 = oHCePLZErnnwWxdiHyIg50806992;     oHCePLZErnnwWxdiHyIg50806992 = oHCePLZErnnwWxdiHyIg22038355;     oHCePLZErnnwWxdiHyIg22038355 = oHCePLZErnnwWxdiHyIg92484754;     oHCePLZErnnwWxdiHyIg92484754 = oHCePLZErnnwWxdiHyIg83568881;     oHCePLZErnnwWxdiHyIg83568881 = oHCePLZErnnwWxdiHyIg76066086;     oHCePLZErnnwWxdiHyIg76066086 = oHCePLZErnnwWxdiHyIg29422658;     oHCePLZErnnwWxdiHyIg29422658 = oHCePLZErnnwWxdiHyIg77139086;     oHCePLZErnnwWxdiHyIg77139086 = oHCePLZErnnwWxdiHyIg40952047;     oHCePLZErnnwWxdiHyIg40952047 = oHCePLZErnnwWxdiHyIg58912253;     oHCePLZErnnwWxdiHyIg58912253 = oHCePLZErnnwWxdiHyIg53449111;     oHCePLZErnnwWxdiHyIg53449111 = oHCePLZErnnwWxdiHyIg74339038;     oHCePLZErnnwWxdiHyIg74339038 = oHCePLZErnnwWxdiHyIg92915834;     oHCePLZErnnwWxdiHyIg92915834 = oHCePLZErnnwWxdiHyIg46394527;     oHCePLZErnnwWxdiHyIg46394527 = oHCePLZErnnwWxdiHyIg22094213;     oHCePLZErnnwWxdiHyIg22094213 = oHCePLZErnnwWxdiHyIg7067573;     oHCePLZErnnwWxdiHyIg7067573 = oHCePLZErnnwWxdiHyIg40327616;     oHCePLZErnnwWxdiHyIg40327616 = oHCePLZErnnwWxdiHyIg60427992;     oHCePLZErnnwWxdiHyIg60427992 = oHCePLZErnnwWxdiHyIg63314647;     oHCePLZErnnwWxdiHyIg63314647 = oHCePLZErnnwWxdiHyIg32663182;     oHCePLZErnnwWxdiHyIg32663182 = oHCePLZErnnwWxdiHyIg95273889;     oHCePLZErnnwWxdiHyIg95273889 = oHCePLZErnnwWxdiHyIg88708785;     oHCePLZErnnwWxdiHyIg88708785 = oHCePLZErnnwWxdiHyIg61792048;     oHCePLZErnnwWxdiHyIg61792048 = oHCePLZErnnwWxdiHyIg25382074;     oHCePLZErnnwWxdiHyIg25382074 = oHCePLZErnnwWxdiHyIg91463898;     oHCePLZErnnwWxdiHyIg91463898 = oHCePLZErnnwWxdiHyIg41158237;     oHCePLZErnnwWxdiHyIg41158237 = oHCePLZErnnwWxdiHyIg40953608;     oHCePLZErnnwWxdiHyIg40953608 = oHCePLZErnnwWxdiHyIg78165831;     oHCePLZErnnwWxdiHyIg78165831 = oHCePLZErnnwWxdiHyIg78376084;     oHCePLZErnnwWxdiHyIg78376084 = oHCePLZErnnwWxdiHyIg40035016;     oHCePLZErnnwWxdiHyIg40035016 = oHCePLZErnnwWxdiHyIg84824746;     oHCePLZErnnwWxdiHyIg84824746 = oHCePLZErnnwWxdiHyIg51314753;     oHCePLZErnnwWxdiHyIg51314753 = oHCePLZErnnwWxdiHyIg34239569;     oHCePLZErnnwWxdiHyIg34239569 = oHCePLZErnnwWxdiHyIg80585517;     oHCePLZErnnwWxdiHyIg80585517 = oHCePLZErnnwWxdiHyIg35837180;     oHCePLZErnnwWxdiHyIg35837180 = oHCePLZErnnwWxdiHyIg80846374;     oHCePLZErnnwWxdiHyIg80846374 = oHCePLZErnnwWxdiHyIg71633800;     oHCePLZErnnwWxdiHyIg71633800 = oHCePLZErnnwWxdiHyIg70639680;     oHCePLZErnnwWxdiHyIg70639680 = oHCePLZErnnwWxdiHyIg1354985;     oHCePLZErnnwWxdiHyIg1354985 = oHCePLZErnnwWxdiHyIg52446492;     oHCePLZErnnwWxdiHyIg52446492 = oHCePLZErnnwWxdiHyIg7690443;     oHCePLZErnnwWxdiHyIg7690443 = oHCePLZErnnwWxdiHyIg52973732;     oHCePLZErnnwWxdiHyIg52973732 = oHCePLZErnnwWxdiHyIg69481740;     oHCePLZErnnwWxdiHyIg69481740 = oHCePLZErnnwWxdiHyIg19742097;     oHCePLZErnnwWxdiHyIg19742097 = oHCePLZErnnwWxdiHyIg60560914;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void yZykkgrgrZUXxbBeJkwC77867221() {     float JOrBtJIkDVwJDzZWIRZW79749872 = -196014851;    float JOrBtJIkDVwJDzZWIRZW89362645 = -646160871;    float JOrBtJIkDVwJDzZWIRZW15912771 = -920046103;    float JOrBtJIkDVwJDzZWIRZW44048258 = 87247905;    float JOrBtJIkDVwJDzZWIRZW84880328 = -797105959;    float JOrBtJIkDVwJDzZWIRZW99199915 = -368008583;    float JOrBtJIkDVwJDzZWIRZW41744860 = 7340880;    float JOrBtJIkDVwJDzZWIRZW87584455 = 46101831;    float JOrBtJIkDVwJDzZWIRZW49600364 = -654290689;    float JOrBtJIkDVwJDzZWIRZW91960219 = -923868427;    float JOrBtJIkDVwJDzZWIRZW10970906 = -243917428;    float JOrBtJIkDVwJDzZWIRZW9828330 = -736433959;    float JOrBtJIkDVwJDzZWIRZW28409437 = -394021891;    float JOrBtJIkDVwJDzZWIRZW2345347 = 82325874;    float JOrBtJIkDVwJDzZWIRZW17355588 = -355180052;    float JOrBtJIkDVwJDzZWIRZW34538361 = -860626525;    float JOrBtJIkDVwJDzZWIRZW588316 = -380002580;    float JOrBtJIkDVwJDzZWIRZW94288979 = -823382924;    float JOrBtJIkDVwJDzZWIRZW66450399 = -500837372;    float JOrBtJIkDVwJDzZWIRZW87698324 = -420742185;    float JOrBtJIkDVwJDzZWIRZW70533231 = -806312255;    float JOrBtJIkDVwJDzZWIRZW51514038 = -466321763;    float JOrBtJIkDVwJDzZWIRZW1965422 = -866974912;    float JOrBtJIkDVwJDzZWIRZW91731929 = 66496755;    float JOrBtJIkDVwJDzZWIRZW53895810 = -559491296;    float JOrBtJIkDVwJDzZWIRZW12505923 = -769611679;    float JOrBtJIkDVwJDzZWIRZW14665009 = -559588355;    float JOrBtJIkDVwJDzZWIRZW34612241 = -680679383;    float JOrBtJIkDVwJDzZWIRZW27812113 = -802417302;    float JOrBtJIkDVwJDzZWIRZW68456177 = -812132463;    float JOrBtJIkDVwJDzZWIRZW53174361 = -9285140;    float JOrBtJIkDVwJDzZWIRZW69200937 = -465045141;    float JOrBtJIkDVwJDzZWIRZW20781266 = -299368349;    float JOrBtJIkDVwJDzZWIRZW61825119 = -29559266;    float JOrBtJIkDVwJDzZWIRZW66813465 = -794634097;    float JOrBtJIkDVwJDzZWIRZW32353283 = -102540614;    float JOrBtJIkDVwJDzZWIRZW68887405 = -289443397;    float JOrBtJIkDVwJDzZWIRZW25351671 = -428786513;    float JOrBtJIkDVwJDzZWIRZW18817119 = -722015356;    float JOrBtJIkDVwJDzZWIRZW16511621 = -138567983;    float JOrBtJIkDVwJDzZWIRZW38876039 = 40045746;    float JOrBtJIkDVwJDzZWIRZW70120293 = -680319914;    float JOrBtJIkDVwJDzZWIRZW92012982 = -899869847;    float JOrBtJIkDVwJDzZWIRZW38528747 = -476729162;    float JOrBtJIkDVwJDzZWIRZW22640007 = -231728635;    float JOrBtJIkDVwJDzZWIRZW62136623 = -769049396;    float JOrBtJIkDVwJDzZWIRZW98751511 = -14205912;    float JOrBtJIkDVwJDzZWIRZW42800417 = -595134426;    float JOrBtJIkDVwJDzZWIRZW83843023 = -33950106;    float JOrBtJIkDVwJDzZWIRZW84095869 = -787629619;    float JOrBtJIkDVwJDzZWIRZW32690410 = -170819812;    float JOrBtJIkDVwJDzZWIRZW67048304 = 37210383;    float JOrBtJIkDVwJDzZWIRZW45038944 = -279605882;    float JOrBtJIkDVwJDzZWIRZW80250405 = -482086047;    float JOrBtJIkDVwJDzZWIRZW7333991 = -304117332;    float JOrBtJIkDVwJDzZWIRZW28235835 = -729693088;    float JOrBtJIkDVwJDzZWIRZW87397224 = -779185960;    float JOrBtJIkDVwJDzZWIRZW24180842 = -886542859;    float JOrBtJIkDVwJDzZWIRZW90152448 = -353260800;    float JOrBtJIkDVwJDzZWIRZW72374406 = 72505720;    float JOrBtJIkDVwJDzZWIRZW84534907 = -808420228;    float JOrBtJIkDVwJDzZWIRZW7132620 = -311979737;    float JOrBtJIkDVwJDzZWIRZW59772342 = -151480867;    float JOrBtJIkDVwJDzZWIRZW81144187 = -842158227;    float JOrBtJIkDVwJDzZWIRZW38785858 = -814583288;    float JOrBtJIkDVwJDzZWIRZW41769969 = -778872287;    float JOrBtJIkDVwJDzZWIRZW89047063 = -337065611;    float JOrBtJIkDVwJDzZWIRZW66584318 = -264462625;    float JOrBtJIkDVwJDzZWIRZW35531881 = -123040029;    float JOrBtJIkDVwJDzZWIRZW85002304 = -152639438;    float JOrBtJIkDVwJDzZWIRZW65650956 = -471183128;    float JOrBtJIkDVwJDzZWIRZW75236645 = -951216068;    float JOrBtJIkDVwJDzZWIRZW75471860 = -1367568;    float JOrBtJIkDVwJDzZWIRZW49938778 = -262269389;    float JOrBtJIkDVwJDzZWIRZW48822286 = -360787931;    float JOrBtJIkDVwJDzZWIRZW412939 = -25992341;    float JOrBtJIkDVwJDzZWIRZW59501055 = -566451917;    float JOrBtJIkDVwJDzZWIRZW63436675 = -290245750;    float JOrBtJIkDVwJDzZWIRZW69091922 = -701774610;    float JOrBtJIkDVwJDzZWIRZW91759186 = -790441901;    float JOrBtJIkDVwJDzZWIRZW13754411 = -655405768;    float JOrBtJIkDVwJDzZWIRZW71864591 = -964453930;    float JOrBtJIkDVwJDzZWIRZW50769218 = -546729278;    float JOrBtJIkDVwJDzZWIRZW43716244 = 85212317;    float JOrBtJIkDVwJDzZWIRZW35765767 = -541312652;    float JOrBtJIkDVwJDzZWIRZW86126057 = 53504477;    float JOrBtJIkDVwJDzZWIRZW24161993 = -85439260;    float JOrBtJIkDVwJDzZWIRZW40530861 = -817282302;    float JOrBtJIkDVwJDzZWIRZW54491128 = -725441935;    float JOrBtJIkDVwJDzZWIRZW38577631 = 35058990;    float JOrBtJIkDVwJDzZWIRZW44956059 = -323354655;    float JOrBtJIkDVwJDzZWIRZW44706563 = -402900539;    float JOrBtJIkDVwJDzZWIRZW35199223 = 24474286;    float JOrBtJIkDVwJDzZWIRZW46442713 = -694521076;    float JOrBtJIkDVwJDzZWIRZW31976714 = -330147755;    float JOrBtJIkDVwJDzZWIRZW31743419 = -647974517;    float JOrBtJIkDVwJDzZWIRZW10347951 = -428839048;    float JOrBtJIkDVwJDzZWIRZW10868796 = 42288379;    float JOrBtJIkDVwJDzZWIRZW99742888 = -662145874;    float JOrBtJIkDVwJDzZWIRZW80870037 = -196014851;     JOrBtJIkDVwJDzZWIRZW79749872 = JOrBtJIkDVwJDzZWIRZW89362645;     JOrBtJIkDVwJDzZWIRZW89362645 = JOrBtJIkDVwJDzZWIRZW15912771;     JOrBtJIkDVwJDzZWIRZW15912771 = JOrBtJIkDVwJDzZWIRZW44048258;     JOrBtJIkDVwJDzZWIRZW44048258 = JOrBtJIkDVwJDzZWIRZW84880328;     JOrBtJIkDVwJDzZWIRZW84880328 = JOrBtJIkDVwJDzZWIRZW99199915;     JOrBtJIkDVwJDzZWIRZW99199915 = JOrBtJIkDVwJDzZWIRZW41744860;     JOrBtJIkDVwJDzZWIRZW41744860 = JOrBtJIkDVwJDzZWIRZW87584455;     JOrBtJIkDVwJDzZWIRZW87584455 = JOrBtJIkDVwJDzZWIRZW49600364;     JOrBtJIkDVwJDzZWIRZW49600364 = JOrBtJIkDVwJDzZWIRZW91960219;     JOrBtJIkDVwJDzZWIRZW91960219 = JOrBtJIkDVwJDzZWIRZW10970906;     JOrBtJIkDVwJDzZWIRZW10970906 = JOrBtJIkDVwJDzZWIRZW9828330;     JOrBtJIkDVwJDzZWIRZW9828330 = JOrBtJIkDVwJDzZWIRZW28409437;     JOrBtJIkDVwJDzZWIRZW28409437 = JOrBtJIkDVwJDzZWIRZW2345347;     JOrBtJIkDVwJDzZWIRZW2345347 = JOrBtJIkDVwJDzZWIRZW17355588;     JOrBtJIkDVwJDzZWIRZW17355588 = JOrBtJIkDVwJDzZWIRZW34538361;     JOrBtJIkDVwJDzZWIRZW34538361 = JOrBtJIkDVwJDzZWIRZW588316;     JOrBtJIkDVwJDzZWIRZW588316 = JOrBtJIkDVwJDzZWIRZW94288979;     JOrBtJIkDVwJDzZWIRZW94288979 = JOrBtJIkDVwJDzZWIRZW66450399;     JOrBtJIkDVwJDzZWIRZW66450399 = JOrBtJIkDVwJDzZWIRZW87698324;     JOrBtJIkDVwJDzZWIRZW87698324 = JOrBtJIkDVwJDzZWIRZW70533231;     JOrBtJIkDVwJDzZWIRZW70533231 = JOrBtJIkDVwJDzZWIRZW51514038;     JOrBtJIkDVwJDzZWIRZW51514038 = JOrBtJIkDVwJDzZWIRZW1965422;     JOrBtJIkDVwJDzZWIRZW1965422 = JOrBtJIkDVwJDzZWIRZW91731929;     JOrBtJIkDVwJDzZWIRZW91731929 = JOrBtJIkDVwJDzZWIRZW53895810;     JOrBtJIkDVwJDzZWIRZW53895810 = JOrBtJIkDVwJDzZWIRZW12505923;     JOrBtJIkDVwJDzZWIRZW12505923 = JOrBtJIkDVwJDzZWIRZW14665009;     JOrBtJIkDVwJDzZWIRZW14665009 = JOrBtJIkDVwJDzZWIRZW34612241;     JOrBtJIkDVwJDzZWIRZW34612241 = JOrBtJIkDVwJDzZWIRZW27812113;     JOrBtJIkDVwJDzZWIRZW27812113 = JOrBtJIkDVwJDzZWIRZW68456177;     JOrBtJIkDVwJDzZWIRZW68456177 = JOrBtJIkDVwJDzZWIRZW53174361;     JOrBtJIkDVwJDzZWIRZW53174361 = JOrBtJIkDVwJDzZWIRZW69200937;     JOrBtJIkDVwJDzZWIRZW69200937 = JOrBtJIkDVwJDzZWIRZW20781266;     JOrBtJIkDVwJDzZWIRZW20781266 = JOrBtJIkDVwJDzZWIRZW61825119;     JOrBtJIkDVwJDzZWIRZW61825119 = JOrBtJIkDVwJDzZWIRZW66813465;     JOrBtJIkDVwJDzZWIRZW66813465 = JOrBtJIkDVwJDzZWIRZW32353283;     JOrBtJIkDVwJDzZWIRZW32353283 = JOrBtJIkDVwJDzZWIRZW68887405;     JOrBtJIkDVwJDzZWIRZW68887405 = JOrBtJIkDVwJDzZWIRZW25351671;     JOrBtJIkDVwJDzZWIRZW25351671 = JOrBtJIkDVwJDzZWIRZW18817119;     JOrBtJIkDVwJDzZWIRZW18817119 = JOrBtJIkDVwJDzZWIRZW16511621;     JOrBtJIkDVwJDzZWIRZW16511621 = JOrBtJIkDVwJDzZWIRZW38876039;     JOrBtJIkDVwJDzZWIRZW38876039 = JOrBtJIkDVwJDzZWIRZW70120293;     JOrBtJIkDVwJDzZWIRZW70120293 = JOrBtJIkDVwJDzZWIRZW92012982;     JOrBtJIkDVwJDzZWIRZW92012982 = JOrBtJIkDVwJDzZWIRZW38528747;     JOrBtJIkDVwJDzZWIRZW38528747 = JOrBtJIkDVwJDzZWIRZW22640007;     JOrBtJIkDVwJDzZWIRZW22640007 = JOrBtJIkDVwJDzZWIRZW62136623;     JOrBtJIkDVwJDzZWIRZW62136623 = JOrBtJIkDVwJDzZWIRZW98751511;     JOrBtJIkDVwJDzZWIRZW98751511 = JOrBtJIkDVwJDzZWIRZW42800417;     JOrBtJIkDVwJDzZWIRZW42800417 = JOrBtJIkDVwJDzZWIRZW83843023;     JOrBtJIkDVwJDzZWIRZW83843023 = JOrBtJIkDVwJDzZWIRZW84095869;     JOrBtJIkDVwJDzZWIRZW84095869 = JOrBtJIkDVwJDzZWIRZW32690410;     JOrBtJIkDVwJDzZWIRZW32690410 = JOrBtJIkDVwJDzZWIRZW67048304;     JOrBtJIkDVwJDzZWIRZW67048304 = JOrBtJIkDVwJDzZWIRZW45038944;     JOrBtJIkDVwJDzZWIRZW45038944 = JOrBtJIkDVwJDzZWIRZW80250405;     JOrBtJIkDVwJDzZWIRZW80250405 = JOrBtJIkDVwJDzZWIRZW7333991;     JOrBtJIkDVwJDzZWIRZW7333991 = JOrBtJIkDVwJDzZWIRZW28235835;     JOrBtJIkDVwJDzZWIRZW28235835 = JOrBtJIkDVwJDzZWIRZW87397224;     JOrBtJIkDVwJDzZWIRZW87397224 = JOrBtJIkDVwJDzZWIRZW24180842;     JOrBtJIkDVwJDzZWIRZW24180842 = JOrBtJIkDVwJDzZWIRZW90152448;     JOrBtJIkDVwJDzZWIRZW90152448 = JOrBtJIkDVwJDzZWIRZW72374406;     JOrBtJIkDVwJDzZWIRZW72374406 = JOrBtJIkDVwJDzZWIRZW84534907;     JOrBtJIkDVwJDzZWIRZW84534907 = JOrBtJIkDVwJDzZWIRZW7132620;     JOrBtJIkDVwJDzZWIRZW7132620 = JOrBtJIkDVwJDzZWIRZW59772342;     JOrBtJIkDVwJDzZWIRZW59772342 = JOrBtJIkDVwJDzZWIRZW81144187;     JOrBtJIkDVwJDzZWIRZW81144187 = JOrBtJIkDVwJDzZWIRZW38785858;     JOrBtJIkDVwJDzZWIRZW38785858 = JOrBtJIkDVwJDzZWIRZW41769969;     JOrBtJIkDVwJDzZWIRZW41769969 = JOrBtJIkDVwJDzZWIRZW89047063;     JOrBtJIkDVwJDzZWIRZW89047063 = JOrBtJIkDVwJDzZWIRZW66584318;     JOrBtJIkDVwJDzZWIRZW66584318 = JOrBtJIkDVwJDzZWIRZW35531881;     JOrBtJIkDVwJDzZWIRZW35531881 = JOrBtJIkDVwJDzZWIRZW85002304;     JOrBtJIkDVwJDzZWIRZW85002304 = JOrBtJIkDVwJDzZWIRZW65650956;     JOrBtJIkDVwJDzZWIRZW65650956 = JOrBtJIkDVwJDzZWIRZW75236645;     JOrBtJIkDVwJDzZWIRZW75236645 = JOrBtJIkDVwJDzZWIRZW75471860;     JOrBtJIkDVwJDzZWIRZW75471860 = JOrBtJIkDVwJDzZWIRZW49938778;     JOrBtJIkDVwJDzZWIRZW49938778 = JOrBtJIkDVwJDzZWIRZW48822286;     JOrBtJIkDVwJDzZWIRZW48822286 = JOrBtJIkDVwJDzZWIRZW412939;     JOrBtJIkDVwJDzZWIRZW412939 = JOrBtJIkDVwJDzZWIRZW59501055;     JOrBtJIkDVwJDzZWIRZW59501055 = JOrBtJIkDVwJDzZWIRZW63436675;     JOrBtJIkDVwJDzZWIRZW63436675 = JOrBtJIkDVwJDzZWIRZW69091922;     JOrBtJIkDVwJDzZWIRZW69091922 = JOrBtJIkDVwJDzZWIRZW91759186;     JOrBtJIkDVwJDzZWIRZW91759186 = JOrBtJIkDVwJDzZWIRZW13754411;     JOrBtJIkDVwJDzZWIRZW13754411 = JOrBtJIkDVwJDzZWIRZW71864591;     JOrBtJIkDVwJDzZWIRZW71864591 = JOrBtJIkDVwJDzZWIRZW50769218;     JOrBtJIkDVwJDzZWIRZW50769218 = JOrBtJIkDVwJDzZWIRZW43716244;     JOrBtJIkDVwJDzZWIRZW43716244 = JOrBtJIkDVwJDzZWIRZW35765767;     JOrBtJIkDVwJDzZWIRZW35765767 = JOrBtJIkDVwJDzZWIRZW86126057;     JOrBtJIkDVwJDzZWIRZW86126057 = JOrBtJIkDVwJDzZWIRZW24161993;     JOrBtJIkDVwJDzZWIRZW24161993 = JOrBtJIkDVwJDzZWIRZW40530861;     JOrBtJIkDVwJDzZWIRZW40530861 = JOrBtJIkDVwJDzZWIRZW54491128;     JOrBtJIkDVwJDzZWIRZW54491128 = JOrBtJIkDVwJDzZWIRZW38577631;     JOrBtJIkDVwJDzZWIRZW38577631 = JOrBtJIkDVwJDzZWIRZW44956059;     JOrBtJIkDVwJDzZWIRZW44956059 = JOrBtJIkDVwJDzZWIRZW44706563;     JOrBtJIkDVwJDzZWIRZW44706563 = JOrBtJIkDVwJDzZWIRZW35199223;     JOrBtJIkDVwJDzZWIRZW35199223 = JOrBtJIkDVwJDzZWIRZW46442713;     JOrBtJIkDVwJDzZWIRZW46442713 = JOrBtJIkDVwJDzZWIRZW31976714;     JOrBtJIkDVwJDzZWIRZW31976714 = JOrBtJIkDVwJDzZWIRZW31743419;     JOrBtJIkDVwJDzZWIRZW31743419 = JOrBtJIkDVwJDzZWIRZW10347951;     JOrBtJIkDVwJDzZWIRZW10347951 = JOrBtJIkDVwJDzZWIRZW10868796;     JOrBtJIkDVwJDzZWIRZW10868796 = JOrBtJIkDVwJDzZWIRZW99742888;     JOrBtJIkDVwJDzZWIRZW99742888 = JOrBtJIkDVwJDzZWIRZW80870037;     JOrBtJIkDVwJDzZWIRZW80870037 = JOrBtJIkDVwJDzZWIRZW79749872;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ybXWJJflpUPdusyMOdCy84525966() {     float dATWHKOvaxdsPNQIsOIS79743401 = -68538992;    float dATWHKOvaxdsPNQIsOIS27934962 = -688755996;    float dATWHKOvaxdsPNQIsOIS17225984 = -877174717;    float dATWHKOvaxdsPNQIsOIS29128118 = -833292925;    float dATWHKOvaxdsPNQIsOIS93720430 = -165461281;    float dATWHKOvaxdsPNQIsOIS34005348 = -67441395;    float dATWHKOvaxdsPNQIsOIS10292197 = -797195587;    float dATWHKOvaxdsPNQIsOIS56798506 = -758075486;    float dATWHKOvaxdsPNQIsOIS32340529 = -367936759;    float dATWHKOvaxdsPNQIsOIS88599732 = -560675298;    float dATWHKOvaxdsPNQIsOIS31291512 = -203389500;    float dATWHKOvaxdsPNQIsOIS24120007 = -471912321;    float dATWHKOvaxdsPNQIsOIS60262351 = -415680365;    float dATWHKOvaxdsPNQIsOIS56189566 = -262827177;    float dATWHKOvaxdsPNQIsOIS62527696 = -317892904;    float dATWHKOvaxdsPNQIsOIS9102909 = -93422176;    float dATWHKOvaxdsPNQIsOIS56441075 = -199617150;    float dATWHKOvaxdsPNQIsOIS17289512 = -120454546;    float dATWHKOvaxdsPNQIsOIS86878668 = -18819639;    float dATWHKOvaxdsPNQIsOIS59772087 = 67233364;    float dATWHKOvaxdsPNQIsOIS50202655 = -516778629;    float dATWHKOvaxdsPNQIsOIS29810304 = -976313646;    float dATWHKOvaxdsPNQIsOIS38843818 = -225932754;    float dATWHKOvaxdsPNQIsOIS35382038 = -914423767;    float dATWHKOvaxdsPNQIsOIS82338499 = -986185149;    float dATWHKOvaxdsPNQIsOIS29080000 = -677817608;    float dATWHKOvaxdsPNQIsOIS84886590 = -860388068;    float dATWHKOvaxdsPNQIsOIS70015766 = -189646372;    float dATWHKOvaxdsPNQIsOIS40490560 = -348487617;    float dATWHKOvaxdsPNQIsOIS44546047 = -487563837;    float dATWHKOvaxdsPNQIsOIS86587039 = -396378374;    float dATWHKOvaxdsPNQIsOIS53924794 = -856866903;    float dATWHKOvaxdsPNQIsOIS6233689 = -501312565;    float dATWHKOvaxdsPNQIsOIS62107956 = 93759001;    float dATWHKOvaxdsPNQIsOIS49171503 = 46051978;    float dATWHKOvaxdsPNQIsOIS75181472 = -182306360;    float dATWHKOvaxdsPNQIsOIS80828772 = -389096776;    float dATWHKOvaxdsPNQIsOIS95269350 = -424049192;    float dATWHKOvaxdsPNQIsOIS54130716 = -334696772;    float dATWHKOvaxdsPNQIsOIS88384513 = -847999670;    float dATWHKOvaxdsPNQIsOIS26731706 = -47898153;    float dATWHKOvaxdsPNQIsOIS55817954 = -874111997;    float dATWHKOvaxdsPNQIsOIS52296343 = -359000101;    float dATWHKOvaxdsPNQIsOIS97760644 = -33693605;    float dATWHKOvaxdsPNQIsOIS6658060 = -704988036;    float dATWHKOvaxdsPNQIsOIS2564592 = -581094066;    float dATWHKOvaxdsPNQIsOIS26449305 = -495486534;    float dATWHKOvaxdsPNQIsOIS18183999 = -103895782;    float dATWHKOvaxdsPNQIsOIS2501413 = -697713967;    float dATWHKOvaxdsPNQIsOIS4141293 = -374654506;    float dATWHKOvaxdsPNQIsOIS45949780 = -849374819;    float dATWHKOvaxdsPNQIsOIS81595561 = -344935365;    float dATWHKOvaxdsPNQIsOIS20487293 = -171659890;    float dATWHKOvaxdsPNQIsOIS90500676 = 85994102;    float dATWHKOvaxdsPNQIsOIS34323311 = -61016610;    float dATWHKOvaxdsPNQIsOIS49933097 = -92225346;    float dATWHKOvaxdsPNQIsOIS89091143 = -362823243;    float dATWHKOvaxdsPNQIsOIS81843945 = -962750950;    float dATWHKOvaxdsPNQIsOIS46789619 = -847107777;    float dATWHKOvaxdsPNQIsOIS64640431 = -487643673;    float dATWHKOvaxdsPNQIsOIS49118758 = -207053327;    float dATWHKOvaxdsPNQIsOIS40276431 = -507549216;    float dATWHKOvaxdsPNQIsOIS16307946 = -309587870;    float dATWHKOvaxdsPNQIsOIS87794481 = -880372923;    float dATWHKOvaxdsPNQIsOIS2012694 = -64296925;    float dATWHKOvaxdsPNQIsOIS77366717 = -346522598;    float dATWHKOvaxdsPNQIsOIS17886318 = -970599757;    float dATWHKOvaxdsPNQIsOIS98154394 = -409439367;    float dATWHKOvaxdsPNQIsOIS7018064 = -208879156;    float dATWHKOvaxdsPNQIsOIS87346224 = -35586544;    float dATWHKOvaxdsPNQIsOIS28274136 = -704325401;    float dATWHKOvaxdsPNQIsOIS61171725 = -775567959;    float dATWHKOvaxdsPNQIsOIS63158796 = -785757774;    float dATWHKOvaxdsPNQIsOIS98494154 = -170819970;    float dATWHKOvaxdsPNQIsOIS33040382 = -884868484;    float dATWHKOvaxdsPNQIsOIS94384701 = -642666633;    float dATWHKOvaxdsPNQIsOIS77513961 = -517313546;    float dATWHKOvaxdsPNQIsOIS41083173 = -92239149;    float dATWHKOvaxdsPNQIsOIS28723979 = -109435731;    float dATWHKOvaxdsPNQIsOIS79773908 = -305091083;    float dATWHKOvaxdsPNQIsOIS2630695 = -82331074;    float dATWHKOvaxdsPNQIsOIS66702591 = -656492286;    float dATWHKOvaxdsPNQIsOIS67514353 = -491932405;    float dATWHKOvaxdsPNQIsOIS36349268 = -973833112;    float dATWHKOvaxdsPNQIsOIS98596266 = -638189019;    float dATWHKOvaxdsPNQIsOIS4991479 = 48556990;    float dATWHKOvaxdsPNQIsOIS33437502 = -585207013;    float dATWHKOvaxdsPNQIsOIS15733013 = -487306667;    float dATWHKOvaxdsPNQIsOIS27784645 = -845224390;    float dATWHKOvaxdsPNQIsOIS99238405 = -861722676;    float dATWHKOvaxdsPNQIsOIS86090328 = -819483118;    float dATWHKOvaxdsPNQIsOIS98984827 = -426345826;    float dATWHKOvaxdsPNQIsOIS48479731 = -576941416;    float dATWHKOvaxdsPNQIsOIS89490284 = -847053099;    float dATWHKOvaxdsPNQIsOIS39265756 = -540946343;    float dATWHKOvaxdsPNQIsOIS86455275 = -540348937;    float dATWHKOvaxdsPNQIsOIS39510008 = -464524128;    float dATWHKOvaxdsPNQIsOIS64501861 = -478627179;    float dATWHKOvaxdsPNQIsOIS95747951 = -969396681;    float dATWHKOvaxdsPNQIsOIS29291342 = -68538992;     dATWHKOvaxdsPNQIsOIS79743401 = dATWHKOvaxdsPNQIsOIS27934962;     dATWHKOvaxdsPNQIsOIS27934962 = dATWHKOvaxdsPNQIsOIS17225984;     dATWHKOvaxdsPNQIsOIS17225984 = dATWHKOvaxdsPNQIsOIS29128118;     dATWHKOvaxdsPNQIsOIS29128118 = dATWHKOvaxdsPNQIsOIS93720430;     dATWHKOvaxdsPNQIsOIS93720430 = dATWHKOvaxdsPNQIsOIS34005348;     dATWHKOvaxdsPNQIsOIS34005348 = dATWHKOvaxdsPNQIsOIS10292197;     dATWHKOvaxdsPNQIsOIS10292197 = dATWHKOvaxdsPNQIsOIS56798506;     dATWHKOvaxdsPNQIsOIS56798506 = dATWHKOvaxdsPNQIsOIS32340529;     dATWHKOvaxdsPNQIsOIS32340529 = dATWHKOvaxdsPNQIsOIS88599732;     dATWHKOvaxdsPNQIsOIS88599732 = dATWHKOvaxdsPNQIsOIS31291512;     dATWHKOvaxdsPNQIsOIS31291512 = dATWHKOvaxdsPNQIsOIS24120007;     dATWHKOvaxdsPNQIsOIS24120007 = dATWHKOvaxdsPNQIsOIS60262351;     dATWHKOvaxdsPNQIsOIS60262351 = dATWHKOvaxdsPNQIsOIS56189566;     dATWHKOvaxdsPNQIsOIS56189566 = dATWHKOvaxdsPNQIsOIS62527696;     dATWHKOvaxdsPNQIsOIS62527696 = dATWHKOvaxdsPNQIsOIS9102909;     dATWHKOvaxdsPNQIsOIS9102909 = dATWHKOvaxdsPNQIsOIS56441075;     dATWHKOvaxdsPNQIsOIS56441075 = dATWHKOvaxdsPNQIsOIS17289512;     dATWHKOvaxdsPNQIsOIS17289512 = dATWHKOvaxdsPNQIsOIS86878668;     dATWHKOvaxdsPNQIsOIS86878668 = dATWHKOvaxdsPNQIsOIS59772087;     dATWHKOvaxdsPNQIsOIS59772087 = dATWHKOvaxdsPNQIsOIS50202655;     dATWHKOvaxdsPNQIsOIS50202655 = dATWHKOvaxdsPNQIsOIS29810304;     dATWHKOvaxdsPNQIsOIS29810304 = dATWHKOvaxdsPNQIsOIS38843818;     dATWHKOvaxdsPNQIsOIS38843818 = dATWHKOvaxdsPNQIsOIS35382038;     dATWHKOvaxdsPNQIsOIS35382038 = dATWHKOvaxdsPNQIsOIS82338499;     dATWHKOvaxdsPNQIsOIS82338499 = dATWHKOvaxdsPNQIsOIS29080000;     dATWHKOvaxdsPNQIsOIS29080000 = dATWHKOvaxdsPNQIsOIS84886590;     dATWHKOvaxdsPNQIsOIS84886590 = dATWHKOvaxdsPNQIsOIS70015766;     dATWHKOvaxdsPNQIsOIS70015766 = dATWHKOvaxdsPNQIsOIS40490560;     dATWHKOvaxdsPNQIsOIS40490560 = dATWHKOvaxdsPNQIsOIS44546047;     dATWHKOvaxdsPNQIsOIS44546047 = dATWHKOvaxdsPNQIsOIS86587039;     dATWHKOvaxdsPNQIsOIS86587039 = dATWHKOvaxdsPNQIsOIS53924794;     dATWHKOvaxdsPNQIsOIS53924794 = dATWHKOvaxdsPNQIsOIS6233689;     dATWHKOvaxdsPNQIsOIS6233689 = dATWHKOvaxdsPNQIsOIS62107956;     dATWHKOvaxdsPNQIsOIS62107956 = dATWHKOvaxdsPNQIsOIS49171503;     dATWHKOvaxdsPNQIsOIS49171503 = dATWHKOvaxdsPNQIsOIS75181472;     dATWHKOvaxdsPNQIsOIS75181472 = dATWHKOvaxdsPNQIsOIS80828772;     dATWHKOvaxdsPNQIsOIS80828772 = dATWHKOvaxdsPNQIsOIS95269350;     dATWHKOvaxdsPNQIsOIS95269350 = dATWHKOvaxdsPNQIsOIS54130716;     dATWHKOvaxdsPNQIsOIS54130716 = dATWHKOvaxdsPNQIsOIS88384513;     dATWHKOvaxdsPNQIsOIS88384513 = dATWHKOvaxdsPNQIsOIS26731706;     dATWHKOvaxdsPNQIsOIS26731706 = dATWHKOvaxdsPNQIsOIS55817954;     dATWHKOvaxdsPNQIsOIS55817954 = dATWHKOvaxdsPNQIsOIS52296343;     dATWHKOvaxdsPNQIsOIS52296343 = dATWHKOvaxdsPNQIsOIS97760644;     dATWHKOvaxdsPNQIsOIS97760644 = dATWHKOvaxdsPNQIsOIS6658060;     dATWHKOvaxdsPNQIsOIS6658060 = dATWHKOvaxdsPNQIsOIS2564592;     dATWHKOvaxdsPNQIsOIS2564592 = dATWHKOvaxdsPNQIsOIS26449305;     dATWHKOvaxdsPNQIsOIS26449305 = dATWHKOvaxdsPNQIsOIS18183999;     dATWHKOvaxdsPNQIsOIS18183999 = dATWHKOvaxdsPNQIsOIS2501413;     dATWHKOvaxdsPNQIsOIS2501413 = dATWHKOvaxdsPNQIsOIS4141293;     dATWHKOvaxdsPNQIsOIS4141293 = dATWHKOvaxdsPNQIsOIS45949780;     dATWHKOvaxdsPNQIsOIS45949780 = dATWHKOvaxdsPNQIsOIS81595561;     dATWHKOvaxdsPNQIsOIS81595561 = dATWHKOvaxdsPNQIsOIS20487293;     dATWHKOvaxdsPNQIsOIS20487293 = dATWHKOvaxdsPNQIsOIS90500676;     dATWHKOvaxdsPNQIsOIS90500676 = dATWHKOvaxdsPNQIsOIS34323311;     dATWHKOvaxdsPNQIsOIS34323311 = dATWHKOvaxdsPNQIsOIS49933097;     dATWHKOvaxdsPNQIsOIS49933097 = dATWHKOvaxdsPNQIsOIS89091143;     dATWHKOvaxdsPNQIsOIS89091143 = dATWHKOvaxdsPNQIsOIS81843945;     dATWHKOvaxdsPNQIsOIS81843945 = dATWHKOvaxdsPNQIsOIS46789619;     dATWHKOvaxdsPNQIsOIS46789619 = dATWHKOvaxdsPNQIsOIS64640431;     dATWHKOvaxdsPNQIsOIS64640431 = dATWHKOvaxdsPNQIsOIS49118758;     dATWHKOvaxdsPNQIsOIS49118758 = dATWHKOvaxdsPNQIsOIS40276431;     dATWHKOvaxdsPNQIsOIS40276431 = dATWHKOvaxdsPNQIsOIS16307946;     dATWHKOvaxdsPNQIsOIS16307946 = dATWHKOvaxdsPNQIsOIS87794481;     dATWHKOvaxdsPNQIsOIS87794481 = dATWHKOvaxdsPNQIsOIS2012694;     dATWHKOvaxdsPNQIsOIS2012694 = dATWHKOvaxdsPNQIsOIS77366717;     dATWHKOvaxdsPNQIsOIS77366717 = dATWHKOvaxdsPNQIsOIS17886318;     dATWHKOvaxdsPNQIsOIS17886318 = dATWHKOvaxdsPNQIsOIS98154394;     dATWHKOvaxdsPNQIsOIS98154394 = dATWHKOvaxdsPNQIsOIS7018064;     dATWHKOvaxdsPNQIsOIS7018064 = dATWHKOvaxdsPNQIsOIS87346224;     dATWHKOvaxdsPNQIsOIS87346224 = dATWHKOvaxdsPNQIsOIS28274136;     dATWHKOvaxdsPNQIsOIS28274136 = dATWHKOvaxdsPNQIsOIS61171725;     dATWHKOvaxdsPNQIsOIS61171725 = dATWHKOvaxdsPNQIsOIS63158796;     dATWHKOvaxdsPNQIsOIS63158796 = dATWHKOvaxdsPNQIsOIS98494154;     dATWHKOvaxdsPNQIsOIS98494154 = dATWHKOvaxdsPNQIsOIS33040382;     dATWHKOvaxdsPNQIsOIS33040382 = dATWHKOvaxdsPNQIsOIS94384701;     dATWHKOvaxdsPNQIsOIS94384701 = dATWHKOvaxdsPNQIsOIS77513961;     dATWHKOvaxdsPNQIsOIS77513961 = dATWHKOvaxdsPNQIsOIS41083173;     dATWHKOvaxdsPNQIsOIS41083173 = dATWHKOvaxdsPNQIsOIS28723979;     dATWHKOvaxdsPNQIsOIS28723979 = dATWHKOvaxdsPNQIsOIS79773908;     dATWHKOvaxdsPNQIsOIS79773908 = dATWHKOvaxdsPNQIsOIS2630695;     dATWHKOvaxdsPNQIsOIS2630695 = dATWHKOvaxdsPNQIsOIS66702591;     dATWHKOvaxdsPNQIsOIS66702591 = dATWHKOvaxdsPNQIsOIS67514353;     dATWHKOvaxdsPNQIsOIS67514353 = dATWHKOvaxdsPNQIsOIS36349268;     dATWHKOvaxdsPNQIsOIS36349268 = dATWHKOvaxdsPNQIsOIS98596266;     dATWHKOvaxdsPNQIsOIS98596266 = dATWHKOvaxdsPNQIsOIS4991479;     dATWHKOvaxdsPNQIsOIS4991479 = dATWHKOvaxdsPNQIsOIS33437502;     dATWHKOvaxdsPNQIsOIS33437502 = dATWHKOvaxdsPNQIsOIS15733013;     dATWHKOvaxdsPNQIsOIS15733013 = dATWHKOvaxdsPNQIsOIS27784645;     dATWHKOvaxdsPNQIsOIS27784645 = dATWHKOvaxdsPNQIsOIS99238405;     dATWHKOvaxdsPNQIsOIS99238405 = dATWHKOvaxdsPNQIsOIS86090328;     dATWHKOvaxdsPNQIsOIS86090328 = dATWHKOvaxdsPNQIsOIS98984827;     dATWHKOvaxdsPNQIsOIS98984827 = dATWHKOvaxdsPNQIsOIS48479731;     dATWHKOvaxdsPNQIsOIS48479731 = dATWHKOvaxdsPNQIsOIS89490284;     dATWHKOvaxdsPNQIsOIS89490284 = dATWHKOvaxdsPNQIsOIS39265756;     dATWHKOvaxdsPNQIsOIS39265756 = dATWHKOvaxdsPNQIsOIS86455275;     dATWHKOvaxdsPNQIsOIS86455275 = dATWHKOvaxdsPNQIsOIS39510008;     dATWHKOvaxdsPNQIsOIS39510008 = dATWHKOvaxdsPNQIsOIS64501861;     dATWHKOvaxdsPNQIsOIS64501861 = dATWHKOvaxdsPNQIsOIS95747951;     dATWHKOvaxdsPNQIsOIS95747951 = dATWHKOvaxdsPNQIsOIS29291342;     dATWHKOvaxdsPNQIsOIS29291342 = dATWHKOvaxdsPNQIsOIS79743401;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void jaLysnwOqbClMbVrvFkF38942179() {     float EqsjXFxlfaDRHyxKxLJy50395068 = -686688122;    float EqsjXFxlfaDRHyxKxLJy96608478 = -560399295;    float EqsjXFxlfaDRHyxKxLJy97194902 = -219469177;    float EqsjXFxlfaDRHyxKxLJy69059405 = 76568685;    float EqsjXFxlfaDRHyxKxLJy15170752 = 51132372;    float EqsjXFxlfaDRHyxKxLJy24005110 = -869022547;    float EqsjXFxlfaDRHyxKxLJy28671092 = -795971297;    float EqsjXFxlfaDRHyxKxLJy13640887 = -4892576;    float EqsjXFxlfaDRHyxKxLJy78205583 = -461284088;    float EqsjXFxlfaDRHyxKxLJy11566194 = -861770649;    float EqsjXFxlfaDRHyxKxLJy73540672 = -238291430;    float EqsjXFxlfaDRHyxKxLJy69666578 = -787676114;    float EqsjXFxlfaDRHyxKxLJy40089738 = -325217424;    float EqsjXFxlfaDRHyxKxLJy40036600 = -63855387;    float EqsjXFxlfaDRHyxKxLJy45247716 = -467642907;    float EqsjXFxlfaDRHyxKxLJy41631549 = -763405762;    float EqsjXFxlfaDRHyxKxLJy50293902 = -108802280;    float EqsjXFxlfaDRHyxKxLJy37583006 = -109620742;    float EqsjXFxlfaDRHyxKxLJy68586450 = -726007581;    float EqsjXFxlfaDRHyxKxLJy18558797 = -475132941;    float EqsjXFxlfaDRHyxKxLJy5338410 = -425457769;    float EqsjXFxlfaDRHyxKxLJy91206068 = -207598952;    float EqsjXFxlfaDRHyxKxLJy40787626 = -793775899;    float EqsjXFxlfaDRHyxKxLJy69231575 = -506824673;    float EqsjXFxlfaDRHyxKxLJy46462888 = -662089837;    float EqsjXFxlfaDRHyxKxLJy59677580 = -776102053;    float EqsjXFxlfaDRHyxKxLJy93147375 = -155565844;    float EqsjXFxlfaDRHyxKxLJy77797991 = -694770139;    float EqsjXFxlfaDRHyxKxLJy72633048 = -642845834;    float EqsjXFxlfaDRHyxKxLJy83718142 = 54041092;    float EqsjXFxlfaDRHyxKxLJy93659036 = -144812060;    float EqsjXFxlfaDRHyxKxLJy98436254 = -85164066;    float EqsjXFxlfaDRHyxKxLJy89179049 = -448274173;    float EqsjXFxlfaDRHyxKxLJy27472931 = -569770683;    float EqsjXFxlfaDRHyxKxLJy27195740 = -900894801;    float EqsjXFxlfaDRHyxKxLJy35174731 = -261532908;    float EqsjXFxlfaDRHyxKxLJy24620259 = -577689983;    float EqsjXFxlfaDRHyxKxLJy48061839 = -208162299;    float EqsjXFxlfaDRHyxKxLJy32664005 = -290425409;    float EqsjXFxlfaDRHyxKxLJy83487812 = -365608032;    float EqsjXFxlfaDRHyxKxLJy86804990 = -431917031;    float EqsjXFxlfaDRHyxKxLJy84300945 = -643501066;    float EqsjXFxlfaDRHyxKxLJy1263303 = -345754153;    float EqsjXFxlfaDRHyxKxLJy4268946 = -105668577;    float EqsjXFxlfaDRHyxKxLJy2612118 = 86365569;    float EqsjXFxlfaDRHyxKxLJy8970426 = -339431221;    float EqsjXFxlfaDRHyxKxLJy52580507 = -611556334;    float EqsjXFxlfaDRHyxKxLJy83798208 = -556026078;    float EqsjXFxlfaDRHyxKxLJy7942865 = -683477936;    float EqsjXFxlfaDRHyxKxLJy12568779 = -657663384;    float EqsjXFxlfaDRHyxKxLJy14139105 = -395946940;    float EqsjXFxlfaDRHyxKxLJy23467066 = -314155087;    float EqsjXFxlfaDRHyxKxLJy75562949 = -99457932;    float EqsjXFxlfaDRHyxKxLJy80727980 = -574887395;    float EqsjXFxlfaDRHyxKxLJy33603770 = -171957524;    float EqsjXFxlfaDRHyxKxLJy59188999 = -379089171;    float EqsjXFxlfaDRHyxKxLJy55820853 = -766623397;    float EqsjXFxlfaDRHyxKxLJy27963328 = -712644504;    float EqsjXFxlfaDRHyxKxLJy22596518 = -261341478;    float EqsjXFxlfaDRHyxKxLJy55493172 = -172765576;    float EqsjXFxlfaDRHyxKxLJy30857735 = -613456704;    float EqsjXFxlfaDRHyxKxLJy50873101 = -1201158;    float EqsjXFxlfaDRHyxKxLJy41007839 = -362046742;    float EqsjXFxlfaDRHyxKxLJy94487440 = -415325181;    float EqsjXFxlfaDRHyxKxLJy17907158 = -616958590;    float EqsjXFxlfaDRHyxKxLJy75104417 = -53127364;    float EqsjXFxlfaDRHyxKxLJy80487528 = -239401941;    float EqsjXFxlfaDRHyxKxLJy12616807 = -755446741;    float EqsjXFxlfaDRHyxKxLJy12840861 = -162960586;    float EqsjXFxlfaDRHyxKxLJy10072985 = -106109999;    float EqsjXFxlfaDRHyxKxLJy17011290 = -85715780;    float EqsjXFxlfaDRHyxKxLJy2232063 = -900639981;    float EqsjXFxlfaDRHyxKxLJy4919002 = -819195333;    float EqsjXFxlfaDRHyxKxLJy85098638 = -260399549;    float EqsjXFxlfaDRHyxKxLJy31753807 = 56784090;    float EqsjXFxlfaDRHyxKxLJy21037465 = -781956704;    float EqsjXFxlfaDRHyxKxLJy89942765 = -861844800;    float EqsjXFxlfaDRHyxKxLJy36518681 = -588107322;    float EqsjXFxlfaDRHyxKxLJy66619457 = -493190243;    float EqsjXFxlfaDRHyxKxLJy37492462 = -222658616;    float EqsjXFxlfaDRHyxKxLJy7097073 = -64545719;    float EqsjXFxlfaDRHyxKxLJy9349168 = -599539766;    float EqsjXFxlfaDRHyxKxLJy69855126 = 88707797;    float EqsjXFxlfaDRHyxKxLJy60064269 = -985182450;    float EqsjXFxlfaDRHyxKxLJy69579038 = -550011968;    float EqsjXFxlfaDRHyxKxLJy70191970 = -830656973;    float EqsjXFxlfaDRHyxKxLJy22873306 = -985706134;    float EqsjXFxlfaDRHyxKxLJy8451070 = -873386779;    float EqsjXFxlfaDRHyxKxLJy93869161 = -297813160;    float EqsjXFxlfaDRHyxKxLJy68006740 = -421805631;    float EqsjXFxlfaDRHyxKxLJy79353878 = -494909511;    float EqsjXFxlfaDRHyxKxLJy96656931 = -865045480;    float EqsjXFxlfaDRHyxKxLJy25465322 = -946820822;    float EqsjXFxlfaDRHyxKxLJy77170832 = -17659834;    float EqsjXFxlfaDRHyxKxLJy52630078 = -752151329;    float EqsjXFxlfaDRHyxKxLJy35931889 = -330715873;    float EqsjXFxlfaDRHyxKxLJy43293107 = -181454324;    float EqsjXFxlfaDRHyxKxLJy6775863 = -930428973;    float EqsjXFxlfaDRHyxKxLJy86361787 = -488709988;    float EqsjXFxlfaDRHyxKxLJy27507701 = -686688122;     EqsjXFxlfaDRHyxKxLJy50395068 = EqsjXFxlfaDRHyxKxLJy96608478;     EqsjXFxlfaDRHyxKxLJy96608478 = EqsjXFxlfaDRHyxKxLJy97194902;     EqsjXFxlfaDRHyxKxLJy97194902 = EqsjXFxlfaDRHyxKxLJy69059405;     EqsjXFxlfaDRHyxKxLJy69059405 = EqsjXFxlfaDRHyxKxLJy15170752;     EqsjXFxlfaDRHyxKxLJy15170752 = EqsjXFxlfaDRHyxKxLJy24005110;     EqsjXFxlfaDRHyxKxLJy24005110 = EqsjXFxlfaDRHyxKxLJy28671092;     EqsjXFxlfaDRHyxKxLJy28671092 = EqsjXFxlfaDRHyxKxLJy13640887;     EqsjXFxlfaDRHyxKxLJy13640887 = EqsjXFxlfaDRHyxKxLJy78205583;     EqsjXFxlfaDRHyxKxLJy78205583 = EqsjXFxlfaDRHyxKxLJy11566194;     EqsjXFxlfaDRHyxKxLJy11566194 = EqsjXFxlfaDRHyxKxLJy73540672;     EqsjXFxlfaDRHyxKxLJy73540672 = EqsjXFxlfaDRHyxKxLJy69666578;     EqsjXFxlfaDRHyxKxLJy69666578 = EqsjXFxlfaDRHyxKxLJy40089738;     EqsjXFxlfaDRHyxKxLJy40089738 = EqsjXFxlfaDRHyxKxLJy40036600;     EqsjXFxlfaDRHyxKxLJy40036600 = EqsjXFxlfaDRHyxKxLJy45247716;     EqsjXFxlfaDRHyxKxLJy45247716 = EqsjXFxlfaDRHyxKxLJy41631549;     EqsjXFxlfaDRHyxKxLJy41631549 = EqsjXFxlfaDRHyxKxLJy50293902;     EqsjXFxlfaDRHyxKxLJy50293902 = EqsjXFxlfaDRHyxKxLJy37583006;     EqsjXFxlfaDRHyxKxLJy37583006 = EqsjXFxlfaDRHyxKxLJy68586450;     EqsjXFxlfaDRHyxKxLJy68586450 = EqsjXFxlfaDRHyxKxLJy18558797;     EqsjXFxlfaDRHyxKxLJy18558797 = EqsjXFxlfaDRHyxKxLJy5338410;     EqsjXFxlfaDRHyxKxLJy5338410 = EqsjXFxlfaDRHyxKxLJy91206068;     EqsjXFxlfaDRHyxKxLJy91206068 = EqsjXFxlfaDRHyxKxLJy40787626;     EqsjXFxlfaDRHyxKxLJy40787626 = EqsjXFxlfaDRHyxKxLJy69231575;     EqsjXFxlfaDRHyxKxLJy69231575 = EqsjXFxlfaDRHyxKxLJy46462888;     EqsjXFxlfaDRHyxKxLJy46462888 = EqsjXFxlfaDRHyxKxLJy59677580;     EqsjXFxlfaDRHyxKxLJy59677580 = EqsjXFxlfaDRHyxKxLJy93147375;     EqsjXFxlfaDRHyxKxLJy93147375 = EqsjXFxlfaDRHyxKxLJy77797991;     EqsjXFxlfaDRHyxKxLJy77797991 = EqsjXFxlfaDRHyxKxLJy72633048;     EqsjXFxlfaDRHyxKxLJy72633048 = EqsjXFxlfaDRHyxKxLJy83718142;     EqsjXFxlfaDRHyxKxLJy83718142 = EqsjXFxlfaDRHyxKxLJy93659036;     EqsjXFxlfaDRHyxKxLJy93659036 = EqsjXFxlfaDRHyxKxLJy98436254;     EqsjXFxlfaDRHyxKxLJy98436254 = EqsjXFxlfaDRHyxKxLJy89179049;     EqsjXFxlfaDRHyxKxLJy89179049 = EqsjXFxlfaDRHyxKxLJy27472931;     EqsjXFxlfaDRHyxKxLJy27472931 = EqsjXFxlfaDRHyxKxLJy27195740;     EqsjXFxlfaDRHyxKxLJy27195740 = EqsjXFxlfaDRHyxKxLJy35174731;     EqsjXFxlfaDRHyxKxLJy35174731 = EqsjXFxlfaDRHyxKxLJy24620259;     EqsjXFxlfaDRHyxKxLJy24620259 = EqsjXFxlfaDRHyxKxLJy48061839;     EqsjXFxlfaDRHyxKxLJy48061839 = EqsjXFxlfaDRHyxKxLJy32664005;     EqsjXFxlfaDRHyxKxLJy32664005 = EqsjXFxlfaDRHyxKxLJy83487812;     EqsjXFxlfaDRHyxKxLJy83487812 = EqsjXFxlfaDRHyxKxLJy86804990;     EqsjXFxlfaDRHyxKxLJy86804990 = EqsjXFxlfaDRHyxKxLJy84300945;     EqsjXFxlfaDRHyxKxLJy84300945 = EqsjXFxlfaDRHyxKxLJy1263303;     EqsjXFxlfaDRHyxKxLJy1263303 = EqsjXFxlfaDRHyxKxLJy4268946;     EqsjXFxlfaDRHyxKxLJy4268946 = EqsjXFxlfaDRHyxKxLJy2612118;     EqsjXFxlfaDRHyxKxLJy2612118 = EqsjXFxlfaDRHyxKxLJy8970426;     EqsjXFxlfaDRHyxKxLJy8970426 = EqsjXFxlfaDRHyxKxLJy52580507;     EqsjXFxlfaDRHyxKxLJy52580507 = EqsjXFxlfaDRHyxKxLJy83798208;     EqsjXFxlfaDRHyxKxLJy83798208 = EqsjXFxlfaDRHyxKxLJy7942865;     EqsjXFxlfaDRHyxKxLJy7942865 = EqsjXFxlfaDRHyxKxLJy12568779;     EqsjXFxlfaDRHyxKxLJy12568779 = EqsjXFxlfaDRHyxKxLJy14139105;     EqsjXFxlfaDRHyxKxLJy14139105 = EqsjXFxlfaDRHyxKxLJy23467066;     EqsjXFxlfaDRHyxKxLJy23467066 = EqsjXFxlfaDRHyxKxLJy75562949;     EqsjXFxlfaDRHyxKxLJy75562949 = EqsjXFxlfaDRHyxKxLJy80727980;     EqsjXFxlfaDRHyxKxLJy80727980 = EqsjXFxlfaDRHyxKxLJy33603770;     EqsjXFxlfaDRHyxKxLJy33603770 = EqsjXFxlfaDRHyxKxLJy59188999;     EqsjXFxlfaDRHyxKxLJy59188999 = EqsjXFxlfaDRHyxKxLJy55820853;     EqsjXFxlfaDRHyxKxLJy55820853 = EqsjXFxlfaDRHyxKxLJy27963328;     EqsjXFxlfaDRHyxKxLJy27963328 = EqsjXFxlfaDRHyxKxLJy22596518;     EqsjXFxlfaDRHyxKxLJy22596518 = EqsjXFxlfaDRHyxKxLJy55493172;     EqsjXFxlfaDRHyxKxLJy55493172 = EqsjXFxlfaDRHyxKxLJy30857735;     EqsjXFxlfaDRHyxKxLJy30857735 = EqsjXFxlfaDRHyxKxLJy50873101;     EqsjXFxlfaDRHyxKxLJy50873101 = EqsjXFxlfaDRHyxKxLJy41007839;     EqsjXFxlfaDRHyxKxLJy41007839 = EqsjXFxlfaDRHyxKxLJy94487440;     EqsjXFxlfaDRHyxKxLJy94487440 = EqsjXFxlfaDRHyxKxLJy17907158;     EqsjXFxlfaDRHyxKxLJy17907158 = EqsjXFxlfaDRHyxKxLJy75104417;     EqsjXFxlfaDRHyxKxLJy75104417 = EqsjXFxlfaDRHyxKxLJy80487528;     EqsjXFxlfaDRHyxKxLJy80487528 = EqsjXFxlfaDRHyxKxLJy12616807;     EqsjXFxlfaDRHyxKxLJy12616807 = EqsjXFxlfaDRHyxKxLJy12840861;     EqsjXFxlfaDRHyxKxLJy12840861 = EqsjXFxlfaDRHyxKxLJy10072985;     EqsjXFxlfaDRHyxKxLJy10072985 = EqsjXFxlfaDRHyxKxLJy17011290;     EqsjXFxlfaDRHyxKxLJy17011290 = EqsjXFxlfaDRHyxKxLJy2232063;     EqsjXFxlfaDRHyxKxLJy2232063 = EqsjXFxlfaDRHyxKxLJy4919002;     EqsjXFxlfaDRHyxKxLJy4919002 = EqsjXFxlfaDRHyxKxLJy85098638;     EqsjXFxlfaDRHyxKxLJy85098638 = EqsjXFxlfaDRHyxKxLJy31753807;     EqsjXFxlfaDRHyxKxLJy31753807 = EqsjXFxlfaDRHyxKxLJy21037465;     EqsjXFxlfaDRHyxKxLJy21037465 = EqsjXFxlfaDRHyxKxLJy89942765;     EqsjXFxlfaDRHyxKxLJy89942765 = EqsjXFxlfaDRHyxKxLJy36518681;     EqsjXFxlfaDRHyxKxLJy36518681 = EqsjXFxlfaDRHyxKxLJy66619457;     EqsjXFxlfaDRHyxKxLJy66619457 = EqsjXFxlfaDRHyxKxLJy37492462;     EqsjXFxlfaDRHyxKxLJy37492462 = EqsjXFxlfaDRHyxKxLJy7097073;     EqsjXFxlfaDRHyxKxLJy7097073 = EqsjXFxlfaDRHyxKxLJy9349168;     EqsjXFxlfaDRHyxKxLJy9349168 = EqsjXFxlfaDRHyxKxLJy69855126;     EqsjXFxlfaDRHyxKxLJy69855126 = EqsjXFxlfaDRHyxKxLJy60064269;     EqsjXFxlfaDRHyxKxLJy60064269 = EqsjXFxlfaDRHyxKxLJy69579038;     EqsjXFxlfaDRHyxKxLJy69579038 = EqsjXFxlfaDRHyxKxLJy70191970;     EqsjXFxlfaDRHyxKxLJy70191970 = EqsjXFxlfaDRHyxKxLJy22873306;     EqsjXFxlfaDRHyxKxLJy22873306 = EqsjXFxlfaDRHyxKxLJy8451070;     EqsjXFxlfaDRHyxKxLJy8451070 = EqsjXFxlfaDRHyxKxLJy93869161;     EqsjXFxlfaDRHyxKxLJy93869161 = EqsjXFxlfaDRHyxKxLJy68006740;     EqsjXFxlfaDRHyxKxLJy68006740 = EqsjXFxlfaDRHyxKxLJy79353878;     EqsjXFxlfaDRHyxKxLJy79353878 = EqsjXFxlfaDRHyxKxLJy96656931;     EqsjXFxlfaDRHyxKxLJy96656931 = EqsjXFxlfaDRHyxKxLJy25465322;     EqsjXFxlfaDRHyxKxLJy25465322 = EqsjXFxlfaDRHyxKxLJy77170832;     EqsjXFxlfaDRHyxKxLJy77170832 = EqsjXFxlfaDRHyxKxLJy52630078;     EqsjXFxlfaDRHyxKxLJy52630078 = EqsjXFxlfaDRHyxKxLJy35931889;     EqsjXFxlfaDRHyxKxLJy35931889 = EqsjXFxlfaDRHyxKxLJy43293107;     EqsjXFxlfaDRHyxKxLJy43293107 = EqsjXFxlfaDRHyxKxLJy6775863;     EqsjXFxlfaDRHyxKxLJy6775863 = EqsjXFxlfaDRHyxKxLJy86361787;     EqsjXFxlfaDRHyxKxLJy86361787 = EqsjXFxlfaDRHyxKxLJy27507701;     EqsjXFxlfaDRHyxKxLJy27507701 = EqsjXFxlfaDRHyxKxLJy50395068;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DtDUpgkUBlHTMEJpXyrM45600924() {     float WLfbdQnYGTFmcHADcDEj50388597 = -559212264;    float WLfbdQnYGTFmcHADcDEj35180796 = -602994421;    float WLfbdQnYGTFmcHADcDEj98508114 = -176597791;    float WLfbdQnYGTFmcHADcDEj54139266 = -843972145;    float WLfbdQnYGTFmcHADcDEj24010854 = -417222949;    float WLfbdQnYGTFmcHADcDEj58810542 = -568455358;    float WLfbdQnYGTFmcHADcDEj97218427 = -500507764;    float WLfbdQnYGTFmcHADcDEj82854937 = -809069893;    float WLfbdQnYGTFmcHADcDEj60945747 = -174930158;    float WLfbdQnYGTFmcHADcDEj8205707 = -498577520;    float WLfbdQnYGTFmcHADcDEj93861277 = -197763502;    float WLfbdQnYGTFmcHADcDEj83958255 = -523154477;    float WLfbdQnYGTFmcHADcDEj71942652 = -346875898;    float WLfbdQnYGTFmcHADcDEj93880820 = -409008439;    float WLfbdQnYGTFmcHADcDEj90419824 = -430355759;    float WLfbdQnYGTFmcHADcDEj16196096 = 3798587;    float WLfbdQnYGTFmcHADcDEj6146662 = 71583150;    float WLfbdQnYGTFmcHADcDEj60583539 = -506692364;    float WLfbdQnYGTFmcHADcDEj89014720 = -243989848;    float WLfbdQnYGTFmcHADcDEj90632559 = 12842608;    float WLfbdQnYGTFmcHADcDEj85007834 = -135924143;    float WLfbdQnYGTFmcHADcDEj69502334 = -717590835;    float WLfbdQnYGTFmcHADcDEj77666023 = -152733741;    float WLfbdQnYGTFmcHADcDEj12881683 = -387745196;    float WLfbdQnYGTFmcHADcDEj74905577 = 11216310;    float WLfbdQnYGTFmcHADcDEj76251657 = -684307981;    float WLfbdQnYGTFmcHADcDEj63368957 = -456365556;    float WLfbdQnYGTFmcHADcDEj13201516 = -203737128;    float WLfbdQnYGTFmcHADcDEj85311495 = -188916149;    float WLfbdQnYGTFmcHADcDEj59808013 = -721390282;    float WLfbdQnYGTFmcHADcDEj27071715 = -531905294;    float WLfbdQnYGTFmcHADcDEj83160112 = -476985827;    float WLfbdQnYGTFmcHADcDEj74631473 = -650218389;    float WLfbdQnYGTFmcHADcDEj27755768 = -446452416;    float WLfbdQnYGTFmcHADcDEj9553778 = -60208726;    float WLfbdQnYGTFmcHADcDEj78002921 = -341298654;    float WLfbdQnYGTFmcHADcDEj36561627 = -677343361;    float WLfbdQnYGTFmcHADcDEj17979519 = -203424978;    float WLfbdQnYGTFmcHADcDEj67977601 = 96893175;    float WLfbdQnYGTFmcHADcDEj55360705 = 24960282;    float WLfbdQnYGTFmcHADcDEj74660657 = -519860929;    float WLfbdQnYGTFmcHADcDEj69998606 = -837293148;    float WLfbdQnYGTFmcHADcDEj61546663 = -904884407;    float WLfbdQnYGTFmcHADcDEj63500843 = -762633020;    float WLfbdQnYGTFmcHADcDEj86630170 = -386893833;    float WLfbdQnYGTFmcHADcDEj49398394 = -151475892;    float WLfbdQnYGTFmcHADcDEj80278300 = 7163044;    float WLfbdQnYGTFmcHADcDEj59181789 = -64787434;    float WLfbdQnYGTFmcHADcDEj26601255 = -247241798;    float WLfbdQnYGTFmcHADcDEj32614202 = -244688271;    float WLfbdQnYGTFmcHADcDEj27398474 = 25498053;    float WLfbdQnYGTFmcHADcDEj38014323 = -696300835;    float WLfbdQnYGTFmcHADcDEj51011298 = 8488060;    float WLfbdQnYGTFmcHADcDEj90978251 = -6807246;    float WLfbdQnYGTFmcHADcDEj60593090 = 71143198;    float WLfbdQnYGTFmcHADcDEj80886262 = -841621429;    float WLfbdQnYGTFmcHADcDEj57514772 = -350260681;    float WLfbdQnYGTFmcHADcDEj85626431 = -788852595;    float WLfbdQnYGTFmcHADcDEj79233688 = -755188455;    float WLfbdQnYGTFmcHADcDEj47759197 = -732914969;    float WLfbdQnYGTFmcHADcDEj95441585 = -12089802;    float WLfbdQnYGTFmcHADcDEj84016911 = -196770637;    float WLfbdQnYGTFmcHADcDEj97543442 = -520153745;    float WLfbdQnYGTFmcHADcDEj1137735 = -453539877;    float WLfbdQnYGTFmcHADcDEj81133992 = -966672226;    float WLfbdQnYGTFmcHADcDEj10701166 = -720777675;    float WLfbdQnYGTFmcHADcDEj9326782 = -872936088;    float WLfbdQnYGTFmcHADcDEj44186884 = -900423483;    float WLfbdQnYGTFmcHADcDEj84327042 = -248799713;    float WLfbdQnYGTFmcHADcDEj12416904 = 10942895;    float WLfbdQnYGTFmcHADcDEj79634469 = -318858052;    float WLfbdQnYGTFmcHADcDEj88167143 = -724991872;    float WLfbdQnYGTFmcHADcDEj92605938 = -503585539;    float WLfbdQnYGTFmcHADcDEj33654016 = -168950130;    float WLfbdQnYGTFmcHADcDEj15971902 = -467296463;    float WLfbdQnYGTFmcHADcDEj15009228 = -298630995;    float WLfbdQnYGTFmcHADcDEj7955672 = -812706429;    float WLfbdQnYGTFmcHADcDEj14165180 = -390100721;    float WLfbdQnYGTFmcHADcDEj26251513 = 99148636;    float WLfbdQnYGTFmcHADcDEj25507183 = -837307799;    float WLfbdQnYGTFmcHADcDEj95973356 = -591471025;    float WLfbdQnYGTFmcHADcDEj4187169 = -291578123;    float WLfbdQnYGTFmcHADcDEj86600261 = -956495330;    float WLfbdQnYGTFmcHADcDEj52697293 = -944227879;    float WLfbdQnYGTFmcHADcDEj32409539 = -646888335;    float WLfbdQnYGTFmcHADcDEj89057391 = -835604460;    float WLfbdQnYGTFmcHADcDEj32148815 = -385473887;    float WLfbdQnYGTFmcHADcDEj83653221 = -543411144;    float WLfbdQnYGTFmcHADcDEj67162678 = -417595614;    float WLfbdQnYGTFmcHADcDEj28667515 = -218587298;    float WLfbdQnYGTFmcHADcDEj20488149 = -991037974;    float WLfbdQnYGTFmcHADcDEj50935195 = -888490767;    float WLfbdQnYGTFmcHADcDEj38745830 = -448236524;    float WLfbdQnYGTFmcHADcDEj20218404 = -170191857;    float WLfbdQnYGTFmcHADcDEj59919120 = -962949916;    float WLfbdQnYGTFmcHADcDEj90643745 = -223090293;    float WLfbdQnYGTFmcHADcDEj72455164 = -217139404;    float WLfbdQnYGTFmcHADcDEj60408928 = -351344531;    float WLfbdQnYGTFmcHADcDEj82366851 = -795960794;    float WLfbdQnYGTFmcHADcDEj75929005 = -559212264;     WLfbdQnYGTFmcHADcDEj50388597 = WLfbdQnYGTFmcHADcDEj35180796;     WLfbdQnYGTFmcHADcDEj35180796 = WLfbdQnYGTFmcHADcDEj98508114;     WLfbdQnYGTFmcHADcDEj98508114 = WLfbdQnYGTFmcHADcDEj54139266;     WLfbdQnYGTFmcHADcDEj54139266 = WLfbdQnYGTFmcHADcDEj24010854;     WLfbdQnYGTFmcHADcDEj24010854 = WLfbdQnYGTFmcHADcDEj58810542;     WLfbdQnYGTFmcHADcDEj58810542 = WLfbdQnYGTFmcHADcDEj97218427;     WLfbdQnYGTFmcHADcDEj97218427 = WLfbdQnYGTFmcHADcDEj82854937;     WLfbdQnYGTFmcHADcDEj82854937 = WLfbdQnYGTFmcHADcDEj60945747;     WLfbdQnYGTFmcHADcDEj60945747 = WLfbdQnYGTFmcHADcDEj8205707;     WLfbdQnYGTFmcHADcDEj8205707 = WLfbdQnYGTFmcHADcDEj93861277;     WLfbdQnYGTFmcHADcDEj93861277 = WLfbdQnYGTFmcHADcDEj83958255;     WLfbdQnYGTFmcHADcDEj83958255 = WLfbdQnYGTFmcHADcDEj71942652;     WLfbdQnYGTFmcHADcDEj71942652 = WLfbdQnYGTFmcHADcDEj93880820;     WLfbdQnYGTFmcHADcDEj93880820 = WLfbdQnYGTFmcHADcDEj90419824;     WLfbdQnYGTFmcHADcDEj90419824 = WLfbdQnYGTFmcHADcDEj16196096;     WLfbdQnYGTFmcHADcDEj16196096 = WLfbdQnYGTFmcHADcDEj6146662;     WLfbdQnYGTFmcHADcDEj6146662 = WLfbdQnYGTFmcHADcDEj60583539;     WLfbdQnYGTFmcHADcDEj60583539 = WLfbdQnYGTFmcHADcDEj89014720;     WLfbdQnYGTFmcHADcDEj89014720 = WLfbdQnYGTFmcHADcDEj90632559;     WLfbdQnYGTFmcHADcDEj90632559 = WLfbdQnYGTFmcHADcDEj85007834;     WLfbdQnYGTFmcHADcDEj85007834 = WLfbdQnYGTFmcHADcDEj69502334;     WLfbdQnYGTFmcHADcDEj69502334 = WLfbdQnYGTFmcHADcDEj77666023;     WLfbdQnYGTFmcHADcDEj77666023 = WLfbdQnYGTFmcHADcDEj12881683;     WLfbdQnYGTFmcHADcDEj12881683 = WLfbdQnYGTFmcHADcDEj74905577;     WLfbdQnYGTFmcHADcDEj74905577 = WLfbdQnYGTFmcHADcDEj76251657;     WLfbdQnYGTFmcHADcDEj76251657 = WLfbdQnYGTFmcHADcDEj63368957;     WLfbdQnYGTFmcHADcDEj63368957 = WLfbdQnYGTFmcHADcDEj13201516;     WLfbdQnYGTFmcHADcDEj13201516 = WLfbdQnYGTFmcHADcDEj85311495;     WLfbdQnYGTFmcHADcDEj85311495 = WLfbdQnYGTFmcHADcDEj59808013;     WLfbdQnYGTFmcHADcDEj59808013 = WLfbdQnYGTFmcHADcDEj27071715;     WLfbdQnYGTFmcHADcDEj27071715 = WLfbdQnYGTFmcHADcDEj83160112;     WLfbdQnYGTFmcHADcDEj83160112 = WLfbdQnYGTFmcHADcDEj74631473;     WLfbdQnYGTFmcHADcDEj74631473 = WLfbdQnYGTFmcHADcDEj27755768;     WLfbdQnYGTFmcHADcDEj27755768 = WLfbdQnYGTFmcHADcDEj9553778;     WLfbdQnYGTFmcHADcDEj9553778 = WLfbdQnYGTFmcHADcDEj78002921;     WLfbdQnYGTFmcHADcDEj78002921 = WLfbdQnYGTFmcHADcDEj36561627;     WLfbdQnYGTFmcHADcDEj36561627 = WLfbdQnYGTFmcHADcDEj17979519;     WLfbdQnYGTFmcHADcDEj17979519 = WLfbdQnYGTFmcHADcDEj67977601;     WLfbdQnYGTFmcHADcDEj67977601 = WLfbdQnYGTFmcHADcDEj55360705;     WLfbdQnYGTFmcHADcDEj55360705 = WLfbdQnYGTFmcHADcDEj74660657;     WLfbdQnYGTFmcHADcDEj74660657 = WLfbdQnYGTFmcHADcDEj69998606;     WLfbdQnYGTFmcHADcDEj69998606 = WLfbdQnYGTFmcHADcDEj61546663;     WLfbdQnYGTFmcHADcDEj61546663 = WLfbdQnYGTFmcHADcDEj63500843;     WLfbdQnYGTFmcHADcDEj63500843 = WLfbdQnYGTFmcHADcDEj86630170;     WLfbdQnYGTFmcHADcDEj86630170 = WLfbdQnYGTFmcHADcDEj49398394;     WLfbdQnYGTFmcHADcDEj49398394 = WLfbdQnYGTFmcHADcDEj80278300;     WLfbdQnYGTFmcHADcDEj80278300 = WLfbdQnYGTFmcHADcDEj59181789;     WLfbdQnYGTFmcHADcDEj59181789 = WLfbdQnYGTFmcHADcDEj26601255;     WLfbdQnYGTFmcHADcDEj26601255 = WLfbdQnYGTFmcHADcDEj32614202;     WLfbdQnYGTFmcHADcDEj32614202 = WLfbdQnYGTFmcHADcDEj27398474;     WLfbdQnYGTFmcHADcDEj27398474 = WLfbdQnYGTFmcHADcDEj38014323;     WLfbdQnYGTFmcHADcDEj38014323 = WLfbdQnYGTFmcHADcDEj51011298;     WLfbdQnYGTFmcHADcDEj51011298 = WLfbdQnYGTFmcHADcDEj90978251;     WLfbdQnYGTFmcHADcDEj90978251 = WLfbdQnYGTFmcHADcDEj60593090;     WLfbdQnYGTFmcHADcDEj60593090 = WLfbdQnYGTFmcHADcDEj80886262;     WLfbdQnYGTFmcHADcDEj80886262 = WLfbdQnYGTFmcHADcDEj57514772;     WLfbdQnYGTFmcHADcDEj57514772 = WLfbdQnYGTFmcHADcDEj85626431;     WLfbdQnYGTFmcHADcDEj85626431 = WLfbdQnYGTFmcHADcDEj79233688;     WLfbdQnYGTFmcHADcDEj79233688 = WLfbdQnYGTFmcHADcDEj47759197;     WLfbdQnYGTFmcHADcDEj47759197 = WLfbdQnYGTFmcHADcDEj95441585;     WLfbdQnYGTFmcHADcDEj95441585 = WLfbdQnYGTFmcHADcDEj84016911;     WLfbdQnYGTFmcHADcDEj84016911 = WLfbdQnYGTFmcHADcDEj97543442;     WLfbdQnYGTFmcHADcDEj97543442 = WLfbdQnYGTFmcHADcDEj1137735;     WLfbdQnYGTFmcHADcDEj1137735 = WLfbdQnYGTFmcHADcDEj81133992;     WLfbdQnYGTFmcHADcDEj81133992 = WLfbdQnYGTFmcHADcDEj10701166;     WLfbdQnYGTFmcHADcDEj10701166 = WLfbdQnYGTFmcHADcDEj9326782;     WLfbdQnYGTFmcHADcDEj9326782 = WLfbdQnYGTFmcHADcDEj44186884;     WLfbdQnYGTFmcHADcDEj44186884 = WLfbdQnYGTFmcHADcDEj84327042;     WLfbdQnYGTFmcHADcDEj84327042 = WLfbdQnYGTFmcHADcDEj12416904;     WLfbdQnYGTFmcHADcDEj12416904 = WLfbdQnYGTFmcHADcDEj79634469;     WLfbdQnYGTFmcHADcDEj79634469 = WLfbdQnYGTFmcHADcDEj88167143;     WLfbdQnYGTFmcHADcDEj88167143 = WLfbdQnYGTFmcHADcDEj92605938;     WLfbdQnYGTFmcHADcDEj92605938 = WLfbdQnYGTFmcHADcDEj33654016;     WLfbdQnYGTFmcHADcDEj33654016 = WLfbdQnYGTFmcHADcDEj15971902;     WLfbdQnYGTFmcHADcDEj15971902 = WLfbdQnYGTFmcHADcDEj15009228;     WLfbdQnYGTFmcHADcDEj15009228 = WLfbdQnYGTFmcHADcDEj7955672;     WLfbdQnYGTFmcHADcDEj7955672 = WLfbdQnYGTFmcHADcDEj14165180;     WLfbdQnYGTFmcHADcDEj14165180 = WLfbdQnYGTFmcHADcDEj26251513;     WLfbdQnYGTFmcHADcDEj26251513 = WLfbdQnYGTFmcHADcDEj25507183;     WLfbdQnYGTFmcHADcDEj25507183 = WLfbdQnYGTFmcHADcDEj95973356;     WLfbdQnYGTFmcHADcDEj95973356 = WLfbdQnYGTFmcHADcDEj4187169;     WLfbdQnYGTFmcHADcDEj4187169 = WLfbdQnYGTFmcHADcDEj86600261;     WLfbdQnYGTFmcHADcDEj86600261 = WLfbdQnYGTFmcHADcDEj52697293;     WLfbdQnYGTFmcHADcDEj52697293 = WLfbdQnYGTFmcHADcDEj32409539;     WLfbdQnYGTFmcHADcDEj32409539 = WLfbdQnYGTFmcHADcDEj89057391;     WLfbdQnYGTFmcHADcDEj89057391 = WLfbdQnYGTFmcHADcDEj32148815;     WLfbdQnYGTFmcHADcDEj32148815 = WLfbdQnYGTFmcHADcDEj83653221;     WLfbdQnYGTFmcHADcDEj83653221 = WLfbdQnYGTFmcHADcDEj67162678;     WLfbdQnYGTFmcHADcDEj67162678 = WLfbdQnYGTFmcHADcDEj28667515;     WLfbdQnYGTFmcHADcDEj28667515 = WLfbdQnYGTFmcHADcDEj20488149;     WLfbdQnYGTFmcHADcDEj20488149 = WLfbdQnYGTFmcHADcDEj50935195;     WLfbdQnYGTFmcHADcDEj50935195 = WLfbdQnYGTFmcHADcDEj38745830;     WLfbdQnYGTFmcHADcDEj38745830 = WLfbdQnYGTFmcHADcDEj20218404;     WLfbdQnYGTFmcHADcDEj20218404 = WLfbdQnYGTFmcHADcDEj59919120;     WLfbdQnYGTFmcHADcDEj59919120 = WLfbdQnYGTFmcHADcDEj90643745;     WLfbdQnYGTFmcHADcDEj90643745 = WLfbdQnYGTFmcHADcDEj72455164;     WLfbdQnYGTFmcHADcDEj72455164 = WLfbdQnYGTFmcHADcDEj60408928;     WLfbdQnYGTFmcHADcDEj60408928 = WLfbdQnYGTFmcHADcDEj82366851;     WLfbdQnYGTFmcHADcDEj82366851 = WLfbdQnYGTFmcHADcDEj75929005;     WLfbdQnYGTFmcHADcDEj75929005 = WLfbdQnYGTFmcHADcDEj50388597;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void PRgaadBaIqLGYdvItJQC17138() {     float UgANJMEMUVosnXnXydke21040264 = -77361394;    float UgANJMEMUVosnXnXydke3854313 = -474637720;    float UgANJMEMUVosnXnXydke78477033 = -618892250;    float UgANJMEMUVosnXnXydke94070552 = 65889466;    float UgANJMEMUVosnXnXydke45461176 = -200629297;    float UgANJMEMUVosnXnXydke48810305 = -270036510;    float UgANJMEMUVosnXnXydke15597324 = -499283474;    float UgANJMEMUVosnXnXydke39697318 = -55886983;    float UgANJMEMUVosnXnXydke6810802 = -268277486;    float UgANJMEMUVosnXnXydke31172168 = -799672871;    float UgANJMEMUVosnXnXydke36110438 = -232665431;    float UgANJMEMUVosnXnXydke29504826 = -838918270;    float UgANJMEMUVosnXnXydke51770039 = -256412956;    float UgANJMEMUVosnXnXydke77727854 = -210036649;    float UgANJMEMUVosnXnXydke73139844 = -580105762;    float UgANJMEMUVosnXnXydke48724736 = -666185000;    float UgANJMEMUVosnXnXydke99999488 = -937601980;    float UgANJMEMUVosnXnXydke80877033 = -495858560;    float UgANJMEMUVosnXnXydke70722502 = -951177790;    float UgANJMEMUVosnXnXydke49419269 = -529523697;    float UgANJMEMUVosnXnXydke40143589 = -44603284;    float UgANJMEMUVosnXnXydke30898099 = 51123859;    float UgANJMEMUVosnXnXydke79609831 = -720576885;    float UgANJMEMUVosnXnXydke46731220 = 19853898;    float UgANJMEMUVosnXnXydke39029966 = -764688378;    float UgANJMEMUVosnXnXydke6849238 = -782592426;    float UgANJMEMUVosnXnXydke71629743 = -851543332;    float UgANJMEMUVosnXnXydke20983742 = -708860895;    float UgANJMEMUVosnXnXydke17453983 = -483274366;    float UgANJMEMUVosnXnXydke98980108 = -179785352;    float UgANJMEMUVosnXnXydke34143711 = -280338980;    float UgANJMEMUVosnXnXydke27671573 = -805282990;    float UgANJMEMUVosnXnXydke57576834 = -597179998;    float UgANJMEMUVosnXnXydke93120743 = -9982100;    float UgANJMEMUVosnXnXydke87578013 = 92844494;    float UgANJMEMUVosnXnXydke37996180 = -420525202;    float UgANJMEMUVosnXnXydke80353112 = -865936569;    float UgANJMEMUVosnXnXydke70772007 = 12461914;    float UgANJMEMUVosnXnXydke46510890 = -958835462;    float UgANJMEMUVosnXnXydke50464003 = -592648081;    float UgANJMEMUVosnXnXydke34733942 = -903879808;    float UgANJMEMUVosnXnXydke98481597 = -606682218;    float UgANJMEMUVosnXnXydke10513623 = -891638459;    float UgANJMEMUVosnXnXydke70009144 = -834607992;    float UgANJMEMUVosnXnXydke82584229 = -695540227;    float UgANJMEMUVosnXnXydke55804229 = 90186953;    float UgANJMEMUVosnXnXydke6409504 = -108906755;    float UgANJMEMUVosnXnXydke24795999 = -516917729;    float UgANJMEMUVosnXnXydke32042707 = -233005767;    float UgANJMEMUVosnXnXydke41041688 = -527697149;    float UgANJMEMUVosnXnXydke95587798 = -621074068;    float UgANJMEMUVosnXnXydke79885827 = -665520557;    float UgANJMEMUVosnXnXydke6086955 = 80690018;    float UgANJMEMUVosnXnXydke81205556 = -667688743;    float UgANJMEMUVosnXnXydke59873549 = -39797716;    float UgANJMEMUVosnXnXydke90142164 = -28485254;    float UgANJMEMUVosnXnXydke24244482 = -754060835;    float UgANJMEMUVosnXnXydke31745813 = -538746149;    float UgANJMEMUVosnXnXydke55040587 = -169422157;    float UgANJMEMUVosnXnXydke38611938 = -418036872;    float UgANJMEMUVosnXnXydke77180562 = -418493179;    float UgANJMEMUVosnXnXydke94613582 = -790422579;    float UgANJMEMUVosnXnXydke22243335 = -572612618;    float UgANJMEMUVosnXnXydke7830693 = 11507865;    float UgANJMEMUVosnXnXydke97028456 = -419333891;    float UgANJMEMUVosnXnXydke8438866 = -427382441;    float UgANJMEMUVosnXnXydke71927992 = -141738272;    float UgANJMEMUVosnXnXydke58649296 = -146430857;    float UgANJMEMUVosnXnXydke90149840 = -202881144;    float UgANJMEMUVosnXnXydke35143664 = -59580560;    float UgANJMEMUVosnXnXydke68371624 = -800248431;    float UgANJMEMUVosnXnXydke29227481 = -850063894;    float UgANJMEMUVosnXnXydke34366144 = -537023098;    float UgANJMEMUVosnXnXydke20258499 = -258529709;    float UgANJMEMUVosnXnXydke14685328 = -625643890;    float UgANJMEMUVosnXnXydke41661991 = -437921066;    float UgANJMEMUVosnXnXydke20384477 = -57237682;    float UgANJMEMUVosnXnXydke9600688 = -885968894;    float UgANJMEMUVosnXnXydke64146991 = -284605876;    float UgANJMEMUVosnXnXydke83225736 = -754875331;    float UgANJMEMUVosnXnXydke439735 = -573685671;    float UgANJMEMUVosnXnXydke46833744 = -234625603;    float UgANJMEMUVosnXnXydke88941034 = -375855129;    float UgANJMEMUVosnXnXydke76412294 = -955577217;    float UgANJMEMUVosnXnXydke3392310 = -558711284;    float UgANJMEMUVosnXnXydke54257884 = -614818423;    float UgANJMEMUVosnXnXydke21584618 = -785973008;    float UgANJMEMUVosnXnXydke76371278 = -929491256;    float UgANJMEMUVosnXnXydke33247195 = -970184384;    float UgANJMEMUVosnXnXydke97435849 = -878670252;    float UgANJMEMUVosnXnXydke13751699 = -666464368;    float UgANJMEMUVosnXnXydke48607299 = -227190421;    float UgANJMEMUVosnXnXydke15731421 = -818115930;    float UgANJMEMUVosnXnXydke7898952 = -440798591;    float UgANJMEMUVosnXnXydke73283441 = -74154903;    float UgANJMEMUVosnXnXydke40120359 = -13457229;    float UgANJMEMUVosnXnXydke76238262 = 65930399;    float UgANJMEMUVosnXnXydke2682930 = -803146325;    float UgANJMEMUVosnXnXydke72980687 = -315274101;    float UgANJMEMUVosnXnXydke74145364 = -77361394;     UgANJMEMUVosnXnXydke21040264 = UgANJMEMUVosnXnXydke3854313;     UgANJMEMUVosnXnXydke3854313 = UgANJMEMUVosnXnXydke78477033;     UgANJMEMUVosnXnXydke78477033 = UgANJMEMUVosnXnXydke94070552;     UgANJMEMUVosnXnXydke94070552 = UgANJMEMUVosnXnXydke45461176;     UgANJMEMUVosnXnXydke45461176 = UgANJMEMUVosnXnXydke48810305;     UgANJMEMUVosnXnXydke48810305 = UgANJMEMUVosnXnXydke15597324;     UgANJMEMUVosnXnXydke15597324 = UgANJMEMUVosnXnXydke39697318;     UgANJMEMUVosnXnXydke39697318 = UgANJMEMUVosnXnXydke6810802;     UgANJMEMUVosnXnXydke6810802 = UgANJMEMUVosnXnXydke31172168;     UgANJMEMUVosnXnXydke31172168 = UgANJMEMUVosnXnXydke36110438;     UgANJMEMUVosnXnXydke36110438 = UgANJMEMUVosnXnXydke29504826;     UgANJMEMUVosnXnXydke29504826 = UgANJMEMUVosnXnXydke51770039;     UgANJMEMUVosnXnXydke51770039 = UgANJMEMUVosnXnXydke77727854;     UgANJMEMUVosnXnXydke77727854 = UgANJMEMUVosnXnXydke73139844;     UgANJMEMUVosnXnXydke73139844 = UgANJMEMUVosnXnXydke48724736;     UgANJMEMUVosnXnXydke48724736 = UgANJMEMUVosnXnXydke99999488;     UgANJMEMUVosnXnXydke99999488 = UgANJMEMUVosnXnXydke80877033;     UgANJMEMUVosnXnXydke80877033 = UgANJMEMUVosnXnXydke70722502;     UgANJMEMUVosnXnXydke70722502 = UgANJMEMUVosnXnXydke49419269;     UgANJMEMUVosnXnXydke49419269 = UgANJMEMUVosnXnXydke40143589;     UgANJMEMUVosnXnXydke40143589 = UgANJMEMUVosnXnXydke30898099;     UgANJMEMUVosnXnXydke30898099 = UgANJMEMUVosnXnXydke79609831;     UgANJMEMUVosnXnXydke79609831 = UgANJMEMUVosnXnXydke46731220;     UgANJMEMUVosnXnXydke46731220 = UgANJMEMUVosnXnXydke39029966;     UgANJMEMUVosnXnXydke39029966 = UgANJMEMUVosnXnXydke6849238;     UgANJMEMUVosnXnXydke6849238 = UgANJMEMUVosnXnXydke71629743;     UgANJMEMUVosnXnXydke71629743 = UgANJMEMUVosnXnXydke20983742;     UgANJMEMUVosnXnXydke20983742 = UgANJMEMUVosnXnXydke17453983;     UgANJMEMUVosnXnXydke17453983 = UgANJMEMUVosnXnXydke98980108;     UgANJMEMUVosnXnXydke98980108 = UgANJMEMUVosnXnXydke34143711;     UgANJMEMUVosnXnXydke34143711 = UgANJMEMUVosnXnXydke27671573;     UgANJMEMUVosnXnXydke27671573 = UgANJMEMUVosnXnXydke57576834;     UgANJMEMUVosnXnXydke57576834 = UgANJMEMUVosnXnXydke93120743;     UgANJMEMUVosnXnXydke93120743 = UgANJMEMUVosnXnXydke87578013;     UgANJMEMUVosnXnXydke87578013 = UgANJMEMUVosnXnXydke37996180;     UgANJMEMUVosnXnXydke37996180 = UgANJMEMUVosnXnXydke80353112;     UgANJMEMUVosnXnXydke80353112 = UgANJMEMUVosnXnXydke70772007;     UgANJMEMUVosnXnXydke70772007 = UgANJMEMUVosnXnXydke46510890;     UgANJMEMUVosnXnXydke46510890 = UgANJMEMUVosnXnXydke50464003;     UgANJMEMUVosnXnXydke50464003 = UgANJMEMUVosnXnXydke34733942;     UgANJMEMUVosnXnXydke34733942 = UgANJMEMUVosnXnXydke98481597;     UgANJMEMUVosnXnXydke98481597 = UgANJMEMUVosnXnXydke10513623;     UgANJMEMUVosnXnXydke10513623 = UgANJMEMUVosnXnXydke70009144;     UgANJMEMUVosnXnXydke70009144 = UgANJMEMUVosnXnXydke82584229;     UgANJMEMUVosnXnXydke82584229 = UgANJMEMUVosnXnXydke55804229;     UgANJMEMUVosnXnXydke55804229 = UgANJMEMUVosnXnXydke6409504;     UgANJMEMUVosnXnXydke6409504 = UgANJMEMUVosnXnXydke24795999;     UgANJMEMUVosnXnXydke24795999 = UgANJMEMUVosnXnXydke32042707;     UgANJMEMUVosnXnXydke32042707 = UgANJMEMUVosnXnXydke41041688;     UgANJMEMUVosnXnXydke41041688 = UgANJMEMUVosnXnXydke95587798;     UgANJMEMUVosnXnXydke95587798 = UgANJMEMUVosnXnXydke79885827;     UgANJMEMUVosnXnXydke79885827 = UgANJMEMUVosnXnXydke6086955;     UgANJMEMUVosnXnXydke6086955 = UgANJMEMUVosnXnXydke81205556;     UgANJMEMUVosnXnXydke81205556 = UgANJMEMUVosnXnXydke59873549;     UgANJMEMUVosnXnXydke59873549 = UgANJMEMUVosnXnXydke90142164;     UgANJMEMUVosnXnXydke90142164 = UgANJMEMUVosnXnXydke24244482;     UgANJMEMUVosnXnXydke24244482 = UgANJMEMUVosnXnXydke31745813;     UgANJMEMUVosnXnXydke31745813 = UgANJMEMUVosnXnXydke55040587;     UgANJMEMUVosnXnXydke55040587 = UgANJMEMUVosnXnXydke38611938;     UgANJMEMUVosnXnXydke38611938 = UgANJMEMUVosnXnXydke77180562;     UgANJMEMUVosnXnXydke77180562 = UgANJMEMUVosnXnXydke94613582;     UgANJMEMUVosnXnXydke94613582 = UgANJMEMUVosnXnXydke22243335;     UgANJMEMUVosnXnXydke22243335 = UgANJMEMUVosnXnXydke7830693;     UgANJMEMUVosnXnXydke7830693 = UgANJMEMUVosnXnXydke97028456;     UgANJMEMUVosnXnXydke97028456 = UgANJMEMUVosnXnXydke8438866;     UgANJMEMUVosnXnXydke8438866 = UgANJMEMUVosnXnXydke71927992;     UgANJMEMUVosnXnXydke71927992 = UgANJMEMUVosnXnXydke58649296;     UgANJMEMUVosnXnXydke58649296 = UgANJMEMUVosnXnXydke90149840;     UgANJMEMUVosnXnXydke90149840 = UgANJMEMUVosnXnXydke35143664;     UgANJMEMUVosnXnXydke35143664 = UgANJMEMUVosnXnXydke68371624;     UgANJMEMUVosnXnXydke68371624 = UgANJMEMUVosnXnXydke29227481;     UgANJMEMUVosnXnXydke29227481 = UgANJMEMUVosnXnXydke34366144;     UgANJMEMUVosnXnXydke34366144 = UgANJMEMUVosnXnXydke20258499;     UgANJMEMUVosnXnXydke20258499 = UgANJMEMUVosnXnXydke14685328;     UgANJMEMUVosnXnXydke14685328 = UgANJMEMUVosnXnXydke41661991;     UgANJMEMUVosnXnXydke41661991 = UgANJMEMUVosnXnXydke20384477;     UgANJMEMUVosnXnXydke20384477 = UgANJMEMUVosnXnXydke9600688;     UgANJMEMUVosnXnXydke9600688 = UgANJMEMUVosnXnXydke64146991;     UgANJMEMUVosnXnXydke64146991 = UgANJMEMUVosnXnXydke83225736;     UgANJMEMUVosnXnXydke83225736 = UgANJMEMUVosnXnXydke439735;     UgANJMEMUVosnXnXydke439735 = UgANJMEMUVosnXnXydke46833744;     UgANJMEMUVosnXnXydke46833744 = UgANJMEMUVosnXnXydke88941034;     UgANJMEMUVosnXnXydke88941034 = UgANJMEMUVosnXnXydke76412294;     UgANJMEMUVosnXnXydke76412294 = UgANJMEMUVosnXnXydke3392310;     UgANJMEMUVosnXnXydke3392310 = UgANJMEMUVosnXnXydke54257884;     UgANJMEMUVosnXnXydke54257884 = UgANJMEMUVosnXnXydke21584618;     UgANJMEMUVosnXnXydke21584618 = UgANJMEMUVosnXnXydke76371278;     UgANJMEMUVosnXnXydke76371278 = UgANJMEMUVosnXnXydke33247195;     UgANJMEMUVosnXnXydke33247195 = UgANJMEMUVosnXnXydke97435849;     UgANJMEMUVosnXnXydke97435849 = UgANJMEMUVosnXnXydke13751699;     UgANJMEMUVosnXnXydke13751699 = UgANJMEMUVosnXnXydke48607299;     UgANJMEMUVosnXnXydke48607299 = UgANJMEMUVosnXnXydke15731421;     UgANJMEMUVosnXnXydke15731421 = UgANJMEMUVosnXnXydke7898952;     UgANJMEMUVosnXnXydke7898952 = UgANJMEMUVosnXnXydke73283441;     UgANJMEMUVosnXnXydke73283441 = UgANJMEMUVosnXnXydke40120359;     UgANJMEMUVosnXnXydke40120359 = UgANJMEMUVosnXnXydke76238262;     UgANJMEMUVosnXnXydke76238262 = UgANJMEMUVosnXnXydke2682930;     UgANJMEMUVosnXnXydke2682930 = UgANJMEMUVosnXnXydke72980687;     UgANJMEMUVosnXnXydke72980687 = UgANJMEMUVosnXnXydke74145364;     UgANJMEMUVosnXnXydke74145364 = UgANJMEMUVosnXnXydke21040264;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BCPtvRXnGPcbpHricRhj6675882() {     float rZCgHRBtHWLFJTKYkqaJ21033792 = 50114464;    float rZCgHRBtHWLFJTKYkqaJ42426629 = -517232845;    float rZCgHRBtHWLFJTKYkqaJ79790245 = -576020864;    float rZCgHRBtHWLFJTKYkqaJ79150413 = -854651364;    float rZCgHRBtHWLFJTKYkqaJ54301278 = -668984618;    float rZCgHRBtHWLFJTKYkqaJ83615737 = 30530679;    float rZCgHRBtHWLFJTKYkqaJ84144659 = -203819941;    float rZCgHRBtHWLFJTKYkqaJ8911369 = -860064300;    float rZCgHRBtHWLFJTKYkqaJ89550965 = 18076444;    float rZCgHRBtHWLFJTKYkqaJ27811682 = -436479742;    float rZCgHRBtHWLFJTKYkqaJ56431044 = -192137503;    float rZCgHRBtHWLFJTKYkqaJ43796503 = -574396632;    float rZCgHRBtHWLFJTKYkqaJ83622952 = -278071431;    float rZCgHRBtHWLFJTKYkqaJ31572074 = -555189700;    float rZCgHRBtHWLFJTKYkqaJ18311953 = -542818614;    float rZCgHRBtHWLFJTKYkqaJ23289284 = -998980651;    float rZCgHRBtHWLFJTKYkqaJ55852247 = -757216549;    float rZCgHRBtHWLFJTKYkqaJ3877567 = -892930181;    float rZCgHRBtHWLFJTKYkqaJ91150771 = -469160056;    float rZCgHRBtHWLFJTKYkqaJ21493032 = -41548148;    float rZCgHRBtHWLFJTKYkqaJ19813013 = -855069657;    float rZCgHRBtHWLFJTKYkqaJ9194365 = -458868024;    float rZCgHRBtHWLFJTKYkqaJ16488229 = -79534727;    float rZCgHRBtHWLFJTKYkqaJ90381328 = -961066625;    float rZCgHRBtHWLFJTKYkqaJ67472655 = -91382231;    float rZCgHRBtHWLFJTKYkqaJ23423315 = -690798354;    float rZCgHRBtHWLFJTKYkqaJ41851325 = -52343044;    float rZCgHRBtHWLFJTKYkqaJ56387266 = -217827883;    float rZCgHRBtHWLFJTKYkqaJ30132430 = -29344681;    float rZCgHRBtHWLFJTKYkqaJ75069978 = -955216726;    float rZCgHRBtHWLFJTKYkqaJ67556390 = -667432214;    float rZCgHRBtHWLFJTKYkqaJ12395431 = -97104752;    float rZCgHRBtHWLFJTKYkqaJ43029257 = -799124214;    float rZCgHRBtHWLFJTKYkqaJ93403580 = -986663832;    float rZCgHRBtHWLFJTKYkqaJ69936052 = -166469430;    float rZCgHRBtHWLFJTKYkqaJ80824369 = -500290948;    float rZCgHRBtHWLFJTKYkqaJ92294480 = -965589947;    float rZCgHRBtHWLFJTKYkqaJ40689687 = 17199235;    float rZCgHRBtHWLFJTKYkqaJ81824486 = -571516878;    float rZCgHRBtHWLFJTKYkqaJ22336896 = -202079767;    float rZCgHRBtHWLFJTKYkqaJ22589609 = -991823706;    float rZCgHRBtHWLFJTKYkqaJ84179258 = -800474300;    float rZCgHRBtHWLFJTKYkqaJ70796983 = -350768713;    float rZCgHRBtHWLFJTKYkqaJ29241042 = -391572435;    float rZCgHRBtHWLFJTKYkqaJ66602281 = -68799629;    float rZCgHRBtHWLFJTKYkqaJ96232197 = -821857718;    float rZCgHRBtHWLFJTKYkqaJ34107296 = -590187377;    float rZCgHRBtHWLFJTKYkqaJ179581 = -25679085;    float rZCgHRBtHWLFJTKYkqaJ50701096 = -896769628;    float rZCgHRBtHWLFJTKYkqaJ61087111 = -114722036;    float rZCgHRBtHWLFJTKYkqaJ8847169 = -199629075;    float rZCgHRBtHWLFJTKYkqaJ94433085 = 52333695;    float rZCgHRBtHWLFJTKYkqaJ81535303 = -911363991;    float rZCgHRBtHWLFJTKYkqaJ91455826 = -99608594;    float rZCgHRBtHWLFJTKYkqaJ86862869 = -896696994;    float rZCgHRBtHWLFJTKYkqaJ11839428 = -491017512;    float rZCgHRBtHWLFJTKYkqaJ25938401 = -337698118;    float rZCgHRBtHWLFJTKYkqaJ89408917 = -614954240;    float rZCgHRBtHWLFJTKYkqaJ11677758 = -663269134;    float rZCgHRBtHWLFJTKYkqaJ30877963 = -978186265;    float rZCgHRBtHWLFJTKYkqaJ41764413 = -917126278;    float rZCgHRBtHWLFJTKYkqaJ27757393 = -985992058;    float rZCgHRBtHWLFJTKYkqaJ78778938 = -730719620;    float rZCgHRBtHWLFJTKYkqaJ14480987 = -26706831;    float rZCgHRBtHWLFJTKYkqaJ60255291 = -769047528;    float rZCgHRBtHWLFJTKYkqaJ44035613 = 4967248;    float rZCgHRBtHWLFJTKYkqaJ767247 = -775272419;    float rZCgHRBtHWLFJTKYkqaJ90219372 = -291407599;    float rZCgHRBtHWLFJTKYkqaJ61636022 = -288720271;    float rZCgHRBtHWLFJTKYkqaJ37487584 = 57472334;    float rZCgHRBtHWLFJTKYkqaJ30994804 = 66609296;    float rZCgHRBtHWLFJTKYkqaJ15162561 = -674415785;    float rZCgHRBtHWLFJTKYkqaJ22053080 = -221413304;    float rZCgHRBtHWLFJTKYkqaJ68813876 = -167080290;    float rZCgHRBtHWLFJTKYkqaJ98903422 = -49724443;    float rZCgHRBtHWLFJTKYkqaJ35633755 = 45404642;    float rZCgHRBtHWLFJTKYkqaJ38397382 = -8099311;    float rZCgHRBtHWLFJTKYkqaJ87247186 = -687962293;    float rZCgHRBtHWLFJTKYkqaJ23779047 = -792266997;    float rZCgHRBtHWLFJTKYkqaJ71240458 = -269524514;    float rZCgHRBtHWLFJTKYkqaJ89316018 = -610977;    float rZCgHRBtHWLFJTKYkqaJ41671745 = 73336040;    float rZCgHRBtHWLFJTKYkqaJ5686171 = -321058256;    float rZCgHRBtHWLFJTKYkqaJ69045318 = -914622646;    float rZCgHRBtHWLFJTKYkqaJ66222810 = -655587651;    float rZCgHRBtHWLFJTKYkqaJ73123305 = -619765910;    float rZCgHRBtHWLFJTKYkqaJ30860128 = -185740762;    float rZCgHRBtHWLFJTKYkqaJ51573430 = -599515621;    float rZCgHRBtHWLFJTKYkqaJ6540712 = 10033161;    float rZCgHRBtHWLFJTKYkqaJ58096624 = -675451919;    float rZCgHRBtHWLFJTKYkqaJ54885968 = -62592831;    float rZCgHRBtHWLFJTKYkqaJ2885563 = -250635708;    float rZCgHRBtHWLFJTKYkqaJ29011929 = -319531632;    float rZCgHRBtHWLFJTKYkqaJ50946524 = -593330614;    float rZCgHRBtHWLFJTKYkqaJ80572483 = -284953490;    float rZCgHRBtHWLFJTKYkqaJ94832216 = 94168351;    float rZCgHRBtHWLFJTKYkqaJ5400320 = 30245319;    float rZCgHRBtHWLFJTKYkqaJ56315996 = -224061883;    float rZCgHRBtHWLFJTKYkqaJ68985751 = -622524908;    float rZCgHRBtHWLFJTKYkqaJ22566668 = 50114464;     rZCgHRBtHWLFJTKYkqaJ21033792 = rZCgHRBtHWLFJTKYkqaJ42426629;     rZCgHRBtHWLFJTKYkqaJ42426629 = rZCgHRBtHWLFJTKYkqaJ79790245;     rZCgHRBtHWLFJTKYkqaJ79790245 = rZCgHRBtHWLFJTKYkqaJ79150413;     rZCgHRBtHWLFJTKYkqaJ79150413 = rZCgHRBtHWLFJTKYkqaJ54301278;     rZCgHRBtHWLFJTKYkqaJ54301278 = rZCgHRBtHWLFJTKYkqaJ83615737;     rZCgHRBtHWLFJTKYkqaJ83615737 = rZCgHRBtHWLFJTKYkqaJ84144659;     rZCgHRBtHWLFJTKYkqaJ84144659 = rZCgHRBtHWLFJTKYkqaJ8911369;     rZCgHRBtHWLFJTKYkqaJ8911369 = rZCgHRBtHWLFJTKYkqaJ89550965;     rZCgHRBtHWLFJTKYkqaJ89550965 = rZCgHRBtHWLFJTKYkqaJ27811682;     rZCgHRBtHWLFJTKYkqaJ27811682 = rZCgHRBtHWLFJTKYkqaJ56431044;     rZCgHRBtHWLFJTKYkqaJ56431044 = rZCgHRBtHWLFJTKYkqaJ43796503;     rZCgHRBtHWLFJTKYkqaJ43796503 = rZCgHRBtHWLFJTKYkqaJ83622952;     rZCgHRBtHWLFJTKYkqaJ83622952 = rZCgHRBtHWLFJTKYkqaJ31572074;     rZCgHRBtHWLFJTKYkqaJ31572074 = rZCgHRBtHWLFJTKYkqaJ18311953;     rZCgHRBtHWLFJTKYkqaJ18311953 = rZCgHRBtHWLFJTKYkqaJ23289284;     rZCgHRBtHWLFJTKYkqaJ23289284 = rZCgHRBtHWLFJTKYkqaJ55852247;     rZCgHRBtHWLFJTKYkqaJ55852247 = rZCgHRBtHWLFJTKYkqaJ3877567;     rZCgHRBtHWLFJTKYkqaJ3877567 = rZCgHRBtHWLFJTKYkqaJ91150771;     rZCgHRBtHWLFJTKYkqaJ91150771 = rZCgHRBtHWLFJTKYkqaJ21493032;     rZCgHRBtHWLFJTKYkqaJ21493032 = rZCgHRBtHWLFJTKYkqaJ19813013;     rZCgHRBtHWLFJTKYkqaJ19813013 = rZCgHRBtHWLFJTKYkqaJ9194365;     rZCgHRBtHWLFJTKYkqaJ9194365 = rZCgHRBtHWLFJTKYkqaJ16488229;     rZCgHRBtHWLFJTKYkqaJ16488229 = rZCgHRBtHWLFJTKYkqaJ90381328;     rZCgHRBtHWLFJTKYkqaJ90381328 = rZCgHRBtHWLFJTKYkqaJ67472655;     rZCgHRBtHWLFJTKYkqaJ67472655 = rZCgHRBtHWLFJTKYkqaJ23423315;     rZCgHRBtHWLFJTKYkqaJ23423315 = rZCgHRBtHWLFJTKYkqaJ41851325;     rZCgHRBtHWLFJTKYkqaJ41851325 = rZCgHRBtHWLFJTKYkqaJ56387266;     rZCgHRBtHWLFJTKYkqaJ56387266 = rZCgHRBtHWLFJTKYkqaJ30132430;     rZCgHRBtHWLFJTKYkqaJ30132430 = rZCgHRBtHWLFJTKYkqaJ75069978;     rZCgHRBtHWLFJTKYkqaJ75069978 = rZCgHRBtHWLFJTKYkqaJ67556390;     rZCgHRBtHWLFJTKYkqaJ67556390 = rZCgHRBtHWLFJTKYkqaJ12395431;     rZCgHRBtHWLFJTKYkqaJ12395431 = rZCgHRBtHWLFJTKYkqaJ43029257;     rZCgHRBtHWLFJTKYkqaJ43029257 = rZCgHRBtHWLFJTKYkqaJ93403580;     rZCgHRBtHWLFJTKYkqaJ93403580 = rZCgHRBtHWLFJTKYkqaJ69936052;     rZCgHRBtHWLFJTKYkqaJ69936052 = rZCgHRBtHWLFJTKYkqaJ80824369;     rZCgHRBtHWLFJTKYkqaJ80824369 = rZCgHRBtHWLFJTKYkqaJ92294480;     rZCgHRBtHWLFJTKYkqaJ92294480 = rZCgHRBtHWLFJTKYkqaJ40689687;     rZCgHRBtHWLFJTKYkqaJ40689687 = rZCgHRBtHWLFJTKYkqaJ81824486;     rZCgHRBtHWLFJTKYkqaJ81824486 = rZCgHRBtHWLFJTKYkqaJ22336896;     rZCgHRBtHWLFJTKYkqaJ22336896 = rZCgHRBtHWLFJTKYkqaJ22589609;     rZCgHRBtHWLFJTKYkqaJ22589609 = rZCgHRBtHWLFJTKYkqaJ84179258;     rZCgHRBtHWLFJTKYkqaJ84179258 = rZCgHRBtHWLFJTKYkqaJ70796983;     rZCgHRBtHWLFJTKYkqaJ70796983 = rZCgHRBtHWLFJTKYkqaJ29241042;     rZCgHRBtHWLFJTKYkqaJ29241042 = rZCgHRBtHWLFJTKYkqaJ66602281;     rZCgHRBtHWLFJTKYkqaJ66602281 = rZCgHRBtHWLFJTKYkqaJ96232197;     rZCgHRBtHWLFJTKYkqaJ96232197 = rZCgHRBtHWLFJTKYkqaJ34107296;     rZCgHRBtHWLFJTKYkqaJ34107296 = rZCgHRBtHWLFJTKYkqaJ179581;     rZCgHRBtHWLFJTKYkqaJ179581 = rZCgHRBtHWLFJTKYkqaJ50701096;     rZCgHRBtHWLFJTKYkqaJ50701096 = rZCgHRBtHWLFJTKYkqaJ61087111;     rZCgHRBtHWLFJTKYkqaJ61087111 = rZCgHRBtHWLFJTKYkqaJ8847169;     rZCgHRBtHWLFJTKYkqaJ8847169 = rZCgHRBtHWLFJTKYkqaJ94433085;     rZCgHRBtHWLFJTKYkqaJ94433085 = rZCgHRBtHWLFJTKYkqaJ81535303;     rZCgHRBtHWLFJTKYkqaJ81535303 = rZCgHRBtHWLFJTKYkqaJ91455826;     rZCgHRBtHWLFJTKYkqaJ91455826 = rZCgHRBtHWLFJTKYkqaJ86862869;     rZCgHRBtHWLFJTKYkqaJ86862869 = rZCgHRBtHWLFJTKYkqaJ11839428;     rZCgHRBtHWLFJTKYkqaJ11839428 = rZCgHRBtHWLFJTKYkqaJ25938401;     rZCgHRBtHWLFJTKYkqaJ25938401 = rZCgHRBtHWLFJTKYkqaJ89408917;     rZCgHRBtHWLFJTKYkqaJ89408917 = rZCgHRBtHWLFJTKYkqaJ11677758;     rZCgHRBtHWLFJTKYkqaJ11677758 = rZCgHRBtHWLFJTKYkqaJ30877963;     rZCgHRBtHWLFJTKYkqaJ30877963 = rZCgHRBtHWLFJTKYkqaJ41764413;     rZCgHRBtHWLFJTKYkqaJ41764413 = rZCgHRBtHWLFJTKYkqaJ27757393;     rZCgHRBtHWLFJTKYkqaJ27757393 = rZCgHRBtHWLFJTKYkqaJ78778938;     rZCgHRBtHWLFJTKYkqaJ78778938 = rZCgHRBtHWLFJTKYkqaJ14480987;     rZCgHRBtHWLFJTKYkqaJ14480987 = rZCgHRBtHWLFJTKYkqaJ60255291;     rZCgHRBtHWLFJTKYkqaJ60255291 = rZCgHRBtHWLFJTKYkqaJ44035613;     rZCgHRBtHWLFJTKYkqaJ44035613 = rZCgHRBtHWLFJTKYkqaJ767247;     rZCgHRBtHWLFJTKYkqaJ767247 = rZCgHRBtHWLFJTKYkqaJ90219372;     rZCgHRBtHWLFJTKYkqaJ90219372 = rZCgHRBtHWLFJTKYkqaJ61636022;     rZCgHRBtHWLFJTKYkqaJ61636022 = rZCgHRBtHWLFJTKYkqaJ37487584;     rZCgHRBtHWLFJTKYkqaJ37487584 = rZCgHRBtHWLFJTKYkqaJ30994804;     rZCgHRBtHWLFJTKYkqaJ30994804 = rZCgHRBtHWLFJTKYkqaJ15162561;     rZCgHRBtHWLFJTKYkqaJ15162561 = rZCgHRBtHWLFJTKYkqaJ22053080;     rZCgHRBtHWLFJTKYkqaJ22053080 = rZCgHRBtHWLFJTKYkqaJ68813876;     rZCgHRBtHWLFJTKYkqaJ68813876 = rZCgHRBtHWLFJTKYkqaJ98903422;     rZCgHRBtHWLFJTKYkqaJ98903422 = rZCgHRBtHWLFJTKYkqaJ35633755;     rZCgHRBtHWLFJTKYkqaJ35633755 = rZCgHRBtHWLFJTKYkqaJ38397382;     rZCgHRBtHWLFJTKYkqaJ38397382 = rZCgHRBtHWLFJTKYkqaJ87247186;     rZCgHRBtHWLFJTKYkqaJ87247186 = rZCgHRBtHWLFJTKYkqaJ23779047;     rZCgHRBtHWLFJTKYkqaJ23779047 = rZCgHRBtHWLFJTKYkqaJ71240458;     rZCgHRBtHWLFJTKYkqaJ71240458 = rZCgHRBtHWLFJTKYkqaJ89316018;     rZCgHRBtHWLFJTKYkqaJ89316018 = rZCgHRBtHWLFJTKYkqaJ41671745;     rZCgHRBtHWLFJTKYkqaJ41671745 = rZCgHRBtHWLFJTKYkqaJ5686171;     rZCgHRBtHWLFJTKYkqaJ5686171 = rZCgHRBtHWLFJTKYkqaJ69045318;     rZCgHRBtHWLFJTKYkqaJ69045318 = rZCgHRBtHWLFJTKYkqaJ66222810;     rZCgHRBtHWLFJTKYkqaJ66222810 = rZCgHRBtHWLFJTKYkqaJ73123305;     rZCgHRBtHWLFJTKYkqaJ73123305 = rZCgHRBtHWLFJTKYkqaJ30860128;     rZCgHRBtHWLFJTKYkqaJ30860128 = rZCgHRBtHWLFJTKYkqaJ51573430;     rZCgHRBtHWLFJTKYkqaJ51573430 = rZCgHRBtHWLFJTKYkqaJ6540712;     rZCgHRBtHWLFJTKYkqaJ6540712 = rZCgHRBtHWLFJTKYkqaJ58096624;     rZCgHRBtHWLFJTKYkqaJ58096624 = rZCgHRBtHWLFJTKYkqaJ54885968;     rZCgHRBtHWLFJTKYkqaJ54885968 = rZCgHRBtHWLFJTKYkqaJ2885563;     rZCgHRBtHWLFJTKYkqaJ2885563 = rZCgHRBtHWLFJTKYkqaJ29011929;     rZCgHRBtHWLFJTKYkqaJ29011929 = rZCgHRBtHWLFJTKYkqaJ50946524;     rZCgHRBtHWLFJTKYkqaJ50946524 = rZCgHRBtHWLFJTKYkqaJ80572483;     rZCgHRBtHWLFJTKYkqaJ80572483 = rZCgHRBtHWLFJTKYkqaJ94832216;     rZCgHRBtHWLFJTKYkqaJ94832216 = rZCgHRBtHWLFJTKYkqaJ5400320;     rZCgHRBtHWLFJTKYkqaJ5400320 = rZCgHRBtHWLFJTKYkqaJ56315996;     rZCgHRBtHWLFJTKYkqaJ56315996 = rZCgHRBtHWLFJTKYkqaJ68985751;     rZCgHRBtHWLFJTKYkqaJ68985751 = rZCgHRBtHWLFJTKYkqaJ22566668;     rZCgHRBtHWLFJTKYkqaJ22566668 = rZCgHRBtHWLFJTKYkqaJ21033792;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LOOPBIvUQEwKaccKPTPJ61092095() {     float qcMeGTcNHEiSJvNxyYTR91685459 = -568034666;    float qcMeGTcNHEiSJvNxyYTR11100147 = -388876144;    float qcMeGTcNHEiSJvNxyYTR59759164 = 81684676;    float qcMeGTcNHEiSJvNxyYTR19081700 = 55210247;    float qcMeGTcNHEiSJvNxyYTR75751599 = -452390966;    float qcMeGTcNHEiSJvNxyYTR73615500 = -771050473;    float qcMeGTcNHEiSJvNxyYTR2523555 = -202595651;    float qcMeGTcNHEiSJvNxyYTR65753749 = -106881390;    float qcMeGTcNHEiSJvNxyYTR35416020 = -75270885;    float qcMeGTcNHEiSJvNxyYTR50778142 = -737575092;    float qcMeGTcNHEiSJvNxyYTR98680204 = -227039433;    float qcMeGTcNHEiSJvNxyYTR89343074 = -890160425;    float qcMeGTcNHEiSJvNxyYTR63450340 = -187608489;    float qcMeGTcNHEiSJvNxyYTR15419108 = -356217910;    float qcMeGTcNHEiSJvNxyYTR1031973 = -692568617;    float qcMeGTcNHEiSJvNxyYTR55817924 = -568964237;    float qcMeGTcNHEiSJvNxyYTR49705074 = -666401679;    float qcMeGTcNHEiSJvNxyYTR24171061 = -882096377;    float qcMeGTcNHEiSJvNxyYTR72858553 = -76347998;    float qcMeGTcNHEiSJvNxyYTR80279741 = -583914453;    float qcMeGTcNHEiSJvNxyYTR74948767 = -763748798;    float qcMeGTcNHEiSJvNxyYTR70590129 = -790153330;    float qcMeGTcNHEiSJvNxyYTR18432037 = -647377872;    float qcMeGTcNHEiSJvNxyYTR24230866 = -553467531;    float qcMeGTcNHEiSJvNxyYTR31597044 = -867286919;    float qcMeGTcNHEiSJvNxyYTR54020895 = -789082799;    float qcMeGTcNHEiSJvNxyYTR50112110 = -447520820;    float qcMeGTcNHEiSJvNxyYTR64169491 = -722951651;    float qcMeGTcNHEiSJvNxyYTR62274917 = -323702898;    float qcMeGTcNHEiSJvNxyYTR14242075 = -413611796;    float qcMeGTcNHEiSJvNxyYTR74628386 = -415865900;    float qcMeGTcNHEiSJvNxyYTR56906891 = -425401915;    float qcMeGTcNHEiSJvNxyYTR25974618 = -746085823;    float qcMeGTcNHEiSJvNxyYTR58768555 = -550193517;    float qcMeGTcNHEiSJvNxyYTR47960288 = -13416210;    float qcMeGTcNHEiSJvNxyYTR40817629 = -579517496;    float qcMeGTcNHEiSJvNxyYTR36085966 = -54183155;    float qcMeGTcNHEiSJvNxyYTR93482175 = -866913872;    float qcMeGTcNHEiSJvNxyYTR60357775 = -527245515;    float qcMeGTcNHEiSJvNxyYTR17440194 = -819688130;    float qcMeGTcNHEiSJvNxyYTR82662893 = -275842584;    float qcMeGTcNHEiSJvNxyYTR12662249 = -569863370;    float qcMeGTcNHEiSJvNxyYTR19763943 = -337522765;    float qcMeGTcNHEiSJvNxyYTR35749343 = -463547407;    float qcMeGTcNHEiSJvNxyYTR62556340 = -377446023;    float qcMeGTcNHEiSJvNxyYTR2638033 = -580194873;    float qcMeGTcNHEiSJvNxyYTR60238499 = -706257177;    float qcMeGTcNHEiSJvNxyYTR65793790 = -477809380;    float qcMeGTcNHEiSJvNxyYTR56142548 = -882533597;    float qcMeGTcNHEiSJvNxyYTR69514597 = -397730914;    float qcMeGTcNHEiSJvNxyYTR77036492 = -846201197;    float qcMeGTcNHEiSJvNxyYTR36304590 = 83113973;    float qcMeGTcNHEiSJvNxyYTR36610960 = -839162033;    float qcMeGTcNHEiSJvNxyYTR81683131 = -760490091;    float qcMeGTcNHEiSJvNxyYTR86143327 = 92362092;    float qcMeGTcNHEiSJvNxyYTR21095330 = -777881337;    float qcMeGTcNHEiSJvNxyYTR92668110 = -741498273;    float qcMeGTcNHEiSJvNxyYTR35528299 = -364847793;    float qcMeGTcNHEiSJvNxyYTR87484656 = -77502835;    float qcMeGTcNHEiSJvNxyYTR21730704 = -663308167;    float qcMeGTcNHEiSJvNxyYTR23503390 = -223529654;    float qcMeGTcNHEiSJvNxyYTR38354063 = -479644000;    float qcMeGTcNHEiSJvNxyYTR3478832 = -783178493;    float qcMeGTcNHEiSJvNxyYTR21173946 = -661659089;    float qcMeGTcNHEiSJvNxyYTR76149755 = -221709193;    float qcMeGTcNHEiSJvNxyYTR41773313 = -801637518;    float qcMeGTcNHEiSJvNxyYTR63368456 = -44074603;    float qcMeGTcNHEiSJvNxyYTR4681785 = -637414973;    float qcMeGTcNHEiSJvNxyYTR67458819 = -242801701;    float qcMeGTcNHEiSJvNxyYTR60214344 = -13051121;    float qcMeGTcNHEiSJvNxyYTR19731958 = -414781083;    float qcMeGTcNHEiSJvNxyYTR56222899 = -799487807;    float qcMeGTcNHEiSJvNxyYTR63813285 = -254850863;    float qcMeGTcNHEiSJvNxyYTR55418359 = -256659869;    float qcMeGTcNHEiSJvNxyYTR97616848 = -208071869;    float qcMeGTcNHEiSJvNxyYTR62286518 = -93885429;    float qcMeGTcNHEiSJvNxyYTR50826187 = -352630565;    float qcMeGTcNHEiSJvNxyYTR82682694 = -83830466;    float qcMeGTcNHEiSJvNxyYTR61674525 = -76021508;    float qcMeGTcNHEiSJvNxyYTR28959012 = -187092047;    float qcMeGTcNHEiSJvNxyYTR93782396 = 17174377;    float qcMeGTcNHEiSJvNxyYTR84318320 = -969711440;    float qcMeGTcNHEiSJvNxyYTR8026944 = -840418054;    float qcMeGTcNHEiSJvNxyYTR92760320 = -925971984;    float qcMeGTcNHEiSJvNxyYTR37205582 = -567410600;    float qcMeGTcNHEiSJvNxyYTR38323797 = -398979873;    float qcMeGTcNHEiSJvNxyYTR20295931 = -586239882;    float qcMeGTcNHEiSJvNxyYTR44291486 = -985595733;    float qcMeGTcNHEiSJvNxyYTR72625227 = -542555609;    float qcMeGTcNHEiSJvNxyYTR26864959 = -235534873;    float qcMeGTcNHEiSJvNxyYTR48149518 = -838019224;    float qcMeGTcNHEiSJvNxyYTR557667 = -689335362;    float qcMeGTcNHEiSJvNxyYTR5997520 = -689411038;    float qcMeGTcNHEiSJvNxyYTR38627072 = -863937348;    float qcMeGTcNHEiSJvNxyYTR93936804 = -496158476;    float qcMeGTcNHEiSJvNxyYTR44308830 = -796198585;    float qcMeGTcNHEiSJvNxyYTR9183418 = -786684877;    float qcMeGTcNHEiSJvNxyYTR98589997 = -675863677;    float qcMeGTcNHEiSJvNxyYTR59599587 = -141838214;    float qcMeGTcNHEiSJvNxyYTR20783027 = -568034666;     qcMeGTcNHEiSJvNxyYTR91685459 = qcMeGTcNHEiSJvNxyYTR11100147;     qcMeGTcNHEiSJvNxyYTR11100147 = qcMeGTcNHEiSJvNxyYTR59759164;     qcMeGTcNHEiSJvNxyYTR59759164 = qcMeGTcNHEiSJvNxyYTR19081700;     qcMeGTcNHEiSJvNxyYTR19081700 = qcMeGTcNHEiSJvNxyYTR75751599;     qcMeGTcNHEiSJvNxyYTR75751599 = qcMeGTcNHEiSJvNxyYTR73615500;     qcMeGTcNHEiSJvNxyYTR73615500 = qcMeGTcNHEiSJvNxyYTR2523555;     qcMeGTcNHEiSJvNxyYTR2523555 = qcMeGTcNHEiSJvNxyYTR65753749;     qcMeGTcNHEiSJvNxyYTR65753749 = qcMeGTcNHEiSJvNxyYTR35416020;     qcMeGTcNHEiSJvNxyYTR35416020 = qcMeGTcNHEiSJvNxyYTR50778142;     qcMeGTcNHEiSJvNxyYTR50778142 = qcMeGTcNHEiSJvNxyYTR98680204;     qcMeGTcNHEiSJvNxyYTR98680204 = qcMeGTcNHEiSJvNxyYTR89343074;     qcMeGTcNHEiSJvNxyYTR89343074 = qcMeGTcNHEiSJvNxyYTR63450340;     qcMeGTcNHEiSJvNxyYTR63450340 = qcMeGTcNHEiSJvNxyYTR15419108;     qcMeGTcNHEiSJvNxyYTR15419108 = qcMeGTcNHEiSJvNxyYTR1031973;     qcMeGTcNHEiSJvNxyYTR1031973 = qcMeGTcNHEiSJvNxyYTR55817924;     qcMeGTcNHEiSJvNxyYTR55817924 = qcMeGTcNHEiSJvNxyYTR49705074;     qcMeGTcNHEiSJvNxyYTR49705074 = qcMeGTcNHEiSJvNxyYTR24171061;     qcMeGTcNHEiSJvNxyYTR24171061 = qcMeGTcNHEiSJvNxyYTR72858553;     qcMeGTcNHEiSJvNxyYTR72858553 = qcMeGTcNHEiSJvNxyYTR80279741;     qcMeGTcNHEiSJvNxyYTR80279741 = qcMeGTcNHEiSJvNxyYTR74948767;     qcMeGTcNHEiSJvNxyYTR74948767 = qcMeGTcNHEiSJvNxyYTR70590129;     qcMeGTcNHEiSJvNxyYTR70590129 = qcMeGTcNHEiSJvNxyYTR18432037;     qcMeGTcNHEiSJvNxyYTR18432037 = qcMeGTcNHEiSJvNxyYTR24230866;     qcMeGTcNHEiSJvNxyYTR24230866 = qcMeGTcNHEiSJvNxyYTR31597044;     qcMeGTcNHEiSJvNxyYTR31597044 = qcMeGTcNHEiSJvNxyYTR54020895;     qcMeGTcNHEiSJvNxyYTR54020895 = qcMeGTcNHEiSJvNxyYTR50112110;     qcMeGTcNHEiSJvNxyYTR50112110 = qcMeGTcNHEiSJvNxyYTR64169491;     qcMeGTcNHEiSJvNxyYTR64169491 = qcMeGTcNHEiSJvNxyYTR62274917;     qcMeGTcNHEiSJvNxyYTR62274917 = qcMeGTcNHEiSJvNxyYTR14242075;     qcMeGTcNHEiSJvNxyYTR14242075 = qcMeGTcNHEiSJvNxyYTR74628386;     qcMeGTcNHEiSJvNxyYTR74628386 = qcMeGTcNHEiSJvNxyYTR56906891;     qcMeGTcNHEiSJvNxyYTR56906891 = qcMeGTcNHEiSJvNxyYTR25974618;     qcMeGTcNHEiSJvNxyYTR25974618 = qcMeGTcNHEiSJvNxyYTR58768555;     qcMeGTcNHEiSJvNxyYTR58768555 = qcMeGTcNHEiSJvNxyYTR47960288;     qcMeGTcNHEiSJvNxyYTR47960288 = qcMeGTcNHEiSJvNxyYTR40817629;     qcMeGTcNHEiSJvNxyYTR40817629 = qcMeGTcNHEiSJvNxyYTR36085966;     qcMeGTcNHEiSJvNxyYTR36085966 = qcMeGTcNHEiSJvNxyYTR93482175;     qcMeGTcNHEiSJvNxyYTR93482175 = qcMeGTcNHEiSJvNxyYTR60357775;     qcMeGTcNHEiSJvNxyYTR60357775 = qcMeGTcNHEiSJvNxyYTR17440194;     qcMeGTcNHEiSJvNxyYTR17440194 = qcMeGTcNHEiSJvNxyYTR82662893;     qcMeGTcNHEiSJvNxyYTR82662893 = qcMeGTcNHEiSJvNxyYTR12662249;     qcMeGTcNHEiSJvNxyYTR12662249 = qcMeGTcNHEiSJvNxyYTR19763943;     qcMeGTcNHEiSJvNxyYTR19763943 = qcMeGTcNHEiSJvNxyYTR35749343;     qcMeGTcNHEiSJvNxyYTR35749343 = qcMeGTcNHEiSJvNxyYTR62556340;     qcMeGTcNHEiSJvNxyYTR62556340 = qcMeGTcNHEiSJvNxyYTR2638033;     qcMeGTcNHEiSJvNxyYTR2638033 = qcMeGTcNHEiSJvNxyYTR60238499;     qcMeGTcNHEiSJvNxyYTR60238499 = qcMeGTcNHEiSJvNxyYTR65793790;     qcMeGTcNHEiSJvNxyYTR65793790 = qcMeGTcNHEiSJvNxyYTR56142548;     qcMeGTcNHEiSJvNxyYTR56142548 = qcMeGTcNHEiSJvNxyYTR69514597;     qcMeGTcNHEiSJvNxyYTR69514597 = qcMeGTcNHEiSJvNxyYTR77036492;     qcMeGTcNHEiSJvNxyYTR77036492 = qcMeGTcNHEiSJvNxyYTR36304590;     qcMeGTcNHEiSJvNxyYTR36304590 = qcMeGTcNHEiSJvNxyYTR36610960;     qcMeGTcNHEiSJvNxyYTR36610960 = qcMeGTcNHEiSJvNxyYTR81683131;     qcMeGTcNHEiSJvNxyYTR81683131 = qcMeGTcNHEiSJvNxyYTR86143327;     qcMeGTcNHEiSJvNxyYTR86143327 = qcMeGTcNHEiSJvNxyYTR21095330;     qcMeGTcNHEiSJvNxyYTR21095330 = qcMeGTcNHEiSJvNxyYTR92668110;     qcMeGTcNHEiSJvNxyYTR92668110 = qcMeGTcNHEiSJvNxyYTR35528299;     qcMeGTcNHEiSJvNxyYTR35528299 = qcMeGTcNHEiSJvNxyYTR87484656;     qcMeGTcNHEiSJvNxyYTR87484656 = qcMeGTcNHEiSJvNxyYTR21730704;     qcMeGTcNHEiSJvNxyYTR21730704 = qcMeGTcNHEiSJvNxyYTR23503390;     qcMeGTcNHEiSJvNxyYTR23503390 = qcMeGTcNHEiSJvNxyYTR38354063;     qcMeGTcNHEiSJvNxyYTR38354063 = qcMeGTcNHEiSJvNxyYTR3478832;     qcMeGTcNHEiSJvNxyYTR3478832 = qcMeGTcNHEiSJvNxyYTR21173946;     qcMeGTcNHEiSJvNxyYTR21173946 = qcMeGTcNHEiSJvNxyYTR76149755;     qcMeGTcNHEiSJvNxyYTR76149755 = qcMeGTcNHEiSJvNxyYTR41773313;     qcMeGTcNHEiSJvNxyYTR41773313 = qcMeGTcNHEiSJvNxyYTR63368456;     qcMeGTcNHEiSJvNxyYTR63368456 = qcMeGTcNHEiSJvNxyYTR4681785;     qcMeGTcNHEiSJvNxyYTR4681785 = qcMeGTcNHEiSJvNxyYTR67458819;     qcMeGTcNHEiSJvNxyYTR67458819 = qcMeGTcNHEiSJvNxyYTR60214344;     qcMeGTcNHEiSJvNxyYTR60214344 = qcMeGTcNHEiSJvNxyYTR19731958;     qcMeGTcNHEiSJvNxyYTR19731958 = qcMeGTcNHEiSJvNxyYTR56222899;     qcMeGTcNHEiSJvNxyYTR56222899 = qcMeGTcNHEiSJvNxyYTR63813285;     qcMeGTcNHEiSJvNxyYTR63813285 = qcMeGTcNHEiSJvNxyYTR55418359;     qcMeGTcNHEiSJvNxyYTR55418359 = qcMeGTcNHEiSJvNxyYTR97616848;     qcMeGTcNHEiSJvNxyYTR97616848 = qcMeGTcNHEiSJvNxyYTR62286518;     qcMeGTcNHEiSJvNxyYTR62286518 = qcMeGTcNHEiSJvNxyYTR50826187;     qcMeGTcNHEiSJvNxyYTR50826187 = qcMeGTcNHEiSJvNxyYTR82682694;     qcMeGTcNHEiSJvNxyYTR82682694 = qcMeGTcNHEiSJvNxyYTR61674525;     qcMeGTcNHEiSJvNxyYTR61674525 = qcMeGTcNHEiSJvNxyYTR28959012;     qcMeGTcNHEiSJvNxyYTR28959012 = qcMeGTcNHEiSJvNxyYTR93782396;     qcMeGTcNHEiSJvNxyYTR93782396 = qcMeGTcNHEiSJvNxyYTR84318320;     qcMeGTcNHEiSJvNxyYTR84318320 = qcMeGTcNHEiSJvNxyYTR8026944;     qcMeGTcNHEiSJvNxyYTR8026944 = qcMeGTcNHEiSJvNxyYTR92760320;     qcMeGTcNHEiSJvNxyYTR92760320 = qcMeGTcNHEiSJvNxyYTR37205582;     qcMeGTcNHEiSJvNxyYTR37205582 = qcMeGTcNHEiSJvNxyYTR38323797;     qcMeGTcNHEiSJvNxyYTR38323797 = qcMeGTcNHEiSJvNxyYTR20295931;     qcMeGTcNHEiSJvNxyYTR20295931 = qcMeGTcNHEiSJvNxyYTR44291486;     qcMeGTcNHEiSJvNxyYTR44291486 = qcMeGTcNHEiSJvNxyYTR72625227;     qcMeGTcNHEiSJvNxyYTR72625227 = qcMeGTcNHEiSJvNxyYTR26864959;     qcMeGTcNHEiSJvNxyYTR26864959 = qcMeGTcNHEiSJvNxyYTR48149518;     qcMeGTcNHEiSJvNxyYTR48149518 = qcMeGTcNHEiSJvNxyYTR557667;     qcMeGTcNHEiSJvNxyYTR557667 = qcMeGTcNHEiSJvNxyYTR5997520;     qcMeGTcNHEiSJvNxyYTR5997520 = qcMeGTcNHEiSJvNxyYTR38627072;     qcMeGTcNHEiSJvNxyYTR38627072 = qcMeGTcNHEiSJvNxyYTR93936804;     qcMeGTcNHEiSJvNxyYTR93936804 = qcMeGTcNHEiSJvNxyYTR44308830;     qcMeGTcNHEiSJvNxyYTR44308830 = qcMeGTcNHEiSJvNxyYTR9183418;     qcMeGTcNHEiSJvNxyYTR9183418 = qcMeGTcNHEiSJvNxyYTR98589997;     qcMeGTcNHEiSJvNxyYTR98589997 = qcMeGTcNHEiSJvNxyYTR59599587;     qcMeGTcNHEiSJvNxyYTR59599587 = qcMeGTcNHEiSJvNxyYTR20783027;     qcMeGTcNHEiSJvNxyYTR20783027 = qcMeGTcNHEiSJvNxyYTR91685459;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BthtOymNfxjDwuHTerIO67750839() {     float pogcmjfgfjSdxNctURSu91678987 = -440558807;    float pogcmjfgfjSdxNctURSu49672463 = -431471270;    float pogcmjfgfjSdxNctURSu61072376 = -975443938;    float pogcmjfgfjSdxNctURSu4161561 = -865330583;    float pogcmjfgfjSdxNctURSu84591701 = -920746287;    float pogcmjfgfjSdxNctURSu8420933 = -470483285;    float pogcmjfgfjSdxNctURSu71070891 = 92867882;    float pogcmjfgfjSdxNctURSu34967799 = -911058707;    float pogcmjfgfjSdxNctURSu18156184 = -888916955;    float pogcmjfgfjSdxNctURSu47417656 = -374381963;    float pogcmjfgfjSdxNctURSu19000810 = -186511505;    float pogcmjfgfjSdxNctURSu3634752 = -625638788;    float pogcmjfgfjSdxNctURSu95303253 = -209266964;    float pogcmjfgfjSdxNctURSu69263328 = -701370962;    float pogcmjfgfjSdxNctURSu46204082 = -655281469;    float pogcmjfgfjSdxNctURSu30382471 = -901759888;    float pogcmjfgfjSdxNctURSu5557834 = -486016249;    float pogcmjfgfjSdxNctURSu47171594 = -179167999;    float pogcmjfgfjSdxNctURSu93286823 = -694330265;    float pogcmjfgfjSdxNctURSu52353504 = -95938904;    float pogcmjfgfjSdxNctURSu54618191 = -474215171;    float pogcmjfgfjSdxNctURSu48886396 = -200145213;    float pogcmjfgfjSdxNctURSu55310433 = -6335714;    float pogcmjfgfjSdxNctURSu67880973 = -434388053;    float pogcmjfgfjSdxNctURSu60039733 = -193980772;    float pogcmjfgfjSdxNctURSu70594972 = -697288727;    float pogcmjfgfjSdxNctURSu20333692 = -748320532;    float pogcmjfgfjSdxNctURSu99573016 = -231918639;    float pogcmjfgfjSdxNctURSu74953364 = -969773213;    float pogcmjfgfjSdxNctURSu90331944 = -89043170;    float pogcmjfgfjSdxNctURSu8041066 = -802959134;    float pogcmjfgfjSdxNctURSu41630749 = -817223676;    float pogcmjfgfjSdxNctURSu11427041 = -948030039;    float pogcmjfgfjSdxNctURSu59051392 = -426875249;    float pogcmjfgfjSdxNctURSu30318326 = -272730134;    float pogcmjfgfjSdxNctURSu83645818 = -659283243;    float pogcmjfgfjSdxNctURSu48027334 = -153836533;    float pogcmjfgfjSdxNctURSu63399854 = -862176551;    float pogcmjfgfjSdxNctURSu95671372 = -139926931;    float pogcmjfgfjSdxNctURSu89313086 = -429119816;    float pogcmjfgfjSdxNctURSu70518560 = -363786483;    float pogcmjfgfjSdxNctURSu98359910 = -763655452;    float pogcmjfgfjSdxNctURSu80047303 = -896653020;    float pogcmjfgfjSdxNctURSu94981241 = -20511850;    float pogcmjfgfjSdxNctURSu46574392 = -850705425;    float pogcmjfgfjSdxNctURSu43066001 = -392239543;    float pogcmjfgfjSdxNctURSu87936291 = -87537799;    float pogcmjfgfjSdxNctURSu41177371 = 13429263;    float pogcmjfgfjSdxNctURSu74800938 = -446297459;    float pogcmjfgfjSdxNctURSu89560020 = 15244199;    float pogcmjfgfjSdxNctURSu90295862 = -424756204;    float pogcmjfgfjSdxNctURSu50851847 = -299031775;    float pogcmjfgfjSdxNctURSu12059309 = -731216041;    float pogcmjfgfjSdxNctURSu91933402 = -192409942;    float pogcmjfgfjSdxNctURSu13132649 = -764537186;    float pogcmjfgfjSdxNctURSu42792592 = -140413595;    float pogcmjfgfjSdxNctURSu94362029 = -325135556;    float pogcmjfgfjSdxNctURSu93191402 = -441055885;    float pogcmjfgfjSdxNctURSu44121827 = -571349812;    float pogcmjfgfjSdxNctURSu13996729 = -123457560;    float pogcmjfgfjSdxNctURSu88087240 = -722162753;    float pogcmjfgfjSdxNctURSu71497874 = -675213479;    float pogcmjfgfjSdxNctURSu60014435 = -941285495;    float pogcmjfgfjSdxNctURSu27824240 = -699873785;    float pogcmjfgfjSdxNctURSu39376591 = -571422830;    float pogcmjfgfjSdxNctURSu77370061 = -369287829;    float pogcmjfgfjSdxNctURSu92207710 = -677608750;    float pogcmjfgfjSdxNctURSu36251861 = -782391715;    float pogcmjfgfjSdxNctURSu38945002 = -328640828;    float pogcmjfgfjSdxNctURSu62558263 = -995998227;    float pogcmjfgfjSdxNctURSu82355137 = -647923355;    float pogcmjfgfjSdxNctURSu42157979 = -623839698;    float pogcmjfgfjSdxNctURSu51500221 = 60758931;    float pogcmjfgfjSdxNctURSu3973737 = -165210450;    float pogcmjfgfjSdxNctURSu81834943 = -732152422;    float pogcmjfgfjSdxNctURSu56258281 = -710559720;    float pogcmjfgfjSdxNctURSu68839092 = -303492194;    float pogcmjfgfjSdxNctURSu60329192 = -985823864;    float pogcmjfgfjSdxNctURSu21306582 = -583682629;    float pogcmjfgfjSdxNctURSu16973733 = -801741229;    float pogcmjfgfjSdxNctURSu82658680 = -509750929;    float pogcmjfgfjSdxNctURSu79156321 = -661749796;    float pogcmjfgfjSdxNctURSu24772079 = -785621181;    float pogcmjfgfjSdxNctURSu85393344 = -885017413;    float pogcmjfgfjSdxNctURSu36082 = -664286967;    float pogcmjfgfjSdxNctURSu57189218 = -403927360;    float pogcmjfgfjSdxNctURSu29571440 = 13992364;    float pogcmjfgfjSdxNctURSu19493639 = -655620097;    float pogcmjfgfjSdxNctURSu45918744 = -662338064;    float pogcmjfgfjSdxNctURSu87525733 = -32316540;    float pogcmjfgfjSdxNctURSu89283788 = -234147687;    float pogcmjfgfjSdxNctURSu54835931 = -712780649;    float pogcmjfgfjSdxNctURSu19278028 = -190826740;    float pogcmjfgfjSdxNctURSu81674643 = 83530629;    float pogcmjfgfjSdxNctURSu1225847 = -706957064;    float pogcmjfgfjSdxNctURSu99020686 = -688573005;    float pogcmjfgfjSdxNctURSu38345475 = -822369957;    float pogcmjfgfjSdxNctURSu52223063 = -96779235;    float pogcmjfgfjSdxNctURSu55604650 = -449089021;    float pogcmjfgfjSdxNctURSu69204331 = -440558807;     pogcmjfgfjSdxNctURSu91678987 = pogcmjfgfjSdxNctURSu49672463;     pogcmjfgfjSdxNctURSu49672463 = pogcmjfgfjSdxNctURSu61072376;     pogcmjfgfjSdxNctURSu61072376 = pogcmjfgfjSdxNctURSu4161561;     pogcmjfgfjSdxNctURSu4161561 = pogcmjfgfjSdxNctURSu84591701;     pogcmjfgfjSdxNctURSu84591701 = pogcmjfgfjSdxNctURSu8420933;     pogcmjfgfjSdxNctURSu8420933 = pogcmjfgfjSdxNctURSu71070891;     pogcmjfgfjSdxNctURSu71070891 = pogcmjfgfjSdxNctURSu34967799;     pogcmjfgfjSdxNctURSu34967799 = pogcmjfgfjSdxNctURSu18156184;     pogcmjfgfjSdxNctURSu18156184 = pogcmjfgfjSdxNctURSu47417656;     pogcmjfgfjSdxNctURSu47417656 = pogcmjfgfjSdxNctURSu19000810;     pogcmjfgfjSdxNctURSu19000810 = pogcmjfgfjSdxNctURSu3634752;     pogcmjfgfjSdxNctURSu3634752 = pogcmjfgfjSdxNctURSu95303253;     pogcmjfgfjSdxNctURSu95303253 = pogcmjfgfjSdxNctURSu69263328;     pogcmjfgfjSdxNctURSu69263328 = pogcmjfgfjSdxNctURSu46204082;     pogcmjfgfjSdxNctURSu46204082 = pogcmjfgfjSdxNctURSu30382471;     pogcmjfgfjSdxNctURSu30382471 = pogcmjfgfjSdxNctURSu5557834;     pogcmjfgfjSdxNctURSu5557834 = pogcmjfgfjSdxNctURSu47171594;     pogcmjfgfjSdxNctURSu47171594 = pogcmjfgfjSdxNctURSu93286823;     pogcmjfgfjSdxNctURSu93286823 = pogcmjfgfjSdxNctURSu52353504;     pogcmjfgfjSdxNctURSu52353504 = pogcmjfgfjSdxNctURSu54618191;     pogcmjfgfjSdxNctURSu54618191 = pogcmjfgfjSdxNctURSu48886396;     pogcmjfgfjSdxNctURSu48886396 = pogcmjfgfjSdxNctURSu55310433;     pogcmjfgfjSdxNctURSu55310433 = pogcmjfgfjSdxNctURSu67880973;     pogcmjfgfjSdxNctURSu67880973 = pogcmjfgfjSdxNctURSu60039733;     pogcmjfgfjSdxNctURSu60039733 = pogcmjfgfjSdxNctURSu70594972;     pogcmjfgfjSdxNctURSu70594972 = pogcmjfgfjSdxNctURSu20333692;     pogcmjfgfjSdxNctURSu20333692 = pogcmjfgfjSdxNctURSu99573016;     pogcmjfgfjSdxNctURSu99573016 = pogcmjfgfjSdxNctURSu74953364;     pogcmjfgfjSdxNctURSu74953364 = pogcmjfgfjSdxNctURSu90331944;     pogcmjfgfjSdxNctURSu90331944 = pogcmjfgfjSdxNctURSu8041066;     pogcmjfgfjSdxNctURSu8041066 = pogcmjfgfjSdxNctURSu41630749;     pogcmjfgfjSdxNctURSu41630749 = pogcmjfgfjSdxNctURSu11427041;     pogcmjfgfjSdxNctURSu11427041 = pogcmjfgfjSdxNctURSu59051392;     pogcmjfgfjSdxNctURSu59051392 = pogcmjfgfjSdxNctURSu30318326;     pogcmjfgfjSdxNctURSu30318326 = pogcmjfgfjSdxNctURSu83645818;     pogcmjfgfjSdxNctURSu83645818 = pogcmjfgfjSdxNctURSu48027334;     pogcmjfgfjSdxNctURSu48027334 = pogcmjfgfjSdxNctURSu63399854;     pogcmjfgfjSdxNctURSu63399854 = pogcmjfgfjSdxNctURSu95671372;     pogcmjfgfjSdxNctURSu95671372 = pogcmjfgfjSdxNctURSu89313086;     pogcmjfgfjSdxNctURSu89313086 = pogcmjfgfjSdxNctURSu70518560;     pogcmjfgfjSdxNctURSu70518560 = pogcmjfgfjSdxNctURSu98359910;     pogcmjfgfjSdxNctURSu98359910 = pogcmjfgfjSdxNctURSu80047303;     pogcmjfgfjSdxNctURSu80047303 = pogcmjfgfjSdxNctURSu94981241;     pogcmjfgfjSdxNctURSu94981241 = pogcmjfgfjSdxNctURSu46574392;     pogcmjfgfjSdxNctURSu46574392 = pogcmjfgfjSdxNctURSu43066001;     pogcmjfgfjSdxNctURSu43066001 = pogcmjfgfjSdxNctURSu87936291;     pogcmjfgfjSdxNctURSu87936291 = pogcmjfgfjSdxNctURSu41177371;     pogcmjfgfjSdxNctURSu41177371 = pogcmjfgfjSdxNctURSu74800938;     pogcmjfgfjSdxNctURSu74800938 = pogcmjfgfjSdxNctURSu89560020;     pogcmjfgfjSdxNctURSu89560020 = pogcmjfgfjSdxNctURSu90295862;     pogcmjfgfjSdxNctURSu90295862 = pogcmjfgfjSdxNctURSu50851847;     pogcmjfgfjSdxNctURSu50851847 = pogcmjfgfjSdxNctURSu12059309;     pogcmjfgfjSdxNctURSu12059309 = pogcmjfgfjSdxNctURSu91933402;     pogcmjfgfjSdxNctURSu91933402 = pogcmjfgfjSdxNctURSu13132649;     pogcmjfgfjSdxNctURSu13132649 = pogcmjfgfjSdxNctURSu42792592;     pogcmjfgfjSdxNctURSu42792592 = pogcmjfgfjSdxNctURSu94362029;     pogcmjfgfjSdxNctURSu94362029 = pogcmjfgfjSdxNctURSu93191402;     pogcmjfgfjSdxNctURSu93191402 = pogcmjfgfjSdxNctURSu44121827;     pogcmjfgfjSdxNctURSu44121827 = pogcmjfgfjSdxNctURSu13996729;     pogcmjfgfjSdxNctURSu13996729 = pogcmjfgfjSdxNctURSu88087240;     pogcmjfgfjSdxNctURSu88087240 = pogcmjfgfjSdxNctURSu71497874;     pogcmjfgfjSdxNctURSu71497874 = pogcmjfgfjSdxNctURSu60014435;     pogcmjfgfjSdxNctURSu60014435 = pogcmjfgfjSdxNctURSu27824240;     pogcmjfgfjSdxNctURSu27824240 = pogcmjfgfjSdxNctURSu39376591;     pogcmjfgfjSdxNctURSu39376591 = pogcmjfgfjSdxNctURSu77370061;     pogcmjfgfjSdxNctURSu77370061 = pogcmjfgfjSdxNctURSu92207710;     pogcmjfgfjSdxNctURSu92207710 = pogcmjfgfjSdxNctURSu36251861;     pogcmjfgfjSdxNctURSu36251861 = pogcmjfgfjSdxNctURSu38945002;     pogcmjfgfjSdxNctURSu38945002 = pogcmjfgfjSdxNctURSu62558263;     pogcmjfgfjSdxNctURSu62558263 = pogcmjfgfjSdxNctURSu82355137;     pogcmjfgfjSdxNctURSu82355137 = pogcmjfgfjSdxNctURSu42157979;     pogcmjfgfjSdxNctURSu42157979 = pogcmjfgfjSdxNctURSu51500221;     pogcmjfgfjSdxNctURSu51500221 = pogcmjfgfjSdxNctURSu3973737;     pogcmjfgfjSdxNctURSu3973737 = pogcmjfgfjSdxNctURSu81834943;     pogcmjfgfjSdxNctURSu81834943 = pogcmjfgfjSdxNctURSu56258281;     pogcmjfgfjSdxNctURSu56258281 = pogcmjfgfjSdxNctURSu68839092;     pogcmjfgfjSdxNctURSu68839092 = pogcmjfgfjSdxNctURSu60329192;     pogcmjfgfjSdxNctURSu60329192 = pogcmjfgfjSdxNctURSu21306582;     pogcmjfgfjSdxNctURSu21306582 = pogcmjfgfjSdxNctURSu16973733;     pogcmjfgfjSdxNctURSu16973733 = pogcmjfgfjSdxNctURSu82658680;     pogcmjfgfjSdxNctURSu82658680 = pogcmjfgfjSdxNctURSu79156321;     pogcmjfgfjSdxNctURSu79156321 = pogcmjfgfjSdxNctURSu24772079;     pogcmjfgfjSdxNctURSu24772079 = pogcmjfgfjSdxNctURSu85393344;     pogcmjfgfjSdxNctURSu85393344 = pogcmjfgfjSdxNctURSu36082;     pogcmjfgfjSdxNctURSu36082 = pogcmjfgfjSdxNctURSu57189218;     pogcmjfgfjSdxNctURSu57189218 = pogcmjfgfjSdxNctURSu29571440;     pogcmjfgfjSdxNctURSu29571440 = pogcmjfgfjSdxNctURSu19493639;     pogcmjfgfjSdxNctURSu19493639 = pogcmjfgfjSdxNctURSu45918744;     pogcmjfgfjSdxNctURSu45918744 = pogcmjfgfjSdxNctURSu87525733;     pogcmjfgfjSdxNctURSu87525733 = pogcmjfgfjSdxNctURSu89283788;     pogcmjfgfjSdxNctURSu89283788 = pogcmjfgfjSdxNctURSu54835931;     pogcmjfgfjSdxNctURSu54835931 = pogcmjfgfjSdxNctURSu19278028;     pogcmjfgfjSdxNctURSu19278028 = pogcmjfgfjSdxNctURSu81674643;     pogcmjfgfjSdxNctURSu81674643 = pogcmjfgfjSdxNctURSu1225847;     pogcmjfgfjSdxNctURSu1225847 = pogcmjfgfjSdxNctURSu99020686;     pogcmjfgfjSdxNctURSu99020686 = pogcmjfgfjSdxNctURSu38345475;     pogcmjfgfjSdxNctURSu38345475 = pogcmjfgfjSdxNctURSu52223063;     pogcmjfgfjSdxNctURSu52223063 = pogcmjfgfjSdxNctURSu55604650;     pogcmjfgfjSdxNctURSu55604650 = pogcmjfgfjSdxNctURSu69204331;     pogcmjfgfjSdxNctURSu69204331 = pogcmjfgfjSdxNctURSu91678987;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mlyLorzmcganfcTyJUcW22167053() {     float OBEfBVvilgTjyRQBaFcQ62330655 = 41292062;    float OBEfBVvilgTjyRQBaFcQ18345981 = -303114569;    float OBEfBVvilgTjyRQBaFcQ41041296 = -317738397;    float OBEfBVvilgTjyRQBaFcQ44092847 = 44531027;    float OBEfBVvilgTjyRQBaFcQ6042023 = -704152634;    float OBEfBVvilgTjyRQBaFcQ98420694 = -172064436;    float OBEfBVvilgTjyRQBaFcQ89449786 = 94092172;    float OBEfBVvilgTjyRQBaFcQ91810179 = -157875797;    float OBEfBVvilgTjyRQBaFcQ64021238 = -982264283;    float OBEfBVvilgTjyRQBaFcQ70384116 = -675477314;    float OBEfBVvilgTjyRQBaFcQ61249970 = -221413434;    float OBEfBVvilgTjyRQBaFcQ49181323 = -941402581;    float OBEfBVvilgTjyRQBaFcQ75130640 = -118804022;    float OBEfBVvilgTjyRQBaFcQ53110361 = -502399172;    float OBEfBVvilgTjyRQBaFcQ28924101 = -805031472;    float OBEfBVvilgTjyRQBaFcQ62911111 = -471743475;    float OBEfBVvilgTjyRQBaFcQ99410660 = -395201379;    float OBEfBVvilgTjyRQBaFcQ67465088 = -168334195;    float OBEfBVvilgTjyRQBaFcQ74994605 = -301518207;    float OBEfBVvilgTjyRQBaFcQ11140214 = -638305209;    float OBEfBVvilgTjyRQBaFcQ9753946 = -382894312;    float OBEfBVvilgTjyRQBaFcQ10282161 = -531430518;    float OBEfBVvilgTjyRQBaFcQ57254242 = -574178859;    float OBEfBVvilgTjyRQBaFcQ1730511 = -26788960;    float OBEfBVvilgTjyRQBaFcQ24164122 = -969885460;    float OBEfBVvilgTjyRQBaFcQ1192553 = -795573172;    float OBEfBVvilgTjyRQBaFcQ28594478 = -43498308;    float OBEfBVvilgTjyRQBaFcQ7355242 = -737042407;    float OBEfBVvilgTjyRQBaFcQ7095853 = -164131430;    float OBEfBVvilgTjyRQBaFcQ29504040 = -647438241;    float OBEfBVvilgTjyRQBaFcQ15113062 = -551392820;    float OBEfBVvilgTjyRQBaFcQ86142209 = -45520839;    float OBEfBVvilgTjyRQBaFcQ94372401 = -894991648;    float OBEfBVvilgTjyRQBaFcQ24416368 = 9595067;    float OBEfBVvilgTjyRQBaFcQ8342563 = -119676914;    float OBEfBVvilgTjyRQBaFcQ43639077 = -738509791;    float OBEfBVvilgTjyRQBaFcQ91818819 = -342429741;    float OBEfBVvilgTjyRQBaFcQ16192344 = -646289659;    float OBEfBVvilgTjyRQBaFcQ74204661 = -95655568;    float OBEfBVvilgTjyRQBaFcQ84416385 = 53271821;    float OBEfBVvilgTjyRQBaFcQ30591845 = -747805361;    float OBEfBVvilgTjyRQBaFcQ26842901 = -533044521;    float OBEfBVvilgTjyRQBaFcQ29014263 = -883407072;    float OBEfBVvilgTjyRQBaFcQ1489542 = -92486822;    float OBEfBVvilgTjyRQBaFcQ42528451 = -59351819;    float OBEfBVvilgTjyRQBaFcQ49471836 = -150576698;    float OBEfBVvilgTjyRQBaFcQ14067495 = -203607598;    float OBEfBVvilgTjyRQBaFcQ6791582 = -438701032;    float OBEfBVvilgTjyRQBaFcQ80242390 = -432061428;    float OBEfBVvilgTjyRQBaFcQ97987506 = -267764679;    float OBEfBVvilgTjyRQBaFcQ58485187 = 28671675;    float OBEfBVvilgTjyRQBaFcQ92723351 = -268251497;    float OBEfBVvilgTjyRQBaFcQ67134965 = -659014083;    float OBEfBVvilgTjyRQBaFcQ82160706 = -853291439;    float OBEfBVvilgTjyRQBaFcQ12413107 = -875478100;    float OBEfBVvilgTjyRQBaFcQ52048494 = -427277420;    float OBEfBVvilgTjyRQBaFcQ61091739 = -728935710;    float OBEfBVvilgTjyRQBaFcQ39310785 = -190949438;    float OBEfBVvilgTjyRQBaFcQ19928726 = 14416487;    float OBEfBVvilgTjyRQBaFcQ4849470 = -908579463;    float OBEfBVvilgTjyRQBaFcQ69826217 = -28566129;    float OBEfBVvilgTjyRQBaFcQ82094544 = -168865421;    float OBEfBVvilgTjyRQBaFcQ84714327 = -993744368;    float OBEfBVvilgTjyRQBaFcQ34517198 = -234826043;    float OBEfBVvilgTjyRQBaFcQ55271055 = -24084495;    float OBEfBVvilgTjyRQBaFcQ75107761 = -75892595;    float OBEfBVvilgTjyRQBaFcQ54808921 = 53589066;    float OBEfBVvilgTjyRQBaFcQ50714273 = -28399089;    float OBEfBVvilgTjyRQBaFcQ44767799 = -282722259;    float OBEfBVvilgTjyRQBaFcQ85285024 = 33478318;    float OBEfBVvilgTjyRQBaFcQ71092292 = -29313735;    float OBEfBVvilgTjyRQBaFcQ83218317 = -748911721;    float OBEfBVvilgTjyRQBaFcQ93260426 = 27321372;    float OBEfBVvilgTjyRQBaFcQ90578219 = -254790029;    float OBEfBVvilgTjyRQBaFcQ80548368 = -890499848;    float OBEfBVvilgTjyRQBaFcQ82911044 = -849849791;    float OBEfBVvilgTjyRQBaFcQ81267897 = -648023447;    float OBEfBVvilgTjyRQBaFcQ55764700 = -381692038;    float OBEfBVvilgTjyRQBaFcQ59202060 = -967437141;    float OBEfBVvilgTjyRQBaFcQ74692286 = -719308762;    float OBEfBVvilgTjyRQBaFcQ87125058 = -491965574;    float OBEfBVvilgTjyRQBaFcQ21802897 = -604797276;    float OBEfBVvilgTjyRQBaFcQ27112852 = -204980980;    float OBEfBVvilgTjyRQBaFcQ9108346 = -896366751;    float OBEfBVvilgTjyRQBaFcQ71018853 = -576109916;    float OBEfBVvilgTjyRQBaFcQ22389710 = -183141323;    float OBEfBVvilgTjyRQBaFcQ19007244 = -386506757;    float OBEfBVvilgTjyRQBaFcQ12211695 = 58299790;    float OBEfBVvilgTjyRQBaFcQ12003261 = -114926834;    float OBEfBVvilgTjyRQBaFcQ56294068 = -692399494;    float OBEfBVvilgTjyRQBaFcQ82547338 = 90425919;    float OBEfBVvilgTjyRQBaFcQ52508035 = -51480303;    float OBEfBVvilgTjyRQBaFcQ96263617 = -560706146;    float OBEfBVvilgTjyRQBaFcQ69355191 = -187076105;    float OBEfBVvilgTjyRQBaFcQ14590168 = -918162050;    float OBEfBVvilgTjyRQBaFcQ48497300 = -478939941;    float OBEfBVvilgTjyRQBaFcQ42128574 = -539300154;    float OBEfBVvilgTjyRQBaFcQ94497064 = -548581029;    float OBEfBVvilgTjyRQBaFcQ46218487 = 31597672;    float OBEfBVvilgTjyRQBaFcQ67420690 = 41292062;     OBEfBVvilgTjyRQBaFcQ62330655 = OBEfBVvilgTjyRQBaFcQ18345981;     OBEfBVvilgTjyRQBaFcQ18345981 = OBEfBVvilgTjyRQBaFcQ41041296;     OBEfBVvilgTjyRQBaFcQ41041296 = OBEfBVvilgTjyRQBaFcQ44092847;     OBEfBVvilgTjyRQBaFcQ44092847 = OBEfBVvilgTjyRQBaFcQ6042023;     OBEfBVvilgTjyRQBaFcQ6042023 = OBEfBVvilgTjyRQBaFcQ98420694;     OBEfBVvilgTjyRQBaFcQ98420694 = OBEfBVvilgTjyRQBaFcQ89449786;     OBEfBVvilgTjyRQBaFcQ89449786 = OBEfBVvilgTjyRQBaFcQ91810179;     OBEfBVvilgTjyRQBaFcQ91810179 = OBEfBVvilgTjyRQBaFcQ64021238;     OBEfBVvilgTjyRQBaFcQ64021238 = OBEfBVvilgTjyRQBaFcQ70384116;     OBEfBVvilgTjyRQBaFcQ70384116 = OBEfBVvilgTjyRQBaFcQ61249970;     OBEfBVvilgTjyRQBaFcQ61249970 = OBEfBVvilgTjyRQBaFcQ49181323;     OBEfBVvilgTjyRQBaFcQ49181323 = OBEfBVvilgTjyRQBaFcQ75130640;     OBEfBVvilgTjyRQBaFcQ75130640 = OBEfBVvilgTjyRQBaFcQ53110361;     OBEfBVvilgTjyRQBaFcQ53110361 = OBEfBVvilgTjyRQBaFcQ28924101;     OBEfBVvilgTjyRQBaFcQ28924101 = OBEfBVvilgTjyRQBaFcQ62911111;     OBEfBVvilgTjyRQBaFcQ62911111 = OBEfBVvilgTjyRQBaFcQ99410660;     OBEfBVvilgTjyRQBaFcQ99410660 = OBEfBVvilgTjyRQBaFcQ67465088;     OBEfBVvilgTjyRQBaFcQ67465088 = OBEfBVvilgTjyRQBaFcQ74994605;     OBEfBVvilgTjyRQBaFcQ74994605 = OBEfBVvilgTjyRQBaFcQ11140214;     OBEfBVvilgTjyRQBaFcQ11140214 = OBEfBVvilgTjyRQBaFcQ9753946;     OBEfBVvilgTjyRQBaFcQ9753946 = OBEfBVvilgTjyRQBaFcQ10282161;     OBEfBVvilgTjyRQBaFcQ10282161 = OBEfBVvilgTjyRQBaFcQ57254242;     OBEfBVvilgTjyRQBaFcQ57254242 = OBEfBVvilgTjyRQBaFcQ1730511;     OBEfBVvilgTjyRQBaFcQ1730511 = OBEfBVvilgTjyRQBaFcQ24164122;     OBEfBVvilgTjyRQBaFcQ24164122 = OBEfBVvilgTjyRQBaFcQ1192553;     OBEfBVvilgTjyRQBaFcQ1192553 = OBEfBVvilgTjyRQBaFcQ28594478;     OBEfBVvilgTjyRQBaFcQ28594478 = OBEfBVvilgTjyRQBaFcQ7355242;     OBEfBVvilgTjyRQBaFcQ7355242 = OBEfBVvilgTjyRQBaFcQ7095853;     OBEfBVvilgTjyRQBaFcQ7095853 = OBEfBVvilgTjyRQBaFcQ29504040;     OBEfBVvilgTjyRQBaFcQ29504040 = OBEfBVvilgTjyRQBaFcQ15113062;     OBEfBVvilgTjyRQBaFcQ15113062 = OBEfBVvilgTjyRQBaFcQ86142209;     OBEfBVvilgTjyRQBaFcQ86142209 = OBEfBVvilgTjyRQBaFcQ94372401;     OBEfBVvilgTjyRQBaFcQ94372401 = OBEfBVvilgTjyRQBaFcQ24416368;     OBEfBVvilgTjyRQBaFcQ24416368 = OBEfBVvilgTjyRQBaFcQ8342563;     OBEfBVvilgTjyRQBaFcQ8342563 = OBEfBVvilgTjyRQBaFcQ43639077;     OBEfBVvilgTjyRQBaFcQ43639077 = OBEfBVvilgTjyRQBaFcQ91818819;     OBEfBVvilgTjyRQBaFcQ91818819 = OBEfBVvilgTjyRQBaFcQ16192344;     OBEfBVvilgTjyRQBaFcQ16192344 = OBEfBVvilgTjyRQBaFcQ74204661;     OBEfBVvilgTjyRQBaFcQ74204661 = OBEfBVvilgTjyRQBaFcQ84416385;     OBEfBVvilgTjyRQBaFcQ84416385 = OBEfBVvilgTjyRQBaFcQ30591845;     OBEfBVvilgTjyRQBaFcQ30591845 = OBEfBVvilgTjyRQBaFcQ26842901;     OBEfBVvilgTjyRQBaFcQ26842901 = OBEfBVvilgTjyRQBaFcQ29014263;     OBEfBVvilgTjyRQBaFcQ29014263 = OBEfBVvilgTjyRQBaFcQ1489542;     OBEfBVvilgTjyRQBaFcQ1489542 = OBEfBVvilgTjyRQBaFcQ42528451;     OBEfBVvilgTjyRQBaFcQ42528451 = OBEfBVvilgTjyRQBaFcQ49471836;     OBEfBVvilgTjyRQBaFcQ49471836 = OBEfBVvilgTjyRQBaFcQ14067495;     OBEfBVvilgTjyRQBaFcQ14067495 = OBEfBVvilgTjyRQBaFcQ6791582;     OBEfBVvilgTjyRQBaFcQ6791582 = OBEfBVvilgTjyRQBaFcQ80242390;     OBEfBVvilgTjyRQBaFcQ80242390 = OBEfBVvilgTjyRQBaFcQ97987506;     OBEfBVvilgTjyRQBaFcQ97987506 = OBEfBVvilgTjyRQBaFcQ58485187;     OBEfBVvilgTjyRQBaFcQ58485187 = OBEfBVvilgTjyRQBaFcQ92723351;     OBEfBVvilgTjyRQBaFcQ92723351 = OBEfBVvilgTjyRQBaFcQ67134965;     OBEfBVvilgTjyRQBaFcQ67134965 = OBEfBVvilgTjyRQBaFcQ82160706;     OBEfBVvilgTjyRQBaFcQ82160706 = OBEfBVvilgTjyRQBaFcQ12413107;     OBEfBVvilgTjyRQBaFcQ12413107 = OBEfBVvilgTjyRQBaFcQ52048494;     OBEfBVvilgTjyRQBaFcQ52048494 = OBEfBVvilgTjyRQBaFcQ61091739;     OBEfBVvilgTjyRQBaFcQ61091739 = OBEfBVvilgTjyRQBaFcQ39310785;     OBEfBVvilgTjyRQBaFcQ39310785 = OBEfBVvilgTjyRQBaFcQ19928726;     OBEfBVvilgTjyRQBaFcQ19928726 = OBEfBVvilgTjyRQBaFcQ4849470;     OBEfBVvilgTjyRQBaFcQ4849470 = OBEfBVvilgTjyRQBaFcQ69826217;     OBEfBVvilgTjyRQBaFcQ69826217 = OBEfBVvilgTjyRQBaFcQ82094544;     OBEfBVvilgTjyRQBaFcQ82094544 = OBEfBVvilgTjyRQBaFcQ84714327;     OBEfBVvilgTjyRQBaFcQ84714327 = OBEfBVvilgTjyRQBaFcQ34517198;     OBEfBVvilgTjyRQBaFcQ34517198 = OBEfBVvilgTjyRQBaFcQ55271055;     OBEfBVvilgTjyRQBaFcQ55271055 = OBEfBVvilgTjyRQBaFcQ75107761;     OBEfBVvilgTjyRQBaFcQ75107761 = OBEfBVvilgTjyRQBaFcQ54808921;     OBEfBVvilgTjyRQBaFcQ54808921 = OBEfBVvilgTjyRQBaFcQ50714273;     OBEfBVvilgTjyRQBaFcQ50714273 = OBEfBVvilgTjyRQBaFcQ44767799;     OBEfBVvilgTjyRQBaFcQ44767799 = OBEfBVvilgTjyRQBaFcQ85285024;     OBEfBVvilgTjyRQBaFcQ85285024 = OBEfBVvilgTjyRQBaFcQ71092292;     OBEfBVvilgTjyRQBaFcQ71092292 = OBEfBVvilgTjyRQBaFcQ83218317;     OBEfBVvilgTjyRQBaFcQ83218317 = OBEfBVvilgTjyRQBaFcQ93260426;     OBEfBVvilgTjyRQBaFcQ93260426 = OBEfBVvilgTjyRQBaFcQ90578219;     OBEfBVvilgTjyRQBaFcQ90578219 = OBEfBVvilgTjyRQBaFcQ80548368;     OBEfBVvilgTjyRQBaFcQ80548368 = OBEfBVvilgTjyRQBaFcQ82911044;     OBEfBVvilgTjyRQBaFcQ82911044 = OBEfBVvilgTjyRQBaFcQ81267897;     OBEfBVvilgTjyRQBaFcQ81267897 = OBEfBVvilgTjyRQBaFcQ55764700;     OBEfBVvilgTjyRQBaFcQ55764700 = OBEfBVvilgTjyRQBaFcQ59202060;     OBEfBVvilgTjyRQBaFcQ59202060 = OBEfBVvilgTjyRQBaFcQ74692286;     OBEfBVvilgTjyRQBaFcQ74692286 = OBEfBVvilgTjyRQBaFcQ87125058;     OBEfBVvilgTjyRQBaFcQ87125058 = OBEfBVvilgTjyRQBaFcQ21802897;     OBEfBVvilgTjyRQBaFcQ21802897 = OBEfBVvilgTjyRQBaFcQ27112852;     OBEfBVvilgTjyRQBaFcQ27112852 = OBEfBVvilgTjyRQBaFcQ9108346;     OBEfBVvilgTjyRQBaFcQ9108346 = OBEfBVvilgTjyRQBaFcQ71018853;     OBEfBVvilgTjyRQBaFcQ71018853 = OBEfBVvilgTjyRQBaFcQ22389710;     OBEfBVvilgTjyRQBaFcQ22389710 = OBEfBVvilgTjyRQBaFcQ19007244;     OBEfBVvilgTjyRQBaFcQ19007244 = OBEfBVvilgTjyRQBaFcQ12211695;     OBEfBVvilgTjyRQBaFcQ12211695 = OBEfBVvilgTjyRQBaFcQ12003261;     OBEfBVvilgTjyRQBaFcQ12003261 = OBEfBVvilgTjyRQBaFcQ56294068;     OBEfBVvilgTjyRQBaFcQ56294068 = OBEfBVvilgTjyRQBaFcQ82547338;     OBEfBVvilgTjyRQBaFcQ82547338 = OBEfBVvilgTjyRQBaFcQ52508035;     OBEfBVvilgTjyRQBaFcQ52508035 = OBEfBVvilgTjyRQBaFcQ96263617;     OBEfBVvilgTjyRQBaFcQ96263617 = OBEfBVvilgTjyRQBaFcQ69355191;     OBEfBVvilgTjyRQBaFcQ69355191 = OBEfBVvilgTjyRQBaFcQ14590168;     OBEfBVvilgTjyRQBaFcQ14590168 = OBEfBVvilgTjyRQBaFcQ48497300;     OBEfBVvilgTjyRQBaFcQ48497300 = OBEfBVvilgTjyRQBaFcQ42128574;     OBEfBVvilgTjyRQBaFcQ42128574 = OBEfBVvilgTjyRQBaFcQ94497064;     OBEfBVvilgTjyRQBaFcQ94497064 = OBEfBVvilgTjyRQBaFcQ46218487;     OBEfBVvilgTjyRQBaFcQ46218487 = OBEfBVvilgTjyRQBaFcQ67420690;     OBEfBVvilgTjyRQBaFcQ67420690 = OBEfBVvilgTjyRQBaFcQ62330655;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void jmmxyfxrLmWgiQDKcUnW28825797() {     float jTJgmpltjGXbHKlwvPzC62324183 = -931232079;    float jTJgmpltjGXbHKlwvPzC56918297 = -345709694;    float jTJgmpltjGXbHKlwvPzC42354508 = -274867011;    float jTJgmpltjGXbHKlwvPzC29172708 = -876009803;    float jTJgmpltjGXbHKlwvPzC14882125 = -72507956;    float jTJgmpltjGXbHKlwvPzC33226127 = -971497248;    float jTJgmpltjGXbHKlwvPzC57997122 = -710444295;    float jTJgmpltjGXbHKlwvPzC61024230 = -962053114;    float jTJgmpltjGXbHKlwvPzC46761402 = -695910353;    float jTJgmpltjGXbHKlwvPzC67023630 = -312284185;    float jTJgmpltjGXbHKlwvPzC81570575 = -180885506;    float jTJgmpltjGXbHKlwvPzC63472999 = -676880944;    float jTJgmpltjGXbHKlwvPzC6983555 = -140462496;    float jTJgmpltjGXbHKlwvPzC6954582 = -847552224;    float jTJgmpltjGXbHKlwvPzC74096210 = -767744324;    float jTJgmpltjGXbHKlwvPzC37475659 = -804539125;    float jTJgmpltjGXbHKlwvPzC55263420 = -214815949;    float jTJgmpltjGXbHKlwvPzC90465620 = -565405817;    float jTJgmpltjGXbHKlwvPzC95422874 = -919500474;    float jTJgmpltjGXbHKlwvPzC83213976 = -150329660;    float jTJgmpltjGXbHKlwvPzC89423369 = -93360685;    float jTJgmpltjGXbHKlwvPzC88578426 = 58577598;    float jTJgmpltjGXbHKlwvPzC94132638 = 66863299;    float jTJgmpltjGXbHKlwvPzC45380619 = 92290518;    float jTJgmpltjGXbHKlwvPzC52606811 = -296579313;    float jTJgmpltjGXbHKlwvPzC17766630 = -703779100;    float jTJgmpltjGXbHKlwvPzC98816059 = -344298020;    float jTJgmpltjGXbHKlwvPzC42758767 = -246009395;    float jTJgmpltjGXbHKlwvPzC19774299 = -810201745;    float jTJgmpltjGXbHKlwvPzC5593911 = -322869614;    float jTJgmpltjGXbHKlwvPzC48525741 = -938486054;    float jTJgmpltjGXbHKlwvPzC70866067 = -437342601;    float jTJgmpltjGXbHKlwvPzC79824825 = 3064136;    float jTJgmpltjGXbHKlwvPzC24699205 = -967086666;    float jTJgmpltjGXbHKlwvPzC90700600 = -378990839;    float jTJgmpltjGXbHKlwvPzC86467266 = -818275537;    float jTJgmpltjGXbHKlwvPzC3760188 = -442083119;    float jTJgmpltjGXbHKlwvPzC86110022 = -641552338;    float jTJgmpltjGXbHKlwvPzC9518258 = -808336984;    float jTJgmpltjGXbHKlwvPzC56289278 = -656159865;    float jTJgmpltjGXbHKlwvPzC18447512 = -835749260;    float jTJgmpltjGXbHKlwvPzC12540563 = -726836604;    float jTJgmpltjGXbHKlwvPzC89297622 = -342537326;    float jTJgmpltjGXbHKlwvPzC60721440 = -749451265;    float jTJgmpltjGXbHKlwvPzC26546504 = -532611221;    float jTJgmpltjGXbHKlwvPzC89899803 = 37378631;    float jTJgmpltjGXbHKlwvPzC41765287 = -684888220;    float jTJgmpltjGXbHKlwvPzC82175162 = 52537612;    float jTJgmpltjGXbHKlwvPzC98900779 = 4174711;    float jTJgmpltjGXbHKlwvPzC18032930 = -954789566;    float jTJgmpltjGXbHKlwvPzC71744556 = -649883332;    float jTJgmpltjGXbHKlwvPzC7270609 = -650397245;    float jTJgmpltjGXbHKlwvPzC42583314 = -551068091;    float jTJgmpltjGXbHKlwvPzC92410977 = -285211290;    float jTJgmpltjGXbHKlwvPzC39402427 = -632377378;    float jTJgmpltjGXbHKlwvPzC73745757 = -889809678;    float jTJgmpltjGXbHKlwvPzC62785658 = -312572994;    float jTJgmpltjGXbHKlwvPzC96973888 = -267157529;    float jTJgmpltjGXbHKlwvPzC76565896 = -479430491;    float jTJgmpltjGXbHKlwvPzC97115495 = -368728856;    float jTJgmpltjGXbHKlwvPzC34410068 = -527199228;    float jTJgmpltjGXbHKlwvPzC15238356 = -364434900;    float jTJgmpltjGXbHKlwvPzC41249931 = -51851370;    float jTJgmpltjGXbHKlwvPzC41167492 = -273040739;    float jTJgmpltjGXbHKlwvPzC18497890 = -373798131;    float jTJgmpltjGXbHKlwvPzC10704509 = -743542906;    float jTJgmpltjGXbHKlwvPzC83648174 = -579945080;    float jTJgmpltjGXbHKlwvPzC82284350 = -173375831;    float jTJgmpltjGXbHKlwvPzC16253981 = -368561385;    float jTJgmpltjGXbHKlwvPzC87628943 = -949468788;    float jTJgmpltjGXbHKlwvPzC33715471 = -262456007;    float jTJgmpltjGXbHKlwvPzC69153397 = -573263611;    float jTJgmpltjGXbHKlwvPzC80947363 = -757068834;    float jTJgmpltjGXbHKlwvPzC39133597 = -163340609;    float jTJgmpltjGXbHKlwvPzC64766464 = -314580401;    float jTJgmpltjGXbHKlwvPzC76882807 = -366524082;    float jTJgmpltjGXbHKlwvPzC99280803 = -598885076;    float jTJgmpltjGXbHKlwvPzC33411199 = -183685436;    float jTJgmpltjGXbHKlwvPzC18834116 = -375098262;    float jTJgmpltjGXbHKlwvPzC62707007 = -233957944;    float jTJgmpltjGXbHKlwvPzC76001342 = 81109119;    float jTJgmpltjGXbHKlwvPzC16640898 = -296835633;    float jTJgmpltjGXbHKlwvPzC43857987 = -150184107;    float jTJgmpltjGXbHKlwvPzC1741370 = -855412180;    float jTJgmpltjGXbHKlwvPzC33849354 = -672986283;    float jTJgmpltjGXbHKlwvPzC41255132 = -188088809;    float jTJgmpltjGXbHKlwvPzC28282753 = -886274510;    float jTJgmpltjGXbHKlwvPzC87413847 = -711724574;    float jTJgmpltjGXbHKlwvPzC85296777 = -234709289;    float jTJgmpltjGXbHKlwvPzC16954843 = -489181161;    float jTJgmpltjGXbHKlwvPzC23681608 = -405702544;    float jTJgmpltjGXbHKlwvPzC6786299 = -74925590;    float jTJgmpltjGXbHKlwvPzC9544127 = -62121848;    float jTJgmpltjGXbHKlwvPzC12402763 = -339608128;    float jTJgmpltjGXbHKlwvPzC21879210 = -28960637;    float jTJgmpltjGXbHKlwvPzC3209157 = -371314360;    float jTJgmpltjGXbHKlwvPzC71290631 = -574985234;    float jTJgmpltjGXbHKlwvPzC48130131 = 30503413;    float jTJgmpltjGXbHKlwvPzC42223550 = -275653135;    float jTJgmpltjGXbHKlwvPzC15841995 = -931232079;     jTJgmpltjGXbHKlwvPzC62324183 = jTJgmpltjGXbHKlwvPzC56918297;     jTJgmpltjGXbHKlwvPzC56918297 = jTJgmpltjGXbHKlwvPzC42354508;     jTJgmpltjGXbHKlwvPzC42354508 = jTJgmpltjGXbHKlwvPzC29172708;     jTJgmpltjGXbHKlwvPzC29172708 = jTJgmpltjGXbHKlwvPzC14882125;     jTJgmpltjGXbHKlwvPzC14882125 = jTJgmpltjGXbHKlwvPzC33226127;     jTJgmpltjGXbHKlwvPzC33226127 = jTJgmpltjGXbHKlwvPzC57997122;     jTJgmpltjGXbHKlwvPzC57997122 = jTJgmpltjGXbHKlwvPzC61024230;     jTJgmpltjGXbHKlwvPzC61024230 = jTJgmpltjGXbHKlwvPzC46761402;     jTJgmpltjGXbHKlwvPzC46761402 = jTJgmpltjGXbHKlwvPzC67023630;     jTJgmpltjGXbHKlwvPzC67023630 = jTJgmpltjGXbHKlwvPzC81570575;     jTJgmpltjGXbHKlwvPzC81570575 = jTJgmpltjGXbHKlwvPzC63472999;     jTJgmpltjGXbHKlwvPzC63472999 = jTJgmpltjGXbHKlwvPzC6983555;     jTJgmpltjGXbHKlwvPzC6983555 = jTJgmpltjGXbHKlwvPzC6954582;     jTJgmpltjGXbHKlwvPzC6954582 = jTJgmpltjGXbHKlwvPzC74096210;     jTJgmpltjGXbHKlwvPzC74096210 = jTJgmpltjGXbHKlwvPzC37475659;     jTJgmpltjGXbHKlwvPzC37475659 = jTJgmpltjGXbHKlwvPzC55263420;     jTJgmpltjGXbHKlwvPzC55263420 = jTJgmpltjGXbHKlwvPzC90465620;     jTJgmpltjGXbHKlwvPzC90465620 = jTJgmpltjGXbHKlwvPzC95422874;     jTJgmpltjGXbHKlwvPzC95422874 = jTJgmpltjGXbHKlwvPzC83213976;     jTJgmpltjGXbHKlwvPzC83213976 = jTJgmpltjGXbHKlwvPzC89423369;     jTJgmpltjGXbHKlwvPzC89423369 = jTJgmpltjGXbHKlwvPzC88578426;     jTJgmpltjGXbHKlwvPzC88578426 = jTJgmpltjGXbHKlwvPzC94132638;     jTJgmpltjGXbHKlwvPzC94132638 = jTJgmpltjGXbHKlwvPzC45380619;     jTJgmpltjGXbHKlwvPzC45380619 = jTJgmpltjGXbHKlwvPzC52606811;     jTJgmpltjGXbHKlwvPzC52606811 = jTJgmpltjGXbHKlwvPzC17766630;     jTJgmpltjGXbHKlwvPzC17766630 = jTJgmpltjGXbHKlwvPzC98816059;     jTJgmpltjGXbHKlwvPzC98816059 = jTJgmpltjGXbHKlwvPzC42758767;     jTJgmpltjGXbHKlwvPzC42758767 = jTJgmpltjGXbHKlwvPzC19774299;     jTJgmpltjGXbHKlwvPzC19774299 = jTJgmpltjGXbHKlwvPzC5593911;     jTJgmpltjGXbHKlwvPzC5593911 = jTJgmpltjGXbHKlwvPzC48525741;     jTJgmpltjGXbHKlwvPzC48525741 = jTJgmpltjGXbHKlwvPzC70866067;     jTJgmpltjGXbHKlwvPzC70866067 = jTJgmpltjGXbHKlwvPzC79824825;     jTJgmpltjGXbHKlwvPzC79824825 = jTJgmpltjGXbHKlwvPzC24699205;     jTJgmpltjGXbHKlwvPzC24699205 = jTJgmpltjGXbHKlwvPzC90700600;     jTJgmpltjGXbHKlwvPzC90700600 = jTJgmpltjGXbHKlwvPzC86467266;     jTJgmpltjGXbHKlwvPzC86467266 = jTJgmpltjGXbHKlwvPzC3760188;     jTJgmpltjGXbHKlwvPzC3760188 = jTJgmpltjGXbHKlwvPzC86110022;     jTJgmpltjGXbHKlwvPzC86110022 = jTJgmpltjGXbHKlwvPzC9518258;     jTJgmpltjGXbHKlwvPzC9518258 = jTJgmpltjGXbHKlwvPzC56289278;     jTJgmpltjGXbHKlwvPzC56289278 = jTJgmpltjGXbHKlwvPzC18447512;     jTJgmpltjGXbHKlwvPzC18447512 = jTJgmpltjGXbHKlwvPzC12540563;     jTJgmpltjGXbHKlwvPzC12540563 = jTJgmpltjGXbHKlwvPzC89297622;     jTJgmpltjGXbHKlwvPzC89297622 = jTJgmpltjGXbHKlwvPzC60721440;     jTJgmpltjGXbHKlwvPzC60721440 = jTJgmpltjGXbHKlwvPzC26546504;     jTJgmpltjGXbHKlwvPzC26546504 = jTJgmpltjGXbHKlwvPzC89899803;     jTJgmpltjGXbHKlwvPzC89899803 = jTJgmpltjGXbHKlwvPzC41765287;     jTJgmpltjGXbHKlwvPzC41765287 = jTJgmpltjGXbHKlwvPzC82175162;     jTJgmpltjGXbHKlwvPzC82175162 = jTJgmpltjGXbHKlwvPzC98900779;     jTJgmpltjGXbHKlwvPzC98900779 = jTJgmpltjGXbHKlwvPzC18032930;     jTJgmpltjGXbHKlwvPzC18032930 = jTJgmpltjGXbHKlwvPzC71744556;     jTJgmpltjGXbHKlwvPzC71744556 = jTJgmpltjGXbHKlwvPzC7270609;     jTJgmpltjGXbHKlwvPzC7270609 = jTJgmpltjGXbHKlwvPzC42583314;     jTJgmpltjGXbHKlwvPzC42583314 = jTJgmpltjGXbHKlwvPzC92410977;     jTJgmpltjGXbHKlwvPzC92410977 = jTJgmpltjGXbHKlwvPzC39402427;     jTJgmpltjGXbHKlwvPzC39402427 = jTJgmpltjGXbHKlwvPzC73745757;     jTJgmpltjGXbHKlwvPzC73745757 = jTJgmpltjGXbHKlwvPzC62785658;     jTJgmpltjGXbHKlwvPzC62785658 = jTJgmpltjGXbHKlwvPzC96973888;     jTJgmpltjGXbHKlwvPzC96973888 = jTJgmpltjGXbHKlwvPzC76565896;     jTJgmpltjGXbHKlwvPzC76565896 = jTJgmpltjGXbHKlwvPzC97115495;     jTJgmpltjGXbHKlwvPzC97115495 = jTJgmpltjGXbHKlwvPzC34410068;     jTJgmpltjGXbHKlwvPzC34410068 = jTJgmpltjGXbHKlwvPzC15238356;     jTJgmpltjGXbHKlwvPzC15238356 = jTJgmpltjGXbHKlwvPzC41249931;     jTJgmpltjGXbHKlwvPzC41249931 = jTJgmpltjGXbHKlwvPzC41167492;     jTJgmpltjGXbHKlwvPzC41167492 = jTJgmpltjGXbHKlwvPzC18497890;     jTJgmpltjGXbHKlwvPzC18497890 = jTJgmpltjGXbHKlwvPzC10704509;     jTJgmpltjGXbHKlwvPzC10704509 = jTJgmpltjGXbHKlwvPzC83648174;     jTJgmpltjGXbHKlwvPzC83648174 = jTJgmpltjGXbHKlwvPzC82284350;     jTJgmpltjGXbHKlwvPzC82284350 = jTJgmpltjGXbHKlwvPzC16253981;     jTJgmpltjGXbHKlwvPzC16253981 = jTJgmpltjGXbHKlwvPzC87628943;     jTJgmpltjGXbHKlwvPzC87628943 = jTJgmpltjGXbHKlwvPzC33715471;     jTJgmpltjGXbHKlwvPzC33715471 = jTJgmpltjGXbHKlwvPzC69153397;     jTJgmpltjGXbHKlwvPzC69153397 = jTJgmpltjGXbHKlwvPzC80947363;     jTJgmpltjGXbHKlwvPzC80947363 = jTJgmpltjGXbHKlwvPzC39133597;     jTJgmpltjGXbHKlwvPzC39133597 = jTJgmpltjGXbHKlwvPzC64766464;     jTJgmpltjGXbHKlwvPzC64766464 = jTJgmpltjGXbHKlwvPzC76882807;     jTJgmpltjGXbHKlwvPzC76882807 = jTJgmpltjGXbHKlwvPzC99280803;     jTJgmpltjGXbHKlwvPzC99280803 = jTJgmpltjGXbHKlwvPzC33411199;     jTJgmpltjGXbHKlwvPzC33411199 = jTJgmpltjGXbHKlwvPzC18834116;     jTJgmpltjGXbHKlwvPzC18834116 = jTJgmpltjGXbHKlwvPzC62707007;     jTJgmpltjGXbHKlwvPzC62707007 = jTJgmpltjGXbHKlwvPzC76001342;     jTJgmpltjGXbHKlwvPzC76001342 = jTJgmpltjGXbHKlwvPzC16640898;     jTJgmpltjGXbHKlwvPzC16640898 = jTJgmpltjGXbHKlwvPzC43857987;     jTJgmpltjGXbHKlwvPzC43857987 = jTJgmpltjGXbHKlwvPzC1741370;     jTJgmpltjGXbHKlwvPzC1741370 = jTJgmpltjGXbHKlwvPzC33849354;     jTJgmpltjGXbHKlwvPzC33849354 = jTJgmpltjGXbHKlwvPzC41255132;     jTJgmpltjGXbHKlwvPzC41255132 = jTJgmpltjGXbHKlwvPzC28282753;     jTJgmpltjGXbHKlwvPzC28282753 = jTJgmpltjGXbHKlwvPzC87413847;     jTJgmpltjGXbHKlwvPzC87413847 = jTJgmpltjGXbHKlwvPzC85296777;     jTJgmpltjGXbHKlwvPzC85296777 = jTJgmpltjGXbHKlwvPzC16954843;     jTJgmpltjGXbHKlwvPzC16954843 = jTJgmpltjGXbHKlwvPzC23681608;     jTJgmpltjGXbHKlwvPzC23681608 = jTJgmpltjGXbHKlwvPzC6786299;     jTJgmpltjGXbHKlwvPzC6786299 = jTJgmpltjGXbHKlwvPzC9544127;     jTJgmpltjGXbHKlwvPzC9544127 = jTJgmpltjGXbHKlwvPzC12402763;     jTJgmpltjGXbHKlwvPzC12402763 = jTJgmpltjGXbHKlwvPzC21879210;     jTJgmpltjGXbHKlwvPzC21879210 = jTJgmpltjGXbHKlwvPzC3209157;     jTJgmpltjGXbHKlwvPzC3209157 = jTJgmpltjGXbHKlwvPzC71290631;     jTJgmpltjGXbHKlwvPzC71290631 = jTJgmpltjGXbHKlwvPzC48130131;     jTJgmpltjGXbHKlwvPzC48130131 = jTJgmpltjGXbHKlwvPzC42223550;     jTJgmpltjGXbHKlwvPzC42223550 = jTJgmpltjGXbHKlwvPzC15841995;     jTJgmpltjGXbHKlwvPzC15841995 = jTJgmpltjGXbHKlwvPzC62324183;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void RoARdbocFXyGrWwzjAeX79755367() {     float GgDlwVneLfzgEyFzqQlk37911078 = -113134144;    float GgDlwVneLfzgEyFzqQlk18672362 = -590538690;    float GgDlwVneLfzgEyFzqQlk99905380 = -363046394;    float GgDlwVneLfzgEyFzqQlk78624672 = -653197489;    float GgDlwVneLfzgEyFzqQlk82014104 = -552341413;    float GgDlwVneLfzgEyFzqQlk41534046 = -101620048;    float GgDlwVneLfzgEyFzqQlk21181133 = -541581385;    float GgDlwVneLfzgEyFzqQlk8730842 = -866690664;    float GgDlwVneLfzgEyFzqQlk1405654 = -988722881;    float GgDlwVneLfzgEyFzqQlk87022134 = 87356009;    float GgDlwVneLfzgEyFzqQlk90823978 = 48554625;    float GgDlwVneLfzgEyFzqQlk95217736 = -636720260;    float GgDlwVneLfzgEyFzqQlk25601104 = -174296243;    float GgDlwVneLfzgEyFzqQlk10053244 = -185788254;    float GgDlwVneLfzgEyFzqQlk69072071 = -665612069;    float GgDlwVneLfzgEyFzqQlk51555565 = -882531690;    float GgDlwVneLfzgEyFzqQlk41098346 = -498232101;    float GgDlwVneLfzgEyFzqQlk30831392 = -157415272;    float GgDlwVneLfzgEyFzqQlk76780528 = -997978764;    float GgDlwVneLfzgEyFzqQlk76357002 = -521762923;    float GgDlwVneLfzgEyFzqQlk70515279 = -77443873;    float GgDlwVneLfzgEyFzqQlk45507466 = -508657360;    float GgDlwVneLfzgEyFzqQlk88676837 = -137489346;    float GgDlwVneLfzgEyFzqQlk86182134 = -346328331;    float GgDlwVneLfzgEyFzqQlk25399279 = -582401992;    float GgDlwVneLfzgEyFzqQlk18112247 = -841178160;    float GgDlwVneLfzgEyFzqQlk88648573 = -934911227;    float GgDlwVneLfzgEyFzqQlk97806685 = -873429881;    float GgDlwVneLfzgEyFzqQlk74481900 = -59978016;    float GgDlwVneLfzgEyFzqQlk64893933 = -312921333;    float GgDlwVneLfzgEyFzqQlk62219205 = -70359777;    float GgDlwVneLfzgEyFzqQlk11875035 = -63182994;    float GgDlwVneLfzgEyFzqQlk29648771 = -669270947;    float GgDlwVneLfzgEyFzqQlk78215566 = -229061204;    float GgDlwVneLfzgEyFzqQlk21516079 = 84401731;    float GgDlwVneLfzgEyFzqQlk80552069 = 59071184;    float GgDlwVneLfzgEyFzqQlk75420231 = -72681719;    float GgDlwVneLfzgEyFzqQlk94203289 = -491451493;    float GgDlwVneLfzgEyFzqQlk91613437 = -813504952;    float GgDlwVneLfzgEyFzqQlk72104728 = -398772262;    float GgDlwVneLfzgEyFzqQlk68634876 = -563823475;    float GgDlwVneLfzgEyFzqQlk60125737 = -87427087;    float GgDlwVneLfzgEyFzqQlk48427002 = -781361144;    float GgDlwVneLfzgEyFzqQlk91706744 = -228625753;    float GgDlwVneLfzgEyFzqQlk47522866 = -628317488;    float GgDlwVneLfzgEyFzqQlk46193134 = -807157739;    float GgDlwVneLfzgEyFzqQlk84609293 = -47036925;    float GgDlwVneLfzgEyFzqQlk46442038 = -263454896;    float GgDlwVneLfzgEyFzqQlk20047502 = 70439450;    float GgDlwVneLfzgEyFzqQlk16831570 = -906056497;    float GgDlwVneLfzgEyFzqQlk28565962 = 96096817;    float GgDlwVneLfzgEyFzqQlk80892374 = -457939988;    float GgDlwVneLfzgEyFzqQlk16794831 = -870953138;    float GgDlwVneLfzgEyFzqQlk8747319 = -742295109;    float GgDlwVneLfzgEyFzqQlk9240089 = -68242976;    float GgDlwVneLfzgEyFzqQlk92403611 = -604476785;    float GgDlwVneLfzgEyFzqQlk29995524 = -353049344;    float GgDlwVneLfzgEyFzqQlk13723246 = 83281937;    float GgDlwVneLfzgEyFzqQlk53225394 = 29204503;    float GgDlwVneLfzgEyFzqQlk63901857 = -711163253;    float GgDlwVneLfzgEyFzqQlk52885473 = -166708822;    float GgDlwVneLfzgEyFzqQlk23374447 = -668151504;    float GgDlwVneLfzgEyFzqQlk34248942 = -706712648;    float GgDlwVneLfzgEyFzqQlk36511721 = -575801548;    float GgDlwVneLfzgEyFzqQlk24802929 = -842284215;    float GgDlwVneLfzgEyFzqQlk78948943 = -888262381;    float GgDlwVneLfzgEyFzqQlk65568965 = -967449313;    float GgDlwVneLfzgEyFzqQlk47385537 = -945235039;    float GgDlwVneLfzgEyFzqQlk88537164 = -170189986;    float GgDlwVneLfzgEyFzqQlk88520001 = -624683253;    float GgDlwVneLfzgEyFzqQlk76135334 = -709849971;    float GgDlwVneLfzgEyFzqQlk46895057 = 93219392;    float GgDlwVneLfzgEyFzqQlk39217955 = -343910321;    float GgDlwVneLfzgEyFzqQlk4675800 = -499206503;    float GgDlwVneLfzgEyFzqQlk7722126 = -957939449;    float GgDlwVneLfzgEyFzqQlk10389543 = -990016787;    float GgDlwVneLfzgEyFzqQlk97080464 = -727296217;    float GgDlwVneLfzgEyFzqQlk96970093 = -908863593;    float GgDlwVneLfzgEyFzqQlk38659268 = -718010844;    float GgDlwVneLfzgEyFzqQlk79206144 = -775244253;    float GgDlwVneLfzgEyFzqQlk33502954 = -694141235;    float GgDlwVneLfzgEyFzqQlk42206535 = -571456332;    float GgDlwVneLfzgEyFzqQlk77759183 = -843869331;    float GgDlwVneLfzgEyFzqQlk57650331 = -153921520;    float GgDlwVneLfzgEyFzqQlk36327972 = -309018151;    float GgDlwVneLfzgEyFzqQlk81326831 = -612419789;    float GgDlwVneLfzgEyFzqQlk95080204 = -192229857;    float GgDlwVneLfzgEyFzqQlk20901453 = -926975839;    float GgDlwVneLfzgEyFzqQlk68975478 = -60818229;    float GgDlwVneLfzgEyFzqQlk29112467 = -311121485;    float GgDlwVneLfzgEyFzqQlk50556546 = -587879472;    float GgDlwVneLfzgEyFzqQlk61696985 = -55963656;    float GgDlwVneLfzgEyFzqQlk40977896 = -420655996;    float GgDlwVneLfzgEyFzqQlk27711580 = -2341699;    float GgDlwVneLfzgEyFzqQlk19219256 = -132063441;    float GgDlwVneLfzgEyFzqQlk45260429 = -895671971;    float GgDlwVneLfzgEyFzqQlk25876795 = -380714439;    float GgDlwVneLfzgEyFzqQlk11915282 = -105559596;    float GgDlwVneLfzgEyFzqQlk66903815 = -386341539;    float GgDlwVneLfzgEyFzqQlk68573923 = -113134144;     GgDlwVneLfzgEyFzqQlk37911078 = GgDlwVneLfzgEyFzqQlk18672362;     GgDlwVneLfzgEyFzqQlk18672362 = GgDlwVneLfzgEyFzqQlk99905380;     GgDlwVneLfzgEyFzqQlk99905380 = GgDlwVneLfzgEyFzqQlk78624672;     GgDlwVneLfzgEyFzqQlk78624672 = GgDlwVneLfzgEyFzqQlk82014104;     GgDlwVneLfzgEyFzqQlk82014104 = GgDlwVneLfzgEyFzqQlk41534046;     GgDlwVneLfzgEyFzqQlk41534046 = GgDlwVneLfzgEyFzqQlk21181133;     GgDlwVneLfzgEyFzqQlk21181133 = GgDlwVneLfzgEyFzqQlk8730842;     GgDlwVneLfzgEyFzqQlk8730842 = GgDlwVneLfzgEyFzqQlk1405654;     GgDlwVneLfzgEyFzqQlk1405654 = GgDlwVneLfzgEyFzqQlk87022134;     GgDlwVneLfzgEyFzqQlk87022134 = GgDlwVneLfzgEyFzqQlk90823978;     GgDlwVneLfzgEyFzqQlk90823978 = GgDlwVneLfzgEyFzqQlk95217736;     GgDlwVneLfzgEyFzqQlk95217736 = GgDlwVneLfzgEyFzqQlk25601104;     GgDlwVneLfzgEyFzqQlk25601104 = GgDlwVneLfzgEyFzqQlk10053244;     GgDlwVneLfzgEyFzqQlk10053244 = GgDlwVneLfzgEyFzqQlk69072071;     GgDlwVneLfzgEyFzqQlk69072071 = GgDlwVneLfzgEyFzqQlk51555565;     GgDlwVneLfzgEyFzqQlk51555565 = GgDlwVneLfzgEyFzqQlk41098346;     GgDlwVneLfzgEyFzqQlk41098346 = GgDlwVneLfzgEyFzqQlk30831392;     GgDlwVneLfzgEyFzqQlk30831392 = GgDlwVneLfzgEyFzqQlk76780528;     GgDlwVneLfzgEyFzqQlk76780528 = GgDlwVneLfzgEyFzqQlk76357002;     GgDlwVneLfzgEyFzqQlk76357002 = GgDlwVneLfzgEyFzqQlk70515279;     GgDlwVneLfzgEyFzqQlk70515279 = GgDlwVneLfzgEyFzqQlk45507466;     GgDlwVneLfzgEyFzqQlk45507466 = GgDlwVneLfzgEyFzqQlk88676837;     GgDlwVneLfzgEyFzqQlk88676837 = GgDlwVneLfzgEyFzqQlk86182134;     GgDlwVneLfzgEyFzqQlk86182134 = GgDlwVneLfzgEyFzqQlk25399279;     GgDlwVneLfzgEyFzqQlk25399279 = GgDlwVneLfzgEyFzqQlk18112247;     GgDlwVneLfzgEyFzqQlk18112247 = GgDlwVneLfzgEyFzqQlk88648573;     GgDlwVneLfzgEyFzqQlk88648573 = GgDlwVneLfzgEyFzqQlk97806685;     GgDlwVneLfzgEyFzqQlk97806685 = GgDlwVneLfzgEyFzqQlk74481900;     GgDlwVneLfzgEyFzqQlk74481900 = GgDlwVneLfzgEyFzqQlk64893933;     GgDlwVneLfzgEyFzqQlk64893933 = GgDlwVneLfzgEyFzqQlk62219205;     GgDlwVneLfzgEyFzqQlk62219205 = GgDlwVneLfzgEyFzqQlk11875035;     GgDlwVneLfzgEyFzqQlk11875035 = GgDlwVneLfzgEyFzqQlk29648771;     GgDlwVneLfzgEyFzqQlk29648771 = GgDlwVneLfzgEyFzqQlk78215566;     GgDlwVneLfzgEyFzqQlk78215566 = GgDlwVneLfzgEyFzqQlk21516079;     GgDlwVneLfzgEyFzqQlk21516079 = GgDlwVneLfzgEyFzqQlk80552069;     GgDlwVneLfzgEyFzqQlk80552069 = GgDlwVneLfzgEyFzqQlk75420231;     GgDlwVneLfzgEyFzqQlk75420231 = GgDlwVneLfzgEyFzqQlk94203289;     GgDlwVneLfzgEyFzqQlk94203289 = GgDlwVneLfzgEyFzqQlk91613437;     GgDlwVneLfzgEyFzqQlk91613437 = GgDlwVneLfzgEyFzqQlk72104728;     GgDlwVneLfzgEyFzqQlk72104728 = GgDlwVneLfzgEyFzqQlk68634876;     GgDlwVneLfzgEyFzqQlk68634876 = GgDlwVneLfzgEyFzqQlk60125737;     GgDlwVneLfzgEyFzqQlk60125737 = GgDlwVneLfzgEyFzqQlk48427002;     GgDlwVneLfzgEyFzqQlk48427002 = GgDlwVneLfzgEyFzqQlk91706744;     GgDlwVneLfzgEyFzqQlk91706744 = GgDlwVneLfzgEyFzqQlk47522866;     GgDlwVneLfzgEyFzqQlk47522866 = GgDlwVneLfzgEyFzqQlk46193134;     GgDlwVneLfzgEyFzqQlk46193134 = GgDlwVneLfzgEyFzqQlk84609293;     GgDlwVneLfzgEyFzqQlk84609293 = GgDlwVneLfzgEyFzqQlk46442038;     GgDlwVneLfzgEyFzqQlk46442038 = GgDlwVneLfzgEyFzqQlk20047502;     GgDlwVneLfzgEyFzqQlk20047502 = GgDlwVneLfzgEyFzqQlk16831570;     GgDlwVneLfzgEyFzqQlk16831570 = GgDlwVneLfzgEyFzqQlk28565962;     GgDlwVneLfzgEyFzqQlk28565962 = GgDlwVneLfzgEyFzqQlk80892374;     GgDlwVneLfzgEyFzqQlk80892374 = GgDlwVneLfzgEyFzqQlk16794831;     GgDlwVneLfzgEyFzqQlk16794831 = GgDlwVneLfzgEyFzqQlk8747319;     GgDlwVneLfzgEyFzqQlk8747319 = GgDlwVneLfzgEyFzqQlk9240089;     GgDlwVneLfzgEyFzqQlk9240089 = GgDlwVneLfzgEyFzqQlk92403611;     GgDlwVneLfzgEyFzqQlk92403611 = GgDlwVneLfzgEyFzqQlk29995524;     GgDlwVneLfzgEyFzqQlk29995524 = GgDlwVneLfzgEyFzqQlk13723246;     GgDlwVneLfzgEyFzqQlk13723246 = GgDlwVneLfzgEyFzqQlk53225394;     GgDlwVneLfzgEyFzqQlk53225394 = GgDlwVneLfzgEyFzqQlk63901857;     GgDlwVneLfzgEyFzqQlk63901857 = GgDlwVneLfzgEyFzqQlk52885473;     GgDlwVneLfzgEyFzqQlk52885473 = GgDlwVneLfzgEyFzqQlk23374447;     GgDlwVneLfzgEyFzqQlk23374447 = GgDlwVneLfzgEyFzqQlk34248942;     GgDlwVneLfzgEyFzqQlk34248942 = GgDlwVneLfzgEyFzqQlk36511721;     GgDlwVneLfzgEyFzqQlk36511721 = GgDlwVneLfzgEyFzqQlk24802929;     GgDlwVneLfzgEyFzqQlk24802929 = GgDlwVneLfzgEyFzqQlk78948943;     GgDlwVneLfzgEyFzqQlk78948943 = GgDlwVneLfzgEyFzqQlk65568965;     GgDlwVneLfzgEyFzqQlk65568965 = GgDlwVneLfzgEyFzqQlk47385537;     GgDlwVneLfzgEyFzqQlk47385537 = GgDlwVneLfzgEyFzqQlk88537164;     GgDlwVneLfzgEyFzqQlk88537164 = GgDlwVneLfzgEyFzqQlk88520001;     GgDlwVneLfzgEyFzqQlk88520001 = GgDlwVneLfzgEyFzqQlk76135334;     GgDlwVneLfzgEyFzqQlk76135334 = GgDlwVneLfzgEyFzqQlk46895057;     GgDlwVneLfzgEyFzqQlk46895057 = GgDlwVneLfzgEyFzqQlk39217955;     GgDlwVneLfzgEyFzqQlk39217955 = GgDlwVneLfzgEyFzqQlk4675800;     GgDlwVneLfzgEyFzqQlk4675800 = GgDlwVneLfzgEyFzqQlk7722126;     GgDlwVneLfzgEyFzqQlk7722126 = GgDlwVneLfzgEyFzqQlk10389543;     GgDlwVneLfzgEyFzqQlk10389543 = GgDlwVneLfzgEyFzqQlk97080464;     GgDlwVneLfzgEyFzqQlk97080464 = GgDlwVneLfzgEyFzqQlk96970093;     GgDlwVneLfzgEyFzqQlk96970093 = GgDlwVneLfzgEyFzqQlk38659268;     GgDlwVneLfzgEyFzqQlk38659268 = GgDlwVneLfzgEyFzqQlk79206144;     GgDlwVneLfzgEyFzqQlk79206144 = GgDlwVneLfzgEyFzqQlk33502954;     GgDlwVneLfzgEyFzqQlk33502954 = GgDlwVneLfzgEyFzqQlk42206535;     GgDlwVneLfzgEyFzqQlk42206535 = GgDlwVneLfzgEyFzqQlk77759183;     GgDlwVneLfzgEyFzqQlk77759183 = GgDlwVneLfzgEyFzqQlk57650331;     GgDlwVneLfzgEyFzqQlk57650331 = GgDlwVneLfzgEyFzqQlk36327972;     GgDlwVneLfzgEyFzqQlk36327972 = GgDlwVneLfzgEyFzqQlk81326831;     GgDlwVneLfzgEyFzqQlk81326831 = GgDlwVneLfzgEyFzqQlk95080204;     GgDlwVneLfzgEyFzqQlk95080204 = GgDlwVneLfzgEyFzqQlk20901453;     GgDlwVneLfzgEyFzqQlk20901453 = GgDlwVneLfzgEyFzqQlk68975478;     GgDlwVneLfzgEyFzqQlk68975478 = GgDlwVneLfzgEyFzqQlk29112467;     GgDlwVneLfzgEyFzqQlk29112467 = GgDlwVneLfzgEyFzqQlk50556546;     GgDlwVneLfzgEyFzqQlk50556546 = GgDlwVneLfzgEyFzqQlk61696985;     GgDlwVneLfzgEyFzqQlk61696985 = GgDlwVneLfzgEyFzqQlk40977896;     GgDlwVneLfzgEyFzqQlk40977896 = GgDlwVneLfzgEyFzqQlk27711580;     GgDlwVneLfzgEyFzqQlk27711580 = GgDlwVneLfzgEyFzqQlk19219256;     GgDlwVneLfzgEyFzqQlk19219256 = GgDlwVneLfzgEyFzqQlk45260429;     GgDlwVneLfzgEyFzqQlk45260429 = GgDlwVneLfzgEyFzqQlk25876795;     GgDlwVneLfzgEyFzqQlk25876795 = GgDlwVneLfzgEyFzqQlk11915282;     GgDlwVneLfzgEyFzqQlk11915282 = GgDlwVneLfzgEyFzqQlk66903815;     GgDlwVneLfzgEyFzqQlk66903815 = GgDlwVneLfzgEyFzqQlk68573923;     GgDlwVneLfzgEyFzqQlk68573923 = GgDlwVneLfzgEyFzqQlk37911078;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LeaywVVfKPVWSEqefDdw34171581() {     float ENssZwvPqTWggvZjFtYP8562746 = -731283275;    float ENssZwvPqTWggvZjFtYP87345878 = -462181989;    float ENssZwvPqTWggvZjFtYP79874299 = -805340853;    float ENssZwvPqTWggvZjFtYP18555959 = -843335878;    float ENssZwvPqTWggvZjFtYP3464426 = -335747760;    float ENssZwvPqTWggvZjFtYP31533808 = -903201200;    float ENssZwvPqTWggvZjFtYP39560029 = -540357095;    float ENssZwvPqTWggvZjFtYP65573222 = -113507754;    float ENssZwvPqTWggvZjFtYP47270708 = 17929791;    float ENssZwvPqTWggvZjFtYP9988595 = -213739341;    float ENssZwvPqTWggvZjFtYP33073139 = 13652696;    float ENssZwvPqTWggvZjFtYP40764308 = -952484053;    float ENssZwvPqTWggvZjFtYP5428491 = -83833302;    float ENssZwvPqTWggvZjFtYP93900277 = 13183536;    float ENssZwvPqTWggvZjFtYP51792090 = -815362072;    float ENssZwvPqTWggvZjFtYP84084205 = -452515277;    float ENssZwvPqTWggvZjFtYP34951173 = -407417230;    float ENssZwvPqTWggvZjFtYP51124886 = -146581468;    float ENssZwvPqTWggvZjFtYP58488310 = -605166706;    float ENssZwvPqTWggvZjFtYP35143711 = 35870772;    float ENssZwvPqTWggvZjFtYP25651034 = 13876987;    float ENssZwvPqTWggvZjFtYP6903232 = -839942666;    float ENssZwvPqTWggvZjFtYP90620646 = -705332490;    float ENssZwvPqTWggvZjFtYP20031672 = 61270763;    float ENssZwvPqTWggvZjFtYP89523666 = -258306680;    float ENssZwvPqTWggvZjFtYP48709828 = -939462605;    float ENssZwvPqTWggvZjFtYP96909358 = -230089002;    float ENssZwvPqTWggvZjFtYP5588911 = -278553648;    float ENssZwvPqTWggvZjFtYP6624388 = -354336233;    float ENssZwvPqTWggvZjFtYP4066030 = -871316403;    float ENssZwvPqTWggvZjFtYP69291202 = -918793462;    float ENssZwvPqTWggvZjFtYP56386496 = -391480157;    float ENssZwvPqTWggvZjFtYP12594132 = -616232556;    float ENssZwvPqTWggvZjFtYP43580542 = -892590889;    float ENssZwvPqTWggvZjFtYP99540315 = -862545048;    float ENssZwvPqTWggvZjFtYP40545329 = -20155364;    float ENssZwvPqTWggvZjFtYP19211717 = -261274926;    float ENssZwvPqTWggvZjFtYP46995778 = -275564601;    float ENssZwvPqTWggvZjFtYP70146726 = -769233588;    float ENssZwvPqTWggvZjFtYP67208027 = 83619375;    float ENssZwvPqTWggvZjFtYP28708160 = -947842353;    float ENssZwvPqTWggvZjFtYP88608728 = -956816156;    float ENssZwvPqTWggvZjFtYP97393962 = -768115196;    float ENssZwvPqTWggvZjFtYP98215044 = -300600725;    float ENssZwvPqTWggvZjFtYP43476925 = -936963882;    float ENssZwvPqTWggvZjFtYP52598969 = -565494894;    float ENssZwvPqTWggvZjFtYP10740496 = -163106724;    float ENssZwvPqTWggvZjFtYP12056248 = -715585191;    float ENssZwvPqTWggvZjFtYP25488955 = 84675480;    float ENssZwvPqTWggvZjFtYP25259056 = -89065376;    float ENssZwvPqTWggvZjFtYP96755285 = -550475304;    float ENssZwvPqTWggvZjFtYP22763879 = -427159710;    float ENssZwvPqTWggvZjFtYP71870487 = -798751180;    float ENssZwvPqTWggvZjFtYP98974623 = -303176605;    float ENssZwvPqTWggvZjFtYP8520548 = -179183890;    float ENssZwvPqTWggvZjFtYP1659515 = -891340610;    float ENssZwvPqTWggvZjFtYP96725232 = -756849499;    float ENssZwvPqTWggvZjFtYP59842628 = -766611617;    float ENssZwvPqTWggvZjFtYP29032292 = -485029199;    float ENssZwvPqTWggvZjFtYP54754598 = -396285156;    float ENssZwvPqTWggvZjFtYP34624450 = -573112198;    float ENssZwvPqTWggvZjFtYP33971118 = -161803447;    float ENssZwvPqTWggvZjFtYP58948835 = -759171521;    float ENssZwvPqTWggvZjFtYP43204679 = -110753806;    float ENssZwvPqTWggvZjFtYP40697393 = -294945880;    float ENssZwvPqTWggvZjFtYP76686643 = -594867147;    float ENssZwvPqTWggvZjFtYP28170176 = -236251497;    float ENssZwvPqTWggvZjFtYP61847949 = -191242413;    float ENssZwvPqTWggvZjFtYP94359961 = -124271417;    float ENssZwvPqTWggvZjFtYP11246762 = -695206709;    float ENssZwvPqTWggvZjFtYP64872488 = -91240351;    float ENssZwvPqTWggvZjFtYP87955395 = -31852630;    float ENssZwvPqTWggvZjFtYP80978160 = -377347880;    float ENssZwvPqTWggvZjFtYP91280283 = -588786082;    float ENssZwvPqTWggvZjFtYP6435552 = -16286875;    float ENssZwvPqTWggvZjFtYP37042306 = -29306857;    float ENssZwvPqTWggvZjFtYP9509269 = 28172529;    float ENssZwvPqTWggvZjFtYP92405601 = -304731766;    float ENssZwvPqTWggvZjFtYP76554746 = -1765355;    float ENssZwvPqTWggvZjFtYP36924698 = -692811786;    float ENssZwvPqTWggvZjFtYP37969332 = -676355881;    float ENssZwvPqTWggvZjFtYP84853111 = -514503812;    float ENssZwvPqTWggvZjFtYP80099956 = -263229129;    float ENssZwvPqTWggvZjFtYP81365332 = -165270858;    float ENssZwvPqTWggvZjFtYP7310744 = -220841100;    float ENssZwvPqTWggvZjFtYP46527323 = -391633753;    float ENssZwvPqTWggvZjFtYP84516008 = -592728978;    float ENssZwvPqTWggvZjFtYP13619509 = -213055951;    float ENssZwvPqTWggvZjFtYP35059994 = -613406999;    float ENssZwvPqTWggvZjFtYP97880801 = -971204439;    float ENssZwvPqTWggvZjFtYP43820096 = -263305866;    float ENssZwvPqTWggvZjFtYP59369089 = -494663310;    float ENssZwvPqTWggvZjFtYP17963486 = -790535403;    float ENssZwvPqTWggvZjFtYP15392128 = -272948433;    float ENssZwvPqTWggvZjFtYP32583577 = -343268427;    float ENssZwvPqTWggvZjFtYP94737042 = -686038907;    float ENssZwvPqTWggvZjFtYP29659893 = -97644636;    float ENssZwvPqTWggvZjFtYP54189283 = -557361390;    float ENssZwvPqTWggvZjFtYP57517652 = 94345154;    float ENssZwvPqTWggvZjFtYP66790281 = -731283275;     ENssZwvPqTWggvZjFtYP8562746 = ENssZwvPqTWggvZjFtYP87345878;     ENssZwvPqTWggvZjFtYP87345878 = ENssZwvPqTWggvZjFtYP79874299;     ENssZwvPqTWggvZjFtYP79874299 = ENssZwvPqTWggvZjFtYP18555959;     ENssZwvPqTWggvZjFtYP18555959 = ENssZwvPqTWggvZjFtYP3464426;     ENssZwvPqTWggvZjFtYP3464426 = ENssZwvPqTWggvZjFtYP31533808;     ENssZwvPqTWggvZjFtYP31533808 = ENssZwvPqTWggvZjFtYP39560029;     ENssZwvPqTWggvZjFtYP39560029 = ENssZwvPqTWggvZjFtYP65573222;     ENssZwvPqTWggvZjFtYP65573222 = ENssZwvPqTWggvZjFtYP47270708;     ENssZwvPqTWggvZjFtYP47270708 = ENssZwvPqTWggvZjFtYP9988595;     ENssZwvPqTWggvZjFtYP9988595 = ENssZwvPqTWggvZjFtYP33073139;     ENssZwvPqTWggvZjFtYP33073139 = ENssZwvPqTWggvZjFtYP40764308;     ENssZwvPqTWggvZjFtYP40764308 = ENssZwvPqTWggvZjFtYP5428491;     ENssZwvPqTWggvZjFtYP5428491 = ENssZwvPqTWggvZjFtYP93900277;     ENssZwvPqTWggvZjFtYP93900277 = ENssZwvPqTWggvZjFtYP51792090;     ENssZwvPqTWggvZjFtYP51792090 = ENssZwvPqTWggvZjFtYP84084205;     ENssZwvPqTWggvZjFtYP84084205 = ENssZwvPqTWggvZjFtYP34951173;     ENssZwvPqTWggvZjFtYP34951173 = ENssZwvPqTWggvZjFtYP51124886;     ENssZwvPqTWggvZjFtYP51124886 = ENssZwvPqTWggvZjFtYP58488310;     ENssZwvPqTWggvZjFtYP58488310 = ENssZwvPqTWggvZjFtYP35143711;     ENssZwvPqTWggvZjFtYP35143711 = ENssZwvPqTWggvZjFtYP25651034;     ENssZwvPqTWggvZjFtYP25651034 = ENssZwvPqTWggvZjFtYP6903232;     ENssZwvPqTWggvZjFtYP6903232 = ENssZwvPqTWggvZjFtYP90620646;     ENssZwvPqTWggvZjFtYP90620646 = ENssZwvPqTWggvZjFtYP20031672;     ENssZwvPqTWggvZjFtYP20031672 = ENssZwvPqTWggvZjFtYP89523666;     ENssZwvPqTWggvZjFtYP89523666 = ENssZwvPqTWggvZjFtYP48709828;     ENssZwvPqTWggvZjFtYP48709828 = ENssZwvPqTWggvZjFtYP96909358;     ENssZwvPqTWggvZjFtYP96909358 = ENssZwvPqTWggvZjFtYP5588911;     ENssZwvPqTWggvZjFtYP5588911 = ENssZwvPqTWggvZjFtYP6624388;     ENssZwvPqTWggvZjFtYP6624388 = ENssZwvPqTWggvZjFtYP4066030;     ENssZwvPqTWggvZjFtYP4066030 = ENssZwvPqTWggvZjFtYP69291202;     ENssZwvPqTWggvZjFtYP69291202 = ENssZwvPqTWggvZjFtYP56386496;     ENssZwvPqTWggvZjFtYP56386496 = ENssZwvPqTWggvZjFtYP12594132;     ENssZwvPqTWggvZjFtYP12594132 = ENssZwvPqTWggvZjFtYP43580542;     ENssZwvPqTWggvZjFtYP43580542 = ENssZwvPqTWggvZjFtYP99540315;     ENssZwvPqTWggvZjFtYP99540315 = ENssZwvPqTWggvZjFtYP40545329;     ENssZwvPqTWggvZjFtYP40545329 = ENssZwvPqTWggvZjFtYP19211717;     ENssZwvPqTWggvZjFtYP19211717 = ENssZwvPqTWggvZjFtYP46995778;     ENssZwvPqTWggvZjFtYP46995778 = ENssZwvPqTWggvZjFtYP70146726;     ENssZwvPqTWggvZjFtYP70146726 = ENssZwvPqTWggvZjFtYP67208027;     ENssZwvPqTWggvZjFtYP67208027 = ENssZwvPqTWggvZjFtYP28708160;     ENssZwvPqTWggvZjFtYP28708160 = ENssZwvPqTWggvZjFtYP88608728;     ENssZwvPqTWggvZjFtYP88608728 = ENssZwvPqTWggvZjFtYP97393962;     ENssZwvPqTWggvZjFtYP97393962 = ENssZwvPqTWggvZjFtYP98215044;     ENssZwvPqTWggvZjFtYP98215044 = ENssZwvPqTWggvZjFtYP43476925;     ENssZwvPqTWggvZjFtYP43476925 = ENssZwvPqTWggvZjFtYP52598969;     ENssZwvPqTWggvZjFtYP52598969 = ENssZwvPqTWggvZjFtYP10740496;     ENssZwvPqTWggvZjFtYP10740496 = ENssZwvPqTWggvZjFtYP12056248;     ENssZwvPqTWggvZjFtYP12056248 = ENssZwvPqTWggvZjFtYP25488955;     ENssZwvPqTWggvZjFtYP25488955 = ENssZwvPqTWggvZjFtYP25259056;     ENssZwvPqTWggvZjFtYP25259056 = ENssZwvPqTWggvZjFtYP96755285;     ENssZwvPqTWggvZjFtYP96755285 = ENssZwvPqTWggvZjFtYP22763879;     ENssZwvPqTWggvZjFtYP22763879 = ENssZwvPqTWggvZjFtYP71870487;     ENssZwvPqTWggvZjFtYP71870487 = ENssZwvPqTWggvZjFtYP98974623;     ENssZwvPqTWggvZjFtYP98974623 = ENssZwvPqTWggvZjFtYP8520548;     ENssZwvPqTWggvZjFtYP8520548 = ENssZwvPqTWggvZjFtYP1659515;     ENssZwvPqTWggvZjFtYP1659515 = ENssZwvPqTWggvZjFtYP96725232;     ENssZwvPqTWggvZjFtYP96725232 = ENssZwvPqTWggvZjFtYP59842628;     ENssZwvPqTWggvZjFtYP59842628 = ENssZwvPqTWggvZjFtYP29032292;     ENssZwvPqTWggvZjFtYP29032292 = ENssZwvPqTWggvZjFtYP54754598;     ENssZwvPqTWggvZjFtYP54754598 = ENssZwvPqTWggvZjFtYP34624450;     ENssZwvPqTWggvZjFtYP34624450 = ENssZwvPqTWggvZjFtYP33971118;     ENssZwvPqTWggvZjFtYP33971118 = ENssZwvPqTWggvZjFtYP58948835;     ENssZwvPqTWggvZjFtYP58948835 = ENssZwvPqTWggvZjFtYP43204679;     ENssZwvPqTWggvZjFtYP43204679 = ENssZwvPqTWggvZjFtYP40697393;     ENssZwvPqTWggvZjFtYP40697393 = ENssZwvPqTWggvZjFtYP76686643;     ENssZwvPqTWggvZjFtYP76686643 = ENssZwvPqTWggvZjFtYP28170176;     ENssZwvPqTWggvZjFtYP28170176 = ENssZwvPqTWggvZjFtYP61847949;     ENssZwvPqTWggvZjFtYP61847949 = ENssZwvPqTWggvZjFtYP94359961;     ENssZwvPqTWggvZjFtYP94359961 = ENssZwvPqTWggvZjFtYP11246762;     ENssZwvPqTWggvZjFtYP11246762 = ENssZwvPqTWggvZjFtYP64872488;     ENssZwvPqTWggvZjFtYP64872488 = ENssZwvPqTWggvZjFtYP87955395;     ENssZwvPqTWggvZjFtYP87955395 = ENssZwvPqTWggvZjFtYP80978160;     ENssZwvPqTWggvZjFtYP80978160 = ENssZwvPqTWggvZjFtYP91280283;     ENssZwvPqTWggvZjFtYP91280283 = ENssZwvPqTWggvZjFtYP6435552;     ENssZwvPqTWggvZjFtYP6435552 = ENssZwvPqTWggvZjFtYP37042306;     ENssZwvPqTWggvZjFtYP37042306 = ENssZwvPqTWggvZjFtYP9509269;     ENssZwvPqTWggvZjFtYP9509269 = ENssZwvPqTWggvZjFtYP92405601;     ENssZwvPqTWggvZjFtYP92405601 = ENssZwvPqTWggvZjFtYP76554746;     ENssZwvPqTWggvZjFtYP76554746 = ENssZwvPqTWggvZjFtYP36924698;     ENssZwvPqTWggvZjFtYP36924698 = ENssZwvPqTWggvZjFtYP37969332;     ENssZwvPqTWggvZjFtYP37969332 = ENssZwvPqTWggvZjFtYP84853111;     ENssZwvPqTWggvZjFtYP84853111 = ENssZwvPqTWggvZjFtYP80099956;     ENssZwvPqTWggvZjFtYP80099956 = ENssZwvPqTWggvZjFtYP81365332;     ENssZwvPqTWggvZjFtYP81365332 = ENssZwvPqTWggvZjFtYP7310744;     ENssZwvPqTWggvZjFtYP7310744 = ENssZwvPqTWggvZjFtYP46527323;     ENssZwvPqTWggvZjFtYP46527323 = ENssZwvPqTWggvZjFtYP84516008;     ENssZwvPqTWggvZjFtYP84516008 = ENssZwvPqTWggvZjFtYP13619509;     ENssZwvPqTWggvZjFtYP13619509 = ENssZwvPqTWggvZjFtYP35059994;     ENssZwvPqTWggvZjFtYP35059994 = ENssZwvPqTWggvZjFtYP97880801;     ENssZwvPqTWggvZjFtYP97880801 = ENssZwvPqTWggvZjFtYP43820096;     ENssZwvPqTWggvZjFtYP43820096 = ENssZwvPqTWggvZjFtYP59369089;     ENssZwvPqTWggvZjFtYP59369089 = ENssZwvPqTWggvZjFtYP17963486;     ENssZwvPqTWggvZjFtYP17963486 = ENssZwvPqTWggvZjFtYP15392128;     ENssZwvPqTWggvZjFtYP15392128 = ENssZwvPqTWggvZjFtYP32583577;     ENssZwvPqTWggvZjFtYP32583577 = ENssZwvPqTWggvZjFtYP94737042;     ENssZwvPqTWggvZjFtYP94737042 = ENssZwvPqTWggvZjFtYP29659893;     ENssZwvPqTWggvZjFtYP29659893 = ENssZwvPqTWggvZjFtYP54189283;     ENssZwvPqTWggvZjFtYP54189283 = ENssZwvPqTWggvZjFtYP57517652;     ENssZwvPqTWggvZjFtYP57517652 = ENssZwvPqTWggvZjFtYP66790281;     ENssZwvPqTWggvZjFtYP66790281 = ENssZwvPqTWggvZjFtYP8562746;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IYpVRCxKvrtYoVRUoDvH40830326() {     float KMuHjUJxxXdutzrUpfwZ8556274 = -603807416;    float KMuHjUJxxXdutzrUpfwZ25918196 = -504777114;    float KMuHjUJxxXdutzrUpfwZ81187511 = -762469467;    float KMuHjUJxxXdutzrUpfwZ3635820 = -663876708;    float KMuHjUJxxXdutzrUpfwZ12304528 = -804103082;    float KMuHjUJxxXdutzrUpfwZ66339240 = -602634011;    float KMuHjUJxxXdutzrUpfwZ8107365 = -244893562;    float KMuHjUJxxXdutzrUpfwZ34787273 = -917685071;    float KMuHjUJxxXdutzrUpfwZ30010873 = -795716279;    float KMuHjUJxxXdutzrUpfwZ6628109 = -950546212;    float KMuHjUJxxXdutzrUpfwZ53393744 = 54180624;    float KMuHjUJxxXdutzrUpfwZ55055985 = -687962415;    float KMuHjUJxxXdutzrUpfwZ37281405 = -105491776;    float KMuHjUJxxXdutzrUpfwZ47744497 = -331969516;    float KMuHjUJxxXdutzrUpfwZ96964199 = -778074924;    float KMuHjUJxxXdutzrUpfwZ58648752 = -785310927;    float KMuHjUJxxXdutzrUpfwZ90803932 = -227031800;    float KMuHjUJxxXdutzrUpfwZ74125419 = -543653090;    float KMuHjUJxxXdutzrUpfwZ78916580 = -123148973;    float KMuHjUJxxXdutzrUpfwZ7217475 = -576153679;    float KMuHjUJxxXdutzrUpfwZ5320458 = -796589387;    float KMuHjUJxxXdutzrUpfwZ85199497 = -249934549;    float KMuHjUJxxXdutzrUpfwZ27499043 = -64290332;    float KMuHjUJxxXdutzrUpfwZ63681780 = -919649760;    float KMuHjUJxxXdutzrUpfwZ17966357 = -685000533;    float KMuHjUJxxXdutzrUpfwZ65283905 = -847668533;    float KMuHjUJxxXdutzrUpfwZ67130940 = -530888715;    float KMuHjUJxxXdutzrUpfwZ40992436 = -887520637;    float KMuHjUJxxXdutzrUpfwZ19302835 = 99593452;    float KMuHjUJxxXdutzrUpfwZ80155899 = -546747777;    float KMuHjUJxxXdutzrUpfwZ2703881 = -205886697;    float KMuHjUJxxXdutzrUpfwZ41110353 = -783301919;    float KMuHjUJxxXdutzrUpfwZ98046555 = -818176772;    float KMuHjUJxxXdutzrUpfwZ43863379 = -769272621;    float KMuHjUJxxXdutzrUpfwZ81898353 = -21858973;    float KMuHjUJxxXdutzrUpfwZ83373518 = -99921110;    float KMuHjUJxxXdutzrUpfwZ31153085 = -360928305;    float KMuHjUJxxXdutzrUpfwZ16913458 = -270827280;    float KMuHjUJxxXdutzrUpfwZ5460323 = -381915005;    float KMuHjUJxxXdutzrUpfwZ39080920 = -625812311;    float KMuHjUJxxXdutzrUpfwZ16563828 = 64213748;    float KMuHjUJxxXdutzrUpfwZ74306389 = -50608239;    float KMuHjUJxxXdutzrUpfwZ57677322 = -227245450;    float KMuHjUJxxXdutzrUpfwZ57446943 = -957565168;    float KMuHjUJxxXdutzrUpfwZ27494978 = -310223284;    float KMuHjUJxxXdutzrUpfwZ93026937 = -377539565;    float KMuHjUJxxXdutzrUpfwZ38438289 = -644387347;    float KMuHjUJxxXdutzrUpfwZ87439828 = -224346547;    float KMuHjUJxxXdutzrUpfwZ44147344 = -579088381;    float KMuHjUJxxXdutzrUpfwZ45304479 = -776090262;    float KMuHjUJxxXdutzrUpfwZ10014656 = -129030311;    float KMuHjUJxxXdutzrUpfwZ37311136 = -809305458;    float KMuHjUJxxXdutzrUpfwZ47318836 = -690805188;    float KMuHjUJxxXdutzrUpfwZ9224894 = -835096457;    float KMuHjUJxxXdutzrUpfwZ35509868 = 63916832;    float KMuHjUJxxXdutzrUpfwZ23356777 = -253872868;    float KMuHjUJxxXdutzrUpfwZ98419152 = -340486782;    float KMuHjUJxxXdutzrUpfwZ17505732 = -842819708;    float KMuHjUJxxXdutzrUpfwZ85669463 = -978876176;    float KMuHjUJxxXdutzrUpfwZ47020623 = -956434549;    float KMuHjUJxxXdutzrUpfwZ99208300 = 28254703;    float KMuHjUJxxXdutzrUpfwZ67114928 = -357372925;    float KMuHjUJxxXdutzrUpfwZ15484439 = -917278523;    float KMuHjUJxxXdutzrUpfwZ49854973 = -148968502;    float KMuHjUJxxXdutzrUpfwZ3924229 = -644659516;    float KMuHjUJxxXdutzrUpfwZ12283392 = -162517458;    float KMuHjUJxxXdutzrUpfwZ57009429 = -869785644;    float KMuHjUJxxXdutzrUpfwZ93418026 = -336219155;    float KMuHjUJxxXdutzrUpfwZ65846144 = -210110543;    float KMuHjUJxxXdutzrUpfwZ13590681 = -578153814;    float KMuHjUJxxXdutzrUpfwZ27495668 = -324382623;    float KMuHjUJxxXdutzrUpfwZ73890475 = -956204521;    float KMuHjUJxxXdutzrUpfwZ68665096 = -61738086;    float KMuHjUJxxXdutzrUpfwZ39835660 = -497336663;    float KMuHjUJxxXdutzrUpfwZ90653646 = -540367428;    float KMuHjUJxxXdutzrUpfwZ31014069 = -645981149;    float KMuHjUJxxXdutzrUpfwZ27522175 = 77310900;    float KMuHjUJxxXdutzrUpfwZ70052099 = -106725165;    float KMuHjUJxxXdutzrUpfwZ36186802 = -509426476;    float KMuHjUJxxXdutzrUpfwZ24939419 = -207460969;    float KMuHjUJxxXdutzrUpfwZ26845616 = -103281187;    float KMuHjUJxxXdutzrUpfwZ79691111 = -206542168;    float KMuHjUJxxXdutzrUpfwZ96845092 = -208432256;    float KMuHjUJxxXdutzrUpfwZ73998356 = -124316287;    float KMuHjUJxxXdutzrUpfwZ70141243 = -317717467;    float KMuHjUJxxXdutzrUpfwZ65392744 = -396581239;    float KMuHjUJxxXdutzrUpfwZ93791517 = 7503269;    float KMuHjUJxxXdutzrUpfwZ88821661 = -983080316;    float KMuHjUJxxXdutzrUpfwZ8353511 = -733189454;    float KMuHjUJxxXdutzrUpfwZ58541576 = -767986106;    float KMuHjUJxxXdutzrUpfwZ84954365 = -759434329;    float KMuHjUJxxXdutzrUpfwZ13647353 = -518108597;    float KMuHjUJxxXdutzrUpfwZ31243995 = -291951105;    float KMuHjUJxxXdutzrUpfwZ58439700 = -425480456;    float KMuHjUJxxXdutzrUpfwZ39872619 = -554067015;    float KMuHjUJxxXdutzrUpfwZ49448899 = -578413327;    float KMuHjUJxxXdutzrUpfwZ58821950 = -133329716;    float KMuHjUJxxXdutzrUpfwZ7822349 = 21723052;    float KMuHjUJxxXdutzrUpfwZ53522715 = -212905653;    float KMuHjUJxxXdutzrUpfwZ15211586 = -603807416;     KMuHjUJxxXdutzrUpfwZ8556274 = KMuHjUJxxXdutzrUpfwZ25918196;     KMuHjUJxxXdutzrUpfwZ25918196 = KMuHjUJxxXdutzrUpfwZ81187511;     KMuHjUJxxXdutzrUpfwZ81187511 = KMuHjUJxxXdutzrUpfwZ3635820;     KMuHjUJxxXdutzrUpfwZ3635820 = KMuHjUJxxXdutzrUpfwZ12304528;     KMuHjUJxxXdutzrUpfwZ12304528 = KMuHjUJxxXdutzrUpfwZ66339240;     KMuHjUJxxXdutzrUpfwZ66339240 = KMuHjUJxxXdutzrUpfwZ8107365;     KMuHjUJxxXdutzrUpfwZ8107365 = KMuHjUJxxXdutzrUpfwZ34787273;     KMuHjUJxxXdutzrUpfwZ34787273 = KMuHjUJxxXdutzrUpfwZ30010873;     KMuHjUJxxXdutzrUpfwZ30010873 = KMuHjUJxxXdutzrUpfwZ6628109;     KMuHjUJxxXdutzrUpfwZ6628109 = KMuHjUJxxXdutzrUpfwZ53393744;     KMuHjUJxxXdutzrUpfwZ53393744 = KMuHjUJxxXdutzrUpfwZ55055985;     KMuHjUJxxXdutzrUpfwZ55055985 = KMuHjUJxxXdutzrUpfwZ37281405;     KMuHjUJxxXdutzrUpfwZ37281405 = KMuHjUJxxXdutzrUpfwZ47744497;     KMuHjUJxxXdutzrUpfwZ47744497 = KMuHjUJxxXdutzrUpfwZ96964199;     KMuHjUJxxXdutzrUpfwZ96964199 = KMuHjUJxxXdutzrUpfwZ58648752;     KMuHjUJxxXdutzrUpfwZ58648752 = KMuHjUJxxXdutzrUpfwZ90803932;     KMuHjUJxxXdutzrUpfwZ90803932 = KMuHjUJxxXdutzrUpfwZ74125419;     KMuHjUJxxXdutzrUpfwZ74125419 = KMuHjUJxxXdutzrUpfwZ78916580;     KMuHjUJxxXdutzrUpfwZ78916580 = KMuHjUJxxXdutzrUpfwZ7217475;     KMuHjUJxxXdutzrUpfwZ7217475 = KMuHjUJxxXdutzrUpfwZ5320458;     KMuHjUJxxXdutzrUpfwZ5320458 = KMuHjUJxxXdutzrUpfwZ85199497;     KMuHjUJxxXdutzrUpfwZ85199497 = KMuHjUJxxXdutzrUpfwZ27499043;     KMuHjUJxxXdutzrUpfwZ27499043 = KMuHjUJxxXdutzrUpfwZ63681780;     KMuHjUJxxXdutzrUpfwZ63681780 = KMuHjUJxxXdutzrUpfwZ17966357;     KMuHjUJxxXdutzrUpfwZ17966357 = KMuHjUJxxXdutzrUpfwZ65283905;     KMuHjUJxxXdutzrUpfwZ65283905 = KMuHjUJxxXdutzrUpfwZ67130940;     KMuHjUJxxXdutzrUpfwZ67130940 = KMuHjUJxxXdutzrUpfwZ40992436;     KMuHjUJxxXdutzrUpfwZ40992436 = KMuHjUJxxXdutzrUpfwZ19302835;     KMuHjUJxxXdutzrUpfwZ19302835 = KMuHjUJxxXdutzrUpfwZ80155899;     KMuHjUJxxXdutzrUpfwZ80155899 = KMuHjUJxxXdutzrUpfwZ2703881;     KMuHjUJxxXdutzrUpfwZ2703881 = KMuHjUJxxXdutzrUpfwZ41110353;     KMuHjUJxxXdutzrUpfwZ41110353 = KMuHjUJxxXdutzrUpfwZ98046555;     KMuHjUJxxXdutzrUpfwZ98046555 = KMuHjUJxxXdutzrUpfwZ43863379;     KMuHjUJxxXdutzrUpfwZ43863379 = KMuHjUJxxXdutzrUpfwZ81898353;     KMuHjUJxxXdutzrUpfwZ81898353 = KMuHjUJxxXdutzrUpfwZ83373518;     KMuHjUJxxXdutzrUpfwZ83373518 = KMuHjUJxxXdutzrUpfwZ31153085;     KMuHjUJxxXdutzrUpfwZ31153085 = KMuHjUJxxXdutzrUpfwZ16913458;     KMuHjUJxxXdutzrUpfwZ16913458 = KMuHjUJxxXdutzrUpfwZ5460323;     KMuHjUJxxXdutzrUpfwZ5460323 = KMuHjUJxxXdutzrUpfwZ39080920;     KMuHjUJxxXdutzrUpfwZ39080920 = KMuHjUJxxXdutzrUpfwZ16563828;     KMuHjUJxxXdutzrUpfwZ16563828 = KMuHjUJxxXdutzrUpfwZ74306389;     KMuHjUJxxXdutzrUpfwZ74306389 = KMuHjUJxxXdutzrUpfwZ57677322;     KMuHjUJxxXdutzrUpfwZ57677322 = KMuHjUJxxXdutzrUpfwZ57446943;     KMuHjUJxxXdutzrUpfwZ57446943 = KMuHjUJxxXdutzrUpfwZ27494978;     KMuHjUJxxXdutzrUpfwZ27494978 = KMuHjUJxxXdutzrUpfwZ93026937;     KMuHjUJxxXdutzrUpfwZ93026937 = KMuHjUJxxXdutzrUpfwZ38438289;     KMuHjUJxxXdutzrUpfwZ38438289 = KMuHjUJxxXdutzrUpfwZ87439828;     KMuHjUJxxXdutzrUpfwZ87439828 = KMuHjUJxxXdutzrUpfwZ44147344;     KMuHjUJxxXdutzrUpfwZ44147344 = KMuHjUJxxXdutzrUpfwZ45304479;     KMuHjUJxxXdutzrUpfwZ45304479 = KMuHjUJxxXdutzrUpfwZ10014656;     KMuHjUJxxXdutzrUpfwZ10014656 = KMuHjUJxxXdutzrUpfwZ37311136;     KMuHjUJxxXdutzrUpfwZ37311136 = KMuHjUJxxXdutzrUpfwZ47318836;     KMuHjUJxxXdutzrUpfwZ47318836 = KMuHjUJxxXdutzrUpfwZ9224894;     KMuHjUJxxXdutzrUpfwZ9224894 = KMuHjUJxxXdutzrUpfwZ35509868;     KMuHjUJxxXdutzrUpfwZ35509868 = KMuHjUJxxXdutzrUpfwZ23356777;     KMuHjUJxxXdutzrUpfwZ23356777 = KMuHjUJxxXdutzrUpfwZ98419152;     KMuHjUJxxXdutzrUpfwZ98419152 = KMuHjUJxxXdutzrUpfwZ17505732;     KMuHjUJxxXdutzrUpfwZ17505732 = KMuHjUJxxXdutzrUpfwZ85669463;     KMuHjUJxxXdutzrUpfwZ85669463 = KMuHjUJxxXdutzrUpfwZ47020623;     KMuHjUJxxXdutzrUpfwZ47020623 = KMuHjUJxxXdutzrUpfwZ99208300;     KMuHjUJxxXdutzrUpfwZ99208300 = KMuHjUJxxXdutzrUpfwZ67114928;     KMuHjUJxxXdutzrUpfwZ67114928 = KMuHjUJxxXdutzrUpfwZ15484439;     KMuHjUJxxXdutzrUpfwZ15484439 = KMuHjUJxxXdutzrUpfwZ49854973;     KMuHjUJxxXdutzrUpfwZ49854973 = KMuHjUJxxXdutzrUpfwZ3924229;     KMuHjUJxxXdutzrUpfwZ3924229 = KMuHjUJxxXdutzrUpfwZ12283392;     KMuHjUJxxXdutzrUpfwZ12283392 = KMuHjUJxxXdutzrUpfwZ57009429;     KMuHjUJxxXdutzrUpfwZ57009429 = KMuHjUJxxXdutzrUpfwZ93418026;     KMuHjUJxxXdutzrUpfwZ93418026 = KMuHjUJxxXdutzrUpfwZ65846144;     KMuHjUJxxXdutzrUpfwZ65846144 = KMuHjUJxxXdutzrUpfwZ13590681;     KMuHjUJxxXdutzrUpfwZ13590681 = KMuHjUJxxXdutzrUpfwZ27495668;     KMuHjUJxxXdutzrUpfwZ27495668 = KMuHjUJxxXdutzrUpfwZ73890475;     KMuHjUJxxXdutzrUpfwZ73890475 = KMuHjUJxxXdutzrUpfwZ68665096;     KMuHjUJxxXdutzrUpfwZ68665096 = KMuHjUJxxXdutzrUpfwZ39835660;     KMuHjUJxxXdutzrUpfwZ39835660 = KMuHjUJxxXdutzrUpfwZ90653646;     KMuHjUJxxXdutzrUpfwZ90653646 = KMuHjUJxxXdutzrUpfwZ31014069;     KMuHjUJxxXdutzrUpfwZ31014069 = KMuHjUJxxXdutzrUpfwZ27522175;     KMuHjUJxxXdutzrUpfwZ27522175 = KMuHjUJxxXdutzrUpfwZ70052099;     KMuHjUJxxXdutzrUpfwZ70052099 = KMuHjUJxxXdutzrUpfwZ36186802;     KMuHjUJxxXdutzrUpfwZ36186802 = KMuHjUJxxXdutzrUpfwZ24939419;     KMuHjUJxxXdutzrUpfwZ24939419 = KMuHjUJxxXdutzrUpfwZ26845616;     KMuHjUJxxXdutzrUpfwZ26845616 = KMuHjUJxxXdutzrUpfwZ79691111;     KMuHjUJxxXdutzrUpfwZ79691111 = KMuHjUJxxXdutzrUpfwZ96845092;     KMuHjUJxxXdutzrUpfwZ96845092 = KMuHjUJxxXdutzrUpfwZ73998356;     KMuHjUJxxXdutzrUpfwZ73998356 = KMuHjUJxxXdutzrUpfwZ70141243;     KMuHjUJxxXdutzrUpfwZ70141243 = KMuHjUJxxXdutzrUpfwZ65392744;     KMuHjUJxxXdutzrUpfwZ65392744 = KMuHjUJxxXdutzrUpfwZ93791517;     KMuHjUJxxXdutzrUpfwZ93791517 = KMuHjUJxxXdutzrUpfwZ88821661;     KMuHjUJxxXdutzrUpfwZ88821661 = KMuHjUJxxXdutzrUpfwZ8353511;     KMuHjUJxxXdutzrUpfwZ8353511 = KMuHjUJxxXdutzrUpfwZ58541576;     KMuHjUJxxXdutzrUpfwZ58541576 = KMuHjUJxxXdutzrUpfwZ84954365;     KMuHjUJxxXdutzrUpfwZ84954365 = KMuHjUJxxXdutzrUpfwZ13647353;     KMuHjUJxxXdutzrUpfwZ13647353 = KMuHjUJxxXdutzrUpfwZ31243995;     KMuHjUJxxXdutzrUpfwZ31243995 = KMuHjUJxxXdutzrUpfwZ58439700;     KMuHjUJxxXdutzrUpfwZ58439700 = KMuHjUJxxXdutzrUpfwZ39872619;     KMuHjUJxxXdutzrUpfwZ39872619 = KMuHjUJxxXdutzrUpfwZ49448899;     KMuHjUJxxXdutzrUpfwZ49448899 = KMuHjUJxxXdutzrUpfwZ58821950;     KMuHjUJxxXdutzrUpfwZ58821950 = KMuHjUJxxXdutzrUpfwZ7822349;     KMuHjUJxxXdutzrUpfwZ7822349 = KMuHjUJxxXdutzrUpfwZ53522715;     KMuHjUJxxXdutzrUpfwZ53522715 = KMuHjUJxxXdutzrUpfwZ15211586;     KMuHjUJxxXdutzrUpfwZ15211586 = KMuHjUJxxXdutzrUpfwZ8556274;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oPLMKdAPaghXYwxOrbkl95246538() {     float cKQtdlIJITIrTTaPrHoa79207941 = -121956547;    float cKQtdlIJITIrTTaPrHoa94591712 = -376420413;    float cKQtdlIJITIrTTaPrHoa61156430 = -104763927;    float cKQtdlIJITIrTTaPrHoa43567106 = -854015097;    float cKQtdlIJITIrTTaPrHoa33754849 = -587509429;    float cKQtdlIJITIrTTaPrHoa56339003 = -304215163;    float cKQtdlIJITIrTTaPrHoa26486260 = -243669272;    float cKQtdlIJITIrTTaPrHoa91629653 = -164502161;    float cKQtdlIJITIrTTaPrHoa75875927 = -889063608;    float cKQtdlIJITIrTTaPrHoa29594570 = -151641563;    float cKQtdlIJITIrTTaPrHoa95642905 = 19278695;    float cKQtdlIJITIrTTaPrHoa602556 = 96273792;    float cKQtdlIJITIrTTaPrHoa17108792 = -15028834;    float cKQtdlIJITIrTTaPrHoa31591531 = -132997726;    float cKQtdlIJITIrTTaPrHoa79684218 = -927824927;    float cKQtdlIJITIrTTaPrHoa91177392 = -355294514;    float cKQtdlIJITIrTTaPrHoa84656759 = -136216930;    float cKQtdlIJITIrTTaPrHoa94418913 = -532819286;    float cKQtdlIJITIrTTaPrHoa60624361 = -830336915;    float cKQtdlIJITIrTTaPrHoa66004183 = -18519984;    float cKQtdlIJITIrTTaPrHoa60456212 = -705268528;    float cKQtdlIJITIrTTaPrHoa46595262 = -581219855;    float cKQtdlIJITIrTTaPrHoa29442852 = -632133477;    float cKQtdlIJITIrTTaPrHoa97531316 = -512050666;    float cKQtdlIJITIrTTaPrHoa82090745 = -360905221;    float cKQtdlIJITIrTTaPrHoa95881485 = -945952978;    float cKQtdlIJITIrTTaPrHoa75391726 = -926066490;    float cKQtdlIJITIrTTaPrHoa48774661 = -292644404;    float cKQtdlIJITIrTTaPrHoa51445322 = -194764765;    float cKQtdlIJITIrTTaPrHoa19327995 = -5142848;    float cKQtdlIJITIrTTaPrHoa9775877 = 45679618;    float cKQtdlIJITIrTTaPrHoa85621814 = -11599081;    float cKQtdlIJITIrTTaPrHoa80991916 = -765138380;    float cKQtdlIJITIrTTaPrHoa9228354 = -332802305;    float cKQtdlIJITIrTTaPrHoa59922590 = -968805752;    float cKQtdlIJITIrTTaPrHoa43366777 = -179147658;    float cKQtdlIJITIrTTaPrHoa74944570 = -549521512;    float cKQtdlIJITIrTTaPrHoa69705946 = -54940387;    float cKQtdlIJITIrTTaPrHoa83993611 = -337643641;    float cKQtdlIJITIrTTaPrHoa34184218 = -143420674;    float cKQtdlIJITIrTTaPrHoa76637111 = -319805130;    float cKQtdlIJITIrTTaPrHoa2789381 = -919997308;    float cKQtdlIJITIrTTaPrHoa6644283 = -213999502;    float cKQtdlIJITIrTTaPrHoa63955244 = 70459860;    float cKQtdlIJITIrTTaPrHoa23449036 = -618869679;    float cKQtdlIJITIrTTaPrHoa99432771 = -135876720;    float cKQtdlIJITIrTTaPrHoa64569492 = -760457146;    float cKQtdlIJITIrTTaPrHoa53054039 = -676476842;    float cKQtdlIJITIrTTaPrHoa49588796 = -564852350;    float cKQtdlIJITIrTTaPrHoa53731965 = 40900859;    float cKQtdlIJITIrTTaPrHoa78203980 = -775602432;    float cKQtdlIJITIrTTaPrHoa79182640 = -778525180;    float cKQtdlIJITIrTTaPrHoa2394493 = -618603230;    float cKQtdlIJITIrTTaPrHoa99452198 = -395977953;    float cKQtdlIJITIrTTaPrHoa34790327 = -47024082;    float cKQtdlIJITIrTTaPrHoa32612679 = -540736693;    float cKQtdlIJITIrTTaPrHoa65148861 = -744286937;    float cKQtdlIJITIrTTaPrHoa63625114 = -592713261;    float cKQtdlIJITIrTTaPrHoa61476361 = -393109877;    float cKQtdlIJITIrTTaPrHoa37873364 = -641556452;    float cKQtdlIJITIrTTaPrHoa80947277 = -378148673;    float cKQtdlIJITIrTTaPrHoa77711599 = -951024868;    float cKQtdlIJITIrTTaPrHoa40184331 = -969737396;    float cKQtdlIJITIrTTaPrHoa56547932 = -783920760;    float cKQtdlIJITIrTTaPrHoa19818693 = -97321181;    float cKQtdlIJITIrTTaPrHoa10021092 = -969122224;    float cKQtdlIJITIrTTaPrHoa19610640 = -138587828;    float cKQtdlIJITIrTTaPrHoa7880439 = -682226529;    float cKQtdlIJITIrTTaPrHoa71668941 = -164191974;    float cKQtdlIJITIrTTaPrHoa36317441 = -648677270;    float cKQtdlIJITIrTTaPrHoa16232822 = -805773002;    float cKQtdlIJITIrTTaPrHoa14950813 = 18723456;    float cKQtdlIJITIrTTaPrHoa10425302 = -95175645;    float cKQtdlIJITIrTTaPrHoa26440144 = -586916242;    float cKQtdlIJITIrTTaPrHoa89367072 = -698714854;    float cKQtdlIJITIrTTaPrHoa57666832 = -785271220;    float cKQtdlIJITIrTTaPrHoa39950980 = -267220353;    float cKQtdlIJITIrTTaPrHoa65487607 = -602593338;    float cKQtdlIJITIrTTaPrHoa74082280 = -893180988;    float cKQtdlIJITIrTTaPrHoa82657973 = -125028501;    float cKQtdlIJITIrTTaPrHoa31311994 = -85495832;    float cKQtdlIJITIrTTaPrHoa22337688 = -149589648;    float cKQtdlIJITIrTTaPrHoa99185865 = -727792055;    float cKQtdlIJITIrTTaPrHoa97713357 = -135665625;    float cKQtdlIJITIrTTaPrHoa41124015 = -229540416;    float cKQtdlIJITIrTTaPrHoa30593237 = -175795203;    float cKQtdlIJITIrTTaPrHoa83227321 = -392995852;    float cKQtdlIJITIrTTaPrHoa81539717 = -269160428;    float cKQtdlIJITIrTTaPrHoa74438027 = -185778224;    float cKQtdlIJITIrTTaPrHoa27309911 = -328069060;    float cKQtdlIJITIrTTaPrHoa78217915 = -434860722;    float cKQtdlIJITIrTTaPrHoa11319457 = -956808251;    float cKQtdlIJITIrTTaPrHoa8229585 = -661830511;    float cKQtdlIJITIrTTaPrHoa46120248 = -696087190;    float cKQtdlIJITIrTTaPrHoa53236941 = -765272001;    float cKQtdlIJITIrTTaPrHoa98925512 = -368780263;    float cKQtdlIJITIrTTaPrHoa62605049 = -950259912;    float cKQtdlIJITIrTTaPrHoa50096351 = -430078742;    float cKQtdlIJITIrTTaPrHoa44136551 = -832218959;    float cKQtdlIJITIrTTaPrHoa13427945 = -121956547;     cKQtdlIJITIrTTaPrHoa79207941 = cKQtdlIJITIrTTaPrHoa94591712;     cKQtdlIJITIrTTaPrHoa94591712 = cKQtdlIJITIrTTaPrHoa61156430;     cKQtdlIJITIrTTaPrHoa61156430 = cKQtdlIJITIrTTaPrHoa43567106;     cKQtdlIJITIrTTaPrHoa43567106 = cKQtdlIJITIrTTaPrHoa33754849;     cKQtdlIJITIrTTaPrHoa33754849 = cKQtdlIJITIrTTaPrHoa56339003;     cKQtdlIJITIrTTaPrHoa56339003 = cKQtdlIJITIrTTaPrHoa26486260;     cKQtdlIJITIrTTaPrHoa26486260 = cKQtdlIJITIrTTaPrHoa91629653;     cKQtdlIJITIrTTaPrHoa91629653 = cKQtdlIJITIrTTaPrHoa75875927;     cKQtdlIJITIrTTaPrHoa75875927 = cKQtdlIJITIrTTaPrHoa29594570;     cKQtdlIJITIrTTaPrHoa29594570 = cKQtdlIJITIrTTaPrHoa95642905;     cKQtdlIJITIrTTaPrHoa95642905 = cKQtdlIJITIrTTaPrHoa602556;     cKQtdlIJITIrTTaPrHoa602556 = cKQtdlIJITIrTTaPrHoa17108792;     cKQtdlIJITIrTTaPrHoa17108792 = cKQtdlIJITIrTTaPrHoa31591531;     cKQtdlIJITIrTTaPrHoa31591531 = cKQtdlIJITIrTTaPrHoa79684218;     cKQtdlIJITIrTTaPrHoa79684218 = cKQtdlIJITIrTTaPrHoa91177392;     cKQtdlIJITIrTTaPrHoa91177392 = cKQtdlIJITIrTTaPrHoa84656759;     cKQtdlIJITIrTTaPrHoa84656759 = cKQtdlIJITIrTTaPrHoa94418913;     cKQtdlIJITIrTTaPrHoa94418913 = cKQtdlIJITIrTTaPrHoa60624361;     cKQtdlIJITIrTTaPrHoa60624361 = cKQtdlIJITIrTTaPrHoa66004183;     cKQtdlIJITIrTTaPrHoa66004183 = cKQtdlIJITIrTTaPrHoa60456212;     cKQtdlIJITIrTTaPrHoa60456212 = cKQtdlIJITIrTTaPrHoa46595262;     cKQtdlIJITIrTTaPrHoa46595262 = cKQtdlIJITIrTTaPrHoa29442852;     cKQtdlIJITIrTTaPrHoa29442852 = cKQtdlIJITIrTTaPrHoa97531316;     cKQtdlIJITIrTTaPrHoa97531316 = cKQtdlIJITIrTTaPrHoa82090745;     cKQtdlIJITIrTTaPrHoa82090745 = cKQtdlIJITIrTTaPrHoa95881485;     cKQtdlIJITIrTTaPrHoa95881485 = cKQtdlIJITIrTTaPrHoa75391726;     cKQtdlIJITIrTTaPrHoa75391726 = cKQtdlIJITIrTTaPrHoa48774661;     cKQtdlIJITIrTTaPrHoa48774661 = cKQtdlIJITIrTTaPrHoa51445322;     cKQtdlIJITIrTTaPrHoa51445322 = cKQtdlIJITIrTTaPrHoa19327995;     cKQtdlIJITIrTTaPrHoa19327995 = cKQtdlIJITIrTTaPrHoa9775877;     cKQtdlIJITIrTTaPrHoa9775877 = cKQtdlIJITIrTTaPrHoa85621814;     cKQtdlIJITIrTTaPrHoa85621814 = cKQtdlIJITIrTTaPrHoa80991916;     cKQtdlIJITIrTTaPrHoa80991916 = cKQtdlIJITIrTTaPrHoa9228354;     cKQtdlIJITIrTTaPrHoa9228354 = cKQtdlIJITIrTTaPrHoa59922590;     cKQtdlIJITIrTTaPrHoa59922590 = cKQtdlIJITIrTTaPrHoa43366777;     cKQtdlIJITIrTTaPrHoa43366777 = cKQtdlIJITIrTTaPrHoa74944570;     cKQtdlIJITIrTTaPrHoa74944570 = cKQtdlIJITIrTTaPrHoa69705946;     cKQtdlIJITIrTTaPrHoa69705946 = cKQtdlIJITIrTTaPrHoa83993611;     cKQtdlIJITIrTTaPrHoa83993611 = cKQtdlIJITIrTTaPrHoa34184218;     cKQtdlIJITIrTTaPrHoa34184218 = cKQtdlIJITIrTTaPrHoa76637111;     cKQtdlIJITIrTTaPrHoa76637111 = cKQtdlIJITIrTTaPrHoa2789381;     cKQtdlIJITIrTTaPrHoa2789381 = cKQtdlIJITIrTTaPrHoa6644283;     cKQtdlIJITIrTTaPrHoa6644283 = cKQtdlIJITIrTTaPrHoa63955244;     cKQtdlIJITIrTTaPrHoa63955244 = cKQtdlIJITIrTTaPrHoa23449036;     cKQtdlIJITIrTTaPrHoa23449036 = cKQtdlIJITIrTTaPrHoa99432771;     cKQtdlIJITIrTTaPrHoa99432771 = cKQtdlIJITIrTTaPrHoa64569492;     cKQtdlIJITIrTTaPrHoa64569492 = cKQtdlIJITIrTTaPrHoa53054039;     cKQtdlIJITIrTTaPrHoa53054039 = cKQtdlIJITIrTTaPrHoa49588796;     cKQtdlIJITIrTTaPrHoa49588796 = cKQtdlIJITIrTTaPrHoa53731965;     cKQtdlIJITIrTTaPrHoa53731965 = cKQtdlIJITIrTTaPrHoa78203980;     cKQtdlIJITIrTTaPrHoa78203980 = cKQtdlIJITIrTTaPrHoa79182640;     cKQtdlIJITIrTTaPrHoa79182640 = cKQtdlIJITIrTTaPrHoa2394493;     cKQtdlIJITIrTTaPrHoa2394493 = cKQtdlIJITIrTTaPrHoa99452198;     cKQtdlIJITIrTTaPrHoa99452198 = cKQtdlIJITIrTTaPrHoa34790327;     cKQtdlIJITIrTTaPrHoa34790327 = cKQtdlIJITIrTTaPrHoa32612679;     cKQtdlIJITIrTTaPrHoa32612679 = cKQtdlIJITIrTTaPrHoa65148861;     cKQtdlIJITIrTTaPrHoa65148861 = cKQtdlIJITIrTTaPrHoa63625114;     cKQtdlIJITIrTTaPrHoa63625114 = cKQtdlIJITIrTTaPrHoa61476361;     cKQtdlIJITIrTTaPrHoa61476361 = cKQtdlIJITIrTTaPrHoa37873364;     cKQtdlIJITIrTTaPrHoa37873364 = cKQtdlIJITIrTTaPrHoa80947277;     cKQtdlIJITIrTTaPrHoa80947277 = cKQtdlIJITIrTTaPrHoa77711599;     cKQtdlIJITIrTTaPrHoa77711599 = cKQtdlIJITIrTTaPrHoa40184331;     cKQtdlIJITIrTTaPrHoa40184331 = cKQtdlIJITIrTTaPrHoa56547932;     cKQtdlIJITIrTTaPrHoa56547932 = cKQtdlIJITIrTTaPrHoa19818693;     cKQtdlIJITIrTTaPrHoa19818693 = cKQtdlIJITIrTTaPrHoa10021092;     cKQtdlIJITIrTTaPrHoa10021092 = cKQtdlIJITIrTTaPrHoa19610640;     cKQtdlIJITIrTTaPrHoa19610640 = cKQtdlIJITIrTTaPrHoa7880439;     cKQtdlIJITIrTTaPrHoa7880439 = cKQtdlIJITIrTTaPrHoa71668941;     cKQtdlIJITIrTTaPrHoa71668941 = cKQtdlIJITIrTTaPrHoa36317441;     cKQtdlIJITIrTTaPrHoa36317441 = cKQtdlIJITIrTTaPrHoa16232822;     cKQtdlIJITIrTTaPrHoa16232822 = cKQtdlIJITIrTTaPrHoa14950813;     cKQtdlIJITIrTTaPrHoa14950813 = cKQtdlIJITIrTTaPrHoa10425302;     cKQtdlIJITIrTTaPrHoa10425302 = cKQtdlIJITIrTTaPrHoa26440144;     cKQtdlIJITIrTTaPrHoa26440144 = cKQtdlIJITIrTTaPrHoa89367072;     cKQtdlIJITIrTTaPrHoa89367072 = cKQtdlIJITIrTTaPrHoa57666832;     cKQtdlIJITIrTTaPrHoa57666832 = cKQtdlIJITIrTTaPrHoa39950980;     cKQtdlIJITIrTTaPrHoa39950980 = cKQtdlIJITIrTTaPrHoa65487607;     cKQtdlIJITIrTTaPrHoa65487607 = cKQtdlIJITIrTTaPrHoa74082280;     cKQtdlIJITIrTTaPrHoa74082280 = cKQtdlIJITIrTTaPrHoa82657973;     cKQtdlIJITIrTTaPrHoa82657973 = cKQtdlIJITIrTTaPrHoa31311994;     cKQtdlIJITIrTTaPrHoa31311994 = cKQtdlIJITIrTTaPrHoa22337688;     cKQtdlIJITIrTTaPrHoa22337688 = cKQtdlIJITIrTTaPrHoa99185865;     cKQtdlIJITIrTTaPrHoa99185865 = cKQtdlIJITIrTTaPrHoa97713357;     cKQtdlIJITIrTTaPrHoa97713357 = cKQtdlIJITIrTTaPrHoa41124015;     cKQtdlIJITIrTTaPrHoa41124015 = cKQtdlIJITIrTTaPrHoa30593237;     cKQtdlIJITIrTTaPrHoa30593237 = cKQtdlIJITIrTTaPrHoa83227321;     cKQtdlIJITIrTTaPrHoa83227321 = cKQtdlIJITIrTTaPrHoa81539717;     cKQtdlIJITIrTTaPrHoa81539717 = cKQtdlIJITIrTTaPrHoa74438027;     cKQtdlIJITIrTTaPrHoa74438027 = cKQtdlIJITIrTTaPrHoa27309911;     cKQtdlIJITIrTTaPrHoa27309911 = cKQtdlIJITIrTTaPrHoa78217915;     cKQtdlIJITIrTTaPrHoa78217915 = cKQtdlIJITIrTTaPrHoa11319457;     cKQtdlIJITIrTTaPrHoa11319457 = cKQtdlIJITIrTTaPrHoa8229585;     cKQtdlIJITIrTTaPrHoa8229585 = cKQtdlIJITIrTTaPrHoa46120248;     cKQtdlIJITIrTTaPrHoa46120248 = cKQtdlIJITIrTTaPrHoa53236941;     cKQtdlIJITIrTTaPrHoa53236941 = cKQtdlIJITIrTTaPrHoa98925512;     cKQtdlIJITIrTTaPrHoa98925512 = cKQtdlIJITIrTTaPrHoa62605049;     cKQtdlIJITIrTTaPrHoa62605049 = cKQtdlIJITIrTTaPrHoa50096351;     cKQtdlIJITIrTTaPrHoa50096351 = cKQtdlIJITIrTTaPrHoa44136551;     cKQtdlIJITIrTTaPrHoa44136551 = cKQtdlIJITIrTTaPrHoa13427945;     cKQtdlIJITIrTTaPrHoa13427945 = cKQtdlIJITIrTTaPrHoa79207941;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void INZJvPgELwAffAmmjFyr1905284() {     float NkvxYZsJmVFORzEfrzZy79201469 = 5519312;    float NkvxYZsJmVFORzEfrzZy33164029 = -419015539;    float NkvxYZsJmVFORzEfrzZy62469642 = -61892541;    float NkvxYZsJmVFORzEfrzZy28646967 = -674555927;    float NkvxYZsJmVFORzEfrzZy42594951 = 44135250;    float NkvxYZsJmVFORzEfrzZy91144435 = -3647974;    float NkvxYZsJmVFORzEfrzZy95033595 = 51794261;    float NkvxYZsJmVFORzEfrzZy60843704 = -968679478;    float NkvxYZsJmVFORzEfrzZy58616091 = -602709678;    float NkvxYZsJmVFORzEfrzZy26234083 = -888448434;    float NkvxYZsJmVFORzEfrzZy15963511 = 59806623;    float NkvxYZsJmVFORzEfrzZy14894233 = -739204571;    float NkvxYZsJmVFORzEfrzZy48961706 = -36687309;    float NkvxYZsJmVFORzEfrzZy85435751 = -478150777;    float NkvxYZsJmVFORzEfrzZy24856328 = -890537779;    float NkvxYZsJmVFORzEfrzZy65741940 = -688090165;    float NkvxYZsJmVFORzEfrzZy40509519 = 44168500;    float NkvxYZsJmVFORzEfrzZy17419447 = -929890908;    float NkvxYZsJmVFORzEfrzZy81052631 = -348319182;    float NkvxYZsJmVFORzEfrzZy38077946 = -630544435;    float NkvxYZsJmVFORzEfrzZy40125636 = -415734901;    float NkvxYZsJmVFORzEfrzZy24891528 = 8788262;    float NkvxYZsJmVFORzEfrzZy66321248 = 8908681;    float NkvxYZsJmVFORzEfrzZy41181425 = -392971189;    float NkvxYZsJmVFORzEfrzZy10533435 = -787599074;    float NkvxYZsJmVFORzEfrzZy12455563 = -854158906;    float NkvxYZsJmVFORzEfrzZy45613308 = -126866203;    float NkvxYZsJmVFORzEfrzZy84178186 = -901611393;    float NkvxYZsJmVFORzEfrzZy64123769 = -840835080;    float NkvxYZsJmVFORzEfrzZy95417865 = -780574221;    float NkvxYZsJmVFORzEfrzZy43188556 = -341413617;    float NkvxYZsJmVFORzEfrzZy70345671 = -403420843;    float NkvxYZsJmVFORzEfrzZy66444339 = -967082596;    float NkvxYZsJmVFORzEfrzZy9511191 = -209484038;    float NkvxYZsJmVFORzEfrzZy42280628 = -128119677;    float NkvxYZsJmVFORzEfrzZy86194967 = -258913404;    float NkvxYZsJmVFORzEfrzZy86885938 = -649174890;    float NkvxYZsJmVFORzEfrzZy39623626 = -50203066;    float NkvxYZsJmVFORzEfrzZy19307208 = 49674943;    float NkvxYZsJmVFORzEfrzZy6057111 = -852852360;    float NkvxYZsJmVFORzEfrzZy64492779 = -407749028;    float NkvxYZsJmVFORzEfrzZy88487041 = -13789390;    float NkvxYZsJmVFORzEfrzZy66927642 = -773129756;    float NkvxYZsJmVFORzEfrzZy23187142 = -586504583;    float NkvxYZsJmVFORzEfrzZy7467089 = 7870920;    float NkvxYZsJmVFORzEfrzZy39860740 = 52078610;    float NkvxYZsJmVFORzEfrzZy92267284 = -141737768;    float NkvxYZsJmVFORzEfrzZy28437620 = -185238198;    float NkvxYZsJmVFORzEfrzZy68247185 = -128616211;    float NkvxYZsJmVFORzEfrzZy73777388 = -646124027;    float NkvxYZsJmVFORzEfrzZy91463349 = -354157439;    float NkvxYZsJmVFORzEfrzZy93729898 = -60670928;    float NkvxYZsJmVFORzEfrzZy77842841 = -510657238;    float NkvxYZsJmVFORzEfrzZy9702470 = -927897804;    float NkvxYZsJmVFORzEfrzZy61779647 = -903923360;    float NkvxYZsJmVFORzEfrzZy54309942 = 96731049;    float NkvxYZsJmVFORzEfrzZy66842781 = -327924220;    float NkvxYZsJmVFORzEfrzZy21288218 = -668921353;    float NkvxYZsJmVFORzEfrzZy18113533 = -886956854;    float NkvxYZsJmVFORzEfrzZy30139389 = -101705845;    float NkvxYZsJmVFORzEfrzZy45531128 = -876781772;    float NkvxYZsJmVFORzEfrzZy10855410 = -46594346;    float NkvxYZsJmVFORzEfrzZy96719935 = -27844399;    float NkvxYZsJmVFORzEfrzZy63198226 = -822135457;    float NkvxYZsJmVFORzEfrzZy83045527 = -447034818;    float NkvxYZsJmVFORzEfrzZy45617839 = -536772535;    float NkvxYZsJmVFORzEfrzZy48449894 = -772121975;    float NkvxYZsJmVFORzEfrzZy39450515 = -827203271;    float NkvxYZsJmVFORzEfrzZy43155123 = -250031101;    float NkvxYZsJmVFORzEfrzZy38661361 = -531624375;    float NkvxYZsJmVFORzEfrzZy78856001 = 61084725;    float NkvxYZsJmVFORzEfrzZy885893 = -905628434;    float NkvxYZsJmVFORzEfrzZy98112238 = -879565851;    float NkvxYZsJmVFORzEfrzZy74995521 = -495466822;    float NkvxYZsJmVFORzEfrzZy73585167 = -122795407;    float NkvxYZsJmVFORzEfrzZy51638595 = -301945511;    float NkvxYZsJmVFORzEfrzZy57963885 = -218081982;    float NkvxYZsJmVFORzEfrzZy43134106 = -404586737;    float NkvxYZsJmVFORzEfrzZy33714337 = -300842109;    float NkvxYZsJmVFORzEfrzZy70672694 = -739677684;    float NkvxYZsJmVFORzEfrzZy20188278 = -612421139;    float NkvxYZsJmVFORzEfrzZy17175689 = -941628005;    float NkvxYZsJmVFORzEfrzZy15931001 = -672995182;    float NkvxYZsJmVFORzEfrzZy90346381 = -94711054;    float NkvxYZsJmVFORzEfrzZy3954516 = -326416783;    float NkvxYZsJmVFORzEfrzZy49458658 = -180742689;    float NkvxYZsJmVFORzEfrzZy92502830 = -892763605;    float NkvxYZsJmVFORzEfrzZy56741870 = 60815208;    float NkvxYZsJmVFORzEfrzZy47731544 = -305560679;    float NkvxYZsJmVFORzEfrzZy87970685 = -124850727;    float NkvxYZsJmVFORzEfrzZy19352186 = -930989185;    float NkvxYZsJmVFORzEfrzZy65597720 = -980253538;    float NkvxYZsJmVFORzEfrzZy21510093 = -163246213;    float NkvxYZsJmVFORzEfrzZy89167819 = -848619213;    float NkvxYZsJmVFORzEfrzZy60525983 = -976070588;    float NkvxYZsJmVFORzEfrzZy53637369 = -261154683;    float NkvxYZsJmVFORzEfrzZy91767106 = -985944992;    float NkvxYZsJmVFORzEfrzZy3729417 = -950994300;    float NkvxYZsJmVFORzEfrzZy40141615 = -39469766;    float NkvxYZsJmVFORzEfrzZy61849249 = 5519312;     NkvxYZsJmVFORzEfrzZy79201469 = NkvxYZsJmVFORzEfrzZy33164029;     NkvxYZsJmVFORzEfrzZy33164029 = NkvxYZsJmVFORzEfrzZy62469642;     NkvxYZsJmVFORzEfrzZy62469642 = NkvxYZsJmVFORzEfrzZy28646967;     NkvxYZsJmVFORzEfrzZy28646967 = NkvxYZsJmVFORzEfrzZy42594951;     NkvxYZsJmVFORzEfrzZy42594951 = NkvxYZsJmVFORzEfrzZy91144435;     NkvxYZsJmVFORzEfrzZy91144435 = NkvxYZsJmVFORzEfrzZy95033595;     NkvxYZsJmVFORzEfrzZy95033595 = NkvxYZsJmVFORzEfrzZy60843704;     NkvxYZsJmVFORzEfrzZy60843704 = NkvxYZsJmVFORzEfrzZy58616091;     NkvxYZsJmVFORzEfrzZy58616091 = NkvxYZsJmVFORzEfrzZy26234083;     NkvxYZsJmVFORzEfrzZy26234083 = NkvxYZsJmVFORzEfrzZy15963511;     NkvxYZsJmVFORzEfrzZy15963511 = NkvxYZsJmVFORzEfrzZy14894233;     NkvxYZsJmVFORzEfrzZy14894233 = NkvxYZsJmVFORzEfrzZy48961706;     NkvxYZsJmVFORzEfrzZy48961706 = NkvxYZsJmVFORzEfrzZy85435751;     NkvxYZsJmVFORzEfrzZy85435751 = NkvxYZsJmVFORzEfrzZy24856328;     NkvxYZsJmVFORzEfrzZy24856328 = NkvxYZsJmVFORzEfrzZy65741940;     NkvxYZsJmVFORzEfrzZy65741940 = NkvxYZsJmVFORzEfrzZy40509519;     NkvxYZsJmVFORzEfrzZy40509519 = NkvxYZsJmVFORzEfrzZy17419447;     NkvxYZsJmVFORzEfrzZy17419447 = NkvxYZsJmVFORzEfrzZy81052631;     NkvxYZsJmVFORzEfrzZy81052631 = NkvxYZsJmVFORzEfrzZy38077946;     NkvxYZsJmVFORzEfrzZy38077946 = NkvxYZsJmVFORzEfrzZy40125636;     NkvxYZsJmVFORzEfrzZy40125636 = NkvxYZsJmVFORzEfrzZy24891528;     NkvxYZsJmVFORzEfrzZy24891528 = NkvxYZsJmVFORzEfrzZy66321248;     NkvxYZsJmVFORzEfrzZy66321248 = NkvxYZsJmVFORzEfrzZy41181425;     NkvxYZsJmVFORzEfrzZy41181425 = NkvxYZsJmVFORzEfrzZy10533435;     NkvxYZsJmVFORzEfrzZy10533435 = NkvxYZsJmVFORzEfrzZy12455563;     NkvxYZsJmVFORzEfrzZy12455563 = NkvxYZsJmVFORzEfrzZy45613308;     NkvxYZsJmVFORzEfrzZy45613308 = NkvxYZsJmVFORzEfrzZy84178186;     NkvxYZsJmVFORzEfrzZy84178186 = NkvxYZsJmVFORzEfrzZy64123769;     NkvxYZsJmVFORzEfrzZy64123769 = NkvxYZsJmVFORzEfrzZy95417865;     NkvxYZsJmVFORzEfrzZy95417865 = NkvxYZsJmVFORzEfrzZy43188556;     NkvxYZsJmVFORzEfrzZy43188556 = NkvxYZsJmVFORzEfrzZy70345671;     NkvxYZsJmVFORzEfrzZy70345671 = NkvxYZsJmVFORzEfrzZy66444339;     NkvxYZsJmVFORzEfrzZy66444339 = NkvxYZsJmVFORzEfrzZy9511191;     NkvxYZsJmVFORzEfrzZy9511191 = NkvxYZsJmVFORzEfrzZy42280628;     NkvxYZsJmVFORzEfrzZy42280628 = NkvxYZsJmVFORzEfrzZy86194967;     NkvxYZsJmVFORzEfrzZy86194967 = NkvxYZsJmVFORzEfrzZy86885938;     NkvxYZsJmVFORzEfrzZy86885938 = NkvxYZsJmVFORzEfrzZy39623626;     NkvxYZsJmVFORzEfrzZy39623626 = NkvxYZsJmVFORzEfrzZy19307208;     NkvxYZsJmVFORzEfrzZy19307208 = NkvxYZsJmVFORzEfrzZy6057111;     NkvxYZsJmVFORzEfrzZy6057111 = NkvxYZsJmVFORzEfrzZy64492779;     NkvxYZsJmVFORzEfrzZy64492779 = NkvxYZsJmVFORzEfrzZy88487041;     NkvxYZsJmVFORzEfrzZy88487041 = NkvxYZsJmVFORzEfrzZy66927642;     NkvxYZsJmVFORzEfrzZy66927642 = NkvxYZsJmVFORzEfrzZy23187142;     NkvxYZsJmVFORzEfrzZy23187142 = NkvxYZsJmVFORzEfrzZy7467089;     NkvxYZsJmVFORzEfrzZy7467089 = NkvxYZsJmVFORzEfrzZy39860740;     NkvxYZsJmVFORzEfrzZy39860740 = NkvxYZsJmVFORzEfrzZy92267284;     NkvxYZsJmVFORzEfrzZy92267284 = NkvxYZsJmVFORzEfrzZy28437620;     NkvxYZsJmVFORzEfrzZy28437620 = NkvxYZsJmVFORzEfrzZy68247185;     NkvxYZsJmVFORzEfrzZy68247185 = NkvxYZsJmVFORzEfrzZy73777388;     NkvxYZsJmVFORzEfrzZy73777388 = NkvxYZsJmVFORzEfrzZy91463349;     NkvxYZsJmVFORzEfrzZy91463349 = NkvxYZsJmVFORzEfrzZy93729898;     NkvxYZsJmVFORzEfrzZy93729898 = NkvxYZsJmVFORzEfrzZy77842841;     NkvxYZsJmVFORzEfrzZy77842841 = NkvxYZsJmVFORzEfrzZy9702470;     NkvxYZsJmVFORzEfrzZy9702470 = NkvxYZsJmVFORzEfrzZy61779647;     NkvxYZsJmVFORzEfrzZy61779647 = NkvxYZsJmVFORzEfrzZy54309942;     NkvxYZsJmVFORzEfrzZy54309942 = NkvxYZsJmVFORzEfrzZy66842781;     NkvxYZsJmVFORzEfrzZy66842781 = NkvxYZsJmVFORzEfrzZy21288218;     NkvxYZsJmVFORzEfrzZy21288218 = NkvxYZsJmVFORzEfrzZy18113533;     NkvxYZsJmVFORzEfrzZy18113533 = NkvxYZsJmVFORzEfrzZy30139389;     NkvxYZsJmVFORzEfrzZy30139389 = NkvxYZsJmVFORzEfrzZy45531128;     NkvxYZsJmVFORzEfrzZy45531128 = NkvxYZsJmVFORzEfrzZy10855410;     NkvxYZsJmVFORzEfrzZy10855410 = NkvxYZsJmVFORzEfrzZy96719935;     NkvxYZsJmVFORzEfrzZy96719935 = NkvxYZsJmVFORzEfrzZy63198226;     NkvxYZsJmVFORzEfrzZy63198226 = NkvxYZsJmVFORzEfrzZy83045527;     NkvxYZsJmVFORzEfrzZy83045527 = NkvxYZsJmVFORzEfrzZy45617839;     NkvxYZsJmVFORzEfrzZy45617839 = NkvxYZsJmVFORzEfrzZy48449894;     NkvxYZsJmVFORzEfrzZy48449894 = NkvxYZsJmVFORzEfrzZy39450515;     NkvxYZsJmVFORzEfrzZy39450515 = NkvxYZsJmVFORzEfrzZy43155123;     NkvxYZsJmVFORzEfrzZy43155123 = NkvxYZsJmVFORzEfrzZy38661361;     NkvxYZsJmVFORzEfrzZy38661361 = NkvxYZsJmVFORzEfrzZy78856001;     NkvxYZsJmVFORzEfrzZy78856001 = NkvxYZsJmVFORzEfrzZy885893;     NkvxYZsJmVFORzEfrzZy885893 = NkvxYZsJmVFORzEfrzZy98112238;     NkvxYZsJmVFORzEfrzZy98112238 = NkvxYZsJmVFORzEfrzZy74995521;     NkvxYZsJmVFORzEfrzZy74995521 = NkvxYZsJmVFORzEfrzZy73585167;     NkvxYZsJmVFORzEfrzZy73585167 = NkvxYZsJmVFORzEfrzZy51638595;     NkvxYZsJmVFORzEfrzZy51638595 = NkvxYZsJmVFORzEfrzZy57963885;     NkvxYZsJmVFORzEfrzZy57963885 = NkvxYZsJmVFORzEfrzZy43134106;     NkvxYZsJmVFORzEfrzZy43134106 = NkvxYZsJmVFORzEfrzZy33714337;     NkvxYZsJmVFORzEfrzZy33714337 = NkvxYZsJmVFORzEfrzZy70672694;     NkvxYZsJmVFORzEfrzZy70672694 = NkvxYZsJmVFORzEfrzZy20188278;     NkvxYZsJmVFORzEfrzZy20188278 = NkvxYZsJmVFORzEfrzZy17175689;     NkvxYZsJmVFORzEfrzZy17175689 = NkvxYZsJmVFORzEfrzZy15931001;     NkvxYZsJmVFORzEfrzZy15931001 = NkvxYZsJmVFORzEfrzZy90346381;     NkvxYZsJmVFORzEfrzZy90346381 = NkvxYZsJmVFORzEfrzZy3954516;     NkvxYZsJmVFORzEfrzZy3954516 = NkvxYZsJmVFORzEfrzZy49458658;     NkvxYZsJmVFORzEfrzZy49458658 = NkvxYZsJmVFORzEfrzZy92502830;     NkvxYZsJmVFORzEfrzZy92502830 = NkvxYZsJmVFORzEfrzZy56741870;     NkvxYZsJmVFORzEfrzZy56741870 = NkvxYZsJmVFORzEfrzZy47731544;     NkvxYZsJmVFORzEfrzZy47731544 = NkvxYZsJmVFORzEfrzZy87970685;     NkvxYZsJmVFORzEfrzZy87970685 = NkvxYZsJmVFORzEfrzZy19352186;     NkvxYZsJmVFORzEfrzZy19352186 = NkvxYZsJmVFORzEfrzZy65597720;     NkvxYZsJmVFORzEfrzZy65597720 = NkvxYZsJmVFORzEfrzZy21510093;     NkvxYZsJmVFORzEfrzZy21510093 = NkvxYZsJmVFORzEfrzZy89167819;     NkvxYZsJmVFORzEfrzZy89167819 = NkvxYZsJmVFORzEfrzZy60525983;     NkvxYZsJmVFORzEfrzZy60525983 = NkvxYZsJmVFORzEfrzZy53637369;     NkvxYZsJmVFORzEfrzZy53637369 = NkvxYZsJmVFORzEfrzZy91767106;     NkvxYZsJmVFORzEfrzZy91767106 = NkvxYZsJmVFORzEfrzZy3729417;     NkvxYZsJmVFORzEfrzZy3729417 = NkvxYZsJmVFORzEfrzZy40141615;     NkvxYZsJmVFORzEfrzZy40141615 = NkvxYZsJmVFORzEfrzZy61849249;     NkvxYZsJmVFORzEfrzZy61849249 = NkvxYZsJmVFORzEfrzZy79201469;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void CwuAPQMdFVlSfaqWjKtG61555374() {     float AIhGzsQfRmKMdkSXYvHS6251074 = 49242942;    float AIhGzsQfRmKMdkSXYvHS91293753 = -893879501;    float AIhGzsQfRmKMdkSXYvHS75330052 = 84479769;    float AIhGzsQfRmKMdkSXYvHS33738771 = -636389832;    float AIhGzsQfRmKMdkSXYvHS4170749 = -183912839;    float AIhGzsQfRmKMdkSXYvHS76695159 = -369116174;    float AIhGzsQfRmKMdkSXYvHS97605868 = -716415133;    float AIhGzsQfRmKMdkSXYvHS29135104 = -659622708;    float AIhGzsQfRmKMdkSXYvHS90128427 = -616701500;    float AIhGzsQfRmKMdkSXYvHS4830262 = -538982447;    float AIhGzsQfRmKMdkSXYvHS6715909 = -62834260;    float AIhGzsQfRmKMdkSXYvHS15697299 = -957026000;    float AIhGzsQfRmKMdkSXYvHS89971059 = -706871840;    float AIhGzsQfRmKMdkSXYvHS78711658 = -736521798;    float AIhGzsQfRmKMdkSXYvHS85350541 = -471746059;    float AIhGzsQfRmKMdkSXYvHS31559704 = -8922708;    float AIhGzsQfRmKMdkSXYvHS20348685 = -382192936;    float AIhGzsQfRmKMdkSXYvHS87241783 = -123858539;    float AIhGzsQfRmKMdkSXYvHS23019526 = -546755022;    float AIhGzsQfRmKMdkSXYvHS94136581 = 86485529;    float AIhGzsQfRmKMdkSXYvHS75596351 = -787222393;    float AIhGzsQfRmKMdkSXYvHS17089003 = -299932305;    float AIhGzsQfRmKMdkSXYvHS59986407 = -455218030;    float AIhGzsQfRmKMdkSXYvHS64546945 = -254854209;    float AIhGzsQfRmKMdkSXYvHS31272936 = 26295666;    float AIhGzsQfRmKMdkSXYvHS96281907 = -866504504;    float AIhGzsQfRmKMdkSXYvHS31915675 = -63297333;    float AIhGzsQfRmKMdkSXYvHS99084230 = -486908971;    float AIhGzsQfRmKMdkSXYvHS45065949 = 67250819;    float AIhGzsQfRmKMdkSXYvHS28898116 = -297338839;    float AIhGzsQfRmKMdkSXYvHS97419041 = 62264249;    float AIhGzsQfRmKMdkSXYvHS77973651 = -427211524;    float AIhGzsQfRmKMdkSXYvHS58323357 = -975513436;    float AIhGzsQfRmKMdkSXYvHS63202224 = -642879532;    float AIhGzsQfRmKMdkSXYvHS66602 = -85596953;    float AIhGzsQfRmKMdkSXYvHS61679141 = -447597195;    float AIhGzsQfRmKMdkSXYvHS65103428 = -382982740;    float AIhGzsQfRmKMdkSXYvHS68829866 = -897679210;    float AIhGzsQfRmKMdkSXYvHS24242993 = -667467921;    float AIhGzsQfRmKMdkSXYvHS85388104 = -310595381;    float AIhGzsQfRmKMdkSXYvHS64389966 = -88546980;    float AIhGzsQfRmKMdkSXYvHS31548015 = -136990065;    float AIhGzsQfRmKMdkSXYvHS13123818 = -352252840;    float AIhGzsQfRmKMdkSXYvHS32585788 = -611342344;    float AIhGzsQfRmKMdkSXYvHS80411853 = -717215904;    float AIhGzsQfRmKMdkSXYvHS52759664 = -708884262;    float AIhGzsQfRmKMdkSXYvHS36416153 = -695604627;    float AIhGzsQfRmKMdkSXYvHS77234127 = -533473866;    float AIhGzsQfRmKMdkSXYvHS56726870 = -248358272;    float AIhGzsQfRmKMdkSXYvHS40646018 = -444382472;    float AIhGzsQfRmKMdkSXYvHS8871894 = -893255459;    float AIhGzsQfRmKMdkSXYvHS88779233 = -253874008;    float AIhGzsQfRmKMdkSXYvHS68574812 = -882416416;    float AIhGzsQfRmKMdkSXYvHS64591933 = -709565284;    float AIhGzsQfRmKMdkSXYvHS85959023 = -644412059;    float AIhGzsQfRmKMdkSXYvHS89162071 = -650824754;    float AIhGzsQfRmKMdkSXYvHS31307347 = -338661471;    float AIhGzsQfRmKMdkSXYvHS10783107 = -660666023;    float AIhGzsQfRmKMdkSXYvHS2465836 = -562685499;    float AIhGzsQfRmKMdkSXYvHS7888841 = -317408335;    float AIhGzsQfRmKMdkSXYvHS44779484 = -205818841;    float AIhGzsQfRmKMdkSXYvHS98521638 = -129506163;    float AIhGzsQfRmKMdkSXYvHS84069154 = -626873528;    float AIhGzsQfRmKMdkSXYvHS61230311 = -219362661;    float AIhGzsQfRmKMdkSXYvHS7411220 = -501246697;    float AIhGzsQfRmKMdkSXYvHS28742258 = -635622737;    float AIhGzsQfRmKMdkSXYvHS57373941 = -981512565;    float AIhGzsQfRmKMdkSXYvHS26768835 = 36007692;    float AIhGzsQfRmKMdkSXYvHS78645057 = -550924845;    float AIhGzsQfRmKMdkSXYvHS23671401 = 75851135;    float AIhGzsQfRmKMdkSXYvHS66456275 = -625939969;    float AIhGzsQfRmKMdkSXYvHS51518819 = -484513726;    float AIhGzsQfRmKMdkSXYvHS62998791 = -456390619;    float AIhGzsQfRmKMdkSXYvHS37631422 = -136159641;    float AIhGzsQfRmKMdkSXYvHS29746615 = -824967492;    float AIhGzsQfRmKMdkSXYvHS44048337 = -550232329;    float AIhGzsQfRmKMdkSXYvHS3965186 = -947679465;    float AIhGzsQfRmKMdkSXYvHS27400619 = -843875687;    float AIhGzsQfRmKMdkSXYvHS84135092 = -537638306;    float AIhGzsQfRmKMdkSXYvHS78513271 = -264820072;    float AIhGzsQfRmKMdkSXYvHS59865755 = -70899878;    float AIhGzsQfRmKMdkSXYvHS54681548 = -529823468;    float AIhGzsQfRmKMdkSXYvHS42357360 = -138550700;    float AIhGzsQfRmKMdkSXYvHS4419931 = -488366709;    float AIhGzsQfRmKMdkSXYvHS20026223 = -404083380;    float AIhGzsQfRmKMdkSXYvHS8639808 = -683861744;    float AIhGzsQfRmKMdkSXYvHS9398839 = -544795109;    float AIhGzsQfRmKMdkSXYvHS93731423 = -165948152;    float AIhGzsQfRmKMdkSXYvHS77243201 = -998467473;    float AIhGzsQfRmKMdkSXYvHS10904531 = -434772200;    float AIhGzsQfRmKMdkSXYvHS30371795 = -8935724;    float AIhGzsQfRmKMdkSXYvHS54320321 = -722316718;    float AIhGzsQfRmKMdkSXYvHS66364030 = -234993712;    float AIhGzsQfRmKMdkSXYvHS16354152 = -250059586;    float AIhGzsQfRmKMdkSXYvHS40608620 = -4776541;    float AIhGzsQfRmKMdkSXYvHS65868328 = -959040817;    float AIhGzsQfRmKMdkSXYvHS47478860 = -510116538;    float AIhGzsQfRmKMdkSXYvHS51893506 = -32890179;    float AIhGzsQfRmKMdkSXYvHS25174568 = -10095647;    float AIhGzsQfRmKMdkSXYvHS51669595 = 49242942;     AIhGzsQfRmKMdkSXYvHS6251074 = AIhGzsQfRmKMdkSXYvHS91293753;     AIhGzsQfRmKMdkSXYvHS91293753 = AIhGzsQfRmKMdkSXYvHS75330052;     AIhGzsQfRmKMdkSXYvHS75330052 = AIhGzsQfRmKMdkSXYvHS33738771;     AIhGzsQfRmKMdkSXYvHS33738771 = AIhGzsQfRmKMdkSXYvHS4170749;     AIhGzsQfRmKMdkSXYvHS4170749 = AIhGzsQfRmKMdkSXYvHS76695159;     AIhGzsQfRmKMdkSXYvHS76695159 = AIhGzsQfRmKMdkSXYvHS97605868;     AIhGzsQfRmKMdkSXYvHS97605868 = AIhGzsQfRmKMdkSXYvHS29135104;     AIhGzsQfRmKMdkSXYvHS29135104 = AIhGzsQfRmKMdkSXYvHS90128427;     AIhGzsQfRmKMdkSXYvHS90128427 = AIhGzsQfRmKMdkSXYvHS4830262;     AIhGzsQfRmKMdkSXYvHS4830262 = AIhGzsQfRmKMdkSXYvHS6715909;     AIhGzsQfRmKMdkSXYvHS6715909 = AIhGzsQfRmKMdkSXYvHS15697299;     AIhGzsQfRmKMdkSXYvHS15697299 = AIhGzsQfRmKMdkSXYvHS89971059;     AIhGzsQfRmKMdkSXYvHS89971059 = AIhGzsQfRmKMdkSXYvHS78711658;     AIhGzsQfRmKMdkSXYvHS78711658 = AIhGzsQfRmKMdkSXYvHS85350541;     AIhGzsQfRmKMdkSXYvHS85350541 = AIhGzsQfRmKMdkSXYvHS31559704;     AIhGzsQfRmKMdkSXYvHS31559704 = AIhGzsQfRmKMdkSXYvHS20348685;     AIhGzsQfRmKMdkSXYvHS20348685 = AIhGzsQfRmKMdkSXYvHS87241783;     AIhGzsQfRmKMdkSXYvHS87241783 = AIhGzsQfRmKMdkSXYvHS23019526;     AIhGzsQfRmKMdkSXYvHS23019526 = AIhGzsQfRmKMdkSXYvHS94136581;     AIhGzsQfRmKMdkSXYvHS94136581 = AIhGzsQfRmKMdkSXYvHS75596351;     AIhGzsQfRmKMdkSXYvHS75596351 = AIhGzsQfRmKMdkSXYvHS17089003;     AIhGzsQfRmKMdkSXYvHS17089003 = AIhGzsQfRmKMdkSXYvHS59986407;     AIhGzsQfRmKMdkSXYvHS59986407 = AIhGzsQfRmKMdkSXYvHS64546945;     AIhGzsQfRmKMdkSXYvHS64546945 = AIhGzsQfRmKMdkSXYvHS31272936;     AIhGzsQfRmKMdkSXYvHS31272936 = AIhGzsQfRmKMdkSXYvHS96281907;     AIhGzsQfRmKMdkSXYvHS96281907 = AIhGzsQfRmKMdkSXYvHS31915675;     AIhGzsQfRmKMdkSXYvHS31915675 = AIhGzsQfRmKMdkSXYvHS99084230;     AIhGzsQfRmKMdkSXYvHS99084230 = AIhGzsQfRmKMdkSXYvHS45065949;     AIhGzsQfRmKMdkSXYvHS45065949 = AIhGzsQfRmKMdkSXYvHS28898116;     AIhGzsQfRmKMdkSXYvHS28898116 = AIhGzsQfRmKMdkSXYvHS97419041;     AIhGzsQfRmKMdkSXYvHS97419041 = AIhGzsQfRmKMdkSXYvHS77973651;     AIhGzsQfRmKMdkSXYvHS77973651 = AIhGzsQfRmKMdkSXYvHS58323357;     AIhGzsQfRmKMdkSXYvHS58323357 = AIhGzsQfRmKMdkSXYvHS63202224;     AIhGzsQfRmKMdkSXYvHS63202224 = AIhGzsQfRmKMdkSXYvHS66602;     AIhGzsQfRmKMdkSXYvHS66602 = AIhGzsQfRmKMdkSXYvHS61679141;     AIhGzsQfRmKMdkSXYvHS61679141 = AIhGzsQfRmKMdkSXYvHS65103428;     AIhGzsQfRmKMdkSXYvHS65103428 = AIhGzsQfRmKMdkSXYvHS68829866;     AIhGzsQfRmKMdkSXYvHS68829866 = AIhGzsQfRmKMdkSXYvHS24242993;     AIhGzsQfRmKMdkSXYvHS24242993 = AIhGzsQfRmKMdkSXYvHS85388104;     AIhGzsQfRmKMdkSXYvHS85388104 = AIhGzsQfRmKMdkSXYvHS64389966;     AIhGzsQfRmKMdkSXYvHS64389966 = AIhGzsQfRmKMdkSXYvHS31548015;     AIhGzsQfRmKMdkSXYvHS31548015 = AIhGzsQfRmKMdkSXYvHS13123818;     AIhGzsQfRmKMdkSXYvHS13123818 = AIhGzsQfRmKMdkSXYvHS32585788;     AIhGzsQfRmKMdkSXYvHS32585788 = AIhGzsQfRmKMdkSXYvHS80411853;     AIhGzsQfRmKMdkSXYvHS80411853 = AIhGzsQfRmKMdkSXYvHS52759664;     AIhGzsQfRmKMdkSXYvHS52759664 = AIhGzsQfRmKMdkSXYvHS36416153;     AIhGzsQfRmKMdkSXYvHS36416153 = AIhGzsQfRmKMdkSXYvHS77234127;     AIhGzsQfRmKMdkSXYvHS77234127 = AIhGzsQfRmKMdkSXYvHS56726870;     AIhGzsQfRmKMdkSXYvHS56726870 = AIhGzsQfRmKMdkSXYvHS40646018;     AIhGzsQfRmKMdkSXYvHS40646018 = AIhGzsQfRmKMdkSXYvHS8871894;     AIhGzsQfRmKMdkSXYvHS8871894 = AIhGzsQfRmKMdkSXYvHS88779233;     AIhGzsQfRmKMdkSXYvHS88779233 = AIhGzsQfRmKMdkSXYvHS68574812;     AIhGzsQfRmKMdkSXYvHS68574812 = AIhGzsQfRmKMdkSXYvHS64591933;     AIhGzsQfRmKMdkSXYvHS64591933 = AIhGzsQfRmKMdkSXYvHS85959023;     AIhGzsQfRmKMdkSXYvHS85959023 = AIhGzsQfRmKMdkSXYvHS89162071;     AIhGzsQfRmKMdkSXYvHS89162071 = AIhGzsQfRmKMdkSXYvHS31307347;     AIhGzsQfRmKMdkSXYvHS31307347 = AIhGzsQfRmKMdkSXYvHS10783107;     AIhGzsQfRmKMdkSXYvHS10783107 = AIhGzsQfRmKMdkSXYvHS2465836;     AIhGzsQfRmKMdkSXYvHS2465836 = AIhGzsQfRmKMdkSXYvHS7888841;     AIhGzsQfRmKMdkSXYvHS7888841 = AIhGzsQfRmKMdkSXYvHS44779484;     AIhGzsQfRmKMdkSXYvHS44779484 = AIhGzsQfRmKMdkSXYvHS98521638;     AIhGzsQfRmKMdkSXYvHS98521638 = AIhGzsQfRmKMdkSXYvHS84069154;     AIhGzsQfRmKMdkSXYvHS84069154 = AIhGzsQfRmKMdkSXYvHS61230311;     AIhGzsQfRmKMdkSXYvHS61230311 = AIhGzsQfRmKMdkSXYvHS7411220;     AIhGzsQfRmKMdkSXYvHS7411220 = AIhGzsQfRmKMdkSXYvHS28742258;     AIhGzsQfRmKMdkSXYvHS28742258 = AIhGzsQfRmKMdkSXYvHS57373941;     AIhGzsQfRmKMdkSXYvHS57373941 = AIhGzsQfRmKMdkSXYvHS26768835;     AIhGzsQfRmKMdkSXYvHS26768835 = AIhGzsQfRmKMdkSXYvHS78645057;     AIhGzsQfRmKMdkSXYvHS78645057 = AIhGzsQfRmKMdkSXYvHS23671401;     AIhGzsQfRmKMdkSXYvHS23671401 = AIhGzsQfRmKMdkSXYvHS66456275;     AIhGzsQfRmKMdkSXYvHS66456275 = AIhGzsQfRmKMdkSXYvHS51518819;     AIhGzsQfRmKMdkSXYvHS51518819 = AIhGzsQfRmKMdkSXYvHS62998791;     AIhGzsQfRmKMdkSXYvHS62998791 = AIhGzsQfRmKMdkSXYvHS37631422;     AIhGzsQfRmKMdkSXYvHS37631422 = AIhGzsQfRmKMdkSXYvHS29746615;     AIhGzsQfRmKMdkSXYvHS29746615 = AIhGzsQfRmKMdkSXYvHS44048337;     AIhGzsQfRmKMdkSXYvHS44048337 = AIhGzsQfRmKMdkSXYvHS3965186;     AIhGzsQfRmKMdkSXYvHS3965186 = AIhGzsQfRmKMdkSXYvHS27400619;     AIhGzsQfRmKMdkSXYvHS27400619 = AIhGzsQfRmKMdkSXYvHS84135092;     AIhGzsQfRmKMdkSXYvHS84135092 = AIhGzsQfRmKMdkSXYvHS78513271;     AIhGzsQfRmKMdkSXYvHS78513271 = AIhGzsQfRmKMdkSXYvHS59865755;     AIhGzsQfRmKMdkSXYvHS59865755 = AIhGzsQfRmKMdkSXYvHS54681548;     AIhGzsQfRmKMdkSXYvHS54681548 = AIhGzsQfRmKMdkSXYvHS42357360;     AIhGzsQfRmKMdkSXYvHS42357360 = AIhGzsQfRmKMdkSXYvHS4419931;     AIhGzsQfRmKMdkSXYvHS4419931 = AIhGzsQfRmKMdkSXYvHS20026223;     AIhGzsQfRmKMdkSXYvHS20026223 = AIhGzsQfRmKMdkSXYvHS8639808;     AIhGzsQfRmKMdkSXYvHS8639808 = AIhGzsQfRmKMdkSXYvHS9398839;     AIhGzsQfRmKMdkSXYvHS9398839 = AIhGzsQfRmKMdkSXYvHS93731423;     AIhGzsQfRmKMdkSXYvHS93731423 = AIhGzsQfRmKMdkSXYvHS77243201;     AIhGzsQfRmKMdkSXYvHS77243201 = AIhGzsQfRmKMdkSXYvHS10904531;     AIhGzsQfRmKMdkSXYvHS10904531 = AIhGzsQfRmKMdkSXYvHS30371795;     AIhGzsQfRmKMdkSXYvHS30371795 = AIhGzsQfRmKMdkSXYvHS54320321;     AIhGzsQfRmKMdkSXYvHS54320321 = AIhGzsQfRmKMdkSXYvHS66364030;     AIhGzsQfRmKMdkSXYvHS66364030 = AIhGzsQfRmKMdkSXYvHS16354152;     AIhGzsQfRmKMdkSXYvHS16354152 = AIhGzsQfRmKMdkSXYvHS40608620;     AIhGzsQfRmKMdkSXYvHS40608620 = AIhGzsQfRmKMdkSXYvHS65868328;     AIhGzsQfRmKMdkSXYvHS65868328 = AIhGzsQfRmKMdkSXYvHS47478860;     AIhGzsQfRmKMdkSXYvHS47478860 = AIhGzsQfRmKMdkSXYvHS51893506;     AIhGzsQfRmKMdkSXYvHS51893506 = AIhGzsQfRmKMdkSXYvHS25174568;     AIhGzsQfRmKMdkSXYvHS25174568 = AIhGzsQfRmKMdkSXYvHS51669595;     AIhGzsQfRmKMdkSXYvHS51669595 = AIhGzsQfRmKMdkSXYvHS6251074;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pYkGQHzrkGzdlbvortmO68214119() {     float cKbdCqsnhRtPHfqGFxvI6244603 = -923281200;    float cKbdCqsnhRtPHfqGFxvI29866070 = -936474626;    float cKbdCqsnhRtPHfqGFxvI76643264 = -972648845;    float cKbdCqsnhRtPHfqGFxvI18818632 = -456930663;    float cKbdCqsnhRtPHfqGFxvI13010851 = -652268161;    float cKbdCqsnhRtPHfqGFxvI11500592 = -68548986;    float cKbdCqsnhRtPHfqGFxvI66153204 = -420951600;    float cKbdCqsnhRtPHfqGFxvI98349153 = -363800026;    float cKbdCqsnhRtPHfqGFxvI72868591 = -330347570;    float cKbdCqsnhRtPHfqGFxvI1469775 = -175789318;    float cKbdCqsnhRtPHfqGFxvI27036514 = -22306332;    float cKbdCqsnhRtPHfqGFxvI29988976 = -692504362;    float cKbdCqsnhRtPHfqGFxvI21823973 = -728530314;    float cKbdCqsnhRtPHfqGFxvI32555879 = 18325150;    float cKbdCqsnhRtPHfqGFxvI30522651 = -434458911;    float cKbdCqsnhRtPHfqGFxvI6124252 = -341718359;    float cKbdCqsnhRtPHfqGFxvI76201444 = -201807506;    float cKbdCqsnhRtPHfqGFxvI10242317 = -520930161;    float cKbdCqsnhRtPHfqGFxvI43447796 = -64737288;    float cKbdCqsnhRtPHfqGFxvI66210344 = -525538922;    float cKbdCqsnhRtPHfqGFxvI55265775 = -497688767;    float cKbdCqsnhRtPHfqGFxvI95385268 = -809924188;    float cKbdCqsnhRtPHfqGFxvI96864803 = -914175872;    float cKbdCqsnhRtPHfqGFxvI8197054 = -135774732;    float cKbdCqsnhRtPHfqGFxvI59715625 = -400398187;    float cKbdCqsnhRtPHfqGFxvI12855985 = -774710433;    float cKbdCqsnhRtPHfqGFxvI2137257 = -364097046;    float cKbdCqsnhRtPHfqGFxvI34487755 = 4124040;    float cKbdCqsnhRtPHfqGFxvI57744395 = -578819496;    float cKbdCqsnhRtPHfqGFxvI4987987 = 27229787;    float cKbdCqsnhRtPHfqGFxvI30831720 = -324828986;    float cKbdCqsnhRtPHfqGFxvI62697508 = -819033285;    float cKbdCqsnhRtPHfqGFxvI43775780 = -77457651;    float cKbdCqsnhRtPHfqGFxvI63485061 = -519561265;    float cKbdCqsnhRtPHfqGFxvI82424639 = -344910878;    float cKbdCqsnhRtPHfqGFxvI4507331 = -527362941;    float cKbdCqsnhRtPHfqGFxvI77044796 = -482636118;    float cKbdCqsnhRtPHfqGFxvI38747546 = -892941889;    float cKbdCqsnhRtPHfqGFxvI59556589 = -280149337;    float cKbdCqsnhRtPHfqGFxvI57260997 = 79972933;    float cKbdCqsnhRtPHfqGFxvI52245634 = -176490878;    float cKbdCqsnhRtPHfqGFxvI17245676 = -330782148;    float cKbdCqsnhRtPHfqGFxvI73407177 = -911383094;    float cKbdCqsnhRtPHfqGFxvI91817685 = -168306787;    float cKbdCqsnhRtPHfqGFxvI64429905 = -90475305;    float cKbdCqsnhRtPHfqGFxvI93187631 = -520928933;    float cKbdCqsnhRtPHfqGFxvI64113945 = -76885249;    float cKbdCqsnhRtPHfqGFxvI52617708 = -42235222;    float cKbdCqsnhRtPHfqGFxvI75385260 = -912122133;    float cKbdCqsnhRtPHfqGFxvI60691440 = -31407359;    float cKbdCqsnhRtPHfqGFxvI22131264 = -471810466;    float cKbdCqsnhRtPHfqGFxvI3326491 = -636019756;    float cKbdCqsnhRtPHfqGFxvI44023160 = -774470424;    float cKbdCqsnhRtPHfqGFxvI74842204 = -141485135;    float cKbdCqsnhRtPHfqGFxvI12948344 = -401311337;    float cKbdCqsnhRtPHfqGFxvI10859334 = -13357012;    float cKbdCqsnhRtPHfqGFxvI33001267 = 77701245;    float cKbdCqsnhRtPHfqGFxvI68446211 = -736874114;    float cKbdCqsnhRtPHfqGFxvI59103007 = 43467524;    float cKbdCqsnhRtPHfqGFxvI154866 = -877557729;    float cKbdCqsnhRtPHfqGFxvI9363335 = -704451940;    float cKbdCqsnhRtPHfqGFxvI31665450 = -325075641;    float cKbdCqsnhRtPHfqGFxvI40604759 = -784980530;    float cKbdCqsnhRtPHfqGFxvI67880605 = -257577357;    float cKbdCqsnhRtPHfqGFxvI70638054 = -850960333;    float cKbdCqsnhRtPHfqGFxvI64339005 = -203273047;    float cKbdCqsnhRtPHfqGFxvI86213195 = -515046711;    float cKbdCqsnhRtPHfqGFxvI58338911 = -108969050;    float cKbdCqsnhRtPHfqGFxvI50131239 = -636763972;    float cKbdCqsnhRtPHfqGFxvI26015320 = -907095970;    float cKbdCqsnhRtPHfqGFxvI29079455 = -859082241;    float cKbdCqsnhRtPHfqGFxvI37453899 = -308865617;    float cKbdCqsnhRtPHfqGFxvI50685727 = -140780824;    float cKbdCqsnhRtPHfqGFxvI86186799 = -44710222;    float cKbdCqsnhRtPHfqGFxvI13964711 = -249048045;    float cKbdCqsnhRtPHfqGFxvI38020100 = -66906620;    float cKbdCqsnhRtPHfqGFxvI21978092 = -898541094;    float cKbdCqsnhRtPHfqGFxvI5047118 = -645869085;    float cKbdCqsnhRtPHfqGFxvI43767149 = 54700573;    float cKbdCqsnhRtPHfqGFxvI66527993 = -879469255;    float cKbdCqsnhRtPHfqGFxvI48742039 = -597825184;    float cKbdCqsnhRtPHfqGFxvI49519549 = -221861824;    float cKbdCqsnhRtPHfqGFxvI59102495 = -83753827;    float cKbdCqsnhRtPHfqGFxvI97052954 = -447412138;    float cKbdCqsnhRtPHfqGFxvI82856722 = -500959747;    float cKbdCqsnhRtPHfqGFxvI27505230 = -688809230;    float cKbdCqsnhRtPHfqGFxvI18674348 = 55437138;    float cKbdCqsnhRtPHfqGFxvI68933576 = -935972517;    float cKbdCqsnhRtPHfqGFxvI50536718 = -18249928;    float cKbdCqsnhRtPHfqGFxvI71565306 = -231553866;    float cKbdCqsnhRtPHfqGFxvI71506064 = -505064187;    float cKbdCqsnhRtPHfqGFxvI8598586 = -745762005;    float cKbdCqsnhRtPHfqGFxvI79644539 = -836409414;    float cKbdCqsnhRtPHfqGFxvI59401723 = -402591609;    float cKbdCqsnhRtPHfqGFxvI47897662 = -215575128;    float cKbdCqsnhRtPHfqGFxvI20580185 = -851415237;    float cKbdCqsnhRtPHfqGFxvI76640917 = -545801618;    float cKbdCqsnhRtPHfqGFxvI5526572 = -553805737;    float cKbdCqsnhRtPHfqGFxvI21179632 = -317346454;    float cKbdCqsnhRtPHfqGFxvI90900 = -923281200;     cKbdCqsnhRtPHfqGFxvI6244603 = cKbdCqsnhRtPHfqGFxvI29866070;     cKbdCqsnhRtPHfqGFxvI29866070 = cKbdCqsnhRtPHfqGFxvI76643264;     cKbdCqsnhRtPHfqGFxvI76643264 = cKbdCqsnhRtPHfqGFxvI18818632;     cKbdCqsnhRtPHfqGFxvI18818632 = cKbdCqsnhRtPHfqGFxvI13010851;     cKbdCqsnhRtPHfqGFxvI13010851 = cKbdCqsnhRtPHfqGFxvI11500592;     cKbdCqsnhRtPHfqGFxvI11500592 = cKbdCqsnhRtPHfqGFxvI66153204;     cKbdCqsnhRtPHfqGFxvI66153204 = cKbdCqsnhRtPHfqGFxvI98349153;     cKbdCqsnhRtPHfqGFxvI98349153 = cKbdCqsnhRtPHfqGFxvI72868591;     cKbdCqsnhRtPHfqGFxvI72868591 = cKbdCqsnhRtPHfqGFxvI1469775;     cKbdCqsnhRtPHfqGFxvI1469775 = cKbdCqsnhRtPHfqGFxvI27036514;     cKbdCqsnhRtPHfqGFxvI27036514 = cKbdCqsnhRtPHfqGFxvI29988976;     cKbdCqsnhRtPHfqGFxvI29988976 = cKbdCqsnhRtPHfqGFxvI21823973;     cKbdCqsnhRtPHfqGFxvI21823973 = cKbdCqsnhRtPHfqGFxvI32555879;     cKbdCqsnhRtPHfqGFxvI32555879 = cKbdCqsnhRtPHfqGFxvI30522651;     cKbdCqsnhRtPHfqGFxvI30522651 = cKbdCqsnhRtPHfqGFxvI6124252;     cKbdCqsnhRtPHfqGFxvI6124252 = cKbdCqsnhRtPHfqGFxvI76201444;     cKbdCqsnhRtPHfqGFxvI76201444 = cKbdCqsnhRtPHfqGFxvI10242317;     cKbdCqsnhRtPHfqGFxvI10242317 = cKbdCqsnhRtPHfqGFxvI43447796;     cKbdCqsnhRtPHfqGFxvI43447796 = cKbdCqsnhRtPHfqGFxvI66210344;     cKbdCqsnhRtPHfqGFxvI66210344 = cKbdCqsnhRtPHfqGFxvI55265775;     cKbdCqsnhRtPHfqGFxvI55265775 = cKbdCqsnhRtPHfqGFxvI95385268;     cKbdCqsnhRtPHfqGFxvI95385268 = cKbdCqsnhRtPHfqGFxvI96864803;     cKbdCqsnhRtPHfqGFxvI96864803 = cKbdCqsnhRtPHfqGFxvI8197054;     cKbdCqsnhRtPHfqGFxvI8197054 = cKbdCqsnhRtPHfqGFxvI59715625;     cKbdCqsnhRtPHfqGFxvI59715625 = cKbdCqsnhRtPHfqGFxvI12855985;     cKbdCqsnhRtPHfqGFxvI12855985 = cKbdCqsnhRtPHfqGFxvI2137257;     cKbdCqsnhRtPHfqGFxvI2137257 = cKbdCqsnhRtPHfqGFxvI34487755;     cKbdCqsnhRtPHfqGFxvI34487755 = cKbdCqsnhRtPHfqGFxvI57744395;     cKbdCqsnhRtPHfqGFxvI57744395 = cKbdCqsnhRtPHfqGFxvI4987987;     cKbdCqsnhRtPHfqGFxvI4987987 = cKbdCqsnhRtPHfqGFxvI30831720;     cKbdCqsnhRtPHfqGFxvI30831720 = cKbdCqsnhRtPHfqGFxvI62697508;     cKbdCqsnhRtPHfqGFxvI62697508 = cKbdCqsnhRtPHfqGFxvI43775780;     cKbdCqsnhRtPHfqGFxvI43775780 = cKbdCqsnhRtPHfqGFxvI63485061;     cKbdCqsnhRtPHfqGFxvI63485061 = cKbdCqsnhRtPHfqGFxvI82424639;     cKbdCqsnhRtPHfqGFxvI82424639 = cKbdCqsnhRtPHfqGFxvI4507331;     cKbdCqsnhRtPHfqGFxvI4507331 = cKbdCqsnhRtPHfqGFxvI77044796;     cKbdCqsnhRtPHfqGFxvI77044796 = cKbdCqsnhRtPHfqGFxvI38747546;     cKbdCqsnhRtPHfqGFxvI38747546 = cKbdCqsnhRtPHfqGFxvI59556589;     cKbdCqsnhRtPHfqGFxvI59556589 = cKbdCqsnhRtPHfqGFxvI57260997;     cKbdCqsnhRtPHfqGFxvI57260997 = cKbdCqsnhRtPHfqGFxvI52245634;     cKbdCqsnhRtPHfqGFxvI52245634 = cKbdCqsnhRtPHfqGFxvI17245676;     cKbdCqsnhRtPHfqGFxvI17245676 = cKbdCqsnhRtPHfqGFxvI73407177;     cKbdCqsnhRtPHfqGFxvI73407177 = cKbdCqsnhRtPHfqGFxvI91817685;     cKbdCqsnhRtPHfqGFxvI91817685 = cKbdCqsnhRtPHfqGFxvI64429905;     cKbdCqsnhRtPHfqGFxvI64429905 = cKbdCqsnhRtPHfqGFxvI93187631;     cKbdCqsnhRtPHfqGFxvI93187631 = cKbdCqsnhRtPHfqGFxvI64113945;     cKbdCqsnhRtPHfqGFxvI64113945 = cKbdCqsnhRtPHfqGFxvI52617708;     cKbdCqsnhRtPHfqGFxvI52617708 = cKbdCqsnhRtPHfqGFxvI75385260;     cKbdCqsnhRtPHfqGFxvI75385260 = cKbdCqsnhRtPHfqGFxvI60691440;     cKbdCqsnhRtPHfqGFxvI60691440 = cKbdCqsnhRtPHfqGFxvI22131264;     cKbdCqsnhRtPHfqGFxvI22131264 = cKbdCqsnhRtPHfqGFxvI3326491;     cKbdCqsnhRtPHfqGFxvI3326491 = cKbdCqsnhRtPHfqGFxvI44023160;     cKbdCqsnhRtPHfqGFxvI44023160 = cKbdCqsnhRtPHfqGFxvI74842204;     cKbdCqsnhRtPHfqGFxvI74842204 = cKbdCqsnhRtPHfqGFxvI12948344;     cKbdCqsnhRtPHfqGFxvI12948344 = cKbdCqsnhRtPHfqGFxvI10859334;     cKbdCqsnhRtPHfqGFxvI10859334 = cKbdCqsnhRtPHfqGFxvI33001267;     cKbdCqsnhRtPHfqGFxvI33001267 = cKbdCqsnhRtPHfqGFxvI68446211;     cKbdCqsnhRtPHfqGFxvI68446211 = cKbdCqsnhRtPHfqGFxvI59103007;     cKbdCqsnhRtPHfqGFxvI59103007 = cKbdCqsnhRtPHfqGFxvI154866;     cKbdCqsnhRtPHfqGFxvI154866 = cKbdCqsnhRtPHfqGFxvI9363335;     cKbdCqsnhRtPHfqGFxvI9363335 = cKbdCqsnhRtPHfqGFxvI31665450;     cKbdCqsnhRtPHfqGFxvI31665450 = cKbdCqsnhRtPHfqGFxvI40604759;     cKbdCqsnhRtPHfqGFxvI40604759 = cKbdCqsnhRtPHfqGFxvI67880605;     cKbdCqsnhRtPHfqGFxvI67880605 = cKbdCqsnhRtPHfqGFxvI70638054;     cKbdCqsnhRtPHfqGFxvI70638054 = cKbdCqsnhRtPHfqGFxvI64339005;     cKbdCqsnhRtPHfqGFxvI64339005 = cKbdCqsnhRtPHfqGFxvI86213195;     cKbdCqsnhRtPHfqGFxvI86213195 = cKbdCqsnhRtPHfqGFxvI58338911;     cKbdCqsnhRtPHfqGFxvI58338911 = cKbdCqsnhRtPHfqGFxvI50131239;     cKbdCqsnhRtPHfqGFxvI50131239 = cKbdCqsnhRtPHfqGFxvI26015320;     cKbdCqsnhRtPHfqGFxvI26015320 = cKbdCqsnhRtPHfqGFxvI29079455;     cKbdCqsnhRtPHfqGFxvI29079455 = cKbdCqsnhRtPHfqGFxvI37453899;     cKbdCqsnhRtPHfqGFxvI37453899 = cKbdCqsnhRtPHfqGFxvI50685727;     cKbdCqsnhRtPHfqGFxvI50685727 = cKbdCqsnhRtPHfqGFxvI86186799;     cKbdCqsnhRtPHfqGFxvI86186799 = cKbdCqsnhRtPHfqGFxvI13964711;     cKbdCqsnhRtPHfqGFxvI13964711 = cKbdCqsnhRtPHfqGFxvI38020100;     cKbdCqsnhRtPHfqGFxvI38020100 = cKbdCqsnhRtPHfqGFxvI21978092;     cKbdCqsnhRtPHfqGFxvI21978092 = cKbdCqsnhRtPHfqGFxvI5047118;     cKbdCqsnhRtPHfqGFxvI5047118 = cKbdCqsnhRtPHfqGFxvI43767149;     cKbdCqsnhRtPHfqGFxvI43767149 = cKbdCqsnhRtPHfqGFxvI66527993;     cKbdCqsnhRtPHfqGFxvI66527993 = cKbdCqsnhRtPHfqGFxvI48742039;     cKbdCqsnhRtPHfqGFxvI48742039 = cKbdCqsnhRtPHfqGFxvI49519549;     cKbdCqsnhRtPHfqGFxvI49519549 = cKbdCqsnhRtPHfqGFxvI59102495;     cKbdCqsnhRtPHfqGFxvI59102495 = cKbdCqsnhRtPHfqGFxvI97052954;     cKbdCqsnhRtPHfqGFxvI97052954 = cKbdCqsnhRtPHfqGFxvI82856722;     cKbdCqsnhRtPHfqGFxvI82856722 = cKbdCqsnhRtPHfqGFxvI27505230;     cKbdCqsnhRtPHfqGFxvI27505230 = cKbdCqsnhRtPHfqGFxvI18674348;     cKbdCqsnhRtPHfqGFxvI18674348 = cKbdCqsnhRtPHfqGFxvI68933576;     cKbdCqsnhRtPHfqGFxvI68933576 = cKbdCqsnhRtPHfqGFxvI50536718;     cKbdCqsnhRtPHfqGFxvI50536718 = cKbdCqsnhRtPHfqGFxvI71565306;     cKbdCqsnhRtPHfqGFxvI71565306 = cKbdCqsnhRtPHfqGFxvI71506064;     cKbdCqsnhRtPHfqGFxvI71506064 = cKbdCqsnhRtPHfqGFxvI8598586;     cKbdCqsnhRtPHfqGFxvI8598586 = cKbdCqsnhRtPHfqGFxvI79644539;     cKbdCqsnhRtPHfqGFxvI79644539 = cKbdCqsnhRtPHfqGFxvI59401723;     cKbdCqsnhRtPHfqGFxvI59401723 = cKbdCqsnhRtPHfqGFxvI47897662;     cKbdCqsnhRtPHfqGFxvI47897662 = cKbdCqsnhRtPHfqGFxvI20580185;     cKbdCqsnhRtPHfqGFxvI20580185 = cKbdCqsnhRtPHfqGFxvI76640917;     cKbdCqsnhRtPHfqGFxvI76640917 = cKbdCqsnhRtPHfqGFxvI5526572;     cKbdCqsnhRtPHfqGFxvI5526572 = cKbdCqsnhRtPHfqGFxvI21179632;     cKbdCqsnhRtPHfqGFxvI21179632 = cKbdCqsnhRtPHfqGFxvI90900;     cKbdCqsnhRtPHfqGFxvI90900 = cKbdCqsnhRtPHfqGFxvI6244603;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void VGWhJBBcDhWKbUHsnhYW22630332() {     float dNFmmRZXFEBpdtRhNeJb76896269 = -441430330;    float dNFmmRZXFEBpdtRhNeJb98539587 = -808117925;    float dNFmmRZXFEBpdtRhNeJb56612184 = -314943304;    float dNFmmRZXFEBpdtRhNeJb58749918 = -647069052;    float dNFmmRZXFEBpdtRhNeJb34461172 = -435674508;    float dNFmmRZXFEBpdtRhNeJb1500355 = -870130137;    float dNFmmRZXFEBpdtRhNeJb84532100 = -419727310;    float dNFmmRZXFEBpdtRhNeJb55191534 = -710617116;    float dNFmmRZXFEBpdtRhNeJb18733646 = -423694898;    float dNFmmRZXFEBpdtRhNeJb24436236 = -476884669;    float dNFmmRZXFEBpdtRhNeJb69285674 = -57208262;    float dNFmmRZXFEBpdtRhNeJb75535547 = 91731845;    float dNFmmRZXFEBpdtRhNeJb1651360 = -638067373;    float dNFmmRZXFEBpdtRhNeJb16402913 = -882703060;    float dNFmmRZXFEBpdtRhNeJb13242670 = -584208914;    float dNFmmRZXFEBpdtRhNeJb38652892 = 88298055;    float dNFmmRZXFEBpdtRhNeJb70054271 = -110992636;    float dNFmmRZXFEBpdtRhNeJb30535811 = -510096357;    float dNFmmRZXFEBpdtRhNeJb25155578 = -771925230;    float dNFmmRZXFEBpdtRhNeJb24997054 = 32094773;    float dNFmmRZXFEBpdtRhNeJb10401530 = -406367908;    float dNFmmRZXFEBpdtRhNeJb56781033 = -41209494;    float dNFmmRZXFEBpdtRhNeJb98808612 = -382019016;    float dNFmmRZXFEBpdtRhNeJb42046591 = -828175638;    float dNFmmRZXFEBpdtRhNeJb23840014 = -76302875;    float dNFmmRZXFEBpdtRhNeJb43453566 = -872994877;    float dNFmmRZXFEBpdtRhNeJb10398043 = -759274822;    float dNFmmRZXFEBpdtRhNeJb42269981 = -500999727;    float dNFmmRZXFEBpdtRhNeJb89886883 = -873177713;    float dNFmmRZXFEBpdtRhNeJb44160082 = -531165283;    float dNFmmRZXFEBpdtRhNeJb37903717 = -73262671;    float dNFmmRZXFEBpdtRhNeJb7208969 = -47330448;    float dNFmmRZXFEBpdtRhNeJb26721141 = -24419260;    float dNFmmRZXFEBpdtRhNeJb28850037 = -83090949;    float dNFmmRZXFEBpdtRhNeJb60448876 = -191857657;    float dNFmmRZXFEBpdtRhNeJb64500590 = -606589489;    float dNFmmRZXFEBpdtRhNeJb20836282 = -671229325;    float dNFmmRZXFEBpdtRhNeJb91540034 = -677054997;    float dNFmmRZXFEBpdtRhNeJb38089878 = -235877973;    float dNFmmRZXFEBpdtRhNeJb52364295 = -537635430;    float dNFmmRZXFEBpdtRhNeJb12318918 = -560509756;    float dNFmmRZXFEBpdtRhNeJb45728666 = -100171217;    float dNFmmRZXFEBpdtRhNeJb22374137 = -898137146;    float dNFmmRZXFEBpdtRhNeJb98325986 = -240281759;    float dNFmmRZXFEBpdtRhNeJb60383964 = -399121700;    float dNFmmRZXFEBpdtRhNeJb99593466 = -279266088;    float dNFmmRZXFEBpdtRhNeJb90245148 = -192955048;    float dNFmmRZXFEBpdtRhNeJb18231918 = -494365518;    float dNFmmRZXFEBpdtRhNeJb80826712 = -897886103;    float dNFmmRZXFEBpdtRhNeJb69118927 = -314416237;    float dNFmmRZXFEBpdtRhNeJb90320587 = -18382587;    float dNFmmRZXFEBpdtRhNeJb45197995 = -605239478;    float dNFmmRZXFEBpdtRhNeJb99098817 = -702268466;    float dNFmmRZXFEBpdtRhNeJb65069509 = -802366632;    float dNFmmRZXFEBpdtRhNeJb12228803 = -512252251;    float dNFmmRZXFEBpdtRhNeJb20115236 = -300220837;    float dNFmmRZXFEBpdtRhNeJb99730975 = -326098909;    float dNFmmRZXFEBpdtRhNeJb14565593 = -486767667;    float dNFmmRZXFEBpdtRhNeJb34909905 = -470766178;    float dNFmmRZXFEBpdtRhNeJb91007606 = -562679631;    float dNFmmRZXFEBpdtRhNeJb91102311 = -10855316;    float dNFmmRZXFEBpdtRhNeJb42262120 = -918727583;    float dNFmmRZXFEBpdtRhNeJb65304651 = -837439403;    float dNFmmRZXFEBpdtRhNeJb74573564 = -892529615;    float dNFmmRZXFEBpdtRhNeJb86532519 = -303621998;    float dNFmmRZXFEBpdtRhNeJb62076705 = 90122186;    float dNFmmRZXFEBpdtRhNeJb48814406 = -883848895;    float dNFmmRZXFEBpdtRhNeJb72801323 = -454976424;    float dNFmmRZXFEBpdtRhNeJb55954037 = -590845403;    float dNFmmRZXFEBpdtRhNeJb48742080 = -977619426;    float dNFmmRZXFEBpdtRhNeJb17816610 = -240472621;    float dNFmmRZXFEBpdtRhNeJb78514236 = -433937640;    float dNFmmRZXFEBpdtRhNeJb92445932 = -174218384;    float dNFmmRZXFEBpdtRhNeJb72791282 = -134289801;    float dNFmmRZXFEBpdtRhNeJb12678136 = -407395471;    float dNFmmRZXFEBpdtRhNeJb64672863 = -206196691;    float dNFmmRZXFEBpdtRhNeJb34406896 = -143072348;    float dNFmmRZXFEBpdtRhNeJb482626 = -41737258;    float dNFmmRZXFEBpdtRhNeJb81662626 = -329053938;    float dNFmmRZXFEBpdtRhNeJb24246547 = -797036788;    float dNFmmRZXFEBpdtRhNeJb53208417 = -580039830;    float dNFmmRZXFEBpdtRhNeJb92166124 = -164909304;    float dNFmmRZXFEBpdtRhNeJb61443268 = -603113625;    float dNFmmRZXFEBpdtRhNeJb20767957 = -458761476;    float dNFmmRZXFEBpdtRhNeJb53839494 = -412782696;    float dNFmmRZXFEBpdtRhNeJb92705721 = -468023194;    float dNFmmRZXFEBpdtRhNeJb8110152 = -345061983;    float dNFmmRZXFEBpdtRhNeJb61651632 = -222052629;    float dNFmmRZXFEBpdtRhNeJb16621234 = -570838698;    float dNFmmRZXFEBpdtRhNeJb40333640 = -891636821;    float dNFmmRZXFEBpdtRhNeJb64769614 = -180490580;    float dNFmmRZXFEBpdtRhNeJb6270690 = -84461659;    float dNFmmRZXFEBpdtRhNeJb56630129 = -106288820;    float dNFmmRZXFEBpdtRhNeJb47082271 = -673198343;    float dNFmmRZXFEBpdtRhNeJb61261983 = -426780114;    float dNFmmRZXFEBpdtRhNeJb70056798 = -641782173;    float dNFmmRZXFEBpdtRhNeJb80424015 = -262731814;    float dNFmmRZXFEBpdtRhNeJb47800573 = 94392469;    float dNFmmRZXFEBpdtRhNeJb11793468 = -936659761;    float dNFmmRZXFEBpdtRhNeJb98307258 = -441430330;     dNFmmRZXFEBpdtRhNeJb76896269 = dNFmmRZXFEBpdtRhNeJb98539587;     dNFmmRZXFEBpdtRhNeJb98539587 = dNFmmRZXFEBpdtRhNeJb56612184;     dNFmmRZXFEBpdtRhNeJb56612184 = dNFmmRZXFEBpdtRhNeJb58749918;     dNFmmRZXFEBpdtRhNeJb58749918 = dNFmmRZXFEBpdtRhNeJb34461172;     dNFmmRZXFEBpdtRhNeJb34461172 = dNFmmRZXFEBpdtRhNeJb1500355;     dNFmmRZXFEBpdtRhNeJb1500355 = dNFmmRZXFEBpdtRhNeJb84532100;     dNFmmRZXFEBpdtRhNeJb84532100 = dNFmmRZXFEBpdtRhNeJb55191534;     dNFmmRZXFEBpdtRhNeJb55191534 = dNFmmRZXFEBpdtRhNeJb18733646;     dNFmmRZXFEBpdtRhNeJb18733646 = dNFmmRZXFEBpdtRhNeJb24436236;     dNFmmRZXFEBpdtRhNeJb24436236 = dNFmmRZXFEBpdtRhNeJb69285674;     dNFmmRZXFEBpdtRhNeJb69285674 = dNFmmRZXFEBpdtRhNeJb75535547;     dNFmmRZXFEBpdtRhNeJb75535547 = dNFmmRZXFEBpdtRhNeJb1651360;     dNFmmRZXFEBpdtRhNeJb1651360 = dNFmmRZXFEBpdtRhNeJb16402913;     dNFmmRZXFEBpdtRhNeJb16402913 = dNFmmRZXFEBpdtRhNeJb13242670;     dNFmmRZXFEBpdtRhNeJb13242670 = dNFmmRZXFEBpdtRhNeJb38652892;     dNFmmRZXFEBpdtRhNeJb38652892 = dNFmmRZXFEBpdtRhNeJb70054271;     dNFmmRZXFEBpdtRhNeJb70054271 = dNFmmRZXFEBpdtRhNeJb30535811;     dNFmmRZXFEBpdtRhNeJb30535811 = dNFmmRZXFEBpdtRhNeJb25155578;     dNFmmRZXFEBpdtRhNeJb25155578 = dNFmmRZXFEBpdtRhNeJb24997054;     dNFmmRZXFEBpdtRhNeJb24997054 = dNFmmRZXFEBpdtRhNeJb10401530;     dNFmmRZXFEBpdtRhNeJb10401530 = dNFmmRZXFEBpdtRhNeJb56781033;     dNFmmRZXFEBpdtRhNeJb56781033 = dNFmmRZXFEBpdtRhNeJb98808612;     dNFmmRZXFEBpdtRhNeJb98808612 = dNFmmRZXFEBpdtRhNeJb42046591;     dNFmmRZXFEBpdtRhNeJb42046591 = dNFmmRZXFEBpdtRhNeJb23840014;     dNFmmRZXFEBpdtRhNeJb23840014 = dNFmmRZXFEBpdtRhNeJb43453566;     dNFmmRZXFEBpdtRhNeJb43453566 = dNFmmRZXFEBpdtRhNeJb10398043;     dNFmmRZXFEBpdtRhNeJb10398043 = dNFmmRZXFEBpdtRhNeJb42269981;     dNFmmRZXFEBpdtRhNeJb42269981 = dNFmmRZXFEBpdtRhNeJb89886883;     dNFmmRZXFEBpdtRhNeJb89886883 = dNFmmRZXFEBpdtRhNeJb44160082;     dNFmmRZXFEBpdtRhNeJb44160082 = dNFmmRZXFEBpdtRhNeJb37903717;     dNFmmRZXFEBpdtRhNeJb37903717 = dNFmmRZXFEBpdtRhNeJb7208969;     dNFmmRZXFEBpdtRhNeJb7208969 = dNFmmRZXFEBpdtRhNeJb26721141;     dNFmmRZXFEBpdtRhNeJb26721141 = dNFmmRZXFEBpdtRhNeJb28850037;     dNFmmRZXFEBpdtRhNeJb28850037 = dNFmmRZXFEBpdtRhNeJb60448876;     dNFmmRZXFEBpdtRhNeJb60448876 = dNFmmRZXFEBpdtRhNeJb64500590;     dNFmmRZXFEBpdtRhNeJb64500590 = dNFmmRZXFEBpdtRhNeJb20836282;     dNFmmRZXFEBpdtRhNeJb20836282 = dNFmmRZXFEBpdtRhNeJb91540034;     dNFmmRZXFEBpdtRhNeJb91540034 = dNFmmRZXFEBpdtRhNeJb38089878;     dNFmmRZXFEBpdtRhNeJb38089878 = dNFmmRZXFEBpdtRhNeJb52364295;     dNFmmRZXFEBpdtRhNeJb52364295 = dNFmmRZXFEBpdtRhNeJb12318918;     dNFmmRZXFEBpdtRhNeJb12318918 = dNFmmRZXFEBpdtRhNeJb45728666;     dNFmmRZXFEBpdtRhNeJb45728666 = dNFmmRZXFEBpdtRhNeJb22374137;     dNFmmRZXFEBpdtRhNeJb22374137 = dNFmmRZXFEBpdtRhNeJb98325986;     dNFmmRZXFEBpdtRhNeJb98325986 = dNFmmRZXFEBpdtRhNeJb60383964;     dNFmmRZXFEBpdtRhNeJb60383964 = dNFmmRZXFEBpdtRhNeJb99593466;     dNFmmRZXFEBpdtRhNeJb99593466 = dNFmmRZXFEBpdtRhNeJb90245148;     dNFmmRZXFEBpdtRhNeJb90245148 = dNFmmRZXFEBpdtRhNeJb18231918;     dNFmmRZXFEBpdtRhNeJb18231918 = dNFmmRZXFEBpdtRhNeJb80826712;     dNFmmRZXFEBpdtRhNeJb80826712 = dNFmmRZXFEBpdtRhNeJb69118927;     dNFmmRZXFEBpdtRhNeJb69118927 = dNFmmRZXFEBpdtRhNeJb90320587;     dNFmmRZXFEBpdtRhNeJb90320587 = dNFmmRZXFEBpdtRhNeJb45197995;     dNFmmRZXFEBpdtRhNeJb45197995 = dNFmmRZXFEBpdtRhNeJb99098817;     dNFmmRZXFEBpdtRhNeJb99098817 = dNFmmRZXFEBpdtRhNeJb65069509;     dNFmmRZXFEBpdtRhNeJb65069509 = dNFmmRZXFEBpdtRhNeJb12228803;     dNFmmRZXFEBpdtRhNeJb12228803 = dNFmmRZXFEBpdtRhNeJb20115236;     dNFmmRZXFEBpdtRhNeJb20115236 = dNFmmRZXFEBpdtRhNeJb99730975;     dNFmmRZXFEBpdtRhNeJb99730975 = dNFmmRZXFEBpdtRhNeJb14565593;     dNFmmRZXFEBpdtRhNeJb14565593 = dNFmmRZXFEBpdtRhNeJb34909905;     dNFmmRZXFEBpdtRhNeJb34909905 = dNFmmRZXFEBpdtRhNeJb91007606;     dNFmmRZXFEBpdtRhNeJb91007606 = dNFmmRZXFEBpdtRhNeJb91102311;     dNFmmRZXFEBpdtRhNeJb91102311 = dNFmmRZXFEBpdtRhNeJb42262120;     dNFmmRZXFEBpdtRhNeJb42262120 = dNFmmRZXFEBpdtRhNeJb65304651;     dNFmmRZXFEBpdtRhNeJb65304651 = dNFmmRZXFEBpdtRhNeJb74573564;     dNFmmRZXFEBpdtRhNeJb74573564 = dNFmmRZXFEBpdtRhNeJb86532519;     dNFmmRZXFEBpdtRhNeJb86532519 = dNFmmRZXFEBpdtRhNeJb62076705;     dNFmmRZXFEBpdtRhNeJb62076705 = dNFmmRZXFEBpdtRhNeJb48814406;     dNFmmRZXFEBpdtRhNeJb48814406 = dNFmmRZXFEBpdtRhNeJb72801323;     dNFmmRZXFEBpdtRhNeJb72801323 = dNFmmRZXFEBpdtRhNeJb55954037;     dNFmmRZXFEBpdtRhNeJb55954037 = dNFmmRZXFEBpdtRhNeJb48742080;     dNFmmRZXFEBpdtRhNeJb48742080 = dNFmmRZXFEBpdtRhNeJb17816610;     dNFmmRZXFEBpdtRhNeJb17816610 = dNFmmRZXFEBpdtRhNeJb78514236;     dNFmmRZXFEBpdtRhNeJb78514236 = dNFmmRZXFEBpdtRhNeJb92445932;     dNFmmRZXFEBpdtRhNeJb92445932 = dNFmmRZXFEBpdtRhNeJb72791282;     dNFmmRZXFEBpdtRhNeJb72791282 = dNFmmRZXFEBpdtRhNeJb12678136;     dNFmmRZXFEBpdtRhNeJb12678136 = dNFmmRZXFEBpdtRhNeJb64672863;     dNFmmRZXFEBpdtRhNeJb64672863 = dNFmmRZXFEBpdtRhNeJb34406896;     dNFmmRZXFEBpdtRhNeJb34406896 = dNFmmRZXFEBpdtRhNeJb482626;     dNFmmRZXFEBpdtRhNeJb482626 = dNFmmRZXFEBpdtRhNeJb81662626;     dNFmmRZXFEBpdtRhNeJb81662626 = dNFmmRZXFEBpdtRhNeJb24246547;     dNFmmRZXFEBpdtRhNeJb24246547 = dNFmmRZXFEBpdtRhNeJb53208417;     dNFmmRZXFEBpdtRhNeJb53208417 = dNFmmRZXFEBpdtRhNeJb92166124;     dNFmmRZXFEBpdtRhNeJb92166124 = dNFmmRZXFEBpdtRhNeJb61443268;     dNFmmRZXFEBpdtRhNeJb61443268 = dNFmmRZXFEBpdtRhNeJb20767957;     dNFmmRZXFEBpdtRhNeJb20767957 = dNFmmRZXFEBpdtRhNeJb53839494;     dNFmmRZXFEBpdtRhNeJb53839494 = dNFmmRZXFEBpdtRhNeJb92705721;     dNFmmRZXFEBpdtRhNeJb92705721 = dNFmmRZXFEBpdtRhNeJb8110152;     dNFmmRZXFEBpdtRhNeJb8110152 = dNFmmRZXFEBpdtRhNeJb61651632;     dNFmmRZXFEBpdtRhNeJb61651632 = dNFmmRZXFEBpdtRhNeJb16621234;     dNFmmRZXFEBpdtRhNeJb16621234 = dNFmmRZXFEBpdtRhNeJb40333640;     dNFmmRZXFEBpdtRhNeJb40333640 = dNFmmRZXFEBpdtRhNeJb64769614;     dNFmmRZXFEBpdtRhNeJb64769614 = dNFmmRZXFEBpdtRhNeJb6270690;     dNFmmRZXFEBpdtRhNeJb6270690 = dNFmmRZXFEBpdtRhNeJb56630129;     dNFmmRZXFEBpdtRhNeJb56630129 = dNFmmRZXFEBpdtRhNeJb47082271;     dNFmmRZXFEBpdtRhNeJb47082271 = dNFmmRZXFEBpdtRhNeJb61261983;     dNFmmRZXFEBpdtRhNeJb61261983 = dNFmmRZXFEBpdtRhNeJb70056798;     dNFmmRZXFEBpdtRhNeJb70056798 = dNFmmRZXFEBpdtRhNeJb80424015;     dNFmmRZXFEBpdtRhNeJb80424015 = dNFmmRZXFEBpdtRhNeJb47800573;     dNFmmRZXFEBpdtRhNeJb47800573 = dNFmmRZXFEBpdtRhNeJb11793468;     dNFmmRZXFEBpdtRhNeJb11793468 = dNFmmRZXFEBpdtRhNeJb98307258;     dNFmmRZXFEBpdtRhNeJb98307258 = dNFmmRZXFEBpdtRhNeJb76896269;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dWjFfYhhWpnjiYDgnTOO29289077() {     float wuKTpEHEEklQDhBDPMeZ76889798 = -313954471;    float wuKTpEHEEklQDhBDPMeZ37111904 = -850713050;    float wuKTpEHEEklQDhBDPMeZ57925396 = -272071918;    float wuKTpEHEEklQDhBDPMeZ43829779 = -467609882;    float wuKTpEHEEklQDhBDPMeZ43301274 = -904029829;    float wuKTpEHEEklQDhBDPMeZ36305787 = -569562949;    float wuKTpEHEEklQDhBDPMeZ53079436 = -124263777;    float wuKTpEHEEklQDhBDPMeZ24405585 = -414794433;    float wuKTpEHEEklQDhBDPMeZ1473811 = -137340968;    float wuKTpEHEEklQDhBDPMeZ21075749 = -113691540;    float wuKTpEHEEklQDhBDPMeZ89606280 = -16680334;    float wuKTpEHEEklQDhBDPMeZ89827224 = -743746518;    float wuKTpEHEEklQDhBDPMeZ33504274 = -659725847;    float wuKTpEHEEklQDhBDPMeZ70247132 = -127856111;    float wuKTpEHEEklQDhBDPMeZ58414779 = -546921766;    float wuKTpEHEEklQDhBDPMeZ13217439 = -244497596;    float wuKTpEHEEklQDhBDPMeZ25907031 = 69392794;    float wuKTpEHEEklQDhBDPMeZ53536343 = -907167978;    float wuKTpEHEEklQDhBDPMeZ45583847 = -289907497;    float wuKTpEHEEklQDhBDPMeZ97070816 = -579929678;    float wuKTpEHEEklQDhBDPMeZ90070953 = -116834281;    float wuKTpEHEEklQDhBDPMeZ35077299 = -551201377;    float wuKTpEHEEklQDhBDPMeZ35687009 = -840976859;    float wuKTpEHEEklQDhBDPMeZ85696699 = -709096160;    float wuKTpEHEEklQDhBDPMeZ52282703 = -502996728;    float wuKTpEHEEklQDhBDPMeZ60027642 = -781200806;    float wuKTpEHEEklQDhBDPMeZ80619624 = 39925466;    float wuKTpEHEEklQDhBDPMeZ77673505 = -9966716;    float wuKTpEHEEklQDhBDPMeZ2565331 = -419248028;    float wuKTpEHEEklQDhBDPMeZ20249952 = -206596657;    float wuKTpEHEEklQDhBDPMeZ71316395 = -460355906;    float wuKTpEHEEklQDhBDPMeZ91932826 = -439152210;    float wuKTpEHEEklQDhBDPMeZ12173565 = -226363476;    float wuKTpEHEEklQDhBDPMeZ29132874 = 40227319;    float wuKTpEHEEklQDhBDPMeZ42806914 = -451171582;    float wuKTpEHEEklQDhBDPMeZ7328780 = -686355235;    float wuKTpEHEEklQDhBDPMeZ32777650 = -770882704;    float wuKTpEHEEklQDhBDPMeZ61457714 = -672317676;    float wuKTpEHEEklQDhBDPMeZ73403474 = -948559390;    float wuKTpEHEEklQDhBDPMeZ24237188 = -147067116;    float wuKTpEHEEklQDhBDPMeZ174586 = -648453655;    float wuKTpEHEEklQDhBDPMeZ31426328 = -293963299;    float wuKTpEHEEklQDhBDPMeZ82657497 = -357267400;    float wuKTpEHEEklQDhBDPMeZ57557885 = -897246202;    float wuKTpEHEEklQDhBDPMeZ44402016 = -872381101;    float wuKTpEHEEklQDhBDPMeZ40021435 = -91310758;    float wuKTpEHEEklQDhBDPMeZ17942941 = -674235671;    float wuKTpEHEEklQDhBDPMeZ93615499 = -3126874;    float wuKTpEHEEklQDhBDPMeZ99485101 = -461649964;    float wuKTpEHEEklQDhBDPMeZ89164350 = 98558876;    float wuKTpEHEEklQDhBDPMeZ3579958 = -696937595;    float wuKTpEHEEklQDhBDPMeZ59745253 = -987385226;    float wuKTpEHEEklQDhBDPMeZ74547165 = -594322474;    float wuKTpEHEEklQDhBDPMeZ75319779 = -234286483;    float wuKTpEHEEklQDhBDPMeZ39218123 = -269151529;    float wuKTpEHEEklQDhBDPMeZ41812499 = -762753095;    float wuKTpEHEEklQDhBDPMeZ1424896 = 90263808;    float wuKTpEHEEklQDhBDPMeZ72228696 = -562975758;    float wuKTpEHEEklQDhBDPMeZ91547076 = -964613155;    float wuKTpEHEEklQDhBDPMeZ83273632 = -22829024;    float wuKTpEHEEklQDhBDPMeZ55686162 = -509488415;    float wuKTpEHEEklQDhBDPMeZ75405930 = -14297062;    float wuKTpEHEEklQDhBDPMeZ21840255 = -995546405;    float wuKTpEHEEklQDhBDPMeZ81223858 = -930744312;    float wuKTpEHEEklQDhBDPMeZ49759354 = -653335635;    float wuKTpEHEEklQDhBDPMeZ97673453 = -577528124;    float wuKTpEHEEklQDhBDPMeZ77653659 = -417383042;    float wuKTpEHEEklQDhBDPMeZ4371401 = -599953166;    float wuKTpEHEEklQDhBDPMeZ27440219 = -676684529;    float wuKTpEHEEklQDhBDPMeZ51086000 = -860566531;    float wuKTpEHEEklQDhBDPMeZ80439789 = -473614893;    float wuKTpEHEEklQDhBDPMeZ64449316 = -258289530;    float wuKTpEHEEklQDhBDPMeZ80132868 = -958608589;    float wuKTpEHEEklQDhBDPMeZ21346660 = -42840382;    float wuKTpEHEEklQDhBDPMeZ96896231 = -931476024;    float wuKTpEHEEklQDhBDPMeZ58644626 = -822870982;    float wuKTpEHEEklQDhBDPMeZ52419802 = -93933977;    float wuKTpEHEEklQDhBDPMeZ78129124 = -943730657;    float wuKTpEHEEklQDhBDPMeZ41294683 = -836715059;    float wuKTpEHEEklQDhBDPMeZ12261268 = -311685970;    float wuKTpEHEEklQDhBDPMeZ42084702 = -6965136;    float wuKTpEHEEklQDhBDPMeZ87004125 = -956947661;    float wuKTpEHEEklQDhBDPMeZ78188404 = -548316752;    float wuKTpEHEEklQDhBDPMeZ13400981 = -417806905;    float wuKTpEHEEklQDhBDPMeZ16669995 = -509659063;    float wuKTpEHEEklQDhBDPMeZ11571143 = -472970680;    float wuKTpEHEEklQDhBDPMeZ17385661 = -844829736;    float wuKTpEHEEklQDhBDPMeZ36853785 = -992076994;    float wuKTpEHEEklQDhBDPMeZ89914750 = -690621153;    float wuKTpEHEEklQDhBDPMeZ994416 = -688418488;    float wuKTpEHEEklQDhBDPMeZ5903885 = -676619043;    float wuKTpEHEEklQDhBDPMeZ60548953 = -107906946;    float wuKTpEHEEklQDhBDPMeZ69910638 = -707704522;    float wuKTpEHEEklQDhBDPMeZ90129842 = -825730366;    float wuKTpEHEEklQDhBDPMeZ68551025 = -637578701;    float wuKTpEHEEklQDhBDPMeZ24768655 = -534156593;    float wuKTpEHEEklQDhBDPMeZ9586073 = -298416894;    float wuKTpEHEEklQDhBDPMeZ1433640 = -426523089;    float wuKTpEHEEklQDhBDPMeZ7798531 = -143910568;    float wuKTpEHEEklQDhBDPMeZ46728563 = -313954471;     wuKTpEHEEklQDhBDPMeZ76889798 = wuKTpEHEEklQDhBDPMeZ37111904;     wuKTpEHEEklQDhBDPMeZ37111904 = wuKTpEHEEklQDhBDPMeZ57925396;     wuKTpEHEEklQDhBDPMeZ57925396 = wuKTpEHEEklQDhBDPMeZ43829779;     wuKTpEHEEklQDhBDPMeZ43829779 = wuKTpEHEEklQDhBDPMeZ43301274;     wuKTpEHEEklQDhBDPMeZ43301274 = wuKTpEHEEklQDhBDPMeZ36305787;     wuKTpEHEEklQDhBDPMeZ36305787 = wuKTpEHEEklQDhBDPMeZ53079436;     wuKTpEHEEklQDhBDPMeZ53079436 = wuKTpEHEEklQDhBDPMeZ24405585;     wuKTpEHEEklQDhBDPMeZ24405585 = wuKTpEHEEklQDhBDPMeZ1473811;     wuKTpEHEEklQDhBDPMeZ1473811 = wuKTpEHEEklQDhBDPMeZ21075749;     wuKTpEHEEklQDhBDPMeZ21075749 = wuKTpEHEEklQDhBDPMeZ89606280;     wuKTpEHEEklQDhBDPMeZ89606280 = wuKTpEHEEklQDhBDPMeZ89827224;     wuKTpEHEEklQDhBDPMeZ89827224 = wuKTpEHEEklQDhBDPMeZ33504274;     wuKTpEHEEklQDhBDPMeZ33504274 = wuKTpEHEEklQDhBDPMeZ70247132;     wuKTpEHEEklQDhBDPMeZ70247132 = wuKTpEHEEklQDhBDPMeZ58414779;     wuKTpEHEEklQDhBDPMeZ58414779 = wuKTpEHEEklQDhBDPMeZ13217439;     wuKTpEHEEklQDhBDPMeZ13217439 = wuKTpEHEEklQDhBDPMeZ25907031;     wuKTpEHEEklQDhBDPMeZ25907031 = wuKTpEHEEklQDhBDPMeZ53536343;     wuKTpEHEEklQDhBDPMeZ53536343 = wuKTpEHEEklQDhBDPMeZ45583847;     wuKTpEHEEklQDhBDPMeZ45583847 = wuKTpEHEEklQDhBDPMeZ97070816;     wuKTpEHEEklQDhBDPMeZ97070816 = wuKTpEHEEklQDhBDPMeZ90070953;     wuKTpEHEEklQDhBDPMeZ90070953 = wuKTpEHEEklQDhBDPMeZ35077299;     wuKTpEHEEklQDhBDPMeZ35077299 = wuKTpEHEEklQDhBDPMeZ35687009;     wuKTpEHEEklQDhBDPMeZ35687009 = wuKTpEHEEklQDhBDPMeZ85696699;     wuKTpEHEEklQDhBDPMeZ85696699 = wuKTpEHEEklQDhBDPMeZ52282703;     wuKTpEHEEklQDhBDPMeZ52282703 = wuKTpEHEEklQDhBDPMeZ60027642;     wuKTpEHEEklQDhBDPMeZ60027642 = wuKTpEHEEklQDhBDPMeZ80619624;     wuKTpEHEEklQDhBDPMeZ80619624 = wuKTpEHEEklQDhBDPMeZ77673505;     wuKTpEHEEklQDhBDPMeZ77673505 = wuKTpEHEEklQDhBDPMeZ2565331;     wuKTpEHEEklQDhBDPMeZ2565331 = wuKTpEHEEklQDhBDPMeZ20249952;     wuKTpEHEEklQDhBDPMeZ20249952 = wuKTpEHEEklQDhBDPMeZ71316395;     wuKTpEHEEklQDhBDPMeZ71316395 = wuKTpEHEEklQDhBDPMeZ91932826;     wuKTpEHEEklQDhBDPMeZ91932826 = wuKTpEHEEklQDhBDPMeZ12173565;     wuKTpEHEEklQDhBDPMeZ12173565 = wuKTpEHEEklQDhBDPMeZ29132874;     wuKTpEHEEklQDhBDPMeZ29132874 = wuKTpEHEEklQDhBDPMeZ42806914;     wuKTpEHEEklQDhBDPMeZ42806914 = wuKTpEHEEklQDhBDPMeZ7328780;     wuKTpEHEEklQDhBDPMeZ7328780 = wuKTpEHEEklQDhBDPMeZ32777650;     wuKTpEHEEklQDhBDPMeZ32777650 = wuKTpEHEEklQDhBDPMeZ61457714;     wuKTpEHEEklQDhBDPMeZ61457714 = wuKTpEHEEklQDhBDPMeZ73403474;     wuKTpEHEEklQDhBDPMeZ73403474 = wuKTpEHEEklQDhBDPMeZ24237188;     wuKTpEHEEklQDhBDPMeZ24237188 = wuKTpEHEEklQDhBDPMeZ174586;     wuKTpEHEEklQDhBDPMeZ174586 = wuKTpEHEEklQDhBDPMeZ31426328;     wuKTpEHEEklQDhBDPMeZ31426328 = wuKTpEHEEklQDhBDPMeZ82657497;     wuKTpEHEEklQDhBDPMeZ82657497 = wuKTpEHEEklQDhBDPMeZ57557885;     wuKTpEHEEklQDhBDPMeZ57557885 = wuKTpEHEEklQDhBDPMeZ44402016;     wuKTpEHEEklQDhBDPMeZ44402016 = wuKTpEHEEklQDhBDPMeZ40021435;     wuKTpEHEEklQDhBDPMeZ40021435 = wuKTpEHEEklQDhBDPMeZ17942941;     wuKTpEHEEklQDhBDPMeZ17942941 = wuKTpEHEEklQDhBDPMeZ93615499;     wuKTpEHEEklQDhBDPMeZ93615499 = wuKTpEHEEklQDhBDPMeZ99485101;     wuKTpEHEEklQDhBDPMeZ99485101 = wuKTpEHEEklQDhBDPMeZ89164350;     wuKTpEHEEklQDhBDPMeZ89164350 = wuKTpEHEEklQDhBDPMeZ3579958;     wuKTpEHEEklQDhBDPMeZ3579958 = wuKTpEHEEklQDhBDPMeZ59745253;     wuKTpEHEEklQDhBDPMeZ59745253 = wuKTpEHEEklQDhBDPMeZ74547165;     wuKTpEHEEklQDhBDPMeZ74547165 = wuKTpEHEEklQDhBDPMeZ75319779;     wuKTpEHEEklQDhBDPMeZ75319779 = wuKTpEHEEklQDhBDPMeZ39218123;     wuKTpEHEEklQDhBDPMeZ39218123 = wuKTpEHEEklQDhBDPMeZ41812499;     wuKTpEHEEklQDhBDPMeZ41812499 = wuKTpEHEEklQDhBDPMeZ1424896;     wuKTpEHEEklQDhBDPMeZ1424896 = wuKTpEHEEklQDhBDPMeZ72228696;     wuKTpEHEEklQDhBDPMeZ72228696 = wuKTpEHEEklQDhBDPMeZ91547076;     wuKTpEHEEklQDhBDPMeZ91547076 = wuKTpEHEEklQDhBDPMeZ83273632;     wuKTpEHEEklQDhBDPMeZ83273632 = wuKTpEHEEklQDhBDPMeZ55686162;     wuKTpEHEEklQDhBDPMeZ55686162 = wuKTpEHEEklQDhBDPMeZ75405930;     wuKTpEHEEklQDhBDPMeZ75405930 = wuKTpEHEEklQDhBDPMeZ21840255;     wuKTpEHEEklQDhBDPMeZ21840255 = wuKTpEHEEklQDhBDPMeZ81223858;     wuKTpEHEEklQDhBDPMeZ81223858 = wuKTpEHEEklQDhBDPMeZ49759354;     wuKTpEHEEklQDhBDPMeZ49759354 = wuKTpEHEEklQDhBDPMeZ97673453;     wuKTpEHEEklQDhBDPMeZ97673453 = wuKTpEHEEklQDhBDPMeZ77653659;     wuKTpEHEEklQDhBDPMeZ77653659 = wuKTpEHEEklQDhBDPMeZ4371401;     wuKTpEHEEklQDhBDPMeZ4371401 = wuKTpEHEEklQDhBDPMeZ27440219;     wuKTpEHEEklQDhBDPMeZ27440219 = wuKTpEHEEklQDhBDPMeZ51086000;     wuKTpEHEEklQDhBDPMeZ51086000 = wuKTpEHEEklQDhBDPMeZ80439789;     wuKTpEHEEklQDhBDPMeZ80439789 = wuKTpEHEEklQDhBDPMeZ64449316;     wuKTpEHEEklQDhBDPMeZ64449316 = wuKTpEHEEklQDhBDPMeZ80132868;     wuKTpEHEEklQDhBDPMeZ80132868 = wuKTpEHEEklQDhBDPMeZ21346660;     wuKTpEHEEklQDhBDPMeZ21346660 = wuKTpEHEEklQDhBDPMeZ96896231;     wuKTpEHEEklQDhBDPMeZ96896231 = wuKTpEHEEklQDhBDPMeZ58644626;     wuKTpEHEEklQDhBDPMeZ58644626 = wuKTpEHEEklQDhBDPMeZ52419802;     wuKTpEHEEklQDhBDPMeZ52419802 = wuKTpEHEEklQDhBDPMeZ78129124;     wuKTpEHEEklQDhBDPMeZ78129124 = wuKTpEHEEklQDhBDPMeZ41294683;     wuKTpEHEEklQDhBDPMeZ41294683 = wuKTpEHEEklQDhBDPMeZ12261268;     wuKTpEHEEklQDhBDPMeZ12261268 = wuKTpEHEEklQDhBDPMeZ42084702;     wuKTpEHEEklQDhBDPMeZ42084702 = wuKTpEHEEklQDhBDPMeZ87004125;     wuKTpEHEEklQDhBDPMeZ87004125 = wuKTpEHEEklQDhBDPMeZ78188404;     wuKTpEHEEklQDhBDPMeZ78188404 = wuKTpEHEEklQDhBDPMeZ13400981;     wuKTpEHEEklQDhBDPMeZ13400981 = wuKTpEHEEklQDhBDPMeZ16669995;     wuKTpEHEEklQDhBDPMeZ16669995 = wuKTpEHEEklQDhBDPMeZ11571143;     wuKTpEHEEklQDhBDPMeZ11571143 = wuKTpEHEEklQDhBDPMeZ17385661;     wuKTpEHEEklQDhBDPMeZ17385661 = wuKTpEHEEklQDhBDPMeZ36853785;     wuKTpEHEEklQDhBDPMeZ36853785 = wuKTpEHEEklQDhBDPMeZ89914750;     wuKTpEHEEklQDhBDPMeZ89914750 = wuKTpEHEEklQDhBDPMeZ994416;     wuKTpEHEEklQDhBDPMeZ994416 = wuKTpEHEEklQDhBDPMeZ5903885;     wuKTpEHEEklQDhBDPMeZ5903885 = wuKTpEHEEklQDhBDPMeZ60548953;     wuKTpEHEEklQDhBDPMeZ60548953 = wuKTpEHEEklQDhBDPMeZ69910638;     wuKTpEHEEklQDhBDPMeZ69910638 = wuKTpEHEEklQDhBDPMeZ90129842;     wuKTpEHEEklQDhBDPMeZ90129842 = wuKTpEHEEklQDhBDPMeZ68551025;     wuKTpEHEEklQDhBDPMeZ68551025 = wuKTpEHEEklQDhBDPMeZ24768655;     wuKTpEHEEklQDhBDPMeZ24768655 = wuKTpEHEEklQDhBDPMeZ9586073;     wuKTpEHEEklQDhBDPMeZ9586073 = wuKTpEHEEklQDhBDPMeZ1433640;     wuKTpEHEEklQDhBDPMeZ1433640 = wuKTpEHEEklQDhBDPMeZ7798531;     wuKTpEHEEklQDhBDPMeZ7798531 = wuKTpEHEEklQDhBDPMeZ46728563;     wuKTpEHEEklQDhBDPMeZ46728563 = wuKTpEHEEklQDhBDPMeZ76889798;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void GYPLlITWuQELdDzVLgXn83705290() {     float tzCeOkNWEYVMbHdjpAsW47541465 = -932103602;    float tzCeOkNWEYVMbHdjpAsW5785422 = -722356350;    float tzCeOkNWEYVMbHdjpAsW37894315 = -714366378;    float tzCeOkNWEYVMbHdjpAsW83761065 = -657748271;    float tzCeOkNWEYVMbHdjpAsW64751596 = -687436177;    float tzCeOkNWEYVMbHdjpAsW26305549 = -271144100;    float tzCeOkNWEYVMbHdjpAsW71458332 = -123039487;    float tzCeOkNWEYVMbHdjpAsW81247965 = -761611523;    float tzCeOkNWEYVMbHdjpAsW47338864 = -230688297;    float tzCeOkNWEYVMbHdjpAsW44042210 = -414786891;    float tzCeOkNWEYVMbHdjpAsW31855441 = -51582263;    float tzCeOkNWEYVMbHdjpAsW35373795 = 40489689;    float tzCeOkNWEYVMbHdjpAsW13331661 = -569262905;    float tzCeOkNWEYVMbHdjpAsW54094166 = 71115679;    float tzCeOkNWEYVMbHdjpAsW41134798 = -696671769;    float tzCeOkNWEYVMbHdjpAsW45746079 = -914481183;    float tzCeOkNWEYVMbHdjpAsW19759858 = -939792336;    float tzCeOkNWEYVMbHdjpAsW73829837 = -896334174;    float tzCeOkNWEYVMbHdjpAsW27291629 = -997095439;    float tzCeOkNWEYVMbHdjpAsW55857526 = -22295983;    float tzCeOkNWEYVMbHdjpAsW45206708 = -25513422;    float tzCeOkNWEYVMbHdjpAsW96473063 = -882486682;    float tzCeOkNWEYVMbHdjpAsW37630817 = -308820003;    float tzCeOkNWEYVMbHdjpAsW19546236 = -301497066;    float tzCeOkNWEYVMbHdjpAsW16407092 = -178901416;    float tzCeOkNWEYVMbHdjpAsW90625223 = -879485251;    float tzCeOkNWEYVMbHdjpAsW88880410 = -355252310;    float tzCeOkNWEYVMbHdjpAsW85455730 = -515090483;    float tzCeOkNWEYVMbHdjpAsW34707818 = -713606245;    float tzCeOkNWEYVMbHdjpAsW59422048 = -764991728;    float tzCeOkNWEYVMbHdjpAsW78388392 = -208789591;    float tzCeOkNWEYVMbHdjpAsW36444287 = -767449373;    float tzCeOkNWEYVMbHdjpAsW95118925 = -173325085;    float tzCeOkNWEYVMbHdjpAsW94497848 = -623302366;    float tzCeOkNWEYVMbHdjpAsW20831151 = -298118362;    float tzCeOkNWEYVMbHdjpAsW67322038 = -765581783;    float tzCeOkNWEYVMbHdjpAsW76569135 = -959475911;    float tzCeOkNWEYVMbHdjpAsW14250203 = -456430783;    float tzCeOkNWEYVMbHdjpAsW51936763 = -904288026;    float tzCeOkNWEYVMbHdjpAsW19340486 = -764675478;    float tzCeOkNWEYVMbHdjpAsW60247869 = 67527467;    float tzCeOkNWEYVMbHdjpAsW59909318 = -63352369;    float tzCeOkNWEYVMbHdjpAsW31624457 = -344021452;    float tzCeOkNWEYVMbHdjpAsW64066185 = -969221173;    float tzCeOkNWEYVMbHdjpAsW40356075 = -81027496;    float tzCeOkNWEYVMbHdjpAsW46427270 = -949647913;    float tzCeOkNWEYVMbHdjpAsW44074144 = -790305470;    float tzCeOkNWEYVMbHdjpAsW59229709 = -455257169;    float tzCeOkNWEYVMbHdjpAsW4926554 = -447413933;    float tzCeOkNWEYVMbHdjpAsW97591836 = -184450002;    float tzCeOkNWEYVMbHdjpAsW71769282 = -243509716;    float tzCeOkNWEYVMbHdjpAsW1616758 = -956604948;    float tzCeOkNWEYVMbHdjpAsW29622823 = -522120516;    float tzCeOkNWEYVMbHdjpAsW65547084 = -895167980;    float tzCeOkNWEYVMbHdjpAsW38498582 = -380092443;    float tzCeOkNWEYVMbHdjpAsW51068401 = 50383080;    float tzCeOkNWEYVMbHdjpAsW68154604 = -313536347;    float tzCeOkNWEYVMbHdjpAsW18348079 = -312869312;    float tzCeOkNWEYVMbHdjpAsW67353974 = -378846856;    float tzCeOkNWEYVMbHdjpAsW74126372 = -807950927;    float tzCeOkNWEYVMbHdjpAsW37425139 = -915891791;    float tzCeOkNWEYVMbHdjpAsW86002601 = -607949004;    float tzCeOkNWEYVMbHdjpAsW46540148 = 51994722;    float tzCeOkNWEYVMbHdjpAsW87916816 = -465696570;    float tzCeOkNWEYVMbHdjpAsW65653818 = -105997300;    float tzCeOkNWEYVMbHdjpAsW95411153 = -284132891;    float tzCeOkNWEYVMbHdjpAsW40254870 = -786185226;    float tzCeOkNWEYVMbHdjpAsW18833813 = -945960540;    float tzCeOkNWEYVMbHdjpAsW33263016 = -630765960;    float tzCeOkNWEYVMbHdjpAsW73812760 = -931089987;    float tzCeOkNWEYVMbHdjpAsW69176943 = -955005272;    float tzCeOkNWEYVMbHdjpAsW5509655 = -383361553;    float tzCeOkNWEYVMbHdjpAsW21893074 = -992046149;    float tzCeOkNWEYVMbHdjpAsW7951143 = -132419961;    float tzCeOkNWEYVMbHdjpAsW95609656 = 10176550;    float tzCeOkNWEYVMbHdjpAsW85297389 = -962161053;    float tzCeOkNWEYVMbHdjpAsW64848607 = -438465230;    float tzCeOkNWEYVMbHdjpAsW73564632 = -339598830;    float tzCeOkNWEYVMbHdjpAsW79190161 = -120469571;    float tzCeOkNWEYVMbHdjpAsW69979821 = -229253503;    float tzCeOkNWEYVMbHdjpAsW46551079 = 10820219;    float tzCeOkNWEYVMbHdjpAsW29650701 = -899995141;    float tzCeOkNWEYVMbHdjpAsW80529177 = 32323449;    float tzCeOkNWEYVMbHdjpAsW37115982 = -429156243;    float tzCeOkNWEYVMbHdjpAsW87652765 = -421482012;    float tzCeOkNWEYVMbHdjpAsW76771634 = -252184644;    float tzCeOkNWEYVMbHdjpAsW6821465 = -145328857;    float tzCeOkNWEYVMbHdjpAsW29571841 = -278157106;    float tzCeOkNWEYVMbHdjpAsW55999267 = -143209923;    float tzCeOkNWEYVMbHdjpAsW69762749 = -248501442;    float tzCeOkNWEYVMbHdjpAsW99167434 = -352045437;    float tzCeOkNWEYVMbHdjpAsW58221057 = -546606600;    float tzCeOkNWEYVMbHdjpAsW46896228 = 22416072;    float tzCeOkNWEYVMbHdjpAsW77810391 = 3662900;    float tzCeOkNWEYVMbHdjpAsW81915347 = -848783688;    float tzCeOkNWEYVMbHdjpAsW74245268 = -324523529;    float tzCeOkNWEYVMbHdjpAsW13369171 = -15347091;    float tzCeOkNWEYVMbHdjpAsW43707641 = -878324883;    float tzCeOkNWEYVMbHdjpAsW98412367 = -763223874;    float tzCeOkNWEYVMbHdjpAsW44944922 = -932103602;     tzCeOkNWEYVMbHdjpAsW47541465 = tzCeOkNWEYVMbHdjpAsW5785422;     tzCeOkNWEYVMbHdjpAsW5785422 = tzCeOkNWEYVMbHdjpAsW37894315;     tzCeOkNWEYVMbHdjpAsW37894315 = tzCeOkNWEYVMbHdjpAsW83761065;     tzCeOkNWEYVMbHdjpAsW83761065 = tzCeOkNWEYVMbHdjpAsW64751596;     tzCeOkNWEYVMbHdjpAsW64751596 = tzCeOkNWEYVMbHdjpAsW26305549;     tzCeOkNWEYVMbHdjpAsW26305549 = tzCeOkNWEYVMbHdjpAsW71458332;     tzCeOkNWEYVMbHdjpAsW71458332 = tzCeOkNWEYVMbHdjpAsW81247965;     tzCeOkNWEYVMbHdjpAsW81247965 = tzCeOkNWEYVMbHdjpAsW47338864;     tzCeOkNWEYVMbHdjpAsW47338864 = tzCeOkNWEYVMbHdjpAsW44042210;     tzCeOkNWEYVMbHdjpAsW44042210 = tzCeOkNWEYVMbHdjpAsW31855441;     tzCeOkNWEYVMbHdjpAsW31855441 = tzCeOkNWEYVMbHdjpAsW35373795;     tzCeOkNWEYVMbHdjpAsW35373795 = tzCeOkNWEYVMbHdjpAsW13331661;     tzCeOkNWEYVMbHdjpAsW13331661 = tzCeOkNWEYVMbHdjpAsW54094166;     tzCeOkNWEYVMbHdjpAsW54094166 = tzCeOkNWEYVMbHdjpAsW41134798;     tzCeOkNWEYVMbHdjpAsW41134798 = tzCeOkNWEYVMbHdjpAsW45746079;     tzCeOkNWEYVMbHdjpAsW45746079 = tzCeOkNWEYVMbHdjpAsW19759858;     tzCeOkNWEYVMbHdjpAsW19759858 = tzCeOkNWEYVMbHdjpAsW73829837;     tzCeOkNWEYVMbHdjpAsW73829837 = tzCeOkNWEYVMbHdjpAsW27291629;     tzCeOkNWEYVMbHdjpAsW27291629 = tzCeOkNWEYVMbHdjpAsW55857526;     tzCeOkNWEYVMbHdjpAsW55857526 = tzCeOkNWEYVMbHdjpAsW45206708;     tzCeOkNWEYVMbHdjpAsW45206708 = tzCeOkNWEYVMbHdjpAsW96473063;     tzCeOkNWEYVMbHdjpAsW96473063 = tzCeOkNWEYVMbHdjpAsW37630817;     tzCeOkNWEYVMbHdjpAsW37630817 = tzCeOkNWEYVMbHdjpAsW19546236;     tzCeOkNWEYVMbHdjpAsW19546236 = tzCeOkNWEYVMbHdjpAsW16407092;     tzCeOkNWEYVMbHdjpAsW16407092 = tzCeOkNWEYVMbHdjpAsW90625223;     tzCeOkNWEYVMbHdjpAsW90625223 = tzCeOkNWEYVMbHdjpAsW88880410;     tzCeOkNWEYVMbHdjpAsW88880410 = tzCeOkNWEYVMbHdjpAsW85455730;     tzCeOkNWEYVMbHdjpAsW85455730 = tzCeOkNWEYVMbHdjpAsW34707818;     tzCeOkNWEYVMbHdjpAsW34707818 = tzCeOkNWEYVMbHdjpAsW59422048;     tzCeOkNWEYVMbHdjpAsW59422048 = tzCeOkNWEYVMbHdjpAsW78388392;     tzCeOkNWEYVMbHdjpAsW78388392 = tzCeOkNWEYVMbHdjpAsW36444287;     tzCeOkNWEYVMbHdjpAsW36444287 = tzCeOkNWEYVMbHdjpAsW95118925;     tzCeOkNWEYVMbHdjpAsW95118925 = tzCeOkNWEYVMbHdjpAsW94497848;     tzCeOkNWEYVMbHdjpAsW94497848 = tzCeOkNWEYVMbHdjpAsW20831151;     tzCeOkNWEYVMbHdjpAsW20831151 = tzCeOkNWEYVMbHdjpAsW67322038;     tzCeOkNWEYVMbHdjpAsW67322038 = tzCeOkNWEYVMbHdjpAsW76569135;     tzCeOkNWEYVMbHdjpAsW76569135 = tzCeOkNWEYVMbHdjpAsW14250203;     tzCeOkNWEYVMbHdjpAsW14250203 = tzCeOkNWEYVMbHdjpAsW51936763;     tzCeOkNWEYVMbHdjpAsW51936763 = tzCeOkNWEYVMbHdjpAsW19340486;     tzCeOkNWEYVMbHdjpAsW19340486 = tzCeOkNWEYVMbHdjpAsW60247869;     tzCeOkNWEYVMbHdjpAsW60247869 = tzCeOkNWEYVMbHdjpAsW59909318;     tzCeOkNWEYVMbHdjpAsW59909318 = tzCeOkNWEYVMbHdjpAsW31624457;     tzCeOkNWEYVMbHdjpAsW31624457 = tzCeOkNWEYVMbHdjpAsW64066185;     tzCeOkNWEYVMbHdjpAsW64066185 = tzCeOkNWEYVMbHdjpAsW40356075;     tzCeOkNWEYVMbHdjpAsW40356075 = tzCeOkNWEYVMbHdjpAsW46427270;     tzCeOkNWEYVMbHdjpAsW46427270 = tzCeOkNWEYVMbHdjpAsW44074144;     tzCeOkNWEYVMbHdjpAsW44074144 = tzCeOkNWEYVMbHdjpAsW59229709;     tzCeOkNWEYVMbHdjpAsW59229709 = tzCeOkNWEYVMbHdjpAsW4926554;     tzCeOkNWEYVMbHdjpAsW4926554 = tzCeOkNWEYVMbHdjpAsW97591836;     tzCeOkNWEYVMbHdjpAsW97591836 = tzCeOkNWEYVMbHdjpAsW71769282;     tzCeOkNWEYVMbHdjpAsW71769282 = tzCeOkNWEYVMbHdjpAsW1616758;     tzCeOkNWEYVMbHdjpAsW1616758 = tzCeOkNWEYVMbHdjpAsW29622823;     tzCeOkNWEYVMbHdjpAsW29622823 = tzCeOkNWEYVMbHdjpAsW65547084;     tzCeOkNWEYVMbHdjpAsW65547084 = tzCeOkNWEYVMbHdjpAsW38498582;     tzCeOkNWEYVMbHdjpAsW38498582 = tzCeOkNWEYVMbHdjpAsW51068401;     tzCeOkNWEYVMbHdjpAsW51068401 = tzCeOkNWEYVMbHdjpAsW68154604;     tzCeOkNWEYVMbHdjpAsW68154604 = tzCeOkNWEYVMbHdjpAsW18348079;     tzCeOkNWEYVMbHdjpAsW18348079 = tzCeOkNWEYVMbHdjpAsW67353974;     tzCeOkNWEYVMbHdjpAsW67353974 = tzCeOkNWEYVMbHdjpAsW74126372;     tzCeOkNWEYVMbHdjpAsW74126372 = tzCeOkNWEYVMbHdjpAsW37425139;     tzCeOkNWEYVMbHdjpAsW37425139 = tzCeOkNWEYVMbHdjpAsW86002601;     tzCeOkNWEYVMbHdjpAsW86002601 = tzCeOkNWEYVMbHdjpAsW46540148;     tzCeOkNWEYVMbHdjpAsW46540148 = tzCeOkNWEYVMbHdjpAsW87916816;     tzCeOkNWEYVMbHdjpAsW87916816 = tzCeOkNWEYVMbHdjpAsW65653818;     tzCeOkNWEYVMbHdjpAsW65653818 = tzCeOkNWEYVMbHdjpAsW95411153;     tzCeOkNWEYVMbHdjpAsW95411153 = tzCeOkNWEYVMbHdjpAsW40254870;     tzCeOkNWEYVMbHdjpAsW40254870 = tzCeOkNWEYVMbHdjpAsW18833813;     tzCeOkNWEYVMbHdjpAsW18833813 = tzCeOkNWEYVMbHdjpAsW33263016;     tzCeOkNWEYVMbHdjpAsW33263016 = tzCeOkNWEYVMbHdjpAsW73812760;     tzCeOkNWEYVMbHdjpAsW73812760 = tzCeOkNWEYVMbHdjpAsW69176943;     tzCeOkNWEYVMbHdjpAsW69176943 = tzCeOkNWEYVMbHdjpAsW5509655;     tzCeOkNWEYVMbHdjpAsW5509655 = tzCeOkNWEYVMbHdjpAsW21893074;     tzCeOkNWEYVMbHdjpAsW21893074 = tzCeOkNWEYVMbHdjpAsW7951143;     tzCeOkNWEYVMbHdjpAsW7951143 = tzCeOkNWEYVMbHdjpAsW95609656;     tzCeOkNWEYVMbHdjpAsW95609656 = tzCeOkNWEYVMbHdjpAsW85297389;     tzCeOkNWEYVMbHdjpAsW85297389 = tzCeOkNWEYVMbHdjpAsW64848607;     tzCeOkNWEYVMbHdjpAsW64848607 = tzCeOkNWEYVMbHdjpAsW73564632;     tzCeOkNWEYVMbHdjpAsW73564632 = tzCeOkNWEYVMbHdjpAsW79190161;     tzCeOkNWEYVMbHdjpAsW79190161 = tzCeOkNWEYVMbHdjpAsW69979821;     tzCeOkNWEYVMbHdjpAsW69979821 = tzCeOkNWEYVMbHdjpAsW46551079;     tzCeOkNWEYVMbHdjpAsW46551079 = tzCeOkNWEYVMbHdjpAsW29650701;     tzCeOkNWEYVMbHdjpAsW29650701 = tzCeOkNWEYVMbHdjpAsW80529177;     tzCeOkNWEYVMbHdjpAsW80529177 = tzCeOkNWEYVMbHdjpAsW37115982;     tzCeOkNWEYVMbHdjpAsW37115982 = tzCeOkNWEYVMbHdjpAsW87652765;     tzCeOkNWEYVMbHdjpAsW87652765 = tzCeOkNWEYVMbHdjpAsW76771634;     tzCeOkNWEYVMbHdjpAsW76771634 = tzCeOkNWEYVMbHdjpAsW6821465;     tzCeOkNWEYVMbHdjpAsW6821465 = tzCeOkNWEYVMbHdjpAsW29571841;     tzCeOkNWEYVMbHdjpAsW29571841 = tzCeOkNWEYVMbHdjpAsW55999267;     tzCeOkNWEYVMbHdjpAsW55999267 = tzCeOkNWEYVMbHdjpAsW69762749;     tzCeOkNWEYVMbHdjpAsW69762749 = tzCeOkNWEYVMbHdjpAsW99167434;     tzCeOkNWEYVMbHdjpAsW99167434 = tzCeOkNWEYVMbHdjpAsW58221057;     tzCeOkNWEYVMbHdjpAsW58221057 = tzCeOkNWEYVMbHdjpAsW46896228;     tzCeOkNWEYVMbHdjpAsW46896228 = tzCeOkNWEYVMbHdjpAsW77810391;     tzCeOkNWEYVMbHdjpAsW77810391 = tzCeOkNWEYVMbHdjpAsW81915347;     tzCeOkNWEYVMbHdjpAsW81915347 = tzCeOkNWEYVMbHdjpAsW74245268;     tzCeOkNWEYVMbHdjpAsW74245268 = tzCeOkNWEYVMbHdjpAsW13369171;     tzCeOkNWEYVMbHdjpAsW13369171 = tzCeOkNWEYVMbHdjpAsW43707641;     tzCeOkNWEYVMbHdjpAsW43707641 = tzCeOkNWEYVMbHdjpAsW98412367;     tzCeOkNWEYVMbHdjpAsW98412367 = tzCeOkNWEYVMbHdjpAsW44944922;     tzCeOkNWEYVMbHdjpAsW44944922 = tzCeOkNWEYVMbHdjpAsW47541465;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void YQNqBXumEQsGLueVqYNS20680462() {     float EPqibXBSKDWhdOPsrxDr15267714 = -1504122;    float EPqibXBSKDWhdOPsrxDr81707618 = -133929715;    float EPqibXBSKDWhdOPsrxDr7244160 = -525764222;    float EPqibXBSKDWhdOPsrxDr12412674 = -478594222;    float EPqibXBSKDWhdOPsrxDr97314281 = -974413260;    float EPqibXBSKDWhdOPsrxDr61819701 = -802034454;    float EPqibXBSKDWhdOPsrxDr11060703 = -447670588;    float EPqibXBSKDWhdOPsrxDr96920771 = -435817251;    float EPqibXBSKDWhdOPsrxDr39467749 = -881677035;    float EPqibXBSKDWhdOPsrxDr49813323 = -615533825;    float EPqibXBSKDWhdOPsrxDr11106611 = -482322164;    float EPqibXBSKDWhdOPsrxDr42803708 = -859309878;    float EPqibXBSKDWhdOPsrxDr48375441 = -306098395;    float EPqibXBSKDWhdOPsrxDr57586708 = -89642552;    float EPqibXBSKDWhdOPsrxDr92818111 = -379740703;    float EPqibXBSKDWhdOPsrxDr74799003 = -898784812;    float EPqibXBSKDWhdOPsrxDr25604205 = -563086897;    float EPqibXBSKDWhdOPsrxDr35210200 = -581584020;    float EPqibXBSKDWhdOPsrxDr87780928 = -364368283;    float EPqibXBSKDWhdOPsrxDr88813016 = -541588741;    float EPqibXBSKDWhdOPsrxDr91584851 = -259383953;    float EPqibXBSKDWhdOPsrxDr87331959 = -630800771;    float EPqibXBSKDWhdOPsrxDr41332706 = -357115016;    float EPqibXBSKDWhdOPsrxDr88267762 = 52630370;    float EPqibXBSKDWhdOPsrxDr84637411 = -954240941;    float EPqibXBSKDWhdOPsrxDr54261347 = -127876618;    float EPqibXBSKDWhdOPsrxDr4201488 = -958794236;    float EPqibXBSKDWhdOPsrxDr73521705 = -904460065;    float EPqibXBSKDWhdOPsrxDr14381149 = -223688803;    float EPqibXBSKDWhdOPsrxDr50233688 = -69961000;    float EPqibXBSKDWhdOPsrxDr4386347 = -128326452;    float EPqibXBSKDWhdOPsrxDr47717725 = -299845961;    float EPqibXBSKDWhdOPsrxDr93954142 = -348095182;    float EPqibXBSKDWhdOPsrxDr59513480 = -43990139;    float EPqibXBSKDWhdOPsrxDr44914396 = -403325449;    float EPqibXBSKDWhdOPsrxDr64516555 = -912747309;    float EPqibXBSKDWhdOPsrxDr7245728 = -941650620;    float EPqibXBSKDWhdOPsrxDr24816744 = -445389914;    float EPqibXBSKDWhdOPsrxDr84788842 = -536066873;    float EPqibXBSKDWhdOPsrxDr87412698 = -852022595;    float EPqibXBSKDWhdOPsrxDr20901507 = -411043939;    float EPqibXBSKDWhdOPsrxDr97440712 = -507521055;    float EPqibXBSKDWhdOPsrxDr6457827 = -950176973;    float EPqibXBSKDWhdOPsrxDr13747804 = 50130400;    float EPqibXBSKDWhdOPsrxDr32373331 = -230912777;    float EPqibXBSKDWhdOPsrxDr39621918 = -875132065;    float EPqibXBSKDWhdOPsrxDr67595622 = -722938961;    float EPqibXBSKDWhdOPsrxDr55784655 = -842901144;    float EPqibXBSKDWhdOPsrxDr75702081 = -61164304;    float EPqibXBSKDWhdOPsrxDr69879342 = -773475853;    float EPqibXBSKDWhdOPsrxDr18784329 = -519925498;    float EPqibXBSKDWhdOPsrxDr633123 = -154503995;    float EPqibXBSKDWhdOPsrxDr51657571 = -346170297;    float EPqibXBSKDWhdOPsrxDr78668143 = -706882155;    float EPqibXBSKDWhdOPsrxDr29095610 = -981787155;    float EPqibXBSKDWhdOPsrxDr27935755 = -370703352;    float EPqibXBSKDWhdOPsrxDr40374913 = -776814700;    float EPqibXBSKDWhdOPsrxDr18976397 = -478394593;    float EPqibXBSKDWhdOPsrxDr27775262 = -524353281;    float EPqibXBSKDWhdOPsrxDr43052934 = -746536643;    float EPqibXBSKDWhdOPsrxDr57618213 = -843240218;    float EPqibXBSKDWhdOPsrxDr37538997 = -543210524;    float EPqibXBSKDWhdOPsrxDr82539622 = -112128448;    float EPqibXBSKDWhdOPsrxDr89234061 = -711716036;    float EPqibXBSKDWhdOPsrxDr45426976 = -387207374;    float EPqibXBSKDWhdOPsrxDr63388885 = -82476204;    float EPqibXBSKDWhdOPsrxDr48849566 = -411214697;    float EPqibXBSKDWhdOPsrxDr88861960 = -162108257;    float EPqibXBSKDWhdOPsrxDr12672312 = -686317103;    float EPqibXBSKDWhdOPsrxDr28301556 = -466993394;    float EPqibXBSKDWhdOPsrxDr67553275 = -957134192;    float EPqibXBSKDWhdOPsrxDr787461 = -17696984;    float EPqibXBSKDWhdOPsrxDr50421357 = 54482853;    float EPqibXBSKDWhdOPsrxDr368231 = -512345689;    float EPqibXBSKDWhdOPsrxDr67911510 = -30544803;    float EPqibXBSKDWhdOPsrxDr94144139 = -751862898;    float EPqibXBSKDWhdOPsrxDr80874132 = -680623799;    float EPqibXBSKDWhdOPsrxDr27584902 = -307245417;    float EPqibXBSKDWhdOPsrxDr55894432 = -716456853;    float EPqibXBSKDWhdOPsrxDr45015494 = 20891123;    float EPqibXBSKDWhdOPsrxDr86665725 = -404937657;    float EPqibXBSKDWhdOPsrxDr48416833 = -15893093;    float EPqibXBSKDWhdOPsrxDr97819624 = -743295761;    float EPqibXBSKDWhdOPsrxDr44501806 = -450212951;    float EPqibXBSKDWhdOPsrxDr31449360 = -550035502;    float EPqibXBSKDWhdOPsrxDr3753225 = -973822458;    float EPqibXBSKDWhdOPsrxDr96060154 = -953675664;    float EPqibXBSKDWhdOPsrxDr15286000 = -641213027;    float EPqibXBSKDWhdOPsrxDr30417871 = -62202984;    float EPqibXBSKDWhdOPsrxDr16978642 = 67377902;    float EPqibXBSKDWhdOPsrxDr24141642 = -35932610;    float EPqibXBSKDWhdOPsrxDr88269331 = -363256028;    float EPqibXBSKDWhdOPsrxDr97041482 = -921036633;    float EPqibXBSKDWhdOPsrxDr41735909 = -789530230;    float EPqibXBSKDWhdOPsrxDr29794485 = 91217623;    float EPqibXBSKDWhdOPsrxDr83362509 = -867833416;    float EPqibXBSKDWhdOPsrxDr14901090 = -295392607;    float EPqibXBSKDWhdOPsrxDr17223766 = -138460937;    float EPqibXBSKDWhdOPsrxDr68320828 = -562662227;    float EPqibXBSKDWhdOPsrxDr68984445 = -1504122;     EPqibXBSKDWhdOPsrxDr15267714 = EPqibXBSKDWhdOPsrxDr81707618;     EPqibXBSKDWhdOPsrxDr81707618 = EPqibXBSKDWhdOPsrxDr7244160;     EPqibXBSKDWhdOPsrxDr7244160 = EPqibXBSKDWhdOPsrxDr12412674;     EPqibXBSKDWhdOPsrxDr12412674 = EPqibXBSKDWhdOPsrxDr97314281;     EPqibXBSKDWhdOPsrxDr97314281 = EPqibXBSKDWhdOPsrxDr61819701;     EPqibXBSKDWhdOPsrxDr61819701 = EPqibXBSKDWhdOPsrxDr11060703;     EPqibXBSKDWhdOPsrxDr11060703 = EPqibXBSKDWhdOPsrxDr96920771;     EPqibXBSKDWhdOPsrxDr96920771 = EPqibXBSKDWhdOPsrxDr39467749;     EPqibXBSKDWhdOPsrxDr39467749 = EPqibXBSKDWhdOPsrxDr49813323;     EPqibXBSKDWhdOPsrxDr49813323 = EPqibXBSKDWhdOPsrxDr11106611;     EPqibXBSKDWhdOPsrxDr11106611 = EPqibXBSKDWhdOPsrxDr42803708;     EPqibXBSKDWhdOPsrxDr42803708 = EPqibXBSKDWhdOPsrxDr48375441;     EPqibXBSKDWhdOPsrxDr48375441 = EPqibXBSKDWhdOPsrxDr57586708;     EPqibXBSKDWhdOPsrxDr57586708 = EPqibXBSKDWhdOPsrxDr92818111;     EPqibXBSKDWhdOPsrxDr92818111 = EPqibXBSKDWhdOPsrxDr74799003;     EPqibXBSKDWhdOPsrxDr74799003 = EPqibXBSKDWhdOPsrxDr25604205;     EPqibXBSKDWhdOPsrxDr25604205 = EPqibXBSKDWhdOPsrxDr35210200;     EPqibXBSKDWhdOPsrxDr35210200 = EPqibXBSKDWhdOPsrxDr87780928;     EPqibXBSKDWhdOPsrxDr87780928 = EPqibXBSKDWhdOPsrxDr88813016;     EPqibXBSKDWhdOPsrxDr88813016 = EPqibXBSKDWhdOPsrxDr91584851;     EPqibXBSKDWhdOPsrxDr91584851 = EPqibXBSKDWhdOPsrxDr87331959;     EPqibXBSKDWhdOPsrxDr87331959 = EPqibXBSKDWhdOPsrxDr41332706;     EPqibXBSKDWhdOPsrxDr41332706 = EPqibXBSKDWhdOPsrxDr88267762;     EPqibXBSKDWhdOPsrxDr88267762 = EPqibXBSKDWhdOPsrxDr84637411;     EPqibXBSKDWhdOPsrxDr84637411 = EPqibXBSKDWhdOPsrxDr54261347;     EPqibXBSKDWhdOPsrxDr54261347 = EPqibXBSKDWhdOPsrxDr4201488;     EPqibXBSKDWhdOPsrxDr4201488 = EPqibXBSKDWhdOPsrxDr73521705;     EPqibXBSKDWhdOPsrxDr73521705 = EPqibXBSKDWhdOPsrxDr14381149;     EPqibXBSKDWhdOPsrxDr14381149 = EPqibXBSKDWhdOPsrxDr50233688;     EPqibXBSKDWhdOPsrxDr50233688 = EPqibXBSKDWhdOPsrxDr4386347;     EPqibXBSKDWhdOPsrxDr4386347 = EPqibXBSKDWhdOPsrxDr47717725;     EPqibXBSKDWhdOPsrxDr47717725 = EPqibXBSKDWhdOPsrxDr93954142;     EPqibXBSKDWhdOPsrxDr93954142 = EPqibXBSKDWhdOPsrxDr59513480;     EPqibXBSKDWhdOPsrxDr59513480 = EPqibXBSKDWhdOPsrxDr44914396;     EPqibXBSKDWhdOPsrxDr44914396 = EPqibXBSKDWhdOPsrxDr64516555;     EPqibXBSKDWhdOPsrxDr64516555 = EPqibXBSKDWhdOPsrxDr7245728;     EPqibXBSKDWhdOPsrxDr7245728 = EPqibXBSKDWhdOPsrxDr24816744;     EPqibXBSKDWhdOPsrxDr24816744 = EPqibXBSKDWhdOPsrxDr84788842;     EPqibXBSKDWhdOPsrxDr84788842 = EPqibXBSKDWhdOPsrxDr87412698;     EPqibXBSKDWhdOPsrxDr87412698 = EPqibXBSKDWhdOPsrxDr20901507;     EPqibXBSKDWhdOPsrxDr20901507 = EPqibXBSKDWhdOPsrxDr97440712;     EPqibXBSKDWhdOPsrxDr97440712 = EPqibXBSKDWhdOPsrxDr6457827;     EPqibXBSKDWhdOPsrxDr6457827 = EPqibXBSKDWhdOPsrxDr13747804;     EPqibXBSKDWhdOPsrxDr13747804 = EPqibXBSKDWhdOPsrxDr32373331;     EPqibXBSKDWhdOPsrxDr32373331 = EPqibXBSKDWhdOPsrxDr39621918;     EPqibXBSKDWhdOPsrxDr39621918 = EPqibXBSKDWhdOPsrxDr67595622;     EPqibXBSKDWhdOPsrxDr67595622 = EPqibXBSKDWhdOPsrxDr55784655;     EPqibXBSKDWhdOPsrxDr55784655 = EPqibXBSKDWhdOPsrxDr75702081;     EPqibXBSKDWhdOPsrxDr75702081 = EPqibXBSKDWhdOPsrxDr69879342;     EPqibXBSKDWhdOPsrxDr69879342 = EPqibXBSKDWhdOPsrxDr18784329;     EPqibXBSKDWhdOPsrxDr18784329 = EPqibXBSKDWhdOPsrxDr633123;     EPqibXBSKDWhdOPsrxDr633123 = EPqibXBSKDWhdOPsrxDr51657571;     EPqibXBSKDWhdOPsrxDr51657571 = EPqibXBSKDWhdOPsrxDr78668143;     EPqibXBSKDWhdOPsrxDr78668143 = EPqibXBSKDWhdOPsrxDr29095610;     EPqibXBSKDWhdOPsrxDr29095610 = EPqibXBSKDWhdOPsrxDr27935755;     EPqibXBSKDWhdOPsrxDr27935755 = EPqibXBSKDWhdOPsrxDr40374913;     EPqibXBSKDWhdOPsrxDr40374913 = EPqibXBSKDWhdOPsrxDr18976397;     EPqibXBSKDWhdOPsrxDr18976397 = EPqibXBSKDWhdOPsrxDr27775262;     EPqibXBSKDWhdOPsrxDr27775262 = EPqibXBSKDWhdOPsrxDr43052934;     EPqibXBSKDWhdOPsrxDr43052934 = EPqibXBSKDWhdOPsrxDr57618213;     EPqibXBSKDWhdOPsrxDr57618213 = EPqibXBSKDWhdOPsrxDr37538997;     EPqibXBSKDWhdOPsrxDr37538997 = EPqibXBSKDWhdOPsrxDr82539622;     EPqibXBSKDWhdOPsrxDr82539622 = EPqibXBSKDWhdOPsrxDr89234061;     EPqibXBSKDWhdOPsrxDr89234061 = EPqibXBSKDWhdOPsrxDr45426976;     EPqibXBSKDWhdOPsrxDr45426976 = EPqibXBSKDWhdOPsrxDr63388885;     EPqibXBSKDWhdOPsrxDr63388885 = EPqibXBSKDWhdOPsrxDr48849566;     EPqibXBSKDWhdOPsrxDr48849566 = EPqibXBSKDWhdOPsrxDr88861960;     EPqibXBSKDWhdOPsrxDr88861960 = EPqibXBSKDWhdOPsrxDr12672312;     EPqibXBSKDWhdOPsrxDr12672312 = EPqibXBSKDWhdOPsrxDr28301556;     EPqibXBSKDWhdOPsrxDr28301556 = EPqibXBSKDWhdOPsrxDr67553275;     EPqibXBSKDWhdOPsrxDr67553275 = EPqibXBSKDWhdOPsrxDr787461;     EPqibXBSKDWhdOPsrxDr787461 = EPqibXBSKDWhdOPsrxDr50421357;     EPqibXBSKDWhdOPsrxDr50421357 = EPqibXBSKDWhdOPsrxDr368231;     EPqibXBSKDWhdOPsrxDr368231 = EPqibXBSKDWhdOPsrxDr67911510;     EPqibXBSKDWhdOPsrxDr67911510 = EPqibXBSKDWhdOPsrxDr94144139;     EPqibXBSKDWhdOPsrxDr94144139 = EPqibXBSKDWhdOPsrxDr80874132;     EPqibXBSKDWhdOPsrxDr80874132 = EPqibXBSKDWhdOPsrxDr27584902;     EPqibXBSKDWhdOPsrxDr27584902 = EPqibXBSKDWhdOPsrxDr55894432;     EPqibXBSKDWhdOPsrxDr55894432 = EPqibXBSKDWhdOPsrxDr45015494;     EPqibXBSKDWhdOPsrxDr45015494 = EPqibXBSKDWhdOPsrxDr86665725;     EPqibXBSKDWhdOPsrxDr86665725 = EPqibXBSKDWhdOPsrxDr48416833;     EPqibXBSKDWhdOPsrxDr48416833 = EPqibXBSKDWhdOPsrxDr97819624;     EPqibXBSKDWhdOPsrxDr97819624 = EPqibXBSKDWhdOPsrxDr44501806;     EPqibXBSKDWhdOPsrxDr44501806 = EPqibXBSKDWhdOPsrxDr31449360;     EPqibXBSKDWhdOPsrxDr31449360 = EPqibXBSKDWhdOPsrxDr3753225;     EPqibXBSKDWhdOPsrxDr3753225 = EPqibXBSKDWhdOPsrxDr96060154;     EPqibXBSKDWhdOPsrxDr96060154 = EPqibXBSKDWhdOPsrxDr15286000;     EPqibXBSKDWhdOPsrxDr15286000 = EPqibXBSKDWhdOPsrxDr30417871;     EPqibXBSKDWhdOPsrxDr30417871 = EPqibXBSKDWhdOPsrxDr16978642;     EPqibXBSKDWhdOPsrxDr16978642 = EPqibXBSKDWhdOPsrxDr24141642;     EPqibXBSKDWhdOPsrxDr24141642 = EPqibXBSKDWhdOPsrxDr88269331;     EPqibXBSKDWhdOPsrxDr88269331 = EPqibXBSKDWhdOPsrxDr97041482;     EPqibXBSKDWhdOPsrxDr97041482 = EPqibXBSKDWhdOPsrxDr41735909;     EPqibXBSKDWhdOPsrxDr41735909 = EPqibXBSKDWhdOPsrxDr29794485;     EPqibXBSKDWhdOPsrxDr29794485 = EPqibXBSKDWhdOPsrxDr83362509;     EPqibXBSKDWhdOPsrxDr83362509 = EPqibXBSKDWhdOPsrxDr14901090;     EPqibXBSKDWhdOPsrxDr14901090 = EPqibXBSKDWhdOPsrxDr17223766;     EPqibXBSKDWhdOPsrxDr17223766 = EPqibXBSKDWhdOPsrxDr68320828;     EPqibXBSKDWhdOPsrxDr68320828 = EPqibXBSKDWhdOPsrxDr68984445;     EPqibXBSKDWhdOPsrxDr68984445 = EPqibXBSKDWhdOPsrxDr15267714;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void fFrNnMesWuprOJeoxesn75096675() {     float VzLfgPzbKjzQIoMpNRHp85919380 = -619653253;    float VzLfgPzbKjzQIoMpNRHp50381136 = -5573015;    float VzLfgPzbKjzQIoMpNRHp87213078 = -968058682;    float VzLfgPzbKjzQIoMpNRHp52343960 = -668732611;    float VzLfgPzbKjzQIoMpNRHp18764603 = -757819607;    float VzLfgPzbKjzQIoMpNRHp51819464 = -503615605;    float VzLfgPzbKjzQIoMpNRHp29439599 = -446446298;    float VzLfgPzbKjzQIoMpNRHp53763152 = -782634341;    float VzLfgPzbKjzQIoMpNRHp85332803 = -975024364;    float VzLfgPzbKjzQIoMpNRHp72779783 = -916629176;    float VzLfgPzbKjzQIoMpNRHp53355772 = -517224093;    float VzLfgPzbKjzQIoMpNRHp88350279 = -75073671;    float VzLfgPzbKjzQIoMpNRHp28202828 = -215635453;    float VzLfgPzbKjzQIoMpNRHp41433742 = -990670762;    float VzLfgPzbKjzQIoMpNRHp75538130 = -529490706;    float VzLfgPzbKjzQIoMpNRHp7327644 = -468768398;    float VzLfgPzbKjzQIoMpNRHp19457032 = -472272027;    float VzLfgPzbKjzQIoMpNRHp55503694 = -570750216;    float VzLfgPzbKjzQIoMpNRHp69488710 = 28443775;    float VzLfgPzbKjzQIoMpNRHp47599726 = 16044954;    float VzLfgPzbKjzQIoMpNRHp46720606 = -168063093;    float VzLfgPzbKjzQIoMpNRHp48727724 = -962086077;    float VzLfgPzbKjzQIoMpNRHp43276514 = -924958161;    float VzLfgPzbKjzQIoMpNRHp22117300 = -639770536;    float VzLfgPzbKjzQIoMpNRHp48761800 = -630145629;    float VzLfgPzbKjzQIoMpNRHp84858928 = -226161063;    float VzLfgPzbKjzQIoMpNRHp12462274 = -253972011;    float VzLfgPzbKjzQIoMpNRHp81303931 = -309583832;    float VzLfgPzbKjzQIoMpNRHp46523637 = -518047020;    float VzLfgPzbKjzQIoMpNRHp89405784 = -628356070;    float VzLfgPzbKjzQIoMpNRHp11458344 = -976760138;    float VzLfgPzbKjzQIoMpNRHp92229185 = -628143124;    float VzLfgPzbKjzQIoMpNRHp76899503 = -295056791;    float VzLfgPzbKjzQIoMpNRHp24878456 = -707519823;    float VzLfgPzbKjzQIoMpNRHp22938633 = -250272229;    float VzLfgPzbKjzQIoMpNRHp24509814 = -991973857;    float VzLfgPzbKjzQIoMpNRHp51037214 = -30243828;    float VzLfgPzbKjzQIoMpNRHp77609232 = -229503021;    float VzLfgPzbKjzQIoMpNRHp63322131 = -491795509;    float VzLfgPzbKjzQIoMpNRHp82515996 = -369630957;    float VzLfgPzbKjzQIoMpNRHp80974790 = -795062818;    float VzLfgPzbKjzQIoMpNRHp25923704 = -276910125;    float VzLfgPzbKjzQIoMpNRHp55424786 = -936931025;    float VzLfgPzbKjzQIoMpNRHp20256104 = -21844572;    float VzLfgPzbKjzQIoMpNRHp28327389 = -539559172;    float VzLfgPzbKjzQIoMpNRHp46027753 = -633469220;    float VzLfgPzbKjzQIoMpNRHp93726825 = -839008760;    float VzLfgPzbKjzQIoMpNRHp21398866 = -195031439;    float VzLfgPzbKjzQIoMpNRHp81143533 = -46928273;    float VzLfgPzbKjzQIoMpNRHp78306829 = 43515268;    float VzLfgPzbKjzQIoMpNRHp86973653 = -66497619;    float VzLfgPzbKjzQIoMpNRHp42504627 = -123723717;    float VzLfgPzbKjzQIoMpNRHp6733229 = -273968339;    float VzLfgPzbKjzQIoMpNRHp68895447 = -267763652;    float VzLfgPzbKjzQIoMpNRHp28376069 = 7271931;    float VzLfgPzbKjzQIoMpNRHp37191657 = -657567177;    float VzLfgPzbKjzQIoMpNRHp7104622 = -80614854;    float VzLfgPzbKjzQIoMpNRHp65095778 = -228288147;    float VzLfgPzbKjzQIoMpNRHp3582160 = 61413017;    float VzLfgPzbKjzQIoMpNRHp33905675 = -431658545;    float VzLfgPzbKjzQIoMpNRHp39357190 = -149643594;    float VzLfgPzbKjzQIoMpNRHp48135668 = -36862466;    float VzLfgPzbKjzQIoMpNRHp7239516 = -164587321;    float VzLfgPzbKjzQIoMpNRHp95927019 = -246668294;    float VzLfgPzbKjzQIoMpNRHp61321440 = -939869039;    float VzLfgPzbKjzQIoMpNRHp61126586 = -889080970;    float VzLfgPzbKjzQIoMpNRHp11450776 = -780016881;    float VzLfgPzbKjzQIoMpNRHp3324373 = -508115631;    float VzLfgPzbKjzQIoMpNRHp18495109 = -640398533;    float VzLfgPzbKjzQIoMpNRHp51028317 = -537516850;    float VzLfgPzbKjzQIoMpNRHp56290429 = -338524571;    float VzLfgPzbKjzQIoMpNRHp41847799 = -142769006;    float VzLfgPzbKjzQIoMpNRHp92181562 = 21045293;    float VzLfgPzbKjzQIoMpNRHp86972713 = -601925268;    float VzLfgPzbKjzQIoMpNRHp66624935 = -188892229;    float VzLfgPzbKjzQIoMpNRHp20796903 = -891152969;    float VzLfgPzbKjzQIoMpNRHp93302937 = 74844948;    float VzLfgPzbKjzQIoMpNRHp23020411 = -803113590;    float VzLfgPzbKjzQIoMpNRHp93789910 = -211364;    float VzLfgPzbKjzQIoMpNRHp2734048 = -996676410;    float VzLfgPzbKjzQIoMpNRHp91132102 = -387152303;    float VzLfgPzbKjzQIoMpNRHp91063408 = 41059427;    float VzLfgPzbKjzQIoMpNRHp160398 = -162655560;    float VzLfgPzbKjzQIoMpNRHp68216807 = -461562289;    float VzLfgPzbKjzQIoMpNRHp2432131 = -461858452;    float VzLfgPzbKjzQIoMpNRHp68953717 = -753036421;    float VzLfgPzbKjzQIoMpNRHp85495957 = -254174785;    float VzLfgPzbKjzQIoMpNRHp8004056 = 72706861;    float VzLfgPzbKjzQIoMpNRHp96502386 = -614791754;    float VzLfgPzbKjzQIoMpNRHp85746976 = -592705052;    float VzLfgPzbKjzQIoMpNRHp17405192 = -811359003;    float VzLfgPzbKjzQIoMpNRHp85941435 = -801955682;    float VzLfgPzbKjzQIoMpNRHp74027073 = -190916039;    float VzLfgPzbKjzQIoMpNRHp29416457 = 39863035;    float VzLfgPzbKjzQIoMpNRHp43158806 = -119987364;    float VzLfgPzbKjzQIoMpNRHp32839123 = -658200352;    float VzLfgPzbKjzQIoMpNRHp18684188 = -12322804;    float VzLfgPzbKjzQIoMpNRHp59497767 = -590262731;    float VzLfgPzbKjzQIoMpNRHp58934664 = -81975534;    float VzLfgPzbKjzQIoMpNRHp67200803 = -619653253;     VzLfgPzbKjzQIoMpNRHp85919380 = VzLfgPzbKjzQIoMpNRHp50381136;     VzLfgPzbKjzQIoMpNRHp50381136 = VzLfgPzbKjzQIoMpNRHp87213078;     VzLfgPzbKjzQIoMpNRHp87213078 = VzLfgPzbKjzQIoMpNRHp52343960;     VzLfgPzbKjzQIoMpNRHp52343960 = VzLfgPzbKjzQIoMpNRHp18764603;     VzLfgPzbKjzQIoMpNRHp18764603 = VzLfgPzbKjzQIoMpNRHp51819464;     VzLfgPzbKjzQIoMpNRHp51819464 = VzLfgPzbKjzQIoMpNRHp29439599;     VzLfgPzbKjzQIoMpNRHp29439599 = VzLfgPzbKjzQIoMpNRHp53763152;     VzLfgPzbKjzQIoMpNRHp53763152 = VzLfgPzbKjzQIoMpNRHp85332803;     VzLfgPzbKjzQIoMpNRHp85332803 = VzLfgPzbKjzQIoMpNRHp72779783;     VzLfgPzbKjzQIoMpNRHp72779783 = VzLfgPzbKjzQIoMpNRHp53355772;     VzLfgPzbKjzQIoMpNRHp53355772 = VzLfgPzbKjzQIoMpNRHp88350279;     VzLfgPzbKjzQIoMpNRHp88350279 = VzLfgPzbKjzQIoMpNRHp28202828;     VzLfgPzbKjzQIoMpNRHp28202828 = VzLfgPzbKjzQIoMpNRHp41433742;     VzLfgPzbKjzQIoMpNRHp41433742 = VzLfgPzbKjzQIoMpNRHp75538130;     VzLfgPzbKjzQIoMpNRHp75538130 = VzLfgPzbKjzQIoMpNRHp7327644;     VzLfgPzbKjzQIoMpNRHp7327644 = VzLfgPzbKjzQIoMpNRHp19457032;     VzLfgPzbKjzQIoMpNRHp19457032 = VzLfgPzbKjzQIoMpNRHp55503694;     VzLfgPzbKjzQIoMpNRHp55503694 = VzLfgPzbKjzQIoMpNRHp69488710;     VzLfgPzbKjzQIoMpNRHp69488710 = VzLfgPzbKjzQIoMpNRHp47599726;     VzLfgPzbKjzQIoMpNRHp47599726 = VzLfgPzbKjzQIoMpNRHp46720606;     VzLfgPzbKjzQIoMpNRHp46720606 = VzLfgPzbKjzQIoMpNRHp48727724;     VzLfgPzbKjzQIoMpNRHp48727724 = VzLfgPzbKjzQIoMpNRHp43276514;     VzLfgPzbKjzQIoMpNRHp43276514 = VzLfgPzbKjzQIoMpNRHp22117300;     VzLfgPzbKjzQIoMpNRHp22117300 = VzLfgPzbKjzQIoMpNRHp48761800;     VzLfgPzbKjzQIoMpNRHp48761800 = VzLfgPzbKjzQIoMpNRHp84858928;     VzLfgPzbKjzQIoMpNRHp84858928 = VzLfgPzbKjzQIoMpNRHp12462274;     VzLfgPzbKjzQIoMpNRHp12462274 = VzLfgPzbKjzQIoMpNRHp81303931;     VzLfgPzbKjzQIoMpNRHp81303931 = VzLfgPzbKjzQIoMpNRHp46523637;     VzLfgPzbKjzQIoMpNRHp46523637 = VzLfgPzbKjzQIoMpNRHp89405784;     VzLfgPzbKjzQIoMpNRHp89405784 = VzLfgPzbKjzQIoMpNRHp11458344;     VzLfgPzbKjzQIoMpNRHp11458344 = VzLfgPzbKjzQIoMpNRHp92229185;     VzLfgPzbKjzQIoMpNRHp92229185 = VzLfgPzbKjzQIoMpNRHp76899503;     VzLfgPzbKjzQIoMpNRHp76899503 = VzLfgPzbKjzQIoMpNRHp24878456;     VzLfgPzbKjzQIoMpNRHp24878456 = VzLfgPzbKjzQIoMpNRHp22938633;     VzLfgPzbKjzQIoMpNRHp22938633 = VzLfgPzbKjzQIoMpNRHp24509814;     VzLfgPzbKjzQIoMpNRHp24509814 = VzLfgPzbKjzQIoMpNRHp51037214;     VzLfgPzbKjzQIoMpNRHp51037214 = VzLfgPzbKjzQIoMpNRHp77609232;     VzLfgPzbKjzQIoMpNRHp77609232 = VzLfgPzbKjzQIoMpNRHp63322131;     VzLfgPzbKjzQIoMpNRHp63322131 = VzLfgPzbKjzQIoMpNRHp82515996;     VzLfgPzbKjzQIoMpNRHp82515996 = VzLfgPzbKjzQIoMpNRHp80974790;     VzLfgPzbKjzQIoMpNRHp80974790 = VzLfgPzbKjzQIoMpNRHp25923704;     VzLfgPzbKjzQIoMpNRHp25923704 = VzLfgPzbKjzQIoMpNRHp55424786;     VzLfgPzbKjzQIoMpNRHp55424786 = VzLfgPzbKjzQIoMpNRHp20256104;     VzLfgPzbKjzQIoMpNRHp20256104 = VzLfgPzbKjzQIoMpNRHp28327389;     VzLfgPzbKjzQIoMpNRHp28327389 = VzLfgPzbKjzQIoMpNRHp46027753;     VzLfgPzbKjzQIoMpNRHp46027753 = VzLfgPzbKjzQIoMpNRHp93726825;     VzLfgPzbKjzQIoMpNRHp93726825 = VzLfgPzbKjzQIoMpNRHp21398866;     VzLfgPzbKjzQIoMpNRHp21398866 = VzLfgPzbKjzQIoMpNRHp81143533;     VzLfgPzbKjzQIoMpNRHp81143533 = VzLfgPzbKjzQIoMpNRHp78306829;     VzLfgPzbKjzQIoMpNRHp78306829 = VzLfgPzbKjzQIoMpNRHp86973653;     VzLfgPzbKjzQIoMpNRHp86973653 = VzLfgPzbKjzQIoMpNRHp42504627;     VzLfgPzbKjzQIoMpNRHp42504627 = VzLfgPzbKjzQIoMpNRHp6733229;     VzLfgPzbKjzQIoMpNRHp6733229 = VzLfgPzbKjzQIoMpNRHp68895447;     VzLfgPzbKjzQIoMpNRHp68895447 = VzLfgPzbKjzQIoMpNRHp28376069;     VzLfgPzbKjzQIoMpNRHp28376069 = VzLfgPzbKjzQIoMpNRHp37191657;     VzLfgPzbKjzQIoMpNRHp37191657 = VzLfgPzbKjzQIoMpNRHp7104622;     VzLfgPzbKjzQIoMpNRHp7104622 = VzLfgPzbKjzQIoMpNRHp65095778;     VzLfgPzbKjzQIoMpNRHp65095778 = VzLfgPzbKjzQIoMpNRHp3582160;     VzLfgPzbKjzQIoMpNRHp3582160 = VzLfgPzbKjzQIoMpNRHp33905675;     VzLfgPzbKjzQIoMpNRHp33905675 = VzLfgPzbKjzQIoMpNRHp39357190;     VzLfgPzbKjzQIoMpNRHp39357190 = VzLfgPzbKjzQIoMpNRHp48135668;     VzLfgPzbKjzQIoMpNRHp48135668 = VzLfgPzbKjzQIoMpNRHp7239516;     VzLfgPzbKjzQIoMpNRHp7239516 = VzLfgPzbKjzQIoMpNRHp95927019;     VzLfgPzbKjzQIoMpNRHp95927019 = VzLfgPzbKjzQIoMpNRHp61321440;     VzLfgPzbKjzQIoMpNRHp61321440 = VzLfgPzbKjzQIoMpNRHp61126586;     VzLfgPzbKjzQIoMpNRHp61126586 = VzLfgPzbKjzQIoMpNRHp11450776;     VzLfgPzbKjzQIoMpNRHp11450776 = VzLfgPzbKjzQIoMpNRHp3324373;     VzLfgPzbKjzQIoMpNRHp3324373 = VzLfgPzbKjzQIoMpNRHp18495109;     VzLfgPzbKjzQIoMpNRHp18495109 = VzLfgPzbKjzQIoMpNRHp51028317;     VzLfgPzbKjzQIoMpNRHp51028317 = VzLfgPzbKjzQIoMpNRHp56290429;     VzLfgPzbKjzQIoMpNRHp56290429 = VzLfgPzbKjzQIoMpNRHp41847799;     VzLfgPzbKjzQIoMpNRHp41847799 = VzLfgPzbKjzQIoMpNRHp92181562;     VzLfgPzbKjzQIoMpNRHp92181562 = VzLfgPzbKjzQIoMpNRHp86972713;     VzLfgPzbKjzQIoMpNRHp86972713 = VzLfgPzbKjzQIoMpNRHp66624935;     VzLfgPzbKjzQIoMpNRHp66624935 = VzLfgPzbKjzQIoMpNRHp20796903;     VzLfgPzbKjzQIoMpNRHp20796903 = VzLfgPzbKjzQIoMpNRHp93302937;     VzLfgPzbKjzQIoMpNRHp93302937 = VzLfgPzbKjzQIoMpNRHp23020411;     VzLfgPzbKjzQIoMpNRHp23020411 = VzLfgPzbKjzQIoMpNRHp93789910;     VzLfgPzbKjzQIoMpNRHp93789910 = VzLfgPzbKjzQIoMpNRHp2734048;     VzLfgPzbKjzQIoMpNRHp2734048 = VzLfgPzbKjzQIoMpNRHp91132102;     VzLfgPzbKjzQIoMpNRHp91132102 = VzLfgPzbKjzQIoMpNRHp91063408;     VzLfgPzbKjzQIoMpNRHp91063408 = VzLfgPzbKjzQIoMpNRHp160398;     VzLfgPzbKjzQIoMpNRHp160398 = VzLfgPzbKjzQIoMpNRHp68216807;     VzLfgPzbKjzQIoMpNRHp68216807 = VzLfgPzbKjzQIoMpNRHp2432131;     VzLfgPzbKjzQIoMpNRHp2432131 = VzLfgPzbKjzQIoMpNRHp68953717;     VzLfgPzbKjzQIoMpNRHp68953717 = VzLfgPzbKjzQIoMpNRHp85495957;     VzLfgPzbKjzQIoMpNRHp85495957 = VzLfgPzbKjzQIoMpNRHp8004056;     VzLfgPzbKjzQIoMpNRHp8004056 = VzLfgPzbKjzQIoMpNRHp96502386;     VzLfgPzbKjzQIoMpNRHp96502386 = VzLfgPzbKjzQIoMpNRHp85746976;     VzLfgPzbKjzQIoMpNRHp85746976 = VzLfgPzbKjzQIoMpNRHp17405192;     VzLfgPzbKjzQIoMpNRHp17405192 = VzLfgPzbKjzQIoMpNRHp85941435;     VzLfgPzbKjzQIoMpNRHp85941435 = VzLfgPzbKjzQIoMpNRHp74027073;     VzLfgPzbKjzQIoMpNRHp74027073 = VzLfgPzbKjzQIoMpNRHp29416457;     VzLfgPzbKjzQIoMpNRHp29416457 = VzLfgPzbKjzQIoMpNRHp43158806;     VzLfgPzbKjzQIoMpNRHp43158806 = VzLfgPzbKjzQIoMpNRHp32839123;     VzLfgPzbKjzQIoMpNRHp32839123 = VzLfgPzbKjzQIoMpNRHp18684188;     VzLfgPzbKjzQIoMpNRHp18684188 = VzLfgPzbKjzQIoMpNRHp59497767;     VzLfgPzbKjzQIoMpNRHp59497767 = VzLfgPzbKjzQIoMpNRHp58934664;     VzLfgPzbKjzQIoMpNRHp58934664 = VzLfgPzbKjzQIoMpNRHp67200803;     VzLfgPzbKjzQIoMpNRHp67200803 = VzLfgPzbKjzQIoMpNRHp85919380;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DxXtMKrVscXwVUYLBHSq81755419() {     float LDIHIIiynTOBYDlUIJyE85912909 = -492177394;    float LDIHIIiynTOBYDlUIJyE88953452 = -48168140;    float LDIHIIiynTOBYDlUIJyE88526290 = -925187296;    float LDIHIIiynTOBYDlUIJyE37423821 = -489273441;    float LDIHIIiynTOBYDlUIJyE27604705 = -126174929;    float LDIHIIiynTOBYDlUIJyE86624896 = -203048417;    float LDIHIIiynTOBYDlUIJyE97986934 = -150982765;    float LDIHIIiynTOBYDlUIJyE22977203 = -486811658;    float LDIHIIiynTOBYDlUIJyE68072967 = -688670434;    float LDIHIIiynTOBYDlUIJyE69419297 = -553436047;    float LDIHIIiynTOBYDlUIJyE73676377 = -476696165;    float LDIHIIiynTOBYDlUIJyE2641957 = -910552033;    float LDIHIIiynTOBYDlUIJyE60055741 = -237293928;    float LDIHIIiynTOBYDlUIJyE95277961 = -235823813;    float LDIHIIiynTOBYDlUIJyE20710240 = -492203558;    float LDIHIIiynTOBYDlUIJyE81892190 = -801564049;    float LDIHIIiynTOBYDlUIJyE75309791 = -291886597;    float LDIHIIiynTOBYDlUIJyE78504227 = -967821837;    float LDIHIIiynTOBYDlUIJyE89916980 = -589538492;    float LDIHIIiynTOBYDlUIJyE19673489 = -595979497;    float LDIHIIiynTOBYDlUIJyE26390031 = -978529467;    float LDIHIIiynTOBYDlUIJyE27023990 = -372077960;    float LDIHIIiynTOBYDlUIJyE80154910 = -283916003;    float LDIHIIiynTOBYDlUIJyE65767408 = -520691058;    float LDIHIIiynTOBYDlUIJyE77204490 = 43160518;    float LDIHIIiynTOBYDlUIJyE1433005 = -134366991;    float LDIHIIiynTOBYDlUIJyE82683855 = -554771724;    float LDIHIIiynTOBYDlUIJyE16707456 = -918550820;    float LDIHIIiynTOBYDlUIJyE59202083 = -64117335;    float LDIHIIiynTOBYDlUIJyE65495654 = -303787444;    float LDIHIIiynTOBYDlUIJyE44871022 = -263853372;    float LDIHIIiynTOBYDlUIJyE76953043 = 80035115;    float LDIHIIiynTOBYDlUIJyE62351926 = -497001007;    float LDIHIIiynTOBYDlUIJyE25161293 = -584201555;    float LDIHIIiynTOBYDlUIJyE5296671 = -509586153;    float LDIHIIiynTOBYDlUIJyE67338003 = 28260397;    float LDIHIIiynTOBYDlUIJyE62978581 = -129897206;    float LDIHIIiynTOBYDlUIJyE47526912 = -224765700;    float LDIHIIiynTOBYDlUIJyE98635728 = -104476925;    float LDIHIIiynTOBYDlUIJyE54388889 = 20937357;    float LDIHIIiynTOBYDlUIJyE68830458 = -883006716;    float LDIHIIiynTOBYDlUIJyE11621365 = -470702207;    float LDIHIIiynTOBYDlUIJyE15708147 = -396061279;    float LDIHIIiynTOBYDlUIJyE79488002 = -678809015;    float LDIHIIiynTOBYDlUIJyE12345442 = 87181426;    float LDIHIIiynTOBYDlUIJyE86455721 = -445513891;    float LDIHIIiynTOBYDlUIJyE21424618 = -220289383;    float LDIHIIiynTOBYDlUIJyE96782446 = -803792795;    float LDIHIIiynTOBYDlUIJyE99801923 = -710692134;    float LDIHIIiynTOBYDlUIJyE98352251 = -643509618;    float LDIHIIiynTOBYDlUIJyE233023 = -745052627;    float LDIHIIiynTOBYDlUIJyE57051884 = -505869465;    float LDIHIIiynTOBYDlUIJyE82181576 = -166022347;    float LDIHIIiynTOBYDlUIJyE79145718 = -799683503;    float LDIHIIiynTOBYDlUIJyE55365389 = -849627347;    float LDIHIIiynTOBYDlUIJyE58888919 = -20099435;    float LDIHIIiynTOBYDlUIJyE8798542 = -764252137;    float LDIHIIiynTOBYDlUIJyE22758883 = -304496238;    float LDIHIIiynTOBYDlUIJyE60219331 = -432433960;    float LDIHIIiynTOBYDlUIJyE26171700 = -991807938;    float LDIHIIiynTOBYDlUIJyE3941041 = -648276693;    float LDIHIIiynTOBYDlUIJyE81279478 = -232431945;    float LDIHIIiynTOBYDlUIJyE63775119 = -322694324;    float LDIHIIiynTOBYDlUIJyE2577314 = -284882990;    float LDIHIIiynTOBYDlUIJyE24548275 = -189582675;    float LDIHIIiynTOBYDlUIJyE96723333 = -456731281;    float LDIHIIiynTOBYDlUIJyE40290030 = -313551027;    float LDIHIIiynTOBYDlUIJyE34894449 = -653092373;    float LDIHIIiynTOBYDlUIJyE89981291 = -726237660;    float LDIHIIiynTOBYDlUIJyE53372236 = -420463955;    float LDIHIIiynTOBYDlUIJyE18913609 = -571666843;    float LDIHIIiynTOBYDlUIJyE27782879 = 32879103;    float LDIHIIiynTOBYDlUIJyE79868499 = -763344912;    float LDIHIIiynTOBYDlUIJyE35528091 = -510475849;    float LDIHIIiynTOBYDlUIJyE50843030 = -712972782;    float LDIHIIiynTOBYDlUIJyE14768666 = -407827260;    float LDIHIIiynTOBYDlUIJyE11315844 = -976016682;    float LDIHIIiynTOBYDlUIJyE666909 = -605106988;    float LDIHIIiynTOBYDlUIJyE53421967 = -507872485;    float LDIHIIiynTOBYDlUIJyE90748768 = -511325592;    float LDIHIIiynTOBYDlUIJyE80008387 = -914077609;    float LDIHIIiynTOBYDlUIJyE85901409 = -750978930;    float LDIHIIiynTOBYDlUIJyE16905533 = -107858687;    float LDIHIIiynTOBYDlUIJyE60849831 = -420607718;    float LDIHIIiynTOBYDlUIJyE65262631 = -558734818;    float LDIHIIiynTOBYDlUIJyE87819138 = -757983908;    float LDIHIIiynTOBYDlUIJyE94771466 = -753942538;    float LDIHIIiynTOBYDlUIJyE83206208 = -697317504;    float LDIHIIiynTOBYDlUIJyE69795903 = -734574209;    float LDIHIIiynTOBYDlUIJyE46407751 = -389486719;    float LDIHIIiynTOBYDlUIJyE58539461 = -207487466;    float LDIHIIiynTOBYDlUIJyE40219699 = -825400969;    float LDIHIIiynTOBYDlUIJyE87307581 = -792331741;    float LDIHIIiynTOBYDlUIJyE72464028 = -112668988;    float LDIHIIiynTOBYDlUIJyE50447849 = -330785951;    float LDIHIIiynTOBYDlUIJyE87550979 = -550574772;    float LDIHIIiynTOBYDlUIJyE47846245 = -48007884;    float LDIHIIiynTOBYDlUIJyE13130833 = -11178289;    float LDIHIIiynTOBYDlUIJyE54939727 = -389226340;    float LDIHIIiynTOBYDlUIJyE15622108 = -492177394;     LDIHIIiynTOBYDlUIJyE85912909 = LDIHIIiynTOBYDlUIJyE88953452;     LDIHIIiynTOBYDlUIJyE88953452 = LDIHIIiynTOBYDlUIJyE88526290;     LDIHIIiynTOBYDlUIJyE88526290 = LDIHIIiynTOBYDlUIJyE37423821;     LDIHIIiynTOBYDlUIJyE37423821 = LDIHIIiynTOBYDlUIJyE27604705;     LDIHIIiynTOBYDlUIJyE27604705 = LDIHIIiynTOBYDlUIJyE86624896;     LDIHIIiynTOBYDlUIJyE86624896 = LDIHIIiynTOBYDlUIJyE97986934;     LDIHIIiynTOBYDlUIJyE97986934 = LDIHIIiynTOBYDlUIJyE22977203;     LDIHIIiynTOBYDlUIJyE22977203 = LDIHIIiynTOBYDlUIJyE68072967;     LDIHIIiynTOBYDlUIJyE68072967 = LDIHIIiynTOBYDlUIJyE69419297;     LDIHIIiynTOBYDlUIJyE69419297 = LDIHIIiynTOBYDlUIJyE73676377;     LDIHIIiynTOBYDlUIJyE73676377 = LDIHIIiynTOBYDlUIJyE2641957;     LDIHIIiynTOBYDlUIJyE2641957 = LDIHIIiynTOBYDlUIJyE60055741;     LDIHIIiynTOBYDlUIJyE60055741 = LDIHIIiynTOBYDlUIJyE95277961;     LDIHIIiynTOBYDlUIJyE95277961 = LDIHIIiynTOBYDlUIJyE20710240;     LDIHIIiynTOBYDlUIJyE20710240 = LDIHIIiynTOBYDlUIJyE81892190;     LDIHIIiynTOBYDlUIJyE81892190 = LDIHIIiynTOBYDlUIJyE75309791;     LDIHIIiynTOBYDlUIJyE75309791 = LDIHIIiynTOBYDlUIJyE78504227;     LDIHIIiynTOBYDlUIJyE78504227 = LDIHIIiynTOBYDlUIJyE89916980;     LDIHIIiynTOBYDlUIJyE89916980 = LDIHIIiynTOBYDlUIJyE19673489;     LDIHIIiynTOBYDlUIJyE19673489 = LDIHIIiynTOBYDlUIJyE26390031;     LDIHIIiynTOBYDlUIJyE26390031 = LDIHIIiynTOBYDlUIJyE27023990;     LDIHIIiynTOBYDlUIJyE27023990 = LDIHIIiynTOBYDlUIJyE80154910;     LDIHIIiynTOBYDlUIJyE80154910 = LDIHIIiynTOBYDlUIJyE65767408;     LDIHIIiynTOBYDlUIJyE65767408 = LDIHIIiynTOBYDlUIJyE77204490;     LDIHIIiynTOBYDlUIJyE77204490 = LDIHIIiynTOBYDlUIJyE1433005;     LDIHIIiynTOBYDlUIJyE1433005 = LDIHIIiynTOBYDlUIJyE82683855;     LDIHIIiynTOBYDlUIJyE82683855 = LDIHIIiynTOBYDlUIJyE16707456;     LDIHIIiynTOBYDlUIJyE16707456 = LDIHIIiynTOBYDlUIJyE59202083;     LDIHIIiynTOBYDlUIJyE59202083 = LDIHIIiynTOBYDlUIJyE65495654;     LDIHIIiynTOBYDlUIJyE65495654 = LDIHIIiynTOBYDlUIJyE44871022;     LDIHIIiynTOBYDlUIJyE44871022 = LDIHIIiynTOBYDlUIJyE76953043;     LDIHIIiynTOBYDlUIJyE76953043 = LDIHIIiynTOBYDlUIJyE62351926;     LDIHIIiynTOBYDlUIJyE62351926 = LDIHIIiynTOBYDlUIJyE25161293;     LDIHIIiynTOBYDlUIJyE25161293 = LDIHIIiynTOBYDlUIJyE5296671;     LDIHIIiynTOBYDlUIJyE5296671 = LDIHIIiynTOBYDlUIJyE67338003;     LDIHIIiynTOBYDlUIJyE67338003 = LDIHIIiynTOBYDlUIJyE62978581;     LDIHIIiynTOBYDlUIJyE62978581 = LDIHIIiynTOBYDlUIJyE47526912;     LDIHIIiynTOBYDlUIJyE47526912 = LDIHIIiynTOBYDlUIJyE98635728;     LDIHIIiynTOBYDlUIJyE98635728 = LDIHIIiynTOBYDlUIJyE54388889;     LDIHIIiynTOBYDlUIJyE54388889 = LDIHIIiynTOBYDlUIJyE68830458;     LDIHIIiynTOBYDlUIJyE68830458 = LDIHIIiynTOBYDlUIJyE11621365;     LDIHIIiynTOBYDlUIJyE11621365 = LDIHIIiynTOBYDlUIJyE15708147;     LDIHIIiynTOBYDlUIJyE15708147 = LDIHIIiynTOBYDlUIJyE79488002;     LDIHIIiynTOBYDlUIJyE79488002 = LDIHIIiynTOBYDlUIJyE12345442;     LDIHIIiynTOBYDlUIJyE12345442 = LDIHIIiynTOBYDlUIJyE86455721;     LDIHIIiynTOBYDlUIJyE86455721 = LDIHIIiynTOBYDlUIJyE21424618;     LDIHIIiynTOBYDlUIJyE21424618 = LDIHIIiynTOBYDlUIJyE96782446;     LDIHIIiynTOBYDlUIJyE96782446 = LDIHIIiynTOBYDlUIJyE99801923;     LDIHIIiynTOBYDlUIJyE99801923 = LDIHIIiynTOBYDlUIJyE98352251;     LDIHIIiynTOBYDlUIJyE98352251 = LDIHIIiynTOBYDlUIJyE233023;     LDIHIIiynTOBYDlUIJyE233023 = LDIHIIiynTOBYDlUIJyE57051884;     LDIHIIiynTOBYDlUIJyE57051884 = LDIHIIiynTOBYDlUIJyE82181576;     LDIHIIiynTOBYDlUIJyE82181576 = LDIHIIiynTOBYDlUIJyE79145718;     LDIHIIiynTOBYDlUIJyE79145718 = LDIHIIiynTOBYDlUIJyE55365389;     LDIHIIiynTOBYDlUIJyE55365389 = LDIHIIiynTOBYDlUIJyE58888919;     LDIHIIiynTOBYDlUIJyE58888919 = LDIHIIiynTOBYDlUIJyE8798542;     LDIHIIiynTOBYDlUIJyE8798542 = LDIHIIiynTOBYDlUIJyE22758883;     LDIHIIiynTOBYDlUIJyE22758883 = LDIHIIiynTOBYDlUIJyE60219331;     LDIHIIiynTOBYDlUIJyE60219331 = LDIHIIiynTOBYDlUIJyE26171700;     LDIHIIiynTOBYDlUIJyE26171700 = LDIHIIiynTOBYDlUIJyE3941041;     LDIHIIiynTOBYDlUIJyE3941041 = LDIHIIiynTOBYDlUIJyE81279478;     LDIHIIiynTOBYDlUIJyE81279478 = LDIHIIiynTOBYDlUIJyE63775119;     LDIHIIiynTOBYDlUIJyE63775119 = LDIHIIiynTOBYDlUIJyE2577314;     LDIHIIiynTOBYDlUIJyE2577314 = LDIHIIiynTOBYDlUIJyE24548275;     LDIHIIiynTOBYDlUIJyE24548275 = LDIHIIiynTOBYDlUIJyE96723333;     LDIHIIiynTOBYDlUIJyE96723333 = LDIHIIiynTOBYDlUIJyE40290030;     LDIHIIiynTOBYDlUIJyE40290030 = LDIHIIiynTOBYDlUIJyE34894449;     LDIHIIiynTOBYDlUIJyE34894449 = LDIHIIiynTOBYDlUIJyE89981291;     LDIHIIiynTOBYDlUIJyE89981291 = LDIHIIiynTOBYDlUIJyE53372236;     LDIHIIiynTOBYDlUIJyE53372236 = LDIHIIiynTOBYDlUIJyE18913609;     LDIHIIiynTOBYDlUIJyE18913609 = LDIHIIiynTOBYDlUIJyE27782879;     LDIHIIiynTOBYDlUIJyE27782879 = LDIHIIiynTOBYDlUIJyE79868499;     LDIHIIiynTOBYDlUIJyE79868499 = LDIHIIiynTOBYDlUIJyE35528091;     LDIHIIiynTOBYDlUIJyE35528091 = LDIHIIiynTOBYDlUIJyE50843030;     LDIHIIiynTOBYDlUIJyE50843030 = LDIHIIiynTOBYDlUIJyE14768666;     LDIHIIiynTOBYDlUIJyE14768666 = LDIHIIiynTOBYDlUIJyE11315844;     LDIHIIiynTOBYDlUIJyE11315844 = LDIHIIiynTOBYDlUIJyE666909;     LDIHIIiynTOBYDlUIJyE666909 = LDIHIIiynTOBYDlUIJyE53421967;     LDIHIIiynTOBYDlUIJyE53421967 = LDIHIIiynTOBYDlUIJyE90748768;     LDIHIIiynTOBYDlUIJyE90748768 = LDIHIIiynTOBYDlUIJyE80008387;     LDIHIIiynTOBYDlUIJyE80008387 = LDIHIIiynTOBYDlUIJyE85901409;     LDIHIIiynTOBYDlUIJyE85901409 = LDIHIIiynTOBYDlUIJyE16905533;     LDIHIIiynTOBYDlUIJyE16905533 = LDIHIIiynTOBYDlUIJyE60849831;     LDIHIIiynTOBYDlUIJyE60849831 = LDIHIIiynTOBYDlUIJyE65262631;     LDIHIIiynTOBYDlUIJyE65262631 = LDIHIIiynTOBYDlUIJyE87819138;     LDIHIIiynTOBYDlUIJyE87819138 = LDIHIIiynTOBYDlUIJyE94771466;     LDIHIIiynTOBYDlUIJyE94771466 = LDIHIIiynTOBYDlUIJyE83206208;     LDIHIIiynTOBYDlUIJyE83206208 = LDIHIIiynTOBYDlUIJyE69795903;     LDIHIIiynTOBYDlUIJyE69795903 = LDIHIIiynTOBYDlUIJyE46407751;     LDIHIIiynTOBYDlUIJyE46407751 = LDIHIIiynTOBYDlUIJyE58539461;     LDIHIIiynTOBYDlUIJyE58539461 = LDIHIIiynTOBYDlUIJyE40219699;     LDIHIIiynTOBYDlUIJyE40219699 = LDIHIIiynTOBYDlUIJyE87307581;     LDIHIIiynTOBYDlUIJyE87307581 = LDIHIIiynTOBYDlUIJyE72464028;     LDIHIIiynTOBYDlUIJyE72464028 = LDIHIIiynTOBYDlUIJyE50447849;     LDIHIIiynTOBYDlUIJyE50447849 = LDIHIIiynTOBYDlUIJyE87550979;     LDIHIIiynTOBYDlUIJyE87550979 = LDIHIIiynTOBYDlUIJyE47846245;     LDIHIIiynTOBYDlUIJyE47846245 = LDIHIIiynTOBYDlUIJyE13130833;     LDIHIIiynTOBYDlUIJyE13130833 = LDIHIIiynTOBYDlUIJyE54939727;     LDIHIIiynTOBYDlUIJyE54939727 = LDIHIIiynTOBYDlUIJyE15622108;     LDIHIIiynTOBYDlUIJyE15622108 = LDIHIIiynTOBYDlUIJyE85912909;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void KlmbMjAUXggokStwkfno36171633() {     float UccgastvHtVSBszLbqJu56564576 = -10326525;    float UccgastvHtVSBszLbqJu57626970 = 80188561;    float UccgastvHtVSBszLbqJu68495209 = -267481756;    float UccgastvHtVSBszLbqJu77355107 = -679411830;    float UccgastvHtVSBszLbqJu49055027 = 90418724;    float UccgastvHtVSBszLbqJu76624659 = 95370432;    float UccgastvHtVSBszLbqJu16365830 = -149758474;    float UccgastvHtVSBszLbqJu79819583 = -833628748;    float UccgastvHtVSBszLbqJu13938022 = -782017762;    float UccgastvHtVSBszLbqJu92385758 = -854531397;    float UccgastvHtVSBszLbqJu15925538 = -511598095;    float UccgastvHtVSBszLbqJu48188527 = -126315826;    float UccgastvHtVSBszLbqJu39883129 = -146830986;    float UccgastvHtVSBszLbqJu79124995 = -36852023;    float UccgastvHtVSBszLbqJu3430259 = -641953561;    float UccgastvHtVSBszLbqJu14420831 = -371547636;    float UccgastvHtVSBszLbqJu69162618 = -201071727;    float UccgastvHtVSBszLbqJu98797721 = -956988033;    float UccgastvHtVSBszLbqJu71624762 = -196726434;    float UccgastvHtVSBszLbqJu78460198 = -38345802;    float UccgastvHtVSBszLbqJu81525785 = -887208607;    float UccgastvHtVSBszLbqJu88419754 = -703363265;    float UccgastvHtVSBszLbqJu82098719 = -851759148;    float UccgastvHtVSBszLbqJu99616945 = -113091965;    float UccgastvHtVSBszLbqJu41328878 = -732744170;    float UccgastvHtVSBszLbqJu32030586 = -232651436;    float UccgastvHtVSBszLbqJu90944641 = -949949500;    float UccgastvHtVSBszLbqJu24489681 = -323674588;    float UccgastvHtVSBszLbqJu91344571 = -358475552;    float UccgastvHtVSBszLbqJu4667750 = -862182515;    float UccgastvHtVSBszLbqJu51943019 = -12287058;    float UccgastvHtVSBszLbqJu21464504 = -248262048;    float UccgastvHtVSBszLbqJu45297287 = -443962615;    float UccgastvHtVSBszLbqJu90526267 = -147731240;    float UccgastvHtVSBszLbqJu83320906 = -356532933;    float UccgastvHtVSBszLbqJu27331263 = -50966151;    float UccgastvHtVSBszLbqJu6770068 = -318490414;    float UccgastvHtVSBszLbqJu319401 = -8878808;    float UccgastvHtVSBszLbqJu77169017 = -60205562;    float UccgastvHtVSBszLbqJu49492188 = -596671006;    float UccgastvHtVSBszLbqJu28903742 = -167025595;    float UccgastvHtVSBszLbqJu40104356 = -240091277;    float UccgastvHtVSBszLbqJu64675106 = -382815331;    float UccgastvHtVSBszLbqJu85996302 = -750783987;    float UccgastvHtVSBszLbqJu8299501 = -221464968;    float UccgastvHtVSBszLbqJu92861556 = -203851045;    float UccgastvHtVSBszLbqJu47555821 = -336359182;    float UccgastvHtVSBszLbqJu62396656 = -155923090;    float UccgastvHtVSBszLbqJu5243376 = -696456104;    float UccgastvHtVSBszLbqJu6779739 = -926518497;    float UccgastvHtVSBszLbqJu68422347 = -291624748;    float UccgastvHtVSBszLbqJu98923388 = -475089187;    float UccgastvHtVSBszLbqJu37257234 = -93820389;    float UccgastvHtVSBszLbqJu69373023 = -360565000;    float UccgastvHtVSBszLbqJu54645848 = -960568261;    float UccgastvHtVSBszLbqJu68144821 = -306963260;    float UccgastvHtVSBszLbqJu75528251 = -68052292;    float UccgastvHtVSBszLbqJu68878264 = -54389791;    float UccgastvHtVSBszLbqJu36026229 = -946667661;    float UccgastvHtVSBszLbqJu17024441 = -676929841;    float UccgastvHtVSBszLbqJu85680017 = 45319931;    float UccgastvHtVSBszLbqJu91876148 = -826083887;    float UccgastvHtVSBszLbqJu88475011 = -375153196;    float UccgastvHtVSBszLbqJu9270273 = -919835248;    float UccgastvHtVSBszLbqJu40442739 = -742244340;    float UccgastvHtVSBszLbqJu94461033 = -163336047;    float UccgastvHtVSBszLbqJu2891241 = -682353211;    float UccgastvHtVSBszLbqJu49356861 = -999099747;    float UccgastvHtVSBszLbqJu95804088 = -680319091;    float UccgastvHtVSBszLbqJu76098996 = -490987411;    float UccgastvHtVSBszLbqJu7650764 = 46942777;    float UccgastvHtVSBszLbqJu68843217 = -92192920;    float UccgastvHtVSBszLbqJu21628705 = -796782472;    float UccgastvHtVSBszLbqJu22132574 = -600055428;    float UccgastvHtVSBszLbqJu49556456 = -871320208;    float UccgastvHtVSBszLbqJu41421429 = -547117331;    float UccgastvHtVSBszLbqJu23744648 = -220547935;    float UccgastvHtVSBszLbqJu96102416 = -975162;    float UccgastvHtVSBszLbqJu91317444 = -891626997;    float UccgastvHtVSBszLbqJu48467322 = -428893125;    float UccgastvHtVSBszLbqJu84474764 = -896292255;    float UccgastvHtVSBszLbqJu28547985 = -694026410;    float UccgastvHtVSBszLbqJu19246306 = -627218485;    float UccgastvHtVSBszLbqJu84564833 = -431957056;    float UccgastvHtVSBszLbqJu36245403 = -470557768;    float UccgastvHtVSBszLbqJu53019630 = -537197871;    float UccgastvHtVSBszLbqJu84207270 = -54441659;    float UccgastvHtVSBszLbqJu75924264 = 16602384;    float UccgastvHtVSBszLbqJu35880420 = -187162979;    float UccgastvHtVSBszLbqJu15176086 = 50430326;    float UccgastvHtVSBszLbqJu51803012 = -982913860;    float UccgastvHtVSBszLbqJu37891803 = -164100623;    float UccgastvHtVSBszLbqJu64293171 = -62211147;    float UccgastvHtVSBszLbqJu60144576 = -383275722;    float UccgastvHtVSBszLbqJu63812170 = -541990937;    float UccgastvHtVSBszLbqJu37027593 = -340941708;    float UccgastvHtVSBszLbqJu51629344 = -864938081;    float UccgastvHtVSBszLbqJu55404834 = -462980083;    float UccgastvHtVSBszLbqJu45553564 = 91460353;    float UccgastvHtVSBszLbqJu13838467 = -10326525;     UccgastvHtVSBszLbqJu56564576 = UccgastvHtVSBszLbqJu57626970;     UccgastvHtVSBszLbqJu57626970 = UccgastvHtVSBszLbqJu68495209;     UccgastvHtVSBszLbqJu68495209 = UccgastvHtVSBszLbqJu77355107;     UccgastvHtVSBszLbqJu77355107 = UccgastvHtVSBszLbqJu49055027;     UccgastvHtVSBszLbqJu49055027 = UccgastvHtVSBszLbqJu76624659;     UccgastvHtVSBszLbqJu76624659 = UccgastvHtVSBszLbqJu16365830;     UccgastvHtVSBszLbqJu16365830 = UccgastvHtVSBszLbqJu79819583;     UccgastvHtVSBszLbqJu79819583 = UccgastvHtVSBszLbqJu13938022;     UccgastvHtVSBszLbqJu13938022 = UccgastvHtVSBszLbqJu92385758;     UccgastvHtVSBszLbqJu92385758 = UccgastvHtVSBszLbqJu15925538;     UccgastvHtVSBszLbqJu15925538 = UccgastvHtVSBszLbqJu48188527;     UccgastvHtVSBszLbqJu48188527 = UccgastvHtVSBszLbqJu39883129;     UccgastvHtVSBszLbqJu39883129 = UccgastvHtVSBszLbqJu79124995;     UccgastvHtVSBszLbqJu79124995 = UccgastvHtVSBszLbqJu3430259;     UccgastvHtVSBszLbqJu3430259 = UccgastvHtVSBszLbqJu14420831;     UccgastvHtVSBszLbqJu14420831 = UccgastvHtVSBszLbqJu69162618;     UccgastvHtVSBszLbqJu69162618 = UccgastvHtVSBszLbqJu98797721;     UccgastvHtVSBszLbqJu98797721 = UccgastvHtVSBszLbqJu71624762;     UccgastvHtVSBszLbqJu71624762 = UccgastvHtVSBszLbqJu78460198;     UccgastvHtVSBszLbqJu78460198 = UccgastvHtVSBszLbqJu81525785;     UccgastvHtVSBszLbqJu81525785 = UccgastvHtVSBszLbqJu88419754;     UccgastvHtVSBszLbqJu88419754 = UccgastvHtVSBszLbqJu82098719;     UccgastvHtVSBszLbqJu82098719 = UccgastvHtVSBszLbqJu99616945;     UccgastvHtVSBszLbqJu99616945 = UccgastvHtVSBszLbqJu41328878;     UccgastvHtVSBszLbqJu41328878 = UccgastvHtVSBszLbqJu32030586;     UccgastvHtVSBszLbqJu32030586 = UccgastvHtVSBszLbqJu90944641;     UccgastvHtVSBszLbqJu90944641 = UccgastvHtVSBszLbqJu24489681;     UccgastvHtVSBszLbqJu24489681 = UccgastvHtVSBszLbqJu91344571;     UccgastvHtVSBszLbqJu91344571 = UccgastvHtVSBszLbqJu4667750;     UccgastvHtVSBszLbqJu4667750 = UccgastvHtVSBszLbqJu51943019;     UccgastvHtVSBszLbqJu51943019 = UccgastvHtVSBszLbqJu21464504;     UccgastvHtVSBszLbqJu21464504 = UccgastvHtVSBszLbqJu45297287;     UccgastvHtVSBszLbqJu45297287 = UccgastvHtVSBszLbqJu90526267;     UccgastvHtVSBszLbqJu90526267 = UccgastvHtVSBszLbqJu83320906;     UccgastvHtVSBszLbqJu83320906 = UccgastvHtVSBszLbqJu27331263;     UccgastvHtVSBszLbqJu27331263 = UccgastvHtVSBszLbqJu6770068;     UccgastvHtVSBszLbqJu6770068 = UccgastvHtVSBszLbqJu319401;     UccgastvHtVSBszLbqJu319401 = UccgastvHtVSBszLbqJu77169017;     UccgastvHtVSBszLbqJu77169017 = UccgastvHtVSBszLbqJu49492188;     UccgastvHtVSBszLbqJu49492188 = UccgastvHtVSBszLbqJu28903742;     UccgastvHtVSBszLbqJu28903742 = UccgastvHtVSBszLbqJu40104356;     UccgastvHtVSBszLbqJu40104356 = UccgastvHtVSBszLbqJu64675106;     UccgastvHtVSBszLbqJu64675106 = UccgastvHtVSBszLbqJu85996302;     UccgastvHtVSBszLbqJu85996302 = UccgastvHtVSBszLbqJu8299501;     UccgastvHtVSBszLbqJu8299501 = UccgastvHtVSBszLbqJu92861556;     UccgastvHtVSBszLbqJu92861556 = UccgastvHtVSBszLbqJu47555821;     UccgastvHtVSBszLbqJu47555821 = UccgastvHtVSBszLbqJu62396656;     UccgastvHtVSBszLbqJu62396656 = UccgastvHtVSBszLbqJu5243376;     UccgastvHtVSBszLbqJu5243376 = UccgastvHtVSBszLbqJu6779739;     UccgastvHtVSBszLbqJu6779739 = UccgastvHtVSBszLbqJu68422347;     UccgastvHtVSBszLbqJu68422347 = UccgastvHtVSBszLbqJu98923388;     UccgastvHtVSBszLbqJu98923388 = UccgastvHtVSBszLbqJu37257234;     UccgastvHtVSBszLbqJu37257234 = UccgastvHtVSBszLbqJu69373023;     UccgastvHtVSBszLbqJu69373023 = UccgastvHtVSBszLbqJu54645848;     UccgastvHtVSBszLbqJu54645848 = UccgastvHtVSBszLbqJu68144821;     UccgastvHtVSBszLbqJu68144821 = UccgastvHtVSBszLbqJu75528251;     UccgastvHtVSBszLbqJu75528251 = UccgastvHtVSBszLbqJu68878264;     UccgastvHtVSBszLbqJu68878264 = UccgastvHtVSBszLbqJu36026229;     UccgastvHtVSBszLbqJu36026229 = UccgastvHtVSBszLbqJu17024441;     UccgastvHtVSBszLbqJu17024441 = UccgastvHtVSBszLbqJu85680017;     UccgastvHtVSBszLbqJu85680017 = UccgastvHtVSBszLbqJu91876148;     UccgastvHtVSBszLbqJu91876148 = UccgastvHtVSBszLbqJu88475011;     UccgastvHtVSBszLbqJu88475011 = UccgastvHtVSBszLbqJu9270273;     UccgastvHtVSBszLbqJu9270273 = UccgastvHtVSBszLbqJu40442739;     UccgastvHtVSBszLbqJu40442739 = UccgastvHtVSBszLbqJu94461033;     UccgastvHtVSBszLbqJu94461033 = UccgastvHtVSBszLbqJu2891241;     UccgastvHtVSBszLbqJu2891241 = UccgastvHtVSBszLbqJu49356861;     UccgastvHtVSBszLbqJu49356861 = UccgastvHtVSBszLbqJu95804088;     UccgastvHtVSBszLbqJu95804088 = UccgastvHtVSBszLbqJu76098996;     UccgastvHtVSBszLbqJu76098996 = UccgastvHtVSBszLbqJu7650764;     UccgastvHtVSBszLbqJu7650764 = UccgastvHtVSBszLbqJu68843217;     UccgastvHtVSBszLbqJu68843217 = UccgastvHtVSBszLbqJu21628705;     UccgastvHtVSBszLbqJu21628705 = UccgastvHtVSBszLbqJu22132574;     UccgastvHtVSBszLbqJu22132574 = UccgastvHtVSBszLbqJu49556456;     UccgastvHtVSBszLbqJu49556456 = UccgastvHtVSBszLbqJu41421429;     UccgastvHtVSBszLbqJu41421429 = UccgastvHtVSBszLbqJu23744648;     UccgastvHtVSBszLbqJu23744648 = UccgastvHtVSBszLbqJu96102416;     UccgastvHtVSBszLbqJu96102416 = UccgastvHtVSBszLbqJu91317444;     UccgastvHtVSBszLbqJu91317444 = UccgastvHtVSBszLbqJu48467322;     UccgastvHtVSBszLbqJu48467322 = UccgastvHtVSBszLbqJu84474764;     UccgastvHtVSBszLbqJu84474764 = UccgastvHtVSBszLbqJu28547985;     UccgastvHtVSBszLbqJu28547985 = UccgastvHtVSBszLbqJu19246306;     UccgastvHtVSBszLbqJu19246306 = UccgastvHtVSBszLbqJu84564833;     UccgastvHtVSBszLbqJu84564833 = UccgastvHtVSBszLbqJu36245403;     UccgastvHtVSBszLbqJu36245403 = UccgastvHtVSBszLbqJu53019630;     UccgastvHtVSBszLbqJu53019630 = UccgastvHtVSBszLbqJu84207270;     UccgastvHtVSBszLbqJu84207270 = UccgastvHtVSBszLbqJu75924264;     UccgastvHtVSBszLbqJu75924264 = UccgastvHtVSBszLbqJu35880420;     UccgastvHtVSBszLbqJu35880420 = UccgastvHtVSBszLbqJu15176086;     UccgastvHtVSBszLbqJu15176086 = UccgastvHtVSBszLbqJu51803012;     UccgastvHtVSBszLbqJu51803012 = UccgastvHtVSBszLbqJu37891803;     UccgastvHtVSBszLbqJu37891803 = UccgastvHtVSBszLbqJu64293171;     UccgastvHtVSBszLbqJu64293171 = UccgastvHtVSBszLbqJu60144576;     UccgastvHtVSBszLbqJu60144576 = UccgastvHtVSBszLbqJu63812170;     UccgastvHtVSBszLbqJu63812170 = UccgastvHtVSBszLbqJu37027593;     UccgastvHtVSBszLbqJu37027593 = UccgastvHtVSBszLbqJu51629344;     UccgastvHtVSBszLbqJu51629344 = UccgastvHtVSBszLbqJu55404834;     UccgastvHtVSBszLbqJu55404834 = UccgastvHtVSBszLbqJu45553564;     UccgastvHtVSBszLbqJu45553564 = UccgastvHtVSBszLbqJu13838467;     UccgastvHtVSBszLbqJu13838467 = UccgastvHtVSBszLbqJu56564576;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HBKFfEgSrevvEjuHPzGG42830377() {     float kYMcNLgcKiupisQqFBoW56558105 = -982850666;    float kYMcNLgcKiupisQqFBoW96199286 = 37593436;    float kYMcNLgcKiupisQqFBoW69808421 = -224610369;    float kYMcNLgcKiupisQqFBoW62434968 = -499952661;    float kYMcNLgcKiupisQqFBoW57895129 = -377936598;    float kYMcNLgcKiupisQqFBoW11430092 = -704062380;    float kYMcNLgcKiupisQqFBoW84913166 = -954294941;    float kYMcNLgcKiupisQqFBoW49033634 = -537806065;    float kYMcNLgcKiupisQqFBoW96678186 = -495663832;    float kYMcNLgcKiupisQqFBoW89025271 = -491338268;    float kYMcNLgcKiupisQqFBoW36246143 = -471070167;    float kYMcNLgcKiupisQqFBoW62480204 = -961794189;    float kYMcNLgcKiupisQqFBoW71736042 = -168489461;    float kYMcNLgcKiupisQqFBoW32969216 = -382005075;    float kYMcNLgcKiupisQqFBoW48602368 = -604666413;    float kYMcNLgcKiupisQqFBoW88985378 = -704343287;    float kYMcNLgcKiupisQqFBoW25015378 = -20686296;    float kYMcNLgcKiupisQqFBoW21798255 = -254059655;    float kYMcNLgcKiupisQqFBoW92053031 = -814708700;    float kYMcNLgcKiupisQqFBoW50533961 = -650370253;    float kYMcNLgcKiupisQqFBoW61195209 = -597674981;    float kYMcNLgcKiupisQqFBoW66716020 = -113355149;    float kYMcNLgcKiupisQqFBoW18977116 = -210716990;    float kYMcNLgcKiupisQqFBoW43267053 = 5987513;    float kYMcNLgcKiupisQqFBoW69771568 = -59438023;    float kYMcNLgcKiupisQqFBoW48604663 = -140857364;    float kYMcNLgcKiupisQqFBoW61166223 = -150749212;    float kYMcNLgcKiupisQqFBoW59893206 = -932641576;    float kYMcNLgcKiupisQqFBoW4023019 = 95454133;    float kYMcNLgcKiupisQqFBoW80757619 = -537613888;    float kYMcNLgcKiupisQqFBoW85355697 = -399380292;    float kYMcNLgcKiupisQqFBoW6188362 = -640083810;    float kYMcNLgcKiupisQqFBoW30749711 = -645906831;    float kYMcNLgcKiupisQqFBoW90809104 = -24412972;    float kYMcNLgcKiupisQqFBoW65678945 = -615846858;    float kYMcNLgcKiupisQqFBoW70159452 = -130731897;    float kYMcNLgcKiupisQqFBoW18711435 = -418143792;    float kYMcNLgcKiupisQqFBoW70237080 = -4141487;    float kYMcNLgcKiupisQqFBoW12482614 = -772886978;    float kYMcNLgcKiupisQqFBoW21365081 = -206102692;    float kYMcNLgcKiupisQqFBoW16759410 = -254969493;    float kYMcNLgcKiupisQqFBoW25802017 = -433883359;    float kYMcNLgcKiupisQqFBoW24958467 = -941945585;    float kYMcNLgcKiupisQqFBoW45228201 = -307748430;    float kYMcNLgcKiupisQqFBoW92317552 = -694724370;    float kYMcNLgcKiupisQqFBoW33289525 = -15895716;    float kYMcNLgcKiupisQqFBoW75253613 = -817639804;    float kYMcNLgcKiupisQqFBoW37780237 = -764684447;    float kYMcNLgcKiupisQqFBoW23901765 = -260219965;    float kYMcNLgcKiupisQqFBoW26825162 = -513543383;    float kYMcNLgcKiupisQqFBoW81681717 = -970179755;    float kYMcNLgcKiupisQqFBoW13470646 = -857234935;    float kYMcNLgcKiupisQqFBoW12705582 = 14125602;    float kYMcNLgcKiupisQqFBoW79623293 = -892484851;    float kYMcNLgcKiupisQqFBoW81635168 = -717467539;    float kYMcNLgcKiupisQqFBoW89842084 = -769495518;    float kYMcNLgcKiupisQqFBoW77222170 = -751689575;    float kYMcNLgcKiupisQqFBoW26541368 = -130597883;    float kYMcNLgcKiupisQqFBoW92663400 = -340514638;    float kYMcNLgcKiupisQqFBoW9290466 = -137079234;    float kYMcNLgcKiupisQqFBoW50263869 = -453313168;    float kYMcNLgcKiupisQqFBoW25019960 = 78346635;    float kYMcNLgcKiupisQqFBoW45010616 = -533260199;    float kYMcNLgcKiupisQqFBoW15920567 = -958049944;    float kYMcNLgcKiupisQqFBoW3669574 = 8042023;    float kYMcNLgcKiupisQqFBoW30057782 = -830986358;    float kYMcNLgcKiupisQqFBoW31730494 = -215887358;    float kYMcNLgcKiupisQqFBoW80926937 = -44076489;    float kYMcNLgcKiupisQqFBoW67290271 = -766158218;    float kYMcNLgcKiupisQqFBoW78442915 = -373934516;    float kYMcNLgcKiupisQqFBoW70273943 = -186199495;    float kYMcNLgcKiupisQqFBoW54778297 = 83455190;    float kYMcNLgcKiupisQqFBoW9315641 = -481172677;    float kYMcNLgcKiupisQqFBoW70687951 = -508606009;    float kYMcNLgcKiupisQqFBoW33774551 = -295400761;    float kYMcNLgcKiupisQqFBoW35393193 = -63791623;    float kYMcNLgcKiupisQqFBoW41757554 = -171409564;    float kYMcNLgcKiupisQqFBoW73748915 = -902968560;    float kYMcNLgcKiupisQqFBoW50949501 = -299288118;    float kYMcNLgcKiupisQqFBoW36482044 = 56457693;    float kYMcNLgcKiupisQqFBoW73351049 = -323217561;    float kYMcNLgcKiupisQqFBoW23385986 = -386064766;    float kYMcNLgcKiupisQqFBoW35991441 = -572421612;    float kYMcNLgcKiupisQqFBoW77197857 = -391002484;    float kYMcNLgcKiupisQqFBoW99075902 = -567434134;    float kYMcNLgcKiupisQqFBoW71885051 = -542145357;    float kYMcNLgcKiupisQqFBoW93482779 = -554209412;    float kYMcNLgcKiupisQqFBoW51126417 = -753421981;    float kYMcNLgcKiupisQqFBoW9173937 = -306945434;    float kYMcNLgcKiupisQqFBoW75836860 = -846351340;    float kYMcNLgcKiupisQqFBoW92937281 = -379042323;    float kYMcNLgcKiupisQqFBoW92170067 = -187545910;    float kYMcNLgcKiupisQqFBoW77573680 = -663626849;    float kYMcNLgcKiupisQqFBoW3192148 = -535807745;    float kYMcNLgcKiupisQqFBoW71101212 = -752789525;    float kYMcNLgcKiupisQqFBoW91739450 = -233316128;    float kYMcNLgcKiupisQqFBoW80791401 = -900623161;    float kYMcNLgcKiupisQqFBoW9037901 = -983895641;    float kYMcNLgcKiupisQqFBoW41558627 = -215790454;    float kYMcNLgcKiupisQqFBoW62259771 = -982850666;     kYMcNLgcKiupisQqFBoW56558105 = kYMcNLgcKiupisQqFBoW96199286;     kYMcNLgcKiupisQqFBoW96199286 = kYMcNLgcKiupisQqFBoW69808421;     kYMcNLgcKiupisQqFBoW69808421 = kYMcNLgcKiupisQqFBoW62434968;     kYMcNLgcKiupisQqFBoW62434968 = kYMcNLgcKiupisQqFBoW57895129;     kYMcNLgcKiupisQqFBoW57895129 = kYMcNLgcKiupisQqFBoW11430092;     kYMcNLgcKiupisQqFBoW11430092 = kYMcNLgcKiupisQqFBoW84913166;     kYMcNLgcKiupisQqFBoW84913166 = kYMcNLgcKiupisQqFBoW49033634;     kYMcNLgcKiupisQqFBoW49033634 = kYMcNLgcKiupisQqFBoW96678186;     kYMcNLgcKiupisQqFBoW96678186 = kYMcNLgcKiupisQqFBoW89025271;     kYMcNLgcKiupisQqFBoW89025271 = kYMcNLgcKiupisQqFBoW36246143;     kYMcNLgcKiupisQqFBoW36246143 = kYMcNLgcKiupisQqFBoW62480204;     kYMcNLgcKiupisQqFBoW62480204 = kYMcNLgcKiupisQqFBoW71736042;     kYMcNLgcKiupisQqFBoW71736042 = kYMcNLgcKiupisQqFBoW32969216;     kYMcNLgcKiupisQqFBoW32969216 = kYMcNLgcKiupisQqFBoW48602368;     kYMcNLgcKiupisQqFBoW48602368 = kYMcNLgcKiupisQqFBoW88985378;     kYMcNLgcKiupisQqFBoW88985378 = kYMcNLgcKiupisQqFBoW25015378;     kYMcNLgcKiupisQqFBoW25015378 = kYMcNLgcKiupisQqFBoW21798255;     kYMcNLgcKiupisQqFBoW21798255 = kYMcNLgcKiupisQqFBoW92053031;     kYMcNLgcKiupisQqFBoW92053031 = kYMcNLgcKiupisQqFBoW50533961;     kYMcNLgcKiupisQqFBoW50533961 = kYMcNLgcKiupisQqFBoW61195209;     kYMcNLgcKiupisQqFBoW61195209 = kYMcNLgcKiupisQqFBoW66716020;     kYMcNLgcKiupisQqFBoW66716020 = kYMcNLgcKiupisQqFBoW18977116;     kYMcNLgcKiupisQqFBoW18977116 = kYMcNLgcKiupisQqFBoW43267053;     kYMcNLgcKiupisQqFBoW43267053 = kYMcNLgcKiupisQqFBoW69771568;     kYMcNLgcKiupisQqFBoW69771568 = kYMcNLgcKiupisQqFBoW48604663;     kYMcNLgcKiupisQqFBoW48604663 = kYMcNLgcKiupisQqFBoW61166223;     kYMcNLgcKiupisQqFBoW61166223 = kYMcNLgcKiupisQqFBoW59893206;     kYMcNLgcKiupisQqFBoW59893206 = kYMcNLgcKiupisQqFBoW4023019;     kYMcNLgcKiupisQqFBoW4023019 = kYMcNLgcKiupisQqFBoW80757619;     kYMcNLgcKiupisQqFBoW80757619 = kYMcNLgcKiupisQqFBoW85355697;     kYMcNLgcKiupisQqFBoW85355697 = kYMcNLgcKiupisQqFBoW6188362;     kYMcNLgcKiupisQqFBoW6188362 = kYMcNLgcKiupisQqFBoW30749711;     kYMcNLgcKiupisQqFBoW30749711 = kYMcNLgcKiupisQqFBoW90809104;     kYMcNLgcKiupisQqFBoW90809104 = kYMcNLgcKiupisQqFBoW65678945;     kYMcNLgcKiupisQqFBoW65678945 = kYMcNLgcKiupisQqFBoW70159452;     kYMcNLgcKiupisQqFBoW70159452 = kYMcNLgcKiupisQqFBoW18711435;     kYMcNLgcKiupisQqFBoW18711435 = kYMcNLgcKiupisQqFBoW70237080;     kYMcNLgcKiupisQqFBoW70237080 = kYMcNLgcKiupisQqFBoW12482614;     kYMcNLgcKiupisQqFBoW12482614 = kYMcNLgcKiupisQqFBoW21365081;     kYMcNLgcKiupisQqFBoW21365081 = kYMcNLgcKiupisQqFBoW16759410;     kYMcNLgcKiupisQqFBoW16759410 = kYMcNLgcKiupisQqFBoW25802017;     kYMcNLgcKiupisQqFBoW25802017 = kYMcNLgcKiupisQqFBoW24958467;     kYMcNLgcKiupisQqFBoW24958467 = kYMcNLgcKiupisQqFBoW45228201;     kYMcNLgcKiupisQqFBoW45228201 = kYMcNLgcKiupisQqFBoW92317552;     kYMcNLgcKiupisQqFBoW92317552 = kYMcNLgcKiupisQqFBoW33289525;     kYMcNLgcKiupisQqFBoW33289525 = kYMcNLgcKiupisQqFBoW75253613;     kYMcNLgcKiupisQqFBoW75253613 = kYMcNLgcKiupisQqFBoW37780237;     kYMcNLgcKiupisQqFBoW37780237 = kYMcNLgcKiupisQqFBoW23901765;     kYMcNLgcKiupisQqFBoW23901765 = kYMcNLgcKiupisQqFBoW26825162;     kYMcNLgcKiupisQqFBoW26825162 = kYMcNLgcKiupisQqFBoW81681717;     kYMcNLgcKiupisQqFBoW81681717 = kYMcNLgcKiupisQqFBoW13470646;     kYMcNLgcKiupisQqFBoW13470646 = kYMcNLgcKiupisQqFBoW12705582;     kYMcNLgcKiupisQqFBoW12705582 = kYMcNLgcKiupisQqFBoW79623293;     kYMcNLgcKiupisQqFBoW79623293 = kYMcNLgcKiupisQqFBoW81635168;     kYMcNLgcKiupisQqFBoW81635168 = kYMcNLgcKiupisQqFBoW89842084;     kYMcNLgcKiupisQqFBoW89842084 = kYMcNLgcKiupisQqFBoW77222170;     kYMcNLgcKiupisQqFBoW77222170 = kYMcNLgcKiupisQqFBoW26541368;     kYMcNLgcKiupisQqFBoW26541368 = kYMcNLgcKiupisQqFBoW92663400;     kYMcNLgcKiupisQqFBoW92663400 = kYMcNLgcKiupisQqFBoW9290466;     kYMcNLgcKiupisQqFBoW9290466 = kYMcNLgcKiupisQqFBoW50263869;     kYMcNLgcKiupisQqFBoW50263869 = kYMcNLgcKiupisQqFBoW25019960;     kYMcNLgcKiupisQqFBoW25019960 = kYMcNLgcKiupisQqFBoW45010616;     kYMcNLgcKiupisQqFBoW45010616 = kYMcNLgcKiupisQqFBoW15920567;     kYMcNLgcKiupisQqFBoW15920567 = kYMcNLgcKiupisQqFBoW3669574;     kYMcNLgcKiupisQqFBoW3669574 = kYMcNLgcKiupisQqFBoW30057782;     kYMcNLgcKiupisQqFBoW30057782 = kYMcNLgcKiupisQqFBoW31730494;     kYMcNLgcKiupisQqFBoW31730494 = kYMcNLgcKiupisQqFBoW80926937;     kYMcNLgcKiupisQqFBoW80926937 = kYMcNLgcKiupisQqFBoW67290271;     kYMcNLgcKiupisQqFBoW67290271 = kYMcNLgcKiupisQqFBoW78442915;     kYMcNLgcKiupisQqFBoW78442915 = kYMcNLgcKiupisQqFBoW70273943;     kYMcNLgcKiupisQqFBoW70273943 = kYMcNLgcKiupisQqFBoW54778297;     kYMcNLgcKiupisQqFBoW54778297 = kYMcNLgcKiupisQqFBoW9315641;     kYMcNLgcKiupisQqFBoW9315641 = kYMcNLgcKiupisQqFBoW70687951;     kYMcNLgcKiupisQqFBoW70687951 = kYMcNLgcKiupisQqFBoW33774551;     kYMcNLgcKiupisQqFBoW33774551 = kYMcNLgcKiupisQqFBoW35393193;     kYMcNLgcKiupisQqFBoW35393193 = kYMcNLgcKiupisQqFBoW41757554;     kYMcNLgcKiupisQqFBoW41757554 = kYMcNLgcKiupisQqFBoW73748915;     kYMcNLgcKiupisQqFBoW73748915 = kYMcNLgcKiupisQqFBoW50949501;     kYMcNLgcKiupisQqFBoW50949501 = kYMcNLgcKiupisQqFBoW36482044;     kYMcNLgcKiupisQqFBoW36482044 = kYMcNLgcKiupisQqFBoW73351049;     kYMcNLgcKiupisQqFBoW73351049 = kYMcNLgcKiupisQqFBoW23385986;     kYMcNLgcKiupisQqFBoW23385986 = kYMcNLgcKiupisQqFBoW35991441;     kYMcNLgcKiupisQqFBoW35991441 = kYMcNLgcKiupisQqFBoW77197857;     kYMcNLgcKiupisQqFBoW77197857 = kYMcNLgcKiupisQqFBoW99075902;     kYMcNLgcKiupisQqFBoW99075902 = kYMcNLgcKiupisQqFBoW71885051;     kYMcNLgcKiupisQqFBoW71885051 = kYMcNLgcKiupisQqFBoW93482779;     kYMcNLgcKiupisQqFBoW93482779 = kYMcNLgcKiupisQqFBoW51126417;     kYMcNLgcKiupisQqFBoW51126417 = kYMcNLgcKiupisQqFBoW9173937;     kYMcNLgcKiupisQqFBoW9173937 = kYMcNLgcKiupisQqFBoW75836860;     kYMcNLgcKiupisQqFBoW75836860 = kYMcNLgcKiupisQqFBoW92937281;     kYMcNLgcKiupisQqFBoW92937281 = kYMcNLgcKiupisQqFBoW92170067;     kYMcNLgcKiupisQqFBoW92170067 = kYMcNLgcKiupisQqFBoW77573680;     kYMcNLgcKiupisQqFBoW77573680 = kYMcNLgcKiupisQqFBoW3192148;     kYMcNLgcKiupisQqFBoW3192148 = kYMcNLgcKiupisQqFBoW71101212;     kYMcNLgcKiupisQqFBoW71101212 = kYMcNLgcKiupisQqFBoW91739450;     kYMcNLgcKiupisQqFBoW91739450 = kYMcNLgcKiupisQqFBoW80791401;     kYMcNLgcKiupisQqFBoW80791401 = kYMcNLgcKiupisQqFBoW9037901;     kYMcNLgcKiupisQqFBoW9037901 = kYMcNLgcKiupisQqFBoW41558627;     kYMcNLgcKiupisQqFBoW41558627 = kYMcNLgcKiupisQqFBoW62259771;     kYMcNLgcKiupisQqFBoW62259771 = kYMcNLgcKiupisQqFBoW56558105;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void AHjbiXEcwnAUCwxQveBu97246590() {     float qpoIeYHZkPNMDplOjjXT27209772 = -500999796;    float qpoIeYHZkPNMDplOjjXT64872804 = -934049864;    float qpoIeYHZkPNMDplOjjXT49777340 = -666904829;    float qpoIeYHZkPNMDplOjjXT2366255 = -690091050;    float qpoIeYHZkPNMDplOjjXT79345450 = -161342945;    float qpoIeYHZkPNMDplOjjXT1429854 = -405643531;    float qpoIeYHZkPNMDplOjjXT3292062 = -953070651;    float qpoIeYHZkPNMDplOjjXT5876015 = -884623155;    float qpoIeYHZkPNMDplOjjXT42543240 = -589011161;    float qpoIeYHZkPNMDplOjjXT11991733 = -792433619;    float qpoIeYHZkPNMDplOjjXT78495303 = -505972096;    float qpoIeYHZkPNMDplOjjXT8026776 = -177557982;    float qpoIeYHZkPNMDplOjjXT51563429 = -78026519;    float qpoIeYHZkPNMDplOjjXT16816249 = -183033285;    float qpoIeYHZkPNMDplOjjXT31322387 = -754416416;    float qpoIeYHZkPNMDplOjjXT21514019 = -274326873;    float qpoIeYHZkPNMDplOjjXT18868204 = 70128574;    float qpoIeYHZkPNMDplOjjXT42091749 = -243225851;    float qpoIeYHZkPNMDplOjjXT73760813 = -421896643;    float qpoIeYHZkPNMDplOjjXT9320670 = -92736558;    float qpoIeYHZkPNMDplOjjXT16330964 = -506354121;    float qpoIeYHZkPNMDplOjjXT28111786 = -444640454;    float qpoIeYHZkPNMDplOjjXT20920925 = -778560135;    float qpoIeYHZkPNMDplOjjXT77116590 = -686413393;    float qpoIeYHZkPNMDplOjjXT33895957 = -835342711;    float qpoIeYHZkPNMDplOjjXT79202243 = -239141809;    float qpoIeYHZkPNMDplOjjXT69427008 = -545926988;    float qpoIeYHZkPNMDplOjjXT67675431 = -337765344;    float qpoIeYHZkPNMDplOjjXT36165506 = -198904084;    float qpoIeYHZkPNMDplOjjXT19929716 = 3991041;    float qpoIeYHZkPNMDplOjjXT92427694 = -147813978;    float qpoIeYHZkPNMDplOjjXT50699822 = -968380972;    float qpoIeYHZkPNMDplOjjXT13695072 = -592868440;    float qpoIeYHZkPNMDplOjjXT56174080 = -687942656;    float qpoIeYHZkPNMDplOjjXT43703181 = -462793637;    float qpoIeYHZkPNMDplOjjXT30152711 = -209958445;    float qpoIeYHZkPNMDplOjjXT62502921 = -606737000;    float qpoIeYHZkPNMDplOjjXT23029569 = -888254594;    float qpoIeYHZkPNMDplOjjXT91015902 = -728615615;    float qpoIeYHZkPNMDplOjjXT16468379 = -823711055;    float qpoIeYHZkPNMDplOjjXT76832693 = -638988371;    float qpoIeYHZkPNMDplOjjXT54285008 = -203272428;    float qpoIeYHZkPNMDplOjjXT73925426 = -928699637;    float qpoIeYHZkPNMDplOjjXT51736501 = -379723402;    float qpoIeYHZkPNMDplOjjXT88271611 = 96629236;    float qpoIeYHZkPNMDplOjjXT39695360 = -874232871;    float qpoIeYHZkPNMDplOjjXT1384817 = -933709603;    float qpoIeYHZkPNMDplOjjXT3394448 = -116814742;    float qpoIeYHZkPNMDplOjjXT29343217 = -245983934;    float qpoIeYHZkPNMDplOjjXT35252648 = -796552262;    float qpoIeYHZkPNMDplOjjXT49871042 = -516751876;    float qpoIeYHZkPNMDplOjjXT55342151 = -826454657;    float qpoIeYHZkPNMDplOjjXT67781239 = 86327560;    float qpoIeYHZkPNMDplOjjXT69850598 = -453366348;    float qpoIeYHZkPNMDplOjjXT80915627 = -828408453;    float qpoIeYHZkPNMDplOjjXT99097986 = 43640657;    float qpoIeYHZkPNMDplOjjXT43951880 = -55489730;    float qpoIeYHZkPNMDplOjjXT72660750 = -980491436;    float qpoIeYHZkPNMDplOjjXT68470298 = -854748339;    float qpoIeYHZkPNMDplOjjXT143207 = -922201136;    float qpoIeYHZkPNMDplOjjXT32002846 = -859716544;    float qpoIeYHZkPNMDplOjjXT35616630 = -515305308;    float qpoIeYHZkPNMDplOjjXT69710508 = -585719072;    float qpoIeYHZkPNMDplOjjXT22613525 = -493002202;    float qpoIeYHZkPNMDplOjjXT19564039 = -544619642;    float qpoIeYHZkPNMDplOjjXT27795482 = -537591124;    float qpoIeYHZkPNMDplOjjXT94331704 = -584689542;    float qpoIeYHZkPNMDplOjjXT95389349 = -390083863;    float qpoIeYHZkPNMDplOjjXT73113068 = -720239648;    float qpoIeYHZkPNMDplOjjXT1169677 = -444457972;    float qpoIeYHZkPNMDplOjjXT59011097 = -667589874;    float qpoIeYHZkPNMDplOjjXT95838635 = -41616833;    float qpoIeYHZkPNMDplOjjXT51075846 = -514610237;    float qpoIeYHZkPNMDplOjjXT57292435 = -598185588;    float qpoIeYHZkPNMDplOjjXT32487977 = -453748187;    float qpoIeYHZkPNMDplOjjXT62045956 = -203081694;    float qpoIeYHZkPNMDplOjjXT54186359 = -515940818;    float qpoIeYHZkPNMDplOjjXT69184423 = -298836733;    float qpoIeYHZkPNMDplOjjXT88844979 = -683042630;    float qpoIeYHZkPNMDplOjjXT94200597 = -961109840;    float qpoIeYHZkPNMDplOjjXT77817426 = -305432206;    float qpoIeYHZkPNMDplOjjXT66032561 = -329112246;    float qpoIeYHZkPNMDplOjjXT38332214 = 8218590;    float qpoIeYHZkPNMDplOjjXT912859 = -402351823;    float qpoIeYHZkPNMDplOjjXT70058674 = -479257084;    float qpoIeYHZkPNMDplOjjXT37085544 = -321359321;    float qpoIeYHZkPNMDplOjjXT82918583 = -954708533;    float qpoIeYHZkPNMDplOjjXT43844473 = -39502093;    float qpoIeYHZkPNMDplOjjXT75258453 = -859534204;    float qpoIeYHZkPNMDplOjjXT44605195 = -406434295;    float qpoIeYHZkPNMDplOjjXT86200831 = -54468716;    float qpoIeYHZkPNMDplOjjXT89842170 = -626245564;    float qpoIeYHZkPNMDplOjjXT54559270 = 66493745;    float qpoIeYHZkPNMDplOjjXT90872695 = -806414479;    float qpoIeYHZkPNMDplOjjXT84465533 = -963994511;    float qpoIeYHZkPNMDplOjjXT41216064 = -23683064;    float qpoIeYHZkPNMDplOjjXT84574499 = -617553357;    float qpoIeYHZkPNMDplOjjXT51311902 = -335697435;    float qpoIeYHZkPNMDplOjjXT32172463 = -835103760;    float qpoIeYHZkPNMDplOjjXT60476130 = -500999796;     qpoIeYHZkPNMDplOjjXT27209772 = qpoIeYHZkPNMDplOjjXT64872804;     qpoIeYHZkPNMDplOjjXT64872804 = qpoIeYHZkPNMDplOjjXT49777340;     qpoIeYHZkPNMDplOjjXT49777340 = qpoIeYHZkPNMDplOjjXT2366255;     qpoIeYHZkPNMDplOjjXT2366255 = qpoIeYHZkPNMDplOjjXT79345450;     qpoIeYHZkPNMDplOjjXT79345450 = qpoIeYHZkPNMDplOjjXT1429854;     qpoIeYHZkPNMDplOjjXT1429854 = qpoIeYHZkPNMDplOjjXT3292062;     qpoIeYHZkPNMDplOjjXT3292062 = qpoIeYHZkPNMDplOjjXT5876015;     qpoIeYHZkPNMDplOjjXT5876015 = qpoIeYHZkPNMDplOjjXT42543240;     qpoIeYHZkPNMDplOjjXT42543240 = qpoIeYHZkPNMDplOjjXT11991733;     qpoIeYHZkPNMDplOjjXT11991733 = qpoIeYHZkPNMDplOjjXT78495303;     qpoIeYHZkPNMDplOjjXT78495303 = qpoIeYHZkPNMDplOjjXT8026776;     qpoIeYHZkPNMDplOjjXT8026776 = qpoIeYHZkPNMDplOjjXT51563429;     qpoIeYHZkPNMDplOjjXT51563429 = qpoIeYHZkPNMDplOjjXT16816249;     qpoIeYHZkPNMDplOjjXT16816249 = qpoIeYHZkPNMDplOjjXT31322387;     qpoIeYHZkPNMDplOjjXT31322387 = qpoIeYHZkPNMDplOjjXT21514019;     qpoIeYHZkPNMDplOjjXT21514019 = qpoIeYHZkPNMDplOjjXT18868204;     qpoIeYHZkPNMDplOjjXT18868204 = qpoIeYHZkPNMDplOjjXT42091749;     qpoIeYHZkPNMDplOjjXT42091749 = qpoIeYHZkPNMDplOjjXT73760813;     qpoIeYHZkPNMDplOjjXT73760813 = qpoIeYHZkPNMDplOjjXT9320670;     qpoIeYHZkPNMDplOjjXT9320670 = qpoIeYHZkPNMDplOjjXT16330964;     qpoIeYHZkPNMDplOjjXT16330964 = qpoIeYHZkPNMDplOjjXT28111786;     qpoIeYHZkPNMDplOjjXT28111786 = qpoIeYHZkPNMDplOjjXT20920925;     qpoIeYHZkPNMDplOjjXT20920925 = qpoIeYHZkPNMDplOjjXT77116590;     qpoIeYHZkPNMDplOjjXT77116590 = qpoIeYHZkPNMDplOjjXT33895957;     qpoIeYHZkPNMDplOjjXT33895957 = qpoIeYHZkPNMDplOjjXT79202243;     qpoIeYHZkPNMDplOjjXT79202243 = qpoIeYHZkPNMDplOjjXT69427008;     qpoIeYHZkPNMDplOjjXT69427008 = qpoIeYHZkPNMDplOjjXT67675431;     qpoIeYHZkPNMDplOjjXT67675431 = qpoIeYHZkPNMDplOjjXT36165506;     qpoIeYHZkPNMDplOjjXT36165506 = qpoIeYHZkPNMDplOjjXT19929716;     qpoIeYHZkPNMDplOjjXT19929716 = qpoIeYHZkPNMDplOjjXT92427694;     qpoIeYHZkPNMDplOjjXT92427694 = qpoIeYHZkPNMDplOjjXT50699822;     qpoIeYHZkPNMDplOjjXT50699822 = qpoIeYHZkPNMDplOjjXT13695072;     qpoIeYHZkPNMDplOjjXT13695072 = qpoIeYHZkPNMDplOjjXT56174080;     qpoIeYHZkPNMDplOjjXT56174080 = qpoIeYHZkPNMDplOjjXT43703181;     qpoIeYHZkPNMDplOjjXT43703181 = qpoIeYHZkPNMDplOjjXT30152711;     qpoIeYHZkPNMDplOjjXT30152711 = qpoIeYHZkPNMDplOjjXT62502921;     qpoIeYHZkPNMDplOjjXT62502921 = qpoIeYHZkPNMDplOjjXT23029569;     qpoIeYHZkPNMDplOjjXT23029569 = qpoIeYHZkPNMDplOjjXT91015902;     qpoIeYHZkPNMDplOjjXT91015902 = qpoIeYHZkPNMDplOjjXT16468379;     qpoIeYHZkPNMDplOjjXT16468379 = qpoIeYHZkPNMDplOjjXT76832693;     qpoIeYHZkPNMDplOjjXT76832693 = qpoIeYHZkPNMDplOjjXT54285008;     qpoIeYHZkPNMDplOjjXT54285008 = qpoIeYHZkPNMDplOjjXT73925426;     qpoIeYHZkPNMDplOjjXT73925426 = qpoIeYHZkPNMDplOjjXT51736501;     qpoIeYHZkPNMDplOjjXT51736501 = qpoIeYHZkPNMDplOjjXT88271611;     qpoIeYHZkPNMDplOjjXT88271611 = qpoIeYHZkPNMDplOjjXT39695360;     qpoIeYHZkPNMDplOjjXT39695360 = qpoIeYHZkPNMDplOjjXT1384817;     qpoIeYHZkPNMDplOjjXT1384817 = qpoIeYHZkPNMDplOjjXT3394448;     qpoIeYHZkPNMDplOjjXT3394448 = qpoIeYHZkPNMDplOjjXT29343217;     qpoIeYHZkPNMDplOjjXT29343217 = qpoIeYHZkPNMDplOjjXT35252648;     qpoIeYHZkPNMDplOjjXT35252648 = qpoIeYHZkPNMDplOjjXT49871042;     qpoIeYHZkPNMDplOjjXT49871042 = qpoIeYHZkPNMDplOjjXT55342151;     qpoIeYHZkPNMDplOjjXT55342151 = qpoIeYHZkPNMDplOjjXT67781239;     qpoIeYHZkPNMDplOjjXT67781239 = qpoIeYHZkPNMDplOjjXT69850598;     qpoIeYHZkPNMDplOjjXT69850598 = qpoIeYHZkPNMDplOjjXT80915627;     qpoIeYHZkPNMDplOjjXT80915627 = qpoIeYHZkPNMDplOjjXT99097986;     qpoIeYHZkPNMDplOjjXT99097986 = qpoIeYHZkPNMDplOjjXT43951880;     qpoIeYHZkPNMDplOjjXT43951880 = qpoIeYHZkPNMDplOjjXT72660750;     qpoIeYHZkPNMDplOjjXT72660750 = qpoIeYHZkPNMDplOjjXT68470298;     qpoIeYHZkPNMDplOjjXT68470298 = qpoIeYHZkPNMDplOjjXT143207;     qpoIeYHZkPNMDplOjjXT143207 = qpoIeYHZkPNMDplOjjXT32002846;     qpoIeYHZkPNMDplOjjXT32002846 = qpoIeYHZkPNMDplOjjXT35616630;     qpoIeYHZkPNMDplOjjXT35616630 = qpoIeYHZkPNMDplOjjXT69710508;     qpoIeYHZkPNMDplOjjXT69710508 = qpoIeYHZkPNMDplOjjXT22613525;     qpoIeYHZkPNMDplOjjXT22613525 = qpoIeYHZkPNMDplOjjXT19564039;     qpoIeYHZkPNMDplOjjXT19564039 = qpoIeYHZkPNMDplOjjXT27795482;     qpoIeYHZkPNMDplOjjXT27795482 = qpoIeYHZkPNMDplOjjXT94331704;     qpoIeYHZkPNMDplOjjXT94331704 = qpoIeYHZkPNMDplOjjXT95389349;     qpoIeYHZkPNMDplOjjXT95389349 = qpoIeYHZkPNMDplOjjXT73113068;     qpoIeYHZkPNMDplOjjXT73113068 = qpoIeYHZkPNMDplOjjXT1169677;     qpoIeYHZkPNMDplOjjXT1169677 = qpoIeYHZkPNMDplOjjXT59011097;     qpoIeYHZkPNMDplOjjXT59011097 = qpoIeYHZkPNMDplOjjXT95838635;     qpoIeYHZkPNMDplOjjXT95838635 = qpoIeYHZkPNMDplOjjXT51075846;     qpoIeYHZkPNMDplOjjXT51075846 = qpoIeYHZkPNMDplOjjXT57292435;     qpoIeYHZkPNMDplOjjXT57292435 = qpoIeYHZkPNMDplOjjXT32487977;     qpoIeYHZkPNMDplOjjXT32487977 = qpoIeYHZkPNMDplOjjXT62045956;     qpoIeYHZkPNMDplOjjXT62045956 = qpoIeYHZkPNMDplOjjXT54186359;     qpoIeYHZkPNMDplOjjXT54186359 = qpoIeYHZkPNMDplOjjXT69184423;     qpoIeYHZkPNMDplOjjXT69184423 = qpoIeYHZkPNMDplOjjXT88844979;     qpoIeYHZkPNMDplOjjXT88844979 = qpoIeYHZkPNMDplOjjXT94200597;     qpoIeYHZkPNMDplOjjXT94200597 = qpoIeYHZkPNMDplOjjXT77817426;     qpoIeYHZkPNMDplOjjXT77817426 = qpoIeYHZkPNMDplOjjXT66032561;     qpoIeYHZkPNMDplOjjXT66032561 = qpoIeYHZkPNMDplOjjXT38332214;     qpoIeYHZkPNMDplOjjXT38332214 = qpoIeYHZkPNMDplOjjXT912859;     qpoIeYHZkPNMDplOjjXT912859 = qpoIeYHZkPNMDplOjjXT70058674;     qpoIeYHZkPNMDplOjjXT70058674 = qpoIeYHZkPNMDplOjjXT37085544;     qpoIeYHZkPNMDplOjjXT37085544 = qpoIeYHZkPNMDplOjjXT82918583;     qpoIeYHZkPNMDplOjjXT82918583 = qpoIeYHZkPNMDplOjjXT43844473;     qpoIeYHZkPNMDplOjjXT43844473 = qpoIeYHZkPNMDplOjjXT75258453;     qpoIeYHZkPNMDplOjjXT75258453 = qpoIeYHZkPNMDplOjjXT44605195;     qpoIeYHZkPNMDplOjjXT44605195 = qpoIeYHZkPNMDplOjjXT86200831;     qpoIeYHZkPNMDplOjjXT86200831 = qpoIeYHZkPNMDplOjjXT89842170;     qpoIeYHZkPNMDplOjjXT89842170 = qpoIeYHZkPNMDplOjjXT54559270;     qpoIeYHZkPNMDplOjjXT54559270 = qpoIeYHZkPNMDplOjjXT90872695;     qpoIeYHZkPNMDplOjjXT90872695 = qpoIeYHZkPNMDplOjjXT84465533;     qpoIeYHZkPNMDplOjjXT84465533 = qpoIeYHZkPNMDplOjjXT41216064;     qpoIeYHZkPNMDplOjjXT41216064 = qpoIeYHZkPNMDplOjjXT84574499;     qpoIeYHZkPNMDplOjjXT84574499 = qpoIeYHZkPNMDplOjjXT51311902;     qpoIeYHZkPNMDplOjjXT51311902 = qpoIeYHZkPNMDplOjjXT32172463;     qpoIeYHZkPNMDplOjjXT32172463 = qpoIeYHZkPNMDplOjjXT60476130;     qpoIeYHZkPNMDplOjjXT60476130 = qpoIeYHZkPNMDplOjjXT27209772;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void rFpyoWJThgirZSTsEdmN3905336() {     float lGjfcoZGTwdYMIUsfofk27203301 = -373523938;    float lGjfcoZGTwdYMIUsfofk3445121 = -976644989;    float lGjfcoZGTwdYMIUsfofk51090553 = -624033443;    float lGjfcoZGTwdYMIUsfofk87446115 = -510631880;    float lGjfcoZGTwdYMIUsfofk88185552 = -629698267;    float lGjfcoZGTwdYMIUsfofk36235286 = -105076343;    float lGjfcoZGTwdYMIUsfofk71839397 = -657607118;    float lGjfcoZGTwdYMIUsfofk75090065 = -588800472;    float lGjfcoZGTwdYMIUsfofk25283405 = -302657231;    float lGjfcoZGTwdYMIUsfofk8631246 = -429240490;    float lGjfcoZGTwdYMIUsfofk98815909 = -465444168;    float lGjfcoZGTwdYMIUsfofk22318453 = 86963656;    float lGjfcoZGTwdYMIUsfofk83416343 = -99684993;    float lGjfcoZGTwdYMIUsfofk70660469 = -528186336;    float lGjfcoZGTwdYMIUsfofk76494496 = -717129268;    float lGjfcoZGTwdYMIUsfofk96078565 = -607122524;    float lGjfcoZGTwdYMIUsfofk74720963 = -849485996;    float lGjfcoZGTwdYMIUsfofk65092281 = -640297473;    float lGjfcoZGTwdYMIUsfofk94189083 = 60121091;    float lGjfcoZGTwdYMIUsfofk81394433 = -704761010;    float lGjfcoZGTwdYMIUsfofk96000387 = -216820495;    float lGjfcoZGTwdYMIUsfofk6408052 = -954632337;    float lGjfcoZGTwdYMIUsfofk57799321 = -137517977;    float lGjfcoZGTwdYMIUsfofk20766699 = -567333916;    float lGjfcoZGTwdYMIUsfofk62338646 = -162036564;    float lGjfcoZGTwdYMIUsfofk95776320 = -147347738;    float lGjfcoZGTwdYMIUsfofk39648590 = -846726700;    float lGjfcoZGTwdYMIUsfofk3078957 = -946732332;    float lGjfcoZGTwdYMIUsfofk48843953 = -844974399;    float lGjfcoZGTwdYMIUsfofk96019585 = -771440333;    float lGjfcoZGTwdYMIUsfofk25840373 = -534907212;    float lGjfcoZGTwdYMIUsfofk35423680 = -260202734;    float lGjfcoZGTwdYMIUsfofk99147494 = -794812656;    float lGjfcoZGTwdYMIUsfofk56456917 = -564624389;    float lGjfcoZGTwdYMIUsfofk26061219 = -722107562;    float lGjfcoZGTwdYMIUsfofk72980900 = -289724191;    float lGjfcoZGTwdYMIUsfofk74444288 = -706390378;    float lGjfcoZGTwdYMIUsfofk92947248 = -883517273;    float lGjfcoZGTwdYMIUsfofk26329499 = -341297031;    float lGjfcoZGTwdYMIUsfofk88341271 = -433142741;    float lGjfcoZGTwdYMIUsfofk64688361 = -726932270;    float lGjfcoZGTwdYMIUsfofk39982669 = -397064510;    float lGjfcoZGTwdYMIUsfofk34208787 = -387829891;    float lGjfcoZGTwdYMIUsfofk10968400 = 63312155;    float lGjfcoZGTwdYMIUsfofk72289663 = -376630166;    float lGjfcoZGTwdYMIUsfofk80123327 = -686277542;    float lGjfcoZGTwdYMIUsfofk29082609 = -314990226;    float lGjfcoZGTwdYMIUsfofk78778028 = -725576098;    float lGjfcoZGTwdYMIUsfofk48001607 = -909747795;    float lGjfcoZGTwdYMIUsfofk55298071 = -383577148;    float lGjfcoZGTwdYMIUsfofk63130411 = -95306883;    float lGjfcoZGTwdYMIUsfofk69889408 = -108600405;    float lGjfcoZGTwdYMIUsfofk43229587 = -905726448;    float lGjfcoZGTwdYMIUsfofk80100869 = -985286199;    float lGjfcoZGTwdYMIUsfofk7904948 = -585307731;    float lGjfcoZGTwdYMIUsfofk20795250 = -418891601;    float lGjfcoZGTwdYMIUsfofk45645799 = -739127013;    float lGjfcoZGTwdYMIUsfofk30323854 = 43300473;    float lGjfcoZGTwdYMIUsfofk25107470 = -248595317;    float lGjfcoZGTwdYMIUsfofk92409232 = -382350530;    float lGjfcoZGTwdYMIUsfofk96586696 = -258349643;    float lGjfcoZGTwdYMIUsfofk68760441 = -710874786;    float lGjfcoZGTwdYMIUsfofk26246112 = -743826074;    float lGjfcoZGTwdYMIUsfofk29263819 = -531216898;    float lGjfcoZGTwdYMIUsfofk82790873 = -894333278;    float lGjfcoZGTwdYMIUsfofk63392229 = -105241435;    float lGjfcoZGTwdYMIUsfofk23170959 = -118223689;    float lGjfcoZGTwdYMIUsfofk26959427 = -535060605;    float lGjfcoZGTwdYMIUsfofk44599250 = -806078775;    float lGjfcoZGTwdYMIUsfofk3513596 = -327405077;    float lGjfcoZGTwdYMIUsfofk21634277 = -900732147;    float lGjfcoZGTwdYMIUsfofk81773715 = -965968724;    float lGjfcoZGTwdYMIUsfofk38762782 = -199000442;    float lGjfcoZGTwdYMIUsfofk5847812 = -506736169;    float lGjfcoZGTwdYMIUsfofk16706072 = -977828740;    float lGjfcoZGTwdYMIUsfofk56017719 = -819755985;    float lGjfcoZGTwdYMIUsfofk72199264 = -466802447;    float lGjfcoZGTwdYMIUsfofk46830922 = -100830132;    float lGjfcoZGTwdYMIUsfofk48477035 = -90703750;    float lGjfcoZGTwdYMIUsfofk82215318 = -475759023;    float lGjfcoZGTwdYMIUsfofk66693711 = -832357513;    float lGjfcoZGTwdYMIUsfofk60870562 = -21150603;    float lGjfcoZGTwdYMIUsfofk55077350 = 63015463;    float lGjfcoZGTwdYMIUsfofk93545882 = -361397251;    float lGjfcoZGTwdYMIUsfofk32889174 = -576133450;    float lGjfcoZGTwdYMIUsfofk55950965 = -326306807;    float lGjfcoZGTwdYMIUsfofk92194092 = -354476287;    float lGjfcoZGTwdYMIUsfofk19046626 = -809526458;    float lGjfcoZGTwdYMIUsfofk48551970 = -979316659;    float lGjfcoZGTwdYMIUsfofk5265970 = -203215961;    float lGjfcoZGTwdYMIUsfofk27335102 = -550597179;    float lGjfcoZGTwdYMIUsfofk44120435 = -649690851;    float lGjfcoZGTwdYMIUsfofk67839779 = -534921957;    float lGjfcoZGTwdYMIUsfofk33920267 = -958946502;    float lGjfcoZGTwdYMIUsfofk91754575 = -74793098;    float lGjfcoZGTwdYMIUsfofk95927920 = 83942516;    float lGjfcoZGTwdYMIUsfofk13736557 = -653238437;    float lGjfcoZGTwdYMIUsfofk4944968 = -856612993;    float lGjfcoZGTwdYMIUsfofk28177527 = -42354567;    float lGjfcoZGTwdYMIUsfofk8897434 = -373523938;     lGjfcoZGTwdYMIUsfofk27203301 = lGjfcoZGTwdYMIUsfofk3445121;     lGjfcoZGTwdYMIUsfofk3445121 = lGjfcoZGTwdYMIUsfofk51090553;     lGjfcoZGTwdYMIUsfofk51090553 = lGjfcoZGTwdYMIUsfofk87446115;     lGjfcoZGTwdYMIUsfofk87446115 = lGjfcoZGTwdYMIUsfofk88185552;     lGjfcoZGTwdYMIUsfofk88185552 = lGjfcoZGTwdYMIUsfofk36235286;     lGjfcoZGTwdYMIUsfofk36235286 = lGjfcoZGTwdYMIUsfofk71839397;     lGjfcoZGTwdYMIUsfofk71839397 = lGjfcoZGTwdYMIUsfofk75090065;     lGjfcoZGTwdYMIUsfofk75090065 = lGjfcoZGTwdYMIUsfofk25283405;     lGjfcoZGTwdYMIUsfofk25283405 = lGjfcoZGTwdYMIUsfofk8631246;     lGjfcoZGTwdYMIUsfofk8631246 = lGjfcoZGTwdYMIUsfofk98815909;     lGjfcoZGTwdYMIUsfofk98815909 = lGjfcoZGTwdYMIUsfofk22318453;     lGjfcoZGTwdYMIUsfofk22318453 = lGjfcoZGTwdYMIUsfofk83416343;     lGjfcoZGTwdYMIUsfofk83416343 = lGjfcoZGTwdYMIUsfofk70660469;     lGjfcoZGTwdYMIUsfofk70660469 = lGjfcoZGTwdYMIUsfofk76494496;     lGjfcoZGTwdYMIUsfofk76494496 = lGjfcoZGTwdYMIUsfofk96078565;     lGjfcoZGTwdYMIUsfofk96078565 = lGjfcoZGTwdYMIUsfofk74720963;     lGjfcoZGTwdYMIUsfofk74720963 = lGjfcoZGTwdYMIUsfofk65092281;     lGjfcoZGTwdYMIUsfofk65092281 = lGjfcoZGTwdYMIUsfofk94189083;     lGjfcoZGTwdYMIUsfofk94189083 = lGjfcoZGTwdYMIUsfofk81394433;     lGjfcoZGTwdYMIUsfofk81394433 = lGjfcoZGTwdYMIUsfofk96000387;     lGjfcoZGTwdYMIUsfofk96000387 = lGjfcoZGTwdYMIUsfofk6408052;     lGjfcoZGTwdYMIUsfofk6408052 = lGjfcoZGTwdYMIUsfofk57799321;     lGjfcoZGTwdYMIUsfofk57799321 = lGjfcoZGTwdYMIUsfofk20766699;     lGjfcoZGTwdYMIUsfofk20766699 = lGjfcoZGTwdYMIUsfofk62338646;     lGjfcoZGTwdYMIUsfofk62338646 = lGjfcoZGTwdYMIUsfofk95776320;     lGjfcoZGTwdYMIUsfofk95776320 = lGjfcoZGTwdYMIUsfofk39648590;     lGjfcoZGTwdYMIUsfofk39648590 = lGjfcoZGTwdYMIUsfofk3078957;     lGjfcoZGTwdYMIUsfofk3078957 = lGjfcoZGTwdYMIUsfofk48843953;     lGjfcoZGTwdYMIUsfofk48843953 = lGjfcoZGTwdYMIUsfofk96019585;     lGjfcoZGTwdYMIUsfofk96019585 = lGjfcoZGTwdYMIUsfofk25840373;     lGjfcoZGTwdYMIUsfofk25840373 = lGjfcoZGTwdYMIUsfofk35423680;     lGjfcoZGTwdYMIUsfofk35423680 = lGjfcoZGTwdYMIUsfofk99147494;     lGjfcoZGTwdYMIUsfofk99147494 = lGjfcoZGTwdYMIUsfofk56456917;     lGjfcoZGTwdYMIUsfofk56456917 = lGjfcoZGTwdYMIUsfofk26061219;     lGjfcoZGTwdYMIUsfofk26061219 = lGjfcoZGTwdYMIUsfofk72980900;     lGjfcoZGTwdYMIUsfofk72980900 = lGjfcoZGTwdYMIUsfofk74444288;     lGjfcoZGTwdYMIUsfofk74444288 = lGjfcoZGTwdYMIUsfofk92947248;     lGjfcoZGTwdYMIUsfofk92947248 = lGjfcoZGTwdYMIUsfofk26329499;     lGjfcoZGTwdYMIUsfofk26329499 = lGjfcoZGTwdYMIUsfofk88341271;     lGjfcoZGTwdYMIUsfofk88341271 = lGjfcoZGTwdYMIUsfofk64688361;     lGjfcoZGTwdYMIUsfofk64688361 = lGjfcoZGTwdYMIUsfofk39982669;     lGjfcoZGTwdYMIUsfofk39982669 = lGjfcoZGTwdYMIUsfofk34208787;     lGjfcoZGTwdYMIUsfofk34208787 = lGjfcoZGTwdYMIUsfofk10968400;     lGjfcoZGTwdYMIUsfofk10968400 = lGjfcoZGTwdYMIUsfofk72289663;     lGjfcoZGTwdYMIUsfofk72289663 = lGjfcoZGTwdYMIUsfofk80123327;     lGjfcoZGTwdYMIUsfofk80123327 = lGjfcoZGTwdYMIUsfofk29082609;     lGjfcoZGTwdYMIUsfofk29082609 = lGjfcoZGTwdYMIUsfofk78778028;     lGjfcoZGTwdYMIUsfofk78778028 = lGjfcoZGTwdYMIUsfofk48001607;     lGjfcoZGTwdYMIUsfofk48001607 = lGjfcoZGTwdYMIUsfofk55298071;     lGjfcoZGTwdYMIUsfofk55298071 = lGjfcoZGTwdYMIUsfofk63130411;     lGjfcoZGTwdYMIUsfofk63130411 = lGjfcoZGTwdYMIUsfofk69889408;     lGjfcoZGTwdYMIUsfofk69889408 = lGjfcoZGTwdYMIUsfofk43229587;     lGjfcoZGTwdYMIUsfofk43229587 = lGjfcoZGTwdYMIUsfofk80100869;     lGjfcoZGTwdYMIUsfofk80100869 = lGjfcoZGTwdYMIUsfofk7904948;     lGjfcoZGTwdYMIUsfofk7904948 = lGjfcoZGTwdYMIUsfofk20795250;     lGjfcoZGTwdYMIUsfofk20795250 = lGjfcoZGTwdYMIUsfofk45645799;     lGjfcoZGTwdYMIUsfofk45645799 = lGjfcoZGTwdYMIUsfofk30323854;     lGjfcoZGTwdYMIUsfofk30323854 = lGjfcoZGTwdYMIUsfofk25107470;     lGjfcoZGTwdYMIUsfofk25107470 = lGjfcoZGTwdYMIUsfofk92409232;     lGjfcoZGTwdYMIUsfofk92409232 = lGjfcoZGTwdYMIUsfofk96586696;     lGjfcoZGTwdYMIUsfofk96586696 = lGjfcoZGTwdYMIUsfofk68760441;     lGjfcoZGTwdYMIUsfofk68760441 = lGjfcoZGTwdYMIUsfofk26246112;     lGjfcoZGTwdYMIUsfofk26246112 = lGjfcoZGTwdYMIUsfofk29263819;     lGjfcoZGTwdYMIUsfofk29263819 = lGjfcoZGTwdYMIUsfofk82790873;     lGjfcoZGTwdYMIUsfofk82790873 = lGjfcoZGTwdYMIUsfofk63392229;     lGjfcoZGTwdYMIUsfofk63392229 = lGjfcoZGTwdYMIUsfofk23170959;     lGjfcoZGTwdYMIUsfofk23170959 = lGjfcoZGTwdYMIUsfofk26959427;     lGjfcoZGTwdYMIUsfofk26959427 = lGjfcoZGTwdYMIUsfofk44599250;     lGjfcoZGTwdYMIUsfofk44599250 = lGjfcoZGTwdYMIUsfofk3513596;     lGjfcoZGTwdYMIUsfofk3513596 = lGjfcoZGTwdYMIUsfofk21634277;     lGjfcoZGTwdYMIUsfofk21634277 = lGjfcoZGTwdYMIUsfofk81773715;     lGjfcoZGTwdYMIUsfofk81773715 = lGjfcoZGTwdYMIUsfofk38762782;     lGjfcoZGTwdYMIUsfofk38762782 = lGjfcoZGTwdYMIUsfofk5847812;     lGjfcoZGTwdYMIUsfofk5847812 = lGjfcoZGTwdYMIUsfofk16706072;     lGjfcoZGTwdYMIUsfofk16706072 = lGjfcoZGTwdYMIUsfofk56017719;     lGjfcoZGTwdYMIUsfofk56017719 = lGjfcoZGTwdYMIUsfofk72199264;     lGjfcoZGTwdYMIUsfofk72199264 = lGjfcoZGTwdYMIUsfofk46830922;     lGjfcoZGTwdYMIUsfofk46830922 = lGjfcoZGTwdYMIUsfofk48477035;     lGjfcoZGTwdYMIUsfofk48477035 = lGjfcoZGTwdYMIUsfofk82215318;     lGjfcoZGTwdYMIUsfofk82215318 = lGjfcoZGTwdYMIUsfofk66693711;     lGjfcoZGTwdYMIUsfofk66693711 = lGjfcoZGTwdYMIUsfofk60870562;     lGjfcoZGTwdYMIUsfofk60870562 = lGjfcoZGTwdYMIUsfofk55077350;     lGjfcoZGTwdYMIUsfofk55077350 = lGjfcoZGTwdYMIUsfofk93545882;     lGjfcoZGTwdYMIUsfofk93545882 = lGjfcoZGTwdYMIUsfofk32889174;     lGjfcoZGTwdYMIUsfofk32889174 = lGjfcoZGTwdYMIUsfofk55950965;     lGjfcoZGTwdYMIUsfofk55950965 = lGjfcoZGTwdYMIUsfofk92194092;     lGjfcoZGTwdYMIUsfofk92194092 = lGjfcoZGTwdYMIUsfofk19046626;     lGjfcoZGTwdYMIUsfofk19046626 = lGjfcoZGTwdYMIUsfofk48551970;     lGjfcoZGTwdYMIUsfofk48551970 = lGjfcoZGTwdYMIUsfofk5265970;     lGjfcoZGTwdYMIUsfofk5265970 = lGjfcoZGTwdYMIUsfofk27335102;     lGjfcoZGTwdYMIUsfofk27335102 = lGjfcoZGTwdYMIUsfofk44120435;     lGjfcoZGTwdYMIUsfofk44120435 = lGjfcoZGTwdYMIUsfofk67839779;     lGjfcoZGTwdYMIUsfofk67839779 = lGjfcoZGTwdYMIUsfofk33920267;     lGjfcoZGTwdYMIUsfofk33920267 = lGjfcoZGTwdYMIUsfofk91754575;     lGjfcoZGTwdYMIUsfofk91754575 = lGjfcoZGTwdYMIUsfofk95927920;     lGjfcoZGTwdYMIUsfofk95927920 = lGjfcoZGTwdYMIUsfofk13736557;     lGjfcoZGTwdYMIUsfofk13736557 = lGjfcoZGTwdYMIUsfofk4944968;     lGjfcoZGTwdYMIUsfofk4944968 = lGjfcoZGTwdYMIUsfofk28177527;     lGjfcoZGTwdYMIUsfofk28177527 = lGjfcoZGTwdYMIUsfofk8897434;     lGjfcoZGTwdYMIUsfofk8897434 = lGjfcoZGTwdYMIUsfofk27203301;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dJOGjJkrQaOFGzDdbcBx58321548() {     float steIgderwEiSfvQgzmin97854967 = -991673068;    float steIgderwEiSfvQgzmin72118638 = -848288288;    float steIgderwEiSfvQgzmin31059472 = 33672098;    float steIgderwEiSfvQgzmin27377403 = -700770269;    float steIgderwEiSfvQgzmin9635874 = -413104614;    float steIgderwEiSfvQgzmin26235049 = -906657495;    float steIgderwEiSfvQgzmin90218293 = -656382828;    float steIgderwEiSfvQgzmin31932446 = -935617562;    float steIgderwEiSfvQgzmin71148459 = -396004559;    float steIgderwEiSfvQgzmin31597707 = -730335841;    float steIgderwEiSfvQgzmin41065070 = -500346097;    float steIgderwEiSfvQgzmin67865024 = -228800137;    float steIgderwEiSfvQgzmin63243730 = -9222052;    float steIgderwEiSfvQgzmin54507503 = -329214546;    float steIgderwEiSfvQgzmin59214516 = -866879271;    float steIgderwEiSfvQgzmin28607206 = -177106111;    float steIgderwEiSfvQgzmin68573790 = -758671126;    float steIgderwEiSfvQgzmin85385775 = -629463669;    float steIgderwEiSfvQgzmin75896865 = -647066851;    float steIgderwEiSfvQgzmin40181142 = -147127314;    float steIgderwEiSfvQgzmin51136142 = -125499635;    float steIgderwEiSfvQgzmin67803816 = -185917643;    float steIgderwEiSfvQgzmin59743130 = -705361121;    float steIgderwEiSfvQgzmin54616236 = -159734822;    float steIgderwEiSfvQgzmin26463035 = -937941252;    float steIgderwEiSfvQgzmin26373901 = -245632182;    float steIgderwEiSfvQgzmin47909376 = -141904476;    float steIgderwEiSfvQgzmin10861182 = -351856100;    float steIgderwEiSfvQgzmin80986440 = -39332616;    float steIgderwEiSfvQgzmin35191682 = -229835403;    float steIgderwEiSfvQgzmin32912370 = -283340898;    float steIgderwEiSfvQgzmin79935140 = -588499897;    float steIgderwEiSfvQgzmin82092855 = -741774265;    float steIgderwEiSfvQgzmin21821892 = -128154073;    float steIgderwEiSfvQgzmin4085456 = -569054341;    float steIgderwEiSfvQgzmin32974160 = -368950739;    float steIgderwEiSfvQgzmin18235775 = -894983585;    float steIgderwEiSfvQgzmin45739737 = -667630381;    float steIgderwEiSfvQgzmin4862788 = -297025668;    float steIgderwEiSfvQgzmin83444569 = 49248896;    float steIgderwEiSfvQgzmin24761645 = -10951148;    float steIgderwEiSfvQgzmin68465659 = -166453580;    float steIgderwEiSfvQgzmin83175746 = -374583943;    float steIgderwEiSfvQgzmin17476701 = -8662817;    float steIgderwEiSfvQgzmin68243722 = -685276560;    float steIgderwEiSfvQgzmin86529162 = -444614697;    float steIgderwEiSfvQgzmin55213812 = -431060025;    float steIgderwEiSfvQgzmin44392238 = -77706393;    float steIgderwEiSfvQgzmin53443059 = -895511765;    float steIgderwEiSfvQgzmin63725557 = -666586027;    float steIgderwEiSfvQgzmin31319736 = -741879004;    float steIgderwEiSfvQgzmin11760913 = -77820127;    float steIgderwEiSfvQgzmin98305244 = -833524490;    float steIgderwEiSfvQgzmin70328173 = -546167695;    float steIgderwEiSfvQgzmin7185407 = -696248645;    float steIgderwEiSfvQgzmin30051152 = -705755426;    float steIgderwEiSfvQgzmin12375509 = -42927167;    float steIgderwEiSfvQgzmin76443236 = -806593081;    float steIgderwEiSfvQgzmin914368 = -762829018;    float steIgderwEiSfvQgzmin83261973 = -67472432;    float steIgderwEiSfvQgzmin78325673 = -664753020;    float steIgderwEiSfvQgzmin79357111 = -204526729;    float steIgderwEiSfvQgzmin50946005 = -796284947;    float steIgderwEiSfvQgzmin35956778 = -66169156;    float steIgderwEiSfvQgzmin98685337 = -346994943;    float steIgderwEiSfvQgzmin61129929 = -911846201;    float steIgderwEiSfvQgzmin85772168 = -487025873;    float steIgderwEiSfvQgzmin41421839 = -881067979;    float steIgderwEiSfvQgzmin50422047 = -760160206;    float steIgderwEiSfvQgzmin26240356 = -397928533;    float steIgderwEiSfvQgzmin10371432 = -282122526;    float steIgderwEiSfvQgzmin22834054 = 8959254;    float steIgderwEiSfvQgzmin80522987 = -232438002;    float steIgderwEiSfvQgzmin92452295 = -596315748;    float steIgderwEiSfvQgzmin15419497 = -36176167;    float steIgderwEiSfvQgzmin82670482 = -959046056;    float steIgderwEiSfvQgzmin84628069 = -811333700;    float steIgderwEiSfvQgzmin42266430 = -596698305;    float steIgderwEiSfvQgzmin86372513 = -474458262;    float steIgderwEiSfvQgzmin39933872 = -393326556;    float steIgderwEiSfvQgzmin71160088 = -814572158;    float steIgderwEiSfvQgzmin3517138 = 35801917;    float steIgderwEiSfvQgzmin57418123 = -456344336;    float steIgderwEiSfvQgzmin17260884 = -372746590;    float steIgderwEiSfvQgzmin3871946 = -487956400;    float steIgderwEiSfvQgzmin21151457 = -105520771;    float steIgderwEiSfvQgzmin81629896 = -754975407;    float steIgderwEiSfvQgzmin11764682 = -95606570;    float steIgderwEiSfvQgzmin14636486 = -431905428;    float steIgderwEiSfvQgzmin74034304 = -863298916;    float steIgderwEiSfvQgzmin20598652 = -226023572;    float steIgderwEiSfvQgzmin41792539 = 11609495;    float steIgderwEiSfvQgzmin44825369 = -904801363;    float steIgderwEiSfvQgzmin21600815 = -129553236;    float steIgderwEiSfvQgzmin5118897 = -285998085;    float steIgderwEiSfvQgzmin45404534 = -806424420;    float steIgderwEiSfvQgzmin17519655 = -370168634;    float steIgderwEiSfvQgzmin47218969 = -208414787;    float steIgderwEiSfvQgzmin18791363 = -661667874;    float steIgderwEiSfvQgzmin7113793 = -991673068;     steIgderwEiSfvQgzmin97854967 = steIgderwEiSfvQgzmin72118638;     steIgderwEiSfvQgzmin72118638 = steIgderwEiSfvQgzmin31059472;     steIgderwEiSfvQgzmin31059472 = steIgderwEiSfvQgzmin27377403;     steIgderwEiSfvQgzmin27377403 = steIgderwEiSfvQgzmin9635874;     steIgderwEiSfvQgzmin9635874 = steIgderwEiSfvQgzmin26235049;     steIgderwEiSfvQgzmin26235049 = steIgderwEiSfvQgzmin90218293;     steIgderwEiSfvQgzmin90218293 = steIgderwEiSfvQgzmin31932446;     steIgderwEiSfvQgzmin31932446 = steIgderwEiSfvQgzmin71148459;     steIgderwEiSfvQgzmin71148459 = steIgderwEiSfvQgzmin31597707;     steIgderwEiSfvQgzmin31597707 = steIgderwEiSfvQgzmin41065070;     steIgderwEiSfvQgzmin41065070 = steIgderwEiSfvQgzmin67865024;     steIgderwEiSfvQgzmin67865024 = steIgderwEiSfvQgzmin63243730;     steIgderwEiSfvQgzmin63243730 = steIgderwEiSfvQgzmin54507503;     steIgderwEiSfvQgzmin54507503 = steIgderwEiSfvQgzmin59214516;     steIgderwEiSfvQgzmin59214516 = steIgderwEiSfvQgzmin28607206;     steIgderwEiSfvQgzmin28607206 = steIgderwEiSfvQgzmin68573790;     steIgderwEiSfvQgzmin68573790 = steIgderwEiSfvQgzmin85385775;     steIgderwEiSfvQgzmin85385775 = steIgderwEiSfvQgzmin75896865;     steIgderwEiSfvQgzmin75896865 = steIgderwEiSfvQgzmin40181142;     steIgderwEiSfvQgzmin40181142 = steIgderwEiSfvQgzmin51136142;     steIgderwEiSfvQgzmin51136142 = steIgderwEiSfvQgzmin67803816;     steIgderwEiSfvQgzmin67803816 = steIgderwEiSfvQgzmin59743130;     steIgderwEiSfvQgzmin59743130 = steIgderwEiSfvQgzmin54616236;     steIgderwEiSfvQgzmin54616236 = steIgderwEiSfvQgzmin26463035;     steIgderwEiSfvQgzmin26463035 = steIgderwEiSfvQgzmin26373901;     steIgderwEiSfvQgzmin26373901 = steIgderwEiSfvQgzmin47909376;     steIgderwEiSfvQgzmin47909376 = steIgderwEiSfvQgzmin10861182;     steIgderwEiSfvQgzmin10861182 = steIgderwEiSfvQgzmin80986440;     steIgderwEiSfvQgzmin80986440 = steIgderwEiSfvQgzmin35191682;     steIgderwEiSfvQgzmin35191682 = steIgderwEiSfvQgzmin32912370;     steIgderwEiSfvQgzmin32912370 = steIgderwEiSfvQgzmin79935140;     steIgderwEiSfvQgzmin79935140 = steIgderwEiSfvQgzmin82092855;     steIgderwEiSfvQgzmin82092855 = steIgderwEiSfvQgzmin21821892;     steIgderwEiSfvQgzmin21821892 = steIgderwEiSfvQgzmin4085456;     steIgderwEiSfvQgzmin4085456 = steIgderwEiSfvQgzmin32974160;     steIgderwEiSfvQgzmin32974160 = steIgderwEiSfvQgzmin18235775;     steIgderwEiSfvQgzmin18235775 = steIgderwEiSfvQgzmin45739737;     steIgderwEiSfvQgzmin45739737 = steIgderwEiSfvQgzmin4862788;     steIgderwEiSfvQgzmin4862788 = steIgderwEiSfvQgzmin83444569;     steIgderwEiSfvQgzmin83444569 = steIgderwEiSfvQgzmin24761645;     steIgderwEiSfvQgzmin24761645 = steIgderwEiSfvQgzmin68465659;     steIgderwEiSfvQgzmin68465659 = steIgderwEiSfvQgzmin83175746;     steIgderwEiSfvQgzmin83175746 = steIgderwEiSfvQgzmin17476701;     steIgderwEiSfvQgzmin17476701 = steIgderwEiSfvQgzmin68243722;     steIgderwEiSfvQgzmin68243722 = steIgderwEiSfvQgzmin86529162;     steIgderwEiSfvQgzmin86529162 = steIgderwEiSfvQgzmin55213812;     steIgderwEiSfvQgzmin55213812 = steIgderwEiSfvQgzmin44392238;     steIgderwEiSfvQgzmin44392238 = steIgderwEiSfvQgzmin53443059;     steIgderwEiSfvQgzmin53443059 = steIgderwEiSfvQgzmin63725557;     steIgderwEiSfvQgzmin63725557 = steIgderwEiSfvQgzmin31319736;     steIgderwEiSfvQgzmin31319736 = steIgderwEiSfvQgzmin11760913;     steIgderwEiSfvQgzmin11760913 = steIgderwEiSfvQgzmin98305244;     steIgderwEiSfvQgzmin98305244 = steIgderwEiSfvQgzmin70328173;     steIgderwEiSfvQgzmin70328173 = steIgderwEiSfvQgzmin7185407;     steIgderwEiSfvQgzmin7185407 = steIgderwEiSfvQgzmin30051152;     steIgderwEiSfvQgzmin30051152 = steIgderwEiSfvQgzmin12375509;     steIgderwEiSfvQgzmin12375509 = steIgderwEiSfvQgzmin76443236;     steIgderwEiSfvQgzmin76443236 = steIgderwEiSfvQgzmin914368;     steIgderwEiSfvQgzmin914368 = steIgderwEiSfvQgzmin83261973;     steIgderwEiSfvQgzmin83261973 = steIgderwEiSfvQgzmin78325673;     steIgderwEiSfvQgzmin78325673 = steIgderwEiSfvQgzmin79357111;     steIgderwEiSfvQgzmin79357111 = steIgderwEiSfvQgzmin50946005;     steIgderwEiSfvQgzmin50946005 = steIgderwEiSfvQgzmin35956778;     steIgderwEiSfvQgzmin35956778 = steIgderwEiSfvQgzmin98685337;     steIgderwEiSfvQgzmin98685337 = steIgderwEiSfvQgzmin61129929;     steIgderwEiSfvQgzmin61129929 = steIgderwEiSfvQgzmin85772168;     steIgderwEiSfvQgzmin85772168 = steIgderwEiSfvQgzmin41421839;     steIgderwEiSfvQgzmin41421839 = steIgderwEiSfvQgzmin50422047;     steIgderwEiSfvQgzmin50422047 = steIgderwEiSfvQgzmin26240356;     steIgderwEiSfvQgzmin26240356 = steIgderwEiSfvQgzmin10371432;     steIgderwEiSfvQgzmin10371432 = steIgderwEiSfvQgzmin22834054;     steIgderwEiSfvQgzmin22834054 = steIgderwEiSfvQgzmin80522987;     steIgderwEiSfvQgzmin80522987 = steIgderwEiSfvQgzmin92452295;     steIgderwEiSfvQgzmin92452295 = steIgderwEiSfvQgzmin15419497;     steIgderwEiSfvQgzmin15419497 = steIgderwEiSfvQgzmin82670482;     steIgderwEiSfvQgzmin82670482 = steIgderwEiSfvQgzmin84628069;     steIgderwEiSfvQgzmin84628069 = steIgderwEiSfvQgzmin42266430;     steIgderwEiSfvQgzmin42266430 = steIgderwEiSfvQgzmin86372513;     steIgderwEiSfvQgzmin86372513 = steIgderwEiSfvQgzmin39933872;     steIgderwEiSfvQgzmin39933872 = steIgderwEiSfvQgzmin71160088;     steIgderwEiSfvQgzmin71160088 = steIgderwEiSfvQgzmin3517138;     steIgderwEiSfvQgzmin3517138 = steIgderwEiSfvQgzmin57418123;     steIgderwEiSfvQgzmin57418123 = steIgderwEiSfvQgzmin17260884;     steIgderwEiSfvQgzmin17260884 = steIgderwEiSfvQgzmin3871946;     steIgderwEiSfvQgzmin3871946 = steIgderwEiSfvQgzmin21151457;     steIgderwEiSfvQgzmin21151457 = steIgderwEiSfvQgzmin81629896;     steIgderwEiSfvQgzmin81629896 = steIgderwEiSfvQgzmin11764682;     steIgderwEiSfvQgzmin11764682 = steIgderwEiSfvQgzmin14636486;     steIgderwEiSfvQgzmin14636486 = steIgderwEiSfvQgzmin74034304;     steIgderwEiSfvQgzmin74034304 = steIgderwEiSfvQgzmin20598652;     steIgderwEiSfvQgzmin20598652 = steIgderwEiSfvQgzmin41792539;     steIgderwEiSfvQgzmin41792539 = steIgderwEiSfvQgzmin44825369;     steIgderwEiSfvQgzmin44825369 = steIgderwEiSfvQgzmin21600815;     steIgderwEiSfvQgzmin21600815 = steIgderwEiSfvQgzmin5118897;     steIgderwEiSfvQgzmin5118897 = steIgderwEiSfvQgzmin45404534;     steIgderwEiSfvQgzmin45404534 = steIgderwEiSfvQgzmin17519655;     steIgderwEiSfvQgzmin17519655 = steIgderwEiSfvQgzmin47218969;     steIgderwEiSfvQgzmin47218969 = steIgderwEiSfvQgzmin18791363;     steIgderwEiSfvQgzmin18791363 = steIgderwEiSfvQgzmin7113793;     steIgderwEiSfvQgzmin7113793 = steIgderwEiSfvQgzmin97854967;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void KWZdxjvpHISlovXgimKh64980293() {     float CeyktjZJtTFKNsoqiJLL97848496 = -864197210;    float CeyktjZJtTFKNsoqiJLL10690955 = -890883413;    float CeyktjZJtTFKNsoqiJLL32372684 = 76543484;    float CeyktjZJtTFKNsoqiJLL12457263 = -521311099;    float CeyktjZJtTFKNsoqiJLL18475976 = -881459935;    float CeyktjZJtTFKNsoqiJLL61040481 = -606090306;    float CeyktjZJtTFKNsoqiJLL58765629 = -360919295;    float CeyktjZJtTFKNsoqiJLL1146496 = -639794880;    float CeyktjZJtTFKNsoqiJLL53888623 = -109650629;    float CeyktjZJtTFKNsoqiJLL28237221 = -367142712;    float CeyktjZJtTFKNsoqiJLL61385675 = -459818169;    float CeyktjZJtTFKNsoqiJLL82156701 = 35721500;    float CeyktjZJtTFKNsoqiJLL95096644 = -30880526;    float CeyktjZJtTFKNsoqiJLL8351723 = -674367598;    float CeyktjZJtTFKNsoqiJLL4386625 = -829592123;    float CeyktjZJtTFKNsoqiJLL3171754 = -509901761;    float CeyktjZJtTFKNsoqiJLL24426550 = -578285696;    float CeyktjZJtTFKNsoqiJLL8386309 = 73464709;    float CeyktjZJtTFKNsoqiJLL96325134 = -165049118;    float CeyktjZJtTFKNsoqiJLL12254906 = -759151766;    float CeyktjZJtTFKNsoqiJLL30805567 = -935966009;    float CeyktjZJtTFKNsoqiJLL46100082 = -695909526;    float CeyktjZJtTFKNsoqiJLL96621526 = -64318963;    float CeyktjZJtTFKNsoqiJLL98266343 = -40655344;    float CeyktjZJtTFKNsoqiJLL54905724 = -264635105;    float CeyktjZJtTFKNsoqiJLL42947978 = -153838111;    float CeyktjZJtTFKNsoqiJLL18130958 = -442704188;    float CeyktjZJtTFKNsoqiJLL46264707 = -960823088;    float CeyktjZJtTFKNsoqiJLL93664887 = -685402931;    float CeyktjZJtTFKNsoqiJLL11281552 = 94733223;    float CeyktjZJtTFKNsoqiJLL66325048 = -670434132;    float CeyktjZJtTFKNsoqiJLL64658998 = -980321658;    float CeyktjZJtTFKNsoqiJLL67545278 = -943718481;    float CeyktjZJtTFKNsoqiJLL22104729 = -4835806;    float CeyktjZJtTFKNsoqiJLL86443493 = -828368266;    float CeyktjZJtTFKNsoqiJLL75802349 = -448716485;    float CeyktjZJtTFKNsoqiJLL30177143 = -994636964;    float CeyktjZJtTFKNsoqiJLL15657417 = -662893060;    float CeyktjZJtTFKNsoqiJLL40176385 = 90292916;    float CeyktjZJtTFKNsoqiJLL55317462 = -660182790;    float CeyktjZJtTFKNsoqiJLL12617313 = -98895046;    float CeyktjZJtTFKNsoqiJLL54163321 = -360245662;    float CeyktjZJtTFKNsoqiJLL43459107 = -933714197;    float CeyktjZJtTFKNsoqiJLL76708598 = -665627260;    float CeyktjZJtTFKNsoqiJLL52261774 = -58535962;    float CeyktjZJtTFKNsoqiJLL26957131 = -256659368;    float CeyktjZJtTFKNsoqiJLL82911605 = -912340647;    float CeyktjZJtTFKNsoqiJLL19775819 = -686467749;    float CeyktjZJtTFKNsoqiJLL72101448 = -459275626;    float CeyktjZJtTFKNsoqiJLL83770980 = -253610913;    float CeyktjZJtTFKNsoqiJLL44579106 = -320434012;    float CeyktjZJtTFKNsoqiJLL26308170 = -459965875;    float CeyktjZJtTFKNsoqiJLL73753592 = -725578498;    float CeyktjZJtTFKNsoqiJLL80578444 = 21912453;    float CeyktjZJtTFKNsoqiJLL34174727 = -453147923;    float CeyktjZJtTFKNsoqiJLL51748414 = -68287684;    float CeyktjZJtTFKNsoqiJLL14069428 = -726564450;    float CeyktjZJtTFKNsoqiJLL34106340 = -882801172;    float CeyktjZJtTFKNsoqiJLL57551539 = -156675995;    float CeyktjZJtTFKNsoqiJLL75527998 = -627621825;    float CeyktjZJtTFKNsoqiJLL42909524 = -63386118;    float CeyktjZJtTFKNsoqiJLL12500923 = -400096207;    float CeyktjZJtTFKNsoqiJLL7481609 = -954391949;    float CeyktjZJtTFKNsoqiJLL42607072 = -104383853;    float CeyktjZJtTFKNsoqiJLL61912172 = -696708580;    float CeyktjZJtTFKNsoqiJLL96726677 = -479496512;    float CeyktjZJtTFKNsoqiJLL14611423 = -20560020;    float CeyktjZJtTFKNsoqiJLL72991915 = 73955279;    float CeyktjZJtTFKNsoqiJLL21908230 = -845999332;    float CeyktjZJtTFKNsoqiJLL28584276 = -280875638;    float CeyktjZJtTFKNsoqiJLL72994610 = -515264798;    float CeyktjZJtTFKNsoqiJLL8769134 = -915392637;    float CeyktjZJtTFKNsoqiJLL68209924 = 83171793;    float CeyktjZJtTFKNsoqiJLL41007672 = -504866329;    float CeyktjZJtTFKNsoqiJLL99637592 = -560256720;    float CeyktjZJtTFKNsoqiJLL76642245 = -475720347;    float CeyktjZJtTFKNsoqiJLL2640976 = -762195329;    float CeyktjZJtTFKNsoqiJLL19912928 = -398691704;    float CeyktjZJtTFKNsoqiJLL46004569 = -982119383;    float CeyktjZJtTFKNsoqiJLL27948593 = 92024262;    float CeyktjZJtTFKNsoqiJLL60036373 = -241497464;    float CeyktjZJtTFKNsoqiJLL98355138 = -756236440;    float CeyktjZJtTFKNsoqiJLL74163258 = -401547463;    float CeyktjZJtTFKNsoqiJLL9893908 = -331792018;    float CeyktjZJtTFKNsoqiJLL66702446 = -584832766;    float CeyktjZJtTFKNsoqiJLL40016878 = -110468257;    float CeyktjZJtTFKNsoqiJLL90905405 = -154743161;    float CeyktjZJtTFKNsoqiJLL86966834 = -865630935;    float CeyktjZJtTFKNsoqiJLL87930002 = -551687883;    float CeyktjZJtTFKNsoqiJLL34695079 = -660080582;    float CeyktjZJtTFKNsoqiJLL61732921 = -722152035;    float CeyktjZJtTFKNsoqiJLL96070802 = -11835792;    float CeyktjZJtTFKNsoqiJLL58105878 = -406217065;    float CeyktjZJtTFKNsoqiJLL64648386 = -282085259;    float CeyktjZJtTFKNsoqiJLL12407939 = -496796672;    float CeyktjZJtTFKNsoqiJLL116391 = -698798840;    float CeyktjZJtTFKNsoqiJLL46681712 = -405853714;    float CeyktjZJtTFKNsoqiJLL852035 = -729330345;    float CeyktjZJtTFKNsoqiJLL14796427 = -968918681;    float CeyktjZJtTFKNsoqiJLL55535097 = -864197210;     CeyktjZJtTFKNsoqiJLL97848496 = CeyktjZJtTFKNsoqiJLL10690955;     CeyktjZJtTFKNsoqiJLL10690955 = CeyktjZJtTFKNsoqiJLL32372684;     CeyktjZJtTFKNsoqiJLL32372684 = CeyktjZJtTFKNsoqiJLL12457263;     CeyktjZJtTFKNsoqiJLL12457263 = CeyktjZJtTFKNsoqiJLL18475976;     CeyktjZJtTFKNsoqiJLL18475976 = CeyktjZJtTFKNsoqiJLL61040481;     CeyktjZJtTFKNsoqiJLL61040481 = CeyktjZJtTFKNsoqiJLL58765629;     CeyktjZJtTFKNsoqiJLL58765629 = CeyktjZJtTFKNsoqiJLL1146496;     CeyktjZJtTFKNsoqiJLL1146496 = CeyktjZJtTFKNsoqiJLL53888623;     CeyktjZJtTFKNsoqiJLL53888623 = CeyktjZJtTFKNsoqiJLL28237221;     CeyktjZJtTFKNsoqiJLL28237221 = CeyktjZJtTFKNsoqiJLL61385675;     CeyktjZJtTFKNsoqiJLL61385675 = CeyktjZJtTFKNsoqiJLL82156701;     CeyktjZJtTFKNsoqiJLL82156701 = CeyktjZJtTFKNsoqiJLL95096644;     CeyktjZJtTFKNsoqiJLL95096644 = CeyktjZJtTFKNsoqiJLL8351723;     CeyktjZJtTFKNsoqiJLL8351723 = CeyktjZJtTFKNsoqiJLL4386625;     CeyktjZJtTFKNsoqiJLL4386625 = CeyktjZJtTFKNsoqiJLL3171754;     CeyktjZJtTFKNsoqiJLL3171754 = CeyktjZJtTFKNsoqiJLL24426550;     CeyktjZJtTFKNsoqiJLL24426550 = CeyktjZJtTFKNsoqiJLL8386309;     CeyktjZJtTFKNsoqiJLL8386309 = CeyktjZJtTFKNsoqiJLL96325134;     CeyktjZJtTFKNsoqiJLL96325134 = CeyktjZJtTFKNsoqiJLL12254906;     CeyktjZJtTFKNsoqiJLL12254906 = CeyktjZJtTFKNsoqiJLL30805567;     CeyktjZJtTFKNsoqiJLL30805567 = CeyktjZJtTFKNsoqiJLL46100082;     CeyktjZJtTFKNsoqiJLL46100082 = CeyktjZJtTFKNsoqiJLL96621526;     CeyktjZJtTFKNsoqiJLL96621526 = CeyktjZJtTFKNsoqiJLL98266343;     CeyktjZJtTFKNsoqiJLL98266343 = CeyktjZJtTFKNsoqiJLL54905724;     CeyktjZJtTFKNsoqiJLL54905724 = CeyktjZJtTFKNsoqiJLL42947978;     CeyktjZJtTFKNsoqiJLL42947978 = CeyktjZJtTFKNsoqiJLL18130958;     CeyktjZJtTFKNsoqiJLL18130958 = CeyktjZJtTFKNsoqiJLL46264707;     CeyktjZJtTFKNsoqiJLL46264707 = CeyktjZJtTFKNsoqiJLL93664887;     CeyktjZJtTFKNsoqiJLL93664887 = CeyktjZJtTFKNsoqiJLL11281552;     CeyktjZJtTFKNsoqiJLL11281552 = CeyktjZJtTFKNsoqiJLL66325048;     CeyktjZJtTFKNsoqiJLL66325048 = CeyktjZJtTFKNsoqiJLL64658998;     CeyktjZJtTFKNsoqiJLL64658998 = CeyktjZJtTFKNsoqiJLL67545278;     CeyktjZJtTFKNsoqiJLL67545278 = CeyktjZJtTFKNsoqiJLL22104729;     CeyktjZJtTFKNsoqiJLL22104729 = CeyktjZJtTFKNsoqiJLL86443493;     CeyktjZJtTFKNsoqiJLL86443493 = CeyktjZJtTFKNsoqiJLL75802349;     CeyktjZJtTFKNsoqiJLL75802349 = CeyktjZJtTFKNsoqiJLL30177143;     CeyktjZJtTFKNsoqiJLL30177143 = CeyktjZJtTFKNsoqiJLL15657417;     CeyktjZJtTFKNsoqiJLL15657417 = CeyktjZJtTFKNsoqiJLL40176385;     CeyktjZJtTFKNsoqiJLL40176385 = CeyktjZJtTFKNsoqiJLL55317462;     CeyktjZJtTFKNsoqiJLL55317462 = CeyktjZJtTFKNsoqiJLL12617313;     CeyktjZJtTFKNsoqiJLL12617313 = CeyktjZJtTFKNsoqiJLL54163321;     CeyktjZJtTFKNsoqiJLL54163321 = CeyktjZJtTFKNsoqiJLL43459107;     CeyktjZJtTFKNsoqiJLL43459107 = CeyktjZJtTFKNsoqiJLL76708598;     CeyktjZJtTFKNsoqiJLL76708598 = CeyktjZJtTFKNsoqiJLL52261774;     CeyktjZJtTFKNsoqiJLL52261774 = CeyktjZJtTFKNsoqiJLL26957131;     CeyktjZJtTFKNsoqiJLL26957131 = CeyktjZJtTFKNsoqiJLL82911605;     CeyktjZJtTFKNsoqiJLL82911605 = CeyktjZJtTFKNsoqiJLL19775819;     CeyktjZJtTFKNsoqiJLL19775819 = CeyktjZJtTFKNsoqiJLL72101448;     CeyktjZJtTFKNsoqiJLL72101448 = CeyktjZJtTFKNsoqiJLL83770980;     CeyktjZJtTFKNsoqiJLL83770980 = CeyktjZJtTFKNsoqiJLL44579106;     CeyktjZJtTFKNsoqiJLL44579106 = CeyktjZJtTFKNsoqiJLL26308170;     CeyktjZJtTFKNsoqiJLL26308170 = CeyktjZJtTFKNsoqiJLL73753592;     CeyktjZJtTFKNsoqiJLL73753592 = CeyktjZJtTFKNsoqiJLL80578444;     CeyktjZJtTFKNsoqiJLL80578444 = CeyktjZJtTFKNsoqiJLL34174727;     CeyktjZJtTFKNsoqiJLL34174727 = CeyktjZJtTFKNsoqiJLL51748414;     CeyktjZJtTFKNsoqiJLL51748414 = CeyktjZJtTFKNsoqiJLL14069428;     CeyktjZJtTFKNsoqiJLL14069428 = CeyktjZJtTFKNsoqiJLL34106340;     CeyktjZJtTFKNsoqiJLL34106340 = CeyktjZJtTFKNsoqiJLL57551539;     CeyktjZJtTFKNsoqiJLL57551539 = CeyktjZJtTFKNsoqiJLL75527998;     CeyktjZJtTFKNsoqiJLL75527998 = CeyktjZJtTFKNsoqiJLL42909524;     CeyktjZJtTFKNsoqiJLL42909524 = CeyktjZJtTFKNsoqiJLL12500923;     CeyktjZJtTFKNsoqiJLL12500923 = CeyktjZJtTFKNsoqiJLL7481609;     CeyktjZJtTFKNsoqiJLL7481609 = CeyktjZJtTFKNsoqiJLL42607072;     CeyktjZJtTFKNsoqiJLL42607072 = CeyktjZJtTFKNsoqiJLL61912172;     CeyktjZJtTFKNsoqiJLL61912172 = CeyktjZJtTFKNsoqiJLL96726677;     CeyktjZJtTFKNsoqiJLL96726677 = CeyktjZJtTFKNsoqiJLL14611423;     CeyktjZJtTFKNsoqiJLL14611423 = CeyktjZJtTFKNsoqiJLL72991915;     CeyktjZJtTFKNsoqiJLL72991915 = CeyktjZJtTFKNsoqiJLL21908230;     CeyktjZJtTFKNsoqiJLL21908230 = CeyktjZJtTFKNsoqiJLL28584276;     CeyktjZJtTFKNsoqiJLL28584276 = CeyktjZJtTFKNsoqiJLL72994610;     CeyktjZJtTFKNsoqiJLL72994610 = CeyktjZJtTFKNsoqiJLL8769134;     CeyktjZJtTFKNsoqiJLL8769134 = CeyktjZJtTFKNsoqiJLL68209924;     CeyktjZJtTFKNsoqiJLL68209924 = CeyktjZJtTFKNsoqiJLL41007672;     CeyktjZJtTFKNsoqiJLL41007672 = CeyktjZJtTFKNsoqiJLL99637592;     CeyktjZJtTFKNsoqiJLL99637592 = CeyktjZJtTFKNsoqiJLL76642245;     CeyktjZJtTFKNsoqiJLL76642245 = CeyktjZJtTFKNsoqiJLL2640976;     CeyktjZJtTFKNsoqiJLL2640976 = CeyktjZJtTFKNsoqiJLL19912928;     CeyktjZJtTFKNsoqiJLL19912928 = CeyktjZJtTFKNsoqiJLL46004569;     CeyktjZJtTFKNsoqiJLL46004569 = CeyktjZJtTFKNsoqiJLL27948593;     CeyktjZJtTFKNsoqiJLL27948593 = CeyktjZJtTFKNsoqiJLL60036373;     CeyktjZJtTFKNsoqiJLL60036373 = CeyktjZJtTFKNsoqiJLL98355138;     CeyktjZJtTFKNsoqiJLL98355138 = CeyktjZJtTFKNsoqiJLL74163258;     CeyktjZJtTFKNsoqiJLL74163258 = CeyktjZJtTFKNsoqiJLL9893908;     CeyktjZJtTFKNsoqiJLL9893908 = CeyktjZJtTFKNsoqiJLL66702446;     CeyktjZJtTFKNsoqiJLL66702446 = CeyktjZJtTFKNsoqiJLL40016878;     CeyktjZJtTFKNsoqiJLL40016878 = CeyktjZJtTFKNsoqiJLL90905405;     CeyktjZJtTFKNsoqiJLL90905405 = CeyktjZJtTFKNsoqiJLL86966834;     CeyktjZJtTFKNsoqiJLL86966834 = CeyktjZJtTFKNsoqiJLL87930002;     CeyktjZJtTFKNsoqiJLL87930002 = CeyktjZJtTFKNsoqiJLL34695079;     CeyktjZJtTFKNsoqiJLL34695079 = CeyktjZJtTFKNsoqiJLL61732921;     CeyktjZJtTFKNsoqiJLL61732921 = CeyktjZJtTFKNsoqiJLL96070802;     CeyktjZJtTFKNsoqiJLL96070802 = CeyktjZJtTFKNsoqiJLL58105878;     CeyktjZJtTFKNsoqiJLL58105878 = CeyktjZJtTFKNsoqiJLL64648386;     CeyktjZJtTFKNsoqiJLL64648386 = CeyktjZJtTFKNsoqiJLL12407939;     CeyktjZJtTFKNsoqiJLL12407939 = CeyktjZJtTFKNsoqiJLL116391;     CeyktjZJtTFKNsoqiJLL116391 = CeyktjZJtTFKNsoqiJLL46681712;     CeyktjZJtTFKNsoqiJLL46681712 = CeyktjZJtTFKNsoqiJLL852035;     CeyktjZJtTFKNsoqiJLL852035 = CeyktjZJtTFKNsoqiJLL14796427;     CeyktjZJtTFKNsoqiJLL14796427 = CeyktjZJtTFKNsoqiJLL55535097;     CeyktjZJtTFKNsoqiJLL55535097 = CeyktjZJtTFKNsoqiJLL97848496;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mhbJoNPzurZIUjZrOqTM19396506() {     float kvegdkzmUCvcBezmyoGn68500163 = -382346340;    float kvegdkzmUCvcBezmyoGn79364472 = -762526713;    float kvegdkzmUCvcBezmyoGn12341603 = -365750976;    float kvegdkzmUCvcBezmyoGn52388550 = -711449489;    float kvegdkzmUCvcBezmyoGn39926298 = -664866283;    float kvegdkzmUCvcBezmyoGn51040244 = -307671458;    float kvegdkzmUCvcBezmyoGn77144524 = -359695005;    float kvegdkzmUCvcBezmyoGn57988876 = -986611969;    float kvegdkzmUCvcBezmyoGn99753677 = -202997958;    float kvegdkzmUCvcBezmyoGn51203681 = -668238062;    float kvegdkzmUCvcBezmyoGn3634836 = -494720099;    float kvegdkzmUCvcBezmyoGn27703272 = -280042293;    float kvegdkzmUCvcBezmyoGn74924031 = 59582416;    float kvegdkzmUCvcBezmyoGn92198756 = -475395808;    float kvegdkzmUCvcBezmyoGn87106644 = -979342126;    float kvegdkzmUCvcBezmyoGn35700393 = -79885348;    float kvegdkzmUCvcBezmyoGn18279377 = -487470826;    float kvegdkzmUCvcBezmyoGn28679803 = 84298513;    float kvegdkzmUCvcBezmyoGn78032916 = -872237060;    float kvegdkzmUCvcBezmyoGn71041614 = -201518070;    float kvegdkzmUCvcBezmyoGn85941320 = -844645150;    float kvegdkzmUCvcBezmyoGn7495847 = 72805168;    float kvegdkzmUCvcBezmyoGn98565335 = -632162108;    float kvegdkzmUCvcBezmyoGn32115881 = -733056251;    float kvegdkzmUCvcBezmyoGn19030113 = 59460207;    float kvegdkzmUCvcBezmyoGn73545558 = -252122555;    float kvegdkzmUCvcBezmyoGn26391743 = -837881964;    float kvegdkzmUCvcBezmyoGn54046932 = -365946856;    float kvegdkzmUCvcBezmyoGn25807376 = -979761148;    float kvegdkzmUCvcBezmyoGn50453647 = -463661848;    float kvegdkzmUCvcBezmyoGn73397044 = -418867818;    float kvegdkzmUCvcBezmyoGn9170459 = -208618821;    float kvegdkzmUCvcBezmyoGn50490639 = -890680090;    float kvegdkzmUCvcBezmyoGn87469704 = -668365490;    float kvegdkzmUCvcBezmyoGn64467730 = -675315045;    float kvegdkzmUCvcBezmyoGn35795608 = -527943033;    float kvegdkzmUCvcBezmyoGn73968628 = -83230171;    float kvegdkzmUCvcBezmyoGn68449905 = -447006167;    float kvegdkzmUCvcBezmyoGn18709674 = -965435721;    float kvegdkzmUCvcBezmyoGn50420761 = -177791152;    float kvegdkzmUCvcBezmyoGn72690596 = -482913925;    float kvegdkzmUCvcBezmyoGn82646311 = -129634732;    float kvegdkzmUCvcBezmyoGn92426066 = -920468250;    float kvegdkzmUCvcBezmyoGn83216899 = -737602232;    float kvegdkzmUCvcBezmyoGn48215833 = -367182357;    float kvegdkzmUCvcBezmyoGn33362966 = -14996522;    float kvegdkzmUCvcBezmyoGn9042808 = 71589554;    float kvegdkzmUCvcBezmyoGn85390029 = -38598045;    float kvegdkzmUCvcBezmyoGn77542900 = -445039595;    float kvegdkzmUCvcBezmyoGn92198466 = -536619792;    float kvegdkzmUCvcBezmyoGn12768430 = -967006133;    float kvegdkzmUCvcBezmyoGn68179674 = -429185597;    float kvegdkzmUCvcBezmyoGn28829250 = -653376540;    float kvegdkzmUCvcBezmyoGn70805749 = -638969043;    float kvegdkzmUCvcBezmyoGn33455185 = -564088837;    float kvegdkzmUCvcBezmyoGn61004316 = -355151509;    float kvegdkzmUCvcBezmyoGn80799137 = -30364605;    float kvegdkzmUCvcBezmyoGn80225722 = -632694726;    float kvegdkzmUCvcBezmyoGn33358437 = -670909696;    float kvegdkzmUCvcBezmyoGn66380739 = -312743728;    float kvegdkzmUCvcBezmyoGn24648501 = -469789495;    float kvegdkzmUCvcBezmyoGn23097593 = -993748150;    float kvegdkzmUCvcBezmyoGn32181501 = 93149178;    float kvegdkzmUCvcBezmyoGn49300030 = -739336111;    float kvegdkzmUCvcBezmyoGn77806636 = -149370245;    float kvegdkzmUCvcBezmyoGn94464377 = -186101278;    float kvegdkzmUCvcBezmyoGn77212633 = -389362204;    float kvegdkzmUCvcBezmyoGn87454327 = -272052095;    float kvegdkzmUCvcBezmyoGn27731027 = -800080763;    float kvegdkzmUCvcBezmyoGn51311036 = -351399094;    float kvegdkzmUCvcBezmyoGn61731765 = -996655177;    float kvegdkzmUCvcBezmyoGn49829471 = 59535341;    float kvegdkzmUCvcBezmyoGn9970130 = 49734233;    float kvegdkzmUCvcBezmyoGn27612156 = -594445908;    float kvegdkzmUCvcBezmyoGn98351017 = -718604146;    float kvegdkzmUCvcBezmyoGn3295010 = -615010418;    float kvegdkzmUCvcBezmyoGn15069780 = -6726583;    float kvegdkzmUCvcBezmyoGn15348437 = -894559877;    float kvegdkzmUCvcBezmyoGn83900047 = -265873895;    float kvegdkzmUCvcBezmyoGn85667147 = -925543271;    float kvegdkzmUCvcBezmyoGn64502750 = -223712110;    float kvegdkzmUCvcBezmyoGn41001714 = -699283920;    float kvegdkzmUCvcBezmyoGn76504031 = -920907261;    float kvegdkzmUCvcBezmyoGn33608909 = -343141357;    float kvegdkzmUCvcBezmyoGn37685217 = -496655716;    float kvegdkzmUCvcBezmyoGn5217371 = -989682221;    float kvegdkzmUCvcBezmyoGn80341209 = -555242282;    float kvegdkzmUCvcBezmyoGn79684890 = -151711047;    float kvegdkzmUCvcBezmyoGn54014519 = -4276653;    float kvegdkzmUCvcBezmyoGn3463414 = -220163537;    float kvegdkzmUCvcBezmyoGn54996471 = -397578429;    float kvegdkzmUCvcBezmyoGn93742906 = -450535446;    float kvegdkzmUCvcBezmyoGn35091468 = -776096471;    float kvegdkzmUCvcBezmyoGn52328934 = -552691993;    float kvegdkzmUCvcBezmyoGn25772260 = -708001658;    float kvegdkzmUCvcBezmyoGn49593004 = -489165776;    float kvegdkzmUCvcBezmyoGn50464811 = -122783910;    float kvegdkzmUCvcBezmyoGn43126037 = -81132139;    float kvegdkzmUCvcBezmyoGn5410263 = -488231987;    float kvegdkzmUCvcBezmyoGn53751456 = -382346340;     kvegdkzmUCvcBezmyoGn68500163 = kvegdkzmUCvcBezmyoGn79364472;     kvegdkzmUCvcBezmyoGn79364472 = kvegdkzmUCvcBezmyoGn12341603;     kvegdkzmUCvcBezmyoGn12341603 = kvegdkzmUCvcBezmyoGn52388550;     kvegdkzmUCvcBezmyoGn52388550 = kvegdkzmUCvcBezmyoGn39926298;     kvegdkzmUCvcBezmyoGn39926298 = kvegdkzmUCvcBezmyoGn51040244;     kvegdkzmUCvcBezmyoGn51040244 = kvegdkzmUCvcBezmyoGn77144524;     kvegdkzmUCvcBezmyoGn77144524 = kvegdkzmUCvcBezmyoGn57988876;     kvegdkzmUCvcBezmyoGn57988876 = kvegdkzmUCvcBezmyoGn99753677;     kvegdkzmUCvcBezmyoGn99753677 = kvegdkzmUCvcBezmyoGn51203681;     kvegdkzmUCvcBezmyoGn51203681 = kvegdkzmUCvcBezmyoGn3634836;     kvegdkzmUCvcBezmyoGn3634836 = kvegdkzmUCvcBezmyoGn27703272;     kvegdkzmUCvcBezmyoGn27703272 = kvegdkzmUCvcBezmyoGn74924031;     kvegdkzmUCvcBezmyoGn74924031 = kvegdkzmUCvcBezmyoGn92198756;     kvegdkzmUCvcBezmyoGn92198756 = kvegdkzmUCvcBezmyoGn87106644;     kvegdkzmUCvcBezmyoGn87106644 = kvegdkzmUCvcBezmyoGn35700393;     kvegdkzmUCvcBezmyoGn35700393 = kvegdkzmUCvcBezmyoGn18279377;     kvegdkzmUCvcBezmyoGn18279377 = kvegdkzmUCvcBezmyoGn28679803;     kvegdkzmUCvcBezmyoGn28679803 = kvegdkzmUCvcBezmyoGn78032916;     kvegdkzmUCvcBezmyoGn78032916 = kvegdkzmUCvcBezmyoGn71041614;     kvegdkzmUCvcBezmyoGn71041614 = kvegdkzmUCvcBezmyoGn85941320;     kvegdkzmUCvcBezmyoGn85941320 = kvegdkzmUCvcBezmyoGn7495847;     kvegdkzmUCvcBezmyoGn7495847 = kvegdkzmUCvcBezmyoGn98565335;     kvegdkzmUCvcBezmyoGn98565335 = kvegdkzmUCvcBezmyoGn32115881;     kvegdkzmUCvcBezmyoGn32115881 = kvegdkzmUCvcBezmyoGn19030113;     kvegdkzmUCvcBezmyoGn19030113 = kvegdkzmUCvcBezmyoGn73545558;     kvegdkzmUCvcBezmyoGn73545558 = kvegdkzmUCvcBezmyoGn26391743;     kvegdkzmUCvcBezmyoGn26391743 = kvegdkzmUCvcBezmyoGn54046932;     kvegdkzmUCvcBezmyoGn54046932 = kvegdkzmUCvcBezmyoGn25807376;     kvegdkzmUCvcBezmyoGn25807376 = kvegdkzmUCvcBezmyoGn50453647;     kvegdkzmUCvcBezmyoGn50453647 = kvegdkzmUCvcBezmyoGn73397044;     kvegdkzmUCvcBezmyoGn73397044 = kvegdkzmUCvcBezmyoGn9170459;     kvegdkzmUCvcBezmyoGn9170459 = kvegdkzmUCvcBezmyoGn50490639;     kvegdkzmUCvcBezmyoGn50490639 = kvegdkzmUCvcBezmyoGn87469704;     kvegdkzmUCvcBezmyoGn87469704 = kvegdkzmUCvcBezmyoGn64467730;     kvegdkzmUCvcBezmyoGn64467730 = kvegdkzmUCvcBezmyoGn35795608;     kvegdkzmUCvcBezmyoGn35795608 = kvegdkzmUCvcBezmyoGn73968628;     kvegdkzmUCvcBezmyoGn73968628 = kvegdkzmUCvcBezmyoGn68449905;     kvegdkzmUCvcBezmyoGn68449905 = kvegdkzmUCvcBezmyoGn18709674;     kvegdkzmUCvcBezmyoGn18709674 = kvegdkzmUCvcBezmyoGn50420761;     kvegdkzmUCvcBezmyoGn50420761 = kvegdkzmUCvcBezmyoGn72690596;     kvegdkzmUCvcBezmyoGn72690596 = kvegdkzmUCvcBezmyoGn82646311;     kvegdkzmUCvcBezmyoGn82646311 = kvegdkzmUCvcBezmyoGn92426066;     kvegdkzmUCvcBezmyoGn92426066 = kvegdkzmUCvcBezmyoGn83216899;     kvegdkzmUCvcBezmyoGn83216899 = kvegdkzmUCvcBezmyoGn48215833;     kvegdkzmUCvcBezmyoGn48215833 = kvegdkzmUCvcBezmyoGn33362966;     kvegdkzmUCvcBezmyoGn33362966 = kvegdkzmUCvcBezmyoGn9042808;     kvegdkzmUCvcBezmyoGn9042808 = kvegdkzmUCvcBezmyoGn85390029;     kvegdkzmUCvcBezmyoGn85390029 = kvegdkzmUCvcBezmyoGn77542900;     kvegdkzmUCvcBezmyoGn77542900 = kvegdkzmUCvcBezmyoGn92198466;     kvegdkzmUCvcBezmyoGn92198466 = kvegdkzmUCvcBezmyoGn12768430;     kvegdkzmUCvcBezmyoGn12768430 = kvegdkzmUCvcBezmyoGn68179674;     kvegdkzmUCvcBezmyoGn68179674 = kvegdkzmUCvcBezmyoGn28829250;     kvegdkzmUCvcBezmyoGn28829250 = kvegdkzmUCvcBezmyoGn70805749;     kvegdkzmUCvcBezmyoGn70805749 = kvegdkzmUCvcBezmyoGn33455185;     kvegdkzmUCvcBezmyoGn33455185 = kvegdkzmUCvcBezmyoGn61004316;     kvegdkzmUCvcBezmyoGn61004316 = kvegdkzmUCvcBezmyoGn80799137;     kvegdkzmUCvcBezmyoGn80799137 = kvegdkzmUCvcBezmyoGn80225722;     kvegdkzmUCvcBezmyoGn80225722 = kvegdkzmUCvcBezmyoGn33358437;     kvegdkzmUCvcBezmyoGn33358437 = kvegdkzmUCvcBezmyoGn66380739;     kvegdkzmUCvcBezmyoGn66380739 = kvegdkzmUCvcBezmyoGn24648501;     kvegdkzmUCvcBezmyoGn24648501 = kvegdkzmUCvcBezmyoGn23097593;     kvegdkzmUCvcBezmyoGn23097593 = kvegdkzmUCvcBezmyoGn32181501;     kvegdkzmUCvcBezmyoGn32181501 = kvegdkzmUCvcBezmyoGn49300030;     kvegdkzmUCvcBezmyoGn49300030 = kvegdkzmUCvcBezmyoGn77806636;     kvegdkzmUCvcBezmyoGn77806636 = kvegdkzmUCvcBezmyoGn94464377;     kvegdkzmUCvcBezmyoGn94464377 = kvegdkzmUCvcBezmyoGn77212633;     kvegdkzmUCvcBezmyoGn77212633 = kvegdkzmUCvcBezmyoGn87454327;     kvegdkzmUCvcBezmyoGn87454327 = kvegdkzmUCvcBezmyoGn27731027;     kvegdkzmUCvcBezmyoGn27731027 = kvegdkzmUCvcBezmyoGn51311036;     kvegdkzmUCvcBezmyoGn51311036 = kvegdkzmUCvcBezmyoGn61731765;     kvegdkzmUCvcBezmyoGn61731765 = kvegdkzmUCvcBezmyoGn49829471;     kvegdkzmUCvcBezmyoGn49829471 = kvegdkzmUCvcBezmyoGn9970130;     kvegdkzmUCvcBezmyoGn9970130 = kvegdkzmUCvcBezmyoGn27612156;     kvegdkzmUCvcBezmyoGn27612156 = kvegdkzmUCvcBezmyoGn98351017;     kvegdkzmUCvcBezmyoGn98351017 = kvegdkzmUCvcBezmyoGn3295010;     kvegdkzmUCvcBezmyoGn3295010 = kvegdkzmUCvcBezmyoGn15069780;     kvegdkzmUCvcBezmyoGn15069780 = kvegdkzmUCvcBezmyoGn15348437;     kvegdkzmUCvcBezmyoGn15348437 = kvegdkzmUCvcBezmyoGn83900047;     kvegdkzmUCvcBezmyoGn83900047 = kvegdkzmUCvcBezmyoGn85667147;     kvegdkzmUCvcBezmyoGn85667147 = kvegdkzmUCvcBezmyoGn64502750;     kvegdkzmUCvcBezmyoGn64502750 = kvegdkzmUCvcBezmyoGn41001714;     kvegdkzmUCvcBezmyoGn41001714 = kvegdkzmUCvcBezmyoGn76504031;     kvegdkzmUCvcBezmyoGn76504031 = kvegdkzmUCvcBezmyoGn33608909;     kvegdkzmUCvcBezmyoGn33608909 = kvegdkzmUCvcBezmyoGn37685217;     kvegdkzmUCvcBezmyoGn37685217 = kvegdkzmUCvcBezmyoGn5217371;     kvegdkzmUCvcBezmyoGn5217371 = kvegdkzmUCvcBezmyoGn80341209;     kvegdkzmUCvcBezmyoGn80341209 = kvegdkzmUCvcBezmyoGn79684890;     kvegdkzmUCvcBezmyoGn79684890 = kvegdkzmUCvcBezmyoGn54014519;     kvegdkzmUCvcBezmyoGn54014519 = kvegdkzmUCvcBezmyoGn3463414;     kvegdkzmUCvcBezmyoGn3463414 = kvegdkzmUCvcBezmyoGn54996471;     kvegdkzmUCvcBezmyoGn54996471 = kvegdkzmUCvcBezmyoGn93742906;     kvegdkzmUCvcBezmyoGn93742906 = kvegdkzmUCvcBezmyoGn35091468;     kvegdkzmUCvcBezmyoGn35091468 = kvegdkzmUCvcBezmyoGn52328934;     kvegdkzmUCvcBezmyoGn52328934 = kvegdkzmUCvcBezmyoGn25772260;     kvegdkzmUCvcBezmyoGn25772260 = kvegdkzmUCvcBezmyoGn49593004;     kvegdkzmUCvcBezmyoGn49593004 = kvegdkzmUCvcBezmyoGn50464811;     kvegdkzmUCvcBezmyoGn50464811 = kvegdkzmUCvcBezmyoGn43126037;     kvegdkzmUCvcBezmyoGn43126037 = kvegdkzmUCvcBezmyoGn5410263;     kvegdkzmUCvcBezmyoGn5410263 = kvegdkzmUCvcBezmyoGn53751456;     kvegdkzmUCvcBezmyoGn53751456 = kvegdkzmUCvcBezmyoGn68500163;}
// Junk Finished
