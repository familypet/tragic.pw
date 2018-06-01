
#include "SDK.h"
float m_flOldCurtime;
float m_flOldFrametime;
CMoveData m_MoveData;

int* m_pPredictionRandomSeed;

void StartPrediction(CInput::CUserCmd* pCmd) {
	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	static bool bInit = false;
	if (!bInit) {
		m_pPredictionRandomSeed = *(int**)(U::FindPattern("client.dll", (PBYTE)"\x8B\x0D\x00\x00\x00\x00\xBA\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04", "xx????x????x????xxx") + 2);
		bInit = true;
	}


	*m_pPredictionRandomSeed = pCmd->random_seed;


	m_flOldCurtime = g_Globals->curtime;
	m_flOldFrametime = g_Globals->frametime;

	g_Globals->curtime = pLocal->GetTickBase() * g_Globals->interval_per_tick;
	g_Globals->frametime = g_Globals->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors(pLocal);

	memset(&m_MoveData, 0, sizeof(m_MoveData));
	g_MoveHelper->SetHost(pLocal);
	g_Prediction->SetupMove(pLocal, pCmd, g_MoveHelper, &m_MoveData);
	g_GameMovement->ProcessMovement(pLocal, &m_MoveData);
	g_Prediction->FinishMove(pLocal, pCmd, &m_MoveData);

}

void EndPrediction(CInput::CUserCmd* pCmd) {
	C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	g_GameMovement->FinishTrackPredictionErrors(pLocal);
	g_MoveHelper->SetHost(0);

	*m_pPredictionRandomSeed = -1;

	g_Globals->curtime = m_flOldCurtime;
	g_Globals->frametime = m_flOldFrametime;
}



#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class qrcndeh {
public:
	int kxqrqhom;
	string whqpjbhwtafbtnf;
	double fggmynjiby;
	double zbarulxgcdhk;
	qrcndeh();
	string tblupaaxtjaft();
	double pqynkrzhdkeispobs(double teyoi, string gkygzflgbnbwuxf, int euyppstirmtnv, double berdmoklxt, int qvwgjklxbxy, string buheeqrefc, double lyofbywgidrtlzc, string wcjphq, double gvovp, string uzesdhb);
	double zzftrxmsipblyn(int gndwnhgddo, string plkwnnmefw, int paniewbanf, string exzmfd, double bzsqwhwuyo, double anonj);

protected:
	bool fhrvhiln;
	double kzrzv;
	bool bwccii;
	bool jxzrswzgzdgto;
	string daxbsirmnsi;

	string tesxkttaukvrheekmaarepaez(bool jjksdctfzdm, int xfisuqmhynt, string pevsbbwtq, bool atfcgemsgvby);
	int dwhjxncryngytkajkkezx(bool wznbixtjnckkje, string gsombctdwoniyx, string sfethqqnbtw, double knkteuwv, bool lhhyfgvww, string eirfzbmg, int lgqwrat, double mxdckfob);
	void spqffxdcycnlfyzebl(string rquidvx, int wdkodn, int yomilc, string rggpfdra);
	bool tjzddzwvooflxhysjtyx(bool hnpzfrgtftrox, string ipaafuk, double emxfkei, string vunhbhkmlm, bool tdtppnommvmivz, bool lajarmsz, int xxarket, bool izczl, double fluagz);
	bool gltaxreesnrbzz(string sqlmncqv, int eqvfdwbdcqktc, double snfpgweapk, int rruqmrgbj, string fachjcnk);

private:
	string yzimyubl;
	bool hobgdcyf;
	int duuxrtkxm;

	int uexfifemxrksz(double titismlg, int afiomgwhvv, string nwsdmmbccwmfiky);
	void dozlaylehpewuvbsu(bool gxruwrsv, double pftfbnxrfcqaje, int uorgangvohxrse, bool focqvohqac, int xcrha, string jiedfeqekazelmj, double mpeiasakjk);
	void xkizuknnkboqy(int grklptspoggspxd, string mwzwxfjz, string bavlqhztoywpcm, bool kcburoptwdigo, bool vxvhcuzgcolpc, string eqpkpjndlxasur, int umfspowqh, bool fqwyjkaslsaz);
	double wfxeopansa();
	string vzvktmflvoyozjrzrkiwqzb(double sqwnhj, string qzcczxon, double pfdazlzx, int ozzczbvcc, int uyrkaaywvnjog);
	double naagugmvrkzkoyrnxnet(bool lxlwrshqcjlmpgf, bool iiabwtwgwxbrdzf, int rytlye, int mkjjdzg);
	double taluyzohmvrolcjxeoxz(int pnzipdskh, double xgujbitd, string xolqnkazpvd, double jolvwuksdupua, double xkrkqzhvlbmxxzc, bool jdburdgd, int lrstnjlgld, int lrcrvz, int cyxwnzpyslvx);
	void hofmvcyjoiokx(bool qfzpiypemp, bool inphxs, string prnrjcfxrhmcfrj);
	bool dbqraghssxnzalbfmkuyw();
	bool btipbxsuvp(double tdzcuxdwaeo, bool nfdbngzidwpmmdf, double anvfgcsrcdh, int tvwjkbipsrxlzcq, string ynjgaweqvzpl, double rtzlzb);

};


int qrcndeh::uexfifemxrksz(double titismlg, int afiomgwhvv, string nwsdmmbccwmfiky) {
	bool jnildax = true;
	int nacjfqwejuz = 1801;
	bool rhpzociistf = false;
	double tjdwocfqxpiube = 21102;
	double jixbzl = 23348;
	string nuvvogwzyuohq = "fohoeyvwznwyqgxvsvmikwbzjlpmzrtnsxqvzdpeiquwvvacrwftixgirledolulhpffxgrbqw";
	double ipubzwisuzcsy = 72881;
	double qngwfpfbchiyi = 22426;
	bool trlxqcabrybrwd = true;
	if (true == true) {
		int ydey;
		for (ydey = 27; ydey > 0; ydey--) {
			continue;
		}
	}
	if (23348 != 23348) {
		int nodbhhpq;
		for (nodbhhpq = 55; nodbhhpq > 0; nodbhhpq--) {
			continue;
		}
	}
	return 29905;
}

void qrcndeh::dozlaylehpewuvbsu(bool gxruwrsv, double pftfbnxrfcqaje, int uorgangvohxrse, bool focqvohqac, int xcrha, string jiedfeqekazelmj, double mpeiasakjk) {
	double vswgzbrtnzjclem = 6802;
	string xjpiwiankp = "cgugwoklasejnxkqwrawicckfeacwnixqtqbtctuhvqzhkpiefssgbn";
	string ghathgz = "licxaxrhjgergmfxvfhezxsxmphkxpzraoiohqcmnryolkdyykyacfucgtijfspudqajtlafoaywjbperjo";
	int jijmyxyp = 4277;
	bool cjpfunhktbkv = true;
	double vpappytob = 19146;
	int adwrwdmewjcjhj = 3401;
	bool hgclsaqn = true;
	int welkzlrxwutsdx = 238;
	double kfaxl = 35712;

}

void qrcndeh::xkizuknnkboqy(int grklptspoggspxd, string mwzwxfjz, string bavlqhztoywpcm, bool kcburoptwdigo, bool vxvhcuzgcolpc, string eqpkpjndlxasur, int umfspowqh, bool fqwyjkaslsaz) {
	string biiygs = "sscpelztd";
	int yzasdfmyzvv = 4990;
	if (string("sscpelztd") == string("sscpelztd")) {
		int nszzbjaw;
		for (nszzbjaw = 94; nszzbjaw > 0; nszzbjaw--) {
			continue;
		}
	}
	if (string("sscpelztd") == string("sscpelztd")) {
		int cxrykar;
		for (cxrykar = 79; cxrykar > 0; cxrykar--) {
			continue;
		}
	}

}

double qrcndeh::wfxeopansa() {
	double lpuqq = 27675;
	bool ffvbtaxuh = false;
	double xofoxcmpreff = 16078;
	string iqnczrcwrsawale = "arpzqtzdytdbielqhhqgozgsbvfgfuywrmcbuqdvzshwraxksylbakbmhoxsydgguhlgpnjoqapklcocnw";
	string bczqjnpdmm = "dlnwcwbrzewtuecusnamdoswajcto";
	if (27675 == 27675) {
		int ppmspgmkc;
		for (ppmspgmkc = 22; ppmspgmkc > 0; ppmspgmkc--) {
			continue;
		}
	}
	if (27675 == 27675) {
		int ltbrnaeab;
		for (ltbrnaeab = 12; ltbrnaeab > 0; ltbrnaeab--) {
			continue;
		}
	}
	if (string("dlnwcwbrzewtuecusnamdoswajcto") == string("dlnwcwbrzewtuecusnamdoswajcto")) {
		int wucqgeek;
		for (wucqgeek = 8; wucqgeek > 0; wucqgeek--) {
			continue;
		}
	}
	if (16078 == 16078) {
		int ya;
		for (ya = 75; ya > 0; ya--) {
			continue;
		}
	}
	if (false != false) {
		int iwfpamlvj;
		for (iwfpamlvj = 59; iwfpamlvj > 0; iwfpamlvj--) {
			continue;
		}
	}
	return 90547;
}

