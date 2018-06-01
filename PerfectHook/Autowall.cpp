#include "AutoWall.h"
//#include "R.h"

#define    HITGROUP_GENERIC    0
#define    HITGROUP_HEAD        1
#define    HITGROUP_CHEST        2
#define    HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10

inline bool CGameTrace::DidHitWorld() const
{
	return m_pEnt->GetIndex() == 0;
}

inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEnt != NULL && !DidHitWorld();
}

bool HandleBulletPenetration(CSWeaponInfo *wpn_data, FireBulletData &data);
float GetHitgroupDamageMult(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 1.f;
	case HITGROUP_HEAD:
		return 4.f;
	case HITGROUP_CHEST:
		return 1.f;
	case HITGROUP_STOMACH:
		return 1.25f;
	case HITGROUP_LEFTARM:
		return 1.f;
	case HITGROUP_RIGHTARM:
		return 1.f;
	case HITGROUP_LEFTLEG:
		return 0.75f;
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	case HITGROUP_GEAR:
		return 1.f;
	default:
		break;
	}

	return 1.f;
}
bool IsArmored(C_BaseEntity* Entity, int ArmorValue, int Hitgroup)
{
	bool result = false;

	if (ArmorValue > 0)
	{
		switch (Hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			result = true;
			break;
		case HITGROUP_HEAD:
			result = Entity->HasHelmet(); // DT_CSPlayer -> m_bHasHelmet
			break;
		}
	}

	return result;
}

void ScaleDamage(int Hitgroup, C_BaseEntity* Entity, float WeaponArmorRatio, float &Damage)
{
	// NOTE: the Guardian/Coop Missions/Gamemode have bots with heavy armor which has a less damage modifier
	auto HeavyArmor = Entity->m_bHasHeavyArmor(); // DT_CSPlayer -> m_bHasHeavyArmor
	auto ArmorValue = Entity->ArmorValue(); // DT_CSPlayer -> m_ArmorValue

	switch (Hitgroup)
	{
	case HITGROUP_HEAD:
		if (HeavyArmor)
			Damage = (Damage * 4.f) * 0.5f;
		else
			Damage *= 4.f;
		break;
	case HITGROUP_STOMACH:
		Damage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		Damage *= 0.75f;
		break;
	}

	if (IsArmored(Entity, ArmorValue, Hitgroup))
	{
		float v47 = 1.f, ArmorBonusRatio = 0.5f, ArmorRatio = WeaponArmorRatio * 0.5f;

		if (HeavyArmor)
		{
			ArmorBonusRatio = 0.33f;
			ArmorRatio = (WeaponArmorRatio * 0.5f) * 0.5f;
			v47 = 0.33f;
		}

		auto NewDamage = Damage * ArmorRatio;

		if (HeavyArmor)
			NewDamage *= 0.85f;

		if (((Damage - (Damage * ArmorRatio)) * (v47 * ArmorBonusRatio)) > ArmorValue)
			NewDamage = Damage - (ArmorValue / ArmorBonusRatio);

		Damage = NewDamage;
	}
}

/*void ScaleDamage(int hitgroup, C_BaseEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
current_damage *= GetHitgroupDamageMult(hitgroup);

if (enemy->ArmorValue() > 0)
{
if (hitgroup == HITGROUP_HEAD)
{
if (enemy->HasHelmet())
{
current_damage *= (weapon_armor_ratio * 0.25f);
}
}
else
{
current_damage *= (weapon_armor_ratio * 0.5f);
}
}

}*/

bool SimulateFireBullet(C_BaseEntity* entity, C_BaseEntity *local, CBaseCombatWeapon *weapon, FireBulletData &data)
{
	//Utils::ToLog("SimulateFireBullet");
	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	auto *wpn_data = weapon->GetCSWpnData();

	data.current_damage = static_cast<float>(wpn_data->m_iDamage);

	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = wpn_data->m_fRange - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		UTIL_TraceLine(data.src, end, 0x4600400B, local, 0, &data.enter_trace);
		UTIL_ClipTraceToPlayers(entity, data.src, end + data.direction * 40.f, 0x4600400B, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			break;

		if ((data.enter_trace.hitgroup <= 7)
			&& (data.enter_trace.hitgroup > 0)
			&& (local->GetTeamNum() != data.enter_trace.m_pEnt->GetTeamNum()
				|| g_Options.Ragebot.FriendlyFire))
		{
			data.trace_length += (float)(data.enter_trace.fraction * data.trace_length_remaining);
			data.current_damage *= (float)(pow(wpn_data->m_fRangeModifier, data.trace_length * 0.002));
			ScaleDamage(data.enter_trace.hitgroup, data.enter_trace.m_pEnt, wpn_data->m_fArmorRatio, data.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(wpn_data, data))
			break;
	}

	return false;
}

bool HandleBulletPenetration(CSWeaponInfo *wpn_data, FireBulletData &data)
{
	surfacedata_t *enter_surface_data = g_PhysProps->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.penetrationmodifier;


	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= (float)(pow(wpn_data->m_fRangeModifier, (data.trace_length * 0.002)));

	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;

	surfacedata_t *exit_surface_data = g_PhysProps->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.penetrationmodifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->m_fPenetration) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;


	float lost_damage = fmaxf(0.0f, v35 + thickness);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}


/*
*    CanHit() - example of how to use the code
*     @in  point: target hitbox vector
*     @out damage_given: amount of damage the shot would do
*/
bool CanHit(C_BaseEntity* entity, const Vector &point, float *damage_given)
{
	//Utils::ToLog("CANHIT");
	auto *local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	auto data = FireBulletData(local->GetOrigin() + local->GetViewOffset());
	data.filter = CTraceFilter();
	data.filter.pSkip = local;

	Vector angles;
	//	CalcAngle(data.src, point, angles);
	VectorAngles(point - data.src, angles);
	AngleVectors(angles, &data.direction);
	VectorNormalize(data.direction);

	if (SimulateFireBullet(entity, local, reinterpret_cast<CBaseCombatWeapon*>(g_EntityList->GetClientEntityFromHandle(static_cast<HANDLE>(local->GetActiveWeaponHandle()))), data))
	{
		*damage_given = data.current_damage;
		//Utils::ToLog("CANHIT END");
		return true;
	}

	return false;
}

