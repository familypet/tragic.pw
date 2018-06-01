#include "Interfaces.h"
#include "Utilities.h"

void InitialiseInterfaces()
{
    auto EnginePointer = get_module_factory(GetModuleHandleW(L"engine.dll"));
    auto ClientPointer = get_module_factory(GetModuleHandleW(L"client.dll"));
    auto VGUISurfacePointer = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll")); 
    auto VGUI2Pointer = get_module_factory(GetModuleHandleW(L"vgui2.dll")); 
    auto MaterialPointer = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
    auto PhysicsPointer = get_module_factory(GetModuleHandleW(L"vphysics.dll"));
    auto StdPointer = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
    auto CachePointer = get_module_factory(GetModuleHandleW(L"datacache.dll"));

    g_CHLClient      = (IBaseClientDLL*)ClientPointer("VClient018", nullptr);
    g_Engine         = (IVEngineClient*)EnginePointer("VEngineClient014", nullptr);
    g_Panel          = (IPanel*)VGUI2Pointer("VGUI_Panel009", nullptr);
	g_Surface        = (ISurface*)VGUISurfacePointer("VGUI_Surface031", nullptr);
	g_EntityList     = (C_BaseEntityList*)ClientPointer("VClientEntityList003", nullptr);
	g_DebugOverlay   = (IVDebugOverlay*)EnginePointer("VDebugOverlay004", nullptr);
	g_Prediction     = (IPrediction*)ClientPointer("VClientPrediction001", nullptr);
	g_MaterialSystem = (CMaterialSystem*)MaterialPointer("VMaterialSystem080", nullptr);
    g_RenderView     = (CVRenderView*)EnginePointer("VEngineRenderView014", nullptr);
    g_ModelRender    = (IVModelRender*)EnginePointer("VEngineModel016", nullptr);
    g_ModelInfo      = (CModelInfo*)EnginePointer("VModelInfoClient004", nullptr);
	IGameEventManager2*   g_GameEvents = nullptr;
    g_EngineTrace          = (IEngineTrace*)EnginePointer("EngineTraceClient004", nullptr);
    g_PhysProps      = (IPhysicsSurfaceProps*)PhysicsPointer("VPhysicsSurfaceProps001", nullptr);
    g_CVar           = (ICVar*)StdPointer("VEngineCvar007", nullptr);
    g_Dlight         = (IVEffects*)EnginePointer("VEngineEffects001", nullptr);
    g_GameMovement   = (IGameMovement*)ClientPointer("GameMovement001", nullptr);
    g_MoveHelper     = **(IMoveHelper***)(U::FindPattern("client.dll", (PBYTE)"\x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68", "xx????xxxx") + 2);
    g_EventManager   = (IGameEventManager2*)EnginePointer("GAMEEVENTSMANAGER002", nullptr);
    g_GameConsole    = (IGameConsole*)ClientPointer("GameConsole004", nullptr);
    g_Input          = *(CInput**)((*reinterpret_cast<DWORD**>(g_CHLClient))[15] + 0x1); // A1 ? ? ? ? B9 ? ? ? ? FF 60 5C + 1
    g_ViewRender     = *(IViewRender**)(U::FindPattern("client.dll", (PBYTE)"\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xFF\x10", "x????x????xx????????xx") + 1);
    g_PlayerResource = **(C_CSPlayerResource***)(U::FindPattern("client.dll", (PBYTE)"\x8B\x3D\x00\x00\x00\x00\x85\xFF\x0F\x84\x00\x00\x00\x00\x81\xC7", "xx????xxxx????xx") + 2);
    g_GameRules      = **(C_CSGameRules***)(U::FindPattern("client.dll", (PBYTE)"\xA1\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x6A\x00\x68\x00\x00\x00\x00\xC6\x80", "x????xx????xxx????xx") + 1);
	g_MdlCache       = (IMDLCache*)CachePointer("MDLCache004", nullptr);
    g_ChatElement    = FindHudElement<CHudChat>("CHudChat");
    g_ClientMode     = **(IClientMode***)((*(DWORD**)g_CHLClient)[10] + 0x5);
    g_Globals        = **(CGlobalVarsBase***)((*(DWORD**)g_CHLClient)[0] + 0x1B);
    g_GlowObjManager = *(CGlowObjectManager**)(U::pattern_scan(GetModuleHandleW(L"client.dll"), "0F 11 05 ? ? ? ? 83 C8 01 C7 05 ? ? ? ? 00 00 00 00") + 3);

}


IBaseClientDLL* g_CHLClient;
IVEngineClient* g_Engine;
IPanel* g_Panel;
C_BaseEntityList* g_EntityList;
ISurface* g_Surface;
IVDebugOverlay* g_DebugOverlay;
IClientMode* g_ClientMode;
CGlobalVarsBase* g_Globals;
IPrediction *g_Prediction;
CMaterialSystem* g_MaterialSystem;
CVRenderView* g_RenderView;
IVModelRender* g_ModelRender;
CModelInfo* g_ModelInfo;
IEngineTrace* g_EngineTrace;
IPhysicsSurfaceProps* g_PhysProps;
ICVar* g_CVar;
IVEffects* g_Dlight;
IMoveHelper* g_MoveHelper;
IGameMovement* g_GameMovement;
CInput* g_Input;
IGameEventManager2* g_EventManager;
C_CSPlayerResource* g_PlayerResource;
C_CSGameRules* g_GameRules;
IViewRender* g_ViewRender;
IGameConsole* g_GameConsole;
IMDLCache* g_MdlCache;
CHudChat* g_ChatElement;
CGlowObjectManager* g_GlowObjManager;



#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class iumezkq {
public:
	bool xmpqhzdviw;
	string enbcatfojxum;
	iumezkq();
	void qownxeyjifbrwa(int rbeadusqvamwp, int gysrybh, int lxxrkmjn, int uxxaqqhmmrofx, double fwernjd, int xyerj, int izcak, int rmisylvyo, int anviuxenzroxcza);
	bool prqpwhixnlqlzi(int ifpfqrqyqypayle, bool fmvcibsyjuwei, string gdkzcdnbmyub);
	void qmotmdnqzps(double dzikbvuqikrfla, string gonxekstcbldzd);
	void kfettkxsgat(double xyxuhtz, string marfrcitccxyjoy, double iwvjnxdjxxgh, bool gflsqfshgfn, string wlbdhbfoav, string ghcqldjtbjaf);

protected:
	int qdfcslf;
	bool zhcyvr;
	int wmcxhgq;
	double gifgkrtnsygr;
	double lehxqrnoxvje;

	bool sflfrkjrbjbdgqxdqtwujl(double bejrabllcwcidfm);
	double nungpkhpsfsgjjgrg(bool stksoxxapwk, double fpvixfljdbsy, string eafvceulhg, bool obufvetbxfqswdn, string sqpipok, bool jsympha, string tnxibfjskvrqqh, int aejxaky, string msnuf, int guzkuce);
	void hvsizzjfoxdl(bool rvldswbfivl, string wgdlbuey, double ajxin, bool xwzeefjr);
	int ecfglqmbokqzhdtkbfk(double tpwldnpwj, double mevzsebefmmwphv);
	string fsockeqoirlehqfodx(double bliihbju, string rprzhqwsxk, bool vqssz, double qgkojkoefmvlepn, int mskhma, double kxgrsu, string odlelwchpse, bool wwqjhgfoqcxxx);
	string bnkyafslbmkoogfikak(string vjdhzqvlc, int oacty, string ughxndyustuxccd);
	int cwwjgnulozzqcpwdpq(bool klxmotp, double efmbmecdas, int zdxhed, int omjwnnjcreyjjy, int gppsqo, bool czwaufqmvr, int xhwwo, double jezbkzwkfvcy);
	bool qdahzjuldawwjbii();
	bool yinijfbmwvls(string apylljvvk, bool rpotwrtjzgmlwwk, double cxgztndoos, double vkabcqhh, int rrzenmbkpluu, int qofvguzrpbvr, string lwdwem, int njqdm);
	int bnlplhtybpdhmemuwutlmr(string dcdxh, string magjoeklfdkln, double rrhkrevjhtnnatm, double zhdznyxws, string zcmzwpgwhqmgpj, int dwlnnxkqlkbd);

private:
	bool yipend;
	string oeirbusv;
	string mltgvanfpw;
	double eqtxtqrdvpjnjpe;

	string gtvavwthdquesahjyjl(bool qivjhdvv);
	string dkbwcuksgsg(string vvtkd, bool zmgzwh, string dfznhy, double gpocbubogj);
	void trsonvsmyfnzwsn(bool qpiald, double teowkuwejnrb);
	double ddnkcyaoiifxwvatqwzeljgty(string iiyznnyniupjc, string ljyiflf, int cbjupkqhlbbnf, int cbryzmcd, double txgeqhdnduvgef, double ecjcondvskjgum, double zudqbmpjdag, double wikaeuemlwvyy);
	void tdtvcgyhgdjvppgpiowtcp(int uaihnesru, string ocnkvxivskxxn, string dxzmkixwzeov, double edmrim, bool rsvchmibc, bool yxvfj, double gklimlnirgfacn, string dlcoapvx, int tmtfztgenobw);
	void ovjlbaidsccfaxtzmqkqg(double tcbaebzmyfwen, bool mvffelnqa);

};


string iumezkq::gtvavwthdquesahjyjl(bool qivjhdvv) {
	bool cyremrkgpfvqo = true;
	string xydvptrwmkic = "xpvuazbhgbphsgxikwrchfgwzfuqujrhwbiageskgqdscktibfsnqxhnuiyvugwnjixfrqmuyuglgjgalvzc";
	int xbatghbbhn = 2882;
	bool wfshrnnlewyxg = false;
	string dcagwoka = "bfbwqrqcvyxqbnbtlfdizkxkwaftrlensutchcqfmowybhqsdytqulfhynvaydttzxvlstwq";
	if (string("bfbwqrqcvyxqbnbtlfdizkxkwaftrlensutchcqfmowybhqsdytqulfhynvaydttzxvlstwq") == string("bfbwqrqcvyxqbnbtlfdizkxkwaftrlensutchcqfmowybhqsdytqulfhynvaydttzxvlstwq")) {
		int zr;
		for (zr = 24; zr > 0; zr--) {
			continue;
		}
	}
	if (true == true) {
		int vpry;
		for (vpry = 36; vpry > 0; vpry--) {
			continue;
		}
	}
	return string("lowazoxtz");
}

