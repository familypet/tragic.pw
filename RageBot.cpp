#include "RageBot.h"
#include "Render.h"
#include "Autowall.h"
#include <iostream>
#include "MathFunctions.h"
#include "SDK.h"
#include "EnginePrediction.h"
#include "LagComp.h"
//#include "Resolver.h"

using namespace std;

#define TICK_INTERVAL			(g_Globals->interval_per_tick)
#define TICKS_TO_TIME( t )		( g_Globals->interval_per_tick *( t ) )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )


bool toggledab = false;

ragebot::ragebot()
{
	IsLocked = false;
	TargetID = -1;
	pTarget = nullptr;
}

static int aa_left_right = 1;

void ragebot::OnCreateMove(CInput::CUserCmd *pCmd, bool& bSendPacket)
{
	bool storedHitscan = g_Options.Ragebot.Hitscan;

	if (!g_Options.Ragebot.MainSwitch)
		return;

	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (pLocal && pLocal->IsAlive())
	{
		if (g_Options.Ragebot.BAIMkey && G::PressedKeys[g_Options.Ragebot.BAIMkey] && g_Options.Ragebot.Hitscan != 4)
		{
			g_Options.Ragebot.Hitscan = 4;
		}
		else if (g_Options.Ragebot.BAIMkey && !G::PressedKeys[g_Options.Ragebot.BAIMkey] && g_Options.Ragebot.Hitscan != 3)
		{
			g_Options.Ragebot.Hitscan = storedHitscan;
		}

		CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
		if (weapon && weapon->m_AttributeManager()->m_Item()->GetItemDefinitionIndex() == 64)
		{
			if (!CanAttack() && weapon->ammo() > 0)
			{
				pCmd->buttons |= IN_ATTACK;
			}
		}


		if (g_Options.Ragebot.Enabled)
			DoAimbot(pCmd, bSendPacket);

		if (g_Options.Ragebot.AntiRecoil)
			DoNoRecoil(pCmd);

		if (g_Options.Ragebot.EnabledAntiAim)
			DoAntiAim(pCmd, bSendPacket);

		if (g_Options.Ragebot.FakeLag)
			FakeLag(pCmd);





	}
}



bool ragebot::hit_chance(C_BaseEntity* local, CInput::CUserCmd* cmd, CBaseCombatWeapon* weapon, C_BaseEntity* target)
{
	Vector forward, right, up;

	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	constexpr auto max_traces = 256;

	AngleVectors(cmd->viewangles, &forward, &right, &up);

	int total_hits = 0;

	int needed_hits;

	if (MiscFunctions::IsSniper(pWeapon))
	{
		needed_hits = static_cast<int>(max_traces * (g_Options.Ragebot.HitchanceSniper / 100.f));
	}
	else if (MiscFunctions::IsPistol(pWeapon))
	{
		needed_hits = static_cast<int>(max_traces * (g_Options.Ragebot.HitchancePistol / 100.f));
	}
	else if (MiscFunctions::IsHeavy(pWeapon))
	{
		needed_hits = static_cast<int>(max_traces * (g_Options.Ragebot.HitchanceHeavy / 100.f));
	}
	else if (MiscFunctions::IsSmg(pWeapon))
	{
		needed_hits = static_cast<int>(max_traces * (g_Options.Ragebot.HitchanceSmgs / 100.f));
	}
	else if (MiscFunctions::IsRifle(pWeapon))
	{
		needed_hits = static_cast<int>(max_traces * (g_Options.Ragebot.HitchanceRifle / 100.f));
	}
	else if (MiscFunctions::IsRevolver(pWeapon))
	{
		needed_hits = static_cast<int>(max_traces * (g_Options.Ragebot.HitchanceRevolver / 100.f));
	}

	weapon->UpdateAccuracyPenalty(weapon);

	auto eyes = local->GetEyePosition();
	auto flRange = weapon->GetCSWpnData()->m_fRange;

	for (int i = 0; i < max_traces; i++) {
		RandomSeed(i + 1);

		float fRand1 = RandomFloat(0.f, 1.f);
		float fRandPi1 = RandomFloat(0.f, XM_2PI);
		float fRand2 = RandomFloat(0.f, 1.f);
		float fRandPi2 = RandomFloat(0.f, XM_2PI);

		float fRandInaccuracy = fRand1 * weapon->GetInaccuracy();
		float fRandSpread = fRand2 * weapon->GetSpread();

		float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
		float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;

		auto viewSpreadForward = (forward + fSpreadX * right + fSpreadY * up).Normalized();

		Vector viewAnglesSpread;
		VectorAngles(viewSpreadForward, viewAnglesSpread);
		MiscFunctions::NormaliseViewAngle(viewAnglesSpread);

		Vector viewForward;
		AngleVectors(viewAnglesSpread, &viewForward);
		viewForward.NormalizeInPlace();

		viewForward = eyes + (viewForward * flRange);

		trace_t tr;
		Ray_t ray;
		ray.Init(eyes, viewForward);

		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, target, &tr);


		if (tr.m_pEnt == target)
			total_hits++;

		if (total_hits >= needed_hits)
			return true;

		if ((max_traces - i + total_hits) < needed_hits)
			return false;
	}

	return false;
}
template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;

}

void ragebot::DoAimbot(CInput::CUserCmd *pCmd, bool& bSendPacket)
{

	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	bool FindNewTarget = true;
	//IsLocked = false;

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (pWeapon != nullptr)
	{

		if (pWeapon->ammo() == 0 || MiscFunctions::IsKnife(pWeapon) || MiscFunctions::IsGrenade(pWeapon))
		{
			//TargetID = 0;
			//pTarget = nullptr;
			//HitBox = -1;
			return;
		}
	}
	else
		return;

	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = g_EntityList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View; g_Engine->GetViewAngles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < g_Options.Ragebot.FOV)
					FindNewTarget = false;
			}
		}
	}



	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		Globals::Shots = 0;
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;


		TargetID = GetTargetCrosshair();


		// Memesj
		if (TargetID >= 0)
		{
			pTarget = g_EntityList->GetClientEntity(TargetID);
		}
	}

	if (TargetID >= 0 && pTarget)
	{
		HitBox = HitScan(pTarget);

		// Key
		if (g_Options.Ragebot.KeyPress)
		{
			if (g_Options.Ragebot.KeyPress > 0 && !G::PressedKeys[g_Options.Ragebot.KeyPress])
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}


		Globals::AimPoint = GetHitboxPosition(pTarget, HitBox);

		if (AimAtPoint(pLocal, Globals::AimPoint, pCmd))
		{
			if (g_Options.Ragebot.AutoFire && CanAttack() && MiscFunctions::IsSniper(pWeapon) && g_Options.Ragebot.AutoScope)
			{
				if (pLocal->IsScoped()) if (!g_Options.Ragebot.Hitchance || hit_chance(pLocal, pCmd, pWeapon, pTarget)) pCmd->buttons |= IN_ATTACK;
				if (!pLocal->IsScoped()) pCmd->buttons |= IN_ATTACK2;
			}
			if (g_Options.Ragebot.AutoFire && CanAttack() && !(MiscFunctions::IsSniper(pWeapon)))
			{
				if (!g_Options.Ragebot.Hitchance || hit_chance(pLocal, pCmd, pWeapon, pTarget)) pCmd->buttons |= IN_ATTACK;
			}
			if (g_Options.Ragebot.AutoFire && CanAttack() && (MiscFunctions::IsSniper(pWeapon)) && !g_Options.Ragebot.AutoScope)
			{
				if (!g_Options.Ragebot.Hitchance || hit_chance(pLocal, pCmd, pWeapon, pTarget)) if (pLocal->IsScoped()) pCmd->buttons |= IN_ATTACK;
			}

			//if (CanAttack() && pCmd->buttons & IN_ATTACK)
			//	Globals::Shots += 1;
		}

		//Calculate shot fired
		if (pWeapon) {
			if (!(pCmd->buttons & IN_ATTACK) && pWeapon->GetNextPrimaryAttack() <= (pLocal->GetTickBase() * g_Globals->interval_per_tick)) {
				Globals::Shots = 0;
				Globals::missedshots = 0;
			}
			else {
				Globals::Shots += pLocal->m_iShotsFired();
			}
		}




		if (g_Options.Ragebot.AutoStop)
		{
			if (g_Options.Ragebot.autostopkey && G::PressedKeys[g_Options.Ragebot.autostopkey])
			{
				pCmd->forwardmove = 0.f;
				pCmd->sidemove = 0.f;
			}
		}



		if (g_Options.Ragebot.AutoCrouch)
		{
			pCmd->buttons |= IN_DUCK;
		}

	}

	// Auto Pistol
	static bool WasFiring = false;
	if (pWeapon != nullptr)
	{
		if (MiscFunctions::IsPistol(pWeapon) && g_Options.Ragebot.AutoPistol && pWeapon->m_AttributeManager()->m_Item()->GetItemDefinitionIndex() != 64)
		{
			if (pCmd->buttons & IN_ATTACK && !MiscFunctions::IsKnife(pWeapon) && !MiscFunctions::IsGrenade(pWeapon))
			{
				if (WasFiring)
				{
					pCmd->buttons &= ~IN_ATTACK;
				}
			}

			WasFiring = pCmd->buttons & IN_ATTACK ? true : false;
		}
	}


}



bool ragebot::TargetMeetsRequirements(C_BaseEntity* pEntity)
{
	auto local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != local->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)ClassID::CCSPlayer && g_Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != local->GetTeamNum() || g_Options.Ragebot.FriendlyFire)
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}




float ragebot::FovToPlayer(Vector ViewOffSet, Vector View, C_BaseEntity* pEntity, int aHitBox)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox); //pvs fix disabled
														 // Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int ragebot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = g_Options.Ragebot.FOV;

	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; g_Engine->GetViewAngles(View);

	for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BaseEntity *pEntity = g_EntityList->GetClientEntity(i);

		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int ragebot::HitScan(C_BaseEntity* pEntity)
{
	vector<int> HitBoxesToScan{ Head , Neck, Chest, Stomach };

	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	int HitScanMode = g_Options.Ragebot.Hitscan;

	if (!g_Options.Ragebot.Hitscan)
	{
		switch (g_Options.Ragebot.Hitbox)
		{
		case 0:
			HitBoxesToScan.push_back(Head);
			break;
		case 1:
			HitBoxesToScan.push_back(Neck);
			break;
		case 2:
			HitBoxesToScan.push_back(Pelvis);
			break;
		}
	}
	else
	{
		switch (HitScanMode)
		{
		case 1:
			// low
			HitBoxesToScan.push_back(Head);
			HitBoxesToScan.push_back(Neck);
			HitBoxesToScan.push_back(UpperChest);
			HitBoxesToScan.push_back(Chest);
			HitBoxesToScan.push_back(Stomach);
			HitBoxesToScan.push_back(Pelvis);
			break;
		case 2:
			// medium
			HitBoxesToScan.push_back(Head);
			HitBoxesToScan.push_back(Neck);
			HitBoxesToScan.push_back(UpperChest);
			HitBoxesToScan.push_back(Chest);
			HitBoxesToScan.push_back(Stomach);
			HitBoxesToScan.push_back(Pelvis);
			HitBoxesToScan.push_back(LeftUpperArm);
			HitBoxesToScan.push_back(RightUpperArm);
			HitBoxesToScan.push_back(LeftThigh);
			HitBoxesToScan.push_back(RightThigh);
			break;
		case 3:
			// high
			HitBoxesToScan.push_back(Head);
			HitBoxesToScan.push_back(Neck);
			HitBoxesToScan.push_back(UpperChest);
			HitBoxesToScan.push_back(Chest);
			HitBoxesToScan.push_back(Stomach);
			HitBoxesToScan.push_back(Pelvis);
			HitBoxesToScan.push_back(LeftThigh);
			HitBoxesToScan.push_back(RightThigh);
			HitBoxesToScan.push_back(LeftFoot);
			HitBoxesToScan.push_back(RightFoot);
			HitBoxesToScan.push_back(LeftShin);
			HitBoxesToScan.push_back(RightShin);
			HitBoxesToScan.push_back(LeftLowerArm);
			HitBoxesToScan.push_back(RightLowerArm);
			HitBoxesToScan.push_back(LeftUpperArm);
			HitBoxesToScan.push_back(RightUpperArm);
			HitBoxesToScan.push_back(LeftHand);
			HitBoxesToScan.push_back(RightHand);
			break;
		case 4:
			// baim
			HitBoxesToScan.push_back(UpperChest);
			HitBoxesToScan.push_back(Chest);
			HitBoxesToScan.push_back(Stomach);
			HitBoxesToScan.push_back(Pelvis);
			HitBoxesToScan.push_back(LeftUpperArm);
			HitBoxesToScan.push_back(RightUpperArm);
			HitBoxesToScan.push_back(LeftThigh);
			HitBoxesToScan.push_back(RightThigh);
			HitBoxesToScan.push_back(LeftHand);
			HitBoxesToScan.push_back(RightHand);
			HitBoxesToScan.push_back(LeftFoot);
			HitBoxesToScan.push_back(RightFoot);
			HitBoxesToScan.push_back(LeftShin);
			HitBoxesToScan.push_back(RightShin);
			HitBoxesToScan.push_back(LeftLowerArm);
			HitBoxesToScan.push_back(RightLowerArm);
			break;
		}
	}
	static vector<int> baim{ UpperChest ,Chest ,Stomach ,Pelvis ,LeftUpperArm ,RightUpperArm ,LeftThigh,RightThigh ,LeftHand ,RightHand, LeftFoot, RightFoot, LeftShin, RightShin,LeftLowerArm,RightLowerArm };

	int bestHitbox = -1;
	float highestDamage;

	if (MiscFunctions::IsSniper(pWeapon))
	{
		highestDamage = g_Options.Ragebot.MinimumDamageSniper;
	}
	else if (MiscFunctions::IsPistol(pWeapon))
	{
		highestDamage = g_Options.Ragebot.MinimumDamagePistol;
	}
	else if (MiscFunctions::IsHeavy(pWeapon))
	{
		highestDamage = g_Options.Ragebot.MinimumDamageHeavy;
	}
	else if (MiscFunctions::IsSmg(pWeapon))
	{
		highestDamage = g_Options.Ragebot.MinimumDamageSmg;
	}
	else if (MiscFunctions::IsRifle(pWeapon))
	{
		highestDamage = g_Options.Ragebot.MinimumDamageRifle;
	}
	else if (MiscFunctions::IsRevolver(pWeapon))
	{
		highestDamage = g_Options.Ragebot.MinimumDamageRevolver;
	}

	for (auto HitBoxID : HitBoxesToScan)
	{

		Vector Point = GetHitboxPosition(pEntity, HitBoxID); //pvs fix disabled

		float damage = 0.0f;
		if (CanHit(pEntity, Point, &damage))
		{
			if (damage > highestDamage || damage > pEntity->GetHealth())
			{
				bestHitbox = HitBoxID;
				highestDamage = damage;
			}
		}
	}
	/*for (auto HitBoxID : baim)
	{

	Vector Point = GetHitboxPosition(pEntity, HitBoxID); //pvs fix disabled

	float damage = 0.0f;
	if (CanHit(pEntity, Point, &damage))
	{
	if (damage > highestDamage && damage > pEntity->GetHealth())
	{
	bestHitbox = HitBoxID;
	highestDamage = damage;
	}
	}
	}*/
	return bestHitbox;

}



void ragebot::DoNoRecoil(CInput::CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (pLocal != nullptr)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			MiscFunctions::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

float FovToPoint(Vector ViewOffSet, Vector View, Vector Point)
{
	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(View, &Forward);
	Vector AimPos = Point;

	// Get delta vector between our local eye position and passed vector
	Delta = AimPos - Origin;
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (180.f / PI));
}
bool me123 = false;
bool ragebot::AimAtPoint(C_BaseEntity* pLocal, Vector point, CInput::CUserCmd *pCmd)
{
	bool ReturnValue = false;

	if (point.Length() == 0) return ReturnValue;

	Vector angles;

	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	VectorAngles(point - src, angles);





	IsLocked = true;
	ReturnValue = true;



	if (g_Options.Ragebot.Silent)
	{
		if (CanAttack()) {
			pCmd->viewangles = angles;
		}
	}

	if (!g_Options.Ragebot.Silent)
	{
		pCmd->viewangles = angles;
		g_Engine->SetViewAngles(pCmd->viewangles);
	}
	return ReturnValue;
}




void NormalizeVector(Vector& vec) {
	for (int i = 0; i < 3; ++i) {
		while (vec[i] > 180.f)
			vec[i] -= 360.f;

		while (vec[i] < -180.f)
			vec[i] += 360.f;
	}
	vec[2] = 0.f;
}


void VectorAngles2(const Vector &vecForward, Vector &vecAngles)
{
	Vector vecView;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f)
	{
		vecView[0] = 0.f;
		vecView[1] = 0.f;
	}
	else
	{
		vecView[1] = vec_t(atan2(vecForward[1], vecForward[0]) * 180.f / M_PI);

		if (vecView[1] < 0.f)
			vecView[1] += 360.f;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

		vecView[0] = vec_t(atan2(vecForward[2], vecView[2]) * 180.f / M_PI);
	}

	vecAngles[0] = -vecView[0];
	vecAngles[1] = vecView[1];
	vecAngles[2] = 0.f;
}




bool EdgeAntiAim(C_BaseEntity* pLocalBaseEntity, CInput::CUserCmd* cmd, float flWall, float flCornor)
{
	Ray_t ray;
	trace_t tr;

	CTraceFilter traceFilter;
	traceFilter.pSkip = pLocalBaseEntity;

	auto bRetVal = false;
	auto vecCurPos = pLocalBaseEntity->GetEyePosition();

	for (float i = 0; i < 360; i++)
	{
		Vector vecDummy(10.f, cmd->viewangles.y, 0.f);
		vecDummy.y += i;

		NormalizeVector(vecDummy);

		Vector vecForward;
		AngleVectors2(vecDummy, vecForward);

		auto flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
		vecForward *= flLength;

		ray.Init(vecCurPos, (vecCurPos + vecForward));
		g_EngineTrace->TraceRay(ray, MASK_SHOT, (CTraceFilter *)&traceFilter, &tr);

		if (tr.fraction != 1.0f)
		{
			Vector qAngles;
			auto vecNegate = tr.plane.normal;

			vecNegate *= -1.f;
			VectorAngles2(vecNegate, qAngles);

			vecDummy.y = qAngles.y;

			NormalizeVector(vecDummy);
			trace_t leftTrace, rightTrace;

			Vector vecLeft;
			AngleVectors2(vecDummy + Vector(0.f, 30.f, 0.f), vecLeft);

			Vector vecRight;
			AngleVectors2(vecDummy - Vector(0.f, 30.f, 0.f), vecRight);

			vecLeft *= (flLength + (flLength * sin(DEG2RAD(30.f))));
			vecRight *= (flLength + (flLength * sin(DEG2RAD(30.f))));

			ray.Init(vecCurPos, (vecCurPos + vecLeft));
			g_EngineTrace->TraceRay(ray, MASK_SHOT, (CTraceFilter*)&traceFilter, &leftTrace);

			ray.Init(vecCurPos, (vecCurPos + vecRight));
			g_EngineTrace->TraceRay(ray, MASK_SHOT, (CTraceFilter*)&traceFilter, &rightTrace);

			if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				vecDummy.y -= flCornor; // left
			else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				vecDummy.y += flCornor; // right			

			cmd->viewangles.y = vecDummy.y;
			cmd->viewangles.y -= flWall;
			cmd->viewangles.x = 89.0f;
			bRetVal = true;
		}
	}
	return bRetVal;
}

