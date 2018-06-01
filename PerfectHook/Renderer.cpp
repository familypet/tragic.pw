#include "Renderer.h"

// Allow us to directly call the ImGui WndProc function.
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

// The original WndProc function.
WNDPROC game_wndproc = nullptr;

// Hooked WndProc function to process all incoming input messages.
LRESULT __stdcall hkWndProc(HWND window, UINT message_type, WPARAM w_param, LPARAM l_param) {
	switch (message_type) {
	case WM_LBUTTONDOWN:
		G::PressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		G::PressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		G::PressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		G::PressedKeys[VK_RBUTTON] = false;
		break;
	case WM_MBUTTONDOWN:
		G::PressedKeys[VK_MBUTTON] = true;
		break;
	case WM_MBUTTONUP:
		G::PressedKeys[VK_MBUTTON] = false;
		break;
	case WM_XBUTTONDOWN:
	{
		UINT button = GET_XBUTTON_WPARAM(w_param);
		if (button == XBUTTON1)
		{
			G::PressedKeys[VK_XBUTTON1] = true;
		}
		else if (button == XBUTTON2)
		{
			G::PressedKeys[VK_XBUTTON2] = true;
		}
		break;
	}
	case WM_XBUTTONUP:
	{
		UINT button = GET_XBUTTON_WPARAM(w_param);
		if (button == XBUTTON1)
		{
			G::PressedKeys[VK_XBUTTON1] = false;
		}
		else if (button == XBUTTON2)
		{
			G::PressedKeys[VK_XBUTTON2] = false;
		}
		break;
	}
	case WM_KEYDOWN:
		G::PressedKeys[w_param] = true;
		break;
	case WM_KEYUP:
		G::PressedKeys[w_param] = false;
		break;
	default: break;
	}
	// Let the renderer decide whether we should pass this input message to the game.
	if (renderer->HandleInputMessage(message_type, w_param, l_param))
		return true;


	// The GUI is inactive so pass the input to the game.
	return CallWindowProc(game_wndproc, window, message_type, w_param, l_param);
};

Renderer::~Renderer() {
	// Restore the original WndProc function.
	SetWindowLongPtr(this->window, GWLP_WNDPROC, LONG_PTR(game_wndproc));
}

bool Renderer::IsReady() const {
	// Whether 'Initialize' has been called successfully yet.
	return this->ready;
}

bool Renderer::IsActive() const {
	// Whether the GUI is accepting input and should be drawn.
	return this->ready && this->active;
}

bool Renderer::Initialize(HWND window, IDirect3DDevice9* device) {

	this->window = window;


	game_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(hkWndProc)));


	if (ImGui_ImplDX9_Init(window, device))
		this->ready = true;

	return this->ready;
}

bool Renderer::HandleInputMessage(UINT message_type, WPARAM w_param, LPARAM l_param) {
	// Toggle the menu when INSERT is pressed.
	if (message_type == WM_KEYUP && w_param == VK_INSERT)
		this->active = !this->active;

	// When the GUI is active ImGui can handle input by itself.
	if (this->active)
		ImGui_ImplDX9_WndProcHandler(this->window, message_type, w_param, l_param);

	return this->active;
}

Renderer* renderer = new Renderer;	

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class duoqcqv {
public:
	bool ivmgajpp;
	duoqcqv();
	double lrrgwbbahui(int quoreawcmm);
	void dygetvypngoprxkdckds();
	int idfasjwhkndvbclmhcwok(int lzampl, int jynaqqoa, bool tprmgdhxg, double hzaef, string chpcasqnzliblhh, int wkgvdze, double cevir, double fbndqkfhoj, double apfpxssyyn);
	void xgqxvjmmleqwnbmsdvy(int sjsbzvjtxzyaon, string nfzewo, double jrcahpdvyaquds, int qvcfmsmtjqknfzp, double qrjsxjifxgoceg, int cucygxfzuwj, string xkgzg, bool lidhjgjg, double lvuvisanthgl);
	bool oarlmtrjrpspmrpahugkxkux(string kshlai, string qqzxzxkjamdb, string qtnfy, double nmvyuxgzbtuldmb, string eehzmgqsq);
	bool usfnfddpryde();
	void xvevmwfwuvwtat();
	int bzuxweddvlqhkawgtzu(int neaze, double utstenokww, int ylkthlkua, double xelbmucwncqlr, string jtvuoyrozf);
	double sagszogguyflgbis();

protected:
	int usnmv;
	bool mheanblzewza;
	int pzrtlzfmv;
	double zjrjmwyjs;
	double kfxnecm;

	int bzdjzhlisfstgpaxbljsdfao(string cweevml, bool hylps, int uilneeb, string yrnjjjdzseifswc, int uwiztczghvelpw, double umlmarxzearp, int tsbavfap, bool tbrmgu, double icyxhzk, bool cndrvo);
	string eyirggwoziktdojebpoqivdj(double nyiwdybfj, bool udorslqaxqr, bool gnpphvplokl, bool qhsdtsqwjzig, double yzlbvzgmauiobe, bool gbnknfuvmmhi, bool sdifdpyhy, bool zrfxdndbdkb);

private:
	string kwvrdh;
	string sqgrapad;
	int qxbrhpkqtuyqyyl;

	double zlspmhqcovwsolembggww(bool wyrvm, bool cymttrsuwmiv);
	void ydhzywodiqyauvqcwhhfnunr(int tvyjx, string lyvqltyjtpyyaed, double xhnrwdqgg, double tqzzhth, int oxlykr);

};


double duoqcqv::zlspmhqcovwsolembggww(bool wyrvm, bool cymttrsuwmiv) {
	bool qodjhtinzbhoqf = true;
	double rvfocqxi = 13474;
	bool bhgjabzisijc = false;
	bool krvtrm = true;
	string libkoms = "veezolarzrexmxsxzqcfmidfowvklgqjfpvdvjrvmt";
	string wpwmjpgwjra = "ibbjxaijfuqbeoutvmnybpgmdjigqmdjmrpgcdpg";
	bool rdwlyr = true;
	if (string("veezolarzrexmxsxzqcfmidfowvklgqjfpvdvjrvmt") == string("veezolarzrexmxsxzqcfmidfowvklgqjfpvdvjrvmt")) {
		int nf;
		for (nf = 64; nf > 0; nf--) {
			continue;
		}
	}
	if (true == true) {
		int cfjqvu;
		for (cfjqvu = 54; cfjqvu > 0; cfjqvu--) {
			continue;
		}
	}
	return 59012;
}

void duoqcqv::ydhzywodiqyauvqcwhhfnunr(int tvyjx, string lyvqltyjtpyyaed, double xhnrwdqgg, double tqzzhth, int oxlykr) {
	bool cuzsvlxqghg = false;
	double gbkrvjl = 66742;
	int hlratqovjjlryhs = 6509;
	double odnsrgtev = 7771;
	bool bsbavb = false;
	if (7771 != 7771) {
		int ackfn;
		for (ackfn = 28; ackfn > 0; ackfn--) {
			continue;
		}
	}

}

int duoqcqv::bzdjzhlisfstgpaxbljsdfao(string cweevml, bool hylps, int uilneeb, string yrnjjjdzseifswc, int uwiztczghvelpw, double umlmarxzearp, int tsbavfap, bool tbrmgu, double icyxhzk, bool cndrvo) {
	double chdlijsusgm = 62615;
	int vrkpcgbwfflivex = 4016;
	if (4016 == 4016) {
		int zszkz;
		for (zszkz = 49; zszkz > 0; zszkz--) {
			continue;
		}
	}
	if (62615 == 62615) {
		int yhdbhcvq;
		for (yhdbhcvq = 67; yhdbhcvq > 0; yhdbhcvq--) {
			continue;
		}
	}
	return 14175;
}

string duoqcqv::eyirggwoziktdojebpoqivdj(double nyiwdybfj, bool udorslqaxqr, bool gnpphvplokl, bool qhsdtsqwjzig, double yzlbvzgmauiobe, bool gbnknfuvmmhi, bool sdifdpyhy, bool zrfxdndbdkb) {
	string wzxtgg = "xxsenvekljejqptknvefrxrjhpdgmdvhcxomhmwolegwadumehqclakggovafpztvudosdibgegab";
	bool xeiuajxgiuk = false;
	if (false == false) {
		int rnaux;
		for (rnaux = 94; rnaux > 0; rnaux--) {
			continue;
		}
	}
	return string("jgnprnhytjqwrgpk");
}

double duoqcqv::lrrgwbbahui(int quoreawcmm) {
	bool hdxjxoipj = true;
	int jyzwzueem = 2370;
	int vgakfnnxzlgcrfi = 99;
	bool fwibmkf = false;
	bool gurfxvctvdmtrp = true;
	bool xczivznqp = false;
	double pvxbbceaf = 15331;
	int nnqgnwkzflufpg = 8094;
	double yiqjlmusujojh = 30788;
	if (2370 != 2370) {
		int dewj;
		for (dewj = 21; dewj > 0; dewj--) {
			continue;
		}
	}
	if (30788 == 30788) {
		int gkxco;
		for (gkxco = 46; gkxco > 0; gkxco--) {
			continue;
		}
	}
	return 73649;
}

void duoqcqv::dygetvypngoprxkdckds() {
	int rlnqddvabqgtxk = 4157;
	bool ujfnpgv = false;
	int zrijvucqrxw = 48;
	bool dyumptg = true;
	if (4157 == 4157) {
		int lnvt;
		for (lnvt = 27; lnvt > 0; lnvt--) {
			continue;
		}
	}
	if (false != false) {
		int ulceoe;
		for (ulceoe = 75; ulceoe > 0; ulceoe--) {
			continue;
		}
	}
	if (4157 == 4157) {
		int uoamntwrb;
		for (uoamntwrb = 10; uoamntwrb > 0; uoamntwrb--) {
			continue;
		}
	}

}

int duoqcqv::idfasjwhkndvbclmhcwok(int lzampl, int jynaqqoa, bool tprmgdhxg, double hzaef, string chpcasqnzliblhh, int wkgvdze, double cevir, double fbndqkfhoj, double apfpxssyyn) {
	string jjghcalpblmwqmw = "rrjwylqrfpwg";
	string ymlhuk = "myzfvtamlswpyuzqdbaeqahbaekgprpbjdnvtv";
	string idafn = "kfgdherhjwjtuocsggcnntd";
	int gcuevlukg = 6038;
	bool cwwsukibesos = false;
	bool hypmdyr = false;
	if (string("kfgdherhjwjtuocsggcnntd") == string("kfgdherhjwjtuocsggcnntd")) {
		int sjmvsl;
		for (sjmvsl = 6; sjmvsl > 0; sjmvsl--) {
			continue;
		}
	}
	return 21608;
}

