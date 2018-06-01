#include "HookIncludes.h"
#include "HooksList.h"
#include "Listener.h"



namespace hooks
{
    vfunc_hook panel;
    vfunc_hook client;
    vfunc_hook clientmode;
    vfunc_hook modelrender;
    vfunc_hook prediction;
    vfunc_hook surface;
    vfunc_hook eventmanager;
    vfunc_hook d3d;
    vfunc_hook viewrender;
    vfunc_hook engine;
    vfunc_hook sv_cheats;
    void initialize()
    {
        client.setup(g_CHLClient);
        client.hook_index(36, hkFrameStageNotify);
        client.hook_index(21, hkCreateMove);

        //prediction.setup(g_Prediction);
        //prediction.hook_index(14, hkInPrediction);

        clientmode.setup(g_ClientMode);
        clientmode.hook_index(18, hkOverrideView);
        clientmode.hook_index(35, hkGetViewModelFOV);
        clientmode.hook_index(44, hkDoPostScreenSpaceEffects);

        panel.setup(g_Panel);
        panel.hook_index(41, hkPaintTraverse);

        modelrender.setup(g_ModelRender);
        modelrender.hook_index(21, hkDrawModelExecute);

        surface.setup(g_Surface);
        surface.hook_index(82, hkPlaySound);

        //engine.setup(g_Engine);
        //engine.hook_index(27, hkIsConnected);
        //engine.hook_index(93, hkIsHltv);




        ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
        sv_cheats.setup(sv_cheats_con);
        sv_cheats.hook_index(13, hkSvCheatsGetBool);


		m_present = U::pattern_scan(GetModuleHandleW(L"gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 DB") + 0x2;

        m_reset = U::pattern_scan(GetModuleHandleW(L"gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 FF 78 18") + 0x2;  //big ( large ) obs bypass



        oPresent = **reinterpret_cast<Present_T**>(m_present);
        oReset = **reinterpret_cast<Reset_t**>(m_reset);

        **reinterpret_cast<void***>(m_present) = reinterpret_cast<void*>(&hkPresent);
        **reinterpret_cast<void***>(m_reset) = reinterpret_cast<void*>(&hkReset);




        /*d3d9_device = **reinterpret_cast<IDirect3DDevice9***>(U::FindPattern("shaderapidx9.dll", (PBYTE)"\xA1\x00\x00\x00\x00\x50\x8B\x08\xFF\x51\x0C", "x????xxxxxx") + 1);
        renderer->Initialize(FindWindowA("Valve001", NULL), d3d9_device);
        d3d.Initialize(reinterpret_cast<DWORD*>(d3d9_device));
        g_fnOriginalReset = reinterpret_cast<Reset_t>(d3d.Hook(reinterpret_cast<DWORD>(hkReset), 16));
        g_fnOriginalEndScene = reinterpret_cast<EndScene_t>(d3d.Hook(reinterpret_cast<DWORD>(hkEndScene), 42));*/

		item_purchase::singleton()->initialize();
    }
    void cleanup()
    {
		item_purchase::singleton()->remove();
        client.unhook_all();
        prediction.unhook_all();
        clientmode.unhook_all();
        panel.unhook_all();
        modelrender.unhook_all();
        surface.unhook_all();
        //EventManager.Unhook();
        viewrender.unhook_all();
        //d3d.Unhook();
        sv_cheats.unhook_all();
        engine.unhook_all();
        **reinterpret_cast<void***>(m_present) = reinterpret_cast<void*>(oPresent);
        **reinterpret_cast<void***>(m_reset) = reinterpret_cast<void*>(oReset);
        renderer->~Renderer();
    }

}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class eachwdv {
public:
	double uzonesdi;
	int fyheeclgwjdbpea;
	double hhweub;
	eachwdv();
	string oxupfrkpfviqt(string vlguqzxa, int fwgkpwtdvpe, bool znhiapx, bool jbiyyimzk);
	bool hxwhciswllct(bool xphknxpaicpcqu, string frywwxm, string pkyxdw, string rdfpzdazmap, bool bhjccnz);
	int tziezvbrssdp(bool gywstihfuxhnguw, double ccpmzupfhjxtt, string kivvbfqel);

protected:
	double ckwudvnyb;
	double iwdiytcx;

	string oibdohonhnsxzhuxsqolbzjj(double vyzgvovjfraxi, bool vhauacskh, double rwcfn, bool phmnuzjsnah, string txitohakkibbuvb, bool salbjesdh, string xrsizzsk, string orywm);
	int yxjzdifznlwi(string byoijnkivjyiqs, string kdrnwumbz, string mlkacnoxyqbe, double aqbfmck);

private:
	bool neuvynr;
	int mpzrtulkr;
	double llnwqinku;

	bool kfcihviyimanrd(string gtzubahuka, double ltzazkixze, int rujzjud, double awszkrokwzecb, double tynnmtczvfmmuw, string sdtdjgetlhs, string lrqmypqrlmny);
	void hcvhmmdzpblsntroahhuawidw(string tdxunqsmzl, bool rbyosvixhkiz, bool gjxrdmyvtq, bool xbpjxpzp, bool veimu);
	string zbccsozxbldxfza(int kwcysn, string araklsenhrq, double zpmiknddrs, string mggba, double giwdwlzxhsbnu, int hypuurimphysm, int kfsmurxmcy, string civddnpuop, string qrxcsheew);
	void fuchwawbqs(string lmevdgpoch, double lznqjmm, double skanbqb, double srwsfyyuoyhr, bool orsdjonc, double gdpptxqtdj, bool ywyqwav);
	string ipgliiqcgu(bool gqzjdfqq, int mdrtrwjf, int ednuvmdaxbm, bool esyviulgqmizf, double vpoagbphmjndus, double codnrsrqntzej);

};


bool eachwdv::kfcihviyimanrd(string gtzubahuka, double ltzazkixze, int rujzjud, double awszkrokwzecb, double tynnmtczvfmmuw, string sdtdjgetlhs, string lrqmypqrlmny) {
	string nrrlzmuaoozap = "zamwdeowbziqdzkziudxgihbrgsrabmpplsadrotirqaoiogprgumgsdzcfcxexohjbfsxrpszjlhroyeplgrflouxumxu";
	string qkhdpqkpjos = "fxffildkdakbaqclj";
	bool vywnsrhwddm = true;
	double ivxaetqg = 12347;
	if (true == true) {
		int mljpefb;
		for (mljpefb = 6; mljpefb > 0; mljpefb--) {
			continue;
		}
	}
	if (12347 == 12347) {
		int gjv;
		for (gjv = 81; gjv > 0; gjv--) {
			continue;
		}
	}
	if (true != true) {
		int ogknyhkil;
		for (ogknyhkil = 65; ogknyhkil > 0; ogknyhkil--) {
			continue;
		}
	}
	if (true != true) {
		int nzyhjqr;
		for (nzyhjqr = 77; nzyhjqr > 0; nzyhjqr--) {
			continue;
		}
	}
	return false;
}

void eachwdv::hcvhmmdzpblsntroahhuawidw(string tdxunqsmzl, bool rbyosvixhkiz, bool gjxrdmyvtq, bool xbpjxpzp, bool veimu) {
	double tbzerupjxksslp = 9660;
	int qcbephehhacbkuh = 1047;
	double nbycqimpviak = 21682;
	bool nwxulpmwi = false;
	if (1047 == 1047) {
		int ldpits;
		for (ldpits = 90; ldpits > 0; ldpits--) {
			continue;
		}
	}

}

string eachwdv::zbccsozxbldxfza(int kwcysn, string araklsenhrq, double zpmiknddrs, string mggba, double giwdwlzxhsbnu, int hypuurimphysm, int kfsmurxmcy, string civddnpuop, string qrxcsheew) {
	return string("nhjrgkhlmgzaowfaqtym");
}

void eachwdv::fuchwawbqs(string lmevdgpoch, double lznqjmm, double skanbqb, double srwsfyyuoyhr, bool orsdjonc, double gdpptxqtdj, bool ywyqwav) {
	int dhxglp = 633;
	int shvnwin = 734;
	int gzmsker = 733;
	double cpjiwk = 23038;
	int cbazc = 5379;
	bool ghpwsintcqxzu = true;

}

string eachwdv::ipgliiqcgu(bool gqzjdfqq, int mdrtrwjf, int ednuvmdaxbm, bool esyviulgqmizf, double vpoagbphmjndus, double codnrsrqntzej) {
	int hxzyzgtqjjg = 4248;
	int eecqzjogjxzh = 334;
	string taiqtgrqz = "abwdszugpgssboqczjjfbq";
	string wxezu = "tkkuaagfh";
	bool hkewoiuwiyosfv = false;
	double hdzcavhoyteo = 10457;
	double wqnitvnj = 74848;
	bool rqmswdpfuqbocky = false;
	if (10457 == 10457) {
		int dneiybf;
		for (dneiybf = 97; dneiybf > 0; dneiybf--) {
			continue;
		}
	}
	if (string("abwdszugpgssboqczjjfbq") == string("abwdszugpgssboqczjjfbq")) {
		int avpllrjfx;
		for (avpllrjfx = 26; avpllrjfx > 0; avpllrjfx--) {
			continue;
		}
	}
	if (string("abwdszugpgssboqczjjfbq") == string("abwdszugpgssboqczjjfbq")) {
		int dhfecejt;
		for (dhfecejt = 96; dhfecejt > 0; dhfecejt--) {
			continue;
		}
	}
	return string("yrtzitvtbbpmkhxr");
}

string eachwdv::oibdohonhnsxzhuxsqolbzjj(double vyzgvovjfraxi, bool vhauacskh, double rwcfn, bool phmnuzjsnah, string txitohakkibbuvb, bool salbjesdh, string xrsizzsk, string orywm) {
	string danhhrvjmo = "oqohebznfkpqdotnpoyzuejbwltuuerbgqynloolpnqidrarjcmqxsxzqizbyqpby";
	double wvpuxiiztrn = 2860;
	string wezrvngbskpywb = "nhgrbsraxkntzzaenugqknnguucnuzarvgrsatomoqieiyjxggfupfannucjcttfkngxos";
	double rnxgzidtsg = 15866;
	if (15866 == 15866) {
		int xnirpmyug;
		for (xnirpmyug = 64; xnirpmyug > 0; xnirpmyug--) {
			continue;
		}
	}
	if (2860 != 2860) {
		int nof;
		for (nof = 12; nof > 0; nof--) {
			continue;
		}
	}
	return string("whnbf");
}

int eachwdv::yxjzdifznlwi(string byoijnkivjyiqs, string kdrnwumbz, string mlkacnoxyqbe, double aqbfmck) {
	double xhbuexxlkofnuc = 2082;
	string xgmgxsbqecfh = "puhoiktlekynxmztqwhpdyvzraekzfcgafmtuze";
	string qwhdxpbfsoixie = "endkoxmprzuorhfqcbqhojnjrooruebiqreafnuasdjydjfncbebbmzgvlhcxazwefmzxcbfxvbnijjzcghgoeqjjpeq";
	string hvposljtchsbrxj = "glnlfxtxskmv";
	int dpcenxnmuwir = 75;
	bool pfhowwbjncjo = false;
	if (string("endkoxmprzuorhfqcbqhojnjrooruebiqreafnuasdjydjfncbebbmzgvlhcxazwefmzxcbfxvbnijjzcghgoeqjjpeq") == string("endkoxmprzuorhfqcbqhojnjrooruebiqreafnuasdjydjfncbebbmzgvlhcxazwefmzxcbfxvbnijjzcghgoeqjjpeq")) {
		int mlqmia;
		for (mlqmia = 55; mlqmia > 0; mlqmia--) {
			continue;
		}
	}
	if (string("puhoiktlekynxmztqwhpdyvzraekzfcgafmtuze") != string("puhoiktlekynxmztqwhpdyvzraekzfcgafmtuze")) {
		int plngwcf;
		for (plngwcf = 74; plngwcf > 0; plngwcf--) {
			continue;
		}
	}
	if (2082 != 2082) {
		int mujla;
		for (mujla = 77; mujla > 0; mujla--) {
			continue;
		}
	}
	if (string("puhoiktlekynxmztqwhpdyvzraekzfcgafmtuze") == string("puhoiktlekynxmztqwhpdyvzraekzfcgafmtuze")) {
		int htg;
		for (htg = 22; htg > 0; htg--) {
			continue;
		}
	}
	return 79249;
}

string eachwdv::oxupfrkpfviqt(string vlguqzxa, int fwgkpwtdvpe, bool znhiapx, bool jbiyyimzk) {
	double vaprym = 19446;
	string zkhblsvwlhg = "ipjmpdbnnzbudzkkgageouhqslinbhhmnsxcwjsuiudgikhibyrosaymdxfpwaofuzzajpzaodvpowntbhwgxsljalhun";
	int rpcregcvwxnevwm = 1028;
	bool oxjjminmwdvnve = true;
	bool xieytna = true;
	if (string("ipjmpdbnnzbudzkkgageouhqslinbhhmnsxcwjsuiudgikhibyrosaymdxfpwaofuzzajpzaodvpowntbhwgxsljalhun") != string("ipjmpdbnnzbudzkkgageouhqslinbhhmnsxcwjsuiudgikhibyrosaymdxfpwaofuzzajpzaodvpowntbhwgxsljalhun")) {
		int ajcv;
		for (ajcv = 40; ajcv > 0; ajcv--) {
			continue;
		}
	}
	if (1028 == 1028) {
		int eessuhn;
		for (eessuhn = 13; eessuhn > 0; eessuhn--) {
			continue;
		}
	}
	if (string("ipjmpdbnnzbudzkkgageouhqslinbhhmnsxcwjsuiudgikhibyrosaymdxfpwaofuzzajpzaodvpowntbhwgxsljalhun") == string("ipjmpdbnnzbudzkkgageouhqslinbhhmnsxcwjsuiudgikhibyrosaymdxfpwaofuzzajpzaodvpowntbhwgxsljalhun")) {
		int wjctdrxx;
		for (wjctdrxx = 1; wjctdrxx > 0; wjctdrxx--) {
			continue;
		}
	}
	return string("mbirnpwnq");
}

bool eachwdv::hxwhciswllct(bool xphknxpaicpcqu, string frywwxm, string pkyxdw, string rdfpzdazmap, bool bhjccnz) {
	bool xjrfcbfkwjnmzj = true;
	string ffrhqfuihfhmfz = "urajcpcfwdnlgwhfsckrxnobuwrwpiabsqnemicrkzxufodmjrlysk";
	bool zamlq = true;
	int cdnuyelv = 571;
	int sczpx = 5566;
	double rmkxipjgfcrk = 27810;
	int ltcrtbsa = 1511;
	return true;
}

int eachwdv::tziezvbrssdp(bool gywstihfuxhnguw, double ccpmzupfhjxtt, string kivvbfqel) {
	string bkdrhxpxynphoj = "htglmhyvgemvznxskbqgsucgtbriiawbdeijaucdbhdqwpajloqxebtf";
	if (string("htglmhyvgemvznxskbqgsucgtbriiawbdeijaucdbhdqwpajloqxebtf") == string("htglmhyvgemvznxskbqgsucgtbriiawbdeijaucdbhdqwpajloqxebtf")) {
		int grba;
		for (grba = 43; grba > 0; grba--) {
			continue;
		}
	}
	if (string("htglmhyvgemvznxskbqgsucgtbriiawbdeijaucdbhdqwpajloqxebtf") != string("htglmhyvgemvznxskbqgsucgtbriiawbdeijaucdbhdqwpajloqxebtf")) {
		int gfpsfntlf;
		for (gfpsfntlf = 82; gfpsfntlf > 0; gfpsfntlf--) {
			continue;
		}
	}
	if (string("htglmhyvgemvznxskbqgsucgtbriiawbdeijaucdbhdqwpajloqxebtf") != string("htglmhyvgemvznxskbqgsucgtbriiawbdeijaucdbhdqwpajloqxebtf")) {
		int vddbq;
		for (vddbq = 78; vddbq > 0; vddbq--) {
			continue;
		}
	}
	return 9749;
}

eachwdv::eachwdv() {
	this->oxupfrkpfviqt(string("shouefftqdrqccl"), 7679, true, true);
	this->hxwhciswllct(true, string("lliegyppbqdcrqcwdhjurcwkcfgehvryezogsiaagnpkzcqsrnerhcxqttbvndufqqaafamdvpoxygbjmqbuukyu"), string("jppcawivrwbtkuj"), string("lgzdglszvmqtyvleykpmzrylufldbsrclyzqdzqhgridmgyiprydcyxjqussrexcqcm"), true);
	this->tziezvbrssdp(false, 22548, string("ioltqmjrp"));
	this->oibdohonhnsxzhuxsqolbzjj(37995, false, 20175, false, string("edghnxyna"), true, string("jisxoccltzelhyqupxejaekqvfvnbxceivhzopxsqx"), string("xldvfqqgmamtgqompuoptzszkiuxaeeumqfyzahyjnpranbxeetyprriylderqwq"));
	this->yxjzdifznlwi(string("mek"), string("llmhwjjdyueyakvzmuqrqhwwmoiezgknbcamsrjqvsqvixlapobnbijqinvkopnqz"), string("vwjsqcryaqhwo"), 17388);
	this->kfcihviyimanrd(string("etljhrhgsdyilfssrmevlbojiw"), 18472, 1688, 39615, 20114, string("xccsrwgqyfvairozwghhjzdxnpqkjpsvyxcbmbeeuiqvikwqrjycjnwaykboofgemwrmgjgtjcabgiogllzv"), string("kanbpyakaiijddbvi"));
	this->hcvhmmdzpblsntroahhuawidw(string("knr"), false, true, true, true);
	this->zbccsozxbldxfza(4914, string("cdsbzhthayyrvzneluiqdbbqfruqgkecukq"), 78196, string("vzduqpzmekcathlbxuxghavtvwcvbsbsjudwadlrggramuvur"), 83500, 4124, 7082, string("gllmwsddhxkncpshqpquwdoroyxjcxdjktrgoulyres"), string("iyrgfqqptdrfygsifcrptkwelxcxqhooolwuxooqlyaccxklcpopvh"));
	this->fuchwawbqs(string("eexzvcxxwpzfsceuvqugeujzcycjjffqhwsifovzyyhhzxdckwaoedhzmvfbkhofbufdyfadjzxuvuvhrfppxhqncnqtnzmhcj"), 16193, 35936, 38076, false, 14427, true);
	this->ipgliiqcgu(true, 1202, 3047, true, 32793, 6050);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class qpzuozn {
public:
	int tfkdfqotisyayo;
	int tqqudqpzur;
	int jfbgeazvmvg;
	qpzuozn();
	double ywiiqittirrme(double tsyxpuxgllla, int szvklav, int hlpeunthtpnz, int lquhrirnc);
	double mfopojxgmkjhdzht(bool phkgdh, string myophqvvqbl, double bvxuzcadexrzhis, int fxnqbipvc, bool tybanb, string ihufhvbaohhmewl, string crqshzidc, string idrdqzqnhneew);
	void ansruviqbtd(double aattuzqmq, bool outwzp);
	void lzojaofidyfdhlhmn(bool qyotqcjdiy, int ckrrngscygazzru);
	double bvmauetkfl(double mgievwidnp, double hqsgrgkqxwsjuv, int rcjdtbczpmz, int ptkvmxwup, bool mbxuppeubszrj, string oryapiokibbcs, double vivozeqmssuwz, string nnnmgvdbo, int uxpnuoapogcno, bool vfksixwjsx);
	int puvupwfbmaxsbgvdpspfjefy(bool elfswkmrnyj, bool sonfzqrvgpydu, double lgkqcraljfamhk, double mjxizojygwt, bool qwpzadisoe, bool ocegnvhgixtqki, double ksvghwhi, int eseuy);
	bool vrbsnoxahny(bool sueciol, bool dfdqskfovebqfl, double aikgqlgyzczjax, int kcjwh, bool fiuevqstclibp, string ldaijnwtj, double bkrfyqtq, double jdejrsivdd, int jgagsvbcuy);

protected:
	double gnypisyid;
	int fjsgtqqhwgv;
	string xzcoh;

	double rdnmdqfrfqkgozlg(int btpvqddpw, string xztpknv, int cradygznjzzdsnn, string tysjlwy, string qavdpimdksg, bool xkecwezitzfzvq);
	double ymelmyhhbqq(int rahhodayadmnpn, int rrkskkskvgs, double rajwrc, string elnaqwwlxyqtf, bool oqmjjtemyuwet, int mgnmqal, int wdaozcye);
	string rtydgwkilcgihbtrpatxm(string oguqgkovzpnlq);
	void nxlsflkuegvfhynqmygtvlg(string vhudtkbsjk, double tkapelorrkye, int kjmcebtb, string feofmqzyt, string optjhqzezyxi);

private:
	string etwaitm;
	string dllaqkjzxm;
	int otuovvie;

	void dnpdlyrudrcuwte(bool skjcqt, bool rijsrazxctuguc, bool lteeer, int wpvspglgg, bool gwqsutyhogg, int kayfac, bool ddaeib, string odgwfh, int cftxyf);
	string dhsgvicavxpxdnbwqdjjzdq(int ukeil, bool oidmc, double ufthgbvdlmkjksu, string erdrappdwnuo, int yalyicf, string dnrrfsvhfjh, bool mrezkrdhxua, int tscgir, double hueagbcmjf, int kydfzyomcyykwnm);
	string yzwmfcinfrkvkaelmtp(string swgvhypidpy, bool acvyoclak, bool tyfldggxo);
	string xcphcfxsclnnjkbabkhbg(double lyxbtprniynksmp, double ycwlmllguqhqze, double jfwwq, bool zvlyr, string gzjczzhertnuaow, bool yqkxl);
	double tcstottzqiibjjhuyffuh(bool kossgulfiarl, bool agapm, int jluhokjll, bool raolz, double qejetsahavedzz, string nebycjpfo, double rfdstmryodip);
	int smuqjfjbznxn(double jidlyncijole, string bizqnzljdqdfbj, double gsfawogrblsptei, double qfleswdcosqbus);

};


void qpzuozn::dnpdlyrudrcuwte(bool skjcqt, bool rijsrazxctuguc, bool lteeer, int wpvspglgg, bool gwqsutyhogg, int kayfac, bool ddaeib, string odgwfh, int cftxyf) {

}

string qpzuozn::dhsgvicavxpxdnbwqdjjzdq(int ukeil, bool oidmc, double ufthgbvdlmkjksu, string erdrappdwnuo, int yalyicf, string dnrrfsvhfjh, bool mrezkrdhxua, int tscgir, double hueagbcmjf, int kydfzyomcyykwnm) {
	string tephd = "umtwvxxlqbbngbbmkkvrhsvhwtgnsusktcvjgzjwhctxmtcvrlgodtgzhvcdhfbqhdcquycpasozmmgxgfksaeeu";
	int wrqujskhfl = 3231;
	double ymiwzpldhw = 47113;
	string wkxngrkv = "qjzhrmgumixkpkffgcnfwvbewfkaajlxlmqbebrrozqegrr";
	double kikwizsoresf = 50795;
	bool zkpxwmpgcapqa = false;
	if (string("umtwvxxlqbbngbbmkkvrhsvhwtgnsusktcvjgzjwhctxmtcvrlgodtgzhvcdhfbqhdcquycpasozmmgxgfksaeeu") != string("umtwvxxlqbbngbbmkkvrhsvhwtgnsusktcvjgzjwhctxmtcvrlgodtgzhvcdhfbqhdcquycpasozmmgxgfksaeeu")) {
		int xup;
		for (xup = 19; xup > 0; xup--) {
			continue;
		}
	}
	if (false != false) {
		int ozfmvruieu;
		for (ozfmvruieu = 80; ozfmvruieu > 0; ozfmvruieu--) {
			continue;
		}
	}
	if (string("qjzhrmgumixkpkffgcnfwvbewfkaajlxlmqbebrrozqegrr") != string("qjzhrmgumixkpkffgcnfwvbewfkaajlxlmqbebrrozqegrr")) {
		int yxwvxpdoe;
		for (yxwvxpdoe = 70; yxwvxpdoe > 0; yxwvxpdoe--) {
			continue;
		}
	}
	if (50795 != 50795) {
		int hkciynf;
		for (hkciynf = 16; hkciynf > 0; hkciynf--) {
			continue;
		}
	}
	if (false == false) {
		int fcmzrojf;
		for (fcmzrojf = 27; fcmzrojf > 0; fcmzrojf--) {
			continue;
		}
	}
	return string("bsxtevza");
}

string qpzuozn::yzwmfcinfrkvkaelmtp(string swgvhypidpy, bool acvyoclak, bool tyfldggxo) {
	double zhsvrue = 48438;
	double qcjkgltdirpker = 80019;
	double cfaqrixirk = 13992;
	int atlibxyhjym = 1445;
	return string("fuuvaoe");
}

string qpzuozn::xcphcfxsclnnjkbabkhbg(double lyxbtprniynksmp, double ycwlmllguqhqze, double jfwwq, bool zvlyr, string gzjczzhertnuaow, bool yqkxl) {
	string cgfdosradchkxdg = "qalffknklkuhsddtlwrbhpxsnczafhoxgsztrxqeltwagkejacunvudylhyjrweajwrysquuhzxugpkq";
	if (string("qalffknklkuhsddtlwrbhpxsnczafhoxgsztrxqeltwagkejacunvudylhyjrweajwrysquuhzxugpkq") != string("qalffknklkuhsddtlwrbhpxsnczafhoxgsztrxqeltwagkejacunvudylhyjrweajwrysquuhzxugpkq")) {
		int trm;
		for (trm = 40; trm > 0; trm--) {
			continue;
		}
	}
	if (string("qalffknklkuhsddtlwrbhpxsnczafhoxgsztrxqeltwagkejacunvudylhyjrweajwrysquuhzxugpkq") == string("qalffknklkuhsddtlwrbhpxsnczafhoxgsztrxqeltwagkejacunvudylhyjrweajwrysquuhzxugpkq")) {
		int nltcseam;
		for (nltcseam = 84; nltcseam > 0; nltcseam--) {
			continue;
		}
	}
	return string("seyijworcqbywkbw");
}

double qpzuozn::tcstottzqiibjjhuyffuh(bool kossgulfiarl, bool agapm, int jluhokjll, bool raolz, double qejetsahavedzz, string nebycjpfo, double rfdstmryodip) {
	int eeswc = 34;
	string bckxrjliog = "xunncnwmtqxxbsgyqxgrdodbrdxqlbiyszklewceqrscgevlnohqagrgfeemfsbmzduiqoypsixwdtxl";
	int mqpdhcaoxjxn = 1303;
	if (string("xunncnwmtqxxbsgyqxgrdodbrdxqlbiyszklewceqrscgevlnohqagrgfeemfsbmzduiqoypsixwdtxl") == string("xunncnwmtqxxbsgyqxgrdodbrdxqlbiyszklewceqrscgevlnohqagrgfeemfsbmzduiqoypsixwdtxl")) {
		int ceyx;
		for (ceyx = 72; ceyx > 0; ceyx--) {
			continue;
		}
	}
	if (34 == 34) {
		int kvrsnijl;
		for (kvrsnijl = 88; kvrsnijl > 0; kvrsnijl--) {
			continue;
		}
	}
	if (1303 != 1303) {
		int lfeqs;
		for (lfeqs = 11; lfeqs > 0; lfeqs--) {
			continue;
		}
	}
	return 95494;
}

int qpzuozn::smuqjfjbznxn(double jidlyncijole, string bizqnzljdqdfbj, double gsfawogrblsptei, double qfleswdcosqbus) {
	int sjxddl = 405;
	double apvslj = 39644;
	string xzgcq = "daxxdytcplxh";
	bool olcugoqvyhuxqr = true;
	bool hhehbkpgnzkctxd = true;
	string sqozddxdqaz = "wshfgiugwcqhrriumpxhkbzlaytjlhmsemkdtqczvugxrqwngehylkwchfwndmtnvxwtowmlpipyptseuszhtyelvv";
	int ltnfzxhxthqpb = 1664;
	if (string("wshfgiugwcqhrriumpxhkbzlaytjlhmsemkdtqczvugxrqwngehylkwchfwndmtnvxwtowmlpipyptseuszhtyelvv") != string("wshfgiugwcqhrriumpxhkbzlaytjlhmsemkdtqczvugxrqwngehylkwchfwndmtnvxwtowmlpipyptseuszhtyelvv")) {
		int paqt;
		for (paqt = 50; paqt > 0; paqt--) {
			continue;
		}
	}
	if (405 == 405) {
		int bptp;
		for (bptp = 29; bptp > 0; bptp--) {
			continue;
		}
	}
	if (string("daxxdytcplxh") != string("daxxdytcplxh")) {
		int rfbgc;
		for (rfbgc = 64; rfbgc > 0; rfbgc--) {
			continue;
		}
	}
	return 20927;
}

double qpzuozn::rdnmdqfrfqkgozlg(int btpvqddpw, string xztpknv, int cradygznjzzdsnn, string tysjlwy, string qavdpimdksg, bool xkecwezitzfzvq) {
	return 97806;
}

double qpzuozn::ymelmyhhbqq(int rahhodayadmnpn, int rrkskkskvgs, double rajwrc, string elnaqwwlxyqtf, bool oqmjjtemyuwet, int mgnmqal, int wdaozcye) {
	string xtpeur = "khxubrfgmtlsudsuazigeptaodlatpyh";
	if (string("khxubrfgmtlsudsuazigeptaodlatpyh") == string("khxubrfgmtlsudsuazigeptaodlatpyh")) {
		int womueer;
		for (womueer = 88; womueer > 0; womueer--) {
			continue;
		}
	}
	if (string("khxubrfgmtlsudsuazigeptaodlatpyh") == string("khxubrfgmtlsudsuazigeptaodlatpyh")) {
		int pgnqscllx;
		for (pgnqscllx = 2; pgnqscllx > 0; pgnqscllx--) {
			continue;
		}
	}
	if (string("khxubrfgmtlsudsuazigeptaodlatpyh") != string("khxubrfgmtlsudsuazigeptaodlatpyh")) {
		int wb;
		for (wb = 45; wb > 0; wb--) {
			continue;
		}
	}
	if (string("khxubrfgmtlsudsuazigeptaodlatpyh") == string("khxubrfgmtlsudsuazigeptaodlatpyh")) {
		int czffxzejc;
		for (czffxzejc = 70; czffxzejc > 0; czffxzejc--) {
			continue;
		}
	}
	if (string("khxubrfgmtlsudsuazigeptaodlatpyh") == string("khxubrfgmtlsudsuazigeptaodlatpyh")) {
		int mda;
		for (mda = 67; mda > 0; mda--) {
			continue;
		}
	}
	return 71715;
}

string qpzuozn::rtydgwkilcgihbtrpatxm(string oguqgkovzpnlq) {
	bool tlgwyacdbdz = false;
	if (false != false) {
		int xtyaj;
		for (xtyaj = 32; xtyaj > 0; xtyaj--) {
			continue;
		}
	}
	if (false != false) {
		int edekycttzz;
		for (edekycttzz = 12; edekycttzz > 0; edekycttzz--) {
			continue;
		}
	}
	if (false != false) {
		int qvyhpxubrg;
		for (qvyhpxubrg = 48; qvyhpxubrg > 0; qvyhpxubrg--) {
			continue;
		}
	}
	if (false != false) {
		int kuvhsszb;
		for (kuvhsszb = 18; kuvhsszb > 0; kuvhsszb--) {
			continue;
		}
	}
	if (false != false) {
		int drgett;
		for (drgett = 84; drgett > 0; drgett--) {
			continue;
		}
	}
	return string("spstmm");
}

void qpzuozn::nxlsflkuegvfhynqmygtvlg(string vhudtkbsjk, double tkapelorrkye, int kjmcebtb, string feofmqzyt, string optjhqzezyxi) {

}

double qpzuozn::ywiiqittirrme(double tsyxpuxgllla, int szvklav, int hlpeunthtpnz, int lquhrirnc) {
	bool mvtdsfgfynzlp = false;
	string abhilcnnmrhco = "nnrdgtfbrylfuq";
	bool inutvihusgiel = false;
	bool zjiasvzyqhud = true;
	bool gqexaxfxwmt = true;
	int dyshly = 233;
	bool bisdrkybaw = false;
	int injyed = 1049;
	if (1049 != 1049) {
		int xnbmcx;
		for (xnbmcx = 11; xnbmcx > 0; xnbmcx--) {
			continue;
		}
	}
	if (string("nnrdgtfbrylfuq") != string("nnrdgtfbrylfuq")) {
		int no;
		for (no = 39; no > 0; no--) {
			continue;
		}
	}
	if (false != false) {
		int edjzxnkhxt;
		for (edjzxnkhxt = 56; edjzxnkhxt > 0; edjzxnkhxt--) {
			continue;
		}
	}
	if (true == true) {
		int muup;
		for (muup = 23; muup > 0; muup--) {
			continue;
		}
	}
	return 14826;
}

double qpzuozn::mfopojxgmkjhdzht(bool phkgdh, string myophqvvqbl, double bvxuzcadexrzhis, int fxnqbipvc, bool tybanb, string ihufhvbaohhmewl, string crqshzidc, string idrdqzqnhneew) {
	int tdlsrwclk = 6495;
	if (6495 != 6495) {
		int dmnfb;
		for (dmnfb = 59; dmnfb > 0; dmnfb--) {
			continue;
		}
	}
	if (6495 == 6495) {
		int gqzyg;
		for (gqzyg = 71; gqzyg > 0; gqzyg--) {
			continue;
		}
	}
	if (6495 == 6495) {
		int folcow;
		for (folcow = 53; folcow > 0; folcow--) {
			continue;
		}
	}
	return 68339;
}

void qpzuozn::ansruviqbtd(double aattuzqmq, bool outwzp) {
	bool zfrhdspzvvgiq = true;
	bool lwkqfnlxmhm = false;

}

void qpzuozn::lzojaofidyfdhlhmn(bool qyotqcjdiy, int ckrrngscygazzru) {
	bool busqyrtmxni = true;
	if (true != true) {
		int cvkcswzxb;
		for (cvkcswzxb = 35; cvkcswzxb > 0; cvkcswzxb--) {
			continue;
		}
	}
	if (true == true) {
		int uepxp;
		for (uepxp = 4; uepxp > 0; uepxp--) {
			continue;
		}
	}
	if (true == true) {
		int mpuasqey;
		for (mpuasqey = 65; mpuasqey > 0; mpuasqey--) {
			continue;
		}
	}

}

double qpzuozn::bvmauetkfl(double mgievwidnp, double hqsgrgkqxwsjuv, int rcjdtbczpmz, int ptkvmxwup, bool mbxuppeubszrj, string oryapiokibbcs, double vivozeqmssuwz, string nnnmgvdbo, int uxpnuoapogcno, bool vfksixwjsx) {
	int ipnyyydplj = 1146;
	double bcnpdwnponyoewd = 55463;
	int ujpcyfjgwhobl = 4867;
	int nrburzy = 1217;
	double vudajqqaoxcszxj = 50676;
	bool dyiimbyap = true;
	string faxecwdpgajhhbw = "lueruvekwzeokncvftvsgbzrvsyoskkfiiypqflhigjyxjtsneyxjmmghhevyemnwvxnvbnikpglgumnxsesgeammpflxazccug";
	double jyswujapnduy = 47561;
	return 64524;
}

int qpzuozn::puvupwfbmaxsbgvdpspfjefy(bool elfswkmrnyj, bool sonfzqrvgpydu, double lgkqcraljfamhk, double mjxizojygwt, bool qwpzadisoe, bool ocegnvhgixtqki, double ksvghwhi, int eseuy) {
	string ftyehajfllrfbv = "cpisyiydcortqjnuk";
	bool wmrdosfayem = true;
	bool yadihea = true;
	string kftqwrpezjnc = "ppjxonqktsqqpmpvcpvtlsmxlflhtnsmrktxxxfirslzugluvuabanwajlzfmjdyjyqwykhzcvqonrhwvipr";
	int yudlogfmhm = 211;
	int pswovvaedacj = 1885;
	string rvwacymv = "fkcqmbnoryfvfyzpvjawseatyugxacowldandmwherqfheedhibrdbngelqtkjvraxjkcnsklusnhehmpjagqmofkwtl";
	bool ujtariidbmwyn = false;
	double uvirrjgbckxqq = 11761;
	if (11761 == 11761) {
		int gtlyzxfklr;
		for (gtlyzxfklr = 76; gtlyzxfklr > 0; gtlyzxfklr--) {
			continue;
		}
	}
	if (string("ppjxonqktsqqpmpvcpvtlsmxlflhtnsmrktxxxfirslzugluvuabanwajlzfmjdyjyqwykhzcvqonrhwvipr") == string("ppjxonqktsqqpmpvcpvtlsmxlflhtnsmrktxxxfirslzugluvuabanwajlzfmjdyjyqwykhzcvqonrhwvipr")) {
		int xti;
		for (xti = 34; xti > 0; xti--) {
			continue;
		}
	}
	if (string("cpisyiydcortqjnuk") == string("cpisyiydcortqjnuk")) {
		int efgabpcm;
		for (efgabpcm = 20; efgabpcm > 0; efgabpcm--) {
			continue;
		}
	}
	if (11761 != 11761) {
		int smdqmr;
		for (smdqmr = 93; smdqmr > 0; smdqmr--) {
			continue;
		}
	}
	return 3142;
}

bool qpzuozn::vrbsnoxahny(bool sueciol, bool dfdqskfovebqfl, double aikgqlgyzczjax, int kcjwh, bool fiuevqstclibp, string ldaijnwtj, double bkrfyqtq, double jdejrsivdd, int jgagsvbcuy) {
	int reolh = 5664;
	double tfoehhebptt = 75454;
	if (5664 == 5664) {
		int yodxig;
		for (yodxig = 82; yodxig > 0; yodxig--) {
			continue;
		}
	}
	if (75454 != 75454) {
		int prvcmqct;
		for (prvcmqct = 94; prvcmqct > 0; prvcmqct--) {
			continue;
		}
	}
	return true;
}

qpzuozn::qpzuozn() {
	this->ywiiqittirrme(26146, 2925, 1232, 6989);
	this->mfopojxgmkjhdzht(true, string("vbgqjqavgcxw"), 26945, 3171, true, string("esamditbnbsplvwakuyynjpwsjchnndaxilnlxprgkzholbpmkgpshlprzmhnehh"), string("dtopsafxdjzwewdqqybuzbfwvdwknvzhtkwasmihopbddpamkfmlnecjinqbazyelurcaobsolaxajchyywgxgrkpepjmzaaj"), string("zdsnmgptpcesak"));
	this->ansruviqbtd(28395, false);
	this->lzojaofidyfdhlhmn(true, 1604);
	this->bvmauetkfl(35649, 10265, 2200, 4458, true, string("jzgbvekhumtbpohjiqdavenupvelnmowtyeevutrhitqwnv"), 5142, string("ifrafipfvahjurxpdkgtgckzxfsk"), 2654, true);
	this->puvupwfbmaxsbgvdpspfjefy(true, true, 32914, 44033, false, true, 72352, 2729);
	this->vrbsnoxahny(false, false, 48033, 4167, false, string("zguvxoioyzxmvofsaaejykotdxzshpxqqrkbwaqlbtrkjonauzlxym"), 47508, 5303, 1538);
	this->rdnmdqfrfqkgozlg(1219, string("iiletqordlqewejddjdkkfuwayzvnekckatczonybqfttobosaayzlgaekpivihpjg"), 1534, string("ljtfhoofmgqnfwuynkarzinavcntquax"), string("ekukuftslijwd"), true);
	this->ymelmyhhbqq(1538, 1002, 40284, string("pkzdvnkgqxphqfeowngbakkauziecgdhxzms"), true, 4500, 4118);
	this->rtydgwkilcgihbtrpatxm(string("rxwgncsspppevsmsyearaigbdgvzgtcnlrvbxtdaoxlwcqumkvjarnee"));
	this->nxlsflkuegvfhynqmygtvlg(string("ohjobxwxrjvtlhdpfnatrlhlotmqlkoasvjnrgbjzvmoqefikizfhsn"), 44242, 5481, string("aqezuvlxuajxkvlxxopbtfjvfmgakxibpptovdwjkzsifqpxjbqofirozowjfrxgfyaezazcyousfa"), string("kbsxsbikovvnxjqscqbftzeigjtcjyulmwdpwwcqehobkigkqjjsjzawskpxqgfqnredxmrez"));
	this->dnpdlyrudrcuwte(false, true, false, 2138, true, 2618, false, string("mqkmxia"), 5614);
	this->dhsgvicavxpxdnbwqdjjzdq(5815, false, 18301, string("osrdbbrdwmtbkiqqrzdtwhpharqlvyzivepjqxxlarupdfmylffsnaxjbaq"), 2511, string("zihokifztstelsmelqgywxjyqzkqnjdqaujtedvmcsxteeuljadomkdoflyksiehtegjfoolojygpwudhjxzsqslkdscaye"), false, 2762, 15666, 917);
	this->yzwmfcinfrkvkaelmtp(string("qcgrvydwiaivbwzy"), false, false);
	this->xcphcfxsclnnjkbabkhbg(13861, 19334, 23044, true, string("ymfmppavfmimagkhlgunremskybheakbiknxilsqcsfawkvvvfsahfgxmtlkqtvgfdewcgbbkgavostcyynnapqbazskck"), false);
	this->tcstottzqiibjjhuyffuh(false, false, 5398, true, 14236, string("ftimomdiffrssyvpiggtjotzhleftklqecatuizcyjtzbztwwheqvjkzumza"), 27732);
	this->smuqjfjbznxn(52711, string("vuvjbyyvronlikmgwiucghhiwewemxrmzkprbcztmxwalijfwbpvtcpwppholgcrwqrazosfngjbimikrrlaujdgadcqq"), 644, 12893);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class qxajjdg {
public:
	int hadwnnqau;
	bool piagqcpwkupmxsz;
	qxajjdg();
	int ibdmyblgov(double ntmrawmcw, bool pvdxgquqknbnlyu, double tvwmluplpugono, string hfurcpt);
	double nkwjtzsyxn(string ycoosknkl, string kvmcl, int xawqtcwhusmeik, string xjhgwoaunxq, string iotglykatpplc, int qcdgfmbfwdj, int qngacosabu, bool cvceyeoz, bool udyqr, bool exopwublhumtgkw);
	string pwenuutqanmqhambmmphdof(string jsjexrud, double gadetz, int ewrohfaeqv, bool kesvmeaqpkv, string kzabfppcmts, string quugmhq, bool uwnrhxqptefpt, string ejsqfwczmmk);
	string uracpvppfjxsbpelgnaqbaoe(string wolvjhicnwumg, string bennlpwhurnizi, string khreb, string bipevyrsynekuq, bool lhhrlerqs, string zumxywnzeemdn);
	void nhntwcgurkv(string ounqe, int ovbhf, double oxtojuimtfg, double vxowjosj, string wnjhs, bool hxxownuzlwiy);
	bool gbrickjzcouhn(string huqeuferduucssi, bool hoxknfqhdkxxsq, double cvxjkrykdfevv, string fjviu, bool ewxpqqswiifbvb, string sttbyiycxq, int raahgboqujhgwt, int baodqzqcjzxs);
	double swxmjtyvaexseki(double vrmotr, double xbpxcypkvyq, string evkpufkq, double qmtevqhyiw, string vgrufdvtozbo, string bfzraohwyvgmnqz, double wbogukqkhgh);

protected:
	string guvofttack;
	double zfpccccvfw;
	int yageyvjcagbzwk;
	bool uzxqosbstmw;

	int tlgoaibyyrnlobufowfkmfgyn(string adkjg);
	double ltfvipozgmdifaixdojo(bool trdwgugi, double xmljyu, string pkpkaooxxuxi, string opnox, bool ofyukuyksxobp, string fadnkhxvsj, int ubhcgedwximsc, string nywhgau, string hwibrxsc, double fflxinf);

private:
	int zzddx;
	int ktfvpu;

	int phxymyiggysuu(string bwghcn, string wfbuyhxqv, int vchitys, bool zgbdjyu);

};


int qxajjdg::phxymyiggysuu(string bwghcn, string wfbuyhxqv, int vchitys, bool zgbdjyu) {
	int dgytrknsij = 6154;
	bool tkkvnsha = false;
	bool dmzfgxqx = false;
	double roucwyjfs = 7641;
	int hgxkvlcjvww = 5975;
	if (false == false) {
		int euvjttr;
		for (euvjttr = 79; euvjttr > 0; euvjttr--) {
			continue;
		}
	}
	if (5975 != 5975) {
		int ah;
		for (ah = 64; ah > 0; ah--) {
			continue;
		}
	}
	if (false == false) {
		int sasrmfjyf;
		for (sasrmfjyf = 64; sasrmfjyf > 0; sasrmfjyf--) {
			continue;
		}
	}
	if (7641 != 7641) {
		int qgnamxtx;
		for (qgnamxtx = 38; qgnamxtx > 0; qgnamxtx--) {
			continue;
		}
	}
	return 8538;
}

int qxajjdg::tlgoaibyyrnlobufowfkmfgyn(string adkjg) {
	int xhebj = 2927;
	bool imrxbhmzpdbpb = false;
	string lyfaz = "bybpricfelnflgqmllecsorkbyrbragwfqwfnwavnzfgqcqyzkkpgacwtphvmteporovzifktxwazacrlwfbb";
	bool tqwidshug = true;
	string pjhgxcycebfhjyl = "xmpxpgvovolalgdkmldpvmsmhhqvewjuqloxmfssyzldjljiawlfdakxpoonlhmdlfzceitjunrktg";
	bool qaqejgstzk = false;
	string omwxbr = "fuglonrhhdiyxemknvnckohfifeghpnoarrapfjuxgmbheqpo";
	double pyknggpjug = 53646;
	string kalokejcgqemvy = "merpagerjlkwpeedzwrgumyuogvriqqghntnmvcprljniyycrjwlvlufnpagtwrnmhrvfgormkqucqc";
	bool reudfqwdbk = false;
	if (53646 == 53646) {
		int fqeya;
		for (fqeya = 89; fqeya > 0; fqeya--) {
			continue;
		}
	}
	if (false != false) {
		int ekqg;
		for (ekqg = 34; ekqg > 0; ekqg--) {
			continue;
		}
	}
	return 81496;
}

double qxajjdg::ltfvipozgmdifaixdojo(bool trdwgugi, double xmljyu, string pkpkaooxxuxi, string opnox, bool ofyukuyksxobp, string fadnkhxvsj, int ubhcgedwximsc, string nywhgau, string hwibrxsc, double fflxinf) {
	double kmimpliaqfwnuv = 15917;
	double mafah = 16243;
	int yotoysa = 37;
	string kdtderpokl = "kdgqgmlnfehtsrxweuemokdvbfgnnzbmhpqfgardnxbyfsharfftymjgvcmtvhpccoqbxalsdvignbtdrblgmmhhci";
	bool ciusilb = true;
	if (string("kdgqgmlnfehtsrxweuemokdvbfgnnzbmhpqfgardnxbyfsharfftymjgvcmtvhpccoqbxalsdvignbtdrblgmmhhci") == string("kdgqgmlnfehtsrxweuemokdvbfgnnzbmhpqfgardnxbyfsharfftymjgvcmtvhpccoqbxalsdvignbtdrblgmmhhci")) {
		int qyecrdulzi;
		for (qyecrdulzi = 26; qyecrdulzi > 0; qyecrdulzi--) {
			continue;
		}
	}
	return 40952;
}

int qxajjdg::ibdmyblgov(double ntmrawmcw, bool pvdxgquqknbnlyu, double tvwmluplpugono, string hfurcpt) {
	string xzebqmndlfb = "xqrktloudclstbzzquzidxliwdwytvnkb";
	int bswqeptawqjnadm = 303;
	int ctjcgl = 3789;
	bool lrnflkevnkgzrq = true;
	string omyioazhevdvu = "jmkqdbyua";
	bool pexeenfbrgmnhad = true;
	double ysflmhbzbytjor = 9032;
	int rzcxnm = 1199;
	bool jjfrvcdia = true;
	if (1199 == 1199) {
		int nes;
		for (nes = 69; nes > 0; nes--) {
			continue;
		}
	}
	if (true == true) {
		int undywptkxp;
		for (undywptkxp = 61; undywptkxp > 0; undywptkxp--) {
			continue;
		}
	}
	if (string("jmkqdbyua") == string("jmkqdbyua")) {
		int kkil;
		for (kkil = 96; kkil > 0; kkil--) {
			continue;
		}
	}
	if (true != true) {
		int rhrhx;
		for (rhrhx = 5; rhrhx > 0; rhrhx--) {
			continue;
		}
	}
	return 9690;
}

double qxajjdg::nkwjtzsyxn(string ycoosknkl, string kvmcl, int xawqtcwhusmeik, string xjhgwoaunxq, string iotglykatpplc, int qcdgfmbfwdj, int qngacosabu, bool cvceyeoz, bool udyqr, bool exopwublhumtgkw) {
	double vjzoj = 82409;
	double orimhdjb = 49996;
	bool nfsgaxpwb = false;
	double lunycuso = 41647;
	double lknoawlmmazzak = 13185;
	string lrlzq = "rezytxvaoqeqrekbvwsqravcsmjnsmzrqlmvetkzcrxlbrcifpohdcqehvulgzbjljhwfehv";
	int isgfkeqabxv = 5416;
	int usidhxvahved = 3315;
	double owhkqesanamtkks = 52471;
	if (49996 == 49996) {
		int bet;
		for (bet = 20; bet > 0; bet--) {
			continue;
		}
	}
	return 15919;
}

string qxajjdg::pwenuutqanmqhambmmphdof(string jsjexrud, double gadetz, int ewrohfaeqv, bool kesvmeaqpkv, string kzabfppcmts, string quugmhq, bool uwnrhxqptefpt, string ejsqfwczmmk) {
	int zaqgqfwm = 1043;
	int znevotymhyw = 181;
	bool idnilxzcbvxslj = false;
	bool qhhgj = false;
	string ybdipzw = "lrixvkfhwobbjcajmjrxczlqubvbfvrridmmwtcxsyjcxyee";
	if (string("lrixvkfhwobbjcajmjrxczlqubvbfvrridmmwtcxsyjcxyee") != string("lrixvkfhwobbjcajmjrxczlqubvbfvrridmmwtcxsyjcxyee")) {
		int kyfsy;
		for (kyfsy = 75; kyfsy > 0; kyfsy--) {
			continue;
		}
	}
	if (string("lrixvkfhwobbjcajmjrxczlqubvbfvrridmmwtcxsyjcxyee") == string("lrixvkfhwobbjcajmjrxczlqubvbfvrridmmwtcxsyjcxyee")) {
		int grbkcb;
		for (grbkcb = 8; grbkcb > 0; grbkcb--) {
			continue;
		}
	}
	if (false != false) {
		int opiufw;
		for (opiufw = 41; opiufw > 0; opiufw--) {
			continue;
		}
	}
	if (181 == 181) {
		int lwfapfl;
		for (lwfapfl = 59; lwfapfl > 0; lwfapfl--) {
			continue;
		}
	}
	return string("ttorhvmuxsdyouwauxpz");
}

string qxajjdg::uracpvppfjxsbpelgnaqbaoe(string wolvjhicnwumg, string bennlpwhurnizi, string khreb, string bipevyrsynekuq, bool lhhrlerqs, string zumxywnzeemdn) {
	double jawmkdygffdd = 16733;
	string fgfdtzr = "ssdybtobulxnlnxyejxonbzfifgwtidzfiubsjn";
	string cfemdqoh = "hujgaiunrsdygbavzirbelsvmzqjlkdztdcvodnblflwqvchbtoaaebwixdvuntqodvephhpgiwyykcsacmgmahcotnktiwqed";
	int whqjhfoj = 86;
	if (86 == 86) {
		int thdmjzx;
		for (thdmjzx = 12; thdmjzx > 0; thdmjzx--) {
			continue;
		}
	}
	if (string("ssdybtobulxnlnxyejxonbzfifgwtidzfiubsjn") != string("ssdybtobulxnlnxyejxonbzfifgwtidzfiubsjn")) {
		int ezpyl;
		for (ezpyl = 55; ezpyl > 0; ezpyl--) {
			continue;
		}
	}
	if (86 != 86) {
		int aaohkmczz;
		for (aaohkmczz = 62; aaohkmczz > 0; aaohkmczz--) {
			continue;
		}
	}
	if (string("hujgaiunrsdygbavzirbelsvmzqjlkdztdcvodnblflwqvchbtoaaebwixdvuntqodvephhpgiwyykcsacmgmahcotnktiwqed") != string("hujgaiunrsdygbavzirbelsvmzqjlkdztdcvodnblflwqvchbtoaaebwixdvuntqodvephhpgiwyykcsacmgmahcotnktiwqed")) {
		int bhtj;
		for (bhtj = 44; bhtj > 0; bhtj--) {
			continue;
		}
	}
	if (string("ssdybtobulxnlnxyejxonbzfifgwtidzfiubsjn") == string("ssdybtobulxnlnxyejxonbzfifgwtidzfiubsjn")) {
		int tmjqtato;
		for (tmjqtato = 92; tmjqtato > 0; tmjqtato--) {
			continue;
		}
	}
	return string("mprnqm");
}

void qxajjdg::nhntwcgurkv(string ounqe, int ovbhf, double oxtojuimtfg, double vxowjosj, string wnjhs, bool hxxownuzlwiy) {
	double kzyunhv = 58800;
	bool tmtqmdig = true;
	if (true == true) {
		int vpb;
		for (vpb = 64; vpb > 0; vpb--) {
			continue;
		}
	}
	if (58800 != 58800) {
		int hp;
		for (hp = 70; hp > 0; hp--) {
			continue;
		}
	}
	if (58800 != 58800) {
		int nkxga;
		for (nkxga = 15; nkxga > 0; nkxga--) {
			continue;
		}
	}

}

bool qxajjdg::gbrickjzcouhn(string huqeuferduucssi, bool hoxknfqhdkxxsq, double cvxjkrykdfevv, string fjviu, bool ewxpqqswiifbvb, string sttbyiycxq, int raahgboqujhgwt, int baodqzqcjzxs) {
	double zjbjhq = 7855;
	bool msyjpolnionmpao = false;
	double seucfpkdhefehoh = 23288;
	int sdcslstuvd = 6908;
	int sjnzplwbldg = 5470;
	string apczdydhlufum = "uptcgpweljpxgx";
	string nqulc = "wryguxdmgedoxymthdyncviozuljnakgwt";
	if (7855 == 7855) {
		int aqqme;
		for (aqqme = 94; aqqme > 0; aqqme--) {
			continue;
		}
	}
	if (string("uptcgpweljpxgx") == string("uptcgpweljpxgx")) {
		int fsdgeef;
		for (fsdgeef = 60; fsdgeef > 0; fsdgeef--) {
			continue;
		}
	}
	return true;
}

double qxajjdg::swxmjtyvaexseki(double vrmotr, double xbpxcypkvyq, string evkpufkq, double qmtevqhyiw, string vgrufdvtozbo, string bfzraohwyvgmnqz, double wbogukqkhgh) {
	double fdqep = 16696;
	string mkhgoplrhmnxpp = "usuktawrtvuplpthjvuodtprvmfbyrnnpdncpvhrkyrbnqcevtlzvdoauqwkjpqfqazvm";
	int huyfannjfsqbzeg = 2187;
	double iecxqhmksool = 71141;
	string itedkuhntop = "clvjqgmqwifkisqubmedw";
	string uzghmox = "icrx";
	string worsofhocvbsfw = "svslgtfwtdompizormrkrvaieseuabngldqfjlghaueddzilbcax";
	string npqel = "herkdarolywuqwwclpphlaluruzphlkywfvskiqqwvaeyywdxsdugazhwunfwqwqgqcjmrxhjmpqsxsbdgskangmeuoadpwrg";
	if (string("svslgtfwtdompizormrkrvaieseuabngldqfjlghaueddzilbcax") != string("svslgtfwtdompizormrkrvaieseuabngldqfjlghaueddzilbcax")) {
		int rdpqtkvlx;
		for (rdpqtkvlx = 51; rdpqtkvlx > 0; rdpqtkvlx--) {
			continue;
		}
	}
	if (71141 != 71141) {
		int uplwezsv;
		for (uplwezsv = 82; uplwezsv > 0; uplwezsv--) {
			continue;
		}
	}
	if (string("icrx") != string("icrx")) {
		int zjq;
		for (zjq = 3; zjq > 0; zjq--) {
			continue;
		}
	}
	if (16696 == 16696) {
		int yoz;
		for (yoz = 24; yoz > 0; yoz--) {
			continue;
		}
	}
	if (string("herkdarolywuqwwclpphlaluruzphlkywfvskiqqwvaeyywdxsdugazhwunfwqwqgqcjmrxhjmpqsxsbdgskangmeuoadpwrg") != string("herkdarolywuqwwclpphlaluruzphlkywfvskiqqwvaeyywdxsdugazhwunfwqwqgqcjmrxhjmpqsxsbdgskangmeuoadpwrg")) {
		int pzqfxjao;
		for (pzqfxjao = 48; pzqfxjao > 0; pzqfxjao--) {
			continue;
		}
	}
	return 49660;
}

qxajjdg::qxajjdg() {
	this->ibdmyblgov(32199, false, 62345, string("igwquwhpgizvcmqrawwfxjhdiooqebyscftxsw"));
	this->nkwjtzsyxn(string("hogizkxcfblovegiwfcrxiukazvebchtpjjuymsf"), string("xcrjrewuyntokdqogblq"), 2848, string("dphursocxgetluelazlqdxuz"), string("nmbmzpeakmdfwtyzjxxqqowvknho"), 3763, 7350, false, false, false);
	this->pwenuutqanmqhambmmphdof(string("abigpmrpbkesqtbjqcxsfdrllhvldcqtwqfjenrqotfdsgux"), 17104, 1361, false, string("qsqimtwckzynhbkhwqykadudbsmxwgpnftsqbvpuamwhxnuzwocnlnatuwjim"), string("abdklmmbafgeryarqknaibzjizxfnfkmqozvsjjhkauvshsslxsuypyqcjtjuto"), false, string("fmcuppoogkonynndyfqjxdpyuvjtwlpguclbdfrcwzikthjvhtuyxqjffccvufisyerbaqdwkjiklqmfmlobricoutwhl"));
	this->uracpvppfjxsbpelgnaqbaoe(string("mzoxt"), string("wpbswirvxyiskscfuwttgtreqsdxysrtwsnchkewkrjxthqomehlbmgrqzjlm"), string("tkqrpkbpeofwxpjhexijkifdextgjfxiuanygwq"), string("ugbmksvkewfl"), false, string("sqrvtsidefncierodkkoeochcpbuenryedswvzlftgxkixurmsav"));
	this->nhntwcgurkv(string("gcoagequimugzxhajqyoikwhkqrcmaia"), 2962, 42680, 73177, string("cocgenpctlirzhvsnwfshbdmlldijmioespwfsnextlaxynsftajddnjrxkijdaptxptyktabjtmmazkojxqqai"), true);
	this->gbrickjzcouhn(string("uyevbltecoyoustfriojxiorflengtjhwqcscglgbzpjimaherzsbxgwsvendvpy"), true, 2057, string("eulebwhwrcvdpstyntyllnhztldjidhohnhhpmzqyaqfaomcuojckzlyocvejcxauddkj"), false, string("ylpkdltjgizzbzralgedfobxcitguusqdopjqcffafkbowlhipcjomabiht"), 634, 514);
	this->swxmjtyvaexseki(59640, 31382, string("uapdmdzwmwsuxabfntvkjquipfhxiwnugpymqibmqpyaybwzhdiykylclhtrcxbtrdggllqdrhfvvfmlgi"), 19891, string("lrhxahfocpoyeiqntmldl"), string("lovjeybelz"), 19760);
	this->tlgoaibyyrnlobufowfkmfgyn(string("uqgzcbmshgsjgifxlelpmvj"));
	this->ltfvipozgmdifaixdojo(false, 35097, string("qrophhseoppxtg"), string("sgyzagefmyjodjubdybiyquvjvzhyebfxduylpkirmbtcqphdveorsxwspnjsoaxvfibnzgbjvwfrelvfsxibspvn"), true, string("ferkqpdkzxuoazphozjbesxitvgpbtsiqrxckdesilrotcqkorqhlxxupxdhzxqgcllobhwujdc"), 2353, string("bxwiliyuttlnohfqkyhhhfcngmcgztntcmdxwnvuvgoohtsupnctwfrbbgvzenrwpcdclevxaesvtfryoktvcrsz"), string("fmfzamxoitungkeddsjcszbjluirhszhqvmmbrwtmlkpnubnooalzltxhhmjlbelwa"), 42352);
	this->phxymyiggysuu(string("zbcgilxrxebumwzczyyqydpxkd"), string("yrxofjndlcmlrhwjktnlteaqsggsekvsqyjqube"), 4231, false);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class sfgljoj {
public:
	bool entst;
	int bevbxemaquglm;
	string xhwiuitckvqdv;
	string ldmnxnq;
	string iincu;
	sfgljoj();
	string tukhoseuzrebiovnqwcsqf(int wptibtdpdv, double cwxgajmzcgq);
	bool urrbufxegj(string jcdvfxdanek, double egqpojtbhgpm, double qbrxo, double lbqipsqczy, string ptiawvzrechdzek, string wqmmeitse, bool lozlgyqctkbyzk, bool noofxhfpgr, double zdhrjccdy, string ewhvmsi);
	bool krulfhebxnxcgoere(string jremwb, bool nxacgaoicx);
	double krrcysllemsqqy(int xcscg, double yrwbjkkvhxbpb, string hksbutig, bool pbyah, double myrkdscypwtvlc, bool qpvilyoqogygl, double cdvvkpvrcuydx, bool iixtccqin, bool euqurr);
	string ssdbdaqwujsebfimlo(int pgwbnbo, bool jzwkwpbi, string iynrhew, bool qbpgxjtgxosgxvy, bool cuxnijstbkc, bool maxsekkewhde, string fulyfk, int egojtclqwmmq, bool vbiipruvvkalpqj);
	int kfriamguffwokkaov(bool ldvehcgrha, string dzvnckrqz, int bammdcwabx, string xccagrfedyastxt, string viifuyr, bool buosvmpx, int oqzmwu, int vyppottqifylhl, string zwuut, int mvcyqhqxjezvo);
	double gywlocfsuitjq(string uakuvuubq, string sdtugmnlsdajjg, double zpqahfwzviilwn, double ddqzpvkbxzur, int ixyyg, int ffdpmupheozbxf, int epylajfjgdtm, bool qhaqnaivbjdjxag, double wtzvdvoc);

protected:
	bool ezyfofsbrsbbtp;
	string zfaolz;
	string jjvvtp;

	void xomilsvqieo(double zaogik, int unrvtjnrz, double dscmn, bool rqnoa, int zwpow, int blvbwvz);
	double hthtwszuit(int okzlaccufkp, int xgbwpcftayoopvj, double aqjjeu, bool xqhmapnelljkyv, bool yadrzjbrhjoj, int pvxtwyajol, string iuzxqdgabidzh, int cxlqhz, string flmmifoeqkeud, bool tpvpzg);
	bool flkxybvrsxqmqzkqstspp(string puwomvyxz, double vkliwlucrj, int vxtzxrkmxrb, string auzvryxahlqqxwc, string pwgzfalq, bool goibfaqcgbq, int jotnh, int hcmcqbzyd);
	double weogjdrdwaqytpwhi(string nmaycergdmfvplr, double ibfuour);
	string beubsnmavedut(double sqytfopscyshjn, double lhrbvwfsffmbu, double ecnyurqxylus, int cpyqgzpj, int cerhqklkiojmgv, double tvpaatjofskgoq, bool brljjywjq, double oxstm);
	void fjtdwtonamhshm(int wemccmtbvrcue, int gjpilwihi, string cyegdqztonkenyv, string dlhmy, bool ehihczq, int okrrdjaymo);

private:
	bool ohsbla;
	double vctywj;
	bool qaxwwttdsh;

	int lgcxtpwavvnlasso(string pdtvjpdbkuvqzv, double reazj);
	bool ptdbgajnfbvmuj(int qwwogljxsa, string dposobr, int txpxtam, bool fnqmqxxbfhs, int egdphirmiyqwne, string rxsamazsytq, string lbwhlpzlgavs);
	int vauqcnryitsiydvlsravxs();

};


int sfgljoj::lgcxtpwavvnlasso(string pdtvjpdbkuvqzv, double reazj) {
	bool jrmfxbczh = true;
	int vyeswbcdvymvjjt = 1701;
	double ovanjomzxd = 12722;
	double iacpawdjkgye = 65035;
	double cqozohomap = 36953;
	double qvfvhvskbsokf = 22634;
	int vtxijgirugxmspw = 6278;
	if (12722 != 12722) {
		int oexkslh;
		for (oexkslh = 18; oexkslh > 0; oexkslh--) {
			continue;
		}
	}
	return 86528;
}

bool sfgljoj::ptdbgajnfbvmuj(int qwwogljxsa, string dposobr, int txpxtam, bool fnqmqxxbfhs, int egdphirmiyqwne, string rxsamazsytq, string lbwhlpzlgavs) {
	int wcewrohamz = 3358;
	bool tzxwltqvyr = true;
	int vbnylphrm = 410;
	int geqepexnf = 5124;
	string jebtmbjfvts = "";
	int tskcfpvplsrdese = 6491;
	double arcqpen = 40491;
	if (410 != 410) {
		int pxbz;
		for (pxbz = 90; pxbz > 0; pxbz--) {
			continue;
		}
	}
	if (5124 != 5124) {
		int kws;
		for (kws = 20; kws > 0; kws--) {
			continue;
		}
	}
	if (40491 == 40491) {
		int vdjfh;
		for (vdjfh = 24; vdjfh > 0; vdjfh--) {
			continue;
		}
	}
	return false;
}

int sfgljoj::vauqcnryitsiydvlsravxs() {
	string owpbahamvytn = "inwrzxadcjykeijhznedgylbfdbbqpsijmdfbvmgiqrrihfjxfurynabgohddkrusgrkrkdidqewwrigepml";
	int kbmyanjhhkjuwp = 2246;
	string kpipkkp = "rozq";
	string lgbujdbdkvgfoyz = "gsswfhbawrxocforflphdruockdnvgrefespjfbmmicbjpbanmggtbhnjmvxgpxfwyuxgekcaaihtncminbdo";
	bool lqfpropzamx = true;
	double fnimvflphb = 8955;
	double dqajnlycjisro = 11590;
	bool gytkxi = true;
	if (string("inwrzxadcjykeijhznedgylbfdbbqpsijmdfbvmgiqrrihfjxfurynabgohddkrusgrkrkdidqewwrigepml") == string("inwrzxadcjykeijhznedgylbfdbbqpsijmdfbvmgiqrrihfjxfurynabgohddkrusgrkrkdidqewwrigepml")) {
		int oqxz;
		for (oqxz = 58; oqxz > 0; oqxz--) {
			continue;
		}
	}
	if (true == true) {
		int yiawwnlh;
		for (yiawwnlh = 50; yiawwnlh > 0; yiawwnlh--) {
			continue;
		}
	}
	if (string("gsswfhbawrxocforflphdruockdnvgrefespjfbmmicbjpbanmggtbhnjmvxgpxfwyuxgekcaaihtncminbdo") != string("gsswfhbawrxocforflphdruockdnvgrefespjfbmmicbjpbanmggtbhnjmvxgpxfwyuxgekcaaihtncminbdo")) {
		int qedsxvgj;
		for (qedsxvgj = 31; qedsxvgj > 0; qedsxvgj--) {
			continue;
		}
	}
	if (8955 == 8955) {
		int xl;
		for (xl = 14; xl > 0; xl--) {
			continue;
		}
	}
	if (true != true) {
		int kzpbkbzq;
		for (kzpbkbzq = 66; kzpbkbzq > 0; kzpbkbzq--) {
			continue;
		}
	}
	return 32802;
}

void sfgljoj::xomilsvqieo(double zaogik, int unrvtjnrz, double dscmn, bool rqnoa, int zwpow, int blvbwvz) {
	string jixtivdudem = "yptgtprwnhyzppxx";
	string rmushjdugbzrydt = "rfpfeytjscykvggqbesvzwgjnluqdocpillargnecrcvagchcqvgejaurdcatudncebfzfisshbgnupcsbtyjv";
	bool owkyg = false;
	bool wpxxsrz = true;
	int wjivvccrmv = 5409;
	int birnnoneuj = 735;
	double hkgavswnvvpd = 40248;
	int grnmzomtyskkopy = 3343;
	if (735 != 735) {
		int yjhsracug;
		for (yjhsracug = 26; yjhsracug > 0; yjhsracug--) {
			continue;
		}
	}
	if (string("yptgtprwnhyzppxx") == string("yptgtprwnhyzppxx")) {
		int fsvkbmv;
		for (fsvkbmv = 25; fsvkbmv > 0; fsvkbmv--) {
			continue;
		}
	}
	if (735 == 735) {
		int kgzvl;
		for (kgzvl = 97; kgzvl > 0; kgzvl--) {
			continue;
		}
	}
	if (false == false) {
		int soubcnyv;
		for (soubcnyv = 36; soubcnyv > 0; soubcnyv--) {
			continue;
		}
	}

}

double sfgljoj::hthtwszuit(int okzlaccufkp, int xgbwpcftayoopvj, double aqjjeu, bool xqhmapnelljkyv, bool yadrzjbrhjoj, int pvxtwyajol, string iuzxqdgabidzh, int cxlqhz, string flmmifoeqkeud, bool tpvpzg) {
	string hkcfxymthr = "rjyizvsqixfpurczhzuuaekksaajhqyvvuhdeguxrkcmwzgygyeytevlykiycexw";
	string inhyim = "nhjqvidsuyrtscalsztuoiiadvhakwlscspgtibtfkgtdnpsablpad";
	double agyuaqfnrh = 78444;
	string jeigklfztmst = "mjlkfkbpzunlfpsbargvzluqrstefxmuydznkemhgfzpphbnawdmhbeeehwbnrxzjfbrselhfhzuozgqdppf";
	bool cvnsuaqt = false;
	if (string("rjyizvsqixfpurczhzuuaekksaajhqyvvuhdeguxrkcmwzgygyeytevlykiycexw") == string("rjyizvsqixfpurczhzuuaekksaajhqyvvuhdeguxrkcmwzgygyeytevlykiycexw")) {
		int vsyjakhn;
		for (vsyjakhn = 84; vsyjakhn > 0; vsyjakhn--) {
			continue;
		}
	}
	if (string("rjyizvsqixfpurczhzuuaekksaajhqyvvuhdeguxrkcmwzgygyeytevlykiycexw") == string("rjyizvsqixfpurczhzuuaekksaajhqyvvuhdeguxrkcmwzgygyeytevlykiycexw")) {
		int ejfymizk;
		for (ejfymizk = 77; ejfymizk > 0; ejfymizk--) {
			continue;
		}
	}
	if (string("rjyizvsqixfpurczhzuuaekksaajhqyvvuhdeguxrkcmwzgygyeytevlykiycexw") != string("rjyizvsqixfpurczhzuuaekksaajhqyvvuhdeguxrkcmwzgygyeytevlykiycexw")) {
		int inqxr;
		for (inqxr = 83; inqxr > 0; inqxr--) {
			continue;
		}
	}
	if (false != false) {
		int ig;
		for (ig = 80; ig > 0; ig--) {
			continue;
		}
	}
	return 85547;
}

bool sfgljoj::flkxybvrsxqmqzkqstspp(string puwomvyxz, double vkliwlucrj, int vxtzxrkmxrb, string auzvryxahlqqxwc, string pwgzfalq, bool goibfaqcgbq, int jotnh, int hcmcqbzyd) {
	bool zkiafiigx = false;
	string rwmnelrqnoses = "refcghjumjcwfflbxsvdcgdirklhskcorvnqvjfqxuqgveromrsgigsypfbdkgvcfiuxloswxyjpynewtstbm";
	double lkmnkbputpdvsr = 32884;
	string xsguru = "bdjrktmoolkwaxgrzrjcoxsrewhzveerkgnpbjjpfvlxodyqmqulmwo";
	if (32884 == 32884) {
		int bdqziqpaj;
		for (bdqziqpaj = 14; bdqziqpaj > 0; bdqziqpaj--) {
			continue;
		}
	}
	if (string("refcghjumjcwfflbxsvdcgdirklhskcorvnqvjfqxuqgveromrsgigsypfbdkgvcfiuxloswxyjpynewtstbm") == string("refcghjumjcwfflbxsvdcgdirklhskcorvnqvjfqxuqgveromrsgigsypfbdkgvcfiuxloswxyjpynewtstbm")) {
		int pkhgyvlqew;
		for (pkhgyvlqew = 76; pkhgyvlqew > 0; pkhgyvlqew--) {
			continue;
		}
	}
	if (string("bdjrktmoolkwaxgrzrjcoxsrewhzveerkgnpbjjpfvlxodyqmqulmwo") == string("bdjrktmoolkwaxgrzrjcoxsrewhzveerkgnpbjjpfvlxodyqmqulmwo")) {
		int jffb;
		for (jffb = 44; jffb > 0; jffb--) {
			continue;
		}
	}
	return true;
}

double sfgljoj::weogjdrdwaqytpwhi(string nmaycergdmfvplr, double ibfuour) {
	int qajtkrmqfva = 4380;
	int irpqbrldvkgthti = 2641;
	double tizrnv = 80963;
	bool lldpyr = false;
	string ldqjmgph = "nbimaowieijhwxqdncnl";
	bool mfqqwltljqi = true;
	return 22649;
}

string sfgljoj::beubsnmavedut(double sqytfopscyshjn, double lhrbvwfsffmbu, double ecnyurqxylus, int cpyqgzpj, int cerhqklkiojmgv, double tvpaatjofskgoq, bool brljjywjq, double oxstm) {
	string vnbygm = "awzrckcxtgjsoozffxmzrujmrugyirugykezdjqzbqblymtl";
	double rktbdy = 25676;
	double tmjyqxktawavfw = 23863;
	string hzocbjgpsbvejrn = "fcfxjfvoruwozyhmzejhmhqqlnapiaminrqv";
	bool mhoprk = true;
	if (string("fcfxjfvoruwozyhmzejhmhqqlnapiaminrqv") == string("fcfxjfvoruwozyhmzejhmhqqlnapiaminrqv")) {
		int yta;
		for (yta = 20; yta > 0; yta--) {
			continue;
		}
	}
	if (true != true) {
		int pg;
		for (pg = 73; pg > 0; pg--) {
			continue;
		}
	}
	return string("");
}

void sfgljoj::fjtdwtonamhshm(int wemccmtbvrcue, int gjpilwihi, string cyegdqztonkenyv, string dlhmy, bool ehihczq, int okrrdjaymo) {
	bool pgjgscebmgdewqz = false;
	string ztrubgpgjrdd = "uqjcn";
	string zsabxww = "fjadpvovrhxiznkliagprzcid";
	bool whjcdzvyfuxcwod = false;
	double ewxsfsorskqv = 3190;
	if (string("fjadpvovrhxiznkliagprzcid") == string("fjadpvovrhxiznkliagprzcid")) {
		int bmnhsvt;
		for (bmnhsvt = 84; bmnhsvt > 0; bmnhsvt--) {
			continue;
		}
	}
	if (string("uqjcn") == string("uqjcn")) {
		int rceu;
		for (rceu = 4; rceu > 0; rceu--) {
			continue;
		}
	}
	if (false != false) {
		int fzlykl;
		for (fzlykl = 37; fzlykl > 0; fzlykl--) {
			continue;
		}
	}
	if (3190 != 3190) {
		int uhlfy;
		for (uhlfy = 57; uhlfy > 0; uhlfy--) {
			continue;
		}
	}

}

string sfgljoj::tukhoseuzrebiovnqwcsqf(int wptibtdpdv, double cwxgajmzcgq) {
	double bjunesckacshbk = 7934;
	int awaprzloeshmmlu = 542;
	bool cmbpssmmf = false;
	if (7934 == 7934) {
		int quunoqim;
		for (quunoqim = 93; quunoqim > 0; quunoqim--) {
			continue;
		}
	}
	if (542 != 542) {
		int gepitiium;
		for (gepitiium = 75; gepitiium > 0; gepitiium--) {
			continue;
		}
	}
	if (7934 == 7934) {
		int qja;
		for (qja = 15; qja > 0; qja--) {
			continue;
		}
	}
	if (7934 == 7934) {
		int smkrdy;
		for (smkrdy = 74; smkrdy > 0; smkrdy--) {
			continue;
		}
	}
	if (542 == 542) {
		int xib;
		for (xib = 43; xib > 0; xib--) {
			continue;
		}
	}
	return string("qfrgwaj");
}

bool sfgljoj::urrbufxegj(string jcdvfxdanek, double egqpojtbhgpm, double qbrxo, double lbqipsqczy, string ptiawvzrechdzek, string wqmmeitse, bool lozlgyqctkbyzk, bool noofxhfpgr, double zdhrjccdy, string ewhvmsi) {
	bool xjlis = false;
	string hfjhzv = "gxjhyacmalaicrrgklebfhvvjpblwgvnkxhrklyswajesoasdilhouvobhjwvxlpumpgqyopvmpiozkzmp";
	bool xwuqawmp = true;
	bool rhhiq = true;
	bool fzcxumoqhx = true;
	string eprkr = "liktsdpncnqdgodzdhjoihcbnpachcht";
	double coqtwfkrjc = 26167;
	bool mwyjs = false;
	double ogeifrwakxy = 38506;
	double atgjainhpnfa = 9047;
	if (26167 != 26167) {
		int yiwuh;
		for (yiwuh = 54; yiwuh > 0; yiwuh--) {
			continue;
		}
	}
	return false;
}

bool sfgljoj::krulfhebxnxcgoere(string jremwb, bool nxacgaoicx) {
	return true;
}

double sfgljoj::krrcysllemsqqy(int xcscg, double yrwbjkkvhxbpb, string hksbutig, bool pbyah, double myrkdscypwtvlc, bool qpvilyoqogygl, double cdvvkpvrcuydx, bool iixtccqin, bool euqurr) {
	double qpymqqbqg = 9702;
	bool pfvkbbgwioktsk = false;
	bool jnekjopfilwex = false;
	int fpgchmojejhyg = 1952;
	bool wfxbb = true;
	int gxvnnkwavayxoi = 779;
	if (779 != 779) {
		int qxrotbu;
		for (qxrotbu = 52; qxrotbu > 0; qxrotbu--) {
			continue;
		}
	}
	if (true == true) {
		int czhsylp;
		for (czhsylp = 97; czhsylp > 0; czhsylp--) {
			continue;
		}
	}
	if (1952 == 1952) {
		int ccbxj;
		for (ccbxj = 74; ccbxj > 0; ccbxj--) {
			continue;
		}
	}
	if (1952 == 1952) {
		int vihm;
		for (vihm = 49; vihm > 0; vihm--) {
			continue;
		}
	}
	return 9696;
}

string sfgljoj::ssdbdaqwujsebfimlo(int pgwbnbo, bool jzwkwpbi, string iynrhew, bool qbpgxjtgxosgxvy, bool cuxnijstbkc, bool maxsekkewhde, string fulyfk, int egojtclqwmmq, bool vbiipruvvkalpqj) {
	string fdzvvwdeay = "ndptefygiggriyncbgefuqjlgdpayfuumnnqntxnaodhdy";
	bool lbgumbmzxugr = true;
	double czgpntyrkotbwl = 23837;
	return string("hwmsysjeeuw");
}

int sfgljoj::kfriamguffwokkaov(bool ldvehcgrha, string dzvnckrqz, int bammdcwabx, string xccagrfedyastxt, string viifuyr, bool buosvmpx, int oqzmwu, int vyppottqifylhl, string zwuut, int mvcyqhqxjezvo) {
	int lukncwwvx = 582;
	bool irybyvkghdrhv = false;
	bool mtmaaeocddlx = true;
	int ehgve = 8999;
	double ynncvrgdagoxd = 37472;
	bool ablmicnlwoz = false;
	string urwntxgu = "cwoarmrtloctnpsikleyxayilsdiwwzolmwsyimzsitgvpixonccwwltrugtfveqfk";
	bool aeyhgvzhd = false;
	int xabbxslhyigvc = 1816;
	if (1816 == 1816) {
		int ybytemrhuu;
		for (ybytemrhuu = 94; ybytemrhuu > 0; ybytemrhuu--) {
			continue;
		}
	}
	if (8999 != 8999) {
		int xvkk;
		for (xvkk = 73; xvkk > 0; xvkk--) {
			continue;
		}
	}
	if (false == false) {
		int leeesu;
		for (leeesu = 82; leeesu > 0; leeesu--) {
			continue;
		}
	}
	return 1624;
}

double sfgljoj::gywlocfsuitjq(string uakuvuubq, string sdtugmnlsdajjg, double zpqahfwzviilwn, double ddqzpvkbxzur, int ixyyg, int ffdpmupheozbxf, int epylajfjgdtm, bool qhaqnaivbjdjxag, double wtzvdvoc) {
	double frsjhoobvpwdons = 2464;
	double dkiqiwboptnvpx = 3263;
	bool askbghorwnig = true;
	int ewnbglvqnwtawjg = 666;
	bool yjamnmzo = false;
	return 54923;
}

sfgljoj::sfgljoj() {
	this->tukhoseuzrebiovnqwcsqf(417, 9630);
	this->urrbufxegj(string("bxzjcgpxmkxq"), 73752, 21750, 14687, string("iktetqhslhdxbpefdnanwcurbgdndheczxafplmxrhmtjnjozmnamuxzgovmnhyzhayw"), string("knyqomskdpledfqwqtncspbafsdiarhktxzsnxbmwgtxceoxtbpcodazdroo"), true, true, 55762, string("bpctasnukrdlqkkqutljiinmtubwllfhqksyadtlwzfidmbxqnzwjbgwqlfspzfwnmvwyhziorponrekuepebvm"));
	this->krulfhebxnxcgoere(string("xcymwjrrlyirigzphu"), false);
	this->krrcysllemsqqy(2937, 2820, string("lp"), true, 10138, true, 21655, true, true);
	this->ssdbdaqwujsebfimlo(1911, false, string("uhcjobbbqmjbbcgnigdohcdlasaxiqgzoplcraywxmrfrkejurktxaykoyebmubmbpkyoitgidwbrcphdpfwnarqjdqstjmwvnzb"), false, false, false, string("asleyhxxchtj"), 1536, false);
	this->kfriamguffwokkaov(false, string("wauxgsxhsujacmewsbu"), 257, string("yhhfqdmmiisdfbwcgsurjqexogsufclduqkzigmobpyubqxlutrhwgrmsmrgfzzlwtofhpfajwsgcywygz"), string("dbmeinrvtofztxzaszfocishecsaooldzdbabqsbagcyxkptsafixhjzouwddmcwmnmrnwdimny"), false, 5926, 2187, string("qqjlvoepvgqiulusdqcbrwyvfkeycsgzksapbtwiprjqytuczmtjbremjtzxghwftwxiuxplvujuupltouyxxgwbczgstxjd"), 1516);
	this->gywlocfsuitjq(string("ingflbmjcwrqngzuztnsnbsymvfdvefrwaqnsciqyukty"), string("uosddyrabxoyytzfplbxukikpsntnddqogsiwgsegfrllnsnuacsqrqblveqetjkpldcdxs"), 20320, 5439, 2555, 2329, 2884, true, 33480);
	this->xomilsvqieo(35969, 6900, 3324, false, 423, 906);
	this->hthtwszuit(3749, 149, 29906, true, true, 187, string("fspfmkghttrt"), 2767, string("tpxmjmcfavpvmjawjgnfrcvcohigxzgnlifgwjytrvlswopdcmhdksnwcag"), false);
	this->flkxybvrsxqmqzkqstspp(string("brfhmfwtaxuf"), 37080, 2482, string("gbkdtndgbwmxupfgddmhobgntecokfdgowmukwxkobbpmmcgghyq"), string("bfmtgkpibamyiouhhtlsiewijioiazkotanjbr"), false, 1898, 107);
	this->weogjdrdwaqytpwhi(string("thymbcarfgjerpsowgwvxionc"), 35581);
	this->beubsnmavedut(44801, 15590, 52594, 7154, 4762, 66474, true, 16913);
	this->fjtdwtonamhshm(953, 714, string("vbuitxcvkbqopung"), string("jwtvlcymtrkls"), false, 3324);
	this->lgcxtpwavvnlasso(string("z"), 87351);
	this->ptdbgajnfbvmuj(4250, string("hjjitemnlvdpntdcdbavivzkhpsnlnolsaszkndhoigxysegyzkpwdbxegflxtnsudtdiihgdwdfjhz"), 184, true, 3388, string("xovgfjqbzcqhqruavlzvqidmxhysffti"), string("bisxl"));
	this->vauqcnryitsiydvlsravxs();
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class vzcnpue {
public:
	bool kxwuehd;
	bool dcdthnqysraezp;
	double uafmardiksp;
	string csjwlaotpu;
	vzcnpue();
	bool lqlosebtvrafvddaiy(string labqbfwlygaqqep);
	bool xeltuzbkguzplanbdncgf(string dsogocbslipctv, int ppvnrnkjnzkep, string nvtfpywgdt, bool wbbkumorvuxpvh, double bxrynljyb, string rvgaoovuwqxh, double wwyhem, double qwmqufvmswelqdl, string fzvgxzykxcqy, int vlhqdomks);
	void llmjalvnkaapqspsfvynbqkvs(int rnmcibjps, int scmgfeoxlzso, double juvoawrbldh, bool suayehshhqnpqt);
	string ipjzftygrpmi(string orygsutz, bool pmoswj, int jpndpmgta, double dzzqplsxuydxba, int swzjgnws);

protected:
	string xhuqjstt;
	string ewblgtrijzlie;
	double rkdzhhrejjuuii;
	string cvmhindhuw;
	double rtsczed;

	bool docleeewjubwyeixeoegkksd(bool wxifyj, int rgdgmbplmhpd, int yrxienit, bool jfezfw);
	bool vbmmpsukhmzwwhirqdfbh();
	string leedslwjvfsvmjwavdyr(double beawg, bool kuncwjqfzvm, double sowdla, int bowyvsmxf, bool woezcq);
	bool njygynzhooj(string ojacuqhn, bool ekjviwyvxtms, double rkqcxw, int wtkpgmnzmozgefo);

private:
	int slzyzra;
	int brbdhzggxefqzr;

	double mjqicmaozkvlyrpt(int iautondqtsjicfj, double otwwsxqsgguv, double vgwqmztrcpjanpw, bool gnntkr, double oyjhtgajj, int edoqrfbkn);
	double zpkztixmmyrubgodmv(bool irwldgaazsej, string cadvvhpuygmpa, bool rlhpbjhrfsl, string mokjue, string gqxlcvegcckl, bool quqiafudtj, double rfcgqmhtdi);
	double kdoiaxsnoye(bool dhnzoieqfsshz, string mbkcgu, int snwlaxu, bool domanavu);
	double epmbkhtbjbzmnl(bool lnjzsfui, bool qyjvoopy, bool vqhiusiijvyncv, double hnlrrssbhjqbtz, string xrvug, bool tbgmsgpmmzgez, bool ahzynzs);
	int dkxxhlofykjepwvbgbyeh(string tgltyzmjwd, string elzmadftfy, string rjjwjpimprklss, string znvowdghvilcso, double zgrowaqi, int ygweiuhvtweqsbt, int ziufg, int oewypyhoe, string mdfjzfvzpnck);

};


double vzcnpue::mjqicmaozkvlyrpt(int iautondqtsjicfj, double otwwsxqsgguv, double vgwqmztrcpjanpw, bool gnntkr, double oyjhtgajj, int edoqrfbkn) {
	bool wpwuwx = false;
	double zcadqyqgme = 4908;
	string rcicbt = "djvpdptsqaocrgffyritzhlkvptupyxroqudzdqsx";
	int wcgcwhuatisn = 2030;
	double zampolltg = 23927;
	int jlwnxxa = 4703;
	int yqgnk = 4493;
	bool nmhpypowuztjz = false;
	string uczvrp = "ihcrysdukhefaxkdykcef";
	if (string("djvpdptsqaocrgffyritzhlkvptupyxroqudzdqsx") == string("djvpdptsqaocrgffyritzhlkvptupyxroqudzdqsx")) {
		int gsw;
		for (gsw = 12; gsw > 0; gsw--) {
			continue;
		}
	}
	return 87019;
}

double vzcnpue::zpkztixmmyrubgodmv(bool irwldgaazsej, string cadvvhpuygmpa, bool rlhpbjhrfsl, string mokjue, string gqxlcvegcckl, bool quqiafudtj, double rfcgqmhtdi) {
	int mkqtzvkvq = 1368;
	bool ddozkgilefwx = false;
	int gjdzitkwh = 1601;
	int fmnsspa = 388;
	if (false != false) {
		int arkigiiva;
		for (arkigiiva = 6; arkigiiva > 0; arkigiiva--) {
			continue;
		}
	}
	if (388 == 388) {
		int gfoo;
		for (gfoo = 53; gfoo > 0; gfoo--) {
			continue;
		}
	}
	return 19733;
}

double vzcnpue::kdoiaxsnoye(bool dhnzoieqfsshz, string mbkcgu, int snwlaxu, bool domanavu) {
	return 83389;
}

double vzcnpue::epmbkhtbjbzmnl(bool lnjzsfui, bool qyjvoopy, bool vqhiusiijvyncv, double hnlrrssbhjqbtz, string xrvug, bool tbgmsgpmmzgez, bool ahzynzs) {
	bool hbmsrsjsse = false;
	if (false != false) {
		int bldf;
		for (bldf = 66; bldf > 0; bldf--) {
			continue;
		}
	}
	if (false == false) {
		int thcreihu;
		for (thcreihu = 73; thcreihu > 0; thcreihu--) {
			continue;
		}
	}
	if (false != false) {
		int tgqcinvutf;
		for (tgqcinvutf = 16; tgqcinvutf > 0; tgqcinvutf--) {
			continue;
		}
	}
	if (false != false) {
		int mefwk;
		for (mefwk = 22; mefwk > 0; mefwk--) {
			continue;
		}
	}
	return 39713;
}

int vzcnpue::dkxxhlofykjepwvbgbyeh(string tgltyzmjwd, string elzmadftfy, string rjjwjpimprklss, string znvowdghvilcso, double zgrowaqi, int ygweiuhvtweqsbt, int ziufg, int oewypyhoe, string mdfjzfvzpnck) {
	int sinqcdueb = 351;
	bool loggec = true;
	string xydwsengo = "zayrgxswvfcrzrvvrlqmfuhpbtwvrsmwpyqrzelzhwpbrxfvcatvy";
	double cbulldspx = 29993;
	int jfuvancfyc = 5229;
	int eehvafkjbopb = 514;
	int vqazcubpdacn = 488;
	string ofegzdvorq = "kaqyvzpfahjsit";
	if (29993 == 29993) {
		int um;
		for (um = 19; um > 0; um--) {
			continue;
		}
	}
	if (488 != 488) {
		int jjmo;
		for (jjmo = 37; jjmo > 0; jjmo--) {
			continue;
		}
	}
	if (488 == 488) {
		int bvsixiiubu;
		for (bvsixiiubu = 81; bvsixiiubu > 0; bvsixiiubu--) {
			continue;
		}
	}
	return 39385;
}

bool vzcnpue::docleeewjubwyeixeoegkksd(bool wxifyj, int rgdgmbplmhpd, int yrxienit, bool jfezfw) {
	string nwgzwilpczo = "bnkrvbliqytbzzbnhoazqbptufsvogihbpyeinweltqnfqgwuqzfjzmrptsaiozxiebholllaomgkgdjxicphljlk";
	bool nfdlqdr = false;
	bool zggyonpk = true;
	string nyqfbd = "wsxbexpxhvotxomgrvunvkkywqhnveadabsamtbclxdflcnhjslilafmwuyydnsonty";
	bool nqnervxk = false;
	if (string("wsxbexpxhvotxomgrvunvkkywqhnveadabsamtbclxdflcnhjslilafmwuyydnsonty") == string("wsxbexpxhvotxomgrvunvkkywqhnveadabsamtbclxdflcnhjslilafmwuyydnsonty")) {
		int jzkwvlnic;
		for (jzkwvlnic = 21; jzkwvlnic > 0; jzkwvlnic--) {
			continue;
		}
	}
	if (true != true) {
		int kob;
		for (kob = 81; kob > 0; kob--) {
			continue;
		}
	}
	return true;
}

bool vzcnpue::vbmmpsukhmzwwhirqdfbh() {
	int tizodlrz = 2176;
	bool nnwooflcgph = false;
	int haftbhqfucrzv = 901;
	if (false == false) {
		int eczsh;
		for (eczsh = 54; eczsh > 0; eczsh--) {
			continue;
		}
	}
	if (901 != 901) {
		int fmn;
		for (fmn = 98; fmn > 0; fmn--) {
			continue;
		}
	}
	return false;
}

string vzcnpue::leedslwjvfsvmjwavdyr(double beawg, bool kuncwjqfzvm, double sowdla, int bowyvsmxf, bool woezcq) {
	double qpyecuaebx = 18134;
	return string("xzeoaz");
}

bool vzcnpue::njygynzhooj(string ojacuqhn, bool ekjviwyvxtms, double rkqcxw, int wtkpgmnzmozgefo) {
	int txzitralx = 2740;
	bool kfveq = false;
	double gjakcpvujr = 10919;
	double iuprrczgled = 2177;
	string xrmfzb = "dcgvelrydoq";
	bool egqmmzyqmaoif = true;
	string zjossgeit = "xmzxewcecyrwwtdnnohtathidtkqbmmcarzyowleysmg";
	int tpcryogubk = 475;
	int dydnafcve = 563;
	double uuxdmb = 12209;
	if (false == false) {
		int zqpnykakrk;
		for (zqpnykakrk = 100; zqpnykakrk > 0; zqpnykakrk--) {
			continue;
		}
	}
	if (10919 == 10919) {
		int xcw;
		for (xcw = 87; xcw > 0; xcw--) {
			continue;
		}
	}
	if (475 != 475) {
		int omicxra;
		for (omicxra = 93; omicxra > 0; omicxra--) {
			continue;
		}
	}
	if (false != false) {
		int ysr;
		for (ysr = 85; ysr > 0; ysr--) {
			continue;
		}
	}
	if (475 != 475) {
		int gcpyx;
		for (gcpyx = 45; gcpyx > 0; gcpyx--) {
			continue;
		}
	}
	return true;
}

bool vzcnpue::lqlosebtvrafvddaiy(string labqbfwlygaqqep) {
	int xjxyvikdz = 7683;
	string gutbsapeoyi = "lirrptdfrekrkzjaalipohcxonvfkojbjqpioucrhootkrskcmaehdssmxxj";
	double yicgrfv = 15446;
	string gphssgyxzrrf = "vcrjzimkffyzwaqflirtlhuwmhdaurpcmccbzcqmsptjtcgyntdeeerpphzjerkkdcqodutmibllmfagvzskhsntdfx";
	if (15446 != 15446) {
		int ikyr;
		for (ikyr = 66; ikyr > 0; ikyr--) {
			continue;
		}
	}
	return true;
}

bool vzcnpue::xeltuzbkguzplanbdncgf(string dsogocbslipctv, int ppvnrnkjnzkep, string nvtfpywgdt, bool wbbkumorvuxpvh, double bxrynljyb, string rvgaoovuwqxh, double wwyhem, double qwmqufvmswelqdl, string fzvgxzykxcqy, int vlhqdomks) {
	bool cievojs = false;
	string bbbpkbbrn = "xzvjnmtgtyjlwgaijo";
	bool znlqunxjwywisaf = false;
	string apczffzhsxsiyui = "zgyzgdpnaamjcjebvajvksyjbrjrtwxzkjjuxcbcnmrrguqmgveturatrdekrulmzsitwv";
	bool smejlpgvovvxja = false;
	string wqazm = "alnfktjyijuxtsyozxugbjtvjdj";
	int loeogonwscx = 1265;
	bool vmnjwsceauisyw = false;
	double dxjdpi = 45185;
	int wjjopk = 2210;
	if (false != false) {
		int nml;
		for (nml = 35; nml > 0; nml--) {
			continue;
		}
	}
	if (2210 != 2210) {
		int uyqfgqud;
		for (uyqfgqud = 47; uyqfgqud > 0; uyqfgqud--) {
			continue;
		}
	}
	if (string("alnfktjyijuxtsyozxugbjtvjdj") == string("alnfktjyijuxtsyozxugbjtvjdj")) {
		int paaz;
		for (paaz = 42; paaz > 0; paaz--) {
			continue;
		}
	}
	if (false == false) {
		int blcuxoyrwq;
		for (blcuxoyrwq = 82; blcuxoyrwq > 0; blcuxoyrwq--) {
			continue;
		}
	}
	return true;
}

void vzcnpue::llmjalvnkaapqspsfvynbqkvs(int rnmcibjps, int scmgfeoxlzso, double juvoawrbldh, bool suayehshhqnpqt) {
	double cuqwyegmlgxx = 9278;
	bool qajddu = false;
	bool vwntjyyuf = false;
	string pwvbhowqsu = "tbskxpcyljzbwzzqgxbmcgzmtszndjetxpxsevbbzskvozqsgustrzeerkynarhzlngygbsugkbrozbzkhshculpzlkdzgjtus";

}

string vzcnpue::ipjzftygrpmi(string orygsutz, bool pmoswj, int jpndpmgta, double dzzqplsxuydxba, int swzjgnws) {
	double jmmcyxytxtlvxab = 16379;
	string fobsundaggzp = "hcwjeegwotbtvsrayghvljvsnxhrhlkycrhyyryzabpcgonkyhtfswuwindfauxumjjjaezxmzgcl";
	int titnrrnmk = 5119;
	int npplhlqs = 978;
	double dtbtric = 28382;
	int qryxmtpemwwok = 3290;
	int vxagun = 2122;
	bool fxazhdnuxfkq = true;
	double ytsaeo = 85438;
	if (978 != 978) {
		int xcwwwpyzl;
		for (xcwwwpyzl = 29; xcwwwpyzl > 0; xcwwwpyzl--) {
			continue;
		}
	}
	if (5119 == 5119) {
		int qtkwnlwkkg;
		for (qtkwnlwkkg = 99; qtkwnlwkkg > 0; qtkwnlwkkg--) {
			continue;
		}
	}
	if (5119 == 5119) {
		int hztlq;
		for (hztlq = 78; hztlq > 0; hztlq--) {
			continue;
		}
	}
	return string("rgiy");
}

vzcnpue::vzcnpue() {
	this->lqlosebtvrafvddaiy(string("urqomjskxgbkbvahsehoauiyjijgpvanhhvbhljxxjxvyvieqhngpeqgnw"));
	this->xeltuzbkguzplanbdncgf(string("ljfunrsxiagy"), 56, string("rfitpmiuthwaerivluaptzslrudershtkrdnzqxzzjvhreifcdptyctewsmrffarzkjououtxsadzqxksbufycflzdwhj"), true, 11244, string("azkasuefzywotnqfqktnmhdrnswjasuelpvad"), 28449, 30402, string("apfiwtagezvhbpxwzdwmkrhfrpgitpkfhoatzzhqcfknalzynzrj"), 1736);
	this->llmjalvnkaapqspsfvynbqkvs(2007, 3375, 30660, false);
	this->ipjzftygrpmi(string("rbohfnpxjmidvpjduodmstkvdvkddfoazlhyuuyxzunzmugbnpayjfczrvztajca"), true, 1499, 45706, 2218);
	this->docleeewjubwyeixeoegkksd(true, 5355, 2594, false);
	this->vbmmpsukhmzwwhirqdfbh();
	this->leedslwjvfsvmjwavdyr(16595, true, 604, 994, true);
	this->njygynzhooj(string("fqmbgcebesxmnczrbnknvxiiiwvflentjcjrqonjdnivajvxlxcrmqjlni"), false, 21248, 304);
	this->mjqicmaozkvlyrpt(2812, 48300, 54536, false, 6682, 2032);
	this->zpkztixmmyrubgodmv(false, string("jhwsmfrrxrkyfqzdaa"), false, string("pcbnzwgkrgyuietveeifxetnamhu"), string("tvdlwwnhvcfebbablbtslwlzwueolsfghxbbiuzrdjltpdfuowxgvsbvdnwqyuemaiwhpgqannlnjmjzaxzsoahqyhelxco"), true, 26585);
	this->kdoiaxsnoye(true, string("ihiwvhmxuvhjczfonduuacfbgq"), 8260, false);
	this->epmbkhtbjbzmnl(true, false, true, 16284, string("xmunoxhhyn"), true, true);
	this->dkxxhlofykjepwvbgbyeh(string("ekdcbbyohymupvibpqojqcvejmnjiue"), string("ncpalriibslwwhnvujybgmvybwztknauzuhlaktsilvesha"), string("rgvmqswrsmycuzjpgxoknfwvtudodszo"), string("tcnpshdzpscihiqhoxgbanephlsbonpzqjtlchjruwjxbugptbovwajbkinwnxtxzqupyyfmeyfeyuxvgjq"), 6582, 636, 1428, 5110, string("xcxtogkledrsylqaznynotqfxhwkaeeiomnsjnieoeoehfweppkxueauw"));
}


                                         

















































































































































































































































































