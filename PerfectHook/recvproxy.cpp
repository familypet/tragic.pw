#include "SDK.h"


#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1
using namespace std;
// Helper function to generate a random sequence number.
inline int RandomSequence(int low, int high) {
    return (rand() % (high - low + 1) + low);
}

#define	LIFE_ALIVE 0

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);



RecvVarProxyFn fnSequenceProxyFn = nullptr;

RecvVarProxyFn oRecvnModelIndex;
void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
    // Get the knife view model id's
    int default_t = g_ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
    int default_ct = g_ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
    int iBayonet = g_ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
    int iButterfly = g_ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
    int iFlip = g_ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
    int iGut = g_ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
    int iKarambit = g_ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
    int iM9Bayonet = g_ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
    int iHuntsman = g_ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
    int iFalchion = g_ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
    int iDagger = g_ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
    int iBowie = g_ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");

    int iGunGame = g_ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");

    // Get local player (just to stop replacing spectators knifes)
    C_BaseEntity* pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
    if (g_Options.Skinchanger.Enabled && pLocal)
    {
        // If we are alive and holding a default knife(if we already have a knife don't worry about changing)
        if (pLocal->IsAlive() && (
            pData->m_Value.m_Int == default_t ||
            pData->m_Value.m_Int == default_ct ||
            pData->m_Value.m_Int == iBayonet ||
            pData->m_Value.m_Int == iFlip ||
            pData->m_Value.m_Int == iGunGame ||
            pData->m_Value.m_Int == iGut ||
            pData->m_Value.m_Int == iKarambit ||
            pData->m_Value.m_Int == iM9Bayonet ||
            pData->m_Value.m_Int == iHuntsman ||
            pData->m_Value.m_Int == iBowie ||
            pData->m_Value.m_Int == iButterfly ||
            pData->m_Value.m_Int == iFalchion ||
            pData->m_Value.m_Int == iDagger))
        {
            // Set whatever knife we want
            if (g_Options.Skinchanger.Knife == 0)
                pData->m_Value.m_Int = iBayonet;
            else if (g_Options.Skinchanger.Knife == 1)
                pData->m_Value.m_Int = iBowie;
            else if (g_Options.Skinchanger.Knife == 2)
                pData->m_Value.m_Int = iButterfly;
            else if (g_Options.Skinchanger.Knife == 3)
                pData->m_Value.m_Int = iFalchion;
            else if (g_Options.Skinchanger.Knife == 4)
                pData->m_Value.m_Int = iFlip;
            else if (g_Options.Skinchanger.Knife == 5)
                pData->m_Value.m_Int = iGut;
            else if (g_Options.Skinchanger.Knife == 6)
                pData->m_Value.m_Int = iHuntsman;
            else if (g_Options.Skinchanger.Knife == 7)
                pData->m_Value.m_Int = iKarambit;
            else if (g_Options.Skinchanger.Knife == 8)
                pData->m_Value.m_Int = iM9Bayonet;
            else if (g_Options.Skinchanger.Knife == 9)
                pData->m_Value.m_Int = iDagger;
        }
    }
    // Carry on the to original proxy
    oRecvnModelIndex(pData, pStruct, pOut);
}





void SetViewModelSequence2(const CRecvProxyData *pDataConst, void *pStruct, void *pOut)
{

    // Make the incoming data editable.
    CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);

    // Confirm that we are replacing our view model and not someone elses.
    CBaseViewModel* pViewModel = static_cast<CBaseViewModel*>(pStruct);

    if (pViewModel)
    {
        C_BaseEntity* pOwner = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(pViewModel->GetOwner() & 0xFFF));

        // Compare the owner entity of this view model to the local player entity.
        if (pOwner && pOwner->GetIndex() == g_Engine->GetLocalPlayer())
        {
            // Get the filename of the current view model.
            void* pModel = g_ModelInfo->GetModel(pViewModel->GetModelIndex());
            std::string szModel = g_ModelInfo->GetModelName(pModel);

            // Store the current sequence.
            int m_nSequence = pData->m_Value.m_Int;
            if (szModel == "models/weapons/v_knife_butterfly.mdl")
            {
                // Fix animations for the Butterfly Knife.
                switch (m_nSequence)
                {
                case SEQUENCE_DEFAULT_DRAW:
                    m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
                    break;
                case SEQUENCE_DEFAULT_LOOKAT01:
                    m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
                    break;
                default:
                    m_nSequence++;
                }
            }
            else if (szModel == "models/weapons/v_knife_falchion_advanced.mdl")
            {
                // Fix animations for the Falchion Knife.
                switch (m_nSequence)
                {
                case SEQUENCE_DEFAULT_IDLE2:
                    m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
                case SEQUENCE_DEFAULT_HEAVY_MISS1:
                    m_nSequence = RandomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
                    break;
                case SEQUENCE_DEFAULT_LOOKAT01:
                    m_nSequence = RandomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
                    break;
                case SEQUENCE_DEFAULT_DRAW:
                case SEQUENCE_DEFAULT_IDLE1:
                    break;
                default:
                    m_nSequence--;
                }
            }
            else if (szModel == "models/weapons/v_knife_push.mdl")
            {
                // Fix animations for the Shadow Daggers.
                switch (m_nSequence)
                {
                case SEQUENCE_DEFAULT_IDLE2:
                    m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
                case SEQUENCE_DEFAULT_LIGHT_MISS1:
                case SEQUENCE_DEFAULT_LIGHT_MISS2:
                    m_nSequence = RandomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
                    break;
                case SEQUENCE_DEFAULT_HEAVY_MISS1:
                    m_nSequence = RandomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
                    break;
                case SEQUENCE_DEFAULT_HEAVY_HIT1:
                case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
                case SEQUENCE_DEFAULT_LOOKAT01:
                    m_nSequence += 3; break;
                case SEQUENCE_DEFAULT_DRAW:
                case SEQUENCE_DEFAULT_IDLE1:
                    break;
                default:
                    m_nSequence += 2;
                }
            }
            else if (szModel == "models/weapons/v_knife_survival_bowie.mdl")
            {
                // Fix animations for the Bowie Knife.
                switch (m_nSequence)
                {
                case SEQUENCE_DEFAULT_DRAW:
                case SEQUENCE_DEFAULT_IDLE1:
                    break;
                case SEQUENCE_DEFAULT_IDLE2:
                    m_nSequence = SEQUENCE_BOWIE_IDLE1;
                    break;
                default:
                    m_nSequence--;
                }
            }

            // Set the fixed sequence.
            pData->m_Value.m_Int = m_nSequence;
        }
    }

    // Call original function with the modified data.

    fnSequenceProxyFn(pData, pStruct, pOut);
}

RecvVarProxyFn fnNoSmoke;
void NoSmoke(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	switch (g_Options.Visuals.Smoke)
	{
	case 2:
		*(bool*)((DWORD)pOut + 0x1) = true;
		break;
	default:
		break;
	}

	fnNoSmoke(pData, pStruct, pOut);
}
void AnimationFixHook()
{
    for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
        if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
            // Search for the 'm_nModelIndex' property.
            RecvTable* pClassTable = pClass->m_pRecvTable;

            for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
                RecvProp* pProp = &pClassTable->m_pProps[nIndex];

                if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
                    continue;

                // Store the original proxy function.
                fnSequenceProxyFn = static_cast<RecvVarProxyFn>(pProp->m_ProxyFn);

                // Replace the proxy function with our sequence changer.
                pProp->m_ProxyFn = static_cast<RecvVarProxyFn>(SetViewModelSequence2);

                break;
            }

            break;
        }
    }
}

void AnimationFixUnhook()
{
    for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
        if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
            // Search for the 'm_nModelIndex' property.
            RecvTable* pClassTable = pClass->m_pRecvTable;

            for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
                RecvProp* pProp = &pClassTable->m_pProps[nIndex];

                if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
                    continue;

                // Replace the proxy function with our sequence changer.
                pProp->m_ProxyFn = fnSequenceProxyFn;

                break;
            }

            break;
        }
    }
}
void NetvarHook()
{
    AnimationFixHook();
    ClientClass *pClass = g_CHLClient->GetAllClasses();
    while (pClass)
    {
        const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
        if (!strcmp(pszName, "DT_SmokeGrenadeProjectile"))
        {
            for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
            {
                RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
                const char *name = pProp->m_pVarName;
                if (!strcmp(name, "m_bDidSmokeEffect"))
                {
                    fnNoSmoke = (RecvVarProxyFn)pProp->m_ProxyFn;
                    pProp->m_ProxyFn = NoSmoke;
                }

            }
        }
        else if (!strcmp(pszName, "DT_BaseViewModel"))
        {
            for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
            {
                RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
                const char *name = pProp->m_pVarName;
                if (!strcmp(name, "m_nModelIndex"))
                {
                    oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
                    pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
                }
            }
        }
        pClass = pClass->m_pNext;
    }
}
void UnloadProxy()
{
    AnimationFixUnhook();
    ClientClass *pClass = g_CHLClient->GetAllClasses();
    while (pClass)
    {
        const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
        if (!strcmp(pszName, "DT_SmokeGrenadeProjectile"))
        {
            for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
            {
                RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
                const char *name = pProp->m_pVarName;
                if (!strcmp(name, "m_bDidSmokeEffect"))
                {
                    pProp->m_ProxyFn = fnNoSmoke;
                }

            }
        }
        else if (!strcmp(pszName, "DT_BaseViewModel"))
        {
            for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
            {
                RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
                const char *name = pProp->m_pVarName;


                // Knives
                if (!strcmp(name, "m_nModelIndex"))
                {
                    pProp->m_ProxyFn = oRecvnModelIndex;
                }
            }
        }
        pClass = pClass->m_pNext;
    }
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class gelbyfb {
public:
	bool akpvektellngt;
	bool bhdcftxikkcz;
	bool uprlyhxfxcldsw;
	double ixxzswrkkc;
	double ipmnyreeyka;
	gelbyfb();
	double gcprkqxqsvn(double tchbd, string vbwwox, string oqmkwhyhlmqfr);
	bool hfmzvmdlnj(string rufiegjdcqm, bool chysthhn, string hfnhjzlyrwvwqe, bool dbfgsfenppbnqi, double uowqixq, bool uighxilpmdpxjyg, int efxdkzupcox, string oixjhkbwowirfwa);
	void uzuhdsvemzeds(string srdrdkhfkmadldp, int yaxvyxhui, double exrsjxv, bool pehvs, bool zkbpbqvvvwqkc, string rbgyfwssdzqvue);
	void ajpzrusamdzb(double yqwauxgqhwz, bool zwzfoernk, string qloto, double cpnsztrukksha);
	void jbhfqvckjqoujlckhdcjtpi();
	bool rehcmfuqomxnezbuskauqm(int xidvrat, bool liafvdw, int njwncdyqsypftnj, int aypvyxullwn, string nbqws);
	int wrkxunkvmotkvw(double wdbtfa, bool rzkzo, string elfifaa, double cwwljskyxp, int llvouzvvnf, bool ltfdqrmkxwvfvjm);

protected:
	int dknvvbbyhnv;
	double qsjxyuf;
	int gppecbhj;

	string bmbdsjmopanszgh(bool giepk, int vskflzadsfeldk, double ztkzb, bool gzpjiyv, bool kxeyaplcrizbtjg);
	bool lwekibwpofydeswpg(double mrlfydawzsbmtb, int zxghfodsig, bool hnzhgrtgbcofjx, int bgmhaenspxfmwwz, bool irswpky, int lsshsneechewnr, double idjwxtntkbf, string fmhlpbnufona);
	bool zfdmhuvptuwjvdknfeypv(bool sdnqikwfndtpbc, string oqrklovp, string lrxlewqvrxxmnss, string knrrdtmgpxhsww, double puisbpclwf, string yalqh);

private:
	string mryhidzdprr;

	double nlndpajmnaon(int oxsnjawzet, int oalhfn, int pcdfdbklylr, bool pltko, string stpmgya, double wxipgro, double hnbbtga);
	double ecdfodfqovnl(bool pguluwr, bool uchfoxxihwrh, bool azohp, int htdan, bool vfournlkcok, int ltyqzkqwkvi, int nndiwtd, int lhzlavjwcz, double kxndsuuvvdg);
	double htkttgmvutszvadhbnep(bool fvewmc, int alehch, int fwdvjjdz, double xqdgbs);
	double dzwyevrgaajnbqp();
	void tjoyhcvfnfftfosrat(double irkphign, bool jmirzv, int relkoorvvgq, bool wrllvriyznelnxm, string mjlkpoinucdsbk, double aywrnagasfueed, double fzlqktswdfpjitk, int lknmarbclfwj, string faxmzubjzd);
	int wwmaunwgpezfcod(bool uwbhjykt, int awjmqps, int bsqhlrbekitjlj, double yqnieiyaoh, int atffg, bool iasjanquhhew);
	void itrfjccozaobxadftd(string olnfuvhwirvmgj, bool hyoxeudsd);
	void hantmuudmxobeqemczweqkars(int eskakn, int eaervlbocmdk, bool hzbylgyt, string dmhhyvefwyru, bool fvtegufr, string fkqmqhiekkeogt, double gdgmoqgvcev);
	int srtkdvqhftbmkvjqudgzazgu(double octdkagghhlvmyq, bool jzblruhhwvkla, bool naqxwaldipimh, bool tnnyxzzkhinuh);
	bool mnnsqhnthgcpbz(int gsdfe, int qihsb, string ahnfsdlnqfkpfvt, double tqyhmrmxlrf, bool hzmwvwotiw, double zfsmprheiukr, int vwlmfj, bool adouuvzadzxmw);

};


double gelbyfb::nlndpajmnaon(int oxsnjawzet, int oalhfn, int pcdfdbklylr, bool pltko, string stpmgya, double wxipgro, double hnbbtga) {
	return 1664;
}

double gelbyfb::ecdfodfqovnl(bool pguluwr, bool uchfoxxihwrh, bool azohp, int htdan, bool vfournlkcok, int ltyqzkqwkvi, int nndiwtd, int lhzlavjwcz, double kxndsuuvvdg) {
	bool yxyjicpxjpfsn = true;
	bool kjiolkw = false;
	string tzatbwwdr = "sdzktepwoohbkphccgkkqnhayvtplylmxjzay";
	bool xmkuf = true;
	int wnjqflntcmnyi = 1098;
	int kfqfqm = 2539;
	bool kfselivjrbiv = false;
	string amrqkfr = "epvusotkvbevrsbjzuxjqlojzxniszlzvmqdkgcainihexerjexsrwxspbyrhhht";
	double xwznctlqabtqaeq = 26201;
	if (26201 != 26201) {
		int xblgzbof;
		for (xblgzbof = 93; xblgzbof > 0; xblgzbof--) {
			continue;
		}
	}
	if (string("sdzktepwoohbkphccgkkqnhayvtplylmxjzay") == string("sdzktepwoohbkphccgkkqnhayvtplylmxjzay")) {
		int kch;
		for (kch = 54; kch > 0; kch--) {
			continue;
		}
	}
	if (1098 != 1098) {
		int ricxq;
		for (ricxq = 94; ricxq > 0; ricxq--) {
			continue;
		}
	}
	if (2539 != 2539) {
		int sbowwh;
		for (sbowwh = 5; sbowwh > 0; sbowwh--) {
			continue;
		}
	}
	return 89081;
}

double gelbyfb::htkttgmvutszvadhbnep(bool fvewmc, int alehch, int fwdvjjdz, double xqdgbs) {
	return 13802;
}

double gelbyfb::dzwyevrgaajnbqp() {
	double mygzpccuyjfqbc = 38311;
	string ockjzthcy = "dxpundyqowdypxwgzeztnybzviob";
	bool ltiegrox = true;
	bool iuqdwvcjjco = true;
	bool pzovqloksvwbbpw = true;
	int cdjzrizvmki = 2481;
	double gdthqcagodvjbd = 14777;
	if (true != true) {
		int mfyaifkm;
		for (mfyaifkm = 43; mfyaifkm > 0; mfyaifkm--) {
			continue;
		}
	}
	if (2481 != 2481) {
		int nbplbmjrzv;
		for (nbplbmjrzv = 42; nbplbmjrzv > 0; nbplbmjrzv--) {
			continue;
		}
	}
	if (true != true) {
		int deu;
		for (deu = 81; deu > 0; deu--) {
			continue;
		}
	}
	if (2481 != 2481) {
		int wwtxtyqlef;
		for (wwtxtyqlef = 34; wwtxtyqlef > 0; wwtxtyqlef--) {
			continue;
		}
	}
	return 52213;
}

void gelbyfb::tjoyhcvfnfftfosrat(double irkphign, bool jmirzv, int relkoorvvgq, bool wrllvriyznelnxm, string mjlkpoinucdsbk, double aywrnagasfueed, double fzlqktswdfpjitk, int lknmarbclfwj, string faxmzubjzd) {
	double xxzkejkctpe = 21674;
	int uwjkdiwfrseyn = 184;
	bool rxyzqfbohjwzq = false;
	string ffnliuxt = "vriyfgxszcsklvpzmbewqthrrodbyrjbxujvfzjyhrjnuplcfjifdgztyjjuwnbkdkyk";
	double afavorrriggcm = 30520;
	if (21674 == 21674) {
		int wdkf;
		for (wdkf = 14; wdkf > 0; wdkf--) {
			continue;
		}
	}
	if (false != false) {
		int wegxccnbm;
		for (wegxccnbm = 17; wegxccnbm > 0; wegxccnbm--) {
			continue;
		}
	}

}

int gelbyfb::wwmaunwgpezfcod(bool uwbhjykt, int awjmqps, int bsqhlrbekitjlj, double yqnieiyaoh, int atffg, bool iasjanquhhew) {
	return 84045;
}

void gelbyfb::itrfjccozaobxadftd(string olnfuvhwirvmgj, bool hyoxeudsd) {
	int lcvbbbgoske = 948;
	string tgechlbhbepy = "saexnsngjcjchehjpwcgqmvizgage";
	bool wpquzgwb = true;
	bool tvukzubjcguso = true;
	int opqzbctvpzm = 770;
	string typndvugknspf = "mdooebxrpssmkkjjusfffbikjdnxnetkqxzayhchllir";

}

void gelbyfb::hantmuudmxobeqemczweqkars(int eskakn, int eaervlbocmdk, bool hzbylgyt, string dmhhyvefwyru, bool fvtegufr, string fkqmqhiekkeogt, double gdgmoqgvcev) {
	int tfsrqn = 1884;
	double jgmjqxuwvk = 35199;
	bool gdwrulhv = false;
	double xeonyzvpvvq = 22229;
	double khgbypit = 36968;
	double ktkgijbxpedkc = 18407;
	bool nnkkdvuappo = true;
	double cedvbditmh = 12679;
	double asxxnan = 72748;
	if (12679 != 12679) {
		int glhmu;
		for (glhmu = 82; glhmu > 0; glhmu--) {
			continue;
		}
	}
	if (true != true) {
		int nxdafz;
		for (nxdafz = 52; nxdafz > 0; nxdafz--) {
			continue;
		}
	}

}

int gelbyfb::srtkdvqhftbmkvjqudgzazgu(double octdkagghhlvmyq, bool jzblruhhwvkla, bool naqxwaldipimh, bool tnnyxzzkhinuh) {
	return 86468;
}

bool gelbyfb::mnnsqhnthgcpbz(int gsdfe, int qihsb, string ahnfsdlnqfkpfvt, double tqyhmrmxlrf, bool hzmwvwotiw, double zfsmprheiukr, int vwlmfj, bool adouuvzadzxmw) {
	string ctugbenza = "nbglrwqftnsvowwanolzefpcbplrhlqfpodcahrxiccrfyaccbkfdakhmbuzvjct";
	bool vdeczz = false;
	int cihopxy = 2228;
	int qkckehvyogdqid = 887;
	double lshlzb = 16865;
	string thfcfopijihkt = "taqabouwibtosvfvkdmvcqqwamgahdldwjelegghyolnvrpljsjqnzhxkjodinritfftaanzrvvxs";
	if (16865 != 16865) {
		int tulvjjyy;
		for (tulvjjyy = 43; tulvjjyy > 0; tulvjjyy--) {
			continue;
		}
	}
	if (887 != 887) {
		int natw;
		for (natw = 59; natw > 0; natw--) {
			continue;
		}
	}
	if (string("taqabouwibtosvfvkdmvcqqwamgahdldwjelegghyolnvrpljsjqnzhxkjodinritfftaanzrvvxs") != string("taqabouwibtosvfvkdmvcqqwamgahdldwjelegghyolnvrpljsjqnzhxkjodinritfftaanzrvvxs")) {
		int ndr;
		for (ndr = 61; ndr > 0; ndr--) {
			continue;
		}
	}
	return true;
}

string gelbyfb::bmbdsjmopanszgh(bool giepk, int vskflzadsfeldk, double ztkzb, bool gzpjiyv, bool kxeyaplcrizbtjg) {
	double maetafwvzyiml = 45053;
	int urwtzguukvurbzr = 2493;
	bool aijdv = false;
	int yuidm = 5637;
	double mnfroynntsp = 32025;
	string xbglonqixkjej = "qovpqhsoomijpxkwfhvgyhzllfqeuesspnpadqmzpmeqbdmagyuqgtavbgchksmxorpctofczlmecywivjchawz";
	bool flquvusvvyo = false;
	double rhqbgfpbrik = 49105;
	double ruadhsqs = 6361;
	string tfxxymrlchobg = "nsnpilkxikbnoqutqvklxxq";
	if (32025 != 32025) {
		int lvsorv;
		for (lvsorv = 35; lvsorv > 0; lvsorv--) {
			continue;
		}
	}
	return string("vzwpjunul");
}

bool gelbyfb::lwekibwpofydeswpg(double mrlfydawzsbmtb, int zxghfodsig, bool hnzhgrtgbcofjx, int bgmhaenspxfmwwz, bool irswpky, int lsshsneechewnr, double idjwxtntkbf, string fmhlpbnufona) {
	bool yzbrrhqpxweyhg = true;
	string uazzibmlnyskj = "ozrnfpjweslonfhrszpupuhkhpjkjddiagsicqkbjzpzjtgugrf";
	if (true != true) {
		int tumamys;
		for (tumamys = 74; tumamys > 0; tumamys--) {
			continue;
		}
	}
	if (true != true) {
		int qzboft;
		for (qzboft = 86; qzboft > 0; qzboft--) {
			continue;
		}
	}
	return true;
}

bool gelbyfb::zfdmhuvptuwjvdknfeypv(bool sdnqikwfndtpbc, string oqrklovp, string lrxlewqvrxxmnss, string knrrdtmgpxhsww, double puisbpclwf, string yalqh) {
	double qivshzhgdo = 11568;
	bool acmnrzdkwbumg = true;
	string xbiwxssxclun = "qrieuhpvkwsyrbczypebdnopzavodtweiqijosffakaajjvllwyfjtwgnknrimnrifytpnomnxhucplmrmdkcbyqt";
	int cerpbk = 1009;
	string lkhjuyfhqclsx = "vxfrwjbhyjpcbrsmpfetwnntwlpojtvvwaotmmltcithjkavkyzfcujtueseyqdqzwqdbttlophkqsxbblwhqkcvhvx";
	if (true != true) {
		int givsdbk;
		for (givsdbk = 49; givsdbk > 0; givsdbk--) {
			continue;
		}
	}
	if (11568 == 11568) {
		int bus;
		for (bus = 43; bus > 0; bus--) {
			continue;
		}
	}
	if (true == true) {
		int dsrmsrlgs;
		for (dsrmsrlgs = 29; dsrmsrlgs > 0; dsrmsrlgs--) {
			continue;
		}
	}
	if (11568 == 11568) {
		int hiqhdn;
		for (hiqhdn = 20; hiqhdn > 0; hiqhdn--) {
			continue;
		}
	}
	if (string("vxfrwjbhyjpcbrsmpfetwnntwlpojtvvwaotmmltcithjkavkyzfcujtueseyqdqzwqdbttlophkqsxbblwhqkcvhvx") == string("vxfrwjbhyjpcbrsmpfetwnntwlpojtvvwaotmmltcithjkavkyzfcujtueseyqdqzwqdbttlophkqsxbblwhqkcvhvx")) {
		int uqrolfrwr;
		for (uqrolfrwr = 84; uqrolfrwr > 0; uqrolfrwr--) {
			continue;
		}
	}
	return false;
}

double gelbyfb::gcprkqxqsvn(double tchbd, string vbwwox, string oqmkwhyhlmqfr) {
	string xppbxmoopcdg = "oygjmrjudeniyblqluzzpzqejxbwukibdwnxof";
	double jejrzfpcterj = 59340;
	return 56714;
}

bool gelbyfb::hfmzvmdlnj(string rufiegjdcqm, bool chysthhn, string hfnhjzlyrwvwqe, bool dbfgsfenppbnqi, double uowqixq, bool uighxilpmdpxjyg, int efxdkzupcox, string oixjhkbwowirfwa) {
	int mkxaak = 1726;
	bool ocvzhbqfd = true;
	bool qplflggtspjnhqe = false;
	bool akhykwhnhwtz = false;
	string dlmlby = "psoxpswupmidqvzhqljorukvfejqygbxictoxqkitmgyvlqkunnjejvrdkzr";
	double szfntyuan = 7590;
	double qcgiiafowzr = 2014;
	bool sqgsa = false;
	double fneoq = 82636;
	if (true == true) {
		int gzhxe;
		for (gzhxe = 69; gzhxe > 0; gzhxe--) {
			continue;
		}
	}
	if (1726 == 1726) {
		int gclvcwiqgh;
		for (gclvcwiqgh = 56; gclvcwiqgh > 0; gclvcwiqgh--) {
			continue;
		}
	}
	return true;
}

void gelbyfb::uzuhdsvemzeds(string srdrdkhfkmadldp, int yaxvyxhui, double exrsjxv, bool pehvs, bool zkbpbqvvvwqkc, string rbgyfwssdzqvue) {
	bool znqnxcqg = false;
	bool karlqlbsq = true;
	int emnimfhscuvx = 483;
	bool kccxuejyadxpb = false;
	string czcmgd = "hbpggwizzqxbjtjwiujwxaxauyzanqpljvdeqvdcsghirlcuqgoovuhsireruvczvcosfzghty";
	int tfykx = 3968;
	bool yfofwcwltzkkq = true;
	int exnodopopzlcauw = 436;
	int xgwdgnkdcftz = 2078;
	double yokmkwwc = 36719;
	if (string("hbpggwizzqxbjtjwiujwxaxauyzanqpljvdeqvdcsghirlcuqgoovuhsireruvczvcosfzghty") != string("hbpggwizzqxbjtjwiujwxaxauyzanqpljvdeqvdcsghirlcuqgoovuhsireruvczvcosfzghty")) {
		int abqcqfrrid;
		for (abqcqfrrid = 70; abqcqfrrid > 0; abqcqfrrid--) {
			continue;
		}
	}
	if (3968 != 3968) {
		int enkeuhxhem;
		for (enkeuhxhem = 41; enkeuhxhem > 0; enkeuhxhem--) {
			continue;
		}
	}
	if (483 != 483) {
		int huakbdu;
		for (huakbdu = 50; huakbdu > 0; huakbdu--) {
			continue;
		}
	}

}

void gelbyfb::ajpzrusamdzb(double yqwauxgqhwz, bool zwzfoernk, string qloto, double cpnsztrukksha) {
	double iqzrrkki = 26695;
	double mvtul = 8741;
	string ltgxmecewnphhu = "pryqlwfwbcsuywqpgvfumpyw";
	bool dyybnhgkjezuh = false;
	int mykdnpcpw = 9172;
	string tnbtub = "dkbpscsuuuqdemwsjueupymgdf";
	if (string("dkbpscsuuuqdemwsjueupymgdf") == string("dkbpscsuuuqdemwsjueupymgdf")) {
		int sc;
		for (sc = 30; sc > 0; sc--) {
			continue;
		}
	}

}

void gelbyfb::jbhfqvckjqoujlckhdcjtpi() {
	string gsgwn = "jqzkdiitmaguhhscmvdiotzzywgybjdgzqrcmuyqdejsjfxmuztlwxmuopmpanwistvwytlgef";
	bool qduvor = false;
	double rumyoj = 22668;
	double ykxpeezqwxqd = 66867;
	int utynlcrnxmm = 2162;
	bool rmifcdwygmppob = true;
	int nqiigdffkxzfse = 3334;
	double eexsgblaima = 60774;
	bool dopirsqkzv = false;
	bool bugylj = true;
	if (22668 == 22668) {
		int hcpjnnr;
		for (hcpjnnr = 1; hcpjnnr > 0; hcpjnnr--) {
			continue;
		}
	}
	if (60774 == 60774) {
		int ygfqneujab;
		for (ygfqneujab = 56; ygfqneujab > 0; ygfqneujab--) {
			continue;
		}
	}
	if (66867 != 66867) {
		int ritkfqlcm;
		for (ritkfqlcm = 18; ritkfqlcm > 0; ritkfqlcm--) {
			continue;
		}
	}

}

bool gelbyfb::rehcmfuqomxnezbuskauqm(int xidvrat, bool liafvdw, int njwncdyqsypftnj, int aypvyxullwn, string nbqws) {
	double uwjpdisk = 42818;
	if (42818 != 42818) {
		int lqdua;
		for (lqdua = 89; lqdua > 0; lqdua--) {
			continue;
		}
	}
	if (42818 != 42818) {
		int ue;
		for (ue = 12; ue > 0; ue--) {
			continue;
		}
	}
	if (42818 == 42818) {
		int vnyomi;
		for (vnyomi = 86; vnyomi > 0; vnyomi--) {
			continue;
		}
	}
	return true;
}

int gelbyfb::wrkxunkvmotkvw(double wdbtfa, bool rzkzo, string elfifaa, double cwwljskyxp, int llvouzvvnf, bool ltfdqrmkxwvfvjm) {
	string vyuuxz = "bzrcyckkjccirwqdndvvmrghamifbzpdzlbelndbymzcwwjy";
	int pyliqpewh = 613;
	string ccqzipcdlejiqtp = "ebhutbjwyvixkonxrtldggdhqhpj";
	string hojwnqa = "slsicppscxlpbqsyhwz";
	double tgwijxrk = 14476;
	double unvldmlilfo = 46976;
	int mkrxo = 3885;
	if (14476 != 14476) {
		int rinfmemau;
		for (rinfmemau = 13; rinfmemau > 0; rinfmemau--) {
			continue;
		}
	}
	if (3885 != 3885) {
		int drx;
		for (drx = 63; drx > 0; drx--) {
			continue;
		}
	}
	if (string("slsicppscxlpbqsyhwz") == string("slsicppscxlpbqsyhwz")) {
		int wnipf;
		for (wnipf = 75; wnipf > 0; wnipf--) {
			continue;
		}
	}
	return 88120;
}

gelbyfb::gelbyfb() {
	this->gcprkqxqsvn(5834, string("gcljtpwmpxvynlwxwmykksrkhqzxbmujhttysadmsemvgmvhpmerahvhqmzfvbgtjrjvhdlywyplvdhxmsqvmmjbguxlywrpiu"), string("gkvuboluesdizlnvdzvwgsjwsvkzxuygds"));
	this->hfmzvmdlnj(string("xhxolbcxnmhmgejyvzqiavpuwrnomjqdebfaiuxmgnyvzq"), false, string("lqinaqrutrcbuiotxmuggwhhdrlhulmbqjgiivwffutxcspucbrctidhjjvwey"), false, 18713, false, 2498, string("wocuvkmmdxrcxgovrlujiqviszlmeolrztffayldilxjmaenst"));
	this->uzuhdsvemzeds(string("asmjaclipqvgmcsordogrewhiuyfnzepkzwppzklonmlzjeyjxstrpyrtbutujtyisavildlunszfqhyhcgyuz"), 3921, 12763, true, false, string("kagfgvfbdgdok"));
	this->ajpzrusamdzb(12547, true, string("rgwnqlycuymaytncalhvrpdczwtgrqxuduigfvijzwxpe"), 2447);
	this->jbhfqvckjqoujlckhdcjtpi();
	this->rehcmfuqomxnezbuskauqm(3318, false, 2985, 3351, string("snkrtumvdwwffndhtayrjnsehssppofovbctszavxgybsspeneneegghyxmwyvpehumyodkaqpkialdrkhbmix"));
	this->wrkxunkvmotkvw(1497, false, string("zyzlhmnhqrctsnfnaevnmzmwfhqbntbjpdwamdgqrfmocfnncntexilwxpzoavbgefydox"), 63788, 1047, true);
	this->bmbdsjmopanszgh(true, 1395, 40770, true, false);
	this->lwekibwpofydeswpg(60180, 6046, false, 104, true, 4813, 46363, string("pnircuerwgireevgheaqmwnipo"));
	this->zfdmhuvptuwjvdknfeypv(true, string("uxoxakpvdnlmfshaacgxlfwyaeycwwvyyfprnrf"), string("oatvttodblkshziraxyihkhiwlszqhoiugtsapkkqgqvnflsueuvqaja"), string("jccomegfdplvfstqhbqvchrjsvodoqkqywzdgwhdjtiphjvrddbrbzqhyapdhnhzfipswnodlaayftcku"), 56240, string("dahyemncze"));
	this->nlndpajmnaon(7251, 2411, 3762, true, string("oyogdztop"), 65760, 11291);
	this->ecdfodfqovnl(true, false, false, 2797, false, 4258, 7943, 2936, 16004);
	this->htkttgmvutszvadhbnep(false, 9199, 3640, 13964);
	this->dzwyevrgaajnbqp();
	this->tjoyhcvfnfftfosrat(63893, false, 4788, true, string("iiyjscmnzbiltkqyaskxywakrsqscqcjpoivghufixz"), 8584, 54778, 1080, string("lktteoapcvdihrcvaeletkswsttwsahrkuwvnwayhrz"));
	this->wwmaunwgpezfcod(true, 1131, 5001, 58151, 3069, true);
	this->itrfjccozaobxadftd(string("lbbq"), true);
	this->hantmuudmxobeqemczweqkars(256, 5578, false, string("kunqqawecnllkcpmzxippxxuzwnwwdksihbxdauqdgvcsvgyi"), false, string("dvmresifoyxoiulqgoxyvy"), 5736);
	this->srtkdvqhftbmkvjqudgzazgu(13657, false, false, true);
	this->mnnsqhnthgcpbz(301, 5772, string("tksgncxvxajupgolsf"), 9341, false, 35613, 3000, true);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jsfxxrj {
public:
	string vexmtk;
	int picmc;
	bool glsoabrt;
	string qkvzslg;
	bool cpeytue;
	jsfxxrj();
	int ygafabroobotdtppg(string qhfqqxayhzqsy, string ikknpvowqkimfw, string vvqfgiwxwcgqh, string dwfpljlxgfvevi, bool wjumupnl, string mylomi, string myowuluhmczc, string kbgrasz, double soashdw);
	double ahzlkwtavxcohzxgjcunpk(bool uaxxsquxhqypsna, string hufxprqpn, double hmpctiqvzueozi);
	string xqzhcugtcz(bool bvarabzdtapnyc, bool ewpmo);

protected:
	int xkuqywgkpknkqev;
	int icebxd;
	string hypmdkg;

	double idhibkpxyirlucqdsy(string mdnaridyvoq, string hijwzyqo, string xueicbojfez, double uwcatzvruxiwy);
	int koxeszrrfimdvcr(double ppufasbpssxuoe, double zzkrlvaglrkt, bool vqqcuoqtgipti, string svqevcgkosfvlb, double ogkwohcswcwylly, int swyvdlwglmbie, int xeqbp);
	bool pdwamygtvdkpxufm(string vpfxlzizzt, string gpzgmwdef, bool lymhgqzefvjlc, string kakfckwmrvlbg, double pqljql, string fkcrxiktgqu);
	int efmhtukgehoyqjsvdj(int aguzyd);
	int ruwlzyszhbztrymaj(bool cuqvouukzysdybd);
	void nwpjwiuczaorbx(string oavgrjyymi, string dektlmz, int tzsmqftyc, string yridwmnzsrtpfst, bool peiqefgean, bool obtrvojoceueowx, string yzpyjffkey, int fviaiux);
	bool qsevwaaimletpgcxfkrn(string lubqlszpfubikm, string apfwjhqqeaz, string ophxq, double zvrwluj, bool gqgnwpvxay, int ckpdpsvwglwm, bool sncdvutexb, int rnjjprcfw);
	bool wulwuuydplpgjfpgrrefsq(double iwawizcfh, bool vucilszaevq, string jabfmnjgnl, double lmxetfowqbjm, bool cbhdjiwb, bool hzafegpcbaqny);
	bool vjlmovfonakxjrbrv(double vtvhvck, string xkgfkoxw, int wrqom, int mxioz);

private:
	int rlcjbhuirw;
	bool dikbr;
	double kqckphyldwc;
	string ljxzdldtkocm;
	double faeniftyc;

	void pjhtanuhbahv(string dwgifw, bool orhtvr, double ugdlsdxy, string ppykdexyljilkk, bool cqodijt, int dtgpk, bool ofwrc, bool oydkl);
	void ehfbzatxor(double ycqlxyzaxlibmou, bool hjpyj, string acjld, double juadzuzxfwamezm, bool xmeuoldvmi, int ljgkbmvcclopd, string vfkeycy);
	string pmnjjyliabyczk(bool hdtdunj, bool pnjfmsvlgct, double ckdexwxwtmpva, int dzgjknz);
	bool xuqzcnnhvgkkqeeuln(double dkundenpdtob, double qwmcwucbkcfygr, bool lynxwxwb);
	int syvubsbprcasgcmjcittwf();

};


void jsfxxrj::pjhtanuhbahv(string dwgifw, bool orhtvr, double ugdlsdxy, string ppykdexyljilkk, bool cqodijt, int dtgpk, bool ofwrc, bool oydkl) {
	bool yjkrvd = true;
	int dzboptgjfgbhv = 4225;
	bool mhduqml = false;
	int qliubuew = 2923;
	string isbsm = "jpktrzmdnwypjetmbmalehfnbqa";
	bool nsgkunfzwjszln = true;
	int tnombr = 549;
	if (false == false) {
		int sxdisulu;
		for (sxdisulu = 47; sxdisulu > 0; sxdisulu--) {
			continue;
		}
	}
	if (2923 == 2923) {
		int fby;
		for (fby = 77; fby > 0; fby--) {
			continue;
		}
	}
	if (549 != 549) {
		int td;
		for (td = 100; td > 0; td--) {
			continue;
		}
	}

}

void jsfxxrj::ehfbzatxor(double ycqlxyzaxlibmou, bool hjpyj, string acjld, double juadzuzxfwamezm, bool xmeuoldvmi, int ljgkbmvcclopd, string vfkeycy) {
	bool eqhphft = false;
	bool ckkzsjhewknxvwf = true;
	double lpzcoajsk = 31017;
	double evphl = 89665;
	bool ottlwdnmpf = true;

}

string jsfxxrj::pmnjjyliabyczk(bool hdtdunj, bool pnjfmsvlgct, double ckdexwxwtmpva, int dzgjknz) {
	int jvnslukfztlb = 308;
	bool jnbudjabb = true;
	bool tufnszkvdkaguh = false;
	bool flgkmjczuzkqyq = true;
	int aglywsmrfbvrewd = 890;
	if (false != false) {
		int jtwqkomw;
		for (jtwqkomw = 73; jtwqkomw > 0; jtwqkomw--) {
			continue;
		}
	}
	if (308 != 308) {
		int nazmfqrrca;
		for (nazmfqrrca = 22; nazmfqrrca > 0; nazmfqrrca--) {
			continue;
		}
	}
	return string("cbismxvezpij");
}

bool jsfxxrj::xuqzcnnhvgkkqeeuln(double dkundenpdtob, double qwmcwucbkcfygr, bool lynxwxwb) {
	double hobyrqsga = 13790;
	double hggfazt = 10094;
	bool ajhit = true;
	bool uakdooaodzvw = false;
	bool xnuwfruzxrs = true;
	double nluiqlym = 36867;
	double yqwqzlu = 15535;
	bool ljmnmswzuyzzmag = true;
	int phbfbwqjmqzkoyo = 4055;
	if (15535 != 15535) {
		int qigm;
		for (qigm = 2; qigm > 0; qigm--) {
			continue;
		}
	}
	if (13790 != 13790) {
		int xc;
		for (xc = 97; xc > 0; xc--) {
			continue;
		}
	}
	if (36867 != 36867) {
		int yv;
		for (yv = 6; yv > 0; yv--) {
			continue;
		}
	}
	return false;
}

int jsfxxrj::syvubsbprcasgcmjcittwf() {
	string ywgdwrva = "fetb";
	double xwguagmraozhivm = 10615;
	int cggygacrtoq = 1871;
	bool srphtlvuodadeb = true;
	double ixzrkcvpnjbu = 25397;
	string eoige = "iykr";
	return 50505;
}

double jsfxxrj::idhibkpxyirlucqdsy(string mdnaridyvoq, string hijwzyqo, string xueicbojfez, double uwcatzvruxiwy) {
	bool utqftgtsrx = true;
	bool fhbyrrnigsmlov = true;
	string tuvoy = "ygvmlfibedivwpagzyixyakcvczwymmuyrraxiizxglhfofazpadwpkmiyzdqtcwrgwajbnzbucjzl";
	string bkpeg = "xvrhdnemeohuhstjbhlbslfgpaymdhrkpzdebwrrbvwc";
	int gdtdquvplh = 1115;
	double nnptbzskrrh = 56723;
	bool fmnizltumkuwgtw = true;
	bool skryjoxolklotg = true;
	string trqjcugxpowq = "xiowjamxnudikbncxghgwnlyfhmnbgfqrl";
	if (string("xiowjamxnudikbncxghgwnlyfhmnbgfqrl") == string("xiowjamxnudikbncxghgwnlyfhmnbgfqrl")) {
		int kpxm;
		for (kpxm = 48; kpxm > 0; kpxm--) {
			continue;
		}
	}
	if (true != true) {
		int hkuire;
		for (hkuire = 25; hkuire > 0; hkuire--) {
			continue;
		}
	}
	if (true != true) {
		int xdqrnkayjk;
		for (xdqrnkayjk = 19; xdqrnkayjk > 0; xdqrnkayjk--) {
			continue;
		}
	}
	if (true == true) {
		int zoj;
		for (zoj = 70; zoj > 0; zoj--) {
			continue;
		}
	}
	return 33004;
}

int jsfxxrj::koxeszrrfimdvcr(double ppufasbpssxuoe, double zzkrlvaglrkt, bool vqqcuoqtgipti, string svqevcgkosfvlb, double ogkwohcswcwylly, int swyvdlwglmbie, int xeqbp) {
	double hhetrnzyaqi = 74273;
	bool bkfxepkwro = true;
	bool ulkixecmekd = true;
	double rajleb = 11253;
	bool eoraivfeboqwf = false;
	string zcumzr = "ipqvdyidlhhsayvigdbphzdkrqaujgcwpafsthktmivmonejlwjbiruwzbocpinltzkfcezlnwwngehplcm";
	bool kyacjkbqiamnis = false;
	bool erdxqxexmalo = true;
	if (true != true) {
		int zrqvvse;
		for (zrqvvse = 47; zrqvvse > 0; zrqvvse--) {
			continue;
		}
	}
	if (74273 == 74273) {
		int gt;
		for (gt = 36; gt > 0; gt--) {
			continue;
		}
	}
	if (false == false) {
		int bamtsjf;
		for (bamtsjf = 17; bamtsjf > 0; bamtsjf--) {
			continue;
		}
	}
	if (true != true) {
		int aktnqattw;
		for (aktnqattw = 73; aktnqattw > 0; aktnqattw--) {
			continue;
		}
	}
	if (11253 != 11253) {
		int vtefntkw;
		for (vtefntkw = 81; vtefntkw > 0; vtefntkw--) {
			continue;
		}
	}
	return 66916;
}

bool jsfxxrj::pdwamygtvdkpxufm(string vpfxlzizzt, string gpzgmwdef, bool lymhgqzefvjlc, string kakfckwmrvlbg, double pqljql, string fkcrxiktgqu) {
	int zkzxzkknaepuhfn = 2036;
	int dmids = 2842;
	string jekpdm = "onksyxujrlpnzhqqtwgooqdqceuqfxajqgljwnk";
	int bljpgzoh = 3754;
	double igbmzzc = 59274;
	string rlvhnoe = "wtudcyrdfjbrkodkodotrgacuapailroixokfqulbkqlhnntxvjrmmdjdlxpmwuzydfmxjrblbole";
	string dxjkqwvkfswvh = "jrrlravdnqyxrmnyjlreazruarfzfommypltoikdlrundfquhljgvobexrkueqothfxtbovlbqmxnwwbhzipooobpjqongekv";
	if (string("onksyxujrlpnzhqqtwgooqdqceuqfxajqgljwnk") == string("onksyxujrlpnzhqqtwgooqdqceuqfxajqgljwnk")) {
		int jykvgaafog;
		for (jykvgaafog = 40; jykvgaafog > 0; jykvgaafog--) {
			continue;
		}
	}
	return false;
}

int jsfxxrj::efmhtukgehoyqjsvdj(int aguzyd) {
	string cozxkqmtpsewjzl = "micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv";
	if (string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv") == string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv")) {
		int pnsn;
		for (pnsn = 80; pnsn > 0; pnsn--) {
			continue;
		}
	}
	if (string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv") == string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv")) {
		int wlcjocfs;
		for (wlcjocfs = 14; wlcjocfs > 0; wlcjocfs--) {
			continue;
		}
	}
	if (string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv") == string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv")) {
		int pntf;
		for (pntf = 22; pntf > 0; pntf--) {
			continue;
		}
	}
	if (string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv") != string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv")) {
		int uriia;
		for (uriia = 23; uriia > 0; uriia--) {
			continue;
		}
	}
	if (string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv") != string("micrvgujpnfrtykajjrpjhombvwijsamzhoikknsjjbezgyhlonvbbjygxeetxjlkbrifirv")) {
		int xvyinussop;
		for (xvyinussop = 9; xvyinussop > 0; xvyinussop--) {
			continue;
		}
	}
	return 54168;
}

int jsfxxrj::ruwlzyszhbztrymaj(bool cuqvouukzysdybd) {
	double preqevdrseazogi = 21023;
	bool nsdduhrra = false;
	int fazhagbcmhpvc = 193;
	int gdyge = 356;
	int xbvqosfewt = 554;
	double iellplx = 17911;
	bool irvekqtkczowgdq = false;
	if (356 != 356) {
		int fusm;
		for (fusm = 61; fusm > 0; fusm--) {
			continue;
		}
	}
	if (356 != 356) {
		int uqtozwv;
		for (uqtozwv = 46; uqtozwv > 0; uqtozwv--) {
			continue;
		}
	}
	if (false == false) {
		int odqvhs;
		for (odqvhs = 62; odqvhs > 0; odqvhs--) {
			continue;
		}
	}
	if (21023 == 21023) {
		int qpfwd;
		for (qpfwd = 56; qpfwd > 0; qpfwd--) {
			continue;
		}
	}
	if (17911 != 17911) {
		int qsdf;
		for (qsdf = 9; qsdf > 0; qsdf--) {
			continue;
		}
	}
	return 1051;
}

void jsfxxrj::nwpjwiuczaorbx(string oavgrjyymi, string dektlmz, int tzsmqftyc, string yridwmnzsrtpfst, bool peiqefgean, bool obtrvojoceueowx, string yzpyjffkey, int fviaiux) {
	bool opvzmk = true;
	string pgxnyfgikavf = "tucjxgznzxelreotcwttrfbqcrgejcbgqoknpdukxchijijgpxckqxxqcwfyv";
	double zuvjgj = 94149;
	double hacuxrl = 43533;
	bool zgvjvumbluzz = true;
	int kgmamovdp = 1807;
	string wuxyqjwvy = "koevmfzfcdnebhlpoeclfxtbpbuslrjufmgacz";
	bool tddyj = true;
	double fepofpc = 1419;
	bool hothgakg = false;

}

bool jsfxxrj::qsevwaaimletpgcxfkrn(string lubqlszpfubikm, string apfwjhqqeaz, string ophxq, double zvrwluj, bool gqgnwpvxay, int ckpdpsvwglwm, bool sncdvutexb, int rnjjprcfw) {
	return false;
}

bool jsfxxrj::wulwuuydplpgjfpgrrefsq(double iwawizcfh, bool vucilszaevq, string jabfmnjgnl, double lmxetfowqbjm, bool cbhdjiwb, bool hzafegpcbaqny) {
	return true;
}

bool jsfxxrj::vjlmovfonakxjrbrv(double vtvhvck, string xkgfkoxw, int wrqom, int mxioz) {
	bool ptjttzvvlgfezbh = true;
	int bbepkcxi = 2462;
	string oxxthxihwienmb = "xxjeuxnlxdskqfgwkk";
	int dvlul = 1508;
	if (2462 == 2462) {
		int yi;
		for (yi = 71; yi > 0; yi--) {
			continue;
		}
	}
	if (2462 != 2462) {
		int zmilw;
		for (zmilw = 45; zmilw > 0; zmilw--) {
			continue;
		}
	}
	if (true == true) {
		int lmsndqm;
		for (lmsndqm = 70; lmsndqm > 0; lmsndqm--) {
			continue;
		}
	}
	return true;
}

int jsfxxrj::ygafabroobotdtppg(string qhfqqxayhzqsy, string ikknpvowqkimfw, string vvqfgiwxwcgqh, string dwfpljlxgfvevi, bool wjumupnl, string mylomi, string myowuluhmczc, string kbgrasz, double soashdw) {
	double kcivczmgyjh = 8133;
	int uuzsrjfwjjzcl = 6580;
	double sbtwvshgd = 32917;
	bool zodfdn = true;
	double fvxilefl = 82193;
	int xpkkquypyqxnd = 2010;
	double czgfg = 15116;
	bool jrzheem = true;
	int stkmjmq = 2498;
	if (15116 != 15116) {
		int ujzesmi;
		for (ujzesmi = 86; ujzesmi > 0; ujzesmi--) {
			continue;
		}
	}
	return 87226;
}

double jsfxxrj::ahzlkwtavxcohzxgjcunpk(bool uaxxsquxhqypsna, string hufxprqpn, double hmpctiqvzueozi) {
	double kaqxrbnimyfsmqr = 684;
	double nmouzpbmwboa = 5618;
	bool nxzgatnijzcvlcv = false;
	string aurisdfe = "yxjtethgiocjjfvxcqasnqdgytxfmwhyslqebgcmwynaattiktbkunyhbydztequvaqfghqjal";
	double pxhljqthztcuabb = 9833;
	bool pxcmzleemojow = true;
	string jyfepox = "dwueqvalhcdebmzypfitoiycpkjvdfelgpruoutblcisuobatjaflhrlpnuiwzrjrvartrtlbusqb";
	double jfepzs = 37798;
	int ksspgxqslpuyh = 4466;
	bool fkzazpwcbvevxy = true;
	if (false == false) {
		int nnkv;
		for (nnkv = 99; nnkv > 0; nnkv--) {
			continue;
		}
	}
	return 43311;
}

string jsfxxrj::xqzhcugtcz(bool bvarabzdtapnyc, bool ewpmo) {
	return string("qoeun");
}

jsfxxrj::jsfxxrj() {
	this->ygafabroobotdtppg(string("okjudzmfgcnyqgezqyjbijvhhggawouonvfpeyzdzmaygzpqtqprrfvuotrywcaupxcuobekfheyseizmyywzfhkecxnczigss"), string("aeiezggleqyofetxvvyjqxjzfqllscrtofcupzorrczjmfcw"), string("wwjtiwucaheimmtesuffqxzgwidijlnlwuxyyipewodqlfals"), string("tcxknmrvsknsjmcyaentpkwkfrdybfqitpzzlzilgcnfdbmjgzonvkzwybtgxwsmtujgjfx"), false, string("hkrpgbsktjktnwlsetahbyhglufujdntscxcs"), string("rsuvuezywiqzbttquyxwcswhnltokztmkembnzhpsvxmwcdtjprkekhjdquo"), string("awfzjaxaarjuvpwyivgiwtoojgvebpkrrtqkqzmteicvuvfqvezzhlycaeuk"), 33639);
	this->ahzlkwtavxcohzxgjcunpk(true, string("zmrmismmucfgxhlxuvjosorgnqcxrscshmeskptdcftlnkpqcspkeurjvazcuwxvifbwgzcqpisibwudlfhgzsgtyjrmhgrqfx"), 12360);
	this->xqzhcugtcz(true, false);
	this->idhibkpxyirlucqdsy(string("buxflswcaomaidi"), string("iepnuxqaivfgladqssuanwpmenlyyjxgsybweqrnlpxlvqkgymiyorqcrrjmbjsnzbgyv"), string("sgnckusmjhmqwvkoeajdptjojihwagtxyiuhoiwmjrplizpgohaincxffcvizlwafnrzyqb"), 12359);
	this->koxeszrrfimdvcr(6285, 48529, false, string("ypuhfakbspcmsvcxmuybgwkmpxuyxnjmfnelcbodwsqwyeqvxvuaadjoqwwldzuvluhbcqxwsojuuossqonydzfsddmbmmgkr"), 7791, 7147, 3091);
	this->pdwamygtvdkpxufm(string("ruhobwhzynpzniwngytzzdctbfhiggkqwozdbkazsqzpvrhwzzwocalqntannpdvoigclthktzme"), string("shqavvckjaglctvjpfbslvcwbgjvenmctsdwvbmgrfobqrzlurhgrztyizmdeyizypoiegwjwaapxtdahwrtijiadtiucakvensr"), false, string("crydoysxzmxodgwaugtxwh"), 26698, string("yblhrfcarbxfnzaykhmmfvetpbpwstce"));
	this->efmhtukgehoyqjsvdj(603);
	this->ruwlzyszhbztrymaj(false);
	this->nwpjwiuczaorbx(string("dvsmpcufacilpwrdgnofffikenawqaflvivdopfoyjhgszozcbwhvmfrykucgyhhsnracthfscsnmgnzeblbzuhwawexcpyemyar"), string("cizbbrshsmjfq"), 1965, string("kcwdzqedqfoajubuarcshyphovwmysrpwovjorhicyuxkzqhozmbggrmucicwzxbsrz"), true, true, string("eanyqimplxqmvzuhdlmhtrybfmiodoojftgtibvqfwzm"), 3907);
	this->qsevwaaimletpgcxfkrn(string("fokqzyxhitnlmkdzkotmptmmlkqybnidyxdzjmovwvuqgdwcdqlmwcnbkjrwspr"), string("kglzyaxqbxdsnkbzibstfykzgipjmwhqtpzatmtetskhhzkixxabgbgegqoun"), string("ennjcrxotrnpyylf"), 34990, false, 5308, false, 1753);
	this->wulwuuydplpgjfpgrrefsq(63897, false, string("ordmsbnkamldfledovzyohdrizkyakzntuaoqrnbcoroeuw"), 47466, true, false);
	this->vjlmovfonakxjrbrv(13198, string("dhnmychgyaxprzbvtehohjogzfjdrwkyiosdlapwdrgvsmzdbjpvizw"), 134, 358);
	this->pjhtanuhbahv(string(""), true, 5189, string("wewzosphzznsyaiztmzvlmphhwvloitbemzgkqaqfelbijlovujkgmmw"), false, 6827, false, true);
	this->ehfbzatxor(38633, true, string("detiqnmxxbyrepgregroosqklrzerywelcswppbfzaamrikorzffyb"), 6752, false, 2950, string("wzddohrbqqttoufaiksnhzdvetuieqfknxmxhsoutbswtnnwxgtczrxibjaqphhywfgothnnkib"));
	this->pmnjjyliabyczk(true, false, 16566, 1669);
	this->xuqzcnnhvgkkqeeuln(30079, 11331, false);
	this->syvubsbprcasgcmjcittwf();
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class grnebgs {
public:
	double ljdafzofejjpryu;
	string coynyiipvpleke;
	grnebgs();
	void rmdtjhqwrqsvcfxvd(bool dpyzmbicxosnvt, string wwuivabslis, bool xepqqqninib, double udjzodpcxky, bool ydhztafjoazbbke, string ovmeeklj);
	bool dhpxbbrdrn(string xgtayyzuxh, bool trkjhegxuote, bool elvpxpzveqzihnh, int aagnhaahogldsh, double nqodpb, double uvuslaiivsmtgr, bool oibyple, string kxbcfqtqkylf);
	string ftlihjafrhbkodkflximfq(bool dksmt, bool gsoqfxep, bool cjhaeuoawfnvyg, int iefexdgylvr, string hyflfbnme, string tqozklsby, int btpeszpux, bool yaitjwihjghcgt, string rdlxylbaz);
	string npsidcrorurqfrjmjqkfyvep(int mtdjqu, double yuzzwov, string wfrpajkpp, string emmjdyuawr);
	double qprtbfuuywnd(bool kaeahcpeigfn, string ateyhrobvj, int jrpbjcxsn, int clsxxlrcj, bool ycxnnh, string lzwvkayeibputt);
	int qutuajsllw(string minxfddjmlckf, double pwggoc, bool kntuwrbpve, double znxdygio, bool uqeepiggoaoyswg, string rekrqongish, double pkcqef);
	string cliulizontgxc(bool cazazsaz);
	string hsjeinhlqaslmpbskkvcfdtux(string evavjk, bool aggakv, bool dejubuozvdrbtpj, double gunnreppoqxwict, double cutkfaegnumulgs, string dwjturlaj, string nmynk, string oknlvdl);
	double xihbcxcekrgodkgawelzkuyk(int qaogpebcettwmu, int qnkxlubvwtxeep, bool znnkbrnjjdgfdq, double bxvjwuvqqceq, double wbgczrfns, string ejyuxsuxokcmjdu, double cxpmoyraa, int wbdretc, string onczwasv);
	double jhybgmjbvyzuhiaspgui(double ljxopsiccmtw, bool fhwttxjgboe, double bgyviqkauvblz, string rrtfmatp, double oszbqlmdv);

protected:
	bool bfpidwi;
	bool mtujpamnv;
	string closlnpnpghfjn;
	double ykwxmrogezcvr;

	void bmrzydcgyvw(bool pfbhctbcxrqt, int qghzxzgnfbwojry, bool pjljedxwezaldwz, double wedoiynyr, double lmtcdzcib, string xzfrjrh, int zcuklierkoa, double ygktrkbotavyy, double wpbpnlwy, int xzufjjvqs);
	int igxmtzsqyvsteyrtarcwwaf(string apukcedjeeqlbp, string ydvdpx, double zywvqhhvsv, bool ppamjwsuhdec, double tsrphwkx, bool izhqpretni, string tedlzgzrmmbsmi, int zlwohaqfmnvylm, double wrssywzk, double gckrszwp);

private:
	double zrafhozkzbrwwh;
	int tvyzqmoq;
	bool arojw;
	string yfmzwl;
	string ueoiybgy;

	double hyvsmtboefnrefcvbvnlayehz(int zfwuvyvdsoa, string eyjvf, string dkptugzqtdj, bool fbtnfigyxkab, string cmiietbjslh, int nbslwxrpxndz, bool mgfokkhuoc);
	int mlyrtygjkawurs(bool hjurzrhea, string mllaifjgbcmasee, string owlzzhfqoij, double khmwtorvytkhood, double zabiyyxyhqbyvn, int ummuvrweydb);
	string yhktjysccc(double qxmqqbn, double ztcgraruoxla, bool kbvxup, double oejocgovmrgzq, string txrqcntna, double yubxzzhvvmi, double taqppvxl, int tonhouhdpwpkcac, int detcpqnnofy);
	int nyrmshmkmrnskig(string vvrqbgle, double rxdjsgi, bool lrxdtucsxfwrt, bool ogtbzehjp, string xvyqphawmsmkaj, double gtxbrbogptyjk, bool zlexzviakaonng, double paqipgvbwqzq);
	double jdahartewjoekb(int nkfew, bool ytjwj, int wuxgqnctleiktwl, double utfehyqtmznr, double sdpsvljzf, double fcamyduvipqzq, bool ktqabusxuem);
	string frlaanaxhwfeuijbtleie(bool joxcqwknbvfo);
	string mgenppzmcecdqujpgheekfsu(string pzsilrpeetr, string bkqyresaoqwedcj, double laeogokwfjt);
	string jqgzmzmocxtppfgf(double kvfmd, int eumadyqgzochtmh);
	int lritjzctwdarfpmcklctimaeh(int sudtizwvfmlpt);
	int rhmwucsarwvwqobryhpzxrg(bool hkkoccprsqweit, bool uoxtjskvydwcy);

};


double grnebgs::hyvsmtboefnrefcvbvnlayehz(int zfwuvyvdsoa, string eyjvf, string dkptugzqtdj, bool fbtnfigyxkab, string cmiietbjslh, int nbslwxrpxndz, bool mgfokkhuoc) {
	string gbhywidqoza = "wenjudtghtiokbmleczkikupxaccrdiorqvycnpsrqxxufnglpxcsmwmckkpgmihtwoehhkufnmzvagivpzymbhsnjhycd";
	int nabilnsbkaf = 3518;
	bool eevlsnzzqi = false;
	string ikwfyhxar = "fvqysdgugeregehezvnraapqholvkwufkqgtxyjbdwatsqelpdekwvvqdocbzefwwxysm";
	string bdesreagv = "byikgfyyxddbejyyktxtstltegzgmj";
	bool omxthfrjt = true;
	string genwgyttgbqllv = "dvqycfkfascircrbhbtiotnzklqrwsuuxvymndwkrfipkvvlndhayqafvisvfzoyzamgqcixyvbyrzifcxkwgishw";
	string zttexil = "weixofdfffabtosmvbnwkpkgcwbyrjryw";
	return 33064;
}

int grnebgs::mlyrtygjkawurs(bool hjurzrhea, string mllaifjgbcmasee, string owlzzhfqoij, double khmwtorvytkhood, double zabiyyxyhqbyvn, int ummuvrweydb) {
	int wpjwutwewqk = 2578;
	string azhynbcllewfogu = "";
	bool czdzvrktdhso = false;
	double ztihpuxbwsrqz = 5574;
	if (5574 != 5574) {
		int jo;
		for (jo = 100; jo > 0; jo--) {
			continue;
		}
	}
	if (string("") != string("")) {
		int ojzkl;
		for (ojzkl = 94; ojzkl > 0; ojzkl--) {
			continue;
		}
	}
	if (5574 != 5574) {
		int gibabkj;
		for (gibabkj = 2; gibabkj > 0; gibabkj--) {
			continue;
		}
	}
	return 79185;
}

string grnebgs::yhktjysccc(double qxmqqbn, double ztcgraruoxla, bool kbvxup, double oejocgovmrgzq, string txrqcntna, double yubxzzhvvmi, double taqppvxl, int tonhouhdpwpkcac, int detcpqnnofy) {
	bool tnhpz = true;
	bool qayrrtymqeu = true;
	if (true == true) {
		int ntxmjefjs;
		for (ntxmjefjs = 21; ntxmjefjs > 0; ntxmjefjs--) {
			continue;
		}
	}
	if (true == true) {
		int iwl;
		for (iwl = 65; iwl > 0; iwl--) {
			continue;
		}
	}
	if (true != true) {
		int oqsootqhz;
		for (oqsootqhz = 90; oqsootqhz > 0; oqsootqhz--) {
			continue;
		}
	}
	if (true == true) {
		int vzxpqkyh;
		for (vzxpqkyh = 19; vzxpqkyh > 0; vzxpqkyh--) {
			continue;
		}
	}
	return string("qnbilbzdfdbfe");
}

int grnebgs::nyrmshmkmrnskig(string vvrqbgle, double rxdjsgi, bool lrxdtucsxfwrt, bool ogtbzehjp, string xvyqphawmsmkaj, double gtxbrbogptyjk, bool zlexzviakaonng, double paqipgvbwqzq) {
	int rpjqnotvxbbcir = 2369;
	int hmisjkjzynxv = 7809;
	double cqzhabjgnzuelfc = 4463;
	int qmicnq = 1355;
	double widprfuagzecf = 7649;
	double kbjpuke = 18643;
	double lbncfn = 3555;
	string gywwy = "srvfxzlkgensrkjgxmlzirgmgpbjjvxisieftsuultnyttkbvdqjcqdddfratfnmxujeyawyrxcuznmwohwnnciarfen";
	bool ergaqxopbx = true;
	int rsdakizfrty = 3309;
	if (2369 != 2369) {
		int zccsne;
		for (zccsne = 85; zccsne > 0; zccsne--) {
			continue;
		}
	}
	return 64845;
}

double grnebgs::jdahartewjoekb(int nkfew, bool ytjwj, int wuxgqnctleiktwl, double utfehyqtmznr, double sdpsvljzf, double fcamyduvipqzq, bool ktqabusxuem) {
	return 71826;
}

string grnebgs::frlaanaxhwfeuijbtleie(bool joxcqwknbvfo) {
	bool wfcgadcxwj = true;
	double wmmyvf = 22729;
	int vtqecx = 3444;
	if (22729 == 22729) {
		int zjnkq;
		for (zjnkq = 49; zjnkq > 0; zjnkq--) {
			continue;
		}
	}
	if (3444 == 3444) {
		int ztlkko;
		for (ztlkko = 76; ztlkko > 0; ztlkko--) {
			continue;
		}
	}
	if (22729 != 22729) {
		int fpdl;
		for (fpdl = 78; fpdl > 0; fpdl--) {
			continue;
		}
	}
	if (true != true) {
		int ahpjqblz;
		for (ahpjqblz = 68; ahpjqblz > 0; ahpjqblz--) {
			continue;
		}
	}
	return string("mlalcwskosrjv");
}

string grnebgs::mgenppzmcecdqujpgheekfsu(string pzsilrpeetr, string bkqyresaoqwedcj, double laeogokwfjt) {
	double nizxf = 11838;
	int cihdosciqnwt = 2933;
	return string("gxdnnuitooinilyvffxw");
}

string grnebgs::jqgzmzmocxtppfgf(double kvfmd, int eumadyqgzochtmh) {
	string vozakiaqpy = "sqwgcfcipqepghmhkskwvgbeickzylcmxre";
	double xthbeassyqf = 66117;
	bool btwzwsfdg = true;
	int ymenbsgbmx = 8282;
	string mclkmsadzid = "dxksdvcvenptjhtahpkdzkmsvycfqirerxvxrewjuzmeiddkdw";
	double qhifgad = 76449;
	int punudtmhgkwigka = 6416;
	string msrbfxlgitlq = "aimwxyuytukziuolwfqflhhoxzsnbpctlhogrejjwa";
	string jhlpwpmlnm = "fyrnqhbpumicyaiiovqpdyizbptvpguamaauufsxwdectuesdqvkaqiiard";
	string lgxvsawsdqowlc = "hfgcsztbvpsxzqelfgxruxmvtnfcjd";
	return string("arrdjrahdbvrp");
}

int grnebgs::lritjzctwdarfpmcklctimaeh(int sudtizwvfmlpt) {
	string gqretcmxzjpmul = "";
	bool mgqbcncrekv = true;
	double zyclhjvxzxnv = 7459;
	if (string("") != string("")) {
		int rqfnpn;
		for (rqfnpn = 62; rqfnpn > 0; rqfnpn--) {
			continue;
		}
	}
	if (string("") == string("")) {
		int flwv;
		for (flwv = 22; flwv > 0; flwv--) {
			continue;
		}
	}
	if (string("") != string("")) {
		int my;
		for (my = 34; my > 0; my--) {
			continue;
		}
	}
	if (string("") != string("")) {
		int zqhjivnxy;
		for (zqhjivnxy = 67; zqhjivnxy > 0; zqhjivnxy--) {
			continue;
		}
	}
	if (string("") == string("")) {
		int ky;
		for (ky = 11; ky > 0; ky--) {
			continue;
		}
	}
	return 48664;
}

int grnebgs::rhmwucsarwvwqobryhpzxrg(bool hkkoccprsqweit, bool uoxtjskvydwcy) {
	int yhaqvfejpvv = 145;
	double ddeivxyllizk = 24516;
	int ygcmveusbjk = 5035;
	bool qmwkq = true;
	double rxdquzyvz = 5755;
	double bvvsezyzh = 32182;
	double feuzm = 6470;
	string bzztieaigiuab = "alexsgmxfrvqhhewhfhtzxxepavm";
	if (5035 != 5035) {
		int yigoilfta;
		for (yigoilfta = 22; yigoilfta > 0; yigoilfta--) {
			continue;
		}
	}
	if (5755 == 5755) {
		int icaspnnjq;
		for (icaspnnjq = 68; icaspnnjq > 0; icaspnnjq--) {
			continue;
		}
	}
	return 53087;
}

void grnebgs::bmrzydcgyvw(bool pfbhctbcxrqt, int qghzxzgnfbwojry, bool pjljedxwezaldwz, double wedoiynyr, double lmtcdzcib, string xzfrjrh, int zcuklierkoa, double ygktrkbotavyy, double wpbpnlwy, int xzufjjvqs) {
	string lkcahmsvd = "gdpolioygwewfndtpippbmokydzpajijxtmf";
	string pgehnq = "ljtusfedrkemexckdvtdrjldouayxvk";
	string gcxslashzo = "heodmcqazqdbmcausyhormzlwrrnwljqllpsdflursvzcssvogyfcsovhibusbkfhculdwipglqagqy";
	double dtjmy = 12201;
	string zmqsjblpte = "vmjmlcdsosmsiakifxxndyrgqklqjbnqfxiqnzyhyzfipesmxjlirncmkzmhz";
	bool kjgesymcc = true;
	double mcovdrfwxjzx = 8171;
	if (string("ljtusfedrkemexckdvtdrjldouayxvk") != string("ljtusfedrkemexckdvtdrjldouayxvk")) {
		int orfqlnqdgg;
		for (orfqlnqdgg = 85; orfqlnqdgg > 0; orfqlnqdgg--) {
			continue;
		}
	}
	if (string("ljtusfedrkemexckdvtdrjldouayxvk") != string("ljtusfedrkemexckdvtdrjldouayxvk")) {
		int iri;
		for (iri = 31; iri > 0; iri--) {
			continue;
		}
	}
	if (12201 == 12201) {
		int dgnvunn;
		for (dgnvunn = 47; dgnvunn > 0; dgnvunn--) {
			continue;
		}
	}

}

int grnebgs::igxmtzsqyvsteyrtarcwwaf(string apukcedjeeqlbp, string ydvdpx, double zywvqhhvsv, bool ppamjwsuhdec, double tsrphwkx, bool izhqpretni, string tedlzgzrmmbsmi, int zlwohaqfmnvylm, double wrssywzk, double gckrszwp) {
	int dufzhtbk = 2780;
	double ztgcfoffsbi = 56280;
	int zckpdekzihyoke = 2152;
	int kbxbug = 3222;
	int eboyfcjde = 2869;
	double irlnadnostfkmh = 18970;
	string gflwzixluegkpqi = "wzmhoxqlfryxfchkqua";
	if (string("wzmhoxqlfryxfchkqua") == string("wzmhoxqlfryxfchkqua")) {
		int ir;
		for (ir = 96; ir > 0; ir--) {
			continue;
		}
	}
	if (3222 != 3222) {
		int dcxb;
		for (dcxb = 30; dcxb > 0; dcxb--) {
			continue;
		}
	}
	if (string("wzmhoxqlfryxfchkqua") != string("wzmhoxqlfryxfchkqua")) {
		int wrjrxhr;
		for (wrjrxhr = 51; wrjrxhr > 0; wrjrxhr--) {
			continue;
		}
	}
	return 77923;
}

void grnebgs::rmdtjhqwrqsvcfxvd(bool dpyzmbicxosnvt, string wwuivabslis, bool xepqqqninib, double udjzodpcxky, bool ydhztafjoazbbke, string ovmeeklj) {
	string rpfbalwgkigxrsa = "hwymlukivdhrwbdbfmxaivjecvccfskklbfrpgxfmnouwtlljfposnvlcpz";
	bool soeczmnnro = true;
	bool hslflxy = true;
	double mwvwxmtbd = 921;
	double vwwpnjo = 18441;
	int aqgidvxafnejnbv = 287;
	double ktmmwrrnevf = 54860;
	string yygesfjoxiaes = "owxtzkvtxoyvnlbnxwpkmbbsuxagqazdimlzjcvgignllmrisquomg";
	bool ducczgyb = true;
	if (18441 == 18441) {
		int carddj;
		for (carddj = 54; carddj > 0; carddj--) {
			continue;
		}
	}
	if (string("owxtzkvtxoyvnlbnxwpkmbbsuxagqazdimlzjcvgignllmrisquomg") != string("owxtzkvtxoyvnlbnxwpkmbbsuxagqazdimlzjcvgignllmrisquomg")) {
		int gllyitpd;
		for (gllyitpd = 76; gllyitpd > 0; gllyitpd--) {
			continue;
		}
	}
	if (true != true) {
		int nbxxbe;
		for (nbxxbe = 7; nbxxbe > 0; nbxxbe--) {
			continue;
		}
	}
	if (string("hwymlukivdhrwbdbfmxaivjecvccfskklbfrpgxfmnouwtlljfposnvlcpz") != string("hwymlukivdhrwbdbfmxaivjecvccfskklbfrpgxfmnouwtlljfposnvlcpz")) {
		int qvuqlu;
		for (qvuqlu = 4; qvuqlu > 0; qvuqlu--) {
			continue;
		}
	}

}

bool grnebgs::dhpxbbrdrn(string xgtayyzuxh, bool trkjhegxuote, bool elvpxpzveqzihnh, int aagnhaahogldsh, double nqodpb, double uvuslaiivsmtgr, bool oibyple, string kxbcfqtqkylf) {
	string acxylahtgx = "asrhxqodecxsvyyncgktnkoggtekeal";
	string qppftxmfqlf = "snerglqovkhautwmgwbstrbberkaotjfucmjghnflgldtvwahbptodxfgqquzddkoqxfdlckarpvpjgmv";
	double sbzpqqiaa = 15294;
	if (string("snerglqovkhautwmgwbstrbberkaotjfucmjghnflgldtvwahbptodxfgqquzddkoqxfdlckarpvpjgmv") != string("snerglqovkhautwmgwbstrbberkaotjfucmjghnflgldtvwahbptodxfgqquzddkoqxfdlckarpvpjgmv")) {
		int rudckjrkn;
		for (rudckjrkn = 50; rudckjrkn > 0; rudckjrkn--) {
			continue;
		}
	}
	return false;
}

string grnebgs::ftlihjafrhbkodkflximfq(bool dksmt, bool gsoqfxep, bool cjhaeuoawfnvyg, int iefexdgylvr, string hyflfbnme, string tqozklsby, int btpeszpux, bool yaitjwihjghcgt, string rdlxylbaz) {
	double mwkceiaydnpe = 21968;
	int zsiapelsga = 6865;
	if (21968 != 21968) {
		int uienoets;
		for (uienoets = 26; uienoets > 0; uienoets--) {
			continue;
		}
	}
	if (6865 == 6865) {
		int mvs;
		for (mvs = 64; mvs > 0; mvs--) {
			continue;
		}
	}
	if (6865 != 6865) {
		int xcuxzwv;
		for (xcuxzwv = 35; xcuxzwv > 0; xcuxzwv--) {
			continue;
		}
	}
	if (21968 != 21968) {
		int yttnyyl;
		for (yttnyyl = 40; yttnyyl > 0; yttnyyl--) {
			continue;
		}
	}
	return string("bfetmuvfcqw");
}

string grnebgs::npsidcrorurqfrjmjqkfyvep(int mtdjqu, double yuzzwov, string wfrpajkpp, string emmjdyuawr) {
	bool nslukpnmlcowhrz = false;
	string dzmootkodeig = "btkwhffpvldrqnixcikfkt";
	int ocfkwgxruwwy = 2637;
	double zgfjykebghyjgzt = 9648;
	int wcvvutechzamuoe = 8088;
	int bxhprhswbod = 1052;
	if (1052 == 1052) {
		int rprqh;
		for (rprqh = 13; rprqh > 0; rprqh--) {
			continue;
		}
	}
	if (9648 == 9648) {
		int lmelai;
		for (lmelai = 32; lmelai > 0; lmelai--) {
			continue;
		}
	}
	if (8088 != 8088) {
		int hxmyeaic;
		for (hxmyeaic = 30; hxmyeaic > 0; hxmyeaic--) {
			continue;
		}
	}
	return string("boxpudmifvxx");
}

double grnebgs::qprtbfuuywnd(bool kaeahcpeigfn, string ateyhrobvj, int jrpbjcxsn, int clsxxlrcj, bool ycxnnh, string lzwvkayeibputt) {
	int zrtloxruycci = 3535;
	string dzrvusvrmlkc = "dhvcaahfvtmwaow";
	double fnlyopquifrj = 16093;
	bool wigivbdhmvewf = true;
	string yeoybdivucv = "uanpphojmoyinhenzidcfegeuuukaiivbqigybdzaoxilmoraoosrwtwtzm";
	bool zeocyrg = false;
	int cogaah = 1400;
	string olynfmlax = "gjhsvpsfofmwygmahztuyly";
	string jkyisggfyae = "vjecxkkmgdgvwjfpuiiifoxsobgv";
	double mwnznbmdr = 9295;
	if (string("gjhsvpsfofmwygmahztuyly") == string("gjhsvpsfofmwygmahztuyly")) {
		int zznk;
		for (zznk = 51; zznk > 0; zznk--) {
			continue;
		}
	}
	if (false == false) {
		int jqhrdoemsj;
		for (jqhrdoemsj = 99; jqhrdoemsj > 0; jqhrdoemsj--) {
			continue;
		}
	}
	if (string("uanpphojmoyinhenzidcfegeuuukaiivbqigybdzaoxilmoraoosrwtwtzm") == string("uanpphojmoyinhenzidcfegeuuukaiivbqigybdzaoxilmoraoosrwtwtzm")) {
		int woa;
		for (woa = 21; woa > 0; woa--) {
			continue;
		}
	}
	if (16093 == 16093) {
		int tzdhcgwxid;
		for (tzdhcgwxid = 39; tzdhcgwxid > 0; tzdhcgwxid--) {
			continue;
		}
	}
	if (3535 != 3535) {
		int lxd;
		for (lxd = 91; lxd > 0; lxd--) {
			continue;
		}
	}
	return 10039;
}

int grnebgs::qutuajsllw(string minxfddjmlckf, double pwggoc, bool kntuwrbpve, double znxdygio, bool uqeepiggoaoyswg, string rekrqongish, double pkcqef) {
	string mwqgaizmpkqxy = "mvarvjdvuklzzcpsrimdowugkuj";
	bool qdqtbmeopptkjm = true;
	if (true == true) {
		int depvf;
		for (depvf = 35; depvf > 0; depvf--) {
			continue;
		}
	}
	return 95566;
}

string grnebgs::cliulizontgxc(bool cazazsaz) {
	double sgombmat = 8913;
	string jgfsftuqck = "fwjryqnwyogytsncbldzhxbecubdicrapjfuylfcsboeerahpbspegbupzvlpyiyibrcoebhcwtcptnaljtjfndnjxccn";
	if (string("fwjryqnwyogytsncbldzhxbecubdicrapjfuylfcsboeerahpbspegbupzvlpyiyibrcoebhcwtcptnaljtjfndnjxccn") == string("fwjryqnwyogytsncbldzhxbecubdicrapjfuylfcsboeerahpbspegbupzvlpyiyibrcoebhcwtcptnaljtjfndnjxccn")) {
		int cail;
		for (cail = 23; cail > 0; cail--) {
			continue;
		}
	}
	if (string("fwjryqnwyogytsncbldzhxbecubdicrapjfuylfcsboeerahpbspegbupzvlpyiyibrcoebhcwtcptnaljtjfndnjxccn") == string("fwjryqnwyogytsncbldzhxbecubdicrapjfuylfcsboeerahpbspegbupzvlpyiyibrcoebhcwtcptnaljtjfndnjxccn")) {
		int tjbumf;
		for (tjbumf = 29; tjbumf > 0; tjbumf--) {
			continue;
		}
	}
	return string("fshzwyualqyoagiz");
}

string grnebgs::hsjeinhlqaslmpbskkvcfdtux(string evavjk, bool aggakv, bool dejubuozvdrbtpj, double gunnreppoqxwict, double cutkfaegnumulgs, string dwjturlaj, string nmynk, string oknlvdl) {
	bool rmbkimniclcelrx = false;
	string avelec = "lvcbmxgtopqlayxvxtuodqxbcygftpbzqwmsskokozig";
	bool dubjr = false;
	double jbwuakkrmnnwvi = 48341;
	int makeq = 7368;
	double eglggyv = 453;
	string kzrdkqm = "yzlaxvrweehlsmueifzxhusgopsypnxbhljeeiqsiuhodichweyoznhocjwakohrnvulfvvywetwjmyauxwbhvflqfdehjuo";
	string fmsiwlkvnm = "z";
	return string("ralkhcrl");
}

double grnebgs::xihbcxcekrgodkgawelzkuyk(int qaogpebcettwmu, int qnkxlubvwtxeep, bool znnkbrnjjdgfdq, double bxvjwuvqqceq, double wbgczrfns, string ejyuxsuxokcmjdu, double cxpmoyraa, int wbdretc, string onczwasv) {
	string zltzwrn = "fcenizfmrqvtpfotwqxrynixfortrpsxnpzfqekukxxmynvnlwazuxtdcsierkcmpugej";
	double ntkukuqnd = 26862;
	string xrpwdwmgjryuxo = "yhbegnzrykarmutgpkz";
	bool gmliapgv = true;
	if (string("yhbegnzrykarmutgpkz") != string("yhbegnzrykarmutgpkz")) {
		int embkcjpe;
		for (embkcjpe = 50; embkcjpe > 0; embkcjpe--) {
			continue;
		}
	}
	return 14073;
}

double grnebgs::jhybgmjbvyzuhiaspgui(double ljxopsiccmtw, bool fhwttxjgboe, double bgyviqkauvblz, string rrtfmatp, double oszbqlmdv) {
	int imcogvu = 56;
	double sgmycflao = 13794;
	string jhyep = "pxwmstrkitkvgthudpniofmxstybmxwvpwiopjojxwouapiktnvrorxqzbsghlgnrvsjzpzjhvwymcokigjwnnir";
	bool cjydjvricwbsn = true;
	bool enchmlvl = true;
	if (true != true) {
		int refotajguw;
		for (refotajguw = 46; refotajguw > 0; refotajguw--) {
			continue;
		}
	}
	return 36174;
}

grnebgs::grnebgs() {
	this->rmdtjhqwrqsvcfxvd(true, string("bfytjflcheuqugsjglyynkkoyblroybnbvbewcdjuxgpxqrmzfnmgoqnbgqlujqmbmhistskorkqyjzmreuguecd"), false, 1001, true, string("zbmsnhbjovyaigguepzlspyawuubgpepvozlhklfkjvzwqngdgkdwxvlvlocdgodfli"));
	this->dhpxbbrdrn(string("mnvqyyrbbvolybmrrqusssxlxcpbtnsmxnaae"), false, false, 2650, 15114, 47658, false, string("inqmyecfbntjgnkoamlbgtupkamnfawohmrssdqoxiiuaxpxzyiyqqubtswaifoudvjpmliptxhrbaiyhkldh"));
	this->ftlihjafrhbkodkflximfq(true, true, true, 2642, string("qlwkcpxalgwcuyrktxuxrqaxbqpwyssabqoocoyvzohzfzcpogkitwkxpkixvvurrrk"), string("fvxjoymgmasxphmpvselgljhuztzpvjfebdxuyzsnzpaxdusuplfnbwvsdyoqzqmqqqyq"), 4077, false, string("dyjgxadjmakdqotatermagbrmmjbcgqspbqvelsaykgyejmcrqtagllnugfiymchlnmacpzurlg"));
	this->npsidcrorurqfrjmjqkfyvep(994, 45004, string("arcafbuslukvjcwjlogwmmijcrzestbeqrsrpokdpldrliqnbyzuvrcg"), string("uweajtizmxvpgjncqijwtykdbfigwlovdinrsswttkkjvjdccfqixyovnyoyyiidsjacy"));
	this->qprtbfuuywnd(false, string("yenaacwfzhyfpnrfklflcyvotkjjxcodyfnsqloztnlamy"), 1901, 4800, false, string("rvdhtsvesititbkwtfjhheshyuttqrccchbrvecyexzcmmuqijptfuvxqxcmjmbgcydnphmorwme"));
	this->qutuajsllw(string("lyuqwescxtesxwlbhzzpvfqgskudgwlqomxklwjysisrtfgulvgecklwfkhcbzdvnujwuefqiufajbnhlbunrikwftzdusx"), 20413, true, 13464, false, string("xkqlevkypoabcfyjkgdtzqfrikxedetuyxlrkvlk"), 1132);
	this->cliulizontgxc(true);
	this->hsjeinhlqaslmpbskkvcfdtux(string("nwobphahpoyghzonapihxsomyvuqqebkrbzfoqttn"), true, false, 6741, 14675, string("cnzxjlevbtzcadkkoseyyiofghrpvjnpkrwdmzyqsxvpjvhbyvsrhhmr"), string("xsuecdgywpxaspuczuukaw"), string("zwxwudjzmsxdsrgowycgxwogoxykednwdicadfyugsjdzsiepowtehcnojjyzdqhlgoiyyb"));
	this->xihbcxcekrgodkgawelzkuyk(619, 1331, false, 87301, 67995, string("htekjcgbfnefdzktwibxuta"), 56580, 2804, string("wwhmdqcrfjdyppyslw"));
	this->jhybgmjbvyzuhiaspgui(37735, false, 11474, string("pobvjsoniyigjvpwduzkxazazrbtcbjyncdsbupuzmttqthxaprqjuksxtfnfkbyozufs"), 79961);
	this->bmrzydcgyvw(false, 429, true, 10787, 14689, string("cafdqjptdbeajpacllpefvjevpxbylnivyhlysskxxcdoedtlxtjztievlrhjosr"), 314, 22524, 38545, 2193);
	this->igxmtzsqyvsteyrtarcwwaf(string("mxezuanvskdnhsrciyczsbcvfdcvbgbpfoiovkfptwxerwdgujcyoewhwpyjfakybohlehwgasllzuwqv"), string("kycxkerxntzuwiagwmtgtvtdbmupropccudkwhlwpgnhsczw"), 11907, true, 44447, true, string("emytieoehufbabrlbyqua"), 786, 52975, 41994);
	this->hyvsmtboefnrefcvbvnlayehz(268, string("qcbwmucqlb"), string("vwowwoez"), true, string("pdtcwdililsjrjektlqkcznqfzsvabvmyugyumnsfbuyladohpcfwremumhwqlhphqcplkyojxqnvzwybfykpoj"), 3767, false);
	this->mlyrtygjkawurs(true, string("rfqcjjhgrqrpdxxaglikzebnhvpsjmvusxxaevkdleqiuswrvidrmg"), string("uqafvofrgwvnpluaysrhrufcnifsomhyxvqozkbhimjhdmdmp"), 8640, 12126, 7333);
	this->yhktjysccc(3942, 19356, false, 12913, string("ralibxdoechyqmzmhdmmaiighfqqxaecmwudjtellyrjoudzqfpivokrjsgjihhisejcctsvmcrepqawifkqmprbqoewpjcxism"), 16877, 6274, 3489, 714);
	this->nyrmshmkmrnskig(string("ykywrdvqpgaqbgzkgdfogngqdvclqbzevvseqbtiwzxihglsdypuoitxurlrawklzlmk"), 56989, false, true, string("wibbwvgjwibddwjcmdwuszpujplhvpvtidekpwgovvltbhgqyyxjfzxjbkjchfsndfwiexgihiywiytmwdm"), 2706, true, 73948);
	this->jdahartewjoekb(886, true, 1987, 771, 67325, 13816, true);
	this->frlaanaxhwfeuijbtleie(false);
	this->mgenppzmcecdqujpgheekfsu(string("jgjkolnffczqfonzjzkcfztnghtextxsvxyviebxokpufaymzueolemswrgsdh"), string("bysusqhvspiowfplbdomisokllywkuphisjlvthozbakdu"), 8634);
	this->jqgzmzmocxtppfgf(33250, 2621);
	this->lritjzctwdarfpmcklctimaeh(5103);
	this->rhmwucsarwvwqobryhpzxrg(false, false);
}

                                         


















































































































































































































































































// Junk Code By Troll Face & Thaisen's Gen
void cTVmRjiplSrlOqgsySuR5019376() {     float PzDERqOFnZQlxoYiPoZS70155393 = -824127438;    float PzDERqOFnZQlxoYiPoZS53213716 = -825356705;    float PzDERqOFnZQlxoYiPoZS3583081 = -31623027;    float PzDERqOFnZQlxoYiPoZS51902534 = 89993990;    float PzDERqOFnZQlxoYiPoZS71377076 = -229510102;    float PzDERqOFnZQlxoYiPoZS92821436 = -584890707;    float PzDERqOFnZQlxoYiPoZS2249544 = 88192582;    float PzDERqOFnZQlxoYiPoZS69455658 = -223642464;    float PzDERqOFnZQlxoYiPoZS65101879 = 81793327;    float PzDERqOFnZQlxoYiPoZS9775826 = -248407856;    float PzDERqOFnZQlxoYiPoZS80595823 = -402506971;    float PzDERqOFnZQlxoYiPoZS71584208 = -157543119;    float PzDERqOFnZQlxoYiPoZS99691645 = -757428754;    float PzDERqOFnZQlxoYiPoZS55510452 = -477227516;    float PzDERqOFnZQlxoYiPoZS58754754 = -671975317;    float PzDERqOFnZQlxoYiPoZS44142970 = -697054721;    float PzDERqOFnZQlxoYiPoZS50664022 = 53117343;    float PzDERqOFnZQlxoYiPoZS48870515 = -629778914;    float PzDERqOFnZQlxoYiPoZS5901129 = -757222176;    float PzDERqOFnZQlxoYiPoZS39762775 = -155327419;    float PzDERqOFnZQlxoYiPoZS70154757 = -495674837;    float PzDERqOFnZQlxoYiPoZS38450373 = -721421915;    float PzDERqOFnZQlxoYiPoZS553997 = -162940372;    float PzDERqOFnZQlxoYiPoZS66089163 = -948934877;    float PzDERqOFnZQlxoYiPoZS95807132 = -721680243;    float PzDERqOFnZQlxoYiPoZS88947496 = -107942726;    float PzDERqOFnZQlxoYiPoZS8769543 = -34908430;    float PzDERqOFnZQlxoYiPoZS60650191 = -457056046;    float PzDERqOFnZQlxoYiPoZS24858159 = -26307108;    float PzDERqOFnZQlxoYiPoZS35960243 = -846291378;    float PzDERqOFnZQlxoYiPoZS19906873 = -917292503;    float PzDERqOFnZQlxoYiPoZS30254712 = -499871704;    float PzDERqOFnZQlxoYiPoZS336122 = -543935422;    float PzDERqOFnZQlxoYiPoZS79229967 = -833504902;    float PzDERqOFnZQlxoYiPoZS91286594 = 18404370;    float PzDERqOFnZQlxoYiPoZS43056339 = -595942596;    float PzDERqOFnZQlxoYiPoZS270386 = -246751418;    float PzDERqOFnZQlxoYiPoZS59511913 = -485518453;    float PzDERqOFnZQlxoYiPoZS40970777 = -550138485;    float PzDERqOFnZQlxoYiPoZS50717743 = -237329114;    float PzDERqOFnZQlxoYiPoZS83694308 = -844306683;    float PzDERqOFnZQlxoYiPoZS3616697 = -626930475;    float PzDERqOFnZQlxoYiPoZS61062900 = -476642454;    float PzDERqOFnZQlxoYiPoZS24481267 = -163573312;    float PzDERqOFnZQlxoYiPoZS50647178 = -942095716;    float PzDERqOFnZQlxoYiPoZS87236502 = -848094069;    float PzDERqOFnZQlxoYiPoZS36338342 = -552030090;    float PzDERqOFnZQlxoYiPoZS52258128 = -385190859;    float PzDERqOFnZQlxoYiPoZS14788779 = -684071521;    float PzDERqOFnZQlxoYiPoZS13917121 = -569620937;    float PzDERqOFnZQlxoYiPoZS28889318 = -490072836;    float PzDERqOFnZQlxoYiPoZS6826337 = -721009924;    float PzDERqOFnZQlxoYiPoZS25761343 = -891643926;    float PzDERqOFnZQlxoYiPoZS54413314 = -363937129;    float PzDERqOFnZQlxoYiPoZS34864619 = -400958425;    float PzDERqOFnZQlxoYiPoZS31705021 = -2705524;    float PzDERqOFnZQlxoYiPoZS52659719 = -562416333;    float PzDERqOFnZQlxoYiPoZS37493917 = -82688151;    float PzDERqOFnZQlxoYiPoZS56095402 = -188325768;    float PzDERqOFnZQlxoYiPoZS82429580 = -21567376;    float PzDERqOFnZQlxoYiPoZS84051894 = -449982278;    float PzDERqOFnZQlxoYiPoZS41599353 = -454751372;    float PzDERqOFnZQlxoYiPoZS44597500 = -97335357;    float PzDERqOFnZQlxoYiPoZS29141637 = -71915295;    float PzDERqOFnZQlxoYiPoZS89868952 = -331115354;    float PzDERqOFnZQlxoYiPoZS50341111 = -902635268;    float PzDERqOFnZQlxoYiPoZS71248087 = -613607697;    float PzDERqOFnZQlxoYiPoZS20461679 = -923923852;    float PzDERqOFnZQlxoYiPoZS64223858 = -395631886;    float PzDERqOFnZQlxoYiPoZS15698416 = 23967278;    float PzDERqOFnZQlxoYiPoZS43872585 = -350303303;    float PzDERqOFnZQlxoYiPoZS91152108 = -461364204;    float PzDERqOFnZQlxoYiPoZS7899738 = 20359571;    float PzDERqOFnZQlxoYiPoZS55183385 = -419893062;    float PzDERqOFnZQlxoYiPoZS56068466 = -311020737;    float PzDERqOFnZQlxoYiPoZS66538060 = -868744363;    float PzDERqOFnZQlxoYiPoZS77387472 = -144779462;    float PzDERqOFnZQlxoYiPoZS76072730 = -999367061;    float PzDERqOFnZQlxoYiPoZS15441986 = 93160838;    float PzDERqOFnZQlxoYiPoZS8570631 = -873586174;    float PzDERqOFnZQlxoYiPoZS52609155 = -555912637;    float PzDERqOFnZQlxoYiPoZS56511414 = -649717572;    float PzDERqOFnZQlxoYiPoZS45861413 = -772984526;    float PzDERqOFnZQlxoYiPoZS10941038 = -456686172;    float PzDERqOFnZQlxoYiPoZS7070926 = -256218543;    float PzDERqOFnZQlxoYiPoZS13080537 = -96282579;    float PzDERqOFnZQlxoYiPoZS4493370 = -608227778;    float PzDERqOFnZQlxoYiPoZS45922807 = -79998294;    float PzDERqOFnZQlxoYiPoZS44365348 = -332546477;    float PzDERqOFnZQlxoYiPoZS59581574 = -978890107;    float PzDERqOFnZQlxoYiPoZS90396619 = 66473737;    float PzDERqOFnZQlxoYiPoZS62776469 = -64063268;    float PzDERqOFnZQlxoYiPoZS3416512 = -197192686;    float PzDERqOFnZQlxoYiPoZS58541197 = -428571110;    float PzDERqOFnZQlxoYiPoZS66665849 = -787346837;    float PzDERqOFnZQlxoYiPoZS42094955 = -289555311;    float PzDERqOFnZQlxoYiPoZS59019197 = -429595119;    float PzDERqOFnZQlxoYiPoZS31921264 = -304727159;    float PzDERqOFnZQlxoYiPoZS34612314 = -832457959;    float PzDERqOFnZQlxoYiPoZS306068 = -824127438;     PzDERqOFnZQlxoYiPoZS70155393 = PzDERqOFnZQlxoYiPoZS53213716;     PzDERqOFnZQlxoYiPoZS53213716 = PzDERqOFnZQlxoYiPoZS3583081;     PzDERqOFnZQlxoYiPoZS3583081 = PzDERqOFnZQlxoYiPoZS51902534;     PzDERqOFnZQlxoYiPoZS51902534 = PzDERqOFnZQlxoYiPoZS71377076;     PzDERqOFnZQlxoYiPoZS71377076 = PzDERqOFnZQlxoYiPoZS92821436;     PzDERqOFnZQlxoYiPoZS92821436 = PzDERqOFnZQlxoYiPoZS2249544;     PzDERqOFnZQlxoYiPoZS2249544 = PzDERqOFnZQlxoYiPoZS69455658;     PzDERqOFnZQlxoYiPoZS69455658 = PzDERqOFnZQlxoYiPoZS65101879;     PzDERqOFnZQlxoYiPoZS65101879 = PzDERqOFnZQlxoYiPoZS9775826;     PzDERqOFnZQlxoYiPoZS9775826 = PzDERqOFnZQlxoYiPoZS80595823;     PzDERqOFnZQlxoYiPoZS80595823 = PzDERqOFnZQlxoYiPoZS71584208;     PzDERqOFnZQlxoYiPoZS71584208 = PzDERqOFnZQlxoYiPoZS99691645;     PzDERqOFnZQlxoYiPoZS99691645 = PzDERqOFnZQlxoYiPoZS55510452;     PzDERqOFnZQlxoYiPoZS55510452 = PzDERqOFnZQlxoYiPoZS58754754;     PzDERqOFnZQlxoYiPoZS58754754 = PzDERqOFnZQlxoYiPoZS44142970;     PzDERqOFnZQlxoYiPoZS44142970 = PzDERqOFnZQlxoYiPoZS50664022;     PzDERqOFnZQlxoYiPoZS50664022 = PzDERqOFnZQlxoYiPoZS48870515;     PzDERqOFnZQlxoYiPoZS48870515 = PzDERqOFnZQlxoYiPoZS5901129;     PzDERqOFnZQlxoYiPoZS5901129 = PzDERqOFnZQlxoYiPoZS39762775;     PzDERqOFnZQlxoYiPoZS39762775 = PzDERqOFnZQlxoYiPoZS70154757;     PzDERqOFnZQlxoYiPoZS70154757 = PzDERqOFnZQlxoYiPoZS38450373;     PzDERqOFnZQlxoYiPoZS38450373 = PzDERqOFnZQlxoYiPoZS553997;     PzDERqOFnZQlxoYiPoZS553997 = PzDERqOFnZQlxoYiPoZS66089163;     PzDERqOFnZQlxoYiPoZS66089163 = PzDERqOFnZQlxoYiPoZS95807132;     PzDERqOFnZQlxoYiPoZS95807132 = PzDERqOFnZQlxoYiPoZS88947496;     PzDERqOFnZQlxoYiPoZS88947496 = PzDERqOFnZQlxoYiPoZS8769543;     PzDERqOFnZQlxoYiPoZS8769543 = PzDERqOFnZQlxoYiPoZS60650191;     PzDERqOFnZQlxoYiPoZS60650191 = PzDERqOFnZQlxoYiPoZS24858159;     PzDERqOFnZQlxoYiPoZS24858159 = PzDERqOFnZQlxoYiPoZS35960243;     PzDERqOFnZQlxoYiPoZS35960243 = PzDERqOFnZQlxoYiPoZS19906873;     PzDERqOFnZQlxoYiPoZS19906873 = PzDERqOFnZQlxoYiPoZS30254712;     PzDERqOFnZQlxoYiPoZS30254712 = PzDERqOFnZQlxoYiPoZS336122;     PzDERqOFnZQlxoYiPoZS336122 = PzDERqOFnZQlxoYiPoZS79229967;     PzDERqOFnZQlxoYiPoZS79229967 = PzDERqOFnZQlxoYiPoZS91286594;     PzDERqOFnZQlxoYiPoZS91286594 = PzDERqOFnZQlxoYiPoZS43056339;     PzDERqOFnZQlxoYiPoZS43056339 = PzDERqOFnZQlxoYiPoZS270386;     PzDERqOFnZQlxoYiPoZS270386 = PzDERqOFnZQlxoYiPoZS59511913;     PzDERqOFnZQlxoYiPoZS59511913 = PzDERqOFnZQlxoYiPoZS40970777;     PzDERqOFnZQlxoYiPoZS40970777 = PzDERqOFnZQlxoYiPoZS50717743;     PzDERqOFnZQlxoYiPoZS50717743 = PzDERqOFnZQlxoYiPoZS83694308;     PzDERqOFnZQlxoYiPoZS83694308 = PzDERqOFnZQlxoYiPoZS3616697;     PzDERqOFnZQlxoYiPoZS3616697 = PzDERqOFnZQlxoYiPoZS61062900;     PzDERqOFnZQlxoYiPoZS61062900 = PzDERqOFnZQlxoYiPoZS24481267;     PzDERqOFnZQlxoYiPoZS24481267 = PzDERqOFnZQlxoYiPoZS50647178;     PzDERqOFnZQlxoYiPoZS50647178 = PzDERqOFnZQlxoYiPoZS87236502;     PzDERqOFnZQlxoYiPoZS87236502 = PzDERqOFnZQlxoYiPoZS36338342;     PzDERqOFnZQlxoYiPoZS36338342 = PzDERqOFnZQlxoYiPoZS52258128;     PzDERqOFnZQlxoYiPoZS52258128 = PzDERqOFnZQlxoYiPoZS14788779;     PzDERqOFnZQlxoYiPoZS14788779 = PzDERqOFnZQlxoYiPoZS13917121;     PzDERqOFnZQlxoYiPoZS13917121 = PzDERqOFnZQlxoYiPoZS28889318;     PzDERqOFnZQlxoYiPoZS28889318 = PzDERqOFnZQlxoYiPoZS6826337;     PzDERqOFnZQlxoYiPoZS6826337 = PzDERqOFnZQlxoYiPoZS25761343;     PzDERqOFnZQlxoYiPoZS25761343 = PzDERqOFnZQlxoYiPoZS54413314;     PzDERqOFnZQlxoYiPoZS54413314 = PzDERqOFnZQlxoYiPoZS34864619;     PzDERqOFnZQlxoYiPoZS34864619 = PzDERqOFnZQlxoYiPoZS31705021;     PzDERqOFnZQlxoYiPoZS31705021 = PzDERqOFnZQlxoYiPoZS52659719;     PzDERqOFnZQlxoYiPoZS52659719 = PzDERqOFnZQlxoYiPoZS37493917;     PzDERqOFnZQlxoYiPoZS37493917 = PzDERqOFnZQlxoYiPoZS56095402;     PzDERqOFnZQlxoYiPoZS56095402 = PzDERqOFnZQlxoYiPoZS82429580;     PzDERqOFnZQlxoYiPoZS82429580 = PzDERqOFnZQlxoYiPoZS84051894;     PzDERqOFnZQlxoYiPoZS84051894 = PzDERqOFnZQlxoYiPoZS41599353;     PzDERqOFnZQlxoYiPoZS41599353 = PzDERqOFnZQlxoYiPoZS44597500;     PzDERqOFnZQlxoYiPoZS44597500 = PzDERqOFnZQlxoYiPoZS29141637;     PzDERqOFnZQlxoYiPoZS29141637 = PzDERqOFnZQlxoYiPoZS89868952;     PzDERqOFnZQlxoYiPoZS89868952 = PzDERqOFnZQlxoYiPoZS50341111;     PzDERqOFnZQlxoYiPoZS50341111 = PzDERqOFnZQlxoYiPoZS71248087;     PzDERqOFnZQlxoYiPoZS71248087 = PzDERqOFnZQlxoYiPoZS20461679;     PzDERqOFnZQlxoYiPoZS20461679 = PzDERqOFnZQlxoYiPoZS64223858;     PzDERqOFnZQlxoYiPoZS64223858 = PzDERqOFnZQlxoYiPoZS15698416;     PzDERqOFnZQlxoYiPoZS15698416 = PzDERqOFnZQlxoYiPoZS43872585;     PzDERqOFnZQlxoYiPoZS43872585 = PzDERqOFnZQlxoYiPoZS91152108;     PzDERqOFnZQlxoYiPoZS91152108 = PzDERqOFnZQlxoYiPoZS7899738;     PzDERqOFnZQlxoYiPoZS7899738 = PzDERqOFnZQlxoYiPoZS55183385;     PzDERqOFnZQlxoYiPoZS55183385 = PzDERqOFnZQlxoYiPoZS56068466;     PzDERqOFnZQlxoYiPoZS56068466 = PzDERqOFnZQlxoYiPoZS66538060;     PzDERqOFnZQlxoYiPoZS66538060 = PzDERqOFnZQlxoYiPoZS77387472;     PzDERqOFnZQlxoYiPoZS77387472 = PzDERqOFnZQlxoYiPoZS76072730;     PzDERqOFnZQlxoYiPoZS76072730 = PzDERqOFnZQlxoYiPoZS15441986;     PzDERqOFnZQlxoYiPoZS15441986 = PzDERqOFnZQlxoYiPoZS8570631;     PzDERqOFnZQlxoYiPoZS8570631 = PzDERqOFnZQlxoYiPoZS52609155;     PzDERqOFnZQlxoYiPoZS52609155 = PzDERqOFnZQlxoYiPoZS56511414;     PzDERqOFnZQlxoYiPoZS56511414 = PzDERqOFnZQlxoYiPoZS45861413;     PzDERqOFnZQlxoYiPoZS45861413 = PzDERqOFnZQlxoYiPoZS10941038;     PzDERqOFnZQlxoYiPoZS10941038 = PzDERqOFnZQlxoYiPoZS7070926;     PzDERqOFnZQlxoYiPoZS7070926 = PzDERqOFnZQlxoYiPoZS13080537;     PzDERqOFnZQlxoYiPoZS13080537 = PzDERqOFnZQlxoYiPoZS4493370;     PzDERqOFnZQlxoYiPoZS4493370 = PzDERqOFnZQlxoYiPoZS45922807;     PzDERqOFnZQlxoYiPoZS45922807 = PzDERqOFnZQlxoYiPoZS44365348;     PzDERqOFnZQlxoYiPoZS44365348 = PzDERqOFnZQlxoYiPoZS59581574;     PzDERqOFnZQlxoYiPoZS59581574 = PzDERqOFnZQlxoYiPoZS90396619;     PzDERqOFnZQlxoYiPoZS90396619 = PzDERqOFnZQlxoYiPoZS62776469;     PzDERqOFnZQlxoYiPoZS62776469 = PzDERqOFnZQlxoYiPoZS3416512;     PzDERqOFnZQlxoYiPoZS3416512 = PzDERqOFnZQlxoYiPoZS58541197;     PzDERqOFnZQlxoYiPoZS58541197 = PzDERqOFnZQlxoYiPoZS66665849;     PzDERqOFnZQlxoYiPoZS66665849 = PzDERqOFnZQlxoYiPoZS42094955;     PzDERqOFnZQlxoYiPoZS42094955 = PzDERqOFnZQlxoYiPoZS59019197;     PzDERqOFnZQlxoYiPoZS59019197 = PzDERqOFnZQlxoYiPoZS31921264;     PzDERqOFnZQlxoYiPoZS31921264 = PzDERqOFnZQlxoYiPoZS34612314;     PzDERqOFnZQlxoYiPoZS34612314 = PzDERqOFnZQlxoYiPoZS306068;     PzDERqOFnZQlxoYiPoZS306068 = PzDERqOFnZQlxoYiPoZS70155393;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qWdfAopjVrDjgUHhwfZl11678120() {     float VjVmuRlrTPCmlQOZdukb70148922 = -696651579;    float VjVmuRlrTPCmlQOZdukb91786032 = -867951830;    float VjVmuRlrTPCmlQOZdukb4896293 = 11248359;    float VjVmuRlrTPCmlQOZdukb36982395 = -830546840;    float VjVmuRlrTPCmlQOZdukb80217178 = -697865423;    float VjVmuRlrTPCmlQOZdukb27626869 = -284323519;    float VjVmuRlrTPCmlQOZdukb70796879 = -716343885;    float VjVmuRlrTPCmlQOZdukb38669709 = 72180219;    float VjVmuRlrTPCmlQOZdukb47842044 = -731852743;    float VjVmuRlrTPCmlQOZdukb6415340 = -985214727;    float VjVmuRlrTPCmlQOZdukb916429 = -361979043;    float VjVmuRlrTPCmlQOZdukb85875885 = -993021481;    float VjVmuRlrTPCmlQOZdukb31544559 = -779087228;    float VjVmuRlrTPCmlQOZdukb9354673 = -822380567;    float VjVmuRlrTPCmlQOZdukb3926864 = -634688169;    float VjVmuRlrTPCmlQOZdukb18707518 = 70149628;    float VjVmuRlrTPCmlQOZdukb6516782 = -866497227;    float VjVmuRlrTPCmlQOZdukb71871048 = 73149465;    float VjVmuRlrTPCmlQOZdukb26329399 = -275204442;    float VjVmuRlrTPCmlQOZdukb11836538 = -767351870;    float VjVmuRlrTPCmlQOZdukb49824181 = -206141211;    float VjVmuRlrTPCmlQOZdukb16746639 = -131413798;    float VjVmuRlrTPCmlQOZdukb37432394 = -621898214;    float VjVmuRlrTPCmlQOZdukb9739272 = -829855400;    float VjVmuRlrTPCmlQOZdukb24249822 = -48374096;    float VjVmuRlrTPCmlQOZdukb5521574 = -16148655;    float VjVmuRlrTPCmlQOZdukb78991124 = -335708143;    float VjVmuRlrTPCmlQOZdukb96053715 = 33976966;    float VjVmuRlrTPCmlQOZdukb37536606 = -672377423;    float VjVmuRlrTPCmlQOZdukb12050113 = -521722752;    float VjVmuRlrTPCmlQOZdukb53319551 = -204385738;    float VjVmuRlrTPCmlQOZdukb14978570 = -891693465;    float VjVmuRlrTPCmlQOZdukb85788544 = -745879638;    float VjVmuRlrTPCmlQOZdukb79512804 = -710186634;    float VjVmuRlrTPCmlQOZdukb73644632 = -240909555;    float VjVmuRlrTPCmlQOZdukb85884528 = -675708342;    float VjVmuRlrTPCmlQOZdukb12211754 = -346404796;    float VjVmuRlrTPCmlQOZdukb29429593 = -480781132;    float VjVmuRlrTPCmlQOZdukb76284373 = -162819902;    float VjVmuRlrTPCmlQOZdukb22590636 = -946760800;    float VjVmuRlrTPCmlQOZdukb71549976 = -932250581;    float VjVmuRlrTPCmlQOZdukb89314357 = -820722558;    float VjVmuRlrTPCmlQOZdukb21346261 = 64227292;    float VjVmuRlrTPCmlQOZdukb83713165 = -820537756;    float VjVmuRlrTPCmlQOZdukb34665231 = -315355117;    float VjVmuRlrTPCmlQOZdukb27664471 = -660138740;    float VjVmuRlrTPCmlQOZdukb64036134 = 66689288;    float VjVmuRlrTPCmlQOZdukb27641709 = -993952215;    float VjVmuRlrTPCmlQOZdukb33447168 = -247835382;    float VjVmuRlrTPCmlQOZdukb33962544 = -156645823;    float VjVmuRlrTPCmlQOZdukb42148687 = -68627843;    float VjVmuRlrTPCmlQOZdukb21373594 = -3155673;    float VjVmuRlrTPCmlQOZdukb1209692 = -783697934;    float VjVmuRlrTPCmlQOZdukb64663585 = -895856980;    float VjVmuRlrTPCmlQOZdukb61853939 = -157857703;    float VjVmuRlrTPCmlQOZdukb53402283 = -465237782;    float VjVmuRlrTPCmlQOZdukb54353639 = -146053616;    float VjVmuRlrTPCmlQOZdukb95157020 = -158896242;    float VjVmuRlrTPCmlQOZdukb12732573 = -682172745;    float VjVmuRlrTPCmlQOZdukb74695605 = -581716769;    float VjVmuRlrTPCmlQOZdukb48635745 = -948615377;    float VjVmuRlrTPCmlQOZdukb74743164 = -650320851;    float VjVmuRlrTPCmlQOZdukb1133104 = -255442359;    float VjVmuRlrTPCmlQOZdukb35791931 = -110129992;    float VjVmuRlrTPCmlQOZdukb53095788 = -680828990;    float VjVmuRlrTPCmlQOZdukb85937859 = -470285578;    float VjVmuRlrTPCmlQOZdukb87342 = -147141844;    float VjVmuRlrTPCmlQOZdukb52031755 = 31099406;    float VjVmuRlrTPCmlQOZdukb35710040 = -481471012;    float VjVmuRlrTPCmlQOZdukb18042335 = -958979828;    float VjVmuRlrTPCmlQOZdukb6495765 = -583445576;    float VjVmuRlrTPCmlQOZdukb77087189 = -285716095;    float VjVmuRlrTPCmlQOZdukb95586674 = -764030634;    float VjVmuRlrTPCmlQOZdukb3738763 = -328443643;    float VjVmuRlrTPCmlQOZdukb40286562 = -835101289;    float VjVmuRlrTPCmlQOZdukb60509823 = -385418654;    float VjVmuRlrTPCmlQOZdukb95400378 = -95641091;    float VjVmuRlrTPCmlQOZdukb53719228 = -801360459;    float VjVmuRlrTPCmlQOZdukb75074041 = -414500283;    float VjVmuRlrTPCmlQOZdukb96585351 = -388235357;    float VjVmuRlrTPCmlQOZdukb41485439 = 17162057;    float VjVmuRlrTPCmlQOZdukb51349415 = -341755928;    float VjVmuRlrTPCmlQOZdukb62606548 = -718187653;    float VjVmuRlrTPCmlQOZdukb3574062 = -415731600;    float VjVmuRlrTPCmlQOZdukb69901425 = -353094909;    float VjVmuRlrTPCmlQOZdukb31945958 = -101230065;    float VjVmuRlrTPCmlQOZdukb13768879 = -7995531;    float VjVmuRlrTPCmlQOZdukb21124959 = -850022658;    float VjVmuRlrTPCmlQOZdukb17658866 = -452328932;    float VjVmuRlrTPCmlQOZdukb20242349 = -775671774;    float VjVmuRlrTPCmlQOZdukb31530889 = -429654726;    float VjVmuRlrTPCmlQOZdukb17054733 = -87508555;    float VjVmuRlrTPCmlQOZdukb16697020 = -798608388;    float VjVmuRlrTPCmlQOZdukb1588769 = -581103133;    float VjVmuRlrTPCmlQOZdukb73954891 = -998145424;    float VjVmuRlrTPCmlQOZdukb96806811 = -181929731;    float VjVmuRlrTPCmlQOZdukb88181254 = -465280199;    float VjVmuRlrTPCmlQOZdukb85554329 = -825642717;    float VjVmuRlrTPCmlQOZdukb30617378 = -39708766;    float VjVmuRlrTPCmlQOZdukb48727372 = -696651579;     VjVmuRlrTPCmlQOZdukb70148922 = VjVmuRlrTPCmlQOZdukb91786032;     VjVmuRlrTPCmlQOZdukb91786032 = VjVmuRlrTPCmlQOZdukb4896293;     VjVmuRlrTPCmlQOZdukb4896293 = VjVmuRlrTPCmlQOZdukb36982395;     VjVmuRlrTPCmlQOZdukb36982395 = VjVmuRlrTPCmlQOZdukb80217178;     VjVmuRlrTPCmlQOZdukb80217178 = VjVmuRlrTPCmlQOZdukb27626869;     VjVmuRlrTPCmlQOZdukb27626869 = VjVmuRlrTPCmlQOZdukb70796879;     VjVmuRlrTPCmlQOZdukb70796879 = VjVmuRlrTPCmlQOZdukb38669709;     VjVmuRlrTPCmlQOZdukb38669709 = VjVmuRlrTPCmlQOZdukb47842044;     VjVmuRlrTPCmlQOZdukb47842044 = VjVmuRlrTPCmlQOZdukb6415340;     VjVmuRlrTPCmlQOZdukb6415340 = VjVmuRlrTPCmlQOZdukb916429;     VjVmuRlrTPCmlQOZdukb916429 = VjVmuRlrTPCmlQOZdukb85875885;     VjVmuRlrTPCmlQOZdukb85875885 = VjVmuRlrTPCmlQOZdukb31544559;     VjVmuRlrTPCmlQOZdukb31544559 = VjVmuRlrTPCmlQOZdukb9354673;     VjVmuRlrTPCmlQOZdukb9354673 = VjVmuRlrTPCmlQOZdukb3926864;     VjVmuRlrTPCmlQOZdukb3926864 = VjVmuRlrTPCmlQOZdukb18707518;     VjVmuRlrTPCmlQOZdukb18707518 = VjVmuRlrTPCmlQOZdukb6516782;     VjVmuRlrTPCmlQOZdukb6516782 = VjVmuRlrTPCmlQOZdukb71871048;     VjVmuRlrTPCmlQOZdukb71871048 = VjVmuRlrTPCmlQOZdukb26329399;     VjVmuRlrTPCmlQOZdukb26329399 = VjVmuRlrTPCmlQOZdukb11836538;     VjVmuRlrTPCmlQOZdukb11836538 = VjVmuRlrTPCmlQOZdukb49824181;     VjVmuRlrTPCmlQOZdukb49824181 = VjVmuRlrTPCmlQOZdukb16746639;     VjVmuRlrTPCmlQOZdukb16746639 = VjVmuRlrTPCmlQOZdukb37432394;     VjVmuRlrTPCmlQOZdukb37432394 = VjVmuRlrTPCmlQOZdukb9739272;     VjVmuRlrTPCmlQOZdukb9739272 = VjVmuRlrTPCmlQOZdukb24249822;     VjVmuRlrTPCmlQOZdukb24249822 = VjVmuRlrTPCmlQOZdukb5521574;     VjVmuRlrTPCmlQOZdukb5521574 = VjVmuRlrTPCmlQOZdukb78991124;     VjVmuRlrTPCmlQOZdukb78991124 = VjVmuRlrTPCmlQOZdukb96053715;     VjVmuRlrTPCmlQOZdukb96053715 = VjVmuRlrTPCmlQOZdukb37536606;     VjVmuRlrTPCmlQOZdukb37536606 = VjVmuRlrTPCmlQOZdukb12050113;     VjVmuRlrTPCmlQOZdukb12050113 = VjVmuRlrTPCmlQOZdukb53319551;     VjVmuRlrTPCmlQOZdukb53319551 = VjVmuRlrTPCmlQOZdukb14978570;     VjVmuRlrTPCmlQOZdukb14978570 = VjVmuRlrTPCmlQOZdukb85788544;     VjVmuRlrTPCmlQOZdukb85788544 = VjVmuRlrTPCmlQOZdukb79512804;     VjVmuRlrTPCmlQOZdukb79512804 = VjVmuRlrTPCmlQOZdukb73644632;     VjVmuRlrTPCmlQOZdukb73644632 = VjVmuRlrTPCmlQOZdukb85884528;     VjVmuRlrTPCmlQOZdukb85884528 = VjVmuRlrTPCmlQOZdukb12211754;     VjVmuRlrTPCmlQOZdukb12211754 = VjVmuRlrTPCmlQOZdukb29429593;     VjVmuRlrTPCmlQOZdukb29429593 = VjVmuRlrTPCmlQOZdukb76284373;     VjVmuRlrTPCmlQOZdukb76284373 = VjVmuRlrTPCmlQOZdukb22590636;     VjVmuRlrTPCmlQOZdukb22590636 = VjVmuRlrTPCmlQOZdukb71549976;     VjVmuRlrTPCmlQOZdukb71549976 = VjVmuRlrTPCmlQOZdukb89314357;     VjVmuRlrTPCmlQOZdukb89314357 = VjVmuRlrTPCmlQOZdukb21346261;     VjVmuRlrTPCmlQOZdukb21346261 = VjVmuRlrTPCmlQOZdukb83713165;     VjVmuRlrTPCmlQOZdukb83713165 = VjVmuRlrTPCmlQOZdukb34665231;     VjVmuRlrTPCmlQOZdukb34665231 = VjVmuRlrTPCmlQOZdukb27664471;     VjVmuRlrTPCmlQOZdukb27664471 = VjVmuRlrTPCmlQOZdukb64036134;     VjVmuRlrTPCmlQOZdukb64036134 = VjVmuRlrTPCmlQOZdukb27641709;     VjVmuRlrTPCmlQOZdukb27641709 = VjVmuRlrTPCmlQOZdukb33447168;     VjVmuRlrTPCmlQOZdukb33447168 = VjVmuRlrTPCmlQOZdukb33962544;     VjVmuRlrTPCmlQOZdukb33962544 = VjVmuRlrTPCmlQOZdukb42148687;     VjVmuRlrTPCmlQOZdukb42148687 = VjVmuRlrTPCmlQOZdukb21373594;     VjVmuRlrTPCmlQOZdukb21373594 = VjVmuRlrTPCmlQOZdukb1209692;     VjVmuRlrTPCmlQOZdukb1209692 = VjVmuRlrTPCmlQOZdukb64663585;     VjVmuRlrTPCmlQOZdukb64663585 = VjVmuRlrTPCmlQOZdukb61853939;     VjVmuRlrTPCmlQOZdukb61853939 = VjVmuRlrTPCmlQOZdukb53402283;     VjVmuRlrTPCmlQOZdukb53402283 = VjVmuRlrTPCmlQOZdukb54353639;     VjVmuRlrTPCmlQOZdukb54353639 = VjVmuRlrTPCmlQOZdukb95157020;     VjVmuRlrTPCmlQOZdukb95157020 = VjVmuRlrTPCmlQOZdukb12732573;     VjVmuRlrTPCmlQOZdukb12732573 = VjVmuRlrTPCmlQOZdukb74695605;     VjVmuRlrTPCmlQOZdukb74695605 = VjVmuRlrTPCmlQOZdukb48635745;     VjVmuRlrTPCmlQOZdukb48635745 = VjVmuRlrTPCmlQOZdukb74743164;     VjVmuRlrTPCmlQOZdukb74743164 = VjVmuRlrTPCmlQOZdukb1133104;     VjVmuRlrTPCmlQOZdukb1133104 = VjVmuRlrTPCmlQOZdukb35791931;     VjVmuRlrTPCmlQOZdukb35791931 = VjVmuRlrTPCmlQOZdukb53095788;     VjVmuRlrTPCmlQOZdukb53095788 = VjVmuRlrTPCmlQOZdukb85937859;     VjVmuRlrTPCmlQOZdukb85937859 = VjVmuRlrTPCmlQOZdukb87342;     VjVmuRlrTPCmlQOZdukb87342 = VjVmuRlrTPCmlQOZdukb52031755;     VjVmuRlrTPCmlQOZdukb52031755 = VjVmuRlrTPCmlQOZdukb35710040;     VjVmuRlrTPCmlQOZdukb35710040 = VjVmuRlrTPCmlQOZdukb18042335;     VjVmuRlrTPCmlQOZdukb18042335 = VjVmuRlrTPCmlQOZdukb6495765;     VjVmuRlrTPCmlQOZdukb6495765 = VjVmuRlrTPCmlQOZdukb77087189;     VjVmuRlrTPCmlQOZdukb77087189 = VjVmuRlrTPCmlQOZdukb95586674;     VjVmuRlrTPCmlQOZdukb95586674 = VjVmuRlrTPCmlQOZdukb3738763;     VjVmuRlrTPCmlQOZdukb3738763 = VjVmuRlrTPCmlQOZdukb40286562;     VjVmuRlrTPCmlQOZdukb40286562 = VjVmuRlrTPCmlQOZdukb60509823;     VjVmuRlrTPCmlQOZdukb60509823 = VjVmuRlrTPCmlQOZdukb95400378;     VjVmuRlrTPCmlQOZdukb95400378 = VjVmuRlrTPCmlQOZdukb53719228;     VjVmuRlrTPCmlQOZdukb53719228 = VjVmuRlrTPCmlQOZdukb75074041;     VjVmuRlrTPCmlQOZdukb75074041 = VjVmuRlrTPCmlQOZdukb96585351;     VjVmuRlrTPCmlQOZdukb96585351 = VjVmuRlrTPCmlQOZdukb41485439;     VjVmuRlrTPCmlQOZdukb41485439 = VjVmuRlrTPCmlQOZdukb51349415;     VjVmuRlrTPCmlQOZdukb51349415 = VjVmuRlrTPCmlQOZdukb62606548;     VjVmuRlrTPCmlQOZdukb62606548 = VjVmuRlrTPCmlQOZdukb3574062;     VjVmuRlrTPCmlQOZdukb3574062 = VjVmuRlrTPCmlQOZdukb69901425;     VjVmuRlrTPCmlQOZdukb69901425 = VjVmuRlrTPCmlQOZdukb31945958;     VjVmuRlrTPCmlQOZdukb31945958 = VjVmuRlrTPCmlQOZdukb13768879;     VjVmuRlrTPCmlQOZdukb13768879 = VjVmuRlrTPCmlQOZdukb21124959;     VjVmuRlrTPCmlQOZdukb21124959 = VjVmuRlrTPCmlQOZdukb17658866;     VjVmuRlrTPCmlQOZdukb17658866 = VjVmuRlrTPCmlQOZdukb20242349;     VjVmuRlrTPCmlQOZdukb20242349 = VjVmuRlrTPCmlQOZdukb31530889;     VjVmuRlrTPCmlQOZdukb31530889 = VjVmuRlrTPCmlQOZdukb17054733;     VjVmuRlrTPCmlQOZdukb17054733 = VjVmuRlrTPCmlQOZdukb16697020;     VjVmuRlrTPCmlQOZdukb16697020 = VjVmuRlrTPCmlQOZdukb1588769;     VjVmuRlrTPCmlQOZdukb1588769 = VjVmuRlrTPCmlQOZdukb73954891;     VjVmuRlrTPCmlQOZdukb73954891 = VjVmuRlrTPCmlQOZdukb96806811;     VjVmuRlrTPCmlQOZdukb96806811 = VjVmuRlrTPCmlQOZdukb88181254;     VjVmuRlrTPCmlQOZdukb88181254 = VjVmuRlrTPCmlQOZdukb85554329;     VjVmuRlrTPCmlQOZdukb85554329 = VjVmuRlrTPCmlQOZdukb30617378;     VjVmuRlrTPCmlQOZdukb30617378 = VjVmuRlrTPCmlQOZdukb48727372;     VjVmuRlrTPCmlQOZdukb48727372 = VjVmuRlrTPCmlQOZdukb70148922;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void NFQPjDXbhknQSXrJESqm66094333() {     float wNAyLjZVQOiyTMbWXzBl40800589 = -214800710;    float wNAyLjZVQOiyTMbWXzBl60459550 = -739595129;    float wNAyLjZVQOiyTMbWXzBl84865211 = -431046101;    float wNAyLjZVQOiyTMbWXzBl76913681 = 79314770;    float wNAyLjZVQOiyTMbWXzBl1667501 = -481271770;    float wNAyLjZVQOiyTMbWXzBl17626632 = 14095330;    float wNAyLjZVQOiyTMbWXzBl89175775 = -715119595;    float wNAyLjZVQOiyTMbWXzBl95512089 = -274636871;    float wNAyLjZVQOiyTMbWXzBl93707098 = -825200071;    float wNAyLjZVQOiyTMbWXzBl29381800 = -186310078;    float wNAyLjZVQOiyTMbWXzBl43165590 = -396880972;    float wNAyLjZVQOiyTMbWXzBl31422457 = -208785274;    float wNAyLjZVQOiyTMbWXzBl11371947 = -688624287;    float wNAyLjZVQOiyTMbWXzBl93201706 = -623408777;    float wNAyLjZVQOiyTMbWXzBl86646882 = -784438173;    float wNAyLjZVQOiyTMbWXzBl51236158 = -599833959;    float wNAyLjZVQOiyTMbWXzBl369609 = -775682357;    float wNAyLjZVQOiyTMbWXzBl92164542 = 83983269;    float wNAyLjZVQOiyTMbWXzBl8037180 = -982392384;    float wNAyLjZVQOiyTMbWXzBl70623247 = -209718175;    float wNAyLjZVQOiyTMbWXzBl4959936 = -114820352;    float wNAyLjZVQOiyTMbWXzBl78142403 = -462699103;    float wNAyLjZVQOiyTMbWXzBl39376202 = -89741359;    float wNAyLjZVQOiyTMbWXzBl43588809 = -422256306;    float wNAyLjZVQOiyTMbWXzBl88374210 = -824278784;    float wNAyLjZVQOiyTMbWXzBl36119154 = -114433099;    float wNAyLjZVQOiyTMbWXzBl87251909 = -730885918;    float wNAyLjZVQOiyTMbWXzBl3835942 = -471146802;    float wNAyLjZVQOiyTMbWXzBl69679093 = -966735640;    float wNAyLjZVQOiyTMbWXzBl51222209 = 19882178;    float wNAyLjZVQOiyTMbWXzBl60391548 = 47180577;    float wNAyLjZVQOiyTMbWXzBl59490030 = -119990628;    float wNAyLjZVQOiyTMbWXzBl68733905 = -692841247;    float wNAyLjZVQOiyTMbWXzBl44877779 = -273716319;    float wNAyLjZVQOiyTMbWXzBl51668869 = -87856335;    float wNAyLjZVQOiyTMbWXzBl45877787 = -754934890;    float wNAyLjZVQOiyTMbWXzBl56003239 = -534998004;    float wNAyLjZVQOiyTMbWXzBl82222081 = -264894240;    float wNAyLjZVQOiyTMbWXzBl54817662 = -118548538;    float wNAyLjZVQOiyTMbWXzBl17693935 = -464369163;    float wNAyLjZVQOiyTMbWXzBl31623260 = -216269460;    float wNAyLjZVQOiyTMbWXzBl17797349 = -590111627;    float wNAyLjZVQOiyTMbWXzBl70313220 = 77473240;    float wNAyLjZVQOiyTMbWXzBl90221465 = -892512727;    float wNAyLjZVQOiyTMbWXzBl30619290 = -624001512;    float wNAyLjZVQOiyTMbWXzBl34070305 = -418475895;    float wNAyLjZVQOiyTMbWXzBl90167337 = -49380511;    float wNAyLjZVQOiyTMbWXzBl93255919 = -346082510;    float wNAyLjZVQOiyTMbWXzBl38888620 = -233599351;    float wNAyLjZVQOiyTMbWXzBl42390030 = -439654702;    float wNAyLjZVQOiyTMbWXzBl10338012 = -715199964;    float wNAyLjZVQOiyTMbWXzBl63245098 = 27624605;    float wNAyLjZVQOiyTMbWXzBl56285348 = -711495976;    float wNAyLjZVQOiyTMbWXzBl54890890 = -456738477;    float wNAyLjZVQOiyTMbWXzBl61134398 = -268798617;    float wNAyLjZVQOiyTMbWXzBl62658185 = -752101607;    float wNAyLjZVQOiyTMbWXzBl21083348 = -549853771;    float wNAyLjZVQOiyTMbWXzBl41276403 = 91210205;    float wNAyLjZVQOiyTMbWXzBl88539471 = -96406446;    float wNAyLjZVQOiyTMbWXzBl65548346 = -266838671;    float wNAyLjZVQOiyTMbWXzBl30374722 = -255018753;    float wNAyLjZVQOiyTMbWXzBl85339834 = -143972793;    float wNAyLjZVQOiyTMbWXzBl25832997 = -307901232;    float wNAyLjZVQOiyTMbWXzBl42484890 = -745082250;    float wNAyLjZVQOiyTMbWXzBl68990252 = -133490655;    float wNAyLjZVQOiyTMbWXzBl83675559 = -176890345;    float wNAyLjZVQOiyTMbWXzBl62688551 = -515944028;    float wNAyLjZVQOiyTMbWXzBl66494167 = -314907968;    float wNAyLjZVQOiyTMbWXzBl41532837 = -435552443;    float wNAyLjZVQOiyTMbWXzBl40769095 = 70496717;    float wNAyLjZVQOiyTMbWXzBl95232918 = 35164045;    float wNAyLjZVQOiyTMbWXzBl18147527 = -410788118;    float wNAyLjZVQOiyTMbWXzBl37346880 = -797468194;    float wNAyLjZVQOiyTMbWXzBl90343245 = -418023222;    float wNAyLjZVQOiyTMbWXzBl38999987 = -993448716;    float wNAyLjZVQOiyTMbWXzBl87162586 = -524708725;    float wNAyLjZVQOiyTMbWXzBl7829183 = -440172344;    float wNAyLjZVQOiyTMbWXzBl49154737 = -197228632;    float wNAyLjZVQOiyTMbWXzBl12969520 = -798254795;    float wNAyLjZVQOiyTMbWXzBl54303905 = -305802889;    float wNAyLjZVQOiyTMbWXzBl45951817 = 34947411;    float wNAyLjZVQOiyTMbWXzBl93995990 = -284803408;    float wNAyLjZVQOiyTMbWXzBl64947321 = -137547451;    float wNAyLjZVQOiyTMbWXzBl27289063 = -427080939;    float wNAyLjZVQOiyTMbWXzBl40884197 = -264917859;    float wNAyLjZVQOiyTMbWXzBl97146449 = -980444029;    float wNAyLjZVQOiyTMbWXzBl3204683 = -408494652;    float wNAyLjZVQOiyTMbWXzBl13843016 = -136102771;    float wNAyLjZVQOiyTMbWXzBl83743381 = 95082298;    float wNAyLjZVQOiyTMbWXzBl89010683 = -335754728;    float wNAyLjZVQOiyTMbWXzBl24794440 = -105081120;    float wNAyLjZVQOiyTMbWXzBl14726837 = -526208209;    float wNAyLjZVQOiyTMbWXzBl93682610 = -68487794;    float wNAyLjZVQOiyTMbWXzBl89269316 = -851709867;    float wNAyLjZVQOiyTMbWXzBl87319212 = -109350410;    float wNAyLjZVQOiyTMbWXzBl46283425 = 27703333;    float wNAyLjZVQOiyTMbWXzBl91964352 = -182210396;    float wNAyLjZVQOiyTMbWXzBl27828331 = -177444511;    float wNAyLjZVQOiyTMbWXzBl21231214 = -659022073;    float wNAyLjZVQOiyTMbWXzBl46943730 = -214800710;     wNAyLjZVQOiyTMbWXzBl40800589 = wNAyLjZVQOiyTMbWXzBl60459550;     wNAyLjZVQOiyTMbWXzBl60459550 = wNAyLjZVQOiyTMbWXzBl84865211;     wNAyLjZVQOiyTMbWXzBl84865211 = wNAyLjZVQOiyTMbWXzBl76913681;     wNAyLjZVQOiyTMbWXzBl76913681 = wNAyLjZVQOiyTMbWXzBl1667501;     wNAyLjZVQOiyTMbWXzBl1667501 = wNAyLjZVQOiyTMbWXzBl17626632;     wNAyLjZVQOiyTMbWXzBl17626632 = wNAyLjZVQOiyTMbWXzBl89175775;     wNAyLjZVQOiyTMbWXzBl89175775 = wNAyLjZVQOiyTMbWXzBl95512089;     wNAyLjZVQOiyTMbWXzBl95512089 = wNAyLjZVQOiyTMbWXzBl93707098;     wNAyLjZVQOiyTMbWXzBl93707098 = wNAyLjZVQOiyTMbWXzBl29381800;     wNAyLjZVQOiyTMbWXzBl29381800 = wNAyLjZVQOiyTMbWXzBl43165590;     wNAyLjZVQOiyTMbWXzBl43165590 = wNAyLjZVQOiyTMbWXzBl31422457;     wNAyLjZVQOiyTMbWXzBl31422457 = wNAyLjZVQOiyTMbWXzBl11371947;     wNAyLjZVQOiyTMbWXzBl11371947 = wNAyLjZVQOiyTMbWXzBl93201706;     wNAyLjZVQOiyTMbWXzBl93201706 = wNAyLjZVQOiyTMbWXzBl86646882;     wNAyLjZVQOiyTMbWXzBl86646882 = wNAyLjZVQOiyTMbWXzBl51236158;     wNAyLjZVQOiyTMbWXzBl51236158 = wNAyLjZVQOiyTMbWXzBl369609;     wNAyLjZVQOiyTMbWXzBl369609 = wNAyLjZVQOiyTMbWXzBl92164542;     wNAyLjZVQOiyTMbWXzBl92164542 = wNAyLjZVQOiyTMbWXzBl8037180;     wNAyLjZVQOiyTMbWXzBl8037180 = wNAyLjZVQOiyTMbWXzBl70623247;     wNAyLjZVQOiyTMbWXzBl70623247 = wNAyLjZVQOiyTMbWXzBl4959936;     wNAyLjZVQOiyTMbWXzBl4959936 = wNAyLjZVQOiyTMbWXzBl78142403;     wNAyLjZVQOiyTMbWXzBl78142403 = wNAyLjZVQOiyTMbWXzBl39376202;     wNAyLjZVQOiyTMbWXzBl39376202 = wNAyLjZVQOiyTMbWXzBl43588809;     wNAyLjZVQOiyTMbWXzBl43588809 = wNAyLjZVQOiyTMbWXzBl88374210;     wNAyLjZVQOiyTMbWXzBl88374210 = wNAyLjZVQOiyTMbWXzBl36119154;     wNAyLjZVQOiyTMbWXzBl36119154 = wNAyLjZVQOiyTMbWXzBl87251909;     wNAyLjZVQOiyTMbWXzBl87251909 = wNAyLjZVQOiyTMbWXzBl3835942;     wNAyLjZVQOiyTMbWXzBl3835942 = wNAyLjZVQOiyTMbWXzBl69679093;     wNAyLjZVQOiyTMbWXzBl69679093 = wNAyLjZVQOiyTMbWXzBl51222209;     wNAyLjZVQOiyTMbWXzBl51222209 = wNAyLjZVQOiyTMbWXzBl60391548;     wNAyLjZVQOiyTMbWXzBl60391548 = wNAyLjZVQOiyTMbWXzBl59490030;     wNAyLjZVQOiyTMbWXzBl59490030 = wNAyLjZVQOiyTMbWXzBl68733905;     wNAyLjZVQOiyTMbWXzBl68733905 = wNAyLjZVQOiyTMbWXzBl44877779;     wNAyLjZVQOiyTMbWXzBl44877779 = wNAyLjZVQOiyTMbWXzBl51668869;     wNAyLjZVQOiyTMbWXzBl51668869 = wNAyLjZVQOiyTMbWXzBl45877787;     wNAyLjZVQOiyTMbWXzBl45877787 = wNAyLjZVQOiyTMbWXzBl56003239;     wNAyLjZVQOiyTMbWXzBl56003239 = wNAyLjZVQOiyTMbWXzBl82222081;     wNAyLjZVQOiyTMbWXzBl82222081 = wNAyLjZVQOiyTMbWXzBl54817662;     wNAyLjZVQOiyTMbWXzBl54817662 = wNAyLjZVQOiyTMbWXzBl17693935;     wNAyLjZVQOiyTMbWXzBl17693935 = wNAyLjZVQOiyTMbWXzBl31623260;     wNAyLjZVQOiyTMbWXzBl31623260 = wNAyLjZVQOiyTMbWXzBl17797349;     wNAyLjZVQOiyTMbWXzBl17797349 = wNAyLjZVQOiyTMbWXzBl70313220;     wNAyLjZVQOiyTMbWXzBl70313220 = wNAyLjZVQOiyTMbWXzBl90221465;     wNAyLjZVQOiyTMbWXzBl90221465 = wNAyLjZVQOiyTMbWXzBl30619290;     wNAyLjZVQOiyTMbWXzBl30619290 = wNAyLjZVQOiyTMbWXzBl34070305;     wNAyLjZVQOiyTMbWXzBl34070305 = wNAyLjZVQOiyTMbWXzBl90167337;     wNAyLjZVQOiyTMbWXzBl90167337 = wNAyLjZVQOiyTMbWXzBl93255919;     wNAyLjZVQOiyTMbWXzBl93255919 = wNAyLjZVQOiyTMbWXzBl38888620;     wNAyLjZVQOiyTMbWXzBl38888620 = wNAyLjZVQOiyTMbWXzBl42390030;     wNAyLjZVQOiyTMbWXzBl42390030 = wNAyLjZVQOiyTMbWXzBl10338012;     wNAyLjZVQOiyTMbWXzBl10338012 = wNAyLjZVQOiyTMbWXzBl63245098;     wNAyLjZVQOiyTMbWXzBl63245098 = wNAyLjZVQOiyTMbWXzBl56285348;     wNAyLjZVQOiyTMbWXzBl56285348 = wNAyLjZVQOiyTMbWXzBl54890890;     wNAyLjZVQOiyTMbWXzBl54890890 = wNAyLjZVQOiyTMbWXzBl61134398;     wNAyLjZVQOiyTMbWXzBl61134398 = wNAyLjZVQOiyTMbWXzBl62658185;     wNAyLjZVQOiyTMbWXzBl62658185 = wNAyLjZVQOiyTMbWXzBl21083348;     wNAyLjZVQOiyTMbWXzBl21083348 = wNAyLjZVQOiyTMbWXzBl41276403;     wNAyLjZVQOiyTMbWXzBl41276403 = wNAyLjZVQOiyTMbWXzBl88539471;     wNAyLjZVQOiyTMbWXzBl88539471 = wNAyLjZVQOiyTMbWXzBl65548346;     wNAyLjZVQOiyTMbWXzBl65548346 = wNAyLjZVQOiyTMbWXzBl30374722;     wNAyLjZVQOiyTMbWXzBl30374722 = wNAyLjZVQOiyTMbWXzBl85339834;     wNAyLjZVQOiyTMbWXzBl85339834 = wNAyLjZVQOiyTMbWXzBl25832997;     wNAyLjZVQOiyTMbWXzBl25832997 = wNAyLjZVQOiyTMbWXzBl42484890;     wNAyLjZVQOiyTMbWXzBl42484890 = wNAyLjZVQOiyTMbWXzBl68990252;     wNAyLjZVQOiyTMbWXzBl68990252 = wNAyLjZVQOiyTMbWXzBl83675559;     wNAyLjZVQOiyTMbWXzBl83675559 = wNAyLjZVQOiyTMbWXzBl62688551;     wNAyLjZVQOiyTMbWXzBl62688551 = wNAyLjZVQOiyTMbWXzBl66494167;     wNAyLjZVQOiyTMbWXzBl66494167 = wNAyLjZVQOiyTMbWXzBl41532837;     wNAyLjZVQOiyTMbWXzBl41532837 = wNAyLjZVQOiyTMbWXzBl40769095;     wNAyLjZVQOiyTMbWXzBl40769095 = wNAyLjZVQOiyTMbWXzBl95232918;     wNAyLjZVQOiyTMbWXzBl95232918 = wNAyLjZVQOiyTMbWXzBl18147527;     wNAyLjZVQOiyTMbWXzBl18147527 = wNAyLjZVQOiyTMbWXzBl37346880;     wNAyLjZVQOiyTMbWXzBl37346880 = wNAyLjZVQOiyTMbWXzBl90343245;     wNAyLjZVQOiyTMbWXzBl90343245 = wNAyLjZVQOiyTMbWXzBl38999987;     wNAyLjZVQOiyTMbWXzBl38999987 = wNAyLjZVQOiyTMbWXzBl87162586;     wNAyLjZVQOiyTMbWXzBl87162586 = wNAyLjZVQOiyTMbWXzBl7829183;     wNAyLjZVQOiyTMbWXzBl7829183 = wNAyLjZVQOiyTMbWXzBl49154737;     wNAyLjZVQOiyTMbWXzBl49154737 = wNAyLjZVQOiyTMbWXzBl12969520;     wNAyLjZVQOiyTMbWXzBl12969520 = wNAyLjZVQOiyTMbWXzBl54303905;     wNAyLjZVQOiyTMbWXzBl54303905 = wNAyLjZVQOiyTMbWXzBl45951817;     wNAyLjZVQOiyTMbWXzBl45951817 = wNAyLjZVQOiyTMbWXzBl93995990;     wNAyLjZVQOiyTMbWXzBl93995990 = wNAyLjZVQOiyTMbWXzBl64947321;     wNAyLjZVQOiyTMbWXzBl64947321 = wNAyLjZVQOiyTMbWXzBl27289063;     wNAyLjZVQOiyTMbWXzBl27289063 = wNAyLjZVQOiyTMbWXzBl40884197;     wNAyLjZVQOiyTMbWXzBl40884197 = wNAyLjZVQOiyTMbWXzBl97146449;     wNAyLjZVQOiyTMbWXzBl97146449 = wNAyLjZVQOiyTMbWXzBl3204683;     wNAyLjZVQOiyTMbWXzBl3204683 = wNAyLjZVQOiyTMbWXzBl13843016;     wNAyLjZVQOiyTMbWXzBl13843016 = wNAyLjZVQOiyTMbWXzBl83743381;     wNAyLjZVQOiyTMbWXzBl83743381 = wNAyLjZVQOiyTMbWXzBl89010683;     wNAyLjZVQOiyTMbWXzBl89010683 = wNAyLjZVQOiyTMbWXzBl24794440;     wNAyLjZVQOiyTMbWXzBl24794440 = wNAyLjZVQOiyTMbWXzBl14726837;     wNAyLjZVQOiyTMbWXzBl14726837 = wNAyLjZVQOiyTMbWXzBl93682610;     wNAyLjZVQOiyTMbWXzBl93682610 = wNAyLjZVQOiyTMbWXzBl89269316;     wNAyLjZVQOiyTMbWXzBl89269316 = wNAyLjZVQOiyTMbWXzBl87319212;     wNAyLjZVQOiyTMbWXzBl87319212 = wNAyLjZVQOiyTMbWXzBl46283425;     wNAyLjZVQOiyTMbWXzBl46283425 = wNAyLjZVQOiyTMbWXzBl91964352;     wNAyLjZVQOiyTMbWXzBl91964352 = wNAyLjZVQOiyTMbWXzBl27828331;     wNAyLjZVQOiyTMbWXzBl27828331 = wNAyLjZVQOiyTMbWXzBl21231214;     wNAyLjZVQOiyTMbWXzBl21231214 = wNAyLjZVQOiyTMbWXzBl46943730;     wNAyLjZVQOiyTMbWXzBl46943730 = wNAyLjZVQOiyTMbWXzBl40800589;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void CvkSRclofLpJEAVTSICA72753077() {     float cQLMCIYdLkSwOuLiDCBE40794118 = -87324851;    float cQLMCIYdLkSwOuLiDCBE99031866 = -782190254;    float cQLMCIYdLkSwOuLiDCBE86178423 = -388174715;    float cQLMCIYdLkSwOuLiDCBE61993542 = -841226060;    float cQLMCIYdLkSwOuLiDCBE10507603 = -949627092;    float cQLMCIYdLkSwOuLiDCBE52432064 = -785337482;    float cQLMCIYdLkSwOuLiDCBE57723111 = -419656061;    float cQLMCIYdLkSwOuLiDCBE64726140 = 21185812;    float cQLMCIYdLkSwOuLiDCBE76447262 = -538846141;    float cQLMCIYdLkSwOuLiDCBE26021314 = -923116949;    float cQLMCIYdLkSwOuLiDCBE63486195 = -356353044;    float cQLMCIYdLkSwOuLiDCBE45714134 = 55736363;    float cQLMCIYdLkSwOuLiDCBE43224860 = -710282761;    float cQLMCIYdLkSwOuLiDCBE47045926 = -968561829;    float cQLMCIYdLkSwOuLiDCBE31818992 = -747151025;    float cQLMCIYdLkSwOuLiDCBE25800705 = -932629609;    float cQLMCIYdLkSwOuLiDCBE56222368 = -595296927;    float cQLMCIYdLkSwOuLiDCBE15165075 = -313088353;    float cQLMCIYdLkSwOuLiDCBE28465450 = -500374651;    float cQLMCIYdLkSwOuLiDCBE42697010 = -821742626;    float cQLMCIYdLkSwOuLiDCBE84629359 = -925286725;    float cQLMCIYdLkSwOuLiDCBE56438669 = -972690987;    float cQLMCIYdLkSwOuLiDCBE76254598 = -548699201;    float cQLMCIYdLkSwOuLiDCBE87238917 = -303176829;    float cQLMCIYdLkSwOuLiDCBE16816900 = -150972637;    float cQLMCIYdLkSwOuLiDCBE52693231 = -22639028;    float cQLMCIYdLkSwOuLiDCBE57473491 = 68314369;    float cQLMCIYdLkSwOuLiDCBE39239466 = 19886210;    float cQLMCIYdLkSwOuLiDCBE82357540 = -512805955;    float cQLMCIYdLkSwOuLiDCBE27312079 = -755549196;    float cQLMCIYdLkSwOuLiDCBE93804226 = -339912658;    float cQLMCIYdLkSwOuLiDCBE44213888 = -511812390;    float cQLMCIYdLkSwOuLiDCBE54186328 = -894785463;    float cQLMCIYdLkSwOuLiDCBE45160616 = -150398051;    float cQLMCIYdLkSwOuLiDCBE34026907 = -347170259;    float cQLMCIYdLkSwOuLiDCBE88705977 = -834700636;    float cQLMCIYdLkSwOuLiDCBE67944607 = -634651382;    float cQLMCIYdLkSwOuLiDCBE52139761 = -260156919;    float cQLMCIYdLkSwOuLiDCBE90131259 = -831229954;    float cQLMCIYdLkSwOuLiDCBE89566827 = -73800849;    float cQLMCIYdLkSwOuLiDCBE19478927 = -304213358;    float cQLMCIYdLkSwOuLiDCBE3495010 = -783903709;    float cQLMCIYdLkSwOuLiDCBE30596581 = -481657014;    float cQLMCIYdLkSwOuLiDCBE49453364 = -449477171;    float cQLMCIYdLkSwOuLiDCBE14637342 = 2739086;    float cQLMCIYdLkSwOuLiDCBE74498273 = -230520565;    float cQLMCIYdLkSwOuLiDCBE17865130 = -530661133;    float cQLMCIYdLkSwOuLiDCBE68639500 = -954843866;    float cQLMCIYdLkSwOuLiDCBE57547009 = -897363213;    float cQLMCIYdLkSwOuLiDCBE62435453 = -26679588;    float cQLMCIYdLkSwOuLiDCBE23597382 = -293754971;    float cQLMCIYdLkSwOuLiDCBE77792355 = -354521143;    float cQLMCIYdLkSwOuLiDCBE31733697 = -603549985;    float cQLMCIYdLkSwOuLiDCBE65141160 = -988658328;    float cQLMCIYdLkSwOuLiDCBE88123718 = -25697895;    float cQLMCIYdLkSwOuLiDCBE84355448 = -114633865;    float cQLMCIYdLkSwOuLiDCBE22777268 = -133491054;    float cQLMCIYdLkSwOuLiDCBE98939506 = 15002114;    float cQLMCIYdLkSwOuLiDCBE45176642 = -590253423;    float cQLMCIYdLkSwOuLiDCBE57814371 = -826988065;    float cQLMCIYdLkSwOuLiDCBE94958572 = -753651852;    float cQLMCIYdLkSwOuLiDCBE18483645 = -339542272;    float cQLMCIYdLkSwOuLiDCBE82368600 = -466008234;    float cQLMCIYdLkSwOuLiDCBE49135184 = -783296946;    float cQLMCIYdLkSwOuLiDCBE32217087 = -483204292;    float cQLMCIYdLkSwOuLiDCBE19272307 = -844540655;    float cQLMCIYdLkSwOuLiDCBE91527805 = -49478174;    float cQLMCIYdLkSwOuLiDCBE98064243 = -459884710;    float cQLMCIYdLkSwOuLiDCBE13019020 = -521391570;    float cQLMCIYdLkSwOuLiDCBE43113015 = -912450389;    float cQLMCIYdLkSwOuLiDCBE57856098 = -197978227;    float cQLMCIYdLkSwOuLiDCBE4082607 = -235140008;    float cQLMCIYdLkSwOuLiDCBE25033816 = -481858399;    float cQLMCIYdLkSwOuLiDCBE38898623 = -326573803;    float cQLMCIYdLkSwOuLiDCBE23218083 = -417529269;    float cQLMCIYdLkSwOuLiDCBE81134349 = -41383016;    float cQLMCIYdLkSwOuLiDCBE25842089 = -391033973;    float cQLMCIYdLkSwOuLiDCBE26801235 = 777969;    float cQLMCIYdLkSwOuLiDCBE72601575 = -205915916;    float cQLMCIYdLkSwOuLiDCBE42318626 = -920452072;    float cQLMCIYdLkSwOuLiDCBE34828101 = -491977895;    float cQLMCIYdLkSwOuLiDCBE88833991 = 23158235;    float cQLMCIYdLkSwOuLiDCBE81692456 = -82750578;    float cQLMCIYdLkSwOuLiDCBE19922087 = -386126367;    float cQLMCIYdLkSwOuLiDCBE3714698 = -361794225;    float cQLMCIYdLkSwOuLiDCBE16011871 = -985391515;    float cQLMCIYdLkSwOuLiDCBE12480192 = -908262405;    float cQLMCIYdLkSwOuLiDCBE89045167 = -906127135;    float cQLMCIYdLkSwOuLiDCBE57036898 = -24700157;    float cQLMCIYdLkSwOuLiDCBE49671458 = -132536395;    float cQLMCIYdLkSwOuLiDCBE65928709 = -601209583;    float cQLMCIYdLkSwOuLiDCBE69005100 = -549653496;    float cQLMCIYdLkSwOuLiDCBE6963119 = -669903496;    float cQLMCIYdLkSwOuLiDCBE32316888 = 95758110;    float cQLMCIYdLkSwOuLiDCBE94608254 = -320148997;    float cQLMCIYdLkSwOuLiDCBE995283 = -964671087;    float cQLMCIYdLkSwOuLiDCBE21126410 = -217895476;    float cQLMCIYdLkSwOuLiDCBE81461397 = -698360069;    float cQLMCIYdLkSwOuLiDCBE17236277 = -966272880;    float cQLMCIYdLkSwOuLiDCBE95365034 = -87324851;     cQLMCIYdLkSwOuLiDCBE40794118 = cQLMCIYdLkSwOuLiDCBE99031866;     cQLMCIYdLkSwOuLiDCBE99031866 = cQLMCIYdLkSwOuLiDCBE86178423;     cQLMCIYdLkSwOuLiDCBE86178423 = cQLMCIYdLkSwOuLiDCBE61993542;     cQLMCIYdLkSwOuLiDCBE61993542 = cQLMCIYdLkSwOuLiDCBE10507603;     cQLMCIYdLkSwOuLiDCBE10507603 = cQLMCIYdLkSwOuLiDCBE52432064;     cQLMCIYdLkSwOuLiDCBE52432064 = cQLMCIYdLkSwOuLiDCBE57723111;     cQLMCIYdLkSwOuLiDCBE57723111 = cQLMCIYdLkSwOuLiDCBE64726140;     cQLMCIYdLkSwOuLiDCBE64726140 = cQLMCIYdLkSwOuLiDCBE76447262;     cQLMCIYdLkSwOuLiDCBE76447262 = cQLMCIYdLkSwOuLiDCBE26021314;     cQLMCIYdLkSwOuLiDCBE26021314 = cQLMCIYdLkSwOuLiDCBE63486195;     cQLMCIYdLkSwOuLiDCBE63486195 = cQLMCIYdLkSwOuLiDCBE45714134;     cQLMCIYdLkSwOuLiDCBE45714134 = cQLMCIYdLkSwOuLiDCBE43224860;     cQLMCIYdLkSwOuLiDCBE43224860 = cQLMCIYdLkSwOuLiDCBE47045926;     cQLMCIYdLkSwOuLiDCBE47045926 = cQLMCIYdLkSwOuLiDCBE31818992;     cQLMCIYdLkSwOuLiDCBE31818992 = cQLMCIYdLkSwOuLiDCBE25800705;     cQLMCIYdLkSwOuLiDCBE25800705 = cQLMCIYdLkSwOuLiDCBE56222368;     cQLMCIYdLkSwOuLiDCBE56222368 = cQLMCIYdLkSwOuLiDCBE15165075;     cQLMCIYdLkSwOuLiDCBE15165075 = cQLMCIYdLkSwOuLiDCBE28465450;     cQLMCIYdLkSwOuLiDCBE28465450 = cQLMCIYdLkSwOuLiDCBE42697010;     cQLMCIYdLkSwOuLiDCBE42697010 = cQLMCIYdLkSwOuLiDCBE84629359;     cQLMCIYdLkSwOuLiDCBE84629359 = cQLMCIYdLkSwOuLiDCBE56438669;     cQLMCIYdLkSwOuLiDCBE56438669 = cQLMCIYdLkSwOuLiDCBE76254598;     cQLMCIYdLkSwOuLiDCBE76254598 = cQLMCIYdLkSwOuLiDCBE87238917;     cQLMCIYdLkSwOuLiDCBE87238917 = cQLMCIYdLkSwOuLiDCBE16816900;     cQLMCIYdLkSwOuLiDCBE16816900 = cQLMCIYdLkSwOuLiDCBE52693231;     cQLMCIYdLkSwOuLiDCBE52693231 = cQLMCIYdLkSwOuLiDCBE57473491;     cQLMCIYdLkSwOuLiDCBE57473491 = cQLMCIYdLkSwOuLiDCBE39239466;     cQLMCIYdLkSwOuLiDCBE39239466 = cQLMCIYdLkSwOuLiDCBE82357540;     cQLMCIYdLkSwOuLiDCBE82357540 = cQLMCIYdLkSwOuLiDCBE27312079;     cQLMCIYdLkSwOuLiDCBE27312079 = cQLMCIYdLkSwOuLiDCBE93804226;     cQLMCIYdLkSwOuLiDCBE93804226 = cQLMCIYdLkSwOuLiDCBE44213888;     cQLMCIYdLkSwOuLiDCBE44213888 = cQLMCIYdLkSwOuLiDCBE54186328;     cQLMCIYdLkSwOuLiDCBE54186328 = cQLMCIYdLkSwOuLiDCBE45160616;     cQLMCIYdLkSwOuLiDCBE45160616 = cQLMCIYdLkSwOuLiDCBE34026907;     cQLMCIYdLkSwOuLiDCBE34026907 = cQLMCIYdLkSwOuLiDCBE88705977;     cQLMCIYdLkSwOuLiDCBE88705977 = cQLMCIYdLkSwOuLiDCBE67944607;     cQLMCIYdLkSwOuLiDCBE67944607 = cQLMCIYdLkSwOuLiDCBE52139761;     cQLMCIYdLkSwOuLiDCBE52139761 = cQLMCIYdLkSwOuLiDCBE90131259;     cQLMCIYdLkSwOuLiDCBE90131259 = cQLMCIYdLkSwOuLiDCBE89566827;     cQLMCIYdLkSwOuLiDCBE89566827 = cQLMCIYdLkSwOuLiDCBE19478927;     cQLMCIYdLkSwOuLiDCBE19478927 = cQLMCIYdLkSwOuLiDCBE3495010;     cQLMCIYdLkSwOuLiDCBE3495010 = cQLMCIYdLkSwOuLiDCBE30596581;     cQLMCIYdLkSwOuLiDCBE30596581 = cQLMCIYdLkSwOuLiDCBE49453364;     cQLMCIYdLkSwOuLiDCBE49453364 = cQLMCIYdLkSwOuLiDCBE14637342;     cQLMCIYdLkSwOuLiDCBE14637342 = cQLMCIYdLkSwOuLiDCBE74498273;     cQLMCIYdLkSwOuLiDCBE74498273 = cQLMCIYdLkSwOuLiDCBE17865130;     cQLMCIYdLkSwOuLiDCBE17865130 = cQLMCIYdLkSwOuLiDCBE68639500;     cQLMCIYdLkSwOuLiDCBE68639500 = cQLMCIYdLkSwOuLiDCBE57547009;     cQLMCIYdLkSwOuLiDCBE57547009 = cQLMCIYdLkSwOuLiDCBE62435453;     cQLMCIYdLkSwOuLiDCBE62435453 = cQLMCIYdLkSwOuLiDCBE23597382;     cQLMCIYdLkSwOuLiDCBE23597382 = cQLMCIYdLkSwOuLiDCBE77792355;     cQLMCIYdLkSwOuLiDCBE77792355 = cQLMCIYdLkSwOuLiDCBE31733697;     cQLMCIYdLkSwOuLiDCBE31733697 = cQLMCIYdLkSwOuLiDCBE65141160;     cQLMCIYdLkSwOuLiDCBE65141160 = cQLMCIYdLkSwOuLiDCBE88123718;     cQLMCIYdLkSwOuLiDCBE88123718 = cQLMCIYdLkSwOuLiDCBE84355448;     cQLMCIYdLkSwOuLiDCBE84355448 = cQLMCIYdLkSwOuLiDCBE22777268;     cQLMCIYdLkSwOuLiDCBE22777268 = cQLMCIYdLkSwOuLiDCBE98939506;     cQLMCIYdLkSwOuLiDCBE98939506 = cQLMCIYdLkSwOuLiDCBE45176642;     cQLMCIYdLkSwOuLiDCBE45176642 = cQLMCIYdLkSwOuLiDCBE57814371;     cQLMCIYdLkSwOuLiDCBE57814371 = cQLMCIYdLkSwOuLiDCBE94958572;     cQLMCIYdLkSwOuLiDCBE94958572 = cQLMCIYdLkSwOuLiDCBE18483645;     cQLMCIYdLkSwOuLiDCBE18483645 = cQLMCIYdLkSwOuLiDCBE82368600;     cQLMCIYdLkSwOuLiDCBE82368600 = cQLMCIYdLkSwOuLiDCBE49135184;     cQLMCIYdLkSwOuLiDCBE49135184 = cQLMCIYdLkSwOuLiDCBE32217087;     cQLMCIYdLkSwOuLiDCBE32217087 = cQLMCIYdLkSwOuLiDCBE19272307;     cQLMCIYdLkSwOuLiDCBE19272307 = cQLMCIYdLkSwOuLiDCBE91527805;     cQLMCIYdLkSwOuLiDCBE91527805 = cQLMCIYdLkSwOuLiDCBE98064243;     cQLMCIYdLkSwOuLiDCBE98064243 = cQLMCIYdLkSwOuLiDCBE13019020;     cQLMCIYdLkSwOuLiDCBE13019020 = cQLMCIYdLkSwOuLiDCBE43113015;     cQLMCIYdLkSwOuLiDCBE43113015 = cQLMCIYdLkSwOuLiDCBE57856098;     cQLMCIYdLkSwOuLiDCBE57856098 = cQLMCIYdLkSwOuLiDCBE4082607;     cQLMCIYdLkSwOuLiDCBE4082607 = cQLMCIYdLkSwOuLiDCBE25033816;     cQLMCIYdLkSwOuLiDCBE25033816 = cQLMCIYdLkSwOuLiDCBE38898623;     cQLMCIYdLkSwOuLiDCBE38898623 = cQLMCIYdLkSwOuLiDCBE23218083;     cQLMCIYdLkSwOuLiDCBE23218083 = cQLMCIYdLkSwOuLiDCBE81134349;     cQLMCIYdLkSwOuLiDCBE81134349 = cQLMCIYdLkSwOuLiDCBE25842089;     cQLMCIYdLkSwOuLiDCBE25842089 = cQLMCIYdLkSwOuLiDCBE26801235;     cQLMCIYdLkSwOuLiDCBE26801235 = cQLMCIYdLkSwOuLiDCBE72601575;     cQLMCIYdLkSwOuLiDCBE72601575 = cQLMCIYdLkSwOuLiDCBE42318626;     cQLMCIYdLkSwOuLiDCBE42318626 = cQLMCIYdLkSwOuLiDCBE34828101;     cQLMCIYdLkSwOuLiDCBE34828101 = cQLMCIYdLkSwOuLiDCBE88833991;     cQLMCIYdLkSwOuLiDCBE88833991 = cQLMCIYdLkSwOuLiDCBE81692456;     cQLMCIYdLkSwOuLiDCBE81692456 = cQLMCIYdLkSwOuLiDCBE19922087;     cQLMCIYdLkSwOuLiDCBE19922087 = cQLMCIYdLkSwOuLiDCBE3714698;     cQLMCIYdLkSwOuLiDCBE3714698 = cQLMCIYdLkSwOuLiDCBE16011871;     cQLMCIYdLkSwOuLiDCBE16011871 = cQLMCIYdLkSwOuLiDCBE12480192;     cQLMCIYdLkSwOuLiDCBE12480192 = cQLMCIYdLkSwOuLiDCBE89045167;     cQLMCIYdLkSwOuLiDCBE89045167 = cQLMCIYdLkSwOuLiDCBE57036898;     cQLMCIYdLkSwOuLiDCBE57036898 = cQLMCIYdLkSwOuLiDCBE49671458;     cQLMCIYdLkSwOuLiDCBE49671458 = cQLMCIYdLkSwOuLiDCBE65928709;     cQLMCIYdLkSwOuLiDCBE65928709 = cQLMCIYdLkSwOuLiDCBE69005100;     cQLMCIYdLkSwOuLiDCBE69005100 = cQLMCIYdLkSwOuLiDCBE6963119;     cQLMCIYdLkSwOuLiDCBE6963119 = cQLMCIYdLkSwOuLiDCBE32316888;     cQLMCIYdLkSwOuLiDCBE32316888 = cQLMCIYdLkSwOuLiDCBE94608254;     cQLMCIYdLkSwOuLiDCBE94608254 = cQLMCIYdLkSwOuLiDCBE995283;     cQLMCIYdLkSwOuLiDCBE995283 = cQLMCIYdLkSwOuLiDCBE21126410;     cQLMCIYdLkSwOuLiDCBE21126410 = cQLMCIYdLkSwOuLiDCBE81461397;     cQLMCIYdLkSwOuLiDCBE81461397 = cQLMCIYdLkSwOuLiDCBE17236277;     cQLMCIYdLkSwOuLiDCBE17236277 = cQLMCIYdLkSwOuLiDCBE95365034;     cQLMCIYdLkSwOuLiDCBE95365034 = cQLMCIYdLkSwOuLiDCBE40794118;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void JuAzAUTYeiToNrXoMkOA27169291() {     float xQPsFnandxGSLjyXDpvj11445785 = -705473981;    float xQPsFnandxGSLjyXDpvj67705384 = -653833554;    float xQPsFnandxGSLjyXDpvj66147342 = -830469174;    float xQPsFnandxGSLjyXDpvj1924829 = 68635551;    float xQPsFnandxGSLjyXDpvj31957924 = -733033439;    float xQPsFnandxGSLjyXDpvj42431826 = -486918634;    float xQPsFnandxGSLjyXDpvj76102006 = -418431771;    float xQPsFnandxGSLjyXDpvj21568521 = -325631278;    float xQPsFnandxGSLjyXDpvj22312317 = -632193470;    float xQPsFnandxGSLjyXDpvj48987774 = -124212299;    float xQPsFnandxGSLjyXDpvj5735356 = -391254974;    float xQPsFnandxGSLjyXDpvj91260705 = -260027430;    float xQPsFnandxGSLjyXDpvj23052247 = -619819819;    float xQPsFnandxGSLjyXDpvj30892960 = -769590039;    float xQPsFnandxGSLjyXDpvj14539011 = -896901028;    float xQPsFnandxGSLjyXDpvj58329345 = -502613196;    float xQPsFnandxGSLjyXDpvj50075194 = -504482057;    float xQPsFnandxGSLjyXDpvj35458569 = -302254549;    float xQPsFnandxGSLjyXDpvj10173232 = -107562593;    float xQPsFnandxGSLjyXDpvj1483719 = -264108931;    float xQPsFnandxGSLjyXDpvj39765114 = -833965866;    float xQPsFnandxGSLjyXDpvj17834434 = -203976292;    float xQPsFnandxGSLjyXDpvj78198407 = -16542346;    float xQPsFnandxGSLjyXDpvj21088454 = -995577735;    float xQPsFnandxGSLjyXDpvj80941288 = -926877325;    float xQPsFnandxGSLjyXDpvj83290811 = -120923473;    float xQPsFnandxGSLjyXDpvj65734277 = -326863406;    float xQPsFnandxGSLjyXDpvj47021691 = -485237558;    float xQPsFnandxGSLjyXDpvj14500028 = -807164172;    float xQPsFnandxGSLjyXDpvj66484174 = -213944266;    float xQPsFnandxGSLjyXDpvj876224 = -88346343;    float xQPsFnandxGSLjyXDpvj88725348 = -840109552;    float xQPsFnandxGSLjyXDpvj37131689 = -841747072;    float xQPsFnandxGSLjyXDpvj10525592 = -813927736;    float xQPsFnandxGSLjyXDpvj12051144 = -194117039;    float xQPsFnandxGSLjyXDpvj48699236 = -913927184;    float xQPsFnandxGSLjyXDpvj11736093 = -823244590;    float xQPsFnandxGSLjyXDpvj4932250 = -44270026;    float xQPsFnandxGSLjyXDpvj68664548 = -786958591;    float xQPsFnandxGSLjyXDpvj84670125 = -691409211;    float xQPsFnandxGSLjyXDpvj79552211 = -688232237;    float xQPsFnandxGSLjyXDpvj31978001 = -553292779;    float xQPsFnandxGSLjyXDpvj79563540 = -468411066;    float xQPsFnandxGSLjyXDpvj55961664 = -521452142;    float xQPsFnandxGSLjyXDpvj10591401 = -305907308;    float xQPsFnandxGSLjyXDpvj80904108 = 11142280;    float xQPsFnandxGSLjyXDpvj43996333 = -646730933;    float xQPsFnandxGSLjyXDpvj34253710 = -306974162;    float xQPsFnandxGSLjyXDpvj62988462 = -883127182;    float xQPsFnandxGSLjyXDpvj70862940 = -309688467;    float xQPsFnandxGSLjyXDpvj91786705 = -940327092;    float xQPsFnandxGSLjyXDpvj19663861 = -323740865;    float xQPsFnandxGSLjyXDpvj86809353 = -531348027;    float xQPsFnandxGSLjyXDpvj55368465 = -549539825;    float xQPsFnandxGSLjyXDpvj87404176 = -136638809;    float xQPsFnandxGSLjyXDpvj93611350 = -401497690;    float xQPsFnandxGSLjyXDpvj89506976 = -537291208;    float xQPsFnandxGSLjyXDpvj45058888 = -834891440;    float xQPsFnandxGSLjyXDpvj20983541 = -4487125;    float xQPsFnandxGSLjyXDpvj48667112 = -512109967;    float xQPsFnandxGSLjyXDpvj76697549 = -60055228;    float xQPsFnandxGSLjyXDpvj29080316 = -933194214;    float xQPsFnandxGSLjyXDpvj7068493 = -518467107;    float xQPsFnandxGSLjyXDpvj55828142 = -318249204;    float xQPsFnandxGSLjyXDpvj48111551 = 64134043;    float xQPsFnandxGSLjyXDpvj17010007 = -551145422;    float xQPsFnandxGSLjyXDpvj54129016 = -418280358;    float xQPsFnandxGSLjyXDpvj12526656 = -805892084;    float xQPsFnandxGSLjyXDpvj18841817 = -475473001;    float xQPsFnandxGSLjyXDpvj65839775 = -982973844;    float xQPsFnandxGSLjyXDpvj46593252 = -679368607;    float xQPsFnandxGSLjyXDpvj45142945 = -360212031;    float xQPsFnandxGSLjyXDpvj66794021 = -515295959;    float xQPsFnandxGSLjyXDpvj25503106 = -416153382;    float xQPsFnandxGSLjyXDpvj21931508 = -575876695;    float xQPsFnandxGSLjyXDpvj7787114 = -180673087;    float xQPsFnandxGSLjyXDpvj38270894 = -735565227;    float xQPsFnandxGSLjyXDpvj22236743 = -495090204;    float xQPsFnandxGSLjyXDpvj10497054 = -589670427;    float xQPsFnandxGSLjyXDpvj37181 = -838019605;    float xQPsFnandxGSLjyXDpvj39294479 = -474192541;    float xQPsFnandxGSLjyXDpvj31480567 = 80110755;    float xQPsFnandxGSLjyXDpvj84033229 = -602110377;    float xQPsFnandxGSLjyXDpvj43637088 = -397475706;    float xQPsFnandxGSLjyXDpvj74697468 = -273617175;    float xQPsFnandxGSLjyXDpvj81212363 = -764605479;    float xQPsFnandxGSLjyXDpvj1915995 = -208761526;    float xQPsFnandxGSLjyXDpvj81763224 = -192207248;    float xQPsFnandxGSLjyXDpvj23121415 = -577288927;    float xQPsFnandxGSLjyXDpvj18439793 = -792619350;    float xQPsFnandxGSLjyXDpvj59192259 = -276635976;    float xQPsFnandxGSLjyXDpvj66677204 = -988353150;    float xQPsFnandxGSLjyXDpvj83948709 = 60217098;    float xQPsFnandxGSLjyXDpvj19997436 = -174848625;    float xQPsFnandxGSLjyXDpvj7972577 = -531353984;    float xQPsFnandxGSLjyXDpvj50471896 = -755038023;    float xQPsFnandxGSLjyXDpvj24909508 = 65174327;    float xQPsFnandxGSLjyXDpvj23735399 = -50161863;    float xQPsFnandxGSLjyXDpvj7850114 = -485586186;    float xQPsFnandxGSLjyXDpvj93581393 = -705473981;     xQPsFnandxGSLjyXDpvj11445785 = xQPsFnandxGSLjyXDpvj67705384;     xQPsFnandxGSLjyXDpvj67705384 = xQPsFnandxGSLjyXDpvj66147342;     xQPsFnandxGSLjyXDpvj66147342 = xQPsFnandxGSLjyXDpvj1924829;     xQPsFnandxGSLjyXDpvj1924829 = xQPsFnandxGSLjyXDpvj31957924;     xQPsFnandxGSLjyXDpvj31957924 = xQPsFnandxGSLjyXDpvj42431826;     xQPsFnandxGSLjyXDpvj42431826 = xQPsFnandxGSLjyXDpvj76102006;     xQPsFnandxGSLjyXDpvj76102006 = xQPsFnandxGSLjyXDpvj21568521;     xQPsFnandxGSLjyXDpvj21568521 = xQPsFnandxGSLjyXDpvj22312317;     xQPsFnandxGSLjyXDpvj22312317 = xQPsFnandxGSLjyXDpvj48987774;     xQPsFnandxGSLjyXDpvj48987774 = xQPsFnandxGSLjyXDpvj5735356;     xQPsFnandxGSLjyXDpvj5735356 = xQPsFnandxGSLjyXDpvj91260705;     xQPsFnandxGSLjyXDpvj91260705 = xQPsFnandxGSLjyXDpvj23052247;     xQPsFnandxGSLjyXDpvj23052247 = xQPsFnandxGSLjyXDpvj30892960;     xQPsFnandxGSLjyXDpvj30892960 = xQPsFnandxGSLjyXDpvj14539011;     xQPsFnandxGSLjyXDpvj14539011 = xQPsFnandxGSLjyXDpvj58329345;     xQPsFnandxGSLjyXDpvj58329345 = xQPsFnandxGSLjyXDpvj50075194;     xQPsFnandxGSLjyXDpvj50075194 = xQPsFnandxGSLjyXDpvj35458569;     xQPsFnandxGSLjyXDpvj35458569 = xQPsFnandxGSLjyXDpvj10173232;     xQPsFnandxGSLjyXDpvj10173232 = xQPsFnandxGSLjyXDpvj1483719;     xQPsFnandxGSLjyXDpvj1483719 = xQPsFnandxGSLjyXDpvj39765114;     xQPsFnandxGSLjyXDpvj39765114 = xQPsFnandxGSLjyXDpvj17834434;     xQPsFnandxGSLjyXDpvj17834434 = xQPsFnandxGSLjyXDpvj78198407;     xQPsFnandxGSLjyXDpvj78198407 = xQPsFnandxGSLjyXDpvj21088454;     xQPsFnandxGSLjyXDpvj21088454 = xQPsFnandxGSLjyXDpvj80941288;     xQPsFnandxGSLjyXDpvj80941288 = xQPsFnandxGSLjyXDpvj83290811;     xQPsFnandxGSLjyXDpvj83290811 = xQPsFnandxGSLjyXDpvj65734277;     xQPsFnandxGSLjyXDpvj65734277 = xQPsFnandxGSLjyXDpvj47021691;     xQPsFnandxGSLjyXDpvj47021691 = xQPsFnandxGSLjyXDpvj14500028;     xQPsFnandxGSLjyXDpvj14500028 = xQPsFnandxGSLjyXDpvj66484174;     xQPsFnandxGSLjyXDpvj66484174 = xQPsFnandxGSLjyXDpvj876224;     xQPsFnandxGSLjyXDpvj876224 = xQPsFnandxGSLjyXDpvj88725348;     xQPsFnandxGSLjyXDpvj88725348 = xQPsFnandxGSLjyXDpvj37131689;     xQPsFnandxGSLjyXDpvj37131689 = xQPsFnandxGSLjyXDpvj10525592;     xQPsFnandxGSLjyXDpvj10525592 = xQPsFnandxGSLjyXDpvj12051144;     xQPsFnandxGSLjyXDpvj12051144 = xQPsFnandxGSLjyXDpvj48699236;     xQPsFnandxGSLjyXDpvj48699236 = xQPsFnandxGSLjyXDpvj11736093;     xQPsFnandxGSLjyXDpvj11736093 = xQPsFnandxGSLjyXDpvj4932250;     xQPsFnandxGSLjyXDpvj4932250 = xQPsFnandxGSLjyXDpvj68664548;     xQPsFnandxGSLjyXDpvj68664548 = xQPsFnandxGSLjyXDpvj84670125;     xQPsFnandxGSLjyXDpvj84670125 = xQPsFnandxGSLjyXDpvj79552211;     xQPsFnandxGSLjyXDpvj79552211 = xQPsFnandxGSLjyXDpvj31978001;     xQPsFnandxGSLjyXDpvj31978001 = xQPsFnandxGSLjyXDpvj79563540;     xQPsFnandxGSLjyXDpvj79563540 = xQPsFnandxGSLjyXDpvj55961664;     xQPsFnandxGSLjyXDpvj55961664 = xQPsFnandxGSLjyXDpvj10591401;     xQPsFnandxGSLjyXDpvj10591401 = xQPsFnandxGSLjyXDpvj80904108;     xQPsFnandxGSLjyXDpvj80904108 = xQPsFnandxGSLjyXDpvj43996333;     xQPsFnandxGSLjyXDpvj43996333 = xQPsFnandxGSLjyXDpvj34253710;     xQPsFnandxGSLjyXDpvj34253710 = xQPsFnandxGSLjyXDpvj62988462;     xQPsFnandxGSLjyXDpvj62988462 = xQPsFnandxGSLjyXDpvj70862940;     xQPsFnandxGSLjyXDpvj70862940 = xQPsFnandxGSLjyXDpvj91786705;     xQPsFnandxGSLjyXDpvj91786705 = xQPsFnandxGSLjyXDpvj19663861;     xQPsFnandxGSLjyXDpvj19663861 = xQPsFnandxGSLjyXDpvj86809353;     xQPsFnandxGSLjyXDpvj86809353 = xQPsFnandxGSLjyXDpvj55368465;     xQPsFnandxGSLjyXDpvj55368465 = xQPsFnandxGSLjyXDpvj87404176;     xQPsFnandxGSLjyXDpvj87404176 = xQPsFnandxGSLjyXDpvj93611350;     xQPsFnandxGSLjyXDpvj93611350 = xQPsFnandxGSLjyXDpvj89506976;     xQPsFnandxGSLjyXDpvj89506976 = xQPsFnandxGSLjyXDpvj45058888;     xQPsFnandxGSLjyXDpvj45058888 = xQPsFnandxGSLjyXDpvj20983541;     xQPsFnandxGSLjyXDpvj20983541 = xQPsFnandxGSLjyXDpvj48667112;     xQPsFnandxGSLjyXDpvj48667112 = xQPsFnandxGSLjyXDpvj76697549;     xQPsFnandxGSLjyXDpvj76697549 = xQPsFnandxGSLjyXDpvj29080316;     xQPsFnandxGSLjyXDpvj29080316 = xQPsFnandxGSLjyXDpvj7068493;     xQPsFnandxGSLjyXDpvj7068493 = xQPsFnandxGSLjyXDpvj55828142;     xQPsFnandxGSLjyXDpvj55828142 = xQPsFnandxGSLjyXDpvj48111551;     xQPsFnandxGSLjyXDpvj48111551 = xQPsFnandxGSLjyXDpvj17010007;     xQPsFnandxGSLjyXDpvj17010007 = xQPsFnandxGSLjyXDpvj54129016;     xQPsFnandxGSLjyXDpvj54129016 = xQPsFnandxGSLjyXDpvj12526656;     xQPsFnandxGSLjyXDpvj12526656 = xQPsFnandxGSLjyXDpvj18841817;     xQPsFnandxGSLjyXDpvj18841817 = xQPsFnandxGSLjyXDpvj65839775;     xQPsFnandxGSLjyXDpvj65839775 = xQPsFnandxGSLjyXDpvj46593252;     xQPsFnandxGSLjyXDpvj46593252 = xQPsFnandxGSLjyXDpvj45142945;     xQPsFnandxGSLjyXDpvj45142945 = xQPsFnandxGSLjyXDpvj66794021;     xQPsFnandxGSLjyXDpvj66794021 = xQPsFnandxGSLjyXDpvj25503106;     xQPsFnandxGSLjyXDpvj25503106 = xQPsFnandxGSLjyXDpvj21931508;     xQPsFnandxGSLjyXDpvj21931508 = xQPsFnandxGSLjyXDpvj7787114;     xQPsFnandxGSLjyXDpvj7787114 = xQPsFnandxGSLjyXDpvj38270894;     xQPsFnandxGSLjyXDpvj38270894 = xQPsFnandxGSLjyXDpvj22236743;     xQPsFnandxGSLjyXDpvj22236743 = xQPsFnandxGSLjyXDpvj10497054;     xQPsFnandxGSLjyXDpvj10497054 = xQPsFnandxGSLjyXDpvj37181;     xQPsFnandxGSLjyXDpvj37181 = xQPsFnandxGSLjyXDpvj39294479;     xQPsFnandxGSLjyXDpvj39294479 = xQPsFnandxGSLjyXDpvj31480567;     xQPsFnandxGSLjyXDpvj31480567 = xQPsFnandxGSLjyXDpvj84033229;     xQPsFnandxGSLjyXDpvj84033229 = xQPsFnandxGSLjyXDpvj43637088;     xQPsFnandxGSLjyXDpvj43637088 = xQPsFnandxGSLjyXDpvj74697468;     xQPsFnandxGSLjyXDpvj74697468 = xQPsFnandxGSLjyXDpvj81212363;     xQPsFnandxGSLjyXDpvj81212363 = xQPsFnandxGSLjyXDpvj1915995;     xQPsFnandxGSLjyXDpvj1915995 = xQPsFnandxGSLjyXDpvj81763224;     xQPsFnandxGSLjyXDpvj81763224 = xQPsFnandxGSLjyXDpvj23121415;     xQPsFnandxGSLjyXDpvj23121415 = xQPsFnandxGSLjyXDpvj18439793;     xQPsFnandxGSLjyXDpvj18439793 = xQPsFnandxGSLjyXDpvj59192259;     xQPsFnandxGSLjyXDpvj59192259 = xQPsFnandxGSLjyXDpvj66677204;     xQPsFnandxGSLjyXDpvj66677204 = xQPsFnandxGSLjyXDpvj83948709;     xQPsFnandxGSLjyXDpvj83948709 = xQPsFnandxGSLjyXDpvj19997436;     xQPsFnandxGSLjyXDpvj19997436 = xQPsFnandxGSLjyXDpvj7972577;     xQPsFnandxGSLjyXDpvj7972577 = xQPsFnandxGSLjyXDpvj50471896;     xQPsFnandxGSLjyXDpvj50471896 = xQPsFnandxGSLjyXDpvj24909508;     xQPsFnandxGSLjyXDpvj24909508 = xQPsFnandxGSLjyXDpvj23735399;     xQPsFnandxGSLjyXDpvj23735399 = xQPsFnandxGSLjyXDpvj7850114;     xQPsFnandxGSLjyXDpvj7850114 = xQPsFnandxGSLjyXDpvj93581393;     xQPsFnandxGSLjyXDpvj93581393 = xQPsFnandxGSLjyXDpvj11445785;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kJAbYjZEMJMxqgPoEFAz33828036() {     float IqKivbWOUFPjjpSVddAg11439314 = -577998123;    float IqKivbWOUFPjjpSVddAg6277701 = -696428679;    float IqKivbWOUFPjjpSVddAg67460554 = -787597788;    float IqKivbWOUFPjjpSVddAg87004689 = -851905279;    float IqKivbWOUFPjjpSVddAg40798026 = -101388761;    float IqKivbWOUFPjjpSVddAg77237258 = -186351445;    float IqKivbWOUFPjjpSVddAg44649343 = -122968238;    float IqKivbWOUFPjjpSVddAg90782571 = -29808595;    float IqKivbWOUFPjjpSVddAg5052481 = -345839540;    float IqKivbWOUFPjjpSVddAg45627288 = -861019170;    float IqKivbWOUFPjjpSVddAg26055961 = -350727046;    float IqKivbWOUFPjjpSVddAg5552383 = 4494208;    float IqKivbWOUFPjjpSVddAg54905161 = -641478294;    float IqKivbWOUFPjjpSVddAg84737180 = -14743090;    float IqKivbWOUFPjjpSVddAg59711120 = -859613880;    float IqKivbWOUFPjjpSVddAg32893893 = -835408847;    float IqKivbWOUFPjjpSVddAg5927954 = -324096627;    float IqKivbWOUFPjjpSVddAg58459102 = -699326171;    float IqKivbWOUFPjjpSVddAg30601502 = -725544860;    float IqKivbWOUFPjjpSVddAg73557482 = -876133382;    float IqKivbWOUFPjjpSVddAg19434538 = -544432239;    float IqKivbWOUFPjjpSVddAg96130699 = -713968175;    float IqKivbWOUFPjjpSVddAg15076804 = -475500188;    float IqKivbWOUFPjjpSVddAg64738562 = -876498257;    float IqKivbWOUFPjjpSVddAg9383978 = -253571178;    float IqKivbWOUFPjjpSVddAg99864888 = -29129401;    float IqKivbWOUFPjjpSVddAg35955859 = -627663119;    float IqKivbWOUFPjjpSVddAg82425216 = 5795454;    float IqKivbWOUFPjjpSVddAg27178475 = -353234487;    float IqKivbWOUFPjjpSVddAg42574044 = -989375640;    float IqKivbWOUFPjjpSVddAg34288902 = -475439578;    float IqKivbWOUFPjjpSVddAg73449206 = -131931314;    float IqKivbWOUFPjjpSVddAg22584113 = 56308712;    float IqKivbWOUFPjjpSVddAg10808429 = -690609468;    float IqKivbWOUFPjjpSVddAg94409181 = -453430964;    float IqKivbWOUFPjjpSVddAg91527425 = -993692930;    float IqKivbWOUFPjjpSVddAg23677461 = -922897968;    float IqKivbWOUFPjjpSVddAg74849929 = -39532705;    float IqKivbWOUFPjjpSVddAg3978145 = -399640007;    float IqKivbWOUFPjjpSVddAg56543018 = -300840897;    float IqKivbWOUFPjjpSVddAg67407878 = -776176135;    float IqKivbWOUFPjjpSVddAg17675662 = -747084861;    float IqKivbWOUFPjjpSVddAg39846901 = 72458680;    float IqKivbWOUFPjjpSVddAg15193563 = -78416586;    float IqKivbWOUFPjjpSVddAg94609452 = -779166710;    float IqKivbWOUFPjjpSVddAg21332077 = -900902391;    float IqKivbWOUFPjjpSVddAg71694125 = -28011555;    float IqKivbWOUFPjjpSVddAg9637291 = -915735518;    float IqKivbWOUFPjjpSVddAg81646851 = -446891043;    float IqKivbWOUFPjjpSVddAg90908362 = -996713353;    float IqKivbWOUFPjjpSVddAg5046076 = -518882100;    float IqKivbWOUFPjjpSVddAg34211118 = -705886613;    float IqKivbWOUFPjjpSVddAg62257702 = -423402035;    float IqKivbWOUFPjjpSVddAg65618736 = 18540324;    float IqKivbWOUFPjjpSVddAg14393498 = -993538087;    float IqKivbWOUFPjjpSVddAg15308614 = -864029948;    float IqKivbWOUFPjjpSVddAg91200896 = -120928491;    float IqKivbWOUFPjjpSVddAg2721993 = -911099531;    float IqKivbWOUFPjjpSVddAg77620711 = -498334102;    float IqKivbWOUFPjjpSVddAg40933138 = 27740640;    float IqKivbWOUFPjjpSVddAg41281400 = -558688327;    float IqKivbWOUFPjjpSVddAg62224126 = -28763693;    float IqKivbWOUFPjjpSVddAg63604096 = -676574109;    float IqKivbWOUFPjjpSVddAg62478436 = -356463900;    float IqKivbWOUFPjjpSVddAg11338386 = -285579593;    float IqKivbWOUFPjjpSVddAg52606755 = -118795732;    float IqKivbWOUFPjjpSVddAg82968269 = 48185495;    float IqKivbWOUFPjjpSVddAg44096733 = -950868826;    float IqKivbWOUFPjjpSVddAg90327999 = -561312127;    float IqKivbWOUFPjjpSVddAg68183694 = -865920950;    float IqKivbWOUFPjjpSVddAg9216432 = -912510879;    float IqKivbWOUFPjjpSVddAg31078025 = -184563922;    float IqKivbWOUFPjjpSVddAg54480957 = -199686164;    float IqKivbWOUFPjjpSVddAg74058483 = -324703963;    float IqKivbWOUFPjjpSVddAg6149604 = 42752;    float IqKivbWOUFPjjpSVddAg1758877 = -797347379;    float IqKivbWOUFPjjpSVddAg56283799 = -686426856;    float IqKivbWOUFPjjpSVddAg99883241 = -297083603;    float IqKivbWOUFPjjpSVddAg70129110 = 2668452;    float IqKivbWOUFPjjpSVddAg88051901 = -352668787;    float IqKivbWOUFPjjpSVddAg28170763 = 98882153;    float IqKivbWOUFPjjpSVddAg26318568 = -711927602;    float IqKivbWOUFPjjpSVddAg778366 = -547313504;    float IqKivbWOUFPjjpSVddAg36270112 = -356521134;    float IqKivbWOUFPjjpSVddAg37527969 = -370493541;    float IqKivbWOUFPjjpSVddAg77785 = -769552965;    float IqKivbWOUFPjjpSVddAg11191504 = -708529280;    float IqKivbWOUFPjjpSVddAg56965376 = -962231612;    float IqKivbWOUFPjjpSVddAg96414931 = -697071381;    float IqKivbWOUFPjjpSVddAg79100568 = -589401016;    float IqKivbWOUFPjjpSVddAg326529 = -772764439;    float IqKivbWOUFPjjpSVddAg20955469 = 88201563;    float IqKivbWOUFPjjpSVddAg97229217 = -541198604;    float IqKivbWOUFPjjpSVddAg63045007 = -327380648;    float IqKivbWOUFPjjpSVddAg15261619 = -742152571;    float IqKivbWOUFPjjpSVddAg5183753 = -647412443;    float IqKivbWOUFPjjpSVddAg54071565 = 29489247;    float IqKivbWOUFPjjpSVddAg77368464 = -571077421;    float IqKivbWOUFPjjpSVddAg3855177 = -792836993;    float IqKivbWOUFPjjpSVddAg42002698 = -577998123;     IqKivbWOUFPjjpSVddAg11439314 = IqKivbWOUFPjjpSVddAg6277701;     IqKivbWOUFPjjpSVddAg6277701 = IqKivbWOUFPjjpSVddAg67460554;     IqKivbWOUFPjjpSVddAg67460554 = IqKivbWOUFPjjpSVddAg87004689;     IqKivbWOUFPjjpSVddAg87004689 = IqKivbWOUFPjjpSVddAg40798026;     IqKivbWOUFPjjpSVddAg40798026 = IqKivbWOUFPjjpSVddAg77237258;     IqKivbWOUFPjjpSVddAg77237258 = IqKivbWOUFPjjpSVddAg44649343;     IqKivbWOUFPjjpSVddAg44649343 = IqKivbWOUFPjjpSVddAg90782571;     IqKivbWOUFPjjpSVddAg90782571 = IqKivbWOUFPjjpSVddAg5052481;     IqKivbWOUFPjjpSVddAg5052481 = IqKivbWOUFPjjpSVddAg45627288;     IqKivbWOUFPjjpSVddAg45627288 = IqKivbWOUFPjjpSVddAg26055961;     IqKivbWOUFPjjpSVddAg26055961 = IqKivbWOUFPjjpSVddAg5552383;     IqKivbWOUFPjjpSVddAg5552383 = IqKivbWOUFPjjpSVddAg54905161;     IqKivbWOUFPjjpSVddAg54905161 = IqKivbWOUFPjjpSVddAg84737180;     IqKivbWOUFPjjpSVddAg84737180 = IqKivbWOUFPjjpSVddAg59711120;     IqKivbWOUFPjjpSVddAg59711120 = IqKivbWOUFPjjpSVddAg32893893;     IqKivbWOUFPjjpSVddAg32893893 = IqKivbWOUFPjjpSVddAg5927954;     IqKivbWOUFPjjpSVddAg5927954 = IqKivbWOUFPjjpSVddAg58459102;     IqKivbWOUFPjjpSVddAg58459102 = IqKivbWOUFPjjpSVddAg30601502;     IqKivbWOUFPjjpSVddAg30601502 = IqKivbWOUFPjjpSVddAg73557482;     IqKivbWOUFPjjpSVddAg73557482 = IqKivbWOUFPjjpSVddAg19434538;     IqKivbWOUFPjjpSVddAg19434538 = IqKivbWOUFPjjpSVddAg96130699;     IqKivbWOUFPjjpSVddAg96130699 = IqKivbWOUFPjjpSVddAg15076804;     IqKivbWOUFPjjpSVddAg15076804 = IqKivbWOUFPjjpSVddAg64738562;     IqKivbWOUFPjjpSVddAg64738562 = IqKivbWOUFPjjpSVddAg9383978;     IqKivbWOUFPjjpSVddAg9383978 = IqKivbWOUFPjjpSVddAg99864888;     IqKivbWOUFPjjpSVddAg99864888 = IqKivbWOUFPjjpSVddAg35955859;     IqKivbWOUFPjjpSVddAg35955859 = IqKivbWOUFPjjpSVddAg82425216;     IqKivbWOUFPjjpSVddAg82425216 = IqKivbWOUFPjjpSVddAg27178475;     IqKivbWOUFPjjpSVddAg27178475 = IqKivbWOUFPjjpSVddAg42574044;     IqKivbWOUFPjjpSVddAg42574044 = IqKivbWOUFPjjpSVddAg34288902;     IqKivbWOUFPjjpSVddAg34288902 = IqKivbWOUFPjjpSVddAg73449206;     IqKivbWOUFPjjpSVddAg73449206 = IqKivbWOUFPjjpSVddAg22584113;     IqKivbWOUFPjjpSVddAg22584113 = IqKivbWOUFPjjpSVddAg10808429;     IqKivbWOUFPjjpSVddAg10808429 = IqKivbWOUFPjjpSVddAg94409181;     IqKivbWOUFPjjpSVddAg94409181 = IqKivbWOUFPjjpSVddAg91527425;     IqKivbWOUFPjjpSVddAg91527425 = IqKivbWOUFPjjpSVddAg23677461;     IqKivbWOUFPjjpSVddAg23677461 = IqKivbWOUFPjjpSVddAg74849929;     IqKivbWOUFPjjpSVddAg74849929 = IqKivbWOUFPjjpSVddAg3978145;     IqKivbWOUFPjjpSVddAg3978145 = IqKivbWOUFPjjpSVddAg56543018;     IqKivbWOUFPjjpSVddAg56543018 = IqKivbWOUFPjjpSVddAg67407878;     IqKivbWOUFPjjpSVddAg67407878 = IqKivbWOUFPjjpSVddAg17675662;     IqKivbWOUFPjjpSVddAg17675662 = IqKivbWOUFPjjpSVddAg39846901;     IqKivbWOUFPjjpSVddAg39846901 = IqKivbWOUFPjjpSVddAg15193563;     IqKivbWOUFPjjpSVddAg15193563 = IqKivbWOUFPjjpSVddAg94609452;     IqKivbWOUFPjjpSVddAg94609452 = IqKivbWOUFPjjpSVddAg21332077;     IqKivbWOUFPjjpSVddAg21332077 = IqKivbWOUFPjjpSVddAg71694125;     IqKivbWOUFPjjpSVddAg71694125 = IqKivbWOUFPjjpSVddAg9637291;     IqKivbWOUFPjjpSVddAg9637291 = IqKivbWOUFPjjpSVddAg81646851;     IqKivbWOUFPjjpSVddAg81646851 = IqKivbWOUFPjjpSVddAg90908362;     IqKivbWOUFPjjpSVddAg90908362 = IqKivbWOUFPjjpSVddAg5046076;     IqKivbWOUFPjjpSVddAg5046076 = IqKivbWOUFPjjpSVddAg34211118;     IqKivbWOUFPjjpSVddAg34211118 = IqKivbWOUFPjjpSVddAg62257702;     IqKivbWOUFPjjpSVddAg62257702 = IqKivbWOUFPjjpSVddAg65618736;     IqKivbWOUFPjjpSVddAg65618736 = IqKivbWOUFPjjpSVddAg14393498;     IqKivbWOUFPjjpSVddAg14393498 = IqKivbWOUFPjjpSVddAg15308614;     IqKivbWOUFPjjpSVddAg15308614 = IqKivbWOUFPjjpSVddAg91200896;     IqKivbWOUFPjjpSVddAg91200896 = IqKivbWOUFPjjpSVddAg2721993;     IqKivbWOUFPjjpSVddAg2721993 = IqKivbWOUFPjjpSVddAg77620711;     IqKivbWOUFPjjpSVddAg77620711 = IqKivbWOUFPjjpSVddAg40933138;     IqKivbWOUFPjjpSVddAg40933138 = IqKivbWOUFPjjpSVddAg41281400;     IqKivbWOUFPjjpSVddAg41281400 = IqKivbWOUFPjjpSVddAg62224126;     IqKivbWOUFPjjpSVddAg62224126 = IqKivbWOUFPjjpSVddAg63604096;     IqKivbWOUFPjjpSVddAg63604096 = IqKivbWOUFPjjpSVddAg62478436;     IqKivbWOUFPjjpSVddAg62478436 = IqKivbWOUFPjjpSVddAg11338386;     IqKivbWOUFPjjpSVddAg11338386 = IqKivbWOUFPjjpSVddAg52606755;     IqKivbWOUFPjjpSVddAg52606755 = IqKivbWOUFPjjpSVddAg82968269;     IqKivbWOUFPjjpSVddAg82968269 = IqKivbWOUFPjjpSVddAg44096733;     IqKivbWOUFPjjpSVddAg44096733 = IqKivbWOUFPjjpSVddAg90327999;     IqKivbWOUFPjjpSVddAg90327999 = IqKivbWOUFPjjpSVddAg68183694;     IqKivbWOUFPjjpSVddAg68183694 = IqKivbWOUFPjjpSVddAg9216432;     IqKivbWOUFPjjpSVddAg9216432 = IqKivbWOUFPjjpSVddAg31078025;     IqKivbWOUFPjjpSVddAg31078025 = IqKivbWOUFPjjpSVddAg54480957;     IqKivbWOUFPjjpSVddAg54480957 = IqKivbWOUFPjjpSVddAg74058483;     IqKivbWOUFPjjpSVddAg74058483 = IqKivbWOUFPjjpSVddAg6149604;     IqKivbWOUFPjjpSVddAg6149604 = IqKivbWOUFPjjpSVddAg1758877;     IqKivbWOUFPjjpSVddAg1758877 = IqKivbWOUFPjjpSVddAg56283799;     IqKivbWOUFPjjpSVddAg56283799 = IqKivbWOUFPjjpSVddAg99883241;     IqKivbWOUFPjjpSVddAg99883241 = IqKivbWOUFPjjpSVddAg70129110;     IqKivbWOUFPjjpSVddAg70129110 = IqKivbWOUFPjjpSVddAg88051901;     IqKivbWOUFPjjpSVddAg88051901 = IqKivbWOUFPjjpSVddAg28170763;     IqKivbWOUFPjjpSVddAg28170763 = IqKivbWOUFPjjpSVddAg26318568;     IqKivbWOUFPjjpSVddAg26318568 = IqKivbWOUFPjjpSVddAg778366;     IqKivbWOUFPjjpSVddAg778366 = IqKivbWOUFPjjpSVddAg36270112;     IqKivbWOUFPjjpSVddAg36270112 = IqKivbWOUFPjjpSVddAg37527969;     IqKivbWOUFPjjpSVddAg37527969 = IqKivbWOUFPjjpSVddAg77785;     IqKivbWOUFPjjpSVddAg77785 = IqKivbWOUFPjjpSVddAg11191504;     IqKivbWOUFPjjpSVddAg11191504 = IqKivbWOUFPjjpSVddAg56965376;     IqKivbWOUFPjjpSVddAg56965376 = IqKivbWOUFPjjpSVddAg96414931;     IqKivbWOUFPjjpSVddAg96414931 = IqKivbWOUFPjjpSVddAg79100568;     IqKivbWOUFPjjpSVddAg79100568 = IqKivbWOUFPjjpSVddAg326529;     IqKivbWOUFPjjpSVddAg326529 = IqKivbWOUFPjjpSVddAg20955469;     IqKivbWOUFPjjpSVddAg20955469 = IqKivbWOUFPjjpSVddAg97229217;     IqKivbWOUFPjjpSVddAg97229217 = IqKivbWOUFPjjpSVddAg63045007;     IqKivbWOUFPjjpSVddAg63045007 = IqKivbWOUFPjjpSVddAg15261619;     IqKivbWOUFPjjpSVddAg15261619 = IqKivbWOUFPjjpSVddAg5183753;     IqKivbWOUFPjjpSVddAg5183753 = IqKivbWOUFPjjpSVddAg54071565;     IqKivbWOUFPjjpSVddAg54071565 = IqKivbWOUFPjjpSVddAg77368464;     IqKivbWOUFPjjpSVddAg77368464 = IqKivbWOUFPjjpSVddAg3855177;     IqKivbWOUFPjjpSVddAg3855177 = IqKivbWOUFPjjpSVddAg42002698;     IqKivbWOUFPjjpSVddAg42002698 = IqKivbWOUFPjjpSVddAg11439314;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void KkhhvbGGRMXVyaAoruqq88244248() {     float UARKAXVusJleXttldSBH82090980 = -96147253;    float UARKAXVusJleXttldSBH74951218 = -568071978;    float UARKAXVusJleXttldSBH47429474 = -129892248;    float UARKAXVusJleXttldSBH26935976 = 57956332;    float UARKAXVusJleXttldSBH62248347 = -984795108;    float UARKAXVusJleXttldSBH67237021 = -987932597;    float UARKAXVusJleXttldSBH63028238 = -121743948;    float UARKAXVusJleXttldSBH47624952 = -376625685;    float UARKAXVusJleXttldSBH50917535 = -439186868;    float UARKAXVusJleXttldSBH68593749 = -62114521;    float UARKAXVusJleXttldSBH68305121 = -385628975;    float UARKAXVusJleXttldSBH51098953 = -311269585;    float UARKAXVusJleXttldSBH34732548 = -551015352;    float UARKAXVusJleXttldSBH68584214 = -915771300;    float UARKAXVusJleXttldSBH42431140 = 90636117;    float UARKAXVusJleXttldSBH65422533 = -405392433;    float UARKAXVusJleXttldSBH99780780 = -233281756;    float UARKAXVusJleXttldSBH78752596 = -688492367;    float UARKAXVusJleXttldSBH12309283 = -332732802;    float UARKAXVusJleXttldSBH32344191 = -318499687;    float UARKAXVusJleXttldSBH74570292 = -453111380;    float UARKAXVusJleXttldSBH57526465 = 54746519;    float UARKAXVusJleXttldSBH17020613 = 56656667;    float UARKAXVusJleXttldSBH98588099 = -468899163;    float UARKAXVusJleXttldSBH73508366 = 70524135;    float UARKAXVusJleXttldSBH30462469 = -127413846;    float UARKAXVusJleXttldSBH44216645 = 77159106;    float UARKAXVusJleXttldSBH90207441 = -499328314;    float UARKAXVusJleXttldSBH59320963 = -647592704;    float UARKAXVusJleXttldSBH81746140 = -447770711;    float UARKAXVusJleXttldSBH41360899 = -223873263;    float UARKAXVusJleXttldSBH17960667 = -460228477;    float UARKAXVusJleXttldSBH5529474 = -990652897;    float UARKAXVusJleXttldSBH76173403 = -254139152;    float UARKAXVusJleXttldSBH72433417 = -300377743;    float UARKAXVusJleXttldSBH51520685 = 27080522;    float UARKAXVusJleXttldSBH67468946 = -11491176;    float UARKAXVusJleXttldSBH27642418 = -923645813;    float UARKAXVusJleXttldSBH82511433 = -355368644;    float UARKAXVusJleXttldSBH51646316 = -918449260;    float UARKAXVusJleXttldSBH27481163 = -60195013;    float UARKAXVusJleXttldSBH46158653 = -516473931;    float UARKAXVusJleXttldSBH88813860 = 85704628;    float UARKAXVusJleXttldSBH21701863 = -150391557;    float UARKAXVusJleXttldSBH90563511 = 12186896;    float UARKAXVusJleXttldSBH27737912 = -659239546;    float UARKAXVusJleXttldSBH97825328 = -144081354;    float UARKAXVusJleXttldSBH75251501 = -267865813;    float UARKAXVusJleXttldSBH87088303 = -432655012;    float UARKAXVusJleXttldSBH99335849 = -179722232;    float UARKAXVusJleXttldSBH73235400 = -65454221;    float UARKAXVusJleXttldSBH76082622 = -675106335;    float UARKAXVusJleXttldSBH17333359 = -351200077;    float UARKAXVusJleXttldSBH55846040 = -642341173;    float UARKAXVusJleXttldSBH13673956 = -4479001;    float UARKAXVusJleXttldSBH24564516 = -50893773;    float UARKAXVusJleXttldSBH57930605 = -524728646;    float UARKAXVusJleXttldSBH48841374 = -660993085;    float UARKAXVusJleXttldSBH53427610 = 87432197;    float UARKAXVusJleXttldSBH31785879 = -757381263;    float UARKAXVusJleXttldSBH23020377 = -965091703;    float UARKAXVusJleXttldSBH72820796 = -622415635;    float UARKAXVusJleXttldSBH88303989 = -729032982;    float UARKAXVusJleXttldSBH69171395 = -991416158;    float UARKAXVusJleXttldSBH27232850 = -838241258;    float UARKAXVusJleXttldSBH50344455 = -925400499;    float UARKAXVusJleXttldSBH45569480 = -320616689;    float UARKAXVusJleXttldSBH58559145 = -196876200;    float UARKAXVusJleXttldSBH96150796 = -515393558;    float UARKAXVusJleXttldSBH90910455 = -936444405;    float UARKAXVusJleXttldSBH97953586 = -293901258;    float UARKAXVusJleXttldSBH72138363 = -309635944;    float UARKAXVusJleXttldSBH96241162 = -233123724;    float UARKAXVusJleXttldSBH60662966 = -414283542;    float UARKAXVusJleXttldSBH4863029 = -158304674;    float UARKAXVusJleXttldSBH28411640 = -936637450;    float UARKAXVusJleXttldSBH68712604 = 69041891;    float UARKAXVusJleXttldSBH95318749 = -792951776;    float UARKAXVusJleXttldSBH8024588 = -381086060;    float UARKAXVusJleXttldSBH45770455 = -270236320;    float UARKAXVusJleXttldSBH32637141 = -983332492;    float UARKAXVusJleXttldSBH68965143 = -654975082;    float UARKAXVusJleXttldSBH3119139 = 33326698;    float UARKAXVusJleXttldSBH59985113 = -367870473;    float UARKAXVusJleXttldSBH8510741 = -282316491;    float UARKAXVusJleXttldSBH65278276 = -548766929;    float UARKAXVusJleXttldSBH627308 = -9028401;    float UARKAXVusJleXttldSBH49683433 = -248311724;    float UARKAXVusJleXttldSBH62499447 = -149660151;    float UARKAXVusJleXttldSBH47868902 = -149483971;    float UARKAXVusJleXttldSBH93590079 = -448190832;    float UARKAXVusJleXttldSBH18627573 = -350498091;    float UARKAXVusJleXttldSBH74214808 = -911078010;    float UARKAXVusJleXttldSBH50725555 = -597987382;    float UARKAXVusJleXttldSBH28625940 = -953357557;    float UARKAXVusJleXttldSBH54660366 = -437779379;    float UARKAXVusJleXttldSBH57854663 = -787440949;    float UARKAXVusJleXttldSBH19642466 = 77120785;    float UARKAXVusJleXttldSBH94469012 = -312150299;    float UARKAXVusJleXttldSBH40219056 = -96147253;     UARKAXVusJleXttldSBH82090980 = UARKAXVusJleXttldSBH74951218;     UARKAXVusJleXttldSBH74951218 = UARKAXVusJleXttldSBH47429474;     UARKAXVusJleXttldSBH47429474 = UARKAXVusJleXttldSBH26935976;     UARKAXVusJleXttldSBH26935976 = UARKAXVusJleXttldSBH62248347;     UARKAXVusJleXttldSBH62248347 = UARKAXVusJleXttldSBH67237021;     UARKAXVusJleXttldSBH67237021 = UARKAXVusJleXttldSBH63028238;     UARKAXVusJleXttldSBH63028238 = UARKAXVusJleXttldSBH47624952;     UARKAXVusJleXttldSBH47624952 = UARKAXVusJleXttldSBH50917535;     UARKAXVusJleXttldSBH50917535 = UARKAXVusJleXttldSBH68593749;     UARKAXVusJleXttldSBH68593749 = UARKAXVusJleXttldSBH68305121;     UARKAXVusJleXttldSBH68305121 = UARKAXVusJleXttldSBH51098953;     UARKAXVusJleXttldSBH51098953 = UARKAXVusJleXttldSBH34732548;     UARKAXVusJleXttldSBH34732548 = UARKAXVusJleXttldSBH68584214;     UARKAXVusJleXttldSBH68584214 = UARKAXVusJleXttldSBH42431140;     UARKAXVusJleXttldSBH42431140 = UARKAXVusJleXttldSBH65422533;     UARKAXVusJleXttldSBH65422533 = UARKAXVusJleXttldSBH99780780;     UARKAXVusJleXttldSBH99780780 = UARKAXVusJleXttldSBH78752596;     UARKAXVusJleXttldSBH78752596 = UARKAXVusJleXttldSBH12309283;     UARKAXVusJleXttldSBH12309283 = UARKAXVusJleXttldSBH32344191;     UARKAXVusJleXttldSBH32344191 = UARKAXVusJleXttldSBH74570292;     UARKAXVusJleXttldSBH74570292 = UARKAXVusJleXttldSBH57526465;     UARKAXVusJleXttldSBH57526465 = UARKAXVusJleXttldSBH17020613;     UARKAXVusJleXttldSBH17020613 = UARKAXVusJleXttldSBH98588099;     UARKAXVusJleXttldSBH98588099 = UARKAXVusJleXttldSBH73508366;     UARKAXVusJleXttldSBH73508366 = UARKAXVusJleXttldSBH30462469;     UARKAXVusJleXttldSBH30462469 = UARKAXVusJleXttldSBH44216645;     UARKAXVusJleXttldSBH44216645 = UARKAXVusJleXttldSBH90207441;     UARKAXVusJleXttldSBH90207441 = UARKAXVusJleXttldSBH59320963;     UARKAXVusJleXttldSBH59320963 = UARKAXVusJleXttldSBH81746140;     UARKAXVusJleXttldSBH81746140 = UARKAXVusJleXttldSBH41360899;     UARKAXVusJleXttldSBH41360899 = UARKAXVusJleXttldSBH17960667;     UARKAXVusJleXttldSBH17960667 = UARKAXVusJleXttldSBH5529474;     UARKAXVusJleXttldSBH5529474 = UARKAXVusJleXttldSBH76173403;     UARKAXVusJleXttldSBH76173403 = UARKAXVusJleXttldSBH72433417;     UARKAXVusJleXttldSBH72433417 = UARKAXVusJleXttldSBH51520685;     UARKAXVusJleXttldSBH51520685 = UARKAXVusJleXttldSBH67468946;     UARKAXVusJleXttldSBH67468946 = UARKAXVusJleXttldSBH27642418;     UARKAXVusJleXttldSBH27642418 = UARKAXVusJleXttldSBH82511433;     UARKAXVusJleXttldSBH82511433 = UARKAXVusJleXttldSBH51646316;     UARKAXVusJleXttldSBH51646316 = UARKAXVusJleXttldSBH27481163;     UARKAXVusJleXttldSBH27481163 = UARKAXVusJleXttldSBH46158653;     UARKAXVusJleXttldSBH46158653 = UARKAXVusJleXttldSBH88813860;     UARKAXVusJleXttldSBH88813860 = UARKAXVusJleXttldSBH21701863;     UARKAXVusJleXttldSBH21701863 = UARKAXVusJleXttldSBH90563511;     UARKAXVusJleXttldSBH90563511 = UARKAXVusJleXttldSBH27737912;     UARKAXVusJleXttldSBH27737912 = UARKAXVusJleXttldSBH97825328;     UARKAXVusJleXttldSBH97825328 = UARKAXVusJleXttldSBH75251501;     UARKAXVusJleXttldSBH75251501 = UARKAXVusJleXttldSBH87088303;     UARKAXVusJleXttldSBH87088303 = UARKAXVusJleXttldSBH99335849;     UARKAXVusJleXttldSBH99335849 = UARKAXVusJleXttldSBH73235400;     UARKAXVusJleXttldSBH73235400 = UARKAXVusJleXttldSBH76082622;     UARKAXVusJleXttldSBH76082622 = UARKAXVusJleXttldSBH17333359;     UARKAXVusJleXttldSBH17333359 = UARKAXVusJleXttldSBH55846040;     UARKAXVusJleXttldSBH55846040 = UARKAXVusJleXttldSBH13673956;     UARKAXVusJleXttldSBH13673956 = UARKAXVusJleXttldSBH24564516;     UARKAXVusJleXttldSBH24564516 = UARKAXVusJleXttldSBH57930605;     UARKAXVusJleXttldSBH57930605 = UARKAXVusJleXttldSBH48841374;     UARKAXVusJleXttldSBH48841374 = UARKAXVusJleXttldSBH53427610;     UARKAXVusJleXttldSBH53427610 = UARKAXVusJleXttldSBH31785879;     UARKAXVusJleXttldSBH31785879 = UARKAXVusJleXttldSBH23020377;     UARKAXVusJleXttldSBH23020377 = UARKAXVusJleXttldSBH72820796;     UARKAXVusJleXttldSBH72820796 = UARKAXVusJleXttldSBH88303989;     UARKAXVusJleXttldSBH88303989 = UARKAXVusJleXttldSBH69171395;     UARKAXVusJleXttldSBH69171395 = UARKAXVusJleXttldSBH27232850;     UARKAXVusJleXttldSBH27232850 = UARKAXVusJleXttldSBH50344455;     UARKAXVusJleXttldSBH50344455 = UARKAXVusJleXttldSBH45569480;     UARKAXVusJleXttldSBH45569480 = UARKAXVusJleXttldSBH58559145;     UARKAXVusJleXttldSBH58559145 = UARKAXVusJleXttldSBH96150796;     UARKAXVusJleXttldSBH96150796 = UARKAXVusJleXttldSBH90910455;     UARKAXVusJleXttldSBH90910455 = UARKAXVusJleXttldSBH97953586;     UARKAXVusJleXttldSBH97953586 = UARKAXVusJleXttldSBH72138363;     UARKAXVusJleXttldSBH72138363 = UARKAXVusJleXttldSBH96241162;     UARKAXVusJleXttldSBH96241162 = UARKAXVusJleXttldSBH60662966;     UARKAXVusJleXttldSBH60662966 = UARKAXVusJleXttldSBH4863029;     UARKAXVusJleXttldSBH4863029 = UARKAXVusJleXttldSBH28411640;     UARKAXVusJleXttldSBH28411640 = UARKAXVusJleXttldSBH68712604;     UARKAXVusJleXttldSBH68712604 = UARKAXVusJleXttldSBH95318749;     UARKAXVusJleXttldSBH95318749 = UARKAXVusJleXttldSBH8024588;     UARKAXVusJleXttldSBH8024588 = UARKAXVusJleXttldSBH45770455;     UARKAXVusJleXttldSBH45770455 = UARKAXVusJleXttldSBH32637141;     UARKAXVusJleXttldSBH32637141 = UARKAXVusJleXttldSBH68965143;     UARKAXVusJleXttldSBH68965143 = UARKAXVusJleXttldSBH3119139;     UARKAXVusJleXttldSBH3119139 = UARKAXVusJleXttldSBH59985113;     UARKAXVusJleXttldSBH59985113 = UARKAXVusJleXttldSBH8510741;     UARKAXVusJleXttldSBH8510741 = UARKAXVusJleXttldSBH65278276;     UARKAXVusJleXttldSBH65278276 = UARKAXVusJleXttldSBH627308;     UARKAXVusJleXttldSBH627308 = UARKAXVusJleXttldSBH49683433;     UARKAXVusJleXttldSBH49683433 = UARKAXVusJleXttldSBH62499447;     UARKAXVusJleXttldSBH62499447 = UARKAXVusJleXttldSBH47868902;     UARKAXVusJleXttldSBH47868902 = UARKAXVusJleXttldSBH93590079;     UARKAXVusJleXttldSBH93590079 = UARKAXVusJleXttldSBH18627573;     UARKAXVusJleXttldSBH18627573 = UARKAXVusJleXttldSBH74214808;     UARKAXVusJleXttldSBH74214808 = UARKAXVusJleXttldSBH50725555;     UARKAXVusJleXttldSBH50725555 = UARKAXVusJleXttldSBH28625940;     UARKAXVusJleXttldSBH28625940 = UARKAXVusJleXttldSBH54660366;     UARKAXVusJleXttldSBH54660366 = UARKAXVusJleXttldSBH57854663;     UARKAXVusJleXttldSBH57854663 = UARKAXVusJleXttldSBH19642466;     UARKAXVusJleXttldSBH19642466 = UARKAXVusJleXttldSBH94469012;     UARKAXVusJleXttldSBH94469012 = UARKAXVusJleXttldSBH40219056;     UARKAXVusJleXttldSBH40219056 = UARKAXVusJleXttldSBH82090980;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MGuTeDjVpLxVFDyVuuFa94902993() {     float pEFmYJsrLAmsmOOFVOdq82084509 = 31328605;    float pEFmYJsrLAmsmOOFVOdq13523535 = -610667103;    float pEFmYJsrLAmsmOOFVOdq48742686 = -87020861;    float pEFmYJsrLAmsmOOFVOdq12015837 = -862584498;    float pEFmYJsrLAmsmOOFVOdq71088449 = -353150429;    float pEFmYJsrLAmsmOOFVOdq2042454 = -687365408;    float pEFmYJsrLAmsmOOFVOdq31575574 = -926280415;    float pEFmYJsrLAmsmOOFVOdq16839003 = -80803003;    float pEFmYJsrLAmsmOOFVOdq33657699 = -152832938;    float pEFmYJsrLAmsmOOFVOdq65233262 = -798921392;    float pEFmYJsrLAmsmOOFVOdq88625727 = -345101047;    float pEFmYJsrLAmsmOOFVOdq65390630 = -46747948;    float pEFmYJsrLAmsmOOFVOdq66585462 = -572673827;    float pEFmYJsrLAmsmOOFVOdq22428434 = -160924352;    float pEFmYJsrLAmsmOOFVOdq87603248 = -972076735;    float pEFmYJsrLAmsmOOFVOdq39987080 = -738188084;    float pEFmYJsrLAmsmOOFVOdq55633540 = -52896326;    float pEFmYJsrLAmsmOOFVOdq1753130 = 14436011;    float pEFmYJsrLAmsmOOFVOdq32737553 = -950715068;    float pEFmYJsrLAmsmOOFVOdq4417954 = -930524138;    float pEFmYJsrLAmsmOOFVOdq54239717 = -163577753;    float pEFmYJsrLAmsmOOFVOdq35822731 = -455245364;    float pEFmYJsrLAmsmOOFVOdq53899009 = -402301175;    float pEFmYJsrLAmsmOOFVOdq42238208 = -349819686;    float pEFmYJsrLAmsmOOFVOdq1951057 = -356169719;    float pEFmYJsrLAmsmOOFVOdq47036546 = -35619774;    float pEFmYJsrLAmsmOOFVOdq14438226 = -223640607;    float pEFmYJsrLAmsmOOFVOdq25610967 = -8295302;    float pEFmYJsrLAmsmOOFVOdq71999410 = -193663019;    float pEFmYJsrLAmsmOOFVOdq57836010 = -123202084;    float pEFmYJsrLAmsmOOFVOdq74773577 = -610966498;    float pEFmYJsrLAmsmOOFVOdq2684524 = -852050238;    float pEFmYJsrLAmsmOOFVOdq90981896 = -92597113;    float pEFmYJsrLAmsmOOFVOdq76456241 = -130820885;    float pEFmYJsrLAmsmOOFVOdq54791456 = -559691668;    float pEFmYJsrLAmsmOOFVOdq94348874 = -52685224;    float pEFmYJsrLAmsmOOFVOdq79410314 = -111144554;    float pEFmYJsrLAmsmOOFVOdq97560097 = -918908492;    float pEFmYJsrLAmsmOOFVOdq17825030 = 31949940;    float pEFmYJsrLAmsmOOFVOdq23519209 = -527880946;    float pEFmYJsrLAmsmOOFVOdq15336830 = -148138912;    float pEFmYJsrLAmsmOOFVOdq31856314 = -710266013;    float pEFmYJsrLAmsmOOFVOdq49097221 = -473425626;    float pEFmYJsrLAmsmOOFVOdq80933761 = -807356001;    float pEFmYJsrLAmsmOOFVOdq74581563 = -461072506;    float pEFmYJsrLAmsmOOFVOdq68165880 = -471284217;    float pEFmYJsrLAmsmOOFVOdq25523121 = -625361976;    float pEFmYJsrLAmsmOOFVOdq50635082 = -876627169;    float pEFmYJsrLAmsmOOFVOdq5746693 = 3581126;    float pEFmYJsrLAmsmOOFVOdq19381273 = -866747118;    float pEFmYJsrLAmsmOOFVOdq86494769 = -744009228;    float pEFmYJsrLAmsmOOFVOdq90629879 = 42747917;    float pEFmYJsrLAmsmOOFVOdq92781707 = -243254085;    float pEFmYJsrLAmsmOOFVOdq66096311 = -74261024;    float pEFmYJsrLAmsmOOFVOdq40663276 = -861378279;    float pEFmYJsrLAmsmOOFVOdq46261778 = -513426031;    float pEFmYJsrLAmsmOOFVOdq59624525 = -108365929;    float pEFmYJsrLAmsmOOFVOdq6504478 = -737201176;    float pEFmYJsrLAmsmOOFVOdq10064781 = -406414780;    float pEFmYJsrLAmsmOOFVOdq24051904 = -217530656;    float pEFmYJsrLAmsmOOFVOdq87604227 = -363724802;    float pEFmYJsrLAmsmOOFVOdq5964608 = -817985113;    float pEFmYJsrLAmsmOOFVOdq44839593 = -887139984;    float pEFmYJsrLAmsmOOFVOdq75821689 = 70369146;    float pEFmYJsrLAmsmOOFVOdq90459685 = -87954895;    float pEFmYJsrLAmsmOOFVOdq85941203 = -493050809;    float pEFmYJsrLAmsmOOFVOdq74408734 = -954150836;    float pEFmYJsrLAmsmOOFVOdq90129221 = -341852942;    float pEFmYJsrLAmsmOOFVOdq67636978 = -601232685;    float pEFmYJsrLAmsmOOFVOdq93254374 = -819391511;    float pEFmYJsrLAmsmOOFVOdq60576766 = -527043531;    float pEFmYJsrLAmsmOOFVOdq58073443 = -133987835;    float pEFmYJsrLAmsmOOFVOdq83928099 = 82486071;    float pEFmYJsrLAmsmOOFVOdq9218344 = -322834123;    float pEFmYJsrLAmsmOOFVOdq89081123 = -682385227;    float pEFmYJsrLAmsmOOFVOdq22383403 = -453311741;    float pEFmYJsrLAmsmOOFVOdq86725510 = -981819738;    float pEFmYJsrLAmsmOOFVOdq72965248 = -594945175;    float pEFmYJsrLAmsmOOFVOdq67656644 = -888747181;    float pEFmYJsrLAmsmOOFVOdq33785176 = -884885502;    float pEFmYJsrLAmsmOOFVOdq21513425 = -410257798;    float pEFmYJsrLAmsmOOFVOdq63803144 = -347013438;    float pEFmYJsrLAmsmOOFVOdq19864274 = 88123571;    float pEFmYJsrLAmsmOOFVOdq52618137 = -326915901;    float pEFmYJsrLAmsmOOFVOdq71341240 = -379192857;    float pEFmYJsrLAmsmOOFVOdq84143697 = -553714415;    float pEFmYJsrLAmsmOOFVOdq9902817 = -508796154;    float pEFmYJsrLAmsmOOFVOdq24885585 = 81663911;    float pEFmYJsrLAmsmOOFVOdq35792965 = -269442606;    float pEFmYJsrLAmsmOOFVOdq8529678 = 53734363;    float pEFmYJsrLAmsmOOFVOdq34724349 = -944319296;    float pEFmYJsrLAmsmOOFVOdq72905836 = -373943378;    float pEFmYJsrLAmsmOOFVOdq87495316 = -412493712;    float pEFmYJsrLAmsmOOFVOdq93773126 = -750519405;    float pEFmYJsrLAmsmOOFVOdq35914982 = -64156145;    float pEFmYJsrLAmsmOOFVOdq9372223 = -330153799;    float pEFmYJsrLAmsmOOFVOdq87016720 = -823126029;    float pEFmYJsrLAmsmOOFVOdq73275531 = -443794773;    float pEFmYJsrLAmsmOOFVOdq90474076 = -619401106;    float pEFmYJsrLAmsmOOFVOdq88640360 = 31328605;     pEFmYJsrLAmsmOOFVOdq82084509 = pEFmYJsrLAmsmOOFVOdq13523535;     pEFmYJsrLAmsmOOFVOdq13523535 = pEFmYJsrLAmsmOOFVOdq48742686;     pEFmYJsrLAmsmOOFVOdq48742686 = pEFmYJsrLAmsmOOFVOdq12015837;     pEFmYJsrLAmsmOOFVOdq12015837 = pEFmYJsrLAmsmOOFVOdq71088449;     pEFmYJsrLAmsmOOFVOdq71088449 = pEFmYJsrLAmsmOOFVOdq2042454;     pEFmYJsrLAmsmOOFVOdq2042454 = pEFmYJsrLAmsmOOFVOdq31575574;     pEFmYJsrLAmsmOOFVOdq31575574 = pEFmYJsrLAmsmOOFVOdq16839003;     pEFmYJsrLAmsmOOFVOdq16839003 = pEFmYJsrLAmsmOOFVOdq33657699;     pEFmYJsrLAmsmOOFVOdq33657699 = pEFmYJsrLAmsmOOFVOdq65233262;     pEFmYJsrLAmsmOOFVOdq65233262 = pEFmYJsrLAmsmOOFVOdq88625727;     pEFmYJsrLAmsmOOFVOdq88625727 = pEFmYJsrLAmsmOOFVOdq65390630;     pEFmYJsrLAmsmOOFVOdq65390630 = pEFmYJsrLAmsmOOFVOdq66585462;     pEFmYJsrLAmsmOOFVOdq66585462 = pEFmYJsrLAmsmOOFVOdq22428434;     pEFmYJsrLAmsmOOFVOdq22428434 = pEFmYJsrLAmsmOOFVOdq87603248;     pEFmYJsrLAmsmOOFVOdq87603248 = pEFmYJsrLAmsmOOFVOdq39987080;     pEFmYJsrLAmsmOOFVOdq39987080 = pEFmYJsrLAmsmOOFVOdq55633540;     pEFmYJsrLAmsmOOFVOdq55633540 = pEFmYJsrLAmsmOOFVOdq1753130;     pEFmYJsrLAmsmOOFVOdq1753130 = pEFmYJsrLAmsmOOFVOdq32737553;     pEFmYJsrLAmsmOOFVOdq32737553 = pEFmYJsrLAmsmOOFVOdq4417954;     pEFmYJsrLAmsmOOFVOdq4417954 = pEFmYJsrLAmsmOOFVOdq54239717;     pEFmYJsrLAmsmOOFVOdq54239717 = pEFmYJsrLAmsmOOFVOdq35822731;     pEFmYJsrLAmsmOOFVOdq35822731 = pEFmYJsrLAmsmOOFVOdq53899009;     pEFmYJsrLAmsmOOFVOdq53899009 = pEFmYJsrLAmsmOOFVOdq42238208;     pEFmYJsrLAmsmOOFVOdq42238208 = pEFmYJsrLAmsmOOFVOdq1951057;     pEFmYJsrLAmsmOOFVOdq1951057 = pEFmYJsrLAmsmOOFVOdq47036546;     pEFmYJsrLAmsmOOFVOdq47036546 = pEFmYJsrLAmsmOOFVOdq14438226;     pEFmYJsrLAmsmOOFVOdq14438226 = pEFmYJsrLAmsmOOFVOdq25610967;     pEFmYJsrLAmsmOOFVOdq25610967 = pEFmYJsrLAmsmOOFVOdq71999410;     pEFmYJsrLAmsmOOFVOdq71999410 = pEFmYJsrLAmsmOOFVOdq57836010;     pEFmYJsrLAmsmOOFVOdq57836010 = pEFmYJsrLAmsmOOFVOdq74773577;     pEFmYJsrLAmsmOOFVOdq74773577 = pEFmYJsrLAmsmOOFVOdq2684524;     pEFmYJsrLAmsmOOFVOdq2684524 = pEFmYJsrLAmsmOOFVOdq90981896;     pEFmYJsrLAmsmOOFVOdq90981896 = pEFmYJsrLAmsmOOFVOdq76456241;     pEFmYJsrLAmsmOOFVOdq76456241 = pEFmYJsrLAmsmOOFVOdq54791456;     pEFmYJsrLAmsmOOFVOdq54791456 = pEFmYJsrLAmsmOOFVOdq94348874;     pEFmYJsrLAmsmOOFVOdq94348874 = pEFmYJsrLAmsmOOFVOdq79410314;     pEFmYJsrLAmsmOOFVOdq79410314 = pEFmYJsrLAmsmOOFVOdq97560097;     pEFmYJsrLAmsmOOFVOdq97560097 = pEFmYJsrLAmsmOOFVOdq17825030;     pEFmYJsrLAmsmOOFVOdq17825030 = pEFmYJsrLAmsmOOFVOdq23519209;     pEFmYJsrLAmsmOOFVOdq23519209 = pEFmYJsrLAmsmOOFVOdq15336830;     pEFmYJsrLAmsmOOFVOdq15336830 = pEFmYJsrLAmsmOOFVOdq31856314;     pEFmYJsrLAmsmOOFVOdq31856314 = pEFmYJsrLAmsmOOFVOdq49097221;     pEFmYJsrLAmsmOOFVOdq49097221 = pEFmYJsrLAmsmOOFVOdq80933761;     pEFmYJsrLAmsmOOFVOdq80933761 = pEFmYJsrLAmsmOOFVOdq74581563;     pEFmYJsrLAmsmOOFVOdq74581563 = pEFmYJsrLAmsmOOFVOdq68165880;     pEFmYJsrLAmsmOOFVOdq68165880 = pEFmYJsrLAmsmOOFVOdq25523121;     pEFmYJsrLAmsmOOFVOdq25523121 = pEFmYJsrLAmsmOOFVOdq50635082;     pEFmYJsrLAmsmOOFVOdq50635082 = pEFmYJsrLAmsmOOFVOdq5746693;     pEFmYJsrLAmsmOOFVOdq5746693 = pEFmYJsrLAmsmOOFVOdq19381273;     pEFmYJsrLAmsmOOFVOdq19381273 = pEFmYJsrLAmsmOOFVOdq86494769;     pEFmYJsrLAmsmOOFVOdq86494769 = pEFmYJsrLAmsmOOFVOdq90629879;     pEFmYJsrLAmsmOOFVOdq90629879 = pEFmYJsrLAmsmOOFVOdq92781707;     pEFmYJsrLAmsmOOFVOdq92781707 = pEFmYJsrLAmsmOOFVOdq66096311;     pEFmYJsrLAmsmOOFVOdq66096311 = pEFmYJsrLAmsmOOFVOdq40663276;     pEFmYJsrLAmsmOOFVOdq40663276 = pEFmYJsrLAmsmOOFVOdq46261778;     pEFmYJsrLAmsmOOFVOdq46261778 = pEFmYJsrLAmsmOOFVOdq59624525;     pEFmYJsrLAmsmOOFVOdq59624525 = pEFmYJsrLAmsmOOFVOdq6504478;     pEFmYJsrLAmsmOOFVOdq6504478 = pEFmYJsrLAmsmOOFVOdq10064781;     pEFmYJsrLAmsmOOFVOdq10064781 = pEFmYJsrLAmsmOOFVOdq24051904;     pEFmYJsrLAmsmOOFVOdq24051904 = pEFmYJsrLAmsmOOFVOdq87604227;     pEFmYJsrLAmsmOOFVOdq87604227 = pEFmYJsrLAmsmOOFVOdq5964608;     pEFmYJsrLAmsmOOFVOdq5964608 = pEFmYJsrLAmsmOOFVOdq44839593;     pEFmYJsrLAmsmOOFVOdq44839593 = pEFmYJsrLAmsmOOFVOdq75821689;     pEFmYJsrLAmsmOOFVOdq75821689 = pEFmYJsrLAmsmOOFVOdq90459685;     pEFmYJsrLAmsmOOFVOdq90459685 = pEFmYJsrLAmsmOOFVOdq85941203;     pEFmYJsrLAmsmOOFVOdq85941203 = pEFmYJsrLAmsmOOFVOdq74408734;     pEFmYJsrLAmsmOOFVOdq74408734 = pEFmYJsrLAmsmOOFVOdq90129221;     pEFmYJsrLAmsmOOFVOdq90129221 = pEFmYJsrLAmsmOOFVOdq67636978;     pEFmYJsrLAmsmOOFVOdq67636978 = pEFmYJsrLAmsmOOFVOdq93254374;     pEFmYJsrLAmsmOOFVOdq93254374 = pEFmYJsrLAmsmOOFVOdq60576766;     pEFmYJsrLAmsmOOFVOdq60576766 = pEFmYJsrLAmsmOOFVOdq58073443;     pEFmYJsrLAmsmOOFVOdq58073443 = pEFmYJsrLAmsmOOFVOdq83928099;     pEFmYJsrLAmsmOOFVOdq83928099 = pEFmYJsrLAmsmOOFVOdq9218344;     pEFmYJsrLAmsmOOFVOdq9218344 = pEFmYJsrLAmsmOOFVOdq89081123;     pEFmYJsrLAmsmOOFVOdq89081123 = pEFmYJsrLAmsmOOFVOdq22383403;     pEFmYJsrLAmsmOOFVOdq22383403 = pEFmYJsrLAmsmOOFVOdq86725510;     pEFmYJsrLAmsmOOFVOdq86725510 = pEFmYJsrLAmsmOOFVOdq72965248;     pEFmYJsrLAmsmOOFVOdq72965248 = pEFmYJsrLAmsmOOFVOdq67656644;     pEFmYJsrLAmsmOOFVOdq67656644 = pEFmYJsrLAmsmOOFVOdq33785176;     pEFmYJsrLAmsmOOFVOdq33785176 = pEFmYJsrLAmsmOOFVOdq21513425;     pEFmYJsrLAmsmOOFVOdq21513425 = pEFmYJsrLAmsmOOFVOdq63803144;     pEFmYJsrLAmsmOOFVOdq63803144 = pEFmYJsrLAmsmOOFVOdq19864274;     pEFmYJsrLAmsmOOFVOdq19864274 = pEFmYJsrLAmsmOOFVOdq52618137;     pEFmYJsrLAmsmOOFVOdq52618137 = pEFmYJsrLAmsmOOFVOdq71341240;     pEFmYJsrLAmsmOOFVOdq71341240 = pEFmYJsrLAmsmOOFVOdq84143697;     pEFmYJsrLAmsmOOFVOdq84143697 = pEFmYJsrLAmsmOOFVOdq9902817;     pEFmYJsrLAmsmOOFVOdq9902817 = pEFmYJsrLAmsmOOFVOdq24885585;     pEFmYJsrLAmsmOOFVOdq24885585 = pEFmYJsrLAmsmOOFVOdq35792965;     pEFmYJsrLAmsmOOFVOdq35792965 = pEFmYJsrLAmsmOOFVOdq8529678;     pEFmYJsrLAmsmOOFVOdq8529678 = pEFmYJsrLAmsmOOFVOdq34724349;     pEFmYJsrLAmsmOOFVOdq34724349 = pEFmYJsrLAmsmOOFVOdq72905836;     pEFmYJsrLAmsmOOFVOdq72905836 = pEFmYJsrLAmsmOOFVOdq87495316;     pEFmYJsrLAmsmOOFVOdq87495316 = pEFmYJsrLAmsmOOFVOdq93773126;     pEFmYJsrLAmsmOOFVOdq93773126 = pEFmYJsrLAmsmOOFVOdq35914982;     pEFmYJsrLAmsmOOFVOdq35914982 = pEFmYJsrLAmsmOOFVOdq9372223;     pEFmYJsrLAmsmOOFVOdq9372223 = pEFmYJsrLAmsmOOFVOdq87016720;     pEFmYJsrLAmsmOOFVOdq87016720 = pEFmYJsrLAmsmOOFVOdq73275531;     pEFmYJsrLAmsmOOFVOdq73275531 = pEFmYJsrLAmsmOOFVOdq90474076;     pEFmYJsrLAmsmOOFVOdq90474076 = pEFmYJsrLAmsmOOFVOdq88640360;     pEFmYJsrLAmsmOOFVOdq88640360 = pEFmYJsrLAmsmOOFVOdq82084509;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BzbeGodgedNkgzNEFwOL49319206() {     float dJNnsfsNDoqWhiBDRqmM52736176 = -586820525;    float dJNnsfsNDoqWhiBDRqmM82197052 = -482310403;    float dJNnsfsNDoqWhiBDRqmM28711605 = -529315321;    float dJNnsfsNDoqWhiBDRqmM51947123 = 47277112;    float dJNnsfsNDoqWhiBDRqmM92538771 = -136556777;    float dJNnsfsNDoqWhiBDRqmM92042216 = -388946560;    float dJNnsfsNDoqWhiBDRqmM49954470 = -925056125;    float dJNnsfsNDoqWhiBDRqmM73681383 = -427620092;    float dJNnsfsNDoqWhiBDRqmM79522753 = -246180267;    float dJNnsfsNDoqWhiBDRqmM88199723 = -16743;    float dJNnsfsNDoqWhiBDRqmM30874888 = -380002976;    float dJNnsfsNDoqWhiBDRqmM10937202 = -362511741;    float dJNnsfsNDoqWhiBDRqmM46412849 = -482210885;    float dJNnsfsNDoqWhiBDRqmM6275468 = 38047438;    float dJNnsfsNDoqWhiBDRqmM70323268 = -21826738;    float dJNnsfsNDoqWhiBDRqmM72515720 = -308171671;    float dJNnsfsNDoqWhiBDRqmM49486367 = 37918544;    float dJNnsfsNDoqWhiBDRqmM22046624 = 25269815;    float dJNnsfsNDoqWhiBDRqmM14445335 = -557903011;    float dJNnsfsNDoqWhiBDRqmM63204663 = -372890443;    float dJNnsfsNDoqWhiBDRqmM9375472 = -72256894;    float dJNnsfsNDoqWhiBDRqmM97218495 = -786530670;    float dJNnsfsNDoqWhiBDRqmM55842818 = -970144319;    float dJNnsfsNDoqWhiBDRqmM76087744 = 57779408;    float dJNnsfsNDoqWhiBDRqmM66075444 = -32074406;    float dJNnsfsNDoqWhiBDRqmM77634126 = -133904219;    float dJNnsfsNDoqWhiBDRqmM22699012 = -618818382;    float dJNnsfsNDoqWhiBDRqmM33393192 = -513419070;    float dJNnsfsNDoqWhiBDRqmM4141898 = -488021236;    float dJNnsfsNDoqWhiBDRqmM97008106 = -681597155;    float dJNnsfsNDoqWhiBDRqmM81845574 = -359400183;    float dJNnsfsNDoqWhiBDRqmM47195985 = -80347401;    float dJNnsfsNDoqWhiBDRqmM73927257 = -39558721;    float dJNnsfsNDoqWhiBDRqmM41821216 = -794350569;    float dJNnsfsNDoqWhiBDRqmM32815692 = -406638447;    float dJNnsfsNDoqWhiBDRqmM54342133 = -131911772;    float dJNnsfsNDoqWhiBDRqmM23201800 = -299737761;    float dJNnsfsNDoqWhiBDRqmM50352586 = -703021599;    float dJNnsfsNDoqWhiBDRqmM96358319 = 76221303;    float dJNnsfsNDoqWhiBDRqmM18622508 = -45489309;    float dJNnsfsNDoqWhiBDRqmM75410114 = -532157790;    float dJNnsfsNDoqWhiBDRqmM60339305 = -479655082;    float dJNnsfsNDoqWhiBDRqmM98064180 = -460179679;    float dJNnsfsNDoqWhiBDRqmM87442061 = -879330972;    float dJNnsfsNDoqWhiBDRqmM70535622 = -769718900;    float dJNnsfsNDoqWhiBDRqmM74571714 = -229621372;    float dJNnsfsNDoqWhiBDRqmM51654324 = -741431775;    float dJNnsfsNDoqWhiBDRqmM16249292 = -228757464;    float dJNnsfsNDoqWhiBDRqmM11188145 = 17817157;    float dJNnsfsNDoqWhiBDRqmM27808759 = -49755997;    float dJNnsfsNDoqWhiBDRqmM54684094 = -290581349;    float dJNnsfsNDoqWhiBDRqmM32501384 = 73528195;    float dJNnsfsNDoqWhiBDRqmM47857364 = -171052127;    float dJNnsfsNDoqWhiBDRqmM56323616 = -735142521;    float dJNnsfsNDoqWhiBDRqmM39943735 = -972319194;    float dJNnsfsNDoqWhiBDRqmM55517681 = -800289856;    float dJNnsfsNDoqWhiBDRqmM26354234 = -512166084;    float dJNnsfsNDoqWhiBDRqmM52623860 = -487094730;    float dJNnsfsNDoqWhiBDRqmM85871679 = -920648482;    float dJNnsfsNDoqWhiBDRqmM14904645 = 97347442;    float dJNnsfsNDoqWhiBDRqmM69343204 = -770128178;    float dJNnsfsNDoqWhiBDRqmM16561278 = -311637056;    float dJNnsfsNDoqWhiBDRqmM69539486 = -939598857;    float dJNnsfsNDoqWhiBDRqmM82514647 = -564583112;    float dJNnsfsNDoqWhiBDRqmM6354150 = -640616560;    float dJNnsfsNDoqWhiBDRqmM83678903 = -199655576;    float dJNnsfsNDoqWhiBDRqmM37009944 = -222953020;    float dJNnsfsNDoqWhiBDRqmM4591634 = -687860316;    float dJNnsfsNDoqWhiBDRqmM73459775 = -555314115;    float dJNnsfsNDoqWhiBDRqmM15981135 = -889914966;    float dJNnsfsNDoqWhiBDRqmM49313920 = 91566090;    float dJNnsfsNDoqWhiBDRqmM99133781 = -259059857;    float dJNnsfsNDoqWhiBDRqmM25688305 = 49048511;    float dJNnsfsNDoqWhiBDRqmM95822827 = -412413702;    float dJNnsfsNDoqWhiBDRqmM87794549 = -840732654;    float dJNnsfsNDoqWhiBDRqmM49036166 = -592601812;    float dJNnsfsNDoqWhiBDRqmM99154314 = -226350992;    float dJNnsfsNDoqWhiBDRqmM68400756 = 9186652;    float dJNnsfsNDoqWhiBDRqmM5552123 = -172501692;    float dJNnsfsNDoqWhiBDRqmM91503729 = -802453035;    float dJNnsfsNDoqWhiBDRqmM25979803 = -392472444;    float dJNnsfsNDoqWhiBDRqmM6449720 = -290060918;    float dJNnsfsNDoqWhiBDRqmM22205047 = -431236227;    float dJNnsfsNDoqWhiBDRqmM76333139 = -338265239;    float dJNnsfsNDoqWhiBDRqmM42324012 = -291015806;    float dJNnsfsNDoqWhiBDRqmM49344190 = -332928379;    float dJNnsfsNDoqWhiBDRqmM99338620 = -909295275;    float dJNnsfsNDoqWhiBDRqmM17603642 = -304416201;    float dJNnsfsNDoqWhiBDRqmM1877481 = -822031376;    float dJNnsfsNDoqWhiBDRqmM77298011 = -606348592;    float dJNnsfsNDoqWhiBDRqmM27987899 = -619745689;    float dJNnsfsNDoqWhiBDRqmM70577940 = -812643032;    float dJNnsfsNDoqWhiBDRqmM64480907 = -782373118;    float dJNnsfsNDoqWhiBDRqmM81453674 = 78873861;    float dJNnsfsNDoqWhiBDRqmM49279303 = -275361131;    float dJNnsfsNDoqWhiBDRqmM58848836 = -120520735;    float dJNnsfsNDoqWhiBDRqmM90799819 = -540056226;    float dJNnsfsNDoqWhiBDRqmM15549533 = -895596567;    float dJNnsfsNDoqWhiBDRqmM81087912 = -138714413;    float dJNnsfsNDoqWhiBDRqmM86856719 = -586820525;     dJNnsfsNDoqWhiBDRqmM52736176 = dJNnsfsNDoqWhiBDRqmM82197052;     dJNnsfsNDoqWhiBDRqmM82197052 = dJNnsfsNDoqWhiBDRqmM28711605;     dJNnsfsNDoqWhiBDRqmM28711605 = dJNnsfsNDoqWhiBDRqmM51947123;     dJNnsfsNDoqWhiBDRqmM51947123 = dJNnsfsNDoqWhiBDRqmM92538771;     dJNnsfsNDoqWhiBDRqmM92538771 = dJNnsfsNDoqWhiBDRqmM92042216;     dJNnsfsNDoqWhiBDRqmM92042216 = dJNnsfsNDoqWhiBDRqmM49954470;     dJNnsfsNDoqWhiBDRqmM49954470 = dJNnsfsNDoqWhiBDRqmM73681383;     dJNnsfsNDoqWhiBDRqmM73681383 = dJNnsfsNDoqWhiBDRqmM79522753;     dJNnsfsNDoqWhiBDRqmM79522753 = dJNnsfsNDoqWhiBDRqmM88199723;     dJNnsfsNDoqWhiBDRqmM88199723 = dJNnsfsNDoqWhiBDRqmM30874888;     dJNnsfsNDoqWhiBDRqmM30874888 = dJNnsfsNDoqWhiBDRqmM10937202;     dJNnsfsNDoqWhiBDRqmM10937202 = dJNnsfsNDoqWhiBDRqmM46412849;     dJNnsfsNDoqWhiBDRqmM46412849 = dJNnsfsNDoqWhiBDRqmM6275468;     dJNnsfsNDoqWhiBDRqmM6275468 = dJNnsfsNDoqWhiBDRqmM70323268;     dJNnsfsNDoqWhiBDRqmM70323268 = dJNnsfsNDoqWhiBDRqmM72515720;     dJNnsfsNDoqWhiBDRqmM72515720 = dJNnsfsNDoqWhiBDRqmM49486367;     dJNnsfsNDoqWhiBDRqmM49486367 = dJNnsfsNDoqWhiBDRqmM22046624;     dJNnsfsNDoqWhiBDRqmM22046624 = dJNnsfsNDoqWhiBDRqmM14445335;     dJNnsfsNDoqWhiBDRqmM14445335 = dJNnsfsNDoqWhiBDRqmM63204663;     dJNnsfsNDoqWhiBDRqmM63204663 = dJNnsfsNDoqWhiBDRqmM9375472;     dJNnsfsNDoqWhiBDRqmM9375472 = dJNnsfsNDoqWhiBDRqmM97218495;     dJNnsfsNDoqWhiBDRqmM97218495 = dJNnsfsNDoqWhiBDRqmM55842818;     dJNnsfsNDoqWhiBDRqmM55842818 = dJNnsfsNDoqWhiBDRqmM76087744;     dJNnsfsNDoqWhiBDRqmM76087744 = dJNnsfsNDoqWhiBDRqmM66075444;     dJNnsfsNDoqWhiBDRqmM66075444 = dJNnsfsNDoqWhiBDRqmM77634126;     dJNnsfsNDoqWhiBDRqmM77634126 = dJNnsfsNDoqWhiBDRqmM22699012;     dJNnsfsNDoqWhiBDRqmM22699012 = dJNnsfsNDoqWhiBDRqmM33393192;     dJNnsfsNDoqWhiBDRqmM33393192 = dJNnsfsNDoqWhiBDRqmM4141898;     dJNnsfsNDoqWhiBDRqmM4141898 = dJNnsfsNDoqWhiBDRqmM97008106;     dJNnsfsNDoqWhiBDRqmM97008106 = dJNnsfsNDoqWhiBDRqmM81845574;     dJNnsfsNDoqWhiBDRqmM81845574 = dJNnsfsNDoqWhiBDRqmM47195985;     dJNnsfsNDoqWhiBDRqmM47195985 = dJNnsfsNDoqWhiBDRqmM73927257;     dJNnsfsNDoqWhiBDRqmM73927257 = dJNnsfsNDoqWhiBDRqmM41821216;     dJNnsfsNDoqWhiBDRqmM41821216 = dJNnsfsNDoqWhiBDRqmM32815692;     dJNnsfsNDoqWhiBDRqmM32815692 = dJNnsfsNDoqWhiBDRqmM54342133;     dJNnsfsNDoqWhiBDRqmM54342133 = dJNnsfsNDoqWhiBDRqmM23201800;     dJNnsfsNDoqWhiBDRqmM23201800 = dJNnsfsNDoqWhiBDRqmM50352586;     dJNnsfsNDoqWhiBDRqmM50352586 = dJNnsfsNDoqWhiBDRqmM96358319;     dJNnsfsNDoqWhiBDRqmM96358319 = dJNnsfsNDoqWhiBDRqmM18622508;     dJNnsfsNDoqWhiBDRqmM18622508 = dJNnsfsNDoqWhiBDRqmM75410114;     dJNnsfsNDoqWhiBDRqmM75410114 = dJNnsfsNDoqWhiBDRqmM60339305;     dJNnsfsNDoqWhiBDRqmM60339305 = dJNnsfsNDoqWhiBDRqmM98064180;     dJNnsfsNDoqWhiBDRqmM98064180 = dJNnsfsNDoqWhiBDRqmM87442061;     dJNnsfsNDoqWhiBDRqmM87442061 = dJNnsfsNDoqWhiBDRqmM70535622;     dJNnsfsNDoqWhiBDRqmM70535622 = dJNnsfsNDoqWhiBDRqmM74571714;     dJNnsfsNDoqWhiBDRqmM74571714 = dJNnsfsNDoqWhiBDRqmM51654324;     dJNnsfsNDoqWhiBDRqmM51654324 = dJNnsfsNDoqWhiBDRqmM16249292;     dJNnsfsNDoqWhiBDRqmM16249292 = dJNnsfsNDoqWhiBDRqmM11188145;     dJNnsfsNDoqWhiBDRqmM11188145 = dJNnsfsNDoqWhiBDRqmM27808759;     dJNnsfsNDoqWhiBDRqmM27808759 = dJNnsfsNDoqWhiBDRqmM54684094;     dJNnsfsNDoqWhiBDRqmM54684094 = dJNnsfsNDoqWhiBDRqmM32501384;     dJNnsfsNDoqWhiBDRqmM32501384 = dJNnsfsNDoqWhiBDRqmM47857364;     dJNnsfsNDoqWhiBDRqmM47857364 = dJNnsfsNDoqWhiBDRqmM56323616;     dJNnsfsNDoqWhiBDRqmM56323616 = dJNnsfsNDoqWhiBDRqmM39943735;     dJNnsfsNDoqWhiBDRqmM39943735 = dJNnsfsNDoqWhiBDRqmM55517681;     dJNnsfsNDoqWhiBDRqmM55517681 = dJNnsfsNDoqWhiBDRqmM26354234;     dJNnsfsNDoqWhiBDRqmM26354234 = dJNnsfsNDoqWhiBDRqmM52623860;     dJNnsfsNDoqWhiBDRqmM52623860 = dJNnsfsNDoqWhiBDRqmM85871679;     dJNnsfsNDoqWhiBDRqmM85871679 = dJNnsfsNDoqWhiBDRqmM14904645;     dJNnsfsNDoqWhiBDRqmM14904645 = dJNnsfsNDoqWhiBDRqmM69343204;     dJNnsfsNDoqWhiBDRqmM69343204 = dJNnsfsNDoqWhiBDRqmM16561278;     dJNnsfsNDoqWhiBDRqmM16561278 = dJNnsfsNDoqWhiBDRqmM69539486;     dJNnsfsNDoqWhiBDRqmM69539486 = dJNnsfsNDoqWhiBDRqmM82514647;     dJNnsfsNDoqWhiBDRqmM82514647 = dJNnsfsNDoqWhiBDRqmM6354150;     dJNnsfsNDoqWhiBDRqmM6354150 = dJNnsfsNDoqWhiBDRqmM83678903;     dJNnsfsNDoqWhiBDRqmM83678903 = dJNnsfsNDoqWhiBDRqmM37009944;     dJNnsfsNDoqWhiBDRqmM37009944 = dJNnsfsNDoqWhiBDRqmM4591634;     dJNnsfsNDoqWhiBDRqmM4591634 = dJNnsfsNDoqWhiBDRqmM73459775;     dJNnsfsNDoqWhiBDRqmM73459775 = dJNnsfsNDoqWhiBDRqmM15981135;     dJNnsfsNDoqWhiBDRqmM15981135 = dJNnsfsNDoqWhiBDRqmM49313920;     dJNnsfsNDoqWhiBDRqmM49313920 = dJNnsfsNDoqWhiBDRqmM99133781;     dJNnsfsNDoqWhiBDRqmM99133781 = dJNnsfsNDoqWhiBDRqmM25688305;     dJNnsfsNDoqWhiBDRqmM25688305 = dJNnsfsNDoqWhiBDRqmM95822827;     dJNnsfsNDoqWhiBDRqmM95822827 = dJNnsfsNDoqWhiBDRqmM87794549;     dJNnsfsNDoqWhiBDRqmM87794549 = dJNnsfsNDoqWhiBDRqmM49036166;     dJNnsfsNDoqWhiBDRqmM49036166 = dJNnsfsNDoqWhiBDRqmM99154314;     dJNnsfsNDoqWhiBDRqmM99154314 = dJNnsfsNDoqWhiBDRqmM68400756;     dJNnsfsNDoqWhiBDRqmM68400756 = dJNnsfsNDoqWhiBDRqmM5552123;     dJNnsfsNDoqWhiBDRqmM5552123 = dJNnsfsNDoqWhiBDRqmM91503729;     dJNnsfsNDoqWhiBDRqmM91503729 = dJNnsfsNDoqWhiBDRqmM25979803;     dJNnsfsNDoqWhiBDRqmM25979803 = dJNnsfsNDoqWhiBDRqmM6449720;     dJNnsfsNDoqWhiBDRqmM6449720 = dJNnsfsNDoqWhiBDRqmM22205047;     dJNnsfsNDoqWhiBDRqmM22205047 = dJNnsfsNDoqWhiBDRqmM76333139;     dJNnsfsNDoqWhiBDRqmM76333139 = dJNnsfsNDoqWhiBDRqmM42324012;     dJNnsfsNDoqWhiBDRqmM42324012 = dJNnsfsNDoqWhiBDRqmM49344190;     dJNnsfsNDoqWhiBDRqmM49344190 = dJNnsfsNDoqWhiBDRqmM99338620;     dJNnsfsNDoqWhiBDRqmM99338620 = dJNnsfsNDoqWhiBDRqmM17603642;     dJNnsfsNDoqWhiBDRqmM17603642 = dJNnsfsNDoqWhiBDRqmM1877481;     dJNnsfsNDoqWhiBDRqmM1877481 = dJNnsfsNDoqWhiBDRqmM77298011;     dJNnsfsNDoqWhiBDRqmM77298011 = dJNnsfsNDoqWhiBDRqmM27987899;     dJNnsfsNDoqWhiBDRqmM27987899 = dJNnsfsNDoqWhiBDRqmM70577940;     dJNnsfsNDoqWhiBDRqmM70577940 = dJNnsfsNDoqWhiBDRqmM64480907;     dJNnsfsNDoqWhiBDRqmM64480907 = dJNnsfsNDoqWhiBDRqmM81453674;     dJNnsfsNDoqWhiBDRqmM81453674 = dJNnsfsNDoqWhiBDRqmM49279303;     dJNnsfsNDoqWhiBDRqmM49279303 = dJNnsfsNDoqWhiBDRqmM58848836;     dJNnsfsNDoqWhiBDRqmM58848836 = dJNnsfsNDoqWhiBDRqmM90799819;     dJNnsfsNDoqWhiBDRqmM90799819 = dJNnsfsNDoqWhiBDRqmM15549533;     dJNnsfsNDoqWhiBDRqmM15549533 = dJNnsfsNDoqWhiBDRqmM81087912;     dJNnsfsNDoqWhiBDRqmM81087912 = dJNnsfsNDoqWhiBDRqmM86856719;     dJNnsfsNDoqWhiBDRqmM86856719 = dJNnsfsNDoqWhiBDRqmM52736176;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void sfPuPdpZJMEJZPadUkEO55977951() {     float FwbwFNLaKzzXySUSnLkg52729705 = -459344666;    float FwbwFNLaKzzXySUSnLkg20769369 = -524905528;    float FwbwFNLaKzzXySUSnLkg30024817 = -486443935;    float FwbwFNLaKzzXySUSnLkg37026984 = -873263718;    float FwbwFNLaKzzXySUSnLkg1378874 = -604912098;    float FwbwFNLaKzzXySUSnLkg26847649 = -88379372;    float FwbwFNLaKzzXySUSnLkg18501806 = -629592592;    float FwbwFNLaKzzXySUSnLkg42895434 = -131797410;    float FwbwFNLaKzzXySUSnLkg62262917 = 40173663;    float FwbwFNLaKzzXySUSnLkg84839236 = -736823614;    float FwbwFNLaKzzXySUSnLkg51195493 = -339475048;    float FwbwFNLaKzzXySUSnLkg25228879 = -97990104;    float FwbwFNLaKzzXySUSnLkg78265763 = -503869359;    float FwbwFNLaKzzXySUSnLkg60119688 = -307105613;    float FwbwFNLaKzzXySUSnLkg15495377 = 15460410;    float FwbwFNLaKzzXySUSnLkg47080268 = -640967321;    float FwbwFNLaKzzXySUSnLkg5339127 = -881696026;    float FwbwFNLaKzzXySUSnLkg45047157 = -371801807;    float FwbwFNLaKzzXySUSnLkg34873605 = -75885277;    float FwbwFNLaKzzXySUSnLkg35278426 = -984914894;    float FwbwFNLaKzzXySUSnLkg89044895 = -882723267;    float FwbwFNLaKzzXySUSnLkg75514761 = -196522553;    float FwbwFNLaKzzXySUSnLkg92721214 = -329102162;    float FwbwFNLaKzzXySUSnLkg19737853 = -923141115;    float FwbwFNLaKzzXySUSnLkg94518134 = -458768259;    float FwbwFNLaKzzXySUSnLkg94208203 = -42110147;    float FwbwFNLaKzzXySUSnLkg92920593 = -919618095;    float FwbwFNLaKzzXySUSnLkg68796716 = -22386058;    float FwbwFNLaKzzXySUSnLkg16820345 = -34091551;    float FwbwFNLaKzzXySUSnLkg73097976 = -357028529;    float FwbwFNLaKzzXySUSnLkg15258253 = -746493418;    float FwbwFNLaKzzXySUSnLkg31919842 = -472169163;    float FwbwFNLaKzzXySUSnLkg59379680 = -241502937;    float FwbwFNLaKzzXySUSnLkg42104053 = -671032302;    float FwbwFNLaKzzXySUSnLkg15173730 = -665952372;    float FwbwFNLaKzzXySUSnLkg97170322 = -211677518;    float FwbwFNLaKzzXySUSnLkg35143168 = -399391140;    float FwbwFNLaKzzXySUSnLkg20270265 = -698284279;    float FwbwFNLaKzzXySUSnLkg31671916 = -636460113;    float FwbwFNLaKzzXySUSnLkg90495400 = -754920995;    float FwbwFNLaKzzXySUSnLkg63265781 = -620101688;    float FwbwFNLaKzzXySUSnLkg46036966 = -673447165;    float FwbwFNLaKzzXySUSnLkg58347540 = 80690067;    float FwbwFNLaKzzXySUSnLkg46673960 = -436295416;    float FwbwFNLaKzzXySUSnLkg54553675 = -142978302;    float FwbwFNLaKzzXySUSnLkg14999683 = -41666042;    float FwbwFNLaKzzXySUSnLkg79352116 = -122712398;    float FwbwFNLaKzzXySUSnLkg91632872 = -837518820;    float FwbwFNLaKzzXySUSnLkg29846535 = -645946704;    float FwbwFNLaKzzXySUSnLkg47854182 = -736780883;    float FwbwFNLaKzzXySUSnLkg67943464 = -969136356;    float FwbwFNLaKzzXySUSnLkg47048642 = -308617553;    float FwbwFNLaKzzXySUSnLkg23305713 = -63106135;    float FwbwFNLaKzzXySUSnLkg66573887 = -167062372;    float FwbwFNLaKzzXySUSnLkg66933055 = -729218471;    float FwbwFNLaKzzXySUSnLkg77214943 = -162822114;    float FwbwFNLaKzzXySUSnLkg28048154 = -95803367;    float FwbwFNLaKzzXySUSnLkg10286964 = -563302821;    float FwbwFNLaKzzXySUSnLkg42508850 = -314495459;    float FwbwFNLaKzzXySUSnLkg7170670 = -462801952;    float FwbwFNLaKzzXySUSnLkg33927055 = -168761277;    float FwbwFNLaKzzXySUSnLkg49705089 = -507206534;    float FwbwFNLaKzzXySUSnLkg26075090 = 2294141;    float FwbwFNLaKzzXySUSnLkg89164941 = -602797808;    float FwbwFNLaKzzXySUSnLkg69580984 = -990330196;    float FwbwFNLaKzzXySUSnLkg19275651 = -867305886;    float FwbwFNLaKzzXySUSnLkg65849198 = -856487167;    float FwbwFNLaKzzXySUSnLkg36161710 = -832837058;    float FwbwFNLaKzzXySUSnLkg44945958 = -641153242;    float FwbwFNLaKzzXySUSnLkg18325055 = -772862072;    float FwbwFNLaKzzXySUSnLkg11937100 = -141576182;    float FwbwFNLaKzzXySUSnLkg85068861 = -83411748;    float FwbwFNLaKzzXySUSnLkg13375241 = -735341694;    float FwbwFNLaKzzXySUSnLkg44378204 = -320964283;    float FwbwFNLaKzzXySUSnLkg72012644 = -264813206;    float FwbwFNLaKzzXySUSnLkg43007930 = -109276103;    float FwbwFNLaKzzXySUSnLkg17167221 = -177212621;    float FwbwFNLaKzzXySUSnLkg46047254 = -892806746;    float FwbwFNLaKzzXySUSnLkg65184178 = -680162813;    float FwbwFNLaKzzXySUSnLkg79518451 = -317102218;    float FwbwFNLaKzzXySUSnLkg14856087 = -919397750;    float FwbwFNLaKzzXySUSnLkg1287721 = 17900725;    float FwbwFNLaKzzXySUSnLkg38950182 = -376439354;    float FwbwFNLaKzzXySUSnLkg68966163 = -297310668;    float FwbwFNLaKzzXySUSnLkg5154513 = -387892173;    float FwbwFNLaKzzXySUSnLkg68209611 = -337875865;    float FwbwFNLaKzzXySUSnLkg8614130 = -309063028;    float FwbwFNLaKzzXySUSnLkg92805793 = 25559434;    float FwbwFNLaKzzXySUSnLkg75170997 = -941813831;    float FwbwFNLaKzzXySUSnLkg37958787 = -403130258;    float FwbwFNLaKzzXySUSnLkg69122168 = -15874152;    float FwbwFNLaKzzXySUSnLkg24856204 = -836088319;    float FwbwFNLaKzzXySUSnLkg77761415 = -283788820;    float FwbwFNLaKzzXySUSnLkg24501246 = -73658162;    float FwbwFNLaKzzXySUSnLkg56568345 = -486159718;    float FwbwFNLaKzzXySUSnLkg13560693 = -12895155;    float FwbwFNLaKzzXySUSnLkg19961877 = -575741306;    float FwbwFNLaKzzXySUSnLkg69182599 = -316512125;    float FwbwFNLaKzzXySUSnLkg77092976 = -445965220;    float FwbwFNLaKzzXySUSnLkg35278024 = -459344666;     FwbwFNLaKzzXySUSnLkg52729705 = FwbwFNLaKzzXySUSnLkg20769369;     FwbwFNLaKzzXySUSnLkg20769369 = FwbwFNLaKzzXySUSnLkg30024817;     FwbwFNLaKzzXySUSnLkg30024817 = FwbwFNLaKzzXySUSnLkg37026984;     FwbwFNLaKzzXySUSnLkg37026984 = FwbwFNLaKzzXySUSnLkg1378874;     FwbwFNLaKzzXySUSnLkg1378874 = FwbwFNLaKzzXySUSnLkg26847649;     FwbwFNLaKzzXySUSnLkg26847649 = FwbwFNLaKzzXySUSnLkg18501806;     FwbwFNLaKzzXySUSnLkg18501806 = FwbwFNLaKzzXySUSnLkg42895434;     FwbwFNLaKzzXySUSnLkg42895434 = FwbwFNLaKzzXySUSnLkg62262917;     FwbwFNLaKzzXySUSnLkg62262917 = FwbwFNLaKzzXySUSnLkg84839236;     FwbwFNLaKzzXySUSnLkg84839236 = FwbwFNLaKzzXySUSnLkg51195493;     FwbwFNLaKzzXySUSnLkg51195493 = FwbwFNLaKzzXySUSnLkg25228879;     FwbwFNLaKzzXySUSnLkg25228879 = FwbwFNLaKzzXySUSnLkg78265763;     FwbwFNLaKzzXySUSnLkg78265763 = FwbwFNLaKzzXySUSnLkg60119688;     FwbwFNLaKzzXySUSnLkg60119688 = FwbwFNLaKzzXySUSnLkg15495377;     FwbwFNLaKzzXySUSnLkg15495377 = FwbwFNLaKzzXySUSnLkg47080268;     FwbwFNLaKzzXySUSnLkg47080268 = FwbwFNLaKzzXySUSnLkg5339127;     FwbwFNLaKzzXySUSnLkg5339127 = FwbwFNLaKzzXySUSnLkg45047157;     FwbwFNLaKzzXySUSnLkg45047157 = FwbwFNLaKzzXySUSnLkg34873605;     FwbwFNLaKzzXySUSnLkg34873605 = FwbwFNLaKzzXySUSnLkg35278426;     FwbwFNLaKzzXySUSnLkg35278426 = FwbwFNLaKzzXySUSnLkg89044895;     FwbwFNLaKzzXySUSnLkg89044895 = FwbwFNLaKzzXySUSnLkg75514761;     FwbwFNLaKzzXySUSnLkg75514761 = FwbwFNLaKzzXySUSnLkg92721214;     FwbwFNLaKzzXySUSnLkg92721214 = FwbwFNLaKzzXySUSnLkg19737853;     FwbwFNLaKzzXySUSnLkg19737853 = FwbwFNLaKzzXySUSnLkg94518134;     FwbwFNLaKzzXySUSnLkg94518134 = FwbwFNLaKzzXySUSnLkg94208203;     FwbwFNLaKzzXySUSnLkg94208203 = FwbwFNLaKzzXySUSnLkg92920593;     FwbwFNLaKzzXySUSnLkg92920593 = FwbwFNLaKzzXySUSnLkg68796716;     FwbwFNLaKzzXySUSnLkg68796716 = FwbwFNLaKzzXySUSnLkg16820345;     FwbwFNLaKzzXySUSnLkg16820345 = FwbwFNLaKzzXySUSnLkg73097976;     FwbwFNLaKzzXySUSnLkg73097976 = FwbwFNLaKzzXySUSnLkg15258253;     FwbwFNLaKzzXySUSnLkg15258253 = FwbwFNLaKzzXySUSnLkg31919842;     FwbwFNLaKzzXySUSnLkg31919842 = FwbwFNLaKzzXySUSnLkg59379680;     FwbwFNLaKzzXySUSnLkg59379680 = FwbwFNLaKzzXySUSnLkg42104053;     FwbwFNLaKzzXySUSnLkg42104053 = FwbwFNLaKzzXySUSnLkg15173730;     FwbwFNLaKzzXySUSnLkg15173730 = FwbwFNLaKzzXySUSnLkg97170322;     FwbwFNLaKzzXySUSnLkg97170322 = FwbwFNLaKzzXySUSnLkg35143168;     FwbwFNLaKzzXySUSnLkg35143168 = FwbwFNLaKzzXySUSnLkg20270265;     FwbwFNLaKzzXySUSnLkg20270265 = FwbwFNLaKzzXySUSnLkg31671916;     FwbwFNLaKzzXySUSnLkg31671916 = FwbwFNLaKzzXySUSnLkg90495400;     FwbwFNLaKzzXySUSnLkg90495400 = FwbwFNLaKzzXySUSnLkg63265781;     FwbwFNLaKzzXySUSnLkg63265781 = FwbwFNLaKzzXySUSnLkg46036966;     FwbwFNLaKzzXySUSnLkg46036966 = FwbwFNLaKzzXySUSnLkg58347540;     FwbwFNLaKzzXySUSnLkg58347540 = FwbwFNLaKzzXySUSnLkg46673960;     FwbwFNLaKzzXySUSnLkg46673960 = FwbwFNLaKzzXySUSnLkg54553675;     FwbwFNLaKzzXySUSnLkg54553675 = FwbwFNLaKzzXySUSnLkg14999683;     FwbwFNLaKzzXySUSnLkg14999683 = FwbwFNLaKzzXySUSnLkg79352116;     FwbwFNLaKzzXySUSnLkg79352116 = FwbwFNLaKzzXySUSnLkg91632872;     FwbwFNLaKzzXySUSnLkg91632872 = FwbwFNLaKzzXySUSnLkg29846535;     FwbwFNLaKzzXySUSnLkg29846535 = FwbwFNLaKzzXySUSnLkg47854182;     FwbwFNLaKzzXySUSnLkg47854182 = FwbwFNLaKzzXySUSnLkg67943464;     FwbwFNLaKzzXySUSnLkg67943464 = FwbwFNLaKzzXySUSnLkg47048642;     FwbwFNLaKzzXySUSnLkg47048642 = FwbwFNLaKzzXySUSnLkg23305713;     FwbwFNLaKzzXySUSnLkg23305713 = FwbwFNLaKzzXySUSnLkg66573887;     FwbwFNLaKzzXySUSnLkg66573887 = FwbwFNLaKzzXySUSnLkg66933055;     FwbwFNLaKzzXySUSnLkg66933055 = FwbwFNLaKzzXySUSnLkg77214943;     FwbwFNLaKzzXySUSnLkg77214943 = FwbwFNLaKzzXySUSnLkg28048154;     FwbwFNLaKzzXySUSnLkg28048154 = FwbwFNLaKzzXySUSnLkg10286964;     FwbwFNLaKzzXySUSnLkg10286964 = FwbwFNLaKzzXySUSnLkg42508850;     FwbwFNLaKzzXySUSnLkg42508850 = FwbwFNLaKzzXySUSnLkg7170670;     FwbwFNLaKzzXySUSnLkg7170670 = FwbwFNLaKzzXySUSnLkg33927055;     FwbwFNLaKzzXySUSnLkg33927055 = FwbwFNLaKzzXySUSnLkg49705089;     FwbwFNLaKzzXySUSnLkg49705089 = FwbwFNLaKzzXySUSnLkg26075090;     FwbwFNLaKzzXySUSnLkg26075090 = FwbwFNLaKzzXySUSnLkg89164941;     FwbwFNLaKzzXySUSnLkg89164941 = FwbwFNLaKzzXySUSnLkg69580984;     FwbwFNLaKzzXySUSnLkg69580984 = FwbwFNLaKzzXySUSnLkg19275651;     FwbwFNLaKzzXySUSnLkg19275651 = FwbwFNLaKzzXySUSnLkg65849198;     FwbwFNLaKzzXySUSnLkg65849198 = FwbwFNLaKzzXySUSnLkg36161710;     FwbwFNLaKzzXySUSnLkg36161710 = FwbwFNLaKzzXySUSnLkg44945958;     FwbwFNLaKzzXySUSnLkg44945958 = FwbwFNLaKzzXySUSnLkg18325055;     FwbwFNLaKzzXySUSnLkg18325055 = FwbwFNLaKzzXySUSnLkg11937100;     FwbwFNLaKzzXySUSnLkg11937100 = FwbwFNLaKzzXySUSnLkg85068861;     FwbwFNLaKzzXySUSnLkg85068861 = FwbwFNLaKzzXySUSnLkg13375241;     FwbwFNLaKzzXySUSnLkg13375241 = FwbwFNLaKzzXySUSnLkg44378204;     FwbwFNLaKzzXySUSnLkg44378204 = FwbwFNLaKzzXySUSnLkg72012644;     FwbwFNLaKzzXySUSnLkg72012644 = FwbwFNLaKzzXySUSnLkg43007930;     FwbwFNLaKzzXySUSnLkg43007930 = FwbwFNLaKzzXySUSnLkg17167221;     FwbwFNLaKzzXySUSnLkg17167221 = FwbwFNLaKzzXySUSnLkg46047254;     FwbwFNLaKzzXySUSnLkg46047254 = FwbwFNLaKzzXySUSnLkg65184178;     FwbwFNLaKzzXySUSnLkg65184178 = FwbwFNLaKzzXySUSnLkg79518451;     FwbwFNLaKzzXySUSnLkg79518451 = FwbwFNLaKzzXySUSnLkg14856087;     FwbwFNLaKzzXySUSnLkg14856087 = FwbwFNLaKzzXySUSnLkg1287721;     FwbwFNLaKzzXySUSnLkg1287721 = FwbwFNLaKzzXySUSnLkg38950182;     FwbwFNLaKzzXySUSnLkg38950182 = FwbwFNLaKzzXySUSnLkg68966163;     FwbwFNLaKzzXySUSnLkg68966163 = FwbwFNLaKzzXySUSnLkg5154513;     FwbwFNLaKzzXySUSnLkg5154513 = FwbwFNLaKzzXySUSnLkg68209611;     FwbwFNLaKzzXySUSnLkg68209611 = FwbwFNLaKzzXySUSnLkg8614130;     FwbwFNLaKzzXySUSnLkg8614130 = FwbwFNLaKzzXySUSnLkg92805793;     FwbwFNLaKzzXySUSnLkg92805793 = FwbwFNLaKzzXySUSnLkg75170997;     FwbwFNLaKzzXySUSnLkg75170997 = FwbwFNLaKzzXySUSnLkg37958787;     FwbwFNLaKzzXySUSnLkg37958787 = FwbwFNLaKzzXySUSnLkg69122168;     FwbwFNLaKzzXySUSnLkg69122168 = FwbwFNLaKzzXySUSnLkg24856204;     FwbwFNLaKzzXySUSnLkg24856204 = FwbwFNLaKzzXySUSnLkg77761415;     FwbwFNLaKzzXySUSnLkg77761415 = FwbwFNLaKzzXySUSnLkg24501246;     FwbwFNLaKzzXySUSnLkg24501246 = FwbwFNLaKzzXySUSnLkg56568345;     FwbwFNLaKzzXySUSnLkg56568345 = FwbwFNLaKzzXySUSnLkg13560693;     FwbwFNLaKzzXySUSnLkg13560693 = FwbwFNLaKzzXySUSnLkg19961877;     FwbwFNLaKzzXySUSnLkg19961877 = FwbwFNLaKzzXySUSnLkg69182599;     FwbwFNLaKzzXySUSnLkg69182599 = FwbwFNLaKzzXySUSnLkg77092976;     FwbwFNLaKzzXySUSnLkg77092976 = FwbwFNLaKzzXySUSnLkg35278024;     FwbwFNLaKzzXySUSnLkg35278024 = FwbwFNLaKzzXySUSnLkg52729705;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void nrObXrsTnUEJEpbFKVnf10394164() {     float POYxhBQaAicWXfOAigMQ23381372 = 22506203;    float POYxhBQaAicWXfOAigMQ89442886 = -396548827;    float POYxhBQaAicWXfOAigMQ9993736 = -928738395;    float POYxhBQaAicWXfOAigMQ76958271 = 36597893;    float POYxhBQaAicWXfOAigMQ22829195 = -388318446;    float POYxhBQaAicWXfOAigMQ16847411 = -889960523;    float POYxhBQaAicWXfOAigMQ36880701 = -628368302;    float POYxhBQaAicWXfOAigMQ99737814 = -478614499;    float POYxhBQaAicWXfOAigMQ8127972 = -53173665;    float POYxhBQaAicWXfOAigMQ7805698 = 62081036;    float POYxhBQaAicWXfOAigMQ93444653 = -374376978;    float POYxhBQaAicWXfOAigMQ70775450 = -413753896;    float POYxhBQaAicWXfOAigMQ58093150 = -413406418;    float POYxhBQaAicWXfOAigMQ43966721 = -108133823;    float POYxhBQaAicWXfOAigMQ98215396 = -134289593;    float POYxhBQaAicWXfOAigMQ79608908 = -210950908;    float POYxhBQaAicWXfOAigMQ99191953 = -790881156;    float POYxhBQaAicWXfOAigMQ65340651 = -360968003;    float POYxhBQaAicWXfOAigMQ16581386 = -783073219;    float POYxhBQaAicWXfOAigMQ94065135 = -427281199;    float POYxhBQaAicWXfOAigMQ44180650 = -791402408;    float POYxhBQaAicWXfOAigMQ36910526 = -527807859;    float POYxhBQaAicWXfOAigMQ94665023 = -896945306;    float POYxhBQaAicWXfOAigMQ53587390 = -515542021;    float POYxhBQaAicWXfOAigMQ58642523 = -134672947;    float POYxhBQaAicWXfOAigMQ24805784 = -140394592;    float POYxhBQaAicWXfOAigMQ1181380 = -214795870;    float POYxhBQaAicWXfOAigMQ76578942 = -527509826;    float POYxhBQaAicWXfOAigMQ48962832 = -328449768;    float POYxhBQaAicWXfOAigMQ12270072 = -915423599;    float POYxhBQaAicWXfOAigMQ22330249 = -494927103;    float POYxhBQaAicWXfOAigMQ76431303 = -800466326;    float POYxhBQaAicWXfOAigMQ42325041 = -188464546;    float POYxhBQaAicWXfOAigMQ7469028 = -234561986;    float POYxhBQaAicWXfOAigMQ93197966 = -512899151;    float POYxhBQaAicWXfOAigMQ57163582 = -290904066;    float POYxhBQaAicWXfOAigMQ78934653 = -587984347;    float POYxhBQaAicWXfOAigMQ73062754 = -482397386;    float POYxhBQaAicWXfOAigMQ10205205 = -592188750;    float POYxhBQaAicWXfOAigMQ85598698 = -272529358;    float POYxhBQaAicWXfOAigMQ23339066 = 95879433;    float POYxhBQaAicWXfOAigMQ74519957 = -442836234;    float POYxhBQaAicWXfOAigMQ7314501 = 93936015;    float POYxhBQaAicWXfOAigMQ53182261 = -508270387;    float POYxhBQaAicWXfOAigMQ50507733 = -451624696;    float POYxhBQaAicWXfOAigMQ21405518 = -900003197;    float POYxhBQaAicWXfOAigMQ5483320 = -238782197;    float POYxhBQaAicWXfOAigMQ57247083 = -189649116;    float POYxhBQaAicWXfOAigMQ35287987 = -631710673;    float POYxhBQaAicWXfOAigMQ56281668 = 80210238;    float POYxhBQaAicWXfOAigMQ36132788 = -515708477;    float POYxhBQaAicWXfOAigMQ88920146 = -277837275;    float POYxhBQaAicWXfOAigMQ78381369 = 9095823;    float POYxhBQaAicWXfOAigMQ56801191 = -827943869;    float POYxhBQaAicWXfOAigMQ66213514 = -840159386;    float POYxhBQaAicWXfOAigMQ86470845 = -449685939;    float POYxhBQaAicWXfOAigMQ94777862 = -499603521;    float POYxhBQaAicWXfOAigMQ56406346 = -313196374;    float POYxhBQaAicWXfOAigMQ18315748 = -828729160;    float POYxhBQaAicWXfOAigMQ98023410 = -147923854;    float POYxhBQaAicWXfOAigMQ15666032 = -575164653;    float POYxhBQaAicWXfOAigMQ60301759 = -858477;    float POYxhBQaAicWXfOAigMQ50774982 = -50164732;    float POYxhBQaAicWXfOAigMQ95857900 = -137750066;    float POYxhBQaAicWXfOAigMQ85475448 = -442991861;    float POYxhBQaAicWXfOAigMQ17013351 = -573910653;    float POYxhBQaAicWXfOAigMQ28450409 = -125289351;    float POYxhBQaAicWXfOAigMQ50624122 = -78844432;    float POYxhBQaAicWXfOAigMQ50768755 = -595234673;    float POYxhBQaAicWXfOAigMQ41051815 = -843385528;    float POYxhBQaAicWXfOAigMQ674255 = -622966562;    float POYxhBQaAicWXfOAigMQ26129200 = -208483771;    float POYxhBQaAicWXfOAigMQ55135446 = -768779253;    float POYxhBQaAicWXfOAigMQ30982688 = -410543862;    float POYxhBQaAicWXfOAigMQ70726070 = -423160633;    float POYxhBQaAicWXfOAigMQ69660693 = -248566174;    float POYxhBQaAicWXfOAigMQ29596026 = -521743874;    float POYxhBQaAicWXfOAigMQ41482763 = -288674920;    float POYxhBQaAicWXfOAigMQ3079657 = 36082675;    float POYxhBQaAicWXfOAigMQ37237005 = -234669750;    float POYxhBQaAicWXfOAigMQ19322465 = -901612396;    float POYxhBQaAicWXfOAigMQ43934296 = 74853245;    float POYxhBQaAicWXfOAigMQ41290955 = -895799153;    float POYxhBQaAicWXfOAigMQ92681164 = -308660006;    float POYxhBQaAicWXfOAigMQ76137283 = -299715122;    float POYxhBQaAicWXfOAigMQ33410103 = -117089829;    float POYxhBQaAicWXfOAigMQ98049933 = -709562149;    float POYxhBQaAicWXfOAigMQ85523850 = -360520678;    float POYxhBQaAicWXfOAigMQ41255514 = -394402601;    float POYxhBQaAicWXfOAigMQ6727121 = 36786787;    float POYxhBQaAicWXfOAigMQ62385719 = -791300545;    float POYxhBQaAicWXfOAigMQ22528308 = -174787973;    float POYxhBQaAicWXfOAigMQ54747006 = -653668226;    float POYxhBQaAicWXfOAigMQ12181794 = -344264896;    float POYxhBQaAicWXfOAigMQ69932666 = -697364705;    float POYxhBQaAicWXfOAigMQ63037306 = -903262091;    float POYxhBQaAicWXfOAigMQ23744975 = -292671502;    float POYxhBQaAicWXfOAigMQ11456601 = -768313919;    float POYxhBQaAicWXfOAigMQ67706812 = 34721474;    float POYxhBQaAicWXfOAigMQ33494383 = 22506203;     POYxhBQaAicWXfOAigMQ23381372 = POYxhBQaAicWXfOAigMQ89442886;     POYxhBQaAicWXfOAigMQ89442886 = POYxhBQaAicWXfOAigMQ9993736;     POYxhBQaAicWXfOAigMQ9993736 = POYxhBQaAicWXfOAigMQ76958271;     POYxhBQaAicWXfOAigMQ76958271 = POYxhBQaAicWXfOAigMQ22829195;     POYxhBQaAicWXfOAigMQ22829195 = POYxhBQaAicWXfOAigMQ16847411;     POYxhBQaAicWXfOAigMQ16847411 = POYxhBQaAicWXfOAigMQ36880701;     POYxhBQaAicWXfOAigMQ36880701 = POYxhBQaAicWXfOAigMQ99737814;     POYxhBQaAicWXfOAigMQ99737814 = POYxhBQaAicWXfOAigMQ8127972;     POYxhBQaAicWXfOAigMQ8127972 = POYxhBQaAicWXfOAigMQ7805698;     POYxhBQaAicWXfOAigMQ7805698 = POYxhBQaAicWXfOAigMQ93444653;     POYxhBQaAicWXfOAigMQ93444653 = POYxhBQaAicWXfOAigMQ70775450;     POYxhBQaAicWXfOAigMQ70775450 = POYxhBQaAicWXfOAigMQ58093150;     POYxhBQaAicWXfOAigMQ58093150 = POYxhBQaAicWXfOAigMQ43966721;     POYxhBQaAicWXfOAigMQ43966721 = POYxhBQaAicWXfOAigMQ98215396;     POYxhBQaAicWXfOAigMQ98215396 = POYxhBQaAicWXfOAigMQ79608908;     POYxhBQaAicWXfOAigMQ79608908 = POYxhBQaAicWXfOAigMQ99191953;     POYxhBQaAicWXfOAigMQ99191953 = POYxhBQaAicWXfOAigMQ65340651;     POYxhBQaAicWXfOAigMQ65340651 = POYxhBQaAicWXfOAigMQ16581386;     POYxhBQaAicWXfOAigMQ16581386 = POYxhBQaAicWXfOAigMQ94065135;     POYxhBQaAicWXfOAigMQ94065135 = POYxhBQaAicWXfOAigMQ44180650;     POYxhBQaAicWXfOAigMQ44180650 = POYxhBQaAicWXfOAigMQ36910526;     POYxhBQaAicWXfOAigMQ36910526 = POYxhBQaAicWXfOAigMQ94665023;     POYxhBQaAicWXfOAigMQ94665023 = POYxhBQaAicWXfOAigMQ53587390;     POYxhBQaAicWXfOAigMQ53587390 = POYxhBQaAicWXfOAigMQ58642523;     POYxhBQaAicWXfOAigMQ58642523 = POYxhBQaAicWXfOAigMQ24805784;     POYxhBQaAicWXfOAigMQ24805784 = POYxhBQaAicWXfOAigMQ1181380;     POYxhBQaAicWXfOAigMQ1181380 = POYxhBQaAicWXfOAigMQ76578942;     POYxhBQaAicWXfOAigMQ76578942 = POYxhBQaAicWXfOAigMQ48962832;     POYxhBQaAicWXfOAigMQ48962832 = POYxhBQaAicWXfOAigMQ12270072;     POYxhBQaAicWXfOAigMQ12270072 = POYxhBQaAicWXfOAigMQ22330249;     POYxhBQaAicWXfOAigMQ22330249 = POYxhBQaAicWXfOAigMQ76431303;     POYxhBQaAicWXfOAigMQ76431303 = POYxhBQaAicWXfOAigMQ42325041;     POYxhBQaAicWXfOAigMQ42325041 = POYxhBQaAicWXfOAigMQ7469028;     POYxhBQaAicWXfOAigMQ7469028 = POYxhBQaAicWXfOAigMQ93197966;     POYxhBQaAicWXfOAigMQ93197966 = POYxhBQaAicWXfOAigMQ57163582;     POYxhBQaAicWXfOAigMQ57163582 = POYxhBQaAicWXfOAigMQ78934653;     POYxhBQaAicWXfOAigMQ78934653 = POYxhBQaAicWXfOAigMQ73062754;     POYxhBQaAicWXfOAigMQ73062754 = POYxhBQaAicWXfOAigMQ10205205;     POYxhBQaAicWXfOAigMQ10205205 = POYxhBQaAicWXfOAigMQ85598698;     POYxhBQaAicWXfOAigMQ85598698 = POYxhBQaAicWXfOAigMQ23339066;     POYxhBQaAicWXfOAigMQ23339066 = POYxhBQaAicWXfOAigMQ74519957;     POYxhBQaAicWXfOAigMQ74519957 = POYxhBQaAicWXfOAigMQ7314501;     POYxhBQaAicWXfOAigMQ7314501 = POYxhBQaAicWXfOAigMQ53182261;     POYxhBQaAicWXfOAigMQ53182261 = POYxhBQaAicWXfOAigMQ50507733;     POYxhBQaAicWXfOAigMQ50507733 = POYxhBQaAicWXfOAigMQ21405518;     POYxhBQaAicWXfOAigMQ21405518 = POYxhBQaAicWXfOAigMQ5483320;     POYxhBQaAicWXfOAigMQ5483320 = POYxhBQaAicWXfOAigMQ57247083;     POYxhBQaAicWXfOAigMQ57247083 = POYxhBQaAicWXfOAigMQ35287987;     POYxhBQaAicWXfOAigMQ35287987 = POYxhBQaAicWXfOAigMQ56281668;     POYxhBQaAicWXfOAigMQ56281668 = POYxhBQaAicWXfOAigMQ36132788;     POYxhBQaAicWXfOAigMQ36132788 = POYxhBQaAicWXfOAigMQ88920146;     POYxhBQaAicWXfOAigMQ88920146 = POYxhBQaAicWXfOAigMQ78381369;     POYxhBQaAicWXfOAigMQ78381369 = POYxhBQaAicWXfOAigMQ56801191;     POYxhBQaAicWXfOAigMQ56801191 = POYxhBQaAicWXfOAigMQ66213514;     POYxhBQaAicWXfOAigMQ66213514 = POYxhBQaAicWXfOAigMQ86470845;     POYxhBQaAicWXfOAigMQ86470845 = POYxhBQaAicWXfOAigMQ94777862;     POYxhBQaAicWXfOAigMQ94777862 = POYxhBQaAicWXfOAigMQ56406346;     POYxhBQaAicWXfOAigMQ56406346 = POYxhBQaAicWXfOAigMQ18315748;     POYxhBQaAicWXfOAigMQ18315748 = POYxhBQaAicWXfOAigMQ98023410;     POYxhBQaAicWXfOAigMQ98023410 = POYxhBQaAicWXfOAigMQ15666032;     POYxhBQaAicWXfOAigMQ15666032 = POYxhBQaAicWXfOAigMQ60301759;     POYxhBQaAicWXfOAigMQ60301759 = POYxhBQaAicWXfOAigMQ50774982;     POYxhBQaAicWXfOAigMQ50774982 = POYxhBQaAicWXfOAigMQ95857900;     POYxhBQaAicWXfOAigMQ95857900 = POYxhBQaAicWXfOAigMQ85475448;     POYxhBQaAicWXfOAigMQ85475448 = POYxhBQaAicWXfOAigMQ17013351;     POYxhBQaAicWXfOAigMQ17013351 = POYxhBQaAicWXfOAigMQ28450409;     POYxhBQaAicWXfOAigMQ28450409 = POYxhBQaAicWXfOAigMQ50624122;     POYxhBQaAicWXfOAigMQ50624122 = POYxhBQaAicWXfOAigMQ50768755;     POYxhBQaAicWXfOAigMQ50768755 = POYxhBQaAicWXfOAigMQ41051815;     POYxhBQaAicWXfOAigMQ41051815 = POYxhBQaAicWXfOAigMQ674255;     POYxhBQaAicWXfOAigMQ674255 = POYxhBQaAicWXfOAigMQ26129200;     POYxhBQaAicWXfOAigMQ26129200 = POYxhBQaAicWXfOAigMQ55135446;     POYxhBQaAicWXfOAigMQ55135446 = POYxhBQaAicWXfOAigMQ30982688;     POYxhBQaAicWXfOAigMQ30982688 = POYxhBQaAicWXfOAigMQ70726070;     POYxhBQaAicWXfOAigMQ70726070 = POYxhBQaAicWXfOAigMQ69660693;     POYxhBQaAicWXfOAigMQ69660693 = POYxhBQaAicWXfOAigMQ29596026;     POYxhBQaAicWXfOAigMQ29596026 = POYxhBQaAicWXfOAigMQ41482763;     POYxhBQaAicWXfOAigMQ41482763 = POYxhBQaAicWXfOAigMQ3079657;     POYxhBQaAicWXfOAigMQ3079657 = POYxhBQaAicWXfOAigMQ37237005;     POYxhBQaAicWXfOAigMQ37237005 = POYxhBQaAicWXfOAigMQ19322465;     POYxhBQaAicWXfOAigMQ19322465 = POYxhBQaAicWXfOAigMQ43934296;     POYxhBQaAicWXfOAigMQ43934296 = POYxhBQaAicWXfOAigMQ41290955;     POYxhBQaAicWXfOAigMQ41290955 = POYxhBQaAicWXfOAigMQ92681164;     POYxhBQaAicWXfOAigMQ92681164 = POYxhBQaAicWXfOAigMQ76137283;     POYxhBQaAicWXfOAigMQ76137283 = POYxhBQaAicWXfOAigMQ33410103;     POYxhBQaAicWXfOAigMQ33410103 = POYxhBQaAicWXfOAigMQ98049933;     POYxhBQaAicWXfOAigMQ98049933 = POYxhBQaAicWXfOAigMQ85523850;     POYxhBQaAicWXfOAigMQ85523850 = POYxhBQaAicWXfOAigMQ41255514;     POYxhBQaAicWXfOAigMQ41255514 = POYxhBQaAicWXfOAigMQ6727121;     POYxhBQaAicWXfOAigMQ6727121 = POYxhBQaAicWXfOAigMQ62385719;     POYxhBQaAicWXfOAigMQ62385719 = POYxhBQaAicWXfOAigMQ22528308;     POYxhBQaAicWXfOAigMQ22528308 = POYxhBQaAicWXfOAigMQ54747006;     POYxhBQaAicWXfOAigMQ54747006 = POYxhBQaAicWXfOAigMQ12181794;     POYxhBQaAicWXfOAigMQ12181794 = POYxhBQaAicWXfOAigMQ69932666;     POYxhBQaAicWXfOAigMQ69932666 = POYxhBQaAicWXfOAigMQ63037306;     POYxhBQaAicWXfOAigMQ63037306 = POYxhBQaAicWXfOAigMQ23744975;     POYxhBQaAicWXfOAigMQ23744975 = POYxhBQaAicWXfOAigMQ11456601;     POYxhBQaAicWXfOAigMQ11456601 = POYxhBQaAicWXfOAigMQ67706812;     POYxhBQaAicWXfOAigMQ67706812 = POYxhBQaAicWXfOAigMQ33494383;     POYxhBQaAicWXfOAigMQ33494383 = POYxhBQaAicWXfOAigMQ23381372;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LOdyehdivMtpYJaQlBSG61323735() {     float myrdWTONMLQeCYSygvak98968266 = -259395862;    float myrdWTONMLQeCYSygvak51196950 = -641377823;    float myrdWTONMLQeCYSygvak67544608 = 83082223;    float myrdWTONMLQeCYSygvak26410236 = -840589793;    float myrdWTONMLQeCYSygvak89961173 = -868151902;    float myrdWTONMLQeCYSygvak25155330 = -20083323;    float myrdWTONMLQeCYSygvak64712 = -459505392;    float myrdWTONMLQeCYSygvak47444426 = -383252049;    float myrdWTONMLQeCYSygvak62772223 = -345986192;    float myrdWTONMLQeCYSygvak27804202 = -638278770;    float myrdWTONMLQeCYSygvak2698057 = -144936846;    float myrdWTONMLQeCYSygvak2520187 = -373593212;    float myrdWTONMLQeCYSygvak76710699 = -447240165;    float myrdWTONMLQeCYSygvak47065383 = -546369854;    float myrdWTONMLQeCYSygvak93191257 = -32157338;    float myrdWTONMLQeCYSygvak93688813 = -288943473;    float myrdWTONMLQeCYSygvak85026879 = 25702692;    float myrdWTONMLQeCYSygvak5706422 = 47022542;    float myrdWTONMLQeCYSygvak97939039 = -861551510;    float myrdWTONMLQeCYSygvak87208161 = -798714462;    float myrdWTONMLQeCYSygvak25272559 = -775485596;    float myrdWTONMLQeCYSygvak93839566 = 4957183;    float myrdWTONMLQeCYSygvak89209221 = -1297951;    float myrdWTONMLQeCYSygvak94388905 = -954160870;    float myrdWTONMLQeCYSygvak31434990 = -420495626;    float myrdWTONMLQeCYSygvak25151402 = -277793652;    float myrdWTONMLQeCYSygvak91013892 = -805409077;    float myrdWTONMLQeCYSygvak31626861 = -54930311;    float myrdWTONMLQeCYSygvak3670433 = -678226039;    float myrdWTONMLQeCYSygvak71570095 = -905475318;    float myrdWTONMLQeCYSygvak36023714 = -726800825;    float myrdWTONMLQeCYSygvak17440271 = -426306719;    float myrdWTONMLQeCYSygvak92148987 = -860799629;    float myrdWTONMLQeCYSygvak60985390 = -596536524;    float myrdWTONMLQeCYSygvak24013445 = -49506581;    float myrdWTONMLQeCYSygvak51248385 = -513557346;    float myrdWTONMLQeCYSygvak50594697 = -218582947;    float myrdWTONMLQeCYSygvak81156020 = -332296541;    float myrdWTONMLQeCYSygvak92300384 = -597356718;    float myrdWTONMLQeCYSygvak1414150 = -15141755;    float myrdWTONMLQeCYSygvak73526429 = -732194782;    float myrdWTONMLQeCYSygvak22105132 = -903426717;    float myrdWTONMLQeCYSygvak66443880 = -344887803;    float myrdWTONMLQeCYSygvak84167565 = 12555125;    float myrdWTONMLQeCYSygvak71484096 = -547330963;    float myrdWTONMLQeCYSygvak77698848 = -644539567;    float myrdWTONMLQeCYSygvak48327326 = -700930902;    float myrdWTONMLQeCYSygvak21513959 = -505641623;    float myrdWTONMLQeCYSygvak56434709 = -565445935;    float myrdWTONMLQeCYSygvak55080307 = -971056693;    float myrdWTONMLQeCYSygvak92954193 = -869728328;    float myrdWTONMLQeCYSygvak62541911 = -85380017;    float myrdWTONMLQeCYSygvak52592886 = -310789224;    float myrdWTONMLQeCYSygvak73137532 = -185027687;    float myrdWTONMLQeCYSygvak36051176 = -276024984;    float myrdWTONMLQeCYSygvak5128701 = -164353045;    float myrdWTONMLQeCYSygvak61987728 = -540079872;    float myrdWTONMLQeCYSygvak73155703 = 37243092;    float myrdWTONMLQeCYSygvak94975245 = -320094167;    float myrdWTONMLQeCYSygvak64809772 = -490358251;    float myrdWTONMLQeCYSygvak34141437 = -214674247;    float myrdWTONMLQeCYSygvak68437851 = -304575081;    float myrdWTONMLQeCYSygvak43773993 = -705026010;    float myrdWTONMLQeCYSygvak91202128 = -440510875;    float myrdWTONMLQeCYSygvak91780487 = -911477945;    float myrdWTONMLQeCYSygvak85257785 = -718630128;    float myrdWTONMLQeCYSygvak10371199 = -512793584;    float myrdWTONMLQeCYSygvak15725310 = -850703641;    float myrdWTONMLQeCYSygvak23051939 = -396863273;    float myrdWTONMLQeCYSygvak41942872 = -518599993;    float myrdWTONMLQeCYSygvak43094117 = 29639474;    float myrdWTONMLQeCYSygvak3870859 = -642000767;    float myrdWTONMLQeCYSygvak13406038 = -355620741;    float myrdWTONMLQeCYSygvak96524890 = -746409755;    float myrdWTONMLQeCYSygvak13681732 = 33480320;    float myrdWTONMLQeCYSygvak3167428 = -872058879;    float myrdWTONMLQeCYSygvak27395686 = -650155015;    float myrdWTONMLQeCYSygvak5041657 = 86146924;    float myrdWTONMLQeCYSygvak22904809 = -306829907;    float myrdWTONMLQeCYSygvak53736142 = -775956060;    float myrdWTONMLQeCYSygvak76824075 = -576862750;    float myrdWTONMLQeCYSygvak69499934 = -199767454;    float myrdWTONMLQeCYSygvak75192151 = -489484377;    float myrdWTONMLQeCYSygvak48590126 = -707169347;    float myrdWTONMLQeCYSygvak78615902 = 64253010;    float myrdWTONMLQeCYSygvak73481802 = -541420809;    float myrdWTONMLQeCYSygvak64847385 = -15517496;    float myrdWTONMLQeCYSygvak19011455 = -575771943;    float myrdWTONMLQeCYSygvak24934214 = -220511541;    float myrdWTONMLQeCYSygvak18884745 = -885153537;    float myrdWTONMLQeCYSygvak89260656 = -973477474;    float myrdWTONMLQeCYSygvak77438994 = -155826040;    float myrdWTONMLQeCYSygvak86180774 = 87797625;    float myrdWTONMLQeCYSygvak27490612 = -6998467;    float myrdWTONMLQeCYSygvak67272712 = -800467509;    float myrdWTONMLQeCYSygvak5088579 = -327619701;    float myrdWTONMLQeCYSygvak78331138 = -98400708;    float myrdWTONMLQeCYSygvak75241751 = -904376928;    float myrdWTONMLQeCYSygvak92387077 = -75966931;    float myrdWTONMLQeCYSygvak86226311 = -259395862;     myrdWTONMLQeCYSygvak98968266 = myrdWTONMLQeCYSygvak51196950;     myrdWTONMLQeCYSygvak51196950 = myrdWTONMLQeCYSygvak67544608;     myrdWTONMLQeCYSygvak67544608 = myrdWTONMLQeCYSygvak26410236;     myrdWTONMLQeCYSygvak26410236 = myrdWTONMLQeCYSygvak89961173;     myrdWTONMLQeCYSygvak89961173 = myrdWTONMLQeCYSygvak25155330;     myrdWTONMLQeCYSygvak25155330 = myrdWTONMLQeCYSygvak64712;     myrdWTONMLQeCYSygvak64712 = myrdWTONMLQeCYSygvak47444426;     myrdWTONMLQeCYSygvak47444426 = myrdWTONMLQeCYSygvak62772223;     myrdWTONMLQeCYSygvak62772223 = myrdWTONMLQeCYSygvak27804202;     myrdWTONMLQeCYSygvak27804202 = myrdWTONMLQeCYSygvak2698057;     myrdWTONMLQeCYSygvak2698057 = myrdWTONMLQeCYSygvak2520187;     myrdWTONMLQeCYSygvak2520187 = myrdWTONMLQeCYSygvak76710699;     myrdWTONMLQeCYSygvak76710699 = myrdWTONMLQeCYSygvak47065383;     myrdWTONMLQeCYSygvak47065383 = myrdWTONMLQeCYSygvak93191257;     myrdWTONMLQeCYSygvak93191257 = myrdWTONMLQeCYSygvak93688813;     myrdWTONMLQeCYSygvak93688813 = myrdWTONMLQeCYSygvak85026879;     myrdWTONMLQeCYSygvak85026879 = myrdWTONMLQeCYSygvak5706422;     myrdWTONMLQeCYSygvak5706422 = myrdWTONMLQeCYSygvak97939039;     myrdWTONMLQeCYSygvak97939039 = myrdWTONMLQeCYSygvak87208161;     myrdWTONMLQeCYSygvak87208161 = myrdWTONMLQeCYSygvak25272559;     myrdWTONMLQeCYSygvak25272559 = myrdWTONMLQeCYSygvak93839566;     myrdWTONMLQeCYSygvak93839566 = myrdWTONMLQeCYSygvak89209221;     myrdWTONMLQeCYSygvak89209221 = myrdWTONMLQeCYSygvak94388905;     myrdWTONMLQeCYSygvak94388905 = myrdWTONMLQeCYSygvak31434990;     myrdWTONMLQeCYSygvak31434990 = myrdWTONMLQeCYSygvak25151402;     myrdWTONMLQeCYSygvak25151402 = myrdWTONMLQeCYSygvak91013892;     myrdWTONMLQeCYSygvak91013892 = myrdWTONMLQeCYSygvak31626861;     myrdWTONMLQeCYSygvak31626861 = myrdWTONMLQeCYSygvak3670433;     myrdWTONMLQeCYSygvak3670433 = myrdWTONMLQeCYSygvak71570095;     myrdWTONMLQeCYSygvak71570095 = myrdWTONMLQeCYSygvak36023714;     myrdWTONMLQeCYSygvak36023714 = myrdWTONMLQeCYSygvak17440271;     myrdWTONMLQeCYSygvak17440271 = myrdWTONMLQeCYSygvak92148987;     myrdWTONMLQeCYSygvak92148987 = myrdWTONMLQeCYSygvak60985390;     myrdWTONMLQeCYSygvak60985390 = myrdWTONMLQeCYSygvak24013445;     myrdWTONMLQeCYSygvak24013445 = myrdWTONMLQeCYSygvak51248385;     myrdWTONMLQeCYSygvak51248385 = myrdWTONMLQeCYSygvak50594697;     myrdWTONMLQeCYSygvak50594697 = myrdWTONMLQeCYSygvak81156020;     myrdWTONMLQeCYSygvak81156020 = myrdWTONMLQeCYSygvak92300384;     myrdWTONMLQeCYSygvak92300384 = myrdWTONMLQeCYSygvak1414150;     myrdWTONMLQeCYSygvak1414150 = myrdWTONMLQeCYSygvak73526429;     myrdWTONMLQeCYSygvak73526429 = myrdWTONMLQeCYSygvak22105132;     myrdWTONMLQeCYSygvak22105132 = myrdWTONMLQeCYSygvak66443880;     myrdWTONMLQeCYSygvak66443880 = myrdWTONMLQeCYSygvak84167565;     myrdWTONMLQeCYSygvak84167565 = myrdWTONMLQeCYSygvak71484096;     myrdWTONMLQeCYSygvak71484096 = myrdWTONMLQeCYSygvak77698848;     myrdWTONMLQeCYSygvak77698848 = myrdWTONMLQeCYSygvak48327326;     myrdWTONMLQeCYSygvak48327326 = myrdWTONMLQeCYSygvak21513959;     myrdWTONMLQeCYSygvak21513959 = myrdWTONMLQeCYSygvak56434709;     myrdWTONMLQeCYSygvak56434709 = myrdWTONMLQeCYSygvak55080307;     myrdWTONMLQeCYSygvak55080307 = myrdWTONMLQeCYSygvak92954193;     myrdWTONMLQeCYSygvak92954193 = myrdWTONMLQeCYSygvak62541911;     myrdWTONMLQeCYSygvak62541911 = myrdWTONMLQeCYSygvak52592886;     myrdWTONMLQeCYSygvak52592886 = myrdWTONMLQeCYSygvak73137532;     myrdWTONMLQeCYSygvak73137532 = myrdWTONMLQeCYSygvak36051176;     myrdWTONMLQeCYSygvak36051176 = myrdWTONMLQeCYSygvak5128701;     myrdWTONMLQeCYSygvak5128701 = myrdWTONMLQeCYSygvak61987728;     myrdWTONMLQeCYSygvak61987728 = myrdWTONMLQeCYSygvak73155703;     myrdWTONMLQeCYSygvak73155703 = myrdWTONMLQeCYSygvak94975245;     myrdWTONMLQeCYSygvak94975245 = myrdWTONMLQeCYSygvak64809772;     myrdWTONMLQeCYSygvak64809772 = myrdWTONMLQeCYSygvak34141437;     myrdWTONMLQeCYSygvak34141437 = myrdWTONMLQeCYSygvak68437851;     myrdWTONMLQeCYSygvak68437851 = myrdWTONMLQeCYSygvak43773993;     myrdWTONMLQeCYSygvak43773993 = myrdWTONMLQeCYSygvak91202128;     myrdWTONMLQeCYSygvak91202128 = myrdWTONMLQeCYSygvak91780487;     myrdWTONMLQeCYSygvak91780487 = myrdWTONMLQeCYSygvak85257785;     myrdWTONMLQeCYSygvak85257785 = myrdWTONMLQeCYSygvak10371199;     myrdWTONMLQeCYSygvak10371199 = myrdWTONMLQeCYSygvak15725310;     myrdWTONMLQeCYSygvak15725310 = myrdWTONMLQeCYSygvak23051939;     myrdWTONMLQeCYSygvak23051939 = myrdWTONMLQeCYSygvak41942872;     myrdWTONMLQeCYSygvak41942872 = myrdWTONMLQeCYSygvak43094117;     myrdWTONMLQeCYSygvak43094117 = myrdWTONMLQeCYSygvak3870859;     myrdWTONMLQeCYSygvak3870859 = myrdWTONMLQeCYSygvak13406038;     myrdWTONMLQeCYSygvak13406038 = myrdWTONMLQeCYSygvak96524890;     myrdWTONMLQeCYSygvak96524890 = myrdWTONMLQeCYSygvak13681732;     myrdWTONMLQeCYSygvak13681732 = myrdWTONMLQeCYSygvak3167428;     myrdWTONMLQeCYSygvak3167428 = myrdWTONMLQeCYSygvak27395686;     myrdWTONMLQeCYSygvak27395686 = myrdWTONMLQeCYSygvak5041657;     myrdWTONMLQeCYSygvak5041657 = myrdWTONMLQeCYSygvak22904809;     myrdWTONMLQeCYSygvak22904809 = myrdWTONMLQeCYSygvak53736142;     myrdWTONMLQeCYSygvak53736142 = myrdWTONMLQeCYSygvak76824075;     myrdWTONMLQeCYSygvak76824075 = myrdWTONMLQeCYSygvak69499934;     myrdWTONMLQeCYSygvak69499934 = myrdWTONMLQeCYSygvak75192151;     myrdWTONMLQeCYSygvak75192151 = myrdWTONMLQeCYSygvak48590126;     myrdWTONMLQeCYSygvak48590126 = myrdWTONMLQeCYSygvak78615902;     myrdWTONMLQeCYSygvak78615902 = myrdWTONMLQeCYSygvak73481802;     myrdWTONMLQeCYSygvak73481802 = myrdWTONMLQeCYSygvak64847385;     myrdWTONMLQeCYSygvak64847385 = myrdWTONMLQeCYSygvak19011455;     myrdWTONMLQeCYSygvak19011455 = myrdWTONMLQeCYSygvak24934214;     myrdWTONMLQeCYSygvak24934214 = myrdWTONMLQeCYSygvak18884745;     myrdWTONMLQeCYSygvak18884745 = myrdWTONMLQeCYSygvak89260656;     myrdWTONMLQeCYSygvak89260656 = myrdWTONMLQeCYSygvak77438994;     myrdWTONMLQeCYSygvak77438994 = myrdWTONMLQeCYSygvak86180774;     myrdWTONMLQeCYSygvak86180774 = myrdWTONMLQeCYSygvak27490612;     myrdWTONMLQeCYSygvak27490612 = myrdWTONMLQeCYSygvak67272712;     myrdWTONMLQeCYSygvak67272712 = myrdWTONMLQeCYSygvak5088579;     myrdWTONMLQeCYSygvak5088579 = myrdWTONMLQeCYSygvak78331138;     myrdWTONMLQeCYSygvak78331138 = myrdWTONMLQeCYSygvak75241751;     myrdWTONMLQeCYSygvak75241751 = myrdWTONMLQeCYSygvak92387077;     myrdWTONMLQeCYSygvak92387077 = myrdWTONMLQeCYSygvak86226311;     myrdWTONMLQeCYSygvak86226311 = myrdWTONMLQeCYSygvak98968266;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void btclZBLoBVfhgYHGUvjM67982479() {     float SzjXYqnSLjMtZOOGLtIg98961795 = -131920004;    float SzjXYqnSLjMtZOOGLtIg89769266 = -683972948;    float SzjXYqnSLjMtZOOGLtIg68857820 = -974046391;    float SzjXYqnSLjMtZOOGLtIg11490096 = -661130623;    float SzjXYqnSLjMtZOOGLtIg98801275 = -236507224;    float SzjXYqnSLjMtZOOGLtIg59960762 = -819516135;    float SzjXYqnSLjMtZOOGLtIg68612048 = -164041859;    float SzjXYqnSLjMtZOOGLtIg16658477 = -87429366;    float SzjXYqnSLjMtZOOGLtIg45512388 = -59632262;    float SzjXYqnSLjMtZOOGLtIg24443716 = -275085641;    float SzjXYqnSLjMtZOOGLtIg23018662 = -104408918;    float SzjXYqnSLjMtZOOGLtIg16811864 = -109071575;    float SzjXYqnSLjMtZOOGLtIg8563614 = -468898639;    float SzjXYqnSLjMtZOOGLtIg909604 = -891522906;    float SzjXYqnSLjMtZOOGLtIg38363366 = 5129810;    float SzjXYqnSLjMtZOOGLtIg68253361 = -621739123;    float SzjXYqnSLjMtZOOGLtIg40879639 = -893911878;    float SzjXYqnSLjMtZOOGLtIg28706955 = -350049080;    float SzjXYqnSLjMtZOOGLtIg18367310 = -379533777;    float SzjXYqnSLjMtZOOGLtIg59281924 = -310738913;    float SzjXYqnSLjMtZOOGLtIg4941984 = -485951969;    float SzjXYqnSLjMtZOOGLtIg72135832 = -505034700;    float SzjXYqnSLjMtZOOGLtIg26087619 = -460255793;    float SzjXYqnSLjMtZOOGLtIg38039014 = -835081392;    float SzjXYqnSLjMtZOOGLtIg59877679 = -847189479;    float SzjXYqnSLjMtZOOGLtIg41725479 = -185999580;    float SzjXYqnSLjMtZOOGLtIg61235474 = -6208789;    float SzjXYqnSLjMtZOOGLtIg67030386 = -663897300;    float SzjXYqnSLjMtZOOGLtIg16348880 = -224296354;    float SzjXYqnSLjMtZOOGLtIg47659965 = -580906691;    float SzjXYqnSLjMtZOOGLtIg69436392 = -13894060;    float SzjXYqnSLjMtZOOGLtIg2164129 = -818128481;    float SzjXYqnSLjMtZOOGLtIg77601410 = 37256155;    float SzjXYqnSLjMtZOOGLtIg61268227 = -473218257;    float SzjXYqnSLjMtZOOGLtIg6371483 = -308820506;    float SzjXYqnSLjMtZOOGLtIg94076574 = -593323092;    float SzjXYqnSLjMtZOOGLtIg62536065 = -318236325;    float SzjXYqnSLjMtZOOGLtIg51073700 = -327559220;    float SzjXYqnSLjMtZOOGLtIg27613981 = -210038134;    float SzjXYqnSLjMtZOOGLtIg73287042 = -724573441;    float SzjXYqnSLjMtZOOGLtIg61382097 = -820138680;    float SzjXYqnSLjMtZOOGLtIg7802793 = 2781200;    float SzjXYqnSLjMtZOOGLtIg26727240 = -904018057;    float SzjXYqnSLjMtZOOGLtIg43399464 = -644409319;    float SzjXYqnSLjMtZOOGLtIg55502149 = 79409635;    float SzjXYqnSLjMtZOOGLtIg18126817 = -456584238;    float SzjXYqnSLjMtZOOGLtIg76025118 = -82211524;    float SzjXYqnSLjMtZOOGLtIg96897539 = -14402979;    float SzjXYqnSLjMtZOOGLtIg75093099 = -129209796;    float SzjXYqnSLjMtZOOGLtIg75125730 = -558081580;    float SzjXYqnSLjMtZOOGLtIg6213563 = -448283335;    float SzjXYqnSLjMtZOOGLtIg77089169 = -467525766;    float SzjXYqnSLjMtZOOGLtIg28041235 = -202843232;    float SzjXYqnSLjMtZOOGLtIg83387803 = -716947538;    float SzjXYqnSLjMtZOOGLtIg63040496 = -32924261;    float SzjXYqnSLjMtZOOGLtIg26825963 = -626885304;    float SzjXYqnSLjMtZOOGLtIg63681648 = -123717155;    float SzjXYqnSLjMtZOOGLtIg30818807 = -38964999;    float SzjXYqnSLjMtZOOGLtIg51612417 = -813941144;    float SzjXYqnSLjMtZOOGLtIg57075797 = 49492356;    float SzjXYqnSLjMtZOOGLtIg98725287 = -713307346;    float SzjXYqnSLjMtZOOGLtIg1581662 = -500144560;    float SzjXYqnSLjMtZOOGLtIg309597 = -863133013;    float SzjXYqnSLjMtZOOGLtIg97852422 = -478725571;    float SzjXYqnSLjMtZOOGLtIg55007323 = -161191581;    float SzjXYqnSLjMtZOOGLtIg20854534 = -286280438;    float SzjXYqnSLjMtZOOGLtIg39210453 = -46327730;    float SzjXYqnSLjMtZOOGLtIg47295386 = -995680383;    float SzjXYqnSLjMtZOOGLtIg94538120 = -482702400;    float SzjXYqnSLjMtZOOGLtIg44286792 = -401547099;    float SzjXYqnSLjMtZOOGLtIg5717297 = -203502798;    float SzjXYqnSLjMtZOOGLtIg89805938 = -466352658;    float SzjXYqnSLjMtZOOGLtIg1092975 = -40010947;    float SzjXYqnSLjMtZOOGLtIg45080268 = -654960336;    float SzjXYqnSLjMtZOOGLtIg97899827 = -490600233;    float SzjXYqnSLjMtZOOGLtIg97139190 = -388733170;    float SzjXYqnSLjMtZOOGLtIg45408592 = -601016644;    float SzjXYqnSLjMtZOOGLtIg82688155 = -815846475;    float SzjXYqnSLjMtZOOGLtIg82536865 = -814491028;    float SzjXYqnSLjMtZOOGLtIg41750863 = -290605242;    float SzjXYqnSLjMtZOOGLtIg65700360 = -3788056;    float SzjXYqnSLjMtZOOGLtIg64337935 = -991805810;    float SzjXYqnSLjMtZOOGLtIg91937287 = -434687504;    float SzjXYqnSLjMtZOOGLtIg41223149 = -666214775;    float SzjXYqnSLjMtZOOGLtIg41446402 = -32623357;    float SzjXYqnSLjMtZOOGLtIg92347223 = -546368295;    float SzjXYqnSLjMtZOOGLtIg74122894 = -515285249;    float SzjXYqnSLjMtZOOGLtIg94213607 = -245796307;    float SzjXYqnSLjMtZOOGLtIg98227731 = -340293996;    float SzjXYqnSLjMtZOOGLtIg79545519 = -681935203;    float SzjXYqnSLjMtZOOGLtIg30394926 = -369605937;    float SzjXYqnSLjMtZOOGLtIg31717258 = -179271327;    float SzjXYqnSLjMtZOOGLtIg99461283 = -513618077;    float SzjXYqnSLjMtZOOGLtIg70538183 = -159530490;    float SzjXYqnSLjMtZOOGLtIg74561754 = 88733904;    float SzjXYqnSLjMtZOOGLtIg59800435 = -219994121;    float SzjXYqnSLjMtZOOGLtIg7493196 = -134085788;    float SzjXYqnSLjMtZOOGLtIg28874818 = -325292486;    float SzjXYqnSLjMtZOOGLtIg88392140 = -383217738;    float SzjXYqnSLjMtZOOGLtIg34647616 = -131920004;     SzjXYqnSLjMtZOOGLtIg98961795 = SzjXYqnSLjMtZOOGLtIg89769266;     SzjXYqnSLjMtZOOGLtIg89769266 = SzjXYqnSLjMtZOOGLtIg68857820;     SzjXYqnSLjMtZOOGLtIg68857820 = SzjXYqnSLjMtZOOGLtIg11490096;     SzjXYqnSLjMtZOOGLtIg11490096 = SzjXYqnSLjMtZOOGLtIg98801275;     SzjXYqnSLjMtZOOGLtIg98801275 = SzjXYqnSLjMtZOOGLtIg59960762;     SzjXYqnSLjMtZOOGLtIg59960762 = SzjXYqnSLjMtZOOGLtIg68612048;     SzjXYqnSLjMtZOOGLtIg68612048 = SzjXYqnSLjMtZOOGLtIg16658477;     SzjXYqnSLjMtZOOGLtIg16658477 = SzjXYqnSLjMtZOOGLtIg45512388;     SzjXYqnSLjMtZOOGLtIg45512388 = SzjXYqnSLjMtZOOGLtIg24443716;     SzjXYqnSLjMtZOOGLtIg24443716 = SzjXYqnSLjMtZOOGLtIg23018662;     SzjXYqnSLjMtZOOGLtIg23018662 = SzjXYqnSLjMtZOOGLtIg16811864;     SzjXYqnSLjMtZOOGLtIg16811864 = SzjXYqnSLjMtZOOGLtIg8563614;     SzjXYqnSLjMtZOOGLtIg8563614 = SzjXYqnSLjMtZOOGLtIg909604;     SzjXYqnSLjMtZOOGLtIg909604 = SzjXYqnSLjMtZOOGLtIg38363366;     SzjXYqnSLjMtZOOGLtIg38363366 = SzjXYqnSLjMtZOOGLtIg68253361;     SzjXYqnSLjMtZOOGLtIg68253361 = SzjXYqnSLjMtZOOGLtIg40879639;     SzjXYqnSLjMtZOOGLtIg40879639 = SzjXYqnSLjMtZOOGLtIg28706955;     SzjXYqnSLjMtZOOGLtIg28706955 = SzjXYqnSLjMtZOOGLtIg18367310;     SzjXYqnSLjMtZOOGLtIg18367310 = SzjXYqnSLjMtZOOGLtIg59281924;     SzjXYqnSLjMtZOOGLtIg59281924 = SzjXYqnSLjMtZOOGLtIg4941984;     SzjXYqnSLjMtZOOGLtIg4941984 = SzjXYqnSLjMtZOOGLtIg72135832;     SzjXYqnSLjMtZOOGLtIg72135832 = SzjXYqnSLjMtZOOGLtIg26087619;     SzjXYqnSLjMtZOOGLtIg26087619 = SzjXYqnSLjMtZOOGLtIg38039014;     SzjXYqnSLjMtZOOGLtIg38039014 = SzjXYqnSLjMtZOOGLtIg59877679;     SzjXYqnSLjMtZOOGLtIg59877679 = SzjXYqnSLjMtZOOGLtIg41725479;     SzjXYqnSLjMtZOOGLtIg41725479 = SzjXYqnSLjMtZOOGLtIg61235474;     SzjXYqnSLjMtZOOGLtIg61235474 = SzjXYqnSLjMtZOOGLtIg67030386;     SzjXYqnSLjMtZOOGLtIg67030386 = SzjXYqnSLjMtZOOGLtIg16348880;     SzjXYqnSLjMtZOOGLtIg16348880 = SzjXYqnSLjMtZOOGLtIg47659965;     SzjXYqnSLjMtZOOGLtIg47659965 = SzjXYqnSLjMtZOOGLtIg69436392;     SzjXYqnSLjMtZOOGLtIg69436392 = SzjXYqnSLjMtZOOGLtIg2164129;     SzjXYqnSLjMtZOOGLtIg2164129 = SzjXYqnSLjMtZOOGLtIg77601410;     SzjXYqnSLjMtZOOGLtIg77601410 = SzjXYqnSLjMtZOOGLtIg61268227;     SzjXYqnSLjMtZOOGLtIg61268227 = SzjXYqnSLjMtZOOGLtIg6371483;     SzjXYqnSLjMtZOOGLtIg6371483 = SzjXYqnSLjMtZOOGLtIg94076574;     SzjXYqnSLjMtZOOGLtIg94076574 = SzjXYqnSLjMtZOOGLtIg62536065;     SzjXYqnSLjMtZOOGLtIg62536065 = SzjXYqnSLjMtZOOGLtIg51073700;     SzjXYqnSLjMtZOOGLtIg51073700 = SzjXYqnSLjMtZOOGLtIg27613981;     SzjXYqnSLjMtZOOGLtIg27613981 = SzjXYqnSLjMtZOOGLtIg73287042;     SzjXYqnSLjMtZOOGLtIg73287042 = SzjXYqnSLjMtZOOGLtIg61382097;     SzjXYqnSLjMtZOOGLtIg61382097 = SzjXYqnSLjMtZOOGLtIg7802793;     SzjXYqnSLjMtZOOGLtIg7802793 = SzjXYqnSLjMtZOOGLtIg26727240;     SzjXYqnSLjMtZOOGLtIg26727240 = SzjXYqnSLjMtZOOGLtIg43399464;     SzjXYqnSLjMtZOOGLtIg43399464 = SzjXYqnSLjMtZOOGLtIg55502149;     SzjXYqnSLjMtZOOGLtIg55502149 = SzjXYqnSLjMtZOOGLtIg18126817;     SzjXYqnSLjMtZOOGLtIg18126817 = SzjXYqnSLjMtZOOGLtIg76025118;     SzjXYqnSLjMtZOOGLtIg76025118 = SzjXYqnSLjMtZOOGLtIg96897539;     SzjXYqnSLjMtZOOGLtIg96897539 = SzjXYqnSLjMtZOOGLtIg75093099;     SzjXYqnSLjMtZOOGLtIg75093099 = SzjXYqnSLjMtZOOGLtIg75125730;     SzjXYqnSLjMtZOOGLtIg75125730 = SzjXYqnSLjMtZOOGLtIg6213563;     SzjXYqnSLjMtZOOGLtIg6213563 = SzjXYqnSLjMtZOOGLtIg77089169;     SzjXYqnSLjMtZOOGLtIg77089169 = SzjXYqnSLjMtZOOGLtIg28041235;     SzjXYqnSLjMtZOOGLtIg28041235 = SzjXYqnSLjMtZOOGLtIg83387803;     SzjXYqnSLjMtZOOGLtIg83387803 = SzjXYqnSLjMtZOOGLtIg63040496;     SzjXYqnSLjMtZOOGLtIg63040496 = SzjXYqnSLjMtZOOGLtIg26825963;     SzjXYqnSLjMtZOOGLtIg26825963 = SzjXYqnSLjMtZOOGLtIg63681648;     SzjXYqnSLjMtZOOGLtIg63681648 = SzjXYqnSLjMtZOOGLtIg30818807;     SzjXYqnSLjMtZOOGLtIg30818807 = SzjXYqnSLjMtZOOGLtIg51612417;     SzjXYqnSLjMtZOOGLtIg51612417 = SzjXYqnSLjMtZOOGLtIg57075797;     SzjXYqnSLjMtZOOGLtIg57075797 = SzjXYqnSLjMtZOOGLtIg98725287;     SzjXYqnSLjMtZOOGLtIg98725287 = SzjXYqnSLjMtZOOGLtIg1581662;     SzjXYqnSLjMtZOOGLtIg1581662 = SzjXYqnSLjMtZOOGLtIg309597;     SzjXYqnSLjMtZOOGLtIg309597 = SzjXYqnSLjMtZOOGLtIg97852422;     SzjXYqnSLjMtZOOGLtIg97852422 = SzjXYqnSLjMtZOOGLtIg55007323;     SzjXYqnSLjMtZOOGLtIg55007323 = SzjXYqnSLjMtZOOGLtIg20854534;     SzjXYqnSLjMtZOOGLtIg20854534 = SzjXYqnSLjMtZOOGLtIg39210453;     SzjXYqnSLjMtZOOGLtIg39210453 = SzjXYqnSLjMtZOOGLtIg47295386;     SzjXYqnSLjMtZOOGLtIg47295386 = SzjXYqnSLjMtZOOGLtIg94538120;     SzjXYqnSLjMtZOOGLtIg94538120 = SzjXYqnSLjMtZOOGLtIg44286792;     SzjXYqnSLjMtZOOGLtIg44286792 = SzjXYqnSLjMtZOOGLtIg5717297;     SzjXYqnSLjMtZOOGLtIg5717297 = SzjXYqnSLjMtZOOGLtIg89805938;     SzjXYqnSLjMtZOOGLtIg89805938 = SzjXYqnSLjMtZOOGLtIg1092975;     SzjXYqnSLjMtZOOGLtIg1092975 = SzjXYqnSLjMtZOOGLtIg45080268;     SzjXYqnSLjMtZOOGLtIg45080268 = SzjXYqnSLjMtZOOGLtIg97899827;     SzjXYqnSLjMtZOOGLtIg97899827 = SzjXYqnSLjMtZOOGLtIg97139190;     SzjXYqnSLjMtZOOGLtIg97139190 = SzjXYqnSLjMtZOOGLtIg45408592;     SzjXYqnSLjMtZOOGLtIg45408592 = SzjXYqnSLjMtZOOGLtIg82688155;     SzjXYqnSLjMtZOOGLtIg82688155 = SzjXYqnSLjMtZOOGLtIg82536865;     SzjXYqnSLjMtZOOGLtIg82536865 = SzjXYqnSLjMtZOOGLtIg41750863;     SzjXYqnSLjMtZOOGLtIg41750863 = SzjXYqnSLjMtZOOGLtIg65700360;     SzjXYqnSLjMtZOOGLtIg65700360 = SzjXYqnSLjMtZOOGLtIg64337935;     SzjXYqnSLjMtZOOGLtIg64337935 = SzjXYqnSLjMtZOOGLtIg91937287;     SzjXYqnSLjMtZOOGLtIg91937287 = SzjXYqnSLjMtZOOGLtIg41223149;     SzjXYqnSLjMtZOOGLtIg41223149 = SzjXYqnSLjMtZOOGLtIg41446402;     SzjXYqnSLjMtZOOGLtIg41446402 = SzjXYqnSLjMtZOOGLtIg92347223;     SzjXYqnSLjMtZOOGLtIg92347223 = SzjXYqnSLjMtZOOGLtIg74122894;     SzjXYqnSLjMtZOOGLtIg74122894 = SzjXYqnSLjMtZOOGLtIg94213607;     SzjXYqnSLjMtZOOGLtIg94213607 = SzjXYqnSLjMtZOOGLtIg98227731;     SzjXYqnSLjMtZOOGLtIg98227731 = SzjXYqnSLjMtZOOGLtIg79545519;     SzjXYqnSLjMtZOOGLtIg79545519 = SzjXYqnSLjMtZOOGLtIg30394926;     SzjXYqnSLjMtZOOGLtIg30394926 = SzjXYqnSLjMtZOOGLtIg31717258;     SzjXYqnSLjMtZOOGLtIg31717258 = SzjXYqnSLjMtZOOGLtIg99461283;     SzjXYqnSLjMtZOOGLtIg99461283 = SzjXYqnSLjMtZOOGLtIg70538183;     SzjXYqnSLjMtZOOGLtIg70538183 = SzjXYqnSLjMtZOOGLtIg74561754;     SzjXYqnSLjMtZOOGLtIg74561754 = SzjXYqnSLjMtZOOGLtIg59800435;     SzjXYqnSLjMtZOOGLtIg59800435 = SzjXYqnSLjMtZOOGLtIg7493196;     SzjXYqnSLjMtZOOGLtIg7493196 = SzjXYqnSLjMtZOOGLtIg28874818;     SzjXYqnSLjMtZOOGLtIg28874818 = SzjXYqnSLjMtZOOGLtIg88392140;     SzjXYqnSLjMtZOOGLtIg88392140 = SzjXYqnSLjMtZOOGLtIg34647616;     SzjXYqnSLjMtZOOGLtIg34647616 = SzjXYqnSLjMtZOOGLtIg98961795;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DHqPTjcvhWtqCUEyzjVI22398693() {     float AXifwstLtArTPCkdajif69613462 = -750069134;    float AXifwstLtArTPCkdajif58442784 = -555616247;    float AXifwstLtArTPCkdajif48826740 = -316340851;    float AXifwstLtArTPCkdajif51421383 = -851269012;    float AXifwstLtArTPCkdajif20251598 = -19913571;    float AXifwstLtArTPCkdajif49960525 = -521097287;    float AXifwstLtArTPCkdajif86990943 = -162817569;    float AXifwstLtArTPCkdajif73500857 = -434246456;    float AXifwstLtArTPCkdajif91377442 = -152979591;    float AXifwstLtArTPCkdajif47410176 = -576180991;    float AXifwstLtArTPCkdajif65267822 = -139310848;    float AXifwstLtArTPCkdajif62358435 = -424835368;    float AXifwstLtArTPCkdajif88391000 = -378435697;    float AXifwstLtArTPCkdajif84756637 = -692551116;    float AXifwstLtArTPCkdajif21083386 = -144620193;    float AXifwstLtArTPCkdajif782002 = -191722710;    float AXifwstLtArTPCkdajif34732466 = -803097007;    float AXifwstLtArTPCkdajif49000449 = -339215276;    float AXifwstLtArTPCkdajif75092 = 13278281;    float AXifwstLtArTPCkdajif18068634 = -853105218;    float AXifwstLtArTPCkdajif60077738 = -394631110;    float AXifwstLtArTPCkdajif33531597 = -836320006;    float AXifwstLtArTPCkdajif28031427 = 71901062;    float AXifwstLtArTPCkdajif71888550 = -427482299;    float AXifwstLtArTPCkdajif24002068 = -523094167;    float AXifwstLtArTPCkdajif72323059 = -284284025;    float AXifwstLtArTPCkdajif69496260 = -401386565;    float AXifwstLtArTPCkdajif74812611 = -69021067;    float AXifwstLtArTPCkdajif48491368 = -518654571;    float AXifwstLtArTPCkdajif86832061 = -39301762;    float AXifwstLtArTPCkdajif76508389 = -862327746;    float AXifwstLtArTPCkdajif46675589 = -46425644;    float AXifwstLtArTPCkdajif60546771 = 90294546;    float AXifwstLtArTPCkdajif26633202 = -36747941;    float AXifwstLtArTPCkdajif84395719 = -155767286;    float AXifwstLtArTPCkdajif54069833 = -672549640;    float AXifwstLtArTPCkdajif6327551 = -506829533;    float AXifwstLtArTPCkdajif3866189 = -111672328;    float AXifwstLtArTPCkdajif6147270 = -165766771;    float AXifwstLtArTPCkdajif68390340 = -242181804;    float AXifwstLtArTPCkdajif21455381 = -104157559;    float AXifwstLtArTPCkdajif36285784 = -866607869;    float AXifwstLtArTPCkdajif75694200 = -890772109;    float AXifwstLtArTPCkdajif49907764 = -716384290;    float AXifwstLtArTPCkdajif51456207 = -229236760;    float AXifwstLtArTPCkdajif24532651 = -214921393;    float AXifwstLtArTPCkdajif2156322 = -198281323;    float AXifwstLtArTPCkdajif62511749 = -466533275;    float AXifwstLtArTPCkdajif80534551 = -114973765;    float AXifwstLtArTPCkdajif83553217 = -841090458;    float AXifwstLtArTPCkdajif74402887 = 5144544;    float AXifwstLtArTPCkdajif18960674 = -436745487;    float AXifwstLtArTPCkdajif83116891 = -130641274;    float AXifwstLtArTPCkdajif73615108 = -277829035;    float AXifwstLtArTPCkdajif62320955 = -143865176;    float AXifwstLtArTPCkdajif36081865 = -913749128;    float AXifwstLtArTPCkdajif30411357 = -527517310;    float AXifwstLtArTPCkdajif76938189 = -888858553;    float AXifwstLtArTPCkdajif27419315 = -228174846;    float AXifwstLtArTPCkdajif47928538 = -735629547;    float AXifwstLtArTPCkdajif80464264 = -19710722;    float AXifwstLtArTPCkdajif12178333 = 6203498;    float AXifwstLtArTPCkdajif25009490 = -915591886;    float AXifwstLtArTPCkdajif4545382 = -13677829;    float AXifwstLtArTPCkdajif70901787 = -713853246;    float AXifwstLtArTPCkdajif18592234 = 7114795;    float AXifwstLtArTPCkdajif1811664 = -415129915;    float AXifwstLtArTPCkdajif61757798 = -241687757;    float AXifwstLtArTPCkdajif360918 = -436783831;    float AXifwstLtArTPCkdajif67013552 = -472070554;    float AXifwstLtArTPCkdajif94454450 = -684893178;    float AXifwstLtArTPCkdajif30866277 = -591424680;    float AXifwstLtArTPCkdajif42853180 = -73448506;    float AXifwstLtArTPCkdajif31684751 = -744539915;    float AXifwstLtArTPCkdajif96613252 = -648947660;    float AXifwstLtArTPCkdajif23791954 = -528023241;    float AXifwstLtArTPCkdajif57837397 = -945547898;    float AXifwstLtArTPCkdajif78123663 = -211714648;    float AXifwstLtArTPCkdajif20432343 = -98245540;    float AXifwstLtArTPCkdajif99469416 = -208172775;    float AXifwstLtArTPCkdajif70166737 = 13997298;    float AXifwstLtArTPCkdajif6984511 = -934853290;    float AXifwstLtArTPCkdajif94278060 = -954047302;    float AXifwstLtArTPCkdajif64938151 = -677564114;    float AXifwstLtArTPCkdajif12429174 = 55553694;    float AXifwstLtArTPCkdajif57547716 = -325582259;    float AXifwstLtArTPCkdajif63558698 = -915784370;    float AXifwstLtArTPCkdajif86931663 = -631876419;    float AXifwstLtArTPCkdajif64312247 = -892882766;    float AXifwstLtArTPCkdajif48313854 = -242018158;    float AXifwstLtArTPCkdajif23658477 = -45032330;    float AXifwstLtArTPCkdajif29389362 = -617970981;    float AXifwstLtArTPCkdajif76446873 = -883497483;    float AXifwstLtArTPCkdajif58218731 = -430137224;    float AXifwstLtArTPCkdajif87926075 = -122471082;    float AXifwstLtArTPCkdajif9277049 = -10361057;    float AXifwstLtArTPCkdajif11276295 = -951015984;    float AXifwstLtArTPCkdajif71148819 = -777094280;    float AXifwstLtArTPCkdajif79005977 = 97468956;    float AXifwstLtArTPCkdajif32863974 = -750069134;     AXifwstLtArTPCkdajif69613462 = AXifwstLtArTPCkdajif58442784;     AXifwstLtArTPCkdajif58442784 = AXifwstLtArTPCkdajif48826740;     AXifwstLtArTPCkdajif48826740 = AXifwstLtArTPCkdajif51421383;     AXifwstLtArTPCkdajif51421383 = AXifwstLtArTPCkdajif20251598;     AXifwstLtArTPCkdajif20251598 = AXifwstLtArTPCkdajif49960525;     AXifwstLtArTPCkdajif49960525 = AXifwstLtArTPCkdajif86990943;     AXifwstLtArTPCkdajif86990943 = AXifwstLtArTPCkdajif73500857;     AXifwstLtArTPCkdajif73500857 = AXifwstLtArTPCkdajif91377442;     AXifwstLtArTPCkdajif91377442 = AXifwstLtArTPCkdajif47410176;     AXifwstLtArTPCkdajif47410176 = AXifwstLtArTPCkdajif65267822;     AXifwstLtArTPCkdajif65267822 = AXifwstLtArTPCkdajif62358435;     AXifwstLtArTPCkdajif62358435 = AXifwstLtArTPCkdajif88391000;     AXifwstLtArTPCkdajif88391000 = AXifwstLtArTPCkdajif84756637;     AXifwstLtArTPCkdajif84756637 = AXifwstLtArTPCkdajif21083386;     AXifwstLtArTPCkdajif21083386 = AXifwstLtArTPCkdajif782002;     AXifwstLtArTPCkdajif782002 = AXifwstLtArTPCkdajif34732466;     AXifwstLtArTPCkdajif34732466 = AXifwstLtArTPCkdajif49000449;     AXifwstLtArTPCkdajif49000449 = AXifwstLtArTPCkdajif75092;     AXifwstLtArTPCkdajif75092 = AXifwstLtArTPCkdajif18068634;     AXifwstLtArTPCkdajif18068634 = AXifwstLtArTPCkdajif60077738;     AXifwstLtArTPCkdajif60077738 = AXifwstLtArTPCkdajif33531597;     AXifwstLtArTPCkdajif33531597 = AXifwstLtArTPCkdajif28031427;     AXifwstLtArTPCkdajif28031427 = AXifwstLtArTPCkdajif71888550;     AXifwstLtArTPCkdajif71888550 = AXifwstLtArTPCkdajif24002068;     AXifwstLtArTPCkdajif24002068 = AXifwstLtArTPCkdajif72323059;     AXifwstLtArTPCkdajif72323059 = AXifwstLtArTPCkdajif69496260;     AXifwstLtArTPCkdajif69496260 = AXifwstLtArTPCkdajif74812611;     AXifwstLtArTPCkdajif74812611 = AXifwstLtArTPCkdajif48491368;     AXifwstLtArTPCkdajif48491368 = AXifwstLtArTPCkdajif86832061;     AXifwstLtArTPCkdajif86832061 = AXifwstLtArTPCkdajif76508389;     AXifwstLtArTPCkdajif76508389 = AXifwstLtArTPCkdajif46675589;     AXifwstLtArTPCkdajif46675589 = AXifwstLtArTPCkdajif60546771;     AXifwstLtArTPCkdajif60546771 = AXifwstLtArTPCkdajif26633202;     AXifwstLtArTPCkdajif26633202 = AXifwstLtArTPCkdajif84395719;     AXifwstLtArTPCkdajif84395719 = AXifwstLtArTPCkdajif54069833;     AXifwstLtArTPCkdajif54069833 = AXifwstLtArTPCkdajif6327551;     AXifwstLtArTPCkdajif6327551 = AXifwstLtArTPCkdajif3866189;     AXifwstLtArTPCkdajif3866189 = AXifwstLtArTPCkdajif6147270;     AXifwstLtArTPCkdajif6147270 = AXifwstLtArTPCkdajif68390340;     AXifwstLtArTPCkdajif68390340 = AXifwstLtArTPCkdajif21455381;     AXifwstLtArTPCkdajif21455381 = AXifwstLtArTPCkdajif36285784;     AXifwstLtArTPCkdajif36285784 = AXifwstLtArTPCkdajif75694200;     AXifwstLtArTPCkdajif75694200 = AXifwstLtArTPCkdajif49907764;     AXifwstLtArTPCkdajif49907764 = AXifwstLtArTPCkdajif51456207;     AXifwstLtArTPCkdajif51456207 = AXifwstLtArTPCkdajif24532651;     AXifwstLtArTPCkdajif24532651 = AXifwstLtArTPCkdajif2156322;     AXifwstLtArTPCkdajif2156322 = AXifwstLtArTPCkdajif62511749;     AXifwstLtArTPCkdajif62511749 = AXifwstLtArTPCkdajif80534551;     AXifwstLtArTPCkdajif80534551 = AXifwstLtArTPCkdajif83553217;     AXifwstLtArTPCkdajif83553217 = AXifwstLtArTPCkdajif74402887;     AXifwstLtArTPCkdajif74402887 = AXifwstLtArTPCkdajif18960674;     AXifwstLtArTPCkdajif18960674 = AXifwstLtArTPCkdajif83116891;     AXifwstLtArTPCkdajif83116891 = AXifwstLtArTPCkdajif73615108;     AXifwstLtArTPCkdajif73615108 = AXifwstLtArTPCkdajif62320955;     AXifwstLtArTPCkdajif62320955 = AXifwstLtArTPCkdajif36081865;     AXifwstLtArTPCkdajif36081865 = AXifwstLtArTPCkdajif30411357;     AXifwstLtArTPCkdajif30411357 = AXifwstLtArTPCkdajif76938189;     AXifwstLtArTPCkdajif76938189 = AXifwstLtArTPCkdajif27419315;     AXifwstLtArTPCkdajif27419315 = AXifwstLtArTPCkdajif47928538;     AXifwstLtArTPCkdajif47928538 = AXifwstLtArTPCkdajif80464264;     AXifwstLtArTPCkdajif80464264 = AXifwstLtArTPCkdajif12178333;     AXifwstLtArTPCkdajif12178333 = AXifwstLtArTPCkdajif25009490;     AXifwstLtArTPCkdajif25009490 = AXifwstLtArTPCkdajif4545382;     AXifwstLtArTPCkdajif4545382 = AXifwstLtArTPCkdajif70901787;     AXifwstLtArTPCkdajif70901787 = AXifwstLtArTPCkdajif18592234;     AXifwstLtArTPCkdajif18592234 = AXifwstLtArTPCkdajif1811664;     AXifwstLtArTPCkdajif1811664 = AXifwstLtArTPCkdajif61757798;     AXifwstLtArTPCkdajif61757798 = AXifwstLtArTPCkdajif360918;     AXifwstLtArTPCkdajif360918 = AXifwstLtArTPCkdajif67013552;     AXifwstLtArTPCkdajif67013552 = AXifwstLtArTPCkdajif94454450;     AXifwstLtArTPCkdajif94454450 = AXifwstLtArTPCkdajif30866277;     AXifwstLtArTPCkdajif30866277 = AXifwstLtArTPCkdajif42853180;     AXifwstLtArTPCkdajif42853180 = AXifwstLtArTPCkdajif31684751;     AXifwstLtArTPCkdajif31684751 = AXifwstLtArTPCkdajif96613252;     AXifwstLtArTPCkdajif96613252 = AXifwstLtArTPCkdajif23791954;     AXifwstLtArTPCkdajif23791954 = AXifwstLtArTPCkdajif57837397;     AXifwstLtArTPCkdajif57837397 = AXifwstLtArTPCkdajif78123663;     AXifwstLtArTPCkdajif78123663 = AXifwstLtArTPCkdajif20432343;     AXifwstLtArTPCkdajif20432343 = AXifwstLtArTPCkdajif99469416;     AXifwstLtArTPCkdajif99469416 = AXifwstLtArTPCkdajif70166737;     AXifwstLtArTPCkdajif70166737 = AXifwstLtArTPCkdajif6984511;     AXifwstLtArTPCkdajif6984511 = AXifwstLtArTPCkdajif94278060;     AXifwstLtArTPCkdajif94278060 = AXifwstLtArTPCkdajif64938151;     AXifwstLtArTPCkdajif64938151 = AXifwstLtArTPCkdajif12429174;     AXifwstLtArTPCkdajif12429174 = AXifwstLtArTPCkdajif57547716;     AXifwstLtArTPCkdajif57547716 = AXifwstLtArTPCkdajif63558698;     AXifwstLtArTPCkdajif63558698 = AXifwstLtArTPCkdajif86931663;     AXifwstLtArTPCkdajif86931663 = AXifwstLtArTPCkdajif64312247;     AXifwstLtArTPCkdajif64312247 = AXifwstLtArTPCkdajif48313854;     AXifwstLtArTPCkdajif48313854 = AXifwstLtArTPCkdajif23658477;     AXifwstLtArTPCkdajif23658477 = AXifwstLtArTPCkdajif29389362;     AXifwstLtArTPCkdajif29389362 = AXifwstLtArTPCkdajif76446873;     AXifwstLtArTPCkdajif76446873 = AXifwstLtArTPCkdajif58218731;     AXifwstLtArTPCkdajif58218731 = AXifwstLtArTPCkdajif87926075;     AXifwstLtArTPCkdajif87926075 = AXifwstLtArTPCkdajif9277049;     AXifwstLtArTPCkdajif9277049 = AXifwstLtArTPCkdajif11276295;     AXifwstLtArTPCkdajif11276295 = AXifwstLtArTPCkdajif71148819;     AXifwstLtArTPCkdajif71148819 = AXifwstLtArTPCkdajif79005977;     AXifwstLtArTPCkdajif79005977 = AXifwstLtArTPCkdajif32863974;     AXifwstLtArTPCkdajif32863974 = AXifwstLtArTPCkdajif69613462;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void aHYIJqWyewVSiWAVktjk29057437() {     float quGYlJqPLjDwYhqhmXuV69606991 = -622593275;    float quGYlJqPLjDwYhqhmXuV97015100 = -598211372;    float quGYlJqPLjDwYhqhmXuV50139952 = -273469465;    float quGYlJqPLjDwYhqhmXuV36501244 = -671809842;    float quGYlJqPLjDwYhqhmXuV29091700 = -488268893;    float quGYlJqPLjDwYhqhmXuV84765957 = -220530098;    float quGYlJqPLjDwYhqhmXuV55538279 = -967354036;    float quGYlJqPLjDwYhqhmXuV42714908 = -138423773;    float quGYlJqPLjDwYhqhmXuV74117606 = -966625661;    float quGYlJqPLjDwYhqhmXuV44049690 = -212987863;    float quGYlJqPLjDwYhqhmXuV85588428 = -98782920;    float quGYlJqPLjDwYhqhmXuV76650112 = -160313731;    float quGYlJqPLjDwYhqhmXuV20243915 = -400094172;    float quGYlJqPLjDwYhqhmXuV38600857 = 62295833;    float quGYlJqPLjDwYhqhmXuV66255494 = -107333045;    float quGYlJqPLjDwYhqhmXuV75346548 = -524518361;    float quGYlJqPLjDwYhqhmXuV90585225 = -622711577;    float quGYlJqPLjDwYhqhmXuV72000982 = -736286898;    float quGYlJqPLjDwYhqhmXuV20503361 = -604703985;    float quGYlJqPLjDwYhqhmXuV90142396 = -365129669;    float quGYlJqPLjDwYhqhmXuV39747162 = -105097483;    float quGYlJqPLjDwYhqhmXuV11827863 = -246311889;    float quGYlJqPLjDwYhqhmXuV64909824 = -387056780;    float quGYlJqPLjDwYhqhmXuV15538659 = -308402821;    float quGYlJqPLjDwYhqhmXuV52444757 = -949788020;    float quGYlJqPLjDwYhqhmXuV88897136 = -192489953;    float quGYlJqPLjDwYhqhmXuV39717842 = -702186277;    float quGYlJqPLjDwYhqhmXuV10216136 = -677988055;    float quGYlJqPLjDwYhqhmXuV61169815 = -64724886;    float quGYlJqPLjDwYhqhmXuV62921931 = -814733136;    float quGYlJqPLjDwYhqhmXuV9921068 = -149420980;    float quGYlJqPLjDwYhqhmXuV31399447 = -438247405;    float quGYlJqPLjDwYhqhmXuV45999195 = -111649670;    float quGYlJqPLjDwYhqhmXuV26916039 = 86570326;    float quGYlJqPLjDwYhqhmXuV66753757 = -415081210;    float quGYlJqPLjDwYhqhmXuV96898022 = -752315386;    float quGYlJqPLjDwYhqhmXuV18268919 = -606482911;    float quGYlJqPLjDwYhqhmXuV73783868 = -106935007;    float quGYlJqPLjDwYhqhmXuV41460866 = -878448187;    float quGYlJqPLjDwYhqhmXuV40263233 = -951613490;    float quGYlJqPLjDwYhqhmXuV9311049 = -192101457;    float quGYlJqPLjDwYhqhmXuV21983445 = 39600049;    float quGYlJqPLjDwYhqhmXuV35977560 = -349902363;    float quGYlJqPLjDwYhqhmXuV9139663 = -273348734;    float quGYlJqPLjDwYhqhmXuV35474260 = -702496161;    float quGYlJqPLjDwYhqhmXuV64960619 = -26966064;    float quGYlJqPLjDwYhqhmXuV29854114 = -679561945;    float quGYlJqPLjDwYhqhmXuV37895331 = 24705369;    float quGYlJqPLjDwYhqhmXuV99192940 = -778737626;    float quGYlJqPLjDwYhqhmXuV3598640 = -428115345;    float quGYlJqPLjDwYhqhmXuV87662257 = -673410463;    float quGYlJqPLjDwYhqhmXuV33507931 = -818891236;    float quGYlJqPLjDwYhqhmXuV58565240 = -22695283;    float quGYlJqPLjDwYhqhmXuV83865378 = -809748886;    float quGYlJqPLjDwYhqhmXuV89310275 = 99235547;    float quGYlJqPLjDwYhqhmXuV57779128 = -276281387;    float quGYlJqPLjDwYhqhmXuV32105277 = -111154593;    float quGYlJqPLjDwYhqhmXuV34601293 = -965066644;    float quGYlJqPLjDwYhqhmXuV84056486 = -722021823;    float quGYlJqPLjDwYhqhmXuV40194564 = -195778940;    float quGYlJqPLjDwYhqhmXuV45048115 = -518343821;    float quGYlJqPLjDwYhqhmXuV45322143 = -189365981;    float quGYlJqPLjDwYhqhmXuV81545093 = 26301112;    float quGYlJqPLjDwYhqhmXuV11195676 = -51892526;    float quGYlJqPLjDwYhqhmXuV34128622 = 36433117;    float quGYlJqPLjDwYhqhmXuV54188981 = -660535515;    float quGYlJqPLjDwYhqhmXuV30650917 = 51335939;    float quGYlJqPLjDwYhqhmXuV93327875 = -386664499;    float quGYlJqPLjDwYhqhmXuV71847100 = -522622957;    float quGYlJqPLjDwYhqhmXuV69357471 = -355017660;    float quGYlJqPLjDwYhqhmXuV57077630 = -918035450;    float quGYlJqPLjDwYhqhmXuV16801357 = -415776571;    float quGYlJqPLjDwYhqhmXuV30540116 = -857838711;    float quGYlJqPLjDwYhqhmXuV80240128 = -653090496;    float quGYlJqPLjDwYhqhmXuV80831347 = -73028212;    float quGYlJqPLjDwYhqhmXuV17763717 = -44697532;    float quGYlJqPLjDwYhqhmXuV75850302 = -896409527;    float quGYlJqPLjDwYhqhmXuV55770161 = -13708047;    float quGYlJqPLjDwYhqhmXuV80064399 = -605906660;    float quGYlJqPLjDwYhqhmXuV87484137 = -822821957;    float quGYlJqPLjDwYhqhmXuV59043022 = -512928008;    float quGYlJqPLjDwYhqhmXuV1822512 = -626891647;    float quGYlJqPLjDwYhqhmXuV11023196 = -899250429;    float quGYlJqPLjDwYhqhmXuV57571175 = -636609542;    float quGYlJqPLjDwYhqhmXuV75259674 = -41322673;    float quGYlJqPLjDwYhqhmXuV76413137 = -330529745;    float quGYlJqPLjDwYhqhmXuV72834207 = -315552123;    float quGYlJqPLjDwYhqhmXuV62133816 = -301900784;    float quGYlJqPLjDwYhqhmXuV37605764 = 87334779;    float quGYlJqPLjDwYhqhmXuV8974629 = -38799824;    float quGYlJqPLjDwYhqhmXuV64792746 = -541160793;    float quGYlJqPLjDwYhqhmXuV83667625 = -641416268;    float quGYlJqPLjDwYhqhmXuV89727382 = -384913185;    float quGYlJqPLjDwYhqhmXuV1266303 = -582669247;    float quGYlJqPLjDwYhqhmXuV95215117 = -333269669;    float quGYlJqPLjDwYhqhmXuV63988905 = 97264523;    float quGYlJqPLjDwYhqhmXuV40438352 = -986701064;    float quGYlJqPLjDwYhqhmXuV24781885 = -198009838;    float quGYlJqPLjDwYhqhmXuV75011040 = -209781851;    float quGYlJqPLjDwYhqhmXuV81285278 = -622593275;     quGYlJqPLjDwYhqhmXuV69606991 = quGYlJqPLjDwYhqhmXuV97015100;     quGYlJqPLjDwYhqhmXuV97015100 = quGYlJqPLjDwYhqhmXuV50139952;     quGYlJqPLjDwYhqhmXuV50139952 = quGYlJqPLjDwYhqhmXuV36501244;     quGYlJqPLjDwYhqhmXuV36501244 = quGYlJqPLjDwYhqhmXuV29091700;     quGYlJqPLjDwYhqhmXuV29091700 = quGYlJqPLjDwYhqhmXuV84765957;     quGYlJqPLjDwYhqhmXuV84765957 = quGYlJqPLjDwYhqhmXuV55538279;     quGYlJqPLjDwYhqhmXuV55538279 = quGYlJqPLjDwYhqhmXuV42714908;     quGYlJqPLjDwYhqhmXuV42714908 = quGYlJqPLjDwYhqhmXuV74117606;     quGYlJqPLjDwYhqhmXuV74117606 = quGYlJqPLjDwYhqhmXuV44049690;     quGYlJqPLjDwYhqhmXuV44049690 = quGYlJqPLjDwYhqhmXuV85588428;     quGYlJqPLjDwYhqhmXuV85588428 = quGYlJqPLjDwYhqhmXuV76650112;     quGYlJqPLjDwYhqhmXuV76650112 = quGYlJqPLjDwYhqhmXuV20243915;     quGYlJqPLjDwYhqhmXuV20243915 = quGYlJqPLjDwYhqhmXuV38600857;     quGYlJqPLjDwYhqhmXuV38600857 = quGYlJqPLjDwYhqhmXuV66255494;     quGYlJqPLjDwYhqhmXuV66255494 = quGYlJqPLjDwYhqhmXuV75346548;     quGYlJqPLjDwYhqhmXuV75346548 = quGYlJqPLjDwYhqhmXuV90585225;     quGYlJqPLjDwYhqhmXuV90585225 = quGYlJqPLjDwYhqhmXuV72000982;     quGYlJqPLjDwYhqhmXuV72000982 = quGYlJqPLjDwYhqhmXuV20503361;     quGYlJqPLjDwYhqhmXuV20503361 = quGYlJqPLjDwYhqhmXuV90142396;     quGYlJqPLjDwYhqhmXuV90142396 = quGYlJqPLjDwYhqhmXuV39747162;     quGYlJqPLjDwYhqhmXuV39747162 = quGYlJqPLjDwYhqhmXuV11827863;     quGYlJqPLjDwYhqhmXuV11827863 = quGYlJqPLjDwYhqhmXuV64909824;     quGYlJqPLjDwYhqhmXuV64909824 = quGYlJqPLjDwYhqhmXuV15538659;     quGYlJqPLjDwYhqhmXuV15538659 = quGYlJqPLjDwYhqhmXuV52444757;     quGYlJqPLjDwYhqhmXuV52444757 = quGYlJqPLjDwYhqhmXuV88897136;     quGYlJqPLjDwYhqhmXuV88897136 = quGYlJqPLjDwYhqhmXuV39717842;     quGYlJqPLjDwYhqhmXuV39717842 = quGYlJqPLjDwYhqhmXuV10216136;     quGYlJqPLjDwYhqhmXuV10216136 = quGYlJqPLjDwYhqhmXuV61169815;     quGYlJqPLjDwYhqhmXuV61169815 = quGYlJqPLjDwYhqhmXuV62921931;     quGYlJqPLjDwYhqhmXuV62921931 = quGYlJqPLjDwYhqhmXuV9921068;     quGYlJqPLjDwYhqhmXuV9921068 = quGYlJqPLjDwYhqhmXuV31399447;     quGYlJqPLjDwYhqhmXuV31399447 = quGYlJqPLjDwYhqhmXuV45999195;     quGYlJqPLjDwYhqhmXuV45999195 = quGYlJqPLjDwYhqhmXuV26916039;     quGYlJqPLjDwYhqhmXuV26916039 = quGYlJqPLjDwYhqhmXuV66753757;     quGYlJqPLjDwYhqhmXuV66753757 = quGYlJqPLjDwYhqhmXuV96898022;     quGYlJqPLjDwYhqhmXuV96898022 = quGYlJqPLjDwYhqhmXuV18268919;     quGYlJqPLjDwYhqhmXuV18268919 = quGYlJqPLjDwYhqhmXuV73783868;     quGYlJqPLjDwYhqhmXuV73783868 = quGYlJqPLjDwYhqhmXuV41460866;     quGYlJqPLjDwYhqhmXuV41460866 = quGYlJqPLjDwYhqhmXuV40263233;     quGYlJqPLjDwYhqhmXuV40263233 = quGYlJqPLjDwYhqhmXuV9311049;     quGYlJqPLjDwYhqhmXuV9311049 = quGYlJqPLjDwYhqhmXuV21983445;     quGYlJqPLjDwYhqhmXuV21983445 = quGYlJqPLjDwYhqhmXuV35977560;     quGYlJqPLjDwYhqhmXuV35977560 = quGYlJqPLjDwYhqhmXuV9139663;     quGYlJqPLjDwYhqhmXuV9139663 = quGYlJqPLjDwYhqhmXuV35474260;     quGYlJqPLjDwYhqhmXuV35474260 = quGYlJqPLjDwYhqhmXuV64960619;     quGYlJqPLjDwYhqhmXuV64960619 = quGYlJqPLjDwYhqhmXuV29854114;     quGYlJqPLjDwYhqhmXuV29854114 = quGYlJqPLjDwYhqhmXuV37895331;     quGYlJqPLjDwYhqhmXuV37895331 = quGYlJqPLjDwYhqhmXuV99192940;     quGYlJqPLjDwYhqhmXuV99192940 = quGYlJqPLjDwYhqhmXuV3598640;     quGYlJqPLjDwYhqhmXuV3598640 = quGYlJqPLjDwYhqhmXuV87662257;     quGYlJqPLjDwYhqhmXuV87662257 = quGYlJqPLjDwYhqhmXuV33507931;     quGYlJqPLjDwYhqhmXuV33507931 = quGYlJqPLjDwYhqhmXuV58565240;     quGYlJqPLjDwYhqhmXuV58565240 = quGYlJqPLjDwYhqhmXuV83865378;     quGYlJqPLjDwYhqhmXuV83865378 = quGYlJqPLjDwYhqhmXuV89310275;     quGYlJqPLjDwYhqhmXuV89310275 = quGYlJqPLjDwYhqhmXuV57779128;     quGYlJqPLjDwYhqhmXuV57779128 = quGYlJqPLjDwYhqhmXuV32105277;     quGYlJqPLjDwYhqhmXuV32105277 = quGYlJqPLjDwYhqhmXuV34601293;     quGYlJqPLjDwYhqhmXuV34601293 = quGYlJqPLjDwYhqhmXuV84056486;     quGYlJqPLjDwYhqhmXuV84056486 = quGYlJqPLjDwYhqhmXuV40194564;     quGYlJqPLjDwYhqhmXuV40194564 = quGYlJqPLjDwYhqhmXuV45048115;     quGYlJqPLjDwYhqhmXuV45048115 = quGYlJqPLjDwYhqhmXuV45322143;     quGYlJqPLjDwYhqhmXuV45322143 = quGYlJqPLjDwYhqhmXuV81545093;     quGYlJqPLjDwYhqhmXuV81545093 = quGYlJqPLjDwYhqhmXuV11195676;     quGYlJqPLjDwYhqhmXuV11195676 = quGYlJqPLjDwYhqhmXuV34128622;     quGYlJqPLjDwYhqhmXuV34128622 = quGYlJqPLjDwYhqhmXuV54188981;     quGYlJqPLjDwYhqhmXuV54188981 = quGYlJqPLjDwYhqhmXuV30650917;     quGYlJqPLjDwYhqhmXuV30650917 = quGYlJqPLjDwYhqhmXuV93327875;     quGYlJqPLjDwYhqhmXuV93327875 = quGYlJqPLjDwYhqhmXuV71847100;     quGYlJqPLjDwYhqhmXuV71847100 = quGYlJqPLjDwYhqhmXuV69357471;     quGYlJqPLjDwYhqhmXuV69357471 = quGYlJqPLjDwYhqhmXuV57077630;     quGYlJqPLjDwYhqhmXuV57077630 = quGYlJqPLjDwYhqhmXuV16801357;     quGYlJqPLjDwYhqhmXuV16801357 = quGYlJqPLjDwYhqhmXuV30540116;     quGYlJqPLjDwYhqhmXuV30540116 = quGYlJqPLjDwYhqhmXuV80240128;     quGYlJqPLjDwYhqhmXuV80240128 = quGYlJqPLjDwYhqhmXuV80831347;     quGYlJqPLjDwYhqhmXuV80831347 = quGYlJqPLjDwYhqhmXuV17763717;     quGYlJqPLjDwYhqhmXuV17763717 = quGYlJqPLjDwYhqhmXuV75850302;     quGYlJqPLjDwYhqhmXuV75850302 = quGYlJqPLjDwYhqhmXuV55770161;     quGYlJqPLjDwYhqhmXuV55770161 = quGYlJqPLjDwYhqhmXuV80064399;     quGYlJqPLjDwYhqhmXuV80064399 = quGYlJqPLjDwYhqhmXuV87484137;     quGYlJqPLjDwYhqhmXuV87484137 = quGYlJqPLjDwYhqhmXuV59043022;     quGYlJqPLjDwYhqhmXuV59043022 = quGYlJqPLjDwYhqhmXuV1822512;     quGYlJqPLjDwYhqhmXuV1822512 = quGYlJqPLjDwYhqhmXuV11023196;     quGYlJqPLjDwYhqhmXuV11023196 = quGYlJqPLjDwYhqhmXuV57571175;     quGYlJqPLjDwYhqhmXuV57571175 = quGYlJqPLjDwYhqhmXuV75259674;     quGYlJqPLjDwYhqhmXuV75259674 = quGYlJqPLjDwYhqhmXuV76413137;     quGYlJqPLjDwYhqhmXuV76413137 = quGYlJqPLjDwYhqhmXuV72834207;     quGYlJqPLjDwYhqhmXuV72834207 = quGYlJqPLjDwYhqhmXuV62133816;     quGYlJqPLjDwYhqhmXuV62133816 = quGYlJqPLjDwYhqhmXuV37605764;     quGYlJqPLjDwYhqhmXuV37605764 = quGYlJqPLjDwYhqhmXuV8974629;     quGYlJqPLjDwYhqhmXuV8974629 = quGYlJqPLjDwYhqhmXuV64792746;     quGYlJqPLjDwYhqhmXuV64792746 = quGYlJqPLjDwYhqhmXuV83667625;     quGYlJqPLjDwYhqhmXuV83667625 = quGYlJqPLjDwYhqhmXuV89727382;     quGYlJqPLjDwYhqhmXuV89727382 = quGYlJqPLjDwYhqhmXuV1266303;     quGYlJqPLjDwYhqhmXuV1266303 = quGYlJqPLjDwYhqhmXuV95215117;     quGYlJqPLjDwYhqhmXuV95215117 = quGYlJqPLjDwYhqhmXuV63988905;     quGYlJqPLjDwYhqhmXuV63988905 = quGYlJqPLjDwYhqhmXuV40438352;     quGYlJqPLjDwYhqhmXuV40438352 = quGYlJqPLjDwYhqhmXuV24781885;     quGYlJqPLjDwYhqhmXuV24781885 = quGYlJqPLjDwYhqhmXuV75011040;     quGYlJqPLjDwYhqhmXuV75011040 = quGYlJqPLjDwYhqhmXuV81285278;     quGYlJqPLjDwYhqhmXuV81285278 = quGYlJqPLjDwYhqhmXuV69606991;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void TABbNVnizxPVtlvhvMYL83473650() {     float LpgOyvUcOzDSahNLWRuT40258658 = -140742406;    float LpgOyvUcOzDSahNLWRuT65688618 = -469854671;    float LpgOyvUcOzDSahNLWRuT30108871 = -715763924;    float LpgOyvUcOzDSahNLWRuT76432530 = -861948232;    float LpgOyvUcOzDSahNLWRuT50542021 = -271675240;    float LpgOyvUcOzDSahNLWRuT74765719 = 77888750;    float LpgOyvUcOzDSahNLWRuT73917175 = -966129746;    float LpgOyvUcOzDSahNLWRuT99557288 = -485240863;    float LpgOyvUcOzDSahNLWRuT19982661 = 40027011;    float LpgOyvUcOzDSahNLWRuT67016150 = -514083213;    float LpgOyvUcOzDSahNLWRuT27837589 = -133684849;    float LpgOyvUcOzDSahNLWRuT22196683 = -476077524;    float LpgOyvUcOzDSahNLWRuT71302 = -309631230;    float LpgOyvUcOzDSahNLWRuT22447891 = -838732377;    float LpgOyvUcOzDSahNLWRuT48975514 = -257083048;    float LpgOyvUcOzDSahNLWRuT7875189 = -94501948;    float LpgOyvUcOzDSahNLWRuT84438052 = -531896707;    float LpgOyvUcOzDSahNLWRuT92294476 = -725453094;    float LpgOyvUcOzDSahNLWRuT2211143 = -211891927;    float LpgOyvUcOzDSahNLWRuT48929106 = -907495974;    float LpgOyvUcOzDSahNLWRuT94882916 = -13776624;    float LpgOyvUcOzDSahNLWRuT73223627 = -577597195;    float LpgOyvUcOzDSahNLWRuT66853632 = -954899924;    float LpgOyvUcOzDSahNLWRuT49388196 = 99196273;    float LpgOyvUcOzDSahNLWRuT16569146 = -625692708;    float LpgOyvUcOzDSahNLWRuT19494717 = -290774398;    float LpgOyvUcOzDSahNLWRuT47978628 = 2635947;    float LpgOyvUcOzDSahNLWRuT17998362 = -83111823;    float LpgOyvUcOzDSahNLWRuT93312302 = -359083103;    float LpgOyvUcOzDSahNLWRuT2094027 = -273128206;    float LpgOyvUcOzDSahNLWRuT16993065 = -997854666;    float LpgOyvUcOzDSahNLWRuT75910907 = -766544568;    float LpgOyvUcOzDSahNLWRuT28944556 = -58611279;    float LpgOyvUcOzDSahNLWRuT92281014 = -576959358;    float LpgOyvUcOzDSahNLWRuT44777994 = -262027990;    float LpgOyvUcOzDSahNLWRuT56891282 = -831541934;    float LpgOyvUcOzDSahNLWRuT62060404 = -795076119;    float LpgOyvUcOzDSahNLWRuT26576357 = -991048114;    float LpgOyvUcOzDSahNLWRuT19994155 = -834176823;    float LpgOyvUcOzDSahNLWRuT35366531 = -469221853;    float LpgOyvUcOzDSahNLWRuT69384332 = -576120336;    float LpgOyvUcOzDSahNLWRuT50466436 = -829789021;    float LpgOyvUcOzDSahNLWRuT84944520 = -336656415;    float LpgOyvUcOzDSahNLWRuT15647963 = -345323705;    float LpgOyvUcOzDSahNLWRuT31428319 = 88857444;    float LpgOyvUcOzDSahNLWRuT71366454 = -885303219;    float LpgOyvUcOzDSahNLWRuT55985317 = -795631745;    float LpgOyvUcOzDSahNLWRuT3509541 = -427424926;    float LpgOyvUcOzDSahNLWRuT4634393 = -764501596;    float LpgOyvUcOzDSahNLWRuT12026127 = -711124223;    float LpgOyvUcOzDSahNLWRuT55851581 = -219982585;    float LpgOyvUcOzDSahNLWRuT75379435 = -788110957;    float LpgOyvUcOzDSahNLWRuT13640897 = 49506675;    float LpgOyvUcOzDSahNLWRuT74092683 = -370630383;    float LpgOyvUcOzDSahNLWRuT88590733 = -11705368;    float LpgOyvUcOzDSahNLWRuT67035030 = -563145211;    float LpgOyvUcOzDSahNLWRuT98834985 = -514954748;    float LpgOyvUcOzDSahNLWRuT80720674 = -714960198;    float LpgOyvUcOzDSahNLWRuT59863384 = -136255524;    float LpgOyvUcOzDSahNLWRuT31047305 = -980900843;    float LpgOyvUcOzDSahNLWRuT26787092 = -924747198;    float LpgOyvUcOzDSahNLWRuT55918813 = -783017923;    float LpgOyvUcOzDSahNLWRuT6244986 = -26157761;    float LpgOyvUcOzDSahNLWRuT17888634 = -686844784;    float LpgOyvUcOzDSahNLWRuT50023086 = -516228548;    float LpgOyvUcOzDSahNLWRuT51926681 = -367140282;    float LpgOyvUcOzDSahNLWRuT93252127 = -317466245;    float LpgOyvUcOzDSahNLWRuT7790288 = -732671873;    float LpgOyvUcOzDSahNLWRuT77669897 = -476704388;    float LpgOyvUcOzDSahNLWRuT92084232 = -425541115;    float LpgOyvUcOzDSahNLWRuT45814784 = -299425829;    float LpgOyvUcOzDSahNLWRuT57861695 = -540848593;    float LpgOyvUcOzDSahNLWRuT72300321 = -891276271;    float LpgOyvUcOzDSahNLWRuT66844611 = -742670075;    float LpgOyvUcOzDSahNLWRuT79544773 = -231375639;    float LpgOyvUcOzDSahNLWRuT44416480 = -183987603;    float LpgOyvUcOzDSahNLWRuT88279107 = -140940780;    float LpgOyvUcOzDSahNLWRuT51205670 = -509576220;    float LpgOyvUcOzDSahNLWRuT17959878 = -989661172;    float LpgOyvUcOzDSahNLWRuT45202691 = -740389490;    float LpgOyvUcOzDSahNLWRuT63509399 = -495142654;    float LpgOyvUcOzDSahNLWRuT44469087 = -569939127;    float LpgOyvUcOzDSahNLWRuT13363969 = -318610228;    float LpgOyvUcOzDSahNLWRuT81286176 = -647958880;    float LpgOyvUcOzDSahNLWRuT46242445 = 46854378;    float LpgOyvUcOzDSahNLWRuT41613629 = -109743709;    float LpgOyvUcOzDSahNLWRuT62270010 = -716051244;    float LpgOyvUcOzDSahNLWRuT54851872 = -687980896;    float LpgOyvUcOzDSahNLWRuT3690281 = -465253991;    float LpgOyvUcOzDSahNLWRuT77742963 = -698882779;    float LpgOyvUcOzDSahNLWRuT58056296 = -216587187;    float LpgOyvUcOzDSahNLWRuT81339729 = 19884078;    float LpgOyvUcOzDSahNLWRuT66712972 = -754792591;    float LpgOyvUcOzDSahNLWRuT88946850 = -853275981;    float LpgOyvUcOzDSahNLWRuT8579440 = -544474656;    float LpgOyvUcOzDSahNLWRuT13465519 = -793102413;    float LpgOyvUcOzDSahNLWRuT44221450 = -703631261;    float LpgOyvUcOzDSahNLWRuT67055886 = -649811632;    float LpgOyvUcOzDSahNLWRuT65624877 = -829095158;    float LpgOyvUcOzDSahNLWRuT79501637 = -140742406;     LpgOyvUcOzDSahNLWRuT40258658 = LpgOyvUcOzDSahNLWRuT65688618;     LpgOyvUcOzDSahNLWRuT65688618 = LpgOyvUcOzDSahNLWRuT30108871;     LpgOyvUcOzDSahNLWRuT30108871 = LpgOyvUcOzDSahNLWRuT76432530;     LpgOyvUcOzDSahNLWRuT76432530 = LpgOyvUcOzDSahNLWRuT50542021;     LpgOyvUcOzDSahNLWRuT50542021 = LpgOyvUcOzDSahNLWRuT74765719;     LpgOyvUcOzDSahNLWRuT74765719 = LpgOyvUcOzDSahNLWRuT73917175;     LpgOyvUcOzDSahNLWRuT73917175 = LpgOyvUcOzDSahNLWRuT99557288;     LpgOyvUcOzDSahNLWRuT99557288 = LpgOyvUcOzDSahNLWRuT19982661;     LpgOyvUcOzDSahNLWRuT19982661 = LpgOyvUcOzDSahNLWRuT67016150;     LpgOyvUcOzDSahNLWRuT67016150 = LpgOyvUcOzDSahNLWRuT27837589;     LpgOyvUcOzDSahNLWRuT27837589 = LpgOyvUcOzDSahNLWRuT22196683;     LpgOyvUcOzDSahNLWRuT22196683 = LpgOyvUcOzDSahNLWRuT71302;     LpgOyvUcOzDSahNLWRuT71302 = LpgOyvUcOzDSahNLWRuT22447891;     LpgOyvUcOzDSahNLWRuT22447891 = LpgOyvUcOzDSahNLWRuT48975514;     LpgOyvUcOzDSahNLWRuT48975514 = LpgOyvUcOzDSahNLWRuT7875189;     LpgOyvUcOzDSahNLWRuT7875189 = LpgOyvUcOzDSahNLWRuT84438052;     LpgOyvUcOzDSahNLWRuT84438052 = LpgOyvUcOzDSahNLWRuT92294476;     LpgOyvUcOzDSahNLWRuT92294476 = LpgOyvUcOzDSahNLWRuT2211143;     LpgOyvUcOzDSahNLWRuT2211143 = LpgOyvUcOzDSahNLWRuT48929106;     LpgOyvUcOzDSahNLWRuT48929106 = LpgOyvUcOzDSahNLWRuT94882916;     LpgOyvUcOzDSahNLWRuT94882916 = LpgOyvUcOzDSahNLWRuT73223627;     LpgOyvUcOzDSahNLWRuT73223627 = LpgOyvUcOzDSahNLWRuT66853632;     LpgOyvUcOzDSahNLWRuT66853632 = LpgOyvUcOzDSahNLWRuT49388196;     LpgOyvUcOzDSahNLWRuT49388196 = LpgOyvUcOzDSahNLWRuT16569146;     LpgOyvUcOzDSahNLWRuT16569146 = LpgOyvUcOzDSahNLWRuT19494717;     LpgOyvUcOzDSahNLWRuT19494717 = LpgOyvUcOzDSahNLWRuT47978628;     LpgOyvUcOzDSahNLWRuT47978628 = LpgOyvUcOzDSahNLWRuT17998362;     LpgOyvUcOzDSahNLWRuT17998362 = LpgOyvUcOzDSahNLWRuT93312302;     LpgOyvUcOzDSahNLWRuT93312302 = LpgOyvUcOzDSahNLWRuT2094027;     LpgOyvUcOzDSahNLWRuT2094027 = LpgOyvUcOzDSahNLWRuT16993065;     LpgOyvUcOzDSahNLWRuT16993065 = LpgOyvUcOzDSahNLWRuT75910907;     LpgOyvUcOzDSahNLWRuT75910907 = LpgOyvUcOzDSahNLWRuT28944556;     LpgOyvUcOzDSahNLWRuT28944556 = LpgOyvUcOzDSahNLWRuT92281014;     LpgOyvUcOzDSahNLWRuT92281014 = LpgOyvUcOzDSahNLWRuT44777994;     LpgOyvUcOzDSahNLWRuT44777994 = LpgOyvUcOzDSahNLWRuT56891282;     LpgOyvUcOzDSahNLWRuT56891282 = LpgOyvUcOzDSahNLWRuT62060404;     LpgOyvUcOzDSahNLWRuT62060404 = LpgOyvUcOzDSahNLWRuT26576357;     LpgOyvUcOzDSahNLWRuT26576357 = LpgOyvUcOzDSahNLWRuT19994155;     LpgOyvUcOzDSahNLWRuT19994155 = LpgOyvUcOzDSahNLWRuT35366531;     LpgOyvUcOzDSahNLWRuT35366531 = LpgOyvUcOzDSahNLWRuT69384332;     LpgOyvUcOzDSahNLWRuT69384332 = LpgOyvUcOzDSahNLWRuT50466436;     LpgOyvUcOzDSahNLWRuT50466436 = LpgOyvUcOzDSahNLWRuT84944520;     LpgOyvUcOzDSahNLWRuT84944520 = LpgOyvUcOzDSahNLWRuT15647963;     LpgOyvUcOzDSahNLWRuT15647963 = LpgOyvUcOzDSahNLWRuT31428319;     LpgOyvUcOzDSahNLWRuT31428319 = LpgOyvUcOzDSahNLWRuT71366454;     LpgOyvUcOzDSahNLWRuT71366454 = LpgOyvUcOzDSahNLWRuT55985317;     LpgOyvUcOzDSahNLWRuT55985317 = LpgOyvUcOzDSahNLWRuT3509541;     LpgOyvUcOzDSahNLWRuT3509541 = LpgOyvUcOzDSahNLWRuT4634393;     LpgOyvUcOzDSahNLWRuT4634393 = LpgOyvUcOzDSahNLWRuT12026127;     LpgOyvUcOzDSahNLWRuT12026127 = LpgOyvUcOzDSahNLWRuT55851581;     LpgOyvUcOzDSahNLWRuT55851581 = LpgOyvUcOzDSahNLWRuT75379435;     LpgOyvUcOzDSahNLWRuT75379435 = LpgOyvUcOzDSahNLWRuT13640897;     LpgOyvUcOzDSahNLWRuT13640897 = LpgOyvUcOzDSahNLWRuT74092683;     LpgOyvUcOzDSahNLWRuT74092683 = LpgOyvUcOzDSahNLWRuT88590733;     LpgOyvUcOzDSahNLWRuT88590733 = LpgOyvUcOzDSahNLWRuT67035030;     LpgOyvUcOzDSahNLWRuT67035030 = LpgOyvUcOzDSahNLWRuT98834985;     LpgOyvUcOzDSahNLWRuT98834985 = LpgOyvUcOzDSahNLWRuT80720674;     LpgOyvUcOzDSahNLWRuT80720674 = LpgOyvUcOzDSahNLWRuT59863384;     LpgOyvUcOzDSahNLWRuT59863384 = LpgOyvUcOzDSahNLWRuT31047305;     LpgOyvUcOzDSahNLWRuT31047305 = LpgOyvUcOzDSahNLWRuT26787092;     LpgOyvUcOzDSahNLWRuT26787092 = LpgOyvUcOzDSahNLWRuT55918813;     LpgOyvUcOzDSahNLWRuT55918813 = LpgOyvUcOzDSahNLWRuT6244986;     LpgOyvUcOzDSahNLWRuT6244986 = LpgOyvUcOzDSahNLWRuT17888634;     LpgOyvUcOzDSahNLWRuT17888634 = LpgOyvUcOzDSahNLWRuT50023086;     LpgOyvUcOzDSahNLWRuT50023086 = LpgOyvUcOzDSahNLWRuT51926681;     LpgOyvUcOzDSahNLWRuT51926681 = LpgOyvUcOzDSahNLWRuT93252127;     LpgOyvUcOzDSahNLWRuT93252127 = LpgOyvUcOzDSahNLWRuT7790288;     LpgOyvUcOzDSahNLWRuT7790288 = LpgOyvUcOzDSahNLWRuT77669897;     LpgOyvUcOzDSahNLWRuT77669897 = LpgOyvUcOzDSahNLWRuT92084232;     LpgOyvUcOzDSahNLWRuT92084232 = LpgOyvUcOzDSahNLWRuT45814784;     LpgOyvUcOzDSahNLWRuT45814784 = LpgOyvUcOzDSahNLWRuT57861695;     LpgOyvUcOzDSahNLWRuT57861695 = LpgOyvUcOzDSahNLWRuT72300321;     LpgOyvUcOzDSahNLWRuT72300321 = LpgOyvUcOzDSahNLWRuT66844611;     LpgOyvUcOzDSahNLWRuT66844611 = LpgOyvUcOzDSahNLWRuT79544773;     LpgOyvUcOzDSahNLWRuT79544773 = LpgOyvUcOzDSahNLWRuT44416480;     LpgOyvUcOzDSahNLWRuT44416480 = LpgOyvUcOzDSahNLWRuT88279107;     LpgOyvUcOzDSahNLWRuT88279107 = LpgOyvUcOzDSahNLWRuT51205670;     LpgOyvUcOzDSahNLWRuT51205670 = LpgOyvUcOzDSahNLWRuT17959878;     LpgOyvUcOzDSahNLWRuT17959878 = LpgOyvUcOzDSahNLWRuT45202691;     LpgOyvUcOzDSahNLWRuT45202691 = LpgOyvUcOzDSahNLWRuT63509399;     LpgOyvUcOzDSahNLWRuT63509399 = LpgOyvUcOzDSahNLWRuT44469087;     LpgOyvUcOzDSahNLWRuT44469087 = LpgOyvUcOzDSahNLWRuT13363969;     LpgOyvUcOzDSahNLWRuT13363969 = LpgOyvUcOzDSahNLWRuT81286176;     LpgOyvUcOzDSahNLWRuT81286176 = LpgOyvUcOzDSahNLWRuT46242445;     LpgOyvUcOzDSahNLWRuT46242445 = LpgOyvUcOzDSahNLWRuT41613629;     LpgOyvUcOzDSahNLWRuT41613629 = LpgOyvUcOzDSahNLWRuT62270010;     LpgOyvUcOzDSahNLWRuT62270010 = LpgOyvUcOzDSahNLWRuT54851872;     LpgOyvUcOzDSahNLWRuT54851872 = LpgOyvUcOzDSahNLWRuT3690281;     LpgOyvUcOzDSahNLWRuT3690281 = LpgOyvUcOzDSahNLWRuT77742963;     LpgOyvUcOzDSahNLWRuT77742963 = LpgOyvUcOzDSahNLWRuT58056296;     LpgOyvUcOzDSahNLWRuT58056296 = LpgOyvUcOzDSahNLWRuT81339729;     LpgOyvUcOzDSahNLWRuT81339729 = LpgOyvUcOzDSahNLWRuT66712972;     LpgOyvUcOzDSahNLWRuT66712972 = LpgOyvUcOzDSahNLWRuT88946850;     LpgOyvUcOzDSahNLWRuT88946850 = LpgOyvUcOzDSahNLWRuT8579440;     LpgOyvUcOzDSahNLWRuT8579440 = LpgOyvUcOzDSahNLWRuT13465519;     LpgOyvUcOzDSahNLWRuT13465519 = LpgOyvUcOzDSahNLWRuT44221450;     LpgOyvUcOzDSahNLWRuT44221450 = LpgOyvUcOzDSahNLWRuT67055886;     LpgOyvUcOzDSahNLWRuT67055886 = LpgOyvUcOzDSahNLWRuT65624877;     LpgOyvUcOzDSahNLWRuT65624877 = LpgOyvUcOzDSahNLWRuT79501637;     LpgOyvUcOzDSahNLWRuT79501637 = LpgOyvUcOzDSahNLWRuT40258658;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ogTztBNEKNAlAtqeVmoc95366272() {     float swXbHBzNsaYRHLNdKkYS96650123 = -451393787;    float swXbHBzNsaYRHLNdKkYS93717141 = -15670460;    float swXbHBzNsaYRHLNdKkYS64313574 = -84225769;    float swXbHBzNsaYRHLNdKkYS26672908 = -454184578;    float swXbHBzNsaYRHLNdKkYS99507598 = -84672303;    float swXbHBzNsaYRHLNdKkYS5122113 = -285431109;    float swXbHBzNsaYRHLNdKkYS26657888 = -340099898;    float swXbHBzNsaYRHLNdKkYS80220357 = -633544321;    float swXbHBzNsaYRHLNdKkYS88370106 = -694263553;    float swXbHBzNsaYRHLNdKkYS19285382 = -600328747;    float swXbHBzNsaYRHLNdKkYS96661431 = -180895875;    float swXbHBzNsaYRHLNdKkYS91744854 = -113613522;    float swXbHBzNsaYRHLNdKkYS93106181 = 8062823;    float swXbHBzNsaYRHLNdKkYS85720985 = -541228239;    float swXbHBzNsaYRHLNdKkYS71921817 = -751254177;    float swXbHBzNsaYRHLNdKkYS15728860 = -178146555;    float swXbHBzNsaYRHLNdKkYS26277151 = -868687583;    float swXbHBzNsaYRHLNdKkYS64823852 = -327326150;    float swXbHBzNsaYRHLNdKkYS82898525 = -321122092;    float swXbHBzNsaYRHLNdKkYS18274795 = -260124156;    float swXbHBzNsaYRHLNdKkYS54887301 = -187051349;    float swXbHBzNsaYRHLNdKkYS82321603 = 34975660;    float swXbHBzNsaYRHLNdKkYS95453379 = -210141332;    float swXbHBzNsaYRHLNdKkYS82554288 = -51206364;    float swXbHBzNsaYRHLNdKkYS1626948 = -562587134;    float swXbHBzNsaYRHLNdKkYS89297558 = -113041479;    float swXbHBzNsaYRHLNdKkYS96241791 = -939417121;    float swXbHBzNsaYRHLNdKkYS60525705 = -872252622;    float swXbHBzNsaYRHLNdKkYS54790441 = -902709302;    float swXbHBzNsaYRHLNdKkYS72492052 = -6929127;    float swXbHBzNsaYRHLNdKkYS97564232 = -132836349;    float swXbHBzNsaYRHLNdKkYS23751284 = -853859848;    float swXbHBzNsaYRHLNdKkYS23330636 = -322024725;    float swXbHBzNsaYRHLNdKkYS80889910 = -223506900;    float swXbHBzNsaYRHLNdKkYS6897769 = -631872411;    float swXbHBzNsaYRHLNdKkYS15210387 = 79235078;    float swXbHBzNsaYRHLNdKkYS8427777 = -439944139;    float swXbHBzNsaYRHLNdKkYS72907788 = -949673830;    float swXbHBzNsaYRHLNdKkYS81710247 = -108272466;    float swXbHBzNsaYRHLNdKkYS91467119 = -18788197;    float swXbHBzNsaYRHLNdKkYS97063903 = 39156693;    float swXbHBzNsaYRHLNdKkYS50742079 = -277392709;    float swXbHBzNsaYRHLNdKkYS42457095 = -488155701;    float swXbHBzNsaYRHLNdKkYS77770206 = -955150937;    float swXbHBzNsaYRHLNdKkYS92437076 = -800842386;    float swXbHBzNsaYRHLNdKkYS18287511 = -599973606;    float swXbHBzNsaYRHLNdKkYS1700776 = -614709426;    float swXbHBzNsaYRHLNdKkYS62075419 = -932291655;    float swXbHBzNsaYRHLNdKkYS6331015 = -462243548;    float swXbHBzNsaYRHLNdKkYS90512692 = -913398676;    float swXbHBzNsaYRHLNdKkYS18330171 = -791063491;    float swXbHBzNsaYRHLNdKkYS43104523 = -294240063;    float swXbHBzNsaYRHLNdKkYS24745559 = -286508468;    float swXbHBzNsaYRHLNdKkYS49005113 = -23336217;    float swXbHBzNsaYRHLNdKkYS40478972 = -498152430;    float swXbHBzNsaYRHLNdKkYS14328520 = -386369448;    float swXbHBzNsaYRHLNdKkYS98263761 = -805529128;    float swXbHBzNsaYRHLNdKkYS81759286 = 66980595;    float swXbHBzNsaYRHLNdKkYS25045961 = -891597445;    float swXbHBzNsaYRHLNdKkYS10210041 = -971630824;    float swXbHBzNsaYRHLNdKkYS8880322 = -346013989;    float swXbHBzNsaYRHLNdKkYS66132183 = -467847276;    float swXbHBzNsaYRHLNdKkYS25429917 = -730835020;    float swXbHBzNsaYRHLNdKkYS15878055 = -587334426;    float swXbHBzNsaYRHLNdKkYS21721149 = -367492399;    float swXbHBzNsaYRHLNdKkYS72910147 = -327036028;    float swXbHBzNsaYRHLNdKkYS68414219 = -791588798;    float swXbHBzNsaYRHLNdKkYS12216272 = -768430278;    float swXbHBzNsaYRHLNdKkYS78823216 = -909355829;    float swXbHBzNsaYRHLNdKkYS56711431 = -730489255;    float swXbHBzNsaYRHLNdKkYS7301084 = -738202417;    float swXbHBzNsaYRHLNdKkYS53369362 = -919013754;    float swXbHBzNsaYRHLNdKkYS83113605 = -119053685;    float swXbHBzNsaYRHLNdKkYS91431406 = -202333895;    float swXbHBzNsaYRHLNdKkYS21210891 = -199280850;    float swXbHBzNsaYRHLNdKkYS4145222 = -909658641;    float swXbHBzNsaYRHLNdKkYS39864509 = -476868639;    float swXbHBzNsaYRHLNdKkYS17683173 = -254990395;    float swXbHBzNsaYRHLNdKkYS90117211 = -250363978;    float swXbHBzNsaYRHLNdKkYS83339436 = -962613528;    float swXbHBzNsaYRHLNdKkYS87596783 = -498332054;    float swXbHBzNsaYRHLNdKkYS34166372 = 92874534;    float swXbHBzNsaYRHLNdKkYS54194690 = -310009075;    float swXbHBzNsaYRHLNdKkYS64277748 = -989310626;    float swXbHBzNsaYRHLNdKkYS54161881 = -215865637;    float swXbHBzNsaYRHLNdKkYS54459709 = -838596286;    float swXbHBzNsaYRHLNdKkYS99005724 = -467351380;    float swXbHBzNsaYRHLNdKkYS74325522 = -198688509;    float swXbHBzNsaYRHLNdKkYS40410938 = -725354470;    float swXbHBzNsaYRHLNdKkYS92569249 = -145502964;    float swXbHBzNsaYRHLNdKkYS16946625 = -115235795;    float swXbHBzNsaYRHLNdKkYS26668491 = -406924734;    float swXbHBzNsaYRHLNdKkYS47861828 = 41923614;    float swXbHBzNsaYRHLNdKkYS71500207 = -136641643;    float swXbHBzNsaYRHLNdKkYS82586797 = -672774209;    float swXbHBzNsaYRHLNdKkYS30931721 = -492996031;    float swXbHBzNsaYRHLNdKkYS25312163 = -546557690;    float swXbHBzNsaYRHLNdKkYS26579040 = -900821275;    float swXbHBzNsaYRHLNdKkYS56049057 = -487658539;    float swXbHBzNsaYRHLNdKkYS19526930 = -451393787;     swXbHBzNsaYRHLNdKkYS96650123 = swXbHBzNsaYRHLNdKkYS93717141;     swXbHBzNsaYRHLNdKkYS93717141 = swXbHBzNsaYRHLNdKkYS64313574;     swXbHBzNsaYRHLNdKkYS64313574 = swXbHBzNsaYRHLNdKkYS26672908;     swXbHBzNsaYRHLNdKkYS26672908 = swXbHBzNsaYRHLNdKkYS99507598;     swXbHBzNsaYRHLNdKkYS99507598 = swXbHBzNsaYRHLNdKkYS5122113;     swXbHBzNsaYRHLNdKkYS5122113 = swXbHBzNsaYRHLNdKkYS26657888;     swXbHBzNsaYRHLNdKkYS26657888 = swXbHBzNsaYRHLNdKkYS80220357;     swXbHBzNsaYRHLNdKkYS80220357 = swXbHBzNsaYRHLNdKkYS88370106;     swXbHBzNsaYRHLNdKkYS88370106 = swXbHBzNsaYRHLNdKkYS19285382;     swXbHBzNsaYRHLNdKkYS19285382 = swXbHBzNsaYRHLNdKkYS96661431;     swXbHBzNsaYRHLNdKkYS96661431 = swXbHBzNsaYRHLNdKkYS91744854;     swXbHBzNsaYRHLNdKkYS91744854 = swXbHBzNsaYRHLNdKkYS93106181;     swXbHBzNsaYRHLNdKkYS93106181 = swXbHBzNsaYRHLNdKkYS85720985;     swXbHBzNsaYRHLNdKkYS85720985 = swXbHBzNsaYRHLNdKkYS71921817;     swXbHBzNsaYRHLNdKkYS71921817 = swXbHBzNsaYRHLNdKkYS15728860;     swXbHBzNsaYRHLNdKkYS15728860 = swXbHBzNsaYRHLNdKkYS26277151;     swXbHBzNsaYRHLNdKkYS26277151 = swXbHBzNsaYRHLNdKkYS64823852;     swXbHBzNsaYRHLNdKkYS64823852 = swXbHBzNsaYRHLNdKkYS82898525;     swXbHBzNsaYRHLNdKkYS82898525 = swXbHBzNsaYRHLNdKkYS18274795;     swXbHBzNsaYRHLNdKkYS18274795 = swXbHBzNsaYRHLNdKkYS54887301;     swXbHBzNsaYRHLNdKkYS54887301 = swXbHBzNsaYRHLNdKkYS82321603;     swXbHBzNsaYRHLNdKkYS82321603 = swXbHBzNsaYRHLNdKkYS95453379;     swXbHBzNsaYRHLNdKkYS95453379 = swXbHBzNsaYRHLNdKkYS82554288;     swXbHBzNsaYRHLNdKkYS82554288 = swXbHBzNsaYRHLNdKkYS1626948;     swXbHBzNsaYRHLNdKkYS1626948 = swXbHBzNsaYRHLNdKkYS89297558;     swXbHBzNsaYRHLNdKkYS89297558 = swXbHBzNsaYRHLNdKkYS96241791;     swXbHBzNsaYRHLNdKkYS96241791 = swXbHBzNsaYRHLNdKkYS60525705;     swXbHBzNsaYRHLNdKkYS60525705 = swXbHBzNsaYRHLNdKkYS54790441;     swXbHBzNsaYRHLNdKkYS54790441 = swXbHBzNsaYRHLNdKkYS72492052;     swXbHBzNsaYRHLNdKkYS72492052 = swXbHBzNsaYRHLNdKkYS97564232;     swXbHBzNsaYRHLNdKkYS97564232 = swXbHBzNsaYRHLNdKkYS23751284;     swXbHBzNsaYRHLNdKkYS23751284 = swXbHBzNsaYRHLNdKkYS23330636;     swXbHBzNsaYRHLNdKkYS23330636 = swXbHBzNsaYRHLNdKkYS80889910;     swXbHBzNsaYRHLNdKkYS80889910 = swXbHBzNsaYRHLNdKkYS6897769;     swXbHBzNsaYRHLNdKkYS6897769 = swXbHBzNsaYRHLNdKkYS15210387;     swXbHBzNsaYRHLNdKkYS15210387 = swXbHBzNsaYRHLNdKkYS8427777;     swXbHBzNsaYRHLNdKkYS8427777 = swXbHBzNsaYRHLNdKkYS72907788;     swXbHBzNsaYRHLNdKkYS72907788 = swXbHBzNsaYRHLNdKkYS81710247;     swXbHBzNsaYRHLNdKkYS81710247 = swXbHBzNsaYRHLNdKkYS91467119;     swXbHBzNsaYRHLNdKkYS91467119 = swXbHBzNsaYRHLNdKkYS97063903;     swXbHBzNsaYRHLNdKkYS97063903 = swXbHBzNsaYRHLNdKkYS50742079;     swXbHBzNsaYRHLNdKkYS50742079 = swXbHBzNsaYRHLNdKkYS42457095;     swXbHBzNsaYRHLNdKkYS42457095 = swXbHBzNsaYRHLNdKkYS77770206;     swXbHBzNsaYRHLNdKkYS77770206 = swXbHBzNsaYRHLNdKkYS92437076;     swXbHBzNsaYRHLNdKkYS92437076 = swXbHBzNsaYRHLNdKkYS18287511;     swXbHBzNsaYRHLNdKkYS18287511 = swXbHBzNsaYRHLNdKkYS1700776;     swXbHBzNsaYRHLNdKkYS1700776 = swXbHBzNsaYRHLNdKkYS62075419;     swXbHBzNsaYRHLNdKkYS62075419 = swXbHBzNsaYRHLNdKkYS6331015;     swXbHBzNsaYRHLNdKkYS6331015 = swXbHBzNsaYRHLNdKkYS90512692;     swXbHBzNsaYRHLNdKkYS90512692 = swXbHBzNsaYRHLNdKkYS18330171;     swXbHBzNsaYRHLNdKkYS18330171 = swXbHBzNsaYRHLNdKkYS43104523;     swXbHBzNsaYRHLNdKkYS43104523 = swXbHBzNsaYRHLNdKkYS24745559;     swXbHBzNsaYRHLNdKkYS24745559 = swXbHBzNsaYRHLNdKkYS49005113;     swXbHBzNsaYRHLNdKkYS49005113 = swXbHBzNsaYRHLNdKkYS40478972;     swXbHBzNsaYRHLNdKkYS40478972 = swXbHBzNsaYRHLNdKkYS14328520;     swXbHBzNsaYRHLNdKkYS14328520 = swXbHBzNsaYRHLNdKkYS98263761;     swXbHBzNsaYRHLNdKkYS98263761 = swXbHBzNsaYRHLNdKkYS81759286;     swXbHBzNsaYRHLNdKkYS81759286 = swXbHBzNsaYRHLNdKkYS25045961;     swXbHBzNsaYRHLNdKkYS25045961 = swXbHBzNsaYRHLNdKkYS10210041;     swXbHBzNsaYRHLNdKkYS10210041 = swXbHBzNsaYRHLNdKkYS8880322;     swXbHBzNsaYRHLNdKkYS8880322 = swXbHBzNsaYRHLNdKkYS66132183;     swXbHBzNsaYRHLNdKkYS66132183 = swXbHBzNsaYRHLNdKkYS25429917;     swXbHBzNsaYRHLNdKkYS25429917 = swXbHBzNsaYRHLNdKkYS15878055;     swXbHBzNsaYRHLNdKkYS15878055 = swXbHBzNsaYRHLNdKkYS21721149;     swXbHBzNsaYRHLNdKkYS21721149 = swXbHBzNsaYRHLNdKkYS72910147;     swXbHBzNsaYRHLNdKkYS72910147 = swXbHBzNsaYRHLNdKkYS68414219;     swXbHBzNsaYRHLNdKkYS68414219 = swXbHBzNsaYRHLNdKkYS12216272;     swXbHBzNsaYRHLNdKkYS12216272 = swXbHBzNsaYRHLNdKkYS78823216;     swXbHBzNsaYRHLNdKkYS78823216 = swXbHBzNsaYRHLNdKkYS56711431;     swXbHBzNsaYRHLNdKkYS56711431 = swXbHBzNsaYRHLNdKkYS7301084;     swXbHBzNsaYRHLNdKkYS7301084 = swXbHBzNsaYRHLNdKkYS53369362;     swXbHBzNsaYRHLNdKkYS53369362 = swXbHBzNsaYRHLNdKkYS83113605;     swXbHBzNsaYRHLNdKkYS83113605 = swXbHBzNsaYRHLNdKkYS91431406;     swXbHBzNsaYRHLNdKkYS91431406 = swXbHBzNsaYRHLNdKkYS21210891;     swXbHBzNsaYRHLNdKkYS21210891 = swXbHBzNsaYRHLNdKkYS4145222;     swXbHBzNsaYRHLNdKkYS4145222 = swXbHBzNsaYRHLNdKkYS39864509;     swXbHBzNsaYRHLNdKkYS39864509 = swXbHBzNsaYRHLNdKkYS17683173;     swXbHBzNsaYRHLNdKkYS17683173 = swXbHBzNsaYRHLNdKkYS90117211;     swXbHBzNsaYRHLNdKkYS90117211 = swXbHBzNsaYRHLNdKkYS83339436;     swXbHBzNsaYRHLNdKkYS83339436 = swXbHBzNsaYRHLNdKkYS87596783;     swXbHBzNsaYRHLNdKkYS87596783 = swXbHBzNsaYRHLNdKkYS34166372;     swXbHBzNsaYRHLNdKkYS34166372 = swXbHBzNsaYRHLNdKkYS54194690;     swXbHBzNsaYRHLNdKkYS54194690 = swXbHBzNsaYRHLNdKkYS64277748;     swXbHBzNsaYRHLNdKkYS64277748 = swXbHBzNsaYRHLNdKkYS54161881;     swXbHBzNsaYRHLNdKkYS54161881 = swXbHBzNsaYRHLNdKkYS54459709;     swXbHBzNsaYRHLNdKkYS54459709 = swXbHBzNsaYRHLNdKkYS99005724;     swXbHBzNsaYRHLNdKkYS99005724 = swXbHBzNsaYRHLNdKkYS74325522;     swXbHBzNsaYRHLNdKkYS74325522 = swXbHBzNsaYRHLNdKkYS40410938;     swXbHBzNsaYRHLNdKkYS40410938 = swXbHBzNsaYRHLNdKkYS92569249;     swXbHBzNsaYRHLNdKkYS92569249 = swXbHBzNsaYRHLNdKkYS16946625;     swXbHBzNsaYRHLNdKkYS16946625 = swXbHBzNsaYRHLNdKkYS26668491;     swXbHBzNsaYRHLNdKkYS26668491 = swXbHBzNsaYRHLNdKkYS47861828;     swXbHBzNsaYRHLNdKkYS47861828 = swXbHBzNsaYRHLNdKkYS71500207;     swXbHBzNsaYRHLNdKkYS71500207 = swXbHBzNsaYRHLNdKkYS82586797;     swXbHBzNsaYRHLNdKkYS82586797 = swXbHBzNsaYRHLNdKkYS30931721;     swXbHBzNsaYRHLNdKkYS30931721 = swXbHBzNsaYRHLNdKkYS25312163;     swXbHBzNsaYRHLNdKkYS25312163 = swXbHBzNsaYRHLNdKkYS26579040;     swXbHBzNsaYRHLNdKkYS26579040 = swXbHBzNsaYRHLNdKkYS56049057;     swXbHBzNsaYRHLNdKkYS56049057 = swXbHBzNsaYRHLNdKkYS19526930;     swXbHBzNsaYRHLNdKkYS19526930 = swXbHBzNsaYRHLNdKkYS96650123;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void YVvZyPPOHtjEYkdQdXiW49782486() {     float SlngVylzRNZdWQedlumX67301790 = 30457083;    float SlngVylzRNZdWQedlumX62390659 = -987313759;    float SlngVylzRNZdWQedlumX44282493 = -526520228;    float SlngVylzRNZdWQedlumX66604195 = -644322967;    float SlngVylzRNZdWQedlumX20957921 = -968078650;    float SlngVylzRNZdWQedlumX95121875 = 12987739;    float SlngVylzRNZdWQedlumX45036784 = -338875608;    float SlngVylzRNZdWQedlumX37062738 = -980361411;    float SlngVylzRNZdWQedlumX34235161 = -787610882;    float SlngVylzRNZdWQedlumX42251842 = -901424098;    float SlngVylzRNZdWQedlumX38910592 = -215797804;    float SlngVylzRNZdWQedlumX37291426 = -429377315;    float SlngVylzRNZdWQedlumX72933568 = 98525764;    float SlngVylzRNZdWQedlumX69568018 = -342256449;    float SlngVylzRNZdWQedlumX54641837 = -901004180;    float SlngVylzRNZdWQedlumX48257500 = -848130142;    float SlngVylzRNZdWQedlumX20129978 = -777872713;    float SlngVylzRNZdWQedlumX85117346 = -316492346;    float SlngVylzRNZdWQedlumX64606307 = 71689966;    float SlngVylzRNZdWQedlumX77061503 = -802490461;    float SlngVylzRNZdWQedlumX10023056 = -95730490;    float SlngVylzRNZdWQedlumX43717368 = -296309645;    float SlngVylzRNZdWQedlumX97397187 = -777984477;    float SlngVylzRNZdWQedlumX16403825 = -743607270;    float SlngVylzRNZdWQedlumX65751336 = -238491821;    float SlngVylzRNZdWQedlumX19895140 = -211325924;    float SlngVylzRNZdWQedlumX4502577 = -234594896;    float SlngVylzRNZdWQedlumX68307930 = -277376390;    float SlngVylzRNZdWQedlumX86932928 = -97067519;    float SlngVylzRNZdWQedlumX11664148 = -565324198;    float SlngVylzRNZdWQedlumX4636229 = -981270035;    float SlngVylzRNZdWQedlumX68262744 = -82157011;    float SlngVylzRNZdWQedlumX6275997 = -268986334;    float SlngVylzRNZdWQedlumX46254885 = -887036584;    float SlngVylzRNZdWQedlumX84922005 = -478819191;    float SlngVylzRNZdWQedlumX75203645 = 8530;    float SlngVylzRNZdWQedlumX52219262 = -628537346;    float SlngVylzRNZdWQedlumX25700277 = -733786937;    float SlngVylzRNZdWQedlumX60243536 = -64001103;    float SlngVylzRNZdWQedlumX86570417 = -636396560;    float SlngVylzRNZdWQedlumX57137187 = -344862185;    float SlngVylzRNZdWQedlumX79225070 = -46781778;    float SlngVylzRNZdWQedlumX91424055 = -474909753;    float SlngVylzRNZdWQedlumX84278506 = 72874091;    float SlngVylzRNZdWQedlumX88391135 = -9488781;    float SlngVylzRNZdWQedlumX24693346 = -358310761;    float SlngVylzRNZdWQedlumX27831978 = -730779226;    float SlngVylzRNZdWQedlumX27689629 = -284421950;    float SlngVylzRNZdWQedlumX11772467 = -448007518;    float SlngVylzRNZdWQedlumX98940178 = -96407555;    float SlngVylzRNZdWQedlumX86519495 = -337635612;    float SlngVylzRNZdWQedlumX84976028 = -263459785;    float SlngVylzRNZdWQedlumX79821216 = -214306510;    float SlngVylzRNZdWQedlumX39232418 = -684217714;    float SlngVylzRNZdWQedlumX39759431 = -609093345;    float SlngVylzRNZdWQedlumX23584422 = -673233273;    float SlngVylzRNZdWQedlumX64993471 = -109329282;    float SlngVylzRNZdWQedlumX27878668 = -782912959;    float SlngVylzRNZdWQedlumX852859 = -305831146;    float SlngVylzRNZdWQedlumX1062782 = -656752726;    float SlngVylzRNZdWQedlumX90619298 = -752417365;    float SlngVylzRNZdWQedlumX76728853 = 38500782;    float SlngVylzRNZdWQedlumX50129809 = -783293892;    float SlngVylzRNZdWQedlumX22571014 = -122286684;    float SlngVylzRNZdWQedlumX37615614 = -920154064;    float SlngVylzRNZdWQedlumX70647847 = -33640794;    float SlngVylzRNZdWQedlumX31015429 = -60390982;    float SlngVylzRNZdWQedlumX26678684 = -14437652;    float SlngVylzRNZdWQedlumX84646013 = -863437259;    float SlngVylzRNZdWQedlumX79438191 = -801012710;    float SlngVylzRNZdWQedlumX96038237 = -119592796;    float SlngVylzRNZdWQedlumX94429700 = 55914224;    float SlngVylzRNZdWQedlumX24873811 = -152491244;    float SlngVylzRNZdWQedlumX78035889 = -291913474;    float SlngVylzRNZdWQedlumX19924317 = -357628276;    float SlngVylzRNZdWQedlumX30797985 = 51051288;    float SlngVylzRNZdWQedlumX52293313 = -821399892;    float SlngVylzRNZdWQedlumX13118682 = -750858569;    float SlngVylzRNZdWQedlumX28012690 = -634118490;    float SlngVylzRNZdWQedlumX41057990 = -880181061;    float SlngVylzRNZdWQedlumX92063161 = -480546699;    float SlngVylzRNZdWQedlumX76812948 = -950172946;    float SlngVylzRNZdWQedlumX56535463 = -829368873;    float SlngVylzRNZdWQedlumX87992749 = 99340035;    float SlngVylzRNZdWQedlumX25144653 = -127688587;    float SlngVylzRNZdWQedlumX19660201 = -617810250;    float SlngVylzRNZdWQedlumX88441528 = -867850501;    float SlngVylzRNZdWQedlumX67043579 = -584768621;    float SlngVylzRNZdWQedlumX6495454 = -177943240;    float SlngVylzRNZdWQedlumX61337583 = -805585918;    float SlngVylzRNZdWQedlumX10210175 = -890662188;    float SlngVylzRNZdWQedlumX24340595 = -845624388;    float SlngVylzRNZdWQedlumX24847419 = -327955792;    float SlngVylzRNZdWQedlumX59180755 = -407248377;    float SlngVylzRNZdWQedlumX95951118 = -883979195;    float SlngVylzRNZdWQedlumX80408334 = -283362968;    float SlngVylzRNZdWQedlumX29095261 = -263487886;    float SlngVylzRNZdWQedlumX68853041 = -252623069;    float SlngVylzRNZdWQedlumX46662893 = -6971846;    float SlngVylzRNZdWQedlumX17743288 = 30457083;     SlngVylzRNZdWQedlumX67301790 = SlngVylzRNZdWQedlumX62390659;     SlngVylzRNZdWQedlumX62390659 = SlngVylzRNZdWQedlumX44282493;     SlngVylzRNZdWQedlumX44282493 = SlngVylzRNZdWQedlumX66604195;     SlngVylzRNZdWQedlumX66604195 = SlngVylzRNZdWQedlumX20957921;     SlngVylzRNZdWQedlumX20957921 = SlngVylzRNZdWQedlumX95121875;     SlngVylzRNZdWQedlumX95121875 = SlngVylzRNZdWQedlumX45036784;     SlngVylzRNZdWQedlumX45036784 = SlngVylzRNZdWQedlumX37062738;     SlngVylzRNZdWQedlumX37062738 = SlngVylzRNZdWQedlumX34235161;     SlngVylzRNZdWQedlumX34235161 = SlngVylzRNZdWQedlumX42251842;     SlngVylzRNZdWQedlumX42251842 = SlngVylzRNZdWQedlumX38910592;     SlngVylzRNZdWQedlumX38910592 = SlngVylzRNZdWQedlumX37291426;     SlngVylzRNZdWQedlumX37291426 = SlngVylzRNZdWQedlumX72933568;     SlngVylzRNZdWQedlumX72933568 = SlngVylzRNZdWQedlumX69568018;     SlngVylzRNZdWQedlumX69568018 = SlngVylzRNZdWQedlumX54641837;     SlngVylzRNZdWQedlumX54641837 = SlngVylzRNZdWQedlumX48257500;     SlngVylzRNZdWQedlumX48257500 = SlngVylzRNZdWQedlumX20129978;     SlngVylzRNZdWQedlumX20129978 = SlngVylzRNZdWQedlumX85117346;     SlngVylzRNZdWQedlumX85117346 = SlngVylzRNZdWQedlumX64606307;     SlngVylzRNZdWQedlumX64606307 = SlngVylzRNZdWQedlumX77061503;     SlngVylzRNZdWQedlumX77061503 = SlngVylzRNZdWQedlumX10023056;     SlngVylzRNZdWQedlumX10023056 = SlngVylzRNZdWQedlumX43717368;     SlngVylzRNZdWQedlumX43717368 = SlngVylzRNZdWQedlumX97397187;     SlngVylzRNZdWQedlumX97397187 = SlngVylzRNZdWQedlumX16403825;     SlngVylzRNZdWQedlumX16403825 = SlngVylzRNZdWQedlumX65751336;     SlngVylzRNZdWQedlumX65751336 = SlngVylzRNZdWQedlumX19895140;     SlngVylzRNZdWQedlumX19895140 = SlngVylzRNZdWQedlumX4502577;     SlngVylzRNZdWQedlumX4502577 = SlngVylzRNZdWQedlumX68307930;     SlngVylzRNZdWQedlumX68307930 = SlngVylzRNZdWQedlumX86932928;     SlngVylzRNZdWQedlumX86932928 = SlngVylzRNZdWQedlumX11664148;     SlngVylzRNZdWQedlumX11664148 = SlngVylzRNZdWQedlumX4636229;     SlngVylzRNZdWQedlumX4636229 = SlngVylzRNZdWQedlumX68262744;     SlngVylzRNZdWQedlumX68262744 = SlngVylzRNZdWQedlumX6275997;     SlngVylzRNZdWQedlumX6275997 = SlngVylzRNZdWQedlumX46254885;     SlngVylzRNZdWQedlumX46254885 = SlngVylzRNZdWQedlumX84922005;     SlngVylzRNZdWQedlumX84922005 = SlngVylzRNZdWQedlumX75203645;     SlngVylzRNZdWQedlumX75203645 = SlngVylzRNZdWQedlumX52219262;     SlngVylzRNZdWQedlumX52219262 = SlngVylzRNZdWQedlumX25700277;     SlngVylzRNZdWQedlumX25700277 = SlngVylzRNZdWQedlumX60243536;     SlngVylzRNZdWQedlumX60243536 = SlngVylzRNZdWQedlumX86570417;     SlngVylzRNZdWQedlumX86570417 = SlngVylzRNZdWQedlumX57137187;     SlngVylzRNZdWQedlumX57137187 = SlngVylzRNZdWQedlumX79225070;     SlngVylzRNZdWQedlumX79225070 = SlngVylzRNZdWQedlumX91424055;     SlngVylzRNZdWQedlumX91424055 = SlngVylzRNZdWQedlumX84278506;     SlngVylzRNZdWQedlumX84278506 = SlngVylzRNZdWQedlumX88391135;     SlngVylzRNZdWQedlumX88391135 = SlngVylzRNZdWQedlumX24693346;     SlngVylzRNZdWQedlumX24693346 = SlngVylzRNZdWQedlumX27831978;     SlngVylzRNZdWQedlumX27831978 = SlngVylzRNZdWQedlumX27689629;     SlngVylzRNZdWQedlumX27689629 = SlngVylzRNZdWQedlumX11772467;     SlngVylzRNZdWQedlumX11772467 = SlngVylzRNZdWQedlumX98940178;     SlngVylzRNZdWQedlumX98940178 = SlngVylzRNZdWQedlumX86519495;     SlngVylzRNZdWQedlumX86519495 = SlngVylzRNZdWQedlumX84976028;     SlngVylzRNZdWQedlumX84976028 = SlngVylzRNZdWQedlumX79821216;     SlngVylzRNZdWQedlumX79821216 = SlngVylzRNZdWQedlumX39232418;     SlngVylzRNZdWQedlumX39232418 = SlngVylzRNZdWQedlumX39759431;     SlngVylzRNZdWQedlumX39759431 = SlngVylzRNZdWQedlumX23584422;     SlngVylzRNZdWQedlumX23584422 = SlngVylzRNZdWQedlumX64993471;     SlngVylzRNZdWQedlumX64993471 = SlngVylzRNZdWQedlumX27878668;     SlngVylzRNZdWQedlumX27878668 = SlngVylzRNZdWQedlumX852859;     SlngVylzRNZdWQedlumX852859 = SlngVylzRNZdWQedlumX1062782;     SlngVylzRNZdWQedlumX1062782 = SlngVylzRNZdWQedlumX90619298;     SlngVylzRNZdWQedlumX90619298 = SlngVylzRNZdWQedlumX76728853;     SlngVylzRNZdWQedlumX76728853 = SlngVylzRNZdWQedlumX50129809;     SlngVylzRNZdWQedlumX50129809 = SlngVylzRNZdWQedlumX22571014;     SlngVylzRNZdWQedlumX22571014 = SlngVylzRNZdWQedlumX37615614;     SlngVylzRNZdWQedlumX37615614 = SlngVylzRNZdWQedlumX70647847;     SlngVylzRNZdWQedlumX70647847 = SlngVylzRNZdWQedlumX31015429;     SlngVylzRNZdWQedlumX31015429 = SlngVylzRNZdWQedlumX26678684;     SlngVylzRNZdWQedlumX26678684 = SlngVylzRNZdWQedlumX84646013;     SlngVylzRNZdWQedlumX84646013 = SlngVylzRNZdWQedlumX79438191;     SlngVylzRNZdWQedlumX79438191 = SlngVylzRNZdWQedlumX96038237;     SlngVylzRNZdWQedlumX96038237 = SlngVylzRNZdWQedlumX94429700;     SlngVylzRNZdWQedlumX94429700 = SlngVylzRNZdWQedlumX24873811;     SlngVylzRNZdWQedlumX24873811 = SlngVylzRNZdWQedlumX78035889;     SlngVylzRNZdWQedlumX78035889 = SlngVylzRNZdWQedlumX19924317;     SlngVylzRNZdWQedlumX19924317 = SlngVylzRNZdWQedlumX30797985;     SlngVylzRNZdWQedlumX30797985 = SlngVylzRNZdWQedlumX52293313;     SlngVylzRNZdWQedlumX52293313 = SlngVylzRNZdWQedlumX13118682;     SlngVylzRNZdWQedlumX13118682 = SlngVylzRNZdWQedlumX28012690;     SlngVylzRNZdWQedlumX28012690 = SlngVylzRNZdWQedlumX41057990;     SlngVylzRNZdWQedlumX41057990 = SlngVylzRNZdWQedlumX92063161;     SlngVylzRNZdWQedlumX92063161 = SlngVylzRNZdWQedlumX76812948;     SlngVylzRNZdWQedlumX76812948 = SlngVylzRNZdWQedlumX56535463;     SlngVylzRNZdWQedlumX56535463 = SlngVylzRNZdWQedlumX87992749;     SlngVylzRNZdWQedlumX87992749 = SlngVylzRNZdWQedlumX25144653;     SlngVylzRNZdWQedlumX25144653 = SlngVylzRNZdWQedlumX19660201;     SlngVylzRNZdWQedlumX19660201 = SlngVylzRNZdWQedlumX88441528;     SlngVylzRNZdWQedlumX88441528 = SlngVylzRNZdWQedlumX67043579;     SlngVylzRNZdWQedlumX67043579 = SlngVylzRNZdWQedlumX6495454;     SlngVylzRNZdWQedlumX6495454 = SlngVylzRNZdWQedlumX61337583;     SlngVylzRNZdWQedlumX61337583 = SlngVylzRNZdWQedlumX10210175;     SlngVylzRNZdWQedlumX10210175 = SlngVylzRNZdWQedlumX24340595;     SlngVylzRNZdWQedlumX24340595 = SlngVylzRNZdWQedlumX24847419;     SlngVylzRNZdWQedlumX24847419 = SlngVylzRNZdWQedlumX59180755;     SlngVylzRNZdWQedlumX59180755 = SlngVylzRNZdWQedlumX95951118;     SlngVylzRNZdWQedlumX95951118 = SlngVylzRNZdWQedlumX80408334;     SlngVylzRNZdWQedlumX80408334 = SlngVylzRNZdWQedlumX29095261;     SlngVylzRNZdWQedlumX29095261 = SlngVylzRNZdWQedlumX68853041;     SlngVylzRNZdWQedlumX68853041 = SlngVylzRNZdWQedlumX46662893;     SlngVylzRNZdWQedlumX46662893 = SlngVylzRNZdWQedlumX17743288;     SlngVylzRNZdWQedlumX17743288 = SlngVylzRNZdWQedlumX67301790;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kceVdwPxQPNfBXyPIsVj56441230() {     float bsfazphQdzIxoOrKraKL67295319 = -942067059;    float bsfazphQdzIxoOrKraKL962976 = 70091116;    float bsfazphQdzIxoOrKraKL45595705 = -483648842;    float bsfazphQdzIxoOrKraKL51684055 = -464863797;    float bsfazphQdzIxoOrKraKL29798023 = -336433972;    float bsfazphQdzIxoOrKraKL29927308 = -786445072;    float bsfazphQdzIxoOrKraKL13584120 = -43412075;    float bsfazphQdzIxoOrKraKL6276789 = -684538728;    float bsfazphQdzIxoOrKraKL16975326 = -501256951;    float bsfazphQdzIxoOrKraKL38891356 = -538230969;    float bsfazphQdzIxoOrKraKL59231197 = -175269876;    float bsfazphQdzIxoOrKraKL51583103 = -164855678;    float bsfazphQdzIxoOrKraKL4786483 = 76867290;    float bsfazphQdzIxoOrKraKL23412239 = -687409501;    float bsfazphQdzIxoOrKraKL99813946 = -863717032;    float bsfazphQdzIxoOrKraKL22822048 = -80925792;    float bsfazphQdzIxoOrKraKL75982737 = -597487283;    float bsfazphQdzIxoOrKraKL8117880 = -713563968;    float bsfazphQdzIxoOrKraKL85034576 = -546292300;    float bsfazphQdzIxoOrKraKL49135267 = -314514912;    float bsfazphQdzIxoOrKraKL89692479 = -906196863;    float bsfazphQdzIxoOrKraKL22013634 = -806301528;    float bsfazphQdzIxoOrKraKL34275585 = -136942319;    float bsfazphQdzIxoOrKraKL60053933 = -624527793;    float bsfazphQdzIxoOrKraKL94194025 = -665185674;    float bsfazphQdzIxoOrKraKL36469216 = -119531853;    float bsfazphQdzIxoOrKraKL74724158 = -535394609;    float bsfazphQdzIxoOrKraKL3711456 = -886343378;    float bsfazphQdzIxoOrKraKL99611375 = -743137834;    float bsfazphQdzIxoOrKraKL87754017 = -240755571;    float bsfazphQdzIxoOrKraKL38048908 = -268363269;    float bsfazphQdzIxoOrKraKL52986602 = -473978772;    float bsfazphQdzIxoOrKraKL91728420 = -470930550;    float bsfazphQdzIxoOrKraKL46537722 = -763718317;    float bsfazphQdzIxoOrKraKL67280043 = -738133115;    float bsfazphQdzIxoOrKraKL18031836 = -79757217;    float bsfazphQdzIxoOrKraKL64160630 = -728190724;    float bsfazphQdzIxoOrKraKL95617956 = -729049617;    float bsfazphQdzIxoOrKraKL95557132 = -776682519;    float bsfazphQdzIxoOrKraKL58443310 = -245828246;    float bsfazphQdzIxoOrKraKL44992855 = -432806084;    float bsfazphQdzIxoOrKraKL64922731 = -240573860;    float bsfazphQdzIxoOrKraKL51707415 = 65959993;    float bsfazphQdzIxoOrKraKL43510405 = -584090352;    float bsfazphQdzIxoOrKraKL72409188 = -482748182;    float bsfazphQdzIxoOrKraKL65121314 = -170355432;    float bsfazphQdzIxoOrKraKL55529771 = -112059848;    float bsfazphQdzIxoOrKraKL3073210 = -893183306;    float bsfazphQdzIxoOrKraKL30430857 = -11771379;    float bsfazphQdzIxoOrKraKL18985602 = -783432441;    float bsfazphQdzIxoOrKraKL99778864 = 83809381;    float bsfazphQdzIxoOrKraKL99523285 = -645605534;    float bsfazphQdzIxoOrKraKL55269564 = -106360518;    float bsfazphQdzIxoOrKraKL49482689 = -116137565;    float bsfazphQdzIxoOrKraKL66748751 = -365992622;    float bsfazphQdzIxoOrKraKL45281685 = -35765531;    float bsfazphQdzIxoOrKraKL66687390 = -792966565;    float bsfazphQdzIxoOrKraKL85541771 = -859121050;    float bsfazphQdzIxoOrKraKL57490030 = -799678123;    float bsfazphQdzIxoOrKraKL93328806 = -116902120;    float bsfazphQdzIxoOrKraKL55203149 = -151050464;    float bsfazphQdzIxoOrKraKL9872664 = -157068697;    float bsfazphQdzIxoOrKraKL6665413 = -941400895;    float bsfazphQdzIxoOrKraKL29221308 = -160501381;    float bsfazphQdzIxoOrKraKL842449 = -169867700;    float bsfazphQdzIxoOrKraKL6244596 = -701291105;    float bsfazphQdzIxoOrKraKL59854683 = -693925128;    float bsfazphQdzIxoOrKraKL58248760 = -159414394;    float bsfazphQdzIxoOrKraKL56132195 = -949276386;    float bsfazphQdzIxoOrKraKL81782110 = -683959816;    float bsfazphQdzIxoOrKraKL58661417 = -352735068;    float bsfazphQdzIxoOrKraKL80364780 = -868437667;    float bsfazphQdzIxoOrKraKL12560747 = -936881450;    float bsfazphQdzIxoOrKraKL26591267 = -200464055;    float bsfazphQdzIxoOrKraKL4142412 = -881708829;    float bsfazphQdzIxoOrKraKL24769748 = -565623003;    float bsfazphQdzIxoOrKraKL70306219 = -772261521;    float bsfazphQdzIxoOrKraKL90765179 = -552851967;    float bsfazphQdzIxoOrKraKL87644745 = -41779611;    float bsfazphQdzIxoOrKraKL29072712 = -394830243;    float bsfazphQdzIxoOrKraKL80939445 = 92527995;    float bsfazphQdzIxoOrKraKL71650948 = -642211303;    float bsfazphQdzIxoOrKraKL73280599 = -774572000;    float bsfazphQdzIxoOrKraKL80625773 = -959705393;    float bsfazphQdzIxoOrKraKL87975153 = -224564953;    float bsfazphQdzIxoOrKraKL38525622 = -622757736;    float bsfazphQdzIxoOrKraKL97717037 = -267618254;    float bsfazphQdzIxoOrKraKL42245731 = -254792986;    float bsfazphQdzIxoOrKraKL79788971 = -297725695;    float bsfazphQdzIxoOrKraKL21998359 = -602367585;    float bsfazphQdzIxoOrKraKL51344445 = -286790652;    float bsfazphQdzIxoOrKraKL78618858 = -869069675;    float bsfazphQdzIxoOrKraKL38127927 = -929371494;    float bsfazphQdzIxoOrKraKL2228327 = -559780400;    float bsfazphQdzIxoOrKraKL3240161 = 5222217;    float bsfazphQdzIxoOrKraKL35120191 = -175737387;    float bsfazphQdzIxoOrKraKL58257318 = -299172966;    float bsfazphQdzIxoOrKraKL22486108 = -773538627;    float bsfazphQdzIxoOrKraKL42667957 = -314222653;    float bsfazphQdzIxoOrKraKL66164592 = -942067059;     bsfazphQdzIxoOrKraKL67295319 = bsfazphQdzIxoOrKraKL962976;     bsfazphQdzIxoOrKraKL962976 = bsfazphQdzIxoOrKraKL45595705;     bsfazphQdzIxoOrKraKL45595705 = bsfazphQdzIxoOrKraKL51684055;     bsfazphQdzIxoOrKraKL51684055 = bsfazphQdzIxoOrKraKL29798023;     bsfazphQdzIxoOrKraKL29798023 = bsfazphQdzIxoOrKraKL29927308;     bsfazphQdzIxoOrKraKL29927308 = bsfazphQdzIxoOrKraKL13584120;     bsfazphQdzIxoOrKraKL13584120 = bsfazphQdzIxoOrKraKL6276789;     bsfazphQdzIxoOrKraKL6276789 = bsfazphQdzIxoOrKraKL16975326;     bsfazphQdzIxoOrKraKL16975326 = bsfazphQdzIxoOrKraKL38891356;     bsfazphQdzIxoOrKraKL38891356 = bsfazphQdzIxoOrKraKL59231197;     bsfazphQdzIxoOrKraKL59231197 = bsfazphQdzIxoOrKraKL51583103;     bsfazphQdzIxoOrKraKL51583103 = bsfazphQdzIxoOrKraKL4786483;     bsfazphQdzIxoOrKraKL4786483 = bsfazphQdzIxoOrKraKL23412239;     bsfazphQdzIxoOrKraKL23412239 = bsfazphQdzIxoOrKraKL99813946;     bsfazphQdzIxoOrKraKL99813946 = bsfazphQdzIxoOrKraKL22822048;     bsfazphQdzIxoOrKraKL22822048 = bsfazphQdzIxoOrKraKL75982737;     bsfazphQdzIxoOrKraKL75982737 = bsfazphQdzIxoOrKraKL8117880;     bsfazphQdzIxoOrKraKL8117880 = bsfazphQdzIxoOrKraKL85034576;     bsfazphQdzIxoOrKraKL85034576 = bsfazphQdzIxoOrKraKL49135267;     bsfazphQdzIxoOrKraKL49135267 = bsfazphQdzIxoOrKraKL89692479;     bsfazphQdzIxoOrKraKL89692479 = bsfazphQdzIxoOrKraKL22013634;     bsfazphQdzIxoOrKraKL22013634 = bsfazphQdzIxoOrKraKL34275585;     bsfazphQdzIxoOrKraKL34275585 = bsfazphQdzIxoOrKraKL60053933;     bsfazphQdzIxoOrKraKL60053933 = bsfazphQdzIxoOrKraKL94194025;     bsfazphQdzIxoOrKraKL94194025 = bsfazphQdzIxoOrKraKL36469216;     bsfazphQdzIxoOrKraKL36469216 = bsfazphQdzIxoOrKraKL74724158;     bsfazphQdzIxoOrKraKL74724158 = bsfazphQdzIxoOrKraKL3711456;     bsfazphQdzIxoOrKraKL3711456 = bsfazphQdzIxoOrKraKL99611375;     bsfazphQdzIxoOrKraKL99611375 = bsfazphQdzIxoOrKraKL87754017;     bsfazphQdzIxoOrKraKL87754017 = bsfazphQdzIxoOrKraKL38048908;     bsfazphQdzIxoOrKraKL38048908 = bsfazphQdzIxoOrKraKL52986602;     bsfazphQdzIxoOrKraKL52986602 = bsfazphQdzIxoOrKraKL91728420;     bsfazphQdzIxoOrKraKL91728420 = bsfazphQdzIxoOrKraKL46537722;     bsfazphQdzIxoOrKraKL46537722 = bsfazphQdzIxoOrKraKL67280043;     bsfazphQdzIxoOrKraKL67280043 = bsfazphQdzIxoOrKraKL18031836;     bsfazphQdzIxoOrKraKL18031836 = bsfazphQdzIxoOrKraKL64160630;     bsfazphQdzIxoOrKraKL64160630 = bsfazphQdzIxoOrKraKL95617956;     bsfazphQdzIxoOrKraKL95617956 = bsfazphQdzIxoOrKraKL95557132;     bsfazphQdzIxoOrKraKL95557132 = bsfazphQdzIxoOrKraKL58443310;     bsfazphQdzIxoOrKraKL58443310 = bsfazphQdzIxoOrKraKL44992855;     bsfazphQdzIxoOrKraKL44992855 = bsfazphQdzIxoOrKraKL64922731;     bsfazphQdzIxoOrKraKL64922731 = bsfazphQdzIxoOrKraKL51707415;     bsfazphQdzIxoOrKraKL51707415 = bsfazphQdzIxoOrKraKL43510405;     bsfazphQdzIxoOrKraKL43510405 = bsfazphQdzIxoOrKraKL72409188;     bsfazphQdzIxoOrKraKL72409188 = bsfazphQdzIxoOrKraKL65121314;     bsfazphQdzIxoOrKraKL65121314 = bsfazphQdzIxoOrKraKL55529771;     bsfazphQdzIxoOrKraKL55529771 = bsfazphQdzIxoOrKraKL3073210;     bsfazphQdzIxoOrKraKL3073210 = bsfazphQdzIxoOrKraKL30430857;     bsfazphQdzIxoOrKraKL30430857 = bsfazphQdzIxoOrKraKL18985602;     bsfazphQdzIxoOrKraKL18985602 = bsfazphQdzIxoOrKraKL99778864;     bsfazphQdzIxoOrKraKL99778864 = bsfazphQdzIxoOrKraKL99523285;     bsfazphQdzIxoOrKraKL99523285 = bsfazphQdzIxoOrKraKL55269564;     bsfazphQdzIxoOrKraKL55269564 = bsfazphQdzIxoOrKraKL49482689;     bsfazphQdzIxoOrKraKL49482689 = bsfazphQdzIxoOrKraKL66748751;     bsfazphQdzIxoOrKraKL66748751 = bsfazphQdzIxoOrKraKL45281685;     bsfazphQdzIxoOrKraKL45281685 = bsfazphQdzIxoOrKraKL66687390;     bsfazphQdzIxoOrKraKL66687390 = bsfazphQdzIxoOrKraKL85541771;     bsfazphQdzIxoOrKraKL85541771 = bsfazphQdzIxoOrKraKL57490030;     bsfazphQdzIxoOrKraKL57490030 = bsfazphQdzIxoOrKraKL93328806;     bsfazphQdzIxoOrKraKL93328806 = bsfazphQdzIxoOrKraKL55203149;     bsfazphQdzIxoOrKraKL55203149 = bsfazphQdzIxoOrKraKL9872664;     bsfazphQdzIxoOrKraKL9872664 = bsfazphQdzIxoOrKraKL6665413;     bsfazphQdzIxoOrKraKL6665413 = bsfazphQdzIxoOrKraKL29221308;     bsfazphQdzIxoOrKraKL29221308 = bsfazphQdzIxoOrKraKL842449;     bsfazphQdzIxoOrKraKL842449 = bsfazphQdzIxoOrKraKL6244596;     bsfazphQdzIxoOrKraKL6244596 = bsfazphQdzIxoOrKraKL59854683;     bsfazphQdzIxoOrKraKL59854683 = bsfazphQdzIxoOrKraKL58248760;     bsfazphQdzIxoOrKraKL58248760 = bsfazphQdzIxoOrKraKL56132195;     bsfazphQdzIxoOrKraKL56132195 = bsfazphQdzIxoOrKraKL81782110;     bsfazphQdzIxoOrKraKL81782110 = bsfazphQdzIxoOrKraKL58661417;     bsfazphQdzIxoOrKraKL58661417 = bsfazphQdzIxoOrKraKL80364780;     bsfazphQdzIxoOrKraKL80364780 = bsfazphQdzIxoOrKraKL12560747;     bsfazphQdzIxoOrKraKL12560747 = bsfazphQdzIxoOrKraKL26591267;     bsfazphQdzIxoOrKraKL26591267 = bsfazphQdzIxoOrKraKL4142412;     bsfazphQdzIxoOrKraKL4142412 = bsfazphQdzIxoOrKraKL24769748;     bsfazphQdzIxoOrKraKL24769748 = bsfazphQdzIxoOrKraKL70306219;     bsfazphQdzIxoOrKraKL70306219 = bsfazphQdzIxoOrKraKL90765179;     bsfazphQdzIxoOrKraKL90765179 = bsfazphQdzIxoOrKraKL87644745;     bsfazphQdzIxoOrKraKL87644745 = bsfazphQdzIxoOrKraKL29072712;     bsfazphQdzIxoOrKraKL29072712 = bsfazphQdzIxoOrKraKL80939445;     bsfazphQdzIxoOrKraKL80939445 = bsfazphQdzIxoOrKraKL71650948;     bsfazphQdzIxoOrKraKL71650948 = bsfazphQdzIxoOrKraKL73280599;     bsfazphQdzIxoOrKraKL73280599 = bsfazphQdzIxoOrKraKL80625773;     bsfazphQdzIxoOrKraKL80625773 = bsfazphQdzIxoOrKraKL87975153;     bsfazphQdzIxoOrKraKL87975153 = bsfazphQdzIxoOrKraKL38525622;     bsfazphQdzIxoOrKraKL38525622 = bsfazphQdzIxoOrKraKL97717037;     bsfazphQdzIxoOrKraKL97717037 = bsfazphQdzIxoOrKraKL42245731;     bsfazphQdzIxoOrKraKL42245731 = bsfazphQdzIxoOrKraKL79788971;     bsfazphQdzIxoOrKraKL79788971 = bsfazphQdzIxoOrKraKL21998359;     bsfazphQdzIxoOrKraKL21998359 = bsfazphQdzIxoOrKraKL51344445;     bsfazphQdzIxoOrKraKL51344445 = bsfazphQdzIxoOrKraKL78618858;     bsfazphQdzIxoOrKraKL78618858 = bsfazphQdzIxoOrKraKL38127927;     bsfazphQdzIxoOrKraKL38127927 = bsfazphQdzIxoOrKraKL2228327;     bsfazphQdzIxoOrKraKL2228327 = bsfazphQdzIxoOrKraKL3240161;     bsfazphQdzIxoOrKraKL3240161 = bsfazphQdzIxoOrKraKL35120191;     bsfazphQdzIxoOrKraKL35120191 = bsfazphQdzIxoOrKraKL58257318;     bsfazphQdzIxoOrKraKL58257318 = bsfazphQdzIxoOrKraKL22486108;     bsfazphQdzIxoOrKraKL22486108 = bsfazphQdzIxoOrKraKL42667957;     bsfazphQdzIxoOrKraKL42667957 = bsfazphQdzIxoOrKraKL66164592;     bsfazphQdzIxoOrKraKL66164592 = bsfazphQdzIxoOrKraKL67295319;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LriQWLadxuNpPbsnZsoe10857444() {     float QJGysSQpLIqJtCzjNZcv37946986 = -460216189;    float QJGysSQpLIqJtCzjNZcv69636492 = -901552183;    float QJGysSQpLIqJtCzjNZcv25564624 = -925943302;    float QJGysSQpLIqJtCzjNZcv91615342 = -655002186;    float QJGysSQpLIqJtCzjNZcv51248344 = -119840319;    float QJGysSQpLIqJtCzjNZcv19927071 = -488026224;    float QJGysSQpLIqJtCzjNZcv31963015 = -42187784;    float QJGysSQpLIqJtCzjNZcv63119169 = 68644182;    float QJGysSQpLIqJtCzjNZcv62840380 = -594604280;    float QJGysSQpLIqJtCzjNZcv61857816 = -839326319;    float QJGysSQpLIqJtCzjNZcv1480358 = -210171806;    float QJGysSQpLIqJtCzjNZcv97129674 = -480619471;    float QJGysSQpLIqJtCzjNZcv84613869 = -932669768;    float QJGysSQpLIqJtCzjNZcv7259273 = -488437711;    float QJGysSQpLIqJtCzjNZcv82533965 = 86532965;    float QJGysSQpLIqJtCzjNZcv55350688 = -750909379;    float QJGysSQpLIqJtCzjNZcv69835563 = -506672413;    float QJGysSQpLIqJtCzjNZcv28411374 = -702730164;    float QJGysSQpLIqJtCzjNZcv66742358 = -153480242;    float QJGysSQpLIqJtCzjNZcv7921976 = -856881217;    float QJGysSQpLIqJtCzjNZcv44828234 = -814876004;    float QJGysSQpLIqJtCzjNZcv83409399 = -37586834;    float QJGysSQpLIqJtCzjNZcv36219393 = -704785464;    float QJGysSQpLIqJtCzjNZcv93903470 = -216928699;    float QJGysSQpLIqJtCzjNZcv58318414 = -341090362;    float QJGysSQpLIqJtCzjNZcv67066797 = -217816297;    float QJGysSQpLIqJtCzjNZcv82984944 = -930572384;    float QJGysSQpLIqJtCzjNZcv11493681 = -291467146;    float QJGysSQpLIqJtCzjNZcv31753863 = 62503949;    float QJGysSQpLIqJtCzjNZcv26926114 = -799150642;    float QJGysSQpLIqJtCzjNZcv45120904 = -16796955;    float QJGysSQpLIqJtCzjNZcv97498062 = -802275935;    float QJGysSQpLIqJtCzjNZcv74673781 = -417892159;    float QJGysSQpLIqJtCzjNZcv11902697 = -327248001;    float QJGysSQpLIqJtCzjNZcv45304280 = -585079895;    float QJGysSQpLIqJtCzjNZcv78025094 = -158983764;    float QJGysSQpLIqJtCzjNZcv7952117 = -916783932;    float QJGysSQpLIqJtCzjNZcv48410445 = -513162724;    float QJGysSQpLIqJtCzjNZcv74090421 = -732411156;    float QJGysSQpLIqJtCzjNZcv53546608 = -863436609;    float QJGysSQpLIqJtCzjNZcv5066139 = -816824962;    float QJGysSQpLIqJtCzjNZcv93405722 = -9962930;    float QJGysSQpLIqJtCzjNZcv674375 = 79205941;    float QJGysSQpLIqJtCzjNZcv50018705 = -656065324;    float QJGysSQpLIqJtCzjNZcv68363246 = -791394577;    float QJGysSQpLIqJtCzjNZcv71527149 = 71307413;    float QJGysSQpLIqJtCzjNZcv81660973 = -228129647;    float QJGysSQpLIqJtCzjNZcv68687420 = -245313601;    float QJGysSQpLIqJtCzjNZcv35872309 = 2464652;    float QJGysSQpLIqJtCzjNZcv27413088 = 33558680;    float QJGysSQpLIqJtCzjNZcv67968189 = -562762740;    float QJGysSQpLIqJtCzjNZcv41394790 = -614825255;    float QJGysSQpLIqJtCzjNZcv10345222 = -34158560;    float QJGysSQpLIqJtCzjNZcv39709993 = -777019061;    float QJGysSQpLIqJtCzjNZcv66029210 = -476933537;    float QJGysSQpLIqJtCzjNZcv54537587 = -322629356;    float QJGysSQpLIqJtCzjNZcv33417100 = -96766720;    float QJGysSQpLIqJtCzjNZcv31661154 = -609014603;    float QJGysSQpLIqJtCzjNZcv33296928 = -213911824;    float QJGysSQpLIqJtCzjNZcv84181547 = -902024022;    float QJGysSQpLIqJtCzjNZcv36942126 = -557453840;    float QJGysSQpLIqJtCzjNZcv20469335 = -750720639;    float QJGysSQpLIqJtCzjNZcv31365306 = -993859768;    float QJGysSQpLIqJtCzjNZcv35914266 = -795453639;    float QJGysSQpLIqJtCzjNZcv16736913 = -722529365;    float QJGysSQpLIqJtCzjNZcv3982296 = -407895871;    float QJGysSQpLIqJtCzjNZcv22455894 = 37272687;    float QJGysSQpLIqJtCzjNZcv72711172 = -505421768;    float QJGysSQpLIqJtCzjNZcv61954993 = -903357817;    float QJGysSQpLIqJtCzjNZcv4508872 = -754483271;    float QJGysSQpLIqJtCzjNZcv47398572 = -834125448;    float QJGysSQpLIqJtCzjNZcv21425119 = -993509689;    float QJGysSQpLIqJtCzjNZcv54320952 = -970319009;    float QJGysSQpLIqJtCzjNZcv13195751 = -290043634;    float QJGysSQpLIqJtCzjNZcv2855837 = 59943744;    float QJGysSQpLIqJtCzjNZcv51422512 = -704913074;    float QJGysSQpLIqJtCzjNZcv82735024 = -16792775;    float QJGysSQpLIqJtCzjNZcv86200687 = 51279860;    float QJGysSQpLIqJtCzjNZcv25540224 = -425534123;    float QJGysSQpLIqJtCzjNZcv86791265 = -312397776;    float QJGysSQpLIqJtCzjNZcv85405823 = -989686651;    float QJGysSQpLIqJtCzjNZcv14297525 = -585258783;    float QJGysSQpLIqJtCzjNZcv75621372 = -193931798;    float QJGysSQpLIqJtCzjNZcv4340776 = -971054731;    float QJGysSQpLIqJtCzjNZcv58957924 = -136387903;    float QJGysSQpLIqJtCzjNZcv3726115 = -401971700;    float QJGysSQpLIqJtCzjNZcv87152841 = -668117375;    float QJGysSQpLIqJtCzjNZcv34963788 = -640873098;    float QJGysSQpLIqJtCzjNZcv45873487 = -850314465;    float QJGysSQpLIqJtCzjNZcv90766692 = -162450540;    float QJGysSQpLIqJtCzjNZcv44607995 = 37782955;    float QJGysSQpLIqJtCzjNZcv76290962 = -207769329;    float QJGysSQpLIqJtCzjNZcv15113517 = -199250900;    float QJGysSQpLIqJtCzjNZcv89908874 = -830387134;    float QJGysSQpLIqJtCzjNZcv16604482 = -205982769;    float QJGysSQpLIqJtCzjNZcv84596804 = 33895677;    float QJGysSQpLIqJtCzjNZcv62040416 = -16103163;    float QJGysSQpLIqJtCzjNZcv64760109 = -125340421;    float QJGysSQpLIqJtCzjNZcv33281793 = -933535959;    float QJGysSQpLIqJtCzjNZcv64380951 = -460216189;     QJGysSQpLIqJtCzjNZcv37946986 = QJGysSQpLIqJtCzjNZcv69636492;     QJGysSQpLIqJtCzjNZcv69636492 = QJGysSQpLIqJtCzjNZcv25564624;     QJGysSQpLIqJtCzjNZcv25564624 = QJGysSQpLIqJtCzjNZcv91615342;     QJGysSQpLIqJtCzjNZcv91615342 = QJGysSQpLIqJtCzjNZcv51248344;     QJGysSQpLIqJtCzjNZcv51248344 = QJGysSQpLIqJtCzjNZcv19927071;     QJGysSQpLIqJtCzjNZcv19927071 = QJGysSQpLIqJtCzjNZcv31963015;     QJGysSQpLIqJtCzjNZcv31963015 = QJGysSQpLIqJtCzjNZcv63119169;     QJGysSQpLIqJtCzjNZcv63119169 = QJGysSQpLIqJtCzjNZcv62840380;     QJGysSQpLIqJtCzjNZcv62840380 = QJGysSQpLIqJtCzjNZcv61857816;     QJGysSQpLIqJtCzjNZcv61857816 = QJGysSQpLIqJtCzjNZcv1480358;     QJGysSQpLIqJtCzjNZcv1480358 = QJGysSQpLIqJtCzjNZcv97129674;     QJGysSQpLIqJtCzjNZcv97129674 = QJGysSQpLIqJtCzjNZcv84613869;     QJGysSQpLIqJtCzjNZcv84613869 = QJGysSQpLIqJtCzjNZcv7259273;     QJGysSQpLIqJtCzjNZcv7259273 = QJGysSQpLIqJtCzjNZcv82533965;     QJGysSQpLIqJtCzjNZcv82533965 = QJGysSQpLIqJtCzjNZcv55350688;     QJGysSQpLIqJtCzjNZcv55350688 = QJGysSQpLIqJtCzjNZcv69835563;     QJGysSQpLIqJtCzjNZcv69835563 = QJGysSQpLIqJtCzjNZcv28411374;     QJGysSQpLIqJtCzjNZcv28411374 = QJGysSQpLIqJtCzjNZcv66742358;     QJGysSQpLIqJtCzjNZcv66742358 = QJGysSQpLIqJtCzjNZcv7921976;     QJGysSQpLIqJtCzjNZcv7921976 = QJGysSQpLIqJtCzjNZcv44828234;     QJGysSQpLIqJtCzjNZcv44828234 = QJGysSQpLIqJtCzjNZcv83409399;     QJGysSQpLIqJtCzjNZcv83409399 = QJGysSQpLIqJtCzjNZcv36219393;     QJGysSQpLIqJtCzjNZcv36219393 = QJGysSQpLIqJtCzjNZcv93903470;     QJGysSQpLIqJtCzjNZcv93903470 = QJGysSQpLIqJtCzjNZcv58318414;     QJGysSQpLIqJtCzjNZcv58318414 = QJGysSQpLIqJtCzjNZcv67066797;     QJGysSQpLIqJtCzjNZcv67066797 = QJGysSQpLIqJtCzjNZcv82984944;     QJGysSQpLIqJtCzjNZcv82984944 = QJGysSQpLIqJtCzjNZcv11493681;     QJGysSQpLIqJtCzjNZcv11493681 = QJGysSQpLIqJtCzjNZcv31753863;     QJGysSQpLIqJtCzjNZcv31753863 = QJGysSQpLIqJtCzjNZcv26926114;     QJGysSQpLIqJtCzjNZcv26926114 = QJGysSQpLIqJtCzjNZcv45120904;     QJGysSQpLIqJtCzjNZcv45120904 = QJGysSQpLIqJtCzjNZcv97498062;     QJGysSQpLIqJtCzjNZcv97498062 = QJGysSQpLIqJtCzjNZcv74673781;     QJGysSQpLIqJtCzjNZcv74673781 = QJGysSQpLIqJtCzjNZcv11902697;     QJGysSQpLIqJtCzjNZcv11902697 = QJGysSQpLIqJtCzjNZcv45304280;     QJGysSQpLIqJtCzjNZcv45304280 = QJGysSQpLIqJtCzjNZcv78025094;     QJGysSQpLIqJtCzjNZcv78025094 = QJGysSQpLIqJtCzjNZcv7952117;     QJGysSQpLIqJtCzjNZcv7952117 = QJGysSQpLIqJtCzjNZcv48410445;     QJGysSQpLIqJtCzjNZcv48410445 = QJGysSQpLIqJtCzjNZcv74090421;     QJGysSQpLIqJtCzjNZcv74090421 = QJGysSQpLIqJtCzjNZcv53546608;     QJGysSQpLIqJtCzjNZcv53546608 = QJGysSQpLIqJtCzjNZcv5066139;     QJGysSQpLIqJtCzjNZcv5066139 = QJGysSQpLIqJtCzjNZcv93405722;     QJGysSQpLIqJtCzjNZcv93405722 = QJGysSQpLIqJtCzjNZcv674375;     QJGysSQpLIqJtCzjNZcv674375 = QJGysSQpLIqJtCzjNZcv50018705;     QJGysSQpLIqJtCzjNZcv50018705 = QJGysSQpLIqJtCzjNZcv68363246;     QJGysSQpLIqJtCzjNZcv68363246 = QJGysSQpLIqJtCzjNZcv71527149;     QJGysSQpLIqJtCzjNZcv71527149 = QJGysSQpLIqJtCzjNZcv81660973;     QJGysSQpLIqJtCzjNZcv81660973 = QJGysSQpLIqJtCzjNZcv68687420;     QJGysSQpLIqJtCzjNZcv68687420 = QJGysSQpLIqJtCzjNZcv35872309;     QJGysSQpLIqJtCzjNZcv35872309 = QJGysSQpLIqJtCzjNZcv27413088;     QJGysSQpLIqJtCzjNZcv27413088 = QJGysSQpLIqJtCzjNZcv67968189;     QJGysSQpLIqJtCzjNZcv67968189 = QJGysSQpLIqJtCzjNZcv41394790;     QJGysSQpLIqJtCzjNZcv41394790 = QJGysSQpLIqJtCzjNZcv10345222;     QJGysSQpLIqJtCzjNZcv10345222 = QJGysSQpLIqJtCzjNZcv39709993;     QJGysSQpLIqJtCzjNZcv39709993 = QJGysSQpLIqJtCzjNZcv66029210;     QJGysSQpLIqJtCzjNZcv66029210 = QJGysSQpLIqJtCzjNZcv54537587;     QJGysSQpLIqJtCzjNZcv54537587 = QJGysSQpLIqJtCzjNZcv33417100;     QJGysSQpLIqJtCzjNZcv33417100 = QJGysSQpLIqJtCzjNZcv31661154;     QJGysSQpLIqJtCzjNZcv31661154 = QJGysSQpLIqJtCzjNZcv33296928;     QJGysSQpLIqJtCzjNZcv33296928 = QJGysSQpLIqJtCzjNZcv84181547;     QJGysSQpLIqJtCzjNZcv84181547 = QJGysSQpLIqJtCzjNZcv36942126;     QJGysSQpLIqJtCzjNZcv36942126 = QJGysSQpLIqJtCzjNZcv20469335;     QJGysSQpLIqJtCzjNZcv20469335 = QJGysSQpLIqJtCzjNZcv31365306;     QJGysSQpLIqJtCzjNZcv31365306 = QJGysSQpLIqJtCzjNZcv35914266;     QJGysSQpLIqJtCzjNZcv35914266 = QJGysSQpLIqJtCzjNZcv16736913;     QJGysSQpLIqJtCzjNZcv16736913 = QJGysSQpLIqJtCzjNZcv3982296;     QJGysSQpLIqJtCzjNZcv3982296 = QJGysSQpLIqJtCzjNZcv22455894;     QJGysSQpLIqJtCzjNZcv22455894 = QJGysSQpLIqJtCzjNZcv72711172;     QJGysSQpLIqJtCzjNZcv72711172 = QJGysSQpLIqJtCzjNZcv61954993;     QJGysSQpLIqJtCzjNZcv61954993 = QJGysSQpLIqJtCzjNZcv4508872;     QJGysSQpLIqJtCzjNZcv4508872 = QJGysSQpLIqJtCzjNZcv47398572;     QJGysSQpLIqJtCzjNZcv47398572 = QJGysSQpLIqJtCzjNZcv21425119;     QJGysSQpLIqJtCzjNZcv21425119 = QJGysSQpLIqJtCzjNZcv54320952;     QJGysSQpLIqJtCzjNZcv54320952 = QJGysSQpLIqJtCzjNZcv13195751;     QJGysSQpLIqJtCzjNZcv13195751 = QJGysSQpLIqJtCzjNZcv2855837;     QJGysSQpLIqJtCzjNZcv2855837 = QJGysSQpLIqJtCzjNZcv51422512;     QJGysSQpLIqJtCzjNZcv51422512 = QJGysSQpLIqJtCzjNZcv82735024;     QJGysSQpLIqJtCzjNZcv82735024 = QJGysSQpLIqJtCzjNZcv86200687;     QJGysSQpLIqJtCzjNZcv86200687 = QJGysSQpLIqJtCzjNZcv25540224;     QJGysSQpLIqJtCzjNZcv25540224 = QJGysSQpLIqJtCzjNZcv86791265;     QJGysSQpLIqJtCzjNZcv86791265 = QJGysSQpLIqJtCzjNZcv85405823;     QJGysSQpLIqJtCzjNZcv85405823 = QJGysSQpLIqJtCzjNZcv14297525;     QJGysSQpLIqJtCzjNZcv14297525 = QJGysSQpLIqJtCzjNZcv75621372;     QJGysSQpLIqJtCzjNZcv75621372 = QJGysSQpLIqJtCzjNZcv4340776;     QJGysSQpLIqJtCzjNZcv4340776 = QJGysSQpLIqJtCzjNZcv58957924;     QJGysSQpLIqJtCzjNZcv58957924 = QJGysSQpLIqJtCzjNZcv3726115;     QJGysSQpLIqJtCzjNZcv3726115 = QJGysSQpLIqJtCzjNZcv87152841;     QJGysSQpLIqJtCzjNZcv87152841 = QJGysSQpLIqJtCzjNZcv34963788;     QJGysSQpLIqJtCzjNZcv34963788 = QJGysSQpLIqJtCzjNZcv45873487;     QJGysSQpLIqJtCzjNZcv45873487 = QJGysSQpLIqJtCzjNZcv90766692;     QJGysSQpLIqJtCzjNZcv90766692 = QJGysSQpLIqJtCzjNZcv44607995;     QJGysSQpLIqJtCzjNZcv44607995 = QJGysSQpLIqJtCzjNZcv76290962;     QJGysSQpLIqJtCzjNZcv76290962 = QJGysSQpLIqJtCzjNZcv15113517;     QJGysSQpLIqJtCzjNZcv15113517 = QJGysSQpLIqJtCzjNZcv89908874;     QJGysSQpLIqJtCzjNZcv89908874 = QJGysSQpLIqJtCzjNZcv16604482;     QJGysSQpLIqJtCzjNZcv16604482 = QJGysSQpLIqJtCzjNZcv84596804;     QJGysSQpLIqJtCzjNZcv84596804 = QJGysSQpLIqJtCzjNZcv62040416;     QJGysSQpLIqJtCzjNZcv62040416 = QJGysSQpLIqJtCzjNZcv64760109;     QJGysSQpLIqJtCzjNZcv64760109 = QJGysSQpLIqJtCzjNZcv33281793;     QJGysSQpLIqJtCzjNZcv33281793 = QJGysSQpLIqJtCzjNZcv64380951;     QJGysSQpLIqJtCzjNZcv64380951 = QJGysSQpLIqJtCzjNZcv37946986;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void RWyzhmtUVqGUwyxlhmLK17516189() {     float tuPjcLdvMEBzlcRXHHbS37940515 = -332740331;    float tuPjcLdvMEBzlcRXHHbS8208810 = -944147309;    float tuPjcLdvMEBzlcRXHHbS26877836 = -883071916;    float tuPjcLdvMEBzlcRXHHbS76695203 = -475543016;    float tuPjcLdvMEBzlcRXHHbS60088446 = -588195640;    float tuPjcLdvMEBzlcRXHHbS54732503 = -187459036;    float tuPjcLdvMEBzlcRXHHbS510351 = -846724251;    float tuPjcLdvMEBzlcRXHHbS32333220 = -735533135;    float tuPjcLdvMEBzlcRXHHbS45580544 = -308250350;    float tuPjcLdvMEBzlcRXHHbS58497330 = -476133190;    float tuPjcLdvMEBzlcRXHHbS21800964 = -169643878;    float tuPjcLdvMEBzlcRXHHbS11421352 = -216097833;    float tuPjcLdvMEBzlcRXHHbS16466783 = -954328243;    float tuPjcLdvMEBzlcRXHHbS61103492 = -833590763;    float tuPjcLdvMEBzlcRXHHbS27706075 = -976179887;    float tuPjcLdvMEBzlcRXHHbS29915235 = 16294970;    float tuPjcLdvMEBzlcRXHHbS25688323 = -326286983;    float tuPjcLdvMEBzlcRXHHbS51411906 = 198214;    float tuPjcLdvMEBzlcRXHHbS87170628 = -771462509;    float tuPjcLdvMEBzlcRXHHbS79995738 = -368905668;    float tuPjcLdvMEBzlcRXHHbS24497658 = -525342377;    float tuPjcLdvMEBzlcRXHHbS61705665 = -547578717;    float tuPjcLdvMEBzlcRXHHbS73097790 = -63743306;    float tuPjcLdvMEBzlcRXHHbS37553579 = -97849222;    float tuPjcLdvMEBzlcRXHHbS86761103 = -767784215;    float tuPjcLdvMEBzlcRXHHbS83640873 = -126022226;    float tuPjcLdvMEBzlcRXHHbS53206526 = -131372097;    float tuPjcLdvMEBzlcRXHHbS46897206 = -900434134;    float tuPjcLdvMEBzlcRXHHbS44432310 = -583566366;    float tuPjcLdvMEBzlcRXHHbS3015984 = -474582016;    float tuPjcLdvMEBzlcRXHHbS78533582 = -403890189;    float tuPjcLdvMEBzlcRXHHbS82221920 = -94097697;    float tuPjcLdvMEBzlcRXHHbS60126204 = -619836375;    float tuPjcLdvMEBzlcRXHHbS12185535 = -203929734;    float tuPjcLdvMEBzlcRXHHbS27662318 = -844393820;    float tuPjcLdvMEBzlcRXHHbS20853284 = -238749511;    float tuPjcLdvMEBzlcRXHHbS19893484 = 83562690;    float tuPjcLdvMEBzlcRXHHbS18328125 = -508425403;    float tuPjcLdvMEBzlcRXHHbS9404019 = -345092572;    float tuPjcLdvMEBzlcRXHHbS25419501 = -472868295;    float tuPjcLdvMEBzlcRXHHbS92921806 = -904768860;    float tuPjcLdvMEBzlcRXHHbS79103383 = -203755012;    float tuPjcLdvMEBzlcRXHHbS60957735 = -479924314;    float tuPjcLdvMEBzlcRXHHbS9250604 = -213029767;    float tuPjcLdvMEBzlcRXHHbS52381299 = -164653979;    float tuPjcLdvMEBzlcRXHHbS11955118 = -840737257;    float tuPjcLdvMEBzlcRXHHbS9358767 = -709410269;    float tuPjcLdvMEBzlcRXHHbS44071001 = -854074958;    float tuPjcLdvMEBzlcRXHHbS54530698 = -661299209;    float tuPjcLdvMEBzlcRXHHbS47458511 = -653466206;    float tuPjcLdvMEBzlcRXHHbS81227559 = -141317747;    float tuPjcLdvMEBzlcRXHHbS55942047 = -996971004;    float tuPjcLdvMEBzlcRXHHbS85793569 = 73787431;    float tuPjcLdvMEBzlcRXHHbS49960264 = -208938913;    float tuPjcLdvMEBzlcRXHHbS93018530 = -233832814;    float tuPjcLdvMEBzlcRXHHbS76234850 = -785161614;    float tuPjcLdvMEBzlcRXHHbS35111020 = -780404003;    float tuPjcLdvMEBzlcRXHHbS89324257 = -685222695;    float tuPjcLdvMEBzlcRXHHbS89934099 = -707758802;    float tuPjcLdvMEBzlcRXHHbS76447572 = -362173415;    float tuPjcLdvMEBzlcRXHHbS1525977 = 43913061;    float tuPjcLdvMEBzlcRXHHbS53613145 = -946290118;    float tuPjcLdvMEBzlcRXHHbS87900909 = -51966770;    float tuPjcLdvMEBzlcRXHHbS42564560 = -833668335;    float tuPjcLdvMEBzlcRXHHbS79963747 = 27756998;    float tuPjcLdvMEBzlcRXHHbS39579043 = 24453818;    float tuPjcLdvMEBzlcRXHHbS51295147 = -596261459;    float tuPjcLdvMEBzlcRXHHbS4281249 = -650398509;    float tuPjcLdvMEBzlcRXHHbS33441175 = -989196943;    float tuPjcLdvMEBzlcRXHHbS6852791 = -637430377;    float tuPjcLdvMEBzlcRXHHbS10021752 = 32732280;    float tuPjcLdvMEBzlcRXHHbS7360199 = -817861580;    float tuPjcLdvMEBzlcRXHHbS42007888 = -654709215;    float tuPjcLdvMEBzlcRXHHbS61751127 = -198594215;    float tuPjcLdvMEBzlcRXHHbS87073932 = -464136808;    float tuPjcLdvMEBzlcRXHHbS45394275 = -221587366;    float tuPjcLdvMEBzlcRXHHbS747930 = 32345596;    float tuPjcLdvMEBzlcRXHHbS63847186 = -850713539;    float tuPjcLdvMEBzlcRXHHbS85172279 = -933195243;    float tuPjcLdvMEBzlcRXHHbS74805986 = -927046958;    float tuPjcLdvMEBzlcRXHHbS74282107 = -416611957;    float tuPjcLdvMEBzlcRXHHbS9135525 = -277297140;    float tuPjcLdvMEBzlcRXHHbS92366507 = -139134925;    float tuPjcLdvMEBzlcRXHHbS96973799 = -930100160;    float tuPjcLdvMEBzlcRXHHbS21788425 = -233264269;    float tuPjcLdvMEBzlcRXHHbS22591536 = -406919186;    float tuPjcLdvMEBzlcRXHHbS96428350 = -67885129;    float tuPjcLdvMEBzlcRXHHbS10165940 = -310897462;    float tuPjcLdvMEBzlcRXHHbS19167004 = -970096920;    float tuPjcLdvMEBzlcRXHHbS51427468 = 40767794;    float tuPjcLdvMEBzlcRXHHbS85742264 = -458345508;    float tuPjcLdvMEBzlcRXHHbS30569226 = -231214616;    float tuPjcLdvMEBzlcRXHHbS28394026 = -800666602;    float tuPjcLdvMEBzlcRXHHbS32956446 = -982919157;    float tuPjcLdvMEBzlcRXHHbS23893524 = -416781356;    float tuPjcLdvMEBzlcRXHHbS39308661 = -958478743;    float tuPjcLdvMEBzlcRXHHbS91202473 = -51788243;    float tuPjcLdvMEBzlcRXHHbS18393175 = -646255979;    float tuPjcLdvMEBzlcRXHHbS29286857 = -140786766;    float tuPjcLdvMEBzlcRXHHbS12802256 = -332740331;     tuPjcLdvMEBzlcRXHHbS37940515 = tuPjcLdvMEBzlcRXHHbS8208810;     tuPjcLdvMEBzlcRXHHbS8208810 = tuPjcLdvMEBzlcRXHHbS26877836;     tuPjcLdvMEBzlcRXHHbS26877836 = tuPjcLdvMEBzlcRXHHbS76695203;     tuPjcLdvMEBzlcRXHHbS76695203 = tuPjcLdvMEBzlcRXHHbS60088446;     tuPjcLdvMEBzlcRXHHbS60088446 = tuPjcLdvMEBzlcRXHHbS54732503;     tuPjcLdvMEBzlcRXHHbS54732503 = tuPjcLdvMEBzlcRXHHbS510351;     tuPjcLdvMEBzlcRXHHbS510351 = tuPjcLdvMEBzlcRXHHbS32333220;     tuPjcLdvMEBzlcRXHHbS32333220 = tuPjcLdvMEBzlcRXHHbS45580544;     tuPjcLdvMEBzlcRXHHbS45580544 = tuPjcLdvMEBzlcRXHHbS58497330;     tuPjcLdvMEBzlcRXHHbS58497330 = tuPjcLdvMEBzlcRXHHbS21800964;     tuPjcLdvMEBzlcRXHHbS21800964 = tuPjcLdvMEBzlcRXHHbS11421352;     tuPjcLdvMEBzlcRXHHbS11421352 = tuPjcLdvMEBzlcRXHHbS16466783;     tuPjcLdvMEBzlcRXHHbS16466783 = tuPjcLdvMEBzlcRXHHbS61103492;     tuPjcLdvMEBzlcRXHHbS61103492 = tuPjcLdvMEBzlcRXHHbS27706075;     tuPjcLdvMEBzlcRXHHbS27706075 = tuPjcLdvMEBzlcRXHHbS29915235;     tuPjcLdvMEBzlcRXHHbS29915235 = tuPjcLdvMEBzlcRXHHbS25688323;     tuPjcLdvMEBzlcRXHHbS25688323 = tuPjcLdvMEBzlcRXHHbS51411906;     tuPjcLdvMEBzlcRXHHbS51411906 = tuPjcLdvMEBzlcRXHHbS87170628;     tuPjcLdvMEBzlcRXHHbS87170628 = tuPjcLdvMEBzlcRXHHbS79995738;     tuPjcLdvMEBzlcRXHHbS79995738 = tuPjcLdvMEBzlcRXHHbS24497658;     tuPjcLdvMEBzlcRXHHbS24497658 = tuPjcLdvMEBzlcRXHHbS61705665;     tuPjcLdvMEBzlcRXHHbS61705665 = tuPjcLdvMEBzlcRXHHbS73097790;     tuPjcLdvMEBzlcRXHHbS73097790 = tuPjcLdvMEBzlcRXHHbS37553579;     tuPjcLdvMEBzlcRXHHbS37553579 = tuPjcLdvMEBzlcRXHHbS86761103;     tuPjcLdvMEBzlcRXHHbS86761103 = tuPjcLdvMEBzlcRXHHbS83640873;     tuPjcLdvMEBzlcRXHHbS83640873 = tuPjcLdvMEBzlcRXHHbS53206526;     tuPjcLdvMEBzlcRXHHbS53206526 = tuPjcLdvMEBzlcRXHHbS46897206;     tuPjcLdvMEBzlcRXHHbS46897206 = tuPjcLdvMEBzlcRXHHbS44432310;     tuPjcLdvMEBzlcRXHHbS44432310 = tuPjcLdvMEBzlcRXHHbS3015984;     tuPjcLdvMEBzlcRXHHbS3015984 = tuPjcLdvMEBzlcRXHHbS78533582;     tuPjcLdvMEBzlcRXHHbS78533582 = tuPjcLdvMEBzlcRXHHbS82221920;     tuPjcLdvMEBzlcRXHHbS82221920 = tuPjcLdvMEBzlcRXHHbS60126204;     tuPjcLdvMEBzlcRXHHbS60126204 = tuPjcLdvMEBzlcRXHHbS12185535;     tuPjcLdvMEBzlcRXHHbS12185535 = tuPjcLdvMEBzlcRXHHbS27662318;     tuPjcLdvMEBzlcRXHHbS27662318 = tuPjcLdvMEBzlcRXHHbS20853284;     tuPjcLdvMEBzlcRXHHbS20853284 = tuPjcLdvMEBzlcRXHHbS19893484;     tuPjcLdvMEBzlcRXHHbS19893484 = tuPjcLdvMEBzlcRXHHbS18328125;     tuPjcLdvMEBzlcRXHHbS18328125 = tuPjcLdvMEBzlcRXHHbS9404019;     tuPjcLdvMEBzlcRXHHbS9404019 = tuPjcLdvMEBzlcRXHHbS25419501;     tuPjcLdvMEBzlcRXHHbS25419501 = tuPjcLdvMEBzlcRXHHbS92921806;     tuPjcLdvMEBzlcRXHHbS92921806 = tuPjcLdvMEBzlcRXHHbS79103383;     tuPjcLdvMEBzlcRXHHbS79103383 = tuPjcLdvMEBzlcRXHHbS60957735;     tuPjcLdvMEBzlcRXHHbS60957735 = tuPjcLdvMEBzlcRXHHbS9250604;     tuPjcLdvMEBzlcRXHHbS9250604 = tuPjcLdvMEBzlcRXHHbS52381299;     tuPjcLdvMEBzlcRXHHbS52381299 = tuPjcLdvMEBzlcRXHHbS11955118;     tuPjcLdvMEBzlcRXHHbS11955118 = tuPjcLdvMEBzlcRXHHbS9358767;     tuPjcLdvMEBzlcRXHHbS9358767 = tuPjcLdvMEBzlcRXHHbS44071001;     tuPjcLdvMEBzlcRXHHbS44071001 = tuPjcLdvMEBzlcRXHHbS54530698;     tuPjcLdvMEBzlcRXHHbS54530698 = tuPjcLdvMEBzlcRXHHbS47458511;     tuPjcLdvMEBzlcRXHHbS47458511 = tuPjcLdvMEBzlcRXHHbS81227559;     tuPjcLdvMEBzlcRXHHbS81227559 = tuPjcLdvMEBzlcRXHHbS55942047;     tuPjcLdvMEBzlcRXHHbS55942047 = tuPjcLdvMEBzlcRXHHbS85793569;     tuPjcLdvMEBzlcRXHHbS85793569 = tuPjcLdvMEBzlcRXHHbS49960264;     tuPjcLdvMEBzlcRXHHbS49960264 = tuPjcLdvMEBzlcRXHHbS93018530;     tuPjcLdvMEBzlcRXHHbS93018530 = tuPjcLdvMEBzlcRXHHbS76234850;     tuPjcLdvMEBzlcRXHHbS76234850 = tuPjcLdvMEBzlcRXHHbS35111020;     tuPjcLdvMEBzlcRXHHbS35111020 = tuPjcLdvMEBzlcRXHHbS89324257;     tuPjcLdvMEBzlcRXHHbS89324257 = tuPjcLdvMEBzlcRXHHbS89934099;     tuPjcLdvMEBzlcRXHHbS89934099 = tuPjcLdvMEBzlcRXHHbS76447572;     tuPjcLdvMEBzlcRXHHbS76447572 = tuPjcLdvMEBzlcRXHHbS1525977;     tuPjcLdvMEBzlcRXHHbS1525977 = tuPjcLdvMEBzlcRXHHbS53613145;     tuPjcLdvMEBzlcRXHHbS53613145 = tuPjcLdvMEBzlcRXHHbS87900909;     tuPjcLdvMEBzlcRXHHbS87900909 = tuPjcLdvMEBzlcRXHHbS42564560;     tuPjcLdvMEBzlcRXHHbS42564560 = tuPjcLdvMEBzlcRXHHbS79963747;     tuPjcLdvMEBzlcRXHHbS79963747 = tuPjcLdvMEBzlcRXHHbS39579043;     tuPjcLdvMEBzlcRXHHbS39579043 = tuPjcLdvMEBzlcRXHHbS51295147;     tuPjcLdvMEBzlcRXHHbS51295147 = tuPjcLdvMEBzlcRXHHbS4281249;     tuPjcLdvMEBzlcRXHHbS4281249 = tuPjcLdvMEBzlcRXHHbS33441175;     tuPjcLdvMEBzlcRXHHbS33441175 = tuPjcLdvMEBzlcRXHHbS6852791;     tuPjcLdvMEBzlcRXHHbS6852791 = tuPjcLdvMEBzlcRXHHbS10021752;     tuPjcLdvMEBzlcRXHHbS10021752 = tuPjcLdvMEBzlcRXHHbS7360199;     tuPjcLdvMEBzlcRXHHbS7360199 = tuPjcLdvMEBzlcRXHHbS42007888;     tuPjcLdvMEBzlcRXHHbS42007888 = tuPjcLdvMEBzlcRXHHbS61751127;     tuPjcLdvMEBzlcRXHHbS61751127 = tuPjcLdvMEBzlcRXHHbS87073932;     tuPjcLdvMEBzlcRXHHbS87073932 = tuPjcLdvMEBzlcRXHHbS45394275;     tuPjcLdvMEBzlcRXHHbS45394275 = tuPjcLdvMEBzlcRXHHbS747930;     tuPjcLdvMEBzlcRXHHbS747930 = tuPjcLdvMEBzlcRXHHbS63847186;     tuPjcLdvMEBzlcRXHHbS63847186 = tuPjcLdvMEBzlcRXHHbS85172279;     tuPjcLdvMEBzlcRXHHbS85172279 = tuPjcLdvMEBzlcRXHHbS74805986;     tuPjcLdvMEBzlcRXHHbS74805986 = tuPjcLdvMEBzlcRXHHbS74282107;     tuPjcLdvMEBzlcRXHHbS74282107 = tuPjcLdvMEBzlcRXHHbS9135525;     tuPjcLdvMEBzlcRXHHbS9135525 = tuPjcLdvMEBzlcRXHHbS92366507;     tuPjcLdvMEBzlcRXHHbS92366507 = tuPjcLdvMEBzlcRXHHbS96973799;     tuPjcLdvMEBzlcRXHHbS96973799 = tuPjcLdvMEBzlcRXHHbS21788425;     tuPjcLdvMEBzlcRXHHbS21788425 = tuPjcLdvMEBzlcRXHHbS22591536;     tuPjcLdvMEBzlcRXHHbS22591536 = tuPjcLdvMEBzlcRXHHbS96428350;     tuPjcLdvMEBzlcRXHHbS96428350 = tuPjcLdvMEBzlcRXHHbS10165940;     tuPjcLdvMEBzlcRXHHbS10165940 = tuPjcLdvMEBzlcRXHHbS19167004;     tuPjcLdvMEBzlcRXHHbS19167004 = tuPjcLdvMEBzlcRXHHbS51427468;     tuPjcLdvMEBzlcRXHHbS51427468 = tuPjcLdvMEBzlcRXHHbS85742264;     tuPjcLdvMEBzlcRXHHbS85742264 = tuPjcLdvMEBzlcRXHHbS30569226;     tuPjcLdvMEBzlcRXHHbS30569226 = tuPjcLdvMEBzlcRXHHbS28394026;     tuPjcLdvMEBzlcRXHHbS28394026 = tuPjcLdvMEBzlcRXHHbS32956446;     tuPjcLdvMEBzlcRXHHbS32956446 = tuPjcLdvMEBzlcRXHHbS23893524;     tuPjcLdvMEBzlcRXHHbS23893524 = tuPjcLdvMEBzlcRXHHbS39308661;     tuPjcLdvMEBzlcRXHHbS39308661 = tuPjcLdvMEBzlcRXHHbS91202473;     tuPjcLdvMEBzlcRXHHbS91202473 = tuPjcLdvMEBzlcRXHHbS18393175;     tuPjcLdvMEBzlcRXHHbS18393175 = tuPjcLdvMEBzlcRXHHbS29286857;     tuPjcLdvMEBzlcRXHHbS29286857 = tuPjcLdvMEBzlcRXHHbS12802256;     tuPjcLdvMEBzlcRXHHbS12802256 = tuPjcLdvMEBzlcRXHHbS37940515;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void KRuXBQyRZjyEIrEEbdwl2248829() {     float ZsVsJkkDUBTHPzOswCPH76324901 = -147765840;    float ZsVsJkkDUBTHPzOswCPH14232208 = -184768848;    float ZsVsJkkDUBTHPzOswCPH74883387 = -79635606;    float ZsVsJkkDUBTHPzOswCPH60198236 = -665986526;    float ZsVsJkkDUBTHPzOswCPH5261352 = -190223750;    float ZsVsJkkDUBTHPzOswCPH45440985 = -720497729;    float ZsVsJkkDUBTHPzOswCPH89944281 = -365594595;    float ZsVsJkkDUBTHPzOswCPH35634356 = 47621363;    float ZsVsJkkDUBTHPzOswCPH834319 = -238940347;    float ZsVsJkkDUBTHPzOswCPH90595390 = -241168604;    float ZsVsJkkDUBTHPzOswCPH22980689 = -675813636;    float ZsVsJkkDUBTHPzOswCPH50106158 = -596182831;    float ZsVsJkkDUBTHPzOswCPH99485035 = -579042316;    float ZsVsJkkDUBTHPzOswCPH94598847 = -450224151;    float ZsVsJkkDUBTHPzOswCPH16937298 = -846285972;    float ZsVsJkkDUBTHPzOswCPH16932252 = -305196595;    float ZsVsJkkDUBTHPzOswCPH69532738 = -39152104;    float ZsVsJkkDUBTHPzOswCPH10085230 = -377146205;    float ZsVsJkkDUBTHPzOswCPH8939440 = -227941029;    float ZsVsJkkDUBTHPzOswCPH99664175 = -818540280;    float ZsVsJkkDUBTHPzOswCPH46342132 = -957425675;    float ZsVsJkkDUBTHPzOswCPH35664059 = -117186228;    float ZsVsJkkDUBTHPzOswCPH41865090 = -220923622;    float ZsVsJkkDUBTHPzOswCPH96474533 = -555202169;    float ZsVsJkkDUBTHPzOswCPH90673123 = -792334576;    float ZsVsJkkDUBTHPzOswCPH61300502 = -664492110;    float ZsVsJkkDUBTHPzOswCPH6566808 = -829292086;    float ZsVsJkkDUBTHPzOswCPH7341881 = -85960495;    float ZsVsJkkDUBTHPzOswCPH43569682 = -841936826;    float ZsVsJkkDUBTHPzOswCPH56909850 = -662514985;    float ZsVsJkkDUBTHPzOswCPH78190855 = -784767501;    float ZsVsJkkDUBTHPzOswCPH53282961 = -662969686;    float ZsVsJkkDUBTHPzOswCPH56454359 = -539623864;    float ZsVsJkkDUBTHPzOswCPH42283304 = -411465459;    float ZsVsJkkDUBTHPzOswCPH47411762 = -537233762;    float ZsVsJkkDUBTHPzOswCPH35212870 = -385375838;    float ZsVsJkkDUBTHPzOswCPH82420194 = 12448151;    float ZsVsJkkDUBTHPzOswCPH11769475 = -286234962;    float ZsVsJkkDUBTHPzOswCPH85475789 = -319918639;    float ZsVsJkkDUBTHPzOswCPH16722119 = -468392088;    float ZsVsJkkDUBTHPzOswCPH25793061 = -579415247;    float ZsVsJkkDUBTHPzOswCPH59420107 = -223520686;    float ZsVsJkkDUBTHPzOswCPH24474704 = -513703632;    float ZsVsJkkDUBTHPzOswCPH6208625 = -808688722;    float ZsVsJkkDUBTHPzOswCPH56334561 = -149926253;    float ZsVsJkkDUBTHPzOswCPH71127632 = -712513893;    float ZsVsJkkDUBTHPzOswCPH31313655 = -276832938;    float ZsVsJkkDUBTHPzOswCPH30856576 = 14912129;    float ZsVsJkkDUBTHPzOswCPH12089289 = -697049688;    float ZsVsJkkDUBTHPzOswCPH8128081 = -838476050;    float ZsVsJkkDUBTHPzOswCPH83172560 = -385750643;    float ZsVsJkkDUBTHPzOswCPH82282659 = -881944025;    float ZsVsJkkDUBTHPzOswCPH87455626 = -886006383;    float ZsVsJkkDUBTHPzOswCPH43058357 = -149614734;    float ZsVsJkkDUBTHPzOswCPH55906697 = -89569163;    float ZsVsJkkDUBTHPzOswCPH40660843 = 69420387;    float ZsVsJkkDUBTHPzOswCPH72367117 = -963845227;    float ZsVsJkkDUBTHPzOswCPH78408853 = -524433438;    float ZsVsJkkDUBTHPzOswCPH69525113 = -873651951;    float ZsVsJkkDUBTHPzOswCPH43960849 = -525731640;    float ZsVsJkkDUBTHPzOswCPH38874178 = -891205643;    float ZsVsJkkDUBTHPzOswCPH82602401 = -179634101;    float ZsVsJkkDUBTHPzOswCPH92064673 = -110441811;    float ZsVsJkkDUBTHPzOswCPH43924469 = -576425363;    float ZsVsJkkDUBTHPzOswCPH12404535 = -456401104;    float ZsVsJkkDUBTHPzOswCPH69697727 = 87156050;    float ZsVsJkkDUBTHPzOswCPH93651799 = 43441033;    float ZsVsJkkDUBTHPzOswCPH57201732 = -67576858;    float ZsVsJkkDUBTHPzOswCPH47187086 = -912990390;    float ZsVsJkkDUBTHPzOswCPH81724427 = -360910134;    float ZsVsJkkDUBTHPzOswCPH34512058 = -217644746;    float ZsVsJkkDUBTHPzOswCPH57763263 = -752917143;    float ZsVsJkkDUBTHPzOswCPH24609441 = 42772433;    float ZsVsJkkDUBTHPzOswCPH92217321 = -759548942;    float ZsVsJkkDUBTHPzOswCPH73871115 = -139125034;    float ZsVsJkkDUBTHPzOswCPH86922024 = -633904990;    float ZsVsJkkDUBTHPzOswCPH11189355 = -603482597;    float ZsVsJkkDUBTHPzOswCPH35656466 = -412234900;    float ZsVsJkkDUBTHPzOswCPH40139973 = -305275916;    float ZsVsJkkDUBTHPzOswCPH19545491 = 20179317;    float ZsVsJkkDUBTHPzOswCPH29986847 = -287659173;    float ZsVsJkkDUBTHPzOswCPH75710231 = -744204215;    float ZsVsJkkDUBTHPzOswCPH95252592 = -388910807;    float ZsVsJkkDUBTHPzOswCPH35441601 = 96539223;    float ZsVsJkkDUBTHPzOswCPH73737289 = -176764342;    float ZsVsJkkDUBTHPzOswCPH95908196 = -902823477;    float ZsVsJkkDUBTHPzOswCPH65827334 = -776963303;    float ZsVsJkkDUBTHPzOswCPH13396002 = -290009131;    float ZsVsJkkDUBTHPzOswCPH86376606 = -221896296;    float ZsVsJkkDUBTHPzOswCPH6750920 = -506654150;    float ZsVsJkkDUBTHPzOswCPH62845752 = -421530611;    float ZsVsJkkDUBTHPzOswCPH4011341 = -463118411;    float ZsVsJkkDUBTHPzOswCPH42244362 = -412583011;    float ZsVsJkkDUBTHPzOswCPH41514940 = -794186999;    float ZsVsJkkDUBTHPzOswCPH77847941 = -577186445;    float ZsVsJkkDUBTHPzOswCPH43190659 = -299781147;    float ZsVsJkkDUBTHPzOswCPH67355434 = -13078876;    float ZsVsJkkDUBTHPzOswCPH80550235 = -937278269;    float ZsVsJkkDUBTHPzOswCPH93804089 = -252287619;    float ZsVsJkkDUBTHPzOswCPH86636833 = -147765840;     ZsVsJkkDUBTHPzOswCPH76324901 = ZsVsJkkDUBTHPzOswCPH14232208;     ZsVsJkkDUBTHPzOswCPH14232208 = ZsVsJkkDUBTHPzOswCPH74883387;     ZsVsJkkDUBTHPzOswCPH74883387 = ZsVsJkkDUBTHPzOswCPH60198236;     ZsVsJkkDUBTHPzOswCPH60198236 = ZsVsJkkDUBTHPzOswCPH5261352;     ZsVsJkkDUBTHPzOswCPH5261352 = ZsVsJkkDUBTHPzOswCPH45440985;     ZsVsJkkDUBTHPzOswCPH45440985 = ZsVsJkkDUBTHPzOswCPH89944281;     ZsVsJkkDUBTHPzOswCPH89944281 = ZsVsJkkDUBTHPzOswCPH35634356;     ZsVsJkkDUBTHPzOswCPH35634356 = ZsVsJkkDUBTHPzOswCPH834319;     ZsVsJkkDUBTHPzOswCPH834319 = ZsVsJkkDUBTHPzOswCPH90595390;     ZsVsJkkDUBTHPzOswCPH90595390 = ZsVsJkkDUBTHPzOswCPH22980689;     ZsVsJkkDUBTHPzOswCPH22980689 = ZsVsJkkDUBTHPzOswCPH50106158;     ZsVsJkkDUBTHPzOswCPH50106158 = ZsVsJkkDUBTHPzOswCPH99485035;     ZsVsJkkDUBTHPzOswCPH99485035 = ZsVsJkkDUBTHPzOswCPH94598847;     ZsVsJkkDUBTHPzOswCPH94598847 = ZsVsJkkDUBTHPzOswCPH16937298;     ZsVsJkkDUBTHPzOswCPH16937298 = ZsVsJkkDUBTHPzOswCPH16932252;     ZsVsJkkDUBTHPzOswCPH16932252 = ZsVsJkkDUBTHPzOswCPH69532738;     ZsVsJkkDUBTHPzOswCPH69532738 = ZsVsJkkDUBTHPzOswCPH10085230;     ZsVsJkkDUBTHPzOswCPH10085230 = ZsVsJkkDUBTHPzOswCPH8939440;     ZsVsJkkDUBTHPzOswCPH8939440 = ZsVsJkkDUBTHPzOswCPH99664175;     ZsVsJkkDUBTHPzOswCPH99664175 = ZsVsJkkDUBTHPzOswCPH46342132;     ZsVsJkkDUBTHPzOswCPH46342132 = ZsVsJkkDUBTHPzOswCPH35664059;     ZsVsJkkDUBTHPzOswCPH35664059 = ZsVsJkkDUBTHPzOswCPH41865090;     ZsVsJkkDUBTHPzOswCPH41865090 = ZsVsJkkDUBTHPzOswCPH96474533;     ZsVsJkkDUBTHPzOswCPH96474533 = ZsVsJkkDUBTHPzOswCPH90673123;     ZsVsJkkDUBTHPzOswCPH90673123 = ZsVsJkkDUBTHPzOswCPH61300502;     ZsVsJkkDUBTHPzOswCPH61300502 = ZsVsJkkDUBTHPzOswCPH6566808;     ZsVsJkkDUBTHPzOswCPH6566808 = ZsVsJkkDUBTHPzOswCPH7341881;     ZsVsJkkDUBTHPzOswCPH7341881 = ZsVsJkkDUBTHPzOswCPH43569682;     ZsVsJkkDUBTHPzOswCPH43569682 = ZsVsJkkDUBTHPzOswCPH56909850;     ZsVsJkkDUBTHPzOswCPH56909850 = ZsVsJkkDUBTHPzOswCPH78190855;     ZsVsJkkDUBTHPzOswCPH78190855 = ZsVsJkkDUBTHPzOswCPH53282961;     ZsVsJkkDUBTHPzOswCPH53282961 = ZsVsJkkDUBTHPzOswCPH56454359;     ZsVsJkkDUBTHPzOswCPH56454359 = ZsVsJkkDUBTHPzOswCPH42283304;     ZsVsJkkDUBTHPzOswCPH42283304 = ZsVsJkkDUBTHPzOswCPH47411762;     ZsVsJkkDUBTHPzOswCPH47411762 = ZsVsJkkDUBTHPzOswCPH35212870;     ZsVsJkkDUBTHPzOswCPH35212870 = ZsVsJkkDUBTHPzOswCPH82420194;     ZsVsJkkDUBTHPzOswCPH82420194 = ZsVsJkkDUBTHPzOswCPH11769475;     ZsVsJkkDUBTHPzOswCPH11769475 = ZsVsJkkDUBTHPzOswCPH85475789;     ZsVsJkkDUBTHPzOswCPH85475789 = ZsVsJkkDUBTHPzOswCPH16722119;     ZsVsJkkDUBTHPzOswCPH16722119 = ZsVsJkkDUBTHPzOswCPH25793061;     ZsVsJkkDUBTHPzOswCPH25793061 = ZsVsJkkDUBTHPzOswCPH59420107;     ZsVsJkkDUBTHPzOswCPH59420107 = ZsVsJkkDUBTHPzOswCPH24474704;     ZsVsJkkDUBTHPzOswCPH24474704 = ZsVsJkkDUBTHPzOswCPH6208625;     ZsVsJkkDUBTHPzOswCPH6208625 = ZsVsJkkDUBTHPzOswCPH56334561;     ZsVsJkkDUBTHPzOswCPH56334561 = ZsVsJkkDUBTHPzOswCPH71127632;     ZsVsJkkDUBTHPzOswCPH71127632 = ZsVsJkkDUBTHPzOswCPH31313655;     ZsVsJkkDUBTHPzOswCPH31313655 = ZsVsJkkDUBTHPzOswCPH30856576;     ZsVsJkkDUBTHPzOswCPH30856576 = ZsVsJkkDUBTHPzOswCPH12089289;     ZsVsJkkDUBTHPzOswCPH12089289 = ZsVsJkkDUBTHPzOswCPH8128081;     ZsVsJkkDUBTHPzOswCPH8128081 = ZsVsJkkDUBTHPzOswCPH83172560;     ZsVsJkkDUBTHPzOswCPH83172560 = ZsVsJkkDUBTHPzOswCPH82282659;     ZsVsJkkDUBTHPzOswCPH82282659 = ZsVsJkkDUBTHPzOswCPH87455626;     ZsVsJkkDUBTHPzOswCPH87455626 = ZsVsJkkDUBTHPzOswCPH43058357;     ZsVsJkkDUBTHPzOswCPH43058357 = ZsVsJkkDUBTHPzOswCPH55906697;     ZsVsJkkDUBTHPzOswCPH55906697 = ZsVsJkkDUBTHPzOswCPH40660843;     ZsVsJkkDUBTHPzOswCPH40660843 = ZsVsJkkDUBTHPzOswCPH72367117;     ZsVsJkkDUBTHPzOswCPH72367117 = ZsVsJkkDUBTHPzOswCPH78408853;     ZsVsJkkDUBTHPzOswCPH78408853 = ZsVsJkkDUBTHPzOswCPH69525113;     ZsVsJkkDUBTHPzOswCPH69525113 = ZsVsJkkDUBTHPzOswCPH43960849;     ZsVsJkkDUBTHPzOswCPH43960849 = ZsVsJkkDUBTHPzOswCPH38874178;     ZsVsJkkDUBTHPzOswCPH38874178 = ZsVsJkkDUBTHPzOswCPH82602401;     ZsVsJkkDUBTHPzOswCPH82602401 = ZsVsJkkDUBTHPzOswCPH92064673;     ZsVsJkkDUBTHPzOswCPH92064673 = ZsVsJkkDUBTHPzOswCPH43924469;     ZsVsJkkDUBTHPzOswCPH43924469 = ZsVsJkkDUBTHPzOswCPH12404535;     ZsVsJkkDUBTHPzOswCPH12404535 = ZsVsJkkDUBTHPzOswCPH69697727;     ZsVsJkkDUBTHPzOswCPH69697727 = ZsVsJkkDUBTHPzOswCPH93651799;     ZsVsJkkDUBTHPzOswCPH93651799 = ZsVsJkkDUBTHPzOswCPH57201732;     ZsVsJkkDUBTHPzOswCPH57201732 = ZsVsJkkDUBTHPzOswCPH47187086;     ZsVsJkkDUBTHPzOswCPH47187086 = ZsVsJkkDUBTHPzOswCPH81724427;     ZsVsJkkDUBTHPzOswCPH81724427 = ZsVsJkkDUBTHPzOswCPH34512058;     ZsVsJkkDUBTHPzOswCPH34512058 = ZsVsJkkDUBTHPzOswCPH57763263;     ZsVsJkkDUBTHPzOswCPH57763263 = ZsVsJkkDUBTHPzOswCPH24609441;     ZsVsJkkDUBTHPzOswCPH24609441 = ZsVsJkkDUBTHPzOswCPH92217321;     ZsVsJkkDUBTHPzOswCPH92217321 = ZsVsJkkDUBTHPzOswCPH73871115;     ZsVsJkkDUBTHPzOswCPH73871115 = ZsVsJkkDUBTHPzOswCPH86922024;     ZsVsJkkDUBTHPzOswCPH86922024 = ZsVsJkkDUBTHPzOswCPH11189355;     ZsVsJkkDUBTHPzOswCPH11189355 = ZsVsJkkDUBTHPzOswCPH35656466;     ZsVsJkkDUBTHPzOswCPH35656466 = ZsVsJkkDUBTHPzOswCPH40139973;     ZsVsJkkDUBTHPzOswCPH40139973 = ZsVsJkkDUBTHPzOswCPH19545491;     ZsVsJkkDUBTHPzOswCPH19545491 = ZsVsJkkDUBTHPzOswCPH29986847;     ZsVsJkkDUBTHPzOswCPH29986847 = ZsVsJkkDUBTHPzOswCPH75710231;     ZsVsJkkDUBTHPzOswCPH75710231 = ZsVsJkkDUBTHPzOswCPH95252592;     ZsVsJkkDUBTHPzOswCPH95252592 = ZsVsJkkDUBTHPzOswCPH35441601;     ZsVsJkkDUBTHPzOswCPH35441601 = ZsVsJkkDUBTHPzOswCPH73737289;     ZsVsJkkDUBTHPzOswCPH73737289 = ZsVsJkkDUBTHPzOswCPH95908196;     ZsVsJkkDUBTHPzOswCPH95908196 = ZsVsJkkDUBTHPzOswCPH65827334;     ZsVsJkkDUBTHPzOswCPH65827334 = ZsVsJkkDUBTHPzOswCPH13396002;     ZsVsJkkDUBTHPzOswCPH13396002 = ZsVsJkkDUBTHPzOswCPH86376606;     ZsVsJkkDUBTHPzOswCPH86376606 = ZsVsJkkDUBTHPzOswCPH6750920;     ZsVsJkkDUBTHPzOswCPH6750920 = ZsVsJkkDUBTHPzOswCPH62845752;     ZsVsJkkDUBTHPzOswCPH62845752 = ZsVsJkkDUBTHPzOswCPH4011341;     ZsVsJkkDUBTHPzOswCPH4011341 = ZsVsJkkDUBTHPzOswCPH42244362;     ZsVsJkkDUBTHPzOswCPH42244362 = ZsVsJkkDUBTHPzOswCPH41514940;     ZsVsJkkDUBTHPzOswCPH41514940 = ZsVsJkkDUBTHPzOswCPH77847941;     ZsVsJkkDUBTHPzOswCPH77847941 = ZsVsJkkDUBTHPzOswCPH43190659;     ZsVsJkkDUBTHPzOswCPH43190659 = ZsVsJkkDUBTHPzOswCPH67355434;     ZsVsJkkDUBTHPzOswCPH67355434 = ZsVsJkkDUBTHPzOswCPH80550235;     ZsVsJkkDUBTHPzOswCPH80550235 = ZsVsJkkDUBTHPzOswCPH93804089;     ZsVsJkkDUBTHPzOswCPH93804089 = ZsVsJkkDUBTHPzOswCPH86636833;     ZsVsJkkDUBTHPzOswCPH86636833 = ZsVsJkkDUBTHPzOswCPH76324901;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void lvCRebmuDXuxMybJvAnA8907574() {     float lGIPMnJZnxtDjNrsYzNL76318430 = -20289982;    float lGIPMnJZnxtDjNrsYzNL52804524 = -227363974;    float lGIPMnJZnxtDjNrsYzNL76196599 = -36764220;    float lGIPMnJZnxtDjNrsYzNL45278097 = -486527356;    float lGIPMnJZnxtDjNrsYzNL14101454 = -658579071;    float lGIPMnJZnxtDjNrsYzNL80246417 = -419930541;    float lGIPMnJZnxtDjNrsYzNL58491618 = -70131062;    float lGIPMnJZnxtDjNrsYzNL4848406 = -756555954;    float lGIPMnJZnxtDjNrsYzNL83574482 = 47413583;    float lGIPMnJZnxtDjNrsYzNL87234903 = -977975476;    float lGIPMnJZnxtDjNrsYzNL43301294 = -635285708;    float lGIPMnJZnxtDjNrsYzNL64397835 = -331661193;    float lGIPMnJZnxtDjNrsYzNL31337950 = -600700791;    float lGIPMnJZnxtDjNrsYzNL48443068 = -795377203;    float lGIPMnJZnxtDjNrsYzNL62109406 = -808998824;    float lGIPMnJZnxtDjNrsYzNL91496799 = -637992245;    float lGIPMnJZnxtDjNrsYzNL25385498 = -958766674;    float lGIPMnJZnxtDjNrsYzNL33085763 = -774217827;    float lGIPMnJZnxtDjNrsYzNL29367710 = -845923295;    float lGIPMnJZnxtDjNrsYzNL71737938 = -330564732;    float lGIPMnJZnxtDjNrsYzNL26011556 = -667892049;    float lGIPMnJZnxtDjNrsYzNL13960325 = -627178111;    float lGIPMnJZnxtDjNrsYzNL78743486 = -679881464;    float lGIPMnJZnxtDjNrsYzNL40124642 = -436122691;    float lGIPMnJZnxtDjNrsYzNL19115813 = -119028429;    float lGIPMnJZnxtDjNrsYzNL77874578 = -572698038;    float lGIPMnJZnxtDjNrsYzNL76788389 = -30091799;    float lGIPMnJZnxtDjNrsYzNL42745406 = -694927483;    float lGIPMnJZnxtDjNrsYzNL56248129 = -388007141;    float lGIPMnJZnxtDjNrsYzNL32999720 = -337946358;    float lGIPMnJZnxtDjNrsYzNL11603535 = -71860736;    float lGIPMnJZnxtDjNrsYzNL38006819 = 45208553;    float lGIPMnJZnxtDjNrsYzNL41906782 = -741568080;    float lGIPMnJZnxtDjNrsYzNL42566141 = -288147191;    float lGIPMnJZnxtDjNrsYzNL29769800 = -796547687;    float lGIPMnJZnxtDjNrsYzNL78041059 = -465141585;    float lGIPMnJZnxtDjNrsYzNL94361561 = -87205227;    float lGIPMnJZnxtDjNrsYzNL81687154 = -281497641;    float lGIPMnJZnxtDjNrsYzNL20789386 = 67399945;    float lGIPMnJZnxtDjNrsYzNL88595011 = -77823774;    float lGIPMnJZnxtDjNrsYzNL13648728 = -667359145;    float lGIPMnJZnxtDjNrsYzNL45117768 = -417312768;    float lGIPMnJZnxtDjNrsYzNL84758064 = 27166114;    float lGIPMnJZnxtDjNrsYzNL65440522 = -365653166;    float lGIPMnJZnxtDjNrsYzNL40352613 = -623185655;    float lGIPMnJZnxtDjNrsYzNL11555601 = -524558564;    float lGIPMnJZnxtDjNrsYzNL59011448 = -758113560;    float lGIPMnJZnxtDjNrsYzNL6240158 = -593849228;    float lGIPMnJZnxtDjNrsYzNL30747678 = -260813549;    float lGIPMnJZnxtDjNrsYzNL28173504 = -425500936;    float lGIPMnJZnxtDjNrsYzNL96431930 = 35694349;    float lGIPMnJZnxtDjNrsYzNL96829916 = -164089773;    float lGIPMnJZnxtDjNrsYzNL62903975 = -778060392;    float lGIPMnJZnxtDjNrsYzNL53308627 = -681534585;    float lGIPMnJZnxtDjNrsYzNL82896017 = -946468440;    float lGIPMnJZnxtDjNrsYzNL62358105 = -393111871;    float lGIPMnJZnxtDjNrsYzNL74061037 = -547482511;    float lGIPMnJZnxtDjNrsYzNL36071957 = -600641529;    float lGIPMnJZnxtDjNrsYzNL26162285 = -267498928;    float lGIPMnJZnxtDjNrsYzNL36226875 = 14118966;    float lGIPMnJZnxtDjNrsYzNL3458029 = -289838742;    float lGIPMnJZnxtDjNrsYzNL15746212 = -375203579;    float lGIPMnJZnxtDjNrsYzNL48600277 = -268548813;    float lGIPMnJZnxtDjNrsYzNL50574763 = -614640059;    float lGIPMnJZnxtDjNrsYzNL75631369 = -806114740;    float lGIPMnJZnxtDjNrsYzNL5294476 = -580494261;    float lGIPMnJZnxtDjNrsYzNL22491054 = -590093114;    float lGIPMnJZnxtDjNrsYzNL88771808 = -212553600;    float lGIPMnJZnxtDjNrsYzNL18673268 = -998829517;    float lGIPMnJZnxtDjNrsYzNL84068347 = -243857240;    float lGIPMnJZnxtDjNrsYzNL97135237 = -450787019;    float lGIPMnJZnxtDjNrsYzNL43698343 = -577269034;    float lGIPMnJZnxtDjNrsYzNL12296377 = -741617773;    float lGIPMnJZnxtDjNrsYzNL40772698 = -668099522;    float lGIPMnJZnxtDjNrsYzNL58089211 = -663205587;    float lGIPMnJZnxtDjNrsYzNL80893788 = -150579281;    float lGIPMnJZnxtDjNrsYzNL29202261 = -554344226;    float lGIPMnJZnxtDjNrsYzNL13302964 = -214228299;    float lGIPMnJZnxtDjNrsYzNL99772029 = -812937037;    float lGIPMnJZnxtDjNrsYzNL7560212 = -594469865;    float lGIPMnJZnxtDjNrsYzNL18863131 = -814584479;    float lGIPMnJZnxtDjNrsYzNL70548232 = -436242572;    float lGIPMnJZnxtDjNrsYzNL11997728 = -334113934;    float lGIPMnJZnxtDjNrsYzNL28074625 = -962506206;    float lGIPMnJZnxtDjNrsYzNL36567790 = -273640708;    float lGIPMnJZnxtDjNrsYzNL14773618 = -907770963;    float lGIPMnJZnxtDjNrsYzNL75102843 = -176731056;    float lGIPMnJZnxtDjNrsYzNL88598154 = 39966504;    float lGIPMnJZnxtDjNrsYzNL59670124 = -341678751;    float lGIPMnJZnxtDjNrsYzNL67411694 = -303435816;    float lGIPMnJZnxtDjNrsYzNL3980023 = -917659075;    float lGIPMnJZnxtDjNrsYzNL58289604 = -486563698;    float lGIPMnJZnxtDjNrsYzNL55524870 = 86001287;    float lGIPMnJZnxtDjNrsYzNL84562511 = -946719022;    float lGIPMnJZnxtDjNrsYzNL85136983 = -787985032;    float lGIPMnJZnxtDjNrsYzNL97902516 = -192155566;    float lGIPMnJZnxtDjNrsYzNL96517491 = -48763956;    float lGIPMnJZnxtDjNrsYzNL34183301 = -358193827;    float lGIPMnJZnxtDjNrsYzNL89809153 = -559538426;    float lGIPMnJZnxtDjNrsYzNL35058138 = -20289982;     lGIPMnJZnxtDjNrsYzNL76318430 = lGIPMnJZnxtDjNrsYzNL52804524;     lGIPMnJZnxtDjNrsYzNL52804524 = lGIPMnJZnxtDjNrsYzNL76196599;     lGIPMnJZnxtDjNrsYzNL76196599 = lGIPMnJZnxtDjNrsYzNL45278097;     lGIPMnJZnxtDjNrsYzNL45278097 = lGIPMnJZnxtDjNrsYzNL14101454;     lGIPMnJZnxtDjNrsYzNL14101454 = lGIPMnJZnxtDjNrsYzNL80246417;     lGIPMnJZnxtDjNrsYzNL80246417 = lGIPMnJZnxtDjNrsYzNL58491618;     lGIPMnJZnxtDjNrsYzNL58491618 = lGIPMnJZnxtDjNrsYzNL4848406;     lGIPMnJZnxtDjNrsYzNL4848406 = lGIPMnJZnxtDjNrsYzNL83574482;     lGIPMnJZnxtDjNrsYzNL83574482 = lGIPMnJZnxtDjNrsYzNL87234903;     lGIPMnJZnxtDjNrsYzNL87234903 = lGIPMnJZnxtDjNrsYzNL43301294;     lGIPMnJZnxtDjNrsYzNL43301294 = lGIPMnJZnxtDjNrsYzNL64397835;     lGIPMnJZnxtDjNrsYzNL64397835 = lGIPMnJZnxtDjNrsYzNL31337950;     lGIPMnJZnxtDjNrsYzNL31337950 = lGIPMnJZnxtDjNrsYzNL48443068;     lGIPMnJZnxtDjNrsYzNL48443068 = lGIPMnJZnxtDjNrsYzNL62109406;     lGIPMnJZnxtDjNrsYzNL62109406 = lGIPMnJZnxtDjNrsYzNL91496799;     lGIPMnJZnxtDjNrsYzNL91496799 = lGIPMnJZnxtDjNrsYzNL25385498;     lGIPMnJZnxtDjNrsYzNL25385498 = lGIPMnJZnxtDjNrsYzNL33085763;     lGIPMnJZnxtDjNrsYzNL33085763 = lGIPMnJZnxtDjNrsYzNL29367710;     lGIPMnJZnxtDjNrsYzNL29367710 = lGIPMnJZnxtDjNrsYzNL71737938;     lGIPMnJZnxtDjNrsYzNL71737938 = lGIPMnJZnxtDjNrsYzNL26011556;     lGIPMnJZnxtDjNrsYzNL26011556 = lGIPMnJZnxtDjNrsYzNL13960325;     lGIPMnJZnxtDjNrsYzNL13960325 = lGIPMnJZnxtDjNrsYzNL78743486;     lGIPMnJZnxtDjNrsYzNL78743486 = lGIPMnJZnxtDjNrsYzNL40124642;     lGIPMnJZnxtDjNrsYzNL40124642 = lGIPMnJZnxtDjNrsYzNL19115813;     lGIPMnJZnxtDjNrsYzNL19115813 = lGIPMnJZnxtDjNrsYzNL77874578;     lGIPMnJZnxtDjNrsYzNL77874578 = lGIPMnJZnxtDjNrsYzNL76788389;     lGIPMnJZnxtDjNrsYzNL76788389 = lGIPMnJZnxtDjNrsYzNL42745406;     lGIPMnJZnxtDjNrsYzNL42745406 = lGIPMnJZnxtDjNrsYzNL56248129;     lGIPMnJZnxtDjNrsYzNL56248129 = lGIPMnJZnxtDjNrsYzNL32999720;     lGIPMnJZnxtDjNrsYzNL32999720 = lGIPMnJZnxtDjNrsYzNL11603535;     lGIPMnJZnxtDjNrsYzNL11603535 = lGIPMnJZnxtDjNrsYzNL38006819;     lGIPMnJZnxtDjNrsYzNL38006819 = lGIPMnJZnxtDjNrsYzNL41906782;     lGIPMnJZnxtDjNrsYzNL41906782 = lGIPMnJZnxtDjNrsYzNL42566141;     lGIPMnJZnxtDjNrsYzNL42566141 = lGIPMnJZnxtDjNrsYzNL29769800;     lGIPMnJZnxtDjNrsYzNL29769800 = lGIPMnJZnxtDjNrsYzNL78041059;     lGIPMnJZnxtDjNrsYzNL78041059 = lGIPMnJZnxtDjNrsYzNL94361561;     lGIPMnJZnxtDjNrsYzNL94361561 = lGIPMnJZnxtDjNrsYzNL81687154;     lGIPMnJZnxtDjNrsYzNL81687154 = lGIPMnJZnxtDjNrsYzNL20789386;     lGIPMnJZnxtDjNrsYzNL20789386 = lGIPMnJZnxtDjNrsYzNL88595011;     lGIPMnJZnxtDjNrsYzNL88595011 = lGIPMnJZnxtDjNrsYzNL13648728;     lGIPMnJZnxtDjNrsYzNL13648728 = lGIPMnJZnxtDjNrsYzNL45117768;     lGIPMnJZnxtDjNrsYzNL45117768 = lGIPMnJZnxtDjNrsYzNL84758064;     lGIPMnJZnxtDjNrsYzNL84758064 = lGIPMnJZnxtDjNrsYzNL65440522;     lGIPMnJZnxtDjNrsYzNL65440522 = lGIPMnJZnxtDjNrsYzNL40352613;     lGIPMnJZnxtDjNrsYzNL40352613 = lGIPMnJZnxtDjNrsYzNL11555601;     lGIPMnJZnxtDjNrsYzNL11555601 = lGIPMnJZnxtDjNrsYzNL59011448;     lGIPMnJZnxtDjNrsYzNL59011448 = lGIPMnJZnxtDjNrsYzNL6240158;     lGIPMnJZnxtDjNrsYzNL6240158 = lGIPMnJZnxtDjNrsYzNL30747678;     lGIPMnJZnxtDjNrsYzNL30747678 = lGIPMnJZnxtDjNrsYzNL28173504;     lGIPMnJZnxtDjNrsYzNL28173504 = lGIPMnJZnxtDjNrsYzNL96431930;     lGIPMnJZnxtDjNrsYzNL96431930 = lGIPMnJZnxtDjNrsYzNL96829916;     lGIPMnJZnxtDjNrsYzNL96829916 = lGIPMnJZnxtDjNrsYzNL62903975;     lGIPMnJZnxtDjNrsYzNL62903975 = lGIPMnJZnxtDjNrsYzNL53308627;     lGIPMnJZnxtDjNrsYzNL53308627 = lGIPMnJZnxtDjNrsYzNL82896017;     lGIPMnJZnxtDjNrsYzNL82896017 = lGIPMnJZnxtDjNrsYzNL62358105;     lGIPMnJZnxtDjNrsYzNL62358105 = lGIPMnJZnxtDjNrsYzNL74061037;     lGIPMnJZnxtDjNrsYzNL74061037 = lGIPMnJZnxtDjNrsYzNL36071957;     lGIPMnJZnxtDjNrsYzNL36071957 = lGIPMnJZnxtDjNrsYzNL26162285;     lGIPMnJZnxtDjNrsYzNL26162285 = lGIPMnJZnxtDjNrsYzNL36226875;     lGIPMnJZnxtDjNrsYzNL36226875 = lGIPMnJZnxtDjNrsYzNL3458029;     lGIPMnJZnxtDjNrsYzNL3458029 = lGIPMnJZnxtDjNrsYzNL15746212;     lGIPMnJZnxtDjNrsYzNL15746212 = lGIPMnJZnxtDjNrsYzNL48600277;     lGIPMnJZnxtDjNrsYzNL48600277 = lGIPMnJZnxtDjNrsYzNL50574763;     lGIPMnJZnxtDjNrsYzNL50574763 = lGIPMnJZnxtDjNrsYzNL75631369;     lGIPMnJZnxtDjNrsYzNL75631369 = lGIPMnJZnxtDjNrsYzNL5294476;     lGIPMnJZnxtDjNrsYzNL5294476 = lGIPMnJZnxtDjNrsYzNL22491054;     lGIPMnJZnxtDjNrsYzNL22491054 = lGIPMnJZnxtDjNrsYzNL88771808;     lGIPMnJZnxtDjNrsYzNL88771808 = lGIPMnJZnxtDjNrsYzNL18673268;     lGIPMnJZnxtDjNrsYzNL18673268 = lGIPMnJZnxtDjNrsYzNL84068347;     lGIPMnJZnxtDjNrsYzNL84068347 = lGIPMnJZnxtDjNrsYzNL97135237;     lGIPMnJZnxtDjNrsYzNL97135237 = lGIPMnJZnxtDjNrsYzNL43698343;     lGIPMnJZnxtDjNrsYzNL43698343 = lGIPMnJZnxtDjNrsYzNL12296377;     lGIPMnJZnxtDjNrsYzNL12296377 = lGIPMnJZnxtDjNrsYzNL40772698;     lGIPMnJZnxtDjNrsYzNL40772698 = lGIPMnJZnxtDjNrsYzNL58089211;     lGIPMnJZnxtDjNrsYzNL58089211 = lGIPMnJZnxtDjNrsYzNL80893788;     lGIPMnJZnxtDjNrsYzNL80893788 = lGIPMnJZnxtDjNrsYzNL29202261;     lGIPMnJZnxtDjNrsYzNL29202261 = lGIPMnJZnxtDjNrsYzNL13302964;     lGIPMnJZnxtDjNrsYzNL13302964 = lGIPMnJZnxtDjNrsYzNL99772029;     lGIPMnJZnxtDjNrsYzNL99772029 = lGIPMnJZnxtDjNrsYzNL7560212;     lGIPMnJZnxtDjNrsYzNL7560212 = lGIPMnJZnxtDjNrsYzNL18863131;     lGIPMnJZnxtDjNrsYzNL18863131 = lGIPMnJZnxtDjNrsYzNL70548232;     lGIPMnJZnxtDjNrsYzNL70548232 = lGIPMnJZnxtDjNrsYzNL11997728;     lGIPMnJZnxtDjNrsYzNL11997728 = lGIPMnJZnxtDjNrsYzNL28074625;     lGIPMnJZnxtDjNrsYzNL28074625 = lGIPMnJZnxtDjNrsYzNL36567790;     lGIPMnJZnxtDjNrsYzNL36567790 = lGIPMnJZnxtDjNrsYzNL14773618;     lGIPMnJZnxtDjNrsYzNL14773618 = lGIPMnJZnxtDjNrsYzNL75102843;     lGIPMnJZnxtDjNrsYzNL75102843 = lGIPMnJZnxtDjNrsYzNL88598154;     lGIPMnJZnxtDjNrsYzNL88598154 = lGIPMnJZnxtDjNrsYzNL59670124;     lGIPMnJZnxtDjNrsYzNL59670124 = lGIPMnJZnxtDjNrsYzNL67411694;     lGIPMnJZnxtDjNrsYzNL67411694 = lGIPMnJZnxtDjNrsYzNL3980023;     lGIPMnJZnxtDjNrsYzNL3980023 = lGIPMnJZnxtDjNrsYzNL58289604;     lGIPMnJZnxtDjNrsYzNL58289604 = lGIPMnJZnxtDjNrsYzNL55524870;     lGIPMnJZnxtDjNrsYzNL55524870 = lGIPMnJZnxtDjNrsYzNL84562511;     lGIPMnJZnxtDjNrsYzNL84562511 = lGIPMnJZnxtDjNrsYzNL85136983;     lGIPMnJZnxtDjNrsYzNL85136983 = lGIPMnJZnxtDjNrsYzNL97902516;     lGIPMnJZnxtDjNrsYzNL97902516 = lGIPMnJZnxtDjNrsYzNL96517491;     lGIPMnJZnxtDjNrsYzNL96517491 = lGIPMnJZnxtDjNrsYzNL34183301;     lGIPMnJZnxtDjNrsYzNL34183301 = lGIPMnJZnxtDjNrsYzNL89809153;     lGIPMnJZnxtDjNrsYzNL89809153 = lGIPMnJZnxtDjNrsYzNL35058138;     lGIPMnJZnxtDjNrsYzNL35058138 = lGIPMnJZnxtDjNrsYzNL76318430;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MCRXozfeWwmZmRfANFgV63323786() {     float JKRCRDVCcUaXcftWnDpV46970097 = -638439112;    float JKRCRDVCcUaXcftWnDpV21478042 = -99007273;    float JKRCRDVCcUaXcftWnDpV56165518 = -479058679;    float JKRCRDVCcUaXcftWnDpV85209384 = -676665746;    float JKRCRDVCcUaXcftWnDpV35551775 = -441985419;    float JKRCRDVCcUaXcftWnDpV70246180 = -121511692;    float JKRCRDVCcUaXcftWnDpV76870513 = -68906772;    float JKRCRDVCcUaXcftWnDpV61690786 = -3373044;    float JKRCRDVCcUaXcftWnDpV29439537 = -45933746;    float JKRCRDVCcUaXcftWnDpV10201365 = -179070826;    float JKRCRDVCcUaXcftWnDpV85550455 = -670187637;    float JKRCRDVCcUaXcftWnDpV9944407 = -647424986;    float JKRCRDVCcUaXcftWnDpV11165337 = -510237849;    float JKRCRDVCcUaXcftWnDpV32290102 = -596405413;    float JKRCRDVCcUaXcftWnDpV44829426 = -958748827;    float JKRCRDVCcUaXcftWnDpV24025440 = -207975832;    float JKRCRDVCcUaXcftWnDpV19238325 = -867951804;    float JKRCRDVCcUaXcftWnDpV53379257 = -763384023;    float JKRCRDVCcUaXcftWnDpV11075492 = -453111237;    float JKRCRDVCcUaXcftWnDpV30524648 = -872931036;    float JKRCRDVCcUaXcftWnDpV81147310 = -576571189;    float JKRCRDVCcUaXcftWnDpV75356089 = -958463417;    float JKRCRDVCcUaXcftWnDpV80687295 = -147724608;    float JKRCRDVCcUaXcftWnDpV73974179 = -28523597;    float JKRCRDVCcUaXcftWnDpV83240201 = -894933117;    float JKRCRDVCcUaXcftWnDpV8472160 = -670982483;    float JKRCRDVCcUaXcftWnDpV85049175 = -425269574;    float JKRCRDVCcUaXcftWnDpV50527631 = -100051251;    float JKRCRDVCcUaXcftWnDpV88390616 = -682365358;    float JKRCRDVCcUaXcftWnDpV72171815 = -896341429;    float JKRCRDVCcUaXcftWnDpV18675531 = -920294421;    float JKRCRDVCcUaXcftWnDpV82518279 = -283088610;    float JKRCRDVCcUaXcftWnDpV24852143 = -688529689;    float JKRCRDVCcUaXcftWnDpV7931116 = -951676875;    float JKRCRDVCcUaXcftWnDpV7794037 = -643494466;    float JKRCRDVCcUaXcftWnDpV38034319 = -544368132;    float JKRCRDVCcUaXcftWnDpV38153048 = -275798435;    float JKRCRDVCcUaXcftWnDpV34479643 = -65610748;    float JKRCRDVCcUaXcftWnDpV99322674 = -988328691;    float JKRCRDVCcUaXcftWnDpV83698310 = -695432136;    float JKRCRDVCcUaXcftWnDpV73722012 = 48621977;    float JKRCRDVCcUaXcftWnDpV73600759 = -186701837;    float JKRCRDVCcUaXcftWnDpV33725024 = 40412062;    float JKRCRDVCcUaXcftWnDpV71948823 = -437628137;    float JKRCRDVCcUaXcftWnDpV36306672 = -931832049;    float JKRCRDVCcUaXcftWnDpV17961436 = -282895719;    float JKRCRDVCcUaXcftWnDpV85142650 = -874183359;    float JKRCRDVCcUaXcftWnDpV71854367 = 54020477;    float JKRCRDVCcUaXcftWnDpV36189131 = -246577519;    float JKRCRDVCcUaXcftWnDpV36600990 = -708509814;    float JKRCRDVCcUaXcftWnDpV64621254 = -610877772;    float JKRCRDVCcUaXcftWnDpV38701421 = -133309495;    float JKRCRDVCcUaXcftWnDpV17979632 = -705858434;    float JKRCRDVCcUaXcftWnDpV43535932 = -242416082;    float JKRCRDVCcUaXcftWnDpV82176476 = 42590645;    float JKRCRDVCcUaXcftWnDpV71614007 = -679975696;    float JKRCRDVCcUaXcftWnDpV40790746 = -951282665;    float JKRCRDVCcUaXcftWnDpV82191339 = -350535083;    float JKRCRDVCcUaXcftWnDpV1969183 = -781732629;    float JKRCRDVCcUaXcftWnDpV27079616 = -771002936;    float JKRCRDVCcUaXcftWnDpV85197005 = -696242118;    float JKRCRDVCcUaXcftWnDpV26342882 = -968855521;    float JKRCRDVCcUaXcftWnDpV73300170 = -321007686;    float JKRCRDVCcUaXcftWnDpV57267721 = -149592317;    float JKRCRDVCcUaXcftWnDpV91525833 = -258776406;    float JKRCRDVCcUaXcftWnDpV3032176 = -287099027;    float JKRCRDVCcUaXcftWnDpV85092263 = -958895298;    float JKRCRDVCcUaXcftWnDpV3234221 = -558560974;    float JKRCRDVCcUaXcftWnDpV24496066 = -952910947;    float JKRCRDVCcUaXcftWnDpV6795108 = -314380695;    float JKRCRDVCcUaXcftWnDpV85872391 = -932177398;    float JKRCRDVCcUaXcftWnDpV84758681 = -702341056;    float JKRCRDVCcUaXcftWnDpV54056582 = -775055332;    float JKRCRDVCcUaXcftWnDpV27377182 = -757679101;    float JKRCRDVCcUaXcftWnDpV56802636 = -821553014;    float JKRCRDVCcUaXcftWnDpV7546552 = -289869352;    float JKRCRDVCcUaXcftWnDpV41631066 = -898875480;    float JKRCRDVCcUaXcftWnDpV8738473 = -710096472;    float JKRCRDVCcUaXcftWnDpV37667508 = -96691549;    float JKRCRDVCcUaXcftWnDpV65278766 = -512037398;    float JKRCRDVCcUaXcftWnDpV23329509 = -796799124;    float JKRCRDVCcUaXcftWnDpV13194808 = -379290052;    float JKRCRDVCcUaXcftWnDpV14338501 = -853473733;    float JKRCRDVCcUaXcftWnDpV51789626 = -973855544;    float JKRCRDVCcUaXcftWnDpV7550562 = -185463658;    float JKRCRDVCcUaXcftWnDpV79974109 = -686984927;    float JKRCRDVCcUaXcftWnDpV64538647 = -577230177;    float JKRCRDVCcUaXcftWnDpV81316210 = -346113608;    float JKRCRDVCcUaXcftWnDpV25754640 = -894267521;    float JKRCRDVCcUaXcftWnDpV36180029 = -963518771;    float JKRCRDVCcUaXcftWnDpV97243572 = -593085468;    float JKRCRDVCcUaXcftWnDpV55961708 = -925263352;    float JKRCRDVCcUaXcftWnDpV32510461 = -283878119;    float JKRCRDVCcUaXcftWnDpV72243059 = -117325756;    float JKRCRDVCcUaXcftWnDpV98501304 = -999190018;    float JKRCRDVCcUaXcftWnDpV47379130 = 17477498;    float JKRCRDVCcUaXcftWnDpV300590 = -865694152;    float JKRCRDVCcUaXcftWnDpV76457302 = -809995621;    float JKRCRDVCcUaXcftWnDpV80422989 = -78851732;    float JKRCRDVCcUaXcftWnDpV33274496 = -638439112;     JKRCRDVCcUaXcftWnDpV46970097 = JKRCRDVCcUaXcftWnDpV21478042;     JKRCRDVCcUaXcftWnDpV21478042 = JKRCRDVCcUaXcftWnDpV56165518;     JKRCRDVCcUaXcftWnDpV56165518 = JKRCRDVCcUaXcftWnDpV85209384;     JKRCRDVCcUaXcftWnDpV85209384 = JKRCRDVCcUaXcftWnDpV35551775;     JKRCRDVCcUaXcftWnDpV35551775 = JKRCRDVCcUaXcftWnDpV70246180;     JKRCRDVCcUaXcftWnDpV70246180 = JKRCRDVCcUaXcftWnDpV76870513;     JKRCRDVCcUaXcftWnDpV76870513 = JKRCRDVCcUaXcftWnDpV61690786;     JKRCRDVCcUaXcftWnDpV61690786 = JKRCRDVCcUaXcftWnDpV29439537;     JKRCRDVCcUaXcftWnDpV29439537 = JKRCRDVCcUaXcftWnDpV10201365;     JKRCRDVCcUaXcftWnDpV10201365 = JKRCRDVCcUaXcftWnDpV85550455;     JKRCRDVCcUaXcftWnDpV85550455 = JKRCRDVCcUaXcftWnDpV9944407;     JKRCRDVCcUaXcftWnDpV9944407 = JKRCRDVCcUaXcftWnDpV11165337;     JKRCRDVCcUaXcftWnDpV11165337 = JKRCRDVCcUaXcftWnDpV32290102;     JKRCRDVCcUaXcftWnDpV32290102 = JKRCRDVCcUaXcftWnDpV44829426;     JKRCRDVCcUaXcftWnDpV44829426 = JKRCRDVCcUaXcftWnDpV24025440;     JKRCRDVCcUaXcftWnDpV24025440 = JKRCRDVCcUaXcftWnDpV19238325;     JKRCRDVCcUaXcftWnDpV19238325 = JKRCRDVCcUaXcftWnDpV53379257;     JKRCRDVCcUaXcftWnDpV53379257 = JKRCRDVCcUaXcftWnDpV11075492;     JKRCRDVCcUaXcftWnDpV11075492 = JKRCRDVCcUaXcftWnDpV30524648;     JKRCRDVCcUaXcftWnDpV30524648 = JKRCRDVCcUaXcftWnDpV81147310;     JKRCRDVCcUaXcftWnDpV81147310 = JKRCRDVCcUaXcftWnDpV75356089;     JKRCRDVCcUaXcftWnDpV75356089 = JKRCRDVCcUaXcftWnDpV80687295;     JKRCRDVCcUaXcftWnDpV80687295 = JKRCRDVCcUaXcftWnDpV73974179;     JKRCRDVCcUaXcftWnDpV73974179 = JKRCRDVCcUaXcftWnDpV83240201;     JKRCRDVCcUaXcftWnDpV83240201 = JKRCRDVCcUaXcftWnDpV8472160;     JKRCRDVCcUaXcftWnDpV8472160 = JKRCRDVCcUaXcftWnDpV85049175;     JKRCRDVCcUaXcftWnDpV85049175 = JKRCRDVCcUaXcftWnDpV50527631;     JKRCRDVCcUaXcftWnDpV50527631 = JKRCRDVCcUaXcftWnDpV88390616;     JKRCRDVCcUaXcftWnDpV88390616 = JKRCRDVCcUaXcftWnDpV72171815;     JKRCRDVCcUaXcftWnDpV72171815 = JKRCRDVCcUaXcftWnDpV18675531;     JKRCRDVCcUaXcftWnDpV18675531 = JKRCRDVCcUaXcftWnDpV82518279;     JKRCRDVCcUaXcftWnDpV82518279 = JKRCRDVCcUaXcftWnDpV24852143;     JKRCRDVCcUaXcftWnDpV24852143 = JKRCRDVCcUaXcftWnDpV7931116;     JKRCRDVCcUaXcftWnDpV7931116 = JKRCRDVCcUaXcftWnDpV7794037;     JKRCRDVCcUaXcftWnDpV7794037 = JKRCRDVCcUaXcftWnDpV38034319;     JKRCRDVCcUaXcftWnDpV38034319 = JKRCRDVCcUaXcftWnDpV38153048;     JKRCRDVCcUaXcftWnDpV38153048 = JKRCRDVCcUaXcftWnDpV34479643;     JKRCRDVCcUaXcftWnDpV34479643 = JKRCRDVCcUaXcftWnDpV99322674;     JKRCRDVCcUaXcftWnDpV99322674 = JKRCRDVCcUaXcftWnDpV83698310;     JKRCRDVCcUaXcftWnDpV83698310 = JKRCRDVCcUaXcftWnDpV73722012;     JKRCRDVCcUaXcftWnDpV73722012 = JKRCRDVCcUaXcftWnDpV73600759;     JKRCRDVCcUaXcftWnDpV73600759 = JKRCRDVCcUaXcftWnDpV33725024;     JKRCRDVCcUaXcftWnDpV33725024 = JKRCRDVCcUaXcftWnDpV71948823;     JKRCRDVCcUaXcftWnDpV71948823 = JKRCRDVCcUaXcftWnDpV36306672;     JKRCRDVCcUaXcftWnDpV36306672 = JKRCRDVCcUaXcftWnDpV17961436;     JKRCRDVCcUaXcftWnDpV17961436 = JKRCRDVCcUaXcftWnDpV85142650;     JKRCRDVCcUaXcftWnDpV85142650 = JKRCRDVCcUaXcftWnDpV71854367;     JKRCRDVCcUaXcftWnDpV71854367 = JKRCRDVCcUaXcftWnDpV36189131;     JKRCRDVCcUaXcftWnDpV36189131 = JKRCRDVCcUaXcftWnDpV36600990;     JKRCRDVCcUaXcftWnDpV36600990 = JKRCRDVCcUaXcftWnDpV64621254;     JKRCRDVCcUaXcftWnDpV64621254 = JKRCRDVCcUaXcftWnDpV38701421;     JKRCRDVCcUaXcftWnDpV38701421 = JKRCRDVCcUaXcftWnDpV17979632;     JKRCRDVCcUaXcftWnDpV17979632 = JKRCRDVCcUaXcftWnDpV43535932;     JKRCRDVCcUaXcftWnDpV43535932 = JKRCRDVCcUaXcftWnDpV82176476;     JKRCRDVCcUaXcftWnDpV82176476 = JKRCRDVCcUaXcftWnDpV71614007;     JKRCRDVCcUaXcftWnDpV71614007 = JKRCRDVCcUaXcftWnDpV40790746;     JKRCRDVCcUaXcftWnDpV40790746 = JKRCRDVCcUaXcftWnDpV82191339;     JKRCRDVCcUaXcftWnDpV82191339 = JKRCRDVCcUaXcftWnDpV1969183;     JKRCRDVCcUaXcftWnDpV1969183 = JKRCRDVCcUaXcftWnDpV27079616;     JKRCRDVCcUaXcftWnDpV27079616 = JKRCRDVCcUaXcftWnDpV85197005;     JKRCRDVCcUaXcftWnDpV85197005 = JKRCRDVCcUaXcftWnDpV26342882;     JKRCRDVCcUaXcftWnDpV26342882 = JKRCRDVCcUaXcftWnDpV73300170;     JKRCRDVCcUaXcftWnDpV73300170 = JKRCRDVCcUaXcftWnDpV57267721;     JKRCRDVCcUaXcftWnDpV57267721 = JKRCRDVCcUaXcftWnDpV91525833;     JKRCRDVCcUaXcftWnDpV91525833 = JKRCRDVCcUaXcftWnDpV3032176;     JKRCRDVCcUaXcftWnDpV3032176 = JKRCRDVCcUaXcftWnDpV85092263;     JKRCRDVCcUaXcftWnDpV85092263 = JKRCRDVCcUaXcftWnDpV3234221;     JKRCRDVCcUaXcftWnDpV3234221 = JKRCRDVCcUaXcftWnDpV24496066;     JKRCRDVCcUaXcftWnDpV24496066 = JKRCRDVCcUaXcftWnDpV6795108;     JKRCRDVCcUaXcftWnDpV6795108 = JKRCRDVCcUaXcftWnDpV85872391;     JKRCRDVCcUaXcftWnDpV85872391 = JKRCRDVCcUaXcftWnDpV84758681;     JKRCRDVCcUaXcftWnDpV84758681 = JKRCRDVCcUaXcftWnDpV54056582;     JKRCRDVCcUaXcftWnDpV54056582 = JKRCRDVCcUaXcftWnDpV27377182;     JKRCRDVCcUaXcftWnDpV27377182 = JKRCRDVCcUaXcftWnDpV56802636;     JKRCRDVCcUaXcftWnDpV56802636 = JKRCRDVCcUaXcftWnDpV7546552;     JKRCRDVCcUaXcftWnDpV7546552 = JKRCRDVCcUaXcftWnDpV41631066;     JKRCRDVCcUaXcftWnDpV41631066 = JKRCRDVCcUaXcftWnDpV8738473;     JKRCRDVCcUaXcftWnDpV8738473 = JKRCRDVCcUaXcftWnDpV37667508;     JKRCRDVCcUaXcftWnDpV37667508 = JKRCRDVCcUaXcftWnDpV65278766;     JKRCRDVCcUaXcftWnDpV65278766 = JKRCRDVCcUaXcftWnDpV23329509;     JKRCRDVCcUaXcftWnDpV23329509 = JKRCRDVCcUaXcftWnDpV13194808;     JKRCRDVCcUaXcftWnDpV13194808 = JKRCRDVCcUaXcftWnDpV14338501;     JKRCRDVCcUaXcftWnDpV14338501 = JKRCRDVCcUaXcftWnDpV51789626;     JKRCRDVCcUaXcftWnDpV51789626 = JKRCRDVCcUaXcftWnDpV7550562;     JKRCRDVCcUaXcftWnDpV7550562 = JKRCRDVCcUaXcftWnDpV79974109;     JKRCRDVCcUaXcftWnDpV79974109 = JKRCRDVCcUaXcftWnDpV64538647;     JKRCRDVCcUaXcftWnDpV64538647 = JKRCRDVCcUaXcftWnDpV81316210;     JKRCRDVCcUaXcftWnDpV81316210 = JKRCRDVCcUaXcftWnDpV25754640;     JKRCRDVCcUaXcftWnDpV25754640 = JKRCRDVCcUaXcftWnDpV36180029;     JKRCRDVCcUaXcftWnDpV36180029 = JKRCRDVCcUaXcftWnDpV97243572;     JKRCRDVCcUaXcftWnDpV97243572 = JKRCRDVCcUaXcftWnDpV55961708;     JKRCRDVCcUaXcftWnDpV55961708 = JKRCRDVCcUaXcftWnDpV32510461;     JKRCRDVCcUaXcftWnDpV32510461 = JKRCRDVCcUaXcftWnDpV72243059;     JKRCRDVCcUaXcftWnDpV72243059 = JKRCRDVCcUaXcftWnDpV98501304;     JKRCRDVCcUaXcftWnDpV98501304 = JKRCRDVCcUaXcftWnDpV47379130;     JKRCRDVCcUaXcftWnDpV47379130 = JKRCRDVCcUaXcftWnDpV300590;     JKRCRDVCcUaXcftWnDpV300590 = JKRCRDVCcUaXcftWnDpV76457302;     JKRCRDVCcUaXcftWnDpV76457302 = JKRCRDVCcUaXcftWnDpV80422989;     JKRCRDVCcUaXcftWnDpV80422989 = JKRCRDVCcUaXcftWnDpV33274496;     JKRCRDVCcUaXcftWnDpV33274496 = JKRCRDVCcUaXcftWnDpV46970097;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kqrIweBzOfvMQEBVqnLf69982531() {     float dQCuYhldkBDZNgIHXDyG46963626 = -510963253;    float dQCuYhldkBDZNgIHXDyG60050358 = -141602398;    float dQCuYhldkBDZNgIHXDyG57478731 = -436187293;    float dQCuYhldkBDZNgIHXDyG70289244 = -497206576;    float dQCuYhldkBDZNgIHXDyG44391877 = -910340740;    float dQCuYhldkBDZNgIHXDyG5051613 = -920944504;    float dQCuYhldkBDZNgIHXDyG45417849 = -873443239;    float dQCuYhldkBDZNgIHXDyG30904837 = -807550361;    float dQCuYhldkBDZNgIHXDyG12179702 = -859579815;    float dQCuYhldkBDZNgIHXDyG6840879 = -915877697;    float dQCuYhldkBDZNgIHXDyG5871061 = -629659709;    float dQCuYhldkBDZNgIHXDyG24236084 = -382903349;    float dQCuYhldkBDZNgIHXDyG43018251 = -531896324;    float dQCuYhldkBDZNgIHXDyG86134321 = -941558465;    float dQCuYhldkBDZNgIHXDyG90001534 = -921461679;    float dQCuYhldkBDZNgIHXDyG98589986 = -540771483;    float dQCuYhldkBDZNgIHXDyG75091084 = -687566374;    float dQCuYhldkBDZNgIHXDyG76379790 = -60455645;    float dQCuYhldkBDZNgIHXDyG31503762 = 28906496;    float dQCuYhldkBDZNgIHXDyG2598411 = -384955488;    float dQCuYhldkBDZNgIHXDyG60816734 = -287037563;    float dQCuYhldkBDZNgIHXDyG53652355 = -368455300;    float dQCuYhldkBDZNgIHXDyG17565692 = -606682450;    float dQCuYhldkBDZNgIHXDyG17624288 = 90555880;    float dQCuYhldkBDZNgIHXDyG11682891 = -221626970;    float dQCuYhldkBDZNgIHXDyG25046237 = -579188411;    float dQCuYhldkBDZNgIHXDyG55270757 = -726069287;    float dQCuYhldkBDZNgIHXDyG85931156 = -709018239;    float dQCuYhldkBDZNgIHXDyG1069064 = -228435673;    float dQCuYhldkBDZNgIHXDyG48261686 = -571772803;    float dQCuYhldkBDZNgIHXDyG52088210 = -207387656;    float dQCuYhldkBDZNgIHXDyG67242137 = -674910372;    float dQCuYhldkBDZNgIHXDyG10304566 = -890473905;    float dQCuYhldkBDZNgIHXDyG8213954 = -828358608;    float dQCuYhldkBDZNgIHXDyG90152074 = -902808391;    float dQCuYhldkBDZNgIHXDyG80862508 = -624133879;    float dQCuYhldkBDZNgIHXDyG50094416 = -375451813;    float dQCuYhldkBDZNgIHXDyG4397323 = -60873427;    float dQCuYhldkBDZNgIHXDyG34636272 = -601010108;    float dQCuYhldkBDZNgIHXDyG55571203 = -304863822;    float dQCuYhldkBDZNgIHXDyG61577679 = -39321922;    float dQCuYhldkBDZNgIHXDyG59298420 = -380493920;    float dQCuYhldkBDZNgIHXDyG94008384 = -518718192;    float dQCuYhldkBDZNgIHXDyG31180721 = 5407419;    float dQCuYhldkBDZNgIHXDyG20324724 = -305091451;    float dQCuYhldkBDZNgIHXDyG58389404 = -94940390;    float dQCuYhldkBDZNgIHXDyG12840444 = -255463981;    float dQCuYhldkBDZNgIHXDyG47237948 = -554740879;    float dQCuYhldkBDZNgIHXDyG54847520 = -910341380;    float dQCuYhldkBDZNgIHXDyG56646413 = -295534701;    float dQCuYhldkBDZNgIHXDyG77880624 = -189432779;    float dQCuYhldkBDZNgIHXDyG53248679 = -515455243;    float dQCuYhldkBDZNgIHXDyG93427980 = -597912442;    float dQCuYhldkBDZNgIHXDyG53786203 = -774335933;    float dQCuYhldkBDZNgIHXDyG9165797 = -814308632;    float dQCuYhldkBDZNgIHXDyG93311270 = -42507954;    float dQCuYhldkBDZNgIHXDyG42484666 = -534919948;    float dQCuYhldkBDZNgIHXDyG39854443 = -426743174;    float dQCuYhldkBDZNgIHXDyG58606354 = -175579606;    float dQCuYhldkBDZNgIHXDyG19345641 = -231152329;    float dQCuYhldkBDZNgIHXDyG49780856 = -94875217;    float dQCuYhldkBDZNgIHXDyG59486693 = -64425000;    float dQCuYhldkBDZNgIHXDyG29835774 = -479114688;    float dQCuYhldkBDZNgIHXDyG63918015 = -187807013;    float dQCuYhldkBDZNgIHXDyG54752668 = -608490042;    float dQCuYhldkBDZNgIHXDyG38628924 = -954749338;    float dQCuYhldkBDZNgIHXDyG13931518 = -492429444;    float dQCuYhldkBDZNgIHXDyG34804298 = -703537716;    float dQCuYhldkBDZNgIHXDyG95982247 = 61249926;    float dQCuYhldkBDZNgIHXDyG9139027 = -197327801;    float dQCuYhldkBDZNgIHXDyG48495571 = -65319670;    float dQCuYhldkBDZNgIHXDyG70693761 = -526692947;    float dQCuYhldkBDZNgIHXDyG41743519 = -459445538;    float dQCuYhldkBDZNgIHXDyG75932558 = -666229682;    float dQCuYhldkBDZNgIHXDyG41020732 = -245633566;    float dQCuYhldkBDZNgIHXDyG1518315 = -906543644;    float dQCuYhldkBDZNgIHXDyG59643971 = -849737109;    float dQCuYhldkBDZNgIHXDyG86384970 = -512089870;    float dQCuYhldkBDZNgIHXDyG97299563 = -604352670;    float dQCuYhldkBDZNgIHXDyG53293487 = -26686581;    float dQCuYhldkBDZNgIHXDyG12205793 = -223724430;    float dQCuYhldkBDZNgIHXDyG8032809 = -71328408;    float dQCuYhldkBDZNgIHXDyG31083636 = -798676860;    float dQCuYhldkBDZNgIHXDyG44422650 = -932900973;    float dQCuYhldkBDZNgIHXDyG70381061 = -282340024;    float dQCuYhldkBDZNgIHXDyG98839530 = -691932413;    float dQCuYhldkBDZNgIHXDyG73814156 = 23002070;    float dQCuYhldkBDZNgIHXDyG56518363 = -16137973;    float dQCuYhldkBDZNgIHXDyG99048156 = 85950024;    float dQCuYhldkBDZNgIHXDyG96840803 = -760300438;    float dQCuYhldkBDZNgIHXDyG38377842 = 10786069;    float dQCuYhldkBDZNgIHXDyG10239973 = -948708639;    float dQCuYhldkBDZNgIHXDyG45790969 = -885293821;    float dQCuYhldkBDZNgIHXDyG15290631 = -269857779;    float dQCuYhldkBDZNgIHXDyG5790347 = -109988606;    float dQCuYhldkBDZNgIHXDyG2090987 = -974896922;    float dQCuYhldkBDZNgIHXDyG29462647 = -901379232;    float dQCuYhldkBDZNgIHXDyG30090369 = -230911179;    float dQCuYhldkBDZNgIHXDyG76428053 = -386102539;    float dQCuYhldkBDZNgIHXDyG81695800 = -510963253;     dQCuYhldkBDZNgIHXDyG46963626 = dQCuYhldkBDZNgIHXDyG60050358;     dQCuYhldkBDZNgIHXDyG60050358 = dQCuYhldkBDZNgIHXDyG57478731;     dQCuYhldkBDZNgIHXDyG57478731 = dQCuYhldkBDZNgIHXDyG70289244;     dQCuYhldkBDZNgIHXDyG70289244 = dQCuYhldkBDZNgIHXDyG44391877;     dQCuYhldkBDZNgIHXDyG44391877 = dQCuYhldkBDZNgIHXDyG5051613;     dQCuYhldkBDZNgIHXDyG5051613 = dQCuYhldkBDZNgIHXDyG45417849;     dQCuYhldkBDZNgIHXDyG45417849 = dQCuYhldkBDZNgIHXDyG30904837;     dQCuYhldkBDZNgIHXDyG30904837 = dQCuYhldkBDZNgIHXDyG12179702;     dQCuYhldkBDZNgIHXDyG12179702 = dQCuYhldkBDZNgIHXDyG6840879;     dQCuYhldkBDZNgIHXDyG6840879 = dQCuYhldkBDZNgIHXDyG5871061;     dQCuYhldkBDZNgIHXDyG5871061 = dQCuYhldkBDZNgIHXDyG24236084;     dQCuYhldkBDZNgIHXDyG24236084 = dQCuYhldkBDZNgIHXDyG43018251;     dQCuYhldkBDZNgIHXDyG43018251 = dQCuYhldkBDZNgIHXDyG86134321;     dQCuYhldkBDZNgIHXDyG86134321 = dQCuYhldkBDZNgIHXDyG90001534;     dQCuYhldkBDZNgIHXDyG90001534 = dQCuYhldkBDZNgIHXDyG98589986;     dQCuYhldkBDZNgIHXDyG98589986 = dQCuYhldkBDZNgIHXDyG75091084;     dQCuYhldkBDZNgIHXDyG75091084 = dQCuYhldkBDZNgIHXDyG76379790;     dQCuYhldkBDZNgIHXDyG76379790 = dQCuYhldkBDZNgIHXDyG31503762;     dQCuYhldkBDZNgIHXDyG31503762 = dQCuYhldkBDZNgIHXDyG2598411;     dQCuYhldkBDZNgIHXDyG2598411 = dQCuYhldkBDZNgIHXDyG60816734;     dQCuYhldkBDZNgIHXDyG60816734 = dQCuYhldkBDZNgIHXDyG53652355;     dQCuYhldkBDZNgIHXDyG53652355 = dQCuYhldkBDZNgIHXDyG17565692;     dQCuYhldkBDZNgIHXDyG17565692 = dQCuYhldkBDZNgIHXDyG17624288;     dQCuYhldkBDZNgIHXDyG17624288 = dQCuYhldkBDZNgIHXDyG11682891;     dQCuYhldkBDZNgIHXDyG11682891 = dQCuYhldkBDZNgIHXDyG25046237;     dQCuYhldkBDZNgIHXDyG25046237 = dQCuYhldkBDZNgIHXDyG55270757;     dQCuYhldkBDZNgIHXDyG55270757 = dQCuYhldkBDZNgIHXDyG85931156;     dQCuYhldkBDZNgIHXDyG85931156 = dQCuYhldkBDZNgIHXDyG1069064;     dQCuYhldkBDZNgIHXDyG1069064 = dQCuYhldkBDZNgIHXDyG48261686;     dQCuYhldkBDZNgIHXDyG48261686 = dQCuYhldkBDZNgIHXDyG52088210;     dQCuYhldkBDZNgIHXDyG52088210 = dQCuYhldkBDZNgIHXDyG67242137;     dQCuYhldkBDZNgIHXDyG67242137 = dQCuYhldkBDZNgIHXDyG10304566;     dQCuYhldkBDZNgIHXDyG10304566 = dQCuYhldkBDZNgIHXDyG8213954;     dQCuYhldkBDZNgIHXDyG8213954 = dQCuYhldkBDZNgIHXDyG90152074;     dQCuYhldkBDZNgIHXDyG90152074 = dQCuYhldkBDZNgIHXDyG80862508;     dQCuYhldkBDZNgIHXDyG80862508 = dQCuYhldkBDZNgIHXDyG50094416;     dQCuYhldkBDZNgIHXDyG50094416 = dQCuYhldkBDZNgIHXDyG4397323;     dQCuYhldkBDZNgIHXDyG4397323 = dQCuYhldkBDZNgIHXDyG34636272;     dQCuYhldkBDZNgIHXDyG34636272 = dQCuYhldkBDZNgIHXDyG55571203;     dQCuYhldkBDZNgIHXDyG55571203 = dQCuYhldkBDZNgIHXDyG61577679;     dQCuYhldkBDZNgIHXDyG61577679 = dQCuYhldkBDZNgIHXDyG59298420;     dQCuYhldkBDZNgIHXDyG59298420 = dQCuYhldkBDZNgIHXDyG94008384;     dQCuYhldkBDZNgIHXDyG94008384 = dQCuYhldkBDZNgIHXDyG31180721;     dQCuYhldkBDZNgIHXDyG31180721 = dQCuYhldkBDZNgIHXDyG20324724;     dQCuYhldkBDZNgIHXDyG20324724 = dQCuYhldkBDZNgIHXDyG58389404;     dQCuYhldkBDZNgIHXDyG58389404 = dQCuYhldkBDZNgIHXDyG12840444;     dQCuYhldkBDZNgIHXDyG12840444 = dQCuYhldkBDZNgIHXDyG47237948;     dQCuYhldkBDZNgIHXDyG47237948 = dQCuYhldkBDZNgIHXDyG54847520;     dQCuYhldkBDZNgIHXDyG54847520 = dQCuYhldkBDZNgIHXDyG56646413;     dQCuYhldkBDZNgIHXDyG56646413 = dQCuYhldkBDZNgIHXDyG77880624;     dQCuYhldkBDZNgIHXDyG77880624 = dQCuYhldkBDZNgIHXDyG53248679;     dQCuYhldkBDZNgIHXDyG53248679 = dQCuYhldkBDZNgIHXDyG93427980;     dQCuYhldkBDZNgIHXDyG93427980 = dQCuYhldkBDZNgIHXDyG53786203;     dQCuYhldkBDZNgIHXDyG53786203 = dQCuYhldkBDZNgIHXDyG9165797;     dQCuYhldkBDZNgIHXDyG9165797 = dQCuYhldkBDZNgIHXDyG93311270;     dQCuYhldkBDZNgIHXDyG93311270 = dQCuYhldkBDZNgIHXDyG42484666;     dQCuYhldkBDZNgIHXDyG42484666 = dQCuYhldkBDZNgIHXDyG39854443;     dQCuYhldkBDZNgIHXDyG39854443 = dQCuYhldkBDZNgIHXDyG58606354;     dQCuYhldkBDZNgIHXDyG58606354 = dQCuYhldkBDZNgIHXDyG19345641;     dQCuYhldkBDZNgIHXDyG19345641 = dQCuYhldkBDZNgIHXDyG49780856;     dQCuYhldkBDZNgIHXDyG49780856 = dQCuYhldkBDZNgIHXDyG59486693;     dQCuYhldkBDZNgIHXDyG59486693 = dQCuYhldkBDZNgIHXDyG29835774;     dQCuYhldkBDZNgIHXDyG29835774 = dQCuYhldkBDZNgIHXDyG63918015;     dQCuYhldkBDZNgIHXDyG63918015 = dQCuYhldkBDZNgIHXDyG54752668;     dQCuYhldkBDZNgIHXDyG54752668 = dQCuYhldkBDZNgIHXDyG38628924;     dQCuYhldkBDZNgIHXDyG38628924 = dQCuYhldkBDZNgIHXDyG13931518;     dQCuYhldkBDZNgIHXDyG13931518 = dQCuYhldkBDZNgIHXDyG34804298;     dQCuYhldkBDZNgIHXDyG34804298 = dQCuYhldkBDZNgIHXDyG95982247;     dQCuYhldkBDZNgIHXDyG95982247 = dQCuYhldkBDZNgIHXDyG9139027;     dQCuYhldkBDZNgIHXDyG9139027 = dQCuYhldkBDZNgIHXDyG48495571;     dQCuYhldkBDZNgIHXDyG48495571 = dQCuYhldkBDZNgIHXDyG70693761;     dQCuYhldkBDZNgIHXDyG70693761 = dQCuYhldkBDZNgIHXDyG41743519;     dQCuYhldkBDZNgIHXDyG41743519 = dQCuYhldkBDZNgIHXDyG75932558;     dQCuYhldkBDZNgIHXDyG75932558 = dQCuYhldkBDZNgIHXDyG41020732;     dQCuYhldkBDZNgIHXDyG41020732 = dQCuYhldkBDZNgIHXDyG1518315;     dQCuYhldkBDZNgIHXDyG1518315 = dQCuYhldkBDZNgIHXDyG59643971;     dQCuYhldkBDZNgIHXDyG59643971 = dQCuYhldkBDZNgIHXDyG86384970;     dQCuYhldkBDZNgIHXDyG86384970 = dQCuYhldkBDZNgIHXDyG97299563;     dQCuYhldkBDZNgIHXDyG97299563 = dQCuYhldkBDZNgIHXDyG53293487;     dQCuYhldkBDZNgIHXDyG53293487 = dQCuYhldkBDZNgIHXDyG12205793;     dQCuYhldkBDZNgIHXDyG12205793 = dQCuYhldkBDZNgIHXDyG8032809;     dQCuYhldkBDZNgIHXDyG8032809 = dQCuYhldkBDZNgIHXDyG31083636;     dQCuYhldkBDZNgIHXDyG31083636 = dQCuYhldkBDZNgIHXDyG44422650;     dQCuYhldkBDZNgIHXDyG44422650 = dQCuYhldkBDZNgIHXDyG70381061;     dQCuYhldkBDZNgIHXDyG70381061 = dQCuYhldkBDZNgIHXDyG98839530;     dQCuYhldkBDZNgIHXDyG98839530 = dQCuYhldkBDZNgIHXDyG73814156;     dQCuYhldkBDZNgIHXDyG73814156 = dQCuYhldkBDZNgIHXDyG56518363;     dQCuYhldkBDZNgIHXDyG56518363 = dQCuYhldkBDZNgIHXDyG99048156;     dQCuYhldkBDZNgIHXDyG99048156 = dQCuYhldkBDZNgIHXDyG96840803;     dQCuYhldkBDZNgIHXDyG96840803 = dQCuYhldkBDZNgIHXDyG38377842;     dQCuYhldkBDZNgIHXDyG38377842 = dQCuYhldkBDZNgIHXDyG10239973;     dQCuYhldkBDZNgIHXDyG10239973 = dQCuYhldkBDZNgIHXDyG45790969;     dQCuYhldkBDZNgIHXDyG45790969 = dQCuYhldkBDZNgIHXDyG15290631;     dQCuYhldkBDZNgIHXDyG15290631 = dQCuYhldkBDZNgIHXDyG5790347;     dQCuYhldkBDZNgIHXDyG5790347 = dQCuYhldkBDZNgIHXDyG2090987;     dQCuYhldkBDZNgIHXDyG2090987 = dQCuYhldkBDZNgIHXDyG29462647;     dQCuYhldkBDZNgIHXDyG29462647 = dQCuYhldkBDZNgIHXDyG30090369;     dQCuYhldkBDZNgIHXDyG30090369 = dQCuYhldkBDZNgIHXDyG76428053;     dQCuYhldkBDZNgIHXDyG76428053 = dQCuYhldkBDZNgIHXDyG81695800;     dQCuYhldkBDZNgIHXDyG81695800 = dQCuYhldkBDZNgIHXDyG46963626;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pYJsVnBIsEOOfFZfJuQn24398745() {     float ECzXHvJBtEdPmsoGXFiH17615293 = -29112384;    float ECzXHvJBtEdPmsoGXFiH28723876 = -13245697;    float ECzXHvJBtEdPmsoGXFiH37447650 = -878481753;    float ECzXHvJBtEdPmsoGXFiH10220532 = -687344965;    float ECzXHvJBtEdPmsoGXFiH65842198 = -693747087;    float ECzXHvJBtEdPmsoGXFiH95051375 = -622525655;    float ECzXHvJBtEdPmsoGXFiH63796745 = -872218949;    float ECzXHvJBtEdPmsoGXFiH87747217 = -54367451;    float ECzXHvJBtEdPmsoGXFiH58044756 = -952927144;    float ECzXHvJBtEdPmsoGXFiH29807339 = -116973048;    float ECzXHvJBtEdPmsoGXFiH48120221 = -664561639;    float ECzXHvJBtEdPmsoGXFiH69782654 = -698667142;    float ECzXHvJBtEdPmsoGXFiH22845638 = -441433382;    float ECzXHvJBtEdPmsoGXFiH69981355 = -742586675;    float ECzXHvJBtEdPmsoGXFiH72721554 = 28788318;    float ECzXHvJBtEdPmsoGXFiH31118627 = -110755069;    float ECzXHvJBtEdPmsoGXFiH68943910 = -596751504;    float ECzXHvJBtEdPmsoGXFiH96673284 = -49621841;    float ECzXHvJBtEdPmsoGXFiH13211543 = -678281446;    float ECzXHvJBtEdPmsoGXFiH61385120 = -927321792;    float ECzXHvJBtEdPmsoGXFiH15952489 = -195716703;    float ECzXHvJBtEdPmsoGXFiH15048121 = -699740606;    float ECzXHvJBtEdPmsoGXFiH19509501 = -74525595;    float ECzXHvJBtEdPmsoGXFiH51473824 = -601845026;    float ECzXHvJBtEdPmsoGXFiH75807279 = -997531658;    float ECzXHvJBtEdPmsoGXFiH55643817 = -677472856;    float ECzXHvJBtEdPmsoGXFiH63531542 = -21247062;    float ECzXHvJBtEdPmsoGXFiH93713381 = -114142007;    float ECzXHvJBtEdPmsoGXFiH33211552 = -522793890;    float ECzXHvJBtEdPmsoGXFiH87433781 = -30167873;    float ECzXHvJBtEdPmsoGXFiH59160206 = 44178659;    float ECzXHvJBtEdPmsoGXFiH11753598 = 96792465;    float ECzXHvJBtEdPmsoGXFiH93249926 = -837435514;    float ECzXHvJBtEdPmsoGXFiH73578928 = -391888292;    float ECzXHvJBtEdPmsoGXFiH68176310 = -749755170;    float ECzXHvJBtEdPmsoGXFiH40855767 = -703360427;    float ECzXHvJBtEdPmsoGXFiH93885901 = -564045020;    float ECzXHvJBtEdPmsoGXFiH57189811 = -944986535;    float ECzXHvJBtEdPmsoGXFiH13169561 = -556738744;    float ECzXHvJBtEdPmsoGXFiH50674501 = -922472185;    float ECzXHvJBtEdPmsoGXFiH21650964 = -423340800;    float ECzXHvJBtEdPmsoGXFiH87781411 = -149882989;    float ECzXHvJBtEdPmsoGXFiH42975344 = -505472244;    float ECzXHvJBtEdPmsoGXFiH37689022 = -66567552;    float ECzXHvJBtEdPmsoGXFiH16278783 = -613737845;    float ECzXHvJBtEdPmsoGXFiH64795238 = -953277545;    float ECzXHvJBtEdPmsoGXFiH38971646 = -371533781;    float ECzXHvJBtEdPmsoGXFiH12852158 = 93128826;    float ECzXHvJBtEdPmsoGXFiH60288972 = -896105349;    float ECzXHvJBtEdPmsoGXFiH65073899 = -578543579;    float ECzXHvJBtEdPmsoGXFiH46069949 = -836004900;    float ECzXHvJBtEdPmsoGXFiH95120183 = -484674965;    float ECzXHvJBtEdPmsoGXFiH48503637 = -525710484;    float ECzXHvJBtEdPmsoGXFiH44013507 = -335217430;    float ECzXHvJBtEdPmsoGXFiH8446256 = -925249547;    float ECzXHvJBtEdPmsoGXFiH2567173 = -329371779;    float ECzXHvJBtEdPmsoGXFiH9214375 = -938720103;    float ECzXHvJBtEdPmsoGXFiH85973825 = -176636728;    float ECzXHvJBtEdPmsoGXFiH34413252 = -689813308;    float ECzXHvJBtEdPmsoGXFiH10198382 = 83725768;    float ECzXHvJBtEdPmsoGXFiH31519833 = -501278594;    float ECzXHvJBtEdPmsoGXFiH70083363 = -658076942;    float ECzXHvJBtEdPmsoGXFiH54535666 = -531573561;    float ECzXHvJBtEdPmsoGXFiH70610974 = -822759271;    float ECzXHvJBtEdPmsoGXFiH70647133 = -61151707;    float ECzXHvJBtEdPmsoGXFiH36366624 = -661354104;    float ECzXHvJBtEdPmsoGXFiH76532728 = -861231629;    float ECzXHvJBtEdPmsoGXFiH49266710 = 50454910;    float ECzXHvJBtEdPmsoGXFiH1805045 = -992831505;    float ECzXHvJBtEdPmsoGXFiH31865787 = -267851256;    float ECzXHvJBtEdPmsoGXFiH37232726 = -546710050;    float ECzXHvJBtEdPmsoGXFiH11754100 = -651764969;    float ECzXHvJBtEdPmsoGXFiH83503724 = -492883097;    float ECzXHvJBtEdPmsoGXFiH62537042 = -755809261;    float ECzXHvJBtEdPmsoGXFiH39734157 = -403980993;    float ECzXHvJBtEdPmsoGXFiH28171078 = 54166285;    float ECzXHvJBtEdPmsoGXFiH72072776 = -94268362;    float ECzXHvJBtEdPmsoGXFiH81820478 = 92041957;    float ECzXHvJBtEdPmsoGXFiH35195042 = -988107181;    float ECzXHvJBtEdPmsoGXFiH11012041 = 55745886;    float ECzXHvJBtEdPmsoGXFiH16672171 = -205939076;    float ECzXHvJBtEdPmsoGXFiH50679384 = -14375888;    float ECzXHvJBtEdPmsoGXFiH33424409 = -218036658;    float ECzXHvJBtEdPmsoGXFiH68137652 = -944250311;    float ECzXHvJBtEdPmsoGXFiH41363833 = -194162974;    float ECzXHvJBtEdPmsoGXFiH64040023 = -471146377;    float ECzXHvJBtEdPmsoGXFiH63249960 = -377497051;    float ECzXHvJBtEdPmsoGXFiH49236419 = -402218085;    float ECzXHvJBtEdPmsoGXFiH65132673 = -466638746;    float ECzXHvJBtEdPmsoGXFiH65609138 = -320383392;    float ECzXHvJBtEdPmsoGXFiH31641392 = -764640324;    float ECzXHvJBtEdPmsoGXFiH7912077 = -287408293;    float ECzXHvJBtEdPmsoGXFiH22776560 = -155173227;    float ECzXHvJBtEdPmsoGXFiH2971180 = -540464513;    float ECzXHvJBtEdPmsoGXFiH19154669 = -321193592;    float ECzXHvJBtEdPmsoGXFiH51567600 = -765263858;    float ECzXHvJBtEdPmsoGXFiH33245745 = -618309429;    float ECzXHvJBtEdPmsoGXFiH72364370 = -682712973;    float ECzXHvJBtEdPmsoGXFiH67041889 = 94584154;    float ECzXHvJBtEdPmsoGXFiH79912159 = -29112384;     ECzXHvJBtEdPmsoGXFiH17615293 = ECzXHvJBtEdPmsoGXFiH28723876;     ECzXHvJBtEdPmsoGXFiH28723876 = ECzXHvJBtEdPmsoGXFiH37447650;     ECzXHvJBtEdPmsoGXFiH37447650 = ECzXHvJBtEdPmsoGXFiH10220532;     ECzXHvJBtEdPmsoGXFiH10220532 = ECzXHvJBtEdPmsoGXFiH65842198;     ECzXHvJBtEdPmsoGXFiH65842198 = ECzXHvJBtEdPmsoGXFiH95051375;     ECzXHvJBtEdPmsoGXFiH95051375 = ECzXHvJBtEdPmsoGXFiH63796745;     ECzXHvJBtEdPmsoGXFiH63796745 = ECzXHvJBtEdPmsoGXFiH87747217;     ECzXHvJBtEdPmsoGXFiH87747217 = ECzXHvJBtEdPmsoGXFiH58044756;     ECzXHvJBtEdPmsoGXFiH58044756 = ECzXHvJBtEdPmsoGXFiH29807339;     ECzXHvJBtEdPmsoGXFiH29807339 = ECzXHvJBtEdPmsoGXFiH48120221;     ECzXHvJBtEdPmsoGXFiH48120221 = ECzXHvJBtEdPmsoGXFiH69782654;     ECzXHvJBtEdPmsoGXFiH69782654 = ECzXHvJBtEdPmsoGXFiH22845638;     ECzXHvJBtEdPmsoGXFiH22845638 = ECzXHvJBtEdPmsoGXFiH69981355;     ECzXHvJBtEdPmsoGXFiH69981355 = ECzXHvJBtEdPmsoGXFiH72721554;     ECzXHvJBtEdPmsoGXFiH72721554 = ECzXHvJBtEdPmsoGXFiH31118627;     ECzXHvJBtEdPmsoGXFiH31118627 = ECzXHvJBtEdPmsoGXFiH68943910;     ECzXHvJBtEdPmsoGXFiH68943910 = ECzXHvJBtEdPmsoGXFiH96673284;     ECzXHvJBtEdPmsoGXFiH96673284 = ECzXHvJBtEdPmsoGXFiH13211543;     ECzXHvJBtEdPmsoGXFiH13211543 = ECzXHvJBtEdPmsoGXFiH61385120;     ECzXHvJBtEdPmsoGXFiH61385120 = ECzXHvJBtEdPmsoGXFiH15952489;     ECzXHvJBtEdPmsoGXFiH15952489 = ECzXHvJBtEdPmsoGXFiH15048121;     ECzXHvJBtEdPmsoGXFiH15048121 = ECzXHvJBtEdPmsoGXFiH19509501;     ECzXHvJBtEdPmsoGXFiH19509501 = ECzXHvJBtEdPmsoGXFiH51473824;     ECzXHvJBtEdPmsoGXFiH51473824 = ECzXHvJBtEdPmsoGXFiH75807279;     ECzXHvJBtEdPmsoGXFiH75807279 = ECzXHvJBtEdPmsoGXFiH55643817;     ECzXHvJBtEdPmsoGXFiH55643817 = ECzXHvJBtEdPmsoGXFiH63531542;     ECzXHvJBtEdPmsoGXFiH63531542 = ECzXHvJBtEdPmsoGXFiH93713381;     ECzXHvJBtEdPmsoGXFiH93713381 = ECzXHvJBtEdPmsoGXFiH33211552;     ECzXHvJBtEdPmsoGXFiH33211552 = ECzXHvJBtEdPmsoGXFiH87433781;     ECzXHvJBtEdPmsoGXFiH87433781 = ECzXHvJBtEdPmsoGXFiH59160206;     ECzXHvJBtEdPmsoGXFiH59160206 = ECzXHvJBtEdPmsoGXFiH11753598;     ECzXHvJBtEdPmsoGXFiH11753598 = ECzXHvJBtEdPmsoGXFiH93249926;     ECzXHvJBtEdPmsoGXFiH93249926 = ECzXHvJBtEdPmsoGXFiH73578928;     ECzXHvJBtEdPmsoGXFiH73578928 = ECzXHvJBtEdPmsoGXFiH68176310;     ECzXHvJBtEdPmsoGXFiH68176310 = ECzXHvJBtEdPmsoGXFiH40855767;     ECzXHvJBtEdPmsoGXFiH40855767 = ECzXHvJBtEdPmsoGXFiH93885901;     ECzXHvJBtEdPmsoGXFiH93885901 = ECzXHvJBtEdPmsoGXFiH57189811;     ECzXHvJBtEdPmsoGXFiH57189811 = ECzXHvJBtEdPmsoGXFiH13169561;     ECzXHvJBtEdPmsoGXFiH13169561 = ECzXHvJBtEdPmsoGXFiH50674501;     ECzXHvJBtEdPmsoGXFiH50674501 = ECzXHvJBtEdPmsoGXFiH21650964;     ECzXHvJBtEdPmsoGXFiH21650964 = ECzXHvJBtEdPmsoGXFiH87781411;     ECzXHvJBtEdPmsoGXFiH87781411 = ECzXHvJBtEdPmsoGXFiH42975344;     ECzXHvJBtEdPmsoGXFiH42975344 = ECzXHvJBtEdPmsoGXFiH37689022;     ECzXHvJBtEdPmsoGXFiH37689022 = ECzXHvJBtEdPmsoGXFiH16278783;     ECzXHvJBtEdPmsoGXFiH16278783 = ECzXHvJBtEdPmsoGXFiH64795238;     ECzXHvJBtEdPmsoGXFiH64795238 = ECzXHvJBtEdPmsoGXFiH38971646;     ECzXHvJBtEdPmsoGXFiH38971646 = ECzXHvJBtEdPmsoGXFiH12852158;     ECzXHvJBtEdPmsoGXFiH12852158 = ECzXHvJBtEdPmsoGXFiH60288972;     ECzXHvJBtEdPmsoGXFiH60288972 = ECzXHvJBtEdPmsoGXFiH65073899;     ECzXHvJBtEdPmsoGXFiH65073899 = ECzXHvJBtEdPmsoGXFiH46069949;     ECzXHvJBtEdPmsoGXFiH46069949 = ECzXHvJBtEdPmsoGXFiH95120183;     ECzXHvJBtEdPmsoGXFiH95120183 = ECzXHvJBtEdPmsoGXFiH48503637;     ECzXHvJBtEdPmsoGXFiH48503637 = ECzXHvJBtEdPmsoGXFiH44013507;     ECzXHvJBtEdPmsoGXFiH44013507 = ECzXHvJBtEdPmsoGXFiH8446256;     ECzXHvJBtEdPmsoGXFiH8446256 = ECzXHvJBtEdPmsoGXFiH2567173;     ECzXHvJBtEdPmsoGXFiH2567173 = ECzXHvJBtEdPmsoGXFiH9214375;     ECzXHvJBtEdPmsoGXFiH9214375 = ECzXHvJBtEdPmsoGXFiH85973825;     ECzXHvJBtEdPmsoGXFiH85973825 = ECzXHvJBtEdPmsoGXFiH34413252;     ECzXHvJBtEdPmsoGXFiH34413252 = ECzXHvJBtEdPmsoGXFiH10198382;     ECzXHvJBtEdPmsoGXFiH10198382 = ECzXHvJBtEdPmsoGXFiH31519833;     ECzXHvJBtEdPmsoGXFiH31519833 = ECzXHvJBtEdPmsoGXFiH70083363;     ECzXHvJBtEdPmsoGXFiH70083363 = ECzXHvJBtEdPmsoGXFiH54535666;     ECzXHvJBtEdPmsoGXFiH54535666 = ECzXHvJBtEdPmsoGXFiH70610974;     ECzXHvJBtEdPmsoGXFiH70610974 = ECzXHvJBtEdPmsoGXFiH70647133;     ECzXHvJBtEdPmsoGXFiH70647133 = ECzXHvJBtEdPmsoGXFiH36366624;     ECzXHvJBtEdPmsoGXFiH36366624 = ECzXHvJBtEdPmsoGXFiH76532728;     ECzXHvJBtEdPmsoGXFiH76532728 = ECzXHvJBtEdPmsoGXFiH49266710;     ECzXHvJBtEdPmsoGXFiH49266710 = ECzXHvJBtEdPmsoGXFiH1805045;     ECzXHvJBtEdPmsoGXFiH1805045 = ECzXHvJBtEdPmsoGXFiH31865787;     ECzXHvJBtEdPmsoGXFiH31865787 = ECzXHvJBtEdPmsoGXFiH37232726;     ECzXHvJBtEdPmsoGXFiH37232726 = ECzXHvJBtEdPmsoGXFiH11754100;     ECzXHvJBtEdPmsoGXFiH11754100 = ECzXHvJBtEdPmsoGXFiH83503724;     ECzXHvJBtEdPmsoGXFiH83503724 = ECzXHvJBtEdPmsoGXFiH62537042;     ECzXHvJBtEdPmsoGXFiH62537042 = ECzXHvJBtEdPmsoGXFiH39734157;     ECzXHvJBtEdPmsoGXFiH39734157 = ECzXHvJBtEdPmsoGXFiH28171078;     ECzXHvJBtEdPmsoGXFiH28171078 = ECzXHvJBtEdPmsoGXFiH72072776;     ECzXHvJBtEdPmsoGXFiH72072776 = ECzXHvJBtEdPmsoGXFiH81820478;     ECzXHvJBtEdPmsoGXFiH81820478 = ECzXHvJBtEdPmsoGXFiH35195042;     ECzXHvJBtEdPmsoGXFiH35195042 = ECzXHvJBtEdPmsoGXFiH11012041;     ECzXHvJBtEdPmsoGXFiH11012041 = ECzXHvJBtEdPmsoGXFiH16672171;     ECzXHvJBtEdPmsoGXFiH16672171 = ECzXHvJBtEdPmsoGXFiH50679384;     ECzXHvJBtEdPmsoGXFiH50679384 = ECzXHvJBtEdPmsoGXFiH33424409;     ECzXHvJBtEdPmsoGXFiH33424409 = ECzXHvJBtEdPmsoGXFiH68137652;     ECzXHvJBtEdPmsoGXFiH68137652 = ECzXHvJBtEdPmsoGXFiH41363833;     ECzXHvJBtEdPmsoGXFiH41363833 = ECzXHvJBtEdPmsoGXFiH64040023;     ECzXHvJBtEdPmsoGXFiH64040023 = ECzXHvJBtEdPmsoGXFiH63249960;     ECzXHvJBtEdPmsoGXFiH63249960 = ECzXHvJBtEdPmsoGXFiH49236419;     ECzXHvJBtEdPmsoGXFiH49236419 = ECzXHvJBtEdPmsoGXFiH65132673;     ECzXHvJBtEdPmsoGXFiH65132673 = ECzXHvJBtEdPmsoGXFiH65609138;     ECzXHvJBtEdPmsoGXFiH65609138 = ECzXHvJBtEdPmsoGXFiH31641392;     ECzXHvJBtEdPmsoGXFiH31641392 = ECzXHvJBtEdPmsoGXFiH7912077;     ECzXHvJBtEdPmsoGXFiH7912077 = ECzXHvJBtEdPmsoGXFiH22776560;     ECzXHvJBtEdPmsoGXFiH22776560 = ECzXHvJBtEdPmsoGXFiH2971180;     ECzXHvJBtEdPmsoGXFiH2971180 = ECzXHvJBtEdPmsoGXFiH19154669;     ECzXHvJBtEdPmsoGXFiH19154669 = ECzXHvJBtEdPmsoGXFiH51567600;     ECzXHvJBtEdPmsoGXFiH51567600 = ECzXHvJBtEdPmsoGXFiH33245745;     ECzXHvJBtEdPmsoGXFiH33245745 = ECzXHvJBtEdPmsoGXFiH72364370;     ECzXHvJBtEdPmsoGXFiH72364370 = ECzXHvJBtEdPmsoGXFiH67041889;     ECzXHvJBtEdPmsoGXFiH67041889 = ECzXHvJBtEdPmsoGXFiH79912159;     ECzXHvJBtEdPmsoGXFiH79912159 = ECzXHvJBtEdPmsoGXFiH17615293;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MjDRPlGxyGqujlqPQnZK31057489() {     float shwepbOYGDNRmINtiIpS17608822 = 98363475;    float shwepbOYGDNRmINtiIpS67296192 = -55840823;    float shwepbOYGDNRmINtiIpS38760862 = -835610367;    float shwepbOYGDNRmINtiIpS95300392 = -507885795;    float shwepbOYGDNRmINtiIpS74682300 = -62102409;    float shwepbOYGDNRmINtiIpS29856808 = -321958467;    float shwepbOYGDNRmINtiIpS32344081 = -576755416;    float shwepbOYGDNRmINtiIpS56961268 = -858544768;    float shwepbOYGDNRmINtiIpS40784920 = -666573214;    float shwepbOYGDNRmINtiIpS26446853 = -853779919;    float shwepbOYGDNRmINtiIpS68440826 = -624033711;    float shwepbOYGDNRmINtiIpS84074331 = -434145504;    float shwepbOYGDNRmINtiIpS54698552 = -463091856;    float shwepbOYGDNRmINtiIpS23825576 = 12260274;    float shwepbOYGDNRmINtiIpS17893664 = 66075466;    float shwepbOYGDNRmINtiIpS5683175 = -443550720;    float shwepbOYGDNRmINtiIpS24796670 = -416366073;    float shwepbOYGDNRmINtiIpS19673818 = -446693463;    float shwepbOYGDNRmINtiIpS33639813 = -196263713;    float shwepbOYGDNRmINtiIpS33458883 = -439346244;    float shwepbOYGDNRmINtiIpS95621913 = 93816923;    float shwepbOYGDNRmINtiIpS93344386 = -109732489;    float shwepbOYGDNRmINtiIpS56387897 = -533483437;    float shwepbOYGDNRmINtiIpS95123932 = -482765548;    float shwepbOYGDNRmINtiIpS4249969 = -324225511;    float shwepbOYGDNRmINtiIpS72217894 = -585678784;    float shwepbOYGDNRmINtiIpS33753124 = -322046775;    float shwepbOYGDNRmINtiIpS29116906 = -723108995;    float shwepbOYGDNRmINtiIpS45889998 = -68864205;    float shwepbOYGDNRmINtiIpS63523651 = -805599247;    float shwepbOYGDNRmINtiIpS92572884 = -342914576;    float shwepbOYGDNRmINtiIpS96477455 = -295029296;    float shwepbOYGDNRmINtiIpS78702350 = 60620270;    float shwepbOYGDNRmINtiIpS73861765 = -268570025;    float shwepbOYGDNRmINtiIpS50534349 = 90930905;    float shwepbOYGDNRmINtiIpS83683956 = -783126173;    float shwepbOYGDNRmINtiIpS5827270 = -663698399;    float shwepbOYGDNRmINtiIpS27107491 = -940249214;    float shwepbOYGDNRmINtiIpS48483157 = -169420160;    float shwepbOYGDNRmINtiIpS22547394 = -531903871;    float shwepbOYGDNRmINtiIpS9506631 = -511284699;    float shwepbOYGDNRmINtiIpS73479072 = -343675071;    float shwepbOYGDNRmINtiIpS3258705 = 35397502;    float shwepbOYGDNRmINtiIpS96920920 = -723531996;    float shwepbOYGDNRmINtiIpS296835 = 13002753;    float shwepbOYGDNRmINtiIpS5223207 = -765322215;    float shwepbOYGDNRmINtiIpS66669439 = -852814403;    float shwepbOYGDNRmINtiIpS88235739 = -515632530;    float shwepbOYGDNRmINtiIpS78947361 = -459869210;    float shwepbOYGDNRmINtiIpS85119322 = -165568466;    float shwepbOYGDNRmINtiIpS59329318 = -414559907;    float shwepbOYGDNRmINtiIpS9667441 = -866820713;    float shwepbOYGDNRmINtiIpS23951986 = -417764492;    float shwepbOYGDNRmINtiIpS54263778 = -867137281;    float shwepbOYGDNRmINtiIpS35435576 = -682148824;    float shwepbOYGDNRmINtiIpS24264436 = -791904037;    float shwepbOYGDNRmINtiIpS10908295 = -522357386;    float shwepbOYGDNRmINtiIpS43636929 = -252844819;    float shwepbOYGDNRmINtiIpS91050423 = -83660285;    float shwepbOYGDNRmINtiIpS2464407 = -476423625;    float shwepbOYGDNRmINtiIpS96103683 = -999911692;    float shwepbOYGDNRmINtiIpS3227175 = -853646421;    float shwepbOYGDNRmINtiIpS11071270 = -689680563;    float shwepbOYGDNRmINtiIpS77261268 = -860973967;    float shwepbOYGDNRmINtiIpS33873968 = -410865344;    float shwepbOYGDNRmINtiIpS71963371 = -229004415;    float shwepbOYGDNRmINtiIpS5371982 = -394765775;    float shwepbOYGDNRmINtiIpS80836786 = -94521832;    float shwepbOYGDNRmINtiIpS73291227 = 21329368;    float shwepbOYGDNRmINtiIpS34209707 = -150798362;    float shwepbOYGDNRmINtiIpS99855905 = -779852322;    float shwepbOYGDNRmINtiIpS97689179 = -476116860;    float shwepbOYGDNRmINtiIpS71190660 = -177273303;    float shwepbOYGDNRmINtiIpS11092420 = -664359842;    float shwepbOYGDNRmINtiIpS23952252 = -928061546;    float shwepbOYGDNRmINtiIpS22142841 = -562508006;    float shwepbOYGDNRmINtiIpS90085681 = -45129991;    float shwepbOYGDNRmINtiIpS59466977 = -809951442;    float shwepbOYGDNRmINtiIpS94827097 = -395768302;    float shwepbOYGDNRmINtiIpS99026761 = -558903296;    float shwepbOYGDNRmINtiIpS5548456 = -732864382;    float shwepbOYGDNRmINtiIpS45517385 = -806414245;    float shwepbOYGDNRmINtiIpS50169545 = -163239785;    float shwepbOYGDNRmINtiIpS60770676 = -903295740;    float shwepbOYGDNRmINtiIpS4194333 = -291039340;    float shwepbOYGDNRmINtiIpS82905444 = -476093863;    float shwepbOYGDNRmINtiIpS72525469 = -877264805;    float shwepbOYGDNRmINtiIpS24438572 = -72242449;    float shwepbOYGDNRmINtiIpS38426190 = -586421201;    float shwepbOYGDNRmINtiIpS26269913 = -117165059;    float shwepbOYGDNRmINtiIpS72775662 = -160768787;    float shwepbOYGDNRmINtiIpS62190340 = -310853580;    float shwepbOYGDNRmINtiIpS36057068 = -756588929;    float shwepbOYGDNRmINtiIpS46018751 = -692996536;    float shwepbOYGDNRmINtiIpS26443711 = -531992179;    float shwepbOYGDNRmINtiIpS6279457 = -657638278;    float shwepbOYGDNRmINtiIpS62407802 = -653994509;    float shwepbOYGDNRmINtiIpS25997436 = -103628531;    float shwepbOYGDNRmINtiIpS63046952 = -212666652;    float shwepbOYGDNRmINtiIpS28333464 = 98363475;     shwepbOYGDNRmINtiIpS17608822 = shwepbOYGDNRmINtiIpS67296192;     shwepbOYGDNRmINtiIpS67296192 = shwepbOYGDNRmINtiIpS38760862;     shwepbOYGDNRmINtiIpS38760862 = shwepbOYGDNRmINtiIpS95300392;     shwepbOYGDNRmINtiIpS95300392 = shwepbOYGDNRmINtiIpS74682300;     shwepbOYGDNRmINtiIpS74682300 = shwepbOYGDNRmINtiIpS29856808;     shwepbOYGDNRmINtiIpS29856808 = shwepbOYGDNRmINtiIpS32344081;     shwepbOYGDNRmINtiIpS32344081 = shwepbOYGDNRmINtiIpS56961268;     shwepbOYGDNRmINtiIpS56961268 = shwepbOYGDNRmINtiIpS40784920;     shwepbOYGDNRmINtiIpS40784920 = shwepbOYGDNRmINtiIpS26446853;     shwepbOYGDNRmINtiIpS26446853 = shwepbOYGDNRmINtiIpS68440826;     shwepbOYGDNRmINtiIpS68440826 = shwepbOYGDNRmINtiIpS84074331;     shwepbOYGDNRmINtiIpS84074331 = shwepbOYGDNRmINtiIpS54698552;     shwepbOYGDNRmINtiIpS54698552 = shwepbOYGDNRmINtiIpS23825576;     shwepbOYGDNRmINtiIpS23825576 = shwepbOYGDNRmINtiIpS17893664;     shwepbOYGDNRmINtiIpS17893664 = shwepbOYGDNRmINtiIpS5683175;     shwepbOYGDNRmINtiIpS5683175 = shwepbOYGDNRmINtiIpS24796670;     shwepbOYGDNRmINtiIpS24796670 = shwepbOYGDNRmINtiIpS19673818;     shwepbOYGDNRmINtiIpS19673818 = shwepbOYGDNRmINtiIpS33639813;     shwepbOYGDNRmINtiIpS33639813 = shwepbOYGDNRmINtiIpS33458883;     shwepbOYGDNRmINtiIpS33458883 = shwepbOYGDNRmINtiIpS95621913;     shwepbOYGDNRmINtiIpS95621913 = shwepbOYGDNRmINtiIpS93344386;     shwepbOYGDNRmINtiIpS93344386 = shwepbOYGDNRmINtiIpS56387897;     shwepbOYGDNRmINtiIpS56387897 = shwepbOYGDNRmINtiIpS95123932;     shwepbOYGDNRmINtiIpS95123932 = shwepbOYGDNRmINtiIpS4249969;     shwepbOYGDNRmINtiIpS4249969 = shwepbOYGDNRmINtiIpS72217894;     shwepbOYGDNRmINtiIpS72217894 = shwepbOYGDNRmINtiIpS33753124;     shwepbOYGDNRmINtiIpS33753124 = shwepbOYGDNRmINtiIpS29116906;     shwepbOYGDNRmINtiIpS29116906 = shwepbOYGDNRmINtiIpS45889998;     shwepbOYGDNRmINtiIpS45889998 = shwepbOYGDNRmINtiIpS63523651;     shwepbOYGDNRmINtiIpS63523651 = shwepbOYGDNRmINtiIpS92572884;     shwepbOYGDNRmINtiIpS92572884 = shwepbOYGDNRmINtiIpS96477455;     shwepbOYGDNRmINtiIpS96477455 = shwepbOYGDNRmINtiIpS78702350;     shwepbOYGDNRmINtiIpS78702350 = shwepbOYGDNRmINtiIpS73861765;     shwepbOYGDNRmINtiIpS73861765 = shwepbOYGDNRmINtiIpS50534349;     shwepbOYGDNRmINtiIpS50534349 = shwepbOYGDNRmINtiIpS83683956;     shwepbOYGDNRmINtiIpS83683956 = shwepbOYGDNRmINtiIpS5827270;     shwepbOYGDNRmINtiIpS5827270 = shwepbOYGDNRmINtiIpS27107491;     shwepbOYGDNRmINtiIpS27107491 = shwepbOYGDNRmINtiIpS48483157;     shwepbOYGDNRmINtiIpS48483157 = shwepbOYGDNRmINtiIpS22547394;     shwepbOYGDNRmINtiIpS22547394 = shwepbOYGDNRmINtiIpS9506631;     shwepbOYGDNRmINtiIpS9506631 = shwepbOYGDNRmINtiIpS73479072;     shwepbOYGDNRmINtiIpS73479072 = shwepbOYGDNRmINtiIpS3258705;     shwepbOYGDNRmINtiIpS3258705 = shwepbOYGDNRmINtiIpS96920920;     shwepbOYGDNRmINtiIpS96920920 = shwepbOYGDNRmINtiIpS296835;     shwepbOYGDNRmINtiIpS296835 = shwepbOYGDNRmINtiIpS5223207;     shwepbOYGDNRmINtiIpS5223207 = shwepbOYGDNRmINtiIpS66669439;     shwepbOYGDNRmINtiIpS66669439 = shwepbOYGDNRmINtiIpS88235739;     shwepbOYGDNRmINtiIpS88235739 = shwepbOYGDNRmINtiIpS78947361;     shwepbOYGDNRmINtiIpS78947361 = shwepbOYGDNRmINtiIpS85119322;     shwepbOYGDNRmINtiIpS85119322 = shwepbOYGDNRmINtiIpS59329318;     shwepbOYGDNRmINtiIpS59329318 = shwepbOYGDNRmINtiIpS9667441;     shwepbOYGDNRmINtiIpS9667441 = shwepbOYGDNRmINtiIpS23951986;     shwepbOYGDNRmINtiIpS23951986 = shwepbOYGDNRmINtiIpS54263778;     shwepbOYGDNRmINtiIpS54263778 = shwepbOYGDNRmINtiIpS35435576;     shwepbOYGDNRmINtiIpS35435576 = shwepbOYGDNRmINtiIpS24264436;     shwepbOYGDNRmINtiIpS24264436 = shwepbOYGDNRmINtiIpS10908295;     shwepbOYGDNRmINtiIpS10908295 = shwepbOYGDNRmINtiIpS43636929;     shwepbOYGDNRmINtiIpS43636929 = shwepbOYGDNRmINtiIpS91050423;     shwepbOYGDNRmINtiIpS91050423 = shwepbOYGDNRmINtiIpS2464407;     shwepbOYGDNRmINtiIpS2464407 = shwepbOYGDNRmINtiIpS96103683;     shwepbOYGDNRmINtiIpS96103683 = shwepbOYGDNRmINtiIpS3227175;     shwepbOYGDNRmINtiIpS3227175 = shwepbOYGDNRmINtiIpS11071270;     shwepbOYGDNRmINtiIpS11071270 = shwepbOYGDNRmINtiIpS77261268;     shwepbOYGDNRmINtiIpS77261268 = shwepbOYGDNRmINtiIpS33873968;     shwepbOYGDNRmINtiIpS33873968 = shwepbOYGDNRmINtiIpS71963371;     shwepbOYGDNRmINtiIpS71963371 = shwepbOYGDNRmINtiIpS5371982;     shwepbOYGDNRmINtiIpS5371982 = shwepbOYGDNRmINtiIpS80836786;     shwepbOYGDNRmINtiIpS80836786 = shwepbOYGDNRmINtiIpS73291227;     shwepbOYGDNRmINtiIpS73291227 = shwepbOYGDNRmINtiIpS34209707;     shwepbOYGDNRmINtiIpS34209707 = shwepbOYGDNRmINtiIpS99855905;     shwepbOYGDNRmINtiIpS99855905 = shwepbOYGDNRmINtiIpS97689179;     shwepbOYGDNRmINtiIpS97689179 = shwepbOYGDNRmINtiIpS71190660;     shwepbOYGDNRmINtiIpS71190660 = shwepbOYGDNRmINtiIpS11092420;     shwepbOYGDNRmINtiIpS11092420 = shwepbOYGDNRmINtiIpS23952252;     shwepbOYGDNRmINtiIpS23952252 = shwepbOYGDNRmINtiIpS22142841;     shwepbOYGDNRmINtiIpS22142841 = shwepbOYGDNRmINtiIpS90085681;     shwepbOYGDNRmINtiIpS90085681 = shwepbOYGDNRmINtiIpS59466977;     shwepbOYGDNRmINtiIpS59466977 = shwepbOYGDNRmINtiIpS94827097;     shwepbOYGDNRmINtiIpS94827097 = shwepbOYGDNRmINtiIpS99026761;     shwepbOYGDNRmINtiIpS99026761 = shwepbOYGDNRmINtiIpS5548456;     shwepbOYGDNRmINtiIpS5548456 = shwepbOYGDNRmINtiIpS45517385;     shwepbOYGDNRmINtiIpS45517385 = shwepbOYGDNRmINtiIpS50169545;     shwepbOYGDNRmINtiIpS50169545 = shwepbOYGDNRmINtiIpS60770676;     shwepbOYGDNRmINtiIpS60770676 = shwepbOYGDNRmINtiIpS4194333;     shwepbOYGDNRmINtiIpS4194333 = shwepbOYGDNRmINtiIpS82905444;     shwepbOYGDNRmINtiIpS82905444 = shwepbOYGDNRmINtiIpS72525469;     shwepbOYGDNRmINtiIpS72525469 = shwepbOYGDNRmINtiIpS24438572;     shwepbOYGDNRmINtiIpS24438572 = shwepbOYGDNRmINtiIpS38426190;     shwepbOYGDNRmINtiIpS38426190 = shwepbOYGDNRmINtiIpS26269913;     shwepbOYGDNRmINtiIpS26269913 = shwepbOYGDNRmINtiIpS72775662;     shwepbOYGDNRmINtiIpS72775662 = shwepbOYGDNRmINtiIpS62190340;     shwepbOYGDNRmINtiIpS62190340 = shwepbOYGDNRmINtiIpS36057068;     shwepbOYGDNRmINtiIpS36057068 = shwepbOYGDNRmINtiIpS46018751;     shwepbOYGDNRmINtiIpS46018751 = shwepbOYGDNRmINtiIpS26443711;     shwepbOYGDNRmINtiIpS26443711 = shwepbOYGDNRmINtiIpS6279457;     shwepbOYGDNRmINtiIpS6279457 = shwepbOYGDNRmINtiIpS62407802;     shwepbOYGDNRmINtiIpS62407802 = shwepbOYGDNRmINtiIpS25997436;     shwepbOYGDNRmINtiIpS25997436 = shwepbOYGDNRmINtiIpS63046952;     shwepbOYGDNRmINtiIpS63046952 = shwepbOYGDNRmINtiIpS28333464;     shwepbOYGDNRmINtiIpS28333464 = shwepbOYGDNRmINtiIpS17608822;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void rVFNWvZLsxQUFdzCUwnv85473702() {     float KGAZfLVBfjSLFgYDSafG88260488 = -519785655;    float KGAZfLVBfjSLFgYDSafG35969709 = 72515878;    float KGAZfLVBfjSLFgYDSafG18729781 = -177904826;    float KGAZfLVBfjSLFgYDSafG35231679 = -698024184;    float KGAZfLVBfjSLFgYDSafG96132622 = -945508756;    float KGAZfLVBfjSLFgYDSafG19856570 = -23539618;    float KGAZfLVBfjSLFgYDSafG50722976 = -575531125;    float KGAZfLVBfjSLFgYDSafG13803649 = -105361858;    float KGAZfLVBfjSLFgYDSafG86649974 = -759920543;    float KGAZfLVBfjSLFgYDSafG49413313 = -54875269;    float KGAZfLVBfjSLFgYDSafG10689988 = -658935640;    float KGAZfLVBfjSLFgYDSafG29620903 = -749909297;    float KGAZfLVBfjSLFgYDSafG34525939 = -372628915;    float KGAZfLVBfjSLFgYDSafG7672609 = -888767936;    float KGAZfLVBfjSLFgYDSafG613683 = -83674537;    float KGAZfLVBfjSLFgYDSafG38211815 = -13534307;    float KGAZfLVBfjSLFgYDSafG18649497 = -325551203;    float KGAZfLVBfjSLFgYDSafG39967312 = -435859659;    float KGAZfLVBfjSLFgYDSafG15347595 = -903451655;    float KGAZfLVBfjSLFgYDSafG92245592 = -981712548;    float KGAZfLVBfjSLFgYDSafG50757668 = -914862218;    float KGAZfLVBfjSLFgYDSafG54740151 = -441017794;    float KGAZfLVBfjSLFgYDSafG58331706 = -1326582;    float KGAZfLVBfjSLFgYDSafG28973470 = -75166455;    float KGAZfLVBfjSLFgYDSafG68374357 = -130198;    float KGAZfLVBfjSLFgYDSafG2815475 = -683963229;    float KGAZfLVBfjSLFgYDSafG42013910 = -717224550;    float KGAZfLVBfjSLFgYDSafG36899132 = -128232763;    float KGAZfLVBfjSLFgYDSafG78032486 = -363222422;    float KGAZfLVBfjSLFgYDSafG2695748 = -263994318;    float KGAZfLVBfjSLFgYDSafG99644881 = -91348261;    float KGAZfLVBfjSLFgYDSafG40988916 = -623326459;    float KGAZfLVBfjSLFgYDSafG61647711 = -986341339;    float KGAZfLVBfjSLFgYDSafG39226740 = -932099709;    float KGAZfLVBfjSLFgYDSafG28558585 = -856015874;    float KGAZfLVBfjSLFgYDSafG43677216 = -862352721;    float KGAZfLVBfjSLFgYDSafG49618755 = -852291606;    float KGAZfLVBfjSLFgYDSafG79899979 = -724362321;    float KGAZfLVBfjSLFgYDSafG27016446 = -125148797;    float KGAZfLVBfjSLFgYDSafG17650692 = -49512234;    float KGAZfLVBfjSLFgYDSafG69579915 = -895303577;    float KGAZfLVBfjSLFgYDSafG1962064 = -113064141;    float KGAZfLVBfjSLFgYDSafG52225664 = 48643450;    float KGAZfLVBfjSLFgYDSafG3429221 = -795506967;    float KGAZfLVBfjSLFgYDSafG96250893 = -295643641;    float KGAZfLVBfjSLFgYDSafG11629042 = -523659370;    float KGAZfLVBfjSLFgYDSafG92800642 = -968884202;    float KGAZfLVBfjSLFgYDSafG53849949 = -967762826;    float KGAZfLVBfjSLFgYDSafG84388813 = -445633180;    float KGAZfLVBfjSLFgYDSafG93546809 = -448577344;    float KGAZfLVBfjSLFgYDSafG27518643 = 38867972;    float KGAZfLVBfjSLFgYDSafG51538945 = -836040435;    float KGAZfLVBfjSLFgYDSafG79027643 = -345562534;    float KGAZfLVBfjSLFgYDSafG44491083 = -428018777;    float KGAZfLVBfjSLFgYDSafG34716035 = -793089739;    float KGAZfLVBfjSLFgYDSafG33520338 = 21232138;    float KGAZfLVBfjSLFgYDSafG77638003 = -926157541;    float KGAZfLVBfjSLFgYDSafG89756311 = -2738372;    float KGAZfLVBfjSLFgYDSafG66857321 = -597893986;    float KGAZfLVBfjSLFgYDSafG93317147 = -161545527;    float KGAZfLVBfjSLFgYDSafG77842660 = -306315069;    float KGAZfLVBfjSLFgYDSafG13823845 = -347298363;    float KGAZfLVBfjSLFgYDSafG35771163 = -742139436;    float KGAZfLVBfjSLFgYDSafG83954226 = -395926225;    float KGAZfLVBfjSLFgYDSafG49768432 = -963527009;    float KGAZfLVBfjSLFgYDSafG69701071 = 64390819;    float KGAZfLVBfjSLFgYDSafG67973192 = -763567959;    float KGAZfLVBfjSLFgYDSafG95299198 = -440529206;    float KGAZfLVBfjSLFgYDSafG79114024 = 67247938;    float KGAZfLVBfjSLFgYDSafG56936467 = -221321817;    float KGAZfLVBfjSLFgYDSafG88593059 = -161242701;    float KGAZfLVBfjSLFgYDSafG38749517 = -601188883;    float KGAZfLVBfjSLFgYDSafG12950866 = -210710862;    float KGAZfLVBfjSLFgYDSafG97696902 = -753939421;    float KGAZfLVBfjSLFgYDSafG22665678 = 13591028;    float KGAZfLVBfjSLFgYDSafG48795604 = -701798077;    float KGAZfLVBfjSLFgYDSafG2514487 = -389661245;    float KGAZfLVBfjSLFgYDSafG54902485 = -205819615;    float KGAZfLVBfjSLFgYDSafG32722576 = -779522814;    float KGAZfLVBfjSLFgYDSafG56745315 = -476470829;    float KGAZfLVBfjSLFgYDSafG10014833 = -715079028;    float KGAZfLVBfjSLFgYDSafG88163960 = -749461725;    float KGAZfLVBfjSLFgYDSafG52510318 = -682599584;    float KGAZfLVBfjSLFgYDSafG84485677 = -914645078;    float KGAZfLVBfjSLFgYDSafG75177104 = -202862290;    float KGAZfLVBfjSLFgYDSafG48105936 = -255307827;    float KGAZfLVBfjSLFgYDSafG61961273 = -177763925;    float KGAZfLVBfjSLFgYDSafG17156628 = -458322562;    float KGAZfLVBfjSLFgYDSafG4510706 = -39009971;    float KGAZfLVBfjSLFgYDSafG95038247 = -777248013;    float KGAZfLVBfjSLFgYDSafG66039212 = -936195181;    float KGAZfLVBfjSLFgYDSafG59862444 = -749553234;    float KGAZfLVBfjSLFgYDSafG13042658 = -26468335;    float KGAZfLVBfjSLFgYDSafG33699299 = -963603270;    float KGAZfLVBfjSLFgYDSafG39808032 = -743197166;    float KGAZfLVBfjSLFgYDSafG55756070 = -448005214;    float KGAZfLVBfjSLFgYDSafG66190901 = -370924705;    float KGAZfLVBfjSLFgYDSafG68271437 = -555430325;    float KGAZfLVBfjSLFgYDSafG53660789 = -831979959;    float KGAZfLVBfjSLFgYDSafG26549822 = -519785655;     KGAZfLVBfjSLFgYDSafG88260488 = KGAZfLVBfjSLFgYDSafG35969709;     KGAZfLVBfjSLFgYDSafG35969709 = KGAZfLVBfjSLFgYDSafG18729781;     KGAZfLVBfjSLFgYDSafG18729781 = KGAZfLVBfjSLFgYDSafG35231679;     KGAZfLVBfjSLFgYDSafG35231679 = KGAZfLVBfjSLFgYDSafG96132622;     KGAZfLVBfjSLFgYDSafG96132622 = KGAZfLVBfjSLFgYDSafG19856570;     KGAZfLVBfjSLFgYDSafG19856570 = KGAZfLVBfjSLFgYDSafG50722976;     KGAZfLVBfjSLFgYDSafG50722976 = KGAZfLVBfjSLFgYDSafG13803649;     KGAZfLVBfjSLFgYDSafG13803649 = KGAZfLVBfjSLFgYDSafG86649974;     KGAZfLVBfjSLFgYDSafG86649974 = KGAZfLVBfjSLFgYDSafG49413313;     KGAZfLVBfjSLFgYDSafG49413313 = KGAZfLVBfjSLFgYDSafG10689988;     KGAZfLVBfjSLFgYDSafG10689988 = KGAZfLVBfjSLFgYDSafG29620903;     KGAZfLVBfjSLFgYDSafG29620903 = KGAZfLVBfjSLFgYDSafG34525939;     KGAZfLVBfjSLFgYDSafG34525939 = KGAZfLVBfjSLFgYDSafG7672609;     KGAZfLVBfjSLFgYDSafG7672609 = KGAZfLVBfjSLFgYDSafG613683;     KGAZfLVBfjSLFgYDSafG613683 = KGAZfLVBfjSLFgYDSafG38211815;     KGAZfLVBfjSLFgYDSafG38211815 = KGAZfLVBfjSLFgYDSafG18649497;     KGAZfLVBfjSLFgYDSafG18649497 = KGAZfLVBfjSLFgYDSafG39967312;     KGAZfLVBfjSLFgYDSafG39967312 = KGAZfLVBfjSLFgYDSafG15347595;     KGAZfLVBfjSLFgYDSafG15347595 = KGAZfLVBfjSLFgYDSafG92245592;     KGAZfLVBfjSLFgYDSafG92245592 = KGAZfLVBfjSLFgYDSafG50757668;     KGAZfLVBfjSLFgYDSafG50757668 = KGAZfLVBfjSLFgYDSafG54740151;     KGAZfLVBfjSLFgYDSafG54740151 = KGAZfLVBfjSLFgYDSafG58331706;     KGAZfLVBfjSLFgYDSafG58331706 = KGAZfLVBfjSLFgYDSafG28973470;     KGAZfLVBfjSLFgYDSafG28973470 = KGAZfLVBfjSLFgYDSafG68374357;     KGAZfLVBfjSLFgYDSafG68374357 = KGAZfLVBfjSLFgYDSafG2815475;     KGAZfLVBfjSLFgYDSafG2815475 = KGAZfLVBfjSLFgYDSafG42013910;     KGAZfLVBfjSLFgYDSafG42013910 = KGAZfLVBfjSLFgYDSafG36899132;     KGAZfLVBfjSLFgYDSafG36899132 = KGAZfLVBfjSLFgYDSafG78032486;     KGAZfLVBfjSLFgYDSafG78032486 = KGAZfLVBfjSLFgYDSafG2695748;     KGAZfLVBfjSLFgYDSafG2695748 = KGAZfLVBfjSLFgYDSafG99644881;     KGAZfLVBfjSLFgYDSafG99644881 = KGAZfLVBfjSLFgYDSafG40988916;     KGAZfLVBfjSLFgYDSafG40988916 = KGAZfLVBfjSLFgYDSafG61647711;     KGAZfLVBfjSLFgYDSafG61647711 = KGAZfLVBfjSLFgYDSafG39226740;     KGAZfLVBfjSLFgYDSafG39226740 = KGAZfLVBfjSLFgYDSafG28558585;     KGAZfLVBfjSLFgYDSafG28558585 = KGAZfLVBfjSLFgYDSafG43677216;     KGAZfLVBfjSLFgYDSafG43677216 = KGAZfLVBfjSLFgYDSafG49618755;     KGAZfLVBfjSLFgYDSafG49618755 = KGAZfLVBfjSLFgYDSafG79899979;     KGAZfLVBfjSLFgYDSafG79899979 = KGAZfLVBfjSLFgYDSafG27016446;     KGAZfLVBfjSLFgYDSafG27016446 = KGAZfLVBfjSLFgYDSafG17650692;     KGAZfLVBfjSLFgYDSafG17650692 = KGAZfLVBfjSLFgYDSafG69579915;     KGAZfLVBfjSLFgYDSafG69579915 = KGAZfLVBfjSLFgYDSafG1962064;     KGAZfLVBfjSLFgYDSafG1962064 = KGAZfLVBfjSLFgYDSafG52225664;     KGAZfLVBfjSLFgYDSafG52225664 = KGAZfLVBfjSLFgYDSafG3429221;     KGAZfLVBfjSLFgYDSafG3429221 = KGAZfLVBfjSLFgYDSafG96250893;     KGAZfLVBfjSLFgYDSafG96250893 = KGAZfLVBfjSLFgYDSafG11629042;     KGAZfLVBfjSLFgYDSafG11629042 = KGAZfLVBfjSLFgYDSafG92800642;     KGAZfLVBfjSLFgYDSafG92800642 = KGAZfLVBfjSLFgYDSafG53849949;     KGAZfLVBfjSLFgYDSafG53849949 = KGAZfLVBfjSLFgYDSafG84388813;     KGAZfLVBfjSLFgYDSafG84388813 = KGAZfLVBfjSLFgYDSafG93546809;     KGAZfLVBfjSLFgYDSafG93546809 = KGAZfLVBfjSLFgYDSafG27518643;     KGAZfLVBfjSLFgYDSafG27518643 = KGAZfLVBfjSLFgYDSafG51538945;     KGAZfLVBfjSLFgYDSafG51538945 = KGAZfLVBfjSLFgYDSafG79027643;     KGAZfLVBfjSLFgYDSafG79027643 = KGAZfLVBfjSLFgYDSafG44491083;     KGAZfLVBfjSLFgYDSafG44491083 = KGAZfLVBfjSLFgYDSafG34716035;     KGAZfLVBfjSLFgYDSafG34716035 = KGAZfLVBfjSLFgYDSafG33520338;     KGAZfLVBfjSLFgYDSafG33520338 = KGAZfLVBfjSLFgYDSafG77638003;     KGAZfLVBfjSLFgYDSafG77638003 = KGAZfLVBfjSLFgYDSafG89756311;     KGAZfLVBfjSLFgYDSafG89756311 = KGAZfLVBfjSLFgYDSafG66857321;     KGAZfLVBfjSLFgYDSafG66857321 = KGAZfLVBfjSLFgYDSafG93317147;     KGAZfLVBfjSLFgYDSafG93317147 = KGAZfLVBfjSLFgYDSafG77842660;     KGAZfLVBfjSLFgYDSafG77842660 = KGAZfLVBfjSLFgYDSafG13823845;     KGAZfLVBfjSLFgYDSafG13823845 = KGAZfLVBfjSLFgYDSafG35771163;     KGAZfLVBfjSLFgYDSafG35771163 = KGAZfLVBfjSLFgYDSafG83954226;     KGAZfLVBfjSLFgYDSafG83954226 = KGAZfLVBfjSLFgYDSafG49768432;     KGAZfLVBfjSLFgYDSafG49768432 = KGAZfLVBfjSLFgYDSafG69701071;     KGAZfLVBfjSLFgYDSafG69701071 = KGAZfLVBfjSLFgYDSafG67973192;     KGAZfLVBfjSLFgYDSafG67973192 = KGAZfLVBfjSLFgYDSafG95299198;     KGAZfLVBfjSLFgYDSafG95299198 = KGAZfLVBfjSLFgYDSafG79114024;     KGAZfLVBfjSLFgYDSafG79114024 = KGAZfLVBfjSLFgYDSafG56936467;     KGAZfLVBfjSLFgYDSafG56936467 = KGAZfLVBfjSLFgYDSafG88593059;     KGAZfLVBfjSLFgYDSafG88593059 = KGAZfLVBfjSLFgYDSafG38749517;     KGAZfLVBfjSLFgYDSafG38749517 = KGAZfLVBfjSLFgYDSafG12950866;     KGAZfLVBfjSLFgYDSafG12950866 = KGAZfLVBfjSLFgYDSafG97696902;     KGAZfLVBfjSLFgYDSafG97696902 = KGAZfLVBfjSLFgYDSafG22665678;     KGAZfLVBfjSLFgYDSafG22665678 = KGAZfLVBfjSLFgYDSafG48795604;     KGAZfLVBfjSLFgYDSafG48795604 = KGAZfLVBfjSLFgYDSafG2514487;     KGAZfLVBfjSLFgYDSafG2514487 = KGAZfLVBfjSLFgYDSafG54902485;     KGAZfLVBfjSLFgYDSafG54902485 = KGAZfLVBfjSLFgYDSafG32722576;     KGAZfLVBfjSLFgYDSafG32722576 = KGAZfLVBfjSLFgYDSafG56745315;     KGAZfLVBfjSLFgYDSafG56745315 = KGAZfLVBfjSLFgYDSafG10014833;     KGAZfLVBfjSLFgYDSafG10014833 = KGAZfLVBfjSLFgYDSafG88163960;     KGAZfLVBfjSLFgYDSafG88163960 = KGAZfLVBfjSLFgYDSafG52510318;     KGAZfLVBfjSLFgYDSafG52510318 = KGAZfLVBfjSLFgYDSafG84485677;     KGAZfLVBfjSLFgYDSafG84485677 = KGAZfLVBfjSLFgYDSafG75177104;     KGAZfLVBfjSLFgYDSafG75177104 = KGAZfLVBfjSLFgYDSafG48105936;     KGAZfLVBfjSLFgYDSafG48105936 = KGAZfLVBfjSLFgYDSafG61961273;     KGAZfLVBfjSLFgYDSafG61961273 = KGAZfLVBfjSLFgYDSafG17156628;     KGAZfLVBfjSLFgYDSafG17156628 = KGAZfLVBfjSLFgYDSafG4510706;     KGAZfLVBfjSLFgYDSafG4510706 = KGAZfLVBfjSLFgYDSafG95038247;     KGAZfLVBfjSLFgYDSafG95038247 = KGAZfLVBfjSLFgYDSafG66039212;     KGAZfLVBfjSLFgYDSafG66039212 = KGAZfLVBfjSLFgYDSafG59862444;     KGAZfLVBfjSLFgYDSafG59862444 = KGAZfLVBfjSLFgYDSafG13042658;     KGAZfLVBfjSLFgYDSafG13042658 = KGAZfLVBfjSLFgYDSafG33699299;     KGAZfLVBfjSLFgYDSafG33699299 = KGAZfLVBfjSLFgYDSafG39808032;     KGAZfLVBfjSLFgYDSafG39808032 = KGAZfLVBfjSLFgYDSafG55756070;     KGAZfLVBfjSLFgYDSafG55756070 = KGAZfLVBfjSLFgYDSafG66190901;     KGAZfLVBfjSLFgYDSafG66190901 = KGAZfLVBfjSLFgYDSafG68271437;     KGAZfLVBfjSLFgYDSafG68271437 = KGAZfLVBfjSLFgYDSafG53660789;     KGAZfLVBfjSLFgYDSafG53660789 = KGAZfLVBfjSLFgYDSafG26549822;     KGAZfLVBfjSLFgYDSafG26549822 = KGAZfLVBfjSLFgYDSafG88260488;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void SzZpYVYZpTNtTKemPrMh92132446() {     float qbgYyuBpAOGFlqEMvMcP88254017 = -392309797;    float qbgYyuBpAOGFlqEMvMcP74542026 = 29920753;    float qbgYyuBpAOGFlqEMvMcP20042993 = -135033440;    float qbgYyuBpAOGFlqEMvMcP20311540 = -518565014;    float qbgYyuBpAOGFlqEMvMcP4972725 = -313864078;    float qbgYyuBpAOGFlqEMvMcP54662002 = -822972430;    float qbgYyuBpAOGFlqEMvMcP19270313 = -280067592;    float qbgYyuBpAOGFlqEMvMcP83017699 = -909539175;    float qbgYyuBpAOGFlqEMvMcP69390138 = -473566612;    float qbgYyuBpAOGFlqEMvMcP46052827 = -791682140;    float qbgYyuBpAOGFlqEMvMcP31010593 = -618407712;    float qbgYyuBpAOGFlqEMvMcP43912580 = -485387660;    float qbgYyuBpAOGFlqEMvMcP66378852 = -394287389;    float qbgYyuBpAOGFlqEMvMcP61516829 = -133920988;    float qbgYyuBpAOGFlqEMvMcP45785792 = -46387389;    float qbgYyuBpAOGFlqEMvMcP12776362 = -346329957;    float qbgYyuBpAOGFlqEMvMcP74502256 = -145165773;    float qbgYyuBpAOGFlqEMvMcP62967844 = -832931280;    float qbgYyuBpAOGFlqEMvMcP35775865 = -421433921;    float qbgYyuBpAOGFlqEMvMcP64319355 = -493737000;    float qbgYyuBpAOGFlqEMvMcP30427092 = -625328591;    float qbgYyuBpAOGFlqEMvMcP33036417 = -951009678;    float qbgYyuBpAOGFlqEMvMcP95210102 = -460284424;    float qbgYyuBpAOGFlqEMvMcP72623578 = 43913023;    float qbgYyuBpAOGFlqEMvMcP96817046 = -426824052;    float qbgYyuBpAOGFlqEMvMcP19389552 = -592169158;    float qbgYyuBpAOGFlqEMvMcP12235492 = 81975737;    float qbgYyuBpAOGFlqEMvMcP72302656 = -737199751;    float qbgYyuBpAOGFlqEMvMcP90710933 = 90707263;    float qbgYyuBpAOGFlqEMvMcP78785617 = 60574309;    float qbgYyuBpAOGFlqEMvMcP33057560 = -478441496;    float qbgYyuBpAOGFlqEMvMcP25712773 = 84851779;    float qbgYyuBpAOGFlqEMvMcP47100134 = -88285555;    float qbgYyuBpAOGFlqEMvMcP39509578 = -808781441;    float qbgYyuBpAOGFlqEMvMcP10916623 = -15329799;    float qbgYyuBpAOGFlqEMvMcP86505405 = -942118467;    float qbgYyuBpAOGFlqEMvMcP61560123 = -951944984;    float qbgYyuBpAOGFlqEMvMcP49817659 = -719625000;    float qbgYyuBpAOGFlqEMvMcP62330043 = -837830213;    float qbgYyuBpAOGFlqEMvMcP89523584 = -758943920;    float qbgYyuBpAOGFlqEMvMcP57435582 = -983247475;    float qbgYyuBpAOGFlqEMvMcP87659724 = -306856223;    float qbgYyuBpAOGFlqEMvMcP12509025 = -510486804;    float qbgYyuBpAOGFlqEMvMcP62661119 = -352471410;    float qbgYyuBpAOGFlqEMvMcP80268946 = -768903043;    float qbgYyuBpAOGFlqEMvMcP52057010 = -335704041;    float qbgYyuBpAOGFlqEMvMcP20498435 = -350164824;    float qbgYyuBpAOGFlqEMvMcP29233530 = -476524182;    float qbgYyuBpAOGFlqEMvMcP3047204 = -9397041;    float qbgYyuBpAOGFlqEMvMcP13592232 = -35602231;    float qbgYyuBpAOGFlqEMvMcP40778013 = -639687036;    float qbgYyuBpAOGFlqEMvMcP66086202 = -118186183;    float qbgYyuBpAOGFlqEMvMcP54475991 = -237616542;    float qbgYyuBpAOGFlqEMvMcP54741354 = -959938629;    float qbgYyuBpAOGFlqEMvMcP61705355 = -549989017;    float qbgYyuBpAOGFlqEMvMcP55217600 = -441300120;    float qbgYyuBpAOGFlqEMvMcP79331923 = -509794824;    float qbgYyuBpAOGFlqEMvMcP47419415 = -78946463;    float qbgYyuBpAOGFlqEMvMcP23494493 = 8259037;    float qbgYyuBpAOGFlqEMvMcP85583172 = -721694921;    float qbgYyuBpAOGFlqEMvMcP42426511 = -804948168;    float qbgYyuBpAOGFlqEMvMcP46967656 = -542867842;    float qbgYyuBpAOGFlqEMvMcP92306766 = -900246438;    float qbgYyuBpAOGFlqEMvMcP90604521 = -434140922;    float qbgYyuBpAOGFlqEMvMcP12995267 = -213240645;    float qbgYyuBpAOGFlqEMvMcP5297820 = -603259492;    float qbgYyuBpAOGFlqEMvMcP96812446 = -297102106;    float qbgYyuBpAOGFlqEMvMcP26869275 = -585505948;    float qbgYyuBpAOGFlqEMvMcP50600206 = -18591189;    float qbgYyuBpAOGFlqEMvMcP59280386 = -104268923;    float qbgYyuBpAOGFlqEMvMcP51216239 = -394384974;    float qbgYyuBpAOGFlqEMvMcP24684597 = -425540773;    float qbgYyuBpAOGFlqEMvMcP637802 = -995101068;    float qbgYyuBpAOGFlqEMvMcP46252280 = -662490002;    float qbgYyuBpAOGFlqEMvMcP6883773 = -510489525;    float qbgYyuBpAOGFlqEMvMcP42767368 = -218472369;    float qbgYyuBpAOGFlqEMvMcP20527393 = -340522874;    float qbgYyuBpAOGFlqEMvMcP32548984 = -7813014;    float qbgYyuBpAOGFlqEMvMcP92354632 = -187183935;    float qbgYyuBpAOGFlqEMvMcP44760037 = 8879989;    float qbgYyuBpAOGFlqEMvMcP98891117 = -142004334;    float qbgYyuBpAOGFlqEMvMcP83001961 = -441500082;    float qbgYyuBpAOGFlqEMvMcP69255453 = -627802711;    float qbgYyuBpAOGFlqEMvMcP77118701 = -873690507;    float qbgYyuBpAOGFlqEMvMcP38007605 = -299738656;    float qbgYyuBpAOGFlqEMvMcP66971357 = -260255313;    float qbgYyuBpAOGFlqEMvMcP71236782 = -677531679;    float qbgYyuBpAOGFlqEMvMcP92358780 = -128346926;    float qbgYyuBpAOGFlqEMvMcP77804222 = -158792425;    float qbgYyuBpAOGFlqEMvMcP55699022 = -574029680;    float qbgYyuBpAOGFlqEMvMcP7173482 = -332323644;    float qbgYyuBpAOGFlqEMvMcP14140708 = -772998521;    float qbgYyuBpAOGFlqEMvMcP26323167 = -627884037;    float qbgYyuBpAOGFlqEMvMcP76746870 = -16135293;    float qbgYyuBpAOGFlqEMvMcP47097074 = -953995753;    float qbgYyuBpAOGFlqEMvMcP10467927 = -340379634;    float qbgYyuBpAOGFlqEMvMcP95352958 = -406609785;    float qbgYyuBpAOGFlqEMvMcP21904504 = 23654117;    float qbgYyuBpAOGFlqEMvMcP49665852 = -39230766;    float qbgYyuBpAOGFlqEMvMcP74971126 = -392309797;     qbgYyuBpAOGFlqEMvMcP88254017 = qbgYyuBpAOGFlqEMvMcP74542026;     qbgYyuBpAOGFlqEMvMcP74542026 = qbgYyuBpAOGFlqEMvMcP20042993;     qbgYyuBpAOGFlqEMvMcP20042993 = qbgYyuBpAOGFlqEMvMcP20311540;     qbgYyuBpAOGFlqEMvMcP20311540 = qbgYyuBpAOGFlqEMvMcP4972725;     qbgYyuBpAOGFlqEMvMcP4972725 = qbgYyuBpAOGFlqEMvMcP54662002;     qbgYyuBpAOGFlqEMvMcP54662002 = qbgYyuBpAOGFlqEMvMcP19270313;     qbgYyuBpAOGFlqEMvMcP19270313 = qbgYyuBpAOGFlqEMvMcP83017699;     qbgYyuBpAOGFlqEMvMcP83017699 = qbgYyuBpAOGFlqEMvMcP69390138;     qbgYyuBpAOGFlqEMvMcP69390138 = qbgYyuBpAOGFlqEMvMcP46052827;     qbgYyuBpAOGFlqEMvMcP46052827 = qbgYyuBpAOGFlqEMvMcP31010593;     qbgYyuBpAOGFlqEMvMcP31010593 = qbgYyuBpAOGFlqEMvMcP43912580;     qbgYyuBpAOGFlqEMvMcP43912580 = qbgYyuBpAOGFlqEMvMcP66378852;     qbgYyuBpAOGFlqEMvMcP66378852 = qbgYyuBpAOGFlqEMvMcP61516829;     qbgYyuBpAOGFlqEMvMcP61516829 = qbgYyuBpAOGFlqEMvMcP45785792;     qbgYyuBpAOGFlqEMvMcP45785792 = qbgYyuBpAOGFlqEMvMcP12776362;     qbgYyuBpAOGFlqEMvMcP12776362 = qbgYyuBpAOGFlqEMvMcP74502256;     qbgYyuBpAOGFlqEMvMcP74502256 = qbgYyuBpAOGFlqEMvMcP62967844;     qbgYyuBpAOGFlqEMvMcP62967844 = qbgYyuBpAOGFlqEMvMcP35775865;     qbgYyuBpAOGFlqEMvMcP35775865 = qbgYyuBpAOGFlqEMvMcP64319355;     qbgYyuBpAOGFlqEMvMcP64319355 = qbgYyuBpAOGFlqEMvMcP30427092;     qbgYyuBpAOGFlqEMvMcP30427092 = qbgYyuBpAOGFlqEMvMcP33036417;     qbgYyuBpAOGFlqEMvMcP33036417 = qbgYyuBpAOGFlqEMvMcP95210102;     qbgYyuBpAOGFlqEMvMcP95210102 = qbgYyuBpAOGFlqEMvMcP72623578;     qbgYyuBpAOGFlqEMvMcP72623578 = qbgYyuBpAOGFlqEMvMcP96817046;     qbgYyuBpAOGFlqEMvMcP96817046 = qbgYyuBpAOGFlqEMvMcP19389552;     qbgYyuBpAOGFlqEMvMcP19389552 = qbgYyuBpAOGFlqEMvMcP12235492;     qbgYyuBpAOGFlqEMvMcP12235492 = qbgYyuBpAOGFlqEMvMcP72302656;     qbgYyuBpAOGFlqEMvMcP72302656 = qbgYyuBpAOGFlqEMvMcP90710933;     qbgYyuBpAOGFlqEMvMcP90710933 = qbgYyuBpAOGFlqEMvMcP78785617;     qbgYyuBpAOGFlqEMvMcP78785617 = qbgYyuBpAOGFlqEMvMcP33057560;     qbgYyuBpAOGFlqEMvMcP33057560 = qbgYyuBpAOGFlqEMvMcP25712773;     qbgYyuBpAOGFlqEMvMcP25712773 = qbgYyuBpAOGFlqEMvMcP47100134;     qbgYyuBpAOGFlqEMvMcP47100134 = qbgYyuBpAOGFlqEMvMcP39509578;     qbgYyuBpAOGFlqEMvMcP39509578 = qbgYyuBpAOGFlqEMvMcP10916623;     qbgYyuBpAOGFlqEMvMcP10916623 = qbgYyuBpAOGFlqEMvMcP86505405;     qbgYyuBpAOGFlqEMvMcP86505405 = qbgYyuBpAOGFlqEMvMcP61560123;     qbgYyuBpAOGFlqEMvMcP61560123 = qbgYyuBpAOGFlqEMvMcP49817659;     qbgYyuBpAOGFlqEMvMcP49817659 = qbgYyuBpAOGFlqEMvMcP62330043;     qbgYyuBpAOGFlqEMvMcP62330043 = qbgYyuBpAOGFlqEMvMcP89523584;     qbgYyuBpAOGFlqEMvMcP89523584 = qbgYyuBpAOGFlqEMvMcP57435582;     qbgYyuBpAOGFlqEMvMcP57435582 = qbgYyuBpAOGFlqEMvMcP87659724;     qbgYyuBpAOGFlqEMvMcP87659724 = qbgYyuBpAOGFlqEMvMcP12509025;     qbgYyuBpAOGFlqEMvMcP12509025 = qbgYyuBpAOGFlqEMvMcP62661119;     qbgYyuBpAOGFlqEMvMcP62661119 = qbgYyuBpAOGFlqEMvMcP80268946;     qbgYyuBpAOGFlqEMvMcP80268946 = qbgYyuBpAOGFlqEMvMcP52057010;     qbgYyuBpAOGFlqEMvMcP52057010 = qbgYyuBpAOGFlqEMvMcP20498435;     qbgYyuBpAOGFlqEMvMcP20498435 = qbgYyuBpAOGFlqEMvMcP29233530;     qbgYyuBpAOGFlqEMvMcP29233530 = qbgYyuBpAOGFlqEMvMcP3047204;     qbgYyuBpAOGFlqEMvMcP3047204 = qbgYyuBpAOGFlqEMvMcP13592232;     qbgYyuBpAOGFlqEMvMcP13592232 = qbgYyuBpAOGFlqEMvMcP40778013;     qbgYyuBpAOGFlqEMvMcP40778013 = qbgYyuBpAOGFlqEMvMcP66086202;     qbgYyuBpAOGFlqEMvMcP66086202 = qbgYyuBpAOGFlqEMvMcP54475991;     qbgYyuBpAOGFlqEMvMcP54475991 = qbgYyuBpAOGFlqEMvMcP54741354;     qbgYyuBpAOGFlqEMvMcP54741354 = qbgYyuBpAOGFlqEMvMcP61705355;     qbgYyuBpAOGFlqEMvMcP61705355 = qbgYyuBpAOGFlqEMvMcP55217600;     qbgYyuBpAOGFlqEMvMcP55217600 = qbgYyuBpAOGFlqEMvMcP79331923;     qbgYyuBpAOGFlqEMvMcP79331923 = qbgYyuBpAOGFlqEMvMcP47419415;     qbgYyuBpAOGFlqEMvMcP47419415 = qbgYyuBpAOGFlqEMvMcP23494493;     qbgYyuBpAOGFlqEMvMcP23494493 = qbgYyuBpAOGFlqEMvMcP85583172;     qbgYyuBpAOGFlqEMvMcP85583172 = qbgYyuBpAOGFlqEMvMcP42426511;     qbgYyuBpAOGFlqEMvMcP42426511 = qbgYyuBpAOGFlqEMvMcP46967656;     qbgYyuBpAOGFlqEMvMcP46967656 = qbgYyuBpAOGFlqEMvMcP92306766;     qbgYyuBpAOGFlqEMvMcP92306766 = qbgYyuBpAOGFlqEMvMcP90604521;     qbgYyuBpAOGFlqEMvMcP90604521 = qbgYyuBpAOGFlqEMvMcP12995267;     qbgYyuBpAOGFlqEMvMcP12995267 = qbgYyuBpAOGFlqEMvMcP5297820;     qbgYyuBpAOGFlqEMvMcP5297820 = qbgYyuBpAOGFlqEMvMcP96812446;     qbgYyuBpAOGFlqEMvMcP96812446 = qbgYyuBpAOGFlqEMvMcP26869275;     qbgYyuBpAOGFlqEMvMcP26869275 = qbgYyuBpAOGFlqEMvMcP50600206;     qbgYyuBpAOGFlqEMvMcP50600206 = qbgYyuBpAOGFlqEMvMcP59280386;     qbgYyuBpAOGFlqEMvMcP59280386 = qbgYyuBpAOGFlqEMvMcP51216239;     qbgYyuBpAOGFlqEMvMcP51216239 = qbgYyuBpAOGFlqEMvMcP24684597;     qbgYyuBpAOGFlqEMvMcP24684597 = qbgYyuBpAOGFlqEMvMcP637802;     qbgYyuBpAOGFlqEMvMcP637802 = qbgYyuBpAOGFlqEMvMcP46252280;     qbgYyuBpAOGFlqEMvMcP46252280 = qbgYyuBpAOGFlqEMvMcP6883773;     qbgYyuBpAOGFlqEMvMcP6883773 = qbgYyuBpAOGFlqEMvMcP42767368;     qbgYyuBpAOGFlqEMvMcP42767368 = qbgYyuBpAOGFlqEMvMcP20527393;     qbgYyuBpAOGFlqEMvMcP20527393 = qbgYyuBpAOGFlqEMvMcP32548984;     qbgYyuBpAOGFlqEMvMcP32548984 = qbgYyuBpAOGFlqEMvMcP92354632;     qbgYyuBpAOGFlqEMvMcP92354632 = qbgYyuBpAOGFlqEMvMcP44760037;     qbgYyuBpAOGFlqEMvMcP44760037 = qbgYyuBpAOGFlqEMvMcP98891117;     qbgYyuBpAOGFlqEMvMcP98891117 = qbgYyuBpAOGFlqEMvMcP83001961;     qbgYyuBpAOGFlqEMvMcP83001961 = qbgYyuBpAOGFlqEMvMcP69255453;     qbgYyuBpAOGFlqEMvMcP69255453 = qbgYyuBpAOGFlqEMvMcP77118701;     qbgYyuBpAOGFlqEMvMcP77118701 = qbgYyuBpAOGFlqEMvMcP38007605;     qbgYyuBpAOGFlqEMvMcP38007605 = qbgYyuBpAOGFlqEMvMcP66971357;     qbgYyuBpAOGFlqEMvMcP66971357 = qbgYyuBpAOGFlqEMvMcP71236782;     qbgYyuBpAOGFlqEMvMcP71236782 = qbgYyuBpAOGFlqEMvMcP92358780;     qbgYyuBpAOGFlqEMvMcP92358780 = qbgYyuBpAOGFlqEMvMcP77804222;     qbgYyuBpAOGFlqEMvMcP77804222 = qbgYyuBpAOGFlqEMvMcP55699022;     qbgYyuBpAOGFlqEMvMcP55699022 = qbgYyuBpAOGFlqEMvMcP7173482;     qbgYyuBpAOGFlqEMvMcP7173482 = qbgYyuBpAOGFlqEMvMcP14140708;     qbgYyuBpAOGFlqEMvMcP14140708 = qbgYyuBpAOGFlqEMvMcP26323167;     qbgYyuBpAOGFlqEMvMcP26323167 = qbgYyuBpAOGFlqEMvMcP76746870;     qbgYyuBpAOGFlqEMvMcP76746870 = qbgYyuBpAOGFlqEMvMcP47097074;     qbgYyuBpAOGFlqEMvMcP47097074 = qbgYyuBpAOGFlqEMvMcP10467927;     qbgYyuBpAOGFlqEMvMcP10467927 = qbgYyuBpAOGFlqEMvMcP95352958;     qbgYyuBpAOGFlqEMvMcP95352958 = qbgYyuBpAOGFlqEMvMcP21904504;     qbgYyuBpAOGFlqEMvMcP21904504 = qbgYyuBpAOGFlqEMvMcP49665852;     qbgYyuBpAOGFlqEMvMcP49665852 = qbgYyuBpAOGFlqEMvMcP74971126;     qbgYyuBpAOGFlqEMvMcP74971126 = qbgYyuBpAOGFlqEMvMcP88254017;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DvLGpvmEEMUWvmOFWQoB46548660() {     float VBQIzmjdGIiFGOAKKYbL58905684 = 89541073;    float VBQIzmjdGIiFGOAKKYbL43215543 = -941722546;    float VBQIzmjdGIiFGOAKKYbL11912 = -577327900;    float VBQIzmjdGIiFGOAKKYbL60242826 = -708703404;    float VBQIzmjdGIiFGOAKKYbL26423046 = -97270425;    float VBQIzmjdGIiFGOAKKYbL44661765 = -524553581;    float VBQIzmjdGIiFGOAKKYbL37649208 = -278843302;    float VBQIzmjdGIiFGOAKKYbL39860080 = -156356265;    float VBQIzmjdGIiFGOAKKYbL15255193 = -566913941;    float VBQIzmjdGIiFGOAKKYbL69019288 = 7222509;    float VBQIzmjdGIiFGOAKKYbL73259753 = -653309641;    float VBQIzmjdGIiFGOAKKYbL89459151 = -801151453;    float VBQIzmjdGIiFGOAKKYbL46206240 = -303824447;    float VBQIzmjdGIiFGOAKKYbL45363863 = 65050802;    float VBQIzmjdGIiFGOAKKYbL28505811 = -196137392;    float VBQIzmjdGIiFGOAKKYbL45305002 = 83686456;    float VBQIzmjdGIiFGOAKKYbL68355083 = -54350903;    float VBQIzmjdGIiFGOAKKYbL83261339 = -822097476;    float VBQIzmjdGIiFGOAKKYbL17483646 = -28621863;    float VBQIzmjdGIiFGOAKKYbL23106065 = 63896696;    float VBQIzmjdGIiFGOAKKYbL85562846 = -534007732;    float VBQIzmjdGIiFGOAKKYbL94432181 = -182294983;    float VBQIzmjdGIiFGOAKKYbL97153910 = 71872431;    float VBQIzmjdGIiFGOAKKYbL6473115 = -648487883;    float VBQIzmjdGIiFGOAKKYbL60941435 = -102728739;    float VBQIzmjdGIiFGOAKKYbL49987132 = -690453602;    float VBQIzmjdGIiFGOAKKYbL20496277 = -313202038;    float VBQIzmjdGIiFGOAKKYbL80084881 = -142323519;    float VBQIzmjdGIiFGOAKKYbL22853421 = -203650954;    float VBQIzmjdGIiFGOAKKYbL17957713 = -497820762;    float VBQIzmjdGIiFGOAKKYbL40129557 = -226875181;    float VBQIzmjdGIiFGOAKKYbL70224234 = -243445384;    float VBQIzmjdGIiFGOAKKYbL30045495 = -35247163;    float VBQIzmjdGIiFGOAKKYbL4874553 = -372311126;    float VBQIzmjdGIiFGOAKKYbL88940859 = -962276579;    float VBQIzmjdGIiFGOAKKYbL46498664 = 78654985;    float VBQIzmjdGIiFGOAKKYbL5351609 = -40538192;    float VBQIzmjdGIiFGOAKKYbL2610148 = -503738108;    float VBQIzmjdGIiFGOAKKYbL40863332 = -793558850;    float VBQIzmjdGIiFGOAKKYbL84626883 = -276552283;    float VBQIzmjdGIiFGOAKKYbL17508867 = -267266354;    float VBQIzmjdGIiFGOAKKYbL16142716 = -76245293;    float VBQIzmjdGIiFGOAKKYbL61475984 = -497240857;    float VBQIzmjdGIiFGOAKKYbL69169419 = -424446382;    float VBQIzmjdGIiFGOAKKYbL76223004 = 22450562;    float VBQIzmjdGIiFGOAKKYbL58462845 = -94041196;    float VBQIzmjdGIiFGOAKKYbL46629638 = -466234624;    float VBQIzmjdGIiFGOAKKYbL94847740 = -928654477;    float VBQIzmjdGIiFGOAKKYbL8488656 = 4838990;    float VBQIzmjdGIiFGOAKKYbL22019719 = -318611109;    float VBQIzmjdGIiFGOAKKYbL8967337 = -186259157;    float VBQIzmjdGIiFGOAKKYbL7957708 = -87405905;    float VBQIzmjdGIiFGOAKKYbL9551649 = -165414584;    float VBQIzmjdGIiFGOAKKYbL44968658 = -520820125;    float VBQIzmjdGIiFGOAKKYbL60985813 = -660929931;    float VBQIzmjdGIiFGOAKKYbL64473503 = -728163945;    float VBQIzmjdGIiFGOAKKYbL46061632 = -913594978;    float VBQIzmjdGIiFGOAKKYbL93538797 = -928840017;    float VBQIzmjdGIiFGOAKKYbL99301390 = -505974665;    float VBQIzmjdGIiFGOAKKYbL76435913 = -406816823;    float VBQIzmjdGIiFGOAKKYbL24165488 = -111351544;    float VBQIzmjdGIiFGOAKKYbL57564326 = -36519784;    float VBQIzmjdGIiFGOAKKYbL17006659 = -952705311;    float VBQIzmjdGIiFGOAKKYbL97297479 = 30906820;    float VBQIzmjdGIiFGOAKKYbL28889731 = -765902310;    float VBQIzmjdGIiFGOAKKYbL3035520 = -309864258;    float VBQIzmjdGIiFGOAKKYbL59413656 = -665904290;    float VBQIzmjdGIiFGOAKKYbL41331687 = -931513322;    float VBQIzmjdGIiFGOAKKYbL56423004 = 27327380;    float VBQIzmjdGIiFGOAKKYbL82007146 = -174792378;    float VBQIzmjdGIiFGOAKKYbL39953394 = -875775353;    float VBQIzmjdGIiFGOAKKYbL65744935 = -550612796;    float VBQIzmjdGIiFGOAKKYbL42398007 = 71461373;    float VBQIzmjdGIiFGOAKKYbL32856763 = -752069581;    float VBQIzmjdGIiFGOAKKYbL5597199 = -668836951;    float VBQIzmjdGIiFGOAKKYbL69420131 = -357762440;    float VBQIzmjdGIiFGOAKKYbL32956197 = -685054127;    float VBQIzmjdGIiFGOAKKYbL27984492 = -503681187;    float VBQIzmjdGIiFGOAKKYbL30250111 = -570938446;    float VBQIzmjdGIiFGOAKKYbL2478591 = 91312456;    float VBQIzmjdGIiFGOAKKYbL3357495 = -124218979;    float VBQIzmjdGIiFGOAKKYbL25648537 = -384547562;    float VBQIzmjdGIiFGOAKKYbL71596226 = -47162509;    float VBQIzmjdGIiFGOAKKYbL833703 = -885039845;    float VBQIzmjdGIiFGOAKKYbL8990376 = -211561606;    float VBQIzmjdGIiFGOAKKYbL32171850 = -39469277;    float VBQIzmjdGIiFGOAKKYbL60672586 = 21969200;    float VBQIzmjdGIiFGOAKKYbL85076836 = -514427039;    float VBQIzmjdGIiFGOAKKYbL43888739 = -711381195;    float VBQIzmjdGIiFGOAKKYbL24467357 = -134112634;    float VBQIzmjdGIiFGOAKKYbL437033 = -7750037;    float VBQIzmjdGIiFGOAKKYbL11812812 = -111698175;    float VBQIzmjdGIiFGOAKKYbL3308757 = -997763443;    float VBQIzmjdGIiFGOAKKYbL64427418 = -286742027;    float VBQIzmjdGIiFGOAKKYbL60461395 = -65200739;    float VBQIzmjdGIiFGOAKKYbL59944540 = -130746570;    float VBQIzmjdGIiFGOAKKYbL99136056 = -123539982;    float VBQIzmjdGIiFGOAKKYbL64178505 = -428147677;    float VBQIzmjdGIiFGOAKKYbL40279688 = -658544072;    float VBQIzmjdGIiFGOAKKYbL73187485 = 89541073;     VBQIzmjdGIiFGOAKKYbL58905684 = VBQIzmjdGIiFGOAKKYbL43215543;     VBQIzmjdGIiFGOAKKYbL43215543 = VBQIzmjdGIiFGOAKKYbL11912;     VBQIzmjdGIiFGOAKKYbL11912 = VBQIzmjdGIiFGOAKKYbL60242826;     VBQIzmjdGIiFGOAKKYbL60242826 = VBQIzmjdGIiFGOAKKYbL26423046;     VBQIzmjdGIiFGOAKKYbL26423046 = VBQIzmjdGIiFGOAKKYbL44661765;     VBQIzmjdGIiFGOAKKYbL44661765 = VBQIzmjdGIiFGOAKKYbL37649208;     VBQIzmjdGIiFGOAKKYbL37649208 = VBQIzmjdGIiFGOAKKYbL39860080;     VBQIzmjdGIiFGOAKKYbL39860080 = VBQIzmjdGIiFGOAKKYbL15255193;     VBQIzmjdGIiFGOAKKYbL15255193 = VBQIzmjdGIiFGOAKKYbL69019288;     VBQIzmjdGIiFGOAKKYbL69019288 = VBQIzmjdGIiFGOAKKYbL73259753;     VBQIzmjdGIiFGOAKKYbL73259753 = VBQIzmjdGIiFGOAKKYbL89459151;     VBQIzmjdGIiFGOAKKYbL89459151 = VBQIzmjdGIiFGOAKKYbL46206240;     VBQIzmjdGIiFGOAKKYbL46206240 = VBQIzmjdGIiFGOAKKYbL45363863;     VBQIzmjdGIiFGOAKKYbL45363863 = VBQIzmjdGIiFGOAKKYbL28505811;     VBQIzmjdGIiFGOAKKYbL28505811 = VBQIzmjdGIiFGOAKKYbL45305002;     VBQIzmjdGIiFGOAKKYbL45305002 = VBQIzmjdGIiFGOAKKYbL68355083;     VBQIzmjdGIiFGOAKKYbL68355083 = VBQIzmjdGIiFGOAKKYbL83261339;     VBQIzmjdGIiFGOAKKYbL83261339 = VBQIzmjdGIiFGOAKKYbL17483646;     VBQIzmjdGIiFGOAKKYbL17483646 = VBQIzmjdGIiFGOAKKYbL23106065;     VBQIzmjdGIiFGOAKKYbL23106065 = VBQIzmjdGIiFGOAKKYbL85562846;     VBQIzmjdGIiFGOAKKYbL85562846 = VBQIzmjdGIiFGOAKKYbL94432181;     VBQIzmjdGIiFGOAKKYbL94432181 = VBQIzmjdGIiFGOAKKYbL97153910;     VBQIzmjdGIiFGOAKKYbL97153910 = VBQIzmjdGIiFGOAKKYbL6473115;     VBQIzmjdGIiFGOAKKYbL6473115 = VBQIzmjdGIiFGOAKKYbL60941435;     VBQIzmjdGIiFGOAKKYbL60941435 = VBQIzmjdGIiFGOAKKYbL49987132;     VBQIzmjdGIiFGOAKKYbL49987132 = VBQIzmjdGIiFGOAKKYbL20496277;     VBQIzmjdGIiFGOAKKYbL20496277 = VBQIzmjdGIiFGOAKKYbL80084881;     VBQIzmjdGIiFGOAKKYbL80084881 = VBQIzmjdGIiFGOAKKYbL22853421;     VBQIzmjdGIiFGOAKKYbL22853421 = VBQIzmjdGIiFGOAKKYbL17957713;     VBQIzmjdGIiFGOAKKYbL17957713 = VBQIzmjdGIiFGOAKKYbL40129557;     VBQIzmjdGIiFGOAKKYbL40129557 = VBQIzmjdGIiFGOAKKYbL70224234;     VBQIzmjdGIiFGOAKKYbL70224234 = VBQIzmjdGIiFGOAKKYbL30045495;     VBQIzmjdGIiFGOAKKYbL30045495 = VBQIzmjdGIiFGOAKKYbL4874553;     VBQIzmjdGIiFGOAKKYbL4874553 = VBQIzmjdGIiFGOAKKYbL88940859;     VBQIzmjdGIiFGOAKKYbL88940859 = VBQIzmjdGIiFGOAKKYbL46498664;     VBQIzmjdGIiFGOAKKYbL46498664 = VBQIzmjdGIiFGOAKKYbL5351609;     VBQIzmjdGIiFGOAKKYbL5351609 = VBQIzmjdGIiFGOAKKYbL2610148;     VBQIzmjdGIiFGOAKKYbL2610148 = VBQIzmjdGIiFGOAKKYbL40863332;     VBQIzmjdGIiFGOAKKYbL40863332 = VBQIzmjdGIiFGOAKKYbL84626883;     VBQIzmjdGIiFGOAKKYbL84626883 = VBQIzmjdGIiFGOAKKYbL17508867;     VBQIzmjdGIiFGOAKKYbL17508867 = VBQIzmjdGIiFGOAKKYbL16142716;     VBQIzmjdGIiFGOAKKYbL16142716 = VBQIzmjdGIiFGOAKKYbL61475984;     VBQIzmjdGIiFGOAKKYbL61475984 = VBQIzmjdGIiFGOAKKYbL69169419;     VBQIzmjdGIiFGOAKKYbL69169419 = VBQIzmjdGIiFGOAKKYbL76223004;     VBQIzmjdGIiFGOAKKYbL76223004 = VBQIzmjdGIiFGOAKKYbL58462845;     VBQIzmjdGIiFGOAKKYbL58462845 = VBQIzmjdGIiFGOAKKYbL46629638;     VBQIzmjdGIiFGOAKKYbL46629638 = VBQIzmjdGIiFGOAKKYbL94847740;     VBQIzmjdGIiFGOAKKYbL94847740 = VBQIzmjdGIiFGOAKKYbL8488656;     VBQIzmjdGIiFGOAKKYbL8488656 = VBQIzmjdGIiFGOAKKYbL22019719;     VBQIzmjdGIiFGOAKKYbL22019719 = VBQIzmjdGIiFGOAKKYbL8967337;     VBQIzmjdGIiFGOAKKYbL8967337 = VBQIzmjdGIiFGOAKKYbL7957708;     VBQIzmjdGIiFGOAKKYbL7957708 = VBQIzmjdGIiFGOAKKYbL9551649;     VBQIzmjdGIiFGOAKKYbL9551649 = VBQIzmjdGIiFGOAKKYbL44968658;     VBQIzmjdGIiFGOAKKYbL44968658 = VBQIzmjdGIiFGOAKKYbL60985813;     VBQIzmjdGIiFGOAKKYbL60985813 = VBQIzmjdGIiFGOAKKYbL64473503;     VBQIzmjdGIiFGOAKKYbL64473503 = VBQIzmjdGIiFGOAKKYbL46061632;     VBQIzmjdGIiFGOAKKYbL46061632 = VBQIzmjdGIiFGOAKKYbL93538797;     VBQIzmjdGIiFGOAKKYbL93538797 = VBQIzmjdGIiFGOAKKYbL99301390;     VBQIzmjdGIiFGOAKKYbL99301390 = VBQIzmjdGIiFGOAKKYbL76435913;     VBQIzmjdGIiFGOAKKYbL76435913 = VBQIzmjdGIiFGOAKKYbL24165488;     VBQIzmjdGIiFGOAKKYbL24165488 = VBQIzmjdGIiFGOAKKYbL57564326;     VBQIzmjdGIiFGOAKKYbL57564326 = VBQIzmjdGIiFGOAKKYbL17006659;     VBQIzmjdGIiFGOAKKYbL17006659 = VBQIzmjdGIiFGOAKKYbL97297479;     VBQIzmjdGIiFGOAKKYbL97297479 = VBQIzmjdGIiFGOAKKYbL28889731;     VBQIzmjdGIiFGOAKKYbL28889731 = VBQIzmjdGIiFGOAKKYbL3035520;     VBQIzmjdGIiFGOAKKYbL3035520 = VBQIzmjdGIiFGOAKKYbL59413656;     VBQIzmjdGIiFGOAKKYbL59413656 = VBQIzmjdGIiFGOAKKYbL41331687;     VBQIzmjdGIiFGOAKKYbL41331687 = VBQIzmjdGIiFGOAKKYbL56423004;     VBQIzmjdGIiFGOAKKYbL56423004 = VBQIzmjdGIiFGOAKKYbL82007146;     VBQIzmjdGIiFGOAKKYbL82007146 = VBQIzmjdGIiFGOAKKYbL39953394;     VBQIzmjdGIiFGOAKKYbL39953394 = VBQIzmjdGIiFGOAKKYbL65744935;     VBQIzmjdGIiFGOAKKYbL65744935 = VBQIzmjdGIiFGOAKKYbL42398007;     VBQIzmjdGIiFGOAKKYbL42398007 = VBQIzmjdGIiFGOAKKYbL32856763;     VBQIzmjdGIiFGOAKKYbL32856763 = VBQIzmjdGIiFGOAKKYbL5597199;     VBQIzmjdGIiFGOAKKYbL5597199 = VBQIzmjdGIiFGOAKKYbL69420131;     VBQIzmjdGIiFGOAKKYbL69420131 = VBQIzmjdGIiFGOAKKYbL32956197;     VBQIzmjdGIiFGOAKKYbL32956197 = VBQIzmjdGIiFGOAKKYbL27984492;     VBQIzmjdGIiFGOAKKYbL27984492 = VBQIzmjdGIiFGOAKKYbL30250111;     VBQIzmjdGIiFGOAKKYbL30250111 = VBQIzmjdGIiFGOAKKYbL2478591;     VBQIzmjdGIiFGOAKKYbL2478591 = VBQIzmjdGIiFGOAKKYbL3357495;     VBQIzmjdGIiFGOAKKYbL3357495 = VBQIzmjdGIiFGOAKKYbL25648537;     VBQIzmjdGIiFGOAKKYbL25648537 = VBQIzmjdGIiFGOAKKYbL71596226;     VBQIzmjdGIiFGOAKKYbL71596226 = VBQIzmjdGIiFGOAKKYbL833703;     VBQIzmjdGIiFGOAKKYbL833703 = VBQIzmjdGIiFGOAKKYbL8990376;     VBQIzmjdGIiFGOAKKYbL8990376 = VBQIzmjdGIiFGOAKKYbL32171850;     VBQIzmjdGIiFGOAKKYbL32171850 = VBQIzmjdGIiFGOAKKYbL60672586;     VBQIzmjdGIiFGOAKKYbL60672586 = VBQIzmjdGIiFGOAKKYbL85076836;     VBQIzmjdGIiFGOAKKYbL85076836 = VBQIzmjdGIiFGOAKKYbL43888739;     VBQIzmjdGIiFGOAKKYbL43888739 = VBQIzmjdGIiFGOAKKYbL24467357;     VBQIzmjdGIiFGOAKKYbL24467357 = VBQIzmjdGIiFGOAKKYbL437033;     VBQIzmjdGIiFGOAKKYbL437033 = VBQIzmjdGIiFGOAKKYbL11812812;     VBQIzmjdGIiFGOAKKYbL11812812 = VBQIzmjdGIiFGOAKKYbL3308757;     VBQIzmjdGIiFGOAKKYbL3308757 = VBQIzmjdGIiFGOAKKYbL64427418;     VBQIzmjdGIiFGOAKKYbL64427418 = VBQIzmjdGIiFGOAKKYbL60461395;     VBQIzmjdGIiFGOAKKYbL60461395 = VBQIzmjdGIiFGOAKKYbL59944540;     VBQIzmjdGIiFGOAKKYbL59944540 = VBQIzmjdGIiFGOAKKYbL99136056;     VBQIzmjdGIiFGOAKKYbL99136056 = VBQIzmjdGIiFGOAKKYbL64178505;     VBQIzmjdGIiFGOAKKYbL64178505 = VBQIzmjdGIiFGOAKKYbL40279688;     VBQIzmjdGIiFGOAKKYbL40279688 = VBQIzmjdGIiFGOAKKYbL73187485;     VBQIzmjdGIiFGOAKKYbL73187485 = VBQIzmjdGIiFGOAKKYbL58905684;}
// Junk Finished
