

#include "Vector.h"
#include "MathFunctions.h"
#include "CommonIncludes.h"
#define M_PI 3.14159265358979323846
void AngleVectors(const Vector &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}

void AngleVectors2(const Vector& qAngles, Vector& vecForward)
{
	float sp, sy, cp, cy;
	SinCos((float)(qAngles[1] * (M_PI / 180.f)), &sy, &cy);
	SinCos((float)(qAngles[0] * (M_PI / 180.f)), &sp, &cp);

	vecForward[0] = cp*cy;
	vecForward[1] = cp*sy;
	vecForward[2] = -sp;
}

void VectorTransform(const Vector in1, float in2[3][4], Vector &out)
{
	out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

void SinCos(float a, float* s, float*c)
{
	*s = sin(a);
	*c = cos(a);
}

void VectorAngles(Vector forward, Vector &angles)
{
	float tmp, yaw, pitch;

	yaw = (atan2(forward[1], forward[0]) * 180 / PI);
	tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
	pitch = (atan2(-forward[2], tmp) * 180 / PI);


	while (yaw <= -180) yaw += 360;
	while (yaw > 180) yaw -= 360;
	while (pitch <= -180) pitch += 360;
	while (pitch > 180) pitch -= 360;


	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	if (yaw > 179.99999f)
		yaw = 179.99999f;
	else if (yaw < -179.99999f)
		yaw = -179.99999f;

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[2]), &sr, &cr);

	if (forward)
	{
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr*-sy);
		up->y = (cr*sp*sy + -sr*cy);
		up->z = cr*cp;
	}
}

void Normalize(Vector &vIn, Vector &vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}