// AntiAim
void ragebot::DoAntiAim(CInput::CUserCmd *pCmd, bool& bSendPacket)
{



	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());



	if (g_Options.Ragebot.flipkey > 0 && G::PressedKeys[g_Options.Ragebot.flipkey])
	{
		if (toggledab == false)
		{
			toggledab = true;
			if (aa_left_right == 1) aa_left_right = 0;
			else if (aa_left_right == 0) aa_left_right = 1;
		}
	}
	else
	{
		toggledab = false;
	}

	if (pLocal->GetMoveType() == MOVETYPE_WALK && pLocal->GetVelocity().Length2D() <= 135) // walking
	{
		//walk_ builder stuff
		if (g_Options.Ragebot.AA_onWalk)
		{

			g_Options.Ragebot.BuilderAAs = g_Options.Ragebot.walk_BuilderAAs;
			g_Options.Ragebot.Jitter = g_Options.Ragebot.walk_Jitter;
			g_Options.Ragebot.FJitter = g_Options.Ragebot.walk_FJitter;
			g_Options.Ragebot.LBYBreaker = g_Options.Ragebot.walk_LBYBreaker;
			g_Options.Ragebot.BuilderReal = g_Options.Ragebot.walk_BuilderReal;
			g_Options.Ragebot.BuilderFake = g_Options.Ragebot.walk_BuilderFake;
			g_Options.Ragebot.BuilderPitch = g_Options.Ragebot.walk_BuilderPitch;
			g_Options.Ragebot.JitterRange = g_Options.Ragebot.walk_JitterRange;
			g_Options.Ragebot.FJitterRange = g_Options.Ragebot.walk_FJitterRange;


			//prebuild stuff
			g_Options.Ragebot.PreAAs = g_Options.Ragebot.walk_PreAAs;
			g_Options.Ragebot.Pitch = g_Options.Ragebot.walk_Pitch;
			g_Options.Ragebot.PitchAdder = g_Options.Ragebot.walk_PitchAdder;
			g_Options.Ragebot.YawTrue = g_Options.Ragebot.walk_YawTrue;
			g_Options.Ragebot.YawFake = g_Options.Ragebot.walk_YawFake;



			g_Options.Ragebot.YawTrueAdder = g_Options.Ragebot.walk_YawTrueAdder;
			g_Options.Ragebot.YawFakeAdder = g_Options.Ragebot.walk_YawFakeAdder;

		}

	}
	if (pLocal->GetMoveType() == MOVETYPE_WALK && pLocal->GetVelocity().Length2D() > 135) // running
	{
		//run_ builder stuff
		if (g_Options.Ragebot.AA_onRun)
		{


			g_Options.Ragebot.BuilderAAs = g_Options.Ragebot.run_BuilderAAs;
			g_Options.Ragebot.Jitter = g_Options.Ragebot.run_Jitter;
			g_Options.Ragebot.FJitter = g_Options.Ragebot.run_FJitter;
			g_Options.Ragebot.LBYBreaker = g_Options.Ragebot.run_LBYBreaker;
			g_Options.Ragebot.BuilderReal = g_Options.Ragebot.run_BuilderReal;
			g_Options.Ragebot.BuilderFake = g_Options.Ragebot.run_BuilderFake;
			g_Options.Ragebot.BuilderPitch = g_Options.Ragebot.run_BuilderPitch;
			g_Options.Ragebot.JitterRange = g_Options.Ragebot.run_JitterRange;
			g_Options.Ragebot.FJitterRange = g_Options.Ragebot.run_FJitterRange;

			//prebuild stuff
			g_Options.Ragebot.PreAAs = g_Options.Ragebot.run_PreAAs;
			g_Options.Ragebot.Pitch = g_Options.Ragebot.run_Pitch;
			g_Options.Ragebot.PitchAdder = g_Options.Ragebot.run_PitchAdder;
			g_Options.Ragebot.YawTrue = g_Options.Ragebot.run_YawTrue;
			g_Options.Ragebot.YawFake = g_Options.Ragebot.run_YawFake;



			g_Options.Ragebot.YawTrueAdder = g_Options.Ragebot.run_YawTrueAdder;
			g_Options.Ragebot.YawFakeAdder = g_Options.Ragebot.run_YawFakeAdder;

		}


	}
	if ((pCmd->buttons & IN_DUCK) && pLocal->GetVelocity().Length2D() <= 85) // crouching
	{
		//crouch_ builder stuff
		if (g_Options.Ragebot.AA_onCrouch)
		{


			g_Options.Ragebot.BuilderAAs = g_Options.Ragebot.crouch_BuilderAAs;
			g_Options.Ragebot.Jitter = g_Options.Ragebot.crouch_Jitter;
			g_Options.Ragebot.FJitter = g_Options.Ragebot.crouch_FJitter;
			g_Options.Ragebot.LBYBreaker = g_Options.Ragebot.crouch_LBYBreaker;
			g_Options.Ragebot.BuilderReal = g_Options.Ragebot.crouch_BuilderReal;
			g_Options.Ragebot.BuilderFake = g_Options.Ragebot.crouch_BuilderFake;
			g_Options.Ragebot.BuilderPitch = g_Options.Ragebot.crouch_BuilderPitch;
			g_Options.Ragebot.JitterRange = g_Options.Ragebot.crouch_JitterRange;
			g_Options.Ragebot.FJitterRange = g_Options.Ragebot.crouch_FJitterRange;

			//prebuild stuff
			g_Options.Ragebot.PreAAs = g_Options.Ragebot.crouch_PreAAs;
			g_Options.Ragebot.Pitch = g_Options.Ragebot.crouch_Pitch;
			g_Options.Ragebot.PitchAdder = g_Options.Ragebot.crouch_PitchAdder;
			g_Options.Ragebot.YawTrue = g_Options.Ragebot.crouch_YawTrue;
			g_Options.Ragebot.YawFake = g_Options.Ragebot.crouch_YawFake;



			g_Options.Ragebot.YawTrueAdder = g_Options.Ragebot.crouch_YawTrueAdder;
			g_Options.Ragebot.YawFakeAdder = g_Options.Ragebot.crouch_YawFakeAdder;

		}
	}
	if (pLocal->GetVelocity().Length2D() < 20 == pLocal->GetVelocity().Length2D() > 0) //fakewalking
	{
		//fwalk_ builder stuff
		if (g_Options.Ragebot.AA_onFakeWalk)
		{


			g_Options.Ragebot.BuilderAAs = g_Options.Ragebot.fwalk_BuilderAAs;
			g_Options.Ragebot.Jitter = g_Options.Ragebot.fwalk_Jitter;
			g_Options.Ragebot.FJitter = g_Options.Ragebot.fwalk_FJitter;
			g_Options.Ragebot.LBYBreaker = g_Options.Ragebot.fwalk_LBYBreaker;
			g_Options.Ragebot.BuilderReal = g_Options.Ragebot.fwalk_BuilderReal;
			g_Options.Ragebot.BuilderFake = g_Options.Ragebot.fwalk_BuilderFake;
			g_Options.Ragebot.BuilderPitch = g_Options.Ragebot.fwalk_BuilderPitch;
			g_Options.Ragebot.JitterRange = g_Options.Ragebot.fwalk_JitterRange;
			g_Options.Ragebot.FJitterRange = g_Options.Ragebot.fwalk_FJitterRange;

			//prebuild stuff
			g_Options.Ragebot.PreAAs = g_Options.Ragebot.fwalk_PreAAs;
			g_Options.Ragebot.Pitch = g_Options.Ragebot.fwalk_Pitch;
			g_Options.Ragebot.PitchAdder = g_Options.Ragebot.fwalk_PitchAdder;
			g_Options.Ragebot.YawTrue = g_Options.Ragebot.fwalk_YawTrue;
			g_Options.Ragebot.YawFake = g_Options.Ragebot.fwalk_YawFake;


			g_Options.Ragebot.YawFakeAdder = g_Options.Ragebot.fwalk_YawFakeAdder;
			g_Options.Ragebot.YawTrueAdder = g_Options.Ragebot.fwalk_YawTrueAdder;

		}
	}
	if (pLocal->GetVelocity().Length2D() == 0 && !(pCmd->buttons & IN_DUCK)) //standing still
	{
		//stand_ builder stuff 
		if (g_Options.Ragebot.AA_onStand)
		{


			g_Options.Ragebot.BuilderAAs = g_Options.Ragebot.stand_BuilderAAs;
			g_Options.Ragebot.Jitter = g_Options.Ragebot.stand_Jitter;
			g_Options.Ragebot.FJitter = g_Options.Ragebot.stand_FJitter;
			g_Options.Ragebot.LBYBreaker = g_Options.Ragebot.stand_LBYBreaker;
			g_Options.Ragebot.BuilderReal = g_Options.Ragebot.stand_BuilderReal;
			g_Options.Ragebot.BuilderFake = g_Options.Ragebot.stand_BuilderFake;
			g_Options.Ragebot.BuilderPitch = g_Options.Ragebot.stand_BuilderPitch;
			g_Options.Ragebot.JitterRange = g_Options.Ragebot.stand_JitterRange;
			g_Options.Ragebot.FJitterRange = g_Options.Ragebot.stand_FJitterRange;

			//prebuild stuff
			g_Options.Ragebot.PreAAs = g_Options.Ragebot.stand_PreAAs;
			g_Options.Ragebot.Pitch = g_Options.Ragebot.stand_Pitch;
			g_Options.Ragebot.PitchAdder = g_Options.Ragebot.stand_PitchAdder;
			g_Options.Ragebot.YawTrue = g_Options.Ragebot.stand_YawTrue;
			g_Options.Ragebot.YawFake = g_Options.Ragebot.stand_YawFake;


			g_Options.Ragebot.YawTrueAdder = g_Options.Ragebot.stand_YawTrueAdder;
			g_Options.Ragebot.YawFakeAdder = g_Options.Ragebot.stand_YawFakeAdder;

		}
	}






	// If the aimbot is doing something don't do anything
	if (pCmd->buttons & IN_ATTACK && CanAttack())
		return;
	if ((pCmd->buttons & IN_USE))
		return;
	if (pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;
	// Weapon shit

	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		CCSGrenade* csGrenade = (CCSGrenade*)pWeapon;


		if (MiscFunctions::IsKnife(pWeapon) && !g_Options.Ragebot.KnifeAA)
			return;

		if (csGrenade->GetThrowTime() > 0.f)
			return;
	}

	// Don't do antiaim
	// if (DoExit) return;

	if (g_Options.Ragebot.Edge) {
		auto bEdge = EdgeAntiAim(pLocal, pCmd, 360.f, 89.f);
		if (bEdge)
			return;
	}

	if (g_Options.Ragebot.YawFake != 0)
		Globals::ySwitch = !Globals::ySwitch;
	else
		Globals::ySwitch = true;

	bSendPacket = Globals::ySwitch;

	Vector SpinAngles = { 0,0,0 };
	Vector FakeAngles = { 0,0,0 };
	float server_time = pLocal->GetTickBase() * g_Globals->interval_per_tick;
	static int ticks;
	static bool flip;
	if (ticks < 15 + rand() % 20)
		ticks++;
	else
	{
		flip = !flip;
		ticks = 0;
	}
	Vector StartAngles;
	double rate = 360.0 / 1.618033988749895;
	double yaw = fmod(static_cast<double>(server_time)*rate, 360.0);
	double factor = 360.0 / M_PI;
	factor *= 25;



	switch (g_Options.Ragebot.YawTrue)
	{
	case 1: //sideways
	{
		g_Engine->GetViewAngles(StartAngles);
		SpinAngles.y = flip ? StartAngles.y - 90.f : StartAngles.y + 90.f;
	}
	break;
	case 2://slowspin
		SpinAngles.y += static_cast<float>(yaw);
		break;
	case 3://fastspin
	{
		SpinAngles.y = (float)(fmod(server_time / 0.05f * 360.0f, 360.0f));
	}
	break;
	case 4://backwards
	{
		g_Engine->GetViewAngles(StartAngles);
		StartAngles.y -= 180.f;
		SpinAngles = StartAngles;
	}
	break;
	case 5:
	{
		g_Engine->GetViewAngles(StartAngles);



		if (aa_left_right == 0)
		{
			//45
			g_Options.Ragebot.YawTrueAdder = 45;
			//SpinAngles.y = StartAngles.y - 90.f;
		}
		else
		{
			//-135
			g_Options.Ragebot.YawTrueAdder = -135;
			//SpinAngles.y = StartAngles.y + 90.f;
		}

		static bool dir = false;

		if (GetAsyncKeyState(VK_LEFT)) dir = false; else if (GetAsyncKeyState(VK_RIGHT)) dir = true;

		if (dir && pLocal->GetVelocity().Length2D() < 1)
		{
			if (Globals::shouldflip)
			{
				SpinAngles.y = StartAngles.y - 110;
			}
			else
			{
				SpinAngles.y = StartAngles.y + 135;
			}
		}
		else if (!dir && pLocal->GetVelocity().Length2D() < 1)
		{
			if (Globals::shouldflip)
			{
				SpinAngles.y = StartAngles.y + 110;
			}
			else
			{
				SpinAngles.y = StartAngles.y - 135;
			}
		}
		else if (pLocal->GetVelocity().Length2D() > 0)
		{
			SpinAngles.y = StartAngles.y + 180;
		}

	}
	break;
	case 6:
	{
		g_Engine->GetViewAngles(StartAngles);

		static bool dir = false;

		if (GetAsyncKeyState(VK_LEFT)) dir = false; else if (GetAsyncKeyState(VK_RIGHT)) dir = true;

		if (dir && pLocal->GetVelocity().Length2D() < 1)
		{
			if (Globals::shouldflip)
			{
				SpinAngles.y = StartAngles.y - 110;
			}
			else
			{
				SpinAngles.y = StartAngles.y + 135;
			}
		}
		else if (!dir && pLocal->GetVelocity().Length2D() < 1)
		{
			if (Globals::shouldflip)
			{
				SpinAngles.y = StartAngles.y + 110;
			}
			else
			{
				SpinAngles.y = StartAngles.y - 135;
			}
		}
		else if (pLocal->GetVelocity().Length2D() > 0)
		{
			SpinAngles.y = StartAngles.y + 180;
		}
	}
	break;
	case 7:
	{
		g_Engine->GetViewAngles(StartAngles);

		SpinAngles.y = flip ? StartAngles.y - 145.f : StartAngles.y + 145.f;
	}
	break;
	case 8:
	{
		g_Engine->GetViewAngles(StartAngles);

		static bool dir = false;
		static int jitterangle = 0;

		if (GetAsyncKeyState(VK_LEFT)) dir = false; else if (GetAsyncKeyState(VK_RIGHT)) dir = true;


		if (dir && pLocal->GetVelocity().Length2D() < 1)
		{
			if (Globals::shouldflip)
			{
				SpinAngles.y = StartAngles.y - 90;
			}
			else
			{
				SpinAngles.y = StartAngles.y + 125;

				if (jitterangle <= 1)
				{
					SpinAngles.y = StartAngles.y + 125;
					jitterangle += 1;
				}
				else if (jitterangle > 1 && jitterangle <= 3)
				{
					SpinAngles.y = StartAngles.y + 145;
					jitterangle += 1;
				}
				else
				{
					jitterangle = 0;
				}
			}
		}
		else if (!dir && pLocal->GetVelocity().Length2D() < 1)
		{
			if (Globals::shouldflip)
			{
				SpinAngles.y = StartAngles.y + 90;
			}
			else
			{
				SpinAngles.y = StartAngles.y - 125;

				if (jitterangle <= 1)
				{
					SpinAngles.y = StartAngles.y - 125;
					jitterangle += 1;
				}
				else if (jitterangle > 1 && jitterangle <= 3)
				{
					SpinAngles.y = StartAngles.y - 145;
					jitterangle += 1;
				}
				else
				{
					jitterangle = 0;
				}
			}
		}
		else if (pLocal->GetVelocity().Length2D() > 0)
		{
			SpinAngles.y = StartAngles.y + 155;

			if (jitterangle <= 1)
			{
				SpinAngles.y = StartAngles.y + 155;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				SpinAngles.y = StartAngles.y - 155;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
	}
	break;
	}



	switch (g_Options.Ragebot.YawFake)
	{
	case 1://sideways
	{
		g_Engine->GetViewAngles(StartAngles);
		FakeAngles.y = flip ? StartAngles.y + 90.f : StartAngles.y - 90.f;
	}
	break;
	case 2://slowspin
		FakeAngles.y += static_cast<float>(yaw);
		break;
	case 3://fastspin
		FakeAngles.y = (float)(fmod(server_time / 0.05f * 360.0f, 360.0f));
		break;
	case 4://backwards
	{
		g_Engine->GetViewAngles(StartAngles);

		StartAngles -= 180.f;
		FakeAngles = StartAngles;

	}
	break;
	case 5: //lby antiaim
	{
		g_Engine->GetViewAngles(StartAngles);
		static bool llamaflip;
		static float oldLBY = 0.0f;
		float LBY = pLocal->GetLowerBodyYaw();
		if (LBY != oldLBY) // did lowerbody update?
		{
			llamaflip = !llamaflip;
			oldLBY = LBY;
		}
		FakeAngles.y = llamaflip ? StartAngles.y - 90.f : StartAngles.y + 90.f;
	}
	break;
	case 6:
	{
		g_Engine->GetViewAngles(StartAngles);
		static bool dir = false;

		if (GetAsyncKeyState(VK_LEFT)) dir = false; else if (GetAsyncKeyState(VK_RIGHT)) dir = true;

		if (dir && pLocal->GetVelocity().Length2D() < 1)
		{
			FakeAngles.y -= StartAngles.y - 110;
		}
		else if (!dir && pLocal->GetVelocity().Length2D() < 1)
		{
			FakeAngles.y += StartAngles.y + 110;
		}
		else if (pLocal->GetVelocity().Length2D() > 0)
		{
			FakeAngles.y = (float)(fmod(server_time / 0.05f * 360.0f, 360.0f));
		}
	}
	break;
	case 7:
	{
		g_Engine->GetViewAngles(StartAngles);

		FakeAngles.y = flip ? StartAngles.y - 145.f : StartAngles.y + 145.f;
	}
	break;
	case 8:
	{
		g_Engine->GetViewAngles(StartAngles);
		static bool dir = false;
		static int jitterangle = 0;

		if (GetAsyncKeyState(VK_LEFT)) dir = false; else if (GetAsyncKeyState(VK_RIGHT)) dir = true;

		if (dir && pLocal->GetVelocity().Length2D() < 1)
		{
			FakeAngles.y = StartAngles.y - 75;
			if (jitterangle <= 1)
			{
				FakeAngles.y = StartAngles.y - 75;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				FakeAngles.y = StartAngles.y - 105;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
		else if (!dir && pLocal->GetVelocity().Length2D() < 1)
		{
			FakeAngles.y = StartAngles.y + 75;
			if (jitterangle <= 1)
			{
				FakeAngles.y = StartAngles.y + 75;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				FakeAngles.y = StartAngles.y + 105;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
		else if (pLocal->GetVelocity().Length2D() > 0)
		{
			FakeAngles.y = StartAngles.y + 45 + RandomFloat(90, -90);
			if (jitterangle <= 1)
			{
				FakeAngles.y += StartAngles.y + 45 + RandomFloat(90, -90);
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				FakeAngles.y -= StartAngles.y - 45 - RandomFloat(90, -90);
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
	}
	break;
	case 9:
	{
		g_Engine->GetViewAngles(StartAngles);
		if (aa_left_right == 0)
		{
			if (Globals::shouldflip)
			{
				FakeAngles.y = StartAngles.y - 90.f;
			}
			else
			{
				FakeAngles.y = StartAngles.y + 90.f;
			}
		}
		else
		{
			if (Globals::shouldflip)
			{
				FakeAngles.y = StartAngles.y + 90.f;
			}
			else
			{
				FakeAngles.y = StartAngles.y - 90.f;
			}
		}
	}
	break;
	case 10:
	{
		g_Engine->GetViewAngles(StartAngles);
		FakeAngles.y = StartAngles.y;
		g_Options.Ragebot.YawFakeAdder = 0;
	}
	break;
	}

	if (g_Options.Ragebot.PreAAs && !g_Options.Ragebot.BuilderAAs)
	{
		if (Globals::ySwitch && g_Options.Ragebot.YawTrue != 0)
			pCmd->viewangles.y = FakeAngles.y + g_Options.Ragebot.YawFakeAdder;
		if (!Globals::ySwitch && g_Options.Ragebot.YawFake != 0)
			pCmd->viewangles.y = SpinAngles.y + g_Options.Ragebot.YawTrueAdder;

	}
	else if (!g_Options.Ragebot.PreAAs && g_Options.Ragebot.BuilderAAs)
	{
		g_Engine->GetViewAngles(StartAngles);
		static int jitterangle;

		pCmd->viewangles.x = g_Options.Ragebot.BuilderPitch;

		if (Globals::ySwitch) //Builder FakeAngle
		{
			if (g_Options.Ragebot.FJitter)
			{
				pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake;
				if (jitterangle <= 1)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake + g_Options.Ragebot.FJitterRange;
					jitterangle += 1;
				}
				else if (jitterangle > 1 && jitterangle <= 3)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake - g_Options.Ragebot.FJitterRange;
					jitterangle += 1;
				}
				else
				{
					jitterangle = 0;
				}
			}
			else if (g_Options.Ragebot.FJitter && g_Options.Ragebot.LBYBreaker)
			{
				if (Globals::shouldflip)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake + 118;
				}
				else
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake;
					if (jitterangle <= 1)
					{
						pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake + g_Options.Ragebot.FJitterRange;
						jitterangle += 1;
					}
					else if (jitterangle > 1 && jitterangle <= 3)
					{
						pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake - g_Options.Ragebot.FJitterRange;
						jitterangle += 1;
					}
					else
					{
						jitterangle = 0;
					}
				}
			}
			else if (!g_Options.Ragebot.FJitter && g_Options.Ragebot.LBYBreaker)
			{
				if (Globals::shouldflip)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake + 118;
				}
				else
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake;
				}
			}
			else
			{
				pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderFake;
			}
		}
		if (!Globals::ySwitch) //Builder RealAngle
		{
			if (g_Options.Ragebot.Jitter)
			{
				pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal;
				if (jitterangle <= 1)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal + g_Options.Ragebot.JitterRange;
					jitterangle += 1;
				}
				else if (jitterangle > 1 && jitterangle <= 3)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal - g_Options.Ragebot.JitterRange;
					jitterangle += 1;
				}
				else
				{
					jitterangle = 0;
				}
			}
			else if (g_Options.Ragebot.Jitter && g_Options.Ragebot.LBYBreaker)
			{
				if (Globals::shouldflip)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal + 118;
				}
				else
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal;
					if (jitterangle <= 1)
					{
						pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal + g_Options.Ragebot.JitterRange;
						jitterangle += 1;
					}
					else if (jitterangle > 1 && jitterangle <= 3)
					{
						pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal - g_Options.Ragebot.JitterRange;
						jitterangle += 1;
					}
					else
					{
						jitterangle = 0;
					}
				}
			}
			else if (!g_Options.Ragebot.Jitter && g_Options.Ragebot.LBYBreaker)
			{
				if (Globals::shouldflip)
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal + 118;
				}
				else
				{
					pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal;
				}
			}
			else
			{
				pCmd->viewangles.y = StartAngles.y + g_Options.Ragebot.BuilderReal;
			}
		}

	}
	/*
	if (g_Options.Ragebot.PreAAs && g_Options.Ragebot.BuilderAAs)
	{
	Globals::error = true;
	}
	*/
	if (g_Options.Ragebot.PreAAs && !g_Options.Ragebot.BuilderAAs)
	{
		switch (g_Options.Ragebot.Pitch)
		{
		case 0:
			// No Pitch AA
			break;
		case 1:
			// Down
			pCmd->viewangles.x = 89 + g_Options.Ragebot.PitchAdder;
			break;
		case 2:
			pCmd->viewangles.x = -89 + g_Options.Ragebot.PitchAdder;
			break;
		case 3:
			pCmd->viewangles.x = -180 + g_Options.Ragebot.PitchAdder;
			break;
		case 4:
			pCmd->viewangles.x = 180 + g_Options.Ragebot.PitchAdder;
			break;
		case 5:
			if (Globals::shouldflip)
			{
				pCmd->viewangles.x = 60;
			}
			else if (!(Globals::shouldflip))
			{
				pCmd->viewangles.x = 89;
			}
			break;
		}
	}


}

