#pragma once
#include "HookIncludes.h"

typedef void(__thiscall *override_view_t)(void* _this, CViewSetup* setup);
typedef float(__thiscall *get_fov_t)(void*);



float __fastcall hkGetViewModelFOV(void* ecx, void* edx)
{
	static auto ofunc = hooks::clientmode.get_original<get_fov_t>(35);
	float viewmodelFOV = ofunc(ecx);
	return g_Options.Visuals.viewmodelChanger;
}

void __fastcall hkOverrideView(void* _this, void* _edx, CViewSetup* setup)
{
	static auto ofunc = hooks::clientmode.get_original<override_view_t>(18);
	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (pLocal && g_Engine->IsInGame())
	{
		/*if (g_Options.Visuals.NoVisualRecoil)
		{
		setup->angles.x -= ((pLocal->localPlayerExclusive()->GetViewPunchAngle().x * 2) * 0.4000000f);
		setup->angles.y -= ((pLocal->localPlayerExclusive()->GetViewPunchAngle().y * 2) * 0.4000000f);


		}*/ //FOV OVERRIDES| !pLocal->IsScoped()
		if (!pLocal->IsScoped() && g_Options.Visuals.Enabled || g_Options.Visuals.OverrideScopedFOV)
			setup->fov += g_Options.Visuals.FOVChanger;
	}
	if (pLocal && g_Engine->IsInGame())
	{
		if (g_Options.Visuals.NoVisualRecoil)
		{
			setup->angles.x -= ((pLocal->localPlayerExclusive()->GetViewPunchAngle().x * 2) * 0.45);
			setup->angles.y -= ((pLocal->localPlayerExclusive()->GetViewPunchAngle().y * 2) * 0.45);
		}
		if (!pLocal->IsScoped())
			setup->fov += g_Options.Visuals.FOVChanger;

		static bool enabledtp = false;
		static bool check = false;
		if (GetAsyncKeyState(g_Options.Visuals.TPKey))
		{
			if (!check)
				enabledtp = !enabledtp;
			check = true;
		}
		else
			check = false;

		if (g_Options.Visuals.ThirdPerson && pLocal->IsAlive() && enabledtp)
		{
			if (!g_Input->m_fCameraInThirdPerson)
			{
				g_Input->m_fCameraInThirdPerson = true;
			}
			g_Input->m_vecCameraOffset.z = g_Options.Visuals.tpdist;
		}
		else
		{
			g_Input->m_fCameraInThirdPerson = false;
		}
	}
	if (g_Options.Visuals.SniperCrosshair) 
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
			static auto debugspread = g_CVar->FindVar("weapon_debug_spread_show");

			if (MiscFunctions::IsSniper(pWeapon))
				debugspread->SetValue(3);
			else
				debugspread->SetValue(0);
		}
	}

	if (g_Options.Visuals.InverseView)
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			setup->angles += 180;
			g_Engine->ClientCmd_Unrestricted("crosshair 0");
		}
	}
	else
		g_Engine->ClientCmd_Unrestricted("crosshair 1");
	grenade_prediction::instance().View(setup);
	ofunc(_this, setup);
	{
		if (g_Options.Visuals.SniperCrosshair) {
			if (g_Engine->IsConnected() && g_Engine->IsInGame())
			{
				CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
				static auto debugspread = g_CVar->FindVar("weapon_debug_spread_show");

				if (MiscFunctions::IsSniper(pWeapon))
					debugspread->SetValue(3);
				else
					debugspread->SetValue(0);
			}
		}

		if (g_Options.Visuals.InverseView)
		{
			setup->angles += 180;
			g_Engine->ClientCmd_Unrestricted("crosshair 0");
		}
		else
			g_Engine->ClientCmd_Unrestricted("crosshair 1");

		//if (g_Options.Visuals.NoVisualRecoil)
		//{
		//setup->angles -= pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2;



		if (g_Options.Misc.bFreeCam)
		{
			static Vector newOrigin;

			int freecamkey = g_Options.Misc.iFreeCamKey;
			float freecamspeed = g_Options.Misc.flFreeCamSpeed;


			if (!GetAsyncKeyState(freecamkey))
			{
				newOrigin = setup->origin;
			}
			if (GetAsyncKeyState(freecamkey))
			{

				unsigned int fSpeed = freecamspeed; //5.f;
				if (GetAsyncKeyState(VK_CONTROL))
					fSpeed = fSpeed * 0.45;
				if (GetAsyncKeyState(VK_SHIFT))
					fSpeed = fSpeed * 1.65;
				if (GetAsyncKeyState(0x57)) // W		
				{
					newOrigin += setup->angles.Forward() * fSpeed;
				}
				if (GetAsyncKeyState(0x41)) // A		
				{
					newOrigin += setup->angles.Right() * fSpeed;
				}
				if (GetAsyncKeyState(0x44)) // D		
				{
					newOrigin -= setup->angles.Right() * fSpeed;
				}
				if (GetAsyncKeyState(0x53)) // S		
				{
					newOrigin -= setup->angles.Forward() * fSpeed;

				}
				if (GetAsyncKeyState(VK_SPACE)) // Space		
				{
					newOrigin += setup->angles.Up() * fSpeed;
				}
				setup->origin = newOrigin;
			}
		}
	}
}
