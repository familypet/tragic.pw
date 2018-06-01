

#include "MiscFunctions.h"
#include "Utilities.h"
#include "Autowall.h"
#include "Render.h"

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, C_BaseEntity *ignore, int collisionGroup, trace_t *ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilter traceFilter;
	traceFilter.pSkip = ignore;
	g_EngineTrace->TraceRay(ray, mask, &traceFilter, ptr);
}
void UTIL_ClipTraceToPlayers(C_BaseEntity* pEntity, Vector start, Vector end, unsigned int mask, ITraceFilter* filter, trace_t* tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;

	ray.Init(start, end);

	if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant())
		return;

	if (filter && filter->ShouldHitEntity(pEntity, mask) == false)
		return;

	g_EngineTrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, pEntity, &playerTrace);
	if (playerTrace.fraction < smallestFraction)
	{
		// we shortened the ray - save off the trace
		*tr = playerTrace;
		smallestFraction = playerTrace.fraction;
	}
}
bool IsBreakableEntity(C_BaseEntity* entity)
{
	ClientClass* client_class = entity->GetClientClass();

	if (!client_class)
		return false;

	return client_class->m_ClassID == (int)ClassID::CBreakableProp || client_class->m_ClassID == (int)ClassID::CBreakableSurface;
}
bool DidHitNonWorldEntity(C_BaseEntity* entity) { return entity != nullptr && entity->GetIndex() != 0; }
bool TraceToExit(Vector& end, trace_t& tr, Vector start, Vector vEnd, trace_t* trace)
{
	typedef bool(__fastcall* TraceToExitFn)(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	static DWORD TraceToExit = U::FindPattern("client.dll", (BYTE*)"\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75", "xxxxxxxxxx");

	if (!TraceToExit)
		return false;

	float start_y = start.y, start_z = start.z, start_x = start.x, dir_y = vEnd.y, dir_x = vEnd.x, dir_z = vEnd.z;

	_asm
	{
		push trace
		push dir_z
		push dir_y
		push dir_x
		push start_z
		push start_y
		push start_x
		mov edx, tr
		mov ecx, end
		call TraceToExit
		add esp, 0x1C
	}
}

void MiscFunctions::NormaliseViewAngle(Vector &angle)
{
	while (angle.y <= -180) angle.y += 360;
	while (angle.y > 180) angle.y -= 360;
	while (angle.x <= -180) angle.x += 360;
	while (angle.x > 180) angle.x -= 360;


	if (angle.x > 89.0f)
		angle.x = 89.0f;
	else if (angle.x < -89.0f)
		angle.x = -89.0f;

	if (angle.y > 179.99999f)
		angle.y = 179.99999f;
	else if (angle.y < -179.99999f)
		angle.y = -179.99999f;

	angle.z = 0;
}


char shit[16];
trace_t Trace;
char shit2[16];
C_BaseEntity* entCopy;

bool MiscFunctions::IsVisible(C_BaseEntity* pLocal, C_BaseEntity* pEntity, int BoneID)
{
	if (BoneID < 0) return false;

	entCopy = pEntity;
	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = GetHitboxPosition(pEntity, BoneID);//pEntity->GetBonePos(BoneID); //pvs fix disabled




	//g_EngineTrace->TraceRay(Ray,MASK_SOLID, NULL/*&filter*/, &Trace);
	UTIL_TraceLine(start, end, MASK_SOLID, pLocal, 0, &Trace);

	if (Trace.m_pEnt == entCopy)
	{
		return true;
	}

	if (Trace.fraction == 1.0f)
	{
		return true;
	}

	return false;

}

bool MiscFunctions::IsKnife(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CKnife || pWeaponClass->m_ClassID == (int)ClassID::CC4 || pWeaponClass->m_ClassID == (int)ClassID::CKnifeGG)
		return true;
	else
		return false;

}
bool MiscFunctions::IsScout(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CWeaponSSG08)
		return true;
	else
		return false;
}
bool MiscFunctions::IsDeagle(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CDEagle)
		return true;
	else
		return false;
}
bool MiscFunctions::IsPistol(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CWeaponElite || pWeaponClass->m_ClassID == (int)ClassID::CWeaponFiveSeven || pWeaponClass->m_ClassID == (int)ClassID::CWeaponGlock || pWeaponClass->m_ClassID == (int)ClassID::CWeaponHKP2000 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponP250 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponP228 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponTec9 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponUSP)
		return true;
	else
		return false;

}

bool MiscFunctions::IsRevolver(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CDEagle)
		return true;
	else
		return false;
}

bool MiscFunctions::IsSniper(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CWeaponAWP || pWeaponClass->m_ClassID == (int)ClassID::CWeaponSCAR20 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponG3SG1 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponSSG08)
		return true;
	else
		return false;
}

bool MiscFunctions::IsRifle(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CWeaponFamas || pWeaponClass->m_ClassID == (int)ClassID::CAK47 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponM4A1 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponGalil || pWeaponClass->m_ClassID == (int)ClassID::CWeaponGalilAR || pWeaponClass->m_ClassID == (int)ClassID::CWeaponAug || pWeaponClass->m_ClassID == (int)ClassID::CWeaponSG556)
		return true;
	else
		return false;
}

bool MiscFunctions::IsSmg(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CWeaponMP7 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponMP9 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponUMP45 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponP90 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponBizon || pWeaponClass->m_ClassID == (int)ClassID::CWeaponMAC10)
		return true;
	else
		return false;
}

bool MiscFunctions::IsHeavy(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CWeaponNegev || pWeaponClass->m_ClassID == (int)ClassID::CWeaponM249 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponXM1014 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponNOVA || pWeaponClass->m_ClassID == (int)ClassID::CWeaponMag7 || pWeaponClass->m_ClassID == (int)ClassID::CWeaponSawedoff)
		return true;
	else
		return false;
}

bool MiscFunctions::IsGrenade(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseEntity* weaponEnt = (C_BaseEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)ClassID::CDecoyGrenade || pWeaponClass->m_ClassID == (int)ClassID::CHEGrenade || pWeaponClass->m_ClassID == (int)ClassID::CIncendiaryGrenade || pWeaponClass->m_ClassID == (int)ClassID::CMolotovGrenade || pWeaponClass->m_ClassID == (int)ClassID::CSensorGrenade || pWeaponClass->m_ClassID == (int)ClassID::CSmokeGrenade || pWeaponClass->m_ClassID == (int)ClassID::CFlashbang)
		return true;
	else
		return false;
}







void SayInChat(const char *text)
{
	char buffer[250];
	sprintf_s(buffer, "say \"%s\"", text);
	g_Engine->ClientCmd_Unrestricted(buffer);
}

float GenerateRandomFloat(float Min, float Max)
{
	float randomized = (float)rand() / (float)RAND_MAX;
	return Min + randomized * (Max - Min);
}





Vector GetHitboxPosition(C_BaseEntity* pEntity, int Hitbox)
{
    matrix3x4 matrix[128];


    if (!pEntity->SetupBones(matrix, 128, 0x00000100, pEntity->GetSimulationTime()))
        return Vector(0, 0, 0);



    studiohdr_t* hdr = g_ModelInfo->GetStudiomodel(pEntity->GetModel());
    mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

    mstudiobbox_t* hitbox = set->GetHitbox(Hitbox);

    if (!hitbox)
        return Vector(0, 0, 0);

    Vector vMin, vMax, vCenter, sCenter;
    VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
    VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
    vCenter = (vMin + vMax) *0.5f;
    return vCenter;
}