void ragebot::FakeLag(CInput::CUserCmd* cmd)
{
	int iChoke = g_Options.Ragebot.FakeLagAmt;

	static int iFakeLag = -1;
	iFakeLag++;

	if (iFakeLag <= iChoke && iFakeLag > -1)
	{
		Globals::ySwitch = false;
	}
	else
	{
		Globals::ySwitch = true;
		iFakeLag = -1;
	}
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class gtqvsgt {
public:
	double aqxaqib;
	string bygnkxpqwjqackh;
	double uyxdiytnblsru;
	gtqvsgt();
	bool jqdthqgchgg();
	int kleysozozixcjednb(double oqgliagkhfeawk, string gjjmzevmp, bool gqotwdm, int mbqxtjefecz);
	void dwzgbtudcqnsgncbjpyayic(string lhlzlwtpjxvadh, bool cproev, int yhwhathlwgm, int slalyhxb, double ubngpprqwrrjfbr, int bthtll, bool uivbrk);
	string hdpszfadeyjximdfyqqem(int uzoaa, string ogrfffzjdpidi, double hkljaryyqobnzj, string kkqmfup, int cyhmbff, int juotxf, string nprzrbdbl);
	int mhlxehwsqgacpwkozhwgqx(int qecih, int dgczffd, string fhmwcjwx, string bbbdwvpbqa, bool fyifzubzlozgic);

protected:
	int ssdmwgszdyc;
	string zrlxg;

	void knvxojifulkn(string ejvhbswjkzu, string enjylgkb, int riawz, double vvoplbx, double npzgbcaimssfd, bool jxskjtvsbk);
	double iswlzsokibojbiuzdk();
	double whauxbotzspy(double vkrxuuycay, bool kdnyqhaiwpy, bool ahjeorwdsz, string vmhyudaniasr, bool ksozbdyha);
	bool eoqudbfhrwowsrazrtge(int dfcgrxgofug, double xvlkwp, int aizmcpvtuazouzc, string szpqqyexdn, string hqnouru, int cotnqycuvqtj, double qxfeslldwfayxt, bool ixpwwoum, double wmhgzur);
	string rhpzvgroff(string cjupbzrffgbikqk, int vmqmfrap, double qvaryh);
	int hrcvdaebdhaqwgbkpso(double gwnceutcsqkuvj, bool conbqctmtpjfh, bool oqwzbvtqu, bool qtglpfhc, double csxtyeykpnjj);
	void nvvtsbtcdbsunmescwmm(string ichffpgzp, int vopfv, double ttlacpzrqp, bool ybaceolozpl, double cwweo, bool eukpxgphcdrwd, double ohkmzpxi, double npgccxrdwniq, int sbpajkxylptmk, double cxfblaaagfgkbe);
	double biwwpvtmwickqentkrfzj(string xtreg, double vmmeifhlvvl, string qcxumx, int nhgkueyyyi, double zklhtkxtz, double yaqljuvwekbyd);

private:
	double pwtocmbcpmprd;
	double dmcgetwwxvxmzt;
	double lbyjgohodicfqz;
	int cpdykrsvhdgqe;
	int worutscjjrnunrv;

	double cxinzqqtwjduzpqvoxtgmb(int zwymqxtaawdvst, double dvrbsen, string zipeme, bool xpzqcyjahobvck, double oapkiwa, string qcbflpa);
	string kcffnvmqitjvuk(bool gcwujldyylldv, bool iajqolvfdzayglr, double lkqqgbn, double uwnsrgppwg, int tcaafnccixm, int prlwmhx, double plvnwxc, double ihcdcru, bool yfwnxiegt);
	string qoqobrmwtmwajdcocgvum(int npmctcapbuvhm, double rsolvznmy, bool voxdwn, bool oczeqovig, string rtiwo, int cxlbmac, double engvuqvtz, int ywnuuqntaimy, int ctdtwt);

};


double gtqvsgt::cxinzqqtwjduzpqvoxtgmb(int zwymqxtaawdvst, double dvrbsen, string zipeme, bool xpzqcyjahobvck, double oapkiwa, string qcbflpa) {
	bool mhmiycmjj = true;
	if (true == true) {
		int xzvsseb;
		for (xzvsseb = 86; xzvsseb > 0; xzvsseb--) {
			continue;
		}
	}
	if (true == true) {
		int icakli;
		for (icakli = 85; icakli > 0; icakli--) {
			continue;
		}
	}
	if (true != true) {
		int gyfi;
		for (gyfi = 49; gyfi > 0; gyfi--) {
			continue;
		}
	}
	if (true == true) {
		int vpqt;
		for (vpqt = 20; vpqt > 0; vpqt--) {
			continue;
		}
	}
	return 52390;
}

string gtqvsgt::kcffnvmqitjvuk(bool gcwujldyylldv, bool iajqolvfdzayglr, double lkqqgbn, double uwnsrgppwg, int tcaafnccixm, int prlwmhx, double plvnwxc, double ihcdcru, bool yfwnxiegt) {
	string anwubmh = "uycnnjxiszevgydaquewxtqnzxorupmcwsakjaowwxsylfxboztnm";
	int eivvxdzyozpzkbb = 5389;
	int pxdswxe = 1391;
	if (string("uycnnjxiszevgydaquewxtqnzxorupmcwsakjaowwxsylfxboztnm") != string("uycnnjxiszevgydaquewxtqnzxorupmcwsakjaowwxsylfxboztnm")) {
		int vfhnjmurqk;
		for (vfhnjmurqk = 63; vfhnjmurqk > 0; vfhnjmurqk--) {
			continue;
		}
	}
	if (5389 == 5389) {
		int xmekkm;
		for (xmekkm = 41; xmekkm > 0; xmekkm--) {
			continue;
		}
	}
	if (string("uycnnjxiszevgydaquewxtqnzxorupmcwsakjaowwxsylfxboztnm") == string("uycnnjxiszevgydaquewxtqnzxorupmcwsakjaowwxsylfxboztnm")) {
		int sclby;
		for (sclby = 66; sclby > 0; sclby--) {
			continue;
		}
	}
	if (5389 != 5389) {
		int wtsgahv;
		for (wtsgahv = 43; wtsgahv > 0; wtsgahv--) {
			continue;
		}
	}
	if (5389 == 5389) {
		int rn;
		for (rn = 89; rn > 0; rn--) {
			continue;
		}
	}
	return string("eyrffcioewyzeokhd");
}

string gtqvsgt::qoqobrmwtmwajdcocgvum(int npmctcapbuvhm, double rsolvznmy, bool voxdwn, bool oczeqovig, string rtiwo, int cxlbmac, double engvuqvtz, int ywnuuqntaimy, int ctdtwt) {
	string zcqhfaqaeixvgw = "jggrwdzvekxrbzqmplcxbzsxrqgvaguufgyezfhwvzdruvzdvbkplvkmhkyfqenemvkavubdw";
	string kwbrvsbtlzmhg = "stopvvgxnzvvjzgytnpruhlspxhduixsjjymwiueoyusvxzgelglukfczsbwzlrqazrgtoox";
	bool ehsuriwcutehroj = true;
	string txshlzdrvbzyocr = "rdetmlxhsyvupuujlsdhjedniwofsrpyrvisxnnxzgclojhyakococvkjvhdubiihckoubsxpaexkdnayv";
	string vafeh = "yqmsyebuwjtbmwwsqidnnhhiugrsnhqevbfviydytgjcwsgbfyiwqnokrewwxymxlgmoo";
	double aajtislbrkbgya = 9885;
	string ulhvlprqn = "gpudltzxmndik";
	bool stiyo = false;
	string yuvpbc = "psbsdwxclsqgkzdylqkuaukogqmvqnwbaunksonr";
	string onfmnctmaxnklr = "hhrgmrwywtsdieuiolwysiklglttkkvpvwpd";
	if (string("jggrwdzvekxrbzqmplcxbzsxrqgvaguufgyezfhwvzdruvzdvbkplvkmhkyfqenemvkavubdw") == string("jggrwdzvekxrbzqmplcxbzsxrqgvaguufgyezfhwvzdruvzdvbkplvkmhkyfqenemvkavubdw")) {
		int qa;
		for (qa = 75; qa > 0; qa--) {
			continue;
		}
	}
	if (string("jggrwdzvekxrbzqmplcxbzsxrqgvaguufgyezfhwvzdruvzdvbkplvkmhkyfqenemvkavubdw") == string("jggrwdzvekxrbzqmplcxbzsxrqgvaguufgyezfhwvzdruvzdvbkplvkmhkyfqenemvkavubdw")) {
		int rhfc;
		for (rhfc = 80; rhfc > 0; rhfc--) {
			continue;
		}
	}
	if (string("psbsdwxclsqgkzdylqkuaukogqmvqnwbaunksonr") != string("psbsdwxclsqgkzdylqkuaukogqmvqnwbaunksonr")) {
		int gjqh;
		for (gjqh = 45; gjqh > 0; gjqh--) {
			continue;
		}
	}
	if (string("stopvvgxnzvvjzgytnpruhlspxhduixsjjymwiueoyusvxzgelglukfczsbwzlrqazrgtoox") == string("stopvvgxnzvvjzgytnpruhlspxhduixsjjymwiueoyusvxzgelglukfczsbwzlrqazrgtoox")) {
		int iq;
		for (iq = 12; iq > 0; iq--) {
			continue;
		}
	}
	return string("wedn");
}

void gtqvsgt::knvxojifulkn(string ejvhbswjkzu, string enjylgkb, int riawz, double vvoplbx, double npzgbcaimssfd, bool jxskjtvsbk) {
	bool fbqhqpxyjqj = false;
	string xnizybosxet = "oombbdvzghiwxgx";
	if (string("oombbdvzghiwxgx") != string("oombbdvzghiwxgx")) {
		int ucs;
		for (ucs = 12; ucs > 0; ucs--) {
			continue;
		}
	}
	if (false != false) {
		int kheu;
		for (kheu = 58; kheu > 0; kheu--) {
			continue;
		}
	}
	if (false != false) {
		int gedslpwo;
		for (gedslpwo = 66; gedslpwo > 0; gedslpwo--) {
			continue;
		}
	}

}

double gtqvsgt::iswlzsokibojbiuzdk() {
	bool wzzmtdhnjqwbx = false;
	double rcowijktbcvc = 30360;
	string yyzaio = "";
	string pwaezxvb = "ycjgjemiucqecmxdwcornjzflcgoshfyvhjnuelstvvgezzolmncguuewxkcx";
	int besjywskrjd = 1867;
	if (false == false) {
		int oj;
		for (oj = 1; oj > 0; oj--) {
			continue;
		}
	}
	if (30360 != 30360) {
		int qronlbd;
		for (qronlbd = 31; qronlbd > 0; qronlbd--) {
			continue;
		}
	}
	return 51660;
}

double gtqvsgt::whauxbotzspy(double vkrxuuycay, bool kdnyqhaiwpy, bool ahjeorwdsz, string vmhyudaniasr, bool ksozbdyha) {
	string tndbrwaudv = "jdjqkyozubevxmhietbdefyncfxhewbuflzgzmjqlufhrzsjpricowmnwqzjzswsquitefcnjkxqulxalotmn";
	int uupsolms = 10;
	double yaafcnhrto = 21984;
	if (21984 != 21984) {
		int mmun;
		for (mmun = 50; mmun > 0; mmun--) {
			continue;
		}
	}
	return 74027;
}

bool gtqvsgt::eoqudbfhrwowsrazrtge(int dfcgrxgofug, double xvlkwp, int aizmcpvtuazouzc, string szpqqyexdn, string hqnouru, int cotnqycuvqtj, double qxfeslldwfayxt, bool ixpwwoum, double wmhgzur) {
	bool njapiye = true;
	string gnnxr = "bnbqbmscsevpvjjstwdgvgwtpwpsrqjfuicfcolawhoxipc";
	bool lvkgsirugbxfcw = false;
	if (false != false) {
		int vxgwedfgs;
		for (vxgwedfgs = 62; vxgwedfgs > 0; vxgwedfgs--) {
			continue;
		}
	}
	return true;
}

string gtqvsgt::rhpzvgroff(string cjupbzrffgbikqk, int vmqmfrap, double qvaryh) {
	double mpwdyiuponqlr = 21854;
	double swxufuvxpkdembz = 39681;
	string uxycwjtgyhfcuv = "oohbhtxqzxbyscxnjuqxwyujxdaapqvhktxhbtt";
	string scgwkef = "ucwfvrjyxsohmfafaviphudvrklvmkbempdotsuhekzkbwwiz";
	double mrlgo = 20132;
	if (string("ucwfvrjyxsohmfafaviphudvrklvmkbempdotsuhekzkbwwiz") == string("ucwfvrjyxsohmfafaviphudvrklvmkbempdotsuhekzkbwwiz")) {
		int ughv;
		for (ughv = 69; ughv > 0; ughv--) {
			continue;
		}
	}
	if (39681 == 39681) {
		int ffb;
		for (ffb = 35; ffb > 0; ffb--) {
			continue;
		}
	}
	if (21854 == 21854) {
		int bxq;
		for (bxq = 100; bxq > 0; bxq--) {
			continue;
		}
	}
	return string("fhtbdjodlbav");
}

int gtqvsgt::hrcvdaebdhaqwgbkpso(double gwnceutcsqkuvj, bool conbqctmtpjfh, bool oqwzbvtqu, bool qtglpfhc, double csxtyeykpnjj) {
	string hqkaniteirisp = "suxpcttknetnrsoyafuvbyhqs";
	int szgbpuixwgf = 793;
	double qzrqzjsbijuol = 28530;
	double szsemjrpq = 42460;
	int wwyhqqdhyfyghx = 1972;
	bool brzlfhsz = false;
	bool npdfydh = true;
	if (true != true) {
		int iwcbmehq;
		for (iwcbmehq = 5; iwcbmehq > 0; iwcbmehq--) {
			continue;
		}
	}
	return 28719;
}

void gtqvsgt::nvvtsbtcdbsunmescwmm(string ichffpgzp, int vopfv, double ttlacpzrqp, bool ybaceolozpl, double cwweo, bool eukpxgphcdrwd, double ohkmzpxi, double npgccxrdwniq, int sbpajkxylptmk, double cxfblaaagfgkbe) {
	int nbmpdcrkvvovlx = 483;
	bool ghbvmxreztuwigz = true;
	string nsazjlmlq = "yvitkfmufmcabkupdmarispdzlwrzbvuchngzuubvkizcrapndadtwrkhbvgrzxagfqy";
	bool zalpqttglcmms = true;
	string kshutqikn = "rrlahsiz";
	string howfu = "qghxywbssybsvxijoejvbzmxzoevkqjuqdauffufhbcwmfpedxnumapeozrczjrrwsuecogawwsmbtlsrzmzpgqguyswjrqweclq";
	int dqomxfzwr = 3180;
	if (string("yvitkfmufmcabkupdmarispdzlwrzbvuchngzuubvkizcrapndadtwrkhbvgrzxagfqy") == string("yvitkfmufmcabkupdmarispdzlwrzbvuchngzuubvkizcrapndadtwrkhbvgrzxagfqy")) {
		int ilunvc;
		for (ilunvc = 27; ilunvc > 0; ilunvc--) {
			continue;
		}
	}
	if (string("yvitkfmufmcabkupdmarispdzlwrzbvuchngzuubvkizcrapndadtwrkhbvgrzxagfqy") != string("yvitkfmufmcabkupdmarispdzlwrzbvuchngzuubvkizcrapndadtwrkhbvgrzxagfqy")) {
		int lxpoox;
		for (lxpoox = 98; lxpoox > 0; lxpoox--) {
			continue;
		}
	}
	if (true != true) {
		int fd;
		for (fd = 29; fd > 0; fd--) {
			continue;
		}
	}
	if (3180 != 3180) {
		int xk;
		for (xk = 1; xk > 0; xk--) {
			continue;
		}
	}
	if (3180 != 3180) {
		int msskn;
		for (msskn = 57; msskn > 0; msskn--) {
			continue;
		}
	}

}

double gtqvsgt::biwwpvtmwickqentkrfzj(string xtreg, double vmmeifhlvvl, string qcxumx, int nhgkueyyyi, double zklhtkxtz, double yaqljuvwekbyd) {
	return 45315;
}

bool gtqvsgt::jqdthqgchgg() {
	int kszsnmbyqo = 7020;
	int dfhowpkawleh = 6461;
	bool givmf = false;
	double hqrnz = 46888;
	if (7020 == 7020) {
		int arq;
		for (arq = 20; arq > 0; arq--) {
			continue;
		}
	}
	if (46888 == 46888) {
		int tizweja;
		for (tizweja = 95; tizweja > 0; tizweja--) {
			continue;
		}
	}
	return true;
}

int gtqvsgt::kleysozozixcjednb(double oqgliagkhfeawk, string gjjmzevmp, bool gqotwdm, int mbqxtjefecz) {
	double hfqyio = 31838;
	bool djcywdqu = true;
	bool sllzu = true;
	int xkevhszlflgzs = 6900;
	bool vvilo = false;
	double llavggoeaamfw = 47187;
	double aazxsxw = 35748;
	int ffeyrp = 2446;
	bool yuwqmveagd = false;
	if (false == false) {
		int cavdp;
		for (cavdp = 90; cavdp > 0; cavdp--) {
			continue;
		}
	}
	if (true != true) {
		int kg;
		for (kg = 87; kg > 0; kg--) {
			continue;
		}
	}
	if (2446 != 2446) {
		int ir;
		for (ir = 55; ir > 0; ir--) {
			continue;
		}
	}
	if (35748 != 35748) {
		int ubvvzwftt;
		for (ubvvzwftt = 46; ubvvzwftt > 0; ubvvzwftt--) {
			continue;
		}
	}
	if (47187 == 47187) {
		int xzjm;
		for (xzjm = 72; xzjm > 0; xzjm--) {
			continue;
		}
	}
	return 63034;
}

void gtqvsgt::dwzgbtudcqnsgncbjpyayic(string lhlzlwtpjxvadh, bool cproev, int yhwhathlwgm, int slalyhxb, double ubngpprqwrrjfbr, int bthtll, bool uivbrk) {
	double haztvadmuxpjxj = 7510;
	int utuaiasyiuwcml = 5770;
	int jzwjdpcubaem = 8162;
	int ofhgelbhlv = 4984;
	string vyxxvca = "qwjgunjukyatdqtfwodxmuwwndnnfbjkobxjxsgjxpiduyqqhmutsomlkqoeqzmcencibsmwhdfzdxigqzifshxvwaltdggdvp";
	int ujcijsynfr = 3469;
	bool joegha = true;
	if (string("qwjgunjukyatdqtfwodxmuwwndnnfbjkobxjxsgjxpiduyqqhmutsomlkqoeqzmcencibsmwhdfzdxigqzifshxvwaltdggdvp") == string("qwjgunjukyatdqtfwodxmuwwndnnfbjkobxjxsgjxpiduyqqhmutsomlkqoeqzmcencibsmwhdfzdxigqzifshxvwaltdggdvp")) {
		int jsslsjre;
		for (jsslsjre = 50; jsslsjre > 0; jsslsjre--) {
			continue;
		}
	}

}

string gtqvsgt::hdpszfadeyjximdfyqqem(int uzoaa, string ogrfffzjdpidi, double hkljaryyqobnzj, string kkqmfup, int cyhmbff, int juotxf, string nprzrbdbl) {
	double aydwjybd = 10931;
	double jamnfkn = 9750;
	bool exypxb = true;
	double xxwvkqz = 49374;
	int eehkkhhbu = 652;
	string ougascwhsb = "ygpjpledxsfrpigzljtzhqaiagfqpujxkgabkxltffcwhzdixjjbxahxkfgpzqhcntonwowepwziqsllorqvqisfbuhzbaeb";
	if (string("ygpjpledxsfrpigzljtzhqaiagfqpujxkgabkxltffcwhzdixjjbxahxkfgpzqhcntonwowepwziqsllorqvqisfbuhzbaeb") != string("ygpjpledxsfrpigzljtzhqaiagfqpujxkgabkxltffcwhzdixjjbxahxkfgpzqhcntonwowepwziqsllorqvqisfbuhzbaeb")) {
		int qdeom;
		for (qdeom = 24; qdeom > 0; qdeom--) {
			continue;
		}
	}
	if (49374 != 49374) {
		int gtqistyg;
		for (gtqistyg = 85; gtqistyg > 0; gtqistyg--) {
			continue;
		}
	}
	if (true != true) {
		int tbltoipg;
		for (tbltoipg = 26; tbltoipg > 0; tbltoipg--) {
			continue;
		}
	}
	if (49374 != 49374) {
		int quaz;
		for (quaz = 44; quaz > 0; quaz--) {
			continue;
		}
	}
	return string("fcekfphjphjtisgjylaj");
}

int gtqvsgt::mhlxehwsqgacpwkozhwgqx(int qecih, int dgczffd, string fhmwcjwx, string bbbdwvpbqa, bool fyifzubzlozgic) {
	string wkhyre = "wuousfolpqimccfupygggnldjrmldmwfzhmsutobtkksslfeqlgnznrmwutbyzltvvabgarwxya";
	double kptwsjmeiommdf = 4501;
	string plgbnudrjkeq = "irvdtkgkfztqaqgyvlytifvsxpfhcssqhmgfjwzx";
	bool rkpcdzpi = false;
	int owxcvdjqty = 1541;
	int mfjagjryuaiqt = 3471;
	bool kfoth = true;
	string rohvfkwkjylkje = "tgzs";
	double aqjiebdlbijjbyx = 5147;
	if (string("irvdtkgkfztqaqgyvlytifvsxpfhcssqhmgfjwzx") == string("irvdtkgkfztqaqgyvlytifvsxpfhcssqhmgfjwzx")) {
		int gxinszxqo;
		for (gxinszxqo = 80; gxinszxqo > 0; gxinszxqo--) {
			continue;
		}
	}
	if (string("tgzs") != string("tgzs")) {
		int almcnuob;
		for (almcnuob = 14; almcnuob > 0; almcnuob--) {
			continue;
		}
	}
	if (string("wuousfolpqimccfupygggnldjrmldmwfzhmsutobtkksslfeqlgnznrmwutbyzltvvabgarwxya") == string("wuousfolpqimccfupygggnldjrmldmwfzhmsutobtkksslfeqlgnznrmwutbyzltvvabgarwxya")) {
		int eqcpbnchq;
		for (eqcpbnchq = 28; eqcpbnchq > 0; eqcpbnchq--) {
			continue;
		}
	}
	return 14906;
}

gtqvsgt::gtqvsgt() {
	this->jqdthqgchgg();
	this->kleysozozixcjednb(7651, string("uhbjjwuyvmmbuhwppcvqanfije"), false, 3275);
	this->dwzgbtudcqnsgncbjpyayic(string("pgdzfxzerklwtfxbkbjubdzxgilaosx"), false, 6399, 7133, 56677, 2581, false);
	this->hdpszfadeyjximdfyqqem(3191, string("r"), 60326, string("jryyfpvxwfbprgneyfeoozcycyrwogxkoqwiwyvjdnahleogtgfttgzheehwpjhckpehvmobnknhtcohkaoazaidmvbfwp"), 2980, 4905, string("pvbrxbnzklsiriojxlfknypdpcxqrcfukhfwhmorbcfhhquis"));
	this->mhlxehwsqgacpwkozhwgqx(4231, 4419, string("gxajmahdxhmvvjkvgyiylznudymqsjwrkjtumepmqfloxnehs"), string("kazlwrzbkblglwmluxqeckdpkeblqhpfacom"), false);
	this->knvxojifulkn(string("hmxjeitvjdilimwthkkcjbsrgpduwglexlifcpofjtudobrzhnapgggdajqmdjmrwlwbdouspp"), string("tsvvkgknlfhixeujlawetqrbttuueyegxqypfgz"), 80, 15094, 59849, false);
	this->iswlzsokibojbiuzdk();
	this->whauxbotzspy(6501, false, true, string("ombemoqeosjbtwrhifbhvkygwxlodypgfcsii"), false);
	this->eoqudbfhrwowsrazrtge(3429, 3639, 795, string("mbixprzjrjgjajyypeavjhrppdjnirjnlliueqrcvyzfriaifkrgblrsmqkwwyywpautmkuq"), string("kqskgtqmrfyyvrakkdu"), 361, 11089, true, 34315);
	this->rhpzvgroff(string("rfpsrjzezuycxsqspidaedvcalyfxzphmxfdzwsuclxfspujegtqtishndmwtyocskftsgfzcwekr"), 1172, 22067);
	this->hrcvdaebdhaqwgbkpso(2172, true, false, false, 58436);
	this->nvvtsbtcdbsunmescwmm(string("pqmxkzzbhpesvlvcthaqazovkhedialkufmlh"), 1607, 33959, true, 7237, true, 20848, 42776, 849, 16698);
	this->biwwpvtmwickqentkrfzj(string("xinmehggwagnufrdwdisjjzcngb"), 39298, string("jrgfbvjlnqccifywbebvamsmbnfbglsmpvztjccpdbsktmfiycaizhdzxgrdhwkgaaoswdzdmitrddpkjji"), 7277, 13136, 27022);
	this->cxinzqqtwjduzpqvoxtgmb(319, 1989, string("jdqkagxlwayiftcwhqdglglluuhnaxvwinknicdllqhpmywnhcneohvwvdelbzwfyhkqtoytcxqqmtwlmiotlxnkszpvs"), true, 22171, string("ehfwlmzkidqwnfuzklshwthzumfnyzedjhnmrbpbwzpostbjffrvywvzrorhxzfrjntnhqtvi"));
	this->kcffnvmqitjvuk(false, true, 12534, 26845, 848, 9559, 11162, 14797, true);
	this->qoqobrmwtmwajdcocgvum(2949, 30458, false, true, string("hfccvjfnkltijxfzexqzdypgtbzhcbtnobraugkewbgmbmpmfedalkowjzk"), 2617, 2369, 862, 3516);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class fahqyrz {
public:
	double lmptmnrkzvf;
	int pnuhboroq;
	bool gzsfqf;
	string ykhvnqadiqdwhks;
	fahqyrz();
	int unubseessloawl(int wbysuwmypzbxy, string bubbpkq, bool jzqjr, bool hzihaauvlx, int rifws, int gmamkublshh, double fxdphfnk, string emogk, bool ytqqdufayrcvgo);
	double itvhflkarhgupp(string ojbvrbfqcote, double fgloyagfpz, bool tddbotkh, int smdbunnqidqdcx, double zngwfdmvnr, int fyzodub, int yncpljtkpudw);
	void tuntjeucyiy(int vdsenakbcljjz);
	string bsnirtjwadlwskgru(double mebgkmnchro);

protected:
	double wzncrtot;
	bool blvjdrlf;
	double jigxgx;

	bool odfzjrubij(int kpztkcyg);
	int gfceetgtjyuqaukygxml(string wjvnot);
	int cmxfqxmefp(int rjblwwhbqcv, double cjygfqe, double ekgxdwcnp, double mhhumqrxbwsg, bool pordfpogctzld, int exvbacifhpjgfh, int lrhuwbbbtuqe, int jpzkla);
	double irmhtfzjvbctpovzglrfmpi(int zcthjwjanfw, int usgixgcchzxxq, bool vlxrkk, int yxkqp, int unewgbnz, int vfenimplsyecp, double mteitewjpi, bool stojlxb);

private:
	double jxficogb;
	string zsfvxjyukptr;

	int vpfskwzvcepansqnbnqfw(double uheeojpelmdl, double xeyudohyyovew, string juyljvqyu, string vragqxkhmptqgdm, double qikktlikk);
	string zdwxiaboecdrrdualbflrydl(bool ywebfopzpdvbzy, double hpffvffua, int akmoq, string fohhlrz);
	string oibbpinprjcubqywicaxvybr(string rohigszy, int cptfid);
	double kdtievujuaxkxy(double zqzsrzabyrvh, int hctvglvle, int tnvibslhlqfyw, string jxfgfnmbmlrqiq, string wgpqwadtkijykfx, string runjnfe, int qcuwqwzxglxdbw, bool aupisucavjz, string sqgxtts, double qnbjob);
	int dklhwmrynstevwwjz(string tlhlyafclotxfg, bool aitsdw, double vxnpowuwyyspsn, string drtkvxwmiurrvqw, int hkyucdc, string eorbmxs);
	string xymnschxlaostolz(int ufhyfwjhmi, double kugdqnqnb);

};


int fahqyrz::vpfskwzvcepansqnbnqfw(double uheeojpelmdl, double xeyudohyyovew, string juyljvqyu, string vragqxkhmptqgdm, double qikktlikk) {
	double mmabnpbnsgrnqop = 35702;
	int fwrhyijw = 1214;
	if (35702 != 35702) {
		int lntcynevtu;
		for (lntcynevtu = 92; lntcynevtu > 0; lntcynevtu--) {
			continue;
		}
	}
	return 14936;
}

string fahqyrz::zdwxiaboecdrrdualbflrydl(bool ywebfopzpdvbzy, double hpffvffua, int akmoq, string fohhlrz) {
	int ojthd = 5261;
	bool vzmdto = true;
	int yenegkf = 4480;
	int anlknujg = 1026;
	bool mfhwbysii = true;
	string sytqagaccbyvz = "cwzxeudqyqwqtisnmpzzqjzjzhacrklvwuodmlgbpflmxohysnqywzqtmst";
	double ipynwktqobtigql = 12911;
	double jedxhczd = 5169;
	bool zwmifvnsw = false;
	string vxkvcqfvnhit = "ypqduoxxxnaldthaplb";
	if (true == true) {
		int hgbbynaya;
		for (hgbbynaya = 37; hgbbynaya > 0; hgbbynaya--) {
			continue;
		}
	}
	return string("hvakzjkbxkgoldazzxk");
}

string fahqyrz::oibbpinprjcubqywicaxvybr(string rohigszy, int cptfid) {
	double ihsegfvjr = 40803;
	int nnpmc = 3515;
	int ctttvleucsh = 4318;
	string vtzdxrsdrx = "atkftrpvvzhlcvpqoggoyhoutsfctgbqmjnnwebevxwbeklphiekwulsvbyfzubatixojybecerwzmauf";
	if (string("atkftrpvvzhlcvpqoggoyhoutsfctgbqmjnnwebevxwbeklphiekwulsvbyfzubatixojybecerwzmauf") == string("atkftrpvvzhlcvpqoggoyhoutsfctgbqmjnnwebevxwbeklphiekwulsvbyfzubatixojybecerwzmauf")) {
		int gjxuposszu;
		for (gjxuposszu = 4; gjxuposszu > 0; gjxuposszu--) {
			continue;
		}
	}
	if (4318 != 4318) {
		int dahazj;
		for (dahazj = 73; dahazj > 0; dahazj--) {
			continue;
		}
	}
	if (4318 == 4318) {
		int wcjnkflog;
		for (wcjnkflog = 13; wcjnkflog > 0; wcjnkflog--) {
			continue;
		}
	}
	if (4318 != 4318) {
		int nm;
		for (nm = 8; nm > 0; nm--) {
			continue;
		}
	}
	if (3515 != 3515) {
		int flc;
		for (flc = 12; flc > 0; flc--) {
			continue;
		}
	}
	return string("xiwempgqtcda");
}

double fahqyrz::kdtievujuaxkxy(double zqzsrzabyrvh, int hctvglvle, int tnvibslhlqfyw, string jxfgfnmbmlrqiq, string wgpqwadtkijykfx, string runjnfe, int qcuwqwzxglxdbw, bool aupisucavjz, string sqgxtts, double qnbjob) {
	int vsonqzvk = 936;
	string rcjvb = "kxwxhepazywzrafcbmcxzgbrihbgsajayijcioqcrcnyjpusuwealyxudeivxjwacpz";
	if (936 != 936) {
		int daddki;
		for (daddki = 49; daddki > 0; daddki--) {
			continue;
		}
	}
	if (string("kxwxhepazywzrafcbmcxzgbrihbgsajayijcioqcrcnyjpusuwealyxudeivxjwacpz") != string("kxwxhepazywzrafcbmcxzgbrihbgsajayijcioqcrcnyjpusuwealyxudeivxjwacpz")) {
		int bcmg;
		for (bcmg = 11; bcmg > 0; bcmg--) {
			continue;
		}
	}
	if (string("kxwxhepazywzrafcbmcxzgbrihbgsajayijcioqcrcnyjpusuwealyxudeivxjwacpz") != string("kxwxhepazywzrafcbmcxzgbrihbgsajayijcioqcrcnyjpusuwealyxudeivxjwacpz")) {
		int mmwgjpmfb;
		for (mmwgjpmfb = 7; mmwgjpmfb > 0; mmwgjpmfb--) {
			continue;
		}
	}
	return 70525;
}

int fahqyrz::dklhwmrynstevwwjz(string tlhlyafclotxfg, bool aitsdw, double vxnpowuwyyspsn, string drtkvxwmiurrvqw, int hkyucdc, string eorbmxs) {
	bool sswvxgpyfrxcejj = true;
	double jfktek = 9963;
	double hirwbmqb = 17360;
	double vqwhfkpabvimo = 700;
	int lgcaiddgu = 7870;
	bool cbijnpuidnt = false;
	string meypfqjqidalv = "thzlarnkmvpyjutptgtuomhgraqnwsphpgoeeeonpuohfbrtnwghfuzojecgk";
	return 92030;
}

string fahqyrz::xymnschxlaostolz(int ufhyfwjhmi, double kugdqnqnb) {
	string hlpqcyqchlksw = "oidzduxrohrvshnxsergagyheeamggozqydrhwyrfwadbnlcxlxlxllblwrpdbu";
	double dsinug = 10323;
	string lhpcbjkjyycgj = "sexbfat";
	if (10323 != 10323) {
		int vkf;
		for (vkf = 53; vkf > 0; vkf--) {
			continue;
		}
	}
	if (string("sexbfat") != string("sexbfat")) {
		int ofjubpkq;
		for (ofjubpkq = 55; ofjubpkq > 0; ofjubpkq--) {
			continue;
		}
	}
	if (10323 != 10323) {
		int yjeebsfn;
		for (yjeebsfn = 10; yjeebsfn > 0; yjeebsfn--) {
			continue;
		}
	}
	if (string("oidzduxrohrvshnxsergagyheeamggozqydrhwyrfwadbnlcxlxlxllblwrpdbu") == string("oidzduxrohrvshnxsergagyheeamggozqydrhwyrfwadbnlcxlxlxllblwrpdbu")) {
		int iopllzj;
		for (iopllzj = 20; iopllzj > 0; iopllzj--) {
			continue;
		}
	}
	if (10323 != 10323) {
		int ia;
		for (ia = 80; ia > 0; ia--) {
			continue;
		}
	}
	return string("lwnf");
}

bool fahqyrz::odfzjrubij(int kpztkcyg) {
	bool smmexseyeklgphh = false;
	bool cckmssiwrwew = false;
	double tmhhgvs = 45978;
	if (false != false) {
		int pzyz;
		for (pzyz = 72; pzyz > 0; pzyz--) {
			continue;
		}
	}
	if (45978 != 45978) {
		int nmh;
		for (nmh = 20; nmh > 0; nmh--) {
			continue;
		}
	}
	if (false != false) {
		int zzeeloyz;
		for (zzeeloyz = 45; zzeeloyz > 0; zzeeloyz--) {
			continue;
		}
	}
	if (45978 == 45978) {
		int flw;
		for (flw = 86; flw > 0; flw--) {
			continue;
		}
	}
	if (false != false) {
		int nwaazkpbbj;
		for (nwaazkpbbj = 99; nwaazkpbbj > 0; nwaazkpbbj--) {
			continue;
		}
	}
	return false;
}

int fahqyrz::gfceetgtjyuqaukygxml(string wjvnot) {
	bool uhmjawynyo = true;
	double tkoiyeymaqaece = 28236;
	double wtveewq = 45049;
	int rjiyvio = 2715;
	if (true != true) {
		int uayd;
		for (uayd = 20; uayd > 0; uayd--) {
			continue;
		}
	}
	if (2715 == 2715) {
		int skilzplu;
		for (skilzplu = 23; skilzplu > 0; skilzplu--) {
			continue;
		}
	}
	if (true != true) {
		int nva;
		for (nva = 98; nva > 0; nva--) {
			continue;
		}
	}
	if (true == true) {
		int wyrd;
		for (wyrd = 55; wyrd > 0; wyrd--) {
			continue;
		}
	}
	if (45049 != 45049) {
		int mu;
		for (mu = 26; mu > 0; mu--) {
			continue;
		}
	}
	return 88356;
}

int fahqyrz::cmxfqxmefp(int rjblwwhbqcv, double cjygfqe, double ekgxdwcnp, double mhhumqrxbwsg, bool pordfpogctzld, int exvbacifhpjgfh, int lrhuwbbbtuqe, int jpzkla) {
	return 69091;
}

double fahqyrz::irmhtfzjvbctpovzglrfmpi(int zcthjwjanfw, int usgixgcchzxxq, bool vlxrkk, int yxkqp, int unewgbnz, int vfenimplsyecp, double mteitewjpi, bool stojlxb) {
	bool ubbztkcygoi = false;
	string gxkibv = "fcznvcgoexebpuhdbqpxg";
	double fxrxtdkqoepve = 45300;
	string wrmuv = "xfuvgguwiepvoubmbcrhihyjencliakegqbulrinrbiindfdpdbghkzsrjzlsnvz";
	string hdqudtuazs = "scibxoqafvqkvmsaglebjxgzbabfvjjstfwzahtxfrjfpeyfsqi";
	bool ibuaagszzxagrgr = true;
	if (45300 == 45300) {
		int tnngmptj;
		for (tnngmptj = 28; tnngmptj > 0; tnngmptj--) {
			continue;
		}
	}
	if (false == false) {
		int bjwkrjljy;
		for (bjwkrjljy = 34; bjwkrjljy > 0; bjwkrjljy--) {
			continue;
		}
	}
	if (string("fcznvcgoexebpuhdbqpxg") != string("fcznvcgoexebpuhdbqpxg")) {
		int ajavogblgk;
		for (ajavogblgk = 77; ajavogblgk > 0; ajavogblgk--) {
			continue;
		}
	}
	if (true == true) {
		int onwaiowrk;
		for (onwaiowrk = 67; onwaiowrk > 0; onwaiowrk--) {
			continue;
		}
	}
	return 44305;
}

int fahqyrz::unubseessloawl(int wbysuwmypzbxy, string bubbpkq, bool jzqjr, bool hzihaauvlx, int rifws, int gmamkublshh, double fxdphfnk, string emogk, bool ytqqdufayrcvgo) {
	double jlkml = 4409;
	int nblurhqx = 824;
	string oqmmfnmys = "mihpxeiqmkvrminsdtoymcufdcjjowspfamtcmucvrvxyptxvzimtljsyhn";
	bool qslqdcdpj = true;
	int aqpoqexuysuusq = 2987;
	double wolgsvcg = 73216;
	bool xsoeta = true;
	bool xpcju = true;
	if (true == true) {
		int sznwunvmvv;
		for (sznwunvmvv = 31; sznwunvmvv > 0; sznwunvmvv--) {
			continue;
		}
	}
	return 45605;
}

double fahqyrz::itvhflkarhgupp(string ojbvrbfqcote, double fgloyagfpz, bool tddbotkh, int smdbunnqidqdcx, double zngwfdmvnr, int fyzodub, int yncpljtkpudw) {
	int agtshzjtw = 511;
	if (511 != 511) {
		int dldxobb;
		for (dldxobb = 83; dldxobb > 0; dldxobb--) {
			continue;
		}
	}
	if (511 == 511) {
		int fi;
		for (fi = 40; fi > 0; fi--) {
			continue;
		}
	}
	if (511 != 511) {
		int vmac;
		for (vmac = 1; vmac > 0; vmac--) {
			continue;
		}
	}
	if (511 != 511) {
		int rbvef;
		for (rbvef = 83; rbvef > 0; rbvef--) {
			continue;
		}
	}
	if (511 == 511) {
		int emcq;
		for (emcq = 21; emcq > 0; emcq--) {
			continue;
		}
	}
	return 65807;
}

void fahqyrz::tuntjeucyiy(int vdsenakbcljjz) {

}

string fahqyrz::bsnirtjwadlwskgru(double mebgkmnchro) {
	bool bmodzfnsh = true;
	bool mcolkhtaqfdf = false;
	bool zyateuivn = true;
	int avvtjzpggxfgqdd = 222;
	int ijnzpuhe = 4312;
	bool wbxvi = true;
	string cwymrvjl = "cjaiswapehmthktrphujvtcxtykchcvttdfpxyrqbalhlfckyqvfftuigqo";
	double wsmtgr = 9223;
	if (false != false) {
		int rd;
		for (rd = 83; rd > 0; rd--) {
			continue;
		}
	}
	if (true != true) {
		int nbitqsxc;
		for (nbitqsxc = 80; nbitqsxc > 0; nbitqsxc--) {
			continue;
		}
	}
	if (222 == 222) {
		int thuqp;
		for (thuqp = 42; thuqp > 0; thuqp--) {
			continue;
		}
	}
	if (true != true) {
		int tllrpsdlj;
		for (tllrpsdlj = 59; tllrpsdlj > 0; tllrpsdlj--) {
			continue;
		}
	}
	return string("nnqpxortekb");
}

fahqyrz::fahqyrz() {
	this->unubseessloawl(2153, string("ujnxlrqcixnjnlmosvvtptwuclnlkovoswabjxkellaovjggvyntwuavr"), true, true, 1167, 4807, 20892, string("eaezrxhkscapvlyjdfpezdauqndlvgiggwprlygoye"), false);
	this->itvhflkarhgupp(string("qrvgjrgnqvsrrennyxvjgoftcmhx"), 10249, false, 1720, 51316, 970, 5845);
	this->tuntjeucyiy(515);
	this->bsnirtjwadlwskgru(17505);
	this->odfzjrubij(4742);
	this->gfceetgtjyuqaukygxml(string("kjoxywbydelxfrtnvgvqlorahvbtitpqqloomfxkyeiqeulvgkehwmlfqeiz"));
	this->cmxfqxmefp(499, 33038, 42002, 43417, false, 1043, 94, 2768);
	this->irmhtfzjvbctpovzglrfmpi(1437, 1542, true, 1873, 5849, 710, 2970, false);
	this->vpfskwzvcepansqnbnqfw(4643, 15173, string("bwsywptcjerlowelkrwjpupzrqsqifmwmfqarqrhultajueipimnqntqazhkunbkolkyuumhfcrmkqsrjj"), string("abbustvuqkdzsgvrbtexwxyigebvmjgfrvkugowibylrr"), 4768);
	this->zdwxiaboecdrrdualbflrydl(true, 15968, 2839, string("frbtscmtqarnbvr"));
	this->oibbpinprjcubqywicaxvybr(string("lgpmbuokkzwvhdvavmwgjqpttqakajlnpjlphgnjvdv"), 2589);
	this->kdtievujuaxkxy(63447, 157, 585, string("lhbvfttfdxvbffvecfkkuyvbrfbbmacoenqpqmdvdptqppycrpekujzdqbanhczdcytlbvvueev"), string("giareirdcchetkvvuealxwttyahwxkhowuvkffgzlsaqqolphbegygurlgzsrmxxpqlzolmzgabpndrhtwhjhlmkexh"), string("itesvdijysmeaqqugvypvhbh"), 5180, true, string("ieioafoumnpdweryveoivqguipeaqmiptusaxsxzatzntkdtybbwheso"), 14445);
	this->dklhwmrynstevwwjz(string("kjzgifkqqznigromqkorcfiuijuqiuvfdwbvqapdcsybxxef"), false, 15939, string("fgaxxfvtyqqenjoakuwevuzzsgealfqyuvtqtvtklgwbjfhsmp"), 453, string("chnhjeisobaadxsbdmptqfhfvl"));
	this->xymnschxlaostolz(305, 31827);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jcscpdf {
public:
	int qlblnihh;
	string ydcbz;
	double fdxmsdluwbp;
	jcscpdf();
	double fhbnojrdbsp(int zuubggifhop, string gtofwf);
	string isiftsxgjkxz(bool dpgmjecnmtmnulb, bool vafawjcvnqax, bool rzwhrjtsfsdfek, int cxqauoiogjli, double rhhclbn, string cvtfh);
	string kkxhziowvreo(int kznvmhdfejkkhdl, string chdngcejdi, string zaqgsiywdywd, bool wczgaugawqihoqm, bool owujnqdetxjaqsa, int vtslhy);
	void blgehqzeznwcpuyfndrubavs(bool ofzhutfd, double yddzfvnwkdramf, string hoqbjxrlztsntoj);
	bool afzhonevrcgoadfp(bool yjdvw, bool rasdyuhusfpmaja, int hsibbwgoomf, int rfxszshvrsvevl, int gkaojasptbwtd, int sdkvc, double ptzxjcakunzesb, string yxpuosaarcrv, bool gdmkuktyklkc);

protected:
	string igisqcsq;
	bool ybgfwmhahqp;
	bool mboeojoehmtvv;
	bool mcchshbkkgz;

	string pylcvlnfskdwxwxdv(double ufgvf, int dkebmdqjp, bool xmyxdybhtaik, bool kqtpwm, int ajzjdpozrnoijd);
	string xuoawhldowtmcpkfkphzwlm(bool aglofhwlxokt, double swhti);

private:
	string nregrwgfdwuy;
	int gswanvvncpyfmfe;
	bool qrqsarmizt;

	double jfpfznpthrlx(int ujtfvkxvunm);
	bool yrfqfmbucsjatlqtsvdesay(double tcuskyps, bool pemmidyevyd);
	int xfpjzgyzwr(double wnazl, double khfqxonwpzkt, bool hwembxbjfua, int kkwanqut, int cnxfifrreirzjm, string ljoioyutq, double gsvunarcrb, double bgfeokcaras, double zbepomqh, int wmbghhiatfaxf);
	double deljzgdbjmayriitsv(bool kubndszujhquuad, bool gkhzw, double plkprctgdcajvm, bool etquqtkfk);
	double rvphxgefkuldttzcujbfpbn(bool iognbcvfadpf, int lhxyybfzkcrcphi, double bnkaldkw, int oemoudi, bool fyajacrhewu, double edllopyxrlsxe, int aouukbjsjy);
	void wollpmdmohridfnancj(double bhnbduftj, bool abstvesn, double icodqegs, int tptpfm, double brhruv, int pmtnxudcqsq, bool pjefbkrjbwddchi, string oosnpo, double ubgxsrin);
	int gcwnvrachdifsjp();
	bool nuysmvncfihy(bool ifvgntdklfxrfh, string gyefpdptzsfm, bool zgokdprveb, double bkjqhuzcwi, bool hxrlidsinukcmyo);

};


double jcscpdf::jfpfznpthrlx(int ujtfvkxvunm) {
	double sweifiyux = 23689;
	bool lxfhgkzzlrcdc = false;
	bool jdnuqclgudzsaa = false;
	if (23689 == 23689) {
		int gwyhhigrht;
		for (gwyhhigrht = 22; gwyhhigrht > 0; gwyhhigrht--) {
			continue;
		}
	}
	if (23689 != 23689) {
		int xnqwjspldz;
		for (xnqwjspldz = 72; xnqwjspldz > 0; xnqwjspldz--) {
			continue;
		}
	}
	if (false != false) {
		int wewcrhmek;
		for (wewcrhmek = 50; wewcrhmek > 0; wewcrhmek--) {
			continue;
		}
	}
	return 14072;
}

bool jcscpdf::yrfqfmbucsjatlqtsvdesay(double tcuskyps, bool pemmidyevyd) {
	bool xnmiqjjejsutun = false;
	int zszpjy = 6823;
	if (6823 == 6823) {
		int ygmwcpnde;
		for (ygmwcpnde = 87; ygmwcpnde > 0; ygmwcpnde--) {
			continue;
		}
	}
	if (6823 != 6823) {
		int dynnqd;
		for (dynnqd = 51; dynnqd > 0; dynnqd--) {
			continue;
		}
	}
	if (false == false) {
		int jlydfrwg;
		for (jlydfrwg = 5; jlydfrwg > 0; jlydfrwg--) {
			continue;
		}
	}
	if (false == false) {
		int ompwmajmgl;
		for (ompwmajmgl = 60; ompwmajmgl > 0; ompwmajmgl--) {
			continue;
		}
	}
	if (false != false) {
		int yzicnh;
		for (yzicnh = 66; yzicnh > 0; yzicnh--) {
			continue;
		}
	}
	return true;
}

int jcscpdf::xfpjzgyzwr(double wnazl, double khfqxonwpzkt, bool hwembxbjfua, int kkwanqut, int cnxfifrreirzjm, string ljoioyutq, double gsvunarcrb, double bgfeokcaras, double zbepomqh, int wmbghhiatfaxf) {
	string efzcaf = "dnhyuifgeyqnxmkecqpuksnzzksxtqhnetkbuneyrcimnnshnerutwuqcei";
	bool rhjpzherjkrpeuy = false;
	double geuosnc = 33978;
	string otsnifzfoam = "anhdhndqvumgofcwyewscxejzdlbzhamftyzmtssszudovngkbqxcoa";
	int gpvxnjxtvmu = 5970;
	string iwahtfi = "frglfyuvxckejbnskmjarjxemttrkwdoavfraonbnqmazzvdkejnerhslhigedrx";
	int jjbjgtdw = 2343;
	int dcpgqegahtyuoz = 1591;
	string vwwfpig = "anwvgrjjjewtzomztjtrdqbandmthpppintvqedoyxqyiovqeedaolrcknbvlskpxoboubbcky";
	return 75012;
}

double jcscpdf::deljzgdbjmayriitsv(bool kubndszujhquuad, bool gkhzw, double plkprctgdcajvm, bool etquqtkfk) {
	string qelockfb = "csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh";
	if (string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh") == string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh")) {
		int ggofrw;
		for (ggofrw = 60; ggofrw > 0; ggofrw--) {
			continue;
		}
	}
	if (string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh") == string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh")) {
		int ownczuvp;
		for (ownczuvp = 66; ownczuvp > 0; ownczuvp--) {
			continue;
		}
	}
	if (string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh") == string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh")) {
		int iu;
		for (iu = 54; iu > 0; iu--) {
			continue;
		}
	}
	if (string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh") == string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh")) {
		int xtk;
		for (xtk = 45; xtk > 0; xtk--) {
			continue;
		}
	}
	if (string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh") != string("csbpcyvteopmdpitwyvyakqykqonrmzbftriehfzrwdnkgrgezdjyantuwomgmspkpkethlxwsniqebglczh")) {
		int ejaefxnu;
		for (ejaefxnu = 83; ejaefxnu > 0; ejaefxnu--) {
			continue;
		}
	}
	return 25147;
}

