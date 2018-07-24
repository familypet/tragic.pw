#pragma once
#include "HookIncludes.h"
#include "Render.h"
#include "Interfaces.h"
#include <ctime>
#include "ESP.h"
#include "Listener.h"
std::vector<const char*> vistasmoke_mats =
{
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
};	
typedef void(__thiscall* paint_traverse_t)(PVOID, unsigned int, bool, bool);


bool once = true;
bool once1 = false;
int width1 = 0;
int height1 = 0;
void __fastcall hkPaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{

	int w, h;
	int centerW, centerh, topH;
	g_Engine->GetScreenSize(w, h);
	centerW = w / 2;


	static auto ofunc = hooks::panel.get_original<paint_traverse_t>(41);
	C_BaseEntity* local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (local != nullptr && local->IsAlive() && g_Options.Visuals.noscopeborder && !strcmp("HudZoom", g_Panel->GetName(vguiPanel)))
	{
		return;
	}
	ofunc(pPanels, vguiPanel, forceRepaint, allowForce);
	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;
	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)g_Panel->GetName(vguiPanel);
		if (strstr(szPanelName, "FocusOverlayPanel"))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			static auto linegoesthrusmoke = U::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");
			static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
			switch (g_Options.Visuals.Smoke)
			{
			case 0:
				for (auto mat_s : vistasmoke_mats)
				{
					IMaterial* mat = g_MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
				}
				break;
			case 1:
				for (auto mat_s : vistasmoke_mats)
				{
					IMaterial* mat = g_MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
				}
				*(int*)(smokecout) = 0;
				break;
			case 2:
				*(int*)(smokecout) = 0;
				break;
			default:
				break;
			}

			visuals::instance().OnPaintTraverse(local);

			auto m_flFlashDuration = NetVarManager->GetOffset("DT_CSPlayer", "m_flFlashDuration");
			auto m_flFlashMaxAlpha = NetVarManager->GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
			if (local != nullptr)
			{
				CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->GetActiveWeaponHandle());
				if (g_Options.Visuals.NoFlash)
				{
					*MakePtr(float*, local, m_flFlashDuration) = 0.f;
					*MakePtr(float*, local, m_flFlashMaxAlpha) = 0.f;
				}
				if (local && local->IsScoped() && g_Options.Visuals.noscopeborder && MiscFunctions::IsSniper(pWeapon))
				{
					int width = 0;
					int height = 0;
					g_Engine->GetScreenSize(width, height);

					int centerX = static_cast<int>(width * 0.5f);
					int centerY = static_cast<int>(height * 0.5f);
					g_Render->Line(0, centerY, width, centerY, Color(0, 0, 0, 255));
					g_Render->Line(centerX, 0, centerX, height, Color(0, 0, 0, 255));
				}
			}
		}
		if (!g_Engine->IsInGame()) {
			g_Options.Misc.NameChangerFix = false;
			g_Options.Misc.NameSpammer = 0;
			g_Options.Misc.NoName = false;
			g_Options.Misc.ChatSpamMode = false;
			g_Options.Visuals.LBYIndicator = false;
			g_Options.Visuals.SniperCrosshair = false;
		}

		/*if (!once1)
		{


		printf("csPlayerResource offset: %p", g_PlayerResource - reinterpret_cast<DWORD>(GetModuleHandleA("client.dll")));
		once1 = true;
		}*/
		if (g_Options.Visuals.Time)
		{
			std::time_t result = std::time(nullptr);
			g_Render->Text(1, 20, Color(g_Options.Colors.timexd), g_Render->font.Defuse, std::asctime(std::localtime(&result)));
		}
		if (g_Options.Visuals.LBYIndicator)
		{
			int width = 0;
			int height = 0;
			g_Engine->GetScreenSize(width, height);
			if (local->GetEyeAngles()->y - local->GetLowerBodyYaw() > 35)
			{
				g_Render->Textf(10, height / 2, Color(0, 255, 0, 255), g_Render->font.LBY, "LBY");
			}
			else
			{
				g_Render->Textf(10, height / 2, Color(255, 0, 0, 255), g_Render->font.LBY, "LBY");
			}
		}


		if (g_Options.Misc.Watermark)
		{
			static float rainbow;
			rainbow += 0.0005f;
			if (rainbow > 1.f) rainbow = 0.f;

			{
				static int u = 0;
				if (u <= 100)                 g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "              t");
				if (u > 100 && u <= 200)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "             tr");
				if (u > 200 && u <= 300)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "            tra");
				if (u > 300 && u <= 400)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "           trag");
				if (u > 400 && u <= 500)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "          tragi");
				if (u > 500 && u <= 600)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "         tragic");
				if (u > 600 && u <= 700)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "        tragic.");
				if (u > 700 && u <= 800)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "       tragic.p");
				if (u > 800 && u <= 900)      g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "      tragic.pw");
				if (u > 900 && u <= 1000)     g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "     tragic.pw ");
				if (u > 1000 && u <= 1100)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "    tragic.pw  ");
				if (u > 1100 && u <= 1200)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "  tragic.pw   ");;
				if (u > 1200 && u <= 1300)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, " tragic.pw    ");
				if (u > 1300 && u <= 1400)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "tragic.pw     ");
				if (u > 1400 && u <= 1500)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "ragic.pw      ");
				if (u > 1500 && u <= 1600)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "agic.pw       ");
				if (u > 1600 && u <= 1700)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "gic.pw        ");
				if (u > 1700 && u <= 1800)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "ic.pw         ");
				if (u > 1800 && u <= 1900)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "c.pw          ");
				if (u > 1900 && u <= 2000)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, ".pw           ");
				if (u > 2000 && u <= 2100)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "pw            ");
				if (u > 2100 && u <= 2200)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "w             ");
				if (u > 2200 && u <= 2300)    g_Render->Textf(1, 1, Color::FromHSB(rainbow, 1.f, 1.f), g_Render->font.Watermark, "              ");
				u += 2.5;
				if (u > 2300) u = 0;
			}
			{


				if (g_Options.Visuals.Warnings)
				{
					/*
					____   _____ _____
					/ __ \ / ____|  __ \
					| |  | | (___ | |  | |
					| |  | |\___ \| |  | |
					| |__| |____) | |__| |
					\____/|_____/|_____/

					*/
					///////////////////		Main Booleans	///////////////////
					bool RageTab = g_Options.Ragebot.MainSwitch;
					bool Aimbot = g_Options.Ragebot.Enabled;
					bool LegitTab = g_Options.LegitBot.MainSwitch;
					bool VisualsTab = g_Options.Visuals.Enabled;
		//	bool RageBacktrack = g_Options.Ragebot.rageBacktrack;
					bool LegitBacktrack = g_Options.LegitBot.backtrack;
			//		bool BacktrackAndRage = g_Options.Ragebot.rageBacktrack && g_Options.Ragebot.MainSwitch;
					bool BacktrackAndLegit = g_Options.LegitBot.backtrack && g_Options.LegitBot.MainSwitch;
	//				bool BacktrackDisabled = RageTab == false && LegitTab == false || RageBacktrack == false && LegitBacktrack == false || RageBacktrack && LegitTab && LegitBacktrack == false && RageTab == false || RageBacktrack == false && LegitTab == false && LegitBacktrack && RageTab;
					///////////////////		Anti-Aim Booleans	///////////////////
					bool PreAAs = g_Options.Ragebot.PreAAs;
					bool EnabledAntiAim = g_Options.Ragebot.EnabledAntiAim;
					bool BuilderAAs = g_Options.Ragebot.BuilderAAs;
				//	bool MoveAA = g_Options.Ragebot.YawFakeMoveEnabled;
					///////////////////////////////////////////////////////////////
					int RealYaw = g_Options.Ragebot.YawTrue;
					int FakeYaw = g_Options.Ragebot.YawFake;
					int MoveRealYaw = g_Options.Ragebot.YawTrueMove;
					int MoveFakeYaw = g_Options.Ragebot.YawFakeMove;
					///////////////////////////////////////////////////////////////

					///////////////////////////////////////////////////////////////

					///////////////////
					//// Positions ////
					///////////////////

					int MainXPos = 250;
					///////////////////
					int FovPosY = 45;
					int RagePosY = 60;
					int LegitPosY = 75;
					int VisualsPosY = 90;
					int AAPosY = 105;
					int MAAPosY = 120;
					int BacktrackPosY = 135;
					int GeneralYPos = 45;

					// Yaw True (real) Position //
					int YawTPosY = 180;
					// Yaw Fake Position //
					int YawFPosY = YawTPosY + 15;
					// Yaw True (real) Move Position //
					int YawTMPosY = YawFPosY + 15;
					// Yaw Fake Move Position //
					int YawFMPosY = YawTMPosY + 15;
					int Oof2 = 0;
					///////////////////////////////////////////////////////////////

					///////////////////////////////////////////////////////////////

					Color FovRed = { 255, 30, 30, 255 };
					Color FovOrange = { 255, 120, 0, 255 };
					Color FovGreen = { 30, 255, 30, 255 };
					Color FovWhite = { 255, 255, 255, 255 };
					int Oof = 0;

					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "FOV:");
					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Ragebot:");
					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Aimbot:");
					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Backtrack:");
					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "LegitBot:");
					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Visuals:");
					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Anti-Aim:");
					g_Render->Textf(MainXPos, GeneralYPos + (15 * Oof++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Move AA:");

					/////////////////////////////////////// FOV INFO ////////////////////////////////////////////////////////

					if (g_Options.Ragebot.FOV > 89.9)
					{
						g_Render->Textf(315, FovPosY, Color(FovRed), g_Render->font.Watermark, "%.0f", g_Options.Ragebot.FOV); // when > 180 = Red
					}
					if (g_Options.Ragebot.FOV < 89.9)
					{
						g_Render->Textf(315, FovPosY, Color(FovOrange), g_Render->font.Watermark, "%.0f", g_Options.Ragebot.FOV); // when > 90 = Orange
					}
					if (g_Options.Ragebot.FOV < 45.1)
					{
						g_Render->Textf(315, FovPosY, Color(FovGreen), g_Render->font.Watermark, "%.0f", g_Options.Ragebot.FOV); // when > 45 = Green
					}
					if (g_Options.Ragebot.FOV < 1)
					{
						g_Render->Textf(315, FovPosY, Color(FovWhite), g_Render->font.Watermark, "%.0f", g_Options.Ragebot.FOV); // when < 1 = White
					}

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// WAS GEMACHT AN DER DISABLE ENABLED KACKE AK

					if (RageTab)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (RageTab == false)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}

					if (Aimbot && RageTab)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (RageTab == false && Aimbot || RageTab == false && Aimbot == false || Aimbot == false)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}

				//	if (RageBacktrack && RageTab || LegitBacktrack && LegitTab)
				//	{
				//		g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
			//		}
				//	if (BacktrackDisabled)
				//	{
				//		g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
			//		}

					if (LegitTab)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (LegitTab == false)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}

					if (VisualsTab)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (VisualsTab == false)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}

					if (EnabledAntiAim)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (EnabledAntiAim == false)
					{
						g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}

				//	if (MoveAA)
				//	{
				//		g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
				//	}
				//	if (MoveAA == false)
				//	{
				//		g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
				//	}

					/*if (RageTab)
					{
					g_Render->Textf(315, 60 + (15* Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (g_Options.Ragebot.Enabled)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (LegitTab)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (VisualsTab)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (EnabledAntiAim && PreAAs || EnabledAntiAim && BuilderAAs)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (EnabledAntiAim && MoveAA)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}
					if (RageBacktrack && RageTab || LegitBacktrack && LegitTab)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 30, 30, 255), g_Render->font.Watermark, "Enabled!");
					}

					if (RageTab == false)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}
					if (LegitTab == false)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}
					if (VisualsTab == false)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}
					if (EnabledAntiAim == false || PreAAs == false && BuilderAAs == false)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}
					if (EnabledAntiAim == false || MoveAA == false)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}
					if (BacktrackDisabled)
					{
					g_Render->Textf(315, 60 + (15 * Oof2++), Color(255, 255, 255, 255), g_Render->font.Watermark, "Disabled");
					}*/

					/////////////////////////////////////// ANTI AIM INFO ////////////////////////////////////////////////////////

					switch (RealYaw)
					{
					case 0:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Off");
						break;
					case 1:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Sideways Jitter");
						break;
					case 2:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Slow Spin");
						break;
					case 3:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Fast Spin");
						break;
					case 4:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Backwards");
						break;
					case 5:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: LBY Break");
						break;
					case 6:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Back Jitter");
						break;
					case 7:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: LBY Jitter");
						break;
					case 8:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Random");
						break;
					case 9:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: 90/90 LBY");
						break;
					case 10:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Manual AA");
						break;
					case 11:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Manual AA Jitter");
						break;
					case 12:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: 165/90 LBY");
						break;
					case 13:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Sideways");
						break;
					case 14:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: 165/30 LBY");
						break;
					case 15:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: 165/30 LBY Jitter");
						break;
					case 16:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: 90/90 LBY Jitter");
						break;
					case 17:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Manual SMAC");
						break;
					case 18:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Sideways SMAC");
						break;
					case 19:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: 250/280 LBY Jitter");
						break;
					case 20:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Experimental");
						break;
					case 21:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: MasterLooser");
						break;
					case 22:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Experimental 2");
						break;
					case 23:
						g_Render->Textf(MainXPos, YawTPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Real Yaw: Custom");
						break;
					}

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////

					switch (FakeYaw)
					{
					case 0:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Off");
						break;
					case 1:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Sideways Jitter");
						break;
					case 2:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Slow Spin");
						break;
					case 3:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Fast Spin");
						break;
					case 4:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Backwards");
						break;
					case 5:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: LBY Break");
						break;
					case 6:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Forward Jitter");
						break;
					case 7:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Jitter");
						break;
					case 8:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: LBY Jitter");
						break;
					case 9:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Forward");
						break;
					case 10:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Random Fake");
						break;
					case 11:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Inversed AA Jitter");
						break;
					case 12:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Inversed AA");
						break;
					case 13:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Sideways");
						break;
					case 14:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Manual SMAC");
						break;
					case 15:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Sideways SMAC");
						break;
					case 16:
						g_Render->Textf(MainXPos, YawFPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Fake Yaw: Half Spin");
						break;
					}

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////

					switch (MoveRealYaw)
					{
					case 0:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Off");
						break;
					case 1:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Sideways Jitter");
						break;
					case 2:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Slow Spin");
						break;
					case 3:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Fast Spin");
						break;
					case 4:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Backwards");
						break;
					case 5:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Back Jitter");
						break;
					case 6:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Random");
						break;
					case 7:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Manual AA");
						break;
					case 8:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Manual AA Jitter");
						break;
					case 9:
						g_Render->Textf(MainXPos, YawTMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Real Yaw: Sideways");
						break;
					}

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////

					switch (MoveFakeYaw)
					{
					case 0:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Off");
						break;
					case 1:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Sideways Jitter");
						break;
					case 2:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Slow Spin");
						break;
					case 3:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Fast Spin");
						break;
					case 4:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Backwards");
						break;
					case 5:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Forward Jitter");
						break;
					case 6:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Jitter AA");
						break;
					case 7:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Random Fake");
						break;
					case 8:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Inversed AA Jitter");
						break;
					case 9:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Inversed AA");
						break;
					case 10:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Sideways");
						break;
					case 11:
						g_Render->Textf(MainXPos, YawFMPosY, Color(255, 255, 255, 255), g_Render->font.Watermark, "Move Fake Yaw: Forward");
						break;
					}
				}
			}
		}
	}
}