string iumezkq::dkbwcuksgsg(string vvtkd, bool zmgzwh, string dfznhy, double gpocbubogj) {
	double unviy = 25733;
	int zkcxahpddnnj = 1688;
	double srgybitnfzd = 684;
	double rgcgnwpulsarx = 75599;
	int jbknarsxgt = 2746;
	if (1688 == 1688) {
		int hsv;
		for (hsv = 12; hsv > 0; hsv--) {
			continue;
		}
	}
	if (1688 != 1688) {
		int ksewfzrt;
		for (ksewfzrt = 8; ksewfzrt > 0; ksewfzrt--) {
			continue;
		}
	}
	if (2746 != 2746) {
		int uvduhgas;
		for (uvduhgas = 31; uvduhgas > 0; uvduhgas--) {
			continue;
		}
	}
	if (684 != 684) {
		int jdarnbf;
		for (jdarnbf = 49; jdarnbf > 0; jdarnbf--) {
			continue;
		}
	}
	return string("flrtidhchcjf");
}

void iumezkq::trsonvsmyfnzwsn(bool qpiald, double teowkuwejnrb) {
	double rdiknybnppfe = 78621;
	string heajudnb = "sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde";
	if (string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde") == string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde")) {
		int yogbwglvdd;
		for (yogbwglvdd = 87; yogbwglvdd > 0; yogbwglvdd--) {
			continue;
		}
	}
	if (string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde") == string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde")) {
		int rffxipnml;
		for (rffxipnml = 92; rffxipnml > 0; rffxipnml--) {
			continue;
		}
	}
	if (string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde") == string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde")) {
		int yoejzqixx;
		for (yoejzqixx = 6; yoejzqixx > 0; yoejzqixx--) {
			continue;
		}
	}
	if (string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde") == string("sxmqntphglzgqgeiwxylucsxlkhuqhzgnfttizfkhxmzdwiwrmzxmdavkondhidgde")) {
		int rsotdvetl;
		for (rsotdvetl = 82; rsotdvetl > 0; rsotdvetl--) {
			continue;
		}
	}
	if (78621 == 78621) {
		int rrfduchdof;
		for (rrfduchdof = 2; rrfduchdof > 0; rrfduchdof--) {
			continue;
		}
	}

}

double iumezkq::ddnkcyaoiifxwvatqwzeljgty(string iiyznnyniupjc, string ljyiflf, int cbjupkqhlbbnf, int cbryzmcd, double txgeqhdnduvgef, double ecjcondvskjgum, double zudqbmpjdag, double wikaeuemlwvyy) {
	bool vacksphqk = false;
	string dazsq = "yodyhtnppvrdmyridwwhbwg";
	double pudmfkwyq = 84212;
	double dqlwcx = 25977;
	if (false == false) {
		int rrehoe;
		for (rrehoe = 29; rrehoe > 0; rrehoe--) {
			continue;
		}
	}
	if (false != false) {
		int ireddfc;
		for (ireddfc = 100; ireddfc > 0; ireddfc--) {
			continue;
		}
	}
	return 3270;
}

void iumezkq::tdtvcgyhgdjvppgpiowtcp(int uaihnesru, string ocnkvxivskxxn, string dxzmkixwzeov, double edmrim, bool rsvchmibc, bool yxvfj, double gklimlnirgfacn, string dlcoapvx, int tmtfztgenobw) {
	double octokzbz = 17748;
	bool wsomkwg = false;

}

void iumezkq::ovjlbaidsccfaxtzmqkqg(double tcbaebzmyfwen, bool mvffelnqa) {
	string fgnpmhdyrbdvheh = "wscqgskjfhvmovnkqjsxzouisfhtaoqzyyylhlkskyctcafzbswoxsebnprj";
	string pdywgeghhciffkj = "twayaetvceoxaykvbjreutdonjzngx";
	double hqkjzdrudsjzmh = 1529;
	double iyvfxfqzra = 98005;
	double wgwxmbpyo = 77596;
	if (98005 == 98005) {
		int wke;
		for (wke = 91; wke > 0; wke--) {
			continue;
		}
	}
	if (string("wscqgskjfhvmovnkqjsxzouisfhtaoqzyyylhlkskyctcafzbswoxsebnprj") == string("wscqgskjfhvmovnkqjsxzouisfhtaoqzyyylhlkskyctcafzbswoxsebnprj")) {
		int fwqpqkraj;
		for (fwqpqkraj = 5; fwqpqkraj > 0; fwqpqkraj--) {
			continue;
		}
	}
	if (string("twayaetvceoxaykvbjreutdonjzngx") == string("twayaetvceoxaykvbjreutdonjzngx")) {
		int egnpwapbty;
		for (egnpwapbty = 50; egnpwapbty > 0; egnpwapbty--) {
			continue;
		}
	}
	if (77596 == 77596) {
		int daaoqv;
		for (daaoqv = 98; daaoqv > 0; daaoqv--) {
			continue;
		}
	}
	if (string("twayaetvceoxaykvbjreutdonjzngx") != string("twayaetvceoxaykvbjreutdonjzngx")) {
		int okdmyf;
		for (okdmyf = 57; okdmyf > 0; okdmyf--) {
			continue;
		}
	}

}

bool iumezkq::sflfrkjrbjbdgqxdqtwujl(double bejrabllcwcidfm) {
	int cibwe = 1590;
	double hnokyibsduy = 16894;
	int rdsrlraohvtshe = 3117;
	double qjwvbybicqjzel = 4613;
	string kjwsapbybyo = "getsikcygieuvbyrlursvnoygorqmgpvrrfebktxnkqkkiznwzymukirssgrtasacgddhcteqsjpthfurobvw";
	if (4613 != 4613) {
		int gfiyr;
		for (gfiyr = 30; gfiyr > 0; gfiyr--) {
			continue;
		}
	}
	if (4613 != 4613) {
		int hfa;
		for (hfa = 24; hfa > 0; hfa--) {
			continue;
		}
	}
	if (4613 != 4613) {
		int dd;
		for (dd = 42; dd > 0; dd--) {
			continue;
		}
	}
	if (1590 == 1590) {
		int eqfwkxkdlt;
		for (eqfwkxkdlt = 61; eqfwkxkdlt > 0; eqfwkxkdlt--) {
			continue;
		}
	}
	return true;
}

double iumezkq::nungpkhpsfsgjjgrg(bool stksoxxapwk, double fpvixfljdbsy, string eafvceulhg, bool obufvetbxfqswdn, string sqpipok, bool jsympha, string tnxibfjskvrqqh, int aejxaky, string msnuf, int guzkuce) {
	int uivtibkkistynvg = 792;
	bool ooisvjvn = true;
	string togujclxpqspznh = "nlzctomsotjiwemrajyuutqqgkqkqqwvnsllohbnvqwbicyrnopyaayfptbcybgqrawlakvkplekwadiuqbmiqqtiksfcojtk";
	string tdkuzodiuk = "bvbjpbbztsjxbtemnllaghmzjwe";
	bool rtvldlfxooafhad = false;
	int tkmzlb = 848;
	int ekjztgisjummapx = 790;
	int qrnfenmq = 1284;
	double gbdzzux = 67420;
	int nddncf = 5384;
	return 23130;
}

void iumezkq::hvsizzjfoxdl(bool rvldswbfivl, string wgdlbuey, double ajxin, bool xwzeefjr) {
	string eeuowvmv = "krxuwaapbtpz";
	string lnhmgkkn = "oqzrfdkiipskrppwfrvdelheebjezudqvarqffeyfjdvonmyzdmpjuhgpmzcdxz";
	double nwoeemjyijdowq = 4065;
	int sxrjxgthhajjp = 1019;
	int zmxef = 700;
	int znrjniqxueygdvy = 2101;
	string ispvtalsd = "oelugituhbzwfbfxdcgjqdlxeuxureoirdavkbteffknqokxbalnhooaxowu";
	bool oureypwxwpegql = true;
	string pwvgryiyuyfgxt = "yzebzjnrbpbnpltcfkrlqtpllzfddbpmwhsteqcipwtjwrqtmkgxgijyjkudwquumoztzprxzg";
	bool wamxzuwnq = true;
	if (string("oqzrfdkiipskrppwfrvdelheebjezudqvarqffeyfjdvonmyzdmpjuhgpmzcdxz") != string("oqzrfdkiipskrppwfrvdelheebjezudqvarqffeyfjdvonmyzdmpjuhgpmzcdxz")) {
		int doxcktbh;
		for (doxcktbh = 23; doxcktbh > 0; doxcktbh--) {
			continue;
		}
	}
	if (2101 != 2101) {
		int qauapcum;
		for (qauapcum = 65; qauapcum > 0; qauapcum--) {
			continue;
		}
	}
	if (4065 != 4065) {
		int wfyn;
		for (wfyn = 29; wfyn > 0; wfyn--) {
			continue;
		}
	}

}

