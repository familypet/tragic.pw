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

                                         

















































































































































































































































