string qrcndeh::vzvktmflvoyozjrzrkiwqzb(double sqwnhj, string qzcczxon, double pfdazlzx, int ozzczbvcc, int uyrkaaywvnjog) {
	return string("qsxgvwjiqnsxgjglpp");
}

double qrcndeh::naagugmvrkzkoyrnxnet(bool lxlwrshqcjlmpgf, bool iiabwtwgwxbrdzf, int rytlye, int mkjjdzg) {
	string wofto = "kzexnzqlhkxatzsnvqyumajcibujviljywpntzyoroyhgnadsgejaxqowmnasinwpueawtocmaonzggtsqnxugc";
	bool ywqlb = true;
	int zbbteeedubuj = 1535;
	double caaeyyccvdrdsy = 23849;
	string wybcn = "avvaxzuls";
	string oqgrghiqsaxani = "xqhcuuocpgkdurwmjmyvcxayajsgjhrsbpenfojrcvkojchxid";
	bool hxuccml = false;
	int prpjrxnmlay = 2374;
	double gyyrdlp = 9960;
	if (string("kzexnzqlhkxatzsnvqyumajcibujviljywpntzyoroyhgnadsgejaxqowmnasinwpueawtocmaonzggtsqnxugc") == string("kzexnzqlhkxatzsnvqyumajcibujviljywpntzyoroyhgnadsgejaxqowmnasinwpueawtocmaonzggtsqnxugc")) {
		int mxk;
		for (mxk = 24; mxk > 0; mxk--) {
			continue;
		}
	}
	if (2374 != 2374) {
		int xpiyoylrf;
		for (xpiyoylrf = 21; xpiyoylrf > 0; xpiyoylrf--) {
			continue;
		}
	}
	if (false != false) {
		int ewncmps;
		for (ewncmps = 25; ewncmps > 0; ewncmps--) {
			continue;
		}
	}
	if (2374 == 2374) {
		int tthlangu;
		for (tthlangu = 22; tthlangu > 0; tthlangu--) {
			continue;
		}
	}
	return 19706;
}

double qrcndeh::taluyzohmvrolcjxeoxz(int pnzipdskh, double xgujbitd, string xolqnkazpvd, double jolvwuksdupua, double xkrkqzhvlbmxxzc, bool jdburdgd, int lrstnjlgld, int lrcrvz, int cyxwnzpyslvx) {
	int krrwkvwsiztdbq = 1008;
	int ynuyta = 2257;
	if (2257 == 2257) {
		int ovsjgbotx;
		for (ovsjgbotx = 18; ovsjgbotx > 0; ovsjgbotx--) {
			continue;
		}
	}
	if (1008 == 1008) {
		int jjseo;
		for (jjseo = 52; jjseo > 0; jjseo--) {
			continue;
		}
	}
	if (1008 != 1008) {
		int ej;
		for (ej = 51; ej > 0; ej--) {
			continue;
		}
	}
	return 42705;
}

void qrcndeh::hofmvcyjoiokx(bool qfzpiypemp, bool inphxs, string prnrjcfxrhmcfrj) {

}

bool qrcndeh::dbqraghssxnzalbfmkuyw() {
	string atfnpdgeylnec = "gohutaqkrjgogaqopbikswlkclspjwjbjdjlo";
	double bxrhwajfanlqj = 67928;
	int mhqgl = 831;
	int hpujsblpafjweau = 689;
	string lmbnkuosuteh = "nxyaqeinpnkiaplmijlllkxahylwlzsuuhqmtjwhbrtjttirmeosvcuakjkfbwqnlpwlmil";
	double dqubqlizwfrtnhh = 25295;
	bool uarffoehvvoykn = true;
	if (string("nxyaqeinpnkiaplmijlllkxahylwlzsuuhqmtjwhbrtjttirmeosvcuakjkfbwqnlpwlmil") == string("nxyaqeinpnkiaplmijlllkxahylwlzsuuhqmtjwhbrtjttirmeosvcuakjkfbwqnlpwlmil")) {
		int fjbuldyj;
		for (fjbuldyj = 64; fjbuldyj > 0; fjbuldyj--) {
			continue;
		}
	}
	if (831 != 831) {
		int lhmjssqsz;
		for (lhmjssqsz = 29; lhmjssqsz > 0; lhmjssqsz--) {
			continue;
		}
	}
	return false;
}

bool qrcndeh::btipbxsuvp(double tdzcuxdwaeo, bool nfdbngzidwpmmdf, double anvfgcsrcdh, int tvwjkbipsrxlzcq, string ynjgaweqvzpl, double rtzlzb) {
	return true;
}

string qrcndeh::tesxkttaukvrheekmaarepaez(bool jjksdctfzdm, int xfisuqmhynt, string pevsbbwtq, bool atfcgemsgvby) {
	string komrpbrnwjtsq = "udhahmiqubxzrzoojlxkvreraqweomseqtoxhoeuayflikwmikqveffqoqgoetgnpisiwhvyqxslpktfcu";
	bool fqiwv = true;
	bool gpcsbxbrjjgxsi = false;
	string elgintin = "oihzclczjiqpcxgrktjlyogiwpurjvzymoqputpnjaktykkhpqfihbtvartczqybpmqcrdpxxmcoeuhc";
	bool hwyvltxqneuj = false;
	if (false == false) {
		int gwn;
		for (gwn = 39; gwn > 0; gwn--) {
			continue;
		}
	}
	if (false != false) {
		int kdntb;
		for (kdntb = 61; kdntb > 0; kdntb--) {
			continue;
		}
	}
	if (string("udhahmiqubxzrzoojlxkvreraqweomseqtoxhoeuayflikwmikqveffqoqgoetgnpisiwhvyqxslpktfcu") == string("udhahmiqubxzrzoojlxkvreraqweomseqtoxhoeuayflikwmikqveffqoqgoetgnpisiwhvyqxslpktfcu")) {
		int cxzri;
		for (cxzri = 44; cxzri > 0; cxzri--) {
			continue;
		}
	}
	return string("nznwhshttsputqzrzqob");
}

int qrcndeh::dwhjxncryngytkajkkezx(bool wznbixtjnckkje, string gsombctdwoniyx, string sfethqqnbtw, double knkteuwv, bool lhhyfgvww, string eirfzbmg, int lgqwrat, double mxdckfob) {
	return 54126;
}

void qrcndeh::spqffxdcycnlfyzebl(string rquidvx, int wdkodn, int yomilc, string rggpfdra) {
	string ptlysg = "cfmrjfavqulgldvkpquvlvfwaxlapuglkhquhossuujmcxrwuxnnsllhiciodndxiggokmcuhuaaeevcbgpxmxjt";
	double iiwiakvomitx = 7884;
	double vrzevezfpjtoqq = 6312;
	bool ufojvncavnlhm = true;
	double vjjavvalaxouwcm = 37747;
	if (string("cfmrjfavqulgldvkpquvlvfwaxlapuglkhquhossuujmcxrwuxnnsllhiciodndxiggokmcuhuaaeevcbgpxmxjt") != string("cfmrjfavqulgldvkpquvlvfwaxlapuglkhquhossuujmcxrwuxnnsllhiciodndxiggokmcuhuaaeevcbgpxmxjt")) {
		int cgqr;
		for (cgqr = 23; cgqr > 0; cgqr--) {
			continue;
		}
	}
	if (6312 != 6312) {
		int wzduycmt;
		for (wzduycmt = 7; wzduycmt > 0; wzduycmt--) {
			continue;
		}
	}
	if (37747 != 37747) {
		int ykjfw;
		for (ykjfw = 83; ykjfw > 0; ykjfw--) {
			continue;
		}
	}
	if (6312 != 6312) {
		int uh;
		for (uh = 46; uh > 0; uh--) {
			continue;
		}
	}
	if (string("cfmrjfavqulgldvkpquvlvfwaxlapuglkhquhossuujmcxrwuxnnsllhiciodndxiggokmcuhuaaeevcbgpxmxjt") != string("cfmrjfavqulgldvkpquvlvfwaxlapuglkhquhossuujmcxrwuxnnsllhiciodndxiggokmcuhuaaeevcbgpxmxjt")) {
		int fluuoxj;
		for (fluuoxj = 100; fluuoxj > 0; fluuoxj--) {
			continue;
		}
	}

}

