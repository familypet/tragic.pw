#pragma once
#include "HookIncludes.h"

int GetEstimatedServerTickCount(float latency)
{
	return (int)floorf((float)((float)(latency) / (float)((uintptr_t)&g_Globals->interval_per_tick)) + 0.5) + 1 + (int)((uintptr_t)&g_Globals->tickcount);
}
struct ResolverData
{
	float simtime, flcycle[13], flprevcycle[13], flweight[13], flweightdatarate[13], fakewalkdetection[2], fakeanglesimtimedetection[2], fakewalkdetectionsimtime[2];
	float yaw, addyaw, lbycurtime;
	float shotsimtime, oldlby, lastmovinglby, balanceadjustsimtime, balanceadjustflcycle;
	int fakeanglesimtickdetectionaverage[4], amountgreaterthan2, amountequal1or2, amountequal0or1, amountequal1, amountequal0, resetmovetick, resetmovetick2;
	int tick, balanceadjusttick, missedshots, activity[13];
	bool bfakeangle, bfakewalk, playerhurtcalled, weaponfirecalled;
	Vector shotaimangles, hitboxPos, balanceadjustaimangles;
	uint32_t norder[13];
	char* resolvermode = "NONE", *fakewalk = "Not Moving";
}   pResolverData[64];;
float oldlowerbodyyaw = 0;

inline float ClampYaw(float yaw) {
	while (yaw > 180.f)
		yaw -= 360.f;
	while (yaw < -180.f)
		yaw += 360.f;
	return yaw;
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