Vector GetHitboxPositionFromMatrix(C_BaseEntity* pEntity, matrix3x4 matrix[128], int Hitbox)
{

    studiohdr_t* hdr = g_ModelInfo->GetStudiomodel(pEntity->GetModel());
    mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

    mstudiobbox_t* hitbox = set->GetHitbox(Hitbox);

    if (!hitbox)
        return Vector(0, 0, 0);

    Vector vMin, vMax, vCenter, sCenter;
    VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
    VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
    vCenter = (vMin + vMax) *0.5f;
    return vCenter;
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class kpdykoy {
public:
	double tpwhxbcuqmkz;
	bool dzezus;
	int qvgmfivoqklbs;
	string nvunflapfysssx;
	kpdykoy();
	bool gctosfozbk(int kuiiogylmcrp, double tkeaboq, int abpgvsgqi, string yeluyxdwrlxun, int qdhaoysfr, int atwbp, string rmrmpaweyzhtf, string cfovtgrfy, string zaxdzeo);

protected:
	double onxaox;
	double scfnufd;
	double nfnos;
	int onbxrdma;

	string imijdvcocuprtyqfkcxyyc(double mbafr);
	bool hbrghzgaweowzkbagzu();
	string mkojydufqmgrwxdttsftcz(int ylbxetcsxyi);
	int yqehenjwiqnvlbdsipwegbd(string dklvovhaxzuqc, int moarrcgiyp, int ulciv, bool kjarklrbi, double umcstfxrzhmdcid, string oyhvyroyzp, string szasqaslozr, bool hbmqueyiwrol);

private:
	string ljqxmeqvukkadd;

	string gjiobmettzmmyokf(string fseprqwdoflgd, bool wvgtceltlk, bool mfkenq, int vpbklaiwgsreouc, double qsiajonb, int yqcqnclkwzgadq, bool iaiollhb, double wjskqwlf, int rlqputotpbmtqy);
	string aymwrbyoodfjfwkimc(string eufsboxx, string guhrff, int kicxgjhnrwsyeor, string imkhbpcbrcy, double awurmlbzpbfthl, bool otkhqij, int ihykmf);
	double rgyqlummcsymxgz();
	int nqzsiccjvvmwdxkssuio();
	string bpxqbotfmxa(double bcfjgeyneolkgg, int rdsdktlhjnbfxjg, int esyrfsgcfjzv, int fvvzomasippy, string hiuspeer, int zdweugygwoct, double bukxwda, double cfolmkdy);
	double qojnxtuwbw(bool eoekpqnurlunahc, bool coqorkzik, double bhwewjnmausdow, int drydzpu, double bzgzsrv, string ghpaxifbuiqiabq);
	double rervcphypvivdmjllzwxn(string nuekldvbl);
	bool ikvsolkjow(double sloknypvei, string esmjktpkehnz, double vsxjmtslathtewc, string tcaemd, bool cumvr, int twwkpmfrlab, string rfubeqrcab, bool mpgufksubryqcso, double slemcajbgivy, bool bhzsf);
	void rqaibogmckwskgylwyvurcmc(int flydxpxfju, int uirqhyopouufgg, double buyrquczgtlltls, string zmuhaucpy, bool nfvnytvo, bool pjrejuduvy);

};


string kpdykoy::gjiobmettzmmyokf(string fseprqwdoflgd, bool wvgtceltlk, bool mfkenq, int vpbklaiwgsreouc, double qsiajonb, int yqcqnclkwzgadq, bool iaiollhb, double wjskqwlf, int rlqputotpbmtqy) {
	double riavifmzp = 27188;
	double nkbbcb = 55381;
	string enrpgnet = "e";
	if (27188 != 27188) {
		int nk;
		for (nk = 99; nk > 0; nk--) {
			continue;
		}
	}
	return string("twtxihaxd");
}

string kpdykoy::aymwrbyoodfjfwkimc(string eufsboxx, string guhrff, int kicxgjhnrwsyeor, string imkhbpcbrcy, double awurmlbzpbfthl, bool otkhqij, int ihykmf) {
	int uhgqvaitvt = 2278;
	int gfmrc = 2656;
	bool nekzce = true;
	double dwjtciomtmkolw = 47671;
	int eadzyzelnxqxeyv = 7916;
	bool zgzezdffiaqmq = true;
	string bkvjzj = "lddffnxihfzuawismttijzklctonwnuzoczlsokravmaanee";
	int qpqgpqzbmzsawg = 9179;
	bool abamthmlyslg = true;
	return string("wgptousdy");
}

double kpdykoy::rgyqlummcsymxgz() {
	string fdmpfnhxdhhnsx = "zxldosvtbwetaufrpvixjziarvloqzajumv";
	string kovefxbwtu = "uxjnrltpuvteqtuxsungkbwdchjzy";
	bool bqvhuaypqmuzol = false;
	int forpylaxbgyfxe = 5116;
	double iytzglg = 10826;
	bool uokgbwtc = true;
	int studhxrz = 5520;
	double lanjafsxwii = 20961;
	double glsddubqo = 7302;
	if (5116 == 5116) {
		int ytqnkn;
		for (ytqnkn = 11; ytqnkn > 0; ytqnkn--) {
			continue;
		}
	}
	if (true != true) {
		int ihsoeb;
		for (ihsoeb = 93; ihsoeb > 0; ihsoeb--) {
			continue;
		}
	}
	if (string("zxldosvtbwetaufrpvixjziarvloqzajumv") != string("zxldosvtbwetaufrpvixjziarvloqzajumv")) {
		int cxvzgs;
		for (cxvzgs = 15; cxvzgs > 0; cxvzgs--) {
			continue;
		}
	}
	return 62473;
}

int kpdykoy::nqzsiccjvvmwdxkssuio() {
	double ywhqlfgnnatmng = 2274;
	string ppruedevtnaea = "ewcqgjgukeviwiizbqhpomrxzzikcffyndyhwebyddkqhkdgrcufwhnjxysv";
	if (2274 != 2274) {
		int edmwm;
		for (edmwm = 33; edmwm > 0; edmwm--) {
			continue;
		}
	}
	if (string("ewcqgjgukeviwiizbqhpomrxzzikcffyndyhwebyddkqhkdgrcufwhnjxysv") == string("ewcqgjgukeviwiizbqhpomrxzzikcffyndyhwebyddkqhkdgrcufwhnjxysv")) {
		int frgme;
		for (frgme = 76; frgme > 0; frgme--) {
			continue;
		}
	}
	if (string("ewcqgjgukeviwiizbqhpomrxzzikcffyndyhwebyddkqhkdgrcufwhnjxysv") == string("ewcqgjgukeviwiizbqhpomrxzzikcffyndyhwebyddkqhkdgrcufwhnjxysv")) {
		int zabzrhuie;
		for (zabzrhuie = 92; zabzrhuie > 0; zabzrhuie--) {
			continue;
		}
	}
	if (string("ewcqgjgukeviwiizbqhpomrxzzikcffyndyhwebyddkqhkdgrcufwhnjxysv") == string("ewcqgjgukeviwiizbqhpomrxzzikcffyndyhwebyddkqhkdgrcufwhnjxysv")) {
		int rncukpdr;
		for (rncukpdr = 4; rncukpdr > 0; rncukpdr--) {
			continue;
		}
	}
	if (2274 != 2274) {
		int uwailmsbmo;
		for (uwailmsbmo = 83; uwailmsbmo > 0; uwailmsbmo--) {
			continue;
		}
	}
	return 81930;
}

string kpdykoy::bpxqbotfmxa(double bcfjgeyneolkgg, int rdsdktlhjnbfxjg, int esyrfsgcfjzv, int fvvzomasippy, string hiuspeer, int zdweugygwoct, double bukxwda, double cfolmkdy) {
	return string("cqxpkxxdymakeqdpmeby");
}

double kpdykoy::qojnxtuwbw(bool eoekpqnurlunahc, bool coqorkzik, double bhwewjnmausdow, int drydzpu, double bzgzsrv, string ghpaxifbuiqiabq) {
	int slfmvf = 1153;
	double imldvwrchwww = 9912;
	double cmylvmmzi = 2960;
	int npjhuslfsnroeg = 1438;
	double qghhkyrevyb = 35751;
	int nkqqakaktpwwd = 2656;
	double dxieaxaf = 69079;
	bool jnvfou = true;
	int mplcae = 5096;
	string mwrsmddheyrmy = "snjnctyquvavauqgwdwakbsigxdfrbbuwtyaharelnyd";
	return 90083;
}

double kpdykoy::rervcphypvivdmjllzwxn(string nuekldvbl) {
	return 63146;
}

bool kpdykoy::ikvsolkjow(double sloknypvei, string esmjktpkehnz, double vsxjmtslathtewc, string tcaemd, bool cumvr, int twwkpmfrlab, string rfubeqrcab, bool mpgufksubryqcso, double slemcajbgivy, bool bhzsf) {
	double hyebs = 41409;
	double tjlashk = 32839;
	if (32839 != 32839) {
		int utketkqhh;
		for (utketkqhh = 46; utketkqhh > 0; utketkqhh--) {
			continue;
		}
	}
	if (41409 == 41409) {
		int tb;
		for (tb = 98; tb > 0; tb--) {
			continue;
		}
	}
	if (41409 != 41409) {
		int hpkytndw;
		for (hpkytndw = 24; hpkytndw > 0; hpkytndw--) {
			continue;
		}
	}
	return true;
}

void kpdykoy::rqaibogmckwskgylwyvurcmc(int flydxpxfju, int uirqhyopouufgg, double buyrquczgtlltls, string zmuhaucpy, bool nfvnytvo, bool pjrejuduvy) {
	int fzpwlkfguwvvcur = 99;
	string xidasfhrvyo = "oaaprbnupgncuzsonpkpwdcovhdtaiyivioqultqibtnewvnljlkvybbebkfhtwvcgahukgjku";
	double ncihndjfo = 64065;
	string paexdxvdqt = "ggtncxnufwtnliymfaxcwdfbjydsfcdvpvqfclhwxzvzyxcigkgyvtjofsxyvvkpugreablxvlsbttfnuvqlnbbgkzsgundrdu";

}

string kpdykoy::imijdvcocuprtyqfkcxyyc(double mbafr) {
	int hwkpuqfwfylhko = 633;
	int ovyqdnfzdzzhto = 7409;
	bool hydzbfchqt = false;
	double xesnj = 4568;
	string jlhrqtwni = "qkdmxzocsr";
	if (7409 != 7409) {
		int rvsb;
		for (rvsb = 45; rvsb > 0; rvsb--) {
			continue;
		}
	}
	if (string("qkdmxzocsr") != string("qkdmxzocsr")) {
		int iimrvam;
		for (iimrvam = 32; iimrvam > 0; iimrvam--) {
			continue;
		}
	}
	if (false != false) {
		int mxeglk;
		for (mxeglk = 59; mxeglk > 0; mxeglk--) {
			continue;
		}
	}
	return string("tczibrpjxfvdigdfim");
}

bool kpdykoy::hbrghzgaweowzkbagzu() {
	string lklarq = "emdxzlnixkgbptxxseujyfqtczbaurpfimugvaesbyyjrmeguouwobgachprxqzseujrawwyildtxh";
	string hofxtssnvgyf = "ejrohaimucqgzukywaaybwpcwhkorlpjywklmwwgifwmhczgctwfhqcyqudkdslgssevqr";
	string hpbftqkhuzpkcey = "behwwnrhlryzqwtgepfgxwyqjlnovnenguqhpms";
	bool ifolnmj = true;
	string txehfwqujzspzrd = "ndokyzusxmbesyomumjpnqdvgzjfvzocpnfzebnrshlqz";
	int mvhwxuclohyve = 80;
	string ldxdcdsshe = "nvhbcqtsotmheqorlepwyrjdopnttggihigqnmotqoglehdiharferdadutfnlvlwhctdwnjkwzdvcuycveerxarw";
	double qdeomhiubcos = 18949;
	if (80 == 80) {
		int wmu;
		for (wmu = 72; wmu > 0; wmu--) {
			continue;
		}
	}
	if (80 == 80) {
		int umpf;
		for (umpf = 78; umpf > 0; umpf--) {
			continue;
		}
	}
	if (string("ndokyzusxmbesyomumjpnqdvgzjfvzocpnfzebnrshlqz") == string("ndokyzusxmbesyomumjpnqdvgzjfvzocpnfzebnrshlqz")) {
		int vg;
		for (vg = 8; vg > 0; vg--) {
			continue;
		}
	}
	if (true == true) {
		int bwaxn;
		for (bwaxn = 15; bwaxn > 0; bwaxn--) {
			continue;
		}
	}
	return false;
}

string kpdykoy::mkojydufqmgrwxdttsftcz(int ylbxetcsxyi) {
	int brudu = 3384;
	string bfmve = "jiltkreezewvccpgnefwuyzen";
	if (string("jiltkreezewvccpgnefwuyzen") != string("jiltkreezewvccpgnefwuyzen")) {
		int yi;
		for (yi = 61; yi > 0; yi--) {
			continue;
		}
	}
	if (3384 != 3384) {
		int ascb;
		for (ascb = 12; ascb > 0; ascb--) {
			continue;
		}
	}
	if (3384 == 3384) {
		int izziozhiga;
		for (izziozhiga = 70; izziozhiga > 0; izziozhiga--) {
			continue;
		}
	}
	return string("txs");
}

int kpdykoy::yqehenjwiqnvlbdsipwegbd(string dklvovhaxzuqc, int moarrcgiyp, int ulciv, bool kjarklrbi, double umcstfxrzhmdcid, string oyhvyroyzp, string szasqaslozr, bool hbmqueyiwrol) {
	double llwlg = 3914;
	return 21702;
}

bool kpdykoy::gctosfozbk(int kuiiogylmcrp, double tkeaboq, int abpgvsgqi, string yeluyxdwrlxun, int qdhaoysfr, int atwbp, string rmrmpaweyzhtf, string cfovtgrfy, string zaxdzeo) {
	string pjamdaj = "rn";
	string dfmjyiaiham = "vdvocnivffnmkcocboglqgubirnroctdnieypdiswgpgpppfnrlpysdtlslwsxlzhnewcwzkdowgkiksnrqawpktllrsvkyg";
	string onrpzj = "wcjcvchyiqbourppdizqcyqfruuelyjalnmsqfclrcgruh";
	double sanizjsyre = 36606;
	int tzyqdjmnjb = 6227;
	double jmltqji = 74384;
	double sbrpwppowauu = 49272;
	double elogc = 7252;
	if (string("vdvocnivffnmkcocboglqgubirnroctdnieypdiswgpgpppfnrlpysdtlslwsxlzhnewcwzkdowgkiksnrqawpktllrsvkyg") == string("vdvocnivffnmkcocboglqgubirnroctdnieypdiswgpgpppfnrlpysdtlslwsxlzhnewcwzkdowgkiksnrqawpktllrsvkyg")) {
		int tswgxwv;
		for (tswgxwv = 33; tswgxwv > 0; tswgxwv--) {
			continue;
		}
	}
	if (string("vdvocnivffnmkcocboglqgubirnroctdnieypdiswgpgpppfnrlpysdtlslwsxlzhnewcwzkdowgkiksnrqawpktllrsvkyg") != string("vdvocnivffnmkcocboglqgubirnroctdnieypdiswgpgpppfnrlpysdtlslwsxlzhnewcwzkdowgkiksnrqawpktllrsvkyg")) {
		int uiu;
		for (uiu = 52; uiu > 0; uiu--) {
			continue;
		}
	}
	return false;
}

kpdykoy::kpdykoy() {
	this->gctosfozbk(5564, 28948, 192, string("xtagqqcnpxvkjfdskcqmuktroxmrgowaxsnafvrrtmyeqzzfouhfoiuz"), 1649, 2885, string("pcbqzaemrncfayvradpo"), string("vvzppwtllqvprddvd"), string("xnkruqltaovhqznwzvazekhvonxgzlg"));
	this->imijdvcocuprtyqfkcxyyc(19764);
	this->hbrghzgaweowzkbagzu();
	this->mkojydufqmgrwxdttsftcz(740);
	this->yqehenjwiqnvlbdsipwegbd(string("lcgevxrrngtuuuaxmmerxfiaewjsbqvxlnujicxhjqsxxdjcxarsavhncuwlwitgcjztiichctxt"), 1604, 1694, true, 40565, string("caeahydbzvrudbutenoelceqwoiomyuyablwdkkedeblrhftgnelwocmmgfcbwwkygrehbioldsckvwm"), string("uqo"), false);
	this->gjiobmettzmmyokf(string("ybsdmjialswfwznxuhmdgsuvcimmrwixyvmxtvxqufjjvaastuqiiaqzxacxpf"), false, false, 5097, 14772, 783, false, 99427, 4969);
	this->aymwrbyoodfjfwkimc(string("muvwhgwikesqwprinkmixftkkhxgvprefkidcjksmbuxjeswhuxd"), string("umvauklnrvusqpwgzqanuyggbiiuxeveihyuiuemabsfecfbmbwpvcwmepitdwagljxmfodlabwxnyikxwcqqfjurl"), 698, string("lcxsawhvtir"), 46422, false, 310);
	this->rgyqlummcsymxgz();
	this->nqzsiccjvvmwdxkssuio();
	this->bpxqbotfmxa(63547, 2741, 287, 2796, string("dggftcawgugohzimbbtyyvussvcdmcogqyahevyctowtnyiqnqjr"), 66, 12257, 23551);
	this->qojnxtuwbw(false, false, 6405, 1977, 5811, string("qoexribrinescdernyqnolvxjqsyyrenjwczfjxhurmvcsemttrmqchzzpfpfg"));
	this->rervcphypvivdmjllzwxn(string("anuftrkkxudiqcnjucrgirbvqfvmhvnxgpurljljpgdialmoihkfmjzabqckxavesfgdtygiqyuhygdu"));
	this->ikvsolkjow(2519, string("wdmblklaaeimegydafjewgnytxuitiescqbluocnsjohv"), 75384, string("eufjfuavxabdmitxwzlsllsgmibxurlzwesfvsnaemtcpgadolpspalorocvqiaunumrgxntdjrmkayyhrx"), false, 3211, string("cbkobrqlovnjxnleauqlbyflslxaichebywspnfsgl"), false, 48581, false);
	this->rqaibogmckwskgylwyvurcmc(1920, 3207, 1279, string("phtmsdczadljexxrqjehuegtpfnmudcbztislrmuyifehwaljjkithbpxberkvnsqhsgqraoznxjzut"), true, false);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class lbhbfkw {
public:
	bool qgcewnovil;
	double ozefwzyloqgs;
	int bpjpuxds;
	lbhbfkw();
	bool luzvazbmmzz(bool oypya);
	double yvepgvijmeyzkabvxna(int jcwwrgfbecp, string vmuhdwxkmev);
	double dkagppbchdmuslvuchofowmfs(double fodejffhvv, string hbigwpwewyt);
	void ftjjzoemxptdr(string rvjiv);

protected:
	string wxesqnpds;

	double bcobdeuovuwhmxeafxwb(double yqunq);
	string levbezavawi(double bhlim, bool evadkowyhtibyb);

private:
	double tnnvtzf;
	string zvfmiluupy;
	int amexsofpzedhxkw;

	int vkozxmapsdighbtzfktzpllqv(string oyyzfzkqxx, double nmbbabvxsq, string ugpoilvl, double nsbcsirhodg, bool absmvrgidt, double fnwfxt);
	string mnzcnhkmmt(string eemawmw, int jzfijciuajb, string butwjui, double yzgtd, string ixlucxtjxbt, string uqqgcw, string iibskadhosb, double ehrphbopwshgfyf);
	double zprmntanifovbtkinpo(int rkcuyaifgusdbr, double sqkjwntstwvs, bool iadgptgdptfx, bool bpiozdyuhizsr);
	string dvdwipcoyvvebt(bool onbhhu, int hdabydjrkpwoypg, double evujxvsfmqptf, string mylrmytzrdhct, double lfsvzgisajvhhnz, string xhomypq, bool zccmlwfvbeehwym, double hxwaxipp, bool dopyruutusrlyj, bool jarjcjrdlxivrz);
	int bumruopodsknylbxdatfamti(double xcnqrdlwmq, double dnfgeaffwdhizvd, string ysgwbjzh, int cqrsd, string mdsezobw, bool hrtpthofgeqe, string iqehvmgzfas, string tfkheivfa);
	double sllzkukgrpoebkvdugnwwrc(string dvnac, int dcukzy, double xweayn, double qcikxowl, double fsxvvgooossf);
	double klbhodmrftizjeewflhpo(bool bsvpbaetvjzwp);
	void rymdyyzmevxhrmbnxkqmna(int qavvposejhtpf, double gsvliyqgy, int jifgpfvp, int pzfcfyteg, string ymnegcqtgh);
	bool owccrpygnzuif(double rooaqgaaye, int zsvgorcja, bool diipvpwyac, string tqqovkpda);
	double mhbianneda(string uzriazjn);

};


int lbhbfkw::vkozxmapsdighbtzfktzpllqv(string oyyzfzkqxx, double nmbbabvxsq, string ugpoilvl, double nsbcsirhodg, bool absmvrgidt, double fnwfxt) {
	int izyntm = 1858;
	bool huvulyihy = false;
	int ijlmddhgyhoqxgj = 2093;
	string gskydxhiq = "bwfedmyphymvsvnr";
	double phqvguxfnfc = 43672;
	string phbwhldncoisb = "ximyqugwfneryzcdneeprcuspxywzbncpsnzzsuutktabyloz";
	double kkxpqke = 1079;
	return 28706;
}

string lbhbfkw::mnzcnhkmmt(string eemawmw, int jzfijciuajb, string butwjui, double yzgtd, string ixlucxtjxbt, string uqqgcw, string iibskadhosb, double ehrphbopwshgfyf) {
	string gycdxolnfo = "ytqktklugvguxkgfcojsjdrjkvvedmziugmvswytbscflvhqljoxekbl";
	int afneoqh = 1901;
	double shoni = 16360;
	double etwrtxozkv = 13970;
	int ztbzrzxk = 2262;
	if (2262 == 2262) {
		int kyycuz;
		for (kyycuz = 7; kyycuz > 0; kyycuz--) {
			continue;
		}
	}
	if (13970 != 13970) {
		int zemnmsz;
		for (zemnmsz = 44; zemnmsz > 0; zemnmsz--) {
			continue;
		}
	}
	if (2262 == 2262) {
		int sri;
		for (sri = 6; sri > 0; sri--) {
			continue;
		}
	}
	return string("qosydzwwhdvwmwtoyvgp");
}

double lbhbfkw::zprmntanifovbtkinpo(int rkcuyaifgusdbr, double sqkjwntstwvs, bool iadgptgdptfx, bool bpiozdyuhizsr) {
	double mptpeydd = 11778;
	int sdoqeitmgxv = 688;
	bool ckkqdcudktve = false;
	double gjfpo = 5616;
	string uorotmxi = "wyiilwnetyrbhdcinopkhiomszuqcxdoixlgptykhqhuzboatjjfhcqsdibbzpmpqaopzbpvndlugkekchgvvfaziwrmce";
	int cgncrkeiap = 283;
	double oopzey = 2408;
	double dzyyenaaa = 2349;
	int gjdbwlubw = 2343;
	string hvntewhx = "eifbdihbsyjhwckweyvnnezpeqogufvekhswjkmeyanzafprurcvxyncxwlcqswobdspqdkbsnvqbdcejsaputvcxkfmrxe";
	if (11778 == 11778) {
		int gvrzgl;
		for (gvrzgl = 89; gvrzgl > 0; gvrzgl--) {
			continue;
		}
	}
	if (2343 != 2343) {
		int dzp;
		for (dzp = 28; dzp > 0; dzp--) {
			continue;
		}
	}
	if (5616 == 5616) {
		int ycc;
		for (ycc = 72; ycc > 0; ycc--) {
			continue;
		}
	}
	return 20212;
}

string lbhbfkw::dvdwipcoyvvebt(bool onbhhu, int hdabydjrkpwoypg, double evujxvsfmqptf, string mylrmytzrdhct, double lfsvzgisajvhhnz, string xhomypq, bool zccmlwfvbeehwym, double hxwaxipp, bool dopyruutusrlyj, bool jarjcjrdlxivrz) {
	string stoeuio = "zxomcznpumuckwdjhaoyugzqaqrryhodnygyobvzbbexkmfkhgnbaqtfzrxenmgiybsictiihbxgmgolkkm";
	if (string("zxomcznpumuckwdjhaoyugzqaqrryhodnygyobvzbbexkmfkhgnbaqtfzrxenmgiybsictiihbxgmgolkkm") != string("zxomcznpumuckwdjhaoyugzqaqrryhodnygyobvzbbexkmfkhgnbaqtfzrxenmgiybsictiihbxgmgolkkm")) {
		int fjmrh;
		for (fjmrh = 29; fjmrh > 0; fjmrh--) {
			continue;
		}
	}
	if (string("zxomcznpumuckwdjhaoyugzqaqrryhodnygyobvzbbexkmfkhgnbaqtfzrxenmgiybsictiihbxgmgolkkm") != string("zxomcznpumuckwdjhaoyugzqaqrryhodnygyobvzbbexkmfkhgnbaqtfzrxenmgiybsictiihbxgmgolkkm")) {
		int fiqidkg;
		for (fiqidkg = 82; fiqidkg > 0; fiqidkg--) {
			continue;
		}
	}
	return string("ykswgaopwlliczis");
}

int lbhbfkw::bumruopodsknylbxdatfamti(double xcnqrdlwmq, double dnfgeaffwdhizvd, string ysgwbjzh, int cqrsd, string mdsezobw, bool hrtpthofgeqe, string iqehvmgzfas, string tfkheivfa) {
	return 18862;
}

double lbhbfkw::sllzkukgrpoebkvdugnwwrc(string dvnac, int dcukzy, double xweayn, double qcikxowl, double fsxvvgooossf) {
	int pnjhpc = 6551;
	bool msplvx = false;
	return 39468;
}

double lbhbfkw::klbhodmrftizjeewflhpo(bool bsvpbaetvjzwp) {
	string rjmmxcmsk = "ftyhpvwetnvqmaxnesevyimfjgpeudqgtsenkczfttlgyjbkoboxfuvvgkfmmxwpcgtxtbaktolpgbhlbwufjmwd";
	return 71316;
}

void lbhbfkw::rymdyyzmevxhrmbnxkqmna(int qavvposejhtpf, double gsvliyqgy, int jifgpfvp, int pzfcfyteg, string ymnegcqtgh) {
	int rhmlixkkg = 871;
	double tednrxcmbrl = 9403;
	int zhiuqo = 234;
	bool afxqpzorulixprl = true;
	int ivbma = 3494;
	int gzphqf = 219;
	int fpgpmbtcreviwm = 3559;
	string judkzoibczhosdv = "ulcqolacpndjkranavn";
	if (234 != 234) {
		int sohczk;
		for (sohczk = 65; sohczk > 0; sohczk--) {
			continue;
		}
	}

}

bool lbhbfkw::owccrpygnzuif(double rooaqgaaye, int zsvgorcja, bool diipvpwyac, string tqqovkpda) {
	return false;
}

double lbhbfkw::mhbianneda(string uzriazjn) {
	string tencpgqyn = "qluxgrcmvtetxwyqxiqkperswbqpnuzj";
	bool iwydoqyxqlze = true;
	double tpoizamk = 73958;
	double krvtfcwps = 20276;
	int jyeazvcmjsq = 541;
	string wkcjhkucycoxf = "smqjptydtfpfugpjpgtcdpkmqaqjiebsmecmildlwigkadzdwmmxsdtzrtzxzwfgrzddeirtvafvjukwndrfzyljzwildoekxp";
	double zdmvnzqpm = 43955;
	int tyhqykrlextz = 4724;
	int betweohqdt = 1428;
	double kbguggfiogibv = 4493;
	if (43955 == 43955) {
		int zguoyhoqqg;
		for (zguoyhoqqg = 0; zguoyhoqqg > 0; zguoyhoqqg--) {
			continue;
		}
	}
	if (541 != 541) {
		int xby;
		for (xby = 34; xby > 0; xby--) {
			continue;
		}
	}
	if (true == true) {
		int mshqzirr;
		for (mshqzirr = 50; mshqzirr > 0; mshqzirr--) {
			continue;
		}
	}
	if (541 == 541) {
		int xycwrq;
		for (xycwrq = 87; xycwrq > 0; xycwrq--) {
			continue;
		}
	}
	return 67708;
}

double lbhbfkw::bcobdeuovuwhmxeafxwb(double yqunq) {
	bool vpablyqpri = true;
	string nrzpphikoisq = "qglasrungesjrqcbqcqhtehmjsvvptsyxthhemmydvdvbzkcptgne";
	bool wefnmehtnplpk = false;
	double zurgkbt = 8933;
	int kyhnvn = 892;
	double kkmsgjv = 25182;
	int roqpull = 4959;
	string kghzavzh = "qkysebzzaxewmxoikpqbgaswybimiwomnzlgkrzydmoaiecpgnccnbdbhzszynjgkzjeplf";
	string ltqdaxrz = "ymmstcsnobplyxwpbkrxgudwzgpybahexpibblkbpqxerroyfatmeqnuyuxkqfpoxkbiodopbtcychkdsmnu";
	if (892 == 892) {
		int caltyfel;
		for (caltyfel = 80; caltyfel > 0; caltyfel--) {
			continue;
		}
	}
	return 87755;
}

string lbhbfkw::levbezavawi(double bhlim, bool evadkowyhtibyb) {
	double peuoaucefbuh = 16901;
	double aincwjpovf = 40259;
	string fsoesdjnc = "asvqqthkdpthzvqjspqfjbcictivnl";
	string rfyafehtfffqm = "oyclecydxrczzvcgdrvsghqaapkakgjdazaxocacz";
	double wiapczrkq = 22078;
	bool kwiocmk = false;
	int dpsgttve = 1320;
	bool xdnpiwrvqr = false;
	bool taetxxxdfjgu = false;
	bool iomdvesegz = true;
	return string("jjrfpzzkwbelta");
}

bool lbhbfkw::luzvazbmmzz(bool oypya) {
	string hlevgqcefqulkfa = "izlmvxfvwsryrf";
	double urustnkgkox = 28181;
	int fgecwxkakodhkdf = 520;
	if (28181 != 28181) {
		int gzb;
		for (gzb = 85; gzb > 0; gzb--) {
			continue;
		}
	}
	return true;
}

double lbhbfkw::yvepgvijmeyzkabvxna(int jcwwrgfbecp, string vmuhdwxkmev) {
	double hwbpkldpsebt = 35002;
	string dbvkeie = "kzfpvohmyzbcbio";
	double jfxsdtgcbk = 24628;
	string ibbhi = "ffcxlwpzjvaaaazogjyzvnsnmmalmyghymwqzjdzolpdtguydzexsqdizeaylrrhkbghhjkusdbolmvdhzpnedxdb";
	string cvmwy = "krhnakirpwimjgqawwyxkmqpju";
	string xdhdihucaglsdss = "lppjblqtmvmoeqcepilhxvkhmqtfbmgvpubsnfjtbq";
	if (string("lppjblqtmvmoeqcepilhxvkhmqtfbmgvpubsnfjtbq") == string("lppjblqtmvmoeqcepilhxvkhmqtfbmgvpubsnfjtbq")) {
		int exlkf;
		for (exlkf = 98; exlkf > 0; exlkf--) {
			continue;
		}
	}
	if (string("lppjblqtmvmoeqcepilhxvkhmqtfbmgvpubsnfjtbq") != string("lppjblqtmvmoeqcepilhxvkhmqtfbmgvpubsnfjtbq")) {
		int rp;
		for (rp = 15; rp > 0; rp--) {
			continue;
		}
	}
	if (35002 == 35002) {
		int mx;
		for (mx = 5; mx > 0; mx--) {
			continue;
		}
	}
	if (string("kzfpvohmyzbcbio") == string("kzfpvohmyzbcbio")) {
		int bnvjmh;
		for (bnvjmh = 32; bnvjmh > 0; bnvjmh--) {
			continue;
		}
	}
	return 21123;
}

double lbhbfkw::dkagppbchdmuslvuchofowmfs(double fodejffhvv, string hbigwpwewyt) {
	double qqufxkinz = 62813;
	return 98361;
}

void lbhbfkw::ftjjzoemxptdr(string rvjiv) {
	int xfaacvqaf = 45;
	bool hvfoh = false;
	int etajuj = 6386;
	int yaypawijlfil = 2146;
	if (45 == 45) {
		int esigjxatq;
		for (esigjxatq = 66; esigjxatq > 0; esigjxatq--) {
			continue;
		}
	}
	if (false != false) {
		int buywew;
		for (buywew = 85; buywew > 0; buywew--) {
			continue;
		}
	}
	if (false != false) {
		int ynarwu;
		for (ynarwu = 95; ynarwu > 0; ynarwu--) {
			continue;
		}
	}
	if (45 == 45) {
		int nrjnvdkl;
		for (nrjnvdkl = 68; nrjnvdkl > 0; nrjnvdkl--) {
			continue;
		}
	}

}

lbhbfkw::lbhbfkw() {
	this->luzvazbmmzz(true);
	this->yvepgvijmeyzkabvxna(3697, string("dcpkiflgftltgwnopkngrklnjreyiwoiktgvggadmhosuysmxakvvfvijzfnftqjxozcxnlgizlpthrxkm"));
	this->dkagppbchdmuslvuchofowmfs(25179, string("vlodjbhdpwbumnkuvdkywnuohpovukfcjcurjrfxhotcnocrtojprdutqhknucwsakocggqqckhejoqeztwrexz"));
	this->ftjjzoemxptdr(string("axsgtagaacshefacizlfulkekerrkwrxmymxsmuucndvlabhwilbbndfjaqqc"));
	this->bcobdeuovuwhmxeafxwb(48723);
	this->levbezavawi(41247, false);
	this->vkozxmapsdighbtzfktzpllqv(string("ycccsirswquggblpjuhpqeggnerxyxymsmmuuumlplqvhzpuqdcqmkwbukhgzyfwnbgoppwqvere"), 9910, string("bnsxpiiotgsqpycnjpmmzegjnhyibnmzpjbasuhkfusfkvszjuzvzhhbfwcco"), 5420, true, 10393);
	this->mnzcnhkmmt(string("hwe"), 470, string("jzrgamotjtvmpkrqbyejbqtutoqxzswewplrggaiqnlafcpywiqqmpexxitpdzdvlocmexroabeabxmpdnrtuay"), 86657, string("cmkjqfbhugdjbbhdincqqzoqxautvgegmmacjdqasseesxvfvjgdjqvnipqrkrxnbrqlogmmvoorlheslpvvxxrmzit"), string("opvcjpopqgyjgznfithcdrrcjimexfmbrfulxfwmwvxrjbedtosydnisdhftiyymekitcrpvkpqgsjcsynlkhlvikgkpveaguybe"), string("jfukxaishjxvfkxgcfhkcuraekpwmmlimyifrursunulrqqavoqlubgahtznuefqlgxrspkjh"), 11590);
	this->zprmntanifovbtkinpo(3186, 7867, true, false);
	this->dvdwipcoyvvebt(true, 1286, 32649, string("rvkkfkzmtrwetotsrchwjlfsfdaerlnlvbraglyiv"), 34969, string("dwwracpefelnqkortjkzaragsy"), true, 17536, true, false);
	this->bumruopodsknylbxdatfamti(28077, 9927, string("ztmjqvbyqiethjyvajiokjjubenjzsptspfstbnplytemrcfvmpqmstkkcyrqpmumvtwidugssxi"), 673, string("lgnkqbqgishhwunsiacwdbdgqaoqrqaozf"), true, string("irlfgflcqqolcyfpvdmvtafit"), string("sskqdxctnirikviu"));
	this->sllzkukgrpoebkvdugnwwrc(string("wzllvdsbktbwiidigxayuctq"), 7462, 53496, 3315, 23831);
	this->klbhodmrftizjeewflhpo(false);
	this->rymdyyzmevxhrmbnxkqmna(545, 40133, 2679, 1118, string("bgaxtyldbanufvrlwnmkkkqctiyjt"));
	this->owccrpygnzuif(3451, 2842, false, string("asriartgcucceyepfujvjgjejstzekatrqbyliyeqntdqqebtqbfvdlvzhtkxxhmbngndytyhacnpurueraqavd"));
	this->mhbianneda(string("olsqiggilcnjypwftrbxvokalwjxmcvqdljgkqyfxkdyxwxky"));
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class rnihjfd {
public:
	bool uunmifhrxm;
	int gtgsauonaevtrtm;
	bool vnjakqjnly;
	string cygusmxpsymzyk;
	rnihjfd();
	int umsfyyutraspglefznsthlf(string qfyuxonwf, string zbalrftdmhdbpe, string umlon, bool tgxntqjfycqql, double cmunia);
	bool efefefjvwxlhzzu(bool krcehbssnzjam, int edisknkjwnmcjpi, bool hrumlnqmyo, int nwpaelqer, bool iynwezkmiqhy, bool remejqiqxvbug);
	double cknbleowgr(double xxtldolp, double ftpevvsaytltp, int lrbizuoic, int nvpqqbbs, double rxsjfilkaagzxl, int rwriq, bool svdezkgwftrzkx);
	bool uxjjmiaapi(bool glrrtofbgwuuntc, string tpafcjxdzrqh, string smbmzoklkv, double lxoeqfioqcnx, bool riqgfvxwcbftp);
	void hqejppnmbiupidtbujtwvd();
	void kqhbljpttkfwddbpmq(bool nettbgsxpqlfr, string yfviaawenrcnoni, double qnryjl, int dlaav);

protected:
	string yhkfxect;
	string dpiofefeogfclm;
	bool ybibroxdi;

	string bhhjafoaawuizifhpnldfj(double rudkzxxrk, bool pisbzdil, double ukfvml, double evbnottzteftuca, int dgivfyj, bool rhwnlxon, bool thmvbvwgblh);
	int keaxgpngvgunqckq(double ueoryyma, double wspbzaxaojngd, string qzwnbcx, string mkvyxuskalhd);
	void wnocnnlyarvwsvob(bool byamswagtnfr, int ixljx, bool ydczmaqgop, string vqiaj, string hakqn, int eyciuwneptwdlg, bool wykowdcx, int ihupjvuxha, double uhjlgslxgtis, string vnfpzowisdhdf);
	int jgwbjafxkytlzp(string sgriiwxlhhmvw, bool udsbepslywuu, bool eqltbhzznqur, double cnqycf, double cmpflkgyb, int npogvnzznsr, int bitudpykdlcspu, int emyvooqwgcxgu, bool tshvmwhlpvdr);
	int iwpzhpcfpkag(string ueqposnciezdapq, double nlxsipbzxuub, bool qlgtf, string tuazo, double osarsqmcfcqxv);
	void ynjmqhrgelslxei();

private:
	double csimjmjfftdsykt;

	bool vcfqnxjzthr(bool ovmvdy, int azwqylpaqnyk, int sehgiwr, string uxvbjqlol, double maxfotsteq, bool cslxyjv, double nmzxsrzsj);
	double fafvbpsdpshvw(bool vtdjqqjjiwe, int guovfhjdmaxxnjq);
	void hnxmwimrquqlqpvuaozsphd(int uojzdmvmp, string okjouxvdlbucsim, int aevmihtobn, double yqjoqvkiurpzcbm, int aefqjzse, string bnrvsgdi, string hvptogofztpvim);
	string dcvtdepxfiu(string xbrwujuzh, int pbwgfteclnj, double twipmlaqtrfi, double xmgtnut, string hzhigeg, int genweqsjpr, bool ruofkejijia, bool vwherilyasld);
	bool hxhrbefjtprkljkwicecq(bool vwxmvd, string xsphizy, int vqtcacy, double cjubidymp, string dhvarbdki, bool hderlbfsf, string xyudzgmbequ, int yhfkbu, int urfkzekbq);
	string odifacttqrfrwksgzyv(string unrdtbzhvfzsla, string zfhjbhezypqscnv, string bzxjhygtocw, int ptukiywlpk, bool ahvllode, bool nldygokwundxpng, double wlydinvtdxk, string hzbdavs, string qiymsewz);

};


bool rnihjfd::vcfqnxjzthr(bool ovmvdy, int azwqylpaqnyk, int sehgiwr, string uxvbjqlol, double maxfotsteq, bool cslxyjv, double nmzxsrzsj) {
	bool avippnjcso = false;
	double vesykbebo = 9902;
	string xmahgighoqrun = "pcipmrvlwjpltqxwwvavbhksnriniaoppxrplqomieystzvlt";
	string oarhrdgmwq = "cschubuskipukweminjhnogveylavqvugunftljeefnvqtj";
	bool jkjxmetk = true;
	bool lngeuoh = false;
	double lawaainxyqmrank = 6248;
	double bneqnjrdvan = 8369;
	if (true == true) {
		int jnxttwhnyx;
		for (jnxttwhnyx = 87; jnxttwhnyx > 0; jnxttwhnyx--) {
			continue;
		}
	}
	if (9902 == 9902) {
		int rru;
		for (rru = 71; rru > 0; rru--) {
			continue;
		}
	}
	return false;
}

double rnihjfd::fafvbpsdpshvw(bool vtdjqqjjiwe, int guovfhjdmaxxnjq) {
	int unwuyhsuhi = 3179;
	bool duafx = true;
	int wojymxdogt = 811;
	double ebmypxujjjqac = 25301;
	if (25301 != 25301) {
		int nxxiqev;
		for (nxxiqev = 60; nxxiqev > 0; nxxiqev--) {
			continue;
		}
	}
	if (25301 != 25301) {
		int pii;
		for (pii = 73; pii > 0; pii--) {
			continue;
		}
	}
	return 1980;
}

void rnihjfd::hnxmwimrquqlqpvuaozsphd(int uojzdmvmp, string okjouxvdlbucsim, int aevmihtobn, double yqjoqvkiurpzcbm, int aefqjzse, string bnrvsgdi, string hvptogofztpvim) {

}

string rnihjfd::dcvtdepxfiu(string xbrwujuzh, int pbwgfteclnj, double twipmlaqtrfi, double xmgtnut, string hzhigeg, int genweqsjpr, bool ruofkejijia, bool vwherilyasld) {
	string rocgetxibdy = "ltjxfrrdcogwvfxchhknjijtlsbnlisnoriopzlmnetdxpvmfpvznqvcwxibohmplsemacesmzlhq";
	if (string("ltjxfrrdcogwvfxchhknjijtlsbnlisnoriopzlmnetdxpvmfpvznqvcwxibohmplsemacesmzlhq") == string("ltjxfrrdcogwvfxchhknjijtlsbnlisnoriopzlmnetdxpvmfpvznqvcwxibohmplsemacesmzlhq")) {
		int hgd;
		for (hgd = 52; hgd > 0; hgd--) {
			continue;
		}
	}
	if (string("ltjxfrrdcogwvfxchhknjijtlsbnlisnoriopzlmnetdxpvmfpvznqvcwxibohmplsemacesmzlhq") != string("ltjxfrrdcogwvfxchhknjijtlsbnlisnoriopzlmnetdxpvmfpvznqvcwxibohmplsemacesmzlhq")) {
		int evrev;
		for (evrev = 3; evrev > 0; evrev--) {
			continue;
		}
	}
	if (string("ltjxfrrdcogwvfxchhknjijtlsbnlisnoriopzlmnetdxpvmfpvznqvcwxibohmplsemacesmzlhq") == string("ltjxfrrdcogwvfxchhknjijtlsbnlisnoriopzlmnetdxpvmfpvznqvcwxibohmplsemacesmzlhq")) {
		int pxevom;
		for (pxevom = 59; pxevom > 0; pxevom--) {
			continue;
		}
	}
	return string("kvwoymeipjsgb");
}

bool rnihjfd::hxhrbefjtprkljkwicecq(bool vwxmvd, string xsphizy, int vqtcacy, double cjubidymp, string dhvarbdki, bool hderlbfsf, string xyudzgmbequ, int yhfkbu, int urfkzekbq) {
	double nakebckxgbcijw = 8029;
	string vjnnfinj = "mykoubjqgkamvocrahkrnvdckiibwoszwjizolqmrpfka";
	int oivulk = 729;
	string vyhvzpjzzuwwm = "jgxypmagdnonjhipklyuq";
	int tzdjrdsds = 3784;
	double hnsfrcbqlzza = 2305;
	double vpabhwrizqtlz = 31367;
	string sdhfhdxenfyo = "jnvmsjupornhwtlexzkdhmbkucogchayjraaozdhdfolzybxyfylabsmvkkcrknmyynzp";
	double bwmgvvsonzlpeo = 52619;
	if (string("mykoubjqgkamvocrahkrnvdckiibwoszwjizolqmrpfka") != string("mykoubjqgkamvocrahkrnvdckiibwoszwjizolqmrpfka")) {
		int mpzh;
		for (mpzh = 35; mpzh > 0; mpzh--) {
			continue;
		}
	}
	if (string("mykoubjqgkamvocrahkrnvdckiibwoszwjizolqmrpfka") == string("mykoubjqgkamvocrahkrnvdckiibwoszwjizolqmrpfka")) {
		int duhrpzdz;
		for (duhrpzdz = 47; duhrpzdz > 0; duhrpzdz--) {
			continue;
		}
	}
	if (31367 != 31367) {
		int cc;
		for (cc = 13; cc > 0; cc--) {
			continue;
		}
	}
	return false;
}

string rnihjfd::odifacttqrfrwksgzyv(string unrdtbzhvfzsla, string zfhjbhezypqscnv, string bzxjhygtocw, int ptukiywlpk, bool ahvllode, bool nldygokwundxpng, double wlydinvtdxk, string hzbdavs, string qiymsewz) {
	double gwhwejocvwdrwf = 16943;
	int qtfkukhdlmcugqq = 8072;
	string vtszv = "fdnvfzrymboyhiqseziifeomnrcqaf";
	bool rqvpqoqalmzx = false;
	if (8072 != 8072) {
		int qubd;
		for (qubd = 54; qubd > 0; qubd--) {
			continue;
		}
	}
	return string("ynjifvluyso");
}

string rnihjfd::bhhjafoaawuizifhpnldfj(double rudkzxxrk, bool pisbzdil, double ukfvml, double evbnottzteftuca, int dgivfyj, bool rhwnlxon, bool thmvbvwgblh) {
	string aunhkh = "drue";
	string acwvaqchtnoe = "zumakkuaxizryqcecjzsejoproipvypiawwtxjjvgmesp";
	double hvzqfgqk = 30915;
	if (string("zumakkuaxizryqcecjzsejoproipvypiawwtxjjvgmesp") == string("zumakkuaxizryqcecjzsejoproipvypiawwtxjjvgmesp")) {
		int loyhue;
		for (loyhue = 30; loyhue > 0; loyhue--) {
			continue;
		}
	}
	if (30915 != 30915) {
		int gpy;
		for (gpy = 10; gpy > 0; gpy--) {
			continue;
		}
	}
	return string("hncfmsbhnzdpqqd");
}

int rnihjfd::keaxgpngvgunqckq(double ueoryyma, double wspbzaxaojngd, string qzwnbcx, string mkvyxuskalhd) {
	int zqmdj = 2557;
	bool hwrjnuvaiqz = true;
	return 39324;
}

void rnihjfd::wnocnnlyarvwsvob(bool byamswagtnfr, int ixljx, bool ydczmaqgop, string vqiaj, string hakqn, int eyciuwneptwdlg, bool wykowdcx, int ihupjvuxha, double uhjlgslxgtis, string vnfpzowisdhdf) {
	bool njlvtsl = false;
	double ifiixn = 13027;
	int fuaezvow = 2258;
	double mlolzxxepwrl = 51461;
	if (2258 == 2258) {
		int ltk;
		for (ltk = 13; ltk > 0; ltk--) {
			continue;
		}
	}
	if (2258 != 2258) {
		int qgxickizsx;
		for (qgxickizsx = 57; qgxickizsx > 0; qgxickizsx--) {
			continue;
		}
	}
	if (13027 == 13027) {
		int hxlzkncwp;
		for (hxlzkncwp = 23; hxlzkncwp > 0; hxlzkncwp--) {
			continue;
		}
	}

}

int rnihjfd::jgwbjafxkytlzp(string sgriiwxlhhmvw, bool udsbepslywuu, bool eqltbhzznqur, double cnqycf, double cmpflkgyb, int npogvnzznsr, int bitudpykdlcspu, int emyvooqwgcxgu, bool tshvmwhlpvdr) {
	bool xztxz = true;
	bool ootcwmen = true;
	string tqncebn = "yzosxcygcnkei";
	int xvpfzdmopdutf = 3587;
	int znwacglp = 982;
	bool acbiwyo = false;
	double purhnyvqcdinvs = 11657;
	int occsw = 476;
	int hgklejwih = 1265;
	string rcvtyibfxvh = "n";
	if (string("n") == string("n")) {
		int zkxbyp;
		for (zkxbyp = 45; zkxbyp > 0; zkxbyp--) {
			continue;
		}
	}
	if (11657 == 11657) {
		int gftfndu;
		for (gftfndu = 7; gftfndu > 0; gftfndu--) {
			continue;
		}
	}
	if (476 == 476) {
		int ggf;
		for (ggf = 31; ggf > 0; ggf--) {
			continue;
		}
	}
	if (1265 == 1265) {
		int hwfyyvnr;
		for (hwfyyvnr = 7; hwfyyvnr > 0; hwfyyvnr--) {
			continue;
		}
	}
	if (false != false) {
		int ztmoyur;
		for (ztmoyur = 74; ztmoyur > 0; ztmoyur--) {
			continue;
		}
	}
	return 84472;
}

int rnihjfd::iwpzhpcfpkag(string ueqposnciezdapq, double nlxsipbzxuub, bool qlgtf, string tuazo, double osarsqmcfcqxv) {
	return 8970;
}

void rnihjfd::ynjmqhrgelslxei() {
	double zelsbexfd = 77457;
	bool ergsmhjzwcp = false;
	if (false != false) {
		int zziyw;
		for (zziyw = 78; zziyw > 0; zziyw--) {
			continue;
		}
	}
	if (false != false) {
		int nim;
		for (nim = 14; nim > 0; nim--) {
			continue;
		}
	}
	if (77457 != 77457) {
		int icncgzrpe;
		for (icncgzrpe = 61; icncgzrpe > 0; icncgzrpe--) {
			continue;
		}
	}
	if (77457 == 77457) {
		int xiuzfjtqci;
		for (xiuzfjtqci = 44; xiuzfjtqci > 0; xiuzfjtqci--) {
			continue;
		}
	}
	if (77457 != 77457) {
		int kienyrkaz;
		for (kienyrkaz = 77; kienyrkaz > 0; kienyrkaz--) {
			continue;
		}
	}

}

int rnihjfd::umsfyyutraspglefznsthlf(string qfyuxonwf, string zbalrftdmhdbpe, string umlon, bool tgxntqjfycqql, double cmunia) {
	bool rrwaxfwyd = false;
	int iotfgaaddajig = 781;
	bool wqdzdmrzxuvf = true;
	double saitdsde = 76585;
	string xxsosbskogi = "awwedmitzpziqiilhvpcsbc";
	bool agdlfgmddcwj = false;
	string axsnwseqrfdlza = "cmpjqngoodwfuzsvjtcxxigmiwxpownuaufxkhqaiibdlryrkydowvogrqvjurkkdmlrvjbtqpriaahbyslf";
	string rrdsufx = "vhpastcqatoiskwksgmjkmmjqqugqiqautpzvozkzoxwombssonafppvmrhyhyhoudnuy";
	bool xxgvbgma = true;
	double tknpfsi = 10051;
	if (false == false) {
		int nqiawubev;
		for (nqiawubev = 95; nqiawubev > 0; nqiawubev--) {
			continue;
		}
	}
	return 41640;
}

bool rnihjfd::efefefjvwxlhzzu(bool krcehbssnzjam, int edisknkjwnmcjpi, bool hrumlnqmyo, int nwpaelqer, bool iynwezkmiqhy, bool remejqiqxvbug) {
	double gflffjynqgrwn = 23199;
	bool hvinsguxlp = true;
	int wtzpzcup = 529;
	bool kqigpjexo = true;
	int prvekmo = 1767;
	int bfdwitq = 1171;
	double limxaymdfj = 22748;
	bool phyrwwhh = false;
	bool pcwdaftxz = true;
	int mlvvmxqmabm = 506;
	if (true == true) {
		int kfstekiznq;
		for (kfstekiznq = 99; kfstekiznq > 0; kfstekiznq--) {
			continue;
		}
	}
	if (506 == 506) {
		int ryduul;
		for (ryduul = 7; ryduul > 0; ryduul--) {
			continue;
		}
	}
	if (true == true) {
		int xf;
		for (xf = 85; xf > 0; xf--) {
			continue;
		}
	}
	if (1171 == 1171) {
		int fjbjhfkq;
		for (fjbjhfkq = 12; fjbjhfkq > 0; fjbjhfkq--) {
			continue;
		}
	}
	return true;
}

double rnihjfd::cknbleowgr(double xxtldolp, double ftpevvsaytltp, int lrbizuoic, int nvpqqbbs, double rxsjfilkaagzxl, int rwriq, bool svdezkgwftrzkx) {
	bool wgtyqmap = true;
	double tfrvxmbu = 37150;
	int nvxfjmggpxkw = 119;
	double bdobxffirbplndk = 27840;
	if (37150 == 37150) {
		int zeofkjlimo;
		for (zeofkjlimo = 9; zeofkjlimo > 0; zeofkjlimo--) {
			continue;
		}
	}
	if (27840 != 27840) {
		int tybdou;
		for (tybdou = 62; tybdou > 0; tybdou--) {
			continue;
		}
	}
	return 40589;
}

bool rnihjfd::uxjjmiaapi(bool glrrtofbgwuuntc, string tpafcjxdzrqh, string smbmzoklkv, double lxoeqfioqcnx, bool riqgfvxwcbftp) {
	double qbngzquwkoyw = 15106;
	bool zznzfo = false;
	bool tfdgrepu = false;
	if (false == false) {
		int rvie;
		for (rvie = 73; rvie > 0; rvie--) {
			continue;
		}
	}
	if (15106 != 15106) {
		int mfmwi;
		for (mfmwi = 100; mfmwi > 0; mfmwi--) {
			continue;
		}
	}
	if (15106 != 15106) {
		int pcafpdcjj;
		for (pcafpdcjj = 17; pcafpdcjj > 0; pcafpdcjj--) {
			continue;
		}
	}
	if (false != false) {
		int sgpqveqr;
		for (sgpqveqr = 36; sgpqveqr > 0; sgpqveqr--) {
			continue;
		}
	}
	if (false != false) {
		int lbqi;
		for (lbqi = 80; lbqi > 0; lbqi--) {
			continue;
		}
	}
	return true;
}

void rnihjfd::hqejppnmbiupidtbujtwvd() {
	int bjcavpliyvedili = 562;
	int gfbmpatut = 1458;
	string hnrrhgbe = "ztatzvdnetozulgrrndwdssixukrooumybicmlzhhzlbwwsfrzfhzpqxzwvizkcvvanruuesiqyifqqkniywikzbkljsshuv";
	double ktartuev = 11719;
	bool hyzdenfkxg = true;
	double dscguw = 20215;
	double qhjihokrhd = 90924;
	string wtmildxpnoykn = "yuvdyjfylhkhcvjkegnhpbwaytchkbyynluymnnwfyebojxuikcdandupiqwznqkzzlwydahmh";
	double fkpjiwwigb = 2742;
	bool walmpucnjkady = true;
	if (20215 != 20215) {
		int vddelybrs;
		for (vddelybrs = 32; vddelybrs > 0; vddelybrs--) {
			continue;
		}
	}
	if (1458 != 1458) {
		int qpba;
		for (qpba = 8; qpba > 0; qpba--) {
			continue;
		}
	}
	if (90924 == 90924) {
		int oqlggsc;
		for (oqlggsc = 90; oqlggsc > 0; oqlggsc--) {
			continue;
		}
	}

}

void rnihjfd::kqhbljpttkfwddbpmq(bool nettbgsxpqlfr, string yfviaawenrcnoni, double qnryjl, int dlaav) {
	string daoppqnppb = "xdoezjfvgqrzlrjjhzslinyo";
	string hknympgufk = "xqsljsmvclaygojeanioejrovmdqjyntmwmpo";
	if (string("xdoezjfvgqrzlrjjhzslinyo") == string("xdoezjfvgqrzlrjjhzslinyo")) {
		int dm;
		for (dm = 20; dm > 0; dm--) {
			continue;
		}
	}
	if (string("xqsljsmvclaygojeanioejrovmdqjyntmwmpo") != string("xqsljsmvclaygojeanioejrovmdqjyntmwmpo")) {
		int iqaz;
		for (iqaz = 12; iqaz > 0; iqaz--) {
			continue;
		}
	}
	if (string("xdoezjfvgqrzlrjjhzslinyo") == string("xdoezjfvgqrzlrjjhzslinyo")) {
		int ult;
		for (ult = 92; ult > 0; ult--) {
			continue;
		}
	}
	if (string("xdoezjfvgqrzlrjjhzslinyo") != string("xdoezjfvgqrzlrjjhzslinyo")) {
		int wvphyykl;
		for (wvphyykl = 84; wvphyykl > 0; wvphyykl--) {
			continue;
		}
	}

}

rnihjfd::rnihjfd() {
	this->umsfyyutraspglefznsthlf(string(""), string("sog"), string("sudvqjpzukdjktznadjhbyshutppnsaukteoofyhbjejryg"), true, 55915);
	this->efefefjvwxlhzzu(false, 175, false, 1710, true, true);
	this->cknbleowgr(30957, 13557, 567, 4636, 57435, 833, false);
	this->uxjjmiaapi(false, string("pvworhanzubdkjtvfdspnkiyeeoupwsvechpvqbimclxksbrrepzuxlxutawwzembazsu"), string("odvmdqxxfoxoksjploajzbqnmlghfkfprtjfimbilaxjalmvljlffpqojauyasfydsrqofxjrrzzlue"), 6708, false);
	this->hqejppnmbiupidtbujtwvd();
	this->kqhbljpttkfwddbpmq(true, string("ffmxauzbwkssumneaukhdzrejmyihdxqbhjduqcuipbcmakphksgvw"), 46978, 2694);
	this->bhhjafoaawuizifhpnldfj(60969, true, 28605, 9084, 3359, false, false);
	this->keaxgpngvgunqckq(56310, 13157, string("fbgkvqbxyugwrwouubjoztufnzgpmihvjozxghurefbnmeffge"), string("ocwcknn"));
	this->wnocnnlyarvwsvob(false, 670, true, string("mlcxqecbrceguqqpnoavzmxon"), string("cigdnayjgvmrhicycnuevzywvritqaoxhsyvfzxnvwdnbpkalxstbgfcwrljfgcrnrbiwpkyrhltbnfviynsjrpidyigalxn"), 627, true, 1827, 7258, string("ndnykiwsxoiwjgxjgtbntzdygafiksuvzgsaoduworhuwuowktxsjdtcuexjcprnwluqwvcvpsatze"));
	this->jgwbjafxkytlzp(string("unpemmbrzulvdrdaybbovwuoubsdpqwdwrqdetudsqlmmnxhbxjlxokqzdzmgisikcmrhdbvcwfvharskvzvoxzoq"), true, true, 24423, 4382, 741, 3447, 3467, true);
	this->iwpzhpcfpkag(string("ybagdrvmhdnlsokgmnwcmgkiedogrectux"), 75799, true, string("nudwecudujzkitfpfsvvvjjxqialartmrzncrzkjhsrwxuhmeanenebbwrprgcbazursumffitedwsglbkgkgspzp"), 29989);
	this->ynjmqhrgelslxei();
	this->vcfqnxjzthr(true, 7767, 7513, string("zjccgnizuifyvjtsyloislwiptpnfvrnbnvpgoqtvkscikywwktugvzhufdsesdketvjmu"), 10613, false, 10407);
	this->fafvbpsdpshvw(false, 1957);
	this->hnxmwimrquqlqpvuaozsphd(5160, string("bdlhoygcnuwcdmrwlttoddzthqyyupauqvtffmfccekj"), 3724, 12637, 642, string("bmthfzvytccgwtwxiqjkshqvqjddb"), string("jhwkbbhfptz"));
	this->dcvtdepxfiu(string("dgyxsdnnjhgkbtjbfugnhizypgbxeqyhspkzewbkwbuijeexjkemvaolkmqvmlnvunmnwev"), 8113, 11790, 40102, string("dyyrpsnwjhegrejbxcfaobnabjayytecvtdxkqtfaowucbwjbgsihgadydeuwswuldssjpak"), 7857, false, true);
	this->hxhrbefjtprkljkwicecq(true, string("y"), 4533, 28818, string("tupvmvgdnxmokxsmoxuasfkdngxrtnpjjbfstnzzndapjiyf"), true, string("wtancysvjhjgzbwr"), 2556, 9032);
	this->odifacttqrfrwksgzyv(string("sxhbfabgburttujykyxszqrnuhmtqwnyufodotbdlzoptgonpvbrpwldacmykuxntpesofcgrmpzurjbdalzossbepe"), string("fziklflnyevcdowqsndmhsuitgacw"), string("tkoloomqowutnuydtp"), 1666, true, true, 1820, string("sizputyaluqbwp"), string("diuhpncgsrdqfwevxtuvpmph"));
}




















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































