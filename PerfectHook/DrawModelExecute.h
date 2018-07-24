#pragma once
#include "HookIncludes.h"
typedef void(__thiscall* dme_t)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);

float flColor[3];
float flVColor[3];

IMaterial* GetCurrentMaterial(int mati)
{
	static IMaterial* mat;

	switch (mati)
	{
	case 3:
		mat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", nullptr);
		break;
	case 4:
		mat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", nullptr);
		break;
	case 5:
		mat = g_MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", nullptr);
		break;
	case 6:
		mat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", nullptr);
		break;
	case 7:
		mat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", nullptr);
		break;
	default:
		mat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", nullptr);
		return mat;
		break;
	}
	return mat;
}

IMaterial* GetCurrentZMaterial(int mati)
{
	static IMaterial* zmat;

	switch (mati)
	{
	case 3:
		zmat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", nullptr);
		zmat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, g_Options.Visuals.XQZ);
		return zmat;
		break;
	case 4:
		zmat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", nullptr);
		zmat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, g_Options.Visuals.XQZ);
		return zmat;
		break;
	case 5:
		zmat = g_MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", nullptr);
		zmat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, g_Options.Visuals.XQZ);
		return zmat;
		break;
	case 6:
		zmat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", nullptr);
		zmat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, g_Options.Visuals.XQZ);
		return zmat;
		break;
	case 7:
		zmat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", nullptr);
		zmat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, g_Options.Visuals.XQZ);
		return zmat;
		break;
	default:
		zmat = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", nullptr);
		return zmat;
		break;
	}
}


static QAngle getRealAngle() {
	CInput::CUserCmd *pCmd;
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (g_Options.Ragebot.EnabledAntiAim || g_Options.LegitBot.LegitAA)
		return pCmd->viewangles;
	else if (pLocal)
		return pLocal->GetEyePosition();
}

static QAngle getFakeAngle() {
	CInput::CUserCmd *pCmd;
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (g_Options.Ragebot.EnabledAntiAim || g_Options.LegitBot.LegitAA)
		return pCmd->viewangles;
	else if (pLocal)
		return pLocal->GetEyePosition();
}

static QAngle getLowerBodyAngle() {
	C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (!pLocal)
		return QAngle();

	return QAngle(0, pLocal->GetLowerBodyYaw(), 0);
}

