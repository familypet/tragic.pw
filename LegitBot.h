


#pragma once

#include "SDK.h"
#include "singleton.hpp"

class legitbot
	: public singleton<legitbot>
{
public:
	legitbot();
	void askdjiqu3h4ruygsfhjafjaf23iua();

	void OnCreateMove(CInput::CUserCmd * pCmd, C_BaseEntity * local, bool & bSendPacket);



	//
private:
	// Targetting
	bool hit_chance(C_BaseEntity* local, CInput::CUserCmd* cmd, CBaseCombatWeapon* weapon, C_BaseEntity* target);
	void legitbot::LegitAA(CInput::CUserCmd *pCmd, bool& bSendPacket);
	void do_aimbot(C_BaseEntity *local, CBaseCombatWeapon *weapon, CInput::CUserCmd *cmd);
	QAngle get_randomized_recoil(C_BaseEntity* local);
	QAngle get_randomized_angles(C_BaseEntity* local);
	void triggerbot(CInput::CUserCmd* cmd, C_BaseEntity* local, CBaseCombatWeapon* weapon);
	void weapon_settings(CBaseCombatWeapon* weapon);
	// Functionality
	bool get_hitbox(C_BaseEntity *local, C_BaseEntity *entity, Vector &destination);
	int get_target(C_BaseEntity *local, CBaseCombatWeapon *weapon, CInput::CUserCmd *cmd, Vector &destination);
	
	// Aimbot
	int best_target;

	QAngle view_angle;
	QAngle aim_angle;
	QAngle delta_angle;
	QAngle final_angle;

	Vector hitbox_position;

	int aim_key;
	float aim_smooth;
	float aim_fov;
	float randomized_smooth;
	float recoil_min;
	float recoil_max;
	float randomized_angle;
	bool shoot;
	int hitbox;
	bool multibone;

};