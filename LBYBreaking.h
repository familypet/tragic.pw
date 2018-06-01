#include "SDK.h"

namespace LBYMod
{
	float OldLBY;
	float LBYBreakerTimer;
	float LastLBYUpdateTime;
	bool bSwitch;
	float GetLatency();
	float GetOutgoingLatency();
	float GetIncomingLatency();
	bool NextLBYUpdate();
	float CurrentVelocity(C_BaseEntity* LocalPlayer);
	bool NextMovingLBYUpdate();

}

float LBYMod::GetLatency()
{
	INetChannelInfo* nci = g_Engine->GetNetChannelInfo();
	if (nci)
	{

		float Latency = nci->GetAvgLatency(0) + nci->GetAvgLatency(1);
		return Latency;
	}
	else
	{

		return 0.0f;
	}
}

float LBYMod::GetOutgoingLatency()
{
	INetChannelInfo *nci = g_Engine->GetNetChannelInfo();
	if (nci)
	{

		float OutgoingLatency = nci->GetAvgLatency(0);
		return OutgoingLatency;
	}
	else
	{

		return 0.0f;
	}
}
float LBYMod::GetIncomingLatency()
{
	INetChannelInfo *nci = g_Engine->GetNetChannelInfo();
	if (nci)
	{
		float IncomingLatency = nci->GetAvgLatency(1);
		return IncomingLatency;
	}
	else
	{

		return 0.0f;
	}
}



bool LBYMod::NextLBYUpdate()
{
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	INetChannelInfo* nci = g_Engine->GetNetChannelInfo();

	int LocalPlayer = g_Engine->GetLocalPlayer();

	float flServerTime = g_Globals->curtime + nci->GetLatency(0);

	if (OldLBY != pLocal->GetLowerBodyYaw())
	{

		LBYBreakerTimer++;
		OldLBY = pLocal->GetLowerBodyYaw();
		bSwitch = !bSwitch;
		LastLBYUpdateTime = flServerTime;
	}

	if (pLocal->GetVelocity().Length() > 1)
	{
		LastLBYUpdateTime = flServerTime;
		return false;
	}

	if ((LastLBYUpdateTime + 0.5 - (GetLatency() * 2) < flServerTime) && (pLocal->GetFlags() & FL_ONGROUND))
	{
		if (LastLBYUpdateTime + 0.7 - (GetLatency() * 2) < flServerTime)
		{
			LastLBYUpdateTime += 0.9;
		}
		return true;
	}
	return false;
}

float LBYMod::CurrentVelocity(C_BaseEntity* LocalPlayer)
{
	int vel = LocalPlayer->GetVelocity().Length2D();
	return vel;
}

bool LBYMod::NextMovingLBYUpdate()
{
	C_BaseEntity* LocalPlayer = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	float flServerTime = (float)(LocalPlayer->GetTickBase()  * g_Globals->interval_per_tick);


	if (OldLBY != LocalPlayer->GetLowerBodyYaw())
	{

		LBYBreakerTimer++;
		OldLBY = LocalPlayer->GetLowerBodyYaw();
		bSwitch = !bSwitch;
		LastLBYUpdateTime = flServerTime;
	}

	if (CurrentVelocity(LocalPlayer) > 1)
	{
		LastLBYUpdateTime = flServerTime;
		return false;
	}

	if ((LastLBYUpdateTime + 1 - (GetLatency() * 2) < flServerTime) && (LocalPlayer->GetFlags() & FL_ONGROUND))
	{
		if (LastLBYUpdateTime + 0.22 - (GetLatency() * 2) < flServerTime)
		{
			LastLBYUpdateTime += 0.22;
		}
		return true;
	}
	return false;
}