#pragma once
#include "HookIncludes.h"
#include "../PerfectHook/playerlist.h"
#include "../PerfectHook/LBYBreaking.h"

POINT Mouse;
static int iShoots;
static int shots_fired = 0;
static float LatestLowerBodyYawUpdateTime[55];

int GetEstimatedServerTickCount(float latency)
{
	return (int)floorf((float)((float)(latency) / (float)((uintptr_t)&g_Globals->interval_per_tick)) + 0.5) + 1 + (int)((uintptr_t)&g_Globals->tickcount);
}
POINT GetMouse()
{
	return Mouse;
}
float oldlowerbodyyaw = 0;
bool Lbywithin35(C_BaseEntity* pEntity) {

	if ((pEntity->GetLowerBodyYaw() - 36 < pEntity->GetEyeAngles()->y && pEntity->GetLowerBodyYaw() + 15 > pEntity->GetEyeAngles()->y) || (pEntity->GetLowerBodyYaw() + 36 > pEntity->GetEyeAngles()->y && pEntity->GetLowerBodyYaw() - 15 < pEntity->GetEyeAngles()->y))
		return true;
	else
		return false;

}


bool HasFakeHead(C_BaseEntity* pEntity) {

	return abs(pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw()) > 35;
}

float override_yaw(C_BaseEntity* player, C_BaseEntity* local) {
	Vector eye_pos, pos_enemy;
	CalcAngle(player->GetEyePosition(), local->GetEyePosition(), eye_pos);
	if (g_Render->TransformScreen(player->GetOrigin(), pos_enemy)) {
		if (GetMouse().x < pos_enemy.x)
			return (eye_pos.y - 90);
		else if (GetMouse().x > pos_enemy.x)
			return (eye_pos.y + 90);
	}
	return eye_pos.y + 180;
}

