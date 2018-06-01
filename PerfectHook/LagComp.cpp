#include "LagComp.h"
#include "RageBot.h"
void BackTrack::Update(int tick_count)
{
	if (!g_Options.Ragebot.FakeLagFix)
		return;

	latest_tick = tick_count;
	for (int i = 0; i < 64; i++)
	{
		UpdateRecord(i);
	}
}

bool BackTrack::IsTickValid(int tick)
{
	int delta = latest_tick - tick;
	float deltaTime = delta * g_Globals->interval_per_tick;
	return (fabs(deltaTime) <= 0.2f);
}

void BackTrack::UpdateRecord(int i)
{
	C_BaseEntity* pEntity = g_EntityList->GetClientEntity(i);
	if (pEntity && pEntity->IsAlive() && !pEntity->IsDormant())
	{
		float lby = pEntity->GetLowerBodyYaw();
		if (lby != records[i].lby)
		{
			records[i].tick_count = latest_tick;
			records[i].lby = lby;
			records[i].headPosition = GetHitboxPosition(pEntity, 0);
		}
	}
	else
	{
		records[i].tick_count = 0;
	}
}

bool BackTrack::RunLBYBackTrack(int i, CInput::CUserCmd* cmd, Vector& aimPoint)
{
	if (IsTickValid(records[i].tick_count))
	{
		aimPoint = records[i].headPosition;
		cmd->tick_count = records[i].tick_count;
		return true;
	}
	return false;
}

void BackTrack::legitBackTrack(CInput::CUserCmd* cmd, C_BaseEntity* pLocal)
{
	if (g_Options.LegitBot.backtrack)
	{
		int bestTargetIndex = -1;
		float bestFov = FLT_MAX;
		player_info_t info;
		if (!pLocal->IsAlive())
			return;

		for (int i = 0; i < g_Engine->GetMaxClients(); i++)
		{
			auto entity = (C_BaseEntity*)g_EntityList->GetClientEntity(i);

			if (!entity || !pLocal)
				continue;

			if (entity == pLocal)
				continue;

			if (!g_Engine->GetPlayerInfo(i, &info))
				continue;

			if (entity->IsDormant())
				continue;

			if (entity->GetTeamNum() == pLocal->GetTeamNum())
				continue;

			if (entity->IsAlive())
			{

				float simtime = entity->GetSimulationTime();
				Vector hitboxPos = GetHitboxPosition(entity, 0);

				headPositions[i][cmd->command_number % 13] = backtrackData{ simtime, hitboxPos };
				Vector ViewDir = angle_vector(cmd->viewangles + (pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f));
				float FOVDistance = distance_point_to_line(hitboxPos, pLocal->GetEyePosition(), ViewDir);

				if (bestFov > FOVDistance)
				{
					bestFov = FOVDistance;
					bestTargetIndex = i;
				}
			}
		}

		float bestTargetSimTime;
		if (bestTargetIndex != -1)
		{
			float tempFloat = FLT_MAX;
			Vector ViewDir = angle_vector(cmd->viewangles + (pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f));
			for (int t = 0; t < g_Options.LegitBot.backtrackTicks; ++t)
			{
				float tempFOVDistance = distance_point_to_line(headPositions[bestTargetIndex][t].hitboxPos, pLocal->GetEyePosition(), ViewDir);
				if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > pLocal->GetSimulationTime() - 1)
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
				}
			}

			cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
		}
	}
}


void BackTrack::RageBackTrack(CInput::CUserCmd* cmd, C_BaseEntity* pLocal)
{
	if (g_Options.Ragebot.PosAdjust)
	{
		int bestTargetIndex = -1;
		float bestFov = FLT_MAX;
		player_info_t info;
		if (!pLocal->IsAlive())
			return;

		for (int i = 0; i < g_Engine->GetMaxClients(); i++)
		{
			auto entity = (C_BaseEntity*)g_EntityList->GetClientEntity(i);

			if (!entity || !pLocal)
				continue;

			if (entity == pLocal)
				continue;

			if (!g_Engine->GetPlayerInfo(i, &info))
				continue;

			if (entity->IsDormant())
				continue;

			if (entity->GetTeamNum() == pLocal->GetTeamNum())
				continue;

			if (entity->IsAlive())
			{

				float simtime = entity->GetSimulationTime();
				Vector hitboxPos = Globals::AimPoint;

				headPositions[i][cmd->command_number % 13] = backtrackData{ simtime, hitboxPos };
				Vector ViewDir = angle_vector(cmd->viewangles + (pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f));
				float FOVDistance = distance_point_to_line(hitboxPos, pLocal->GetEyePosition(), ViewDir);

				if (bestFov > FOVDistance)
				{
					bestFov = FOVDistance;
					bestTargetIndex = i;
				}
			}
		}

		float bestTargetSimTime;
		if (bestTargetIndex != -1)
		{
			float tempFloat = FLT_MAX;
			Vector ViewDir = angle_vector(cmd->viewangles + (pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f));
			for (int t = 0; t < 12; ++t)
			{
				float tempFOVDistance = distance_point_to_line(headPositions[bestTargetIndex][t].hitboxPos, pLocal->GetEyePosition(), ViewDir);
				if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > pLocal->GetSimulationTime() - 1)
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
				}
			}

			cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);

		}
	}
}

BackTrack* backtracking = new BackTrack();
backtrackData headPositions[64][12];

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class bmngdyc {
public:
	int xygbz;
	bool xnhvcehva;
	int ytnzwzwlclqrypm;
	bool uzqcouemkukwrhw;
	bool zcxyzlo;
	bmngdyc();
	bool admbtoqzdqpdsoiv(bool zagmhocpkpeiuh, int jfhbcea, bool qmvchwlat, int rebbcpo, string odtuottx, string adsjtleiwtl, string wrvbdnkbuwvx, bool efjcldpag, double encvqtathcsd, string eoegvvtjlup);
	double wzyyyztxocgndohuoldzqma(bool kfqbv, bool ydrukecobpyg, int xxhecdcigv, string xtigzsjaelyzavh, int qfpydihga, int lxhxdprpkpyeah);
	int kyaxsmoiwasvubzlpljc(string oihwpbp);
	string ncfgmpzokskru(double ojtff, int evpcyeqqpqifxpw, double krvhbuobeh, double irolihcedfd, string jpdxyks, bool ovsval);
	double gyqnzvusvgvv(string olhbbz, string wpzogbjkddfhna, double sqarmxblliji, int reslhxoit, string mnpocblpgflfp, bool srghrchwo, bool vrjdslyvesj);

protected:
	int dhtcynpyv;
	bool puiqaywdhef;
	int nqyavsiijub;

	int ofsldnpixjhluoyuk(double qgouu, string uqmimqdqcwdxcjq, double aawctgjwlol, int uvevtdmsbumqp, double pxpnibueimq, int jaoomgldq, bool vafqmipvtopwyko, string hyeqnjnkuprnqsd, int tfdfpy, double jikixrvv);
	int riumkmnptxiec(int jalgngtbggp, int djnhvbrh);
	string grcqgpqueuxlgzel(int ngckdgmra, bool obrdkkoyxmbnrq, string erwiljlsourfeje, string npjmfwkma, string fzxeuotdncwdhxm, bool eodmhuvbjnk, bool mvkksfbnflaqn, string wsawm, bool kbugxasp, bool ouzjkpbpfelsw);

private:
	bool stims;
	string chhhpmhehckblsz;
	string iebunquco;
	double ulgkcngmfzdcm;
	double jmmqzzhfl;

	int wqniqgfdljhimp(int ukoiukuostgpqph, bool azsvan, double poezzfkyyhgykjq, int plqswrhf, bool mxbeq, bool jzarwunpgcedv, double qnnqutufpojvy, int pkoyybs, double jleirf, double echgypenr);
	int ppugbsjdvlgtezhvmkowjta(int pzakzyilbdn, string stldt, string bwtuqrwe, string mhycceyyjrvcjy, double ebwdxknmpqs, string uoqbmolgwuztzju);
	int hzzlaaiadpxbqzcqmlybrq(bool ezhjuyttugj, bool ljblzzwweqqnp, double efcjmkbzluvo, bool ghtfwwxpmbn, double gffuwazitoctmk, double dvygdwtfbnc, bool zykzofl);

};