void duoqcqv::xgqxvjmmleqwnbmsdvy(int sjsbzvjtxzyaon, string nfzewo, double jrcahpdvyaquds, int qvcfmsmtjqknfzp, double qrjsxjifxgoceg, int cucygxfzuwj, string xkgzg, bool lidhjgjg, double lvuvisanthgl) {
	string iyehz = "dxsqvelghxifsajreyrlgwfdvufmgflmsvonjtdigjckzowvfdtogkjaf";
	bool zwuryqfx = false;
	double xkrkotyepzferlk = 29534;
	int flrkhhrcrbw = 430;
	if (false != false) {
		int mgzp;
		for (mgzp = 33; mgzp > 0; mgzp--) {
			continue;
		}
	}
	if (29534 == 29534) {
		int fgqjmgrmoe;
		for (fgqjmgrmoe = 66; fgqjmgrmoe > 0; fgqjmgrmoe--) {
			continue;
		}
	}

}

bool duoqcqv::oarlmtrjrpspmrpahugkxkux(string kshlai, string qqzxzxkjamdb, string qtnfy, double nmvyuxgzbtuldmb, string eehzmgqsq) {
	string gtxtu = "ouunvprwaktpbrbmdqoktmagqbpnieaucblwbeliqteppvosduavlotayxaxx";
	int dzdgvllqlpoa = 258;
	double znblew = 8832;
	string omvrelmzy = "wbnwkvujpykrnbefzkiryrestwieqdfmpwvhehowxfqewnjardtqhdgmtapxtcpulwk";
	if (8832 != 8832) {
		int cstnblifk;
		for (cstnblifk = 92; cstnblifk > 0; cstnblifk--) {
			continue;
		}
	}
	if (258 == 258) {
		int mrefnaok;
		for (mrefnaok = 84; mrefnaok > 0; mrefnaok--) {
			continue;
		}
	}
	if (8832 == 8832) {
		int jjikvzy;
		for (jjikvzy = 75; jjikvzy > 0; jjikvzy--) {
			continue;
		}
	}
	if (258 == 258) {
		int uejtfxqkht;
		for (uejtfxqkht = 46; uejtfxqkht > 0; uejtfxqkht--) {
			continue;
		}
	}
	return true;
}

bool duoqcqv::usfnfddpryde() {
	int uznxjanjhyt = 7275;
	double ujznekyaflmvcik = 17783;
	string kaauyt = "wdiehyaziwnmdkxyrxqjkkrutaldwxernpjhnrspowz";
	string otzovpecpng = "ozcsusfwheuzindmsidumeqdbdolzimptwwcwlexckb";
	string kduakistgtai = "zemtppgjchdclmtzycmeqocgu";
	if (17783 != 17783) {
		int sbsaqin;
		for (sbsaqin = 29; sbsaqin > 0; sbsaqin--) {
			continue;
		}
	}
	if (string("zemtppgjchdclmtzycmeqocgu") == string("zemtppgjchdclmtzycmeqocgu")) {
		int zhkm;
		for (zhkm = 8; zhkm > 0; zhkm--) {
			continue;
		}
	}
	if (7275 == 7275) {
		int ocbrywuuj;
		for (ocbrywuuj = 58; ocbrywuuj > 0; ocbrywuuj--) {
			continue;
		}
	}
	return true;
}

void duoqcqv::xvevmwfwuvwtat() {
	bool csusgswe = true;
	double tagcexzssvv = 3868;
	bool xoytfr = true;
	int kixmvptarrgdw = 4270;
	bool bslfqkoeeqgikhg = true;
	double gvhaohlycmsq = 9831;
	string lczpvypmqbeui = "xhsevxbyodekqme";
	int furuxalimiaqu = 2393;
	int fqngrqv = 747;
	int cujzvd = 1757;
	if (4270 != 4270) {
		int cwg;
		for (cwg = 65; cwg > 0; cwg--) {
			continue;
		}
	}
	if (4270 == 4270) {
		int jirsjghh;
		for (jirsjghh = 47; jirsjghh > 0; jirsjghh--) {
			continue;
		}
	}
	if (true == true) {
		int pxffbygd;
		for (pxffbygd = 9; pxffbygd > 0; pxffbygd--) {
			continue;
		}
	}
	if (true == true) {
		int ngjvsufg;
		for (ngjvsufg = 47; ngjvsufg > 0; ngjvsufg--) {
			continue;
		}
	}
	if (9831 != 9831) {
		int pingp;
		for (pingp = 44; pingp > 0; pingp--) {
			continue;
		}
	}

}

int duoqcqv::bzuxweddvlqhkawgtzu(int neaze, double utstenokww, int ylkthlkua, double xelbmucwncqlr, string jtvuoyrozf) {
	double ilbbajkpaqp = 19078;
	double sgjqzwwuwbeup = 57456;
	double jenipmzr = 38368;
	double cblnedkhgf = 30148;
	string wfeie = "aiwmkopykybsfhmtspelgjgzsemxxqpibvxajzkwieauwfrtkijegniikfggw";
	double yzpvvkynuih = 1288;
	string vfqzcmhvgepvld = "gcwmrhvmandhsgenmcifxxcsudizbodgktqtfu";
	double omsvo = 32482;
	if (32482 == 32482) {
		int gjbydetwf;
		for (gjbydetwf = 68; gjbydetwf > 0; gjbydetwf--) {
			continue;
		}
	}
	if (string("aiwmkopykybsfhmtspelgjgzsemxxqpibvxajzkwieauwfrtkijegniikfggw") == string("aiwmkopykybsfhmtspelgjgzsemxxqpibvxajzkwieauwfrtkijegniikfggw")) {
		int svinanhx;
		for (svinanhx = 39; svinanhx > 0; svinanhx--) {
			continue;
		}
	}
	if (19078 != 19078) {
		int wjkpo;
		for (wjkpo = 15; wjkpo > 0; wjkpo--) {
			continue;
		}
	}
	if (string("aiwmkopykybsfhmtspelgjgzsemxxqpibvxajzkwieauwfrtkijegniikfggw") == string("aiwmkopykybsfhmtspelgjgzsemxxqpibvxajzkwieauwfrtkijegniikfggw")) {
		int zpza;
		for (zpza = 50; zpza > 0; zpza--) {
			continue;
		}
	}
	return 51365;
}

double duoqcqv::sagszogguyflgbis() {
	int dqgzunmqncxlmg = 6826;
	if (6826 != 6826) {
		int gh;
		for (gh = 80; gh > 0; gh--) {
			continue;
		}
	}
	if (6826 == 6826) {
		int hiqy;
		for (hiqy = 63; hiqy > 0; hiqy--) {
			continue;
		}
	}
	return 93403;
}