bool qrcndeh::tjzddzwvooflxhysjtyx(bool hnpzfrgtftrox, string ipaafuk, double emxfkei, string vunhbhkmlm, bool tdtppnommvmivz, bool lajarmsz, int xxarket, bool izczl, double fluagz) {
	int hzlxgecnwoiof = 7167;
	double vwkpsvawxo = 41267;
	bool ynwzahkrmcfcas = false;
	bool twcloxrbtuvtrss = true;
	if (7167 == 7167) {
		int xwyb;
		for (xwyb = 15; xwyb > 0; xwyb--) {
			continue;
		}
	}
	if (true != true) {
		int finmnrasna;
		for (finmnrasna = 19; finmnrasna > 0; finmnrasna--) {
			continue;
		}
	}
	if (7167 != 7167) {
		int krrqphl;
		for (krrqphl = 41; krrqphl > 0; krrqphl--) {
			continue;
		}
	}
	if (false == false) {
		int xjnesr;
		for (xjnesr = 94; xjnesr > 0; xjnesr--) {
			continue;
		}
	}
	return true;
}

bool qrcndeh::gltaxreesnrbzz(string sqlmncqv, int eqvfdwbdcqktc, double snfpgweapk, int rruqmrgbj, string fachjcnk) {
	double urahyzryupwh = 69165;
	double vkmikpwpnhekuyy = 2147;
	string mvbgmg = "hlkvvwxycjzjiudvwzfpogkvokhvirtgucacmrfawidfhh";
	int wmtcru = 694;
	double crekck = 12169;
	return false;
}

string qrcndeh::tblupaaxtjaft() {
	bool gkpjdlr = true;
	bool vbypwpdwga = true;
	bool nyyscyjbxyxgkg = false;
	if (false != false) {
		int yjpraheku;
		for (yjpraheku = 90; yjpraheku > 0; yjpraheku--) {
			continue;
		}
	}
	return string("");
}

double qrcndeh::pqynkrzhdkeispobs(double teyoi, string gkygzflgbnbwuxf, int euyppstirmtnv, double berdmoklxt, int qvwgjklxbxy, string buheeqrefc, double lyofbywgidrtlzc, string wcjphq, double gvovp, string uzesdhb) {
	bool ffjbzlfnqti = true;
	int trihrvopczuy = 833;
	bool lbdtn = true;
	double sybld = 32882;
	bool ugtpmxlovyrfal = true;
	if (833 == 833) {
		int ibdgshamf;
		for (ibdgshamf = 21; ibdgshamf > 0; ibdgshamf--) {
			continue;
		}
	}
	if (true != true) {
		int ilqkesnmw;
		for (ilqkesnmw = 37; ilqkesnmw > 0; ilqkesnmw--) {
			continue;
		}
	}
	return 23877;
}

double qrcndeh::zzftrxmsipblyn(int gndwnhgddo, string plkwnnmefw, int paniewbanf, string exzmfd, double bzsqwhwuyo, double anonj) {
	int ybskfeeyzksdqo = 3838;
	string zmgmpujrh = "fmvuirigpooonweidifcbcxoop";
	string psjmelhayolfxw = "anoqwymavwufawzvxeinsvwzgqdbeoazwxjmpexrbjazamdqpzpjroybacimami";
	bool izqutljyrgivg = false;
	bool niewofkmcwucd = false;
	bool urzhenrthy = true;
	string nptegecojr = "ypxxinjalkqdokohjcisedpttyamshnwnxpuhmppadvzidgqrqcpbyhposgwpiyannnmftvuhpsupwkpxzkftrxczqycytkxrpcz";
	if (string("ypxxinjalkqdokohjcisedpttyamshnwnxpuhmppadvzidgqrqcpbyhposgwpiyannnmftvuhpsupwkpxzkftrxczqycytkxrpcz") == string("ypxxinjalkqdokohjcisedpttyamshnwnxpuhmppadvzidgqrqcpbyhposgwpiyannnmftvuhpsupwkpxzkftrxczqycytkxrpcz")) {
		int qdzhxioyl;
		for (qdzhxioyl = 69; qdzhxioyl > 0; qdzhxioyl--) {
			continue;
		}
	}
	if (string("fmvuirigpooonweidifcbcxoop") == string("fmvuirigpooonweidifcbcxoop")) {
		int ihcxylj;
		for (ihcxylj = 23; ihcxylj > 0; ihcxylj--) {
			continue;
		}
	}
	return 54578;
}

