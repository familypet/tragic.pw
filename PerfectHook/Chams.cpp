#include "Chams.h"

#include "SDK.h"
#include "Interfaces.h"
#include <sstream>
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

void InitKeyValues(KeyValues* keyValues, const char* name)
{
	static uint8_t* sig1;
	if (!sig1)
	{
		sig1 = U::pattern_scan(GetModuleHandleW(L"client.dll"), "68 ? ? ? ? 8B C8 E8 ? ? ? ? 89 45 FC EB 07 C7 45 ? ? ? ? ? 8B 03 56");
		sig1 += 7;
		sig1 = sig1 + *reinterpret_cast<PDWORD_PTR>(sig1 + 1) + 5;
	}

	static auto function = (void(__thiscall*)(KeyValues*, const char*))sig1;
	function(keyValues, name);

}

void LoadFromBuffer(KeyValues* keyValues, char const* resourceName, const char* pBuffer)
{
	static uint8_t* offset;
	if (!offset) offset = U::pattern_scan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");
	static auto function = (void(__thiscall*)(KeyValues*, char const*, const char*, void*, const char*, void*))offset;
	function(keyValues, resourceName, pBuffer, 0, 0, 0);
}

IMaterial* CreateMaterial(bool Ignore, bool Lit, bool Wireframe)
{
	static int created = 0;
	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\"  \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\"  \"0\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};
	char* baseType = (Lit == true ? "VertexLitGeneric" : "UnlitGeneric");
	char material[512];
	char name[512];
	sprintf_s(material, sizeof(material), tmp, baseType, (Ignore) ? 1 : 0, (Wireframe) ? 1 : 0);
	sprintf_s(name, sizeof(name), "#Aimpulse_Chams_%i.vmt", created);
	++created;
	KeyValues* keyValues = static_cast< KeyValues* >(malloc(sizeof(KeyValues)));
	InitKeyValues(keyValues, baseType);
	LoadFromBuffer(keyValues, name, material);
	IMaterial* createdMaterial = g_MaterialSystem->CreateMaterial(name, keyValues);
	createdMaterial->IncrementReferenceCount();
	return createdMaterial;
}

void ForceMaterial(Color color, IMaterial* material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float temp[3] =
		{
			(float)color.r(),
			(float)color.g(),
			(float)color.b()
		};

		temp[0] /= 255.f;
		temp[1] /= 255.f;
		temp[2] /= 255.f;


		float alpha = (float)color.a();

		g_RenderView->SetBlend(1.0f);
		g_RenderView->SetColorModulation(temp);
	}

	if (forceMaterial)
		g_ModelRender->ForcedMaterialOverride(material);
	else
		g_ModelRender->ForcedMaterialOverride(NULL);
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class mpptqeb {
public:
	bool wqhloxrvcra;
	mpptqeb();
	int cwnfuzicojjddtch(int rddezxanfrdi, bool xmulvbihmrzvwhx, double ausqfc, bool wuucpmtlstp, string zjckezi, int aemnehl, int fnkaiccgfmxcfi, string sjixjuhmfcnnmd, int dzdmz);
	bool gcfxgwbyibs(int rrmgcyopawxee, int tgcrm, bool jdlshzqaj, string bwvkkmcyea, int oxegkwtwt, double pgglzx, int qxjtwvhkkwqttaq, double zknbeo, int inxluwxptudw, bool jqplpxbnu);
	bool eojxkzavieaxf(double ubawiwnl, string ryprpqvtojcd, string phjeydznwbyq, string mxnlwbbzfkf, bool aepdiao, string adtnggjiobgrbd);
	string isiptkxtxd(string ekjxwav, string wfwxu, int okztev);
	double vqfkgxzezudxao(int rtxajaucigzl, bool vdkgklhnpulua, int dtrutjoyvay);
	void zdrzxexyzxr(bool zxwsvcyq, double sgsuuk, int gyelxxj, bool bjjds, bool ykluzbqenuoesih);
	string sxemibvdaxfkhcbietnkifbx(int ezjhfmlbzx, double jqznbzhw, string wgntqqg, int cqxxaiukvwlhl, bool adcpewfjohtxeuf, int txmpmcgvkgtef, double eoyhinepjewin, string prmttujhgxkq, int clqxqpkgg);
	void wdctsnngmpouekbletlpnvhdd(int hfvjklrodcz);
	string tygxubhvpmnlzxfr(int acfmjxgxv);

protected:
	double myeigfuajbgt;
	bool goxbcezia;
	string nwjanjnpvcfleqq;
	double jqihplhgi;

	string cnelyixfaatnbfrkecumqno(bool kfuitqfjmeensz);
	bool heniwtvrtuoopvfpitzyrzxm(bool nnkxv, double dyibstxavyqqlss, bool cmqaan, bool eudimhcsbpnlpib, string vbvmyeycf);
	bool fftzdlhsyecvinnzz(double vwbityowqdtxo, int wpqqxay);

private:
	int dfptalg;
	string diohbojh;
	bool plaub;
	int fkkzcjsrdlilomo;
	int luukfjhchz;

	double hlrlmedbaozc();
	double rwbciopcktjalm(double nscadwxnrsdxi, string yntnnzams, string uzfrj, bool xhxzsnhbcy);
	string atwgjwscnkleqqjxpgounm(bool kipyopiaxadakv, string nkpsxvllzgpfjp, double vinlmpl, string dvhjaunzvxggl, double hupvttxhku, int yzkoubx, int xvkhyekanxva, int bvdmqnadchtzs);
	bool uwolquvtwayhlvddumt();
	int xookibcyugxvuuuvxqtzshy(string disjm, int qcudejizme, string sfehypuy, bool oczdvwfeo, string xqjaah, double zjplh, string dpnnqylnyibn, double magvz, double ccclfs);
	bool yjinzgcwcksingc(string jdzcu, bool dhcgf, int oowfbugjipdl, string ghxudfg, bool zzaqfgg);
	double liriuitzffjkhmcnfjmjffox(int jickno, bool pxutnkkrrvsatg, int iyamgpsqskq, string wwslqitsp, bool btiiyirhlan, bool bvbgovojogq, double qtsyhmlb, int sapbuviqsnavtoe, bool hbeqifzcja, int gncsqkodgisgq);
	double triulqpokkpkeh(string fcsjlohgkhf, int xuxzc, int gzxmhtvleyglea, double thmidfuyvezci, int zvlrgmkrksil, int eegiltej, double ykkyb, string csbita, bool zjdxxxe, int nynoukwemqygjgq);
	int oauwitcfgdcvchvnrvknxyo();

};


double mpptqeb::hlrlmedbaozc() {
	bool izvbrooj = true;
	bool tnboea = true;
	double tyoxhpdtaibfo = 9307;
	double yayenvzcxkxbqbz = 5485;
	string myrya = "cmbpsxkfhjxpmbdqjxmvgcvmvtarxknjclumvqrilddxvmgpbhhrgjndqy";
	int xhgjlcwc = 972;
	if (string("cmbpsxkfhjxpmbdqjxmvgcvmvtarxknjclumvqrilddxvmgpbhhrgjndqy") == string("cmbpsxkfhjxpmbdqjxmvgcvmvtarxknjclumvqrilddxvmgpbhhrgjndqy")) {
		int tr;
		for (tr = 36; tr > 0; tr--) {
			continue;
		}
	}
	return 84069;
}

double mpptqeb::rwbciopcktjalm(double nscadwxnrsdxi, string yntnnzams, string uzfrj, bool xhxzsnhbcy) {
	int eokrrtsvcjgnt = 1398;
	bool klvtwfkedyayrik = false;
	string gyevrdnwbj = "dljfhaahwmyhcfxuetjhganjqrirvivgkrzkbrksualjmcqqkfutbmpoyiojkwjm";
	if (string("dljfhaahwmyhcfxuetjhganjqrirvivgkrzkbrksualjmcqqkfutbmpoyiojkwjm") != string("dljfhaahwmyhcfxuetjhganjqrirvivgkrzkbrksualjmcqqkfutbmpoyiojkwjm")) {
		int zddlbccnfu;
		for (zddlbccnfu = 45; zddlbccnfu > 0; zddlbccnfu--) {
			continue;
		}
	}
	if (string("dljfhaahwmyhcfxuetjhganjqrirvivgkrzkbrksualjmcqqkfutbmpoyiojkwjm") == string("dljfhaahwmyhcfxuetjhganjqrirvivgkrzkbrksualjmcqqkfutbmpoyiojkwjm")) {
		int nqaucsxnj;
		for (nqaucsxnj = 9; nqaucsxnj > 0; nqaucsxnj--) {
			continue;
		}
	}
	return 93055;
}

string mpptqeb::atwgjwscnkleqqjxpgounm(bool kipyopiaxadakv, string nkpsxvllzgpfjp, double vinlmpl, string dvhjaunzvxggl, double hupvttxhku, int yzkoubx, int xvkhyekanxva, int bvdmqnadchtzs) {
	string wjgvfwg = "mdpzzdcrwjcdsgcjgfganfalkmbcshyjuwxalgxagxdbfzopypaxzxvxwxwgmppzjwewbvmqtopfbomukijiuufyqk";
	if (string("mdpzzdcrwjcdsgcjgfganfalkmbcshyjuwxalgxagxdbfzopypaxzxvxwxwgmppzjwewbvmqtopfbomukijiuufyqk") != string("mdpzzdcrwjcdsgcjgfganfalkmbcshyjuwxalgxagxdbfzopypaxzxvxwxwgmppzjwewbvmqtopfbomukijiuufyqk")) {
		int eitocff;
		for (eitocff = 63; eitocff > 0; eitocff--) {
			continue;
		}
	}
	if (string("mdpzzdcrwjcdsgcjgfganfalkmbcshyjuwxalgxagxdbfzopypaxzxvxwxwgmppzjwewbvmqtopfbomukijiuufyqk") == string("mdpzzdcrwjcdsgcjgfganfalkmbcshyjuwxalgxagxdbfzopypaxzxvxwxwgmppzjwewbvmqtopfbomukijiuufyqk")) {
		int io;
		for (io = 52; io > 0; io--) {
			continue;
		}
	}
	return string("brbxx");
}

bool mpptqeb::uwolquvtwayhlvddumt() {
	bool hsjrlwyvwlb = true;
	bool iedox = false;
	bool xydzlgzfzfwg = true;
	int mkbxwr = 2388;
	double zgxjkts = 14549;
	bool duxdqkzqkl = false;
	if (true == true) {
		int gyxpige;
		for (gyxpige = 95; gyxpige > 0; gyxpige--) {
			continue;
		}
	}
	if (14549 == 14549) {
		int guvg;
		for (guvg = 20; guvg > 0; guvg--) {
			continue;
		}
	}
	return false;
}

int mpptqeb::xookibcyugxvuuuvxqtzshy(string disjm, int qcudejizme, string sfehypuy, bool oczdvwfeo, string xqjaah, double zjplh, string dpnnqylnyibn, double magvz, double ccclfs) {
	double cjuuxsgcqjuk = 58774;
	bool hynnqdnyvy = true;
	bool vbdqhuuvlkxeby = false;
	bool znhzuvytw = true;
	double wwgukuufebfimb = 21900;
	if (21900 != 21900) {
		int hgblfraii;
		for (hgblfraii = 17; hgblfraii > 0; hgblfraii--) {
			continue;
		}
	}
	if (true != true) {
		int ojhjrsvo;
		for (ojhjrsvo = 90; ojhjrsvo > 0; ojhjrsvo--) {
			continue;
		}
	}
	if (true != true) {
		int afg;
		for (afg = 48; afg > 0; afg--) {
			continue;
		}
	}
	return 46360;
}

bool mpptqeb::yjinzgcwcksingc(string jdzcu, bool dhcgf, int oowfbugjipdl, string ghxudfg, bool zzaqfgg) {
	string tyupqhrljzu = "qhbywtkxenlpdavqftwfhdnciacdjbahwfjytttrlhy";
	bool ndwsintbccavcfd = true;
	double fmcphoialxdehm = 25049;
	string pjsbdshee = "";
	int gvidhq = 664;
	int xaybgtoihbg = 1645;
	int alkprewkmt = 3173;
	double xdsmuhognan = 3417;
	int uzywomztyr = 5052;
	if (true == true) {
		int fykv;
		for (fykv = 14; fykv > 0; fykv--) {
			continue;
		}
	}
	if (664 == 664) {
		int fazzveodby;
		for (fazzveodby = 87; fazzveodby > 0; fazzveodby--) {
			continue;
		}
	}
	if (5052 != 5052) {
		int yqfzwfprl;
		for (yqfzwfprl = 38; yqfzwfprl > 0; yqfzwfprl--) {
			continue;
		}
	}
	return true;
}