void GhostChams()
{
	if (g_Engine->IsConnected() && g_Engine->IsInGame())
	{
		if (g_Options.Visuals.GhostChams) {
			C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
			
				static IMaterial* mat;
				Vector Chamsang;
				if (g_Options.Visuals.GhostChamsMode == 0)
					Chamsang = getRealAngle();
				else if (g_Options.Visuals.GhostChamsMode == 1)
					Chamsang = getFakeAngle();
				else if (g_Options.Visuals.GhostChamsMode == 2)
					Chamsang = getLowerBodyAngle();
					Vector OrigAng;
					OrigAng = pLocal->GetEyeAnglesNew();
					pLocal->SetAngle2(Vector(0, Chamsang.y, 0));
					g_RenderView->SetColorModulation(g_Options.Visuals.GhostChamsColor);
					g_ModelRender->ForcedMaterialOverride(mat);
					g_ModelRender->ForcedMaterialOverride(nullptr);
					pLocal->SetAngle2(OrigAng);
				
			
		}
	}
}
void __fastcall hkDrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	static bool DontDraw = false;
	static auto ofunc = hooks::modelrender.get_original<dme_t>(21);
	
	static IMaterial* ignorez = CreateMaterial(true, true, false);
	static IMaterial* notignorez = CreateMaterial(false, true, false);

	IMaterial* zmat = GetCurrentZMaterial(g_Options.Visuals.matierial);
	IMaterial* mat = GetCurrentMaterial(g_Options.Visuals.matierial);

	IMaterial* hmat = GetCurrentMaterial(g_Options.Visuals.matierial);
	IMaterial* wmat = GetCurrentMaterial(g_Options.Visuals.matierial);

	static IMaterial* norm = CreateMaterial(false, true, false);
	static IMaterial* flat = CreateMaterial(false, false, false);
	static IMaterial* wire = CreateMaterial(false, false, true);

	static IMaterial* znorm = CreateMaterial(true, true, false);
	static IMaterial* zflat = CreateMaterial(true, false, false);
	static IMaterial* zwire = CreateMaterial(true, false, true);

	static IMaterial* Outline = CreateMaterial(true, false, true);
	DontDraw = false;

	const char * ModelName = g_ModelInfo->GetModelName((model_t*)pInfo.pModel);
	C_BaseEntity* pModelEntity = (C_BaseEntity*)g_EntityList->GetClientEntity(pInfo.entity_index);
	C_BaseEntity* pLocal = (C_BaseEntity*)g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());

	if (ModelName && strstr(ModelName, "models/player"))
	{
		if (pModelEntity && pLocal)
		{
			if ((g_Options.Visuals.Teamchams || pModelEntity->GetTeamNum() != pLocal->GetTeamNum()) && g_Options.Visuals.Chams)
			{
				pModelEntity = g_EntityList->GetClientEntity(pInfo.entity_index);

				if (pModelEntity->GetTeamNum() != pLocal->GetTeamNum())
				{
					flColor[0] = g_Options.Colors.EnemyChamsNVis[0];
					flColor[1] = g_Options.Colors.EnemyChamsNVis[1];
					flColor[2] = g_Options.Colors.EnemyChamsNVis[2];
					flVColor[0] = g_Options.Colors.EnemyChamsVis[0];
					flVColor[1] = g_Options.Colors.EnemyChamsVis[1];
					flVColor[2] = g_Options.Colors.EnemyChamsVis[2];
				}
				if (pModelEntity->GetTeamNum() == pLocal->GetTeamNum())
				{
					flColor[0] = g_Options.Colors.TeamChamsVis[0];
					flColor[1] = g_Options.Colors.TeamChamsVis[1];
					flColor[2] = g_Options.Colors.TeamChamsVis[2];
					flVColor[0] = g_Options.Colors.TeamChamsVis[0];
					flVColor[1] = g_Options.Colors.TeamChamsVis[1];
					flVColor[2] = g_Options.Colors.TeamChamsVis[2];
				}
				if (pModelEntity)
				{
					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0)
					{
						if (g_Options.Visuals.matierial == 0)
						{
							if (g_Options.Visuals.XQZ)
							{

								g_RenderView->SetColorModulation(flColor);
								g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
								g_ModelRender->ForcedMaterialOverride(znorm);
								ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
							}
							g_RenderView->SetColorModulation(flVColor);
							g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
							g_ModelRender->ForcedMaterialOverride(norm);
							ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
						else if (g_Options.Visuals.matierial == 1)
						{
							if (g_Options.Visuals.XQZ)
							{

								g_RenderView->SetColorModulation(flColor);
								g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
								g_ModelRender->ForcedMaterialOverride(zflat);
								ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
							}
							g_RenderView->SetColorModulation(flVColor);
							g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
							g_ModelRender->ForcedMaterialOverride(flat);
							ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
						else if (g_Options.Visuals.matierial == 2)
						{
							if (g_Options.Visuals.XQZ)
							{

								g_RenderView->SetColorModulation(flColor);
								g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
								g_ModelRender->ForcedMaterialOverride(zwire);
								ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
							}
							g_RenderView->SetColorModulation(flVColor);
							g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
							g_ModelRender->ForcedMaterialOverride(wire);
							ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
						else
						{
							if (g_Options.Visuals.XQZ)
							{

								g_RenderView->SetColorModulation(flColor);
								g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
								g_ModelRender->ForcedMaterialOverride(zmat);
								ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
							}
							g_RenderView->SetColorModulation(flVColor);
							g_RenderView->SetBlend(g_Options.Visuals.champlayeralpha / 100.f);
							g_ModelRender->ForcedMaterialOverride(mat);
							ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
					}
				/*	else
					{
						ForceMaterial(Color(255, 255, 255), znorm);
					}*/

				}
			}
			if (g_Options.Visuals.chamsHp && g_Options.Visuals.Chams && g_Options.Visuals.Enabled)
			{
				float color1[3] = { 0.f, 1.f, 0.f };
				float color2[3] = { 1.f, 1.f, 0.f };
				float color3[3] = { 1.f, 0.f, 0.f };

				float alpha = 1.f;
				if (pModelEntity->HasGunGameImmunity())
					alpha = 0, 1.f;

				if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 && pModelEntity->GetTeamNum() != pLocal->GetTeamNum() && pModelEntity != pLocal)
				{

					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 50)
					{
						g_RenderView->SetColorModulation(color1);
						g_RenderView->SetBlend(alpha);
						g_ModelRender->ForcedMaterialOverride(norm);
						ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
					}
					else if (pModelEntity->IsAlive() && pModelEntity->GetHealth() < 50 && pModelEntity->GetHealth() > 25)
					{
						g_RenderView->SetColorModulation(color2);
						g_RenderView->SetBlend(alpha);
						g_ModelRender->ForcedMaterialOverride(norm);
						ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
					}
					else if (pModelEntity->IsAlive() && pModelEntity->GetHealth() < 25)
					{
						g_RenderView->SetColorModulation(color3);
						g_RenderView->SetBlend(alpha);
						g_ModelRender->ForcedMaterialOverride(norm);
						ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
					}

				}
				else if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 && g_Options.Visuals.Teamchams && pModelEntity != pLocal)
				{
					if (pModelEntity->GetTeamNum() == pLocal->GetTeamNum())
					{
						if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 50)
						{
							g_RenderView->SetColorModulation(color1);
							g_RenderView->SetBlend(alpha);
							g_ModelRender->ForcedMaterialOverride(norm);
							ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
						else if (pModelEntity->IsAlive() && pModelEntity->GetHealth() < 50 && pModelEntity->GetHealth() > 25)
						{
							g_RenderView->SetColorModulation(color2);
							g_RenderView->SetBlend(alpha);
							g_ModelRender->ForcedMaterialOverride(norm);
							ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
						else if (pModelEntity->IsAlive() && pModelEntity->GetHealth() < 25)
						{
							g_RenderView->SetColorModulation(color3);
							g_RenderView->SetBlend(alpha);
							g_ModelRender->ForcedMaterialOverride(norm);
							ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
					}
				}
			}

			if (pModelEntity == pLocal)
			{
				if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0)
				{

					g_RenderView->SetBlend(0.3f);
					g_ModelRender->ForcedMaterialOverride(nullptr);
					ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

				}
				else
				{
					ForceMaterial(Color(255, 255, 255), zmat);
				}
			}
		}
	}


	else if (g_Options.Visuals.Chams && g_Options.Visuals.WeaponsWorld && strstr(ModelName, "_dropped.mdl"))
	{
		ForceMaterial(Color(255, 255, 255), notignorez);
	}

	if (g_Options.Visuals.Hands && strstr(ModelName, "arms"))
	{
		if (g_Options.Visuals.Hands == 1)
		{
			DontDraw = true;
		}
		if (g_Options.Visuals.Hands == 2)
		{
			g_ModelRender->ForcedMaterialOverride(notignorez);
			g_RenderView->SetColorModulation(g_Options.Colors.hands_color);
			g_RenderView->SetBlend(g_Options.Colors.hands_alpha);

		}
		if (g_Options.Visuals.Hands == 3)
		{
			IMaterial* Hands = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", "Model textures");
			g_ModelRender->ForcedMaterialOverride(Hands);
			g_RenderView->SetBlend(g_Options.Colors.hands_alpha);
		}
		if (g_Options.Visuals.Hands == 4)
		{
			IMaterial* Hands = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", "Model textures");
			g_ModelRender->ForcedMaterialOverride(Hands);
			g_RenderView->SetBlend(g_Options.Colors.hands_alpha);
		}
		if (g_Options.Visuals.Hands == 5)
		{
			IMaterial* Hands = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", "Model textures");
			g_ModelRender->ForcedMaterialOverride(Hands);
			g_RenderView->SetBlend(g_Options.Colors.hands_alpha);
		}
		if (g_Options.Visuals.Hands == 6)
		{
			IMaterial* Hands = g_MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", "Model textures");
			g_ModelRender->ForcedMaterialOverride(Hands);
			g_RenderView->SetBlend(g_Options.Colors.hands_alpha);
		}
		if (g_Options.Visuals.Hands == 7)
		{
			IMaterial* Hands = g_MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", "Model textures");
			g_ModelRender->ForcedMaterialOverride(Hands);
			g_RenderView->SetBlend(g_Options.Colors.hands_alpha);
		}
	}

	if (!DontDraw)
		ofunc(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	g_ModelRender->ForcedMaterialOverride(NULL);

	GhostChams();
}

