#pragma once
#include "HookIncludes.h"
static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_CHLClient->GetAllClasses();

	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}
IClientNetworkable* CL_CreateDLLEntity(int iEnt, int iClass, int iSerialNum)
{
	ClientClass* pClient = g_CHLClient->GetAllClasses();
	if (!pClient)
		return false;

	while (pClient)
	{
		if (pClient->m_ClassID == iClass)
			return pClient->m_pCreateFn(iEnt, iSerialNum);
		pClient = pClient->m_pNext;
	}
	return false;
}

bool isKnife(int id)
{
	switch (id)
	{
	case 42:
	case 500:
	case 506:
	case 505:
	case 508:
	case 507:
	case 515:
	case 514:
	case 512:
	case 516:
	case 59:
		return true;
	}
	return false;
}

void GloveChanger()
{
	C_BaseEntity *local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	player_info_t localPlayerInfo;
	if (!g_Engine->GetPlayerInfo(g_Engine->GetLocalPlayer(), &localPlayerInfo)) return;


	CBaseHandle* wearables = (CBaseHandle*)local->GetWearables();
	static CBaseHandle glove_handle = 0;

	auto glove = reinterpret_cast<CGloves*>(g_EntityList->GetClientEntityFromHandle(wearables[0]));

	if (!glove) // There is no glove
	{
		// Try to get our last created glove
		auto our_glove = reinterpret_cast<CGloves*>(g_EntityList->GetClientEntityFromHandle(glove_handle));

		if (our_glove) // Our glove still exists
		{
			wearables[0] = glove_handle;
			glove = our_glove;
		}
	}

	if (!local->IsAlive())
	{
		// We are dead but we have a glove, destroy it
		if (glove)
		{
			glove->SetDestroyedOnRecreateEntities();
			glove->Release();
		}

		return;
	}



	if (!glove)
	{
		int iEnt = g_EntityList->GetHighestEntityIndex() + 1;
		int iSerialNumber = RandomInt(0x0, 0xFFF);

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)CL_CreateDLLEntity(iEnt, (int)ClassID::CEconWearable, iSerialNumber);
		if (pWeapon)
		{
			auto m_Index = iEnt | (iSerialNumber << 16);
			glove_handle = m_Index;
			*(DWORD*)((DWORD)local + offsetz.DT_BaseCombatCharacter.m_hMyWearables) = m_Index;
			glove = (CGloves*)g_EntityList->GetClientEntity(iEnt);
		}

		{
			static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>
				(U::FindPattern("client.dll", (BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1", "xxxxxxxxxxxx"));

			static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

			set_abs_origin_fn(glove, new_pos);
		}
	}



	*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;


	if (glove)
	{
		switch (g_Options.Skinchanger.gloves)
		{
		case 1: //bloodhound bronzed
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5027 || *glove->FallbackPaintKit() != 10008)
			{
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
				*glove->FallbackPaintKit() = 10008;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 2: //bloodhound Charred
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5027 || *glove->FallbackPaintKit() != 10006)
			{
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
				*glove->FallbackPaintKit() = 10006;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 3: //bloodhound SnakeBite
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5027 || *glove->FallbackPaintKit() != 10007)
			{
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
				*glove->FallbackPaintKit() = 10007;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 4: //bloodhound Guerrilla
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5027 || *glove->FallbackPaintKit() != 10039)
			{
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5027;
				*glove->FallbackPaintKit() = 10039;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 5: //sporty Pandora's Box
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5030 || *glove->FallbackPaintKit() != 10037)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
				*glove->FallbackPaintKit() = 10037;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 6: //sporty Arid
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5030 || *glove->FallbackPaintKit() != 10019)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
				*glove->FallbackPaintKit() = 10019;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 7: //sporty Supercondutor
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5030 || *glove->FallbackPaintKit() != 10018)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
				*glove->FallbackPaintKit() = 10018;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 8: //sporty Hedge Maze
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5030 || *glove->FallbackPaintKit() != 10038)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5030;
				*glove->FallbackPaintKit() = 10038;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 9://SLICK_GLOVES Crimson Weave
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5031 || *glove->FallbackPaintKit() != 10016)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
				*glove->FallbackPaintKit() = 10016;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 10://SLICK_GLOVES Lunar Weave
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5031 || *glove->FallbackPaintKit() != 10013)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
				*glove->FallbackPaintKit() = 10013;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 11://SLICK_GLOVES Diamond Back
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5031 || *glove->FallbackPaintKit() != 10040)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
				*glove->FallbackPaintKit() = 10040;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 12://SLICK_GLOVES Convoy
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5031 || *glove->FallbackPaintKit() != 10016)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5031;
				*glove->FallbackPaintKit() = 10016;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 13://LEATHER_HANDWRAPS Leather
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5032 || *glove->FallbackPaintKit() != 10009)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
				*glove->FallbackPaintKit() = 10009;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 14://LEATHER_HANDWRAPS Badlands
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5032 || *glove->FallbackPaintKit() != 10036)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
				*glove->FallbackPaintKit() = 10036;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 15://LEATHER_HANDWRAPS Spruce DDPAT
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5032 || *glove->FallbackPaintKit() != 10010)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
				*glove->FallbackPaintKit() = 10010;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 16://LEATHER_HANDWRAPS Slaughter
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5032 || *glove->FallbackPaintKit() != 10021)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5032;
				*glove->FallbackPaintKit() = 10021;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 17://MOTORCYCLE_GLOVES Spearmint
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5033 || *glove->FallbackPaintKit() != 10026)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
				*glove->FallbackPaintKit() = 10026;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 18://MOTORCYCLE_GLOVES Cool mint
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5033 || *glove->FallbackPaintKit() != 10028)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
				*glove->FallbackPaintKit() = 10028;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 19://MOTORCYCLE_GLOVES BOOM!
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5033 || *glove->FallbackPaintKit() != 10027)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
				*glove->FallbackPaintKit() = 10027;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 20://MOTORCYCLE_GLOVES Eclipse
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5033 || *glove->FallbackPaintKit() != 10024)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5033;
				*glove->FallbackPaintKit() = 10024;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 21://SPECIALIST_GLOVES Emerald Web
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5034 || *glove->FallbackPaintKit() != 10034)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
				*glove->FallbackPaintKit() = 10034;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 22://SPECIALIST_GLOVES Foundation
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5034 || *glove->FallbackPaintKit() != 10035)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
				*glove->FallbackPaintKit() = 10035;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 23://SPECIALIST_GLOVES Forest DDPAT
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5034 || *glove->FallbackPaintKit() != 10030)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
				*glove->FallbackPaintKit() = 10030;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 24://SPECIALIST_GLOVES Crimson Kimono
		{
			if (*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() != 5034 || *glove->FallbackPaintKit() != 10033)
			{
				glove->SetGloveModelIndex(g_ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
				*glove->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 5034;
				*glove->FallbackPaintKit() = 10033;
				*glove->GetEntityQuality() = 4;
				*glove->FallbackSeed() = 0;
				*glove->FallbackWear() = 0.001f;
				*glove->FallbackStatTrak() = -1;
				*glove->m_AttributeManager()->m_Item()->ItemIDHigh() = -1;
				*glove->AccountID() = localPlayerInfo.xuid;

				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		}
	}
}