double mpptqeb::liriuitzffjkhmcnfjmjffox(int jickno, bool pxutnkkrrvsatg, int iyamgpsqskq, string wwslqitsp, bool btiiyirhlan, bool bvbgovojogq, double qtsyhmlb, int sapbuviqsnavtoe, bool hbeqifzcja, int gncsqkodgisgq) {
	bool dacmmkvdernfwkf = true;
	string wbjuexpvqu = "zvturyhjbnftfikhsidjevivrgmdxbaztfzizhphzxwowgrapzzaqcaxzfmqldie";
	bool xaifzqbzsxc = true;
	int dqbkbeggssimc = 2121;
	string rzqgc = "zgcuimlkmwlfxm";
	double kuihqhcme = 19132;
	if (2121 != 2121) {
		int ysorcrdqhd;
		for (ysorcrdqhd = 54; ysorcrdqhd > 0; ysorcrdqhd--) {
			continue;
		}
	}
	return 80246;
}

double mpptqeb::triulqpokkpkeh(string fcsjlohgkhf, int xuxzc, int gzxmhtvleyglea, double thmidfuyvezci, int zvlrgmkrksil, int eegiltej, double ykkyb, string csbita, bool zjdxxxe, int nynoukwemqygjgq) {
	int yyfvbbfromnyeqy = 105;
	bool cykzaofc = true;
	int eievnmlorfasx = 3554;
	int dnvfhwzzdv = 2831;
	string hfuzkbx = "unlm";
	string utubouf = "eqfajsvbpz";
	double asvve = 25081;
	double vszfg = 41452;
	bool phxlak = false;
	if (false == false) {
		int jhoz;
		for (jhoz = 12; jhoz > 0; jhoz--) {
			continue;
		}
	}
	if (41452 == 41452) {
		int ubgdtoic;
		for (ubgdtoic = 12; ubgdtoic > 0; ubgdtoic--) {
			continue;
		}
	}
	if (true == true) {
		int digeqr;
		for (digeqr = 29; digeqr > 0; digeqr--) {
			continue;
		}
	}
	if (false != false) {
		int ixgk;
		for (ixgk = 47; ixgk > 0; ixgk--) {
			continue;
		}
	}
	if (false != false) {
		int ieqhnnzsoo;
		for (ieqhnnzsoo = 76; ieqhnnzsoo > 0; ieqhnnzsoo--) {
			continue;
		}
	}
	return 16820;
}

int mpptqeb::oauwitcfgdcvchvnrvknxyo() {
	return 31978;
}

string mpptqeb::cnelyixfaatnbfrkecumqno(bool kfuitqfjmeensz) {
	double xhdkglgsoyzyyj = 1128;
	bool safwd = false;
	int todamb = 402;
	string wkbqo = "idfvhmpyynumesvoquseelmwodhbsjrrjdpbtrlquhnipruveseirdc";
	bool tldzwbaszpv = true;
	double ofgehqfeirdqnty = 13344;
	double qwsokxvhdhsahss = 41789;
	return string("qhmqpfcfafkrdvimigg");
}

bool mpptqeb::heniwtvrtuoopvfpitzyrzxm(bool nnkxv, double dyibstxavyqqlss, bool cmqaan, bool eudimhcsbpnlpib, string vbvmyeycf) {
	string gttlucqletnz = "ilzlyjcrrthsszhczfqolmhyydrlrevrfrebtbpbzqrgebqj";
	bool rhsejdshim = true;
	bool lwkdeeomjrth = true;
	double epjej = 6549;
	double hlhqwcyijdxcr = 8418;
	bool xsuozhmqxc = true;
	double rwekodormfwp = 39527;
	double ttkjw = 19746;
	if (6549 == 6549) {
		int ptr;
		for (ptr = 41; ptr > 0; ptr--) {
			continue;
		}
	}
	if (19746 == 19746) {
		int tewa;
		for (tewa = 74; tewa > 0; tewa--) {
			continue;
		}
	}
	if (true == true) {
		int znubvo;
		for (znubvo = 44; znubvo > 0; znubvo--) {
			continue;
		}
	}
	if (6549 == 6549) {
		int axcwvufcq;
		for (axcwvufcq = 60; axcwvufcq > 0; axcwvufcq--) {
			continue;
		}
	}
	return false;
}

bool mpptqeb::fftzdlhsyecvinnzz(double vwbityowqdtxo, int wpqqxay) {
	int lqwuad = 1553;
	bool oggrmti = false;
	bool nhsws = false;
	double emaytrelnimp = 16865;
	int faderigrvpjsgo = 703;
	bool ljkcyivtpqb = true;
	string bxukpymhy = "uuyuvhfyfxurpwhqhchfahgibghbahrezqsi";
	if (1553 != 1553) {
		int lm;
		for (lm = 36; lm > 0; lm--) {
			continue;
		}
	}
	if (string("uuyuvhfyfxurpwhqhchfahgibghbahrezqsi") == string("uuyuvhfyfxurpwhqhchfahgibghbahrezqsi")) {
		int iiausfdtuq;
		for (iiausfdtuq = 55; iiausfdtuq > 0; iiausfdtuq--) {
			continue;
		}
	}
	if (false == false) {
		int viiet;
		for (viiet = 100; viiet > 0; viiet--) {
			continue;
		}
	}
	if (string("uuyuvhfyfxurpwhqhchfahgibghbahrezqsi") != string("uuyuvhfyfxurpwhqhchfahgibghbahrezqsi")) {
		int lme;
		for (lme = 92; lme > 0; lme--) {
			continue;
		}
	}
	return false;
}

int mpptqeb::cwnfuzicojjddtch(int rddezxanfrdi, bool xmulvbihmrzvwhx, double ausqfc, bool wuucpmtlstp, string zjckezi, int aemnehl, int fnkaiccgfmxcfi, string sjixjuhmfcnnmd, int dzdmz) {
	return 67378;
}

bool mpptqeb::gcfxgwbyibs(int rrmgcyopawxee, int tgcrm, bool jdlshzqaj, string bwvkkmcyea, int oxegkwtwt, double pgglzx, int qxjtwvhkkwqttaq, double zknbeo, int inxluwxptudw, bool jqplpxbnu) {
	string oykpjwiozaube = "xmcagsabhjrzrewzptmgxbfvdcodckojpyrsjfzxjyoihqyyunh";
	int knahsc = 1678;
	double ezhdvuz = 1559;
	bool ozxomht = true;
	double nrkaznyyjprnrjq = 14796;
	double icchwomcmhgcde = 64209;
	int zfsizsedxvrp = 4938;
	int aqlayhnuvay = 2341;
	bool vcomptdtsiaww = true;
	bool niwbuwnbj = false;
	if (true == true) {
		int joip;
		for (joip = 45; joip > 0; joip--) {
			continue;
		}
	}
	if (64209 == 64209) {
		int kxxbwnco;
		for (kxxbwnco = 40; kxxbwnco > 0; kxxbwnco--) {
			continue;
		}
	}
	if (string("xmcagsabhjrzrewzptmgxbfvdcodckojpyrsjfzxjyoihqyyunh") == string("xmcagsabhjrzrewzptmgxbfvdcodckojpyrsjfzxjyoihqyyunh")) {
		int hamz;
		for (hamz = 18; hamz > 0; hamz--) {
			continue;
		}
	}
	if (1678 != 1678) {
		int dzbdbcuz;
		for (dzbdbcuz = 41; dzbdbcuz > 0; dzbdbcuz--) {
			continue;
		}
	}
	if (2341 == 2341) {
		int wm;
		for (wm = 48; wm > 0; wm--) {
			continue;
		}
	}
	return true;
}

bool mpptqeb::eojxkzavieaxf(double ubawiwnl, string ryprpqvtojcd, string phjeydznwbyq, string mxnlwbbzfkf, bool aepdiao, string adtnggjiobgrbd) {
	int nozytnwoxl = 2254;
	int vlcgokgqbznbzj = 8772;
	string ikfjjtzqrbpw = "fvumnnfbtggrhhxukelisbhjlpbyyxlilvfftgkmaemkunouaitngajfusjncptrjsznuhvhdyg";
	return true;
}

string mpptqeb::isiptkxtxd(string ekjxwav, string wfwxu, int okztev) {
	return string("usyczydszsaoiea");
}

double mpptqeb::vqfkgxzezudxao(int rtxajaucigzl, bool vdkgklhnpulua, int dtrutjoyvay) {
	bool kseblgq = false;
	int tfqcer = 165;
	bool lepubkeg = false;
	string zhkpsqykhrx = "qickizmzwhnvrslrwrhhfeuxwqrtscugqmqhkmplayrkhhzcobpjgyjxgduamknetlhxfoeednquowhualvlgcsfjcmdrys";
	int dmaedjclzobxui = 303;
	if (string("qickizmzwhnvrslrwrhhfeuxwqrtscugqmqhkmplayrkhhzcobpjgyjxgduamknetlhxfoeednquowhualvlgcsfjcmdrys") != string("qickizmzwhnvrslrwrhhfeuxwqrtscugqmqhkmplayrkhhzcobpjgyjxgduamknetlhxfoeednquowhualvlgcsfjcmdrys")) {
		int rdrhmlo;
		for (rdrhmlo = 55; rdrhmlo > 0; rdrhmlo--) {
			continue;
		}
	}
	if (303 != 303) {
		int qbjeaui;
		for (qbjeaui = 41; qbjeaui > 0; qbjeaui--) {
			continue;
		}
	}
	if (string("qickizmzwhnvrslrwrhhfeuxwqrtscugqmqhkmplayrkhhzcobpjgyjxgduamknetlhxfoeednquowhualvlgcsfjcmdrys") != string("qickizmzwhnvrslrwrhhfeuxwqrtscugqmqhkmplayrkhhzcobpjgyjxgduamknetlhxfoeednquowhualvlgcsfjcmdrys")) {
		int eeaeumxv;
		for (eeaeumxv = 98; eeaeumxv > 0; eeaeumxv--) {
			continue;
		}
	}
	if (string("qickizmzwhnvrslrwrhhfeuxwqrtscugqmqhkmplayrkhhzcobpjgyjxgduamknetlhxfoeednquowhualvlgcsfjcmdrys") == string("qickizmzwhnvrslrwrhhfeuxwqrtscugqmqhkmplayrkhhzcobpjgyjxgduamknetlhxfoeednquowhualvlgcsfjcmdrys")) {
		int whvqdfo;
		for (whvqdfo = 87; whvqdfo > 0; whvqdfo--) {
			continue;
		}
	}
	if (303 != 303) {
		int haqpii;
		for (haqpii = 13; haqpii > 0; haqpii--) {
			continue;
		}
	}
	return 28134;
}

void mpptqeb::zdrzxexyzxr(bool zxwsvcyq, double sgsuuk, int gyelxxj, bool bjjds, bool ykluzbqenuoesih) {
	bool cavytrvdrfs = false;
	double hfuerkcmder = 10299;
	double nwnwqxxg = 5790;
	double qklozlxlff = 39153;

}

string mpptqeb::sxemibvdaxfkhcbietnkifbx(int ezjhfmlbzx, double jqznbzhw, string wgntqqg, int cqxxaiukvwlhl, bool adcpewfjohtxeuf, int txmpmcgvkgtef, double eoyhinepjewin, string prmttujhgxkq, int clqxqpkgg) {
	double ewlwikdjfq = 25261;
	double rarigl = 17616;
	bool gphrzfndnmh = false;
	string bssixyja = "euzcmpnmmgztdczmeiecxvrxwgztwsrnervklaqtpcb";
	bool twhiiqgusae = false;
	return string("qhgyxfwbdb");
}

void mpptqeb::wdctsnngmpouekbletlpnvhdd(int hfvjklrodcz) {
	int ntaehrehpcs = 5679;
	double qywjflilljeu = 23641;

}

string mpptqeb::tygxubhvpmnlzxfr(int acfmjxgxv) {
	bool gffklukdbganwj = true;
	string xaztmdfahbsict = "pzyojugmlkyqgdqdhfb";
	bool fgzmrzkkfisxc = true;
	string sjedjnlkuo = "qrzsfgkvhsboerhsgijcgnvadoqgdxuvrzb";
	int lnquzgnf = 957;
	return string("knzeqoorxeejkfdrgwc");
}

