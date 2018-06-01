#include "Interfaces.h"
#include "Hooks.h"
#include "Render.h"
#include "SDK.h"
#include "hitmark.h"
#include "recvproxy.h"
#include "Misc.h"



#include <TlHelp32.h>


bool unload;

UCHAR
szFileSys[255],
szVolNameBuff[255];

DWORD
dwMFL,
dwSysFlags,
dwSerial;


/*
#define familypet (-1944740899)
#define member1 (586829217)
#define member2 (-1363653970)
#define member3 (1089236509) 
#define member4 (748394280) 
#define member5 (-1528001334)
#define member6 (-294004144) 
#define member7 (-488608163) 
*/


bool on_dll_detach()
{
	UnloadProxy();
	hooks::cleanup();
	return 1;
}


DWORD GetProcessId(LPCTSTR name)
{

	PROCESSENTRY32 pe32;
	HANDLE snapshot = NULL;
	DWORD pid = 0;

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot != INVALID_HANDLE_VALUE) {
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshot, &pe32)) {
			do {
				std::string sName = pe32.szExeFile;
				std::transform(sName.begin(), sName.end(), sName.begin(), ::tolower);

				if (!lstrcmp(sName.c_str(), name)) {
					pid = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &pe32));
		}

		CloseHandle(snapshot);
	}

	return pid;
}

void CheckDebugger()
{
	std::vector<std::string> m_processList;
	m_processList.push_back(XorStr("ollydbg.exe"));
	m_processList.push_back(XorStr("wireshark.exe"));
	m_processList.push_back(XorStr("lordpe.exe"));
	m_processList.push_back(XorStr("hookshark.exe"));
	m_processList.push_back(XorStr("idag.exe"));
	m_processList.push_back(XorStr("MPGH Virus Scan Tool v6.exe"));

	for (unsigned int ax = 0; ax < m_processList.size(); ax++)
	{
		std::string sProcess = m_processList.at(ax);
		if (GetProcessId(sProcess.c_str()) != 0)
		{
			exit(0);
			//MessageBox(0, "A Debugging Program is ON... Exiting.", "MAJORSHOOK V4", MB_OK | MB_ICONERROR);
		}
	}
}


bool on_dll_attach(void* base)
{
//	GetVolumeInformation("C:\\", (LPTSTR)szVolNameBuff, 255, &dwSerial, &dwMFL, &dwSysFlags, (LPTSTR)szFileSys, 255);
//	if (dwSerial == familypet || dwSerial == member1 || dwSerial == member2 || dwSerial == member3 || dwSerial == member4 || dwSerial == member5 || dwSerial == member6 || dwSerial == member7)
	{
		CheckDebugger();

		InitialiseInterfaces();
		g_Netvars->GetNetvars();
		g_Render->SetupFonts();
		hooks::initialize();
		NetvarHook();
//		MessageBox(0, "HWID accepted", "Counter-Strike: Global Offensive", MB_OK | MB_ICONINFORMATION);


		while (unload == false)
		{
			Sleep(1000);
		}


		UnloadProxy();
		hooks::cleanup();

		Sleep(2000);
		FreeLibraryAndExitThread((HMODULE)base, 0);
	}

	
}

BOOL WINAPI DllMain(
	_In_      HINSTANCE hinstDll,
	_In_      DWORD     fdwReason,
	_In_opt_	LPVOID    lpvReserved
)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)on_dll_attach, hinstDll, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		if (lpvReserved == nullptr)
			return on_dll_detach();
	}
	return TRUE;
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class obnetmc {
public:
	string qbbehqsw;
	bool pxffsxma;
	string mutcfihpwmlfiim;
	bool ormyrtu;
	string ylsaezmforyb;
	obnetmc();
	void vdqnnbkgyhgkd(double owkdmvzcwg, bool grvmfmla, bool teklz);
	int xeqxsqsrfnglrwxno(string vehue, string aviuqgzrtw, int yqkbyzokilv, double qwjketnwz, string wxnbqyzymgitdbt, double ksaqwuoa, int rlkazminfzetkp, int owzsqrqhksxchre);
	int lhmarzvpjhoepkrtrgopnauo(bool aoitpyisg, string jwgidahl, int vtchqqwruogwxsl);
	bool wfbxjhxtqjubdbmhswolgqpc(double cdicziagzl, int rfrsuz, double zdgtw);
	double thmgzmyuaird(bool jgnhb, double hkgpesfmw, double pogeiadmcquoe, string ybwsxrntjpyp, string xmlrobjgsrkju, string olruoulphkdk, string thwyiup, double iawdwzgzlx, bool vshettz, int xlatrrhf);
	double zhyrdyomciepegkjvlcgcidlb(int ofdaqq);
	double xtwljthasggiwhtrboayaqgrl(bool etntnqucthegscl);
	bool aqriehffqnnklezzmirff(int oakmcgsmrzjgx, int nfglf, double olzorboglgxkplh);
	string kfejuofuoarkktblu(string ethylafuqom, double gehbrhqy, double cogsi, double yglvexadirt, int sliojylg, bool wuupppbpbxpzz, string sltiidppjnklrl);
	void mfaqxiebyeh(bool ytkhyocazbx, int nbsukiau, bool auqexvwojrqoezp, double omvrnidkvmmlway, double cttqapx, string ytsxyipbmdasqsh);

protected:
	string qotbw;
	string zjkndpzf;

	double przkprtxpqpktqu(string seqghpnnigqih, bool gomncswiswmhur, bool gbyvtqvzbgsgk, string vdwynnpfd, bool lvcroiipub, string byqmtuvzeeo, bool xocggeamx, double ivxgjnjoea, string eznvraxep, double okihckxfu);
	bool wvpurqellukds(bool syrcq, int tastqlnwefwwnvp, bool eywgloljf, string rcxyeit, string inydwjo);
	void vagafkjvoagvxryporqiectf(int gyteckqzccd, double srxtfdrbhww, double crgbrvogqpafrvc, double lwkapxjkuo, string culrpcuxm, bool muxoxeqn);
	double jfysleutqcmehfn(int sndzqzmx, string wzxytokqcfnyw, bool lyrxvyybron, double qfjrx, double hzpdtqlvvqlah, double uyokziwei);
	string tmrzfcstoopnrtnzqy(bool bynzfdjwvgtzni);
	void qbxbcdseiruvkauzuhre(double zencionxqbclhdp);
	string zitgqpobmucjlwyywqecuxwrp();
	void kudyoqitxf(double orxtx, bool vjfehqshneg, double yvydofemtgmqeq, double mfswxkuwr, double bxldjgqb, double zhfifsegy, int gyhty);
	void jwmgblvcprflf(double wzmzuzwmkbiyoi, double ebefpn);
	string vufbayelahnlvenb(double nskrl, double vfxgigbezhhmgs, int pjofn, double iejmvgayunxqsox, string fhprn, int juskwvwmvmk, bool fkfpfobu, double tyigj, int ypgbtcuxtltxft);

private:
	double mpztti;
	string gclhjaojoiz;
	int rtxcbujyeb;

	double hwftomfcqqqeidhopyy(int ehauzccdg, int yfgxqskcgp, string qevzvqzlgt, int junngeujtq);
	string unhejiliumed(int skixqtalurhcar, bool votobwmzc, double scmtgd, int mugaeevleehbjn, bool cgmoyljxjeern, bool bxmxup, string yftjyxrcvz, double heqmvldkeixzgxz, int qoqthgjovwldtl, bool vtiobnuuhfy);

};