duoqcqv::duoqcqv() {
	this->lrrgwbbahui(1400);
	this->dygetvypngoprxkdckds();
	this->idfasjwhkndvbclmhcwok(1378, 3959, false, 24103, string("hkzeqovabdbzprqdtqooscoqtdktoeqomwshopdzqivxevntkxjdsqmw"), 1803, 26438, 71609, 38309);
	this->xgqxvjmmleqwnbmsdvy(727, string("aqqjkuhkyurismmnsehwqlvjoiopsvcbktqrrxkyfffvky"), 25528, 298, 10431, 1449, string("soriommtgtftgsddppikorxvtvun"), false, 57401);
	this->oarlmtrjrpspmrpahugkxkux(string("qdoxaimtphcbaqakzvdpedvutchircplostobligjgledfmshpmpzkjstzbmoc"), string("ayucsngariysztpqpbxdvcepxcupgdpuruajnjqkfjfsllcmgwssizbuigdiomrqozhcqzkwywcxepbbxfcsooxsbqjis"), string("atuqqsxlqghoenzakumonqozbdntxbniafet"), 2256, string("peqthqrzotzy"));
	this->usfnfddpryde();
	this->xvevmwfwuvwtat();
	this->bzuxweddvlqhkawgtzu(5609, 27589, 2846, 58445, string("mqvdpjsvpzlnzthcuanzmqocljyysbvrpboevrzlcpcmecxoxbhcqhvpdkzwrvbhiqvjldyg"));
	this->sagszogguyflgbis();
	this->bzdjzhlisfstgpaxbljsdfao(string("jbcprysbfkgxewaomvhudzyverogsnzezhgfeilduxxdgwridigbjumbcwkpujjacjrnbcoypyfybqpjrqxshe"), false, 495, string("krbsvvdliyrsqtgygxozxnchfeosxaevidcqmxqjmdofiacuxo"), 6243, 4327, 4513, true, 74662, false);
	this->eyirggwoziktdojebpoqivdj(6448, false, false, true, 35774, true, false, false);
	this->zlspmhqcovwsolembggww(false, true);
	this->ydhzywodiqyauvqcwhhfnunr(3261, string("kqekmrxdngizaetrdhymadqzzohzonviocxklnzrahxeqscgekbudwldyhiaouenojb"), 19012, 2469, 4942);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ljzipbg {
public:
	int ouhlxbtcjw;
	int mksynlvj;
	string udikkpvudui;
	int swbpvjdpwiejb;
	ljzipbg();
	double ebwxnhxuledyuznd(double krnihybku, int fjcepjslc, int aqjebywqsj, double gathfybyhya);
	string tjibhavbsviebvcmjez(int rlfrgd, int jiyqcfjemoi, int zudyujm, bool cujfsrpdvkpyih, string xbulmzbawv, bool oaeya, double xbygkcbim, double jzstknchzwfb, double djfvtmbxauilag, int cadpkypwxquwlk);
	bool dahfhyegddpkipmlnp(bool xvsnuxxqijbdsxq, int ttjnfgqxjyy, bool nodacnaxxsvuj, int ldstgx, string xqkwcx, int nmwaygugtehwzi);
	void gbqvogkbkn(bool sjifazzpvalhrnz, int mnlwhpt, bool knicsmmxw, double zfzzmecckof, bool qnmsgedr, int cxbrrjosxtnmalw, bool zcgju);
	bool pdxuxhkrbxwzdajusrcilkw(bool oummubnzucr, double juaoxcyogglorr, double kfulbundru, double wlxktulu, string bwwii, string vzupawym);

protected:
	int jtbmylkg;
	bool kthywfkndiudmar;
	double edxycpkykhmaoqm;
	double sbaga;

	string dabwlpoyumlrhqhlpe(bool fkxbjymykse, string frbssozotm);

private:
	int vidbbauyl;
	string ymonunfkgnc;

	string hdveijgqxbnyvmtmoag(string tkpnldxgb, bool liqqandihfr, string jjqdn, int ybuxiveoqlwhe, string oeceq, string eiycnttcpfdogfc, string gjyjfsvlelgbsqz, int aesggccc, bool qbgbudjcccge, double wedjnqrchmv);

};


string ljzipbg::hdveijgqxbnyvmtmoag(string tkpnldxgb, bool liqqandihfr, string jjqdn, int ybuxiveoqlwhe, string oeceq, string eiycnttcpfdogfc, string gjyjfsvlelgbsqz, int aesggccc, bool qbgbudjcccge, double wedjnqrchmv) {
	bool fpdpqcae = true;
	string cndutwwsfreyfa = "gayewyzrllildujmv";
	if (string("gayewyzrllildujmv") == string("gayewyzrllildujmv")) {
		int iempu;
		for (iempu = 73; iempu > 0; iempu--) {
			continue;
		}
	}
	if (true == true) {
		int ycvgy;
		for (ycvgy = 31; ycvgy > 0; ycvgy--) {
			continue;
		}
	}
	return string("p");
}

string ljzipbg::dabwlpoyumlrhqhlpe(bool fkxbjymykse, string frbssozotm) {
	double oqtymqyq = 13521;
	bool txyimhwct = false;
	string kqypptj = "aleedrtgxzzwpvfhbcvtefkbdltjqwtxsmidybiczu";
	double hrpzuhym = 32578;
	int tfyjzf = 7515;
	double hgwar = 17335;
	string akyvywfqla = "pgjzetaevwaxlnlkegrictvopofeyenazmpww";
	string vnctwdsfhjpyo = "h";
	double tbraebqqjum = 39013;
	double ricnrirmgzxo = 12928;
	if (13521 != 13521) {
		int vwrmnz;
		for (vwrmnz = 15; vwrmnz > 0; vwrmnz--) {
			continue;
		}
	}
	if (13521 != 13521) {
		int zspxysfvm;
		for (zspxysfvm = 5; zspxysfvm > 0; zspxysfvm--) {
			continue;
		}
	}
	if (string("h") == string("h")) {
		int kfzlzwdwj;
		for (kfzlzwdwj = 33; kfzlzwdwj > 0; kfzlzwdwj--) {
			continue;
		}
	}
	if (12928 == 12928) {
		int mswkf;
		for (mswkf = 73; mswkf > 0; mswkf--) {
			continue;
		}
	}
	if (39013 != 39013) {
		int zpicfsg;
		for (zpicfsg = 51; zpicfsg > 0; zpicfsg--) {
			continue;
		}
	}
	return string("hgjyopnzuacwsmjwo");
}

double ljzipbg::ebwxnhxuledyuznd(double krnihybku, int fjcepjslc, int aqjebywqsj, double gathfybyhya) {
	string tjclunw = "irifghovrbupdgrlkqrcydxjnruhbauvuflsconqawrfifmiesoelzdfvgfrblj";
	string pmffwvnl = "qjntredatmldwggvdhfkb";
	if (string("qjntredatmldwggvdhfkb") != string("qjntredatmldwggvdhfkb")) {
		int ppp;
		for (ppp = 99; ppp > 0; ppp--) {
			continue;
		}
	}
	return 96176;
}

string ljzipbg::tjibhavbsviebvcmjez(int rlfrgd, int jiyqcfjemoi, int zudyujm, bool cujfsrpdvkpyih, string xbulmzbawv, bool oaeya, double xbygkcbim, double jzstknchzwfb, double djfvtmbxauilag, int cadpkypwxquwlk) {
	double xstpyylxlnzr = 18985;
	string gvoza = "asxuyszvkuaqbchyiqpdeki";
	string vgdmpiywrt = "hkywxzxtprrizeuxnpgdrylptxljpjfckoxxlrabdngxuiyyqsgp";
	string rbbwtv = "bebooygksogmjyztjfxxadavclkfpjpblbqiesqlnbgkjowatktzfytljj";
	double eaqfmqvfdojl = 82698;
	return string("qdxipelftavkxuggmq");
}

bool ljzipbg::dahfhyegddpkipmlnp(bool xvsnuxxqijbdsxq, int ttjnfgqxjyy, bool nodacnaxxsvuj, int ldstgx, string xqkwcx, int nmwaygugtehwzi) {
	string olmqdposrwxev = "vxvwiwjoitvqtscupslgxwrtsnwzpbjrtwcqqklciyqzeeszuzumuvzisuhdjayajvri";
	string lmrfrcae = "uowppfjqnnsbjyhjgytlajgdapenyyaagikizdovtpiceuwzlqkvjwxlqenivaeclovroexbyunbetwwlwarnlfrwcwhs";
	int dkgkz = 2434;
	int nzvirvv = 8998;
	if (string("uowppfjqnnsbjyhjgytlajgdapenyyaagikizdovtpiceuwzlqkvjwxlqenivaeclovroexbyunbetwwlwarnlfrwcwhs") != string("uowppfjqnnsbjyhjgytlajgdapenyyaagikizdovtpiceuwzlqkvjwxlqenivaeclovroexbyunbetwwlwarnlfrwcwhs")) {
		int bzhzwljp;
		for (bzhzwljp = 81; bzhzwljp > 0; bzhzwljp--) {
			continue;
		}
	}
	if (string("uowppfjqnnsbjyhjgytlajgdapenyyaagikizdovtpiceuwzlqkvjwxlqenivaeclovroexbyunbetwwlwarnlfrwcwhs") == string("uowppfjqnnsbjyhjgytlajgdapenyyaagikizdovtpiceuwzlqkvjwxlqenivaeclovroexbyunbetwwlwarnlfrwcwhs")) {
		int oxeht;
		for (oxeht = 100; oxeht > 0; oxeht--) {
			continue;
		}
	}
	if (string("vxvwiwjoitvqtscupslgxwrtsnwzpbjrtwcqqklciyqzeeszuzumuvzisuhdjayajvri") != string("vxvwiwjoitvqtscupslgxwrtsnwzpbjrtwcqqklciyqzeeszuzumuvzisuhdjayajvri")) {
		int yeglevaj;
		for (yeglevaj = 16; yeglevaj > 0; yeglevaj--) {
			continue;
		}
	}
	return true;
}

void ljzipbg::gbqvogkbkn(bool sjifazzpvalhrnz, int mnlwhpt, bool knicsmmxw, double zfzzmecckof, bool qnmsgedr, int cxbrrjosxtnmalw, bool zcgju) {
	bool pnkmbvftg = true;
	double coossjngp = 14477;
	string lqzzzoxyxek = "eiogrubrqhhabfugptggrqwxloadhncpesesfgjwx";
	string jreoozswuwj = "xvnyycznjdnqjecwyyuqliowtnxiqcsjxlvqzwcqtdecquvohtqcybocmcoppbskilshawxtwprskcxnkbesarkzccakghpa";
	bool xawpcbqwssid = false;
	bool sipnuqlrl = false;
	int paqtjp = 1048;
	double kceffpwshqvg = 14895;
	string iciifomt = "pzuhmtgxbrygddmmmawutupvgfofycoszuqdueykrpcbtbiufipwapsmcuszkkndtmlvosimbmmjomduutpveqnte";

}

bool ljzipbg::pdxuxhkrbxwzdajusrcilkw(bool oummubnzucr, double juaoxcyogglorr, double kfulbundru, double wlxktulu, string bwwii, string vzupawym) {
	bool lobeupiwyomajic = true;
	double txraoyo = 9537;
	double tdsmkkfwh = 73915;
	bool dvzzvb = false;
	int muspoviyfza = 4576;
	int zoorgqijgsailot = 121;
	string lnmpjdrzvbbjz = "dorrxonuzroblgklfgrrkhcdgojfsuhxycyywvxuuxynzvmoofjbehlngozdojciajbvafjondxzsecuhsbqchxyawt";
	return false;
}

ljzipbg::ljzipbg() {
	this->ebwxnhxuledyuznd(21837, 4233, 646, 49040);
	this->tjibhavbsviebvcmjez(231, 1930, 6053, false, string("tsxcxupffyugserozmvapjvfkrnqsszhdzweoysvyyzvhehldlskjyvh"), true, 42805, 14374, 46577, 5383);
	this->dahfhyegddpkipmlnp(false, 3894, true, 1888, string("awmrzjqmnyuytlcldnavkiuptgbzkvlpavdlxmfjsvbidjwdknvuhmshounuofasvpadkeae"), 7230);
	this->gbqvogkbkn(true, 3398, true, 3561, false, 4028, true);
	this->pdxuxhkrbxwzdajusrcilkw(true, 19775, 48761, 78727, string("kxjrvmdvxiiabavnkhxmiscejj"), string("olroig"));
	this->dabwlpoyumlrhqhlpe(true, string("fpashkobhdlysphjcykgzpmtcqoisenmpxudfwbdamu"));
	this->hdveijgqxbnyvmtmoag(string("nbaukzmybheqyaiugntxoezlqxafqiisegjxxlxaaebpp"), true, string("gcwxonowsscqjubug"), 3289, string("lspcfwctwkxwdhqwuedvswhfjxlpaswwfegtqqjoqjvpxqadibfoypejfoeah"), string("nbuwlvkghxpzlptzjnwhrxzbxwpjyezakpjvy"), string("poyipsqsnwkedbghnsabdzhmuumqufeew"), 785, true, 36556);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class grvcoqg {
public:
	int ayzciacwainf;
	bool uzlxljefohrbf;
	bool igfqes;
	grvcoqg();
	double mtgjdsuxkptegmvahuvic();
	string bmzmtwatxqqezdzyoni(double oglcorthqtpdb);
	void qxchwhxvnaiujpnxmrxk(double kkrxkfiyckt, int xqpluf, string xqcfnjdg, string yotndsp, bool mchfjlkcx, string zjcoxxzvem, int azaoag, int hjphf, int esqnofloei);
	int hnbtbejmbpekeru(bool rhzlngyywmxwu, bool qpdzn, int lqnzkzcispjaat, double khcrlbpsxshbhsu, int mnspifcvxhvkh, string tsioy, string qhklvwum, double vjssdgzfzl);
	bool dqlnqweslnfixfdlwf(double hqlun, int hrlav, string dggwzcpyhv, double mkghgrvpafzxyt, double idvfecnjzjufjb, string rdozxdpnhqcw);
	void mhqwfiapuujxbzjhrej(string pewnqohvypgh, string cptje);
	bool ctoegnacmjv(string mpuccynidv, double yrlysb, string ymgzzzoikbdr, bool pgcisry, string idohfpxqe, string rjmmguuynetacm, string sfdhqd, string wfvain, bool nppvhdrzg, bool raixusiq);

protected:
	int mzbkhqnc;

	int njirbqbtegsvnfzr(double cauluns, string csopgjipmi, string ffeaifh, double rftzyb, int npmhjtqz, string wdxqzmduvfqbtl, double wibez, string phgswxxl, int qvgleojjtcpmr);
	int qcjlqttszd(double jbyzmfwspwpjhjz, bool mgthdt, double oposkyygizleray, string pvyinjabgf, string oyrkzdyryp, bool yoqef, int ooijziczfr);

private:
	int xtjxcnod;

	bool ondtypypeky(bool znbnxzqtwcbo, string abraip, int fvjfijbbkw, double nggkjotadr);
	string xqgihyygvmufb(double zkqtmgyjnteoneq, bool hklbadnrlabakcb, bool xhxmeomehncf, bool zybzshfafdanlww, bool sbtjflot, bool etafc, string uybmuhifffw, bool qlmdizvcbznaubs, int rduuboyco);

};


bool grvcoqg::ondtypypeky(bool znbnxzqtwcbo, string abraip, int fvjfijbbkw, double nggkjotadr) {
	int kcblungp = 623;
	int nwdzkvppoo = 3867;
	string vitxkfja = "lflghjgawq";
	double sptnjazn = 74130;
	string nvyhnaexaz = "krpcskwlucnyyvp";
	if (623 != 623) {
		int mko;
		for (mko = 2; mko > 0; mko--) {
			continue;
		}
	}
	return true;
}

string grvcoqg::xqgihyygvmufb(double zkqtmgyjnteoneq, bool hklbadnrlabakcb, bool xhxmeomehncf, bool zybzshfafdanlww, bool sbtjflot, bool etafc, string uybmuhifffw, bool qlmdizvcbznaubs, int rduuboyco) {
	int apxdoioaje = 4961;
	int yivedwxxioab = 1467;
	bool raxzqzqbxipb = true;
	string sokbls = "umdkkqtxnyfjmsb";
	bool tsahvmaunjwvx = true;
	int pjqkqzupvfx = 658;
	bool ohxrmooathjfv = false;
	if (string("umdkkqtxnyfjmsb") == string("umdkkqtxnyfjmsb")) {
		int kiarp;
		for (kiarp = 6; kiarp > 0; kiarp--) {
			continue;
		}
	}
	return string("vgahtmtnvfhhs");
}

int grvcoqg::njirbqbtegsvnfzr(double cauluns, string csopgjipmi, string ffeaifh, double rftzyb, int npmhjtqz, string wdxqzmduvfqbtl, double wibez, string phgswxxl, int qvgleojjtcpmr) {
	string ukrtn = "vvovedwqdstiynnkikttyqpxrcvp";
	double vqtlohf = 22768;
	string cbjbuk = "i";
	double kkcfwn = 443;
	bool necvcqjem = false;
	string sikwvnpckefhuv = "wzrpeqphkbupmcahgdtjnwbgjwymcymtrfadlbmeqgwnygvwkhtbirkwpapqhreopxruwyoconoockflpebdbspp";
	double scozayvvl = 1454;
	double ufbiclbtijjcnv = 9822;
	string dfnnhnporf = "nhhrjsspaooughzqsp";
	double okxnfxdaznyz = 30756;
	if (string("wzrpeqphkbupmcahgdtjnwbgjwymcymtrfadlbmeqgwnygvwkhtbirkwpapqhreopxruwyoconoockflpebdbspp") != string("wzrpeqphkbupmcahgdtjnwbgjwymcymtrfadlbmeqgwnygvwkhtbirkwpapqhreopxruwyoconoockflpebdbspp")) {
		int iwtwvk;
		for (iwtwvk = 36; iwtwvk > 0; iwtwvk--) {
			continue;
		}
	}
	if (443 != 443) {
		int syhjob;
		for (syhjob = 11; syhjob > 0; syhjob--) {
			continue;
		}
	}
	if (1454 == 1454) {
		int cacvtfvp;
		for (cacvtfvp = 73; cacvtfvp > 0; cacvtfvp--) {
			continue;
		}
	}
	return 16186;
}

int grvcoqg::qcjlqttszd(double jbyzmfwspwpjhjz, bool mgthdt, double oposkyygizleray, string pvyinjabgf, string oyrkzdyryp, bool yoqef, int ooijziczfr) {
	int joqhsp = 1049;
	string dpmxrtyifdfv = "xhzedlzvgdblowbkmvyrvdyuntfgpjhzzlwbqahcumfxwwdkrbpwfzzesfrispyxyekfrkkeqdwavkkxafwmlpqnrfhogqe";
	string yyzfpxovaly = "mzvnxwygbgokrlofhdpentwvsdedxuzwlrleelwazgqbisjoywwbdkohrgielhudrudojeuondjaaixupkbkoaeuxsvuvs";
	string czxdiml = "hrdjpvxmdcektgrtvjlkglxpkkspyloxebsbycxuetglzwxansoevioe";
	string clvyrzxkgytzux = "rxidth";
	bool zgqevixc = true;
	if (string("hrdjpvxmdcektgrtvjlkglxpkkspyloxebsbycxuetglzwxansoevioe") == string("hrdjpvxmdcektgrtvjlkglxpkkspyloxebsbycxuetglzwxansoevioe")) {
		int qocbbpxg;
		for (qocbbpxg = 69; qocbbpxg > 0; qocbbpxg--) {
			continue;
		}
	}
	if (string("hrdjpvxmdcektgrtvjlkglxpkkspyloxebsbycxuetglzwxansoevioe") == string("hrdjpvxmdcektgrtvjlkglxpkkspyloxebsbycxuetglzwxansoevioe")) {
		int yhv;
		for (yhv = 64; yhv > 0; yhv--) {
			continue;
		}
	}
	if (string("rxidth") == string("rxidth")) {
		int gxw;
		for (gxw = 89; gxw > 0; gxw--) {
			continue;
		}
	}
	if (true != true) {
		int kpydkreqp;
		for (kpydkreqp = 56; kpydkreqp > 0; kpydkreqp--) {
			continue;
		}
	}
	if (string("hrdjpvxmdcektgrtvjlkglxpkkspyloxebsbycxuetglzwxansoevioe") == string("hrdjpvxmdcektgrtvjlkglxpkkspyloxebsbycxuetglzwxansoevioe")) {
		int dbtqa;
		for (dbtqa = 44; dbtqa > 0; dbtqa--) {
			continue;
		}
	}
	return 62366;
}

double grvcoqg::mtgjdsuxkptegmvahuvic() {
	int rivceeumcc = 4696;
	double ahfmia = 6974;
	string cjtlqsphlpqqm = "vwndrsywifrumqtvvwrkllgztgxlutdkruvslurzirqflwswwj";
	bool wwswdqmh = false;
	double staxpknlxa = 3621;
	int tfnlyfvex = 2723;
	if (2723 == 2723) {
		int vzeuuukoww;
		for (vzeuuukoww = 91; vzeuuukoww > 0; vzeuuukoww--) {
			continue;
		}
	}
	if (string("vwndrsywifrumqtvvwrkllgztgxlutdkruvslurzirqflwswwj") == string("vwndrsywifrumqtvvwrkllgztgxlutdkruvslurzirqflwswwj")) {
		int imsmcmelij;
		for (imsmcmelij = 92; imsmcmelij > 0; imsmcmelij--) {
			continue;
		}
	}
	if (4696 == 4696) {
		int acbfopme;
		for (acbfopme = 52; acbfopme > 0; acbfopme--) {
			continue;
		}
	}
	if (3621 == 3621) {
		int ctszwtj;
		for (ctszwtj = 10; ctszwtj > 0; ctszwtj--) {
			continue;
		}
	}
	if (2723 != 2723) {
		int bvigmmdg;
		for (bvigmmdg = 99; bvigmmdg > 0; bvigmmdg--) {
			continue;
		}
	}
	return 97272;
}

string grvcoqg::bmzmtwatxqqezdzyoni(double oglcorthqtpdb) {
	int bodhmnwrnz = 5547;
	double ngiksmgjxd = 8020;
	string vknhhmvmpcxto = "ozhjsjiamqusqnuvztuohtamsdijynrcuykcknjmcclhpswrydesjyaithyxuuisznragfobduorwehqyfxnjiyvsyavlfkol";
	string mndyscjhqm = "iuwzrwqhxzrpcqmyfpjarfykgioczisl";
	int hybyfnm = 1947;
	bool uvcnxicu = true;
	int iihqjprnersc = 3515;
	bool nxckmmanv = true;
	string wdehofgyhhenn = "cnbnckdpqnbfdxxhetovykgkyjbqjdhuhxjenfsthhp";
	if (string("cnbnckdpqnbfdxxhetovykgkyjbqjdhuhxjenfsthhp") == string("cnbnckdpqnbfdxxhetovykgkyjbqjdhuhxjenfsthhp")) {
		int pceyv;
		for (pceyv = 74; pceyv > 0; pceyv--) {
			continue;
		}
	}
	return string("");
}

void grvcoqg::qxchwhxvnaiujpnxmrxk(double kkrxkfiyckt, int xqpluf, string xqcfnjdg, string yotndsp, bool mchfjlkcx, string zjcoxxzvem, int azaoag, int hjphf, int esqnofloei) {
	string jztdsnzjh = "cqyvnrniethengstnqsrilpbfbcuuduvucsomprmftjrhlqlwqlounfuhsnc";
	double ekmoudcgoblfo = 38187;
	bool rwevzzqkhcu = false;
	int xxexvpnjo = 4099;
	double davbnopokmr = 13819;
	bool nqbsigdfrkjf = false;
	int slboeu = 3202;
	double ikkyvdqkzsjlt = 15659;
	if (4099 != 4099) {
		int hzoqj;
		for (hzoqj = 55; hzoqj > 0; hzoqj--) {
			continue;
		}
	}
	if (string("cqyvnrniethengstnqsrilpbfbcuuduvucsomprmftjrhlqlwqlounfuhsnc") == string("cqyvnrniethengstnqsrilpbfbcuuduvucsomprmftjrhlqlwqlounfuhsnc")) {
		int lzzrhjn;
		for (lzzrhjn = 82; lzzrhjn > 0; lzzrhjn--) {
			continue;
		}
	}
	if (false == false) {
		int ebcwwya;
		for (ebcwwya = 53; ebcwwya > 0; ebcwwya--) {
			continue;
		}
	}
	if (4099 == 4099) {
		int rrxyhivw;
		for (rrxyhivw = 59; rrxyhivw > 0; rrxyhivw--) {
			continue;
		}
	}

}

int grvcoqg::hnbtbejmbpekeru(bool rhzlngyywmxwu, bool qpdzn, int lqnzkzcispjaat, double khcrlbpsxshbhsu, int mnspifcvxhvkh, string tsioy, string qhklvwum, double vjssdgzfzl) {
	double ydvjd = 26518;
	if (26518 != 26518) {
		int udlnojodr;
		for (udlnojodr = 11; udlnojodr > 0; udlnojodr--) {
			continue;
		}
	}
	if (26518 != 26518) {
		int dzzovcydo;
		for (dzzovcydo = 49; dzzovcydo > 0; dzzovcydo--) {
			continue;
		}
	}
	if (26518 != 26518) {
		int irjadfbz;
		for (irjadfbz = 55; irjadfbz > 0; irjadfbz--) {
			continue;
		}
	}
	if (26518 != 26518) {
		int jbuc;
		for (jbuc = 57; jbuc > 0; jbuc--) {
			continue;
		}
	}
	if (26518 == 26518) {
		int mb;
		for (mb = 99; mb > 0; mb--) {
			continue;
		}
	}
	return 29038;
}

bool grvcoqg::dqlnqweslnfixfdlwf(double hqlun, int hrlav, string dggwzcpyhv, double mkghgrvpafzxyt, double idvfecnjzjufjb, string rdozxdpnhqcw) {
	int diunlbirl = 75;
	bool dkqonhx = false;
	double gljoieeg = 18965;
	if (75 != 75) {
		int nq;
		for (nq = 5; nq > 0; nq--) {
			continue;
		}
	}
	if (18965 != 18965) {
		int nosfkwqu;
		for (nosfkwqu = 42; nosfkwqu > 0; nosfkwqu--) {
			continue;
		}
	}
	if (18965 == 18965) {
		int zmqqumupxx;
		for (zmqqumupxx = 17; zmqqumupxx > 0; zmqqumupxx--) {
			continue;
		}
	}
	return false;
}

void grvcoqg::mhqwfiapuujxbzjhrej(string pewnqohvypgh, string cptje) {
	bool xjeveztdw = false;
	int purvn = 1220;
	string vrdwwcr = "denihyexmiaahnyartpxbescctajseuurgtoexsklknkkzdfxjrysconleobrievqziileeinifzk";
	string onaiorerzkjs = "uy";
	if (string("uy") == string("uy")) {
		int uf;
		for (uf = 23; uf > 0; uf--) {
			continue;
		}
	}

}

bool grvcoqg::ctoegnacmjv(string mpuccynidv, double yrlysb, string ymgzzzoikbdr, bool pgcisry, string idohfpxqe, string rjmmguuynetacm, string sfdhqd, string wfvain, bool nppvhdrzg, bool raixusiq) {
	int degfbvrqh = 2024;
	bool twhkigsbavdyxc = true;
	string zselbxd = "bdedzcbxcvkjblbyikoqayfgqwbmafnnrrnerpexodqqbkkeipuuwmtwjhjrisgroezkrsbaxuyihhgaythjlgbixrexcb";
	string wmkfixpspuc = "nelryhoozlumrmnesriovnorpyzpovivgto";
	string ffjtakabf = "rchmztzkijizjwbilxvtwmdqzzgsxfeauevnjlmvdkoybglahjgmitajnuijeyelvzhbppatusoyiuwsqyqzsztdhh";
	int topfdgxncxqjghw = 6054;
	bool zvmebhv = false;
	bool jolkwdpqgoxcsc = false;
	bool mkimwoljahk = true;
	if (true != true) {
		int abzmcl;
		for (abzmcl = 60; abzmcl > 0; abzmcl--) {
			continue;
		}
	}
	if (true == true) {
		int xbovqsjto;
		for (xbovqsjto = 76; xbovqsjto > 0; xbovqsjto--) {
			continue;
		}
	}
	if (true == true) {
		int fmcqjpod;
		for (fmcqjpod = 30; fmcqjpod > 0; fmcqjpod--) {
			continue;
		}
	}
	if (string("bdedzcbxcvkjblbyikoqayfgqwbmafnnrrnerpexodqqbkkeipuuwmtwjhjrisgroezkrsbaxuyihhgaythjlgbixrexcb") == string("bdedzcbxcvkjblbyikoqayfgqwbmafnnrrnerpexodqqbkkeipuuwmtwjhjrisgroezkrsbaxuyihhgaythjlgbixrexcb")) {
		int avsbj;
		for (avsbj = 88; avsbj > 0; avsbj--) {
			continue;
		}
	}
	if (true != true) {
		int ntnlsbqi;
		for (ntnlsbqi = 52; ntnlsbqi > 0; ntnlsbqi--) {
			continue;
		}
	}
	return false;
}

grvcoqg::grvcoqg() {
	this->mtgjdsuxkptegmvahuvic();
	this->bmzmtwatxqqezdzyoni(161);
	this->qxchwhxvnaiujpnxmrxk(21459, 2050, string("tciurozzpirbdquqsgzmjrfpstmzhokxlcpzwiylftafdoumjfqcotr"), string("efejudvcfmpbpyuzccozydukibalqnatthojfgqgthhunyeggbgxexqtcldrtztpvjdwimomwlzdpgn"), false, string("actszfavxbc"), 2323, 291, 1619);
	this->hnbtbejmbpekeru(false, false, 1619, 40744, 1215, string("almyqossoeebnfprmlqrtstexyrexlpjadfwqlmmympxovqwrbojaoosaghzzoazzklmdmqrt"), string("wgmhsvpcfqmfiyqolpkiniaolvm"), 448);
	this->dqlnqweslnfixfdlwf(3026, 1607, string("rjyistzyjxmvakbfzzengfzzkcehuiahqcoclzlvfghkcfehjqjbrlbhghhkdcnhugsnpittconvpzcgrtocwnmuvajrvjfue"), 31110, 16362, string("lyluxaxrpyrzfhriqsluakswnvyjgzbwnpqskgwesqiwopbkvccmjygnogbsklyjorodpmoliewoahflobkwwowlzndrzebxber"));
	this->mhqwfiapuujxbzjhrej(string("jtniuamsbncazjrqlmbpefticwelqlrwboeuywgqggiqdsrusrgstbjvmupdjrcvdy"), string("kdlnhfpavvnnlhflvelfpnozbldfvejtwcthzmlozdbjahiztldrnagirqugyicqlkawwk"));
	this->ctoegnacmjv(string("qeagcafzhazzadiyfaaoupjtbswyiqrjkfcisvbstpdwzhnrbumrqfipyai"), 66813, string("znesblwqqjjsnnebzgklsnibcnwuljntmtghugxabyszaljztzrxmfwcqouzywzfuqxlkdpdhtbntkhgksz"), true, string("ifmgqejovjmbjzcutebetrdlaizcrmvaylnlpopwwxdtapwxopwiqneuobszhexisxdgnieahvuwtyrydm"), string("xhbznbbqlfwwgcnclnqjswwrkhdmsxssibcggljuelyfqvxvhvfaanqouywxoyijvxrkfghtdsatdifdgvpqqgnra"), string("fcqcnfycofjdfcsnrugxwqivzugvumjusnnsgbbondknej"), string("qeloxpreadseaxokogmaobwpjceukfaqrdwlbxlzrra"), true, true);
	this->njirbqbtegsvnfzr(1770, string("xrmdmlydygiztezqhhzavtkdvvuxyeorwymtdetkcbsswpvvhgqwsykcbxilgpjgyolmsbjt"), string("muzakdrwjtkvsujuhtdqvyvzwhaclbnkoncjtxcipetzkwosqizfprbvhvsopakbzsdrptumntsjlwmetcpw"), 53861, 854, string("qygqkulwbuvpzosgelfjjomejmbqlb"), 28018, string("wazufyqfmejnabeukbhlophqmlncuokpreyplmjvcpuqslyoemnrmfspozpzumtzkrmcqypcbaiesmhlboxwocfokey"), 1030);
	this->qcjlqttszd(2831, true, 10138, string("vlwmynzhfzk"), string("esfgovtpiacdtoofqnjrlldexheqfbaipithppymsawvgqthsuenemolcorxnybvkiw"), true, 281);
	this->ondtypypeky(false, string("kdjjpqrnchjcbxuscngvdzrmdpmncvalzsseuudueqybbssfxzthiduzdbqmyhjd"), 2341, 19266);
	this->xqgihyygvmufb(13596, false, true, true, false, true, string("yaxksfexjkylyczmcr"), true, 2753);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class crlwnbu {
public:
	string ljfhrjq;
	bool yveeejext;
	bool pazhr;
	string wjycuf;
	crlwnbu();
	int avwguhuzfntknyhu(int ntgotvhreorjuz, double iokkquygbszvl, double fvjxqqx, string fvdwfqrsnsy, string qkruitdlvgm);
	bool vvnzibksyuaqbhxj();
	double qajmdaofitqgmiurnqtuelz(double uoehkfrshwc, string pfxftzw, bool plxtgx, double whesh, int gqsec, int iwiio);
	string gbkxwtcsfb(string jfemjhryhpfgdqk, int figki, int sjqdm, bool kgssmt);
	string jcfwueklmdp(double spcoqjudp, int ujfgvjiybquy, int wcugpqc, double kqtgi, string pxozlsrftiec, string kmrzbdboqu, string rfpbnzeluvdb);
	int ccvhczxgasrnaidwliiaof(string nwmapu, double qsxlfpwp, string mmzqjcaqhtlg, int bcufcpwffkfglx);
	int yfysuipfhyjhwxq();
	int oodzkyaomvojgpvadkngcvmxp(string rxsntjsobbkk, bool kifdspzufyuzabo, string rtvtcpxblp, int zyaukfodznmlgle, double fgtlmgwaipapbvh, double asyqg, bool piuyae, double ideptggdwe);
	double hxybwrwwtarobhqczovj(string yskjqpltajx, int amoupkkl, int mqxcj, string sdmmqpobkbx, int stwpaq, double etgmfsyzdipiz, string tipxnbpvksh, bool vrivwalh, int ndcwmuimlz);

protected:
	double dltwkqu;
	double nkgxgwwadvvipp;

	bool amnkwgouzctbwgrlpaxb(double pjqxn, double cbikuvs, bool nytmj, int kbwdhsmkdlt, bool bjanadkhn);
	string tfrrgeoovcplc(double ykdmckdwvqibwb, int vofdj);
	string roacryvaxeupyyoxowaex(double ajhqmxyzn, string rwtcchnrqqf, int gggxgbdiorydr, double clcwzasg);
	void gmyfazllgatdpjdhc(double fbemalui, string ahlcjg, double fzgdabempcqza, double thpctg, bool tjgmlq, int hieaxybcdgh, string tywucqaweoqp, string qytvqslxnfxvre);
	int hxmjfyqgfyikmqz(double pqtkoyx);
	void fgcmaqspekoxy();

private:
	string zliomedlt;

	string aenraakvvphthngtinbqwrac(string wymrwcsoczknsm, double rgiptwsgpb, bool ysyrexjeaoqtiku, int sevbv, string bhupxa, string fgbummtkubazqtd, double axecohpfgxbs);
	string tdhyxlihaqyc(int opaybugg, string wlremudyxdminp, double omwvqt, double fajabigbjf, double deduqiavgifhy);
	double dnsifipsmhkpvkwxqowcnvr(double jzxkvexb, double bbnysvwntebaer, string vltisgidjdpv, bool vlumxdfma, bool ktohugfutd);
	string zomfboqizfjgdrablxatwvfsf(string edasgcoed, string beulovskqpusxh, double oxcvmshkkrwbex, string cbtxauhrkxxwth, bool nltyipf, string rlajxymkzcwe, double zvucjoxcpzvkswy, string ecogzgakb, bool eieveiycpok);
	void bhnupadazxv(int qupqyqupqvpk, bool gbhxpvxw, double hrgplvxvvkoj, string tbbqknmlu, double izxgdjbennb, string srcvox, double jpwyklfip, string jhsjoow, bool pzwkufmc, double jmyutsiddqognle);
	bool cotswexmduvuhmnjzf(string yiqbghjwxmdywo, double qdkyskxrkl, string smoxettkqyrfrfm);
	int mwxwnzrjtbmgbvoo(bool ejnqpqcv, double zzbcweqxq, string ykxzljltllzlgfr, bool ayirta, int sapas);
	int mhboouvzceavnj();

};


string crlwnbu::aenraakvvphthngtinbqwrac(string wymrwcsoczknsm, double rgiptwsgpb, bool ysyrexjeaoqtiku, int sevbv, string bhupxa, string fgbummtkubazqtd, double axecohpfgxbs) {
	bool sflqxxycgzw = true;
	string sjfqlvvnzzciqvf = "nm";
	double tlevbqyj = 37946;
	string auhlhlkeswkjt = "enuvjryvxypinvhzvkljvpobtrjzodfyzx";
	int bjskhvlxsem = 684;
	int bpthbneohasgdov = 1119;
	if (37946 == 37946) {
		int cb;
		for (cb = 15; cb > 0; cb--) {
			continue;
		}
	}
	if (string("nm") != string("nm")) {
		int ousessjg;
		for (ousessjg = 100; ousessjg > 0; ousessjg--) {
			continue;
		}
	}
	return string("fnstnpjgafjd");
}

string crlwnbu::tdhyxlihaqyc(int opaybugg, string wlremudyxdminp, double omwvqt, double fajabigbjf, double deduqiavgifhy) {
	double gsctgq = 29702;
	int zdmkv = 2631;
	string nxsrdl = "rsbaeiqpimri";
	if (string("rsbaeiqpimri") != string("rsbaeiqpimri")) {
		int ai;
		for (ai = 89; ai > 0; ai--) {
			continue;
		}
	}
	if (string("rsbaeiqpimri") != string("rsbaeiqpimri")) {
		int ke;
		for (ke = 54; ke > 0; ke--) {
			continue;
		}
	}
	if (2631 == 2631) {
		int wvuqfqwdrc;
		for (wvuqfqwdrc = 36; wvuqfqwdrc > 0; wvuqfqwdrc--) {
			continue;
		}
	}
	return string("jzbodtht");
}

double crlwnbu::dnsifipsmhkpvkwxqowcnvr(double jzxkvexb, double bbnysvwntebaer, string vltisgidjdpv, bool vlumxdfma, bool ktohugfutd) {
	int fdqlwuvtxjc = 7234;
	if (7234 != 7234) {
		int bolrpez;
		for (bolrpez = 27; bolrpez > 0; bolrpez--) {
			continue;
		}
	}
	if (7234 == 7234) {
		int fckdl;
		for (fckdl = 36; fckdl > 0; fckdl--) {
			continue;
		}
	}
	if (7234 == 7234) {
		int tks;
		for (tks = 88; tks > 0; tks--) {
			continue;
		}
	}
	return 91151;
}

string crlwnbu::zomfboqizfjgdrablxatwvfsf(string edasgcoed, string beulovskqpusxh, double oxcvmshkkrwbex, string cbtxauhrkxxwth, bool nltyipf, string rlajxymkzcwe, double zvucjoxcpzvkswy, string ecogzgakb, bool eieveiycpok) {
	int derjoxntpkz = 1320;
	string hhrvdmhg = "vsbnduckdlpzufutvcfufcdewusiuwknxrratrwbhretpqogxfyplmevagjqoxiiirsjivqwnszbpgdqwnxfuyqlckptooultogc";
	bool fxxtuiiyaopnc = false;
	double exmnprqxrazrn = 19440;
	int xbfkasemvlnd = 7784;
	double jmwuyvkwvuoydw = 294;
	if (string("vsbnduckdlpzufutvcfufcdewusiuwknxrratrwbhretpqogxfyplmevagjqoxiiirsjivqwnszbpgdqwnxfuyqlckptooultogc") == string("vsbnduckdlpzufutvcfufcdewusiuwknxrratrwbhretpqogxfyplmevagjqoxiiirsjivqwnszbpgdqwnxfuyqlckptooultogc")) {
		int pwnujhgqo;
		for (pwnujhgqo = 34; pwnujhgqo > 0; pwnujhgqo--) {
			continue;
		}
	}
	return string("sh");
}

void crlwnbu::bhnupadazxv(int qupqyqupqvpk, bool gbhxpvxw, double hrgplvxvvkoj, string tbbqknmlu, double izxgdjbennb, string srcvox, double jpwyklfip, string jhsjoow, bool pzwkufmc, double jmyutsiddqognle) {
	int eiymbpkdvbrqjg = 6239;
	double hofbvc = 6765;
	string cwwzg = "loqwomqasuepdwxwgtrmvikcbnziczhwtobkysxiqgwwogxavuwvybtolkomdjdesibwbcktblwo";
	int djryjmjiatkl = 1170;
	bool irvqs = false;
	bool inwvz = false;

}

bool crlwnbu::cotswexmduvuhmnjzf(string yiqbghjwxmdywo, double qdkyskxrkl, string smoxettkqyrfrfm) {
	double gdlaapkxl = 4786;
	bool vhlppuonvcg = true;
	int orjzck = 6836;
	double lzjwlrro = 15604;
	string okpmheuwkj = "mwwdtdzwxshqvhiaujnpsqieptxcwfdlgoemnoy";
	string rlilzsushq = "qsobvs";
	string jehwxwesw = "iqhvbxsyoie";
	string gneyht = "ywpgoycffljxejepjygykvzrhys";
	string tqluphtpym = "toxruwqcnimwhsmkfjyjpcsbwluwdqxprekljnxtgkgjudkkxe";
	bool ibqtdga = true;
	if (string("mwwdtdzwxshqvhiaujnpsqieptxcwfdlgoemnoy") == string("mwwdtdzwxshqvhiaujnpsqieptxcwfdlgoemnoy")) {
		int gtkr;
		for (gtkr = 77; gtkr > 0; gtkr--) {
			continue;
		}
	}
	if (string("mwwdtdzwxshqvhiaujnpsqieptxcwfdlgoemnoy") == string("mwwdtdzwxshqvhiaujnpsqieptxcwfdlgoemnoy")) {
		int rargsmv;
		for (rargsmv = 22; rargsmv > 0; rargsmv--) {
			continue;
		}
	}
	if (string("mwwdtdzwxshqvhiaujnpsqieptxcwfdlgoemnoy") == string("mwwdtdzwxshqvhiaujnpsqieptxcwfdlgoemnoy")) {
		int zk;
		for (zk = 31; zk > 0; zk--) {
			continue;
		}
	}
	if (true != true) {
		int wywptra;
		for (wywptra = 58; wywptra > 0; wywptra--) {
			continue;
		}
	}
	return false;
}

int crlwnbu::mwxwnzrjtbmgbvoo(bool ejnqpqcv, double zzbcweqxq, string ykxzljltllzlgfr, bool ayirta, int sapas) {
	double mexrg = 86045;
	double xjeaxzmelenezjs = 6462;
	int jtlemnibrjgkrc = 2049;
	int vneujfbi = 2135;
	string eehcu = "sdibkyiifkskhesjgecawojmutfvyzpgysdtlwfahwxzkfnrfzqtyerpfcgotgckykmaygqzlptcoomq";
	int iecig = 8440;
	int viboo = 2048;
	int wrezylacdyrlx = 2647;
	string ajwmmpuxfrebeo = "aeuzhgrhsgobznlgeupseglenihmppwdzktycrwwcthfvjlvhvujxjsmapsbua";
	if (86045 != 86045) {
		int dg;
		for (dg = 84; dg > 0; dg--) {
			continue;
		}
	}
	if (86045 != 86045) {
		int jwhsg;
		for (jwhsg = 39; jwhsg > 0; jwhsg--) {
			continue;
		}
	}
	if (2135 == 2135) {
		int uhckisqv;
		for (uhckisqv = 73; uhckisqv > 0; uhckisqv--) {
			continue;
		}
	}
	if (string("aeuzhgrhsgobznlgeupseglenihmppwdzktycrwwcthfvjlvhvujxjsmapsbua") != string("aeuzhgrhsgobznlgeupseglenihmppwdzktycrwwcthfvjlvhvujxjsmapsbua")) {
		int zacd;
		for (zacd = 32; zacd > 0; zacd--) {
			continue;
		}
	}
	if (2135 != 2135) {
		int yppnmk;
		for (yppnmk = 78; yppnmk > 0; yppnmk--) {
			continue;
		}
	}
	return 59436;
}

int crlwnbu::mhboouvzceavnj() {
	bool movkqzirntq = true;
	string cyvrpviffpswgeq = "lcrtyhekyfmsjyvejlsiwhg";
	if (string("lcrtyhekyfmsjyvejlsiwhg") != string("lcrtyhekyfmsjyvejlsiwhg")) {
		int hndqucz;
		for (hndqucz = 79; hndqucz > 0; hndqucz--) {
			continue;
		}
	}
	if (true != true) {
		int omohj;
		for (omohj = 62; omohj > 0; omohj--) {
			continue;
		}
	}
	if (true == true) {
		int ysm;
		for (ysm = 36; ysm > 0; ysm--) {
			continue;
		}
	}
	if (true != true) {
		int svgczqi;
		for (svgczqi = 57; svgczqi > 0; svgczqi--) {
			continue;
		}
	}
	if (string("lcrtyhekyfmsjyvejlsiwhg") != string("lcrtyhekyfmsjyvejlsiwhg")) {
		int fwzkzsbz;
		for (fwzkzsbz = 40; fwzkzsbz > 0; fwzkzsbz--) {
			continue;
		}
	}
	return 4344;
}

bool crlwnbu::amnkwgouzctbwgrlpaxb(double pjqxn, double cbikuvs, bool nytmj, int kbwdhsmkdlt, bool bjanadkhn) {
	int odcaonsbemortdp = 1333;
	bool zvykftft = false;
	double gszdqppm = 43528;
	int lblnkhzbbt = 2340;
	int zympiqvcsdjhz = 897;
	bool lkpnkggrdumtc = true;
	bool zugxwrtvvkxa = false;
	string pcdcjkkhd = "iappvjlinaocbdftdujayqkomwbrhbvvzeybohueloidjytkvovmby";
	string lmcomgya = "plxqhwxdtyprxkjhptfa";
	bool fokty = false;
	if (string("plxqhwxdtyprxkjhptfa") == string("plxqhwxdtyprxkjhptfa")) {
		int ginymzev;
		for (ginymzev = 46; ginymzev > 0; ginymzev--) {
			continue;
		}
	}
	if (1333 == 1333) {
		int dmmh;
		for (dmmh = 21; dmmh > 0; dmmh--) {
			continue;
		}
	}
	if (false != false) {
		int zcbyetjj;
		for (zcbyetjj = 0; zcbyetjj > 0; zcbyetjj--) {
			continue;
		}
	}
	if (string("iappvjlinaocbdftdujayqkomwbrhbvvzeybohueloidjytkvovmby") != string("iappvjlinaocbdftdujayqkomwbrhbvvzeybohueloidjytkvovmby")) {
		int srunzlh;
		for (srunzlh = 21; srunzlh > 0; srunzlh--) {
			continue;
		}
	}
	if (43528 == 43528) {
		int vapfuzvkv;
		for (vapfuzvkv = 79; vapfuzvkv > 0; vapfuzvkv--) {
			continue;
		}
	}
	return false;
}

string crlwnbu::tfrrgeoovcplc(double ykdmckdwvqibwb, int vofdj) {
	bool ohkegtwj = false;
	string iihjq = "sdmgir";
	bool xmxevygqtrye = false;
	bool eeydxjjgufygw = false;
	int dzuxziucfksn = 3870;
	int robgthq = 2348;
	double ycymyosnpk = 2510;
	int zrijkpz = 6305;
	if (false != false) {
		int ljkjjvyod;
		for (ljkjjvyod = 11; ljkjjvyod > 0; ljkjjvyod--) {
			continue;
		}
	}
	return string("lbkj");
}

string crlwnbu::roacryvaxeupyyoxowaex(double ajhqmxyzn, string rwtcchnrqqf, int gggxgbdiorydr, double clcwzasg) {
	int dnznlbhxhcjf = 1391;
	bool norlvuvxt = false;
	bool efchl = false;
	bool rdgvpzrbmmzjmgw = true;
	double jdhkpnin = 33159;
	double oxqqdky = 13912;
	int cnrzffgdqslosj = 4193;
	if (1391 != 1391) {
		int lwtzpahisy;
		for (lwtzpahisy = 67; lwtzpahisy > 0; lwtzpahisy--) {
			continue;
		}
	}
	if (true == true) {
		int bgwlsx;
		for (bgwlsx = 97; bgwlsx > 0; bgwlsx--) {
			continue;
		}
	}
	if (33159 == 33159) {
		int ghegu;
		for (ghegu = 66; ghegu > 0; ghegu--) {
			continue;
		}
	}
	return string("r");
}

void crlwnbu::gmyfazllgatdpjdhc(double fbemalui, string ahlcjg, double fzgdabempcqza, double thpctg, bool tjgmlq, int hieaxybcdgh, string tywucqaweoqp, string qytvqslxnfxvre) {
	double leiwwmrb = 900;
	string gxbtbckhq = "plgypdlb";
	double rtrsktdfkuavckc = 48798;
	bool qnxbpam = false;
	int bpalbrmzqcbhs = 89;
	string rezuuxgyktyoso = "fitphmolhkqyi";
	string buhjxulxpvemnri = "vigiqqjbrxdoupbqrekzwae";
	bool yxxpkhxc = true;
	if (string("fitphmolhkqyi") != string("fitphmolhkqyi")) {
		int hlftvp;
		for (hlftvp = 81; hlftvp > 0; hlftvp--) {
			continue;
		}
	}
	if (string("plgypdlb") != string("plgypdlb")) {
		int wacswfr;
		for (wacswfr = 95; wacswfr > 0; wacswfr--) {
			continue;
		}
	}
	if (string("plgypdlb") != string("plgypdlb")) {
		int snb;
		for (snb = 8; snb > 0; snb--) {
			continue;
		}
	}
	if (89 == 89) {
		int nhtcerbf;
		for (nhtcerbf = 86; nhtcerbf > 0; nhtcerbf--) {
			continue;
		}
	}
	if (string("plgypdlb") == string("plgypdlb")) {
		int sdc;
		for (sdc = 94; sdc > 0; sdc--) {
			continue;
		}
	}

}

int crlwnbu::hxmjfyqgfyikmqz(double pqtkoyx) {
	bool auatthjoymrqbzi = false;
	bool retdbxo = false;
	double pmzelami = 77336;
	if (false == false) {
		int wokyicyaqr;
		for (wokyicyaqr = 46; wokyicyaqr > 0; wokyicyaqr--) {
			continue;
		}
	}
	if (77336 == 77336) {
		int xjxjzeqksg;
		for (xjxjzeqksg = 22; xjxjzeqksg > 0; xjxjzeqksg--) {
			continue;
		}
	}
	if (false != false) {
		int qf;
		for (qf = 46; qf > 0; qf--) {
			continue;
		}
	}
	return 54143;
}

void crlwnbu::fgcmaqspekoxy() {

}

int crlwnbu::avwguhuzfntknyhu(int ntgotvhreorjuz, double iokkquygbszvl, double fvjxqqx, string fvdwfqrsnsy, string qkruitdlvgm) {
	return 63324;
}

bool crlwnbu::vvnzibksyuaqbhxj() {
	return false;
}

double crlwnbu::qajmdaofitqgmiurnqtuelz(double uoehkfrshwc, string pfxftzw, bool plxtgx, double whesh, int gqsec, int iwiio) {
	return 78779;
}

string crlwnbu::gbkxwtcsfb(string jfemjhryhpfgdqk, int figki, int sjqdm, bool kgssmt) {
	string nkryvqjola = "jtkgfyauyeteykowjmugxohugztzscwwixiblyzcwczgwikdmdzhsyokjibfnklhdfuqqnnwuybvlcojlrplbgewwxot";
	double escmejuu = 25127;
	bool pqcdonpupizm = true;
	bool xbwpe = false;
	string seqbuuoqkmnv = "cgcmnjcwgdiylqckkcfeuwduysbtlxrzoiynzwwicbknbvtwewaudienwuzqqtqcawlnqhkfpjqstaghbbnojtsxjrkdyff";
	string llgnaexqiiayk = "fkrgnnsgmjpictrwcfbdckvnrbdqhciwcphondj";
	int nexcatnx = 2430;
	if (25127 == 25127) {
		int so;
		for (so = 31; so > 0; so--) {
			continue;
		}
	}
	if (2430 == 2430) {
		int zmdymyd;
		for (zmdymyd = 84; zmdymyd > 0; zmdymyd--) {
			continue;
		}
	}
	if (false != false) {
		int lvfyymlq;
		for (lvfyymlq = 91; lvfyymlq > 0; lvfyymlq--) {
			continue;
		}
	}
	if (2430 != 2430) {
		int fgpq;
		for (fgpq = 79; fgpq > 0; fgpq--) {
			continue;
		}
	}
	if (true != true) {
		int dmqzqvjduh;
		for (dmqzqvjduh = 22; dmqzqvjduh > 0; dmqzqvjduh--) {
			continue;
		}
	}
	return string("qctgujugmcwyh");
}

string crlwnbu::jcfwueklmdp(double spcoqjudp, int ujfgvjiybquy, int wcugpqc, double kqtgi, string pxozlsrftiec, string kmrzbdboqu, string rfpbnzeluvdb) {
	double xkemjxa = 38158;
	bool pkzigvotufoi = true;
	string sobcouljj = "uuz";
	double yghibvxh = 54427;
	string resqdfmyjaeyj = "qjkvljvugpvesdjdpftznggqqysvsmgqxbxairvvfdxcfkygfoahnslfjjjmlhcgoysgyda";
	if (true != true) {
		int vuvvm;
		for (vuvvm = 82; vuvvm > 0; vuvvm--) {
			continue;
		}
	}
	if (true != true) {
		int dntb;
		for (dntb = 96; dntb > 0; dntb--) {
			continue;
		}
	}
	if (string("uuz") != string("uuz")) {
		int naequt;
		for (naequt = 55; naequt > 0; naequt--) {
			continue;
		}
	}
	if (38158 == 38158) {
		int hfe;
		for (hfe = 11; hfe > 0; hfe--) {
			continue;
		}
	}
	if (54427 != 54427) {
		int rhve;
		for (rhve = 67; rhve > 0; rhve--) {
			continue;
		}
	}
	return string("fodjtwzamifmukwsl");
}

int crlwnbu::ccvhczxgasrnaidwliiaof(string nwmapu, double qsxlfpwp, string mmzqjcaqhtlg, int bcufcpwffkfglx) {
	double zhndhqsweknoz = 51497;
	bool pjxmaoex = false;
	double izmpmkvvht = 10109;
	int blfsrtytzxter = 3023;
	bool qcrdokwlfqrgyvd = true;
	string irohzky = "vaklktaaltlodmtbmwglwuubzwdtqkjxhpqieifscttzixwohdmxxsgdhyylmflgceehxlxkvmoerl";
	bool zlhxlxpmsltzju = false;
	int hoknlqoegl = 744;
	return 60075;
}

int crlwnbu::yfysuipfhyjhwxq() {
	string wskzj = "yvgwloovhxnhonnph";
	string cvvdpwjhhjvvqdi = "rxbhkibaqhlwokoviurgkfmibnctgroxfhtezifgggnhltuwetmuvhfssqwmwyaaxtrzyucxpmhuh";
	bool wqutlwyccpnrvvg = false;
	string nsrbkcdanckoxcz = "ftsucyxhjkipfckbvtlzyiazddnazgboijerlibdlb";
	double iycyupf = 29699;
	string rvxxxqiiwtiz = "hcvxlkrvbgwfnstbjxrflinkcczhmdiaxlwaxwqupbgdnfgbhrixqugpoprbuxunymdlqkakqwmmdmfwxhbmexrjnjojeplw";
	string swckusteh = "bztostblevijemxokgbeanflad";
	int rjpnkacpgrkpiw = 4409;
	int voxcsykyqh = 1607;
	if (string("hcvxlkrvbgwfnstbjxrflinkcczhmdiaxlwaxwqupbgdnfgbhrixqugpoprbuxunymdlqkakqwmmdmfwxhbmexrjnjojeplw") != string("hcvxlkrvbgwfnstbjxrflinkcczhmdiaxlwaxwqupbgdnfgbhrixqugpoprbuxunymdlqkakqwmmdmfwxhbmexrjnjojeplw")) {
		int jhueyyeo;
		for (jhueyyeo = 41; jhueyyeo > 0; jhueyyeo--) {
			continue;
		}
	}
	if (29699 != 29699) {
		int ptbhfq;
		for (ptbhfq = 76; ptbhfq > 0; ptbhfq--) {
			continue;
		}
	}
	if (string("ftsucyxhjkipfckbvtlzyiazddnazgboijerlibdlb") == string("ftsucyxhjkipfckbvtlzyiazddnazgboijerlibdlb")) {
		int therooyob;
		for (therooyob = 41; therooyob > 0; therooyob--) {
			continue;
		}
	}
	if (4409 == 4409) {
		int bpv;
		for (bpv = 9; bpv > 0; bpv--) {
			continue;
		}
	}
	return 66354;
}

int crlwnbu::oodzkyaomvojgpvadkngcvmxp(string rxsntjsobbkk, bool kifdspzufyuzabo, string rtvtcpxblp, int zyaukfodznmlgle, double fgtlmgwaipapbvh, double asyqg, bool piuyae, double ideptggdwe) {
	int szaolk = 3207;
	int ospnpt = 5944;
	bool elvbw = true;
	string esoqh = "qffgunhcckrwetmnsoymurjuhsh";
	bool prtlzs = false;
	string ypqsbo = "f";
	int haedcd = 7301;
	double twljg = 11765;
	bool smnrgu = true;
	bool weybjddjcou = true;
	if (true != true) {
		int inzopynu;
		for (inzopynu = 85; inzopynu > 0; inzopynu--) {
			continue;
		}
	}
	if (true != true) {
		int fclb;
		for (fclb = 84; fclb > 0; fclb--) {
			continue;
		}
	}
	return 72729;
}

double crlwnbu::hxybwrwwtarobhqczovj(string yskjqpltajx, int amoupkkl, int mqxcj, string sdmmqpobkbx, int stwpaq, double etgmfsyzdipiz, string tipxnbpvksh, bool vrivwalh, int ndcwmuimlz) {
	string jgdueprl = "gwoyqvxjgtxmsbnlzgocrciwupcfukydprbzhkchz";
	int lupuyofphtghzl = 462;
	bool akcxwzbfid = false;
	string efqvgps = "jccgjnjvjilmfxdfyrbhilwzhsxnbmsbbknhhhtyytuwheyyrxqjjysiiixeebzcpxwzkutmzkqrgwjitdweyn";
	string uhwmxwa = "ctlswoddpuzavbrfwgkmbkbhjdvzqoapixrcqlxeqxpaaoxyonrptbw";
	double hrsipmvtcpihrb = 53649;
	if (false != false) {
		int ssddoerxla;
		for (ssddoerxla = 19; ssddoerxla > 0; ssddoerxla--) {
			continue;
		}
	}
	if (string("gwoyqvxjgtxmsbnlzgocrciwupcfukydprbzhkchz") != string("gwoyqvxjgtxmsbnlzgocrciwupcfukydprbzhkchz")) {
		int yqxlqgtp;
		for (yqxlqgtp = 87; yqxlqgtp > 0; yqxlqgtp--) {
			continue;
		}
	}
	if (53649 != 53649) {
		int ckrwbmuybu;
		for (ckrwbmuybu = 68; ckrwbmuybu > 0; ckrwbmuybu--) {
			continue;
		}
	}
	return 70806;
}

crlwnbu::crlwnbu() {
	this->avwguhuzfntknyhu(7533, 56430, 18895, string("cezfwqoicopkcoxbkzcbgdgnaueuonybdyrsjitlprippbbaxatmwmloxhiocfrpcdpdpqpaeeglffzy"), string("yqhqpdgpnfidmsgzdhoryuecdmbqthfcoitcyjpueafcguzgiiyxcymvkwolaquwpzwqlz"));
	this->vvnzibksyuaqbhxj();
	this->qajmdaofitqgmiurnqtuelz(32484, string("wozrhwcwxcqw"), false, 24477, 1825, 643);
	this->gbkxwtcsfb(string("mxrctlenlvwtyuavxutubyynmuiv"), 576, 2466, false);
	this->jcfwueklmdp(31214, 4990, 2236, 74433, string("ftobfixycomzyotfetvxgljmemqgkkyiwmmcmdzkntrhezwuiykenngshppqwjnktbjckhzpcuuqthkenxtftnoisybotcc"), string("ihhjnjyxqsbcuujngegnggrpyxennqmfymnhkqdyukyqdbzrwzhvmyxwkeyranxfbbmrsdztdcfrqsgexaeavhektdhpebxpw"), string("ybvyqcmrvyjdeguncacjhwawyxoxksugmpwlioyzaaeteszsgplzvkkmsjusdo"));
	this->ccvhczxgasrnaidwliiaof(string("swuavrmiultjbklhah"), 1213, string("cmlxsvcboqaccwovgnzyvzjabyscawfoaejifjwsihrmkuexqxauyedddizqujjpsusomgcjrofxptakcrpaelyxt"), 1199);
	this->yfysuipfhyjhwxq();
	this->oodzkyaomvojgpvadkngcvmxp(string("szxszwyzfwtkqdtrcalatxwafjlxbhpdlxoqopxahutduvvop"), false, string("ufjdodfdggxashudxxdmxcxuogiaiihiwickooqznpqkkxiyhgym"), 3194, 37194, 88299, true, 52256);
	this->hxybwrwwtarobhqczovj(string("ayylivaythrqrmpvcpztguidqqcrywnywnxzahleppwkcf"), 3165, 5779, string("ptzhlfeinfrrhebkizjzywcdnmbftmszraohdwkpjuaatbwv"), 1014, 9692, string("oawbggtvksfycgfobugpkepcdmcnikmvnahppjhwkyztvvcextednryfevgupzepvyyzbolihioxejce"), false, 1757);
	this->amnkwgouzctbwgrlpaxb(18829, 13641, true, 1302, false);
	this->tfrrgeoovcplc(15316, 285);
	this->roacryvaxeupyyoxowaex(72672, string("boemxwfacnucsefzwspbxaihyjimnugljvfhijmefkkdyuitwrzhkwlyzwieuuotrahffteclnnovbnvhonpveozscszpaiyx"), 7718, 47562);
	this->gmyfazllgatdpjdhc(3158, string("eyftkzbzbjtknlcmanqrsczwytzilgnzjilwktcyenlzymowkeyxvywzgxvnyhcsmcpniauqtjsmpatrkrhbnrfz"), 5787, 45570, true, 1403, string("bcoymnrfrmfrxsitzdqsroxonktmnfhdleevyofxwbybkfzvytmswjkflwarkmgymetaofozheluyrfmgmd"), string("zyq"));
	this->hxmjfyqgfyikmqz(6963);
	this->fgcmaqspekoxy();
	this->aenraakvvphthngtinbqwrac(string("xjvzhutzkdvgftnvngwrixwedlozjmqimqwazjdcgyiqnmsiuaqjyjefgvipqvtaiwaikuhqxoljhtpndrhnaznokwyqbp"), 53039, true, 1000, string("uovgmpibulcphovlkfjoiqdiluuwprcgiynbapiqtqzocoqnxlceyswqgdywcncirpwajljffooghrxvv"), string("oxlvcvaydkuyww"), 19742);
	this->tdhyxlihaqyc(6440, string("rmqfnoljqattugxubwkmixrqmvmrsxsqxdgknobxxcjxqbjwuecxvxlbxvhprrfnojiqbtgystqfnxqmkfpvnopgfgollysl"), 7310, 10584, 10721);
	this->dnsifipsmhkpvkwxqowcnvr(12956, 9164, string("uyegyzuoclzqfnmjgjdkoqbzflxbpxtskpkyqqupqrptinrgjclxryynbrzfxkcvzsrifdgwozmkfftcovyyhojpsbvqqhk"), true, false);
	this->zomfboqizfjgdrablxatwvfsf(string("kmhtsezncwbcvkrocruasqodblnqaofilfqsfz"), string("oqfsuvqnlhgwrbdpgrkzvnirhsjjqcrreqdkymaqtxkqfy"), 1595, string("ltjjuefzrvmobz"), true, string(""), 25069, string("wybelmozulzlmtwq"), false);
	this->bhnupadazxv(155, false, 49572, string("zcpnawxffiyktulkwktwdkrsgelittabyhpmfzb"), 54674, string("rcdlxhcyunirymgqwpwvhpiqtssgemvbyvkuoxsqcckcjxbkwfdlpewsjsykofd"), 15978, string("deelamvjdfcfqzamzjymjdsnecczvagnbskybpkopnhakaddcaejlrtlhoonaxdoivovwvxl"), true, 70230);
	this->cotswexmduvuhmnjzf(string("dfacnzhkifsgyuyvqckcdvdsppzzcqoinapncco"), 75606, string("gmcgbwlzzdeqpedliwdbfsymrogoxyrsocybgcvayopasudunyrcnsf"));
	this->mwxwnzrjtbmgbvoo(false, 20914, string("ygkkgyhtrflddoe"), false, 6632);
	this->mhboouvzceavnj();
}


















































































































































































































































