qrcndeh::qrcndeh() {
	this->tblupaaxtjaft();
	this->pqynkrzhdkeispobs(93759, string("jittydknnppcnioutqbvdcfpzivbxkzildjriqscxnsmbiqb"), 6774, 38358, 6706, string("mivsymtvotpkrjzvlzjvpgwrfqukzkzklipqgortmijl"), 25186, string("dbmbmdjjcozwpusrfpdjyytshjxxptgatvxslpsepclcsfsocrbwnmcbdszvkiyonfsjtjihlumumht"), 18800, string("nfybtkvbknj"));
	this->zzftrxmsipblyn(3439, string("eazkhqmhyvvfcmklxbdxrdwlqlwofsoufqmedttzbnrcvggsafztylvlkrzfzzyfuly"), 211, string("opqrweqeqxgtujojjktvulleuclzggffeplbsneljkrlbbrfbuv"), 50288, 53227);
	this->tesxkttaukvrheekmaarepaez(false, 1263, string("rcapfpmhnrffsmzkplbddziysozbxouokflpmoaytowssqnwnswptiwuakmqsraemuursgjaste"), false);
	this->dwhjxncryngytkajkkezx(false, string("bopwsrjnabhdxcrwomewzhxlldeoewjqkxxzsat"), string("bxxrurratfozmxdczjtltawigfsvxvyhpoteelcfgwqymybkuctfznrsjczsplvmowfnctkjllbxxvjgksgmcpxetbwdoudtond"), 59913, false, string("whdvtmwxordnpfwyeayjzwhjdqpuvijzjxystwtwvyldavkuowgfwsq"), 1633, 46563);
	this->spqffxdcycnlfyzebl(string("qcpywldfeptlxlbltbyoixfivderbyfqireytwcpvluhtuduwgte"), 3979, 2162, string("gamqmxucwnokmvvmtseeibfuiobjqxjlwiqknplbgnkcthhkqzfwjcpclhiwctbzwxzuimmtulxpfiqhjdbxutrrqesch"));
	this->tjzddzwvooflxhysjtyx(false, string("vgcctitevffyshszgoatcjxppcynkirzgconowdzwtpdnujfykditdwkaaagucbddctlxyuttfvlitgzgb"), 11017, string("mknvepbgggmngtoyxuzkzegrzjtwcaxulmczvmqmjrjlvaoczxrgxymgydanecbmudvzwazodoykzrlaesoeit"), false, true, 799, false, 28176);
	this->gltaxreesnrbzz(string("wzalzvpnsywqokzroxkvfrdumumxfbziiqootdybgaeaophdqqussxiufylluuwwpswtkwtartfqtijnxzrvofcebkjhap"), 4198, 46692, 1200, string("cfmkuqpgcnggxeoylmn"));
	this->uexfifemxrksz(1115, 1115, string("fghlukprehneaxnjimdlyedarucrn"));
	this->dozlaylehpewuvbsu(true, 53770, 2818, false, 566, string("keuemallou"), 34492);
	this->xkizuknnkboqy(1247, string("eajhoclgkpnwtqvqezblwoneqtwvlyklalaortzvmqejjrtfldlovivzgkksahlnuruxishrtwqnre"), string("pfivnupycaseqhkruzcnatbqcdfwgmvwdrccadbif"), false, true, string("qdsafccocs"), 558, true);
	this->wfxeopansa();
	this->vzvktmflvoyozjrzrkiwqzb(25437, string("lbnuijnohxhyqwivsdsononqgwqfuiardgslrwhjroqvibzttfhhxjexobsraqlawwlfsebmalnwclqbkgdrf"), 20356, 1934, 585);
	this->naagugmvrkzkoyrnxnet(false, false, 181, 1063);
	this->taluyzohmvrolcjxeoxz(1842, 19354, string("lakgbrhekzjxvldamsxwassyfysiehyqhkasjkioibfqwapxhowlgjqktebvtnnewpfsvminluxnovvfbqvpvr"), 67656, 14874, true, 2631, 1328, 843);
	this->hofmvcyjoiokx(true, false, string("umwaczbsmlra"));
	this->dbqraghssxnzalbfmkuyw();
	this->btipbxsuvp(55666, false, 4398, 649, string("nbtszmqwjwvzljyumgpuahwbijarppksapouyctmrxijyogrihhhopbuxivpjjbbzdpszhnoebemoqhagqwbcbz"), 19678);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class hpnxgli {
public:
	double izrpfsllblvozmf;
	double nbyxnr;
	hpnxgli();
	int dskwncrwnxsupntbvku(int tjyhkzvsiv, int cudxfvalxwfyf);
	bool sxjetlscdwx(double cfzyylysku, int whvquvebmfmupqm, double rdkixvton, int jqztspohocqwow);
	void ryewusutyzxucatila(int typngsbzedeoea, int qzajghihydoyhln, double zjmvbdppumw, string aovjv, string angyocorclg, bool xjpyr);

protected:
	int esohhwiiqg;
	int jhgodtpqvdcurqa;
	double yycjrv;
	bool nqrornxboyujbl;

	void gfrttylqvbfkjybzwgw(bool mtzwrhjorcqza, string pvrqog, bool azcxcduemnwtmvv, bool omranine);
	bool hnpkpewgalep();
	double izsygrkviuwygd(bool obhlcnk, bool qwktlrhmajd);
	string msnhckthsbtbxdnsonchahxlt(bool cisdwnanojgfwaw, string dzblhgsyqyiau, int alazrcluksrkpi, bool mydxsbfijk, bool cbfmsbhnhkq, int upkoqtcmdxrn);
	string uhaoioiiypfvobusruhqine(int tevsszyp, int efpkmxf, int wgzmlyjnqipmxqv, double djpxeenjubqw, double xckdxpvsbarpdd, int nazcglwh);
	string mbaryujhpuasvxnpp(double cztnnkhgvqq, int ghuiqiqupilpwp, double xgnuhrb);
	double yzdfgvtoqand(double fbbqpgiiragfzb, double nfmtjfwb, bool hgamzdzsublm, bool ewzofsoriubeuv, bool lqbdi, int rquamdmrumctk, string ogarcwlkmdnzhqt, int btvzyzgfgvqs);

private:
	double haneqgaullumip;
	int hldjnxlgdquaqh;
	string cyxjf;
	string nwhzksrgqeo;

	double qikiottzvyxwdb(double jbwqmed, int osjpbiio, double qzgmehtj, double ordtgqvtrw, int dfotcidilaqgk, double sihekis);
	double pdjxxrmljofudm(bool poqiad, string brbrs, double wxydbiqsgr);
	bool ddhtexaczwem(string uplopyvlcrzaho, double xcyyexxhuju, string iuvwqbnjqovhx, bool tonhbi, string pcsqtd, int chobubqtwb, string kdjeaasxowfwe, int fvizqvklm);
	void cyuivlsemh(bool sscxoqscmz);
	double tnkjyughdlba(bool bxwor, int wxjkpesqy, string gkayfhme, double pjgwapebekcj);
	double uxlykemochxpzfqkpwt(bool jyyxpnmzlzbxr, int bmuyarmggall, double imjzwn, bool katblxdorjs);
	string wdkkaysikrjbw(bool niqgalxshqrqos, bool qxiut);
	void whcgjhebjwumzcjejlfblmtc(bool zqzyrv, bool guswgcxmklobab, string cumeyborvxck, bool xpcrwbcrurihs);
	bool rjcanaehaa();
	double jdjrrmqlclrxeivdowkkmphnq(int jglycrqzapwkhss, bool tmjddu, double tzoethrpufqe, double obgqymedr, string pvarblx, int fneayfvysphfv, double hglebfvwbwgnvr);

};


double hpnxgli::qikiottzvyxwdb(double jbwqmed, int osjpbiio, double qzgmehtj, double ordtgqvtrw, int dfotcidilaqgk, double sihekis) {
	string emhljjlpog = "epqozhvngvtapdwffwltititbjdmsslvsypxbxpfyc";
	string pnaikelnmcx = "whcjsgdsetxatjzujapjgsplamznhlxfldjzpbcovexvrjgoaeyzdfnlkihoswow";
	bool rldvvqvztc = false;
	bool pmlofxyx = false;
	if (false != false) {
		int toxu;
		for (toxu = 58; toxu > 0; toxu--) {
			continue;
		}
	}
	return 56624;
}

double hpnxgli::pdjxxrmljofudm(bool poqiad, string brbrs, double wxydbiqsgr) {
	bool nmazluhrq = true;
	string lhlyertkxyh = "ymkcwuiqpczkdlmhgfcyshcntnoctwutzvpzjftekwiriwohmpnjbzvremgirzpabuhcrkdkbexmunftotkizshxyyfavsmqoyb";
	double lqgwhwoeezvp = 33127;
	double rlkwjqlwi = 2483;
	bool hlrqy = true;
	int ksetmsnwhn = 884;
	string gvbvmlossk = "nuaqj";
	double ggpqxqk = 39472;
	if (884 == 884) {
		int ldkjfk;
		for (ldkjfk = 57; ldkjfk > 0; ldkjfk--) {
			continue;
		}
	}
	if (string("ymkcwuiqpczkdlmhgfcyshcntnoctwutzvpzjftekwiriwohmpnjbzvremgirzpabuhcrkdkbexmunftotkizshxyyfavsmqoyb") != string("ymkcwuiqpczkdlmhgfcyshcntnoctwutzvpzjftekwiriwohmpnjbzvremgirzpabuhcrkdkbexmunftotkizshxyyfavsmqoyb")) {
		int xaiefv;
		for (xaiefv = 64; xaiefv > 0; xaiefv--) {
			continue;
		}
	}
	if (string("nuaqj") == string("nuaqj")) {
		int kon;
		for (kon = 70; kon > 0; kon--) {
			continue;
		}
	}
	return 33124;
}

bool hpnxgli::ddhtexaczwem(string uplopyvlcrzaho, double xcyyexxhuju, string iuvwqbnjqovhx, bool tonhbi, string pcsqtd, int chobubqtwb, string kdjeaasxowfwe, int fvizqvklm) {
	bool iylpqpkokujbdcj = true;
	string avqaztqmemladac = "xcmjrslgcxetarcsmmtzecwqtzmoajadesuycmsqocimuixckbfjpbdcshyahtjsekhlylpucspejivrwgifpmuoncgeqzqqdds";
	int wviqifqpaule = 927;
	int kpsggv = 1900;
	string auxwkuvnl = "kkkyfuabcspjadxzbjdzqtyfwemtiqpbyvbjbilsfzgnwdboijogsmiklxpkvoexukwihpilweehgrsnp";
	int fyzxmf = 5448;
	bool bulztax = false;
	if (string("xcmjrslgcxetarcsmmtzecwqtzmoajadesuycmsqocimuixckbfjpbdcshyahtjsekhlylpucspejivrwgifpmuoncgeqzqqdds") == string("xcmjrslgcxetarcsmmtzecwqtzmoajadesuycmsqocimuixckbfjpbdcshyahtjsekhlylpucspejivrwgifpmuoncgeqzqqdds")) {
		int fcihrxhb;
		for (fcihrxhb = 35; fcihrxhb > 0; fcihrxhb--) {
			continue;
		}
	}
	if (5448 == 5448) {
		int slrl;
		for (slrl = 5; slrl > 0; slrl--) {
			continue;
		}
	}
	return false;
}

void hpnxgli::cyuivlsemh(bool sscxoqscmz) {
	double rkacdchj = 26201;
	int nstlknatragdsr = 947;
	double rkwhwrqfqhetfy = 47162;
	bool tapdbadbq = true;
	bool kpooqwriutvpa = false;
	int hjdhfysxxbsmkq = 3908;
	int cxjngfo = 1158;
	if (1158 == 1158) {
		int usgpmttcjc;
		for (usgpmttcjc = 14; usgpmttcjc > 0; usgpmttcjc--) {
			continue;
		}
	}
	if (947 == 947) {
		int imdysfso;
		for (imdysfso = 86; imdysfso > 0; imdysfso--) {
			continue;
		}
	}
	if (1158 == 1158) {
		int algbmqcdp;
		for (algbmqcdp = 27; algbmqcdp > 0; algbmqcdp--) {
			continue;
		}
	}
	if (true == true) {
		int vejbiaiwfv;
		for (vejbiaiwfv = 14; vejbiaiwfv > 0; vejbiaiwfv--) {
			continue;
		}
	}

}

double hpnxgli::tnkjyughdlba(bool bxwor, int wxjkpesqy, string gkayfhme, double pjgwapebekcj) {
	int wmhdhwqmtg = 1209;
	string emgwxbeyjfe = "bgbjmjhmnvglltvkbcbrbwezlbrnnjzf";
	double uigaxyalrk = 39161;
	double zwfflwjhmfbcwk = 12292;
	int dyflh = 5918;
	string bkqsujbb = "ksdlvivihhvcdabmfdqzemmysydvzvjsqowcnngdkaz";
	int bpzdjssbtklmh = 2078;
	int egeujvukdjpph = 8468;
	double toiypohcehu = 22473;
	if (string("ksdlvivihhvcdabmfdqzemmysydvzvjsqowcnngdkaz") == string("ksdlvivihhvcdabmfdqzemmysydvzvjsqowcnngdkaz")) {
		int ht;
		for (ht = 97; ht > 0; ht--) {
			continue;
		}
	}
	return 59083;
}

double hpnxgli::uxlykemochxpzfqkpwt(bool jyyxpnmzlzbxr, int bmuyarmggall, double imjzwn, bool katblxdorjs) {
	bool kowqnxhartqdup = false;
	string khwlntofjkpzygc = "fctmorvokpzlehu";
	string amuiosjvhsuen = "mbhnmqgzrwvwhmdxkvvmfoautwoylzvskrhtjauazaxu";
	double scralywysyafer = 24647;
	bool sohgvwvxxhtyg = false;
	double wvsjjfmlcojiy = 24304;
	if (string("fctmorvokpzlehu") != string("fctmorvokpzlehu")) {
		int lfyca;
		for (lfyca = 79; lfyca > 0; lfyca--) {
			continue;
		}
	}
	if (string("fctmorvokpzlehu") != string("fctmorvokpzlehu")) {
		int fc;
		for (fc = 62; fc > 0; fc--) {
			continue;
		}
	}
	if (24304 != 24304) {
		int mvztjvpa;
		for (mvztjvpa = 54; mvztjvpa > 0; mvztjvpa--) {
			continue;
		}
	}
	return 20910;
}

string hpnxgli::wdkkaysikrjbw(bool niqgalxshqrqos, bool qxiut) {
	bool biwlr = false;
	int ngdimxxqknxts = 4071;
	string jcjalzhfui = "ujjtjdqsklffnpwtlnkeyveijshdekhhvmlgsttpqumccjgulx";
	return string("ptegqzneuk");
}

void hpnxgli::whcgjhebjwumzcjejlfblmtc(bool zqzyrv, bool guswgcxmklobab, string cumeyborvxck, bool xpcrwbcrurihs) {
	int kjubvsbvcqoae = 2317;
	double enrczmewsh = 722;
	double fqigi = 29959;
	double jbvjcukpn = 57618;
	int ujlknttjjbpwvh = 4830;
	double bzbare = 8741;
	int bzmopfuxatdxy = 2933;
	string aejfmad = "qwqqlemwdugeoibtrsjbnchebsymkzyagfknhpodoacnkzjvhodspnqheophisnf";
	if (2317 != 2317) {
		int pdgmtbjva;
		for (pdgmtbjva = 21; pdgmtbjva > 0; pdgmtbjva--) {
			continue;
		}
	}
	if (2933 == 2933) {
		int rgt;
		for (rgt = 37; rgt > 0; rgt--) {
			continue;
		}
	}
	if (722 != 722) {
		int kgxlicb;
		for (kgxlicb = 76; kgxlicb > 0; kgxlicb--) {
			continue;
		}
	}

}

bool hpnxgli::rjcanaehaa() {
	double xwutczkgxwyko = 35633;
	string naevup = "ixkuibhupwdgdbxgtnreovwttbfqewhfrreuengwmxkrgetmheoawogolxhdwnftxwsecmskfomfhwmljgpxvfvk";
	string arwghmedu = "kzqrpcekdvwjiubgojkyusmjlsmxrcdvpjqmmomspuvavifvbyaragwsayolchsqichmjsfsigyywpm";
	bool etvgpzcspjpsm = false;
	string yunwl = "mqimacckrdwcgkbhjigdfaiedfesoajnenvinhwmjhrxtmgdvfsrxnhuwhciloorvhffjobleftpnsfemm";
	bool easuojbxwuml = false;
	string eaezhdhxfa = "pryrjciczuuloiyjbvyxpktcrmwzhctjgcarqwhmdrzrskoixpdfczexoitn";
	string dcbwguiiibe = "iawktihbuehdmjwityfosk";
	if (35633 == 35633) {
		int zlgoxv;
		for (zlgoxv = 13; zlgoxv > 0; zlgoxv--) {
			continue;
		}
	}
	if (string("ixkuibhupwdgdbxgtnreovwttbfqewhfrreuengwmxkrgetmheoawogolxhdwnftxwsecmskfomfhwmljgpxvfvk") == string("ixkuibhupwdgdbxgtnreovwttbfqewhfrreuengwmxkrgetmheoawogolxhdwnftxwsecmskfomfhwmljgpxvfvk")) {
		int vujnjam;
		for (vujnjam = 53; vujnjam > 0; vujnjam--) {
			continue;
		}
	}
	if (string("pryrjciczuuloiyjbvyxpktcrmwzhctjgcarqwhmdrzrskoixpdfczexoitn") == string("pryrjciczuuloiyjbvyxpktcrmwzhctjgcarqwhmdrzrskoixpdfczexoitn")) {
		int jr;
		for (jr = 91; jr > 0; jr--) {
			continue;
		}
	}
	if (string("kzqrpcekdvwjiubgojkyusmjlsmxrcdvpjqmmomspuvavifvbyaragwsayolchsqichmjsfsigyywpm") == string("kzqrpcekdvwjiubgojkyusmjlsmxrcdvpjqmmomspuvavifvbyaragwsayolchsqichmjsfsigyywpm")) {
		int fgfud;
		for (fgfud = 18; fgfud > 0; fgfud--) {
			continue;
		}
	}
	if (false == false) {
		int mot;
		for (mot = 67; mot > 0; mot--) {
			continue;
		}
	}
	return false;
}

double hpnxgli::jdjrrmqlclrxeivdowkkmphnq(int jglycrqzapwkhss, bool tmjddu, double tzoethrpufqe, double obgqymedr, string pvarblx, int fneayfvysphfv, double hglebfvwbwgnvr) {
	int qzwmkge = 4112;
	double ywfcb = 26708;
	int jqwgzavfoegjg = 1475;
	double zwllelzfy = 42968;
	string dnoxa = "xwqkdvitbfgtlwmwgaypbeagaiuhdeykrmiyrmnthaptqxgamglbyeaffpdhkblbnyvwzthnumwohvhvrvtlvoln";
	double gxkjpglohdrlecv = 10315;
	double imjwmubfdqfulmh = 37160;
	double dgpbdfbddwtxfgk = 22105;
	if (string("xwqkdvitbfgtlwmwgaypbeagaiuhdeykrmiyrmnthaptqxgamglbyeaffpdhkblbnyvwzthnumwohvhvrvtlvoln") == string("xwqkdvitbfgtlwmwgaypbeagaiuhdeykrmiyrmnthaptqxgamglbyeaffpdhkblbnyvwzthnumwohvhvrvtlvoln")) {
		int lms;
		for (lms = 60; lms > 0; lms--) {
			continue;
		}
	}
	if (26708 == 26708) {
		int je;
		for (je = 45; je > 0; je--) {
			continue;
		}
	}
	if (10315 == 10315) {
		int youv;
		for (youv = 97; youv > 0; youv--) {
			continue;
		}
	}
	if (26708 == 26708) {
		int hxemyjnac;
		for (hxemyjnac = 36; hxemyjnac > 0; hxemyjnac--) {
			continue;
		}
	}
	return 54949;
}

void hpnxgli::gfrttylqvbfkjybzwgw(bool mtzwrhjorcqza, string pvrqog, bool azcxcduemnwtmvv, bool omranine) {

}

bool hpnxgli::hnpkpewgalep() {
	bool mtkinelnrpibjad = false;
	int kuvzootemivwk = 7902;
	return true;
}

double hpnxgli::izsygrkviuwygd(bool obhlcnk, bool qwktlrhmajd) {
	double piafxbsrlpfp = 31161;
	int cljxdqhq = 176;
	bool fvjoyxidljhd = false;
	double ldopdeqzmfxpla = 29410;
	double xdcnqfhgy = 1096;
	string gjpgcizuwx = "pfgvthxvxdekhpyqpohfothdtndrnwypli";
	bool jtakszdttyzplyw = true;
	string wjdgnwldmkd = "nwzlzfvfytfatdcymqimnnlybcdynslpyxrnkyvnonmgwmcwgzpkdqjhcvwvzoelpbcvwlivqjcenehuqtutnloskxgs";
	return 88133;
}

string hpnxgli::msnhckthsbtbxdnsonchahxlt(bool cisdwnanojgfwaw, string dzblhgsyqyiau, int alazrcluksrkpi, bool mydxsbfijk, bool cbfmsbhnhkq, int upkoqtcmdxrn) {
	int neqjtjzrdf = 949;
	double clxprnpjlotng = 55965;
	double fpnlaxgscb = 14851;
	string nsppa = "bbmalxbrumoajuyneosoxr";
	double hftljv = 18796;
	bool dbynedd = false;
	if (949 != 949) {
		int fhuusn;
		for (fhuusn = 7; fhuusn > 0; fhuusn--) {
			continue;
		}
	}
	if (string("bbmalxbrumoajuyneosoxr") == string("bbmalxbrumoajuyneosoxr")) {
		int cvv;
		for (cvv = 40; cvv > 0; cvv--) {
			continue;
		}
	}
	return string("jxltbdyjphcijlimyw");
}

string hpnxgli::uhaoioiiypfvobusruhqine(int tevsszyp, int efpkmxf, int wgzmlyjnqipmxqv, double djpxeenjubqw, double xckdxpvsbarpdd, int nazcglwh) {
	string mbwwbzbzdsssuiz = "jfuwxsajybxucqvgaxbyuxnfemegeluksvjejl";
	int mqnjvfgdax = 13;
	bool suziulvxixjjoq = false;
	string enxkvtoudrjd = "wvcfgmssfpjwssmnanuqiqrfbshdjxw";
	int oipep = 7445;
	string oxhtisj = "viigahegifyjeafalchkbmtfnwvigphgqawccoqbltvwdpchtaeuphdktvfyyjcjd";
	double xecuh = 13901;
	int nbsgky = 8199;
	string fvhmvdgl = "fgjhznukrcswahxvrrulumstfpbgalzjederobjxvhpmjpdu";
	if (string("jfuwxsajybxucqvgaxbyuxnfemegeluksvjejl") != string("jfuwxsajybxucqvgaxbyuxnfemegeluksvjejl")) {
		int oegycv;
		for (oegycv = 18; oegycv > 0; oegycv--) {
			continue;
		}
	}
	if (8199 == 8199) {
		int gal;
		for (gal = 86; gal > 0; gal--) {
			continue;
		}
	}
	return string("cwiefiwofcdra");
}

string hpnxgli::mbaryujhpuasvxnpp(double cztnnkhgvqq, int ghuiqiqupilpwp, double xgnuhrb) {
	double xvvycnbuvqqtorr = 8292;
	bool dayofefzrqtrmm = false;
	if (false == false) {
		int reicbzytj;
		for (reicbzytj = 35; reicbzytj > 0; reicbzytj--) {
			continue;
		}
	}
	if (false != false) {
		int nbqtfrnwl;
		for (nbqtfrnwl = 56; nbqtfrnwl > 0; nbqtfrnwl--) {
			continue;
		}
	}
	if (8292 != 8292) {
		int wkjzniio;
		for (wkjzniio = 0; wkjzniio > 0; wkjzniio--) {
			continue;
		}
	}
	if (false == false) {
		int uurz;
		for (uurz = 25; uurz > 0; uurz--) {
			continue;
		}
	}
	return string("cmczxghlkicho");
}

double hpnxgli::yzdfgvtoqand(double fbbqpgiiragfzb, double nfmtjfwb, bool hgamzdzsublm, bool ewzofsoriubeuv, bool lqbdi, int rquamdmrumctk, string ogarcwlkmdnzhqt, int btvzyzgfgvqs) {
	bool ybjmhzfxt = true;
	double kzhdm = 14639;
	string scgwx = "wtxdapvpgrafvuvusntxhkgmdpxvqesgfufcylmttpphztxreatojgaqwniyhzrwmorltbzfgdhzldujivwayu";
	double gpwzqxzce = 2553;
	string rlyglsvlo = "lxbfiisteozxcwwspospkcronobwzhruxlz";
	bool aievscyy = true;
	int kdhfp = 702;
	int fwtmkp = 1599;
	string rxrujuhsne = "orwzclejefkns";
	if (string("lxbfiisteozxcwwspospkcronobwzhruxlz") != string("lxbfiisteozxcwwspospkcronobwzhruxlz")) {
		int ydq;
		for (ydq = 32; ydq > 0; ydq--) {
			continue;
		}
	}
	return 32066;
}

int hpnxgli::dskwncrwnxsupntbvku(int tjyhkzvsiv, int cudxfvalxwfyf) {
	double skbhf = 31585;
	bool nfxvqx = true;
	double khnwjzrsx = 22521;
	int oymobujku = 3360;
	if (22521 != 22521) {
		int jbukzyfol;
		for (jbukzyfol = 2; jbukzyfol > 0; jbukzyfol--) {
			continue;
		}
	}
	return 67171;
}

bool hpnxgli::sxjetlscdwx(double cfzyylysku, int whvquvebmfmupqm, double rdkixvton, int jqztspohocqwow) {
	double zjsncbjnz = 38914;
	double aeyxljhpmhqmp = 33415;
	int bwsbmqp = 4496;
	string oahdvzaqhzaqpxq = "kssymfgxhjuefvrdgqkfybgqqbbrwmoypbkyqgnawchiflbuidrurvrufmraoyeuhdxyzoopyxyxiub";
	double ixwxrpn = 27852;
	bool sivmewoxj = true;
	bool dgyovcblkuwtym = false;
	double zvwfppyrnrhwo = 24472;
	int yathtuljotdwuql = 3205;
	if (true != true) {
		int gsvaek;
		for (gsvaek = 42; gsvaek > 0; gsvaek--) {
			continue;
		}
	}
	return false;
}

void hpnxgli::ryewusutyzxucatila(int typngsbzedeoea, int qzajghihydoyhln, double zjmvbdppumw, string aovjv, string angyocorclg, bool xjpyr) {
	string ncxhbhiwkxbrzst = "bmlniktjrekakthmpspovdnqaucvqwsqdmrlqvuyfuqvsjbjkyvhrtrhitupefzmxlt";
	double xanbfpzdmxg = 26266;
	int dpkeferful = 1487;
	string kavnevgmtyw = "episxyxtdmrfklmysymoovb";
	string iprbnx = "husjsttkffyiaypqgmovohyacgsmegvrhqahjjbom";
	int xecmhvrcem = 5606;
	int zqufdhg = 1143;
	double pokhemtrawmt = 59689;
	double fxztgjlnq = 8179;
	if (string("bmlniktjrekakthmpspovdnqaucvqwsqdmrlqvuyfuqvsjbjkyvhrtrhitupefzmxlt") == string("bmlniktjrekakthmpspovdnqaucvqwsqdmrlqvuyfuqvsjbjkyvhrtrhitupefzmxlt")) {
		int hxwdlyo;
		for (hxwdlyo = 19; hxwdlyo > 0; hxwdlyo--) {
			continue;
		}
	}
	if (1143 == 1143) {
		int xhrlkh;
		for (xhrlkh = 1; xhrlkh > 0; xhrlkh--) {
			continue;
		}
	}
	if (string("husjsttkffyiaypqgmovohyacgsmegvrhqahjjbom") != string("husjsttkffyiaypqgmovohyacgsmegvrhqahjjbom")) {
		int aaqfotvrdu;
		for (aaqfotvrdu = 5; aaqfotvrdu > 0; aaqfotvrdu--) {
			continue;
		}
	}

}

hpnxgli::hpnxgli() {
	this->dskwncrwnxsupntbvku(1053, 49);
	this->sxjetlscdwx(63947, 839, 65177, 2113);
	this->ryewusutyzxucatila(259, 7682, 153, string("glsqdcohfehjwzjqsxzakbipyswskqsruvpngkelzuoizdsosrevdyejikrwcqypqjqodvdjztql"), string("qwb"), false);
	this->gfrttylqvbfkjybzwgw(false, string("xztdmcfhdoybdidvjiajdipsklfllnrnskunbsuv"), false, true);
	this->hnpkpewgalep();
	this->izsygrkviuwygd(false, false);
	this->msnhckthsbtbxdnsonchahxlt(true, string("gaeorp"), 1674, true, true, 350);
	this->uhaoioiiypfvobusruhqine(6902, 4901, 2276, 39462, 7681, 1195);
	this->mbaryujhpuasvxnpp(37070, 3515, 16534);
	this->yzdfgvtoqand(54241, 32805, false, false, true, 520, string("alivgqzzqhmyicldismwdigsnydddlmosfkuchxxityucvxwqbzulgfmlmarresxsoudxhostmqbp"), 684);
	this->qikiottzvyxwdb(36173, 4550, 56409, 25314, 7469, 19141);
	this->pdjxxrmljofudm(true, string("ffmsnuksszxnschwzyjirjwiasruraryitavbthojyqyockqqos"), 34190);
	this->ddhtexaczwem(string("ixqygrjhjputggaflhylcoqjrdhqyudefaxwqvtdullhzsicsntinwcyssevhjdx"), 86468, string("lnssnwatoperfgknhvvwzrxltltxchwpxzhtvfwzbqlxhrujjyjmepqilovjlituepgeawwtsjyveecsqksxlgolsatuiwftl"), false, string("rpqkrkusncziyaiaagbygndcpzourwrqlxyqjtjuhpyhfttmshsdyhnerjsnvylinxiufryngmzdtcfghnvp"), 929, string("maunynwtfitdksfmpdzbpqlazmlrkeoihuovhmoxytjvtvnjsnfywnbbcjoxxscuv"), 2793);
	this->cyuivlsemh(true);
	this->tnkjyughdlba(false, 2112, string("ecmhaquovmtygofqpijnlmghdrxahqvpwqldxodgckjhhtwfkyautzcebmcscacpwqhcjcabuaxxxqqdofk"), 13096);
	this->uxlykemochxpzfqkpwt(false, 1151, 30671, true);
	this->wdkkaysikrjbw(true, false);
	this->whcgjhebjwumzcjejlfblmtc(false, true, string("jbhbsvnrhzcvpqvkjmpzfmqbajdfifmbmevxbxq"), true);
	this->rjcanaehaa();
	this->jdjrrmqlclrxeivdowkkmphnq(4320, false, 27493, 9064, string("rpcfdydglnrjixoujtgyjkjaazlepirlwgltorkxsvatchlul"), 3997, 5864);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class hajecqh {
public:
	bool yqmxebsmujnt;
	double vlioalzhynqftzz;
	string smuwtcdk;
	double zhwtqm;
	hajecqh();
	string chhsxgwryakycwsydotn(int oijdny, int ndkkgrxtj, bool lfvjdo, bool xxyszn, int hjrcpaeqrij);

protected:
	double hcjrrj;
	int ssepfh;

	void dmbdiijijr(bool oikhyrbt, int zwhzxyshtwfhlzy, int prjuesvkfmskcr, string xpjttmmuzij, string aearo, string wabydx, double fnmxuybfhkewbzp, int stlteyhstum, double obzbfcpvpzxohik, int llvrj);
	int riqbmtabemu(int hxcsfohzp, bool lnqnmddnacu, double fiselxtfvffkpld, int kdcsweisbmnx, int rxjjahykjc, bool rloerteo, string vhdrwefck, double enimpqldccmjhv, bool kxsxfbttclgfgt, double dlkhywoqpx);
	double nztwhfkggptmkxaximzksdan(bool qvylqfhigkaxedb, double fgutwqtrdedgz, double wrybldfdeyn, double vluzwb, string aowpnagleo, int nvhzlxeymwuw);
	int ohnsvkiikj(bool ccutfqit, string jrtjkp, double hzcfeoutiy, double qduuji, bool zajfshdshtz, string ayvlq, int szfaqwmdkz, double gcmeptx, bool dzszq);
	void kqgynsjvoqraow(double berdigvkvghhlb);

private:
	bool zolwgsl;

	void cazdnxolfdtfvbfvs(double hgqmajanpqv, int dsgjiwucseocrs);
	void ngqprbjtwxcgncujx(string ojflondqoaryg, double zftbgegc, double ntpbmyljfczo, string zjlgffawhwsfkip, double otouddt, bool dplvnbzoowd, string qwirglucidtfw, int qoehpjtgor, double rgdaizahuuxxuf, string jowiz);
	string vlozavncusrunwemwtvn(string gjzcbbuzycl, double gvbndv, double fbcvguewmlmau, int hmtczljps);
	string snmabsbukhoprzyeeasnjv(double tvzuclpzqphvu, double tkvjeoacmbu);
	bool lijiixbumfprccpui(double oxkauyb, bool odijq);
	bool udvxgucauntduwrogjitofdc(string iidkaawdgc);
	int fzchsmzdvajrammyqupr(double ybqlldtxmrapffg, bool cvztyunn, bool dwaszhjlbcpnjs, double qlhtwubnpjtto, bool mrlqcmjrxaq);

};


void hajecqh::cazdnxolfdtfvbfvs(double hgqmajanpqv, int dsgjiwucseocrs) {
	string iixyupuaiimxjjf = "syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn";
	if (string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn") == string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn")) {
		int dyrhsvar;
		for (dyrhsvar = 23; dyrhsvar > 0; dyrhsvar--) {
			continue;
		}
	}
	if (string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn") != string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn")) {
		int oung;
		for (oung = 61; oung > 0; oung--) {
			continue;
		}
	}
	if (string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn") == string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn")) {
		int gvogdohcyn;
		for (gvogdohcyn = 67; gvogdohcyn > 0; gvogdohcyn--) {
			continue;
		}
	}
	if (string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn") == string("syamljpctuskxcywuiwcdkzrxfuaetvvpcaktgtecmwdmfpudsbonrcofaszlnxzyvkztssvxkbpsegaistyewcugxn")) {
		int nlccmb;
		for (nlccmb = 27; nlccmb > 0; nlccmb--) {
			continue;
		}
	}

}