double jcscpdf::rvphxgefkuldttzcujbfpbn(bool iognbcvfadpf, int lhxyybfzkcrcphi, double bnkaldkw, int oemoudi, bool fyajacrhewu, double edllopyxrlsxe, int aouukbjsjy) {
	return 39501;
}

void jcscpdf::wollpmdmohridfnancj(double bhnbduftj, bool abstvesn, double icodqegs, int tptpfm, double brhruv, int pmtnxudcqsq, bool pjefbkrjbwddchi, string oosnpo, double ubgxsrin) {
	string zplvxkbu = "fenmjyxtnkslomv";
	string pyjfwgnl = "jnwfsnkrwivmuubahygqbbshpfddoxrdhjnliijbqaburbpdrzy";
	double qnrobx = 45966;
	double wguuhzbzyjhupz = 27044;
	string yuabrtljn = "tsvemlekpetixcrelusanfy";
	int pykcymilpn = 5666;
	string osfyvffotvkx = "unigwmfoqqyhwpegxqgqqllzveqhujhmwbikzvlqokdbcqqp";
	string yneenmzwbekircg = "rnuwarczcgfgcwndxiwwubflomiphxsmveahfxvfqvlezqjdnynyzhmhgbdvwtikeqeqzmxrqseklswsexstf";
	string tiodw = "mnmgopnfebrijajyfdmgqorspemqweyyynijxnmm";
	bool jholiptzeiv = false;
	if (5666 != 5666) {
		int vgc;
		for (vgc = 29; vgc > 0; vgc--) {
			continue;
		}
	}

}

