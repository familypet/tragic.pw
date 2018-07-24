#pragma once
#include "HookIncludes.h"
#include "Misc.h"
#include "MovementRecorder.h"
#include "RageBot.h"
#include "LegitBot.h"
#include "GrenadePrediction.h"
#include "LagComp.h"
#include "ESP.h"
template<class T, class U>
T Clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}
using create_move_t = void(__thiscall *)(IBaseClientDLL *, int, float, bool);
QAngle qLastTickAngles = QAngle(0.0f, 0.0f, 0.0f);
void __stdcall CHLCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
{
	static auto ofunc = hooks::client.get_original<create_move_t>(22);
	ofunc(g_CHLClient, sequence_number, input_sample_frametime, active);
	C_BaseEntity *local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	CInput::CUserCmd *cmd = g_Input->GetUserCmd(0, sequence_number);
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle((HANDLE)local->GetActiveWeaponHandle());
	if (!cmd)
		return;
		if (g_Options.Visuals.ToggleKey)
		{

			if (GetKeyState(g_Options.Visuals.ToggleKey))
			{
				g_Options.Visuals.Filter.Players = true;
			//	g_Options.Visuals.Box = true;
			//	g_Options.Visuals.Weapon = true;
			}
			else
			{
				g_Options.Visuals.Filter.Players = false;
			//	g_Options.Visuals.Box = false;
			//	g_Options.Visuals.Weapon = false;
			}
		}
	static float next_time = 0;

	if (!Globals::ySwitch && local->IsAlive() && local->GetVelocity().Length2D() == 0)
	{

		float TickStuff = TICKS_TO_TIME(local->GetTickBase());
		Globals::shouldflip = false;
		bool moveFlip;
		bool checkmove;
		Globals::NextTime = next_time;

		if (next_time - TICKS_TO_TIME(local->GetTickBase()) > 1.1) next_time = 0;

		//if (local->GetVelocity().Length2D() > 1) moveFlip = true;

		if (local->GetVelocity().Length2D() == 0)
		{
			if (TickStuff > next_time + 1.1f) // 1.1
			{
				next_time = TickStuff + TICKS_TO_TIME(1);
				Globals::shouldflip = true;
			}
		}

	}

	if (!cmd->command_number)
		return;

	if (g_Options.Misc.crashpaste)
	{
		static bool steal = false;
		ConVar* nameConvar = g_CVar->FindVar(("name"));
		*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = NULL;
		if (nameConvar)
		{
			if (steal)
			{
				nameConvar->SetValue(("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------YOUR AYYWARE PASTE HAS CRASHED----------------------------"));
			}
			else
			{
				nameConvar->SetValue(("TRAGIC.PW"));
			}

			steal = !steal;
		}
	}
	if (g_Options.Misc.NameHide)
	{
		ConVar* nameConvar = g_CVar->FindVar("name");
		*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = NULL;
		nameConvar->SetValue("\n\xAD\xAD\xAD");//namesteal fix
	}
	if (g_Options.Misc.bunnyhop)
	{
		static bool bLastJumped = false;
		static bool bShouldFake = false;
	
		if (!bLastJumped && bShouldFake)
		{
			bShouldFake = false;
			cmd->buttons |= IN_JUMP;
		}
		else if (cmd->buttons & IN_JUMP)
		{
			if (pLocal->GetFlags() & FL_ONGROUND)
			{
				bLastJumped = true;
				bShouldFake = true;
			}
			else
			{
				cmd->buttons &= ~IN_JUMP;
				bLastJumped = false;
			}
		}
		else
		{
			bLastJumped = false;
			bShouldFake = false;
		}
	}
	if (g_Options.Misc.autostrafenew)
	{
		
		if (pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_LADDER) return;
		if ((pLocal->GetFlags() & FL_ONGROUND)) return;
		static float move = 650.f;
		float s_move = move * 0.5065f;

		cmd->forwardmove = move * 0.015f;
		cmd->sidemove += static_cast<float>(((cmd->tick_count % 2) * 2) - 1) * s_move;

		if (cmd->mousedx)
			cmd->sidemove = static_cast<float>(Clamp(cmd->mousedx, -1, 1)) * s_move;

		static auto strafe = cmd->viewangles.y;

		float rt = cmd->viewangles.y, rotation;
		rotation = strafe - rt;

		if (rotation < -g_Globals->interval_per_tick)
			cmd->sidemove = -s_move;

		if (rotation > g_Globals->interval_per_tick)
			cmd->sidemove = s_move;

		strafe = rt;
	}
	if (g_Options.Misc.rageautostrafe)
	{
		if (pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_LADDER) return;
		if ((pLocal->GetFlags() & FL_ONGROUND)) return;

		if (GetAsyncKeyState(0x57)//W 
			|| GetAsyncKeyState(0x41)//A
			|| GetAsyncKeyState(0x53)//S
			|| GetAsyncKeyState(0x44))//D
			return;

		if (GetAsyncKeyState(VK_SPACE) && (pLocal->GetFlags() & FL_ONGROUND)
			&& pLocal->GetVelocity().Length() <= 60.f)
		{
			cmd->forwardmove = 450.f;
		}
		auto Delta = 0.f;
		if (pLocal->GetVelocity().Length() > 60.f)
		{
			auto x = 45.f;
			auto z = x / pLocal->GetVelocity().Length();
			z = fabsf(z);
			auto a = x * z;
			Delta = a;
		}
		if (GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND))
		{
			if (cmd->mousedx < 0)
				cmd->sidemove = -450;
			if (cmd->mousedx > 0)
				cmd->sidemove = 450;
			if (cmd->tick_count % 2)
			{
				cmd->sidemove = -450;
			}
			else
			{
				cmd->viewangles.y += Delta;
				cmd->sidemove = 450;
			}
		}
	}
	if (g_Options.Misc.moonwalk)
	{
		if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP)
			return;

		if (cmd->buttons & IN_FORWARD)
		{
			cmd->forwardmove = 450;
			cmd->buttons &= ~IN_FORWARD;
			cmd->buttons |= IN_BACK;
		}
		else if (cmd->buttons & IN_BACK)
		{
			cmd->forwardmove = -450;
			cmd->buttons &= ~IN_BACK;
			cmd->buttons |= IN_FORWARD;
		}

		if (cmd->buttons & IN_MOVELEFT)
		{
			cmd->sidemove = -450;
			cmd->buttons &= ~IN_MOVELEFT;
			cmd->buttons |= IN_MOVERIGHT;
		}
		else if (cmd->buttons & IN_MOVERIGHT)
		{
			cmd->sidemove = 450;
			cmd->buttons &= ~IN_MOVERIGHT;
			cmd->buttons |= IN_MOVELEFT;
		}
	}
	if (g_Options.Misc.WalkBot)
	{
		bool walkbotBefore = false;


		QAngle viewangles;
		g_Engine->GetViewAngles(viewangles);

		auto fDistanceToWall = [&](Vector diff = Vector(0, 0, 0))->float
		{
			Vector src, dst, forward;
			trace_t tr;
			Ray_t ray;
			CTraceFilter filter;

			QAngle viewangle = cmd->viewangles;
			AngleVectors(viewangle, &forward);
			forward *= weapon->GetCSWpnData()->m_fRange;
			filter.pSkip = pLocal;
			src = pLocal->GetEyePosition();
			dst = src + forward;

			ray.Init(src, dst);

			g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
			return (src - tr.endpos).LengthSqr();
		};
		static float old1, old2, old3;
		if (pLocal->GetVelocity().Length() < 3)
		{
			viewangles.y += 15.f;
		}


		if (fDistanceToWall() < 110)
		{

			float negativeDist = fDistanceToWall(Vector(0, -1, 0)), positiveDist = fDistanceToWall(Vector(0, 1, 0));
			if (abs(negativeDist - positiveDist) < 1.0f)
			{

				viewangles.y += 15.f;
			}
			else
			{

				viewangles.y += positiveDist < negativeDist ? -1 : 1;

			}
		}
		cmd->viewangles.Clamp();

		g_Engine->SetViewAngles(viewangles);

		if (!walkbotBefore)
		{
			if (g_Options.Ragebot.Enabled)
				cmd->forwardmove = -450;
			else
				cmd->forwardmove = +450;

			walkbotBefore = true;
		}

		else if (walkbotBefore)
		{
			walkbotBefore = false;

			if (g_Options.Ragebot.Enabled)
				cmd->forwardmove = 450;
			else
				cmd->forwardmove = -450;
		}
	}
	if (g_Options.Misc.fakelag)
	{
		if (cmd->buttons & IN_ATTACK || g_Options.LegitBot.LegitAA)
			return;

		static int lagSwitch = -g_Options.Misc.fakelagsend;

		if (g_Options.Misc.fakelagchoke > 0 && g_Options.Misc.fakelagsend > 0)
		{
			if (lagSwitch < 0)
			{
				G::SendPacket = true;
			}
			else {
				if (lagSwitch >= g_Options.Misc.fakelagchoke)
				{
					lagSwitch = -g_Options.Misc.fakelagsend;
				}
				G::SendPacket = false;
			}
			lagSwitch++;
		}
	}
	CInput::CVerifiedUserCmd *verifiedCommands = *(CInput::CVerifiedUserCmd **)(reinterpret_cast< uint32_t >(g_Input) + 0xF0);
	CInput::CVerifiedUserCmd *verified = &verifiedCommands[sequence_number % 150];
	if (!verified)
		return;

	if (!local)
		return;
	QAngle oldAngle = cmd->viewangles;
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;

	backtracking->legitBackTrack(cmd, local);
	backtracking->RageBackTrack(cmd, local);

	RankReveal(cmd);

	misc::instance().OnCreateMove(cmd, local, bSendPacket);
	legitbot::instance().OnCreateMove(cmd, local, bSendPacket);
	ragebot::instance().OnCreateMove(cmd, bSendPacket);


	if (GetAsyncKeyState(g_Options.Misc.iFreeCamKey) && g_Options.Misc.bFreeCam)
	{
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
	}

	grenade_prediction::instance().Tick(cmd->buttons);
	if (!bSendPacket && g_Options.Visuals.antiaim_thirdperson_angle == ANTIAIM_THIRDPERSON_REAL)  qLastTickAngles = cmd->viewangles;
	else if (bSendPacket && g_Options.Visuals.antiaim_thirdperson_angle == ANTIAIM_THIRDPERSON_FAKE) qLastTickAngles = cmd->viewangles;
	if (!bSendPacket && g_Options.Ragebot.YawFake) qLastTickAngles = cmd->viewangles;
	else if (bSendPacket && !g_Options.Ragebot.YawFake) qLastTickAngles = cmd->viewangles;

	if (g_Options.Misc.antiuntrusted)
		if (!sanitize_angles(cmd->viewangles))
			return;

	movementfix(oldAngle, cmd);
	MovementRecorder(cmd);

	if (g_Options.Misc.antiuntrusted)
		clampMovement(cmd);

	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum();


	if (Globals::ySwitch) {
		Globals::FakeAngle = cmd->viewangles.y;
	}
	else {
		Globals::RealAngle = cmd->viewangles.y;
	}
}
#pragma warning(disable : 4409)
__declspec(naked) void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		PUSH	EBP
		MOV		EBP, ESP
		PUSH	EBX
		LEA		ECX, [ESP]
		PUSH	ECX
		PUSH	active
		PUSH	input_sample_frametime
		PUSH	sequence_number
		CALL	CHLCreateMove
		POP		EBX
		POP		EBP
		RETN	0xC
	}
}