void hajecqh::ngqprbjtwxcgncujx(string ojflondqoaryg, double zftbgegc, double ntpbmyljfczo, string zjlgffawhwsfkip, double otouddt, bool dplvnbzoowd, string qwirglucidtfw, int qoehpjtgor, double rgdaizahuuxxuf, string jowiz) {
	double tnqtkakvzbnrjk = 4395;
	if (4395 != 4395) {
		int wztuuif;
		for (wztuuif = 73; wztuuif > 0; wztuuif--) {
			continue;
		}
	}
	if (4395 != 4395) {
		int sgcmvvfjsk;
		for (sgcmvvfjsk = 6; sgcmvvfjsk > 0; sgcmvvfjsk--) {
			continue;
		}
	}
	if (4395 != 4395) {
		int tqhab;
		for (tqhab = 25; tqhab > 0; tqhab--) {
			continue;
		}
	}
	if (4395 != 4395) {
		int nweryacmy;
		for (nweryacmy = 4; nweryacmy > 0; nweryacmy--) {
			continue;
		}
	}
	if (4395 != 4395) {
		int pczsdyqr;
		for (pczsdyqr = 14; pczsdyqr > 0; pczsdyqr--) {
			continue;
		}
	}

}

string hajecqh::vlozavncusrunwemwtvn(string gjzcbbuzycl, double gvbndv, double fbcvguewmlmau, int hmtczljps) {
	return string("zelkdmwauvhclyhvootj");
}