int jcscpdf::gcwnvrachdifsjp() {
	bool tioviqbwyebx = true;
	int smzifozmfiqahl = 1342;
	if (true != true) {
		int tzcjmghh;
		for (tzcjmghh = 70; tzcjmghh > 0; tzcjmghh--) {
			continue;
		}
	}
	if (true != true) {
		int vabzl;
		for (vabzl = 8; vabzl > 0; vabzl--) {
			continue;
		}
	}
	return 32203;
}

bool jcscpdf::nuysmvncfihy(bool ifvgntdklfxrfh, string gyefpdptzsfm, bool zgokdprveb, double bkjqhuzcwi, bool hxrlidsinukcmyo) {
	string hcmwde = "fberorqwnmqdbcupnfmgfournmbmmtuwtlvrllbcwoxzhdnxpkcctrakmcyquqroikjkzixsugqluqu";
	string ndchu = "ivijkxktyll";
	string fnvul = "nm";
	int ddlmovhhzcesdmc = 686;
	string tqdqhww = "bhwtkmdrnkcgsrevtwzpqumbsbcmvscldzmkhcrtnafmmuggkkhjairaqyhlyjjwwehcndkiqweqkuiwa";
	string eemwqkgmeu = "grxrxiajbndexhggapvdzcqppglacvtudetklyyxjkdrjnntk";
	bool jxvzc = false;
	bool bwaexnwd = true;
	double pxnktclz = 34971;
	return false;
}

string jcscpdf::pylcvlnfskdwxwxdv(double ufgvf, int dkebmdqjp, bool xmyxdybhtaik, bool kqtpwm, int ajzjdpozrnoijd) {
	bool izjmdtwlugbkj = true;
	double fdrks = 49705;
	string ohoxop = "kyzqxqjtvfjxxisnmmnoucsdszltopfeacgbwzhyyqifjwhcoebiozgqvtqwrumtxlfity";
	bool hrwcpygth = true;
	bool xsndsaukdnnltei = false;
	double beluelyeousqm = 52299;
	if (string("kyzqxqjtvfjxxisnmmnoucsdszltopfeacgbwzhyyqifjwhcoebiozgqvtqwrumtxlfity") == string("kyzqxqjtvfjxxisnmmnoucsdszltopfeacgbwzhyyqifjwhcoebiozgqvtqwrumtxlfity")) {
		int jhfxudvit;
		for (jhfxudvit = 96; jhfxudvit > 0; jhfxudvit--) {
			continue;
		}
	}
	if (49705 != 49705) {
		int faghts;
		for (faghts = 64; faghts > 0; faghts--) {
			continue;
		}
	}
	if (49705 == 49705) {
		int mub;
		for (mub = 12; mub > 0; mub--) {
			continue;
		}
	}
	if (false != false) {
		int npslziher;
		for (npslziher = 13; npslziher > 0; npslziher--) {
			continue;
		}
	}
	return string("smuhewvx");
}

string jcscpdf::xuoawhldowtmcpkfkphzwlm(bool aglofhwlxokt, double swhti) {
	string pxzvk = "ypcgndlforwceoueijspa";
	return string("tmozeizoh");
}

double jcscpdf::fhbnojrdbsp(int zuubggifhop, string gtofwf) {
	int gdinsufystkyum = 2724;
	double hhijoxdgcdw = 20876;
	double cjupgevxu = 34850;
	bool iixebr = true;
	string oydvmenjhaewto = "hkixyheqftoiumczohdekaevhlzjhhdafpexaksaponnsakxopdelcwmpydknopfi";
	bool mwfcliywl = true;
	double goadhprafoiu = 63577;
	bool iqkejwaynaketft = false;
	string wiwbfi = "bdsfrudrrzgdzpomsyhidyqyufolsxuduwlunowqeihcbihyaixtegxymewnxjnaqfxccmhanhyzkxzwdfqncj";
	if (false != false) {
		int rbkuxepfj;
		for (rbkuxepfj = 47; rbkuxepfj > 0; rbkuxepfj--) {
			continue;
		}
	}
	return 47869;
}

string jcscpdf::isiftsxgjkxz(bool dpgmjecnmtmnulb, bool vafawjcvnqax, bool rzwhrjtsfsdfek, int cxqauoiogjli, double rhhclbn, string cvtfh) {
	bool sivwpnlhjwcqllf = false;
	bool omzix = false;
	int htxgfsrjlbnucq = 3536;
	double hxoguaxbvdocvpr = 14292;
	string xjkcbmjspfgc = "aabpirblkzzqjtebmzpf";
	string xavdizh = "xvvlusqrocjxsilbunklxcifgazmwpjgfvlneenhsorfuovwqppqhqcdhspewplpsauaa";
	double khruemcvxtnngde = 34097;
	string voxgsiih = "ghwxvwdhcdufkklruhsnuifcoxbukxmtnstgknwhtucnemrtvrgpfasulzljyfnztaijlugsenetfydwbrenls";
	double elsykdtza = 44928;
	string ldcaomlpmq = "bkgbngmgahpymccamchflozeqjpfnwbxpnwrbmeoqmbdkpudfmqbzyoqumonhumljnnnkwqmqptnesmhisystgfzmqvvmodfz";
	if (string("bkgbngmgahpymccamchflozeqjpfnwbxpnwrbmeoqmbdkpudfmqbzyoqumonhumljnnnkwqmqptnesmhisystgfzmqvvmodfz") == string("bkgbngmgahpymccamchflozeqjpfnwbxpnwrbmeoqmbdkpudfmqbzyoqumonhumljnnnkwqmqptnesmhisystgfzmqvvmodfz")) {
		int mwxb;
		for (mwxb = 36; mwxb > 0; mwxb--) {
			continue;
		}
	}
	if (string("ghwxvwdhcdufkklruhsnuifcoxbukxmtnstgknwhtucnemrtvrgpfasulzljyfnztaijlugsenetfydwbrenls") != string("ghwxvwdhcdufkklruhsnuifcoxbukxmtnstgknwhtucnemrtvrgpfasulzljyfnztaijlugsenetfydwbrenls")) {
		int diwawpm;
		for (diwawpm = 75; diwawpm > 0; diwawpm--) {
			continue;
		}
	}
	if (false != false) {
		int kvyubxtddv;
		for (kvyubxtddv = 20; kvyubxtddv > 0; kvyubxtddv--) {
			continue;
		}
	}
	if (string("ghwxvwdhcdufkklruhsnuifcoxbukxmtnstgknwhtucnemrtvrgpfasulzljyfnztaijlugsenetfydwbrenls") == string("ghwxvwdhcdufkklruhsnuifcoxbukxmtnstgknwhtucnemrtvrgpfasulzljyfnztaijlugsenetfydwbrenls")) {
		int bcdvnjbb;
		for (bcdvnjbb = 95; bcdvnjbb > 0; bcdvnjbb--) {
			continue;
		}
	}
	if (34097 == 34097) {
		int omejsq;
		for (omejsq = 14; omejsq > 0; omejsq--) {
			continue;
		}
	}
	return string("lrlh");
}

string jcscpdf::kkxhziowvreo(int kznvmhdfejkkhdl, string chdngcejdi, string zaqgsiywdywd, bool wczgaugawqihoqm, bool owujnqdetxjaqsa, int vtslhy) {
	double ssgklm = 10123;
	bool nhuhmqbjmw = false;
	string fvswif = "shnlrrjsmjkgpoxswojjurseqttsmpgbr";
	double bsqtieyiktrzv = 17747;
	double jhgyg = 75918;
	int zntbfxrrvriyr = 6819;
	int vckhfoswx = 2437;
	string pfwisuvlb = "mbpdplnpvmyjflzhohawxdpxodapigziweeoodejxncqudjwygredtdmnvcftl";
	string mbboyxijhqj = "hpinuztqzwzocstik";
	if (string("shnlrrjsmjkgpoxswojjurseqttsmpgbr") != string("shnlrrjsmjkgpoxswojjurseqttsmpgbr")) {
		int fwfibhddi;
		for (fwfibhddi = 43; fwfibhddi > 0; fwfibhddi--) {
			continue;
		}
	}
	if (false == false) {
		int rfsfjbn;
		for (rfsfjbn = 78; rfsfjbn > 0; rfsfjbn--) {
			continue;
		}
	}
	if (10123 != 10123) {
		int arpvbrblhz;
		for (arpvbrblhz = 98; arpvbrblhz > 0; arpvbrblhz--) {
			continue;
		}
	}
	if (string("hpinuztqzwzocstik") != string("hpinuztqzwzocstik")) {
		int awickfb;
		for (awickfb = 70; awickfb > 0; awickfb--) {
			continue;
		}
	}
	if (string("hpinuztqzwzocstik") != string("hpinuztqzwzocstik")) {
		int zfkknktkx;
		for (zfkknktkx = 80; zfkknktkx > 0; zfkknktkx--) {
			continue;
		}
	}
	return string("iqftovvg");
}

void jcscpdf::blgehqzeznwcpuyfndrubavs(bool ofzhutfd, double yddzfvnwkdramf, string hoqbjxrlztsntoj) {
	string tbqqqcn = "fmnjgegaolscwobnszteiru";
	if (string("fmnjgegaolscwobnszteiru") != string("fmnjgegaolscwobnszteiru")) {
		int qqcbnt;
		for (qqcbnt = 5; qqcbnt > 0; qqcbnt--) {
			continue;
		}
	}
	if (string("fmnjgegaolscwobnszteiru") != string("fmnjgegaolscwobnszteiru")) {
		int hzrxwoj;
		for (hzrxwoj = 37; hzrxwoj > 0; hzrxwoj--) {
			continue;
		}
	}
	if (string("fmnjgegaolscwobnszteiru") != string("fmnjgegaolscwobnszteiru")) {
		int xsszianaie;
		for (xsszianaie = 11; xsszianaie > 0; xsszianaie--) {
			continue;
		}
	}
	if (string("fmnjgegaolscwobnszteiru") == string("fmnjgegaolscwobnszteiru")) {
		int ufgpewhc;
		for (ufgpewhc = 47; ufgpewhc > 0; ufgpewhc--) {
			continue;
		}
	}
	if (string("fmnjgegaolscwobnszteiru") == string("fmnjgegaolscwobnszteiru")) {
		int szgosb;
		for (szgosb = 93; szgosb > 0; szgosb--) {
			continue;
		}
	}

}

bool jcscpdf::afzhonevrcgoadfp(bool yjdvw, bool rasdyuhusfpmaja, int hsibbwgoomf, int rfxszshvrsvevl, int gkaojasptbwtd, int sdkvc, double ptzxjcakunzesb, string yxpuosaarcrv, bool gdmkuktyklkc) {
	string qinain = "qvfrhujqrpofaymrnkhbmgloeqyzmwsfnpkwgo";
	int aokjzlpwct = 1765;
	bool fhtar = true;
	int azugm = 7755;
	bool yeyfdadhamcts = true;
	string hgwctpca = "ukkvhyagojjkvyiklzskzaasilvvzrvqmt";
	bool kkustmrruwht = true;
	string kitpsi = "njlxgxfbiyzyuogulodlpqvspykjywhbjtbmjcrsbjjr";
	if (1765 != 1765) {
		int lcwttvg;
		for (lcwttvg = 40; lcwttvg > 0; lcwttvg--) {
			continue;
		}
	}
	if (string("njlxgxfbiyzyuogulodlpqvspykjywhbjtbmjcrsbjjr") == string("njlxgxfbiyzyuogulodlpqvspykjywhbjtbmjcrsbjjr")) {
		int yjlvco;
		for (yjlvco = 61; yjlvco > 0; yjlvco--) {
			continue;
		}
	}
	return true;
}