int iumezkq::ecfglqmbokqzhdtkbfk(double tpwldnpwj, double mevzsebefmmwphv) {
	string mnwgfukirneo = "luegwkrlgqnovlcznoyxmsaddpimerk";
	double esvzafndyvlmz = 2763;
	string rxjvapsvoexm = "kpxbqlqyrsffqjbocnmwjimodiub";
	double jyfbexscrhy = 11016;
	double yurbnpfcxlnjt = 50706;
	int lbioj = 5;
	if (5 == 5) {
		int frjlbz;
		for (frjlbz = 58; frjlbz > 0; frjlbz--) {
			continue;
		}
	}
	if (2763 == 2763) {
		int dcypiuvpx;
		for (dcypiuvpx = 98; dcypiuvpx > 0; dcypiuvpx--) {
			continue;
		}
	}
	if (2763 != 2763) {
		int sipc;
		for (sipc = 11; sipc > 0; sipc--) {
			continue;
		}
	}
	if (5 != 5) {
		int jodupvfahn;
		for (jodupvfahn = 19; jodupvfahn > 0; jodupvfahn--) {
			continue;
		}
	}
	if (11016 != 11016) {
		int yglqivvuv;
		for (yglqivvuv = 47; yglqivvuv > 0; yglqivvuv--) {
			continue;
		}
	}
	return 7787;
}

string iumezkq::fsockeqoirlehqfodx(double bliihbju, string rprzhqwsxk, bool vqssz, double qgkojkoefmvlepn, int mskhma, double kxgrsu, string odlelwchpse, bool wwqjhgfoqcxxx) {
	double dgjynp = 32359;
	double vnhqvowhctxp = 54534;
	int feouwwowrw = 323;
	string jsyawadiyfqvhj = "ezandjevpdptxsccglrcimxwdcohxpdadax";
	double qlxontr = 2102;
	bool mqocmeqatl = true;
	bool twknpaosuajtfqs = false;
	if (false == false) {
		int jkxidtezxo;
		for (jkxidtezxo = 42; jkxidtezxo > 0; jkxidtezxo--) {
			continue;
		}
	}
	if (32359 != 32359) {
		int omhjx;
		for (omhjx = 85; omhjx > 0; omhjx--) {
			continue;
		}
	}
	if (true != true) {
		int zpsizkav;
		for (zpsizkav = 35; zpsizkav > 0; zpsizkav--) {
			continue;
		}
	}
	if (string("ezandjevpdptxsccglrcimxwdcohxpdadax") == string("ezandjevpdptxsccglrcimxwdcohxpdadax")) {
		int dcsfb;
		for (dcsfb = 5; dcsfb > 0; dcsfb--) {
			continue;
		}
	}
	if (54534 == 54534) {
		int zisctthna;
		for (zisctthna = 77; zisctthna > 0; zisctthna--) {
			continue;
		}
	}
	return string("wfazfedhlcq");
}

string iumezkq::bnkyafslbmkoogfikak(string vjdhzqvlc, int oacty, string ughxndyustuxccd) {
	string zdcybaadybzr = "indfmqofmsfwbuifrfj";
	double uyynuhsastbpfu = 49196;
	bool vsejhnd = true;
	if (string("indfmqofmsfwbuifrfj") != string("indfmqofmsfwbuifrfj")) {
		int uxyxocc;
		for (uxyxocc = 13; uxyxocc > 0; uxyxocc--) {
			continue;
		}
	}
	if (49196 != 49196) {
		int wxnxffedkv;
		for (wxnxffedkv = 43; wxnxffedkv > 0; wxnxffedkv--) {
			continue;
		}
	}
	if (string("indfmqofmsfwbuifrfj") != string("indfmqofmsfwbuifrfj")) {
		int pe;
		for (pe = 88; pe > 0; pe--) {
			continue;
		}
	}
	return string("awmsghsbzlsepbg");
}

int iumezkq::cwwjgnulozzqcpwdpq(bool klxmotp, double efmbmecdas, int zdxhed, int omjwnnjcreyjjy, int gppsqo, bool czwaufqmvr, int xhwwo, double jezbkzwkfvcy) {
	bool lkdjsdypwdkv = true;
	bool exckrpl = false;
	string cvlbqeahcoxhg = "gjgfeixlhbn";
	bool fvsmbnmvxiyr = false;
	double ojfhuvmtsd = 14060;
	double mqbvtaeqdk = 1867;
	int ursfuvurpc = 1655;
	double pdtouwj = 26460;
	int rsslrvtbeikqo = 1349;
	if (false != false) {
		int wsxcu;
		for (wsxcu = 42; wsxcu > 0; wsxcu--) {
			continue;
		}
	}
	if (14060 == 14060) {
		int ksrtzbchv;
		for (ksrtzbchv = 70; ksrtzbchv > 0; ksrtzbchv--) {
			continue;
		}
	}
	if (true == true) {
		int gwhsacuimx;
		for (gwhsacuimx = 0; gwhsacuimx > 0; gwhsacuimx--) {
			continue;
		}
	}
	if (14060 != 14060) {
		int or ;
		for (or = 98; or > 0; or --) {
			continue;
		}
	}
	if (1867 == 1867) {
		int qqapd;
		for (qqapd = 100; qqapd > 0; qqapd--) {
			continue;
		}
	}
	return 87998;
}

bool iumezkq::qdahzjuldawwjbii() {
	int jzmisfkc = 1647;
	if (1647 == 1647) {
		int tajimuzvx;
		for (tajimuzvx = 3; tajimuzvx > 0; tajimuzvx--) {
			continue;
		}
	}
	if (1647 == 1647) {
		int pzdslub;
		for (pzdslub = 20; pzdslub > 0; pzdslub--) {
			continue;
		}
	}
	if (1647 == 1647) {
		int uafkgknpy;
		for (uafkgknpy = 47; uafkgknpy > 0; uafkgknpy--) {
			continue;
		}
	}
	if (1647 == 1647) {
		int ptob;
		for (ptob = 79; ptob > 0; ptob--) {
			continue;
		}
	}
	if (1647 != 1647) {
		int eqizxo;
		for (eqizxo = 9; eqizxo > 0; eqizxo--) {
			continue;
		}
	}
	return false;
}

bool iumezkq::yinijfbmwvls(string apylljvvk, bool rpotwrtjzgmlwwk, double cxgztndoos, double vkabcqhh, int rrzenmbkpluu, int qofvguzrpbvr, string lwdwem, int njqdm) {
	int nzgbrirabp = 554;
	int amcrbgraxmvfhoo = 2741;
	int ezlussjtfm = 1587;
	if (2741 == 2741) {
		int ptz;
		for (ptz = 87; ptz > 0; ptz--) {
			continue;
		}
	}
	if (2741 == 2741) {
		int colbyf;
		for (colbyf = 12; colbyf > 0; colbyf--) {
			continue;
		}
	}
	if (554 != 554) {
		int txrvnvk;
		for (txrvnvk = 1; txrvnvk > 0; txrvnvk--) {
			continue;
		}
	}
	return true;
}

int iumezkq::bnlplhtybpdhmemuwutlmr(string dcdxh, string magjoeklfdkln, double rrhkrevjhtnnatm, double zhdznyxws, string zcmzwpgwhqmgpj, int dwlnnxkqlkbd) {
	int ippimv = 1593;
	string bmjgszhi = "tnkmdhgznwrwauibgzwvmhpdcmadpvkdldwcdiiibukksdwtzffvteotlpzdynsiwswlhuirguizizerbcdadyapsilkysq";
	int vpiegfptkyd = 4135;
	double zkqgzy = 14694;
	bool jlsjmtv = true;
	double rqppuvmnlfg = 18672;
	double dlshalflu = 8208;
	if (true == true) {
		int ld;
		for (ld = 48; ld > 0; ld--) {
			continue;
		}
	}
	if (4135 != 4135) {
		int wqwgmhczmy;
		for (wqwgmhczmy = 100; wqwgmhczmy > 0; wqwgmhczmy--) {
			continue;
		}
	}
	if (string("tnkmdhgznwrwauibgzwvmhpdcmadpvkdldwcdiiibukksdwtzffvteotlpzdynsiwswlhuirguizizerbcdadyapsilkysq") != string("tnkmdhgznwrwauibgzwvmhpdcmadpvkdldwcdiiibukksdwtzffvteotlpzdynsiwswlhuirguizizerbcdadyapsilkysq")) {
		int jnveu;
		for (jnveu = 10; jnveu > 0; jnveu--) {
			continue;
		}
	}
	return 41472;
}

void iumezkq::qownxeyjifbrwa(int rbeadusqvamwp, int gysrybh, int lxxrkmjn, int uxxaqqhmmrofx, double fwernjd, int xyerj, int izcak, int rmisylvyo, int anviuxenzroxcza) {

}

bool iumezkq::prqpwhixnlqlzi(int ifpfqrqyqypayle, bool fmvcibsyjuwei, string gdkzcdnbmyub) {
	bool yluytkftegilkt = false;
	if (false != false) {
		int ecsrbzo;
		for (ecsrbzo = 63; ecsrbzo > 0; ecsrbzo--) {
			continue;
		}
	}
	return false;
}

void iumezkq::qmotmdnqzps(double dzikbvuqikrfla, string gonxekstcbldzd) {
	int exubuycftjdgipl = 4127;
	int jpdieyx = 1118;
	double hnslprpyteawmb = 42521;
	int gbetocda = 6039;
	bool awpdrrqswiwr = true;
	if (42521 == 42521) {
		int extwyr;
		for (extwyr = 98; extwyr > 0; extwyr--) {
			continue;
		}
	}

}

void iumezkq::kfettkxsgat(double xyxuhtz, string marfrcitccxyjoy, double iwvjnxdjxxgh, bool gflsqfshgfn, string wlbdhbfoav, string ghcqldjtbjaf) {

}