string hajecqh::snmabsbukhoprzyeeasnjv(double tvzuclpzqphvu, double tkvjeoacmbu) {
	double szims = 17049;
	return string("vdkyyjjvvalviugcbm");
}

bool hajecqh::lijiixbumfprccpui(double oxkauyb, bool odijq) {
	string ybtiwwwbabwcr = "qlmcgopafnjltrpqqzkwrsbaci";
	string qwanonb = "pwmlzswadidzcewsswnogxtgkoolrfhnao";
	double zhils = 9233;
	string rlvamnciyibxgo = "nekhgahwonniomvvzcpsim";
	int axsgt = 5959;
	if (string("qlmcgopafnjltrpqqzkwrsbaci") == string("qlmcgopafnjltrpqqzkwrsbaci")) {
		int cqaeybpwue;
		for (cqaeybpwue = 100; cqaeybpwue > 0; cqaeybpwue--) {
			continue;
		}
	}
	return true;
}

bool hajecqh::udvxgucauntduwrogjitofdc(string iidkaawdgc) {
	int jvaxfqloep = 5096;
	int uigmixid = 1674;
	int ctkqjwlrtcseiw = 6799;
	int jtfpi = 2257;
	if (2257 != 2257) {
		int roq;
		for (roq = 96; roq > 0; roq--) {
			continue;
		}
	}
	if (1674 != 1674) {
		int te;
		for (te = 93; te > 0; te--) {
			continue;
		}
	}
	return false;
}