jcscpdf::jcscpdf() {
	this->fhbnojrdbsp(1709, string("noeripulmmmzdnpkhkszilrimrfoncodysndknhvzfpjcchhrtfwmojmdginhmljrwkwpklwurxfzmetunvbhpmvwkeyts"));
	this->isiftsxgjkxz(true, false, false, 5160, 6387, string("repdafggrqtleaihnvgtpbtkcawfpvafaztjqopthxbpgmdkvnibofaiqfrqklmwxjikkulcqoqjxlhmpi"));
	this->kkxhziowvreo(1641, string("zafxbcdiumaaeacethrrmnbhrbvwlcvstywinhxtnxporszgudxsann"), string("tbyeuroyshtzacstxsksukitbxrgrtyybtfvcqpbnhokcpbpjrfiienxrydqmsxnwsnjldmpfx"), false, true, 3576);
	this->blgehqzeznwcpuyfndrubavs(false, 6740, string("hassbvnvmcvjfhkrjvsdaxrqmrgcj"));
	this->afzhonevrcgoadfp(false, false, 7040, 1058, 8247, 329, 21726, string("ktewobkvukwjquxxygnuyyttfjreomgbwohi"), true);
	this->pylcvlnfskdwxwxdv(67732, 3167, false, true, 2836);
	this->xuoawhldowtmcpkfkphzwlm(true, 33153);
	this->jfpfznpthrlx(444);
	this->yrfqfmbucsjatlqtsvdesay(27697, false);
	this->xfpjzgyzwr(13709, 48871, false, 96, 1915, string("xsswwcothbolglnyfrezwxdjcrkwgwsqaurwlhfprtumyecpywlkfupmgaonpgc"), 5468, 91493, 11396, 1456);
	this->deljzgdbjmayriitsv(true, true, 6879, true);
	this->rvphxgefkuldttzcujbfpbn(true, 321, 41061, 3119, true, 10456, 2539);
	this->wollpmdmohridfnancj(3558, true, 75933, 2218, 58712, 6057, false, string("eshwcfpoijgoulgqsczfswnwsanlivvjsguavlsdglk"), 7147);
	this->gcwnvrachdifsjp();
	this->nuysmvncfihy(true, string("jlgjffgjoqajvamesqbsmzekpgmtgeqfhgtmtpmdwkkg"), true, 6776, false);
}




#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class wschurz {
public:
	bool clplz;
	double gzvsdarnju;
	wschurz();
	double rpgkhywmwyaphe(double urbnvcytangdhft, string tiitegjyizwz, bool bofujkybssbdhgt, string bcnawjkhkvs, string tajxxxsghuhpbg, double qqyxukwrmgbfehh, double tcarcf, double yvbykntbu);
	int qubhoaeompcqz(bool vdurdwdpcr, int zaipeuegmfrxixn);
	string erfpmyupvyawufzznnewsy(double ahyinecmrdgfrjc, int plimkrbbgdewzuq, string rqbfk, int mmkedet, bool ksyzhkewosniozv, int vjritthqmuvlx);
	double wujqcbwumfjr(bool ujyjsrjethymoxo, double fsytmvhsfrrnpow, string bvbqen, int azrxec, double kfpaqmctcfujg, int dorpwcc, double yxttrajqnhsm, string pnasyaugum, string eyekagrebrccct, string cepxiwuvtdxihkx);
	double tpgujrgziwrzk(int skjyvrtg, double umxbxczeezdk, string ljspnxsvvua, string wpoitqyilzqtkgp, int bxwvncvqspx);
	double qhyojjxgyjetnupcsphuhm(bool lylpqcprd, bool pcckygnuimhol, bool iljmrnpuzlr, string ormizekqhm, string xcegrhuzuec, double skbwtv, string sjujiqttu, string hgvbjmvrlbby);
	string xwscwgkpzsqpxpoxcagizhpf(double pxqimenfj);

protected:
	int tdtdepgohb;
	int zorfepa;
	string ytjfzfas;

	string dhexrwdncpesgfajpjcsq(double cxmro);

private:
	bool vuyhfqvdbzspfqs;
	int langbupueldx;
	bool kaolxs;
	int uglbvrslo;

	string dvdfdeybqrjxalmsdrnzpal(string bbsdhiwp, bool rpliyoptckrgvk, double ivcyrnm, int ycdrwpt, double cmpfw, bool cctbvonkeqqt);
	void tzwwbuibuipjnk(string kjshdvjivapt);

};


string wschurz::dvdfdeybqrjxalmsdrnzpal(string bbsdhiwp, bool rpliyoptckrgvk, double ivcyrnm, int ycdrwpt, double cmpfw, bool cctbvonkeqqt) {
	bool ouhelo = true;
	bool xgqubldm = false;
	string misxcelkvzcgjs = "loaemvvmfgyiojkzjhuknpuukibwcguhwrjshjxaorktybuqvgfetskqcexmkrmwdaqptchzdqvgqayexbgimzrfliysqwafxdgz";
	if (string("loaemvvmfgyiojkzjhuknpuukibwcguhwrjshjxaorktybuqvgfetskqcexmkrmwdaqptchzdqvgqayexbgimzrfliysqwafxdgz") != string("loaemvvmfgyiojkzjhuknpuukibwcguhwrjshjxaorktybuqvgfetskqcexmkrmwdaqptchzdqvgqayexbgimzrfliysqwafxdgz")) {
		int riv;
		for (riv = 12; riv > 0; riv--) {
			continue;
		}
	}
	if (string("loaemvvmfgyiojkzjhuknpuukibwcguhwrjshjxaorktybuqvgfetskqcexmkrmwdaqptchzdqvgqayexbgimzrfliysqwafxdgz") == string("loaemvvmfgyiojkzjhuknpuukibwcguhwrjshjxaorktybuqvgfetskqcexmkrmwdaqptchzdqvgqayexbgimzrfliysqwafxdgz")) {
		int arhe;
		for (arhe = 49; arhe > 0; arhe--) {
			continue;
		}
	}
	return string("m");
}

void wschurz::tzwwbuibuipjnk(string kjshdvjivapt) {
	bool tpwvq = true;
	int febgtxuupbt = 2950;
	bool orffehcmy = true;
	string tihiz = "fdnxdltvefdabjzbasrvbegjcae";
	double jufafvlj = 15739;
	int pzudzmjibgz = 2610;
	string zduucirlnroplcx = "koixixccpeqhqsghwpzkzzndkbncblc";
	string xebqmeadukhlcc = "xaeecdrwryfhhpeaukjwcskwhpiinhopgpehnkuits";
	int bxcueoergftg = 173;
	if (true == true) {
		int efyycyoj;
		for (efyycyoj = 19; efyycyoj > 0; efyycyoj--) {
			continue;
		}
	}
	if (2610 != 2610) {
		int vaybonj;
		for (vaybonj = 58; vaybonj > 0; vaybonj--) {
			continue;
		}
	}
	if (173 != 173) {
		int ub;
		for (ub = 5; ub > 0; ub--) {
			continue;
		}
	}

}

string wschurz::dhexrwdncpesgfajpjcsq(double cxmro) {
	string weiqeiycjmbblh = "pmqdffgarfwrrgwfabxzglgjjpiprgwlkgxsngkxmvwwjxgdmjvyitof";
	bool qwthpxwylaw = false;
	bool inolkxegqtglhz = true;
	string yjrelmfhzrgjx = "uoinsxhyvmkzxukeichrpkqayrykzartvxqrphayujhpfwssfqiygzcfocaoypzelvklkhecitzrndbxwoocqyzwwgyqymwzuh";
	bool gzpzfcryxv = false;
	int lykvbpzkcvaexbf = 6037;
	bool isaoedg = true;
	string evycrjyg = "geavhgywsczmczzeqfdekkabwxf";
	int bcrgqjlmq = 1968;
	return string("xmbhaoixorltmfsrr");
}

double wschurz::rpgkhywmwyaphe(double urbnvcytangdhft, string tiitegjyizwz, bool bofujkybssbdhgt, string bcnawjkhkvs, string tajxxxsghuhpbg, double qqyxukwrmgbfehh, double tcarcf, double yvbykntbu) {
	bool btehcyccqhoy = false;
	double xswag = 16827;
	double wkovvzj = 16741;
	double ryulisbj = 6475;
	bool kcaemhdmhr = false;
	double ggoqxorgkfk = 2015;
	double xjhki = 15184;
	double wcyauqkge = 3787;
	bool lforzdznanhapzf = false;
	int ndflj = 4317;
	return 47992;
}

int wschurz::qubhoaeompcqz(bool vdurdwdpcr, int zaipeuegmfrxixn) {
	string qaudh = "ttuejaruytxfmmgovpwmhqiyvxixqsfgqskswbvqnahqjhzqgjhlticwndcrkepakcupqjzgwmgy";
	string dwxno = "ujcgodwrlxacaxmuzzmxasyra";
	bool druexrqxmu = true;
	double uyizntikeqdla = 27477;
	string owigsxyqckq = "nbfldwxrtpnliwpelqryhdyihpzngl";
	double hxochwif = 4075;
	string tuaivpywlkjqm = "ypfszwthklacphtrlyjgvvyzumozqmcmhnjlvkrojwvksuszxkmnxmgtumdelucdy";
	bool gnvouubuxtreqg = true;
	string cjxksgijo = "nsywstiroeqztsvhrwtyrwtvdekaczevzuurkpnwopwuacxjxwgdoxihotitpmekbgxzvqdyejfcddzrbssavivfanancuelys";
	if (27477 != 27477) {
		int gwhyayhgjf;
		for (gwhyayhgjf = 88; gwhyayhgjf > 0; gwhyayhgjf--) {
			continue;
		}
	}
	if (string("ttuejaruytxfmmgovpwmhqiyvxixqsfgqskswbvqnahqjhzqgjhlticwndcrkepakcupqjzgwmgy") == string("ttuejaruytxfmmgovpwmhqiyvxixqsfgqskswbvqnahqjhzqgjhlticwndcrkepakcupqjzgwmgy")) {
		int pr;
		for (pr = 5; pr > 0; pr--) {
			continue;
		}
	}
	if (string("nsywstiroeqztsvhrwtyrwtvdekaczevzuurkpnwopwuacxjxwgdoxihotitpmekbgxzvqdyejfcddzrbssavivfanancuelys") != string("nsywstiroeqztsvhrwtyrwtvdekaczevzuurkpnwopwuacxjxwgdoxihotitpmekbgxzvqdyejfcddzrbssavivfanancuelys")) {
		int ikfzrp;
		for (ikfzrp = 71; ikfzrp > 0; ikfzrp--) {
			continue;
		}
	}
	if (string("ujcgodwrlxacaxmuzzmxasyra") != string("ujcgodwrlxacaxmuzzmxasyra")) {
		int ypbhcfw;
		for (ypbhcfw = 32; ypbhcfw > 0; ypbhcfw--) {
			continue;
		}
	}
	if (4075 != 4075) {
		int zwymuxrvkh;
		for (zwymuxrvkh = 58; zwymuxrvkh > 0; zwymuxrvkh--) {
			continue;
		}
	}
	return 71403;
}

string wschurz::erfpmyupvyawufzznnewsy(double ahyinecmrdgfrjc, int plimkrbbgdewzuq, string rqbfk, int mmkedet, bool ksyzhkewosniozv, int vjritthqmuvlx) {
	bool ulgiuxv = true;
	string ytirzini = "rqzdvgsvceffonjtrfiukitopigcugmuqcaaket";
	string pzxsy = "hrsacimivfokqfhyjqwejyzwzeexnoeoaxqkxvfnhtupzjuhvhmrqba";
	bool mhmbgrgtf = false;
	string mwgba = "plcnaejunseolyjbsaxhsggfkjyreiquvamkbfuvehqohzgfxqozojszhiidponqykk";
	int sulie = 1184;
	string jtqmulougre = "wrximonbd";
	double fokozdkbqnqs = 7562;
	string kpaibkdnarrqxfr = "njyrzakeyuqegfbhoikqjfgselxslefwixvmpubbuegdaabqpviltatpfomqftalskzjgiu";
	if (1184 == 1184) {
		int wcqfacbnad;
		for (wcqfacbnad = 54; wcqfacbnad > 0; wcqfacbnad--) {
			continue;
		}
	}
	return string("vvucvkpjiagftocemjj");
}

double wschurz::wujqcbwumfjr(bool ujyjsrjethymoxo, double fsytmvhsfrrnpow, string bvbqen, int azrxec, double kfpaqmctcfujg, int dorpwcc, double yxttrajqnhsm, string pnasyaugum, string eyekagrebrccct, string cepxiwuvtdxihkx) {
	string diwvfjuzgg = "rdhxbmxux";
	int rqcshem = 4049;
	int puxemkx = 2364;
	return 57793;
}

double wschurz::tpgujrgziwrzk(int skjyvrtg, double umxbxczeezdk, string ljspnxsvvua, string wpoitqyilzqtkgp, int bxwvncvqspx) {
	double zkglbsy = 20925;
	double zzsdioci = 8255;
	bool utmeklujr = false;
	int kmurfdomy = 1449;
	int acbmxhjbgk = 2585;
	if (8255 == 8255) {
		int imy;
		for (imy = 48; imy > 0; imy--) {
			continue;
		}
	}
	if (2585 == 2585) {
		int kp;
		for (kp = 16; kp > 0; kp--) {
			continue;
		}
	}
	if (8255 == 8255) {
		int jqftd;
		for (jqftd = 32; jqftd > 0; jqftd--) {
			continue;
		}
	}
	if (20925 != 20925) {
		int ffdd;
		for (ffdd = 99; ffdd > 0; ffdd--) {
			continue;
		}
	}
	return 4082;
}

double wschurz::qhyojjxgyjetnupcsphuhm(bool lylpqcprd, bool pcckygnuimhol, bool iljmrnpuzlr, string ormizekqhm, string xcegrhuzuec, double skbwtv, string sjujiqttu, string hgvbjmvrlbby) {
	string eqbulvi = "vpxtjubtrbvgbwznnqlejdgpymmyejgrejcmainogtmzrhqjimrs";
	double jasesxajzw = 8582;
	if (8582 != 8582) {
		int zhwbcpqj;
		for (zhwbcpqj = 84; zhwbcpqj > 0; zhwbcpqj--) {
			continue;
		}
	}
	return 32496;
}

string wschurz::xwscwgkpzsqpxpoxcagizhpf(double pxqimenfj) {
	bool shlpwznjo = true;
	double gdmaiolfiidhys = 77288;
	string ftfsain = "zdhvembkuqlbliqeemyucmonpgtbmeeqmcisbhkpxvapsvsgknqslobviujtvgdfkufshduwgxewt";
	bool gijhn = false;
	string emxdyu = "nvnddzsnumldpvublgzrjhvcahrltkaevebgmdjplmaiyqcdnm";
	if (string("zdhvembkuqlbliqeemyucmonpgtbmeeqmcisbhkpxvapsvsgknqslobviujtvgdfkufshduwgxewt") == string("zdhvembkuqlbliqeemyucmonpgtbmeeqmcisbhkpxvapsvsgknqslobviujtvgdfkufshduwgxewt")) {
		int vkbuyhwl;
		for (vkbuyhwl = 66; vkbuyhwl > 0; vkbuyhwl--) {
			continue;
		}
	}
	return string("rfuwj");
}

wschurz::wschurz() {
	this->rpgkhywmwyaphe(30888, string("yygbyyygupcpmvjkmugtgufmdlmyhgdrxme"), false, string("gzxbxcnlebdxzzzkyphggybfjumgqrrwueapyppnaxagmush"), string("bivlnyzggjufbuxbkfhsw"), 20347, 28547, 18189);
	this->qubhoaeompcqz(true, 549);
	this->erfpmyupvyawufzznnewsy(17533, 321, string("rodcnlnzthv"), 3060, false, 5232);
	this->wujqcbwumfjr(true, 46720, string("fvxpnbakjhqtymjqbzregwimvd"), 68, 11072, 3455, 24761, string("nvfxtswqwvkdknlxaxmjxtpghklinghvvqhikwpbmnttrsrpyuhx"), string("ycrqesam"), string("jwwdjjdloaxufrikulubgbqofqijrvrwrkzeuascpihynlkunvklfupsupnmmpdwzzkejzoeyglluephtxjpzdlnnpetmkxls"));
	this->tpgujrgziwrzk(8045, 24310, string("pawzgqvzdobtzrmvufpzernjnpbxaecbybclpjfanvkjdfbxzzlhgwsszdnbiockdbzuamlboydnatomuxkggowuroqbwdparg"), string("zabfnhxahrkpeepvlunjucyofsddavnylufbxd"), 1416);
	this->qhyojjxgyjetnupcsphuhm(true, false, true, string("ipfviwwaccigipuqfvmdrefjoriuimbzeeopkrrkoltdrfjdatckwtkujekshfqkehwtyslivzvgsbtgazwguswvx"), string("wvqnxdfzbmgqwbcrrwavervkmnczupvvq"), 13525, string("k"), string("tfvtsywinpajdjhkibcrdl"));
	this->xwscwgkpzsqpxpoxcagizhpf(76204);
	this->dhexrwdncpesgfajpjcsq(2540);
	this->dvdfdeybqrjxalmsdrnzpal(string("xjswksakyzvfsgdfjbmtpuwlsyhalegwojtnlbqyxwnbnvxrrazpk"), true, 3202, 396, 32850, false);
	this->tzwwbuibuipjnk(string("ovrcackcg"));
}




#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class uaecjee {
public:
	double cnuoaezbvulo;
	double lcgrsah;
	int yqvvlaiuh;
	uaecjee();
	bool iunprocxfzboael(string kbepytukgjxh, string aogxkfwsxzqxxjy, int zpjpa, double sblanqgtdxw);
	double ukyypvafqowrbqgtyufrkrni();
	void vanmlaeajimolt(bool ofvvskkmusjswz, double bchvnwvr, string bacuaowscza);

protected:
	double ornzfzxyl;

	double uhjygpaoxalzrollhicczli(string vekujiw, string cqvputeb, string yvhxfzhtzga, double yieooqsdnwzbuv, double jyypvxhc, string roazrnxllaon, bool uxylwlhau, string sihhsz);
	void hdoqujfjrjwqpmptc(bool wssxal, bool ouhzatqdepb, bool kphoaavbjswgyh, double ewlbh, int iwwnoeukidwkotq, int ksjyoqu, double gmjxvlzjhiwxqk, int qzviibbbd, int pusvcyqon);
	int eczmqfsejjfmyeqlbetiye();
	bool qmiiwmyokatmwxswtribi(double zmialuja, int iknubmhwaaqo, double sklfsyn, double qhvjveke, string utcuvllo, bool txguornqounjo, bool hbdzitvhbqad, string qqchapyrrzfc);
	double yxedcslfzjpnkentcov(double uzijqjk, bool mlqpz, string qgyai, bool ncqlkvosmiuois, bool dhgzmqam, bool bsndxeqtum, double achpqbamhkznoc);

private:
	bool ajpvmwvl;
	string qhazhiy;

	void zldvnvwsqtuoguqxmunfmjodb(string dkpwq, double hcppaddxmz, bool rzcpnxtufnpuy, string oakpevhcanxhv, string uejzcculsukhtkv);
	bool hkuybqcprcbhv(int dhabgcwapuxhcnn, int hxazirhcf);
	string fjnlxmirchgwee(bool uldjgpj, double bmcuzbwvhigfqrx, int ikpmokskbzzyu, int toukep, int gfinirrw, bool ugpfnpbtco, bool pvjjzxyd, string peicnl, int fggzo);
	string dzuoiveocburcgetabjgjqn(int bzbokshvrmstou, bool hnpmfronbbyc, string agvrv, string plxnvveufraqbl, bool jvxtfojmhqocfy, int dhztsdqgk, double fszxvvhcljxypbg, int fahrclzjomvfri);
	int twkjmcxxurovxojya(double kkvfolesjsrbne, string kzeuqvhfrtlt, int hqfctezdzmrnor);
	void xjgywttimlqdhsypxccvkfd(int okswr, double tdclvvn, string swycfg, int srqeuilspuiovy, int atpjz);
	string cbtvucmvqd(double odsed);
	int kwppwonxeoebzydrvyh(int egcdlwiijwuowc, bool knpllwfrshwthk, double aiscad, bool mdugcjcqsbp, double eomxuel, string dkjxagc, bool xbveucpg, double nmssmwm);
	string plgtctvrjyrbbr();
	string tubfwfvsoay(string yabdeydamky, int mhsxwnaxmjt);

};


void uaecjee::zldvnvwsqtuoguqxmunfmjodb(string dkpwq, double hcppaddxmz, bool rzcpnxtufnpuy, string oakpevhcanxhv, string uejzcculsukhtkv) {
	double riileppvihupxk = 15241;
	bool vmofoy = false;
	int mhhsbulpwga = 1007;
	int usrxkdrp = 4242;
	bool zjzchlnpv = true;
	if (15241 != 15241) {
		int iwvtzu;
		for (iwvtzu = 17; iwvtzu > 0; iwvtzu--) {
			continue;
		}
	}

}

bool uaecjee::hkuybqcprcbhv(int dhabgcwapuxhcnn, int hxazirhcf) {
	int rhhtmuavnydui = 2398;
	double vqemitf = 67140;
	if (67140 == 67140) {
		int wj;
		for (wj = 55; wj > 0; wj--) {
			continue;
		}
	}
	if (2398 != 2398) {
		int irj;
		for (irj = 28; irj > 0; irj--) {
			continue;
		}
	}
	if (67140 != 67140) {
		int sxmvypvw;
		for (sxmvypvw = 81; sxmvypvw > 0; sxmvypvw--) {
			continue;
		}
	}
	if (67140 != 67140) {
		int tbxhfyfwsl;
		for (tbxhfyfwsl = 26; tbxhfyfwsl > 0; tbxhfyfwsl--) {
			continue;
		}
	}
	return true;
}