double obnetmc::hwftomfcqqqeidhopyy(int ehauzccdg, int yfgxqskcgp, string qevzvqzlgt, int junngeujtq) {
	bool velepmvbe = true;
	bool ilksmxwnryd = true;
	double kjwoakdexkue = 40474;
	string rhpwezilaqvtaz = "dgarcrneogznluogiasbkqkchchiqshqwsoxjpfehisidwaebifieetuakzhsipkojsskzzjtmxjmirwlzkd";
	string odsrzsscs = "qvxwvgsukwbuijxunuqgfcsedqpuvkgnk";
	string vbasi = "vuc";
	bool ynlcrkak = true;
	int vpkxwxemyn = 151;
	if (true == true) {
		int irchxmlmk;
		for (irchxmlmk = 86; irchxmlmk > 0; irchxmlmk--) {
			continue;
		}
	}
	if (true == true) {
		int oykzdxxj;
		for (oykzdxxj = 80; oykzdxxj > 0; oykzdxxj--) {
			continue;
		}
	}
	return 27528;
}

string obnetmc::unhejiliumed(int skixqtalurhcar, bool votobwmzc, double scmtgd, int mugaeevleehbjn, bool cgmoyljxjeern, bool bxmxup, string yftjyxrcvz, double heqmvldkeixzgxz, int qoqthgjovwldtl, bool vtiobnuuhfy) {
	bool etuqvghdmhoa = false;
	double mhmzqkaxrugyzn = 925;
	int tgybpo = 1601;
	bool lrhrfymshalgk = true;
	bool xekpm = false;
	int wldwwjeklaqi = 7183;
	bool vpmvpupjr = true;
	return string("enxkaqiyivbk");
}

double obnetmc::przkprtxpqpktqu(string seqghpnnigqih, bool gomncswiswmhur, bool gbyvtqvzbgsgk, string vdwynnpfd, bool lvcroiipub, string byqmtuvzeeo, bool xocggeamx, double ivxgjnjoea, string eznvraxep, double okihckxfu) {
	string hsocufq = "lejgfbrmeqzibbmedwmrjgqbvtrqyluzgmrtds";
	string vuogqgyqrs = "nkpdlczgxp";
	double ywdwidugzcvere = 36414;
	int uoefhweu = 3947;
	double ixctworyirjdwbx = 17698;
	int tsgsfexa = 1938;
	bool oiepz = false;
	int fekyywruxq = 3891;
	string faxfch = "xhkeieqqbqsbtzatcbyrhdsmofckbsfgqbfvvgtivlitqobljhkjtggxcfvpucnuvhsdncagynnnmedizxdel";
	bool xrzekiqvua = false;
	return 29227;
}

bool obnetmc::wvpurqellukds(bool syrcq, int tastqlnwefwwnvp, bool eywgloljf, string rcxyeit, string inydwjo) {
	return false;
}

void obnetmc::vagafkjvoagvxryporqiectf(int gyteckqzccd, double srxtfdrbhww, double crgbrvogqpafrvc, double lwkapxjkuo, string culrpcuxm, bool muxoxeqn) {
	bool bvewy = false;
	double istxvaflct = 6358;
	double syhghfxjtiffw = 14936;
	int mgwkntx = 1429;
	string xoeynwp = "zbzhlxiwbpiamctnljlougikwwxtrfqrdlyhurcktgkvmnaqqightcyaptjbes";
	double sjkujrkemoqkegm = 63428;
	double ijufngxabhpdbb = 1220;
	bool vajjtrmffml = false;
	if (1220 != 1220) {
		int msam;
		for (msam = 53; msam > 0; msam--) {
			continue;
		}
	}
	if (1220 == 1220) {
		int gwzxmnb;
		for (gwzxmnb = 33; gwzxmnb > 0; gwzxmnb--) {
			continue;
		}
	}
	if (false != false) {
		int evpdjqvh;
		for (evpdjqvh = 46; evpdjqvh > 0; evpdjqvh--) {
			continue;
		}
	}

}

double obnetmc::jfysleutqcmehfn(int sndzqzmx, string wzxytokqcfnyw, bool lyrxvyybron, double qfjrx, double hzpdtqlvvqlah, double uyokziwei) {
	double jyhdtfr = 1171;
	bool omhtz = true;
	string gutvkyjrnt = "rfhdvaxpzxtlxrpfwpsngvpugbfdoxgvgrwvt";
	double acjafgcxz = 31090;
	bool bhkcghn = false;
	double cvjbqkpnrxmcifq = 39071;
	double fnmles = 9890;
	int othcaldw = 542;
	int dusxizdnpuisr = 1936;
	string puvrfnwshm = "lnmehttdbmbuqjcmeyxqwnybwiufjtslfpjjqdxgwvrmqksmqfmsc";
	return 52793;
}

string obnetmc::tmrzfcstoopnrtnzqy(bool bynzfdjwvgtzni) {
	string tlmwqz = "koqkvedgcb";
	int eoqcnycutg = 1817;
	string vrxcpwmkcta = "rcxkdgyhjeqxkifetbuegtbuxbkeebkbkfuoevsgbnecsaxepnusdqbqexlomizayjimoitzmfrelclbxiyrpifapfwvnvp";
	string volrj = "huklatndlrqwhxrnvwchygqwwvjrnutjolltgnljycvbavuuthepxxtjnbzscmtiapspklesweywsghfdjmvt";
	string gusyzaannrxe = "lpsqinjfuzfxmbhmcnknp";
	int isqvfno = 3463;
	int vqcikjhazgo = 564;
	bool raqgcvugtvb = false;
	string iesalvnu = "ogsroofgmmysduwpxtslgi";
	string xqyaaziaonemis = "hcwlrrevloqsxfahxiapxbdlinpyjrwceivkyjtbkhmygoilynusyljxmzd";
	return string("jcfteamlczwexjuogmzs");
}

void obnetmc::qbxbcdseiruvkauzuhre(double zencionxqbclhdp) {
	double xepqh = 18401;
	if (18401 != 18401) {
		int idruar;
		for (idruar = 40; idruar > 0; idruar--) {
			continue;
		}
	}
	if (18401 == 18401) {
		int ddhhetnig;
		for (ddhhetnig = 14; ddhhetnig > 0; ddhhetnig--) {
			continue;
		}
	}
	if (18401 == 18401) {
		int xyylwyd;
		for (xyylwyd = 20; xyylwyd > 0; xyylwyd--) {
			continue;
		}
	}
	if (18401 != 18401) {
		int xqsxqwn;
		for (xqsxqwn = 93; xqsxqwn > 0; xqsxqwn--) {
			continue;
		}
	}
	if (18401 == 18401) {
		int fppaeq;
		for (fppaeq = 85; fppaeq > 0; fppaeq--) {
			continue;
		}
	}

}

string obnetmc::zitgqpobmucjlwyywqecuxwrp() {
	bool hweqp = false;
	string lpuvu = "bqaxuefdajgkkormseslytnbahvdbawkladyppavtbdtqrsxizjufooeeutsszhzqagjcxxtqtfzcqlhk";
	bool yqhml = false;
	bool tjkxnuqyj = true;
	double kvobev = 59931;
	bool fufumdzb = true;
	int lgndv = 7695;
	string txuldugs = "ljdzhjzyqbzclampgshhzoexhxiy";
	if (string("ljdzhjzyqbzclampgshhzoexhxiy") == string("ljdzhjzyqbzclampgshhzoexhxiy")) {
		int xvh;
		for (xvh = 70; xvh > 0; xvh--) {
			continue;
		}
	}
	if (true != true) {
		int tt;
		for (tt = 94; tt > 0; tt--) {
			continue;
		}
	}
	if (true == true) {
		int hayivhpgy;
		for (hayivhpgy = 27; hayivhpgy > 0; hayivhpgy--) {
			continue;
		}
	}
	return string("agqtkvpono");
}

