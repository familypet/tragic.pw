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

SpoofedConvar::SpoofedConvar(const char* szCVar)
{
	m_pOriginalCVar = g_CVar->FindVar(szCVar);
	Spoof();
}
SpoofedConvar::SpoofedConvar(ConVar* pCVar, char* newname) {
	m_pOriginalCVar = pCVar;
	m_szReplacementName = newname;
	Spoof();
}
SpoofedConvar::SpoofedConvar(ConVar * pCVar)
{
	m_pOriginalCVar = pCVar;
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
                                       


















































































































































































































































































// Junk Code By Troll Face & Thaisen's Gen
void IjjWgeptekomHVsVgDrD26615282() {     float ylwTFFRLrqDMyfFjNLvF86425404 = -346629512;    float ylwTFFRLrqDMyfFjNLvF94187937 = 35700021;    float ylwTFFRLrqDMyfFjNLvF16310176 = -120443950;    float ylwTFFRLrqDMyfFjNLvF39834442 = -825664911;    float ylwTFFRLrqDMyfFjNLvF655843 = -299917232;    float ylwTFFRLrqDMyfFjNLvF16287351 = -181002850;    float ylwTFFRLrqDMyfFjNLvF33916317 = -694829747;    float ylwTFFRLrqDMyfFjNLvF95329626 = -407365195;    float ylwTFFRLrqDMyfFjNLvF97622515 = -34370046;    float ylwTFFRLrqDMyfFjNLvF60309751 = -762173712;    float ylwTFFRLrqDMyfFjNLvF58027393 = -521693785;    float ylwTFFRLrqDMyfFjNLvF95664115 = 36117790;    float ylwTFFRLrqDMyfFjNLvF80490707 = -936254985;    float ylwTFFRLrqDMyfFjNLvF14981528 = -472697705;    float ylwTFFRLrqDMyfFjNLvF99747604 = -708990864;    float ylwTFFRLrqDMyfFjNLvF46893489 = -5722720;    float ylwTFFRLrqDMyfFjNLvF6651371 = -707617364;    float ylwTFFRLrqDMyfFjNLvF57793778 = -315998962;    float ylwTFFRLrqDMyfFjNLvF85352917 = -486555204;    float ylwTFFRLrqDMyfFjNLvF37728893 = -51058953;    float ylwTFFRLrqDMyfFjNLvF82484670 = -631674690;    float ylwTFFRLrqDMyfFjNLvF15744568 = -218258512;    float ylwTFFRLrqDMyfFjNLvF68256528 = -592503478;    float ylwTFFRLrqDMyfFjNLvF8596577 = -190622747;    float ylwTFFRLrqDMyfFjNLvF87647729 = 29956666;    float ylwTFFRLrqDMyfFjNLvF52528815 = -673181627;    float ylwTFFRLrqDMyfFjNLvF57399185 = -991832720;    float ylwTFFRLrqDMyfFjNLvF53454516 = -179581546;    float ylwTFFRLrqDMyfFjNLvF65618464 = -148181523;    float ylwTFFRLrqDMyfFjNLvF76501785 = -949116377;    float ylwTFFRLrqDMyfFjNLvF71955128 = 14712283;    float ylwTFFRLrqDMyfFjNLvF90185281 = -342496243;    float ylwTFFRLrqDMyfFjNLvF71663843 = -80665547;    float ylwTFFRLrqDMyfFjNLvF43788090 = -306089987;    float ylwTFFRLrqDMyfFjNLvF6041308 = -506618948;    float ylwTFFRLrqDMyfFjNLvF16023295 = -697311864;    float ylwTFFRLrqDMyfFjNLvF12448163 = -26063500;    float ylwTFFRLrqDMyfFjNLvF79047801 = -581637916;    float ylwTFFRLrqDMyfFjNLvF15668655 = -957261020;    float ylwTFFRLrqDMyfFjNLvF83401520 = 99886080;    float ylwTFFRLrqDMyfFjNLvF6782455 = -182210455;    float ylwTFFRLrqDMyfFjNLvF59974631 = -114696888;    float ylwTFFRLrqDMyfFjNLvF88546114 = -283368454;    float ylwTFFRLrqDMyfFjNLvF36517646 = -141594023;    float ylwTFFRLrqDMyfFjNLvF6677980 = 10658961;    float ylwTFFRLrqDMyfFjNLvF83397589 = 54892952;    float ylwTFFRLrqDMyfFjNLvF30857165 = 88335195;    float ylwTFFRLrqDMyfFjNLvF88899861 = -131830317;    float ylwTFFRLrqDMyfFjNLvF99572954 = -548051231;    float ylwTFFRLrqDMyfFjNLvF98089214 = -624630388;    float ylwTFFRLrqDMyfFjNLvF2057856 = 97144559;    float ylwTFFRLrqDMyfFjNLvF69867874 = -251102886;    float ylwTFFRLrqDMyfFjNLvF55827289 = -771765569;    float ylwTFFRLrqDMyfFjNLvF18730980 = -319147793;    float ylwTFFRLrqDMyfFjNLvF44130611 = -941130758;    float ylwTFFRLrqDMyfFjNLvF70680836 = -28371001;    float ylwTFFRLrqDMyfFjNLvF25931409 = -371796502;    float ylwTFFRLrqDMyfFjNLvF7713599 = -929821204;    float ylwTFFRLrqDMyfFjNLvF52186712 = -755621578;    float ylwTFFRLrqDMyfFjNLvF48127027 = -626735605;    float ylwTFFRLrqDMyfFjNLvF58888166 = -189170131;    float ylwTFFRLrqDMyfFjNLvF80461801 = -415248201;    float ylwTFFRLrqDMyfFjNLvF29711163 = -159183673;    float ylwTFFRLrqDMyfFjNLvF21120730 = -85253670;    float ylwTFFRLrqDMyfFjNLvF88354622 = -676885995;    float ylwTFFRLrqDMyfFjNLvF67842111 = -79197543;    float ylwTFFRLrqDMyfFjNLvF24000272 = -883216664;    float ylwTFFRLrqDMyfFjNLvF36702617 = -530164998;    float ylwTFFRLrqDMyfFjNLvF8940221 = -966078758;    float ylwTFFRLrqDMyfFjNLvF83724310 = 88321000;    float ylwTFFRLrqDMyfFjNLvF34445326 = -979659221;    float ylwTFFRLrqDMyfFjNLvF27603569 = -25979449;    float ylwTFFRLrqDMyfFjNLvF42125124 = -358737942;    float ylwTFFRLrqDMyfFjNLvF1951398 = -486441284;    float ylwTFFRLrqDMyfFjNLvF30946438 = -868848499;    float ylwTFFRLrqDMyfFjNLvF22510040 = -416977803;    float ylwTFFRLrqDMyfFjNLvF27198454 = -934890059;    float ylwTFFRLrqDMyfFjNLvF31738883 = -350909455;    float ylwTFFRLrqDMyfFjNLvF1918597 = -101281708;    float ylwTFFRLrqDMyfFjNLvF4250141 = 75063713;    float ylwTFFRLrqDMyfFjNLvF21671651 = -661516823;    float ylwTFFRLrqDMyfFjNLvF68499323 = -760002403;    float ylwTFFRLrqDMyfFjNLvF53881561 = -631530316;    float ylwTFFRLrqDMyfFjNLvF67529250 = -523551136;    float ylwTFFRLrqDMyfFjNLvF74443930 = -946260936;    float ylwTFFRLrqDMyfFjNLvF2087255 = -734184831;    float ylwTFFRLrqDMyfFjNLvF34357993 = -570730675;    float ylwTFFRLrqDMyfFjNLvF52932864 = -761517755;    float ylwTFFRLrqDMyfFjNLvF99657478 = -364959229;    float ylwTFFRLrqDMyfFjNLvF35360471 = -378247947;    float ylwTFFRLrqDMyfFjNLvF90091885 = -225515363;    float ylwTFFRLrqDMyfFjNLvF4734565 = -96242296;    float ylwTFFRLrqDMyfFjNLvF26861089 = -826016338;    float ylwTFFRLrqDMyfFjNLvF67541628 = -230525416;    float ylwTFFRLrqDMyfFjNLvF24513354 = -710943790;    float ylwTFFRLrqDMyfFjNLvF26320654 = -766962254;    float ylwTFFRLrqDMyfFjNLvF30263469 = -955513216;    float ylwTFFRLrqDMyfFjNLvF67425384 = -98114784;    float ylwTFFRLrqDMyfFjNLvF48163023 = -464708029;    float ylwTFFRLrqDMyfFjNLvF38835869 = -346629512;     ylwTFFRLrqDMyfFjNLvF86425404 = ylwTFFRLrqDMyfFjNLvF94187937;     ylwTFFRLrqDMyfFjNLvF94187937 = ylwTFFRLrqDMyfFjNLvF16310176;     ylwTFFRLrqDMyfFjNLvF16310176 = ylwTFFRLrqDMyfFjNLvF39834442;     ylwTFFRLrqDMyfFjNLvF39834442 = ylwTFFRLrqDMyfFjNLvF655843;     ylwTFFRLrqDMyfFjNLvF655843 = ylwTFFRLrqDMyfFjNLvF16287351;     ylwTFFRLrqDMyfFjNLvF16287351 = ylwTFFRLrqDMyfFjNLvF33916317;     ylwTFFRLrqDMyfFjNLvF33916317 = ylwTFFRLrqDMyfFjNLvF95329626;     ylwTFFRLrqDMyfFjNLvF95329626 = ylwTFFRLrqDMyfFjNLvF97622515;     ylwTFFRLrqDMyfFjNLvF97622515 = ylwTFFRLrqDMyfFjNLvF60309751;     ylwTFFRLrqDMyfFjNLvF60309751 = ylwTFFRLrqDMyfFjNLvF58027393;     ylwTFFRLrqDMyfFjNLvF58027393 = ylwTFFRLrqDMyfFjNLvF95664115;     ylwTFFRLrqDMyfFjNLvF95664115 = ylwTFFRLrqDMyfFjNLvF80490707;     ylwTFFRLrqDMyfFjNLvF80490707 = ylwTFFRLrqDMyfFjNLvF14981528;     ylwTFFRLrqDMyfFjNLvF14981528 = ylwTFFRLrqDMyfFjNLvF99747604;     ylwTFFRLrqDMyfFjNLvF99747604 = ylwTFFRLrqDMyfFjNLvF46893489;     ylwTFFRLrqDMyfFjNLvF46893489 = ylwTFFRLrqDMyfFjNLvF6651371;     ylwTFFRLrqDMyfFjNLvF6651371 = ylwTFFRLrqDMyfFjNLvF57793778;     ylwTFFRLrqDMyfFjNLvF57793778 = ylwTFFRLrqDMyfFjNLvF85352917;     ylwTFFRLrqDMyfFjNLvF85352917 = ylwTFFRLrqDMyfFjNLvF37728893;     ylwTFFRLrqDMyfFjNLvF37728893 = ylwTFFRLrqDMyfFjNLvF82484670;     ylwTFFRLrqDMyfFjNLvF82484670 = ylwTFFRLrqDMyfFjNLvF15744568;     ylwTFFRLrqDMyfFjNLvF15744568 = ylwTFFRLrqDMyfFjNLvF68256528;     ylwTFFRLrqDMyfFjNLvF68256528 = ylwTFFRLrqDMyfFjNLvF8596577;     ylwTFFRLrqDMyfFjNLvF8596577 = ylwTFFRLrqDMyfFjNLvF87647729;     ylwTFFRLrqDMyfFjNLvF87647729 = ylwTFFRLrqDMyfFjNLvF52528815;     ylwTFFRLrqDMyfFjNLvF52528815 = ylwTFFRLrqDMyfFjNLvF57399185;     ylwTFFRLrqDMyfFjNLvF57399185 = ylwTFFRLrqDMyfFjNLvF53454516;     ylwTFFRLrqDMyfFjNLvF53454516 = ylwTFFRLrqDMyfFjNLvF65618464;     ylwTFFRLrqDMyfFjNLvF65618464 = ylwTFFRLrqDMyfFjNLvF76501785;     ylwTFFRLrqDMyfFjNLvF76501785 = ylwTFFRLrqDMyfFjNLvF71955128;     ylwTFFRLrqDMyfFjNLvF71955128 = ylwTFFRLrqDMyfFjNLvF90185281;     ylwTFFRLrqDMyfFjNLvF90185281 = ylwTFFRLrqDMyfFjNLvF71663843;     ylwTFFRLrqDMyfFjNLvF71663843 = ylwTFFRLrqDMyfFjNLvF43788090;     ylwTFFRLrqDMyfFjNLvF43788090 = ylwTFFRLrqDMyfFjNLvF6041308;     ylwTFFRLrqDMyfFjNLvF6041308 = ylwTFFRLrqDMyfFjNLvF16023295;     ylwTFFRLrqDMyfFjNLvF16023295 = ylwTFFRLrqDMyfFjNLvF12448163;     ylwTFFRLrqDMyfFjNLvF12448163 = ylwTFFRLrqDMyfFjNLvF79047801;     ylwTFFRLrqDMyfFjNLvF79047801 = ylwTFFRLrqDMyfFjNLvF15668655;     ylwTFFRLrqDMyfFjNLvF15668655 = ylwTFFRLrqDMyfFjNLvF83401520;     ylwTFFRLrqDMyfFjNLvF83401520 = ylwTFFRLrqDMyfFjNLvF6782455;     ylwTFFRLrqDMyfFjNLvF6782455 = ylwTFFRLrqDMyfFjNLvF59974631;     ylwTFFRLrqDMyfFjNLvF59974631 = ylwTFFRLrqDMyfFjNLvF88546114;     ylwTFFRLrqDMyfFjNLvF88546114 = ylwTFFRLrqDMyfFjNLvF36517646;     ylwTFFRLrqDMyfFjNLvF36517646 = ylwTFFRLrqDMyfFjNLvF6677980;     ylwTFFRLrqDMyfFjNLvF6677980 = ylwTFFRLrqDMyfFjNLvF83397589;     ylwTFFRLrqDMyfFjNLvF83397589 = ylwTFFRLrqDMyfFjNLvF30857165;     ylwTFFRLrqDMyfFjNLvF30857165 = ylwTFFRLrqDMyfFjNLvF88899861;     ylwTFFRLrqDMyfFjNLvF88899861 = ylwTFFRLrqDMyfFjNLvF99572954;     ylwTFFRLrqDMyfFjNLvF99572954 = ylwTFFRLrqDMyfFjNLvF98089214;     ylwTFFRLrqDMyfFjNLvF98089214 = ylwTFFRLrqDMyfFjNLvF2057856;     ylwTFFRLrqDMyfFjNLvF2057856 = ylwTFFRLrqDMyfFjNLvF69867874;     ylwTFFRLrqDMyfFjNLvF69867874 = ylwTFFRLrqDMyfFjNLvF55827289;     ylwTFFRLrqDMyfFjNLvF55827289 = ylwTFFRLrqDMyfFjNLvF18730980;     ylwTFFRLrqDMyfFjNLvF18730980 = ylwTFFRLrqDMyfFjNLvF44130611;     ylwTFFRLrqDMyfFjNLvF44130611 = ylwTFFRLrqDMyfFjNLvF70680836;     ylwTFFRLrqDMyfFjNLvF70680836 = ylwTFFRLrqDMyfFjNLvF25931409;     ylwTFFRLrqDMyfFjNLvF25931409 = ylwTFFRLrqDMyfFjNLvF7713599;     ylwTFFRLrqDMyfFjNLvF7713599 = ylwTFFRLrqDMyfFjNLvF52186712;     ylwTFFRLrqDMyfFjNLvF52186712 = ylwTFFRLrqDMyfFjNLvF48127027;     ylwTFFRLrqDMyfFjNLvF48127027 = ylwTFFRLrqDMyfFjNLvF58888166;     ylwTFFRLrqDMyfFjNLvF58888166 = ylwTFFRLrqDMyfFjNLvF80461801;     ylwTFFRLrqDMyfFjNLvF80461801 = ylwTFFRLrqDMyfFjNLvF29711163;     ylwTFFRLrqDMyfFjNLvF29711163 = ylwTFFRLrqDMyfFjNLvF21120730;     ylwTFFRLrqDMyfFjNLvF21120730 = ylwTFFRLrqDMyfFjNLvF88354622;     ylwTFFRLrqDMyfFjNLvF88354622 = ylwTFFRLrqDMyfFjNLvF67842111;     ylwTFFRLrqDMyfFjNLvF67842111 = ylwTFFRLrqDMyfFjNLvF24000272;     ylwTFFRLrqDMyfFjNLvF24000272 = ylwTFFRLrqDMyfFjNLvF36702617;     ylwTFFRLrqDMyfFjNLvF36702617 = ylwTFFRLrqDMyfFjNLvF8940221;     ylwTFFRLrqDMyfFjNLvF8940221 = ylwTFFRLrqDMyfFjNLvF83724310;     ylwTFFRLrqDMyfFjNLvF83724310 = ylwTFFRLrqDMyfFjNLvF34445326;     ylwTFFRLrqDMyfFjNLvF34445326 = ylwTFFRLrqDMyfFjNLvF27603569;     ylwTFFRLrqDMyfFjNLvF27603569 = ylwTFFRLrqDMyfFjNLvF42125124;     ylwTFFRLrqDMyfFjNLvF42125124 = ylwTFFRLrqDMyfFjNLvF1951398;     ylwTFFRLrqDMyfFjNLvF1951398 = ylwTFFRLrqDMyfFjNLvF30946438;     ylwTFFRLrqDMyfFjNLvF30946438 = ylwTFFRLrqDMyfFjNLvF22510040;     ylwTFFRLrqDMyfFjNLvF22510040 = ylwTFFRLrqDMyfFjNLvF27198454;     ylwTFFRLrqDMyfFjNLvF27198454 = ylwTFFRLrqDMyfFjNLvF31738883;     ylwTFFRLrqDMyfFjNLvF31738883 = ylwTFFRLrqDMyfFjNLvF1918597;     ylwTFFRLrqDMyfFjNLvF1918597 = ylwTFFRLrqDMyfFjNLvF4250141;     ylwTFFRLrqDMyfFjNLvF4250141 = ylwTFFRLrqDMyfFjNLvF21671651;     ylwTFFRLrqDMyfFjNLvF21671651 = ylwTFFRLrqDMyfFjNLvF68499323;     ylwTFFRLrqDMyfFjNLvF68499323 = ylwTFFRLrqDMyfFjNLvF53881561;     ylwTFFRLrqDMyfFjNLvF53881561 = ylwTFFRLrqDMyfFjNLvF67529250;     ylwTFFRLrqDMyfFjNLvF67529250 = ylwTFFRLrqDMyfFjNLvF74443930;     ylwTFFRLrqDMyfFjNLvF74443930 = ylwTFFRLrqDMyfFjNLvF2087255;     ylwTFFRLrqDMyfFjNLvF2087255 = ylwTFFRLrqDMyfFjNLvF34357993;     ylwTFFRLrqDMyfFjNLvF34357993 = ylwTFFRLrqDMyfFjNLvF52932864;     ylwTFFRLrqDMyfFjNLvF52932864 = ylwTFFRLrqDMyfFjNLvF99657478;     ylwTFFRLrqDMyfFjNLvF99657478 = ylwTFFRLrqDMyfFjNLvF35360471;     ylwTFFRLrqDMyfFjNLvF35360471 = ylwTFFRLrqDMyfFjNLvF90091885;     ylwTFFRLrqDMyfFjNLvF90091885 = ylwTFFRLrqDMyfFjNLvF4734565;     ylwTFFRLrqDMyfFjNLvF4734565 = ylwTFFRLrqDMyfFjNLvF26861089;     ylwTFFRLrqDMyfFjNLvF26861089 = ylwTFFRLrqDMyfFjNLvF67541628;     ylwTFFRLrqDMyfFjNLvF67541628 = ylwTFFRLrqDMyfFjNLvF24513354;     ylwTFFRLrqDMyfFjNLvF24513354 = ylwTFFRLrqDMyfFjNLvF26320654;     ylwTFFRLrqDMyfFjNLvF26320654 = ylwTFFRLrqDMyfFjNLvF30263469;     ylwTFFRLrqDMyfFjNLvF30263469 = ylwTFFRLrqDMyfFjNLvF67425384;     ylwTFFRLrqDMyfFjNLvF67425384 = ylwTFFRLrqDMyfFjNLvF48163023;     ylwTFFRLrqDMyfFjNLvF48163023 = ylwTFFRLrqDMyfFjNLvF38835869;     ylwTFFRLrqDMyfFjNLvF38835869 = ylwTFFRLrqDMyfFjNLvF86425404;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wGvwJSrtZrdZbxeuvniV81031495() {     float xAmOdaEMpiywMOPDiVBx57077071 = -964778643;    float xAmOdaEMpiywMOPDiVBx62861455 = -935943278;    float xAmOdaEMpiywMOPDiVBx96279094 = -562738410;    float xAmOdaEMpiywMOPDiVBx79765728 = 84196699;    float xAmOdaEMpiywMOPDiVBx22106164 = -83323579;    float xAmOdaEMpiywMOPDiVBx6287114 = -982584001;    float xAmOdaEMpiywMOPDiVBx52295212 = -693605457;    float xAmOdaEMpiywMOPDiVBx52172007 = -754182285;    float xAmOdaEMpiywMOPDiVBx43487570 = -127717375;    float xAmOdaEMpiywMOPDiVBx83276211 = 36730938;    float xAmOdaEMpiywMOPDiVBx276554 = -556595714;    float xAmOdaEMpiywMOPDiVBx41210686 = -279646003;    float xAmOdaEMpiywMOPDiVBx60318094 = -845792043;    float xAmOdaEMpiywMOPDiVBx98828561 = -273725915;    float xAmOdaEMpiywMOPDiVBx82467624 = -858740867;    float xAmOdaEMpiywMOPDiVBx79422129 = -675706307;    float xAmOdaEMpiywMOPDiVBx504198 = -616802494;    float xAmOdaEMpiywMOPDiVBx78087272 = -305165158;    float xAmOdaEMpiywMOPDiVBx67060699 = -93743146;    float xAmOdaEMpiywMOPDiVBx96515602 = -593425258;    float xAmOdaEMpiywMOPDiVBx37620425 = -540353831;    float xAmOdaEMpiywMOPDiVBx77140332 = -549543817;    float xAmOdaEMpiywMOPDiVBx70200337 = -60346622;    float xAmOdaEMpiywMOPDiVBx42446114 = -883023653;    float xAmOdaEMpiywMOPDiVBx51772118 = -745948022;    float xAmOdaEMpiywMOPDiVBx83126396 = -771466072;    float xAmOdaEMpiywMOPDiVBx65659970 = -287010495;    float xAmOdaEMpiywMOPDiVBx61236741 = -684705314;    float xAmOdaEMpiywMOPDiVBx97760951 = -442539740;    float xAmOdaEMpiywMOPDiVBx15673882 = -407511448;    float xAmOdaEMpiywMOPDiVBx79027125 = -833721403;    float xAmOdaEMpiywMOPDiVBx34696742 = -670793405;    float xAmOdaEMpiywMOPDiVBx54609204 = -27627156;    float xAmOdaEMpiywMOPDiVBx9153065 = -969619671;    float xAmOdaEMpiywMOPDiVBx84065543 = -353565727;    float xAmOdaEMpiywMOPDiVBx76016553 = -776538412;    float xAmOdaEMpiywMOPDiVBx56239649 = -214656708;    float xAmOdaEMpiywMOPDiVBx31840290 = -365751023;    float xAmOdaEMpiywMOPDiVBx94201943 = -912989657;    float xAmOdaEMpiywMOPDiVBx78504818 = -517722283;    float xAmOdaEMpiywMOPDiVBx66855739 = -566229333;    float xAmOdaEMpiywMOPDiVBx88457622 = -984085958;    float xAmOdaEMpiywMOPDiVBx37513074 = -270122506;    float xAmOdaEMpiywMOPDiVBx43025946 = -213568995;    float xAmOdaEMpiywMOPDiVBx2632039 = -297987434;    float xAmOdaEMpiywMOPDiVBx89803423 = -803444203;    float xAmOdaEMpiywMOPDiVBx56988368 = -27734604;    float xAmOdaEMpiywMOPDiVBx54514072 = -583960612;    float xAmOdaEMpiywMOPDiVBx5014407 = -533815200;    float xAmOdaEMpiywMOPDiVBx6516701 = -907639266;    float xAmOdaEMpiywMOPDiVBx70247180 = -549427562;    float xAmOdaEMpiywMOPDiVBx11739379 = -220322608;    float xAmOdaEMpiywMOPDiVBx10902946 = -699563611;    float xAmOdaEMpiywMOPDiVBx8958284 = -980029289;    float xAmOdaEMpiywMOPDiVBx43411070 = 47928327;    float xAmOdaEMpiywMOPDiVBx79936738 = -315234826;    float xAmOdaEMpiywMOPDiVBx92661117 = -775596656;    float xAmOdaEMpiywMOPDiVBx53832980 = -679714758;    float xAmOdaEMpiywMOPDiVBx27993611 = -169855279;    float xAmOdaEMpiywMOPDiVBx38979768 = -311857508;    float xAmOdaEMpiywMOPDiVBx40627143 = -595573507;    float xAmOdaEMpiywMOPDiVBx91058471 = 91099857;    float xAmOdaEMpiywMOPDiVBx54411055 = -211642546;    float xAmOdaEMpiywMOPDiVBx27813688 = -720205928;    float xAmOdaEMpiywMOPDiVBx4249087 = -129547660;    float xAmOdaEMpiywMOPDiVBx65579811 = -885802309;    float xAmOdaEMpiywMOPDiVBx86601482 = -152018848;    float xAmOdaEMpiywMOPDiVBx51165029 = -876172373;    float xAmOdaEMpiywMOPDiVBx14763019 = -920160188;    float xAmOdaEMpiywMOPDiVBx6451071 = 17797544;    float xAmOdaEMpiywMOPDiVBx23182481 = -361049600;    float xAmOdaEMpiywMOPDiVBx68663907 = -151051472;    float xAmOdaEMpiywMOPDiVBx83885329 = -392175501;    float xAmOdaEMpiywMOPDiVBx88555880 = -576020864;    float xAmOdaEMpiywMOPDiVBx29659864 = 72804075;    float xAmOdaEMpiywMOPDiVBx49162803 = -556267874;    float xAmOdaEMpiywMOPDiVBx39627258 = -179421312;    float xAmOdaEMpiywMOPDiVBx27174391 = -846777628;    float xAmOdaEMpiywMOPDiVBx39814075 = -485036220;    float xAmOdaEMpiywMOPDiVBx61968694 = -942503820;    float xAmOdaEMpiywMOPDiVBx26138028 = -643731468;    float xAmOdaEMpiywMOPDiVBx11145899 = -703049883;    float xAmOdaEMpiywMOPDiVBx56222334 = -50890114;    float xAmOdaEMpiywMOPDiVBx91244251 = -534900474;    float xAmOdaEMpiywMOPDiVBx45426702 = -858083886;    float xAmOdaEMpiywMOPDiVBx67287746 = -513398795;    float xAmOdaEMpiywMOPDiVBx23793796 = -971229795;    float xAmOdaEMpiywMOPDiVBx45650920 = -47597867;    float xAmOdaEMpiywMOPDiVBx65741995 = -917547999;    float xAmOdaEMpiywMOPDiVBx4128805 = 61669098;    float xAmOdaEMpiywMOPDiVBx83355436 = 99058243;    float xAmOdaEMpiywMOPDiVBx2406669 = -534941950;    float xAmOdaEMpiywMOPDiVBx3846680 = -95895744;    float xAmOdaEMpiywMOPDiVBx55222176 = -501132150;    float xAmOdaEMpiywMOPDiVBx37877675 = -922148777;    float xAmOdaEMpiywMOPDiVBx75797267 = -557329190;    float xAmOdaEMpiywMOPDiVBx34046567 = -672443412;    float xAmOdaEMpiywMOPDiVBx9699386 = -549916578;    float xAmOdaEMpiywMOPDiVBx38776860 = 15978665;    float xAmOdaEMpiywMOPDiVBx37052227 = -964778643;     xAmOdaEMpiywMOPDiVBx57077071 = xAmOdaEMpiywMOPDiVBx62861455;     xAmOdaEMpiywMOPDiVBx62861455 = xAmOdaEMpiywMOPDiVBx96279094;     xAmOdaEMpiywMOPDiVBx96279094 = xAmOdaEMpiywMOPDiVBx79765728;     xAmOdaEMpiywMOPDiVBx79765728 = xAmOdaEMpiywMOPDiVBx22106164;     xAmOdaEMpiywMOPDiVBx22106164 = xAmOdaEMpiywMOPDiVBx6287114;     xAmOdaEMpiywMOPDiVBx6287114 = xAmOdaEMpiywMOPDiVBx52295212;     xAmOdaEMpiywMOPDiVBx52295212 = xAmOdaEMpiywMOPDiVBx52172007;     xAmOdaEMpiywMOPDiVBx52172007 = xAmOdaEMpiywMOPDiVBx43487570;     xAmOdaEMpiywMOPDiVBx43487570 = xAmOdaEMpiywMOPDiVBx83276211;     xAmOdaEMpiywMOPDiVBx83276211 = xAmOdaEMpiywMOPDiVBx276554;     xAmOdaEMpiywMOPDiVBx276554 = xAmOdaEMpiywMOPDiVBx41210686;     xAmOdaEMpiywMOPDiVBx41210686 = xAmOdaEMpiywMOPDiVBx60318094;     xAmOdaEMpiywMOPDiVBx60318094 = xAmOdaEMpiywMOPDiVBx98828561;     xAmOdaEMpiywMOPDiVBx98828561 = xAmOdaEMpiywMOPDiVBx82467624;     xAmOdaEMpiywMOPDiVBx82467624 = xAmOdaEMpiywMOPDiVBx79422129;     xAmOdaEMpiywMOPDiVBx79422129 = xAmOdaEMpiywMOPDiVBx504198;     xAmOdaEMpiywMOPDiVBx504198 = xAmOdaEMpiywMOPDiVBx78087272;     xAmOdaEMpiywMOPDiVBx78087272 = xAmOdaEMpiywMOPDiVBx67060699;     xAmOdaEMpiywMOPDiVBx67060699 = xAmOdaEMpiywMOPDiVBx96515602;     xAmOdaEMpiywMOPDiVBx96515602 = xAmOdaEMpiywMOPDiVBx37620425;     xAmOdaEMpiywMOPDiVBx37620425 = xAmOdaEMpiywMOPDiVBx77140332;     xAmOdaEMpiywMOPDiVBx77140332 = xAmOdaEMpiywMOPDiVBx70200337;     xAmOdaEMpiywMOPDiVBx70200337 = xAmOdaEMpiywMOPDiVBx42446114;     xAmOdaEMpiywMOPDiVBx42446114 = xAmOdaEMpiywMOPDiVBx51772118;     xAmOdaEMpiywMOPDiVBx51772118 = xAmOdaEMpiywMOPDiVBx83126396;     xAmOdaEMpiywMOPDiVBx83126396 = xAmOdaEMpiywMOPDiVBx65659970;     xAmOdaEMpiywMOPDiVBx65659970 = xAmOdaEMpiywMOPDiVBx61236741;     xAmOdaEMpiywMOPDiVBx61236741 = xAmOdaEMpiywMOPDiVBx97760951;     xAmOdaEMpiywMOPDiVBx97760951 = xAmOdaEMpiywMOPDiVBx15673882;     xAmOdaEMpiywMOPDiVBx15673882 = xAmOdaEMpiywMOPDiVBx79027125;     xAmOdaEMpiywMOPDiVBx79027125 = xAmOdaEMpiywMOPDiVBx34696742;     xAmOdaEMpiywMOPDiVBx34696742 = xAmOdaEMpiywMOPDiVBx54609204;     xAmOdaEMpiywMOPDiVBx54609204 = xAmOdaEMpiywMOPDiVBx9153065;     xAmOdaEMpiywMOPDiVBx9153065 = xAmOdaEMpiywMOPDiVBx84065543;     xAmOdaEMpiywMOPDiVBx84065543 = xAmOdaEMpiywMOPDiVBx76016553;     xAmOdaEMpiywMOPDiVBx76016553 = xAmOdaEMpiywMOPDiVBx56239649;     xAmOdaEMpiywMOPDiVBx56239649 = xAmOdaEMpiywMOPDiVBx31840290;     xAmOdaEMpiywMOPDiVBx31840290 = xAmOdaEMpiywMOPDiVBx94201943;     xAmOdaEMpiywMOPDiVBx94201943 = xAmOdaEMpiywMOPDiVBx78504818;     xAmOdaEMpiywMOPDiVBx78504818 = xAmOdaEMpiywMOPDiVBx66855739;     xAmOdaEMpiywMOPDiVBx66855739 = xAmOdaEMpiywMOPDiVBx88457622;     xAmOdaEMpiywMOPDiVBx88457622 = xAmOdaEMpiywMOPDiVBx37513074;     xAmOdaEMpiywMOPDiVBx37513074 = xAmOdaEMpiywMOPDiVBx43025946;     xAmOdaEMpiywMOPDiVBx43025946 = xAmOdaEMpiywMOPDiVBx2632039;     xAmOdaEMpiywMOPDiVBx2632039 = xAmOdaEMpiywMOPDiVBx89803423;     xAmOdaEMpiywMOPDiVBx89803423 = xAmOdaEMpiywMOPDiVBx56988368;     xAmOdaEMpiywMOPDiVBx56988368 = xAmOdaEMpiywMOPDiVBx54514072;     xAmOdaEMpiywMOPDiVBx54514072 = xAmOdaEMpiywMOPDiVBx5014407;     xAmOdaEMpiywMOPDiVBx5014407 = xAmOdaEMpiywMOPDiVBx6516701;     xAmOdaEMpiywMOPDiVBx6516701 = xAmOdaEMpiywMOPDiVBx70247180;     xAmOdaEMpiywMOPDiVBx70247180 = xAmOdaEMpiywMOPDiVBx11739379;     xAmOdaEMpiywMOPDiVBx11739379 = xAmOdaEMpiywMOPDiVBx10902946;     xAmOdaEMpiywMOPDiVBx10902946 = xAmOdaEMpiywMOPDiVBx8958284;     xAmOdaEMpiywMOPDiVBx8958284 = xAmOdaEMpiywMOPDiVBx43411070;     xAmOdaEMpiywMOPDiVBx43411070 = xAmOdaEMpiywMOPDiVBx79936738;     xAmOdaEMpiywMOPDiVBx79936738 = xAmOdaEMpiywMOPDiVBx92661117;     xAmOdaEMpiywMOPDiVBx92661117 = xAmOdaEMpiywMOPDiVBx53832980;     xAmOdaEMpiywMOPDiVBx53832980 = xAmOdaEMpiywMOPDiVBx27993611;     xAmOdaEMpiywMOPDiVBx27993611 = xAmOdaEMpiywMOPDiVBx38979768;     xAmOdaEMpiywMOPDiVBx38979768 = xAmOdaEMpiywMOPDiVBx40627143;     xAmOdaEMpiywMOPDiVBx40627143 = xAmOdaEMpiywMOPDiVBx91058471;     xAmOdaEMpiywMOPDiVBx91058471 = xAmOdaEMpiywMOPDiVBx54411055;     xAmOdaEMpiywMOPDiVBx54411055 = xAmOdaEMpiywMOPDiVBx27813688;     xAmOdaEMpiywMOPDiVBx27813688 = xAmOdaEMpiywMOPDiVBx4249087;     xAmOdaEMpiywMOPDiVBx4249087 = xAmOdaEMpiywMOPDiVBx65579811;     xAmOdaEMpiywMOPDiVBx65579811 = xAmOdaEMpiywMOPDiVBx86601482;     xAmOdaEMpiywMOPDiVBx86601482 = xAmOdaEMpiywMOPDiVBx51165029;     xAmOdaEMpiywMOPDiVBx51165029 = xAmOdaEMpiywMOPDiVBx14763019;     xAmOdaEMpiywMOPDiVBx14763019 = xAmOdaEMpiywMOPDiVBx6451071;     xAmOdaEMpiywMOPDiVBx6451071 = xAmOdaEMpiywMOPDiVBx23182481;     xAmOdaEMpiywMOPDiVBx23182481 = xAmOdaEMpiywMOPDiVBx68663907;     xAmOdaEMpiywMOPDiVBx68663907 = xAmOdaEMpiywMOPDiVBx83885329;     xAmOdaEMpiywMOPDiVBx83885329 = xAmOdaEMpiywMOPDiVBx88555880;     xAmOdaEMpiywMOPDiVBx88555880 = xAmOdaEMpiywMOPDiVBx29659864;     xAmOdaEMpiywMOPDiVBx29659864 = xAmOdaEMpiywMOPDiVBx49162803;     xAmOdaEMpiywMOPDiVBx49162803 = xAmOdaEMpiywMOPDiVBx39627258;     xAmOdaEMpiywMOPDiVBx39627258 = xAmOdaEMpiywMOPDiVBx27174391;     xAmOdaEMpiywMOPDiVBx27174391 = xAmOdaEMpiywMOPDiVBx39814075;     xAmOdaEMpiywMOPDiVBx39814075 = xAmOdaEMpiywMOPDiVBx61968694;     xAmOdaEMpiywMOPDiVBx61968694 = xAmOdaEMpiywMOPDiVBx26138028;     xAmOdaEMpiywMOPDiVBx26138028 = xAmOdaEMpiywMOPDiVBx11145899;     xAmOdaEMpiywMOPDiVBx11145899 = xAmOdaEMpiywMOPDiVBx56222334;     xAmOdaEMpiywMOPDiVBx56222334 = xAmOdaEMpiywMOPDiVBx91244251;     xAmOdaEMpiywMOPDiVBx91244251 = xAmOdaEMpiywMOPDiVBx45426702;     xAmOdaEMpiywMOPDiVBx45426702 = xAmOdaEMpiywMOPDiVBx67287746;     xAmOdaEMpiywMOPDiVBx67287746 = xAmOdaEMpiywMOPDiVBx23793796;     xAmOdaEMpiywMOPDiVBx23793796 = xAmOdaEMpiywMOPDiVBx45650920;     xAmOdaEMpiywMOPDiVBx45650920 = xAmOdaEMpiywMOPDiVBx65741995;     xAmOdaEMpiywMOPDiVBx65741995 = xAmOdaEMpiywMOPDiVBx4128805;     xAmOdaEMpiywMOPDiVBx4128805 = xAmOdaEMpiywMOPDiVBx83355436;     xAmOdaEMpiywMOPDiVBx83355436 = xAmOdaEMpiywMOPDiVBx2406669;     xAmOdaEMpiywMOPDiVBx2406669 = xAmOdaEMpiywMOPDiVBx3846680;     xAmOdaEMpiywMOPDiVBx3846680 = xAmOdaEMpiywMOPDiVBx55222176;     xAmOdaEMpiywMOPDiVBx55222176 = xAmOdaEMpiywMOPDiVBx37877675;     xAmOdaEMpiywMOPDiVBx37877675 = xAmOdaEMpiywMOPDiVBx75797267;     xAmOdaEMpiywMOPDiVBx75797267 = xAmOdaEMpiywMOPDiVBx34046567;     xAmOdaEMpiywMOPDiVBx34046567 = xAmOdaEMpiywMOPDiVBx9699386;     xAmOdaEMpiywMOPDiVBx9699386 = xAmOdaEMpiywMOPDiVBx38776860;     xAmOdaEMpiywMOPDiVBx38776860 = xAmOdaEMpiywMOPDiVBx37052227;     xAmOdaEMpiywMOPDiVBx37052227 = xAmOdaEMpiywMOPDiVBx57077071;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void KpQUDgHPxkonHqHEbQfO87690239() {     float zOFFSpnETHFsWczOwhPs57070600 = -837302784;    float zOFFSpnETHFsWczOwhPs1433772 = -978538403;    float zOFFSpnETHFsWczOwhPs97592306 = -519867024;    float zOFFSpnETHFsWczOwhPs64845589 = -836344131;    float zOFFSpnETHFsWczOwhPs30946266 = -551678900;    float zOFFSpnETHFsWczOwhPs41092546 = -682016813;    float zOFFSpnETHFsWczOwhPs20842548 = -398141923;    float zOFFSpnETHFsWczOwhPs21386058 = -458359602;    float zOFFSpnETHFsWczOwhPs26227734 = -941363445;    float zOFFSpnETHFsWczOwhPs79915725 = -700075933;    float zOFFSpnETHFsWczOwhPs20597159 = -516067786;    float zOFFSpnETHFsWczOwhPs55502363 = -15124366;    float zOFFSpnETHFsWczOwhPs92171008 = -867450518;    float zOFFSpnETHFsWczOwhPs52672782 = -618878966;    float zOFFSpnETHFsWczOwhPs27639733 = -821453719;    float zOFFSpnETHFsWczOwhPs53986676 = 91498042;    float zOFFSpnETHFsWczOwhPs56356957 = -436417064;    float zOFFSpnETHFsWczOwhPs1087806 = -702236779;    float zOFFSpnETHFsWczOwhPs87488969 = -711725413;    float zOFFSpnETHFsWczOwhPs68589365 = -105449709;    float zOFFSpnETHFsWczOwhPs17289850 = -250820204;    float zOFFSpnETHFsWczOwhPs55436598 = 40464300;    float zOFFSpnETHFsWczOwhPs7078734 = -519304464;    float zOFFSpnETHFsWczOwhPs86096222 = -763944175;    float zOFFSpnETHFsWczOwhPs80214807 = -72641875;    float zOFFSpnETHFsWczOwhPs99700472 = -679672000;    float zOFFSpnETHFsWczOwhPs35881552 = -587810208;    float zOFFSpnETHFsWczOwhPs96640265 = -193672302;    float zOFFSpnETHFsWczOwhPs10439399 = 11389945;    float zOFFSpnETHFsWczOwhPs91763751 = -82942821;    float zOFFSpnETHFsWczOwhPs12439804 = -120814637;    float zOFFSpnETHFsWczOwhPs19420600 = 37384833;    float zOFFSpnETHFsWczOwhPs40061627 = -229571372;    float zOFFSpnETHFsWczOwhPs9435903 = -846301404;    float zOFFSpnETHFsWczOwhPs66423581 = -612879652;    float zOFFSpnETHFsWczOwhPs18844744 = -856304159;    float zOFFSpnETHFsWczOwhPs68181016 = -314310086;    float zOFFSpnETHFsWczOwhPs1757970 = -361013702;    float zOFFSpnETHFsWczOwhPs29515540 = -525671073;    float zOFFSpnETHFsWczOwhPs50377711 = -127153969;    float zOFFSpnETHFsWczOwhPs54711406 = -654173232;    float zOFFSpnETHFsWczOwhPs74155283 = -77878040;    float zOFFSpnETHFsWczOwhPs97796434 = -829252760;    float zOFFSpnETHFsWczOwhPs2257845 = -870533438;    float zOFFSpnETHFsWczOwhPs86650091 = -771246835;    float zOFFSpnETHFsWczOwhPs30231392 = -615488874;    float zOFFSpnETHFsWczOwhPs84686160 = -509015226;    float zOFFSpnETHFsWczOwhPs29897653 = -92721969;    float zOFFSpnETHFsWczOwhPs23672796 = -97579062;    float zOFFSpnETHFsWczOwhPs26562124 = -494664153;    float zOFFSpnETHFsWczOwhPs83506549 = -127982570;    float zOFFSpnETHFsWczOwhPs26286636 = -602468356;    float zOFFSpnETHFsWczOwhPs86351294 = -591617619;    float zOFFSpnETHFsWczOwhPs19208555 = -411949141;    float zOFFSpnETHFsWczOwhPs70400390 = -808970950;    float zOFFSpnETHFsWczOwhPs1634002 = -777767084;    float zOFFSpnETHFsWczOwhPs94355037 = -359233939;    float zOFFSpnETHFsWczOwhPs11496085 = -755922849;    float zOFFSpnETHFsWczOwhPs84630781 = -663702256;    float zOFFSpnETHFsWczOwhPs31245793 = -872006901;    float zOFFSpnETHFsWczOwhPs5210994 = 5793394;    float zOFFSpnETHFsWczOwhPs24202282 = -104469622;    float zOFFSpnETHFsWczOwhPs10946659 = -369749548;    float zOFFSpnETHFsWczOwhPs34463983 = -758420624;    float zOFFSpnETHFsWczOwhPs67475921 = -479261297;    float zOFFSpnETHFsWczOwhPs1176560 = -453452620;    float zOFFSpnETHFsWczOwhPs15440736 = -785552995;    float zOFFSpnETHFsWczOwhPs82735106 = 78850886;    float zOFFSpnETHFsWczOwhPs86249200 = 94000685;    float zOFFSpnETHFsWczOwhPs8794990 = -965149561;    float zOFFSpnETHFsWczOwhPs85805659 = -594191872;    float zOFFSpnETHFsWczOwhPs54598987 = 24596638;    float zOFFSpnETHFsWczOwhPs71572265 = -76565707;    float zOFFSpnETHFsWczOwhPs37111258 = -484571444;    float zOFFSpnETHFsWczOwhPs13877959 = -451276478;    float zOFFSpnETHFsWczOwhPs43134566 = -72942165;    float zOFFSpnETHFsWczOwhPs57640164 = -130282941;    float zOFFSpnETHFsWczOwhPs4820890 = -648771027;    float zOFFSpnETHFsWczOwhPs99446131 = -992697341;    float zOFFSpnETHFsWczOwhPs49983415 = -457153002;    float zOFFSpnETHFsWczOwhPs15014313 = -70656774;    float zOFFSpnETHFsWczOwhPs5983900 = -395088240;    float zOFFSpnETHFsWczOwhPs72967470 = 3906759;    float zOFFSpnETHFsWczOwhPs83877275 = -493945902;    float zOFFSpnETHFsWczOwhPs8257202 = -954960252;    float zOFFSpnETHFsWczOwhPs86153167 = -518346281;    float zOFFSpnETHFsWczOwhPs33069305 = -370997549;    float zOFFSpnETHFsWczOwhPs20853073 = -817622232;    float zOFFSpnETHFsWczOwhPs39035512 = 62669546;    float zOFFSpnETHFsWczOwhPs64789580 = -835112568;    float zOFFSpnETHFsWczOwhPs24489706 = -397070220;    float zOFFSpnETHFsWczOwhPs56684932 = -558387238;    float zOFFSpnETHFsWczOwhPs17127188 = -697311446;    float zOFFSpnETHFsWczOwhPs98269747 = -653664173;    float zOFFSpnETHFsWczOwhPs45166717 = -32947364;    float zOFFSpnETHFsWczOwhPs30509124 = -449703610;    float zOFFSpnETHFsWczOwhPs63208624 = -708128492;    float zOFFSpnETHFsWczOwhPs63332452 = 29167864;    float zOFFSpnETHFsWczOwhPs34781923 = -291272142;    float zOFFSpnETHFsWczOwhPs85473531 = -837302784;     zOFFSpnETHFsWczOwhPs57070600 = zOFFSpnETHFsWczOwhPs1433772;     zOFFSpnETHFsWczOwhPs1433772 = zOFFSpnETHFsWczOwhPs97592306;     zOFFSpnETHFsWczOwhPs97592306 = zOFFSpnETHFsWczOwhPs64845589;     zOFFSpnETHFsWczOwhPs64845589 = zOFFSpnETHFsWczOwhPs30946266;     zOFFSpnETHFsWczOwhPs30946266 = zOFFSpnETHFsWczOwhPs41092546;     zOFFSpnETHFsWczOwhPs41092546 = zOFFSpnETHFsWczOwhPs20842548;     zOFFSpnETHFsWczOwhPs20842548 = zOFFSpnETHFsWczOwhPs21386058;     zOFFSpnETHFsWczOwhPs21386058 = zOFFSpnETHFsWczOwhPs26227734;     zOFFSpnETHFsWczOwhPs26227734 = zOFFSpnETHFsWczOwhPs79915725;     zOFFSpnETHFsWczOwhPs79915725 = zOFFSpnETHFsWczOwhPs20597159;     zOFFSpnETHFsWczOwhPs20597159 = zOFFSpnETHFsWczOwhPs55502363;     zOFFSpnETHFsWczOwhPs55502363 = zOFFSpnETHFsWczOwhPs92171008;     zOFFSpnETHFsWczOwhPs92171008 = zOFFSpnETHFsWczOwhPs52672782;     zOFFSpnETHFsWczOwhPs52672782 = zOFFSpnETHFsWczOwhPs27639733;     zOFFSpnETHFsWczOwhPs27639733 = zOFFSpnETHFsWczOwhPs53986676;     zOFFSpnETHFsWczOwhPs53986676 = zOFFSpnETHFsWczOwhPs56356957;     zOFFSpnETHFsWczOwhPs56356957 = zOFFSpnETHFsWczOwhPs1087806;     zOFFSpnETHFsWczOwhPs1087806 = zOFFSpnETHFsWczOwhPs87488969;     zOFFSpnETHFsWczOwhPs87488969 = zOFFSpnETHFsWczOwhPs68589365;     zOFFSpnETHFsWczOwhPs68589365 = zOFFSpnETHFsWczOwhPs17289850;     zOFFSpnETHFsWczOwhPs17289850 = zOFFSpnETHFsWczOwhPs55436598;     zOFFSpnETHFsWczOwhPs55436598 = zOFFSpnETHFsWczOwhPs7078734;     zOFFSpnETHFsWczOwhPs7078734 = zOFFSpnETHFsWczOwhPs86096222;     zOFFSpnETHFsWczOwhPs86096222 = zOFFSpnETHFsWczOwhPs80214807;     zOFFSpnETHFsWczOwhPs80214807 = zOFFSpnETHFsWczOwhPs99700472;     zOFFSpnETHFsWczOwhPs99700472 = zOFFSpnETHFsWczOwhPs35881552;     zOFFSpnETHFsWczOwhPs35881552 = zOFFSpnETHFsWczOwhPs96640265;     zOFFSpnETHFsWczOwhPs96640265 = zOFFSpnETHFsWczOwhPs10439399;     zOFFSpnETHFsWczOwhPs10439399 = zOFFSpnETHFsWczOwhPs91763751;     zOFFSpnETHFsWczOwhPs91763751 = zOFFSpnETHFsWczOwhPs12439804;     zOFFSpnETHFsWczOwhPs12439804 = zOFFSpnETHFsWczOwhPs19420600;     zOFFSpnETHFsWczOwhPs19420600 = zOFFSpnETHFsWczOwhPs40061627;     zOFFSpnETHFsWczOwhPs40061627 = zOFFSpnETHFsWczOwhPs9435903;     zOFFSpnETHFsWczOwhPs9435903 = zOFFSpnETHFsWczOwhPs66423581;     zOFFSpnETHFsWczOwhPs66423581 = zOFFSpnETHFsWczOwhPs18844744;     zOFFSpnETHFsWczOwhPs18844744 = zOFFSpnETHFsWczOwhPs68181016;     zOFFSpnETHFsWczOwhPs68181016 = zOFFSpnETHFsWczOwhPs1757970;     zOFFSpnETHFsWczOwhPs1757970 = zOFFSpnETHFsWczOwhPs29515540;     zOFFSpnETHFsWczOwhPs29515540 = zOFFSpnETHFsWczOwhPs50377711;     zOFFSpnETHFsWczOwhPs50377711 = zOFFSpnETHFsWczOwhPs54711406;     zOFFSpnETHFsWczOwhPs54711406 = zOFFSpnETHFsWczOwhPs74155283;     zOFFSpnETHFsWczOwhPs74155283 = zOFFSpnETHFsWczOwhPs97796434;     zOFFSpnETHFsWczOwhPs97796434 = zOFFSpnETHFsWczOwhPs2257845;     zOFFSpnETHFsWczOwhPs2257845 = zOFFSpnETHFsWczOwhPs86650091;     zOFFSpnETHFsWczOwhPs86650091 = zOFFSpnETHFsWczOwhPs30231392;     zOFFSpnETHFsWczOwhPs30231392 = zOFFSpnETHFsWczOwhPs84686160;     zOFFSpnETHFsWczOwhPs84686160 = zOFFSpnETHFsWczOwhPs29897653;     zOFFSpnETHFsWczOwhPs29897653 = zOFFSpnETHFsWczOwhPs23672796;     zOFFSpnETHFsWczOwhPs23672796 = zOFFSpnETHFsWczOwhPs26562124;     zOFFSpnETHFsWczOwhPs26562124 = zOFFSpnETHFsWczOwhPs83506549;     zOFFSpnETHFsWczOwhPs83506549 = zOFFSpnETHFsWczOwhPs26286636;     zOFFSpnETHFsWczOwhPs26286636 = zOFFSpnETHFsWczOwhPs86351294;     zOFFSpnETHFsWczOwhPs86351294 = zOFFSpnETHFsWczOwhPs19208555;     zOFFSpnETHFsWczOwhPs19208555 = zOFFSpnETHFsWczOwhPs70400390;     zOFFSpnETHFsWczOwhPs70400390 = zOFFSpnETHFsWczOwhPs1634002;     zOFFSpnETHFsWczOwhPs1634002 = zOFFSpnETHFsWczOwhPs94355037;     zOFFSpnETHFsWczOwhPs94355037 = zOFFSpnETHFsWczOwhPs11496085;     zOFFSpnETHFsWczOwhPs11496085 = zOFFSpnETHFsWczOwhPs84630781;     zOFFSpnETHFsWczOwhPs84630781 = zOFFSpnETHFsWczOwhPs31245793;     zOFFSpnETHFsWczOwhPs31245793 = zOFFSpnETHFsWczOwhPs5210994;     zOFFSpnETHFsWczOwhPs5210994 = zOFFSpnETHFsWczOwhPs24202282;     zOFFSpnETHFsWczOwhPs24202282 = zOFFSpnETHFsWczOwhPs10946659;     zOFFSpnETHFsWczOwhPs10946659 = zOFFSpnETHFsWczOwhPs34463983;     zOFFSpnETHFsWczOwhPs34463983 = zOFFSpnETHFsWczOwhPs67475921;     zOFFSpnETHFsWczOwhPs67475921 = zOFFSpnETHFsWczOwhPs1176560;     zOFFSpnETHFsWczOwhPs1176560 = zOFFSpnETHFsWczOwhPs15440736;     zOFFSpnETHFsWczOwhPs15440736 = zOFFSpnETHFsWczOwhPs82735106;     zOFFSpnETHFsWczOwhPs82735106 = zOFFSpnETHFsWczOwhPs86249200;     zOFFSpnETHFsWczOwhPs86249200 = zOFFSpnETHFsWczOwhPs8794990;     zOFFSpnETHFsWczOwhPs8794990 = zOFFSpnETHFsWczOwhPs85805659;     zOFFSpnETHFsWczOwhPs85805659 = zOFFSpnETHFsWczOwhPs54598987;     zOFFSpnETHFsWczOwhPs54598987 = zOFFSpnETHFsWczOwhPs71572265;     zOFFSpnETHFsWczOwhPs71572265 = zOFFSpnETHFsWczOwhPs37111258;     zOFFSpnETHFsWczOwhPs37111258 = zOFFSpnETHFsWczOwhPs13877959;     zOFFSpnETHFsWczOwhPs13877959 = zOFFSpnETHFsWczOwhPs43134566;     zOFFSpnETHFsWczOwhPs43134566 = zOFFSpnETHFsWczOwhPs57640164;     zOFFSpnETHFsWczOwhPs57640164 = zOFFSpnETHFsWczOwhPs4820890;     zOFFSpnETHFsWczOwhPs4820890 = zOFFSpnETHFsWczOwhPs99446131;     zOFFSpnETHFsWczOwhPs99446131 = zOFFSpnETHFsWczOwhPs49983415;     zOFFSpnETHFsWczOwhPs49983415 = zOFFSpnETHFsWczOwhPs15014313;     zOFFSpnETHFsWczOwhPs15014313 = zOFFSpnETHFsWczOwhPs5983900;     zOFFSpnETHFsWczOwhPs5983900 = zOFFSpnETHFsWczOwhPs72967470;     zOFFSpnETHFsWczOwhPs72967470 = zOFFSpnETHFsWczOwhPs83877275;     zOFFSpnETHFsWczOwhPs83877275 = zOFFSpnETHFsWczOwhPs8257202;     zOFFSpnETHFsWczOwhPs8257202 = zOFFSpnETHFsWczOwhPs86153167;     zOFFSpnETHFsWczOwhPs86153167 = zOFFSpnETHFsWczOwhPs33069305;     zOFFSpnETHFsWczOwhPs33069305 = zOFFSpnETHFsWczOwhPs20853073;     zOFFSpnETHFsWczOwhPs20853073 = zOFFSpnETHFsWczOwhPs39035512;     zOFFSpnETHFsWczOwhPs39035512 = zOFFSpnETHFsWczOwhPs64789580;     zOFFSpnETHFsWczOwhPs64789580 = zOFFSpnETHFsWczOwhPs24489706;     zOFFSpnETHFsWczOwhPs24489706 = zOFFSpnETHFsWczOwhPs56684932;     zOFFSpnETHFsWczOwhPs56684932 = zOFFSpnETHFsWczOwhPs17127188;     zOFFSpnETHFsWczOwhPs17127188 = zOFFSpnETHFsWczOwhPs98269747;     zOFFSpnETHFsWczOwhPs98269747 = zOFFSpnETHFsWczOwhPs45166717;     zOFFSpnETHFsWczOwhPs45166717 = zOFFSpnETHFsWczOwhPs30509124;     zOFFSpnETHFsWczOwhPs30509124 = zOFFSpnETHFsWczOwhPs63208624;     zOFFSpnETHFsWczOwhPs63208624 = zOFFSpnETHFsWczOwhPs63332452;     zOFFSpnETHFsWczOwhPs63332452 = zOFFSpnETHFsWczOwhPs34781923;     zOFFSpnETHFsWczOwhPs34781923 = zOFFSpnETHFsWczOwhPs85473531;     zOFFSpnETHFsWczOwhPs85473531 = zOFFSpnETHFsWczOwhPs57070600;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void niAQojdNxItuDDvgeVPe42106453() {     float ONkUtzuxzEqFRBvxiLWg27722267 = -355451914;    float ONkUtzuxzEqFRBvxiLWg70107288 = -850181702;    float ONkUtzuxzEqFRBvxiLWg77561225 = -962161484;    float ONkUtzuxzEqFRBvxiLWg4776876 = 73517480;    float ONkUtzuxzEqFRBvxiLWg52396587 = -335085248;    float ONkUtzuxzEqFRBvxiLWg31092309 = -383597965;    float ONkUtzuxzEqFRBvxiLWg39221444 = -396917633;    float ONkUtzuxzEqFRBvxiLWg78228438 = -805176692;    float ONkUtzuxzEqFRBvxiLWg72092788 = 65289227;    float ONkUtzuxzEqFRBvxiLWg2882187 = 98828716;    float ONkUtzuxzEqFRBvxiLWg62846320 = -550969716;    float ONkUtzuxzEqFRBvxiLWg1048935 = -330888159;    float ONkUtzuxzEqFRBvxiLWg71998395 = -776987576;    float ONkUtzuxzEqFRBvxiLWg36519816 = -419907176;    float ONkUtzuxzEqFRBvxiLWg10359753 = -971203723;    float ONkUtzuxzEqFRBvxiLWg86515316 = -578485545;    float ONkUtzuxzEqFRBvxiLWg50209784 = -345602194;    float ONkUtzuxzEqFRBvxiLWg21381300 = -691402975;    float ONkUtzuxzEqFRBvxiLWg69196751 = -318913355;    float ONkUtzuxzEqFRBvxiLWg27376075 = -647816014;    float ONkUtzuxzEqFRBvxiLWg72425603 = -159499345;    float ONkUtzuxzEqFRBvxiLWg16832363 = -290821006;    float ONkUtzuxzEqFRBvxiLWg9022543 = 12852391;    float ONkUtzuxzEqFRBvxiLWg19945759 = -356345082;    float ONkUtzuxzEqFRBvxiLWg44339196 = -848546563;    float ONkUtzuxzEqFRBvxiLWg30298054 = -777956445;    float ONkUtzuxzEqFRBvxiLWg44142338 = -982987983;    float ONkUtzuxzEqFRBvxiLWg4422492 = -698796070;    float ONkUtzuxzEqFRBvxiLWg42581887 = -282968272;    float ONkUtzuxzEqFRBvxiLWg30935847 = -641337892;    float ONkUtzuxzEqFRBvxiLWg19511801 = -969248323;    float ONkUtzuxzEqFRBvxiLWg63932060 = -290912330;    float ONkUtzuxzEqFRBvxiLWg23006988 = -176532980;    float ONkUtzuxzEqFRBvxiLWg74800877 = -409831088;    float ONkUtzuxzEqFRBvxiLWg44447818 = -459826431;    float ONkUtzuxzEqFRBvxiLWg78838002 = -935530706;    float ONkUtzuxzEqFRBvxiLWg11972503 = -502903293;    float ONkUtzuxzEqFRBvxiLWg54550458 = -145126810;    float ONkUtzuxzEqFRBvxiLWg8048829 = -481399710;    float ONkUtzuxzEqFRBvxiLWg45481010 = -744762332;    float ONkUtzuxzEqFRBvxiLWg14784691 = 61807890;    float ONkUtzuxzEqFRBvxiLWg2638275 = -947267109;    float ONkUtzuxzEqFRBvxiLWg46763394 = -816006812;    float ONkUtzuxzEqFRBvxiLWg8766145 = -942508410;    float ONkUtzuxzEqFRBvxiLWg82604149 = 20106770;    float ONkUtzuxzEqFRBvxiLWg36637227 = -373826029;    float ONkUtzuxzEqFRBvxiLWg10817364 = -625085026;    float ONkUtzuxzEqFRBvxiLWg95511862 = -544852264;    float ONkUtzuxzEqFRBvxiLWg29114249 = -83343031;    float ONkUtzuxzEqFRBvxiLWg34989610 = -777673031;    float ONkUtzuxzEqFRBvxiLWg51695874 = -774554691;    float ONkUtzuxzEqFRBvxiLWg68158140 = -571688078;    float ONkUtzuxzEqFRBvxiLWg41426951 = -519415661;    float ONkUtzuxzEqFRBvxiLWg9435860 = 27169363;    float ONkUtzuxzEqFRBvxiLWg69680849 = -919911865;    float ONkUtzuxzEqFRBvxiLWg10889904 = 35369091;    float ONkUtzuxzEqFRBvxiLWg61084746 = -763034094;    float ONkUtzuxzEqFRBvxiLWg57615466 = -505816402;    float ONkUtzuxzEqFRBvxiLWg60437680 = -77935958;    float ONkUtzuxzEqFRBvxiLWg22098534 = -557128803;    float ONkUtzuxzEqFRBvxiLWg86949971 = -400609982;    float ONkUtzuxzEqFRBvxiLWg34798953 = -698121564;    float ONkUtzuxzEqFRBvxiLWg35646552 = -422208421;    float ONkUtzuxzEqFRBvxiLWg41156941 = -293372882;    float ONkUtzuxzEqFRBvxiLWg83370385 = 68077038;    float ONkUtzuxzEqFRBvxiLWg98914259 = -160057386;    float ONkUtzuxzEqFRBvxiLWg78041946 = -54355179;    float ONkUtzuxzEqFRBvxiLWg97197518 = -267156489;    float ONkUtzuxzEqFRBvxiLWg92071997 = -960080746;    float ONkUtzuxzEqFRBvxiLWg31521750 = 64326983;    float ONkUtzuxzEqFRBvxiLWg74542814 = 24417748;    float ONkUtzuxzEqFRBvxiLWg95659325 = -100475385;    float ONkUtzuxzEqFRBvxiLWg13332471 = -110003266;    float ONkUtzuxzEqFRBvxiLWg23715742 = -574151023;    float ONkUtzuxzEqFRBvxiLWg12591384 = -609623905;    float ONkUtzuxzEqFRBvxiLWg69787329 = -212232236;    float ONkUtzuxzEqFRBvxiLWg70068969 = -474814195;    float ONkUtzuxzEqFRBvxiLWg256398 = -44639200;    float ONkUtzuxzEqFRBvxiLWg37341610 = -276451852;    float ONkUtzuxzEqFRBvxiLWg7701969 = -374720535;    float ONkUtzuxzEqFRBvxiLWg19480690 = -52871420;    float ONkUtzuxzEqFRBvxiLWg48630475 = -338135720;    float ONkUtzuxzEqFRBvxiLWg75308243 = -515453039;    float ONkUtzuxzEqFRBvxiLWg7592277 = -505295241;    float ONkUtzuxzEqFRBvxiLWg79239973 = -866783202;    float ONkUtzuxzEqFRBvxiLWg51353660 = -297560245;    float ONkUtzuxzEqFRBvxiLWg22505109 = -771496670;    float ONkUtzuxzEqFRBvxiLWg13571129 = -103702344;    float ONkUtzuxzEqFRBvxiLWg5120028 = -489919224;    float ONkUtzuxzEqFRBvxiLWg33557914 = -395195523;    float ONkUtzuxzEqFRBvxiLWg17753256 = -72496613;    float ONkUtzuxzEqFRBvxiLWg54357036 = -997086891;    float ONkUtzuxzEqFRBvxiLWg94112778 = 32809147;    float ONkUtzuxzEqFRBvxiLWg85950295 = -924270907;    float ONkUtzuxzEqFRBvxiLWg58531039 = -244152350;    float ONkUtzuxzEqFRBvxiLWg79985738 = -240070546;    float ONkUtzuxzEqFRBvxiLWg66991722 = -425058689;    float ONkUtzuxzEqFRBvxiLWg5606454 = -422633930;    float ONkUtzuxzEqFRBvxiLWg25395759 = -910585449;    float ONkUtzuxzEqFRBvxiLWg83689890 = -355451914;     ONkUtzuxzEqFRBvxiLWg27722267 = ONkUtzuxzEqFRBvxiLWg70107288;     ONkUtzuxzEqFRBvxiLWg70107288 = ONkUtzuxzEqFRBvxiLWg77561225;     ONkUtzuxzEqFRBvxiLWg77561225 = ONkUtzuxzEqFRBvxiLWg4776876;     ONkUtzuxzEqFRBvxiLWg4776876 = ONkUtzuxzEqFRBvxiLWg52396587;     ONkUtzuxzEqFRBvxiLWg52396587 = ONkUtzuxzEqFRBvxiLWg31092309;     ONkUtzuxzEqFRBvxiLWg31092309 = ONkUtzuxzEqFRBvxiLWg39221444;     ONkUtzuxzEqFRBvxiLWg39221444 = ONkUtzuxzEqFRBvxiLWg78228438;     ONkUtzuxzEqFRBvxiLWg78228438 = ONkUtzuxzEqFRBvxiLWg72092788;     ONkUtzuxzEqFRBvxiLWg72092788 = ONkUtzuxzEqFRBvxiLWg2882187;     ONkUtzuxzEqFRBvxiLWg2882187 = ONkUtzuxzEqFRBvxiLWg62846320;     ONkUtzuxzEqFRBvxiLWg62846320 = ONkUtzuxzEqFRBvxiLWg1048935;     ONkUtzuxzEqFRBvxiLWg1048935 = ONkUtzuxzEqFRBvxiLWg71998395;     ONkUtzuxzEqFRBvxiLWg71998395 = ONkUtzuxzEqFRBvxiLWg36519816;     ONkUtzuxzEqFRBvxiLWg36519816 = ONkUtzuxzEqFRBvxiLWg10359753;     ONkUtzuxzEqFRBvxiLWg10359753 = ONkUtzuxzEqFRBvxiLWg86515316;     ONkUtzuxzEqFRBvxiLWg86515316 = ONkUtzuxzEqFRBvxiLWg50209784;     ONkUtzuxzEqFRBvxiLWg50209784 = ONkUtzuxzEqFRBvxiLWg21381300;     ONkUtzuxzEqFRBvxiLWg21381300 = ONkUtzuxzEqFRBvxiLWg69196751;     ONkUtzuxzEqFRBvxiLWg69196751 = ONkUtzuxzEqFRBvxiLWg27376075;     ONkUtzuxzEqFRBvxiLWg27376075 = ONkUtzuxzEqFRBvxiLWg72425603;     ONkUtzuxzEqFRBvxiLWg72425603 = ONkUtzuxzEqFRBvxiLWg16832363;     ONkUtzuxzEqFRBvxiLWg16832363 = ONkUtzuxzEqFRBvxiLWg9022543;     ONkUtzuxzEqFRBvxiLWg9022543 = ONkUtzuxzEqFRBvxiLWg19945759;     ONkUtzuxzEqFRBvxiLWg19945759 = ONkUtzuxzEqFRBvxiLWg44339196;     ONkUtzuxzEqFRBvxiLWg44339196 = ONkUtzuxzEqFRBvxiLWg30298054;     ONkUtzuxzEqFRBvxiLWg30298054 = ONkUtzuxzEqFRBvxiLWg44142338;     ONkUtzuxzEqFRBvxiLWg44142338 = ONkUtzuxzEqFRBvxiLWg4422492;     ONkUtzuxzEqFRBvxiLWg4422492 = ONkUtzuxzEqFRBvxiLWg42581887;     ONkUtzuxzEqFRBvxiLWg42581887 = ONkUtzuxzEqFRBvxiLWg30935847;     ONkUtzuxzEqFRBvxiLWg30935847 = ONkUtzuxzEqFRBvxiLWg19511801;     ONkUtzuxzEqFRBvxiLWg19511801 = ONkUtzuxzEqFRBvxiLWg63932060;     ONkUtzuxzEqFRBvxiLWg63932060 = ONkUtzuxzEqFRBvxiLWg23006988;     ONkUtzuxzEqFRBvxiLWg23006988 = ONkUtzuxzEqFRBvxiLWg74800877;     ONkUtzuxzEqFRBvxiLWg74800877 = ONkUtzuxzEqFRBvxiLWg44447818;     ONkUtzuxzEqFRBvxiLWg44447818 = ONkUtzuxzEqFRBvxiLWg78838002;     ONkUtzuxzEqFRBvxiLWg78838002 = ONkUtzuxzEqFRBvxiLWg11972503;     ONkUtzuxzEqFRBvxiLWg11972503 = ONkUtzuxzEqFRBvxiLWg54550458;     ONkUtzuxzEqFRBvxiLWg54550458 = ONkUtzuxzEqFRBvxiLWg8048829;     ONkUtzuxzEqFRBvxiLWg8048829 = ONkUtzuxzEqFRBvxiLWg45481010;     ONkUtzuxzEqFRBvxiLWg45481010 = ONkUtzuxzEqFRBvxiLWg14784691;     ONkUtzuxzEqFRBvxiLWg14784691 = ONkUtzuxzEqFRBvxiLWg2638275;     ONkUtzuxzEqFRBvxiLWg2638275 = ONkUtzuxzEqFRBvxiLWg46763394;     ONkUtzuxzEqFRBvxiLWg46763394 = ONkUtzuxzEqFRBvxiLWg8766145;     ONkUtzuxzEqFRBvxiLWg8766145 = ONkUtzuxzEqFRBvxiLWg82604149;     ONkUtzuxzEqFRBvxiLWg82604149 = ONkUtzuxzEqFRBvxiLWg36637227;     ONkUtzuxzEqFRBvxiLWg36637227 = ONkUtzuxzEqFRBvxiLWg10817364;     ONkUtzuxzEqFRBvxiLWg10817364 = ONkUtzuxzEqFRBvxiLWg95511862;     ONkUtzuxzEqFRBvxiLWg95511862 = ONkUtzuxzEqFRBvxiLWg29114249;     ONkUtzuxzEqFRBvxiLWg29114249 = ONkUtzuxzEqFRBvxiLWg34989610;     ONkUtzuxzEqFRBvxiLWg34989610 = ONkUtzuxzEqFRBvxiLWg51695874;     ONkUtzuxzEqFRBvxiLWg51695874 = ONkUtzuxzEqFRBvxiLWg68158140;     ONkUtzuxzEqFRBvxiLWg68158140 = ONkUtzuxzEqFRBvxiLWg41426951;     ONkUtzuxzEqFRBvxiLWg41426951 = ONkUtzuxzEqFRBvxiLWg9435860;     ONkUtzuxzEqFRBvxiLWg9435860 = ONkUtzuxzEqFRBvxiLWg69680849;     ONkUtzuxzEqFRBvxiLWg69680849 = ONkUtzuxzEqFRBvxiLWg10889904;     ONkUtzuxzEqFRBvxiLWg10889904 = ONkUtzuxzEqFRBvxiLWg61084746;     ONkUtzuxzEqFRBvxiLWg61084746 = ONkUtzuxzEqFRBvxiLWg57615466;     ONkUtzuxzEqFRBvxiLWg57615466 = ONkUtzuxzEqFRBvxiLWg60437680;     ONkUtzuxzEqFRBvxiLWg60437680 = ONkUtzuxzEqFRBvxiLWg22098534;     ONkUtzuxzEqFRBvxiLWg22098534 = ONkUtzuxzEqFRBvxiLWg86949971;     ONkUtzuxzEqFRBvxiLWg86949971 = ONkUtzuxzEqFRBvxiLWg34798953;     ONkUtzuxzEqFRBvxiLWg34798953 = ONkUtzuxzEqFRBvxiLWg35646552;     ONkUtzuxzEqFRBvxiLWg35646552 = ONkUtzuxzEqFRBvxiLWg41156941;     ONkUtzuxzEqFRBvxiLWg41156941 = ONkUtzuxzEqFRBvxiLWg83370385;     ONkUtzuxzEqFRBvxiLWg83370385 = ONkUtzuxzEqFRBvxiLWg98914259;     ONkUtzuxzEqFRBvxiLWg98914259 = ONkUtzuxzEqFRBvxiLWg78041946;     ONkUtzuxzEqFRBvxiLWg78041946 = ONkUtzuxzEqFRBvxiLWg97197518;     ONkUtzuxzEqFRBvxiLWg97197518 = ONkUtzuxzEqFRBvxiLWg92071997;     ONkUtzuxzEqFRBvxiLWg92071997 = ONkUtzuxzEqFRBvxiLWg31521750;     ONkUtzuxzEqFRBvxiLWg31521750 = ONkUtzuxzEqFRBvxiLWg74542814;     ONkUtzuxzEqFRBvxiLWg74542814 = ONkUtzuxzEqFRBvxiLWg95659325;     ONkUtzuxzEqFRBvxiLWg95659325 = ONkUtzuxzEqFRBvxiLWg13332471;     ONkUtzuxzEqFRBvxiLWg13332471 = ONkUtzuxzEqFRBvxiLWg23715742;     ONkUtzuxzEqFRBvxiLWg23715742 = ONkUtzuxzEqFRBvxiLWg12591384;     ONkUtzuxzEqFRBvxiLWg12591384 = ONkUtzuxzEqFRBvxiLWg69787329;     ONkUtzuxzEqFRBvxiLWg69787329 = ONkUtzuxzEqFRBvxiLWg70068969;     ONkUtzuxzEqFRBvxiLWg70068969 = ONkUtzuxzEqFRBvxiLWg256398;     ONkUtzuxzEqFRBvxiLWg256398 = ONkUtzuxzEqFRBvxiLWg37341610;     ONkUtzuxzEqFRBvxiLWg37341610 = ONkUtzuxzEqFRBvxiLWg7701969;     ONkUtzuxzEqFRBvxiLWg7701969 = ONkUtzuxzEqFRBvxiLWg19480690;     ONkUtzuxzEqFRBvxiLWg19480690 = ONkUtzuxzEqFRBvxiLWg48630475;     ONkUtzuxzEqFRBvxiLWg48630475 = ONkUtzuxzEqFRBvxiLWg75308243;     ONkUtzuxzEqFRBvxiLWg75308243 = ONkUtzuxzEqFRBvxiLWg7592277;     ONkUtzuxzEqFRBvxiLWg7592277 = ONkUtzuxzEqFRBvxiLWg79239973;     ONkUtzuxzEqFRBvxiLWg79239973 = ONkUtzuxzEqFRBvxiLWg51353660;     ONkUtzuxzEqFRBvxiLWg51353660 = ONkUtzuxzEqFRBvxiLWg22505109;     ONkUtzuxzEqFRBvxiLWg22505109 = ONkUtzuxzEqFRBvxiLWg13571129;     ONkUtzuxzEqFRBvxiLWg13571129 = ONkUtzuxzEqFRBvxiLWg5120028;     ONkUtzuxzEqFRBvxiLWg5120028 = ONkUtzuxzEqFRBvxiLWg33557914;     ONkUtzuxzEqFRBvxiLWg33557914 = ONkUtzuxzEqFRBvxiLWg17753256;     ONkUtzuxzEqFRBvxiLWg17753256 = ONkUtzuxzEqFRBvxiLWg54357036;     ONkUtzuxzEqFRBvxiLWg54357036 = ONkUtzuxzEqFRBvxiLWg94112778;     ONkUtzuxzEqFRBvxiLWg94112778 = ONkUtzuxzEqFRBvxiLWg85950295;     ONkUtzuxzEqFRBvxiLWg85950295 = ONkUtzuxzEqFRBvxiLWg58531039;     ONkUtzuxzEqFRBvxiLWg58531039 = ONkUtzuxzEqFRBvxiLWg79985738;     ONkUtzuxzEqFRBvxiLWg79985738 = ONkUtzuxzEqFRBvxiLWg66991722;     ONkUtzuxzEqFRBvxiLWg66991722 = ONkUtzuxzEqFRBvxiLWg5606454;     ONkUtzuxzEqFRBvxiLWg5606454 = ONkUtzuxzEqFRBvxiLWg25395759;     ONkUtzuxzEqFRBvxiLWg25395759 = ONkUtzuxzEqFRBvxiLWg83689890;     ONkUtzuxzEqFRBvxiLWg83689890 = ONkUtzuxzEqFRBvxiLWg27722267;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void suFnIoaFpmzzlcqFPQOo48765198() {     float ydjQeaYMBTNJrhOYWVdA27715795 = -227976056;    float ydjQeaYMBTNJrhOYWVdA8679606 = -892776828;    float ydjQeaYMBTNJrhOYWVdA78874437 = -919290097;    float ydjQeaYMBTNJrhOYWVdA89856736 = -847023350;    float ydjQeaYMBTNJrhOYWVdA61236689 = -803440569;    float ydjQeaYMBTNJrhOYWVdA65897741 = -83030776;    float ydjQeaYMBTNJrhOYWVdA7768780 = -101454100;    float ydjQeaYMBTNJrhOYWVdA47442489 = -509354009;    float ydjQeaYMBTNJrhOYWVdA54832952 = -748356843;    float ydjQeaYMBTNJrhOYWVdA99521699 = -637978155;    float ydjQeaYMBTNJrhOYWVdA83166925 = -510441788;    float ydjQeaYMBTNJrhOYWVdA15340612 = -66366521;    float ydjQeaYMBTNJrhOYWVdA3851310 = -798646050;    float ydjQeaYMBTNJrhOYWVdA90364035 = -765060228;    float ydjQeaYMBTNJrhOYWVdA55531861 = -933916574;    float ydjQeaYMBTNJrhOYWVdA61079864 = -911281195;    float ydjQeaYMBTNJrhOYWVdA6062543 = -165216764;    float ydjQeaYMBTNJrhOYWVdA44381833 = 11525403;    float ydjQeaYMBTNJrhOYWVdA89625020 = -936895621;    float ydjQeaYMBTNJrhOYWVdA99449837 = -159840465;    float ydjQeaYMBTNJrhOYWVdA52095028 = -969965719;    float ydjQeaYMBTNJrhOYWVdA95128628 = -800812889;    float ydjQeaYMBTNJrhOYWVdA45900939 = -446105451;    float ydjQeaYMBTNJrhOYWVdA63595867 = -237265604;    float ydjQeaYMBTNJrhOYWVdA72781885 = -175240416;    float ydjQeaYMBTNJrhOYWVdA46872131 = -686162373;    float ydjQeaYMBTNJrhOYWVdA14363920 = -183787696;    float ydjQeaYMBTNJrhOYWVdA39826016 = -207763058;    float ydjQeaYMBTNJrhOYWVdA55260333 = -929038587;    float ydjQeaYMBTNJrhOYWVdA7025718 = -316769266;    float ydjQeaYMBTNJrhOYWVdA52924479 = -256341557;    float ydjQeaYMBTNJrhOYWVdA48655918 = -682734091;    float ydjQeaYMBTNJrhOYWVdA8459412 = -378477196;    float ydjQeaYMBTNJrhOYWVdA75083714 = -286512820;    float ydjQeaYMBTNJrhOYWVdA26805856 = -719140356;    float ydjQeaYMBTNJrhOYWVdA21666192 = 84703547;    float ydjQeaYMBTNJrhOYWVdA23913871 = -602556672;    float ydjQeaYMBTNJrhOYWVdA24468138 = -140389489;    float ydjQeaYMBTNJrhOYWVdA43362426 = -94081126;    float ydjQeaYMBTNJrhOYWVdA17353903 = -354194018;    float ydjQeaYMBTNJrhOYWVdA2640358 = -26136008;    float ydjQeaYMBTNJrhOYWVdA88335935 = -41059192;    float ydjQeaYMBTNJrhOYWVdA7046755 = -275137066;    float ydjQeaYMBTNJrhOYWVdA67998043 = -499472853;    float ydjQeaYMBTNJrhOYWVdA66622202 = -453152631;    float ydjQeaYMBTNJrhOYWVdA77065195 = -185870699;    float ydjQeaYMBTNJrhOYWVdA38515156 = -6365648;    float ydjQeaYMBTNJrhOYWVdA70895444 = -53613620;    float ydjQeaYMBTNJrhOYWVdA47772638 = -747106892;    float ydjQeaYMBTNJrhOYWVdA55035033 = -364697918;    float ydjQeaYMBTNJrhOYWVdA64955244 = -353109698;    float ydjQeaYMBTNJrhOYWVdA82705398 = -953833827;    float ydjQeaYMBTNJrhOYWVdA16875300 = -411469669;    float ydjQeaYMBTNJrhOYWVdA19686130 = -504750488;    float ydjQeaYMBTNJrhOYWVdA96670169 = -676811142;    float ydjQeaYMBTNJrhOYWVdA32587167 = -427163167;    float ydjQeaYMBTNJrhOYWVdA62778666 = -346671377;    float ydjQeaYMBTNJrhOYWVdA15278570 = -582024494;    float ydjQeaYMBTNJrhOYWVdA17074851 = -571782935;    float ydjQeaYMBTNJrhOYWVdA14364559 = -17278196;    float ydjQeaYMBTNJrhOYWVdA51533822 = -899243081;    float ydjQeaYMBTNJrhOYWVdA67942763 = -893691043;    float ydjQeaYMBTNJrhOYWVdA92182155 = -580315423;    float ydjQeaYMBTNJrhOYWVdA47807235 = -331587578;    float ydjQeaYMBTNJrhOYWVdA46597221 = -281636598;    float ydjQeaYMBTNJrhOYWVdA34511008 = -827707697;    float ydjQeaYMBTNJrhOYWVdA6881201 = -687889325;    float ydjQeaYMBTNJrhOYWVdA28767595 = -412133230;    float ydjQeaYMBTNJrhOYWVdA63558180 = 54080128;    float ydjQeaYMBTNJrhOYWVdA33865670 = -918620122;    float ydjQeaYMBTNJrhOYWVdA37165994 = -208724524;    float ydjQeaYMBTNJrhOYWVdA81594405 = 75172724;    float ydjQeaYMBTNJrhOYWVdA1019408 = -894393472;    float ydjQeaYMBTNJrhOYWVdA72271118 = -482701604;    float ydjQeaYMBTNJrhOYWVdA96809479 = -33704457;    float ydjQeaYMBTNJrhOYWVdA63759093 = -828906527;    float ydjQeaYMBTNJrhOYWVdA88081874 = -425675824;    float ydjQeaYMBTNJrhOYWVdA77902895 = -946632599;    float ydjQeaYMBTNJrhOYWVdA96973665 = -784112973;    float ydjQeaYMBTNJrhOYWVdA95716689 = -989369717;    float ydjQeaYMBTNJrhOYWVdA8356975 = -579796726;    float ydjQeaYMBTNJrhOYWVdA43468476 = -30174076;    float ydjQeaYMBTNJrhOYWVdA92053378 = -460656166;    float ydjQeaYMBTNJrhOYWVdA225301 = -464340669;    float ydjQeaYMBTNJrhOYWVdA42070473 = -963659568;    float ydjQeaYMBTNJrhOYWVdA70219081 = -302507731;    float ydjQeaYMBTNJrhOYWVdA31780618 = -171264423;    float ydjQeaYMBTNJrhOYWVdA88773281 = -873726708;    float ydjQeaYMBTNJrhOYWVdA78413545 = -609701679;    float ydjQeaYMBTNJrhOYWVdA94218689 = -191977189;    float ydjQeaYMBTNJrhOYWVdA58887525 = -568625076;    float ydjQeaYMBTNJrhOYWVdA8635301 = 79467821;    float ydjQeaYMBTNJrhOYWVdA7393287 = -568606555;    float ydjQeaYMBTNJrhOYWVdA28997867 = 23197070;    float ydjQeaYMBTNJrhOYWVdA65820081 = -454950937;    float ydjQeaYMBTNJrhOYWVdA34697595 = -132444966;    float ydjQeaYMBTNJrhOYWVdA96153779 = -460743769;    float ydjQeaYMBTNJrhOYWVdA59239519 = -943549488;    float ydjQeaYMBTNJrhOYWVdA21400823 = -117836255;    float ydjQeaYMBTNJrhOYWVdA32111195 = -227976056;     ydjQeaYMBTNJrhOYWVdA27715795 = ydjQeaYMBTNJrhOYWVdA8679606;     ydjQeaYMBTNJrhOYWVdA8679606 = ydjQeaYMBTNJrhOYWVdA78874437;     ydjQeaYMBTNJrhOYWVdA78874437 = ydjQeaYMBTNJrhOYWVdA89856736;     ydjQeaYMBTNJrhOYWVdA89856736 = ydjQeaYMBTNJrhOYWVdA61236689;     ydjQeaYMBTNJrhOYWVdA61236689 = ydjQeaYMBTNJrhOYWVdA65897741;     ydjQeaYMBTNJrhOYWVdA65897741 = ydjQeaYMBTNJrhOYWVdA7768780;     ydjQeaYMBTNJrhOYWVdA7768780 = ydjQeaYMBTNJrhOYWVdA47442489;     ydjQeaYMBTNJrhOYWVdA47442489 = ydjQeaYMBTNJrhOYWVdA54832952;     ydjQeaYMBTNJrhOYWVdA54832952 = ydjQeaYMBTNJrhOYWVdA99521699;     ydjQeaYMBTNJrhOYWVdA99521699 = ydjQeaYMBTNJrhOYWVdA83166925;     ydjQeaYMBTNJrhOYWVdA83166925 = ydjQeaYMBTNJrhOYWVdA15340612;     ydjQeaYMBTNJrhOYWVdA15340612 = ydjQeaYMBTNJrhOYWVdA3851310;     ydjQeaYMBTNJrhOYWVdA3851310 = ydjQeaYMBTNJrhOYWVdA90364035;     ydjQeaYMBTNJrhOYWVdA90364035 = ydjQeaYMBTNJrhOYWVdA55531861;     ydjQeaYMBTNJrhOYWVdA55531861 = ydjQeaYMBTNJrhOYWVdA61079864;     ydjQeaYMBTNJrhOYWVdA61079864 = ydjQeaYMBTNJrhOYWVdA6062543;     ydjQeaYMBTNJrhOYWVdA6062543 = ydjQeaYMBTNJrhOYWVdA44381833;     ydjQeaYMBTNJrhOYWVdA44381833 = ydjQeaYMBTNJrhOYWVdA89625020;     ydjQeaYMBTNJrhOYWVdA89625020 = ydjQeaYMBTNJrhOYWVdA99449837;     ydjQeaYMBTNJrhOYWVdA99449837 = ydjQeaYMBTNJrhOYWVdA52095028;     ydjQeaYMBTNJrhOYWVdA52095028 = ydjQeaYMBTNJrhOYWVdA95128628;     ydjQeaYMBTNJrhOYWVdA95128628 = ydjQeaYMBTNJrhOYWVdA45900939;     ydjQeaYMBTNJrhOYWVdA45900939 = ydjQeaYMBTNJrhOYWVdA63595867;     ydjQeaYMBTNJrhOYWVdA63595867 = ydjQeaYMBTNJrhOYWVdA72781885;     ydjQeaYMBTNJrhOYWVdA72781885 = ydjQeaYMBTNJrhOYWVdA46872131;     ydjQeaYMBTNJrhOYWVdA46872131 = ydjQeaYMBTNJrhOYWVdA14363920;     ydjQeaYMBTNJrhOYWVdA14363920 = ydjQeaYMBTNJrhOYWVdA39826016;     ydjQeaYMBTNJrhOYWVdA39826016 = ydjQeaYMBTNJrhOYWVdA55260333;     ydjQeaYMBTNJrhOYWVdA55260333 = ydjQeaYMBTNJrhOYWVdA7025718;     ydjQeaYMBTNJrhOYWVdA7025718 = ydjQeaYMBTNJrhOYWVdA52924479;     ydjQeaYMBTNJrhOYWVdA52924479 = ydjQeaYMBTNJrhOYWVdA48655918;     ydjQeaYMBTNJrhOYWVdA48655918 = ydjQeaYMBTNJrhOYWVdA8459412;     ydjQeaYMBTNJrhOYWVdA8459412 = ydjQeaYMBTNJrhOYWVdA75083714;     ydjQeaYMBTNJrhOYWVdA75083714 = ydjQeaYMBTNJrhOYWVdA26805856;     ydjQeaYMBTNJrhOYWVdA26805856 = ydjQeaYMBTNJrhOYWVdA21666192;     ydjQeaYMBTNJrhOYWVdA21666192 = ydjQeaYMBTNJrhOYWVdA23913871;     ydjQeaYMBTNJrhOYWVdA23913871 = ydjQeaYMBTNJrhOYWVdA24468138;     ydjQeaYMBTNJrhOYWVdA24468138 = ydjQeaYMBTNJrhOYWVdA43362426;     ydjQeaYMBTNJrhOYWVdA43362426 = ydjQeaYMBTNJrhOYWVdA17353903;     ydjQeaYMBTNJrhOYWVdA17353903 = ydjQeaYMBTNJrhOYWVdA2640358;     ydjQeaYMBTNJrhOYWVdA2640358 = ydjQeaYMBTNJrhOYWVdA88335935;     ydjQeaYMBTNJrhOYWVdA88335935 = ydjQeaYMBTNJrhOYWVdA7046755;     ydjQeaYMBTNJrhOYWVdA7046755 = ydjQeaYMBTNJrhOYWVdA67998043;     ydjQeaYMBTNJrhOYWVdA67998043 = ydjQeaYMBTNJrhOYWVdA66622202;     ydjQeaYMBTNJrhOYWVdA66622202 = ydjQeaYMBTNJrhOYWVdA77065195;     ydjQeaYMBTNJrhOYWVdA77065195 = ydjQeaYMBTNJrhOYWVdA38515156;     ydjQeaYMBTNJrhOYWVdA38515156 = ydjQeaYMBTNJrhOYWVdA70895444;     ydjQeaYMBTNJrhOYWVdA70895444 = ydjQeaYMBTNJrhOYWVdA47772638;     ydjQeaYMBTNJrhOYWVdA47772638 = ydjQeaYMBTNJrhOYWVdA55035033;     ydjQeaYMBTNJrhOYWVdA55035033 = ydjQeaYMBTNJrhOYWVdA64955244;     ydjQeaYMBTNJrhOYWVdA64955244 = ydjQeaYMBTNJrhOYWVdA82705398;     ydjQeaYMBTNJrhOYWVdA82705398 = ydjQeaYMBTNJrhOYWVdA16875300;     ydjQeaYMBTNJrhOYWVdA16875300 = ydjQeaYMBTNJrhOYWVdA19686130;     ydjQeaYMBTNJrhOYWVdA19686130 = ydjQeaYMBTNJrhOYWVdA96670169;     ydjQeaYMBTNJrhOYWVdA96670169 = ydjQeaYMBTNJrhOYWVdA32587167;     ydjQeaYMBTNJrhOYWVdA32587167 = ydjQeaYMBTNJrhOYWVdA62778666;     ydjQeaYMBTNJrhOYWVdA62778666 = ydjQeaYMBTNJrhOYWVdA15278570;     ydjQeaYMBTNJrhOYWVdA15278570 = ydjQeaYMBTNJrhOYWVdA17074851;     ydjQeaYMBTNJrhOYWVdA17074851 = ydjQeaYMBTNJrhOYWVdA14364559;     ydjQeaYMBTNJrhOYWVdA14364559 = ydjQeaYMBTNJrhOYWVdA51533822;     ydjQeaYMBTNJrhOYWVdA51533822 = ydjQeaYMBTNJrhOYWVdA67942763;     ydjQeaYMBTNJrhOYWVdA67942763 = ydjQeaYMBTNJrhOYWVdA92182155;     ydjQeaYMBTNJrhOYWVdA92182155 = ydjQeaYMBTNJrhOYWVdA47807235;     ydjQeaYMBTNJrhOYWVdA47807235 = ydjQeaYMBTNJrhOYWVdA46597221;     ydjQeaYMBTNJrhOYWVdA46597221 = ydjQeaYMBTNJrhOYWVdA34511008;     ydjQeaYMBTNJrhOYWVdA34511008 = ydjQeaYMBTNJrhOYWVdA6881201;     ydjQeaYMBTNJrhOYWVdA6881201 = ydjQeaYMBTNJrhOYWVdA28767595;     ydjQeaYMBTNJrhOYWVdA28767595 = ydjQeaYMBTNJrhOYWVdA63558180;     ydjQeaYMBTNJrhOYWVdA63558180 = ydjQeaYMBTNJrhOYWVdA33865670;     ydjQeaYMBTNJrhOYWVdA33865670 = ydjQeaYMBTNJrhOYWVdA37165994;     ydjQeaYMBTNJrhOYWVdA37165994 = ydjQeaYMBTNJrhOYWVdA81594405;     ydjQeaYMBTNJrhOYWVdA81594405 = ydjQeaYMBTNJrhOYWVdA1019408;     ydjQeaYMBTNJrhOYWVdA1019408 = ydjQeaYMBTNJrhOYWVdA72271118;     ydjQeaYMBTNJrhOYWVdA72271118 = ydjQeaYMBTNJrhOYWVdA96809479;     ydjQeaYMBTNJrhOYWVdA96809479 = ydjQeaYMBTNJrhOYWVdA63759093;     ydjQeaYMBTNJrhOYWVdA63759093 = ydjQeaYMBTNJrhOYWVdA88081874;     ydjQeaYMBTNJrhOYWVdA88081874 = ydjQeaYMBTNJrhOYWVdA77902895;     ydjQeaYMBTNJrhOYWVdA77902895 = ydjQeaYMBTNJrhOYWVdA96973665;     ydjQeaYMBTNJrhOYWVdA96973665 = ydjQeaYMBTNJrhOYWVdA95716689;     ydjQeaYMBTNJrhOYWVdA95716689 = ydjQeaYMBTNJrhOYWVdA8356975;     ydjQeaYMBTNJrhOYWVdA8356975 = ydjQeaYMBTNJrhOYWVdA43468476;     ydjQeaYMBTNJrhOYWVdA43468476 = ydjQeaYMBTNJrhOYWVdA92053378;     ydjQeaYMBTNJrhOYWVdA92053378 = ydjQeaYMBTNJrhOYWVdA225301;     ydjQeaYMBTNJrhOYWVdA225301 = ydjQeaYMBTNJrhOYWVdA42070473;     ydjQeaYMBTNJrhOYWVdA42070473 = ydjQeaYMBTNJrhOYWVdA70219081;     ydjQeaYMBTNJrhOYWVdA70219081 = ydjQeaYMBTNJrhOYWVdA31780618;     ydjQeaYMBTNJrhOYWVdA31780618 = ydjQeaYMBTNJrhOYWVdA88773281;     ydjQeaYMBTNJrhOYWVdA88773281 = ydjQeaYMBTNJrhOYWVdA78413545;     ydjQeaYMBTNJrhOYWVdA78413545 = ydjQeaYMBTNJrhOYWVdA94218689;     ydjQeaYMBTNJrhOYWVdA94218689 = ydjQeaYMBTNJrhOYWVdA58887525;     ydjQeaYMBTNJrhOYWVdA58887525 = ydjQeaYMBTNJrhOYWVdA8635301;     ydjQeaYMBTNJrhOYWVdA8635301 = ydjQeaYMBTNJrhOYWVdA7393287;     ydjQeaYMBTNJrhOYWVdA7393287 = ydjQeaYMBTNJrhOYWVdA28997867;     ydjQeaYMBTNJrhOYWVdA28997867 = ydjQeaYMBTNJrhOYWVdA65820081;     ydjQeaYMBTNJrhOYWVdA65820081 = ydjQeaYMBTNJrhOYWVdA34697595;     ydjQeaYMBTNJrhOYWVdA34697595 = ydjQeaYMBTNJrhOYWVdA96153779;     ydjQeaYMBTNJrhOYWVdA96153779 = ydjQeaYMBTNJrhOYWVdA59239519;     ydjQeaYMBTNJrhOYWVdA59239519 = ydjQeaYMBTNJrhOYWVdA21400823;     ydjQeaYMBTNJrhOYWVdA21400823 = ydjQeaYMBTNJrhOYWVdA32111195;     ydjQeaYMBTNJrhOYWVdA32111195 = ydjQeaYMBTNJrhOYWVdA27715795;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mtSqXzdCzZKyrqIhbhog3181411() {     float jRnQWbrNJzxuQpUtEawK98367462 = -846125186;    float jRnQWbrNJzxuQpUtEawK77353122 = -764420127;    float jRnQWbrNJzxuQpUtEawK58843356 = -261584557;    float jRnQWbrNJzxuQpUtEawK29788023 = 62838261;    float jRnQWbrNJzxuQpUtEawK82687010 = -586846916;    float jRnQWbrNJzxuQpUtEawK55897503 = -884611928;    float jRnQWbrNJzxuQpUtEawK26147675 = -100229810;    float jRnQWbrNJzxuQpUtEawK4284870 = -856171099;    float jRnQWbrNJzxuQpUtEawK698007 = -841704172;    float jRnQWbrNJzxuQpUtEawK22488161 = -939073505;    float jRnQWbrNJzxuQpUtEawK25416086 = -545343717;    float jRnQWbrNJzxuQpUtEawK60887183 = -382130314;    float jRnQWbrNJzxuQpUtEawK83678696 = -708183109;    float jRnQWbrNJzxuQpUtEawK74211069 = -566088438;    float jRnQWbrNJzxuQpUtEawK38251881 = 16333422;    float jRnQWbrNJzxuQpUtEawK93608504 = -481264782;    float jRnQWbrNJzxuQpUtEawK99915369 = -74401894;    float jRnQWbrNJzxuQpUtEawK64675327 = 22359207;    float jRnQWbrNJzxuQpUtEawK71332802 = -544083564;    float jRnQWbrNJzxuQpUtEawK58236547 = -702206770;    float jRnQWbrNJzxuQpUtEawK7230783 = -878644859;    float jRnQWbrNJzxuQpUtEawK56524393 = -32098195;    float jRnQWbrNJzxuQpUtEawK47844747 = 86051404;    float jRnQWbrNJzxuQpUtEawK97445404 = -929666510;    float jRnQWbrNJzxuQpUtEawK36906274 = -951145104;    float jRnQWbrNJzxuQpUtEawK77469711 = -784446818;    float jRnQWbrNJzxuQpUtEawK22624705 = -578965471;    float jRnQWbrNJzxuQpUtEawK47608241 = -712886825;    float jRnQWbrNJzxuQpUtEawK87402821 = -123396804;    float jRnQWbrNJzxuQpUtEawK46197813 = -875164336;    float jRnQWbrNJzxuQpUtEawK59996475 = -4775243;    float jRnQWbrNJzxuQpUtEawK93167378 = 88968746;    float jRnQWbrNJzxuQpUtEawK91404772 = -325438805;    float jRnQWbrNJzxuQpUtEawK40448689 = -950042505;    float jRnQWbrNJzxuQpUtEawK4830093 = -566087135;    float jRnQWbrNJzxuQpUtEawK81659451 = 5476999;    float jRnQWbrNJzxuQpUtEawK67705356 = -791149879;    float jRnQWbrNJzxuQpUtEawK77260626 = 75497404;    float jRnQWbrNJzxuQpUtEawK21895715 = -49809763;    float jRnQWbrNJzxuQpUtEawK12457201 = -971802381;    float jRnQWbrNJzxuQpUtEawK62713642 = -410154887;    float jRnQWbrNJzxuQpUtEawK16818927 = -910448261;    float jRnQWbrNJzxuQpUtEawK56013714 = -261891118;    float jRnQWbrNJzxuQpUtEawK74506343 = -571447825;    float jRnQWbrNJzxuQpUtEawK62576261 = -761799026;    float jRnQWbrNJzxuQpUtEawK83471030 = 55792146;    float jRnQWbrNJzxuQpUtEawK64646359 = -122435447;    float jRnQWbrNJzxuQpUtEawK36509654 = -505743915;    float jRnQWbrNJzxuQpUtEawK53214090 = -732870861;    float jRnQWbrNJzxuQpUtEawK63462519 = -647706796;    float jRnQWbrNJzxuQpUtEawK33144568 = -999681819;    float jRnQWbrNJzxuQpUtEawK24576903 = -923053548;    float jRnQWbrNJzxuQpUtEawK71950956 = -339267711;    float jRnQWbrNJzxuQpUtEawK9913435 = -65631985;    float jRnQWbrNJzxuQpUtEawK95950628 = -787752057;    float jRnQWbrNJzxuQpUtEawK41843069 = -714026992;    float jRnQWbrNJzxuQpUtEawK29508375 = -750471532;    float jRnQWbrNJzxuQpUtEawK61397952 = -331918047;    float jRnQWbrNJzxuQpUtEawK92881749 = 13983364;    float jRnQWbrNJzxuQpUtEawK5217300 = -802400099;    float jRnQWbrNJzxuQpUtEawK33272799 = -205646457;    float jRnQWbrNJzxuQpUtEawK78539434 = -387342985;    float jRnQWbrNJzxuQpUtEawK16882048 = -632774296;    float jRnQWbrNJzxuQpUtEawK54500194 = -966539836;    float jRnQWbrNJzxuQpUtEawK62491685 = -834298263;    float jRnQWbrNJzxuQpUtEawK32248708 = -534312463;    float jRnQWbrNJzxuQpUtEawK69482410 = 43308491;    float jRnQWbrNJzxuQpUtEawK43230007 = -758140605;    float jRnQWbrNJzxuQpUtEawK69380977 = 99998697;    float jRnQWbrNJzxuQpUtEawK56592430 = -989143578;    float jRnQWbrNJzxuQpUtEawK25903148 = -690114903;    float jRnQWbrNJzxuQpUtEawK22654744 = -49899298;    float jRnQWbrNJzxuQpUtEawK42779613 = -927831031;    float jRnQWbrNJzxuQpUtEawK58875602 = -572281183;    float jRnQWbrNJzxuQpUtEawK95522904 = -192051884;    float jRnQWbrNJzxuQpUtEawK90411856 = -968196598;    float jRnQWbrNJzxuQpUtEawK510680 = -770207077;    float jRnQWbrNJzxuQpUtEawK73338403 = -342500772;    float jRnQWbrNJzxuQpUtEawK34869144 = -67867485;    float jRnQWbrNJzxuQpUtEawK53435244 = -906937250;    float jRnQWbrNJzxuQpUtEawK12823353 = -562011371;    float jRnQWbrNJzxuQpUtEawK86115051 = 26778444;    float jRnQWbrNJzxuQpUtEawK94394151 = -980015965;    float jRnQWbrNJzxuQpUtEawK23940302 = -475690008;    float jRnQWbrNJzxuQpUtEawK13053245 = -875482518;    float jRnQWbrNJzxuQpUtEawK35419573 = -81721695;    float jRnQWbrNJzxuQpUtEawK21216422 = -571763544;    float jRnQWbrNJzxuQpUtEawK81491337 = -159806821;    float jRnQWbrNJzxuQpUtEawK44498061 = -62290449;    float jRnQWbrNJzxuQpUtEawK62987023 = -852060144;    float jRnQWbrNJzxuQpUtEawK52151076 = -244051470;    float jRnQWbrNJzxuQpUtEawK6307405 = -359231832;    float jRnQWbrNJzxuQpUtEawK84378877 = -938485961;    float jRnQWbrNJzxuQpUtEawK16678415 = -247409664;    float jRnQWbrNJzxuQpUtEawK79184402 = -666155924;    float jRnQWbrNJzxuQpUtEawK84174208 = 77188098;    float jRnQWbrNJzxuQpUtEawK99936878 = -177673966;    float jRnQWbrNJzxuQpUtEawK1513521 = -295351282;    float jRnQWbrNJzxuQpUtEawK12014659 = -737149562;    float jRnQWbrNJzxuQpUtEawK30327553 = -846125186;     jRnQWbrNJzxuQpUtEawK98367462 = jRnQWbrNJzxuQpUtEawK77353122;     jRnQWbrNJzxuQpUtEawK77353122 = jRnQWbrNJzxuQpUtEawK58843356;     jRnQWbrNJzxuQpUtEawK58843356 = jRnQWbrNJzxuQpUtEawK29788023;     jRnQWbrNJzxuQpUtEawK29788023 = jRnQWbrNJzxuQpUtEawK82687010;     jRnQWbrNJzxuQpUtEawK82687010 = jRnQWbrNJzxuQpUtEawK55897503;     jRnQWbrNJzxuQpUtEawK55897503 = jRnQWbrNJzxuQpUtEawK26147675;     jRnQWbrNJzxuQpUtEawK26147675 = jRnQWbrNJzxuQpUtEawK4284870;     jRnQWbrNJzxuQpUtEawK4284870 = jRnQWbrNJzxuQpUtEawK698007;     jRnQWbrNJzxuQpUtEawK698007 = jRnQWbrNJzxuQpUtEawK22488161;     jRnQWbrNJzxuQpUtEawK22488161 = jRnQWbrNJzxuQpUtEawK25416086;     jRnQWbrNJzxuQpUtEawK25416086 = jRnQWbrNJzxuQpUtEawK60887183;     jRnQWbrNJzxuQpUtEawK60887183 = jRnQWbrNJzxuQpUtEawK83678696;     jRnQWbrNJzxuQpUtEawK83678696 = jRnQWbrNJzxuQpUtEawK74211069;     jRnQWbrNJzxuQpUtEawK74211069 = jRnQWbrNJzxuQpUtEawK38251881;     jRnQWbrNJzxuQpUtEawK38251881 = jRnQWbrNJzxuQpUtEawK93608504;     jRnQWbrNJzxuQpUtEawK93608504 = jRnQWbrNJzxuQpUtEawK99915369;     jRnQWbrNJzxuQpUtEawK99915369 = jRnQWbrNJzxuQpUtEawK64675327;     jRnQWbrNJzxuQpUtEawK64675327 = jRnQWbrNJzxuQpUtEawK71332802;     jRnQWbrNJzxuQpUtEawK71332802 = jRnQWbrNJzxuQpUtEawK58236547;     jRnQWbrNJzxuQpUtEawK58236547 = jRnQWbrNJzxuQpUtEawK7230783;     jRnQWbrNJzxuQpUtEawK7230783 = jRnQWbrNJzxuQpUtEawK56524393;     jRnQWbrNJzxuQpUtEawK56524393 = jRnQWbrNJzxuQpUtEawK47844747;     jRnQWbrNJzxuQpUtEawK47844747 = jRnQWbrNJzxuQpUtEawK97445404;     jRnQWbrNJzxuQpUtEawK97445404 = jRnQWbrNJzxuQpUtEawK36906274;     jRnQWbrNJzxuQpUtEawK36906274 = jRnQWbrNJzxuQpUtEawK77469711;     jRnQWbrNJzxuQpUtEawK77469711 = jRnQWbrNJzxuQpUtEawK22624705;     jRnQWbrNJzxuQpUtEawK22624705 = jRnQWbrNJzxuQpUtEawK47608241;     jRnQWbrNJzxuQpUtEawK47608241 = jRnQWbrNJzxuQpUtEawK87402821;     jRnQWbrNJzxuQpUtEawK87402821 = jRnQWbrNJzxuQpUtEawK46197813;     jRnQWbrNJzxuQpUtEawK46197813 = jRnQWbrNJzxuQpUtEawK59996475;     jRnQWbrNJzxuQpUtEawK59996475 = jRnQWbrNJzxuQpUtEawK93167378;     jRnQWbrNJzxuQpUtEawK93167378 = jRnQWbrNJzxuQpUtEawK91404772;     jRnQWbrNJzxuQpUtEawK91404772 = jRnQWbrNJzxuQpUtEawK40448689;     jRnQWbrNJzxuQpUtEawK40448689 = jRnQWbrNJzxuQpUtEawK4830093;     jRnQWbrNJzxuQpUtEawK4830093 = jRnQWbrNJzxuQpUtEawK81659451;     jRnQWbrNJzxuQpUtEawK81659451 = jRnQWbrNJzxuQpUtEawK67705356;     jRnQWbrNJzxuQpUtEawK67705356 = jRnQWbrNJzxuQpUtEawK77260626;     jRnQWbrNJzxuQpUtEawK77260626 = jRnQWbrNJzxuQpUtEawK21895715;     jRnQWbrNJzxuQpUtEawK21895715 = jRnQWbrNJzxuQpUtEawK12457201;     jRnQWbrNJzxuQpUtEawK12457201 = jRnQWbrNJzxuQpUtEawK62713642;     jRnQWbrNJzxuQpUtEawK62713642 = jRnQWbrNJzxuQpUtEawK16818927;     jRnQWbrNJzxuQpUtEawK16818927 = jRnQWbrNJzxuQpUtEawK56013714;     jRnQWbrNJzxuQpUtEawK56013714 = jRnQWbrNJzxuQpUtEawK74506343;     jRnQWbrNJzxuQpUtEawK74506343 = jRnQWbrNJzxuQpUtEawK62576261;     jRnQWbrNJzxuQpUtEawK62576261 = jRnQWbrNJzxuQpUtEawK83471030;     jRnQWbrNJzxuQpUtEawK83471030 = jRnQWbrNJzxuQpUtEawK64646359;     jRnQWbrNJzxuQpUtEawK64646359 = jRnQWbrNJzxuQpUtEawK36509654;     jRnQWbrNJzxuQpUtEawK36509654 = jRnQWbrNJzxuQpUtEawK53214090;     jRnQWbrNJzxuQpUtEawK53214090 = jRnQWbrNJzxuQpUtEawK63462519;     jRnQWbrNJzxuQpUtEawK63462519 = jRnQWbrNJzxuQpUtEawK33144568;     jRnQWbrNJzxuQpUtEawK33144568 = jRnQWbrNJzxuQpUtEawK24576903;     jRnQWbrNJzxuQpUtEawK24576903 = jRnQWbrNJzxuQpUtEawK71950956;     jRnQWbrNJzxuQpUtEawK71950956 = jRnQWbrNJzxuQpUtEawK9913435;     jRnQWbrNJzxuQpUtEawK9913435 = jRnQWbrNJzxuQpUtEawK95950628;     jRnQWbrNJzxuQpUtEawK95950628 = jRnQWbrNJzxuQpUtEawK41843069;     jRnQWbrNJzxuQpUtEawK41843069 = jRnQWbrNJzxuQpUtEawK29508375;     jRnQWbrNJzxuQpUtEawK29508375 = jRnQWbrNJzxuQpUtEawK61397952;     jRnQWbrNJzxuQpUtEawK61397952 = jRnQWbrNJzxuQpUtEawK92881749;     jRnQWbrNJzxuQpUtEawK92881749 = jRnQWbrNJzxuQpUtEawK5217300;     jRnQWbrNJzxuQpUtEawK5217300 = jRnQWbrNJzxuQpUtEawK33272799;     jRnQWbrNJzxuQpUtEawK33272799 = jRnQWbrNJzxuQpUtEawK78539434;     jRnQWbrNJzxuQpUtEawK78539434 = jRnQWbrNJzxuQpUtEawK16882048;     jRnQWbrNJzxuQpUtEawK16882048 = jRnQWbrNJzxuQpUtEawK54500194;     jRnQWbrNJzxuQpUtEawK54500194 = jRnQWbrNJzxuQpUtEawK62491685;     jRnQWbrNJzxuQpUtEawK62491685 = jRnQWbrNJzxuQpUtEawK32248708;     jRnQWbrNJzxuQpUtEawK32248708 = jRnQWbrNJzxuQpUtEawK69482410;     jRnQWbrNJzxuQpUtEawK69482410 = jRnQWbrNJzxuQpUtEawK43230007;     jRnQWbrNJzxuQpUtEawK43230007 = jRnQWbrNJzxuQpUtEawK69380977;     jRnQWbrNJzxuQpUtEawK69380977 = jRnQWbrNJzxuQpUtEawK56592430;     jRnQWbrNJzxuQpUtEawK56592430 = jRnQWbrNJzxuQpUtEawK25903148;     jRnQWbrNJzxuQpUtEawK25903148 = jRnQWbrNJzxuQpUtEawK22654744;     jRnQWbrNJzxuQpUtEawK22654744 = jRnQWbrNJzxuQpUtEawK42779613;     jRnQWbrNJzxuQpUtEawK42779613 = jRnQWbrNJzxuQpUtEawK58875602;     jRnQWbrNJzxuQpUtEawK58875602 = jRnQWbrNJzxuQpUtEawK95522904;     jRnQWbrNJzxuQpUtEawK95522904 = jRnQWbrNJzxuQpUtEawK90411856;     jRnQWbrNJzxuQpUtEawK90411856 = jRnQWbrNJzxuQpUtEawK510680;     jRnQWbrNJzxuQpUtEawK510680 = jRnQWbrNJzxuQpUtEawK73338403;     jRnQWbrNJzxuQpUtEawK73338403 = jRnQWbrNJzxuQpUtEawK34869144;     jRnQWbrNJzxuQpUtEawK34869144 = jRnQWbrNJzxuQpUtEawK53435244;     jRnQWbrNJzxuQpUtEawK53435244 = jRnQWbrNJzxuQpUtEawK12823353;     jRnQWbrNJzxuQpUtEawK12823353 = jRnQWbrNJzxuQpUtEawK86115051;     jRnQWbrNJzxuQpUtEawK86115051 = jRnQWbrNJzxuQpUtEawK94394151;     jRnQWbrNJzxuQpUtEawK94394151 = jRnQWbrNJzxuQpUtEawK23940302;     jRnQWbrNJzxuQpUtEawK23940302 = jRnQWbrNJzxuQpUtEawK13053245;     jRnQWbrNJzxuQpUtEawK13053245 = jRnQWbrNJzxuQpUtEawK35419573;     jRnQWbrNJzxuQpUtEawK35419573 = jRnQWbrNJzxuQpUtEawK21216422;     jRnQWbrNJzxuQpUtEawK21216422 = jRnQWbrNJzxuQpUtEawK81491337;     jRnQWbrNJzxuQpUtEawK81491337 = jRnQWbrNJzxuQpUtEawK44498061;     jRnQWbrNJzxuQpUtEawK44498061 = jRnQWbrNJzxuQpUtEawK62987023;     jRnQWbrNJzxuQpUtEawK62987023 = jRnQWbrNJzxuQpUtEawK52151076;     jRnQWbrNJzxuQpUtEawK52151076 = jRnQWbrNJzxuQpUtEawK6307405;     jRnQWbrNJzxuQpUtEawK6307405 = jRnQWbrNJzxuQpUtEawK84378877;     jRnQWbrNJzxuQpUtEawK84378877 = jRnQWbrNJzxuQpUtEawK16678415;     jRnQWbrNJzxuQpUtEawK16678415 = jRnQWbrNJzxuQpUtEawK79184402;     jRnQWbrNJzxuQpUtEawK79184402 = jRnQWbrNJzxuQpUtEawK84174208;     jRnQWbrNJzxuQpUtEawK84174208 = jRnQWbrNJzxuQpUtEawK99936878;     jRnQWbrNJzxuQpUtEawK99936878 = jRnQWbrNJzxuQpUtEawK1513521;     jRnQWbrNJzxuQpUtEawK1513521 = jRnQWbrNJzxuQpUtEawK12014659;     jRnQWbrNJzxuQpUtEawK12014659 = jRnQWbrNJzxuQpUtEawK30327553;     jRnQWbrNJzxuQpUtEawK30327553 = jRnQWbrNJzxuQpUtEawK98367462;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void bLnxvFnOhTiYrJMkVscE9840156() {     float fmbKhrWLhaGdvKCjEMYX98360990 = -718649328;    float fmbKhrWLhaGdvKCjEMYX15925439 = -807015252;    float fmbKhrWLhaGdvKCjEMYX60156569 = -218713171;    float fmbKhrWLhaGdvKCjEMYX14867884 = -857702570;    float fmbKhrWLhaGdvKCjEMYX91527113 = 44797762;    float fmbKhrWLhaGdvKCjEMYX90702936 = -584044740;    float fmbKhrWLhaGdvKCjEMYX94695011 = -904766277;    float fmbKhrWLhaGdvKCjEMYX73498920 = -560348416;    float fmbKhrWLhaGdvKCjEMYX83438170 = -555350242;    float fmbKhrWLhaGdvKCjEMYX19127674 = -575880376;    float fmbKhrWLhaGdvKCjEMYX45736691 = -504815789;    float fmbKhrWLhaGdvKCjEMYX75178859 = -117608677;    float fmbKhrWLhaGdvKCjEMYX15531611 = -729841583;    float fmbKhrWLhaGdvKCjEMYX28055290 = -911241489;    float fmbKhrWLhaGdvKCjEMYX83423989 = 53620570;    float fmbKhrWLhaGdvKCjEMYX68173051 = -814060433;    float fmbKhrWLhaGdvKCjEMYX55768129 = -994016464;    float fmbKhrWLhaGdvKCjEMYX87675859 = -374712415;    float fmbKhrWLhaGdvKCjEMYX91761072 = -62065830;    float fmbKhrWLhaGdvKCjEMYX30310310 = -214231221;    float fmbKhrWLhaGdvKCjEMYX86900206 = -589111233;    float fmbKhrWLhaGdvKCjEMYX34820659 = -542090078;    float fmbKhrWLhaGdvKCjEMYX84723144 = -372906438;    float fmbKhrWLhaGdvKCjEMYX41095513 = -810587033;    float fmbKhrWLhaGdvKCjEMYX65348963 = -277838957;    float fmbKhrWLhaGdvKCjEMYX94043788 = -692652746;    float fmbKhrWLhaGdvKCjEMYX92846286 = -879765184;    float fmbKhrWLhaGdvKCjEMYX83011766 = -221853814;    float fmbKhrWLhaGdvKCjEMYX81269 = -769467119;    float fmbKhrWLhaGdvKCjEMYX22287683 = -550595710;    float fmbKhrWLhaGdvKCjEMYX93409154 = -391868477;    float fmbKhrWLhaGdvKCjEMYX77891235 = -302853016;    float fmbKhrWLhaGdvKCjEMYX76857195 = -527383021;    float fmbKhrWLhaGdvKCjEMYX40731527 = -826724237;    float fmbKhrWLhaGdvKCjEMYX87188130 = -825401060;    float fmbKhrWLhaGdvKCjEMYX24487641 = -74288747;    float fmbKhrWLhaGdvKCjEMYX79646724 = -890803257;    float fmbKhrWLhaGdvKCjEMYX47178306 = 80234725;    float fmbKhrWLhaGdvKCjEMYX57209311 = -762491179;    float fmbKhrWLhaGdvKCjEMYX84330093 = -581234067;    float fmbKhrWLhaGdvKCjEMYX50569309 = -498098785;    float fmbKhrWLhaGdvKCjEMYX2516588 = -4240343;    float fmbKhrWLhaGdvKCjEMYX16297075 = -821021372;    float fmbKhrWLhaGdvKCjEMYX33738242 = -128412268;    float fmbKhrWLhaGdvKCjEMYX46594313 = -135058428;    float fmbKhrWLhaGdvKCjEMYX23898999 = -856252525;    float fmbKhrWLhaGdvKCjEMYX92344151 = -603716069;    float fmbKhrWLhaGdvKCjEMYX11893235 = -14505271;    float fmbKhrWLhaGdvKCjEMYX71872479 = -296634723;    float fmbKhrWLhaGdvKCjEMYX83507942 = -234731683;    float fmbKhrWLhaGdvKCjEMYX46403938 = -578236826;    float fmbKhrWLhaGdvKCjEMYX39124160 = -205199297;    float fmbKhrWLhaGdvKCjEMYX47399305 = -231321719;    float fmbKhrWLhaGdvKCjEMYX20163706 = -597551836;    float fmbKhrWLhaGdvKCjEMYX22939949 = -544651334;    float fmbKhrWLhaGdvKCjEMYX63540331 = -76559250;    float fmbKhrWLhaGdvKCjEMYX31202295 = -334108815;    float fmbKhrWLhaGdvKCjEMYX19061056 = -408126138;    float fmbKhrWLhaGdvKCjEMYX49518920 = -479863613;    float fmbKhrWLhaGdvKCjEMYX97483324 = -262549492;    float fmbKhrWLhaGdvKCjEMYX97856649 = -704279556;    float fmbKhrWLhaGdvKCjEMYX11683245 = -582912464;    float fmbKhrWLhaGdvKCjEMYX73417652 = -790881298;    float fmbKhrWLhaGdvKCjEMYX61150488 = 95245468;    float fmbKhrWLhaGdvKCjEMYX25718520 = -84011900;    float fmbKhrWLhaGdvKCjEMYX67845455 = -101962774;    float fmbKhrWLhaGdvKCjEMYX98321664 = -590225656;    float fmbKhrWLhaGdvKCjEMYX74800083 = -903117346;    float fmbKhrWLhaGdvKCjEMYX40867159 = 14159570;    float fmbKhrWLhaGdvKCjEMYX58936349 = -872090683;    float fmbKhrWLhaGdvKCjEMYX88526327 = -923257176;    float fmbKhrWLhaGdvKCjEMYX8589824 = -974251189;    float fmbKhrWLhaGdvKCjEMYX30466549 = -612221237;    float fmbKhrWLhaGdvKCjEMYX7430979 = -480831764;    float fmbKhrWLhaGdvKCjEMYX79741000 = -716132437;    float fmbKhrWLhaGdvKCjEMYX84383619 = -484870890;    float fmbKhrWLhaGdvKCjEMYX18523585 = -721068706;    float fmbKhrWLhaGdvKCjEMYX50984902 = -144494170;    float fmbKhrWLhaGdvKCjEMYX94501199 = -575528606;    float fmbKhrWLhaGdvKCjEMYX41449965 = -421586432;    float fmbKhrWLhaGdvKCjEMYX1699637 = 11063322;    float fmbKhrWLhaGdvKCjEMYX80953052 = -765259913;    float fmbKhrWLhaGdvKCjEMYX11139287 = -925219092;    float fmbKhrWLhaGdvKCjEMYX16573326 = -434735436;    float fmbKhrWLhaGdvKCjEMYX75883745 = -972358884;    float fmbKhrWLhaGdvKCjEMYX54284994 = -86669181;    float fmbKhrWLhaGdvKCjEMYX30491931 = 28468703;    float fmbKhrWLhaGdvKCjEMYX56693490 = -929831185;    float fmbKhrWLhaGdvKCjEMYX17791578 = -182072903;    float fmbKhrWLhaGdvKCjEMYX23647799 = -648841810;    float fmbKhrWLhaGdvKCjEMYX93285345 = -740179933;    float fmbKhrWLhaGdvKCjEMYX60585668 = -382677120;    float fmbKhrWLhaGdvKCjEMYX97659385 = -439901663;    float fmbKhrWLhaGdvKCjEMYX59725986 = -399941687;    float fmbKhrWLhaGdvKCjEMYX86473444 = -876954511;    float fmbKhrWLhaGdvKCjEMYX38886065 = -915186322;    float fmbKhrWLhaGdvKCjEMYX29098936 = -213359046;    float fmbKhrWLhaGdvKCjEMYX55146586 = -816266840;    float fmbKhrWLhaGdvKCjEMYX8019723 = 55599631;    float fmbKhrWLhaGdvKCjEMYX78748857 = -718649328;     fmbKhrWLhaGdvKCjEMYX98360990 = fmbKhrWLhaGdvKCjEMYX15925439;     fmbKhrWLhaGdvKCjEMYX15925439 = fmbKhrWLhaGdvKCjEMYX60156569;     fmbKhrWLhaGdvKCjEMYX60156569 = fmbKhrWLhaGdvKCjEMYX14867884;     fmbKhrWLhaGdvKCjEMYX14867884 = fmbKhrWLhaGdvKCjEMYX91527113;     fmbKhrWLhaGdvKCjEMYX91527113 = fmbKhrWLhaGdvKCjEMYX90702936;     fmbKhrWLhaGdvKCjEMYX90702936 = fmbKhrWLhaGdvKCjEMYX94695011;     fmbKhrWLhaGdvKCjEMYX94695011 = fmbKhrWLhaGdvKCjEMYX73498920;     fmbKhrWLhaGdvKCjEMYX73498920 = fmbKhrWLhaGdvKCjEMYX83438170;     fmbKhrWLhaGdvKCjEMYX83438170 = fmbKhrWLhaGdvKCjEMYX19127674;     fmbKhrWLhaGdvKCjEMYX19127674 = fmbKhrWLhaGdvKCjEMYX45736691;     fmbKhrWLhaGdvKCjEMYX45736691 = fmbKhrWLhaGdvKCjEMYX75178859;     fmbKhrWLhaGdvKCjEMYX75178859 = fmbKhrWLhaGdvKCjEMYX15531611;     fmbKhrWLhaGdvKCjEMYX15531611 = fmbKhrWLhaGdvKCjEMYX28055290;     fmbKhrWLhaGdvKCjEMYX28055290 = fmbKhrWLhaGdvKCjEMYX83423989;     fmbKhrWLhaGdvKCjEMYX83423989 = fmbKhrWLhaGdvKCjEMYX68173051;     fmbKhrWLhaGdvKCjEMYX68173051 = fmbKhrWLhaGdvKCjEMYX55768129;     fmbKhrWLhaGdvKCjEMYX55768129 = fmbKhrWLhaGdvKCjEMYX87675859;     fmbKhrWLhaGdvKCjEMYX87675859 = fmbKhrWLhaGdvKCjEMYX91761072;     fmbKhrWLhaGdvKCjEMYX91761072 = fmbKhrWLhaGdvKCjEMYX30310310;     fmbKhrWLhaGdvKCjEMYX30310310 = fmbKhrWLhaGdvKCjEMYX86900206;     fmbKhrWLhaGdvKCjEMYX86900206 = fmbKhrWLhaGdvKCjEMYX34820659;     fmbKhrWLhaGdvKCjEMYX34820659 = fmbKhrWLhaGdvKCjEMYX84723144;     fmbKhrWLhaGdvKCjEMYX84723144 = fmbKhrWLhaGdvKCjEMYX41095513;     fmbKhrWLhaGdvKCjEMYX41095513 = fmbKhrWLhaGdvKCjEMYX65348963;     fmbKhrWLhaGdvKCjEMYX65348963 = fmbKhrWLhaGdvKCjEMYX94043788;     fmbKhrWLhaGdvKCjEMYX94043788 = fmbKhrWLhaGdvKCjEMYX92846286;     fmbKhrWLhaGdvKCjEMYX92846286 = fmbKhrWLhaGdvKCjEMYX83011766;     fmbKhrWLhaGdvKCjEMYX83011766 = fmbKhrWLhaGdvKCjEMYX81269;     fmbKhrWLhaGdvKCjEMYX81269 = fmbKhrWLhaGdvKCjEMYX22287683;     fmbKhrWLhaGdvKCjEMYX22287683 = fmbKhrWLhaGdvKCjEMYX93409154;     fmbKhrWLhaGdvKCjEMYX93409154 = fmbKhrWLhaGdvKCjEMYX77891235;     fmbKhrWLhaGdvKCjEMYX77891235 = fmbKhrWLhaGdvKCjEMYX76857195;     fmbKhrWLhaGdvKCjEMYX76857195 = fmbKhrWLhaGdvKCjEMYX40731527;     fmbKhrWLhaGdvKCjEMYX40731527 = fmbKhrWLhaGdvKCjEMYX87188130;     fmbKhrWLhaGdvKCjEMYX87188130 = fmbKhrWLhaGdvKCjEMYX24487641;     fmbKhrWLhaGdvKCjEMYX24487641 = fmbKhrWLhaGdvKCjEMYX79646724;     fmbKhrWLhaGdvKCjEMYX79646724 = fmbKhrWLhaGdvKCjEMYX47178306;     fmbKhrWLhaGdvKCjEMYX47178306 = fmbKhrWLhaGdvKCjEMYX57209311;     fmbKhrWLhaGdvKCjEMYX57209311 = fmbKhrWLhaGdvKCjEMYX84330093;     fmbKhrWLhaGdvKCjEMYX84330093 = fmbKhrWLhaGdvKCjEMYX50569309;     fmbKhrWLhaGdvKCjEMYX50569309 = fmbKhrWLhaGdvKCjEMYX2516588;     fmbKhrWLhaGdvKCjEMYX2516588 = fmbKhrWLhaGdvKCjEMYX16297075;     fmbKhrWLhaGdvKCjEMYX16297075 = fmbKhrWLhaGdvKCjEMYX33738242;     fmbKhrWLhaGdvKCjEMYX33738242 = fmbKhrWLhaGdvKCjEMYX46594313;     fmbKhrWLhaGdvKCjEMYX46594313 = fmbKhrWLhaGdvKCjEMYX23898999;     fmbKhrWLhaGdvKCjEMYX23898999 = fmbKhrWLhaGdvKCjEMYX92344151;     fmbKhrWLhaGdvKCjEMYX92344151 = fmbKhrWLhaGdvKCjEMYX11893235;     fmbKhrWLhaGdvKCjEMYX11893235 = fmbKhrWLhaGdvKCjEMYX71872479;     fmbKhrWLhaGdvKCjEMYX71872479 = fmbKhrWLhaGdvKCjEMYX83507942;     fmbKhrWLhaGdvKCjEMYX83507942 = fmbKhrWLhaGdvKCjEMYX46403938;     fmbKhrWLhaGdvKCjEMYX46403938 = fmbKhrWLhaGdvKCjEMYX39124160;     fmbKhrWLhaGdvKCjEMYX39124160 = fmbKhrWLhaGdvKCjEMYX47399305;     fmbKhrWLhaGdvKCjEMYX47399305 = fmbKhrWLhaGdvKCjEMYX20163706;     fmbKhrWLhaGdvKCjEMYX20163706 = fmbKhrWLhaGdvKCjEMYX22939949;     fmbKhrWLhaGdvKCjEMYX22939949 = fmbKhrWLhaGdvKCjEMYX63540331;     fmbKhrWLhaGdvKCjEMYX63540331 = fmbKhrWLhaGdvKCjEMYX31202295;     fmbKhrWLhaGdvKCjEMYX31202295 = fmbKhrWLhaGdvKCjEMYX19061056;     fmbKhrWLhaGdvKCjEMYX19061056 = fmbKhrWLhaGdvKCjEMYX49518920;     fmbKhrWLhaGdvKCjEMYX49518920 = fmbKhrWLhaGdvKCjEMYX97483324;     fmbKhrWLhaGdvKCjEMYX97483324 = fmbKhrWLhaGdvKCjEMYX97856649;     fmbKhrWLhaGdvKCjEMYX97856649 = fmbKhrWLhaGdvKCjEMYX11683245;     fmbKhrWLhaGdvKCjEMYX11683245 = fmbKhrWLhaGdvKCjEMYX73417652;     fmbKhrWLhaGdvKCjEMYX73417652 = fmbKhrWLhaGdvKCjEMYX61150488;     fmbKhrWLhaGdvKCjEMYX61150488 = fmbKhrWLhaGdvKCjEMYX25718520;     fmbKhrWLhaGdvKCjEMYX25718520 = fmbKhrWLhaGdvKCjEMYX67845455;     fmbKhrWLhaGdvKCjEMYX67845455 = fmbKhrWLhaGdvKCjEMYX98321664;     fmbKhrWLhaGdvKCjEMYX98321664 = fmbKhrWLhaGdvKCjEMYX74800083;     fmbKhrWLhaGdvKCjEMYX74800083 = fmbKhrWLhaGdvKCjEMYX40867159;     fmbKhrWLhaGdvKCjEMYX40867159 = fmbKhrWLhaGdvKCjEMYX58936349;     fmbKhrWLhaGdvKCjEMYX58936349 = fmbKhrWLhaGdvKCjEMYX88526327;     fmbKhrWLhaGdvKCjEMYX88526327 = fmbKhrWLhaGdvKCjEMYX8589824;     fmbKhrWLhaGdvKCjEMYX8589824 = fmbKhrWLhaGdvKCjEMYX30466549;     fmbKhrWLhaGdvKCjEMYX30466549 = fmbKhrWLhaGdvKCjEMYX7430979;     fmbKhrWLhaGdvKCjEMYX7430979 = fmbKhrWLhaGdvKCjEMYX79741000;     fmbKhrWLhaGdvKCjEMYX79741000 = fmbKhrWLhaGdvKCjEMYX84383619;     fmbKhrWLhaGdvKCjEMYX84383619 = fmbKhrWLhaGdvKCjEMYX18523585;     fmbKhrWLhaGdvKCjEMYX18523585 = fmbKhrWLhaGdvKCjEMYX50984902;     fmbKhrWLhaGdvKCjEMYX50984902 = fmbKhrWLhaGdvKCjEMYX94501199;     fmbKhrWLhaGdvKCjEMYX94501199 = fmbKhrWLhaGdvKCjEMYX41449965;     fmbKhrWLhaGdvKCjEMYX41449965 = fmbKhrWLhaGdvKCjEMYX1699637;     fmbKhrWLhaGdvKCjEMYX1699637 = fmbKhrWLhaGdvKCjEMYX80953052;     fmbKhrWLhaGdvKCjEMYX80953052 = fmbKhrWLhaGdvKCjEMYX11139287;     fmbKhrWLhaGdvKCjEMYX11139287 = fmbKhrWLhaGdvKCjEMYX16573326;     fmbKhrWLhaGdvKCjEMYX16573326 = fmbKhrWLhaGdvKCjEMYX75883745;     fmbKhrWLhaGdvKCjEMYX75883745 = fmbKhrWLhaGdvKCjEMYX54284994;     fmbKhrWLhaGdvKCjEMYX54284994 = fmbKhrWLhaGdvKCjEMYX30491931;     fmbKhrWLhaGdvKCjEMYX30491931 = fmbKhrWLhaGdvKCjEMYX56693490;     fmbKhrWLhaGdvKCjEMYX56693490 = fmbKhrWLhaGdvKCjEMYX17791578;     fmbKhrWLhaGdvKCjEMYX17791578 = fmbKhrWLhaGdvKCjEMYX23647799;     fmbKhrWLhaGdvKCjEMYX23647799 = fmbKhrWLhaGdvKCjEMYX93285345;     fmbKhrWLhaGdvKCjEMYX93285345 = fmbKhrWLhaGdvKCjEMYX60585668;     fmbKhrWLhaGdvKCjEMYX60585668 = fmbKhrWLhaGdvKCjEMYX97659385;     fmbKhrWLhaGdvKCjEMYX97659385 = fmbKhrWLhaGdvKCjEMYX59725986;     fmbKhrWLhaGdvKCjEMYX59725986 = fmbKhrWLhaGdvKCjEMYX86473444;     fmbKhrWLhaGdvKCjEMYX86473444 = fmbKhrWLhaGdvKCjEMYX38886065;     fmbKhrWLhaGdvKCjEMYX38886065 = fmbKhrWLhaGdvKCjEMYX29098936;     fmbKhrWLhaGdvKCjEMYX29098936 = fmbKhrWLhaGdvKCjEMYX55146586;     fmbKhrWLhaGdvKCjEMYX55146586 = fmbKhrWLhaGdvKCjEMYX8019723;     fmbKhrWLhaGdvKCjEMYX8019723 = fmbKhrWLhaGdvKCjEMYX78748857;     fmbKhrWLhaGdvKCjEMYX78748857 = fmbKhrWLhaGdvKCjEMYX98360990;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void gfcipmMIjFMwELoMYVCK64256368() {     float TRoifUbndhgGjJevAGzS69012658 = -236798458;    float TRoifUbndhgGjJevAGzS84598956 = -678658551;    float TRoifUbndhgGjJevAGzS40125488 = -661007631;    float TRoifUbndhgGjJevAGzS54799170 = 52159041;    float TRoifUbndhgGjJevAGzS12977435 = -838608585;    float TRoifUbndhgGjJevAGzS80702698 = -285625891;    float TRoifUbndhgGjJevAGzS13073907 = -903541987;    float TRoifUbndhgGjJevAGzS30341301 = -907165506;    float TRoifUbndhgGjJevAGzS29303225 = -648697570;    float TRoifUbndhgGjJevAGzS42094135 = -876975727;    float TRoifUbndhgGjJevAGzS87985851 = -539717719;    float TRoifUbndhgGjJevAGzS20725431 = -433372470;    float TRoifUbndhgGjJevAGzS95358997 = -639378641;    float TRoifUbndhgGjJevAGzS11902323 = -712269699;    float TRoifUbndhgGjJevAGzS66144009 = -96129433;    float TRoifUbndhgGjJevAGzS701692 = -384044019;    float TRoifUbndhgGjJevAGzS49620956 = -903201593;    float TRoifUbndhgGjJevAGzS7969354 = -363878611;    float TRoifUbndhgGjJevAGzS73468854 = -769253772;    float TRoifUbndhgGjJevAGzS89097018 = -756597526;    float TRoifUbndhgGjJevAGzS42035961 = -497790373;    float TRoifUbndhgGjJevAGzS96216424 = -873375383;    float TRoifUbndhgGjJevAGzS86666952 = -940749583;    float TRoifUbndhgGjJevAGzS74945049 = -402987939;    float TRoifUbndhgGjJevAGzS29473352 = 46256355;    float TRoifUbndhgGjJevAGzS24641369 = -790937191;    float TRoifUbndhgGjJevAGzS1107073 = -174942959;    float TRoifUbndhgGjJevAGzS90793991 = -726977581;    float TRoifUbndhgGjJevAGzS32223756 = 36174664;    float TRoifUbndhgGjJevAGzS61459779 = -8990780;    float TRoifUbndhgGjJevAGzS481151 = -140302163;    float TRoifUbndhgGjJevAGzS22402697 = -631150179;    float TRoifUbndhgGjJevAGzS59802556 = -474344630;    float TRoifUbndhgGjJevAGzS6096502 = -390253921;    float TRoifUbndhgGjJevAGzS65212366 = -672347839;    float TRoifUbndhgGjJevAGzS84480899 = -153515295;    float TRoifUbndhgGjJevAGzS23438210 = 20603535;    float TRoifUbndhgGjJevAGzS99970794 = -803878383;    float TRoifUbndhgGjJevAGzS35742600 = -718219816;    float TRoifUbndhgGjJevAGzS79433391 = -98842429;    float TRoifUbndhgGjJevAGzS10642594 = -882117664;    float TRoifUbndhgGjJevAGzS30999578 = -873629413;    float TRoifUbndhgGjJevAGzS65264034 = -807775424;    float TRoifUbndhgGjJevAGzS40246543 = -200387240;    float TRoifUbndhgGjJevAGzS42548372 = -443704822;    float TRoifUbndhgGjJevAGzS30304833 = -614589680;    float TRoifUbndhgGjJevAGzS18475355 = -719785868;    float TRoifUbndhgGjJevAGzS77507444 = -466635567;    float TRoifUbndhgGjJevAGzS77313931 = -282398692;    float TRoifUbndhgGjJevAGzS91935428 = -517740561;    float TRoifUbndhgGjJevAGzS14593263 = -124808948;    float TRoifUbndhgGjJevAGzS80995664 = -174419018;    float TRoifUbndhgGjJevAGzS2474962 = -159119761;    float TRoifUbndhgGjJevAGzS10391010 = -158433333;    float TRoifUbndhgGjJevAGzS22220408 = -655592249;    float TRoifUbndhgGjJevAGzS72796234 = -363423075;    float TRoifUbndhgGjJevAGzS97932003 = -737908969;    float TRoifUbndhgGjJevAGzS65180438 = -158019692;    float TRoifUbndhgGjJevAGzS25325819 = -994097315;    float TRoifUbndhgGjJevAGzS88336065 = 52328605;    float TRoifUbndhgGjJevAGzS79595626 = -10682932;    float TRoifUbndhgGjJevAGzS22279915 = -76564406;    float TRoifUbndhgGjJevAGzS98117544 = -843340171;    float TRoifUbndhgGjJevAGzS67843446 = -539706790;    float TRoifUbndhgGjJevAGzS41612984 = -636673565;    float TRoifUbndhgGjJevAGzS65583155 = -908567540;    float TRoifUbndhgGjJevAGzS60922875 = -959027840;    float TRoifUbndhgGjJevAGzS89262495 = -149124721;    float TRoifUbndhgGjJevAGzS46689957 = 60078140;    float TRoifUbndhgGjJevAGzS81663109 = -942614139;    float TRoifUbndhgGjJevAGzS77263482 = -304647555;    float TRoifUbndhgGjJevAGzS49650162 = 676789;    float TRoifUbndhgGjJevAGzS72226754 = -645658796;    float TRoifUbndhgGjJevAGzS94035462 = -570411343;    float TRoifUbndhgGjJevAGzS78454425 = -874479863;    float TRoifUbndhgGjJevAGzS11036383 = -624160961;    float TRoifUbndhgGjJevAGzS30952390 = 34400040;    float TRoifUbndhgGjJevAGzS46420410 = -640362343;    float TRoifUbndhgGjJevAGzS32396678 = -959283117;    float TRoifUbndhgGjJevAGzS99168518 = -339153965;    float TRoifUbndhgGjJevAGzS6166015 = 28848677;    float TRoifUbndhgGjJevAGzS23599628 = -708307393;    float TRoifUbndhgGjJevAGzS13480060 = -344578890;    float TRoifUbndhgGjJevAGzS40288327 = -446084775;    float TRoifUbndhgGjJevAGzS46866516 = -884181833;    float TRoifUbndhgGjJevAGzS19485487 = -965883145;    float TRoifUbndhgGjJevAGzS19927735 = -372030418;    float TRoifUbndhgGjJevAGzS49411546 = -215911298;    float TRoifUbndhgGjJevAGzS83876094 = -734661673;    float TRoifUbndhgGjJevAGzS92416132 = -208924765;    float TRoifUbndhgGjJevAGzS86548895 = -415606326;    float TRoifUbndhgGjJevAGzS58257772 = -821376774;    float TRoifUbndhgGjJevAGzS74644976 = -809781069;    float TRoifUbndhgGjJevAGzS47406534 = -670548421;    float TRoifUbndhgGjJevAGzS99837765 = 11840503;    float TRoifUbndhgGjJevAGzS88362678 = -705553258;    float TRoifUbndhgGjJevAGzS32882034 = 69710758;    float TRoifUbndhgGjJevAGzS97420587 = -168068635;    float TRoifUbndhgGjJevAGzS98633558 = -563713675;    float TRoifUbndhgGjJevAGzS76965216 = -236798458;     TRoifUbndhgGjJevAGzS69012658 = TRoifUbndhgGjJevAGzS84598956;     TRoifUbndhgGjJevAGzS84598956 = TRoifUbndhgGjJevAGzS40125488;     TRoifUbndhgGjJevAGzS40125488 = TRoifUbndhgGjJevAGzS54799170;     TRoifUbndhgGjJevAGzS54799170 = TRoifUbndhgGjJevAGzS12977435;     TRoifUbndhgGjJevAGzS12977435 = TRoifUbndhgGjJevAGzS80702698;     TRoifUbndhgGjJevAGzS80702698 = TRoifUbndhgGjJevAGzS13073907;     TRoifUbndhgGjJevAGzS13073907 = TRoifUbndhgGjJevAGzS30341301;     TRoifUbndhgGjJevAGzS30341301 = TRoifUbndhgGjJevAGzS29303225;     TRoifUbndhgGjJevAGzS29303225 = TRoifUbndhgGjJevAGzS42094135;     TRoifUbndhgGjJevAGzS42094135 = TRoifUbndhgGjJevAGzS87985851;     TRoifUbndhgGjJevAGzS87985851 = TRoifUbndhgGjJevAGzS20725431;     TRoifUbndhgGjJevAGzS20725431 = TRoifUbndhgGjJevAGzS95358997;     TRoifUbndhgGjJevAGzS95358997 = TRoifUbndhgGjJevAGzS11902323;     TRoifUbndhgGjJevAGzS11902323 = TRoifUbndhgGjJevAGzS66144009;     TRoifUbndhgGjJevAGzS66144009 = TRoifUbndhgGjJevAGzS701692;     TRoifUbndhgGjJevAGzS701692 = TRoifUbndhgGjJevAGzS49620956;     TRoifUbndhgGjJevAGzS49620956 = TRoifUbndhgGjJevAGzS7969354;     TRoifUbndhgGjJevAGzS7969354 = TRoifUbndhgGjJevAGzS73468854;     TRoifUbndhgGjJevAGzS73468854 = TRoifUbndhgGjJevAGzS89097018;     TRoifUbndhgGjJevAGzS89097018 = TRoifUbndhgGjJevAGzS42035961;     TRoifUbndhgGjJevAGzS42035961 = TRoifUbndhgGjJevAGzS96216424;     TRoifUbndhgGjJevAGzS96216424 = TRoifUbndhgGjJevAGzS86666952;     TRoifUbndhgGjJevAGzS86666952 = TRoifUbndhgGjJevAGzS74945049;     TRoifUbndhgGjJevAGzS74945049 = TRoifUbndhgGjJevAGzS29473352;     TRoifUbndhgGjJevAGzS29473352 = TRoifUbndhgGjJevAGzS24641369;     TRoifUbndhgGjJevAGzS24641369 = TRoifUbndhgGjJevAGzS1107073;     TRoifUbndhgGjJevAGzS1107073 = TRoifUbndhgGjJevAGzS90793991;     TRoifUbndhgGjJevAGzS90793991 = TRoifUbndhgGjJevAGzS32223756;     TRoifUbndhgGjJevAGzS32223756 = TRoifUbndhgGjJevAGzS61459779;     TRoifUbndhgGjJevAGzS61459779 = TRoifUbndhgGjJevAGzS481151;     TRoifUbndhgGjJevAGzS481151 = TRoifUbndhgGjJevAGzS22402697;     TRoifUbndhgGjJevAGzS22402697 = TRoifUbndhgGjJevAGzS59802556;     TRoifUbndhgGjJevAGzS59802556 = TRoifUbndhgGjJevAGzS6096502;     TRoifUbndhgGjJevAGzS6096502 = TRoifUbndhgGjJevAGzS65212366;     TRoifUbndhgGjJevAGzS65212366 = TRoifUbndhgGjJevAGzS84480899;     TRoifUbndhgGjJevAGzS84480899 = TRoifUbndhgGjJevAGzS23438210;     TRoifUbndhgGjJevAGzS23438210 = TRoifUbndhgGjJevAGzS99970794;     TRoifUbndhgGjJevAGzS99970794 = TRoifUbndhgGjJevAGzS35742600;     TRoifUbndhgGjJevAGzS35742600 = TRoifUbndhgGjJevAGzS79433391;     TRoifUbndhgGjJevAGzS79433391 = TRoifUbndhgGjJevAGzS10642594;     TRoifUbndhgGjJevAGzS10642594 = TRoifUbndhgGjJevAGzS30999578;     TRoifUbndhgGjJevAGzS30999578 = TRoifUbndhgGjJevAGzS65264034;     TRoifUbndhgGjJevAGzS65264034 = TRoifUbndhgGjJevAGzS40246543;     TRoifUbndhgGjJevAGzS40246543 = TRoifUbndhgGjJevAGzS42548372;     TRoifUbndhgGjJevAGzS42548372 = TRoifUbndhgGjJevAGzS30304833;     TRoifUbndhgGjJevAGzS30304833 = TRoifUbndhgGjJevAGzS18475355;     TRoifUbndhgGjJevAGzS18475355 = TRoifUbndhgGjJevAGzS77507444;     TRoifUbndhgGjJevAGzS77507444 = TRoifUbndhgGjJevAGzS77313931;     TRoifUbndhgGjJevAGzS77313931 = TRoifUbndhgGjJevAGzS91935428;     TRoifUbndhgGjJevAGzS91935428 = TRoifUbndhgGjJevAGzS14593263;     TRoifUbndhgGjJevAGzS14593263 = TRoifUbndhgGjJevAGzS80995664;     TRoifUbndhgGjJevAGzS80995664 = TRoifUbndhgGjJevAGzS2474962;     TRoifUbndhgGjJevAGzS2474962 = TRoifUbndhgGjJevAGzS10391010;     TRoifUbndhgGjJevAGzS10391010 = TRoifUbndhgGjJevAGzS22220408;     TRoifUbndhgGjJevAGzS22220408 = TRoifUbndhgGjJevAGzS72796234;     TRoifUbndhgGjJevAGzS72796234 = TRoifUbndhgGjJevAGzS97932003;     TRoifUbndhgGjJevAGzS97932003 = TRoifUbndhgGjJevAGzS65180438;     TRoifUbndhgGjJevAGzS65180438 = TRoifUbndhgGjJevAGzS25325819;     TRoifUbndhgGjJevAGzS25325819 = TRoifUbndhgGjJevAGzS88336065;     TRoifUbndhgGjJevAGzS88336065 = TRoifUbndhgGjJevAGzS79595626;     TRoifUbndhgGjJevAGzS79595626 = TRoifUbndhgGjJevAGzS22279915;     TRoifUbndhgGjJevAGzS22279915 = TRoifUbndhgGjJevAGzS98117544;     TRoifUbndhgGjJevAGzS98117544 = TRoifUbndhgGjJevAGzS67843446;     TRoifUbndhgGjJevAGzS67843446 = TRoifUbndhgGjJevAGzS41612984;     TRoifUbndhgGjJevAGzS41612984 = TRoifUbndhgGjJevAGzS65583155;     TRoifUbndhgGjJevAGzS65583155 = TRoifUbndhgGjJevAGzS60922875;     TRoifUbndhgGjJevAGzS60922875 = TRoifUbndhgGjJevAGzS89262495;     TRoifUbndhgGjJevAGzS89262495 = TRoifUbndhgGjJevAGzS46689957;     TRoifUbndhgGjJevAGzS46689957 = TRoifUbndhgGjJevAGzS81663109;     TRoifUbndhgGjJevAGzS81663109 = TRoifUbndhgGjJevAGzS77263482;     TRoifUbndhgGjJevAGzS77263482 = TRoifUbndhgGjJevAGzS49650162;     TRoifUbndhgGjJevAGzS49650162 = TRoifUbndhgGjJevAGzS72226754;     TRoifUbndhgGjJevAGzS72226754 = TRoifUbndhgGjJevAGzS94035462;     TRoifUbndhgGjJevAGzS94035462 = TRoifUbndhgGjJevAGzS78454425;     TRoifUbndhgGjJevAGzS78454425 = TRoifUbndhgGjJevAGzS11036383;     TRoifUbndhgGjJevAGzS11036383 = TRoifUbndhgGjJevAGzS30952390;     TRoifUbndhgGjJevAGzS30952390 = TRoifUbndhgGjJevAGzS46420410;     TRoifUbndhgGjJevAGzS46420410 = TRoifUbndhgGjJevAGzS32396678;     TRoifUbndhgGjJevAGzS32396678 = TRoifUbndhgGjJevAGzS99168518;     TRoifUbndhgGjJevAGzS99168518 = TRoifUbndhgGjJevAGzS6166015;     TRoifUbndhgGjJevAGzS6166015 = TRoifUbndhgGjJevAGzS23599628;     TRoifUbndhgGjJevAGzS23599628 = TRoifUbndhgGjJevAGzS13480060;     TRoifUbndhgGjJevAGzS13480060 = TRoifUbndhgGjJevAGzS40288327;     TRoifUbndhgGjJevAGzS40288327 = TRoifUbndhgGjJevAGzS46866516;     TRoifUbndhgGjJevAGzS46866516 = TRoifUbndhgGjJevAGzS19485487;     TRoifUbndhgGjJevAGzS19485487 = TRoifUbndhgGjJevAGzS19927735;     TRoifUbndhgGjJevAGzS19927735 = TRoifUbndhgGjJevAGzS49411546;     TRoifUbndhgGjJevAGzS49411546 = TRoifUbndhgGjJevAGzS83876094;     TRoifUbndhgGjJevAGzS83876094 = TRoifUbndhgGjJevAGzS92416132;     TRoifUbndhgGjJevAGzS92416132 = TRoifUbndhgGjJevAGzS86548895;     TRoifUbndhgGjJevAGzS86548895 = TRoifUbndhgGjJevAGzS58257772;     TRoifUbndhgGjJevAGzS58257772 = TRoifUbndhgGjJevAGzS74644976;     TRoifUbndhgGjJevAGzS74644976 = TRoifUbndhgGjJevAGzS47406534;     TRoifUbndhgGjJevAGzS47406534 = TRoifUbndhgGjJevAGzS99837765;     TRoifUbndhgGjJevAGzS99837765 = TRoifUbndhgGjJevAGzS88362678;     TRoifUbndhgGjJevAGzS88362678 = TRoifUbndhgGjJevAGzS32882034;     TRoifUbndhgGjJevAGzS32882034 = TRoifUbndhgGjJevAGzS97420587;     TRoifUbndhgGjJevAGzS97420587 = TRoifUbndhgGjJevAGzS98633558;     TRoifUbndhgGjJevAGzS98633558 = TRoifUbndhgGjJevAGzS76965216;     TRoifUbndhgGjJevAGzS76965216 = TRoifUbndhgGjJevAGzS69012658;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xGLnrKJzmLGVXcaSWYtn70915113() {     float WVotMfAAkdQZpQmAVyxB69006186 = -109322599;    float WVotMfAAkdQZpQmAVyxB23171273 = -721253677;    float WVotMfAAkdQZpQmAVyxB41438700 = -618136244;    float WVotMfAAkdQZpQmAVyxB39879031 = -868381789;    float WVotMfAAkdQZpQmAVyxB21817537 = -206963907;    float WVotMfAAkdQZpQmAVyxB15508131 = 14941297;    float WVotMfAAkdQZpQmAVyxB81621242 = -608078454;    float WVotMfAAkdQZpQmAVyxB99555351 = -611342823;    float WVotMfAAkdQZpQmAVyxB12043390 = -362343640;    float WVotMfAAkdQZpQmAVyxB38733649 = -513782598;    float WVotMfAAkdQZpQmAVyxB8306458 = -499189791;    float WVotMfAAkdQZpQmAVyxB35017108 = -168850832;    float WVotMfAAkdQZpQmAVyxB27211911 = -661037116;    float WVotMfAAkdQZpQmAVyxB65746543 = 42577249;    float WVotMfAAkdQZpQmAVyxB11316119 = -58842285;    float WVotMfAAkdQZpQmAVyxB75266239 = -716839670;    float WVotMfAAkdQZpQmAVyxB5473716 = -722816163;    float WVotMfAAkdQZpQmAVyxB30969887 = -760950233;    float WVotMfAAkdQZpQmAVyxB93897123 = -287236039;    float WVotMfAAkdQZpQmAVyxB61170782 = -268621977;    float WVotMfAAkdQZpQmAVyxB21705385 = -208256747;    float WVotMfAAkdQZpQmAVyxB74512690 = -283367267;    float WVotMfAAkdQZpQmAVyxB23545349 = -299707425;    float WVotMfAAkdQZpQmAVyxB18595158 = -283908462;    float WVotMfAAkdQZpQmAVyxB57916041 = -380437498;    float WVotMfAAkdQZpQmAVyxB41215446 = -699143119;    float WVotMfAAkdQZpQmAVyxB71328654 = -475742672;    float WVotMfAAkdQZpQmAVyxB26197517 = -235944570;    float WVotMfAAkdQZpQmAVyxB44902203 = -609895650;    float WVotMfAAkdQZpQmAVyxB37549649 = -784422154;    float WVotMfAAkdQZpQmAVyxB33893830 = -527395397;    float WVotMfAAkdQZpQmAVyxB7126554 = 77028060;    float WVotMfAAkdQZpQmAVyxB45254979 = -676288846;    float WVotMfAAkdQZpQmAVyxB6379339 = -266935654;    float WVotMfAAkdQZpQmAVyxB47570405 = -931661764;    float WVotMfAAkdQZpQmAVyxB27309089 = -233281041;    float WVotMfAAkdQZpQmAVyxB35379578 = -79049843;    float WVotMfAAkdQZpQmAVyxB69888474 = -799141062;    float WVotMfAAkdQZpQmAVyxB71056196 = -330901232;    float WVotMfAAkdQZpQmAVyxB51306284 = -808274116;    float WVotMfAAkdQZpQmAVyxB98498260 = -970061562;    float WVotMfAAkdQZpQmAVyxB16697240 = 32578505;    float WVotMfAAkdQZpQmAVyxB25547395 = -266905678;    float WVotMfAAkdQZpQmAVyxB99478440 = -857351683;    float WVotMfAAkdQZpQmAVyxB26566424 = -916964224;    float WVotMfAAkdQZpQmAVyxB70732801 = -426634351;    float WVotMfAAkdQZpQmAVyxB46173147 = -101066491;    float WVotMfAAkdQZpQmAVyxB52891026 = 24603077;    float WVotMfAAkdQZpQmAVyxB95972321 = -946162553;    float WVotMfAAkdQZpQmAVyxB11980852 = -104765448;    float WVotMfAAkdQZpQmAVyxB27852632 = -803363955;    float WVotMfAAkdQZpQmAVyxB95542921 = -556564767;    float WVotMfAAkdQZpQmAVyxB77923310 = -51173770;    float WVotMfAAkdQZpQmAVyxB20641281 = -690353184;    float WVotMfAAkdQZpQmAVyxB49209728 = -412491526;    float WVotMfAAkdQZpQmAVyxB94493496 = -825955333;    float WVotMfAAkdQZpQmAVyxB99625923 = -321546252;    float WVotMfAAkdQZpQmAVyxB22843542 = -234227783;    float WVotMfAAkdQZpQmAVyxB81962989 = -387944292;    float WVotMfAAkdQZpQmAVyxB80602091 = -507820788;    float WVotMfAAkdQZpQmAVyxB44179477 = -509316031;    float WVotMfAAkdQZpQmAVyxB55423726 = -272133885;    float WVotMfAAkdQZpQmAVyxB54653148 = 98552826;    float WVotMfAAkdQZpQmAVyxB74493740 = -577921486;    float WVotMfAAkdQZpQmAVyxB4839819 = -986387201;    float WVotMfAAkdQZpQmAVyxB1179904 = -476217851;    float WVotMfAAkdQZpQmAVyxB89762128 = -492561987;    float WVotMfAAkdQZpQmAVyxB20832573 = -294101462;    float WVotMfAAkdQZpQmAVyxB18176139 = -25760987;    float WVotMfAAkdQZpQmAVyxB84007029 = -825561244;    float WVotMfAAkdQZpQmAVyxB39886662 = -537789827;    float WVotMfAAkdQZpQmAVyxB35585242 = -923675102;    float WVotMfAAkdQZpQmAVyxB59913690 = -330049002;    float WVotMfAAkdQZpQmAVyxB42590839 = -478961924;    float WVotMfAAkdQZpQmAVyxB62672521 = -298560416;    float WVotMfAAkdQZpQmAVyxB5008146 = -140835252;    float WVotMfAAkdQZpQmAVyxB48965296 = 83538411;    float WVotMfAAkdQZpQmAVyxB24066909 = -442355742;    float WVotMfAAkdQZpQmAVyxB92028734 = -366944238;    float WVotMfAAkdQZpQmAVyxB87183239 = -953803148;    float WVotMfAAkdQZpQmAVyxB95042298 = -498076629;    float WVotMfAAkdQZpQmAVyxB18437629 = -400345750;    float WVotMfAAkdQZpQmAVyxB30225196 = -289782017;    float WVotMfAAkdQZpQmAVyxB32921351 = -405130203;    float WVotMfAAkdQZpQmAVyxB9697017 = -981058200;    float WVotMfAAkdQZpQmAVyxB38350908 = -970830631;    float WVotMfAAkdQZpQmAVyxB29203244 = -871798171;    float WVotMfAAkdQZpQmAVyxB24613699 = -985935662;    float WVotMfAAkdQZpQmAVyxB57169611 = -854444128;    float WVotMfAAkdQZpQmAVyxB53076908 = -5706432;    float WVotMfAAkdQZpQmAVyxB27683165 = -911734789;    float WVotMfAAkdQZpQmAVyxB12536036 = -844822061;    float WVotMfAAkdQZpQmAVyxB87925484 = -311196771;    float WVotMfAAkdQZpQmAVyxB90454105 = -823080444;    float WVotMfAAkdQZpQmAVyxB7126808 = -198958085;    float WVotMfAAkdQZpQmAVyxB43074535 = -597927678;    float WVotMfAAkdQZpQmAVyxB62044091 = 34025678;    float WVotMfAAkdQZpQmAVyxB51053654 = -688984193;    float WVotMfAAkdQZpQmAVyxB94638621 = -870964482;    float WVotMfAAkdQZpQmAVyxB25386521 = -109322599;     WVotMfAAkdQZpQmAVyxB69006186 = WVotMfAAkdQZpQmAVyxB23171273;     WVotMfAAkdQZpQmAVyxB23171273 = WVotMfAAkdQZpQmAVyxB41438700;     WVotMfAAkdQZpQmAVyxB41438700 = WVotMfAAkdQZpQmAVyxB39879031;     WVotMfAAkdQZpQmAVyxB39879031 = WVotMfAAkdQZpQmAVyxB21817537;     WVotMfAAkdQZpQmAVyxB21817537 = WVotMfAAkdQZpQmAVyxB15508131;     WVotMfAAkdQZpQmAVyxB15508131 = WVotMfAAkdQZpQmAVyxB81621242;     WVotMfAAkdQZpQmAVyxB81621242 = WVotMfAAkdQZpQmAVyxB99555351;     WVotMfAAkdQZpQmAVyxB99555351 = WVotMfAAkdQZpQmAVyxB12043390;     WVotMfAAkdQZpQmAVyxB12043390 = WVotMfAAkdQZpQmAVyxB38733649;     WVotMfAAkdQZpQmAVyxB38733649 = WVotMfAAkdQZpQmAVyxB8306458;     WVotMfAAkdQZpQmAVyxB8306458 = WVotMfAAkdQZpQmAVyxB35017108;     WVotMfAAkdQZpQmAVyxB35017108 = WVotMfAAkdQZpQmAVyxB27211911;     WVotMfAAkdQZpQmAVyxB27211911 = WVotMfAAkdQZpQmAVyxB65746543;     WVotMfAAkdQZpQmAVyxB65746543 = WVotMfAAkdQZpQmAVyxB11316119;     WVotMfAAkdQZpQmAVyxB11316119 = WVotMfAAkdQZpQmAVyxB75266239;     WVotMfAAkdQZpQmAVyxB75266239 = WVotMfAAkdQZpQmAVyxB5473716;     WVotMfAAkdQZpQmAVyxB5473716 = WVotMfAAkdQZpQmAVyxB30969887;     WVotMfAAkdQZpQmAVyxB30969887 = WVotMfAAkdQZpQmAVyxB93897123;     WVotMfAAkdQZpQmAVyxB93897123 = WVotMfAAkdQZpQmAVyxB61170782;     WVotMfAAkdQZpQmAVyxB61170782 = WVotMfAAkdQZpQmAVyxB21705385;     WVotMfAAkdQZpQmAVyxB21705385 = WVotMfAAkdQZpQmAVyxB74512690;     WVotMfAAkdQZpQmAVyxB74512690 = WVotMfAAkdQZpQmAVyxB23545349;     WVotMfAAkdQZpQmAVyxB23545349 = WVotMfAAkdQZpQmAVyxB18595158;     WVotMfAAkdQZpQmAVyxB18595158 = WVotMfAAkdQZpQmAVyxB57916041;     WVotMfAAkdQZpQmAVyxB57916041 = WVotMfAAkdQZpQmAVyxB41215446;     WVotMfAAkdQZpQmAVyxB41215446 = WVotMfAAkdQZpQmAVyxB71328654;     WVotMfAAkdQZpQmAVyxB71328654 = WVotMfAAkdQZpQmAVyxB26197517;     WVotMfAAkdQZpQmAVyxB26197517 = WVotMfAAkdQZpQmAVyxB44902203;     WVotMfAAkdQZpQmAVyxB44902203 = WVotMfAAkdQZpQmAVyxB37549649;     WVotMfAAkdQZpQmAVyxB37549649 = WVotMfAAkdQZpQmAVyxB33893830;     WVotMfAAkdQZpQmAVyxB33893830 = WVotMfAAkdQZpQmAVyxB7126554;     WVotMfAAkdQZpQmAVyxB7126554 = WVotMfAAkdQZpQmAVyxB45254979;     WVotMfAAkdQZpQmAVyxB45254979 = WVotMfAAkdQZpQmAVyxB6379339;     WVotMfAAkdQZpQmAVyxB6379339 = WVotMfAAkdQZpQmAVyxB47570405;     WVotMfAAkdQZpQmAVyxB47570405 = WVotMfAAkdQZpQmAVyxB27309089;     WVotMfAAkdQZpQmAVyxB27309089 = WVotMfAAkdQZpQmAVyxB35379578;     WVotMfAAkdQZpQmAVyxB35379578 = WVotMfAAkdQZpQmAVyxB69888474;     WVotMfAAkdQZpQmAVyxB69888474 = WVotMfAAkdQZpQmAVyxB71056196;     WVotMfAAkdQZpQmAVyxB71056196 = WVotMfAAkdQZpQmAVyxB51306284;     WVotMfAAkdQZpQmAVyxB51306284 = WVotMfAAkdQZpQmAVyxB98498260;     WVotMfAAkdQZpQmAVyxB98498260 = WVotMfAAkdQZpQmAVyxB16697240;     WVotMfAAkdQZpQmAVyxB16697240 = WVotMfAAkdQZpQmAVyxB25547395;     WVotMfAAkdQZpQmAVyxB25547395 = WVotMfAAkdQZpQmAVyxB99478440;     WVotMfAAkdQZpQmAVyxB99478440 = WVotMfAAkdQZpQmAVyxB26566424;     WVotMfAAkdQZpQmAVyxB26566424 = WVotMfAAkdQZpQmAVyxB70732801;     WVotMfAAkdQZpQmAVyxB70732801 = WVotMfAAkdQZpQmAVyxB46173147;     WVotMfAAkdQZpQmAVyxB46173147 = WVotMfAAkdQZpQmAVyxB52891026;     WVotMfAAkdQZpQmAVyxB52891026 = WVotMfAAkdQZpQmAVyxB95972321;     WVotMfAAkdQZpQmAVyxB95972321 = WVotMfAAkdQZpQmAVyxB11980852;     WVotMfAAkdQZpQmAVyxB11980852 = WVotMfAAkdQZpQmAVyxB27852632;     WVotMfAAkdQZpQmAVyxB27852632 = WVotMfAAkdQZpQmAVyxB95542921;     WVotMfAAkdQZpQmAVyxB95542921 = WVotMfAAkdQZpQmAVyxB77923310;     WVotMfAAkdQZpQmAVyxB77923310 = WVotMfAAkdQZpQmAVyxB20641281;     WVotMfAAkdQZpQmAVyxB20641281 = WVotMfAAkdQZpQmAVyxB49209728;     WVotMfAAkdQZpQmAVyxB49209728 = WVotMfAAkdQZpQmAVyxB94493496;     WVotMfAAkdQZpQmAVyxB94493496 = WVotMfAAkdQZpQmAVyxB99625923;     WVotMfAAkdQZpQmAVyxB99625923 = WVotMfAAkdQZpQmAVyxB22843542;     WVotMfAAkdQZpQmAVyxB22843542 = WVotMfAAkdQZpQmAVyxB81962989;     WVotMfAAkdQZpQmAVyxB81962989 = WVotMfAAkdQZpQmAVyxB80602091;     WVotMfAAkdQZpQmAVyxB80602091 = WVotMfAAkdQZpQmAVyxB44179477;     WVotMfAAkdQZpQmAVyxB44179477 = WVotMfAAkdQZpQmAVyxB55423726;     WVotMfAAkdQZpQmAVyxB55423726 = WVotMfAAkdQZpQmAVyxB54653148;     WVotMfAAkdQZpQmAVyxB54653148 = WVotMfAAkdQZpQmAVyxB74493740;     WVotMfAAkdQZpQmAVyxB74493740 = WVotMfAAkdQZpQmAVyxB4839819;     WVotMfAAkdQZpQmAVyxB4839819 = WVotMfAAkdQZpQmAVyxB1179904;     WVotMfAAkdQZpQmAVyxB1179904 = WVotMfAAkdQZpQmAVyxB89762128;     WVotMfAAkdQZpQmAVyxB89762128 = WVotMfAAkdQZpQmAVyxB20832573;     WVotMfAAkdQZpQmAVyxB20832573 = WVotMfAAkdQZpQmAVyxB18176139;     WVotMfAAkdQZpQmAVyxB18176139 = WVotMfAAkdQZpQmAVyxB84007029;     WVotMfAAkdQZpQmAVyxB84007029 = WVotMfAAkdQZpQmAVyxB39886662;     WVotMfAAkdQZpQmAVyxB39886662 = WVotMfAAkdQZpQmAVyxB35585242;     WVotMfAAkdQZpQmAVyxB35585242 = WVotMfAAkdQZpQmAVyxB59913690;     WVotMfAAkdQZpQmAVyxB59913690 = WVotMfAAkdQZpQmAVyxB42590839;     WVotMfAAkdQZpQmAVyxB42590839 = WVotMfAAkdQZpQmAVyxB62672521;     WVotMfAAkdQZpQmAVyxB62672521 = WVotMfAAkdQZpQmAVyxB5008146;     WVotMfAAkdQZpQmAVyxB5008146 = WVotMfAAkdQZpQmAVyxB48965296;     WVotMfAAkdQZpQmAVyxB48965296 = WVotMfAAkdQZpQmAVyxB24066909;     WVotMfAAkdQZpQmAVyxB24066909 = WVotMfAAkdQZpQmAVyxB92028734;     WVotMfAAkdQZpQmAVyxB92028734 = WVotMfAAkdQZpQmAVyxB87183239;     WVotMfAAkdQZpQmAVyxB87183239 = WVotMfAAkdQZpQmAVyxB95042298;     WVotMfAAkdQZpQmAVyxB95042298 = WVotMfAAkdQZpQmAVyxB18437629;     WVotMfAAkdQZpQmAVyxB18437629 = WVotMfAAkdQZpQmAVyxB30225196;     WVotMfAAkdQZpQmAVyxB30225196 = WVotMfAAkdQZpQmAVyxB32921351;     WVotMfAAkdQZpQmAVyxB32921351 = WVotMfAAkdQZpQmAVyxB9697017;     WVotMfAAkdQZpQmAVyxB9697017 = WVotMfAAkdQZpQmAVyxB38350908;     WVotMfAAkdQZpQmAVyxB38350908 = WVotMfAAkdQZpQmAVyxB29203244;     WVotMfAAkdQZpQmAVyxB29203244 = WVotMfAAkdQZpQmAVyxB24613699;     WVotMfAAkdQZpQmAVyxB24613699 = WVotMfAAkdQZpQmAVyxB57169611;     WVotMfAAkdQZpQmAVyxB57169611 = WVotMfAAkdQZpQmAVyxB53076908;     WVotMfAAkdQZpQmAVyxB53076908 = WVotMfAAkdQZpQmAVyxB27683165;     WVotMfAAkdQZpQmAVyxB27683165 = WVotMfAAkdQZpQmAVyxB12536036;     WVotMfAAkdQZpQmAVyxB12536036 = WVotMfAAkdQZpQmAVyxB87925484;     WVotMfAAkdQZpQmAVyxB87925484 = WVotMfAAkdQZpQmAVyxB90454105;     WVotMfAAkdQZpQmAVyxB90454105 = WVotMfAAkdQZpQmAVyxB7126808;     WVotMfAAkdQZpQmAVyxB7126808 = WVotMfAAkdQZpQmAVyxB43074535;     WVotMfAAkdQZpQmAVyxB43074535 = WVotMfAAkdQZpQmAVyxB62044091;     WVotMfAAkdQZpQmAVyxB62044091 = WVotMfAAkdQZpQmAVyxB51053654;     WVotMfAAkdQZpQmAVyxB51053654 = WVotMfAAkdQZpQmAVyxB94638621;     WVotMfAAkdQZpQmAVyxB94638621 = WVotMfAAkdQZpQmAVyxB25386521;     WVotMfAAkdQZpQmAVyxB25386521 = WVotMfAAkdQZpQmAVyxB69006186;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void SPfVtUmRIDRMCSHlsWsO25331326() {     float XbSnduTiCcCFeFgdEReU39657854 = -727471730;    float XbSnduTiCcCFeFgdEReU91844790 = -592896976;    float XbSnduTiCcCFeFgdEReU21407619 = 39569296;    float XbSnduTiCcCFeFgdEReU79810317 = 41479822;    float XbSnduTiCcCFeFgdEReU43267858 = 9629746;    float XbSnduTiCcCFeFgdEReU5507894 = -786639854;    float XbSnduTiCcCFeFgdEReU139 = -606854164;    float XbSnduTiCcCFeFgdEReU56397732 = -958159913;    float XbSnduTiCcCFeFgdEReU57908443 = -455690969;    float XbSnduTiCcCFeFgdEReU61700109 = -814877949;    float XbSnduTiCcCFeFgdEReU50555618 = -534091720;    float XbSnduTiCcCFeFgdEReU80563679 = -484614625;    float XbSnduTiCcCFeFgdEReU7039299 = -570574174;    float XbSnduTiCcCFeFgdEReU49593577 = -858450961;    float XbSnduTiCcCFeFgdEReU94036137 = -208592288;    float XbSnduTiCcCFeFgdEReU7794880 = -286823257;    float XbSnduTiCcCFeFgdEReU99326542 = -632001293;    float XbSnduTiCcCFeFgdEReU51263381 = -750116429;    float XbSnduTiCcCFeFgdEReU75604905 = -994423981;    float XbSnduTiCcCFeFgdEReU19957491 = -810988282;    float XbSnduTiCcCFeFgdEReU76841139 = -116935887;    float XbSnduTiCcCFeFgdEReU35908455 = -614652572;    float XbSnduTiCcCFeFgdEReU25489158 = -867550569;    float XbSnduTiCcCFeFgdEReU52444695 = -976309368;    float XbSnduTiCcCFeFgdEReU22040430 = -56342186;    float XbSnduTiCcCFeFgdEReU71813026 = -797427564;    float XbSnduTiCcCFeFgdEReU79589440 = -870920447;    float XbSnduTiCcCFeFgdEReU33979742 = -741068337;    float XbSnduTiCcCFeFgdEReU77044690 = -904253867;    float XbSnduTiCcCFeFgdEReU76721744 = -242817225;    float XbSnduTiCcCFeFgdEReU40965826 = -275829083;    float XbSnduTiCcCFeFgdEReU51638015 = -251269103;    float XbSnduTiCcCFeFgdEReU28200340 = -623250455;    float XbSnduTiCcCFeFgdEReU71744314 = -930465338;    float XbSnduTiCcCFeFgdEReU25594641 = -778608544;    float XbSnduTiCcCFeFgdEReU87302348 = -312507589;    float XbSnduTiCcCFeFgdEReU79171063 = -267643051;    float XbSnduTiCcCFeFgdEReU22680963 = -583254169;    float XbSnduTiCcCFeFgdEReU49589485 = -286629868;    float XbSnduTiCcCFeFgdEReU46409583 = -325882478;    float XbSnduTiCcCFeFgdEReU58571545 = -254080440;    float XbSnduTiCcCFeFgdEReU45180230 = -836810565;    float XbSnduTiCcCFeFgdEReU74514354 = -253659731;    float XbSnduTiCcCFeFgdEReU5986742 = -929326655;    float XbSnduTiCcCFeFgdEReU22520483 = -125610618;    float XbSnduTiCcCFeFgdEReU77138636 = -184971506;    float XbSnduTiCcCFeFgdEReU72304350 = -217136290;    float XbSnduTiCcCFeFgdEReU18505236 = -427527218;    float XbSnduTiCcCFeFgdEReU1413774 = -931926522;    float XbSnduTiCcCFeFgdEReU20408338 = -387774326;    float XbSnduTiCcCFeFgdEReU96041956 = -349936076;    float XbSnduTiCcCFeFgdEReU37414427 = -525784489;    float XbSnduTiCcCFeFgdEReU32998967 = 21028188;    float XbSnduTiCcCFeFgdEReU10868586 = -251234681;    float XbSnduTiCcCFeFgdEReU48490187 = -523432441;    float XbSnduTiCcCFeFgdEReU3749399 = -12819158;    float XbSnduTiCcCFeFgdEReU66355632 = -725346407;    float XbSnduTiCcCFeFgdEReU68962924 = 15878663;    float XbSnduTiCcCFeFgdEReU57769888 = -902177993;    float XbSnduTiCcCFeFgdEReU71454831 = -192942690;    float XbSnduTiCcCFeFgdEReU25918454 = -915719408;    float XbSnduTiCcCFeFgdEReU66020396 = -865785827;    float XbSnduTiCcCFeFgdEReU79353041 = 46093954;    float XbSnduTiCcCFeFgdEReU81186699 = -112873744;    float XbSnduTiCcCFeFgdEReU20734283 = -439048866;    float XbSnduTiCcCFeFgdEReU98917603 = -182822617;    float XbSnduTiCcCFeFgdEReU52363339 = -861364171;    float XbSnduTiCcCFeFgdEReU35294985 = -640108836;    float XbSnduTiCcCFeFgdEReU23998936 = 20157582;    float XbSnduTiCcCFeFgdEReU6733790 = -896084700;    float XbSnduTiCcCFeFgdEReU28623816 = 80819793;    float XbSnduTiCcCFeFgdEReU76645579 = 51252876;    float XbSnduTiCcCFeFgdEReU1673896 = -363486561;    float XbSnduTiCcCFeFgdEReU29195323 = -568541503;    float XbSnduTiCcCFeFgdEReU61385946 = -456907842;    float XbSnduTiCcCFeFgdEReU31660909 = -280125323;    float XbSnduTiCcCFeFgdEReU61394100 = -260992842;    float XbSnduTiCcCFeFgdEReU19502417 = -938223915;    float XbSnduTiCcCFeFgdEReU29924213 = -750698750;    float XbSnduTiCcCFeFgdEReU44901793 = -871370681;    float XbSnduTiCcCFeFgdEReU99508676 = -480291275;    float XbSnduTiCcCFeFgdEReU61084204 = -343393230;    float XbSnduTiCcCFeFgdEReU32565969 = -809141815;    float XbSnduTiCcCFeFgdEReU56636353 = -416479542;    float XbSnduTiCcCFeFgdEReU80679788 = -892881149;    float XbSnduTiCcCFeFgdEReU3551400 = -750044595;    float XbSnduTiCcCFeFgdEReU18639048 = -172297292;    float XbSnduTiCcCFeFgdEReU17331755 = -272015774;    float XbSnduTiCcCFeFgdEReU23254127 = -307032898;    float XbSnduTiCcCFeFgdEReU21845242 = -665789386;    float XbSnduTiCcCFeFgdEReU20946716 = -587161182;    float XbSnduTiCcCFeFgdEReU10208140 = -183521715;    float XbSnduTiCcCFeFgdEReU64911075 = -681076177;    float XbSnduTiCcCFeFgdEReU78134653 = 6312821;    float XbSnduTiCcCFeFgdEReU20491129 = -410163071;    float XbSnduTiCcCFeFgdEReU92551148 = -388294614;    float XbSnduTiCcCFeFgdEReU65827189 = -782904519;    float XbSnduTiCcCFeFgdEReU93327655 = -40785987;    float XbSnduTiCcCFeFgdEReU85252458 = -390277789;    float XbSnduTiCcCFeFgdEReU23602880 = -727471730;     XbSnduTiCcCFeFgdEReU39657854 = XbSnduTiCcCFeFgdEReU91844790;     XbSnduTiCcCFeFgdEReU91844790 = XbSnduTiCcCFeFgdEReU21407619;     XbSnduTiCcCFeFgdEReU21407619 = XbSnduTiCcCFeFgdEReU79810317;     XbSnduTiCcCFeFgdEReU79810317 = XbSnduTiCcCFeFgdEReU43267858;     XbSnduTiCcCFeFgdEReU43267858 = XbSnduTiCcCFeFgdEReU5507894;     XbSnduTiCcCFeFgdEReU5507894 = XbSnduTiCcCFeFgdEReU139;     XbSnduTiCcCFeFgdEReU139 = XbSnduTiCcCFeFgdEReU56397732;     XbSnduTiCcCFeFgdEReU56397732 = XbSnduTiCcCFeFgdEReU57908443;     XbSnduTiCcCFeFgdEReU57908443 = XbSnduTiCcCFeFgdEReU61700109;     XbSnduTiCcCFeFgdEReU61700109 = XbSnduTiCcCFeFgdEReU50555618;     XbSnduTiCcCFeFgdEReU50555618 = XbSnduTiCcCFeFgdEReU80563679;     XbSnduTiCcCFeFgdEReU80563679 = XbSnduTiCcCFeFgdEReU7039299;     XbSnduTiCcCFeFgdEReU7039299 = XbSnduTiCcCFeFgdEReU49593577;     XbSnduTiCcCFeFgdEReU49593577 = XbSnduTiCcCFeFgdEReU94036137;     XbSnduTiCcCFeFgdEReU94036137 = XbSnduTiCcCFeFgdEReU7794880;     XbSnduTiCcCFeFgdEReU7794880 = XbSnduTiCcCFeFgdEReU99326542;     XbSnduTiCcCFeFgdEReU99326542 = XbSnduTiCcCFeFgdEReU51263381;     XbSnduTiCcCFeFgdEReU51263381 = XbSnduTiCcCFeFgdEReU75604905;     XbSnduTiCcCFeFgdEReU75604905 = XbSnduTiCcCFeFgdEReU19957491;     XbSnduTiCcCFeFgdEReU19957491 = XbSnduTiCcCFeFgdEReU76841139;     XbSnduTiCcCFeFgdEReU76841139 = XbSnduTiCcCFeFgdEReU35908455;     XbSnduTiCcCFeFgdEReU35908455 = XbSnduTiCcCFeFgdEReU25489158;     XbSnduTiCcCFeFgdEReU25489158 = XbSnduTiCcCFeFgdEReU52444695;     XbSnduTiCcCFeFgdEReU52444695 = XbSnduTiCcCFeFgdEReU22040430;     XbSnduTiCcCFeFgdEReU22040430 = XbSnduTiCcCFeFgdEReU71813026;     XbSnduTiCcCFeFgdEReU71813026 = XbSnduTiCcCFeFgdEReU79589440;     XbSnduTiCcCFeFgdEReU79589440 = XbSnduTiCcCFeFgdEReU33979742;     XbSnduTiCcCFeFgdEReU33979742 = XbSnduTiCcCFeFgdEReU77044690;     XbSnduTiCcCFeFgdEReU77044690 = XbSnduTiCcCFeFgdEReU76721744;     XbSnduTiCcCFeFgdEReU76721744 = XbSnduTiCcCFeFgdEReU40965826;     XbSnduTiCcCFeFgdEReU40965826 = XbSnduTiCcCFeFgdEReU51638015;     XbSnduTiCcCFeFgdEReU51638015 = XbSnduTiCcCFeFgdEReU28200340;     XbSnduTiCcCFeFgdEReU28200340 = XbSnduTiCcCFeFgdEReU71744314;     XbSnduTiCcCFeFgdEReU71744314 = XbSnduTiCcCFeFgdEReU25594641;     XbSnduTiCcCFeFgdEReU25594641 = XbSnduTiCcCFeFgdEReU87302348;     XbSnduTiCcCFeFgdEReU87302348 = XbSnduTiCcCFeFgdEReU79171063;     XbSnduTiCcCFeFgdEReU79171063 = XbSnduTiCcCFeFgdEReU22680963;     XbSnduTiCcCFeFgdEReU22680963 = XbSnduTiCcCFeFgdEReU49589485;     XbSnduTiCcCFeFgdEReU49589485 = XbSnduTiCcCFeFgdEReU46409583;     XbSnduTiCcCFeFgdEReU46409583 = XbSnduTiCcCFeFgdEReU58571545;     XbSnduTiCcCFeFgdEReU58571545 = XbSnduTiCcCFeFgdEReU45180230;     XbSnduTiCcCFeFgdEReU45180230 = XbSnduTiCcCFeFgdEReU74514354;     XbSnduTiCcCFeFgdEReU74514354 = XbSnduTiCcCFeFgdEReU5986742;     XbSnduTiCcCFeFgdEReU5986742 = XbSnduTiCcCFeFgdEReU22520483;     XbSnduTiCcCFeFgdEReU22520483 = XbSnduTiCcCFeFgdEReU77138636;     XbSnduTiCcCFeFgdEReU77138636 = XbSnduTiCcCFeFgdEReU72304350;     XbSnduTiCcCFeFgdEReU72304350 = XbSnduTiCcCFeFgdEReU18505236;     XbSnduTiCcCFeFgdEReU18505236 = XbSnduTiCcCFeFgdEReU1413774;     XbSnduTiCcCFeFgdEReU1413774 = XbSnduTiCcCFeFgdEReU20408338;     XbSnduTiCcCFeFgdEReU20408338 = XbSnduTiCcCFeFgdEReU96041956;     XbSnduTiCcCFeFgdEReU96041956 = XbSnduTiCcCFeFgdEReU37414427;     XbSnduTiCcCFeFgdEReU37414427 = XbSnduTiCcCFeFgdEReU32998967;     XbSnduTiCcCFeFgdEReU32998967 = XbSnduTiCcCFeFgdEReU10868586;     XbSnduTiCcCFeFgdEReU10868586 = XbSnduTiCcCFeFgdEReU48490187;     XbSnduTiCcCFeFgdEReU48490187 = XbSnduTiCcCFeFgdEReU3749399;     XbSnduTiCcCFeFgdEReU3749399 = XbSnduTiCcCFeFgdEReU66355632;     XbSnduTiCcCFeFgdEReU66355632 = XbSnduTiCcCFeFgdEReU68962924;     XbSnduTiCcCFeFgdEReU68962924 = XbSnduTiCcCFeFgdEReU57769888;     XbSnduTiCcCFeFgdEReU57769888 = XbSnduTiCcCFeFgdEReU71454831;     XbSnduTiCcCFeFgdEReU71454831 = XbSnduTiCcCFeFgdEReU25918454;     XbSnduTiCcCFeFgdEReU25918454 = XbSnduTiCcCFeFgdEReU66020396;     XbSnduTiCcCFeFgdEReU66020396 = XbSnduTiCcCFeFgdEReU79353041;     XbSnduTiCcCFeFgdEReU79353041 = XbSnduTiCcCFeFgdEReU81186699;     XbSnduTiCcCFeFgdEReU81186699 = XbSnduTiCcCFeFgdEReU20734283;     XbSnduTiCcCFeFgdEReU20734283 = XbSnduTiCcCFeFgdEReU98917603;     XbSnduTiCcCFeFgdEReU98917603 = XbSnduTiCcCFeFgdEReU52363339;     XbSnduTiCcCFeFgdEReU52363339 = XbSnduTiCcCFeFgdEReU35294985;     XbSnduTiCcCFeFgdEReU35294985 = XbSnduTiCcCFeFgdEReU23998936;     XbSnduTiCcCFeFgdEReU23998936 = XbSnduTiCcCFeFgdEReU6733790;     XbSnduTiCcCFeFgdEReU6733790 = XbSnduTiCcCFeFgdEReU28623816;     XbSnduTiCcCFeFgdEReU28623816 = XbSnduTiCcCFeFgdEReU76645579;     XbSnduTiCcCFeFgdEReU76645579 = XbSnduTiCcCFeFgdEReU1673896;     XbSnduTiCcCFeFgdEReU1673896 = XbSnduTiCcCFeFgdEReU29195323;     XbSnduTiCcCFeFgdEReU29195323 = XbSnduTiCcCFeFgdEReU61385946;     XbSnduTiCcCFeFgdEReU61385946 = XbSnduTiCcCFeFgdEReU31660909;     XbSnduTiCcCFeFgdEReU31660909 = XbSnduTiCcCFeFgdEReU61394100;     XbSnduTiCcCFeFgdEReU61394100 = XbSnduTiCcCFeFgdEReU19502417;     XbSnduTiCcCFeFgdEReU19502417 = XbSnduTiCcCFeFgdEReU29924213;     XbSnduTiCcCFeFgdEReU29924213 = XbSnduTiCcCFeFgdEReU44901793;     XbSnduTiCcCFeFgdEReU44901793 = XbSnduTiCcCFeFgdEReU99508676;     XbSnduTiCcCFeFgdEReU99508676 = XbSnduTiCcCFeFgdEReU61084204;     XbSnduTiCcCFeFgdEReU61084204 = XbSnduTiCcCFeFgdEReU32565969;     XbSnduTiCcCFeFgdEReU32565969 = XbSnduTiCcCFeFgdEReU56636353;     XbSnduTiCcCFeFgdEReU56636353 = XbSnduTiCcCFeFgdEReU80679788;     XbSnduTiCcCFeFgdEReU80679788 = XbSnduTiCcCFeFgdEReU3551400;     XbSnduTiCcCFeFgdEReU3551400 = XbSnduTiCcCFeFgdEReU18639048;     XbSnduTiCcCFeFgdEReU18639048 = XbSnduTiCcCFeFgdEReU17331755;     XbSnduTiCcCFeFgdEReU17331755 = XbSnduTiCcCFeFgdEReU23254127;     XbSnduTiCcCFeFgdEReU23254127 = XbSnduTiCcCFeFgdEReU21845242;     XbSnduTiCcCFeFgdEReU21845242 = XbSnduTiCcCFeFgdEReU20946716;     XbSnduTiCcCFeFgdEReU20946716 = XbSnduTiCcCFeFgdEReU10208140;     XbSnduTiCcCFeFgdEReU10208140 = XbSnduTiCcCFeFgdEReU64911075;     XbSnduTiCcCFeFgdEReU64911075 = XbSnduTiCcCFeFgdEReU78134653;     XbSnduTiCcCFeFgdEReU78134653 = XbSnduTiCcCFeFgdEReU20491129;     XbSnduTiCcCFeFgdEReU20491129 = XbSnduTiCcCFeFgdEReU92551148;     XbSnduTiCcCFeFgdEReU92551148 = XbSnduTiCcCFeFgdEReU65827189;     XbSnduTiCcCFeFgdEReU65827189 = XbSnduTiCcCFeFgdEReU93327655;     XbSnduTiCcCFeFgdEReU93327655 = XbSnduTiCcCFeFgdEReU85252458;     XbSnduTiCcCFeFgdEReU85252458 = XbSnduTiCcCFeFgdEReU23602880;     XbSnduTiCcCFeFgdEReU23602880 = XbSnduTiCcCFeFgdEReU39657854;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wyHltFrTghzthLgJmZWf31990071() {     float EVAbRVpfLdjEvSgIbpTX39651382 = -599995871;    float EVAbRVpfLdjEvSgIbpTX30417107 = -635492101;    float EVAbRVpfLdjEvSgIbpTX22720831 = 82440682;    float EVAbRVpfLdjEvSgIbpTX64890178 = -879061008;    float EVAbRVpfLdjEvSgIbpTX52107960 = -458725576;    float EVAbRVpfLdjEvSgIbpTX40313326 = -486072666;    float EVAbRVpfLdjEvSgIbpTX68547474 = -311390631;    float EVAbRVpfLdjEvSgIbpTX25611782 = -662337231;    float EVAbRVpfLdjEvSgIbpTX40648608 = -169337039;    float EVAbRVpfLdjEvSgIbpTX58339623 = -451684820;    float EVAbRVpfLdjEvSgIbpTX70876223 = -493563792;    float EVAbRVpfLdjEvSgIbpTX94855356 = -220092988;    float EVAbRVpfLdjEvSgIbpTX38892212 = -592232649;    float EVAbRVpfLdjEvSgIbpTX3437797 = -103604013;    float EVAbRVpfLdjEvSgIbpTX39208247 = -171305140;    float EVAbRVpfLdjEvSgIbpTX82359426 = -619618907;    float EVAbRVpfLdjEvSgIbpTX55179302 = -451615863;    float EVAbRVpfLdjEvSgIbpTX74263914 = -47188051;    float EVAbRVpfLdjEvSgIbpTX96033175 = -512406248;    float EVAbRVpfLdjEvSgIbpTX92031253 = -323012733;    float EVAbRVpfLdjEvSgIbpTX56510564 = -927402261;    float EVAbRVpfLdjEvSgIbpTX14204721 = -24644455;    float EVAbRVpfLdjEvSgIbpTX62367554 = -226508411;    float EVAbRVpfLdjEvSgIbpTX96094803 = -857229890;    float EVAbRVpfLdjEvSgIbpTX50483119 = -483036039;    float EVAbRVpfLdjEvSgIbpTX88387103 = -705633493;    float EVAbRVpfLdjEvSgIbpTX49811021 = -71720160;    float EVAbRVpfLdjEvSgIbpTX69383267 = -250035326;    float EVAbRVpfLdjEvSgIbpTX89723137 = -450324182;    float EVAbRVpfLdjEvSgIbpTX52811615 = 81751401;    float EVAbRVpfLdjEvSgIbpTX74378505 = -662922317;    float EVAbRVpfLdjEvSgIbpTX36361872 = -643090865;    float EVAbRVpfLdjEvSgIbpTX13652764 = -825194671;    float EVAbRVpfLdjEvSgIbpTX72027151 = -807147071;    float EVAbRVpfLdjEvSgIbpTX7952679 = 62077532;    float EVAbRVpfLdjEvSgIbpTX30130538 = -392273335;    float EVAbRVpfLdjEvSgIbpTX91112431 = -367296429;    float EVAbRVpfLdjEvSgIbpTX92598642 = -578516848;    float EVAbRVpfLdjEvSgIbpTX84903082 = -999311285;    float EVAbRVpfLdjEvSgIbpTX18282476 = 64685836;    float EVAbRVpfLdjEvSgIbpTX46427212 = -342024339;    float EVAbRVpfLdjEvSgIbpTX30877891 = 69397353;    float EVAbRVpfLdjEvSgIbpTX34797714 = -812789985;    float EVAbRVpfLdjEvSgIbpTX65218639 = -486291098;    float EVAbRVpfLdjEvSgIbpTX6538535 = -598870020;    float EVAbRVpfLdjEvSgIbpTX17566605 = 2983824;    float EVAbRVpfLdjEvSgIbpTX2143 = -698416912;    float EVAbRVpfLdjEvSgIbpTX93888816 = 63711426;    float EVAbRVpfLdjEvSgIbpTX20072163 = -495690384;    float EVAbRVpfLdjEvSgIbpTX40453761 = 25200787;    float EVAbRVpfLdjEvSgIbpTX9301327 = 71508917;    float EVAbRVpfLdjEvSgIbpTX51961684 = -907930237;    float EVAbRVpfLdjEvSgIbpTX8447316 = -971025820;    float EVAbRVpfLdjEvSgIbpTX21118857 = -783154532;    float EVAbRVpfLdjEvSgIbpTX75479507 = -280331718;    float EVAbRVpfLdjEvSgIbpTX25446662 = -475351416;    float EVAbRVpfLdjEvSgIbpTX68049552 = -308983690;    float EVAbRVpfLdjEvSgIbpTX26626028 = -60329428;    float EVAbRVpfLdjEvSgIbpTX14407059 = -296024970;    float EVAbRVpfLdjEvSgIbpTX63720857 = -753092083;    float EVAbRVpfLdjEvSgIbpTX90502304 = -314352506;    float EVAbRVpfLdjEvSgIbpTX99164207 = 38644694;    float EVAbRVpfLdjEvSgIbpTX35888645 = -112013049;    float EVAbRVpfLdjEvSgIbpTX87836993 = -151088441;    float EVAbRVpfLdjEvSgIbpTX83961118 = -788762503;    float EVAbRVpfLdjEvSgIbpTX34514351 = -850472928;    float EVAbRVpfLdjEvSgIbpTX81202593 = -394898318;    float EVAbRVpfLdjEvSgIbpTX66865061 = -785085578;    float EVAbRVpfLdjEvSgIbpTX95485118 = -65681545;    float EVAbRVpfLdjEvSgIbpTX9077709 = -779031805;    float EVAbRVpfLdjEvSgIbpTX91246995 = -152322479;    float EVAbRVpfLdjEvSgIbpTX62580659 = -873099015;    float EVAbRVpfLdjEvSgIbpTX89360832 = -47876767;    float EVAbRVpfLdjEvSgIbpTX77750700 = -477092084;    float EVAbRVpfLdjEvSgIbpTX45604042 = -980988395;    float EVAbRVpfLdjEvSgIbpTX25632673 = -896799614;    float EVAbRVpfLdjEvSgIbpTX79407006 = -211854471;    float EVAbRVpfLdjEvSgIbpTX97148915 = -740217314;    float EVAbRVpfLdjEvSgIbpTX89556268 = -158359871;    float EVAbRVpfLdjEvSgIbpTX32916515 = -386019863;    float EVAbRVpfLdjEvSgIbpTX88384960 = 92783419;    float EVAbRVpfLdjEvSgIbpTX55922205 = -35431586;    float EVAbRVpfLdjEvSgIbpTX49311104 = -754344943;    float EVAbRVpfLdjEvSgIbpTX49269376 = -375524970;    float EVAbRVpfLdjEvSgIbpTX43510288 = -989757516;    float EVAbRVpfLdjEvSgIbpTX22416821 = -754992081;    float EVAbRVpfLdjEvSgIbpTX27914557 = -672065045;    float EVAbRVpfLdjEvSgIbpTX92533907 = 57959861;    float EVAbRVpfLdjEvSgIbpTX96547643 = -426815353;    float EVAbRVpfLdjEvSgIbpTX82506017 = -462571053;    float EVAbRVpfLdjEvSgIbpTX62080985 = 16710355;    float EVAbRVpfLdjEvSgIbpTX64486404 = -206967002;    float EVAbRVpfLdjEvSgIbpTX78191583 = -182491879;    float EVAbRVpfLdjEvSgIbpTX21182226 = -146219202;    float EVAbRVpfLdjEvSgIbpTX27780171 = -620961658;    float EVAbRVpfLdjEvSgIbpTX47263005 = -280669033;    float EVAbRVpfLdjEvSgIbpTX94989246 = -818589599;    float EVAbRVpfLdjEvSgIbpTX46960721 = -561701545;    float EVAbRVpfLdjEvSgIbpTX81257521 = -697528596;    float EVAbRVpfLdjEvSgIbpTX72024184 = -599995871;     EVAbRVpfLdjEvSgIbpTX39651382 = EVAbRVpfLdjEvSgIbpTX30417107;     EVAbRVpfLdjEvSgIbpTX30417107 = EVAbRVpfLdjEvSgIbpTX22720831;     EVAbRVpfLdjEvSgIbpTX22720831 = EVAbRVpfLdjEvSgIbpTX64890178;     EVAbRVpfLdjEvSgIbpTX64890178 = EVAbRVpfLdjEvSgIbpTX52107960;     EVAbRVpfLdjEvSgIbpTX52107960 = EVAbRVpfLdjEvSgIbpTX40313326;     EVAbRVpfLdjEvSgIbpTX40313326 = EVAbRVpfLdjEvSgIbpTX68547474;     EVAbRVpfLdjEvSgIbpTX68547474 = EVAbRVpfLdjEvSgIbpTX25611782;     EVAbRVpfLdjEvSgIbpTX25611782 = EVAbRVpfLdjEvSgIbpTX40648608;     EVAbRVpfLdjEvSgIbpTX40648608 = EVAbRVpfLdjEvSgIbpTX58339623;     EVAbRVpfLdjEvSgIbpTX58339623 = EVAbRVpfLdjEvSgIbpTX70876223;     EVAbRVpfLdjEvSgIbpTX70876223 = EVAbRVpfLdjEvSgIbpTX94855356;     EVAbRVpfLdjEvSgIbpTX94855356 = EVAbRVpfLdjEvSgIbpTX38892212;     EVAbRVpfLdjEvSgIbpTX38892212 = EVAbRVpfLdjEvSgIbpTX3437797;     EVAbRVpfLdjEvSgIbpTX3437797 = EVAbRVpfLdjEvSgIbpTX39208247;     EVAbRVpfLdjEvSgIbpTX39208247 = EVAbRVpfLdjEvSgIbpTX82359426;     EVAbRVpfLdjEvSgIbpTX82359426 = EVAbRVpfLdjEvSgIbpTX55179302;     EVAbRVpfLdjEvSgIbpTX55179302 = EVAbRVpfLdjEvSgIbpTX74263914;     EVAbRVpfLdjEvSgIbpTX74263914 = EVAbRVpfLdjEvSgIbpTX96033175;     EVAbRVpfLdjEvSgIbpTX96033175 = EVAbRVpfLdjEvSgIbpTX92031253;     EVAbRVpfLdjEvSgIbpTX92031253 = EVAbRVpfLdjEvSgIbpTX56510564;     EVAbRVpfLdjEvSgIbpTX56510564 = EVAbRVpfLdjEvSgIbpTX14204721;     EVAbRVpfLdjEvSgIbpTX14204721 = EVAbRVpfLdjEvSgIbpTX62367554;     EVAbRVpfLdjEvSgIbpTX62367554 = EVAbRVpfLdjEvSgIbpTX96094803;     EVAbRVpfLdjEvSgIbpTX96094803 = EVAbRVpfLdjEvSgIbpTX50483119;     EVAbRVpfLdjEvSgIbpTX50483119 = EVAbRVpfLdjEvSgIbpTX88387103;     EVAbRVpfLdjEvSgIbpTX88387103 = EVAbRVpfLdjEvSgIbpTX49811021;     EVAbRVpfLdjEvSgIbpTX49811021 = EVAbRVpfLdjEvSgIbpTX69383267;     EVAbRVpfLdjEvSgIbpTX69383267 = EVAbRVpfLdjEvSgIbpTX89723137;     EVAbRVpfLdjEvSgIbpTX89723137 = EVAbRVpfLdjEvSgIbpTX52811615;     EVAbRVpfLdjEvSgIbpTX52811615 = EVAbRVpfLdjEvSgIbpTX74378505;     EVAbRVpfLdjEvSgIbpTX74378505 = EVAbRVpfLdjEvSgIbpTX36361872;     EVAbRVpfLdjEvSgIbpTX36361872 = EVAbRVpfLdjEvSgIbpTX13652764;     EVAbRVpfLdjEvSgIbpTX13652764 = EVAbRVpfLdjEvSgIbpTX72027151;     EVAbRVpfLdjEvSgIbpTX72027151 = EVAbRVpfLdjEvSgIbpTX7952679;     EVAbRVpfLdjEvSgIbpTX7952679 = EVAbRVpfLdjEvSgIbpTX30130538;     EVAbRVpfLdjEvSgIbpTX30130538 = EVAbRVpfLdjEvSgIbpTX91112431;     EVAbRVpfLdjEvSgIbpTX91112431 = EVAbRVpfLdjEvSgIbpTX92598642;     EVAbRVpfLdjEvSgIbpTX92598642 = EVAbRVpfLdjEvSgIbpTX84903082;     EVAbRVpfLdjEvSgIbpTX84903082 = EVAbRVpfLdjEvSgIbpTX18282476;     EVAbRVpfLdjEvSgIbpTX18282476 = EVAbRVpfLdjEvSgIbpTX46427212;     EVAbRVpfLdjEvSgIbpTX46427212 = EVAbRVpfLdjEvSgIbpTX30877891;     EVAbRVpfLdjEvSgIbpTX30877891 = EVAbRVpfLdjEvSgIbpTX34797714;     EVAbRVpfLdjEvSgIbpTX34797714 = EVAbRVpfLdjEvSgIbpTX65218639;     EVAbRVpfLdjEvSgIbpTX65218639 = EVAbRVpfLdjEvSgIbpTX6538535;     EVAbRVpfLdjEvSgIbpTX6538535 = EVAbRVpfLdjEvSgIbpTX17566605;     EVAbRVpfLdjEvSgIbpTX17566605 = EVAbRVpfLdjEvSgIbpTX2143;     EVAbRVpfLdjEvSgIbpTX2143 = EVAbRVpfLdjEvSgIbpTX93888816;     EVAbRVpfLdjEvSgIbpTX93888816 = EVAbRVpfLdjEvSgIbpTX20072163;     EVAbRVpfLdjEvSgIbpTX20072163 = EVAbRVpfLdjEvSgIbpTX40453761;     EVAbRVpfLdjEvSgIbpTX40453761 = EVAbRVpfLdjEvSgIbpTX9301327;     EVAbRVpfLdjEvSgIbpTX9301327 = EVAbRVpfLdjEvSgIbpTX51961684;     EVAbRVpfLdjEvSgIbpTX51961684 = EVAbRVpfLdjEvSgIbpTX8447316;     EVAbRVpfLdjEvSgIbpTX8447316 = EVAbRVpfLdjEvSgIbpTX21118857;     EVAbRVpfLdjEvSgIbpTX21118857 = EVAbRVpfLdjEvSgIbpTX75479507;     EVAbRVpfLdjEvSgIbpTX75479507 = EVAbRVpfLdjEvSgIbpTX25446662;     EVAbRVpfLdjEvSgIbpTX25446662 = EVAbRVpfLdjEvSgIbpTX68049552;     EVAbRVpfLdjEvSgIbpTX68049552 = EVAbRVpfLdjEvSgIbpTX26626028;     EVAbRVpfLdjEvSgIbpTX26626028 = EVAbRVpfLdjEvSgIbpTX14407059;     EVAbRVpfLdjEvSgIbpTX14407059 = EVAbRVpfLdjEvSgIbpTX63720857;     EVAbRVpfLdjEvSgIbpTX63720857 = EVAbRVpfLdjEvSgIbpTX90502304;     EVAbRVpfLdjEvSgIbpTX90502304 = EVAbRVpfLdjEvSgIbpTX99164207;     EVAbRVpfLdjEvSgIbpTX99164207 = EVAbRVpfLdjEvSgIbpTX35888645;     EVAbRVpfLdjEvSgIbpTX35888645 = EVAbRVpfLdjEvSgIbpTX87836993;     EVAbRVpfLdjEvSgIbpTX87836993 = EVAbRVpfLdjEvSgIbpTX83961118;     EVAbRVpfLdjEvSgIbpTX83961118 = EVAbRVpfLdjEvSgIbpTX34514351;     EVAbRVpfLdjEvSgIbpTX34514351 = EVAbRVpfLdjEvSgIbpTX81202593;     EVAbRVpfLdjEvSgIbpTX81202593 = EVAbRVpfLdjEvSgIbpTX66865061;     EVAbRVpfLdjEvSgIbpTX66865061 = EVAbRVpfLdjEvSgIbpTX95485118;     EVAbRVpfLdjEvSgIbpTX95485118 = EVAbRVpfLdjEvSgIbpTX9077709;     EVAbRVpfLdjEvSgIbpTX9077709 = EVAbRVpfLdjEvSgIbpTX91246995;     EVAbRVpfLdjEvSgIbpTX91246995 = EVAbRVpfLdjEvSgIbpTX62580659;     EVAbRVpfLdjEvSgIbpTX62580659 = EVAbRVpfLdjEvSgIbpTX89360832;     EVAbRVpfLdjEvSgIbpTX89360832 = EVAbRVpfLdjEvSgIbpTX77750700;     EVAbRVpfLdjEvSgIbpTX77750700 = EVAbRVpfLdjEvSgIbpTX45604042;     EVAbRVpfLdjEvSgIbpTX45604042 = EVAbRVpfLdjEvSgIbpTX25632673;     EVAbRVpfLdjEvSgIbpTX25632673 = EVAbRVpfLdjEvSgIbpTX79407006;     EVAbRVpfLdjEvSgIbpTX79407006 = EVAbRVpfLdjEvSgIbpTX97148915;     EVAbRVpfLdjEvSgIbpTX97148915 = EVAbRVpfLdjEvSgIbpTX89556268;     EVAbRVpfLdjEvSgIbpTX89556268 = EVAbRVpfLdjEvSgIbpTX32916515;     EVAbRVpfLdjEvSgIbpTX32916515 = EVAbRVpfLdjEvSgIbpTX88384960;     EVAbRVpfLdjEvSgIbpTX88384960 = EVAbRVpfLdjEvSgIbpTX55922205;     EVAbRVpfLdjEvSgIbpTX55922205 = EVAbRVpfLdjEvSgIbpTX49311104;     EVAbRVpfLdjEvSgIbpTX49311104 = EVAbRVpfLdjEvSgIbpTX49269376;     EVAbRVpfLdjEvSgIbpTX49269376 = EVAbRVpfLdjEvSgIbpTX43510288;     EVAbRVpfLdjEvSgIbpTX43510288 = EVAbRVpfLdjEvSgIbpTX22416821;     EVAbRVpfLdjEvSgIbpTX22416821 = EVAbRVpfLdjEvSgIbpTX27914557;     EVAbRVpfLdjEvSgIbpTX27914557 = EVAbRVpfLdjEvSgIbpTX92533907;     EVAbRVpfLdjEvSgIbpTX92533907 = EVAbRVpfLdjEvSgIbpTX96547643;     EVAbRVpfLdjEvSgIbpTX96547643 = EVAbRVpfLdjEvSgIbpTX82506017;     EVAbRVpfLdjEvSgIbpTX82506017 = EVAbRVpfLdjEvSgIbpTX62080985;     EVAbRVpfLdjEvSgIbpTX62080985 = EVAbRVpfLdjEvSgIbpTX64486404;     EVAbRVpfLdjEvSgIbpTX64486404 = EVAbRVpfLdjEvSgIbpTX78191583;     EVAbRVpfLdjEvSgIbpTX78191583 = EVAbRVpfLdjEvSgIbpTX21182226;     EVAbRVpfLdjEvSgIbpTX21182226 = EVAbRVpfLdjEvSgIbpTX27780171;     EVAbRVpfLdjEvSgIbpTX27780171 = EVAbRVpfLdjEvSgIbpTX47263005;     EVAbRVpfLdjEvSgIbpTX47263005 = EVAbRVpfLdjEvSgIbpTX94989246;     EVAbRVpfLdjEvSgIbpTX94989246 = EVAbRVpfLdjEvSgIbpTX46960721;     EVAbRVpfLdjEvSgIbpTX46960721 = EVAbRVpfLdjEvSgIbpTX81257521;     EVAbRVpfLdjEvSgIbpTX81257521 = EVAbRVpfLdjEvSgIbpTX72024184;     EVAbRVpfLdjEvSgIbpTX72024184 = EVAbRVpfLdjEvSgIbpTX39651382;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void XYXlBEWLPoVQjbQlkcwT82919641() {     float bjpAHTBQFDqokTWkthDL15238277 = -881897936;    float bjpAHTBQFDqokTWkthDL92171171 = -880321097;    float bjpAHTBQFDqokTWkthDL80271703 = -5738700;    float bjpAHTBQFDqokTWkthDL14342143 = -656248694;    float bjpAHTBQFDqokTWkthDL19239940 = -938559032;    float bjpAHTBQFDqokTWkthDL48621244 = -716195466;    float bjpAHTBQFDqokTWkthDL31731485 = -142527721;    float bjpAHTBQFDqokTWkthDL73318394 = -566974780;    float bjpAHTBQFDqokTWkthDL95292859 = -462149566;    float bjpAHTBQFDqokTWkthDL78338127 = -52044625;    float bjpAHTBQFDqokTWkthDL80129626 = -264123661;    float bjpAHTBQFDqokTWkthDL26600093 = -179932304;    float bjpAHTBQFDqokTWkthDL57509761 = -626066395;    float bjpAHTBQFDqokTWkthDL6536459 = -541840043;    float bjpAHTBQFDqokTWkthDL34184108 = -69172885;    float bjpAHTBQFDqokTWkthDL96439332 = -697611472;    float bjpAHTBQFDqokTWkthDL41014228 = -735032015;    float bjpAHTBQFDqokTWkthDL14629686 = -739197506;    float bjpAHTBQFDqokTWkthDL77390828 = -590884538;    float bjpAHTBQFDqokTWkthDL85174279 = -694445996;    float bjpAHTBQFDqokTWkthDL37602473 = -911485448;    float bjpAHTBQFDqokTWkthDL71133761 = -591879414;    float bjpAHTBQFDqokTWkthDL56911753 = -430861056;    float bjpAHTBQFDqokTWkthDL36896319 = -195848739;    float bjpAHTBQFDqokTWkthDL23275587 = -768858718;    float bjpAHTBQFDqokTWkthDL88732720 = -843032552;    float bjpAHTBQFDqokTWkthDL39643535 = -662333366;    float bjpAHTBQFDqokTWkthDL24431186 = -877455811;    float bjpAHTBQFDqokTWkthDL44430738 = -800100454;    float bjpAHTBQFDqokTWkthDL12111638 = 91699683;    float bjpAHTBQFDqokTWkthDL88071969 = -894796039;    float bjpAHTBQFDqokTWkthDL77370840 = -268931258;    float bjpAHTBQFDqokTWkthDL63476709 = -397529754;    float bjpAHTBQFDqokTWkthDL25543513 = -69121609;    float bjpAHTBQFDqokTWkthDL38768158 = -574529899;    float bjpAHTBQFDqokTWkthDL24215341 = -614926614;    float bjpAHTBQFDqokTWkthDL62772475 = 2104971;    float bjpAHTBQFDqokTWkthDL691910 = -428416004;    float bjpAHTBQFDqokTWkthDL66998261 = 95520747;    float bjpAHTBQFDqokTWkthDL34097926 = -777926562;    float bjpAHTBQFDqokTWkthDL96614576 = -70098554;    float bjpAHTBQFDqokTWkthDL78463066 = -391193130;    float bjpAHTBQFDqokTWkthDL93927093 = -151613803;    float bjpAHTBQFDqokTWkthDL96203944 = 34534414;    float bjpAHTBQFDqokTWkthDL27514898 = -694576287;    float bjpAHTBQFDqokTWkthDL73859935 = -841552546;    float bjpAHTBQFDqokTWkthDL42846149 = -60565617;    float bjpAHTBQFDqokTWkthDL58155692 = -252281082;    float bjpAHTBQFDqokTWkthDL41218886 = -429425645;    float bjpAHTBQFDqokTWkthDL39252401 = 73933856;    float bjpAHTBQFDqokTWkthDL66122731 = -282510934;    float bjpAHTBQFDqokTWkthDL25583449 = -715472979;    float bjpAHTBQFDqokTWkthDL82658832 = -190910867;    float bjpAHTBQFDqokTWkthDL37455197 = -140238351;    float bjpAHTBQFDqokTWkthDL45317168 = -816197316;    float bjpAHTBQFDqokTWkthDL44104516 = -190018523;    float bjpAHTBQFDqokTWkthDL35259418 = -349460041;    float bjpAHTBQFDqokTWkthDL43375385 = -809889962;    float bjpAHTBQFDqokTWkthDL91066556 = -887389977;    float bjpAHTBQFDqokTWkthDL30507219 = 4473519;    float bjpAHTBQFDqokTWkthDL8977710 = 46137900;    float bjpAHTBQFDqokTWkthDL7300299 = -265071910;    float bjpAHTBQFDqokTWkthDL28887656 = -766874327;    float bjpAHTBQFDqokTWkthDL83181221 = -453849249;    float bjpAHTBQFDqokTWkthDL90266157 = -157248586;    float bjpAHTBQFDqokTWkthDL2758786 = -995192403;    float bjpAHTBQFDqokTWkthDL63123383 = -782402551;    float bjpAHTBQFDqokTWkthDL31966249 = -456944787;    float bjpAHTBQFDqokTWkthDL67768301 = -967310145;    float bjpAHTBQFDqokTWkthDL9968767 = -454246271;    float bjpAHTBQFDqokTWkthDL33666858 = -599716443;    float bjpAHTBQFDqokTWkthDL40322319 = -206616012;    float bjpAHTBQFDqokTWkthDL47631424 = -734718254;    float bjpAHTBQFDqokTWkthDL43292903 = -812957977;    float bjpAHTBQFDqokTWkthDL88559703 = -524347443;    float bjpAHTBQFDqokTWkthDL59139407 = -420292319;    float bjpAHTBQFDqokTWkthDL77206667 = -340265612;    float bjpAHTBQFDqokTWkthDL60707809 = -365395471;    float bjpAHTBQFDqokTWkthDL9381421 = -501272453;    float bjpAHTBQFDqokTWkthDL49415651 = -927306172;    float bjpAHTBQFDqokTWkthDL45886572 = -682466936;    float bjpAHTBQFDqokTWkthDL81487843 = -310052285;    float bjpAHTBQFDqokTWkthDL83212300 = -348030167;    float bjpAHTBQFDqokTWkthDL5178338 = -774034310;    float bjpAHTBQFDqokTWkthDL45988907 = -625789384;    float bjpAHTBQFDqokTWkthDL62488520 = -79323061;    float bjpAHTBQFDqokTWkthDL94712008 = 21979608;    float bjpAHTBQFDqokTWkthDL26021512 = -157291403;    float bjpAHTBQFDqokTWkthDL80226344 = -252924293;    float bjpAHTBQFDqokTWkthDL94663641 = -284511376;    float bjpAHTBQFDqokTWkthDL88955922 = -165466574;    float bjpAHTBQFDqokTWkthDL19397090 = -188005068;    float bjpAHTBQFDqokTWkthDL9625353 = -541026027;    float bjpAHTBQFDqokTWkthDL36491043 = -908952772;    float bjpAHTBQFDqokTWkthDL25120217 = -724064462;    float bjpAHTBQFDqokTWkthDL89314277 = -805026644;    float bjpAHTBQFDqokTWkthDL49575411 = -624318804;    float bjpAHTBQFDqokTWkthDL10745873 = -697764554;    float bjpAHTBQFDqokTWkthDL5937787 = -808217000;    float bjpAHTBQFDqokTWkthDL24756113 = -881897936;     bjpAHTBQFDqokTWkthDL15238277 = bjpAHTBQFDqokTWkthDL92171171;     bjpAHTBQFDqokTWkthDL92171171 = bjpAHTBQFDqokTWkthDL80271703;     bjpAHTBQFDqokTWkthDL80271703 = bjpAHTBQFDqokTWkthDL14342143;     bjpAHTBQFDqokTWkthDL14342143 = bjpAHTBQFDqokTWkthDL19239940;     bjpAHTBQFDqokTWkthDL19239940 = bjpAHTBQFDqokTWkthDL48621244;     bjpAHTBQFDqokTWkthDL48621244 = bjpAHTBQFDqokTWkthDL31731485;     bjpAHTBQFDqokTWkthDL31731485 = bjpAHTBQFDqokTWkthDL73318394;     bjpAHTBQFDqokTWkthDL73318394 = bjpAHTBQFDqokTWkthDL95292859;     bjpAHTBQFDqokTWkthDL95292859 = bjpAHTBQFDqokTWkthDL78338127;     bjpAHTBQFDqokTWkthDL78338127 = bjpAHTBQFDqokTWkthDL80129626;     bjpAHTBQFDqokTWkthDL80129626 = bjpAHTBQFDqokTWkthDL26600093;     bjpAHTBQFDqokTWkthDL26600093 = bjpAHTBQFDqokTWkthDL57509761;     bjpAHTBQFDqokTWkthDL57509761 = bjpAHTBQFDqokTWkthDL6536459;     bjpAHTBQFDqokTWkthDL6536459 = bjpAHTBQFDqokTWkthDL34184108;     bjpAHTBQFDqokTWkthDL34184108 = bjpAHTBQFDqokTWkthDL96439332;     bjpAHTBQFDqokTWkthDL96439332 = bjpAHTBQFDqokTWkthDL41014228;     bjpAHTBQFDqokTWkthDL41014228 = bjpAHTBQFDqokTWkthDL14629686;     bjpAHTBQFDqokTWkthDL14629686 = bjpAHTBQFDqokTWkthDL77390828;     bjpAHTBQFDqokTWkthDL77390828 = bjpAHTBQFDqokTWkthDL85174279;     bjpAHTBQFDqokTWkthDL85174279 = bjpAHTBQFDqokTWkthDL37602473;     bjpAHTBQFDqokTWkthDL37602473 = bjpAHTBQFDqokTWkthDL71133761;     bjpAHTBQFDqokTWkthDL71133761 = bjpAHTBQFDqokTWkthDL56911753;     bjpAHTBQFDqokTWkthDL56911753 = bjpAHTBQFDqokTWkthDL36896319;     bjpAHTBQFDqokTWkthDL36896319 = bjpAHTBQFDqokTWkthDL23275587;     bjpAHTBQFDqokTWkthDL23275587 = bjpAHTBQFDqokTWkthDL88732720;     bjpAHTBQFDqokTWkthDL88732720 = bjpAHTBQFDqokTWkthDL39643535;     bjpAHTBQFDqokTWkthDL39643535 = bjpAHTBQFDqokTWkthDL24431186;     bjpAHTBQFDqokTWkthDL24431186 = bjpAHTBQFDqokTWkthDL44430738;     bjpAHTBQFDqokTWkthDL44430738 = bjpAHTBQFDqokTWkthDL12111638;     bjpAHTBQFDqokTWkthDL12111638 = bjpAHTBQFDqokTWkthDL88071969;     bjpAHTBQFDqokTWkthDL88071969 = bjpAHTBQFDqokTWkthDL77370840;     bjpAHTBQFDqokTWkthDL77370840 = bjpAHTBQFDqokTWkthDL63476709;     bjpAHTBQFDqokTWkthDL63476709 = bjpAHTBQFDqokTWkthDL25543513;     bjpAHTBQFDqokTWkthDL25543513 = bjpAHTBQFDqokTWkthDL38768158;     bjpAHTBQFDqokTWkthDL38768158 = bjpAHTBQFDqokTWkthDL24215341;     bjpAHTBQFDqokTWkthDL24215341 = bjpAHTBQFDqokTWkthDL62772475;     bjpAHTBQFDqokTWkthDL62772475 = bjpAHTBQFDqokTWkthDL691910;     bjpAHTBQFDqokTWkthDL691910 = bjpAHTBQFDqokTWkthDL66998261;     bjpAHTBQFDqokTWkthDL66998261 = bjpAHTBQFDqokTWkthDL34097926;     bjpAHTBQFDqokTWkthDL34097926 = bjpAHTBQFDqokTWkthDL96614576;     bjpAHTBQFDqokTWkthDL96614576 = bjpAHTBQFDqokTWkthDL78463066;     bjpAHTBQFDqokTWkthDL78463066 = bjpAHTBQFDqokTWkthDL93927093;     bjpAHTBQFDqokTWkthDL93927093 = bjpAHTBQFDqokTWkthDL96203944;     bjpAHTBQFDqokTWkthDL96203944 = bjpAHTBQFDqokTWkthDL27514898;     bjpAHTBQFDqokTWkthDL27514898 = bjpAHTBQFDqokTWkthDL73859935;     bjpAHTBQFDqokTWkthDL73859935 = bjpAHTBQFDqokTWkthDL42846149;     bjpAHTBQFDqokTWkthDL42846149 = bjpAHTBQFDqokTWkthDL58155692;     bjpAHTBQFDqokTWkthDL58155692 = bjpAHTBQFDqokTWkthDL41218886;     bjpAHTBQFDqokTWkthDL41218886 = bjpAHTBQFDqokTWkthDL39252401;     bjpAHTBQFDqokTWkthDL39252401 = bjpAHTBQFDqokTWkthDL66122731;     bjpAHTBQFDqokTWkthDL66122731 = bjpAHTBQFDqokTWkthDL25583449;     bjpAHTBQFDqokTWkthDL25583449 = bjpAHTBQFDqokTWkthDL82658832;     bjpAHTBQFDqokTWkthDL82658832 = bjpAHTBQFDqokTWkthDL37455197;     bjpAHTBQFDqokTWkthDL37455197 = bjpAHTBQFDqokTWkthDL45317168;     bjpAHTBQFDqokTWkthDL45317168 = bjpAHTBQFDqokTWkthDL44104516;     bjpAHTBQFDqokTWkthDL44104516 = bjpAHTBQFDqokTWkthDL35259418;     bjpAHTBQFDqokTWkthDL35259418 = bjpAHTBQFDqokTWkthDL43375385;     bjpAHTBQFDqokTWkthDL43375385 = bjpAHTBQFDqokTWkthDL91066556;     bjpAHTBQFDqokTWkthDL91066556 = bjpAHTBQFDqokTWkthDL30507219;     bjpAHTBQFDqokTWkthDL30507219 = bjpAHTBQFDqokTWkthDL8977710;     bjpAHTBQFDqokTWkthDL8977710 = bjpAHTBQFDqokTWkthDL7300299;     bjpAHTBQFDqokTWkthDL7300299 = bjpAHTBQFDqokTWkthDL28887656;     bjpAHTBQFDqokTWkthDL28887656 = bjpAHTBQFDqokTWkthDL83181221;     bjpAHTBQFDqokTWkthDL83181221 = bjpAHTBQFDqokTWkthDL90266157;     bjpAHTBQFDqokTWkthDL90266157 = bjpAHTBQFDqokTWkthDL2758786;     bjpAHTBQFDqokTWkthDL2758786 = bjpAHTBQFDqokTWkthDL63123383;     bjpAHTBQFDqokTWkthDL63123383 = bjpAHTBQFDqokTWkthDL31966249;     bjpAHTBQFDqokTWkthDL31966249 = bjpAHTBQFDqokTWkthDL67768301;     bjpAHTBQFDqokTWkthDL67768301 = bjpAHTBQFDqokTWkthDL9968767;     bjpAHTBQFDqokTWkthDL9968767 = bjpAHTBQFDqokTWkthDL33666858;     bjpAHTBQFDqokTWkthDL33666858 = bjpAHTBQFDqokTWkthDL40322319;     bjpAHTBQFDqokTWkthDL40322319 = bjpAHTBQFDqokTWkthDL47631424;     bjpAHTBQFDqokTWkthDL47631424 = bjpAHTBQFDqokTWkthDL43292903;     bjpAHTBQFDqokTWkthDL43292903 = bjpAHTBQFDqokTWkthDL88559703;     bjpAHTBQFDqokTWkthDL88559703 = bjpAHTBQFDqokTWkthDL59139407;     bjpAHTBQFDqokTWkthDL59139407 = bjpAHTBQFDqokTWkthDL77206667;     bjpAHTBQFDqokTWkthDL77206667 = bjpAHTBQFDqokTWkthDL60707809;     bjpAHTBQFDqokTWkthDL60707809 = bjpAHTBQFDqokTWkthDL9381421;     bjpAHTBQFDqokTWkthDL9381421 = bjpAHTBQFDqokTWkthDL49415651;     bjpAHTBQFDqokTWkthDL49415651 = bjpAHTBQFDqokTWkthDL45886572;     bjpAHTBQFDqokTWkthDL45886572 = bjpAHTBQFDqokTWkthDL81487843;     bjpAHTBQFDqokTWkthDL81487843 = bjpAHTBQFDqokTWkthDL83212300;     bjpAHTBQFDqokTWkthDL83212300 = bjpAHTBQFDqokTWkthDL5178338;     bjpAHTBQFDqokTWkthDL5178338 = bjpAHTBQFDqokTWkthDL45988907;     bjpAHTBQFDqokTWkthDL45988907 = bjpAHTBQFDqokTWkthDL62488520;     bjpAHTBQFDqokTWkthDL62488520 = bjpAHTBQFDqokTWkthDL94712008;     bjpAHTBQFDqokTWkthDL94712008 = bjpAHTBQFDqokTWkthDL26021512;     bjpAHTBQFDqokTWkthDL26021512 = bjpAHTBQFDqokTWkthDL80226344;     bjpAHTBQFDqokTWkthDL80226344 = bjpAHTBQFDqokTWkthDL94663641;     bjpAHTBQFDqokTWkthDL94663641 = bjpAHTBQFDqokTWkthDL88955922;     bjpAHTBQFDqokTWkthDL88955922 = bjpAHTBQFDqokTWkthDL19397090;     bjpAHTBQFDqokTWkthDL19397090 = bjpAHTBQFDqokTWkthDL9625353;     bjpAHTBQFDqokTWkthDL9625353 = bjpAHTBQFDqokTWkthDL36491043;     bjpAHTBQFDqokTWkthDL36491043 = bjpAHTBQFDqokTWkthDL25120217;     bjpAHTBQFDqokTWkthDL25120217 = bjpAHTBQFDqokTWkthDL89314277;     bjpAHTBQFDqokTWkthDL89314277 = bjpAHTBQFDqokTWkthDL49575411;     bjpAHTBQFDqokTWkthDL49575411 = bjpAHTBQFDqokTWkthDL10745873;     bjpAHTBQFDqokTWkthDL10745873 = bjpAHTBQFDqokTWkthDL5937787;     bjpAHTBQFDqokTWkthDL5937787 = bjpAHTBQFDqokTWkthDL24756113;     bjpAHTBQFDqokTWkthDL24756113 = bjpAHTBQFDqokTWkthDL15238277;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void sCOXEouOkxZKawdyuihh37335855() {     float ZwucHLRQbrREOPcRYUNC85889944 = -400047067;    float ZwucHLRQbrREOPcRYUNC60844688 = -751964396;    float ZwucHLRQbrREOPcRYUNC60240622 = -448033160;    float ZwucHLRQbrREOPcRYUNC54273430 = -846387083;    float ZwucHLRQbrREOPcRYUNC40690261 = -721965380;    float ZwucHLRQbrREOPcRYUNC38621007 = -417776618;    float ZwucHLRQbrREOPcRYUNC50110380 = -141303431;    float ZwucHLRQbrREOPcRYUNC30160775 = -913791870;    float ZwucHLRQbrREOPcRYUNC41157914 = -555496894;    float ZwucHLRQbrREOPcRYUNC1304588 = -353139976;    float ZwucHLRQbrREOPcRYUNC22378787 = -299025590;    float ZwucHLRQbrREOPcRYUNC72146664 = -495696097;    float ZwucHLRQbrREOPcRYUNC37337149 = -535603454;    float ZwucHLRQbrREOPcRYUNC90383492 = -342868253;    float ZwucHLRQbrREOPcRYUNC16904127 = -218922888;    float ZwucHLRQbrREOPcRYUNC28967973 = -267595059;    float ZwucHLRQbrREOPcRYUNC34867055 = -644217145;    float ZwucHLRQbrREOPcRYUNC34923180 = -728363702;    float ZwucHLRQbrREOPcRYUNC59098610 = -198072480;    float ZwucHLRQbrREOPcRYUNC43960989 = -136812301;    float ZwucHLRQbrREOPcRYUNC92738227 = -820164589;    float ZwucHLRQbrREOPcRYUNC32529526 = -923164720;    float ZwucHLRQbrREOPcRYUNC58855562 = -998704201;    float ZwucHLRQbrREOPcRYUNC70745855 = -888249646;    float ZwucHLRQbrREOPcRYUNC87399974 = -444763406;    float ZwucHLRQbrREOPcRYUNC19330302 = -941316997;    float ZwucHLRQbrREOPcRYUNC47904321 = 42488859;    float ZwucHLRQbrREOPcRYUNC32213411 = -282579579;    float ZwucHLRQbrREOPcRYUNC76573226 = 5541329;    float ZwucHLRQbrREOPcRYUNC51283734 = -466695387;    float ZwucHLRQbrREOPcRYUNC95143966 = -643229725;    float ZwucHLRQbrREOPcRYUNC21882301 = -597228421;    float ZwucHLRQbrREOPcRYUNC46422070 = -344491363;    float ZwucHLRQbrREOPcRYUNC90908487 = -732651293;    float ZwucHLRQbrREOPcRYUNC16792394 = -421476678;    float ZwucHLRQbrREOPcRYUNC84208599 = -694153162;    float ZwucHLRQbrREOPcRYUNC6563961 = -186488237;    float ZwucHLRQbrREOPcRYUNC53484398 = -212529111;    float ZwucHLRQbrREOPcRYUNC45531550 = -960207889;    float ZwucHLRQbrREOPcRYUNC29201224 = -295534925;    float ZwucHLRQbrREOPcRYUNC56687860 = -454117432;    float ZwucHLRQbrREOPcRYUNC6946058 = -160582200;    float ZwucHLRQbrREOPcRYUNC42894054 = -138367855;    float ZwucHLRQbrREOPcRYUNC2712245 = -37440558;    float ZwucHLRQbrREOPcRYUNC23468957 = 96777319;    float ZwucHLRQbrREOPcRYUNC80265769 = -599889701;    float ZwucHLRQbrREOPcRYUNC68977352 = -176635416;    float ZwucHLRQbrREOPcRYUNC23769903 = -704411377;    float ZwucHLRQbrREOPcRYUNC46660338 = -415189614;    float ZwucHLRQbrREOPcRYUNC47679887 = -209075023;    float ZwucHLRQbrREOPcRYUNC34312056 = -929083055;    float ZwucHLRQbrREOPcRYUNC67454953 = -684692701;    float ZwucHLRQbrREOPcRYUNC37734489 = -118708909;    float ZwucHLRQbrREOPcRYUNC27682502 = -801119848;    float ZwucHLRQbrREOPcRYUNC44597627 = -927138231;    float ZwucHLRQbrREOPcRYUNC53360418 = -476882348;    float ZwucHLRQbrREOPcRYUNC1989127 = -753260195;    float ZwucHLRQbrREOPcRYUNC89494766 = -559783515;    float ZwucHLRQbrREOPcRYUNC66873455 = -301623678;    float ZwucHLRQbrREOPcRYUNC21359960 = -780648383;    float ZwucHLRQbrREOPcRYUNC90716686 = -360265477;    float ZwucHLRQbrREOPcRYUNC17896970 = -858723853;    float ZwucHLRQbrREOPcRYUNC53587548 = -819333200;    float ZwucHLRQbrREOPcRYUNC89874179 = 11198493;    float ZwucHLRQbrREOPcRYUNC6160622 = -709910251;    float ZwucHLRQbrREOPcRYUNC496486 = -701797169;    float ZwucHLRQbrREOPcRYUNC25724594 = -51204735;    float ZwucHLRQbrREOPcRYUNC46428661 = -802952161;    float ZwucHLRQbrREOPcRYUNC73591098 = -921391576;    float ZwucHLRQbrREOPcRYUNC32695527 = -524769726;    float ZwucHLRQbrREOPcRYUNC22404013 = 18893177;    float ZwucHLRQbrREOPcRYUNC81382657 = -331688034;    float ZwucHLRQbrREOPcRYUNC89391629 = -768155813;    float ZwucHLRQbrREOPcRYUNC29897386 = -902537556;    float ZwucHLRQbrREOPcRYUNC87273128 = -682694869;    float ZwucHLRQbrREOPcRYUNC85792170 = -559582390;    float ZwucHLRQbrREOPcRYUNC89635472 = -684796866;    float ZwucHLRQbrREOPcRYUNC56143317 = -861263644;    float ZwucHLRQbrREOPcRYUNC47276899 = -885026965;    float ZwucHLRQbrREOPcRYUNC7134206 = -844873705;    float ZwucHLRQbrREOPcRYUNC50352949 = -664681581;    float ZwucHLRQbrREOPcRYUNC24134419 = -253099765;    float ZwucHLRQbrREOPcRYUNC85553073 = -867389965;    float ZwucHLRQbrREOPcRYUNC28893339 = -785383649;    float ZwucHLRQbrREOPcRYUNC16971678 = -537612333;    float ZwucHLRQbrREOPcRYUNC27689013 = -958537024;    float ZwucHLRQbrREOPcRYUNC84147812 = -378519513;    float ZwucHLRQbrREOPcRYUNC18739569 = -543371516;    float ZwucHLRQbrREOPcRYUNC46310861 = -805513063;    float ZwucHLRQbrREOPcRYUNC63431975 = -944594331;    float ZwucHLRQbrREOPcRYUNC82219473 = -940892967;    float ZwucHLRQbrREOPcRYUNC17069194 = -626704722;    float ZwucHLRQbrREOPcRYUNC86610942 = -910905433;    float ZwucHLRQbrREOPcRYUNC24171591 = -79559506;    float ZwucHLRQbrREOPcRYUNC38484538 = -935269448;    float ZwucHLRQbrREOPcRYUNC38790891 = -595393580;    float ZwucHLRQbrREOPcRYUNC53358509 = -341249001;    float ZwucHLRQbrREOPcRYUNC53019874 = -49566348;    float ZwucHLRQbrREOPcRYUNC96551622 = -327530307;    float ZwucHLRQbrREOPcRYUNC22972471 = -400047067;     ZwucHLRQbrREOPcRYUNC85889944 = ZwucHLRQbrREOPcRYUNC60844688;     ZwucHLRQbrREOPcRYUNC60844688 = ZwucHLRQbrREOPcRYUNC60240622;     ZwucHLRQbrREOPcRYUNC60240622 = ZwucHLRQbrREOPcRYUNC54273430;     ZwucHLRQbrREOPcRYUNC54273430 = ZwucHLRQbrREOPcRYUNC40690261;     ZwucHLRQbrREOPcRYUNC40690261 = ZwucHLRQbrREOPcRYUNC38621007;     ZwucHLRQbrREOPcRYUNC38621007 = ZwucHLRQbrREOPcRYUNC50110380;     ZwucHLRQbrREOPcRYUNC50110380 = ZwucHLRQbrREOPcRYUNC30160775;     ZwucHLRQbrREOPcRYUNC30160775 = ZwucHLRQbrREOPcRYUNC41157914;     ZwucHLRQbrREOPcRYUNC41157914 = ZwucHLRQbrREOPcRYUNC1304588;     ZwucHLRQbrREOPcRYUNC1304588 = ZwucHLRQbrREOPcRYUNC22378787;     ZwucHLRQbrREOPcRYUNC22378787 = ZwucHLRQbrREOPcRYUNC72146664;     ZwucHLRQbrREOPcRYUNC72146664 = ZwucHLRQbrREOPcRYUNC37337149;     ZwucHLRQbrREOPcRYUNC37337149 = ZwucHLRQbrREOPcRYUNC90383492;     ZwucHLRQbrREOPcRYUNC90383492 = ZwucHLRQbrREOPcRYUNC16904127;     ZwucHLRQbrREOPcRYUNC16904127 = ZwucHLRQbrREOPcRYUNC28967973;     ZwucHLRQbrREOPcRYUNC28967973 = ZwucHLRQbrREOPcRYUNC34867055;     ZwucHLRQbrREOPcRYUNC34867055 = ZwucHLRQbrREOPcRYUNC34923180;     ZwucHLRQbrREOPcRYUNC34923180 = ZwucHLRQbrREOPcRYUNC59098610;     ZwucHLRQbrREOPcRYUNC59098610 = ZwucHLRQbrREOPcRYUNC43960989;     ZwucHLRQbrREOPcRYUNC43960989 = ZwucHLRQbrREOPcRYUNC92738227;     ZwucHLRQbrREOPcRYUNC92738227 = ZwucHLRQbrREOPcRYUNC32529526;     ZwucHLRQbrREOPcRYUNC32529526 = ZwucHLRQbrREOPcRYUNC58855562;     ZwucHLRQbrREOPcRYUNC58855562 = ZwucHLRQbrREOPcRYUNC70745855;     ZwucHLRQbrREOPcRYUNC70745855 = ZwucHLRQbrREOPcRYUNC87399974;     ZwucHLRQbrREOPcRYUNC87399974 = ZwucHLRQbrREOPcRYUNC19330302;     ZwucHLRQbrREOPcRYUNC19330302 = ZwucHLRQbrREOPcRYUNC47904321;     ZwucHLRQbrREOPcRYUNC47904321 = ZwucHLRQbrREOPcRYUNC32213411;     ZwucHLRQbrREOPcRYUNC32213411 = ZwucHLRQbrREOPcRYUNC76573226;     ZwucHLRQbrREOPcRYUNC76573226 = ZwucHLRQbrREOPcRYUNC51283734;     ZwucHLRQbrREOPcRYUNC51283734 = ZwucHLRQbrREOPcRYUNC95143966;     ZwucHLRQbrREOPcRYUNC95143966 = ZwucHLRQbrREOPcRYUNC21882301;     ZwucHLRQbrREOPcRYUNC21882301 = ZwucHLRQbrREOPcRYUNC46422070;     ZwucHLRQbrREOPcRYUNC46422070 = ZwucHLRQbrREOPcRYUNC90908487;     ZwucHLRQbrREOPcRYUNC90908487 = ZwucHLRQbrREOPcRYUNC16792394;     ZwucHLRQbrREOPcRYUNC16792394 = ZwucHLRQbrREOPcRYUNC84208599;     ZwucHLRQbrREOPcRYUNC84208599 = ZwucHLRQbrREOPcRYUNC6563961;     ZwucHLRQbrREOPcRYUNC6563961 = ZwucHLRQbrREOPcRYUNC53484398;     ZwucHLRQbrREOPcRYUNC53484398 = ZwucHLRQbrREOPcRYUNC45531550;     ZwucHLRQbrREOPcRYUNC45531550 = ZwucHLRQbrREOPcRYUNC29201224;     ZwucHLRQbrREOPcRYUNC29201224 = ZwucHLRQbrREOPcRYUNC56687860;     ZwucHLRQbrREOPcRYUNC56687860 = ZwucHLRQbrREOPcRYUNC6946058;     ZwucHLRQbrREOPcRYUNC6946058 = ZwucHLRQbrREOPcRYUNC42894054;     ZwucHLRQbrREOPcRYUNC42894054 = ZwucHLRQbrREOPcRYUNC2712245;     ZwucHLRQbrREOPcRYUNC2712245 = ZwucHLRQbrREOPcRYUNC23468957;     ZwucHLRQbrREOPcRYUNC23468957 = ZwucHLRQbrREOPcRYUNC80265769;     ZwucHLRQbrREOPcRYUNC80265769 = ZwucHLRQbrREOPcRYUNC68977352;     ZwucHLRQbrREOPcRYUNC68977352 = ZwucHLRQbrREOPcRYUNC23769903;     ZwucHLRQbrREOPcRYUNC23769903 = ZwucHLRQbrREOPcRYUNC46660338;     ZwucHLRQbrREOPcRYUNC46660338 = ZwucHLRQbrREOPcRYUNC47679887;     ZwucHLRQbrREOPcRYUNC47679887 = ZwucHLRQbrREOPcRYUNC34312056;     ZwucHLRQbrREOPcRYUNC34312056 = ZwucHLRQbrREOPcRYUNC67454953;     ZwucHLRQbrREOPcRYUNC67454953 = ZwucHLRQbrREOPcRYUNC37734489;     ZwucHLRQbrREOPcRYUNC37734489 = ZwucHLRQbrREOPcRYUNC27682502;     ZwucHLRQbrREOPcRYUNC27682502 = ZwucHLRQbrREOPcRYUNC44597627;     ZwucHLRQbrREOPcRYUNC44597627 = ZwucHLRQbrREOPcRYUNC53360418;     ZwucHLRQbrREOPcRYUNC53360418 = ZwucHLRQbrREOPcRYUNC1989127;     ZwucHLRQbrREOPcRYUNC1989127 = ZwucHLRQbrREOPcRYUNC89494766;     ZwucHLRQbrREOPcRYUNC89494766 = ZwucHLRQbrREOPcRYUNC66873455;     ZwucHLRQbrREOPcRYUNC66873455 = ZwucHLRQbrREOPcRYUNC21359960;     ZwucHLRQbrREOPcRYUNC21359960 = ZwucHLRQbrREOPcRYUNC90716686;     ZwucHLRQbrREOPcRYUNC90716686 = ZwucHLRQbrREOPcRYUNC17896970;     ZwucHLRQbrREOPcRYUNC17896970 = ZwucHLRQbrREOPcRYUNC53587548;     ZwucHLRQbrREOPcRYUNC53587548 = ZwucHLRQbrREOPcRYUNC89874179;     ZwucHLRQbrREOPcRYUNC89874179 = ZwucHLRQbrREOPcRYUNC6160622;     ZwucHLRQbrREOPcRYUNC6160622 = ZwucHLRQbrREOPcRYUNC496486;     ZwucHLRQbrREOPcRYUNC496486 = ZwucHLRQbrREOPcRYUNC25724594;     ZwucHLRQbrREOPcRYUNC25724594 = ZwucHLRQbrREOPcRYUNC46428661;     ZwucHLRQbrREOPcRYUNC46428661 = ZwucHLRQbrREOPcRYUNC73591098;     ZwucHLRQbrREOPcRYUNC73591098 = ZwucHLRQbrREOPcRYUNC32695527;     ZwucHLRQbrREOPcRYUNC32695527 = ZwucHLRQbrREOPcRYUNC22404013;     ZwucHLRQbrREOPcRYUNC22404013 = ZwucHLRQbrREOPcRYUNC81382657;     ZwucHLRQbrREOPcRYUNC81382657 = ZwucHLRQbrREOPcRYUNC89391629;     ZwucHLRQbrREOPcRYUNC89391629 = ZwucHLRQbrREOPcRYUNC29897386;     ZwucHLRQbrREOPcRYUNC29897386 = ZwucHLRQbrREOPcRYUNC87273128;     ZwucHLRQbrREOPcRYUNC87273128 = ZwucHLRQbrREOPcRYUNC85792170;     ZwucHLRQbrREOPcRYUNC85792170 = ZwucHLRQbrREOPcRYUNC89635472;     ZwucHLRQbrREOPcRYUNC89635472 = ZwucHLRQbrREOPcRYUNC56143317;     ZwucHLRQbrREOPcRYUNC56143317 = ZwucHLRQbrREOPcRYUNC47276899;     ZwucHLRQbrREOPcRYUNC47276899 = ZwucHLRQbrREOPcRYUNC7134206;     ZwucHLRQbrREOPcRYUNC7134206 = ZwucHLRQbrREOPcRYUNC50352949;     ZwucHLRQbrREOPcRYUNC50352949 = ZwucHLRQbrREOPcRYUNC24134419;     ZwucHLRQbrREOPcRYUNC24134419 = ZwucHLRQbrREOPcRYUNC85553073;     ZwucHLRQbrREOPcRYUNC85553073 = ZwucHLRQbrREOPcRYUNC28893339;     ZwucHLRQbrREOPcRYUNC28893339 = ZwucHLRQbrREOPcRYUNC16971678;     ZwucHLRQbrREOPcRYUNC16971678 = ZwucHLRQbrREOPcRYUNC27689013;     ZwucHLRQbrREOPcRYUNC27689013 = ZwucHLRQbrREOPcRYUNC84147812;     ZwucHLRQbrREOPcRYUNC84147812 = ZwucHLRQbrREOPcRYUNC18739569;     ZwucHLRQbrREOPcRYUNC18739569 = ZwucHLRQbrREOPcRYUNC46310861;     ZwucHLRQbrREOPcRYUNC46310861 = ZwucHLRQbrREOPcRYUNC63431975;     ZwucHLRQbrREOPcRYUNC63431975 = ZwucHLRQbrREOPcRYUNC82219473;     ZwucHLRQbrREOPcRYUNC82219473 = ZwucHLRQbrREOPcRYUNC17069194;     ZwucHLRQbrREOPcRYUNC17069194 = ZwucHLRQbrREOPcRYUNC86610942;     ZwucHLRQbrREOPcRYUNC86610942 = ZwucHLRQbrREOPcRYUNC24171591;     ZwucHLRQbrREOPcRYUNC24171591 = ZwucHLRQbrREOPcRYUNC38484538;     ZwucHLRQbrREOPcRYUNC38484538 = ZwucHLRQbrREOPcRYUNC38790891;     ZwucHLRQbrREOPcRYUNC38790891 = ZwucHLRQbrREOPcRYUNC53358509;     ZwucHLRQbrREOPcRYUNC53358509 = ZwucHLRQbrREOPcRYUNC53019874;     ZwucHLRQbrREOPcRYUNC53019874 = ZwucHLRQbrREOPcRYUNC96551622;     ZwucHLRQbrREOPcRYUNC96551622 = ZwucHLRQbrREOPcRYUNC22972471;     ZwucHLRQbrREOPcRYUNC22972471 = ZwucHLRQbrREOPcRYUNC85889944;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void OYUfXJzuBgmLgfUOLADm43994599() {     float ivbZzwvRiigrTRybUtUz85883472 = -272571208;    float ivbZzwvRiigrTRybUtUz99417005 = -794559521;    float ivbZzwvRiigrTRybUtUz61553835 = -405161774;    float ivbZzwvRiigrTRybUtUz39353290 = -666927914;    float ivbZzwvRiigrTRybUtUz49530363 = -90320701;    float ivbZzwvRiigrTRybUtUz73426439 = -117209429;    float ivbZzwvRiigrTRybUtUz18657717 = -945839898;    float ivbZzwvRiigrTRybUtUz99374824 = -617969187;    float ivbZzwvRiigrTRybUtUz23898078 = -269142964;    float ivbZzwvRiigrTRybUtUz97944101 = 10053153;    float ivbZzwvRiigrTRybUtUz42699392 = -258497662;    float ivbZzwvRiigrTRybUtUz86438341 = -231174460;    float ivbZzwvRiigrTRybUtUz69190062 = -557261928;    float ivbZzwvRiigrTRybUtUz44227713 = -688021305;    float ivbZzwvRiigrTRybUtUz62076236 = -181635740;    float ivbZzwvRiigrTRybUtUz3532521 = -600390709;    float ivbZzwvRiigrTRybUtUz90719814 = -463831715;    float ivbZzwvRiigrTRybUtUz57923712 = -25435324;    float ivbZzwvRiigrTRybUtUz79526880 = -816054747;    float ivbZzwvRiigrTRybUtUz16034752 = -748836752;    float ivbZzwvRiigrTRybUtUz72407651 = -530630963;    float ivbZzwvRiigrTRybUtUz10825792 = -333156603;    float ivbZzwvRiigrTRybUtUz95733958 = -357662043;    float ivbZzwvRiigrTRybUtUz14395964 = -769170168;    float ivbZzwvRiigrTRybUtUz15842665 = -871457259;    float ivbZzwvRiigrTRybUtUz35904378 = -849522925;    float ivbZzwvRiigrTRybUtUz18125903 = -258310854;    float ivbZzwvRiigrTRybUtUz67616936 = -891546567;    float ivbZzwvRiigrTRybUtUz89251673 = -640528986;    float ivbZzwvRiigrTRybUtUz27373604 = -142126761;    float ivbZzwvRiigrTRybUtUz28556645 = 69677041;    float ivbZzwvRiigrTRybUtUz6606159 = -989050183;    float ivbZzwvRiigrTRybUtUz31874494 = -546435579;    float ivbZzwvRiigrTRybUtUz91191325 = -609333026;    float ivbZzwvRiigrTRybUtUz99150431 = -680790603;    float ivbZzwvRiigrTRybUtUz27036790 = -773918909;    float ivbZzwvRiigrTRybUtUz18505329 = -286141615;    float ivbZzwvRiigrTRybUtUz23402077 = -207791790;    float ivbZzwvRiigrTRybUtUz80845147 = -572889305;    float ivbZzwvRiigrTRybUtUz1074117 = 95033389;    float ivbZzwvRiigrTRybUtUz44543528 = -542061331;    float ivbZzwvRiigrTRybUtUz92643718 = -354374282;    float ivbZzwvRiigrTRybUtUz3177414 = -697498109;    float ivbZzwvRiigrTRybUtUz61944143 = -694405001;    float ivbZzwvRiigrTRybUtUz7487009 = -376482083;    float ivbZzwvRiigrTRybUtUz20693738 = -411934372;    float ivbZzwvRiigrTRybUtUz96675144 = -657916038;    float ivbZzwvRiigrTRybUtUz99153483 = -213172733;    float ivbZzwvRiigrTRybUtUz65318727 = 21046525;    float ivbZzwvRiigrTRybUtUz67725310 = -896099909;    float ivbZzwvRiigrTRybUtUz47571426 = -507638062;    float ivbZzwvRiigrTRybUtUz82002211 = 33161551;    float ivbZzwvRiigrTRybUtUz13182838 = -10762917;    float ivbZzwvRiigrTRybUtUz37932773 = -233039699;    float ivbZzwvRiigrTRybUtUz71586947 = -684037508;    float ivbZzwvRiigrTRybUtUz75057681 = -939414606;    float ivbZzwvRiigrTRybUtUz3683047 = -336897479;    float ivbZzwvRiigrTRybUtUz47157871 = -635991606;    float ivbZzwvRiigrTRybUtUz23510626 = -795470655;    float ivbZzwvRiigrTRybUtUz13625985 = -240797776;    float ivbZzwvRiigrTRybUtUz55300537 = -858898576;    float ivbZzwvRiigrTRybUtUz51040780 = 45706669;    float ivbZzwvRiigrTRybUtUz10123152 = -977440202;    float ivbZzwvRiigrTRybUtUz96524474 = -27016204;    float ivbZzwvRiigrTRybUtUz69387456 = 40376112;    float ivbZzwvRiigrTRybUtUz36093234 = -269447480;    float ivbZzwvRiigrTRybUtUz54563848 = -684738881;    float ivbZzwvRiigrTRybUtUz77998737 = -947928903;    float ivbZzwvRiigrTRybUtUz45077281 = 92769297;    float ivbZzwvRiigrTRybUtUz35039447 = -407716832;    float ivbZzwvRiigrTRybUtUz85027191 = -214249095;    float ivbZzwvRiigrTRybUtUz67317737 = -156039925;    float ivbZzwvRiigrTRybUtUz77078565 = -452546019;    float ivbZzwvRiigrTRybUtUz78452763 = -811088137;    float ivbZzwvRiigrTRybUtUz71491224 = -106775422;    float ivbZzwvRiigrTRybUtUz79763933 = -76256681;    float ivbZzwvRiigrTRybUtUz7648378 = -635658495;    float ivbZzwvRiigrTRybUtUz33789816 = -663257042;    float ivbZzwvRiigrTRybUtUz6908955 = -292688086;    float ivbZzwvRiigrTRybUtUz95148926 = -359522887;    float ivbZzwvRiigrTRybUtUz39229234 = -91606887;    float ivbZzwvRiigrTRybUtUz18972420 = 54861878;    float ivbZzwvRiigrTRybUtUz2298209 = -812593092;    float ivbZzwvRiigrTRybUtUz21526363 = -744429077;    float ivbZzwvRiigrTRybUtUz79802178 = -634488700;    float ivbZzwvRiigrTRybUtUz46554434 = -963484511;    float ivbZzwvRiigrTRybUtUz93423321 = -878287266;    float ivbZzwvRiigrTRybUtUz93941720 = -213395880;    float ivbZzwvRiigrTRybUtUz19604378 = -925295518;    float ivbZzwvRiigrTRybUtUz24092751 = -741375997;    float ivbZzwvRiigrTRybUtUz23353743 = -337021430;    float ivbZzwvRiigrTRybUtUz71347457 = -650150009;    float ivbZzwvRiigrTRybUtUz99891451 = -412321135;    float ivbZzwvRiigrTRybUtUz67219162 = -232091529;    float ivbZzwvRiigrTRybUtUz45773580 = -46068036;    float ivbZzwvRiigrTRybUtUz93502747 = -487768000;    float ivbZzwvRiigrTRybUtUz82520566 = -376934081;    float ivbZzwvRiigrTRybUtUz6652940 = -570481906;    float ivbZzwvRiigrTRybUtUz92556686 = -634781114;    float ivbZzwvRiigrTRybUtUz71393775 = -272571208;     ivbZzwvRiigrTRybUtUz85883472 = ivbZzwvRiigrTRybUtUz99417005;     ivbZzwvRiigrTRybUtUz99417005 = ivbZzwvRiigrTRybUtUz61553835;     ivbZzwvRiigrTRybUtUz61553835 = ivbZzwvRiigrTRybUtUz39353290;     ivbZzwvRiigrTRybUtUz39353290 = ivbZzwvRiigrTRybUtUz49530363;     ivbZzwvRiigrTRybUtUz49530363 = ivbZzwvRiigrTRybUtUz73426439;     ivbZzwvRiigrTRybUtUz73426439 = ivbZzwvRiigrTRybUtUz18657717;     ivbZzwvRiigrTRybUtUz18657717 = ivbZzwvRiigrTRybUtUz99374824;     ivbZzwvRiigrTRybUtUz99374824 = ivbZzwvRiigrTRybUtUz23898078;     ivbZzwvRiigrTRybUtUz23898078 = ivbZzwvRiigrTRybUtUz97944101;     ivbZzwvRiigrTRybUtUz97944101 = ivbZzwvRiigrTRybUtUz42699392;     ivbZzwvRiigrTRybUtUz42699392 = ivbZzwvRiigrTRybUtUz86438341;     ivbZzwvRiigrTRybUtUz86438341 = ivbZzwvRiigrTRybUtUz69190062;     ivbZzwvRiigrTRybUtUz69190062 = ivbZzwvRiigrTRybUtUz44227713;     ivbZzwvRiigrTRybUtUz44227713 = ivbZzwvRiigrTRybUtUz62076236;     ivbZzwvRiigrTRybUtUz62076236 = ivbZzwvRiigrTRybUtUz3532521;     ivbZzwvRiigrTRybUtUz3532521 = ivbZzwvRiigrTRybUtUz90719814;     ivbZzwvRiigrTRybUtUz90719814 = ivbZzwvRiigrTRybUtUz57923712;     ivbZzwvRiigrTRybUtUz57923712 = ivbZzwvRiigrTRybUtUz79526880;     ivbZzwvRiigrTRybUtUz79526880 = ivbZzwvRiigrTRybUtUz16034752;     ivbZzwvRiigrTRybUtUz16034752 = ivbZzwvRiigrTRybUtUz72407651;     ivbZzwvRiigrTRybUtUz72407651 = ivbZzwvRiigrTRybUtUz10825792;     ivbZzwvRiigrTRybUtUz10825792 = ivbZzwvRiigrTRybUtUz95733958;     ivbZzwvRiigrTRybUtUz95733958 = ivbZzwvRiigrTRybUtUz14395964;     ivbZzwvRiigrTRybUtUz14395964 = ivbZzwvRiigrTRybUtUz15842665;     ivbZzwvRiigrTRybUtUz15842665 = ivbZzwvRiigrTRybUtUz35904378;     ivbZzwvRiigrTRybUtUz35904378 = ivbZzwvRiigrTRybUtUz18125903;     ivbZzwvRiigrTRybUtUz18125903 = ivbZzwvRiigrTRybUtUz67616936;     ivbZzwvRiigrTRybUtUz67616936 = ivbZzwvRiigrTRybUtUz89251673;     ivbZzwvRiigrTRybUtUz89251673 = ivbZzwvRiigrTRybUtUz27373604;     ivbZzwvRiigrTRybUtUz27373604 = ivbZzwvRiigrTRybUtUz28556645;     ivbZzwvRiigrTRybUtUz28556645 = ivbZzwvRiigrTRybUtUz6606159;     ivbZzwvRiigrTRybUtUz6606159 = ivbZzwvRiigrTRybUtUz31874494;     ivbZzwvRiigrTRybUtUz31874494 = ivbZzwvRiigrTRybUtUz91191325;     ivbZzwvRiigrTRybUtUz91191325 = ivbZzwvRiigrTRybUtUz99150431;     ivbZzwvRiigrTRybUtUz99150431 = ivbZzwvRiigrTRybUtUz27036790;     ivbZzwvRiigrTRybUtUz27036790 = ivbZzwvRiigrTRybUtUz18505329;     ivbZzwvRiigrTRybUtUz18505329 = ivbZzwvRiigrTRybUtUz23402077;     ivbZzwvRiigrTRybUtUz23402077 = ivbZzwvRiigrTRybUtUz80845147;     ivbZzwvRiigrTRybUtUz80845147 = ivbZzwvRiigrTRybUtUz1074117;     ivbZzwvRiigrTRybUtUz1074117 = ivbZzwvRiigrTRybUtUz44543528;     ivbZzwvRiigrTRybUtUz44543528 = ivbZzwvRiigrTRybUtUz92643718;     ivbZzwvRiigrTRybUtUz92643718 = ivbZzwvRiigrTRybUtUz3177414;     ivbZzwvRiigrTRybUtUz3177414 = ivbZzwvRiigrTRybUtUz61944143;     ivbZzwvRiigrTRybUtUz61944143 = ivbZzwvRiigrTRybUtUz7487009;     ivbZzwvRiigrTRybUtUz7487009 = ivbZzwvRiigrTRybUtUz20693738;     ivbZzwvRiigrTRybUtUz20693738 = ivbZzwvRiigrTRybUtUz96675144;     ivbZzwvRiigrTRybUtUz96675144 = ivbZzwvRiigrTRybUtUz99153483;     ivbZzwvRiigrTRybUtUz99153483 = ivbZzwvRiigrTRybUtUz65318727;     ivbZzwvRiigrTRybUtUz65318727 = ivbZzwvRiigrTRybUtUz67725310;     ivbZzwvRiigrTRybUtUz67725310 = ivbZzwvRiigrTRybUtUz47571426;     ivbZzwvRiigrTRybUtUz47571426 = ivbZzwvRiigrTRybUtUz82002211;     ivbZzwvRiigrTRybUtUz82002211 = ivbZzwvRiigrTRybUtUz13182838;     ivbZzwvRiigrTRybUtUz13182838 = ivbZzwvRiigrTRybUtUz37932773;     ivbZzwvRiigrTRybUtUz37932773 = ivbZzwvRiigrTRybUtUz71586947;     ivbZzwvRiigrTRybUtUz71586947 = ivbZzwvRiigrTRybUtUz75057681;     ivbZzwvRiigrTRybUtUz75057681 = ivbZzwvRiigrTRybUtUz3683047;     ivbZzwvRiigrTRybUtUz3683047 = ivbZzwvRiigrTRybUtUz47157871;     ivbZzwvRiigrTRybUtUz47157871 = ivbZzwvRiigrTRybUtUz23510626;     ivbZzwvRiigrTRybUtUz23510626 = ivbZzwvRiigrTRybUtUz13625985;     ivbZzwvRiigrTRybUtUz13625985 = ivbZzwvRiigrTRybUtUz55300537;     ivbZzwvRiigrTRybUtUz55300537 = ivbZzwvRiigrTRybUtUz51040780;     ivbZzwvRiigrTRybUtUz51040780 = ivbZzwvRiigrTRybUtUz10123152;     ivbZzwvRiigrTRybUtUz10123152 = ivbZzwvRiigrTRybUtUz96524474;     ivbZzwvRiigrTRybUtUz96524474 = ivbZzwvRiigrTRybUtUz69387456;     ivbZzwvRiigrTRybUtUz69387456 = ivbZzwvRiigrTRybUtUz36093234;     ivbZzwvRiigrTRybUtUz36093234 = ivbZzwvRiigrTRybUtUz54563848;     ivbZzwvRiigrTRybUtUz54563848 = ivbZzwvRiigrTRybUtUz77998737;     ivbZzwvRiigrTRybUtUz77998737 = ivbZzwvRiigrTRybUtUz45077281;     ivbZzwvRiigrTRybUtUz45077281 = ivbZzwvRiigrTRybUtUz35039447;     ivbZzwvRiigrTRybUtUz35039447 = ivbZzwvRiigrTRybUtUz85027191;     ivbZzwvRiigrTRybUtUz85027191 = ivbZzwvRiigrTRybUtUz67317737;     ivbZzwvRiigrTRybUtUz67317737 = ivbZzwvRiigrTRybUtUz77078565;     ivbZzwvRiigrTRybUtUz77078565 = ivbZzwvRiigrTRybUtUz78452763;     ivbZzwvRiigrTRybUtUz78452763 = ivbZzwvRiigrTRybUtUz71491224;     ivbZzwvRiigrTRybUtUz71491224 = ivbZzwvRiigrTRybUtUz79763933;     ivbZzwvRiigrTRybUtUz79763933 = ivbZzwvRiigrTRybUtUz7648378;     ivbZzwvRiigrTRybUtUz7648378 = ivbZzwvRiigrTRybUtUz33789816;     ivbZzwvRiigrTRybUtUz33789816 = ivbZzwvRiigrTRybUtUz6908955;     ivbZzwvRiigrTRybUtUz6908955 = ivbZzwvRiigrTRybUtUz95148926;     ivbZzwvRiigrTRybUtUz95148926 = ivbZzwvRiigrTRybUtUz39229234;     ivbZzwvRiigrTRybUtUz39229234 = ivbZzwvRiigrTRybUtUz18972420;     ivbZzwvRiigrTRybUtUz18972420 = ivbZzwvRiigrTRybUtUz2298209;     ivbZzwvRiigrTRybUtUz2298209 = ivbZzwvRiigrTRybUtUz21526363;     ivbZzwvRiigrTRybUtUz21526363 = ivbZzwvRiigrTRybUtUz79802178;     ivbZzwvRiigrTRybUtUz79802178 = ivbZzwvRiigrTRybUtUz46554434;     ivbZzwvRiigrTRybUtUz46554434 = ivbZzwvRiigrTRybUtUz93423321;     ivbZzwvRiigrTRybUtUz93423321 = ivbZzwvRiigrTRybUtUz93941720;     ivbZzwvRiigrTRybUtUz93941720 = ivbZzwvRiigrTRybUtUz19604378;     ivbZzwvRiigrTRybUtUz19604378 = ivbZzwvRiigrTRybUtUz24092751;     ivbZzwvRiigrTRybUtUz24092751 = ivbZzwvRiigrTRybUtUz23353743;     ivbZzwvRiigrTRybUtUz23353743 = ivbZzwvRiigrTRybUtUz71347457;     ivbZzwvRiigrTRybUtUz71347457 = ivbZzwvRiigrTRybUtUz99891451;     ivbZzwvRiigrTRybUtUz99891451 = ivbZzwvRiigrTRybUtUz67219162;     ivbZzwvRiigrTRybUtUz67219162 = ivbZzwvRiigrTRybUtUz45773580;     ivbZzwvRiigrTRybUtUz45773580 = ivbZzwvRiigrTRybUtUz93502747;     ivbZzwvRiigrTRybUtUz93502747 = ivbZzwvRiigrTRybUtUz82520566;     ivbZzwvRiigrTRybUtUz82520566 = ivbZzwvRiigrTRybUtUz6652940;     ivbZzwvRiigrTRybUtUz6652940 = ivbZzwvRiigrTRybUtUz92556686;     ivbZzwvRiigrTRybUtUz92556686 = ivbZzwvRiigrTRybUtUz71393775;     ivbZzwvRiigrTRybUtUz71393775 = ivbZzwvRiigrTRybUtUz85883472;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void WerWmwMGrzpZwsLoiWVc98410812() {     float jWtIQkhFRSQSqHhlrrZH56535140 = -890720339;    float jWtIQkhFRSQSqHhlrrZH68090522 = -666202820;    float jWtIQkhFRSQSqHhlrrZH41522754 = -847456234;    float jWtIQkhFRSQSqHhlrrZH79284577 = -857066303;    float jWtIQkhFRSQSqHhlrrZH70980684 = -973727049;    float jWtIQkhFRSQSqHhlrrZH63426202 = -918790581;    float jWtIQkhFRSQSqHhlrrZH37036612 = -944615608;    float jWtIQkhFRSQSqHhlrrZH56217205 = -964786277;    float jWtIQkhFRSQSqHhlrrZH69763132 = -362490293;    float jWtIQkhFRSQSqHhlrrZH20910562 = -291042197;    float jWtIQkhFRSQSqHhlrrZH84948552 = -293399591;    float jWtIQkhFRSQSqHhlrrZH31984913 = -546938253;    float jWtIQkhFRSQSqHhlrrZH49017449 = -466798987;    float jWtIQkhFRSQSqHhlrrZH28074746 = -489049515;    float jWtIQkhFRSQSqHhlrrZH44796255 = -331385743;    float jWtIQkhFRSQSqHhlrrZH36061160 = -170374296;    float jWtIQkhFRSQSqHhlrrZH84572641 = -373016844;    float jWtIQkhFRSQSqHhlrrZH78217206 = -14601520;    float jWtIQkhFRSQSqHhlrrZH61234662 = -423242689;    float jWtIQkhFRSQSqHhlrrZH74821461 = -191203057;    float jWtIQkhFRSQSqHhlrrZH27543406 = -439310103;    float jWtIQkhFRSQSqHhlrrZH72221556 = -664441908;    float jWtIQkhFRSQSqHhlrrZH97677767 = -925505188;    float jWtIQkhFRSQSqHhlrrZH48245501 = -361571074;    float jWtIQkhFRSQSqHhlrrZH79967053 = -547361947;    float jWtIQkhFRSQSqHhlrrZH66501959 = -947807370;    float jWtIQkhFRSQSqHhlrrZH26386688 = -653488629;    float jWtIQkhFRSQSqHhlrrZH75399161 = -296670335;    float jWtIQkhFRSQSqHhlrrZH21394161 = -934887203;    float jWtIQkhFRSQSqHhlrrZH66545699 = -700521832;    float jWtIQkhFRSQSqHhlrrZH35628642 = -778756645;    float jWtIQkhFRSQSqHhlrrZH51117619 = -217347346;    float jWtIQkhFRSQSqHhlrrZH14819855 = -493397188;    float jWtIQkhFRSQSqHhlrrZH56556300 = -172862710;    float jWtIQkhFRSQSqHhlrrZH77174668 = -527737382;    float jWtIQkhFRSQSqHhlrrZH87030048 = -853145456;    float jWtIQkhFRSQSqHhlrrZH62296814 = -474734822;    float jWtIQkhFRSQSqHhlrrZH76194566 = 8095102;    float jWtIQkhFRSQSqHhlrrZH59378436 = -528617942;    float jWtIQkhFRSQSqHhlrrZH96177415 = -522574973;    float jWtIQkhFRSQSqHhlrrZH4616812 = -926080209;    float jWtIQkhFRSQSqHhlrrZH21126709 = -123763352;    float jWtIQkhFRSQSqHhlrrZH52144374 = -684252161;    float jWtIQkhFRSQSqHhlrrZH68452443 = -766379973;    float jWtIQkhFRSQSqHhlrrZH3441068 = -685128477;    float jWtIQkhFRSQSqHhlrrZH27099573 = -170271527;    float jWtIQkhFRSQSqHhlrrZH22806348 = -773985838;    float jWtIQkhFRSQSqHhlrrZH64767693 = -665303028;    float jWtIQkhFRSQSqHhlrrZH70760179 = 35282555;    float jWtIQkhFRSQSqHhlrrZH76152796 = -79108788;    float jWtIQkhFRSQSqHhlrrZH15760750 = -54210183;    float jWtIQkhFRSQSqHhlrrZH23873716 = 63941829;    float jWtIQkhFRSQSqHhlrrZH68258494 = 61439041;    float jWtIQkhFRSQSqHhlrrZH28160077 = -893921195;    float jWtIQkhFRSQSqHhlrrZH70867406 = -794978423;    float jWtIQkhFRSQSqHhlrrZH84313583 = -126278431;    float jWtIQkhFRSQSqHhlrrZH70412755 = -740697633;    float jWtIQkhFRSQSqHhlrrZH93277252 = -385885160;    float jWtIQkhFRSQSqHhlrrZH99317524 = -209704357;    float jWtIQkhFRSQSqHhlrrZH4478726 = 74080321;    float jWtIQkhFRSQSqHhlrrZH37039514 = -165301952;    float jWtIQkhFRSQSqHhlrrZH61637450 = -547945274;    float jWtIQkhFRSQSqHhlrrZH34823045 = 70100925;    float jWtIQkhFRSQSqHhlrrZH3217433 = -661968462;    float jWtIQkhFRSQSqHhlrrZH85281920 = -512285553;    float jWtIQkhFRSQSqHhlrrZH33830934 = 23947754;    float jWtIQkhFRSQSqHhlrrZH17165058 = 46458935;    float jWtIQkhFRSQSqHhlrrZH92461149 = -193936277;    float jWtIQkhFRSQSqHhlrrZH50900078 = -961312133;    float jWtIQkhFRSQSqHhlrrZH57766207 = -478240287;    float jWtIQkhFRSQSqHhlrrZH73764346 = -695639474;    float jWtIQkhFRSQSqHhlrrZH8378076 = -281111947;    float jWtIQkhFRSQSqHhlrrZH18838771 = -485983578;    float jWtIQkhFRSQSqHhlrrZH65057247 = -900667716;    float jWtIQkhFRSQSqHhlrrZH70204649 = -265122848;    float jWtIQkhFRSQSqHhlrrZH6416697 = -215546752;    float jWtIQkhFRSQSqHhlrrZH20077183 = -980189748;    float jWtIQkhFRSQSqHhlrrZH29225324 = -59125216;    float jWtIQkhFRSQSqHhlrrZH44804433 = -676442597;    float jWtIQkhFRSQSqHhlrrZH52867480 = -277090420;    float jWtIQkhFRSQSqHhlrrZH43695611 = -73821533;    float jWtIQkhFRSQSqHhlrrZH61618995 = -988185602;    float jWtIQkhFRSQSqHhlrrZH4638982 = -231952891;    float jWtIQkhFRSQSqHhlrrZH45241365 = -755778416;    float jWtIQkhFRSQSqHhlrrZH50784950 = -546311649;    float jWtIQkhFRSQSqHhlrrZH11754926 = -742698474;    float jWtIQkhFRSQSqHhlrrZH82859124 = -178786387;    float jWtIQkhFRSQSqHhlrrZH86659777 = -599475993;    float jWtIQkhFRSQSqHhlrrZH85688893 = -377884288;    float jWtIQkhFRSQSqHhlrrZH92861084 = -301458952;    float jWtIQkhFRSQSqHhlrrZH16617293 = -12447824;    float jWtIQkhFRSQSqHhlrrZH69019561 = 11150337;    float jWtIQkhFRSQSqHhlrrZH76877041 = -782200541;    float jWtIQkhFRSQSqHhlrrZH54899710 = -502698264;    float jWtIQkhFRSQSqHhlrrZH59137901 = -257273022;    float jWtIQkhFRSQSqHhlrrZH42979361 = -278134936;    float jWtIQkhFRSQSqHhlrrZH86303664 = -93864277;    float jWtIQkhFRSQSqHhlrrZH48926941 = 77716300;    float jWtIQkhFRSQSqHhlrrZH83170522 = -154094420;    float jWtIQkhFRSQSqHhlrrZH69610134 = -890720339;     jWtIQkhFRSQSqHhlrrZH56535140 = jWtIQkhFRSQSqHhlrrZH68090522;     jWtIQkhFRSQSqHhlrrZH68090522 = jWtIQkhFRSQSqHhlrrZH41522754;     jWtIQkhFRSQSqHhlrrZH41522754 = jWtIQkhFRSQSqHhlrrZH79284577;     jWtIQkhFRSQSqHhlrrZH79284577 = jWtIQkhFRSQSqHhlrrZH70980684;     jWtIQkhFRSQSqHhlrrZH70980684 = jWtIQkhFRSQSqHhlrrZH63426202;     jWtIQkhFRSQSqHhlrrZH63426202 = jWtIQkhFRSQSqHhlrrZH37036612;     jWtIQkhFRSQSqHhlrrZH37036612 = jWtIQkhFRSQSqHhlrrZH56217205;     jWtIQkhFRSQSqHhlrrZH56217205 = jWtIQkhFRSQSqHhlrrZH69763132;     jWtIQkhFRSQSqHhlrrZH69763132 = jWtIQkhFRSQSqHhlrrZH20910562;     jWtIQkhFRSQSqHhlrrZH20910562 = jWtIQkhFRSQSqHhlrrZH84948552;     jWtIQkhFRSQSqHhlrrZH84948552 = jWtIQkhFRSQSqHhlrrZH31984913;     jWtIQkhFRSQSqHhlrrZH31984913 = jWtIQkhFRSQSqHhlrrZH49017449;     jWtIQkhFRSQSqHhlrrZH49017449 = jWtIQkhFRSQSqHhlrrZH28074746;     jWtIQkhFRSQSqHhlrrZH28074746 = jWtIQkhFRSQSqHhlrrZH44796255;     jWtIQkhFRSQSqHhlrrZH44796255 = jWtIQkhFRSQSqHhlrrZH36061160;     jWtIQkhFRSQSqHhlrrZH36061160 = jWtIQkhFRSQSqHhlrrZH84572641;     jWtIQkhFRSQSqHhlrrZH84572641 = jWtIQkhFRSQSqHhlrrZH78217206;     jWtIQkhFRSQSqHhlrrZH78217206 = jWtIQkhFRSQSqHhlrrZH61234662;     jWtIQkhFRSQSqHhlrrZH61234662 = jWtIQkhFRSQSqHhlrrZH74821461;     jWtIQkhFRSQSqHhlrrZH74821461 = jWtIQkhFRSQSqHhlrrZH27543406;     jWtIQkhFRSQSqHhlrrZH27543406 = jWtIQkhFRSQSqHhlrrZH72221556;     jWtIQkhFRSQSqHhlrrZH72221556 = jWtIQkhFRSQSqHhlrrZH97677767;     jWtIQkhFRSQSqHhlrrZH97677767 = jWtIQkhFRSQSqHhlrrZH48245501;     jWtIQkhFRSQSqHhlrrZH48245501 = jWtIQkhFRSQSqHhlrrZH79967053;     jWtIQkhFRSQSqHhlrrZH79967053 = jWtIQkhFRSQSqHhlrrZH66501959;     jWtIQkhFRSQSqHhlrrZH66501959 = jWtIQkhFRSQSqHhlrrZH26386688;     jWtIQkhFRSQSqHhlrrZH26386688 = jWtIQkhFRSQSqHhlrrZH75399161;     jWtIQkhFRSQSqHhlrrZH75399161 = jWtIQkhFRSQSqHhlrrZH21394161;     jWtIQkhFRSQSqHhlrrZH21394161 = jWtIQkhFRSQSqHhlrrZH66545699;     jWtIQkhFRSQSqHhlrrZH66545699 = jWtIQkhFRSQSqHhlrrZH35628642;     jWtIQkhFRSQSqHhlrrZH35628642 = jWtIQkhFRSQSqHhlrrZH51117619;     jWtIQkhFRSQSqHhlrrZH51117619 = jWtIQkhFRSQSqHhlrrZH14819855;     jWtIQkhFRSQSqHhlrrZH14819855 = jWtIQkhFRSQSqHhlrrZH56556300;     jWtIQkhFRSQSqHhlrrZH56556300 = jWtIQkhFRSQSqHhlrrZH77174668;     jWtIQkhFRSQSqHhlrrZH77174668 = jWtIQkhFRSQSqHhlrrZH87030048;     jWtIQkhFRSQSqHhlrrZH87030048 = jWtIQkhFRSQSqHhlrrZH62296814;     jWtIQkhFRSQSqHhlrrZH62296814 = jWtIQkhFRSQSqHhlrrZH76194566;     jWtIQkhFRSQSqHhlrrZH76194566 = jWtIQkhFRSQSqHhlrrZH59378436;     jWtIQkhFRSQSqHhlrrZH59378436 = jWtIQkhFRSQSqHhlrrZH96177415;     jWtIQkhFRSQSqHhlrrZH96177415 = jWtIQkhFRSQSqHhlrrZH4616812;     jWtIQkhFRSQSqHhlrrZH4616812 = jWtIQkhFRSQSqHhlrrZH21126709;     jWtIQkhFRSQSqHhlrrZH21126709 = jWtIQkhFRSQSqHhlrrZH52144374;     jWtIQkhFRSQSqHhlrrZH52144374 = jWtIQkhFRSQSqHhlrrZH68452443;     jWtIQkhFRSQSqHhlrrZH68452443 = jWtIQkhFRSQSqHhlrrZH3441068;     jWtIQkhFRSQSqHhlrrZH3441068 = jWtIQkhFRSQSqHhlrrZH27099573;     jWtIQkhFRSQSqHhlrrZH27099573 = jWtIQkhFRSQSqHhlrrZH22806348;     jWtIQkhFRSQSqHhlrrZH22806348 = jWtIQkhFRSQSqHhlrrZH64767693;     jWtIQkhFRSQSqHhlrrZH64767693 = jWtIQkhFRSQSqHhlrrZH70760179;     jWtIQkhFRSQSqHhlrrZH70760179 = jWtIQkhFRSQSqHhlrrZH76152796;     jWtIQkhFRSQSqHhlrrZH76152796 = jWtIQkhFRSQSqHhlrrZH15760750;     jWtIQkhFRSQSqHhlrrZH15760750 = jWtIQkhFRSQSqHhlrrZH23873716;     jWtIQkhFRSQSqHhlrrZH23873716 = jWtIQkhFRSQSqHhlrrZH68258494;     jWtIQkhFRSQSqHhlrrZH68258494 = jWtIQkhFRSQSqHhlrrZH28160077;     jWtIQkhFRSQSqHhlrrZH28160077 = jWtIQkhFRSQSqHhlrrZH70867406;     jWtIQkhFRSQSqHhlrrZH70867406 = jWtIQkhFRSQSqHhlrrZH84313583;     jWtIQkhFRSQSqHhlrrZH84313583 = jWtIQkhFRSQSqHhlrrZH70412755;     jWtIQkhFRSQSqHhlrrZH70412755 = jWtIQkhFRSQSqHhlrrZH93277252;     jWtIQkhFRSQSqHhlrrZH93277252 = jWtIQkhFRSQSqHhlrrZH99317524;     jWtIQkhFRSQSqHhlrrZH99317524 = jWtIQkhFRSQSqHhlrrZH4478726;     jWtIQkhFRSQSqHhlrrZH4478726 = jWtIQkhFRSQSqHhlrrZH37039514;     jWtIQkhFRSQSqHhlrrZH37039514 = jWtIQkhFRSQSqHhlrrZH61637450;     jWtIQkhFRSQSqHhlrrZH61637450 = jWtIQkhFRSQSqHhlrrZH34823045;     jWtIQkhFRSQSqHhlrrZH34823045 = jWtIQkhFRSQSqHhlrrZH3217433;     jWtIQkhFRSQSqHhlrrZH3217433 = jWtIQkhFRSQSqHhlrrZH85281920;     jWtIQkhFRSQSqHhlrrZH85281920 = jWtIQkhFRSQSqHhlrrZH33830934;     jWtIQkhFRSQSqHhlrrZH33830934 = jWtIQkhFRSQSqHhlrrZH17165058;     jWtIQkhFRSQSqHhlrrZH17165058 = jWtIQkhFRSQSqHhlrrZH92461149;     jWtIQkhFRSQSqHhlrrZH92461149 = jWtIQkhFRSQSqHhlrrZH50900078;     jWtIQkhFRSQSqHhlrrZH50900078 = jWtIQkhFRSQSqHhlrrZH57766207;     jWtIQkhFRSQSqHhlrrZH57766207 = jWtIQkhFRSQSqHhlrrZH73764346;     jWtIQkhFRSQSqHhlrrZH73764346 = jWtIQkhFRSQSqHhlrrZH8378076;     jWtIQkhFRSQSqHhlrrZH8378076 = jWtIQkhFRSQSqHhlrrZH18838771;     jWtIQkhFRSQSqHhlrrZH18838771 = jWtIQkhFRSQSqHhlrrZH65057247;     jWtIQkhFRSQSqHhlrrZH65057247 = jWtIQkhFRSQSqHhlrrZH70204649;     jWtIQkhFRSQSqHhlrrZH70204649 = jWtIQkhFRSQSqHhlrrZH6416697;     jWtIQkhFRSQSqHhlrrZH6416697 = jWtIQkhFRSQSqHhlrrZH20077183;     jWtIQkhFRSQSqHhlrrZH20077183 = jWtIQkhFRSQSqHhlrrZH29225324;     jWtIQkhFRSQSqHhlrrZH29225324 = jWtIQkhFRSQSqHhlrrZH44804433;     jWtIQkhFRSQSqHhlrrZH44804433 = jWtIQkhFRSQSqHhlrrZH52867480;     jWtIQkhFRSQSqHhlrrZH52867480 = jWtIQkhFRSQSqHhlrrZH43695611;     jWtIQkhFRSQSqHhlrrZH43695611 = jWtIQkhFRSQSqHhlrrZH61618995;     jWtIQkhFRSQSqHhlrrZH61618995 = jWtIQkhFRSQSqHhlrrZH4638982;     jWtIQkhFRSQSqHhlrrZH4638982 = jWtIQkhFRSQSqHhlrrZH45241365;     jWtIQkhFRSQSqHhlrrZH45241365 = jWtIQkhFRSQSqHhlrrZH50784950;     jWtIQkhFRSQSqHhlrrZH50784950 = jWtIQkhFRSQSqHhlrrZH11754926;     jWtIQkhFRSQSqHhlrrZH11754926 = jWtIQkhFRSQSqHhlrrZH82859124;     jWtIQkhFRSQSqHhlrrZH82859124 = jWtIQkhFRSQSqHhlrrZH86659777;     jWtIQkhFRSQSqHhlrrZH86659777 = jWtIQkhFRSQSqHhlrrZH85688893;     jWtIQkhFRSQSqHhlrrZH85688893 = jWtIQkhFRSQSqHhlrrZH92861084;     jWtIQkhFRSQSqHhlrrZH92861084 = jWtIQkhFRSQSqHhlrrZH16617293;     jWtIQkhFRSQSqHhlrrZH16617293 = jWtIQkhFRSQSqHhlrrZH69019561;     jWtIQkhFRSQSqHhlrrZH69019561 = jWtIQkhFRSQSqHhlrrZH76877041;     jWtIQkhFRSQSqHhlrrZH76877041 = jWtIQkhFRSQSqHhlrrZH54899710;     jWtIQkhFRSQSqHhlrrZH54899710 = jWtIQkhFRSQSqHhlrrZH59137901;     jWtIQkhFRSQSqHhlrrZH59137901 = jWtIQkhFRSQSqHhlrrZH42979361;     jWtIQkhFRSQSqHhlrrZH42979361 = jWtIQkhFRSQSqHhlrrZH86303664;     jWtIQkhFRSQSqHhlrrZH86303664 = jWtIQkhFRSQSqHhlrrZH48926941;     jWtIQkhFRSQSqHhlrrZH48926941 = jWtIQkhFRSQSqHhlrrZH83170522;     jWtIQkhFRSQSqHhlrrZH83170522 = jWtIQkhFRSQSqHhlrrZH69610134;     jWtIQkhFRSQSqHhlrrZH69610134 = jWtIQkhFRSQSqHhlrrZH56535140;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dyfprcponosLjumUUSLN93941406() {     float GaMxFhCFOABMhDhJBZvS53054532 = 83003115;    float GaMxFhCFOABMhDhJBZvS44601031 = -576295997;    float GaMxFhCFOABMhDhJBZvS95891852 = -638430386;    float GaMxFhCFOABMhDhJBZvS6753565 = -405339263;    float GaMxFhCFOABMhDhJBZvS30792973 = -60958723;    float GaMxFhCFOABMhDhJBZvS65867641 = -149885345;    float GaMxFhCFOABMhDhJBZvS42303929 = -304997116;    float GaMxFhCFOABMhDhJBZvS22455325 = -273493144;    float GaMxFhCFOABMhDhJBZvS91277224 = -901261977;    float GaMxFhCFOABMhDhJBZvS78275585 = -312960539;    float GaMxFhCFOABMhDhJBZvS24844063 = -309162757;    float GaMxFhCFOABMhDhJBZvS32709673 = -280192796;    float GaMxFhCFOABMhDhJBZvS22435233 = -730926176;    float GaMxFhCFOABMhDhJBZvS41305639 = -653417998;    float GaMxFhCFOABMhDhJBZvS4523904 = -219999601;    float GaMxFhCFOABMhDhJBZvS74453436 = -696199861;    float GaMxFhCFOABMhDhJBZvS56410730 = -466249319;    float GaMxFhCFOABMhDhJBZvS91352162 = -235055963;    float GaMxFhCFOABMhDhJBZvS22729368 = -294387723;    float GaMxFhCFOABMhDhJBZvS43472957 = -588703436;    float GaMxFhCFOABMhDhJBZvS55552837 = -939393327;    float GaMxFhCFOABMhDhJBZvS34827048 = -532467718;    float GaMxFhCFOABMhDhJBZvS50296333 = -747467056;    float GaMxFhCFOABMhDhJBZvS28420163 = -439767956;    float GaMxFhCFOABMhDhJBZvS29799371 = -746093853;    float GaMxFhCFOABMhDhJBZvS25952247 = -118896704;    float GaMxFhCFOABMhDhJBZvS4061792 = -179870763;    float GaMxFhCFOABMhDhJBZvS32245999 = -443459445;    float GaMxFhCFOABMhDhJBZvS90911685 = -154194871;    float GaMxFhCFOABMhDhJBZvS90710337 = -389867300;    float GaMxFhCFOABMhDhJBZvS11310043 = -693631564;    float GaMxFhCFOABMhDhJBZvS2143945 = -157531604;    float GaMxFhCFOABMhDhJBZvS46811870 = -181549739;    float GaMxFhCFOABMhDhJBZvS68933131 = -116690977;    float GaMxFhCFOABMhDhJBZvS4301469 = -483088983;    float GaMxFhCFOABMhDhJBZvS87873112 = 49543581;    float GaMxFhCFOABMhDhJBZvS30912413 = -985505402;    float GaMxFhCFOABMhDhJBZvS79403860 = -917774187;    float GaMxFhCFOABMhDhJBZvS72799146 = -157005276;    float GaMxFhCFOABMhDhJBZvS3821922 = -349491169;    float GaMxFhCFOABMhDhJBZvS49032139 = -269678481;    float GaMxFhCFOABMhDhJBZvS79622400 = -437412232;    float GaMxFhCFOABMhDhJBZvS79402949 = -621394479;    float GaMxFhCFOABMhDhJBZvS21428653 = -251049283;    float GaMxFhCFOABMhDhJBZvS85409730 = -744023414;    float GaMxFhCFOABMhDhJBZvS84352631 = -690554652;    float GaMxFhCFOABMhDhJBZvS92020647 = -571225864;    float GaMxFhCFOABMhDhJBZvS69874135 = -219635671;    float GaMxFhCFOABMhDhJBZvS70710858 = 67542222;    float GaMxFhCFOABMhDhJBZvS28908413 = -841623357;    float GaMxFhCFOABMhDhJBZvS54290020 = -5034382;    float GaMxFhCFOABMhDhJBZvS81735096 = -136077673;    float GaMxFhCFOABMhDhJBZvS84953524 = -838415595;    float GaMxFhCFOABMhDhJBZvS3417003 = -812202348;    float GaMxFhCFOABMhDhJBZvS38388569 = -370800937;    float GaMxFhCFOABMhDhJBZvS18227484 = -384529168;    float GaMxFhCFOABMhDhJBZvS94304698 = -828828942;    float GaMxFhCFOABMhDhJBZvS67471689 = -98662430;    float GaMxFhCFOABMhDhJBZvS76954194 = -659245411;    float GaMxFhCFOABMhDhJBZvS4840726 = -942062019;    float GaMxFhCFOABMhDhJBZvS61805850 = -970014583;    float GaMxFhCFOABMhDhJBZvS10057931 = -861537671;    float GaMxFhCFOABMhDhJBZvS31543640 = -19298274;    float GaMxFhCFOABMhDhJBZvS566888 = -411394677;    float GaMxFhCFOABMhDhJBZvS66965542 = -619328976;    float GaMxFhCFOABMhDhJBZvS22700118 = -51631153;    float GaMxFhCFOABMhDhJBZvS85897802 = 1356943;    float GaMxFhCFOABMhDhJBZvS53502102 = -514235199;    float GaMxFhCFOABMhDhJBZvS37004171 = -70329016;    float GaMxFhCFOABMhDhJBZvS16650791 = -169543183;    float GaMxFhCFOABMhDhJBZvS43541023 = -710694459;    float GaMxFhCFOABMhDhJBZvS77006870 = -548475132;    float GaMxFhCFOABMhDhJBZvS18553016 = 21949313;    float GaMxFhCFOABMhDhJBZvS18907447 = -944896554;    float GaMxFhCFOABMhDhJBZvS94440818 = -219024955;    float GaMxFhCFOABMhDhJBZvS75930436 = -401981096;    float GaMxFhCFOABMhDhJBZvS55424098 = -911073240;    float GaMxFhCFOABMhDhJBZvS28867680 = -396417774;    float GaMxFhCFOABMhDhJBZvS43010433 = -695744543;    float GaMxFhCFOABMhDhJBZvS45446739 = 44460799;    float GaMxFhCFOABMhDhJBZvS33931599 = -547670841;    float GaMxFhCFOABMhDhJBZvS34187656 = -960235093;    float GaMxFhCFOABMhDhJBZvS61535141 = -411001667;    float GaMxFhCFOABMhDhJBZvS62003273 = -312571515;    float GaMxFhCFOABMhDhJBZvS36420317 = -284832919;    float GaMxFhCFOABMhDhJBZvS29574946 = -457553891;    float GaMxFhCFOABMhDhJBZvS17190421 = -319116010;    float GaMxFhCFOABMhDhJBZvS43394868 = -369347392;    float GaMxFhCFOABMhDhJBZvS30544562 = -745890040;    float GaMxFhCFOABMhDhJBZvS86073984 = 1440184;    float GaMxFhCFOABMhDhJBZvS93568413 = -121627477;    float GaMxFhCFOABMhDhJBZvS63440724 = -786842972;    float GaMxFhCFOABMhDhJBZvS2449667 = -158528777;    float GaMxFhCFOABMhDhJBZvS67958420 = -214943258;    float GaMxFhCFOABMhDhJBZvS42016071 = -379476587;    float GaMxFhCFOABMhDhJBZvS38974209 = -408140811;    float GaMxFhCFOABMhDhJBZvS48078760 = -318113958;    float GaMxFhCFOABMhDhJBZvS78836062 = -109999802;    float GaMxFhCFOABMhDhJBZvS54463111 = -631720307;    float GaMxFhCFOABMhDhJBZvS62709612 = 83003115;     GaMxFhCFOABMhDhJBZvS53054532 = GaMxFhCFOABMhDhJBZvS44601031;     GaMxFhCFOABMhDhJBZvS44601031 = GaMxFhCFOABMhDhJBZvS95891852;     GaMxFhCFOABMhDhJBZvS95891852 = GaMxFhCFOABMhDhJBZvS6753565;     GaMxFhCFOABMhDhJBZvS6753565 = GaMxFhCFOABMhDhJBZvS30792973;     GaMxFhCFOABMhDhJBZvS30792973 = GaMxFhCFOABMhDhJBZvS65867641;     GaMxFhCFOABMhDhJBZvS65867641 = GaMxFhCFOABMhDhJBZvS42303929;     GaMxFhCFOABMhDhJBZvS42303929 = GaMxFhCFOABMhDhJBZvS22455325;     GaMxFhCFOABMhDhJBZvS22455325 = GaMxFhCFOABMhDhJBZvS91277224;     GaMxFhCFOABMhDhJBZvS91277224 = GaMxFhCFOABMhDhJBZvS78275585;     GaMxFhCFOABMhDhJBZvS78275585 = GaMxFhCFOABMhDhJBZvS24844063;     GaMxFhCFOABMhDhJBZvS24844063 = GaMxFhCFOABMhDhJBZvS32709673;     GaMxFhCFOABMhDhJBZvS32709673 = GaMxFhCFOABMhDhJBZvS22435233;     GaMxFhCFOABMhDhJBZvS22435233 = GaMxFhCFOABMhDhJBZvS41305639;     GaMxFhCFOABMhDhJBZvS41305639 = GaMxFhCFOABMhDhJBZvS4523904;     GaMxFhCFOABMhDhJBZvS4523904 = GaMxFhCFOABMhDhJBZvS74453436;     GaMxFhCFOABMhDhJBZvS74453436 = GaMxFhCFOABMhDhJBZvS56410730;     GaMxFhCFOABMhDhJBZvS56410730 = GaMxFhCFOABMhDhJBZvS91352162;     GaMxFhCFOABMhDhJBZvS91352162 = GaMxFhCFOABMhDhJBZvS22729368;     GaMxFhCFOABMhDhJBZvS22729368 = GaMxFhCFOABMhDhJBZvS43472957;     GaMxFhCFOABMhDhJBZvS43472957 = GaMxFhCFOABMhDhJBZvS55552837;     GaMxFhCFOABMhDhJBZvS55552837 = GaMxFhCFOABMhDhJBZvS34827048;     GaMxFhCFOABMhDhJBZvS34827048 = GaMxFhCFOABMhDhJBZvS50296333;     GaMxFhCFOABMhDhJBZvS50296333 = GaMxFhCFOABMhDhJBZvS28420163;     GaMxFhCFOABMhDhJBZvS28420163 = GaMxFhCFOABMhDhJBZvS29799371;     GaMxFhCFOABMhDhJBZvS29799371 = GaMxFhCFOABMhDhJBZvS25952247;     GaMxFhCFOABMhDhJBZvS25952247 = GaMxFhCFOABMhDhJBZvS4061792;     GaMxFhCFOABMhDhJBZvS4061792 = GaMxFhCFOABMhDhJBZvS32245999;     GaMxFhCFOABMhDhJBZvS32245999 = GaMxFhCFOABMhDhJBZvS90911685;     GaMxFhCFOABMhDhJBZvS90911685 = GaMxFhCFOABMhDhJBZvS90710337;     GaMxFhCFOABMhDhJBZvS90710337 = GaMxFhCFOABMhDhJBZvS11310043;     GaMxFhCFOABMhDhJBZvS11310043 = GaMxFhCFOABMhDhJBZvS2143945;     GaMxFhCFOABMhDhJBZvS2143945 = GaMxFhCFOABMhDhJBZvS46811870;     GaMxFhCFOABMhDhJBZvS46811870 = GaMxFhCFOABMhDhJBZvS68933131;     GaMxFhCFOABMhDhJBZvS68933131 = GaMxFhCFOABMhDhJBZvS4301469;     GaMxFhCFOABMhDhJBZvS4301469 = GaMxFhCFOABMhDhJBZvS87873112;     GaMxFhCFOABMhDhJBZvS87873112 = GaMxFhCFOABMhDhJBZvS30912413;     GaMxFhCFOABMhDhJBZvS30912413 = GaMxFhCFOABMhDhJBZvS79403860;     GaMxFhCFOABMhDhJBZvS79403860 = GaMxFhCFOABMhDhJBZvS72799146;     GaMxFhCFOABMhDhJBZvS72799146 = GaMxFhCFOABMhDhJBZvS3821922;     GaMxFhCFOABMhDhJBZvS3821922 = GaMxFhCFOABMhDhJBZvS49032139;     GaMxFhCFOABMhDhJBZvS49032139 = GaMxFhCFOABMhDhJBZvS79622400;     GaMxFhCFOABMhDhJBZvS79622400 = GaMxFhCFOABMhDhJBZvS79402949;     GaMxFhCFOABMhDhJBZvS79402949 = GaMxFhCFOABMhDhJBZvS21428653;     GaMxFhCFOABMhDhJBZvS21428653 = GaMxFhCFOABMhDhJBZvS85409730;     GaMxFhCFOABMhDhJBZvS85409730 = GaMxFhCFOABMhDhJBZvS84352631;     GaMxFhCFOABMhDhJBZvS84352631 = GaMxFhCFOABMhDhJBZvS92020647;     GaMxFhCFOABMhDhJBZvS92020647 = GaMxFhCFOABMhDhJBZvS69874135;     GaMxFhCFOABMhDhJBZvS69874135 = GaMxFhCFOABMhDhJBZvS70710858;     GaMxFhCFOABMhDhJBZvS70710858 = GaMxFhCFOABMhDhJBZvS28908413;     GaMxFhCFOABMhDhJBZvS28908413 = GaMxFhCFOABMhDhJBZvS54290020;     GaMxFhCFOABMhDhJBZvS54290020 = GaMxFhCFOABMhDhJBZvS81735096;     GaMxFhCFOABMhDhJBZvS81735096 = GaMxFhCFOABMhDhJBZvS84953524;     GaMxFhCFOABMhDhJBZvS84953524 = GaMxFhCFOABMhDhJBZvS3417003;     GaMxFhCFOABMhDhJBZvS3417003 = GaMxFhCFOABMhDhJBZvS38388569;     GaMxFhCFOABMhDhJBZvS38388569 = GaMxFhCFOABMhDhJBZvS18227484;     GaMxFhCFOABMhDhJBZvS18227484 = GaMxFhCFOABMhDhJBZvS94304698;     GaMxFhCFOABMhDhJBZvS94304698 = GaMxFhCFOABMhDhJBZvS67471689;     GaMxFhCFOABMhDhJBZvS67471689 = GaMxFhCFOABMhDhJBZvS76954194;     GaMxFhCFOABMhDhJBZvS76954194 = GaMxFhCFOABMhDhJBZvS4840726;     GaMxFhCFOABMhDhJBZvS4840726 = GaMxFhCFOABMhDhJBZvS61805850;     GaMxFhCFOABMhDhJBZvS61805850 = GaMxFhCFOABMhDhJBZvS10057931;     GaMxFhCFOABMhDhJBZvS10057931 = GaMxFhCFOABMhDhJBZvS31543640;     GaMxFhCFOABMhDhJBZvS31543640 = GaMxFhCFOABMhDhJBZvS566888;     GaMxFhCFOABMhDhJBZvS566888 = GaMxFhCFOABMhDhJBZvS66965542;     GaMxFhCFOABMhDhJBZvS66965542 = GaMxFhCFOABMhDhJBZvS22700118;     GaMxFhCFOABMhDhJBZvS22700118 = GaMxFhCFOABMhDhJBZvS85897802;     GaMxFhCFOABMhDhJBZvS85897802 = GaMxFhCFOABMhDhJBZvS53502102;     GaMxFhCFOABMhDhJBZvS53502102 = GaMxFhCFOABMhDhJBZvS37004171;     GaMxFhCFOABMhDhJBZvS37004171 = GaMxFhCFOABMhDhJBZvS16650791;     GaMxFhCFOABMhDhJBZvS16650791 = GaMxFhCFOABMhDhJBZvS43541023;     GaMxFhCFOABMhDhJBZvS43541023 = GaMxFhCFOABMhDhJBZvS77006870;     GaMxFhCFOABMhDhJBZvS77006870 = GaMxFhCFOABMhDhJBZvS18553016;     GaMxFhCFOABMhDhJBZvS18553016 = GaMxFhCFOABMhDhJBZvS18907447;     GaMxFhCFOABMhDhJBZvS18907447 = GaMxFhCFOABMhDhJBZvS94440818;     GaMxFhCFOABMhDhJBZvS94440818 = GaMxFhCFOABMhDhJBZvS75930436;     GaMxFhCFOABMhDhJBZvS75930436 = GaMxFhCFOABMhDhJBZvS55424098;     GaMxFhCFOABMhDhJBZvS55424098 = GaMxFhCFOABMhDhJBZvS28867680;     GaMxFhCFOABMhDhJBZvS28867680 = GaMxFhCFOABMhDhJBZvS43010433;     GaMxFhCFOABMhDhJBZvS43010433 = GaMxFhCFOABMhDhJBZvS45446739;     GaMxFhCFOABMhDhJBZvS45446739 = GaMxFhCFOABMhDhJBZvS33931599;     GaMxFhCFOABMhDhJBZvS33931599 = GaMxFhCFOABMhDhJBZvS34187656;     GaMxFhCFOABMhDhJBZvS34187656 = GaMxFhCFOABMhDhJBZvS61535141;     GaMxFhCFOABMhDhJBZvS61535141 = GaMxFhCFOABMhDhJBZvS62003273;     GaMxFhCFOABMhDhJBZvS62003273 = GaMxFhCFOABMhDhJBZvS36420317;     GaMxFhCFOABMhDhJBZvS36420317 = GaMxFhCFOABMhDhJBZvS29574946;     GaMxFhCFOABMhDhJBZvS29574946 = GaMxFhCFOABMhDhJBZvS17190421;     GaMxFhCFOABMhDhJBZvS17190421 = GaMxFhCFOABMhDhJBZvS43394868;     GaMxFhCFOABMhDhJBZvS43394868 = GaMxFhCFOABMhDhJBZvS30544562;     GaMxFhCFOABMhDhJBZvS30544562 = GaMxFhCFOABMhDhJBZvS86073984;     GaMxFhCFOABMhDhJBZvS86073984 = GaMxFhCFOABMhDhJBZvS93568413;     GaMxFhCFOABMhDhJBZvS93568413 = GaMxFhCFOABMhDhJBZvS63440724;     GaMxFhCFOABMhDhJBZvS63440724 = GaMxFhCFOABMhDhJBZvS2449667;     GaMxFhCFOABMhDhJBZvS2449667 = GaMxFhCFOABMhDhJBZvS67958420;     GaMxFhCFOABMhDhJBZvS67958420 = GaMxFhCFOABMhDhJBZvS42016071;     GaMxFhCFOABMhDhJBZvS42016071 = GaMxFhCFOABMhDhJBZvS38974209;     GaMxFhCFOABMhDhJBZvS38974209 = GaMxFhCFOABMhDhJBZvS48078760;     GaMxFhCFOABMhDhJBZvS48078760 = GaMxFhCFOABMhDhJBZvS78836062;     GaMxFhCFOABMhDhJBZvS78836062 = GaMxFhCFOABMhDhJBZvS54463111;     GaMxFhCFOABMhDhJBZvS54463111 = GaMxFhCFOABMhDhJBZvS62709612;     GaMxFhCFOABMhDhJBZvS62709612 = GaMxFhCFOABMhDhJBZvS53054532;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void GCPLsArhHYbmQlcgOGoK64719648() {     float OLVSDKQemdFXchwyArEs83578272 = -719520850;    float OLVSDKQemdFXchwyArEs64792563 = -83661908;    float OLVSDKQemdFXchwyArEs55696376 = -658212538;    float OLVSDKQemdFXchwyArEs69456241 = -639441038;    float OLVSDKQemdFXchwyArEs41396584 = -570130459;    float OLVSDKQemdFXchwyArEs83782357 = -983691592;    float OLVSDKQemdFXchwyArEs8156221 = -317361470;    float OLVSDKQemdFXchwyArEs93722655 = -359906825;    float OLVSDKQemdFXchwyArEs84015632 = -90128185;    float OLVSDKQemdFXchwyArEs96146253 = -678383082;    float OLVSDKQemdFXchwyArEs96021556 = -375512547;    float OLVSDKQemdFXchwyArEs47079655 = -500238044;    float OLVSDKQemdFXchwyArEs21879717 = -58641992;    float OLVSDKQemdFXchwyArEs75194874 = 7426413;    float OLVSDKQemdFXchwyArEs50462578 = -975306875;    float OLVSDKQemdFXchwyArEs76443471 = -924002490;    float OLVSDKQemdFXchwyArEs20264567 = -618992850;    float OLVSDKQemdFXchwyArEs71040077 = -705640772;    float OLVSDKQemdFXchwyArEs23629826 = -139660796;    float OLVSDKQemdFXchwyArEs2953860 = -86197544;    float OLVSDKQemdFXchwyArEs42683545 = -521263969;    float OLVSDKQemdFXchwyArEs42715297 = -383154359;    float OLVSDKQemdFXchwyArEs28221323 = -748589740;    float OLVSDKQemdFXchwyArEs15261130 = -104374617;    float OLVSDKQemdFXchwyArEs29149244 = -160161060;    float OLVSDKQemdFXchwyArEs66902381 = -868358897;    float OLVSDKQemdFXchwyArEs82910637 = -890719473;    float OLVSDKQemdFXchwyArEs25708731 = -490934902;    float OLVSDKQemdFXchwyArEs15014787 = -672871619;    float OLVSDKQemdFXchwyArEs76115820 = -992717823;    float OLVSDKQemdFXchwyArEs23271806 = -762172014;    float OLVSDKQemdFXchwyArEs43469456 = -632959788;    float OLVSDKQemdFXchwyArEs92151295 = -703772243;    float OLVSDKQemdFXchwyArEs10530171 = -482939937;    float OLVSDKQemdFXchwyArEs17318680 = -744528583;    float OLVSDKQemdFXchwyArEs5342413 = -21594993;    float OLVSDKQemdFXchwyArEs52455672 = -308196050;    float OLVSDKQemdFXchwyArEs75318485 = -834643721;    float OLVSDKQemdFXchwyArEs99627816 = -858442221;    float OLVSDKQemdFXchwyArEs47381301 = -689749681;    float OLVSDKQemdFXchwyArEs92369666 = -694822059;    float OLVSDKQemdFXchwyArEs49885344 = -440756109;    float OLVSDKQemdFXchwyArEs58623909 = -822505499;    float OLVSDKQemdFXchwyArEs37082987 = -348182176;    float OLVSDKQemdFXchwyArEs60403885 = -783474703;    float OLVSDKQemdFXchwyArEs80426464 = -743279069;    float OLVSDKQemdFXchwyArEs94653008 = -709133319;    float OLVSDKQemdFXchwyArEs88947781 = -522300052;    float OLVSDKQemdFXchwyArEs77898253 = -748223366;    float OLVSDKQemdFXchwyArEs63066849 = -564392119;    float OLVSDKQemdFXchwyArEs46428663 = -171863210;    float OLVSDKQemdFXchwyArEs33470308 = -511406999;    float OLVSDKQemdFXchwyArEs34438814 = -202374144;    float OLVSDKQemdFXchwyArEs93299812 = -107508526;    float OLVSDKQemdFXchwyArEs22036103 = -292366400;    float OLVSDKQemdFXchwyArEs40862975 = -236366492;    float OLVSDKQemdFXchwyArEs36571241 = -335072168;    float OLVSDKQemdFXchwyArEs40435246 = -453837921;    float OLVSDKQemdFXchwyArEs40306998 = -379279979;    float OLVSDKQemdFXchwyArEs74494202 = -701771563;    float OLVSDKQemdFXchwyArEs871721 = 7027881;    float OLVSDKQemdFXchwyArEs82447490 = -826426569;    float OLVSDKQemdFXchwyArEs78707868 = -687035207;    float OLVSDKQemdFXchwyArEs7899813 = -97410362;    float OLVSDKQemdFXchwyArEs72874448 = -916211069;    float OLVSDKQemdFXchwyArEs52552100 = -742552759;    float OLVSDKQemdFXchwyArEs54928360 = -796465802;    float OLVSDKQemdFXchwyArEs11349546 = -575702056;    float OLVSDKQemdFXchwyArEs57876194 = -248045004;    float OLVSDKQemdFXchwyArEs45120166 = -853711882;    float OLVSDKQemdFXchwyArEs23987800 = -515806441;    float OLVSDKQemdFXchwyArEs44946081 = -784349130;    float OLVSDKQemdFXchwyArEs71412260 = -847198551;    float OLVSDKQemdFXchwyArEs76248525 = -449911115;    float OLVSDKQemdFXchwyArEs10584193 = -391375486;    float OLVSDKQemdFXchwyArEs92798201 = 19492139;    float OLVSDKQemdFXchwyArEs84091388 = -560648860;    float OLVSDKQemdFXchwyArEs91138335 = -300407564;    float OLVSDKQemdFXchwyArEs54857245 = -320899915;    float OLVSDKQemdFXchwyArEs48722779 = -416881991;    float OLVSDKQemdFXchwyArEs72249373 = -59225578;    float OLVSDKQemdFXchwyArEs93962855 = -268419421;    float OLVSDKQemdFXchwyArEs47810477 = -742711536;    float OLVSDKQemdFXchwyArEs51947938 = -8479500;    float OLVSDKQemdFXchwyArEs29687157 = -720854614;    float OLVSDKQemdFXchwyArEs89801497 = -150765016;    float OLVSDKQemdFXchwyArEs9030643 = -330585644;    float OLVSDKQemdFXchwyArEs98851483 = -496263717;    float OLVSDKQemdFXchwyArEs88494067 = -90573538;    float OLVSDKQemdFXchwyArEs76455704 = -408162092;    float OLVSDKQemdFXchwyArEs68771171 = -686522826;    float OLVSDKQemdFXchwyArEs12020427 = -854358129;    float OLVSDKQemdFXchwyArEs35011488 = -355363743;    float OLVSDKQemdFXchwyArEs25133615 = -56670659;    float OLVSDKQemdFXchwyArEs46509581 = -596777562;    float OLVSDKQemdFXchwyArEs9922177 = -868395491;    float OLVSDKQemdFXchwyArEs71177475 = -753720903;    float OLVSDKQemdFXchwyArEs50724096 = -625095137;    float OLVSDKQemdFXchwyArEs64208539 = -431971108;    float OLVSDKQemdFXchwyArEs7851785 = -719520850;     OLVSDKQemdFXchwyArEs83578272 = OLVSDKQemdFXchwyArEs64792563;     OLVSDKQemdFXchwyArEs64792563 = OLVSDKQemdFXchwyArEs55696376;     OLVSDKQemdFXchwyArEs55696376 = OLVSDKQemdFXchwyArEs69456241;     OLVSDKQemdFXchwyArEs69456241 = OLVSDKQemdFXchwyArEs41396584;     OLVSDKQemdFXchwyArEs41396584 = OLVSDKQemdFXchwyArEs83782357;     OLVSDKQemdFXchwyArEs83782357 = OLVSDKQemdFXchwyArEs8156221;     OLVSDKQemdFXchwyArEs8156221 = OLVSDKQemdFXchwyArEs93722655;     OLVSDKQemdFXchwyArEs93722655 = OLVSDKQemdFXchwyArEs84015632;     OLVSDKQemdFXchwyArEs84015632 = OLVSDKQemdFXchwyArEs96146253;     OLVSDKQemdFXchwyArEs96146253 = OLVSDKQemdFXchwyArEs96021556;     OLVSDKQemdFXchwyArEs96021556 = OLVSDKQemdFXchwyArEs47079655;     OLVSDKQemdFXchwyArEs47079655 = OLVSDKQemdFXchwyArEs21879717;     OLVSDKQemdFXchwyArEs21879717 = OLVSDKQemdFXchwyArEs75194874;     OLVSDKQemdFXchwyArEs75194874 = OLVSDKQemdFXchwyArEs50462578;     OLVSDKQemdFXchwyArEs50462578 = OLVSDKQemdFXchwyArEs76443471;     OLVSDKQemdFXchwyArEs76443471 = OLVSDKQemdFXchwyArEs20264567;     OLVSDKQemdFXchwyArEs20264567 = OLVSDKQemdFXchwyArEs71040077;     OLVSDKQemdFXchwyArEs71040077 = OLVSDKQemdFXchwyArEs23629826;     OLVSDKQemdFXchwyArEs23629826 = OLVSDKQemdFXchwyArEs2953860;     OLVSDKQemdFXchwyArEs2953860 = OLVSDKQemdFXchwyArEs42683545;     OLVSDKQemdFXchwyArEs42683545 = OLVSDKQemdFXchwyArEs42715297;     OLVSDKQemdFXchwyArEs42715297 = OLVSDKQemdFXchwyArEs28221323;     OLVSDKQemdFXchwyArEs28221323 = OLVSDKQemdFXchwyArEs15261130;     OLVSDKQemdFXchwyArEs15261130 = OLVSDKQemdFXchwyArEs29149244;     OLVSDKQemdFXchwyArEs29149244 = OLVSDKQemdFXchwyArEs66902381;     OLVSDKQemdFXchwyArEs66902381 = OLVSDKQemdFXchwyArEs82910637;     OLVSDKQemdFXchwyArEs82910637 = OLVSDKQemdFXchwyArEs25708731;     OLVSDKQemdFXchwyArEs25708731 = OLVSDKQemdFXchwyArEs15014787;     OLVSDKQemdFXchwyArEs15014787 = OLVSDKQemdFXchwyArEs76115820;     OLVSDKQemdFXchwyArEs76115820 = OLVSDKQemdFXchwyArEs23271806;     OLVSDKQemdFXchwyArEs23271806 = OLVSDKQemdFXchwyArEs43469456;     OLVSDKQemdFXchwyArEs43469456 = OLVSDKQemdFXchwyArEs92151295;     OLVSDKQemdFXchwyArEs92151295 = OLVSDKQemdFXchwyArEs10530171;     OLVSDKQemdFXchwyArEs10530171 = OLVSDKQemdFXchwyArEs17318680;     OLVSDKQemdFXchwyArEs17318680 = OLVSDKQemdFXchwyArEs5342413;     OLVSDKQemdFXchwyArEs5342413 = OLVSDKQemdFXchwyArEs52455672;     OLVSDKQemdFXchwyArEs52455672 = OLVSDKQemdFXchwyArEs75318485;     OLVSDKQemdFXchwyArEs75318485 = OLVSDKQemdFXchwyArEs99627816;     OLVSDKQemdFXchwyArEs99627816 = OLVSDKQemdFXchwyArEs47381301;     OLVSDKQemdFXchwyArEs47381301 = OLVSDKQemdFXchwyArEs92369666;     OLVSDKQemdFXchwyArEs92369666 = OLVSDKQemdFXchwyArEs49885344;     OLVSDKQemdFXchwyArEs49885344 = OLVSDKQemdFXchwyArEs58623909;     OLVSDKQemdFXchwyArEs58623909 = OLVSDKQemdFXchwyArEs37082987;     OLVSDKQemdFXchwyArEs37082987 = OLVSDKQemdFXchwyArEs60403885;     OLVSDKQemdFXchwyArEs60403885 = OLVSDKQemdFXchwyArEs80426464;     OLVSDKQemdFXchwyArEs80426464 = OLVSDKQemdFXchwyArEs94653008;     OLVSDKQemdFXchwyArEs94653008 = OLVSDKQemdFXchwyArEs88947781;     OLVSDKQemdFXchwyArEs88947781 = OLVSDKQemdFXchwyArEs77898253;     OLVSDKQemdFXchwyArEs77898253 = OLVSDKQemdFXchwyArEs63066849;     OLVSDKQemdFXchwyArEs63066849 = OLVSDKQemdFXchwyArEs46428663;     OLVSDKQemdFXchwyArEs46428663 = OLVSDKQemdFXchwyArEs33470308;     OLVSDKQemdFXchwyArEs33470308 = OLVSDKQemdFXchwyArEs34438814;     OLVSDKQemdFXchwyArEs34438814 = OLVSDKQemdFXchwyArEs93299812;     OLVSDKQemdFXchwyArEs93299812 = OLVSDKQemdFXchwyArEs22036103;     OLVSDKQemdFXchwyArEs22036103 = OLVSDKQemdFXchwyArEs40862975;     OLVSDKQemdFXchwyArEs40862975 = OLVSDKQemdFXchwyArEs36571241;     OLVSDKQemdFXchwyArEs36571241 = OLVSDKQemdFXchwyArEs40435246;     OLVSDKQemdFXchwyArEs40435246 = OLVSDKQemdFXchwyArEs40306998;     OLVSDKQemdFXchwyArEs40306998 = OLVSDKQemdFXchwyArEs74494202;     OLVSDKQemdFXchwyArEs74494202 = OLVSDKQemdFXchwyArEs871721;     OLVSDKQemdFXchwyArEs871721 = OLVSDKQemdFXchwyArEs82447490;     OLVSDKQemdFXchwyArEs82447490 = OLVSDKQemdFXchwyArEs78707868;     OLVSDKQemdFXchwyArEs78707868 = OLVSDKQemdFXchwyArEs7899813;     OLVSDKQemdFXchwyArEs7899813 = OLVSDKQemdFXchwyArEs72874448;     OLVSDKQemdFXchwyArEs72874448 = OLVSDKQemdFXchwyArEs52552100;     OLVSDKQemdFXchwyArEs52552100 = OLVSDKQemdFXchwyArEs54928360;     OLVSDKQemdFXchwyArEs54928360 = OLVSDKQemdFXchwyArEs11349546;     OLVSDKQemdFXchwyArEs11349546 = OLVSDKQemdFXchwyArEs57876194;     OLVSDKQemdFXchwyArEs57876194 = OLVSDKQemdFXchwyArEs45120166;     OLVSDKQemdFXchwyArEs45120166 = OLVSDKQemdFXchwyArEs23987800;     OLVSDKQemdFXchwyArEs23987800 = OLVSDKQemdFXchwyArEs44946081;     OLVSDKQemdFXchwyArEs44946081 = OLVSDKQemdFXchwyArEs71412260;     OLVSDKQemdFXchwyArEs71412260 = OLVSDKQemdFXchwyArEs76248525;     OLVSDKQemdFXchwyArEs76248525 = OLVSDKQemdFXchwyArEs10584193;     OLVSDKQemdFXchwyArEs10584193 = OLVSDKQemdFXchwyArEs92798201;     OLVSDKQemdFXchwyArEs92798201 = OLVSDKQemdFXchwyArEs84091388;     OLVSDKQemdFXchwyArEs84091388 = OLVSDKQemdFXchwyArEs91138335;     OLVSDKQemdFXchwyArEs91138335 = OLVSDKQemdFXchwyArEs54857245;     OLVSDKQemdFXchwyArEs54857245 = OLVSDKQemdFXchwyArEs48722779;     OLVSDKQemdFXchwyArEs48722779 = OLVSDKQemdFXchwyArEs72249373;     OLVSDKQemdFXchwyArEs72249373 = OLVSDKQemdFXchwyArEs93962855;     OLVSDKQemdFXchwyArEs93962855 = OLVSDKQemdFXchwyArEs47810477;     OLVSDKQemdFXchwyArEs47810477 = OLVSDKQemdFXchwyArEs51947938;     OLVSDKQemdFXchwyArEs51947938 = OLVSDKQemdFXchwyArEs29687157;     OLVSDKQemdFXchwyArEs29687157 = OLVSDKQemdFXchwyArEs89801497;     OLVSDKQemdFXchwyArEs89801497 = OLVSDKQemdFXchwyArEs9030643;     OLVSDKQemdFXchwyArEs9030643 = OLVSDKQemdFXchwyArEs98851483;     OLVSDKQemdFXchwyArEs98851483 = OLVSDKQemdFXchwyArEs88494067;     OLVSDKQemdFXchwyArEs88494067 = OLVSDKQemdFXchwyArEs76455704;     OLVSDKQemdFXchwyArEs76455704 = OLVSDKQemdFXchwyArEs68771171;     OLVSDKQemdFXchwyArEs68771171 = OLVSDKQemdFXchwyArEs12020427;     OLVSDKQemdFXchwyArEs12020427 = OLVSDKQemdFXchwyArEs35011488;     OLVSDKQemdFXchwyArEs35011488 = OLVSDKQemdFXchwyArEs25133615;     OLVSDKQemdFXchwyArEs25133615 = OLVSDKQemdFXchwyArEs46509581;     OLVSDKQemdFXchwyArEs46509581 = OLVSDKQemdFXchwyArEs9922177;     OLVSDKQemdFXchwyArEs9922177 = OLVSDKQemdFXchwyArEs71177475;     OLVSDKQemdFXchwyArEs71177475 = OLVSDKQemdFXchwyArEs50724096;     OLVSDKQemdFXchwyArEs50724096 = OLVSDKQemdFXchwyArEs64208539;     OLVSDKQemdFXchwyArEs64208539 = OLVSDKQemdFXchwyArEs7851785;     OLVSDKQemdFXchwyArEs7851785 = OLVSDKQemdFXchwyArEs83578272;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oZrNFHtyQGazDHzlJOBa71378392() {     float zMBPaxYfZjQUEhUTKxMl83571801 = -592044992;    float zMBPaxYfZjQUEhUTKxMl3364880 = -126257033;    float zMBPaxYfZjQUEhUTKxMl57009588 = -615341151;    float zMBPaxYfZjQUEhUTKxMl54536102 = -459981868;    float zMBPaxYfZjQUEhUTKxMl50236686 = 61514220;    float zMBPaxYfZjQUEhUTKxMl18587790 = -683124404;    float zMBPaxYfZjQUEhUTKxMl76703556 = -21897937;    float zMBPaxYfZjQUEhUTKxMl62936706 = -64084142;    float zMBPaxYfZjQUEhUTKxMl66755797 = -903774255;    float zMBPaxYfZjQUEhUTKxMl92785767 = -315189953;    float zMBPaxYfZjQUEhUTKxMl16342162 = -334984619;    float zMBPaxYfZjQUEhUTKxMl61371332 = -235716407;    float zMBPaxYfZjQUEhUTKxMl53732630 = -80300466;    float zMBPaxYfZjQUEhUTKxMl29039094 = -337726639;    float zMBPaxYfZjQUEhUTKxMl95634687 = -938019727;    float zMBPaxYfZjQUEhUTKxMl51008019 = -156798141;    float zMBPaxYfZjQUEhUTKxMl76117326 = -438607420;    float zMBPaxYfZjQUEhUTKxMl94040609 = -2712394;    float zMBPaxYfZjQUEhUTKxMl44058096 = -757643062;    float zMBPaxYfZjQUEhUTKxMl75027622 = -698221995;    float zMBPaxYfZjQUEhUTKxMl22352969 = -231730343;    float zMBPaxYfZjQUEhUTKxMl21011563 = -893146242;    float zMBPaxYfZjQUEhUTKxMl65099719 = -107547582;    float zMBPaxYfZjQUEhUTKxMl58911238 = 14704860;    float zMBPaxYfZjQUEhUTKxMl57591933 = -586854913;    float zMBPaxYfZjQUEhUTKxMl83476458 = -776564825;    float zMBPaxYfZjQUEhUTKxMl53132219 = -91519186;    float zMBPaxYfZjQUEhUTKxMl61112255 = 98110;    float zMBPaxYfZjQUEhUTKxMl27693234 = -218941934;    float zMBPaxYfZjQUEhUTKxMl52205691 = -668149197;    float zMBPaxYfZjQUEhUTKxMl56684484 = -49265249;    float zMBPaxYfZjQUEhUTKxMl28193314 = 75218450;    float zMBPaxYfZjQUEhUTKxMl77603718 = -905716459;    float zMBPaxYfZjQUEhUTKxMl10813008 = -359621669;    float zMBPaxYfZjQUEhUTKxMl99676717 = 96157492;    float zMBPaxYfZjQUEhUTKxMl48170602 = -101360739;    float zMBPaxYfZjQUEhUTKxMl64397040 = -407849428;    float zMBPaxYfZjQUEhUTKxMl45236165 = -829906400;    float zMBPaxYfZjQUEhUTKxMl34941414 = -471123638;    float zMBPaxYfZjQUEhUTKxMl19254194 = -299181367;    float zMBPaxYfZjQUEhUTKxMl80225334 = -782765957;    float zMBPaxYfZjQUEhUTKxMl35583005 = -634548191;    float zMBPaxYfZjQUEhUTKxMl18907269 = -281635753;    float zMBPaxYfZjQUEhUTKxMl96314885 = 94853380;    float zMBPaxYfZjQUEhUTKxMl44421937 = -156734104;    float zMBPaxYfZjQUEhUTKxMl20854433 = -555323740;    float zMBPaxYfZjQUEhUTKxMl22350802 = -90413941;    float zMBPaxYfZjQUEhUTKxMl64331362 = -31061408;    float zMBPaxYfZjQUEhUTKxMl96556643 = -311987228;    float zMBPaxYfZjQUEhUTKxMl83112271 = -151417006;    float zMBPaxYfZjQUEhUTKxMl59688033 = -850418217;    float zMBPaxYfZjQUEhUTKxMl48017566 = -893552747;    float zMBPaxYfZjQUEhUTKxMl9887162 = -94428152;    float zMBPaxYfZjQUEhUTKxMl3550083 = -639428377;    float zMBPaxYfZjQUEhUTKxMl49025424 = -49265677;    float zMBPaxYfZjQUEhUTKxMl62560238 = -698898750;    float zMBPaxYfZjQUEhUTKxMl38265161 = 81290549;    float zMBPaxYfZjQUEhUTKxMl98098349 = -530046012;    float zMBPaxYfZjQUEhUTKxMl96944169 = -873126956;    float zMBPaxYfZjQUEhUTKxMl66760227 = -161920956;    float zMBPaxYfZjQUEhUTKxMl65455571 = -491605218;    float zMBPaxYfZjQUEhUTKxMl15591302 = 78003953;    float zMBPaxYfZjQUEhUTKxMl35243472 = -845142209;    float zMBPaxYfZjQUEhUTKxMl14550107 = -135625059;    float zMBPaxYfZjQUEhUTKxMl36101283 = -165924705;    float zMBPaxYfZjQUEhUTKxMl88148847 = -310203069;    float zMBPaxYfZjQUEhUTKxMl83767613 = -329999949;    float zMBPaxYfZjQUEhUTKxMl42919622 = -720678798;    float zMBPaxYfZjQUEhUTKxMl29362376 = -333884131;    float zMBPaxYfZjQUEhUTKxMl47464086 = -736658988;    float zMBPaxYfZjQUEhUTKxMl86610979 = -748948713;    float zMBPaxYfZjQUEhUTKxMl30881161 = -608701021;    float zMBPaxYfZjQUEhUTKxMl59099196 = -531588757;    float zMBPaxYfZjQUEhUTKxMl24803902 = -358461696;    float zMBPaxYfZjQUEhUTKxMl94802287 = -915456039;    float zMBPaxYfZjQUEhUTKxMl86769964 = -597182152;    float zMBPaxYfZjQUEhUTKxMl2104295 = -511510489;    float zMBPaxYfZjQUEhUTKxMl68784833 = -102400963;    float zMBPaxYfZjQUEhUTKxMl14489301 = -828561036;    float zMBPaxYfZjQUEhUTKxMl36737500 = 68468827;    float zMBPaxYfZjQUEhUTKxMl61125657 = -586150884;    float zMBPaxYfZjQUEhUTKxMl88800856 = 39542222;    float zMBPaxYfZjQUEhUTKxMl64555612 = -687914663;    float zMBPaxYfZjQUEhUTKxMl44580962 = 32475072;    float zMBPaxYfZjQUEhUTKxMl92517657 = -817730980;    float zMBPaxYfZjQUEhUTKxMl8666919 = -155712502;    float zMBPaxYfZjQUEhUTKxMl18306152 = -830353398;    float zMBPaxYfZjQUEhUTKxMl74053635 = -166288082;    float zMBPaxYfZjQUEhUTKxMl61787584 = -210355992;    float zMBPaxYfZjQUEhUTKxMl37116480 = -204943758;    float zMBPaxYfZjQUEhUTKxMl9905442 = -82651289;    float zMBPaxYfZjQUEhUTKxMl66298690 = -877803416;    float zMBPaxYfZjQUEhUTKxMl48291996 = -956779445;    float zMBPaxYfZjQUEhUTKxMl68181186 = -209202682;    float zMBPaxYfZjQUEhUTKxMl53798623 = -807576149;    float zMBPaxYfZjQUEhUTKxMl64634033 = -760769910;    float zMBPaxYfZjQUEhUTKxMl339533 = -789405983;    float zMBPaxYfZjQUEhUTKxMl4357163 = -46010695;    float zMBPaxYfZjQUEhUTKxMl60213602 = -739221915;    float zMBPaxYfZjQUEhUTKxMl56273089 = -592044992;     zMBPaxYfZjQUEhUTKxMl83571801 = zMBPaxYfZjQUEhUTKxMl3364880;     zMBPaxYfZjQUEhUTKxMl3364880 = zMBPaxYfZjQUEhUTKxMl57009588;     zMBPaxYfZjQUEhUTKxMl57009588 = zMBPaxYfZjQUEhUTKxMl54536102;     zMBPaxYfZjQUEhUTKxMl54536102 = zMBPaxYfZjQUEhUTKxMl50236686;     zMBPaxYfZjQUEhUTKxMl50236686 = zMBPaxYfZjQUEhUTKxMl18587790;     zMBPaxYfZjQUEhUTKxMl18587790 = zMBPaxYfZjQUEhUTKxMl76703556;     zMBPaxYfZjQUEhUTKxMl76703556 = zMBPaxYfZjQUEhUTKxMl62936706;     zMBPaxYfZjQUEhUTKxMl62936706 = zMBPaxYfZjQUEhUTKxMl66755797;     zMBPaxYfZjQUEhUTKxMl66755797 = zMBPaxYfZjQUEhUTKxMl92785767;     zMBPaxYfZjQUEhUTKxMl92785767 = zMBPaxYfZjQUEhUTKxMl16342162;     zMBPaxYfZjQUEhUTKxMl16342162 = zMBPaxYfZjQUEhUTKxMl61371332;     zMBPaxYfZjQUEhUTKxMl61371332 = zMBPaxYfZjQUEhUTKxMl53732630;     zMBPaxYfZjQUEhUTKxMl53732630 = zMBPaxYfZjQUEhUTKxMl29039094;     zMBPaxYfZjQUEhUTKxMl29039094 = zMBPaxYfZjQUEhUTKxMl95634687;     zMBPaxYfZjQUEhUTKxMl95634687 = zMBPaxYfZjQUEhUTKxMl51008019;     zMBPaxYfZjQUEhUTKxMl51008019 = zMBPaxYfZjQUEhUTKxMl76117326;     zMBPaxYfZjQUEhUTKxMl76117326 = zMBPaxYfZjQUEhUTKxMl94040609;     zMBPaxYfZjQUEhUTKxMl94040609 = zMBPaxYfZjQUEhUTKxMl44058096;     zMBPaxYfZjQUEhUTKxMl44058096 = zMBPaxYfZjQUEhUTKxMl75027622;     zMBPaxYfZjQUEhUTKxMl75027622 = zMBPaxYfZjQUEhUTKxMl22352969;     zMBPaxYfZjQUEhUTKxMl22352969 = zMBPaxYfZjQUEhUTKxMl21011563;     zMBPaxYfZjQUEhUTKxMl21011563 = zMBPaxYfZjQUEhUTKxMl65099719;     zMBPaxYfZjQUEhUTKxMl65099719 = zMBPaxYfZjQUEhUTKxMl58911238;     zMBPaxYfZjQUEhUTKxMl58911238 = zMBPaxYfZjQUEhUTKxMl57591933;     zMBPaxYfZjQUEhUTKxMl57591933 = zMBPaxYfZjQUEhUTKxMl83476458;     zMBPaxYfZjQUEhUTKxMl83476458 = zMBPaxYfZjQUEhUTKxMl53132219;     zMBPaxYfZjQUEhUTKxMl53132219 = zMBPaxYfZjQUEhUTKxMl61112255;     zMBPaxYfZjQUEhUTKxMl61112255 = zMBPaxYfZjQUEhUTKxMl27693234;     zMBPaxYfZjQUEhUTKxMl27693234 = zMBPaxYfZjQUEhUTKxMl52205691;     zMBPaxYfZjQUEhUTKxMl52205691 = zMBPaxYfZjQUEhUTKxMl56684484;     zMBPaxYfZjQUEhUTKxMl56684484 = zMBPaxYfZjQUEhUTKxMl28193314;     zMBPaxYfZjQUEhUTKxMl28193314 = zMBPaxYfZjQUEhUTKxMl77603718;     zMBPaxYfZjQUEhUTKxMl77603718 = zMBPaxYfZjQUEhUTKxMl10813008;     zMBPaxYfZjQUEhUTKxMl10813008 = zMBPaxYfZjQUEhUTKxMl99676717;     zMBPaxYfZjQUEhUTKxMl99676717 = zMBPaxYfZjQUEhUTKxMl48170602;     zMBPaxYfZjQUEhUTKxMl48170602 = zMBPaxYfZjQUEhUTKxMl64397040;     zMBPaxYfZjQUEhUTKxMl64397040 = zMBPaxYfZjQUEhUTKxMl45236165;     zMBPaxYfZjQUEhUTKxMl45236165 = zMBPaxYfZjQUEhUTKxMl34941414;     zMBPaxYfZjQUEhUTKxMl34941414 = zMBPaxYfZjQUEhUTKxMl19254194;     zMBPaxYfZjQUEhUTKxMl19254194 = zMBPaxYfZjQUEhUTKxMl80225334;     zMBPaxYfZjQUEhUTKxMl80225334 = zMBPaxYfZjQUEhUTKxMl35583005;     zMBPaxYfZjQUEhUTKxMl35583005 = zMBPaxYfZjQUEhUTKxMl18907269;     zMBPaxYfZjQUEhUTKxMl18907269 = zMBPaxYfZjQUEhUTKxMl96314885;     zMBPaxYfZjQUEhUTKxMl96314885 = zMBPaxYfZjQUEhUTKxMl44421937;     zMBPaxYfZjQUEhUTKxMl44421937 = zMBPaxYfZjQUEhUTKxMl20854433;     zMBPaxYfZjQUEhUTKxMl20854433 = zMBPaxYfZjQUEhUTKxMl22350802;     zMBPaxYfZjQUEhUTKxMl22350802 = zMBPaxYfZjQUEhUTKxMl64331362;     zMBPaxYfZjQUEhUTKxMl64331362 = zMBPaxYfZjQUEhUTKxMl96556643;     zMBPaxYfZjQUEhUTKxMl96556643 = zMBPaxYfZjQUEhUTKxMl83112271;     zMBPaxYfZjQUEhUTKxMl83112271 = zMBPaxYfZjQUEhUTKxMl59688033;     zMBPaxYfZjQUEhUTKxMl59688033 = zMBPaxYfZjQUEhUTKxMl48017566;     zMBPaxYfZjQUEhUTKxMl48017566 = zMBPaxYfZjQUEhUTKxMl9887162;     zMBPaxYfZjQUEhUTKxMl9887162 = zMBPaxYfZjQUEhUTKxMl3550083;     zMBPaxYfZjQUEhUTKxMl3550083 = zMBPaxYfZjQUEhUTKxMl49025424;     zMBPaxYfZjQUEhUTKxMl49025424 = zMBPaxYfZjQUEhUTKxMl62560238;     zMBPaxYfZjQUEhUTKxMl62560238 = zMBPaxYfZjQUEhUTKxMl38265161;     zMBPaxYfZjQUEhUTKxMl38265161 = zMBPaxYfZjQUEhUTKxMl98098349;     zMBPaxYfZjQUEhUTKxMl98098349 = zMBPaxYfZjQUEhUTKxMl96944169;     zMBPaxYfZjQUEhUTKxMl96944169 = zMBPaxYfZjQUEhUTKxMl66760227;     zMBPaxYfZjQUEhUTKxMl66760227 = zMBPaxYfZjQUEhUTKxMl65455571;     zMBPaxYfZjQUEhUTKxMl65455571 = zMBPaxYfZjQUEhUTKxMl15591302;     zMBPaxYfZjQUEhUTKxMl15591302 = zMBPaxYfZjQUEhUTKxMl35243472;     zMBPaxYfZjQUEhUTKxMl35243472 = zMBPaxYfZjQUEhUTKxMl14550107;     zMBPaxYfZjQUEhUTKxMl14550107 = zMBPaxYfZjQUEhUTKxMl36101283;     zMBPaxYfZjQUEhUTKxMl36101283 = zMBPaxYfZjQUEhUTKxMl88148847;     zMBPaxYfZjQUEhUTKxMl88148847 = zMBPaxYfZjQUEhUTKxMl83767613;     zMBPaxYfZjQUEhUTKxMl83767613 = zMBPaxYfZjQUEhUTKxMl42919622;     zMBPaxYfZjQUEhUTKxMl42919622 = zMBPaxYfZjQUEhUTKxMl29362376;     zMBPaxYfZjQUEhUTKxMl29362376 = zMBPaxYfZjQUEhUTKxMl47464086;     zMBPaxYfZjQUEhUTKxMl47464086 = zMBPaxYfZjQUEhUTKxMl86610979;     zMBPaxYfZjQUEhUTKxMl86610979 = zMBPaxYfZjQUEhUTKxMl30881161;     zMBPaxYfZjQUEhUTKxMl30881161 = zMBPaxYfZjQUEhUTKxMl59099196;     zMBPaxYfZjQUEhUTKxMl59099196 = zMBPaxYfZjQUEhUTKxMl24803902;     zMBPaxYfZjQUEhUTKxMl24803902 = zMBPaxYfZjQUEhUTKxMl94802287;     zMBPaxYfZjQUEhUTKxMl94802287 = zMBPaxYfZjQUEhUTKxMl86769964;     zMBPaxYfZjQUEhUTKxMl86769964 = zMBPaxYfZjQUEhUTKxMl2104295;     zMBPaxYfZjQUEhUTKxMl2104295 = zMBPaxYfZjQUEhUTKxMl68784833;     zMBPaxYfZjQUEhUTKxMl68784833 = zMBPaxYfZjQUEhUTKxMl14489301;     zMBPaxYfZjQUEhUTKxMl14489301 = zMBPaxYfZjQUEhUTKxMl36737500;     zMBPaxYfZjQUEhUTKxMl36737500 = zMBPaxYfZjQUEhUTKxMl61125657;     zMBPaxYfZjQUEhUTKxMl61125657 = zMBPaxYfZjQUEhUTKxMl88800856;     zMBPaxYfZjQUEhUTKxMl88800856 = zMBPaxYfZjQUEhUTKxMl64555612;     zMBPaxYfZjQUEhUTKxMl64555612 = zMBPaxYfZjQUEhUTKxMl44580962;     zMBPaxYfZjQUEhUTKxMl44580962 = zMBPaxYfZjQUEhUTKxMl92517657;     zMBPaxYfZjQUEhUTKxMl92517657 = zMBPaxYfZjQUEhUTKxMl8666919;     zMBPaxYfZjQUEhUTKxMl8666919 = zMBPaxYfZjQUEhUTKxMl18306152;     zMBPaxYfZjQUEhUTKxMl18306152 = zMBPaxYfZjQUEhUTKxMl74053635;     zMBPaxYfZjQUEhUTKxMl74053635 = zMBPaxYfZjQUEhUTKxMl61787584;     zMBPaxYfZjQUEhUTKxMl61787584 = zMBPaxYfZjQUEhUTKxMl37116480;     zMBPaxYfZjQUEhUTKxMl37116480 = zMBPaxYfZjQUEhUTKxMl9905442;     zMBPaxYfZjQUEhUTKxMl9905442 = zMBPaxYfZjQUEhUTKxMl66298690;     zMBPaxYfZjQUEhUTKxMl66298690 = zMBPaxYfZjQUEhUTKxMl48291996;     zMBPaxYfZjQUEhUTKxMl48291996 = zMBPaxYfZjQUEhUTKxMl68181186;     zMBPaxYfZjQUEhUTKxMl68181186 = zMBPaxYfZjQUEhUTKxMl53798623;     zMBPaxYfZjQUEhUTKxMl53798623 = zMBPaxYfZjQUEhUTKxMl64634033;     zMBPaxYfZjQUEhUTKxMl64634033 = zMBPaxYfZjQUEhUTKxMl339533;     zMBPaxYfZjQUEhUTKxMl339533 = zMBPaxYfZjQUEhUTKxMl4357163;     zMBPaxYfZjQUEhUTKxMl4357163 = zMBPaxYfZjQUEhUTKxMl60213602;     zMBPaxYfZjQUEhUTKxMl60213602 = zMBPaxYfZjQUEhUTKxMl56273089;     zMBPaxYfZjQUEhUTKxMl56273089 = zMBPaxYfZjQUEhUTKxMl83571801;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void AkShZkdLNXXTOSqbuqox25794606() {     float jrbpeKyJXEQvTcvyuwTx54223468 = -110194122;    float jrbpeKyJXEQvTcvyuwTx72038397 = 2099668;    float jrbpeKyJXEQvTcvyuwTx36978507 = 42364389;    float jrbpeKyJXEQvTcvyuwTx94467389 = -650120257;    float jrbpeKyJXEQvTcvyuwTx71687007 = -821892128;    float jrbpeKyJXEQvTcvyuwTx8587553 = -384705555;    float jrbpeKyJXEQvTcvyuwTx95082452 = -20673646;    float jrbpeKyJXEQvTcvyuwTx19779087 = -410901232;    float jrbpeKyJXEQvTcvyuwTx12620852 = -997121584;    float jrbpeKyJXEQvTcvyuwTx15752229 = -616285304;    float jrbpeKyJXEQvTcvyuwTx58591322 = -369886548;    float jrbpeKyJXEQvTcvyuwTx6917904 = -551480200;    float jrbpeKyJXEQvTcvyuwTx33560017 = 10162475;    float jrbpeKyJXEQvTcvyuwTx12886128 = -138754849;    float jrbpeKyJXEQvTcvyuwTx78354706 = 12230270;    float jrbpeKyJXEQvTcvyuwTx83536659 = -826781728;    float jrbpeKyJXEQvTcvyuwTx69970153 = -347792550;    float jrbpeKyJXEQvTcvyuwTx14334104 = 8121410;    float jrbpeKyJXEQvTcvyuwTx25765878 = -364831004;    float jrbpeKyJXEQvTcvyuwTx33814332 = -140588300;    float jrbpeKyJXEQvTcvyuwTx77488723 = -140409483;    float jrbpeKyJXEQvTcvyuwTx82407327 = -124431548;    float jrbpeKyJXEQvTcvyuwTx67043528 = -675390727;    float jrbpeKyJXEQvTcvyuwTx92760775 = -677696046;    float jrbpeKyJXEQvTcvyuwTx21716322 = -262759601;    float jrbpeKyJXEQvTcvyuwTx14074039 = -874849270;    float jrbpeKyJXEQvTcvyuwTx61393005 = -486696961;    float jrbpeKyJXEQvTcvyuwTx68894480 = -505025658;    float jrbpeKyJXEQvTcvyuwTx59835722 = -513300151;    float jrbpeKyJXEQvTcvyuwTx91377786 = -126544267;    float jrbpeKyJXEQvTcvyuwTx63756481 = -897698934;    float jrbpeKyJXEQvTcvyuwTx72704774 = -253078712;    float jrbpeKyJXEQvTcvyuwTx60549079 = -852678067;    float jrbpeKyJXEQvTcvyuwTx76177983 = 76848646;    float jrbpeKyJXEQvTcvyuwTx77700954 = -850789287;    float jrbpeKyJXEQvTcvyuwTx8163861 = -180587287;    float jrbpeKyJXEQvTcvyuwTx8188526 = -596442636;    float jrbpeKyJXEQvTcvyuwTx98028653 = -614019507;    float jrbpeKyJXEQvTcvyuwTx13474703 = -426852274;    float jrbpeKyJXEQvTcvyuwTx14357493 = -916789729;    float jrbpeKyJXEQvTcvyuwTx40298618 = -66784835;    float jrbpeKyJXEQvTcvyuwTx64065995 = -403937260;    float jrbpeKyJXEQvTcvyuwTx67874228 = -268389805;    float jrbpeKyJXEQvTcvyuwTx2823187 = 22878409;    float jrbpeKyJXEQvTcvyuwTx40375996 = -465380499;    float jrbpeKyJXEQvTcvyuwTx27260268 = -313660895;    float jrbpeKyJXEQvTcvyuwTx48482004 = -206483740;    float jrbpeKyJXEQvTcvyuwTx29945573 = -483191704;    float jrbpeKyJXEQvTcvyuwTx1998096 = -297751197;    float jrbpeKyJXEQvTcvyuwTx91539758 = -434425884;    float jrbpeKyJXEQvTcvyuwTx27877358 = -396990338;    float jrbpeKyJXEQvTcvyuwTx89889070 = -862772469;    float jrbpeKyJXEQvTcvyuwTx64962819 = -22226194;    float jrbpeKyJXEQvTcvyuwTx93777387 = -200309874;    float jrbpeKyJXEQvTcvyuwTx48305882 = -160206592;    float jrbpeKyJXEQvTcvyuwTx71816140 = -985762575;    float jrbpeKyJXEQvTcvyuwTx4994870 = -322509606;    float jrbpeKyJXEQvTcvyuwTx44217732 = -279939566;    float jrbpeKyJXEQvTcvyuwTx72751067 = -287360657;    float jrbpeKyJXEQvTcvyuwTx57612968 = -947042858;    float jrbpeKyJXEQvTcvyuwTx47194548 = -898008595;    float jrbpeKyJXEQvTcvyuwTx26187972 = -515647989;    float jrbpeKyJXEQvTcvyuwTx59943364 = -897601082;    float jrbpeKyJXEQvTcvyuwTx21243065 = -770577317;    float jrbpeKyJXEQvTcvyuwTx51995747 = -718586370;    float jrbpeKyJXEQvTcvyuwTx85886547 = -16807836;    float jrbpeKyJXEQvTcvyuwTx46368824 = -698802133;    float jrbpeKyJXEQvTcvyuwTx57382034 = 33313828;    float jrbpeKyJXEQvTcvyuwTx35185174 = -287965562;    float jrbpeKyJXEQvTcvyuwTx70190846 = -807182443;    float jrbpeKyJXEQvTcvyuwTx75348133 = -130339092;    float jrbpeKyJXEQvTcvyuwTx71941499 = -733773043;    float jrbpeKyJXEQvTcvyuwTx859402 = -565026316;    float jrbpeKyJXEQvTcvyuwTx11408386 = -448041275;    float jrbpeKyJXEQvTcvyuwTx93515713 = 26196535;    float jrbpeKyJXEQvTcvyuwTx13422728 = -736472223;    float jrbpeKyJXEQvTcvyuwTx14533099 = -856041743;    float jrbpeKyJXEQvTcvyuwTx64220342 = -598269136;    float jrbpeKyJXEQvTcvyuwTx52384779 = -112315548;    float jrbpeKyJXEQvTcvyuwTx94456053 = -949098706;    float jrbpeKyJXEQvTcvyuwTx65592035 = -568365530;    float jrbpeKyJXEQvTcvyuwTx31447432 = 96494742;    float jrbpeKyJXEQvTcvyuwTx66896385 = -107274461;    float jrbpeKyJXEQvTcvyuwTx68295963 = 21125733;    float jrbpeKyJXEQvTcvyuwTx63500429 = -729553930;    float jrbpeKyJXEQvTcvyuwTx73867411 = 65073534;    float jrbpeKyJXEQvTcvyuwTx7741956 = -130852519;    float jrbpeKyJXEQvTcvyuwTx66771692 = -552368194;    float jrbpeKyJXEQvTcvyuwTx27872101 = -762944762;    float jrbpeKyJXEQvTcvyuwTx5884814 = -865026713;    float jrbpeKyJXEQvTcvyuwTx3168992 = -858077682;    float jrbpeKyJXEQvTcvyuwTx63970794 = -216503070;    float jrbpeKyJXEQvTcvyuwTx25277586 = -226658851;    float jrbpeKyJXEQvTcvyuwTx55861734 = -479809416;    float jrbpeKyJXEQvTcvyuwTx67162944 = 81218865;    float jrbpeKyJXEQvTcvyuwTx14110647 = -551136846;    float jrbpeKyJXEQvTcvyuwTx4122632 = -506336179;    float jrbpeKyJXEQvTcvyuwTx46631164 = -497812489;    float jrbpeKyJXEQvTcvyuwTx50827439 = -258535222;    float jrbpeKyJXEQvTcvyuwTx54489448 = -110194122;     jrbpeKyJXEQvTcvyuwTx54223468 = jrbpeKyJXEQvTcvyuwTx72038397;     jrbpeKyJXEQvTcvyuwTx72038397 = jrbpeKyJXEQvTcvyuwTx36978507;     jrbpeKyJXEQvTcvyuwTx36978507 = jrbpeKyJXEQvTcvyuwTx94467389;     jrbpeKyJXEQvTcvyuwTx94467389 = jrbpeKyJXEQvTcvyuwTx71687007;     jrbpeKyJXEQvTcvyuwTx71687007 = jrbpeKyJXEQvTcvyuwTx8587553;     jrbpeKyJXEQvTcvyuwTx8587553 = jrbpeKyJXEQvTcvyuwTx95082452;     jrbpeKyJXEQvTcvyuwTx95082452 = jrbpeKyJXEQvTcvyuwTx19779087;     jrbpeKyJXEQvTcvyuwTx19779087 = jrbpeKyJXEQvTcvyuwTx12620852;     jrbpeKyJXEQvTcvyuwTx12620852 = jrbpeKyJXEQvTcvyuwTx15752229;     jrbpeKyJXEQvTcvyuwTx15752229 = jrbpeKyJXEQvTcvyuwTx58591322;     jrbpeKyJXEQvTcvyuwTx58591322 = jrbpeKyJXEQvTcvyuwTx6917904;     jrbpeKyJXEQvTcvyuwTx6917904 = jrbpeKyJXEQvTcvyuwTx33560017;     jrbpeKyJXEQvTcvyuwTx33560017 = jrbpeKyJXEQvTcvyuwTx12886128;     jrbpeKyJXEQvTcvyuwTx12886128 = jrbpeKyJXEQvTcvyuwTx78354706;     jrbpeKyJXEQvTcvyuwTx78354706 = jrbpeKyJXEQvTcvyuwTx83536659;     jrbpeKyJXEQvTcvyuwTx83536659 = jrbpeKyJXEQvTcvyuwTx69970153;     jrbpeKyJXEQvTcvyuwTx69970153 = jrbpeKyJXEQvTcvyuwTx14334104;     jrbpeKyJXEQvTcvyuwTx14334104 = jrbpeKyJXEQvTcvyuwTx25765878;     jrbpeKyJXEQvTcvyuwTx25765878 = jrbpeKyJXEQvTcvyuwTx33814332;     jrbpeKyJXEQvTcvyuwTx33814332 = jrbpeKyJXEQvTcvyuwTx77488723;     jrbpeKyJXEQvTcvyuwTx77488723 = jrbpeKyJXEQvTcvyuwTx82407327;     jrbpeKyJXEQvTcvyuwTx82407327 = jrbpeKyJXEQvTcvyuwTx67043528;     jrbpeKyJXEQvTcvyuwTx67043528 = jrbpeKyJXEQvTcvyuwTx92760775;     jrbpeKyJXEQvTcvyuwTx92760775 = jrbpeKyJXEQvTcvyuwTx21716322;     jrbpeKyJXEQvTcvyuwTx21716322 = jrbpeKyJXEQvTcvyuwTx14074039;     jrbpeKyJXEQvTcvyuwTx14074039 = jrbpeKyJXEQvTcvyuwTx61393005;     jrbpeKyJXEQvTcvyuwTx61393005 = jrbpeKyJXEQvTcvyuwTx68894480;     jrbpeKyJXEQvTcvyuwTx68894480 = jrbpeKyJXEQvTcvyuwTx59835722;     jrbpeKyJXEQvTcvyuwTx59835722 = jrbpeKyJXEQvTcvyuwTx91377786;     jrbpeKyJXEQvTcvyuwTx91377786 = jrbpeKyJXEQvTcvyuwTx63756481;     jrbpeKyJXEQvTcvyuwTx63756481 = jrbpeKyJXEQvTcvyuwTx72704774;     jrbpeKyJXEQvTcvyuwTx72704774 = jrbpeKyJXEQvTcvyuwTx60549079;     jrbpeKyJXEQvTcvyuwTx60549079 = jrbpeKyJXEQvTcvyuwTx76177983;     jrbpeKyJXEQvTcvyuwTx76177983 = jrbpeKyJXEQvTcvyuwTx77700954;     jrbpeKyJXEQvTcvyuwTx77700954 = jrbpeKyJXEQvTcvyuwTx8163861;     jrbpeKyJXEQvTcvyuwTx8163861 = jrbpeKyJXEQvTcvyuwTx8188526;     jrbpeKyJXEQvTcvyuwTx8188526 = jrbpeKyJXEQvTcvyuwTx98028653;     jrbpeKyJXEQvTcvyuwTx98028653 = jrbpeKyJXEQvTcvyuwTx13474703;     jrbpeKyJXEQvTcvyuwTx13474703 = jrbpeKyJXEQvTcvyuwTx14357493;     jrbpeKyJXEQvTcvyuwTx14357493 = jrbpeKyJXEQvTcvyuwTx40298618;     jrbpeKyJXEQvTcvyuwTx40298618 = jrbpeKyJXEQvTcvyuwTx64065995;     jrbpeKyJXEQvTcvyuwTx64065995 = jrbpeKyJXEQvTcvyuwTx67874228;     jrbpeKyJXEQvTcvyuwTx67874228 = jrbpeKyJXEQvTcvyuwTx2823187;     jrbpeKyJXEQvTcvyuwTx2823187 = jrbpeKyJXEQvTcvyuwTx40375996;     jrbpeKyJXEQvTcvyuwTx40375996 = jrbpeKyJXEQvTcvyuwTx27260268;     jrbpeKyJXEQvTcvyuwTx27260268 = jrbpeKyJXEQvTcvyuwTx48482004;     jrbpeKyJXEQvTcvyuwTx48482004 = jrbpeKyJXEQvTcvyuwTx29945573;     jrbpeKyJXEQvTcvyuwTx29945573 = jrbpeKyJXEQvTcvyuwTx1998096;     jrbpeKyJXEQvTcvyuwTx1998096 = jrbpeKyJXEQvTcvyuwTx91539758;     jrbpeKyJXEQvTcvyuwTx91539758 = jrbpeKyJXEQvTcvyuwTx27877358;     jrbpeKyJXEQvTcvyuwTx27877358 = jrbpeKyJXEQvTcvyuwTx89889070;     jrbpeKyJXEQvTcvyuwTx89889070 = jrbpeKyJXEQvTcvyuwTx64962819;     jrbpeKyJXEQvTcvyuwTx64962819 = jrbpeKyJXEQvTcvyuwTx93777387;     jrbpeKyJXEQvTcvyuwTx93777387 = jrbpeKyJXEQvTcvyuwTx48305882;     jrbpeKyJXEQvTcvyuwTx48305882 = jrbpeKyJXEQvTcvyuwTx71816140;     jrbpeKyJXEQvTcvyuwTx71816140 = jrbpeKyJXEQvTcvyuwTx4994870;     jrbpeKyJXEQvTcvyuwTx4994870 = jrbpeKyJXEQvTcvyuwTx44217732;     jrbpeKyJXEQvTcvyuwTx44217732 = jrbpeKyJXEQvTcvyuwTx72751067;     jrbpeKyJXEQvTcvyuwTx72751067 = jrbpeKyJXEQvTcvyuwTx57612968;     jrbpeKyJXEQvTcvyuwTx57612968 = jrbpeKyJXEQvTcvyuwTx47194548;     jrbpeKyJXEQvTcvyuwTx47194548 = jrbpeKyJXEQvTcvyuwTx26187972;     jrbpeKyJXEQvTcvyuwTx26187972 = jrbpeKyJXEQvTcvyuwTx59943364;     jrbpeKyJXEQvTcvyuwTx59943364 = jrbpeKyJXEQvTcvyuwTx21243065;     jrbpeKyJXEQvTcvyuwTx21243065 = jrbpeKyJXEQvTcvyuwTx51995747;     jrbpeKyJXEQvTcvyuwTx51995747 = jrbpeKyJXEQvTcvyuwTx85886547;     jrbpeKyJXEQvTcvyuwTx85886547 = jrbpeKyJXEQvTcvyuwTx46368824;     jrbpeKyJXEQvTcvyuwTx46368824 = jrbpeKyJXEQvTcvyuwTx57382034;     jrbpeKyJXEQvTcvyuwTx57382034 = jrbpeKyJXEQvTcvyuwTx35185174;     jrbpeKyJXEQvTcvyuwTx35185174 = jrbpeKyJXEQvTcvyuwTx70190846;     jrbpeKyJXEQvTcvyuwTx70190846 = jrbpeKyJXEQvTcvyuwTx75348133;     jrbpeKyJXEQvTcvyuwTx75348133 = jrbpeKyJXEQvTcvyuwTx71941499;     jrbpeKyJXEQvTcvyuwTx71941499 = jrbpeKyJXEQvTcvyuwTx859402;     jrbpeKyJXEQvTcvyuwTx859402 = jrbpeKyJXEQvTcvyuwTx11408386;     jrbpeKyJXEQvTcvyuwTx11408386 = jrbpeKyJXEQvTcvyuwTx93515713;     jrbpeKyJXEQvTcvyuwTx93515713 = jrbpeKyJXEQvTcvyuwTx13422728;     jrbpeKyJXEQvTcvyuwTx13422728 = jrbpeKyJXEQvTcvyuwTx14533099;     jrbpeKyJXEQvTcvyuwTx14533099 = jrbpeKyJXEQvTcvyuwTx64220342;     jrbpeKyJXEQvTcvyuwTx64220342 = jrbpeKyJXEQvTcvyuwTx52384779;     jrbpeKyJXEQvTcvyuwTx52384779 = jrbpeKyJXEQvTcvyuwTx94456053;     jrbpeKyJXEQvTcvyuwTx94456053 = jrbpeKyJXEQvTcvyuwTx65592035;     jrbpeKyJXEQvTcvyuwTx65592035 = jrbpeKyJXEQvTcvyuwTx31447432;     jrbpeKyJXEQvTcvyuwTx31447432 = jrbpeKyJXEQvTcvyuwTx66896385;     jrbpeKyJXEQvTcvyuwTx66896385 = jrbpeKyJXEQvTcvyuwTx68295963;     jrbpeKyJXEQvTcvyuwTx68295963 = jrbpeKyJXEQvTcvyuwTx63500429;     jrbpeKyJXEQvTcvyuwTx63500429 = jrbpeKyJXEQvTcvyuwTx73867411;     jrbpeKyJXEQvTcvyuwTx73867411 = jrbpeKyJXEQvTcvyuwTx7741956;     jrbpeKyJXEQvTcvyuwTx7741956 = jrbpeKyJXEQvTcvyuwTx66771692;     jrbpeKyJXEQvTcvyuwTx66771692 = jrbpeKyJXEQvTcvyuwTx27872101;     jrbpeKyJXEQvTcvyuwTx27872101 = jrbpeKyJXEQvTcvyuwTx5884814;     jrbpeKyJXEQvTcvyuwTx5884814 = jrbpeKyJXEQvTcvyuwTx3168992;     jrbpeKyJXEQvTcvyuwTx3168992 = jrbpeKyJXEQvTcvyuwTx63970794;     jrbpeKyJXEQvTcvyuwTx63970794 = jrbpeKyJXEQvTcvyuwTx25277586;     jrbpeKyJXEQvTcvyuwTx25277586 = jrbpeKyJXEQvTcvyuwTx55861734;     jrbpeKyJXEQvTcvyuwTx55861734 = jrbpeKyJXEQvTcvyuwTx67162944;     jrbpeKyJXEQvTcvyuwTx67162944 = jrbpeKyJXEQvTcvyuwTx14110647;     jrbpeKyJXEQvTcvyuwTx14110647 = jrbpeKyJXEQvTcvyuwTx4122632;     jrbpeKyJXEQvTcvyuwTx4122632 = jrbpeKyJXEQvTcvyuwTx46631164;     jrbpeKyJXEQvTcvyuwTx46631164 = jrbpeKyJXEQvTcvyuwTx50827439;     jrbpeKyJXEQvTcvyuwTx50827439 = jrbpeKyJXEQvTcvyuwTx54489448;     jrbpeKyJXEQvTcvyuwTx54489448 = jrbpeKyJXEQvTcvyuwTx54223468;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void RRJdKvyrscoeffcDlYVl32453351() {     float hKbXvEFpZrzpDKPTsZTM54216997 = 17281737;    float hKbXvEFpZrzpDKPTsZTM10610714 = -40495457;    float hKbXvEFpZrzpDKPTsZTM38291719 = 85235775;    float hKbXvEFpZrzpDKPTsZTM79547249 = -470661087;    float hKbXvEFpZrzpDKPTsZTM80527109 = -190247449;    float hKbXvEFpZrzpDKPTsZTM43392985 = -84138367;    float hKbXvEFpZrzpDKPTsZTM63629788 = -825210113;    float hKbXvEFpZrzpDKPTsZTM88993136 = -115078549;    float hKbXvEFpZrzpDKPTsZTM95361015 = -710767654;    float hKbXvEFpZrzpDKPTsZTM12391742 = -253092175;    float hKbXvEFpZrzpDKPTsZTM78911927 = -329358620;    float hKbXvEFpZrzpDKPTsZTM21209581 = -286958562;    float hKbXvEFpZrzpDKPTsZTM65412931 = -11495999;    float hKbXvEFpZrzpDKPTsZTM66730348 = -483907900;    float hKbXvEFpZrzpDKPTsZTM23526816 = 49517418;    float hKbXvEFpZrzpDKPTsZTM58101206 = -59577378;    float hKbXvEFpZrzpDKPTsZTM25822913 = -167407120;    float hKbXvEFpZrzpDKPTsZTM37334637 = -388950212;    float hKbXvEFpZrzpDKPTsZTM46194148 = -982813271;    float hKbXvEFpZrzpDKPTsZTM5888095 = -752612751;    float hKbXvEFpZrzpDKPTsZTM57158148 = -950875857;    float hKbXvEFpZrzpDKPTsZTM60703594 = -634423431;    float hKbXvEFpZrzpDKPTsZTM3921925 = -34348569;    float hKbXvEFpZrzpDKPTsZTM36410884 = -558616568;    float hKbXvEFpZrzpDKPTsZTM50159011 = -689453454;    float hKbXvEFpZrzpDKPTsZTM30648116 = -783055198;    float hKbXvEFpZrzpDKPTsZTM31614587 = -787496674;    float hKbXvEFpZrzpDKPTsZTM4298006 = -13992646;    float hKbXvEFpZrzpDKPTsZTM72514168 = -59370465;    float hKbXvEFpZrzpDKPTsZTM67467656 = -901975641;    float hKbXvEFpZrzpDKPTsZTM97169159 = -184792169;    float hKbXvEFpZrzpDKPTsZTM57428632 = -644900474;    float hKbXvEFpZrzpDKPTsZTM46001503 = 45377717;    float hKbXvEFpZrzpDKPTsZTM76460820 = -899833086;    float hKbXvEFpZrzpDKPTsZTM60058992 = -10103212;    float hKbXvEFpZrzpDKPTsZTM50992050 = -260353033;    float hKbXvEFpZrzpDKPTsZTM20129894 = -696096014;    float hKbXvEFpZrzpDKPTsZTM67946333 = -609282186;    float hKbXvEFpZrzpDKPTsZTM48788299 = -39533690;    float hKbXvEFpZrzpDKPTsZTM86230385 = -526221415;    float hKbXvEFpZrzpDKPTsZTM28154286 = -154728734;    float hKbXvEFpZrzpDKPTsZTM49763657 = -597729343;    float hKbXvEFpZrzpDKPTsZTM28157589 = -827520059;    float hKbXvEFpZrzpDKPTsZTM62055084 = -634086035;    float hKbXvEFpZrzpDKPTsZTM24394048 = -938639900;    float hKbXvEFpZrzpDKPTsZTM67688236 = -125705566;    float hKbXvEFpZrzpDKPTsZTM76179797 = -687764362;    float hKbXvEFpZrzpDKPTsZTM5329154 = 8046940;    float hKbXvEFpZrzpDKPTsZTM20656485 = -961515058;    float hKbXvEFpZrzpDKPTsZTM11585182 = -21450771;    float hKbXvEFpZrzpDKPTsZTM41136727 = 24454654;    float hKbXvEFpZrzpDKPTsZTM4436328 = -144918217;    float hKbXvEFpZrzpDKPTsZTM40411167 = 85719797;    float hKbXvEFpZrzpDKPTsZTM4027659 = -732229725;    float hKbXvEFpZrzpDKPTsZTM75295202 = 82894131;    float hKbXvEFpZrzpDKPTsZTM93513403 = -348294833;    float hKbXvEFpZrzpDKPTsZTM6688790 = 93853111;    float hKbXvEFpZrzpDKPTsZTM1880836 = -356147657;    float hKbXvEFpZrzpDKPTsZTM29388239 = -781207634;    float hKbXvEFpZrzpDKPTsZTM49878994 = -407192251;    float hKbXvEFpZrzpDKPTsZTM11778399 = -296641693;    float hKbXvEFpZrzpDKPTsZTM59331782 = -711217468;    float hKbXvEFpZrzpDKPTsZTM16478969 = 44291916;    float hKbXvEFpZrzpDKPTsZTM27893359 = -808792013;    float hKbXvEFpZrzpDKPTsZTM15222582 = 31699993;    float hKbXvEFpZrzpDKPTsZTM21483296 = -684458146;    float hKbXvEFpZrzpDKPTsZTM75208078 = -232336279;    float hKbXvEFpZrzpDKPTsZTM88952111 = -111662914;    float hKbXvEFpZrzpDKPTsZTM6671356 = -373804689;    float hKbXvEFpZrzpDKPTsZTM72534765 = -690129549;    float hKbXvEFpZrzpDKPTsZTM37971313 = -363481365;    float hKbXvEFpZrzpDKPTsZTM57876579 = -558124934;    float hKbXvEFpZrzpDKPTsZTM88546337 = -249416522;    float hKbXvEFpZrzpDKPTsZTM59963762 = -356591856;    float hKbXvEFpZrzpDKPTsZTM77733808 = -497884018;    float hKbXvEFpZrzpDKPTsZTM7394492 = -253146514;    float hKbXvEFpZrzpDKPTsZTM32546005 = -806903372;    float hKbXvEFpZrzpDKPTsZTM41866840 = -400262535;    float hKbXvEFpZrzpDKPTsZTM12016836 = -619976669;    float hKbXvEFpZrzpDKPTsZTM82470775 = -463747889;    float hKbXvEFpZrzpDKPTsZTM54468319 = 4709164;    float hKbXvEFpZrzpDKPTsZTM26285433 = -695543614;    float hKbXvEFpZrzpDKPTsZTM83641520 = -52477588;    float hKbXvEFpZrzpDKPTsZTM60928987 = 62080305;    float hKbXvEFpZrzpDKPTsZTM26330929 = -826430296;    float hKbXvEFpZrzpDKPTsZTM92732832 = 60126048;    float hKbXvEFpZrzpDKPTsZTM17017465 = -630620272;    float hKbXvEFpZrzpDKPTsZTM41973844 = -222392559;    float hKbXvEFpZrzpDKPTsZTM1165618 = -882727217;    float hKbXvEFpZrzpDKPTsZTM66545589 = -661808379;    float hKbXvEFpZrzpDKPTsZTM44303261 = -254206145;    float hKbXvEFpZrzpDKPTsZTM18249058 = -239948357;    float hKbXvEFpZrzpDKPTsZTM38558095 = -828074553;    float hKbXvEFpZrzpDKPTsZTM98909305 = -632341439;    float hKbXvEFpZrzpDKPTsZTM74451986 = -129579723;    float hKbXvEFpZrzpDKPTsZTM68822503 = -443511266;    float hKbXvEFpZrzpDKPTsZTM33284689 = -542021259;    float hKbXvEFpZrzpDKPTsZTM264230 = 81271953;    float hKbXvEFpZrzpDKPTsZTM46832502 = -565786029;    float hKbXvEFpZrzpDKPTsZTM2910753 = 17281737;     hKbXvEFpZrzpDKPTsZTM54216997 = hKbXvEFpZrzpDKPTsZTM10610714;     hKbXvEFpZrzpDKPTsZTM10610714 = hKbXvEFpZrzpDKPTsZTM38291719;     hKbXvEFpZrzpDKPTsZTM38291719 = hKbXvEFpZrzpDKPTsZTM79547249;     hKbXvEFpZrzpDKPTsZTM79547249 = hKbXvEFpZrzpDKPTsZTM80527109;     hKbXvEFpZrzpDKPTsZTM80527109 = hKbXvEFpZrzpDKPTsZTM43392985;     hKbXvEFpZrzpDKPTsZTM43392985 = hKbXvEFpZrzpDKPTsZTM63629788;     hKbXvEFpZrzpDKPTsZTM63629788 = hKbXvEFpZrzpDKPTsZTM88993136;     hKbXvEFpZrzpDKPTsZTM88993136 = hKbXvEFpZrzpDKPTsZTM95361015;     hKbXvEFpZrzpDKPTsZTM95361015 = hKbXvEFpZrzpDKPTsZTM12391742;     hKbXvEFpZrzpDKPTsZTM12391742 = hKbXvEFpZrzpDKPTsZTM78911927;     hKbXvEFpZrzpDKPTsZTM78911927 = hKbXvEFpZrzpDKPTsZTM21209581;     hKbXvEFpZrzpDKPTsZTM21209581 = hKbXvEFpZrzpDKPTsZTM65412931;     hKbXvEFpZrzpDKPTsZTM65412931 = hKbXvEFpZrzpDKPTsZTM66730348;     hKbXvEFpZrzpDKPTsZTM66730348 = hKbXvEFpZrzpDKPTsZTM23526816;     hKbXvEFpZrzpDKPTsZTM23526816 = hKbXvEFpZrzpDKPTsZTM58101206;     hKbXvEFpZrzpDKPTsZTM58101206 = hKbXvEFpZrzpDKPTsZTM25822913;     hKbXvEFpZrzpDKPTsZTM25822913 = hKbXvEFpZrzpDKPTsZTM37334637;     hKbXvEFpZrzpDKPTsZTM37334637 = hKbXvEFpZrzpDKPTsZTM46194148;     hKbXvEFpZrzpDKPTsZTM46194148 = hKbXvEFpZrzpDKPTsZTM5888095;     hKbXvEFpZrzpDKPTsZTM5888095 = hKbXvEFpZrzpDKPTsZTM57158148;     hKbXvEFpZrzpDKPTsZTM57158148 = hKbXvEFpZrzpDKPTsZTM60703594;     hKbXvEFpZrzpDKPTsZTM60703594 = hKbXvEFpZrzpDKPTsZTM3921925;     hKbXvEFpZrzpDKPTsZTM3921925 = hKbXvEFpZrzpDKPTsZTM36410884;     hKbXvEFpZrzpDKPTsZTM36410884 = hKbXvEFpZrzpDKPTsZTM50159011;     hKbXvEFpZrzpDKPTsZTM50159011 = hKbXvEFpZrzpDKPTsZTM30648116;     hKbXvEFpZrzpDKPTsZTM30648116 = hKbXvEFpZrzpDKPTsZTM31614587;     hKbXvEFpZrzpDKPTsZTM31614587 = hKbXvEFpZrzpDKPTsZTM4298006;     hKbXvEFpZrzpDKPTsZTM4298006 = hKbXvEFpZrzpDKPTsZTM72514168;     hKbXvEFpZrzpDKPTsZTM72514168 = hKbXvEFpZrzpDKPTsZTM67467656;     hKbXvEFpZrzpDKPTsZTM67467656 = hKbXvEFpZrzpDKPTsZTM97169159;     hKbXvEFpZrzpDKPTsZTM97169159 = hKbXvEFpZrzpDKPTsZTM57428632;     hKbXvEFpZrzpDKPTsZTM57428632 = hKbXvEFpZrzpDKPTsZTM46001503;     hKbXvEFpZrzpDKPTsZTM46001503 = hKbXvEFpZrzpDKPTsZTM76460820;     hKbXvEFpZrzpDKPTsZTM76460820 = hKbXvEFpZrzpDKPTsZTM60058992;     hKbXvEFpZrzpDKPTsZTM60058992 = hKbXvEFpZrzpDKPTsZTM50992050;     hKbXvEFpZrzpDKPTsZTM50992050 = hKbXvEFpZrzpDKPTsZTM20129894;     hKbXvEFpZrzpDKPTsZTM20129894 = hKbXvEFpZrzpDKPTsZTM67946333;     hKbXvEFpZrzpDKPTsZTM67946333 = hKbXvEFpZrzpDKPTsZTM48788299;     hKbXvEFpZrzpDKPTsZTM48788299 = hKbXvEFpZrzpDKPTsZTM86230385;     hKbXvEFpZrzpDKPTsZTM86230385 = hKbXvEFpZrzpDKPTsZTM28154286;     hKbXvEFpZrzpDKPTsZTM28154286 = hKbXvEFpZrzpDKPTsZTM49763657;     hKbXvEFpZrzpDKPTsZTM49763657 = hKbXvEFpZrzpDKPTsZTM28157589;     hKbXvEFpZrzpDKPTsZTM28157589 = hKbXvEFpZrzpDKPTsZTM62055084;     hKbXvEFpZrzpDKPTsZTM62055084 = hKbXvEFpZrzpDKPTsZTM24394048;     hKbXvEFpZrzpDKPTsZTM24394048 = hKbXvEFpZrzpDKPTsZTM67688236;     hKbXvEFpZrzpDKPTsZTM67688236 = hKbXvEFpZrzpDKPTsZTM76179797;     hKbXvEFpZrzpDKPTsZTM76179797 = hKbXvEFpZrzpDKPTsZTM5329154;     hKbXvEFpZrzpDKPTsZTM5329154 = hKbXvEFpZrzpDKPTsZTM20656485;     hKbXvEFpZrzpDKPTsZTM20656485 = hKbXvEFpZrzpDKPTsZTM11585182;     hKbXvEFpZrzpDKPTsZTM11585182 = hKbXvEFpZrzpDKPTsZTM41136727;     hKbXvEFpZrzpDKPTsZTM41136727 = hKbXvEFpZrzpDKPTsZTM4436328;     hKbXvEFpZrzpDKPTsZTM4436328 = hKbXvEFpZrzpDKPTsZTM40411167;     hKbXvEFpZrzpDKPTsZTM40411167 = hKbXvEFpZrzpDKPTsZTM4027659;     hKbXvEFpZrzpDKPTsZTM4027659 = hKbXvEFpZrzpDKPTsZTM75295202;     hKbXvEFpZrzpDKPTsZTM75295202 = hKbXvEFpZrzpDKPTsZTM93513403;     hKbXvEFpZrzpDKPTsZTM93513403 = hKbXvEFpZrzpDKPTsZTM6688790;     hKbXvEFpZrzpDKPTsZTM6688790 = hKbXvEFpZrzpDKPTsZTM1880836;     hKbXvEFpZrzpDKPTsZTM1880836 = hKbXvEFpZrzpDKPTsZTM29388239;     hKbXvEFpZrzpDKPTsZTM29388239 = hKbXvEFpZrzpDKPTsZTM49878994;     hKbXvEFpZrzpDKPTsZTM49878994 = hKbXvEFpZrzpDKPTsZTM11778399;     hKbXvEFpZrzpDKPTsZTM11778399 = hKbXvEFpZrzpDKPTsZTM59331782;     hKbXvEFpZrzpDKPTsZTM59331782 = hKbXvEFpZrzpDKPTsZTM16478969;     hKbXvEFpZrzpDKPTsZTM16478969 = hKbXvEFpZrzpDKPTsZTM27893359;     hKbXvEFpZrzpDKPTsZTM27893359 = hKbXvEFpZrzpDKPTsZTM15222582;     hKbXvEFpZrzpDKPTsZTM15222582 = hKbXvEFpZrzpDKPTsZTM21483296;     hKbXvEFpZrzpDKPTsZTM21483296 = hKbXvEFpZrzpDKPTsZTM75208078;     hKbXvEFpZrzpDKPTsZTM75208078 = hKbXvEFpZrzpDKPTsZTM88952111;     hKbXvEFpZrzpDKPTsZTM88952111 = hKbXvEFpZrzpDKPTsZTM6671356;     hKbXvEFpZrzpDKPTsZTM6671356 = hKbXvEFpZrzpDKPTsZTM72534765;     hKbXvEFpZrzpDKPTsZTM72534765 = hKbXvEFpZrzpDKPTsZTM37971313;     hKbXvEFpZrzpDKPTsZTM37971313 = hKbXvEFpZrzpDKPTsZTM57876579;     hKbXvEFpZrzpDKPTsZTM57876579 = hKbXvEFpZrzpDKPTsZTM88546337;     hKbXvEFpZrzpDKPTsZTM88546337 = hKbXvEFpZrzpDKPTsZTM59963762;     hKbXvEFpZrzpDKPTsZTM59963762 = hKbXvEFpZrzpDKPTsZTM77733808;     hKbXvEFpZrzpDKPTsZTM77733808 = hKbXvEFpZrzpDKPTsZTM7394492;     hKbXvEFpZrzpDKPTsZTM7394492 = hKbXvEFpZrzpDKPTsZTM32546005;     hKbXvEFpZrzpDKPTsZTM32546005 = hKbXvEFpZrzpDKPTsZTM41866840;     hKbXvEFpZrzpDKPTsZTM41866840 = hKbXvEFpZrzpDKPTsZTM12016836;     hKbXvEFpZrzpDKPTsZTM12016836 = hKbXvEFpZrzpDKPTsZTM82470775;     hKbXvEFpZrzpDKPTsZTM82470775 = hKbXvEFpZrzpDKPTsZTM54468319;     hKbXvEFpZrzpDKPTsZTM54468319 = hKbXvEFpZrzpDKPTsZTM26285433;     hKbXvEFpZrzpDKPTsZTM26285433 = hKbXvEFpZrzpDKPTsZTM83641520;     hKbXvEFpZrzpDKPTsZTM83641520 = hKbXvEFpZrzpDKPTsZTM60928987;     hKbXvEFpZrzpDKPTsZTM60928987 = hKbXvEFpZrzpDKPTsZTM26330929;     hKbXvEFpZrzpDKPTsZTM26330929 = hKbXvEFpZrzpDKPTsZTM92732832;     hKbXvEFpZrzpDKPTsZTM92732832 = hKbXvEFpZrzpDKPTsZTM17017465;     hKbXvEFpZrzpDKPTsZTM17017465 = hKbXvEFpZrzpDKPTsZTM41973844;     hKbXvEFpZrzpDKPTsZTM41973844 = hKbXvEFpZrzpDKPTsZTM1165618;     hKbXvEFpZrzpDKPTsZTM1165618 = hKbXvEFpZrzpDKPTsZTM66545589;     hKbXvEFpZrzpDKPTsZTM66545589 = hKbXvEFpZrzpDKPTsZTM44303261;     hKbXvEFpZrzpDKPTsZTM44303261 = hKbXvEFpZrzpDKPTsZTM18249058;     hKbXvEFpZrzpDKPTsZTM18249058 = hKbXvEFpZrzpDKPTsZTM38558095;     hKbXvEFpZrzpDKPTsZTM38558095 = hKbXvEFpZrzpDKPTsZTM98909305;     hKbXvEFpZrzpDKPTsZTM98909305 = hKbXvEFpZrzpDKPTsZTM74451986;     hKbXvEFpZrzpDKPTsZTM74451986 = hKbXvEFpZrzpDKPTsZTM68822503;     hKbXvEFpZrzpDKPTsZTM68822503 = hKbXvEFpZrzpDKPTsZTM33284689;     hKbXvEFpZrzpDKPTsZTM33284689 = hKbXvEFpZrzpDKPTsZTM264230;     hKbXvEFpZrzpDKPTsZTM264230 = hKbXvEFpZrzpDKPTsZTM46832502;     hKbXvEFpZrzpDKPTsZTM46832502 = hKbXvEFpZrzpDKPTsZTM2910753;     hKbXvEFpZrzpDKPTsZTM2910753 = hKbXvEFpZrzpDKPTsZTM54216997;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BTCmJloFlbWkpMWBfASI88946996() {     float LEOkfrxxHMfAYDGsRvbw31540960 = -137744126;    float LEOkfrxxHMfAYDGsRvbw53395681 = -351575427;    float LEOkfrxxHMfAYDGsRvbw69314648 = -86020838;    float LEOkfrxxHMfAYDGsRvbw7804651 = -933982708;    float LEOkfrxxHMfAYDGsRvbw22172996 = -810642729;    float LEOkfrxxHMfAYDGsRvbw67882900 = -548430191;    float LEOkfrxxHMfAYDGsRvbw58453808 = -828424683;    float LEOkfrxxHMfAYDGsRvbw88187713 = -767451324;    float LEOkfrxxHMfAYDGsRvbw30618303 = -591017374;    float LEOkfrxxHMfAYDGsRvbw2027492 = -760896245;    float LEOkfrxxHMfAYDGsRvbw78377877 = -621772942;    float LEOkfrxxHMfAYDGsRvbw59737776 = -797909788;    float LEOkfrxxHMfAYDGsRvbw13248046 = 75904611;    float LEOkfrxxHMfAYDGsRvbw40135674 = 87463785;    float LEOkfrxxHMfAYDGsRvbw11224907 = -435399823;    float LEOkfrxxHMfAYDGsRvbw90267248 = -591054986;    float LEOkfrxxHMfAYDGsRvbw53665174 = -314014319;    float LEOkfrxxHMfAYDGsRvbw82633197 = -619268257;    float LEOkfrxxHMfAYDGsRvbw7018583 = -884710178;    float LEOkfrxxHMfAYDGsRvbw50742254 = -131308050;    float LEOkfrxxHMfAYDGsRvbw64080054 = -540150619;    float LEOkfrxxHMfAYDGsRvbw75478021 = -422641426;    float LEOkfrxxHMfAYDGsRvbw90596038 = -7199201;    float LEOkfrxxHMfAYDGsRvbw8950122 = -348597238;    float LEOkfrxxHMfAYDGsRvbw12256664 = -539257106;    float LEOkfrxxHMfAYDGsRvbw59555628 = -739012555;    float LEOkfrxxHMfAYDGsRvbw67720339 = -665318670;    float LEOkfrxxHMfAYDGsRvbw48624268 = -322502070;    float LEOkfrxxHMfAYDGsRvbw48802231 = -572528061;    float LEOkfrxxHMfAYDGsRvbw2730296 = -335070312;    float LEOkfrxxHMfAYDGsRvbw39728264 = -102775049;    float LEOkfrxxHMfAYDGsRvbw15286366 = 53440381;    float LEOkfrxxHMfAYDGsRvbw22555653 = -333853198;    float LEOkfrxxHMfAYDGsRvbw23930185 = -128234357;    float LEOkfrxxHMfAYDGsRvbw18490090 = -798691804;    float LEOkfrxxHMfAYDGsRvbw16069416 = -424233705;    float LEOkfrxxHMfAYDGsRvbw63452821 = -121136013;    float LEOkfrxxHMfAYDGsRvbw9393793 = -543878036;    float LEOkfrxxHMfAYDGsRvbw41829922 = -36848699;    float LEOkfrxxHMfAYDGsRvbw34160029 = -160091558;    float LEOkfrxxHMfAYDGsRvbw50061820 = -254975762;    float LEOkfrxxHMfAYDGsRvbw60949816 = -998391427;    float LEOkfrxxHMfAYDGsRvbw53799360 = -477337845;    float LEOkfrxxHMfAYDGsRvbw46168233 = -149408089;    float LEOkfrxxHMfAYDGsRvbw46395106 = -691528400;    float LEOkfrxxHMfAYDGsRvbw15569021 = -616122708;    float LEOkfrxxHMfAYDGsRvbw48265549 = -941933365;    float LEOkfrxxHMfAYDGsRvbw4734599 = -835159924;    float LEOkfrxxHMfAYDGsRvbw1157063 = -693262083;    float LEOkfrxxHMfAYDGsRvbw44028724 = 65027139;    float LEOkfrxxHMfAYDGsRvbw85323182 = -143430600;    float LEOkfrxxHMfAYDGsRvbw56401031 = -43524083;    float LEOkfrxxHMfAYDGsRvbw93999342 = -830264935;    float LEOkfrxxHMfAYDGsRvbw37860673 = -946132893;    float LEOkfrxxHMfAYDGsRvbw24866943 = -543509849;    float LEOkfrxxHMfAYDGsRvbw56062939 = -715102700;    float LEOkfrxxHMfAYDGsRvbw62799643 = -244376227;    float LEOkfrxxHMfAYDGsRvbw60364526 = -737423601;    float LEOkfrxxHMfAYDGsRvbw95547987 = -294725602;    float LEOkfrxxHMfAYDGsRvbw62617368 = 28369825;    float LEOkfrxxHMfAYDGsRvbw162561 = -883111521;    float LEOkfrxxHMfAYDGsRvbw9829540 = -405922613;    float LEOkfrxxHMfAYDGsRvbw39385483 = -94923264;    float LEOkfrxxHMfAYDGsRvbw27888007 = -155947062;    float LEOkfrxxHMfAYDGsRvbw62299228 = -558121197;    float LEOkfrxxHMfAYDGsRvbw63091512 = -575213324;    float LEOkfrxxHMfAYDGsRvbw37182123 = -364056590;    float LEOkfrxxHMfAYDGsRvbw89317860 = -795861032;    float LEOkfrxxHMfAYDGsRvbw21645584 = -113844411;    float LEOkfrxxHMfAYDGsRvbw95155490 = 88833881;    float LEOkfrxxHMfAYDGsRvbw26814427 = -369918973;    float LEOkfrxxHMfAYDGsRvbw44271381 = -770136283;    float LEOkfrxxHMfAYDGsRvbw40803275 = -482419558;    float LEOkfrxxHMfAYDGsRvbw72858554 = -624618620;    float LEOkfrxxHMfAYDGsRvbw680434 = -876332288;    float LEOkfrxxHMfAYDGsRvbw3130238 = -541759192;    float LEOkfrxxHMfAYDGsRvbw21678661 = -945303581;    float LEOkfrxxHMfAYDGsRvbw44427806 = -857791112;    float LEOkfrxxHMfAYDGsRvbw62555016 = -657068838;    float LEOkfrxxHMfAYDGsRvbw96687642 = -923134398;    float LEOkfrxxHMfAYDGsRvbw11290079 = -797079190;    float LEOkfrxxHMfAYDGsRvbw62985741 = -830158746;    float LEOkfrxxHMfAYDGsRvbw47467205 = -629239987;    float LEOkfrxxHMfAYDGsRvbw4773507 = -537555200;    float LEOkfrxxHMfAYDGsRvbw17407114 = -91639712;    float LEOkfrxxHMfAYDGsRvbw83327232 = 40749034;    float LEOkfrxxHMfAYDGsRvbw21287023 = -116294684;    float LEOkfrxxHMfAYDGsRvbw84694980 = -387720306;    float LEOkfrxxHMfAYDGsRvbw99063242 = -584724509;    float LEOkfrxxHMfAYDGsRvbw62427151 = 16410896;    float LEOkfrxxHMfAYDGsRvbw53269773 = -79857478;    float LEOkfrxxHMfAYDGsRvbw3088296 = -383712413;    float LEOkfrxxHMfAYDGsRvbw13845806 = -149152435;    float LEOkfrxxHMfAYDGsRvbw79212554 = 34781476;    float LEOkfrxxHMfAYDGsRvbw33997469 = -276980037;    float LEOkfrxxHMfAYDGsRvbw40232280 = -849053149;    float LEOkfrxxHMfAYDGsRvbw21564333 = -803468164;    float LEOkfrxxHMfAYDGsRvbw25911354 = -221390784;    float LEOkfrxxHMfAYDGsRvbw83869005 = -591286893;    float LEOkfrxxHMfAYDGsRvbw83303594 = -137744126;     LEOkfrxxHMfAYDGsRvbw31540960 = LEOkfrxxHMfAYDGsRvbw53395681;     LEOkfrxxHMfAYDGsRvbw53395681 = LEOkfrxxHMfAYDGsRvbw69314648;     LEOkfrxxHMfAYDGsRvbw69314648 = LEOkfrxxHMfAYDGsRvbw7804651;     LEOkfrxxHMfAYDGsRvbw7804651 = LEOkfrxxHMfAYDGsRvbw22172996;     LEOkfrxxHMfAYDGsRvbw22172996 = LEOkfrxxHMfAYDGsRvbw67882900;     LEOkfrxxHMfAYDGsRvbw67882900 = LEOkfrxxHMfAYDGsRvbw58453808;     LEOkfrxxHMfAYDGsRvbw58453808 = LEOkfrxxHMfAYDGsRvbw88187713;     LEOkfrxxHMfAYDGsRvbw88187713 = LEOkfrxxHMfAYDGsRvbw30618303;     LEOkfrxxHMfAYDGsRvbw30618303 = LEOkfrxxHMfAYDGsRvbw2027492;     LEOkfrxxHMfAYDGsRvbw2027492 = LEOkfrxxHMfAYDGsRvbw78377877;     LEOkfrxxHMfAYDGsRvbw78377877 = LEOkfrxxHMfAYDGsRvbw59737776;     LEOkfrxxHMfAYDGsRvbw59737776 = LEOkfrxxHMfAYDGsRvbw13248046;     LEOkfrxxHMfAYDGsRvbw13248046 = LEOkfrxxHMfAYDGsRvbw40135674;     LEOkfrxxHMfAYDGsRvbw40135674 = LEOkfrxxHMfAYDGsRvbw11224907;     LEOkfrxxHMfAYDGsRvbw11224907 = LEOkfrxxHMfAYDGsRvbw90267248;     LEOkfrxxHMfAYDGsRvbw90267248 = LEOkfrxxHMfAYDGsRvbw53665174;     LEOkfrxxHMfAYDGsRvbw53665174 = LEOkfrxxHMfAYDGsRvbw82633197;     LEOkfrxxHMfAYDGsRvbw82633197 = LEOkfrxxHMfAYDGsRvbw7018583;     LEOkfrxxHMfAYDGsRvbw7018583 = LEOkfrxxHMfAYDGsRvbw50742254;     LEOkfrxxHMfAYDGsRvbw50742254 = LEOkfrxxHMfAYDGsRvbw64080054;     LEOkfrxxHMfAYDGsRvbw64080054 = LEOkfrxxHMfAYDGsRvbw75478021;     LEOkfrxxHMfAYDGsRvbw75478021 = LEOkfrxxHMfAYDGsRvbw90596038;     LEOkfrxxHMfAYDGsRvbw90596038 = LEOkfrxxHMfAYDGsRvbw8950122;     LEOkfrxxHMfAYDGsRvbw8950122 = LEOkfrxxHMfAYDGsRvbw12256664;     LEOkfrxxHMfAYDGsRvbw12256664 = LEOkfrxxHMfAYDGsRvbw59555628;     LEOkfrxxHMfAYDGsRvbw59555628 = LEOkfrxxHMfAYDGsRvbw67720339;     LEOkfrxxHMfAYDGsRvbw67720339 = LEOkfrxxHMfAYDGsRvbw48624268;     LEOkfrxxHMfAYDGsRvbw48624268 = LEOkfrxxHMfAYDGsRvbw48802231;     LEOkfrxxHMfAYDGsRvbw48802231 = LEOkfrxxHMfAYDGsRvbw2730296;     LEOkfrxxHMfAYDGsRvbw2730296 = LEOkfrxxHMfAYDGsRvbw39728264;     LEOkfrxxHMfAYDGsRvbw39728264 = LEOkfrxxHMfAYDGsRvbw15286366;     LEOkfrxxHMfAYDGsRvbw15286366 = LEOkfrxxHMfAYDGsRvbw22555653;     LEOkfrxxHMfAYDGsRvbw22555653 = LEOkfrxxHMfAYDGsRvbw23930185;     LEOkfrxxHMfAYDGsRvbw23930185 = LEOkfrxxHMfAYDGsRvbw18490090;     LEOkfrxxHMfAYDGsRvbw18490090 = LEOkfrxxHMfAYDGsRvbw16069416;     LEOkfrxxHMfAYDGsRvbw16069416 = LEOkfrxxHMfAYDGsRvbw63452821;     LEOkfrxxHMfAYDGsRvbw63452821 = LEOkfrxxHMfAYDGsRvbw9393793;     LEOkfrxxHMfAYDGsRvbw9393793 = LEOkfrxxHMfAYDGsRvbw41829922;     LEOkfrxxHMfAYDGsRvbw41829922 = LEOkfrxxHMfAYDGsRvbw34160029;     LEOkfrxxHMfAYDGsRvbw34160029 = LEOkfrxxHMfAYDGsRvbw50061820;     LEOkfrxxHMfAYDGsRvbw50061820 = LEOkfrxxHMfAYDGsRvbw60949816;     LEOkfrxxHMfAYDGsRvbw60949816 = LEOkfrxxHMfAYDGsRvbw53799360;     LEOkfrxxHMfAYDGsRvbw53799360 = LEOkfrxxHMfAYDGsRvbw46168233;     LEOkfrxxHMfAYDGsRvbw46168233 = LEOkfrxxHMfAYDGsRvbw46395106;     LEOkfrxxHMfAYDGsRvbw46395106 = LEOkfrxxHMfAYDGsRvbw15569021;     LEOkfrxxHMfAYDGsRvbw15569021 = LEOkfrxxHMfAYDGsRvbw48265549;     LEOkfrxxHMfAYDGsRvbw48265549 = LEOkfrxxHMfAYDGsRvbw4734599;     LEOkfrxxHMfAYDGsRvbw4734599 = LEOkfrxxHMfAYDGsRvbw1157063;     LEOkfrxxHMfAYDGsRvbw1157063 = LEOkfrxxHMfAYDGsRvbw44028724;     LEOkfrxxHMfAYDGsRvbw44028724 = LEOkfrxxHMfAYDGsRvbw85323182;     LEOkfrxxHMfAYDGsRvbw85323182 = LEOkfrxxHMfAYDGsRvbw56401031;     LEOkfrxxHMfAYDGsRvbw56401031 = LEOkfrxxHMfAYDGsRvbw93999342;     LEOkfrxxHMfAYDGsRvbw93999342 = LEOkfrxxHMfAYDGsRvbw37860673;     LEOkfrxxHMfAYDGsRvbw37860673 = LEOkfrxxHMfAYDGsRvbw24866943;     LEOkfrxxHMfAYDGsRvbw24866943 = LEOkfrxxHMfAYDGsRvbw56062939;     LEOkfrxxHMfAYDGsRvbw56062939 = LEOkfrxxHMfAYDGsRvbw62799643;     LEOkfrxxHMfAYDGsRvbw62799643 = LEOkfrxxHMfAYDGsRvbw60364526;     LEOkfrxxHMfAYDGsRvbw60364526 = LEOkfrxxHMfAYDGsRvbw95547987;     LEOkfrxxHMfAYDGsRvbw95547987 = LEOkfrxxHMfAYDGsRvbw62617368;     LEOkfrxxHMfAYDGsRvbw62617368 = LEOkfrxxHMfAYDGsRvbw162561;     LEOkfrxxHMfAYDGsRvbw162561 = LEOkfrxxHMfAYDGsRvbw9829540;     LEOkfrxxHMfAYDGsRvbw9829540 = LEOkfrxxHMfAYDGsRvbw39385483;     LEOkfrxxHMfAYDGsRvbw39385483 = LEOkfrxxHMfAYDGsRvbw27888007;     LEOkfrxxHMfAYDGsRvbw27888007 = LEOkfrxxHMfAYDGsRvbw62299228;     LEOkfrxxHMfAYDGsRvbw62299228 = LEOkfrxxHMfAYDGsRvbw63091512;     LEOkfrxxHMfAYDGsRvbw63091512 = LEOkfrxxHMfAYDGsRvbw37182123;     LEOkfrxxHMfAYDGsRvbw37182123 = LEOkfrxxHMfAYDGsRvbw89317860;     LEOkfrxxHMfAYDGsRvbw89317860 = LEOkfrxxHMfAYDGsRvbw21645584;     LEOkfrxxHMfAYDGsRvbw21645584 = LEOkfrxxHMfAYDGsRvbw95155490;     LEOkfrxxHMfAYDGsRvbw95155490 = LEOkfrxxHMfAYDGsRvbw26814427;     LEOkfrxxHMfAYDGsRvbw26814427 = LEOkfrxxHMfAYDGsRvbw44271381;     LEOkfrxxHMfAYDGsRvbw44271381 = LEOkfrxxHMfAYDGsRvbw40803275;     LEOkfrxxHMfAYDGsRvbw40803275 = LEOkfrxxHMfAYDGsRvbw72858554;     LEOkfrxxHMfAYDGsRvbw72858554 = LEOkfrxxHMfAYDGsRvbw680434;     LEOkfrxxHMfAYDGsRvbw680434 = LEOkfrxxHMfAYDGsRvbw3130238;     LEOkfrxxHMfAYDGsRvbw3130238 = LEOkfrxxHMfAYDGsRvbw21678661;     LEOkfrxxHMfAYDGsRvbw21678661 = LEOkfrxxHMfAYDGsRvbw44427806;     LEOkfrxxHMfAYDGsRvbw44427806 = LEOkfrxxHMfAYDGsRvbw62555016;     LEOkfrxxHMfAYDGsRvbw62555016 = LEOkfrxxHMfAYDGsRvbw96687642;     LEOkfrxxHMfAYDGsRvbw96687642 = LEOkfrxxHMfAYDGsRvbw11290079;     LEOkfrxxHMfAYDGsRvbw11290079 = LEOkfrxxHMfAYDGsRvbw62985741;     LEOkfrxxHMfAYDGsRvbw62985741 = LEOkfrxxHMfAYDGsRvbw47467205;     LEOkfrxxHMfAYDGsRvbw47467205 = LEOkfrxxHMfAYDGsRvbw4773507;     LEOkfrxxHMfAYDGsRvbw4773507 = LEOkfrxxHMfAYDGsRvbw17407114;     LEOkfrxxHMfAYDGsRvbw17407114 = LEOkfrxxHMfAYDGsRvbw83327232;     LEOkfrxxHMfAYDGsRvbw83327232 = LEOkfrxxHMfAYDGsRvbw21287023;     LEOkfrxxHMfAYDGsRvbw21287023 = LEOkfrxxHMfAYDGsRvbw84694980;     LEOkfrxxHMfAYDGsRvbw84694980 = LEOkfrxxHMfAYDGsRvbw99063242;     LEOkfrxxHMfAYDGsRvbw99063242 = LEOkfrxxHMfAYDGsRvbw62427151;     LEOkfrxxHMfAYDGsRvbw62427151 = LEOkfrxxHMfAYDGsRvbw53269773;     LEOkfrxxHMfAYDGsRvbw53269773 = LEOkfrxxHMfAYDGsRvbw3088296;     LEOkfrxxHMfAYDGsRvbw3088296 = LEOkfrxxHMfAYDGsRvbw13845806;     LEOkfrxxHMfAYDGsRvbw13845806 = LEOkfrxxHMfAYDGsRvbw79212554;     LEOkfrxxHMfAYDGsRvbw79212554 = LEOkfrxxHMfAYDGsRvbw33997469;     LEOkfrxxHMfAYDGsRvbw33997469 = LEOkfrxxHMfAYDGsRvbw40232280;     LEOkfrxxHMfAYDGsRvbw40232280 = LEOkfrxxHMfAYDGsRvbw21564333;     LEOkfrxxHMfAYDGsRvbw21564333 = LEOkfrxxHMfAYDGsRvbw25911354;     LEOkfrxxHMfAYDGsRvbw25911354 = LEOkfrxxHMfAYDGsRvbw83869005;     LEOkfrxxHMfAYDGsRvbw83869005 = LEOkfrxxHMfAYDGsRvbw83303594;     LEOkfrxxHMfAYDGsRvbw83303594 = LEOkfrxxHMfAYDGsRvbw31540960;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dQhhPOjPNNiiHNZnyqDQ23844736() {     float MIjKgHYXoXkJsRpHmwkP92594912 = -770267914;    float MIjKgHYXoXkJsRpHmwkP55206428 = -423712122;    float MIjKgHYXoXkJsRpHmwkP87610482 = -168456529;    float MIjKgHYXoXkJsRpHmwkP48130144 = -481645427;    float MIjKgHYXoXkJsRpHmwkP34540117 = -260630880;    float MIjKgHYXoXkJsRpHmwkP68906900 = -316609872;    float MIjKgHYXoXkJsRpHmwkP21611055 = -48616924;    float MIjKgHYXoXkJsRpHmwkP61508323 = -136101368;    float MIjKgHYXoXkJsRpHmwkP33354955 = -355103721;    float MIjKgHYXoXkJsRpHmwkP41129316 = -754934460;    float MIjKgHYXoXkJsRpHmwkP412259 = -795000450;    float MIjKgHYXoXkJsRpHmwkP74186064 = -402521922;    float MIjKgHYXoXkJsRpHmwkP80284098 = -757868547;    float MIjKgHYXoXkJsRpHmwkP54069923 = -445694341;    float MIjKgHYXoXkJsRpHmwkP57930148 = -883301518;    float MIjKgHYXoXkJsRpHmwkP19682771 = -713864594;    float MIjKgHYXoXkJsRpHmwkP25520087 = -799886811;    float MIjKgHYXoXkJsRpHmwkP19008494 = -63366253;    float MIjKgHYXoXkJsRpHmwkP88391229 = 42725943;    float MIjKgHYXoXkJsRpHmwkP97630294 = -714271815;    float MIjKgHYXoXkJsRpHmwkP58672046 = 6574472;    float MIjKgHYXoXkJsRpHmwkP12958254 = -714022825;    float MIjKgHYXoXkJsRpHmwkP9567622 = -650486727;    float MIjKgHYXoXkJsRpHmwkP38981947 = -896890038;    float MIjKgHYXoXkJsRpHmwkP82513720 = -40697667;    float MIjKgHYXoXkJsRpHmwkP24881821 = -129731010;    float MIjKgHYXoXkJsRpHmwkP55196450 = -686216376;    float MIjKgHYXoXkJsRpHmwkP146206 = -908485995;    float MIjKgHYXoXkJsRpHmwkP84329987 = -963811241;    float MIjKgHYXoXkJsRpHmwkP97451392 = -765339984;    float MIjKgHYXoXkJsRpHmwkP30239112 = -952762715;    float MIjKgHYXoXkJsRpHmwkP13213531 = -505594225;    float MIjKgHYXoXkJsRpHmwkP27782081 = -76353989;    float MIjKgHYXoXkJsRpHmwkP6841427 = -984050543;    float MIjKgHYXoXkJsRpHmwkP62166474 = 37742921;    float MIjKgHYXoXkJsRpHmwkP8179826 = -486745107;    float MIjKgHYXoXkJsRpHmwkP94597971 = -866863931;    float MIjKgHYXoXkJsRpHmwkP31305364 = -382354424;    float MIjKgHYXoXkJsRpHmwkP60173667 = -727041173;    float MIjKgHYXoXkJsRpHmwkP49405896 = -131176894;    float MIjKgHYXoXkJsRpHmwkP48881207 = 82680982;    float MIjKgHYXoXkJsRpHmwkP15778042 = -811287099;    float MIjKgHYXoXkJsRpHmwkP51957918 = -320429631;    float MIjKgHYXoXkJsRpHmwkP18245003 = -786709433;    float MIjKgHYXoXkJsRpHmwkP12365363 = -297171576;    float MIjKgHYXoXkJsRpHmwkP67288719 = -909526872;    float MIjKgHYXoXkJsRpHmwkP25832478 = -736467653;    float MIjKgHYXoXkJsRpHmwkP67498310 = -831727330;    float MIjKgHYXoXkJsRpHmwkP96873464 = -561029398;    float MIjKgHYXoXkJsRpHmwkP92300173 = -893485500;    float MIjKgHYXoXkJsRpHmwkP56341098 = -898533249;    float MIjKgHYXoXkJsRpHmwkP45324197 = -412036987;    float MIjKgHYXoXkJsRpHmwkP17521573 = -766128026;    float MIjKgHYXoXkJsRpHmwkP7376022 = -104825397;    float MIjKgHYXoXkJsRpHmwkP65172690 = -629741495;    float MIjKgHYXoXkJsRpHmwkP79636658 = 43754910;    float MIjKgHYXoXkJsRpHmwkP45638807 = -773225396;    float MIjKgHYXoXkJsRpHmwkP48628535 = -271566492;    float MIjKgHYXoXkJsRpHmwkP65616424 = -340947761;    float MIjKgHYXoXkJsRpHmwkP9658296 = -30899870;    float MIjKgHYXoXkJsRpHmwkP13710450 = -630393496;    float MIjKgHYXoXkJsRpHmwkP21464849 = -140130930;    float MIjKgHYXoXkJsRpHmwkP77178336 = -172290127;    float MIjKgHYXoXkJsRpHmwkP35903562 = -589763737;    float MIjKgHYXoXkJsRpHmwkP10890204 = -802171745;    float MIjKgHYXoXkJsRpHmwkP87198728 = -189406226;    float MIjKgHYXoXkJsRpHmwkP46403984 = -226167934;    float MIjKgHYXoXkJsRpHmwkP73442671 = -773818004;    float MIjKgHYXoXkJsRpHmwkP91903448 = -383437262;    float MIjKgHYXoXkJsRpHmwkP49750322 = -296556412;    float MIjKgHYXoXkJsRpHmwkP25084799 = -847000664;    float MIjKgHYXoXkJsRpHmwkP94214723 = -317532388;    float MIjKgHYXoXkJsRpHmwkP58834826 = -336325080;    float MIjKgHYXoXkJsRpHmwkP38985333 = -826097163;    float MIjKgHYXoXkJsRpHmwkP48749087 = -696952797;    float MIjKgHYXoXkJsRpHmwkP42894004 = -182138430;    float MIjKgHYXoXkJsRpHmwkP61000336 = -293593194;    float MIjKgHYXoXkJsRpHmwkP91322618 = -863777294;    float MIjKgHYXoXkJsRpHmwkP26616585 = -499718462;    float MIjKgHYXoXkJsRpHmwkP15225001 = -131170796;    float MIjKgHYXoXkJsRpHmwkP99049342 = -393263358;    float MIjKgHYXoXkJsRpHmwkP87698140 = -854489046;    float MIjKgHYXoXkJsRpHmwkP3272741 = -247456597;    float MIjKgHYXoXkJsRpHmwkP92029813 = 29674259;    float MIjKgHYXoXkJsRpHmwkP41110294 = -866806735;    float MIjKgHYXoXkJsRpHmwkP84914914 = -440725729;    float MIjKgHYXoXkJsRpHmwkP95691957 = -739466200;    float MIjKgHYXoXkJsRpHmwkP20406059 = -971528592;    float MIjKgHYXoXkJsRpHmwkP41668737 = -254309048;    float MIjKgHYXoXkJsRpHmwkP82529816 = 93988010;    float MIjKgHYXoXkJsRpHmwkP62541019 = -713519712;    float MIjKgHYXoXkJsRpHmwkP45969436 = -495297440;    float MIjKgHYXoXkJsRpHmwkP65688939 = 58593336;    float MIjKgHYXoXkJsRpHmwkP50515371 = -596141304;    float MIjKgHYXoXkJsRpHmwkP35695446 = -500783398;    float MIjKgHYXoXkJsRpHmwkP27416358 = -777188089;    float MIjKgHYXoXkJsRpHmwkP38599706 = -538996972;    float MIjKgHYXoXkJsRpHmwkP16054357 = -730665895;    float MIjKgHYXoXkJsRpHmwkP7354799 = -984537688;    float MIjKgHYXoXkJsRpHmwkP25166635 = -770267914;     MIjKgHYXoXkJsRpHmwkP92594912 = MIjKgHYXoXkJsRpHmwkP55206428;     MIjKgHYXoXkJsRpHmwkP55206428 = MIjKgHYXoXkJsRpHmwkP87610482;     MIjKgHYXoXkJsRpHmwkP87610482 = MIjKgHYXoXkJsRpHmwkP48130144;     MIjKgHYXoXkJsRpHmwkP48130144 = MIjKgHYXoXkJsRpHmwkP34540117;     MIjKgHYXoXkJsRpHmwkP34540117 = MIjKgHYXoXkJsRpHmwkP68906900;     MIjKgHYXoXkJsRpHmwkP68906900 = MIjKgHYXoXkJsRpHmwkP21611055;     MIjKgHYXoXkJsRpHmwkP21611055 = MIjKgHYXoXkJsRpHmwkP61508323;     MIjKgHYXoXkJsRpHmwkP61508323 = MIjKgHYXoXkJsRpHmwkP33354955;     MIjKgHYXoXkJsRpHmwkP33354955 = MIjKgHYXoXkJsRpHmwkP41129316;     MIjKgHYXoXkJsRpHmwkP41129316 = MIjKgHYXoXkJsRpHmwkP412259;     MIjKgHYXoXkJsRpHmwkP412259 = MIjKgHYXoXkJsRpHmwkP74186064;     MIjKgHYXoXkJsRpHmwkP74186064 = MIjKgHYXoXkJsRpHmwkP80284098;     MIjKgHYXoXkJsRpHmwkP80284098 = MIjKgHYXoXkJsRpHmwkP54069923;     MIjKgHYXoXkJsRpHmwkP54069923 = MIjKgHYXoXkJsRpHmwkP57930148;     MIjKgHYXoXkJsRpHmwkP57930148 = MIjKgHYXoXkJsRpHmwkP19682771;     MIjKgHYXoXkJsRpHmwkP19682771 = MIjKgHYXoXkJsRpHmwkP25520087;     MIjKgHYXoXkJsRpHmwkP25520087 = MIjKgHYXoXkJsRpHmwkP19008494;     MIjKgHYXoXkJsRpHmwkP19008494 = MIjKgHYXoXkJsRpHmwkP88391229;     MIjKgHYXoXkJsRpHmwkP88391229 = MIjKgHYXoXkJsRpHmwkP97630294;     MIjKgHYXoXkJsRpHmwkP97630294 = MIjKgHYXoXkJsRpHmwkP58672046;     MIjKgHYXoXkJsRpHmwkP58672046 = MIjKgHYXoXkJsRpHmwkP12958254;     MIjKgHYXoXkJsRpHmwkP12958254 = MIjKgHYXoXkJsRpHmwkP9567622;     MIjKgHYXoXkJsRpHmwkP9567622 = MIjKgHYXoXkJsRpHmwkP38981947;     MIjKgHYXoXkJsRpHmwkP38981947 = MIjKgHYXoXkJsRpHmwkP82513720;     MIjKgHYXoXkJsRpHmwkP82513720 = MIjKgHYXoXkJsRpHmwkP24881821;     MIjKgHYXoXkJsRpHmwkP24881821 = MIjKgHYXoXkJsRpHmwkP55196450;     MIjKgHYXoXkJsRpHmwkP55196450 = MIjKgHYXoXkJsRpHmwkP146206;     MIjKgHYXoXkJsRpHmwkP146206 = MIjKgHYXoXkJsRpHmwkP84329987;     MIjKgHYXoXkJsRpHmwkP84329987 = MIjKgHYXoXkJsRpHmwkP97451392;     MIjKgHYXoXkJsRpHmwkP97451392 = MIjKgHYXoXkJsRpHmwkP30239112;     MIjKgHYXoXkJsRpHmwkP30239112 = MIjKgHYXoXkJsRpHmwkP13213531;     MIjKgHYXoXkJsRpHmwkP13213531 = MIjKgHYXoXkJsRpHmwkP27782081;     MIjKgHYXoXkJsRpHmwkP27782081 = MIjKgHYXoXkJsRpHmwkP6841427;     MIjKgHYXoXkJsRpHmwkP6841427 = MIjKgHYXoXkJsRpHmwkP62166474;     MIjKgHYXoXkJsRpHmwkP62166474 = MIjKgHYXoXkJsRpHmwkP8179826;     MIjKgHYXoXkJsRpHmwkP8179826 = MIjKgHYXoXkJsRpHmwkP94597971;     MIjKgHYXoXkJsRpHmwkP94597971 = MIjKgHYXoXkJsRpHmwkP31305364;     MIjKgHYXoXkJsRpHmwkP31305364 = MIjKgHYXoXkJsRpHmwkP60173667;     MIjKgHYXoXkJsRpHmwkP60173667 = MIjKgHYXoXkJsRpHmwkP49405896;     MIjKgHYXoXkJsRpHmwkP49405896 = MIjKgHYXoXkJsRpHmwkP48881207;     MIjKgHYXoXkJsRpHmwkP48881207 = MIjKgHYXoXkJsRpHmwkP15778042;     MIjKgHYXoXkJsRpHmwkP15778042 = MIjKgHYXoXkJsRpHmwkP51957918;     MIjKgHYXoXkJsRpHmwkP51957918 = MIjKgHYXoXkJsRpHmwkP18245003;     MIjKgHYXoXkJsRpHmwkP18245003 = MIjKgHYXoXkJsRpHmwkP12365363;     MIjKgHYXoXkJsRpHmwkP12365363 = MIjKgHYXoXkJsRpHmwkP67288719;     MIjKgHYXoXkJsRpHmwkP67288719 = MIjKgHYXoXkJsRpHmwkP25832478;     MIjKgHYXoXkJsRpHmwkP25832478 = MIjKgHYXoXkJsRpHmwkP67498310;     MIjKgHYXoXkJsRpHmwkP67498310 = MIjKgHYXoXkJsRpHmwkP96873464;     MIjKgHYXoXkJsRpHmwkP96873464 = MIjKgHYXoXkJsRpHmwkP92300173;     MIjKgHYXoXkJsRpHmwkP92300173 = MIjKgHYXoXkJsRpHmwkP56341098;     MIjKgHYXoXkJsRpHmwkP56341098 = MIjKgHYXoXkJsRpHmwkP45324197;     MIjKgHYXoXkJsRpHmwkP45324197 = MIjKgHYXoXkJsRpHmwkP17521573;     MIjKgHYXoXkJsRpHmwkP17521573 = MIjKgHYXoXkJsRpHmwkP7376022;     MIjKgHYXoXkJsRpHmwkP7376022 = MIjKgHYXoXkJsRpHmwkP65172690;     MIjKgHYXoXkJsRpHmwkP65172690 = MIjKgHYXoXkJsRpHmwkP79636658;     MIjKgHYXoXkJsRpHmwkP79636658 = MIjKgHYXoXkJsRpHmwkP45638807;     MIjKgHYXoXkJsRpHmwkP45638807 = MIjKgHYXoXkJsRpHmwkP48628535;     MIjKgHYXoXkJsRpHmwkP48628535 = MIjKgHYXoXkJsRpHmwkP65616424;     MIjKgHYXoXkJsRpHmwkP65616424 = MIjKgHYXoXkJsRpHmwkP9658296;     MIjKgHYXoXkJsRpHmwkP9658296 = MIjKgHYXoXkJsRpHmwkP13710450;     MIjKgHYXoXkJsRpHmwkP13710450 = MIjKgHYXoXkJsRpHmwkP21464849;     MIjKgHYXoXkJsRpHmwkP21464849 = MIjKgHYXoXkJsRpHmwkP77178336;     MIjKgHYXoXkJsRpHmwkP77178336 = MIjKgHYXoXkJsRpHmwkP35903562;     MIjKgHYXoXkJsRpHmwkP35903562 = MIjKgHYXoXkJsRpHmwkP10890204;     MIjKgHYXoXkJsRpHmwkP10890204 = MIjKgHYXoXkJsRpHmwkP87198728;     MIjKgHYXoXkJsRpHmwkP87198728 = MIjKgHYXoXkJsRpHmwkP46403984;     MIjKgHYXoXkJsRpHmwkP46403984 = MIjKgHYXoXkJsRpHmwkP73442671;     MIjKgHYXoXkJsRpHmwkP73442671 = MIjKgHYXoXkJsRpHmwkP91903448;     MIjKgHYXoXkJsRpHmwkP91903448 = MIjKgHYXoXkJsRpHmwkP49750322;     MIjKgHYXoXkJsRpHmwkP49750322 = MIjKgHYXoXkJsRpHmwkP25084799;     MIjKgHYXoXkJsRpHmwkP25084799 = MIjKgHYXoXkJsRpHmwkP94214723;     MIjKgHYXoXkJsRpHmwkP94214723 = MIjKgHYXoXkJsRpHmwkP58834826;     MIjKgHYXoXkJsRpHmwkP58834826 = MIjKgHYXoXkJsRpHmwkP38985333;     MIjKgHYXoXkJsRpHmwkP38985333 = MIjKgHYXoXkJsRpHmwkP48749087;     MIjKgHYXoXkJsRpHmwkP48749087 = MIjKgHYXoXkJsRpHmwkP42894004;     MIjKgHYXoXkJsRpHmwkP42894004 = MIjKgHYXoXkJsRpHmwkP61000336;     MIjKgHYXoXkJsRpHmwkP61000336 = MIjKgHYXoXkJsRpHmwkP91322618;     MIjKgHYXoXkJsRpHmwkP91322618 = MIjKgHYXoXkJsRpHmwkP26616585;     MIjKgHYXoXkJsRpHmwkP26616585 = MIjKgHYXoXkJsRpHmwkP15225001;     MIjKgHYXoXkJsRpHmwkP15225001 = MIjKgHYXoXkJsRpHmwkP99049342;     MIjKgHYXoXkJsRpHmwkP99049342 = MIjKgHYXoXkJsRpHmwkP87698140;     MIjKgHYXoXkJsRpHmwkP87698140 = MIjKgHYXoXkJsRpHmwkP3272741;     MIjKgHYXoXkJsRpHmwkP3272741 = MIjKgHYXoXkJsRpHmwkP92029813;     MIjKgHYXoXkJsRpHmwkP92029813 = MIjKgHYXoXkJsRpHmwkP41110294;     MIjKgHYXoXkJsRpHmwkP41110294 = MIjKgHYXoXkJsRpHmwkP84914914;     MIjKgHYXoXkJsRpHmwkP84914914 = MIjKgHYXoXkJsRpHmwkP95691957;     MIjKgHYXoXkJsRpHmwkP95691957 = MIjKgHYXoXkJsRpHmwkP20406059;     MIjKgHYXoXkJsRpHmwkP20406059 = MIjKgHYXoXkJsRpHmwkP41668737;     MIjKgHYXoXkJsRpHmwkP41668737 = MIjKgHYXoXkJsRpHmwkP82529816;     MIjKgHYXoXkJsRpHmwkP82529816 = MIjKgHYXoXkJsRpHmwkP62541019;     MIjKgHYXoXkJsRpHmwkP62541019 = MIjKgHYXoXkJsRpHmwkP45969436;     MIjKgHYXoXkJsRpHmwkP45969436 = MIjKgHYXoXkJsRpHmwkP65688939;     MIjKgHYXoXkJsRpHmwkP65688939 = MIjKgHYXoXkJsRpHmwkP50515371;     MIjKgHYXoXkJsRpHmwkP50515371 = MIjKgHYXoXkJsRpHmwkP35695446;     MIjKgHYXoXkJsRpHmwkP35695446 = MIjKgHYXoXkJsRpHmwkP27416358;     MIjKgHYXoXkJsRpHmwkP27416358 = MIjKgHYXoXkJsRpHmwkP38599706;     MIjKgHYXoXkJsRpHmwkP38599706 = MIjKgHYXoXkJsRpHmwkP16054357;     MIjKgHYXoXkJsRpHmwkP16054357 = MIjKgHYXoXkJsRpHmwkP7354799;     MIjKgHYXoXkJsRpHmwkP7354799 = MIjKgHYXoXkJsRpHmwkP25166635;     MIjKgHYXoXkJsRpHmwkP25166635 = MIjKgHYXoXkJsRpHmwkP92594912;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void tlltdACRhBlgYUJraESh78260948() {     float rTQWBuUrURCAwyIINUoR63246579 = -288417045;    float rTQWBuUrURCAwyIINUoR23879946 = -295355422;    float rTQWBuUrURCAwyIINUoR67579401 = -610750989;    float rTQWBuUrURCAwyIINUoR88061430 = -671783817;    float rTQWBuUrURCAwyIINUoR55990438 = -44037227;    float rTQWBuUrURCAwyIINUoR58906662 = -18191023;    float rTQWBuUrURCAwyIINUoR39989950 = -47392634;    float rTQWBuUrURCAwyIINUoR18350704 = -482918458;    float rTQWBuUrURCAwyIINUoR79220008 = -448451049;    float rTQWBuUrURCAwyIINUoR64095776 = 43970190;    float rTQWBuUrURCAwyIINUoR42661419 = -829902379;    float rTQWBuUrURCAwyIINUoR19732636 = -718285715;    float rTQWBuUrURCAwyIINUoR60111485 = -667405606;    float rTQWBuUrURCAwyIINUoR37916957 = -246722551;    float rTQWBuUrURCAwyIINUoR40650167 = 66948478;    float rTQWBuUrURCAwyIINUoR52211411 = -283848181;    float rTQWBuUrURCAwyIINUoR19372914 = -709071941;    float rTQWBuUrURCAwyIINUoR39301988 = -52532449;    float rTQWBuUrURCAwyIINUoR70099011 = -664461999;    float rTQWBuUrURCAwyIINUoR56417003 = -156638119;    float rTQWBuUrURCAwyIINUoR13807801 = 97895331;    float rTQWBuUrURCAwyIINUoR74354018 = 54691869;    float rTQWBuUrURCAwyIINUoR11511430 = -118329872;    float rTQWBuUrURCAwyIINUoR72831484 = -489290944;    float rTQWBuUrURCAwyIINUoR46638108 = -816602355;    float rTQWBuUrURCAwyIINUoR55479402 = -228015455;    float rTQWBuUrURCAwyIINUoR63457236 = 18605849;    float rTQWBuUrURCAwyIINUoR7928431 = -313609762;    float rTQWBuUrURCAwyIINUoR16472475 = -158169458;    float rTQWBuUrURCAwyIINUoR36623489 = -223735054;    float rTQWBuUrURCAwyIINUoR37311108 = -701196400;    float rTQWBuUrURCAwyIINUoR57724991 = -833891388;    float rTQWBuUrURCAwyIINUoR10727442 = -23315598;    float rTQWBuUrURCAwyIINUoR72206402 = -547580228;    float rTQWBuUrURCAwyIINUoR40190711 = -909203858;    float rTQWBuUrURCAwyIINUoR68173085 = -565971655;    float rTQWBuUrURCAwyIINUoR38389458 = 44542862;    float rTQWBuUrURCAwyIINUoR84097852 = -166467531;    float rTQWBuUrURCAwyIINUoR38706956 = -682769810;    float rTQWBuUrURCAwyIINUoR44509194 = -748785257;    float rTQWBuUrURCAwyIINUoR8954492 = -301337897;    float rTQWBuUrURCAwyIINUoR44261033 = -580676168;    float rTQWBuUrURCAwyIINUoR924878 = -307183684;    float rTQWBuUrURCAwyIINUoR24753304 = -858684405;    float rTQWBuUrURCAwyIINUoR8319421 = -605817971;    float rTQWBuUrURCAwyIINUoR73694554 = -667864027;    float rTQWBuUrURCAwyIINUoR51963681 = -852537452;    float rTQWBuUrURCAwyIINUoR33112520 = -183857625;    float rTQWBuUrURCAwyIINUoR2314918 = -546793367;    float rTQWBuUrURCAwyIINUoR727661 = -76494379;    float rTQWBuUrURCAwyIINUoR24530423 = -445105370;    float rTQWBuUrURCAwyIINUoR87195701 = -381256708;    float rTQWBuUrURCAwyIINUoR72597229 = -693926068;    float rTQWBuUrURCAwyIINUoR97603326 = -765706894;    float rTQWBuUrURCAwyIINUoR64453148 = -740682410;    float rTQWBuUrURCAwyIINUoR88892560 = -243108915;    float rTQWBuUrURCAwyIINUoR12368516 = -77025551;    float rTQWBuUrURCAwyIINUoR94747917 = -21460045;    float rTQWBuUrURCAwyIINUoR41423323 = -855181462;    float rTQWBuUrURCAwyIINUoR511037 = -816021772;    float rTQWBuUrURCAwyIINUoR95449426 = 63203127;    float rTQWBuUrURCAwyIINUoR32061519 = -733782872;    float rTQWBuUrURCAwyIINUoR1878229 = -224749000;    float rTQWBuUrURCAwyIINUoR42596520 = -124715995;    float rTQWBuUrURCAwyIINUoR26784669 = -254833411;    float rTQWBuUrURCAwyIINUoR84936428 = -996010992;    float rTQWBuUrURCAwyIINUoR9005195 = -594970118;    float rTQWBuUrURCAwyIINUoR87905083 = -19825378;    float rTQWBuUrURCAwyIINUoR97726246 = -337518693;    float rTQWBuUrURCAwyIINUoR72477082 = -367079867;    float rTQWBuUrURCAwyIINUoR13821954 = -228391043;    float rTQWBuUrURCAwyIINUoR35275061 = -442604410;    float rTQWBuUrURCAwyIINUoR595032 = -369762640;    float rTQWBuUrURCAwyIINUoR25589817 = -915676743;    float rTQWBuUrURCAwyIINUoR47462512 = -855300223;    float rTQWBuUrURCAwyIINUoR69546767 = -321428501;    float rTQWBuUrURCAwyIINUoR73429140 = -638124448;    float rTQWBuUrURCAwyIINUoR86758126 = -259645467;    float rTQWBuUrURCAwyIINUoR64512063 = -883472974;    float rTQWBuUrURCAwyIINUoR72943554 = -48738328;    float rTQWBuUrURCAwyIINUoR3515721 = -375478003;    float rTQWBuUrURCAwyIINUoR30344716 = -797536526;    float rTQWBuUrURCAwyIINUoR5613514 = -766816395;    float rTQWBuUrURCAwyIINUoR15744815 = 18324921;    float rTQWBuUrURCAwyIINUoR12093066 = -778629685;    float rTQWBuUrURCAwyIINUoR50115406 = -219939693;    float rTQWBuUrURCAwyIINUoR85127761 = -39965320;    float rTQWBuUrURCAwyIINUoR13124116 = -257608704;    float rTQWBuUrURCAwyIINUoR7753254 = -806897818;    float rTQWBuUrURCAwyIINUoR51298150 = -566094944;    float rTQWBuUrURCAwyIINUoR55804569 = -388946105;    float rTQWBuUrURCAwyIINUoR43641540 = -933997094;    float rTQWBuUrURCAwyIINUoR42674530 = -311286070;    float rTQWBuUrURCAwyIINUoR38195919 = -866748038;    float rTQWBuUrURCAwyIINUoR49059767 = -711988385;    float rTQWBuUrURCAwyIINUoR76892972 = -567555025;    float rTQWBuUrURCAwyIINUoR42382804 = -255927169;    float rTQWBuUrURCAwyIINUoR58328358 = -82467689;    float rTQWBuUrURCAwyIINUoR97968635 = -503850995;    float rTQWBuUrURCAwyIINUoR23382993 = -288417045;     rTQWBuUrURCAwyIINUoR63246579 = rTQWBuUrURCAwyIINUoR23879946;     rTQWBuUrURCAwyIINUoR23879946 = rTQWBuUrURCAwyIINUoR67579401;     rTQWBuUrURCAwyIINUoR67579401 = rTQWBuUrURCAwyIINUoR88061430;     rTQWBuUrURCAwyIINUoR88061430 = rTQWBuUrURCAwyIINUoR55990438;     rTQWBuUrURCAwyIINUoR55990438 = rTQWBuUrURCAwyIINUoR58906662;     rTQWBuUrURCAwyIINUoR58906662 = rTQWBuUrURCAwyIINUoR39989950;     rTQWBuUrURCAwyIINUoR39989950 = rTQWBuUrURCAwyIINUoR18350704;     rTQWBuUrURCAwyIINUoR18350704 = rTQWBuUrURCAwyIINUoR79220008;     rTQWBuUrURCAwyIINUoR79220008 = rTQWBuUrURCAwyIINUoR64095776;     rTQWBuUrURCAwyIINUoR64095776 = rTQWBuUrURCAwyIINUoR42661419;     rTQWBuUrURCAwyIINUoR42661419 = rTQWBuUrURCAwyIINUoR19732636;     rTQWBuUrURCAwyIINUoR19732636 = rTQWBuUrURCAwyIINUoR60111485;     rTQWBuUrURCAwyIINUoR60111485 = rTQWBuUrURCAwyIINUoR37916957;     rTQWBuUrURCAwyIINUoR37916957 = rTQWBuUrURCAwyIINUoR40650167;     rTQWBuUrURCAwyIINUoR40650167 = rTQWBuUrURCAwyIINUoR52211411;     rTQWBuUrURCAwyIINUoR52211411 = rTQWBuUrURCAwyIINUoR19372914;     rTQWBuUrURCAwyIINUoR19372914 = rTQWBuUrURCAwyIINUoR39301988;     rTQWBuUrURCAwyIINUoR39301988 = rTQWBuUrURCAwyIINUoR70099011;     rTQWBuUrURCAwyIINUoR70099011 = rTQWBuUrURCAwyIINUoR56417003;     rTQWBuUrURCAwyIINUoR56417003 = rTQWBuUrURCAwyIINUoR13807801;     rTQWBuUrURCAwyIINUoR13807801 = rTQWBuUrURCAwyIINUoR74354018;     rTQWBuUrURCAwyIINUoR74354018 = rTQWBuUrURCAwyIINUoR11511430;     rTQWBuUrURCAwyIINUoR11511430 = rTQWBuUrURCAwyIINUoR72831484;     rTQWBuUrURCAwyIINUoR72831484 = rTQWBuUrURCAwyIINUoR46638108;     rTQWBuUrURCAwyIINUoR46638108 = rTQWBuUrURCAwyIINUoR55479402;     rTQWBuUrURCAwyIINUoR55479402 = rTQWBuUrURCAwyIINUoR63457236;     rTQWBuUrURCAwyIINUoR63457236 = rTQWBuUrURCAwyIINUoR7928431;     rTQWBuUrURCAwyIINUoR7928431 = rTQWBuUrURCAwyIINUoR16472475;     rTQWBuUrURCAwyIINUoR16472475 = rTQWBuUrURCAwyIINUoR36623489;     rTQWBuUrURCAwyIINUoR36623489 = rTQWBuUrURCAwyIINUoR37311108;     rTQWBuUrURCAwyIINUoR37311108 = rTQWBuUrURCAwyIINUoR57724991;     rTQWBuUrURCAwyIINUoR57724991 = rTQWBuUrURCAwyIINUoR10727442;     rTQWBuUrURCAwyIINUoR10727442 = rTQWBuUrURCAwyIINUoR72206402;     rTQWBuUrURCAwyIINUoR72206402 = rTQWBuUrURCAwyIINUoR40190711;     rTQWBuUrURCAwyIINUoR40190711 = rTQWBuUrURCAwyIINUoR68173085;     rTQWBuUrURCAwyIINUoR68173085 = rTQWBuUrURCAwyIINUoR38389458;     rTQWBuUrURCAwyIINUoR38389458 = rTQWBuUrURCAwyIINUoR84097852;     rTQWBuUrURCAwyIINUoR84097852 = rTQWBuUrURCAwyIINUoR38706956;     rTQWBuUrURCAwyIINUoR38706956 = rTQWBuUrURCAwyIINUoR44509194;     rTQWBuUrURCAwyIINUoR44509194 = rTQWBuUrURCAwyIINUoR8954492;     rTQWBuUrURCAwyIINUoR8954492 = rTQWBuUrURCAwyIINUoR44261033;     rTQWBuUrURCAwyIINUoR44261033 = rTQWBuUrURCAwyIINUoR924878;     rTQWBuUrURCAwyIINUoR924878 = rTQWBuUrURCAwyIINUoR24753304;     rTQWBuUrURCAwyIINUoR24753304 = rTQWBuUrURCAwyIINUoR8319421;     rTQWBuUrURCAwyIINUoR8319421 = rTQWBuUrURCAwyIINUoR73694554;     rTQWBuUrURCAwyIINUoR73694554 = rTQWBuUrURCAwyIINUoR51963681;     rTQWBuUrURCAwyIINUoR51963681 = rTQWBuUrURCAwyIINUoR33112520;     rTQWBuUrURCAwyIINUoR33112520 = rTQWBuUrURCAwyIINUoR2314918;     rTQWBuUrURCAwyIINUoR2314918 = rTQWBuUrURCAwyIINUoR727661;     rTQWBuUrURCAwyIINUoR727661 = rTQWBuUrURCAwyIINUoR24530423;     rTQWBuUrURCAwyIINUoR24530423 = rTQWBuUrURCAwyIINUoR87195701;     rTQWBuUrURCAwyIINUoR87195701 = rTQWBuUrURCAwyIINUoR72597229;     rTQWBuUrURCAwyIINUoR72597229 = rTQWBuUrURCAwyIINUoR97603326;     rTQWBuUrURCAwyIINUoR97603326 = rTQWBuUrURCAwyIINUoR64453148;     rTQWBuUrURCAwyIINUoR64453148 = rTQWBuUrURCAwyIINUoR88892560;     rTQWBuUrURCAwyIINUoR88892560 = rTQWBuUrURCAwyIINUoR12368516;     rTQWBuUrURCAwyIINUoR12368516 = rTQWBuUrURCAwyIINUoR94747917;     rTQWBuUrURCAwyIINUoR94747917 = rTQWBuUrURCAwyIINUoR41423323;     rTQWBuUrURCAwyIINUoR41423323 = rTQWBuUrURCAwyIINUoR511037;     rTQWBuUrURCAwyIINUoR511037 = rTQWBuUrURCAwyIINUoR95449426;     rTQWBuUrURCAwyIINUoR95449426 = rTQWBuUrURCAwyIINUoR32061519;     rTQWBuUrURCAwyIINUoR32061519 = rTQWBuUrURCAwyIINUoR1878229;     rTQWBuUrURCAwyIINUoR1878229 = rTQWBuUrURCAwyIINUoR42596520;     rTQWBuUrURCAwyIINUoR42596520 = rTQWBuUrURCAwyIINUoR26784669;     rTQWBuUrURCAwyIINUoR26784669 = rTQWBuUrURCAwyIINUoR84936428;     rTQWBuUrURCAwyIINUoR84936428 = rTQWBuUrURCAwyIINUoR9005195;     rTQWBuUrURCAwyIINUoR9005195 = rTQWBuUrURCAwyIINUoR87905083;     rTQWBuUrURCAwyIINUoR87905083 = rTQWBuUrURCAwyIINUoR97726246;     rTQWBuUrURCAwyIINUoR97726246 = rTQWBuUrURCAwyIINUoR72477082;     rTQWBuUrURCAwyIINUoR72477082 = rTQWBuUrURCAwyIINUoR13821954;     rTQWBuUrURCAwyIINUoR13821954 = rTQWBuUrURCAwyIINUoR35275061;     rTQWBuUrURCAwyIINUoR35275061 = rTQWBuUrURCAwyIINUoR595032;     rTQWBuUrURCAwyIINUoR595032 = rTQWBuUrURCAwyIINUoR25589817;     rTQWBuUrURCAwyIINUoR25589817 = rTQWBuUrURCAwyIINUoR47462512;     rTQWBuUrURCAwyIINUoR47462512 = rTQWBuUrURCAwyIINUoR69546767;     rTQWBuUrURCAwyIINUoR69546767 = rTQWBuUrURCAwyIINUoR73429140;     rTQWBuUrURCAwyIINUoR73429140 = rTQWBuUrURCAwyIINUoR86758126;     rTQWBuUrURCAwyIINUoR86758126 = rTQWBuUrURCAwyIINUoR64512063;     rTQWBuUrURCAwyIINUoR64512063 = rTQWBuUrURCAwyIINUoR72943554;     rTQWBuUrURCAwyIINUoR72943554 = rTQWBuUrURCAwyIINUoR3515721;     rTQWBuUrURCAwyIINUoR3515721 = rTQWBuUrURCAwyIINUoR30344716;     rTQWBuUrURCAwyIINUoR30344716 = rTQWBuUrURCAwyIINUoR5613514;     rTQWBuUrURCAwyIINUoR5613514 = rTQWBuUrURCAwyIINUoR15744815;     rTQWBuUrURCAwyIINUoR15744815 = rTQWBuUrURCAwyIINUoR12093066;     rTQWBuUrURCAwyIINUoR12093066 = rTQWBuUrURCAwyIINUoR50115406;     rTQWBuUrURCAwyIINUoR50115406 = rTQWBuUrURCAwyIINUoR85127761;     rTQWBuUrURCAwyIINUoR85127761 = rTQWBuUrURCAwyIINUoR13124116;     rTQWBuUrURCAwyIINUoR13124116 = rTQWBuUrURCAwyIINUoR7753254;     rTQWBuUrURCAwyIINUoR7753254 = rTQWBuUrURCAwyIINUoR51298150;     rTQWBuUrURCAwyIINUoR51298150 = rTQWBuUrURCAwyIINUoR55804569;     rTQWBuUrURCAwyIINUoR55804569 = rTQWBuUrURCAwyIINUoR43641540;     rTQWBuUrURCAwyIINUoR43641540 = rTQWBuUrURCAwyIINUoR42674530;     rTQWBuUrURCAwyIINUoR42674530 = rTQWBuUrURCAwyIINUoR38195919;     rTQWBuUrURCAwyIINUoR38195919 = rTQWBuUrURCAwyIINUoR49059767;     rTQWBuUrURCAwyIINUoR49059767 = rTQWBuUrURCAwyIINUoR76892972;     rTQWBuUrURCAwyIINUoR76892972 = rTQWBuUrURCAwyIINUoR42382804;     rTQWBuUrURCAwyIINUoR42382804 = rTQWBuUrURCAwyIINUoR58328358;     rTQWBuUrURCAwyIINUoR58328358 = rTQWBuUrURCAwyIINUoR97968635;     rTQWBuUrURCAwyIINUoR97968635 = rTQWBuUrURCAwyIINUoR23382993;     rTQWBuUrURCAwyIINUoR23382993 = rTQWBuUrURCAwyIINUoR63246579;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZVbfYnLMUuQCUGhGmanT84919693() {     float TUTrwhGuUfeVPfQyTALA63240108 = -160941186;    float TUTrwhGuUfeVPfQyTALA62452262 = -337950547;    float TUTrwhGuUfeVPfQyTALA68892613 = -567879602;    float TUTrwhGuUfeVPfQyTALA73141291 = -492324647;    float TUTrwhGuUfeVPfQyTALA64830540 = -512392549;    float TUTrwhGuUfeVPfQyTALA93712094 = -817623835;    float TUTrwhGuUfeVPfQyTALA8537287 = -851929101;    float TUTrwhGuUfeVPfQyTALA87564754 = -187095775;    float TUTrwhGuUfeVPfQyTALA61960173 = -162097119;    float TUTrwhGuUfeVPfQyTALA60735290 = -692836682;    float TUTrwhGuUfeVPfQyTALA62982024 = -789374451;    float TUTrwhGuUfeVPfQyTALA34024313 = -453764078;    float TUTrwhGuUfeVPfQyTALA91964398 = -689064080;    float TUTrwhGuUfeVPfQyTALA91761177 = -591875602;    float TUTrwhGuUfeVPfQyTALA85822276 = -995764374;    float TUTrwhGuUfeVPfQyTALA26775959 = -616643831;    float TUTrwhGuUfeVPfQyTALA75225673 = -528686511;    float TUTrwhGuUfeVPfQyTALA62302520 = -449604071;    float TUTrwhGuUfeVPfQyTALA90527280 = -182444266;    float TUTrwhGuUfeVPfQyTALA28490767 = -768662571;    float TUTrwhGuUfeVPfQyTALA93477224 = -712571042;    float TUTrwhGuUfeVPfQyTALA52650284 = -455300014;    float TUTrwhGuUfeVPfQyTALA48389827 = -577287714;    float TUTrwhGuUfeVPfQyTALA16481593 = -370211467;    float TUTrwhGuUfeVPfQyTALA75080798 = -143296208;    float TUTrwhGuUfeVPfQyTALA72053478 = -136221384;    float TUTrwhGuUfeVPfQyTALA33678818 = -282193864;    float TUTrwhGuUfeVPfQyTALA43331956 = -922576751;    float TUTrwhGuUfeVPfQyTALA29150922 = -804239773;    float TUTrwhGuUfeVPfQyTALA12713359 = -999166428;    float TUTrwhGuUfeVPfQyTALA70723786 = 11710365;    float TUTrwhGuUfeVPfQyTALA42448849 = -125713149;    float TUTrwhGuUfeVPfQyTALA96179864 = -225259814;    float TUTrwhGuUfeVPfQyTALA72489239 = -424261960;    float TUTrwhGuUfeVPfQyTALA22548749 = -68517783;    float TUTrwhGuUfeVPfQyTALA11001275 = -645737401;    float TUTrwhGuUfeVPfQyTALA50330825 = -55110516;    float TUTrwhGuUfeVPfQyTALA54015532 = -161730211;    float TUTrwhGuUfeVPfQyTALA74020552 = -295451226;    float TUTrwhGuUfeVPfQyTALA16382087 = -358216943;    float TUTrwhGuUfeVPfQyTALA96810158 = -389281795;    float TUTrwhGuUfeVPfQyTALA29958694 = -774468250;    float TUTrwhGuUfeVPfQyTALA61208238 = -866313938;    float TUTrwhGuUfeVPfQyTALA83985201 = -415648848;    float TUTrwhGuUfeVPfQyTALA92337473 = 20922628;    float TUTrwhGuUfeVPfQyTALA14122523 = -479908698;    float TUTrwhGuUfeVPfQyTALA79661474 = -233818074;    float TUTrwhGuUfeVPfQyTALA8496101 = -792618981;    float TUTrwhGuUfeVPfQyTALA20973307 = -110557229;    float TUTrwhGuUfeVPfQyTALA20773084 = -763519265;    float TUTrwhGuUfeVPfQyTALA37789793 = -23660377;    float TUTrwhGuUfeVPfQyTALA1742959 = -763402457;    float TUTrwhGuUfeVPfQyTALA48045578 = -585980076;    float TUTrwhGuUfeVPfQyTALA7853597 = -197626745;    float TUTrwhGuUfeVPfQyTALA91442468 = -497581688;    float TUTrwhGuUfeVPfQyTALA10589824 = -705641173;    float TUTrwhGuUfeVPfQyTALA14062436 = -760662834;    float TUTrwhGuUfeVPfQyTALA52411021 = -97668136;    float TUTrwhGuUfeVPfQyTALA98060493 = -249028439;    float TUTrwhGuUfeVPfQyTALA92777061 = -276171166;    float TUTrwhGuUfeVPfQyTALA60033277 = -435429972;    float TUTrwhGuUfeVPfQyTALA65205330 = -929352351;    float TUTrwhGuUfeVPfQyTALA58413832 = -382856002;    float TUTrwhGuUfeVPfQyTALA49246814 = -162930691;    float TUTrwhGuUfeVPfQyTALA90011503 = -604547047;    float TUTrwhGuUfeVPfQyTALA20533176 = -563661303;    float TUTrwhGuUfeVPfQyTALA37844448 = -128504265;    float TUTrwhGuUfeVPfQyTALA19475160 = -164802120;    float TUTrwhGuUfeVPfQyTALA69212428 = -423357819;    float TUTrwhGuUfeVPfQyTALA74821001 = -250026973;    float TUTrwhGuUfeVPfQyTALA76445133 = -461533315;    float TUTrwhGuUfeVPfQyTALA21210142 = -266956301;    float TUTrwhGuUfeVPfQyTALA88281968 = -54152845;    float TUTrwhGuUfeVPfQyTALA74145194 = -824227323;    float TUTrwhGuUfeVPfQyTALA31680608 = -279380776;    float TUTrwhGuUfeVPfQyTALA63518531 = -938102792;    float TUTrwhGuUfeVPfQyTALA91442046 = -588986077;    float TUTrwhGuUfeVPfQyTALA64404625 = -61638866;    float TUTrwhGuUfeVPfQyTALA24144119 = -291134095;    float TUTrwhGuUfeVPfQyTALA60958275 = -663387511;    float TUTrwhGuUfeVPfQyTALA92392004 = -902403310;    float TUTrwhGuUfeVPfQyTALA25182717 = -489574883;    float TUTrwhGuUfeVPfQyTALA22358650 = -712019522;    float TUTrwhGuUfeVPfQyTALA8377839 = 59279492;    float TUTrwhGuUfeVPfQyTALA74923565 = -875506051;    float TUTrwhGuUfeVPfQyTALA68980828 = -224887179;    float TUTrwhGuUfeVPfQyTALA94403270 = -539733074;    float TUTrwhGuUfeVPfQyTALA88326267 = 72366931;    float TUTrwhGuUfeVPfQyTALA81046770 = -926680273;    float TUTrwhGuUfeVPfQyTALA11958926 = -362876611;    float TUTrwhGuUfeVPfQyTALA96938838 = -885074568;    float TUTrwhGuUfeVPfQyTALA97919804 = -957442381;    float TUTrwhGuUfeVPfQyTALA55955038 = -912701772;    float TUTrwhGuUfeVPfQyTALA81243490 = 80719939;    float TUTrwhGuUfeVPfQyTALA56348809 = -922786972;    float TUTrwhGuUfeVPfQyTALA31604829 = -459929445;    float TUTrwhGuUfeVPfQyTALA71544861 = -291612249;    float TUTrwhGuUfeVPfQyTALA11961424 = -603383247;    float TUTrwhGuUfeVPfQyTALA93973698 = -811101801;    float TUTrwhGuUfeVPfQyTALA71804297 = -160941186;     TUTrwhGuUfeVPfQyTALA63240108 = TUTrwhGuUfeVPfQyTALA62452262;     TUTrwhGuUfeVPfQyTALA62452262 = TUTrwhGuUfeVPfQyTALA68892613;     TUTrwhGuUfeVPfQyTALA68892613 = TUTrwhGuUfeVPfQyTALA73141291;     TUTrwhGuUfeVPfQyTALA73141291 = TUTrwhGuUfeVPfQyTALA64830540;     TUTrwhGuUfeVPfQyTALA64830540 = TUTrwhGuUfeVPfQyTALA93712094;     TUTrwhGuUfeVPfQyTALA93712094 = TUTrwhGuUfeVPfQyTALA8537287;     TUTrwhGuUfeVPfQyTALA8537287 = TUTrwhGuUfeVPfQyTALA87564754;     TUTrwhGuUfeVPfQyTALA87564754 = TUTrwhGuUfeVPfQyTALA61960173;     TUTrwhGuUfeVPfQyTALA61960173 = TUTrwhGuUfeVPfQyTALA60735290;     TUTrwhGuUfeVPfQyTALA60735290 = TUTrwhGuUfeVPfQyTALA62982024;     TUTrwhGuUfeVPfQyTALA62982024 = TUTrwhGuUfeVPfQyTALA34024313;     TUTrwhGuUfeVPfQyTALA34024313 = TUTrwhGuUfeVPfQyTALA91964398;     TUTrwhGuUfeVPfQyTALA91964398 = TUTrwhGuUfeVPfQyTALA91761177;     TUTrwhGuUfeVPfQyTALA91761177 = TUTrwhGuUfeVPfQyTALA85822276;     TUTrwhGuUfeVPfQyTALA85822276 = TUTrwhGuUfeVPfQyTALA26775959;     TUTrwhGuUfeVPfQyTALA26775959 = TUTrwhGuUfeVPfQyTALA75225673;     TUTrwhGuUfeVPfQyTALA75225673 = TUTrwhGuUfeVPfQyTALA62302520;     TUTrwhGuUfeVPfQyTALA62302520 = TUTrwhGuUfeVPfQyTALA90527280;     TUTrwhGuUfeVPfQyTALA90527280 = TUTrwhGuUfeVPfQyTALA28490767;     TUTrwhGuUfeVPfQyTALA28490767 = TUTrwhGuUfeVPfQyTALA93477224;     TUTrwhGuUfeVPfQyTALA93477224 = TUTrwhGuUfeVPfQyTALA52650284;     TUTrwhGuUfeVPfQyTALA52650284 = TUTrwhGuUfeVPfQyTALA48389827;     TUTrwhGuUfeVPfQyTALA48389827 = TUTrwhGuUfeVPfQyTALA16481593;     TUTrwhGuUfeVPfQyTALA16481593 = TUTrwhGuUfeVPfQyTALA75080798;     TUTrwhGuUfeVPfQyTALA75080798 = TUTrwhGuUfeVPfQyTALA72053478;     TUTrwhGuUfeVPfQyTALA72053478 = TUTrwhGuUfeVPfQyTALA33678818;     TUTrwhGuUfeVPfQyTALA33678818 = TUTrwhGuUfeVPfQyTALA43331956;     TUTrwhGuUfeVPfQyTALA43331956 = TUTrwhGuUfeVPfQyTALA29150922;     TUTrwhGuUfeVPfQyTALA29150922 = TUTrwhGuUfeVPfQyTALA12713359;     TUTrwhGuUfeVPfQyTALA12713359 = TUTrwhGuUfeVPfQyTALA70723786;     TUTrwhGuUfeVPfQyTALA70723786 = TUTrwhGuUfeVPfQyTALA42448849;     TUTrwhGuUfeVPfQyTALA42448849 = TUTrwhGuUfeVPfQyTALA96179864;     TUTrwhGuUfeVPfQyTALA96179864 = TUTrwhGuUfeVPfQyTALA72489239;     TUTrwhGuUfeVPfQyTALA72489239 = TUTrwhGuUfeVPfQyTALA22548749;     TUTrwhGuUfeVPfQyTALA22548749 = TUTrwhGuUfeVPfQyTALA11001275;     TUTrwhGuUfeVPfQyTALA11001275 = TUTrwhGuUfeVPfQyTALA50330825;     TUTrwhGuUfeVPfQyTALA50330825 = TUTrwhGuUfeVPfQyTALA54015532;     TUTrwhGuUfeVPfQyTALA54015532 = TUTrwhGuUfeVPfQyTALA74020552;     TUTrwhGuUfeVPfQyTALA74020552 = TUTrwhGuUfeVPfQyTALA16382087;     TUTrwhGuUfeVPfQyTALA16382087 = TUTrwhGuUfeVPfQyTALA96810158;     TUTrwhGuUfeVPfQyTALA96810158 = TUTrwhGuUfeVPfQyTALA29958694;     TUTrwhGuUfeVPfQyTALA29958694 = TUTrwhGuUfeVPfQyTALA61208238;     TUTrwhGuUfeVPfQyTALA61208238 = TUTrwhGuUfeVPfQyTALA83985201;     TUTrwhGuUfeVPfQyTALA83985201 = TUTrwhGuUfeVPfQyTALA92337473;     TUTrwhGuUfeVPfQyTALA92337473 = TUTrwhGuUfeVPfQyTALA14122523;     TUTrwhGuUfeVPfQyTALA14122523 = TUTrwhGuUfeVPfQyTALA79661474;     TUTrwhGuUfeVPfQyTALA79661474 = TUTrwhGuUfeVPfQyTALA8496101;     TUTrwhGuUfeVPfQyTALA8496101 = TUTrwhGuUfeVPfQyTALA20973307;     TUTrwhGuUfeVPfQyTALA20973307 = TUTrwhGuUfeVPfQyTALA20773084;     TUTrwhGuUfeVPfQyTALA20773084 = TUTrwhGuUfeVPfQyTALA37789793;     TUTrwhGuUfeVPfQyTALA37789793 = TUTrwhGuUfeVPfQyTALA1742959;     TUTrwhGuUfeVPfQyTALA1742959 = TUTrwhGuUfeVPfQyTALA48045578;     TUTrwhGuUfeVPfQyTALA48045578 = TUTrwhGuUfeVPfQyTALA7853597;     TUTrwhGuUfeVPfQyTALA7853597 = TUTrwhGuUfeVPfQyTALA91442468;     TUTrwhGuUfeVPfQyTALA91442468 = TUTrwhGuUfeVPfQyTALA10589824;     TUTrwhGuUfeVPfQyTALA10589824 = TUTrwhGuUfeVPfQyTALA14062436;     TUTrwhGuUfeVPfQyTALA14062436 = TUTrwhGuUfeVPfQyTALA52411021;     TUTrwhGuUfeVPfQyTALA52411021 = TUTrwhGuUfeVPfQyTALA98060493;     TUTrwhGuUfeVPfQyTALA98060493 = TUTrwhGuUfeVPfQyTALA92777061;     TUTrwhGuUfeVPfQyTALA92777061 = TUTrwhGuUfeVPfQyTALA60033277;     TUTrwhGuUfeVPfQyTALA60033277 = TUTrwhGuUfeVPfQyTALA65205330;     TUTrwhGuUfeVPfQyTALA65205330 = TUTrwhGuUfeVPfQyTALA58413832;     TUTrwhGuUfeVPfQyTALA58413832 = TUTrwhGuUfeVPfQyTALA49246814;     TUTrwhGuUfeVPfQyTALA49246814 = TUTrwhGuUfeVPfQyTALA90011503;     TUTrwhGuUfeVPfQyTALA90011503 = TUTrwhGuUfeVPfQyTALA20533176;     TUTrwhGuUfeVPfQyTALA20533176 = TUTrwhGuUfeVPfQyTALA37844448;     TUTrwhGuUfeVPfQyTALA37844448 = TUTrwhGuUfeVPfQyTALA19475160;     TUTrwhGuUfeVPfQyTALA19475160 = TUTrwhGuUfeVPfQyTALA69212428;     TUTrwhGuUfeVPfQyTALA69212428 = TUTrwhGuUfeVPfQyTALA74821001;     TUTrwhGuUfeVPfQyTALA74821001 = TUTrwhGuUfeVPfQyTALA76445133;     TUTrwhGuUfeVPfQyTALA76445133 = TUTrwhGuUfeVPfQyTALA21210142;     TUTrwhGuUfeVPfQyTALA21210142 = TUTrwhGuUfeVPfQyTALA88281968;     TUTrwhGuUfeVPfQyTALA88281968 = TUTrwhGuUfeVPfQyTALA74145194;     TUTrwhGuUfeVPfQyTALA74145194 = TUTrwhGuUfeVPfQyTALA31680608;     TUTrwhGuUfeVPfQyTALA31680608 = TUTrwhGuUfeVPfQyTALA63518531;     TUTrwhGuUfeVPfQyTALA63518531 = TUTrwhGuUfeVPfQyTALA91442046;     TUTrwhGuUfeVPfQyTALA91442046 = TUTrwhGuUfeVPfQyTALA64404625;     TUTrwhGuUfeVPfQyTALA64404625 = TUTrwhGuUfeVPfQyTALA24144119;     TUTrwhGuUfeVPfQyTALA24144119 = TUTrwhGuUfeVPfQyTALA60958275;     TUTrwhGuUfeVPfQyTALA60958275 = TUTrwhGuUfeVPfQyTALA92392004;     TUTrwhGuUfeVPfQyTALA92392004 = TUTrwhGuUfeVPfQyTALA25182717;     TUTrwhGuUfeVPfQyTALA25182717 = TUTrwhGuUfeVPfQyTALA22358650;     TUTrwhGuUfeVPfQyTALA22358650 = TUTrwhGuUfeVPfQyTALA8377839;     TUTrwhGuUfeVPfQyTALA8377839 = TUTrwhGuUfeVPfQyTALA74923565;     TUTrwhGuUfeVPfQyTALA74923565 = TUTrwhGuUfeVPfQyTALA68980828;     TUTrwhGuUfeVPfQyTALA68980828 = TUTrwhGuUfeVPfQyTALA94403270;     TUTrwhGuUfeVPfQyTALA94403270 = TUTrwhGuUfeVPfQyTALA88326267;     TUTrwhGuUfeVPfQyTALA88326267 = TUTrwhGuUfeVPfQyTALA81046770;     TUTrwhGuUfeVPfQyTALA81046770 = TUTrwhGuUfeVPfQyTALA11958926;     TUTrwhGuUfeVPfQyTALA11958926 = TUTrwhGuUfeVPfQyTALA96938838;     TUTrwhGuUfeVPfQyTALA96938838 = TUTrwhGuUfeVPfQyTALA97919804;     TUTrwhGuUfeVPfQyTALA97919804 = TUTrwhGuUfeVPfQyTALA55955038;     TUTrwhGuUfeVPfQyTALA55955038 = TUTrwhGuUfeVPfQyTALA81243490;     TUTrwhGuUfeVPfQyTALA81243490 = TUTrwhGuUfeVPfQyTALA56348809;     TUTrwhGuUfeVPfQyTALA56348809 = TUTrwhGuUfeVPfQyTALA31604829;     TUTrwhGuUfeVPfQyTALA31604829 = TUTrwhGuUfeVPfQyTALA71544861;     TUTrwhGuUfeVPfQyTALA71544861 = TUTrwhGuUfeVPfQyTALA11961424;     TUTrwhGuUfeVPfQyTALA11961424 = TUTrwhGuUfeVPfQyTALA93973698;     TUTrwhGuUfeVPfQyTALA93973698 = TUTrwhGuUfeVPfQyTALA71804297;     TUTrwhGuUfeVPfQyTALA71804297 = TUTrwhGuUfeVPfQyTALA63240108;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kUtIZWTGqzMOZYRguWpS39335907() {     float gByPBIcekNJZXwoIpohb33891775 = -779090317;    float gByPBIcekNJZXwoIpohb31125780 = -209593846;    float gByPBIcekNJZXwoIpohb48861533 = 89825938;    float gByPBIcekNJZXwoIpohb13072579 = -682463036;    float gByPBIcekNJZXwoIpohb86280862 = -295798896;    float gByPBIcekNJZXwoIpohb83711857 = -519204986;    float gByPBIcekNJZXwoIpohb26916182 = -850704811;    float gByPBIcekNJZXwoIpohb44407135 = -533912865;    float gByPBIcekNJZXwoIpohb7825228 = -255444448;    float gByPBIcekNJZXwoIpohb83701750 = -993932032;    float gByPBIcekNJZXwoIpohb5231186 = -824276381;    float gByPBIcekNJZXwoIpohb79570884 = -769527871;    float gByPBIcekNJZXwoIpohb71791786 = -598601138;    float gByPBIcekNJZXwoIpohb75608210 = -392903812;    float gByPBIcekNJZXwoIpohb68542295 = -45514377;    float gByPBIcekNJZXwoIpohb59304599 = -186627418;    float gByPBIcekNJZXwoIpohb69078499 = -437871641;    float gByPBIcekNJZXwoIpohb82596015 = -438770267;    float gByPBIcekNJZXwoIpohb72235062 = -889632208;    float gByPBIcekNJZXwoIpohb87277475 = -211028875;    float gByPBIcekNJZXwoIpohb48612979 = -621250183;    float gByPBIcekNJZXwoIpohb14046049 = -786585319;    float gByPBIcekNJZXwoIpohb50333635 = -45130858;    float gByPBIcekNJZXwoIpohb50331129 = 37387627;    float gByPBIcekNJZXwoIpohb39205187 = -919200896;    float gByPBIcekNJZXwoIpohb2651060 = -234505828;    float gByPBIcekNJZXwoIpohb41939603 = -677371639;    float gByPBIcekNJZXwoIpohb51114181 = -327700518;    float gByPBIcekNJZXwoIpohb61293410 = 1402010;    float gByPBIcekNJZXwoIpohb51885454 = -457561499;    float gByPBIcekNJZXwoIpohb77795783 = -836723321;    float gByPBIcekNJZXwoIpohb86960309 = -454010312;    float gByPBIcekNJZXwoIpohb79125225 = -172221422;    float gByPBIcekNJZXwoIpohb37854214 = 12208356;    float gByPBIcekNJZXwoIpohb572986 = 84535437;    float gByPBIcekNJZXwoIpohb70994533 = -724963949;    float gByPBIcekNJZXwoIpohb94122311 = -243703724;    float gByPBIcekNJZXwoIpohb6808021 = 54156682;    float gByPBIcekNJZXwoIpohb52553841 = -251179863;    float gByPBIcekNJZXwoIpohb11485385 = -975825306;    float gByPBIcekNJZXwoIpohb56883442 = -773300674;    float gByPBIcekNJZXwoIpohb58441685 = -543857320;    float gByPBIcekNJZXwoIpohb10175198 = -853067990;    float gByPBIcekNJZXwoIpohb90493502 = -487623820;    float gByPBIcekNJZXwoIpohb88291532 = -287723767;    float gByPBIcekNJZXwoIpohb20528357 = -238245853;    float gByPBIcekNJZXwoIpohb5792677 = -349887874;    float gByPBIcekNJZXwoIpohb74110311 = -144749276;    float gByPBIcekNJZXwoIpohb26414759 = -96321198;    float gByPBIcekNJZXwoIpohb29200570 = 53471856;    float gByPBIcekNJZXwoIpohb5979118 = -670232499;    float gByPBIcekNJZXwoIpohb43614464 = -732622178;    float gByPBIcekNJZXwoIpohb3121236 = -513778118;    float gByPBIcekNJZXwoIpohb98080901 = -858508242;    float gByPBIcekNJZXwoIpohb90722927 = -608522602;    float gByPBIcekNJZXwoIpohb19845726 = -992504998;    float gByPBIcekNJZXwoIpohb80792144 = -64462988;    float gByPBIcekNJZXwoIpohb98530403 = -947561690;    float gByPBIcekNJZXwoIpohb73867392 = -763262141;    float gByPBIcekNJZXwoIpohb83629802 = 38706932;    float gByPBIcekNJZXwoIpohb41772254 = -841833348;    float gByPBIcekNJZXwoIpohb75802000 = -423004293;    float gByPBIcekNJZXwoIpohb83113725 = -435314875;    float gByPBIcekNJZXwoIpohb55939773 = -797882949;    float gByPBIcekNJZXwoIpohb5905968 = -57208712;    float gByPBIcekNJZXwoIpohb18270876 = -270266069;    float gByPBIcekNJZXwoIpohb445659 = -497306449;    float gByPBIcekNJZXwoIpohb33937572 = -510809494;    float gByPBIcekNJZXwoIpohb75035225 = -377439250;    float gByPBIcekNJZXwoIpohb97547762 = -320550428;    float gByPBIcekNJZXwoIpohb65182287 = -942923695;    float gByPBIcekNJZXwoIpohb62270479 = -392028323;    float gByPBIcekNJZXwoIpohb30042174 = -87590405;    float gByPBIcekNJZXwoIpohb60749677 = -913806903;    float gByPBIcekNJZXwoIpohb30394033 = -437728202;    float gByPBIcekNJZXwoIpohb90171294 = 22607137;    float gByPBIcekNJZXwoIpohb3870852 = -933517330;    float gByPBIcekNJZXwoIpohb59840133 = -557507039;    float gByPBIcekNJZXwoIpohb62039597 = -674888606;    float gByPBIcekNJZXwoIpohb18676830 = -580955044;    float gByPBIcekNJZXwoIpohb96858382 = -884617955;    float gByPBIcekNJZXwoIpohb67829292 = -432622363;    float gByPBIcekNJZXwoIpohb24699423 = -131379321;    float gByPBIcekNJZXwoIpohb32092840 = 47930154;    float gByPBIcekNJZXwoIpohb45906337 = -787329001;    float gByPBIcekNJZXwoIpohb34181320 = -4101143;    float gByPBIcekNJZXwoIpohb83839074 = -940232195;    float gByPBIcekNJZXwoIpohb81044324 = -313713181;    float gByPBIcekNJZXwoIpohb47131286 = -379269043;    float gByPBIcekNJZXwoIpohb80727259 = 77040435;    float gByPBIcekNJZXwoIpohb90202388 = -560500961;    float gByPBIcekNJZXwoIpohb95591908 = -296142035;    float gByPBIcekNJZXwoIpohb32940629 = -182581178;    float gByPBIcekNJZXwoIpohb68924039 = -189886795;    float gByPBIcekNJZXwoIpohb69713131 = -33991958;    float gByPBIcekNJZXwoIpohb81081442 = -250296381;    float gByPBIcekNJZXwoIpohb75327959 = -8542445;    float gByPBIcekNJZXwoIpohb54235425 = 44814959;    float gByPBIcekNJZXwoIpohb84587535 = -330415108;    float gByPBIcekNJZXwoIpohb70020656 = -779090317;     gByPBIcekNJZXwoIpohb33891775 = gByPBIcekNJZXwoIpohb31125780;     gByPBIcekNJZXwoIpohb31125780 = gByPBIcekNJZXwoIpohb48861533;     gByPBIcekNJZXwoIpohb48861533 = gByPBIcekNJZXwoIpohb13072579;     gByPBIcekNJZXwoIpohb13072579 = gByPBIcekNJZXwoIpohb86280862;     gByPBIcekNJZXwoIpohb86280862 = gByPBIcekNJZXwoIpohb83711857;     gByPBIcekNJZXwoIpohb83711857 = gByPBIcekNJZXwoIpohb26916182;     gByPBIcekNJZXwoIpohb26916182 = gByPBIcekNJZXwoIpohb44407135;     gByPBIcekNJZXwoIpohb44407135 = gByPBIcekNJZXwoIpohb7825228;     gByPBIcekNJZXwoIpohb7825228 = gByPBIcekNJZXwoIpohb83701750;     gByPBIcekNJZXwoIpohb83701750 = gByPBIcekNJZXwoIpohb5231186;     gByPBIcekNJZXwoIpohb5231186 = gByPBIcekNJZXwoIpohb79570884;     gByPBIcekNJZXwoIpohb79570884 = gByPBIcekNJZXwoIpohb71791786;     gByPBIcekNJZXwoIpohb71791786 = gByPBIcekNJZXwoIpohb75608210;     gByPBIcekNJZXwoIpohb75608210 = gByPBIcekNJZXwoIpohb68542295;     gByPBIcekNJZXwoIpohb68542295 = gByPBIcekNJZXwoIpohb59304599;     gByPBIcekNJZXwoIpohb59304599 = gByPBIcekNJZXwoIpohb69078499;     gByPBIcekNJZXwoIpohb69078499 = gByPBIcekNJZXwoIpohb82596015;     gByPBIcekNJZXwoIpohb82596015 = gByPBIcekNJZXwoIpohb72235062;     gByPBIcekNJZXwoIpohb72235062 = gByPBIcekNJZXwoIpohb87277475;     gByPBIcekNJZXwoIpohb87277475 = gByPBIcekNJZXwoIpohb48612979;     gByPBIcekNJZXwoIpohb48612979 = gByPBIcekNJZXwoIpohb14046049;     gByPBIcekNJZXwoIpohb14046049 = gByPBIcekNJZXwoIpohb50333635;     gByPBIcekNJZXwoIpohb50333635 = gByPBIcekNJZXwoIpohb50331129;     gByPBIcekNJZXwoIpohb50331129 = gByPBIcekNJZXwoIpohb39205187;     gByPBIcekNJZXwoIpohb39205187 = gByPBIcekNJZXwoIpohb2651060;     gByPBIcekNJZXwoIpohb2651060 = gByPBIcekNJZXwoIpohb41939603;     gByPBIcekNJZXwoIpohb41939603 = gByPBIcekNJZXwoIpohb51114181;     gByPBIcekNJZXwoIpohb51114181 = gByPBIcekNJZXwoIpohb61293410;     gByPBIcekNJZXwoIpohb61293410 = gByPBIcekNJZXwoIpohb51885454;     gByPBIcekNJZXwoIpohb51885454 = gByPBIcekNJZXwoIpohb77795783;     gByPBIcekNJZXwoIpohb77795783 = gByPBIcekNJZXwoIpohb86960309;     gByPBIcekNJZXwoIpohb86960309 = gByPBIcekNJZXwoIpohb79125225;     gByPBIcekNJZXwoIpohb79125225 = gByPBIcekNJZXwoIpohb37854214;     gByPBIcekNJZXwoIpohb37854214 = gByPBIcekNJZXwoIpohb572986;     gByPBIcekNJZXwoIpohb572986 = gByPBIcekNJZXwoIpohb70994533;     gByPBIcekNJZXwoIpohb70994533 = gByPBIcekNJZXwoIpohb94122311;     gByPBIcekNJZXwoIpohb94122311 = gByPBIcekNJZXwoIpohb6808021;     gByPBIcekNJZXwoIpohb6808021 = gByPBIcekNJZXwoIpohb52553841;     gByPBIcekNJZXwoIpohb52553841 = gByPBIcekNJZXwoIpohb11485385;     gByPBIcekNJZXwoIpohb11485385 = gByPBIcekNJZXwoIpohb56883442;     gByPBIcekNJZXwoIpohb56883442 = gByPBIcekNJZXwoIpohb58441685;     gByPBIcekNJZXwoIpohb58441685 = gByPBIcekNJZXwoIpohb10175198;     gByPBIcekNJZXwoIpohb10175198 = gByPBIcekNJZXwoIpohb90493502;     gByPBIcekNJZXwoIpohb90493502 = gByPBIcekNJZXwoIpohb88291532;     gByPBIcekNJZXwoIpohb88291532 = gByPBIcekNJZXwoIpohb20528357;     gByPBIcekNJZXwoIpohb20528357 = gByPBIcekNJZXwoIpohb5792677;     gByPBIcekNJZXwoIpohb5792677 = gByPBIcekNJZXwoIpohb74110311;     gByPBIcekNJZXwoIpohb74110311 = gByPBIcekNJZXwoIpohb26414759;     gByPBIcekNJZXwoIpohb26414759 = gByPBIcekNJZXwoIpohb29200570;     gByPBIcekNJZXwoIpohb29200570 = gByPBIcekNJZXwoIpohb5979118;     gByPBIcekNJZXwoIpohb5979118 = gByPBIcekNJZXwoIpohb43614464;     gByPBIcekNJZXwoIpohb43614464 = gByPBIcekNJZXwoIpohb3121236;     gByPBIcekNJZXwoIpohb3121236 = gByPBIcekNJZXwoIpohb98080901;     gByPBIcekNJZXwoIpohb98080901 = gByPBIcekNJZXwoIpohb90722927;     gByPBIcekNJZXwoIpohb90722927 = gByPBIcekNJZXwoIpohb19845726;     gByPBIcekNJZXwoIpohb19845726 = gByPBIcekNJZXwoIpohb80792144;     gByPBIcekNJZXwoIpohb80792144 = gByPBIcekNJZXwoIpohb98530403;     gByPBIcekNJZXwoIpohb98530403 = gByPBIcekNJZXwoIpohb73867392;     gByPBIcekNJZXwoIpohb73867392 = gByPBIcekNJZXwoIpohb83629802;     gByPBIcekNJZXwoIpohb83629802 = gByPBIcekNJZXwoIpohb41772254;     gByPBIcekNJZXwoIpohb41772254 = gByPBIcekNJZXwoIpohb75802000;     gByPBIcekNJZXwoIpohb75802000 = gByPBIcekNJZXwoIpohb83113725;     gByPBIcekNJZXwoIpohb83113725 = gByPBIcekNJZXwoIpohb55939773;     gByPBIcekNJZXwoIpohb55939773 = gByPBIcekNJZXwoIpohb5905968;     gByPBIcekNJZXwoIpohb5905968 = gByPBIcekNJZXwoIpohb18270876;     gByPBIcekNJZXwoIpohb18270876 = gByPBIcekNJZXwoIpohb445659;     gByPBIcekNJZXwoIpohb445659 = gByPBIcekNJZXwoIpohb33937572;     gByPBIcekNJZXwoIpohb33937572 = gByPBIcekNJZXwoIpohb75035225;     gByPBIcekNJZXwoIpohb75035225 = gByPBIcekNJZXwoIpohb97547762;     gByPBIcekNJZXwoIpohb97547762 = gByPBIcekNJZXwoIpohb65182287;     gByPBIcekNJZXwoIpohb65182287 = gByPBIcekNJZXwoIpohb62270479;     gByPBIcekNJZXwoIpohb62270479 = gByPBIcekNJZXwoIpohb30042174;     gByPBIcekNJZXwoIpohb30042174 = gByPBIcekNJZXwoIpohb60749677;     gByPBIcekNJZXwoIpohb60749677 = gByPBIcekNJZXwoIpohb30394033;     gByPBIcekNJZXwoIpohb30394033 = gByPBIcekNJZXwoIpohb90171294;     gByPBIcekNJZXwoIpohb90171294 = gByPBIcekNJZXwoIpohb3870852;     gByPBIcekNJZXwoIpohb3870852 = gByPBIcekNJZXwoIpohb59840133;     gByPBIcekNJZXwoIpohb59840133 = gByPBIcekNJZXwoIpohb62039597;     gByPBIcekNJZXwoIpohb62039597 = gByPBIcekNJZXwoIpohb18676830;     gByPBIcekNJZXwoIpohb18676830 = gByPBIcekNJZXwoIpohb96858382;     gByPBIcekNJZXwoIpohb96858382 = gByPBIcekNJZXwoIpohb67829292;     gByPBIcekNJZXwoIpohb67829292 = gByPBIcekNJZXwoIpohb24699423;     gByPBIcekNJZXwoIpohb24699423 = gByPBIcekNJZXwoIpohb32092840;     gByPBIcekNJZXwoIpohb32092840 = gByPBIcekNJZXwoIpohb45906337;     gByPBIcekNJZXwoIpohb45906337 = gByPBIcekNJZXwoIpohb34181320;     gByPBIcekNJZXwoIpohb34181320 = gByPBIcekNJZXwoIpohb83839074;     gByPBIcekNJZXwoIpohb83839074 = gByPBIcekNJZXwoIpohb81044324;     gByPBIcekNJZXwoIpohb81044324 = gByPBIcekNJZXwoIpohb47131286;     gByPBIcekNJZXwoIpohb47131286 = gByPBIcekNJZXwoIpohb80727259;     gByPBIcekNJZXwoIpohb80727259 = gByPBIcekNJZXwoIpohb90202388;     gByPBIcekNJZXwoIpohb90202388 = gByPBIcekNJZXwoIpohb95591908;     gByPBIcekNJZXwoIpohb95591908 = gByPBIcekNJZXwoIpohb32940629;     gByPBIcekNJZXwoIpohb32940629 = gByPBIcekNJZXwoIpohb68924039;     gByPBIcekNJZXwoIpohb68924039 = gByPBIcekNJZXwoIpohb69713131;     gByPBIcekNJZXwoIpohb69713131 = gByPBIcekNJZXwoIpohb81081442;     gByPBIcekNJZXwoIpohb81081442 = gByPBIcekNJZXwoIpohb75327959;     gByPBIcekNJZXwoIpohb75327959 = gByPBIcekNJZXwoIpohb54235425;     gByPBIcekNJZXwoIpohb54235425 = gByPBIcekNJZXwoIpohb84587535;     gByPBIcekNJZXwoIpohb84587535 = gByPBIcekNJZXwoIpohb70020656;     gByPBIcekNJZXwoIpohb70020656 = gByPBIcekNJZXwoIpohb33891775;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MyIPkXkDfCClDlbigbmW45994651() {     float jRrmoWfMEMYRMjhDeRtC33885304 = -651614458;    float jRrmoWfMEMYRMjhDeRtC69698096 = -252188971;    float jRrmoWfMEMYRMjhDeRtC50174745 = -967302676;    float jRrmoWfMEMYRMjhDeRtC98152438 = -503003866;    float jRrmoWfMEMYRMjhDeRtC95120964 = -764154217;    float jRrmoWfMEMYRMjhDeRtC18517290 = -218637798;    float jRrmoWfMEMYRMjhDeRtC95463517 = -555241278;    float jRrmoWfMEMYRMjhDeRtC13621186 = -238090182;    float jRrmoWfMEMYRMjhDeRtC90565391 = 30909482;    float jRrmoWfMEMYRMjhDeRtC80341264 = -630738903;    float jRrmoWfMEMYRMjhDeRtC25551791 = -783748453;    float jRrmoWfMEMYRMjhDeRtC93862561 = -505006233;    float jRrmoWfMEMYRMjhDeRtC3644700 = -620259613;    float jRrmoWfMEMYRMjhDeRtC29452431 = -738056864;    float jRrmoWfMEMYRMjhDeRtC13714405 = -8227229;    float jRrmoWfMEMYRMjhDeRtC33869146 = -519423069;    float jRrmoWfMEMYRMjhDeRtC24931259 = -257486211;    float jRrmoWfMEMYRMjhDeRtC5596548 = -835841889;    float jRrmoWfMEMYRMjhDeRtC92663332 = -407614474;    float jRrmoWfMEMYRMjhDeRtC59351238 = -823053327;    float jRrmoWfMEMYRMjhDeRtC28282403 = -331716556;    float jRrmoWfMEMYRMjhDeRtC92342315 = -196577203;    float jRrmoWfMEMYRMjhDeRtC87212031 = -504088700;    float jRrmoWfMEMYRMjhDeRtC93981237 = -943532895;    float jRrmoWfMEMYRMjhDeRtC67647876 = -245894749;    float jRrmoWfMEMYRMjhDeRtC19225137 = -142711757;    float jRrmoWfMEMYRMjhDeRtC12161185 = -978171352;    float jRrmoWfMEMYRMjhDeRtC86517706 = -936667507;    float jRrmoWfMEMYRMjhDeRtC73971856 = -644668305;    float jRrmoWfMEMYRMjhDeRtC27975324 = -132992873;    float jRrmoWfMEMYRMjhDeRtC11208462 = -123816555;    float jRrmoWfMEMYRMjhDeRtC71684166 = -845832074;    float jRrmoWfMEMYRMjhDeRtC64577648 = -374165638;    float jRrmoWfMEMYRMjhDeRtC38137051 = -964473377;    float jRrmoWfMEMYRMjhDeRtC82931023 = -174778487;    float jRrmoWfMEMYRMjhDeRtC13822724 = -804729695;    float jRrmoWfMEMYRMjhDeRtC6063679 = -343357102;    float jRrmoWfMEMYRMjhDeRtC76725699 = 58894003;    float jRrmoWfMEMYRMjhDeRtC87867438 = -963861279;    float jRrmoWfMEMYRMjhDeRtC83358278 = -585256992;    float jRrmoWfMEMYRMjhDeRtC44739110 = -861244572;    float jRrmoWfMEMYRMjhDeRtC44139346 = -737649402;    float jRrmoWfMEMYRMjhDeRtC70458558 = -312198244;    float jRrmoWfMEMYRMjhDeRtC49725401 = -44588263;    float jRrmoWfMEMYRMjhDeRtC72309584 = -760983169;    float jRrmoWfMEMYRMjhDeRtC60956325 = -50290524;    float jRrmoWfMEMYRMjhDeRtC33490470 = -831168496;    float jRrmoWfMEMYRMjhDeRtC49493892 = -753510633;    float jRrmoWfMEMYRMjhDeRtC45073148 = -760085059;    float jRrmoWfMEMYRMjhDeRtC49245993 = -633553030;    float jRrmoWfMEMYRMjhDeRtC19238487 = -248787506;    float jRrmoWfMEMYRMjhDeRtC58161721 = -14767927;    float jRrmoWfMEMYRMjhDeRtC78569583 = -405832126;    float jRrmoWfMEMYRMjhDeRtC8331173 = -290428093;    float jRrmoWfMEMYRMjhDeRtC17712248 = -365421880;    float jRrmoWfMEMYRMjhDeRtC41542989 = -355037256;    float jRrmoWfMEMYRMjhDeRtC82486064 = -748100272;    float jRrmoWfMEMYRMjhDeRtC56193507 = 76230219;    float jRrmoWfMEMYRMjhDeRtC30504563 = -157109118;    float jRrmoWfMEMYRMjhDeRtC75895828 = -521442461;    float jRrmoWfMEMYRMjhDeRtC6356105 = -240466447;    float jRrmoWfMEMYRMjhDeRtC8945812 = -618573771;    float jRrmoWfMEMYRMjhDeRtC39649329 = -593421877;    float jRrmoWfMEMYRMjhDeRtC62590067 = -836097645;    float jRrmoWfMEMYRMjhDeRtC69132802 = -406922349;    float jRrmoWfMEMYRMjhDeRtC53867624 = -937916380;    float jRrmoWfMEMYRMjhDeRtC29284913 = -30840595;    float jRrmoWfMEMYRMjhDeRtC65507648 = -655786236;    float jRrmoWfMEMYRMjhDeRtC46521408 = -463278377;    float jRrmoWfMEMYRMjhDeRtC99891681 = -203497534;    float jRrmoWfMEMYRMjhDeRtC27805467 = -76065967;    float jRrmoWfMEMYRMjhDeRtC48205559 = -216380214;    float jRrmoWfMEMYRMjhDeRtC17729110 = -871980610;    float jRrmoWfMEMYRMjhDeRtC9305055 = -822357483;    float jRrmoWfMEMYRMjhDeRtC14612129 = -961808755;    float jRrmoWfMEMYRMjhDeRtC84143057 = -594067155;    float jRrmoWfMEMYRMjhDeRtC21883757 = -884378959;    float jRrmoWfMEMYRMjhDeRtC37486631 = -359500438;    float jRrmoWfMEMYRMjhDeRtC21671654 = -82549727;    float jRrmoWfMEMYRMjhDeRtC6691551 = -95604226;    float jRrmoWfMEMYRMjhDeRtC85734666 = -311543261;    float jRrmoWfMEMYRMjhDeRtC62667293 = -124660720;    float jRrmoWfMEMYRMjhDeRtC41444558 = -76582448;    float jRrmoWfMEMYRMjhDeRtC24725864 = 88884725;    float jRrmoWfMEMYRMjhDeRtC8736838 = -884205367;    float jRrmoWfMEMYRMjhDeRtC53046741 = -9048629;    float jRrmoWfMEMYRMjhDeRtC93114583 = -339999948;    float jRrmoWfMEMYRMjhDeRtC56246476 = 16262454;    float jRrmoWfMEMYRMjhDeRtC20424803 = -499051498;    float jRrmoWfMEMYRMjhDeRtC41388035 = -819741232;    float jRrmoWfMEMYRMjhDeRtC31336659 = 43370576;    float jRrmoWfMEMYRMjhDeRtC49870172 = -319587322;    float jRrmoWfMEMYRMjhDeRtC46221137 = -783996880;    float jRrmoWfMEMYRMjhDeRtC11971611 = -342418818;    float jRrmoWfMEMYRMjhDeRtC77002173 = -244790546;    float jRrmoWfMEMYRMjhDeRtC35793299 = -142670801;    float jRrmoWfMEMYRMjhDeRtC4490017 = -44227525;    float jRrmoWfMEMYRMjhDeRtC7868491 = -476100599;    float jRrmoWfMEMYRMjhDeRtC80592598 = -637665915;    float jRrmoWfMEMYRMjhDeRtC18441961 = -651614458;     jRrmoWfMEMYRMjhDeRtC33885304 = jRrmoWfMEMYRMjhDeRtC69698096;     jRrmoWfMEMYRMjhDeRtC69698096 = jRrmoWfMEMYRMjhDeRtC50174745;     jRrmoWfMEMYRMjhDeRtC50174745 = jRrmoWfMEMYRMjhDeRtC98152438;     jRrmoWfMEMYRMjhDeRtC98152438 = jRrmoWfMEMYRMjhDeRtC95120964;     jRrmoWfMEMYRMjhDeRtC95120964 = jRrmoWfMEMYRMjhDeRtC18517290;     jRrmoWfMEMYRMjhDeRtC18517290 = jRrmoWfMEMYRMjhDeRtC95463517;     jRrmoWfMEMYRMjhDeRtC95463517 = jRrmoWfMEMYRMjhDeRtC13621186;     jRrmoWfMEMYRMjhDeRtC13621186 = jRrmoWfMEMYRMjhDeRtC90565391;     jRrmoWfMEMYRMjhDeRtC90565391 = jRrmoWfMEMYRMjhDeRtC80341264;     jRrmoWfMEMYRMjhDeRtC80341264 = jRrmoWfMEMYRMjhDeRtC25551791;     jRrmoWfMEMYRMjhDeRtC25551791 = jRrmoWfMEMYRMjhDeRtC93862561;     jRrmoWfMEMYRMjhDeRtC93862561 = jRrmoWfMEMYRMjhDeRtC3644700;     jRrmoWfMEMYRMjhDeRtC3644700 = jRrmoWfMEMYRMjhDeRtC29452431;     jRrmoWfMEMYRMjhDeRtC29452431 = jRrmoWfMEMYRMjhDeRtC13714405;     jRrmoWfMEMYRMjhDeRtC13714405 = jRrmoWfMEMYRMjhDeRtC33869146;     jRrmoWfMEMYRMjhDeRtC33869146 = jRrmoWfMEMYRMjhDeRtC24931259;     jRrmoWfMEMYRMjhDeRtC24931259 = jRrmoWfMEMYRMjhDeRtC5596548;     jRrmoWfMEMYRMjhDeRtC5596548 = jRrmoWfMEMYRMjhDeRtC92663332;     jRrmoWfMEMYRMjhDeRtC92663332 = jRrmoWfMEMYRMjhDeRtC59351238;     jRrmoWfMEMYRMjhDeRtC59351238 = jRrmoWfMEMYRMjhDeRtC28282403;     jRrmoWfMEMYRMjhDeRtC28282403 = jRrmoWfMEMYRMjhDeRtC92342315;     jRrmoWfMEMYRMjhDeRtC92342315 = jRrmoWfMEMYRMjhDeRtC87212031;     jRrmoWfMEMYRMjhDeRtC87212031 = jRrmoWfMEMYRMjhDeRtC93981237;     jRrmoWfMEMYRMjhDeRtC93981237 = jRrmoWfMEMYRMjhDeRtC67647876;     jRrmoWfMEMYRMjhDeRtC67647876 = jRrmoWfMEMYRMjhDeRtC19225137;     jRrmoWfMEMYRMjhDeRtC19225137 = jRrmoWfMEMYRMjhDeRtC12161185;     jRrmoWfMEMYRMjhDeRtC12161185 = jRrmoWfMEMYRMjhDeRtC86517706;     jRrmoWfMEMYRMjhDeRtC86517706 = jRrmoWfMEMYRMjhDeRtC73971856;     jRrmoWfMEMYRMjhDeRtC73971856 = jRrmoWfMEMYRMjhDeRtC27975324;     jRrmoWfMEMYRMjhDeRtC27975324 = jRrmoWfMEMYRMjhDeRtC11208462;     jRrmoWfMEMYRMjhDeRtC11208462 = jRrmoWfMEMYRMjhDeRtC71684166;     jRrmoWfMEMYRMjhDeRtC71684166 = jRrmoWfMEMYRMjhDeRtC64577648;     jRrmoWfMEMYRMjhDeRtC64577648 = jRrmoWfMEMYRMjhDeRtC38137051;     jRrmoWfMEMYRMjhDeRtC38137051 = jRrmoWfMEMYRMjhDeRtC82931023;     jRrmoWfMEMYRMjhDeRtC82931023 = jRrmoWfMEMYRMjhDeRtC13822724;     jRrmoWfMEMYRMjhDeRtC13822724 = jRrmoWfMEMYRMjhDeRtC6063679;     jRrmoWfMEMYRMjhDeRtC6063679 = jRrmoWfMEMYRMjhDeRtC76725699;     jRrmoWfMEMYRMjhDeRtC76725699 = jRrmoWfMEMYRMjhDeRtC87867438;     jRrmoWfMEMYRMjhDeRtC87867438 = jRrmoWfMEMYRMjhDeRtC83358278;     jRrmoWfMEMYRMjhDeRtC83358278 = jRrmoWfMEMYRMjhDeRtC44739110;     jRrmoWfMEMYRMjhDeRtC44739110 = jRrmoWfMEMYRMjhDeRtC44139346;     jRrmoWfMEMYRMjhDeRtC44139346 = jRrmoWfMEMYRMjhDeRtC70458558;     jRrmoWfMEMYRMjhDeRtC70458558 = jRrmoWfMEMYRMjhDeRtC49725401;     jRrmoWfMEMYRMjhDeRtC49725401 = jRrmoWfMEMYRMjhDeRtC72309584;     jRrmoWfMEMYRMjhDeRtC72309584 = jRrmoWfMEMYRMjhDeRtC60956325;     jRrmoWfMEMYRMjhDeRtC60956325 = jRrmoWfMEMYRMjhDeRtC33490470;     jRrmoWfMEMYRMjhDeRtC33490470 = jRrmoWfMEMYRMjhDeRtC49493892;     jRrmoWfMEMYRMjhDeRtC49493892 = jRrmoWfMEMYRMjhDeRtC45073148;     jRrmoWfMEMYRMjhDeRtC45073148 = jRrmoWfMEMYRMjhDeRtC49245993;     jRrmoWfMEMYRMjhDeRtC49245993 = jRrmoWfMEMYRMjhDeRtC19238487;     jRrmoWfMEMYRMjhDeRtC19238487 = jRrmoWfMEMYRMjhDeRtC58161721;     jRrmoWfMEMYRMjhDeRtC58161721 = jRrmoWfMEMYRMjhDeRtC78569583;     jRrmoWfMEMYRMjhDeRtC78569583 = jRrmoWfMEMYRMjhDeRtC8331173;     jRrmoWfMEMYRMjhDeRtC8331173 = jRrmoWfMEMYRMjhDeRtC17712248;     jRrmoWfMEMYRMjhDeRtC17712248 = jRrmoWfMEMYRMjhDeRtC41542989;     jRrmoWfMEMYRMjhDeRtC41542989 = jRrmoWfMEMYRMjhDeRtC82486064;     jRrmoWfMEMYRMjhDeRtC82486064 = jRrmoWfMEMYRMjhDeRtC56193507;     jRrmoWfMEMYRMjhDeRtC56193507 = jRrmoWfMEMYRMjhDeRtC30504563;     jRrmoWfMEMYRMjhDeRtC30504563 = jRrmoWfMEMYRMjhDeRtC75895828;     jRrmoWfMEMYRMjhDeRtC75895828 = jRrmoWfMEMYRMjhDeRtC6356105;     jRrmoWfMEMYRMjhDeRtC6356105 = jRrmoWfMEMYRMjhDeRtC8945812;     jRrmoWfMEMYRMjhDeRtC8945812 = jRrmoWfMEMYRMjhDeRtC39649329;     jRrmoWfMEMYRMjhDeRtC39649329 = jRrmoWfMEMYRMjhDeRtC62590067;     jRrmoWfMEMYRMjhDeRtC62590067 = jRrmoWfMEMYRMjhDeRtC69132802;     jRrmoWfMEMYRMjhDeRtC69132802 = jRrmoWfMEMYRMjhDeRtC53867624;     jRrmoWfMEMYRMjhDeRtC53867624 = jRrmoWfMEMYRMjhDeRtC29284913;     jRrmoWfMEMYRMjhDeRtC29284913 = jRrmoWfMEMYRMjhDeRtC65507648;     jRrmoWfMEMYRMjhDeRtC65507648 = jRrmoWfMEMYRMjhDeRtC46521408;     jRrmoWfMEMYRMjhDeRtC46521408 = jRrmoWfMEMYRMjhDeRtC99891681;     jRrmoWfMEMYRMjhDeRtC99891681 = jRrmoWfMEMYRMjhDeRtC27805467;     jRrmoWfMEMYRMjhDeRtC27805467 = jRrmoWfMEMYRMjhDeRtC48205559;     jRrmoWfMEMYRMjhDeRtC48205559 = jRrmoWfMEMYRMjhDeRtC17729110;     jRrmoWfMEMYRMjhDeRtC17729110 = jRrmoWfMEMYRMjhDeRtC9305055;     jRrmoWfMEMYRMjhDeRtC9305055 = jRrmoWfMEMYRMjhDeRtC14612129;     jRrmoWfMEMYRMjhDeRtC14612129 = jRrmoWfMEMYRMjhDeRtC84143057;     jRrmoWfMEMYRMjhDeRtC84143057 = jRrmoWfMEMYRMjhDeRtC21883757;     jRrmoWfMEMYRMjhDeRtC21883757 = jRrmoWfMEMYRMjhDeRtC37486631;     jRrmoWfMEMYRMjhDeRtC37486631 = jRrmoWfMEMYRMjhDeRtC21671654;     jRrmoWfMEMYRMjhDeRtC21671654 = jRrmoWfMEMYRMjhDeRtC6691551;     jRrmoWfMEMYRMjhDeRtC6691551 = jRrmoWfMEMYRMjhDeRtC85734666;     jRrmoWfMEMYRMjhDeRtC85734666 = jRrmoWfMEMYRMjhDeRtC62667293;     jRrmoWfMEMYRMjhDeRtC62667293 = jRrmoWfMEMYRMjhDeRtC41444558;     jRrmoWfMEMYRMjhDeRtC41444558 = jRrmoWfMEMYRMjhDeRtC24725864;     jRrmoWfMEMYRMjhDeRtC24725864 = jRrmoWfMEMYRMjhDeRtC8736838;     jRrmoWfMEMYRMjhDeRtC8736838 = jRrmoWfMEMYRMjhDeRtC53046741;     jRrmoWfMEMYRMjhDeRtC53046741 = jRrmoWfMEMYRMjhDeRtC93114583;     jRrmoWfMEMYRMjhDeRtC93114583 = jRrmoWfMEMYRMjhDeRtC56246476;     jRrmoWfMEMYRMjhDeRtC56246476 = jRrmoWfMEMYRMjhDeRtC20424803;     jRrmoWfMEMYRMjhDeRtC20424803 = jRrmoWfMEMYRMjhDeRtC41388035;     jRrmoWfMEMYRMjhDeRtC41388035 = jRrmoWfMEMYRMjhDeRtC31336659;     jRrmoWfMEMYRMjhDeRtC31336659 = jRrmoWfMEMYRMjhDeRtC49870172;     jRrmoWfMEMYRMjhDeRtC49870172 = jRrmoWfMEMYRMjhDeRtC46221137;     jRrmoWfMEMYRMjhDeRtC46221137 = jRrmoWfMEMYRMjhDeRtC11971611;     jRrmoWfMEMYRMjhDeRtC11971611 = jRrmoWfMEMYRMjhDeRtC77002173;     jRrmoWfMEMYRMjhDeRtC77002173 = jRrmoWfMEMYRMjhDeRtC35793299;     jRrmoWfMEMYRMjhDeRtC35793299 = jRrmoWfMEMYRMjhDeRtC4490017;     jRrmoWfMEMYRMjhDeRtC4490017 = jRrmoWfMEMYRMjhDeRtC7868491;     jRrmoWfMEMYRMjhDeRtC7868491 = jRrmoWfMEMYRMjhDeRtC80592598;     jRrmoWfMEMYRMjhDeRtC80592598 = jRrmoWfMEMYRMjhDeRtC18441961;     jRrmoWfMEMYRMjhDeRtC18441961 = jRrmoWfMEMYRMjhDeRtC33885304;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void PpsqrXbwKaljqWGpZCCk410865() {     float KHbAMpYyihHDvABxnruO4536971 = -169763588;    float KHbAMpYyihHDvABxnruO38371614 = -123832271;    float KHbAMpYyihHDvABxnruO30143664 = -309597136;    float KHbAMpYyihHDvABxnruO38083726 = -693142255;    float KHbAMpYyihHDvABxnruO16571286 = -547560565;    float KHbAMpYyihHDvABxnruO8517053 = 79781051;    float KHbAMpYyihHDvABxnruO13842414 = -554016987;    float KHbAMpYyihHDvABxnruO70463566 = -584907272;    float KHbAMpYyihHDvABxnruO36430446 = -62437846;    float KHbAMpYyihHDvABxnruO3307725 = -931834254;    float KHbAMpYyihHDvABxnruO67800951 = -818650382;    float KHbAMpYyihHDvABxnruO39409132 = -820770026;    float KHbAMpYyihHDvABxnruO83472087 = -529796671;    float KHbAMpYyihHDvABxnruO13299465 = -539085074;    float KHbAMpYyihHDvABxnruO96434423 = -157977232;    float KHbAMpYyihHDvABxnruO66397786 = -89406655;    float KHbAMpYyihHDvABxnruO18784086 = -166671341;    float KHbAMpYyihHDvABxnruO25890042 = -825008085;    float KHbAMpYyihHDvABxnruO74371114 = -14802416;    float KHbAMpYyihHDvABxnruO18137948 = -265419631;    float KHbAMpYyihHDvABxnruO83418157 = -240395697;    float KHbAMpYyihHDvABxnruO53738080 = -527862508;    float KHbAMpYyihHDvABxnruO89155840 = 28068155;    float KHbAMpYyihHDvABxnruO27830775 = -535933801;    float KHbAMpYyihHDvABxnruO31772265 = 78200563;    float KHbAMpYyihHDvABxnruO49822717 = -240996202;    float KHbAMpYyihHDvABxnruO20421971 = -273349127;    float KHbAMpYyihHDvABxnruO94299931 = -341791274;    float KHbAMpYyihHDvABxnruO6114345 = -939026522;    float KHbAMpYyihHDvABxnruO67147420 = -691387943;    float KHbAMpYyihHDvABxnruO18280459 = -972250241;    float KHbAMpYyihHDvABxnruO16195628 = -74129237;    float KHbAMpYyihHDvABxnruO47523009 = -321127247;    float KHbAMpYyihHDvABxnruO3502027 = -528003061;    float KHbAMpYyihHDvABxnruO60955259 = -21725267;    float KHbAMpYyihHDvABxnruO73815982 = -883956243;    float KHbAMpYyihHDvABxnruO49855165 = -531950310;    float KHbAMpYyihHDvABxnruO29518189 = -825219105;    float KHbAMpYyihHDvABxnruO66400727 = -919589916;    float KHbAMpYyihHDvABxnruO78461576 = -102865354;    float KHbAMpYyihHDvABxnruO4812394 = -145263450;    float KHbAMpYyihHDvABxnruO72622336 = -507038472;    float KHbAMpYyihHDvABxnruO19425518 = -298952296;    float KHbAMpYyihHDvABxnruO56233701 = -116563235;    float KHbAMpYyihHDvABxnruO68263643 = 30370437;    float KHbAMpYyihHDvABxnruO67362160 = -908627678;    float KHbAMpYyihHDvABxnruO59621672 = -947238295;    float KHbAMpYyihHDvABxnruO15108102 = -105640928;    float KHbAMpYyihHDvABxnruO50514600 = -745849028;    float KHbAMpYyihHDvABxnruO57673479 = -916561909;    float KHbAMpYyihHDvABxnruO87427811 = -895359627;    float KHbAMpYyihHDvABxnruO33226 = 16012352;    float KHbAMpYyihHDvABxnruO33645241 = -333630168;    float KHbAMpYyihHDvABxnruO98558476 = -951309590;    float KHbAMpYyihHDvABxnruO16992707 = -476362794;    float KHbAMpYyihHDvABxnruO50798891 = -641901081;    float KHbAMpYyihHDvABxnruO49215773 = -51900426;    float KHbAMpYyihHDvABxnruO2312889 = -773663335;    float KHbAMpYyihHDvABxnruO6311462 = -671342819;    float KHbAMpYyihHDvABxnruO66748569 = -206564364;    float KHbAMpYyihHDvABxnruO88095081 = -646869823;    float KHbAMpYyihHDvABxnruO19542482 = -112225714;    float KHbAMpYyihHDvABxnruO64349221 = -645880750;    float KHbAMpYyihHDvABxnruO69283025 = -371049903;    float KHbAMpYyihHDvABxnruO85027266 = -959584014;    float KHbAMpYyihHDvABxnruO51605324 = -644521146;    float KHbAMpYyihHDvABxnruO91886122 = -399642780;    float KHbAMpYyihHDvABxnruO79970060 = 98206390;    float KHbAMpYyihHDvABxnruO52344205 = -417359807;    float KHbAMpYyihHDvABxnruO22618442 = -274020989;    float KHbAMpYyihHDvABxnruO16542622 = -557456346;    float KHbAMpYyihHDvABxnruO89265897 = -341452237;    float KHbAMpYyihHDvABxnruO59489315 = -905418170;    float KHbAMpYyihHDvABxnruO95909537 = -911937062;    float KHbAMpYyihHDvABxnruO13325554 = -20156182;    float KHbAMpYyihHDvABxnruO10795821 = -733357226;    float KHbAMpYyihHDvABxnruO34312562 = -128910213;    float KHbAMpYyihHDvABxnruO32922140 = -855368611;    float KHbAMpYyihHDvABxnruO59567132 = -466304239;    float KHbAMpYyihHDvABxnruO64410104 = -13171759;    float KHbAMpYyihHDvABxnruO90201044 = -293757907;    float KHbAMpYyihHDvABxnruO5313869 = -67708200;    float KHbAMpYyihHDvABxnruO43785331 = -595942246;    float KHbAMpYyihHDvABxnruO48440866 = 77535387;    float KHbAMpYyihHDvABxnruO79719609 = -796028317;    float KHbAMpYyihHDvABxnruO18247233 = -888262593;    float KHbAMpYyihHDvABxnruO82550387 = -740499069;    float KHbAMpYyihHDvABxnruO48964533 = -369817658;    float KHbAMpYyihHDvABxnruO86509319 = 48359732;    float KHbAMpYyihHDvABxnruO10156369 = -379824187;    float KHbAMpYyihHDvABxnruO24600209 = -732055818;    float KHbAMpYyihHDvABxnruO47542276 = -758286976;    float KHbAMpYyihHDvABxnruO23206727 = -53876286;    float KHbAMpYyihHDvABxnruO99652158 = -613025553;    float KHbAMpYyihHDvABxnruO90366494 = -455995532;    float KHbAMpYyihHDvABxnruO85269912 = 66962263;    float KHbAMpYyihHDvABxnruO8273116 = -861157722;    float KHbAMpYyihHDvABxnruO50142492 = -927902393;    float KHbAMpYyihHDvABxnruO71206434 = -156979221;    float KHbAMpYyihHDvABxnruO16658319 = -169763588;     KHbAMpYyihHDvABxnruO4536971 = KHbAMpYyihHDvABxnruO38371614;     KHbAMpYyihHDvABxnruO38371614 = KHbAMpYyihHDvABxnruO30143664;     KHbAMpYyihHDvABxnruO30143664 = KHbAMpYyihHDvABxnruO38083726;     KHbAMpYyihHDvABxnruO38083726 = KHbAMpYyihHDvABxnruO16571286;     KHbAMpYyihHDvABxnruO16571286 = KHbAMpYyihHDvABxnruO8517053;     KHbAMpYyihHDvABxnruO8517053 = KHbAMpYyihHDvABxnruO13842414;     KHbAMpYyihHDvABxnruO13842414 = KHbAMpYyihHDvABxnruO70463566;     KHbAMpYyihHDvABxnruO70463566 = KHbAMpYyihHDvABxnruO36430446;     KHbAMpYyihHDvABxnruO36430446 = KHbAMpYyihHDvABxnruO3307725;     KHbAMpYyihHDvABxnruO3307725 = KHbAMpYyihHDvABxnruO67800951;     KHbAMpYyihHDvABxnruO67800951 = KHbAMpYyihHDvABxnruO39409132;     KHbAMpYyihHDvABxnruO39409132 = KHbAMpYyihHDvABxnruO83472087;     KHbAMpYyihHDvABxnruO83472087 = KHbAMpYyihHDvABxnruO13299465;     KHbAMpYyihHDvABxnruO13299465 = KHbAMpYyihHDvABxnruO96434423;     KHbAMpYyihHDvABxnruO96434423 = KHbAMpYyihHDvABxnruO66397786;     KHbAMpYyihHDvABxnruO66397786 = KHbAMpYyihHDvABxnruO18784086;     KHbAMpYyihHDvABxnruO18784086 = KHbAMpYyihHDvABxnruO25890042;     KHbAMpYyihHDvABxnruO25890042 = KHbAMpYyihHDvABxnruO74371114;     KHbAMpYyihHDvABxnruO74371114 = KHbAMpYyihHDvABxnruO18137948;     KHbAMpYyihHDvABxnruO18137948 = KHbAMpYyihHDvABxnruO83418157;     KHbAMpYyihHDvABxnruO83418157 = KHbAMpYyihHDvABxnruO53738080;     KHbAMpYyihHDvABxnruO53738080 = KHbAMpYyihHDvABxnruO89155840;     KHbAMpYyihHDvABxnruO89155840 = KHbAMpYyihHDvABxnruO27830775;     KHbAMpYyihHDvABxnruO27830775 = KHbAMpYyihHDvABxnruO31772265;     KHbAMpYyihHDvABxnruO31772265 = KHbAMpYyihHDvABxnruO49822717;     KHbAMpYyihHDvABxnruO49822717 = KHbAMpYyihHDvABxnruO20421971;     KHbAMpYyihHDvABxnruO20421971 = KHbAMpYyihHDvABxnruO94299931;     KHbAMpYyihHDvABxnruO94299931 = KHbAMpYyihHDvABxnruO6114345;     KHbAMpYyihHDvABxnruO6114345 = KHbAMpYyihHDvABxnruO67147420;     KHbAMpYyihHDvABxnruO67147420 = KHbAMpYyihHDvABxnruO18280459;     KHbAMpYyihHDvABxnruO18280459 = KHbAMpYyihHDvABxnruO16195628;     KHbAMpYyihHDvABxnruO16195628 = KHbAMpYyihHDvABxnruO47523009;     KHbAMpYyihHDvABxnruO47523009 = KHbAMpYyihHDvABxnruO3502027;     KHbAMpYyihHDvABxnruO3502027 = KHbAMpYyihHDvABxnruO60955259;     KHbAMpYyihHDvABxnruO60955259 = KHbAMpYyihHDvABxnruO73815982;     KHbAMpYyihHDvABxnruO73815982 = KHbAMpYyihHDvABxnruO49855165;     KHbAMpYyihHDvABxnruO49855165 = KHbAMpYyihHDvABxnruO29518189;     KHbAMpYyihHDvABxnruO29518189 = KHbAMpYyihHDvABxnruO66400727;     KHbAMpYyihHDvABxnruO66400727 = KHbAMpYyihHDvABxnruO78461576;     KHbAMpYyihHDvABxnruO78461576 = KHbAMpYyihHDvABxnruO4812394;     KHbAMpYyihHDvABxnruO4812394 = KHbAMpYyihHDvABxnruO72622336;     KHbAMpYyihHDvABxnruO72622336 = KHbAMpYyihHDvABxnruO19425518;     KHbAMpYyihHDvABxnruO19425518 = KHbAMpYyihHDvABxnruO56233701;     KHbAMpYyihHDvABxnruO56233701 = KHbAMpYyihHDvABxnruO68263643;     KHbAMpYyihHDvABxnruO68263643 = KHbAMpYyihHDvABxnruO67362160;     KHbAMpYyihHDvABxnruO67362160 = KHbAMpYyihHDvABxnruO59621672;     KHbAMpYyihHDvABxnruO59621672 = KHbAMpYyihHDvABxnruO15108102;     KHbAMpYyihHDvABxnruO15108102 = KHbAMpYyihHDvABxnruO50514600;     KHbAMpYyihHDvABxnruO50514600 = KHbAMpYyihHDvABxnruO57673479;     KHbAMpYyihHDvABxnruO57673479 = KHbAMpYyihHDvABxnruO87427811;     KHbAMpYyihHDvABxnruO87427811 = KHbAMpYyihHDvABxnruO33226;     KHbAMpYyihHDvABxnruO33226 = KHbAMpYyihHDvABxnruO33645241;     KHbAMpYyihHDvABxnruO33645241 = KHbAMpYyihHDvABxnruO98558476;     KHbAMpYyihHDvABxnruO98558476 = KHbAMpYyihHDvABxnruO16992707;     KHbAMpYyihHDvABxnruO16992707 = KHbAMpYyihHDvABxnruO50798891;     KHbAMpYyihHDvABxnruO50798891 = KHbAMpYyihHDvABxnruO49215773;     KHbAMpYyihHDvABxnruO49215773 = KHbAMpYyihHDvABxnruO2312889;     KHbAMpYyihHDvABxnruO2312889 = KHbAMpYyihHDvABxnruO6311462;     KHbAMpYyihHDvABxnruO6311462 = KHbAMpYyihHDvABxnruO66748569;     KHbAMpYyihHDvABxnruO66748569 = KHbAMpYyihHDvABxnruO88095081;     KHbAMpYyihHDvABxnruO88095081 = KHbAMpYyihHDvABxnruO19542482;     KHbAMpYyihHDvABxnruO19542482 = KHbAMpYyihHDvABxnruO64349221;     KHbAMpYyihHDvABxnruO64349221 = KHbAMpYyihHDvABxnruO69283025;     KHbAMpYyihHDvABxnruO69283025 = KHbAMpYyihHDvABxnruO85027266;     KHbAMpYyihHDvABxnruO85027266 = KHbAMpYyihHDvABxnruO51605324;     KHbAMpYyihHDvABxnruO51605324 = KHbAMpYyihHDvABxnruO91886122;     KHbAMpYyihHDvABxnruO91886122 = KHbAMpYyihHDvABxnruO79970060;     KHbAMpYyihHDvABxnruO79970060 = KHbAMpYyihHDvABxnruO52344205;     KHbAMpYyihHDvABxnruO52344205 = KHbAMpYyihHDvABxnruO22618442;     KHbAMpYyihHDvABxnruO22618442 = KHbAMpYyihHDvABxnruO16542622;     KHbAMpYyihHDvABxnruO16542622 = KHbAMpYyihHDvABxnruO89265897;     KHbAMpYyihHDvABxnruO89265897 = KHbAMpYyihHDvABxnruO59489315;     KHbAMpYyihHDvABxnruO59489315 = KHbAMpYyihHDvABxnruO95909537;     KHbAMpYyihHDvABxnruO95909537 = KHbAMpYyihHDvABxnruO13325554;     KHbAMpYyihHDvABxnruO13325554 = KHbAMpYyihHDvABxnruO10795821;     KHbAMpYyihHDvABxnruO10795821 = KHbAMpYyihHDvABxnruO34312562;     KHbAMpYyihHDvABxnruO34312562 = KHbAMpYyihHDvABxnruO32922140;     KHbAMpYyihHDvABxnruO32922140 = KHbAMpYyihHDvABxnruO59567132;     KHbAMpYyihHDvABxnruO59567132 = KHbAMpYyihHDvABxnruO64410104;     KHbAMpYyihHDvABxnruO64410104 = KHbAMpYyihHDvABxnruO90201044;     KHbAMpYyihHDvABxnruO90201044 = KHbAMpYyihHDvABxnruO5313869;     KHbAMpYyihHDvABxnruO5313869 = KHbAMpYyihHDvABxnruO43785331;     KHbAMpYyihHDvABxnruO43785331 = KHbAMpYyihHDvABxnruO48440866;     KHbAMpYyihHDvABxnruO48440866 = KHbAMpYyihHDvABxnruO79719609;     KHbAMpYyihHDvABxnruO79719609 = KHbAMpYyihHDvABxnruO18247233;     KHbAMpYyihHDvABxnruO18247233 = KHbAMpYyihHDvABxnruO82550387;     KHbAMpYyihHDvABxnruO82550387 = KHbAMpYyihHDvABxnruO48964533;     KHbAMpYyihHDvABxnruO48964533 = KHbAMpYyihHDvABxnruO86509319;     KHbAMpYyihHDvABxnruO86509319 = KHbAMpYyihHDvABxnruO10156369;     KHbAMpYyihHDvABxnruO10156369 = KHbAMpYyihHDvABxnruO24600209;     KHbAMpYyihHDvABxnruO24600209 = KHbAMpYyihHDvABxnruO47542276;     KHbAMpYyihHDvABxnruO47542276 = KHbAMpYyihHDvABxnruO23206727;     KHbAMpYyihHDvABxnruO23206727 = KHbAMpYyihHDvABxnruO99652158;     KHbAMpYyihHDvABxnruO99652158 = KHbAMpYyihHDvABxnruO90366494;     KHbAMpYyihHDvABxnruO90366494 = KHbAMpYyihHDvABxnruO85269912;     KHbAMpYyihHDvABxnruO85269912 = KHbAMpYyihHDvABxnruO8273116;     KHbAMpYyihHDvABxnruO8273116 = KHbAMpYyihHDvABxnruO50142492;     KHbAMpYyihHDvABxnruO50142492 = KHbAMpYyihHDvABxnruO71206434;     KHbAMpYyihHDvABxnruO71206434 = KHbAMpYyihHDvABxnruO16658319;     KHbAMpYyihHDvABxnruO16658319 = KHbAMpYyihHDvABxnruO4536971;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LdQQNxByBXnVQFSJiVbQ7069609() {     float NJmwlFtCFFOCHEVvXAEn4530500 = -42287730;    float NJmwlFtCFFOCHEVvXAEn76943930 = -166427396;    float NJmwlFtCFFOCHEVvXAEn31456876 = -266725749;    float NJmwlFtCFFOCHEVvXAEn23163587 = -513683086;    float NJmwlFtCFFOCHEVvXAEn25411388 = 84084114;    float NJmwlFtCFFOCHEVvXAEn43322485 = -719651761;    float NJmwlFtCFFOCHEVvXAEn82389749 = -258553454;    float NJmwlFtCFFOCHEVvXAEn39677617 = -289084589;    float NJmwlFtCFFOCHEVvXAEn19170610 = -876083916;    float NJmwlFtCFFOCHEVvXAEn99947238 = -568641125;    float NJmwlFtCFFOCHEVvXAEn88121556 = -778122454;    float NJmwlFtCFFOCHEVvXAEn53700809 = -556248389;    float NJmwlFtCFFOCHEVvXAEn15325001 = -551455146;    float NJmwlFtCFFOCHEVvXAEn67143684 = -884238125;    float NJmwlFtCFFOCHEVvXAEn41606533 = -120690084;    float NJmwlFtCFFOCHEVvXAEn40962333 = -422202306;    float NJmwlFtCFFOCHEVvXAEn74636845 = 13714090;    float NJmwlFtCFFOCHEVvXAEn48890575 = -122079707;    float NJmwlFtCFFOCHEVvXAEn94799383 = -632784683;    float NJmwlFtCFFOCHEVvXAEn90211710 = -877444083;    float NJmwlFtCFFOCHEVvXAEn63087581 = 49137929;    float NJmwlFtCFFOCHEVvXAEn32034346 = 62145609;    float NJmwlFtCFFOCHEVvXAEn26034237 = -430889687;    float NJmwlFtCFFOCHEVvXAEn71480883 = -416854324;    float NJmwlFtCFFOCHEVvXAEn60214954 = -348493290;    float NJmwlFtCFFOCHEVvXAEn66396794 = -149202130;    float NJmwlFtCFFOCHEVvXAEn90643552 = -574148840;    float NJmwlFtCFFOCHEVvXAEn29703457 = -950758263;    float NJmwlFtCFFOCHEVvXAEn18792792 = -485096837;    float NJmwlFtCFFOCHEVvXAEn43237290 = -366819317;    float NJmwlFtCFFOCHEVvXAEn51693137 = -259343475;    float NJmwlFtCFFOCHEVvXAEn919485 = -465950998;    float NJmwlFtCFFOCHEVvXAEn32975433 = -523071463;    float NJmwlFtCFFOCHEVvXAEn3784864 = -404684794;    float NJmwlFtCFFOCHEVvXAEn43313298 = -281039192;    float NJmwlFtCFFOCHEVvXAEn16644172 = -963721990;    float NJmwlFtCFFOCHEVvXAEn61796532 = -631603688;    float NJmwlFtCFFOCHEVvXAEn99435867 = -820481784;    float NJmwlFtCFFOCHEVvXAEn1714324 = -532271332;    float NJmwlFtCFFOCHEVvXAEn50334469 = -812297041;    float NJmwlFtCFFOCHEVvXAEn92668061 = -233207349;    float NJmwlFtCFFOCHEVvXAEn58319998 = -700830554;    float NJmwlFtCFFOCHEVvXAEn79708878 = -858082550;    float NJmwlFtCFFOCHEVvXAEn15465600 = -773527678;    float NJmwlFtCFFOCHEVvXAEn52281695 = -442888965;    float NJmwlFtCFFOCHEVvXAEn7790129 = -720672349;    float NJmwlFtCFFOCHEVvXAEn87319465 = -328518917;    float NJmwlFtCFFOCHEVvXAEn90491682 = -714402284;    float NJmwlFtCFFOCHEVvXAEn69172990 = -309612890;    float NJmwlFtCFFOCHEVvXAEn77718902 = -503586795;    float NJmwlFtCFFOCHEVvXAEn687182 = -473914634;    float NJmwlFtCFFOCHEVvXAEn14580483 = -366133397;    float NJmwlFtCFFOCHEVvXAEn9093589 = -225684177;    float NJmwlFtCFFOCHEVvXAEn8808748 = -383229441;    float NJmwlFtCFFOCHEVvXAEn43982027 = -233262072;    float NJmwlFtCFFOCHEVvXAEn72496153 = -4433339;    float NJmwlFtCFFOCHEVvXAEn50909693 = -735537709;    float NJmwlFtCFFOCHEVvXAEn59975993 = -849871426;    float NJmwlFtCFFOCHEVvXAEn62948632 = -65189796;    float NJmwlFtCFFOCHEVvXAEn59014594 = -766713757;    float NJmwlFtCFFOCHEVvXAEn52678933 = -45502922;    float NJmwlFtCFFOCHEVvXAEn52686293 = -307795192;    float NJmwlFtCFFOCHEVvXAEn20884825 = -803987752;    float NJmwlFtCFFOCHEVvXAEn75933319 = -409264600;    float NJmwlFtCFFOCHEVvXAEn48254101 = -209297650;    float NJmwlFtCFFOCHEVvXAEn87202071 = -212171457;    float NJmwlFtCFFOCHEVvXAEn20725377 = 66823074;    float NJmwlFtCFFOCHEVvXAEn11540138 = -46770352;    float NJmwlFtCFFOCHEVvXAEn23830387 = -503198934;    float NJmwlFtCFFOCHEVvXAEn24962361 = -156968095;    float NJmwlFtCFFOCHEVvXAEn79165800 = -790598619;    float NJmwlFtCFFOCHEVvXAEn75200977 = -165804127;    float NJmwlFtCFFOCHEVvXAEn47176251 = -589808375;    float NJmwlFtCFFOCHEVvXAEn44464915 = -820487643;    float NJmwlFtCFFOCHEVvXAEn97543649 = -544236734;    float NJmwlFtCFFOCHEVvXAEn4767584 = -250031517;    float NJmwlFtCFFOCHEVvXAEn52325468 = -79771842;    float NJmwlFtCFFOCHEVvXAEn10568638 = -657362010;    float NJmwlFtCFFOCHEVvXAEn19199188 = -973965360;    float NJmwlFtCFFOCHEVvXAEn52424825 = -627820941;    float NJmwlFtCFFOCHEVvXAEn79077328 = -820683213;    float NJmwlFtCFFOCHEVvXAEn151870 = -859746556;    float NJmwlFtCFFOCHEVvXAEn60530466 = -541145373;    float NJmwlFtCFFOCHEVvXAEn41073889 = -981510042;    float NJmwlFtCFFOCHEVvXAEn42550109 = -892904683;    float NJmwlFtCFFOCHEVvXAEn37112654 = -893210079;    float NJmwlFtCFFOCHEVvXAEn91825896 = -140266822;    float NJmwlFtCFFOCHEVvXAEn24166685 = -39842023;    float NJmwlFtCFFOCHEVvXAEn59802836 = -71422723;    float NJmwlFtCFFOCHEVvXAEn70817144 = -176605853;    float NJmwlFtCFFOCHEVvXAEn65734478 = -128184281;    float NJmwlFtCFFOCHEVvXAEn1820540 = -781732263;    float NJmwlFtCFFOCHEVvXAEn36487236 = -655291988;    float NJmwlFtCFFOCHEVvXAEn42699730 = -765557576;    float NJmwlFtCFFOCHEVvXAEn97655536 = -666794119;    float NJmwlFtCFFOCHEVvXAEn39981769 = -925412157;    float NJmwlFtCFFOCHEVvXAEn37435173 = -896842802;    float NJmwlFtCFFOCHEVvXAEn3775559 = -348817951;    float NJmwlFtCFFOCHEVvXAEn67211498 = -464230028;    float NJmwlFtCFFOCHEVvXAEn65079623 = -42287730;     NJmwlFtCFFOCHEVvXAEn4530500 = NJmwlFtCFFOCHEVvXAEn76943930;     NJmwlFtCFFOCHEVvXAEn76943930 = NJmwlFtCFFOCHEVvXAEn31456876;     NJmwlFtCFFOCHEVvXAEn31456876 = NJmwlFtCFFOCHEVvXAEn23163587;     NJmwlFtCFFOCHEVvXAEn23163587 = NJmwlFtCFFOCHEVvXAEn25411388;     NJmwlFtCFFOCHEVvXAEn25411388 = NJmwlFtCFFOCHEVvXAEn43322485;     NJmwlFtCFFOCHEVvXAEn43322485 = NJmwlFtCFFOCHEVvXAEn82389749;     NJmwlFtCFFOCHEVvXAEn82389749 = NJmwlFtCFFOCHEVvXAEn39677617;     NJmwlFtCFFOCHEVvXAEn39677617 = NJmwlFtCFFOCHEVvXAEn19170610;     NJmwlFtCFFOCHEVvXAEn19170610 = NJmwlFtCFFOCHEVvXAEn99947238;     NJmwlFtCFFOCHEVvXAEn99947238 = NJmwlFtCFFOCHEVvXAEn88121556;     NJmwlFtCFFOCHEVvXAEn88121556 = NJmwlFtCFFOCHEVvXAEn53700809;     NJmwlFtCFFOCHEVvXAEn53700809 = NJmwlFtCFFOCHEVvXAEn15325001;     NJmwlFtCFFOCHEVvXAEn15325001 = NJmwlFtCFFOCHEVvXAEn67143684;     NJmwlFtCFFOCHEVvXAEn67143684 = NJmwlFtCFFOCHEVvXAEn41606533;     NJmwlFtCFFOCHEVvXAEn41606533 = NJmwlFtCFFOCHEVvXAEn40962333;     NJmwlFtCFFOCHEVvXAEn40962333 = NJmwlFtCFFOCHEVvXAEn74636845;     NJmwlFtCFFOCHEVvXAEn74636845 = NJmwlFtCFFOCHEVvXAEn48890575;     NJmwlFtCFFOCHEVvXAEn48890575 = NJmwlFtCFFOCHEVvXAEn94799383;     NJmwlFtCFFOCHEVvXAEn94799383 = NJmwlFtCFFOCHEVvXAEn90211710;     NJmwlFtCFFOCHEVvXAEn90211710 = NJmwlFtCFFOCHEVvXAEn63087581;     NJmwlFtCFFOCHEVvXAEn63087581 = NJmwlFtCFFOCHEVvXAEn32034346;     NJmwlFtCFFOCHEVvXAEn32034346 = NJmwlFtCFFOCHEVvXAEn26034237;     NJmwlFtCFFOCHEVvXAEn26034237 = NJmwlFtCFFOCHEVvXAEn71480883;     NJmwlFtCFFOCHEVvXAEn71480883 = NJmwlFtCFFOCHEVvXAEn60214954;     NJmwlFtCFFOCHEVvXAEn60214954 = NJmwlFtCFFOCHEVvXAEn66396794;     NJmwlFtCFFOCHEVvXAEn66396794 = NJmwlFtCFFOCHEVvXAEn90643552;     NJmwlFtCFFOCHEVvXAEn90643552 = NJmwlFtCFFOCHEVvXAEn29703457;     NJmwlFtCFFOCHEVvXAEn29703457 = NJmwlFtCFFOCHEVvXAEn18792792;     NJmwlFtCFFOCHEVvXAEn18792792 = NJmwlFtCFFOCHEVvXAEn43237290;     NJmwlFtCFFOCHEVvXAEn43237290 = NJmwlFtCFFOCHEVvXAEn51693137;     NJmwlFtCFFOCHEVvXAEn51693137 = NJmwlFtCFFOCHEVvXAEn919485;     NJmwlFtCFFOCHEVvXAEn919485 = NJmwlFtCFFOCHEVvXAEn32975433;     NJmwlFtCFFOCHEVvXAEn32975433 = NJmwlFtCFFOCHEVvXAEn3784864;     NJmwlFtCFFOCHEVvXAEn3784864 = NJmwlFtCFFOCHEVvXAEn43313298;     NJmwlFtCFFOCHEVvXAEn43313298 = NJmwlFtCFFOCHEVvXAEn16644172;     NJmwlFtCFFOCHEVvXAEn16644172 = NJmwlFtCFFOCHEVvXAEn61796532;     NJmwlFtCFFOCHEVvXAEn61796532 = NJmwlFtCFFOCHEVvXAEn99435867;     NJmwlFtCFFOCHEVvXAEn99435867 = NJmwlFtCFFOCHEVvXAEn1714324;     NJmwlFtCFFOCHEVvXAEn1714324 = NJmwlFtCFFOCHEVvXAEn50334469;     NJmwlFtCFFOCHEVvXAEn50334469 = NJmwlFtCFFOCHEVvXAEn92668061;     NJmwlFtCFFOCHEVvXAEn92668061 = NJmwlFtCFFOCHEVvXAEn58319998;     NJmwlFtCFFOCHEVvXAEn58319998 = NJmwlFtCFFOCHEVvXAEn79708878;     NJmwlFtCFFOCHEVvXAEn79708878 = NJmwlFtCFFOCHEVvXAEn15465600;     NJmwlFtCFFOCHEVvXAEn15465600 = NJmwlFtCFFOCHEVvXAEn52281695;     NJmwlFtCFFOCHEVvXAEn52281695 = NJmwlFtCFFOCHEVvXAEn7790129;     NJmwlFtCFFOCHEVvXAEn7790129 = NJmwlFtCFFOCHEVvXAEn87319465;     NJmwlFtCFFOCHEVvXAEn87319465 = NJmwlFtCFFOCHEVvXAEn90491682;     NJmwlFtCFFOCHEVvXAEn90491682 = NJmwlFtCFFOCHEVvXAEn69172990;     NJmwlFtCFFOCHEVvXAEn69172990 = NJmwlFtCFFOCHEVvXAEn77718902;     NJmwlFtCFFOCHEVvXAEn77718902 = NJmwlFtCFFOCHEVvXAEn687182;     NJmwlFtCFFOCHEVvXAEn687182 = NJmwlFtCFFOCHEVvXAEn14580483;     NJmwlFtCFFOCHEVvXAEn14580483 = NJmwlFtCFFOCHEVvXAEn9093589;     NJmwlFtCFFOCHEVvXAEn9093589 = NJmwlFtCFFOCHEVvXAEn8808748;     NJmwlFtCFFOCHEVvXAEn8808748 = NJmwlFtCFFOCHEVvXAEn43982027;     NJmwlFtCFFOCHEVvXAEn43982027 = NJmwlFtCFFOCHEVvXAEn72496153;     NJmwlFtCFFOCHEVvXAEn72496153 = NJmwlFtCFFOCHEVvXAEn50909693;     NJmwlFtCFFOCHEVvXAEn50909693 = NJmwlFtCFFOCHEVvXAEn59975993;     NJmwlFtCFFOCHEVvXAEn59975993 = NJmwlFtCFFOCHEVvXAEn62948632;     NJmwlFtCFFOCHEVvXAEn62948632 = NJmwlFtCFFOCHEVvXAEn59014594;     NJmwlFtCFFOCHEVvXAEn59014594 = NJmwlFtCFFOCHEVvXAEn52678933;     NJmwlFtCFFOCHEVvXAEn52678933 = NJmwlFtCFFOCHEVvXAEn52686293;     NJmwlFtCFFOCHEVvXAEn52686293 = NJmwlFtCFFOCHEVvXAEn20884825;     NJmwlFtCFFOCHEVvXAEn20884825 = NJmwlFtCFFOCHEVvXAEn75933319;     NJmwlFtCFFOCHEVvXAEn75933319 = NJmwlFtCFFOCHEVvXAEn48254101;     NJmwlFtCFFOCHEVvXAEn48254101 = NJmwlFtCFFOCHEVvXAEn87202071;     NJmwlFtCFFOCHEVvXAEn87202071 = NJmwlFtCFFOCHEVvXAEn20725377;     NJmwlFtCFFOCHEVvXAEn20725377 = NJmwlFtCFFOCHEVvXAEn11540138;     NJmwlFtCFFOCHEVvXAEn11540138 = NJmwlFtCFFOCHEVvXAEn23830387;     NJmwlFtCFFOCHEVvXAEn23830387 = NJmwlFtCFFOCHEVvXAEn24962361;     NJmwlFtCFFOCHEVvXAEn24962361 = NJmwlFtCFFOCHEVvXAEn79165800;     NJmwlFtCFFOCHEVvXAEn79165800 = NJmwlFtCFFOCHEVvXAEn75200977;     NJmwlFtCFFOCHEVvXAEn75200977 = NJmwlFtCFFOCHEVvXAEn47176251;     NJmwlFtCFFOCHEVvXAEn47176251 = NJmwlFtCFFOCHEVvXAEn44464915;     NJmwlFtCFFOCHEVvXAEn44464915 = NJmwlFtCFFOCHEVvXAEn97543649;     NJmwlFtCFFOCHEVvXAEn97543649 = NJmwlFtCFFOCHEVvXAEn4767584;     NJmwlFtCFFOCHEVvXAEn4767584 = NJmwlFtCFFOCHEVvXAEn52325468;     NJmwlFtCFFOCHEVvXAEn52325468 = NJmwlFtCFFOCHEVvXAEn10568638;     NJmwlFtCFFOCHEVvXAEn10568638 = NJmwlFtCFFOCHEVvXAEn19199188;     NJmwlFtCFFOCHEVvXAEn19199188 = NJmwlFtCFFOCHEVvXAEn52424825;     NJmwlFtCFFOCHEVvXAEn52424825 = NJmwlFtCFFOCHEVvXAEn79077328;     NJmwlFtCFFOCHEVvXAEn79077328 = NJmwlFtCFFOCHEVvXAEn151870;     NJmwlFtCFFOCHEVvXAEn151870 = NJmwlFtCFFOCHEVvXAEn60530466;     NJmwlFtCFFOCHEVvXAEn60530466 = NJmwlFtCFFOCHEVvXAEn41073889;     NJmwlFtCFFOCHEVvXAEn41073889 = NJmwlFtCFFOCHEVvXAEn42550109;     NJmwlFtCFFOCHEVvXAEn42550109 = NJmwlFtCFFOCHEVvXAEn37112654;     NJmwlFtCFFOCHEVvXAEn37112654 = NJmwlFtCFFOCHEVvXAEn91825896;     NJmwlFtCFFOCHEVvXAEn91825896 = NJmwlFtCFFOCHEVvXAEn24166685;     NJmwlFtCFFOCHEVvXAEn24166685 = NJmwlFtCFFOCHEVvXAEn59802836;     NJmwlFtCFFOCHEVvXAEn59802836 = NJmwlFtCFFOCHEVvXAEn70817144;     NJmwlFtCFFOCHEVvXAEn70817144 = NJmwlFtCFFOCHEVvXAEn65734478;     NJmwlFtCFFOCHEVvXAEn65734478 = NJmwlFtCFFOCHEVvXAEn1820540;     NJmwlFtCFFOCHEVvXAEn1820540 = NJmwlFtCFFOCHEVvXAEn36487236;     NJmwlFtCFFOCHEVvXAEn36487236 = NJmwlFtCFFOCHEVvXAEn42699730;     NJmwlFtCFFOCHEVvXAEn42699730 = NJmwlFtCFFOCHEVvXAEn97655536;     NJmwlFtCFFOCHEVvXAEn97655536 = NJmwlFtCFFOCHEVvXAEn39981769;     NJmwlFtCFFOCHEVvXAEn39981769 = NJmwlFtCFFOCHEVvXAEn37435173;     NJmwlFtCFFOCHEVvXAEn37435173 = NJmwlFtCFFOCHEVvXAEn3775559;     NJmwlFtCFFOCHEVvXAEn3775559 = NJmwlFtCFFOCHEVvXAEn67211498;     NJmwlFtCFFOCHEVvXAEn67211498 = NJmwlFtCFFOCHEVvXAEn65079623;     NJmwlFtCFFOCHEVvXAEn65079623 = NJmwlFtCFFOCHEVvXAEn4530500;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MICsXwiPBbwqoWwzpQLn61485822() {     float dlBrrFNwcTXxdokOGJGc75182166 = -660436860;    float dlBrrFNwcTXxdokOGJGc45617448 = -38070695;    float dlBrrFNwcTXxdokOGJGc11425795 = -709020209;    float dlBrrFNwcTXxdokOGJGc63094873 = -703821475;    float dlBrrFNwcTXxdokOGJGc46861709 = -799322234;    float dlBrrFNwcTXxdokOGJGc33322247 = -421232913;    float dlBrrFNwcTXxdokOGJGc768645 = -257329164;    float dlBrrFNwcTXxdokOGJGc96519997 = -635901679;    float dlBrrFNwcTXxdokOGJGc65035664 = -969431245;    float dlBrrFNwcTXxdokOGJGc22913700 = -869736475;    float dlBrrFNwcTXxdokOGJGc30370718 = -813024384;    float dlBrrFNwcTXxdokOGJGc99247380 = -872012182;    float dlBrrFNwcTXxdokOGJGc95152387 = -460992204;    float dlBrrFNwcTXxdokOGJGc50990718 = -685266335;    float dlBrrFNwcTXxdokOGJGc24326553 = -270440087;    float dlBrrFNwcTXxdokOGJGc73490973 = 7814107;    float dlBrrFNwcTXxdokOGJGc68489672 = -995471040;    float dlBrrFNwcTXxdokOGJGc69184069 = -111245903;    float dlBrrFNwcTXxdokOGJGc76507165 = -239972625;    float dlBrrFNwcTXxdokOGJGc48998420 = -319810387;    float dlBrrFNwcTXxdokOGJGc18223336 = -959541211;    float dlBrrFNwcTXxdokOGJGc93430110 = -269139697;    float dlBrrFNwcTXxdokOGJGc27978046 = -998732832;    float dlBrrFNwcTXxdokOGJGc5330420 = -9255230;    float dlBrrFNwcTXxdokOGJGc24339343 = -24397978;    float dlBrrFNwcTXxdokOGJGc96994374 = -247486575;    float dlBrrFNwcTXxdokOGJGc98904337 = -969326615;    float dlBrrFNwcTXxdokOGJGc37485682 = -355882030;    float dlBrrFNwcTXxdokOGJGc50935279 = -779455054;    float dlBrrFNwcTXxdokOGJGc82409386 = -925214387;    float dlBrrFNwcTXxdokOGJGc58765134 = -7777161;    float dlBrrFNwcTXxdokOGJGc45430946 = -794248161;    float dlBrrFNwcTXxdokOGJGc15920794 = -470033072;    float dlBrrFNwcTXxdokOGJGc69149838 = 31785522;    float dlBrrFNwcTXxdokOGJGc21337534 = -127985971;    float dlBrrFNwcTXxdokOGJGc76637430 = 57051462;    float dlBrrFNwcTXxdokOGJGc5588019 = -820196896;    float dlBrrFNwcTXxdokOGJGc52228356 = -604594891;    float dlBrrFNwcTXxdokOGJGc80247612 = -487999969;    float dlBrrFNwcTXxdokOGJGc45437767 = -329905403;    float dlBrrFNwcTXxdokOGJGc52741345 = -617226227;    float dlBrrFNwcTXxdokOGJGc86802988 = -470219623;    float dlBrrFNwcTXxdokOGJGc28675838 = -844836602;    float dlBrrFNwcTXxdokOGJGc21973900 = -845502649;    float dlBrrFNwcTXxdokOGJGc48235754 = -751535359;    float dlBrrFNwcTXxdokOGJGc14195964 = -479009504;    float dlBrrFNwcTXxdokOGJGc13450669 = -444588717;    float dlBrrFNwcTXxdokOGJGc56105893 = -66532579;    float dlBrrFNwcTXxdokOGJGc74614442 = -295376859;    float dlBrrFNwcTXxdokOGJGc86146388 = -786595674;    float dlBrrFNwcTXxdokOGJGc68876505 = -20486755;    float dlBrrFNwcTXxdokOGJGc56451987 = -335353119;    float dlBrrFNwcTXxdokOGJGc64169246 = -153482219;    float dlBrrFNwcTXxdokOGJGc99036052 = 55889062;    float dlBrrFNwcTXxdokOGJGc43262486 = -344202986;    float dlBrrFNwcTXxdokOGJGc81752055 = -291297164;    float dlBrrFNwcTXxdokOGJGc17639402 = -39337864;    float dlBrrFNwcTXxdokOGJGc6095375 = -599764979;    float dlBrrFNwcTXxdokOGJGc38755531 = -579423498;    float dlBrrFNwcTXxdokOGJGc49867335 = -451835659;    float dlBrrFNwcTXxdokOGJGc34417910 = -451906298;    float dlBrrFNwcTXxdokOGJGc63282963 = -901447135;    float dlBrrFNwcTXxdokOGJGc45584718 = -856446625;    float dlBrrFNwcTXxdokOGJGc82626278 = 55783142;    float dlBrrFNwcTXxdokOGJGc64148566 = -761959315;    float dlBrrFNwcTXxdokOGJGc84939771 = 81223777;    float dlBrrFNwcTXxdokOGJGc83326587 = -301979110;    float dlBrrFNwcTXxdokOGJGc26002550 = -392777726;    float dlBrrFNwcTXxdokOGJGc29653185 = -457280365;    float dlBrrFNwcTXxdokOGJGc47689122 = -227491550;    float dlBrrFNwcTXxdokOGJGc67902955 = -171988998;    float dlBrrFNwcTXxdokOGJGc16261316 = -290876150;    float dlBrrFNwcTXxdokOGJGc88936456 = -623245934;    float dlBrrFNwcTXxdokOGJGc31069398 = -910067222;    float dlBrrFNwcTXxdokOGJGc96257074 = -702584161;    float dlBrrFNwcTXxdokOGJGc31420347 = -389321588;    float dlBrrFNwcTXxdokOGJGc64754272 = -424303095;    float dlBrrFNwcTXxdokOGJGc6004146 = -53230183;    float dlBrrFNwcTXxdokOGJGc57094666 = -257719871;    float dlBrrFNwcTXxdokOGJGc10143379 = -545388474;    float dlBrrFNwcTXxdokOGJGc83543706 = -802897859;    float dlBrrFNwcTXxdokOGJGc42798445 = -802794036;    float dlBrrFNwcTXxdokOGJGc62871239 = 39494828;    float dlBrrFNwcTXxdokOGJGc64788891 = -992859380;    float dlBrrFNwcTXxdokOGJGc13532881 = -804727633;    float dlBrrFNwcTXxdokOGJGc2313147 = -672424043;    float dlBrrFNwcTXxdokOGJGc81261700 = -540765943;    float dlBrrFNwcTXxdokOGJGc16884742 = -425922135;    float dlBrrFNwcTXxdokOGJGc25887353 = -624011493;    float dlBrrFNwcTXxdokOGJGc39585478 = -836688808;    float dlBrrFNwcTXxdokOGJGc58998029 = -903610674;    float dlBrrFNwcTXxdokOGJGc99492643 = -120431917;    float dlBrrFNwcTXxdokOGJGc13472826 = 74828606;    float dlBrrFNwcTXxdokOGJGc30380278 = 63835690;    float dlBrrFNwcTXxdokOGJGc11019858 = -877999106;    float dlBrrFNwcTXxdokOGJGc89458382 = -715779093;    float dlBrrFNwcTXxdokOGJGc41218271 = -613772998;    float dlBrrFNwcTXxdokOGJGc46049560 = -800619745;    float dlBrrFNwcTXxdokOGJGc57825334 = 16456665;    float dlBrrFNwcTXxdokOGJGc63295982 = -660436860;     dlBrrFNwcTXxdokOGJGc75182166 = dlBrrFNwcTXxdokOGJGc45617448;     dlBrrFNwcTXxdokOGJGc45617448 = dlBrrFNwcTXxdokOGJGc11425795;     dlBrrFNwcTXxdokOGJGc11425795 = dlBrrFNwcTXxdokOGJGc63094873;     dlBrrFNwcTXxdokOGJGc63094873 = dlBrrFNwcTXxdokOGJGc46861709;     dlBrrFNwcTXxdokOGJGc46861709 = dlBrrFNwcTXxdokOGJGc33322247;     dlBrrFNwcTXxdokOGJGc33322247 = dlBrrFNwcTXxdokOGJGc768645;     dlBrrFNwcTXxdokOGJGc768645 = dlBrrFNwcTXxdokOGJGc96519997;     dlBrrFNwcTXxdokOGJGc96519997 = dlBrrFNwcTXxdokOGJGc65035664;     dlBrrFNwcTXxdokOGJGc65035664 = dlBrrFNwcTXxdokOGJGc22913700;     dlBrrFNwcTXxdokOGJGc22913700 = dlBrrFNwcTXxdokOGJGc30370718;     dlBrrFNwcTXxdokOGJGc30370718 = dlBrrFNwcTXxdokOGJGc99247380;     dlBrrFNwcTXxdokOGJGc99247380 = dlBrrFNwcTXxdokOGJGc95152387;     dlBrrFNwcTXxdokOGJGc95152387 = dlBrrFNwcTXxdokOGJGc50990718;     dlBrrFNwcTXxdokOGJGc50990718 = dlBrrFNwcTXxdokOGJGc24326553;     dlBrrFNwcTXxdokOGJGc24326553 = dlBrrFNwcTXxdokOGJGc73490973;     dlBrrFNwcTXxdokOGJGc73490973 = dlBrrFNwcTXxdokOGJGc68489672;     dlBrrFNwcTXxdokOGJGc68489672 = dlBrrFNwcTXxdokOGJGc69184069;     dlBrrFNwcTXxdokOGJGc69184069 = dlBrrFNwcTXxdokOGJGc76507165;     dlBrrFNwcTXxdokOGJGc76507165 = dlBrrFNwcTXxdokOGJGc48998420;     dlBrrFNwcTXxdokOGJGc48998420 = dlBrrFNwcTXxdokOGJGc18223336;     dlBrrFNwcTXxdokOGJGc18223336 = dlBrrFNwcTXxdokOGJGc93430110;     dlBrrFNwcTXxdokOGJGc93430110 = dlBrrFNwcTXxdokOGJGc27978046;     dlBrrFNwcTXxdokOGJGc27978046 = dlBrrFNwcTXxdokOGJGc5330420;     dlBrrFNwcTXxdokOGJGc5330420 = dlBrrFNwcTXxdokOGJGc24339343;     dlBrrFNwcTXxdokOGJGc24339343 = dlBrrFNwcTXxdokOGJGc96994374;     dlBrrFNwcTXxdokOGJGc96994374 = dlBrrFNwcTXxdokOGJGc98904337;     dlBrrFNwcTXxdokOGJGc98904337 = dlBrrFNwcTXxdokOGJGc37485682;     dlBrrFNwcTXxdokOGJGc37485682 = dlBrrFNwcTXxdokOGJGc50935279;     dlBrrFNwcTXxdokOGJGc50935279 = dlBrrFNwcTXxdokOGJGc82409386;     dlBrrFNwcTXxdokOGJGc82409386 = dlBrrFNwcTXxdokOGJGc58765134;     dlBrrFNwcTXxdokOGJGc58765134 = dlBrrFNwcTXxdokOGJGc45430946;     dlBrrFNwcTXxdokOGJGc45430946 = dlBrrFNwcTXxdokOGJGc15920794;     dlBrrFNwcTXxdokOGJGc15920794 = dlBrrFNwcTXxdokOGJGc69149838;     dlBrrFNwcTXxdokOGJGc69149838 = dlBrrFNwcTXxdokOGJGc21337534;     dlBrrFNwcTXxdokOGJGc21337534 = dlBrrFNwcTXxdokOGJGc76637430;     dlBrrFNwcTXxdokOGJGc76637430 = dlBrrFNwcTXxdokOGJGc5588019;     dlBrrFNwcTXxdokOGJGc5588019 = dlBrrFNwcTXxdokOGJGc52228356;     dlBrrFNwcTXxdokOGJGc52228356 = dlBrrFNwcTXxdokOGJGc80247612;     dlBrrFNwcTXxdokOGJGc80247612 = dlBrrFNwcTXxdokOGJGc45437767;     dlBrrFNwcTXxdokOGJGc45437767 = dlBrrFNwcTXxdokOGJGc52741345;     dlBrrFNwcTXxdokOGJGc52741345 = dlBrrFNwcTXxdokOGJGc86802988;     dlBrrFNwcTXxdokOGJGc86802988 = dlBrrFNwcTXxdokOGJGc28675838;     dlBrrFNwcTXxdokOGJGc28675838 = dlBrrFNwcTXxdokOGJGc21973900;     dlBrrFNwcTXxdokOGJGc21973900 = dlBrrFNwcTXxdokOGJGc48235754;     dlBrrFNwcTXxdokOGJGc48235754 = dlBrrFNwcTXxdokOGJGc14195964;     dlBrrFNwcTXxdokOGJGc14195964 = dlBrrFNwcTXxdokOGJGc13450669;     dlBrrFNwcTXxdokOGJGc13450669 = dlBrrFNwcTXxdokOGJGc56105893;     dlBrrFNwcTXxdokOGJGc56105893 = dlBrrFNwcTXxdokOGJGc74614442;     dlBrrFNwcTXxdokOGJGc74614442 = dlBrrFNwcTXxdokOGJGc86146388;     dlBrrFNwcTXxdokOGJGc86146388 = dlBrrFNwcTXxdokOGJGc68876505;     dlBrrFNwcTXxdokOGJGc68876505 = dlBrrFNwcTXxdokOGJGc56451987;     dlBrrFNwcTXxdokOGJGc56451987 = dlBrrFNwcTXxdokOGJGc64169246;     dlBrrFNwcTXxdokOGJGc64169246 = dlBrrFNwcTXxdokOGJGc99036052;     dlBrrFNwcTXxdokOGJGc99036052 = dlBrrFNwcTXxdokOGJGc43262486;     dlBrrFNwcTXxdokOGJGc43262486 = dlBrrFNwcTXxdokOGJGc81752055;     dlBrrFNwcTXxdokOGJGc81752055 = dlBrrFNwcTXxdokOGJGc17639402;     dlBrrFNwcTXxdokOGJGc17639402 = dlBrrFNwcTXxdokOGJGc6095375;     dlBrrFNwcTXxdokOGJGc6095375 = dlBrrFNwcTXxdokOGJGc38755531;     dlBrrFNwcTXxdokOGJGc38755531 = dlBrrFNwcTXxdokOGJGc49867335;     dlBrrFNwcTXxdokOGJGc49867335 = dlBrrFNwcTXxdokOGJGc34417910;     dlBrrFNwcTXxdokOGJGc34417910 = dlBrrFNwcTXxdokOGJGc63282963;     dlBrrFNwcTXxdokOGJGc63282963 = dlBrrFNwcTXxdokOGJGc45584718;     dlBrrFNwcTXxdokOGJGc45584718 = dlBrrFNwcTXxdokOGJGc82626278;     dlBrrFNwcTXxdokOGJGc82626278 = dlBrrFNwcTXxdokOGJGc64148566;     dlBrrFNwcTXxdokOGJGc64148566 = dlBrrFNwcTXxdokOGJGc84939771;     dlBrrFNwcTXxdokOGJGc84939771 = dlBrrFNwcTXxdokOGJGc83326587;     dlBrrFNwcTXxdokOGJGc83326587 = dlBrrFNwcTXxdokOGJGc26002550;     dlBrrFNwcTXxdokOGJGc26002550 = dlBrrFNwcTXxdokOGJGc29653185;     dlBrrFNwcTXxdokOGJGc29653185 = dlBrrFNwcTXxdokOGJGc47689122;     dlBrrFNwcTXxdokOGJGc47689122 = dlBrrFNwcTXxdokOGJGc67902955;     dlBrrFNwcTXxdokOGJGc67902955 = dlBrrFNwcTXxdokOGJGc16261316;     dlBrrFNwcTXxdokOGJGc16261316 = dlBrrFNwcTXxdokOGJGc88936456;     dlBrrFNwcTXxdokOGJGc88936456 = dlBrrFNwcTXxdokOGJGc31069398;     dlBrrFNwcTXxdokOGJGc31069398 = dlBrrFNwcTXxdokOGJGc96257074;     dlBrrFNwcTXxdokOGJGc96257074 = dlBrrFNwcTXxdokOGJGc31420347;     dlBrrFNwcTXxdokOGJGc31420347 = dlBrrFNwcTXxdokOGJGc64754272;     dlBrrFNwcTXxdokOGJGc64754272 = dlBrrFNwcTXxdokOGJGc6004146;     dlBrrFNwcTXxdokOGJGc6004146 = dlBrrFNwcTXxdokOGJGc57094666;     dlBrrFNwcTXxdokOGJGc57094666 = dlBrrFNwcTXxdokOGJGc10143379;     dlBrrFNwcTXxdokOGJGc10143379 = dlBrrFNwcTXxdokOGJGc83543706;     dlBrrFNwcTXxdokOGJGc83543706 = dlBrrFNwcTXxdokOGJGc42798445;     dlBrrFNwcTXxdokOGJGc42798445 = dlBrrFNwcTXxdokOGJGc62871239;     dlBrrFNwcTXxdokOGJGc62871239 = dlBrrFNwcTXxdokOGJGc64788891;     dlBrrFNwcTXxdokOGJGc64788891 = dlBrrFNwcTXxdokOGJGc13532881;     dlBrrFNwcTXxdokOGJGc13532881 = dlBrrFNwcTXxdokOGJGc2313147;     dlBrrFNwcTXxdokOGJGc2313147 = dlBrrFNwcTXxdokOGJGc81261700;     dlBrrFNwcTXxdokOGJGc81261700 = dlBrrFNwcTXxdokOGJGc16884742;     dlBrrFNwcTXxdokOGJGc16884742 = dlBrrFNwcTXxdokOGJGc25887353;     dlBrrFNwcTXxdokOGJGc25887353 = dlBrrFNwcTXxdokOGJGc39585478;     dlBrrFNwcTXxdokOGJGc39585478 = dlBrrFNwcTXxdokOGJGc58998029;     dlBrrFNwcTXxdokOGJGc58998029 = dlBrrFNwcTXxdokOGJGc99492643;     dlBrrFNwcTXxdokOGJGc99492643 = dlBrrFNwcTXxdokOGJGc13472826;     dlBrrFNwcTXxdokOGJGc13472826 = dlBrrFNwcTXxdokOGJGc30380278;     dlBrrFNwcTXxdokOGJGc30380278 = dlBrrFNwcTXxdokOGJGc11019858;     dlBrrFNwcTXxdokOGJGc11019858 = dlBrrFNwcTXxdokOGJGc89458382;     dlBrrFNwcTXxdokOGJGc89458382 = dlBrrFNwcTXxdokOGJGc41218271;     dlBrrFNwcTXxdokOGJGc41218271 = dlBrrFNwcTXxdokOGJGc46049560;     dlBrrFNwcTXxdokOGJGc46049560 = dlBrrFNwcTXxdokOGJGc57825334;     dlBrrFNwcTXxdokOGJGc57825334 = dlBrrFNwcTXxdokOGJGc63295982;     dlBrrFNwcTXxdokOGJGc63295982 = dlBrrFNwcTXxdokOGJGc75182166;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BYamYoeSeigZzpfIhdzD68144566() {     float RzQAntuTAbXdbvADxUZR75175695 = -532961002;    float RzQAntuTAbXdbvADxUZR84189764 = -80665820;    float RzQAntuTAbXdbvADxUZR12739007 = -666148823;    float RzQAntuTAbXdbvADxUZR48174734 = -524362305;    float RzQAntuTAbXdbvADxUZR55701811 = -167677555;    float RzQAntuTAbXdbvADxUZR68127679 = -120665724;    float RzQAntuTAbXdbvADxUZR69315981 = 38134369;    float RzQAntuTAbXdbvADxUZR65734048 = -340078996;    float RzQAntuTAbXdbvADxUZR47775828 = -683077315;    float RzQAntuTAbXdbvADxUZR19553213 = -506543346;    float RzQAntuTAbXdbvADxUZR50691323 = -772496456;    float RzQAntuTAbXdbvADxUZR13539058 = -607490544;    float RzQAntuTAbXdbvADxUZR27005302 = -482650678;    float RzQAntuTAbXdbvADxUZR4834939 = 69580613;    float RzQAntuTAbXdbvADxUZR69498661 = -233152939;    float RzQAntuTAbXdbvADxUZR48055521 = -324981543;    float RzQAntuTAbXdbvADxUZR24342432 = -815085610;    float RzQAntuTAbXdbvADxUZR92184602 = -508317524;    float RzQAntuTAbXdbvADxUZR96935435 = -857954892;    float RzQAntuTAbXdbvADxUZR21072183 = -931834839;    float RzQAntuTAbXdbvADxUZR97892760 = -670007585;    float RzQAntuTAbXdbvADxUZR71726376 = -779131580;    float RzQAntuTAbXdbvADxUZR64856442 = -357690674;    float RzQAntuTAbXdbvADxUZR48980528 = -990175753;    float RzQAntuTAbXdbvADxUZR52782032 = -451091831;    float RzQAntuTAbXdbvADxUZR13568452 = -155692503;    float RzQAntuTAbXdbvADxUZR69125919 = -170126328;    float RzQAntuTAbXdbvADxUZR72889206 = -964849018;    float RzQAntuTAbXdbvADxUZR63613726 = -325525369;    float RzQAntuTAbXdbvADxUZR58499256 = -600645761;    float RzQAntuTAbXdbvADxUZR92177812 = -394870395;    float RzQAntuTAbXdbvADxUZR30154803 = -86069923;    float RzQAntuTAbXdbvADxUZR1373217 = -671977288;    float RzQAntuTAbXdbvADxUZR69432675 = -944896211;    float RzQAntuTAbXdbvADxUZR3695572 = -387299896;    float RzQAntuTAbXdbvADxUZR19465621 = -22714284;    float RzQAntuTAbXdbvADxUZR17529387 = -919850274;    float RzQAntuTAbXdbvADxUZR22146036 = -599857570;    float RzQAntuTAbXdbvADxUZR15561209 = -100681385;    float RzQAntuTAbXdbvADxUZR17310660 = 60662911;    float RzQAntuTAbXdbvADxUZR40597013 = -705170126;    float RzQAntuTAbXdbvADxUZR72500650 = -664011706;    float RzQAntuTAbXdbvADxUZR88959198 = -303966856;    float RzQAntuTAbXdbvADxUZR81205798 = -402467093;    float RzQAntuTAbXdbvADxUZR32253806 = -124794761;    float RzQAntuTAbXdbvADxUZR54623932 = -291054175;    float RzQAntuTAbXdbvADxUZR41148461 = -925869339;    float RzQAntuTAbXdbvADxUZR31489474 = -675293935;    float RzQAntuTAbXdbvADxUZR93272831 = -959140720;    float RzQAntuTAbXdbvADxUZR6191812 = -373620560;    float RzQAntuTAbXdbvADxUZR82135875 = -699041762;    float RzQAntuTAbXdbvADxUZR70999245 = -717498867;    float RzQAntuTAbXdbvADxUZR39617594 = -45536227;    float RzQAntuTAbXdbvADxUZR9286323 = -476030789;    float RzQAntuTAbXdbvADxUZR70251806 = -101102264;    float RzQAntuTAbXdbvADxUZR3449319 = -753829422;    float RzQAntuTAbXdbvADxUZR19333322 = -722975147;    float RzQAntuTAbXdbvADxUZR63758478 = -675973071;    float RzQAntuTAbXdbvADxUZR95392701 = 26729525;    float RzQAntuTAbXdbvADxUZR42133360 = 88014947;    float RzQAntuTAbXdbvADxUZR99001760 = -950539397;    float RzQAntuTAbXdbvADxUZR96426774 = 2983387;    float RzQAntuTAbXdbvADxUZR2120322 = 85446372;    float RzQAntuTAbXdbvADxUZR89276572 = 17568446;    float RzQAntuTAbXdbvADxUZR27375401 = -11672952;    float RzQAntuTAbXdbvADxUZR20536520 = -586426534;    float RzQAntuTAbXdbvADxUZR12165841 = -935513257;    float RzQAntuTAbXdbvADxUZR57572626 = -537754468;    float RzQAntuTAbXdbvADxUZR1139367 = -543119492;    float RzQAntuTAbXdbvADxUZR50033041 = -110438656;    float RzQAntuTAbXdbvADxUZR30526135 = -405131270;    float RzQAntuTAbXdbvADxUZR2196396 = -115228041;    float RzQAntuTAbXdbvADxUZR76623393 = -307636140;    float RzQAntuTAbXdbvADxUZR79624775 = -818617803;    float RzQAntuTAbXdbvADxUZR80475169 = -126664714;    float RzQAntuTAbXdbvADxUZR25392111 = 94004120;    float RzQAntuTAbXdbvADxUZR82767178 = -375164724;    float RzQAntuTAbXdbvADxUZR83650644 = -955223581;    float RzQAntuTAbXdbvADxUZR16726722 = -765380992;    float RzQAntuTAbXdbvADxUZR98158100 = -60037657;    float RzQAntuTAbXdbvADxUZR72419990 = -229823165;    float RzQAntuTAbXdbvADxUZR37636446 = -494832393;    float RzQAntuTAbXdbvADxUZR79616375 = 94291701;    float RzQAntuTAbXdbvADxUZR57421915 = -951904809;    float RzQAntuTAbXdbvADxUZR76363380 = -901603999;    float RzQAntuTAbXdbvADxUZR21178568 = -677371529;    float RzQAntuTAbXdbvADxUZR90537209 = 59466304;    float RzQAntuTAbXdbvADxUZR92086893 = -95946500;    float RzQAntuTAbXdbvADxUZR99180869 = -743793947;    float RzQAntuTAbXdbvADxUZR246254 = -633470474;    float RzQAntuTAbXdbvADxUZR132299 = -299739137;    float RzQAntuTAbXdbvADxUZR53770908 = -143877204;    float RzQAntuTAbXdbvADxUZR26753335 = -526587096;    float RzQAntuTAbXdbvADxUZR73427849 = -88696333;    float RzQAntuTAbXdbvADxUZR18308900 = 11202307;    float RzQAntuTAbXdbvADxUZR44170239 = -608153513;    float RzQAntuTAbXdbvADxUZR70380328 = -649458078;    float RzQAntuTAbXdbvADxUZR99682625 = -221535303;    float RzQAntuTAbXdbvADxUZR53830398 = -290794142;    float RzQAntuTAbXdbvADxUZR11717287 = -532961002;     RzQAntuTAbXdbvADxUZR75175695 = RzQAntuTAbXdbvADxUZR84189764;     RzQAntuTAbXdbvADxUZR84189764 = RzQAntuTAbXdbvADxUZR12739007;     RzQAntuTAbXdbvADxUZR12739007 = RzQAntuTAbXdbvADxUZR48174734;     RzQAntuTAbXdbvADxUZR48174734 = RzQAntuTAbXdbvADxUZR55701811;     RzQAntuTAbXdbvADxUZR55701811 = RzQAntuTAbXdbvADxUZR68127679;     RzQAntuTAbXdbvADxUZR68127679 = RzQAntuTAbXdbvADxUZR69315981;     RzQAntuTAbXdbvADxUZR69315981 = RzQAntuTAbXdbvADxUZR65734048;     RzQAntuTAbXdbvADxUZR65734048 = RzQAntuTAbXdbvADxUZR47775828;     RzQAntuTAbXdbvADxUZR47775828 = RzQAntuTAbXdbvADxUZR19553213;     RzQAntuTAbXdbvADxUZR19553213 = RzQAntuTAbXdbvADxUZR50691323;     RzQAntuTAbXdbvADxUZR50691323 = RzQAntuTAbXdbvADxUZR13539058;     RzQAntuTAbXdbvADxUZR13539058 = RzQAntuTAbXdbvADxUZR27005302;     RzQAntuTAbXdbvADxUZR27005302 = RzQAntuTAbXdbvADxUZR4834939;     RzQAntuTAbXdbvADxUZR4834939 = RzQAntuTAbXdbvADxUZR69498661;     RzQAntuTAbXdbvADxUZR69498661 = RzQAntuTAbXdbvADxUZR48055521;     RzQAntuTAbXdbvADxUZR48055521 = RzQAntuTAbXdbvADxUZR24342432;     RzQAntuTAbXdbvADxUZR24342432 = RzQAntuTAbXdbvADxUZR92184602;     RzQAntuTAbXdbvADxUZR92184602 = RzQAntuTAbXdbvADxUZR96935435;     RzQAntuTAbXdbvADxUZR96935435 = RzQAntuTAbXdbvADxUZR21072183;     RzQAntuTAbXdbvADxUZR21072183 = RzQAntuTAbXdbvADxUZR97892760;     RzQAntuTAbXdbvADxUZR97892760 = RzQAntuTAbXdbvADxUZR71726376;     RzQAntuTAbXdbvADxUZR71726376 = RzQAntuTAbXdbvADxUZR64856442;     RzQAntuTAbXdbvADxUZR64856442 = RzQAntuTAbXdbvADxUZR48980528;     RzQAntuTAbXdbvADxUZR48980528 = RzQAntuTAbXdbvADxUZR52782032;     RzQAntuTAbXdbvADxUZR52782032 = RzQAntuTAbXdbvADxUZR13568452;     RzQAntuTAbXdbvADxUZR13568452 = RzQAntuTAbXdbvADxUZR69125919;     RzQAntuTAbXdbvADxUZR69125919 = RzQAntuTAbXdbvADxUZR72889206;     RzQAntuTAbXdbvADxUZR72889206 = RzQAntuTAbXdbvADxUZR63613726;     RzQAntuTAbXdbvADxUZR63613726 = RzQAntuTAbXdbvADxUZR58499256;     RzQAntuTAbXdbvADxUZR58499256 = RzQAntuTAbXdbvADxUZR92177812;     RzQAntuTAbXdbvADxUZR92177812 = RzQAntuTAbXdbvADxUZR30154803;     RzQAntuTAbXdbvADxUZR30154803 = RzQAntuTAbXdbvADxUZR1373217;     RzQAntuTAbXdbvADxUZR1373217 = RzQAntuTAbXdbvADxUZR69432675;     RzQAntuTAbXdbvADxUZR69432675 = RzQAntuTAbXdbvADxUZR3695572;     RzQAntuTAbXdbvADxUZR3695572 = RzQAntuTAbXdbvADxUZR19465621;     RzQAntuTAbXdbvADxUZR19465621 = RzQAntuTAbXdbvADxUZR17529387;     RzQAntuTAbXdbvADxUZR17529387 = RzQAntuTAbXdbvADxUZR22146036;     RzQAntuTAbXdbvADxUZR22146036 = RzQAntuTAbXdbvADxUZR15561209;     RzQAntuTAbXdbvADxUZR15561209 = RzQAntuTAbXdbvADxUZR17310660;     RzQAntuTAbXdbvADxUZR17310660 = RzQAntuTAbXdbvADxUZR40597013;     RzQAntuTAbXdbvADxUZR40597013 = RzQAntuTAbXdbvADxUZR72500650;     RzQAntuTAbXdbvADxUZR72500650 = RzQAntuTAbXdbvADxUZR88959198;     RzQAntuTAbXdbvADxUZR88959198 = RzQAntuTAbXdbvADxUZR81205798;     RzQAntuTAbXdbvADxUZR81205798 = RzQAntuTAbXdbvADxUZR32253806;     RzQAntuTAbXdbvADxUZR32253806 = RzQAntuTAbXdbvADxUZR54623932;     RzQAntuTAbXdbvADxUZR54623932 = RzQAntuTAbXdbvADxUZR41148461;     RzQAntuTAbXdbvADxUZR41148461 = RzQAntuTAbXdbvADxUZR31489474;     RzQAntuTAbXdbvADxUZR31489474 = RzQAntuTAbXdbvADxUZR93272831;     RzQAntuTAbXdbvADxUZR93272831 = RzQAntuTAbXdbvADxUZR6191812;     RzQAntuTAbXdbvADxUZR6191812 = RzQAntuTAbXdbvADxUZR82135875;     RzQAntuTAbXdbvADxUZR82135875 = RzQAntuTAbXdbvADxUZR70999245;     RzQAntuTAbXdbvADxUZR70999245 = RzQAntuTAbXdbvADxUZR39617594;     RzQAntuTAbXdbvADxUZR39617594 = RzQAntuTAbXdbvADxUZR9286323;     RzQAntuTAbXdbvADxUZR9286323 = RzQAntuTAbXdbvADxUZR70251806;     RzQAntuTAbXdbvADxUZR70251806 = RzQAntuTAbXdbvADxUZR3449319;     RzQAntuTAbXdbvADxUZR3449319 = RzQAntuTAbXdbvADxUZR19333322;     RzQAntuTAbXdbvADxUZR19333322 = RzQAntuTAbXdbvADxUZR63758478;     RzQAntuTAbXdbvADxUZR63758478 = RzQAntuTAbXdbvADxUZR95392701;     RzQAntuTAbXdbvADxUZR95392701 = RzQAntuTAbXdbvADxUZR42133360;     RzQAntuTAbXdbvADxUZR42133360 = RzQAntuTAbXdbvADxUZR99001760;     RzQAntuTAbXdbvADxUZR99001760 = RzQAntuTAbXdbvADxUZR96426774;     RzQAntuTAbXdbvADxUZR96426774 = RzQAntuTAbXdbvADxUZR2120322;     RzQAntuTAbXdbvADxUZR2120322 = RzQAntuTAbXdbvADxUZR89276572;     RzQAntuTAbXdbvADxUZR89276572 = RzQAntuTAbXdbvADxUZR27375401;     RzQAntuTAbXdbvADxUZR27375401 = RzQAntuTAbXdbvADxUZR20536520;     RzQAntuTAbXdbvADxUZR20536520 = RzQAntuTAbXdbvADxUZR12165841;     RzQAntuTAbXdbvADxUZR12165841 = RzQAntuTAbXdbvADxUZR57572626;     RzQAntuTAbXdbvADxUZR57572626 = RzQAntuTAbXdbvADxUZR1139367;     RzQAntuTAbXdbvADxUZR1139367 = RzQAntuTAbXdbvADxUZR50033041;     RzQAntuTAbXdbvADxUZR50033041 = RzQAntuTAbXdbvADxUZR30526135;     RzQAntuTAbXdbvADxUZR30526135 = RzQAntuTAbXdbvADxUZR2196396;     RzQAntuTAbXdbvADxUZR2196396 = RzQAntuTAbXdbvADxUZR76623393;     RzQAntuTAbXdbvADxUZR76623393 = RzQAntuTAbXdbvADxUZR79624775;     RzQAntuTAbXdbvADxUZR79624775 = RzQAntuTAbXdbvADxUZR80475169;     RzQAntuTAbXdbvADxUZR80475169 = RzQAntuTAbXdbvADxUZR25392111;     RzQAntuTAbXdbvADxUZR25392111 = RzQAntuTAbXdbvADxUZR82767178;     RzQAntuTAbXdbvADxUZR82767178 = RzQAntuTAbXdbvADxUZR83650644;     RzQAntuTAbXdbvADxUZR83650644 = RzQAntuTAbXdbvADxUZR16726722;     RzQAntuTAbXdbvADxUZR16726722 = RzQAntuTAbXdbvADxUZR98158100;     RzQAntuTAbXdbvADxUZR98158100 = RzQAntuTAbXdbvADxUZR72419990;     RzQAntuTAbXdbvADxUZR72419990 = RzQAntuTAbXdbvADxUZR37636446;     RzQAntuTAbXdbvADxUZR37636446 = RzQAntuTAbXdbvADxUZR79616375;     RzQAntuTAbXdbvADxUZR79616375 = RzQAntuTAbXdbvADxUZR57421915;     RzQAntuTAbXdbvADxUZR57421915 = RzQAntuTAbXdbvADxUZR76363380;     RzQAntuTAbXdbvADxUZR76363380 = RzQAntuTAbXdbvADxUZR21178568;     RzQAntuTAbXdbvADxUZR21178568 = RzQAntuTAbXdbvADxUZR90537209;     RzQAntuTAbXdbvADxUZR90537209 = RzQAntuTAbXdbvADxUZR92086893;     RzQAntuTAbXdbvADxUZR92086893 = RzQAntuTAbXdbvADxUZR99180869;     RzQAntuTAbXdbvADxUZR99180869 = RzQAntuTAbXdbvADxUZR246254;     RzQAntuTAbXdbvADxUZR246254 = RzQAntuTAbXdbvADxUZR132299;     RzQAntuTAbXdbvADxUZR132299 = RzQAntuTAbXdbvADxUZR53770908;     RzQAntuTAbXdbvADxUZR53770908 = RzQAntuTAbXdbvADxUZR26753335;     RzQAntuTAbXdbvADxUZR26753335 = RzQAntuTAbXdbvADxUZR73427849;     RzQAntuTAbXdbvADxUZR73427849 = RzQAntuTAbXdbvADxUZR18308900;     RzQAntuTAbXdbvADxUZR18308900 = RzQAntuTAbXdbvADxUZR44170239;     RzQAntuTAbXdbvADxUZR44170239 = RzQAntuTAbXdbvADxUZR70380328;     RzQAntuTAbXdbvADxUZR70380328 = RzQAntuTAbXdbvADxUZR99682625;     RzQAntuTAbXdbvADxUZR99682625 = RzQAntuTAbXdbvADxUZR53830398;     RzQAntuTAbXdbvADxUZR53830398 = RzQAntuTAbXdbvADxUZR11717287;     RzQAntuTAbXdbvADxUZR11717287 = RzQAntuTAbXdbvADxUZR75175695;}
// Junk Finished