inline float ClampYaw(float yaw) {
	while (yaw > 180.f)
		yaw -= 360.f;
	while (yaw < -180.f)
		yaw += 360.f;
	return yaw;
}
float GetCurtime()
{
	auto local_player = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (!local_player)
		return 0.f;

	return local_player->GetTickBase() * g_Globals->interval_per_tick;
}
namespace Globals
{
	int Shots;
	int missedshots;
	float RealAngle;
	float FakeAngle;
	Vector AimPoint;
	bool shouldflip;
	bool ySwitch;
	float NextTime;
	int resolvemode = 1;
	float fakeAngle;
	float OldSimulationTime[65];
	bool error;
	bool pFakewalk = false;
	int fired;
	int hit;
}
void LowerBodyYawFix(Vector* & Angle, PlayerList::CPlayer* Player)
{
	if (Player->entity->GetVelocity().Length() > 1 && (Player->entity->GetFlags() & Player->entity->GetFlags() & FL_ONGROUND))
		Angle->y = Player->entity->GetLowerBodyYaw();
}
static inline bool IsNearEqual(float v1, float v2, float Tolerance)
{
	return std::abs(v1 - v2) <= std::abs(Tolerance);
}
void LBYResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{
		Vector* angs = Player->entity->GetEyeAngles();
		if (angs->x > 90.0f && angs->x < 270.f)
		{
			if (angs->x > 180.f)
				angs->x = -89.f;
			else
				angs->x = 89.0f;
		}
		//angs->y = spin;
	LowerBodyYawFix(angs, Player);

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}

		if (Player->entity->GetVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
		Vector* angs = Player->entity->GetEyeAngles();
		LowerBodyYawFix(angs, Player);
	}
}
void ShootsResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}

		if (iShoots > 11)
			iShoots = 1;


		switch (iShoots) {
		case 1:flResolve = flResolve; break;
		case 2:flResolve += 0; break;
		case 3:flResolve = flResolve + 180; break;
		case 4:flResolve += 15; break;
		case 5:flResolve = flResolve + 90; break;
		case 6:flResolve -= 15; break;
		case 7:flResolve = flResolve + 270; break;
		case 8:flResolve += 0; break;
		case 9:flResolve = flResolve + 180; break;
		case 10:flResolve -= 45; break;
		case 11:flResolve += 45; break;
		}

		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void ForceResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}

		int value = rand() % 11;
		switch (value) {
		case 0:flResolve = flResolve; break;
		case 1:flResolve += 0; break;
		case 2:flResolve = flResolve + 180; break;
		case 3:flResolve += 15; break;
		case 4:flResolve = flResolve + 90; break;
		case 5:flResolve -= 15; break;
		case 6:flResolve = flResolve + 270; break;
		case 7:flResolve += 0; break;
		case 8:flResolve = flResolve + 180; break;
		case 9:flResolve -= 45; break;
		case 10:flResolve += 45; break;
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void AutoResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (flLowerBodyYaw != Player->flLastPelvisAng)
		{
			if (Player->entity->GetVelocity().Length2D() == 0)
			{
				int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
				while (temp < 0)
					temp += 360;
				while (temp > 360)
					temp -= 360;
				Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
			}

			iLastUpdatedTick = 0;
			Player->flLastPelvisAng = flLowerBodyYaw;
			Player->flEyeAng = orginalview.y;
		}

		if (Player->entity->GetVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			flResolve = Player->flLastPelvisAng;
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void SecAutoResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}

		int lastpelvisang = 0;
		float difference = 0;
		static bool jitter = false;
		float pelvisview = Player->entity->GetLowerBodyYaw();
		Vector angles = Vector(0, 0, 0);
		if (g_Engine->GetLocalPlayer())
			CalcAngle(Player->entity->GetEyePosition(), Player->entity->GetOrigin(), angles);
		if (lastpelvisang != pelvisview) {
			difference = flResolve - pelvisview;
			lastpelvisang = (int)pelvisview;
		}
		flResolve = flResolve - difference;

		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void RandResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}

		int value = rand() % 3 + 0;
		switch (value)
		{
		case 0:
		{
			flResolve = (rand() % 180);
			break;
		}
		case 1:
		{
			flResolve = (rand() % 360);
			break;
		}
		case 2:
		{
			flResolve = 0;
			break;
		}
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void RandShootsResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}

		if (iShoots > 3)
			iShoots = 1;


		switch (iShoots)
		{
		case 1:
		{
			flResolve = (rand() % 180);
			break;
		}
		case 2:
		{
			flResolve = (rand() % 360);
			break;
		}
		case 3:
		{
			flResolve = 0;
			break;
		}
		}

		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void MathResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		static float TimedYaw;

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}


		TimedYaw = Player->entity->GetEyeAngles()->y - Player->entity->GetLowerBodyYaw();
		flResolve = abs(TimedYaw);

		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void LBYInverseResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		static float TimedYaw;

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}


		if (Player->entity->GetVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw + 180;
		}

		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void Smart1Resolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		static float TimedYaw;

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (flLowerBodyYaw != Player->flLastPelvisAng)
		{
			if (Player->entity->GetVelocity().Length2D() == 0)
			{
				int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
				while (temp < 0)
					temp += 360;
				while (temp > 360)
					temp -= 360;
				Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;

				TimedYaw = Player->entity->GetEyeAngles()->y - Player->entity->GetLowerBodyYaw();
				flResolve = abs(TimedYaw);
			}

			iLastUpdatedTick = 0;
			Player->flLastPelvisAng = flLowerBodyYaw;
			Player->flEyeAng = orginalview.y;
		}

		if (Player->entity->GetVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			int temp = static_cast<int>(floor(orginalview.y - flLowerBodyYaw));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			flResolve = Player->Backtrack[temp] + orginalview.y;

			TimedYaw = Player->entity->GetEyeAngles()->y - Player->entity->GetLowerBodyYaw();
			flResolve = abs(TimedYaw);
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void Smart2Resolver(Vector* & Angle, PlayerList::CPlayer* Player)
{

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		static float TimedYaw;

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (iShoots > 11)
			iShoots = 1;

		if (flLowerBodyYaw != Player->flLastPelvisAng)
		{
			if (Player->entity->GetVelocity().Length2D() == 0)
			{
				int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
				while (temp < 0)
					temp += 360;
				while (temp > 360)
					temp -= 360;
				Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;

				switch (iShoots) {
				case 1:flResolve = flResolve; break;
				case 2:flResolve += 0; break;
				case 3:flResolve = flResolve + 180; break;
				case 4:flResolve += 15; break;
				case 5:flResolve = flResolve + 90; break;
				case 6:flResolve -= 15; break;
				case 7:flResolve = flResolve + 270; break;
				case 8:flResolve += 0; break;
				case 9:flResolve = flResolve + 180; break;
				case 10:flResolve -= 45; break;
				case 11:flResolve += 45; break;
				}
			}

			iLastUpdatedTick = 0;
			Player->flLastPelvisAng = flLowerBodyYaw;
			Player->flEyeAng = orginalview.y;
		}

		if (Player->entity->GetVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			int temp = static_cast<int>(floor(orginalview.y - flLowerBodyYaw));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			flResolve = Player->Backtrack[temp] + orginalview.y;

			switch (iShoots) {
			case 1:flResolve = flResolve; break;
			case 2:flResolve += 0; break;
			case 3:flResolve = flResolve + 180; break;
			case 4:flResolve += 15; break;
			case 5:flResolve = flResolve + 90; break;
			case 6:flResolve -= 15; break;
			case 7:flResolve = flResolve + 270; break;
			case 8:flResolve += 0; break;
			case 9:flResolve = flResolve + 180; break;
			case 10:flResolve -= 45; break;
			case 11:flResolve += 45; break;
			}
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void AutoResolver2(Vector* & Angle, PlayerList::CPlayer* Player)
{

	bool MeetsLBYReq;

	if (Player->entity->GetFlags() & FL_ONGROUND)
		MeetsLBYReq = true;
	else
		MeetsLBYReq = false;

	static int UpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		static float TimedYaw;

		int difference = orginalview.y - flLowerBodyYaw;

		UpdatedTick++;


		static int AimWare = 0;
		if (AimWare > 2399)
			AimWare = 1;

		int ResolverCase = AimWare / 200;


		int Resolve = 1;
		if (ResolverCase < 2 && ResolverCase > 0)
			Resolve = 1;
		else if (ResolverCase < 3 && ResolverCase > 1)
			Resolve = 2;
		else if (ResolverCase < 4 && ResolverCase > 2)
			Resolve = 3;
		else if (ResolverCase < 5 && ResolverCase > 3)
			Resolve = 4;
		else if (ResolverCase < 6 && ResolverCase > 4)
			Resolve = 5;
		else if (ResolverCase < 7 && ResolverCase > 5)
			Resolve = 6;
		else if (ResolverCase < 8 && ResolverCase > 6)
			Resolve = 7;
		else if (ResolverCase < 9 && ResolverCase > 7)
			Resolve = 8;
		else if (ResolverCase < 10 && ResolverCase > 8)
			Resolve = 9;
		else if (ResolverCase < 11 && ResolverCase > 9)
			Resolve = 10;
		else if (ResolverCase < 12 && ResolverCase > 10)
			Resolve = 11;
		else if (ResolverCase < 13 && ResolverCase > 11)
			Resolve = 1;

		AimWare++;



		if (flLowerBodyYaw != Player->flLastPelvisAng)
		{
			if (Player->entity->GetVelocity().Length2D() == 0)
			{
				int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
				while (temp < 0)
					temp += 360;
				while (temp > 360)
					temp -= 360;




				switch (Resolve) {


				case 1:flResolve = flResolve; break;
				case 2:flResolve += 0; break;
				case 3:flResolve = flResolve + 180; break;
				case 4:flResolve += 15; break;
				case 5:flResolve = flResolve + 90; break;
				case 6:flResolve -= 15; break;
				case 7:flResolve = flResolve + 270; break;
				case 8:flResolve += 0; break;
				case 9:flResolve = flResolve + 180; break;
				case 10:flResolve -= 45; break;
				case 11:flResolve += 45; break;

				}

				Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng - flResolve;

			}

			UpdatedTick = 0;
			Player->flLastPelvisAng = flLowerBodyYaw;
			Player->flEyeAng = orginalview.y;
		}

		if (Player->entity->GetVelocity().Length2D() >= 10)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			int temp = static_cast<int>(floor(orginalview.y - flLowerBodyYaw));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			flResolve = Player->Backtrack[temp] + orginalview.y;


			switch (Resolve) {
			case 1:flResolve = flResolve; break;
			case 2:flResolve += 0; break;
			case 3:flResolve = flResolve + 180; break;
			case 4:flResolve += 15; break;
			case 5:flResolve = flResolve + 90; break;
			case 6:flResolve -= 15; break;
			case 7:flResolve = flResolve + 270; break;
			case 8:flResolve += 0; break;
			case 9:flResolve = flResolve + 180; break;
			case 10:flResolve -= 45; break;
			case 11:flResolve += 45; break;

			}

		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
	Vector* angs = Player->entity->GetEyeAngles();
	LowerBodyYawFix(angs, Player);
}
void AutoResolvergg(Vector* & Angle, PlayerList::CPlayer* Player)
{
	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (flLowerBodyYaw != Player->flLastPelvisAng)
		{
			if (Player->entity->GetVelocity().Length2D() == 0)
			{
				int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
				while (temp < 0)
					temp += 360;
				while (temp > 360)
					temp -= 360;
				Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
			}

			iLastUpdatedTick = 0;
			Player->flLastPelvisAng = flLowerBodyYaw;
			Player->flEyeAng = orginalview.y;
		}

		if (Player->entity->GetVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			int temp = static_cast<int>(floor(orginalview.y - flLowerBodyYaw));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			flResolve = Player->Backtrack[temp] + orginalview.y;
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void Resolver1(Vector* & Angle, PlayerList::CPlayer* Player)
{
	C_BaseEntity *local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	int tick = local->GetTickBase();
	float flServerTime = (tick * g_Globals->interval_per_tick);
	float LastLBYUpdateTime = flServerTime - LBYMod::GetOutgoingLatency();
	bool lbyupdate;
	if (((LastLBYUpdateTime + 1.125 - LBYMod::GetOutgoingLatency()) <= flServerTime) && (local->GetFlags() & FL_ONGROUND))
	{
		LastLBYUpdateTime += 1.125;
		lbyupdate = true;
	}

	lbyupdate = false;

	Vector* eYaw = Player->entity->GetEyeAngles();
	float flResolveyaw;

	float lastLBY[64];
	float lbyDif[64];

	float curLbyYaw = Player->entity->GetLowerBodyYaw();

	if (Player->entity->GetVelocity().Length2D() > 0)
		eYaw->y = Player->entity->GetLowerBodyYaw();
	else
	{
		if (!IsNearEqual(lastLBY[Player->entity->GetIndex()], curLbyYaw, 15.f))
		{
			lbyDif[Player->entity->GetIndex()] = curLbyYaw - eYaw->y;
			lastLBY[Player->entity->GetIndex()] = curLbyYaw;
			eYaw->y += Player->entity->GetLowerBodyYaw();
		}
		else
		{
			eYaw->y += lbyDif[Player->entity->GetIndex()];
		}
	}


	Vector* angs = Player->entity->GetEyeAngles();
	Player->resolved = eYaw->y;
	AutoResolvergg(angs, Player);

}
void Ekzi_Resolver(Vector* & Angle, PlayerList::CPlayer* Player)
{
	static int LastResolved;
	static float ResolvedAngles;
	float LBY = Player->entity->GetLowerBodyYaw();
	static float last_lby;
	ResolvedAngles = Player->entity->GetEyeAngles()->y;

	if (ResolvedAngles == LastResolved)
		return;

	if (shots_fired >= 1)
	{
		Angle->x = 0;
		ResolvedAngles += 360;

		if (shots_fired > 2)
		{
			Angle->x = 89;
			shots_fired = 0;
			ResolvedAngles -= 90;
		}
	}
	else
	{
		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int v1 = Player->entity->GetEyeAngles()->y - Player->entity->GetLowerBodyYaw();
			ResolvedAngles = abs(v1);
		}
		else
		{
			ResolvedAngles = LBY;
		}
	}
	last_lby = LBY;
	LastResolved = ResolvedAngles;
	Angle->y = ResolvedAngles;
}
void AutoResolver2XGG(Vector* & Angle, PlayerList::CPlayer* Player)
{
	static int shoots;
	if (true)
		shoots++;

	if (Player->entity->GetVelocity().Length2D() == 0)
		Angle->y = Player->entity->GetLowerBodyYaw();
	else
	{
		static float OldLowerBodyYaws;
		static float OldYawDeltas;
		Angle->y = Player->entity->GetLowerBodyYaw();
		float CurYaw = Player->entity->GetLowerBodyYaw();
		if (OldLowerBodyYaws != CurYaw) {
			OldYawDeltas = Angle->y - CurYaw;
			OldLowerBodyYaws = CurYaw;
			Angle->y = CurYaw;
			return;
		}
		else
			Angle->y -= OldYawDeltas;
	}
}
void Resolver(Vector* & Angle, PlayerList::CPlayer* Player)
{
	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if (orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->GetLowerBodyYaw();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (Player->entity->GetVelocity().Length2D() == 0)
		{
			int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
		}

		if (iShoots > 11)
			iShoots = 1;


		switch (iShoots) {
		case 0:flResolve = flResolve; break;
		case 1:flResolve += 0; break;
		case 2:flResolve = flResolve + 180; break;
		case 3:flResolve += 15; break;
		case 4:flResolve = flResolve + 90; break;
		case 5:flResolve -= 15; break;
		case 6:flResolve = flResolve + 270; break;
		case 7:flResolve += 0; break;
		case 8:flResolve = flResolve + 180; break;
		case 9:flResolve -= 45; break;
		case 10:flResolve += 45; break;
		}

		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}
void Resolver2(Vector* & Angle, PlayerList::CPlayer* Player)
{
	if (Player->entity->GetVelocity().Length2D() == 0)
	{
		Angle->y = Player->entity->GetLowerBodyYaw();
		float CurYaw = Player->entity->GetLowerBodyYaw();
		if (Player->OldLowerBodyYaws != CurYaw)
		{
			Player->OldYawDeltas = Angle->y - CurYaw;
			Player->OldLowerBodyYaws = CurYaw;
			Angle->y = CurYaw;
			return;
		}
		else
			Angle->y -= Player->OldYawDeltas;
	}
	else
		Angle->y = Player->entity->GetLowerBodyYaw();
}

bool IsAntiAiming(float* angle, float* angle2)
{
	if (*angle = 88.f)
		return true;
	else if (*angle = -88.f)
		return true;
	else if (*angle = 89.f)
		return true;
	else if (*angle = -89.f)
		return true;
	else if (*angle > 89.f)
		return true;
	else if (*angle < -89.f)
		return true;

	if (*angle2 > 180.f)
		return true;
	else if (*angle2 < -180.f)
		return true;

	return false;
}
void LbyCorrect()
{
	{
		{
			C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
			for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
			{
				C_BaseEntity* pEnt = g_EntityList->GetClientEntity(i);
				if (!pEnt) continue;

				if (pEnt == pLocal) return;

				if (pEnt->IsDormant()) continue;

				player_info_t pTemp;

				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;

				float Janusz = 1000 + 2 * 9;
				float over9000 = Janusz + 20;
				float MatkaMichalka = 1337;
				float Michalek = MatkaMichalka / 7;
				float OjciecMichalka = over9000;
				float ShootsToResolve = 1;

				switch (pEnt->m_iShotsFired() % 2)
				{
				case 1: ShootsToResolve += 0.5f; break;
				case 2: ShootsToResolve += 0.5f; break;
				}


				if (pEnt->GetEyeAngles()->y != pEnt->GetLowerBodyYaw()) {
					pEnt->GetEyeAngles()->y / pEnt->GetEyeAngles()->x - Michalek;
				}
				else if (pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw()) {
					Janusz++;
					MatkaMichalka++;

					switch (pEnt->m_iShotsFired() % 4)
					{
					case 1: pEnt->GetEyeAngles()->y = OjciecMichalka + 1939; break;
					case 2: pEnt->GetEyeAngles()->y = OjciecMichalka - 1939; break;
					case 3: pEnt->GetEyeAngles()->y = MatkaMichalka / 1939; break;
					case 4: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() / ShootsToResolve; break;
					}

				}
			}
		}
	}
}



void Resolver()
{

	if (g_Options.Ragebot.Resolver == 1)//pasted
	{
		{
			{
				C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
				for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
				{
					C_BaseEntity* pEnt = g_EntityList->GetClientEntity(i);
					if (!pEnt) continue;

					if (pEnt == pLocal) continue;

					if (pEnt->IsDormant()) continue;

					player_info_t pTemp;

					if (!g_Engine->GetPlayerInfo(i, &pTemp))
						continue;


					if (pEnt->GetVelocity().Length2D() > 36)
					{
						pEnt->GetEyeAngles()->y -= 10.f;
					}
					else
					{
						switch (pEnt->m_iShotsFired() % 4)
						{
						case 1:
							pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() * 7;
							break;
						case 2:
							pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + 11;
							break;
						case 3:
							pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() - 25;
							break;
						case 4:
							pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + 90;
							break;

						}

					}
				}
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 2)//pasted
	{
		{
			{
				C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
				for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
				{
					C_BaseEntity* pEnt = g_EntityList->GetClientEntity(i);

					if (!pEnt) return;

					if (pEnt == pLocal) continue;

					if (pEnt->IsDormant()) continue;

					player_info_t pTemp;

					if (!g_Engine->GetPlayerInfo(i, &pTemp))
						continue;

					switch (pEnt->m_iShotsFired() % 5) {
					case 1: pEnt->GetEyeAngles()->y * pEnt->GetLowerBodyYaw() / 2 * 1 - 20; break;
					case 2: pEnt->GetEyeAngles()->y -= pEnt->GetLowerBodyYaw(); break;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw(); break;
					case 4: pEnt->GetEyeAngles()->y += pEnt->GetLowerBodyYaw(); break;
					case 5: pEnt->GetEyeAngles()->y -= pEnt->GetLowerBodyYaw() - 1 + 2 - 11 + 12 - 6 - 6 / 2; break;
					}

				}
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 3)//pasted
	{
		{
			{
				C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
				for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
				{
					C_BaseEntity* pEnt = g_EntityList->GetClientEntity(i);

					if (!pEnt) continue;

					if (pEnt == pLocal) continue;

					if (pEnt->IsDormant()) continue;

					player_info_t pTemp;

					if (!g_Engine->GetPlayerInfo(i, &pTemp))
						continue;

					if (pEnt->GetEyeAngles()->y != pEnt->GetLowerBodyYaw())
						pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw();

				}
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 4)//pasted
	{
		{
			{
				C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
				for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
				{
					C_BaseEntity* pEnt = g_EntityList->GetClientEntity(i);

					float complex = g_Globals->curtime;

					float meme = 0.f;
					meme++;
					if (meme > 0.5)
						meme = 0.f;


					if (!pEnt) continue;

					if (pEnt == pLocal) continue;

					if (pEnt->IsDormant()) continue;

					player_info_t pTemp;

					if (!g_Engine->GetPlayerInfo(i, &pTemp))
						continue;

					if (pEnt->GetEyeAngles()->y >= 35 || pEnt->GetEyeAngles()->y >= pEnt->GetLowerBodyYaw()) {
						pEnt->GetEyeAngles()->y -= complex;
						meme--;
					}
					else {
						pEnt->GetEyeAngles()->y = meme;
						meme++;
					}

					//return;


				}
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 5) { //pasted
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
			{
				C_BaseEntity* pEnt = g_EntityList->GetClientEntity(i);

				if (!pEnt) continue;

				if (pEnt == pLocal) continue;

				if (pEnt->IsDormant()) continue;

				player_info_t pTemp;

				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;

				if (pEnt->GetTeamNum() == pLocal->GetTeamNum()) continue;

				auto FYaw = pEnt->GetLowerBodyYaw();
				auto pitch = pEnt->GetEyeAngles()->x;

				pEnt->GetEyeAngles()->y = FYaw;
				float PlayerIsMoving = abs(pEnt->GetVelocity().Length2D());
				bool bLowerBodyUpdated = false;
				bool IsBreakingLBY = false;

				bool isFakeHeading = false;

				float oldLBY = pEnt->GetLowerBodyYaw();


				if (oldLBY != pEnt->GetLowerBodyYaw())
				{
					bLowerBodyUpdated = true;
				}
				else
				{
					bLowerBodyUpdated = false;
				}

				if (pEnt->GetEyeAngles()->y - pEnt->GetLowerBodyYaw() > 35)
				{
					isFakeHeading = true;
				}
				else
				{
					isFakeHeading = false;
				}

				float bodyeyedelta = fabs(pEnt->GetEyeAngles()->y - pEnt->GetLowerBodyYaw());
				if (PlayerIsMoving || bLowerBodyUpdated)// || LastUpdatedNetVars->eyeangles.x != CurrentNetVars->eyeangles.x || LastUpdatedNetVars->eyeyaw != CurrentNetVars->eyeangles.y)
				{

					pEnt->GetEyeAngles()->y = FYaw;
					oldLBY = pEnt->GetEyeAngles()->y;

					IsBreakingLBY = false;
				}
				else
				{

					if (bodyeyedelta == 0.f && pEnt->GetVelocity().Length2D() >= 0 && pEnt->GetVelocity().Length2D() < 36)
					{
						pEnt->GetEyeAngles()->y = oldLBY;
						IsBreakingLBY = true;
					}
					else
					{
						IsBreakingLBY = false;
					}
				}

				if (IsBreakingLBY)
				{

					pEnt->GetEyeAngles()->y = oldLBY;

					switch (pLocal->m_iShotsFired() % 3)
					{
					case 1: pEnt->GetEyeAngles()->y = 180; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + pEnt->GetEyeAngles()->y + rand() % 35;
					}
				}
				else if (!IsBreakingLBY && !isFakeHeading)
				{

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw();

					switch (pLocal->m_iShotsFired() % 4)
					{
					case 1: pEnt->GetEyeAngles()->y = 45 + rand() % 180;
					case 2: pEnt->GetEyeAngles()->y = oldLBY + rand() % 90;
					case 3: pEnt->GetEyeAngles()->y = 180 + rand() % 90;
					case 4: pEnt->GetEyeAngles()->y = oldLBY + pEnt->GetEyeAngles()->y + rand() % 45;
					}
				}
				else if (isFakeHeading)
				{

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() - pEnt->GetEyeAngles()->y;

					switch (pLocal->m_iShotsFired() % 2)
					{
					case 1: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetEyeAngles()->y + rand() % 360; break;
					}
				}
				else if (pLocal->m_iShotsFired() >= g_Options.Ragebot.bruteAfterX && g_Options.Ragebot.bruteAfterX != 0)
				{

					pEnt->GetEyeAngles()->y = 180;

					switch (pLocal->m_iShotsFired() % 4)
					{
					case 1: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw(); break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180; break;
					case 4: pEnt->GetEyeAngles()->y = oldLBY + rand() % 45; break;
					}
				}
				else
				{

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;

					switch (pLocal->m_iShotsFired() % 13)
					{
					case 1: pEnt->GetEyeAngles()->y = 180; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + pEnt->GetEyeAngles()->y + rand() % 35;
					case 4: pEnt->GetEyeAngles()->y = 45 + rand() % 180;
					case 5: pEnt->GetEyeAngles()->y = oldLBY + rand() % 90;
					case 6: pEnt->GetEyeAngles()->y = 180 + rand() % 90;
					case 7: pEnt->GetEyeAngles()->y = oldLBY + pEnt->GetEyeAngles()->y + rand() % 45;
					case 8: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw(); break;
					case 9: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 10: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180; break;
					case 11: pEnt->GetEyeAngles()->y = oldLBY + rand() % 45; break;
					case 12: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 13: pEnt->GetEyeAngles()->y = pEnt->GetEyeAngles()->y + rand() % 360; break;
					}
				}
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 6)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				LBYResolver(angs, Player);
				Vector* govno = Player->entity->GetEyeAngles();
				static bool flip_lby[64];
				static int shoots = 0;
				static float last_lby[64];
				if (shoots > 10)
					shoots = 0;
				if (true)
					shoots++;
				static float StoredLBY = 0.0f;
				static bool bLowerBodyIsUpdated;

				if (Player->entity->GetLowerBodyYaw() != StoredLBY) bLowerBodyIsUpdated = true;
				else bLowerBodyIsUpdated = false;

				float bodyeyedelta = govno->y - Player->entity->GetLowerBodyYaw();

				if (bLowerBodyIsUpdated || Player->entity->GetVelocity().Length2D() || fabsf(bodyeyedelta) >= 35.0f)
				{
					govno->y = Player->entity->GetLowerBodyYaw();
					StoredLBY = Player->entity->GetLowerBodyYaw();
				}

				else
				{
					if (fabsf(bodyeyedelta) < 35.0f && fabsf(bodyeyedelta) > 0.0f)
						govno->y = Player->entity->GetLowerBodyYaw() + bodyeyedelta;
				}

				switch (shoots)
				{
				case 0:

					govno->x = 90;
					break;
				case 1:

					govno->x = -179.f;
					break;
				case 2:

					break;
				case 3:

					govno->x = 179.f;
					break;
				case 4:

					govno->x = 90;
					break;
				case 5:
					govno->x = 179.f;
					break;
				case 6:
					govno->x = 179.f;

					break;
				case 7:
					govno->x = 77;

					break;
				case 8:
					govno->x = 89;

					break;
				case 9:
					govno->x = 179.f;
					break;
				case 10:

					govno->x = -179.f;
					break;

				}

				if (last_lby[i] != Player->entity->GetLowerBodyYaw())
					flip_lby[i] = !flip_lby[i];

				if (govno->x < -179.f) govno->x += 360.f;
				else if (govno->x > 90.0 || govno->x < -90.0) govno->x = 89.f;
				else if (govno->x > 89.0 && govno->x < 91.0) govno->x -= 90.f;
				else if (govno->x > 179.0 && govno->x < 181.0) govno->x -= 180;
				else if (govno->x > -179.0 && govno->x < -181.0) govno->x += 180;
				else if (fabs(govno->x) == 0) govno->x = std::copysign(89.0f, govno->x);
				
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 7)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				ForceResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 8)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				ForceResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 9)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				AutoResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 10)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				SecAutoResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 11)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				RandResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 12)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				RandShootsResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 13)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				MathResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 14)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				LBYInverseResolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 15)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				Smart1Resolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 16)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				Smart2Resolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 17)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				AutoResolver2(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 18)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				Resolver1(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 19)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				Ekzi_Resolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 20)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				AutoResolver2XGG(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 21)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				Resolver(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 22)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == pLocal) continue;
				if (pEntity->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
				Player->entity = pEntity;
				Vector* angs = Player->entity->GetEyeAngles();
				if (angs->x > 90.0f && angs->x < 270.f)
				{
					if (angs->x > 180.f)
						angs->x = -89.f;
					else
						angs->x = 89.0f;
				}
				Resolver2(angs, Player);
			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 23)
	{
		C_BaseEntity *pEntity = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto player = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!player) continue;
				if (player == pEntity) continue;
				if (player->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (player->GetTeamNum() == pEntity->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(player);
				Player->entity = player;
				Vector* angs = Player->entity->GetEyeAngles();

				float moving_lby[64];
				float last_moving_lby[64];
				bool ismoving;
				if (pEntity->GetVelocity().Length2D() > 1.1) ismoving = true; else ismoving = false;
				float moving_lby2[64];
				float last_moving_lby2[64];
				bool MeetsLBYReq;
				bool maybefakehead = 0;
				float moving_pitch[64];
				float last_pitch[64];

				float delta = pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw();

				float loglby[64];
				float loggedlby[64];
				static bool bLowerBodyIsUpdated = false;
				float kevin[64];
				if (pEntity->GetLowerBodyYaw() != kevin[pEntity->GetIndex()])
					bLowerBodyIsUpdated = true;
				else
					bLowerBodyIsUpdated = false;

				if (LatestLowerBodyYawUpdateTime[pEntity->GetIndex()] < pEntity->GetSimulationTime() || bLowerBodyIsUpdated)
				{
					LatestLowerBodyYawUpdateTime[pEntity->GetIndex()] = pEntity->GetSimulationTime() + 1.1;

					switch (Globals::missedshots % 4)
					{
					case 0: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw(); break;
					case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()]; break;
					case 2: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() - 35; break;
					case 3: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 35; break;
					}

				}
				else
				{
					if (pEntity->GetVelocity().Length2D() > 45 & pEntity->GetFlags() & FL_ONGROUND)
					{
						moving_lby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
						last_moving_lby[pEntity->GetIndex()] = moving_lby[pEntity->GetIndex()];

						pEntity->GetEyeAngles()->y = moving_lby[pEntity->GetIndex()];

					}
					else if (pEntity->GetVelocity().Length2D() > 25.f && pEntity->GetFlags() & FL_ONGROUND)
					{
						switch (Globals::missedshots % 4)
						{
						case 0: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw(); break;
						case 1: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() - 45; break;
						case 2: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw(); break;
						case 3: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 45; break;
						}
					}
					else if (pEntity->GetVelocity().Length2D() > 25.f && pEntity->GetFlags() & FL_ONGROUND && pEntity->GetVelocity().Length2D() <= 25.0f && pEntity->GetVelocity().Length2D() > 1.1f && !(pEntity->GetMoveType() & FL_DUCKING))
					{
						switch (Globals::missedshots % 4)
						{
						case 0: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()]; break;
						case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 60; break;
						case 2: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 60; break;
						case 3: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 180; break;
						}
					}
					else if (!(pEntity->GetFlags() & FL_ONGROUND))
					{
						switch (Globals::missedshots % 4) // 
						{
						case 0: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()]; break;
						case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 75; break;
						case 2: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 75; break;
						case 3: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 180; break;
						}
					}
					else if (!ismoving && MeetsLBYReq)
					{
						if (Lbywithin35(pEntity))
						{
							if (HasFakeHead(pEntity))
							{
								switch (Globals::missedshots % 5)
								{
								case 0: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()]; break;
								case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 25; break;
								case 2: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 25; break;
								case 3: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 15; break;
								case 4: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 145; break;
								}
							}
							else
							{
								switch (Globals::missedshots % 5)
								{
								case 0: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()]; break;
								case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 25; break;
								case 2: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 25; break;
								case 3: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 15; break;
								case 4: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 145; break;
								}
							}
						}
						else
						{
							switch (Globals::missedshots % 5)
							{
							case 0: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()]; break;
							case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 25; break;
							case 2: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 35; break;
							case 3: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 45; break;
							case 4: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 145; break;
							}
						}


					}
					else
					{
						switch (Globals::missedshots % 5)
						{
						case 0: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw(); break;
						case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 25; break;
						case 2: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 35; break;
						case 3: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() - 45; break;
						case 4: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 145; break;
						}
					}
				}
			}

		}
	}
	else if (g_Options.Ragebot.Resolver == 24)
	{
		C_BaseEntity *pEntity = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto player = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!player) continue;
				if (player == pEntity) continue;
				if (player->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (player->GetTeamNum() == pEntity->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(player);
				Player->entity = player;
				Vector* angs = Player->entity->GetEyeAngles();

				
				float last_moving_lby[64];
				bool ismoving;
				if (pEntity->GetVelocity().Length2D() > 1.1) ismoving = true; else ismoving = false;
				float moving_lby2[64];
				float last_moving_lby2[64];
				bool MeetsLBYReq;
				bool maybefakehead = 0;
				float moving_pitch[64];
				float last_pitch[64];

				float delta = pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw();

				float loglby[64];
				float loggedlby[64];
				static bool bLowerBodyIsUpdated = false;
				float kevin[64];
				if (pEntity->GetLowerBodyYaw() != kevin[pEntity->GetIndex()])
					bLowerBodyIsUpdated = true;
				else
					bLowerBodyIsUpdated = false;

				float last_simtime[64] = { 0.f };
				float stored_pitch[64] = { 89.f };
				float last_lby[64] = { 0.f };
				float last_lby_delta[64] = { 0.f };
				float large_lby_delta[64] = { 0.f };
				float moving_lby[64] = { 0.f };
				bool  was_moving[64] = { false };

				auto legit = false;
				auto update = false;


				const auto local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

				if (!local) return;

				for (auto i = 0; i <g_Engine->GetMaxClients(); ++i) {
					const auto player = const_cast <C_BaseEntity*>(g_EntityList->GetClientEntity(i));



					if (!player || local == player || player->GetTeamNum() == local->GetTeamNum() || player->IsDormant()) {
						last_simtime[i] = 0.f;
						stored_pitch[i] = 89.f;
						last_lby[i] = 0.f;
						last_lby_delta[i] = 0.f;
						large_lby_delta[i] = 0.f;
						was_moving[i] = false;
						continue;
					}

					const auto lby = player->GetLowerBodyYaw();
					const auto eye = player->GetEyeAngles();
					const auto sim = player->GetSimulationTime();
					const auto vel = player->GetVelocity().Length2D();

					auto missed = g_Globals->fired - g_Globals->hit;
					while (missed > 8) missed -= 8;
					while (missed < 0) missed += 8;

					auto angle = 0.f;
					auto angle2 = 0.f;

					if (lby != last_lby[i]) {
						update = true;
						auto delta = fabsf(lby - last_lby[i]);
						last_lby_delta[i] = delta;
						if (delta > 90)
						{
							large_lby_delta[i] = delta;
						}
						last_lby[i] = lby;
					}


					if (pEntity->GetVelocity().Length2D() > 1)
					{
						if (pEntity->GetFlags() & FL_ONGROUND)
						{
							if (pEntity->GetVelocity().Length2D() > 35)
							{
								moving_lby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
								last_moving_lby[pEntity->GetIndex()] = moving_lby[pEntity->GetIndex()];
								was_moving[i] = true;

								if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 100)
								{
									angle = override_yaw(player, local);
								}
								else
								{
									switch (Globals::missedshots % 5)
									{
									case 0: angle = pEntity->GetLowerBodyYaw() - 10; break;
									case 1: angle = moving_lby[pEntity->GetIndex()]; break;
									case 2: angle = moving_lby[pEntity->GetIndex()] + 25; break;
									case 3: angle = last_moving_lby[pEntity->GetIndex()]; break;
									case 4: angle = moving_lby[pEntity->GetIndex()]; break;
									}
								}
							}
							else
							{
								if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 60)
								{
									angle = override_yaw(player, local);
								}
								else
								{
									if (pEntity->GetMoveType() & FL_DUCKING)
									{
										switch (Globals::missedshots % 5)
										{
										case 0: angle = lby - 25; break;
										case 1: angle = lby; break;
										case 2: angle = lby + 25; break;
										case 3: angle = last_moving_lby[pEntity->GetIndex()] + 30; break;
										case 4: angle = last_moving_lby[pEntity->GetIndex()] - 30; break;
										}
									}
									else
									{
										switch (Globals::missedshots % 5)
										{
										case 0: angle = last_moving_lby[pEntity->GetIndex()] + 15; break;
										case 1: angle = lby - 15; break;
										case 2: angle = lby + 15; break;
										case 3: angle = lby + 45; break;
										case 4: angle = last_moving_lby[pEntity->GetIndex()] - 40; break;
										}
									}
								}
							}
						}
						else
						{
							switch (missed)
							{
							case 0: angle = lby + 25; break;
							case 1: angle = lby - 45; break;
							case 2: angle = lby - 180; break;
							case 3: angle = last_moving_lby[pEntity->GetIndex()] - 45; break;
							case 4: angle = last_moving_lby[pEntity->GetIndex()] + 45; break;
							}
						}
					}
					else
					{
						if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 60)
						{
							angle = override_yaw(player, local);
						}
						if (sim - last_simtime[i] >= 1)
						{
							if (sim - last_simtime[i] == 1)
							{
								legit = true;
								switch (missed)
								{
								case 0: angle = last_moving_lby[pEntity->GetIndex()] - 20; break;
								case 1: angle = last_moving_lby[pEntity->GetIndex()] + 20; break;
								case 2: angle = pEntity->GetLowerBodyYaw(); break;
								case 3: angle = pEntity->GetLowerBodyYaw(); break;
								}
							}
						}
						else
						{


							if (was_moving[i])
							{
								switch (Globals::missedshots % 5) {
								case 0: angle = moving_lby[i]; break;
								case 1: angle = lby + large_lby_delta[i]; break;
								case 2: angle = lby + last_lby_delta[i]; break;
								case 3: angle = moving_lby[i]; break;
								case 4: angle = moving_lby[i] + 180; break;
								default: angle = last_moving_lby[pEntity->GetIndex()];
								}

							}
							else
							{

								switch (Globals::missedshots % 9)
								{
								case 0: angle = last_moving_lby[pEntity->GetIndex()] - 25; break;
								case 1: angle = last_moving_lby[pEntity->GetIndex()] + 45; break;
								case 2: angle = lby + last_lby_delta[i]; break;
								case 3: angle = lby + large_lby_delta[i]; break;
								case 4: angle = last_moving_lby[pEntity->GetIndex()]; break;
								case 5: angle = angle - 120; break;
								case 6: angle = angle + 120; break;
								case 7: angle = angle - last_lby_delta[i]; break;
								case 8: angle = angle - 180; break;
								}

							}

						}
					}
					player->GetEyeAngles()->y = angle;
				}

			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 25)
	{
		C_BaseEntity *pEntity = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto player = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!player) continue;
				if (player == pEntity) continue;
				if (player->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (player->GetTeamNum() == pEntity->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(player);
				Player->entity = player;
				Vector* angs = Player->entity->GetEyeAngles();

				float last_moving_lby[64];
				bool ismoving;
				if (pEntity->GetVelocity().Length2D() > 1.1) ismoving = true; else ismoving = false;
				float moving_lby2[64];
				float last_moving_lby2[64];
				bool MeetsLBYReq;
				bool maybefakehead = 0;
				float moving_pitch[64];
				float last_pitch[64];
				float stored_pitch[64] = { 89.f };
				float delta = pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw();

				float loglby[64];
				float loggedlby[64];
				static bool bLowerBodyIsUpdated = false;
				float kevin[64];
				if (pEntity->GetLowerBodyYaw() != kevin[pEntity->GetIndex()])
					bLowerBodyIsUpdated = true;
				else
					bLowerBodyIsUpdated = false;
				auto legit = false;
				auto update = false;
				float last_simtime[64] = { 0.f };
				float last_lby[64] = { 0.f };
				float last_lby_delta[64] = { 0.f };
				float large_lby_delta[64] = { 0.f };
				float moving_lby[64] = { 0.f };
				bool  was_moving[64] = { false };

				const auto pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
				if (!pLocal) return;

				for (auto i = 0; i < g_Engine->GetMaxClients(); ++i)
				{
					const auto player = const_cast <C_BaseEntity*>(g_EntityList->GetClientEntity(i));

					if (!player || pLocal == player || player->GetTeamNum() == pLocal->GetTeamNum() || player->IsImmune() || player->IsDormant()) {
						last_simtime[i] = 0.f;
						stored_pitch[i] = 89.f;
						last_lby[i] = 0.f;

						last_lby_delta[i] = 0.f;
						large_lby_delta[i] = 0.f;

						was_moving[i] = false;
						continue;
					}

					const auto lby = player->GetLowerBodyYaw();
					const auto eye = player->GetEyeAngles();
					const auto sim = player->GetSimulationTime();
					const auto vel = player->GetVelocity().Length2D();

					auto missed = Globals::fired - Globals::hit;
					while (missed > 8) missed -= 8;
					while (missed < 0) missed += 8;


					if (lby != last_lby[i]) {
						update = true;
						auto delta = fabsf(lby - last_lby[i]);
						last_lby_delta[i] = delta;
						if (delta > 80)
						{
							large_lby_delta[i] = delta;
						}
						last_lby[i] = lby;
					}



					if (sim - last_simtime[i] >= 1)
					{
						if (sim - last_simtime[i] == 1)
						{

							legit = true;
						}


						last_simtime[i] = sim;
					}




					auto angle = 0.f;
					if (pEntity->GetVelocity().Length2D() > 25 && (pEntity->GetFlags() & FL_ONGROUND))
					{

						moving_lby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
						last_moving_lby[pEntity->GetIndex()] = moving_lby[pEntity->GetIndex()];
						was_moving[i] = true;
						moving_lby2[i] = lby;

						if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 150)
						{
							angle = override_yaw(player, pLocal);
						}
						else
						{
							angle = lby;
						}


					}
					else if (pEntity->GetVelocity().Length2D() > 25 && !(pEntity->GetFlags() & FL_ONGROUND))
					{
						if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 145)
						{
							angle = override_yaw(player, pLocal);
						}
						else
						{
							switch (Globals::missedshots % 6)
							{
							case 0: angle = moving_lby2[i] + RandomFloat(-10, 10); break;
							case 1: angle = pEntity->GetLowerBodyYaw() + 45; break;
							case 2: angle = pEntity->GetLowerBodyYaw() + 180; break;
							case 3: angle = pEntity->GetLowerBodyYaw() - 90 + RandomFloat(-35, 35); break;
							case 4: angle = pEntity->GetLowerBodyYaw(); break;
							case 5: angle = pEntity->GetLowerBodyYaw() - 40; break;
							}
						}
					}
					else
					{

						if (legit)
						{
							if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 145)
							{
								angle = override_yaw(player, pLocal);
							}
							else
							{
								if (was_moving[i])
								{
									switch (Globals::missedshots % 6)
									{
									case 0: angle = moving_lby2[i]; break;
									case 1: angle = pEntity->GetLowerBodyYaw(); break;
									case 2: angle = pEntity->GetLowerBodyYaw() + 25; break;
									case 3: angle = pEntity->GetLowerBodyYaw() - 25; break;
									case 4: angle = pEntity->GetLowerBodyYaw() - 90; break;
									case 5: angle = pEntity->GetLowerBodyYaw(); break;
									}
								}
								else
								{
									switch (Globals::missedshots % 12)
									{
									case 0: angle = pEntity->GetEyeAngles()->y; break;
									case 1: angle = pEntity->GetLowerBodyYaw(); break;
									case 2: angle = pEntity->GetLowerBodyYaw() - 30; break;
									case 3: angle = pEntity->GetLowerBodyYaw() + 15; break;
									case 4: angle = pEntity->GetLowerBodyYaw() - 90; break;
									case 5: angle = pEntity->GetLowerBodyYaw() + 35; break;
									case 6: angle = pEntity->GetLowerBodyYaw(); break;
									case 7: angle = pEntity->GetEyeAngles()->y; break;
									case 8: angle = pEntity->GetEyeAngles()->y + 30; break;
									case 9: angle = pEntity->GetLowerBodyYaw() + 45; break;
									case 10: angle = pEntity->GetLowerBodyYaw() - 45; break;
									case 11: angle = pEntity->GetLowerBodyYaw(); break;
									}
								}
							}


						}
						else
						{


							if (was_moving[i])
							{
								if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 160)
								{
									angle = override_yaw(player, pLocal);
								}
								else
								{
									switch (Globals::missedshots % 8)
									{
									case 0: angle = moving_lby2[i] - 15; break;
									case 1: angle = moving_lby2[i] + 30; break;
									case 2: angle = lby + last_lby_delta[i]; break;
									case 3: angle = moving_lby2[i] - 30; break;
									case 4: angle = lby; break;
									case 5: angle = moving_lby[i] - 90; break;
									case 6: angle = moving_lby[i] + 90; break;
									case 7: angle = last_lby_delta[i]; break;
									}
								}

							}
							else
							{
								if (GetAsyncKeyState(g_Options.Misc.overridekey) && vel < 150)
								{
									angle = override_yaw(player, pLocal);
								}
								else
								{
									switch (Globals::missedshots % 9)
									{
									case 0: angle = large_lby_delta[i]; break;
									case 1: angle = last_lby_delta[i] + RandomFloat(-10, 10); break;
									case 2: angle = lby + last_lby_delta[i] + RandomFloat(-15, 15); break;
									case 3: angle = lby + large_lby_delta[i]; break;
									case 4: angle = lby + 75 + rand() % 20; break;
									case 5: angle = lby; break;
									case 6: angle = lby - 120; break;
									case 7: angle = angle - last_lby_delta[i]; break;
									case 8: angle = angle - 180; break;
									}
								}

							}

						}
					}
					player->GetEyeAngles()->y = angle;
				}

			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 26)
	{
		C_BaseEntity *pEntity = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto player = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!player) continue;
				if (player == pEntity) continue;
				if (player->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (player->GetTeamNum() == pEntity->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(player);
				Player->entity = player;
				Vector* angs = Player->entity->GetEyeAngles();

				float moving_lby[64];
				float last_moving_lby[64];
				bool ismoving;
				if (pEntity->GetVelocity().Length2D() > 1.1) ismoving = true; else ismoving = false;
				float moving_lby2[64];
				float last_moving_lby2[64];
				bool MeetsLBYReq;
				bool maybefakehead = 0;
				float moving_pitch[64];
				float last_pitch[64];

				float delta = pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw();

				float loglby[64];
				float loggedlby[64];
				static bool bLowerBodyIsUpdated = false;
				float kevin[64];
				if (pEntity->GetLowerBodyYaw() != kevin[pEntity->GetIndex()])
					bLowerBodyIsUpdated = true;
				else
					bLowerBodyIsUpdated = false;

				CUserCmd* pCmd;
				bool  was_moving[64] = { false };
				const auto pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
				for (auto i = 0; i < g_Engine->GetMaxClients(); ++i) {


					auto angle = 0.f;
					if (pEntity->GetVelocity().Length2D() > 1)
					{
						if (pEntity->GetFlags() & FL_ONGROUND)
						{
							if (pEntity->GetVelocity().Length2D() > 45)
							{
								moving_lby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
								last_moving_lby[pEntity->GetIndex()] = moving_lby[pEntity->GetIndex()];

								was_moving[i] = true;

								angle = moving_lby[pEntity->GetIndex()];
							}
							else
							{
								switch (Globals::missedshots % 4)
								{
								case 0: angle = pEntity->GetLowerBodyYaw(); break;
								case 1: angle = last_moving_lby[pEntity->GetIndex()]; break;
								case 2: angle = pEntity->GetLowerBodyYaw() - 25; break;
								case 3: angle = moving_lby[pEntity->GetIndex()] + 25; break;
								}
							}
						}
						else
						{
							switch (Globals::missedshots % 4)
							{
							case 0: angle = last_moving_lby[pEntity->GetIndex()]; break;
							case 1: angle = pEntity->GetLowerBodyYaw() - 45; break;
							case 2: angle = pEntity->GetLowerBodyYaw() - 180; break;
							case 3: angle = pEntity->GetLowerBodyYaw() + 135; break;
							}
						}

					}
					else
					{
						if (Lbywithin35(pEntity))
						{

							if (HasFakeHead(pEntity))
							{
								if (GetAsyncKeyState(g_Options.Misc.overridekey))
								{
									angle = override_yaw(pEntity, pLocal);
								}
								else
								{
									switch (Globals::missedshots % 7)
									{
									case 0: angle = last_moving_lby[pEntity->GetIndex()] + 15; break;
									case 1: angle = last_moving_lby[pEntity->GetIndex()] - 45; break;
									case 2: angle = last_moving_lby[pEntity->GetIndex()] + 65; break;
									case 3: angle = angle - pEntity->GetLowerBodyYaw(); break;
									case 4: angle = (angle + pEntity->GetLowerBodyYaw()) - 120; break;
									case 5: angle = (angle + pEntity->GetLowerBodyYaw()) + 120; break;
									case 6: angle = (angle - pEntity->GetLowerBodyYaw()) - (150 - rand() % 30); break;
									}
								}
							}
							else
							{
								if (GetAsyncKeyState(g_Options.Misc.overridekey))
								{
									angle = override_yaw(pEntity, pLocal);
								}
								else
								{
									switch (Globals::missedshots % 7)
									{
									case 0: angle = last_moving_lby[pEntity->GetIndex()] - 15; break;
									case 1: angle = last_moving_lby[pEntity->GetIndex()] + 15; break;
									case 2: angle = pEntity->GetLowerBodyYaw() - 119; break;
									case 3: angle = angle - 90; break;
									case 4: angle = angle - 60; break;
									case 5: angle = (angle + pEntity->GetLowerBodyYaw()) + 120; break;
									case 6: angle = (angle - pEntity->GetLowerBodyYaw()) - (150 - rand() % 30); break;
									}
								}
							}

						}
						else
						{
							if (GetAsyncKeyState(g_Options.Misc.overridekey))
							{
								angle = override_yaw(pEntity, pLocal);
							}
							else
							{
								if (was_moving[i])
								{
									switch (Globals::missedshots % 7)
									{
									case 0: angle = last_moving_lby[pEntity->GetIndex()] - 10; break;
									case 1: angle = last_moving_lby[pEntity->GetIndex()] + 35; break;
									case 2: angle = last_moving_lby[pEntity->GetIndex()] + 90; break;
									case 3: angle = last_moving_lby[pEntity->GetIndex()] - 90; break;
									case 4: angle = last_moving_lby[pEntity->GetIndex()]; break;
									case 5: angle = last_moving_lby[pEntity->GetIndex()] - 180; break;
									case 6: angle = angle - last_moving_lby[pEntity->GetIndex()]; break;
									}
								}
								else
								{
									switch (Globals::missedshots % 7)
									{
									case 0: angle = angle - pEntity->GetLowerBodyYaw(); break;
									case 1: angle = angle - 90; break;
									case 2: angle = angle + 90; break;
									case 3: angle = pEntity->GetLowerBodyYaw(); break;
									case 4: angle = angle - pEntity->GetLowerBodyYaw(); break;
									case 5: angle = angle - 90; break;
									case 6: angle = angle + 180; break;
									}

								}
							}
						}

					}
					pEntity->GetEyeAngles()->y = angle;
				}




			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 27)
	{
		C_BaseEntity *pEntity = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto player = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
				if (!player) continue;
				if (player == pEntity) continue;
				if (player->IsDormant()) continue;
				player_info_t pTemp;
				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;
				if (player->GetTeamNum() == pEntity->GetTeamNum()) continue;
				PlayerList::CPlayer* Player = plist.FindPlayer(player);
				Player->entity = player;
				Vector* angs = Player->entity->GetEyeAngles();

				float moving_lby[64];
				float last_moving_lby[64];
				bool ismoving;
				if (pEntity->GetVelocity().Length2D() > 1.1) ismoving = true; else ismoving = false;
				float moving_lby2[64];
				float last_moving_lby2[64];
				bool MeetsLBYReq;
				bool maybefakehead = 0;
				float moving_pitch[64];
				float last_pitch[64];

				float delta = pEntity->GetEyeAngles()->y - pEntity->GetLowerBodyYaw();

				float loglby[64];
				float loggedlby[64];
				static bool bLowerBodyIsUpdated = false;
				float kevin[64];
				if (pEntity->GetLowerBodyYaw() != kevin[pEntity->GetIndex()])
					bLowerBodyIsUpdated = true;
				else
					bLowerBodyIsUpdated = false;

				if (pEntity->GetVelocity().Length2D() >= 10)
				{
					if (pEntity->GetFlags() & FL_ONGROUND)
					{
						moving_lby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
						last_moving_lby[pEntity->GetIndex()] = moving_lby[pEntity->GetIndex()];

						switch (Globals::missedshots % 5)
						{
						case 0: pEntity->GetEyeAngles()->y = moving_lby[pEntity->GetIndex()];
						case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] + 25;
						case 2: pEntity->GetEyeAngles()->y = moving_lby[pEntity->GetIndex()];
						case 3: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 25;
						case 4: pEntity->GetEyeAngles()->y = moving_lby[pEntity->GetIndex()];
						}
					}
					else
					{
						switch (Globals::missedshots % 5)
						{
						case 0: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw();
						case 1: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() - 40;
						case 2: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() - 180;
						case 3: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 40;
						case 4: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()];
						}
					}
				}
				else
				{
					if (pEntity->GetEyeAngles()->x > 80)
					{
						switch (Globals::missedshots % 6)
						{
						case 0: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw();
						case 1: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()];
						case 2: pEntity->GetEyeAngles()->y = last_moving_lby[pEntity->GetIndex()] - 45;
						case 3: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() - 90;
						case 4: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 90;
						case 5: pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw() + 180;
						}
					}
					else
					{
						pEntity->GetEyeAngles()->y = pEntity->GetLowerBodyYaw();
					}
				}

			}
		}
	}
	else if (g_Options.Ragebot.Resolver == 28)
	{
		//so huge -fps
	}
}