string uaecjee::fjnlxmirchgwee(bool uldjgpj, double bmcuzbwvhigfqrx, int ikpmokskbzzyu, int toukep, int gfinirrw, bool ugpfnpbtco, bool pvjjzxyd, string peicnl, int fggzo) {
	double snsbah = 6647;
	string ieijun = "tu";
	string xvvriidhrvlm = "ohmjvcbomezecghdmmixgawdagnwgqothpwkjqiznwitjxlmowmzryntcjpxxdussmfztjjnsrhqmyxr";
	if (string("ohmjvcbomezecghdmmixgawdagnwgqothpwkjqiznwitjxlmowmzryntcjpxxdussmfztjjnsrhqmyxr") == string("ohmjvcbomezecghdmmixgawdagnwgqothpwkjqiznwitjxlmowmzryntcjpxxdussmfztjjnsrhqmyxr")) {
		int hb;
		for (hb = 80; hb > 0; hb--) {
			continue;
		}
	}
	return string("msu");
}

string uaecjee::dzuoiveocburcgetabjgjqn(int bzbokshvrmstou, bool hnpmfronbbyc, string agvrv, string plxnvveufraqbl, bool jvxtfojmhqocfy, int dhztsdqgk, double fszxvvhcljxypbg, int fahrclzjomvfri) {
	int tlkjkqwsihrtmdb = 3453;
	int gdpiq = 1573;
	bool yhqdkvspork = true;
	double lrtwkf = 66117;
	string eisbylkhedpntxh = "bxcmbvhytdnbmbweifdrquwtxsob";
	bool tnycsntrevtjps = true;
	int ycmdreojyrprjd = 828;
	if (true == true) {
		int krfpwk;
		for (krfpwk = 93; krfpwk > 0; krfpwk--) {
			continue;
		}
	}
	if (true != true) {
		int ozkhu;
		for (ozkhu = 70; ozkhu > 0; ozkhu--) {
			continue;
		}
	}
	return string("avjqve");
}

int uaecjee::twkjmcxxurovxojya(double kkvfolesjsrbne, string kzeuqvhfrtlt, int hqfctezdzmrnor) {
	bool oahafxbjwzcpeqp = false;
	string qpbihdo = "dsahpldnzkuvugenqcwxggkztruyldiwyshlaqbxwdfofacjnoommqofxixrchlvechzqsekwrdtefuzjgamgmacidcytpki";
	string yzkrtkmloamo = "uwrbycfenvxiziuznhgzlkwzsvwajtikquoevsdnuhfnvewkjahrzvhqiaxqmevtikl";
	string tsdutdzyb = "zjxynklmtobrwgt";
	bool zzlarbjqcs = true;
	int ikltuzecwd = 1911;
	bool ioovttzjh = false;
	int vauft = 855;
	int yixkbtl = 562;
	if (false != false) {
		int zojgbuntx;
		for (zojgbuntx = 25; zojgbuntx > 0; zojgbuntx--) {
			continue;
		}
	}
	if (string("zjxynklmtobrwgt") != string("zjxynklmtobrwgt")) {
		int osg;
		for (osg = 45; osg > 0; osg--) {
			continue;
		}
	}
	if (true == true) {
		int ddtovjr;
		for (ddtovjr = 91; ddtovjr > 0; ddtovjr--) {
			continue;
		}
	}
	return 87418;
}

void uaecjee::xjgywttimlqdhsypxccvkfd(int okswr, double tdclvvn, string swycfg, int srqeuilspuiovy, int atpjz) {
	int uutqbrqyiggfu = 7675;
	double cwujsfcnhtywbz = 58785;
	if (58785 == 58785) {
		int xfwlgwgguu;
		for (xfwlgwgguu = 85; xfwlgwgguu > 0; xfwlgwgguu--) {
			continue;
		}
	}
	if (7675 != 7675) {
		int kijoe;
		for (kijoe = 28; kijoe > 0; kijoe--) {
			continue;
		}
	}

}

string uaecjee::cbtvucmvqd(double odsed) {
	bool bpwyvjheqyk = false;
	double xcezukutuml = 43855;
	string gnzhuchbgaoj = "moy";
	string hwdwenxsrarfo = "vpmbaxyvz";
	int jsoyqcfr = 615;
	string mrkeoxxcruzqarc = "wjifschvnpdubqoqzowagjadrkrmwxqpwvbxovxvihrqphubfsgolershwhiyulitzwfddpvrnkxpbhcfgychrb";
	double njbskutermrzm = 4777;
	string uhjaneqegz = "xrpkpxwxptdwadmsolzhuaumxmolrqnqbsiskzrgrzpnixbdnyrklhxowatvljwfjmwzuujkmnf";
	int znjjgz = 1651;
	if (43855 == 43855) {
		int zlpxbotkz;
		for (zlpxbotkz = 19; zlpxbotkz > 0; zlpxbotkz--) {
			continue;
		}
	}
	if (1651 == 1651) {
		int dkrikig;
		for (dkrikig = 48; dkrikig > 0; dkrikig--) {
			continue;
		}
	}
	if (string("vpmbaxyvz") == string("vpmbaxyvz")) {
		int hb;
		for (hb = 67; hb > 0; hb--) {
			continue;
		}
	}
	return string("ymbyxrysskndqjfy");
}

int uaecjee::kwppwonxeoebzydrvyh(int egcdlwiijwuowc, bool knpllwfrshwthk, double aiscad, bool mdugcjcqsbp, double eomxuel, string dkjxagc, bool xbveucpg, double nmssmwm) {
	double qiqqxlngtrfr = 53571;
	double qkwimmdjk = 21614;
	bool rdicmcz = true;
	int npfrpctdxc = 2274;
	string xmgqcjsiodd = "zqufecxyckhchfouqbwxtvbejjcepeouballgrxkxrbh";
	double xggierldyzdlzyk = 12410;
	double cerefrkspub = 7919;
	int vrmdmtgxqwr = 382;
	double aqcjwbnezfp = 1449;
	string eqhbad = "lskrpljbxhmtrrezy";
	if (string("lskrpljbxhmtrrezy") == string("lskrpljbxhmtrrezy")) {
		int dasdhm;
		for (dasdhm = 6; dasdhm > 0; dasdhm--) {
			continue;
		}
	}
	return 75217;
}

string uaecjee::plgtctvrjyrbbr() {
	string huwfvuddrj = "elkqdhlcbwnpxdwoubmlfixrtvxcawquevpxojrwjrabbnsbyverwalrnmg";
	if (string("elkqdhlcbwnpxdwoubmlfixrtvxcawquevpxojrwjrabbnsbyverwalrnmg") != string("elkqdhlcbwnpxdwoubmlfixrtvxcawquevpxojrwjrabbnsbyverwalrnmg")) {
		int vflzizpikz;
		for (vflzizpikz = 68; vflzizpikz > 0; vflzizpikz--) {
			continue;
		}
	}
	return string("lhfhcmpxxtjmkjoptnt");
}

string uaecjee::tubfwfvsoay(string yabdeydamky, int mhsxwnaxmjt) {
	return string("ajcfjild");
}

double uaecjee::uhjygpaoxalzrollhicczli(string vekujiw, string cqvputeb, string yvhxfzhtzga, double yieooqsdnwzbuv, double jyypvxhc, string roazrnxllaon, bool uxylwlhau, string sihhsz) {
	bool tngvyjg = true;
	bool hktiyjmrw = true;
	bool pvaxi = true;
	int iikdom = 2752;
	double nfzvgdcsgftshhc = 74560;
	string enursoxel = "hljucvemrxxlohbzqtaiwrwyenzoxrazwwahclovfhmidvlxfbnxgargmgahrutkboc";
	if (true != true) {
		int lztkcxey;
		for (lztkcxey = 98; lztkcxey > 0; lztkcxey--) {
			continue;
		}
	}
	return 76318;
}

void uaecjee::hdoqujfjrjwqpmptc(bool wssxal, bool ouhzatqdepb, bool kphoaavbjswgyh, double ewlbh, int iwwnoeukidwkotq, int ksjyoqu, double gmjxvlzjhiwxqk, int qzviibbbd, int pusvcyqon) {
	double isccbrooplm = 31315;
	int vpstlbjy = 1;
	int olykhibwwr = 8348;
	double pwoml = 9241;
	int vqlmixlpmyfv = 5363;
	bool kmvrpkluqyjtcg = true;
	bool yascgkanyvo = false;
	if (1 != 1) {
		int rwz;
		for (rwz = 93; rwz > 0; rwz--) {
			continue;
		}
	}
	if (8348 != 8348) {
		int xdfas;
		for (xdfas = 93; xdfas > 0; xdfas--) {
			continue;
		}
	}

}

int uaecjee::eczmqfsejjfmyeqlbetiye() {
	int atyrrobw = 3019;
	bool zaaaikdngszasbj = false;
	int sfpzverljdj = 219;
	double lmpvergevqwb = 51620;
	int qceuoacuhzrz = 3433;
	string yxqplhrykdl = "aqaowybnoztxqhaazddujxczoynvwrafeswsgz";
	bool nkhccuqsw = false;
	bool ainwc = false;
	string urhoxbj = "hvyxafzdocopveidjktnjtjujphkeskrrkkzwghyrndmjwtqsqqvyspvywboibkfgqvcheqrxsnoxbwwafszfzohioesk";
	string wmwpnrhz = "htgmfunukxxuhseikxnlvvtfvonugldxyrvfiatgyoojuequdatrxybceztfvdlgeqsdwukgybqs";
	if (string("hvyxafzdocopveidjktnjtjujphkeskrrkkzwghyrndmjwtqsqqvyspvywboibkfgqvcheqrxsnoxbwwafszfzohioesk") == string("hvyxafzdocopveidjktnjtjujphkeskrrkkzwghyrndmjwtqsqqvyspvywboibkfgqvcheqrxsnoxbwwafszfzohioesk")) {
		int pvedqhysk;
		for (pvedqhysk = 71; pvedqhysk > 0; pvedqhysk--) {
			continue;
		}
	}
	return 98710;
}

bool uaecjee::qmiiwmyokatmwxswtribi(double zmialuja, int iknubmhwaaqo, double sklfsyn, double qhvjveke, string utcuvllo, bool txguornqounjo, bool hbdzitvhbqad, string qqchapyrrzfc) {
	string gveikdxvjjhctbl = "zjlssgjzukfllysrsagyismapbltfoyblzgzbhomfqpldvn";
	int kyutz = 1680;
	int dqnpazodilwvmtq = 5516;
	bool ozatwyouc = false;
	int yndizi = 1619;
	double iphaukwed = 63;
	double lqahclqgmnfier = 15311;
	return true;
}

double uaecjee::yxedcslfzjpnkentcov(double uzijqjk, bool mlqpz, string qgyai, bool ncqlkvosmiuois, bool dhgzmqam, bool bsndxeqtum, double achpqbamhkznoc) {
	bool fjrreqeoutiwhki = true;
	if (true != true) {
		int iuxuwc;
		for (iuxuwc = 66; iuxuwc > 0; iuxuwc--) {
			continue;
		}
	}
	if (true != true) {
		int bbrgujh;
		for (bbrgujh = 16; bbrgujh > 0; bbrgujh--) {
			continue;
		}
	}
	if (true != true) {
		int dkc;
		for (dkc = 92; dkc > 0; dkc--) {
			continue;
		}
	}
	return 9087;
}

bool uaecjee::iunprocxfzboael(string kbepytukgjxh, string aogxkfwsxzqxxjy, int zpjpa, double sblanqgtdxw) {
	return false;
}

double uaecjee::ukyypvafqowrbqgtyufrkrni() {
	int sgxmrvtcyvkjw = 44;
	bool mqzsqei = false;
	string vpocjyavppvd = "tctcyvjztrgjmtzdzxdujuwifeaadyjdgqjdnaomenaflpl";
	double hcdllqelsvjtslp = 9387;
	int wquyg = 3768;
	int refqmhmgeidyxjm = 6745;
	int fxsonkicrc = 946;
	double hcouxvzir = 7729;
	bool kfaikzhicgha = false;
	double bvvrosbsecolpmh = 23020;
	if (false == false) {
		int rqengb;
		for (rqengb = 98; rqengb > 0; rqengb--) {
			continue;
		}
	}
	if (9387 == 9387) {
		int wxskqmur;
		for (wxskqmur = 66; wxskqmur > 0; wxskqmur--) {
			continue;
		}
	}
	if (false != false) {
		int zuxwpn;
		for (zuxwpn = 73; zuxwpn > 0; zuxwpn--) {
			continue;
		}
	}
	return 29729;
}

void uaecjee::vanmlaeajimolt(bool ofvvskkmusjswz, double bchvnwvr, string bacuaowscza) {
	bool trnxetcxhvjn = true;
	double lmtkyqdhrhrs = 8451;
	string wnrlgk = "drpkhcrbcarfpnidxxwfanjdersaotlkkimlh";
	if (string("drpkhcrbcarfpnidxxwfanjdersaotlkkimlh") == string("drpkhcrbcarfpnidxxwfanjdersaotlkkimlh")) {
		int nqx;
		for (nqx = 86; nqx > 0; nqx--) {
			continue;
		}
	}
	if (8451 == 8451) {
		int eljm;
		for (eljm = 98; eljm > 0; eljm--) {
			continue;
		}
	}

}

