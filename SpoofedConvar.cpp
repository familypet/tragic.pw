#include "SDK.h"
#include "MiscClasses.h"

template<typename T>
inline void MinspecCvar::SetValue(T value)
{
	m_pConVar->SetValue(T);
}
MinspecCvar::MinspecCvar(const char* szCVar, char* newname, float newvalue) : m_pConVar(nullptr)
{
	m_pConVar = g_CVar->FindVar(szCVar);
	m_newvalue = newvalue;
	m_szReplacementName = newname;
	Spoof();
}

MinspecCvar::~MinspecCvar()
{
	if (ValidCvar())
	{
		g_CVar->UnregisterConCommand(m_pConVar);
		m_pConVar->pszName = m_szOriginalName;
		m_pConVar->SetValue(m_OriginalValue);
		g_CVar->RegisterConCommand(m_pConVar);
	}
}

bool MinspecCvar::ValidCvar()
{
	return m_pConVar != nullptr;
}
void MinspecCvar::Spoof()
{
	if (ValidCvar())
	{
		g_CVar->UnregisterConCommand(m_pConVar);
		m_szOriginalName = m_pConVar->pszName;
		m_OriginalValue = m_pConVar->GetFloat();

		m_pConVar->pszName = m_szReplacementName;
		g_CVar->RegisterConCommand(m_pConVar);
		m_pConVar->SetValue(m_newvalue);
	}
}

int MinspecCvar::GetInt()
{
	if (ValidCvar()) {
		return m_pConVar->GetInt();
	}
	return 0;
}

float MinspecCvar::GetFloat()
{
	if (ValidCvar()) {
		return m_pConVar->GetFloat();
	}
	return 0.0f;
}

const char* MinspecCvar::GetString()
{
	if (ValidCvar()) {
		return m_pConVar->GetString();
	}
	return nullptr;
}

SpoofedConvar::SpoofedConvar(const char* szCVar) {
	m_pOriginalCVar = g_CVar->FindVar(szCVar);
	Spoof();
}
SpoofedConvar::SpoofedConvar(ConVar* pCVar, char* newname) {
	m_pOriginalCVar = pCVar;
	m_szReplacementName = newname;
	Spoof();
}
SpoofedConvar::~SpoofedConvar() {
	if (IsSpoofed()) {
		DWORD dwOld;

		SetFlags(m_iOriginalFlags);
		SetString(m_szOriginalValue);

		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, PAGE_READWRITE, &dwOld);
		strcpy((char*)m_pOriginalCVar->pszName, m_szOriginalName);
		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, dwOld, &dwOld);

		//Unregister dummy cvar
		g_CVar->UnregisterConCommand(m_pDummyCVar);
		free(m_pDummyCVar);
		m_pDummyCVar = NULL;
	}
}
bool SpoofedConvar::IsSpoofed() {
	return m_pDummyCVar != NULL;
}
void SpoofedConvar::Spoof() {
	if (!IsSpoofed() && m_pOriginalCVar) {
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->pszDefaultValue);

		sprintf_s(m_szDummyName, 128, "%s", m_szReplacementName);

		//Create the dummy cvar
		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->pNext = NULL;
		//Register it
		g_CVar->RegisterConCommand(m_pDummyCVar);

		//Fix "write access violation" bullshit
		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, PAGE_READWRITE, &dwOld);

		//Rename the cvar
		strcpy((char*)m_pOriginalCVar->pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}
void SpoofedConvar::SetFlags(int flags) {
	if (IsSpoofed()) {
		m_pOriginalCVar->nFlags = flags;
	}
}
int SpoofedConvar::GetFlags() {
	return m_pOriginalCVar->nFlags;
}
void SpoofedConvar::SetInt(int iValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(iValue);
	}
}
void SpoofedConvar::SetBool(bool bValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(bValue);
	}
}
void SpoofedConvar::SetFloat(float flValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(flValue);
	}
}
void SpoofedConvar::SetString(const char* szValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(szValue);
	}
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class riqyeva {
public:
	int rvmcfnfefyura;
	double padtwrmq;
	double rdvwpqrwawpwrtn;
	riqyeva();
	int cuuykfdayjhpquo(bool iapbbbcetre, bool rublfcdta, double ubovvmwgcc, int fbpkvfovmjsl);

protected:
	int zoxwvafpa;
	double wqutwiiadfigcn;
	bool xxjhrdxrhqpqa;

	string lnwaxldngfcqdye(int iqvhgaa, double urcrkxczzzu, bool auvvaobrwa, int fumgk, int afgrw);
	int jiywuxnzyxct(double iohfdm, bool bonslia, int qrcjjprxmpjfaf, double ncvstsjabclre, double bfzjsaz, double pokln);
	bool bqhikdmgfl(bool lhqtkgisidoxunm, string pagbhfu, int jrhfbrrx, string untigpljmik, string yylwanqpj, double wtkazqgl, double jtefenmx, int rcuazhqbftizfzf, int biarlibvasjqmao, double ewuaj);
	double mctnbqzkzvuthxqb(int xoikurxrtynbrir, int dbtjtfxzx, double ikcxsbkior, double ikcmexyrjqgjq, int xkvdlyxjas);
	bool afioxaaoslmcjkyjvevt(bool tlsjeu, double batxnp, int titrulnhutao, bool fuiewqys, double lstewmkpks);

private:
	bool auhjnhjbt;

	string jxwofjsqfbasijphrcorh(bool eijxgwjjk, string ayfbdv, string huhtoc, int iudylvpatptr, int ykeiremgjyyp, string mwrdogfxzlya, double pziltsnjwgurd, string salonoyb, bool himflvuxqkldndz);
	double kvibdphpsmqkembvoydfzszf(int vbidkjyocaivcfr, bool mgemmlrslq, double khlfqwldhqj, int dkkvjyewdfea, double wjhdfjnmr, string kunizaqkehx, int cahwg, int zysgliazmhfeo, int nzkrv);
	bool qmpoagosvumzszl();
	int pgqqphpbrfazilk(bool cadkyxz, double qmvrjs, int apmvwiyaucezzb, string lnjhi, string dnpin, int cylpbspdr, string wwtqbekdey, int cemiqzizfhmgmgj, double osbhktjrxzs, int libnlhcixfm);
	double aoqzvtxbdkiua(bool mnwtasuuzhkmjzs, bool zeryjeu, double eyfnbn);
	bool lpuxjcbxnnxcbx();
	void ekceueiqdkyi();
	int cbtfgbecbkxihhiv(int dtglhvmglqy, string vsvcht, int whsyjeokjrb, bool iopsbddf);

};


string riqyeva::jxwofjsqfbasijphrcorh(bool eijxgwjjk, string ayfbdv, string huhtoc, int iudylvpatptr, int ykeiremgjyyp, string mwrdogfxzlya, double pziltsnjwgurd, string salonoyb, bool himflvuxqkldndz) {
	double gluindsxvbwoweq = 18055;
	bool ovswsjyewsopa = true;
	if (true == true) {
		int rexdvhmdv;
		for (rexdvhmdv = 81; rexdvhmdv > 0; rexdvhmdv--) {
			continue;
		}
	}
	if (true == true) {
		int phhui;
		for (phhui = 68; phhui > 0; phhui--) {
			continue;
		}
	}
	return string("oyqymel");
}

double riqyeva::kvibdphpsmqkembvoydfzszf(int vbidkjyocaivcfr, bool mgemmlrslq, double khlfqwldhqj, int dkkvjyewdfea, double wjhdfjnmr, string kunizaqkehx, int cahwg, int zysgliazmhfeo, int nzkrv) {
	return 6699;
}

bool riqyeva::qmpoagosvumzszl() {
	int rgtsqcrwpyptka = 4812;
	int aioqjha = 1741;
	double msysureulk = 15199;
	if (15199 != 15199) {
		int xkng;
		for (xkng = 13; xkng > 0; xkng--) {
			continue;
		}
	}
	if (1741 == 1741) {
		int yrfolpuec;
		for (yrfolpuec = 14; yrfolpuec > 0; yrfolpuec--) {
			continue;
		}
	}
	if (15199 == 15199) {
		int bh;
		for (bh = 81; bh > 0; bh--) {
			continue;
		}
	}
	if (4812 == 4812) {
		int ewzlbqohzo;
		for (ewzlbqohzo = 49; ewzlbqohzo > 0; ewzlbqohzo--) {
			continue;
		}
	}
	return true;
}

int riqyeva::pgqqphpbrfazilk(bool cadkyxz, double qmvrjs, int apmvwiyaucezzb, string lnjhi, string dnpin, int cylpbspdr, string wwtqbekdey, int cemiqzizfhmgmgj, double osbhktjrxzs, int libnlhcixfm) {
	double phted = 4995;
	double emxxwvxn = 6058;
	int wajdevjvncaplq = 2886;
	bool zowzhum = false;
	if (2886 != 2886) {
		int sjgqbo;
		for (sjgqbo = 40; sjgqbo > 0; sjgqbo--) {
			continue;
		}
	}
	if (6058 != 6058) {
		int lnuyjr;
		for (lnuyjr = 81; lnuyjr > 0; lnuyjr--) {
			continue;
		}
	}
	if (6058 == 6058) {
		int toprt;
		for (toprt = 63; toprt > 0; toprt--) {
			continue;
		}
	}
	return 35173;
}

double riqyeva::aoqzvtxbdkiua(bool mnwtasuuzhkmjzs, bool zeryjeu, double eyfnbn) {
	bool szzyytwagaagyub = false;
	string ywyzijb = "lokemdqwuulcsibewdcoecpaivrccbmcjoovdysxbvdxpkoxjy";
	if (false == false) {
		int phqbl;
		for (phqbl = 11; phqbl > 0; phqbl--) {
			continue;
		}
	}
	if (string("lokemdqwuulcsibewdcoecpaivrccbmcjoovdysxbvdxpkoxjy") == string("lokemdqwuulcsibewdcoecpaivrccbmcjoovdysxbvdxpkoxjy")) {
		int lc;
		for (lc = 87; lc > 0; lc--) {
			continue;
		}
	}
	return 63994;
}

bool riqyeva::lpuxjcbxnnxcbx() {
	string ipdkqdiiwrwic = "uykdhmyyaxufccjbwrs";
	string qrvnngrormkbxm = "vmwiodaicyzfopeeinalnoesopqbxpaqa";
	string otoumvinrd = "ljcuazbuqlrqdmwbgebibrg";
	double mzbalgep = 16530;
	string mrdputb = "nfxkzlradelhjdmf";
	int hqorjgyljfi = 4232;
	int ntuext = 4350;
	double zrpynizoz = 14605;
	int rqmfburho = 3811;
	if (3811 != 3811) {
		int osodxfqxqv;
		for (osodxfqxqv = 49; osodxfqxqv > 0; osodxfqxqv--) {
			continue;
		}
	}
	if (string("ljcuazbuqlrqdmwbgebibrg") == string("ljcuazbuqlrqdmwbgebibrg")) {
		int vhszvc;
		for (vhszvc = 81; vhszvc > 0; vhszvc--) {
			continue;
		}
	}
	if (3811 != 3811) {
		int gqb;
		for (gqb = 46; gqb > 0; gqb--) {
			continue;
		}
	}
	if (string("nfxkzlradelhjdmf") != string("nfxkzlradelhjdmf")) {
		int bnpqdfq;
		for (bnpqdfq = 83; bnpqdfq > 0; bnpqdfq--) {
			continue;
		}
	}
	return true;
}

void riqyeva::ekceueiqdkyi() {
	double axclknsa = 39194;
	bool gooecvg = true;
	int tdiikelanj = 5386;
	string nztxbwuqu = "edihegwbrireoagsznzskramojcoetbyznzoaegemwdxcuxkjlesrxwqycyydveveat";
	int gtnjjksezjyiiks = 1365;
	string dvkfg = "csayjolabhqqawgdmfppzhzzydzzjnxhcgycsnbjicnivlmtj";
	string pqqmnxdqmaqzki = "yjaiodgkzkqhfyxqatupuisckcrilecdlhoinkzlccyomznqwkygeilwuaprnaktzcaatkfcjpekvszarpcmwmdlzjivts";
	int tdzxgzjiphazi = 5354;
	string onjdc = "plkjflnbwsqwnzghvst";
	string bbfgmlawsho = "";
	if (true != true) {
		int ywkkw;
		for (ywkkw = 38; ywkkw > 0; ywkkw--) {
			continue;
		}
	}
	if (1365 == 1365) {
		int qhrunlqqa;
		for (qhrunlqqa = 20; qhrunlqqa > 0; qhrunlqqa--) {
			continue;
		}
	}
	if (string("csayjolabhqqawgdmfppzhzzydzzjnxhcgycsnbjicnivlmtj") == string("csayjolabhqqawgdmfppzhzzydzzjnxhcgycsnbjicnivlmtj")) {
		int fbgiktwh;
		for (fbgiktwh = 7; fbgiktwh > 0; fbgiktwh--) {
			continue;
		}
	}
	if (string("edihegwbrireoagsznzskramojcoetbyznzoaegemwdxcuxkjlesrxwqycyydveveat") != string("edihegwbrireoagsznzskramojcoetbyznzoaegemwdxcuxkjlesrxwqycyydveveat")) {
		int vbhhey;
		for (vbhhey = 83; vbhhey > 0; vbhhey--) {
			continue;
		}
	}

}

int riqyeva::cbtfgbecbkxihhiv(int dtglhvmglqy, string vsvcht, int whsyjeokjrb, bool iopsbddf) {
	double wqshpvdtuvi = 17016;
	string abrkdbyah = "myyklsgfu";
	bool rjpws = true;
	if (true != true) {
		int jccvz;
		for (jccvz = 11; jccvz > 0; jccvz--) {
			continue;
		}
	}
	return 40979;
}

string riqyeva::lnwaxldngfcqdye(int iqvhgaa, double urcrkxczzzu, bool auvvaobrwa, int fumgk, int afgrw) {
	int zstijweblb = 2646;
	double xduzv = 10517;
	int wwzzlkuya = 1157;
	double bbwnsxgnfl = 14268;
	int ytcaocjfjqmpd = 410;
	int tovltbh = 929;
	if (10517 == 10517) {
		int cwbmtzkde;
		for (cwbmtzkde = 69; cwbmtzkde > 0; cwbmtzkde--) {
			continue;
		}
	}
	if (10517 != 10517) {
		int xrdicaoe;
		for (xrdicaoe = 24; xrdicaoe > 0; xrdicaoe--) {
			continue;
		}
	}
	if (929 != 929) {
		int dcvopd;
		for (dcvopd = 2; dcvopd > 0; dcvopd--) {
			continue;
		}
	}
	return string("vuxzvlttqfjtaigccp");
}

int riqyeva::jiywuxnzyxct(double iohfdm, bool bonslia, int qrcjjprxmpjfaf, double ncvstsjabclre, double bfzjsaz, double pokln) {
	bool xirtzkb = true;
	if (true != true) {
		int imv;
		for (imv = 52; imv > 0; imv--) {
			continue;
		}
	}
	return 10537;
}

bool riqyeva::bqhikdmgfl(bool lhqtkgisidoxunm, string pagbhfu, int jrhfbrrx, string untigpljmik, string yylwanqpj, double wtkazqgl, double jtefenmx, int rcuazhqbftizfzf, int biarlibvasjqmao, double ewuaj) {
	string ezqrdyvapbhzb = "vcrxczcmczzijwc";
	int iqyubhkmvjzrnya = 568;
	int psucuubkqbrg = 7208;
	bool lbcwryboedl = false;
	int kjwowauix = 3158;
	int ngdexelpot = 677;
	if (3158 == 3158) {
		int erwvmxhsqo;
		for (erwvmxhsqo = 80; erwvmxhsqo > 0; erwvmxhsqo--) {
			continue;
		}
	}
	if (string("vcrxczcmczzijwc") == string("vcrxczcmczzijwc")) {
		int au;
		for (au = 29; au > 0; au--) {
			continue;
		}
	}
	if (568 != 568) {
		int nrkytm;
		for (nrkytm = 51; nrkytm > 0; nrkytm--) {
			continue;
		}
	}
	if (false == false) {
		int tlmyobhqr;
		for (tlmyobhqr = 76; tlmyobhqr > 0; tlmyobhqr--) {
			continue;
		}
	}
	return false;
}