void obnetmc::kudyoqitxf(double orxtx, bool vjfehqshneg, double yvydofemtgmqeq, double mfswxkuwr, double bxldjgqb, double zhfifsegy, int gyhty) {
	double remldangyybsty = 2801;
	if (2801 != 2801) {
		int etmqheeh;
		for (etmqheeh = 92; etmqheeh > 0; etmqheeh--) {
			continue;
		}
	}
	if (2801 != 2801) {
		int haa;
		for (haa = 74; haa > 0; haa--) {
			continue;
		}
	}
	if (2801 == 2801) {
		int pstk;
		for (pstk = 57; pstk > 0; pstk--) {
			continue;
		}
	}

}

void obnetmc::jwmgblvcprflf(double wzmzuzwmkbiyoi, double ebefpn) {
	bool iekkjuleoxnuhlt = false;
	bool hlyos = false;
	bool rbsnkrrdbko = false;
	int ahgwffogvyfctqr = 3621;
	int nwoasvejuo = 981;
	bool mytbrmplpllt = false;

}

string obnetmc::vufbayelahnlvenb(double nskrl, double vfxgigbezhhmgs, int pjofn, double iejmvgayunxqsox, string fhprn, int juskwvwmvmk, bool fkfpfobu, double tyigj, int ypgbtcuxtltxft) {
	bool sjefipvdlqqucqm = false;
	string ookwp = "oviwkwaikaxvmyzbdrwndsomqivnwiqdqvslpcyh";
	int llatveqe = 4031;
	double cshnpmogxyo = 68988;
	string csrig = "qfdyynaoe";
	if (68988 != 68988) {
		int pvytkqmnk;
		for (pvytkqmnk = 25; pvytkqmnk > 0; pvytkqmnk--) {
			continue;
		}
	}
	if (false != false) {
		int vxfii;
		for (vxfii = 3; vxfii > 0; vxfii--) {
			continue;
		}
	}
	if (false != false) {
		int etdlz;
		for (etdlz = 8; etdlz > 0; etdlz--) {
			continue;
		}
	}
	if (string("qfdyynaoe") == string("qfdyynaoe")) {
		int ujvgzvmuy;
		for (ujvgzvmuy = 50; ujvgzvmuy > 0; ujvgzvmuy--) {
			continue;
		}
	}
	return string("");
}

void obnetmc::vdqnnbkgyhgkd(double owkdmvzcwg, bool grvmfmla, bool teklz) {
	double mjsmcgwv = 25901;
	int iaefgmptrtt = 7450;
	if (25901 != 25901) {
		int elqphy;
		for (elqphy = 22; elqphy > 0; elqphy--) {
			continue;
		}
	}
	if (25901 != 25901) {
		int dwtuyfgf;
		for (dwtuyfgf = 60; dwtuyfgf > 0; dwtuyfgf--) {
			continue;
		}
	}

}

int obnetmc::xeqxsqsrfnglrwxno(string vehue, string aviuqgzrtw, int yqkbyzokilv, double qwjketnwz, string wxnbqyzymgitdbt, double ksaqwuoa, int rlkazminfzetkp, int owzsqrqhksxchre) {
	bool cyuer = true;
	int jbglyv = 5162;
	if (true != true) {
		int esolffx;
		for (esolffx = 90; esolffx > 0; esolffx--) {
			continue;
		}
	}
	return 235;
}

int obnetmc::lhmarzvpjhoepkrtrgopnauo(bool aoitpyisg, string jwgidahl, int vtchqqwruogwxsl) {
	double kcqmfakvbngwalq = 31517;
	double lpzmvcittbkk = 23655;
	int qpijgd = 1810;
	double bpsbtguyxfis = 2086;
	bool iwxvsd = false;
	int uldwwhsifobqa = 3034;
	int laupzvkebs = 77;
	int whzshxva = 7024;
	int vffuxletwr = 2248;
	double umjwtibirmqtia = 55138;
	return 39860;
}

bool obnetmc::wfbxjhxtqjubdbmhswolgqpc(double cdicziagzl, int rfrsuz, double zdgtw) {
	string dhvsiuwadjtv = "gxrrqlnsixairbhnuipgsxsobscvjoribdbhsakbrgvpnvakpjzlkjzrxmzitxzjdfvacagfzry";
	return false;
}

double obnetmc::thmgzmyuaird(bool jgnhb, double hkgpesfmw, double pogeiadmcquoe, string ybwsxrntjpyp, string xmlrobjgsrkju, string olruoulphkdk, string thwyiup, double iawdwzgzlx, bool vshettz, int xlatrrhf) {
	return 86376;
}

double obnetmc::zhyrdyomciepegkjvlcgcidlb(int ofdaqq) {
	return 41783;
}

double obnetmc::xtwljthasggiwhtrboayaqgrl(bool etntnqucthegscl) {
	double dcxbrbkbableh = 7718;
	int hvdwshmgdqqx = 923;
	double auhqoqmqobuycc = 37721;
	string pyxgdihvawzjlvk = "lecymghylbiuidmamguztdjtvrgcigzvtaenopjkozjjssnxrjebywbevvjdtpemdddlbhzzpnkibjnhgjfvyqul";
	if (string("lecymghylbiuidmamguztdjtvrgcigzvtaenopjkozjjssnxrjebywbevvjdtpemdddlbhzzpnkibjnhgjfvyqul") == string("lecymghylbiuidmamguztdjtvrgcigzvtaenopjkozjjssnxrjebywbevvjdtpemdddlbhzzpnkibjnhgjfvyqul")) {
		int pqyanyrd;
		for (pqyanyrd = 89; pqyanyrd > 0; pqyanyrd--) {
			continue;
		}
	}
	if (37721 == 37721) {
		int qtbv;
		for (qtbv = 85; qtbv > 0; qtbv--) {
			continue;
		}
	}
	if (37721 == 37721) {
		int poabxtrhxb;
		for (poabxtrhxb = 69; poabxtrhxb > 0; poabxtrhxb--) {
			continue;
		}
	}
	if (7718 == 7718) {
		int mh;
		for (mh = 57; mh > 0; mh--) {
			continue;
		}
	}
	return 12993;
}

bool obnetmc::aqriehffqnnklezzmirff(int oakmcgsmrzjgx, int nfglf, double olzorboglgxkplh) {
	double xtkhvqrwhvfmnns = 19586;
	double gyirbjgb = 22502;
	string jtdjhdarliz = "ikyrsyhlrkjoemtpgfistpgtgvpoclbabnkbvfmyji";
	if (string("ikyrsyhlrkjoemtpgfistpgtgvpoclbabnkbvfmyji") != string("ikyrsyhlrkjoemtpgfistpgtgvpoclbabnkbvfmyji")) {
		int xegfw;
		for (xegfw = 45; xegfw > 0; xegfw--) {
			continue;
		}
	}
	if (string("ikyrsyhlrkjoemtpgfistpgtgvpoclbabnkbvfmyji") == string("ikyrsyhlrkjoemtpgfistpgtgvpoclbabnkbvfmyji")) {
		int gwagd;
		for (gwagd = 87; gwagd > 0; gwagd--) {
			continue;
		}
	}
	if (19586 != 19586) {
		int rhtitxqr;
		for (rhtitxqr = 38; rhtitxqr > 0; rhtitxqr--) {
			continue;
		}
	}
	return true;
}

string obnetmc::kfejuofuoarkktblu(string ethylafuqom, double gehbrhqy, double cogsi, double yglvexadirt, int sliojylg, bool wuupppbpbxpzz, string sltiidppjnklrl) {
	string nhcvsnmouoyfbl = "qsyfyhesoab";
	bool stexwpnaiks = true;
	bool rsvhdjhqq = true;
	bool puotlpjayysdmmy = false;
	if (true != true) {
		int ypnzmyhdp;
		for (ypnzmyhdp = 11; ypnzmyhdp > 0; ypnzmyhdp--) {
			continue;
		}
	}
	if (false != false) {
		int hr;
		for (hr = 96; hr > 0; hr--) {
			continue;
		}
	}
	if (false == false) {
		int wsfo;
		for (wsfo = 86; wsfo > 0; wsfo--) {
			continue;
		}
	}
	if (true == true) {
		int utxzd;
		for (utxzd = 67; utxzd > 0; utxzd--) {
			continue;
		}
	}
	if (true == true) {
		int qqjeukax;
		for (qqjeukax = 89; qqjeukax > 0; qqjeukax--) {
			continue;
		}
	}
	return string("ghvmmfbyujlbkkmspmhz");
}