void PitchAdjustment()
{
	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (g_Options.Ragebot.PitchAdjustment ==0)
	{
		pLocal->GetEyeAngles()->x = 0;
	}
	else if (g_Options.Ragebot.PitchAdjustment == 1)
	{
		pLocal->GetEyeAngles()->x = 89;
	}
	else if (g_Options.Ragebot.PitchAdjustment == 2)
	{
		pLocal->GetEyeAngles()->x = -89;
	}
	else if (g_Options.Ragebot.PitchAdjustment == 3)
	{
		pLocal->GetEyeAngles()->x = 0;
	}
	else if (g_Options.Ragebot.PitchAdjustment == 4)
	{
		float last_simtime[64] = { 0.f };
		float stored_pitch[64] = { 89.f };
		float last_lby[64] = { 0.f };
		float last_lby_delta[64] = { 0.f };
		float large_lby_delta[64] = { 0.f };
		float moving_lby[64] = { 0.f };
		bool  was_moving[64] = { false };
		const auto local = pLocal;
		if (!local) return;
		for (auto i = 0; i < g_Engine->GetMaxClients(); ++i) {
			const auto player = const_cast <C_BaseEntity*>(g_EntityList->GetClientEntity(i));


			if (!player || local == player || player->GetTeamNum() == local->GetTeamNum() || player->IsImmune() || player->IsDormant()) {
				last_simtime[i] = 0.f;
				stored_pitch[i] = 89.f;
				continue;
			}


			const auto eye = player->GetEyeAngles();
			const auto sim = player->GetSimulationTime();



			auto missed = Globals::fired - Globals::hit;
			while (missed > 5) missed -= 5;
			while (missed < 0) missed += 5;

			auto is_legit = false;
			auto update = false;


			if (sim - last_simtime[i] >= 1)
			{
				stored_pitch[i] = eye->x;
				last_simtime[i] = sim;
			}

			if (Globals::missedshots < 3)
				player->GetEyeAngles()->x = stored_pitch[i];
			else
				player->GetEyeAngles()->x = 89;

		}
	}
}



