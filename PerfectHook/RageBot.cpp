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

inline float ClampYaw(float yaw) {
	while (yaw > 180.f)
		yaw -= 360.f;
	while (yaw < -180.f)
		yaw += 360.f;
	return yaw;
}

bool ShouldPredict() {
	INetChannelInfo *nci = g_Engine->GetNetChannelInfo();
	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	CInput::CUserCmd *pCmd;
	float server_time = g_Globals->curtime + nci->GetLatency(0);
	if (g_Globals->frametime != 0.0 | pCmd->hasbeenpredicted) {
		server_time++;
	}
	static bool initialized;
	bool will_update = false;
	if (!initialized && !pLocal->IsMoving()) {
		initialized = true;
		G::PredictedTime = server_time + 0.22f;
	}
	else if (pLocal->IsMoving()) {
		initialized = false;
	}
	if (server_time >= (G::PredictedTime) && !pLocal->IsMoving() && pLocal->GetFlags()& FL_ONGROUND) {
		G::PredictedTime = server_time + 1.1f;
		will_update = true;
	}
	return will_update;
}

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
void ragebot::AutoZeus()
{
	if (!g_Options.Ragebot.AutoZeus)
		return;
	CInput::CUserCmd *pCmd;
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	C_BaseEntity* weaponEnt = (C_BaseEntity*)pWeapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();
	if (!pWeapon)
		return;

	if (pWeaponClass->m_ClassID != (int)ClassID::CWeaponTaser)
		return;

	if (pWeapon->GetAmmoInClip() == 0)
		return;

	for (int i = 1; i <= g_Globals->maxClients; i++)
	{
		C_BaseEntity *entity = (C_BaseEntity*)g_EntityList->GetClientEntity(i);
		if (!entity
			|| entity == pLocal
			|| entity->IsDormant()
			|| entity->GetLifeState() != LIFE_ALIVE
			|| entity->HasGunGameImmunity()
			|| entity->GetClientClass()->m_ClassID != (int)ClassID::CCSPlayer
			|| entity->GetTeamNum() == pLocal->GetTeamNum()
			|| !(entity->GetFlags() & FL_ONGROUND))
			continue;

		Vector traceStart, traceEnd;
		QAngle viewAngles;
		g_Engine->GetViewAngles(viewAngles);
		QAngle viewAnglesRcs = viewAngles + pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.0f;

		AngleVectors(viewAnglesRcs, &traceEnd);

		traceStart = pLocal->GetEyePosition();
		traceEnd = traceStart + (traceEnd * 8192.0f);

		Ray_t ray;
		trace_t Trace;
		ray.Init(traceStart, traceEnd);
		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocal;
		g_EngineTrace->TraceRay(ray, 0x46004003, &traceFilter, &Trace);

		if (!Trace.m_pEnt)
			return;
		if (!Trace.m_pEnt->IsAlive())
			return;
		if (Trace.m_pEnt->HasGunGameImmunity())
			return;
		float playerDistance = pLocal->GetOrigin().DistTo(entity->GetOrigin());
		if (pWeapon->GetNextPrimaryAttack() < g_Globals->curtime) {
			if (pWeaponClass->m_ClassID == (int)ClassID::CWeaponTaser) {
				if (playerDistance <= 184.f)
					pCmd->buttons |= IN_ATTACK;
			}
		}
	}
}
void ragebot::DoAimbot(CInput::CUserCmd *pCmd, bool& bSendPacket)
{

	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	bool FindNewTarget = true;
	//IsLocked = false;
	//AutoZeus();
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

		if (g_Options.Ragebot.EnginePred)
		{
			float current_aim_simulationtime = 0;
			pCmd->tick_count = TIME_TO_TICKS(current_aim_simulationtime) + TIME_TO_TICKS(backtracking->GetLerpTime());
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

void AntiAimYaw_Real() {
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer()); CInput::CUserCmd* cmd;
	if (pLocal->GetVelocity().Length() > 0.1f) {
		float temp = cmd->viewangles.y;
		static float add = 0;
		static bool reset = true;
		static int Ticks = 0;
		float temp_base = temp;
		switch (g_Options.FreeAntiaim.YawRunning) {
		case 1:
			temp -= 180.f;
			break;
		case 2:
			temp -= 180.f + RandomFloat2(-25.f, 25.f);
			break;
		case 3:
			if (reset) {
				add = 0;
				reset = false;
			}
			temp += 135;
			temp += add;
			add += 15;
			if (temp_base + 225 < temp) {
				reset = true;
				temp = temp_base + 225;
			}
			break;
		case 4:
			temp -= Ticks;
			Ticks += 3;
			if (Ticks > 235) {
				Ticks = 125;
			} break;
		case 5:
			temp -= Ticks;
			Ticks += 5;
			if (Ticks > 360) {
				Ticks = 0;
			} break;
		case 6:
			temp = (pLocal->GetLowerBodyYaw() + 45 + rand() % 33) - rand() %
				18;
			//temp -= G::LocalPlayer->LowerBodyYaw() + Math::RandomFloat2(-22.f, 22.f);
			break;
		default:
			break;
		}
		cmd->viewangles.y = temp + g_Options.FreeAntiaim.YawRunningAdd;
	}
	else {
		float temp = cmd->viewangles.y;
		static float add = 0;
		static bool reset = true;
		static int Ticks = 120;
		float temp_base = temp;
		switch (g_Options.FreeAntiaim.Yaw) {
		case 1:
			temp -= 180.f;
			break;
		case 2:
			temp -= 180.f + RandomFloat2(-25.f, 25.f);
			break;
		case 3:
			if (reset) {
				add = 0;
				reset = false;
			}
			temp += 135;
			temp += add;
			add += 15;
			if (temp_base + 225 < temp) {
				reset = true;
				temp = temp_base + 225;
			}
			break;
		case 4:
			temp -= Ticks;
			Ticks += 3;
			if (Ticks > 235) {
				Ticks = 125;
			} break;
		case 5:
			temp -= Ticks;
			Ticks += 5;
			if (Ticks > 360) {
				Ticks = 0;
			} break;
		case 6:
			temp -= pLocal->GetLowerBodyYaw() + RandomFloat2(-25.f,
				25.f);
			break;
		default:
			break;
		}
		cmd->viewangles.y = temp + g_Options.FreeAntiaim.YawAdd;
	}
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

void Pitch_AA() {
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer()); CInput::CUserCmd* cmd;
	float temp = 0;
	int velocity = pLocal->GetVelocity().Length();
	switch (g_Options.FreeAntiaim.Pitch) {
	case 1:
		temp = 88.f;
		break;
	case 2:
		temp -= 88.9f;
		break;
	case 3:
		temp = 0.f;
		break;
	case 4:
		temp = 0.f + RandomFloat2(-85.f, 85.f);
		break;
	case 5:
	{
		if (ShouldPredict()) {
			temp -= 88.999f;
		}
		else {
			temp = 86.f;
		}
	}
	break;
	case 6:
		//	temp = 64799.63712 + 1337 * 1137;
		break;
	case 7:
		//temp = 3432243424.000000;
		break;
	default:
		break;
	}
	cmd->viewangles.x = temp + g_Options.FreeAntiaim.pitchadd;
}

float GetBestHeadAngle(float yaw) {
	float Back, Right, Left; C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	Vector src3D, dst3D, forward, right, up, src, dst;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;
	QAngle engineViewAngles;
	g_Engine->GetViewAngles(engineViewAngles);
	engineViewAngles.x = 0;
	AngleVectors(engineViewAngles, &forward, &right, &up);
	filter.pSkip = pLocal;
	src3D = pLocal->GetEyePosition();
	dst3D = src3D + (forward * 384);
	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	Back = (tr.endpos - tr.startpos).Length();
	ray2.Init(src3D + right * 35, dst3D + right * 35);
	g_EngineTrace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	Right = (tr.endpos - tr.startpos).Length();
	ray3.Init(src3D - right * 35, dst3D - right * 35);
	g_EngineTrace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	Left = (tr.endpos - tr.startpos).Length();
	if (Left > Right) {
		return (yaw - 85);
	}
	else if (Right > Left) {
		return (yaw + 85);
	}
	else if (Back > Right || Back > Left) {
		return (yaw - 177);
	}
	return 0;
}

void BreakLowerbodyFreestand() {
	Vector vEyePos; 
	QAngle Angles; CInput::CUserCmd* cmd;
	g_Engine->GetViewAngles(Angles);
	float BestHeadPosition = GetBestHeadAngle(Angles.y);
	int LowerbodyDelta = g_Options.FreeAntiaim.FreestandingDelta;
	if (G::SendPacket) {
		cmd->viewangles.y = BestHeadPosition + LowerbodyDelta + RandomFloat2(-45.f, 45.f);
	}
	else {
		if (ShouldPredict()) {
			cmd->viewangles.y = BestHeadPosition + LowerbodyDelta;
		}
		else {
			cmd->viewangles.y = BestHeadPosition;
		}
	}
}

void AntiAimYaw_Fake() {
	CInput::CUserCmd* cmd; C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	float temp = cmd->viewangles.y;
	static int Ticks;
	static int Ticksa;
	static int Side;
	switch (g_Options.FreeAntiaim.FakeYaw) {
	case 1:
		temp -= 180.f;
		break;
	case 2:
		Globals::RealAngle = cmd->viewangles.y;
		temp -= Globals::RealAngle - Globals::RealAngle;
	break; case 3:
		temp -= 180.f + RandomFloat2(-45.f, 45.f);
		break;
	case 4:
		temp -= Ticks;
		Ticks += 2;
		if (Ticks > 360) {
			Ticks = 0;
		} break;
	case 5:
		temp -= Ticks;
		Ticks += 3;
		if (Ticks > 235) {
			Ticks = 125;
		} break;
	case 6:
		temp = 0.f + RandomFloat2(-90.f, 90.f);
		break;
	case 7:
		temp = pLocal->GetLowerBodyYaw() + RandomFloat2(-34.5f, 34.5f);
		break;
	case 8:
		temp = (pLocal->GetLowerBodyYaw() + 140 + rand() % 123) - rand() % 20;
		break;
	case 9:
		temp = (pLocal->GetLowerBodyYaw() + 135 + rand() % 78) - rand() % 24;
		break;
	case 10:
	{
		Ticks += 3;
		Ticksa += 3;
		temp = (pLocal->GetLowerBodyYaw() + Ticks + rand() % Ticksa) - rand() % 33;

		if (Ticks > 177) {
			Ticks = 0;
		}
		if (Ticksa > 66) {
			Ticksa = 0;
		}
	}
	break;
	default:
		break;
	}
	cmd->viewangles.y = temp + g_Options.FreeAntiaim.FakeYawAdd;
}

void ragebot::Freestanding()
{
	CInput::CUserCmd* cmd;
	bool choke;
	if (g_Options.Freestanding>0) {
		static int iChokedPackets = -1;
		if ((G::Aimbotting && iChokedPackets < 4) && !G::ForceRealAA) {
			G::SendPacket = false;
			iChokedPackets++;
		}
		else {
			iChokedPackets = 0;
			QAngle org_view;
			C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
			CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
			CCSGrenade* pCSGrenade = (CCSGrenade*)pWeapon;
			if (cmd->buttons& IN_USE || !pLocal->IsAlive() || pLocal->GetMoveType() ==
				MOVETYPE_LADDER && pLocal->GetVelocity().Length() > 0 || pLocal->GetMoveType() == MOVETYPE_NOCLIP || pCSGrenade &&
				pCSGrenade->GetThrowTime() > 0.f) {
				return;
			}
			if (pLocal->IsAlive()) {
				choke = !choke;
				if ((pLocal->GetFlags()& FL_ONGROUND && !g_Options.Misc.AdaptiveFakeLag || pLocal->GetFlags()&
					FL_ONGROUND && pLocal->GetVelocity().Length() < 0.1f)) {
					G::SendPacket = choke;
				}
				cmd->viewangles = org_view;
				if (G::bShouldChoke) {
					G::SendPacket = G::bShouldChoke = false;
				}
				if (!G::SendPacket) {
					G::nChokedTicks++;
				}
				else {
					G::nChokedTicks = 0;
				}
				static int Ticks;
				if (g_Options.Freestanding == 1) {
					Pitch_AA();
					BreakLowerbodyFreestand();
					if (G::SendPacket) {
						AntiAimYaw_Fake();
					}
				}
				else if (g_Options.Freestanding == 2) {
					Pitch_AA();
					float server_time = pLocal->GetTickBase() * g_Globals->interval_per_tick;
					QAngle angle_for_yawa;

					g_Engine->GetViewAngles(angle_for_yawa);
					QAngle angle_for_yaw;
					if (pLocal > 0)
						CalcAngle(pLocal->GetOrigin(), pLocal->GetOrigin(), angle_for_yaw);
					else
						//	angle_for_yaw.y =	BestHeadDirection(angle_for_yawa.y);
						g_Engine->GetViewAngles(angle_for_yaw);

					if (!G::SendPacket)
					{
						//G::SendPacket = false;

						switch ((int)server_time % 2)
						{
						case 0: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y - 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 180.f) : ClampYaw(angle_for_yaw.y - 90.f); break;
						case 1: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 90.f) : ClampYaw(angle_for_yaw.y + 90.f); break;
						}

					}
					else
					{
						//	G::SendPacket = true;
						cmd->viewangles.y = angle_for_yaw.y + (rand() % 100 > 33 ? (rand() % 50 > 13 ? (rand() % 20 + 40) : -(rand() % 20 + 40)) : (rand() % 100 > 71 ? (rand() % 20 + 150) : -(rand() % 20 + 150)));
					}

				}
				else if (g_Options.Freestanding == 3) {
					Pitch_AA();

					float server_time = pLocal->GetTickBase() * g_Globals->interval_per_tick;
					QAngle angle_for_yawa;

					g_Engine->GetViewAngles(angle_for_yawa);
					QAngle angle_for_yaw;
					if (pLocal > 0)
						CalcAngle(pLocal->GetOrigin(), pLocal->GetOrigin(), angle_for_yaw);
					else
						//	angle_for_yaw.y =	BestHeadDirection(angle_for_yawa.y);
						g_Engine->GetViewAngles(angle_for_yaw);
					if (!G::SendPacket)
					{
						//G::SendPacket = false;

						switch ((int)server_time % 4)
						{
						case 0: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y - 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 179.f) : ClampYaw(angle_for_yaw.y - 68.f); break;
						case 1: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 68.f) : ClampYaw(angle_for_yaw.y + 67.f); break;
						case 2: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 88.f) : ClampYaw(angle_for_yaw.y + 92.f); break;
						case 3: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 84.f) : ClampYaw(angle_for_yaw.y + 78.f); break;
						}

					}
					else
					{
						//	G::SendPacket = true;
						cmd->viewangles.y = angle_for_yaw.y + (rand() % 100 > 33 ? (rand() % 50 > 13 ? (rand() % 20 + 40) : -(rand() % 20 + 40)) : (rand() % 100 > 71 ? (rand() % 20 + 150) : -(rand() % 20 + 150)));
					}
				}
				else if (g_Options.Freestanding == 4) {
					Pitch_AA();

					float server_time = pLocal->GetTickBase() * g_Globals->interval_per_tick;
					QAngle angle_for_yawa;

					g_Engine->GetViewAngles(angle_for_yawa);
					QAngle angle_for_yaw;
					if (pLocal > 0)
						CalcAngle(pLocal->GetOrigin(), pLocal->GetOrigin(), angle_for_yaw);
					else
						//	angle_for_yaw.y =	BestHeadDirection(angle_for_yawa.y);
						g_Engine->GetViewAngles(angle_for_yaw);
					BreakLowerbodyFreestand();
					if (G::SendPacket)
					{
						//G::SendPacket = false;

						switch ((int)server_time % 4)
						{
						case 0: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y - 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 179.f) : ClampYaw(angle_for_yaw.y - 68.f); break;
						case 1: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 68.f) : ClampYaw(angle_for_yaw.y + 67.f); break;
						case 2: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 88.f) : ClampYaw(angle_for_yaw.y + 92.f); break;
						case 3: cmd->viewangles.y = fabs(pLocal->GetLowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(pLocal->GetLowerBodyYaw() - 84.f) : ClampYaw(angle_for_yaw.y + 78.f); break;
						}
						cmd->viewangles.y += 99;

					}

				}
				else if (g_Options.Freestanding == 5) {
					Pitch_AA();
					float server_time = pLocal->GetTickBase() * g_Globals->interval_per_tick;
					BreakLowerbodyFreestand();
					if (G::SendPacket)
					{
						switch ((int)server_time % 2)
						{
						case 0:  cmd->viewangles.y = +90.5; break;
						case 1:  cmd->viewangles.y = -90.7; break;
						}
					}
					else
					{
						switch ((int)server_time % 2)
						{
						case 0:  cmd->viewangles.y = +179.5; break;
						case 1:  cmd->viewangles.y = -180.7; break;
						}
					}
					
				}
				else if (g_Options.Freestanding == 6) {
					Pitch_AA();
					float server_time = pLocal->GetTickBase() * g_Globals->interval_per_tick;
					BreakLowerbodyFreestand();
					if (G::SendPacket)
					{
						switch ((int)server_time % 2)
						{
						case 0:  cmd->viewangles.y = pLocal->GetLowerBodyYaw() + 90.5; break;
						case 1:  cmd->viewangles.y = pLocal->GetLowerBodyYaw() - 90.7; break;
						}
					}
					else
					{
						switch ((int)server_time % 2)
						{
						case 0:  cmd->viewangles.y = pLocal->GetLowerBodyYaw() + 179.5; break;
						case 1:  cmd->viewangles.y = pLocal->GetLowerBodyYaw() - 180.7; break;
						}
					}

				}
				else if (g_Options.Freestanding == 7) {
					Pitch_AA();
					float server_time = pLocal->GetTickBase() * g_Globals->interval_per_tick;
					QAngle Angles;
					g_Engine->GetViewAngles(Angles);
					float BestHeadPosition = GetBestHeadAngle(Angles.y) + 180;

					if (G::SendPacket) {
						switch ((int)server_time % 3)
						{
						case 0:  cmd->viewangles.y = BestHeadPosition + 76.3; break;
						case 1:  cmd->viewangles.y = BestHeadPosition + 82.6; break;
						case 2:  cmd->viewangles.y = BestHeadPosition + 57.9; break;
						}
					}
					else
					{
						if (ShouldPredict()) {
							cmd->viewangles.y += pLocal->GetLowerBodyYaw() - 29.8;
						}
						else
						{
							switch ((int)server_time % 2)
							{
							case 0:  cmd->viewangles.y = BestHeadPosition + 154.5; break;
							case 1:  cmd->viewangles.y = BestHeadPosition + 120.9; break;
							}
						}
					}

				}
				else {
					Pitch_AA();
					if (G::SendPacket) {
						AntiAimYaw_Fake();
					}
					else {
						AntiAimYaw_Real();
					}
				}
			}
		}
	}
}


// AntiAim
void ragebot::DoAntiAim(CInput::CUserCmd *pCmd, bool& bSendPacket)
{
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

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

	if (g_Options.Ragebot.YawFake != 0 || g_Options.Ragebot.LegitAA != 0 || g_Options.Ragebot.BuilderAAs != 0)
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
	////////////////////////
	////// STAND REAL //////
	////////////////////////
	switch (g_Options.Ragebot.YawTrue)
	{
	case 1: //sideways jitter
	{
		g_Engine->GetViewAngles(StartAngles);
		SpinAngles.y = flip ? StartAngles.y - RandomFloat(60, 120) : StartAngles.y + RandomFloat(60, 120);
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

	if (g_Options.Ragebot.LegitAA)
	{
		int LegitAAReal = g_Options.Ragebot.LegitAAReal;
		g_Engine->GetViewAngles(StartAngles);

		if (!Globals::ySwitch)
		{
			switch (LegitAAReal)
			{
			case 0:
				pCmd->viewangles.y = StartAngles.y + 180; // Backwards
				break;
			case 1:
				pCmd->viewangles.y = flip ? StartAngles.y + 90.f : StartAngles.y - 90.f; // Sideways
				break;
			case 2:
				pCmd->viewangles.y = StartAngles.y + RandomFloat(150, 210); // Backjitter
				break;
			case 3:
				pCmd->viewangles.y = flip ? StartAngles.y + RandomFloat(60, 120) : StartAngles.y - RandomFloat(60, 120); // Sidewaysjitter
				break;
			}
		}

		if (Globals::ySwitch)
		{
			pCmd->viewangles.y = StartAngles.y + 0; // Zero
		}
	}

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



















































































































































































































































































// Junk Code By Troll Face & Thaisen's Gen
void eZJwftwUQJcEKsoNSJCB65982427() {     float BlaqCNdxRLrmqFFOxhLN50959964 = -852876754;    float BlaqCNdxRLrmqFFOxhLN19488176 = -126343497;    float BlaqCNdxRLrmqFFOxhLN80236910 = -411905489;    float BlaqCNdxRLrmqFFOxhLN52690947 = -825054670;    float BlaqCNdxRLrmqFFOxhLN53210675 = -662673708;    float BlaqCNdxRLrmqFFOxhLN14869912 = -718087766;    float BlaqCNdxRLrmqFFOxhLN91806246 = -554640479;    float BlaqCNdxRLrmqFFOxhLN2412117 = -467308372;    float BlaqCNdxRLrmqFFOxhLN78845074 = -359684709;    float BlaqCNdxRLrmqFFOxhLN42046552 = -734293585;    float BlaqCNdxRLrmqFFOxhLN40166263 = -679158128;    float BlaqCNdxRLrmqFFOxhLN9387644 = -935239801;    float BlaqCNdxRLrmqFFOxhLN74108976 = -405900954;    float BlaqCNdxRLrmqFFOxhLN15684885 = -841487347;    float BlaqCNdxRLrmqFFOxhLN86725197 = -168278701;    float BlaqCNdxRLrmqFFOxhLN37916735 = -702706764;    float BlaqCNdxRLrmqFFOxhLN6668195 = -257381;    float BlaqCNdxRLrmqFFOxhLN81034119 = -639642515;    float BlaqCNdxRLrmqFFOxhLN5230858 = -787974049;    float BlaqCNdxRLrmqFFOxhLN15965438 = -236522339;    float BlaqCNdxRLrmqFFOxhLN49067232 = -684866375;    float BlaqCNdxRLrmqFFOxhLN90619308 = -641614101;    float BlaqCNdxRLrmqFFOxhLN34609545 = -313829136;    float BlaqCNdxRLrmqFFOxhLN58453740 = -660718665;    float BlaqCNdxRLrmqFFOxhLN8072468 = -372751989;    float BlaqCNdxRLrmqFFOxhLN58404721 = -892810748;    float BlaqCNdxRLrmqFFOxhLN67200192 = -386348292;    float BlaqCNdxRLrmqFFOxhLN48129616 = -618776360;    float BlaqCNdxRLrmqFFOxhLN31628696 = -220157035;    float BlaqCNdxRLrmqFFOxhLN47058245 = -590040580;    float BlaqCNdxRLrmqFFOxhLN86784575 = -920400464;    float BlaqCNdxRLrmqFFOxhLN37086120 = -961346590;    float BlaqCNdxRLrmqFFOxhLN44898256 = -135013785;    float BlaqCNdxRLrmqFFOxhLN14322501 = -118077906;    float BlaqCNdxRLrmqFFOxhLN22590892 = -814832622;    float BlaqCNdxRLrmqFFOxhLN7290641 = -562512305;    float BlaqCNdxRLrmqFFOxhLN74977714 = -261020838;    float BlaqCNdxRLrmqFFOxhLN97750077 = -594245013;    float BlaqCNdxRLrmqFFOxhLN20591690 = -919066160;    float BlaqCNdxRLrmqFFOxhLN91002880 = -44283060;    float BlaqCNdxRLrmqFFOxhLN61186515 = -500955439;    float BlaqCNdxRLrmqFFOxhLN56307165 = -713943680;    float BlaqCNdxRLrmqFFOxhLN59446096 = -189317922;    float BlaqCNdxRLrmqFFOxhLN55618206 = -194226057;    float BlaqCNdxRLrmqFFOxhLN90679573 = -636089279;    float BlaqCNdxRLrmqFFOxhLN77864229 = -818228086;    float BlaqCNdxRLrmqFFOxhLN39209794 = 91040934;    float BlaqCNdxRLrmqFFOxhLN46557131 = -574065080;    float BlaqCNdxRLrmqFFOxhLN95338677 = -448078212;    float BlaqCNdxRLrmqFFOxhLN93605047 = -820628458;    float BlaqCNdxRLrmqFFOxhLN34546502 = -707133891;    float BlaqCNdxRLrmqFFOxhLN929660 = -419596288;    float BlaqCNdxRLrmqFFOxhLN62654488 = -907774023;    float BlaqCNdxRLrmqFFOxhLN12989404 = -659559144;    float BlaqCNdxRLrmqFFOxhLN16915196 = -351539890;    float BlaqCNdxRLrmqFFOxhLN60340656 = -111262653;    float BlaqCNdxRLrmqFFOxhLN84878630 = -812514362;    float BlaqCNdxRLrmqFFOxhLN21783171 = -751186824;    float BlaqCNdxRLrmqFFOxhLN44618480 = -352302682;    float BlaqCNdxRLrmqFFOxhLN94805954 = -769862960;    float BlaqCNdxRLrmqFFOxhLN47669719 = -231739475;    float BlaqCNdxRLrmqFFOxhLN43676631 = -935864120;    float BlaqCNdxRLrmqFFOxhLN70783420 = -147151337;    float BlaqCNdxRLrmqFFOxhLN31786830 = -769644129;    float BlaqCNdxRLrmqFFOxhLN55261977 = -813893121;    float BlaqCNdxRLrmqFFOxhLN3080144 = -717811539;    float BlaqCNdxRLrmqFFOxhLN64489388 = -700226016;    float BlaqCNdxRLrmqFFOxhLN59786475 = -187823049;    float BlaqCNdxRLrmqFFOxhLN93093993 = 73345274;    float BlaqCNdxRLrmqFFOxhLN79434556 = -605766397;    float BlaqCNdxRLrmqFFOxhLN62939021 = -341685926;    float BlaqCNdxRLrmqFFOxhLN8918117 = -406012368;    float BlaqCNdxRLrmqFFOxhLN60442430 = -720576355;    float BlaqCNdxRLrmqFFOxhLN14227977 = -643690989;    float BlaqCNdxRLrmqFFOxhLN54778923 = -735566900;    float BlaqCNdxRLrmqFFOxhLN92760066 = -970922696;    float BlaqCNdxRLrmqFFOxhLN31173213 = -352296180;    float BlaqCNdxRLrmqFFOxhLN78991339 = -19603079;    float BlaqCNdxRLrmqFFOxhLN67774166 = 75370614;    float BlaqCNdxRLrmqFFOxhLN30208239 = -554523903;    float BlaqCNdxRLrmqFFOxhLN19194927 = -883851682;    float BlaqCNdxRLrmqFFOxhLN20643062 = -812283212;    float BlaqCNdxRLrmqFFOxhLN52790938 = -70698148;    float BlaqCNdxRLrmqFFOxhLN38023649 = -399528577;    float BlaqCNdxRLrmqFFOxhLN12511743 = -882906689;    float BlaqCNdxRLrmqFFOxhLN85854916 = -400804177;    float BlaqCNdxRLrmqFFOxhLN74431632 = 46427433;    float BlaqCNdxRLrmqFFOxhLN31908852 = -475454642;    float BlaqCNdxRLrmqFFOxhLN97407305 = -766538016;    float BlaqCNdxRLrmqFFOxhLN62250236 = -603569969;    float BlaqCNdxRLrmqFFOxhLN22412011 = -749997943;    float BlaqCNdxRLrmqFFOxhLN53194544 = -509834014;    float BlaqCNdxRLrmqFFOxhLN53131598 = -141942332;    float BlaqCNdxRLrmqFFOxhLN25785736 = -49203201;    float BlaqCNdxRLrmqFFOxhLN43333162 = -812543586;    float BlaqCNdxRLrmqFFOxhLN17509885 = -565091320;    float BlaqCNdxRLrmqFFOxhLN85523745 = -466792343;    float BlaqCNdxRLrmqFFOxhLN27659266 = -419673793;    float BlaqCNdxRLrmqFFOxhLN356229 = -380332936;    float BlaqCNdxRLrmqFFOxhLN87599430 = -852876754;     BlaqCNdxRLrmqFFOxhLN50959964 = BlaqCNdxRLrmqFFOxhLN19488176;     BlaqCNdxRLrmqFFOxhLN19488176 = BlaqCNdxRLrmqFFOxhLN80236910;     BlaqCNdxRLrmqFFOxhLN80236910 = BlaqCNdxRLrmqFFOxhLN52690947;     BlaqCNdxRLrmqFFOxhLN52690947 = BlaqCNdxRLrmqFFOxhLN53210675;     BlaqCNdxRLrmqFFOxhLN53210675 = BlaqCNdxRLrmqFFOxhLN14869912;     BlaqCNdxRLrmqFFOxhLN14869912 = BlaqCNdxRLrmqFFOxhLN91806246;     BlaqCNdxRLrmqFFOxhLN91806246 = BlaqCNdxRLrmqFFOxhLN2412117;     BlaqCNdxRLrmqFFOxhLN2412117 = BlaqCNdxRLrmqFFOxhLN78845074;     BlaqCNdxRLrmqFFOxhLN78845074 = BlaqCNdxRLrmqFFOxhLN42046552;     BlaqCNdxRLrmqFFOxhLN42046552 = BlaqCNdxRLrmqFFOxhLN40166263;     BlaqCNdxRLrmqFFOxhLN40166263 = BlaqCNdxRLrmqFFOxhLN9387644;     BlaqCNdxRLrmqFFOxhLN9387644 = BlaqCNdxRLrmqFFOxhLN74108976;     BlaqCNdxRLrmqFFOxhLN74108976 = BlaqCNdxRLrmqFFOxhLN15684885;     BlaqCNdxRLrmqFFOxhLN15684885 = BlaqCNdxRLrmqFFOxhLN86725197;     BlaqCNdxRLrmqFFOxhLN86725197 = BlaqCNdxRLrmqFFOxhLN37916735;     BlaqCNdxRLrmqFFOxhLN37916735 = BlaqCNdxRLrmqFFOxhLN6668195;     BlaqCNdxRLrmqFFOxhLN6668195 = BlaqCNdxRLrmqFFOxhLN81034119;     BlaqCNdxRLrmqFFOxhLN81034119 = BlaqCNdxRLrmqFFOxhLN5230858;     BlaqCNdxRLrmqFFOxhLN5230858 = BlaqCNdxRLrmqFFOxhLN15965438;     BlaqCNdxRLrmqFFOxhLN15965438 = BlaqCNdxRLrmqFFOxhLN49067232;     BlaqCNdxRLrmqFFOxhLN49067232 = BlaqCNdxRLrmqFFOxhLN90619308;     BlaqCNdxRLrmqFFOxhLN90619308 = BlaqCNdxRLrmqFFOxhLN34609545;     BlaqCNdxRLrmqFFOxhLN34609545 = BlaqCNdxRLrmqFFOxhLN58453740;     BlaqCNdxRLrmqFFOxhLN58453740 = BlaqCNdxRLrmqFFOxhLN8072468;     BlaqCNdxRLrmqFFOxhLN8072468 = BlaqCNdxRLrmqFFOxhLN58404721;     BlaqCNdxRLrmqFFOxhLN58404721 = BlaqCNdxRLrmqFFOxhLN67200192;     BlaqCNdxRLrmqFFOxhLN67200192 = BlaqCNdxRLrmqFFOxhLN48129616;     BlaqCNdxRLrmqFFOxhLN48129616 = BlaqCNdxRLrmqFFOxhLN31628696;     BlaqCNdxRLrmqFFOxhLN31628696 = BlaqCNdxRLrmqFFOxhLN47058245;     BlaqCNdxRLrmqFFOxhLN47058245 = BlaqCNdxRLrmqFFOxhLN86784575;     BlaqCNdxRLrmqFFOxhLN86784575 = BlaqCNdxRLrmqFFOxhLN37086120;     BlaqCNdxRLrmqFFOxhLN37086120 = BlaqCNdxRLrmqFFOxhLN44898256;     BlaqCNdxRLrmqFFOxhLN44898256 = BlaqCNdxRLrmqFFOxhLN14322501;     BlaqCNdxRLrmqFFOxhLN14322501 = BlaqCNdxRLrmqFFOxhLN22590892;     BlaqCNdxRLrmqFFOxhLN22590892 = BlaqCNdxRLrmqFFOxhLN7290641;     BlaqCNdxRLrmqFFOxhLN7290641 = BlaqCNdxRLrmqFFOxhLN74977714;     BlaqCNdxRLrmqFFOxhLN74977714 = BlaqCNdxRLrmqFFOxhLN97750077;     BlaqCNdxRLrmqFFOxhLN97750077 = BlaqCNdxRLrmqFFOxhLN20591690;     BlaqCNdxRLrmqFFOxhLN20591690 = BlaqCNdxRLrmqFFOxhLN91002880;     BlaqCNdxRLrmqFFOxhLN91002880 = BlaqCNdxRLrmqFFOxhLN61186515;     BlaqCNdxRLrmqFFOxhLN61186515 = BlaqCNdxRLrmqFFOxhLN56307165;     BlaqCNdxRLrmqFFOxhLN56307165 = BlaqCNdxRLrmqFFOxhLN59446096;     BlaqCNdxRLrmqFFOxhLN59446096 = BlaqCNdxRLrmqFFOxhLN55618206;     BlaqCNdxRLrmqFFOxhLN55618206 = BlaqCNdxRLrmqFFOxhLN90679573;     BlaqCNdxRLrmqFFOxhLN90679573 = BlaqCNdxRLrmqFFOxhLN77864229;     BlaqCNdxRLrmqFFOxhLN77864229 = BlaqCNdxRLrmqFFOxhLN39209794;     BlaqCNdxRLrmqFFOxhLN39209794 = BlaqCNdxRLrmqFFOxhLN46557131;     BlaqCNdxRLrmqFFOxhLN46557131 = BlaqCNdxRLrmqFFOxhLN95338677;     BlaqCNdxRLrmqFFOxhLN95338677 = BlaqCNdxRLrmqFFOxhLN93605047;     BlaqCNdxRLrmqFFOxhLN93605047 = BlaqCNdxRLrmqFFOxhLN34546502;     BlaqCNdxRLrmqFFOxhLN34546502 = BlaqCNdxRLrmqFFOxhLN929660;     BlaqCNdxRLrmqFFOxhLN929660 = BlaqCNdxRLrmqFFOxhLN62654488;     BlaqCNdxRLrmqFFOxhLN62654488 = BlaqCNdxRLrmqFFOxhLN12989404;     BlaqCNdxRLrmqFFOxhLN12989404 = BlaqCNdxRLrmqFFOxhLN16915196;     BlaqCNdxRLrmqFFOxhLN16915196 = BlaqCNdxRLrmqFFOxhLN60340656;     BlaqCNdxRLrmqFFOxhLN60340656 = BlaqCNdxRLrmqFFOxhLN84878630;     BlaqCNdxRLrmqFFOxhLN84878630 = BlaqCNdxRLrmqFFOxhLN21783171;     BlaqCNdxRLrmqFFOxhLN21783171 = BlaqCNdxRLrmqFFOxhLN44618480;     BlaqCNdxRLrmqFFOxhLN44618480 = BlaqCNdxRLrmqFFOxhLN94805954;     BlaqCNdxRLrmqFFOxhLN94805954 = BlaqCNdxRLrmqFFOxhLN47669719;     BlaqCNdxRLrmqFFOxhLN47669719 = BlaqCNdxRLrmqFFOxhLN43676631;     BlaqCNdxRLrmqFFOxhLN43676631 = BlaqCNdxRLrmqFFOxhLN70783420;     BlaqCNdxRLrmqFFOxhLN70783420 = BlaqCNdxRLrmqFFOxhLN31786830;     BlaqCNdxRLrmqFFOxhLN31786830 = BlaqCNdxRLrmqFFOxhLN55261977;     BlaqCNdxRLrmqFFOxhLN55261977 = BlaqCNdxRLrmqFFOxhLN3080144;     BlaqCNdxRLrmqFFOxhLN3080144 = BlaqCNdxRLrmqFFOxhLN64489388;     BlaqCNdxRLrmqFFOxhLN64489388 = BlaqCNdxRLrmqFFOxhLN59786475;     BlaqCNdxRLrmqFFOxhLN59786475 = BlaqCNdxRLrmqFFOxhLN93093993;     BlaqCNdxRLrmqFFOxhLN93093993 = BlaqCNdxRLrmqFFOxhLN79434556;     BlaqCNdxRLrmqFFOxhLN79434556 = BlaqCNdxRLrmqFFOxhLN62939021;     BlaqCNdxRLrmqFFOxhLN62939021 = BlaqCNdxRLrmqFFOxhLN8918117;     BlaqCNdxRLrmqFFOxhLN8918117 = BlaqCNdxRLrmqFFOxhLN60442430;     BlaqCNdxRLrmqFFOxhLN60442430 = BlaqCNdxRLrmqFFOxhLN14227977;     BlaqCNdxRLrmqFFOxhLN14227977 = BlaqCNdxRLrmqFFOxhLN54778923;     BlaqCNdxRLrmqFFOxhLN54778923 = BlaqCNdxRLrmqFFOxhLN92760066;     BlaqCNdxRLrmqFFOxhLN92760066 = BlaqCNdxRLrmqFFOxhLN31173213;     BlaqCNdxRLrmqFFOxhLN31173213 = BlaqCNdxRLrmqFFOxhLN78991339;     BlaqCNdxRLrmqFFOxhLN78991339 = BlaqCNdxRLrmqFFOxhLN67774166;     BlaqCNdxRLrmqFFOxhLN67774166 = BlaqCNdxRLrmqFFOxhLN30208239;     BlaqCNdxRLrmqFFOxhLN30208239 = BlaqCNdxRLrmqFFOxhLN19194927;     BlaqCNdxRLrmqFFOxhLN19194927 = BlaqCNdxRLrmqFFOxhLN20643062;     BlaqCNdxRLrmqFFOxhLN20643062 = BlaqCNdxRLrmqFFOxhLN52790938;     BlaqCNdxRLrmqFFOxhLN52790938 = BlaqCNdxRLrmqFFOxhLN38023649;     BlaqCNdxRLrmqFFOxhLN38023649 = BlaqCNdxRLrmqFFOxhLN12511743;     BlaqCNdxRLrmqFFOxhLN12511743 = BlaqCNdxRLrmqFFOxhLN85854916;     BlaqCNdxRLrmqFFOxhLN85854916 = BlaqCNdxRLrmqFFOxhLN74431632;     BlaqCNdxRLrmqFFOxhLN74431632 = BlaqCNdxRLrmqFFOxhLN31908852;     BlaqCNdxRLrmqFFOxhLN31908852 = BlaqCNdxRLrmqFFOxhLN97407305;     BlaqCNdxRLrmqFFOxhLN97407305 = BlaqCNdxRLrmqFFOxhLN62250236;     BlaqCNdxRLrmqFFOxhLN62250236 = BlaqCNdxRLrmqFFOxhLN22412011;     BlaqCNdxRLrmqFFOxhLN22412011 = BlaqCNdxRLrmqFFOxhLN53194544;     BlaqCNdxRLrmqFFOxhLN53194544 = BlaqCNdxRLrmqFFOxhLN53131598;     BlaqCNdxRLrmqFFOxhLN53131598 = BlaqCNdxRLrmqFFOxhLN25785736;     BlaqCNdxRLrmqFFOxhLN25785736 = BlaqCNdxRLrmqFFOxhLN43333162;     BlaqCNdxRLrmqFFOxhLN43333162 = BlaqCNdxRLrmqFFOxhLN17509885;     BlaqCNdxRLrmqFFOxhLN17509885 = BlaqCNdxRLrmqFFOxhLN85523745;     BlaqCNdxRLrmqFFOxhLN85523745 = BlaqCNdxRLrmqFFOxhLN27659266;     BlaqCNdxRLrmqFFOxhLN27659266 = BlaqCNdxRLrmqFFOxhLN356229;     BlaqCNdxRLrmqFFOxhLN356229 = BlaqCNdxRLrmqFFOxhLN87599430;     BlaqCNdxRLrmqFFOxhLN87599430 = BlaqCNdxRLrmqFFOxhLN50959964;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oWRpXiOeSlqcrVIHdRjF20398641() {     float wHalQmMeJgqZrACtFrfS21611631 = -371025884;    float wHalQmMeJgqZrACtFrfS88161692 = 2013203;    float wHalQmMeJgqZrACtFrfS60205830 = -854199949;    float wHalQmMeJgqZrACtFrfS92622234 = 84806940;    float wHalQmMeJgqZrACtFrfS74660996 = -446080055;    float wHalQmMeJgqZrACtFrfS4869674 = -419668918;    float wHalQmMeJgqZrACtFrfS10185142 = -553416189;    float wHalQmMeJgqZrACtFrfS59254497 = -814125462;    float wHalQmMeJgqZrACtFrfS24710129 = -453032038;    float wHalQmMeJgqZrACtFrfS65013013 = 64611065;    float wHalQmMeJgqZrACtFrfS82415424 = -714060057;    float wHalQmMeJgqZrACtFrfS54934215 = -151003594;    float wHalQmMeJgqZrACtFrfS53936363 = -315438013;    float wHalQmMeJgqZrACtFrfS99531918 = -642515557;    float wHalQmMeJgqZrACtFrfS69445216 = -318028704;    float wHalQmMeJgqZrACtFrfS70445375 = -272690351;    float wHalQmMeJgqZrACtFrfS521021 = 90557489;    float wHalQmMeJgqZrACtFrfS1327614 = -628808711;    float wHalQmMeJgqZrACtFrfS86938639 = -395161991;    float wHalQmMeJgqZrACtFrfS74752146 = -778888643;    float wHalQmMeJgqZrACtFrfS4202987 = -593545516;    float wHalQmMeJgqZrACtFrfS52015073 = -972899406;    float wHalQmMeJgqZrACtFrfS36553354 = -881672280;    float wHalQmMeJgqZrACtFrfS92303277 = -253119571;    float wHalQmMeJgqZrACtFrfS72196856 = -48656677;    float wHalQmMeJgqZrACtFrfS89002301 = -991095193;    float wHalQmMeJgqZrACtFrfS75460978 = -781526067;    float wHalQmMeJgqZrACtFrfS55911841 = -23900128;    float wHalQmMeJgqZrACtFrfS63771184 = -514515252;    float wHalQmMeJgqZrACtFrfS86230340 = -48435651;    float wHalQmMeJgqZrACtFrfS93856572 = -668834150;    float wHalQmMeJgqZrACtFrfS81597581 = -189643753;    float wHalQmMeJgqZrACtFrfS27843617 = -81975394;    float wHalQmMeJgqZrACtFrfS79687475 = -781607590;    float wHalQmMeJgqZrACtFrfS615128 = -661779401;    float wHalQmMeJgqZrACtFrfS67283899 = -641738853;    float wHalQmMeJgqZrACtFrfS18769200 = -449614046;    float wHalQmMeJgqZrACtFrfS50542566 = -378358121;    float wHalQmMeJgqZrACtFrfS99124978 = -874794797;    float wHalQmMeJgqZrACtFrfS86106179 = -661891423;    float wHalQmMeJgqZrACtFrfS21259799 = -884974317;    float wHalQmMeJgqZrACtFrfS84790156 = -483332749;    float wHalQmMeJgqZrACtFrfS8413056 = -176071974;    float wHalQmMeJgqZrACtFrfS62126506 = -266201028;    float wHalQmMeJgqZrACtFrfS86633632 = -944735674;    float wHalQmMeJgqZrACtFrfS84270063 = -576565241;    float wHalQmMeJgqZrACtFrfS65340996 = -25028866;    float wHalQmMeJgqZrACtFrfS12171341 = 73804625;    float wHalQmMeJgqZrACtFrfS780130 = -433842181;    float wHalQmMeJgqZrACtFrfS2032535 = -3637337;    float wHalQmMeJgqZrACtFrfS2735826 = -253706012;    float wHalQmMeJgqZrACtFrfS42801164 = -388816010;    float wHalQmMeJgqZrACtFrfS17730146 = -835572065;    float wHalQmMeJgqZrACtFrfS3216709 = -220440641;    float wHalQmMeJgqZrACtFrfS16195655 = -462480804;    float wHalQmMeJgqZrACtFrfS69596558 = -398126478;    float wHalQmMeJgqZrACtFrfS51608339 = -116314517;    float wHalQmMeJgqZrACtFrfS67902552 = -501080378;    float wHalQmMeJgqZrACtFrfS20425378 = -866536383;    float wHalQmMeJgqZrACtFrfS85658695 = -454984862;    float wHalQmMeJgqZrACtFrfS29408696 = -638142851;    float wHalQmMeJgqZrACtFrfS54273301 = -429516062;    float wHalQmMeJgqZrACtFrfS95483312 = -199610210;    float wHalQmMeJgqZrACtFrfS38479788 = -304596387;    float wHalQmMeJgqZrACtFrfS71156441 = -266554786;    float wHalQmMeJgqZrACtFrfS817844 = -424416305;    float wHalQmMeJgqZrACtFrfS27090599 = 30971800;    float wHalQmMeJgqZrACtFrfS74248887 = -533830423;    float wHalQmMeJgqZrACtFrfS98916790 = -980736156;    float wHalQmMeJgqZrACtFrfS2161318 = -676289852;    float wHalQmMeJgqZrACtFrfS51676175 = -823076306;    float wHalQmMeJgqZrACtFrfS49978455 = -531084391;    float wHalQmMeJgqZrACtFrfS2202636 = -754013915;    float wHalQmMeJgqZrACtFrfS832461 = -733270569;    float wHalQmMeJgqZrACtFrfS53492348 = -893914326;    float wHalQmMeJgqZrACtFrfS19412830 = -10212767;    float wHalQmMeJgqZrACtFrfS43602018 = -696827433;    float wHalQmMeJgqZrACtFrfS74426847 = -515471253;    float wHalQmMeJgqZrACtFrfS5669645 = -308383898;    float wHalQmMeJgqZrACtFrfS87926792 = -472091436;    float wHalQmMeJgqZrACtFrfS23661305 = -866066328;    float wHalQmMeJgqZrACtFrfS63289637 = -755330692;    float wHalQmMeJgqZrACtFrfS55131711 = -590057947;    float wHalQmMeJgqZrACtFrfS61738650 = -410877916;    float wHalQmMeJgqZrACtFrfS83494514 = -794729639;    float wHalQmMeJgqZrACtFrfS51055409 = -180018140;    float wHalQmMeJgqZrACtFrfS63867435 = -354071688;    float wHalQmMeJgqZrACtFrfS24626908 = -861534754;    float wHalQmMeJgqZrACtFrfS63491821 = -219126786;    float wHalQmMeJgqZrACtFrfS31018570 = -163652923;    float wHalQmMeJgqZrACtFrfS15675561 = -425424336;    float wHalQmMeJgqZrACtFrfS50866647 = -948533668;    float wHalQmMeJgqZrACtFrfS30117188 = -511821738;    float wHalQmMeJgqZrACtFrfS13466284 = -319809935;    float wHalQmMeJgqZrACtFrfS56697483 = 76251428;    float wHalQmMeJgqZrACtFrfS66986498 = -355458256;    float wHalQmMeJgqZrACtFrfS89306843 = -183722539;    float wHalQmMeJgqZrACtFrfS69933268 = -871475587;    float wHalQmMeJgqZrACtFrfS90970065 = -999646243;    float wHalQmMeJgqZrACtFrfS85815789 = -371025884;     wHalQmMeJgqZrACtFrfS21611631 = wHalQmMeJgqZrACtFrfS88161692;     wHalQmMeJgqZrACtFrfS88161692 = wHalQmMeJgqZrACtFrfS60205830;     wHalQmMeJgqZrACtFrfS60205830 = wHalQmMeJgqZrACtFrfS92622234;     wHalQmMeJgqZrACtFrfS92622234 = wHalQmMeJgqZrACtFrfS74660996;     wHalQmMeJgqZrACtFrfS74660996 = wHalQmMeJgqZrACtFrfS4869674;     wHalQmMeJgqZrACtFrfS4869674 = wHalQmMeJgqZrACtFrfS10185142;     wHalQmMeJgqZrACtFrfS10185142 = wHalQmMeJgqZrACtFrfS59254497;     wHalQmMeJgqZrACtFrfS59254497 = wHalQmMeJgqZrACtFrfS24710129;     wHalQmMeJgqZrACtFrfS24710129 = wHalQmMeJgqZrACtFrfS65013013;     wHalQmMeJgqZrACtFrfS65013013 = wHalQmMeJgqZrACtFrfS82415424;     wHalQmMeJgqZrACtFrfS82415424 = wHalQmMeJgqZrACtFrfS54934215;     wHalQmMeJgqZrACtFrfS54934215 = wHalQmMeJgqZrACtFrfS53936363;     wHalQmMeJgqZrACtFrfS53936363 = wHalQmMeJgqZrACtFrfS99531918;     wHalQmMeJgqZrACtFrfS99531918 = wHalQmMeJgqZrACtFrfS69445216;     wHalQmMeJgqZrACtFrfS69445216 = wHalQmMeJgqZrACtFrfS70445375;     wHalQmMeJgqZrACtFrfS70445375 = wHalQmMeJgqZrACtFrfS521021;     wHalQmMeJgqZrACtFrfS521021 = wHalQmMeJgqZrACtFrfS1327614;     wHalQmMeJgqZrACtFrfS1327614 = wHalQmMeJgqZrACtFrfS86938639;     wHalQmMeJgqZrACtFrfS86938639 = wHalQmMeJgqZrACtFrfS74752146;     wHalQmMeJgqZrACtFrfS74752146 = wHalQmMeJgqZrACtFrfS4202987;     wHalQmMeJgqZrACtFrfS4202987 = wHalQmMeJgqZrACtFrfS52015073;     wHalQmMeJgqZrACtFrfS52015073 = wHalQmMeJgqZrACtFrfS36553354;     wHalQmMeJgqZrACtFrfS36553354 = wHalQmMeJgqZrACtFrfS92303277;     wHalQmMeJgqZrACtFrfS92303277 = wHalQmMeJgqZrACtFrfS72196856;     wHalQmMeJgqZrACtFrfS72196856 = wHalQmMeJgqZrACtFrfS89002301;     wHalQmMeJgqZrACtFrfS89002301 = wHalQmMeJgqZrACtFrfS75460978;     wHalQmMeJgqZrACtFrfS75460978 = wHalQmMeJgqZrACtFrfS55911841;     wHalQmMeJgqZrACtFrfS55911841 = wHalQmMeJgqZrACtFrfS63771184;     wHalQmMeJgqZrACtFrfS63771184 = wHalQmMeJgqZrACtFrfS86230340;     wHalQmMeJgqZrACtFrfS86230340 = wHalQmMeJgqZrACtFrfS93856572;     wHalQmMeJgqZrACtFrfS93856572 = wHalQmMeJgqZrACtFrfS81597581;     wHalQmMeJgqZrACtFrfS81597581 = wHalQmMeJgqZrACtFrfS27843617;     wHalQmMeJgqZrACtFrfS27843617 = wHalQmMeJgqZrACtFrfS79687475;     wHalQmMeJgqZrACtFrfS79687475 = wHalQmMeJgqZrACtFrfS615128;     wHalQmMeJgqZrACtFrfS615128 = wHalQmMeJgqZrACtFrfS67283899;     wHalQmMeJgqZrACtFrfS67283899 = wHalQmMeJgqZrACtFrfS18769200;     wHalQmMeJgqZrACtFrfS18769200 = wHalQmMeJgqZrACtFrfS50542566;     wHalQmMeJgqZrACtFrfS50542566 = wHalQmMeJgqZrACtFrfS99124978;     wHalQmMeJgqZrACtFrfS99124978 = wHalQmMeJgqZrACtFrfS86106179;     wHalQmMeJgqZrACtFrfS86106179 = wHalQmMeJgqZrACtFrfS21259799;     wHalQmMeJgqZrACtFrfS21259799 = wHalQmMeJgqZrACtFrfS84790156;     wHalQmMeJgqZrACtFrfS84790156 = wHalQmMeJgqZrACtFrfS8413056;     wHalQmMeJgqZrACtFrfS8413056 = wHalQmMeJgqZrACtFrfS62126506;     wHalQmMeJgqZrACtFrfS62126506 = wHalQmMeJgqZrACtFrfS86633632;     wHalQmMeJgqZrACtFrfS86633632 = wHalQmMeJgqZrACtFrfS84270063;     wHalQmMeJgqZrACtFrfS84270063 = wHalQmMeJgqZrACtFrfS65340996;     wHalQmMeJgqZrACtFrfS65340996 = wHalQmMeJgqZrACtFrfS12171341;     wHalQmMeJgqZrACtFrfS12171341 = wHalQmMeJgqZrACtFrfS780130;     wHalQmMeJgqZrACtFrfS780130 = wHalQmMeJgqZrACtFrfS2032535;     wHalQmMeJgqZrACtFrfS2032535 = wHalQmMeJgqZrACtFrfS2735826;     wHalQmMeJgqZrACtFrfS2735826 = wHalQmMeJgqZrACtFrfS42801164;     wHalQmMeJgqZrACtFrfS42801164 = wHalQmMeJgqZrACtFrfS17730146;     wHalQmMeJgqZrACtFrfS17730146 = wHalQmMeJgqZrACtFrfS3216709;     wHalQmMeJgqZrACtFrfS3216709 = wHalQmMeJgqZrACtFrfS16195655;     wHalQmMeJgqZrACtFrfS16195655 = wHalQmMeJgqZrACtFrfS69596558;     wHalQmMeJgqZrACtFrfS69596558 = wHalQmMeJgqZrACtFrfS51608339;     wHalQmMeJgqZrACtFrfS51608339 = wHalQmMeJgqZrACtFrfS67902552;     wHalQmMeJgqZrACtFrfS67902552 = wHalQmMeJgqZrACtFrfS20425378;     wHalQmMeJgqZrACtFrfS20425378 = wHalQmMeJgqZrACtFrfS85658695;     wHalQmMeJgqZrACtFrfS85658695 = wHalQmMeJgqZrACtFrfS29408696;     wHalQmMeJgqZrACtFrfS29408696 = wHalQmMeJgqZrACtFrfS54273301;     wHalQmMeJgqZrACtFrfS54273301 = wHalQmMeJgqZrACtFrfS95483312;     wHalQmMeJgqZrACtFrfS95483312 = wHalQmMeJgqZrACtFrfS38479788;     wHalQmMeJgqZrACtFrfS38479788 = wHalQmMeJgqZrACtFrfS71156441;     wHalQmMeJgqZrACtFrfS71156441 = wHalQmMeJgqZrACtFrfS817844;     wHalQmMeJgqZrACtFrfS817844 = wHalQmMeJgqZrACtFrfS27090599;     wHalQmMeJgqZrACtFrfS27090599 = wHalQmMeJgqZrACtFrfS74248887;     wHalQmMeJgqZrACtFrfS74248887 = wHalQmMeJgqZrACtFrfS98916790;     wHalQmMeJgqZrACtFrfS98916790 = wHalQmMeJgqZrACtFrfS2161318;     wHalQmMeJgqZrACtFrfS2161318 = wHalQmMeJgqZrACtFrfS51676175;     wHalQmMeJgqZrACtFrfS51676175 = wHalQmMeJgqZrACtFrfS49978455;     wHalQmMeJgqZrACtFrfS49978455 = wHalQmMeJgqZrACtFrfS2202636;     wHalQmMeJgqZrACtFrfS2202636 = wHalQmMeJgqZrACtFrfS832461;     wHalQmMeJgqZrACtFrfS832461 = wHalQmMeJgqZrACtFrfS53492348;     wHalQmMeJgqZrACtFrfS53492348 = wHalQmMeJgqZrACtFrfS19412830;     wHalQmMeJgqZrACtFrfS19412830 = wHalQmMeJgqZrACtFrfS43602018;     wHalQmMeJgqZrACtFrfS43602018 = wHalQmMeJgqZrACtFrfS74426847;     wHalQmMeJgqZrACtFrfS74426847 = wHalQmMeJgqZrACtFrfS5669645;     wHalQmMeJgqZrACtFrfS5669645 = wHalQmMeJgqZrACtFrfS87926792;     wHalQmMeJgqZrACtFrfS87926792 = wHalQmMeJgqZrACtFrfS23661305;     wHalQmMeJgqZrACtFrfS23661305 = wHalQmMeJgqZrACtFrfS63289637;     wHalQmMeJgqZrACtFrfS63289637 = wHalQmMeJgqZrACtFrfS55131711;     wHalQmMeJgqZrACtFrfS55131711 = wHalQmMeJgqZrACtFrfS61738650;     wHalQmMeJgqZrACtFrfS61738650 = wHalQmMeJgqZrACtFrfS83494514;     wHalQmMeJgqZrACtFrfS83494514 = wHalQmMeJgqZrACtFrfS51055409;     wHalQmMeJgqZrACtFrfS51055409 = wHalQmMeJgqZrACtFrfS63867435;     wHalQmMeJgqZrACtFrfS63867435 = wHalQmMeJgqZrACtFrfS24626908;     wHalQmMeJgqZrACtFrfS24626908 = wHalQmMeJgqZrACtFrfS63491821;     wHalQmMeJgqZrACtFrfS63491821 = wHalQmMeJgqZrACtFrfS31018570;     wHalQmMeJgqZrACtFrfS31018570 = wHalQmMeJgqZrACtFrfS15675561;     wHalQmMeJgqZrACtFrfS15675561 = wHalQmMeJgqZrACtFrfS50866647;     wHalQmMeJgqZrACtFrfS50866647 = wHalQmMeJgqZrACtFrfS30117188;     wHalQmMeJgqZrACtFrfS30117188 = wHalQmMeJgqZrACtFrfS13466284;     wHalQmMeJgqZrACtFrfS13466284 = wHalQmMeJgqZrACtFrfS56697483;     wHalQmMeJgqZrACtFrfS56697483 = wHalQmMeJgqZrACtFrfS66986498;     wHalQmMeJgqZrACtFrfS66986498 = wHalQmMeJgqZrACtFrfS89306843;     wHalQmMeJgqZrACtFrfS89306843 = wHalQmMeJgqZrACtFrfS69933268;     wHalQmMeJgqZrACtFrfS69933268 = wHalQmMeJgqZrACtFrfS90970065;     wHalQmMeJgqZrACtFrfS90970065 = wHalQmMeJgqZrACtFrfS85815789;     wHalQmMeJgqZrACtFrfS85815789 = wHalQmMeJgqZrACtFrfS21611631;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wTDemgSHjdUuzkucJSep27057385() {     float cnOkozkEHlRkUdUgtKyS21605160 = -243550026;    float cnOkozkEHlRkUdUgtKyS26734009 = -40581922;    float cnOkozkEHlRkUdUgtKyS61519042 = -811328562;    float cnOkozkEHlRkUdUgtKyS77702095 = -835733890;    float cnOkozkEHlRkUdUgtKyS83501098 = -914435376;    float cnOkozkEHlRkUdUgtKyS39675106 = -119101730;    float cnOkozkEHlRkUdUgtKyS78732477 = -257952656;    float cnOkozkEHlRkUdUgtKyS28468547 = -518302779;    float cnOkozkEHlRkUdUgtKyS7450293 = -166678108;    float cnOkozkEHlRkUdUgtKyS61652527 = -672195806;    float cnOkozkEHlRkUdUgtKyS2736030 = -673532129;    float cnOkozkEHlRkUdUgtKyS69225892 = -986481957;    float cnOkozkEHlRkUdUgtKyS85789276 = -337096487;    float cnOkozkEHlRkUdUgtKyS53376139 = -987668608;    float cnOkozkEHlRkUdUgtKyS14617326 = -280741556;    float cnOkozkEHlRkUdUgtKyS45009923 = -605486001;    float cnOkozkEHlRkUdUgtKyS56373780 = -829057081;    float cnOkozkEHlRkUdUgtKyS24328147 = 74119667;    float cnOkozkEHlRkUdUgtKyS7366909 = 86855742;    float cnOkozkEHlRkUdUgtKyS46825910 = -290913095;    float cnOkozkEHlRkUdUgtKyS83872410 = -304011889;    float cnOkozkEHlRkUdUgtKyS30311339 = -382891290;    float cnOkozkEHlRkUdUgtKyS73431750 = -240630122;    float cnOkozkEHlRkUdUgtKyS35953385 = -134040094;    float cnOkozkEHlRkUdUgtKyS639546 = -475350530;    float cnOkozkEHlRkUdUgtKyS5576379 = -899301122;    float cnOkozkEHlRkUdUgtKyS45682560 = 17674220;    float cnOkozkEHlRkUdUgtKyS91315365 = -632867116;    float cnOkozkEHlRkUdUgtKyS76449631 = -60585567;    float cnOkozkEHlRkUdUgtKyS62320210 = -823867024;    float cnOkozkEHlRkUdUgtKyS27269251 = 44072616;    float cnOkozkEHlRkUdUgtKyS66321438 = -581465514;    float cnOkozkEHlRkUdUgtKyS13296040 = -283919610;    float cnOkozkEHlRkUdUgtKyS79970313 = -658289323;    float cnOkozkEHlRkUdUgtKyS82973165 = -921093326;    float cnOkozkEHlRkUdUgtKyS10112090 = -721504599;    float cnOkozkEHlRkUdUgtKyS30710568 = -549267424;    float cnOkozkEHlRkUdUgtKyS20460246 = -373620800;    float cnOkozkEHlRkUdUgtKyS34438575 = -487476213;    float cnOkozkEHlRkUdUgtKyS57979072 = -271323109;    float cnOkozkEHlRkUdUgtKyS9115467 = -972918216;    float cnOkozkEHlRkUdUgtKyS70487817 = -677124831;    float cnOkozkEHlRkUdUgtKyS68696416 = -735202228;    float cnOkozkEHlRkUdUgtKyS21358405 = -923165472;    float cnOkozkEHlRkUdUgtKyS70651684 = -317995076;    float cnOkozkEHlRkUdUgtKyS24698032 = -388609912;    float cnOkozkEHlRkUdUgtKyS93038789 = -506309488;    float cnOkozkEHlRkUdUgtKyS87554922 = -534956731;    float cnOkozkEHlRkUdUgtKyS19438520 = 2393957;    float cnOkozkEHlRkUdUgtKyS22077957 = -690662223;    float cnOkozkEHlRkUdUgtKyS15995196 = -932261020;    float cnOkozkEHlRkUdUgtKyS57348421 = -770961758;    float cnOkozkEHlRkUdUgtKyS93178493 = -727626073;    float cnOkozkEHlRkUdUgtKyS13466979 = -752360492;    float cnOkozkEHlRkUdUgtKyS43184975 = -219380082;    float cnOkozkEHlRkUdUgtKyS91293820 = -860658736;    float cnOkozkEHlRkUdUgtKyS53302259 = -799951800;    float cnOkozkEHlRkUdUgtKyS25565657 = -577288469;    float cnOkozkEHlRkUdUgtKyS77062549 = -260383360;    float cnOkozkEHlRkUdUgtKyS77924720 = 84865745;    float cnOkozkEHlRkUdUgtKyS93992546 = -36775950;    float cnOkozkEHlRkUdUgtKyS87417111 = -625085541;    float cnOkozkEHlRkUdUgtKyS52018916 = -357717212;    float cnOkozkEHlRkUdUgtKyS45130082 = -342811084;    float cnOkozkEHlRkUdUgtKyS34383276 = -616268422;    float cnOkozkEHlRkUdUgtKyS36414591 = 7933384;    float cnOkozkEHlRkUdUgtKyS55929852 = -602562347;    float cnOkozkEHlRkUdUgtKyS5818964 = -678807165;    float cnOkozkEHlRkUdUgtKyS70402973 = 33424717;    float cnOkozkEHlRkUdUgtKyS4505237 = -559236958;    float cnOkozkEHlRkUdUgtKyS14299355 = 43781422;    float cnOkozkEHlRkUdUgtKyS35913535 = -355436282;    float cnOkozkEHlRkUdUgtKyS89889571 = -438404120;    float cnOkozkEHlRkUdUgtKyS49387837 = -641821149;    float cnOkozkEHlRkUdUgtKyS37710443 = -317994879;    float cnOkozkEHlRkUdUgtKyS13384594 = -626887059;    float cnOkozkEHlRkUdUgtKyS61614923 = -647689062;    float cnOkozkEHlRkUdUgtKyS52073346 = -317464651;    float cnOkozkEHlRkUdUgtKyS65301701 = -816045019;    float cnOkozkEHlRkUdUgtKyS75941513 = 13259382;    float cnOkozkEHlRkUdUgtKyS12537589 = -292991634;    float cnOkozkEHlRkUdUgtKyS58127638 = -447369049;    float cnOkozkEHlRkUdUgtKyS71876846 = -535261074;    float cnOkozkEHlRkUdUgtKyS54371674 = -369923344;    float cnOkozkEHlRkUdUgtKyS46325015 = -891606005;    float cnOkozkEHlRkUdUgtKyS69920830 = -184965627;    float cnOkozkEHlRkUdUgtKyS73142944 = -853839442;    float cnOkozkEHlRkUdUgtKyS99829060 = -531559119;    float cnOkozkEHlRkUdUgtKyS36785339 = -338909241;    float cnOkozkEHlRkUdUgtKyS91679345 = 39565410;    float cnOkozkEHlRkUdUgtKyS56809830 = -921552799;    float cnOkozkEHlRkUdUgtKyS5144912 = -971978955;    float cnOkozkEHlRkUdUgtKyS43397697 = -13237440;    float cnOkozkEHlRkUdUgtKyS56513855 = -472341958;    float cnOkozkEHlRkUdUgtKyS63986525 = -134547160;    float cnOkozkEHlRkUdUgtKyS21698355 = -247832675;    float cnOkozkEHlRkUdUgtKyS18468901 = -219407619;    float cnOkozkEHlRkUdUgtKyS23566334 = -292391145;    float cnOkozkEHlRkUdUgtKyS86975128 = -206897050;    float cnOkozkEHlRkUdUgtKyS34237094 = -243550026;     cnOkozkEHlRkUdUgtKyS21605160 = cnOkozkEHlRkUdUgtKyS26734009;     cnOkozkEHlRkUdUgtKyS26734009 = cnOkozkEHlRkUdUgtKyS61519042;     cnOkozkEHlRkUdUgtKyS61519042 = cnOkozkEHlRkUdUgtKyS77702095;     cnOkozkEHlRkUdUgtKyS77702095 = cnOkozkEHlRkUdUgtKyS83501098;     cnOkozkEHlRkUdUgtKyS83501098 = cnOkozkEHlRkUdUgtKyS39675106;     cnOkozkEHlRkUdUgtKyS39675106 = cnOkozkEHlRkUdUgtKyS78732477;     cnOkozkEHlRkUdUgtKyS78732477 = cnOkozkEHlRkUdUgtKyS28468547;     cnOkozkEHlRkUdUgtKyS28468547 = cnOkozkEHlRkUdUgtKyS7450293;     cnOkozkEHlRkUdUgtKyS7450293 = cnOkozkEHlRkUdUgtKyS61652527;     cnOkozkEHlRkUdUgtKyS61652527 = cnOkozkEHlRkUdUgtKyS2736030;     cnOkozkEHlRkUdUgtKyS2736030 = cnOkozkEHlRkUdUgtKyS69225892;     cnOkozkEHlRkUdUgtKyS69225892 = cnOkozkEHlRkUdUgtKyS85789276;     cnOkozkEHlRkUdUgtKyS85789276 = cnOkozkEHlRkUdUgtKyS53376139;     cnOkozkEHlRkUdUgtKyS53376139 = cnOkozkEHlRkUdUgtKyS14617326;     cnOkozkEHlRkUdUgtKyS14617326 = cnOkozkEHlRkUdUgtKyS45009923;     cnOkozkEHlRkUdUgtKyS45009923 = cnOkozkEHlRkUdUgtKyS56373780;     cnOkozkEHlRkUdUgtKyS56373780 = cnOkozkEHlRkUdUgtKyS24328147;     cnOkozkEHlRkUdUgtKyS24328147 = cnOkozkEHlRkUdUgtKyS7366909;     cnOkozkEHlRkUdUgtKyS7366909 = cnOkozkEHlRkUdUgtKyS46825910;     cnOkozkEHlRkUdUgtKyS46825910 = cnOkozkEHlRkUdUgtKyS83872410;     cnOkozkEHlRkUdUgtKyS83872410 = cnOkozkEHlRkUdUgtKyS30311339;     cnOkozkEHlRkUdUgtKyS30311339 = cnOkozkEHlRkUdUgtKyS73431750;     cnOkozkEHlRkUdUgtKyS73431750 = cnOkozkEHlRkUdUgtKyS35953385;     cnOkozkEHlRkUdUgtKyS35953385 = cnOkozkEHlRkUdUgtKyS639546;     cnOkozkEHlRkUdUgtKyS639546 = cnOkozkEHlRkUdUgtKyS5576379;     cnOkozkEHlRkUdUgtKyS5576379 = cnOkozkEHlRkUdUgtKyS45682560;     cnOkozkEHlRkUdUgtKyS45682560 = cnOkozkEHlRkUdUgtKyS91315365;     cnOkozkEHlRkUdUgtKyS91315365 = cnOkozkEHlRkUdUgtKyS76449631;     cnOkozkEHlRkUdUgtKyS76449631 = cnOkozkEHlRkUdUgtKyS62320210;     cnOkozkEHlRkUdUgtKyS62320210 = cnOkozkEHlRkUdUgtKyS27269251;     cnOkozkEHlRkUdUgtKyS27269251 = cnOkozkEHlRkUdUgtKyS66321438;     cnOkozkEHlRkUdUgtKyS66321438 = cnOkozkEHlRkUdUgtKyS13296040;     cnOkozkEHlRkUdUgtKyS13296040 = cnOkozkEHlRkUdUgtKyS79970313;     cnOkozkEHlRkUdUgtKyS79970313 = cnOkozkEHlRkUdUgtKyS82973165;     cnOkozkEHlRkUdUgtKyS82973165 = cnOkozkEHlRkUdUgtKyS10112090;     cnOkozkEHlRkUdUgtKyS10112090 = cnOkozkEHlRkUdUgtKyS30710568;     cnOkozkEHlRkUdUgtKyS30710568 = cnOkozkEHlRkUdUgtKyS20460246;     cnOkozkEHlRkUdUgtKyS20460246 = cnOkozkEHlRkUdUgtKyS34438575;     cnOkozkEHlRkUdUgtKyS34438575 = cnOkozkEHlRkUdUgtKyS57979072;     cnOkozkEHlRkUdUgtKyS57979072 = cnOkozkEHlRkUdUgtKyS9115467;     cnOkozkEHlRkUdUgtKyS9115467 = cnOkozkEHlRkUdUgtKyS70487817;     cnOkozkEHlRkUdUgtKyS70487817 = cnOkozkEHlRkUdUgtKyS68696416;     cnOkozkEHlRkUdUgtKyS68696416 = cnOkozkEHlRkUdUgtKyS21358405;     cnOkozkEHlRkUdUgtKyS21358405 = cnOkozkEHlRkUdUgtKyS70651684;     cnOkozkEHlRkUdUgtKyS70651684 = cnOkozkEHlRkUdUgtKyS24698032;     cnOkozkEHlRkUdUgtKyS24698032 = cnOkozkEHlRkUdUgtKyS93038789;     cnOkozkEHlRkUdUgtKyS93038789 = cnOkozkEHlRkUdUgtKyS87554922;     cnOkozkEHlRkUdUgtKyS87554922 = cnOkozkEHlRkUdUgtKyS19438520;     cnOkozkEHlRkUdUgtKyS19438520 = cnOkozkEHlRkUdUgtKyS22077957;     cnOkozkEHlRkUdUgtKyS22077957 = cnOkozkEHlRkUdUgtKyS15995196;     cnOkozkEHlRkUdUgtKyS15995196 = cnOkozkEHlRkUdUgtKyS57348421;     cnOkozkEHlRkUdUgtKyS57348421 = cnOkozkEHlRkUdUgtKyS93178493;     cnOkozkEHlRkUdUgtKyS93178493 = cnOkozkEHlRkUdUgtKyS13466979;     cnOkozkEHlRkUdUgtKyS13466979 = cnOkozkEHlRkUdUgtKyS43184975;     cnOkozkEHlRkUdUgtKyS43184975 = cnOkozkEHlRkUdUgtKyS91293820;     cnOkozkEHlRkUdUgtKyS91293820 = cnOkozkEHlRkUdUgtKyS53302259;     cnOkozkEHlRkUdUgtKyS53302259 = cnOkozkEHlRkUdUgtKyS25565657;     cnOkozkEHlRkUdUgtKyS25565657 = cnOkozkEHlRkUdUgtKyS77062549;     cnOkozkEHlRkUdUgtKyS77062549 = cnOkozkEHlRkUdUgtKyS77924720;     cnOkozkEHlRkUdUgtKyS77924720 = cnOkozkEHlRkUdUgtKyS93992546;     cnOkozkEHlRkUdUgtKyS93992546 = cnOkozkEHlRkUdUgtKyS87417111;     cnOkozkEHlRkUdUgtKyS87417111 = cnOkozkEHlRkUdUgtKyS52018916;     cnOkozkEHlRkUdUgtKyS52018916 = cnOkozkEHlRkUdUgtKyS45130082;     cnOkozkEHlRkUdUgtKyS45130082 = cnOkozkEHlRkUdUgtKyS34383276;     cnOkozkEHlRkUdUgtKyS34383276 = cnOkozkEHlRkUdUgtKyS36414591;     cnOkozkEHlRkUdUgtKyS36414591 = cnOkozkEHlRkUdUgtKyS55929852;     cnOkozkEHlRkUdUgtKyS55929852 = cnOkozkEHlRkUdUgtKyS5818964;     cnOkozkEHlRkUdUgtKyS5818964 = cnOkozkEHlRkUdUgtKyS70402973;     cnOkozkEHlRkUdUgtKyS70402973 = cnOkozkEHlRkUdUgtKyS4505237;     cnOkozkEHlRkUdUgtKyS4505237 = cnOkozkEHlRkUdUgtKyS14299355;     cnOkozkEHlRkUdUgtKyS14299355 = cnOkozkEHlRkUdUgtKyS35913535;     cnOkozkEHlRkUdUgtKyS35913535 = cnOkozkEHlRkUdUgtKyS89889571;     cnOkozkEHlRkUdUgtKyS89889571 = cnOkozkEHlRkUdUgtKyS49387837;     cnOkozkEHlRkUdUgtKyS49387837 = cnOkozkEHlRkUdUgtKyS37710443;     cnOkozkEHlRkUdUgtKyS37710443 = cnOkozkEHlRkUdUgtKyS13384594;     cnOkozkEHlRkUdUgtKyS13384594 = cnOkozkEHlRkUdUgtKyS61614923;     cnOkozkEHlRkUdUgtKyS61614923 = cnOkozkEHlRkUdUgtKyS52073346;     cnOkozkEHlRkUdUgtKyS52073346 = cnOkozkEHlRkUdUgtKyS65301701;     cnOkozkEHlRkUdUgtKyS65301701 = cnOkozkEHlRkUdUgtKyS75941513;     cnOkozkEHlRkUdUgtKyS75941513 = cnOkozkEHlRkUdUgtKyS12537589;     cnOkozkEHlRkUdUgtKyS12537589 = cnOkozkEHlRkUdUgtKyS58127638;     cnOkozkEHlRkUdUgtKyS58127638 = cnOkozkEHlRkUdUgtKyS71876846;     cnOkozkEHlRkUdUgtKyS71876846 = cnOkozkEHlRkUdUgtKyS54371674;     cnOkozkEHlRkUdUgtKyS54371674 = cnOkozkEHlRkUdUgtKyS46325015;     cnOkozkEHlRkUdUgtKyS46325015 = cnOkozkEHlRkUdUgtKyS69920830;     cnOkozkEHlRkUdUgtKyS69920830 = cnOkozkEHlRkUdUgtKyS73142944;     cnOkozkEHlRkUdUgtKyS73142944 = cnOkozkEHlRkUdUgtKyS99829060;     cnOkozkEHlRkUdUgtKyS99829060 = cnOkozkEHlRkUdUgtKyS36785339;     cnOkozkEHlRkUdUgtKyS36785339 = cnOkozkEHlRkUdUgtKyS91679345;     cnOkozkEHlRkUdUgtKyS91679345 = cnOkozkEHlRkUdUgtKyS56809830;     cnOkozkEHlRkUdUgtKyS56809830 = cnOkozkEHlRkUdUgtKyS5144912;     cnOkozkEHlRkUdUgtKyS5144912 = cnOkozkEHlRkUdUgtKyS43397697;     cnOkozkEHlRkUdUgtKyS43397697 = cnOkozkEHlRkUdUgtKyS56513855;     cnOkozkEHlRkUdUgtKyS56513855 = cnOkozkEHlRkUdUgtKyS63986525;     cnOkozkEHlRkUdUgtKyS63986525 = cnOkozkEHlRkUdUgtKyS21698355;     cnOkozkEHlRkUdUgtKyS21698355 = cnOkozkEHlRkUdUgtKyS18468901;     cnOkozkEHlRkUdUgtKyS18468901 = cnOkozkEHlRkUdUgtKyS23566334;     cnOkozkEHlRkUdUgtKyS23566334 = cnOkozkEHlRkUdUgtKyS86975128;     cnOkozkEHlRkUdUgtKyS86975128 = cnOkozkEHlRkUdUgtKyS34237094;     cnOkozkEHlRkUdUgtKyS34237094 = cnOkozkEHlRkUdUgtKyS21605160;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IobjLwExdnFrwKngqRHN81473598() {     float doVSgKUomQkxSSzDOtCM92256826 = -861699156;    float doVSgKUomQkxSSzDOtCM95407526 = 87774779;    float doVSgKUomQkxSSzDOtCM41487961 = -153623022;    float doVSgKUomQkxSSzDOtCM17633382 = 74127721;    float doVSgKUomQkxSSzDOtCM4951421 = -697841724;    float doVSgKUomQkxSSzDOtCM29674869 = -920682881;    float doVSgKUomQkxSSzDOtCM97111373 = -256728366;    float doVSgKUomQkxSSzDOtCM85310927 = -865119869;    float doVSgKUomQkxSSzDOtCM53315347 = -260025436;    float doVSgKUomQkxSSzDOtCM84618987 = -973291157;    float doVSgKUomQkxSSzDOtCM44985190 = -708434059;    float doVSgKUomQkxSSzDOtCM14772463 = -202245750;    float doVSgKUomQkxSSzDOtCM65616664 = -246633545;    float doVSgKUomQkxSSzDOtCM37223173 = -788696819;    float doVSgKUomQkxSSzDOtCM97337345 = -430491559;    float doVSgKUomQkxSSzDOtCM77538563 = -175469588;    float doVSgKUomQkxSSzDOtCM50226607 = -738242211;    float doVSgKUomQkxSSzDOtCM44621641 = 84953471;    float doVSgKUomQkxSSzDOtCM89074690 = -620332200;    float doVSgKUomQkxSSzDOtCM5612619 = -833279399;    float doVSgKUomQkxSSzDOtCM39008165 = -212691030;    float doVSgKUomQkxSSzDOtCM91707104 = -714176595;    float doVSgKUomQkxSSzDOtCM75375559 = -808473267;    float doVSgKUomQkxSSzDOtCM69802922 = -826441000;    float doVSgKUomQkxSSzDOtCM64763934 = -151255218;    float doVSgKUomQkxSSzDOtCM36173959 = -997585566;    float doVSgKUomQkxSSzDOtCM53943345 = -377503555;    float doVSgKUomQkxSSzDOtCM99097591 = -37990883;    float doVSgKUomQkxSSzDOtCM8592119 = -354943784;    float doVSgKUomQkxSSzDOtCM1492307 = -282262095;    float doVSgKUomQkxSSzDOtCM34341248 = -804361070;    float doVSgKUomQkxSSzDOtCM10832899 = -909762677;    float doVSgKUomQkxSSzDOtCM96241400 = -230881219;    float doVSgKUomQkxSSzDOtCM45335288 = -221819007;    float doVSgKUomQkxSSzDOtCM60997402 = -768040105;    float doVSgKUomQkxSSzDOtCM70105348 = -800731147;    float doVSgKUomQkxSSzDOtCM74502054 = -737860631;    float doVSgKUomQkxSSzDOtCM73252734 = -157733908;    float doVSgKUomQkxSSzDOtCM12971864 = -443204850;    float doVSgKUomQkxSSzDOtCM53082370 = -888931472;    float doVSgKUomQkxSSzDOtCM69188750 = -256937094;    float doVSgKUomQkxSSzDOtCM98970808 = -446513901;    float doVSgKUomQkxSSzDOtCM17663376 = -721956280;    float doVSgKUomQkxSSzDOtCM27866705 = -995140443;    float doVSgKUomQkxSSzDOtCM66605743 = -626641470;    float doVSgKUomQkxSSzDOtCM31103867 = -146947067;    float doVSgKUomQkxSSzDOtCM19169992 = -622379287;    float doVSgKUomQkxSSzDOtCM53169132 = -987087026;    float doVSgKUomQkxSSzDOtCM24879972 = 16629988;    float doVSgKUomQkxSSzDOtCM30505444 = -973671102;    float doVSgKUomQkxSSzDOtCM84184520 = -478833141;    float doVSgKUomQkxSSzDOtCM99219925 = -740181480;    float doVSgKUomQkxSSzDOtCM48254151 = -655424115;    float doVSgKUomQkxSSzDOtCM3694284 = -313241989;    float doVSgKUomQkxSSzDOtCM42465433 = -330320996;    float doVSgKUomQkxSSzDOtCM549723 = -47522561;    float doVSgKUomQkxSSzDOtCM20031968 = -103751955;    float doVSgKUomQkxSSzDOtCM71685038 = -327182023;    float doVSgKUomQkxSSzDOtCM52869447 = -774617062;    float doVSgKUomQkxSSzDOtCM68777461 = -700256158;    float doVSgKUomQkxSSzDOtCM75731523 = -443179326;    float doVSgKUomQkxSSzDOtCM98013782 = -118737483;    float doVSgKUomQkxSSzDOtCM76718809 = -410176085;    float doVSgKUomQkxSSzDOtCM51823041 = -977763342;    float doVSgKUomQkxSSzDOtCM50277740 = -68930087;    float doVSgKUomQkxSSzDOtCM34152291 = -798671382;    float doVSgKUomQkxSSzDOtCM18531063 = -971364531;    float doVSgKUomQkxSSzDOtCM20281376 = 75185461;    float doVSgKUomQkxSSzDOtCM76225770 = 79343286;    float doVSgKUomQkxSSzDOtCM27231997 = -629760413;    float doVSgKUomQkxSSzDOtCM3036510 = -437608957;    float doVSgKUomQkxSSzDOtCM76973873 = -480508304;    float doVSgKUomQkxSSzDOtCM31649777 = -471841679;    float doVSgKUomQkxSSzDOtCM35992321 = -731400729;    float doVSgKUomQkxSSzDOtCM36423869 = -476342306;    float doVSgKUomQkxSSzDOtCM40037357 = -766177129;    float doVSgKUomQkxSSzDOtCM74043728 = -992220316;    float doVSgKUomQkxSSzDOtCM47508854 = -813332824;    float doVSgKUomQkxSSzDOtCM3197179 = -99799530;    float doVSgKUomQkxSSzDOtCM33660067 = 95691849;    float doVSgKUomQkxSSzDOtCM17003967 = -275206280;    float doVSgKUomQkxSSzDOtCM774214 = -390416529;    float doVSgKUomQkxSSzDOtCM74217619 = 45379128;    float doVSgKUomQkxSSzDOtCM78086675 = -381272683;    float doVSgKUomQkxSSzDOtCM17307787 = -803428955;    float doVSgKUomQkxSSzDOtCM35121322 = 35820410;    float doVSgKUomQkxSSzDOtCM62578748 = -154338563;    float doVSgKUomQkxSSzDOtCM92547116 = -917639231;    float doVSgKUomQkxSSzDOtCM2869855 = -891498011;    float doVSgKUomQkxSSzDOtCM60447679 = -620517544;    float doVSgKUomQkxSSzDOtCM50073380 = -596979193;    float doVSgKUomQkxSSzDOtCM2817016 = -310678609;    float doVSgKUomQkxSSzDOtCM20383287 = -383116846;    float doVSgKUomQkxSSzDOtCM44194403 = -742948692;    float doVSgKUomQkxSSzDOtCM77350846 = -345752146;    float doVSgKUomQkxSSzDOtCM71174968 = -38199612;    float doVSgKUomQkxSSzDOtCM22252000 = 63662184;    float doVSgKUomQkxSSzDOtCM65840335 = -744192939;    float doVSgKUomQkxSSzDOtCM77588965 = -826210356;    float doVSgKUomQkxSSzDOtCM32453452 = -861699156;     doVSgKUomQkxSSzDOtCM92256826 = doVSgKUomQkxSSzDOtCM95407526;     doVSgKUomQkxSSzDOtCM95407526 = doVSgKUomQkxSSzDOtCM41487961;     doVSgKUomQkxSSzDOtCM41487961 = doVSgKUomQkxSSzDOtCM17633382;     doVSgKUomQkxSSzDOtCM17633382 = doVSgKUomQkxSSzDOtCM4951421;     doVSgKUomQkxSSzDOtCM4951421 = doVSgKUomQkxSSzDOtCM29674869;     doVSgKUomQkxSSzDOtCM29674869 = doVSgKUomQkxSSzDOtCM97111373;     doVSgKUomQkxSSzDOtCM97111373 = doVSgKUomQkxSSzDOtCM85310927;     doVSgKUomQkxSSzDOtCM85310927 = doVSgKUomQkxSSzDOtCM53315347;     doVSgKUomQkxSSzDOtCM53315347 = doVSgKUomQkxSSzDOtCM84618987;     doVSgKUomQkxSSzDOtCM84618987 = doVSgKUomQkxSSzDOtCM44985190;     doVSgKUomQkxSSzDOtCM44985190 = doVSgKUomQkxSSzDOtCM14772463;     doVSgKUomQkxSSzDOtCM14772463 = doVSgKUomQkxSSzDOtCM65616664;     doVSgKUomQkxSSzDOtCM65616664 = doVSgKUomQkxSSzDOtCM37223173;     doVSgKUomQkxSSzDOtCM37223173 = doVSgKUomQkxSSzDOtCM97337345;     doVSgKUomQkxSSzDOtCM97337345 = doVSgKUomQkxSSzDOtCM77538563;     doVSgKUomQkxSSzDOtCM77538563 = doVSgKUomQkxSSzDOtCM50226607;     doVSgKUomQkxSSzDOtCM50226607 = doVSgKUomQkxSSzDOtCM44621641;     doVSgKUomQkxSSzDOtCM44621641 = doVSgKUomQkxSSzDOtCM89074690;     doVSgKUomQkxSSzDOtCM89074690 = doVSgKUomQkxSSzDOtCM5612619;     doVSgKUomQkxSSzDOtCM5612619 = doVSgKUomQkxSSzDOtCM39008165;     doVSgKUomQkxSSzDOtCM39008165 = doVSgKUomQkxSSzDOtCM91707104;     doVSgKUomQkxSSzDOtCM91707104 = doVSgKUomQkxSSzDOtCM75375559;     doVSgKUomQkxSSzDOtCM75375559 = doVSgKUomQkxSSzDOtCM69802922;     doVSgKUomQkxSSzDOtCM69802922 = doVSgKUomQkxSSzDOtCM64763934;     doVSgKUomQkxSSzDOtCM64763934 = doVSgKUomQkxSSzDOtCM36173959;     doVSgKUomQkxSSzDOtCM36173959 = doVSgKUomQkxSSzDOtCM53943345;     doVSgKUomQkxSSzDOtCM53943345 = doVSgKUomQkxSSzDOtCM99097591;     doVSgKUomQkxSSzDOtCM99097591 = doVSgKUomQkxSSzDOtCM8592119;     doVSgKUomQkxSSzDOtCM8592119 = doVSgKUomQkxSSzDOtCM1492307;     doVSgKUomQkxSSzDOtCM1492307 = doVSgKUomQkxSSzDOtCM34341248;     doVSgKUomQkxSSzDOtCM34341248 = doVSgKUomQkxSSzDOtCM10832899;     doVSgKUomQkxSSzDOtCM10832899 = doVSgKUomQkxSSzDOtCM96241400;     doVSgKUomQkxSSzDOtCM96241400 = doVSgKUomQkxSSzDOtCM45335288;     doVSgKUomQkxSSzDOtCM45335288 = doVSgKUomQkxSSzDOtCM60997402;     doVSgKUomQkxSSzDOtCM60997402 = doVSgKUomQkxSSzDOtCM70105348;     doVSgKUomQkxSSzDOtCM70105348 = doVSgKUomQkxSSzDOtCM74502054;     doVSgKUomQkxSSzDOtCM74502054 = doVSgKUomQkxSSzDOtCM73252734;     doVSgKUomQkxSSzDOtCM73252734 = doVSgKUomQkxSSzDOtCM12971864;     doVSgKUomQkxSSzDOtCM12971864 = doVSgKUomQkxSSzDOtCM53082370;     doVSgKUomQkxSSzDOtCM53082370 = doVSgKUomQkxSSzDOtCM69188750;     doVSgKUomQkxSSzDOtCM69188750 = doVSgKUomQkxSSzDOtCM98970808;     doVSgKUomQkxSSzDOtCM98970808 = doVSgKUomQkxSSzDOtCM17663376;     doVSgKUomQkxSSzDOtCM17663376 = doVSgKUomQkxSSzDOtCM27866705;     doVSgKUomQkxSSzDOtCM27866705 = doVSgKUomQkxSSzDOtCM66605743;     doVSgKUomQkxSSzDOtCM66605743 = doVSgKUomQkxSSzDOtCM31103867;     doVSgKUomQkxSSzDOtCM31103867 = doVSgKUomQkxSSzDOtCM19169992;     doVSgKUomQkxSSzDOtCM19169992 = doVSgKUomQkxSSzDOtCM53169132;     doVSgKUomQkxSSzDOtCM53169132 = doVSgKUomQkxSSzDOtCM24879972;     doVSgKUomQkxSSzDOtCM24879972 = doVSgKUomQkxSSzDOtCM30505444;     doVSgKUomQkxSSzDOtCM30505444 = doVSgKUomQkxSSzDOtCM84184520;     doVSgKUomQkxSSzDOtCM84184520 = doVSgKUomQkxSSzDOtCM99219925;     doVSgKUomQkxSSzDOtCM99219925 = doVSgKUomQkxSSzDOtCM48254151;     doVSgKUomQkxSSzDOtCM48254151 = doVSgKUomQkxSSzDOtCM3694284;     doVSgKUomQkxSSzDOtCM3694284 = doVSgKUomQkxSSzDOtCM42465433;     doVSgKUomQkxSSzDOtCM42465433 = doVSgKUomQkxSSzDOtCM549723;     doVSgKUomQkxSSzDOtCM549723 = doVSgKUomQkxSSzDOtCM20031968;     doVSgKUomQkxSSzDOtCM20031968 = doVSgKUomQkxSSzDOtCM71685038;     doVSgKUomQkxSSzDOtCM71685038 = doVSgKUomQkxSSzDOtCM52869447;     doVSgKUomQkxSSzDOtCM52869447 = doVSgKUomQkxSSzDOtCM68777461;     doVSgKUomQkxSSzDOtCM68777461 = doVSgKUomQkxSSzDOtCM75731523;     doVSgKUomQkxSSzDOtCM75731523 = doVSgKUomQkxSSzDOtCM98013782;     doVSgKUomQkxSSzDOtCM98013782 = doVSgKUomQkxSSzDOtCM76718809;     doVSgKUomQkxSSzDOtCM76718809 = doVSgKUomQkxSSzDOtCM51823041;     doVSgKUomQkxSSzDOtCM51823041 = doVSgKUomQkxSSzDOtCM50277740;     doVSgKUomQkxSSzDOtCM50277740 = doVSgKUomQkxSSzDOtCM34152291;     doVSgKUomQkxSSzDOtCM34152291 = doVSgKUomQkxSSzDOtCM18531063;     doVSgKUomQkxSSzDOtCM18531063 = doVSgKUomQkxSSzDOtCM20281376;     doVSgKUomQkxSSzDOtCM20281376 = doVSgKUomQkxSSzDOtCM76225770;     doVSgKUomQkxSSzDOtCM76225770 = doVSgKUomQkxSSzDOtCM27231997;     doVSgKUomQkxSSzDOtCM27231997 = doVSgKUomQkxSSzDOtCM3036510;     doVSgKUomQkxSSzDOtCM3036510 = doVSgKUomQkxSSzDOtCM76973873;     doVSgKUomQkxSSzDOtCM76973873 = doVSgKUomQkxSSzDOtCM31649777;     doVSgKUomQkxSSzDOtCM31649777 = doVSgKUomQkxSSzDOtCM35992321;     doVSgKUomQkxSSzDOtCM35992321 = doVSgKUomQkxSSzDOtCM36423869;     doVSgKUomQkxSSzDOtCM36423869 = doVSgKUomQkxSSzDOtCM40037357;     doVSgKUomQkxSSzDOtCM40037357 = doVSgKUomQkxSSzDOtCM74043728;     doVSgKUomQkxSSzDOtCM74043728 = doVSgKUomQkxSSzDOtCM47508854;     doVSgKUomQkxSSzDOtCM47508854 = doVSgKUomQkxSSzDOtCM3197179;     doVSgKUomQkxSSzDOtCM3197179 = doVSgKUomQkxSSzDOtCM33660067;     doVSgKUomQkxSSzDOtCM33660067 = doVSgKUomQkxSSzDOtCM17003967;     doVSgKUomQkxSSzDOtCM17003967 = doVSgKUomQkxSSzDOtCM774214;     doVSgKUomQkxSSzDOtCM774214 = doVSgKUomQkxSSzDOtCM74217619;     doVSgKUomQkxSSzDOtCM74217619 = doVSgKUomQkxSSzDOtCM78086675;     doVSgKUomQkxSSzDOtCM78086675 = doVSgKUomQkxSSzDOtCM17307787;     doVSgKUomQkxSSzDOtCM17307787 = doVSgKUomQkxSSzDOtCM35121322;     doVSgKUomQkxSSzDOtCM35121322 = doVSgKUomQkxSSzDOtCM62578748;     doVSgKUomQkxSSzDOtCM62578748 = doVSgKUomQkxSSzDOtCM92547116;     doVSgKUomQkxSSzDOtCM92547116 = doVSgKUomQkxSSzDOtCM2869855;     doVSgKUomQkxSSzDOtCM2869855 = doVSgKUomQkxSSzDOtCM60447679;     doVSgKUomQkxSSzDOtCM60447679 = doVSgKUomQkxSSzDOtCM50073380;     doVSgKUomQkxSSzDOtCM50073380 = doVSgKUomQkxSSzDOtCM2817016;     doVSgKUomQkxSSzDOtCM2817016 = doVSgKUomQkxSSzDOtCM20383287;     doVSgKUomQkxSSzDOtCM20383287 = doVSgKUomQkxSSzDOtCM44194403;     doVSgKUomQkxSSzDOtCM44194403 = doVSgKUomQkxSSzDOtCM77350846;     doVSgKUomQkxSSzDOtCM77350846 = doVSgKUomQkxSSzDOtCM71174968;     doVSgKUomQkxSSzDOtCM71174968 = doVSgKUomQkxSSzDOtCM22252000;     doVSgKUomQkxSSzDOtCM22252000 = doVSgKUomQkxSSzDOtCM65840335;     doVSgKUomQkxSSzDOtCM65840335 = doVSgKUomQkxSSzDOtCM77588965;     doVSgKUomQkxSSzDOtCM77588965 = doVSgKUomQkxSSzDOtCM32453452;     doVSgKUomQkxSSzDOtCM32453452 = doVSgKUomQkxSSzDOtCM92256826;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void sAgRrSvAVqHTxqzOropH88132342() {     float REowDqVekckUtJNCIYdG92250355 = -734223297;    float REowDqVekckUtJNCIYdG33979843 = 45179654;    float REowDqVekckUtJNCIYdG42801173 = -110751636;    float REowDqVekckUtJNCIYdG2713243 = -846413109;    float REowDqVekckUtJNCIYdG13791523 = -66197045;    float REowDqVekckUtJNCIYdG64480301 = -620115693;    float REowDqVekckUtJNCIYdG65658709 = 38735167;    float REowDqVekckUtJNCIYdG54524978 = -569297186;    float REowDqVekckUtJNCIYdG36055511 = 26328494;    float REowDqVekckUtJNCIYdG81258501 = -610098028;    float REowDqVekckUtJNCIYdG65305795 = -667906131;    float REowDqVekckUtJNCIYdG29064140 = 62275888;    float REowDqVekckUtJNCIYdG97469577 = -268292020;    float REowDqVekckUtJNCIYdG91067392 = -33849870;    float REowDqVekckUtJNCIYdG42509454 = -393204411;    float REowDqVekckUtJNCIYdG52103110 = -508265239;    float REowDqVekckUtJNCIYdG6079367 = -557856781;    float REowDqVekckUtJNCIYdG67622174 = -312118151;    float REowDqVekckUtJNCIYdG9502961 = -138314467;    float REowDqVekckUtJNCIYdG77686382 = -345303851;    float REowDqVekckUtJNCIYdG18677589 = 76842597;    float REowDqVekckUtJNCIYdG70003370 = -124168478;    float REowDqVekckUtJNCIYdG12253956 = -167431109;    float REowDqVekckUtJNCIYdG13453031 = -707361523;    float REowDqVekckUtJNCIYdG93206623 = -577949071;    float REowDqVekckUtJNCIYdG52748036 = -905791495;    float REowDqVekckUtJNCIYdG24164927 = -678303268;    float REowDqVekckUtJNCIYdG34501116 = -646957872;    float REowDqVekckUtJNCIYdG21270566 = 98985901;    float REowDqVekckUtJNCIYdG77582176 = 42306531;    float REowDqVekckUtJNCIYdG67753926 = -91454305;    float REowDqVekckUtJNCIYdG95556756 = -201584439;    float REowDqVekckUtJNCIYdG81693823 = -432825435;    float REowDqVekckUtJNCIYdG45618125 = -98500739;    float REowDqVekckUtJNCIYdG43355440 = 72645970;    float REowDqVekckUtJNCIYdG12933538 = -880496893;    float REowDqVekckUtJNCIYdG86443421 = -837514010;    float REowDqVekckUtJNCIYdG43170414 = -152996587;    float REowDqVekckUtJNCIYdG48285461 = -55886266;    float REowDqVekckUtJNCIYdG24955263 = -498363158;    float REowDqVekckUtJNCIYdG57044418 = -344880993;    float REowDqVekckUtJNCIYdG84668469 = -640305983;    float REowDqVekckUtJNCIYdG77946736 = -181086534;    float REowDqVekckUtJNCIYdG87098603 = -552104887;    float REowDqVekckUtJNCIYdG50623796 = 99128;    float REowDqVekckUtJNCIYdG71531835 = 41008262;    float REowDqVekckUtJNCIYdG46867785 = -3659909;    float REowDqVekckUtJNCIYdG28552713 = -495848383;    float REowDqVekckUtJNCIYdG43538361 = -647133873;    float REowDqVekckUtJNCIYdG50550867 = -560695988;    float REowDqVekckUtJNCIYdG97443889 = -57388148;    float REowDqVekckUtJNCIYdG13767183 = -22327228;    float REowDqVekckUtJNCIYdG23702499 = -547478123;    float REowDqVekckUtJNCIYdG13944555 = -845161840;    float REowDqVekckUtJNCIYdG69454754 = -87220274;    float REowDqVekckUtJNCIYdG22246986 = -510054820;    float REowDqVekckUtJNCIYdG21725888 = -787389238;    float REowDqVekckUtJNCIYdG29348142 = -403390114;    float REowDqVekckUtJNCIYdG9506619 = -168464039;    float REowDqVekckUtJNCIYdG61043486 = -160405551;    float REowDqVekckUtJNCIYdG40315374 = -941812425;    float REowDqVekckUtJNCIYdG31157593 = -314306962;    float REowDqVekckUtJNCIYdG33254413 = -568283088;    float REowDqVekckUtJNCIYdG58473335 = 84021962;    float REowDqVekckUtJNCIYdG13504575 = -418643724;    float REowDqVekckUtJNCIYdG69749039 = -366321693;    float REowDqVekckUtJNCIYdG47370317 = -504898678;    float REowDqVekckUtJNCIYdG51851453 = -69791281;    float REowDqVekckUtJNCIYdG47711952 = -6495841;    float REowDqVekckUtJNCIYdG29575917 = -512707519;    float REowDqVekckUtJNCIYdG65659689 = -670751230;    float REowDqVekckUtJNCIYdG62908953 = -304860195;    float REowDqVekckUtJNCIYdG19336714 = -156231885;    float REowDqVekckUtJNCIYdG84547697 = -639951309;    float REowDqVekckUtJNCIYdG20641964 = 99577141;    float REowDqVekckUtJNCIYdG34009120 = -282851421;    float REowDqVekckUtJNCIYdG92056633 = -943081945;    float REowDqVekckUtJNCIYdG25155353 = -615326223;    float REowDqVekckUtJNCIYdG62829235 = -607460651;    float REowDqVekckUtJNCIYdG21674789 = -518957334;    float REowDqVekckUtJNCIYdG5880252 = -802131586;    float REowDqVekckUtJNCIYdG95612214 = -82454886;    float REowDqVekckUtJNCIYdG90962755 = -999823999;    float REowDqVekckUtJNCIYdG70719699 = -340318111;    float REowDqVekckUtJNCIYdG80138286 = -900305321;    float REowDqVekckUtJNCIYdG53986743 = 30872923;    float REowDqVekckUtJNCIYdG71854257 = -654106316;    float REowDqVekckUtJNCIYdG67749269 = -587663596;    float REowDqVekckUtJNCIYdG76163371 = 88719534;    float REowDqVekckUtJNCIYdG21108455 = -417299211;    float REowDqVekckUtJNCIYdG91207650 = 6892344;    float REowDqVekckUtJNCIYdG57095279 = -334123896;    float REowDqVekckUtJNCIYdG33663795 = -984532548;    float REowDqVekckUtJNCIYdG87241974 = -895480715;    float REowDqVekckUtJNCIYdG84639888 = -556550733;    float REowDqVekckUtJNCIYdG25886825 = 69425969;    float REowDqVekckUtJNCIYdG51414057 = 27977104;    float REowDqVekckUtJNCIYdG19473401 = -165108497;    float REowDqVekckUtJNCIYdG73594028 = -33461163;    float REowDqVekckUtJNCIYdG80874756 = -734223297;     REowDqVekckUtJNCIYdG92250355 = REowDqVekckUtJNCIYdG33979843;     REowDqVekckUtJNCIYdG33979843 = REowDqVekckUtJNCIYdG42801173;     REowDqVekckUtJNCIYdG42801173 = REowDqVekckUtJNCIYdG2713243;     REowDqVekckUtJNCIYdG2713243 = REowDqVekckUtJNCIYdG13791523;     REowDqVekckUtJNCIYdG13791523 = REowDqVekckUtJNCIYdG64480301;     REowDqVekckUtJNCIYdG64480301 = REowDqVekckUtJNCIYdG65658709;     REowDqVekckUtJNCIYdG65658709 = REowDqVekckUtJNCIYdG54524978;     REowDqVekckUtJNCIYdG54524978 = REowDqVekckUtJNCIYdG36055511;     REowDqVekckUtJNCIYdG36055511 = REowDqVekckUtJNCIYdG81258501;     REowDqVekckUtJNCIYdG81258501 = REowDqVekckUtJNCIYdG65305795;     REowDqVekckUtJNCIYdG65305795 = REowDqVekckUtJNCIYdG29064140;     REowDqVekckUtJNCIYdG29064140 = REowDqVekckUtJNCIYdG97469577;     REowDqVekckUtJNCIYdG97469577 = REowDqVekckUtJNCIYdG91067392;     REowDqVekckUtJNCIYdG91067392 = REowDqVekckUtJNCIYdG42509454;     REowDqVekckUtJNCIYdG42509454 = REowDqVekckUtJNCIYdG52103110;     REowDqVekckUtJNCIYdG52103110 = REowDqVekckUtJNCIYdG6079367;     REowDqVekckUtJNCIYdG6079367 = REowDqVekckUtJNCIYdG67622174;     REowDqVekckUtJNCIYdG67622174 = REowDqVekckUtJNCIYdG9502961;     REowDqVekckUtJNCIYdG9502961 = REowDqVekckUtJNCIYdG77686382;     REowDqVekckUtJNCIYdG77686382 = REowDqVekckUtJNCIYdG18677589;     REowDqVekckUtJNCIYdG18677589 = REowDqVekckUtJNCIYdG70003370;     REowDqVekckUtJNCIYdG70003370 = REowDqVekckUtJNCIYdG12253956;     REowDqVekckUtJNCIYdG12253956 = REowDqVekckUtJNCIYdG13453031;     REowDqVekckUtJNCIYdG13453031 = REowDqVekckUtJNCIYdG93206623;     REowDqVekckUtJNCIYdG93206623 = REowDqVekckUtJNCIYdG52748036;     REowDqVekckUtJNCIYdG52748036 = REowDqVekckUtJNCIYdG24164927;     REowDqVekckUtJNCIYdG24164927 = REowDqVekckUtJNCIYdG34501116;     REowDqVekckUtJNCIYdG34501116 = REowDqVekckUtJNCIYdG21270566;     REowDqVekckUtJNCIYdG21270566 = REowDqVekckUtJNCIYdG77582176;     REowDqVekckUtJNCIYdG77582176 = REowDqVekckUtJNCIYdG67753926;     REowDqVekckUtJNCIYdG67753926 = REowDqVekckUtJNCIYdG95556756;     REowDqVekckUtJNCIYdG95556756 = REowDqVekckUtJNCIYdG81693823;     REowDqVekckUtJNCIYdG81693823 = REowDqVekckUtJNCIYdG45618125;     REowDqVekckUtJNCIYdG45618125 = REowDqVekckUtJNCIYdG43355440;     REowDqVekckUtJNCIYdG43355440 = REowDqVekckUtJNCIYdG12933538;     REowDqVekckUtJNCIYdG12933538 = REowDqVekckUtJNCIYdG86443421;     REowDqVekckUtJNCIYdG86443421 = REowDqVekckUtJNCIYdG43170414;     REowDqVekckUtJNCIYdG43170414 = REowDqVekckUtJNCIYdG48285461;     REowDqVekckUtJNCIYdG48285461 = REowDqVekckUtJNCIYdG24955263;     REowDqVekckUtJNCIYdG24955263 = REowDqVekckUtJNCIYdG57044418;     REowDqVekckUtJNCIYdG57044418 = REowDqVekckUtJNCIYdG84668469;     REowDqVekckUtJNCIYdG84668469 = REowDqVekckUtJNCIYdG77946736;     REowDqVekckUtJNCIYdG77946736 = REowDqVekckUtJNCIYdG87098603;     REowDqVekckUtJNCIYdG87098603 = REowDqVekckUtJNCIYdG50623796;     REowDqVekckUtJNCIYdG50623796 = REowDqVekckUtJNCIYdG71531835;     REowDqVekckUtJNCIYdG71531835 = REowDqVekckUtJNCIYdG46867785;     REowDqVekckUtJNCIYdG46867785 = REowDqVekckUtJNCIYdG28552713;     REowDqVekckUtJNCIYdG28552713 = REowDqVekckUtJNCIYdG43538361;     REowDqVekckUtJNCIYdG43538361 = REowDqVekckUtJNCIYdG50550867;     REowDqVekckUtJNCIYdG50550867 = REowDqVekckUtJNCIYdG97443889;     REowDqVekckUtJNCIYdG97443889 = REowDqVekckUtJNCIYdG13767183;     REowDqVekckUtJNCIYdG13767183 = REowDqVekckUtJNCIYdG23702499;     REowDqVekckUtJNCIYdG23702499 = REowDqVekckUtJNCIYdG13944555;     REowDqVekckUtJNCIYdG13944555 = REowDqVekckUtJNCIYdG69454754;     REowDqVekckUtJNCIYdG69454754 = REowDqVekckUtJNCIYdG22246986;     REowDqVekckUtJNCIYdG22246986 = REowDqVekckUtJNCIYdG21725888;     REowDqVekckUtJNCIYdG21725888 = REowDqVekckUtJNCIYdG29348142;     REowDqVekckUtJNCIYdG29348142 = REowDqVekckUtJNCIYdG9506619;     REowDqVekckUtJNCIYdG9506619 = REowDqVekckUtJNCIYdG61043486;     REowDqVekckUtJNCIYdG61043486 = REowDqVekckUtJNCIYdG40315374;     REowDqVekckUtJNCIYdG40315374 = REowDqVekckUtJNCIYdG31157593;     REowDqVekckUtJNCIYdG31157593 = REowDqVekckUtJNCIYdG33254413;     REowDqVekckUtJNCIYdG33254413 = REowDqVekckUtJNCIYdG58473335;     REowDqVekckUtJNCIYdG58473335 = REowDqVekckUtJNCIYdG13504575;     REowDqVekckUtJNCIYdG13504575 = REowDqVekckUtJNCIYdG69749039;     REowDqVekckUtJNCIYdG69749039 = REowDqVekckUtJNCIYdG47370317;     REowDqVekckUtJNCIYdG47370317 = REowDqVekckUtJNCIYdG51851453;     REowDqVekckUtJNCIYdG51851453 = REowDqVekckUtJNCIYdG47711952;     REowDqVekckUtJNCIYdG47711952 = REowDqVekckUtJNCIYdG29575917;     REowDqVekckUtJNCIYdG29575917 = REowDqVekckUtJNCIYdG65659689;     REowDqVekckUtJNCIYdG65659689 = REowDqVekckUtJNCIYdG62908953;     REowDqVekckUtJNCIYdG62908953 = REowDqVekckUtJNCIYdG19336714;     REowDqVekckUtJNCIYdG19336714 = REowDqVekckUtJNCIYdG84547697;     REowDqVekckUtJNCIYdG84547697 = REowDqVekckUtJNCIYdG20641964;     REowDqVekckUtJNCIYdG20641964 = REowDqVekckUtJNCIYdG34009120;     REowDqVekckUtJNCIYdG34009120 = REowDqVekckUtJNCIYdG92056633;     REowDqVekckUtJNCIYdG92056633 = REowDqVekckUtJNCIYdG25155353;     REowDqVekckUtJNCIYdG25155353 = REowDqVekckUtJNCIYdG62829235;     REowDqVekckUtJNCIYdG62829235 = REowDqVekckUtJNCIYdG21674789;     REowDqVekckUtJNCIYdG21674789 = REowDqVekckUtJNCIYdG5880252;     REowDqVekckUtJNCIYdG5880252 = REowDqVekckUtJNCIYdG95612214;     REowDqVekckUtJNCIYdG95612214 = REowDqVekckUtJNCIYdG90962755;     REowDqVekckUtJNCIYdG90962755 = REowDqVekckUtJNCIYdG70719699;     REowDqVekckUtJNCIYdG70719699 = REowDqVekckUtJNCIYdG80138286;     REowDqVekckUtJNCIYdG80138286 = REowDqVekckUtJNCIYdG53986743;     REowDqVekckUtJNCIYdG53986743 = REowDqVekckUtJNCIYdG71854257;     REowDqVekckUtJNCIYdG71854257 = REowDqVekckUtJNCIYdG67749269;     REowDqVekckUtJNCIYdG67749269 = REowDqVekckUtJNCIYdG76163371;     REowDqVekckUtJNCIYdG76163371 = REowDqVekckUtJNCIYdG21108455;     REowDqVekckUtJNCIYdG21108455 = REowDqVekckUtJNCIYdG91207650;     REowDqVekckUtJNCIYdG91207650 = REowDqVekckUtJNCIYdG57095279;     REowDqVekckUtJNCIYdG57095279 = REowDqVekckUtJNCIYdG33663795;     REowDqVekckUtJNCIYdG33663795 = REowDqVekckUtJNCIYdG87241974;     REowDqVekckUtJNCIYdG87241974 = REowDqVekckUtJNCIYdG84639888;     REowDqVekckUtJNCIYdG84639888 = REowDqVekckUtJNCIYdG25886825;     REowDqVekckUtJNCIYdG25886825 = REowDqVekckUtJNCIYdG51414057;     REowDqVekckUtJNCIYdG51414057 = REowDqVekckUtJNCIYdG19473401;     REowDqVekckUtJNCIYdG19473401 = REowDqVekckUtJNCIYdG73594028;     REowDqVekckUtJNCIYdG73594028 = REowDqVekckUtJNCIYdG80874756;     REowDqVekckUtJNCIYdG80874756 = REowDqVekckUtJNCIYdG92250355;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void TJeRrdDaftojrekOdxFC42548556() {     float feEZQrVMQxxNMdKfytOQ62902022 = -252372428;    float feEZQrVMQxxNMdKfytOQ2653361 = -926463646;    float feEZQrVMQxxNMdKfytOQ22770092 = -553046096;    float feEZQrVMQxxNMdKfytOQ42644529 = 63448502;    float feEZQrVMQxxNMdKfytOQ35241844 = -949603393;    float feEZQrVMQxxNMdKfytOQ54480064 = -321696844;    float feEZQrVMQxxNMdKfytOQ84037605 = 39959457;    float feEZQrVMQxxNMdKfytOQ11367359 = -916114276;    float feEZQrVMQxxNMdKfytOQ81920565 = -67018835;    float feEZQrVMQxxNMdKfytOQ4224962 = -911193378;    float feEZQrVMQxxNMdKfytOQ7554957 = -702808060;    float feEZQrVMQxxNMdKfytOQ74610711 = -253487905;    float feEZQrVMQxxNMdKfytOQ77296964 = -177829078;    float feEZQrVMQxxNMdKfytOQ74914426 = -934878080;    float feEZQrVMQxxNMdKfytOQ25229474 = -542954415;    float feEZQrVMQxxNMdKfytOQ84631750 = -78248826;    float feEZQrVMQxxNMdKfytOQ99932193 = -467041911;    float feEZQrVMQxxNMdKfytOQ87915668 = -301284347;    float feEZQrVMQxxNMdKfytOQ91210742 = -845502409;    float feEZQrVMQxxNMdKfytOQ36473091 = -887670155;    float feEZQrVMQxxNMdKfytOQ73813343 = -931836544;    float feEZQrVMQxxNMdKfytOQ31399135 = -455453784;    float feEZQrVMQxxNMdKfytOQ14197765 = -735274254;    float feEZQrVMQxxNMdKfytOQ47302568 = -299762429;    float feEZQrVMQxxNMdKfytOQ57331012 = -253853759;    float feEZQrVMQxxNMdKfytOQ83345616 = 95924060;    float feEZQrVMQxxNMdKfytOQ32425713 = 26518957;    float feEZQrVMQxxNMdKfytOQ42283341 = -52081639;    float feEZQrVMQxxNMdKfytOQ53413053 = -195372316;    float feEZQrVMQxxNMdKfytOQ16754272 = -516088539;    float feEZQrVMQxxNMdKfytOQ74825922 = -939887990;    float feEZQrVMQxxNMdKfytOQ40068217 = -529881602;    float feEZQrVMQxxNMdKfytOQ64639184 = -379787044;    float feEZQrVMQxxNMdKfytOQ10983101 = -762030424;    float feEZQrVMQxxNMdKfytOQ21379677 = -874300809;    float feEZQrVMQxxNMdKfytOQ72926796 = -959723441;    float feEZQrVMQxxNMdKfytOQ30234908 = 73892783;    float feEZQrVMQxxNMdKfytOQ95962902 = 62890306;    float feEZQrVMQxxNMdKfytOQ26818750 = -11614903;    float feEZQrVMQxxNMdKfytOQ20058561 = -15971521;    float feEZQrVMQxxNMdKfytOQ17117702 = -728899871;    float feEZQrVMQxxNMdKfytOQ13151461 = -409695053;    float feEZQrVMQxxNMdKfytOQ26913696 = -167840586;    float feEZQrVMQxxNMdKfytOQ93606903 = -624079858;    float feEZQrVMQxxNMdKfytOQ46577854 = -308547266;    float feEZQrVMQxxNMdKfytOQ77937670 = -817328893;    float feEZQrVMQxxNMdKfytOQ72998988 = -119729709;    float feEZQrVMQxxNMdKfytOQ94166922 = -947978678;    float feEZQrVMQxxNMdKfytOQ48979813 = -632897842;    float feEZQrVMQxxNMdKfytOQ58978353 = -843704867;    float feEZQrVMQxxNMdKfytOQ65633214 = -703960269;    float feEZQrVMQxxNMdKfytOQ55638687 = 8453050;    float feEZQrVMQxxNMdKfytOQ78778156 = -475276165;    float feEZQrVMQxxNMdKfytOQ4171859 = -406043337;    float feEZQrVMQxxNMdKfytOQ68735212 = -198161188;    float feEZQrVMQxxNMdKfytOQ31502888 = -796918644;    float feEZQrVMQxxNMdKfytOQ88455596 = -91189392;    float feEZQrVMQxxNMdKfytOQ75467524 = -153283668;    float feEZQrVMQxxNMdKfytOQ85313516 = -682697740;    float feEZQrVMQxxNMdKfytOQ51896227 = -945527454;    float feEZQrVMQxxNMdKfytOQ22054351 = -248215802;    float feEZQrVMQxxNMdKfytOQ41754264 = -907958904;    float feEZQrVMQxxNMdKfytOQ57954305 = -620741960;    float feEZQrVMQxxNMdKfytOQ65166293 = -550930296;    float feEZQrVMQxxNMdKfytOQ29399039 = -971305389;    float feEZQrVMQxxNMdKfytOQ67486739 = -72926459;    float feEZQrVMQxxNMdKfytOQ9971527 = -873700862;    float feEZQrVMQxxNMdKfytOQ66313864 = -415798655;    float feEZQrVMQxxNMdKfytOQ53534750 = 39422729;    float feEZQrVMQxxNMdKfytOQ52302677 = -583230974;    float feEZQrVMQxxNMdKfytOQ54396843 = -52141609;    float feEZQrVMQxxNMdKfytOQ3969291 = -429932217;    float feEZQrVMQxxNMdKfytOQ61096919 = -189669444;    float feEZQrVMQxxNMdKfytOQ71152181 = -729530889;    float feEZQrVMQxxNMdKfytOQ19355390 = -58770285;    float feEZQrVMQxxNMdKfytOQ60661883 = -422141492;    float feEZQrVMQxxNMdKfytOQ4485439 = -187613198;    float feEZQrVMQxxNMdKfytOQ20590861 = -11194396;    float feEZQrVMQxxNMdKfytOQ724714 = -991215163;    float feEZQrVMQxxNMdKfytOQ79393342 = -436524866;    float feEZQrVMQxxNMdKfytOQ10346629 = -784346231;    float feEZQrVMQxxNMdKfytOQ38258790 = -25502366;    float feEZQrVMQxxNMdKfytOQ93303528 = -419183798;    float feEZQrVMQxxNMdKfytOQ94434700 = -351667450;    float feEZQrVMQxxNMdKfytOQ51121058 = -812128271;    float feEZQrVMQxxNMdKfytOQ19187235 = -848341040;    float feEZQrVMQxxNMdKfytOQ61290061 = 45394563;    float feEZQrVMQxxNMdKfytOQ60467325 = -973743708;    float feEZQrVMQxxNMdKfytOQ42247888 = -463869236;    float feEZQrVMQxxNMdKfytOQ89876788 = 22617834;    float feEZQrVMQxxNMdKfytOQ84471200 = -768534049;    float feEZQrVMQxxNMdKfytOQ54767383 = -772823550;    float feEZQrVMQxxNMdKfytOQ10649386 = -254411954;    float feEZQrVMQxxNMdKfytOQ74922522 = -66087450;    float feEZQrVMQxxNMdKfytOQ98004209 = -767755720;    float feEZQrVMQxxNMdKfytOQ75363438 = -820940967;    float feEZQrVMQxxNMdKfytOQ55197155 = -788953093;    float feEZQrVMQxxNMdKfytOQ61747402 = -616910291;    float feEZQrVMQxxNMdKfytOQ64207864 = -652774470;    float feEZQrVMQxxNMdKfytOQ79091115 = -252372428;     feEZQrVMQxxNMdKfytOQ62902022 = feEZQrVMQxxNMdKfytOQ2653361;     feEZQrVMQxxNMdKfytOQ2653361 = feEZQrVMQxxNMdKfytOQ22770092;     feEZQrVMQxxNMdKfytOQ22770092 = feEZQrVMQxxNMdKfytOQ42644529;     feEZQrVMQxxNMdKfytOQ42644529 = feEZQrVMQxxNMdKfytOQ35241844;     feEZQrVMQxxNMdKfytOQ35241844 = feEZQrVMQxxNMdKfytOQ54480064;     feEZQrVMQxxNMdKfytOQ54480064 = feEZQrVMQxxNMdKfytOQ84037605;     feEZQrVMQxxNMdKfytOQ84037605 = feEZQrVMQxxNMdKfytOQ11367359;     feEZQrVMQxxNMdKfytOQ11367359 = feEZQrVMQxxNMdKfytOQ81920565;     feEZQrVMQxxNMdKfytOQ81920565 = feEZQrVMQxxNMdKfytOQ4224962;     feEZQrVMQxxNMdKfytOQ4224962 = feEZQrVMQxxNMdKfytOQ7554957;     feEZQrVMQxxNMdKfytOQ7554957 = feEZQrVMQxxNMdKfytOQ74610711;     feEZQrVMQxxNMdKfytOQ74610711 = feEZQrVMQxxNMdKfytOQ77296964;     feEZQrVMQxxNMdKfytOQ77296964 = feEZQrVMQxxNMdKfytOQ74914426;     feEZQrVMQxxNMdKfytOQ74914426 = feEZQrVMQxxNMdKfytOQ25229474;     feEZQrVMQxxNMdKfytOQ25229474 = feEZQrVMQxxNMdKfytOQ84631750;     feEZQrVMQxxNMdKfytOQ84631750 = feEZQrVMQxxNMdKfytOQ99932193;     feEZQrVMQxxNMdKfytOQ99932193 = feEZQrVMQxxNMdKfytOQ87915668;     feEZQrVMQxxNMdKfytOQ87915668 = feEZQrVMQxxNMdKfytOQ91210742;     feEZQrVMQxxNMdKfytOQ91210742 = feEZQrVMQxxNMdKfytOQ36473091;     feEZQrVMQxxNMdKfytOQ36473091 = feEZQrVMQxxNMdKfytOQ73813343;     feEZQrVMQxxNMdKfytOQ73813343 = feEZQrVMQxxNMdKfytOQ31399135;     feEZQrVMQxxNMdKfytOQ31399135 = feEZQrVMQxxNMdKfytOQ14197765;     feEZQrVMQxxNMdKfytOQ14197765 = feEZQrVMQxxNMdKfytOQ47302568;     feEZQrVMQxxNMdKfytOQ47302568 = feEZQrVMQxxNMdKfytOQ57331012;     feEZQrVMQxxNMdKfytOQ57331012 = feEZQrVMQxxNMdKfytOQ83345616;     feEZQrVMQxxNMdKfytOQ83345616 = feEZQrVMQxxNMdKfytOQ32425713;     feEZQrVMQxxNMdKfytOQ32425713 = feEZQrVMQxxNMdKfytOQ42283341;     feEZQrVMQxxNMdKfytOQ42283341 = feEZQrVMQxxNMdKfytOQ53413053;     feEZQrVMQxxNMdKfytOQ53413053 = feEZQrVMQxxNMdKfytOQ16754272;     feEZQrVMQxxNMdKfytOQ16754272 = feEZQrVMQxxNMdKfytOQ74825922;     feEZQrVMQxxNMdKfytOQ74825922 = feEZQrVMQxxNMdKfytOQ40068217;     feEZQrVMQxxNMdKfytOQ40068217 = feEZQrVMQxxNMdKfytOQ64639184;     feEZQrVMQxxNMdKfytOQ64639184 = feEZQrVMQxxNMdKfytOQ10983101;     feEZQrVMQxxNMdKfytOQ10983101 = feEZQrVMQxxNMdKfytOQ21379677;     feEZQrVMQxxNMdKfytOQ21379677 = feEZQrVMQxxNMdKfytOQ72926796;     feEZQrVMQxxNMdKfytOQ72926796 = feEZQrVMQxxNMdKfytOQ30234908;     feEZQrVMQxxNMdKfytOQ30234908 = feEZQrVMQxxNMdKfytOQ95962902;     feEZQrVMQxxNMdKfytOQ95962902 = feEZQrVMQxxNMdKfytOQ26818750;     feEZQrVMQxxNMdKfytOQ26818750 = feEZQrVMQxxNMdKfytOQ20058561;     feEZQrVMQxxNMdKfytOQ20058561 = feEZQrVMQxxNMdKfytOQ17117702;     feEZQrVMQxxNMdKfytOQ17117702 = feEZQrVMQxxNMdKfytOQ13151461;     feEZQrVMQxxNMdKfytOQ13151461 = feEZQrVMQxxNMdKfytOQ26913696;     feEZQrVMQxxNMdKfytOQ26913696 = feEZQrVMQxxNMdKfytOQ93606903;     feEZQrVMQxxNMdKfytOQ93606903 = feEZQrVMQxxNMdKfytOQ46577854;     feEZQrVMQxxNMdKfytOQ46577854 = feEZQrVMQxxNMdKfytOQ77937670;     feEZQrVMQxxNMdKfytOQ77937670 = feEZQrVMQxxNMdKfytOQ72998988;     feEZQrVMQxxNMdKfytOQ72998988 = feEZQrVMQxxNMdKfytOQ94166922;     feEZQrVMQxxNMdKfytOQ94166922 = feEZQrVMQxxNMdKfytOQ48979813;     feEZQrVMQxxNMdKfytOQ48979813 = feEZQrVMQxxNMdKfytOQ58978353;     feEZQrVMQxxNMdKfytOQ58978353 = feEZQrVMQxxNMdKfytOQ65633214;     feEZQrVMQxxNMdKfytOQ65633214 = feEZQrVMQxxNMdKfytOQ55638687;     feEZQrVMQxxNMdKfytOQ55638687 = feEZQrVMQxxNMdKfytOQ78778156;     feEZQrVMQxxNMdKfytOQ78778156 = feEZQrVMQxxNMdKfytOQ4171859;     feEZQrVMQxxNMdKfytOQ4171859 = feEZQrVMQxxNMdKfytOQ68735212;     feEZQrVMQxxNMdKfytOQ68735212 = feEZQrVMQxxNMdKfytOQ31502888;     feEZQrVMQxxNMdKfytOQ31502888 = feEZQrVMQxxNMdKfytOQ88455596;     feEZQrVMQxxNMdKfytOQ88455596 = feEZQrVMQxxNMdKfytOQ75467524;     feEZQrVMQxxNMdKfytOQ75467524 = feEZQrVMQxxNMdKfytOQ85313516;     feEZQrVMQxxNMdKfytOQ85313516 = feEZQrVMQxxNMdKfytOQ51896227;     feEZQrVMQxxNMdKfytOQ51896227 = feEZQrVMQxxNMdKfytOQ22054351;     feEZQrVMQxxNMdKfytOQ22054351 = feEZQrVMQxxNMdKfytOQ41754264;     feEZQrVMQxxNMdKfytOQ41754264 = feEZQrVMQxxNMdKfytOQ57954305;     feEZQrVMQxxNMdKfytOQ57954305 = feEZQrVMQxxNMdKfytOQ65166293;     feEZQrVMQxxNMdKfytOQ65166293 = feEZQrVMQxxNMdKfytOQ29399039;     feEZQrVMQxxNMdKfytOQ29399039 = feEZQrVMQxxNMdKfytOQ67486739;     feEZQrVMQxxNMdKfytOQ67486739 = feEZQrVMQxxNMdKfytOQ9971527;     feEZQrVMQxxNMdKfytOQ9971527 = feEZQrVMQxxNMdKfytOQ66313864;     feEZQrVMQxxNMdKfytOQ66313864 = feEZQrVMQxxNMdKfytOQ53534750;     feEZQrVMQxxNMdKfytOQ53534750 = feEZQrVMQxxNMdKfytOQ52302677;     feEZQrVMQxxNMdKfytOQ52302677 = feEZQrVMQxxNMdKfytOQ54396843;     feEZQrVMQxxNMdKfytOQ54396843 = feEZQrVMQxxNMdKfytOQ3969291;     feEZQrVMQxxNMdKfytOQ3969291 = feEZQrVMQxxNMdKfytOQ61096919;     feEZQrVMQxxNMdKfytOQ61096919 = feEZQrVMQxxNMdKfytOQ71152181;     feEZQrVMQxxNMdKfytOQ71152181 = feEZQrVMQxxNMdKfytOQ19355390;     feEZQrVMQxxNMdKfytOQ19355390 = feEZQrVMQxxNMdKfytOQ60661883;     feEZQrVMQxxNMdKfytOQ60661883 = feEZQrVMQxxNMdKfytOQ4485439;     feEZQrVMQxxNMdKfytOQ4485439 = feEZQrVMQxxNMdKfytOQ20590861;     feEZQrVMQxxNMdKfytOQ20590861 = feEZQrVMQxxNMdKfytOQ724714;     feEZQrVMQxxNMdKfytOQ724714 = feEZQrVMQxxNMdKfytOQ79393342;     feEZQrVMQxxNMdKfytOQ79393342 = feEZQrVMQxxNMdKfytOQ10346629;     feEZQrVMQxxNMdKfytOQ10346629 = feEZQrVMQxxNMdKfytOQ38258790;     feEZQrVMQxxNMdKfytOQ38258790 = feEZQrVMQxxNMdKfytOQ93303528;     feEZQrVMQxxNMdKfytOQ93303528 = feEZQrVMQxxNMdKfytOQ94434700;     feEZQrVMQxxNMdKfytOQ94434700 = feEZQrVMQxxNMdKfytOQ51121058;     feEZQrVMQxxNMdKfytOQ51121058 = feEZQrVMQxxNMdKfytOQ19187235;     feEZQrVMQxxNMdKfytOQ19187235 = feEZQrVMQxxNMdKfytOQ61290061;     feEZQrVMQxxNMdKfytOQ61290061 = feEZQrVMQxxNMdKfytOQ60467325;     feEZQrVMQxxNMdKfytOQ60467325 = feEZQrVMQxxNMdKfytOQ42247888;     feEZQrVMQxxNMdKfytOQ42247888 = feEZQrVMQxxNMdKfytOQ89876788;     feEZQrVMQxxNMdKfytOQ89876788 = feEZQrVMQxxNMdKfytOQ84471200;     feEZQrVMQxxNMdKfytOQ84471200 = feEZQrVMQxxNMdKfytOQ54767383;     feEZQrVMQxxNMdKfytOQ54767383 = feEZQrVMQxxNMdKfytOQ10649386;     feEZQrVMQxxNMdKfytOQ10649386 = feEZQrVMQxxNMdKfytOQ74922522;     feEZQrVMQxxNMdKfytOQ74922522 = feEZQrVMQxxNMdKfytOQ98004209;     feEZQrVMQxxNMdKfytOQ98004209 = feEZQrVMQxxNMdKfytOQ75363438;     feEZQrVMQxxNMdKfytOQ75363438 = feEZQrVMQxxNMdKfytOQ55197155;     feEZQrVMQxxNMdKfytOQ55197155 = feEZQrVMQxxNMdKfytOQ61747402;     feEZQrVMQxxNMdKfytOQ61747402 = feEZQrVMQxxNMdKfytOQ64207864;     feEZQrVMQxxNMdKfytOQ64207864 = feEZQrVMQxxNMdKfytOQ79091115;     feEZQrVMQxxNMdKfytOQ79091115 = feEZQrVMQxxNMdKfytOQ62902022;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void yrrZOZJDFOTmdUoeixqE49207301() {     float VKAxjXFJmuOvRibfsDaw62895551 = -124896569;    float VKAxjXFJmuOvRibfsDaw41225677 = -969058771;    float VKAxjXFJmuOvRibfsDaw24083304 = -510174709;    float VKAxjXFJmuOvRibfsDaw27724390 = -857092329;    float VKAxjXFJmuOvRibfsDaw44081946 = -317958714;    float VKAxjXFJmuOvRibfsDaw89285496 = -21129656;    float VKAxjXFJmuOvRibfsDaw52584941 = -764577010;    float VKAxjXFJmuOvRibfsDaw80581409 = -620291593;    float VKAxjXFJmuOvRibfsDaw64660729 = -880664905;    float VKAxjXFJmuOvRibfsDaw864476 = -548000250;    float VKAxjXFJmuOvRibfsDaw27875562 = -662280132;    float VKAxjXFJmuOvRibfsDaw88902388 = 11033732;    float VKAxjXFJmuOvRibfsDaw9149879 = -199487553;    float VKAxjXFJmuOvRibfsDaw28758647 = -180031132;    float VKAxjXFJmuOvRibfsDaw70401582 = -505667266;    float VKAxjXFJmuOvRibfsDaw59196298 = -411044476;    float VKAxjXFJmuOvRibfsDaw55784953 = -286656481;    float VKAxjXFJmuOvRibfsDaw10916201 = -698355968;    float VKAxjXFJmuOvRibfsDaw11639012 = -363484675;    float VKAxjXFJmuOvRibfsDaw8546854 = -399694607;    float VKAxjXFJmuOvRibfsDaw53482768 = -642302918;    float VKAxjXFJmuOvRibfsDaw9695401 = -965445667;    float VKAxjXFJmuOvRibfsDaw51076161 = -94232096;    float VKAxjXFJmuOvRibfsDaw90952675 = -180682951;    float VKAxjXFJmuOvRibfsDaw85773701 = -680547612;    float VKAxjXFJmuOvRibfsDaw99919693 = -912281868;    float VKAxjXFJmuOvRibfsDaw2647295 = -274280756;    float VKAxjXFJmuOvRibfsDaw77686866 = -661048628;    float VKAxjXFJmuOvRibfsDaw66091500 = -841442631;    float VKAxjXFJmuOvRibfsDaw92844142 = -191519913;    float VKAxjXFJmuOvRibfsDaw8238602 = -226981225;    float VKAxjXFJmuOvRibfsDaw24792075 = -921703363;    float VKAxjXFJmuOvRibfsDaw50091608 = -581731260;    float VKAxjXFJmuOvRibfsDaw11265938 = -638712156;    float VKAxjXFJmuOvRibfsDaw3737715 = -33614734;    float VKAxjXFJmuOvRibfsDaw15754987 = 60510813;    float VKAxjXFJmuOvRibfsDaw42176275 = -25760595;    float VKAxjXFJmuOvRibfsDaw65880582 = 67627627;    float VKAxjXFJmuOvRibfsDaw62132346 = -724296319;    float VKAxjXFJmuOvRibfsDaw91931453 = -725403207;    float VKAxjXFJmuOvRibfsDaw4973370 = -816843769;    float VKAxjXFJmuOvRibfsDaw98849121 = -603487135;    float VKAxjXFJmuOvRibfsDaw87197056 = -726970840;    float VKAxjXFJmuOvRibfsDaw52838802 = -181044302;    float VKAxjXFJmuOvRibfsDaw30595907 = -781806668;    float VKAxjXFJmuOvRibfsDaw18365639 = -629373564;    float VKAxjXFJmuOvRibfsDaw696781 = -601010331;    float VKAxjXFJmuOvRibfsDaw69550504 = -456740034;    float VKAxjXFJmuOvRibfsDaw67638203 = -196661704;    float VKAxjXFJmuOvRibfsDaw79023776 = -430729753;    float VKAxjXFJmuOvRibfsDaw78892584 = -282515276;    float VKAxjXFJmuOvRibfsDaw70185945 = -373692698;    float VKAxjXFJmuOvRibfsDaw54226504 = -367330174;    float VKAxjXFJmuOvRibfsDaw14422130 = -937963188;    float VKAxjXFJmuOvRibfsDaw95724532 = 44939534;    float VKAxjXFJmuOvRibfsDaw53200151 = -159450903;    float VKAxjXFJmuOvRibfsDaw90149516 = -774826675;    float VKAxjXFJmuOvRibfsDaw33130628 = -229491759;    float VKAxjXFJmuOvRibfsDaw41950688 = -76544717;    float VKAxjXFJmuOvRibfsDaw44162253 = -405676847;    float VKAxjXFJmuOvRibfsDaw86638201 = -746848900;    float VKAxjXFJmuOvRibfsDaw74898074 = -3528383;    float VKAxjXFJmuOvRibfsDaw14489909 = -778848963;    float VKAxjXFJmuOvRibfsDaw71816587 = -589144992;    float VKAxjXFJmuOvRibfsDaw92625874 = -221019025;    float VKAxjXFJmuOvRibfsDaw3083487 = -740576770;    float VKAxjXFJmuOvRibfsDaw38810781 = -407235009;    float VKAxjXFJmuOvRibfsDaw97883941 = -560775397;    float VKAxjXFJmuOvRibfsDaw25020932 = -46416398;    float VKAxjXFJmuOvRibfsDaw54646596 = -466178080;    float VKAxjXFJmuOvRibfsDaw17020023 = -285283881;    float VKAxjXFJmuOvRibfsDaw89904371 = -254284108;    float VKAxjXFJmuOvRibfsDaw48783855 = -974059650;    float VKAxjXFJmuOvRibfsDaw19707559 = -638081469;    float VKAxjXFJmuOvRibfsDaw3573485 = -582850838;    float VKAxjXFJmuOvRibfsDaw54633646 = 61184217;    float VKAxjXFJmuOvRibfsDaw22498345 = -138474827;    float VKAxjXFJmuOvRibfsDaw98237358 = -913187795;    float VKAxjXFJmuOvRibfsDaw60356769 = -398876284;    float VKAxjXFJmuOvRibfsDaw67408063 = 48825951;    float VKAxjXFJmuOvRibfsDaw99222913 = -211271538;    float VKAxjXFJmuOvRibfsDaw33096791 = -817540722;    float VKAxjXFJmuOvRibfsDaw10048664 = -364386925;    float VKAxjXFJmuOvRibfsDaw87067724 = -310712878;    float VKAxjXFJmuOvRibfsDaw13951558 = -909004637;    float VKAxjXFJmuOvRibfsDaw38052657 = -853288527;    float VKAxjXFJmuOvRibfsDaw70565570 = -454373190;    float VKAxjXFJmuOvRibfsDaw35669478 = -643768072;    float VKAxjXFJmuOvRibfsDaw15541405 = -583651691;    float VKAxjXFJmuOvRibfsDaw50537564 = -874163832;    float VKAxjXFJmuOvRibfsDaw25605470 = -164662512;    float VKAxjXFJmuOvRibfsDaw9045647 = -796268837;    float VKAxjXFJmuOvRibfsDaw23929894 = -855827656;    float VKAxjXFJmuOvRibfsDaw17970094 = -218619473;    float VKAxjXFJmuOvRibfsDaw5293252 = -978554307;    float VKAxjXFJmuOvRibfsDaw30075295 = -713315387;    float VKAxjXFJmuOvRibfsDaw84359212 = -824638173;    float VKAxjXFJmuOvRibfsDaw15380469 = -37825849;    float VKAxjXFJmuOvRibfsDaw60212928 = -960025277;    float VKAxjXFJmuOvRibfsDaw27512420 = -124896569;     VKAxjXFJmuOvRibfsDaw62895551 = VKAxjXFJmuOvRibfsDaw41225677;     VKAxjXFJmuOvRibfsDaw41225677 = VKAxjXFJmuOvRibfsDaw24083304;     VKAxjXFJmuOvRibfsDaw24083304 = VKAxjXFJmuOvRibfsDaw27724390;     VKAxjXFJmuOvRibfsDaw27724390 = VKAxjXFJmuOvRibfsDaw44081946;     VKAxjXFJmuOvRibfsDaw44081946 = VKAxjXFJmuOvRibfsDaw89285496;     VKAxjXFJmuOvRibfsDaw89285496 = VKAxjXFJmuOvRibfsDaw52584941;     VKAxjXFJmuOvRibfsDaw52584941 = VKAxjXFJmuOvRibfsDaw80581409;     VKAxjXFJmuOvRibfsDaw80581409 = VKAxjXFJmuOvRibfsDaw64660729;     VKAxjXFJmuOvRibfsDaw64660729 = VKAxjXFJmuOvRibfsDaw864476;     VKAxjXFJmuOvRibfsDaw864476 = VKAxjXFJmuOvRibfsDaw27875562;     VKAxjXFJmuOvRibfsDaw27875562 = VKAxjXFJmuOvRibfsDaw88902388;     VKAxjXFJmuOvRibfsDaw88902388 = VKAxjXFJmuOvRibfsDaw9149879;     VKAxjXFJmuOvRibfsDaw9149879 = VKAxjXFJmuOvRibfsDaw28758647;     VKAxjXFJmuOvRibfsDaw28758647 = VKAxjXFJmuOvRibfsDaw70401582;     VKAxjXFJmuOvRibfsDaw70401582 = VKAxjXFJmuOvRibfsDaw59196298;     VKAxjXFJmuOvRibfsDaw59196298 = VKAxjXFJmuOvRibfsDaw55784953;     VKAxjXFJmuOvRibfsDaw55784953 = VKAxjXFJmuOvRibfsDaw10916201;     VKAxjXFJmuOvRibfsDaw10916201 = VKAxjXFJmuOvRibfsDaw11639012;     VKAxjXFJmuOvRibfsDaw11639012 = VKAxjXFJmuOvRibfsDaw8546854;     VKAxjXFJmuOvRibfsDaw8546854 = VKAxjXFJmuOvRibfsDaw53482768;     VKAxjXFJmuOvRibfsDaw53482768 = VKAxjXFJmuOvRibfsDaw9695401;     VKAxjXFJmuOvRibfsDaw9695401 = VKAxjXFJmuOvRibfsDaw51076161;     VKAxjXFJmuOvRibfsDaw51076161 = VKAxjXFJmuOvRibfsDaw90952675;     VKAxjXFJmuOvRibfsDaw90952675 = VKAxjXFJmuOvRibfsDaw85773701;     VKAxjXFJmuOvRibfsDaw85773701 = VKAxjXFJmuOvRibfsDaw99919693;     VKAxjXFJmuOvRibfsDaw99919693 = VKAxjXFJmuOvRibfsDaw2647295;     VKAxjXFJmuOvRibfsDaw2647295 = VKAxjXFJmuOvRibfsDaw77686866;     VKAxjXFJmuOvRibfsDaw77686866 = VKAxjXFJmuOvRibfsDaw66091500;     VKAxjXFJmuOvRibfsDaw66091500 = VKAxjXFJmuOvRibfsDaw92844142;     VKAxjXFJmuOvRibfsDaw92844142 = VKAxjXFJmuOvRibfsDaw8238602;     VKAxjXFJmuOvRibfsDaw8238602 = VKAxjXFJmuOvRibfsDaw24792075;     VKAxjXFJmuOvRibfsDaw24792075 = VKAxjXFJmuOvRibfsDaw50091608;     VKAxjXFJmuOvRibfsDaw50091608 = VKAxjXFJmuOvRibfsDaw11265938;     VKAxjXFJmuOvRibfsDaw11265938 = VKAxjXFJmuOvRibfsDaw3737715;     VKAxjXFJmuOvRibfsDaw3737715 = VKAxjXFJmuOvRibfsDaw15754987;     VKAxjXFJmuOvRibfsDaw15754987 = VKAxjXFJmuOvRibfsDaw42176275;     VKAxjXFJmuOvRibfsDaw42176275 = VKAxjXFJmuOvRibfsDaw65880582;     VKAxjXFJmuOvRibfsDaw65880582 = VKAxjXFJmuOvRibfsDaw62132346;     VKAxjXFJmuOvRibfsDaw62132346 = VKAxjXFJmuOvRibfsDaw91931453;     VKAxjXFJmuOvRibfsDaw91931453 = VKAxjXFJmuOvRibfsDaw4973370;     VKAxjXFJmuOvRibfsDaw4973370 = VKAxjXFJmuOvRibfsDaw98849121;     VKAxjXFJmuOvRibfsDaw98849121 = VKAxjXFJmuOvRibfsDaw87197056;     VKAxjXFJmuOvRibfsDaw87197056 = VKAxjXFJmuOvRibfsDaw52838802;     VKAxjXFJmuOvRibfsDaw52838802 = VKAxjXFJmuOvRibfsDaw30595907;     VKAxjXFJmuOvRibfsDaw30595907 = VKAxjXFJmuOvRibfsDaw18365639;     VKAxjXFJmuOvRibfsDaw18365639 = VKAxjXFJmuOvRibfsDaw696781;     VKAxjXFJmuOvRibfsDaw696781 = VKAxjXFJmuOvRibfsDaw69550504;     VKAxjXFJmuOvRibfsDaw69550504 = VKAxjXFJmuOvRibfsDaw67638203;     VKAxjXFJmuOvRibfsDaw67638203 = VKAxjXFJmuOvRibfsDaw79023776;     VKAxjXFJmuOvRibfsDaw79023776 = VKAxjXFJmuOvRibfsDaw78892584;     VKAxjXFJmuOvRibfsDaw78892584 = VKAxjXFJmuOvRibfsDaw70185945;     VKAxjXFJmuOvRibfsDaw70185945 = VKAxjXFJmuOvRibfsDaw54226504;     VKAxjXFJmuOvRibfsDaw54226504 = VKAxjXFJmuOvRibfsDaw14422130;     VKAxjXFJmuOvRibfsDaw14422130 = VKAxjXFJmuOvRibfsDaw95724532;     VKAxjXFJmuOvRibfsDaw95724532 = VKAxjXFJmuOvRibfsDaw53200151;     VKAxjXFJmuOvRibfsDaw53200151 = VKAxjXFJmuOvRibfsDaw90149516;     VKAxjXFJmuOvRibfsDaw90149516 = VKAxjXFJmuOvRibfsDaw33130628;     VKAxjXFJmuOvRibfsDaw33130628 = VKAxjXFJmuOvRibfsDaw41950688;     VKAxjXFJmuOvRibfsDaw41950688 = VKAxjXFJmuOvRibfsDaw44162253;     VKAxjXFJmuOvRibfsDaw44162253 = VKAxjXFJmuOvRibfsDaw86638201;     VKAxjXFJmuOvRibfsDaw86638201 = VKAxjXFJmuOvRibfsDaw74898074;     VKAxjXFJmuOvRibfsDaw74898074 = VKAxjXFJmuOvRibfsDaw14489909;     VKAxjXFJmuOvRibfsDaw14489909 = VKAxjXFJmuOvRibfsDaw71816587;     VKAxjXFJmuOvRibfsDaw71816587 = VKAxjXFJmuOvRibfsDaw92625874;     VKAxjXFJmuOvRibfsDaw92625874 = VKAxjXFJmuOvRibfsDaw3083487;     VKAxjXFJmuOvRibfsDaw3083487 = VKAxjXFJmuOvRibfsDaw38810781;     VKAxjXFJmuOvRibfsDaw38810781 = VKAxjXFJmuOvRibfsDaw97883941;     VKAxjXFJmuOvRibfsDaw97883941 = VKAxjXFJmuOvRibfsDaw25020932;     VKAxjXFJmuOvRibfsDaw25020932 = VKAxjXFJmuOvRibfsDaw54646596;     VKAxjXFJmuOvRibfsDaw54646596 = VKAxjXFJmuOvRibfsDaw17020023;     VKAxjXFJmuOvRibfsDaw17020023 = VKAxjXFJmuOvRibfsDaw89904371;     VKAxjXFJmuOvRibfsDaw89904371 = VKAxjXFJmuOvRibfsDaw48783855;     VKAxjXFJmuOvRibfsDaw48783855 = VKAxjXFJmuOvRibfsDaw19707559;     VKAxjXFJmuOvRibfsDaw19707559 = VKAxjXFJmuOvRibfsDaw3573485;     VKAxjXFJmuOvRibfsDaw3573485 = VKAxjXFJmuOvRibfsDaw54633646;     VKAxjXFJmuOvRibfsDaw54633646 = VKAxjXFJmuOvRibfsDaw22498345;     VKAxjXFJmuOvRibfsDaw22498345 = VKAxjXFJmuOvRibfsDaw98237358;     VKAxjXFJmuOvRibfsDaw98237358 = VKAxjXFJmuOvRibfsDaw60356769;     VKAxjXFJmuOvRibfsDaw60356769 = VKAxjXFJmuOvRibfsDaw67408063;     VKAxjXFJmuOvRibfsDaw67408063 = VKAxjXFJmuOvRibfsDaw99222913;     VKAxjXFJmuOvRibfsDaw99222913 = VKAxjXFJmuOvRibfsDaw33096791;     VKAxjXFJmuOvRibfsDaw33096791 = VKAxjXFJmuOvRibfsDaw10048664;     VKAxjXFJmuOvRibfsDaw10048664 = VKAxjXFJmuOvRibfsDaw87067724;     VKAxjXFJmuOvRibfsDaw87067724 = VKAxjXFJmuOvRibfsDaw13951558;     VKAxjXFJmuOvRibfsDaw13951558 = VKAxjXFJmuOvRibfsDaw38052657;     VKAxjXFJmuOvRibfsDaw38052657 = VKAxjXFJmuOvRibfsDaw70565570;     VKAxjXFJmuOvRibfsDaw70565570 = VKAxjXFJmuOvRibfsDaw35669478;     VKAxjXFJmuOvRibfsDaw35669478 = VKAxjXFJmuOvRibfsDaw15541405;     VKAxjXFJmuOvRibfsDaw15541405 = VKAxjXFJmuOvRibfsDaw50537564;     VKAxjXFJmuOvRibfsDaw50537564 = VKAxjXFJmuOvRibfsDaw25605470;     VKAxjXFJmuOvRibfsDaw25605470 = VKAxjXFJmuOvRibfsDaw9045647;     VKAxjXFJmuOvRibfsDaw9045647 = VKAxjXFJmuOvRibfsDaw23929894;     VKAxjXFJmuOvRibfsDaw23929894 = VKAxjXFJmuOvRibfsDaw17970094;     VKAxjXFJmuOvRibfsDaw17970094 = VKAxjXFJmuOvRibfsDaw5293252;     VKAxjXFJmuOvRibfsDaw5293252 = VKAxjXFJmuOvRibfsDaw30075295;     VKAxjXFJmuOvRibfsDaw30075295 = VKAxjXFJmuOvRibfsDaw84359212;     VKAxjXFJmuOvRibfsDaw84359212 = VKAxjXFJmuOvRibfsDaw15380469;     VKAxjXFJmuOvRibfsDaw15380469 = VKAxjXFJmuOvRibfsDaw60212928;     VKAxjXFJmuOvRibfsDaw60212928 = VKAxjXFJmuOvRibfsDaw27512420;     VKAxjXFJmuOvRibfsDaw27512420 = VKAxjXFJmuOvRibfsDaw62895551;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void lcBzQeTEzDiaXkFxSwFa3623514() {     float MQhHuppOyMeZigZFLZJq33547218 = -743045700;    float MQhHuppOyMeZigZFLZJq9899195 = -840702070;    float MQhHuppOyMeZigZFLZJq4052223 = -952469169;    float MQhHuppOyMeZigZFLZJq67655676 = 52769282;    float MQhHuppOyMeZigZFLZJq65532267 = -101365061;    float MQhHuppOyMeZigZFLZJq79285258 = -822710807;    float MQhHuppOyMeZigZFLZJq70963836 = -763352720;    float MQhHuppOyMeZigZFLZJq37423790 = -967108683;    float MQhHuppOyMeZigZFLZJq10525784 = -974012233;    float MQhHuppOyMeZigZFLZJq23830937 = -849095600;    float MQhHuppOyMeZigZFLZJq70124722 = -697182061;    float MQhHuppOyMeZigZFLZJq34448960 = -304730061;    float MQhHuppOyMeZigZFLZJq88977265 = -109024611;    float MQhHuppOyMeZigZFLZJq12605680 = 18940658;    float MQhHuppOyMeZigZFLZJq53121602 = -655417270;    float MQhHuppOyMeZigZFLZJq91724938 = 18971937;    float MQhHuppOyMeZigZFLZJq49637780 = -195841611;    float MQhHuppOyMeZigZFLZJq31209696 = -687522164;    float MQhHuppOyMeZigZFLZJq93346793 = 29327383;    float MQhHuppOyMeZigZFLZJq67333563 = -942060911;    float MQhHuppOyMeZigZFLZJq8618523 = -550982058;    float MQhHuppOyMeZigZFLZJq71091165 = -196730973;    float MQhHuppOyMeZigZFLZJq53019969 = -662075241;    float MQhHuppOyMeZigZFLZJq24802213 = -873083857;    float MQhHuppOyMeZigZFLZJq49898090 = -356452300;    float MQhHuppOyMeZigZFLZJq30517274 = 89433687;    float MQhHuppOyMeZigZFLZJq10908080 = -669458531;    float MQhHuppOyMeZigZFLZJq85469091 = -66172395;    float MQhHuppOyMeZigZFLZJq98233988 = -35800848;    float MQhHuppOyMeZigZFLZJq32016238 = -749914984;    float MQhHuppOyMeZigZFLZJq15310598 = 24585090;    float MQhHuppOyMeZigZFLZJq69303535 = -150000526;    float MQhHuppOyMeZigZFLZJq33036969 = -528692869;    float MQhHuppOyMeZigZFLZJq76630912 = -202241840;    float MQhHuppOyMeZigZFLZJq81761950 = -980561514;    float MQhHuppOyMeZigZFLZJq75748245 = -18715735;    float MQhHuppOyMeZigZFLZJq85967761 = -214353803;    float MQhHuppOyMeZigZFLZJq18673071 = -816485481;    float MQhHuppOyMeZigZFLZJq40665635 = -680024955;    float MQhHuppOyMeZigZFLZJq87034752 = -243011570;    float MQhHuppOyMeZigZFLZJq65046653 = -100862648;    float MQhHuppOyMeZigZFLZJq27332113 = -372876204;    float MQhHuppOyMeZigZFLZJq36164016 = -713724892;    float MQhHuppOyMeZigZFLZJq59347102 = -253019273;    float MQhHuppOyMeZigZFLZJq26549966 = 9546938;    float MQhHuppOyMeZigZFLZJq24771473 = -387710718;    float MQhHuppOyMeZigZFLZJq26827984 = -717080130;    float MQhHuppOyMeZigZFLZJq35164714 = -908870329;    float MQhHuppOyMeZigZFLZJq73079655 = -182425673;    float MQhHuppOyMeZigZFLZJq87451262 = -713738632;    float MQhHuppOyMeZigZFLZJq47081908 = -929087397;    float MQhHuppOyMeZigZFLZJq12057450 = -342912420;    float MQhHuppOyMeZigZFLZJq9302162 = -295128216;    float MQhHuppOyMeZigZFLZJq4649435 = -498844685;    float MQhHuppOyMeZigZFLZJq95004991 = -66001380;    float MQhHuppOyMeZigZFLZJq62456053 = -446314727;    float MQhHuppOyMeZigZFLZJq56879225 = -78626830;    float MQhHuppOyMeZigZFLZJq79250010 = 20614688;    float MQhHuppOyMeZigZFLZJq17757586 = -590778419;    float MQhHuppOyMeZigZFLZJq35014993 = -90798749;    float MQhHuppOyMeZigZFLZJq68377179 = -53252277;    float MQhHuppOyMeZigZFLZJq85494744 = -597180325;    float MQhHuppOyMeZigZFLZJq39189802 = -831307836;    float MQhHuppOyMeZigZFLZJq78509546 = -124097250;    float MQhHuppOyMeZigZFLZJq8520339 = -773680691;    float MQhHuppOyMeZigZFLZJq821187 = -447181536;    float MQhHuppOyMeZigZFLZJq1411992 = -776037193;    float MQhHuppOyMeZigZFLZJq12346354 = -906782771;    float MQhHuppOyMeZigZFLZJq30843729 = -497829;    float MQhHuppOyMeZigZFLZJq77373356 = -536701535;    float MQhHuppOyMeZigZFLZJq5757178 = -766674261;    float MQhHuppOyMeZigZFLZJq30964709 = -379356131;    float MQhHuppOyMeZigZFLZJq90544060 = 92502791;    float MQhHuppOyMeZigZFLZJq6312042 = -727661048;    float MQhHuppOyMeZigZFLZJq2286910 = -741198264;    float MQhHuppOyMeZigZFLZJq81286409 = -78105854;    float MQhHuppOyMeZigZFLZJq34927149 = -483006081;    float MQhHuppOyMeZigZFLZJq93672867 = -309055968;    float MQhHuppOyMeZigZFLZJq98252247 = -782630796;    float MQhHuppOyMeZigZFLZJq25126617 = -968741582;    float MQhHuppOyMeZigZFLZJq3689291 = -193486183;    float MQhHuppOyMeZigZFLZJq75743366 = -760588202;    float MQhHuppOyMeZigZFLZJq12389437 = -883746723;    float MQhHuppOyMeZigZFLZJq10782726 = -322062217;    float MQhHuppOyMeZigZFLZJq84934329 = -820827587;    float MQhHuppOyMeZigZFLZJq3253149 = -632502490;    float MQhHuppOyMeZigZFLZJq60001374 = -854872311;    float MQhHuppOyMeZigZFLZJq28387534 = 70151815;    float MQhHuppOyMeZigZFLZJq81625920 = -36240460;    float MQhHuppOyMeZigZFLZJq19305898 = -434246787;    float MQhHuppOyMeZigZFLZJq18869020 = -940088906;    float MQhHuppOyMeZigZFLZJq6717751 = -134968491;    float MQhHuppOyMeZigZFLZJq915485 = -125707062;    float MQhHuppOyMeZigZFLZJq5650642 = -489226207;    float MQhHuppOyMeZigZFLZJq18657574 = -89759293;    float MQhHuppOyMeZigZFLZJq79551908 = -503682323;    float MQhHuppOyMeZigZFLZJq88142310 = -541568369;    float MQhHuppOyMeZigZFLZJq57654470 = -489627643;    float MQhHuppOyMeZigZFLZJq50826764 = -479338583;    float MQhHuppOyMeZigZFLZJq25728779 = -743045700;     MQhHuppOyMeZigZFLZJq33547218 = MQhHuppOyMeZigZFLZJq9899195;     MQhHuppOyMeZigZFLZJq9899195 = MQhHuppOyMeZigZFLZJq4052223;     MQhHuppOyMeZigZFLZJq4052223 = MQhHuppOyMeZigZFLZJq67655676;     MQhHuppOyMeZigZFLZJq67655676 = MQhHuppOyMeZigZFLZJq65532267;     MQhHuppOyMeZigZFLZJq65532267 = MQhHuppOyMeZigZFLZJq79285258;     MQhHuppOyMeZigZFLZJq79285258 = MQhHuppOyMeZigZFLZJq70963836;     MQhHuppOyMeZigZFLZJq70963836 = MQhHuppOyMeZigZFLZJq37423790;     MQhHuppOyMeZigZFLZJq37423790 = MQhHuppOyMeZigZFLZJq10525784;     MQhHuppOyMeZigZFLZJq10525784 = MQhHuppOyMeZigZFLZJq23830937;     MQhHuppOyMeZigZFLZJq23830937 = MQhHuppOyMeZigZFLZJq70124722;     MQhHuppOyMeZigZFLZJq70124722 = MQhHuppOyMeZigZFLZJq34448960;     MQhHuppOyMeZigZFLZJq34448960 = MQhHuppOyMeZigZFLZJq88977265;     MQhHuppOyMeZigZFLZJq88977265 = MQhHuppOyMeZigZFLZJq12605680;     MQhHuppOyMeZigZFLZJq12605680 = MQhHuppOyMeZigZFLZJq53121602;     MQhHuppOyMeZigZFLZJq53121602 = MQhHuppOyMeZigZFLZJq91724938;     MQhHuppOyMeZigZFLZJq91724938 = MQhHuppOyMeZigZFLZJq49637780;     MQhHuppOyMeZigZFLZJq49637780 = MQhHuppOyMeZigZFLZJq31209696;     MQhHuppOyMeZigZFLZJq31209696 = MQhHuppOyMeZigZFLZJq93346793;     MQhHuppOyMeZigZFLZJq93346793 = MQhHuppOyMeZigZFLZJq67333563;     MQhHuppOyMeZigZFLZJq67333563 = MQhHuppOyMeZigZFLZJq8618523;     MQhHuppOyMeZigZFLZJq8618523 = MQhHuppOyMeZigZFLZJq71091165;     MQhHuppOyMeZigZFLZJq71091165 = MQhHuppOyMeZigZFLZJq53019969;     MQhHuppOyMeZigZFLZJq53019969 = MQhHuppOyMeZigZFLZJq24802213;     MQhHuppOyMeZigZFLZJq24802213 = MQhHuppOyMeZigZFLZJq49898090;     MQhHuppOyMeZigZFLZJq49898090 = MQhHuppOyMeZigZFLZJq30517274;     MQhHuppOyMeZigZFLZJq30517274 = MQhHuppOyMeZigZFLZJq10908080;     MQhHuppOyMeZigZFLZJq10908080 = MQhHuppOyMeZigZFLZJq85469091;     MQhHuppOyMeZigZFLZJq85469091 = MQhHuppOyMeZigZFLZJq98233988;     MQhHuppOyMeZigZFLZJq98233988 = MQhHuppOyMeZigZFLZJq32016238;     MQhHuppOyMeZigZFLZJq32016238 = MQhHuppOyMeZigZFLZJq15310598;     MQhHuppOyMeZigZFLZJq15310598 = MQhHuppOyMeZigZFLZJq69303535;     MQhHuppOyMeZigZFLZJq69303535 = MQhHuppOyMeZigZFLZJq33036969;     MQhHuppOyMeZigZFLZJq33036969 = MQhHuppOyMeZigZFLZJq76630912;     MQhHuppOyMeZigZFLZJq76630912 = MQhHuppOyMeZigZFLZJq81761950;     MQhHuppOyMeZigZFLZJq81761950 = MQhHuppOyMeZigZFLZJq75748245;     MQhHuppOyMeZigZFLZJq75748245 = MQhHuppOyMeZigZFLZJq85967761;     MQhHuppOyMeZigZFLZJq85967761 = MQhHuppOyMeZigZFLZJq18673071;     MQhHuppOyMeZigZFLZJq18673071 = MQhHuppOyMeZigZFLZJq40665635;     MQhHuppOyMeZigZFLZJq40665635 = MQhHuppOyMeZigZFLZJq87034752;     MQhHuppOyMeZigZFLZJq87034752 = MQhHuppOyMeZigZFLZJq65046653;     MQhHuppOyMeZigZFLZJq65046653 = MQhHuppOyMeZigZFLZJq27332113;     MQhHuppOyMeZigZFLZJq27332113 = MQhHuppOyMeZigZFLZJq36164016;     MQhHuppOyMeZigZFLZJq36164016 = MQhHuppOyMeZigZFLZJq59347102;     MQhHuppOyMeZigZFLZJq59347102 = MQhHuppOyMeZigZFLZJq26549966;     MQhHuppOyMeZigZFLZJq26549966 = MQhHuppOyMeZigZFLZJq24771473;     MQhHuppOyMeZigZFLZJq24771473 = MQhHuppOyMeZigZFLZJq26827984;     MQhHuppOyMeZigZFLZJq26827984 = MQhHuppOyMeZigZFLZJq35164714;     MQhHuppOyMeZigZFLZJq35164714 = MQhHuppOyMeZigZFLZJq73079655;     MQhHuppOyMeZigZFLZJq73079655 = MQhHuppOyMeZigZFLZJq87451262;     MQhHuppOyMeZigZFLZJq87451262 = MQhHuppOyMeZigZFLZJq47081908;     MQhHuppOyMeZigZFLZJq47081908 = MQhHuppOyMeZigZFLZJq12057450;     MQhHuppOyMeZigZFLZJq12057450 = MQhHuppOyMeZigZFLZJq9302162;     MQhHuppOyMeZigZFLZJq9302162 = MQhHuppOyMeZigZFLZJq4649435;     MQhHuppOyMeZigZFLZJq4649435 = MQhHuppOyMeZigZFLZJq95004991;     MQhHuppOyMeZigZFLZJq95004991 = MQhHuppOyMeZigZFLZJq62456053;     MQhHuppOyMeZigZFLZJq62456053 = MQhHuppOyMeZigZFLZJq56879225;     MQhHuppOyMeZigZFLZJq56879225 = MQhHuppOyMeZigZFLZJq79250010;     MQhHuppOyMeZigZFLZJq79250010 = MQhHuppOyMeZigZFLZJq17757586;     MQhHuppOyMeZigZFLZJq17757586 = MQhHuppOyMeZigZFLZJq35014993;     MQhHuppOyMeZigZFLZJq35014993 = MQhHuppOyMeZigZFLZJq68377179;     MQhHuppOyMeZigZFLZJq68377179 = MQhHuppOyMeZigZFLZJq85494744;     MQhHuppOyMeZigZFLZJq85494744 = MQhHuppOyMeZigZFLZJq39189802;     MQhHuppOyMeZigZFLZJq39189802 = MQhHuppOyMeZigZFLZJq78509546;     MQhHuppOyMeZigZFLZJq78509546 = MQhHuppOyMeZigZFLZJq8520339;     MQhHuppOyMeZigZFLZJq8520339 = MQhHuppOyMeZigZFLZJq821187;     MQhHuppOyMeZigZFLZJq821187 = MQhHuppOyMeZigZFLZJq1411992;     MQhHuppOyMeZigZFLZJq1411992 = MQhHuppOyMeZigZFLZJq12346354;     MQhHuppOyMeZigZFLZJq12346354 = MQhHuppOyMeZigZFLZJq30843729;     MQhHuppOyMeZigZFLZJq30843729 = MQhHuppOyMeZigZFLZJq77373356;     MQhHuppOyMeZigZFLZJq77373356 = MQhHuppOyMeZigZFLZJq5757178;     MQhHuppOyMeZigZFLZJq5757178 = MQhHuppOyMeZigZFLZJq30964709;     MQhHuppOyMeZigZFLZJq30964709 = MQhHuppOyMeZigZFLZJq90544060;     MQhHuppOyMeZigZFLZJq90544060 = MQhHuppOyMeZigZFLZJq6312042;     MQhHuppOyMeZigZFLZJq6312042 = MQhHuppOyMeZigZFLZJq2286910;     MQhHuppOyMeZigZFLZJq2286910 = MQhHuppOyMeZigZFLZJq81286409;     MQhHuppOyMeZigZFLZJq81286409 = MQhHuppOyMeZigZFLZJq34927149;     MQhHuppOyMeZigZFLZJq34927149 = MQhHuppOyMeZigZFLZJq93672867;     MQhHuppOyMeZigZFLZJq93672867 = MQhHuppOyMeZigZFLZJq98252247;     MQhHuppOyMeZigZFLZJq98252247 = MQhHuppOyMeZigZFLZJq25126617;     MQhHuppOyMeZigZFLZJq25126617 = MQhHuppOyMeZigZFLZJq3689291;     MQhHuppOyMeZigZFLZJq3689291 = MQhHuppOyMeZigZFLZJq75743366;     MQhHuppOyMeZigZFLZJq75743366 = MQhHuppOyMeZigZFLZJq12389437;     MQhHuppOyMeZigZFLZJq12389437 = MQhHuppOyMeZigZFLZJq10782726;     MQhHuppOyMeZigZFLZJq10782726 = MQhHuppOyMeZigZFLZJq84934329;     MQhHuppOyMeZigZFLZJq84934329 = MQhHuppOyMeZigZFLZJq3253149;     MQhHuppOyMeZigZFLZJq3253149 = MQhHuppOyMeZigZFLZJq60001374;     MQhHuppOyMeZigZFLZJq60001374 = MQhHuppOyMeZigZFLZJq28387534;     MQhHuppOyMeZigZFLZJq28387534 = MQhHuppOyMeZigZFLZJq81625920;     MQhHuppOyMeZigZFLZJq81625920 = MQhHuppOyMeZigZFLZJq19305898;     MQhHuppOyMeZigZFLZJq19305898 = MQhHuppOyMeZigZFLZJq18869020;     MQhHuppOyMeZigZFLZJq18869020 = MQhHuppOyMeZigZFLZJq6717751;     MQhHuppOyMeZigZFLZJq6717751 = MQhHuppOyMeZigZFLZJq915485;     MQhHuppOyMeZigZFLZJq915485 = MQhHuppOyMeZigZFLZJq5650642;     MQhHuppOyMeZigZFLZJq5650642 = MQhHuppOyMeZigZFLZJq18657574;     MQhHuppOyMeZigZFLZJq18657574 = MQhHuppOyMeZigZFLZJq79551908;     MQhHuppOyMeZigZFLZJq79551908 = MQhHuppOyMeZigZFLZJq88142310;     MQhHuppOyMeZigZFLZJq88142310 = MQhHuppOyMeZigZFLZJq57654470;     MQhHuppOyMeZigZFLZJq57654470 = MQhHuppOyMeZigZFLZJq50826764;     MQhHuppOyMeZigZFLZJq50826764 = MQhHuppOyMeZigZFLZJq25728779;     MQhHuppOyMeZigZFLZJq25728779 = MQhHuppOyMeZigZFLZJq33547218;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xaqPXiCGcnrdaGaDMeID10282259() {     float CtVtBLFtspvLkNCmMqwZ33540747 = -615569841;    float CtVtBLFtspvLkNCmMqwZ48471511 = -883297195;    float CtVtBLFtspvLkNCmMqwZ5365436 = -909597783;    float CtVtBLFtspvLkNCmMqwZ52735537 = -867771548;    float CtVtBLFtspvLkNCmMqwZ74372369 = -569720383;    float CtVtBLFtspvLkNCmMqwZ14090691 = -522143619;    float CtVtBLFtspvLkNCmMqwZ39511172 = -467889187;    float CtVtBLFtspvLkNCmMqwZ6637841 = -671286000;    float CtVtBLFtspvLkNCmMqwZ93265948 = -687658303;    float CtVtBLFtspvLkNCmMqwZ20470450 = -485902471;    float CtVtBLFtspvLkNCmMqwZ90445327 = -656654133;    float CtVtBLFtspvLkNCmMqwZ48740637 = -40208424;    float CtVtBLFtspvLkNCmMqwZ20830180 = -130683085;    float CtVtBLFtspvLkNCmMqwZ66449900 = -326212393;    float CtVtBLFtspvLkNCmMqwZ98293710 = -618130122;    float CtVtBLFtspvLkNCmMqwZ66289485 = -313823714;    float CtVtBLFtspvLkNCmMqwZ5490540 = -15456181;    float CtVtBLFtspvLkNCmMqwZ54210228 = 15406214;    float CtVtBLFtspvLkNCmMqwZ13775064 = -588654884;    float CtVtBLFtspvLkNCmMqwZ39407326 = -454085363;    float CtVtBLFtspvLkNCmMqwZ88287946 = -261448432;    float CtVtBLFtspvLkNCmMqwZ49387431 = -706722856;    float CtVtBLFtspvLkNCmMqwZ89898366 = -21033083;    float CtVtBLFtspvLkNCmMqwZ68452321 = -754004380;    float CtVtBLFtspvLkNCmMqwZ78340779 = -783146153;    float CtVtBLFtspvLkNCmMqwZ47091351 = -918772241;    float CtVtBLFtspvLkNCmMqwZ81129661 = -970258244;    float CtVtBLFtspvLkNCmMqwZ20872617 = -675139384;    float CtVtBLFtspvLkNCmMqwZ10912436 = -681871163;    float CtVtBLFtspvLkNCmMqwZ8106108 = -425346357;    float CtVtBLFtspvLkNCmMqwZ48723277 = -362508145;    float CtVtBLFtspvLkNCmMqwZ54027393 = -541822287;    float CtVtBLFtspvLkNCmMqwZ18489392 = -730637085;    float CtVtBLFtspvLkNCmMqwZ76913749 = -78923573;    float CtVtBLFtspvLkNCmMqwZ64119989 = -139875438;    float CtVtBLFtspvLkNCmMqwZ18576435 = -98481481;    float CtVtBLFtspvLkNCmMqwZ97909128 = -314007181;    float CtVtBLFtspvLkNCmMqwZ88590750 = -811748160;    float CtVtBLFtspvLkNCmMqwZ75979231 = -292706372;    float CtVtBLFtspvLkNCmMqwZ58907645 = -952443256;    float CtVtBLFtspvLkNCmMqwZ52902321 = -188806546;    float CtVtBLFtspvLkNCmMqwZ13029774 = -566668287;    float CtVtBLFtspvLkNCmMqwZ96447376 = -172855147;    float CtVtBLFtspvLkNCmMqwZ18579001 = -909983717;    float CtVtBLFtspvLkNCmMqwZ10568018 = -463712464;    float CtVtBLFtspvLkNCmMqwZ65199441 = -199755389;    float CtVtBLFtspvLkNCmMqwZ54525776 = -98360752;    float CtVtBLFtspvLkNCmMqwZ10548295 = -417631685;    float CtVtBLFtspvLkNCmMqwZ91738044 = -846189534;    float CtVtBLFtspvLkNCmMqwZ7496686 = -300763518;    float CtVtBLFtspvLkNCmMqwZ60341278 = -507642405;    float CtVtBLFtspvLkNCmMqwZ26604707 = -725058168;    float CtVtBLFtspvLkNCmMqwZ84750509 = -187182224;    float CtVtBLFtspvLkNCmMqwZ14899705 = 69235464;    float CtVtBLFtspvLkNCmMqwZ21994312 = -922900658;    float CtVtBLFtspvLkNCmMqwZ84153315 = -908846986;    float CtVtBLFtspvLkNCmMqwZ58573145 = -762264113;    float CtVtBLFtspvLkNCmMqwZ36913114 = -55593403;    float CtVtBLFtspvLkNCmMqwZ74394757 = 15374604;    float CtVtBLFtspvLkNCmMqwZ27281019 = -650948142;    float CtVtBLFtspvLkNCmMqwZ32961030 = -551885376;    float CtVtBLFtspvLkNCmMqwZ18638556 = -792749804;    float CtVtBLFtspvLkNCmMqwZ95725405 = -989414838;    float CtVtBLFtspvLkNCmMqwZ85159840 = -162311946;    float CtVtBLFtspvLkNCmMqwZ71747173 = -23394327;    float CtVtBLFtspvLkNCmMqwZ36417935 = -14831846;    float CtVtBLFtspvLkNCmMqwZ30251245 = -309571339;    float CtVtBLFtspvLkNCmMqwZ43916430 = 48240487;    float CtVtBLFtspvLkNCmMqwZ2329912 = -86336955;    float CtVtBLFtspvLkNCmMqwZ79717276 = -419648641;    float CtVtBLFtspvLkNCmMqwZ68380356 = -999816533;    float CtVtBLFtspvLkNCmMqwZ16899789 = -203708021;    float CtVtBLFtspvLkNCmMqwZ78230996 = -691887415;    float CtVtBLFtspvLkNCmMqwZ54867419 = -636211629;    float CtVtBLFtspvLkNCmMqwZ86505005 = -165278817;    float CtVtBLFtspvLkNCmMqwZ75258173 = -694780146;    float CtVtBLFtspvLkNCmMqwZ52940055 = -433867710;    float CtVtBLFtspvLkNCmMqwZ71319365 = -111049367;    float CtVtBLFtspvLkNCmMqwZ57884303 = -190291917;    float CtVtBLFtspvLkNCmMqwZ13141339 = -483390764;    float CtVtBLFtspvLkNCmMqwZ92565575 = -720411489;    float CtVtBLFtspvLkNCmMqwZ70581367 = -452626559;    float CtVtBLFtspvLkNCmMqwZ29134572 = -828949850;    float CtVtBLFtspvLkNCmMqwZ3415750 = -281107645;    float CtVtBLFtspvLkNCmMqwZ47764830 = -917703953;    float CtVtBLFtspvLkNCmMqwZ22118570 = -637449977;    float CtVtBLFtspvLkNCmMqwZ69276883 = -254640064;    float CtVtBLFtspvLkNCmMqwZ3589687 = -699872549;    float CtVtBLFtspvLkNCmMqwZ54919438 = -156022915;    float CtVtBLFtspvLkNCmMqwZ79966673 = -231028453;    float CtVtBLFtspvLkNCmMqwZ60003290 = -336217369;    float CtVtBLFtspvLkNCmMqwZ60996015 = -158413778;    float CtVtBLFtspvLkNCmMqwZ14195993 = -727122765;    float CtVtBLFtspvLkNCmMqwZ48698213 = -641758230;    float CtVtBLFtspvLkNCmMqwZ25946616 = -300557881;    float CtVtBLFtspvLkNCmMqwZ34263765 = -396056743;    float CtVtBLFtspvLkNCmMqwZ17304368 = -577253449;    float CtVtBLFtspvLkNCmMqwZ11287536 = 89456799;    float CtVtBLFtspvLkNCmMqwZ46831828 = -786589390;    float CtVtBLFtspvLkNCmMqwZ74150083 = -615569841;     CtVtBLFtspvLkNCmMqwZ33540747 = CtVtBLFtspvLkNCmMqwZ48471511;     CtVtBLFtspvLkNCmMqwZ48471511 = CtVtBLFtspvLkNCmMqwZ5365436;     CtVtBLFtspvLkNCmMqwZ5365436 = CtVtBLFtspvLkNCmMqwZ52735537;     CtVtBLFtspvLkNCmMqwZ52735537 = CtVtBLFtspvLkNCmMqwZ74372369;     CtVtBLFtspvLkNCmMqwZ74372369 = CtVtBLFtspvLkNCmMqwZ14090691;     CtVtBLFtspvLkNCmMqwZ14090691 = CtVtBLFtspvLkNCmMqwZ39511172;     CtVtBLFtspvLkNCmMqwZ39511172 = CtVtBLFtspvLkNCmMqwZ6637841;     CtVtBLFtspvLkNCmMqwZ6637841 = CtVtBLFtspvLkNCmMqwZ93265948;     CtVtBLFtspvLkNCmMqwZ93265948 = CtVtBLFtspvLkNCmMqwZ20470450;     CtVtBLFtspvLkNCmMqwZ20470450 = CtVtBLFtspvLkNCmMqwZ90445327;     CtVtBLFtspvLkNCmMqwZ90445327 = CtVtBLFtspvLkNCmMqwZ48740637;     CtVtBLFtspvLkNCmMqwZ48740637 = CtVtBLFtspvLkNCmMqwZ20830180;     CtVtBLFtspvLkNCmMqwZ20830180 = CtVtBLFtspvLkNCmMqwZ66449900;     CtVtBLFtspvLkNCmMqwZ66449900 = CtVtBLFtspvLkNCmMqwZ98293710;     CtVtBLFtspvLkNCmMqwZ98293710 = CtVtBLFtspvLkNCmMqwZ66289485;     CtVtBLFtspvLkNCmMqwZ66289485 = CtVtBLFtspvLkNCmMqwZ5490540;     CtVtBLFtspvLkNCmMqwZ5490540 = CtVtBLFtspvLkNCmMqwZ54210228;     CtVtBLFtspvLkNCmMqwZ54210228 = CtVtBLFtspvLkNCmMqwZ13775064;     CtVtBLFtspvLkNCmMqwZ13775064 = CtVtBLFtspvLkNCmMqwZ39407326;     CtVtBLFtspvLkNCmMqwZ39407326 = CtVtBLFtspvLkNCmMqwZ88287946;     CtVtBLFtspvLkNCmMqwZ88287946 = CtVtBLFtspvLkNCmMqwZ49387431;     CtVtBLFtspvLkNCmMqwZ49387431 = CtVtBLFtspvLkNCmMqwZ89898366;     CtVtBLFtspvLkNCmMqwZ89898366 = CtVtBLFtspvLkNCmMqwZ68452321;     CtVtBLFtspvLkNCmMqwZ68452321 = CtVtBLFtspvLkNCmMqwZ78340779;     CtVtBLFtspvLkNCmMqwZ78340779 = CtVtBLFtspvLkNCmMqwZ47091351;     CtVtBLFtspvLkNCmMqwZ47091351 = CtVtBLFtspvLkNCmMqwZ81129661;     CtVtBLFtspvLkNCmMqwZ81129661 = CtVtBLFtspvLkNCmMqwZ20872617;     CtVtBLFtspvLkNCmMqwZ20872617 = CtVtBLFtspvLkNCmMqwZ10912436;     CtVtBLFtspvLkNCmMqwZ10912436 = CtVtBLFtspvLkNCmMqwZ8106108;     CtVtBLFtspvLkNCmMqwZ8106108 = CtVtBLFtspvLkNCmMqwZ48723277;     CtVtBLFtspvLkNCmMqwZ48723277 = CtVtBLFtspvLkNCmMqwZ54027393;     CtVtBLFtspvLkNCmMqwZ54027393 = CtVtBLFtspvLkNCmMqwZ18489392;     CtVtBLFtspvLkNCmMqwZ18489392 = CtVtBLFtspvLkNCmMqwZ76913749;     CtVtBLFtspvLkNCmMqwZ76913749 = CtVtBLFtspvLkNCmMqwZ64119989;     CtVtBLFtspvLkNCmMqwZ64119989 = CtVtBLFtspvLkNCmMqwZ18576435;     CtVtBLFtspvLkNCmMqwZ18576435 = CtVtBLFtspvLkNCmMqwZ97909128;     CtVtBLFtspvLkNCmMqwZ97909128 = CtVtBLFtspvLkNCmMqwZ88590750;     CtVtBLFtspvLkNCmMqwZ88590750 = CtVtBLFtspvLkNCmMqwZ75979231;     CtVtBLFtspvLkNCmMqwZ75979231 = CtVtBLFtspvLkNCmMqwZ58907645;     CtVtBLFtspvLkNCmMqwZ58907645 = CtVtBLFtspvLkNCmMqwZ52902321;     CtVtBLFtspvLkNCmMqwZ52902321 = CtVtBLFtspvLkNCmMqwZ13029774;     CtVtBLFtspvLkNCmMqwZ13029774 = CtVtBLFtspvLkNCmMqwZ96447376;     CtVtBLFtspvLkNCmMqwZ96447376 = CtVtBLFtspvLkNCmMqwZ18579001;     CtVtBLFtspvLkNCmMqwZ18579001 = CtVtBLFtspvLkNCmMqwZ10568018;     CtVtBLFtspvLkNCmMqwZ10568018 = CtVtBLFtspvLkNCmMqwZ65199441;     CtVtBLFtspvLkNCmMqwZ65199441 = CtVtBLFtspvLkNCmMqwZ54525776;     CtVtBLFtspvLkNCmMqwZ54525776 = CtVtBLFtspvLkNCmMqwZ10548295;     CtVtBLFtspvLkNCmMqwZ10548295 = CtVtBLFtspvLkNCmMqwZ91738044;     CtVtBLFtspvLkNCmMqwZ91738044 = CtVtBLFtspvLkNCmMqwZ7496686;     CtVtBLFtspvLkNCmMqwZ7496686 = CtVtBLFtspvLkNCmMqwZ60341278;     CtVtBLFtspvLkNCmMqwZ60341278 = CtVtBLFtspvLkNCmMqwZ26604707;     CtVtBLFtspvLkNCmMqwZ26604707 = CtVtBLFtspvLkNCmMqwZ84750509;     CtVtBLFtspvLkNCmMqwZ84750509 = CtVtBLFtspvLkNCmMqwZ14899705;     CtVtBLFtspvLkNCmMqwZ14899705 = CtVtBLFtspvLkNCmMqwZ21994312;     CtVtBLFtspvLkNCmMqwZ21994312 = CtVtBLFtspvLkNCmMqwZ84153315;     CtVtBLFtspvLkNCmMqwZ84153315 = CtVtBLFtspvLkNCmMqwZ58573145;     CtVtBLFtspvLkNCmMqwZ58573145 = CtVtBLFtspvLkNCmMqwZ36913114;     CtVtBLFtspvLkNCmMqwZ36913114 = CtVtBLFtspvLkNCmMqwZ74394757;     CtVtBLFtspvLkNCmMqwZ74394757 = CtVtBLFtspvLkNCmMqwZ27281019;     CtVtBLFtspvLkNCmMqwZ27281019 = CtVtBLFtspvLkNCmMqwZ32961030;     CtVtBLFtspvLkNCmMqwZ32961030 = CtVtBLFtspvLkNCmMqwZ18638556;     CtVtBLFtspvLkNCmMqwZ18638556 = CtVtBLFtspvLkNCmMqwZ95725405;     CtVtBLFtspvLkNCmMqwZ95725405 = CtVtBLFtspvLkNCmMqwZ85159840;     CtVtBLFtspvLkNCmMqwZ85159840 = CtVtBLFtspvLkNCmMqwZ71747173;     CtVtBLFtspvLkNCmMqwZ71747173 = CtVtBLFtspvLkNCmMqwZ36417935;     CtVtBLFtspvLkNCmMqwZ36417935 = CtVtBLFtspvLkNCmMqwZ30251245;     CtVtBLFtspvLkNCmMqwZ30251245 = CtVtBLFtspvLkNCmMqwZ43916430;     CtVtBLFtspvLkNCmMqwZ43916430 = CtVtBLFtspvLkNCmMqwZ2329912;     CtVtBLFtspvLkNCmMqwZ2329912 = CtVtBLFtspvLkNCmMqwZ79717276;     CtVtBLFtspvLkNCmMqwZ79717276 = CtVtBLFtspvLkNCmMqwZ68380356;     CtVtBLFtspvLkNCmMqwZ68380356 = CtVtBLFtspvLkNCmMqwZ16899789;     CtVtBLFtspvLkNCmMqwZ16899789 = CtVtBLFtspvLkNCmMqwZ78230996;     CtVtBLFtspvLkNCmMqwZ78230996 = CtVtBLFtspvLkNCmMqwZ54867419;     CtVtBLFtspvLkNCmMqwZ54867419 = CtVtBLFtspvLkNCmMqwZ86505005;     CtVtBLFtspvLkNCmMqwZ86505005 = CtVtBLFtspvLkNCmMqwZ75258173;     CtVtBLFtspvLkNCmMqwZ75258173 = CtVtBLFtspvLkNCmMqwZ52940055;     CtVtBLFtspvLkNCmMqwZ52940055 = CtVtBLFtspvLkNCmMqwZ71319365;     CtVtBLFtspvLkNCmMqwZ71319365 = CtVtBLFtspvLkNCmMqwZ57884303;     CtVtBLFtspvLkNCmMqwZ57884303 = CtVtBLFtspvLkNCmMqwZ13141339;     CtVtBLFtspvLkNCmMqwZ13141339 = CtVtBLFtspvLkNCmMqwZ92565575;     CtVtBLFtspvLkNCmMqwZ92565575 = CtVtBLFtspvLkNCmMqwZ70581367;     CtVtBLFtspvLkNCmMqwZ70581367 = CtVtBLFtspvLkNCmMqwZ29134572;     CtVtBLFtspvLkNCmMqwZ29134572 = CtVtBLFtspvLkNCmMqwZ3415750;     CtVtBLFtspvLkNCmMqwZ3415750 = CtVtBLFtspvLkNCmMqwZ47764830;     CtVtBLFtspvLkNCmMqwZ47764830 = CtVtBLFtspvLkNCmMqwZ22118570;     CtVtBLFtspvLkNCmMqwZ22118570 = CtVtBLFtspvLkNCmMqwZ69276883;     CtVtBLFtspvLkNCmMqwZ69276883 = CtVtBLFtspvLkNCmMqwZ3589687;     CtVtBLFtspvLkNCmMqwZ3589687 = CtVtBLFtspvLkNCmMqwZ54919438;     CtVtBLFtspvLkNCmMqwZ54919438 = CtVtBLFtspvLkNCmMqwZ79966673;     CtVtBLFtspvLkNCmMqwZ79966673 = CtVtBLFtspvLkNCmMqwZ60003290;     CtVtBLFtspvLkNCmMqwZ60003290 = CtVtBLFtspvLkNCmMqwZ60996015;     CtVtBLFtspvLkNCmMqwZ60996015 = CtVtBLFtspvLkNCmMqwZ14195993;     CtVtBLFtspvLkNCmMqwZ14195993 = CtVtBLFtspvLkNCmMqwZ48698213;     CtVtBLFtspvLkNCmMqwZ48698213 = CtVtBLFtspvLkNCmMqwZ25946616;     CtVtBLFtspvLkNCmMqwZ25946616 = CtVtBLFtspvLkNCmMqwZ34263765;     CtVtBLFtspvLkNCmMqwZ34263765 = CtVtBLFtspvLkNCmMqwZ17304368;     CtVtBLFtspvLkNCmMqwZ17304368 = CtVtBLFtspvLkNCmMqwZ11287536;     CtVtBLFtspvLkNCmMqwZ11287536 = CtVtBLFtspvLkNCmMqwZ46831828;     CtVtBLFtspvLkNCmMqwZ46831828 = CtVtBLFtspvLkNCmMqwZ74150083;     CtVtBLFtspvLkNCmMqwZ74150083 = CtVtBLFtspvLkNCmMqwZ33540747;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void gZkOqfDggTeIgRRxlQPM64698471() {     float xDFzKcaHFjORwGHRyORw4192414 = -133718971;    float xDFzKcaHFjORwGHRyORw17145029 = -754940494;    float xDFzKcaHFjORwGHRyORw85334354 = -251892243;    float xDFzKcaHFjORwGHRyORw92666823 = 42090063;    float xDFzKcaHFjORwGHRyORw95822691 = -353126730;    float xDFzKcaHFjORwGHRyORw4090454 = -223724770;    float xDFzKcaHFjORwGHRyORw57890068 = -466664897;    float xDFzKcaHFjORwGHRyORw63480221 = 81896910;    float xDFzKcaHFjORwGHRyORw39131003 = -781005632;    float xDFzKcaHFjORwGHRyORw43436911 = -786997822;    float xDFzKcaHFjORwGHRyORw32694489 = -691556063;    float xDFzKcaHFjORwGHRyORw94287207 = -355972216;    float xDFzKcaHFjORwGHRyORw657567 = -40220144;    float xDFzKcaHFjORwGHRyORw50296934 = -127240603;    float xDFzKcaHFjORwGHRyORw81013730 = -767880125;    float xDFzKcaHFjORwGHRyORw98818125 = -983807300;    float xDFzKcaHFjORwGHRyORw99343365 = 75358690;    float xDFzKcaHFjORwGHRyORw74503722 = 26240018;    float xDFzKcaHFjORwGHRyORw95482845 = -195842826;    float xDFzKcaHFjORwGHRyORw98194035 = -996451667;    float xDFzKcaHFjORwGHRyORw43423701 = -170127572;    float xDFzKcaHFjORwGHRyORw10783196 = 61991839;    float xDFzKcaHFjORwGHRyORw91842174 = -588876227;    float xDFzKcaHFjORwGHRyORw2301859 = -346405286;    float xDFzKcaHFjORwGHRyORw42465168 = -459050840;    float xDFzKcaHFjORwGHRyORw77688931 = 82943314;    float xDFzKcaHFjORwGHRyORw89390447 = -265436019;    float xDFzKcaHFjORwGHRyORw28654842 = -80263151;    float xDFzKcaHFjORwGHRyORw43054923 = -976229380;    float xDFzKcaHFjORwGHRyORw47278204 = -983741428;    float xDFzKcaHFjORwGHRyORw55795273 = -110941830;    float xDFzKcaHFjORwGHRyORw98538853 = -870119450;    float xDFzKcaHFjORwGHRyORw1434753 = -677598693;    float xDFzKcaHFjORwGHRyORw42278725 = -742453257;    float xDFzKcaHFjORwGHRyORw42144225 = 13177782;    float xDFzKcaHFjORwGHRyORw78569694 = -177708029;    float xDFzKcaHFjORwGHRyORw41700615 = -502600389;    float xDFzKcaHFjORwGHRyORw41383239 = -595861267;    float xDFzKcaHFjORwGHRyORw54512520 = -248435008;    float xDFzKcaHFjORwGHRyORw54010943 = -470051618;    float xDFzKcaHFjORwGHRyORw12975605 = -572825424;    float xDFzKcaHFjORwGHRyORw41512765 = -336057356;    float xDFzKcaHFjORwGHRyORw45414336 = -159609199;    float xDFzKcaHFjORwGHRyORw25087302 = -981958688;    float xDFzKcaHFjORwGHRyORw6522077 = -772358858;    float xDFzKcaHFjORwGHRyORw71605276 = 41907456;    float xDFzKcaHFjORwGHRyORw80656979 = -214430552;    float xDFzKcaHFjORwGHRyORw76162504 = -869761981;    float xDFzKcaHFjORwGHRyORw97179496 = -831953503;    float xDFzKcaHFjORwGHRyORw15924172 = -583772397;    float xDFzKcaHFjORwGHRyORw28530603 = -54214526;    float xDFzKcaHFjORwGHRyORw68476211 = -694277890;    float xDFzKcaHFjORwGHRyORw39826167 = -114980266;    float xDFzKcaHFjORwGHRyORw5127010 = -591646033;    float xDFzKcaHFjORwGHRyORw21274771 = 66158427;    float xDFzKcaHFjORwGHRyORw93409217 = -95710810;    float xDFzKcaHFjORwGHRyORw25302854 = -66064268;    float xDFzKcaHFjORwGHRyORw83032496 = -905486957;    float xDFzKcaHFjORwGHRyORw50201655 = -498859097;    float xDFzKcaHFjORwGHRyORw18133760 = -336070045;    float xDFzKcaHFjORwGHRyORw14700007 = -958288752;    float xDFzKcaHFjORwGHRyORw29235226 = -286401746;    float xDFzKcaHFjORwGHRyORw20425299 = 58126289;    float xDFzKcaHFjORwGHRyORw91852798 = -797264204;    float xDFzKcaHFjORwGHRyORw87641637 = -576055992;    float xDFzKcaHFjORwGHRyORw34155635 = -821436613;    float xDFzKcaHFjORwGHRyORw92852455 = -678373523;    float xDFzKcaHFjORwGHRyORw58378842 = -297766887;    float xDFzKcaHFjORwGHRyORw8152709 = -40418386;    float xDFzKcaHFjORwGHRyORw2444037 = -490172096;    float xDFzKcaHFjORwGHRyORw57117511 = -381206912;    float xDFzKcaHFjORwGHRyORw57960127 = -328780044;    float xDFzKcaHFjORwGHRyORw19991202 = -725324974;    float xDFzKcaHFjORwGHRyORw41471902 = -725791208;    float xDFzKcaHFjORwGHRyORw85218430 = -323626244;    float xDFzKcaHFjORwGHRyORw1910937 = -834070217;    float xDFzKcaHFjORwGHRyORw65368860 = -778398963;    float xDFzKcaHFjORwGHRyORw66754873 = -606917540;    float xDFzKcaHFjORwGHRyORw95779781 = -574046428;    float xDFzKcaHFjORwGHRyORw70859892 = -400958297;    float xDFzKcaHFjORwGHRyORw97031952 = -702626135;    float xDFzKcaHFjORwGHRyORw13227943 = -395674039;    float xDFzKcaHFjORwGHRyORw31475345 = -248309648;    float xDFzKcaHFjORwGHRyORw27130751 = -292456983;    float xDFzKcaHFjORwGHRyORw18747601 = -829526903;    float xDFzKcaHFjORwGHRyORw87319061 = -416663940;    float xDFzKcaHFjORwGHRyORw58712687 = -655139185;    float xDFzKcaHFjORwGHRyORw96307742 = 14047339;    float xDFzKcaHFjORwGHRyORw21003954 = -708611685;    float xDFzKcaHFjORwGHRyORw48735007 = -891111408;    float xDFzKcaHFjORwGHRyORw53266840 = -11643762;    float xDFzKcaHFjORwGHRyORw58668119 = -597113432;    float xDFzKcaHFjORwGHRyORw91181583 = 2997829;    float xDFzKcaHFjORwGHRyORw36378761 = -912364964;    float xDFzKcaHFjORwGHRyORw39310937 = -511762867;    float xDFzKcaHFjORwGHRyORw83740378 = -186423679;    float xDFzKcaHFjORwGHRyORw21087467 = -294183646;    float xDFzKcaHFjORwGHRyORw53561537 = -362344995;    float xDFzKcaHFjORwGHRyORw37445664 = -305902696;    float xDFzKcaHFjORwGHRyORw72366441 = -133718971;     xDFzKcaHFjORwGHRyORw4192414 = xDFzKcaHFjORwGHRyORw17145029;     xDFzKcaHFjORwGHRyORw17145029 = xDFzKcaHFjORwGHRyORw85334354;     xDFzKcaHFjORwGHRyORw85334354 = xDFzKcaHFjORwGHRyORw92666823;     xDFzKcaHFjORwGHRyORw92666823 = xDFzKcaHFjORwGHRyORw95822691;     xDFzKcaHFjORwGHRyORw95822691 = xDFzKcaHFjORwGHRyORw4090454;     xDFzKcaHFjORwGHRyORw4090454 = xDFzKcaHFjORwGHRyORw57890068;     xDFzKcaHFjORwGHRyORw57890068 = xDFzKcaHFjORwGHRyORw63480221;     xDFzKcaHFjORwGHRyORw63480221 = xDFzKcaHFjORwGHRyORw39131003;     xDFzKcaHFjORwGHRyORw39131003 = xDFzKcaHFjORwGHRyORw43436911;     xDFzKcaHFjORwGHRyORw43436911 = xDFzKcaHFjORwGHRyORw32694489;     xDFzKcaHFjORwGHRyORw32694489 = xDFzKcaHFjORwGHRyORw94287207;     xDFzKcaHFjORwGHRyORw94287207 = xDFzKcaHFjORwGHRyORw657567;     xDFzKcaHFjORwGHRyORw657567 = xDFzKcaHFjORwGHRyORw50296934;     xDFzKcaHFjORwGHRyORw50296934 = xDFzKcaHFjORwGHRyORw81013730;     xDFzKcaHFjORwGHRyORw81013730 = xDFzKcaHFjORwGHRyORw98818125;     xDFzKcaHFjORwGHRyORw98818125 = xDFzKcaHFjORwGHRyORw99343365;     xDFzKcaHFjORwGHRyORw99343365 = xDFzKcaHFjORwGHRyORw74503722;     xDFzKcaHFjORwGHRyORw74503722 = xDFzKcaHFjORwGHRyORw95482845;     xDFzKcaHFjORwGHRyORw95482845 = xDFzKcaHFjORwGHRyORw98194035;     xDFzKcaHFjORwGHRyORw98194035 = xDFzKcaHFjORwGHRyORw43423701;     xDFzKcaHFjORwGHRyORw43423701 = xDFzKcaHFjORwGHRyORw10783196;     xDFzKcaHFjORwGHRyORw10783196 = xDFzKcaHFjORwGHRyORw91842174;     xDFzKcaHFjORwGHRyORw91842174 = xDFzKcaHFjORwGHRyORw2301859;     xDFzKcaHFjORwGHRyORw2301859 = xDFzKcaHFjORwGHRyORw42465168;     xDFzKcaHFjORwGHRyORw42465168 = xDFzKcaHFjORwGHRyORw77688931;     xDFzKcaHFjORwGHRyORw77688931 = xDFzKcaHFjORwGHRyORw89390447;     xDFzKcaHFjORwGHRyORw89390447 = xDFzKcaHFjORwGHRyORw28654842;     xDFzKcaHFjORwGHRyORw28654842 = xDFzKcaHFjORwGHRyORw43054923;     xDFzKcaHFjORwGHRyORw43054923 = xDFzKcaHFjORwGHRyORw47278204;     xDFzKcaHFjORwGHRyORw47278204 = xDFzKcaHFjORwGHRyORw55795273;     xDFzKcaHFjORwGHRyORw55795273 = xDFzKcaHFjORwGHRyORw98538853;     xDFzKcaHFjORwGHRyORw98538853 = xDFzKcaHFjORwGHRyORw1434753;     xDFzKcaHFjORwGHRyORw1434753 = xDFzKcaHFjORwGHRyORw42278725;     xDFzKcaHFjORwGHRyORw42278725 = xDFzKcaHFjORwGHRyORw42144225;     xDFzKcaHFjORwGHRyORw42144225 = xDFzKcaHFjORwGHRyORw78569694;     xDFzKcaHFjORwGHRyORw78569694 = xDFzKcaHFjORwGHRyORw41700615;     xDFzKcaHFjORwGHRyORw41700615 = xDFzKcaHFjORwGHRyORw41383239;     xDFzKcaHFjORwGHRyORw41383239 = xDFzKcaHFjORwGHRyORw54512520;     xDFzKcaHFjORwGHRyORw54512520 = xDFzKcaHFjORwGHRyORw54010943;     xDFzKcaHFjORwGHRyORw54010943 = xDFzKcaHFjORwGHRyORw12975605;     xDFzKcaHFjORwGHRyORw12975605 = xDFzKcaHFjORwGHRyORw41512765;     xDFzKcaHFjORwGHRyORw41512765 = xDFzKcaHFjORwGHRyORw45414336;     xDFzKcaHFjORwGHRyORw45414336 = xDFzKcaHFjORwGHRyORw25087302;     xDFzKcaHFjORwGHRyORw25087302 = xDFzKcaHFjORwGHRyORw6522077;     xDFzKcaHFjORwGHRyORw6522077 = xDFzKcaHFjORwGHRyORw71605276;     xDFzKcaHFjORwGHRyORw71605276 = xDFzKcaHFjORwGHRyORw80656979;     xDFzKcaHFjORwGHRyORw80656979 = xDFzKcaHFjORwGHRyORw76162504;     xDFzKcaHFjORwGHRyORw76162504 = xDFzKcaHFjORwGHRyORw97179496;     xDFzKcaHFjORwGHRyORw97179496 = xDFzKcaHFjORwGHRyORw15924172;     xDFzKcaHFjORwGHRyORw15924172 = xDFzKcaHFjORwGHRyORw28530603;     xDFzKcaHFjORwGHRyORw28530603 = xDFzKcaHFjORwGHRyORw68476211;     xDFzKcaHFjORwGHRyORw68476211 = xDFzKcaHFjORwGHRyORw39826167;     xDFzKcaHFjORwGHRyORw39826167 = xDFzKcaHFjORwGHRyORw5127010;     xDFzKcaHFjORwGHRyORw5127010 = xDFzKcaHFjORwGHRyORw21274771;     xDFzKcaHFjORwGHRyORw21274771 = xDFzKcaHFjORwGHRyORw93409217;     xDFzKcaHFjORwGHRyORw93409217 = xDFzKcaHFjORwGHRyORw25302854;     xDFzKcaHFjORwGHRyORw25302854 = xDFzKcaHFjORwGHRyORw83032496;     xDFzKcaHFjORwGHRyORw83032496 = xDFzKcaHFjORwGHRyORw50201655;     xDFzKcaHFjORwGHRyORw50201655 = xDFzKcaHFjORwGHRyORw18133760;     xDFzKcaHFjORwGHRyORw18133760 = xDFzKcaHFjORwGHRyORw14700007;     xDFzKcaHFjORwGHRyORw14700007 = xDFzKcaHFjORwGHRyORw29235226;     xDFzKcaHFjORwGHRyORw29235226 = xDFzKcaHFjORwGHRyORw20425299;     xDFzKcaHFjORwGHRyORw20425299 = xDFzKcaHFjORwGHRyORw91852798;     xDFzKcaHFjORwGHRyORw91852798 = xDFzKcaHFjORwGHRyORw87641637;     xDFzKcaHFjORwGHRyORw87641637 = xDFzKcaHFjORwGHRyORw34155635;     xDFzKcaHFjORwGHRyORw34155635 = xDFzKcaHFjORwGHRyORw92852455;     xDFzKcaHFjORwGHRyORw92852455 = xDFzKcaHFjORwGHRyORw58378842;     xDFzKcaHFjORwGHRyORw58378842 = xDFzKcaHFjORwGHRyORw8152709;     xDFzKcaHFjORwGHRyORw8152709 = xDFzKcaHFjORwGHRyORw2444037;     xDFzKcaHFjORwGHRyORw2444037 = xDFzKcaHFjORwGHRyORw57117511;     xDFzKcaHFjORwGHRyORw57117511 = xDFzKcaHFjORwGHRyORw57960127;     xDFzKcaHFjORwGHRyORw57960127 = xDFzKcaHFjORwGHRyORw19991202;     xDFzKcaHFjORwGHRyORw19991202 = xDFzKcaHFjORwGHRyORw41471902;     xDFzKcaHFjORwGHRyORw41471902 = xDFzKcaHFjORwGHRyORw85218430;     xDFzKcaHFjORwGHRyORw85218430 = xDFzKcaHFjORwGHRyORw1910937;     xDFzKcaHFjORwGHRyORw1910937 = xDFzKcaHFjORwGHRyORw65368860;     xDFzKcaHFjORwGHRyORw65368860 = xDFzKcaHFjORwGHRyORw66754873;     xDFzKcaHFjORwGHRyORw66754873 = xDFzKcaHFjORwGHRyORw95779781;     xDFzKcaHFjORwGHRyORw95779781 = xDFzKcaHFjORwGHRyORw70859892;     xDFzKcaHFjORwGHRyORw70859892 = xDFzKcaHFjORwGHRyORw97031952;     xDFzKcaHFjORwGHRyORw97031952 = xDFzKcaHFjORwGHRyORw13227943;     xDFzKcaHFjORwGHRyORw13227943 = xDFzKcaHFjORwGHRyORw31475345;     xDFzKcaHFjORwGHRyORw31475345 = xDFzKcaHFjORwGHRyORw27130751;     xDFzKcaHFjORwGHRyORw27130751 = xDFzKcaHFjORwGHRyORw18747601;     xDFzKcaHFjORwGHRyORw18747601 = xDFzKcaHFjORwGHRyORw87319061;     xDFzKcaHFjORwGHRyORw87319061 = xDFzKcaHFjORwGHRyORw58712687;     xDFzKcaHFjORwGHRyORw58712687 = xDFzKcaHFjORwGHRyORw96307742;     xDFzKcaHFjORwGHRyORw96307742 = xDFzKcaHFjORwGHRyORw21003954;     xDFzKcaHFjORwGHRyORw21003954 = xDFzKcaHFjORwGHRyORw48735007;     xDFzKcaHFjORwGHRyORw48735007 = xDFzKcaHFjORwGHRyORw53266840;     xDFzKcaHFjORwGHRyORw53266840 = xDFzKcaHFjORwGHRyORw58668119;     xDFzKcaHFjORwGHRyORw58668119 = xDFzKcaHFjORwGHRyORw91181583;     xDFzKcaHFjORwGHRyORw91181583 = xDFzKcaHFjORwGHRyORw36378761;     xDFzKcaHFjORwGHRyORw36378761 = xDFzKcaHFjORwGHRyORw39310937;     xDFzKcaHFjORwGHRyORw39310937 = xDFzKcaHFjORwGHRyORw83740378;     xDFzKcaHFjORwGHRyORw83740378 = xDFzKcaHFjORwGHRyORw21087467;     xDFzKcaHFjORwGHRyORw21087467 = xDFzKcaHFjORwGHRyORw53561537;     xDFzKcaHFjORwGHRyORw53561537 = xDFzKcaHFjORwGHRyORw37445664;     xDFzKcaHFjORwGHRyORw37445664 = xDFzKcaHFjORwGHRyORw72366441;     xDFzKcaHFjORwGHRyORw72366441 = xDFzKcaHFjORwGHRyORw4192414;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void CSGvGaOLMkOIpGipvnJC71357216() {     float rwIkHaLzmODNsPGAMYPd4185943 = -6243113;    float rwIkHaLzmODNsPGAMYPd55717345 = -797535620;    float rwIkHaLzmODNsPGAMYPd86647566 = -209020856;    float rwIkHaLzmODNsPGAMYPd77746684 = -878450767;    float rwIkHaLzmODNsPGAMYPd4662794 = -821482052;    float rwIkHaLzmODNsPGAMYPd38895886 = 76842418;    float rwIkHaLzmODNsPGAMYPd26437404 = -171201364;    float rwIkHaLzmODNsPGAMYPd32694272 = -722280407;    float rwIkHaLzmODNsPGAMYPd21871167 = -494651702;    float rwIkHaLzmODNsPGAMYPd40076424 = -423804693;    float rwIkHaLzmODNsPGAMYPd53015094 = -651028135;    float rwIkHaLzmODNsPGAMYPd8578885 = -91450579;    float rwIkHaLzmODNsPGAMYPd32510481 = -61878618;    float rwIkHaLzmODNsPGAMYPd4141154 = -472393655;    float rwIkHaLzmODNsPGAMYPd26185840 = -730592977;    float rwIkHaLzmODNsPGAMYPd73382673 = -216602951;    float rwIkHaLzmODNsPGAMYPd55196125 = -844255880;    float rwIkHaLzmODNsPGAMYPd97504255 = -370831604;    float rwIkHaLzmODNsPGAMYPd15911115 = -813825093;    float rwIkHaLzmODNsPGAMYPd70267798 = -508476119;    float rwIkHaLzmODNsPGAMYPd23093125 = -980593946;    float rwIkHaLzmODNsPGAMYPd89079461 = -448000045;    float rwIkHaLzmODNsPGAMYPd28720572 = 52165931;    float rwIkHaLzmODNsPGAMYPd45951966 = -227325809;    float rwIkHaLzmODNsPGAMYPd70907858 = -885744694;    float rwIkHaLzmODNsPGAMYPd94263008 = -925262614;    float rwIkHaLzmODNsPGAMYPd59612029 = -566235732;    float rwIkHaLzmODNsPGAMYPd64058367 = -689230140;    float rwIkHaLzmODNsPGAMYPd55733370 = -522299695;    float rwIkHaLzmODNsPGAMYPd23368074 = -659172802;    float rwIkHaLzmODNsPGAMYPd89207952 = -498035065;    float rwIkHaLzmODNsPGAMYPd83262711 = -161941212;    float rwIkHaLzmODNsPGAMYPd86887175 = -879542909;    float rwIkHaLzmODNsPGAMYPd42561562 = -619134990;    float rwIkHaLzmODNsPGAMYPd24502263 = -246136142;    float rwIkHaLzmODNsPGAMYPd21397884 = -257473775;    float rwIkHaLzmODNsPGAMYPd53641982 = -602253767;    float rwIkHaLzmODNsPGAMYPd11300919 = -591123946;    float rwIkHaLzmODNsPGAMYPd89826117 = -961116424;    float rwIkHaLzmODNsPGAMYPd25883836 = -79483304;    float rwIkHaLzmODNsPGAMYPd831273 = -660769323;    float rwIkHaLzmODNsPGAMYPd27210426 = -529849438;    float rwIkHaLzmODNsPGAMYPd5697696 = -718739453;    float rwIkHaLzmODNsPGAMYPd84319199 = -538923131;    float rwIkHaLzmODNsPGAMYPd90540128 = -145618260;    float rwIkHaLzmODNsPGAMYPd12033245 = -870137215;    float rwIkHaLzmODNsPGAMYPd8354772 = -695711174;    float rwIkHaLzmODNsPGAMYPd51546086 = -378523337;    float rwIkHaLzmODNsPGAMYPd15837886 = -395717365;    float rwIkHaLzmODNsPGAMYPd35969595 = -170797283;    float rwIkHaLzmODNsPGAMYPd41789972 = -732769533;    float rwIkHaLzmODNsPGAMYPd83023469 = 23576362;    float rwIkHaLzmODNsPGAMYPd15274515 = -7034274;    float rwIkHaLzmODNsPGAMYPd15377281 = -23565884;    float rwIkHaLzmODNsPGAMYPd48264091 = -790740850;    float rwIkHaLzmODNsPGAMYPd15106481 = -558243069;    float rwIkHaLzmODNsPGAMYPd26996774 = -749701551;    float rwIkHaLzmODNsPGAMYPd40695600 = -981695048;    float rwIkHaLzmODNsPGAMYPd6838827 = -992706074;    float rwIkHaLzmODNsPGAMYPd10399785 = -896219438;    float rwIkHaLzmODNsPGAMYPd79283857 = -356921851;    float rwIkHaLzmODNsPGAMYPd62379037 = -481971225;    float rwIkHaLzmODNsPGAMYPd76960902 = -99980713;    float rwIkHaLzmODNsPGAMYPd98503092 = -835478900;    float rwIkHaLzmODNsPGAMYPd50868472 = -925769629;    float rwIkHaLzmODNsPGAMYPd69752383 = -389086923;    float rwIkHaLzmODNsPGAMYPd21691710 = -211907670;    float rwIkHaLzmODNsPGAMYPd89948918 = -442743629;    float rwIkHaLzmODNsPGAMYPd79638890 = -126257513;    float rwIkHaLzmODNsPGAMYPd4787956 = -373119202;    float rwIkHaLzmODNsPGAMYPd19740691 = -614349185;    float rwIkHaLzmODNsPGAMYPd43895207 = -153131935;    float rwIkHaLzmODNsPGAMYPd7678139 = -409715180;    float rwIkHaLzmODNsPGAMYPd90027279 = -634341789;    float rwIkHaLzmODNsPGAMYPd69436526 = -847706796;    float rwIkHaLzmODNsPGAMYPd95882699 = -350744508;    float rwIkHaLzmODNsPGAMYPd83381765 = -729260592;    float rwIkHaLzmODNsPGAMYPd44401372 = -408910938;    float rwIkHaLzmODNsPGAMYPd55411838 = 18292451;    float rwIkHaLzmODNsPGAMYPd58874613 = 84392521;    float rwIkHaLzmODNsPGAMYPd85908237 = -129551441;    float rwIkHaLzmODNsPGAMYPd8065944 = -87712396;    float rwIkHaLzmODNsPGAMYPd48220481 = -193512775;    float rwIkHaLzmODNsPGAMYPd19763775 = -251502412;    float rwIkHaLzmODNsPGAMYPd81578101 = -926403269;    float rwIkHaLzmODNsPGAMYPd6184484 = -421611427;    float rwIkHaLzmODNsPGAMYPd67988196 = -54906938;    float rwIkHaLzmODNsPGAMYPd71509895 = -755977026;    float rwIkHaLzmODNsPGAMYPd94297470 = -828394140;    float rwIkHaLzmODNsPGAMYPd9395783 = -687893074;    float rwIkHaLzmODNsPGAMYPd94401109 = -507772225;    float rwIkHaLzmODNsPGAMYPd12946383 = -620558719;    float rwIkHaLzmODNsPGAMYPd4462092 = -598417873;    float rwIkHaLzmODNsPGAMYPd79426332 = 35103013;    float rwIkHaLzmODNsPGAMYPd46599979 = -722561454;    float rwIkHaLzmODNsPGAMYPd38452235 = -78798099;    float rwIkHaLzmODNsPGAMYPd50249524 = -329868726;    float rwIkHaLzmODNsPGAMYPd7194604 = -883260553;    float rwIkHaLzmODNsPGAMYPd33450727 = -613153503;    float rwIkHaLzmODNsPGAMYPd20787746 = -6243113;     rwIkHaLzmODNsPGAMYPd4185943 = rwIkHaLzmODNsPGAMYPd55717345;     rwIkHaLzmODNsPGAMYPd55717345 = rwIkHaLzmODNsPGAMYPd86647566;     rwIkHaLzmODNsPGAMYPd86647566 = rwIkHaLzmODNsPGAMYPd77746684;     rwIkHaLzmODNsPGAMYPd77746684 = rwIkHaLzmODNsPGAMYPd4662794;     rwIkHaLzmODNsPGAMYPd4662794 = rwIkHaLzmODNsPGAMYPd38895886;     rwIkHaLzmODNsPGAMYPd38895886 = rwIkHaLzmODNsPGAMYPd26437404;     rwIkHaLzmODNsPGAMYPd26437404 = rwIkHaLzmODNsPGAMYPd32694272;     rwIkHaLzmODNsPGAMYPd32694272 = rwIkHaLzmODNsPGAMYPd21871167;     rwIkHaLzmODNsPGAMYPd21871167 = rwIkHaLzmODNsPGAMYPd40076424;     rwIkHaLzmODNsPGAMYPd40076424 = rwIkHaLzmODNsPGAMYPd53015094;     rwIkHaLzmODNsPGAMYPd53015094 = rwIkHaLzmODNsPGAMYPd8578885;     rwIkHaLzmODNsPGAMYPd8578885 = rwIkHaLzmODNsPGAMYPd32510481;     rwIkHaLzmODNsPGAMYPd32510481 = rwIkHaLzmODNsPGAMYPd4141154;     rwIkHaLzmODNsPGAMYPd4141154 = rwIkHaLzmODNsPGAMYPd26185840;     rwIkHaLzmODNsPGAMYPd26185840 = rwIkHaLzmODNsPGAMYPd73382673;     rwIkHaLzmODNsPGAMYPd73382673 = rwIkHaLzmODNsPGAMYPd55196125;     rwIkHaLzmODNsPGAMYPd55196125 = rwIkHaLzmODNsPGAMYPd97504255;     rwIkHaLzmODNsPGAMYPd97504255 = rwIkHaLzmODNsPGAMYPd15911115;     rwIkHaLzmODNsPGAMYPd15911115 = rwIkHaLzmODNsPGAMYPd70267798;     rwIkHaLzmODNsPGAMYPd70267798 = rwIkHaLzmODNsPGAMYPd23093125;     rwIkHaLzmODNsPGAMYPd23093125 = rwIkHaLzmODNsPGAMYPd89079461;     rwIkHaLzmODNsPGAMYPd89079461 = rwIkHaLzmODNsPGAMYPd28720572;     rwIkHaLzmODNsPGAMYPd28720572 = rwIkHaLzmODNsPGAMYPd45951966;     rwIkHaLzmODNsPGAMYPd45951966 = rwIkHaLzmODNsPGAMYPd70907858;     rwIkHaLzmODNsPGAMYPd70907858 = rwIkHaLzmODNsPGAMYPd94263008;     rwIkHaLzmODNsPGAMYPd94263008 = rwIkHaLzmODNsPGAMYPd59612029;     rwIkHaLzmODNsPGAMYPd59612029 = rwIkHaLzmODNsPGAMYPd64058367;     rwIkHaLzmODNsPGAMYPd64058367 = rwIkHaLzmODNsPGAMYPd55733370;     rwIkHaLzmODNsPGAMYPd55733370 = rwIkHaLzmODNsPGAMYPd23368074;     rwIkHaLzmODNsPGAMYPd23368074 = rwIkHaLzmODNsPGAMYPd89207952;     rwIkHaLzmODNsPGAMYPd89207952 = rwIkHaLzmODNsPGAMYPd83262711;     rwIkHaLzmODNsPGAMYPd83262711 = rwIkHaLzmODNsPGAMYPd86887175;     rwIkHaLzmODNsPGAMYPd86887175 = rwIkHaLzmODNsPGAMYPd42561562;     rwIkHaLzmODNsPGAMYPd42561562 = rwIkHaLzmODNsPGAMYPd24502263;     rwIkHaLzmODNsPGAMYPd24502263 = rwIkHaLzmODNsPGAMYPd21397884;     rwIkHaLzmODNsPGAMYPd21397884 = rwIkHaLzmODNsPGAMYPd53641982;     rwIkHaLzmODNsPGAMYPd53641982 = rwIkHaLzmODNsPGAMYPd11300919;     rwIkHaLzmODNsPGAMYPd11300919 = rwIkHaLzmODNsPGAMYPd89826117;     rwIkHaLzmODNsPGAMYPd89826117 = rwIkHaLzmODNsPGAMYPd25883836;     rwIkHaLzmODNsPGAMYPd25883836 = rwIkHaLzmODNsPGAMYPd831273;     rwIkHaLzmODNsPGAMYPd831273 = rwIkHaLzmODNsPGAMYPd27210426;     rwIkHaLzmODNsPGAMYPd27210426 = rwIkHaLzmODNsPGAMYPd5697696;     rwIkHaLzmODNsPGAMYPd5697696 = rwIkHaLzmODNsPGAMYPd84319199;     rwIkHaLzmODNsPGAMYPd84319199 = rwIkHaLzmODNsPGAMYPd90540128;     rwIkHaLzmODNsPGAMYPd90540128 = rwIkHaLzmODNsPGAMYPd12033245;     rwIkHaLzmODNsPGAMYPd12033245 = rwIkHaLzmODNsPGAMYPd8354772;     rwIkHaLzmODNsPGAMYPd8354772 = rwIkHaLzmODNsPGAMYPd51546086;     rwIkHaLzmODNsPGAMYPd51546086 = rwIkHaLzmODNsPGAMYPd15837886;     rwIkHaLzmODNsPGAMYPd15837886 = rwIkHaLzmODNsPGAMYPd35969595;     rwIkHaLzmODNsPGAMYPd35969595 = rwIkHaLzmODNsPGAMYPd41789972;     rwIkHaLzmODNsPGAMYPd41789972 = rwIkHaLzmODNsPGAMYPd83023469;     rwIkHaLzmODNsPGAMYPd83023469 = rwIkHaLzmODNsPGAMYPd15274515;     rwIkHaLzmODNsPGAMYPd15274515 = rwIkHaLzmODNsPGAMYPd15377281;     rwIkHaLzmODNsPGAMYPd15377281 = rwIkHaLzmODNsPGAMYPd48264091;     rwIkHaLzmODNsPGAMYPd48264091 = rwIkHaLzmODNsPGAMYPd15106481;     rwIkHaLzmODNsPGAMYPd15106481 = rwIkHaLzmODNsPGAMYPd26996774;     rwIkHaLzmODNsPGAMYPd26996774 = rwIkHaLzmODNsPGAMYPd40695600;     rwIkHaLzmODNsPGAMYPd40695600 = rwIkHaLzmODNsPGAMYPd6838827;     rwIkHaLzmODNsPGAMYPd6838827 = rwIkHaLzmODNsPGAMYPd10399785;     rwIkHaLzmODNsPGAMYPd10399785 = rwIkHaLzmODNsPGAMYPd79283857;     rwIkHaLzmODNsPGAMYPd79283857 = rwIkHaLzmODNsPGAMYPd62379037;     rwIkHaLzmODNsPGAMYPd62379037 = rwIkHaLzmODNsPGAMYPd76960902;     rwIkHaLzmODNsPGAMYPd76960902 = rwIkHaLzmODNsPGAMYPd98503092;     rwIkHaLzmODNsPGAMYPd98503092 = rwIkHaLzmODNsPGAMYPd50868472;     rwIkHaLzmODNsPGAMYPd50868472 = rwIkHaLzmODNsPGAMYPd69752383;     rwIkHaLzmODNsPGAMYPd69752383 = rwIkHaLzmODNsPGAMYPd21691710;     rwIkHaLzmODNsPGAMYPd21691710 = rwIkHaLzmODNsPGAMYPd89948918;     rwIkHaLzmODNsPGAMYPd89948918 = rwIkHaLzmODNsPGAMYPd79638890;     rwIkHaLzmODNsPGAMYPd79638890 = rwIkHaLzmODNsPGAMYPd4787956;     rwIkHaLzmODNsPGAMYPd4787956 = rwIkHaLzmODNsPGAMYPd19740691;     rwIkHaLzmODNsPGAMYPd19740691 = rwIkHaLzmODNsPGAMYPd43895207;     rwIkHaLzmODNsPGAMYPd43895207 = rwIkHaLzmODNsPGAMYPd7678139;     rwIkHaLzmODNsPGAMYPd7678139 = rwIkHaLzmODNsPGAMYPd90027279;     rwIkHaLzmODNsPGAMYPd90027279 = rwIkHaLzmODNsPGAMYPd69436526;     rwIkHaLzmODNsPGAMYPd69436526 = rwIkHaLzmODNsPGAMYPd95882699;     rwIkHaLzmODNsPGAMYPd95882699 = rwIkHaLzmODNsPGAMYPd83381765;     rwIkHaLzmODNsPGAMYPd83381765 = rwIkHaLzmODNsPGAMYPd44401372;     rwIkHaLzmODNsPGAMYPd44401372 = rwIkHaLzmODNsPGAMYPd55411838;     rwIkHaLzmODNsPGAMYPd55411838 = rwIkHaLzmODNsPGAMYPd58874613;     rwIkHaLzmODNsPGAMYPd58874613 = rwIkHaLzmODNsPGAMYPd85908237;     rwIkHaLzmODNsPGAMYPd85908237 = rwIkHaLzmODNsPGAMYPd8065944;     rwIkHaLzmODNsPGAMYPd8065944 = rwIkHaLzmODNsPGAMYPd48220481;     rwIkHaLzmODNsPGAMYPd48220481 = rwIkHaLzmODNsPGAMYPd19763775;     rwIkHaLzmODNsPGAMYPd19763775 = rwIkHaLzmODNsPGAMYPd81578101;     rwIkHaLzmODNsPGAMYPd81578101 = rwIkHaLzmODNsPGAMYPd6184484;     rwIkHaLzmODNsPGAMYPd6184484 = rwIkHaLzmODNsPGAMYPd67988196;     rwIkHaLzmODNsPGAMYPd67988196 = rwIkHaLzmODNsPGAMYPd71509895;     rwIkHaLzmODNsPGAMYPd71509895 = rwIkHaLzmODNsPGAMYPd94297470;     rwIkHaLzmODNsPGAMYPd94297470 = rwIkHaLzmODNsPGAMYPd9395783;     rwIkHaLzmODNsPGAMYPd9395783 = rwIkHaLzmODNsPGAMYPd94401109;     rwIkHaLzmODNsPGAMYPd94401109 = rwIkHaLzmODNsPGAMYPd12946383;     rwIkHaLzmODNsPGAMYPd12946383 = rwIkHaLzmODNsPGAMYPd4462092;     rwIkHaLzmODNsPGAMYPd4462092 = rwIkHaLzmODNsPGAMYPd79426332;     rwIkHaLzmODNsPGAMYPd79426332 = rwIkHaLzmODNsPGAMYPd46599979;     rwIkHaLzmODNsPGAMYPd46599979 = rwIkHaLzmODNsPGAMYPd38452235;     rwIkHaLzmODNsPGAMYPd38452235 = rwIkHaLzmODNsPGAMYPd50249524;     rwIkHaLzmODNsPGAMYPd50249524 = rwIkHaLzmODNsPGAMYPd7194604;     rwIkHaLzmODNsPGAMYPd7194604 = rwIkHaLzmODNsPGAMYPd33450727;     rwIkHaLzmODNsPGAMYPd33450727 = rwIkHaLzmODNsPGAMYPd20787746;     rwIkHaLzmODNsPGAMYPd20787746 = rwIkHaLzmODNsPGAMYPd4185943;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void afGjcocgfHIiVenAlxGe22286787() {     float NJuFRVBeoBlpUHRbjGui79772837 = -288145178;    float NJuFRVBeoBlpUHRbjGui17471410 = 57635385;    float NJuFRVBeoBlpUHRbjGui44198439 = -297200239;    float NJuFRVBeoBlpUHRbjGui27198649 = -655638453;    float NJuFRVBeoBlpUHRbjGui71794772 = -201315508;    float NJuFRVBeoBlpUHRbjGui47203805 = -153280383;    float NJuFRVBeoBlpUHRbjGui89621414 = -2338454;    float NJuFRVBeoBlpUHRbjGui80400883 = -626917957;    float NJuFRVBeoBlpUHRbjGui76515418 = -787464229;    float NJuFRVBeoBlpUHRbjGui60074928 = -24164498;    float NJuFRVBeoBlpUHRbjGui62268496 = -421588003;    float NJuFRVBeoBlpUHRbjGui40323622 = -51289895;    float NJuFRVBeoBlpUHRbjGui51128030 = -95712365;    float NJuFRVBeoBlpUHRbjGui7239816 = -910629685;    float NJuFRVBeoBlpUHRbjGui21161700 = -628460722;    float NJuFRVBeoBlpUHRbjGui87462579 = -294595515;    float NJuFRVBeoBlpUHRbjGui41031052 = -27672032;    float NJuFRVBeoBlpUHRbjGui37870027 = 37158941;    float NJuFRVBeoBlpUHRbjGui97268768 = -892303384;    float NJuFRVBeoBlpUHRbjGui63410824 = -879909381;    float NJuFRVBeoBlpUHRbjGui4185035 = -964677133;    float NJuFRVBeoBlpUHRbjGui46008502 = 84764997;    float NJuFRVBeoBlpUHRbjGui23264770 = -152186714;    float NJuFRVBeoBlpUHRbjGui86753481 = -665944658;    float NJuFRVBeoBlpUHRbjGui43700325 = -71567373;    float NJuFRVBeoBlpUHRbjGui94608626 = 37338326;    float NJuFRVBeoBlpUHRbjGui49444543 = -56848938;    float NJuFRVBeoBlpUHRbjGui19106286 = -216650625;    float NJuFRVBeoBlpUHRbjGui10440971 = -872075966;    float NJuFRVBeoBlpUHRbjGui82668097 = -649224520;    float NJuFRVBeoBlpUHRbjGui2901417 = -729908787;    float NJuFRVBeoBlpUHRbjGui24271679 = -887781605;    float NJuFRVBeoBlpUHRbjGui36711122 = -451877992;    float NJuFRVBeoBlpUHRbjGui96077923 = -981109528;    float NJuFRVBeoBlpUHRbjGui55317742 = -882743573;    float NJuFRVBeoBlpUHRbjGui15482687 = -480127055;    float NJuFRVBeoBlpUHRbjGui25302026 = -232852367;    float NJuFRVBeoBlpUHRbjGui19394185 = -441023102;    float NJuFRVBeoBlpUHRbjGui71921296 = -966284392;    float NJuFRVBeoBlpUHRbjGui41699287 = -922095702;    float NJuFRVBeoBlpUHRbjGui51018636 = -388843538;    float NJuFRVBeoBlpUHRbjGui74795600 = -990439922;    float NJuFRVBeoBlpUHRbjGui64827075 = -57563271;    float NJuFRVBeoBlpUHRbjGui15304504 = -18097619;    float NJuFRVBeoBlpUHRbjGui11516492 = -241324527;    float NJuFRVBeoBlpUHRbjGui68326574 = -614673585;    float NJuFRVBeoBlpUHRbjGui51198778 = -57859879;    float NJuFRVBeoBlpUHRbjGui15812962 = -694515844;    float NJuFRVBeoBlpUHRbjGui36984609 = -329452626;    float NJuFRVBeoBlpUHRbjGui34768234 = -122064215;    float NJuFRVBeoBlpUHRbjGui98611377 = 13210617;    float NJuFRVBeoBlpUHRbjGui56645234 = -883966381;    float NJuFRVBeoBlpUHRbjGui89486031 = -326919321;    float NJuFRVBeoBlpUHRbjGui31713622 = -480649702;    float NJuFRVBeoBlpUHRbjGui18101753 = -226606448;    float NJuFRVBeoBlpUHRbjGui33764336 = -272910175;    float NJuFRVBeoBlpUHRbjGui94206639 = -790177902;    float NJuFRVBeoBlpUHRbjGui57444957 = -631255582;    float NJuFRVBeoBlpUHRbjGui83498324 = -484071081;    float NJuFRVBeoBlpUHRbjGui77186146 = -138653835;    float NJuFRVBeoBlpUHRbjGui97759261 = 3568555;    float NJuFRVBeoBlpUHRbjGui70515128 = -785687829;    float NJuFRVBeoBlpUHRbjGui69959913 = -754841991;    float NJuFRVBeoBlpUHRbjGui93847321 = -38239709;    float NJuFRVBeoBlpUHRbjGui57173512 = -294255712;    float NJuFRVBeoBlpUHRbjGui37996817 = -533806399;    float NJuFRVBeoBlpUHRbjGui3612500 = -599411903;    float NJuFRVBeoBlpUHRbjGui55050106 = -114602837;    float NJuFRVBeoBlpUHRbjGui51922074 = 72113887;    float NJuFRVBeoBlpUHRbjGui5679014 = -48333667;    float NJuFRVBeoBlpUHRbjGui62160553 = 38256851;    float NJuFRVBeoBlpUHRbjGui21636867 = -586648931;    float NJuFRVBeoBlpUHRbjGui65948730 = 3443333;    float NJuFRVBeoBlpUHRbjGui55569482 = -970207682;    float NJuFRVBeoBlpUHRbjGui12392188 = -391065844;    float NJuFRVBeoBlpUHRbjGui29389434 = -974237212;    float NJuFRVBeoBlpUHRbjGui81181426 = -857671733;    float NJuFRVBeoBlpUHRbjGui7960266 = -34089095;    float NJuFRVBeoBlpUHRbjGui75236990 = -324620131;    float NJuFRVBeoBlpUHRbjGui75373750 = -456893788;    float NJuFRVBeoBlpUHRbjGui43409848 = -904801796;    float NJuFRVBeoBlpUHRbjGui33631582 = -362333094;    float NJuFRVBeoBlpUHRbjGui82121677 = -887198000;    float NJuFRVBeoBlpUHRbjGui75672736 = -650011752;    float NJuFRVBeoBlpUHRbjGui84056719 = -562435137;    float NJuFRVBeoBlpUHRbjGui46256183 = -845942406;    float NJuFRVBeoBlpUHRbjGui34785648 = -460862285;    float NJuFRVBeoBlpUHRbjGui4997501 = -971228291;    float NJuFRVBeoBlpUHRbjGui77976171 = -654503080;    float NJuFRVBeoBlpUHRbjGui21553407 = -509833398;    float NJuFRVBeoBlpUHRbjGui21276048 = -689949154;    float NJuFRVBeoBlpUHRbjGui67857069 = -601596786;    float NJuFRVBeoBlpUHRbjGui35895861 = -956952021;    float NJuFRVBeoBlpUHRbjGui94735150 = -727630558;    float NJuFRVBeoBlpUHRbjGui43940025 = -825664258;    float NJuFRVBeoBlpUHRbjGui80503507 = -603155709;    float NJuFRVBeoBlpUHRbjGui4835688 = -135597931;    float NJuFRVBeoBlpUHRbjGui70979754 = 80676438;    float NJuFRVBeoBlpUHRbjGui58130992 = -723841908;    float NJuFRVBeoBlpUHRbjGui73519674 = -288145178;     NJuFRVBeoBlpUHRbjGui79772837 = NJuFRVBeoBlpUHRbjGui17471410;     NJuFRVBeoBlpUHRbjGui17471410 = NJuFRVBeoBlpUHRbjGui44198439;     NJuFRVBeoBlpUHRbjGui44198439 = NJuFRVBeoBlpUHRbjGui27198649;     NJuFRVBeoBlpUHRbjGui27198649 = NJuFRVBeoBlpUHRbjGui71794772;     NJuFRVBeoBlpUHRbjGui71794772 = NJuFRVBeoBlpUHRbjGui47203805;     NJuFRVBeoBlpUHRbjGui47203805 = NJuFRVBeoBlpUHRbjGui89621414;     NJuFRVBeoBlpUHRbjGui89621414 = NJuFRVBeoBlpUHRbjGui80400883;     NJuFRVBeoBlpUHRbjGui80400883 = NJuFRVBeoBlpUHRbjGui76515418;     NJuFRVBeoBlpUHRbjGui76515418 = NJuFRVBeoBlpUHRbjGui60074928;     NJuFRVBeoBlpUHRbjGui60074928 = NJuFRVBeoBlpUHRbjGui62268496;     NJuFRVBeoBlpUHRbjGui62268496 = NJuFRVBeoBlpUHRbjGui40323622;     NJuFRVBeoBlpUHRbjGui40323622 = NJuFRVBeoBlpUHRbjGui51128030;     NJuFRVBeoBlpUHRbjGui51128030 = NJuFRVBeoBlpUHRbjGui7239816;     NJuFRVBeoBlpUHRbjGui7239816 = NJuFRVBeoBlpUHRbjGui21161700;     NJuFRVBeoBlpUHRbjGui21161700 = NJuFRVBeoBlpUHRbjGui87462579;     NJuFRVBeoBlpUHRbjGui87462579 = NJuFRVBeoBlpUHRbjGui41031052;     NJuFRVBeoBlpUHRbjGui41031052 = NJuFRVBeoBlpUHRbjGui37870027;     NJuFRVBeoBlpUHRbjGui37870027 = NJuFRVBeoBlpUHRbjGui97268768;     NJuFRVBeoBlpUHRbjGui97268768 = NJuFRVBeoBlpUHRbjGui63410824;     NJuFRVBeoBlpUHRbjGui63410824 = NJuFRVBeoBlpUHRbjGui4185035;     NJuFRVBeoBlpUHRbjGui4185035 = NJuFRVBeoBlpUHRbjGui46008502;     NJuFRVBeoBlpUHRbjGui46008502 = NJuFRVBeoBlpUHRbjGui23264770;     NJuFRVBeoBlpUHRbjGui23264770 = NJuFRVBeoBlpUHRbjGui86753481;     NJuFRVBeoBlpUHRbjGui86753481 = NJuFRVBeoBlpUHRbjGui43700325;     NJuFRVBeoBlpUHRbjGui43700325 = NJuFRVBeoBlpUHRbjGui94608626;     NJuFRVBeoBlpUHRbjGui94608626 = NJuFRVBeoBlpUHRbjGui49444543;     NJuFRVBeoBlpUHRbjGui49444543 = NJuFRVBeoBlpUHRbjGui19106286;     NJuFRVBeoBlpUHRbjGui19106286 = NJuFRVBeoBlpUHRbjGui10440971;     NJuFRVBeoBlpUHRbjGui10440971 = NJuFRVBeoBlpUHRbjGui82668097;     NJuFRVBeoBlpUHRbjGui82668097 = NJuFRVBeoBlpUHRbjGui2901417;     NJuFRVBeoBlpUHRbjGui2901417 = NJuFRVBeoBlpUHRbjGui24271679;     NJuFRVBeoBlpUHRbjGui24271679 = NJuFRVBeoBlpUHRbjGui36711122;     NJuFRVBeoBlpUHRbjGui36711122 = NJuFRVBeoBlpUHRbjGui96077923;     NJuFRVBeoBlpUHRbjGui96077923 = NJuFRVBeoBlpUHRbjGui55317742;     NJuFRVBeoBlpUHRbjGui55317742 = NJuFRVBeoBlpUHRbjGui15482687;     NJuFRVBeoBlpUHRbjGui15482687 = NJuFRVBeoBlpUHRbjGui25302026;     NJuFRVBeoBlpUHRbjGui25302026 = NJuFRVBeoBlpUHRbjGui19394185;     NJuFRVBeoBlpUHRbjGui19394185 = NJuFRVBeoBlpUHRbjGui71921296;     NJuFRVBeoBlpUHRbjGui71921296 = NJuFRVBeoBlpUHRbjGui41699287;     NJuFRVBeoBlpUHRbjGui41699287 = NJuFRVBeoBlpUHRbjGui51018636;     NJuFRVBeoBlpUHRbjGui51018636 = NJuFRVBeoBlpUHRbjGui74795600;     NJuFRVBeoBlpUHRbjGui74795600 = NJuFRVBeoBlpUHRbjGui64827075;     NJuFRVBeoBlpUHRbjGui64827075 = NJuFRVBeoBlpUHRbjGui15304504;     NJuFRVBeoBlpUHRbjGui15304504 = NJuFRVBeoBlpUHRbjGui11516492;     NJuFRVBeoBlpUHRbjGui11516492 = NJuFRVBeoBlpUHRbjGui68326574;     NJuFRVBeoBlpUHRbjGui68326574 = NJuFRVBeoBlpUHRbjGui51198778;     NJuFRVBeoBlpUHRbjGui51198778 = NJuFRVBeoBlpUHRbjGui15812962;     NJuFRVBeoBlpUHRbjGui15812962 = NJuFRVBeoBlpUHRbjGui36984609;     NJuFRVBeoBlpUHRbjGui36984609 = NJuFRVBeoBlpUHRbjGui34768234;     NJuFRVBeoBlpUHRbjGui34768234 = NJuFRVBeoBlpUHRbjGui98611377;     NJuFRVBeoBlpUHRbjGui98611377 = NJuFRVBeoBlpUHRbjGui56645234;     NJuFRVBeoBlpUHRbjGui56645234 = NJuFRVBeoBlpUHRbjGui89486031;     NJuFRVBeoBlpUHRbjGui89486031 = NJuFRVBeoBlpUHRbjGui31713622;     NJuFRVBeoBlpUHRbjGui31713622 = NJuFRVBeoBlpUHRbjGui18101753;     NJuFRVBeoBlpUHRbjGui18101753 = NJuFRVBeoBlpUHRbjGui33764336;     NJuFRVBeoBlpUHRbjGui33764336 = NJuFRVBeoBlpUHRbjGui94206639;     NJuFRVBeoBlpUHRbjGui94206639 = NJuFRVBeoBlpUHRbjGui57444957;     NJuFRVBeoBlpUHRbjGui57444957 = NJuFRVBeoBlpUHRbjGui83498324;     NJuFRVBeoBlpUHRbjGui83498324 = NJuFRVBeoBlpUHRbjGui77186146;     NJuFRVBeoBlpUHRbjGui77186146 = NJuFRVBeoBlpUHRbjGui97759261;     NJuFRVBeoBlpUHRbjGui97759261 = NJuFRVBeoBlpUHRbjGui70515128;     NJuFRVBeoBlpUHRbjGui70515128 = NJuFRVBeoBlpUHRbjGui69959913;     NJuFRVBeoBlpUHRbjGui69959913 = NJuFRVBeoBlpUHRbjGui93847321;     NJuFRVBeoBlpUHRbjGui93847321 = NJuFRVBeoBlpUHRbjGui57173512;     NJuFRVBeoBlpUHRbjGui57173512 = NJuFRVBeoBlpUHRbjGui37996817;     NJuFRVBeoBlpUHRbjGui37996817 = NJuFRVBeoBlpUHRbjGui3612500;     NJuFRVBeoBlpUHRbjGui3612500 = NJuFRVBeoBlpUHRbjGui55050106;     NJuFRVBeoBlpUHRbjGui55050106 = NJuFRVBeoBlpUHRbjGui51922074;     NJuFRVBeoBlpUHRbjGui51922074 = NJuFRVBeoBlpUHRbjGui5679014;     NJuFRVBeoBlpUHRbjGui5679014 = NJuFRVBeoBlpUHRbjGui62160553;     NJuFRVBeoBlpUHRbjGui62160553 = NJuFRVBeoBlpUHRbjGui21636867;     NJuFRVBeoBlpUHRbjGui21636867 = NJuFRVBeoBlpUHRbjGui65948730;     NJuFRVBeoBlpUHRbjGui65948730 = NJuFRVBeoBlpUHRbjGui55569482;     NJuFRVBeoBlpUHRbjGui55569482 = NJuFRVBeoBlpUHRbjGui12392188;     NJuFRVBeoBlpUHRbjGui12392188 = NJuFRVBeoBlpUHRbjGui29389434;     NJuFRVBeoBlpUHRbjGui29389434 = NJuFRVBeoBlpUHRbjGui81181426;     NJuFRVBeoBlpUHRbjGui81181426 = NJuFRVBeoBlpUHRbjGui7960266;     NJuFRVBeoBlpUHRbjGui7960266 = NJuFRVBeoBlpUHRbjGui75236990;     NJuFRVBeoBlpUHRbjGui75236990 = NJuFRVBeoBlpUHRbjGui75373750;     NJuFRVBeoBlpUHRbjGui75373750 = NJuFRVBeoBlpUHRbjGui43409848;     NJuFRVBeoBlpUHRbjGui43409848 = NJuFRVBeoBlpUHRbjGui33631582;     NJuFRVBeoBlpUHRbjGui33631582 = NJuFRVBeoBlpUHRbjGui82121677;     NJuFRVBeoBlpUHRbjGui82121677 = NJuFRVBeoBlpUHRbjGui75672736;     NJuFRVBeoBlpUHRbjGui75672736 = NJuFRVBeoBlpUHRbjGui84056719;     NJuFRVBeoBlpUHRbjGui84056719 = NJuFRVBeoBlpUHRbjGui46256183;     NJuFRVBeoBlpUHRbjGui46256183 = NJuFRVBeoBlpUHRbjGui34785648;     NJuFRVBeoBlpUHRbjGui34785648 = NJuFRVBeoBlpUHRbjGui4997501;     NJuFRVBeoBlpUHRbjGui4997501 = NJuFRVBeoBlpUHRbjGui77976171;     NJuFRVBeoBlpUHRbjGui77976171 = NJuFRVBeoBlpUHRbjGui21553407;     NJuFRVBeoBlpUHRbjGui21553407 = NJuFRVBeoBlpUHRbjGui21276048;     NJuFRVBeoBlpUHRbjGui21276048 = NJuFRVBeoBlpUHRbjGui67857069;     NJuFRVBeoBlpUHRbjGui67857069 = NJuFRVBeoBlpUHRbjGui35895861;     NJuFRVBeoBlpUHRbjGui35895861 = NJuFRVBeoBlpUHRbjGui94735150;     NJuFRVBeoBlpUHRbjGui94735150 = NJuFRVBeoBlpUHRbjGui43940025;     NJuFRVBeoBlpUHRbjGui43940025 = NJuFRVBeoBlpUHRbjGui80503507;     NJuFRVBeoBlpUHRbjGui80503507 = NJuFRVBeoBlpUHRbjGui4835688;     NJuFRVBeoBlpUHRbjGui4835688 = NJuFRVBeoBlpUHRbjGui70979754;     NJuFRVBeoBlpUHRbjGui70979754 = NJuFRVBeoBlpUHRbjGui58130992;     NJuFRVBeoBlpUHRbjGui58130992 = NJuFRVBeoBlpUHRbjGui73519674;     NJuFRVBeoBlpUHRbjGui73519674 = NJuFRVBeoBlpUHRbjGui79772837;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void vzeRcrFNKidOrFrvdFqp76703000() {     float AofihQZFemJmaZtIjjfc50424504 = -906294308;    float AofihQZFemJmaZtIjjfc86144926 = -914007914;    float AofihQZFemJmaZtIjjfc24167358 = -739494699;    float AofihQZFemJmaZtIjjfc67129935 = -845776842;    float AofihQZFemJmaZtIjjfc93245093 = 15278144;    float AofihQZFemJmaZtIjjfc37203567 = -954861534;    float AofihQZFemJmaZtIjjfc8000310 = -1114164;    float AofihQZFemJmaZtIjjfc37243264 = -973735047;    float AofihQZFemJmaZtIjjfc22380473 = -880811557;    float AofihQZFemJmaZtIjjfc83041389 = -325259849;    float AofihQZFemJmaZtIjjfc4517658 = -456489933;    float AofihQZFemJmaZtIjjfc85870192 = -367053688;    float AofihQZFemJmaZtIjjfc30955417 = -5249423;    float AofihQZFemJmaZtIjjfc91086849 = -711657896;    float AofihQZFemJmaZtIjjfc3881720 = -778210725;    float AofihQZFemJmaZtIjjfc19991220 = -964579102;    float AofihQZFemJmaZtIjjfc34883879 = 63142838;    float AofihQZFemJmaZtIjjfc58163521 = 47992745;    float AofihQZFemJmaZtIjjfc78976550 = -499491326;    float AofihQZFemJmaZtIjjfc22197534 = -322275686;    float AofihQZFemJmaZtIjjfc59320789 = -873356274;    float AofihQZFemJmaZtIjjfc7404267 = -246520309;    float AofihQZFemJmaZtIjjfc25208579 = -720029859;    float AofihQZFemJmaZtIjjfc20603019 = -258345564;    float AofihQZFemJmaZtIjjfc7824714 = -847472060;    float AofihQZFemJmaZtIjjfc25206207 = -60946118;    float AofihQZFemJmaZtIjjfc57705328 = -452026714;    float AofihQZFemJmaZtIjjfc26888511 = -721774393;    float AofihQZFemJmaZtIjjfc42583458 = -66434183;    float AofihQZFemJmaZtIjjfc21840193 = -107619591;    float AofihQZFemJmaZtIjjfc9973414 = -478342472;    float AofihQZFemJmaZtIjjfc68783140 = -116078768;    float AofihQZFemJmaZtIjjfc19656483 = -398839601;    float AofihQZFemJmaZtIjjfc61442898 = -544639212;    float AofihQZFemJmaZtIjjfc33341978 = -729690352;    float AofihQZFemJmaZtIjjfc75475945 = -559353603;    float AofihQZFemJmaZtIjjfc69093512 = -421445575;    float AofihQZFemJmaZtIjjfc72186674 = -225136209;    float AofihQZFemJmaZtIjjfc50454586 = -922013029;    float AofihQZFemJmaZtIjjfc36802585 = -439704065;    float AofihQZFemJmaZtIjjfc11091921 = -772862416;    float AofihQZFemJmaZtIjjfc3278592 = -759828991;    float AofihQZFemJmaZtIjjfc13794036 = -44317323;    float AofihQZFemJmaZtIjjfc21812805 = -90072591;    float AofihQZFemJmaZtIjjfc7470551 = -549970922;    float AofihQZFemJmaZtIjjfc74732409 = -373010740;    float AofihQZFemJmaZtIjjfc77329981 = -173929678;    float AofihQZFemJmaZtIjjfc81427171 = -46646140;    float AofihQZFemJmaZtIjjfc42426061 = -315216595;    float AofihQZFemJmaZtIjjfc43195721 = -405073093;    float AofihQZFemJmaZtIjjfc66800702 = -633361504;    float AofihQZFemJmaZtIjjfc98516738 = -853186103;    float AofihQZFemJmaZtIjjfc44561689 = -254717363;    float AofihQZFemJmaZtIjjfc21940926 = -41531199;    float AofihQZFemJmaZtIjjfc17382211 = -337547363;    float AofihQZFemJmaZtIjjfc43020238 = -559774000;    float AofihQZFemJmaZtIjjfc60936348 = -93978056;    float AofihQZFemJmaZtIjjfc3564340 = -381149135;    float AofihQZFemJmaZtIjjfc59305222 = -998304782;    float AofihQZFemJmaZtIjjfc68038887 = -923775738;    float AofihQZFemJmaZtIjjfc79498238 = -402834821;    float AofihQZFemJmaZtIjjfc81111799 = -279339772;    float AofihQZFemJmaZtIjjfc94659805 = -807300864;    float AofihQZFemJmaZtIjjfc540280 = -673191967;    float AofihQZFemJmaZtIjjfc73067976 = -846917377;    float AofihQZFemJmaZtIjjfc35734517 = -240411165;    float AofihQZFemJmaZtIjjfc66213710 = -968214087;    float AofihQZFemJmaZtIjjfc69512518 = -460610211;    float AofihQZFemJmaZtIjjfc57744871 = -981967544;    float AofihQZFemJmaZtIjjfc28405774 = -118857123;    float AofihQZFemJmaZtIjjfc50897707 = -443133528;    float AofihQZFemJmaZtIjjfc62697205 = -711720953;    float AofihQZFemJmaZtIjjfc7708936 = -29994227;    float AofihQZFemJmaZtIjjfc42173966 = 40212739;    float AofihQZFemJmaZtIjjfc11105614 = -549413270;    float AofihQZFemJmaZtIjjfc56042197 = -13527283;    float AofihQZFemJmaZtIjjfc93610231 = -102202987;    float AofihQZFemJmaZtIjjfc3395775 = -529957268;    float AofihQZFemJmaZtIjjfc13132469 = -708374643;    float AofihQZFemJmaZtIjjfc33092304 = -374461321;    float AofihQZFemJmaZtIjjfc47876226 = -887016441;    float AofihQZFemJmaZtIjjfc76278157 = -305380574;    float AofihQZFemJmaZtIjjfc84462450 = -306557798;    float AofihQZFemJmaZtIjjfc99387737 = -661361091;    float AofihQZFemJmaZtIjjfc55039491 = -474258087;    float AofihQZFemJmaZtIjjfc11456675 = -625156370;    float AofihQZFemJmaZtIjjfc24221451 = -861361406;    float AofihQZFemJmaZtIjjfc97715556 = -257308403;    float AofihQZFemJmaZtIjjfc44060687 = -107091850;    float AofihQZFemJmaZtIjjfc90321740 = -69916353;    float AofihQZFemJmaZtIjjfc14539598 = -365375547;    float AofihQZFemJmaZtIjjfc65529173 = 59703560;    float AofihQZFemJmaZtIjjfc12881452 = -226831427;    float AofihQZFemJmaZtIjjfc82415698 = -998237292;    float AofihQZFemJmaZtIjjfc57304346 = 63130756;    float AofihQZFemJmaZtIjjfc29980122 = -393522645;    float AofihQZFemJmaZtIjjfc8618786 = -952528128;    float AofihQZFemJmaZtIjjfc13253756 = -371125356;    float AofihQZFemJmaZtIjjfc48744829 = -243155214;    float AofihQZFemJmaZtIjjfc71736033 = -906294308;     AofihQZFemJmaZtIjjfc50424504 = AofihQZFemJmaZtIjjfc86144926;     AofihQZFemJmaZtIjjfc86144926 = AofihQZFemJmaZtIjjfc24167358;     AofihQZFemJmaZtIjjfc24167358 = AofihQZFemJmaZtIjjfc67129935;     AofihQZFemJmaZtIjjfc67129935 = AofihQZFemJmaZtIjjfc93245093;     AofihQZFemJmaZtIjjfc93245093 = AofihQZFemJmaZtIjjfc37203567;     AofihQZFemJmaZtIjjfc37203567 = AofihQZFemJmaZtIjjfc8000310;     AofihQZFemJmaZtIjjfc8000310 = AofihQZFemJmaZtIjjfc37243264;     AofihQZFemJmaZtIjjfc37243264 = AofihQZFemJmaZtIjjfc22380473;     AofihQZFemJmaZtIjjfc22380473 = AofihQZFemJmaZtIjjfc83041389;     AofihQZFemJmaZtIjjfc83041389 = AofihQZFemJmaZtIjjfc4517658;     AofihQZFemJmaZtIjjfc4517658 = AofihQZFemJmaZtIjjfc85870192;     AofihQZFemJmaZtIjjfc85870192 = AofihQZFemJmaZtIjjfc30955417;     AofihQZFemJmaZtIjjfc30955417 = AofihQZFemJmaZtIjjfc91086849;     AofihQZFemJmaZtIjjfc91086849 = AofihQZFemJmaZtIjjfc3881720;     AofihQZFemJmaZtIjjfc3881720 = AofihQZFemJmaZtIjjfc19991220;     AofihQZFemJmaZtIjjfc19991220 = AofihQZFemJmaZtIjjfc34883879;     AofihQZFemJmaZtIjjfc34883879 = AofihQZFemJmaZtIjjfc58163521;     AofihQZFemJmaZtIjjfc58163521 = AofihQZFemJmaZtIjjfc78976550;     AofihQZFemJmaZtIjjfc78976550 = AofihQZFemJmaZtIjjfc22197534;     AofihQZFemJmaZtIjjfc22197534 = AofihQZFemJmaZtIjjfc59320789;     AofihQZFemJmaZtIjjfc59320789 = AofihQZFemJmaZtIjjfc7404267;     AofihQZFemJmaZtIjjfc7404267 = AofihQZFemJmaZtIjjfc25208579;     AofihQZFemJmaZtIjjfc25208579 = AofihQZFemJmaZtIjjfc20603019;     AofihQZFemJmaZtIjjfc20603019 = AofihQZFemJmaZtIjjfc7824714;     AofihQZFemJmaZtIjjfc7824714 = AofihQZFemJmaZtIjjfc25206207;     AofihQZFemJmaZtIjjfc25206207 = AofihQZFemJmaZtIjjfc57705328;     AofihQZFemJmaZtIjjfc57705328 = AofihQZFemJmaZtIjjfc26888511;     AofihQZFemJmaZtIjjfc26888511 = AofihQZFemJmaZtIjjfc42583458;     AofihQZFemJmaZtIjjfc42583458 = AofihQZFemJmaZtIjjfc21840193;     AofihQZFemJmaZtIjjfc21840193 = AofihQZFemJmaZtIjjfc9973414;     AofihQZFemJmaZtIjjfc9973414 = AofihQZFemJmaZtIjjfc68783140;     AofihQZFemJmaZtIjjfc68783140 = AofihQZFemJmaZtIjjfc19656483;     AofihQZFemJmaZtIjjfc19656483 = AofihQZFemJmaZtIjjfc61442898;     AofihQZFemJmaZtIjjfc61442898 = AofihQZFemJmaZtIjjfc33341978;     AofihQZFemJmaZtIjjfc33341978 = AofihQZFemJmaZtIjjfc75475945;     AofihQZFemJmaZtIjjfc75475945 = AofihQZFemJmaZtIjjfc69093512;     AofihQZFemJmaZtIjjfc69093512 = AofihQZFemJmaZtIjjfc72186674;     AofihQZFemJmaZtIjjfc72186674 = AofihQZFemJmaZtIjjfc50454586;     AofihQZFemJmaZtIjjfc50454586 = AofihQZFemJmaZtIjjfc36802585;     AofihQZFemJmaZtIjjfc36802585 = AofihQZFemJmaZtIjjfc11091921;     AofihQZFemJmaZtIjjfc11091921 = AofihQZFemJmaZtIjjfc3278592;     AofihQZFemJmaZtIjjfc3278592 = AofihQZFemJmaZtIjjfc13794036;     AofihQZFemJmaZtIjjfc13794036 = AofihQZFemJmaZtIjjfc21812805;     AofihQZFemJmaZtIjjfc21812805 = AofihQZFemJmaZtIjjfc7470551;     AofihQZFemJmaZtIjjfc7470551 = AofihQZFemJmaZtIjjfc74732409;     AofihQZFemJmaZtIjjfc74732409 = AofihQZFemJmaZtIjjfc77329981;     AofihQZFemJmaZtIjjfc77329981 = AofihQZFemJmaZtIjjfc81427171;     AofihQZFemJmaZtIjjfc81427171 = AofihQZFemJmaZtIjjfc42426061;     AofihQZFemJmaZtIjjfc42426061 = AofihQZFemJmaZtIjjfc43195721;     AofihQZFemJmaZtIjjfc43195721 = AofihQZFemJmaZtIjjfc66800702;     AofihQZFemJmaZtIjjfc66800702 = AofihQZFemJmaZtIjjfc98516738;     AofihQZFemJmaZtIjjfc98516738 = AofihQZFemJmaZtIjjfc44561689;     AofihQZFemJmaZtIjjfc44561689 = AofihQZFemJmaZtIjjfc21940926;     AofihQZFemJmaZtIjjfc21940926 = AofihQZFemJmaZtIjjfc17382211;     AofihQZFemJmaZtIjjfc17382211 = AofihQZFemJmaZtIjjfc43020238;     AofihQZFemJmaZtIjjfc43020238 = AofihQZFemJmaZtIjjfc60936348;     AofihQZFemJmaZtIjjfc60936348 = AofihQZFemJmaZtIjjfc3564340;     AofihQZFemJmaZtIjjfc3564340 = AofihQZFemJmaZtIjjfc59305222;     AofihQZFemJmaZtIjjfc59305222 = AofihQZFemJmaZtIjjfc68038887;     AofihQZFemJmaZtIjjfc68038887 = AofihQZFemJmaZtIjjfc79498238;     AofihQZFemJmaZtIjjfc79498238 = AofihQZFemJmaZtIjjfc81111799;     AofihQZFemJmaZtIjjfc81111799 = AofihQZFemJmaZtIjjfc94659805;     AofihQZFemJmaZtIjjfc94659805 = AofihQZFemJmaZtIjjfc540280;     AofihQZFemJmaZtIjjfc540280 = AofihQZFemJmaZtIjjfc73067976;     AofihQZFemJmaZtIjjfc73067976 = AofihQZFemJmaZtIjjfc35734517;     AofihQZFemJmaZtIjjfc35734517 = AofihQZFemJmaZtIjjfc66213710;     AofihQZFemJmaZtIjjfc66213710 = AofihQZFemJmaZtIjjfc69512518;     AofihQZFemJmaZtIjjfc69512518 = AofihQZFemJmaZtIjjfc57744871;     AofihQZFemJmaZtIjjfc57744871 = AofihQZFemJmaZtIjjfc28405774;     AofihQZFemJmaZtIjjfc28405774 = AofihQZFemJmaZtIjjfc50897707;     AofihQZFemJmaZtIjjfc50897707 = AofihQZFemJmaZtIjjfc62697205;     AofihQZFemJmaZtIjjfc62697205 = AofihQZFemJmaZtIjjfc7708936;     AofihQZFemJmaZtIjjfc7708936 = AofihQZFemJmaZtIjjfc42173966;     AofihQZFemJmaZtIjjfc42173966 = AofihQZFemJmaZtIjjfc11105614;     AofihQZFemJmaZtIjjfc11105614 = AofihQZFemJmaZtIjjfc56042197;     AofihQZFemJmaZtIjjfc56042197 = AofihQZFemJmaZtIjjfc93610231;     AofihQZFemJmaZtIjjfc93610231 = AofihQZFemJmaZtIjjfc3395775;     AofihQZFemJmaZtIjjfc3395775 = AofihQZFemJmaZtIjjfc13132469;     AofihQZFemJmaZtIjjfc13132469 = AofihQZFemJmaZtIjjfc33092304;     AofihQZFemJmaZtIjjfc33092304 = AofihQZFemJmaZtIjjfc47876226;     AofihQZFemJmaZtIjjfc47876226 = AofihQZFemJmaZtIjjfc76278157;     AofihQZFemJmaZtIjjfc76278157 = AofihQZFemJmaZtIjjfc84462450;     AofihQZFemJmaZtIjjfc84462450 = AofihQZFemJmaZtIjjfc99387737;     AofihQZFemJmaZtIjjfc99387737 = AofihQZFemJmaZtIjjfc55039491;     AofihQZFemJmaZtIjjfc55039491 = AofihQZFemJmaZtIjjfc11456675;     AofihQZFemJmaZtIjjfc11456675 = AofihQZFemJmaZtIjjfc24221451;     AofihQZFemJmaZtIjjfc24221451 = AofihQZFemJmaZtIjjfc97715556;     AofihQZFemJmaZtIjjfc97715556 = AofihQZFemJmaZtIjjfc44060687;     AofihQZFemJmaZtIjjfc44060687 = AofihQZFemJmaZtIjjfc90321740;     AofihQZFemJmaZtIjjfc90321740 = AofihQZFemJmaZtIjjfc14539598;     AofihQZFemJmaZtIjjfc14539598 = AofihQZFemJmaZtIjjfc65529173;     AofihQZFemJmaZtIjjfc65529173 = AofihQZFemJmaZtIjjfc12881452;     AofihQZFemJmaZtIjjfc12881452 = AofihQZFemJmaZtIjjfc82415698;     AofihQZFemJmaZtIjjfc82415698 = AofihQZFemJmaZtIjjfc57304346;     AofihQZFemJmaZtIjjfc57304346 = AofihQZFemJmaZtIjjfc29980122;     AofihQZFemJmaZtIjjfc29980122 = AofihQZFemJmaZtIjjfc8618786;     AofihQZFemJmaZtIjjfc8618786 = AofihQZFemJmaZtIjjfc13253756;     AofihQZFemJmaZtIjjfc13253756 = AofihQZFemJmaZtIjjfc48744829;     AofihQZFemJmaZtIjjfc48744829 = AofihQZFemJmaZtIjjfc71736033;     AofihQZFemJmaZtIjjfc71736033 = AofihQZFemJmaZtIjjfc50424504;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void PymkGcCVMNjWjZOFwkbo83361744() {     float kQzCPycengRqdXgOOcVY50418033 = -778818450;    float kQzCPycengRqdXgOOcVY24717243 = -956603040;    float kQzCPycengRqdXgOOcVY25480570 = -696623312;    float kQzCPycengRqdXgOOcVY52209796 = -666317672;    float kQzCPycengRqdXgOOcVY2085196 = -453077177;    float kQzCPycengRqdXgOOcVY72008999 = -654294346;    float kQzCPycengRqdXgOOcVY76547646 = -805650631;    float kQzCPycengRqdXgOOcVY6457315 = -677912364;    float kQzCPycengRqdXgOOcVY5120637 = -594457627;    float kQzCPycengRqdXgOOcVY79680903 = 37933280;    float kQzCPycengRqdXgOOcVY24838263 = -415962005;    float kQzCPycengRqdXgOOcVY161870 = -102532051;    float kQzCPycengRqdXgOOcVY62808331 = -26907898;    float kQzCPycengRqdXgOOcVY44931070 = 43189053;    float kQzCPycengRqdXgOOcVY49053828 = -740923577;    float kQzCPycengRqdXgOOcVY94555766 = -197374753;    float kQzCPycengRqdXgOOcVY90736638 = -856471732;    float kQzCPycengRqdXgOOcVY81164053 = -349078877;    float kQzCPycengRqdXgOOcVY99404820 = -17473592;    float kQzCPycengRqdXgOOcVY94271296 = -934300138;    float kQzCPycengRqdXgOOcVY38990213 = -583822647;    float kQzCPycengRqdXgOOcVY85700532 = -756512192;    float kQzCPycengRqdXgOOcVY62086975 = -78987701;    float kQzCPycengRqdXgOOcVY64253127 = -139266086;    float kQzCPycengRqdXgOOcVY36267403 = -174165914;    float kQzCPycengRqdXgOOcVY41780284 = 30847953;    float kQzCPycengRqdXgOOcVY27926910 = -752826426;    float kQzCPycengRqdXgOOcVY62292036 = -230741381;    float kQzCPycengRqdXgOOcVY55261905 = -712504498;    float kQzCPycengRqdXgOOcVY97930062 = -883050964;    float kQzCPycengRqdXgOOcVY43386092 = -865435707;    float kQzCPycengRqdXgOOcVY53506997 = -507900530;    float kQzCPycengRqdXgOOcVY5108906 = -600783817;    float kQzCPycengRqdXgOOcVY61725736 = -421320945;    float kQzCPycengRqdXgOOcVY15700016 = -989004277;    float kQzCPycengRqdXgOOcVY18304135 = -639119349;    float kQzCPycengRqdXgOOcVY81034879 = -521098953;    float kQzCPycengRqdXgOOcVY42104353 = -220398888;    float kQzCPycengRqdXgOOcVY85768182 = -534694445;    float kQzCPycengRqdXgOOcVY8675478 = -49135751;    float kQzCPycengRqdXgOOcVY98947587 = -860806315;    float kQzCPycengRqdXgOOcVY88976252 = -953621073;    float kQzCPycengRqdXgOOcVY74077395 = -603447577;    float kQzCPycengRqdXgOOcVY81044703 = -747037034;    float kQzCPycengRqdXgOOcVY91488602 = 76769677;    float kQzCPycengRqdXgOOcVY15160378 = -185055411;    float kQzCPycengRqdXgOOcVY5027774 = -655210300;    float kQzCPycengRqdXgOOcVY56810752 = -655407496;    float kQzCPycengRqdXgOOcVY61084450 = -978980457;    float kQzCPycengRqdXgOOcVY63241144 = 7902020;    float kQzCPycengRqdXgOOcVY80060071 = -211916512;    float kQzCPycengRqdXgOOcVY13063996 = -135331851;    float kQzCPycengRqdXgOOcVY20010037 = -146771371;    float kQzCPycengRqdXgOOcVY32191197 = -573451050;    float kQzCPycengRqdXgOOcVY44371532 = -94446640;    float kQzCPycengRqdXgOOcVY64717500 = 77693742;    float kQzCPycengRqdXgOOcVY62630268 = -777615339;    float kQzCPycengRqdXgOOcVY61227443 = -457357227;    float kQzCPycengRqdXgOOcVY15942394 = -392151759;    float kQzCPycengRqdXgOOcVY60304912 = -383925131;    float kQzCPycengRqdXgOOcVY44082089 = -901467920;    float kQzCPycengRqdXgOOcVY14255610 = -474909250;    float kQzCPycengRqdXgOOcVY51195409 = -965407866;    float kQzCPycengRqdXgOOcVY7190574 = -711406663;    float kQzCPycengRqdXgOOcVY36294811 = -96631014;    float kQzCPycengRqdXgOOcVY71331265 = -908061476;    float kQzCPycengRqdXgOOcVY95052964 = -501748234;    float kQzCPycengRqdXgOOcVY1082596 = -605586953;    float kQzCPycengRqdXgOOcVY29231054 = 32193329;    float kQzCPycengRqdXgOOcVY30749694 = -1804228;    float kQzCPycengRqdXgOOcVY13520887 = -676275801;    float kQzCPycengRqdXgOOcVY48632285 = -536072844;    float kQzCPycengRqdXgOOcVY95395871 = -814384432;    float kQzCPycengRqdXgOOcVY90729342 = -968337842;    float kQzCPycengRqdXgOOcVY95323708 = 26506177;    float kQzCPycengRqdXgOOcVY50013961 = -630201575;    float kQzCPycengRqdXgOOcVY11623138 = -53064616;    float kQzCPycengRqdXgOOcVY81042272 = -331950667;    float kQzCPycengRqdXgOOcVY72764524 = -116035764;    float kQzCPycengRqdXgOOcVY21107025 = -989110504;    float kQzCPycengRqdXgOOcVY36752510 = -313941747;    float kQzCPycengRqdXgOOcVY71116158 = 2581069;    float kQzCPycengRqdXgOOcVY1207586 = -251760925;    float kQzCPycengRqdXgOOcVY92020761 = -620406519;    float kQzCPycengRqdXgOOcVY17869992 = -571134453;    float kQzCPycengRqdXgOOcVY30322096 = -630103856;    float kQzCPycengRqdXgOOcVY33496960 = -261129159;    float kQzCPycengRqdXgOOcVY72917709 = 72667233;    float kQzCPycengRqdXgOOcVY17354205 = -226874305;    float kQzCPycengRqdXgOOcVY50982516 = -966698019;    float kQzCPycengRqdXgOOcVY55673867 = -861504010;    float kQzCPycengRqdXgOOcVY19807437 = 36258273;    float kQzCPycengRqdXgOOcVY26161960 = -828247129;    float kQzCPycengRqdXgOOcVY25463270 = -50769315;    float kQzCPycengRqdXgOOcVY64593388 = -147667832;    float kQzCPycengRqdXgOOcVY84691978 = -285897065;    float kQzCPycengRqdXgOOcVY37780843 = -988213208;    float kQzCPycengRqdXgOOcVY66886821 = -892040914;    float kQzCPycengRqdXgOOcVY44749892 = -550406021;    float kQzCPycengRqdXgOOcVY20157338 = -778818450;     kQzCPycengRqdXgOOcVY50418033 = kQzCPycengRqdXgOOcVY24717243;     kQzCPycengRqdXgOOcVY24717243 = kQzCPycengRqdXgOOcVY25480570;     kQzCPycengRqdXgOOcVY25480570 = kQzCPycengRqdXgOOcVY52209796;     kQzCPycengRqdXgOOcVY52209796 = kQzCPycengRqdXgOOcVY2085196;     kQzCPycengRqdXgOOcVY2085196 = kQzCPycengRqdXgOOcVY72008999;     kQzCPycengRqdXgOOcVY72008999 = kQzCPycengRqdXgOOcVY76547646;     kQzCPycengRqdXgOOcVY76547646 = kQzCPycengRqdXgOOcVY6457315;     kQzCPycengRqdXgOOcVY6457315 = kQzCPycengRqdXgOOcVY5120637;     kQzCPycengRqdXgOOcVY5120637 = kQzCPycengRqdXgOOcVY79680903;     kQzCPycengRqdXgOOcVY79680903 = kQzCPycengRqdXgOOcVY24838263;     kQzCPycengRqdXgOOcVY24838263 = kQzCPycengRqdXgOOcVY161870;     kQzCPycengRqdXgOOcVY161870 = kQzCPycengRqdXgOOcVY62808331;     kQzCPycengRqdXgOOcVY62808331 = kQzCPycengRqdXgOOcVY44931070;     kQzCPycengRqdXgOOcVY44931070 = kQzCPycengRqdXgOOcVY49053828;     kQzCPycengRqdXgOOcVY49053828 = kQzCPycengRqdXgOOcVY94555766;     kQzCPycengRqdXgOOcVY94555766 = kQzCPycengRqdXgOOcVY90736638;     kQzCPycengRqdXgOOcVY90736638 = kQzCPycengRqdXgOOcVY81164053;     kQzCPycengRqdXgOOcVY81164053 = kQzCPycengRqdXgOOcVY99404820;     kQzCPycengRqdXgOOcVY99404820 = kQzCPycengRqdXgOOcVY94271296;     kQzCPycengRqdXgOOcVY94271296 = kQzCPycengRqdXgOOcVY38990213;     kQzCPycengRqdXgOOcVY38990213 = kQzCPycengRqdXgOOcVY85700532;     kQzCPycengRqdXgOOcVY85700532 = kQzCPycengRqdXgOOcVY62086975;     kQzCPycengRqdXgOOcVY62086975 = kQzCPycengRqdXgOOcVY64253127;     kQzCPycengRqdXgOOcVY64253127 = kQzCPycengRqdXgOOcVY36267403;     kQzCPycengRqdXgOOcVY36267403 = kQzCPycengRqdXgOOcVY41780284;     kQzCPycengRqdXgOOcVY41780284 = kQzCPycengRqdXgOOcVY27926910;     kQzCPycengRqdXgOOcVY27926910 = kQzCPycengRqdXgOOcVY62292036;     kQzCPycengRqdXgOOcVY62292036 = kQzCPycengRqdXgOOcVY55261905;     kQzCPycengRqdXgOOcVY55261905 = kQzCPycengRqdXgOOcVY97930062;     kQzCPycengRqdXgOOcVY97930062 = kQzCPycengRqdXgOOcVY43386092;     kQzCPycengRqdXgOOcVY43386092 = kQzCPycengRqdXgOOcVY53506997;     kQzCPycengRqdXgOOcVY53506997 = kQzCPycengRqdXgOOcVY5108906;     kQzCPycengRqdXgOOcVY5108906 = kQzCPycengRqdXgOOcVY61725736;     kQzCPycengRqdXgOOcVY61725736 = kQzCPycengRqdXgOOcVY15700016;     kQzCPycengRqdXgOOcVY15700016 = kQzCPycengRqdXgOOcVY18304135;     kQzCPycengRqdXgOOcVY18304135 = kQzCPycengRqdXgOOcVY81034879;     kQzCPycengRqdXgOOcVY81034879 = kQzCPycengRqdXgOOcVY42104353;     kQzCPycengRqdXgOOcVY42104353 = kQzCPycengRqdXgOOcVY85768182;     kQzCPycengRqdXgOOcVY85768182 = kQzCPycengRqdXgOOcVY8675478;     kQzCPycengRqdXgOOcVY8675478 = kQzCPycengRqdXgOOcVY98947587;     kQzCPycengRqdXgOOcVY98947587 = kQzCPycengRqdXgOOcVY88976252;     kQzCPycengRqdXgOOcVY88976252 = kQzCPycengRqdXgOOcVY74077395;     kQzCPycengRqdXgOOcVY74077395 = kQzCPycengRqdXgOOcVY81044703;     kQzCPycengRqdXgOOcVY81044703 = kQzCPycengRqdXgOOcVY91488602;     kQzCPycengRqdXgOOcVY91488602 = kQzCPycengRqdXgOOcVY15160378;     kQzCPycengRqdXgOOcVY15160378 = kQzCPycengRqdXgOOcVY5027774;     kQzCPycengRqdXgOOcVY5027774 = kQzCPycengRqdXgOOcVY56810752;     kQzCPycengRqdXgOOcVY56810752 = kQzCPycengRqdXgOOcVY61084450;     kQzCPycengRqdXgOOcVY61084450 = kQzCPycengRqdXgOOcVY63241144;     kQzCPycengRqdXgOOcVY63241144 = kQzCPycengRqdXgOOcVY80060071;     kQzCPycengRqdXgOOcVY80060071 = kQzCPycengRqdXgOOcVY13063996;     kQzCPycengRqdXgOOcVY13063996 = kQzCPycengRqdXgOOcVY20010037;     kQzCPycengRqdXgOOcVY20010037 = kQzCPycengRqdXgOOcVY32191197;     kQzCPycengRqdXgOOcVY32191197 = kQzCPycengRqdXgOOcVY44371532;     kQzCPycengRqdXgOOcVY44371532 = kQzCPycengRqdXgOOcVY64717500;     kQzCPycengRqdXgOOcVY64717500 = kQzCPycengRqdXgOOcVY62630268;     kQzCPycengRqdXgOOcVY62630268 = kQzCPycengRqdXgOOcVY61227443;     kQzCPycengRqdXgOOcVY61227443 = kQzCPycengRqdXgOOcVY15942394;     kQzCPycengRqdXgOOcVY15942394 = kQzCPycengRqdXgOOcVY60304912;     kQzCPycengRqdXgOOcVY60304912 = kQzCPycengRqdXgOOcVY44082089;     kQzCPycengRqdXgOOcVY44082089 = kQzCPycengRqdXgOOcVY14255610;     kQzCPycengRqdXgOOcVY14255610 = kQzCPycengRqdXgOOcVY51195409;     kQzCPycengRqdXgOOcVY51195409 = kQzCPycengRqdXgOOcVY7190574;     kQzCPycengRqdXgOOcVY7190574 = kQzCPycengRqdXgOOcVY36294811;     kQzCPycengRqdXgOOcVY36294811 = kQzCPycengRqdXgOOcVY71331265;     kQzCPycengRqdXgOOcVY71331265 = kQzCPycengRqdXgOOcVY95052964;     kQzCPycengRqdXgOOcVY95052964 = kQzCPycengRqdXgOOcVY1082596;     kQzCPycengRqdXgOOcVY1082596 = kQzCPycengRqdXgOOcVY29231054;     kQzCPycengRqdXgOOcVY29231054 = kQzCPycengRqdXgOOcVY30749694;     kQzCPycengRqdXgOOcVY30749694 = kQzCPycengRqdXgOOcVY13520887;     kQzCPycengRqdXgOOcVY13520887 = kQzCPycengRqdXgOOcVY48632285;     kQzCPycengRqdXgOOcVY48632285 = kQzCPycengRqdXgOOcVY95395871;     kQzCPycengRqdXgOOcVY95395871 = kQzCPycengRqdXgOOcVY90729342;     kQzCPycengRqdXgOOcVY90729342 = kQzCPycengRqdXgOOcVY95323708;     kQzCPycengRqdXgOOcVY95323708 = kQzCPycengRqdXgOOcVY50013961;     kQzCPycengRqdXgOOcVY50013961 = kQzCPycengRqdXgOOcVY11623138;     kQzCPycengRqdXgOOcVY11623138 = kQzCPycengRqdXgOOcVY81042272;     kQzCPycengRqdXgOOcVY81042272 = kQzCPycengRqdXgOOcVY72764524;     kQzCPycengRqdXgOOcVY72764524 = kQzCPycengRqdXgOOcVY21107025;     kQzCPycengRqdXgOOcVY21107025 = kQzCPycengRqdXgOOcVY36752510;     kQzCPycengRqdXgOOcVY36752510 = kQzCPycengRqdXgOOcVY71116158;     kQzCPycengRqdXgOOcVY71116158 = kQzCPycengRqdXgOOcVY1207586;     kQzCPycengRqdXgOOcVY1207586 = kQzCPycengRqdXgOOcVY92020761;     kQzCPycengRqdXgOOcVY92020761 = kQzCPycengRqdXgOOcVY17869992;     kQzCPycengRqdXgOOcVY17869992 = kQzCPycengRqdXgOOcVY30322096;     kQzCPycengRqdXgOOcVY30322096 = kQzCPycengRqdXgOOcVY33496960;     kQzCPycengRqdXgOOcVY33496960 = kQzCPycengRqdXgOOcVY72917709;     kQzCPycengRqdXgOOcVY72917709 = kQzCPycengRqdXgOOcVY17354205;     kQzCPycengRqdXgOOcVY17354205 = kQzCPycengRqdXgOOcVY50982516;     kQzCPycengRqdXgOOcVY50982516 = kQzCPycengRqdXgOOcVY55673867;     kQzCPycengRqdXgOOcVY55673867 = kQzCPycengRqdXgOOcVY19807437;     kQzCPycengRqdXgOOcVY19807437 = kQzCPycengRqdXgOOcVY26161960;     kQzCPycengRqdXgOOcVY26161960 = kQzCPycengRqdXgOOcVY25463270;     kQzCPycengRqdXgOOcVY25463270 = kQzCPycengRqdXgOOcVY64593388;     kQzCPycengRqdXgOOcVY64593388 = kQzCPycengRqdXgOOcVY84691978;     kQzCPycengRqdXgOOcVY84691978 = kQzCPycengRqdXgOOcVY37780843;     kQzCPycengRqdXgOOcVY37780843 = kQzCPycengRqdXgOOcVY66886821;     kQzCPycengRqdXgOOcVY66886821 = kQzCPycengRqdXgOOcVY44749892;     kQzCPycengRqdXgOOcVY44749892 = kQzCPycengRqdXgOOcVY20157338;     kQzCPycengRqdXgOOcVY20157338 = kQzCPycengRqdXgOOcVY50418033;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void hQxvCJgFVNcHwTeGaTvQ37777958() {     float oLfDLSTmyxcTQhHvDduO21069700 = -296967580;    float oLfDLSTmyxcTQhHvDduO93390760 = -828246339;    float oLfDLSTmyxcTQhHvDduO5449489 = -38917772;    float oLfDLSTmyxcTQhHvDduO92141082 = -856456062;    float oLfDLSTmyxcTQhHvDduO23535518 = -236483525;    float oLfDLSTmyxcTQhHvDduO62008762 = -355875497;    float oLfDLSTmyxcTQhHvDduO94926541 = -804426341;    float oLfDLSTmyxcTQhHvDduO63299695 = 75270546;    float oLfDLSTmyxcTQhHvDduO50985691 = -687804956;    float oLfDLSTmyxcTQhHvDduO2647364 = -263162071;    float oLfDLSTmyxcTQhHvDduO67087423 = -450863934;    float oLfDLSTmyxcTQhHvDduO45708441 = -418295844;    float oLfDLSTmyxcTQhHvDduO42635718 = 63555044;    float oLfDLSTmyxcTQhHvDduO28778103 = -857839157;    float oLfDLSTmyxcTQhHvDduO31773848 = -890673580;    float oLfDLSTmyxcTQhHvDduO27084407 = -867358340;    float oLfDLSTmyxcTQhHvDduO84589464 = -765656862;    float oLfDLSTmyxcTQhHvDduO1457549 = -338245073;    float oLfDLSTmyxcTQhHvDduO81112602 = -724661534;    float oLfDLSTmyxcTQhHvDduO53058006 = -376666442;    float oLfDLSTmyxcTQhHvDduO94125967 = -492501788;    float oLfDLSTmyxcTQhHvDduO47096297 = 12202502;    float oLfDLSTmyxcTQhHvDduO64030784 = -646830846;    float oLfDLSTmyxcTQhHvDduO98102664 = -831666993;    float oLfDLSTmyxcTQhHvDduO391792 = -950070601;    float oLfDLSTmyxcTQhHvDduO72377864 = -67436492;    float oLfDLSTmyxcTQhHvDduO36187696 = -48004202;    float oLfDLSTmyxcTQhHvDduO70074261 = -735865149;    float oLfDLSTmyxcTQhHvDduO87404393 = 93137285;    float oLfDLSTmyxcTQhHvDduO37102159 = -341446035;    float oLfDLSTmyxcTQhHvDduO50458089 = -613869392;    float oLfDLSTmyxcTQhHvDduO98018458 = -836197693;    float oLfDLSTmyxcTQhHvDduO88054266 = -547745426;    float oLfDLSTmyxcTQhHvDduO27090711 = 15149371;    float oLfDLSTmyxcTQhHvDduO93724252 = -835951056;    float oLfDLSTmyxcTQhHvDduO78297394 = -718345897;    float oLfDLSTmyxcTQhHvDduO24826366 = -709692160;    float oLfDLSTmyxcTQhHvDduO94896841 = -4511996;    float oLfDLSTmyxcTQhHvDduO64301471 = -490423082;    float oLfDLSTmyxcTQhHvDduO3778776 = -666744114;    float oLfDLSTmyxcTQhHvDduO59020872 = -144825193;    float oLfDLSTmyxcTQhHvDduO17459244 = -723010143;    float oLfDLSTmyxcTQhHvDduO23044356 = -590201629;    float oLfDLSTmyxcTQhHvDduO87553003 = -819012006;    float oLfDLSTmyxcTQhHvDduO87442661 = -231876718;    float oLfDLSTmyxcTQhHvDduO21566213 = 56607435;    float oLfDLSTmyxcTQhHvDduO31158977 = -771280099;    float oLfDLSTmyxcTQhHvDduO22424963 = -7537791;    float oLfDLSTmyxcTQhHvDduO66525903 = -964744426;    float oLfDLSTmyxcTQhHvDduO71668630 = -275106858;    float oLfDLSTmyxcTQhHvDduO48249396 = -858488633;    float oLfDLSTmyxcTQhHvDduO54935501 = -104551573;    float oLfDLSTmyxcTQhHvDduO75085694 = -74569413;    float oLfDLSTmyxcTQhHvDduO22418502 = -134332547;    float oLfDLSTmyxcTQhHvDduO43651990 = -205387555;    float oLfDLSTmyxcTQhHvDduO73973402 = -209170083;    float oLfDLSTmyxcTQhHvDduO29359977 = -81415494;    float oLfDLSTmyxcTQhHvDduO7346825 = -207250780;    float oLfDLSTmyxcTQhHvDduO91749291 = -906385461;    float oLfDLSTmyxcTQhHvDduO51157653 = -69047033;    float oLfDLSTmyxcTQhHvDduO25821067 = -207871296;    float oLfDLSTmyxcTQhHvDduO24852280 = 31438808;    float oLfDLSTmyxcTQhHvDduO75895302 = 82133261;    float oLfDLSTmyxcTQhHvDduO13883533 = -246358921;    float oLfDLSTmyxcTQhHvDduO52189275 = -649292679;    float oLfDLSTmyxcTQhHvDduO69068965 = -614666242;    float oLfDLSTmyxcTQhHvDduO57654174 = -870550418;    float oLfDLSTmyxcTQhHvDduO15545008 = -951594327;    float oLfDLSTmyxcTQhHvDduO35053851 = 78111899;    float oLfDLSTmyxcTQhHvDduO53476454 = -72327684;    float oLfDLSTmyxcTQhHvDduO2258042 = -57666180;    float oLfDLSTmyxcTQhHvDduO89692622 = -661144867;    float oLfDLSTmyxcTQhHvDduO37156077 = -847821992;    float oLfDLSTmyxcTQhHvDduO77333826 = 42082579;    float oLfDLSTmyxcTQhHvDduO94037133 = -131841250;    float oLfDLSTmyxcTQhHvDduO76666724 = -769491646;    float oLfDLSTmyxcTQhHvDduO24051942 = -397595869;    float oLfDLSTmyxcTQhHvDduO76477780 = -827818840;    float oLfDLSTmyxcTQhHvDduO10660003 = -499790275;    float oLfDLSTmyxcTQhHvDduO78825578 = -906678036;    float oLfDLSTmyxcTQhHvDduO41218888 = -296156393;    float oLfDLSTmyxcTQhHvDduO13762734 = 59533589;    float oLfDLSTmyxcTQhHvDduO3548359 = -771120723;    float oLfDLSTmyxcTQhHvDduO15735763 = -631755857;    float oLfDLSTmyxcTQhHvDduO88852762 = -482957403;    float oLfDLSTmyxcTQhHvDduO95522587 = -409317820;    float oLfDLSTmyxcTQhHvDduO22932764 = -661628280;    float oLfDLSTmyxcTQhHvDduO65635765 = -313412880;    float oLfDLSTmyxcTQhHvDduO83438720 = -779463075;    float oLfDLSTmyxcTQhHvDduO19750850 = -526780974;    float oLfDLSTmyxcTQhHvDduO48937417 = -536930403;    float oLfDLSTmyxcTQhHvDduO17479541 = -402441381;    float oLfDLSTmyxcTQhHvDduO3147551 = -98126535;    float oLfDLSTmyxcTQhHvDduO13143818 = -321376049;    float oLfDLSTmyxcTQhHvDduO77957709 = -358872818;    float oLfDLSTmyxcTQhHvDduO34168592 = -76264001;    float oLfDLSTmyxcTQhHvDduO41563941 = -705143404;    float oLfDLSTmyxcTQhHvDduO9160823 = -243842708;    float oLfDLSTmyxcTQhHvDduO35363728 = -69719328;    float oLfDLSTmyxcTQhHvDduO18373697 = -296967580;     oLfDLSTmyxcTQhHvDduO21069700 = oLfDLSTmyxcTQhHvDduO93390760;     oLfDLSTmyxcTQhHvDduO93390760 = oLfDLSTmyxcTQhHvDduO5449489;     oLfDLSTmyxcTQhHvDduO5449489 = oLfDLSTmyxcTQhHvDduO92141082;     oLfDLSTmyxcTQhHvDduO92141082 = oLfDLSTmyxcTQhHvDduO23535518;     oLfDLSTmyxcTQhHvDduO23535518 = oLfDLSTmyxcTQhHvDduO62008762;     oLfDLSTmyxcTQhHvDduO62008762 = oLfDLSTmyxcTQhHvDduO94926541;     oLfDLSTmyxcTQhHvDduO94926541 = oLfDLSTmyxcTQhHvDduO63299695;     oLfDLSTmyxcTQhHvDduO63299695 = oLfDLSTmyxcTQhHvDduO50985691;     oLfDLSTmyxcTQhHvDduO50985691 = oLfDLSTmyxcTQhHvDduO2647364;     oLfDLSTmyxcTQhHvDduO2647364 = oLfDLSTmyxcTQhHvDduO67087423;     oLfDLSTmyxcTQhHvDduO67087423 = oLfDLSTmyxcTQhHvDduO45708441;     oLfDLSTmyxcTQhHvDduO45708441 = oLfDLSTmyxcTQhHvDduO42635718;     oLfDLSTmyxcTQhHvDduO42635718 = oLfDLSTmyxcTQhHvDduO28778103;     oLfDLSTmyxcTQhHvDduO28778103 = oLfDLSTmyxcTQhHvDduO31773848;     oLfDLSTmyxcTQhHvDduO31773848 = oLfDLSTmyxcTQhHvDduO27084407;     oLfDLSTmyxcTQhHvDduO27084407 = oLfDLSTmyxcTQhHvDduO84589464;     oLfDLSTmyxcTQhHvDduO84589464 = oLfDLSTmyxcTQhHvDduO1457549;     oLfDLSTmyxcTQhHvDduO1457549 = oLfDLSTmyxcTQhHvDduO81112602;     oLfDLSTmyxcTQhHvDduO81112602 = oLfDLSTmyxcTQhHvDduO53058006;     oLfDLSTmyxcTQhHvDduO53058006 = oLfDLSTmyxcTQhHvDduO94125967;     oLfDLSTmyxcTQhHvDduO94125967 = oLfDLSTmyxcTQhHvDduO47096297;     oLfDLSTmyxcTQhHvDduO47096297 = oLfDLSTmyxcTQhHvDduO64030784;     oLfDLSTmyxcTQhHvDduO64030784 = oLfDLSTmyxcTQhHvDduO98102664;     oLfDLSTmyxcTQhHvDduO98102664 = oLfDLSTmyxcTQhHvDduO391792;     oLfDLSTmyxcTQhHvDduO391792 = oLfDLSTmyxcTQhHvDduO72377864;     oLfDLSTmyxcTQhHvDduO72377864 = oLfDLSTmyxcTQhHvDduO36187696;     oLfDLSTmyxcTQhHvDduO36187696 = oLfDLSTmyxcTQhHvDduO70074261;     oLfDLSTmyxcTQhHvDduO70074261 = oLfDLSTmyxcTQhHvDduO87404393;     oLfDLSTmyxcTQhHvDduO87404393 = oLfDLSTmyxcTQhHvDduO37102159;     oLfDLSTmyxcTQhHvDduO37102159 = oLfDLSTmyxcTQhHvDduO50458089;     oLfDLSTmyxcTQhHvDduO50458089 = oLfDLSTmyxcTQhHvDduO98018458;     oLfDLSTmyxcTQhHvDduO98018458 = oLfDLSTmyxcTQhHvDduO88054266;     oLfDLSTmyxcTQhHvDduO88054266 = oLfDLSTmyxcTQhHvDduO27090711;     oLfDLSTmyxcTQhHvDduO27090711 = oLfDLSTmyxcTQhHvDduO93724252;     oLfDLSTmyxcTQhHvDduO93724252 = oLfDLSTmyxcTQhHvDduO78297394;     oLfDLSTmyxcTQhHvDduO78297394 = oLfDLSTmyxcTQhHvDduO24826366;     oLfDLSTmyxcTQhHvDduO24826366 = oLfDLSTmyxcTQhHvDduO94896841;     oLfDLSTmyxcTQhHvDduO94896841 = oLfDLSTmyxcTQhHvDduO64301471;     oLfDLSTmyxcTQhHvDduO64301471 = oLfDLSTmyxcTQhHvDduO3778776;     oLfDLSTmyxcTQhHvDduO3778776 = oLfDLSTmyxcTQhHvDduO59020872;     oLfDLSTmyxcTQhHvDduO59020872 = oLfDLSTmyxcTQhHvDduO17459244;     oLfDLSTmyxcTQhHvDduO17459244 = oLfDLSTmyxcTQhHvDduO23044356;     oLfDLSTmyxcTQhHvDduO23044356 = oLfDLSTmyxcTQhHvDduO87553003;     oLfDLSTmyxcTQhHvDduO87553003 = oLfDLSTmyxcTQhHvDduO87442661;     oLfDLSTmyxcTQhHvDduO87442661 = oLfDLSTmyxcTQhHvDduO21566213;     oLfDLSTmyxcTQhHvDduO21566213 = oLfDLSTmyxcTQhHvDduO31158977;     oLfDLSTmyxcTQhHvDduO31158977 = oLfDLSTmyxcTQhHvDduO22424963;     oLfDLSTmyxcTQhHvDduO22424963 = oLfDLSTmyxcTQhHvDduO66525903;     oLfDLSTmyxcTQhHvDduO66525903 = oLfDLSTmyxcTQhHvDduO71668630;     oLfDLSTmyxcTQhHvDduO71668630 = oLfDLSTmyxcTQhHvDduO48249396;     oLfDLSTmyxcTQhHvDduO48249396 = oLfDLSTmyxcTQhHvDduO54935501;     oLfDLSTmyxcTQhHvDduO54935501 = oLfDLSTmyxcTQhHvDduO75085694;     oLfDLSTmyxcTQhHvDduO75085694 = oLfDLSTmyxcTQhHvDduO22418502;     oLfDLSTmyxcTQhHvDduO22418502 = oLfDLSTmyxcTQhHvDduO43651990;     oLfDLSTmyxcTQhHvDduO43651990 = oLfDLSTmyxcTQhHvDduO73973402;     oLfDLSTmyxcTQhHvDduO73973402 = oLfDLSTmyxcTQhHvDduO29359977;     oLfDLSTmyxcTQhHvDduO29359977 = oLfDLSTmyxcTQhHvDduO7346825;     oLfDLSTmyxcTQhHvDduO7346825 = oLfDLSTmyxcTQhHvDduO91749291;     oLfDLSTmyxcTQhHvDduO91749291 = oLfDLSTmyxcTQhHvDduO51157653;     oLfDLSTmyxcTQhHvDduO51157653 = oLfDLSTmyxcTQhHvDduO25821067;     oLfDLSTmyxcTQhHvDduO25821067 = oLfDLSTmyxcTQhHvDduO24852280;     oLfDLSTmyxcTQhHvDduO24852280 = oLfDLSTmyxcTQhHvDduO75895302;     oLfDLSTmyxcTQhHvDduO75895302 = oLfDLSTmyxcTQhHvDduO13883533;     oLfDLSTmyxcTQhHvDduO13883533 = oLfDLSTmyxcTQhHvDduO52189275;     oLfDLSTmyxcTQhHvDduO52189275 = oLfDLSTmyxcTQhHvDduO69068965;     oLfDLSTmyxcTQhHvDduO69068965 = oLfDLSTmyxcTQhHvDduO57654174;     oLfDLSTmyxcTQhHvDduO57654174 = oLfDLSTmyxcTQhHvDduO15545008;     oLfDLSTmyxcTQhHvDduO15545008 = oLfDLSTmyxcTQhHvDduO35053851;     oLfDLSTmyxcTQhHvDduO35053851 = oLfDLSTmyxcTQhHvDduO53476454;     oLfDLSTmyxcTQhHvDduO53476454 = oLfDLSTmyxcTQhHvDduO2258042;     oLfDLSTmyxcTQhHvDduO2258042 = oLfDLSTmyxcTQhHvDduO89692622;     oLfDLSTmyxcTQhHvDduO89692622 = oLfDLSTmyxcTQhHvDduO37156077;     oLfDLSTmyxcTQhHvDduO37156077 = oLfDLSTmyxcTQhHvDduO77333826;     oLfDLSTmyxcTQhHvDduO77333826 = oLfDLSTmyxcTQhHvDduO94037133;     oLfDLSTmyxcTQhHvDduO94037133 = oLfDLSTmyxcTQhHvDduO76666724;     oLfDLSTmyxcTQhHvDduO76666724 = oLfDLSTmyxcTQhHvDduO24051942;     oLfDLSTmyxcTQhHvDduO24051942 = oLfDLSTmyxcTQhHvDduO76477780;     oLfDLSTmyxcTQhHvDduO76477780 = oLfDLSTmyxcTQhHvDduO10660003;     oLfDLSTmyxcTQhHvDduO10660003 = oLfDLSTmyxcTQhHvDduO78825578;     oLfDLSTmyxcTQhHvDduO78825578 = oLfDLSTmyxcTQhHvDduO41218888;     oLfDLSTmyxcTQhHvDduO41218888 = oLfDLSTmyxcTQhHvDduO13762734;     oLfDLSTmyxcTQhHvDduO13762734 = oLfDLSTmyxcTQhHvDduO3548359;     oLfDLSTmyxcTQhHvDduO3548359 = oLfDLSTmyxcTQhHvDduO15735763;     oLfDLSTmyxcTQhHvDduO15735763 = oLfDLSTmyxcTQhHvDduO88852762;     oLfDLSTmyxcTQhHvDduO88852762 = oLfDLSTmyxcTQhHvDduO95522587;     oLfDLSTmyxcTQhHvDduO95522587 = oLfDLSTmyxcTQhHvDduO22932764;     oLfDLSTmyxcTQhHvDduO22932764 = oLfDLSTmyxcTQhHvDduO65635765;     oLfDLSTmyxcTQhHvDduO65635765 = oLfDLSTmyxcTQhHvDduO83438720;     oLfDLSTmyxcTQhHvDduO83438720 = oLfDLSTmyxcTQhHvDduO19750850;     oLfDLSTmyxcTQhHvDduO19750850 = oLfDLSTmyxcTQhHvDduO48937417;     oLfDLSTmyxcTQhHvDduO48937417 = oLfDLSTmyxcTQhHvDduO17479541;     oLfDLSTmyxcTQhHvDduO17479541 = oLfDLSTmyxcTQhHvDduO3147551;     oLfDLSTmyxcTQhHvDduO3147551 = oLfDLSTmyxcTQhHvDduO13143818;     oLfDLSTmyxcTQhHvDduO13143818 = oLfDLSTmyxcTQhHvDduO77957709;     oLfDLSTmyxcTQhHvDduO77957709 = oLfDLSTmyxcTQhHvDduO34168592;     oLfDLSTmyxcTQhHvDduO34168592 = oLfDLSTmyxcTQhHvDduO41563941;     oLfDLSTmyxcTQhHvDduO41563941 = oLfDLSTmyxcTQhHvDduO9160823;     oLfDLSTmyxcTQhHvDduO9160823 = oLfDLSTmyxcTQhHvDduO35363728;     oLfDLSTmyxcTQhHvDduO35363728 = oLfDLSTmyxcTQhHvDduO18373697;     oLfDLSTmyxcTQhHvDduO18373697 = oLfDLSTmyxcTQhHvDduO21069700;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void FoUMPodynXjsuDCxpQth44436702() {     float MXsHoEcoSYZnYRpDuXpM21063229 = -169491722;    float MXsHoEcoSYZnYRpDuXpM31963077 = -870841464;    float MXsHoEcoSYZnYRpDuXpM6762702 = 3953614;    float MXsHoEcoSYZnYRpDuXpM77220943 = -676996892;    float MXsHoEcoSYZnYRpDuXpM32375620 = -704838846;    float MXsHoEcoSYZnYRpDuXpM96814194 = -55308309;    float MXsHoEcoSYZnYRpDuXpM63473877 = -508962808;    float MXsHoEcoSYZnYRpDuXpM32513746 = -728906771;    float MXsHoEcoSYZnYRpDuXpM33725855 = -401451026;    float MXsHoEcoSYZnYRpDuXpM99286877 = -999968942;    float MXsHoEcoSYZnYRpDuXpM87408028 = -410336006;    float MXsHoEcoSYZnYRpDuXpM60000118 = -153774206;    float MXsHoEcoSYZnYRpDuXpM74488632 = 41896569;    float MXsHoEcoSYZnYRpDuXpM82622323 = -102992209;    float MXsHoEcoSYZnYRpDuXpM76945957 = -853386432;    float MXsHoEcoSYZnYRpDuXpM1648955 = -100153990;    float MXsHoEcoSYZnYRpDuXpM40442224 = -585271431;    float MXsHoEcoSYZnYRpDuXpM24458081 = -735316695;    float MXsHoEcoSYZnYRpDuXpM1540872 = -242643801;    float MXsHoEcoSYZnYRpDuXpM25131769 = -988690894;    float MXsHoEcoSYZnYRpDuXpM73795391 = -202968162;    float MXsHoEcoSYZnYRpDuXpM25392563 = -497789381;    float MXsHoEcoSYZnYRpDuXpM909181 = -5788688;    float MXsHoEcoSYZnYRpDuXpM41752773 = -712587515;    float MXsHoEcoSYZnYRpDuXpM28834481 = -276764454;    float MXsHoEcoSYZnYRpDuXpM88951941 = 24357580;    float MXsHoEcoSYZnYRpDuXpM6409278 = -348803914;    float MXsHoEcoSYZnYRpDuXpM5477787 = -244832137;    float MXsHoEcoSYZnYRpDuXpM82841 = -552933030;    float MXsHoEcoSYZnYRpDuXpM13192029 = -16877409;    float MXsHoEcoSYZnYRpDuXpM83870767 = 99037373;    float MXsHoEcoSYZnYRpDuXpM82742315 = -128019454;    float MXsHoEcoSYZnYRpDuXpM73506690 = -749689642;    float MXsHoEcoSYZnYRpDuXpM27373548 = -961532362;    float MXsHoEcoSYZnYRpDuXpM76082290 = 4735019;    float MXsHoEcoSYZnYRpDuXpM21125584 = -798111643;    float MXsHoEcoSYZnYRpDuXpM36767733 = -809345539;    float MXsHoEcoSYZnYRpDuXpM64814521 = 225325;    float MXsHoEcoSYZnYRpDuXpM99615067 = -103104498;    float MXsHoEcoSYZnYRpDuXpM75651668 = -276175800;    float MXsHoEcoSYZnYRpDuXpM46876539 = -232769092;    float MXsHoEcoSYZnYRpDuXpM3156905 = -916802225;    float MXsHoEcoSYZnYRpDuXpM83327715 = -49331883;    float MXsHoEcoSYZnYRpDuXpM46784902 = -375976449;    float MXsHoEcoSYZnYRpDuXpM71460713 = -705136119;    float MXsHoEcoSYZnYRpDuXpM61994181 = -855437236;    float MXsHoEcoSYZnYRpDuXpM58856769 = -152560722;    float MXsHoEcoSYZnYRpDuXpM97808543 = -616299147;    float MXsHoEcoSYZnYRpDuXpM85184292 = -528508287;    float MXsHoEcoSYZnYRpDuXpM91714053 = -962131745;    float MXsHoEcoSYZnYRpDuXpM61508766 = -437043640;    float MXsHoEcoSYZnYRpDuXpM69482758 = -486697321;    float MXsHoEcoSYZnYRpDuXpM50534042 = 33376579;    float MXsHoEcoSYZnYRpDuXpM32668773 = -666252398;    float MXsHoEcoSYZnYRpDuXpM70641310 = 37713168;    float MXsHoEcoSYZnYRpDuXpM95670665 = -671702341;    float MXsHoEcoSYZnYRpDuXpM31053897 = -765052777;    float MXsHoEcoSYZnYRpDuXpM65009929 = -283458871;    float MXsHoEcoSYZnYRpDuXpM48386463 = -300232438;    float MXsHoEcoSYZnYRpDuXpM43423678 = -629196427;    float MXsHoEcoSYZnYRpDuXpM90404917 = -706504395;    float MXsHoEcoSYZnYRpDuXpM57996091 = -164130671;    float MXsHoEcoSYZnYRpDuXpM32430906 = -75973741;    float MXsHoEcoSYZnYRpDuXpM20533827 = -284573618;    float MXsHoEcoSYZnYRpDuXpM15416110 = -999006315;    float MXsHoEcoSYZnYRpDuXpM4665714 = -182316553;    float MXsHoEcoSYZnYRpDuXpM86493428 = -404084565;    float MXsHoEcoSYZnYRpDuXpM47115084 = 3428931;    float MXsHoEcoSYZnYRpDuXpM6540033 = -7727228;    float MXsHoEcoSYZnYRpDuXpM55820373 = 44725211;    float MXsHoEcoSYZnYRpDuXpM64881221 = -290808452;    float MXsHoEcoSYZnYRpDuXpM75627703 = -485496757;    float MXsHoEcoSYZnYRpDuXpM24843014 = -532212197;    float MXsHoEcoSYZnYRpDuXpM25889204 = -966468002;    float MXsHoEcoSYZnYRpDuXpM78255229 = -655921802;    float MXsHoEcoSYZnYRpDuXpM70638487 = -286165937;    float MXsHoEcoSYZnYRpDuXpM42064848 = -348457498;    float MXsHoEcoSYZnYRpDuXpM54124279 = -629812239;    float MXsHoEcoSYZnYRpDuXpM70292059 = 92548604;    float MXsHoEcoSYZnYRpDuXpM66840300 = -421327219;    float MXsHoEcoSYZnYRpDuXpM30095172 = -823081699;    float MXsHoEcoSYZnYRpDuXpM8600735 = -732504768;    float MXsHoEcoSYZnYRpDuXpM20293494 = -716323850;    float MXsHoEcoSYZnYRpDuXpM8368787 = -590801286;    float MXsHoEcoSYZnYRpDuXpM51683263 = -579833769;    float MXsHoEcoSYZnYRpDuXpM14388010 = -414265306;    float MXsHoEcoSYZnYRpDuXpM32208273 = -61396033;    float MXsHoEcoSYZnYRpDuXpM40837918 = 16562756;    float MXsHoEcoSYZnYRpDuXpM56732237 = -899245530;    float MXsHoEcoSYZnYRpDuXpM80411625 = -323562640;    float MXsHoEcoSYZnYRpDuXpM90071687 = 66941134;    float MXsHoEcoSYZnYRpDuXpM71757804 = -425886668;    float MXsHoEcoSYZnYRpDuXpM16428059 = -699542237;    float MXsHoEcoSYZnYRpDuXpM56191389 = -473908072;    float MXsHoEcoSYZnYRpDuXpM85246751 = -569671405;    float MXsHoEcoSYZnYRpDuXpM88880448 = 31361579;    float MXsHoEcoSYZnYRpDuXpM70725998 = -740828484;    float MXsHoEcoSYZnYRpDuXpM62793889 = -764758266;    float MXsHoEcoSYZnYRpDuXpM31368792 = -376970135;    float MXsHoEcoSYZnYRpDuXpM66795000 = -169491722;     MXsHoEcoSYZnYRpDuXpM21063229 = MXsHoEcoSYZnYRpDuXpM31963077;     MXsHoEcoSYZnYRpDuXpM31963077 = MXsHoEcoSYZnYRpDuXpM6762702;     MXsHoEcoSYZnYRpDuXpM6762702 = MXsHoEcoSYZnYRpDuXpM77220943;     MXsHoEcoSYZnYRpDuXpM77220943 = MXsHoEcoSYZnYRpDuXpM32375620;     MXsHoEcoSYZnYRpDuXpM32375620 = MXsHoEcoSYZnYRpDuXpM96814194;     MXsHoEcoSYZnYRpDuXpM96814194 = MXsHoEcoSYZnYRpDuXpM63473877;     MXsHoEcoSYZnYRpDuXpM63473877 = MXsHoEcoSYZnYRpDuXpM32513746;     MXsHoEcoSYZnYRpDuXpM32513746 = MXsHoEcoSYZnYRpDuXpM33725855;     MXsHoEcoSYZnYRpDuXpM33725855 = MXsHoEcoSYZnYRpDuXpM99286877;     MXsHoEcoSYZnYRpDuXpM99286877 = MXsHoEcoSYZnYRpDuXpM87408028;     MXsHoEcoSYZnYRpDuXpM87408028 = MXsHoEcoSYZnYRpDuXpM60000118;     MXsHoEcoSYZnYRpDuXpM60000118 = MXsHoEcoSYZnYRpDuXpM74488632;     MXsHoEcoSYZnYRpDuXpM74488632 = MXsHoEcoSYZnYRpDuXpM82622323;     MXsHoEcoSYZnYRpDuXpM82622323 = MXsHoEcoSYZnYRpDuXpM76945957;     MXsHoEcoSYZnYRpDuXpM76945957 = MXsHoEcoSYZnYRpDuXpM1648955;     MXsHoEcoSYZnYRpDuXpM1648955 = MXsHoEcoSYZnYRpDuXpM40442224;     MXsHoEcoSYZnYRpDuXpM40442224 = MXsHoEcoSYZnYRpDuXpM24458081;     MXsHoEcoSYZnYRpDuXpM24458081 = MXsHoEcoSYZnYRpDuXpM1540872;     MXsHoEcoSYZnYRpDuXpM1540872 = MXsHoEcoSYZnYRpDuXpM25131769;     MXsHoEcoSYZnYRpDuXpM25131769 = MXsHoEcoSYZnYRpDuXpM73795391;     MXsHoEcoSYZnYRpDuXpM73795391 = MXsHoEcoSYZnYRpDuXpM25392563;     MXsHoEcoSYZnYRpDuXpM25392563 = MXsHoEcoSYZnYRpDuXpM909181;     MXsHoEcoSYZnYRpDuXpM909181 = MXsHoEcoSYZnYRpDuXpM41752773;     MXsHoEcoSYZnYRpDuXpM41752773 = MXsHoEcoSYZnYRpDuXpM28834481;     MXsHoEcoSYZnYRpDuXpM28834481 = MXsHoEcoSYZnYRpDuXpM88951941;     MXsHoEcoSYZnYRpDuXpM88951941 = MXsHoEcoSYZnYRpDuXpM6409278;     MXsHoEcoSYZnYRpDuXpM6409278 = MXsHoEcoSYZnYRpDuXpM5477787;     MXsHoEcoSYZnYRpDuXpM5477787 = MXsHoEcoSYZnYRpDuXpM82841;     MXsHoEcoSYZnYRpDuXpM82841 = MXsHoEcoSYZnYRpDuXpM13192029;     MXsHoEcoSYZnYRpDuXpM13192029 = MXsHoEcoSYZnYRpDuXpM83870767;     MXsHoEcoSYZnYRpDuXpM83870767 = MXsHoEcoSYZnYRpDuXpM82742315;     MXsHoEcoSYZnYRpDuXpM82742315 = MXsHoEcoSYZnYRpDuXpM73506690;     MXsHoEcoSYZnYRpDuXpM73506690 = MXsHoEcoSYZnYRpDuXpM27373548;     MXsHoEcoSYZnYRpDuXpM27373548 = MXsHoEcoSYZnYRpDuXpM76082290;     MXsHoEcoSYZnYRpDuXpM76082290 = MXsHoEcoSYZnYRpDuXpM21125584;     MXsHoEcoSYZnYRpDuXpM21125584 = MXsHoEcoSYZnYRpDuXpM36767733;     MXsHoEcoSYZnYRpDuXpM36767733 = MXsHoEcoSYZnYRpDuXpM64814521;     MXsHoEcoSYZnYRpDuXpM64814521 = MXsHoEcoSYZnYRpDuXpM99615067;     MXsHoEcoSYZnYRpDuXpM99615067 = MXsHoEcoSYZnYRpDuXpM75651668;     MXsHoEcoSYZnYRpDuXpM75651668 = MXsHoEcoSYZnYRpDuXpM46876539;     MXsHoEcoSYZnYRpDuXpM46876539 = MXsHoEcoSYZnYRpDuXpM3156905;     MXsHoEcoSYZnYRpDuXpM3156905 = MXsHoEcoSYZnYRpDuXpM83327715;     MXsHoEcoSYZnYRpDuXpM83327715 = MXsHoEcoSYZnYRpDuXpM46784902;     MXsHoEcoSYZnYRpDuXpM46784902 = MXsHoEcoSYZnYRpDuXpM71460713;     MXsHoEcoSYZnYRpDuXpM71460713 = MXsHoEcoSYZnYRpDuXpM61994181;     MXsHoEcoSYZnYRpDuXpM61994181 = MXsHoEcoSYZnYRpDuXpM58856769;     MXsHoEcoSYZnYRpDuXpM58856769 = MXsHoEcoSYZnYRpDuXpM97808543;     MXsHoEcoSYZnYRpDuXpM97808543 = MXsHoEcoSYZnYRpDuXpM85184292;     MXsHoEcoSYZnYRpDuXpM85184292 = MXsHoEcoSYZnYRpDuXpM91714053;     MXsHoEcoSYZnYRpDuXpM91714053 = MXsHoEcoSYZnYRpDuXpM61508766;     MXsHoEcoSYZnYRpDuXpM61508766 = MXsHoEcoSYZnYRpDuXpM69482758;     MXsHoEcoSYZnYRpDuXpM69482758 = MXsHoEcoSYZnYRpDuXpM50534042;     MXsHoEcoSYZnYRpDuXpM50534042 = MXsHoEcoSYZnYRpDuXpM32668773;     MXsHoEcoSYZnYRpDuXpM32668773 = MXsHoEcoSYZnYRpDuXpM70641310;     MXsHoEcoSYZnYRpDuXpM70641310 = MXsHoEcoSYZnYRpDuXpM95670665;     MXsHoEcoSYZnYRpDuXpM95670665 = MXsHoEcoSYZnYRpDuXpM31053897;     MXsHoEcoSYZnYRpDuXpM31053897 = MXsHoEcoSYZnYRpDuXpM65009929;     MXsHoEcoSYZnYRpDuXpM65009929 = MXsHoEcoSYZnYRpDuXpM48386463;     MXsHoEcoSYZnYRpDuXpM48386463 = MXsHoEcoSYZnYRpDuXpM43423678;     MXsHoEcoSYZnYRpDuXpM43423678 = MXsHoEcoSYZnYRpDuXpM90404917;     MXsHoEcoSYZnYRpDuXpM90404917 = MXsHoEcoSYZnYRpDuXpM57996091;     MXsHoEcoSYZnYRpDuXpM57996091 = MXsHoEcoSYZnYRpDuXpM32430906;     MXsHoEcoSYZnYRpDuXpM32430906 = MXsHoEcoSYZnYRpDuXpM20533827;     MXsHoEcoSYZnYRpDuXpM20533827 = MXsHoEcoSYZnYRpDuXpM15416110;     MXsHoEcoSYZnYRpDuXpM15416110 = MXsHoEcoSYZnYRpDuXpM4665714;     MXsHoEcoSYZnYRpDuXpM4665714 = MXsHoEcoSYZnYRpDuXpM86493428;     MXsHoEcoSYZnYRpDuXpM86493428 = MXsHoEcoSYZnYRpDuXpM47115084;     MXsHoEcoSYZnYRpDuXpM47115084 = MXsHoEcoSYZnYRpDuXpM6540033;     MXsHoEcoSYZnYRpDuXpM6540033 = MXsHoEcoSYZnYRpDuXpM55820373;     MXsHoEcoSYZnYRpDuXpM55820373 = MXsHoEcoSYZnYRpDuXpM64881221;     MXsHoEcoSYZnYRpDuXpM64881221 = MXsHoEcoSYZnYRpDuXpM75627703;     MXsHoEcoSYZnYRpDuXpM75627703 = MXsHoEcoSYZnYRpDuXpM24843014;     MXsHoEcoSYZnYRpDuXpM24843014 = MXsHoEcoSYZnYRpDuXpM25889204;     MXsHoEcoSYZnYRpDuXpM25889204 = MXsHoEcoSYZnYRpDuXpM78255229;     MXsHoEcoSYZnYRpDuXpM78255229 = MXsHoEcoSYZnYRpDuXpM70638487;     MXsHoEcoSYZnYRpDuXpM70638487 = MXsHoEcoSYZnYRpDuXpM42064848;     MXsHoEcoSYZnYRpDuXpM42064848 = MXsHoEcoSYZnYRpDuXpM54124279;     MXsHoEcoSYZnYRpDuXpM54124279 = MXsHoEcoSYZnYRpDuXpM70292059;     MXsHoEcoSYZnYRpDuXpM70292059 = MXsHoEcoSYZnYRpDuXpM66840300;     MXsHoEcoSYZnYRpDuXpM66840300 = MXsHoEcoSYZnYRpDuXpM30095172;     MXsHoEcoSYZnYRpDuXpM30095172 = MXsHoEcoSYZnYRpDuXpM8600735;     MXsHoEcoSYZnYRpDuXpM8600735 = MXsHoEcoSYZnYRpDuXpM20293494;     MXsHoEcoSYZnYRpDuXpM20293494 = MXsHoEcoSYZnYRpDuXpM8368787;     MXsHoEcoSYZnYRpDuXpM8368787 = MXsHoEcoSYZnYRpDuXpM51683263;     MXsHoEcoSYZnYRpDuXpM51683263 = MXsHoEcoSYZnYRpDuXpM14388010;     MXsHoEcoSYZnYRpDuXpM14388010 = MXsHoEcoSYZnYRpDuXpM32208273;     MXsHoEcoSYZnYRpDuXpM32208273 = MXsHoEcoSYZnYRpDuXpM40837918;     MXsHoEcoSYZnYRpDuXpM40837918 = MXsHoEcoSYZnYRpDuXpM56732237;     MXsHoEcoSYZnYRpDuXpM56732237 = MXsHoEcoSYZnYRpDuXpM80411625;     MXsHoEcoSYZnYRpDuXpM80411625 = MXsHoEcoSYZnYRpDuXpM90071687;     MXsHoEcoSYZnYRpDuXpM90071687 = MXsHoEcoSYZnYRpDuXpM71757804;     MXsHoEcoSYZnYRpDuXpM71757804 = MXsHoEcoSYZnYRpDuXpM16428059;     MXsHoEcoSYZnYRpDuXpM16428059 = MXsHoEcoSYZnYRpDuXpM56191389;     MXsHoEcoSYZnYRpDuXpM56191389 = MXsHoEcoSYZnYRpDuXpM85246751;     MXsHoEcoSYZnYRpDuXpM85246751 = MXsHoEcoSYZnYRpDuXpM88880448;     MXsHoEcoSYZnYRpDuXpM88880448 = MXsHoEcoSYZnYRpDuXpM70725998;     MXsHoEcoSYZnYRpDuXpM70725998 = MXsHoEcoSYZnYRpDuXpM62793889;     MXsHoEcoSYZnYRpDuXpM62793889 = MXsHoEcoSYZnYRpDuXpM31368792;     MXsHoEcoSYZnYRpDuXpM31368792 = MXsHoEcoSYZnYRpDuXpM66795000;     MXsHoEcoSYZnYRpDuXpM66795000 = MXsHoEcoSYZnYRpDuXpM21063229;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IGDIsMeIPXHcNjwxuQoB4086794() {     float JekRTQThknPFcAqVYcId48112833 = -125768092;    float JekRTQThknPFcAqVYcId90092801 = -245705426;    float JekRTQThknPFcAqVYcId19623111 = -949674076;    float JekRTQThknPFcAqVYcId82312747 = -638830797;    float JekRTQThknPFcAqVYcId93951416 = -932886935;    float JekRTQThknPFcAqVYcId82364918 = -420776508;    float JekRTQThknPFcAqVYcId66046150 = -177172202;    float JekRTQThknPFcAqVYcId805145 = -419850002;    float JekRTQThknPFcAqVYcId65238192 = -415442848;    float JekRTQThknPFcAqVYcId77883055 = -650502955;    float JekRTQThknPFcAqVYcId78160426 = -532976889;    float JekRTQThknPFcAqVYcId60803184 = -371595635;    float JekRTQThknPFcAqVYcId15497985 = -628287962;    float JekRTQThknPFcAqVYcId75898231 = -361363229;    float JekRTQThknPFcAqVYcId37440171 = -434594712;    float JekRTQThknPFcAqVYcId67466718 = -520986534;    float JekRTQThknPFcAqVYcId20281391 = 88367133;    float JekRTQThknPFcAqVYcId94280418 = 70715674;    float JekRTQThknPFcAqVYcId43507766 = -441079641;    float JekRTQThknPFcAqVYcId81190403 = -271660929;    float JekRTQThknPFcAqVYcId9266107 = -574455654;    float JekRTQThknPFcAqVYcId17590039 = -806509948;    float JekRTQThknPFcAqVYcId94574339 = -469915398;    float JekRTQThknPFcAqVYcId65118293 = -574470536;    float JekRTQThknPFcAqVYcId49573982 = -562869715;    float JekRTQThknPFcAqVYcId72778286 = 12011982;    float JekRTQThknPFcAqVYcId92711645 = -285235045;    float JekRTQThknPFcAqVYcId20383831 = -930129716;    float JekRTQThknPFcAqVYcId81025019 = -744847131;    float JekRTQThknPFcAqVYcId46672280 = -633642026;    float JekRTQThknPFcAqVYcId38101253 = -597284762;    float JekRTQThknPFcAqVYcId90370295 = -151810135;    float JekRTQThknPFcAqVYcId65385708 = -758120481;    float JekRTQThknPFcAqVYcId81064581 = -294927856;    float JekRTQThknPFcAqVYcId33868264 = 47257743;    float JekRTQThknPFcAqVYcId96609757 = -986795433;    float JekRTQThknPFcAqVYcId14985224 = -543153388;    float JekRTQThknPFcAqVYcId94020761 = -847250819;    float JekRTQThknPFcAqVYcId4550852 = -820247361;    float JekRTQThknPFcAqVYcId54982662 = -833918821;    float JekRTQThknPFcAqVYcId46773727 = 86432957;    float JekRTQThknPFcAqVYcId46217878 = 59997100;    float JekRTQThknPFcAqVYcId29523891 = -728454967;    float JekRTQThknPFcAqVYcId56183547 = -400814210;    float JekRTQThknPFcAqVYcId44405478 = -330222943;    float JekRTQThknPFcAqVYcId74893104 = -516400108;    float JekRTQThknPFcAqVYcId3005638 = -706427580;    float JekRTQThknPFcAqVYcId46605051 = -964534815;    float JekRTQThknPFcAqVYcId73663977 = -648250348;    float JekRTQThknPFcAqVYcId58582682 = -760390190;    float JekRTQThknPFcAqVYcId78917309 = -976141660;    float JekRTQThknPFcAqVYcId64532093 = -679900401;    float JekRTQThknPFcAqVYcId41266013 = -338382598;    float JekRTQThknPFcAqVYcId87558236 = -447919877;    float JekRTQThknPFcAqVYcId94820687 = -802775531;    float JekRTQThknPFcAqVYcId30522795 = -319258144;    float JekRTQThknPFcAqVYcId95518461 = -775790029;    float JekRTQThknPFcAqVYcId54504818 = -275203541;    float JekRTQThknPFcAqVYcId32738766 = 24038917;    float JekRTQThknPFcAqVYcId21173130 = -844898917;    float JekRTQThknPFcAqVYcId89653273 = -35541464;    float JekRTQThknPFcAqVYcId45662320 = -247042487;    float JekRTQThknPFcAqVYcId19780126 = -675002871;    float JekRTQThknPFcAqVYcId18565912 = -781800822;    float JekRTQThknPFcAqVYcId39781803 = 46781806;    float JekRTQThknPFcAqVYcId87790131 = -281166755;    float JekRTQThknPFcAqVYcId95417476 = -613475155;    float JekRTQThknPFcAqVYcId34433404 = -233360106;    float JekRTQThknPFcAqVYcId42029967 = -308620973;    float JekRTQThknPFcAqVYcId40830413 = -447799279;    float JekRTQThknPFcAqVYcId52481495 = -977833146;    float JekRTQThknPFcAqVYcId26260629 = -64382049;    float JekRTQThknPFcAqVYcId89729566 = -109036965;    float JekRTQThknPFcAqVYcId88525104 = -607160821;    float JekRTQThknPFcAqVYcId34416677 = -258093887;    float JekRTQThknPFcAqVYcId63048228 = -534452754;    float JekRTQThknPFcAqVYcId88066148 = 21945019;    float JekRTQThknPFcAqVYcId38390792 = 30898811;    float JekRTQThknPFcAqVYcId20712815 = -144247593;    float JekRTQThknPFcAqVYcId74680877 = 53530393;    float JekRTQThknPFcAqVYcId69772649 = -281560438;    float JekRTQThknPFcAqVYcId46106594 = -320700230;    float JekRTQThknPFcAqVYcId46719853 = -181879368;    float JekRTQThknPFcAqVYcId22442337 = -984456942;    float JekRTQThknPFcAqVYcId67754970 = -657500367;    float JekRTQThknPFcAqVYcId73569159 = -917384361;    float JekRTQThknPFcAqVYcId49104282 = -813427537;    float JekRTQThknPFcAqVYcId77827471 = -210200604;    float JekRTQThknPFcAqVYcId86243894 = -492152325;    float JekRTQThknPFcAqVYcId3345470 = -633484113;    float JekRTQThknPFcAqVYcId1091297 = -111005405;    float JekRTQThknPFcAqVYcId60480405 = -167949847;    float JekRTQThknPFcAqVYcId61281996 = -771289736;    float JekRTQThknPFcAqVYcId83377722 = -975348444;    float JekRTQThknPFcAqVYcId65329389 = -698377357;    float JekRTQThknPFcAqVYcId1111407 = -666524556;    float JekRTQThknPFcAqVYcId26437752 = -265000030;    float JekRTQThknPFcAqVYcId10957979 = -946654145;    float JekRTQThknPFcAqVYcId16401745 = -347596016;    float JekRTQThknPFcAqVYcId56615347 = -125768092;     JekRTQThknPFcAqVYcId48112833 = JekRTQThknPFcAqVYcId90092801;     JekRTQThknPFcAqVYcId90092801 = JekRTQThknPFcAqVYcId19623111;     JekRTQThknPFcAqVYcId19623111 = JekRTQThknPFcAqVYcId82312747;     JekRTQThknPFcAqVYcId82312747 = JekRTQThknPFcAqVYcId93951416;     JekRTQThknPFcAqVYcId93951416 = JekRTQThknPFcAqVYcId82364918;     JekRTQThknPFcAqVYcId82364918 = JekRTQThknPFcAqVYcId66046150;     JekRTQThknPFcAqVYcId66046150 = JekRTQThknPFcAqVYcId805145;     JekRTQThknPFcAqVYcId805145 = JekRTQThknPFcAqVYcId65238192;     JekRTQThknPFcAqVYcId65238192 = JekRTQThknPFcAqVYcId77883055;     JekRTQThknPFcAqVYcId77883055 = JekRTQThknPFcAqVYcId78160426;     JekRTQThknPFcAqVYcId78160426 = JekRTQThknPFcAqVYcId60803184;     JekRTQThknPFcAqVYcId60803184 = JekRTQThknPFcAqVYcId15497985;     JekRTQThknPFcAqVYcId15497985 = JekRTQThknPFcAqVYcId75898231;     JekRTQThknPFcAqVYcId75898231 = JekRTQThknPFcAqVYcId37440171;     JekRTQThknPFcAqVYcId37440171 = JekRTQThknPFcAqVYcId67466718;     JekRTQThknPFcAqVYcId67466718 = JekRTQThknPFcAqVYcId20281391;     JekRTQThknPFcAqVYcId20281391 = JekRTQThknPFcAqVYcId94280418;     JekRTQThknPFcAqVYcId94280418 = JekRTQThknPFcAqVYcId43507766;     JekRTQThknPFcAqVYcId43507766 = JekRTQThknPFcAqVYcId81190403;     JekRTQThknPFcAqVYcId81190403 = JekRTQThknPFcAqVYcId9266107;     JekRTQThknPFcAqVYcId9266107 = JekRTQThknPFcAqVYcId17590039;     JekRTQThknPFcAqVYcId17590039 = JekRTQThknPFcAqVYcId94574339;     JekRTQThknPFcAqVYcId94574339 = JekRTQThknPFcAqVYcId65118293;     JekRTQThknPFcAqVYcId65118293 = JekRTQThknPFcAqVYcId49573982;     JekRTQThknPFcAqVYcId49573982 = JekRTQThknPFcAqVYcId72778286;     JekRTQThknPFcAqVYcId72778286 = JekRTQThknPFcAqVYcId92711645;     JekRTQThknPFcAqVYcId92711645 = JekRTQThknPFcAqVYcId20383831;     JekRTQThknPFcAqVYcId20383831 = JekRTQThknPFcAqVYcId81025019;     JekRTQThknPFcAqVYcId81025019 = JekRTQThknPFcAqVYcId46672280;     JekRTQThknPFcAqVYcId46672280 = JekRTQThknPFcAqVYcId38101253;     JekRTQThknPFcAqVYcId38101253 = JekRTQThknPFcAqVYcId90370295;     JekRTQThknPFcAqVYcId90370295 = JekRTQThknPFcAqVYcId65385708;     JekRTQThknPFcAqVYcId65385708 = JekRTQThknPFcAqVYcId81064581;     JekRTQThknPFcAqVYcId81064581 = JekRTQThknPFcAqVYcId33868264;     JekRTQThknPFcAqVYcId33868264 = JekRTQThknPFcAqVYcId96609757;     JekRTQThknPFcAqVYcId96609757 = JekRTQThknPFcAqVYcId14985224;     JekRTQThknPFcAqVYcId14985224 = JekRTQThknPFcAqVYcId94020761;     JekRTQThknPFcAqVYcId94020761 = JekRTQThknPFcAqVYcId4550852;     JekRTQThknPFcAqVYcId4550852 = JekRTQThknPFcAqVYcId54982662;     JekRTQThknPFcAqVYcId54982662 = JekRTQThknPFcAqVYcId46773727;     JekRTQThknPFcAqVYcId46773727 = JekRTQThknPFcAqVYcId46217878;     JekRTQThknPFcAqVYcId46217878 = JekRTQThknPFcAqVYcId29523891;     JekRTQThknPFcAqVYcId29523891 = JekRTQThknPFcAqVYcId56183547;     JekRTQThknPFcAqVYcId56183547 = JekRTQThknPFcAqVYcId44405478;     JekRTQThknPFcAqVYcId44405478 = JekRTQThknPFcAqVYcId74893104;     JekRTQThknPFcAqVYcId74893104 = JekRTQThknPFcAqVYcId3005638;     JekRTQThknPFcAqVYcId3005638 = JekRTQThknPFcAqVYcId46605051;     JekRTQThknPFcAqVYcId46605051 = JekRTQThknPFcAqVYcId73663977;     JekRTQThknPFcAqVYcId73663977 = JekRTQThknPFcAqVYcId58582682;     JekRTQThknPFcAqVYcId58582682 = JekRTQThknPFcAqVYcId78917309;     JekRTQThknPFcAqVYcId78917309 = JekRTQThknPFcAqVYcId64532093;     JekRTQThknPFcAqVYcId64532093 = JekRTQThknPFcAqVYcId41266013;     JekRTQThknPFcAqVYcId41266013 = JekRTQThknPFcAqVYcId87558236;     JekRTQThknPFcAqVYcId87558236 = JekRTQThknPFcAqVYcId94820687;     JekRTQThknPFcAqVYcId94820687 = JekRTQThknPFcAqVYcId30522795;     JekRTQThknPFcAqVYcId30522795 = JekRTQThknPFcAqVYcId95518461;     JekRTQThknPFcAqVYcId95518461 = JekRTQThknPFcAqVYcId54504818;     JekRTQThknPFcAqVYcId54504818 = JekRTQThknPFcAqVYcId32738766;     JekRTQThknPFcAqVYcId32738766 = JekRTQThknPFcAqVYcId21173130;     JekRTQThknPFcAqVYcId21173130 = JekRTQThknPFcAqVYcId89653273;     JekRTQThknPFcAqVYcId89653273 = JekRTQThknPFcAqVYcId45662320;     JekRTQThknPFcAqVYcId45662320 = JekRTQThknPFcAqVYcId19780126;     JekRTQThknPFcAqVYcId19780126 = JekRTQThknPFcAqVYcId18565912;     JekRTQThknPFcAqVYcId18565912 = JekRTQThknPFcAqVYcId39781803;     JekRTQThknPFcAqVYcId39781803 = JekRTQThknPFcAqVYcId87790131;     JekRTQThknPFcAqVYcId87790131 = JekRTQThknPFcAqVYcId95417476;     JekRTQThknPFcAqVYcId95417476 = JekRTQThknPFcAqVYcId34433404;     JekRTQThknPFcAqVYcId34433404 = JekRTQThknPFcAqVYcId42029967;     JekRTQThknPFcAqVYcId42029967 = JekRTQThknPFcAqVYcId40830413;     JekRTQThknPFcAqVYcId40830413 = JekRTQThknPFcAqVYcId52481495;     JekRTQThknPFcAqVYcId52481495 = JekRTQThknPFcAqVYcId26260629;     JekRTQThknPFcAqVYcId26260629 = JekRTQThknPFcAqVYcId89729566;     JekRTQThknPFcAqVYcId89729566 = JekRTQThknPFcAqVYcId88525104;     JekRTQThknPFcAqVYcId88525104 = JekRTQThknPFcAqVYcId34416677;     JekRTQThknPFcAqVYcId34416677 = JekRTQThknPFcAqVYcId63048228;     JekRTQThknPFcAqVYcId63048228 = JekRTQThknPFcAqVYcId88066148;     JekRTQThknPFcAqVYcId88066148 = JekRTQThknPFcAqVYcId38390792;     JekRTQThknPFcAqVYcId38390792 = JekRTQThknPFcAqVYcId20712815;     JekRTQThknPFcAqVYcId20712815 = JekRTQThknPFcAqVYcId74680877;     JekRTQThknPFcAqVYcId74680877 = JekRTQThknPFcAqVYcId69772649;     JekRTQThknPFcAqVYcId69772649 = JekRTQThknPFcAqVYcId46106594;     JekRTQThknPFcAqVYcId46106594 = JekRTQThknPFcAqVYcId46719853;     JekRTQThknPFcAqVYcId46719853 = JekRTQThknPFcAqVYcId22442337;     JekRTQThknPFcAqVYcId22442337 = JekRTQThknPFcAqVYcId67754970;     JekRTQThknPFcAqVYcId67754970 = JekRTQThknPFcAqVYcId73569159;     JekRTQThknPFcAqVYcId73569159 = JekRTQThknPFcAqVYcId49104282;     JekRTQThknPFcAqVYcId49104282 = JekRTQThknPFcAqVYcId77827471;     JekRTQThknPFcAqVYcId77827471 = JekRTQThknPFcAqVYcId86243894;     JekRTQThknPFcAqVYcId86243894 = JekRTQThknPFcAqVYcId3345470;     JekRTQThknPFcAqVYcId3345470 = JekRTQThknPFcAqVYcId1091297;     JekRTQThknPFcAqVYcId1091297 = JekRTQThknPFcAqVYcId60480405;     JekRTQThknPFcAqVYcId60480405 = JekRTQThknPFcAqVYcId61281996;     JekRTQThknPFcAqVYcId61281996 = JekRTQThknPFcAqVYcId83377722;     JekRTQThknPFcAqVYcId83377722 = JekRTQThknPFcAqVYcId65329389;     JekRTQThknPFcAqVYcId65329389 = JekRTQThknPFcAqVYcId1111407;     JekRTQThknPFcAqVYcId1111407 = JekRTQThknPFcAqVYcId26437752;     JekRTQThknPFcAqVYcId26437752 = JekRTQThknPFcAqVYcId10957979;     JekRTQThknPFcAqVYcId10957979 = JekRTQThknPFcAqVYcId16401745;     JekRTQThknPFcAqVYcId16401745 = JekRTQThknPFcAqVYcId56615347;     JekRTQThknPFcAqVYcId56615347 = JekRTQThknPFcAqVYcId48112833;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xxIVBBKieCimwFcCZGhD10745538() {     float yhTknfiYvxgjBwnOcvjR48106361 = 1707767;    float yhTknfiYvxgjBwnOcvjR28665118 = -288300551;    float yhTknfiYvxgjBwnOcvjR20936323 = -906802690;    float yhTknfiYvxgjBwnOcvjR67392608 = -459371627;    float yhTknfiYvxgjBwnOcvjR2791519 = -301242256;    float yhTknfiYvxgjBwnOcvjR17170351 = -120209320;    float yhTknfiYvxgjBwnOcvjR34593486 = -981708669;    float yhTknfiYvxgjBwnOcvjR70019195 = -124027319;    float yhTknfiYvxgjBwnOcvjR47978356 = -129088918;    float yhTknfiYvxgjBwnOcvjR74522569 = -287309826;    float yhTknfiYvxgjBwnOcvjR98481031 = -492448961;    float yhTknfiYvxgjBwnOcvjR75094861 = -107073998;    float yhTknfiYvxgjBwnOcvjR47350899 = -649946436;    float yhTknfiYvxgjBwnOcvjR29742451 = -706516281;    float yhTknfiYvxgjBwnOcvjR82612280 = -397307563;    float yhTknfiYvxgjBwnOcvjR42031266 = -853782184;    float yhTknfiYvxgjBwnOcvjR76134149 = -831247437;    float yhTknfiYvxgjBwnOcvjR17280951 = -326355947;    float yhTknfiYvxgjBwnOcvjR63936036 = 40938093;    float yhTknfiYvxgjBwnOcvjR53264167 = -883685381;    float yhTknfiYvxgjBwnOcvjR88935530 = -284922027;    float yhTknfiYvxgjBwnOcvjR95886304 = -216501831;    float yhTknfiYvxgjBwnOcvjR31452736 = -928873240;    float yhTknfiYvxgjBwnOcvjR8768402 = -455391058;    float yhTknfiYvxgjBwnOcvjR78016671 = -989563568;    float yhTknfiYvxgjBwnOcvjR89352363 = -996193946;    float yhTknfiYvxgjBwnOcvjR62933227 = -586034758;    float yhTknfiYvxgjBwnOcvjR55787355 = -439096704;    float yhTknfiYvxgjBwnOcvjR93703466 = -290917446;    float yhTknfiYvxgjBwnOcvjR22762150 = -309073400;    float yhTknfiYvxgjBwnOcvjR71513931 = -984377996;    float yhTknfiYvxgjBwnOcvjR75094152 = -543631897;    float yhTknfiYvxgjBwnOcvjR50838131 = -960064697;    float yhTknfiYvxgjBwnOcvjR81347418 = -171609588;    float yhTknfiYvxgjBwnOcvjR16226303 = -212056182;    float yhTknfiYvxgjBwnOcvjR39437948 = 33438820;    float yhTknfiYvxgjBwnOcvjR26926591 = -642806766;    float yhTknfiYvxgjBwnOcvjR63938441 = -842513498;    float yhTknfiYvxgjBwnOcvjR39864449 = -432928777;    float yhTknfiYvxgjBwnOcvjR26855555 = -443350507;    float yhTknfiYvxgjBwnOcvjR34629394 = -1510941;    float yhTknfiYvxgjBwnOcvjR31915539 = -133794982;    float yhTknfiYvxgjBwnOcvjR89807250 = -187585221;    float yhTknfiYvxgjBwnOcvjR15415446 = 42221347;    float yhTknfiYvxgjBwnOcvjR28423531 = -803482344;    float yhTknfiYvxgjBwnOcvjR15321073 = -328444779;    float yhTknfiYvxgjBwnOcvjR30703430 = -87708203;    float yhTknfiYvxgjBwnOcvjR21988632 = -473296171;    float yhTknfiYvxgjBwnOcvjR92322366 = -212014209;    float yhTknfiYvxgjBwnOcvjR78628105 = -347415076;    float yhTknfiYvxgjBwnOcvjR92176679 = -554696667;    float yhTknfiYvxgjBwnOcvjR79079350 = 37953851;    float yhTknfiYvxgjBwnOcvjR16714362 = -230436607;    float yhTknfiYvxgjBwnOcvjR97808507 = -979839729;    float yhTknfiYvxgjBwnOcvjR21810008 = -559674809;    float yhTknfiYvxgjBwnOcvjR52220057 = -781790403;    float yhTknfiYvxgjBwnOcvjR97212381 = -359427312;    float yhTknfiYvxgjBwnOcvjR12167922 = -351411632;    float yhTknfiYvxgjBwnOcvjR89375936 = -469808060;    float yhTknfiYvxgjBwnOcvjR13439156 = -305048310;    float yhTknfiYvxgjBwnOcvjR54237124 = -534174563;    float yhTknfiYvxgjBwnOcvjR78806131 = -442611966;    float yhTknfiYvxgjBwnOcvjR76315729 = -833109873;    float yhTknfiYvxgjBwnOcvjR25216206 = -820015518;    float yhTknfiYvxgjBwnOcvjR3008638 = -302931831;    float yhTknfiYvxgjBwnOcvjR23386880 = -948817065;    float yhTknfiYvxgjBwnOcvjR24256730 = -147009301;    float yhTknfiYvxgjBwnOcvjR66003480 = -378336848;    float yhTknfiYvxgjBwnOcvjR13516149 = -394460099;    float yhTknfiYvxgjBwnOcvjR43174333 = -330746384;    float yhTknfiYvxgjBwnOcvjR15104675 = -110975419;    float yhTknfiYvxgjBwnOcvjR12195709 = -988733940;    float yhTknfiYvxgjBwnOcvjR77416502 = -893427171;    float yhTknfiYvxgjBwnOcvjR37080481 = -515711401;    float yhTknfiYvxgjBwnOcvjR18634773 = -782174440;    float yhTknfiYvxgjBwnOcvjR57019992 = -51127046;    float yhTknfiYvxgjBwnOcvjR6079054 = 71083390;    float yhTknfiYvxgjBwnOcvjR16037291 = -871094587;    float yhTknfiYvxgjBwnOcvjR80344870 = -651908714;    float yhTknfiYvxgjBwnOcvjR62695599 = -561118790;    float yhTknfiYvxgjBwnOcvjR58648934 = -808485744;    float yhTknfiYvxgjBwnOcvjR40944595 = -12738587;    float yhTknfiYvxgjBwnOcvjR63464989 = -127082496;    float yhTknfiYvxgjBwnOcvjR15075361 = -943502370;    float yhTknfiYvxgjBwnOcvjR30585471 = -754376733;    float yhTknfiYvxgjBwnOcvjR92434581 = -922331848;    float yhTknfiYvxgjBwnOcvjR58379791 = -213195291;    float yhTknfiYvxgjBwnOcvjR53029624 = -980224969;    float yhTknfiYvxgjBwnOcvjR59537411 = -611934780;    float yhTknfiYvxgjBwnOcvjR64006245 = -430265780;    float yhTknfiYvxgjBwnOcvjR42225566 = -607133868;    float yhTknfiYvxgjBwnOcvjR14758670 = -191395134;    float yhTknfiYvxgjBwnOcvjR74562504 = -272705438;    float yhTknfiYvxgjBwnOcvjR26425294 = -27880468;    float yhTknfiYvxgjBwnOcvjR72618431 = -909175945;    float yhTknfiYvxgjBwnOcvjR55823263 = -558898976;    float yhTknfiYvxgjBwnOcvjR55599809 = -300685110;    float yhTknfiYvxgjBwnOcvjR64591044 = -367569703;    float yhTknfiYvxgjBwnOcvjR12406809 = -654846823;    float yhTknfiYvxgjBwnOcvjR5036652 = 1707767;     yhTknfiYvxgjBwnOcvjR48106361 = yhTknfiYvxgjBwnOcvjR28665118;     yhTknfiYvxgjBwnOcvjR28665118 = yhTknfiYvxgjBwnOcvjR20936323;     yhTknfiYvxgjBwnOcvjR20936323 = yhTknfiYvxgjBwnOcvjR67392608;     yhTknfiYvxgjBwnOcvjR67392608 = yhTknfiYvxgjBwnOcvjR2791519;     yhTknfiYvxgjBwnOcvjR2791519 = yhTknfiYvxgjBwnOcvjR17170351;     yhTknfiYvxgjBwnOcvjR17170351 = yhTknfiYvxgjBwnOcvjR34593486;     yhTknfiYvxgjBwnOcvjR34593486 = yhTknfiYvxgjBwnOcvjR70019195;     yhTknfiYvxgjBwnOcvjR70019195 = yhTknfiYvxgjBwnOcvjR47978356;     yhTknfiYvxgjBwnOcvjR47978356 = yhTknfiYvxgjBwnOcvjR74522569;     yhTknfiYvxgjBwnOcvjR74522569 = yhTknfiYvxgjBwnOcvjR98481031;     yhTknfiYvxgjBwnOcvjR98481031 = yhTknfiYvxgjBwnOcvjR75094861;     yhTknfiYvxgjBwnOcvjR75094861 = yhTknfiYvxgjBwnOcvjR47350899;     yhTknfiYvxgjBwnOcvjR47350899 = yhTknfiYvxgjBwnOcvjR29742451;     yhTknfiYvxgjBwnOcvjR29742451 = yhTknfiYvxgjBwnOcvjR82612280;     yhTknfiYvxgjBwnOcvjR82612280 = yhTknfiYvxgjBwnOcvjR42031266;     yhTknfiYvxgjBwnOcvjR42031266 = yhTknfiYvxgjBwnOcvjR76134149;     yhTknfiYvxgjBwnOcvjR76134149 = yhTknfiYvxgjBwnOcvjR17280951;     yhTknfiYvxgjBwnOcvjR17280951 = yhTknfiYvxgjBwnOcvjR63936036;     yhTknfiYvxgjBwnOcvjR63936036 = yhTknfiYvxgjBwnOcvjR53264167;     yhTknfiYvxgjBwnOcvjR53264167 = yhTknfiYvxgjBwnOcvjR88935530;     yhTknfiYvxgjBwnOcvjR88935530 = yhTknfiYvxgjBwnOcvjR95886304;     yhTknfiYvxgjBwnOcvjR95886304 = yhTknfiYvxgjBwnOcvjR31452736;     yhTknfiYvxgjBwnOcvjR31452736 = yhTknfiYvxgjBwnOcvjR8768402;     yhTknfiYvxgjBwnOcvjR8768402 = yhTknfiYvxgjBwnOcvjR78016671;     yhTknfiYvxgjBwnOcvjR78016671 = yhTknfiYvxgjBwnOcvjR89352363;     yhTknfiYvxgjBwnOcvjR89352363 = yhTknfiYvxgjBwnOcvjR62933227;     yhTknfiYvxgjBwnOcvjR62933227 = yhTknfiYvxgjBwnOcvjR55787355;     yhTknfiYvxgjBwnOcvjR55787355 = yhTknfiYvxgjBwnOcvjR93703466;     yhTknfiYvxgjBwnOcvjR93703466 = yhTknfiYvxgjBwnOcvjR22762150;     yhTknfiYvxgjBwnOcvjR22762150 = yhTknfiYvxgjBwnOcvjR71513931;     yhTknfiYvxgjBwnOcvjR71513931 = yhTknfiYvxgjBwnOcvjR75094152;     yhTknfiYvxgjBwnOcvjR75094152 = yhTknfiYvxgjBwnOcvjR50838131;     yhTknfiYvxgjBwnOcvjR50838131 = yhTknfiYvxgjBwnOcvjR81347418;     yhTknfiYvxgjBwnOcvjR81347418 = yhTknfiYvxgjBwnOcvjR16226303;     yhTknfiYvxgjBwnOcvjR16226303 = yhTknfiYvxgjBwnOcvjR39437948;     yhTknfiYvxgjBwnOcvjR39437948 = yhTknfiYvxgjBwnOcvjR26926591;     yhTknfiYvxgjBwnOcvjR26926591 = yhTknfiYvxgjBwnOcvjR63938441;     yhTknfiYvxgjBwnOcvjR63938441 = yhTknfiYvxgjBwnOcvjR39864449;     yhTknfiYvxgjBwnOcvjR39864449 = yhTknfiYvxgjBwnOcvjR26855555;     yhTknfiYvxgjBwnOcvjR26855555 = yhTknfiYvxgjBwnOcvjR34629394;     yhTknfiYvxgjBwnOcvjR34629394 = yhTknfiYvxgjBwnOcvjR31915539;     yhTknfiYvxgjBwnOcvjR31915539 = yhTknfiYvxgjBwnOcvjR89807250;     yhTknfiYvxgjBwnOcvjR89807250 = yhTknfiYvxgjBwnOcvjR15415446;     yhTknfiYvxgjBwnOcvjR15415446 = yhTknfiYvxgjBwnOcvjR28423531;     yhTknfiYvxgjBwnOcvjR28423531 = yhTknfiYvxgjBwnOcvjR15321073;     yhTknfiYvxgjBwnOcvjR15321073 = yhTknfiYvxgjBwnOcvjR30703430;     yhTknfiYvxgjBwnOcvjR30703430 = yhTknfiYvxgjBwnOcvjR21988632;     yhTknfiYvxgjBwnOcvjR21988632 = yhTknfiYvxgjBwnOcvjR92322366;     yhTknfiYvxgjBwnOcvjR92322366 = yhTknfiYvxgjBwnOcvjR78628105;     yhTknfiYvxgjBwnOcvjR78628105 = yhTknfiYvxgjBwnOcvjR92176679;     yhTknfiYvxgjBwnOcvjR92176679 = yhTknfiYvxgjBwnOcvjR79079350;     yhTknfiYvxgjBwnOcvjR79079350 = yhTknfiYvxgjBwnOcvjR16714362;     yhTknfiYvxgjBwnOcvjR16714362 = yhTknfiYvxgjBwnOcvjR97808507;     yhTknfiYvxgjBwnOcvjR97808507 = yhTknfiYvxgjBwnOcvjR21810008;     yhTknfiYvxgjBwnOcvjR21810008 = yhTknfiYvxgjBwnOcvjR52220057;     yhTknfiYvxgjBwnOcvjR52220057 = yhTknfiYvxgjBwnOcvjR97212381;     yhTknfiYvxgjBwnOcvjR97212381 = yhTknfiYvxgjBwnOcvjR12167922;     yhTknfiYvxgjBwnOcvjR12167922 = yhTknfiYvxgjBwnOcvjR89375936;     yhTknfiYvxgjBwnOcvjR89375936 = yhTknfiYvxgjBwnOcvjR13439156;     yhTknfiYvxgjBwnOcvjR13439156 = yhTknfiYvxgjBwnOcvjR54237124;     yhTknfiYvxgjBwnOcvjR54237124 = yhTknfiYvxgjBwnOcvjR78806131;     yhTknfiYvxgjBwnOcvjR78806131 = yhTknfiYvxgjBwnOcvjR76315729;     yhTknfiYvxgjBwnOcvjR76315729 = yhTknfiYvxgjBwnOcvjR25216206;     yhTknfiYvxgjBwnOcvjR25216206 = yhTknfiYvxgjBwnOcvjR3008638;     yhTknfiYvxgjBwnOcvjR3008638 = yhTknfiYvxgjBwnOcvjR23386880;     yhTknfiYvxgjBwnOcvjR23386880 = yhTknfiYvxgjBwnOcvjR24256730;     yhTknfiYvxgjBwnOcvjR24256730 = yhTknfiYvxgjBwnOcvjR66003480;     yhTknfiYvxgjBwnOcvjR66003480 = yhTknfiYvxgjBwnOcvjR13516149;     yhTknfiYvxgjBwnOcvjR13516149 = yhTknfiYvxgjBwnOcvjR43174333;     yhTknfiYvxgjBwnOcvjR43174333 = yhTknfiYvxgjBwnOcvjR15104675;     yhTknfiYvxgjBwnOcvjR15104675 = yhTknfiYvxgjBwnOcvjR12195709;     yhTknfiYvxgjBwnOcvjR12195709 = yhTknfiYvxgjBwnOcvjR77416502;     yhTknfiYvxgjBwnOcvjR77416502 = yhTknfiYvxgjBwnOcvjR37080481;     yhTknfiYvxgjBwnOcvjR37080481 = yhTknfiYvxgjBwnOcvjR18634773;     yhTknfiYvxgjBwnOcvjR18634773 = yhTknfiYvxgjBwnOcvjR57019992;     yhTknfiYvxgjBwnOcvjR57019992 = yhTknfiYvxgjBwnOcvjR6079054;     yhTknfiYvxgjBwnOcvjR6079054 = yhTknfiYvxgjBwnOcvjR16037291;     yhTknfiYvxgjBwnOcvjR16037291 = yhTknfiYvxgjBwnOcvjR80344870;     yhTknfiYvxgjBwnOcvjR80344870 = yhTknfiYvxgjBwnOcvjR62695599;     yhTknfiYvxgjBwnOcvjR62695599 = yhTknfiYvxgjBwnOcvjR58648934;     yhTknfiYvxgjBwnOcvjR58648934 = yhTknfiYvxgjBwnOcvjR40944595;     yhTknfiYvxgjBwnOcvjR40944595 = yhTknfiYvxgjBwnOcvjR63464989;     yhTknfiYvxgjBwnOcvjR63464989 = yhTknfiYvxgjBwnOcvjR15075361;     yhTknfiYvxgjBwnOcvjR15075361 = yhTknfiYvxgjBwnOcvjR30585471;     yhTknfiYvxgjBwnOcvjR30585471 = yhTknfiYvxgjBwnOcvjR92434581;     yhTknfiYvxgjBwnOcvjR92434581 = yhTknfiYvxgjBwnOcvjR58379791;     yhTknfiYvxgjBwnOcvjR58379791 = yhTknfiYvxgjBwnOcvjR53029624;     yhTknfiYvxgjBwnOcvjR53029624 = yhTknfiYvxgjBwnOcvjR59537411;     yhTknfiYvxgjBwnOcvjR59537411 = yhTknfiYvxgjBwnOcvjR64006245;     yhTknfiYvxgjBwnOcvjR64006245 = yhTknfiYvxgjBwnOcvjR42225566;     yhTknfiYvxgjBwnOcvjR42225566 = yhTknfiYvxgjBwnOcvjR14758670;     yhTknfiYvxgjBwnOcvjR14758670 = yhTknfiYvxgjBwnOcvjR74562504;     yhTknfiYvxgjBwnOcvjR74562504 = yhTknfiYvxgjBwnOcvjR26425294;     yhTknfiYvxgjBwnOcvjR26425294 = yhTknfiYvxgjBwnOcvjR72618431;     yhTknfiYvxgjBwnOcvjR72618431 = yhTknfiYvxgjBwnOcvjR55823263;     yhTknfiYvxgjBwnOcvjR55823263 = yhTknfiYvxgjBwnOcvjR55599809;     yhTknfiYvxgjBwnOcvjR55599809 = yhTknfiYvxgjBwnOcvjR64591044;     yhTknfiYvxgjBwnOcvjR64591044 = yhTknfiYvxgjBwnOcvjR12406809;     yhTknfiYvxgjBwnOcvjR12406809 = yhTknfiYvxgjBwnOcvjR5036652;     yhTknfiYvxgjBwnOcvjR5036652 = yhTknfiYvxgjBwnOcvjR48106361;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mFfefkdhzFqOvzaDBGkb65161751() {     float nWwnIvJlBZFBhfWbvDRW18758029 = -616441364;    float nWwnIvJlBZFBhfWbvDRW97338635 = -159943851;    float nWwnIvJlBZFBhfWbvDRW905243 = -249097150;    float nWwnIvJlBZFBhfWbvDRW7323895 = -649510016;    float nWwnIvJlBZFBhfWbvDRW24241841 = -84648604;    float nWwnIvJlBZFBhfWbvDRW7170113 = -921790471;    float nWwnIvJlBZFBhfWbvDRW52972382 = -980484379;    float nWwnIvJlBZFBhfWbvDRW26861576 = -470844409;    float nWwnIvJlBZFBhfWbvDRW93843410 = -222436247;    float nWwnIvJlBZFBhfWbvDRW97489029 = -588405177;    float nWwnIvJlBZFBhfWbvDRW40730193 = -527350891;    float nWwnIvJlBZFBhfWbvDRW20641433 = -422837791;    float nWwnIvJlBZFBhfWbvDRW27178286 = -559483494;    float nWwnIvJlBZFBhfWbvDRW13589485 = -507544491;    float nWwnIvJlBZFBhfWbvDRW65332299 = -547057567;    float nWwnIvJlBZFBhfWbvDRW74559905 = -423765771;    float nWwnIvJlBZFBhfWbvDRW69986976 = -740432567;    float nWwnIvJlBZFBhfWbvDRW37574445 = -315522143;    float nWwnIvJlBZFBhfWbvDRW45643818 = -666249849;    float nWwnIvJlBZFBhfWbvDRW12050876 = -326051685;    float nWwnIvJlBZFBhfWbvDRW44071285 = -193601168;    float nWwnIvJlBZFBhfWbvDRW57282069 = -547787137;    float nWwnIvJlBZFBhfWbvDRW33396545 = -396716385;    float nWwnIvJlBZFBhfWbvDRW42617938 = -47791964;    float nWwnIvJlBZFBhfWbvDRW42141060 = -665468255;    float nWwnIvJlBZFBhfWbvDRW19949945 = 5521609;    float nWwnIvJlBZFBhfWbvDRW71194012 = -981212533;    float nWwnIvJlBZFBhfWbvDRW63569580 = -944220471;    float nWwnIvJlBZFBhfWbvDRW25845954 = -585275663;    float nWwnIvJlBZFBhfWbvDRW61934245 = -867468471;    float nWwnIvJlBZFBhfWbvDRW78585928 = -732811682;    float nWwnIvJlBZFBhfWbvDRW19605613 = -871929060;    float nWwnIvJlBZFBhfWbvDRW33783492 = -907026306;    float nWwnIvJlBZFBhfWbvDRW46712394 = -835139273;    float nWwnIvJlBZFBhfWbvDRW94250538 = -59002961;    float nWwnIvJlBZFBhfWbvDRW99431206 = -45787727;    float nWwnIvJlBZFBhfWbvDRW70718077 = -831399974;    float nWwnIvJlBZFBhfWbvDRW16730930 = -626626605;    float nWwnIvJlBZFBhfWbvDRW18397738 = -388657414;    float nWwnIvJlBZFBhfWbvDRW21958853 = 39041131;    float nWwnIvJlBZFBhfWbvDRW94702678 = -385529820;    float nWwnIvJlBZFBhfWbvDRW60398530 = 96815948;    float nWwnIvJlBZFBhfWbvDRW38774210 = -174339273;    float nWwnIvJlBZFBhfWbvDRW21923746 = -29753625;    float nWwnIvJlBZFBhfWbvDRW24377590 = -12128739;    float nWwnIvJlBZFBhfWbvDRW21726908 = -86781934;    float nWwnIvJlBZFBhfWbvDRW56834633 = -203778002;    float nWwnIvJlBZFBhfWbvDRW87602841 = -925426466;    float nWwnIvJlBZFBhfWbvDRW97763818 = -197778178;    float nWwnIvJlBZFBhfWbvDRW87055592 = -630423955;    float nWwnIvJlBZFBhfWbvDRW60366004 = -101268788;    float nWwnIvJlBZFBhfWbvDRW20950855 = 68734129;    float nWwnIvJlBZFBhfWbvDRW71790018 = -158234649;    float nWwnIvJlBZFBhfWbvDRW88035811 = -540721225;    float nWwnIvJlBZFBhfWbvDRW21090467 = -670615724;    float nWwnIvJlBZFBhfWbvDRW61475959 = 31345773;    float nWwnIvJlBZFBhfWbvDRW63942090 = -763227466;    float nWwnIvJlBZFBhfWbvDRW58287304 = -101305186;    float nWwnIvJlBZFBhfWbvDRW65182835 = -984041761;    float nWwnIvJlBZFBhfWbvDRW4291897 = 9829787;    float nWwnIvJlBZFBhfWbvDRW35976101 = -940577939;    float nWwnIvJlBZFBhfWbvDRW89402801 = 63736092;    float nWwnIvJlBZFBhfWbvDRW1015622 = -885568746;    float nWwnIvJlBZFBhfWbvDRW31909165 = -354967776;    float nWwnIvJlBZFBhfWbvDRW18903102 = -855593496;    float nWwnIvJlBZFBhfWbvDRW21124580 = -655421832;    float nWwnIvJlBZFBhfWbvDRW86857940 = -515811485;    float nWwnIvJlBZFBhfWbvDRW80465892 = -724344222;    float nWwnIvJlBZFBhfWbvDRW19338946 = -348541530;    float nWwnIvJlBZFBhfWbvDRW65901093 = -401269840;    float nWwnIvJlBZFBhfWbvDRW3841829 = -592365798;    float nWwnIvJlBZFBhfWbvDRW53256046 = -13805963;    float nWwnIvJlBZFBhfWbvDRW19176708 = -926864730;    float nWwnIvJlBZFBhfWbvDRW23684965 = -605290981;    float nWwnIvJlBZFBhfWbvDRW17348198 = -940521866;    float nWwnIvJlBZFBhfWbvDRW83672755 = -190417117;    float nWwnIvJlBZFBhfWbvDRW18507859 = -273447864;    float nWwnIvJlBZFBhfWbvDRW11472799 = -266962761;    float nWwnIvJlBZFBhfWbvDRW18240349 = 64336774;    float nWwnIvJlBZFBhfWbvDRW20414153 = -478686322;    float nWwnIvJlBZFBhfWbvDRW63115311 = -790700390;    float nWwnIvJlBZFBhfWbvDRW83591170 = 44213933;    float nWwnIvJlBZFBhfWbvDRW65805762 = -646442294;    float nWwnIvJlBZFBhfWbvDRW38790362 = -954851708;    float nWwnIvJlBZFBhfWbvDRW1568242 = -666199683;    float nWwnIvJlBZFBhfWbvDRW57635073 = -701545811;    float nWwnIvJlBZFBhfWbvDRW47815595 = -613694411;    float nWwnIvJlBZFBhfWbvDRW45747680 = -266305081;    float nWwnIvJlBZFBhfWbvDRW25621927 = -64523550;    float nWwnIvJlBZFBhfWbvDRW32774579 = 9651266;    float nWwnIvJlBZFBhfWbvDRW35489116 = -282560262;    float nWwnIvJlBZFBhfWbvDRW12430774 = -630094788;    float nWwnIvJlBZFBhfWbvDRW51548095 = -642584844;    float nWwnIvJlBZFBhfWbvDRW14105842 = -298487202;    float nWwnIvJlBZFBhfWbvDRW85982752 = -20380931;    float nWwnIvJlBZFBhfWbvDRW5299877 = -349265912;    float nWwnIvJlBZFBhfWbvDRW59382908 = -17615306;    float nWwnIvJlBZFBhfWbvDRW6865046 = -819371497;    float nWwnIvJlBZFBhfWbvDRW3020645 = -174160130;    float nWwnIvJlBZFBhfWbvDRW3253010 = -616441364;     nWwnIvJlBZFBhfWbvDRW18758029 = nWwnIvJlBZFBhfWbvDRW97338635;     nWwnIvJlBZFBhfWbvDRW97338635 = nWwnIvJlBZFBhfWbvDRW905243;     nWwnIvJlBZFBhfWbvDRW905243 = nWwnIvJlBZFBhfWbvDRW7323895;     nWwnIvJlBZFBhfWbvDRW7323895 = nWwnIvJlBZFBhfWbvDRW24241841;     nWwnIvJlBZFBhfWbvDRW24241841 = nWwnIvJlBZFBhfWbvDRW7170113;     nWwnIvJlBZFBhfWbvDRW7170113 = nWwnIvJlBZFBhfWbvDRW52972382;     nWwnIvJlBZFBhfWbvDRW52972382 = nWwnIvJlBZFBhfWbvDRW26861576;     nWwnIvJlBZFBhfWbvDRW26861576 = nWwnIvJlBZFBhfWbvDRW93843410;     nWwnIvJlBZFBhfWbvDRW93843410 = nWwnIvJlBZFBhfWbvDRW97489029;     nWwnIvJlBZFBhfWbvDRW97489029 = nWwnIvJlBZFBhfWbvDRW40730193;     nWwnIvJlBZFBhfWbvDRW40730193 = nWwnIvJlBZFBhfWbvDRW20641433;     nWwnIvJlBZFBhfWbvDRW20641433 = nWwnIvJlBZFBhfWbvDRW27178286;     nWwnIvJlBZFBhfWbvDRW27178286 = nWwnIvJlBZFBhfWbvDRW13589485;     nWwnIvJlBZFBhfWbvDRW13589485 = nWwnIvJlBZFBhfWbvDRW65332299;     nWwnIvJlBZFBhfWbvDRW65332299 = nWwnIvJlBZFBhfWbvDRW74559905;     nWwnIvJlBZFBhfWbvDRW74559905 = nWwnIvJlBZFBhfWbvDRW69986976;     nWwnIvJlBZFBhfWbvDRW69986976 = nWwnIvJlBZFBhfWbvDRW37574445;     nWwnIvJlBZFBhfWbvDRW37574445 = nWwnIvJlBZFBhfWbvDRW45643818;     nWwnIvJlBZFBhfWbvDRW45643818 = nWwnIvJlBZFBhfWbvDRW12050876;     nWwnIvJlBZFBhfWbvDRW12050876 = nWwnIvJlBZFBhfWbvDRW44071285;     nWwnIvJlBZFBhfWbvDRW44071285 = nWwnIvJlBZFBhfWbvDRW57282069;     nWwnIvJlBZFBhfWbvDRW57282069 = nWwnIvJlBZFBhfWbvDRW33396545;     nWwnIvJlBZFBhfWbvDRW33396545 = nWwnIvJlBZFBhfWbvDRW42617938;     nWwnIvJlBZFBhfWbvDRW42617938 = nWwnIvJlBZFBhfWbvDRW42141060;     nWwnIvJlBZFBhfWbvDRW42141060 = nWwnIvJlBZFBhfWbvDRW19949945;     nWwnIvJlBZFBhfWbvDRW19949945 = nWwnIvJlBZFBhfWbvDRW71194012;     nWwnIvJlBZFBhfWbvDRW71194012 = nWwnIvJlBZFBhfWbvDRW63569580;     nWwnIvJlBZFBhfWbvDRW63569580 = nWwnIvJlBZFBhfWbvDRW25845954;     nWwnIvJlBZFBhfWbvDRW25845954 = nWwnIvJlBZFBhfWbvDRW61934245;     nWwnIvJlBZFBhfWbvDRW61934245 = nWwnIvJlBZFBhfWbvDRW78585928;     nWwnIvJlBZFBhfWbvDRW78585928 = nWwnIvJlBZFBhfWbvDRW19605613;     nWwnIvJlBZFBhfWbvDRW19605613 = nWwnIvJlBZFBhfWbvDRW33783492;     nWwnIvJlBZFBhfWbvDRW33783492 = nWwnIvJlBZFBhfWbvDRW46712394;     nWwnIvJlBZFBhfWbvDRW46712394 = nWwnIvJlBZFBhfWbvDRW94250538;     nWwnIvJlBZFBhfWbvDRW94250538 = nWwnIvJlBZFBhfWbvDRW99431206;     nWwnIvJlBZFBhfWbvDRW99431206 = nWwnIvJlBZFBhfWbvDRW70718077;     nWwnIvJlBZFBhfWbvDRW70718077 = nWwnIvJlBZFBhfWbvDRW16730930;     nWwnIvJlBZFBhfWbvDRW16730930 = nWwnIvJlBZFBhfWbvDRW18397738;     nWwnIvJlBZFBhfWbvDRW18397738 = nWwnIvJlBZFBhfWbvDRW21958853;     nWwnIvJlBZFBhfWbvDRW21958853 = nWwnIvJlBZFBhfWbvDRW94702678;     nWwnIvJlBZFBhfWbvDRW94702678 = nWwnIvJlBZFBhfWbvDRW60398530;     nWwnIvJlBZFBhfWbvDRW60398530 = nWwnIvJlBZFBhfWbvDRW38774210;     nWwnIvJlBZFBhfWbvDRW38774210 = nWwnIvJlBZFBhfWbvDRW21923746;     nWwnIvJlBZFBhfWbvDRW21923746 = nWwnIvJlBZFBhfWbvDRW24377590;     nWwnIvJlBZFBhfWbvDRW24377590 = nWwnIvJlBZFBhfWbvDRW21726908;     nWwnIvJlBZFBhfWbvDRW21726908 = nWwnIvJlBZFBhfWbvDRW56834633;     nWwnIvJlBZFBhfWbvDRW56834633 = nWwnIvJlBZFBhfWbvDRW87602841;     nWwnIvJlBZFBhfWbvDRW87602841 = nWwnIvJlBZFBhfWbvDRW97763818;     nWwnIvJlBZFBhfWbvDRW97763818 = nWwnIvJlBZFBhfWbvDRW87055592;     nWwnIvJlBZFBhfWbvDRW87055592 = nWwnIvJlBZFBhfWbvDRW60366004;     nWwnIvJlBZFBhfWbvDRW60366004 = nWwnIvJlBZFBhfWbvDRW20950855;     nWwnIvJlBZFBhfWbvDRW20950855 = nWwnIvJlBZFBhfWbvDRW71790018;     nWwnIvJlBZFBhfWbvDRW71790018 = nWwnIvJlBZFBhfWbvDRW88035811;     nWwnIvJlBZFBhfWbvDRW88035811 = nWwnIvJlBZFBhfWbvDRW21090467;     nWwnIvJlBZFBhfWbvDRW21090467 = nWwnIvJlBZFBhfWbvDRW61475959;     nWwnIvJlBZFBhfWbvDRW61475959 = nWwnIvJlBZFBhfWbvDRW63942090;     nWwnIvJlBZFBhfWbvDRW63942090 = nWwnIvJlBZFBhfWbvDRW58287304;     nWwnIvJlBZFBhfWbvDRW58287304 = nWwnIvJlBZFBhfWbvDRW65182835;     nWwnIvJlBZFBhfWbvDRW65182835 = nWwnIvJlBZFBhfWbvDRW4291897;     nWwnIvJlBZFBhfWbvDRW4291897 = nWwnIvJlBZFBhfWbvDRW35976101;     nWwnIvJlBZFBhfWbvDRW35976101 = nWwnIvJlBZFBhfWbvDRW89402801;     nWwnIvJlBZFBhfWbvDRW89402801 = nWwnIvJlBZFBhfWbvDRW1015622;     nWwnIvJlBZFBhfWbvDRW1015622 = nWwnIvJlBZFBhfWbvDRW31909165;     nWwnIvJlBZFBhfWbvDRW31909165 = nWwnIvJlBZFBhfWbvDRW18903102;     nWwnIvJlBZFBhfWbvDRW18903102 = nWwnIvJlBZFBhfWbvDRW21124580;     nWwnIvJlBZFBhfWbvDRW21124580 = nWwnIvJlBZFBhfWbvDRW86857940;     nWwnIvJlBZFBhfWbvDRW86857940 = nWwnIvJlBZFBhfWbvDRW80465892;     nWwnIvJlBZFBhfWbvDRW80465892 = nWwnIvJlBZFBhfWbvDRW19338946;     nWwnIvJlBZFBhfWbvDRW19338946 = nWwnIvJlBZFBhfWbvDRW65901093;     nWwnIvJlBZFBhfWbvDRW65901093 = nWwnIvJlBZFBhfWbvDRW3841829;     nWwnIvJlBZFBhfWbvDRW3841829 = nWwnIvJlBZFBhfWbvDRW53256046;     nWwnIvJlBZFBhfWbvDRW53256046 = nWwnIvJlBZFBhfWbvDRW19176708;     nWwnIvJlBZFBhfWbvDRW19176708 = nWwnIvJlBZFBhfWbvDRW23684965;     nWwnIvJlBZFBhfWbvDRW23684965 = nWwnIvJlBZFBhfWbvDRW17348198;     nWwnIvJlBZFBhfWbvDRW17348198 = nWwnIvJlBZFBhfWbvDRW83672755;     nWwnIvJlBZFBhfWbvDRW83672755 = nWwnIvJlBZFBhfWbvDRW18507859;     nWwnIvJlBZFBhfWbvDRW18507859 = nWwnIvJlBZFBhfWbvDRW11472799;     nWwnIvJlBZFBhfWbvDRW11472799 = nWwnIvJlBZFBhfWbvDRW18240349;     nWwnIvJlBZFBhfWbvDRW18240349 = nWwnIvJlBZFBhfWbvDRW20414153;     nWwnIvJlBZFBhfWbvDRW20414153 = nWwnIvJlBZFBhfWbvDRW63115311;     nWwnIvJlBZFBhfWbvDRW63115311 = nWwnIvJlBZFBhfWbvDRW83591170;     nWwnIvJlBZFBhfWbvDRW83591170 = nWwnIvJlBZFBhfWbvDRW65805762;     nWwnIvJlBZFBhfWbvDRW65805762 = nWwnIvJlBZFBhfWbvDRW38790362;     nWwnIvJlBZFBhfWbvDRW38790362 = nWwnIvJlBZFBhfWbvDRW1568242;     nWwnIvJlBZFBhfWbvDRW1568242 = nWwnIvJlBZFBhfWbvDRW57635073;     nWwnIvJlBZFBhfWbvDRW57635073 = nWwnIvJlBZFBhfWbvDRW47815595;     nWwnIvJlBZFBhfWbvDRW47815595 = nWwnIvJlBZFBhfWbvDRW45747680;     nWwnIvJlBZFBhfWbvDRW45747680 = nWwnIvJlBZFBhfWbvDRW25621927;     nWwnIvJlBZFBhfWbvDRW25621927 = nWwnIvJlBZFBhfWbvDRW32774579;     nWwnIvJlBZFBhfWbvDRW32774579 = nWwnIvJlBZFBhfWbvDRW35489116;     nWwnIvJlBZFBhfWbvDRW35489116 = nWwnIvJlBZFBhfWbvDRW12430774;     nWwnIvJlBZFBhfWbvDRW12430774 = nWwnIvJlBZFBhfWbvDRW51548095;     nWwnIvJlBZFBhfWbvDRW51548095 = nWwnIvJlBZFBhfWbvDRW14105842;     nWwnIvJlBZFBhfWbvDRW14105842 = nWwnIvJlBZFBhfWbvDRW85982752;     nWwnIvJlBZFBhfWbvDRW85982752 = nWwnIvJlBZFBhfWbvDRW5299877;     nWwnIvJlBZFBhfWbvDRW5299877 = nWwnIvJlBZFBhfWbvDRW59382908;     nWwnIvJlBZFBhfWbvDRW59382908 = nWwnIvJlBZFBhfWbvDRW6865046;     nWwnIvJlBZFBhfWbvDRW6865046 = nWwnIvJlBZFBhfWbvDRW3020645;     nWwnIvJlBZFBhfWbvDRW3020645 = nWwnIvJlBZFBhfWbvDRW3253010;     nWwnIvJlBZFBhfWbvDRW3253010 = nWwnIvJlBZFBhfWbvDRW18758029;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void jOrvZsQQyOvUUfTVTmTL71820495() {     float bwpzHubHKzVcEfUKfKsQ18751557 = -488965505;    float bwpzHubHKzVcEfUKfKsQ35910952 = -202538976;    float bwpzHubHKzVcEfUKfKsQ2218455 = -206225763;    float bwpzHubHKzVcEfUKfKsQ92403755 = -470050846;    float bwpzHubHKzVcEfUKfKsQ33081943 = -553003925;    float bwpzHubHKzVcEfUKfKsQ41975545 = -621223283;    float bwpzHubHKzVcEfUKfKsQ21519718 = -685020846;    float bwpzHubHKzVcEfUKfKsQ96075626 = -175021726;    float bwpzHubHKzVcEfUKfKsQ76583574 = 63917684;    float bwpzHubHKzVcEfUKfKsQ94128543 = -225212048;    float bwpzHubHKzVcEfUKfKsQ61050798 = -486822963;    float bwpzHubHKzVcEfUKfKsQ34933110 = -158316153;    float bwpzHubHKzVcEfUKfKsQ59031200 = -581141969;    float bwpzHubHKzVcEfUKfKsQ67433705 = -852697542;    float bwpzHubHKzVcEfUKfKsQ10504409 = -509770419;    float bwpzHubHKzVcEfUKfKsQ49124453 = -756561422;    float bwpzHubHKzVcEfUKfKsQ25839736 = -560047137;    float bwpzHubHKzVcEfUKfKsQ60574978 = -712593765;    float bwpzHubHKzVcEfUKfKsQ66072087 = -184232116;    float bwpzHubHKzVcEfUKfKsQ84124638 = -938076137;    float bwpzHubHKzVcEfUKfKsQ23740709 = 95932458;    float bwpzHubHKzVcEfUKfKsQ35578335 = 42220980;    float bwpzHubHKzVcEfUKfKsQ70274941 = -855674227;    float bwpzHubHKzVcEfUKfKsQ86268046 = 71287513;    float bwpzHubHKzVcEfUKfKsQ70583749 = 7837891;    float bwpzHubHKzVcEfUKfKsQ36524021 = 97315680;    float bwpzHubHKzVcEfUKfKsQ41415594 = -182012246;    float bwpzHubHKzVcEfUKfKsQ98973105 = -453187460;    float bwpzHubHKzVcEfUKfKsQ38524401 = -131345978;    float bwpzHubHKzVcEfUKfKsQ38024116 = -542899844;    float bwpzHubHKzVcEfUKfKsQ11998607 = -19904916;    float bwpzHubHKzVcEfUKfKsQ4329471 = -163750821;    float bwpzHubHKzVcEfUKfKsQ19235915 = -8970522;    float bwpzHubHKzVcEfUKfKsQ46995231 = -711821005;    float bwpzHubHKzVcEfUKfKsQ76608576 = -318316886;    float bwpzHubHKzVcEfUKfKsQ42259396 = -125553474;    float bwpzHubHKzVcEfUKfKsQ82659445 = -931053352;    float bwpzHubHKzVcEfUKfKsQ86648609 = -621889284;    float bwpzHubHKzVcEfUKfKsQ53711334 = -1338830;    float bwpzHubHKzVcEfUKfKsQ93831745 = -670390556;    float bwpzHubHKzVcEfUKfKsQ82558345 = -473473718;    float bwpzHubHKzVcEfUKfKsQ46096191 = -96976134;    float bwpzHubHKzVcEfUKfKsQ99057570 = -733469528;    float bwpzHubHKzVcEfUKfKsQ81155644 = -686718068;    float bwpzHubHKzVcEfUKfKsQ8395642 = -485388141;    float bwpzHubHKzVcEfUKfKsQ62154876 = -998826604;    float bwpzHubHKzVcEfUKfKsQ84532425 = -685058624;    float bwpzHubHKzVcEfUKfKsQ62986422 = -434187823;    float bwpzHubHKzVcEfUKfKsQ16422208 = -861542039;    float bwpzHubHKzVcEfUKfKsQ7101015 = -217448841;    float bwpzHubHKzVcEfUKfKsQ73625373 = -779823795;    float bwpzHubHKzVcEfUKfKsQ35498113 = -313411619;    float bwpzHubHKzVcEfUKfKsQ47238367 = -50288657;    float bwpzHubHKzVcEfUKfKsQ98286082 = 27358923;    float bwpzHubHKzVcEfUKfKsQ48079787 = -427515001;    float bwpzHubHKzVcEfUKfKsQ83173222 = -431186486;    float bwpzHubHKzVcEfUKfKsQ65636010 = -346864750;    float bwpzHubHKzVcEfUKfKsQ15950408 = -177513277;    float bwpzHubHKzVcEfUKfKsQ21820006 = -377888738;    float bwpzHubHKzVcEfUKfKsQ96557921 = -550319606;    float bwpzHubHKzVcEfUKfKsQ559952 = -339211038;    float bwpzHubHKzVcEfUKfKsQ22546612 = -131833387;    float bwpzHubHKzVcEfUKfKsQ57551225 = 56324252;    float bwpzHubHKzVcEfUKfKsQ38559459 = -393182473;    float bwpzHubHKzVcEfUKfKsQ82129936 = -105307132;    float bwpzHubHKzVcEfUKfKsQ56721327 = -223072142;    float bwpzHubHKzVcEfUKfKsQ15697195 = -49345632;    float bwpzHubHKzVcEfUKfKsQ12035969 = -869320964;    float bwpzHubHKzVcEfUKfKsQ90825128 = -434380657;    float bwpzHubHKzVcEfUKfKsQ68245012 = -284216945;    float bwpzHubHKzVcEfUKfKsQ66465008 = -825508071;    float bwpzHubHKzVcEfUKfKsQ39191127 = -938157853;    float bwpzHubHKzVcEfUKfKsQ6863644 = -611254936;    float bwpzHubHKzVcEfUKfKsQ72240342 = -513841561;    float bwpzHubHKzVcEfUKfKsQ1566294 = -364602419;    float bwpzHubHKzVcEfUKfKsQ77644518 = -807091408;    float bwpzHubHKzVcEfUKfKsQ36520764 = -224309493;    float bwpzHubHKzVcEfUKfKsQ89119297 = -68956159;    float bwpzHubHKzVcEfUKfKsQ77872405 = -443324347;    float bwpzHubHKzVcEfUKfKsQ8428874 = 6664495;    float bwpzHubHKzVcEfUKfKsQ51991596 = -217625696;    float bwpzHubHKzVcEfUKfKsQ78429171 = -747824424;    float bwpzHubHKzVcEfUKfKsQ82550897 = -591645421;    float bwpzHubHKzVcEfUKfKsQ31423386 = -913897137;    float bwpzHubHKzVcEfUKfKsQ64398742 = -763076049;    float bwpzHubHKzVcEfUKfKsQ76500494 = -706493298;    float bwpzHubHKzVcEfUKfKsQ57091104 = -13462165;    float bwpzHubHKzVcEfUKfKsQ20949833 = 63670554;    float bwpzHubHKzVcEfUKfKsQ98915444 = -184306004;    float bwpzHubHKzVcEfUKfKsQ93435354 = -887130401;    float bwpzHubHKzVcEfUKfKsQ76623385 = -778688725;    float bwpzHubHKzVcEfUKfKsQ66709037 = -653540075;    float bwpzHubHKzVcEfUKfKsQ64828603 = -144000547;    float bwpzHubHKzVcEfUKfKsQ57153413 = -451019225;    float bwpzHubHKzVcEfUKfKsQ93271794 = -231179518;    float bwpzHubHKzVcEfUKfKsQ60011733 = -241640332;    float bwpzHubHKzVcEfUKfKsQ88544965 = -53300386;    float bwpzHubHKzVcEfUKfKsQ60498112 = -240287055;    float bwpzHubHKzVcEfUKfKsQ99025707 = -481410936;    float bwpzHubHKzVcEfUKfKsQ51674314 = -488965505;     bwpzHubHKzVcEfUKfKsQ18751557 = bwpzHubHKzVcEfUKfKsQ35910952;     bwpzHubHKzVcEfUKfKsQ35910952 = bwpzHubHKzVcEfUKfKsQ2218455;     bwpzHubHKzVcEfUKfKsQ2218455 = bwpzHubHKzVcEfUKfKsQ92403755;     bwpzHubHKzVcEfUKfKsQ92403755 = bwpzHubHKzVcEfUKfKsQ33081943;     bwpzHubHKzVcEfUKfKsQ33081943 = bwpzHubHKzVcEfUKfKsQ41975545;     bwpzHubHKzVcEfUKfKsQ41975545 = bwpzHubHKzVcEfUKfKsQ21519718;     bwpzHubHKzVcEfUKfKsQ21519718 = bwpzHubHKzVcEfUKfKsQ96075626;     bwpzHubHKzVcEfUKfKsQ96075626 = bwpzHubHKzVcEfUKfKsQ76583574;     bwpzHubHKzVcEfUKfKsQ76583574 = bwpzHubHKzVcEfUKfKsQ94128543;     bwpzHubHKzVcEfUKfKsQ94128543 = bwpzHubHKzVcEfUKfKsQ61050798;     bwpzHubHKzVcEfUKfKsQ61050798 = bwpzHubHKzVcEfUKfKsQ34933110;     bwpzHubHKzVcEfUKfKsQ34933110 = bwpzHubHKzVcEfUKfKsQ59031200;     bwpzHubHKzVcEfUKfKsQ59031200 = bwpzHubHKzVcEfUKfKsQ67433705;     bwpzHubHKzVcEfUKfKsQ67433705 = bwpzHubHKzVcEfUKfKsQ10504409;     bwpzHubHKzVcEfUKfKsQ10504409 = bwpzHubHKzVcEfUKfKsQ49124453;     bwpzHubHKzVcEfUKfKsQ49124453 = bwpzHubHKzVcEfUKfKsQ25839736;     bwpzHubHKzVcEfUKfKsQ25839736 = bwpzHubHKzVcEfUKfKsQ60574978;     bwpzHubHKzVcEfUKfKsQ60574978 = bwpzHubHKzVcEfUKfKsQ66072087;     bwpzHubHKzVcEfUKfKsQ66072087 = bwpzHubHKzVcEfUKfKsQ84124638;     bwpzHubHKzVcEfUKfKsQ84124638 = bwpzHubHKzVcEfUKfKsQ23740709;     bwpzHubHKzVcEfUKfKsQ23740709 = bwpzHubHKzVcEfUKfKsQ35578335;     bwpzHubHKzVcEfUKfKsQ35578335 = bwpzHubHKzVcEfUKfKsQ70274941;     bwpzHubHKzVcEfUKfKsQ70274941 = bwpzHubHKzVcEfUKfKsQ86268046;     bwpzHubHKzVcEfUKfKsQ86268046 = bwpzHubHKzVcEfUKfKsQ70583749;     bwpzHubHKzVcEfUKfKsQ70583749 = bwpzHubHKzVcEfUKfKsQ36524021;     bwpzHubHKzVcEfUKfKsQ36524021 = bwpzHubHKzVcEfUKfKsQ41415594;     bwpzHubHKzVcEfUKfKsQ41415594 = bwpzHubHKzVcEfUKfKsQ98973105;     bwpzHubHKzVcEfUKfKsQ98973105 = bwpzHubHKzVcEfUKfKsQ38524401;     bwpzHubHKzVcEfUKfKsQ38524401 = bwpzHubHKzVcEfUKfKsQ38024116;     bwpzHubHKzVcEfUKfKsQ38024116 = bwpzHubHKzVcEfUKfKsQ11998607;     bwpzHubHKzVcEfUKfKsQ11998607 = bwpzHubHKzVcEfUKfKsQ4329471;     bwpzHubHKzVcEfUKfKsQ4329471 = bwpzHubHKzVcEfUKfKsQ19235915;     bwpzHubHKzVcEfUKfKsQ19235915 = bwpzHubHKzVcEfUKfKsQ46995231;     bwpzHubHKzVcEfUKfKsQ46995231 = bwpzHubHKzVcEfUKfKsQ76608576;     bwpzHubHKzVcEfUKfKsQ76608576 = bwpzHubHKzVcEfUKfKsQ42259396;     bwpzHubHKzVcEfUKfKsQ42259396 = bwpzHubHKzVcEfUKfKsQ82659445;     bwpzHubHKzVcEfUKfKsQ82659445 = bwpzHubHKzVcEfUKfKsQ86648609;     bwpzHubHKzVcEfUKfKsQ86648609 = bwpzHubHKzVcEfUKfKsQ53711334;     bwpzHubHKzVcEfUKfKsQ53711334 = bwpzHubHKzVcEfUKfKsQ93831745;     bwpzHubHKzVcEfUKfKsQ93831745 = bwpzHubHKzVcEfUKfKsQ82558345;     bwpzHubHKzVcEfUKfKsQ82558345 = bwpzHubHKzVcEfUKfKsQ46096191;     bwpzHubHKzVcEfUKfKsQ46096191 = bwpzHubHKzVcEfUKfKsQ99057570;     bwpzHubHKzVcEfUKfKsQ99057570 = bwpzHubHKzVcEfUKfKsQ81155644;     bwpzHubHKzVcEfUKfKsQ81155644 = bwpzHubHKzVcEfUKfKsQ8395642;     bwpzHubHKzVcEfUKfKsQ8395642 = bwpzHubHKzVcEfUKfKsQ62154876;     bwpzHubHKzVcEfUKfKsQ62154876 = bwpzHubHKzVcEfUKfKsQ84532425;     bwpzHubHKzVcEfUKfKsQ84532425 = bwpzHubHKzVcEfUKfKsQ62986422;     bwpzHubHKzVcEfUKfKsQ62986422 = bwpzHubHKzVcEfUKfKsQ16422208;     bwpzHubHKzVcEfUKfKsQ16422208 = bwpzHubHKzVcEfUKfKsQ7101015;     bwpzHubHKzVcEfUKfKsQ7101015 = bwpzHubHKzVcEfUKfKsQ73625373;     bwpzHubHKzVcEfUKfKsQ73625373 = bwpzHubHKzVcEfUKfKsQ35498113;     bwpzHubHKzVcEfUKfKsQ35498113 = bwpzHubHKzVcEfUKfKsQ47238367;     bwpzHubHKzVcEfUKfKsQ47238367 = bwpzHubHKzVcEfUKfKsQ98286082;     bwpzHubHKzVcEfUKfKsQ98286082 = bwpzHubHKzVcEfUKfKsQ48079787;     bwpzHubHKzVcEfUKfKsQ48079787 = bwpzHubHKzVcEfUKfKsQ83173222;     bwpzHubHKzVcEfUKfKsQ83173222 = bwpzHubHKzVcEfUKfKsQ65636010;     bwpzHubHKzVcEfUKfKsQ65636010 = bwpzHubHKzVcEfUKfKsQ15950408;     bwpzHubHKzVcEfUKfKsQ15950408 = bwpzHubHKzVcEfUKfKsQ21820006;     bwpzHubHKzVcEfUKfKsQ21820006 = bwpzHubHKzVcEfUKfKsQ96557921;     bwpzHubHKzVcEfUKfKsQ96557921 = bwpzHubHKzVcEfUKfKsQ559952;     bwpzHubHKzVcEfUKfKsQ559952 = bwpzHubHKzVcEfUKfKsQ22546612;     bwpzHubHKzVcEfUKfKsQ22546612 = bwpzHubHKzVcEfUKfKsQ57551225;     bwpzHubHKzVcEfUKfKsQ57551225 = bwpzHubHKzVcEfUKfKsQ38559459;     bwpzHubHKzVcEfUKfKsQ38559459 = bwpzHubHKzVcEfUKfKsQ82129936;     bwpzHubHKzVcEfUKfKsQ82129936 = bwpzHubHKzVcEfUKfKsQ56721327;     bwpzHubHKzVcEfUKfKsQ56721327 = bwpzHubHKzVcEfUKfKsQ15697195;     bwpzHubHKzVcEfUKfKsQ15697195 = bwpzHubHKzVcEfUKfKsQ12035969;     bwpzHubHKzVcEfUKfKsQ12035969 = bwpzHubHKzVcEfUKfKsQ90825128;     bwpzHubHKzVcEfUKfKsQ90825128 = bwpzHubHKzVcEfUKfKsQ68245012;     bwpzHubHKzVcEfUKfKsQ68245012 = bwpzHubHKzVcEfUKfKsQ66465008;     bwpzHubHKzVcEfUKfKsQ66465008 = bwpzHubHKzVcEfUKfKsQ39191127;     bwpzHubHKzVcEfUKfKsQ39191127 = bwpzHubHKzVcEfUKfKsQ6863644;     bwpzHubHKzVcEfUKfKsQ6863644 = bwpzHubHKzVcEfUKfKsQ72240342;     bwpzHubHKzVcEfUKfKsQ72240342 = bwpzHubHKzVcEfUKfKsQ1566294;     bwpzHubHKzVcEfUKfKsQ1566294 = bwpzHubHKzVcEfUKfKsQ77644518;     bwpzHubHKzVcEfUKfKsQ77644518 = bwpzHubHKzVcEfUKfKsQ36520764;     bwpzHubHKzVcEfUKfKsQ36520764 = bwpzHubHKzVcEfUKfKsQ89119297;     bwpzHubHKzVcEfUKfKsQ89119297 = bwpzHubHKzVcEfUKfKsQ77872405;     bwpzHubHKzVcEfUKfKsQ77872405 = bwpzHubHKzVcEfUKfKsQ8428874;     bwpzHubHKzVcEfUKfKsQ8428874 = bwpzHubHKzVcEfUKfKsQ51991596;     bwpzHubHKzVcEfUKfKsQ51991596 = bwpzHubHKzVcEfUKfKsQ78429171;     bwpzHubHKzVcEfUKfKsQ78429171 = bwpzHubHKzVcEfUKfKsQ82550897;     bwpzHubHKzVcEfUKfKsQ82550897 = bwpzHubHKzVcEfUKfKsQ31423386;     bwpzHubHKzVcEfUKfKsQ31423386 = bwpzHubHKzVcEfUKfKsQ64398742;     bwpzHubHKzVcEfUKfKsQ64398742 = bwpzHubHKzVcEfUKfKsQ76500494;     bwpzHubHKzVcEfUKfKsQ76500494 = bwpzHubHKzVcEfUKfKsQ57091104;     bwpzHubHKzVcEfUKfKsQ57091104 = bwpzHubHKzVcEfUKfKsQ20949833;     bwpzHubHKzVcEfUKfKsQ20949833 = bwpzHubHKzVcEfUKfKsQ98915444;     bwpzHubHKzVcEfUKfKsQ98915444 = bwpzHubHKzVcEfUKfKsQ93435354;     bwpzHubHKzVcEfUKfKsQ93435354 = bwpzHubHKzVcEfUKfKsQ76623385;     bwpzHubHKzVcEfUKfKsQ76623385 = bwpzHubHKzVcEfUKfKsQ66709037;     bwpzHubHKzVcEfUKfKsQ66709037 = bwpzHubHKzVcEfUKfKsQ64828603;     bwpzHubHKzVcEfUKfKsQ64828603 = bwpzHubHKzVcEfUKfKsQ57153413;     bwpzHubHKzVcEfUKfKsQ57153413 = bwpzHubHKzVcEfUKfKsQ93271794;     bwpzHubHKzVcEfUKfKsQ93271794 = bwpzHubHKzVcEfUKfKsQ60011733;     bwpzHubHKzVcEfUKfKsQ60011733 = bwpzHubHKzVcEfUKfKsQ88544965;     bwpzHubHKzVcEfUKfKsQ88544965 = bwpzHubHKzVcEfUKfKsQ60498112;     bwpzHubHKzVcEfUKfKsQ60498112 = bwpzHubHKzVcEfUKfKsQ99025707;     bwpzHubHKzVcEfUKfKsQ99025707 = bwpzHubHKzVcEfUKfKsQ51674314;     bwpzHubHKzVcEfUKfKsQ51674314 = bwpzHubHKzVcEfUKfKsQ18751557;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void spvRudEoEryrDKWoGRgi26236709() {     float uGHIFkjJYVOmOIySnGfE89403224 = -7114635;    float uGHIFkjJYVOmOIySnGfE4584470 = -74182275;    float uGHIFkjJYVOmOIySnGfE82187373 = -648520223;    float uGHIFkjJYVOmOIySnGfE32335042 = -660189236;    float uGHIFkjJYVOmOIySnGfE54532264 = -336410272;    float uGHIFkjJYVOmOIySnGfE31975308 = -322804435;    float uGHIFkjJYVOmOIySnGfE39898613 = -683796556;    float uGHIFkjJYVOmOIySnGfE52918007 = -521838816;    float uGHIFkjJYVOmOIySnGfE22448629 = -29429645;    float uGHIFkjJYVOmOIySnGfE17095004 = -526307398;    float uGHIFkjJYVOmOIySnGfE3299959 = -521724892;    float uGHIFkjJYVOmOIySnGfE80479680 = -474079946;    float uGHIFkjJYVOmOIySnGfE38858587 = -490679027;    float uGHIFkjJYVOmOIySnGfE51280739 = -653725752;    float uGHIFkjJYVOmOIySnGfE93224427 = -659520422;    float uGHIFkjJYVOmOIySnGfE81653093 = -326545009;    float uGHIFkjJYVOmOIySnGfE19692563 = -469232267;    float uGHIFkjJYVOmOIySnGfE80868472 = -701759961;    float uGHIFkjJYVOmOIySnGfE47779869 = -891420058;    float uGHIFkjJYVOmOIySnGfE42911348 = -380442441;    float uGHIFkjJYVOmOIySnGfE78876463 = -912746682;    float uGHIFkjJYVOmOIySnGfE96974099 = -289064326;    float uGHIFkjJYVOmOIySnGfE72218750 = -323517372;    float uGHIFkjJYVOmOIySnGfE20117584 = -621113393;    float uGHIFkjJYVOmOIySnGfE34708138 = -768066796;    float uGHIFkjJYVOmOIySnGfE67121602 = -968764;    float uGHIFkjJYVOmOIySnGfE49676380 = -577190021;    float uGHIFkjJYVOmOIySnGfE6755331 = -958311227;    float uGHIFkjJYVOmOIySnGfE70666889 = -425704195;    float uGHIFkjJYVOmOIySnGfE77196211 = -1294915;    float uGHIFkjJYVOmOIySnGfE19070604 = -868338602;    float uGHIFkjJYVOmOIySnGfE48840931 = -492047984;    float uGHIFkjJYVOmOIySnGfE2181276 = 44067869;    float uGHIFkjJYVOmOIySnGfE12360206 = -275350689;    float uGHIFkjJYVOmOIySnGfE54632813 = -165263665;    float uGHIFkjJYVOmOIySnGfE2252656 = -204780022;    float uGHIFkjJYVOmOIySnGfE26450931 = -19646559;    float uGHIFkjJYVOmOIySnGfE39441098 = -406002392;    float uGHIFkjJYVOmOIySnGfE32244623 = 42932533;    float uGHIFkjJYVOmOIySnGfE88935044 = -187998918;    float uGHIFkjJYVOmOIySnGfE42631630 = -857492596;    float uGHIFkjJYVOmOIySnGfE74579182 = -966365203;    float uGHIFkjJYVOmOIySnGfE48024530 = -720223580;    float uGHIFkjJYVOmOIySnGfE87663945 = -758693040;    float uGHIFkjJYVOmOIySnGfE4349701 = -794034535;    float uGHIFkjJYVOmOIySnGfE68560710 = -757163759;    float uGHIFkjJYVOmOIySnGfE10663629 = -801128423;    float uGHIFkjJYVOmOIySnGfE28600633 = -886318118;    float uGHIFkjJYVOmOIySnGfE21863661 = -847306009;    float uGHIFkjJYVOmOIySnGfE15528502 = -500457720;    float uGHIFkjJYVOmOIySnGfE41814698 = -326395917;    float uGHIFkjJYVOmOIySnGfE77369617 = -282631341;    float uGHIFkjJYVOmOIySnGfE2314024 = 21913301;    float uGHIFkjJYVOmOIySnGfE88513387 = -633522573;    float uGHIFkjJYVOmOIySnGfE47360245 = -538455916;    float uGHIFkjJYVOmOIySnGfE92429124 = -718050310;    float uGHIFkjJYVOmOIySnGfE32365719 = -750664904;    float uGHIFkjJYVOmOIySnGfE62069790 = 72593169;    float uGHIFkjJYVOmOIySnGfE97626904 = -892122440;    float uGHIFkjJYVOmOIySnGfE87410662 = -235441509;    float uGHIFkjJYVOmOIySnGfE82298928 = -745614414;    float uGHIFkjJYVOmOIySnGfE33143283 = -725485329;    float uGHIFkjJYVOmOIySnGfE82251118 = 3865379;    float uGHIFkjJYVOmOIySnGfE45252417 = 71865269;    float uGHIFkjJYVOmOIySnGfE98024400 = -657968797;    float uGHIFkjJYVOmOIySnGfE54459027 = 70323091;    float uGHIFkjJYVOmOIySnGfE78298404 = -418147816;    float uGHIFkjJYVOmOIySnGfE26498381 = -115328338;    float uGHIFkjJYVOmOIySnGfE96647925 = -388462087;    float uGHIFkjJYVOmOIySnGfE90971772 = -354740401;    float uGHIFkjJYVOmOIySnGfE55202162 = -206898450;    float uGHIFkjJYVOmOIySnGfE80251464 = 36770124;    float uGHIFkjJYVOmOIySnGfE48623849 = -644692495;    float uGHIFkjJYVOmOIySnGfE58844825 = -603421140;    float uGHIFkjJYVOmOIySnGfE279719 = -522949846;    float uGHIFkjJYVOmOIySnGfE4297282 = -946381479;    float uGHIFkjJYVOmOIySnGfE48949569 = -568840746;    float uGHIFkjJYVOmOIySnGfE84554805 = -564824332;    float uGHIFkjJYVOmOIySnGfE15767883 = -827078858;    float uGHIFkjJYVOmOIySnGfE66147427 = 89096962;    float uGHIFkjJYVOmOIySnGfE56457973 = -199840342;    float uGHIFkjJYVOmOIySnGfE21075747 = -690871904;    float uGHIFkjJYVOmOIySnGfE84891670 = -11005219;    float uGHIFkjJYVOmOIySnGfE55138387 = -925246475;    float uGHIFkjJYVOmOIySnGfE35381514 = -674898999;    float uGHIFkjJYVOmOIySnGfE41700986 = -485707261;    float uGHIFkjJYVOmOIySnGfE46526907 = -413961286;    float uGHIFkjJYVOmOIySnGfE13667889 = -322409558;    float uGHIFkjJYVOmOIySnGfE64999960 = -736894774;    float uGHIFkjJYVOmOIySnGfE62203688 = -447213356;    float uGHIFkjJYVOmOIySnGfE69886936 = -454115118;    float uGHIFkjJYVOmOIySnGfE64381141 = 7760271;    float uGHIFkjJYVOmOIySnGfE41814194 = -513879953;    float uGHIFkjJYVOmOIySnGfE44833961 = -721625959;    float uGHIFkjJYVOmOIySnGfE6636116 = -442384505;    float uGHIFkjJYVOmOIySnGfE9488347 = -32007268;    float uGHIFkjJYVOmOIySnGfE92328063 = -870230583;    float uGHIFkjJYVOmOIySnGfE2772114 = -692088850;    float uGHIFkjJYVOmOIySnGfE89639544 = -724243;    float uGHIFkjJYVOmOIySnGfE49890673 = -7114635;     uGHIFkjJYVOmOIySnGfE89403224 = uGHIFkjJYVOmOIySnGfE4584470;     uGHIFkjJYVOmOIySnGfE4584470 = uGHIFkjJYVOmOIySnGfE82187373;     uGHIFkjJYVOmOIySnGfE82187373 = uGHIFkjJYVOmOIySnGfE32335042;     uGHIFkjJYVOmOIySnGfE32335042 = uGHIFkjJYVOmOIySnGfE54532264;     uGHIFkjJYVOmOIySnGfE54532264 = uGHIFkjJYVOmOIySnGfE31975308;     uGHIFkjJYVOmOIySnGfE31975308 = uGHIFkjJYVOmOIySnGfE39898613;     uGHIFkjJYVOmOIySnGfE39898613 = uGHIFkjJYVOmOIySnGfE52918007;     uGHIFkjJYVOmOIySnGfE52918007 = uGHIFkjJYVOmOIySnGfE22448629;     uGHIFkjJYVOmOIySnGfE22448629 = uGHIFkjJYVOmOIySnGfE17095004;     uGHIFkjJYVOmOIySnGfE17095004 = uGHIFkjJYVOmOIySnGfE3299959;     uGHIFkjJYVOmOIySnGfE3299959 = uGHIFkjJYVOmOIySnGfE80479680;     uGHIFkjJYVOmOIySnGfE80479680 = uGHIFkjJYVOmOIySnGfE38858587;     uGHIFkjJYVOmOIySnGfE38858587 = uGHIFkjJYVOmOIySnGfE51280739;     uGHIFkjJYVOmOIySnGfE51280739 = uGHIFkjJYVOmOIySnGfE93224427;     uGHIFkjJYVOmOIySnGfE93224427 = uGHIFkjJYVOmOIySnGfE81653093;     uGHIFkjJYVOmOIySnGfE81653093 = uGHIFkjJYVOmOIySnGfE19692563;     uGHIFkjJYVOmOIySnGfE19692563 = uGHIFkjJYVOmOIySnGfE80868472;     uGHIFkjJYVOmOIySnGfE80868472 = uGHIFkjJYVOmOIySnGfE47779869;     uGHIFkjJYVOmOIySnGfE47779869 = uGHIFkjJYVOmOIySnGfE42911348;     uGHIFkjJYVOmOIySnGfE42911348 = uGHIFkjJYVOmOIySnGfE78876463;     uGHIFkjJYVOmOIySnGfE78876463 = uGHIFkjJYVOmOIySnGfE96974099;     uGHIFkjJYVOmOIySnGfE96974099 = uGHIFkjJYVOmOIySnGfE72218750;     uGHIFkjJYVOmOIySnGfE72218750 = uGHIFkjJYVOmOIySnGfE20117584;     uGHIFkjJYVOmOIySnGfE20117584 = uGHIFkjJYVOmOIySnGfE34708138;     uGHIFkjJYVOmOIySnGfE34708138 = uGHIFkjJYVOmOIySnGfE67121602;     uGHIFkjJYVOmOIySnGfE67121602 = uGHIFkjJYVOmOIySnGfE49676380;     uGHIFkjJYVOmOIySnGfE49676380 = uGHIFkjJYVOmOIySnGfE6755331;     uGHIFkjJYVOmOIySnGfE6755331 = uGHIFkjJYVOmOIySnGfE70666889;     uGHIFkjJYVOmOIySnGfE70666889 = uGHIFkjJYVOmOIySnGfE77196211;     uGHIFkjJYVOmOIySnGfE77196211 = uGHIFkjJYVOmOIySnGfE19070604;     uGHIFkjJYVOmOIySnGfE19070604 = uGHIFkjJYVOmOIySnGfE48840931;     uGHIFkjJYVOmOIySnGfE48840931 = uGHIFkjJYVOmOIySnGfE2181276;     uGHIFkjJYVOmOIySnGfE2181276 = uGHIFkjJYVOmOIySnGfE12360206;     uGHIFkjJYVOmOIySnGfE12360206 = uGHIFkjJYVOmOIySnGfE54632813;     uGHIFkjJYVOmOIySnGfE54632813 = uGHIFkjJYVOmOIySnGfE2252656;     uGHIFkjJYVOmOIySnGfE2252656 = uGHIFkjJYVOmOIySnGfE26450931;     uGHIFkjJYVOmOIySnGfE26450931 = uGHIFkjJYVOmOIySnGfE39441098;     uGHIFkjJYVOmOIySnGfE39441098 = uGHIFkjJYVOmOIySnGfE32244623;     uGHIFkjJYVOmOIySnGfE32244623 = uGHIFkjJYVOmOIySnGfE88935044;     uGHIFkjJYVOmOIySnGfE88935044 = uGHIFkjJYVOmOIySnGfE42631630;     uGHIFkjJYVOmOIySnGfE42631630 = uGHIFkjJYVOmOIySnGfE74579182;     uGHIFkjJYVOmOIySnGfE74579182 = uGHIFkjJYVOmOIySnGfE48024530;     uGHIFkjJYVOmOIySnGfE48024530 = uGHIFkjJYVOmOIySnGfE87663945;     uGHIFkjJYVOmOIySnGfE87663945 = uGHIFkjJYVOmOIySnGfE4349701;     uGHIFkjJYVOmOIySnGfE4349701 = uGHIFkjJYVOmOIySnGfE68560710;     uGHIFkjJYVOmOIySnGfE68560710 = uGHIFkjJYVOmOIySnGfE10663629;     uGHIFkjJYVOmOIySnGfE10663629 = uGHIFkjJYVOmOIySnGfE28600633;     uGHIFkjJYVOmOIySnGfE28600633 = uGHIFkjJYVOmOIySnGfE21863661;     uGHIFkjJYVOmOIySnGfE21863661 = uGHIFkjJYVOmOIySnGfE15528502;     uGHIFkjJYVOmOIySnGfE15528502 = uGHIFkjJYVOmOIySnGfE41814698;     uGHIFkjJYVOmOIySnGfE41814698 = uGHIFkjJYVOmOIySnGfE77369617;     uGHIFkjJYVOmOIySnGfE77369617 = uGHIFkjJYVOmOIySnGfE2314024;     uGHIFkjJYVOmOIySnGfE2314024 = uGHIFkjJYVOmOIySnGfE88513387;     uGHIFkjJYVOmOIySnGfE88513387 = uGHIFkjJYVOmOIySnGfE47360245;     uGHIFkjJYVOmOIySnGfE47360245 = uGHIFkjJYVOmOIySnGfE92429124;     uGHIFkjJYVOmOIySnGfE92429124 = uGHIFkjJYVOmOIySnGfE32365719;     uGHIFkjJYVOmOIySnGfE32365719 = uGHIFkjJYVOmOIySnGfE62069790;     uGHIFkjJYVOmOIySnGfE62069790 = uGHIFkjJYVOmOIySnGfE97626904;     uGHIFkjJYVOmOIySnGfE97626904 = uGHIFkjJYVOmOIySnGfE87410662;     uGHIFkjJYVOmOIySnGfE87410662 = uGHIFkjJYVOmOIySnGfE82298928;     uGHIFkjJYVOmOIySnGfE82298928 = uGHIFkjJYVOmOIySnGfE33143283;     uGHIFkjJYVOmOIySnGfE33143283 = uGHIFkjJYVOmOIySnGfE82251118;     uGHIFkjJYVOmOIySnGfE82251118 = uGHIFkjJYVOmOIySnGfE45252417;     uGHIFkjJYVOmOIySnGfE45252417 = uGHIFkjJYVOmOIySnGfE98024400;     uGHIFkjJYVOmOIySnGfE98024400 = uGHIFkjJYVOmOIySnGfE54459027;     uGHIFkjJYVOmOIySnGfE54459027 = uGHIFkjJYVOmOIySnGfE78298404;     uGHIFkjJYVOmOIySnGfE78298404 = uGHIFkjJYVOmOIySnGfE26498381;     uGHIFkjJYVOmOIySnGfE26498381 = uGHIFkjJYVOmOIySnGfE96647925;     uGHIFkjJYVOmOIySnGfE96647925 = uGHIFkjJYVOmOIySnGfE90971772;     uGHIFkjJYVOmOIySnGfE90971772 = uGHIFkjJYVOmOIySnGfE55202162;     uGHIFkjJYVOmOIySnGfE55202162 = uGHIFkjJYVOmOIySnGfE80251464;     uGHIFkjJYVOmOIySnGfE80251464 = uGHIFkjJYVOmOIySnGfE48623849;     uGHIFkjJYVOmOIySnGfE48623849 = uGHIFkjJYVOmOIySnGfE58844825;     uGHIFkjJYVOmOIySnGfE58844825 = uGHIFkjJYVOmOIySnGfE279719;     uGHIFkjJYVOmOIySnGfE279719 = uGHIFkjJYVOmOIySnGfE4297282;     uGHIFkjJYVOmOIySnGfE4297282 = uGHIFkjJYVOmOIySnGfE48949569;     uGHIFkjJYVOmOIySnGfE48949569 = uGHIFkjJYVOmOIySnGfE84554805;     uGHIFkjJYVOmOIySnGfE84554805 = uGHIFkjJYVOmOIySnGfE15767883;     uGHIFkjJYVOmOIySnGfE15767883 = uGHIFkjJYVOmOIySnGfE66147427;     uGHIFkjJYVOmOIySnGfE66147427 = uGHIFkjJYVOmOIySnGfE56457973;     uGHIFkjJYVOmOIySnGfE56457973 = uGHIFkjJYVOmOIySnGfE21075747;     uGHIFkjJYVOmOIySnGfE21075747 = uGHIFkjJYVOmOIySnGfE84891670;     uGHIFkjJYVOmOIySnGfE84891670 = uGHIFkjJYVOmOIySnGfE55138387;     uGHIFkjJYVOmOIySnGfE55138387 = uGHIFkjJYVOmOIySnGfE35381514;     uGHIFkjJYVOmOIySnGfE35381514 = uGHIFkjJYVOmOIySnGfE41700986;     uGHIFkjJYVOmOIySnGfE41700986 = uGHIFkjJYVOmOIySnGfE46526907;     uGHIFkjJYVOmOIySnGfE46526907 = uGHIFkjJYVOmOIySnGfE13667889;     uGHIFkjJYVOmOIySnGfE13667889 = uGHIFkjJYVOmOIySnGfE64999960;     uGHIFkjJYVOmOIySnGfE64999960 = uGHIFkjJYVOmOIySnGfE62203688;     uGHIFkjJYVOmOIySnGfE62203688 = uGHIFkjJYVOmOIySnGfE69886936;     uGHIFkjJYVOmOIySnGfE69886936 = uGHIFkjJYVOmOIySnGfE64381141;     uGHIFkjJYVOmOIySnGfE64381141 = uGHIFkjJYVOmOIySnGfE41814194;     uGHIFkjJYVOmOIySnGfE41814194 = uGHIFkjJYVOmOIySnGfE44833961;     uGHIFkjJYVOmOIySnGfE44833961 = uGHIFkjJYVOmOIySnGfE6636116;     uGHIFkjJYVOmOIySnGfE6636116 = uGHIFkjJYVOmOIySnGfE9488347;     uGHIFkjJYVOmOIySnGfE9488347 = uGHIFkjJYVOmOIySnGfE92328063;     uGHIFkjJYVOmOIySnGfE92328063 = uGHIFkjJYVOmOIySnGfE2772114;     uGHIFkjJYVOmOIySnGfE2772114 = uGHIFkjJYVOmOIySnGfE89639544;     uGHIFkjJYVOmOIySnGfE89639544 = uGHIFkjJYVOmOIySnGfE49890673;     uGHIFkjJYVOmOIySnGfE49890673 = uGHIFkjJYVOmOIySnGfE89403224;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void vBuUTJJVBbVkTMhhxrwY63211881() {     float JqaQYhkRzJRlzQrfOqcw57129472 = -176515156;    float JqaQYhkRzJRlzQrfOqcw80506666 = -585755641;    float JqaQYhkRzJRlzQrfOqcw51537218 = -459918068;    float JqaQYhkRzJRlzQrfOqcw60986650 = -481035186;    float JqaQYhkRzJRlzQrfOqcw87094949 = -623387356;    float JqaQYhkRzJRlzQrfOqcw67489460 = -853694788;    float JqaQYhkRzJRlzQrfOqcw79500984 = 91572343;    float JqaQYhkRzJRlzQrfOqcw68590813 = -196044544;    float JqaQYhkRzJRlzQrfOqcw14577514 = -680418383;    float JqaQYhkRzJRlzQrfOqcw22866117 = -727054333;    float JqaQYhkRzJRlzQrfOqcw82551128 = -952464793;    float JqaQYhkRzJRlzQrfOqcw87909593 = -273879513;    float JqaQYhkRzJRlzQrfOqcw73902366 = -227514517;    float JqaQYhkRzJRlzQrfOqcw54773280 = -814483983;    float JqaQYhkRzJRlzQrfOqcw44907740 = -342589355;    float JqaQYhkRzJRlzQrfOqcw10706018 = -310848637;    float JqaQYhkRzJRlzQrfOqcw25536911 = -92526828;    float JqaQYhkRzJRlzQrfOqcw42248835 = -387009806;    float JqaQYhkRzJRlzQrfOqcw8269170 = -258692902;    float JqaQYhkRzJRlzQrfOqcw75866838 = -899735200;    float JqaQYhkRzJRlzQrfOqcw25254607 = -46617213;    float JqaQYhkRzJRlzQrfOqcw87832994 = -37378414;    float JqaQYhkRzJRlzQrfOqcw75920638 = -371812385;    float JqaQYhkRzJRlzQrfOqcw88839110 = -266985956;    float JqaQYhkRzJRlzQrfOqcw2938459 = -443406322;    float JqaQYhkRzJRlzQrfOqcw30757726 = -349360132;    float JqaQYhkRzJRlzQrfOqcw64997457 = -80731948;    float JqaQYhkRzJRlzQrfOqcw94821305 = -247680809;    float JqaQYhkRzJRlzQrfOqcw50340220 = 64213246;    float JqaQYhkRzJRlzQrfOqcw68007852 = -406264187;    float JqaQYhkRzJRlzQrfOqcw45068559 = -787875462;    float JqaQYhkRzJRlzQrfOqcw60114369 = -24444572;    float JqaQYhkRzJRlzQrfOqcw1016493 = -130702227;    float JqaQYhkRzJRlzQrfOqcw77375837 = -796038462;    float JqaQYhkRzJRlzQrfOqcw78716058 = -270470753;    float JqaQYhkRzJRlzQrfOqcw99447171 = -351945548;    float JqaQYhkRzJRlzQrfOqcw57127523 = -1821269;    float JqaQYhkRzJRlzQrfOqcw50007640 = -394961522;    float JqaQYhkRzJRlzQrfOqcw65096702 = -688846313;    float JqaQYhkRzJRlzQrfOqcw57007256 = -275346034;    float JqaQYhkRzJRlzQrfOqcw3285268 = -236064003;    float JqaQYhkRzJRlzQrfOqcw12110576 = -310533890;    float JqaQYhkRzJRlzQrfOqcw22857900 = -226379100;    float JqaQYhkRzJRlzQrfOqcw37345563 = -839341466;    float JqaQYhkRzJRlzQrfOqcw96366955 = -943919817;    float JqaQYhkRzJRlzQrfOqcw61755359 = -682647911;    float JqaQYhkRzJRlzQrfOqcw34185107 = -733761915;    float JqaQYhkRzJRlzQrfOqcw25155579 = -173962093;    float JqaQYhkRzJRlzQrfOqcw92639188 = -461056379;    float JqaQYhkRzJRlzQrfOqcw87816007 = 10516429;    float JqaQYhkRzJRlzQrfOqcw88829744 = -602811699;    float JqaQYhkRzJRlzQrfOqcw76385982 = -580530388;    float JqaQYhkRzJRlzQrfOqcw24348773 = -902136480;    float JqaQYhkRzJRlzQrfOqcw1634446 = -445236749;    float JqaQYhkRzJRlzQrfOqcw37957274 = -40150627;    float JqaQYhkRzJRlzQrfOqcw69296478 = -39136742;    float JqaQYhkRzJRlzQrfOqcw4586029 = -113943257;    float JqaQYhkRzJRlzQrfOqcw62698107 = -92932112;    float JqaQYhkRzJRlzQrfOqcw58048192 = 62371135;    float JqaQYhkRzJRlzQrfOqcw56337223 = -174027224;    float JqaQYhkRzJRlzQrfOqcw2492003 = -672962841;    float JqaQYhkRzJRlzQrfOqcw84679678 = -660746848;    float JqaQYhkRzJRlzQrfOqcw18250594 = -160257791;    float JqaQYhkRzJRlzQrfOqcw46569662 = -174154197;    float JqaQYhkRzJRlzQrfOqcw77797558 = -939178871;    float JqaQYhkRzJRlzQrfOqcw22436760 = -828020221;    float JqaQYhkRzJRlzQrfOqcw86893100 = -43177286;    float JqaQYhkRzJRlzQrfOqcw96526528 = -431476055;    float JqaQYhkRzJRlzQrfOqcw76057221 = -444013230;    float JqaQYhkRzJRlzQrfOqcw45460569 = -990643808;    float JqaQYhkRzJRlzQrfOqcw53578494 = -209027369;    float JqaQYhkRzJRlzQrfOqcw75529270 = -697565307;    float JqaQYhkRzJRlzQrfOqcw77152132 = -698163494;    float JqaQYhkRzJRlzQrfOqcw51261913 = -983346868;    float JqaQYhkRzJRlzQrfOqcw72581571 = -563671198;    float JqaQYhkRzJRlzQrfOqcw13144032 = -736083324;    float JqaQYhkRzJRlzQrfOqcw64975095 = -810999315;    float JqaQYhkRzJRlzQrfOqcw38575075 = -532470919;    float JqaQYhkRzJRlzQrfOqcw92472154 = -323066140;    float JqaQYhkRzJRlzQrfOqcw41183099 = -760758412;    float JqaQYhkRzJRlzQrfOqcw96572619 = -615598218;    float JqaQYhkRzJRlzQrfOqcw39841879 = -906769856;    float JqaQYhkRzJRlzQrfOqcw2182118 = -786624430;    float JqaQYhkRzJRlzQrfOqcw62524212 = -946303183;    float JqaQYhkRzJRlzQrfOqcw79178107 = -803452489;    float JqaQYhkRzJRlzQrfOqcw68682576 = -107345075;    float JqaQYhkRzJRlzQrfOqcw35765597 = -122308092;    float JqaQYhkRzJRlzQrfOqcw99382047 = -685465479;    float JqaQYhkRzJRlzQrfOqcw39418564 = -655887836;    float JqaQYhkRzJRlzQrfOqcw9419581 = -131334011;    float JqaQYhkRzJRlzQrfOqcw94861143 = -138002291;    float JqaQYhkRzJRlzQrfOqcw94429415 = -908889157;    float JqaQYhkRzJRlzQrfOqcw91959447 = -357332658;    float JqaQYhkRzJRlzQrfOqcw8759479 = -414819089;    float JqaQYhkRzJRlzQrfOqcw54515254 = -602383194;    float JqaQYhkRzJRlzQrfOqcw18605589 = -575317155;    float JqaQYhkRzJRlzQrfOqcw93859982 = -50276099;    float JqaQYhkRzJRlzQrfOqcw76288238 = 47775097;    float JqaQYhkRzJRlzQrfOqcw59548005 = -900162596;    float JqaQYhkRzJRlzQrfOqcw73930196 = -176515156;     JqaQYhkRzJRlzQrfOqcw57129472 = JqaQYhkRzJRlzQrfOqcw80506666;     JqaQYhkRzJRlzQrfOqcw80506666 = JqaQYhkRzJRlzQrfOqcw51537218;     JqaQYhkRzJRlzQrfOqcw51537218 = JqaQYhkRzJRlzQrfOqcw60986650;     JqaQYhkRzJRlzQrfOqcw60986650 = JqaQYhkRzJRlzQrfOqcw87094949;     JqaQYhkRzJRlzQrfOqcw87094949 = JqaQYhkRzJRlzQrfOqcw67489460;     JqaQYhkRzJRlzQrfOqcw67489460 = JqaQYhkRzJRlzQrfOqcw79500984;     JqaQYhkRzJRlzQrfOqcw79500984 = JqaQYhkRzJRlzQrfOqcw68590813;     JqaQYhkRzJRlzQrfOqcw68590813 = JqaQYhkRzJRlzQrfOqcw14577514;     JqaQYhkRzJRlzQrfOqcw14577514 = JqaQYhkRzJRlzQrfOqcw22866117;     JqaQYhkRzJRlzQrfOqcw22866117 = JqaQYhkRzJRlzQrfOqcw82551128;     JqaQYhkRzJRlzQrfOqcw82551128 = JqaQYhkRzJRlzQrfOqcw87909593;     JqaQYhkRzJRlzQrfOqcw87909593 = JqaQYhkRzJRlzQrfOqcw73902366;     JqaQYhkRzJRlzQrfOqcw73902366 = JqaQYhkRzJRlzQrfOqcw54773280;     JqaQYhkRzJRlzQrfOqcw54773280 = JqaQYhkRzJRlzQrfOqcw44907740;     JqaQYhkRzJRlzQrfOqcw44907740 = JqaQYhkRzJRlzQrfOqcw10706018;     JqaQYhkRzJRlzQrfOqcw10706018 = JqaQYhkRzJRlzQrfOqcw25536911;     JqaQYhkRzJRlzQrfOqcw25536911 = JqaQYhkRzJRlzQrfOqcw42248835;     JqaQYhkRzJRlzQrfOqcw42248835 = JqaQYhkRzJRlzQrfOqcw8269170;     JqaQYhkRzJRlzQrfOqcw8269170 = JqaQYhkRzJRlzQrfOqcw75866838;     JqaQYhkRzJRlzQrfOqcw75866838 = JqaQYhkRzJRlzQrfOqcw25254607;     JqaQYhkRzJRlzQrfOqcw25254607 = JqaQYhkRzJRlzQrfOqcw87832994;     JqaQYhkRzJRlzQrfOqcw87832994 = JqaQYhkRzJRlzQrfOqcw75920638;     JqaQYhkRzJRlzQrfOqcw75920638 = JqaQYhkRzJRlzQrfOqcw88839110;     JqaQYhkRzJRlzQrfOqcw88839110 = JqaQYhkRzJRlzQrfOqcw2938459;     JqaQYhkRzJRlzQrfOqcw2938459 = JqaQYhkRzJRlzQrfOqcw30757726;     JqaQYhkRzJRlzQrfOqcw30757726 = JqaQYhkRzJRlzQrfOqcw64997457;     JqaQYhkRzJRlzQrfOqcw64997457 = JqaQYhkRzJRlzQrfOqcw94821305;     JqaQYhkRzJRlzQrfOqcw94821305 = JqaQYhkRzJRlzQrfOqcw50340220;     JqaQYhkRzJRlzQrfOqcw50340220 = JqaQYhkRzJRlzQrfOqcw68007852;     JqaQYhkRzJRlzQrfOqcw68007852 = JqaQYhkRzJRlzQrfOqcw45068559;     JqaQYhkRzJRlzQrfOqcw45068559 = JqaQYhkRzJRlzQrfOqcw60114369;     JqaQYhkRzJRlzQrfOqcw60114369 = JqaQYhkRzJRlzQrfOqcw1016493;     JqaQYhkRzJRlzQrfOqcw1016493 = JqaQYhkRzJRlzQrfOqcw77375837;     JqaQYhkRzJRlzQrfOqcw77375837 = JqaQYhkRzJRlzQrfOqcw78716058;     JqaQYhkRzJRlzQrfOqcw78716058 = JqaQYhkRzJRlzQrfOqcw99447171;     JqaQYhkRzJRlzQrfOqcw99447171 = JqaQYhkRzJRlzQrfOqcw57127523;     JqaQYhkRzJRlzQrfOqcw57127523 = JqaQYhkRzJRlzQrfOqcw50007640;     JqaQYhkRzJRlzQrfOqcw50007640 = JqaQYhkRzJRlzQrfOqcw65096702;     JqaQYhkRzJRlzQrfOqcw65096702 = JqaQYhkRzJRlzQrfOqcw57007256;     JqaQYhkRzJRlzQrfOqcw57007256 = JqaQYhkRzJRlzQrfOqcw3285268;     JqaQYhkRzJRlzQrfOqcw3285268 = JqaQYhkRzJRlzQrfOqcw12110576;     JqaQYhkRzJRlzQrfOqcw12110576 = JqaQYhkRzJRlzQrfOqcw22857900;     JqaQYhkRzJRlzQrfOqcw22857900 = JqaQYhkRzJRlzQrfOqcw37345563;     JqaQYhkRzJRlzQrfOqcw37345563 = JqaQYhkRzJRlzQrfOqcw96366955;     JqaQYhkRzJRlzQrfOqcw96366955 = JqaQYhkRzJRlzQrfOqcw61755359;     JqaQYhkRzJRlzQrfOqcw61755359 = JqaQYhkRzJRlzQrfOqcw34185107;     JqaQYhkRzJRlzQrfOqcw34185107 = JqaQYhkRzJRlzQrfOqcw25155579;     JqaQYhkRzJRlzQrfOqcw25155579 = JqaQYhkRzJRlzQrfOqcw92639188;     JqaQYhkRzJRlzQrfOqcw92639188 = JqaQYhkRzJRlzQrfOqcw87816007;     JqaQYhkRzJRlzQrfOqcw87816007 = JqaQYhkRzJRlzQrfOqcw88829744;     JqaQYhkRzJRlzQrfOqcw88829744 = JqaQYhkRzJRlzQrfOqcw76385982;     JqaQYhkRzJRlzQrfOqcw76385982 = JqaQYhkRzJRlzQrfOqcw24348773;     JqaQYhkRzJRlzQrfOqcw24348773 = JqaQYhkRzJRlzQrfOqcw1634446;     JqaQYhkRzJRlzQrfOqcw1634446 = JqaQYhkRzJRlzQrfOqcw37957274;     JqaQYhkRzJRlzQrfOqcw37957274 = JqaQYhkRzJRlzQrfOqcw69296478;     JqaQYhkRzJRlzQrfOqcw69296478 = JqaQYhkRzJRlzQrfOqcw4586029;     JqaQYhkRzJRlzQrfOqcw4586029 = JqaQYhkRzJRlzQrfOqcw62698107;     JqaQYhkRzJRlzQrfOqcw62698107 = JqaQYhkRzJRlzQrfOqcw58048192;     JqaQYhkRzJRlzQrfOqcw58048192 = JqaQYhkRzJRlzQrfOqcw56337223;     JqaQYhkRzJRlzQrfOqcw56337223 = JqaQYhkRzJRlzQrfOqcw2492003;     JqaQYhkRzJRlzQrfOqcw2492003 = JqaQYhkRzJRlzQrfOqcw84679678;     JqaQYhkRzJRlzQrfOqcw84679678 = JqaQYhkRzJRlzQrfOqcw18250594;     JqaQYhkRzJRlzQrfOqcw18250594 = JqaQYhkRzJRlzQrfOqcw46569662;     JqaQYhkRzJRlzQrfOqcw46569662 = JqaQYhkRzJRlzQrfOqcw77797558;     JqaQYhkRzJRlzQrfOqcw77797558 = JqaQYhkRzJRlzQrfOqcw22436760;     JqaQYhkRzJRlzQrfOqcw22436760 = JqaQYhkRzJRlzQrfOqcw86893100;     JqaQYhkRzJRlzQrfOqcw86893100 = JqaQYhkRzJRlzQrfOqcw96526528;     JqaQYhkRzJRlzQrfOqcw96526528 = JqaQYhkRzJRlzQrfOqcw76057221;     JqaQYhkRzJRlzQrfOqcw76057221 = JqaQYhkRzJRlzQrfOqcw45460569;     JqaQYhkRzJRlzQrfOqcw45460569 = JqaQYhkRzJRlzQrfOqcw53578494;     JqaQYhkRzJRlzQrfOqcw53578494 = JqaQYhkRzJRlzQrfOqcw75529270;     JqaQYhkRzJRlzQrfOqcw75529270 = JqaQYhkRzJRlzQrfOqcw77152132;     JqaQYhkRzJRlzQrfOqcw77152132 = JqaQYhkRzJRlzQrfOqcw51261913;     JqaQYhkRzJRlzQrfOqcw51261913 = JqaQYhkRzJRlzQrfOqcw72581571;     JqaQYhkRzJRlzQrfOqcw72581571 = JqaQYhkRzJRlzQrfOqcw13144032;     JqaQYhkRzJRlzQrfOqcw13144032 = JqaQYhkRzJRlzQrfOqcw64975095;     JqaQYhkRzJRlzQrfOqcw64975095 = JqaQYhkRzJRlzQrfOqcw38575075;     JqaQYhkRzJRlzQrfOqcw38575075 = JqaQYhkRzJRlzQrfOqcw92472154;     JqaQYhkRzJRlzQrfOqcw92472154 = JqaQYhkRzJRlzQrfOqcw41183099;     JqaQYhkRzJRlzQrfOqcw41183099 = JqaQYhkRzJRlzQrfOqcw96572619;     JqaQYhkRzJRlzQrfOqcw96572619 = JqaQYhkRzJRlzQrfOqcw39841879;     JqaQYhkRzJRlzQrfOqcw39841879 = JqaQYhkRzJRlzQrfOqcw2182118;     JqaQYhkRzJRlzQrfOqcw2182118 = JqaQYhkRzJRlzQrfOqcw62524212;     JqaQYhkRzJRlzQrfOqcw62524212 = JqaQYhkRzJRlzQrfOqcw79178107;     JqaQYhkRzJRlzQrfOqcw79178107 = JqaQYhkRzJRlzQrfOqcw68682576;     JqaQYhkRzJRlzQrfOqcw68682576 = JqaQYhkRzJRlzQrfOqcw35765597;     JqaQYhkRzJRlzQrfOqcw35765597 = JqaQYhkRzJRlzQrfOqcw99382047;     JqaQYhkRzJRlzQrfOqcw99382047 = JqaQYhkRzJRlzQrfOqcw39418564;     JqaQYhkRzJRlzQrfOqcw39418564 = JqaQYhkRzJRlzQrfOqcw9419581;     JqaQYhkRzJRlzQrfOqcw9419581 = JqaQYhkRzJRlzQrfOqcw94861143;     JqaQYhkRzJRlzQrfOqcw94861143 = JqaQYhkRzJRlzQrfOqcw94429415;     JqaQYhkRzJRlzQrfOqcw94429415 = JqaQYhkRzJRlzQrfOqcw91959447;     JqaQYhkRzJRlzQrfOqcw91959447 = JqaQYhkRzJRlzQrfOqcw8759479;     JqaQYhkRzJRlzQrfOqcw8759479 = JqaQYhkRzJRlzQrfOqcw54515254;     JqaQYhkRzJRlzQrfOqcw54515254 = JqaQYhkRzJRlzQrfOqcw18605589;     JqaQYhkRzJRlzQrfOqcw18605589 = JqaQYhkRzJRlzQrfOqcw93859982;     JqaQYhkRzJRlzQrfOqcw93859982 = JqaQYhkRzJRlzQrfOqcw76288238;     JqaQYhkRzJRlzQrfOqcw76288238 = JqaQYhkRzJRlzQrfOqcw59548005;     JqaQYhkRzJRlzQrfOqcw59548005 = JqaQYhkRzJRlzQrfOqcw73930196;     JqaQYhkRzJRlzQrfOqcw73930196 = JqaQYhkRzJRlzQrfOqcw57129472;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kNsZtKzMCRxdCAaOwqyd17628094() {     float naRrmtYsEnFKUdMcVsHl27781140 = -794664286;    float naRrmtYsEnFKUdMcVsHl49180184 = -457398940;    float naRrmtYsEnFKUdMcVsHl31506137 = -902212527;    float naRrmtYsEnFKUdMcVsHl917937 = -671173576;    float naRrmtYsEnFKUdMcVsHl8545272 = -406793703;    float naRrmtYsEnFKUdMcVsHl57489223 = -555275940;    float naRrmtYsEnFKUdMcVsHl97879879 = 92796633;    float naRrmtYsEnFKUdMcVsHl25433194 = -542861634;    float naRrmtYsEnFKUdMcVsHl60442568 = -773765712;    float naRrmtYsEnFKUdMcVsHl45832578 = 71850316;    float naRrmtYsEnFKUdMcVsHl24800290 = -987366722;    float naRrmtYsEnFKUdMcVsHl33456165 = -589643306;    float naRrmtYsEnFKUdMcVsHl53729753 = -137051575;    float naRrmtYsEnFKUdMcVsHl38620314 = -615512193;    float naRrmtYsEnFKUdMcVsHl27627760 = -492339359;    float naRrmtYsEnFKUdMcVsHl43234658 = -980832224;    float naRrmtYsEnFKUdMcVsHl19389737 = -1711958;    float naRrmtYsEnFKUdMcVsHl62542329 = -376176002;    float naRrmtYsEnFKUdMcVsHl89976950 = -965880844;    float naRrmtYsEnFKUdMcVsHl34653548 = -342101505;    float naRrmtYsEnFKUdMcVsHl80390361 = 44703646;    float naRrmtYsEnFKUdMcVsHl49228760 = -368663720;    float naRrmtYsEnFKUdMcVsHl77864447 = -939655529;    float naRrmtYsEnFKUdMcVsHl22688648 = -959386862;    float naRrmtYsEnFKUdMcVsHl67062847 = -119311010;    float naRrmtYsEnFKUdMcVsHl61355307 = -447644577;    float naRrmtYsEnFKUdMcVsHl73258243 = -475909723;    float naRrmtYsEnFKUdMcVsHl2603531 = -752804576;    float naRrmtYsEnFKUdMcVsHl82482707 = -230144971;    float naRrmtYsEnFKUdMcVsHl7179948 = -964659258;    float naRrmtYsEnFKUdMcVsHl52140555 = -536309148;    float naRrmtYsEnFKUdMcVsHl4625831 = -352741735;    float naRrmtYsEnFKUdMcVsHl83961853 = -77663836;    float naRrmtYsEnFKUdMcVsHl42740813 = -359568147;    float naRrmtYsEnFKUdMcVsHl56740295 = -117417533;    float naRrmtYsEnFKUdMcVsHl59440431 = -431172096;    float naRrmtYsEnFKUdMcVsHl919009 = -190414476;    float naRrmtYsEnFKUdMcVsHl2800129 = -179074629;    float naRrmtYsEnFKUdMcVsHl43629991 = -644574950;    float naRrmtYsEnFKUdMcVsHl52110555 = -892954397;    float naRrmtYsEnFKUdMcVsHl63358551 = -620082881;    float naRrmtYsEnFKUdMcVsHl40593567 = -79922959;    float naRrmtYsEnFKUdMcVsHl71824859 = -213133152;    float naRrmtYsEnFKUdMcVsHl43853864 = -911316438;    float naRrmtYsEnFKUdMcVsHl92321014 = -152566211;    float naRrmtYsEnFKUdMcVsHl68161194 = -440985066;    float naRrmtYsEnFKUdMcVsHl60316310 = -849831714;    float naRrmtYsEnFKUdMcVsHl90769789 = -626092388;    float naRrmtYsEnFKUdMcVsHl98080640 = -446820349;    float naRrmtYsEnFKUdMcVsHl96243493 = -272492450;    float naRrmtYsEnFKUdMcVsHl57019069 = -149383820;    float naRrmtYsEnFKUdMcVsHl18257487 = -549750110;    float naRrmtYsEnFKUdMcVsHl79424429 = -829934522;    float naRrmtYsEnFKUdMcVsHl91861750 = -6118246;    float naRrmtYsEnFKUdMcVsHl37237733 = -151091542;    float naRrmtYsEnFKUdMcVsHl78552380 = -326000567;    float naRrmtYsEnFKUdMcVsHl71315737 = -517743411;    float naRrmtYsEnFKUdMcVsHl8817490 = -942825665;    float naRrmtYsEnFKUdMcVsHl33855090 = -451862566;    float naRrmtYsEnFKUdMcVsHl47189964 = -959149127;    float naRrmtYsEnFKUdMcVsHl84230979 = 20633783;    float naRrmtYsEnFKUdMcVsHl95276348 = -154398791;    float naRrmtYsEnFKUdMcVsHl42950486 = -212716664;    float naRrmtYsEnFKUdMcVsHl53262620 = -809106455;    float naRrmtYsEnFKUdMcVsHl93692022 = -391840536;    float naRrmtYsEnFKUdMcVsHl20174460 = -534624988;    float naRrmtYsEnFKUdMcVsHl49494311 = -411979471;    float naRrmtYsEnFKUdMcVsHl10988941 = -777483429;    float naRrmtYsEnFKUdMcVsHl81880018 = -398094661;    float naRrmtYsEnFKUdMcVsHl68187329 = 38832737;    float naRrmtYsEnFKUdMcVsHl42315649 = -690417749;    float naRrmtYsEnFKUdMcVsHl16589609 = -822637329;    float naRrmtYsEnFKUdMcVsHl18912338 = -731601053;    float naRrmtYsEnFKUdMcVsHl37866396 = 27073552;    float naRrmtYsEnFKUdMcVsHl71294997 = -722018624;    float naRrmtYsEnFKUdMcVsHl39796795 = -875373395;    float naRrmtYsEnFKUdMcVsHl77403900 = -55530569;    float naRrmtYsEnFKUdMcVsHl34010583 = 71660908;    float naRrmtYsEnFKUdMcVsHl30367633 = -706820652;    float naRrmtYsEnFKUdMcVsHl98901652 = -678325945;    float naRrmtYsEnFKUdMcVsHl1038997 = -597812863;    float naRrmtYsEnFKUdMcVsHl82488454 = -849817336;    float naRrmtYsEnFKUdMcVsHl4522891 = -205984228;    float naRrmtYsEnFKUdMcVsHl86239213 = -957652521;    float naRrmtYsEnFKUdMcVsHl50160879 = -715275438;    float naRrmtYsEnFKUdMcVsHl33883069 = -986559038;    float naRrmtYsEnFKUdMcVsHl25201401 = -522807213;    float naRrmtYsEnFKUdMcVsHl92100103 = 28454409;    float naRrmtYsEnFKUdMcVsHl5503080 = -108476606;    float naRrmtYsEnFKUdMcVsHl78187915 = -791416966;    float naRrmtYsEnFKUdMcVsHl88124693 = -913428685;    float naRrmtYsEnFKUdMcVsHl92101519 = -247588811;    float naRrmtYsEnFKUdMcVsHl68945038 = -727212064;    float naRrmtYsEnFKUdMcVsHl96440026 = -685425823;    float naRrmtYsEnFKUdMcVsHl67879575 = -813588180;    float naRrmtYsEnFKUdMcVsHl68082202 = -365684091;    float naRrmtYsEnFKUdMcVsHl97643080 = -867206296;    float naRrmtYsEnFKUdMcVsHl18562240 = -404026697;    float naRrmtYsEnFKUdMcVsHl50161841 = -419475902;    float naRrmtYsEnFKUdMcVsHl72146555 = -794664286;     naRrmtYsEnFKUdMcVsHl27781140 = naRrmtYsEnFKUdMcVsHl49180184;     naRrmtYsEnFKUdMcVsHl49180184 = naRrmtYsEnFKUdMcVsHl31506137;     naRrmtYsEnFKUdMcVsHl31506137 = naRrmtYsEnFKUdMcVsHl917937;     naRrmtYsEnFKUdMcVsHl917937 = naRrmtYsEnFKUdMcVsHl8545272;     naRrmtYsEnFKUdMcVsHl8545272 = naRrmtYsEnFKUdMcVsHl57489223;     naRrmtYsEnFKUdMcVsHl57489223 = naRrmtYsEnFKUdMcVsHl97879879;     naRrmtYsEnFKUdMcVsHl97879879 = naRrmtYsEnFKUdMcVsHl25433194;     naRrmtYsEnFKUdMcVsHl25433194 = naRrmtYsEnFKUdMcVsHl60442568;     naRrmtYsEnFKUdMcVsHl60442568 = naRrmtYsEnFKUdMcVsHl45832578;     naRrmtYsEnFKUdMcVsHl45832578 = naRrmtYsEnFKUdMcVsHl24800290;     naRrmtYsEnFKUdMcVsHl24800290 = naRrmtYsEnFKUdMcVsHl33456165;     naRrmtYsEnFKUdMcVsHl33456165 = naRrmtYsEnFKUdMcVsHl53729753;     naRrmtYsEnFKUdMcVsHl53729753 = naRrmtYsEnFKUdMcVsHl38620314;     naRrmtYsEnFKUdMcVsHl38620314 = naRrmtYsEnFKUdMcVsHl27627760;     naRrmtYsEnFKUdMcVsHl27627760 = naRrmtYsEnFKUdMcVsHl43234658;     naRrmtYsEnFKUdMcVsHl43234658 = naRrmtYsEnFKUdMcVsHl19389737;     naRrmtYsEnFKUdMcVsHl19389737 = naRrmtYsEnFKUdMcVsHl62542329;     naRrmtYsEnFKUdMcVsHl62542329 = naRrmtYsEnFKUdMcVsHl89976950;     naRrmtYsEnFKUdMcVsHl89976950 = naRrmtYsEnFKUdMcVsHl34653548;     naRrmtYsEnFKUdMcVsHl34653548 = naRrmtYsEnFKUdMcVsHl80390361;     naRrmtYsEnFKUdMcVsHl80390361 = naRrmtYsEnFKUdMcVsHl49228760;     naRrmtYsEnFKUdMcVsHl49228760 = naRrmtYsEnFKUdMcVsHl77864447;     naRrmtYsEnFKUdMcVsHl77864447 = naRrmtYsEnFKUdMcVsHl22688648;     naRrmtYsEnFKUdMcVsHl22688648 = naRrmtYsEnFKUdMcVsHl67062847;     naRrmtYsEnFKUdMcVsHl67062847 = naRrmtYsEnFKUdMcVsHl61355307;     naRrmtYsEnFKUdMcVsHl61355307 = naRrmtYsEnFKUdMcVsHl73258243;     naRrmtYsEnFKUdMcVsHl73258243 = naRrmtYsEnFKUdMcVsHl2603531;     naRrmtYsEnFKUdMcVsHl2603531 = naRrmtYsEnFKUdMcVsHl82482707;     naRrmtYsEnFKUdMcVsHl82482707 = naRrmtYsEnFKUdMcVsHl7179948;     naRrmtYsEnFKUdMcVsHl7179948 = naRrmtYsEnFKUdMcVsHl52140555;     naRrmtYsEnFKUdMcVsHl52140555 = naRrmtYsEnFKUdMcVsHl4625831;     naRrmtYsEnFKUdMcVsHl4625831 = naRrmtYsEnFKUdMcVsHl83961853;     naRrmtYsEnFKUdMcVsHl83961853 = naRrmtYsEnFKUdMcVsHl42740813;     naRrmtYsEnFKUdMcVsHl42740813 = naRrmtYsEnFKUdMcVsHl56740295;     naRrmtYsEnFKUdMcVsHl56740295 = naRrmtYsEnFKUdMcVsHl59440431;     naRrmtYsEnFKUdMcVsHl59440431 = naRrmtYsEnFKUdMcVsHl919009;     naRrmtYsEnFKUdMcVsHl919009 = naRrmtYsEnFKUdMcVsHl2800129;     naRrmtYsEnFKUdMcVsHl2800129 = naRrmtYsEnFKUdMcVsHl43629991;     naRrmtYsEnFKUdMcVsHl43629991 = naRrmtYsEnFKUdMcVsHl52110555;     naRrmtYsEnFKUdMcVsHl52110555 = naRrmtYsEnFKUdMcVsHl63358551;     naRrmtYsEnFKUdMcVsHl63358551 = naRrmtYsEnFKUdMcVsHl40593567;     naRrmtYsEnFKUdMcVsHl40593567 = naRrmtYsEnFKUdMcVsHl71824859;     naRrmtYsEnFKUdMcVsHl71824859 = naRrmtYsEnFKUdMcVsHl43853864;     naRrmtYsEnFKUdMcVsHl43853864 = naRrmtYsEnFKUdMcVsHl92321014;     naRrmtYsEnFKUdMcVsHl92321014 = naRrmtYsEnFKUdMcVsHl68161194;     naRrmtYsEnFKUdMcVsHl68161194 = naRrmtYsEnFKUdMcVsHl60316310;     naRrmtYsEnFKUdMcVsHl60316310 = naRrmtYsEnFKUdMcVsHl90769789;     naRrmtYsEnFKUdMcVsHl90769789 = naRrmtYsEnFKUdMcVsHl98080640;     naRrmtYsEnFKUdMcVsHl98080640 = naRrmtYsEnFKUdMcVsHl96243493;     naRrmtYsEnFKUdMcVsHl96243493 = naRrmtYsEnFKUdMcVsHl57019069;     naRrmtYsEnFKUdMcVsHl57019069 = naRrmtYsEnFKUdMcVsHl18257487;     naRrmtYsEnFKUdMcVsHl18257487 = naRrmtYsEnFKUdMcVsHl79424429;     naRrmtYsEnFKUdMcVsHl79424429 = naRrmtYsEnFKUdMcVsHl91861750;     naRrmtYsEnFKUdMcVsHl91861750 = naRrmtYsEnFKUdMcVsHl37237733;     naRrmtYsEnFKUdMcVsHl37237733 = naRrmtYsEnFKUdMcVsHl78552380;     naRrmtYsEnFKUdMcVsHl78552380 = naRrmtYsEnFKUdMcVsHl71315737;     naRrmtYsEnFKUdMcVsHl71315737 = naRrmtYsEnFKUdMcVsHl8817490;     naRrmtYsEnFKUdMcVsHl8817490 = naRrmtYsEnFKUdMcVsHl33855090;     naRrmtYsEnFKUdMcVsHl33855090 = naRrmtYsEnFKUdMcVsHl47189964;     naRrmtYsEnFKUdMcVsHl47189964 = naRrmtYsEnFKUdMcVsHl84230979;     naRrmtYsEnFKUdMcVsHl84230979 = naRrmtYsEnFKUdMcVsHl95276348;     naRrmtYsEnFKUdMcVsHl95276348 = naRrmtYsEnFKUdMcVsHl42950486;     naRrmtYsEnFKUdMcVsHl42950486 = naRrmtYsEnFKUdMcVsHl53262620;     naRrmtYsEnFKUdMcVsHl53262620 = naRrmtYsEnFKUdMcVsHl93692022;     naRrmtYsEnFKUdMcVsHl93692022 = naRrmtYsEnFKUdMcVsHl20174460;     naRrmtYsEnFKUdMcVsHl20174460 = naRrmtYsEnFKUdMcVsHl49494311;     naRrmtYsEnFKUdMcVsHl49494311 = naRrmtYsEnFKUdMcVsHl10988941;     naRrmtYsEnFKUdMcVsHl10988941 = naRrmtYsEnFKUdMcVsHl81880018;     naRrmtYsEnFKUdMcVsHl81880018 = naRrmtYsEnFKUdMcVsHl68187329;     naRrmtYsEnFKUdMcVsHl68187329 = naRrmtYsEnFKUdMcVsHl42315649;     naRrmtYsEnFKUdMcVsHl42315649 = naRrmtYsEnFKUdMcVsHl16589609;     naRrmtYsEnFKUdMcVsHl16589609 = naRrmtYsEnFKUdMcVsHl18912338;     naRrmtYsEnFKUdMcVsHl18912338 = naRrmtYsEnFKUdMcVsHl37866396;     naRrmtYsEnFKUdMcVsHl37866396 = naRrmtYsEnFKUdMcVsHl71294997;     naRrmtYsEnFKUdMcVsHl71294997 = naRrmtYsEnFKUdMcVsHl39796795;     naRrmtYsEnFKUdMcVsHl39796795 = naRrmtYsEnFKUdMcVsHl77403900;     naRrmtYsEnFKUdMcVsHl77403900 = naRrmtYsEnFKUdMcVsHl34010583;     naRrmtYsEnFKUdMcVsHl34010583 = naRrmtYsEnFKUdMcVsHl30367633;     naRrmtYsEnFKUdMcVsHl30367633 = naRrmtYsEnFKUdMcVsHl98901652;     naRrmtYsEnFKUdMcVsHl98901652 = naRrmtYsEnFKUdMcVsHl1038997;     naRrmtYsEnFKUdMcVsHl1038997 = naRrmtYsEnFKUdMcVsHl82488454;     naRrmtYsEnFKUdMcVsHl82488454 = naRrmtYsEnFKUdMcVsHl4522891;     naRrmtYsEnFKUdMcVsHl4522891 = naRrmtYsEnFKUdMcVsHl86239213;     naRrmtYsEnFKUdMcVsHl86239213 = naRrmtYsEnFKUdMcVsHl50160879;     naRrmtYsEnFKUdMcVsHl50160879 = naRrmtYsEnFKUdMcVsHl33883069;     naRrmtYsEnFKUdMcVsHl33883069 = naRrmtYsEnFKUdMcVsHl25201401;     naRrmtYsEnFKUdMcVsHl25201401 = naRrmtYsEnFKUdMcVsHl92100103;     naRrmtYsEnFKUdMcVsHl92100103 = naRrmtYsEnFKUdMcVsHl5503080;     naRrmtYsEnFKUdMcVsHl5503080 = naRrmtYsEnFKUdMcVsHl78187915;     naRrmtYsEnFKUdMcVsHl78187915 = naRrmtYsEnFKUdMcVsHl88124693;     naRrmtYsEnFKUdMcVsHl88124693 = naRrmtYsEnFKUdMcVsHl92101519;     naRrmtYsEnFKUdMcVsHl92101519 = naRrmtYsEnFKUdMcVsHl68945038;     naRrmtYsEnFKUdMcVsHl68945038 = naRrmtYsEnFKUdMcVsHl96440026;     naRrmtYsEnFKUdMcVsHl96440026 = naRrmtYsEnFKUdMcVsHl67879575;     naRrmtYsEnFKUdMcVsHl67879575 = naRrmtYsEnFKUdMcVsHl68082202;     naRrmtYsEnFKUdMcVsHl68082202 = naRrmtYsEnFKUdMcVsHl97643080;     naRrmtYsEnFKUdMcVsHl97643080 = naRrmtYsEnFKUdMcVsHl18562240;     naRrmtYsEnFKUdMcVsHl18562240 = naRrmtYsEnFKUdMcVsHl50161841;     naRrmtYsEnFKUdMcVsHl50161841 = naRrmtYsEnFKUdMcVsHl72146555;     naRrmtYsEnFKUdMcVsHl72146555 = naRrmtYsEnFKUdMcVsHl27781140;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void FojBeklxvHsqadmQkyoF24286839() {     float YFadUIbbgspoUYBSqNWJ27774668 = -667188428;    float YFadUIbbgspoUYBSqNWJ87752500 = -499994066;    float YFadUIbbgspoUYBSqNWJ32819349 = -859341141;    float YFadUIbbgspoUYBSqNWJ85997797 = -491714406;    float YFadUIbbgspoUYBSqNWJ17385374 = -875149025;    float YFadUIbbgspoUYBSqNWJ92294655 = -254708751;    float YFadUIbbgspoUYBSqNWJ66427216 = -711739834;    float YFadUIbbgspoUYBSqNWJ94647243 = -247038951;    float YFadUIbbgspoUYBSqNWJ43182732 = -487411782;    float YFadUIbbgspoUYBSqNWJ42472091 = -664956555;    float YFadUIbbgspoUYBSqNWJ45120895 = -946838794;    float YFadUIbbgspoUYBSqNWJ47747842 = -325121669;    float YFadUIbbgspoUYBSqNWJ85582667 = -158710050;    float YFadUIbbgspoUYBSqNWJ92464534 = -960665244;    float YFadUIbbgspoUYBSqNWJ72799869 = -455052210;    float YFadUIbbgspoUYBSqNWJ17799205 = -213627875;    float YFadUIbbgspoUYBSqNWJ75242496 = -921326528;    float YFadUIbbgspoUYBSqNWJ85542862 = -773247624;    float YFadUIbbgspoUYBSqNWJ10405221 = -483863111;    float YFadUIbbgspoUYBSqNWJ6727311 = -954125956;    float YFadUIbbgspoUYBSqNWJ60059785 = -765762727;    float YFadUIbbgspoUYBSqNWJ27525026 = -878655603;    float YFadUIbbgspoUYBSqNWJ14742844 = -298613372;    float YFadUIbbgspoUYBSqNWJ66338755 = -840307385;    float YFadUIbbgspoUYBSqNWJ95505536 = -546004863;    float YFadUIbbgspoUYBSqNWJ77929384 = -355850505;    float YFadUIbbgspoUYBSqNWJ43479825 = -776709436;    float YFadUIbbgspoUYBSqNWJ38007056 = -261771565;    float YFadUIbbgspoUYBSqNWJ95161154 = -876215285;    float YFadUIbbgspoUYBSqNWJ83269817 = -640090631;    float YFadUIbbgspoUYBSqNWJ85553233 = -923402382;    float YFadUIbbgspoUYBSqNWJ89349687 = -744563496;    float YFadUIbbgspoUYBSqNWJ69414277 = -279608052;    float YFadUIbbgspoUYBSqNWJ43023650 = -236249879;    float YFadUIbbgspoUYBSqNWJ39098333 = -376731457;    float YFadUIbbgspoUYBSqNWJ2268621 = -510937842;    float YFadUIbbgspoUYBSqNWJ12860377 = -290067854;    float YFadUIbbgspoUYBSqNWJ72717807 = -174337308;    float YFadUIbbgspoUYBSqNWJ78943587 = -257256366;    float YFadUIbbgspoUYBSqNWJ23983448 = -502386083;    float YFadUIbbgspoUYBSqNWJ51214219 = -708026779;    float YFadUIbbgspoUYBSqNWJ26291228 = -273715042;    float YFadUIbbgspoUYBSqNWJ32108220 = -772263406;    float YFadUIbbgspoUYBSqNWJ3085762 = -468280881;    float YFadUIbbgspoUYBSqNWJ76339067 = -625825613;    float YFadUIbbgspoUYBSqNWJ8589163 = -253029736;    float YFadUIbbgspoUYBSqNWJ88014102 = -231112336;    float YFadUIbbgspoUYBSqNWJ66153370 = -134853744;    float YFadUIbbgspoUYBSqNWJ16739030 = -10584210;    float YFadUIbbgspoUYBSqNWJ16288917 = -959517336;    float YFadUIbbgspoUYBSqNWJ70278439 = -827938827;    float YFadUIbbgspoUYBSqNWJ32804744 = -931895858;    float YFadUIbbgspoUYBSqNWJ54872778 = -721988530;    float YFadUIbbgspoUYBSqNWJ2112022 = -538038097;    float YFadUIbbgspoUYBSqNWJ64227053 = 92009181;    float YFadUIbbgspoUYBSqNWJ249643 = -788532825;    float YFadUIbbgspoUYBSqNWJ73009657 = -101380695;    float YFadUIbbgspoUYBSqNWJ66480593 = 80966243;    float YFadUIbbgspoUYBSqNWJ90492261 = -945709543;    float YFadUIbbgspoUYBSqNWJ39455990 = -419298520;    float YFadUIbbgspoUYBSqNWJ48814830 = -477999316;    float YFadUIbbgspoUYBSqNWJ28420160 = -349968269;    float YFadUIbbgspoUYBSqNWJ99486089 = -370823666;    float YFadUIbbgspoUYBSqNWJ59912914 = -847321151;    float YFadUIbbgspoUYBSqNWJ56918857 = -741554173;    float YFadUIbbgspoUYBSqNWJ55771207 = -102275298;    float YFadUIbbgspoUYBSqNWJ78333564 = 54486383;    float YFadUIbbgspoUYBSqNWJ42559018 = -922460171;    float YFadUIbbgspoUYBSqNWJ53366201 = -483933788;    float YFadUIbbgspoUYBSqNWJ70531248 = -944114369;    float YFadUIbbgspoUYBSqNWJ4938829 = -923560021;    float YFadUIbbgspoUYBSqNWJ2524689 = -646989220;    float YFadUIbbgspoUYBSqNWJ6599275 = -415991259;    float YFadUIbbgspoUYBSqNWJ86421773 = -981477028;    float YFadUIbbgspoUYBSqNWJ55513092 = -146099177;    float YFadUIbbgspoUYBSqNWJ33768558 = -392047686;    float YFadUIbbgspoUYBSqNWJ95416805 = -6392198;    float YFadUIbbgspoUYBSqNWJ11657082 = -830332491;    float YFadUIbbgspoUYBSqNWJ89999688 = -114481773;    float YFadUIbbgspoUYBSqNWJ86916374 = -192975127;    float YFadUIbbgspoUYBSqNWJ89915281 = -24738170;    float YFadUIbbgspoUYBSqNWJ77326455 = -541855692;    float YFadUIbbgspoUYBSqNWJ21268026 = -151187355;    float YFadUIbbgspoUYBSqNWJ78872237 = -916697950;    float YFadUIbbgspoUYBSqNWJ12991379 = -812151805;    float YFadUIbbgspoUYBSqNWJ52748490 = -991506525;    float YFadUIbbgspoUYBSqNWJ34476910 = 77425033;    float YFadUIbbgspoUYBSqNWJ67302256 = -741569956;    float YFadUIbbgspoUYBSqNWJ78796597 = -228259060;    float YFadUIbbgspoUYBSqNWJ38848690 = -588198632;    float YFadUIbbgspoUYBSqNWJ29258963 = -309557148;    float YFadUIbbgspoUYBSqNWJ46379783 = -271034098;    float YFadUIbbgspoUYBSqNWJ82225546 = -228627766;    float YFadUIbbgspoUYBSqNWJ39487598 = -837957846;    float YFadUIbbgspoUYBSqNWJ75168617 = 75613232;    float YFadUIbbgspoUYBSqNWJ22794059 = -258058511;    float YFadUIbbgspoUYBSqNWJ26805138 = -902891376;    float YFadUIbbgspoUYBSqNWJ72195305 = -924942255;    float YFadUIbbgspoUYBSqNWJ46166904 = -726726709;    float YFadUIbbgspoUYBSqNWJ20567860 = -667188428;     YFadUIbbgspoUYBSqNWJ27774668 = YFadUIbbgspoUYBSqNWJ87752500;     YFadUIbbgspoUYBSqNWJ87752500 = YFadUIbbgspoUYBSqNWJ32819349;     YFadUIbbgspoUYBSqNWJ32819349 = YFadUIbbgspoUYBSqNWJ85997797;     YFadUIbbgspoUYBSqNWJ85997797 = YFadUIbbgspoUYBSqNWJ17385374;     YFadUIbbgspoUYBSqNWJ17385374 = YFadUIbbgspoUYBSqNWJ92294655;     YFadUIbbgspoUYBSqNWJ92294655 = YFadUIbbgspoUYBSqNWJ66427216;     YFadUIbbgspoUYBSqNWJ66427216 = YFadUIbbgspoUYBSqNWJ94647243;     YFadUIbbgspoUYBSqNWJ94647243 = YFadUIbbgspoUYBSqNWJ43182732;     YFadUIbbgspoUYBSqNWJ43182732 = YFadUIbbgspoUYBSqNWJ42472091;     YFadUIbbgspoUYBSqNWJ42472091 = YFadUIbbgspoUYBSqNWJ45120895;     YFadUIbbgspoUYBSqNWJ45120895 = YFadUIbbgspoUYBSqNWJ47747842;     YFadUIbbgspoUYBSqNWJ47747842 = YFadUIbbgspoUYBSqNWJ85582667;     YFadUIbbgspoUYBSqNWJ85582667 = YFadUIbbgspoUYBSqNWJ92464534;     YFadUIbbgspoUYBSqNWJ92464534 = YFadUIbbgspoUYBSqNWJ72799869;     YFadUIbbgspoUYBSqNWJ72799869 = YFadUIbbgspoUYBSqNWJ17799205;     YFadUIbbgspoUYBSqNWJ17799205 = YFadUIbbgspoUYBSqNWJ75242496;     YFadUIbbgspoUYBSqNWJ75242496 = YFadUIbbgspoUYBSqNWJ85542862;     YFadUIbbgspoUYBSqNWJ85542862 = YFadUIbbgspoUYBSqNWJ10405221;     YFadUIbbgspoUYBSqNWJ10405221 = YFadUIbbgspoUYBSqNWJ6727311;     YFadUIbbgspoUYBSqNWJ6727311 = YFadUIbbgspoUYBSqNWJ60059785;     YFadUIbbgspoUYBSqNWJ60059785 = YFadUIbbgspoUYBSqNWJ27525026;     YFadUIbbgspoUYBSqNWJ27525026 = YFadUIbbgspoUYBSqNWJ14742844;     YFadUIbbgspoUYBSqNWJ14742844 = YFadUIbbgspoUYBSqNWJ66338755;     YFadUIbbgspoUYBSqNWJ66338755 = YFadUIbbgspoUYBSqNWJ95505536;     YFadUIbbgspoUYBSqNWJ95505536 = YFadUIbbgspoUYBSqNWJ77929384;     YFadUIbbgspoUYBSqNWJ77929384 = YFadUIbbgspoUYBSqNWJ43479825;     YFadUIbbgspoUYBSqNWJ43479825 = YFadUIbbgspoUYBSqNWJ38007056;     YFadUIbbgspoUYBSqNWJ38007056 = YFadUIbbgspoUYBSqNWJ95161154;     YFadUIbbgspoUYBSqNWJ95161154 = YFadUIbbgspoUYBSqNWJ83269817;     YFadUIbbgspoUYBSqNWJ83269817 = YFadUIbbgspoUYBSqNWJ85553233;     YFadUIbbgspoUYBSqNWJ85553233 = YFadUIbbgspoUYBSqNWJ89349687;     YFadUIbbgspoUYBSqNWJ89349687 = YFadUIbbgspoUYBSqNWJ69414277;     YFadUIbbgspoUYBSqNWJ69414277 = YFadUIbbgspoUYBSqNWJ43023650;     YFadUIbbgspoUYBSqNWJ43023650 = YFadUIbbgspoUYBSqNWJ39098333;     YFadUIbbgspoUYBSqNWJ39098333 = YFadUIbbgspoUYBSqNWJ2268621;     YFadUIbbgspoUYBSqNWJ2268621 = YFadUIbbgspoUYBSqNWJ12860377;     YFadUIbbgspoUYBSqNWJ12860377 = YFadUIbbgspoUYBSqNWJ72717807;     YFadUIbbgspoUYBSqNWJ72717807 = YFadUIbbgspoUYBSqNWJ78943587;     YFadUIbbgspoUYBSqNWJ78943587 = YFadUIbbgspoUYBSqNWJ23983448;     YFadUIbbgspoUYBSqNWJ23983448 = YFadUIbbgspoUYBSqNWJ51214219;     YFadUIbbgspoUYBSqNWJ51214219 = YFadUIbbgspoUYBSqNWJ26291228;     YFadUIbbgspoUYBSqNWJ26291228 = YFadUIbbgspoUYBSqNWJ32108220;     YFadUIbbgspoUYBSqNWJ32108220 = YFadUIbbgspoUYBSqNWJ3085762;     YFadUIbbgspoUYBSqNWJ3085762 = YFadUIbbgspoUYBSqNWJ76339067;     YFadUIbbgspoUYBSqNWJ76339067 = YFadUIbbgspoUYBSqNWJ8589163;     YFadUIbbgspoUYBSqNWJ8589163 = YFadUIbbgspoUYBSqNWJ88014102;     YFadUIbbgspoUYBSqNWJ88014102 = YFadUIbbgspoUYBSqNWJ66153370;     YFadUIbbgspoUYBSqNWJ66153370 = YFadUIbbgspoUYBSqNWJ16739030;     YFadUIbbgspoUYBSqNWJ16739030 = YFadUIbbgspoUYBSqNWJ16288917;     YFadUIbbgspoUYBSqNWJ16288917 = YFadUIbbgspoUYBSqNWJ70278439;     YFadUIbbgspoUYBSqNWJ70278439 = YFadUIbbgspoUYBSqNWJ32804744;     YFadUIbbgspoUYBSqNWJ32804744 = YFadUIbbgspoUYBSqNWJ54872778;     YFadUIbbgspoUYBSqNWJ54872778 = YFadUIbbgspoUYBSqNWJ2112022;     YFadUIbbgspoUYBSqNWJ2112022 = YFadUIbbgspoUYBSqNWJ64227053;     YFadUIbbgspoUYBSqNWJ64227053 = YFadUIbbgspoUYBSqNWJ249643;     YFadUIbbgspoUYBSqNWJ249643 = YFadUIbbgspoUYBSqNWJ73009657;     YFadUIbbgspoUYBSqNWJ73009657 = YFadUIbbgspoUYBSqNWJ66480593;     YFadUIbbgspoUYBSqNWJ66480593 = YFadUIbbgspoUYBSqNWJ90492261;     YFadUIbbgspoUYBSqNWJ90492261 = YFadUIbbgspoUYBSqNWJ39455990;     YFadUIbbgspoUYBSqNWJ39455990 = YFadUIbbgspoUYBSqNWJ48814830;     YFadUIbbgspoUYBSqNWJ48814830 = YFadUIbbgspoUYBSqNWJ28420160;     YFadUIbbgspoUYBSqNWJ28420160 = YFadUIbbgspoUYBSqNWJ99486089;     YFadUIbbgspoUYBSqNWJ99486089 = YFadUIbbgspoUYBSqNWJ59912914;     YFadUIbbgspoUYBSqNWJ59912914 = YFadUIbbgspoUYBSqNWJ56918857;     YFadUIbbgspoUYBSqNWJ56918857 = YFadUIbbgspoUYBSqNWJ55771207;     YFadUIbbgspoUYBSqNWJ55771207 = YFadUIbbgspoUYBSqNWJ78333564;     YFadUIbbgspoUYBSqNWJ78333564 = YFadUIbbgspoUYBSqNWJ42559018;     YFadUIbbgspoUYBSqNWJ42559018 = YFadUIbbgspoUYBSqNWJ53366201;     YFadUIbbgspoUYBSqNWJ53366201 = YFadUIbbgspoUYBSqNWJ70531248;     YFadUIbbgspoUYBSqNWJ70531248 = YFadUIbbgspoUYBSqNWJ4938829;     YFadUIbbgspoUYBSqNWJ4938829 = YFadUIbbgspoUYBSqNWJ2524689;     YFadUIbbgspoUYBSqNWJ2524689 = YFadUIbbgspoUYBSqNWJ6599275;     YFadUIbbgspoUYBSqNWJ6599275 = YFadUIbbgspoUYBSqNWJ86421773;     YFadUIbbgspoUYBSqNWJ86421773 = YFadUIbbgspoUYBSqNWJ55513092;     YFadUIbbgspoUYBSqNWJ55513092 = YFadUIbbgspoUYBSqNWJ33768558;     YFadUIbbgspoUYBSqNWJ33768558 = YFadUIbbgspoUYBSqNWJ95416805;     YFadUIbbgspoUYBSqNWJ95416805 = YFadUIbbgspoUYBSqNWJ11657082;     YFadUIbbgspoUYBSqNWJ11657082 = YFadUIbbgspoUYBSqNWJ89999688;     YFadUIbbgspoUYBSqNWJ89999688 = YFadUIbbgspoUYBSqNWJ86916374;     YFadUIbbgspoUYBSqNWJ86916374 = YFadUIbbgspoUYBSqNWJ89915281;     YFadUIbbgspoUYBSqNWJ89915281 = YFadUIbbgspoUYBSqNWJ77326455;     YFadUIbbgspoUYBSqNWJ77326455 = YFadUIbbgspoUYBSqNWJ21268026;     YFadUIbbgspoUYBSqNWJ21268026 = YFadUIbbgspoUYBSqNWJ78872237;     YFadUIbbgspoUYBSqNWJ78872237 = YFadUIbbgspoUYBSqNWJ12991379;     YFadUIbbgspoUYBSqNWJ12991379 = YFadUIbbgspoUYBSqNWJ52748490;     YFadUIbbgspoUYBSqNWJ52748490 = YFadUIbbgspoUYBSqNWJ34476910;     YFadUIbbgspoUYBSqNWJ34476910 = YFadUIbbgspoUYBSqNWJ67302256;     YFadUIbbgspoUYBSqNWJ67302256 = YFadUIbbgspoUYBSqNWJ78796597;     YFadUIbbgspoUYBSqNWJ78796597 = YFadUIbbgspoUYBSqNWJ38848690;     YFadUIbbgspoUYBSqNWJ38848690 = YFadUIbbgspoUYBSqNWJ29258963;     YFadUIbbgspoUYBSqNWJ29258963 = YFadUIbbgspoUYBSqNWJ46379783;     YFadUIbbgspoUYBSqNWJ46379783 = YFadUIbbgspoUYBSqNWJ82225546;     YFadUIbbgspoUYBSqNWJ82225546 = YFadUIbbgspoUYBSqNWJ39487598;     YFadUIbbgspoUYBSqNWJ39487598 = YFadUIbbgspoUYBSqNWJ75168617;     YFadUIbbgspoUYBSqNWJ75168617 = YFadUIbbgspoUYBSqNWJ22794059;     YFadUIbbgspoUYBSqNWJ22794059 = YFadUIbbgspoUYBSqNWJ26805138;     YFadUIbbgspoUYBSqNWJ26805138 = YFadUIbbgspoUYBSqNWJ72195305;     YFadUIbbgspoUYBSqNWJ72195305 = YFadUIbbgspoUYBSqNWJ46166904;     YFadUIbbgspoUYBSqNWJ46166904 = YFadUIbbgspoUYBSqNWJ20567860;     YFadUIbbgspoUYBSqNWJ20567860 = YFadUIbbgspoUYBSqNWJ27774668;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kCixItLKlORjfCcXZcys78703051() {     float xOLuldPvHqpjxBSaUAPS98426335 = -185337558;    float xOLuldPvHqpjxBSaUAPS56426018 = -371637365;    float xOLuldPvHqpjxBSaUAPS12788268 = -201635601;    float xOLuldPvHqpjxBSaUAPS25929084 = -681852795;    float xOLuldPvHqpjxBSaUAPS38835695 = -658555372;    float xOLuldPvHqpjxBSaUAPS82294417 = 43710097;    float xOLuldPvHqpjxBSaUAPS84806111 = -710515543;    float xOLuldPvHqpjxBSaUAPS51489624 = -593856041;    float xOLuldPvHqpjxBSaUAPS89047786 = -580759111;    float xOLuldPvHqpjxBSaUAPS65438552 = -966051905;    float xOLuldPvHqpjxBSaUAPS87370055 = -981740724;    float xOLuldPvHqpjxBSaUAPS93294412 = -640885462;    float xOLuldPvHqpjxBSaUAPS65410054 = -68247108;    float xOLuldPvHqpjxBSaUAPS76311567 = -761693454;    float xOLuldPvHqpjxBSaUAPS55519888 = -604802214;    float xOLuldPvHqpjxBSaUAPS50327845 = -883611462;    float xOLuldPvHqpjxBSaUAPS69095323 = -830511658;    float xOLuldPvHqpjxBSaUAPS5836357 = -762413820;    float xOLuldPvHqpjxBSaUAPS92113002 = -91051053;    float xOLuldPvHqpjxBSaUAPS65514020 = -396492261;    float xOLuldPvHqpjxBSaUAPS15195540 = -674441868;    float xOLuldPvHqpjxBSaUAPS88920790 = -109940909;    float xOLuldPvHqpjxBSaUAPS16686652 = -866456516;    float xOLuldPvHqpjxBSaUAPS188293 = -432708291;    float xOLuldPvHqpjxBSaUAPS59629925 = -221909551;    float xOLuldPvHqpjxBSaUAPS8526965 = -454134950;    float xOLuldPvHqpjxBSaUAPS51740611 = -71887211;    float xOLuldPvHqpjxBSaUAPS45789281 = -766895332;    float xOLuldPvHqpjxBSaUAPS27303642 = -70573502;    float xOLuldPvHqpjxBSaUAPS22441914 = -98485702;    float xOLuldPvHqpjxBSaUAPS92625230 = -671836068;    float xOLuldPvHqpjxBSaUAPS33861148 = 27139341;    float xOLuldPvHqpjxBSaUAPS52359638 = -226569661;    float xOLuldPvHqpjxBSaUAPS8388625 = -899779563;    float xOLuldPvHqpjxBSaUAPS17122570 = -223678237;    float xOLuldPvHqpjxBSaUAPS62261879 = -590164390;    float xOLuldPvHqpjxBSaUAPS56651862 = -478661062;    float xOLuldPvHqpjxBSaUAPS25510297 = 41549584;    float xOLuldPvHqpjxBSaUAPS57476876 = -212985003;    float xOLuldPvHqpjxBSaUAPS19086746 = -19994446;    float xOLuldPvHqpjxBSaUAPS11287503 = 7954342;    float xOLuldPvHqpjxBSaUAPS54774219 = -43104111;    float xOLuldPvHqpjxBSaUAPS81075179 = -759017458;    float xOLuldPvHqpjxBSaUAPS9594063 = -540255853;    float xOLuldPvHqpjxBSaUAPS72293125 = -934472007;    float xOLuldPvHqpjxBSaUAPS14994997 = -11366891;    float xOLuldPvHqpjxBSaUAPS14145306 = -347182135;    float xOLuldPvHqpjxBSaUAPS31767580 = -586984039;    float xOLuldPvHqpjxBSaUAPS22180482 = 3651821;    float xOLuldPvHqpjxBSaUAPS24716404 = -142526215;    float xOLuldPvHqpjxBSaUAPS38467763 = -374510948;    float xOLuldPvHqpjxBSaUAPS74676248 = -901115580;    float xOLuldPvHqpjxBSaUAPS9948435 = -649786572;    float xOLuldPvHqpjxBSaUAPS92339325 = -98919593;    float xOLuldPvHqpjxBSaUAPS63507512 = -18931734;    float xOLuldPvHqpjxBSaUAPS9505545 = 24603350;    float xOLuldPvHqpjxBSaUAPS39739366 = -505180849;    float xOLuldPvHqpjxBSaUAPS12599976 = -768927310;    float xOLuldPvHqpjxBSaUAPS66299159 = -359943245;    float xOLuldPvHqpjxBSaUAPS30308731 = -104420423;    float xOLuldPvHqpjxBSaUAPS30553807 = -884402692;    float xOLuldPvHqpjxBSaUAPS39016830 = -943620212;    float xOLuldPvHqpjxBSaUAPS24185983 = -423282539;    float xOLuldPvHqpjxBSaUAPS66605873 = -382273409;    float xOLuldPvHqpjxBSaUAPS72813322 = -194215838;    float xOLuldPvHqpjxBSaUAPS53508907 = -908880065;    float xOLuldPvHqpjxBSaUAPS40934775 = -314315801;    float xOLuldPvHqpjxBSaUAPS57021430 = -168467545;    float xOLuldPvHqpjxBSaUAPS59188998 = -438015218;    float xOLuldPvHqpjxBSaUAPS93258008 = 85362175;    float xOLuldPvHqpjxBSaUAPS93675982 = -304950400;    float xOLuldPvHqpjxBSaUAPS43585027 = -772061243;    float xOLuldPvHqpjxBSaUAPS48359480 = -449428818;    float xOLuldPvHqpjxBSaUAPS73026256 = 28943392;    float xOLuldPvHqpjxBSaUAPS54226518 = -304446603;    float xOLuldPvHqpjxBSaUAPS60421321 = -531337757;    float xOLuldPvHqpjxBSaUAPS7845611 = -350923451;    float xOLuldPvHqpjxBSaUAPS7092590 = -226200664;    float xOLuldPvHqpjxBSaUAPS27895167 = -498236284;    float xOLuldPvHqpjxBSaUAPS44634928 = -110542660;    float xOLuldPvHqpjxBSaUAPS94381658 = -6952815;    float xOLuldPvHqpjxBSaUAPS19973031 = -484903172;    float xOLuldPvHqpjxBSaUAPS23608799 = -670547154;    float xOLuldPvHqpjxBSaUAPS2587239 = -928047288;    float xOLuldPvHqpjxBSaUAPS83974150 = -723974754;    float xOLuldPvHqpjxBSaUAPS17948982 = -770720488;    float xOLuldPvHqpjxBSaUAPS23912714 = -323074087;    float xOLuldPvHqpjxBSaUAPS60020312 = -27650068;    float xOLuldPvHqpjxBSaUAPS44881113 = -780847830;    float xOLuldPvHqpjxBSaUAPS7617025 = -148281587;    float xOLuldPvHqpjxBSaUAPS22522514 = 15016459;    float xOLuldPvHqpjxBSaUAPS44051887 = -709733752;    float xOLuldPvHqpjxBSaUAPS59211137 = -598507172;    float xOLuldPvHqpjxBSaUAPS27168146 = -8564581;    float xOLuldPvHqpjxBSaUAPS88532938 = -135591754;    float xOLuldPvHqpjxBSaUAPS72270672 = -48425447;    float xOLuldPvHqpjxBSaUAPS30588237 = -619821572;    float xOLuldPvHqpjxBSaUAPS14469307 = -276744049;    float xOLuldPvHqpjxBSaUAPS36780741 = -246040016;    float xOLuldPvHqpjxBSaUAPS18784218 = -185337558;     xOLuldPvHqpjxBSaUAPS98426335 = xOLuldPvHqpjxBSaUAPS56426018;     xOLuldPvHqpjxBSaUAPS56426018 = xOLuldPvHqpjxBSaUAPS12788268;     xOLuldPvHqpjxBSaUAPS12788268 = xOLuldPvHqpjxBSaUAPS25929084;     xOLuldPvHqpjxBSaUAPS25929084 = xOLuldPvHqpjxBSaUAPS38835695;     xOLuldPvHqpjxBSaUAPS38835695 = xOLuldPvHqpjxBSaUAPS82294417;     xOLuldPvHqpjxBSaUAPS82294417 = xOLuldPvHqpjxBSaUAPS84806111;     xOLuldPvHqpjxBSaUAPS84806111 = xOLuldPvHqpjxBSaUAPS51489624;     xOLuldPvHqpjxBSaUAPS51489624 = xOLuldPvHqpjxBSaUAPS89047786;     xOLuldPvHqpjxBSaUAPS89047786 = xOLuldPvHqpjxBSaUAPS65438552;     xOLuldPvHqpjxBSaUAPS65438552 = xOLuldPvHqpjxBSaUAPS87370055;     xOLuldPvHqpjxBSaUAPS87370055 = xOLuldPvHqpjxBSaUAPS93294412;     xOLuldPvHqpjxBSaUAPS93294412 = xOLuldPvHqpjxBSaUAPS65410054;     xOLuldPvHqpjxBSaUAPS65410054 = xOLuldPvHqpjxBSaUAPS76311567;     xOLuldPvHqpjxBSaUAPS76311567 = xOLuldPvHqpjxBSaUAPS55519888;     xOLuldPvHqpjxBSaUAPS55519888 = xOLuldPvHqpjxBSaUAPS50327845;     xOLuldPvHqpjxBSaUAPS50327845 = xOLuldPvHqpjxBSaUAPS69095323;     xOLuldPvHqpjxBSaUAPS69095323 = xOLuldPvHqpjxBSaUAPS5836357;     xOLuldPvHqpjxBSaUAPS5836357 = xOLuldPvHqpjxBSaUAPS92113002;     xOLuldPvHqpjxBSaUAPS92113002 = xOLuldPvHqpjxBSaUAPS65514020;     xOLuldPvHqpjxBSaUAPS65514020 = xOLuldPvHqpjxBSaUAPS15195540;     xOLuldPvHqpjxBSaUAPS15195540 = xOLuldPvHqpjxBSaUAPS88920790;     xOLuldPvHqpjxBSaUAPS88920790 = xOLuldPvHqpjxBSaUAPS16686652;     xOLuldPvHqpjxBSaUAPS16686652 = xOLuldPvHqpjxBSaUAPS188293;     xOLuldPvHqpjxBSaUAPS188293 = xOLuldPvHqpjxBSaUAPS59629925;     xOLuldPvHqpjxBSaUAPS59629925 = xOLuldPvHqpjxBSaUAPS8526965;     xOLuldPvHqpjxBSaUAPS8526965 = xOLuldPvHqpjxBSaUAPS51740611;     xOLuldPvHqpjxBSaUAPS51740611 = xOLuldPvHqpjxBSaUAPS45789281;     xOLuldPvHqpjxBSaUAPS45789281 = xOLuldPvHqpjxBSaUAPS27303642;     xOLuldPvHqpjxBSaUAPS27303642 = xOLuldPvHqpjxBSaUAPS22441914;     xOLuldPvHqpjxBSaUAPS22441914 = xOLuldPvHqpjxBSaUAPS92625230;     xOLuldPvHqpjxBSaUAPS92625230 = xOLuldPvHqpjxBSaUAPS33861148;     xOLuldPvHqpjxBSaUAPS33861148 = xOLuldPvHqpjxBSaUAPS52359638;     xOLuldPvHqpjxBSaUAPS52359638 = xOLuldPvHqpjxBSaUAPS8388625;     xOLuldPvHqpjxBSaUAPS8388625 = xOLuldPvHqpjxBSaUAPS17122570;     xOLuldPvHqpjxBSaUAPS17122570 = xOLuldPvHqpjxBSaUAPS62261879;     xOLuldPvHqpjxBSaUAPS62261879 = xOLuldPvHqpjxBSaUAPS56651862;     xOLuldPvHqpjxBSaUAPS56651862 = xOLuldPvHqpjxBSaUAPS25510297;     xOLuldPvHqpjxBSaUAPS25510297 = xOLuldPvHqpjxBSaUAPS57476876;     xOLuldPvHqpjxBSaUAPS57476876 = xOLuldPvHqpjxBSaUAPS19086746;     xOLuldPvHqpjxBSaUAPS19086746 = xOLuldPvHqpjxBSaUAPS11287503;     xOLuldPvHqpjxBSaUAPS11287503 = xOLuldPvHqpjxBSaUAPS54774219;     xOLuldPvHqpjxBSaUAPS54774219 = xOLuldPvHqpjxBSaUAPS81075179;     xOLuldPvHqpjxBSaUAPS81075179 = xOLuldPvHqpjxBSaUAPS9594063;     xOLuldPvHqpjxBSaUAPS9594063 = xOLuldPvHqpjxBSaUAPS72293125;     xOLuldPvHqpjxBSaUAPS72293125 = xOLuldPvHqpjxBSaUAPS14994997;     xOLuldPvHqpjxBSaUAPS14994997 = xOLuldPvHqpjxBSaUAPS14145306;     xOLuldPvHqpjxBSaUAPS14145306 = xOLuldPvHqpjxBSaUAPS31767580;     xOLuldPvHqpjxBSaUAPS31767580 = xOLuldPvHqpjxBSaUAPS22180482;     xOLuldPvHqpjxBSaUAPS22180482 = xOLuldPvHqpjxBSaUAPS24716404;     xOLuldPvHqpjxBSaUAPS24716404 = xOLuldPvHqpjxBSaUAPS38467763;     xOLuldPvHqpjxBSaUAPS38467763 = xOLuldPvHqpjxBSaUAPS74676248;     xOLuldPvHqpjxBSaUAPS74676248 = xOLuldPvHqpjxBSaUAPS9948435;     xOLuldPvHqpjxBSaUAPS9948435 = xOLuldPvHqpjxBSaUAPS92339325;     xOLuldPvHqpjxBSaUAPS92339325 = xOLuldPvHqpjxBSaUAPS63507512;     xOLuldPvHqpjxBSaUAPS63507512 = xOLuldPvHqpjxBSaUAPS9505545;     xOLuldPvHqpjxBSaUAPS9505545 = xOLuldPvHqpjxBSaUAPS39739366;     xOLuldPvHqpjxBSaUAPS39739366 = xOLuldPvHqpjxBSaUAPS12599976;     xOLuldPvHqpjxBSaUAPS12599976 = xOLuldPvHqpjxBSaUAPS66299159;     xOLuldPvHqpjxBSaUAPS66299159 = xOLuldPvHqpjxBSaUAPS30308731;     xOLuldPvHqpjxBSaUAPS30308731 = xOLuldPvHqpjxBSaUAPS30553807;     xOLuldPvHqpjxBSaUAPS30553807 = xOLuldPvHqpjxBSaUAPS39016830;     xOLuldPvHqpjxBSaUAPS39016830 = xOLuldPvHqpjxBSaUAPS24185983;     xOLuldPvHqpjxBSaUAPS24185983 = xOLuldPvHqpjxBSaUAPS66605873;     xOLuldPvHqpjxBSaUAPS66605873 = xOLuldPvHqpjxBSaUAPS72813322;     xOLuldPvHqpjxBSaUAPS72813322 = xOLuldPvHqpjxBSaUAPS53508907;     xOLuldPvHqpjxBSaUAPS53508907 = xOLuldPvHqpjxBSaUAPS40934775;     xOLuldPvHqpjxBSaUAPS40934775 = xOLuldPvHqpjxBSaUAPS57021430;     xOLuldPvHqpjxBSaUAPS57021430 = xOLuldPvHqpjxBSaUAPS59188998;     xOLuldPvHqpjxBSaUAPS59188998 = xOLuldPvHqpjxBSaUAPS93258008;     xOLuldPvHqpjxBSaUAPS93258008 = xOLuldPvHqpjxBSaUAPS93675982;     xOLuldPvHqpjxBSaUAPS93675982 = xOLuldPvHqpjxBSaUAPS43585027;     xOLuldPvHqpjxBSaUAPS43585027 = xOLuldPvHqpjxBSaUAPS48359480;     xOLuldPvHqpjxBSaUAPS48359480 = xOLuldPvHqpjxBSaUAPS73026256;     xOLuldPvHqpjxBSaUAPS73026256 = xOLuldPvHqpjxBSaUAPS54226518;     xOLuldPvHqpjxBSaUAPS54226518 = xOLuldPvHqpjxBSaUAPS60421321;     xOLuldPvHqpjxBSaUAPS60421321 = xOLuldPvHqpjxBSaUAPS7845611;     xOLuldPvHqpjxBSaUAPS7845611 = xOLuldPvHqpjxBSaUAPS7092590;     xOLuldPvHqpjxBSaUAPS7092590 = xOLuldPvHqpjxBSaUAPS27895167;     xOLuldPvHqpjxBSaUAPS27895167 = xOLuldPvHqpjxBSaUAPS44634928;     xOLuldPvHqpjxBSaUAPS44634928 = xOLuldPvHqpjxBSaUAPS94381658;     xOLuldPvHqpjxBSaUAPS94381658 = xOLuldPvHqpjxBSaUAPS19973031;     xOLuldPvHqpjxBSaUAPS19973031 = xOLuldPvHqpjxBSaUAPS23608799;     xOLuldPvHqpjxBSaUAPS23608799 = xOLuldPvHqpjxBSaUAPS2587239;     xOLuldPvHqpjxBSaUAPS2587239 = xOLuldPvHqpjxBSaUAPS83974150;     xOLuldPvHqpjxBSaUAPS83974150 = xOLuldPvHqpjxBSaUAPS17948982;     xOLuldPvHqpjxBSaUAPS17948982 = xOLuldPvHqpjxBSaUAPS23912714;     xOLuldPvHqpjxBSaUAPS23912714 = xOLuldPvHqpjxBSaUAPS60020312;     xOLuldPvHqpjxBSaUAPS60020312 = xOLuldPvHqpjxBSaUAPS44881113;     xOLuldPvHqpjxBSaUAPS44881113 = xOLuldPvHqpjxBSaUAPS7617025;     xOLuldPvHqpjxBSaUAPS7617025 = xOLuldPvHqpjxBSaUAPS22522514;     xOLuldPvHqpjxBSaUAPS22522514 = xOLuldPvHqpjxBSaUAPS44051887;     xOLuldPvHqpjxBSaUAPS44051887 = xOLuldPvHqpjxBSaUAPS59211137;     xOLuldPvHqpjxBSaUAPS59211137 = xOLuldPvHqpjxBSaUAPS27168146;     xOLuldPvHqpjxBSaUAPS27168146 = xOLuldPvHqpjxBSaUAPS88532938;     xOLuldPvHqpjxBSaUAPS88532938 = xOLuldPvHqpjxBSaUAPS72270672;     xOLuldPvHqpjxBSaUAPS72270672 = xOLuldPvHqpjxBSaUAPS30588237;     xOLuldPvHqpjxBSaUAPS30588237 = xOLuldPvHqpjxBSaUAPS14469307;     xOLuldPvHqpjxBSaUAPS14469307 = xOLuldPvHqpjxBSaUAPS36780741;     xOLuldPvHqpjxBSaUAPS36780741 = xOLuldPvHqpjxBSaUAPS18784218;     xOLuldPvHqpjxBSaUAPS18784218 = xOLuldPvHqpjxBSaUAPS98426335;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MIyLGuYqeAAMXHHAJhKe85361796() {     float fgyDuTqVqGsFCVNDXuuS98419863 = -57861700;    float fgyDuTqVqGsFCVNDXuuS94998334 = -414232490;    float fgyDuTqVqGsFCVNDXuuS14101480 = -158764215;    float fgyDuTqVqGsFCVNDXuuS11008945 = -502393625;    float fgyDuTqVqGsFCVNDXuuS47675797 = -26910693;    float fgyDuTqVqGsFCVNDXuuS17099850 = -755722714;    float fgyDuTqVqGsFCVNDXuuS53353447 = -415052010;    float fgyDuTqVqGsFCVNDXuuS20703675 = -298033358;    float fgyDuTqVqGsFCVNDXuuS71787950 = -294405180;    float fgyDuTqVqGsFCVNDXuuS62078066 = -602858776;    float fgyDuTqVqGsFCVNDXuuS7690661 = -941212796;    float fgyDuTqVqGsFCVNDXuuS7586090 = -376363824;    float fgyDuTqVqGsFCVNDXuuS97262968 = -89905582;    float fgyDuTqVqGsFCVNDXuuS30155788 = -6846506;    float fgyDuTqVqGsFCVNDXuuS691998 = -567515066;    float fgyDuTqVqGsFCVNDXuuS24892393 = -116407112;    float fgyDuTqVqGsFCVNDXuuS24948083 = -650126228;    float fgyDuTqVqGsFCVNDXuuS28836889 = -59485442;    float fgyDuTqVqGsFCVNDXuuS12541272 = -709033320;    float fgyDuTqVqGsFCVNDXuuS37587783 = 91483288;    float fgyDuTqVqGsFCVNDXuuS94864964 = -384908241;    float fgyDuTqVqGsFCVNDXuuS67217056 = -619932792;    float fgyDuTqVqGsFCVNDXuuS53565049 = -225414358;    float fgyDuTqVqGsFCVNDXuuS43838401 = -313628814;    float fgyDuTqVqGsFCVNDXuuS88072614 = -648603404;    float fgyDuTqVqGsFCVNDXuuS25101042 = -362340878;    float fgyDuTqVqGsFCVNDXuuS21962193 = -372686924;    float fgyDuTqVqGsFCVNDXuuS81192806 = -275862321;    float fgyDuTqVqGsFCVNDXuuS39982089 = -716643817;    float fgyDuTqVqGsFCVNDXuuS98531783 = -873917076;    float fgyDuTqVqGsFCVNDXuuS26037909 = 41070698;    float fgyDuTqVqGsFCVNDXuuS18585006 = -364682421;    float fgyDuTqVqGsFCVNDXuuS37812061 = -428513877;    float fgyDuTqVqGsFCVNDXuuS8671462 = -776461296;    float fgyDuTqVqGsFCVNDXuuS99480607 = -482992161;    float fgyDuTqVqGsFCVNDXuuS5090069 = -669930136;    float fgyDuTqVqGsFCVNDXuuS68593230 = -578314440;    float fgyDuTqVqGsFCVNDXuuS95427975 = 46286905;    float fgyDuTqVqGsFCVNDXuuS92790473 = -925666419;    float fgyDuTqVqGsFCVNDXuuS90959638 = -729426132;    float fgyDuTqVqGsFCVNDXuuS99143170 = -79989556;    float fgyDuTqVqGsFCVNDXuuS40471880 = -236896193;    float fgyDuTqVqGsFCVNDXuuS41358540 = -218147712;    float fgyDuTqVqGsFCVNDXuuS68825960 = -97220296;    float fgyDuTqVqGsFCVNDXuuS56311178 = -307731409;    float fgyDuTqVqGsFCVNDXuuS55422965 = -923411562;    float fgyDuTqVqGsFCVNDXuuS41843098 = -828462758;    float fgyDuTqVqGsFCVNDXuuS7151161 = -95745395;    float fgyDuTqVqGsFCVNDXuuS40838872 = -660112040;    float fgyDuTqVqGsFCVNDXuuS44761826 = -829551101;    float fgyDuTqVqGsFCVNDXuuS51727133 = 46934044;    float fgyDuTqVqGsFCVNDXuuS89223506 = -183261328;    float fgyDuTqVqGsFCVNDXuuS85396783 = -541840581;    float fgyDuTqVqGsFCVNDXuuS2589597 = -630839445;    float fgyDuTqVqGsFCVNDXuuS90496832 = -875831011;    float fgyDuTqVqGsFCVNDXuuS31202808 = -437928908;    float fgyDuTqVqGsFCVNDXuuS41433286 = -88818132;    float fgyDuTqVqGsFCVNDXuuS70263079 = -845135401;    float fgyDuTqVqGsFCVNDXuuS22936331 = -853790222;    float fgyDuTqVqGsFCVNDXuuS22574756 = -664569816;    float fgyDuTqVqGsFCVNDXuuS95137657 = -283035791;    float fgyDuTqVqGsFCVNDXuuS72160641 = -39189690;    float fgyDuTqVqGsFCVNDXuuS80721586 = -581389542;    float fgyDuTqVqGsFCVNDXuuS73256167 = -420488105;    float fgyDuTqVqGsFCVNDXuuS36040157 = -543929474;    float fgyDuTqVqGsFCVNDXuuS89105655 = -476530375;    float fgyDuTqVqGsFCVNDXuuS69774029 = -947849948;    float fgyDuTqVqGsFCVNDXuuS88591506 = -313444287;    float fgyDuTqVqGsFCVNDXuuS30675181 = -523854345;    float fgyDuTqVqGsFCVNDXuuS95601928 = -897584930;    float fgyDuTqVqGsFCVNDXuuS56299162 = -538092673;    float fgyDuTqVqGsFCVNDXuuS29520107 = -596413133;    float fgyDuTqVqGsFCVNDXuuS36046416 = -133819024;    float fgyDuTqVqGsFCVNDXuuS21581634 = -979607188;    float fgyDuTqVqGsFCVNDXuuS38444613 = -828527156;    float fgyDuTqVqGsFCVNDXuuS54393084 = -48012048;    float fgyDuTqVqGsFCVNDXuuS25858517 = -301785080;    float fgyDuTqVqGsFCVNDXuuS84739088 = -28194062;    float fgyDuTqVqGsFCVNDXuuS87527223 = 94102595;    float fgyDuTqVqGsFCVNDXuuS32649649 = -725191842;    float fgyDuTqVqGsFCVNDXuuS83257943 = -533878121;    float fgyDuTqVqGsFCVNDXuuS14811032 = -176941529;    float fgyDuTqVqGsFCVNDXuuS40353935 = -615750281;    float fgyDuTqVqGsFCVNDXuuS95220262 = -887092717;    float fgyDuTqVqGsFCVNDXuuS46804650 = -820851121;    float fgyDuTqVqGsFCVNDXuuS36814403 = -775667975;    float fgyDuTqVqGsFCVNDXuuS33188223 = -822841841;    float fgyDuTqVqGsFCVNDXuuS35222465 = -797674433;    float fgyDuTqVqGsFCVNDXuuS18174630 = -900630285;    float fgyDuTqVqGsFCVNDXuuS68277800 = 54936746;    float fgyDuTqVqGsFCVNDXuuS63656783 = -481112004;    float fgyDuTqVqGsFCVNDXuuS98330151 = -733179039;    float fgyDuTqVqGsFCVNDXuuS72491645 = -99922874;    float fgyDuTqVqGsFCVNDXuuS70215718 = -161096604;    float fgyDuTqVqGsFCVNDXuuS95821980 = -346390341;    float fgyDuTqVqGsFCVNDXuuS26982529 = 59200134;    float fgyDuTqVqGsFCVNDXuuS59750294 = -655506652;    float fgyDuTqVqGsFCVNDXuuS68102373 = -797659607;    float fgyDuTqVqGsFCVNDXuuS32785804 = -553290823;    float fgyDuTqVqGsFCVNDXuuS67205522 = -57861700;     fgyDuTqVqGsFCVNDXuuS98419863 = fgyDuTqVqGsFCVNDXuuS94998334;     fgyDuTqVqGsFCVNDXuuS94998334 = fgyDuTqVqGsFCVNDXuuS14101480;     fgyDuTqVqGsFCVNDXuuS14101480 = fgyDuTqVqGsFCVNDXuuS11008945;     fgyDuTqVqGsFCVNDXuuS11008945 = fgyDuTqVqGsFCVNDXuuS47675797;     fgyDuTqVqGsFCVNDXuuS47675797 = fgyDuTqVqGsFCVNDXuuS17099850;     fgyDuTqVqGsFCVNDXuuS17099850 = fgyDuTqVqGsFCVNDXuuS53353447;     fgyDuTqVqGsFCVNDXuuS53353447 = fgyDuTqVqGsFCVNDXuuS20703675;     fgyDuTqVqGsFCVNDXuuS20703675 = fgyDuTqVqGsFCVNDXuuS71787950;     fgyDuTqVqGsFCVNDXuuS71787950 = fgyDuTqVqGsFCVNDXuuS62078066;     fgyDuTqVqGsFCVNDXuuS62078066 = fgyDuTqVqGsFCVNDXuuS7690661;     fgyDuTqVqGsFCVNDXuuS7690661 = fgyDuTqVqGsFCVNDXuuS7586090;     fgyDuTqVqGsFCVNDXuuS7586090 = fgyDuTqVqGsFCVNDXuuS97262968;     fgyDuTqVqGsFCVNDXuuS97262968 = fgyDuTqVqGsFCVNDXuuS30155788;     fgyDuTqVqGsFCVNDXuuS30155788 = fgyDuTqVqGsFCVNDXuuS691998;     fgyDuTqVqGsFCVNDXuuS691998 = fgyDuTqVqGsFCVNDXuuS24892393;     fgyDuTqVqGsFCVNDXuuS24892393 = fgyDuTqVqGsFCVNDXuuS24948083;     fgyDuTqVqGsFCVNDXuuS24948083 = fgyDuTqVqGsFCVNDXuuS28836889;     fgyDuTqVqGsFCVNDXuuS28836889 = fgyDuTqVqGsFCVNDXuuS12541272;     fgyDuTqVqGsFCVNDXuuS12541272 = fgyDuTqVqGsFCVNDXuuS37587783;     fgyDuTqVqGsFCVNDXuuS37587783 = fgyDuTqVqGsFCVNDXuuS94864964;     fgyDuTqVqGsFCVNDXuuS94864964 = fgyDuTqVqGsFCVNDXuuS67217056;     fgyDuTqVqGsFCVNDXuuS67217056 = fgyDuTqVqGsFCVNDXuuS53565049;     fgyDuTqVqGsFCVNDXuuS53565049 = fgyDuTqVqGsFCVNDXuuS43838401;     fgyDuTqVqGsFCVNDXuuS43838401 = fgyDuTqVqGsFCVNDXuuS88072614;     fgyDuTqVqGsFCVNDXuuS88072614 = fgyDuTqVqGsFCVNDXuuS25101042;     fgyDuTqVqGsFCVNDXuuS25101042 = fgyDuTqVqGsFCVNDXuuS21962193;     fgyDuTqVqGsFCVNDXuuS21962193 = fgyDuTqVqGsFCVNDXuuS81192806;     fgyDuTqVqGsFCVNDXuuS81192806 = fgyDuTqVqGsFCVNDXuuS39982089;     fgyDuTqVqGsFCVNDXuuS39982089 = fgyDuTqVqGsFCVNDXuuS98531783;     fgyDuTqVqGsFCVNDXuuS98531783 = fgyDuTqVqGsFCVNDXuuS26037909;     fgyDuTqVqGsFCVNDXuuS26037909 = fgyDuTqVqGsFCVNDXuuS18585006;     fgyDuTqVqGsFCVNDXuuS18585006 = fgyDuTqVqGsFCVNDXuuS37812061;     fgyDuTqVqGsFCVNDXuuS37812061 = fgyDuTqVqGsFCVNDXuuS8671462;     fgyDuTqVqGsFCVNDXuuS8671462 = fgyDuTqVqGsFCVNDXuuS99480607;     fgyDuTqVqGsFCVNDXuuS99480607 = fgyDuTqVqGsFCVNDXuuS5090069;     fgyDuTqVqGsFCVNDXuuS5090069 = fgyDuTqVqGsFCVNDXuuS68593230;     fgyDuTqVqGsFCVNDXuuS68593230 = fgyDuTqVqGsFCVNDXuuS95427975;     fgyDuTqVqGsFCVNDXuuS95427975 = fgyDuTqVqGsFCVNDXuuS92790473;     fgyDuTqVqGsFCVNDXuuS92790473 = fgyDuTqVqGsFCVNDXuuS90959638;     fgyDuTqVqGsFCVNDXuuS90959638 = fgyDuTqVqGsFCVNDXuuS99143170;     fgyDuTqVqGsFCVNDXuuS99143170 = fgyDuTqVqGsFCVNDXuuS40471880;     fgyDuTqVqGsFCVNDXuuS40471880 = fgyDuTqVqGsFCVNDXuuS41358540;     fgyDuTqVqGsFCVNDXuuS41358540 = fgyDuTqVqGsFCVNDXuuS68825960;     fgyDuTqVqGsFCVNDXuuS68825960 = fgyDuTqVqGsFCVNDXuuS56311178;     fgyDuTqVqGsFCVNDXuuS56311178 = fgyDuTqVqGsFCVNDXuuS55422965;     fgyDuTqVqGsFCVNDXuuS55422965 = fgyDuTqVqGsFCVNDXuuS41843098;     fgyDuTqVqGsFCVNDXuuS41843098 = fgyDuTqVqGsFCVNDXuuS7151161;     fgyDuTqVqGsFCVNDXuuS7151161 = fgyDuTqVqGsFCVNDXuuS40838872;     fgyDuTqVqGsFCVNDXuuS40838872 = fgyDuTqVqGsFCVNDXuuS44761826;     fgyDuTqVqGsFCVNDXuuS44761826 = fgyDuTqVqGsFCVNDXuuS51727133;     fgyDuTqVqGsFCVNDXuuS51727133 = fgyDuTqVqGsFCVNDXuuS89223506;     fgyDuTqVqGsFCVNDXuuS89223506 = fgyDuTqVqGsFCVNDXuuS85396783;     fgyDuTqVqGsFCVNDXuuS85396783 = fgyDuTqVqGsFCVNDXuuS2589597;     fgyDuTqVqGsFCVNDXuuS2589597 = fgyDuTqVqGsFCVNDXuuS90496832;     fgyDuTqVqGsFCVNDXuuS90496832 = fgyDuTqVqGsFCVNDXuuS31202808;     fgyDuTqVqGsFCVNDXuuS31202808 = fgyDuTqVqGsFCVNDXuuS41433286;     fgyDuTqVqGsFCVNDXuuS41433286 = fgyDuTqVqGsFCVNDXuuS70263079;     fgyDuTqVqGsFCVNDXuuS70263079 = fgyDuTqVqGsFCVNDXuuS22936331;     fgyDuTqVqGsFCVNDXuuS22936331 = fgyDuTqVqGsFCVNDXuuS22574756;     fgyDuTqVqGsFCVNDXuuS22574756 = fgyDuTqVqGsFCVNDXuuS95137657;     fgyDuTqVqGsFCVNDXuuS95137657 = fgyDuTqVqGsFCVNDXuuS72160641;     fgyDuTqVqGsFCVNDXuuS72160641 = fgyDuTqVqGsFCVNDXuuS80721586;     fgyDuTqVqGsFCVNDXuuS80721586 = fgyDuTqVqGsFCVNDXuuS73256167;     fgyDuTqVqGsFCVNDXuuS73256167 = fgyDuTqVqGsFCVNDXuuS36040157;     fgyDuTqVqGsFCVNDXuuS36040157 = fgyDuTqVqGsFCVNDXuuS89105655;     fgyDuTqVqGsFCVNDXuuS89105655 = fgyDuTqVqGsFCVNDXuuS69774029;     fgyDuTqVqGsFCVNDXuuS69774029 = fgyDuTqVqGsFCVNDXuuS88591506;     fgyDuTqVqGsFCVNDXuuS88591506 = fgyDuTqVqGsFCVNDXuuS30675181;     fgyDuTqVqGsFCVNDXuuS30675181 = fgyDuTqVqGsFCVNDXuuS95601928;     fgyDuTqVqGsFCVNDXuuS95601928 = fgyDuTqVqGsFCVNDXuuS56299162;     fgyDuTqVqGsFCVNDXuuS56299162 = fgyDuTqVqGsFCVNDXuuS29520107;     fgyDuTqVqGsFCVNDXuuS29520107 = fgyDuTqVqGsFCVNDXuuS36046416;     fgyDuTqVqGsFCVNDXuuS36046416 = fgyDuTqVqGsFCVNDXuuS21581634;     fgyDuTqVqGsFCVNDXuuS21581634 = fgyDuTqVqGsFCVNDXuuS38444613;     fgyDuTqVqGsFCVNDXuuS38444613 = fgyDuTqVqGsFCVNDXuuS54393084;     fgyDuTqVqGsFCVNDXuuS54393084 = fgyDuTqVqGsFCVNDXuuS25858517;     fgyDuTqVqGsFCVNDXuuS25858517 = fgyDuTqVqGsFCVNDXuuS84739088;     fgyDuTqVqGsFCVNDXuuS84739088 = fgyDuTqVqGsFCVNDXuuS87527223;     fgyDuTqVqGsFCVNDXuuS87527223 = fgyDuTqVqGsFCVNDXuuS32649649;     fgyDuTqVqGsFCVNDXuuS32649649 = fgyDuTqVqGsFCVNDXuuS83257943;     fgyDuTqVqGsFCVNDXuuS83257943 = fgyDuTqVqGsFCVNDXuuS14811032;     fgyDuTqVqGsFCVNDXuuS14811032 = fgyDuTqVqGsFCVNDXuuS40353935;     fgyDuTqVqGsFCVNDXuuS40353935 = fgyDuTqVqGsFCVNDXuuS95220262;     fgyDuTqVqGsFCVNDXuuS95220262 = fgyDuTqVqGsFCVNDXuuS46804650;     fgyDuTqVqGsFCVNDXuuS46804650 = fgyDuTqVqGsFCVNDXuuS36814403;     fgyDuTqVqGsFCVNDXuuS36814403 = fgyDuTqVqGsFCVNDXuuS33188223;     fgyDuTqVqGsFCVNDXuuS33188223 = fgyDuTqVqGsFCVNDXuuS35222465;     fgyDuTqVqGsFCVNDXuuS35222465 = fgyDuTqVqGsFCVNDXuuS18174630;     fgyDuTqVqGsFCVNDXuuS18174630 = fgyDuTqVqGsFCVNDXuuS68277800;     fgyDuTqVqGsFCVNDXuuS68277800 = fgyDuTqVqGsFCVNDXuuS63656783;     fgyDuTqVqGsFCVNDXuuS63656783 = fgyDuTqVqGsFCVNDXuuS98330151;     fgyDuTqVqGsFCVNDXuuS98330151 = fgyDuTqVqGsFCVNDXuuS72491645;     fgyDuTqVqGsFCVNDXuuS72491645 = fgyDuTqVqGsFCVNDXuuS70215718;     fgyDuTqVqGsFCVNDXuuS70215718 = fgyDuTqVqGsFCVNDXuuS95821980;     fgyDuTqVqGsFCVNDXuuS95821980 = fgyDuTqVqGsFCVNDXuuS26982529;     fgyDuTqVqGsFCVNDXuuS26982529 = fgyDuTqVqGsFCVNDXuuS59750294;     fgyDuTqVqGsFCVNDXuuS59750294 = fgyDuTqVqGsFCVNDXuuS68102373;     fgyDuTqVqGsFCVNDXuuS68102373 = fgyDuTqVqGsFCVNDXuuS32785804;     fgyDuTqVqGsFCVNDXuuS32785804 = fgyDuTqVqGsFCVNDXuuS67205522;     fgyDuTqVqGsFCVNDXuuS67205522 = fgyDuTqVqGsFCVNDXuuS98419863;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ApsdApBcLfRHynHBAPca39778010() {     float clZFEqwUggqpsahDnidT69071531 = -676010830;    float clZFEqwUggqpsahDnidT63671852 = -285875789;    float clZFEqwUggqpsahDnidT94070399 = -601058674;    float clZFEqwUggqpsahDnidT50940231 = -692532014;    float clZFEqwUggqpsahDnidT69126118 = -910317041;    float clZFEqwUggqpsahDnidT7099613 = -457303866;    float clZFEqwUggqpsahDnidT71732343 = -413827720;    float clZFEqwUggqpsahDnidT77546055 = -644850448;    float clZFEqwUggqpsahDnidT17653005 = -387752509;    float clZFEqwUggqpsahDnidT85044526 = -903954127;    float clZFEqwUggqpsahDnidT49939822 = -976114725;    float clZFEqwUggqpsahDnidT53132661 = -692127617;    float clZFEqwUggqpsahDnidT77090355 = 557359;    float clZFEqwUggqpsahDnidT14002822 = -907874716;    float clZFEqwUggqpsahDnidT83412016 = -717265069;    float clZFEqwUggqpsahDnidT57421032 = -786390699;    float clZFEqwUggqpsahDnidT18800910 = -559311358;    float clZFEqwUggqpsahDnidT49130383 = -48651638;    float clZFEqwUggqpsahDnidT94249053 = -316221262;    float clZFEqwUggqpsahDnidT96374492 = -450883017;    float clZFEqwUggqpsahDnidT50000719 = -293587382;    float clZFEqwUggqpsahDnidT28612821 = -951218097;    float clZFEqwUggqpsahDnidT55508857 = -793257503;    float clZFEqwUggqpsahDnidT77687938 = 93970280;    float clZFEqwUggqpsahDnidT52197003 = -324508092;    float clZFEqwUggqpsahDnidT55698622 = -460625323;    float clZFEqwUggqpsahDnidT30222978 = -767864699;    float clZFEqwUggqpsahDnidT88975031 = -780986088;    float clZFEqwUggqpsahDnidT72124577 = 88997966;    float clZFEqwUggqpsahDnidT37703879 = -332312146;    float clZFEqwUggqpsahDnidT33109906 = -807362988;    float clZFEqwUggqpsahDnidT63096466 = -692979584;    float clZFEqwUggqpsahDnidT20757422 = -375475486;    float clZFEqwUggqpsahDnidT74036437 = -339990980;    float clZFEqwUggqpsahDnidT77504844 = -329938941;    float clZFEqwUggqpsahDnidT65083328 = -749156684;    float clZFEqwUggqpsahDnidT12384716 = -766907648;    float clZFEqwUggqpsahDnidT48220464 = -837826202;    float clZFEqwUggqpsahDnidT71323762 = -881395056;    float clZFEqwUggqpsahDnidT86062936 = -247034495;    float clZFEqwUggqpsahDnidT59216454 = -464008435;    float clZFEqwUggqpsahDnidT68954871 = -6285263;    float clZFEqwUggqpsahDnidT90325499 = -204901764;    float clZFEqwUggqpsahDnidT75334261 = -169195268;    float clZFEqwUggqpsahDnidT52265237 = -616377803;    float clZFEqwUggqpsahDnidT61828800 = -681748717;    float clZFEqwUggqpsahDnidT67974301 = -944532557;    float clZFEqwUggqpsahDnidT72765371 = -547875691;    float clZFEqwUggqpsahDnidT46280324 = -645876010;    float clZFEqwUggqpsahDnidT53189313 = -12559980;    float clZFEqwUggqpsahDnidT19916458 = -599638077;    float clZFEqwUggqpsahDnidT31095011 = -152481050;    float clZFEqwUggqpsahDnidT40472440 = -469638623;    float clZFEqwUggqpsahDnidT92816901 = -191720941;    float clZFEqwUggqpsahDnidT89777290 = -986771926;    float clZFEqwUggqpsahDnidT40458710 = -724792733;    float clZFEqwUggqpsahDnidT8162995 = -492618287;    float clZFEqwUggqpsahDnidT16382462 = -595028955;    float clZFEqwUggqpsahDnidT98743228 = -268023923;    float clZFEqwUggqpsahDnidT13427497 = -349691718;    float clZFEqwUggqpsahDnidT76876634 = -689439167;    float clZFEqwUggqpsahDnidT82757311 = -632841633;    float clZFEqwUggqpsahDnidT5421479 = -633848415;    float clZFEqwUggqpsahDnidT79949125 = 44559637;    float clZFEqwUggqpsahDnidT51934621 = 3408861;    float clZFEqwUggqpsahDnidT86843355 = -183135142;    float clZFEqwUggqpsahDnidT32375239 = -216652132;    float clZFEqwUggqpsahDnidT3053919 = -659451661;    float clZFEqwUggqpsahDnidT36497978 = -477935776;    float clZFEqwUggqpsahDnidT18328689 = -968108386;    float clZFEqwUggqpsahDnidT45036317 = 80516948;    float clZFEqwUggqpsahDnidT70580445 = -721485156;    float clZFEqwUggqpsahDnidT77806621 = -167256583;    float clZFEqwUggqpsahDnidT8186118 = 30813232;    float clZFEqwUggqpsahDnidT37158038 = -986874583;    float clZFEqwUggqpsahDnidT81045847 = -187302119;    float clZFEqwUggqpsahDnidT38287321 = -646316334;    float clZFEqwUggqpsahDnidT80174596 = -524062236;    float clZFEqwUggqpsahDnidT25422701 = -289651917;    float clZFEqwUggqpsahDnidT90368202 = -642759375;    float clZFEqwUggqpsahDnidT87724320 = -516092767;    float clZFEqwUggqpsahDnidT57457607 = -119989009;    float clZFEqwUggqpsahDnidT42694708 = -35110079;    float clZFEqwUggqpsahDnidT18935264 = -898442055;    float clZFEqwUggqpsahDnidT17787422 = -732674070;    float clZFEqwUggqpsahDnidT2014896 = -554881938;    float clZFEqwUggqpsahDnidT22624027 = -123340962;    float clZFEqwUggqpsahDnidT27940521 = -83754545;    float clZFEqwUggqpsahDnidT84259146 = -353219055;    float clZFEqwUggqpsahDnidT37046134 = -605146208;    float clZFEqwUggqpsahDnidT56920333 = -156538397;    float clZFEqwUggqpsahDnidT96002254 = -71878693;    float clZFEqwUggqpsahDnidT49477236 = -469802280;    float clZFEqwUggqpsahDnidT57896266 = -431703338;    float clZFEqwUggqpsahDnidT9186302 = -557595328;    float clZFEqwUggqpsahDnidT76459142 = -831166802;    float clZFEqwUggqpsahDnidT63533392 = -372436849;    float clZFEqwUggqpsahDnidT10376375 = -149461401;    float clZFEqwUggqpsahDnidT23399641 = -72604129;    float clZFEqwUggqpsahDnidT65421881 = -676010830;     clZFEqwUggqpsahDnidT69071531 = clZFEqwUggqpsahDnidT63671852;     clZFEqwUggqpsahDnidT63671852 = clZFEqwUggqpsahDnidT94070399;     clZFEqwUggqpsahDnidT94070399 = clZFEqwUggqpsahDnidT50940231;     clZFEqwUggqpsahDnidT50940231 = clZFEqwUggqpsahDnidT69126118;     clZFEqwUggqpsahDnidT69126118 = clZFEqwUggqpsahDnidT7099613;     clZFEqwUggqpsahDnidT7099613 = clZFEqwUggqpsahDnidT71732343;     clZFEqwUggqpsahDnidT71732343 = clZFEqwUggqpsahDnidT77546055;     clZFEqwUggqpsahDnidT77546055 = clZFEqwUggqpsahDnidT17653005;     clZFEqwUggqpsahDnidT17653005 = clZFEqwUggqpsahDnidT85044526;     clZFEqwUggqpsahDnidT85044526 = clZFEqwUggqpsahDnidT49939822;     clZFEqwUggqpsahDnidT49939822 = clZFEqwUggqpsahDnidT53132661;     clZFEqwUggqpsahDnidT53132661 = clZFEqwUggqpsahDnidT77090355;     clZFEqwUggqpsahDnidT77090355 = clZFEqwUggqpsahDnidT14002822;     clZFEqwUggqpsahDnidT14002822 = clZFEqwUggqpsahDnidT83412016;     clZFEqwUggqpsahDnidT83412016 = clZFEqwUggqpsahDnidT57421032;     clZFEqwUggqpsahDnidT57421032 = clZFEqwUggqpsahDnidT18800910;     clZFEqwUggqpsahDnidT18800910 = clZFEqwUggqpsahDnidT49130383;     clZFEqwUggqpsahDnidT49130383 = clZFEqwUggqpsahDnidT94249053;     clZFEqwUggqpsahDnidT94249053 = clZFEqwUggqpsahDnidT96374492;     clZFEqwUggqpsahDnidT96374492 = clZFEqwUggqpsahDnidT50000719;     clZFEqwUggqpsahDnidT50000719 = clZFEqwUggqpsahDnidT28612821;     clZFEqwUggqpsahDnidT28612821 = clZFEqwUggqpsahDnidT55508857;     clZFEqwUggqpsahDnidT55508857 = clZFEqwUggqpsahDnidT77687938;     clZFEqwUggqpsahDnidT77687938 = clZFEqwUggqpsahDnidT52197003;     clZFEqwUggqpsahDnidT52197003 = clZFEqwUggqpsahDnidT55698622;     clZFEqwUggqpsahDnidT55698622 = clZFEqwUggqpsahDnidT30222978;     clZFEqwUggqpsahDnidT30222978 = clZFEqwUggqpsahDnidT88975031;     clZFEqwUggqpsahDnidT88975031 = clZFEqwUggqpsahDnidT72124577;     clZFEqwUggqpsahDnidT72124577 = clZFEqwUggqpsahDnidT37703879;     clZFEqwUggqpsahDnidT37703879 = clZFEqwUggqpsahDnidT33109906;     clZFEqwUggqpsahDnidT33109906 = clZFEqwUggqpsahDnidT63096466;     clZFEqwUggqpsahDnidT63096466 = clZFEqwUggqpsahDnidT20757422;     clZFEqwUggqpsahDnidT20757422 = clZFEqwUggqpsahDnidT74036437;     clZFEqwUggqpsahDnidT74036437 = clZFEqwUggqpsahDnidT77504844;     clZFEqwUggqpsahDnidT77504844 = clZFEqwUggqpsahDnidT65083328;     clZFEqwUggqpsahDnidT65083328 = clZFEqwUggqpsahDnidT12384716;     clZFEqwUggqpsahDnidT12384716 = clZFEqwUggqpsahDnidT48220464;     clZFEqwUggqpsahDnidT48220464 = clZFEqwUggqpsahDnidT71323762;     clZFEqwUggqpsahDnidT71323762 = clZFEqwUggqpsahDnidT86062936;     clZFEqwUggqpsahDnidT86062936 = clZFEqwUggqpsahDnidT59216454;     clZFEqwUggqpsahDnidT59216454 = clZFEqwUggqpsahDnidT68954871;     clZFEqwUggqpsahDnidT68954871 = clZFEqwUggqpsahDnidT90325499;     clZFEqwUggqpsahDnidT90325499 = clZFEqwUggqpsahDnidT75334261;     clZFEqwUggqpsahDnidT75334261 = clZFEqwUggqpsahDnidT52265237;     clZFEqwUggqpsahDnidT52265237 = clZFEqwUggqpsahDnidT61828800;     clZFEqwUggqpsahDnidT61828800 = clZFEqwUggqpsahDnidT67974301;     clZFEqwUggqpsahDnidT67974301 = clZFEqwUggqpsahDnidT72765371;     clZFEqwUggqpsahDnidT72765371 = clZFEqwUggqpsahDnidT46280324;     clZFEqwUggqpsahDnidT46280324 = clZFEqwUggqpsahDnidT53189313;     clZFEqwUggqpsahDnidT53189313 = clZFEqwUggqpsahDnidT19916458;     clZFEqwUggqpsahDnidT19916458 = clZFEqwUggqpsahDnidT31095011;     clZFEqwUggqpsahDnidT31095011 = clZFEqwUggqpsahDnidT40472440;     clZFEqwUggqpsahDnidT40472440 = clZFEqwUggqpsahDnidT92816901;     clZFEqwUggqpsahDnidT92816901 = clZFEqwUggqpsahDnidT89777290;     clZFEqwUggqpsahDnidT89777290 = clZFEqwUggqpsahDnidT40458710;     clZFEqwUggqpsahDnidT40458710 = clZFEqwUggqpsahDnidT8162995;     clZFEqwUggqpsahDnidT8162995 = clZFEqwUggqpsahDnidT16382462;     clZFEqwUggqpsahDnidT16382462 = clZFEqwUggqpsahDnidT98743228;     clZFEqwUggqpsahDnidT98743228 = clZFEqwUggqpsahDnidT13427497;     clZFEqwUggqpsahDnidT13427497 = clZFEqwUggqpsahDnidT76876634;     clZFEqwUggqpsahDnidT76876634 = clZFEqwUggqpsahDnidT82757311;     clZFEqwUggqpsahDnidT82757311 = clZFEqwUggqpsahDnidT5421479;     clZFEqwUggqpsahDnidT5421479 = clZFEqwUggqpsahDnidT79949125;     clZFEqwUggqpsahDnidT79949125 = clZFEqwUggqpsahDnidT51934621;     clZFEqwUggqpsahDnidT51934621 = clZFEqwUggqpsahDnidT86843355;     clZFEqwUggqpsahDnidT86843355 = clZFEqwUggqpsahDnidT32375239;     clZFEqwUggqpsahDnidT32375239 = clZFEqwUggqpsahDnidT3053919;     clZFEqwUggqpsahDnidT3053919 = clZFEqwUggqpsahDnidT36497978;     clZFEqwUggqpsahDnidT36497978 = clZFEqwUggqpsahDnidT18328689;     clZFEqwUggqpsahDnidT18328689 = clZFEqwUggqpsahDnidT45036317;     clZFEqwUggqpsahDnidT45036317 = clZFEqwUggqpsahDnidT70580445;     clZFEqwUggqpsahDnidT70580445 = clZFEqwUggqpsahDnidT77806621;     clZFEqwUggqpsahDnidT77806621 = clZFEqwUggqpsahDnidT8186118;     clZFEqwUggqpsahDnidT8186118 = clZFEqwUggqpsahDnidT37158038;     clZFEqwUggqpsahDnidT37158038 = clZFEqwUggqpsahDnidT81045847;     clZFEqwUggqpsahDnidT81045847 = clZFEqwUggqpsahDnidT38287321;     clZFEqwUggqpsahDnidT38287321 = clZFEqwUggqpsahDnidT80174596;     clZFEqwUggqpsahDnidT80174596 = clZFEqwUggqpsahDnidT25422701;     clZFEqwUggqpsahDnidT25422701 = clZFEqwUggqpsahDnidT90368202;     clZFEqwUggqpsahDnidT90368202 = clZFEqwUggqpsahDnidT87724320;     clZFEqwUggqpsahDnidT87724320 = clZFEqwUggqpsahDnidT57457607;     clZFEqwUggqpsahDnidT57457607 = clZFEqwUggqpsahDnidT42694708;     clZFEqwUggqpsahDnidT42694708 = clZFEqwUggqpsahDnidT18935264;     clZFEqwUggqpsahDnidT18935264 = clZFEqwUggqpsahDnidT17787422;     clZFEqwUggqpsahDnidT17787422 = clZFEqwUggqpsahDnidT2014896;     clZFEqwUggqpsahDnidT2014896 = clZFEqwUggqpsahDnidT22624027;     clZFEqwUggqpsahDnidT22624027 = clZFEqwUggqpsahDnidT27940521;     clZFEqwUggqpsahDnidT27940521 = clZFEqwUggqpsahDnidT84259146;     clZFEqwUggqpsahDnidT84259146 = clZFEqwUggqpsahDnidT37046134;     clZFEqwUggqpsahDnidT37046134 = clZFEqwUggqpsahDnidT56920333;     clZFEqwUggqpsahDnidT56920333 = clZFEqwUggqpsahDnidT96002254;     clZFEqwUggqpsahDnidT96002254 = clZFEqwUggqpsahDnidT49477236;     clZFEqwUggqpsahDnidT49477236 = clZFEqwUggqpsahDnidT57896266;     clZFEqwUggqpsahDnidT57896266 = clZFEqwUggqpsahDnidT9186302;     clZFEqwUggqpsahDnidT9186302 = clZFEqwUggqpsahDnidT76459142;     clZFEqwUggqpsahDnidT76459142 = clZFEqwUggqpsahDnidT63533392;     clZFEqwUggqpsahDnidT63533392 = clZFEqwUggqpsahDnidT10376375;     clZFEqwUggqpsahDnidT10376375 = clZFEqwUggqpsahDnidT23399641;     clZFEqwUggqpsahDnidT23399641 = clZFEqwUggqpsahDnidT65421881;     clZFEqwUggqpsahDnidT65421881 = clZFEqwUggqpsahDnidT69071531;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZHyptWWEbnfUOBjaQWNQ46436754() {     float ZXnBBxMPECHTyNVAIFQh69065059 = -548534971;    float ZXnBBxMPECHTyNVAIFQh2244169 = -328470914;    float ZXnBBxMPECHTyNVAIFQh95383611 = -558187288;    float ZXnBBxMPECHTyNVAIFQh36020092 = -513072844;    float ZXnBBxMPECHTyNVAIFQh77966220 = -278672362;    float ZXnBBxMPECHTyNVAIFQh41905045 = -156736678;    float ZXnBBxMPECHTyNVAIFQh40279679 = -118364187;    float ZXnBBxMPECHTyNVAIFQh46760106 = -349027766;    float ZXnBBxMPECHTyNVAIFQh393169 = -101398579;    float ZXnBBxMPECHTyNVAIFQh81684040 = -540760998;    float ZXnBBxMPECHTyNVAIFQh70260427 = -935586797;    float ZXnBBxMPECHTyNVAIFQh67424338 = -427605980;    float ZXnBBxMPECHTyNVAIFQh8943270 = -21101115;    float ZXnBBxMPECHTyNVAIFQh67847041 = -153027767;    float ZXnBBxMPECHTyNVAIFQh28584126 = -679977921;    float ZXnBBxMPECHTyNVAIFQh31985580 = -19186350;    float ZXnBBxMPECHTyNVAIFQh74653669 = -378925928;    float ZXnBBxMPECHTyNVAIFQh72130916 = -445723260;    float ZXnBBxMPECHTyNVAIFQh14677324 = -934203528;    float ZXnBBxMPECHTyNVAIFQh68448255 = 37092532;    float ZXnBBxMPECHTyNVAIFQh29670143 = -4053755;    float ZXnBBxMPECHTyNVAIFQh6909087 = -361209981;    float ZXnBBxMPECHTyNVAIFQh92387253 = -152215345;    float ZXnBBxMPECHTyNVAIFQh21338046 = -886950242;    float ZXnBBxMPECHTyNVAIFQh80639692 = -751201945;    float ZXnBBxMPECHTyNVAIFQh72272699 = -368831251;    float ZXnBBxMPECHTyNVAIFQh444560 = 31335588;    float ZXnBBxMPECHTyNVAIFQh24378557 = -289953076;    float ZXnBBxMPECHTyNVAIFQh84803023 = -557072349;    float ZXnBBxMPECHTyNVAIFQh13793750 = -7743520;    float ZXnBBxMPECHTyNVAIFQh66522584 = -94456222;    float ZXnBBxMPECHTyNVAIFQh47820324 = 15198655;    float ZXnBBxMPECHTyNVAIFQh6209845 = -577419702;    float ZXnBBxMPECHTyNVAIFQh74319274 = -216672713;    float ZXnBBxMPECHTyNVAIFQh59862882 = -589252866;    float ZXnBBxMPECHTyNVAIFQh7911518 = -828922430;    float ZXnBBxMPECHTyNVAIFQh24326084 = -866561026;    float ZXnBBxMPECHTyNVAIFQh18138144 = -833088882;    float ZXnBBxMPECHTyNVAIFQh6637359 = -494076472;    float ZXnBBxMPECHTyNVAIFQh57935829 = -956466181;    float ZXnBBxMPECHTyNVAIFQh47072122 = -551952333;    float ZXnBBxMPECHTyNVAIFQh54652532 = -200077345;    float ZXnBBxMPECHTyNVAIFQh50608860 = -764032018;    float ZXnBBxMPECHTyNVAIFQh34566160 = -826159711;    float ZXnBBxMPECHTyNVAIFQh36283289 = 10362795;    float ZXnBBxMPECHTyNVAIFQh2256769 = -493793388;    float ZXnBBxMPECHTyNVAIFQh95672093 = -325813179;    float ZXnBBxMPECHTyNVAIFQh48148952 = -56637047;    float ZXnBBxMPECHTyNVAIFQh64938713 = -209639871;    float ZXnBBxMPECHTyNVAIFQh73234736 = -699584866;    float ZXnBBxMPECHTyNVAIFQh33175827 = -178193084;    float ZXnBBxMPECHTyNVAIFQh45642268 = -534626798;    float ZXnBBxMPECHTyNVAIFQh15920789 = -361692631;    float ZXnBBxMPECHTyNVAIFQh3067172 = -723640793;    float ZXnBBxMPECHTyNVAIFQh16766612 = -743671203;    float ZXnBBxMPECHTyNVAIFQh62155973 = -87324991;    float ZXnBBxMPECHTyNVAIFQh9856915 = -76255570;    float ZXnBBxMPECHTyNVAIFQh74045565 = -671237046;    float ZXnBBxMPECHTyNVAIFQh55380400 = -761870900;    float ZXnBBxMPECHTyNVAIFQh5693522 = -909841111;    float ZXnBBxMPECHTyNVAIFQh41460485 = -88072266;    float ZXnBBxMPECHTyNVAIFQh15901123 = -828411111;    float ZXnBBxMPECHTyNVAIFQh61957082 = -791955417;    float ZXnBBxMPECHTyNVAIFQh86599419 = 6344941;    float ZXnBBxMPECHTyNVAIFQh15161456 = -346304776;    float ZXnBBxMPECHTyNVAIFQh22440103 = -850785452;    float ZXnBBxMPECHTyNVAIFQh61214493 = -850186279;    float ZXnBBxMPECHTyNVAIFQh34623995 = -804428403;    float ZXnBBxMPECHTyNVAIFQh7984160 = -563774902;    float ZXnBBxMPECHTyNVAIFQh20672608 = -851055491;    float ZXnBBxMPECHTyNVAIFQh7659496 = -152625324;    float ZXnBBxMPECHTyNVAIFQh56515525 = -545837047;    float ZXnBBxMPECHTyNVAIFQh65493557 = -951646789;    float ZXnBBxMPECHTyNVAIFQh56741494 = -977737348;    float ZXnBBxMPECHTyNVAIFQh21376134 = -410955136;    float ZXnBBxMPECHTyNVAIFQh75017611 = -803976411;    float ZXnBBxMPECHTyNVAIFQh56300227 = -597177963;    float ZXnBBxMPECHTyNVAIFQh57821094 = -326055634;    float ZXnBBxMPECHTyNVAIFQh85054757 = -797313038;    float ZXnBBxMPECHTyNVAIFQh78382924 = -157408558;    float ZXnBBxMPECHTyNVAIFQh76600605 = 56981927;    float ZXnBBxMPECHTyNVAIFQh52295608 = -912027366;    float ZXnBBxMPECHTyNVAIFQh59439843 = 19686794;    float ZXnBBxMPECHTyNVAIFQh11568288 = -857487484;    float ZXnBBxMPECHTyNVAIFQh80617922 = -829550437;    float ZXnBBxMPECHTyNVAIFQh20880317 = -559829425;    float ZXnBBxMPECHTyNVAIFQh31899536 = -623108715;    float ZXnBBxMPECHTyNVAIFQh3142674 = -853778910;    float ZXnBBxMPECHTyNVAIFQh57552663 = -473001510;    float ZXnBBxMPECHTyNVAIFQh97706909 = -401927875;    float ZXnBBxMPECHTyNVAIFQh98054603 = -652666860;    float ZXnBBxMPECHTyNVAIFQh50280519 = -95323980;    float ZXnBBxMPECHTyNVAIFQh62757744 = 28782018;    float ZXnBBxMPECHTyNVAIFQh943838 = -584235361;    float ZXnBBxMPECHTyNVAIFQh16475344 = -768393915;    float ZXnBBxMPECHTyNVAIFQh31170999 = -723541222;    float ZXnBBxMPECHTyNVAIFQh92695449 = -408121929;    float ZXnBBxMPECHTyNVAIFQh64009440 = -670376959;    float ZXnBBxMPECHTyNVAIFQh19404704 = -379854936;    float ZXnBBxMPECHTyNVAIFQh13843186 = -548534971;     ZXnBBxMPECHTyNVAIFQh69065059 = ZXnBBxMPECHTyNVAIFQh2244169;     ZXnBBxMPECHTyNVAIFQh2244169 = ZXnBBxMPECHTyNVAIFQh95383611;     ZXnBBxMPECHTyNVAIFQh95383611 = ZXnBBxMPECHTyNVAIFQh36020092;     ZXnBBxMPECHTyNVAIFQh36020092 = ZXnBBxMPECHTyNVAIFQh77966220;     ZXnBBxMPECHTyNVAIFQh77966220 = ZXnBBxMPECHTyNVAIFQh41905045;     ZXnBBxMPECHTyNVAIFQh41905045 = ZXnBBxMPECHTyNVAIFQh40279679;     ZXnBBxMPECHTyNVAIFQh40279679 = ZXnBBxMPECHTyNVAIFQh46760106;     ZXnBBxMPECHTyNVAIFQh46760106 = ZXnBBxMPECHTyNVAIFQh393169;     ZXnBBxMPECHTyNVAIFQh393169 = ZXnBBxMPECHTyNVAIFQh81684040;     ZXnBBxMPECHTyNVAIFQh81684040 = ZXnBBxMPECHTyNVAIFQh70260427;     ZXnBBxMPECHTyNVAIFQh70260427 = ZXnBBxMPECHTyNVAIFQh67424338;     ZXnBBxMPECHTyNVAIFQh67424338 = ZXnBBxMPECHTyNVAIFQh8943270;     ZXnBBxMPECHTyNVAIFQh8943270 = ZXnBBxMPECHTyNVAIFQh67847041;     ZXnBBxMPECHTyNVAIFQh67847041 = ZXnBBxMPECHTyNVAIFQh28584126;     ZXnBBxMPECHTyNVAIFQh28584126 = ZXnBBxMPECHTyNVAIFQh31985580;     ZXnBBxMPECHTyNVAIFQh31985580 = ZXnBBxMPECHTyNVAIFQh74653669;     ZXnBBxMPECHTyNVAIFQh74653669 = ZXnBBxMPECHTyNVAIFQh72130916;     ZXnBBxMPECHTyNVAIFQh72130916 = ZXnBBxMPECHTyNVAIFQh14677324;     ZXnBBxMPECHTyNVAIFQh14677324 = ZXnBBxMPECHTyNVAIFQh68448255;     ZXnBBxMPECHTyNVAIFQh68448255 = ZXnBBxMPECHTyNVAIFQh29670143;     ZXnBBxMPECHTyNVAIFQh29670143 = ZXnBBxMPECHTyNVAIFQh6909087;     ZXnBBxMPECHTyNVAIFQh6909087 = ZXnBBxMPECHTyNVAIFQh92387253;     ZXnBBxMPECHTyNVAIFQh92387253 = ZXnBBxMPECHTyNVAIFQh21338046;     ZXnBBxMPECHTyNVAIFQh21338046 = ZXnBBxMPECHTyNVAIFQh80639692;     ZXnBBxMPECHTyNVAIFQh80639692 = ZXnBBxMPECHTyNVAIFQh72272699;     ZXnBBxMPECHTyNVAIFQh72272699 = ZXnBBxMPECHTyNVAIFQh444560;     ZXnBBxMPECHTyNVAIFQh444560 = ZXnBBxMPECHTyNVAIFQh24378557;     ZXnBBxMPECHTyNVAIFQh24378557 = ZXnBBxMPECHTyNVAIFQh84803023;     ZXnBBxMPECHTyNVAIFQh84803023 = ZXnBBxMPECHTyNVAIFQh13793750;     ZXnBBxMPECHTyNVAIFQh13793750 = ZXnBBxMPECHTyNVAIFQh66522584;     ZXnBBxMPECHTyNVAIFQh66522584 = ZXnBBxMPECHTyNVAIFQh47820324;     ZXnBBxMPECHTyNVAIFQh47820324 = ZXnBBxMPECHTyNVAIFQh6209845;     ZXnBBxMPECHTyNVAIFQh6209845 = ZXnBBxMPECHTyNVAIFQh74319274;     ZXnBBxMPECHTyNVAIFQh74319274 = ZXnBBxMPECHTyNVAIFQh59862882;     ZXnBBxMPECHTyNVAIFQh59862882 = ZXnBBxMPECHTyNVAIFQh7911518;     ZXnBBxMPECHTyNVAIFQh7911518 = ZXnBBxMPECHTyNVAIFQh24326084;     ZXnBBxMPECHTyNVAIFQh24326084 = ZXnBBxMPECHTyNVAIFQh18138144;     ZXnBBxMPECHTyNVAIFQh18138144 = ZXnBBxMPECHTyNVAIFQh6637359;     ZXnBBxMPECHTyNVAIFQh6637359 = ZXnBBxMPECHTyNVAIFQh57935829;     ZXnBBxMPECHTyNVAIFQh57935829 = ZXnBBxMPECHTyNVAIFQh47072122;     ZXnBBxMPECHTyNVAIFQh47072122 = ZXnBBxMPECHTyNVAIFQh54652532;     ZXnBBxMPECHTyNVAIFQh54652532 = ZXnBBxMPECHTyNVAIFQh50608860;     ZXnBBxMPECHTyNVAIFQh50608860 = ZXnBBxMPECHTyNVAIFQh34566160;     ZXnBBxMPECHTyNVAIFQh34566160 = ZXnBBxMPECHTyNVAIFQh36283289;     ZXnBBxMPECHTyNVAIFQh36283289 = ZXnBBxMPECHTyNVAIFQh2256769;     ZXnBBxMPECHTyNVAIFQh2256769 = ZXnBBxMPECHTyNVAIFQh95672093;     ZXnBBxMPECHTyNVAIFQh95672093 = ZXnBBxMPECHTyNVAIFQh48148952;     ZXnBBxMPECHTyNVAIFQh48148952 = ZXnBBxMPECHTyNVAIFQh64938713;     ZXnBBxMPECHTyNVAIFQh64938713 = ZXnBBxMPECHTyNVAIFQh73234736;     ZXnBBxMPECHTyNVAIFQh73234736 = ZXnBBxMPECHTyNVAIFQh33175827;     ZXnBBxMPECHTyNVAIFQh33175827 = ZXnBBxMPECHTyNVAIFQh45642268;     ZXnBBxMPECHTyNVAIFQh45642268 = ZXnBBxMPECHTyNVAIFQh15920789;     ZXnBBxMPECHTyNVAIFQh15920789 = ZXnBBxMPECHTyNVAIFQh3067172;     ZXnBBxMPECHTyNVAIFQh3067172 = ZXnBBxMPECHTyNVAIFQh16766612;     ZXnBBxMPECHTyNVAIFQh16766612 = ZXnBBxMPECHTyNVAIFQh62155973;     ZXnBBxMPECHTyNVAIFQh62155973 = ZXnBBxMPECHTyNVAIFQh9856915;     ZXnBBxMPECHTyNVAIFQh9856915 = ZXnBBxMPECHTyNVAIFQh74045565;     ZXnBBxMPECHTyNVAIFQh74045565 = ZXnBBxMPECHTyNVAIFQh55380400;     ZXnBBxMPECHTyNVAIFQh55380400 = ZXnBBxMPECHTyNVAIFQh5693522;     ZXnBBxMPECHTyNVAIFQh5693522 = ZXnBBxMPECHTyNVAIFQh41460485;     ZXnBBxMPECHTyNVAIFQh41460485 = ZXnBBxMPECHTyNVAIFQh15901123;     ZXnBBxMPECHTyNVAIFQh15901123 = ZXnBBxMPECHTyNVAIFQh61957082;     ZXnBBxMPECHTyNVAIFQh61957082 = ZXnBBxMPECHTyNVAIFQh86599419;     ZXnBBxMPECHTyNVAIFQh86599419 = ZXnBBxMPECHTyNVAIFQh15161456;     ZXnBBxMPECHTyNVAIFQh15161456 = ZXnBBxMPECHTyNVAIFQh22440103;     ZXnBBxMPECHTyNVAIFQh22440103 = ZXnBBxMPECHTyNVAIFQh61214493;     ZXnBBxMPECHTyNVAIFQh61214493 = ZXnBBxMPECHTyNVAIFQh34623995;     ZXnBBxMPECHTyNVAIFQh34623995 = ZXnBBxMPECHTyNVAIFQh7984160;     ZXnBBxMPECHTyNVAIFQh7984160 = ZXnBBxMPECHTyNVAIFQh20672608;     ZXnBBxMPECHTyNVAIFQh20672608 = ZXnBBxMPECHTyNVAIFQh7659496;     ZXnBBxMPECHTyNVAIFQh7659496 = ZXnBBxMPECHTyNVAIFQh56515525;     ZXnBBxMPECHTyNVAIFQh56515525 = ZXnBBxMPECHTyNVAIFQh65493557;     ZXnBBxMPECHTyNVAIFQh65493557 = ZXnBBxMPECHTyNVAIFQh56741494;     ZXnBBxMPECHTyNVAIFQh56741494 = ZXnBBxMPECHTyNVAIFQh21376134;     ZXnBBxMPECHTyNVAIFQh21376134 = ZXnBBxMPECHTyNVAIFQh75017611;     ZXnBBxMPECHTyNVAIFQh75017611 = ZXnBBxMPECHTyNVAIFQh56300227;     ZXnBBxMPECHTyNVAIFQh56300227 = ZXnBBxMPECHTyNVAIFQh57821094;     ZXnBBxMPECHTyNVAIFQh57821094 = ZXnBBxMPECHTyNVAIFQh85054757;     ZXnBBxMPECHTyNVAIFQh85054757 = ZXnBBxMPECHTyNVAIFQh78382924;     ZXnBBxMPECHTyNVAIFQh78382924 = ZXnBBxMPECHTyNVAIFQh76600605;     ZXnBBxMPECHTyNVAIFQh76600605 = ZXnBBxMPECHTyNVAIFQh52295608;     ZXnBBxMPECHTyNVAIFQh52295608 = ZXnBBxMPECHTyNVAIFQh59439843;     ZXnBBxMPECHTyNVAIFQh59439843 = ZXnBBxMPECHTyNVAIFQh11568288;     ZXnBBxMPECHTyNVAIFQh11568288 = ZXnBBxMPECHTyNVAIFQh80617922;     ZXnBBxMPECHTyNVAIFQh80617922 = ZXnBBxMPECHTyNVAIFQh20880317;     ZXnBBxMPECHTyNVAIFQh20880317 = ZXnBBxMPECHTyNVAIFQh31899536;     ZXnBBxMPECHTyNVAIFQh31899536 = ZXnBBxMPECHTyNVAIFQh3142674;     ZXnBBxMPECHTyNVAIFQh3142674 = ZXnBBxMPECHTyNVAIFQh57552663;     ZXnBBxMPECHTyNVAIFQh57552663 = ZXnBBxMPECHTyNVAIFQh97706909;     ZXnBBxMPECHTyNVAIFQh97706909 = ZXnBBxMPECHTyNVAIFQh98054603;     ZXnBBxMPECHTyNVAIFQh98054603 = ZXnBBxMPECHTyNVAIFQh50280519;     ZXnBBxMPECHTyNVAIFQh50280519 = ZXnBBxMPECHTyNVAIFQh62757744;     ZXnBBxMPECHTyNVAIFQh62757744 = ZXnBBxMPECHTyNVAIFQh943838;     ZXnBBxMPECHTyNVAIFQh943838 = ZXnBBxMPECHTyNVAIFQh16475344;     ZXnBBxMPECHTyNVAIFQh16475344 = ZXnBBxMPECHTyNVAIFQh31170999;     ZXnBBxMPECHTyNVAIFQh31170999 = ZXnBBxMPECHTyNVAIFQh92695449;     ZXnBBxMPECHTyNVAIFQh92695449 = ZXnBBxMPECHTyNVAIFQh64009440;     ZXnBBxMPECHTyNVAIFQh64009440 = ZXnBBxMPECHTyNVAIFQh19404704;     ZXnBBxMPECHTyNVAIFQh19404704 = ZXnBBxMPECHTyNVAIFQh13843186;     ZXnBBxMPECHTyNVAIFQh13843186 = ZXnBBxMPECHTyNVAIFQh69065059;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void GRsyBZWCsXAVPCTtaExW852968() {     float MUCBlTjuVTYGUZCFmVTG39716727 = -66684102;    float MUCBlTjuVTYGUZCFmVTG70917686 = -200114214;    float MUCBlTjuVTYGUZCFmVTG75352530 = 99518252;    float MUCBlTjuVTYGUZCFmVTG75951379 = -703211234;    float MUCBlTjuVTYGUZCFmVTG99416542 = -62078710;    float MUCBlTjuVTYGUZCFmVTG31904808 = -958317829;    float MUCBlTjuVTYGUZCFmVTG58658574 = -117139897;    float MUCBlTjuVTYGUZCFmVTG3602487 = -695844855;    float MUCBlTjuVTYGUZCFmVTG46258223 = -194745908;    float MUCBlTjuVTYGUZCFmVTG4650501 = -841856348;    float MUCBlTjuVTYGUZCFmVTG12509588 = -970488726;    float MUCBlTjuVTYGUZCFmVTG12970910 = -743369773;    float MUCBlTjuVTYGUZCFmVTG88770656 = 69361827;    float MUCBlTjuVTYGUZCFmVTG51694075 = 45944023;    float MUCBlTjuVTYGUZCFmVTG11304145 = -829727924;    float MUCBlTjuVTYGUZCFmVTG64514220 = -689169936;    float MUCBlTjuVTYGUZCFmVTG68506496 = -288111058;    float MUCBlTjuVTYGUZCFmVTG92424410 = -434889456;    float MUCBlTjuVTYGUZCFmVTG96385105 = -541391470;    float MUCBlTjuVTYGUZCFmVTG27234965 = -505273773;    float MUCBlTjuVTYGUZCFmVTG84805897 = 87267104;    float MUCBlTjuVTYGUZCFmVTG68304851 = -692495286;    float MUCBlTjuVTYGUZCFmVTG94331062 = -720058490;    float MUCBlTjuVTYGUZCFmVTG55187583 = -479351149;    float MUCBlTjuVTYGUZCFmVTG44764081 = -427106633;    float MUCBlTjuVTYGUZCFmVTG2870280 = -467115696;    float MUCBlTjuVTYGUZCFmVTG8705346 = -363842187;    float MUCBlTjuVTYGUZCFmVTG32160782 = -795076844;    float MUCBlTjuVTYGUZCFmVTG16945512 = -851430566;    float MUCBlTjuVTYGUZCFmVTG52965845 = -566138591;    float MUCBlTjuVTYGUZCFmVTG73594581 = -942889908;    float MUCBlTjuVTYGUZCFmVTG92331784 = -313098508;    float MUCBlTjuVTYGUZCFmVTG89155205 = -524381311;    float MUCBlTjuVTYGUZCFmVTG39684249 = -880202397;    float MUCBlTjuVTYGUZCFmVTG37887118 = -436199645;    float MUCBlTjuVTYGUZCFmVTG67904776 = -908148978;    float MUCBlTjuVTYGUZCFmVTG68117570 = 44845766;    float MUCBlTjuVTYGUZCFmVTG70930632 = -617201989;    float MUCBlTjuVTYGUZCFmVTG85170647 = -449805108;    float MUCBlTjuVTYGUZCFmVTG53039128 = -474074543;    float MUCBlTjuVTYGUZCFmVTG7145406 = -935971211;    float MUCBlTjuVTYGUZCFmVTG83135523 = 30533585;    float MUCBlTjuVTYGUZCFmVTG99575819 = -750786070;    float MUCBlTjuVTYGUZCFmVTG41074460 = -898134683;    float MUCBlTjuVTYGUZCFmVTG32237348 = -298283599;    float MUCBlTjuVTYGUZCFmVTG8662604 = -252130543;    float MUCBlTjuVTYGUZCFmVTG21803297 = -441882978;    float MUCBlTjuVTYGUZCFmVTG13763162 = -508767342;    float MUCBlTjuVTYGUZCFmVTG70380165 = -195403840;    float MUCBlTjuVTYGUZCFmVTG81662222 = -982593745;    float MUCBlTjuVTYGUZCFmVTG1365152 = -824765205;    float MUCBlTjuVTYGUZCFmVTG87513772 = -503846520;    float MUCBlTjuVTYGUZCFmVTG70996445 = -289490673;    float MUCBlTjuVTYGUZCFmVTG93294476 = -284522289;    float MUCBlTjuVTYGUZCFmVTG16047070 = -854612118;    float MUCBlTjuVTYGUZCFmVTG71411875 = -374188816;    float MUCBlTjuVTYGUZCFmVTG76586623 = -480055725;    float MUCBlTjuVTYGUZCFmVTG20164947 = -421130600;    float MUCBlTjuVTYGUZCFmVTG31187298 = -176104602;    float MUCBlTjuVTYGUZCFmVTG96546262 = -594963014;    float MUCBlTjuVTYGUZCFmVTG23199462 = -494475642;    float MUCBlTjuVTYGUZCFmVTG26497793 = -322063053;    float MUCBlTjuVTYGUZCFmVTG86656975 = -844414290;    float MUCBlTjuVTYGUZCFmVTG93292378 = -628607317;    float MUCBlTjuVTYGUZCFmVTG31055920 = -898966441;    float MUCBlTjuVTYGUZCFmVTG20177804 = -557390219;    float MUCBlTjuVTYGUZCFmVTG23815704 = -118988463;    float MUCBlTjuVTYGUZCFmVTG49086407 = -50435777;    float MUCBlTjuVTYGUZCFmVTG13806957 = -517856333;    float MUCBlTjuVTYGUZCFmVTG43399369 = -921578947;    float MUCBlTjuVTYGUZCFmVTG96396650 = -634015703;    float MUCBlTjuVTYGUZCFmVTG97575863 = -670909069;    float MUCBlTjuVTYGUZCFmVTG7253763 = -985084348;    float MUCBlTjuVTYGUZCFmVTG43345978 = 32683072;    float MUCBlTjuVTYGUZCFmVTG20089559 = -569302562;    float MUCBlTjuVTYGUZCFmVTG1670375 = -943266482;    float MUCBlTjuVTYGUZCFmVTG68729032 = -941709216;    float MUCBlTjuVTYGUZCFmVTG53256603 = -821923807;    float MUCBlTjuVTYGUZCFmVTG22950236 = -81067550;    float MUCBlTjuVTYGUZCFmVTG36101478 = -74976090;    float MUCBlTjuVTYGUZCFmVTG81066982 = 74767282;    float MUCBlTjuVTYGUZCFmVTG94942183 = -855074846;    float MUCBlTjuVTYGUZCFmVTG61780616 = -499673004;    float MUCBlTjuVTYGUZCFmVTG35283290 = -868836822;    float MUCBlTjuVTYGUZCFmVTG51600694 = -741373386;    float MUCBlTjuVTYGUZCFmVTG86080808 = -339043388;    float MUCBlTjuVTYGUZCFmVTG21335339 = 76392164;    float MUCBlTjuVTYGUZCFmVTG95860729 = -139859022;    float MUCBlTjuVTYGUZCFmVTG23637179 = 74409720;    float MUCBlTjuVTYGUZCFmVTG66475243 = 37989171;    float MUCBlTjuVTYGUZCFmVTG91318153 = -328093254;    float MUCBlTjuVTYGUZCFmVTG47952623 = -534023634;    float MUCBlTjuVTYGUZCFmVTG39743335 = -341097388;    float MUCBlTjuVTYGUZCFmVTG88624385 = -854842095;    float MUCBlTjuVTYGUZCFmVTG29839666 = -979598901;    float MUCBlTjuVTYGUZCFmVTG80647612 = -513908158;    float MUCBlTjuVTYGUZCFmVTG96478547 = -125052125;    float MUCBlTjuVTYGUZCFmVTG6283442 = -22178754;    float MUCBlTjuVTYGUZCFmVTG10018540 = -999168243;    float MUCBlTjuVTYGUZCFmVTG12059545 = -66684102;     MUCBlTjuVTYGUZCFmVTG39716727 = MUCBlTjuVTYGUZCFmVTG70917686;     MUCBlTjuVTYGUZCFmVTG70917686 = MUCBlTjuVTYGUZCFmVTG75352530;     MUCBlTjuVTYGUZCFmVTG75352530 = MUCBlTjuVTYGUZCFmVTG75951379;     MUCBlTjuVTYGUZCFmVTG75951379 = MUCBlTjuVTYGUZCFmVTG99416542;     MUCBlTjuVTYGUZCFmVTG99416542 = MUCBlTjuVTYGUZCFmVTG31904808;     MUCBlTjuVTYGUZCFmVTG31904808 = MUCBlTjuVTYGUZCFmVTG58658574;     MUCBlTjuVTYGUZCFmVTG58658574 = MUCBlTjuVTYGUZCFmVTG3602487;     MUCBlTjuVTYGUZCFmVTG3602487 = MUCBlTjuVTYGUZCFmVTG46258223;     MUCBlTjuVTYGUZCFmVTG46258223 = MUCBlTjuVTYGUZCFmVTG4650501;     MUCBlTjuVTYGUZCFmVTG4650501 = MUCBlTjuVTYGUZCFmVTG12509588;     MUCBlTjuVTYGUZCFmVTG12509588 = MUCBlTjuVTYGUZCFmVTG12970910;     MUCBlTjuVTYGUZCFmVTG12970910 = MUCBlTjuVTYGUZCFmVTG88770656;     MUCBlTjuVTYGUZCFmVTG88770656 = MUCBlTjuVTYGUZCFmVTG51694075;     MUCBlTjuVTYGUZCFmVTG51694075 = MUCBlTjuVTYGUZCFmVTG11304145;     MUCBlTjuVTYGUZCFmVTG11304145 = MUCBlTjuVTYGUZCFmVTG64514220;     MUCBlTjuVTYGUZCFmVTG64514220 = MUCBlTjuVTYGUZCFmVTG68506496;     MUCBlTjuVTYGUZCFmVTG68506496 = MUCBlTjuVTYGUZCFmVTG92424410;     MUCBlTjuVTYGUZCFmVTG92424410 = MUCBlTjuVTYGUZCFmVTG96385105;     MUCBlTjuVTYGUZCFmVTG96385105 = MUCBlTjuVTYGUZCFmVTG27234965;     MUCBlTjuVTYGUZCFmVTG27234965 = MUCBlTjuVTYGUZCFmVTG84805897;     MUCBlTjuVTYGUZCFmVTG84805897 = MUCBlTjuVTYGUZCFmVTG68304851;     MUCBlTjuVTYGUZCFmVTG68304851 = MUCBlTjuVTYGUZCFmVTG94331062;     MUCBlTjuVTYGUZCFmVTG94331062 = MUCBlTjuVTYGUZCFmVTG55187583;     MUCBlTjuVTYGUZCFmVTG55187583 = MUCBlTjuVTYGUZCFmVTG44764081;     MUCBlTjuVTYGUZCFmVTG44764081 = MUCBlTjuVTYGUZCFmVTG2870280;     MUCBlTjuVTYGUZCFmVTG2870280 = MUCBlTjuVTYGUZCFmVTG8705346;     MUCBlTjuVTYGUZCFmVTG8705346 = MUCBlTjuVTYGUZCFmVTG32160782;     MUCBlTjuVTYGUZCFmVTG32160782 = MUCBlTjuVTYGUZCFmVTG16945512;     MUCBlTjuVTYGUZCFmVTG16945512 = MUCBlTjuVTYGUZCFmVTG52965845;     MUCBlTjuVTYGUZCFmVTG52965845 = MUCBlTjuVTYGUZCFmVTG73594581;     MUCBlTjuVTYGUZCFmVTG73594581 = MUCBlTjuVTYGUZCFmVTG92331784;     MUCBlTjuVTYGUZCFmVTG92331784 = MUCBlTjuVTYGUZCFmVTG89155205;     MUCBlTjuVTYGUZCFmVTG89155205 = MUCBlTjuVTYGUZCFmVTG39684249;     MUCBlTjuVTYGUZCFmVTG39684249 = MUCBlTjuVTYGUZCFmVTG37887118;     MUCBlTjuVTYGUZCFmVTG37887118 = MUCBlTjuVTYGUZCFmVTG67904776;     MUCBlTjuVTYGUZCFmVTG67904776 = MUCBlTjuVTYGUZCFmVTG68117570;     MUCBlTjuVTYGUZCFmVTG68117570 = MUCBlTjuVTYGUZCFmVTG70930632;     MUCBlTjuVTYGUZCFmVTG70930632 = MUCBlTjuVTYGUZCFmVTG85170647;     MUCBlTjuVTYGUZCFmVTG85170647 = MUCBlTjuVTYGUZCFmVTG53039128;     MUCBlTjuVTYGUZCFmVTG53039128 = MUCBlTjuVTYGUZCFmVTG7145406;     MUCBlTjuVTYGUZCFmVTG7145406 = MUCBlTjuVTYGUZCFmVTG83135523;     MUCBlTjuVTYGUZCFmVTG83135523 = MUCBlTjuVTYGUZCFmVTG99575819;     MUCBlTjuVTYGUZCFmVTG99575819 = MUCBlTjuVTYGUZCFmVTG41074460;     MUCBlTjuVTYGUZCFmVTG41074460 = MUCBlTjuVTYGUZCFmVTG32237348;     MUCBlTjuVTYGUZCFmVTG32237348 = MUCBlTjuVTYGUZCFmVTG8662604;     MUCBlTjuVTYGUZCFmVTG8662604 = MUCBlTjuVTYGUZCFmVTG21803297;     MUCBlTjuVTYGUZCFmVTG21803297 = MUCBlTjuVTYGUZCFmVTG13763162;     MUCBlTjuVTYGUZCFmVTG13763162 = MUCBlTjuVTYGUZCFmVTG70380165;     MUCBlTjuVTYGUZCFmVTG70380165 = MUCBlTjuVTYGUZCFmVTG81662222;     MUCBlTjuVTYGUZCFmVTG81662222 = MUCBlTjuVTYGUZCFmVTG1365152;     MUCBlTjuVTYGUZCFmVTG1365152 = MUCBlTjuVTYGUZCFmVTG87513772;     MUCBlTjuVTYGUZCFmVTG87513772 = MUCBlTjuVTYGUZCFmVTG70996445;     MUCBlTjuVTYGUZCFmVTG70996445 = MUCBlTjuVTYGUZCFmVTG93294476;     MUCBlTjuVTYGUZCFmVTG93294476 = MUCBlTjuVTYGUZCFmVTG16047070;     MUCBlTjuVTYGUZCFmVTG16047070 = MUCBlTjuVTYGUZCFmVTG71411875;     MUCBlTjuVTYGUZCFmVTG71411875 = MUCBlTjuVTYGUZCFmVTG76586623;     MUCBlTjuVTYGUZCFmVTG76586623 = MUCBlTjuVTYGUZCFmVTG20164947;     MUCBlTjuVTYGUZCFmVTG20164947 = MUCBlTjuVTYGUZCFmVTG31187298;     MUCBlTjuVTYGUZCFmVTG31187298 = MUCBlTjuVTYGUZCFmVTG96546262;     MUCBlTjuVTYGUZCFmVTG96546262 = MUCBlTjuVTYGUZCFmVTG23199462;     MUCBlTjuVTYGUZCFmVTG23199462 = MUCBlTjuVTYGUZCFmVTG26497793;     MUCBlTjuVTYGUZCFmVTG26497793 = MUCBlTjuVTYGUZCFmVTG86656975;     MUCBlTjuVTYGUZCFmVTG86656975 = MUCBlTjuVTYGUZCFmVTG93292378;     MUCBlTjuVTYGUZCFmVTG93292378 = MUCBlTjuVTYGUZCFmVTG31055920;     MUCBlTjuVTYGUZCFmVTG31055920 = MUCBlTjuVTYGUZCFmVTG20177804;     MUCBlTjuVTYGUZCFmVTG20177804 = MUCBlTjuVTYGUZCFmVTG23815704;     MUCBlTjuVTYGUZCFmVTG23815704 = MUCBlTjuVTYGUZCFmVTG49086407;     MUCBlTjuVTYGUZCFmVTG49086407 = MUCBlTjuVTYGUZCFmVTG13806957;     MUCBlTjuVTYGUZCFmVTG13806957 = MUCBlTjuVTYGUZCFmVTG43399369;     MUCBlTjuVTYGUZCFmVTG43399369 = MUCBlTjuVTYGUZCFmVTG96396650;     MUCBlTjuVTYGUZCFmVTG96396650 = MUCBlTjuVTYGUZCFmVTG97575863;     MUCBlTjuVTYGUZCFmVTG97575863 = MUCBlTjuVTYGUZCFmVTG7253763;     MUCBlTjuVTYGUZCFmVTG7253763 = MUCBlTjuVTYGUZCFmVTG43345978;     MUCBlTjuVTYGUZCFmVTG43345978 = MUCBlTjuVTYGUZCFmVTG20089559;     MUCBlTjuVTYGUZCFmVTG20089559 = MUCBlTjuVTYGUZCFmVTG1670375;     MUCBlTjuVTYGUZCFmVTG1670375 = MUCBlTjuVTYGUZCFmVTG68729032;     MUCBlTjuVTYGUZCFmVTG68729032 = MUCBlTjuVTYGUZCFmVTG53256603;     MUCBlTjuVTYGUZCFmVTG53256603 = MUCBlTjuVTYGUZCFmVTG22950236;     MUCBlTjuVTYGUZCFmVTG22950236 = MUCBlTjuVTYGUZCFmVTG36101478;     MUCBlTjuVTYGUZCFmVTG36101478 = MUCBlTjuVTYGUZCFmVTG81066982;     MUCBlTjuVTYGUZCFmVTG81066982 = MUCBlTjuVTYGUZCFmVTG94942183;     MUCBlTjuVTYGUZCFmVTG94942183 = MUCBlTjuVTYGUZCFmVTG61780616;     MUCBlTjuVTYGUZCFmVTG61780616 = MUCBlTjuVTYGUZCFmVTG35283290;     MUCBlTjuVTYGUZCFmVTG35283290 = MUCBlTjuVTYGUZCFmVTG51600694;     MUCBlTjuVTYGUZCFmVTG51600694 = MUCBlTjuVTYGUZCFmVTG86080808;     MUCBlTjuVTYGUZCFmVTG86080808 = MUCBlTjuVTYGUZCFmVTG21335339;     MUCBlTjuVTYGUZCFmVTG21335339 = MUCBlTjuVTYGUZCFmVTG95860729;     MUCBlTjuVTYGUZCFmVTG95860729 = MUCBlTjuVTYGUZCFmVTG23637179;     MUCBlTjuVTYGUZCFmVTG23637179 = MUCBlTjuVTYGUZCFmVTG66475243;     MUCBlTjuVTYGUZCFmVTG66475243 = MUCBlTjuVTYGUZCFmVTG91318153;     MUCBlTjuVTYGUZCFmVTG91318153 = MUCBlTjuVTYGUZCFmVTG47952623;     MUCBlTjuVTYGUZCFmVTG47952623 = MUCBlTjuVTYGUZCFmVTG39743335;     MUCBlTjuVTYGUZCFmVTG39743335 = MUCBlTjuVTYGUZCFmVTG88624385;     MUCBlTjuVTYGUZCFmVTG88624385 = MUCBlTjuVTYGUZCFmVTG29839666;     MUCBlTjuVTYGUZCFmVTG29839666 = MUCBlTjuVTYGUZCFmVTG80647612;     MUCBlTjuVTYGUZCFmVTG80647612 = MUCBlTjuVTYGUZCFmVTG96478547;     MUCBlTjuVTYGUZCFmVTG96478547 = MUCBlTjuVTYGUZCFmVTG6283442;     MUCBlTjuVTYGUZCFmVTG6283442 = MUCBlTjuVTYGUZCFmVTG10018540;     MUCBlTjuVTYGUZCFmVTG10018540 = MUCBlTjuVTYGUZCFmVTG12059545;     MUCBlTjuVTYGUZCFmVTG12059545 = MUCBlTjuVTYGUZCFmVTG39716727;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void aBhvBsRAtOmPtNgDkVrE7511712() {     float GVeQGAXPjlPAgQGXUfPd39710255 = 60791757;    float GVeQGAXPjlPAgQGXUfPd9490003 = -242709339;    float GVeQGAXPjlPAgQGXUfPd76665742 = -957610361;    float GVeQGAXPjlPAgQGXUfPd61031239 = -523752064;    float GVeQGAXPjlPAgQGXUfPd8256645 = -530434031;    float GVeQGAXPjlPAgQGXUfPd66710240 = -657750641;    float GVeQGAXPjlPAgQGXUfPd27205911 = -921676364;    float GVeQGAXPjlPAgQGXUfPd72816537 = -400022173;    float GVeQGAXPjlPAgQGXUfPd28998387 = 91608023;    float GVeQGAXPjlPAgQGXUfPd1290015 = -478663219;    float GVeQGAXPjlPAgQGXUfPd32830193 = -929960798;    float GVeQGAXPjlPAgQGXUfPd27262587 = -478848136;    float GVeQGAXPjlPAgQGXUfPd20623571 = 47703352;    float GVeQGAXPjlPAgQGXUfPd5538296 = -299209029;    float GVeQGAXPjlPAgQGXUfPd56476254 = -792440776;    float GVeQGAXPjlPAgQGXUfPd39078767 = 78034413;    float GVeQGAXPjlPAgQGXUfPd24359256 = -107725627;    float GVeQGAXPjlPAgQGXUfPd15424944 = -831961078;    float GVeQGAXPjlPAgQGXUfPd16813375 = -59373737;    float GVeQGAXPjlPAgQGXUfPd99308727 = -17298224;    float GVeQGAXPjlPAgQGXUfPd64475321 = -723199270;    float GVeQGAXPjlPAgQGXUfPd46601117 = -102487169;    float GVeQGAXPjlPAgQGXUfPd31209459 = -79016332;    float GVeQGAXPjlPAgQGXUfPd98837691 = -360271671;    float GVeQGAXPjlPAgQGXUfPd73206770 = -853800486;    float GVeQGAXPjlPAgQGXUfPd19444357 = -375321625;    float GVeQGAXPjlPAgQGXUfPd78926927 = -664641900;    float GVeQGAXPjlPAgQGXUfPd67564306 = -304043832;    float GVeQGAXPjlPAgQGXUfPd29623959 = -397500881;    float GVeQGAXPjlPAgQGXUfPd29055715 = -241569964;    float GVeQGAXPjlPAgQGXUfPd7007260 = -229983143;    float GVeQGAXPjlPAgQGXUfPd77055642 = -704920270;    float GVeQGAXPjlPAgQGXUfPd74607629 = -726325527;    float GVeQGAXPjlPAgQGXUfPd39967086 = -756884130;    float GVeQGAXPjlPAgQGXUfPd20245156 = -695513570;    float GVeQGAXPjlPAgQGXUfPd10732967 = -987914724;    float GVeQGAXPjlPAgQGXUfPd80058937 = -54807612;    float GVeQGAXPjlPAgQGXUfPd40848312 = -612464668;    float GVeQGAXPjlPAgQGXUfPd20484245 = -62486525;    float GVeQGAXPjlPAgQGXUfPd24912021 = -83506229;    float GVeQGAXPjlPAgQGXUfPd95001073 = 76084890;    float GVeQGAXPjlPAgQGXUfPd68833184 = -163258497;    float GVeQGAXPjlPAgQGXUfPd59859180 = -209916325;    float GVeQGAXPjlPAgQGXUfPd306359 = -455099126;    float GVeQGAXPjlPAgQGXUfPd16255400 = -771543001;    float GVeQGAXPjlPAgQGXUfPd49090572 = -64175213;    float GVeQGAXPjlPAgQGXUfPd49501090 = -923163600;    float GVeQGAXPjlPAgQGXUfPd89146742 = -17528698;    float GVeQGAXPjlPAgQGXUfPd89038554 = -859167701;    float GVeQGAXPjlPAgQGXUfPd1707646 = -569618631;    float GVeQGAXPjlPAgQGXUfPd14624522 = -403320212;    float GVeQGAXPjlPAgQGXUfPd2061030 = -885992269;    float GVeQGAXPjlPAgQGXUfPd46444794 = -181544681;    float GVeQGAXPjlPAgQGXUfPd3544748 = -816442141;    float GVeQGAXPjlPAgQGXUfPd43036390 = -611511395;    float GVeQGAXPjlPAgQGXUfPd93109137 = -836721074;    float GVeQGAXPjlPAgQGXUfPd78280543 = -63693008;    float GVeQGAXPjlPAgQGXUfPd77828051 = -497338691;    float GVeQGAXPjlPAgQGXUfPd87824469 = -669951579;    float GVeQGAXPjlPAgQGXUfPd88812287 = -55112407;    float GVeQGAXPjlPAgQGXUfPd87783312 = -993108741;    float GVeQGAXPjlPAgQGXUfPd59641604 = -517632532;    float GVeQGAXPjlPAgQGXUfPd43192579 = 97478708;    float GVeQGAXPjlPAgQGXUfPd99942672 = -666822014;    float GVeQGAXPjlPAgQGXUfPd94282754 = -148680077;    float GVeQGAXPjlPAgQGXUfPd55774551 = -125040529;    float GVeQGAXPjlPAgQGXUfPd52654957 = -752522609;    float GVeQGAXPjlPAgQGXUfPd80656484 = -195412519;    float GVeQGAXPjlPAgQGXUfPd85293139 = -603695460;    float GVeQGAXPjlPAgQGXUfPd45743288 = -804526052;    float GVeQGAXPjlPAgQGXUfPd59019830 = -867157976;    float GVeQGAXPjlPAgQGXUfPd83510943 = -495260960;    float GVeQGAXPjlPAgQGXUfPd94940699 = -669474554;    float GVeQGAXPjlPAgQGXUfPd91901354 = -975867508;    float GVeQGAXPjlPAgQGXUfPd4307655 = 6616885;    float GVeQGAXPjlPAgQGXUfPd95642137 = -459940773;    float GVeQGAXPjlPAgQGXUfPd86741937 = -892570845;    float GVeQGAXPjlPAgQGXUfPd30903101 = -623917206;    float GVeQGAXPjlPAgQGXUfPd82582291 = -588728670;    float GVeQGAXPjlPAgQGXUfPd24116199 = -689625273;    float GVeQGAXPjlPAgQGXUfPd69943267 = -452158025;    float GVeQGAXPjlPAgQGXUfPd89780184 = -547113202;    float GVeQGAXPjlPAgQGXUfPd78525751 = -444876132;    float GVeQGAXPjlPAgQGXUfPd27916313 = -827882251;    float GVeQGAXPjlPAgQGXUfPd14431194 = -838249753;    float GVeQGAXPjlPAgQGXUfPd4946230 = -343990875;    float GVeQGAXPjlPAgQGXUfPd30610849 = -423375589;    float GVeQGAXPjlPAgQGXUfPd71062882 = -909883387;    float GVeQGAXPjlPAgQGXUfPd96930695 = -45372735;    float GVeQGAXPjlPAgQGXUfPd27136019 = -858792496;    float GVeQGAXPjlPAgQGXUfPd32452423 = -824221717;    float GVeQGAXPjlPAgQGXUfPd2230887 = -557468921;    float GVeQGAXPjlPAgQGXUfPd53023843 = -942513090;    float GVeQGAXPjlPAgQGXUfPd31671957 = 92625882;    float GVeQGAXPjlPAgQGXUfPd37128708 = -90397489;    float GVeQGAXPjlPAgQGXUfPd35359469 = -406282578;    float GVeQGAXPjlPAgQGXUfPd25640605 = -160737205;    float GVeQGAXPjlPAgQGXUfPd59916507 = -543094312;    float GVeQGAXPjlPAgQGXUfPd6023604 = -206419049;    float GVeQGAXPjlPAgQGXUfPd60480849 = 60791757;     GVeQGAXPjlPAgQGXUfPd39710255 = GVeQGAXPjlPAgQGXUfPd9490003;     GVeQGAXPjlPAgQGXUfPd9490003 = GVeQGAXPjlPAgQGXUfPd76665742;     GVeQGAXPjlPAgQGXUfPd76665742 = GVeQGAXPjlPAgQGXUfPd61031239;     GVeQGAXPjlPAgQGXUfPd61031239 = GVeQGAXPjlPAgQGXUfPd8256645;     GVeQGAXPjlPAgQGXUfPd8256645 = GVeQGAXPjlPAgQGXUfPd66710240;     GVeQGAXPjlPAgQGXUfPd66710240 = GVeQGAXPjlPAgQGXUfPd27205911;     GVeQGAXPjlPAgQGXUfPd27205911 = GVeQGAXPjlPAgQGXUfPd72816537;     GVeQGAXPjlPAgQGXUfPd72816537 = GVeQGAXPjlPAgQGXUfPd28998387;     GVeQGAXPjlPAgQGXUfPd28998387 = GVeQGAXPjlPAgQGXUfPd1290015;     GVeQGAXPjlPAgQGXUfPd1290015 = GVeQGAXPjlPAgQGXUfPd32830193;     GVeQGAXPjlPAgQGXUfPd32830193 = GVeQGAXPjlPAgQGXUfPd27262587;     GVeQGAXPjlPAgQGXUfPd27262587 = GVeQGAXPjlPAgQGXUfPd20623571;     GVeQGAXPjlPAgQGXUfPd20623571 = GVeQGAXPjlPAgQGXUfPd5538296;     GVeQGAXPjlPAgQGXUfPd5538296 = GVeQGAXPjlPAgQGXUfPd56476254;     GVeQGAXPjlPAgQGXUfPd56476254 = GVeQGAXPjlPAgQGXUfPd39078767;     GVeQGAXPjlPAgQGXUfPd39078767 = GVeQGAXPjlPAgQGXUfPd24359256;     GVeQGAXPjlPAgQGXUfPd24359256 = GVeQGAXPjlPAgQGXUfPd15424944;     GVeQGAXPjlPAgQGXUfPd15424944 = GVeQGAXPjlPAgQGXUfPd16813375;     GVeQGAXPjlPAgQGXUfPd16813375 = GVeQGAXPjlPAgQGXUfPd99308727;     GVeQGAXPjlPAgQGXUfPd99308727 = GVeQGAXPjlPAgQGXUfPd64475321;     GVeQGAXPjlPAgQGXUfPd64475321 = GVeQGAXPjlPAgQGXUfPd46601117;     GVeQGAXPjlPAgQGXUfPd46601117 = GVeQGAXPjlPAgQGXUfPd31209459;     GVeQGAXPjlPAgQGXUfPd31209459 = GVeQGAXPjlPAgQGXUfPd98837691;     GVeQGAXPjlPAgQGXUfPd98837691 = GVeQGAXPjlPAgQGXUfPd73206770;     GVeQGAXPjlPAgQGXUfPd73206770 = GVeQGAXPjlPAgQGXUfPd19444357;     GVeQGAXPjlPAgQGXUfPd19444357 = GVeQGAXPjlPAgQGXUfPd78926927;     GVeQGAXPjlPAgQGXUfPd78926927 = GVeQGAXPjlPAgQGXUfPd67564306;     GVeQGAXPjlPAgQGXUfPd67564306 = GVeQGAXPjlPAgQGXUfPd29623959;     GVeQGAXPjlPAgQGXUfPd29623959 = GVeQGAXPjlPAgQGXUfPd29055715;     GVeQGAXPjlPAgQGXUfPd29055715 = GVeQGAXPjlPAgQGXUfPd7007260;     GVeQGAXPjlPAgQGXUfPd7007260 = GVeQGAXPjlPAgQGXUfPd77055642;     GVeQGAXPjlPAgQGXUfPd77055642 = GVeQGAXPjlPAgQGXUfPd74607629;     GVeQGAXPjlPAgQGXUfPd74607629 = GVeQGAXPjlPAgQGXUfPd39967086;     GVeQGAXPjlPAgQGXUfPd39967086 = GVeQGAXPjlPAgQGXUfPd20245156;     GVeQGAXPjlPAgQGXUfPd20245156 = GVeQGAXPjlPAgQGXUfPd10732967;     GVeQGAXPjlPAgQGXUfPd10732967 = GVeQGAXPjlPAgQGXUfPd80058937;     GVeQGAXPjlPAgQGXUfPd80058937 = GVeQGAXPjlPAgQGXUfPd40848312;     GVeQGAXPjlPAgQGXUfPd40848312 = GVeQGAXPjlPAgQGXUfPd20484245;     GVeQGAXPjlPAgQGXUfPd20484245 = GVeQGAXPjlPAgQGXUfPd24912021;     GVeQGAXPjlPAgQGXUfPd24912021 = GVeQGAXPjlPAgQGXUfPd95001073;     GVeQGAXPjlPAgQGXUfPd95001073 = GVeQGAXPjlPAgQGXUfPd68833184;     GVeQGAXPjlPAgQGXUfPd68833184 = GVeQGAXPjlPAgQGXUfPd59859180;     GVeQGAXPjlPAgQGXUfPd59859180 = GVeQGAXPjlPAgQGXUfPd306359;     GVeQGAXPjlPAgQGXUfPd306359 = GVeQGAXPjlPAgQGXUfPd16255400;     GVeQGAXPjlPAgQGXUfPd16255400 = GVeQGAXPjlPAgQGXUfPd49090572;     GVeQGAXPjlPAgQGXUfPd49090572 = GVeQGAXPjlPAgQGXUfPd49501090;     GVeQGAXPjlPAgQGXUfPd49501090 = GVeQGAXPjlPAgQGXUfPd89146742;     GVeQGAXPjlPAgQGXUfPd89146742 = GVeQGAXPjlPAgQGXUfPd89038554;     GVeQGAXPjlPAgQGXUfPd89038554 = GVeQGAXPjlPAgQGXUfPd1707646;     GVeQGAXPjlPAgQGXUfPd1707646 = GVeQGAXPjlPAgQGXUfPd14624522;     GVeQGAXPjlPAgQGXUfPd14624522 = GVeQGAXPjlPAgQGXUfPd2061030;     GVeQGAXPjlPAgQGXUfPd2061030 = GVeQGAXPjlPAgQGXUfPd46444794;     GVeQGAXPjlPAgQGXUfPd46444794 = GVeQGAXPjlPAgQGXUfPd3544748;     GVeQGAXPjlPAgQGXUfPd3544748 = GVeQGAXPjlPAgQGXUfPd43036390;     GVeQGAXPjlPAgQGXUfPd43036390 = GVeQGAXPjlPAgQGXUfPd93109137;     GVeQGAXPjlPAgQGXUfPd93109137 = GVeQGAXPjlPAgQGXUfPd78280543;     GVeQGAXPjlPAgQGXUfPd78280543 = GVeQGAXPjlPAgQGXUfPd77828051;     GVeQGAXPjlPAgQGXUfPd77828051 = GVeQGAXPjlPAgQGXUfPd87824469;     GVeQGAXPjlPAgQGXUfPd87824469 = GVeQGAXPjlPAgQGXUfPd88812287;     GVeQGAXPjlPAgQGXUfPd88812287 = GVeQGAXPjlPAgQGXUfPd87783312;     GVeQGAXPjlPAgQGXUfPd87783312 = GVeQGAXPjlPAgQGXUfPd59641604;     GVeQGAXPjlPAgQGXUfPd59641604 = GVeQGAXPjlPAgQGXUfPd43192579;     GVeQGAXPjlPAgQGXUfPd43192579 = GVeQGAXPjlPAgQGXUfPd99942672;     GVeQGAXPjlPAgQGXUfPd99942672 = GVeQGAXPjlPAgQGXUfPd94282754;     GVeQGAXPjlPAgQGXUfPd94282754 = GVeQGAXPjlPAgQGXUfPd55774551;     GVeQGAXPjlPAgQGXUfPd55774551 = GVeQGAXPjlPAgQGXUfPd52654957;     GVeQGAXPjlPAgQGXUfPd52654957 = GVeQGAXPjlPAgQGXUfPd80656484;     GVeQGAXPjlPAgQGXUfPd80656484 = GVeQGAXPjlPAgQGXUfPd85293139;     GVeQGAXPjlPAgQGXUfPd85293139 = GVeQGAXPjlPAgQGXUfPd45743288;     GVeQGAXPjlPAgQGXUfPd45743288 = GVeQGAXPjlPAgQGXUfPd59019830;     GVeQGAXPjlPAgQGXUfPd59019830 = GVeQGAXPjlPAgQGXUfPd83510943;     GVeQGAXPjlPAgQGXUfPd83510943 = GVeQGAXPjlPAgQGXUfPd94940699;     GVeQGAXPjlPAgQGXUfPd94940699 = GVeQGAXPjlPAgQGXUfPd91901354;     GVeQGAXPjlPAgQGXUfPd91901354 = GVeQGAXPjlPAgQGXUfPd4307655;     GVeQGAXPjlPAgQGXUfPd4307655 = GVeQGAXPjlPAgQGXUfPd95642137;     GVeQGAXPjlPAgQGXUfPd95642137 = GVeQGAXPjlPAgQGXUfPd86741937;     GVeQGAXPjlPAgQGXUfPd86741937 = GVeQGAXPjlPAgQGXUfPd30903101;     GVeQGAXPjlPAgQGXUfPd30903101 = GVeQGAXPjlPAgQGXUfPd82582291;     GVeQGAXPjlPAgQGXUfPd82582291 = GVeQGAXPjlPAgQGXUfPd24116199;     GVeQGAXPjlPAgQGXUfPd24116199 = GVeQGAXPjlPAgQGXUfPd69943267;     GVeQGAXPjlPAgQGXUfPd69943267 = GVeQGAXPjlPAgQGXUfPd89780184;     GVeQGAXPjlPAgQGXUfPd89780184 = GVeQGAXPjlPAgQGXUfPd78525751;     GVeQGAXPjlPAgQGXUfPd78525751 = GVeQGAXPjlPAgQGXUfPd27916313;     GVeQGAXPjlPAgQGXUfPd27916313 = GVeQGAXPjlPAgQGXUfPd14431194;     GVeQGAXPjlPAgQGXUfPd14431194 = GVeQGAXPjlPAgQGXUfPd4946230;     GVeQGAXPjlPAgQGXUfPd4946230 = GVeQGAXPjlPAgQGXUfPd30610849;     GVeQGAXPjlPAgQGXUfPd30610849 = GVeQGAXPjlPAgQGXUfPd71062882;     GVeQGAXPjlPAgQGXUfPd71062882 = GVeQGAXPjlPAgQGXUfPd96930695;     GVeQGAXPjlPAgQGXUfPd96930695 = GVeQGAXPjlPAgQGXUfPd27136019;     GVeQGAXPjlPAgQGXUfPd27136019 = GVeQGAXPjlPAgQGXUfPd32452423;     GVeQGAXPjlPAgQGXUfPd32452423 = GVeQGAXPjlPAgQGXUfPd2230887;     GVeQGAXPjlPAgQGXUfPd2230887 = GVeQGAXPjlPAgQGXUfPd53023843;     GVeQGAXPjlPAgQGXUfPd53023843 = GVeQGAXPjlPAgQGXUfPd31671957;     GVeQGAXPjlPAgQGXUfPd31671957 = GVeQGAXPjlPAgQGXUfPd37128708;     GVeQGAXPjlPAgQGXUfPd37128708 = GVeQGAXPjlPAgQGXUfPd35359469;     GVeQGAXPjlPAgQGXUfPd35359469 = GVeQGAXPjlPAgQGXUfPd25640605;     GVeQGAXPjlPAgQGXUfPd25640605 = GVeQGAXPjlPAgQGXUfPd59916507;     GVeQGAXPjlPAgQGXUfPd59916507 = GVeQGAXPjlPAgQGXUfPd6023604;     GVeQGAXPjlPAgQGXUfPd6023604 = GVeQGAXPjlPAgQGXUfPd60480849;     GVeQGAXPjlPAgQGXUfPd60480849 = GVeQGAXPjlPAgQGXUfPd39710255;}
// Junk Finished