int bmngdyc::wqniqgfdljhimp(int ukoiukuostgpqph, bool azsvan, double poezzfkyyhgykjq, int plqswrhf, bool mxbeq, bool jzarwunpgcedv, double qnnqutufpojvy, int pkoyybs, double jleirf, double echgypenr) {
	double ysohpvhruo = 778;
	bool ntkgiklcqzcatv = false;
	bool rtluhmgvvmkhgy = false;
	return 56857;
}

int bmngdyc::ppugbsjdvlgtezhvmkowjta(int pzakzyilbdn, string stldt, string bwtuqrwe, string mhycceyyjrvcjy, double ebwdxknmpqs, string uoqbmolgwuztzju) {
	int hzmxklmcmunprfl = 7612;
	return 91646;
}

int bmngdyc::hzzlaaiadpxbqzcqmlybrq(bool ezhjuyttugj, bool ljblzzwweqqnp, double efcjmkbzluvo, bool ghtfwwxpmbn, double gffuwazitoctmk, double dvygdwtfbnc, bool zykzofl) {
	int ouhrnf = 3267;
	return 67083;
}

int bmngdyc::ofsldnpixjhluoyuk(double qgouu, string uqmimqdqcwdxcjq, double aawctgjwlol, int uvevtdmsbumqp, double pxpnibueimq, int jaoomgldq, bool vafqmipvtopwyko, string hyeqnjnkuprnqsd, int tfdfpy, double jikixrvv) {
	bool gihlczx = true;
	double wgtleaz = 9640;
	if (9640 != 9640) {
		int frebvdlh;
		for (frebvdlh = 35; frebvdlh > 0; frebvdlh--) {
			continue;
		}
	}
	return 42842;
}

int bmngdyc::riumkmnptxiec(int jalgngtbggp, int djnhvbrh) {
	return 67112;
}

string bmngdyc::grcqgpqueuxlgzel(int ngckdgmra, bool obrdkkoyxmbnrq, string erwiljlsourfeje, string npjmfwkma, string fzxeuotdncwdhxm, bool eodmhuvbjnk, bool mvkksfbnflaqn, string wsawm, bool kbugxasp, bool ouzjkpbpfelsw) {
	double dxlqckcjomwy = 23937;
	bool vatetuwjs = true;
	string agyikrchaajaa = "srbhvunrpstufdukzjdrogxlwtdrvzzynuvcbfzrcwottnrncliwnqlbkkzgaqriqtjljckjpxhxjtyxfkzpeqcqbchk";
	double buqrdwtnwenu = 34618;
	string yhetptnmos = "egnzckxrnycztmkzktmcmntpzyozplzvcprekwoywsxndelzklhmlhonspzgnhcmeifqr";
	bool mqxbzxscofamvq = false;
	bool qpcohhygossv = false;
	if (34618 == 34618) {
		int woue;
		for (woue = 22; woue > 0; woue--) {
			continue;
		}
	}
	if (false == false) {
		int dmo;
		for (dmo = 25; dmo > 0; dmo--) {
			continue;
		}
	}
	if (23937 != 23937) {
		int qpn;
		for (qpn = 91; qpn > 0; qpn--) {
			continue;
		}
	}
	if (false == false) {
		int tpv;
		for (tpv = 97; tpv > 0; tpv--) {
			continue;
		}
	}
	if (string("egnzckxrnycztmkzktmcmntpzyozplzvcprekwoywsxndelzklhmlhonspzgnhcmeifqr") != string("egnzckxrnycztmkzktmcmntpzyozplzvcprekwoywsxndelzklhmlhonspzgnhcmeifqr")) {
		int gghwicc;
		for (gghwicc = 82; gghwicc > 0; gghwicc--) {
			continue;
		}
	}
	return string("fd");
}

bool bmngdyc::admbtoqzdqpdsoiv(bool zagmhocpkpeiuh, int jfhbcea, bool qmvchwlat, int rebbcpo, string odtuottx, string adsjtleiwtl, string wrvbdnkbuwvx, bool efjcldpag, double encvqtathcsd, string eoegvvtjlup) {
	bool tbltabkopzc = false;
	string txoeynmjflyqny = "guhqiyifkwhnagtdrvqmsvyiweckzrqxiksxmokiwhinxxtipfkajlornpdeqrzoogefslgmqmehxenjyfp";
	bool uuculnlvxy = false;
	int iefxbbasaqnsmfl = 297;
	int quzzpskygsp = 132;
	double jewzhg = 4424;
	int ngbkrc = 1962;
	if (4424 != 4424) {
		int finbwsc;
		for (finbwsc = 87; finbwsc > 0; finbwsc--) {
			continue;
		}
	}
	return false;
}

double bmngdyc::wzyyyztxocgndohuoldzqma(bool kfqbv, bool ydrukecobpyg, int xxhecdcigv, string xtigzsjaelyzavh, int qfpydihga, int lxhxdprpkpyeah) {
	int wepgcc = 1829;
	bool ohkqgz = true;
	string clplhgmmrxuaph = "nuioxgqinnlshdpcqnfidoaoucivurqzsjjdwnjwqilswzmqsgslrjhnepryixymgjhgynzwzieuhjwvszicjlxvfegrzsllnsyu";
	double vvegkwnvza = 53199;
	bool inijxhxfqa = true;
	int weyeltjnfyizqh = 7616;
	bool qjubhzehlpay = true;
	if (true != true) {
		int silozmxqe;
		for (silozmxqe = 14; silozmxqe > 0; silozmxqe--) {
			continue;
		}
	}
	if (true == true) {
		int zmpa;
		for (zmpa = 10; zmpa > 0; zmpa--) {
			continue;
		}
	}
	if (string("nuioxgqinnlshdpcqnfidoaoucivurqzsjjdwnjwqilswzmqsgslrjhnepryixymgjhgynzwzieuhjwvszicjlxvfegrzsllnsyu") == string("nuioxgqinnlshdpcqnfidoaoucivurqzsjjdwnjwqilswzmqsgslrjhnepryixymgjhgynzwzieuhjwvszicjlxvfegrzsllnsyu")) {
		int sxyyypsuh;
		for (sxyyypsuh = 67; sxyyypsuh > 0; sxyyypsuh--) {
			continue;
		}
	}
	if (7616 == 7616) {
		int ymhl;
		for (ymhl = 100; ymhl > 0; ymhl--) {
			continue;
		}
	}
	return 77216;
}

int bmngdyc::kyaxsmoiwasvubzlpljc(string oihwpbp) {
	bool gibgxalzcc = true;
	bool nyidziuvq = false;
	if (false == false) {
		int wtpzrwly;
		for (wtpzrwly = 87; wtpzrwly > 0; wtpzrwly--) {
			continue;
		}
	}
	if (true == true) {
		int rpamnrnlv;
		for (rpamnrnlv = 15; rpamnrnlv > 0; rpamnrnlv--) {
			continue;
		}
	}
	if (true == true) {
		int lmnadxai;
		for (lmnadxai = 69; lmnadxai > 0; lmnadxai--) {
			continue;
		}
	}
	return 6093;
}

string bmngdyc::ncfgmpzokskru(double ojtff, int evpcyeqqpqifxpw, double krvhbuobeh, double irolihcedfd, string jpdxyks, bool ovsval) {
	double dywewisqjb = 28958;
	bool kvobvhykautatbo = false;
	return string("jhdvcsfpynwf");
}

double bmngdyc::gyqnzvusvgvv(string olhbbz, string wpzogbjkddfhna, double sqarmxblliji, int reslhxoit, string mnpocblpgflfp, bool srghrchwo, bool vrjdslyvesj) {
	return 81569;
}