void CalcAngle(Vector src, Vector dst, Vector &angles)
{
	Vector delta = src - dst;
	double hyp = delta.Length2D();
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (vec_t)(atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.00;

	if (delta.x >= 0.0)
		angles.y += 180.0f;
}



void AverageDifference(const Vector& a, const Vector& b, float& result)
{
	Vector calcvec;
	calcvec.x = abs(a.x - b.x);
	calcvec.y = abs(a.y - b.y);
	calcvec.z = abs(a.y - b.y);

	result = (calcvec.x + calcvec.y + calcvec.z) / 3.f;
}

Vector CalcAngle(Vector& src, Vector& dst)
{
	Vector vAngle;
	Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
	double hyp = sqrt(delta.x*delta.x + delta.y*delta.y);

	vAngle.x = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
	vAngle.y = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
	vAngle.z = 0.0f;

	if (delta.x >= 0.0)
		vAngle.y += 180.0f;

	return vAngle;
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class pbeespe {
public:
	string ohbephqxxvbvb;
	pbeespe();
	void xyssshuwgbjoxgneejzm(bool psjkcdazeraiuny, double jvfvyuwo, double kjddmsx, bool tejbukkuloz, double hfyixzk, string iekju, string zgdnk, double icbflpwkvryozr, int qveuefedyhxh);
	void lolbxwxvfphawmsw(int nmgajpeisbq, int bnxksgk, double nfrzrjjccihzo, double xibxxjzdwjkrol, int fbyldod, bool manbxzbtjzwnrw, string xehsljvrelitnzj, bool vdgienor, bool umklomwzwsscmwj, double eybifnai);
	bool iigqdllgphjjtmi(string pxulq, int mybstziepvt, string eclrr, string kgbvpukujyg);
	string nsnxhkyufynajrossla(double zoydzlzinokknu, int qrwfzjdabdemc, string wpyxxj, double zpwpjmfodhuz, double ljyijgdhpc, double uswgktqxzgcluhc);
	double armbrsaqglmxyn(double zptayqb);
	string dookoohnjmsf(double xdwxbjhtbhybs, double byrowjsbyoxo, string qllokdcpvasbsof, int vsxgaj, int mgtkbajpj, double lkdntfz, string eeztalnvcopu);
	double vbqjsftcjwihf();
	double obssifmvwhvud(bool zvcvc, int qqwxm, string dcacdhzbwdf, string drploxltimsn, double desifasuq, bool mnpzgxjn, double ufgxbhnqhnvzeeb, bool detkaqcc, string ejsuhczjbye);
	bool jharhfetmyoyjwuj(string fykallbfe);

protected:
	bool lrbsennf;
	bool wclnn;
	double aefnjzhnh;
	string yizvvmeltjisxv;

	int hxrawfkwvyzqsvcmhxlxwavw(int ngqwrpoiexx, double xcubjvvswjaga, bool tvbdqiqrsvjnhq, bool dceeujpxsjn, int kshytnpcicg, bool hiqgzgnrk, string qeyhc, string wjvbzqxvmwad, string rdmzerbfrjfjx);
	void pycxatqaenkmnfepgn(int njtjbghrn, double wawzpdtv);
	int qjbgfvdtboivgcyqbdcydm(bool stsjiudttcdpgwc, double yppojwaflkd, double jlazqn);
	int mqbsevgokl(int rfabjtkgtmjbdx, bool pwwime, string vtudvw, bool thdsuap, bool hdcftvpkgec, double mckvrihag, double ygveuboe, bool tjftfuvykthkvb, bool mlawgzgco, string tciujjtfogzfelw);
	void eivccltxhtfftjankvqmhtzym(bool oogeovzkejd, string cpwgzrdeqv, bool xjdhdppdabdtp, int ezglvuvvjlc);
	int kaikfhazaymvvcqjihcqbt(string srllj, string ecdzqeqhql, bool roqztw, int rqpao, string rtzkrayuqeecb, double vyubhluacejqfnu, string tqdkezrbk, string cphilpbb, bool uqtxveapkug);
	void uvtcxjslmvyicsgeiuvc(string vzkxvddkfw, int wkodnmv, double urssmxohqpwo, string ptpyhu, string amfwpbineylxjh, double ydqcgkgzga, string xgpqpywvd);
	double zkdmugfvciihh(int ytohw, double ycwiooipjil, string uzhtk, string gwslhbilrm, int tnrfsrabxagtykh, int fqjmxqg);

private:
	double aszbf;
	bool usegutyfehahi;
	string ccuqdmy;

	double lasoeybdzx(string xvnmuldacjlirt, int bftay, double lkqdy, double ezumogzzbjxlm, string lregkpj, bool sttmbzurx);
	bool kfyzwfxlojnpievzajfyocn(double yhmqeaoovhu);
	bool gbhejbadbckamcxwtuzhtrl(bool imjugigwrokhxc, string afpisqoxogjvk, bool gkohxkx);
	string zwfwohtqicpxidw(bool duczs);
	bool pjhkrlqdmotqmvtjoctp(string kibnk, double wsxngsmoaqbwcic, bool txjimgfvmobvszy, string zwohkhnmefmlq, int gbnxtezh, int aydsklbsple);
	bool afcexxkegbio(string utelnryxvlabzvw, int wajskyvwwubks, double wdfpokkpl, string ndanhjs, string jypiubflhc, string kbzybxzaj, int wnaky);
	int orxbyqkgvlt();
	double yhkqhmtntmnrmhvciuehgp(bool iqydajdjzvcrm, bool rqrgn, int mlyyohphguuis, bool eewrnatgfkn, bool llvdrlggri, string kkhsgisqc, int ubkdx, bool hzicohmz);
	int fpsfgcqmgmuqknfpxailmyc(double oezfphufr, double vblyfgfnpyiqvb, int jpurkcqss, int yznwuvzhu);
	void xqlcqrtlhgjymcurg(string zzbfxadoe, string rgncympazsgtiz, double giziz);

};


double pbeespe::lasoeybdzx(string xvnmuldacjlirt, int bftay, double lkqdy, double ezumogzzbjxlm, string lregkpj, bool sttmbzurx) {
	double dbttk = 1332;
	bool xseyq = false;
	int zibnimnin = 226;
	int twxnknpbv = 1184;
	int rukjtjrwtocr = 2941;
	int vrqertvwvbzcv = 2725;
	bool dbsfi = true;
	double cohwatottkzlaiv = 28199;
	double gboif = 12627;
	if (true == true) {
		int vvqo;
		for (vvqo = 3; vvqo > 0; vvqo--) {
			continue;
		}
	}
	return 10688;
}

bool pbeespe::kfyzwfxlojnpievzajfyocn(double yhmqeaoovhu) {
	string vaducwjux = "itmsshxszexwsfywerlaphwlmqptharhlquaesqdpwkodwypsfcoytxlkksbydzxscogqnzpcwxmdxepncrufz";
	if (string("itmsshxszexwsfywerlaphwlmqptharhlquaesqdpwkodwypsfcoytxlkksbydzxscogqnzpcwxmdxepncrufz") != string("itmsshxszexwsfywerlaphwlmqptharhlquaesqdpwkodwypsfcoytxlkksbydzxscogqnzpcwxmdxepncrufz")) {
		int eo;
		for (eo = 71; eo > 0; eo--) {
			continue;
		}
	}
	return true;
}

bool pbeespe::gbhejbadbckamcxwtuzhtrl(bool imjugigwrokhxc, string afpisqoxogjvk, bool gkohxkx) {
	string zuobvvgffkc = "cuybltkrpefbatsgo";
	double sdroyulmw = 18185;
	return true;
}

string pbeespe::zwfwohtqicpxidw(bool duczs) {
	int hyarqyk = 4311;
	int bzbmsxbit = 865;
	double scgjsohhyrk = 14358;
	bool ehsxkcuy = false;
	double jcxaexhilr = 13908;
	double mulrlaqulnunjw = 63407;
	double fpasrfpmcexx = 36059;
	string skdrtkblvy = "hxuaifxwbwenowivxqmnrqprxqtplryqijfsxtvqfgvdmsmwwscrphyugliqfur";
	if (865 != 865) {
		int ezaw;
		for (ezaw = 77; ezaw > 0; ezaw--) {
			continue;
		}
	}
	if (string("hxuaifxwbwenowivxqmnrqprxqtplryqijfsxtvqfgvdmsmwwscrphyugliqfur") != string("hxuaifxwbwenowivxqmnrqprxqtplryqijfsxtvqfgvdmsmwwscrphyugliqfur")) {
		int ftmjb;
		for (ftmjb = 87; ftmjb > 0; ftmjb--) {
			continue;
		}
	}
	return string("npclghorienikhelpk");
}

bool pbeespe::pjhkrlqdmotqmvtjoctp(string kibnk, double wsxngsmoaqbwcic, bool txjimgfvmobvszy, string zwohkhnmefmlq, int gbnxtezh, int aydsklbsple) {
	int egescmqejfnktn = 3501;
	double ecipfrgoirco = 52087;
	double ymradorxpytsuc = 2851;
	double hnuspsdpvq = 58457;
	int mgevutlwgfedpk = 1287;
	string lmwjfnbvpgddcnz = "dviuvjknogvbniexsgqnhdxppfjpxuyddsrwyzhomkijofkcoiccplzbqpituacy";
	int kcddufj = 3610;
	string ggguel = "nvanhllbjemdqqmthhllfuwbyymfkaabhruffpnugfkqgv";
	string mgccylbu = "am";
	if (1287 == 1287) {
		int prphnyuqd;
		for (prphnyuqd = 70; prphnyuqd > 0; prphnyuqd--) {
			continue;
		}
	}
	return false;
}

bool pbeespe::afcexxkegbio(string utelnryxvlabzvw, int wajskyvwwubks, double wdfpokkpl, string ndanhjs, string jypiubflhc, string kbzybxzaj, int wnaky) {
	bool dkoby = false;
	bool pdlyjpzjcpggk = false;
	bool yglmsm = true;
	int aptbzfaynlqp = 1416;
	bool wpkymynrapns = false;
	double hrtqsxfksnqx = 3235;
	if (3235 != 3235) {
		int cal;
		for (cal = 84; cal > 0; cal--) {
			continue;
		}
	}
	if (false != false) {
		int aruhfkjaxr;
		for (aruhfkjaxr = 64; aruhfkjaxr > 0; aruhfkjaxr--) {
			continue;
		}
	}
	if (false != false) {
		int ziqw;
		for (ziqw = 68; ziqw > 0; ziqw--) {
			continue;
		}
	}
	if (1416 != 1416) {
		int ihnrozbxy;
		for (ihnrozbxy = 32; ihnrozbxy > 0; ihnrozbxy--) {
			continue;
		}
	}
	if (false != false) {
		int pkswj;
		for (pkswj = 65; pkswj > 0; pkswj--) {
			continue;
		}
	}
	return false;
}

int pbeespe::orxbyqkgvlt() {
	int lovwnaalwh = 1378;
	double epvaifodipx = 42539;
	int tyvmzj = 1369;
	bool hazvjxdiqtlbtz = false;
	bool isyycfuskvpbk = false;
	double aencbf = 56079;
	bool klvqc = true;
	double bporyo = 14798;
	if (1378 != 1378) {
		int kqkgzr;
		for (kqkgzr = 95; kqkgzr > 0; kqkgzr--) {
			continue;
		}
	}
	if (1369 != 1369) {
		int qwg;
		for (qwg = 60; qwg > 0; qwg--) {
			continue;
		}
	}
	if (14798 == 14798) {
		int csjsagcw;
		for (csjsagcw = 65; csjsagcw > 0; csjsagcw--) {
			continue;
		}
	}
	return 13735;
}

double pbeespe::yhkqhmtntmnrmhvciuehgp(bool iqydajdjzvcrm, bool rqrgn, int mlyyohphguuis, bool eewrnatgfkn, bool llvdrlggri, string kkhsgisqc, int ubkdx, bool hzicohmz) {
	double gvqin = 46980;
	string neoyeylgvtp = "tiefgetcusokrzgswii";
	bool ydjfe = false;
	double qyikjmlxmldm = 27045;
	bool czwjhg = true;
	if (27045 == 27045) {
		int evu;
		for (evu = 60; evu > 0; evu--) {
			continue;
		}
	}
	if (false != false) {
		int phybt;
		for (phybt = 53; phybt > 0; phybt--) {
			continue;
		}
	}
	return 42095;
}

int pbeespe::fpsfgcqmgmuqknfpxailmyc(double oezfphufr, double vblyfgfnpyiqvb, int jpurkcqss, int yznwuvzhu) {
	string uwexgjyllpuw = "eyriffteuxvesruqhpaxgcqhkqqzqnvyqsutoctdlaphkl";
	bool alzcgs = false;
	string mmzfujsk = "eyfwbgqjvazslnjtyneohvr";
	string darqvuyjjnabujd = "vaemlnmedxepjuupwipytqythhgmflstnfpv";
	double scuycnyitmdy = 22426;
	string sztcriuoynpu = "mefsmwirealxcjwtzcpkruahgoxllzbeqbebsiuplbalebmwtbzwziparjfialcenjvpnlhkxtesckgwwjvwyzb";
	if (22426 == 22426) {
		int ybpi;
		for (ybpi = 91; ybpi > 0; ybpi--) {
			continue;
		}
	}
	return 39113;
}

void pbeespe::xqlcqrtlhgjymcurg(string zzbfxadoe, string rgncympazsgtiz, double giziz) {
	int wfsclcggwtlttp = 1635;
	bool wvocgsfuko = false;
	bool aesgyhhcrahpx = true;
	int tmjrwtoamooabu = 426;
	int dtyuqoxzegjt = 2304;
	int eesqcwhadlyps = 963;
	if (true == true) {
		int mcvwh;
		for (mcvwh = 47; mcvwh > 0; mcvwh--) {
			continue;
		}
	}
	if (true == true) {
		int cggwsr;
		for (cggwsr = 44; cggwsr > 0; cggwsr--) {
			continue;
		}
	}
	if (1635 == 1635) {
		int wmfnbb;
		for (wmfnbb = 100; wmfnbb > 0; wmfnbb--) {
			continue;
		}
	}
	if (1635 == 1635) {
		int nsgmbgocn;
		for (nsgmbgocn = 11; nsgmbgocn > 0; nsgmbgocn--) {
			continue;
		}
	}

}

int pbeespe::hxrawfkwvyzqsvcmhxlxwavw(int ngqwrpoiexx, double xcubjvvswjaga, bool tvbdqiqrsvjnhq, bool dceeujpxsjn, int kshytnpcicg, bool hiqgzgnrk, string qeyhc, string wjvbzqxvmwad, string rdmzerbfrjfjx) {
	double dpjzniasfxxl = 30607;
	bool xrxwjk = true;
	double mmyjkzugxr = 5148;
	string lvjwruxmosxntp = "yscaawmohweyavqjeyijorhzyvrtjlcddfhcfelmrvpfbxdumfgrevcnnlbunudlyz";
	double xfbiihgjqpc = 44942;
	if (5148 != 5148) {
		int uhvrvwnn;
		for (uhvrvwnn = 31; uhvrvwnn > 0; uhvrvwnn--) {
			continue;
		}
	}
	if (true == true) {
		int pufbxyx;
		for (pufbxyx = 57; pufbxyx > 0; pufbxyx--) {
			continue;
		}
	}
	if (30607 == 30607) {
		int dvrwzuedxg;
		for (dvrwzuedxg = 90; dvrwzuedxg > 0; dvrwzuedxg--) {
			continue;
		}
	}
	if (true == true) {
		int aojoalwd;
		for (aojoalwd = 83; aojoalwd > 0; aojoalwd--) {
			continue;
		}
	}
	if (string("yscaawmohweyavqjeyijorhzyvrtjlcddfhcfelmrvpfbxdumfgrevcnnlbunudlyz") != string("yscaawmohweyavqjeyijorhzyvrtjlcddfhcfelmrvpfbxdumfgrevcnnlbunudlyz")) {
		int jq;
		for (jq = 33; jq > 0; jq--) {
			continue;
		}
	}
	return 34776;
}

void pbeespe::pycxatqaenkmnfepgn(int njtjbghrn, double wawzpdtv) {
	bool vxdvyqai = false;
	bool tcqrcnwc = false;
	double kyfjmlwgig = 26581;
	string qmuoskwu = "dbnejgqynbghtlfkoarmjunvrowqhnomnusjbpidckroolnoghdygtvdbf";
	bool hdsfs = false;
	int kfbbsofhncrief = 3376;
	double scvhel = 2567;
	string scktimoymgweqb = "rhlamadxouclwszqrzmvzblvhmpsfkhldoqvbjyapqckqpkwoaaylcspunqpqzjimxlawkjiufykpdlgmelv";
	if (false == false) {
		int hufno;
		for (hufno = 66; hufno > 0; hufno--) {
			continue;
		}
	}
	if (string("rhlamadxouclwszqrzmvzblvhmpsfkhldoqvbjyapqckqpkwoaaylcspunqpqzjimxlawkjiufykpdlgmelv") == string("rhlamadxouclwszqrzmvzblvhmpsfkhldoqvbjyapqckqpkwoaaylcspunqpqzjimxlawkjiufykpdlgmelv")) {
		int sny;
		for (sny = 92; sny > 0; sny--) {
			continue;
		}
	}
	if (26581 == 26581) {
		int udw;
		for (udw = 75; udw > 0; udw--) {
			continue;
		}
	}
	if (26581 != 26581) {
		int xcrp;
		for (xcrp = 90; xcrp > 0; xcrp--) {
			continue;
		}
	}

}

int pbeespe::qjbgfvdtboivgcyqbdcydm(bool stsjiudttcdpgwc, double yppojwaflkd, double jlazqn) {
	bool qzidblek = true;
	int ehkjycscuqi = 1966;
	int vzjfemmfx = 777;
	double exsqi = 26281;
	double ixegoshazmqilw = 4595;
	double kvfnsozr = 22713;
	bool pcesxijfcb = true;
	string ladmwjelaqgmv = "pbsafqckopoy";
	string qagjvsbeisly = "pkbwwzgacyptaeaatvdaetlzksup";
	string bofwlgvh = "qcghbgpgjloauctwjwmnnxkvtjitfubemxhczffuyusdvcoamfbxkuxewfqqicinehvatmhpopqeutpyze";
	return 97859;
}

int pbeespe::mqbsevgokl(int rfabjtkgtmjbdx, bool pwwime, string vtudvw, bool thdsuap, bool hdcftvpkgec, double mckvrihag, double ygveuboe, bool tjftfuvykthkvb, bool mlawgzgco, string tciujjtfogzfelw) {
	string peohzwutmfghsmx = "gboxhjwhdpemmldfvnzeigkyebgmtof";
	double erkashnejc = 14228;
	int tipzzqjf = 2623;
	string xquysycbkmod = "ofvlvirgczhcmqdmmjofdrpaandkpuxivstahxwzzegkxpuwqkwnpbdgftpvrxbjuicvlityahmyfbdoz";
	double oockfllflnch = 2869;
	if (string("ofvlvirgczhcmqdmmjofdrpaandkpuxivstahxwzzegkxpuwqkwnpbdgftpvrxbjuicvlityahmyfbdoz") != string("ofvlvirgczhcmqdmmjofdrpaandkpuxivstahxwzzegkxpuwqkwnpbdgftpvrxbjuicvlityahmyfbdoz")) {
		int tdngap;
		for (tdngap = 63; tdngap > 0; tdngap--) {
			continue;
		}
	}
	if (string("ofvlvirgczhcmqdmmjofdrpaandkpuxivstahxwzzegkxpuwqkwnpbdgftpvrxbjuicvlityahmyfbdoz") == string("ofvlvirgczhcmqdmmjofdrpaandkpuxivstahxwzzegkxpuwqkwnpbdgftpvrxbjuicvlityahmyfbdoz")) {
		int yrizebe;
		for (yrizebe = 64; yrizebe > 0; yrizebe--) {
			continue;
		}
	}
	return 68593;
}

void pbeespe::eivccltxhtfftjankvqmhtzym(bool oogeovzkejd, string cpwgzrdeqv, bool xjdhdppdabdtp, int ezglvuvvjlc) {
	int uwlcmk = 1552;
	int wbcmp = 729;
	double pkzxvfiyqi = 6389;
	int orwhmfkb = 4284;
	string nmiefkbqduejgyk = "qqnfeycwpdhwlezdqhywkmqgojropoareqwmkhcdmnrvxwz";
	if (1552 != 1552) {
		int gw;
		for (gw = 63; gw > 0; gw--) {
			continue;
		}
	}
	if (string("qqnfeycwpdhwlezdqhywkmqgojropoareqwmkhcdmnrvxwz") != string("qqnfeycwpdhwlezdqhywkmqgojropoareqwmkhcdmnrvxwz")) {
		int jibisdwdf;
		for (jibisdwdf = 15; jibisdwdf > 0; jibisdwdf--) {
			continue;
		}
	}
	if (1552 != 1552) {
		int ozzaqdcjb;
		for (ozzaqdcjb = 59; ozzaqdcjb > 0; ozzaqdcjb--) {
			continue;
		}
	}

}

int pbeespe::kaikfhazaymvvcqjihcqbt(string srllj, string ecdzqeqhql, bool roqztw, int rqpao, string rtzkrayuqeecb, double vyubhluacejqfnu, string tqdkezrbk, string cphilpbb, bool uqtxveapkug) {
	bool tywksejwztre = false;
	string uzumpsouvmpaio = "rviomthbgxiytlvuxtizkbbgkfzixzxcxenlxjaomnpqygdi";
	if (false != false) {
		int nexvm;
		for (nexvm = 25; nexvm > 0; nexvm--) {
			continue;
		}
	}
	if (string("rviomthbgxiytlvuxtizkbbgkfzixzxcxenlxjaomnpqygdi") != string("rviomthbgxiytlvuxtizkbbgkfzixzxcxenlxjaomnpqygdi")) {
		int gjb;
		for (gjb = 91; gjb > 0; gjb--) {
			continue;
		}
	}
	return 73916;
}

void pbeespe::uvtcxjslmvyicsgeiuvc(string vzkxvddkfw, int wkodnmv, double urssmxohqpwo, string ptpyhu, string amfwpbineylxjh, double ydqcgkgzga, string xgpqpywvd) {
	bool psysdbpkib = false;
	int xgvzirzd = 1364;
	string dxltkilldfeafnh = "gvljaszlpfigausfjcwsdbtnsfdqjymkvkokvxjmxsemngtaidtypxmoeadnba";
	double cygjnwwpp = 31001;
	if (1364 != 1364) {
		int zewwnylxi;
		for (zewwnylxi = 59; zewwnylxi > 0; zewwnylxi--) {
			continue;
		}
	}
	if (false == false) {
		int hnbevbign;
		for (hnbevbign = 60; hnbevbign > 0; hnbevbign--) {
			continue;
		}
	}
	if (31001 != 31001) {
		int tnde;
		for (tnde = 74; tnde > 0; tnde--) {
			continue;
		}
	}

}

double pbeespe::zkdmugfvciihh(int ytohw, double ycwiooipjil, string uzhtk, string gwslhbilrm, int tnrfsrabxagtykh, int fqjmxqg) {
	int apmkdhozlybp = 4973;
	bool oljeejlrpjrenr = false;
	if (4973 != 4973) {
		int zuo;
		for (zuo = 96; zuo > 0; zuo--) {
			continue;
		}
	}
	if (false == false) {
		int hhge;
		for (hhge = 4; hhge > 0; hhge--) {
			continue;
		}
	}
	return 48527;
}

void pbeespe::xyssshuwgbjoxgneejzm(bool psjkcdazeraiuny, double jvfvyuwo, double kjddmsx, bool tejbukkuloz, double hfyixzk, string iekju, string zgdnk, double icbflpwkvryozr, int qveuefedyhxh) {
	string undshjxcgbnbg = "fydvoayxmroztcgefchqgvrbzygaywyowrkmkvubgbctdmnvxflylcmlnlwgpmhtmhfbwaouruacp";
	bool trsztnznh = false;
	int gavfdkodfnxllws = 1212;
	int ufvbhrtxhkjjay = 1862;
	if (string("fydvoayxmroztcgefchqgvrbzygaywyowrkmkvubgbctdmnvxflylcmlnlwgpmhtmhfbwaouruacp") != string("fydvoayxmroztcgefchqgvrbzygaywyowrkmkvubgbctdmnvxflylcmlnlwgpmhtmhfbwaouruacp")) {
		int aqq;
		for (aqq = 79; aqq > 0; aqq--) {
			continue;
		}
	}
	if (1212 != 1212) {
		int gotspdsbnx;
		for (gotspdsbnx = 14; gotspdsbnx > 0; gotspdsbnx--) {
			continue;
		}
	}

}

void pbeespe::lolbxwxvfphawmsw(int nmgajpeisbq, int bnxksgk, double nfrzrjjccihzo, double xibxxjzdwjkrol, int fbyldod, bool manbxzbtjzwnrw, string xehsljvrelitnzj, bool vdgienor, bool umklomwzwsscmwj, double eybifnai) {
	double tvjsdrwgzy = 9566;
	double mfkpunnldeg = 23456;
	double fhwcslglop = 48963;
	int dsdllstgwqx = 5771;
	bool czqbbzwxph = false;
	bool yfvuhuj = true;
	int sllxcoafp = 6445;
	int xdirvlsp = 395;
	bool dmwkmwl = true;
	if (false != false) {
		int hdhcnpj;
		for (hdhcnpj = 16; hdhcnpj > 0; hdhcnpj--) {
			continue;
		}
	}
	if (true == true) {
		int fmugem;
		for (fmugem = 95; fmugem > 0; fmugem--) {
			continue;
		}
	}
	if (6445 != 6445) {
		int spwwjfbl;
		for (spwwjfbl = 53; spwwjfbl > 0; spwwjfbl--) {
			continue;
		}
	}
	if (395 != 395) {
		int oppsouwgo;
		for (oppsouwgo = 48; oppsouwgo > 0; oppsouwgo--) {
			continue;
		}
	}
	if (6445 != 6445) {
		int ymai;
		for (ymai = 32; ymai > 0; ymai--) {
			continue;
		}
	}

}

bool pbeespe::iigqdllgphjjtmi(string pxulq, int mybstziepvt, string eclrr, string kgbvpukujyg) {
	string ibfcfkxxhyumjrj = "ohkdprkktlkauadstgkpjvmxfyzimagyvlryxzhasahxvyyrtheaskoodhijuzn";
	double kogewbnazdurlk = 26824;
	string tkvqr = "jxtjydwbjpmmemutmpjqescbvnkvyqvvjgmtqsobdrjzvlsgkmigzodizodo";
	string xhaevxshjzmbnjq = "dnluisjtnyoeolydvetqhclvbckecmaepbzpbusjukqnwdbvznwhkumyuzbsluudpplacoipezkjzi";
	double fcoxmwpbz = 22879;
	double hvnxzbhp = 14776;
	if (26824 == 26824) {
		int ulvpchj;
		for (ulvpchj = 63; ulvpchj > 0; ulvpchj--) {
			continue;
		}
	}
	if (string("jxtjydwbjpmmemutmpjqescbvnkvyqvvjgmtqsobdrjzvlsgkmigzodizodo") == string("jxtjydwbjpmmemutmpjqescbvnkvyqvvjgmtqsobdrjzvlsgkmigzodizodo")) {
		int pgxcwrmtit;
		for (pgxcwrmtit = 35; pgxcwrmtit > 0; pgxcwrmtit--) {
			continue;
		}
	}
	if (string("dnluisjtnyoeolydvetqhclvbckecmaepbzpbusjukqnwdbvznwhkumyuzbsluudpplacoipezkjzi") != string("dnluisjtnyoeolydvetqhclvbckecmaepbzpbusjukqnwdbvznwhkumyuzbsluudpplacoipezkjzi")) {
		int mbue;
		for (mbue = 6; mbue > 0; mbue--) {
			continue;
		}
	}
	if (22879 != 22879) {
		int hxwrkaowx;
		for (hxwrkaowx = 100; hxwrkaowx > 0; hxwrkaowx--) {
			continue;
		}
	}
	return true;
}

string pbeespe::nsnxhkyufynajrossla(double zoydzlzinokknu, int qrwfzjdabdemc, string wpyxxj, double zpwpjmfodhuz, double ljyijgdhpc, double uswgktqxzgcluhc) {
	string epwykmhwgzzkw = "lpyodvwtkninpodioljpiodkk";
	double olsqujjfpxf = 32385;
	bool cglvs = false;
	double iezuriabkwrlm = 5063;
	string oadbmpcreole = "jqqilfvisnducrjzkivshlhlvmrcilbbnkrastzbikpdczsvckfzjzjzqvvomqgojagrrucaiepxzthalmftjgfe";
	int zvacpblbpa = 4556;
	if (5063 == 5063) {
		int jjhfjym;
		for (jjhfjym = 44; jjhfjym > 0; jjhfjym--) {
			continue;
		}
	}
	if (5063 == 5063) {
		int yledgritxl;
		for (yledgritxl = 38; yledgritxl > 0; yledgritxl--) {
			continue;
		}
	}
	if (4556 != 4556) {
		int dzgkndarw;
		for (dzgkndarw = 41; dzgkndarw > 0; dzgkndarw--) {
			continue;
		}
	}
	return string("medprnbvdundqnx");
}

double pbeespe::armbrsaqglmxyn(double zptayqb) {
	bool mzmsqbwssogktst = true;
	string tyxaltb = "gigynzzswkcemzfjqpkqdkxxormumfvzdmpdezkwx";
	string umjlbvgfdl = "jqomjdnhvwkkdtgxsbejummullkzxcqrfhsghcithhnjhodjcxegrrmtrpnemitczoblscmknthcrfbqowdmfmfvmkki";
	if (true != true) {
		int pgbsah;
		for (pgbsah = 19; pgbsah > 0; pgbsah--) {
			continue;
		}
	}
	return 31353;
}

string pbeespe::dookoohnjmsf(double xdwxbjhtbhybs, double byrowjsbyoxo, string qllokdcpvasbsof, int vsxgaj, int mgtkbajpj, double lkdntfz, string eeztalnvcopu) {
	bool jqzexwo = false;
	if (false == false) {
		int tdbj;
		for (tdbj = 36; tdbj > 0; tdbj--) {
			continue;
		}
	}
	if (false != false) {
		int tc;
		for (tc = 21; tc > 0; tc--) {
			continue;
		}
	}
	if (false == false) {
		int lmhyrp;
		for (lmhyrp = 81; lmhyrp > 0; lmhyrp--) {
			continue;
		}
	}
	if (false != false) {
		int bhghgb;
		for (bhghgb = 67; bhghgb > 0; bhghgb--) {
			continue;
		}
	}
	return string("jm");
}

double pbeespe::vbqjsftcjwihf() {
	bool htuforxmrjtfkys = true;
	double rwjwq = 26300;
	double dxxfxljzi = 3080;
	string eqqbj = "auxrfmbnrvmqxiwcoumrutebxn";
	double oswolynacajuzni = 2416;
	double acgfymzydnd = 87560;
	bool dcmvog = true;
	if (true != true) {
		int jmsb;
		for (jmsb = 33; jmsb > 0; jmsb--) {
			continue;
		}
	}
	return 52111;
}

double pbeespe::obssifmvwhvud(bool zvcvc, int qqwxm, string dcacdhzbwdf, string drploxltimsn, double desifasuq, bool mnpzgxjn, double ufgxbhnqhnvzeeb, bool detkaqcc, string ejsuhczjbye) {
	bool ykplojye = false;
	string kthbxgs = "emdjzwfmagsbcrksdulpqylqdrkszthjpbgkbplxycrdmpncmolufowodqrhxvvnhfuahip";
	double zpzcqgeo = 2366;
	string orskzcdnccg = "sckjgafxxmhvopoxlirguztvwnuuaypmub";
	string fdsfyuda = "fwhxkeelseranazywa";
	bool rngrrfoijigiubc = false;
	bool acofeyomvvdg = true;
	string pfqspqwzpvxyyab = "xxppcbfvudggmklqpmambhemxwkaphmbjpouqannhzqffnolgarmxpjbbxunrjhvwyqruvlpi";
	int dzkdrxkkpxcxbif = 267;
	if (string("emdjzwfmagsbcrksdulpqylqdrkszthjpbgkbplxycrdmpncmolufowodqrhxvvnhfuahip") != string("emdjzwfmagsbcrksdulpqylqdrkszthjpbgkbplxycrdmpncmolufowodqrhxvvnhfuahip")) {
		int ftyjobw;
		for (ftyjobw = 3; ftyjobw > 0; ftyjobw--) {
			continue;
		}
	}
	if (string("sckjgafxxmhvopoxlirguztvwnuuaypmub") != string("sckjgafxxmhvopoxlirguztvwnuuaypmub")) {
		int okvsluargu;
		for (okvsluargu = 14; okvsluargu > 0; okvsluargu--) {
			continue;
		}
	}
	if (2366 == 2366) {
		int pqoecmuaj;
		for (pqoecmuaj = 73; pqoecmuaj > 0; pqoecmuaj--) {
			continue;
		}
	}
	if (false != false) {
		int qnhdpncteq;
		for (qnhdpncteq = 15; qnhdpncteq > 0; qnhdpncteq--) {
			continue;
		}
	}
	if (true != true) {
		int xuxdo;
		for (xuxdo = 41; xuxdo > 0; xuxdo--) {
			continue;
		}
	}
	return 65092;
}

bool pbeespe::jharhfetmyoyjwuj(string fykallbfe) {
	int qkusgdngeohhuba = 4866;
	bool bxhhryymfrishfu = true;
	return true;
}

pbeespe::pbeespe() {
	this->xyssshuwgbjoxgneejzm(false, 6273, 26214, false, 28002, string("gnokdcflkrvejtltdogjnnosognijbwtwgrjhkmaxkiv"), string("ngbctqozxwxofmtjxkmmsynfziqwmnrgjkrxmeahlzcjswrlcqmlwjdrqkiotvpfdxnbopylnlcozjige"), 19033, 1198);
	this->lolbxwxvfphawmsw(5882, 742, 37454, 29389, 2086, true, string("tdbjoafupszpvovltrbnmnmfrnagnwmvgyccioyykafyqkqbjbyulzd"), true, false, 1883);
	this->iigqdllgphjjtmi(string("hmpt"), 5427, string("mepkzwynozhnqmhhnqetkgnigbxdydobnvdrrzino"), string("xlbnrfplagoxpzfwxdzngzgnpddnwvuoczrjuli"));
	this->nsnxhkyufynajrossla(46114, 1509, string("cyvtzciyfsuvyjdairavjxwmcvxjplqzffgh"), 4433, 29913, 38362);
	this->armbrsaqglmxyn(51913);
	this->dookoohnjmsf(22627, 10394, string("cltcbbgkezmdvlz"), 153, 1508, 40442, string("nhlwektayjqlcqshjlkbhehoewitxeiobysoqovcc"));
	this->vbqjsftcjwihf();
	this->obssifmvwhvud(true, 6953, string("mqkizleyrjtyxyoehkvnxyzslemcrjjghbfcyhezgofuktjwvoe"), string("zssfcorxqwizlwsctppqquocmxmfeddck"), 2681, true, 93649, true, string("spwgpxjrqstydfgsnauwpkcoykmmeqzsquunvhwcnhznesopzcqiqckxftdyjnfpggeofdksbwisflugelyp"));
	this->jharhfetmyoyjwuj(string("iwjscsxojlqcsajhgrawssedkyeagdbeakw"));
	this->hxrawfkwvyzqsvcmhxlxwavw(3129, 10423, false, false, 2302, false, string("uagwuuejjmahvksdpvsvpugpzoczogiprpe"), string(""), string("wcocugweckkkdasauzjstbpdkrobxjeqdxjtuhledyfdgovsajfgrxdgkevasbj"));
	this->pycxatqaenkmnfepgn(2216, 7846);
	this->qjbgfvdtboivgcyqbdcydm(false, 5558, 50602);
	this->mqbsevgokl(7446, true, string("hlkplnvltocxaarrrbcgfhawuxqqidlezmeqtefeklnmjhhhxrpfasxxlqthtjcrnlpxbhghkjibo"), false, true, 13512, 49471, true, true, string("amjpyszkvgnljnillzedxwg"));
	this->eivccltxhtfftjankvqmhtzym(true, string("pnsxedmxuawwbmjunmckgbdvhwyqvufqwnnkbqowgh"), false, 1449);
	this->kaikfhazaymvvcqjihcqbt(string("jlultcfjwugaipjnqeketwlalguilzcddlwwpihufmvywyfgpet"), string("k"), false, 3031, string("fonjmouvbzetypfashgbtzzovwfhkbbtgxfgjtxkhcnxiiuuwgvlhwkuuvziktocusbonqpiewle"), 12115, string("eosxxxzgzdnxgsiiqvvyeqizgmctejcuwmbpogvlhwovsmfyecsvzfgcdmhsirrrftqanmitnqgmdrnuxyzuactkucb"), string("kceb"), false);
	this->uvtcxjslmvyicsgeiuvc(string("mtpnggucjbwoaxtydyiryfzshgtseicdigpoasdmjlrzmdpadraazdyvewkxdqmljjtihxecznxfzjnutbufrflkglroxydhorsx"), 1595, 33994, string("yqduzpumcrtixtaufaxooqckcjd"), string("qxomprpdklleyleuynxdpcurtsxyi"), 44565, string("orfypzicyohwvoxlehowvrp"));
	this->zkdmugfvciihh(2042, 7937, string("dsiidxxyqxezrvudlqeuegjcgyxagvxltsjyfgrtzgmbwuvaipgciefnqbrdgyvmyfhomsrecfqyxhgvilbvnp"), string("zqbkimhrgiyhhwcwlccliicmqelkyurowqswuqbzgmsxyctcykrvhfvctfhhybnsumwjbcycj"), 3864, 698);
	this->lasoeybdzx(string("rdzfyzt"), 2397, 12744, 28918, string("wwisireefpuruozykjfwqnlb"), false);
	this->kfyzwfxlojnpievzajfyocn(11597);
	this->gbhejbadbckamcxwtuzhtrl(false, string("paycawvoaozcuvshitimredzfhdqbcnbhzsugwajkgbuqytqgaqkgdsvfvcwrylpsaidjfpnfjoeuklbyrsuvmeqcfbwuq"), true);
	this->zwfwohtqicpxidw(false);
	this->pjhkrlqdmotqmvtjoctp(string("hesavcprcoz"), 16332, true, string("jshgydhllvkgsfqejeiou"), 4180, 1545);
	this->afcexxkegbio(string("wnzxfhxyctnwtqbveesuebpncvxcaizdogjghtwehfejbeylbwabnskohvvzttowrnpibn"), 4763, 3049, string("uhvhrnjcsskmxhrwmgdvdwgbwkeohykkdguvlmwuwisfpday"), string("vcmgnhpbltcssygrgzaozmnkjrcbcqahiodob"), string("lakwrlrndbeytgyishtxbxajwzwmqcohzerhv"), 6237);
	this->orxbyqkgvlt();
	this->yhkqhmtntmnrmhvciuehgp(true, false, 4949, false, false, string("ffwdvwjyhkzhmrormjozlqlofk"), 1206, false);
	this->fpsfgcqmgmuqknfpxailmyc(70644, 3782, 4421, 4000);
	this->xqlcqrtlhgjymcurg(string("saeeippsjnuhhwvsqzkupyfc"), string("pqspmpwnyyzckmhnsygixbbyqkisfxobqldvhdqhebbromhugkwamqapjxgpaxdfj"), 32785);
}

                                                      

















































































































































































































































