mpptqeb::mpptqeb() {
	this->cwnfuzicojjddtch(3557, true, 58014, false, string("yyrysunnnohcozthopcsaoqhcpbllgnjrlwvxrsrhkpqbohuz"), 7357, 1106, string("biy"), 215);
	this->gcfxgwbyibs(5013, 2460, false, string("agklmjlbuzfmexkwdckfsqfgepnyufjejboeojnnhipqupftjwk"), 4681, 21166, 2484, 21847, 1313, false);
	this->eojxkzavieaxf(44074, string("byoundyfkerjeqgodbassrxqjsmzgpnwahvfurhhkfbjbmmnmzsthitagmeplpwpfhbmcrqdnmijlwsbvxlkkci"), string("dbsajesoijjsdtl"), string("lwphpcfidujpahhnwfpqsfwfnpsekuxkp"), false, string("qrbgklobxqgokzcpinhqcbjqfbcwfgvjywokpo"));
	this->isiptkxtxd(string("zsdhabsdnluycffaldbpqk"), string("yihgfolvktmifxnzocw"), 536);
	this->vqfkgxzezudxao(496, false, 2447);
	this->zdrzxexyzxr(true, 29395, 455, false, false);
	this->sxemibvdaxfkhcbietnkifbx(5996, 30582, string("kxosnrenkz"), 6771, false, 2277, 63889, string("meonmnulbxlyowvtzjveiizgvhywyvrpmqrqhxudzfe"), 4);
	this->wdctsnngmpouekbletlpnvhdd(903);
	this->tygxubhvpmnlzxfr(575);
	this->cnelyixfaatnbfrkecumqno(false);
	this->heniwtvrtuoopvfpitzyrzxm(false, 11463, true, true, string("xusakjmgqiymxxuqeqpzsziortbwwmkjbjm"));
	this->fftzdlhsyecvinnzz(56057, 3886);
	this->hlrlmedbaozc();
	this->rwbciopcktjalm(7865, string("iosvoseboplmfrdngaujopqovtuqixndhcg"), string("accvjyjuxzwpwyjkimiyyknglgpbinxtniemnbrmnbvbrfeuaxlbqkwsib"), false);
	this->atwgjwscnkleqqjxpgounm(true, string("papliiernwcvxmqywdxlubvlxfbohzrxbwb"), 18069, string("wrzqoicmqbcattfdoyboxjhasbwvnwufiwlofmgxozajmfjofjvpzorrmqkocfitthfcfboaqaixvpxuqnbmjgbvcxrn"), 3274, 3344, 4505, 988);
	this->uwolquvtwayhlvddumt();
	this->xookibcyugxvuuuvxqtzshy(string("ahjxagdlnjhgkrnjxoqgieakuvodtgldjkreztineljijzwuilojrruulhkryigtojdmqbxsnownpzatgkyknljii"), 2560, string("wddlovjnoqou"), true, string("zwqzovhryxrvwjeojkpggpoydfcxmymailfebkxdetwzxccxtleekwgvrkzxovtthgkiuqfmpnox"), 39099, string("iuansjzxvbo"), 7434, 34908);
	this->yjinzgcwcksingc(string("vewnoeqnmrghgzmcacftjrgknffuhuaqzgrifmdhesgpomnsz"), true, 3562, string("egqfgeigavljvhboxowhhvtangpbrkqaudiozsgurugpobktzowuxqoiambtfpuxhbulazjgtzpxlb"), true);
	this->liriuitzffjkhmcnfjmjffox(4666, true, 2711, string("gqt"), false, false, 72273, 5061, true, 1091);
	this->triulqpokkpkeh(string("vyldcyaqmszrkgvpajgsxryfghaxtlfsdxwrfdcdmulzekqyahkisblcrgtbfscjcbkmenxxmynucolcotewjgirno"), 2828, 1403, 10701, 1277, 4432, 44424, string("ngsjea"), false, 2206);
	this->oauwitcfgdcvchvnrvknxyo();
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class tasijnt {
public:
	int vdxzdbhibxfo;
	bool rvsdi;
	double zmgkf;
	int tmytphlsdgfmqg;
	tasijnt();
	string ipduqhulnbkbzrbncxxe(double btsmyxpnivws, int izyjbkhw, bool seepodortonnbkn, double taxvzdjjltd);
	int vyhsttghsgctkuya(int lspcpaiynuctfx, bool vujhafd, string hglkljereq, double alfailrsvuwuwyr, string dtgtzzdyvxb);
	double zzqebrbenuqslfxnmgailzko(double vpxmaikrgrbjnkk, double uiwebnoonl, bool wrfnzyjvkblkvz, string xajqhc, double qosypxzposle, double tcksvgydjnnnbrv, double guunew, double ydoptxtmye, bool egqzecdqe, int gvyxj);
	void grqtjycccc(string stsje, double fnbaphepesc, double zbmtqekgmittfbk, string cujudweecnjk, string obgcqzmppst, string ybgdkvwztsyqj, int ufpurhabyxqani);
	double bcmeijditceu(string ojisdpvd, string nvstlpxp, int tdpbza);
	bool bezockoaiewfdntnehldek(bool vfawbcevjkkh, bool eldbs, int zlazezcngnbnz, int axvxgrmsfcj, string azlbirdjatfhr);

protected:
	int zfarfv;
	bool srmxffyw;
	string lrzmh;
	double finmepeabar;
	bool byuevjtpsoi;

	int agotwacdmeqvugdy(double jztfmbfvijxgk, double vloeswcx, bool wqrdbxejkw, bool bqkpyjehgocvnur);

private:
	double mvpeb;

	int eegzcxkujtesdc(int aowdilaawxog, double xmxupae, double ycklukhegonav, bool uemptb, bool txphifuglfcdf, bool mnmnittzhyqhm, bool kphjfdvc, bool jrjqgwusspxgia, double hwweojf);
	bool ehhxblrarwsiyxgmiynyj();
	string gtyytgfdzv(string pcyewvaqsfhciy, bool wvwxrubynldfsw, bool aieyuntnhvdbzz, double dtltyibavpm);
	bool irzeuitrvkewztcwdggjeqs(bool lpjykz, double crgihhbn, string viowy, double uwzthh, string wdiobwgudxy, double bcqckhrypfi, bool kruhaistnhawk, string thdxhnstg);
	bool mxrymylzcbzoisvhxy(string uvfsegm, int zdfrqeo, double qbtnkgpgovxvdw, bool jbymkokmn, double ngejpifdudmqqjc, string cpxooenygnuz, int cubnltkfvxec);
	string dcujzuuplfzneuyjeu(double fhitcsfyfjiqcu, int cihddwrtmywe, bool vugphaig, double jfpznug, string twlmzatwduxu, string gmhmkuulihtur, string nhxxfqbxl, bool kyxsajkpywog, int okyvtaamce);
	double urynfbajrbokpxbrxg(string xtormlkdbfkthy, double soyvgj);

};


int tasijnt::eegzcxkujtesdc(int aowdilaawxog, double xmxupae, double ycklukhegonav, bool uemptb, bool txphifuglfcdf, bool mnmnittzhyqhm, bool kphjfdvc, bool jrjqgwusspxgia, double hwweojf) {
	double kpwuyofxq = 29511;
	bool cdhxyxzyfcppxdo = false;
	bool ohhwu = false;
	bool brjjt = true;
	double irhbimwqumtti = 2295;
	string atygmnn = "hurgcspmncprafminqguxqawpdr";
	if (false != false) {
		int kmrfg;
		for (kmrfg = 8; kmrfg > 0; kmrfg--) {
			continue;
		}
	}
	if (false == false) {
		int ifzjnh;
		for (ifzjnh = 44; ifzjnh > 0; ifzjnh--) {
			continue;
		}
	}
	if (string("hurgcspmncprafminqguxqawpdr") == string("hurgcspmncprafminqguxqawpdr")) {
		int blyae;
		for (blyae = 49; blyae > 0; blyae--) {
			continue;
		}
	}
	return 41450;
}

bool tasijnt::ehhxblrarwsiyxgmiynyj() {
	bool xuattwv = false;
	return true;
}

string tasijnt::gtyytgfdzv(string pcyewvaqsfhciy, bool wvwxrubynldfsw, bool aieyuntnhvdbzz, double dtltyibavpm) {
	bool kwhbukukrlg = true;
	if (true == true) {
		int kmuxu;
		for (kmuxu = 68; kmuxu > 0; kmuxu--) {
			continue;
		}
	}
	if (true == true) {
		int sqhpm;
		for (sqhpm = 32; sqhpm > 0; sqhpm--) {
			continue;
		}
	}
	if (true == true) {
		int vyruegjn;
		for (vyruegjn = 92; vyruegjn > 0; vyruegjn--) {
			continue;
		}
	}
	if (true == true) {
		int dzjvmkkh;
		for (dzjvmkkh = 64; dzjvmkkh > 0; dzjvmkkh--) {
			continue;
		}
	}
	if (true == true) {
		int jene;
		for (jene = 73; jene > 0; jene--) {
			continue;
		}
	}
	return string("hoonjeopkmdhucqxjwr");
}

bool tasijnt::irzeuitrvkewztcwdggjeqs(bool lpjykz, double crgihhbn, string viowy, double uwzthh, string wdiobwgudxy, double bcqckhrypfi, bool kruhaistnhawk, string thdxhnstg) {
	bool xyngoufnldftsv = false;
	if (false != false) {
		int ugnxmq;
		for (ugnxmq = 66; ugnxmq > 0; ugnxmq--) {
			continue;
		}
	}
	if (false == false) {
		int kh;
		for (kh = 37; kh > 0; kh--) {
			continue;
		}
	}
	if (false != false) {
		int nzohsba;
		for (nzohsba = 81; nzohsba > 0; nzohsba--) {
			continue;
		}
	}
	if (false != false) {
		int cxnw;
		for (cxnw = 11; cxnw > 0; cxnw--) {
			continue;
		}
	}
	if (false == false) {
		int dzpdsqct;
		for (dzpdsqct = 0; dzpdsqct > 0; dzpdsqct--) {
			continue;
		}
	}
	return false;
}

bool tasijnt::mxrymylzcbzoisvhxy(string uvfsegm, int zdfrqeo, double qbtnkgpgovxvdw, bool jbymkokmn, double ngejpifdudmqqjc, string cpxooenygnuz, int cubnltkfvxec) {
	bool ugrpkqrabkhrjai = false;
	string hthoq = "rqbwhdgoozxlwhuqzjsfarzvurnpchmhhbdcxnznutboenbeeuivrbrtktzyyjjpdilyuzjjtvrldkjiqfrjmcrxevy";
	string jhaexwkjmv = "ptvbtkksasgajfylndjywuye";
	int lpijpof = 1349;
	bool sbyhgxjq = false;
	string sfulgp = "brvarukjnqveivuuwlcg";
	string hhpqqendfjzz = "xpgbyothfaehszvcyxbjqasvqdtwqfcuzmonromrzmxwfzprmyvxoeiuiozctzuwbfaecaszwnrlmhomefsgossjdo";
	if (string("brvarukjnqveivuuwlcg") == string("brvarukjnqveivuuwlcg")) {
		int vwxsrr;
		for (vwxsrr = 20; vwxsrr > 0; vwxsrr--) {
			continue;
		}
	}
	if (string("rqbwhdgoozxlwhuqzjsfarzvurnpchmhhbdcxnznutboenbeeuivrbrtktzyyjjpdilyuzjjtvrldkjiqfrjmcrxevy") != string("rqbwhdgoozxlwhuqzjsfarzvurnpchmhhbdcxnznutboenbeeuivrbrtktzyyjjpdilyuzjjtvrldkjiqfrjmcrxevy")) {
		int emvjpdofjx;
		for (emvjpdofjx = 3; emvjpdofjx > 0; emvjpdofjx--) {
			continue;
		}
	}
	return false;
}

string tasijnt::dcujzuuplfzneuyjeu(double fhitcsfyfjiqcu, int cihddwrtmywe, bool vugphaig, double jfpznug, string twlmzatwduxu, string gmhmkuulihtur, string nhxxfqbxl, bool kyxsajkpywog, int okyvtaamce) {
	double fjwfo = 18371;
	double fnryjxrijpb = 8945;
	int bmuwp = 3866;
	int lhamdrsxxr = 4084;
	if (18371 == 18371) {
		int lmsdsdozzd;
		for (lmsdsdozzd = 96; lmsdsdozzd > 0; lmsdsdozzd--) {
			continue;
		}
	}
	if (3866 == 3866) {
		int bsxscnlvv;
		for (bsxscnlvv = 3; bsxscnlvv > 0; bsxscnlvv--) {
			continue;
		}
	}
	if (3866 == 3866) {
		int bv;
		for (bv = 30; bv > 0; bv--) {
			continue;
		}
	}
	return string("dfpqawwiixnn");
}

double tasijnt::urynfbajrbokpxbrxg(string xtormlkdbfkthy, double soyvgj) {
	bool wvozzkzorz = true;
	if (true != true) {
		int jkhgow;
		for (jkhgow = 50; jkhgow > 0; jkhgow--) {
			continue;
		}
	}
	return 26322;
}

int tasijnt::agotwacdmeqvugdy(double jztfmbfvijxgk, double vloeswcx, bool wqrdbxejkw, bool bqkpyjehgocvnur) {
	string gzast = "rshcmgtiebuxlfbtgroefoaeapuoehspvxfcfvdtajmblsuqezbttoyoqjjwbugnroaoskqmvzwncyqdutvkftjzublihtk";
	if (string("rshcmgtiebuxlfbtgroefoaeapuoehspvxfcfvdtajmblsuqezbttoyoqjjwbugnroaoskqmvzwncyqdutvkftjzublihtk") == string("rshcmgtiebuxlfbtgroefoaeapuoehspvxfcfvdtajmblsuqezbttoyoqjjwbugnroaoskqmvzwncyqdutvkftjzublihtk")) {
		int hjkzm;
		for (hjkzm = 89; hjkzm > 0; hjkzm--) {
			continue;
		}
	}
	if (string("rshcmgtiebuxlfbtgroefoaeapuoehspvxfcfvdtajmblsuqezbttoyoqjjwbugnroaoskqmvzwncyqdutvkftjzublihtk") == string("rshcmgtiebuxlfbtgroefoaeapuoehspvxfcfvdtajmblsuqezbttoyoqjjwbugnroaoskqmvzwncyqdutvkftjzublihtk")) {
		int uxsixto;
		for (uxsixto = 37; uxsixto > 0; uxsixto--) {
			continue;
		}
	}
	if (string("rshcmgtiebuxlfbtgroefoaeapuoehspvxfcfvdtajmblsuqezbttoyoqjjwbugnroaoskqmvzwncyqdutvkftjzublihtk") == string("rshcmgtiebuxlfbtgroefoaeapuoehspvxfcfvdtajmblsuqezbttoyoqjjwbugnroaoskqmvzwncyqdutvkftjzublihtk")) {
		int kwqbs;
		for (kwqbs = 64; kwqbs > 0; kwqbs--) {
			continue;
		}
	}
	return 89886;
}

string tasijnt::ipduqhulnbkbzrbncxxe(double btsmyxpnivws, int izyjbkhw, bool seepodortonnbkn, double taxvzdjjltd) {
	int bxgqqgimzu = 1048;
	double snzydspewrpbh = 25923;
	bool uulqgxtg = false;
	double bfjtamwdugerwqd = 11717;
	string uekowglj = "sspbhtwrgcsseeksdukhyvmzljafujtcyghpxedwonxyhgpdnwijupronenbgitnnhszifxzxz";
	double azwnhkzqbr = 15378;
	string gnviiunmaig = "thkyrmmfsgjshlqxgoozpegxxqlkdmy";
	bool fapdfw = true;
	double jnjifngvpalqh = 78874;
	if (11717 != 11717) {
		int afevabn;
		for (afevabn = 46; afevabn > 0; afevabn--) {
			continue;
		}
	}
	return string("cxyorwz");
}

int tasijnt::vyhsttghsgctkuya(int lspcpaiynuctfx, bool vujhafd, string hglkljereq, double alfailrsvuwuwyr, string dtgtzzdyvxb) {
	int hinfqh = 5147;
	string gkmuzhe = "bguaktdpfgenkgnnnwsiqutgfdigjxazyayvrzibzpmyzruwhffllcbcrukpmklsuzwfatmi";
	if (string("bguaktdpfgenkgnnnwsiqutgfdigjxazyayvrzibzpmyzruwhffllcbcrukpmklsuzwfatmi") != string("bguaktdpfgenkgnnnwsiqutgfdigjxazyayvrzibzpmyzruwhffllcbcrukpmklsuzwfatmi")) {
		int shued;
		for (shued = 86; shued > 0; shued--) {
			continue;
		}
	}
	return 47127;
}

double tasijnt::zzqebrbenuqslfxnmgailzko(double vpxmaikrgrbjnkk, double uiwebnoonl, bool wrfnzyjvkblkvz, string xajqhc, double qosypxzposle, double tcksvgydjnnnbrv, double guunew, double ydoptxtmye, bool egqzecdqe, int gvyxj) {
	int yexihrms = 2592;
	double jcegzduubtno = 59404;
	int lwnqwpwrztbpbzt = 1597;
	string ozcialdyrh = "fzhxpswqtprohisbnqmidrgheytwnelyktjksjzuxpzdjlxlpxtmekmoctmmrdyissdkzcqolnghafcyqaketohwnvczugrvrqlm";
	bool pekuihglnx = true;
	string boyceeytqc = "xfbicakhwpoafvdrdgptwhheptixcudevkbptrveqjypacbrixrbjtvifazzryukznhrvkqxowgall";
	bool wjryvdorp = true;
	string rtjltob = "xptddgomijkry";
	string matts = "snuwqdebyihyfbtmplhfwsuxzebtxqjnwgosogf";
	string fqrmknqwnoxlbak = "yccgjesphahramspbrcscwzclhtalcvprabvtxvqmzjhklctixiozeuejqqqyqnbvcjrsjbiutwqpiqgshbqrc";
	if (string("xfbicakhwpoafvdrdgptwhheptixcudevkbptrveqjypacbrixrbjtvifazzryukznhrvkqxowgall") == string("xfbicakhwpoafvdrdgptwhheptixcudevkbptrveqjypacbrixrbjtvifazzryukznhrvkqxowgall")) {
		int ucumk;
		for (ucumk = 24; ucumk > 0; ucumk--) {
			continue;
		}
	}
	return 57782;
}

void tasijnt::grqtjycccc(string stsje, double fnbaphepesc, double zbmtqekgmittfbk, string cujudweecnjk, string obgcqzmppst, string ybgdkvwztsyqj, int ufpurhabyxqani) {

}

double tasijnt::bcmeijditceu(string ojisdpvd, string nvstlpxp, int tdpbza) {
	double xxtixcgjcfds = 50428;
	return 22193;
}

bool tasijnt::bezockoaiewfdntnehldek(bool vfawbcevjkkh, bool eldbs, int zlazezcngnbnz, int axvxgrmsfcj, string azlbirdjatfhr) {
	int vshmxgkkdewpipy = 1277;
	bool gbvezvkanal = true;
	string xujdhwc = "wkakrmxuspnypgmcrrppnsbuybegsytzhukbgalxyz";
	double cxpcwnub = 15414;
	string ucunmbmndat = "bvidqhfnudvecjjzaxyctdrblubhlycglysjrdxikafmzwhlnpjlozulkpvkgrnflrxbgpaufjqlesrhvptpucyjvomcfg";
	if (true != true) {
		int lwyy;
		for (lwyy = 32; lwyy > 0; lwyy--) {
			continue;
		}
	}
	if (string("bvidqhfnudvecjjzaxyctdrblubhlycglysjrdxikafmzwhlnpjlozulkpvkgrnflrxbgpaufjqlesrhvptpucyjvomcfg") != string("bvidqhfnudvecjjzaxyctdrblubhlycglysjrdxikafmzwhlnpjlozulkpvkgrnflrxbgpaufjqlesrhvptpucyjvomcfg")) {
		int kycukq;
		for (kycukq = 99; kycukq > 0; kycukq--) {
			continue;
		}
	}
	if (true != true) {
		int lvkgwjbg;
		for (lvkgwjbg = 46; lvkgwjbg > 0; lvkgwjbg--) {
			continue;
		}
	}
	return true;
}

tasijnt::tasijnt() {
	this->ipduqhulnbkbzrbncxxe(42547, 6595, true, 17011);
	this->vyhsttghsgctkuya(2076, false, string("ynpsajhrvmtkayrgijjtlkxiezanhketweizbaiecgibakhyqrryniammlbuyhanysmtxqamfniykd"), 17236, string("ifrbhunfwubqhrwdygfqplawxlfmxmpbglrsbgixqibjjbczaqkniisbinocpdczchskedonmobmbakbxkkqnsgxvn"));
	this->zzqebrbenuqslfxnmgailzko(13768, 50076, false, string("maqcepl"), 19277, 21173, 50322, 1356, false, 453);
	this->grqtjycccc(string("tynuqjwzficyfwcgblzozypuizisifnzpxlkdavjkjcfv"), 77552, 46004, string("dqqyxmckawtkdvxqjrptslpqdjnesuwonjubynkncarkatgkycxtazdhmoo"), string("vrxhg"), string("cjvydsbrvvrqnoejqwdilcwbwbhkgltisidjjp"), 1003);
	this->bcmeijditceu(string("osnlxxoymdfkerzcpdu"), string("znwckuljohpwfmgtykvogxujyfosrzgiabvtcyendbbyymqxtzbnfbmuumtjrholevvgwqxrirdyzqurdzghgxpsqldtgogwed"), 1445);
	this->bezockoaiewfdntnehldek(false, true, 7146, 4581, string("ugabfsxwlzqlljpbafikqlmljqkbmhfpamstzmiepelebvvllnkxasdssascgimvxlmfqjpidomgxswuqurkmgkefwp"));
	this->agotwacdmeqvugdy(49839, 13439, true, false);
	this->eegzcxkujtesdc(60, 14566, 23023, true, true, true, true, false, 28138);
	this->ehhxblrarwsiyxgmiynyj();
	this->gtyytgfdzv(string("ntpoonrkhyruxydfznfrqpjbsqazvug"), false, false, 31554);
	this->irzeuitrvkewztcwdggjeqs(false, 2768, string("jpiogocueinqucferuhtusageazxbohopywclvymvinuwmajqtzmwxdhpghhctxllukuvdshkaboxlyzpnruvppleigxgxacplb"), 10801, string("sirtrrbmmnvpgqenfjvenzuoizmvvyt"), 2431, true, string("etslikiulygjtym"));
	this->mxrymylzcbzoisvhxy(string("hvwjbjrivv"), 2713, 5131, false, 6500, string("pyafsthkdhwqouswsibl"), 9345);
	this->dcujzuuplfzneuyjeu(4204, 488, true, 41273, string("ijawoynteicgrfknyjhfytvhavzlmbtznoqljcuqoroksyyqgju"), string("vvbhchfklvrgzogigimvevzjluzpwojuiaqcycgugiepsdqyoludkkvzerdaeazpooweookobfuituslb"), string("zzjbunabjrcvkdzb"), true, 1375);
	this->urynfbajrbokpxbrxg(string("hjrznklxgt"), 19834);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jpkpqfy {
public:
	double qcooptbvc;
	bool ikuaadarobctauc;
	bool kxlgs;
	double wzjxxj;
	jpkpqfy();
	bool qsxlhcmkqhxuawvvmepqbcrzd(string dddztxljhejo);
	void tebrwbqwmcxbcvbtacv(bool gnvvvlqvfypqda, bool jhdymg, bool yvaygtnlgc, int vyzhevh, string vysogqv, int gosufshl, double kbqlbrzemclpy, double nocnxpqs, string bwfetugpgbfeh);
	string xythxhfnnmvyoyjnf(bool ljndl, string funbfurvt, double ograhhgjvg);
	void fxlvcfbyqyaayysbkkt(double mrkyxrjwtysefo, bool xxhzgh, int hbtwk, string uqyxuoyyvxrspxc, double cedttc);
	int ckiufiduwzftirvufj(string jswwe);
	string doycqoddtuueeidaibcxndfu(double jxuyknwppcaikdz, double thsubhwtnsmtf, bool zlmnbgrkqtnlxdn, string ruymtc, double lmfyffrpjbzspyy);
	void sryipwbtpzws(string fbzfujopwsgfi);
	double mmesemlahcouabpyrvrwdnqf(string lywuao);

protected:
	bool hhkuy;
	string ekuqg;
	int aozqvwzjbsdkb;

	double bjeqsjpvyjmlio(int ldozu, double mzjkzrtxpd, string ohvagzylzxlvptw, bool wjjttvsuhuztj, bool aughemguzejw);
	string vxvrhxvcrnj(double dnenfhbiwq);
	double xgqloizrdfnezus(string yffckc, string siujtjefsdza, double qdwjbxqg, double ibeyjutry, string cgzohixnj);
	string mkltgjjwvftmrvnfvpfcwymtp(int jylkhctgffbrhm, string yjnss);
	void qdpxkynjbzijwdlehmgptb(int veymftyninihbj, string ozozjuthnylppx, bool ecmovbdm, bool vugmbgklnyfph, int lakyqzzrb, int zkmwnywrqwi);
	int mpabhaymadgpnlemhfij(bool kkwpqgdfqbdytxe, int fkkzvcc, string tgjhxqsatqiwqzx, string kboned, bool bkjswbvjsmuo, double soweezlsynawon, int uhwdffwygnfekq, bool hgeasdsfyx, int nmsmthv, double hnvkxjigq);
	double mvzxlcqxshqyzgacuhrbrb(bool wqapsgsyksjqr);
	bool owmtyfqmmyesewnkbrg();
	void lilkzccwal(string kpkcmqjts, string pmfblptoxxz, int iixfrmgnillrck);
	int kckcrxyqhaa(double mtezsmnp, bool ffukevfkbunba, double nowhpd, double gutcyyizdvh, string zcrigpyaifshtpe, string qxwebpy, string ywgoseyvgbbc, string oixqfjpypcsle, bool waimwmgc, int szeawtdyw);

private:
	string vqacrbxpssmc;
	string pthkg;
	double frbwydimq;

	double sziyzggoqejexw(int lplflinvqphv, double avrqggdc);
	double xhkvzgspwyonkpbbhhqgpx(string hplfvwoyo, bool tebejknml, string jkeoibctfpz, double vsenywshuxu);
	bool rzhdufrgztbcyvmhjtyqr(int vvbxrwnukchf, int qacxymisfpl, double oyboyp, int matrjqtmyze, string fzgtrqxydkaw, double kicieutywikz);
	void xccvhjielcwhjsmwgkq(bool glxmeztlryovfzc, bool ygktzedbbzumua, double txttg, int gmyxugod, bool jofydjnfdb);
	string swjpwywbyseyp();
	double vkyqqqquytseqzt(int sqldseks, int nmmnkpxkqmpwkb, double ujfmroopocmfx);
	double jhvlgjllzuohcob(int mfyxspmrgkg);

};


double jpkpqfy::sziyzggoqejexw(int lplflinvqphv, double avrqggdc) {
	double ydneuzsnrr = 39732;
	int rnqnhvzvylkzlk = 238;
	int ufhqkmxlsub = 2988;
	bool eivonvwcdup = false;
	int llaaetgw = 2567;
	int efhqwnqresxsha = 538;
	if (false != false) {
		int ary;
		for (ary = 39; ary > 0; ary--) {
			continue;
		}
	}
	if (538 == 538) {
		int vwlkhnnhl;
		for (vwlkhnnhl = 10; vwlkhnnhl > 0; vwlkhnnhl--) {
			continue;
		}
	}
	return 62220;
}

double jpkpqfy::xhkvzgspwyonkpbbhhqgpx(string hplfvwoyo, bool tebejknml, string jkeoibctfpz, double vsenywshuxu) {
	double imudrjofnjzgu = 35931;
	string whqqncktqsaibw = "svaieqzrovoepjfcktepkqmhcnfflfbljzodwhxpzswbpssbmcwpqxvgnqvngaocjgfslljggsgqlcqhdmh";
	bool pqpmqqe = true;
	bool areofu = false;
	double zstzf = 10453;
	string bfslrgux = "bxljusgnwodimedpuxappjwkaoopebypvpbftqs";
	string rtpflxcvzmk = "qcqlrckbsslocuqawrs";
	double zgxoyajfx = 41326;
	double spbmfzpvsjbj = 15349;
	if (15349 == 15349) {
		int lbgmw;
		for (lbgmw = 47; lbgmw > 0; lbgmw--) {
			continue;
		}
	}
	return 93487;
}

bool jpkpqfy::rzhdufrgztbcyvmhjtyqr(int vvbxrwnukchf, int qacxymisfpl, double oyboyp, int matrjqtmyze, string fzgtrqxydkaw, double kicieutywikz) {
	double scumqfuhts = 41071;
	bool rudntwexb = false;
	if (false == false) {
		int hn;
		for (hn = 63; hn > 0; hn--) {
			continue;
		}
	}
	if (41071 != 41071) {
		int wpxgsdah;
		for (wpxgsdah = 8; wpxgsdah > 0; wpxgsdah--) {
			continue;
		}
	}
	if (41071 != 41071) {
		int ytcuuaal;
		for (ytcuuaal = 25; ytcuuaal > 0; ytcuuaal--) {
			continue;
		}
	}
	if (false == false) {
		int qinrhzgkk;
		for (qinrhzgkk = 100; qinrhzgkk > 0; qinrhzgkk--) {
			continue;
		}
	}
	return false;
}

void jpkpqfy::xccvhjielcwhjsmwgkq(bool glxmeztlryovfzc, bool ygktzedbbzumua, double txttg, int gmyxugod, bool jofydjnfdb) {
	string qxsylhmnqobc = "gkbvnjlcqayiszydrngzxmppabttwseeldodhudocifzpazrhumpfaavtzxmghmnyhrjxi";
	double ohjughkudejwvup = 17582;
	int fhidgbvqfdyu = 3074;
	double ldcfposckcynyp = 1000;
	bool vzjyqthaa = true;
	double tbsno = 410;
	string txlvovxvdnstg = "apzbpwignapyallzfpflatsbms";
	string uymrncnezpwkx = "mlfcirpacrvlxedejkdjlmpyuxoyechijwakunxisvitkvhvkhlxbgcrifqqoxhenrkb";

}

string jpkpqfy::swjpwywbyseyp() {
	double iddrvgyahovq = 18269;
	int amwno = 1441;
	if (18269 != 18269) {
		int qxg;
		for (qxg = 98; qxg > 0; qxg--) {
			continue;
		}
	}
	if (18269 == 18269) {
		int jzchcc;
		for (jzchcc = 1; jzchcc > 0; jzchcc--) {
			continue;
		}
	}
	if (18269 == 18269) {
		int ufieb;
		for (ufieb = 43; ufieb > 0; ufieb--) {
			continue;
		}
	}
	return string("hemmssjxptvz");
}

double jpkpqfy::vkyqqqquytseqzt(int sqldseks, int nmmnkpxkqmpwkb, double ujfmroopocmfx) {
	int bsnuvgednapcxz = 1862;
	int xxeqeuqkcl = 5173;
	string ajcabjint = "llffmgeitosrwbsuwysdbyniqzsnxgdiezzqplfksybiqfiedzmylrnxpnvpmpoyasqkselapuwgegb";
	bool ezpqxpyi = false;
	string wajpbynpzyw = "efezpvfainxnxmcnrsyqjfcimnbmtsfbenwxxdjldwqlqnuixivodzcjgvbbgbco";
	string xffvhrwtock = "aifwuraqfsbmzrtsmpgqjuyxcppogiqiitqyjeymdepjrdaecwowgehgvnqucqrzsxnhjumeekai";
	double uxzraaog = 24296;
	double bfxqstlxqjnugti = 5363;
	string tvdxsys = "hppnbbvzxftycboycqfkymcdtxoucrgyffreyoexmaawgxtiogkyhsuaapomaruwqnxjfsbmfhvqp";
	double tfxapsikp = 10273;
	if (10273 == 10273) {
		int pjohw;
		for (pjohw = 11; pjohw > 0; pjohw--) {
			continue;
		}
	}
	if (false == false) {
		int lr;
		for (lr = 2; lr > 0; lr--) {
			continue;
		}
	}
	if (1862 != 1862) {
		int aqvjznll;
		for (aqvjznll = 95; aqvjznll > 0; aqvjznll--) {
			continue;
		}
	}
	return 53868;
}

double jpkpqfy::jhvlgjllzuohcob(int mfyxspmrgkg) {
	int zcvsvp = 4775;
	double cxiymspreaax = 38893;
	string kwiqx = "zv";
	string tctyprkeoswjqfn = "jlucszvdvnodgiqkqtqmsftvlmctriuhurjfgydrursjdfujfhkkjo";
	string sbgisdeqckutqp = "ycznegmlssfiuxuaxfubbpphjiqcydsglrvxxxkahhepqmvswwbkykwfznufoliwc";
	string ofigwiqyog = "yeockdbeisncucjxjzjunfqbgkjtkxyehtzxivldhhqkswgwaoplvgvlzltbugsqimjydfymffgccvaaewvp";
	int mmzocrjbgwttl = 7225;
	int uhzxxsehluo = 4025;
	if (string("jlucszvdvnodgiqkqtqmsftvlmctriuhurjfgydrursjdfujfhkkjo") == string("jlucszvdvnodgiqkqtqmsftvlmctriuhurjfgydrursjdfujfhkkjo")) {
		int cmbe;
		for (cmbe = 40; cmbe > 0; cmbe--) {
			continue;
		}
	}
	if (7225 == 7225) {
		int ofmrlss;
		for (ofmrlss = 37; ofmrlss > 0; ofmrlss--) {
			continue;
		}
	}
	if (string("yeockdbeisncucjxjzjunfqbgkjtkxyehtzxivldhhqkswgwaoplvgvlzltbugsqimjydfymffgccvaaewvp") == string("yeockdbeisncucjxjzjunfqbgkjtkxyehtzxivldhhqkswgwaoplvgvlzltbugsqimjydfymffgccvaaewvp")) {
		int dmc;
		for (dmc = 34; dmc > 0; dmc--) {
			continue;
		}
	}
	if (38893 == 38893) {
		int ivssdrqhxu;
		for (ivssdrqhxu = 83; ivssdrqhxu > 0; ivssdrqhxu--) {
			continue;
		}
	}
	if (38893 != 38893) {
		int ckjyavf;
		for (ckjyavf = 5; ckjyavf > 0; ckjyavf--) {
			continue;
		}
	}
	return 78145;
}

double jpkpqfy::bjeqsjpvyjmlio(int ldozu, double mzjkzrtxpd, string ohvagzylzxlvptw, bool wjjttvsuhuztj, bool aughemguzejw) {
	int vffds = 526;
	bool sakminuzqwwpetw = false;
	int sbelihthbxjx = 1606;
	string gsitcfrf = "girv";
	if (false == false) {
		int zvigvhef;
		for (zvigvhef = 10; zvigvhef > 0; zvigvhef--) {
			continue;
		}
	}
	return 1362;
}

string jpkpqfy::vxvrhxvcrnj(double dnenfhbiwq) {
	int tnkgiflvcdex = 477;
	bool mlcpgcxwyu = false;
	string bunaubuvgvnapvg = "ntwyptcwduuunsloskzyurknvzbkavjdhzrdwviaujqagwcfunfbaovtfzvzmmnibcdjsptkuplitkhtjivfpjqgsy";
	double aldjinquzbekxym = 41269;
	double fcdlwketmxnlqm = 15966;
	string vzuajdugtqgek = "imhbcczmrjbecezdaigogcuurayckwtqcxggykulwkczhdq";
	bool lhihw = false;
	bool wwthko = true;
	int wcqzb = 728;
	if (false != false) {
		int ig;
		for (ig = 26; ig > 0; ig--) {
			continue;
		}
	}
	if (false == false) {
		int fo;
		for (fo = 28; fo > 0; fo--) {
			continue;
		}
	}
	if (477 == 477) {
		int zpm;
		for (zpm = 23; zpm > 0; zpm--) {
			continue;
		}
	}
	if (15966 != 15966) {
		int iuopb;
		for (iuopb = 68; iuopb > 0; iuopb--) {
			continue;
		}
	}
	if (string("imhbcczmrjbecezdaigogcuurayckwtqcxggykulwkczhdq") == string("imhbcczmrjbecezdaigogcuurayckwtqcxggykulwkczhdq")) {
		int slirkmqrso;
		for (slirkmqrso = 6; slirkmqrso > 0; slirkmqrso--) {
			continue;
		}
	}
	return string("yhfijwrpeovcya");
}

double jpkpqfy::xgqloizrdfnezus(string yffckc, string siujtjefsdza, double qdwjbxqg, double ibeyjutry, string cgzohixnj) {
	bool kbjfdfjygqspycs = true;
	string iidwlwxqcc = "l";
	double giegrjkgbtaee = 60027;
	int kdiixc = 1921;
	bool encheig = true;
	string ofptpdzxdw = "jolljpguprnipgjjfwwkkqxpylzrraqrcnubjmrv";
	bool nryvxgysug = true;
	bool gxkoj = true;
	string juhxvzry = "sukxtszhflvlhtxmscocjcbbxrlxxgogpvdwsxxahqphwccsayqgrfixrtfkhgxclbazynygwspdogicubwtefxdjemkof";
	if (true != true) {
		int fnhvorowbc;
		for (fnhvorowbc = 85; fnhvorowbc > 0; fnhvorowbc--) {
			continue;
		}
	}
	if (true == true) {
		int qqwh;
		for (qqwh = 69; qqwh > 0; qqwh--) {
			continue;
		}
	}
	if (true != true) {
		int mqvkcg;
		for (mqvkcg = 77; mqvkcg > 0; mqvkcg--) {
			continue;
		}
	}
	return 14913;
}

string jpkpqfy::mkltgjjwvftmrvnfvpfcwymtp(int jylkhctgffbrhm, string yjnss) {
	double gzitclngt = 64695;
	bool inlyitigjkbyi = false;
	bool kniysh = false;
	double ozphm = 8947;
	bool knwmsmgyp = true;
	bool eymtrerjanzr = false;
	return string("kzmcwdfdobvzig");
}

void jpkpqfy::qdpxkynjbzijwdlehmgptb(int veymftyninihbj, string ozozjuthnylppx, bool ecmovbdm, bool vugmbgklnyfph, int lakyqzzrb, int zkmwnywrqwi) {

}

int jpkpqfy::mpabhaymadgpnlemhfij(bool kkwpqgdfqbdytxe, int fkkzvcc, string tgjhxqsatqiwqzx, string kboned, bool bkjswbvjsmuo, double soweezlsynawon, int uhwdffwygnfekq, bool hgeasdsfyx, int nmsmthv, double hnvkxjigq) {
	bool cjgxlkb = false;
	bool mwlwqnmzxckdyvc = true;
	int uqooveorc = 2420;
	bool xmglmjbh = false;
	bool tdgiltbsptdwai = true;
	string qlnjtffxejezi = "neuxlpnqzpmufvdghmrnltrggmetbhoblmznajlxrbqwjvbqjwhuibfdolxsftrgnyjgdzdxqbyz";
	bool bxwzfa = false;
	double dpfngmmsehwz = 17565;
	return 80340;
}

double jpkpqfy::mvzxlcqxshqyzgacuhrbrb(bool wqapsgsyksjqr) {
	string ykfhch = "zkrkvlvwxfmjdvxfvvmctkhnwsvdmtsymjcvxcnlghvsiktkelcntyzihaehequgoiuloxtx";
	string fqymvudcpwfpstn = "susdwsqwtvmhfdigckymiqdzvkispebvbfpofbdkngmg";
	int nrhqy = 240;
	if (string("susdwsqwtvmhfdigckymiqdzvkispebvbfpofbdkngmg") != string("susdwsqwtvmhfdigckymiqdzvkispebvbfpofbdkngmg")) {
		int lttwa;
		for (lttwa = 8; lttwa > 0; lttwa--) {
			continue;
		}
	}
	if (string("zkrkvlvwxfmjdvxfvvmctkhnwsvdmtsymjcvxcnlghvsiktkelcntyzihaehequgoiuloxtx") != string("zkrkvlvwxfmjdvxfvvmctkhnwsvdmtsymjcvxcnlghvsiktkelcntyzihaehequgoiuloxtx")) {
		int rtyl;
		for (rtyl = 21; rtyl > 0; rtyl--) {
			continue;
		}
	}
	if (string("susdwsqwtvmhfdigckymiqdzvkispebvbfpofbdkngmg") != string("susdwsqwtvmhfdigckymiqdzvkispebvbfpofbdkngmg")) {
		int wbzodbtkb;
		for (wbzodbtkb = 39; wbzodbtkb > 0; wbzodbtkb--) {
			continue;
		}
	}
	if (240 == 240) {
		int ycm;
		for (ycm = 18; ycm > 0; ycm--) {
			continue;
		}
	}
	return 25614;
}

bool jpkpqfy::owmtyfqmmyesewnkbrg() {
	bool ieewogrhqbbjvqc = true;
	int sfwcflsnm = 2356;
	double rqewzlesu = 1234;
	string oddmjdwcf = "fgtkauimyimukxkjtzutxzlvjrjw";
	bool skfxhamze = true;
	string zuuzfqlw = "lskzlbygubdspjvken";
	double qadapmefu = 2072;
	double dguqwh = 35991;
	string mhgqibsuinclai = "xrlovyndah";
	if (1234 != 1234) {
		int coiwjjkc;
		for (coiwjjkc = 97; coiwjjkc > 0; coiwjjkc--) {
			continue;
		}
	}
	if (string("lskzlbygubdspjvken") != string("lskzlbygubdspjvken")) {
		int wsfbrdfq;
		for (wsfbrdfq = 91; wsfbrdfq > 0; wsfbrdfq--) {
			continue;
		}
	}
	if (string("xrlovyndah") != string("xrlovyndah")) {
		int plmfr;
		for (plmfr = 51; plmfr > 0; plmfr--) {
			continue;
		}
	}
	if (string("fgtkauimyimukxkjtzutxzlvjrjw") != string("fgtkauimyimukxkjtzutxzlvjrjw")) {
		int bee;
		for (bee = 99; bee > 0; bee--) {
			continue;
		}
	}
	return true;
}

void jpkpqfy::lilkzccwal(string kpkcmqjts, string pmfblptoxxz, int iixfrmgnillrck) {
	int wsxtexfudfwnfkk = 5024;
	bool cuddcvm = true;
	if (5024 != 5024) {
		int xxrlprckd;
		for (xxrlprckd = 86; xxrlprckd > 0; xxrlprckd--) {
			continue;
		}
	}
	if (5024 != 5024) {
		int ftegwlgq;
		for (ftegwlgq = 93; ftegwlgq > 0; ftegwlgq--) {
			continue;
		}
	}
	if (5024 == 5024) {
		int swqczpx;
		for (swqczpx = 1; swqczpx > 0; swqczpx--) {
			continue;
		}
	}

}

int jpkpqfy::kckcrxyqhaa(double mtezsmnp, bool ffukevfkbunba, double nowhpd, double gutcyyizdvh, string zcrigpyaifshtpe, string qxwebpy, string ywgoseyvgbbc, string oixqfjpypcsle, bool waimwmgc, int szeawtdyw) {
	bool nlosd = false;
	return 14754;
}

bool jpkpqfy::qsxlhcmkqhxuawvvmepqbcrzd(string dddztxljhejo) {
	return false;
}

void jpkpqfy::tebrwbqwmcxbcvbtacv(bool gnvvvlqvfypqda, bool jhdymg, bool yvaygtnlgc, int vyzhevh, string vysogqv, int gosufshl, double kbqlbrzemclpy, double nocnxpqs, string bwfetugpgbfeh) {
	int pypvgelf = 4011;
	bool exyhaskxftmj = true;
	int sgqrhgylwkcii = 2216;
	bool zypinh = false;
	if (false != false) {
		int koepwqe;
		for (koepwqe = 30; koepwqe > 0; koepwqe--) {
			continue;
		}
	}
	if (false != false) {
		int pvkkblaz;
		for (pvkkblaz = 81; pvkkblaz > 0; pvkkblaz--) {
			continue;
		}
	}

}

string jpkpqfy::xythxhfnnmvyoyjnf(bool ljndl, string funbfurvt, double ograhhgjvg) {
	double jhyvq = 8623;
	double yornekgmxkzyf = 13374;
	double qwyzrgr = 19499;
	if (19499 != 19499) {
		int jvx;
		for (jvx = 6; jvx > 0; jvx--) {
			continue;
		}
	}
	if (19499 == 19499) {
		int ijrzwwrir;
		for (ijrzwwrir = 90; ijrzwwrir > 0; ijrzwwrir--) {
			continue;
		}
	}
	if (19499 != 19499) {
		int eljdw;
		for (eljdw = 15; eljdw > 0; eljdw--) {
			continue;
		}
	}
	if (19499 == 19499) {
		int tpoylofbv;
		for (tpoylofbv = 15; tpoylofbv > 0; tpoylofbv--) {
			continue;
		}
	}
	return string("");
}

void jpkpqfy::fxlvcfbyqyaayysbkkt(double mrkyxrjwtysefo, bool xxhzgh, int hbtwk, string uqyxuoyyvxrspxc, double cedttc) {
	double odvxfjccxj = 29;
	int bmdvxpzedfici = 272;
	bool hcbrpysi = false;
	string gtpzoybhcoayhwf = "tykgxayvxwhyqwwhzposemdfgqudxlcmqokwlyaczqalmpvhprjdrruu";
	double adyphvjry = 20834;
	string dyhdgcgupncesqz = "oqnlwdgzenowhpwqbojkkyhuttwiqxpuelyjtukosemreynghrqaehqptdcvypqysblimuenhioovtxsbfiyrac";
	string jqoatms = "mlyrczywldg";
	bool htddwtzuup = false;
	if (272 == 272) {
		int ydwfho;
		for (ydwfho = 60; ydwfho > 0; ydwfho--) {
			continue;
		}
	}
	if (29 != 29) {
		int pulevxcdzj;
		for (pulevxcdzj = 37; pulevxcdzj > 0; pulevxcdzj--) {
			continue;
		}
	}
	if (20834 == 20834) {
		int rdshn;
		for (rdshn = 68; rdshn > 0; rdshn--) {
			continue;
		}
	}

}

int jpkpqfy::ckiufiduwzftirvufj(string jswwe) {
	double jqlqykxtud = 5865;
	int zzwwzfyjta = 2913;
	string ziazgyvbgavjqvg = "ahtppax";
	double jjkrzapjmvndwh = 13498;
	if (2913 == 2913) {
		int kejhnhqyqm;
		for (kejhnhqyqm = 31; kejhnhqyqm > 0; kejhnhqyqm--) {
			continue;
		}
	}
	return 23304;
}

string jpkpqfy::doycqoddtuueeidaibcxndfu(double jxuyknwppcaikdz, double thsubhwtnsmtf, bool zlmnbgrkqtnlxdn, string ruymtc, double lmfyffrpjbzspyy) {
	double pehxtoyvwatq = 1977;
	int zlfphh = 653;
	if (653 != 653) {
		int wq;
		for (wq = 99; wq > 0; wq--) {
			continue;
		}
	}
	if (653 != 653) {
		int sphswr;
		for (sphswr = 54; sphswr > 0; sphswr--) {
			continue;
		}
	}
	if (653 == 653) {
		int fourhpaevu;
		for (fourhpaevu = 44; fourhpaevu > 0; fourhpaevu--) {
			continue;
		}
	}
	return string("sryjiduo");
}

void jpkpqfy::sryipwbtpzws(string fbzfujopwsgfi) {
	int bybalhwcjsec = 3691;
	int yvsauxxbxqmgq = 3286;
	int nmkifpnb = 3204;
	double qdimzvaiipjp = 40589;
	int uzxytjnvjd = 4700;
	string icmogntnvzuczr = "cjlpcusfanvctkoswumazdjnckeveowjawslnsrhalmnotqjzebkyfjcvxexhamknhwsgqruusywsrktuzikgsnbo";
	double kesueifsnc = 936;
	double xwrrsstck = 24578;
	int wwvcwdbfksbvftz = 2757;
	double qtxhr = 54206;

}

double jpkpqfy::mmesemlahcouabpyrvrwdnqf(string lywuao) {
	double utkdialindxew = 11061;
	double xhhugm = 6172;
	if (11061 == 11061) {
		int ftmdagqau;
		for (ftmdagqau = 42; ftmdagqau > 0; ftmdagqau--) {
			continue;
		}
	}
	if (11061 != 11061) {
		int lde;
		for (lde = 53; lde > 0; lde--) {
			continue;
		}
	}
	if (6172 == 6172) {
		int egxxc;
		for (egxxc = 81; egxxc > 0; egxxc--) {
			continue;
		}
	}
	if (11061 != 11061) {
		int xmsrnafxx;
		for (xmsrnafxx = 40; xmsrnafxx > 0; xmsrnafxx--) {
			continue;
		}
	}
	if (11061 == 11061) {
		int cjr;
		for (cjr = 43; cjr > 0; cjr--) {
			continue;
		}
	}
	return 56017;
}

jpkpqfy::jpkpqfy() {
	this->qsxlhcmkqhxuawvvmepqbcrzd(string("rf"));
	this->tebrwbqwmcxbcvbtacv(false, false, false, 1326, string("bnshjseeufnejcyrpikmwxowjkvrlrumzfcqbfefmdxekghlhq"), 1664, 28411, 54055, string("rmeepznlzebvnudscvujiatqxsalsjfibvvzlsvuysvyjepjbxywgxdxc"));
	this->xythxhfnnmvyoyjnf(false, string("ocognorlmswgunpzlbcibbofxigowhkatzfupyphmqypiikqlstowavepxcmsjmffjzccreivbpt"), 18469);
	this->fxlvcfbyqyaayysbkkt(8926, true, 614, string("buuxchgcvnrpbrcchutuhudffmrhqjgrvxfypazzhjwtdpddvnrlvqzhuykhodispsvtp"), 13308);
	this->ckiufiduwzftirvufj(string("ixehpjswirtsivtvblvvqbpfhyrwswdiyercdcnnjvklmarreeisczdjhvwrsdwoqrkqadmjotk"));
	this->doycqoddtuueeidaibcxndfu(30941, 4372, false, string("opopqesxxbaazfshnsdmebuxqbnhtyyt"), 45764);
	this->sryipwbtpzws(string("demzgahmiowircqzpoynvokhwirpnzhgggiwsyxkkxszwkdrlczgdjgqhnvwcgmxeiibhvsftawdjvtbd"));
	this->mmesemlahcouabpyrvrwdnqf(string("gcqrzjmforuanpxsjlgwtxusttttrytoelvnalbllp"));
	this->bjeqsjpvyjmlio(4315, 11458, string("xtyykcaemktaziqacmsxwsdvlfxxuirxcmkyxrzbkaylcceheqojwflqrjmjubnmbwvq"), true, true);
	this->vxvrhxvcrnj(59149);
	this->xgqloizrdfnezus(string("odhmddjudfggwwjdsgijdoewjkvebedyvby"), string("bdgildpnqgismypkggnjzlomfwloekoyduatjyxpozszb"), 8100, 7529, string("hyvcxchvxorydubjbceecfwjqc"));
	this->mkltgjjwvftmrvnfvpfcwymtp(2544, string("bbvfbsofnhqjdctjzcokududwsofhtityqsbvatoebdn"));
	this->qdpxkynjbzijwdlehmgptb(387, string("nxiyqohvkr"), false, true, 6711, 2242);
	this->mpabhaymadgpnlemhfij(true, 234, string("whbyhqnzymkpppvacrkpykhzfofigscg"), string("zdrsnvsoedekiypugkeklemooxuudvrsaoqjphhypxfjxbwvnvyqvhdprgdneahofkeffkfykosuzglqfsinuusonee"), false, 35465, 3107, true, 6838, 1683);
	this->mvzxlcqxshqyzgacuhrbrb(true);
	this->owmtyfqmmyesewnkbrg();
	this->lilkzccwal(string("ksgcscagznbmlotkfsyolmqgcao"), string("trtkysheacnlrcbdsfjjyfkmekwbsmokjsfsggdytsebhnspytexzqoymlsiyuimusibmtdltlzlrchauecpzadhoooeidx"), 8782);
	this->kckcrxyqhaa(1853, true, 54169, 38136, string("tpswlofqrhrfyzwmanapzhixdefmawukwrjflvqbejgstgvahmlcryscncgqjckfujzub"), string("ldmfqjkixhhqgwgivgrqbwmoxkyqspickxnfzclquvazmyjcxl"), string("vqwowlyoozylzvaywvqbmbyyewwakpgojrmhunuizzxcjnlnddcayjwrreirolxgxfulgfvkzpykqhmrnruke"), string("qcqugkcmrippindwwuxkwnoukiulijtzfjtfibjhktfaardjqdqnlrvauj"), false, 718);
	this->sziyzggoqejexw(1211, 11424);
	this->xhkvzgspwyonkpbbhhqgpx(string("eozjpeuztsbpzetswtjjntwaamaootxpli"), true, string("hhgsfsapdphyx"), 51817);
	this->rzhdufrgztbcyvmhjtyqr(289, 3225, 25201, 2696, string("ijakqcxjmhfrlxisdjiuiqlvrbzqztspdvjb"), 37515);
	this->xccvhjielcwhjsmwgkq(false, false, 20403, 1710, false);
	this->swjpwywbyseyp();
	this->vkyqqqquytseqzt(599, 1123, 14864);
	this->jhvlgjllzuohcob(725);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class thpbdfm {
public:
	int olmyumbimdbgkfz;
	thpbdfm();
	int lpnoqkkifxtrbzfwjzs(double nrfprknyi, int lnzfwfphqvdnb, string udomhryyiwgqlt, bool zpvhnsxmxfbofa, double kdtodmvpwvwdi, double cfmwhvafmrewpt, double dzgnpg, string lveguepwc);
	string bhttafhehhioxnquid(bool liekeoly, int tsermhjabzlogn, int pfkqefsnlkw);
	int ghkzlkoazcsmoensqeakhq(double ukgxwfphfcdpr, string ziqdcqkklzehy, double ajlzalkr, int qlsrzgiv, bool sohpoy, string srznylpfyn, string kmbkn, double igawoyky);
	string unsloyuwklhfxacpw(int zdfiagzsetva, bool lsqmurdynf);
	string fejamrfusmpblqsnwxaq(bool shjwmht, string locxzqkrprhtwrd, double ptnqelddce, double rntixgxkbxufe, double ijnua);
	int byfblixqhgd(string ylbreijd, bool zcbwkpbml, int pvmnobefoabls, string ktlvwdfartkwgsm);
	int wfilrvuwkp(double xbwrjydqm);

protected:
	int vpokyiruxy;
	int uchbunnfni;

	void zmgmccvjvcwjlcaethfwp(string cxkdcgdwecnky, double nexjndkpxoegh, double pfegv, double zrehjdf, int dmnufdt, double exhgcpl);
	void axtsgeypkpyfm(int sohljybwdfvf, string zwfasbsqvjtfmiq, string vshgcfrkqwwr, int tyyzd, string cgwlkodnl, int iwvfv, bool wvvetlx);
	int vhjjlscollpkgzkvvbcozss(int owgxqlelwdawvn, bool txvqpx, bool alijrlhbwz, bool xushyvrflsn, bool itoddzujghrxtr);
	string icafppvgctl(bool dkbcsn, int fjitlcjtjd, int lipzia, string zpezvkcdmffy);

private:
	double htoknjtxfoqjxi;
	string aboblheppxgp;
	int vlxolrpexsirkb;
	int jtrdtvq;

	double oetcfjsjxfwaoewgjcubg(double pqcoevaowkyrl, bool vrclxbmx, double yrcxhbt, double vlhvde, double kzivr, double nohrztqjlgoen, bool wfoigxijehrppta, string uigyrggbaxmo);
	double izouipixefwnrpbsle(bool uqjsnqbzhllgka, string ihujlumkfkz, int mmdpwezghbs, int lqfrou);
	double lfoqayfuryzes();
	void hhvoxquchztawkspbexfkdqxq(int ydqairecoozo, bool hdtcqobsjcw, bool itcqywpsk, string rauienxn, double bgqerm, bool srfxbacgawzt, int zxgvwatmxryxm, int dukph, string gtrhnv);
	bool asrigmveequa(bool ezsulhrrhjzw, string axglswqzt, string yquskratxmqaq, bool pbcprrpov, string pgfzjlqqtp, int efbwsvby);
	bool uiksdhhtjueanqpwjiqueafbe(int vwwbaaapzctv, bool psclwojqmhwvqf, bool dflrstcyzex, bool aacubcfvvw, double dtyyf, int mehcrib, double zgdszuy, int csrajrgmii, string xieto);
	double npiaiktbzlxdh(string stkqoabih, string vgtsemkdpo, int wvujlid, string qogtk, int jqvdqqtpoqljbj, string wwtav, int wiikqmmv, string exodtqrmztpnrq, bool wjxzolly);
	int fpiljyazgchauafrtp(double hmmqhxpuplktaps, double lmlodttlcah, int rtqexpi, int xoezgnbui, bool rutolfbetzyhd, bool jggbwisrnlfrdd, string nveup);

};


double thpbdfm::oetcfjsjxfwaoewgjcubg(double pqcoevaowkyrl, bool vrclxbmx, double yrcxhbt, double vlhvde, double kzivr, double nohrztqjlgoen, bool wfoigxijehrppta, string uigyrggbaxmo) {
	string vnncbmcwzbezn = "vjjshemjx";
	string iwiuqk = "fbwogkeihhjuzfygtudhfkqgzyefhtmhvzrucuddr";
	double acffgrlletw = 53294;
	int sxllzlunlyok = 2092;
	if (string("fbwogkeihhjuzfygtudhfkqgzyefhtmhvzrucuddr") != string("fbwogkeihhjuzfygtudhfkqgzyefhtmhvzrucuddr")) {
		int eimls;
		for (eimls = 80; eimls > 0; eimls--) {
			continue;
		}
	}
	if (53294 == 53294) {
		int jn;
		for (jn = 10; jn > 0; jn--) {
			continue;
		}
	}
	if (string("vjjshemjx") != string("vjjshemjx")) {
		int ualafoy;
		for (ualafoy = 6; ualafoy > 0; ualafoy--) {
			continue;
		}
	}
	if (string("fbwogkeihhjuzfygtudhfkqgzyefhtmhvzrucuddr") != string("fbwogkeihhjuzfygtudhfkqgzyefhtmhvzrucuddr")) {
		int rwfdvtzq;
		for (rwfdvtzq = 60; rwfdvtzq > 0; rwfdvtzq--) {
			continue;
		}
	}
	return 55458;
}

double thpbdfm::izouipixefwnrpbsle(bool uqjsnqbzhllgka, string ihujlumkfkz, int mmdpwezghbs, int lqfrou) {
	int ujkkr = 3748;
	bool adkxszjudfzmn = true;
	double fdkagx = 46299;
	double gxtvg = 27323;
	double zobvgwfzyp = 34696;
	int neyybwhx = 555;
	if (46299 == 46299) {
		int nkf;
		for (nkf = 7; nkf > 0; nkf--) {
			continue;
		}
	}
	if (3748 == 3748) {
		int jeh;
		for (jeh = 24; jeh > 0; jeh--) {
			continue;
		}
	}
	return 65427;
}

double thpbdfm::lfoqayfuryzes() {
	double zazdutkcjn = 20534;
	double nodakog = 7174;
	double aonmmoocrdvwg = 10617;
	string bonykunbepag = "whasgbqkcmbtnvzfhexsjblbochc";
	string ymqvkmncvawa = "holqkbfnasvdmubrtmjdrqyrxdewqbkadqqrjnmmcox";
	if (10617 == 10617) {
		int hphn;
		for (hphn = 49; hphn > 0; hphn--) {
			continue;
		}
	}
	if (20534 == 20534) {
		int njmguawngo;
		for (njmguawngo = 47; njmguawngo > 0; njmguawngo--) {
			continue;
		}
	}
	return 6697;
}

void thpbdfm::hhvoxquchztawkspbexfkdqxq(int ydqairecoozo, bool hdtcqobsjcw, bool itcqywpsk, string rauienxn, double bgqerm, bool srfxbacgawzt, int zxgvwatmxryxm, int dukph, string gtrhnv) {
	bool usumynf = true;
	bool fpanufsud = true;
	int awdxyerfvokuo = 1790;
	double xsiefvfmcfbufx = 10363;
	int eugajacu = 2772;
	string dgtngtggieg = "xiulkcrwazpibwwhxygwysxrt";

}

bool thpbdfm::asrigmveequa(bool ezsulhrrhjzw, string axglswqzt, string yquskratxmqaq, bool pbcprrpov, string pgfzjlqqtp, int efbwsvby) {
	return false;
}

bool thpbdfm::uiksdhhtjueanqpwjiqueafbe(int vwwbaaapzctv, bool psclwojqmhwvqf, bool dflrstcyzex, bool aacubcfvvw, double dtyyf, int mehcrib, double zgdszuy, int csrajrgmii, string xieto) {
	bool rpwjyrbex = true;
	int nxqjvzxv = 531;
	bool nyxockzkuufdutq = false;
	string sibypxjxbn = "rhtfvehtlizzsvqutrjqh";
	int sqwugrunxtgyntg = 511;
	string cygcjzxqfpriz = "xho";
	if (string("xho") != string("xho")) {
		int to;
		for (to = 59; to > 0; to--) {
			continue;
		}
	}
	if (string("xho") != string("xho")) {
		int cbmgm;
		for (cbmgm = 70; cbmgm > 0; cbmgm--) {
			continue;
		}
	}
	if (531 == 531) {
		int roia;
		for (roia = 63; roia > 0; roia--) {
			continue;
		}
	}
	return true;
}

double thpbdfm::npiaiktbzlxdh(string stkqoabih, string vgtsemkdpo, int wvujlid, string qogtk, int jqvdqqtpoqljbj, string wwtav, int wiikqmmv, string exodtqrmztpnrq, bool wjxzolly) {
	return 89753;
}

int thpbdfm::fpiljyazgchauafrtp(double hmmqhxpuplktaps, double lmlodttlcah, int rtqexpi, int xoezgnbui, bool rutolfbetzyhd, bool jggbwisrnlfrdd, string nveup) {
	string gkdxdbjemhsu = "rwojtplfrifalznkenshixxsspqzk";
	string emkew = "axbtxhewtmixsdwpqhyvdmjuhhrrmhlqldsqzewpnnsbvmqymmaeq";
	double aafibl = 49654;
	if (string("axbtxhewtmixsdwpqhyvdmjuhhrrmhlqldsqzewpnnsbvmqymmaeq") == string("axbtxhewtmixsdwpqhyvdmjuhhrrmhlqldsqzewpnnsbvmqymmaeq")) {
		int znu;
		for (znu = 12; znu > 0; znu--) {
			continue;
		}
	}
	if (49654 != 49654) {
		int qditplr;
		for (qditplr = 55; qditplr > 0; qditplr--) {
			continue;
		}
	}
	if (string("rwojtplfrifalznkenshixxsspqzk") != string("rwojtplfrifalznkenshixxsspqzk")) {
		int droz;
		for (droz = 30; droz > 0; droz--) {
			continue;
		}
	}
	if (string("axbtxhewtmixsdwpqhyvdmjuhhrrmhlqldsqzewpnnsbvmqymmaeq") == string("axbtxhewtmixsdwpqhyvdmjuhhrrmhlqldsqzewpnnsbvmqymmaeq")) {
		int jdzupc;
		for (jdzupc = 90; jdzupc > 0; jdzupc--) {
			continue;
		}
	}
	if (string("rwojtplfrifalznkenshixxsspqzk") != string("rwojtplfrifalznkenshixxsspqzk")) {
		int znfc;
		for (znfc = 67; znfc > 0; znfc--) {
			continue;
		}
	}
	return 40137;
}

void thpbdfm::zmgmccvjvcwjlcaethfwp(string cxkdcgdwecnky, double nexjndkpxoegh, double pfegv, double zrehjdf, int dmnufdt, double exhgcpl) {
	int kxhqadozz = 1329;
	bool yztikt = false;
	if (false == false) {
		int rbazh;
		for (rbazh = 40; rbazh > 0; rbazh--) {
			continue;
		}
	}
	if (false != false) {
		int glfuqy;
		for (glfuqy = 75; glfuqy > 0; glfuqy--) {
			continue;
		}
	}
	if (false == false) {
		int uwduccbdfk;
		for (uwduccbdfk = 58; uwduccbdfk > 0; uwduccbdfk--) {
			continue;
		}
	}
	if (false != false) {
		int yitv;
		for (yitv = 9; yitv > 0; yitv--) {
			continue;
		}
	}
	if (false != false) {
		int fwlsinmp;
		for (fwlsinmp = 89; fwlsinmp > 0; fwlsinmp--) {
			continue;
		}
	}

}

void thpbdfm::axtsgeypkpyfm(int sohljybwdfvf, string zwfasbsqvjtfmiq, string vshgcfrkqwwr, int tyyzd, string cgwlkodnl, int iwvfv, bool wvvetlx) {
	int fiorvgydxg = 8946;
	double frrtmxd = 32794;
	int yfgwwmcn = 4150;
	if (8946 == 8946) {
		int gemg;
		for (gemg = 41; gemg > 0; gemg--) {
			continue;
		}
	}
	if (8946 == 8946) {
		int hbkmb;
		for (hbkmb = 61; hbkmb > 0; hbkmb--) {
			continue;
		}
	}

}

int thpbdfm::vhjjlscollpkgzkvvbcozss(int owgxqlelwdawvn, bool txvqpx, bool alijrlhbwz, bool xushyvrflsn, bool itoddzujghrxtr) {
	string uoxiwur = "irrnyhvizfvnboxlntokijijgjqniwgbgvqdfjfpomeocdqopr";
	double rgcedxea = 39897;
	double fxiqut = 3341;
	bool znadjnclgp = true;
	return 95062;
}

string thpbdfm::icafppvgctl(bool dkbcsn, int fjitlcjtjd, int lipzia, string zpezvkcdmffy) {
	string jxwxbdhgqmrtcj = "bfxhqxuafzwxmlugqlbcjzsjdprbggctcufhixeakqscuyftfccnuheybjpwigzmxcbdzgavkveogvvhmuiogglylspnm";
	double jobnvcm = 33765;
	if (string("bfxhqxuafzwxmlugqlbcjzsjdprbggctcufhixeakqscuyftfccnuheybjpwigzmxcbdzgavkveogvvhmuiogglylspnm") != string("bfxhqxuafzwxmlugqlbcjzsjdprbggctcufhixeakqscuyftfccnuheybjpwigzmxcbdzgavkveogvvhmuiogglylspnm")) {
		int pmf;
		for (pmf = 76; pmf > 0; pmf--) {
			continue;
		}
	}
	if (string("bfxhqxuafzwxmlugqlbcjzsjdprbggctcufhixeakqscuyftfccnuheybjpwigzmxcbdzgavkveogvvhmuiogglylspnm") == string("bfxhqxuafzwxmlugqlbcjzsjdprbggctcufhixeakqscuyftfccnuheybjpwigzmxcbdzgavkveogvvhmuiogglylspnm")) {
		int omdzsul;
		for (omdzsul = 11; omdzsul > 0; omdzsul--) {
			continue;
		}
	}
	if (string("bfxhqxuafzwxmlugqlbcjzsjdprbggctcufhixeakqscuyftfccnuheybjpwigzmxcbdzgavkveogvvhmuiogglylspnm") != string("bfxhqxuafzwxmlugqlbcjzsjdprbggctcufhixeakqscuyftfccnuheybjpwigzmxcbdzgavkveogvvhmuiogglylspnm")) {
		int wzqjr;
		for (wzqjr = 32; wzqjr > 0; wzqjr--) {
			continue;
		}
	}
	return string("lncufsod");
}

int thpbdfm::lpnoqkkifxtrbzfwjzs(double nrfprknyi, int lnzfwfphqvdnb, string udomhryyiwgqlt, bool zpvhnsxmxfbofa, double kdtodmvpwvwdi, double cfmwhvafmrewpt, double dzgnpg, string lveguepwc) {
	double tlmpajoafoz = 2681;
	bool zfozoht = false;
	bool mbuamwjiqlzpk = true;
	bool gplyzuxvgmfkbzs = true;
	bool ghzosjfyxin = false;
	if (false == false) {
		int ptia;
		for (ptia = 84; ptia > 0; ptia--) {
			continue;
		}
	}
	return 77973;
}

string thpbdfm::bhttafhehhioxnquid(bool liekeoly, int tsermhjabzlogn, int pfkqefsnlkw) {
	int omoqjh = 1163;
	string wnkrdagpogj = "ltxpxlerltnpseliwaaojbzgqbfvwfzozlzgbkjcjqjpassnggz";
	string hlzmsyjzizkrk = "iqxcyfcfrucgzphfawxkqkjtrxazrdyddblgusdhlrdpuacxozabtpppmohmdvngaxaaalapcqhjvj";
	bool tmragsgso = true;
	double oqbjtstarh = 9332;
	bool qhklprzbehhb = false;
	string ddzuzltaaitlpxk = "kpvkhaoiplwywodrkwluxvpdpwlslchuyfjifdvza";
	string dtznf = "pgjvsimbsjkbcfkfw";
	double dfegobflnnrixf = 2474;
	if (1163 == 1163) {
		int wjd;
		for (wjd = 28; wjd > 0; wjd--) {
			continue;
		}
	}
	if (string("kpvkhaoiplwywodrkwluxvpdpwlslchuyfjifdvza") != string("kpvkhaoiplwywodrkwluxvpdpwlslchuyfjifdvza")) {
		int ic;
		for (ic = 68; ic > 0; ic--) {
			continue;
		}
	}
	return string("ave");
}

int thpbdfm::ghkzlkoazcsmoensqeakhq(double ukgxwfphfcdpr, string ziqdcqkklzehy, double ajlzalkr, int qlsrzgiv, bool sohpoy, string srznylpfyn, string kmbkn, double igawoyky) {
	string xahqpqjxlgbnfj = "oiirdpycmgpphrzueudznnqdfkyju";
	bool gxiwyxixlfkgz = true;
	int rtrwaxkr = 4753;
	double snkiawn = 1083;
	int ldprcbyiqdgpvx = 2067;
	string ubgnhggxcb = "difyqppuiz";
	string lxgkgrsxjqah = "vroovxyxahvevqegcuvqjotbvprygdhppbhwojffdpulvxnscxszrtnazftfusxtrb";
	return 29324;
}

string thpbdfm::unsloyuwklhfxacpw(int zdfiagzsetva, bool lsqmurdynf) {
	bool grvghiy = false;
	int feubzrnrqxfnn = 2838;
	bool vzkpvkkuv = false;
	int ojpfvlsgogd = 2336;
	bool rrvutq = false;
	string rusccbsagzowl = "vzfjpyjocgpglufudejufetixweiuovzpbraeoznkltgbufkdmdadxrsgwhefedyzgvjmnvelwthcdhnvpaugj";
	int eajahk = 4195;
	int dmcosnaqmxjicq = 684;
	int bhzfdzteczvi = 5666;
	if (false == false) {
		int prx;
		for (prx = 14; prx > 0; prx--) {
			continue;
		}
	}
	if (2336 == 2336) {
		int wobxgi;
		for (wobxgi = 55; wobxgi > 0; wobxgi--) {
			continue;
		}
	}
	if (string("vzfjpyjocgpglufudejufetixweiuovzpbraeoznkltgbufkdmdadxrsgwhefedyzgvjmnvelwthcdhnvpaugj") == string("vzfjpyjocgpglufudejufetixweiuovzpbraeoznkltgbufkdmdadxrsgwhefedyzgvjmnvelwthcdhnvpaugj")) {
		int zhs;
		for (zhs = 20; zhs > 0; zhs--) {
			continue;
		}
	}
	if (2838 == 2838) {
		int vkutsodqsa;
		for (vkutsodqsa = 78; vkutsodqsa > 0; vkutsodqsa--) {
			continue;
		}
	}
	if (684 == 684) {
		int loj;
		for (loj = 34; loj > 0; loj--) {
			continue;
		}
	}
	return string("rwpqxskvdhijacniisd");
}

string thpbdfm::fejamrfusmpblqsnwxaq(bool shjwmht, string locxzqkrprhtwrd, double ptnqelddce, double rntixgxkbxufe, double ijnua) {
	bool ahjhd = true;
	if (true == true) {
		int tky;
		for (tky = 32; tky > 0; tky--) {
			continue;
		}
	}
	if (true == true) {
		int nmdwohhz;
		for (nmdwohhz = 7; nmdwohhz > 0; nmdwohhz--) {
			continue;
		}
	}
	return string("gnpua");
}

int thpbdfm::byfblixqhgd(string ylbreijd, bool zcbwkpbml, int pvmnobefoabls, string ktlvwdfartkwgsm) {
	int yasdlgdbo = 1083;
	int yweyyqr = 919;
	int brqmuthcehugwaa = 2363;
	int jyyowencdmhumda = 3845;
	string flaghiihgkj = "dkkkuqwypjbqqdhtvpvqjoqzepxtokwlaakhvxccentfvdcrnnxeekvooioibukoahsuourgddn";
	double jpifwkzdbsd = 21694;
	string anzurtoumjkdvm = "kbdwngfogckmwlpojrnewcfyuqcpfjqcqgwgbdrzsparjmswwq";
	if (21694 != 21694) {
		int bhldul;
		for (bhldul = 98; bhldul > 0; bhldul--) {
			continue;
		}
	}
	if (2363 != 2363) {
		int kxbgpmfs;
		for (kxbgpmfs = 21; kxbgpmfs > 0; kxbgpmfs--) {
			continue;
		}
	}
	if (string("kbdwngfogckmwlpojrnewcfyuqcpfjqcqgwgbdrzsparjmswwq") == string("kbdwngfogckmwlpojrnewcfyuqcpfjqcqgwgbdrzsparjmswwq")) {
		int gm;
		for (gm = 88; gm > 0; gm--) {
			continue;
		}
	}
	return 46796;
}

int thpbdfm::wfilrvuwkp(double xbwrjydqm) {
	double esmpfppzwg = 59547;
	string nkrrmeeaievl = "yj";
	return 6110;
}

thpbdfm::thpbdfm() {
	this->lpnoqkkifxtrbzfwjzs(26170, 754, string("dqpwidwgfuokq"), true, 8336, 8625, 20021, string("mczpzjxomtoaxlbrgvojskyxrysdlbuezvgupbuvlxicnsbdbmmsjjlqkreqzoirvkbimmkvxtujzxvlnm"));
	this->bhttafhehhioxnquid(true, 1854, 2039);
	this->ghkzlkoazcsmoensqeakhq(23098, string("ssccgjxeuhstqowtvkzxfys"), 35044, 1175, false, string("wbffdslmcqtoehwchdsfmoypvybykynbovaevvqwwapesffossxxifaivlbqflboglpoqenddv"), string("suhpcamvfbvtgl"), 9331);
	this->unsloyuwklhfxacpw(4292, false);
	this->fejamrfusmpblqsnwxaq(true, string("theztyinbtcsxbfjgaqmtgtlltgphbktmdbfbkkizxxtjtceubxpqqzdrhbjkfayefe"), 39480, 40035, 4814);
	this->byfblixqhgd(string("ikvelzpecxpgbzefbrbettljwyqncidokrorlodbdtkp"), true, 2028, string("krafceqfczoichipzdhgwmfvpqlftjknlhyagkpfwbscyb"));
	this->wfilrvuwkp(33805);
	this->zmgmccvjvcwjlcaethfwp(string("mluxdhgofnfqfnvfepjujxdcccfhgbeuumzuaempgfrmneddsqcrdxpqbezwfwuousksortuwwsrnklqhiml"), 78185, 26195, 5072, 558, 24270);
	this->axtsgeypkpyfm(7160, string("stoolgzskmxk"), string("nwilzknbjxoxjsjkfgdteelfjosmznbtrmerykmnaeqpedkixs"), 1190, string("gimutwlovowsblcqjhsdduykmpybslbibcxth"), 751, true);
	this->vhjjlscollpkgzkvvbcozss(3137, true, true, false, false);
	this->icafppvgctl(false, 9, 1653, string("tgeddrtqfwbyvjbsnmzmtwwvyjgmtsofhvsupwryxvggjf"));
	this->oetcfjsjxfwaoewgjcubg(59666, true, 37506, 12760, 26045, 20074, false, string("ghfvgiluwikydpgj"));
	this->izouipixefwnrpbsle(false, string("xztbgtjvdozfnrllntkyygowyfwbakghlnyupkynnorgkqqaljwswblfulnbjkmejqdyhmoqbexqrttajgahxddpma"), 3912, 3839);
	this->lfoqayfuryzes();
	this->hhvoxquchztawkspbexfkdqxq(456, false, true, string("cdbfgyhmcrkcqhppjlrhoidrdkyeiwszeboqoryhhvfacjnjslfe"), 8500, false, 267, 1913, string("nunfhwnyveqqlvpvaophocnskrnyayptrtmqlbzvisjrqjmjintetmjvttpghmnhvlvurdsyjlxikac"));
	this->asrigmveequa(true, string("ksafwsomjivgpjvgnwwifkxqwgwdklyxferlpbloyvnqalgzcjljvclesb"), string("iixt"), true, string("dtxkogqfqosxyyopjdnwcyjcfnkeiqbejxg"), 7085);
	this->uiksdhhtjueanqpwjiqueafbe(2668, false, true, true, 4549, 314, 34784, 1663, string("iiabqu"));
	this->npiaiktbzlxdh(string("smvmrkgivieaoyywouvccdynejqacawmjptzwfcvviefaqxoiwptjvdndexxctctmbxcmpurxpuxmpjzstbwzspuikovk"), string("xljkvgprpcykwtlqx"), 142, string("eycxgamuzhxjvxbxmdkzvhuvobxinakmq"), 538, string("rerjvwgiizsqrktfctbgxerugccnfxdgcfufiop"), 984, string("rcoqzigfagymghrprtsstgt"), false);
	this->fpiljyazgchauafrtp(29648, 6835, 3634, 183, false, false, string(""));
}


















































































































































































































































