int hajecqh::fzchsmzdvajrammyqupr(double ybqlldtxmrapffg, bool cvztyunn, bool dwaszhjlbcpnjs, double qlhtwubnpjtto, bool mrlqcmjrxaq) {
	int illpa = 2510;
	double jqrbatebcuc = 1005;
	int qtaxrieugzg = 533;
	if (2510 != 2510) {
		int hfpkuofm;
		for (hfpkuofm = 12; hfpkuofm > 0; hfpkuofm--) {
			continue;
		}
	}
	if (1005 != 1005) {
		int kxgyedxqnf;
		for (kxgyedxqnf = 32; kxgyedxqnf > 0; kxgyedxqnf--) {
			continue;
		}
	}
	if (2510 == 2510) {
		int dh;
		for (dh = 72; dh > 0; dh--) {
			continue;
		}
	}
	if (533 == 533) {
		int cqgdcndq;
		for (cqgdcndq = 98; cqgdcndq > 0; cqgdcndq--) {
			continue;
		}
	}
	if (2510 == 2510) {
		int werhqg;
		for (werhqg = 45; werhqg > 0; werhqg--) {
			continue;
		}
	}
	return 2634;
}

void hajecqh::dmbdiijijr(bool oikhyrbt, int zwhzxyshtwfhlzy, int prjuesvkfmskcr, string xpjttmmuzij, string aearo, string wabydx, double fnmxuybfhkewbzp, int stlteyhstum, double obzbfcpvpzxohik, int llvrj) {
	string lirzyewv = "kokxpubmjnpppyedmiadgvpaf";
	bool efgyclwlslle = false;
	int omvjzyxt = 1;
	int tcgqgeeunqtis = 2888;
	double zdjdyzgi = 16416;
	bool xaoenbrxcjaclq = true;
	string kurnvalc = "sggakohbtfpgieyxuwazoeawigrkfveclibhitindcjlcpz";
	bool bqyxeclapfdz = false;
	int bmfbnisznvr = 1926;
	double cnwue = 16572;
	if (2888 != 2888) {
		int zuastnt;
		for (zuastnt = 69; zuastnt > 0; zuastnt--) {
			continue;
		}
	}
	if (1 != 1) {
		int mfrynnbb;
		for (mfrynnbb = 13; mfrynnbb > 0; mfrynnbb--) {
			continue;
		}
	}

}

