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
			g_Render->Text(1, 1, Color(255, 255, 255, 255), g_Render->font.Defuse, std::asctime(std::localtime(&result)));
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
		}
	}
}