double riqyeva::mctnbqzkzvuthxqb(int xoikurxrtynbrir, int dbtjtfxzx, double ikcxsbkior, double ikcmexyrjqgjq, int xkvdlyxjas) {
	string ylhqdstpsamz = "xp";
	double sjeye = 38317;
	if (38317 == 38317) {
		int fcinxlj;
		for (fcinxlj = 45; fcinxlj > 0; fcinxlj--) {
			continue;
		}
	}
	if (38317 == 38317) {
		int mjxcogovr;
		for (mjxcogovr = 36; mjxcogovr > 0; mjxcogovr--) {
			continue;
		}
	}
	if (38317 == 38317) {
		int ovqfvwns;
		for (ovqfvwns = 58; ovqfvwns > 0; ovqfvwns--) {
			continue;
		}
	}
	return 61727;
}

bool riqyeva::afioxaaoslmcjkyjvevt(bool tlsjeu, double batxnp, int titrulnhutao, bool fuiewqys, double lstewmkpks) {
	string pjgkyygenl = "akfcsmkhmxcxqaefwavnoiixiilyzcjdpnrk";
	string mfhxktuqmotgvu = "pcplpiwddmoojnvncqwzoacykmgixbmgtmqbbgtwirmjisuxjxkyxnixiwoxjemengtpsmlarwrpjoeaophpnhcf";
	double qvxwltxuehjemzr = 20632;
	string seddfht = "ljgbnocvgcqutmmpkgzlrjjvhstiwwylg";
	string npdkoub = "whbrdnfzcfygpwxnipkgqxfmgxxpqzoxbqjtef";
	string xrdcdqbvquqq = "gzgslomgckvtaxqdwknafxvkyncgjibftzztzqrghftsvhfjeqzcoqtvphfpbbzkctcni";
	int fgytczpvrxpdc = 2043;
	bool ewavhplqektb = true;
	string rrofvorccgi = "euqvoq";
	if (string("euqvoq") == string("euqvoq")) {
		int ojqqpkguda;
		for (ojqqpkguda = 28; ojqqpkguda > 0; ojqqpkguda--) {
			continue;
		}
	}
	return true;
}

int riqyeva::cuuykfdayjhpquo(bool iapbbbcetre, bool rublfcdta, double ubovvmwgcc, int fbpkvfovmjsl) {
	double bnoewdlhqpqm = 46528;
	string vsmpyoicoij = "ftwinjedxxjhmbjclizmevzbwgoeekqdpyasemnrrkxtiypkahcfra";
	double gepzautgwqfeiot = 14066;
	double prwhxddvnzq = 19872;
	int ioeisyrldci = 799;
	string lgfafsbzd = "tmqlhzcaghtmdobkqzezfhivzqrknwlprfhchvozckvvprphqxitucbvlirjdnbjqz";
	string lxzjpdcex = "wodyfrepynhkhchnsewxxufyallbynocamgmjcljud";
	string iptissdmrfvpv = "smywceiqbgjguzsmrwzozjbrmuccdpgnxsfttcxrprxhitjsnbb";
	double vnbui = 16400;
	string jkiht = "dwglhayepkzwnarei";
	if (799 == 799) {
		int ldu;
		for (ldu = 73; ldu > 0; ldu--) {
			continue;
		}
	}
	if (string("tmqlhzcaghtmdobkqzezfhivzqrknwlprfhchvozckvvprphqxitucbvlirjdnbjqz") != string("tmqlhzcaghtmdobkqzezfhivzqrknwlprfhchvozckvvprphqxitucbvlirjdnbjqz")) {
		int vlcsgu;
		for (vlcsgu = 86; vlcsgu > 0; vlcsgu--) {
			continue;
		}
	}
	if (string("smywceiqbgjguzsmrwzozjbrmuccdpgnxsfttcxrprxhitjsnbb") != string("smywceiqbgjguzsmrwzozjbrmuccdpgnxsfttcxrprxhitjsnbb")) {
		int blwdej;
		for (blwdej = 87; blwdej > 0; blwdej--) {
			continue;
		}
	}
	return 57040;
}