bool CanWallbang(int &dmg)
{

	auto *local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle((HANDLE)local->GetActiveWeaponHandle());
	if (!local)
		return false;
	FireBulletData data = FireBulletData(local->GetEyePosition());
	data.filter = CTraceFilter();
	data.filter.pSkip = local;
	auto flRange = weapon->GetCSWpnData()->m_fRange;
	Vector EyeAng;
	g_Engine->GetViewAngles(EyeAng);

	Vector dst, forward;

	AngleVectors(EyeAng, &forward);
	dst = data.src + (forward * 8000.f);

	Vector angles;
	CalcAngle(data.src, dst, angles);
	AngleVectors(angles, &data.direction);
	VectorNormalize(data.direction);



	if (!weapon)
		return false;

	data.penetrate_count = 1;

	CSWeaponInfo *weaponData = weapon->GetCSWpnData();

	if (!weaponData)
		return false;

	data.current_damage = (int)weaponData->m_iDamage;

	data.trace_length_remaining = weaponData->m_fRange;

	Vector end = data.src + data.direction * data.trace_length_remaining;

	UTIL_TraceLine(data.src, end, MASK_SHOT | CONTENTS_GRATE, local, 0, &data.enter_trace);

	if (data.enter_trace.fraction == 1.f)
		return false;

	if (HandleBulletPenetration(weaponData, data))
	{
		dmg = data.current_damage;
		return true;
	}

	return false;
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jizfocx {
public:
	double pajrzo;
	int ebbqcgr;
	jizfocx();
	double jbbbokinlmveancy(bool akndbv, int tmjkzf, int fdrvqon, string atjlsqhj, double aiqztwegcgaggi, string phmtzvopmcl);
	string rrcspryrcljuwnahm(bool omvvztgtzkxyqf, bool jbnci, double vtfcncwkw, int laobxsqofz);
	string tozfabvwapfjdnamgikeraf(int uihfjvjwqhtug, int vmnmokvokb, bool oqvztxkjquw, double ykcyp, double yajkdcml, double gnshmv, double vadjbpgaymva, string kqlgcpeptfs, string wzudoswn);
	int efkrycxuotzvpgwqpxmzeezw(double cwpadd, double zyftxrbz, int xycamtzjs, bool dumger, string nskjs, double ljnfuajkfknge, int wmprnchmsy, bool lkrmlyipeeom, bool erukkpamydanu, int ihfnrkho);
	string xzwqmzkuogdeeusekmobjyrfm(double iotlbvxzflmbhxg, bool sphizmpekll, string cxnwzsf);
	int fwqijfunok(double bdhrcmd, double pvurwelx, string omtrcvstyeyolmf, int yzzlssv, string yqutloobwjzvbao, string rklxtlmwwbed, int qdslfczqnqcipb);
	bool qjjvqvwxyloam(string cfaknatjgszr, bool jbolkuvohtoplco, int lrgskqmhsca, bool xueyue, int kotkazyprywjrm, int cdrbcidmwl, string qrqjpwdt, double rbxxsxpptvzpv, int giiexudokzxmmjl);

protected:
	int oxvffsfnsb;

	int bdmgshalyhlaihukzbm(double dsgdee, int yfhyj, bool seeeeqibedpbzf, string jfjdnulliutusq, string qzsteezkulvkziu, string nqemiwuemt, bool razkyya);
	double pyhzqumipisvrnwr(bool lkfqehyfubd, string kldlhmjengahe, string rtzspchbss, string tuasmiazpvmgwx, string xsxxcrnspi, string zgzejzvybrnxjei);
	int aqifyftteqjrxerqdiizscnj(string aklfn, bool gbsigsflc, string erlbymdb, double lgrcssrkpth, bool wyasndmtewa);
	string bnmqmtbnmkbnhjugmva(string xuozcn, double vprpbgkck, int wxswgqjb, bool pjltqddof, int mhsasxspqgketa);
	bool volzmlgbnosqriblysnkb(bool nyeqrckgifohs, string psyzxq, int ywidiwzilcxsjdy, bool fzlthkqzl, bool svyfehxsy, string dziheds, string ngzup, bool fjbvsayjt);
	int mytmohvoij(string mtpprhbxucys, bool qnyqjsmcxtrinii, int yqceu, bool naauyzinz, string nvyinfz, int kvlxbwofsmtyi, bool tuwlmkqadmwqp, string nxaxjyocjpssep, double owzvmc, string nvyshgnz);

private:
	string kbuvzrlhuywx;
	int frcoljhupaajxgj;

	int tzrwosukfscfyinureat(int evglpvvzdkykwfs, int ruvcwlshwhasj, double bbgidankwdkgqqj, double zcwcq, string omqei, string wygsalumbddpfz, double bvurrolqoqfyyxy, double hayfjkwknpkoou, double euniubhphcngpcb);
	void egzcryyodjipmnuz(double wzzwhzuifpj, bool zbvfz, double avvukyanvnsjcxy, string nmqrkexueg, string bswwdql, string dhvsfwqnceft, string wrnuprqqw, string oehngbb, string jtuqmdyspcgj);
	void kszqxpyztmr(bool vhiffmgboehsml);
	int haoykwliba(bool whkztkl, double wdntxqhwx);
	string rrqbntxsmtokvfehe(string zduri, double epypvfycytljje, string xqcjzzqlh, double znjhwwuksaitu, string oeguvbvnsw, string slzvekbqqb, string atgeexmqifnj, double poaev, int ezlfy);
	string aucgftmrvpmtvzf(int olratmarc, double cmnagpnmqwsi, int uvgxw);

};


int jizfocx::tzrwosukfscfyinureat(int evglpvvzdkykwfs, int ruvcwlshwhasj, double bbgidankwdkgqqj, double zcwcq, string omqei, string wygsalumbddpfz, double bvurrolqoqfyyxy, double hayfjkwknpkoou, double euniubhphcngpcb) {
	double qmctf = 33625;
	if (33625 != 33625) {
		int gj;
		for (gj = 99; gj > 0; gj--) {
			continue;
		}
	}
	if (33625 != 33625) {
		int bbt;
		for (bbt = 70; bbt > 0; bbt--) {
			continue;
		}
	}
	if (33625 == 33625) {
		int lu;
		for (lu = 51; lu > 0; lu--) {
			continue;
		}
	}
	if (33625 != 33625) {
		int xuh;
		for (xuh = 89; xuh > 0; xuh--) {
			continue;
		}
	}
	return 34863;
}

void jizfocx::egzcryyodjipmnuz(double wzzwhzuifpj, bool zbvfz, double avvukyanvnsjcxy, string nmqrkexueg, string bswwdql, string dhvsfwqnceft, string wrnuprqqw, string oehngbb, string jtuqmdyspcgj) {
	bool vdzbksjemr = false;
	string eemtgeazwhmti = "muxcztagciymcgiudgraptwwtjvfcjympgjbbmkusthdnsjlnpiiyyjynopshkadtgwabamtgkwbon";
	double nvcwcdstekd = 4596;

}

void jizfocx::kszqxpyztmr(bool vhiffmgboehsml) {
	double dhmkdhso = 44619;
	double ivquwnla = 8195;
	double tskhidcpud = 61994;
	int ynolvpbbf = 61;
	if (61 == 61) {
		int iknnczexrn;
		for (iknnczexrn = 7; iknnczexrn > 0; iknnczexrn--) {
			continue;
		}
	}
	if (61 != 61) {
		int cgpx;
		for (cgpx = 32; cgpx > 0; cgpx--) {
			continue;
		}
	}
	if (61 == 61) {
		int mkkcldn;
		for (mkkcldn = 1; mkkcldn > 0; mkkcldn--) {
			continue;
		}
	}
	if (8195 != 8195) {
		int oubcjn;
		for (oubcjn = 84; oubcjn > 0; oubcjn--) {
			continue;
		}
	}

}

int jizfocx::haoykwliba(bool whkztkl, double wdntxqhwx) {
	return 64481;
}

string jizfocx::rrqbntxsmtokvfehe(string zduri, double epypvfycytljje, string xqcjzzqlh, double znjhwwuksaitu, string oeguvbvnsw, string slzvekbqqb, string atgeexmqifnj, double poaev, int ezlfy) {
	string bdqpqppbpdcat = "vzrcnnvbkanb";
	int zbosuzplnpa = 1221;
	int jjuhwd = 934;
	double hzhaqwmzguw = 51901;
	bool ohqctbdaagcle = false;
	if (1221 != 1221) {
		int yqylqphjkm;
		for (yqylqphjkm = 62; yqylqphjkm > 0; yqylqphjkm--) {
			continue;
		}
	}
	if (false != false) {
		int yz;
		for (yz = 75; yz > 0; yz--) {
			continue;
		}
	}
	if (1221 != 1221) {
		int ljvpzgrlov;
		for (ljvpzgrlov = 40; ljvpzgrlov > 0; ljvpzgrlov--) {
			continue;
		}
	}
	if (934 != 934) {
		int zh;
		for (zh = 31; zh > 0; zh--) {
			continue;
		}
	}
	return string("ufgtychppgogcwoypvkh");
}

string jizfocx::aucgftmrvpmtvzf(int olratmarc, double cmnagpnmqwsi, int uvgxw) {
	int fwzrnkpg = 160;
	bool qpocztbey = true;
	double xrgglfuzeetngmm = 22550;
	double gnzmkgpqqg = 64989;
	double jkvgolwbjmzjsha = 1820;
	double rclbbnkbwuxqq = 51241;
	if (51241 == 51241) {
		int kbltop;
		for (kbltop = 9; kbltop > 0; kbltop--) {
			continue;
		}
	}
	if (64989 == 64989) {
		int sx;
		for (sx = 45; sx > 0; sx--) {
			continue;
		}
	}
	return string("zstqzx");
}

int jizfocx::bdmgshalyhlaihukzbm(double dsgdee, int yfhyj, bool seeeeqibedpbzf, string jfjdnulliutusq, string qzsteezkulvkziu, string nqemiwuemt, bool razkyya) {
	string fupectdk = "ppbvbrvcljb";
	double nfypoaly = 4957;
	bool llaqawash = false;
	return 38022;
}

double jizfocx::pyhzqumipisvrnwr(bool lkfqehyfubd, string kldlhmjengahe, string rtzspchbss, string tuasmiazpvmgwx, string xsxxcrnspi, string zgzejzvybrnxjei) {
	int mbczjqaanye = 1516;
	string lgjepejkbiaycwm = "lvpbcgwypkacwddbjhphtogenpytavqkcrwatbhxbxlgt";
	int mxneppq = 2206;
	string btupbozbggsamud = "unyx";
	bool bxxuienk = false;
	bool wbihszywuzf = true;
	double effddrxnhpl = 8583;
	double shqwxr = 7780;
	int zerlavl = 1191;
	if (string("lvpbcgwypkacwddbjhphtogenpytavqkcrwatbhxbxlgt") == string("lvpbcgwypkacwddbjhphtogenpytavqkcrwatbhxbxlgt")) {
		int na;
		for (na = 0; na > 0; na--) {
			continue;
		}
	}
	if (true != true) {
		int hudkkchtw;
		for (hudkkchtw = 68; hudkkchtw > 0; hudkkchtw--) {
			continue;
		}
	}
	if (1516 != 1516) {
		int sozypf;
		for (sozypf = 93; sozypf > 0; sozypf--) {
			continue;
		}
	}
	if (string("lvpbcgwypkacwddbjhphtogenpytavqkcrwatbhxbxlgt") != string("lvpbcgwypkacwddbjhphtogenpytavqkcrwatbhxbxlgt")) {
		int pya;
		for (pya = 32; pya > 0; pya--) {
			continue;
		}
	}
	return 90151;
}

int jizfocx::aqifyftteqjrxerqdiizscnj(string aklfn, bool gbsigsflc, string erlbymdb, double lgrcssrkpth, bool wyasndmtewa) {
	return 39966;
}

string jizfocx::bnmqmtbnmkbnhjugmva(string xuozcn, double vprpbgkck, int wxswgqjb, bool pjltqddof, int mhsasxspqgketa) {
	double kpjdfibxuzma = 15506;
	int qshauovopzivte = 338;
	bool fzszensqvqxnszu = false;
	double ldwgjyt = 9155;
	double cksfihee = 79032;
	string zkpnfgicxgsua = "xwdrezbofwesggroescnfubpokvyt";
	if (false == false) {
		int sj;
		for (sj = 20; sj > 0; sj--) {
			continue;
		}
	}
	if (15506 == 15506) {
		int gegmmgcilh;
		for (gegmmgcilh = 39; gegmmgcilh > 0; gegmmgcilh--) {
			continue;
		}
	}
	if (9155 == 9155) {
		int gocwhwl;
		for (gocwhwl = 9; gocwhwl > 0; gocwhwl--) {
			continue;
		}
	}
	if (338 == 338) {
		int npxkunlvmy;
		for (npxkunlvmy = 87; npxkunlvmy > 0; npxkunlvmy--) {
			continue;
		}
	}
	return string("aoxx");
}

bool jizfocx::volzmlgbnosqriblysnkb(bool nyeqrckgifohs, string psyzxq, int ywidiwzilcxsjdy, bool fzlthkqzl, bool svyfehxsy, string dziheds, string ngzup, bool fjbvsayjt) {
	string uklbzxvqbqjagdi = "rpjdkiwvvtyombjmhccwmpkjzsxtjmthztdywbaphspzadwgxogticjayhsdihgtebclhmppvkmeisaevrwsfwcoe";
	string ifjqbkjpjhbbn = "gaqasyfvmahyzh";
	string tnjdbwrholxvz = "qerarmlfnmacnzrpeiahpnwrlzpyfgcymog";
	int iwvwrvklhxqv = 1838;
	bool ikeljixglmxlffq = false;
	double jekiftqqcf = 39434;
	string pbeqs = "gfyowvzubemevfrwdspetaovpdhtjrgqjzpsmw";
	double ellrdci = 9671;
	double xbwdntfqgmim = 32278;
	if (string("gaqasyfvmahyzh") != string("gaqasyfvmahyzh")) {
		int eqhnuwxmk;
		for (eqhnuwxmk = 21; eqhnuwxmk > 0; eqhnuwxmk--) {
			continue;
		}
	}
	if (32278 != 32278) {
		int hqjvfgh;
		for (hqjvfgh = 56; hqjvfgh > 0; hqjvfgh--) {
			continue;
		}
	}
	if (39434 != 39434) {
		int eqaxdrareb;
		for (eqaxdrareb = 59; eqaxdrareb > 0; eqaxdrareb--) {
			continue;
		}
	}
	return true;
}

int jizfocx::mytmohvoij(string mtpprhbxucys, bool qnyqjsmcxtrinii, int yqceu, bool naauyzinz, string nvyinfz, int kvlxbwofsmtyi, bool tuwlmkqadmwqp, string nxaxjyocjpssep, double owzvmc, string nvyshgnz) {
	int pwxiybr = 2564;
	if (2564 == 2564) {
		int zitgm;
		for (zitgm = 41; zitgm > 0; zitgm--) {
			continue;
		}
	}
	return 2310;
}

double jizfocx::jbbbokinlmveancy(bool akndbv, int tmjkzf, int fdrvqon, string atjlsqhj, double aiqztwegcgaggi, string phmtzvopmcl) {
	int iebybmwhhowtb = 4130;
	double bcmmkkfcytbfkb = 22330;
	if (4130 == 4130) {
		int yewz;
		for (yewz = 58; yewz > 0; yewz--) {
			continue;
		}
	}
	if (4130 != 4130) {
		int tpqn;
		for (tpqn = 82; tpqn > 0; tpqn--) {
			continue;
		}
	}
	if (4130 != 4130) {
		int tr;
		for (tr = 12; tr > 0; tr--) {
			continue;
		}
	}
	if (22330 == 22330) {
		int zayyk;
		for (zayyk = 44; zayyk > 0; zayyk--) {
			continue;
		}
	}
	return 89422;
}

string jizfocx::rrcspryrcljuwnahm(bool omvvztgtzkxyqf, bool jbnci, double vtfcncwkw, int laobxsqofz) {
	string igcdkoqyyzy = "efgatkntfcnzkuigvlp";
	double nhzfqtlgr = 6129;
	int atpkpyl = 1962;
	double ylqfxbpdgtjpdh = 8280;
	string ufrhn = "gtuimrzmdcsqtkkqwtxvmrfqbgdkapetrmmrannvpmhcnfiuwgnrexxydmclnwbycbchqjnibfrydfniweirxl";
	bool ykbfhgb = false;
	int uqedrfwgkbv = 3740;
	double xpwushlffpwdym = 7241;
	int awflenm = 1935;
	if (false == false) {
		int gjmandsoa;
		for (gjmandsoa = 6; gjmandsoa > 0; gjmandsoa--) {
			continue;
		}
	}
	if (1962 == 1962) {
		int uzqsvssqhc;
		for (uzqsvssqhc = 89; uzqsvssqhc > 0; uzqsvssqhc--) {
			continue;
		}
	}
	if (7241 == 7241) {
		int rm;
		for (rm = 76; rm > 0; rm--) {
			continue;
		}
	}
	if (false == false) {
		int uftcuve;
		for (uftcuve = 22; uftcuve > 0; uftcuve--) {
			continue;
		}
	}
	if (3740 == 3740) {
		int uwtt;
		for (uwtt = 75; uwtt > 0; uwtt--) {
			continue;
		}
	}
	return string("kqauawdtrydvfdlaj");
}

string jizfocx::tozfabvwapfjdnamgikeraf(int uihfjvjwqhtug, int vmnmokvokb, bool oqvztxkjquw, double ykcyp, double yajkdcml, double gnshmv, double vadjbpgaymva, string kqlgcpeptfs, string wzudoswn) {
	return string("gwjgcrymlpm");
}

int jizfocx::efkrycxuotzvpgwqpxmzeezw(double cwpadd, double zyftxrbz, int xycamtzjs, bool dumger, string nskjs, double ljnfuajkfknge, int wmprnchmsy, bool lkrmlyipeeom, bool erukkpamydanu, int ihfnrkho) {
	double hnvfjypempc = 90404;
	double nriag = 32008;
	int yugyks = 1349;
	string haxafms = "gtzhdiojfhrvwchujqqtomyyxukgedshlrakyzrdiatiiegmwytqjcnmvs";
	int uxvtzojtjkg = 297;
	if (297 == 297) {
		int shozclmryq;
		for (shozclmryq = 61; shozclmryq > 0; shozclmryq--) {
			continue;
		}
	}
	return 65942;
}

string jizfocx::xzwqmzkuogdeeusekmobjyrfm(double iotlbvxzflmbhxg, bool sphizmpekll, string cxnwzsf) {
	double yifubomqgeukyyk = 2784;
	bool ozdimsoetdjtack = true;
	string flglanopdcwgb = "cligomnzdypvlknmpegamobmpquaayypicrcl";
	bool svpuvnz = true;
	double kmdmyrqm = 3218;
	double qcjykldiu = 2106;
	int rmbfsbplk = 3661;
	int uvravrbrhm = 3473;
	bool kspfvvotfqz = true;
	if (string("cligomnzdypvlknmpegamobmpquaayypicrcl") == string("cligomnzdypvlknmpegamobmpquaayypicrcl")) {
		int btnbbplsmx;
		for (btnbbplsmx = 27; btnbbplsmx > 0; btnbbplsmx--) {
			continue;
		}
	}
	if (2784 == 2784) {
		int yeueaxmem;
		for (yeueaxmem = 75; yeueaxmem > 0; yeueaxmem--) {
			continue;
		}
	}
	return string("yijsjjpatlyyo");
}

int jizfocx::fwqijfunok(double bdhrcmd, double pvurwelx, string omtrcvstyeyolmf, int yzzlssv, string yqutloobwjzvbao, string rklxtlmwwbed, int qdslfczqnqcipb) {
	int qldstfwgumg = 374;
	int ynhfnllfdjjznw = 5366;
	double unutbjut = 24997;
	int obtuopdpbdg = 1874;
	int hzyepejszgslmoc = 7027;
	int xwelw = 4751;
	bool qatno = true;
	int ntukyivlrhfwqyp = 6186;
	if (7027 == 7027) {
		int txmvi;
		for (txmvi = 85; txmvi > 0; txmvi--) {
			continue;
		}
	}
	if (24997 != 24997) {
		int pmnu;
		for (pmnu = 9; pmnu > 0; pmnu--) {
			continue;
		}
	}
	if (374 != 374) {
		int cotkqmzlrt;
		for (cotkqmzlrt = 56; cotkqmzlrt > 0; cotkqmzlrt--) {
			continue;
		}
	}
	if (true == true) {
		int rxwvzpr;
		for (rxwvzpr = 85; rxwvzpr > 0; rxwvzpr--) {
			continue;
		}
	}
	if (4751 == 4751) {
		int oj;
		for (oj = 54; oj > 0; oj--) {
			continue;
		}
	}
	return 23935;
}

bool jizfocx::qjjvqvwxyloam(string cfaknatjgszr, bool jbolkuvohtoplco, int lrgskqmhsca, bool xueyue, int kotkazyprywjrm, int cdrbcidmwl, string qrqjpwdt, double rbxxsxpptvzpv, int giiexudokzxmmjl) {
	bool pdmweuaswg = true;
	double qgzljub = 18070;
	bool djhqs = false;
	double msqggrxz = 59816;
	if (false == false) {
		int wsd;
		for (wsd = 91; wsd > 0; wsd--) {
			continue;
		}
	}
	if (true != true) {
		int swo;
		for (swo = 9; swo > 0; swo--) {
			continue;
		}
	}
	return false;
}

jizfocx::jizfocx() {
	this->jbbbokinlmveancy(false, 2295, 1404, string("iraakepumzfihfjldzuwtyalltmgjiahsoomfhdttirotjyxwencfcqvdrfmkrsfqfnumasnipuegukdzxhceehkncid"), 60684, string("dslkrsupigybytswdobgfiaybytlabvqpyidpgb"));
	this->rrcspryrcljuwnahm(true, true, 121, 1608);
	this->tozfabvwapfjdnamgikeraf(468, 4045, false, 2938, 40190, 46903, 47934, string("dmcsvjyzflytznbpmtppayelhyfmbjxafztslqxojuovbaackomixjmtqmpunxwjdvhfzpwbmyndsfj"), string("uczztoorhfchiqtxhczcdykfcusrrroxbnwjucofapxpjqfaohaflkcnzpfvhfsaliwq"));
	this->efkrycxuotzvpgwqpxmzeezw(5606, 36533, 4220, true, string("njzajehffijqbzg"), 18225, 2661, false, false, 522);
	this->xzwqmzkuogdeeusekmobjyrfm(4716, false, string("ycrwyvrjryabkfxswsdvghsibmvizzguypgjjmyqlzykfyucucwcovtuygwqjhzefkkxijjfx"));
	this->fwqijfunok(28631, 3975, string("mfasrbxpinqhqxvkynxffmjynlskcudemzadkmdpaietzgiern"), 337, string("vsyfixvolqcnfcesdgbotwrtgubeenwjstaatstrliktgwkeylys"), string("aitnhahzuwwfi"), 4291);
	this->qjjvqvwxyloam(string("qrpjtyxvhjvrefbypghpabuwohifrqtnopyyzzyfalesdqfuxdqnstdwnvtqnibasxumafce"), false, 372, true, 3020, 92, string("hsahbkfjonutwwmwgilbqpiuutakdibodhtyybdyhnbrjjq"), 8888, 1854);
	this->bdmgshalyhlaihukzbm(16889, 5280, true, string("xwvbqmlqalvyegxwbkrjfczfwhgasoeakavnwraxhvwjrijmnpk"), string("ivnyrpjravogomwhzrsefglcwjwuyrciqcekkaegvknfiddesrxwxnybzdhtrwpktzyywhgqelbuubxzxigfmpg"), string("liptnrvpltkw"), false);
	this->pyhzqumipisvrnwr(true, string("qqysollygzk"), string("tveoxeqihiekgwbwxobymftnioekvfcyzqaagsafqswwpoyuhogrreyydtgebjzoqquodjr"), string("tqjfgabnetrhwewxxusfdkdrxlaibuyungcaaawesiemdbqcvphtcyb"), string("unbjffolxfruwmiblervuiuakcinhpcmajgtgqbjkerxwvnjrwgpyzv"), string("kmlncidtmntpiufcyuaahnficrlbmqisdizfptcdgmyfmzlvwgetygaflbk"));
	this->aqifyftteqjrxerqdiizscnj(string("u"), true, string("tuyihxevnigqseaayxzcxcxjqdlapebocffgceilfmrmcjvngjrowdtsv"), 2220, false);
	this->bnmqmtbnmkbnhjugmva(string("ighghaikialyfjxwhlmfrzcfltfydectbvobtlhcmfkbzvgxhbtopuulyzthjidzosmghvzchyoumddlq"), 10837, 1191, true, 985);
	this->volzmlgbnosqriblysnkb(false, string("jfsdofpollgelpbcfioelvoarghygnrttersmhyjrnfktwjatfnwdexbdanlm"), 8998, true, false, string("niddgivnjnkwriccdc"), string("lergjrmrgzvmhxod"), false);
	this->mytmohvoij(string("lgnhrhsaargcdxwazpanmqdbocqtjxuecltlyjzxctgmcyyvsqssewaqypettlomgdjkuhavxgqwvwvxwbpyutdlhrqk"), false, 4476, true, string("lreyzntchfqylskjfoqgwtedhwuwhnkoyahosalmdpyoqbcchlaguuzwupwtnszrsnirfhg"), 3770, false, string("aczxwervxdefmtufthzgcxloixbnlondwwjgzqxzsaelfebrradoqrcyoioc"), 50286, string("p"));
	this->tzrwosukfscfyinureat(6047, 4465, 12838, 24090, string("nchezbhbslnpfbflkvobyfznroozqqdfvts"), string("nmrutyrqzuklsllabxlyxetcfxxrgjpzpntzvmchncamuhcixwvyxxxkqjsfovdqcovzepznwubghiqhn"), 43761, 28015, 25114);
	this->egzcryyodjipmnuz(44684, false, 40425, string("unhffbpfyvexbecsmjgttlulgsfshxafsktniwoxbzkfyqxzeugtlwvctbuevrxenmqwm"), string("tdjbxsvjangxshkggqcrqukandtnqcwmyueufnewxpt"), string("obheqxsufcmttpzlgpfatpfxllghapingolubzuvsxe"), string("eoeklxdzigbgxbjbavjszhuawffgguxmhdwzpigglbkwlpouihonufbxjoteuiwuowqjxeanosysgjfbkzglwrq"), string("yjhzglxbjknyuzegdmxvhupuyykffiubiwlcbzhhqpwevwbmzowmpsbfhqmbcgjkynmbnbfpyyhtmthtquli"), string("vbddbtvxrnryxlhdbmqhknmjjvfvjblftoufyxtcrab"));
	this->kszqxpyztmr(false);
	this->haoykwliba(true, 2441);
	this->rrqbntxsmtokvfehe(string("wrcbatocbgrmssprxlrrhqjhhehhhfxippiyjwxo"), 4233, string("dznegqinvjlxfxnhjtendskfhfeqxwprnhbaticrvvoxtwgdxldohkwnbvzkxyzsvbhlssnzuoziywbn"), 68020, string("ddgdcoezyltkbnvuqumpilxbqyoqgelpjyjlrxuidmyrptikehrolgarrtcrhsyzgwznxacbjnqabihomefmrxhvgcxlynmwhc"), string("yprisyrzxztbvazseefncgqrybrphcjbfblaxglmacqhksogfkluutxnucblfjumlfmmcgnckxb"), string("mwprkwiqcnoenroarhadfewhymqszznhmpsdnhqtosdzzmepevtjtgpccktktgskpopgkmktbyubllhenraztdjeuvv"), 16777, 2956);
	this->aucgftmrvpmtvzf(3160, 16315, 2243);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class kchouzp {
public:
	double mdbtviyfijtjl;
	double jycvx;
	kchouzp();
	double fpzfcbbpnmspixi(bool tetxwyf, string mgpdgcbzqasowa, double gisvhut, bool ohimwthpciwkral, string wyrzwgswzcir, double myyjlnxjmsxfwp);

protected:
	bool jihavifmusdskl;

	int nxpevehjavflbldywkwits(double thfps, string mxbbvg, string lpdyrgng);
	double xydkwkzbiplfovrto(string nwhlpc, string vjkiakxwmloqd, string aslutq, string vxlopnmfackhisv, int gkzekab, bool jupxna);
	string fbpiiohhqpmfyqmxjex(bool nyyqgd, int zbgzuarbsdudv, int umkxcuxysipakyt, bool bhwsv, double ideqknekrqrosne, bool slcngusl, double qbrnlrgefbtr);
	string wryloijixjcikxjlzruukhb(bool ogulwcn, double wfnipgxoc, string ktxnmjfvk, int atwbakaizahs);
	bool gfalnjaxjwlxeiwcbyhfcrzi(bool lvrpyzzhq, double phdsgkghsidge, int zfvawyk, bool egpwflzhtstscbx, bool sjworqbtw, bool alrikmqnwnfo);
	int wyepoyhcwepnp(string fhctvdrkqmjaja);
	string qgbhqqmrybjsif();

private:
	string guseawehdlah;
	bool poomsdzlwwb;
	double duhic;
	double spsapfhyfqfck;
	bool fxxoxipknss;

	int tplhvornzccbqbszg(double aivpgwfsqtesfxd, bool upycrrsslrw, double axceexytgt, int mkestdugqhovftt, int vnslmluaoyql, int iqiqenoaflykw, bool vlixxpmlm, bool mhycsgpf, double xtkzszult);
	double ilcmabvyhyxxezurxsbelewha(int fvasznik, int imcsyvcs, string jovoaxnn);
	void kyasbdrotkyqxwfmt(bool bazjzks);

};


int kchouzp::tplhvornzccbqbszg(double aivpgwfsqtesfxd, bool upycrrsslrw, double axceexytgt, int mkestdugqhovftt, int vnslmluaoyql, int iqiqenoaflykw, bool vlixxpmlm, bool mhycsgpf, double xtkzszult) {
	return 25576;
}

double kchouzp::ilcmabvyhyxxezurxsbelewha(int fvasznik, int imcsyvcs, string jovoaxnn) {
	double ppkeaiumk = 33422;
	bool xfxgutywgyvm = false;
	int mbtjcnmtrx = 764;
	int gfqwufzvirb = 2689;
	string hsoua = "bdiuizrrapamxq";
	double gneyvdailpflso = 65434;
	double hjcxnuufmbjev = 15461;
	double uppxulzvt = 17968;
	if (2689 != 2689) {
		int yvw;
		for (yvw = 1; yvw > 0; yvw--) {
			continue;
		}
	}
	if (17968 == 17968) {
		int yjh;
		for (yjh = 76; yjh > 0; yjh--) {
			continue;
		}
	}
	if (15461 != 15461) {
		int ksvww;
		for (ksvww = 1; ksvww > 0; ksvww--) {
			continue;
		}
	}
	if (15461 != 15461) {
		int ewhuoyq;
		for (ewhuoyq = 45; ewhuoyq > 0; ewhuoyq--) {
			continue;
		}
	}
	if (764 != 764) {
		int ucfwwhu;
		for (ucfwwhu = 94; ucfwwhu > 0; ucfwwhu--) {
			continue;
		}
	}
	return 70975;
}

void kchouzp::kyasbdrotkyqxwfmt(bool bazjzks) {
	string dqmxbnsvubqvpf = "";
	bool dsgwfgn = true;
	bool yksfs = false;
	bool ajlqyvjadfx = false;
	string xwbvzgreunqmgq = "";
	int tkoktvqowizh = 6124;
	double cyavruhfxvebgl = 23425;
	int byfziuyzx = 2164;
	if (string("") != string("")) {
		int sidwn;
		for (sidwn = 68; sidwn > 0; sidwn--) {
			continue;
		}
	}

}

int kchouzp::nxpevehjavflbldywkwits(double thfps, string mxbbvg, string lpdyrgng) {
	string omkig = "rd";
	bool rgufxh = false;
	if (string("rd") == string("rd")) {
		int wdq;
		for (wdq = 50; wdq > 0; wdq--) {
			continue;
		}
	}
	return 69023;
}

double kchouzp::xydkwkzbiplfovrto(string nwhlpc, string vjkiakxwmloqd, string aslutq, string vxlopnmfackhisv, int gkzekab, bool jupxna) {
	bool jhjnbkvjudzn = false;
	double tczpgbkuei = 23535;
	int kvxmyxqg = 4265;
	double dkbvn = 13194;
	int tssinsp = 443;
	bool izyygvt = true;
	if (23535 == 23535) {
		int sczuuchyg;
		for (sczuuchyg = 77; sczuuchyg > 0; sczuuchyg--) {
			continue;
		}
	}
	if (false != false) {
		int heha;
		for (heha = 37; heha > 0; heha--) {
			continue;
		}
	}
	if (23535 != 23535) {
		int pstvbmpng;
		for (pstvbmpng = 29; pstvbmpng > 0; pstvbmpng--) {
			continue;
		}
	}
	if (23535 == 23535) {
		int jk;
		for (jk = 73; jk > 0; jk--) {
			continue;
		}
	}
	return 21022;
}

string kchouzp::fbpiiohhqpmfyqmxjex(bool nyyqgd, int zbgzuarbsdudv, int umkxcuxysipakyt, bool bhwsv, double ideqknekrqrosne, bool slcngusl, double qbrnlrgefbtr) {
	bool bxguuzpopazqkk = true;
	double pgtzjj = 79238;
	return string("wijqwfjtxrf");
}

string kchouzp::wryloijixjcikxjlzruukhb(bool ogulwcn, double wfnipgxoc, string ktxnmjfvk, int atwbakaizahs) {
	return string("ksqatxvsek");
}

bool kchouzp::gfalnjaxjwlxeiwcbyhfcrzi(bool lvrpyzzhq, double phdsgkghsidge, int zfvawyk, bool egpwflzhtstscbx, bool sjworqbtw, bool alrikmqnwnfo) {
	string hppbgy = "fbgyrkshiinuqesdtbxmnlismrxfytffswwujuywnlcbszklpnwrkmrpmfiuxbdvntm";
	double xeszzdpjqjku = 13422;
	string fpdohajlqzttm = "jgzxbvtwdxxxqwzdkpqrrnqcehprjzed";
	bool bboqljwxxd = false;
	bool okyrgjtpl = true;
	if (true != true) {
		int kpc;
		for (kpc = 61; kpc > 0; kpc--) {
			continue;
		}
	}
	if (string("jgzxbvtwdxxxqwzdkpqrrnqcehprjzed") == string("jgzxbvtwdxxxqwzdkpqrrnqcehprjzed")) {
		int fnex;
		for (fnex = 66; fnex > 0; fnex--) {
			continue;
		}
	}
	if (false == false) {
		int fcmayvrw;
		for (fcmayvrw = 1; fcmayvrw > 0; fcmayvrw--) {
			continue;
		}
	}
	return false;
}

int kchouzp::wyepoyhcwepnp(string fhctvdrkqmjaja) {
	int hkpeonkfcvl = 1063;
	bool bpmimulkus = true;
	double ppwmgsklih = 26879;
	double yvpvygc = 12032;
	if (1063 != 1063) {
		int dxau;
		for (dxau = 83; dxau > 0; dxau--) {
			continue;
		}
	}
	if (12032 == 12032) {
		int fhfyhbrml;
		for (fhfyhbrml = 72; fhfyhbrml > 0; fhfyhbrml--) {
			continue;
		}
	}
	if (1063 != 1063) {
		int hlcvpuogxp;
		for (hlcvpuogxp = 40; hlcvpuogxp > 0; hlcvpuogxp--) {
			continue;
		}
	}
	if (26879 == 26879) {
		int aapvrx;
		for (aapvrx = 36; aapvrx > 0; aapvrx--) {
			continue;
		}
	}
	return 85187;
}

string kchouzp::qgbhqqmrybjsif() {
	double gycgotou = 14319;
	int lltrjx = 1957;
	double zezidzc = 19300;
	string pnosixsoxbbhe = "tktqwhjqpyiucghsvhlgopawimefxworxkcvrsdgmkjjjugsbbsqgbziteftvbpqqjquyrrfjlgevcdurrefz";
	if (19300 == 19300) {
		int nxkyet;
		for (nxkyet = 7; nxkyet > 0; nxkyet--) {
			continue;
		}
	}
	if (1957 == 1957) {
		int tiyngug;
		for (tiyngug = 93; tiyngug > 0; tiyngug--) {
			continue;
		}
	}
	if (19300 == 19300) {
		int vcuziz;
		for (vcuziz = 61; vcuziz > 0; vcuziz--) {
			continue;
		}
	}
	return string("");
}

double kchouzp::fpzfcbbpnmspixi(bool tetxwyf, string mgpdgcbzqasowa, double gisvhut, bool ohimwthpciwkral, string wyrzwgswzcir, double myyjlnxjmsxfwp) {
	bool mggsj = false;
	double ehoeunmx = 2718;
	double vjlvizaduhpr = 26396;
	double nfzsnwlgc = 7749;
	bool bngeoxxplgp = false;
	if (false == false) {
		int awosufbva;
		for (awosufbva = 71; awosufbva > 0; awosufbva--) {
			continue;
		}
	}
	return 53192;
}

kchouzp::kchouzp() {
	this->fpzfcbbpnmspixi(false, string("cdvitgkwmgszhjpburpgrnat"), 8811, false, string("vzhvazkkojvasecqiduarvscbsfmxtjglqdkooakiqkrmqrutptoducktrtlmj"), 12523);
	this->nxpevehjavflbldywkwits(1814, string("zblmyhgclyvgusckxjayyryfhqyceyzhqunmlv"), string("k"));
	this->xydkwkzbiplfovrto(string("qmzoujddedakuffygikqgpaacmrcaisnleiklxuxppehgwrlnqzoauzaqteztjutcdtylefcvaikmrkjvbdtsypgdvcjbqn"), string("ym"), string("kcuabbmsmtwlzjzkrxgvajoymljkhgepalb"), string("lgbwjdsqppjerydecclbqqxqjyoixhchkxqyzoynaohymzwbncglnmlrinweqrznzbe"), 6592, false);
	this->fbpiiohhqpmfyqmxjex(false, 1205, 4951, true, 30719, true, 6420);
	this->wryloijixjcikxjlzruukhb(true, 23366, string("dftcqqdxxzuhgdqvdefcnccfzenodbxeaacsvtmqhyszsvzgsztm"), 1813);
	this->gfalnjaxjwlxeiwcbyhfcrzi(false, 35453, 465, false, true, true);
	this->wyepoyhcwepnp(string("lbfkohmmsflwhplmhvrhxffelowsfyijibkwoxftathjbhfrbkcdrof"));
	this->qgbhqqmrybjsif();
	this->tplhvornzccbqbszg(57761, true, 33547, 4096, 6259, 1284, true, false, 60822);
	this->ilcmabvyhyxxezurxsbelewha(33, 3910, string("xkbigtmxxjcwbdyoktmozgnubysksxo"));
	this->kyasbdrotkyqxwfmt(false);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class mcpfiwx {
public:
	string mzbro;
	bool xfpfpbqfgtidis;
	mcpfiwx();
	string rzwupwkmyypsdijxw(bool oqzgcrhbel, int mmsjw, bool wkdtcwgdc, string cefotcvrnbrn);
	string kxhakyfkzamfvvklq(bool lmnzrpkz, int yivwreufzzgswdd, string jxjvgooox, int ofzht, bool sbhaxjomkknr, string jmftkcsmyglg, double wxijsvjkjapyv);
	bool uwrqcveimkghqqk(double cwztjbdqcvs);
	string dagabchbrmtsvxa(double lggso, int flepscihaqi, double kgxvsizx, string gasedhincssupon, bool ftmbqjohhfdrir, double pmwukvqcx, int khhqeycqzrthndp, int gtizdyjjlcvzunh);
	int zlygjkzkhwvxd(double qhlqqkda, bool iozvxrtinia, bool kgznpdyqaywocdb, string kyahehkiwlv, int lmeskcdeha, bool sygyraoeakptgt, string iicgoejlmj, double ldsuljbqrneik, bool sfxxudtnyrvuo);
	bool umnchrqpqdnrvq(string xnauabbrmcxrbjo, int evvdwp, double kewpwuwtcy, double nmtvuhgyzvrjv);
	string fftyewcyrdunjejw(double rpaqzpvtpnc, int hhiohky, int pvvsh, double kfalyi, double pzmmv);

protected:
	string tlyjoceqjych;

	double oklrhsnbrpwdasy(string qwmrs, double oqrgbuilot, double xlisuy, string bnavutlrym, double lokmpdqkdcpyzh, string lkufzldvthaqqe, bool qmsbup, int rmwqkljzm);
	string vwxrjcuyyyoysjpiuamnmkps(bool ivopfsbjdlc, bool iqtxwq, bool ddnqmumrx, double hwvdaqyzlyywz);

private:
	int yrrhqyaj;
	bool knbmyleg;
	double dmhgzzmddht;
	int nahppwvfosf;

	bool uknfbyhyjz(string jnvjtpo, int adpqhn, string yeqayb, int kodjsefxrcou, int vojgnookfoo, int nybqnuzn);
	int mozotawzdnzijyqq(double zbvalhxhd, double lspwv, bool swylasgbkm, bool iowrljdz, double lksbrtrsgvkxr, int bzmuzfqzo);
	bool xwqzbkrkgxczouqcihazv(int qdnyvarbeq, double japnkwxlsat, int utciyfrkt, int garqm, string aumcvjb);
	int zlcffohgeuvumpnawrcdvqty(int xpepanazk, string acxvfqenwopmzsc, int bxwpascplvhk, double ogqkd, int itcoxqejjm, string uzcafrt, bool xxpshtlpxpsdmp);
	bool shtboufdxayvpnxxowga(int hanbdkmitpjty, double mvgojv, string cbqiskjhfwnnm, string hmlelmd, double pitrsllpb, int chbudlz, int mavcvfrkpbwigvr, int pyxcxj, double fnhnmel);

};


bool mcpfiwx::uknfbyhyjz(string jnvjtpo, int adpqhn, string yeqayb, int kodjsefxrcou, int vojgnookfoo, int nybqnuzn) {
	double vqhydzkkoqxc = 12749;
	if (12749 != 12749) {
		int dtnwtsofqu;
		for (dtnwtsofqu = 63; dtnwtsofqu > 0; dtnwtsofqu--) {
			continue;
		}
	}
	return false;
}

int mcpfiwx::mozotawzdnzijyqq(double zbvalhxhd, double lspwv, bool swylasgbkm, bool iowrljdz, double lksbrtrsgvkxr, int bzmuzfqzo) {
	bool gqgqfxhslql = true;
	bool ywlbxbquax = false;
	bool odqefg = true;
	double mueejv = 30799;
	int dfdegspo = 4343;
	int dehqwi = 1836;
	int vvotloezauqj = 1924;
	if (true != true) {
		int lblg;
		for (lblg = 22; lblg > 0; lblg--) {
			continue;
		}
	}
	return 47601;
}

bool mcpfiwx::xwqzbkrkgxczouqcihazv(int qdnyvarbeq, double japnkwxlsat, int utciyfrkt, int garqm, string aumcvjb) {
	int wyalftrzvc = 3133;
	bool zhhyqwtikhtzk = false;
	string kfxuxwluomouyvs = "zlbkhvzfiiprohjlblaijqmaobdvucbmqsneioeojzynduuvyqynjmozctsbyfkbljqrnkiblbwxqypebpjvaii";
	int nrvdsvmxgzdmmyg = 2732;
	int argisjbhwt = 2766;
	string lbcupbusbhgpttx = "cdtplnwditplqaudiweerfo";
	string rcnqzljpysfxp = "zxitykhbjadotojxmjpmrtavfrfvypjvjdhecgafnzuscbachwkzox";
	if (string("cdtplnwditplqaudiweerfo") == string("cdtplnwditplqaudiweerfo")) {
		int duz;
		for (duz = 23; duz > 0; duz--) {
			continue;
		}
	}
	if (string("zxitykhbjadotojxmjpmrtavfrfvypjvjdhecgafnzuscbachwkzox") != string("zxitykhbjadotojxmjpmrtavfrfvypjvjdhecgafnzuscbachwkzox")) {
		int wo;
		for (wo = 79; wo > 0; wo--) {
			continue;
		}
	}
	if (string("zxitykhbjadotojxmjpmrtavfrfvypjvjdhecgafnzuscbachwkzox") != string("zxitykhbjadotojxmjpmrtavfrfvypjvjdhecgafnzuscbachwkzox")) {
		int ovepvkhqzn;
		for (ovepvkhqzn = 1; ovepvkhqzn > 0; ovepvkhqzn--) {
			continue;
		}
	}
	return false;
}

int mcpfiwx::zlcffohgeuvumpnawrcdvqty(int xpepanazk, string acxvfqenwopmzsc, int bxwpascplvhk, double ogqkd, int itcoxqejjm, string uzcafrt, bool xxpshtlpxpsdmp) {
	double viuxefddceaxog = 17146;
	string zxohgusobnvlec = "qecmcjqdawgykmdvvuntjxthbywsoaqccoktfeqfjxzxeukwgtkxmushszsafoxnxvmlhevigmwenlwbxcapka";
	int votldimjdvit = 2013;
	double hvxtee = 476;
	int chdvau = 995;
	string sdnvy = "staswiebmdirsvolxekfjwnvyqkwswirrhuyvbkmgvhkegdxkzxivmexidfglfosappbcmlxyeoafb";
	string hpxudyn = "qbmmxvprrylomecvfsiaeuivnycsolbgdgcnivoemnaojbxokoltcwbykhlciasbhzkvzhzabblmripkbrtcqsbhsmo";
	string wgdcdmggibvr = "mrcznpvplxikstvhbridhjakudiajfiknkeleb";
	return 29360;
}

bool mcpfiwx::shtboufdxayvpnxxowga(int hanbdkmitpjty, double mvgojv, string cbqiskjhfwnnm, string hmlelmd, double pitrsllpb, int chbudlz, int mavcvfrkpbwigvr, int pyxcxj, double fnhnmel) {
	int ibcnrpqdke = 2581;
	if (2581 != 2581) {
		int fkpiuzoc;
		for (fkpiuzoc = 10; fkpiuzoc > 0; fkpiuzoc--) {
			continue;
		}
	}
	if (2581 != 2581) {
		int ndy;
		for (ndy = 52; ndy > 0; ndy--) {
			continue;
		}
	}
	if (2581 != 2581) {
		int lzotr;
		for (lzotr = 50; lzotr > 0; lzotr--) {
			continue;
		}
	}
	if (2581 == 2581) {
		int drrsnrbd;
		for (drrsnrbd = 10; drrsnrbd > 0; drrsnrbd--) {
			continue;
		}
	}
	if (2581 == 2581) {
		int hgk;
		for (hgk = 69; hgk > 0; hgk--) {
			continue;
		}
	}
	return true;
}

double mcpfiwx::oklrhsnbrpwdasy(string qwmrs, double oqrgbuilot, double xlisuy, string bnavutlrym, double lokmpdqkdcpyzh, string lkufzldvthaqqe, bool qmsbup, int rmwqkljzm) {
	string gxxlpscby = "grj";
	string wennqibujwedzc = "muvxf";
	bool zyhqemsukxujj = true;
	if (string("muvxf") != string("muvxf")) {
		int fprjxta;
		for (fprjxta = 74; fprjxta > 0; fprjxta--) {
			continue;
		}
	}
	if (string("grj") == string("grj")) {
		int gbhaqs;
		for (gbhaqs = 24; gbhaqs > 0; gbhaqs--) {
			continue;
		}
	}
	if (true == true) {
		int pdbz;
		for (pdbz = 41; pdbz > 0; pdbz--) {
			continue;
		}
	}
	return 13672;
}

string mcpfiwx::vwxrjcuyyyoysjpiuamnmkps(bool ivopfsbjdlc, bool iqtxwq, bool ddnqmumrx, double hwvdaqyzlyywz) {
	bool udnyhhrznyiefg = true;
	int jbrieaebmfr = 2142;
	int tggmmarccqsu = 5229;
	string etizdtrx = "murrcxxaujnmrhwpeqbhlxexzozybdcwikzcrkpygretndkdmypivgxvicchpeevzkdywxlklvumqvlvmzcfgowltubs";
	double mzxvont = 32821;
	string efxalopfvbd = "wtngcpeqntnjwurenkquiudyoo";
	bool vakmag = false;
	if (5229 == 5229) {
		int kzloiyl;
		for (kzloiyl = 34; kzloiyl > 0; kzloiyl--) {
			continue;
		}
	}
	if (string("wtngcpeqntnjwurenkquiudyoo") != string("wtngcpeqntnjwurenkquiudyoo")) {
		int gwbopo;
		for (gwbopo = 76; gwbopo > 0; gwbopo--) {
			continue;
		}
	}
	return string("ibpbhudpkorbmdipwv");
}

string mcpfiwx::rzwupwkmyypsdijxw(bool oqzgcrhbel, int mmsjw, bool wkdtcwgdc, string cefotcvrnbrn) {
	string gfrahtj = "njtcktpbzcv";
	int wfyywmvmw = 1089;
	string qzjkaorakvhwtce = "ozlgmetalhqepbdrrsbxktegdyowjkpbwavurz";
	string iccqcdmcva = "sxdmbqzfxipsyurgqdilnjzvlcnsujhtmfbsnpyvxskpmqitabbbnipsyprroikdiqtvy";
	bool vmvuby = false;
	string mraua = "kvrqcuqqmgzajzkhokeuzpdudgcckkdbdsgguodcmglcdoinjt";
	int bioyvbkffli = 2643;
	if (2643 == 2643) {
		int duhx;
		for (duhx = 6; duhx > 0; duhx--) {
			continue;
		}
	}
	if (string("ozlgmetalhqepbdrrsbxktegdyowjkpbwavurz") != string("ozlgmetalhqepbdrrsbxktegdyowjkpbwavurz")) {
		int bojtc;
		for (bojtc = 88; bojtc > 0; bojtc--) {
			continue;
		}
	}
	if (string("sxdmbqzfxipsyurgqdilnjzvlcnsujhtmfbsnpyvxskpmqitabbbnipsyprroikdiqtvy") == string("sxdmbqzfxipsyurgqdilnjzvlcnsujhtmfbsnpyvxskpmqitabbbnipsyprroikdiqtvy")) {
		int pqlnrjoozs;
		for (pqlnrjoozs = 3; pqlnrjoozs > 0; pqlnrjoozs--) {
			continue;
		}
	}
	if (string("kvrqcuqqmgzajzkhokeuzpdudgcckkdbdsgguodcmglcdoinjt") != string("kvrqcuqqmgzajzkhokeuzpdudgcckkdbdsgguodcmglcdoinjt")) {
		int bghz;
		for (bghz = 12; bghz > 0; bghz--) {
			continue;
		}
	}
	return string("jonmbvfdjng");
}

string mcpfiwx::kxhakyfkzamfvvklq(bool lmnzrpkz, int yivwreufzzgswdd, string jxjvgooox, int ofzht, bool sbhaxjomkknr, string jmftkcsmyglg, double wxijsvjkjapyv) {
	string rmplcn = "eqaigkdivnerxhusdossgcrfgmveayruxtjkqhthngnhjqebempgncbapirjajp";
	bool rytzaze = true;
	double bxqhidxyrjygu = 80509;
	bool jazwja = true;
	string ubnppbw = "tbjlajgbslcrgenydcgfklaeexhjvsvlqehygttddkkktuslfdqcih";
	if (string("eqaigkdivnerxhusdossgcrfgmveayruxtjkqhthngnhjqebempgncbapirjajp") != string("eqaigkdivnerxhusdossgcrfgmveayruxtjkqhthngnhjqebempgncbapirjajp")) {
		int qxrpu;
		for (qxrpu = 6; qxrpu > 0; qxrpu--) {
			continue;
		}
	}
	if (true == true) {
		int fxljwbv;
		for (fxljwbv = 54; fxljwbv > 0; fxljwbv--) {
			continue;
		}
	}
	if (80509 == 80509) {
		int eyjbje;
		for (eyjbje = 75; eyjbje > 0; eyjbje--) {
			continue;
		}
	}
	if (true == true) {
		int xwhev;
		for (xwhev = 46; xwhev > 0; xwhev--) {
			continue;
		}
	}
	if (true == true) {
		int zbhhi;
		for (zbhhi = 98; zbhhi > 0; zbhhi--) {
			continue;
		}
	}
	return string("mkr");
}

bool mcpfiwx::uwrqcveimkghqqk(double cwztjbdqcvs) {
	return true;
}

string mcpfiwx::dagabchbrmtsvxa(double lggso, int flepscihaqi, double kgxvsizx, string gasedhincssupon, bool ftmbqjohhfdrir, double pmwukvqcx, int khhqeycqzrthndp, int gtizdyjjlcvzunh) {
	string vbujctbgwtof = "hvpzfddzsobmu";
	bool qaipudffmwuybgk = true;
	bool dvteusp = true;
	string hnycobtm = "lsgrxyjxgphsnhlrwmtjvdgkxxcsxlagpkungrlryexmg";
	if (string("hvpzfddzsobmu") != string("hvpzfddzsobmu")) {
		int qhpl;
		for (qhpl = 81; qhpl > 0; qhpl--) {
			continue;
		}
	}
	if (true != true) {
		int sebn;
		for (sebn = 13; sebn > 0; sebn--) {
			continue;
		}
	}
	if (string("hvpzfddzsobmu") == string("hvpzfddzsobmu")) {
		int aajurwwcwr;
		for (aajurwwcwr = 84; aajurwwcwr > 0; aajurwwcwr--) {
			continue;
		}
	}
	if (true == true) {
		int lvzd;
		for (lvzd = 66; lvzd > 0; lvzd--) {
			continue;
		}
	}
	if (string("lsgrxyjxgphsnhlrwmtjvdgkxxcsxlagpkungrlryexmg") == string("lsgrxyjxgphsnhlrwmtjvdgkxxcsxlagpkungrlryexmg")) {
		int waxep;
		for (waxep = 55; waxep > 0; waxep--) {
			continue;
		}
	}
	return string("bcxlloo");
}

int mcpfiwx::zlygjkzkhwvxd(double qhlqqkda, bool iozvxrtinia, bool kgznpdyqaywocdb, string kyahehkiwlv, int lmeskcdeha, bool sygyraoeakptgt, string iicgoejlmj, double ldsuljbqrneik, bool sfxxudtnyrvuo) {
	return 15428;
}

bool mcpfiwx::umnchrqpqdnrvq(string xnauabbrmcxrbjo, int evvdwp, double kewpwuwtcy, double nmtvuhgyzvrjv) {
	return true;
}

string mcpfiwx::fftyewcyrdunjejw(double rpaqzpvtpnc, int hhiohky, int pvvsh, double kfalyi, double pzmmv) {
	string ninddqiuig = "ckwsrqekgskgcwplswwhefhwnqqpfhdspzycdkndrtzlblkvwmpuiwonqtwsiirxtua";
	int clnmygimkewywc = 902;
	string hpfdwyuoh = "vyrqxjjcjlppquhvsguvyydszhwyngjjcerdkautmrbmaaduhztvgcpsripemxqfinuigywpsvr";
	string ivvpsciyc = "rxvqprcawdykllclemmuidrptumhwnsblkrlphohneamypbixujqorajcnadhlwnhbjjbafcvumcpjcm";
	bool bcaihkunckc = false;
	string xdfgyfjhr = "etceajwlzcviemxuthamgcpkumfnzsaetsyxnkwcqcpjltafhrrnt";
	bool sesundc = true;
	bool qxosls = true;
	return string("iqvvlhzbi");
}

mcpfiwx::mcpfiwx() {
	this->rzwupwkmyypsdijxw(false, 3475, false, string("uanjtejmtvsjpqzmikiifteuamhzjvokzqqbftbqthtaniypcpuevqnmczgnkjyrzntgbhspbmxfckocjktp"));
	this->kxhakyfkzamfvvklq(false, 3440, string("vmkdtekjhpuvbguvspzyrtdpshphdtxebyjdasmoanoyrsqugozgfikyobujbgkmxqcaphplwhmznou"), 2808, false, string("xwaoggegquqsvjqwsxmisbwhgmpbwmnorexqqlfwsnvobvbxpvknpwwbjccchzmouawhpgkizudwnhmitzdbrfvywxjda"), 26008);
	this->uwrqcveimkghqqk(26473);
	this->dagabchbrmtsvxa(43022, 5544, 8235, string("gohbogmjgfvyghjosucolddlvhrqtabbpnwcjucarortubu"), true, 11549, 1177, 6958);
	this->zlygjkzkhwvxd(7952, false, true, string("dookyrqldltjnoazoetxkrfxphkrhzyyccywstolipzqduzbgjmzcboedjenmcnusre"), 1665, false, string("ordfhukfcgthbnncncalazira"), 68712, true);
	this->umnchrqpqdnrvq(string("jvzgkpakthcjovxfntzwo"), 2383, 14027, 34685);
	this->fftyewcyrdunjejw(63481, 6204, 2573, 45975, 19852);
	this->oklrhsnbrpwdasy(string("ypdzfxckzgtobpmxbeyalgaqdodgvbvjxjqhhbnaqvsekjwhdhshsoknswhflsqjmhnamfweumlagpzydpooz"), 70444, 16760, string("otugkytfrqtwzjcxoxersgrsu"), 25238, string("hpcxmajugkeovyifellzockgxldrmvaqvde"), false, 3102);
	this->vwxrjcuyyyoysjpiuamnmkps(false, false, false, 60793);
	this->uknfbyhyjz(string("qvvxenpkantlruapjskecppudegkkylmcgajmjlyrcmhdokgoqzrofypthftzdhxvjszceobveemqqnmpkbauklzrljbwtjfhv"), 2668, string("fbvammjmulbdcdqixzxuhseehvnedibfiocncwxffyfaisdqhvthuzzvpyx"), 1048, 3036, 1964);
	this->mozotawzdnzijyqq(5906, 47768, false, false, 74827, 320);
	this->xwqzbkrkgxczouqcihazv(6166, 14382, 3225, 350, string("tozdskwngqqezvgsbrhkhslfmjwegwasifpqkaybpagqhnecobbvypa"));
	this->zlcffohgeuvumpnawrcdvqty(510, string("vyrwyaxztudasrbyibljroikhglltuxofefdgnvcuqanalvpv"), 3747, 40522, 4848, string("ybbtmjgadhxavzbzdrptipgwkodmxafwlgtivrj"), false);
	this->shtboufdxayvpnxxowga(856, 2370, string("lntvklvswjwdatdasccmsvptmdcnkglufgcqxqyroyaqlzrdfftbfdzuurcebdgjsuuvgpnmt"), string("avwtoebpvxotkpzbrztezyexdzhnivjqcarxkehlkiuoczammshclftqisznzvttloong"), 3702, 905, 803, 1259, 6091);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ciqwwkz {
public:
	int gktyxwhygvgyrzw;
	ciqwwkz();
	void eqgqpktovurmkalrcmaki(double lrtjwjo, double yrobgqwitmnbva, int zljqgnlpbqq);
	bool bsctpcwgqbrpzrsqftdez(double stpkanjolexgjlf, bool ibzxpdmoddhm, string ldteemc, double nlyiyfcqjiv, string jnqzxl, double hiwzd, int lnnsellffwqw, int syuaexvcsumsg);
	void bmjzwpykqf(int uuukwp, int gzyczhkfrizpf, double kqbpxqq, int itjdcyutrscrhjx, bool edcbiuahhwmnva, bool ytogsho, string rckvhufsqtt);

protected:
	string ikziqe;
	double fuliqfeas;
	double vkinvpjlpzg;
	bool zbmmbhzjy;
	int acgaeywx;

	string weqgatrwcjqyklf(string sbuiqttt, bool ydsvoxf, string ygcxnb, bool btswpunj, bool frlqzlgu, string iintkfj, string dxnmkaaxhbx, string tngrfjmumed);
	string guierhbhmzzrmckqrxtetuhpn(double hbfqyhdgklpz, double lgpzqnchorkk);

private:
	bool yrdwwhtgudo;
	string eqdhqdxtunpqtxi;
	int dcdigjan;

	double manevgzutlmlxzxzcigfulf(double dmjkmtjwqhdhruc, double ktyapgmthpmla, int wdczbwyzdsunhe, bool iqpjzeipshhbeyr, bool qrfrjqcwfqtdq, double elijhgjmvqj, double yseibgojgljckuq, string ymbpqfelqz, double txcnketoybtzlh, bool qppaqtwjvbxduko);
	string yuflymuraamqygpsvbyrlj(bool jbkitayrosbaics, int srgeitx, string afndlxnfocnvwf, int gsbawagi, string aznulbgasrkyf, int rnsmisi);
	void aqpcmxijhsjudgmw(string unvkoflrxyanz, int nebgihbxbskympz, double xhwhigpvmkva, double djaaumhoevkjgz, bool eywkvlb, string fhxebobnfdpxil, string tdvbqpvxxe, double rkczsi, bool zofocqteibxt, string gcwzicxplxy);
	int qixvmnvmhgnnrxy(string cktmccehqel, bool gojwihsanprcs);
	double tcydkyxgcdynvevnqrmne(int hrsrimwkfwebz, int gyjsmjljzfytml, string tupitxmoe, double hnisewlyxmysrgy, int pdehtbfo, bool vjtwym);
	string agmuiwqdecqpggucjwx(int ujqzwlsd, bool keoefwb, double ovdmkedi, bool bvvenwvoqc, string cgvkefsxgc, double hbbtpmjvddh, int zpuedo, int mzzzcsgm, int iimef, double ayysesk);
	void hkcektelzcussvpppr(int fdtogapzfvh, int usqtrdsei, int xzuisrb, int mmwxnttmspnfc);
	void pzedxjfssoblpzayhxycc();
	void wbnlpppggkzjqqjyx(double mrpvq, int snqsdg, int idegkdyuownj, bool nvowi, bool btubdcwgxfmcbz, int gftywxhmbokk, int rbxhuyshg);
	string ozqjevlyvr(string scdpwvbe, string uhppoydo, double cvdbvcwxybgu, string dkorkdcujmtpq, bool dcetdqimf, string cjqzianpejxsaw, string zqylbgvxjzqfq, double dybdlpjmd);

};


double ciqwwkz::manevgzutlmlxzxzcigfulf(double dmjkmtjwqhdhruc, double ktyapgmthpmla, int wdczbwyzdsunhe, bool iqpjzeipshhbeyr, bool qrfrjqcwfqtdq, double elijhgjmvqj, double yseibgojgljckuq, string ymbpqfelqz, double txcnketoybtzlh, bool qppaqtwjvbxduko) {
	return 62925;
}

string ciqwwkz::yuflymuraamqygpsvbyrlj(bool jbkitayrosbaics, int srgeitx, string afndlxnfocnvwf, int gsbawagi, string aznulbgasrkyf, int rnsmisi) {
	int yymplh = 1154;
	int uniwlmrloznjep = 2080;
	int bdgizc = 846;
	int yrdlhwzqhnzoif = 610;
	string gnmknrcfqarqdkt = "ocuepyqzoqxwxzvcaycmwghpfhqgxqbkmlfwipwoakelizlvrclexhvokhzefempcnynsndsihqxnqbgmvfunbi";
	bool snbsekll = true;
	return string("spflk");
}

void ciqwwkz::aqpcmxijhsjudgmw(string unvkoflrxyanz, int nebgihbxbskympz, double xhwhigpvmkva, double djaaumhoevkjgz, bool eywkvlb, string fhxebobnfdpxil, string tdvbqpvxxe, double rkczsi, bool zofocqteibxt, string gcwzicxplxy) {
	bool nzvfyptapyyjllr = false;
	int yzciknfs = 618;
	string huleu = "nvgntuqhckbirtujypwj";
	int umzjuuqlikpxq = 103;
	double daohtyxew = 13020;

}

int ciqwwkz::qixvmnvmhgnnrxy(string cktmccehqel, bool gojwihsanprcs) {
	int catgfrjnm = 6582;
	string hyebwibmjxa = "sxijxzklunwqswfaoxapeqyayobjumtjibjwayiwirbrpsfpgthzdmwxghsputqpmkbjozcawyqdgjnqijnairddvb";
	double fvhjolrgdndjbr = 64632;
	bool qsajrsglqs = false;
	int tcqjkpnoictsrh = 1459;
	bool jaalhgdhtacq = true;
	string bagsflpkxp = "ahcfoxkmvxlycxdflxycsrfscxxgwmaodzcnfiwfrbibcsvzmgmgxmmdapsxqxpufmlultnywzvqbgrtbitfpogckbojdgcb";
	int qqurazvbi = 254;
	int fybwqadb = 5174;
	double lcrqewlmnsmc = 3558;
	if (3558 != 3558) {
		int xkziz;
		for (xkziz = 36; xkziz > 0; xkziz--) {
			continue;
		}
	}
	if (3558 == 3558) {
		int rwgyoiq;
		for (rwgyoiq = 83; rwgyoiq > 0; rwgyoiq--) {
			continue;
		}
	}
	if (64632 == 64632) {
		int wzv;
		for (wzv = 61; wzv > 0; wzv--) {
			continue;
		}
	}
	if (false == false) {
		int rpjygkdtw;
		for (rpjygkdtw = 32; rpjygkdtw > 0; rpjygkdtw--) {
			continue;
		}
	}
	if (64632 != 64632) {
		int jisjbchvh;
		for (jisjbchvh = 36; jisjbchvh > 0; jisjbchvh--) {
			continue;
		}
	}
	return 94420;
}

double ciqwwkz::tcydkyxgcdynvevnqrmne(int hrsrimwkfwebz, int gyjsmjljzfytml, string tupitxmoe, double hnisewlyxmysrgy, int pdehtbfo, bool vjtwym) {
	string sbgluxodtry = "lxhdkzxbvaghbxoucodlrcmohqhnvrxuikmvrpwtxknapmojldzubomwcmfqlyzysbymxlhdrcnfnpoxpcaq";
	bool vvzbowljuv = false;
	bool abmlpghtl = true;
	double uynfjj = 77870;
	bool nkntkbuz = true;
	bool ncebogsngy = true;
	double ygotokkceevhk = 12271;
	if (false == false) {
		int ikqvcxezo;
		for (ikqvcxezo = 33; ikqvcxezo > 0; ikqvcxezo--) {
			continue;
		}
	}
	if (true != true) {
		int aevsuexjrr;
		for (aevsuexjrr = 49; aevsuexjrr > 0; aevsuexjrr--) {
			continue;
		}
	}
	if (77870 == 77870) {
		int zlogjzh;
		for (zlogjzh = 85; zlogjzh > 0; zlogjzh--) {
			continue;
		}
	}
	if (true != true) {
		int xz;
		for (xz = 50; xz > 0; xz--) {
			continue;
		}
	}
	return 43749;
}

string ciqwwkz::agmuiwqdecqpggucjwx(int ujqzwlsd, bool keoefwb, double ovdmkedi, bool bvvenwvoqc, string cgvkefsxgc, double hbbtpmjvddh, int zpuedo, int mzzzcsgm, int iimef, double ayysesk) {
	double efflvrxfhf = 10271;
	bool cyjbzmw = false;
	bool wnmqvepgvr = false;
	string adhyyeljyzd = "uqzuavfrygdlmichosjqrbwgzxukdkxvvjjwppmpluubqhxsiwkyyrj";
	bool qcjjnxdigslxjii = true;
	bool wsvqbaknxj = true;
	string xgixfpzmcxij = "ljoubdcqpewgbbzxuityggfwzhmsjuopbrvhgnwxpiaptbavqohtapis";
	double etupnnqelzfq = 33913;
	if (10271 != 10271) {
		int nkmm;
		for (nkmm = 82; nkmm > 0; nkmm--) {
			continue;
		}
	}
	return string("");
}

void ciqwwkz::hkcektelzcussvpppr(int fdtogapzfvh, int usqtrdsei, int xzuisrb, int mmwxnttmspnfc) {
	int occydj = 4829;

}

void ciqwwkz::pzedxjfssoblpzayhxycc() {
	string lzscwaqifv = "lwbijdkdhlxyjeazsbqoafapmuwcvylpousbk";
	string cjqjbkit = "dhlszyjbozdlcxlommsdawwtpcbcyketimewjomvidkcosomkbnpyviimqxjfynggeggnhdcekrjhguiysecxjhjjftuikockca";
	if (string("dhlszyjbozdlcxlommsdawwtpcbcyketimewjomvidkcosomkbnpyviimqxjfynggeggnhdcekrjhguiysecxjhjjftuikockca") != string("dhlszyjbozdlcxlommsdawwtpcbcyketimewjomvidkcosomkbnpyviimqxjfynggeggnhdcekrjhguiysecxjhjjftuikockca")) {
		int cl;
		for (cl = 25; cl > 0; cl--) {
			continue;
		}
	}
	if (string("lwbijdkdhlxyjeazsbqoafapmuwcvylpousbk") == string("lwbijdkdhlxyjeazsbqoafapmuwcvylpousbk")) {
		int pnnmal;
		for (pnnmal = 63; pnnmal > 0; pnnmal--) {
			continue;
		}
	}

}

void ciqwwkz::wbnlpppggkzjqqjyx(double mrpvq, int snqsdg, int idegkdyuownj, bool nvowi, bool btubdcwgxfmcbz, int gftywxhmbokk, int rbxhuyshg) {
	double hxdun = 41331;
	double copjyrry = 29289;
	string tdarnmnldk = "akvycdnzuidlkrmaeraosuuyfaewjpsiezkvakmuxlziactwvtqtcbrexfvxytppusmnxeqlkoysmdpbkanrlovnmkan";
	double uisbv = 24103;
	double oeyuypyg = 38998;
	if (24103 == 24103) {
		int hsfgaber;
		for (hsfgaber = 7; hsfgaber > 0; hsfgaber--) {
			continue;
		}
	}
	if (29289 == 29289) {
		int vfdmp;
		for (vfdmp = 2; vfdmp > 0; vfdmp--) {
			continue;
		}
	}

}

string ciqwwkz::ozqjevlyvr(string scdpwvbe, string uhppoydo, double cvdbvcwxybgu, string dkorkdcujmtpq, bool dcetdqimf, string cjqzianpejxsaw, string zqylbgvxjzqfq, double dybdlpjmd) {
	double dvbqveljpr = 50260;
	bool wmesmkb = false;
	int aubmsk = 216;
	double qrllpkztku = 34244;
	string afgkjspwkvqmbjj = "wgnzoybvr";
	if (false == false) {
		int cyllinkq;
		for (cyllinkq = 78; cyllinkq > 0; cyllinkq--) {
			continue;
		}
	}
	if (216 == 216) {
		int cpbfm;
		for (cpbfm = 53; cpbfm > 0; cpbfm--) {
			continue;
		}
	}
	if (false == false) {
		int vyvwjmr;
		for (vyvwjmr = 57; vyvwjmr > 0; vyvwjmr--) {
			continue;
		}
	}
	if (false == false) {
		int tdkxqxguub;
		for (tdkxqxguub = 13; tdkxqxguub > 0; tdkxqxguub--) {
			continue;
		}
	}
	return string("ecmpxwzcviiculykaym");
}

string ciqwwkz::weqgatrwcjqyklf(string sbuiqttt, bool ydsvoxf, string ygcxnb, bool btswpunj, bool frlqzlgu, string iintkfj, string dxnmkaaxhbx, string tngrfjmumed) {
	bool bxgqlz = true;
	bool xfgdqunfpicj = false;
	double zydvwgu = 1884;
	if (false == false) {
		int wcenguukjr;
		for (wcenguukjr = 20; wcenguukjr > 0; wcenguukjr--) {
			continue;
		}
	}
	if (1884 != 1884) {
		int zbp;
		for (zbp = 76; zbp > 0; zbp--) {
			continue;
		}
	}
	if (false == false) {
		int ilm;
		for (ilm = 2; ilm > 0; ilm--) {
			continue;
		}
	}
	if (1884 == 1884) {
		int tr;
		for (tr = 77; tr > 0; tr--) {
			continue;
		}
	}
	return string("w");
}

string ciqwwkz::guierhbhmzzrmckqrxtetuhpn(double hbfqyhdgklpz, double lgpzqnchorkk) {
	double dxbqw = 37813;
	if (37813 == 37813) {
		int ozjuhtsp;
		for (ozjuhtsp = 22; ozjuhtsp > 0; ozjuhtsp--) {
			continue;
		}
	}
	if (37813 == 37813) {
		int htznmpo;
		for (htznmpo = 95; htznmpo > 0; htznmpo--) {
			continue;
		}
	}
	if (37813 != 37813) {
		int snsdrcl;
		for (snsdrcl = 68; snsdrcl > 0; snsdrcl--) {
			continue;
		}
	}
	return string("wlojwupzlvoorlbtlhuc");
}

void ciqwwkz::eqgqpktovurmkalrcmaki(double lrtjwjo, double yrobgqwitmnbva, int zljqgnlpbqq) {
	string xnjknqoadffnq = "kwsbnlyedctguiqiruxogcwnyskmxeblirnxhkbmvnwixyndymzxszxsnoaclljvywlbwvbtmr";
	double rpcxvfroh = 24291;
	bool nutazqqupjccks = true;
	double brozofbwnegux = 10622;
	int olgjmbx = 838;
	int trawwy = 2636;
	bool ozfkjrm = false;
	if (string("kwsbnlyedctguiqiruxogcwnyskmxeblirnxhkbmvnwixyndymzxszxsnoaclljvywlbwvbtmr") != string("kwsbnlyedctguiqiruxogcwnyskmxeblirnxhkbmvnwixyndymzxszxsnoaclljvywlbwvbtmr")) {
		int qd;
		for (qd = 6; qd > 0; qd--) {
			continue;
		}
	}
	if (string("kwsbnlyedctguiqiruxogcwnyskmxeblirnxhkbmvnwixyndymzxszxsnoaclljvywlbwvbtmr") != string("kwsbnlyedctguiqiruxogcwnyskmxeblirnxhkbmvnwixyndymzxszxsnoaclljvywlbwvbtmr")) {
		int kbfb;
		for (kbfb = 11; kbfb > 0; kbfb--) {
			continue;
		}
	}

}

bool ciqwwkz::bsctpcwgqbrpzrsqftdez(double stpkanjolexgjlf, bool ibzxpdmoddhm, string ldteemc, double nlyiyfcqjiv, string jnqzxl, double hiwzd, int lnnsellffwqw, int syuaexvcsumsg) {
	int wlnjqhjxruz = 1397;
	string nfizix = "jcatcvxprqodskdyfqwxfwqvezdrdvriehxpgwbjvuukjvypijaysqrotmqhtujfjgr";
	if (1397 == 1397) {
		int tpvc;
		for (tpvc = 2; tpvc > 0; tpvc--) {
			continue;
		}
	}
	if (1397 != 1397) {
		int gmrodbwtm;
		for (gmrodbwtm = 67; gmrodbwtm > 0; gmrodbwtm--) {
			continue;
		}
	}
	return false;
}

void ciqwwkz::bmjzwpykqf(int uuukwp, int gzyczhkfrizpf, double kqbpxqq, int itjdcyutrscrhjx, bool edcbiuahhwmnva, bool ytogsho, string rckvhufsqtt) {
	double daoztq = 2742;
	if (2742 == 2742) {
		int vamfb;
		for (vamfb = 77; vamfb > 0; vamfb--) {
			continue;
		}
	}
	if (2742 == 2742) {
		int aahvxq;
		for (aahvxq = 58; aahvxq > 0; aahvxq--) {
			continue;
		}
	}
	if (2742 == 2742) {
		int gg;
		for (gg = 73; gg > 0; gg--) {
			continue;
		}
	}
	if (2742 != 2742) {
		int we;
		for (we = 62; we > 0; we--) {
			continue;
		}
	}

}

ciqwwkz::ciqwwkz() {
	this->eqgqpktovurmkalrcmaki(82628, 61315, 4483);
	this->bsctpcwgqbrpzrsqftdez(9249, true, string("iuygtyymtxvxxndvodnmjehyftpldgsvmsaampjsdpdgopwfhmwjyvzgpqrqyzqvbpctivwrgffme"), 6583, string("tokqpzgopkklexjmuyqvrysehvyutqsgaxsbbzmsgxweuwypiiuudduvuvqvhzbgbaxmzpsnvnbvf"), 18681, 2501, 372);
	this->bmjzwpykqf(650, 785, 30612, 5690, false, true, string("patwthhegeujyvghotqibnmjclrjnqiysbhxtaulfpwmirkiqawewpnrcedphhzbgvlyzrslwonanfwrkkhagzrfvg"));
	this->weqgatrwcjqyklf(string("bngvcbzeltmstrkdvwz"), false, string("luxjiubocgtaijneafxffcdqbvpzuuqknxcqqjuaz"), true, false, string("rcypbbcyauzwuhxxfdzaaumbvimvhyrxiaummirtxopcngbrkcnyrslxzaknblhvpfbc"), string("nbjvypgccjfllehlumfxedrixovx"), string("fmwswmvokanhctjvugofzjtcmxohsrbcqyudupbveokupmme"));
	this->guierhbhmzzrmckqrxtetuhpn(26221, 25233);
	this->manevgzutlmlxzxzcigfulf(47130, 4271, 73, true, true, 1023, 3144, string("ibpyfvuiiipqvggksxlfquqwvgnwuhiknmvqpeadxkcjnouuqzeriefqscpmgcorrxdjexfywwhlnnysnvfeotnf"), 7603, false);
	this->yuflymuraamqygpsvbyrlj(true, 7551, string("rwqkgdatbjyqvkukrmsmzdul"), 7198, string("mugvushogpmyhamwmyqpwrizzybvcotzvhzhtzxggbkfqdvhixmvcmmvczhd"), 1457);
	this->aqpcmxijhsjudgmw(string("yykdiuzailbtucroazlcpmcastmdrx"), 1823, 41152, 28028, true, string("pefeaukurjgisczthahtdjieejyirgvtnnbrqxbsp"), string("qummidhkubtpsgwflvvobvjbtvqwouixzxmaq"), 8210, true, string("okxvutxfeogfbqtiwosjhbpgzvrbsgjieisgqhvepdptfxfwojohmstpnopzhmvwfydfqyyfjumoapg"));
	this->qixvmnvmhgnnrxy(string("xfuthwxpaghhntpgftsowyibzxuxwiwyicxbcsgmgerurwzrsooodjhfwcjosi"), false);
	this->tcydkyxgcdynvevnqrmne(8429, 509, string(""), 34641, 2402, false);
	this->agmuiwqdecqpggucjwx(2407, false, 19815, false, string("gaowvjomqvixxmequjqabsdbbzixrtylpvdbywfaymesekokyqtvgl"), 14635, 4709, 3405, 3875, 34163);
	this->hkcektelzcussvpppr(349, 4279, 1051, 2516);
	this->pzedxjfssoblpzayhxycc();
	this->wbnlpppggkzjqqjyx(14131, 841, 2498, false, false, 3418, 5987);
	this->ozqjevlyvr(string("ozfclnekthohbliiimuzuypspmedfkofchbwafppoiofvemllwstpmkgakgespaaj"), string("qpmjfaxfjzdjxryqagkzgtrqayeywgbnhfspscevmqpjmqtsnaksjeqwxyezruvksilovyenefdtkydqcnfm"), 10770, string("nbqjlzqbwubgtnadmccrrfrxwaagiqppdierwyijuoowwgwhfcuxknmxfjgggdifsddashzwe"), true, string("bsdtqirvhjnjooxaejkstrgbpcwpjpyxaijkguo"), string("zhseupvdmslroborisnqwtteanymzrzxonhvvolqosmocrkmrkepddxjorbdfnavbzlsdfs"), 19869);
}


















































































































































































































































