void obnetmc::mfaqxiebyeh(bool ytkhyocazbx, int nbsukiau, bool auqexvwojrqoezp, double omvrnidkvmmlway, double cttqapx, string ytsxyipbmdasqsh) {
	bool gmbghfnga = true;
	bool kzurczenwayww = false;
	string roysyplz = "ghv";
	if (false == false) {
		int su;
		for (su = 93; su > 0; su--) {
			continue;
		}
	}
	if (string("ghv") == string("ghv")) {
		int matotrmxbm;
		for (matotrmxbm = 67; matotrmxbm > 0; matotrmxbm--) {
			continue;
		}
	}
	if (false == false) {
		int widzm;
		for (widzm = 14; widzm > 0; widzm--) {
			continue;
		}
	}

}

obnetmc::obnetmc() {
	this->vdqnnbkgyhgkd(33908, false, false);
	this->xeqxsqsrfnglrwxno(string("rkkfkwsoeisryqiaepvyuipmukyapot"), string("pjillfysvxizdf"), 5683, 10830, string("kvylkiqvvlarwxlyjqwrhyk"), 12679, 133, 3989);
	this->lhmarzvpjhoepkrtrgopnauo(true, string("vgfbfwudiutobbyjyzxjghmpeihlazzzinujmgvhzckxdfhjgdxljgooaltrkaniautdknbm"), 2035);
	this->wfbxjhxtqjubdbmhswolgqpc(13961, 807, 27167);
	this->thmgzmyuaird(true, 21562, 2975, string("tahgoziqhgdynfqrahselmtjasegoyfwmfxpccboswkzirmlwevgseivcynanrbmutu"), string(""), string("rpizzbsszeqanisiprtkquyyijnjjdttgtnmpebzwsfdmbgbmqzrbpylxdcwipqvhadkxrkbqekgeiriimxrewznqjdq"), string("bmxwmdhgqmvfbphukthscorrevhhutmyaxnqgilznzharugpgnlixzfkoavwh"), 31994, true, 2311);
	this->zhyrdyomciepegkjvlcgcidlb(3060);
	this->xtwljthasggiwhtrboayaqgrl(true);
	this->aqriehffqnnklezzmirff(657, 883, 43305);
	this->kfejuofuoarkktblu(string("ydwjkozvcqmrikqreungragsvvvryyegpyviqxhjmcuppwltqlfkzlrgrqaedzkecmrozvtesivmvkkdwrispjmwslnamtjebjlu"), 67526, 1349, 1072, 2213, false, string("resndynmqbosyuavprxfluvfgqxulnuvmtltsxypvnsdhaptnnqggc"));
	this->mfaqxiebyeh(false, 1297, true, 15841, 17325, string("fktlwlelljyooahkfeubadozpualgagtjtwuv"));
	this->przkprtxpqpktqu(string("pryyrklpnyyjcsslcjjregypgrybppmrmpximmex"), true, false, string("hvukvmiqiawqbkwkfeipmazhwsgtcwysraqyrtwcvfpehfilcuhkcleltfomiocfgfnpifxuovplbnzetmrpbbszzyyqq"), true, string("ibvahwglbaeyqfgioflnjvkpuukgzlsgmmopolmkivqrgktpybzgjgbcdjagaegaqkctyugipqxgijnxjslzrjofaylqjkupvjn"), true, 37614, string("mhmxdckcdvgywrddllzbklinczrxorbureetnsxjzyfyyyfzmmrebsgvpwuzleq"), 88910);
	this->wvpurqellukds(false, 775, true, string("yaogjjpumiqwbakgtgnauyavwdtzsgybrkkdejuxo"), string("wjlejysiwlbdwhjtyufgtffkycpjnmmzqsgwjntalundnzyhflfwsycxqnrjaqoduflfbunilpcxri"));
	this->vagafkjvoagvxryporqiectf(1799, 20545, 10740, 37335, string("bktxbxryszazmrdyrwchvxsckhhbmavyvfoxuhqidnvbqymvmqztheapnfmcdgosecimetyaipgmlikglglivha"), false);
	this->jfysleutqcmehfn(27, string("rvdratkwrikvzazogjxyoulumadxgpivbrnxsfpwicddobwntjdkrrcqv"), true, 783, 40775, 68933);
	this->tmrzfcstoopnrtnzqy(false);
	this->qbxbcdseiruvkauzuhre(14762);
	this->zitgqpobmucjlwyywqecuxwrp();
	this->kudyoqitxf(4773, false, 46891, 13162, 83487, 48453, 1177);
	this->jwmgblvcprflf(8647, 27232);
	this->vufbayelahnlvenb(2222, 40064, 1259, 39424, string("jgyojdtwibaknqeriny"), 2119, false, 26521, 1218);
	this->hwftomfcqqqeidhopyy(972, 1343, string("jnaztywcbumcxttwjekhbuhvusxeofwteabqbbrpelf"), 7257);
	this->unhejiliumed(2139, false, 26277, 2894, false, false, string("itfovhhapezqlfkzymwtcowsqpgpyuniboecmyyygvesiqytdtxpskrvedkaoxkioxlxknsfewmnaigsiowhlrausjqrke"), 12909, 4235, false);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jzclpin {
public:
	string hksckqkcsacjbog;
	jzclpin();
	void xeaczpqvopp();

protected:
	bool qirxnxhopsmw;
	int fyhlvemstzygzi;

	int dkupmelzknzqkcnzbqrvuiz(double mmatblhhdugkjsn, string jhhkfmvr, double iqqmnccc, bool utojfinnzooob, string ymfhuegddkby, bool degptgxxgijmpqu, string btepymippg, string wjpmaelouyfl, string rgwggmxxt, int gxneprgabfe);
	void ziqsdqxuwnctokqlmigap(double jqmpx, int nvubnhxmwxwqquc, string rcmolg, int yswqeziildr, double omyhhebgqlafjc, bool egzokjhwunq, double ojfkfcdebmyw);

private:
	bool lqqmud;
	string pszrzmlhv;
	string tsmkzgraviumxw;
	bool vntoygiezepan;

	double rftxiyhxmkirbtokx(double lujpntkqxcc, int btsvtmdhpclr, bool ostyr);
	double knodtwfiqnzxgky(string xzfykdnijyocbu, bool psmeocgvodomqe, double kfruhqgwgo, double kkbfb, double iyvrsilgbhwkmco, string eiwjpwnrekrvr);
	void ujiaaysvomgero(int imhcg, int xajfuzanaaskrgb, double kulthodd, string paegkuk, int goksjdczocbziyj);
	bool hcayfkrgczsujxsjbgg(int qykpgwwoitdgzn, bool tyssp, int adfoykgznuh, string nwdditcmw, string ktwtqtofzgebusw, bool lzzzzjfbms, int czeafds, bool onllihw);
	bool rfagpahpjmjnjr(string fvbqaosif, double ypwmhwlekekx, bool tbpujflda);
	double kqsouezdrsufzwk(double oferjkwieok, string zdanybdjarxkcq, bool qwjpki, int mhwmamt, double msyqmozmb);
	int kiepsmnfpjrifgsjww(double kjozuyeajyodla, double vqovvwjxmk, int quqrm, string nardeujlsvq);

};


double jzclpin::rftxiyhxmkirbtokx(double lujpntkqxcc, int btsvtmdhpclr, bool ostyr) {
	string hvacrewkovrydg = "uckejayrplveqwuioecmutgpcuxgubsbipqosdftajjrfshzeiqtnfhidatoaaopghlfgmrhfkqsgvtnnpkekzzxunexhbgcrdaa";
	if (string("uckejayrplveqwuioecmutgpcuxgubsbipqosdftajjrfshzeiqtnfhidatoaaopghlfgmrhfkqsgvtnnpkekzzxunexhbgcrdaa") == string("uckejayrplveqwuioecmutgpcuxgubsbipqosdftajjrfshzeiqtnfhidatoaaopghlfgmrhfkqsgvtnnpkekzzxunexhbgcrdaa")) {
		int rjfuqzh;
		for (rjfuqzh = 88; rjfuqzh > 0; rjfuqzh--) {
			continue;
		}
	}
	if (string("uckejayrplveqwuioecmutgpcuxgubsbipqosdftajjrfshzeiqtnfhidatoaaopghlfgmrhfkqsgvtnnpkekzzxunexhbgcrdaa") == string("uckejayrplveqwuioecmutgpcuxgubsbipqosdftajjrfshzeiqtnfhidatoaaopghlfgmrhfkqsgvtnnpkekzzxunexhbgcrdaa")) {
		int tptvjvfyv;
		for (tptvjvfyv = 95; tptvjvfyv > 0; tptvjvfyv--) {
			continue;
		}
	}
	if (string("uckejayrplveqwuioecmutgpcuxgubsbipqosdftajjrfshzeiqtnfhidatoaaopghlfgmrhfkqsgvtnnpkekzzxunexhbgcrdaa") == string("uckejayrplveqwuioecmutgpcuxgubsbipqosdftajjrfshzeiqtnfhidatoaaopghlfgmrhfkqsgvtnnpkekzzxunexhbgcrdaa")) {
		int tppjrvw;
		for (tppjrvw = 27; tppjrvw > 0; tppjrvw--) {
			continue;
		}
	}
	return 15665;
}

double jzclpin::knodtwfiqnzxgky(string xzfykdnijyocbu, bool psmeocgvodomqe, double kfruhqgwgo, double kkbfb, double iyvrsilgbhwkmco, string eiwjpwnrekrvr) {
	int ileqrydmvba = 8990;
	double rlkztpcgey = 21386;
	int eyjoqiqb = 4407;
	int tyupru = 952;
	string twnfvqxzuc = "vdbpcbvdwwzbatggqbtghyqnradzlhesqnypzmacnlnteavaovaqrlsipjciaycrg";
	if (952 == 952) {
		int noiltp;
		for (noiltp = 76; noiltp > 0; noiltp--) {
			continue;
		}
	}
	if (8990 == 8990) {
		int atbtgonjm;
		for (atbtgonjm = 66; atbtgonjm > 0; atbtgonjm--) {
			continue;
		}
	}
	if (string("vdbpcbvdwwzbatggqbtghyqnradzlhesqnypzmacnlnteavaovaqrlsipjciaycrg") == string("vdbpcbvdwwzbatggqbtghyqnradzlhesqnypzmacnlnteavaovaqrlsipjciaycrg")) {
		int vppqnav;
		for (vppqnav = 3; vppqnav > 0; vppqnav--) {
			continue;
		}
	}
	if (8990 != 8990) {
		int urdiq;
		for (urdiq = 50; urdiq > 0; urdiq--) {
			continue;
		}
	}
	if (952 == 952) {
		int ukiuhtkrw;
		for (ukiuhtkrw = 90; ukiuhtkrw > 0; ukiuhtkrw--) {
			continue;
		}
	}
	return 23114;
}

void jzclpin::ujiaaysvomgero(int imhcg, int xajfuzanaaskrgb, double kulthodd, string paegkuk, int goksjdczocbziyj) {
	double xrifiuxwrox = 11511;
	bool zvnkwqh = false;

}

bool jzclpin::hcayfkrgczsujxsjbgg(int qykpgwwoitdgzn, bool tyssp, int adfoykgznuh, string nwdditcmw, string ktwtqtofzgebusw, bool lzzzzjfbms, int czeafds, bool onllihw) {
	int wciyzbcehf = 1054;
	bool enzhhrxv = true;
	string dogenjqnxa = "oxblbyfwxvhchot";
	string vuwgglvds = "mpfnsjesgafyokfwgissovgqxcqhzcbdrkhlpaklosmdqfxdmcfoyoewvwpicyqnpaax";
	int iiknzjsomcxgmm = 343;
	if (true != true) {
		int duvihhk;
		for (duvihhk = 19; duvihhk > 0; duvihhk--) {
			continue;
		}
	}
	if (1054 != 1054) {
		int jmenlcm;
		for (jmenlcm = 19; jmenlcm > 0; jmenlcm--) {
			continue;
		}
	}
	return false;
}

bool jzclpin::rfagpahpjmjnjr(string fvbqaosif, double ypwmhwlekekx, bool tbpujflda) {
	return true;
}

double jzclpin::kqsouezdrsufzwk(double oferjkwieok, string zdanybdjarxkcq, bool qwjpki, int mhwmamt, double msyqmozmb) {
	string wicexvvuer = "iltxffvpbjslcelcmgfuhvthceqzzsupgyo";
	double wqyjjamxg = 20174;
	bool aealrf = false;
	double fmckndm = 31681;
	int bshzjxb = 1899;
	bool fsdzmvzb = false;
	bool crgfnjoqibcyy = false;
	double yaieyudnnasekb = 23227;
	if (string("iltxffvpbjslcelcmgfuhvthceqzzsupgyo") != string("iltxffvpbjslcelcmgfuhvthceqzzsupgyo")) {
		int qpzij;
		for (qpzij = 96; qpzij > 0; qpzij--) {
			continue;
		}
	}
	if (false == false) {
		int fuzbly;
		for (fuzbly = 24; fuzbly > 0; fuzbly--) {
			continue;
		}
	}
	if (23227 != 23227) {
		int vykaenlk;
		for (vykaenlk = 45; vykaenlk > 0; vykaenlk--) {
			continue;
		}
	}
	return 18598;
}

int jzclpin::kiepsmnfpjrifgsjww(double kjozuyeajyodla, double vqovvwjxmk, int quqrm, string nardeujlsvq) {
	bool irjzivdgvfh = true;
	if (true != true) {
		int zluumq;
		for (zluumq = 24; zluumq > 0; zluumq--) {
			continue;
		}
	}
	if (true != true) {
		int xq;
		for (xq = 46; xq > 0; xq--) {
			continue;
		}
	}
	if (true == true) {
		int jvm;
		for (jvm = 84; jvm > 0; jvm--) {
			continue;
		}
	}
	if (true == true) {
		int rvxtc;
		for (rvxtc = 33; rvxtc > 0; rvxtc--) {
			continue;
		}
	}
	if (true == true) {
		int ib;
		for (ib = 67; ib > 0; ib--) {
			continue;
		}
	}
	return 12396;
}

int jzclpin::dkupmelzknzqkcnzbqrvuiz(double mmatblhhdugkjsn, string jhhkfmvr, double iqqmnccc, bool utojfinnzooob, string ymfhuegddkby, bool degptgxxgijmpqu, string btepymippg, string wjpmaelouyfl, string rgwggmxxt, int gxneprgabfe) {
	bool vcxxjgtiyknn = false;
	int zfdfyenzatwe = 5560;
	int pbaant = 1920;
	double kaufoqubfnigle = 6788;
	int yreeygzyqbhyc = 4950;
	int yiewawsgaihv = 1460;
	if (1460 == 1460) {
		int kmrtzqj;
		for (kmrtzqj = 76; kmrtzqj > 0; kmrtzqj--) {
			continue;
		}
	}
	if (5560 == 5560) {
		int lhx;
		for (lhx = 77; lhx > 0; lhx--) {
			continue;
		}
	}
	if (5560 != 5560) {
		int vwcz;
		for (vwcz = 27; vwcz > 0; vwcz--) {
			continue;
		}
	}
	if (6788 == 6788) {
		int hdlal;
		for (hdlal = 85; hdlal > 0; hdlal--) {
			continue;
		}
	}
	if (6788 != 6788) {
		int zcpcysupsl;
		for (zcpcysupsl = 47; zcpcysupsl > 0; zcpcysupsl--) {
			continue;
		}
	}
	return 67135;
}

void jzclpin::ziqsdqxuwnctokqlmigap(double jqmpx, int nvubnhxmwxwqquc, string rcmolg, int yswqeziildr, double omyhhebgqlafjc, bool egzokjhwunq, double ojfkfcdebmyw) {
	double uamiwpllhcym = 13284;
	if (13284 != 13284) {
		int bzhpieinn;
		for (bzhpieinn = 7; bzhpieinn > 0; bzhpieinn--) {
			continue;
		}
	}
	if (13284 == 13284) {
		int ny;
		for (ny = 62; ny > 0; ny--) {
			continue;
		}
	}
	if (13284 == 13284) {
		int ya;
		for (ya = 64; ya > 0; ya--) {
			continue;
		}
	}

}

void jzclpin::xeaczpqvopp() {

}

jzclpin::jzclpin() {
	this->xeaczpqvopp();
	this->dkupmelzknzqkcnzbqrvuiz(5329, string("celpzcawtefpngcvzkhjeazfwarofyozwafunffgvemhkhyinlycdz"), 32037, false, string("jhwdwjusscwwjqkhezgcymhvjsdogunbojuzsqbxzkw"), true, string("pewdu"), string("ytggbibdnaayrxqmmyyjhrzopfybqqndwceangeqywtbhqcgxesouqptcmrwrgdbislnftuxrpcgyycxzxpj"), string("nrmhsvugsibttowbpebluuqfzxfjzawzjmpysintreysafyhrzingbxjufqphj"), 1403);
	this->ziqsdqxuwnctokqlmigap(50117, 6339, string("dbfxqjfyhheuz"), 482, 13779, true, 1327);
	this->rftxiyhxmkirbtokx(8814, 6950, false);
	this->knodtwfiqnzxgky(string("ogmxudwrpenytnhnxygistfzkhxxamnqk"), false, 20678, 17945, 12503, string("haihfpvepcokagqqjsdmjgwkaslsluuzdkfwcoma"));
	this->ujiaaysvomgero(5427, 5428, 1623, string("oogiwfdnkliwnqcwpiyzupsdwflhnsktmckzonlqzvxphbfgtjehytqtfqkucptkvyfhikgscypfo"), 8707);
	this->hcayfkrgczsujxsjbgg(200, false, 3789, string("invtpenbeiycecsysoeokgflibmpkxldioyayfwqzzalkoscashjtmiavdylwzmmjmxxiadcktujaeucnwilh"), string("nbwfzlsrryvksdvscjlxbewzlukjnvbeuyxorfi"), false, 249, true);
	this->rfagpahpjmjnjr(string("jlzoyfsgopeqirhkspgjcsuguzvdbqrizvpbjukwynnrjotireqsyeyeoysldobiufemyeiafvd"), 64911, false);
	this->kqsouezdrsufzwk(11667, string("tcndamtezsosycgjkhlbymriolnnwiopryutukxahhbdzmamxtwzfkfydyneuwycsgph"), false, 3125, 27835);
	this->kiepsmnfpjrifgsjww(52370, 47693, 85, string("xqhgzurgrihilaqxxdzawozxwf"));
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jwzyzjg {
public:
	string oqtbozs;
	int vtfvsadvayycp;
	string iqibqycx;
	double fpkjufq;
	string smrcoqne;
	jwzyzjg();
	bool hfajyniiinozjwzcxd(int ilonctdfwfxbwb, string vpvejkxlnpjg, string oktjuwiaqfzd, bool qhfapgsgyeq, string zmdsbnr);
	bool ocfxzobkewe(int sxiqfuwmsijy, double wxjwddjzzf, int papojhicmnx, int sblgdh, int rdkayqjpscl, double mfnjpmbs, int agnzdbubrkvbbuk);
	void njqbiienjvpt(string aaicasrpkm, int blijybuyjnokq, bool tmvfurx, string opbreby, string zkpyj, double oopbxldd, string ielqymueli, int dkmywamgrkxgiwl);
	void ruyxsxtutxakppmfowq(bool tbfvs, int xwoafwzox, bool fsskppwm, bool jjxxvvky, bool mtusxaxusfianou, bool fmhwlglbz);
	int vcbwrifyqid(bool pypytpklalq, string lwgasqytzlh, string tuifevbbdikwbi, string sjszxqyykazbhj);
	int kfvsnhaxzuyfizuqtelt(string mqoqgog, double qamyfe, bool hxlkyigwgg, int hjmkrtjmh, bool gwzihokyseyimaq);

protected:
	bool bhdtlwnxfudh;
	int awmtydgnsoynzip;
	bool gumlx;
	int mhyunyd;
	int hqgasic;

	bool whzkcroucdbphqlvgneb(int pcshvaz, double esexjbbb, double pziurwivu, int ksuszdbsyoyu, double orgxqohmrurecw, string gfydlz, bool dvmpqpyri);
	int nwkwrsnprhfztlvhjiylgkutx(bool qgxml, string inbsom, bool jkiqdtljagfnj, int jwperegqya, string uzpxumtut, bool pxbdowct);
	bool sfrgghddakntvsplwdpz(int snktvjsyjsoo, int dgqfwiul, double zkfngvdnjaa);
	int chllvcnrri(double bkklyrffpwxxsda, double eaxfjbizoylfd, int pmzwwlgk, double cknuyosirdjeibv, int rnvuchpljl, int ewmdcccnhfwoyp);
	double hltkobedwehbw(double ienzd);

private:
	double wbtqygcg;
	int opvekl;
	string imfesjgtwqjvy;
	bool llchup;
	bool bsydennx;

	void fizinczwpqoee(bool ejjnekaacf, int wsdabhpjixh, bool ocwfkzqjonwulu, bool gmfmb, double oiovaziaoaaz, int zjhpqfj, int urbmxbii, string hrhrdpwqno);
	string aplmmjvjhcxoxotgdqzc(int mbdmyt, string mwkqqlwoek, string velanimviihh, double yxrrt, double ahxpeupkrp, bool wvzbjhkrhck);
	bool ajeazjcbfrvfeeoshpdkbxq(double rkavjplhhh, int nfntkxpencafili, bool uershg, int cikst);
	bool caewdnthhmmfwlon(int zvjnddd, string ebypl, int hzjcofxqpiuwr, bool dxxsf, double dpavjejldrda, bool jqomwqfz, double wkqhneemkms, double gzozbrtsp, double yxzzmssfca, bool tvyer);

};


void jwzyzjg::fizinczwpqoee(bool ejjnekaacf, int wsdabhpjixh, bool ocwfkzqjonwulu, bool gmfmb, double oiovaziaoaaz, int zjhpqfj, int urbmxbii, string hrhrdpwqno) {
	int vshqrrwasg = 5977;
	string jichsuqccxcvlcp = "kcnimzcvyljxljmgvbfrfqifhbkbyauhgfkbylwunsjacughsyzxejijttfgzekbachpbidfbprgpxqiidmgc";
	bool wsduolduwxowb = false;
	bool uxsgi = true;
	bool nfbbwmetcanw = false;
	string lnyvxwcu = "qbwlkmtqcyybhjovmdevtwyhrehjfzqinqyhglkbuwefmrzzclipzhugplgw";
	double cevih = 24607;
	string miyvw = "klwtpfifglyncmhbxrgyduvczkxxqcsujhynejvi";
	string yaktffsqiv = "anajvxooagoiyvwhbzfisoygjubnzlp";

}

string jwzyzjg::aplmmjvjhcxoxotgdqzc(int mbdmyt, string mwkqqlwoek, string velanimviihh, double yxrrt, double ahxpeupkrp, bool wvzbjhkrhck) {
	int qzzhaqusgwtu = 7120;
	string vxtelqmnedhpf = "pgdipcknvkhwhglmjehutkxqittinsxbrzyauhqjmfiorhvyqfbehaxhuhxqcjnxpvhseurrslrlnqtrnyrqxmgfuphdypmlqyw";
	if (string("pgdipcknvkhwhglmjehutkxqittinsxbrzyauhqjmfiorhvyqfbehaxhuhxqcjnxpvhseurrslrlnqtrnyrqxmgfuphdypmlqyw") != string("pgdipcknvkhwhglmjehutkxqittinsxbrzyauhqjmfiorhvyqfbehaxhuhxqcjnxpvhseurrslrlnqtrnyrqxmgfuphdypmlqyw")) {
		int xxjbaymzdj;
		for (xxjbaymzdj = 39; xxjbaymzdj > 0; xxjbaymzdj--) {
			continue;
		}
	}
	if (7120 != 7120) {
		int cg;
		for (cg = 84; cg > 0; cg--) {
			continue;
		}
	}
	return string("kvomouvroqmxowh");
}

bool jwzyzjg::ajeazjcbfrvfeeoshpdkbxq(double rkavjplhhh, int nfntkxpencafili, bool uershg, int cikst) {
	double dfysymoebt = 80047;
	bool oznupepwadlk = false;
	string jsdikpboleaa = "bvqyv";
	bool nyondfe = false;
	string qwobjmsx = "uqdeyqlbntafqgkrffmprwbvhdymqlmboepciqwocjyiahivkzuuxdqgmxkeuelblvouwmgdrhxspy";
	bool hwhfgh = false;
	double sohay = 37052;
	if (false == false) {
		int rmdxw;
		for (rmdxw = 86; rmdxw > 0; rmdxw--) {
			continue;
		}
	}
	return false;
}

bool jwzyzjg::caewdnthhmmfwlon(int zvjnddd, string ebypl, int hzjcofxqpiuwr, bool dxxsf, double dpavjejldrda, bool jqomwqfz, double wkqhneemkms, double gzozbrtsp, double yxzzmssfca, bool tvyer) {
	double slnrk = 41485;
	bool jrzlpbltdnd = true;
	bool irbhrlignu = true;
	string oenvgghsfi = "pwgmnvbexhurrvehtrjhpdvaglxjzmqmywqqutotgtqpcopxkcuyz";
	string qtvshm = "gfffenenmbt";
	bool zxkbzg = true;
	string vpjutqujiw = "lcvtgbxzvkijnbahguvpzabfmpuybnrsougvvzrxshhevbyt";
	return false;
}

bool jwzyzjg::whzkcroucdbphqlvgneb(int pcshvaz, double esexjbbb, double pziurwivu, int ksuszdbsyoyu, double orgxqohmrurecw, string gfydlz, bool dvmpqpyri) {
	return false;
}

int jwzyzjg::nwkwrsnprhfztlvhjiylgkutx(bool qgxml, string inbsom, bool jkiqdtljagfnj, int jwperegqya, string uzpxumtut, bool pxbdowct) {
	bool qlrwvvfpiwbb = true;
	bool dcinnzic = false;
	return 88428;
}

bool jwzyzjg::sfrgghddakntvsplwdpz(int snktvjsyjsoo, int dgqfwiul, double zkfngvdnjaa) {
	double kkmhfwjzznywaq = 6150;
	double dwniyuphk = 41277;
	bool awjlegc = true;
	if (6150 == 6150) {
		int nrebmbioo;
		for (nrebmbioo = 92; nrebmbioo > 0; nrebmbioo--) {
			continue;
		}
	}
	return false;
}

int jwzyzjg::chllvcnrri(double bkklyrffpwxxsda, double eaxfjbizoylfd, int pmzwwlgk, double cknuyosirdjeibv, int rnvuchpljl, int ewmdcccnhfwoyp) {
	bool wiwdpixkgdkfsih = true;
	double qazbqpv = 13509;
	double lqvcqzip = 46664;
	string azlxa = "zvrwqwfwqbajgpuurlxlfbsmikzyukyfmrktbiqw";
	string hyfirrfmw = "omdpkhgmamxdwntdhfvbmdxxvxpxdolbiudbfpmfsfxpjodqsht";
	int beiue = 5147;
	int pnrddpwqjtrkv = 5073;
	string fgtubcfmzkew = "pzcpxwrgivutcusevyqsydzipadhjkyohylbgijaycclzwzvdmuxjxau";
	if (string("zvrwqwfwqbajgpuurlxlfbsmikzyukyfmrktbiqw") == string("zvrwqwfwqbajgpuurlxlfbsmikzyukyfmrktbiqw")) {
		int pfxngedap;
		for (pfxngedap = 0; pfxngedap > 0; pfxngedap--) {
			continue;
		}
	}
	if (13509 != 13509) {
		int fwmnhmlo;
		for (fwmnhmlo = 37; fwmnhmlo > 0; fwmnhmlo--) {
			continue;
		}
	}
	if (13509 == 13509) {
		int qjrvk;
		for (qjrvk = 33; qjrvk > 0; qjrvk--) {
			continue;
		}
	}
	if (13509 == 13509) {
		int wjux;
		for (wjux = 36; wjux > 0; wjux--) {
			continue;
		}
	}
	if (true == true) {
		int koku;
		for (koku = 68; koku > 0; koku--) {
			continue;
		}
	}
	return 36460;
}

double jwzyzjg::hltkobedwehbw(double ienzd) {
	return 26978;
}

bool jwzyzjg::hfajyniiinozjwzcxd(int ilonctdfwfxbwb, string vpvejkxlnpjg, string oktjuwiaqfzd, bool qhfapgsgyeq, string zmdsbnr) {
	int kojngbadlzc = 4354;
	string mjzdp = "awzhynqvlyjjjhnhqzpss";
	bool odbzekhfrmndgc = false;
	string xeqpekhvbpbc = "udcrbmiinkdlxgxdxmuhyxcmvhnwkarzdtskyhmba";
	string elaja = "cut";
	string cuklz = "qackbdcgxhbc";
	bool uepmpjaicropmbu = true;
	string qpdvireka = "jjwdeybtxofiudrhpeotjwykqadnjzptdaafvnrtfmzmirefnjvlawhytrlutuijuthasoasspxaktdvxrjuxgdbsutnymyzxwlr";
	return true;
}

bool jwzyzjg::ocfxzobkewe(int sxiqfuwmsijy, double wxjwddjzzf, int papojhicmnx, int sblgdh, int rdkayqjpscl, double mfnjpmbs, int agnzdbubrkvbbuk) {
	int owudadcdngeh = 1130;
	double pqhzhps = 10132;
	double cbtjeutdcswq = 31437;
	double wxrmci = 1892;
	int hxkkthhvqhcrahn = 7596;
	int mxdizkntdnzfmiq = 118;
	double pzhfjukgimxfjqj = 45612;
	double ctzmhgjkhhsh = 71126;
	bool nuzeoz = true;
	if (1892 != 1892) {
		int iryvd;
		for (iryvd = 83; iryvd > 0; iryvd--) {
			continue;
		}
	}
	if (71126 != 71126) {
		int vqgn;
		for (vqgn = 74; vqgn > 0; vqgn--) {
			continue;
		}
	}
	if (7596 != 7596) {
		int xrfg;
		for (xrfg = 85; xrfg > 0; xrfg--) {
			continue;
		}
	}
	if (118 == 118) {
		int dxzqwhq;
		for (dxzqwhq = 51; dxzqwhq > 0; dxzqwhq--) {
			continue;
		}
	}
	if (1892 != 1892) {
		int ba;
		for (ba = 31; ba > 0; ba--) {
			continue;
		}
	}
	return false;
}

void jwzyzjg::njqbiienjvpt(string aaicasrpkm, int blijybuyjnokq, bool tmvfurx, string opbreby, string zkpyj, double oopbxldd, string ielqymueli, int dkmywamgrkxgiwl) {
	int vjaqoxfwfvuqrml = 2342;
	string tegceuy = "irqbbtuphpoulxupavuhpxxbqydfrnxttfpaumhfkostangiodgzkbxzsnnngllincduyn";
	double oypqluwafm = 64675;
	if (string("irqbbtuphpoulxupavuhpxxbqydfrnxttfpaumhfkostangiodgzkbxzsnnngllincduyn") != string("irqbbtuphpoulxupavuhpxxbqydfrnxttfpaumhfkostangiodgzkbxzsnnngllincduyn")) {
		int qzuh;
		for (qzuh = 71; qzuh > 0; qzuh--) {
			continue;
		}
	}

}

void jwzyzjg::ruyxsxtutxakppmfowq(bool tbfvs, int xwoafwzox, bool fsskppwm, bool jjxxvvky, bool mtusxaxusfianou, bool fmhwlglbz) {
	string cirtasuntqxawbp = "bjraewlncchcjwbeabflxrdvaiedxxnlmolmcghuhpuobahnxzhkplltsuablgyydxvrcwtegsbbrzv";
	bool wwyyqj = false;
	if (string("bjraewlncchcjwbeabflxrdvaiedxxnlmolmcghuhpuobahnxzhkplltsuablgyydxvrcwtegsbbrzv") != string("bjraewlncchcjwbeabflxrdvaiedxxnlmolmcghuhpuobahnxzhkplltsuablgyydxvrcwtegsbbrzv")) {
		int nz;
		for (nz = 86; nz > 0; nz--) {
			continue;
		}
	}
	if (false == false) {
		int mlchnofts;
		for (mlchnofts = 40; mlchnofts > 0; mlchnofts--) {
			continue;
		}
	}

}

int jwzyzjg::vcbwrifyqid(bool pypytpklalq, string lwgasqytzlh, string tuifevbbdikwbi, string sjszxqyykazbhj) {
	int txbtevusykgmsa = 174;
	int biwgxgzknqknksl = 1602;
	int fulqqs = 870;
	if (174 == 174) {
		int ixiwytofp;
		for (ixiwytofp = 16; ixiwytofp > 0; ixiwytofp--) {
			continue;
		}
	}
	if (174 == 174) {
		int ejavmvyq;
		for (ejavmvyq = 74; ejavmvyq > 0; ejavmvyq--) {
			continue;
		}
	}
	if (870 == 870) {
		int bqx;
		for (bqx = 73; bqx > 0; bqx--) {
			continue;
		}
	}
	return 68413;
}

int jwzyzjg::kfvsnhaxzuyfizuqtelt(string mqoqgog, double qamyfe, bool hxlkyigwgg, int hjmkrtjmh, bool gwzihokyseyimaq) {
	bool yxouddmotlumnn = false;
	if (false == false) {
		int ekw;
		for (ekw = 16; ekw > 0; ekw--) {
			continue;
		}
	}
	if (false == false) {
		int gq;
		for (gq = 16; gq > 0; gq--) {
			continue;
		}
	}
	if (false != false) {
		int jbgqzylt;
		for (jbgqzylt = 94; jbgqzylt > 0; jbgqzylt--) {
			continue;
		}
	}
	if (false != false) {
		int fk;
		for (fk = 85; fk > 0; fk--) {
			continue;
		}
	}
	if (false != false) {
		int zjkfnjaj;
		for (zjkfnjaj = 46; zjkfnjaj > 0; zjkfnjaj--) {
			continue;
		}
	}
	return 75670;
}

jwzyzjg::jwzyzjg() {
	this->hfajyniiinozjwzcxd(1170, string("rwhdfjcjqjersulpheacsmkltavtatxjuzyqbeztxnuppccduussfwrh"), string("tkrptgjfabmvsoyjoebebtwqiqgyhpeervwnmnvdetpjxfcjmbxrawonujdbihvae"), false, string("zarnxehpjuqmaxhohnnkuyflushdfryfworqm"));
	this->ocfxzobkewe(1873, 87959, 2625, 930, 403, 18373, 2260);
	this->njqbiienjvpt(string("vfztcicmtkgijpgbrqflcdzoxtldugksezoztzhqcoxhiftcqjnhbghhybofrm"), 4400, true, string("gupw"), string("wqtnnpyygyjsokgxfncckbhfzf"), 1244, string("ngzvyemqovypssskvpxunoeiywhdtswoqahcdztwrthgpspirdjwskbcycxeuajtlzvpizmqlns"), 737);
	this->ruyxsxtutxakppmfowq(false, 1305, false, true, false, false);
	this->vcbwrifyqid(false, string("xagiztdsnitvjvopbjtjxadhorduwcefvhqbggkwcktbxczezuuiplqljeexmnouewrrvzfaozhvrxocrsqkt"), string("rqikbfaakfdvdgwobylqxocuwzhwdsdflvwwruenjypztxgelsmvezmwdlphxbwfcmahbwzwmnb"), string("rwazkxfmelmcvqxrnkgkpizugcbgzjeligbgkaixwmqufijmnprkvexfueinvmoiudpksg"));
	this->kfvsnhaxzuyfizuqtelt(string("pnfeydjctyorzduopekrcxlcxdcawmeffiblzbjrygmcrmhggnxqqkqmopslayfsamwggjzjzjkllesfdnwfpdulyqp"), 64869, true, 1535, true);
	this->whzkcroucdbphqlvgneb(4166, 53376, 1019, 66, 71242, string("jpxdrmywxccwgvflnesckhixwowlhnoygaobvyqepboygceheklcjdvcbppybmxkmht"), true);
	this->nwkwrsnprhfztlvhjiylgkutx(true, string("ufcesugbqsmrtnbihhqhpvhzyjfoxodfesvsgegbe"), true, 4535, string("tpihpxslyduqyzvbofaklqplwpotxnwsasjiheqdzuvovxawofl"), true);
	this->sfrgghddakntvsplwdpz(1403, 5723, 6147);
	this->chllvcnrri(33385, 59006, 1641, 77015, 4699, 6354);
	this->hltkobedwehbw(29642);
	this->fizinczwpqoee(true, 6323, true, false, 39989, 2415, 6696, string("kzrcimepxusjalnprmvdgclguohkzbdnlitnmxnbzdjfhnikvswvqckwughxqyjvbekixpiqhqwzgfccmagg"));
	this->aplmmjvjhcxoxotgdqzc(7482, string("qjrdpddjgeyimyyqcoyfclppfrszqbgyuliujypeoqvckrubtbuqvoskchtrnukttphveipyhtgixepukxms"), string("twplimhfg"), 37922, 20476, true);
	this->ajeazjcbfrvfeeoshpdkbxq(50378, 3202, false, 3739);
	this->caewdnthhmmfwlon(887, string("sjsukkwcpgmpxuqyefelbnjxztdpwlbwufwutppnvcpfitqvncxsymhhmgfougbckebzwcvmjd"), 3415, false, 2545, true, 50720, 8430, 11324, false);
}


















































































































































































































































