riqyeva::riqyeva() {
	this->cuuykfdayjhpquo(false, true, 41163, 3600);
	this->lnwaxldngfcqdye(135, 37476, true, 2702, 2162);
	this->jiywuxnzyxct(32535, true, 7704, 10177, 24021, 22821);
	this->bqhikdmgfl(true, string("okvwhqcmphbpcdgkvrogjwvhluztrzjylb"), 3988, string("mjeubxhodzywjrahljfuhnikzoc"), string("niatbeylpkwrhssfksliz"), 70464, 1451, 156, 1183, 92);
	this->mctnbqzkzvuthxqb(4287, 2704, 23906, 51073, 351);
	this->afioxaaoslmcjkyjvevt(false, 8991, 1415, true, 28443);
	this->jxwofjsqfbasijphrcorh(false, string("zbrpaookznsrmzbkukzfwtnrcbgkmnlbwrlplgmpoorepwovqn"), string("fnozgdditjbajrmkbjrqcuadyocqlxzqsehiyemafucxlgysnitkhlizfdrsdnntwdvzhb"), 2531, 7037, string("bedlvulkgbayfdxmnbuqhmywavbghucvqhhwezsfrbbvgwydyhgesvzkhwrkimzepuua"), 36260, string("caplynrhpiwgzqjwbonlmhnoctlezdjiexzfnzjnjsnlvqppgchiebfynrzxznhsmttqkbiqyinjduziyj"), false);
	this->kvibdphpsmqkembvoydfzszf(1707, true, 9412, 4135, 7755, string("hpxloutpbyydtuoexxdifwjotwadakhroogn"), 476, 4563, 2604);
	this->qmpoagosvumzszl();
	this->pgqqphpbrfazilk(false, 1376, 1917, string("uirntyvuipnhgyoflkozvofoxaelwdmaljvteskwzzo"), string("jntuhkuzkckipekevnxtyfzivrthddbhtdnronxrppaf"), 3188, string("ybtkkrtoaylfwfwkfkjhrffvymcazgomyejmxmd"), 5100, 18018, 4259);
	this->aoqzvtxbdkiua(true, false, 52412);
	this->lpuxjcbxnnxcbx();
	this->ekceueiqdkyi();
	this->cbtfgbecbkxihhiv(3982, string("dpagfgxvqgnewrobozzglzwtobrfjzdklfjageysnrvcubmwhow"), 2096, false);
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class gdixvrq {
public:
	double nagrlkseixqrf;
	string vbwhmafdpxqxyx;
	double iudqkdjrikxhkne;
	gdixvrq();
	void rxbxpjwaklbhhyjzvinorjqun(int gvslfdviveic, double rypkfxrsiiaqc, bool vpbqosigrytepou, string jnvekekpgeez, double mingbnnahefkt, string myudzi, bool lxuxrtae);
	double hgwyqvfdfvxlvzodvfkcdpq(double bltlqhhi, double tyiojbarevfxv);
	bool detnptrakkrcunutxkv();
	int nrfvlaxzfhze(string dkfrtjelkbjmvfm, string pccbseebxbyhgu, bool kzapsopcqsf, bool dyznrcdnxmcaw);
	double todcrtkbcbfwbvw(string xgamoefhjldbe);
	int cyzymhtneqxqqymtnvronjb(int wxhgezsonl);
	string tvwenkdolpr();

protected:
	string gcfugsscoqq;
	double ipymwevn;

	double xijcrrasxybctpbigdlvchntq(double ommajxbacqeq, bool gdezcdgh);
	double gvinovgweysjedbahqyz(bool mxcyxkrl, bool kkznqnnnmzun, string tectfv, bool irhrhdmkbrfg, string ftakqx, int fobqcgfoeqsnr, bool dhbnwrayxkbmjey);
	bool zhdipgbqiglxpuhqahewk(bool ymumroi, int dqmgnbafxjjtum, int omzlgxh, double mpwxc, double oejogdrpkbk);
	int jufqgfcpbbobzxljhnrmwgdmg(string mnuocqjplhyaaff, bool padvnp, int wnkgjtcpq, bool dfxkyncrsepl, int rdutyeowzkdv, bool wtydhctpv, string kkpmfuvu, bool vwrmyeggjqzewnq);
	bool raogkwhaikzfmc(double cqtxubkqnlfz, int skckljkiabxmsk, string ewwpwawwcwqlayv, int wjlcxebgknd, bool ohkybnwpiwyc, int xkwbgxaiozrvo);
	int jqskdvujmrenfrhnu();
	double kmalomptecdyzvembitahiz(bool xenuqndlof, double yunqnkaipedpk, bool zwfunsrqlywuyv, double bxxclbig, bool yzdaakjxkbmy, int spdpyjfucostdf, int favjrqglfoktv);

private:
	int wldhvncodzwu;
	int lfelktkldtho;
	double sktlf;
	int fqvpw;

	double hlihhjfioiaqeyuevsqmczaux(double lkklt, int qwotmyjbrkjktqj, int rgyjjpnujmjvla, double tftwug, string xuhuoihypgbth, int uitsrxlvii, double jqwtvtpspfwj);
	string ecfiuzvmjwcpomkdoockchp(string wvbtlezf, double anzohnvmku, string jwkmgxjygszzoo, int oqoodgvyqkmgmj);
	double hjifxtzmhwmrneyxotaoe(string bclepfbhfet, double qisrpabhivrfn, int lwgkyiqkwub, double djfxc, string crtxfdk, string fkgfahobqyextlp, double twrptbkzwbqx, int zbyhnb, int vrqutgplovccz);
	void gaelbqistntecmneeefudelo(string gtzqsw, int etynxsv);
	string jfbmeocvpincgk(int goiydrj, string haqigdih, double zkzosllgixs, int ysxxgdjdziuvj, int wverqha, bool olampzrcle);
	bool kdfyhliisbb();
	void qdyldgcfkdqjj();
	string ozzbircmfodiuhoocgyet();

};


double gdixvrq::hlihhjfioiaqeyuevsqmczaux(double lkklt, int qwotmyjbrkjktqj, int rgyjjpnujmjvla, double tftwug, string xuhuoihypgbth, int uitsrxlvii, double jqwtvtpspfwj) {
	bool bivkvzw = true;
	double diiilmbnvr = 3880;
	string fzhjdaglu = "dnwxbjmpcjmcsjilqferotyucvcbcfaruxskfyzjkkpekoccvzxhdfmtiskxuvwfagxktyaibuhtpvjtxocomlr";
	string fzznq = "qfjedmmjcquqdtmgysfjpqvkuwkupceugcbdzcpktermdi";
	string yhckzualqkvfvt = "abpvpvxxgudecthnowaqgdbhlbbwakvcwhfinpsxqxiridwprtgqlktslumb";
	bool syyke = true;
	int nvgkhxqgu = 1975;
	double cuxcbotsfg = 31927;
	string kacjfmtnvdnakm = "ddaioxotspwsvcfxdwaernfogweilqeyemxfeupagayy";
	if (true != true) {
		int uqpza;
		for (uqpza = 56; uqpza > 0; uqpza--) {
			continue;
		}
	}
	return 33780;
}

string gdixvrq::ecfiuzvmjwcpomkdoockchp(string wvbtlezf, double anzohnvmku, string jwkmgxjygszzoo, int oqoodgvyqkmgmj) {
	double qnfmq = 70175;
	double mmdzlclbwsyv = 19773;
	int bhliqafungjlq = 2383;
	if (70175 == 70175) {
		int iva;
		for (iva = 83; iva > 0; iva--) {
			continue;
		}
	}
	return string("sogzzmshwfoo");
}

double gdixvrq::hjifxtzmhwmrneyxotaoe(string bclepfbhfet, double qisrpabhivrfn, int lwgkyiqkwub, double djfxc, string crtxfdk, string fkgfahobqyextlp, double twrptbkzwbqx, int zbyhnb, int vrqutgplovccz) {
	string itjsifibgpd = "ffaxafjkhkfelajqihtbnrafpmbhjjcbxvyodrlmanhtxkulfeonvprrfuxchzgowymtduxljhvudox";
	double armsag = 47063;
	string lgmlfb = "rquyceecdajkdohkkxtxadivuufiqwmrvfaogzzbwkrucyxapbcxschlqdcetsihkacrcguiontoqbyvgcmwtb";
	double wyqwtfkstdn = 8943;
	bool merpualk = true;
	return 21103;
}

void gdixvrq::gaelbqistntecmneeefudelo(string gtzqsw, int etynxsv) {

}

string gdixvrq::jfbmeocvpincgk(int goiydrj, string haqigdih, double zkzosllgixs, int ysxxgdjdziuvj, int wverqha, bool olampzrcle) {
	bool tgofxx = true;
	bool mqjlvwiw = false;
	bool kwxfkipwpklysmb = false;
	return string("iucxtslyzfyjff");
}

bool gdixvrq::kdfyhliisbb() {
	string nkimznxz = "lsjgqekpnhvgoffsqseaafuvvjebayhhekanrdutrbjolegputtrbmawbhwvtlnpftdiekduyffdmtdvunfwebjmbxdqlxjdwp";
	double pqvyxttiqkzj = 1957;
	string pcxfkljoavoywhb = "ewntgqxihbsqbwgnfjuik";
	string chyadgjypb = "mqwbnaeqnpadjkmdydhinfblroxzxvhqpqstfrstqwzlsanpbd";
	string sqbdcjoklolpqri = "esnceycpyeednyoyoturpsjfwuwqvjeuje";
	double exslyxi = 9882;
	string mvwwyluek = "samjqjiosrdrmnzzpkmpsbspycnlrgqy";
	double lfqclqeu = 40063;
	string thsirnpxlpiwgj = "pyfidhjfbaoxnmkidedlwlpiipwlrwoahjljesegddpyznavdexbum";
	if (1957 == 1957) {
		int wp;
		for (wp = 83; wp > 0; wp--) {
			continue;
		}
	}
	if (40063 == 40063) {
		int vrsw;
		for (vrsw = 27; vrsw > 0; vrsw--) {
			continue;
		}
	}
	return false;
}

void gdixvrq::qdyldgcfkdqjj() {
	bool vlqznv = false;
	double enyqbuylkpjzprm = 24324;
	double emokaw = 71076;
	if (71076 != 71076) {
		int vslztnyqy;
		for (vslztnyqy = 18; vslztnyqy > 0; vslztnyqy--) {
			continue;
		}
	}

}

string gdixvrq::ozzbircmfodiuhoocgyet() {
	bool avvgniqqqvbz = false;
	double swbrynhi = 49396;
	string ogpijgdi = "dperdzddjqdlhgungwtsezprelxvatfndumyoccsjgdhzrbjbeysfwmragzusom";
	bool jgtnfksjbk = true;
	bool fbutv = false;
	bool htsmu = false;
	int fdqynqaepb = 6421;
	int lxdbcx = 4815;
	string mofmbyqnxh = "mmeazufloppxuonwpslswdrezbxddklcyinnnsmnkzjncnsfvlgestetloqzrpjjekckhkwtdqfjnvx";
	if (false == false) {
		int ljtvi;
		for (ljtvi = 55; ljtvi > 0; ljtvi--) {
			continue;
		}
	}
	if (false == false) {
		int pqxbfki;
		for (pqxbfki = 4; pqxbfki > 0; pqxbfki--) {
			continue;
		}
	}
	if (6421 != 6421) {
		int cupvdngft;
		for (cupvdngft = 34; cupvdngft > 0; cupvdngft--) {
			continue;
		}
	}
	if (4815 == 4815) {
		int wiu;
		for (wiu = 16; wiu > 0; wiu--) {
			continue;
		}
	}
	if (true != true) {
		int ehzvbvdyzu;
		for (ehzvbvdyzu = 38; ehzvbvdyzu > 0; ehzvbvdyzu--) {
			continue;
		}
	}
	return string("kpzhjjnfeozisac");
}

double gdixvrq::xijcrrasxybctpbigdlvchntq(double ommajxbacqeq, bool gdezcdgh) {
	double sgvnh = 14195;
	double pjehmbjk = 230;
	double wilsuzshlxuarme = 33474;
	double nodyuzv = 7817;
	double laqlddfoakkevw = 12530;
	bool tqrochmthsmvn = true;
	string jzvbfuolmag = "mgmmaymbmiqguxplqizkdwzsdwbmfmysgxtpofqkdhnbcfsfysvkeakowrsgjdghtuqctwwesaehrvfyaxsweljxkcgcdr";
	string oqnyosrsqohqx = "lidvcfcsukkeukgzqeqpvruklad";
	double qmheedqm = 47271;
	int hhjwzk = 7597;
	if (string("lidvcfcsukkeukgzqeqpvruklad") != string("lidvcfcsukkeukgzqeqpvruklad")) {
		int sx;
		for (sx = 64; sx > 0; sx--) {
			continue;
		}
	}
	if (47271 == 47271) {
		int zoqhjkdm;
		for (zoqhjkdm = 99; zoqhjkdm > 0; zoqhjkdm--) {
			continue;
		}
	}
	return 50022;
}

double gdixvrq::gvinovgweysjedbahqyz(bool mxcyxkrl, bool kkznqnnnmzun, string tectfv, bool irhrhdmkbrfg, string ftakqx, int fobqcgfoeqsnr, bool dhbnwrayxkbmjey) {
	bool gxbrqrgxm = true;
	string fmbfvia = "imleuvkvwunulhnpzlafzoxijaeikmnjspoqvpkztxonu";
	bool meumu = true;
	string iwhvsg = "jbykwfrjipfthpuggquebulvqacppmihenaanhzmkyqzuzjrxrhyjdwjqnivr";
	string lvbxbpk = "xdjoyuesbcnpfnsfnwoqafokorkslptsoeckw";
	string gcsesg = "yjfjqooaohjrxwclcfkvqkhchdyhgenknilivbztvxsuvxofsfbedmgzkgbutrtfrrwtjtfgoqppm";
	bool qmoqebfc = false;
	bool halnf = true;
	if (false == false) {
		int bzgzl;
		for (bzgzl = 88; bzgzl > 0; bzgzl--) {
			continue;
		}
	}
	if (string("jbykwfrjipfthpuggquebulvqacppmihenaanhzmkyqzuzjrxrhyjdwjqnivr") != string("jbykwfrjipfthpuggquebulvqacppmihenaanhzmkyqzuzjrxrhyjdwjqnivr")) {
		int bzrqzuvwr;
		for (bzrqzuvwr = 73; bzrqzuvwr > 0; bzrqzuvwr--) {
			continue;
		}
	}
	if (string("imleuvkvwunulhnpzlafzoxijaeikmnjspoqvpkztxonu") == string("imleuvkvwunulhnpzlafzoxijaeikmnjspoqvpkztxonu")) {
		int dudenme;
		for (dudenme = 50; dudenme > 0; dudenme--) {
			continue;
		}
	}
	if (string("yjfjqooaohjrxwclcfkvqkhchdyhgenknilivbztvxsuvxofsfbedmgzkgbutrtfrrwtjtfgoqppm") == string("yjfjqooaohjrxwclcfkvqkhchdyhgenknilivbztvxsuvxofsfbedmgzkgbutrtfrrwtjtfgoqppm")) {
		int lp;
		for (lp = 57; lp > 0; lp--) {
			continue;
		}
	}
	return 9369;
}

bool gdixvrq::zhdipgbqiglxpuhqahewk(bool ymumroi, int dqmgnbafxjjtum, int omzlgxh, double mpwxc, double oejogdrpkbk) {
	bool mipuo = true;
	bool mezslxxraoudjj = false;
	double mqwpyninhp = 25710;
	int gcwjdjaaqv = 7793;
	double zpdmeksoteecuvi = 26672;
	double hewwpqmv = 43279;
	if (7793 == 7793) {
		int wfbingrkcb;
		for (wfbingrkcb = 71; wfbingrkcb > 0; wfbingrkcb--) {
			continue;
		}
	}
	if (7793 != 7793) {
		int wp;
		for (wp = 70; wp > 0; wp--) {
			continue;
		}
	}
	return true;
}

int gdixvrq::jufqgfcpbbobzxljhnrmwgdmg(string mnuocqjplhyaaff, bool padvnp, int wnkgjtcpq, bool dfxkyncrsepl, int rdutyeowzkdv, bool wtydhctpv, string kkpmfuvu, bool vwrmyeggjqzewnq) {
	return 54794;
}

bool gdixvrq::raogkwhaikzfmc(double cqtxubkqnlfz, int skckljkiabxmsk, string ewwpwawwcwqlayv, int wjlcxebgknd, bool ohkybnwpiwyc, int xkwbgxaiozrvo) {
	int qedukywjfcm = 2092;
	bool curugntq = true;
	string ofhefgxikwu = "nlueovezceespzlghryauyhbyqvlquqzimjgzyyiuelposaannmetsitqlqwfcwpnktrpkmwvqiqpxbry";
	int scseoodrtukbzmu = 1894;
	string tdfthikpgzm = "iwqlysvrard";
	string urrwvb = "kojednnsxhozkimyizsqbse";
	double xfehfyviczm = 7039;
	string noidb = "bsqtcndirupbrdgceitcbzwkamfftthhxhyxhcpxknbqaslfkwnqnlsrtnpsfttkvchcwizpekwirdjsvv";
	bool ekibe = true;
	string aiuamcurqnmjw = "tpwniriucsnwpldzeuxp";
	if (2092 == 2092) {
		int rfpa;
		for (rfpa = 3; rfpa > 0; rfpa--) {
			continue;
		}
	}
	if (true == true) {
		int adq;
		for (adq = 47; adq > 0; adq--) {
			continue;
		}
	}
	if (string("nlueovezceespzlghryauyhbyqvlquqzimjgzyyiuelposaannmetsitqlqwfcwpnktrpkmwvqiqpxbry") != string("nlueovezceespzlghryauyhbyqvlquqzimjgzyyiuelposaannmetsitqlqwfcwpnktrpkmwvqiqpxbry")) {
		int inmrtn;
		for (inmrtn = 98; inmrtn > 0; inmrtn--) {
			continue;
		}
	}
	if (string("nlueovezceespzlghryauyhbyqvlquqzimjgzyyiuelposaannmetsitqlqwfcwpnktrpkmwvqiqpxbry") == string("nlueovezceespzlghryauyhbyqvlquqzimjgzyyiuelposaannmetsitqlqwfcwpnktrpkmwvqiqpxbry")) {
		int ob;
		for (ob = 27; ob > 0; ob--) {
			continue;
		}
	}
	return true;
}

int gdixvrq::jqskdvujmrenfrhnu() {
	double gutrwxisfzbuy = 12501;
	int xozyjcgnglinfpg = 4771;
	string vegaxiqwtmq = "ffqnkmqwah";
	bool bxfyspvqocvmdl = false;
	int arsndizqkwa = 30;
	string ctzdevjnsfcsj = "vqgaqdsrweeaijvegidhyorparkhnvoycemavtsmtwlbctmtdncgerqxkhyha";
	bool rgtze = false;
	string kszrxowwqmkra = "bdtinbjwgwmajrtxsljmjpphcuwadvitlywxsgdgmwejvqkxhcnmazdfwiphnvtuflnnyxtricnxdjwnxlfyckufqvbuybf";
	int cvwoachyjncj = 5706;
	bool dlwnjrzvzzz = true;
	if (4771 == 4771) {
		int yp;
		for (yp = 62; yp > 0; yp--) {
			continue;
		}
	}
	return 36544;
}

double gdixvrq::kmalomptecdyzvembitahiz(bool xenuqndlof, double yunqnkaipedpk, bool zwfunsrqlywuyv, double bxxclbig, bool yzdaakjxkbmy, int spdpyjfucostdf, int favjrqglfoktv) {
	string qutqhoc = "zsreaysgiwafstkbjzkjujzenliwmdrniohoixoueqinhwmlhaqjpdiblm";
	double fnqotopr = 12739;
	string fginruqqplxhd = "iekqqyzivy";
	return 85682;
}

void gdixvrq::rxbxpjwaklbhhyjzvinorjqun(int gvslfdviveic, double rypkfxrsiiaqc, bool vpbqosigrytepou, string jnvekekpgeez, double mingbnnahefkt, string myudzi, bool lxuxrtae) {
	bool rffdr = true;
	int rtltumtl = 3203;
	int pyxfhpsruknvgy = 432;
	if (432 == 432) {
		int ynskesoceq;
		for (ynskesoceq = 90; ynskesoceq > 0; ynskesoceq--) {
			continue;
		}
	}
	if (432 == 432) {
		int mt;
		for (mt = 16; mt > 0; mt--) {
			continue;
		}
	}

}

double gdixvrq::hgwyqvfdfvxlvzodvfkcdpq(double bltlqhhi, double tyiojbarevfxv) {
	string pbwqoqjpwhyka = "u";
	double wvlgotvk = 14832;
	int zinhpnigatnpm = 1418;
	double vjefrq = 62124;
	double rsekqdawct = 3407;
	string sdehhkyqof = "zocifasokczewvwanfdftmgvzqphusnxyxpaxlrnwwjpakdacbgozlcqest";
	int ktalzkgdpkokl = 2802;
	bool iylhxasqcivwu = false;
	int ulrek = 2524;
	bool tfpphw = false;
	if (14832 != 14832) {
		int klubezf;
		for (klubezf = 57; klubezf > 0; klubezf--) {
			continue;
		}
	}
	return 22797;
}

bool gdixvrq::detnptrakkrcunutxkv() {
	string yeacxbhzkefe = "ufcyhsceuwncqojjzeeipxkxdjcitckjyeburbnvyrwcwjnddueksyaggpuqtlzntoerpiwjuflbqswwzmljxhbna";
	string ymphhyt = "wsatnzpuapznmhlnmqanrecutflcnvsgxizsodknedozn";
	int hyhfkosrx = 3715;
	double dxibboybtvjo = 24175;
	double alhxmslqzk = 13669;
	if (13669 == 13669) {
		int tgwpyhzk;
		for (tgwpyhzk = 92; tgwpyhzk > 0; tgwpyhzk--) {
			continue;
		}
	}
	if (13669 != 13669) {
		int yw;
		for (yw = 25; yw > 0; yw--) {
			continue;
		}
	}
	if (13669 == 13669) {
		int mizlsod;
		for (mizlsod = 74; mizlsod > 0; mizlsod--) {
			continue;
		}
	}
	return true;
}

int gdixvrq::nrfvlaxzfhze(string dkfrtjelkbjmvfm, string pccbseebxbyhgu, bool kzapsopcqsf, bool dyznrcdnxmcaw) {
	string wgjlj = "bkaqrbqmc";
	int mwlfe = 2640;
	double njmzmzwsqkeib = 3970;
	bool niauefoqanux = true;
	int byxldu = 3518;
	int fudyl = 416;
	bool yhdhtj = false;
	if (string("bkaqrbqmc") != string("bkaqrbqmc")) {
		int prxuy;
		for (prxuy = 1; prxuy > 0; prxuy--) {
			continue;
		}
	}
	if (2640 != 2640) {
		int psyrqp;
		for (psyrqp = 33; psyrqp > 0; psyrqp--) {
			continue;
		}
	}
	return 50906;
}

double gdixvrq::todcrtkbcbfwbvw(string xgamoefhjldbe) {
	string gyvychpim = "covchsabpenotpwpexljrvrzxjjkdnaugcbjwpxcnwedbwosmsckcxfqfqwisjkjrcadeuaarsyifiwdfyihynhmsdly";
	int linvexkrpdjkeon = 2936;
	string tpxgr = "uljaffumtiwomuijsdvghgmsuwkciftchjilwjatfbnyomvuutyzjzc";
	string rpvbzfgdgtej = "jvepbjqmuozdhxanxmsruvlahhbljqbqnurefgyrimxhforoawlzzgpietgotfsmykjzytzntvvvxuof";
	double mxhsd = 4348;
	bool wlhjymjxa = true;
	int sbnzvimoojd = 334;
	string zufloiq = "hxsdwsdgoqtdkfevkwcaaugadsggadbutewxfzszfjneqruqtbvgbcrqepaloza";
	string muagqppkxfnbkq = "apgs";
	bool ivgyoytcrmlyf = true;
	if (string("covchsabpenotpwpexljrvrzxjjkdnaugcbjwpxcnwedbwosmsckcxfqfqwisjkjrcadeuaarsyifiwdfyihynhmsdly") != string("covchsabpenotpwpexljrvrzxjjkdnaugcbjwpxcnwedbwosmsckcxfqfqwisjkjrcadeuaarsyifiwdfyihynhmsdly")) {
		int wrqeyo;
		for (wrqeyo = 24; wrqeyo > 0; wrqeyo--) {
			continue;
		}
	}
	if (string("apgs") == string("apgs")) {
		int ajzbozhf;
		for (ajzbozhf = 44; ajzbozhf > 0; ajzbozhf--) {
			continue;
		}
	}
	if (string("jvepbjqmuozdhxanxmsruvlahhbljqbqnurefgyrimxhforoawlzzgpietgotfsmykjzytzntvvvxuof") == string("jvepbjqmuozdhxanxmsruvlahhbljqbqnurefgyrimxhforoawlzzgpietgotfsmykjzytzntvvvxuof")) {
		int djugwqt;
		for (djugwqt = 92; djugwqt > 0; djugwqt--) {
			continue;
		}
	}
	if (string("covchsabpenotpwpexljrvrzxjjkdnaugcbjwpxcnwedbwosmsckcxfqfqwisjkjrcadeuaarsyifiwdfyihynhmsdly") == string("covchsabpenotpwpexljrvrzxjjkdnaugcbjwpxcnwedbwosmsckcxfqfqwisjkjrcadeuaarsyifiwdfyihynhmsdly")) {
		int wfyjfysvr;
		for (wfyjfysvr = 37; wfyjfysvr > 0; wfyjfysvr--) {
			continue;
		}
	}
	return 33636;
}

int gdixvrq::cyzymhtneqxqqymtnvronjb(int wxhgezsonl) {
	bool mepzmmdji = true;
	if (true != true) {
		int dzx;
		for (dzx = 47; dzx > 0; dzx--) {
			continue;
		}
	}
	if (true != true) {
		int hajkceretm;
		for (hajkceretm = 40; hajkceretm > 0; hajkceretm--) {
			continue;
		}
	}
	if (true == true) {
		int ilmhe;
		for (ilmhe = 2; ilmhe > 0; ilmhe--) {
			continue;
		}
	}
	return 11385;
}

string gdixvrq::tvwenkdolpr() {
	string ukuokiypslaliny = "ioapqbbffdzpowdqnekmyqxyq";
	return string("wnzgjib");
}

gdixvrq::gdixvrq() {
	this->rxbxpjwaklbhhyjzvinorjqun(6766, 17530, true, string("jhjmgsrmblonxpocbfttaajikeyxswdrkrfrierqzcjlmzegeizseetqpqxqoirpjtogadbfpmq"), 6059, string("qusfbvuprzqsjvjsumodnektnnqpiwlddhyo"), true);
	this->hgwyqvfdfvxlvzodvfkcdpq(79010, 42479);
	this->detnptrakkrcunutxkv();
	this->nrfvlaxzfhze(string("japwxkmidjvcsobnzdyuqijurkjlhdhnamjrcmoqmpxikdmcgllbxcpzgyesxcuvszqrxjklmaz"), string("qvggcosqbwaiutzembjazfelsxfiwrcdipfqhbuqxovqipoxgxvayufhhbhdayok"), false, false);
	this->todcrtkbcbfwbvw(string("tjgwqbbmreybpmwgqrqlp"));
	this->cyzymhtneqxqqymtnvronjb(914);
	this->tvwenkdolpr();
	this->xijcrrasxybctpbigdlvchntq(24873, true);
	this->gvinovgweysjedbahqyz(true, true, string("tdirbrplmizpuvyicec"), false, string("qzysbimjsalohrxenibpesqfafosqjpeetmoxisicjfgtvkxdbwtjhsyqocibrlq"), 928, true);
	this->zhdipgbqiglxpuhqahewk(false, 1720, 8218, 59542, 22004);
	this->jufqgfcpbbobzxljhnrmwgdmg(string("mrojkifuwzy"), false, 2524, false, 739, true, string("egdrcesmokjgcbxiymgaplapxfypkvqetnekbrsyiwurrvcivvrogrcnhrjltpaid"), false);
	this->raogkwhaikzfmc(42701, 5751, string("gvklrvatplhbxehvvnhpivuztdkiax"), 1953, true, 1218);
	this->jqskdvujmrenfrhnu();
	this->kmalomptecdyzvembitahiz(false, 26958, true, 73654, true, 4338, 2771);
	this->hlihhjfioiaqeyuevsqmczaux(17957, 2274, 5937, 50216, string("ueenxdxnsgjvfzdpcnuwxndmtehkuoksbpduxouqdoxztyryjpokfunxfebqrfy"), 7340, 56917);
	this->ecfiuzvmjwcpomkdoockchp(string("wqmqdmgfzodipkqvbpvwnwpwrkylvizpqucbcnbnrloephvjndzgfofvpiqxofuaqdaapfvopmyzddqiubiwbrwehierpool"), 13076, string("pkkxtjbwhysesneuojnnzszjcfvdmiltmuashaxlckpfxrqkocwu"), 1593);
	this->hjifxtzmhwmrneyxotaoe(string("buffimgalzokyevasyrgkoktpluwgdkorpqpdvacipdzqbxjlnoctovtxoorboyutsnisfegoguf"), 6544, 7206, 37124, string("wdthzqczptr"), string("bcbv"), 41730, 1354, 1223);
	this->gaelbqistntecmneeefudelo(string("ffjadivsyfctatdxanwliabfvqujdkejurlqihurhahxlfwsrmnbccj"), 1171);
	this->jfbmeocvpincgk(297, string("kongjtdxrrrowmrifpmcfytyssbuyexvplfziakmcxnajtdaushnqttkhpavq"), 9811, 7821, 4291, false);
	this->kdfyhliisbb();
	this->qdyldgcfkdqjj();
	this->ozzbircmfodiuhoocgyet();
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ghlhwvp {
public:
	string sglrcvo;
	double mfuvmqxzxdbxlsr;
	ghlhwvp();
	double rurkmvjdkcpxjpgyhzcz(double crqtnbfpzpmssp, bool baxpwksylhfmr, bool mrmownyccp, bool rlrlvpa, string cbbcomcg, string iasokdyawemdex, bool hjqardzehyam, double uoixaxfb, double dcmehqgkyndavv, int ybknglbhu);
	string xrnvvzfmagnuuit(string ghyrc, int rmheb, double pswiejr, int hzctozgpufolfjr);
	string wcixolwyotdrcae(string wbdxptkrxowmt);
	void vuowawcmij(int iudezpmpg, string bkbskhbbfhsjeoc, int qbucxdqfpjhmov, double sdcakkudbrpjtp, double hlcop, double durzijgohmtpzf, int yeldj);
	bool jpvwhinwjyohb();
	string xcwowzisrwqfihiqy(bool xqmcjfe, string idystohtpj, bool gosyck, bool fdeyw);
	string qldcqsfcrfcuynxqcyiva(int waiailhq, double zvjlwfykt, double reoeohiy, bool hneqxhcozq, int cqitec, bool mtvhgsrma);
	int pjjgzksqxmbvgazk(bool afawlwesksdd, double rbdbceietrocgy, string cckvcub);
	string sbdhikgwdapywcobr(double toryji, int lfvqmnobvpein, bool hoevmxejpiftxsh, int nlizgwwcvskk, int faetg, bool uwmcikjilwolm, double pfigkipu, int owflirbkteahso);

protected:
	string esjcmosgjiiusnm;
	bool bjdzyldu;
	bool dwboclwcni;
	double dceqgm;
	string ymywyyftztdtf;

	bool hluhkcmymozqvbkmdquqphqj();
	double bccdfxrmjb(bool zwjerw);
	int mtkclpwmiloysegnqoltdwclv(bool qheyeefsmsequb, int gwypfvpr, double vfomvbyhqfw, int nobdhn, int ccevhbhxko, double pnuuirt, bool cyfrjked);
	int zspjyxchwabhhtpoojetkdn(int azbtgayx, string uazrxgkahmm, int pezordim);
	int lmpljuuleiplkkcrskkodi(string ibttbjayfv, double euadgmxskvwp, bool rudbankabimxu, int pzjimtunubpzz, double ntblbtomx, string sxcwcgtnnqohayn, string qnyxnkmzbhwqwrp, double benavwweiw, double kavyqnchc, int yhqsgsrjhmev);

private:
	double zlpchwpkgwsuhd;
	int ihptvmhjtukk;
	int afpcsisqcgdgz;
	int mvaqgfag;

	void mkhapmrygeodfjytqymg(string arhywbyixbqe, string lnxxhnnngifsyyk);
	string hsyoclesctcccdbaz(int fzxmfpmrpbrmems, double vlbxg, int rruegxzblchefro, int cvflr);
	double khuyclllrqubwbfmqwppm(int kdafvdfwpav, double zxftijiwd, string nrxfyemrzq, double avpsbhmiy, double qloffsmwuduksyl);
	double kcwhnlzrmzhhndzauirk(bool zerljwmvsgmddxh, string mxmipsfxwfqk, double fdoukz, int dmerqjxpsrxboq, double kltlgqaxidgi, string chdbpdgdix, bool qalhdvqfdftvy, string nltkcmyvnh);
	int bavdakrnpnowlobvbsdt(string lnfnvpkslgj, string oprmeaasncai, bool qrkzfzowiafdoso, double tjvbd, string iufeu, string puicrngwqzc, int euzzzjpc, string kegbz, double cdijzljhhobjp);
	void xcdhtozcjmns(string puiet, bool rgnbxpb, double tksezpca);
	string rkgvdwpvvdcfuum(string tfaluahxws, string xkqnfhvtjxr, string nxicmwkqakvpr);

};


void ghlhwvp::mkhapmrygeodfjytqymg(string arhywbyixbqe, string lnxxhnnngifsyyk) {
	double tlrabf = 52329;
	bool klwlhhm = true;
	string pfcjqyoneqamtpi = "skiguqxolpovucbatcldrmtfiqlluveynhacskbdnxrzwqoznduycsyvhhiozse";
	string vcynekujz = "dvsyvexgolvbkjfbgolbduubwruhiavzccdlnorpsfruhvfbplyhbeqznkyueblbjzfwhatsoqr";
	int vtyiguzvah = 4893;
	int okoydjpw = 1074;
	if (1074 == 1074) {
		int pwbfzo;
		for (pwbfzo = 17; pwbfzo > 0; pwbfzo--) {
			continue;
		}
	}
	if (string("skiguqxolpovucbatcldrmtfiqlluveynhacskbdnxrzwqoznduycsyvhhiozse") == string("skiguqxolpovucbatcldrmtfiqlluveynhacskbdnxrzwqoznduycsyvhhiozse")) {
		int jdvbaqor;
		for (jdvbaqor = 11; jdvbaqor > 0; jdvbaqor--) {
			continue;
		}
	}
	if (string("skiguqxolpovucbatcldrmtfiqlluveynhacskbdnxrzwqoznduycsyvhhiozse") != string("skiguqxolpovucbatcldrmtfiqlluveynhacskbdnxrzwqoznduycsyvhhiozse")) {
		int ddofilgq;
		for (ddofilgq = 3; ddofilgq > 0; ddofilgq--) {
			continue;
		}
	}
	if (string("skiguqxolpovucbatcldrmtfiqlluveynhacskbdnxrzwqoznduycsyvhhiozse") == string("skiguqxolpovucbatcldrmtfiqlluveynhacskbdnxrzwqoznduycsyvhhiozse")) {
		int hjgevgaxt;
		for (hjgevgaxt = 76; hjgevgaxt > 0; hjgevgaxt--) {
			continue;
		}
	}

}

string ghlhwvp::hsyoclesctcccdbaz(int fzxmfpmrpbrmems, double vlbxg, int rruegxzblchefro, int cvflr) {
	double smwog = 10;
	int iugbbsvlxig = 4519;
	double ruwfszfjhnpw = 30733;
	double uarqv = 31591;
	double essfvlrtvjai = 93027;
	double ungsfqmckxxhm = 5682;
	string wbgzvgqhc = "uyttrzawstgjssakqgkkenqeeshspxkwpxbzlmowndhkxdxjvovoicnlzytmzutupjnfmxzpcyclxzoktqwnimdusdbbbpzxpvjz";
	string rhkyyceu = "qfzkcefvfooiphqtustholzvutbfmjopqnjpjfqoouevyynbfshmysowvnayvftyairruwmfv";
	double gygjspirujxbhpa = 45967;
	string wxasvyfoiwf = "pl";
	if (string("qfzkcefvfooiphqtustholzvutbfmjopqnjpjfqoouevyynbfshmysowvnayvftyairruwmfv") != string("qfzkcefvfooiphqtustholzvutbfmjopqnjpjfqoouevyynbfshmysowvnayvftyairruwmfv")) {
		int qeiolpuxoi;
		for (qeiolpuxoi = 58; qeiolpuxoi > 0; qeiolpuxoi--) {
			continue;
		}
	}
	if (30733 != 30733) {
		int tuh;
		for (tuh = 62; tuh > 0; tuh--) {
			continue;
		}
	}
	if (4519 == 4519) {
		int bqo;
		for (bqo = 5; bqo > 0; bqo--) {
			continue;
		}
	}
	if (93027 != 93027) {
		int kpsgolqrz;
		for (kpsgolqrz = 46; kpsgolqrz > 0; kpsgolqrz--) {
			continue;
		}
	}
	return string("oyww");
}

double ghlhwvp::khuyclllrqubwbfmqwppm(int kdafvdfwpav, double zxftijiwd, string nrxfyemrzq, double avpsbhmiy, double qloffsmwuduksyl) {
	string vjsweszfzwmowrw = "iexkjcounozrblzfzkqhoetpyaxlgoipmyoqcuqwzemba";
	bool hdgudnpxggqam = true;
	double hqsapbfbzrdk = 48252;
	string zbbkanuqtohvkhr = "xcwafrdegljivrrjugojgxbsdirifbxdomgwmmchxxmqrwyjhtxerpszdgelulceguxuyoofoteckisnijgj";
	bool oplrid = true;
	double aaubbso = 822;
	string lphavdgevdfx = "ieaktwlhurxsfxyrfdunycasbbguumdkqrqtmzkleanelzuviuybjebcuhtnedwdfqfkhwf";
	if (string("ieaktwlhurxsfxyrfdunycasbbguumdkqrqtmzkleanelzuviuybjebcuhtnedwdfqfkhwf") == string("ieaktwlhurxsfxyrfdunycasbbguumdkqrqtmzkleanelzuviuybjebcuhtnedwdfqfkhwf")) {
		int rb;
		for (rb = 68; rb > 0; rb--) {
			continue;
		}
	}
	if (string("xcwafrdegljivrrjugojgxbsdirifbxdomgwmmchxxmqrwyjhtxerpszdgelulceguxuyoofoteckisnijgj") != string("xcwafrdegljivrrjugojgxbsdirifbxdomgwmmchxxmqrwyjhtxerpszdgelulceguxuyoofoteckisnijgj")) {
		int fgtfdimiu;
		for (fgtfdimiu = 8; fgtfdimiu > 0; fgtfdimiu--) {
			continue;
		}
	}
	if (822 != 822) {
		int olea;
		for (olea = 64; olea > 0; olea--) {
			continue;
		}
	}
	if (string("iexkjcounozrblzfzkqhoetpyaxlgoipmyoqcuqwzemba") == string("iexkjcounozrblzfzkqhoetpyaxlgoipmyoqcuqwzemba")) {
		int znoz;
		for (znoz = 69; znoz > 0; znoz--) {
			continue;
		}
	}
	return 22769;
}

double ghlhwvp::kcwhnlzrmzhhndzauirk(bool zerljwmvsgmddxh, string mxmipsfxwfqk, double fdoukz, int dmerqjxpsrxboq, double kltlgqaxidgi, string chdbpdgdix, bool qalhdvqfdftvy, string nltkcmyvnh) {
	double orxqsncbwpzun = 60172;
	string ulvusefpskskq = "ahfddfthcgvvmyjoyjkhrurmhnrykoaciaizsukxzmogwlqqopevcrinrxyslbihpts";
	bool qqadqyftezwoge = true;
	if (true != true) {
		int ttdahzttho;
		for (ttdahzttho = 64; ttdahzttho > 0; ttdahzttho--) {
			continue;
		}
	}
	if (string("ahfddfthcgvvmyjoyjkhrurmhnrykoaciaizsukxzmogwlqqopevcrinrxyslbihpts") != string("ahfddfthcgvvmyjoyjkhrurmhnrykoaciaizsukxzmogwlqqopevcrinrxyslbihpts")) {
		int crv;
		for (crv = 80; crv > 0; crv--) {
			continue;
		}
	}
	if (true != true) {
		int nigudcft;
		for (nigudcft = 32; nigudcft > 0; nigudcft--) {
			continue;
		}
	}
	if (60172 != 60172) {
		int dhfhdfaaom;
		for (dhfhdfaaom = 7; dhfhdfaaom > 0; dhfhdfaaom--) {
			continue;
		}
	}
	return 6810;
}

int ghlhwvp::bavdakrnpnowlobvbsdt(string lnfnvpkslgj, string oprmeaasncai, bool qrkzfzowiafdoso, double tjvbd, string iufeu, string puicrngwqzc, int euzzzjpc, string kegbz, double cdijzljhhobjp) {
	return 15344;
}

void ghlhwvp::xcdhtozcjmns(string puiet, bool rgnbxpb, double tksezpca) {
	double ehotgvhtxppc = 41430;
	int sgkjgxprasl = 1741;
	double ztzgqycjn = 29721;
	double wjndnekog = 34269;
	double ejtjtjcqqgytnxl = 11567;
	double pxljlkykkomklw = 17400;
	if (1741 == 1741) {
		int eiowzs;
		for (eiowzs = 56; eiowzs > 0; eiowzs--) {
			continue;
		}
	}

}

string ghlhwvp::rkgvdwpvvdcfuum(string tfaluahxws, string xkqnfhvtjxr, string nxicmwkqakvpr) {
	bool rtmpyilerjhcck = true;
	int crhey = 9594;
	bool euwxqwcurdj = true;
	int bmzgr = 612;
	string rocgdivitfbfz = "y";
	bool mgzyebjxhz = false;
	int dvwfdbs = 6731;
	int yfpqjtxrsqkjq = 892;
	if (true == true) {
		int haeiro;
		for (haeiro = 44; haeiro > 0; haeiro--) {
			continue;
		}
	}
	return string("fopclehhn");
}

bool ghlhwvp::hluhkcmymozqvbkmdquqphqj() {
	return false;
}

double ghlhwvp::bccdfxrmjb(bool zwjerw) {
	double vaotzbvfbdex = 84702;
	string mufgr = "nzseysfbusdwvqpzodwjoxiylmucfkoajazfahnubzhuamdfkak";
	string naqfnphabgntvc = "mjwttlecrmgixrptmpxovdiqtlpqfpastpeeyziogrpqngsukjkvrlhuj";
	return 13737;
}

int ghlhwvp::mtkclpwmiloysegnqoltdwclv(bool qheyeefsmsequb, int gwypfvpr, double vfomvbyhqfw, int nobdhn, int ccevhbhxko, double pnuuirt, bool cyfrjked) {
	double bgzlutnc = 48382;
	bool lkelockcb = false;
	bool leasxtvl = true;
	string qyqmwzisu = "bvqakddkbtqxxsmayupdtgfkygsqqcmsgpiqoskkdobayprxwofwrgieeto";
	string qaclprg = "bmfzswmdmgkhxnjtervnhahocokxfpccmgzrwrdjgybvdmfaoevmveoimcxrqosqswrenbwowvjldywwouyeydh";
	double qpfcmggyoyen = 2122;
	string gomfhoisixbxyfb = "atiscoksknpzjaxcrtmctbeqdlbtvzlkpvidyxaaqo";
	int ooyrk = 100;
	return 45422;
}

int ghlhwvp::zspjyxchwabhhtpoojetkdn(int azbtgayx, string uazrxgkahmm, int pezordim) {
	string ddthcbwvnmwtx = "jjkqwwcwtiooozbslbcdjbvbpeysfclnnctfv";
	bool cixbdptxi = false;
	if (false == false) {
		int dbanp;
		for (dbanp = 26; dbanp > 0; dbanp--) {
			continue;
		}
	}
	if (string("jjkqwwcwtiooozbslbcdjbvbpeysfclnnctfv") == string("jjkqwwcwtiooozbslbcdjbvbpeysfclnnctfv")) {
		int bjxry;
		for (bjxry = 19; bjxry > 0; bjxry--) {
			continue;
		}
	}
	if (false == false) {
		int dkjcxpzgoo;
		for (dkjcxpzgoo = 50; dkjcxpzgoo > 0; dkjcxpzgoo--) {
			continue;
		}
	}
	if (false != false) {
		int ec;
		for (ec = 63; ec > 0; ec--) {
			continue;
		}
	}
	if (string("jjkqwwcwtiooozbslbcdjbvbpeysfclnnctfv") != string("jjkqwwcwtiooozbslbcdjbvbpeysfclnnctfv")) {
		int jylkwvylpm;
		for (jylkwvylpm = 10; jylkwvylpm > 0; jylkwvylpm--) {
			continue;
		}
	}
	return 19210;
}

int ghlhwvp::lmpljuuleiplkkcrskkodi(string ibttbjayfv, double euadgmxskvwp, bool rudbankabimxu, int pzjimtunubpzz, double ntblbtomx, string sxcwcgtnnqohayn, string qnyxnkmzbhwqwrp, double benavwweiw, double kavyqnchc, int yhqsgsrjhmev) {
	return 68548;
}

double ghlhwvp::rurkmvjdkcpxjpgyhzcz(double crqtnbfpzpmssp, bool baxpwksylhfmr, bool mrmownyccp, bool rlrlvpa, string cbbcomcg, string iasokdyawemdex, bool hjqardzehyam, double uoixaxfb, double dcmehqgkyndavv, int ybknglbhu) {
	double pxvkkywddpnrb = 13348;
	string gmkmagcgdd = "rnwsltvvshatehhswrmmdbqmyemlfncxubznpaxibsowtmkhoofltxghhfrbcbfpe";
	bool dgvwc = false;
	double rshlo = 28716;
	double fbtovnmaewt = 26512;
	int euvcrbdsxmtbur = 8426;
	string ykfrvfzftioitdt = "khwra";
	int aojhfrylzuq = 1688;
	if (13348 == 13348) {
		int qyihoqp;
		for (qyihoqp = 86; qyihoqp > 0; qyihoqp--) {
			continue;
		}
	}
	if (string("rnwsltvvshatehhswrmmdbqmyemlfncxubznpaxibsowtmkhoofltxghhfrbcbfpe") == string("rnwsltvvshatehhswrmmdbqmyemlfncxubznpaxibsowtmkhoofltxghhfrbcbfpe")) {
		int zc;
		for (zc = 73; zc > 0; zc--) {
			continue;
		}
	}
	return 90600;
}

string ghlhwvp::xrnvvzfmagnuuit(string ghyrc, int rmheb, double pswiejr, int hzctozgpufolfjr) {
	string wmwizwr = "tsfwveelpdoobopezjvwto";
	int kurnistbbziaz = 3310;
	double ibiolxfme = 28796;
	string ykcslvjnpknye = "cecbdxvob";
	double jfndwwdpcnumz = 52817;
	double ntkjvuldwwx = 9509;
	bool hgsektjfwelgu = true;
	if (28796 != 28796) {
		int pxngnctlu;
		for (pxngnctlu = 79; pxngnctlu > 0; pxngnctlu--) {
			continue;
		}
	}
	if (28796 == 28796) {
		int gx;
		for (gx = 7; gx > 0; gx--) {
			continue;
		}
	}
	if (3310 == 3310) {
		int bvoqdof;
		for (bvoqdof = 74; bvoqdof > 0; bvoqdof--) {
			continue;
		}
	}
	if (string("cecbdxvob") != string("cecbdxvob")) {
		int aswqzfsnn;
		for (aswqzfsnn = 72; aswqzfsnn > 0; aswqzfsnn--) {
			continue;
		}
	}
	return string("j");
}

string ghlhwvp::wcixolwyotdrcae(string wbdxptkrxowmt) {
	double hurcgpjfceuvuax = 43869;
	bool kbkmfhivhuzpkr = false;
	double cwjnii = 4287;
	int xavhhxjohogcmd = 2542;
	return string("kfyjziitmmhldwj");
}

void ghlhwvp::vuowawcmij(int iudezpmpg, string bkbskhbbfhsjeoc, int qbucxdqfpjhmov, double sdcakkudbrpjtp, double hlcop, double durzijgohmtpzf, int yeldj) {
	bool gzgvhpcws = true;
	double orgvvay = 33317;
	int puudhjyulrgx = 1565;
	double lvtafvnknwukhtc = 42300;
	string ykwgrryqgie = "scsivwecabtgneheiignxoieuuovzwbgvkzspcaambaifr";
	bool dxhsbwzzi = false;
	int lugxrrddyab = 6341;
	string btosijlrc = "hbxblqjcbfiwluojeokwrndsiwqrpcslaknsu";
	bool fafwrcxxnxnkngi = false;
	if (string("scsivwecabtgneheiignxoieuuovzwbgvkzspcaambaifr") != string("scsivwecabtgneheiignxoieuuovzwbgvkzspcaambaifr")) {
		int xzjdaw;
		for (xzjdaw = 50; xzjdaw > 0; xzjdaw--) {
			continue;
		}
	}

}

bool ghlhwvp::jpvwhinwjyohb() {
	double euodwe = 48148;
	int capvtrbpmvopcf = 768;
	bool igldjaszipklny = false;
	string vjajwbxufz = "xggyhcjjotsmtvxvujjctpeaen";
	int xajrcuwhkze = 884;
	bool xlwfwr = false;
	if (768 == 768) {
		int ttagrxrt;
		for (ttagrxrt = 8; ttagrxrt > 0; ttagrxrt--) {
			continue;
		}
	}
	if (false == false) {
		int qrnlllb;
		for (qrnlllb = 3; qrnlllb > 0; qrnlllb--) {
			continue;
		}
	}
	if (768 == 768) {
		int hxlrlklhb;
		for (hxlrlklhb = 40; hxlrlklhb > 0; hxlrlklhb--) {
			continue;
		}
	}
	if (false != false) {
		int mhxpl;
		for (mhxpl = 75; mhxpl > 0; mhxpl--) {
			continue;
		}
	}
	if (false == false) {
		int xnsldl;
		for (xnsldl = 0; xnsldl > 0; xnsldl--) {
			continue;
		}
	}
	return false;
}

string ghlhwvp::xcwowzisrwqfihiqy(bool xqmcjfe, string idystohtpj, bool gosyck, bool fdeyw) {
	int esdxsspt = 3143;
	bool nhisqnt = true;
	bool eylqxe = false;
	bool hkicwneuuybvy = false;
	bool royjtxpudlll = false;
	int jjoqp = 3619;
	int azgkesjtcqfewn = 1426;
	string fbywbdqfpdaael = "dkps";
	double zrtydzbqqdzt = 33636;
	bool usfmcnxpiib = false;
	if (string("dkps") == string("dkps")) {
		int izqpcozh;
		for (izqpcozh = 73; izqpcozh > 0; izqpcozh--) {
			continue;
		}
	}
	if (string("dkps") == string("dkps")) {
		int abkegzj;
		for (abkegzj = 75; abkegzj > 0; abkegzj--) {
			continue;
		}
	}
	return string("carhrmka");
}

string ghlhwvp::qldcqsfcrfcuynxqcyiva(int waiailhq, double zvjlwfykt, double reoeohiy, bool hneqxhcozq, int cqitec, bool mtvhgsrma) {
	double idxdzwkb = 4764;
	bool uoezduyrbdo = true;
	string mfkdftvlcmsf = "mrpkkwmbydyisgbpyseau";
	int rkscefwgrijvd = 2248;
	string cxnmxtwoipdkxnc = "xvfctofmvhrzinbdioszfpvxzqeluzikmvzmfscubwvxrppljiamtchzdesghqkgjaxvj";
	if (true != true) {
		int aatn;
		for (aatn = 91; aatn > 0; aatn--) {
			continue;
		}
	}
	if (string("xvfctofmvhrzinbdioszfpvxzqeluzikmvzmfscubwvxrppljiamtchzdesghqkgjaxvj") != string("xvfctofmvhrzinbdioszfpvxzqeluzikmvzmfscubwvxrppljiamtchzdesghqkgjaxvj")) {
		int rudmxklnm;
		for (rudmxklnm = 97; rudmxklnm > 0; rudmxklnm--) {
			continue;
		}
	}
	if (true == true) {
		int nzodlq;
		for (nzodlq = 38; nzodlq > 0; nzodlq--) {
			continue;
		}
	}
	if (string("xvfctofmvhrzinbdioszfpvxzqeluzikmvzmfscubwvxrppljiamtchzdesghqkgjaxvj") != string("xvfctofmvhrzinbdioszfpvxzqeluzikmvzmfscubwvxrppljiamtchzdesghqkgjaxvj")) {
		int qd;
		for (qd = 52; qd > 0; qd--) {
			continue;
		}
	}
	return string("jm");
}

int ghlhwvp::pjjgzksqxmbvgazk(bool afawlwesksdd, double rbdbceietrocgy, string cckvcub) {
	string urrfpagymmwr = "mfkmxscakuqnbcwxgszsnirauefemteoqqybaibegvqxcpuigobthoo";
	string nsxqm = "hpduqyhciirctwyimieuybvqsyangdanhtouinsztbyhduoflyplsqltualqkceytqrhhloxljgmzthfltgawfacgykhfvdag";
	bool jqhesmrolygi = true;
	string auphdjtshnfpa = "hbqbfyutbpletkxytewtcyxavkldyqvehiagmbqqfgnolfmfnnsrfgpkvche";
	int ldjybchwi = 5185;
	if (string("mfkmxscakuqnbcwxgszsnirauefemteoqqybaibegvqxcpuigobthoo") != string("mfkmxscakuqnbcwxgszsnirauefemteoqqybaibegvqxcpuigobthoo")) {
		int lne;
		for (lne = 20; lne > 0; lne--) {
			continue;
		}
	}
	if (true != true) {
		int vfdnfglcgl;
		for (vfdnfglcgl = 27; vfdnfglcgl > 0; vfdnfglcgl--) {
			continue;
		}
	}
	if (string("hpduqyhciirctwyimieuybvqsyangdanhtouinsztbyhduoflyplsqltualqkceytqrhhloxljgmzthfltgawfacgykhfvdag") != string("hpduqyhciirctwyimieuybvqsyangdanhtouinsztbyhduoflyplsqltualqkceytqrhhloxljgmzthfltgawfacgykhfvdag")) {
		int jozg;
		for (jozg = 14; jozg > 0; jozg--) {
			continue;
		}
	}
	if (string("hpduqyhciirctwyimieuybvqsyangdanhtouinsztbyhduoflyplsqltualqkceytqrhhloxljgmzthfltgawfacgykhfvdag") == string("hpduqyhciirctwyimieuybvqsyangdanhtouinsztbyhduoflyplsqltualqkceytqrhhloxljgmzthfltgawfacgykhfvdag")) {
		int leysznf;
		for (leysznf = 65; leysznf > 0; leysznf--) {
			continue;
		}
	}
	if (string("hpduqyhciirctwyimieuybvqsyangdanhtouinsztbyhduoflyplsqltualqkceytqrhhloxljgmzthfltgawfacgykhfvdag") == string("hpduqyhciirctwyimieuybvqsyangdanhtouinsztbyhduoflyplsqltualqkceytqrhhloxljgmzthfltgawfacgykhfvdag")) {
		int koiiiwqidj;
		for (koiiiwqidj = 1; koiiiwqidj > 0; koiiiwqidj--) {
			continue;
		}
	}
	return 15110;
}

string ghlhwvp::sbdhikgwdapywcobr(double toryji, int lfvqmnobvpein, bool hoevmxejpiftxsh, int nlizgwwcvskk, int faetg, bool uwmcikjilwolm, double pfigkipu, int owflirbkteahso) {
	string hikkdyp = "eypqlwhp";
	string xjwtwcboqpuufa = "xgvqeolyjvetpbuxqndxashkrdqjpuvdadfepqjjblxmihfojgcsirudiustkesppt";
	double yakeb = 29356;
	return string("gamklhjxqqoektvmfhhj");
}

ghlhwvp::ghlhwvp() {
	this->rurkmvjdkcpxjpgyhzcz(15337, true, false, true, string("rqcrzjhpqi"), string("xenfrocuxixdsnnkgviilavclkxomyqnabtddxdylwnibqztivhnsxwybinejie"), true, 37158, 23583, 1822);
	this->xrnvvzfmagnuuit(string("skwmblvjvaipmtqsktewmdifyiobfjbsabyux"), 654, 7616, 6087);
	this->wcixolwyotdrcae(string("zkndrgdbeglutvvllvyreesnwxepvvvkwvjjtrpbjsxgtdwygsammampdpzwyumw"));
	this->vuowawcmij(3485, string("uslfqaxarrmjmvglcuukbqsx"), 3479, 39967, 814, 69851, 2428);
	this->jpvwhinwjyohb();
	this->xcwowzisrwqfihiqy(true, string("tgnmxmqwnxzmupddwarfabgyswzsjntjbgycmsybeddtaijqhttrzfuknhqntgbskreokcies"), true, false);
	this->qldcqsfcrfcuynxqcyiva(2131, 15280, 3925, true, 3064, true);
	this->pjjgzksqxmbvgazk(false, 3615, string("uqybsxlutuqnjwwxfptotziikvfoyuvpndelxgkizsfoqmlrgdsxyjrfi"));
	this->sbdhikgwdapywcobr(17272, 1583, false, 691, 945, false, 71213, 1480);
	this->hluhkcmymozqvbkmdquqphqj();
	this->bccdfxrmjb(true);
	this->mtkclpwmiloysegnqoltdwclv(false, 3707, 39901, 1083, 275, 24697, false);
	this->zspjyxchwabhhtpoojetkdn(115, string("xtjnxooudvwiwskqdggxjklayzbgeokoecejxivrmabjhncdhnwpywvcwiioclvjqdcsygb"), 3829);
	this->lmpljuuleiplkkcrskkodi(string("bowxrojxuliyijiq"), 6944, true, 7798, 31485, string("qntzzvewlbhkzsouehjxgpcqeecizvv"), string("wshntjmsaxbckukbvrrifrwckbxhxyzptaokldirkazficfovuzgdqfmfyismkgeupqojvwoohyybpyppuholifvlhrvsfqpfog"), 20689, 32618, 2785);
	this->mkhapmrygeodfjytqymg(string("eiikzhgumwxuckopvxyjfmawmkysyqdfqmgjrthelyhbt"), string("sayxxbralhocviliaxw"));
	this->hsyoclesctcccdbaz(5188, 70040, 135, 220);
	this->khuyclllrqubwbfmqwppm(1824, 21551, string("rrfpxvhocuozrrsrtltbsixdgnemadwfdiekt"), 24758, 6392);
	this->kcwhnlzrmzhhndzauirk(true, string("ujmlsrunhotuoxyhbpqgspcesfscdvquyfkxtzfabapwvmzowkjkanb"), 76671, 502, 6073, string("vjppnc"), true, string("bkvvytzkscnyadbcdsordmdqjkazpvnorotweyvpyjvgfxvyirqdxuczfswlgbfqg"));
	this->bavdakrnpnowlobvbsdt(string("mnzrmaiubgvovsqntmiagsrudzfduxssjkgxpsmceibkkgal"), string("fcneivwownxbnomv"), true, 61697, string("udoqbtaxucpwuonjjxrznmcsxcgdoebqtrnoreggpxbcbhpedkulq"), string("weytjcczstlnynfrljtbnqjmkajyxyxkaguztkrshcdmydusbmkmltvghfmluyzvjkep"), 748, string("fxpxpirhcognfegtbobwjazigwmczbwcgcldzgfbyvldswmgrmghpqbvseqiufkjvvjpfdytmvvyydqzoynfmsoaq"), 6354);
	this->xcdhtozcjmns(string("pyfpkxsjaheguqqgslzfumijtncmwm"), false, 34789);
	this->rkgvdwpvvdcfuum(string("lscxtcnhnyddfuvgiiphdoykwczpmtvesckzyvnqqqrqommpjidxajnksnwacpnthozvkshygmeevqdwtqxuzjtvaey"), string("vyvnscieuo"), string("imqhomylrbeuryjevvwimfppzhadijkbeklxwqaqrlfivwhllwtd"));
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class yvfqlsn {
public:
	int wyfxzuuazxby;
	double kdzeynlu;
	int hqwoievn;
	yvfqlsn();
	int ccebnyxxiskvrvrt(string kxynbvpcirgc, int leymivpwpleeyk, string rszpggsuauu, double xmgjjjfz, double nfkhjeey);
	bool juhiqskcarfzfhhpdclb(bool fgmdelm, string mfknehfnukus, bool owlkmq, string cycmdtjwzhvyfy, int qhquful, bool obabgpgqxcg, int sgrheonwywcr, string nkkyz, int joriau, int ocplh);
	void uvffjoyhohzzdgilc(bool qbnkbz);

protected:
	int bvmeuipyshqk;
	double knrxcgs;
	bool alcbdjjgum;
	int csiqkezplfi;

	void mofsjmblutmamzaufvnuib(string dlbnhdpp, bool nuzyqiyrtxf, double azschxevngjal);
	string qnupndnwlaczcm(bool ajxfhoyobnvgl, bool ikgslkhtgy, bool ixsxjv, string ixpywolvi, double acyfcwfigjbt, bool ehuemubssc, int cmudraqia, bool ktiyukzjpzcxalq, string ghnepdznoqnkk);
	bool msitodlfszzfjoey(int huwnel, int tyasorotbx, string jtjnlujzzo, double xbhwfarfkbgab);
	bool qxtdccuivedfxsgrpzd(double cfsmremloil, int osqgnzkkewy);
	double scmurkanzvxclwvfx(int yptajvqmlzkgdru, int etgcpouww, bool pcjmceqrr, int cozqozo, string nxbsmxlesymzxfp, bool vhmkjrnnveyzdrk, int psmdz);
	double avnkvaetvk(bool klykfxsddsw, bool tlzxronkk);
	double uywgikzsgqwmwxcf(double ldbfndhcjlhxqv, double onhhdo, double rykjlcuqqzpgbmf, string gtwkbfbvhpw);
	string chrxrxpeoifytyybrwifig(string wymgnjvjgk, int xkdvnznyiyl, string dvykqrzslh, int sihpvbzx);
	bool bipwemoavzvjddpfdgtzswi(int dqhfevgvgyyr, string lwzwwbu, string ksvmflrowtodpn);

private:
	string efoztlofbslqd;
	bool jymsxfndhp;
	bool aryswvvio;
	bool nvjyqdnfvllrq;

	void gacpgtfvqluiuwhlyqa(int eoshx, int hjclrestpqgmb, double ztskcbtjh);

};


void yvfqlsn::gacpgtfvqluiuwhlyqa(int eoshx, int hjclrestpqgmb, double ztskcbtjh) {

}

void yvfqlsn::mofsjmblutmamzaufvnuib(string dlbnhdpp, bool nuzyqiyrtxf, double azschxevngjal) {
	int mkmdwel = 192;
	bool tzvettx = false;
	string xjxlkwxkh = "mzgzxyfgnbkbzeujddhinfaygsbarxatm";
	if (false == false) {
		int xxwjpqxey;
		for (xxwjpqxey = 41; xxwjpqxey > 0; xxwjpqxey--) {
			continue;
		}
	}
	if (string("mzgzxyfgnbkbzeujddhinfaygsbarxatm") != string("mzgzxyfgnbkbzeujddhinfaygsbarxatm")) {
		int nffqyeg;
		for (nffqyeg = 33; nffqyeg > 0; nffqyeg--) {
			continue;
		}
	}

}

string yvfqlsn::qnupndnwlaczcm(bool ajxfhoyobnvgl, bool ikgslkhtgy, bool ixsxjv, string ixpywolvi, double acyfcwfigjbt, bool ehuemubssc, int cmudraqia, bool ktiyukzjpzcxalq, string ghnepdznoqnkk) {
	bool jxqkrvrejrbvavr = true;
	string wgbatduwd = "qlgfaoabkambuwwdwdipkomrihnjnbqw";
	string yjtiviuaxem = "qtcoajayrhffegnjxkdgshwpkgidyugdwieligqrkxoekurkbftvjjfmwzlbaqaxfmqljfytabjlo";
	string ddqikiyikveq = "dhnarzjcxptmtljfaoveynswgsfpzhkrzaxovirzxrbtgilpr";
	if (string("qlgfaoabkambuwwdwdipkomrihnjnbqw") == string("qlgfaoabkambuwwdwdipkomrihnjnbqw")) {
		int vsf;
		for (vsf = 44; vsf > 0; vsf--) {
			continue;
		}
	}
	if (string("qtcoajayrhffegnjxkdgshwpkgidyugdwieligqrkxoekurkbftvjjfmwzlbaqaxfmqljfytabjlo") == string("qtcoajayrhffegnjxkdgshwpkgidyugdwieligqrkxoekurkbftvjjfmwzlbaqaxfmqljfytabjlo")) {
		int tgqfw;
		for (tgqfw = 98; tgqfw > 0; tgqfw--) {
			continue;
		}
	}
	if (string("qtcoajayrhffegnjxkdgshwpkgidyugdwieligqrkxoekurkbftvjjfmwzlbaqaxfmqljfytabjlo") != string("qtcoajayrhffegnjxkdgshwpkgidyugdwieligqrkxoekurkbftvjjfmwzlbaqaxfmqljfytabjlo")) {
		int hjytwyd;
		for (hjytwyd = 72; hjytwyd > 0; hjytwyd--) {
			continue;
		}
	}
	if (string("dhnarzjcxptmtljfaoveynswgsfpzhkrzaxovirzxrbtgilpr") != string("dhnarzjcxptmtljfaoveynswgsfpzhkrzaxovirzxrbtgilpr")) {
		int nqi;
		for (nqi = 14; nqi > 0; nqi--) {
			continue;
		}
	}
	return string("jrxzeqsax");
}

bool yvfqlsn::msitodlfszzfjoey(int huwnel, int tyasorotbx, string jtjnlujzzo, double xbhwfarfkbgab) {
	return false;
}

bool yvfqlsn::qxtdccuivedfxsgrpzd(double cfsmremloil, int osqgnzkkewy) {
	bool twdhtfh = false;
	int lmsnfeehcay = 1283;
	double wkkqkngw = 2411;
	int kbmgqwzcigat = 7091;
	int ddktmsuzfyrk = 3588;
	bool rlkdt = false;
	string tpgfb = "thuewocouolvrqzsxapuetncobfrczrfcpsotrsavgr";
	int dmzmwphbh = 2616;
	int sczkmfpeupisoxo = 1766;
	int agfbwwy = 2903;
	if (1283 != 1283) {
		int xnbjdzuptb;
		for (xnbjdzuptb = 79; xnbjdzuptb > 0; xnbjdzuptb--) {
			continue;
		}
	}
	if (2903 != 2903) {
		int yof;
		for (yof = 27; yof > 0; yof--) {
			continue;
		}
	}
	if (string("thuewocouolvrqzsxapuetncobfrczrfcpsotrsavgr") != string("thuewocouolvrqzsxapuetncobfrczrfcpsotrsavgr")) {
		int yxzd;
		for (yxzd = 50; yxzd > 0; yxzd--) {
			continue;
		}
	}
	return false;
}

double yvfqlsn::scmurkanzvxclwvfx(int yptajvqmlzkgdru, int etgcpouww, bool pcjmceqrr, int cozqozo, string nxbsmxlesymzxfp, bool vhmkjrnnveyzdrk, int psmdz) {
	int vcaarfsxx = 807;
	bool urzvzpkjpwytims = false;
	int tqfxmkbznyn = 6208;
	int szdymnasj = 6102;
	bool pehygikhrtid = true;
	double dmgqrbux = 16357;
	if (false == false) {
		int rywscwf;
		for (rywscwf = 47; rywscwf > 0; rywscwf--) {
			continue;
		}
	}
	if (false != false) {
		int dngpiaylb;
		for (dngpiaylb = 56; dngpiaylb > 0; dngpiaylb--) {
			continue;
		}
	}
	if (false == false) {
		int omthk;
		for (omthk = 90; omthk > 0; omthk--) {
			continue;
		}
	}
	if (6208 != 6208) {
		int oebnhanddh;
		for (oebnhanddh = 88; oebnhanddh > 0; oebnhanddh--) {
			continue;
		}
	}
	return 87404;
}

double yvfqlsn::avnkvaetvk(bool klykfxsddsw, bool tlzxronkk) {
	bool vnspqwo = true;
	string mfqjbytujafpcyw = "qnveeeczxugubvcvpudylmboajfoo";
	string ofveayvcb = "cmmt";
	string lfssuwxknmwwa = "yzkobxzlrwocaaxejkptkfftzeedfbgdeavdmtui";
	int pqaejykanvd = 4744;
	bool wzkardpnzmr = true;
	bool wzmttmim = true;
	if (true != true) {
		int snksazydy;
		for (snksazydy = 97; snksazydy > 0; snksazydy--) {
			continue;
		}
	}
	if (string("cmmt") != string("cmmt")) {
		int gywov;
		for (gywov = 85; gywov > 0; gywov--) {
			continue;
		}
	}
	return 81756;
}

double yvfqlsn::uywgikzsgqwmwxcf(double ldbfndhcjlhxqv, double onhhdo, double rykjlcuqqzpgbmf, string gtwkbfbvhpw) {
	double koavmphnajc = 5002;
	string ydgof = "amgnmdsvxaotvigopqiekjzhnxihcpglgsesdrvuocjusgpukxykfddorzcuvprqkj";
	if (5002 != 5002) {
		int jutxyok;
		for (jutxyok = 18; jutxyok > 0; jutxyok--) {
			continue;
		}
	}
	if (string("amgnmdsvxaotvigopqiekjzhnxihcpglgsesdrvuocjusgpukxykfddorzcuvprqkj") == string("amgnmdsvxaotvigopqiekjzhnxihcpglgsesdrvuocjusgpukxykfddorzcuvprqkj")) {
		int kdfsbxbc;
		for (kdfsbxbc = 16; kdfsbxbc > 0; kdfsbxbc--) {
			continue;
		}
	}
	return 45386;
}

string yvfqlsn::chrxrxpeoifytyybrwifig(string wymgnjvjgk, int xkdvnznyiyl, string dvykqrzslh, int sihpvbzx) {
	double dqkoodqjxymkznb = 39972;
	bool fuytznj = true;
	if (true == true) {
		int lzbhlkhi;
		for (lzbhlkhi = 36; lzbhlkhi > 0; lzbhlkhi--) {
			continue;
		}
	}
	if (39972 == 39972) {
		int tp;
		for (tp = 76; tp > 0; tp--) {
			continue;
		}
	}
	if (true == true) {
		int qfwgihjy;
		for (qfwgihjy = 54; qfwgihjy > 0; qfwgihjy--) {
			continue;
		}
	}
	return string("sgiceonne");
}

bool yvfqlsn::bipwemoavzvjddpfdgtzswi(int dqhfevgvgyyr, string lwzwwbu, string ksvmflrowtodpn) {
	string gqmylc = "zyhqhshdiyuwnbdjjkapjufccjopzzxomqlbrmnumeukemqlctntcrcentflsvifbxtfcfwt";
	int hdkszhvhy = 715;
	double anvnsyhwh = 12629;
	double edhyrehp = 16411;
	int mkazwtvefsg = 574;
	double uygcoslrzrbjwgx = 47866;
	if (12629 == 12629) {
		int aawkvjaj;
		for (aawkvjaj = 93; aawkvjaj > 0; aawkvjaj--) {
			continue;
		}
	}
	if (47866 == 47866) {
		int jtzbymf;
		for (jtzbymf = 13; jtzbymf > 0; jtzbymf--) {
			continue;
		}
	}
	if (574 == 574) {
		int sgtkpikobs;
		for (sgtkpikobs = 2; sgtkpikobs > 0; sgtkpikobs--) {
			continue;
		}
	}
	if (string("zyhqhshdiyuwnbdjjkapjufccjopzzxomqlbrmnumeukemqlctntcrcentflsvifbxtfcfwt") != string("zyhqhshdiyuwnbdjjkapjufccjopzzxomqlbrmnumeukemqlctntcrcentflsvifbxtfcfwt")) {
		int dinmf;
		for (dinmf = 85; dinmf > 0; dinmf--) {
			continue;
		}
	}
	if (12629 != 12629) {
		int iwjjkng;
		for (iwjjkng = 18; iwjjkng > 0; iwjjkng--) {
			continue;
		}
	}
	return true;
}

int yvfqlsn::ccebnyxxiskvrvrt(string kxynbvpcirgc, int leymivpwpleeyk, string rszpggsuauu, double xmgjjjfz, double nfkhjeey) {
	string fowkgq = "lnxqqwlyphmpqxwvngtpmiqkyjekqqvgcrzrpyjuqxxurffivzckfaeqmvtvobdamyomskbifzpjpu";
	bool hwjawzcrc = false;
	int hxepbayagtjolvj = 4950;
	double hxwcpaspso = 27917;
	return 67771;
}

bool yvfqlsn::juhiqskcarfzfhhpdclb(bool fgmdelm, string mfknehfnukus, bool owlkmq, string cycmdtjwzhvyfy, int qhquful, bool obabgpgqxcg, int sgrheonwywcr, string nkkyz, int joriau, int ocplh) {
	return true;
}

void yvfqlsn::uvffjoyhohzzdgilc(bool qbnkbz) {
	string cilutgsi = "jvtwcfhvlimhgcroihcnbrppfinrrsbbktswkryzbkjupacqwjhinwqntzhbqzsn";
	int fqmztkwodgi = 917;
	bool keppu = false;
	string ygrvnm = "dyxwwknjahst";
	int svymz = 578;
	double dmxhfi = 80164;
	if (80164 != 80164) {
		int llndwqdy;
		for (llndwqdy = 12; llndwqdy > 0; llndwqdy--) {
			continue;
		}
	}
	if (string("jvtwcfhvlimhgcroihcnbrppfinrrsbbktswkryzbkjupacqwjhinwqntzhbqzsn") == string("jvtwcfhvlimhgcroihcnbrppfinrrsbbktswkryzbkjupacqwjhinwqntzhbqzsn")) {
		int uoogw;
		for (uoogw = 85; uoogw > 0; uoogw--) {
			continue;
		}
	}

}

yvfqlsn::yvfqlsn() {
	this->ccebnyxxiskvrvrt(string("ynkxuinfxtbgmfjyefqaoesfmrnyjldecjytnjwagnerlqxrquqkzffhws"), 2756, string("pwygcmgvrjmjxzcaocdeevghfadqykcrtqoijuykphgynrftvfmpyacfqfoqzsx"), 20136, 17211);
	this->juhiqskcarfzfhhpdclb(true, string("dwcewkgjgenrhuinh"), false, string("dxbkquvpddqykujkoejjnahqobwqvpmshpqtibrzelswlppu"), 6273, false, 348, string("zvosgmsaudsqlnnpjybplrjlqcbnyvsnfllwxzanikivxknabvhjaamrcdcelwyylodefsoswnnsplivltdfehrn"), 3116, 4334);
	this->uvffjoyhohzzdgilc(true);
	this->mofsjmblutmamzaufvnuib(string("gmvutgdkjcptyx"), true, 35533);
	this->qnupndnwlaczcm(true, false, false, string("lrvllajssnkjyxxhsszoygnz"), 6685, false, 2245, true, string("cxcodjmbmqkqcutmjjskrwzqhivzclzfvtizayzqihtgbikketfajcolhrjdzmjlxsefhlcpdrrf"));
	this->msitodlfszzfjoey(1964, 6017, string(""), 21830);
	this->qxtdccuivedfxsgrpzd(75188, 9044);
	this->scmurkanzvxclwvfx(5112, 1364, true, 1682, string("miaiakvoqqwlevhtzafovustdtyivdllnhepiasqzxkxrbrbzrhscjdqmujg"), false, 1123);
	this->avnkvaetvk(true, true);
	this->uywgikzsgqwmwxcf(8902, 18290, 1057, string("cjziktrmajxbtnqmpmetcyyqwbhuhogwqltopbbjxibwvloxedaengrukeluengayybrrjxwzrmtazaqmaokmguxg"));
	this->chrxrxpeoifytyybrwifig(string("hxhjzhtfiyckilwajaltdzlcwlrxvnfpgxntdnfvkfjyqtupiqcevccutqzzqvjwffrlolqkchzunqwrxpktywgoreozigyek"), 1734, string("qpajwmsgfvcgekmpddrexfdqqlefphrnlvgizishyolzhkjbjupzwwkxegbtljfwpeynildtkcsjcvsjbirtxqehsiboluqwlw"), 3447);
	this->bipwemoavzvjddpfdgtzswi(3505, string("gifamhcqnmwflrakkepwtvdtkckdmqzxmduiaozawjaruwddqvhb"), string("maaiqqnmswhfldbphyzxgnkoarsurenwjddoqdkppucslbpxusyykyzjcqllhefxeuuakuzfi"));
	this->gacpgtfvqluiuwhlyqa(4511, 1442, 1264);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class omyxyws {
public:
	int qzulgb;
	int qtqenv;
	double gneeujuynvxqfyu;
	omyxyws();
	int ejbgcndazqckvrgw(string pogchhcuzaclmi, int rpjxwdwnxcmok, bool nsbcyqfpptrexze);
	double ddewrwmuwc(double agoiw, double evxsysbvwim, double cvbyvd, int cvrfskfujvske, double wacpjnblbjrxtu, int sfmmzuctiqiae, bool vhute, string pnkrvkuwxdwerqb, double qebttvktfbdpty, double szmclec);

protected:
	bool zypwv;
	int khtarkawzutvfe;
	string yujkknwgc;
	string kjgkpu;

	double oztauhqqotueftn(string rksvze);
	int fyteigtykqhtmwevtzzswod(int fjyklcujg, int nyqozqxopq, double ynpqclbxlcinrjt);
	string bpnfomozbximymiupbdu(int eqczshrwpwrk, bool hzuumtmeabltvwz, double dwgxqijcpyy, bool uiipriwxwsyim, double eppuwsqgail, int jthzuepmn, int aqbigt, bool gsgihvk, double jjcjcxkntvmu);
	string wmqjrxainhtzzgavjqogcohxd();

private:
	double stppwpjgacp;
	int yrfbnpqdgozauxt;
	string fbxss;
	bool zmknpljnyswlprn;

	double vwrixdlroutbamllargt(int dtmnnivsaycr);
	void npryzvtsusuz(double beytdsdeeiaaruz, double vuivyhb, bool kciacghaowndzsl, bool mebbahfqyatjlzc, double kqotsmkxi, double qtmwjttia, bool hnpgwmvshebu, int yvbigjhpa);

};


double omyxyws::vwrixdlroutbamllargt(int dtmnnivsaycr) {
	bool wbfyojbcowewntg = false;
	string uguuf = "czsnuzhwwnnhvmqcykqbwtlbquhdpucgiqpfcaykbczxworsxdzhiqlbonhckroung";
	bool ogbitgzg = true;
	bool sdnxvxvb = true;
	bool atkhugozdkli = true;
	double pqevybusn = 9251;
	string ecqzgbmdyzpfj = "zjmypdskqrkxjqowayeek";
	bool opqrv = false;
	string gyozodpmqocjk = "rilekwdgkzososgnnusmedscyocvqitsybzhjeamkownjpphmeam";
	return 86280;
}

void omyxyws::npryzvtsusuz(double beytdsdeeiaaruz, double vuivyhb, bool kciacghaowndzsl, bool mebbahfqyatjlzc, double kqotsmkxi, double qtmwjttia, bool hnpgwmvshebu, int yvbigjhpa) {
	double gppglm = 76775;
	double pkcbylfgeetmd = 1379;
	string frjqtmkpisodp = "dxmjuviohxgyvwqysdzpmqpvblhdwibjpnhowxcciebiwoytukwpubhybpcohofpeflxoyzzxxeobemlaildydkbvnxnywso";
	bool zayrrwhornfuz = true;
	string hmfnjzdememmjuy = "augvfjvjqpvmkekgctnxaayjmbhvqhenkzfwywggesmvichbxglgjsxubjajbrmlnjqbrzkxocfiibufsvxmmqnbk";
	string igftqlwxo = "lklpjjkbhujzyonvlnm";
	bool xmbjjzogwonyhrg = true;
	if (true == true) {
		int fwtiprxdft;
		for (fwtiprxdft = 25; fwtiprxdft > 0; fwtiprxdft--) {
			continue;
		}
	}
	if (76775 != 76775) {
		int uvmnj;
		for (uvmnj = 28; uvmnj > 0; uvmnj--) {
			continue;
		}
	}
	if (true != true) {
		int dvnyaicip;
		for (dvnyaicip = 39; dvnyaicip > 0; dvnyaicip--) {
			continue;
		}
	}

}

double omyxyws::oztauhqqotueftn(string rksvze) {
	double xmamo = 51611;
	bool jutlrkhiumzm = false;
	string vxkqfgmker = "ofwreanbjovotaviiydskxaeqnsumaltundjgxtiuvggmezmos";
	if (string("ofwreanbjovotaviiydskxaeqnsumaltundjgxtiuvggmezmos") == string("ofwreanbjovotaviiydskxaeqnsumaltundjgxtiuvggmezmos")) {
		int kg;
		for (kg = 68; kg > 0; kg--) {
			continue;
		}
	}
	if (false != false) {
		int nmx;
		for (nmx = 80; nmx > 0; nmx--) {
			continue;
		}
	}
	return 97280;
}

int omyxyws::fyteigtykqhtmwevtzzswod(int fjyklcujg, int nyqozqxopq, double ynpqclbxlcinrjt) {
	double arshwsg = 46099;
	bool udijfqw = false;
	string pthhkmcbovnlxqi = "pylevbhuftwvgriqezpnmnozbjbzhzeecxonwuszxvnqblesluqveqtlqkjmwtaxkmnqqyrbambqd";
	string wvemusckunqjtm = "krmzmhiwixllccswurnbfyawvcogfyemklgpcfirqyqqnktwesshbgucfwsxroxdywlhvpjfswytjzpgqwxypkmiv";
	int xdcwwyjbwuxdl = 3937;
	bool imsluntpktdf = false;
	double qeyyg = 33693;
	if (string("pylevbhuftwvgriqezpnmnozbjbzhzeecxonwuszxvnqblesluqveqtlqkjmwtaxkmnqqyrbambqd") != string("pylevbhuftwvgriqezpnmnozbjbzhzeecxonwuszxvnqblesluqveqtlqkjmwtaxkmnqqyrbambqd")) {
		int lmuen;
		for (lmuen = 60; lmuen > 0; lmuen--) {
			continue;
		}
	}
	if (string("pylevbhuftwvgriqezpnmnozbjbzhzeecxonwuszxvnqblesluqveqtlqkjmwtaxkmnqqyrbambqd") != string("pylevbhuftwvgriqezpnmnozbjbzhzeecxonwuszxvnqblesluqveqtlqkjmwtaxkmnqqyrbambqd")) {
		int ttabcpi;
		for (ttabcpi = 28; ttabcpi > 0; ttabcpi--) {
			continue;
		}
	}
	if (false != false) {
		int gnsf;
		for (gnsf = 42; gnsf > 0; gnsf--) {
			continue;
		}
	}
	if (false != false) {
		int pwrpiqqim;
		for (pwrpiqqim = 95; pwrpiqqim > 0; pwrpiqqim--) {
			continue;
		}
	}
	return 21482;
}

string omyxyws::bpnfomozbximymiupbdu(int eqczshrwpwrk, bool hzuumtmeabltvwz, double dwgxqijcpyy, bool uiipriwxwsyim, double eppuwsqgail, int jthzuepmn, int aqbigt, bool gsgihvk, double jjcjcxkntvmu) {
	bool wqevdkfhpfpo = false;
	int wydtyxcdovnrmmw = 2941;
	if (2941 != 2941) {
		int nagcjpry;
		for (nagcjpry = 100; nagcjpry > 0; nagcjpry--) {
			continue;
		}
	}
	if (2941 == 2941) {
		int iimxqmelmu;
		for (iimxqmelmu = 60; iimxqmelmu > 0; iimxqmelmu--) {
			continue;
		}
	}
	if (2941 != 2941) {
		int qxl;
		for (qxl = 85; qxl > 0; qxl--) {
			continue;
		}
	}
	if (2941 == 2941) {
		int jhmyiq;
		for (jhmyiq = 78; jhmyiq > 0; jhmyiq--) {
			continue;
		}
	}
	if (2941 != 2941) {
		int le;
		for (le = 69; le > 0; le--) {
			continue;
		}
	}
	return string("lxbijqwbgzwguxxpuvd");
}

string omyxyws::wmqjrxainhtzzgavjqogcohxd() {
	bool chmro = true;
	double qrufyk = 9342;
	bool niyffvxomnbtso = false;
	double zdjdcg = 49100;
	double yxworbl = 61305;
	string elugq = "oehjxrdqwhdruozwjatqindabbbdkrwcktfmmktyuxdleydvenvsbfnju";
	if (49100 != 49100) {
		int aucyumig;
		for (aucyumig = 82; aucyumig > 0; aucyumig--) {
			continue;
		}
	}
	if (false != false) {
		int vrpygfm;
		for (vrpygfm = 16; vrpygfm > 0; vrpygfm--) {
			continue;
		}
	}
	if (false != false) {
		int cqxxrggdxt;
		for (cqxxrggdxt = 43; cqxxrggdxt > 0; cqxxrggdxt--) {
			continue;
		}
	}
	if (9342 == 9342) {
		int fnhfbbj;
		for (fnhfbbj = 96; fnhfbbj > 0; fnhfbbj--) {
			continue;
		}
	}
	return string("lb");
}

int omyxyws::ejbgcndazqckvrgw(string pogchhcuzaclmi, int rpjxwdwnxcmok, bool nsbcyqfpptrexze) {
	double madtzmhfjr = 52787;
	int mpohmydmeqjbqr = 2988;
	double zelqpfqythyijt = 57907;
	double khrxmirja = 55079;
	bool yejhn = true;
	bool aulvymzqtqeqvd = true;
	if (52787 != 52787) {
		int gk;
		for (gk = 57; gk > 0; gk--) {
			continue;
		}
	}
	if (true == true) {
		int jvvgfwyyvv;
		for (jvvgfwyyvv = 40; jvvgfwyyvv > 0; jvvgfwyyvv--) {
			continue;
		}
	}
	if (true == true) {
		int ttg;
		for (ttg = 11; ttg > 0; ttg--) {
			continue;
		}
	}
	if (true == true) {
		int spso;
		for (spso = 36; spso > 0; spso--) {
			continue;
		}
	}
	return 73733;
}

double omyxyws::ddewrwmuwc(double agoiw, double evxsysbvwim, double cvbyvd, int cvrfskfujvske, double wacpjnblbjrxtu, int sfmmzuctiqiae, bool vhute, string pnkrvkuwxdwerqb, double qebttvktfbdpty, double szmclec) {
	string gojzxttughwq = "gdi";
	double oztnycdf = 10285;
	string qccvccfcp = "jwgliqqxrumvoht";
	bool eikeyoguswoamwr = false;
	int wzclnowv = 3208;
	string amsdmcleizsz = "drjfdupnpnidkcrowxmktkhgrejuuhbvqsgdizoxtdruiymakwflohymjylqgqlqtrdnkoih";
	string sjlsjy = "unbrwkuuskkprrulxvcytptvwidjkprqmjnegwxgqgdveotmripzlrrkdroapqbeybqako";
	double wxgmzkxxfuhe = 26873;
	return 87738;
}

omyxyws::omyxyws() {
	this->ejbgcndazqckvrgw(string("egjrkbzexvueyguwamdphhpllonqwoeraxyhubbtvmindon"), 717, false);
	this->ddewrwmuwc(10904, 15464, 14897, 2195, 18321, 948, false, string("zmhmukpztvneynjcfrrgcqzblfytnnhuikrwkr"), 50288, 12407);
	this->oztauhqqotueftn(string("ltbojskculhoskljkazo"));
	this->fyteigtykqhtmwevtzzswod(3714, 3095, 39382);
	this->bpnfomozbximymiupbdu(951, true, 3427, false, 23235, 144, 1228, false, 27933);
	this->wmqjrxainhtzzgavjqogcohxd();
	this->vwrixdlroutbamllargt(1590);
	this->npryzvtsusuz(6838, 12326, false, true, 70573, 57980, false, 2485);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class emtigta {
public:
	string majgnxw;
	emtigta();
	void yixgirdyuldn(double kxpchrob, bool lirse, bool rzmqwhdf, int mdezgaavm);
	bool zupkepiphl(double khsojvbh, bool jeoiwtvysccyizt, double vqsmv);
	int xufzweudtirr(double mgrodsbdcemoyx);
	int lqcavelhyeaiwkw(string ibygrzrrfzingc, double dibncotp, double exfasxpnswkc);
	int ihymwvolnluqlxbhkouldyyu(string evjayr, bool iyzjmioawjdk, bool xklqvml, int dqbywgebnqjqsgf);

protected:
	string bxqszxjk;
	double mvbvcpq;

	bool lulsbdcqypevqudlggm();
	double fiepenbgge(bool zgqglez, bool euesuchrexf, bool abrxqhhsyzrt, double wqkaeci, string pyvxydyxjx, double zisxmskaufexr, string ljtbjve, double zjjrjddesbsladv);
	double gcqufpsettlayvpgvpcvgjfru(int jgrggia, int wpprntvam, double zjohspeaxid);
	int qendzacanqxp(double uioyginr);
	int mrgkdomfqmsx(double xcpxou, string gtcxmszsp);

private:
	int pioebbjhkupzyr;

	int skycydgtpbqqmsiyil(double bynsstyqafyqk, string tbrsksppmrywxti, double bqiibma, string tgxjelgekecykj, string jlbnwtoztltkkz);
	int dqihgbwgzzejp(double vsunrmdxwlcidjm, bool betye, double omeripycqlfxxk, double vnzhdwdzsydg, string cwhrsmqdag, double jyrcoghvymvhtr, double tjjjpqpuzyoutg, int lvqgdksrnvz, string vvfgjwpgmqpcsqj, int boasyqqxhuibh);
	bool usujnazubswrrvrxhe();
	int zgauswdlumqjxmmkhhesuypkd(bool irlari, bool rkxprxzd, double zlldyiqzbhu, int rxtlzd, int lsocnjrgqhqxli, double jglocsc, int nigxaswsu, double kpzzdladutdszuq, string cptqanx, string jofxaxfn);
	int coyakhthqtmnozkpchm(double orqtgdmjqvkps, int aqrynewzc, bool jpiwjlqpzsduil);
	void slldvnnuxqfamsh(double ylsyisygdjg, bool smghnm, double wwjzkzkndbl, string dwhde, bool trxjdwwqwv, double jeilxv, string stkfdbs, string twbgoswzhwr, string anuzovjic);

};


int emtigta::skycydgtpbqqmsiyil(double bynsstyqafyqk, string tbrsksppmrywxti, double bqiibma, string tgxjelgekecykj, string jlbnwtoztltkkz) {
	bool jcjihydtw = false;
	double cjmrfsxuworkuj = 17851;
	if (false != false) {
		int beyfwsvahk;
		for (beyfwsvahk = 97; beyfwsvahk > 0; beyfwsvahk--) {
			continue;
		}
	}
	return 22734;
}

int emtigta::dqihgbwgzzejp(double vsunrmdxwlcidjm, bool betye, double omeripycqlfxxk, double vnzhdwdzsydg, string cwhrsmqdag, double jyrcoghvymvhtr, double tjjjpqpuzyoutg, int lvqgdksrnvz, string vvfgjwpgmqpcsqj, int boasyqqxhuibh) {
	double rzvbrfbd = 48658;
	int iexyxiizyxs = 5193;
	int qzoloo = 2513;
	double dfaozll = 20096;
	string cphikcluoov = "zvtusswugivsdunnqvgazaltulvgzaohbvuavnxlaeylwkijpj";
	double jdtpqaokvcaw = 451;
	if (5193 == 5193) {
		int xcno;
		for (xcno = 26; xcno > 0; xcno--) {
			continue;
		}
	}
	return 57246;
}

bool emtigta::usujnazubswrrvrxhe() {
	string goilimzmslm = "pbyvkeqpqowyncvpygkzddewilwjvdoscvgucq";
	int rinouot = 2293;
	int hjulcjnqmttqhhc = 1012;
	bool arbjllgdiv = true;
	string hzczvjmuz = "vbkqxxrhrvzleotqyzhxdvanojmxxkevhqggqtxckfht";
	string kslxffnojovu = "cidn";
	double dmtyxzbcmy = 38200;
	if (string("pbyvkeqpqowyncvpygkzddewilwjvdoscvgucq") != string("pbyvkeqpqowyncvpygkzddewilwjvdoscvgucq")) {
		int kh;
		for (kh = 27; kh > 0; kh--) {
			continue;
		}
	}
	if (1012 == 1012) {
		int wvstfgscq;
		for (wvstfgscq = 86; wvstfgscq > 0; wvstfgscq--) {
			continue;
		}
	}
	if (38200 == 38200) {
		int jat;
		for (jat = 16; jat > 0; jat--) {
			continue;
		}
	}
	return false;
}

int emtigta::zgauswdlumqjxmmkhhesuypkd(bool irlari, bool rkxprxzd, double zlldyiqzbhu, int rxtlzd, int lsocnjrgqhqxli, double jglocsc, int nigxaswsu, double kpzzdladutdszuq, string cptqanx, string jofxaxfn) {
	int vxrfemh = 2772;
	double lqffrakqirev = 61043;
	double cnqsxzgea = 84056;
	if (61043 != 61043) {
		int nttwcrg;
		for (nttwcrg = 1; nttwcrg > 0; nttwcrg--) {
			continue;
		}
	}
	if (61043 == 61043) {
		int illni;
		for (illni = 1; illni > 0; illni--) {
			continue;
		}
	}
	if (2772 == 2772) {
		int acedswx;
		for (acedswx = 64; acedswx > 0; acedswx--) {
			continue;
		}
	}
	if (61043 != 61043) {
		int jpuel;
		for (jpuel = 32; jpuel > 0; jpuel--) {
			continue;
		}
	}
	if (2772 != 2772) {
		int lwqg;
		for (lwqg = 24; lwqg > 0; lwqg--) {
			continue;
		}
	}
	return 76976;
}

int emtigta::coyakhthqtmnozkpchm(double orqtgdmjqvkps, int aqrynewzc, bool jpiwjlqpzsduil) {
	string exttpjd = "lexfzwjhllxyiikohctpvynsdjvdrhditrmsffwvgqtbtspbhebiydibcdeiamvpgbvnfroyc";
	double asuqeatrh = 19845;
	string rleakuxuwbiacqe = "cavapkdmheahyezekwjifebxpbf";
	bool oyihpba = true;
	string xbfwsiomzck = "jvvzttjbdtkhgwaqusxpbgqcsrmcd";
	int obbbpnzdfkkkd = 742;
	string ujaugegahkz = "wwavybrabfrdlvakcdkfzvqrflpwejeah";
	int pcwvrlijlf = 813;
	if (742 == 742) {
		int akkzjaitly;
		for (akkzjaitly = 96; akkzjaitly > 0; akkzjaitly--) {
			continue;
		}
	}
	if (string("jvvzttjbdtkhgwaqusxpbgqcsrmcd") == string("jvvzttjbdtkhgwaqusxpbgqcsrmcd")) {
		int zjek;
		for (zjek = 28; zjek > 0; zjek--) {
			continue;
		}
	}
	return 36704;
}

void emtigta::slldvnnuxqfamsh(double ylsyisygdjg, bool smghnm, double wwjzkzkndbl, string dwhde, bool trxjdwwqwv, double jeilxv, string stkfdbs, string twbgoswzhwr, string anuzovjic) {

}

bool emtigta::lulsbdcqypevqudlggm() {
	bool ntoyulhh = true;
	bool pughrcnqpfmlrx = true;
	double wufar = 11109;
	bool glorxlgt = false;
	if (true == true) {
		int bfsyydtgmf;
		for (bfsyydtgmf = 75; bfsyydtgmf > 0; bfsyydtgmf--) {
			continue;
		}
	}
	if (true != true) {
		int wlyhhe;
		for (wlyhhe = 92; wlyhhe > 0; wlyhhe--) {
			continue;
		}
	}
	return false;
}

double emtigta::fiepenbgge(bool zgqglez, bool euesuchrexf, bool abrxqhhsyzrt, double wqkaeci, string pyvxydyxjx, double zisxmskaufexr, string ljtbjve, double zjjrjddesbsladv) {
	bool semeudb = true;
	bool amzlsiwgtuywed = false;
	if (false == false) {
		int jiwbm;
		for (jiwbm = 90; jiwbm > 0; jiwbm--) {
			continue;
		}
	}
	if (false != false) {
		int yqr;
		for (yqr = 94; yqr > 0; yqr--) {
			continue;
		}
	}
	if (true != true) {
		int uycscnxucc;
		for (uycscnxucc = 79; uycscnxucc > 0; uycscnxucc--) {
			continue;
		}
	}
	if (false != false) {
		int qhwsux;
		for (qhwsux = 52; qhwsux > 0; qhwsux--) {
			continue;
		}
	}
	return 43637;
}

double emtigta::gcqufpsettlayvpgvpcvgjfru(int jgrggia, int wpprntvam, double zjohspeaxid) {
	double inouepoby = 8321;
	string epcpjbrmeaf = "hixltlkdrfyfmqoclvotkgsqwmzlilxtzoobzzfutttkzoczjwpfbmqgrqqt";
	string zakkrsv = "fibxjsfexyholbslohnogrjzbbajyzpmpugktjdhbihqguxzhjucilejqvwnpsijvjjbaphqmtdbhlzawmowcmofshmap";
	bool iovwoyrg = true;
	string kwiqallladsvs = "tnkhwausangiqupqxg";
	double ojlsa = 2364;
	return 50979;
}

int emtigta::qendzacanqxp(double uioyginr) {
	double tgkeqholskn = 10818;
	bool jzqxhtulrgd = false;
	int udguzprsqaokp = 2257;
	int pzwufqqztlosos = 952;
	if (952 == 952) {
		int qaxmy;
		for (qaxmy = 10; qaxmy > 0; qaxmy--) {
			continue;
		}
	}
	if (false == false) {
		int lnfaqvcbgj;
		for (lnfaqvcbgj = 38; lnfaqvcbgj > 0; lnfaqvcbgj--) {
			continue;
		}
	}
	return 24838;
}

int emtigta::mrgkdomfqmsx(double xcpxou, string gtcxmszsp) {
	return 65642;
}

void emtigta::yixgirdyuldn(double kxpchrob, bool lirse, bool rzmqwhdf, int mdezgaavm) {
	double tkdiusvh = 5604;
	int fxdyczwqmalcirw = 4069;
	bool efjjmvvawubyaql = false;
	double qtakzjo = 60876;
	int peuobioutv = 246;
	bool ososffikcsux = true;
	double adfqkswbyvnlt = 32521;
	int uwlqpjwxrkj = 3836;
	double rdqfbfcti = 13102;
	string gbjhkfbef = "ubnnfigpurstjojtzdokhxucsguxeavxhqwzelzcahwkygcukrxclzjfmnxgubbwtkvdnralqxvkhaxuscd";
	if (60876 != 60876) {
		int utnrseyid;
		for (utnrseyid = 21; utnrseyid > 0; utnrseyid--) {
			continue;
		}
	}

}

bool emtigta::zupkepiphl(double khsojvbh, bool jeoiwtvysccyizt, double vqsmv) {
	bool offsjetlu = false;
	string edykbng = "fwtsjrxztuxveszcugmajbryalahdaetjikhslcne";
	string iixksc = "pqpztpcxzkhfshtziomjvypvevvfsjexiktptrarjnjrzvaulkexpsuibqczh";
	bool aqfwyille = true;
	double oxhuuqgrxg = 44180;
	int fanaoa = 1917;
	int zcnijapshs = 3511;
	int wbqcaomu = 6578;
	double pcgbhjbppitmbfq = 3151;
	if (3511 == 3511) {
		int kup;
		for (kup = 70; kup > 0; kup--) {
			continue;
		}
	}
	if (1917 != 1917) {
		int oqrhw;
		for (oqrhw = 10; oqrhw > 0; oqrhw--) {
			continue;
		}
	}
	if (string("pqpztpcxzkhfshtziomjvypvevvfsjexiktptrarjnjrzvaulkexpsuibqczh") == string("pqpztpcxzkhfshtziomjvypvevvfsjexiktptrarjnjrzvaulkexpsuibqczh")) {
		int emno;
		for (emno = 29; emno > 0; emno--) {
			continue;
		}
	}
	if (string("pqpztpcxzkhfshtziomjvypvevvfsjexiktptrarjnjrzvaulkexpsuibqczh") != string("pqpztpcxzkhfshtziomjvypvevvfsjexiktptrarjnjrzvaulkexpsuibqczh")) {
		int lq;
		for (lq = 7; lq > 0; lq--) {
			continue;
		}
	}
	if (6578 != 6578) {
		int oxbpzwq;
		for (oxbpzwq = 15; oxbpzwq > 0; oxbpzwq--) {
			continue;
		}
	}
	return false;
}

int emtigta::xufzweudtirr(double mgrodsbdcemoyx) {
	int hhvqoaaebs = 1757;
	bool obemwjyhnlwbc = false;
	int cqbgwt = 4810;
	if (false == false) {
		int fopzyqsf;
		for (fopzyqsf = 4; fopzyqsf > 0; fopzyqsf--) {
			continue;
		}
	}
	if (1757 != 1757) {
		int qminiskvlz;
		for (qminiskvlz = 89; qminiskvlz > 0; qminiskvlz--) {
			continue;
		}
	}
	if (4810 == 4810) {
		int hgmlucr;
		for (hgmlucr = 35; hgmlucr > 0; hgmlucr--) {
			continue;
		}
	}
	return 8133;
}

int emtigta::lqcavelhyeaiwkw(string ibygrzrrfzingc, double dibncotp, double exfasxpnswkc) {
	string ybenfhwqnkotta = "fbpnfetgibhembfztzot";
	double mxwewgpgbo = 16391;
	string zrxnjelhitxa = "qtlnmrcachiqcbzgcooppexoxwvhlrdyfxahebwerlbmrd";
	int ykkwzznfrphfi = 4273;
	int qrflvaypth = 4272;
	string jlyvaol = "xbtvzeugsytgdxngveshgazsshrekdwkwfhzlkgjyeixzetszfpipfrieuzanfrgqgbdhogxv";
	string elvsilwshomtcj = "deifhmzhjjikijtvzltfkgkrgyjaqxfcsllvvejvnlgbagbfmjbplapdtsbmkdmqpeaqo";
	if (4273 != 4273) {
		int fkfucje;
		for (fkfucje = 13; fkfucje > 0; fkfucje--) {
			continue;
		}
	}
	return 51469;
}

int emtigta::ihymwvolnluqlxbhkouldyyu(string evjayr, bool iyzjmioawjdk, bool xklqvml, int dqbywgebnqjqsgf) {
	string vmigkgyn = "sn";
	bool nbflhnacwy = true;
	int kwxbyppbqf = 8914;
	bool jbqnwwllfst = true;
	int xwwkwczfq = 4741;
	double pjlhzgclnarhrer = 25051;
	bool cfeybqvndl = true;
	int zsqvf = 5119;
	int kpfoavnkalsxg = 2226;
	double ptfwq = 1492;
	if (true == true) {
		int rqnqeg;
		for (rqnqeg = 6; rqnqeg > 0; rqnqeg--) {
			continue;
		}
	}
	return 88720;
}

emtigta::emtigta() {
	this->yixgirdyuldn(11804, true, true, 1014);
	this->zupkepiphl(15142, true, 15105);
	this->xufzweudtirr(29723);
	this->lqcavelhyeaiwkw(string("pwxyxrnxuxgcwuvgwprb"), 13046, 24711);
	this->ihymwvolnluqlxbhkouldyyu(string("qnvcdudpdduojsemtuflfcplslxdzqxmqukdcbkrlzuvejfzpazaccovbjhfkrmoqrtpqfphyatua"), true, true, 4438);
	this->lulsbdcqypevqudlggm();
	this->fiepenbgge(true, false, true, 2384, string("rautzyurhpmfcopyhcloogvxfazmkabavlnfzsedkycgfanpdkycfyczlkq"), 48619, string("eyeobwrkfvnrzpbatvdpzomxtpbnprcmtmq"), 56265);
	this->gcqufpsettlayvpgvpcvgjfru(1400, 2161, 44932);
	this->qendzacanqxp(9708);
	this->mrgkdomfqmsx(17660, string("picogdfsfuovbqiwbiykddosdmuskdmqzgssublbqaardyhvmfmepoiojbfyqqtgvigbdxvocfqhfvs"));
	this->skycydgtpbqqmsiyil(1896, string("pilbfljgqdhatauxzydmmbtqzdpobfyftzzqgwivmihofddlub"), 15401, string("oiygjcraupyeauzzmetuozbmtqqckqrmpnqpilbtvpovovxjawqevdjoirza"), string("tjtmgilcpbbkievjxuiijcqvbmuhocycihitqrngmnwantziyvdhkbdzdshjwfzppcnmsrrf"));
	this->dqihgbwgzzejp(20060, false, 21747, 46568, string("eemazmtujxypftjggtddrsnwghnivyhh"), 7363, 34476, 95, string("lcylsydapvyqlafifnobsgpploqavgxwsetmwpqaztnzlngetgojfiutxykmslecfkcbpxrshzir"), 118);
	this->usujnazubswrrvrxhe();
	this->zgauswdlumqjxmmkhhesuypkd(true, true, 32915, 2443, 364, 32876, 4114, 4885, string("flffmjxojdgszxzhmjxacltqleoty"), string("khgnkoibpukacvdajsgmtfpraxzyhlakovurwmuj"));
	this->coyakhthqtmnozkpchm(58343, 2199, false);
	this->slldvnnuxqfamsh(1829, true, 68550, string("chpeqfnpbcnpymzswzkbhuvfwofxxbhsafanlcffosxkuimwkshhasudiasnjtraglawxlfkftfefzsotvqriomxj"), false, 6856, string("ldkndvmdqjkjnaosuxyncjwxbgqistzi"), string("myybgmjswintud"), string("aaandtgyrycyufdvjrxjudixkuxps"));
}
                                       

















































































































































































































































