uaecjee::uaecjee() {
	this->iunprocxfzboael(string("tlqelzaofrarffnurfbzipwonzrbgchvodcelroiaiwimspmnvyvtowwirqpagjewtipntpupkolmihkzcjspxlsbr"), string("wqqcqkbbcmslzugpeitlvnwhxfdfodpkwgoozwuailynwiqddbackjrvkcytxfltartpythbjrsacwwped"), 1107, 377);
	this->ukyypvafqowrbqgtyufrkrni();
	this->vanmlaeajimolt(false, 1193, string("tpqgqrbbbqofdkphoch"));
	this->uhjygpaoxalzrollhicczli(string("ofttohyfunwzdlkbuvcaayemrmuegajfnel"), string("hgikaaahtmvmdjkpinyczjnhimjaykldupdscjfktuyzldgtmvcvmaofzglzkinomyhentguazmudqypvbxmb"), string("baniblcgeirzybdeeqbuoweynvfytplcywnjxzrkzmdhkwkzeudthlbruzbpftlkxsdmjzkptdhjnbotbk"), 12923, 36409, string("cfirheutzkgayzggonboobdgahrqdmmtbgpkkjtvgiifqczcawmmylmpvtzpxvtrfavhnbqqooxanirgtxjrobxnasjvbexx"), false, string("tuglzymnlybsaaxltxgtakuhjyqqhovijmpshqdvkfvclkcmeczhyfunnyjsdutdrkayhvvxuaggxrjil"));
	this->hdoqujfjrjwqpmptc(true, true, false, 9417, 3535, 913, 14244, 301, 2367);
	this->eczmqfsejjfmyeqlbetiye();
	this->qmiiwmyokatmwxswtribi(61525, 2083, 62205, 13045, string("gmuhwridvguohgxqcjyftaljzjzwrdxbclwjmxuuqpikaaujbzwvesfmnzodmflipneuxvvqhjyvjozwxyznllgkl"), false, false, string("jyaspqwhtxawfzireledfcudq"));
	this->yxedcslfzjpnkentcov(32956, true, string("lqksrgebkejpbqhvqididxqbgiyrwexishenlvffdkvtteolzrduxzwwompawxrqjqxxy"), true, false, true, 14413);
	this->zldvnvwsqtuoguqxmunfmjodb(string("qxqoimjkgosvqdwmokjjevwfanmupjdrrjoyqgfpsajihysdrhadfapbhybhtuhwhjvnkfeahyaqhpstsrftk"), 46720, true, string("fbvwgnqdpqwayxnpgtszenjtagjhxlskriboblsbryauykuowbvvglzvczowhwhztjdgvujhjfypvtlokktc"), string("apgzsmdukqzfzqactl"));
	this->hkuybqcprcbhv(2919, 1402);
	this->fjnlxmirchgwee(false, 5761, 3672, 6286, 1695, true, false, string("pkzrzhxdgeiiwxeftjqimzxoneqekqrlmqtscsyhtucxntbjfrwandxbdsbmaspayhykmmbwanjiccmbydyxwdprjzyunybnj"), 2920);
	this->dzuoiveocburcgetabjgjqn(2247, true, string("kccixvfrldtzkvvsyvsubxwamszbcxkgtgkfki"), string("yfdyzbqanhslwrvhyyvgmfjgaevxjwqxwe"), false, 3092, 3448, 1125);
	this->twkjmcxxurovxojya(32563, string("bnycdmeqeaotmeyghkispqsucetl"), 715);
	this->xjgywttimlqdhsypxccvkfd(3596, 3437, string("zqzboglvsxxxpprjdpejdehfe"), 4857, 4844);
	this->cbtvucmvqd(68190);
	this->kwppwonxeoebzydrvyh(5925, true, 19495, false, 2845, string("dhwtrnxgguicmsjeasceicxgzgaxkfdgxyjaqkn"), false, 18357);
	this->plgtctvrjyrbbr();
	this->tubfwfvsoay(string("zrhkulffnejjvufvdpiyrmucyhcqcnnfrawdzdcdgbgfpyqhwfgrjbtngwnqjeqiigdbrmrhiwyofdbmonooptaaug"), 3320);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class estuhzp {
public:
	int pfrlveqzdbuj;
	bool dxoqdfufuiie;
	string wignujogdvxb;
	double pizzmbdrtk;
	estuhzp();
	double gnernqutxorfrbwe(int vxbmzy, double vnnfzyllqu, string dsbgsfhkllllun, double alnwplcbuzyrlp, int tznja, int blpxgryswtqi);
	int fxdzpdnvgqyk(double astjybfnmzgdyxl, int fypwq, string jnpsrwavehcv);
	int ibofbtyiew(string lmawmbdf, string djdtr, bool jjfuthmizh, string tofajgcrs);
	string snzzsrfzfwtuljawgim(int eyaltbnsgwdezjn, int wwylultswzixjt, double cnirx, int ippugabxjrvjvff, double vjveklxbilvurau, double ihlgmueebfvpv, int jnljaxxsolzvwes, bool dgoaus, string mwechsswojdgi, bool lipswihfkqkb);
	string jtmnwuxgfjyaigvqxqezvux();
	double jyrwjgmlluvihujieazen(bool ampuwavfvaihgqn, double ecvktbtk, string zhmumilk, bool mrzqbepqtcc, string fupjuwy, bool xlbkfz, double xrvrmtbgfjxv);

protected:
	int zucyshwwryn;
	double jzzeqsbjykyatlk;

	void jtpxfkuagvv();
	string spiennqcivbrpeopdqujrtdco(bool pxnrkjtxargha, double xcinrtkiyt, double vsgbkntzqwr, double zmqpdxe);
	int gbsfnmkxqjpnkm(bool lrcsjr, bool vrxlqv, int etrwt);
	int hmrhaegwhyjrlc(string evpzfgfoxbyrz, string weahdxylrwsd, int bzyuaforgnr, bool tmbcglg, bool xllii, string bvtttkwsoneiy, double ndzlqprrvdchgu);
	bool boidbtascbyvijvvmkcmptcd(string apweiz, double snrkpfkherevzbs, string ninfkicvtylqqt, string wyubpbkikegjfje, double odwnnn);
	double dtclnhukqz(int ntnfap, string kyxuh, string vxmkltwjcpxu);
	double kzyxtcpdinisawbaljfavdxjp(string tiyxhnotwx, bool upzgz, bool ziluk);
	void enbpuebkpicdxbgdpka(bool gqkheuejz, bool osknvtv, string lselzdrjgn, double vqovesi, double iwsdzlo, string ylulkxawguxs, bool vskwltufk, double unawwvoonwukqqq);
	double lrpsqvdbebfnylmwxrpytulq(double jiimgnzmioczp, int sylavk, int axslppkmmxlno, string ovotiw);
	int vqfghzxscudpcyllrrskhmxb(string bqofjbbpzhx, bool ikkoxlxl, double ibgovktonzc, string cayfiypnt, int zknmbhu, double snmuiwdfmw, int fhtzqaipdpimd, bool asttosnmolzof);

private:
	double vabqyvnqi;

	string eobyguakgnlrzfij();
	bool vqijiwaaruulywaysihit(bool ygxmvcle, string rchsbia, string uggovhmnikes, int ygjwmintzodpcp, string rkfazwbw, double sibmjvtydsop, double nzhfzbrixgqtxp, int cckowak, string pkscfhbwkj);
	void rzzpejcmrmfw(bool zwftctr, double ltkhzutvzq, double vocbh, bool zhhbt);
	double lzkculbyfcnxwm(double kvejuxnqyio, double rmwqvlgaqerofqr, double fahgpfux);

};


string estuhzp::eobyguakgnlrzfij() {
	double thaqaaffx = 15038;
	bool ifcrkqbu = true;
	bool ycbghu = false;
	if (15038 != 15038) {
		int flfm;
		for (flfm = 74; flfm > 0; flfm--) {
			continue;
		}
	}
	if (true == true) {
		int te;
		for (te = 89; te > 0; te--) {
			continue;
		}
	}
	return string("");
}

bool estuhzp::vqijiwaaruulywaysihit(bool ygxmvcle, string rchsbia, string uggovhmnikes, int ygjwmintzodpcp, string rkfazwbw, double sibmjvtydsop, double nzhfzbrixgqtxp, int cckowak, string pkscfhbwkj) {
	double xvvojmakjphdxxs = 20679;
	bool bbmzi = false;
	if (false != false) {
		int gmudoytoj;
		for (gmudoytoj = 35; gmudoytoj > 0; gmudoytoj--) {
			continue;
		}
	}
	if (20679 == 20679) {
		int mjb;
		for (mjb = 35; mjb > 0; mjb--) {
			continue;
		}
	}
	if (false != false) {
		int fmgqbfy;
		for (fmgqbfy = 86; fmgqbfy > 0; fmgqbfy--) {
			continue;
		}
	}
	if (20679 != 20679) {
		int oodnfyfehg;
		for (oodnfyfehg = 23; oodnfyfehg > 0; oodnfyfehg--) {
			continue;
		}
	}
	if (false != false) {
		int fbg;
		for (fbg = 99; fbg > 0; fbg--) {
			continue;
		}
	}
	return false;
}

void estuhzp::rzzpejcmrmfw(bool zwftctr, double ltkhzutvzq, double vocbh, bool zhhbt) {
	int ckzewtzi = 7627;
	double xwzzaqqncuze = 43596;
	string jfxefmui = "cbaauuxqojliuqridkealoubieiwlfwdgvbipuelsapbcusobpwjcnsddlumeudtvl";
	if (7627 != 7627) {
		int jbh;
		for (jbh = 86; jbh > 0; jbh--) {
			continue;
		}
	}
	if (string("cbaauuxqojliuqridkealoubieiwlfwdgvbipuelsapbcusobpwjcnsddlumeudtvl") == string("cbaauuxqojliuqridkealoubieiwlfwdgvbipuelsapbcusobpwjcnsddlumeudtvl")) {
		int idinpjf;
		for (idinpjf = 6; idinpjf > 0; idinpjf--) {
			continue;
		}
	}
	if (43596 == 43596) {
		int getupmh;
		for (getupmh = 70; getupmh > 0; getupmh--) {
			continue;
		}
	}

}

double estuhzp::lzkculbyfcnxwm(double kvejuxnqyio, double rmwqvlgaqerofqr, double fahgpfux) {
	string chnkhlkbyru = "qzpwh";
	int zkoldbhrrd = 1662;
	int topbcxnwhyreh = 4028;
	int jedjfhtb = 1767;
	string rvvcnypugpv = "bqsqsiobrsaaknnlgdirvgbkmuuvuhetwcvwhhontfojwwliqqhnuiontkxxcksszfzluchstamgoqnth";
	string nimczcgnlibnjh = "amehcokygvmyowmqadbnwkmgqocapeixqmnycjsfujihfyhytbyzsquduboqwbuaxankopnxfhunfgyxhoyauvnzyfbfpauett";
	bool gqbxqwgvt = true;
	int wljpozbitvm = 1836;
	string hifqdwddhmm = "udhoicnljfevwdfn";
	int pmlqpppfozwrz = 475;
	if (true == true) {
		int rgreguggz;
		for (rgreguggz = 69; rgreguggz > 0; rgreguggz--) {
			continue;
		}
	}
	return 91740;
}

void estuhzp::jtpxfkuagvv() {

}

string estuhzp::spiennqcivbrpeopdqujrtdco(bool pxnrkjtxargha, double xcinrtkiyt, double vsgbkntzqwr, double zmqpdxe) {
	int jrxcglsnhtai = 57;
	double nxvdpyoxmejfmng = 4621;
	double qblttsmfuhfvsem = 3470;
	bool frrvuglf = true;
	int lktqgofmzxrxqtp = 4066;
	int nbcjczknjrq = 1697;
	double osaemxocf = 55372;
	int cfbtd = 4539;
	if (3470 != 3470) {
		int ba;
		for (ba = 20; ba > 0; ba--) {
			continue;
		}
	}
	return string("qmjkdszz");
}

int estuhzp::gbsfnmkxqjpnkm(bool lrcsjr, bool vrxlqv, int etrwt) {
	int eivqfdunhflcqs = 2531;
	if (2531 == 2531) {
		int pyhqou;
		for (pyhqou = 78; pyhqou > 0; pyhqou--) {
			continue;
		}
	}
	if (2531 != 2531) {
		int nenpas;
		for (nenpas = 74; nenpas > 0; nenpas--) {
			continue;
		}
	}
	if (2531 == 2531) {
		int djdtnnyjz;
		for (djdtnnyjz = 2; djdtnnyjz > 0; djdtnnyjz--) {
			continue;
		}
	}
	if (2531 != 2531) {
		int qttp;
		for (qttp = 33; qttp > 0; qttp--) {
			continue;
		}
	}
	if (2531 == 2531) {
		int snohvv;
		for (snohvv = 1; snohvv > 0; snohvv--) {
			continue;
		}
	}
	return 55784;
}

int estuhzp::hmrhaegwhyjrlc(string evpzfgfoxbyrz, string weahdxylrwsd, int bzyuaforgnr, bool tmbcglg, bool xllii, string bvtttkwsoneiy, double ndzlqprrvdchgu) {
	return 45413;
}

bool estuhzp::boidbtascbyvijvvmkcmptcd(string apweiz, double snrkpfkherevzbs, string ninfkicvtylqqt, string wyubpbkikegjfje, double odwnnn) {
	string ckmuzp = "nxqbhgynjoebnbyejuiudjlnlwibkdmxlqyoiiothusanjdxnmlhxribyksfpvybewpcydcyygjqkumz";
	double lvpzizohddvjuf = 18234;
	if (18234 != 18234) {
		int trob;
		for (trob = 50; trob > 0; trob--) {
			continue;
		}
	}
	if (string("nxqbhgynjoebnbyejuiudjlnlwibkdmxlqyoiiothusanjdxnmlhxribyksfpvybewpcydcyygjqkumz") == string("nxqbhgynjoebnbyejuiudjlnlwibkdmxlqyoiiothusanjdxnmlhxribyksfpvybewpcydcyygjqkumz")) {
		int aaeeayj;
		for (aaeeayj = 18; aaeeayj > 0; aaeeayj--) {
			continue;
		}
	}
	return false;
}

double estuhzp::dtclnhukqz(int ntnfap, string kyxuh, string vxmkltwjcpxu) {
	return 17336;
}

double estuhzp::kzyxtcpdinisawbaljfavdxjp(string tiyxhnotwx, bool upzgz, bool ziluk) {
	int gnachmmdcg = 2694;
	double unkjcxjcwiixzqq = 60001;
	if (2694 != 2694) {
		int sronw;
		for (sronw = 14; sronw > 0; sronw--) {
			continue;
		}
	}
	if (2694 != 2694) {
		int pqjisqxk;
		for (pqjisqxk = 20; pqjisqxk > 0; pqjisqxk--) {
			continue;
		}
	}
	return 10789;
}

void estuhzp::enbpuebkpicdxbgdpka(bool gqkheuejz, bool osknvtv, string lselzdrjgn, double vqovesi, double iwsdzlo, string ylulkxawguxs, bool vskwltufk, double unawwvoonwukqqq) {
	int qrxjksnezv = 233;
	string kybizyuuut = "mqrxntjpfbzvyqkeinosflegcmpazehewdshgiabezpwswjepeioitoilimbzjcxkjmwywnthxfgl";

}

double estuhzp::lrpsqvdbebfnylmwxrpytulq(double jiimgnzmioczp, int sylavk, int axslppkmmxlno, string ovotiw) {
	bool yfoxl = true;
	string mflgonwryf = "sumooowfnthvgxqvsyfzpyfkofor";
	bool xvwcere = false;
	int ydynbmncq = 1031;
	if (string("sumooowfnthvgxqvsyfzpyfkofor") == string("sumooowfnthvgxqvsyfzpyfkofor")) {
		int ouyw;
		for (ouyw = 24; ouyw > 0; ouyw--) {
			continue;
		}
	}
	if (1031 != 1031) {
		int hutr;
		for (hutr = 89; hutr > 0; hutr--) {
			continue;
		}
	}
	if (false != false) {
		int alfxsxlmy;
		for (alfxsxlmy = 92; alfxsxlmy > 0; alfxsxlmy--) {
			continue;
		}
	}
	if (string("sumooowfnthvgxqvsyfzpyfkofor") != string("sumooowfnthvgxqvsyfzpyfkofor")) {
		int hcozumgak;
		for (hcozumgak = 80; hcozumgak > 0; hcozumgak--) {
			continue;
		}
	}
	if (true != true) {
		int zmjkve;
		for (zmjkve = 98; zmjkve > 0; zmjkve--) {
			continue;
		}
	}
	return 67483;
}

int estuhzp::vqfghzxscudpcyllrrskhmxb(string bqofjbbpzhx, bool ikkoxlxl, double ibgovktonzc, string cayfiypnt, int zknmbhu, double snmuiwdfmw, int fhtzqaipdpimd, bool asttosnmolzof) {
	string uzbqfeyj = "dveffpmboqyizffwantxwagrsznjdguetycupvsizcxmskntnyygchluspbsuxihxeurlecebmltcfgjravaxnglqrpfetqahgpq";
	int bowltlyymjsb = 1808;
	bool vogwcwkmjzjshla = true;
	double rdtqukaoyf = 10438;
	string yrzwjkhfgwrqw = "ybojuiiadhtvqnggd";
	string pqezm = "yweapfzlqxxgyquehtcbsmfhznyauayvqtinwecpiulyhijwteojgltbqveqkhajvmbbvdppijthre";
	if (string("dveffpmboqyizffwantxwagrsznjdguetycupvsizcxmskntnyygchluspbsuxihxeurlecebmltcfgjravaxnglqrpfetqahgpq") != string("dveffpmboqyizffwantxwagrsznjdguetycupvsizcxmskntnyygchluspbsuxihxeurlecebmltcfgjravaxnglqrpfetqahgpq")) {
		int hmzr;
		for (hmzr = 79; hmzr > 0; hmzr--) {
			continue;
		}
	}
	if (string("ybojuiiadhtvqnggd") != string("ybojuiiadhtvqnggd")) {
		int eueqvrr;
		for (eueqvrr = 59; eueqvrr > 0; eueqvrr--) {
			continue;
		}
	}
	if (true == true) {
		int xm;
		for (xm = 83; xm > 0; xm--) {
			continue;
		}
	}
	if (string("dveffpmboqyizffwantxwagrsznjdguetycupvsizcxmskntnyygchluspbsuxihxeurlecebmltcfgjravaxnglqrpfetqahgpq") == string("dveffpmboqyizffwantxwagrsznjdguetycupvsizcxmskntnyygchluspbsuxihxeurlecebmltcfgjravaxnglqrpfetqahgpq")) {
		int timo;
		for (timo = 63; timo > 0; timo--) {
			continue;
		}
	}
	return 85290;
}

double estuhzp::gnernqutxorfrbwe(int vxbmzy, double vnnfzyllqu, string dsbgsfhkllllun, double alnwplcbuzyrlp, int tznja, int blpxgryswtqi) {
	string bojmewmpclhsp = "mvieicptxytggnpqzaspteqjljanveyqzrjwgdrordmbfdnzmeigggwmqytp";
	double svfuk = 50839;
	if (string("mvieicptxytggnpqzaspteqjljanveyqzrjwgdrordmbfdnzmeigggwmqytp") != string("mvieicptxytggnpqzaspteqjljanveyqzrjwgdrordmbfdnzmeigggwmqytp")) {
		int tl;
		for (tl = 10; tl > 0; tl--) {
			continue;
		}
	}
	if (50839 == 50839) {
		int wse;
		for (wse = 42; wse > 0; wse--) {
			continue;
		}
	}
	if (50839 == 50839) {
		int cqdwyzbr;
		for (cqdwyzbr = 3; cqdwyzbr > 0; cqdwyzbr--) {
			continue;
		}
	}
	if (50839 != 50839) {
		int vqwj;
		for (vqwj = 67; vqwj > 0; vqwj--) {
			continue;
		}
	}
	return 93168;
}

int estuhzp::fxdzpdnvgqyk(double astjybfnmzgdyxl, int fypwq, string jnpsrwavehcv) {
	bool oysioieiirlnufz = true;
	string fujuoj = "p";
	string xjmcjlmc = "kflgfgillgufrcdafhbjxpeqljroavyyefzuipctprzbqfgs";
	double ikaxqfzflod = 10497;
	string pinhjotcvmj = "rbarjfjskslfsvejevqgwajqbwlthbhjpbwycabeziiaubphzelxgjioixa";
	if (10497 != 10497) {
		int mcinwpsdsb;
		for (mcinwpsdsb = 38; mcinwpsdsb > 0; mcinwpsdsb--) {
			continue;
		}
	}
	if (string("p") != string("p")) {
		int ikbcdz;
		for (ikbcdz = 21; ikbcdz > 0; ikbcdz--) {
			continue;
		}
	}
	if (10497 == 10497) {
		int gkgekqudcx;
		for (gkgekqudcx = 49; gkgekqudcx > 0; gkgekqudcx--) {
			continue;
		}
	}
	if (string("p") == string("p")) {
		int emdg;
		for (emdg = 3; emdg > 0; emdg--) {
			continue;
		}
	}
	if (string("p") == string("p")) {
		int vt;
		for (vt = 74; vt > 0; vt--) {
			continue;
		}
	}
	return 51332;
}

int estuhzp::ibofbtyiew(string lmawmbdf, string djdtr, bool jjfuthmizh, string tofajgcrs) {
	int ldgcjchd = 4605;
	int zehgpdhiipmiv = 1615;
	if (1615 == 1615) {
		int jq;
		for (jq = 20; jq > 0; jq--) {
			continue;
		}
	}
	if (1615 == 1615) {
		int cidy;
		for (cidy = 32; cidy > 0; cidy--) {
			continue;
		}
	}
	return 66582;
}

string estuhzp::snzzsrfzfwtuljawgim(int eyaltbnsgwdezjn, int wwylultswzixjt, double cnirx, int ippugabxjrvjvff, double vjveklxbilvurau, double ihlgmueebfvpv, int jnljaxxsolzvwes, bool dgoaus, string mwechsswojdgi, bool lipswihfkqkb) {
	double wlmpevlmlsymejl = 13678;
	int mafhvtojix = 1957;
	int sbhogtm = 4248;
	if (4248 != 4248) {
		int bhc;
		for (bhc = 65; bhc > 0; bhc--) {
			continue;
		}
	}
	if (1957 == 1957) {
		int ds;
		for (ds = 61; ds > 0; ds--) {
			continue;
		}
	}
	if (13678 == 13678) {
		int lgd;
		for (lgd = 48; lgd > 0; lgd--) {
			continue;
		}
	}
	if (4248 != 4248) {
		int epbojzom;
		for (epbojzom = 64; epbojzom > 0; epbojzom--) {
			continue;
		}
	}
	if (1957 != 1957) {
		int qceveiuh;
		for (qceveiuh = 78; qceveiuh > 0; qceveiuh--) {
			continue;
		}
	}
	return string("njclgyugvjtldir");
}

string estuhzp::jtmnwuxgfjyaigvqxqezvux() {
	string kjdui = "ebpkdcwiywhkmtwjvrevzzvdlk";
	if (string("ebpkdcwiywhkmtwjvrevzzvdlk") == string("ebpkdcwiywhkmtwjvrevzzvdlk")) {
		int gaampkg;
		for (gaampkg = 16; gaampkg > 0; gaampkg--) {
			continue;
		}
	}
	if (string("ebpkdcwiywhkmtwjvrevzzvdlk") != string("ebpkdcwiywhkmtwjvrevzzvdlk")) {
		int evzdg;
		for (evzdg = 69; evzdg > 0; evzdg--) {
			continue;
		}
	}
	if (string("ebpkdcwiywhkmtwjvrevzzvdlk") == string("ebpkdcwiywhkmtwjvrevzzvdlk")) {
		int qtfvbh;
		for (qtfvbh = 3; qtfvbh > 0; qtfvbh--) {
			continue;
		}
	}
	if (string("ebpkdcwiywhkmtwjvrevzzvdlk") == string("ebpkdcwiywhkmtwjvrevzzvdlk")) {
		int pvtv;
		for (pvtv = 67; pvtv > 0; pvtv--) {
			continue;
		}
	}
	if (string("ebpkdcwiywhkmtwjvrevzzvdlk") != string("ebpkdcwiywhkmtwjvrevzzvdlk")) {
		int luey;
		for (luey = 14; luey > 0; luey--) {
			continue;
		}
	}
	return string("");
}

double estuhzp::jyrwjgmlluvihujieazen(bool ampuwavfvaihgqn, double ecvktbtk, string zhmumilk, bool mrzqbepqtcc, string fupjuwy, bool xlbkfz, double xrvrmtbgfjxv) {
	string hhzmqftdvf = "pbjygjtdjzafyaluvonbjichlvrdqgvpeeuqxhaanxcbduwhwllgezuujrjlyuvdjcnfvpcthnpcufuolvm";
	double nlhmkvinuiugunl = 21770;
	int khaucoy = 741;
	if (string("pbjygjtdjzafyaluvonbjichlvrdqgvpeeuqxhaanxcbduwhwllgezuujrjlyuvdjcnfvpcthnpcufuolvm") == string("pbjygjtdjzafyaluvonbjichlvrdqgvpeeuqxhaanxcbduwhwllgezuujrjlyuvdjcnfvpcthnpcufuolvm")) {
		int zzdeidt;
		for (zzdeidt = 24; zzdeidt > 0; zzdeidt--) {
			continue;
		}
	}
	return 21407;
}

estuhzp::estuhzp() {
	this->gnernqutxorfrbwe(5021, 19446, string("kquojsiwnusitdxzwejlylfcdtwwxqxbexdlkvuicjynlxvcmhsipxmuvkshpbsuowyisepqpalbqfiijqwwtdauo"), 50272, 3055, 7366);
	this->fxdzpdnvgqyk(15561, 1823, string("jjwclcltewcwngificonfwafbzscutwphdokceahhslqtvexrgmgrrxuypschhzlkazuoxzdmskxrikzvrzkwixqafmhbuyi"));
	this->ibofbtyiew(string("gnbwkcuww"), string("usfjappmepzlzvqieqasurcxqxlcsaxdrkiyymibdsduexiltsortlzxlbhjcveztamej"), false, string("evydmdoitwsuhdfyvdvpelhmbwjbnjupriilrpzpfequwwcpdygfnzrzlxtphbeyaunargno"));
	this->snzzsrfzfwtuljawgim(1617, 4865, 74132, 2950, 39007, 14942, 3744, false, string("zxxxnblhugryihfqfawgaplgfbehpdiksmhgquddixlsyakunwfgedfhehisdnwdkn"), false);
	this->jtmnwuxgfjyaigvqxqezvux();
	this->jyrwjgmlluvihujieazen(false, 3580, string("levulesdxkqtmajrzxgisbaallxonfsmuilythyrhyiijeuhndfyczrifrlycayzmipnwrjexaqjnwcbnyaaxviqspusftwnqqd"), false, string("lpeayrsnhfdywl"), false, 14225);
	this->jtpxfkuagvv();
	this->spiennqcivbrpeopdqujrtdco(true, 22354, 23197, 5719);
	this->gbsfnmkxqjpnkm(false, true, 1488);
	this->hmrhaegwhyjrlc(string("rvpqymmjaaaesrhfecuaaffldzetavkyfkpxcjfhutiejboaqumpndbpfrqhtnkfgpnyvyjbfrieyysbozlknjiihms"), string("kxttmawmkwrwtbwuhdgibkkislibvqjikbbcaqbmelzibatfsqwoyoyvtxgqhw"), 3215, true, true, string("hfvepvrnjnmzexszrmauhmqbhcupopsbytnphaik"), 7116);
	this->boidbtascbyvijvvmkcmptcd(string("giiuxgwzbgxjjatrjwdwvkkziqtaoqiflkaiizwgbalhfqxwsqjwizt"), 16873, string("twdlhikefnforqhmcxhrnzbhezrzdswliyvoqtkqhumwpqqhqzwiecmypwhwkochxbasvfrzdciclsiflskimdmayzhkxgvmiqii"), string("wzkrklxydyxklvhxrismuhxltemakaeyiqytxte"), 24731);
	this->dtclnhukqz(894, string("omwzosntijztytybtlhvcdq"), string("lvrtgaqdlrcajlmzzbgqrwfxdwszjuliyrapilpsxyxhijhzoqwuj"));
	this->kzyxtcpdinisawbaljfavdxjp(string("iheulcdahqdyfjwofrebvoumjqq"), true, true);
	this->enbpuebkpicdxbgdpka(false, false, string("flojccjkcnotvtskghmusqhspgxsqoanptwmyszpeyuitthzsvcgzuohshfkudrmjapfffsyooclyrdhedehbydyeuyfrqao"), 3227, 28733, string("cveflzqedapxabgucxblmqqmrchkdwjdextcoulavheqmveptupwlgfuql"), false, 911);
	this->lrpsqvdbebfnylmwxrpytulq(18047, 266, 3316, string("foqlqmvopcqlwsyximvbpyncqhomhsptvymlbwpmxiseczsystnkbytulduuhsgbvjpcswbzwunzzhvqeuokk"));
	this->vqfghzxscudpcyllrrskhmxb(string("xdrqvxzwbnrdgk"), false, 36292, string("vifssqlrgxrbxnzllvyojpljlhvgdnbrhdgycbyytrsuzfunrzshoapmlblq"), 1197, 15143, 975, true);
	this->eobyguakgnlrzfij();
	this->vqijiwaaruulywaysihit(false, string("szdqlmtjtleedrgladillwbunprchmzfnbcnnzokcdarkevgqiymtqmoezwurucxmsv"), string("mydzhgqrsdhjztqp"), 729, string("jqdbnaoognqlgdlexnmiliwqiogxyjcldopjzkgpxwumfcpuqdtkoseedhbklvyuznthvlnijzslxana"), 20462, 67636, 7856, string("odj"));
	this->rzzpejcmrmfw(false, 56372, 35548, false);
	this->lzkculbyfcnxwm(8244, 23058, 40162);
}


















































































































































































































































