void legitresolver()
{

	if (g_Options.LegitBot.legitresolver)
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		{
			for (auto i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
			{
				C_BaseEntity* pEnt = g_EntityList->GetClientEntity(i);

				if (!pEnt) continue;

				if (pEnt == pLocal) continue;

				if (pEnt->IsDormant()) continue;

				player_info_t pTemp;

				if (!g_Engine->GetPlayerInfo(i, &pTemp))
					continue;

				if (pEnt->GetTeamNum() == pLocal->GetTeamNum()) continue;

				auto FYaw = pEnt->GetLowerBodyYaw();
				auto pitch = pEnt->GetEyeAngles()->x;

				pEnt->GetEyeAngles()->y = FYaw;
				float PlayerIsMoving = abs(pEnt->GetVelocity().Length2D());
				bool bLowerBodyUpdated = false;
				bool IsBreakingLBY = false;

				bool isFakeHeading = false;

				float oldLBY = pEnt->GetLowerBodyYaw();


				if (oldLBY != pEnt->GetLowerBodyYaw())
				{
					bLowerBodyUpdated = true;
				}
				else
				{
					bLowerBodyUpdated = false;
				}

				if (pEnt->GetEyeAngles()->y - pEnt->GetLowerBodyYaw() > 35)
				{
					isFakeHeading = true;
				}
				else
				{
					isFakeHeading = false;
				}

				float bodyeyedelta = fabs(pEnt->GetEyeAngles()->y - pEnt->GetLowerBodyYaw());
				if (PlayerIsMoving || bLowerBodyUpdated)// || LastUpdatedNetVars->eyeangles.x != CurrentNetVars->eyeangles.x || LastUpdatedNetVars->eyeyaw != CurrentNetVars->eyeangles.y)
				{
					Globals::resolvemode = 3;

					pEnt->GetEyeAngles()->y = FYaw;
					oldLBY = pEnt->GetEyeAngles()->y;

					IsBreakingLBY = false;
				}
				else
				{
					Globals::resolvemode = 1;

					if (bodyeyedelta == 0.f && pEnt->GetVelocity().Length2D() >= 0 && pEnt->GetVelocity().Length2D() < 36)
					{
						pEnt->GetEyeAngles()->y = oldLBY;
						IsBreakingLBY = true;
					}
					else
					{
						IsBreakingLBY = false;
					}
				}

				if (IsBreakingLBY)
				{
					Globals::resolvemode = 2;

					pEnt->GetEyeAngles()->y = oldLBY;

					switch (Globals::Shots % 3)
					{
					case 1: pEnt->GetEyeAngles()->y = 180; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + pEnt->GetEyeAngles()->y + rand() % 35;
					}
				}
				else if (!IsBreakingLBY && !isFakeHeading)
				{
					Globals::resolvemode = 3;

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw();

					switch (Globals::Shots % 4)
					{
					case 1: pEnt->GetEyeAngles()->y = 45 + rand() % 180;
					case 2: pEnt->GetEyeAngles()->y = oldLBY + rand() % 90;
					case 3: pEnt->GetEyeAngles()->y = 180 + rand() % 90;
					case 4: pEnt->GetEyeAngles()->y = oldLBY + pEnt->GetEyeAngles()->y + rand() % 45;
					}
				}
				else if (isFakeHeading)
				{
					Globals::resolvemode = 2;

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() - pEnt->GetEyeAngles()->y;

					switch (Globals::Shots % 2)
					{
					case 1: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetEyeAngles()->y + rand() % 360; break;
					}
				}
				else if (Globals::Shots >= g_Options.Ragebot.bruteAfterX && g_Options.Ragebot.bruteAfterX != 0)
				{
					Globals::resolvemode = 2;

					pEnt->GetEyeAngles()->y = 180;

					switch (Globals::Shots % 4)
					{
					case 1: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw(); break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180; break;
					case 4: pEnt->GetEyeAngles()->y = oldLBY + rand() % 45; break;
					}
				}
				else
				{
					Globals::resolvemode = 1;

					pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;

					switch (Globals::Shots % 13)
					{
					case 1: pEnt->GetEyeAngles()->y = 180; break;
					case 2: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180;
					case 3: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + pEnt->GetEyeAngles()->y + rand() % 35;
					case 4: pEnt->GetEyeAngles()->y = 45 + rand() % 180;
					case 5: pEnt->GetEyeAngles()->y = oldLBY + rand() % 90;
					case 6: pEnt->GetEyeAngles()->y = 180 + rand() % 90;
					case 7: pEnt->GetEyeAngles()->y = oldLBY + pEnt->GetEyeAngles()->y + rand() % 45;
					case 8: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw(); break;
					case 9: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 10: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 180; break;
					case 11: pEnt->GetEyeAngles()->y = oldLBY + rand() % 45; break;
					case 12: pEnt->GetEyeAngles()->y = pEnt->GetLowerBodyYaw() + rand() % 90; break;
					case 13: pEnt->GetEyeAngles()->y = pEnt->GetEyeAngles()->y + rand() % 360; break;
					}
				}
			}
		}
	}
}