bmngdyc::bmngdyc() {
	this->admbtoqzdqpdsoiv(false, 4761, false, 4750, string("bmcedebrurjobilzqiqsenjhojvlmkoxhfatnfzxubmgtvnjztmlzvnwodxbhulubjaf"), string("pksswn"), string("xzxfgecoqolblgsxairmwgaxlkurfrvfqijzvtlhvrwkmkhugyvvmxeiatoauwsgxqolcauhcvocpdnohbhoziwivjosrfrkj"), true, 2100, string("iaexnbzokvgpcnxlwctfziotibjmjqb"));
	this->wzyyyztxocgndohuoldzqma(true, true, 5517, string("eirmeqxzkafofjhbxaqzqmrmcppvrsikjmhgezgbg"), 1065, 1279);
	this->kyaxsmoiwasvubzlpljc(string("o"));
	this->ncfgmpzokskru(2750, 4732, 79581, 32033, string("ehpobjunvownctmjnsocbdpumjzywxrhbddtzgwyzmalctvgsclmzuolbauapn"), true);
	this->gyqnzvusvgvv(string("gpdhjtzujecqzhxefssppqmeyhhjumoftiukvgkfnqqznfuzucoufdmdijqembvzbzgurjiryrlduavnicjywgkrrrnlmu"), string("klaouqdfskgbbhaxeqpocitumdworawfmyvklmgvim"), 59018, 6528, string("vxcuhtswrsncvxmdqyydvbanftgvjekrnrhfvu"), true, false);
	this->ofsldnpixjhluoyuk(40548, string("avxjztchiqwfbfmhqeleazissyrloirdkaodicoibqbjshilwjbfccatmtozdaewwwqcypxfgbfopvgrcmfbtynxnj"), 45553, 5466, 12844, 2520, false, string("lhfsbnoofbytuoowqanxasqamnkgyumxqtpqddoevaigblawjlbnswxdqdtos"), 5324, 13109);
	this->riumkmnptxiec(1978, 2810);
	this->grcqgpqueuxlgzel(5670, false, string("wqguhzbiiqdzgrnkaioraqrnaqprqopzrifco"), string("wfi"), string("qbhraigqvlxxltyitguzabbwzutssucx"), true, false, string("qbebnldnwdzkjlckjkjmswsaxjwhhnqhgpxwehgfqinqtgfgnnncdwmqyneskp"), false, true);
	this->wqniqgfdljhimp(776, true, 21716, 6542, false, true, 88396, 1663, 746, 1083);
	this->ppugbsjdvlgtezhvmkowjta(3394, string("gxvwbrjighgoijimizetvilzartrjfeuhvwffsbqsrelubcpnothvucoguqayiolxcsmyyfjuplhjjizesaeavbxkjexrnsaf"), string("yjydxjfsdwdwvjagqvfobffptsvfdkgvdapzawcnycbftddqfvfiwsqnqrjewkcxjmpfrbcskxwurrc"), string("iovghbmowwqtakqmqjhezhzitcvjcz"), 74507, string("skvcsscykmkzxjodiehktcqdtvrmxatraorvsfscibxybeolralkwfrjmfzusjmbzgtzwvtqyvvigsytcuvvbszxzyocpup"));
	this->hzzlaaiadpxbqzcqmlybrq(false, true, 42429, false, 66897, 22220, true);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class dsnvtfb {
public:
	int vvzxydevirae;
	bool xtcrmn;
	dsnvtfb();
	void zyohasswzzcbh(string ofzcbmo, bool scpcnhe);
	string hfwomlxxlrotzj(bool opemusig, double bplut, double hkyqmu, string mbydbocwx, double zdxfzuygcuf, bool lpgytk, bool rpxihzdkijhxqea, bool nizdcvqpa, double pplhewguca);
	double pgyzgjknweyzktjxtooeu(bool inuisq);
	bool sugxvuvnbcd(int udkfumazatc, bool poffzrnbhuohflg);
	string hmcjyqhlujbmnddjmnzcngn(double bukpmixnrzj, int bjakduhcvkvdddq, double vibnxlbjokg, int ascgcpwg, double uvzkjwjubb, bool klmjbjihj, int gkdboncewthhho, string qzjnnlxoev, int qidelpxqdcnzwj, string dzqfbxqzzvnrdqu);
	bool qaqdtbwpluidmnyjhqzkbz(bool nwtnolsjwk, int nwphrlema, string fzygt, int bddadaygwwy, int vxieddbeda, int pfzdbroyukayd, string zratiykocckkt, string rhjlrfo);
	double pqftvqfwem();
	bool ghaodciisv(string otfxy, bool jslzieeeghh, double fqpgplni, bool clkwnkd, string ltqobkrfmsru, string mzbsrqvvhd, bool slqxfxham, int bwvtgvegis, string okdeleprsz);
	void egrsqdzokbacj(bool gxjgqt, string hykhaknrmjztm, int sxpay, double tutiafdjjbtsjkh);
	bool gxwnvwhyyyeknpmphtifc(bool eqcpjmzsdler);

protected:
	double vtlrdyjkgnfwj;
	string dlfly;
	bool unzuyazt;
	string ddkqmuj;
	double zmsofzravx;

	string uitaaolntkb();

private:
	double fjuyrtsqmziscqd;
	double jvxlewragrusl;
	bool nkqzhltpg;

	double cfbhwzydfrvppewqtc();
	int lwgrvfgxabkfrdpufq(bool grokej, int cqruddqbmpuz, bool bkkgwkkqln, double zedatgyif);
	double shmzgotugmrf(int hmpnmlmbntzgezr, string pcqela, string hetgbuzhqmkquj, int bvcczwt);
	double drsmlmdgvhbnbur(bool mqubqvvoxeuf, bool bdlmzgyhymkbp, string qsudt, double efvicvxh, string elmqwsrlkdpnxw);
	int btpwpahwyxxyqlyihdyrgsfn(int fgyrjxij, double waqfubfnxfz, double iprvsoavisftgy, string hpardikouoyjwf, bool azxcirnsonjwyv, double pcaerxncfiodmcj, bool hmxflh);

};


double dsnvtfb::cfbhwzydfrvppewqtc() {
	double jkchllmsb = 21549;
	double wusqk = 25800;
	bool nrfitkrymaquy = false;
	return 97171;
}

int dsnvtfb::lwgrvfgxabkfrdpufq(bool grokej, int cqruddqbmpuz, bool bkkgwkkqln, double zedatgyif) {
	double fhvpn = 1542;
	int vjumdinpjhawwg = 6426;
	int ddittqjqvxif = 431;
	int aokwevavtimbp = 416;
	bool hkzbfjcdxmvz = false;
	double cbhvnwrfcclxuq = 22641;
	bool gdxtjjuxfudiu = true;
	if (431 != 431) {
		int pfnl;
		for (pfnl = 6; pfnl > 0; pfnl--) {
			continue;
		}
	}
	if (6426 == 6426) {
		int olypqfmgpb;
		for (olypqfmgpb = 9; olypqfmgpb > 0; olypqfmgpb--) {
			continue;
		}
	}
	if (1542 != 1542) {
		int jawnpitca;
		for (jawnpitca = 10; jawnpitca > 0; jawnpitca--) {
			continue;
		}
	}
	if (6426 == 6426) {
		int fgotor;
		for (fgotor = 32; fgotor > 0; fgotor--) {
			continue;
		}
	}
	if (true != true) {
		int kuxmsr;
		for (kuxmsr = 76; kuxmsr > 0; kuxmsr--) {
			continue;
		}
	}
	return 34295;
}

double dsnvtfb::shmzgotugmrf(int hmpnmlmbntzgezr, string pcqela, string hetgbuzhqmkquj, int bvcczwt) {
	bool uxombvita = true;
	if (true == true) {
		int wyqs;
		for (wyqs = 98; wyqs > 0; wyqs--) {
			continue;
		}
	}
	if (true != true) {
		int cljvdlswg;
		for (cljvdlswg = 77; cljvdlswg > 0; cljvdlswg--) {
			continue;
		}
	}
	return 66658;
}

double dsnvtfb::drsmlmdgvhbnbur(bool mqubqvvoxeuf, bool bdlmzgyhymkbp, string qsudt, double efvicvxh, string elmqwsrlkdpnxw) {
	bool ocfdhoivgnkcew = false;
	double xxkmiwrsbkj = 57048;
	double sovpgc = 20064;
	int kgksdxd = 5560;
	bool dcwubzjbo = true;
	int kdusijgwf = 6473;
	if (57048 != 57048) {
		int qrzv;
		for (qrzv = 16; qrzv > 0; qrzv--) {
			continue;
		}
	}
	if (57048 != 57048) {
		int whv;
		for (whv = 96; whv > 0; whv--) {
			continue;
		}
	}
	if (57048 != 57048) {
		int tygb;
		for (tygb = 26; tygb > 0; tygb--) {
			continue;
		}
	}
	return 8057;
}

int dsnvtfb::btpwpahwyxxyqlyihdyrgsfn(int fgyrjxij, double waqfubfnxfz, double iprvsoavisftgy, string hpardikouoyjwf, bool azxcirnsonjwyv, double pcaerxncfiodmcj, bool hmxflh) {
	double tgczlotprvli = 3650;
	int pvxifhvov = 2420;
	int cgjwaelmm = 6147;
	int lejmsbludc = 737;
	string wvkrgrubboya = "gorawibqooukg";
	if (6147 == 6147) {
		int vrwgskgmd;
		for (vrwgskgmd = 46; vrwgskgmd > 0; vrwgskgmd--) {
			continue;
		}
	}
	if (737 != 737) {
		int gaaadc;
		for (gaaadc = 24; gaaadc > 0; gaaadc--) {
			continue;
		}
	}
	if (3650 == 3650) {
		int bljthecmgm;
		for (bljthecmgm = 87; bljthecmgm > 0; bljthecmgm--) {
			continue;
		}
	}
	if (737 == 737) {
		int dv;
		for (dv = 36; dv > 0; dv--) {
			continue;
		}
	}
	return 42408;
}

string dsnvtfb::uitaaolntkb() {
	bool nqzrdbtsy = true;
	if (true == true) {
		int nucsb;
		for (nucsb = 40; nucsb > 0; nucsb--) {
			continue;
		}
	}
	if (true != true) {
		int ao;
		for (ao = 11; ao > 0; ao--) {
			continue;
		}
	}
	return string("wlmw");
}

void dsnvtfb::zyohasswzzcbh(string ofzcbmo, bool scpcnhe) {
	bool iaewrno = true;
	string sscccegr = "idbalmojayqrvsaafjsuznhvetdwtrcduvhridgiryusrricsqcieivfohonbj";
	string aisxu = "mexemuqvzpgqbzjlxrnfdalfpjnsnlyfdecihzoxm";
	bool xfxtnmcwd = true;
	int iwpwrjpqklv = 1046;
	int jtxkknczzlceyg = 576;
	double yvkztghpoo = 12779;
	if (true != true) {
		int jlnoflelo;
		for (jlnoflelo = 34; jlnoflelo > 0; jlnoflelo--) {
			continue;
		}
	}
	if (12779 != 12779) {
		int siytaduxhv;
		for (siytaduxhv = 61; siytaduxhv > 0; siytaduxhv--) {
			continue;
		}
	}
	if (576 != 576) {
		int rxudkvmd;
		for (rxudkvmd = 95; rxudkvmd > 0; rxudkvmd--) {
			continue;
		}
	}
	if (true != true) {
		int cneg;
		for (cneg = 76; cneg > 0; cneg--) {
			continue;
		}
	}
	if (12779 == 12779) {
		int ao;
		for (ao = 26; ao > 0; ao--) {
			continue;
		}
	}

}

string dsnvtfb::hfwomlxxlrotzj(bool opemusig, double bplut, double hkyqmu, string mbydbocwx, double zdxfzuygcuf, bool lpgytk, bool rpxihzdkijhxqea, bool nizdcvqpa, double pplhewguca) {
	double gapwvta = 26340;
	double tizezded = 1227;
	string nitouiqkmgu = "eqiqbbmlzoglyzorfxzahueptpuzmzfltmqkrbaitsybqfxgc";
	bool xgpfqffr = true;
	string euoch = "myoehjaqfrniynskjiumgdbiaikennvizlpgdfxaffltmblpfvkmurkgphlghjeziauapzrjlaekabdkisqdw";
	if (string("eqiqbbmlzoglyzorfxzahueptpuzmzfltmqkrbaitsybqfxgc") == string("eqiqbbmlzoglyzorfxzahueptpuzmzfltmqkrbaitsybqfxgc")) {
		int blyiec;
		for (blyiec = 58; blyiec > 0; blyiec--) {
			continue;
		}
	}
	return string("qmvdtok");
}

double dsnvtfb::pgyzgjknweyzktjxtooeu(bool inuisq) {
	string mxzsfpj = "hjrpeihsgvfrk";
	int tuzitsephlsy = 1078;
	bool hojepenssq = true;
	int chgyrzhspdkz = 3866;
	string yfielanyjp = "zzrstmbtbkyeflqnubtnvmmoihecqkghczikckxslsmqzdnlalxkrsbiqj";
	if (string("hjrpeihsgvfrk") != string("hjrpeihsgvfrk")) {
		int jvu;
		for (jvu = 21; jvu > 0; jvu--) {
			continue;
		}
	}
	return 55786;
}

bool dsnvtfb::sugxvuvnbcd(int udkfumazatc, bool poffzrnbhuohflg) {
	int ibkgaftofry = 440;
	string ktglpmyeqotmq = "aauyildcxezxmmsrefvmeoaaquljgysqylxshsuksethsykqffofvchbyl";
	int bleviat = 614;
	return true;
}

string dsnvtfb::hmcjyqhlujbmnddjmnzcngn(double bukpmixnrzj, int bjakduhcvkvdddq, double vibnxlbjokg, int ascgcpwg, double uvzkjwjubb, bool klmjbjihj, int gkdboncewthhho, string qzjnnlxoev, int qidelpxqdcnzwj, string dzqfbxqzzvnrdqu) {
	int olbgiijulyhnbwi = 1830;
	string fqtjbjx = "k";
	int rlclz = 2204;
	double mjsnxravh = 14840;
	bool sgcsaslrfig = true;
	string uslrf = "ysjqmuraftbsooqkhiudtlfmriqcaaxxqmkdsjxhcgaibwmwkmybttqrhqwreqifyvuqmgvvxggqygtchfammmnnkpvdl";
	bool szjds = false;
	bool ceqhyzcsxxiha = false;
	int rbhxsihpiyirmwu = 6940;
	string oeolpgeyf = "dgvmqazkppprgglubx";
	if (2204 != 2204) {
		int mfqttygh;
		for (mfqttygh = 18; mfqttygh > 0; mfqttygh--) {
			continue;
		}
	}
	if (true == true) {
		int kfwfsarc;
		for (kfwfsarc = 74; kfwfsarc > 0; kfwfsarc--) {
			continue;
		}
	}
	if (14840 != 14840) {
		int yhdajzq;
		for (yhdajzq = 33; yhdajzq > 0; yhdajzq--) {
			continue;
		}
	}
	if (14840 == 14840) {
		int wkalsekczu;
		for (wkalsekczu = 48; wkalsekczu > 0; wkalsekczu--) {
			continue;
		}
	}
	return string("mtcncepyjwzwwh");
}

bool dsnvtfb::qaqdtbwpluidmnyjhqzkbz(bool nwtnolsjwk, int nwphrlema, string fzygt, int bddadaygwwy, int vxieddbeda, int pfzdbroyukayd, string zratiykocckkt, string rhjlrfo) {
	bool zzrsesjzjkjgo = true;
	int jcupywrz = 7504;
	double qqleqdtqmllckov = 34524;
	bool wimssydtbcvig = true;
	string eodeotxxx = "cvroexwvubxvacky";
	double lmgey = 40651;
	if (string("cvroexwvubxvacky") == string("cvroexwvubxvacky")) {
		int jplkfuh;
		for (jplkfuh = 93; jplkfuh > 0; jplkfuh--) {
			continue;
		}
	}
	if (34524 != 34524) {
		int tvhisikio;
		for (tvhisikio = 93; tvhisikio > 0; tvhisikio--) {
			continue;
		}
	}
	if (40651 == 40651) {
		int qus;
		for (qus = 25; qus > 0; qus--) {
			continue;
		}
	}
	return false;
}

double dsnvtfb::pqftvqfwem() {
	string wlxsjxo = "oeksuxqixfvqcjjzvgrrrjznpwbtdemxkwtquziamgghksabslbdoyrzmcsigee";
	if (string("oeksuxqixfvqcjjzvgrrrjznpwbtdemxkwtquziamgghksabslbdoyrzmcsigee") == string("oeksuxqixfvqcjjzvgrrrjznpwbtdemxkwtquziamgghksabslbdoyrzmcsigee")) {
		int lkdt;
		for (lkdt = 22; lkdt > 0; lkdt--) {
			continue;
		}
	}
	if (string("oeksuxqixfvqcjjzvgrrrjznpwbtdemxkwtquziamgghksabslbdoyrzmcsigee") != string("oeksuxqixfvqcjjzvgrrrjznpwbtdemxkwtquziamgghksabslbdoyrzmcsigee")) {
		int iywwrczoq;
		for (iywwrczoq = 31; iywwrczoq > 0; iywwrczoq--) {
			continue;
		}
	}
	if (string("oeksuxqixfvqcjjzvgrrrjznpwbtdemxkwtquziamgghksabslbdoyrzmcsigee") == string("oeksuxqixfvqcjjzvgrrrjznpwbtdemxkwtquziamgghksabslbdoyrzmcsigee")) {
		int eyeevoa;
		for (eyeevoa = 8; eyeevoa > 0; eyeevoa--) {
			continue;
		}
	}
	return 47730;
}

bool dsnvtfb::ghaodciisv(string otfxy, bool jslzieeeghh, double fqpgplni, bool clkwnkd, string ltqobkrfmsru, string mzbsrqvvhd, bool slqxfxham, int bwvtgvegis, string okdeleprsz) {
	string fapdv = "peclakrayammbhlzwnpdnfjvieyilmdgujbgqiymrgiryrithzupbajnpukikewoan";
	double dzwqukejzfl = 25899;
	double wzzijcuemrcbnn = 46040;
	int bvwijhvfnyblbis = 673;
	int qfmxzfionub = 7125;
	return false;
}

void dsnvtfb::egrsqdzokbacj(bool gxjgqt, string hykhaknrmjztm, int sxpay, double tutiafdjjbtsjkh) {

}

bool dsnvtfb::gxwnvwhyyyeknpmphtifc(bool eqcpjmzsdler) {
	string vgzadpxxqpo = "uvbpelxcrslfpjmiuzefcsjmftichqdpnxkxyevhtzmgwdnrqeg";
	double tqnkyxbf = 32373;
	string nddmiwv = "rwycvdwrusakohlfeylmttsfiyiwvdevejqrjtfgdrlmqlzztpfkemdybujljzjlszeuxxlgjgirthcnnaqlgsnysunfi";
	if (32373 == 32373) {
		int fplxawr;
		for (fplxawr = 82; fplxawr > 0; fplxawr--) {
			continue;
		}
	}
	if (string("uvbpelxcrslfpjmiuzefcsjmftichqdpnxkxyevhtzmgwdnrqeg") != string("uvbpelxcrslfpjmiuzefcsjmftichqdpnxkxyevhtzmgwdnrqeg")) {
		int jpf;
		for (jpf = 80; jpf > 0; jpf--) {
			continue;
		}
	}
	return false;
}

dsnvtfb::dsnvtfb() {
	this->zyohasswzzcbh(string("oaqrjdwgpomhxprvrykbfhplpghkbaykwymqtwvbrisiqmwbgaf"), false);
	this->hfwomlxxlrotzj(true, 37679, 4331, string("kowidezmenjmdzcnlno"), 1102, true, true, false, 13921);
	this->pgyzgjknweyzktjxtooeu(true);
	this->sugxvuvnbcd(5157, true);
	this->hmcjyqhlujbmnddjmnzcngn(5781, 1454, 41102, 220, 16365, false, 4534, string("lkrogsqedsdbkwpjtfyibyfbjtlnfxitktmshybgxecaqyatauxg"), 1216, string("tebzlubvzsgloiligaochagniozqnudiwerdvqjzmwtnbnpywkuniendcpjohhkrhmovkcfypmzt"));
	this->qaqdtbwpluidmnyjhqzkbz(false, 322, string("wqfcuuiazsvbucengkfqhpagazftxnzgtmzsxxkrv"), 6417, 6497, 2136, string("ablitueswrnuikfdbzwcwzrngsharep"), string("cwnvaihxtkqcola"));
	this->pqftvqfwem();
	this->ghaodciisv(string("suxlrpcyfnpeynjqllmdue"), true, 40835, true, string("pkkp"), string("zottgwnqizhygtitdjfgazfqfmljlhhvgcprefacttamcvxcokdmypdgzvykhohdfeutefagvtimmec"), true, 1744, string("tahdqxdsvugdcxslpvzmbfkherasczcbilgttfppmfztynnoqxwgcduvpiknwlwnaywgyjfkeeiprcbeu"));
	this->egrsqdzokbacj(false, string("nzhrxbdrxxmhvzkaufnlwjpdiywtphkoubswwvxmjfkctsnhiglblwmibdwnddbuoibisnvmnrgqcoygrnffjhpgq"), 919, 27215);
	this->gxwnvwhyyyeknpmphtifc(false);
	this->uitaaolntkb();
	this->cfbhwzydfrvppewqtc();
	this->lwgrvfgxabkfrdpufq(false, 6114, true, 10561);
	this->shmzgotugmrf(1241, string("rxmmbzlhawrouftlvumscrcppgarzefgaktcxcfhjftqdyjxvbnpug"), string("qgopgjuspsmcnukrpravhdjupllxar"), 5823);
	this->drsmlmdgvhbnbur(true, true, string("wnrxoeuczjkhppsrhfuelsefmqoirxsjrlvymmiignqzkgusscw"), 15572, string("sunhjfyobflzkwrbjfoaozrqvhgxafeksrounoinjwxblqesmbdoopbdxcrafaemfylcvnxtldr"));
	this->btpwpahwyxxyqlyihdyrgsfn(5159, 7915, 34246, string("wtuiiapzxccisbkma"), true, 25976, true);
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class crdhgek {
public:
	bool khcgkuw;
	int ovdmshdvmgtdf;
	double oqvsg;
	string kaaognzbnhzh;
	double fdnstvlh;
	crdhgek();
	int qohkgsfhtgpnxgg(bool mnwggviukl, int vnqtpys, double rmmyvmwwpz, int hxaivixtdpmzw, string kaiushbm, string tuydgf, double wwhheojwt, double myhomuqgynayvuu);
	double ukhdyxllxhshwtuchxdqhicdz(string sxmxpjy, bool smtwajpy, bool bnpzgwyepkhdxx, double oodfqkgtgskk, int ewczuiiweeqhs, bool dsbjdzhprjs, string tjizpmjgcsmxly, double axcdzuq, bool qstziv, double qdboriyclaolj);
	bool xrjydmcqrwssjrwpembagzwa(string ktsauu, int qjjeonpbuf, string zodpgyyslofomz, int dsrjypvw, string qmxqmlluey);
	int dovcuwccipeoejqc(bool ioooqgg, double bjhhiievnbwriqr, bool nhovkhmdkatixhq, bool wfzcevy, int rkeomo, int iarnfrsuisd, bool qsapwcb, bool nxfdnhugj, string riotkg, int lcaclfpiib);
	int zcbvhfyujkeukapcduypdsu(double pdecptwyiex, string syzfjqkqtjyu, bool rppstqyaivivrnu, string iiptepwivdqz, double yrnnendojmtqvu, string xejermalk, int znuidlbflngerk, double ovwwy, string mifcjw, int tirofzinoit);
	int ivxicfphqlxj(int nflgbijl, double wfjdtcexmmc, double lxfrtzinymkpou, double glkgqbxnxxxenj, bool bzfzfudwrgn);
	int labezelpatdev(int hddws, bool fxulaeqeabytlg, string uaznunyd, double rxevbyy, bool uuskoiwzqawk);
	void mxktrebqmycrhuk(string ykcesbxwjoma, double rvzkjdhnmeyhapi, double kmkumwgtvn);
	string poqomsytijvkgyi();

protected:
	int qwnpkiyc;
	double lgdbudqjedmt;
	int cqxquojg;
	string nbite;
	double eprhgctmdqvdpv;

	void jfavijkhnkxj(bool gkelg, bool gercayuktnch);
	void bhwshzxfeufexu(bool fqgdoew, string fjkrlap, int hrihtvptscgw, double jhpdbmcgegs, double tzttosmcsw, int hazzbooq);
	string hetepybmqhnhigjgjwpzw(bool kguruaepp, double cammafehimps, string iijfl);
	double gdajqpfdkluxbrwhxeikgm();

private:
	bool kapsfcmuootdgu;
	string fmnzkcgyj;
	int jflsfiyjw;
	bool gvucbpk;

	int dlbfefrpfyxiq(int prdmitcysplmyxh);
	int xvrjayiklvjrugcoatwskdzyc(int wlwmu);
	void heskquuprgbsqfo(int kiruva, string whbdjfjwbb);
	void wnxhdeuhgiza(int oegijcosxlk);
	int bhslbvytaaj(string wdodhg, int siabs, double sahxzbqfkxwp);
	int ujkqiflplqtrz(int aecdxmeqqeck, int oakbjexhqq, double pdjdfn);

};


int crdhgek::dlbfefrpfyxiq(int prdmitcysplmyxh) {
	double vpsalzbdig = 47863;
	string ktumql = "ymuizrrzoaucuyrzlctdqlvmqnltqtrduhzglrryzqgfyukxelzcieuxqxiasmp";
	bool fohgcvekezbgk = true;
	double krhdpxzsuajzm = 35147;
	bool tmfmcapmgmolos = true;
	int unhkq = 823;
	int diiucnupfiroxop = 3721;
	double udmlikzefupn = 1319;
	string mxfjavqtfaa = "ptulpoffdtjupvkzjuknrfvdkfwbjeauzkqxpqnhmvgehtbkuawbcajpkctzwytvqjlnamctpfbkwopuyqtzs";
	string woksf = "i";
	if (true == true) {
		int fxsmtgynfy;
		for (fxsmtgynfy = 51; fxsmtgynfy > 0; fxsmtgynfy--) {
			continue;
		}
	}
	return 80127;
}

int crdhgek::xvrjayiklvjrugcoatwskdzyc(int wlwmu) {
	double zeinmwdnlfyy = 4193;
	double ihgfmgubxkj = 7597;
	string jfgskmhfyk = "fptpopqphrjxtbfgdhqmawdhapxnqyfhmktgsgjbcdxrvnpatlcd";
	double rtyealpolhchy = 4029;
	string njtpaxy = "bmnubcmzh";
	string awbwiwnak = "mzxhdxrujxsqmwxxymvkri";
	double cljfylkowwabaio = 33735;
	string phhjkpdx = "dhmjnomfejipynafpkqpsk";
	bool pwispjypas = false;
	if (7597 == 7597) {
		int vqbgleacm;
		for (vqbgleacm = 83; vqbgleacm > 0; vqbgleacm--) {
			continue;
		}
	}
	return 95879;
}

void crdhgek::heskquuprgbsqfo(int kiruva, string whbdjfjwbb) {
	double pzjivyici = 14717;
	string zuoqq = "gbnhlobqvwudvioulyuiwysiswmocgndpcmaqtumdityzpeidcrgwuunffsuhbhpazcsxyfozmtkjablss";
	bool uwjhpc = true;
	bool kbqwpvjnjmagh = true;
	int azlmy = 627;
	double ohlgtgzbk = 19650;
	string xgjjdgc = "nbuyqsdihjqggznbculfcpnxpwzrugogrpzlcebarjgoonenabcxnrupccamimseq";
	bool ncruhoby = false;
	if (true != true) {
		int sxgppcg;
		for (sxgppcg = 0; sxgppcg > 0; sxgppcg--) {
			continue;
		}
	}
	if (14717 != 14717) {
		int keknlqjnsq;
		for (keknlqjnsq = 63; keknlqjnsq > 0; keknlqjnsq--) {
			continue;
		}
	}
	if (true != true) {
		int spvlroxwc;
		for (spvlroxwc = 5; spvlroxwc > 0; spvlroxwc--) {
			continue;
		}
	}
	if (627 == 627) {
		int qnzxq;
		for (qnzxq = 96; qnzxq > 0; qnzxq--) {
			continue;
		}
	}
	if (14717 != 14717) {
		int siflsem;
		for (siflsem = 26; siflsem > 0; siflsem--) {
			continue;
		}
	}

}

void crdhgek::wnxhdeuhgiza(int oegijcosxlk) {
	string dtiqaqyea = "hnelemqdtovwaxazzpevgygfkswzdmiwoagbacewfswfhhsjetornrsvsuvktxoenodlgsniyxsjomkzcq";
	double skeoqwqlj = 38106;
	bool evcghuwwiisv = true;
	if (true == true) {
		int yz;
		for (yz = 30; yz > 0; yz--) {
			continue;
		}
	}
	if (string("hnelemqdtovwaxazzpevgygfkswzdmiwoagbacewfswfhhsjetornrsvsuvktxoenodlgsniyxsjomkzcq") != string("hnelemqdtovwaxazzpevgygfkswzdmiwoagbacewfswfhhsjetornrsvsuvktxoenodlgsniyxsjomkzcq")) {
		int hlaiti;
		for (hlaiti = 22; hlaiti > 0; hlaiti--) {
			continue;
		}
	}
	if (38106 == 38106) {
		int jwtpne;
		for (jwtpne = 68; jwtpne > 0; jwtpne--) {
			continue;
		}
	}
	if (38106 != 38106) {
		int ppbhw;
		for (ppbhw = 7; ppbhw > 0; ppbhw--) {
			continue;
		}
	}
	if (string("hnelemqdtovwaxazzpevgygfkswzdmiwoagbacewfswfhhsjetornrsvsuvktxoenodlgsniyxsjomkzcq") != string("hnelemqdtovwaxazzpevgygfkswzdmiwoagbacewfswfhhsjetornrsvsuvktxoenodlgsniyxsjomkzcq")) {
		int dkpmzkxde;
		for (dkpmzkxde = 77; dkpmzkxde > 0; dkpmzkxde--) {
			continue;
		}
	}

}

int crdhgek::bhslbvytaaj(string wdodhg, int siabs, double sahxzbqfkxwp) {
	string vlrqjrx = "ubnfnorczceauiudtygjbygitlwgbq";
	bool mifmdnkojt = true;
	string nqpfaocm = "wkersqtfpnqaqzhdaknhxsvjhdfmlebwsomtkdfmuumdqknqosnswhltjtdlutcbpbykajelhsoufnyhdalvnw";
	string rtnfpwimawmqmb = "gxuzqwyxpzulhpebgyucknkzoyvlgkmfpodixbhsuxmvxwcufvgeqhnugcyjlesrhbqymjtjybyqyww";
	bool vdbnemifnxshhsm = true;
	double fagsj = 15918;
	if (true != true) {
		int pbfcot;
		for (pbfcot = 60; pbfcot > 0; pbfcot--) {
			continue;
		}
	}
	if (string("wkersqtfpnqaqzhdaknhxsvjhdfmlebwsomtkdfmuumdqknqosnswhltjtdlutcbpbykajelhsoufnyhdalvnw") == string("wkersqtfpnqaqzhdaknhxsvjhdfmlebwsomtkdfmuumdqknqosnswhltjtdlutcbpbykajelhsoufnyhdalvnw")) {
		int tkwaftjfs;
		for (tkwaftjfs = 18; tkwaftjfs > 0; tkwaftjfs--) {
			continue;
		}
	}
	if (15918 == 15918) {
		int teuogdiv;
		for (teuogdiv = 7; teuogdiv > 0; teuogdiv--) {
			continue;
		}
	}
	if (true == true) {
		int iky;
		for (iky = 51; iky > 0; iky--) {
			continue;
		}
	}
	if (string("ubnfnorczceauiudtygjbygitlwgbq") == string("ubnfnorczceauiudtygjbygitlwgbq")) {
		int kqdgciblg;
		for (kqdgciblg = 85; kqdgciblg > 0; kqdgciblg--) {
			continue;
		}
	}
	return 95304;
}

int crdhgek::ujkqiflplqtrz(int aecdxmeqqeck, int oakbjexhqq, double pdjdfn) {
	string qcmgptmg = "houisuicgdzkeflocjtizsinrzfpemjkvbxfiwogabahxnsqobwghcmpgbctzyl";
	int tfemroxtkfhayqg = 6592;
	int tkfutwvrnyu = 1030;
	int ikhmc = 5657;
	int fcydfjhdxjhttu = 2024;
	bool exkzahfq = true;
	int umuknfsgadgr = 498;
	double wwzoopdfn = 20593;
	int cyttjdktk = 807;
	bool qpltndllcodf = true;
	if (5657 != 5657) {
		int yqdxlylhr;
		for (yqdxlylhr = 18; yqdxlylhr > 0; yqdxlylhr--) {
			continue;
		}
	}
	return 18879;
}

void crdhgek::jfavijkhnkxj(bool gkelg, bool gercayuktnch) {
	double tndzb = 3629;
	double vsqnkphkkjyrzth = 2235;
	string osrnhxaqasvf = "gtgwn";
	double bzboqzhj = 21147;
	bool icoiwkan = false;
	string oynai = "csmaoxjiblzukqwwzrlajjfpwcriyinmfaeqizwkixmwolkrbptrjfuidvqqodlietzlbxlyvuxnzqcjtptnrtuflxoq";
	bool lalsbtauddple = false;
	int kgwskxtki = 5613;
	bool gprrlocfmqpblza = false;
	int jzncsnxfte = 1811;

}

void crdhgek::bhwshzxfeufexu(bool fqgdoew, string fjkrlap, int hrihtvptscgw, double jhpdbmcgegs, double tzttosmcsw, int hazzbooq) {
	double qeyimmzdzfv = 19561;
	double xjmxjdmxkczym = 71497;
	double jovitekq = 85519;
	string rxdrnomhjpnm = "bnwgyihkvphdhvjjpjtsnekvpiotmgcbuhxkhamarqlbqrdrzhryljgmklpjlpvqbpuvchxhct";
	string pcnjhziwiayi = "uemprfzvusgdzbamfouazxrmfyhonbodsgjtlckqmcqhjyshlhaxgstldanaiklmjbhfabwlxjqzaqaq";
	double ambcgrmghclier = 51002;
	int bmyjtyapno = 2632;
	if (2632 == 2632) {
		int erzlcnrx;
		for (erzlcnrx = 92; erzlcnrx > 0; erzlcnrx--) {
			continue;
		}
	}
	if (19561 == 19561) {
		int fxldksvjug;
		for (fxldksvjug = 79; fxldksvjug > 0; fxldksvjug--) {
			continue;
		}
	}
	if (51002 != 51002) {
		int nn;
		for (nn = 82; nn > 0; nn--) {
			continue;
		}
	}
	if (string("uemprfzvusgdzbamfouazxrmfyhonbodsgjtlckqmcqhjyshlhaxgstldanaiklmjbhfabwlxjqzaqaq") != string("uemprfzvusgdzbamfouazxrmfyhonbodsgjtlckqmcqhjyshlhaxgstldanaiklmjbhfabwlxjqzaqaq")) {
		int bjeohge;
		for (bjeohge = 48; bjeohge > 0; bjeohge--) {
			continue;
		}
	}

}

string crdhgek::hetepybmqhnhigjgjwpzw(bool kguruaepp, double cammafehimps, string iijfl) {
	bool nrstbrsnfp = true;
	bool pxpakpsrfbeopm = true;
	if (true != true) {
		int racxyfqwf;
		for (racxyfqwf = 21; racxyfqwf > 0; racxyfqwf--) {
			continue;
		}
	}
	if (true == true) {
		int vosumjbhk;
		for (vosumjbhk = 33; vosumjbhk > 0; vosumjbhk--) {
			continue;
		}
	}
	return string("mfdsrayxzrzla");
}

double crdhgek::gdajqpfdkluxbrwhxeikgm() {
	int tapdxpk = 3764;
	double mpmwnu = 22090;
	string rchpmargiaw = "pdpqdowaqganpmwkipffmaqgolevhtrvmrcwcsxhaqnsywhdyh";
	string dmggqafvhwaysmo = "jczucnogxdmrunevvryw";
	if (3764 == 3764) {
		int yyziqto;
		for (yyziqto = 58; yyziqto > 0; yyziqto--) {
			continue;
		}
	}
	if (3764 == 3764) {
		int okqtomiljl;
		for (okqtomiljl = 14; okqtomiljl > 0; okqtomiljl--) {
			continue;
		}
	}
	if (3764 != 3764) {
		int za;
		for (za = 24; za > 0; za--) {
			continue;
		}
	}
	return 58111;
}

int crdhgek::qohkgsfhtgpnxgg(bool mnwggviukl, int vnqtpys, double rmmyvmwwpz, int hxaivixtdpmzw, string kaiushbm, string tuydgf, double wwhheojwt, double myhomuqgynayvuu) {
	return 17462;
}

double crdhgek::ukhdyxllxhshwtuchxdqhicdz(string sxmxpjy, bool smtwajpy, bool bnpzgwyepkhdxx, double oodfqkgtgskk, int ewczuiiweeqhs, bool dsbjdzhprjs, string tjizpmjgcsmxly, double axcdzuq, bool qstziv, double qdboriyclaolj) {
	double xrdbuwxvi = 11279;
	int llsujw = 1664;
	double omttxufebfiecaq = 19542;
	double eakzlyk = 403;
	string eouuwvmhh = "sklduwfmyrbuxkuzpujttirefmywebshbx";
	bool ekcpigzexmo = true;
	int osqoxybvpqfbw = 1352;
	int blrqvfkxpso = 4381;
	if (1352 != 1352) {
		int lbfdjxaxn;
		for (lbfdjxaxn = 74; lbfdjxaxn > 0; lbfdjxaxn--) {
			continue;
		}
	}
	if (1352 == 1352) {
		int pdtzwzkt;
		for (pdtzwzkt = 27; pdtzwzkt > 0; pdtzwzkt--) {
			continue;
		}
	}
	if (1352 != 1352) {
		int hk;
		for (hk = 4; hk > 0; hk--) {
			continue;
		}
	}
	if (19542 != 19542) {
		int lrzzgcn;
		for (lrzzgcn = 58; lrzzgcn > 0; lrzzgcn--) {
			continue;
		}
	}
	if (4381 != 4381) {
		int kbpf;
		for (kbpf = 34; kbpf > 0; kbpf--) {
			continue;
		}
	}
	return 23387;
}

bool crdhgek::xrjydmcqrwssjrwpembagzwa(string ktsauu, int qjjeonpbuf, string zodpgyyslofomz, int dsrjypvw, string qmxqmlluey) {
	int hjlgdw = 4443;
	bool gskqxb = false;
	int zxxvzxrdxouyk = 686;
	int nbegiqap = 4108;
	int vnmgsczmtvyz = 2359;
	bool wljhrgcbkjgs = false;
	string yalfjkkrxlfbc = "evanim";
	double ednuulothtb = 26821;
	int lhzwopjj = 3448;
	if (26821 != 26821) {
		int wft;
		for (wft = 19; wft > 0; wft--) {
			continue;
		}
	}
	if (false == false) {
		int ui;
		for (ui = 83; ui > 0; ui--) {
			continue;
		}
	}
	if (false == false) {
		int cfso;
		for (cfso = 84; cfso > 0; cfso--) {
			continue;
		}
	}
	if (false == false) {
		int wctkzgqty;
		for (wctkzgqty = 90; wctkzgqty > 0; wctkzgqty--) {
			continue;
		}
	}
	return false;
}

int crdhgek::dovcuwccipeoejqc(bool ioooqgg, double bjhhiievnbwriqr, bool nhovkhmdkatixhq, bool wfzcevy, int rkeomo, int iarnfrsuisd, bool qsapwcb, bool nxfdnhugj, string riotkg, int lcaclfpiib) {
	string vhadomahrkyytw = "tovjfawzxlaxxotkqcbmwgmmaznhvhzxedeomwiswhkqjdimnuqrwksbxhanfomvaaf";
	double gvexbvz = 12383;
	if (string("tovjfawzxlaxxotkqcbmwgmmaznhvhzxedeomwiswhkqjdimnuqrwksbxhanfomvaaf") != string("tovjfawzxlaxxotkqcbmwgmmaznhvhzxedeomwiswhkqjdimnuqrwksbxhanfomvaaf")) {
		int zkzutratw;
		for (zkzutratw = 72; zkzutratw > 0; zkzutratw--) {
			continue;
		}
	}
	if (string("tovjfawzxlaxxotkqcbmwgmmaznhvhzxedeomwiswhkqjdimnuqrwksbxhanfomvaaf") != string("tovjfawzxlaxxotkqcbmwgmmaznhvhzxedeomwiswhkqjdimnuqrwksbxhanfomvaaf")) {
		int egfysbgedz;
		for (egfysbgedz = 36; egfysbgedz > 0; egfysbgedz--) {
			continue;
		}
	}
	if (12383 == 12383) {
		int sja;
		for (sja = 82; sja > 0; sja--) {
			continue;
		}
	}
	return 1866;
}

int crdhgek::zcbvhfyujkeukapcduypdsu(double pdecptwyiex, string syzfjqkqtjyu, bool rppstqyaivivrnu, string iiptepwivdqz, double yrnnendojmtqvu, string xejermalk, int znuidlbflngerk, double ovwwy, string mifcjw, int tirofzinoit) {
	double yybqvyjzt = 13572;
	string dwepkstqdopa = "kptetzhonybkhwvggnesodrfuaszhlmkiuuxmfzdocblbspcbmnvtcokjchcmpjxaxqnrstpvcobxpuvmqackhmaqlbcriycue";
	int kegghlbcxrl = 2099;
	bool fzeroxqvfijr = false;
	string htfopvvarmykluc = "nshuzkhkfhmjjwudcbdps";
	if (string("nshuzkhkfhmjjwudcbdps") != string("nshuzkhkfhmjjwudcbdps")) {
		int lzimttzg;
		for (lzimttzg = 26; lzimttzg > 0; lzimttzg--) {
			continue;
		}
	}
	if (false == false) {
		int gwmvlj;
		for (gwmvlj = 14; gwmvlj > 0; gwmvlj--) {
			continue;
		}
	}
	if (string("nshuzkhkfhmjjwudcbdps") != string("nshuzkhkfhmjjwudcbdps")) {
		int fadnlltp;
		for (fadnlltp = 79; fadnlltp > 0; fadnlltp--) {
			continue;
		}
	}
	return 31553;
}

int crdhgek::ivxicfphqlxj(int nflgbijl, double wfjdtcexmmc, double lxfrtzinymkpou, double glkgqbxnxxxenj, bool bzfzfudwrgn) {
	bool xotzrp = false;
	string onfsaijp = "eqqkzrmmrrxyquybdudjgm";
	bool aefdfjscdtmzr = false;
	if (false == false) {
		int jd;
		for (jd = 20; jd > 0; jd--) {
			continue;
		}
	}
	if (false == false) {
		int qm;
		for (qm = 13; qm > 0; qm--) {
			continue;
		}
	}
	if (false != false) {
		int ifj;
		for (ifj = 68; ifj > 0; ifj--) {
			continue;
		}
	}
	return 80907;
}

int crdhgek::labezelpatdev(int hddws, bool fxulaeqeabytlg, string uaznunyd, double rxevbyy, bool uuskoiwzqawk) {
	string aibwkee = "uxswkfxdgaezfdqppxmirlgpykhldwumrt";
	bool wpufzgr = false;
	string wcrabdwzm = "bktxzwrdsljzlxizmjmhuwulfbfczlywhmuasbxpxjenrgclfvdjqycpfcpteupd";
	int qltml = 1018;
	if (string("bktxzwrdsljzlxizmjmhuwulfbfczlywhmuasbxpxjenrgclfvdjqycpfcpteupd") == string("bktxzwrdsljzlxizmjmhuwulfbfczlywhmuasbxpxjenrgclfvdjqycpfcpteupd")) {
		int pkprvfazl;
		for (pkprvfazl = 39; pkprvfazl > 0; pkprvfazl--) {
			continue;
		}
	}
	if (string("uxswkfxdgaezfdqppxmirlgpykhldwumrt") != string("uxswkfxdgaezfdqppxmirlgpykhldwumrt")) {
		int jogw;
		for (jogw = 45; jogw > 0; jogw--) {
			continue;
		}
	}
	if (string("bktxzwrdsljzlxizmjmhuwulfbfczlywhmuasbxpxjenrgclfvdjqycpfcpteupd") == string("bktxzwrdsljzlxizmjmhuwulfbfczlywhmuasbxpxjenrgclfvdjqycpfcpteupd")) {
		int jpbr;
		for (jpbr = 3; jpbr > 0; jpbr--) {
			continue;
		}
	}
	if (false == false) {
		int lac;
		for (lac = 47; lac > 0; lac--) {
			continue;
		}
	}
	if (1018 != 1018) {
		int jkjgha;
		for (jkjgha = 61; jkjgha > 0; jkjgha--) {
			continue;
		}
	}
	return 91721;
}

void crdhgek::mxktrebqmycrhuk(string ykcesbxwjoma, double rvzkjdhnmeyhapi, double kmkumwgtvn) {
	string nnlyonrxovloure = "ugsbrmbqopdbteeh";
	double ynnamwyxzsncm = 15345;
	bool xxuevgdyxmgaple = false;

}

string crdhgek::poqomsytijvkgyi() {
	string vdfddbcz = "veatxofumbxhmufikadgxiicnawsfokzdqlww";
	double bhwohtqfow = 76872;
	string clrcbxicmvhas = "ghfwwglemzujdumvriafjxqsdanrizssfzt";
	string qtcgzzrg = "xzjmsfrsddlkwcosbcenngfwtyowjashzwmknjcdyhxzfkfntfojjplijpwd";
	double oyszxdvylm = 4729;
	double hovlwocighpddhv = 7896;
	double hkwhvrsmahymx = 14634;
	if (14634 != 14634) {
		int xcpsisxu;
		for (xcpsisxu = 61; xcpsisxu > 0; xcpsisxu--) {
			continue;
		}
	}
	if (string("xzjmsfrsddlkwcosbcenngfwtyowjashzwmknjcdyhxzfkfntfojjplijpwd") == string("xzjmsfrsddlkwcosbcenngfwtyowjashzwmknjcdyhxzfkfntfojjplijpwd")) {
		int xix;
		for (xix = 28; xix > 0; xix--) {
			continue;
		}
	}
	if (76872 == 76872) {
		int rhbaylais;
		for (rhbaylais = 87; rhbaylais > 0; rhbaylais--) {
			continue;
		}
	}
	if (string("ghfwwglemzujdumvriafjxqsdanrizssfzt") != string("ghfwwglemzujdumvriafjxqsdanrizssfzt")) {
		int bpvcghk;
		for (bpvcghk = 33; bpvcghk > 0; bpvcghk--) {
			continue;
		}
	}
	if (string("ghfwwglemzujdumvriafjxqsdanrizssfzt") == string("ghfwwglemzujdumvriafjxqsdanrizssfzt")) {
		int bu;
		for (bu = 51; bu > 0; bu--) {
			continue;
		}
	}
	return string("omej");
}

crdhgek::crdhgek() {
	this->qohkgsfhtgpnxgg(true, 39, 883, 7380, string("ykmkxscefylynssmfmcvk"), string("jxpevxgkdvcuxkenfdeedvtpp"), 18037, 3316);
	this->ukhdyxllxhshwtuchxdqhicdz(string("vpsxlgefbjqgb"), false, true, 29951, 827, false, string("qjua"), 13462, true, 12437);
	this->xrjydmcqrwssjrwpembagzwa(string("rfimxixqixbkhkolryvzljskwxiafpqreffwdisamrmticjisbipatyeqdhguffxgvzayusinc"), 8486, string("tzjroqhincuftuajjedtnrnaveehnyvirafxrivqis"), 147, string("woigobfxddzlredvlfwi"));
	this->dovcuwccipeoejqc(true, 72960, false, true, 410, 788, false, false, string("hpigvmspbfichmsqibmkawzqozxzwunzqrohfbgmsvjupvgnmfhwfarbwjbqekfovklzilihevjcjzszjivpajmgxqeppw"), 105);
	this->zcbvhfyujkeukapcduypdsu(80510, string("cyhmsuzehwgvflqpzxyehwdnnrnabnnpcdfhlataeuwrg"), true, string("gjwirhtnrckwxuodlbqkiwsljtfcyzfuwoxsjkusisgxqzkhkkflwmofhcytulourwpcfurmslcqjxhypdzaajvcmv"), 39693, string("tnocrauscdvbhimrtksbxrqjszmvom"), 80, 8502, string("hmqdmtxsqrlbxppesfvhdpxwvtkeshchgilxexzubmtqlsagtyzlppjozu"), 2447);
	this->ivxicfphqlxj(1479, 28955, 926, 91139, true);
	this->labezelpatdev(4027, true, string("kppdowzetfknswnsfhvyxgrqgxxawewwamhlbcufnrkzvgqzlwsiz"), 1884, true);
	this->mxktrebqmycrhuk(string("rm"), 34172, 9452);
	this->poqomsytijvkgyi();
	this->jfavijkhnkxj(true, false);
	this->bhwshzxfeufexu(false, string("lkdgbbysiezkczvtbygsygzgwestyhjguvoncmlyubljqyhrbsdombuxscsnsucnmspwoquhbrxjzlpyeq"), 6332, 22637, 51324, 4739);
	this->hetepybmqhnhigjgjwpzw(true, 14804, string("iqbtfxilmb"));
	this->gdajqpfdkluxbrwhxeikgm();
	this->dlbfefrpfyxiq(2307);
	this->xvrjayiklvjrugcoatwskdzyc(3884);
	this->heskquuprgbsqfo(1907, string("omcswccluazyoauzbcjhjmekrxznwxjjlznqbbgyicbsvyqeavsuusexnvavsqzbrjgctntcjsxlwuacvbjqhjsgai"));
	this->wnxhdeuhgiza(3577);
	this->bhslbvytaaj(string("vzoxegwhwjgkgygiuadahqydddedmdgeavmytfbsrhvbrrdggxfgovvcyvrehpdfndzopzxykhbexxyr"), 628, 6218);
	this->ujkqiflplqtrz(52, 2675, 30225);
}


















































































































































































































































































