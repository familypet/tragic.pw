#pragma once
#include "sdk.h"
class ResolverData
{
public:
	int kill_type;
	float resolved;
	float last_angle;
	float last_lby;
	int shots_fired = 0;
};
class PlayerList
{
public:
	class CPlayer
	{
	public:
		float OldYawDeltas;
		float OldLowerBodyYaws;
		Vector ShootPos[125];
		int index = -1;
		C_BaseEntity* entity;
		Vector reset = Vector(0, 0, 0);
		float lastsim = 0;
		Vector lastorigin = Vector(0, 0, 0);
		std::vector< float > pastangles;
		int ScannedNumber = 0;
		int BestIndex = 0;
		float difference = 0.f;
		float Backtrack[360];
		float flLastPelvisAng = 0.f;
		float flEyeAng = 0.f;
		float resolved = 0.f;
		float posedifference = 0.f;
		CSGOHitboxID* box;

		CPlayer(C_BaseEntity* entity, int index, int lastsim) : entity(entity), index(index), lastsim(lastsim)
		{
		}
	};

private:
	std::vector< CPlayer > Players;
public:
	void Update()
	{
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

		for (size_t i = 0; i < Players.size(); i++)
		{
			if (Players[i].entity == nullptr)
			{
				Players.erase(Players.begin() + i);
				continue;
			}
			if (Players[i].entity == pLocal)
			{
				Players.erase(Players.begin() + i);
				continue;
			};
		}
	}

	void UpdateSim()
	{
		Update();
		for (size_t i = 0; i < Players.size(); i++)
		{
			Players[i].lastsim = Players[i].entity->GetSimulationTime();
		}
	}

	void AddPlayer(C_BaseEntity* ent)
	{
		Players.emplace_back(CPlayer(ent, ent->GetIndex(), ent->GetSimulationTime()));
	}

	CPlayer* FindPlayer(C_BaseEntity* ent)
	{
		for (size_t i = 0; i < Players.size(); i++)
			if (Players[i].index == ent->GetIndex())
				return &Players[i];
		AddPlayer(ent);
		return FindPlayer(ent);
	}
} plist;