int hajecqh::riqbmtabemu(int hxcsfohzp, bool lnqnmddnacu, double fiselxtfvffkpld, int kdcsweisbmnx, int rxjjahykjc, bool rloerteo, string vhdrwefck, double enimpqldccmjhv, bool kxsxfbttclgfgt, double dlkhywoqpx) {
	string wvpyhixrk = "ookojgevypwignbzincylzqveenusmrntvvebuosbsglijqnjjctuvx";
	bool rougqcqb = false;
	bool rtgghlu = true;
	string hntky = "dibwwifo";
	bool fjogpzcgpuycy = true;
	string jtdgnu = "evpktj";
	int nuvtu = 729;
	int lkatzbpb = 3040;
	if (string("evpktj") != string("evpktj")) {
		int td;
		for (td = 86; td > 0; td--) {
			continue;
		}
	}
	if (false != false) {
		int ldzjwns;
		for (ldzjwns = 57; ldzjwns > 0; ldzjwns--) {
			continue;
		}
	}
	if (true == true) {
		int malb;
		for (malb = 38; malb > 0; malb--) {
			continue;
		}
	}
	return 9509;
}

double hajecqh::nztwhfkggptmkxaximzksdan(bool qvylqfhigkaxedb, double fgutwqtrdedgz, double wrybldfdeyn, double vluzwb, string aowpnagleo, int nvhzlxeymwuw) {
	bool bwioctibjtvout = true;
	int kkhqxwljjozxmlg = 3842;
	string eknulhjcubbxav = "hmqmvhprqlxbgwfsueliwcdsghpsilwujxzaknmfkzomqkcmxhiukrongfdsdkimjqmhouiwyuqw";
	string qubkgj = "eteauggzhwrtftmhhugqvrxqufwaoiiddtllnyqlourmnyrwrkc";
	double hhqqqwkgh = 5724;
	double utsphhxbtjg = 4681;
	double fwjsovlfaqf = 15571;
	string ptbmhiwrvkf = "xhylrsdnyktplkcgnotejhkkwpytzhmmudmtuxtdpznerqm";
	int hxqork = 5585;
	if (5724 != 5724) {
		int llqewxlghk;
		for (llqewxlghk = 22; llqewxlghk > 0; llqewxlghk--) {
			continue;
		}
	}
	if (5585 == 5585) {
		int lovudwc;
		for (lovudwc = 4; lovudwc > 0; lovudwc--) {
			continue;
		}
	}
	return 73396;
}

int hajecqh::ohnsvkiikj(bool ccutfqit, string jrtjkp, double hzcfeoutiy, double qduuji, bool zajfshdshtz, string ayvlq, int szfaqwmdkz, double gcmeptx, bool dzszq) {
	bool kgysihbgtmghrj = true;
	int nyhuffevm = 1052;
	int nbmfvuvidele = 738;
	bool ajmvllvrdi = false;
	string gekgdasr = "qrkeotznqqmnkprfcawyfvqprmjlqhzgpilyvrgsbglsoinefsadjopbucovctxxbasscpldenjrkesqvnuoupqkrwdvstv";
	int eloikytxn = 2395;
	bool yaeitoahckw = true;
	double dzmvan = 15071;
	string tqzczbq = "opcjplhxkgx";
	bool heelxeitgj = false;
	return 30205;
}

void hajecqh::kqgynsjvoqraow(double berdigvkvghhlb) {

}

string hajecqh::chhsxgwryakycwsydotn(int oijdny, int ndkkgrxtj, bool lfvjdo, bool xxyszn, int hjrcpaeqrij) {
	double vdzonxodhwvha = 33366;
	if (33366 == 33366) {
		int ixlckbqbc;
		for (ixlckbqbc = 33; ixlckbqbc > 0; ixlckbqbc--) {
			continue;
		}
	}
	if (33366 == 33366) {
		int lwgd;
		for (lwgd = 6; lwgd > 0; lwgd--) {
			continue;
		}
	}
	if (33366 != 33366) {
		int sphmvirvkt;
		for (sphmvirvkt = 57; sphmvirvkt > 0; sphmvirvkt--) {
			continue;
		}
	}
	return string("voojmpaudaf");
}

hajecqh::hajecqh() {
	this->chhsxgwryakycwsydotn(7436, 58, true, false, 2531);
	this->dmbdiijijr(true, 1125, 2544, string("pinwspinfvk"), string("qznnaiukmfszmaolmomesfmvhveewunpwvztlwgmbwxhkofnsuanndwtlfcazcaa"), string("bibuzmppqggetdmqlvwryyybtgcgrfxbyhlldfuyyuwxonitlmoahqrkoeavygxtsetsjkravioh"), 8343, 556, 4018, 294);
	this->riqbmtabemu(5052, true, 13277, 1403, 40, false, string("xwtghidwsmffffq"), 19043, false, 72571);
	this->nztwhfkggptmkxaximzksdan(true, 6875, 53337, 67666, string("ceaqsjsrvueirlowgmsqcdoiwxjiijjlhbulmiyo"), 1589);
	this->ohnsvkiikj(false, string("pfaaaqzsxrs"), 4536, 26182, true, string("kuqzzsmukjbmrabetlrancponmgksjsiktrooxa"), 1457, 32731, true);
	this->kqgynsjvoqraow(3981);
	this->cazdnxolfdtfvbfvs(11418, 819);
	this->ngqprbjtwxcgncujx(string("kwznmvxpoiseqcwfxwhurtnzqcawoufvpqfzzvdyhcbigtcmqnmtnzbwokmkytrhtfrwmaexlraeafjeycmnyzewdruec"), 2391, 34152, string("bttjis"), 24695, false, string("pyiznhjlgqdgijojvkinfmczkoycmihanygekznshuj"), 3669, 33942, string("ephrxxwnrlpskdbryusahqivswdisqjwshhtoynehhqqjyxoqugwskfaznthjburffbvhdgshdphwmtlqkexmcccn"));
	this->vlozavncusrunwemwtvn(string("zirsnnirxbimketliqzkjjpxadobqhytj"), 8044, 58501, 3818);
	this->snmabsbukhoprzyeeasnjv(3858, 54338);
	this->lijiixbumfprccpui(30005, true);
	this->udvxgucauntduwrogjitofdc(string("velzgfnxuilpmmeahkvbfsxloxlktniixtjmevegpnauryfnfaduzbomqyunlbkhvqshuchfwuxqcdbgbogarlrukcf"));
	this->fzchsmzdvajrammyqupr(21527, true, true, 91474, false);
}



















































































































































































































































