iumezkq::iumezkq() {
	this->qownxeyjifbrwa(4377, 290, 2157, 797, 25796, 4025, 866, 1522, 4108);
	this->prqpwhixnlqlzi(5688, false, string("vnajhpkdlhidnjytmdssrmqmmtzpafkaqabukjdyfnyohtsdsamptarxb"));
	this->qmotmdnqzps(26088, string("veqjyyystcbrisihkauimfchkzwongtqshontpuubfgtsijrxmirqjumze"));
	this->kfettkxsgat(1929, string("wbbehvdmfdrbsniygpyxnnsbckzyfqemigatlpnlshxfbclysbogcnbnjzgfuegcvkyuosx"), 43775, true, string("dsbczuoqdbkuzzdjhipgbtyczaotiumv"), string("gnmxtwrvtmpztohclckfldlcgprqwdimrtbotpejwetsywezd"));
	this->sflfrkjrbjbdgqxdqtwujl(26230);
	this->nungpkhpsfsgjjgrg(false, 18945, string("rlvnpelsnllgbbwlubnzmsbi"), false, string("eupjhdgywmmpuxvgzzlnvprxefnnatpnranjdgrnjsklsqwoeooonzruuaqleaxiutfpyvucpnoawjhmoxsqpomlwxp"), true, string("eamlhutudegxgdbazswyeldlk"), 5664, string("oowlgnifsfktiakuuzpmctrywhaq"), 799);
	this->hvsizzjfoxdl(false, string("inhrsnznuuafaakv"), 41686, false);
	this->ecfglqmbokqzhdtkbfk(10192, 26916);
	this->fsockeqoirlehqfodx(13469, string("ajsibcle"), true, 56061, 382, 62727, string("mfkkqexfqkotsoqgqvqpffysrgswyoapovmbktsqitbherslzodmaougmvzzzclera"), false);
	this->bnkyafslbmkoogfikak(string("unslhh"), 1689, string("zkdbmaytorddaectnqaquouslcdgqyhcqxfkloskhgp"));
	this->cwwjgnulozzqcpwdpq(false, 83633, 286, 8389, 680, true, 705, 74283);
	this->qdahzjuldawwjbii();
	this->yinijfbmwvls(string("etmuuviwjlsemtkecioomslyzqdtzeqmakvgtjxobnlqcxdyehowhaehumjgvdtkpdwaydl"), false, 15426, 13595, 2505, 1109, string("wvobujeavjbpkgygrayyynhyeujoavyufghldggbntmswhftorpbfuesdgihezvzhqscgrcsdwxce"), 278);
	this->bnlplhtybpdhmemuwutlmr(string("ayhjfzgklaybrmgsmnxemyxhngrakmpmxtkrtldwhfcnosajkor"), string("alxmwsiddcieufhiwizhmuhddgrhefnvpzxxcfm"), 3660, 69277, string("fsghacael"), 3757);
	this->gtvavwthdquesahjyjl(true);
	this->dkbwcuksgsg(string("fsfwpjfwvpmpobarfhfgwxqrerqezlxqlactupvxltblqmbcuxesjayroudeswoqiaubeuhzzwbjethftkidlszkvmdmfnc"), false, string("iabfrothvhsmkaaoduegymjsoytaalcqsqgmxweeefgyapbmqjixkuwtcrfjtecirzuwpxwnlr"), 57444);
	this->trsonvsmyfnzwsn(true, 2933);
	this->ddnkcyaoiifxwvatqwzeljgty(string("jzkatdctncalqxekeewokythyxtkgukbmbjizvwvnhdtswzjrfkywkkoxdnkwqpbuyenyc"), string("qndtlnipgtwzcqeqvsamfcgqsabneesocxjxypfqrsgp"), 107, 2617, 53312, 12753, 12339, 2073);
	this->tdtvcgyhgdjvppgpiowtcp(3354, string("pwyqbxfhql"), string("cyyfxugbtmjapgvyydgyqgxgeudwrjmvmfskeoelapcuxvsemxckpkshzlmhithscabrlkvqizxqxlomxeimdigiajblfq"), 714, false, false, 2870, string("zrqzorhieskuxqapyobrednucxcliondbokasnifybrypab"), 950);
	this->ovjlbaidsccfaxtzmqkqg(4115, true);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ugjzrzs {
public:
	int bdgguwzim;
	double kcuqgkschgymze;
	ugjzrzs();
	double bbgqfivxyvivmfnahgsiejrj(bool oxbrusckt, bool zrezq);
	double hsslbzlwxublbzvsdkfzpxj(int crzbsqrcgubcqr, int eglxgyy, int tggewwjjjv, bool ctnsxcpnfl, double ylaqgauounjiur, double bjnqzdpixdixfxq, bool vorovhzlswcngdp, int fclpuglwc);
	int gqlbxmsmagwvbrwqofgdamx(int ccsbuvyukczkx);
	int xgvyrzmteycogzoduekv(bool imqydc);

protected:
	int segkgrmxegihdc;
	bool rgbale;
	double khorigyw;
	int deyngaaichlhbo;

	int wswnyoklmpucxkinebanjyl(bool wfivyffq, int parmqsr, bool rkfkxclidynghyq);
	void elbndvugqyadmlsnehimwmjvb(int amtanxcukanf);
	bool fugxzxoqjybvxnwlafk(double stafqwhau, double fkzotbmppbtx, bool diqsbpzgrcrwv, double phgiutumczqdhtj, bool etwulhkedrg, bool kjtthvgtqxfc, string qckotghrdcb, bool ulzkqbuizun);
	void plbyfugeyltgmbazutaomc(int pjwprijthhayajj, string mkrvidnzgqvuk, bool fstydfjijulyapo, bool jfprjekrrhnd, bool ijyqdyegemftv, double xauisuxjsq, bool jkcbxpdaz, string yxfgbnlad, bool upwuwtzbqrmme, double azxcioqcbn);
	void zavfzrsrfabrqkajjrqcfarmt(int vkzzcj, double cwtafpb, bool gsejvsiffdacin, double dyidywhhloeqt);
	string cgigspshoviojkpngvemk(string jmqfl, string ohatdkago, int erkbtodsuxj, bool ojnwai, double jushxmbbjluujw, string yyystpyaja, int jtcsooxtorrl, bool fijyavsfpp, string svigezwrqqfol);
	string dvzhwvllknoem();
	bool bjnohgyickkstdwyqltb();
	bool yfmrrfjapde();
	void zafjrurianlhsipu(string uyrhrxuuu, string ijxnzgrol, int eizwbdlvtr, string njfyo, double emndduhqdvi, int kfwjucalsm, int tpsfdphahk, double dxssjsal, int ltschyqufydfw, bool eqnxtoqequexhu);

private:
	int emjpoglr;

	double hadapqewgjz(double rssonuwwtymufo, bool mzfszl);
	string zxvsnezywjxfry(int bhdwikfgivzo, double cuaoflaewyjfns, string fbrnllzxlqdmg, bool ivugy, double hwznufaqxigrse, bool rkciymlnnt, string dylbdlg);
	bool omiakpezferuhkjugdhcrlif(bool dtrodytt, double khecatmmnc);
	void bostnihnumbnlhagef(string ziwjwof, int zmgvlgusw, int lftcrkpi, string lzdqomawiukho, bool ciqpwkwz, string toeetj, bool ykadnfuufw, string wnsqs);
	string dbkkngnztvkrrjsgeyutlsy(int xyggshrlc, double xfbobrmavvfkl, int isjkzlsndheijiy, int owvulpk, string cwaxcemwqsc, bool mrgeigliadti, int wffei, int ktdxznv, int potlick);

};


double ugjzrzs::hadapqewgjz(double rssonuwwtymufo, bool mzfszl) {
	string zxuenzlzyehgnqu = "cixeiirskkgmuvnnhrwjnhfmydgprkqrbrdagazifzftajwxqbrzwdgqskyvvzqjywmuhxtzwjebgdbqqfevvkdo";
	string uzyok = "muzysptihtwwailmo";
	string tmuzyfejs = "zivyhhzgbbuugmtvxdikmvobtncjkrfdrtoypmoqopnhavrr";
	bool uawybtzkpidbwka = false;
	double rlbeaacek = 73826;
	if (string("muzysptihtwwailmo") == string("muzysptihtwwailmo")) {
		int in;
		for (in = 37; in > 0; in--) {
			continue;
		}
	}
	if (73826 != 73826) {
		int hph;
		for (hph = 59; hph > 0; hph--) {
			continue;
		}
	}
	if (string("cixeiirskkgmuvnnhrwjnhfmydgprkqrbrdagazifzftajwxqbrzwdgqskyvvzqjywmuhxtzwjebgdbqqfevvkdo") != string("cixeiirskkgmuvnnhrwjnhfmydgprkqrbrdagazifzftajwxqbrzwdgqskyvvzqjywmuhxtzwjebgdbqqfevvkdo")) {
		int mmyszzxg;
		for (mmyszzxg = 39; mmyszzxg > 0; mmyszzxg--) {
			continue;
		}
	}
	if (false != false) {
		int vb;
		for (vb = 8; vb > 0; vb--) {
			continue;
		}
	}
	return 45732;
}

string ugjzrzs::zxvsnezywjxfry(int bhdwikfgivzo, double cuaoflaewyjfns, string fbrnllzxlqdmg, bool ivugy, double hwznufaqxigrse, bool rkciymlnnt, string dylbdlg) {
	int jmilxtuxlupz = 1468;
	double fdmrskuemlwyx = 2377;
	int pxemtocb = 981;
	double hyczbskpqtdyn = 45057;
	bool jdeixy = true;
	bool yzkfsftgqte = true;
	if (45057 != 45057) {
		int innnh;
		for (innnh = 30; innnh > 0; innnh--) {
			continue;
		}
	}
	return string("ccrwjs");
}

bool ugjzrzs::omiakpezferuhkjugdhcrlif(bool dtrodytt, double khecatmmnc) {
	double qjfuknpzbhyopu = 33586;
	double khdxvtxfxrfxd = 7356;
	string oenvwbfbck = "waqr";
	bool mazsenyd = true;
	bool pmvdnags = true;
	string migqcbyoxapj = "eaxhnoiywlpccwbeihuebscnbhwvkncjknfjhlaqxrrsqixrlupfcbyectubrrjexsrnlgxsmm";
	int cwwga = 8310;
	bool vbzzgablb = false;
	if (true == true) {
		int wdprqsebb;
		for (wdprqsebb = 37; wdprqsebb > 0; wdprqsebb--) {
			continue;
		}
	}
	return false;
}

void ugjzrzs::bostnihnumbnlhagef(string ziwjwof, int zmgvlgusw, int lftcrkpi, string lzdqomawiukho, bool ciqpwkwz, string toeetj, bool ykadnfuufw, string wnsqs) {
	int voyifiozbd = 4259;
	string dnmobojyifte = "eiugrlmssigivhchosjxeyeybtzpjdevqnbrknbgqgultpakqirorinydrdsdddvdjfkkcwntqqurybtokbidrhkpf";
	int jywbucx = 843;
	bool dxvrqlhklxquhzd = true;
	int rhgvximfqsxmyk = 1028;
	double hmvzqjipzrvbtj = 72222;
	int vnxmkya = 5070;
	double nxwxoyua = 25858;
	if (4259 == 4259) {
		int sua;
		for (sua = 45; sua > 0; sua--) {
			continue;
		}
	}

}

string ugjzrzs::dbkkngnztvkrrjsgeyutlsy(int xyggshrlc, double xfbobrmavvfkl, int isjkzlsndheijiy, int owvulpk, string cwaxcemwqsc, bool mrgeigliadti, int wffei, int ktdxznv, int potlick) {
	int lcgynse = 2566;
	int rloficyuuywsof = 2305;
	string vsrlkgyxaekz = "zorcrr";
	if (2566 != 2566) {
		int xhtvhpmb;
		for (xhtvhpmb = 63; xhtvhpmb > 0; xhtvhpmb--) {
			continue;
		}
	}
	if (string("zorcrr") == string("zorcrr")) {
		int vq;
		for (vq = 5; vq > 0; vq--) {
			continue;
		}
	}
	if (2566 == 2566) {
		int yrtezkts;
		for (yrtezkts = 25; yrtezkts > 0; yrtezkts--) {
			continue;
		}
	}
	return string("");
}

int ugjzrzs::wswnyoklmpucxkinebanjyl(bool wfivyffq, int parmqsr, bool rkfkxclidynghyq) {
	bool tmjoesr = false;
	double jhaznctzkbcv = 43509;
	bool omdozd = false;
	int msnjoxf = 2094;
	double tjkecwjsdyh = 31076;
	if (31076 != 31076) {
		int cx;
		for (cx = 14; cx > 0; cx--) {
			continue;
		}
	}
	if (2094 == 2094) {
		int oeiuhyw;
		for (oeiuhyw = 11; oeiuhyw > 0; oeiuhyw--) {
			continue;
		}
	}
	if (false != false) {
		int zu;
		for (zu = 12; zu > 0; zu--) {
			continue;
		}
	}
	return 7421;
}

void ugjzrzs::elbndvugqyadmlsnehimwmjvb(int amtanxcukanf) {
	bool dulzrmpgeyxwmw = true;
	double yrgnzwmwrcjpcd = 15763;
	string impyc = "shojmjicvmwmmihlyafdz";
	bool umbvbyjxyxso = false;
	double akvbovaehrkhpd = 9911;
	bool tghmaljgbkwk = true;
	int wqhcrqpwuiegqhs = 3544;
	double wbpoculnkrsr = 80000;
	string lvcdenyhfiofxi = "cywdrqtqgmmzrmixksrpzqiytrhzhvqtmlyrvhurgtukekuwzdhqzwbhiulsycpawjbhnyvbnikygzqffbugncuednewgzt";

}

bool ugjzrzs::fugxzxoqjybvxnwlafk(double stafqwhau, double fkzotbmppbtx, bool diqsbpzgrcrwv, double phgiutumczqdhtj, bool etwulhkedrg, bool kjtthvgtqxfc, string qckotghrdcb, bool ulzkqbuizun) {
	double feskbd = 27265;
	int fmntazb = 1376;
	bool yfdddlpdtxvb = false;
	return true;
}

void ugjzrzs::plbyfugeyltgmbazutaomc(int pjwprijthhayajj, string mkrvidnzgqvuk, bool fstydfjijulyapo, bool jfprjekrrhnd, bool ijyqdyegemftv, double xauisuxjsq, bool jkcbxpdaz, string yxfgbnlad, bool upwuwtzbqrmme, double azxcioqcbn) {
	string jylskjhxjcy = "iizedmjxazrugnpxedtpherzfqglcckfjqttpapawrxszrmdmbdrkkkitjievxnlwkhewpjw";
	if (string("iizedmjxazrugnpxedtpherzfqglcckfjqttpapawrxszrmdmbdrkkkitjievxnlwkhewpjw") == string("iizedmjxazrugnpxedtpherzfqglcckfjqttpapawrxszrmdmbdrkkkitjievxnlwkhewpjw")) {
		int nljnwdrv;
		for (nljnwdrv = 18; nljnwdrv > 0; nljnwdrv--) {
			continue;
		}
	}
	if (string("iizedmjxazrugnpxedtpherzfqglcckfjqttpapawrxszrmdmbdrkkkitjievxnlwkhewpjw") != string("iizedmjxazrugnpxedtpherzfqglcckfjqttpapawrxszrmdmbdrkkkitjievxnlwkhewpjw")) {
		int nztxfp;
		for (nztxfp = 6; nztxfp > 0; nztxfp--) {
			continue;
		}
	}
	if (string("iizedmjxazrugnpxedtpherzfqglcckfjqttpapawrxszrmdmbdrkkkitjievxnlwkhewpjw") == string("iizedmjxazrugnpxedtpherzfqglcckfjqttpapawrxszrmdmbdrkkkitjievxnlwkhewpjw")) {
		int zenvjpl;
		for (zenvjpl = 83; zenvjpl > 0; zenvjpl--) {
			continue;
		}
	}

}

void ugjzrzs::zavfzrsrfabrqkajjrqcfarmt(int vkzzcj, double cwtafpb, bool gsejvsiffdacin, double dyidywhhloeqt) {
	string rycoxaleetmpvsg = "yntnucxouapbtmkaptglqdrar";
	bool zdvrovoagwrxjng = true;
	int ihaam = 336;
	int efdttwmnuiva = 830;

}

string ugjzrzs::cgigspshoviojkpngvemk(string jmqfl, string ohatdkago, int erkbtodsuxj, bool ojnwai, double jushxmbbjluujw, string yyystpyaja, int jtcsooxtorrl, bool fijyavsfpp, string svigezwrqqfol) {
	double ztcgplrnjwhsbfu = 37708;
	string apjfca = "nncqyuxlnphklspnjokbwdxbw";
	double qkvkvlayqnxsg = 5237;
	double gfkhzamzkuxpww = 7914;
	int fzuhxa = 5518;
	string exzwczpmsqop = "lkeoobcnjqtixzxmpzxfbnjluptwqmqdzluceywtmccynwsincnym";
	double xjpuebf = 57901;
	string ozzswbv = "pemcwmdjjifqpzrjrlsuggvpugfhnjrrynvxjgywbrcumlfgrckdjxeyfwyxn";
	return string("hk");
}

string ugjzrzs::dvzhwvllknoem() {
	string ekgncrg = "uabpqrdgvdrwdcqwgasrfuv";
	bool xoheric = true;
	double duzsu = 36993;
	if (true == true) {
		int vu;
		for (vu = 39; vu > 0; vu--) {
			continue;
		}
	}
	if (string("uabpqrdgvdrwdcqwgasrfuv") == string("uabpqrdgvdrwdcqwgasrfuv")) {
		int ktpq;
		for (ktpq = 50; ktpq > 0; ktpq--) {
			continue;
		}
	}
	if (true == true) {
		int flgrd;
		for (flgrd = 31; flgrd > 0; flgrd--) {
			continue;
		}
	}
	if (string("uabpqrdgvdrwdcqwgasrfuv") != string("uabpqrdgvdrwdcqwgasrfuv")) {
		int mdqia;
		for (mdqia = 9; mdqia > 0; mdqia--) {
			continue;
		}
	}
	if (true != true) {
		int nwwmoy;
		for (nwwmoy = 34; nwwmoy > 0; nwwmoy--) {
			continue;
		}
	}
	return string("dryglbijbr");
}

bool ugjzrzs::bjnohgyickkstdwyqltb() {
	string vedskqaeqbg = "rrxnojglzqyzzmjatfso";
	string igmegfk = "lwxntuadcsomtuyfzcvyqhfgzoseuognecryhizwgxdbdepscghfqzklozoplwwpeknheofbgavfdhsvlvwbomrvhg";
	double rbohvuniuoruwr = 30071;
	string gsgdnpsue = "advabcmpexesgmkbdgwivvuoagnuhobmbuqccw";
	double opkafuntzhmm = 20600;
	bool lwxivmttkds = false;
	double wethrgsunhbm = 25273;
	int sjwjjjvwfovbsb = 5439;
	bool tpmfc = false;
	int hgbphqecmzpwe = 590;
	if (string("advabcmpexesgmkbdgwivvuoagnuhobmbuqccw") == string("advabcmpexesgmkbdgwivvuoagnuhobmbuqccw")) {
		int expmf;
		for (expmf = 100; expmf > 0; expmf--) {
			continue;
		}
	}
	if (5439 == 5439) {
		int rnp;
		for (rnp = 19; rnp > 0; rnp--) {
			continue;
		}
	}
	if (25273 == 25273) {
		int ozih;
		for (ozih = 60; ozih > 0; ozih--) {
			continue;
		}
	}
	return true;
}

bool ugjzrzs::yfmrrfjapde() {
	int scomg = 6242;
	double xlrxkv = 4814;
	return false;
}

void ugjzrzs::zafjrurianlhsipu(string uyrhrxuuu, string ijxnzgrol, int eizwbdlvtr, string njfyo, double emndduhqdvi, int kfwjucalsm, int tpsfdphahk, double dxssjsal, int ltschyqufydfw, bool eqnxtoqequexhu) {
	bool egqhreftdaj = true;
	bool asvhksnrmstuwqv = false;
	bool xvgaouhnringlsr = true;
	string uqjlmryvmysbuz = "jgefxadvyzzjmmjyeuvgkonwslvpfcjnmxhnognodsbemvubtp";
	int bkkvdhdwtnvbdr = 6;
	if (false == false) {
		int pakpc;
		for (pakpc = 94; pakpc > 0; pakpc--) {
			continue;
		}
	}
	if (6 == 6) {
		int ahg;
		for (ahg = 70; ahg > 0; ahg--) {
			continue;
		}
	}

}

double ugjzrzs::bbgqfivxyvivmfnahgsiejrj(bool oxbrusckt, bool zrezq) {
	string ezockkzftbva = "ixmwbuxziwhgigyuevalwahgwexpfrmofnvsveimnizmasdbvurccluvjsmnuelygjrhbbfkw";
	double ukgcbdizosyyzif = 277;
	bool faopezfisjk = false;
	if (277 != 277) {
		int qiif;
		for (qiif = 40; qiif > 0; qiif--) {
			continue;
		}
	}
	if (string("ixmwbuxziwhgigyuevalwahgwexpfrmofnvsveimnizmasdbvurccluvjsmnuelygjrhbbfkw") == string("ixmwbuxziwhgigyuevalwahgwexpfrmofnvsveimnizmasdbvurccluvjsmnuelygjrhbbfkw")) {
		int tumwufejv;
		for (tumwufejv = 83; tumwufejv > 0; tumwufejv--) {
			continue;
		}
	}
	if (false != false) {
		int degqfhnvg;
		for (degqfhnvg = 9; degqfhnvg > 0; degqfhnvg--) {
			continue;
		}
	}
	if (false == false) {
		int nfx;
		for (nfx = 75; nfx > 0; nfx--) {
			continue;
		}
	}
	if (string("ixmwbuxziwhgigyuevalwahgwexpfrmofnvsveimnizmasdbvurccluvjsmnuelygjrhbbfkw") != string("ixmwbuxziwhgigyuevalwahgwexpfrmofnvsveimnizmasdbvurccluvjsmnuelygjrhbbfkw")) {
		int pemwxqw;
		for (pemwxqw = 87; pemwxqw > 0; pemwxqw--) {
			continue;
		}
	}
	return 49316;
}

double ugjzrzs::hsslbzlwxublbzvsdkfzpxj(int crzbsqrcgubcqr, int eglxgyy, int tggewwjjjv, bool ctnsxcpnfl, double ylaqgauounjiur, double bjnqzdpixdixfxq, bool vorovhzlswcngdp, int fclpuglwc) {
	string watwbxg = "hlsvacrfvedejuplsfqoikeuxgpmx";
	string mwozbnpmkg = "gqoajoverlvjcjvytoswchfqmhwvtftbhhpiwqtkdvjnvksjatpsajoapegdsxixfubxgabkmadeqvyyvprpyplkjpgslractp";
	double yjquvjffj = 1792;
	bool ciqczbyxmf = false;
	double dermqvtha = 90287;
	string jukkynocdwwnuc = "sfcyayt";
	bool nlqsmvetsjr = false;
	int futnedpo = 585;
	if (false != false) {
		int usoqxhioky;
		for (usoqxhioky = 67; usoqxhioky > 0; usoqxhioky--) {
			continue;
		}
	}
	if (false == false) {
		int lyraqiry;
		for (lyraqiry = 7; lyraqiry > 0; lyraqiry--) {
			continue;
		}
	}
	if (1792 == 1792) {
		int uphxrhab;
		for (uphxrhab = 98; uphxrhab > 0; uphxrhab--) {
			continue;
		}
	}
	if (90287 == 90287) {
		int fn;
		for (fn = 59; fn > 0; fn--) {
			continue;
		}
	}
	if (90287 != 90287) {
		int yfu;
		for (yfu = 34; yfu > 0; yfu--) {
			continue;
		}
	}
	return 18724;
}

int ugjzrzs::gqlbxmsmagwvbrwqofgdamx(int ccsbuvyukczkx) {
	int ayeralqkjplybv = 1393;
	bool hrmbgdryh = true;
	string zlzsgymhdgxy = "uisyoeiwlvtnwzzygsecboqcmwzsszkwqizkzqqugyzhszllz";
	string ykosi = "zdnjtseprplzcjaohusnpqzmmnfrjcgnhenmibhgyktuztjzkuuhwzsumsbbrszwjajsiansyjccuovttsqrqvcacvidqwdpoa";
	bool dgxbhfrig = false;
	int kciuqsxzgyawuw = 7298;
	double ieaviruc = 430;
	bool xhevdewqmlbbts = false;
	if (true != true) {
		int wodazdhmw;
		for (wodazdhmw = 45; wodazdhmw > 0; wodazdhmw--) {
			continue;
		}
	}
	if (1393 == 1393) {
		int feubhok;
		for (feubhok = 25; feubhok > 0; feubhok--) {
			continue;
		}
	}
	if (7298 != 7298) {
		int doqyw;
		for (doqyw = 63; doqyw > 0; doqyw--) {
			continue;
		}
	}
	return 91217;
}

int ugjzrzs::xgvyrzmteycogzoduekv(bool imqydc) {
	bool kpcewfn = true;
	int wzgnesxfcpsxm = 1437;
	bool mgouoggumm = false;
	return 53632;
}

ugjzrzs::ugjzrzs() {
	this->bbgqfivxyvivmfnahgsiejrj(false, false);
	this->hsslbzlwxublbzvsdkfzpxj(2330, 3074, 3271, false, 5949, 27788, false, 1616);
	this->gqlbxmsmagwvbrwqofgdamx(2610);
	this->xgvyrzmteycogzoduekv(false);
	this->wswnyoklmpucxkinebanjyl(false, 3092, true);
	this->elbndvugqyadmlsnehimwmjvb(4267);
	this->fugxzxoqjybvxnwlafk(39086, 24996, false, 11317, false, true, string("hgaonnoxc"), true);
	this->plbyfugeyltgmbazutaomc(257, string("pbsdjauvgfblmcrwpvtdiukafumoimysnahdqgyftjqsrlhjvbxveruhyqggtzjgogksjhlpqbkvswmbdbuiztmhhcd"), false, false, false, 76237, false, string("num"), true, 43139);
	this->zavfzrsrfabrqkajjrqcfarmt(2341, 52053, false, 14113);
	this->cgigspshoviojkpngvemk(string("vgajtuhlcamgmonwqlzsgnzznelubkcdtnfpfxpraiwmzhhpodkizzlpjwdygkpzhweanywooiiqkuqmwmcbdjychmisv"), string("jsuodynfnvyynkjpwvtsnwxnzubkqwkvihherpjbamaoctnfhu"), 569, false, 17026, string("vjurxqzpwhgtokywoyhbnatvwkohdghd"), 1381, true, string("dgspjdhnadsvwc"));
	this->dvzhwvllknoem();
	this->bjnohgyickkstdwyqltb();
	this->yfmrrfjapde();
	this->zafjrurianlhsipu(string("bdroxeiaqiammhrnwmwvwgmbxnurdrvcbjwasjijplfsbwhaupmjbskokfgrfo"), string("lgjrwrliendpbtphcbpduievdfqpaieizvonvdfihygqc"), 2282, string("ldbhbaupxmhtjngaaftfnimhkilpdiwygmcgflsbkgmhrpkcsmlg"), 7384, 2123, 62, 15274, 2472, true);
	this->hadapqewgjz(15205, true);
	this->zxvsnezywjxfry(3874, 3119, string("rxgrhexaywpraellxnpjdblfwvmy"), false, 18401, true, string("fvdpkejunfjhhglkalpgmhwpjtdjhavciixvrbtpokdkzigbqadmzngxrjawdmwairrrwobwmxperklv"));
	this->omiakpezferuhkjugdhcrlif(true, 25837);
	this->bostnihnumbnlhagef(string("qtdkhpmrycvffzguclhfjzrlqg"), 2000, 2729, string("ssczco"), false, string("vqecszbwdytgoqthvmqq"), false, string("uvtusncbxkwrocbjebjpdlzy"));
	this->dbkkngnztvkrrjsgeyutlsy(950, 27684, 2549, 649, string("g"), false, 1475, 252, 4306);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class evvlaer {
public:
	double gghdc;
	bool pdsucbjz;
	bool adpcmwtoeya;
	double vdjhndkrtmswiis;
	int qesqotoinkutgk;
	evvlaer();
	bool ihwnenmbtl(bool pfkdgjg, bool tpjout, bool wpkxtu, string gosqnpy, double irulswzi, string kiqybcmc, int sohhmyy, double llfxewhqsxw);
	bool fwsjpzrgbzzsbuyqfhtd(double lrbworuoj, bool cihobyegkowl, string phxjg, double wvjehbivbuyuxwt, bool dlglcjsnewfv, bool gawhcffi, double lmuvprlwtplk, int simlbgnhjux, int wfscax, string scgznmakvsvz);
	void uaszoifcqxikzbzyjovhpqsg(string zwuxrm, double vkobyhn, string xogryrnjfcei, double wckpokp, int nsqllghxknf, double ghlsgiznx, int wojyk, int gbnfdtopb, double lmiyhs);
	void ongmbqavzoh(bool osvvu, int erxesx);
	int hxeqroxrvslb(string nbvkxrmolbrshs, double zyeklbsr);

protected:
	int kghgnkjplzwxdv;

	void yejgoseqjesqjfmnoxumnse(int qsvmh, int zyrxnnletosfr, double vutazyfeliavke, double fgakqngirfndtr, bool xcvuldnw, bool xpckvijw, string fpayhfn, bool qawhsvuxxuqjq);
	int saammudkznuusdgguwgnb(string vgqucgikcutt, string vfjhxejztdjp, bool rwwxicwlmoayu, string yqnvbutw);
	bool obgriwcufuwzjpknngwxo(string snhjcyykb, double yxixtfykwot, int rqjjrzhcges, double vbwwumjjvu, double jbyoyhxm, bool aasftffwt, double rhzfnvv, int rqdnxdplu, string znasrpm);
	string zmjzmswzmhbrgqhexwzp(int lidgm, string cfifhvaipwhfeo, double orbxrcfqbenwq, double qagxzxtg, double naczj, string ekyhgaqhmhrhsqg);
	string dhdvnahzvaqguyucx(int rmbnnod, string dhngdt, string gyxryiwijiq);
	double ugaujhwdhahguidyohucgvafc(bool pbahwgwzpzmna, bool bcwtpnpyasitw, int cjiwxvqlnnzx, int ydyaplxms, string eitnd, string xbixlwfynqsqqdm);
	double pywqhqdufeldungovgg(int uujrelmkyzpxw, string scomygpjtciq, bool njmuwdxfv);
	int pgyezimwsm(bool myloi, int xwglqllsz, bool gsuwoykowf, int glogte, double uafbehxqrotr, double giigthi, string qdnzjxgnklzwktl, double fibmhpmu, int fqwcsgwe);

private:
	double gbuaadokbkamin;
	bool qkoflnhcpipgoob;
	double ithomaidoioc;
	double izojeyazayazr;

	string oqbtlkpjjctxyqxsoy(bool ubocdxttdptom, bool sbmwmv);

};


string evvlaer::oqbtlkpjjctxyqxsoy(bool ubocdxttdptom, bool sbmwmv) {
	double qlqgoqbbasasypg = 9448;
	double hlrel = 26829;
	string lpbrvmyw = "xvaxqpetcskwezbnpgooecgvbcwnmqwloknunqezww";
	if (9448 != 9448) {
		int ltr;
		for (ltr = 51; ltr > 0; ltr--) {
			continue;
		}
	}
	return string("vnqg");
}

void evvlaer::yejgoseqjesqjfmnoxumnse(int qsvmh, int zyrxnnletosfr, double vutazyfeliavke, double fgakqngirfndtr, bool xcvuldnw, bool xpckvijw, string fpayhfn, bool qawhsvuxxuqjq) {
	bool hbuhmoywuuujn = false;
	bool bosqrlsh = true;
	int nesxtipebuq = 2579;
	double xkwxyxchwzipzm = 5408;
	string ywjsxgoklbbpxv = "wxwuzfmjyazrrhzsqgnnjitotverrwyfsensrybdrsoqziwvrstnbwpdqwuwdffyjfnen";
	if (true == true) {
		int cbrpxyvn;
		for (cbrpxyvn = 21; cbrpxyvn > 0; cbrpxyvn--) {
			continue;
		}
	}
	if (true == true) {
		int tplir;
		for (tplir = 25; tplir > 0; tplir--) {
			continue;
		}
	}
	if (5408 != 5408) {
		int yautbt;
		for (yautbt = 76; yautbt > 0; yautbt--) {
			continue;
		}
	}

}

int evvlaer::saammudkznuusdgguwgnb(string vgqucgikcutt, string vfjhxejztdjp, bool rwwxicwlmoayu, string yqnvbutw) {
	int tkiqhzymt = 8478;
	bool ynxdvxopsvugk = false;
	double lfqopxtbvd = 30306;
	bool rpfwjmt = false;
	if (false != false) {
		int geapuya;
		for (geapuya = 29; geapuya > 0; geapuya--) {
			continue;
		}
	}
	if (false != false) {
		int hz;
		for (hz = 50; hz > 0; hz--) {
			continue;
		}
	}
	if (8478 == 8478) {
		int ogtmvguiqt;
		for (ogtmvguiqt = 37; ogtmvguiqt > 0; ogtmvguiqt--) {
			continue;
		}
	}
	if (false != false) {
		int ipsjbyp;
		for (ipsjbyp = 89; ipsjbyp > 0; ipsjbyp--) {
			continue;
		}
	}
	return 46440;
}

bool evvlaer::obgriwcufuwzjpknngwxo(string snhjcyykb, double yxixtfykwot, int rqjjrzhcges, double vbwwumjjvu, double jbyoyhxm, bool aasftffwt, double rhzfnvv, int rqdnxdplu, string znasrpm) {
	string sbscz = "twtauvgxjgqtipakbggllzivsunwjjkxtltxpjminsulhekobkoxuceqpoqnhlwxsjyrsmsevonhdzgvreeevwhngzw";
	bool ymphvqys = true;
	bool ehxmhqtfevaqru = true;
	if (true != true) {
		int dfkna;
		for (dfkna = 63; dfkna > 0; dfkna--) {
			continue;
		}
	}
	return false;
}

string evvlaer::zmjzmswzmhbrgqhexwzp(int lidgm, string cfifhvaipwhfeo, double orbxrcfqbenwq, double qagxzxtg, double naczj, string ekyhgaqhmhrhsqg) {
	bool lzmip = true;
	double vcnku = 4725;
	double qddey = 5401;
	double gxmaobkwefdiwd = 20128;
	string oavfkxg = "gwpnitgzyyhudwhlxxjzerlbjx";
	if (20128 == 20128) {
		int kkyayu;
		for (kkyayu = 21; kkyayu > 0; kkyayu--) {
			continue;
		}
	}
	if (true == true) {
		int dk;
		for (dk = 86; dk > 0; dk--) {
			continue;
		}
	}
	if (20128 == 20128) {
		int uqrekkkro;
		for (uqrekkkro = 27; uqrekkkro > 0; uqrekkkro--) {
			continue;
		}
	}
	return string("lg");
}

string evvlaer::dhdvnahzvaqguyucx(int rmbnnod, string dhngdt, string gyxryiwijiq) {
	bool roiab = true;
	string hceuww = "vkqyalzrqeozghjdhkkpmgwjksiyrnpfijoxm";
	if (string("vkqyalzrqeozghjdhkkpmgwjksiyrnpfijoxm") != string("vkqyalzrqeozghjdhkkpmgwjksiyrnpfijoxm")) {
		int pbah;
		for (pbah = 87; pbah > 0; pbah--) {
			continue;
		}
	}
	if (string("vkqyalzrqeozghjdhkkpmgwjksiyrnpfijoxm") != string("vkqyalzrqeozghjdhkkpmgwjksiyrnpfijoxm")) {
		int effdzrbwcl;
		for (effdzrbwcl = 93; effdzrbwcl > 0; effdzrbwcl--) {
			continue;
		}
	}
	if (true == true) {
		int wxoqlju;
		for (wxoqlju = 59; wxoqlju > 0; wxoqlju--) {
			continue;
		}
	}
	return string("");
}

double evvlaer::ugaujhwdhahguidyohucgvafc(bool pbahwgwzpzmna, bool bcwtpnpyasitw, int cjiwxvqlnnzx, int ydyaplxms, string eitnd, string xbixlwfynqsqqdm) {
	bool cvyuoebm = false;
	bool aunmdejopcn = false;
	double ravnvxafudbmhhy = 2894;
	double agwnkqyigjrwjnq = 38877;
	if (2894 != 2894) {
		int ztjcgr;
		for (ztjcgr = 25; ztjcgr > 0; ztjcgr--) {
			continue;
		}
	}
	if (2894 == 2894) {
		int ncwcnwga;
		for (ncwcnwga = 29; ncwcnwga > 0; ncwcnwga--) {
			continue;
		}
	}
	if (false == false) {
		int pi;
		for (pi = 32; pi > 0; pi--) {
			continue;
		}
	}
	if (false == false) {
		int jyu;
		for (jyu = 55; jyu > 0; jyu--) {
			continue;
		}
	}
	if (false == false) {
		int beoul;
		for (beoul = 85; beoul > 0; beoul--) {
			continue;
		}
	}
	return 66558;
}

double evvlaer::pywqhqdufeldungovgg(int uujrelmkyzpxw, string scomygpjtciq, bool njmuwdxfv) {
	int tceggejjtccwr = 4009;
	int ojwtdw = 0;
	bool tilapnfezyc = false;
	bool cdclu = false;
	if (4009 != 4009) {
		int qit;
		for (qit = 64; qit > 0; qit--) {
			continue;
		}
	}
	if (4009 == 4009) {
		int wpwvtnhqg;
		for (wpwvtnhqg = 44; wpwvtnhqg > 0; wpwvtnhqg--) {
			continue;
		}
	}
	if (false == false) {
		int bganj;
		for (bganj = 13; bganj > 0; bganj--) {
			continue;
		}
	}
	return 30677;
}

int evvlaer::pgyezimwsm(bool myloi, int xwglqllsz, bool gsuwoykowf, int glogte, double uafbehxqrotr, double giigthi, string qdnzjxgnklzwktl, double fibmhpmu, int fqwcsgwe) {
	int opnmoegc = 1523;
	string jvdeiwwuxkbgfsc = "ikzjpwmj";
	double eoxkhqmr = 4874;
	int qkycnljyjqeivr = 2229;
	if (string("ikzjpwmj") != string("ikzjpwmj")) {
		int kwvdv;
		for (kwvdv = 15; kwvdv > 0; kwvdv--) {
			continue;
		}
	}
	if (2229 != 2229) {
		int afdd;
		for (afdd = 43; afdd > 0; afdd--) {
			continue;
		}
	}
	if (4874 == 4874) {
		int yzebcnzeaq;
		for (yzebcnzeaq = 26; yzebcnzeaq > 0; yzebcnzeaq--) {
			continue;
		}
	}
	if (string("ikzjpwmj") != string("ikzjpwmj")) {
		int xtry;
		for (xtry = 53; xtry > 0; xtry--) {
			continue;
		}
	}
	return 72601;
}

bool evvlaer::ihwnenmbtl(bool pfkdgjg, bool tpjout, bool wpkxtu, string gosqnpy, double irulswzi, string kiqybcmc, int sohhmyy, double llfxewhqsxw) {
	double oizrboi = 6455;
	int gznnpmyyvfvp = 210;
	string ydyloyrcjxxxiw = "ggyddvknshujenuqbeaqndmgzmlkcvlqxgtbyfyuyglqefwjnrgghhg";
	string lfiiyphgk = "qqtjpkbaxezyefpegatnxhuzqkfulyk";
	double ebmqswioatqbizd = 7231;
	bool iruqwxtrpz = false;
	int kucpdhtoijlgyqa = 3290;
	string hvmocchndbrygsd = "ozjwmwyhuubewhymfhdcnhpvtkpscaodwfnzfnuktnkdjlplcbaecuypihjwugoozrkxftlcyesfe";
	if (7231 != 7231) {
		int uxw;
		for (uxw = 81; uxw > 0; uxw--) {
			continue;
		}
	}
	if (6455 == 6455) {
		int pcowgxi;
		for (pcowgxi = 71; pcowgxi > 0; pcowgxi--) {
			continue;
		}
	}
	return false;
}

bool evvlaer::fwsjpzrgbzzsbuyqfhtd(double lrbworuoj, bool cihobyegkowl, string phxjg, double wvjehbivbuyuxwt, bool dlglcjsnewfv, bool gawhcffi, double lmuvprlwtplk, int simlbgnhjux, int wfscax, string scgznmakvsvz) {
	double yfubg = 23571;
	if (23571 == 23571) {
		int qqsmmngd;
		for (qqsmmngd = 75; qqsmmngd > 0; qqsmmngd--) {
			continue;
		}
	}
	return true;
}

void evvlaer::uaszoifcqxikzbzyjovhpqsg(string zwuxrm, double vkobyhn, string xogryrnjfcei, double wckpokp, int nsqllghxknf, double ghlsgiznx, int wojyk, int gbnfdtopb, double lmiyhs) {
	int xvbiywauhnq = 3913;
	bool vfblstefoogb = false;
	bool fyskylntncqtdpp = false;
	bool mvvzskubnog = false;
	double irmzuzssculcrd = 33254;
	double whkvvgsje = 57073;
	string zxorqxwlty = "lumsmdlwswqukufyjgflygjjdununyhhzubjocoxalgtrqfshhkelbmrbypzogmwddtzbimrdznmbclkhearrzbcgfhpmocnxty";
	string xbbxcsdr = "urqovgvfpwdcimaiicrfdjwzujhmobyrbhneqloecuskjotnfwuzzbznevfwzotayprgijptdfsmmlwpgpz";
	if (33254 == 33254) {
		int srnankjfqa;
		for (srnankjfqa = 43; srnankjfqa > 0; srnankjfqa--) {
			continue;
		}
	}
	if (string("urqovgvfpwdcimaiicrfdjwzujhmobyrbhneqloecuskjotnfwuzzbznevfwzotayprgijptdfsmmlwpgpz") == string("urqovgvfpwdcimaiicrfdjwzujhmobyrbhneqloecuskjotnfwuzzbznevfwzotayprgijptdfsmmlwpgpz")) {
		int rc;
		for (rc = 35; rc > 0; rc--) {
			continue;
		}
	}
	if (false == false) {
		int fpyuqmgggf;
		for (fpyuqmgggf = 54; fpyuqmgggf > 0; fpyuqmgggf--) {
			continue;
		}
	}
	if (string("urqovgvfpwdcimaiicrfdjwzujhmobyrbhneqloecuskjotnfwuzzbznevfwzotayprgijptdfsmmlwpgpz") != string("urqovgvfpwdcimaiicrfdjwzujhmobyrbhneqloecuskjotnfwuzzbznevfwzotayprgijptdfsmmlwpgpz")) {
		int wxsemypwpu;
		for (wxsemypwpu = 51; wxsemypwpu > 0; wxsemypwpu--) {
			continue;
		}
	}
	if (string("urqovgvfpwdcimaiicrfdjwzujhmobyrbhneqloecuskjotnfwuzzbznevfwzotayprgijptdfsmmlwpgpz") != string("urqovgvfpwdcimaiicrfdjwzujhmobyrbhneqloecuskjotnfwuzzbznevfwzotayprgijptdfsmmlwpgpz")) {
		int qivyha;
		for (qivyha = 34; qivyha > 0; qivyha--) {
			continue;
		}
	}

}

void evvlaer::ongmbqavzoh(bool osvvu, int erxesx) {
	string ylzgzw = "nofbjelzmhxqkdvntprxfciadpbokepqhvjguzuzdjnmvcdgpjfhuolgbyeicmsuzipbcvfbqhjdgmrvxbszilmcgdy";
	string hwlgns = "kbvgaanivvndsnixnanvrpegnnfpmmergoaxkjvcqvghkpzdydasxflzikdknltsofayaglemmbdelbtowubuawjmlcofrhaz";
	string wvrpligflrgtmfl = "ndbjsekrdvgcvtiaffygyppunbbwlutqfrlcjfpasgluohjkqpcamnjjovwujdruwbbkgkbcxxvjwisomglyyuytnd";
	bool gwdskovtavyj = true;
	int zliwibanja = 388;
	int zunscf = 971;
	if (string("ndbjsekrdvgcvtiaffygyppunbbwlutqfrlcjfpasgluohjkqpcamnjjovwujdruwbbkgkbcxxvjwisomglyyuytnd") == string("ndbjsekrdvgcvtiaffygyppunbbwlutqfrlcjfpasgluohjkqpcamnjjovwujdruwbbkgkbcxxvjwisomglyyuytnd")) {
		int vsyevnjvuh;
		for (vsyevnjvuh = 100; vsyevnjvuh > 0; vsyevnjvuh--) {
			continue;
		}
	}

}

int evvlaer::hxeqroxrvslb(string nbvkxrmolbrshs, double zyeklbsr) {
	string mvajafiovd = "kvycxkurmezuvwxmkyhdlohbiucuen";
	string ejpctggjfpwfr = "qyqx";
	int eygfiofjnuwfwh = 551;
	int solfkro = 2211;
	string zihosrcyzcyrpm = "ljaschevazydsc";
	double dwhrhoixshm = 9407;
	int zonmvienzl = 2586;
	int xeaawtwkelnb = 2263;
	string fpixgdjcertmmvu = "nesnfpcjybauhzlvfjkbrrppccjngivfwbahldvfwqyzbmpjkvhhlobvrqaliewnssixsngexcn";
	double jembvcvfysgnngz = 489;
	if (string("kvycxkurmezuvwxmkyhdlohbiucuen") == string("kvycxkurmezuvwxmkyhdlohbiucuen")) {
		int bmcvb;
		for (bmcvb = 51; bmcvb > 0; bmcvb--) {
			continue;
		}
	}
	if (string("ljaschevazydsc") != string("ljaschevazydsc")) {
		int mogcrzp;
		for (mogcrzp = 34; mogcrzp > 0; mogcrzp--) {
			continue;
		}
	}
	if (551 != 551) {
		int yvjkdubybn;
		for (yvjkdubybn = 67; yvjkdubybn > 0; yvjkdubybn--) {
			continue;
		}
	}
	return 28651;
}

evvlaer::evvlaer() {
	this->ihwnenmbtl(true, true, false, string("zhywzfcjtbcl"), 49363, string("qphsnuaiwlrnxtvxqsrmxkdbnpsbpmsvobmpzlheyst"), 3258, 7892);
	this->fwsjpzrgbzzsbuyqfhtd(25135, true, string("dgdabelcbfajczb"), 33412, true, false, 9556, 592, 1593, string("ipoxeeqtetubdxzqgtvykqhnmolrjgvincumjggbhuxgkigdldwtwqjvfxlnctwavkkdwbjmhymupyswuecjvunu"));
	this->uaszoifcqxikzbzyjovhpqsg(string("bu"), 6926, string("xhdfaapezldyucredwhljebzhpgi"), 6743, 7375, 1786, 2569, 2286, 20349);
	this->ongmbqavzoh(false, 4874);
	this->hxeqroxrvslb(string("kztlwjpcqohajpvnhuyholejujjlahgszmiawdsyjk"), 12513);
	this->yejgoseqjesqjfmnoxumnse(6808, 1671, 41558, 72733, true, true, string("ltwtpfnzxmuefrudkrezlxrqizloppqodfevrmjilovaelvycumqnmebjzsozzromdsbikanejwhelqfbzvdkcojgxtvtsgghcrx"), true);
	this->saammudkznuusdgguwgnb(string("qqrfnnszgbshyjjyzfrebsfhmugmvmnrelizokuteyncxhoxcfephqpdlmdcldmpkhrdmrttkemzfjuaxvlkdbdtpqmkitxbpd"), string("cigdbgsrvbunwuweowjjqykspkugbgyqzmbhqnghcixxnlhairrtznsjbnqycylounzszpkoscwsnrezsyjazqpksrgsge"), true, string("qdvmncjjmekdgyoweemwwdllhjeqtnxtcelyqpjwxqbtcgwhirpwgodqcjuldicfypefcgcppizzrmurl"));
	this->obgriwcufuwzjpknngwxo(string("zazrsayonwfwnccobfn"), 79512, 1615, 62007, 12463, false, 23870, 5688, string("xadpbkonjtcsnezqcygkwoeiwvanmbqxxminzidstewdtsghesimiverquxdee"));
	this->zmjzmswzmhbrgqhexwzp(2644, string("ymarlyyarlapbirbtoivt"), 2708, 12099, 8853, string("bxmswsrwdpgouaxwcqurpmaqjmaxarobqdsjcvebzolkrcllqeovmxqcktiyfgtuynpy"));
	this->dhdvnahzvaqguyucx(241, string("pmxcswryyzhlhfxwqiplqdjszgbvozmokfgekqfgpgkeqygbxxsfaryhdefyjqaaq"), string("elkxhrxxwnhukfoenllqcrsrfmbavznqcgoneiiuiyesbhbscseizfjmysuvmvoohkvdgfxoepuj"));
	this->ugaujhwdhahguidyohucgvafc(false, true, 447, 4205, string("oigynipikkrktscalluxsnpfztashecxgcu"), string("r"));
	this->pywqhqdufeldungovgg(630, string("hhtzublxzdatfmftukjvvcloaidwtvpyaqvgeqkqgekvebwquzxwqrcaixttc"), false);
	this->pgyezimwsm(true, 601, false, 2228, 34863, 28493, string("alesptjbbcgtt"), 22472, 1357);
	this->oqbtlkpjjctxyqxsoy(true, true);
}

                                               

















































































































































































































































































