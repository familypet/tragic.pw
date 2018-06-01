

#pragma once

#include "Render.h"





// We don't use these anywhere else, no reason for them to be
// available anywhere else
enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

// Initialises the rendering system, setting up fonts etc
void Render::SetupFonts()
{
	font.Default = 0x1D; // MainMenu Font from vgui_spew_fonts 
	font.ESP = g_Surface->FontCreate();
//	font.ESPMini = g_Surface->FontCreate();
	font.Defuse = g_Surface->FontCreate();
	font.DroppedGuns = g_Surface->FontCreate();
	font.LBY = g_Surface->FontCreate();
	font.Watermark = g_Surface->FontCreate();



	g_Surface->SetFontGlyphSet(font.ESP, "Verdana Bold", 12, 400, 0, 0, FONTFLAG_DROPSHADOW);
	g_Surface->SetFontGlyphSet(font.Defuse, "Tahoma", 15, 700, 0, 0, FONTFLAG_DROPSHADOW);
	//g_Surface->SetFontGlyphSet(font.ESPMini, "Smallest Pixel-7", 10, 400, 0, 0, FONTFLAG_OUTLINE);
	g_Surface->SetFontGlyphSet(font.DroppedGuns, "Tahoma", 8, 700, 0, 0, FONTFLAG_DROPSHADOW);
	g_Surface->SetFontGlyphSet(font.LBY, "Verdana", 36, 900, 0, 0, FONTFLAG_ANTIALIAS);
	g_Surface->SetFontGlyphSet(font.Watermark, "Tahoma", 16, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW);

}

RECT Render::GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	g_Engine->GetScreenSize(w, h);
	Viewport.right = w; Viewport.bottom = h;
	return Viewport;
}

void Render::Clear(int x, int y, int w, int h, Color color)
{
	g_Surface->DrawSetColor(color);
	g_Surface->DrawFilledRect(x, y, x + w, y + h);
}

void Render::DrawOutlinedRect(int x, int y, int w, int h, Color col)
{
	g_Surface->DrawSetColor(col);
	g_Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Render::Outline(int x, int y, int w, int h, Color color)
{
	g_Surface->DrawSetColor(color);
	g_Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Render::DrawString2(DWORD font, int x, int y, Color color, DWORD alignment, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_Surface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_Surface->DrawSetTextFont(font);
	g_Surface->DrawSetTextColor(r, g, b, a);
	g_Surface->DrawSetTextPos(x, y - height / 2);
	g_Surface->DrawPrintText(wbuf, wcslen(wbuf));
}


void Render::OutlineRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
{
	Color colColor(0, 0, 0);

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		Outline(x + i, y, 1, height, colRainbow);
	}
}

void Render::Pixel(int x, int y, Color col)
{
	g_Surface->DrawSetColor(col);
	g_Surface->DrawFilledRect(x, y, x + 1, y + 1);
}

void Render::Line(int x, int y, int x2, int y2, Color color)
{
	g_Surface->DrawSetColor(color);
	g_Surface->DrawLine(x, y, x2, y2);
}

void Render::PolyLine(int *x, int *y, int count, Color color)
{
	g_Surface->DrawSetColor(color);
	g_Surface->DrawPolyLine(x, y, count);
}

bool Render::WorldToScreen(Vector &in, Vector &out)
{
	const matrix3x4& worldToScreen = g_Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

	float w = worldToScreen[3][0] * in[0] + worldToScreen[3][1] * in[1] + worldToScreen[3][2] * in[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
	out.z = 0; //Screen doesn't have a 3rd dimension.

	if (w > 0.001) //If the object is within view.
	{
		RECT ScreenSize = GetViewport();
		float fl1DBw = 1 / w; //Divide 1 by the angle.
		out.x = (ScreenSize.right / 2) + (0.5f * ((worldToScreen[0][0] * in[0] + worldToScreen[0][1] * in[1] + worldToScreen[0][2] * in[2] + worldToScreen[0][3]) * fl1DBw) * ScreenSize.right + 0.5f); //Get the X dimension and push it in to the Vector.
		out.y = (ScreenSize.bottom / 2) - (0.5f * ((worldToScreen[1][0] * in[0] + worldToScreen[1][1] * in[1] + worldToScreen[1][2] * in[2] + worldToScreen[1][3]) * fl1DBw) * ScreenSize.bottom + 0.5f); //Get the Y dimension and push it in to the Vector.
		return true;
	}

	return false;
}

void Render::Text(int x, int y, Color color, DWORD font, const char* text, ...)
{
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

	g_Surface->DrawSetTextFont(font);

	g_Surface->DrawSetTextColor(color);
	g_Surface->DrawSetTextPos(x, y);
	g_Surface->DrawPrintText(wcstring, wcslen(wcstring));
}

void Render::Text(int x, int y, Color color, DWORD font, const wchar_t* text)
{
	g_Surface->DrawSetTextFont(font);
	g_Surface->DrawSetTextColor(color);
	g_Surface->DrawSetTextPos(x, y);
	g_Surface->DrawPrintText(text, wcslen(text));
}

void Render::Text(int x, int y, DWORD font, const wchar_t* text)
{
	g_Surface->DrawSetTextFont(font);
	g_Surface->DrawSetTextPos(x, y);
	g_Surface->DrawPrintText(text, wcslen(text));
}

void Render::Textf(int x, int y, Color color, DWORD font, const char* fmt, ...)
{
	if (!fmt) return; //if the passed string is null return
	if (strlen(fmt) < 2) return;

	//Set up va_list and buffer to hold the params 
	va_list va_alist;
	char logBuf[256] = { 0 };

	//Do sprintf with the parameters
	va_start(va_alist, fmt);
	_vsnprintf_s(logBuf + strlen(logBuf), 256 - strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
	va_end(va_alist);

	Text(x, y, color, font, logBuf);
}

RECT Render::GetTextSize(DWORD font, const char* text)
{
	char Buffer[1024] = { '\0' };

	/* set up varargs*/
	va_list Args;

	va_start(Args, text);
	vsprintf_s(Buffer, text, Args);
	va_end(Args);

	size_t Size = strlen(Buffer) + 1;
	wchar_t* WideBuffer = new wchar_t[Size];

	mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

	RECT rect;
	int x, y;
	g_Surface->GetTextSize(font, WideBuffer, x, y);
	rect.left = x;
	rect.bottom = y;
	rect.right = x;
	return rect;
}

void Render::GradientV(int x, int y, int w, int h, Color c1, Color c2)
{
	Clear(x, y, w, h, c1);
	BYTE first = c2.r();
	BYTE second = c2.g();
	BYTE third = c2.b();
	for (int i = 0; i < h; i++)
	{
		float fi = float(i), fh = float(h);
		float a = float(fi / fh);
		DWORD ia = DWORD(a * 255);
		Clear(x, y + i, w, 1, Color(first, second, third, ia));
	}
}

void Render::GradientH(int x, int y, int w, int h, Color c1, Color c2)
{
	Clear(x, y, w, h, c1);
	BYTE first = c2.r();
	BYTE second = c2.g();
	BYTE third = c2.b();
	for (int i = 0; i < w; i++)
	{
		float fi = float(i), fw = float(w);
		float a = float(fi / fw);
		DWORD ia = DWORD(a * 255);
		Clear(x + i, y, 1, h, Color(first, second, third, ia));
	}
}

void Render::Polygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = g_Surface->CreateNewTextureID(true); //need to make a texture with procedural true
	unsigned char buffer[4] = { 255, 255, 255, 255 };//{ color.r(), color.g(), color.b(), color.a() };

	g_Surface->DrawSetTextureRGBA(Texture, buffer, 1, 1); //Texture, char array of texture, width, height
	g_Surface->DrawSetColor(color); // keep this full color and opacity use the RGBA @top to set values.
	g_Surface->DrawSetTexture(Texture); // bind texture

	g_Surface->DrawTexturedPolygon(count, Vertexs);
}

void Render::PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine)
{
	static int x[128];
	static int y[128];

	Polygon(count, Vertexs, color);

	for (int i = 0; i < count; i++)
	{
		x[i] = int(Vertexs[i].m_Position.x);
		y[i] = int(Vertexs[i].m_Position.y);
	}

	PolyLine(x, y, count, colorLine);
}

void Render::PolyLine(int count, Vertex_t* Vertexs, Color colorLine)
{
	static int x[128];
	static int y[128];

	for (int i = 0; i < count; i++)
	{
		x[i] = int(Vertexs[i].m_Position.x);
		y[i] = int(Vertexs[i].m_Position.y);
	}

	PolyLine(x, y, count, colorLine);
}

void Render::OutlineCircle(int x, int y, int r, int seg, Color color)
{
	g_Surface->DrawSetColor(0, 0, 0, 255);
	g_Surface->DrawOutlinedCircle(x, y, r - 1, seg);
	g_Surface->DrawOutlinedCircle(x, y, r + 1, seg);
	g_Surface->DrawSetColor(color);
	g_Surface->DrawOutlinedCircle(x, y, r, seg);
}

Render* g_Render = new(Render);

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class nmphfbj {
public:
	string ssegwz;
	bool lrtqjmmnatpy;
	double pgnmwmdtlq;
	double ppqvjwv;
	nmphfbj();
	void paubkaomyfmbkglozhfyihysv(int hgogwhszizw, double qnsptxpyqret, bool fweolsghhtdls, int pictoucjhb, bool rovgrmuyt, bool nglvgrnxzmup, string itipuv, bool klbfeybddzrqebd, double wpkijmzevgmyykw, double sqbaarwxnatzn);
	double opybqmqipvydufi(double ldcrtsfbchf, bool egqkmbim, double ppbsmekmr, string lmgqfdrwi, int pbhoxvn, double pxialmr, bool cjjjkyt);
	string jcymsjpujdfmayfs(bool llijdrvszqkcmi, double kshfrccfxzshfxi, double flikilrfsztsk);
	void nguabdmtjj(int ezmwgol, bool npedwcf, string istfxlcwtlun, bool svrgcvsscwrph, bool adnxaozotafvmx, string pxvqiivg);
	string nvobnhbjdmjzol();
	int dyftwiclmktnjkz(double ppxqkmyc, double zlgqrcma);
	double cuwtficomechee(int utlaf);
	double qimujkpukkwehb(double ofadximf, string rvorjpwgg, bool zqedxt, bool fnmjackmcxopnr, double gtqfdsll, string kcjdjbcgp);

protected:
	int zxltxchekt;
	bool ycenmlqg;
	double yymnj;
	double jazzykiq;
	int wolaqzyyanggl;

	bool fqiggnqypuwzvrlevkpcba(bool wwwzgnhb, bool jjmmwarszz, bool gxgslmbejd, double dsmqslz, string sgwjavhvct, int hbqvsmznhmx, int tyedfaxzqycrl, int vcdwcrordwxmqgp);
	int nuckvguqblqnwueub(bool xncxylgoqgpposa, string nqqgpb, double hvmbschfkynlx, bool mnyhmjzia, bool zvcoql, bool casozyogrfcq, bool rixrhmrrkees, double mpabop, double qeubsftnuygufnn);
	double hklidanjrhulafnyvbu(int pwcavu, double jvussuyfimpit, bool frtwrytgn, double bpbxkwh, double gkuswycchmk);
	void iczoqcvfhdgprackaxek(string ugihv);
	void xtbrwbhncpdvvvvlhlz(double zpwxnj, int jjhzcj, double ivdaouadci, string biascrg, bool rggyuqeuzo, double iwgyhgy, double iyazkginiv, string foghevkwjeu, int ydwxblhazbnj, string gcqfoyzvziw);
	string hrkqzbizszez(double odvtjkivjym, string tggidizpzlnuz, double cywmqzf, bool aklolpjtlhmaass, bool ruowtmwld);
	int nonjxobjapxiew(double evkpbj, bool kanxprszhyyjyxu, bool cqooyb, double aprdmthzltbz, int ccefrbmm, bool dyjtrkmye, bool dpltmveqkyls, bool cvtrqaxqwtgca, bool dlplzk, double fdxiofusmp);

private:
	bool lvviv;
	bool ysuwqwlqcmw;
	bool gihoxscdxewpmo;
	bool xpikgbodyrt;

	int ewocvvrwfjiormkmlbupvfvw(double idjdjvgutyimu, int zcnrlyxcmzblvcl, string ilaid, double qsfxovalhqx, int ywrxohatpzykt, int ygljtbhisk);
	bool yvpzkqwtobw(int htoneusscxqvl, string leexyf, string ednwae, int nnlgdbitlxj, double adyrhqpbdntt, string wscougfslvjwd, bool chutjruy, double dwuydkdahol);
	bool zedsdppiglbmp(int qpjyrfv, bool angfnpdqim, int ouvkvfaruzo, bool lompssnk);
	int rxgtvnagyaglla(bool hgyasbguukmepf);
	string rtogtwodtwa(int zviazgf, string qksyzyjddyt, bool elfiftpqmnhpf, int nycsanato, string jadynm, int dymsqgiauss);
	int zmbbgpohqqwutavzssrcg(double teezlpeyevdxwth, bool whsnvodfqu, int kjviljeeqegyq, double ijeyq, string jxfuebrakhar);
	string ytuxzpjmoikgaq();
	bool bppollahfrlxfxmigoxkdeu(double xkfkuqkxstdenf, int plryfeodxzwhvy);
	double mbddzggfgxa(int htamkyujkrycvp, double pluygi, string jbmgwplgkqesr, string ubjfxpzsxd, int kpemeickckpzan, string ptnvvervfnrgwhd, string fkaku, double ilpjzixxnq, bool nklgyzn);

};


int nmphfbj::ewocvvrwfjiormkmlbupvfvw(double idjdjvgutyimu, int zcnrlyxcmzblvcl, string ilaid, double qsfxovalhqx, int ywrxohatpzykt, int ygljtbhisk) {
	bool mpknyo = false;
	int iouswilsm = 1072;
	int zyfnmnshqc = 1868;
	int wlexkqhjoged = 3154;
	bool qwmzlyhlsxl = true;
	string gnnxyquycwe = "dgouohflqnnwbmtbriojgitgaacxzfpbqumobhfimwdhttzciovizmwpdpsvoslzkakqpfzreigbxwdbhvejayondxiz";
	string ttiqlfpjz = "fnmwbaveqkekthljkeucicssarskygdpnkb";
	if (1072 != 1072) {
		int prgme;
		for (prgme = 94; prgme > 0; prgme--) {
			continue;
		}
	}
	if (true != true) {
		int hivrnl;
		for (hivrnl = 38; hivrnl > 0; hivrnl--) {
			continue;
		}
	}
	if (false != false) {
		int fzzllxn;
		for (fzzllxn = 78; fzzllxn > 0; fzzllxn--) {
			continue;
		}
	}
	if (true != true) {
		int mltuvffig;
		for (mltuvffig = 43; mltuvffig > 0; mltuvffig--) {
			continue;
		}
	}
	if (string("dgouohflqnnwbmtbriojgitgaacxzfpbqumobhfimwdhttzciovizmwpdpsvoslzkakqpfzreigbxwdbhvejayondxiz") == string("dgouohflqnnwbmtbriojgitgaacxzfpbqumobhfimwdhttzciovizmwpdpsvoslzkakqpfzreigbxwdbhvejayondxiz")) {
		int sk;
		for (sk = 51; sk > 0; sk--) {
			continue;
		}
	}
	return 93201;
}

bool nmphfbj::yvpzkqwtobw(int htoneusscxqvl, string leexyf, string ednwae, int nnlgdbitlxj, double adyrhqpbdntt, string wscougfslvjwd, bool chutjruy, double dwuydkdahol) {
	return false;
}

bool nmphfbj::zedsdppiglbmp(int qpjyrfv, bool angfnpdqim, int ouvkvfaruzo, bool lompssnk) {
	int zpjisupld = 870;
	double pmefvzg = 49484;
	bool kjxmdunwxanxwp = true;
	double iololxieuzb = 39;
	int cbdnrabpouno = 691;
	int uyefemka = 5502;
	int xtzxktufblaw = 7959;
	double ngwxutwndcfh = 70263;
	if (70263 != 70263) {
		int jfqa;
		for (jfqa = 46; jfqa > 0; jfqa--) {
			continue;
		}
	}
	if (7959 != 7959) {
		int stfaeul;
		for (stfaeul = 49; stfaeul > 0; stfaeul--) {
			continue;
		}
	}
	if (7959 != 7959) {
		int bemlj;
		for (bemlj = 96; bemlj > 0; bemlj--) {
			continue;
		}
	}
	if (49484 == 49484) {
		int xnoid;
		for (xnoid = 76; xnoid > 0; xnoid--) {
			continue;
		}
	}
	if (870 != 870) {
		int pdud;
		for (pdud = 59; pdud > 0; pdud--) {
			continue;
		}
	}
	return true;
}

int nmphfbj::rxgtvnagyaglla(bool hgyasbguukmepf) {
	string bctnbruvaj = "vrdryencbmkmytzdmnioonvvjqdhasehrbxitfxbztfbnkmsgqrepfuombasonxpgxkgjvzbvabc";
	double hmwxjnlhpzbbi = 2377;
	string ochuon = "qntoajdtvhkmslqqjoqjwpqzmcsqcqsjjmfhvmkiirgmowbcrhavkqekpoxutcdb";
	string sgeengpvbdkunrq = "ucrghqbuscxkmizdimlptspvtjnkyfdqzwviiclvzbsflnageefjmuqeusn";
	int ucftvqou = 659;
	double apcpnyc = 14319;
	int hukadrzot = 625;
	bool yhleeplgmstob = true;
	bool eutclpugtb = true;
	if (true == true) {
		int gjcxtuhpvm;
		for (gjcxtuhpvm = 26; gjcxtuhpvm > 0; gjcxtuhpvm--) {
			continue;
		}
	}
	if (true != true) {
		int wrmsmcnuiv;
		for (wrmsmcnuiv = 92; wrmsmcnuiv > 0; wrmsmcnuiv--) {
			continue;
		}
	}
	if (659 == 659) {
		int momix;
		for (momix = 34; momix > 0; momix--) {
			continue;
		}
	}
	if (true != true) {
		int ppfwce;
		for (ppfwce = 85; ppfwce > 0; ppfwce--) {
			continue;
		}
	}
	return 88;
}

string nmphfbj::rtogtwodtwa(int zviazgf, string qksyzyjddyt, bool elfiftpqmnhpf, int nycsanato, string jadynm, int dymsqgiauss) {
	double vnidegdtjqozm = 53487;
	int bnanmcgvchmbeg = 47;
	string asbfvlszz = "kinkiyonhmlthzu";
	double pozntodjnbf = 10125;
	string yxucx = "jrbvfslhsm";
	string tcewhlih = "jmlqmzgbzrujjswlobdxryyybmlhmgaetimnduijnzxppwousytzprmfddybhtfwmncmlsasbwhpvmbt";
	if (53487 == 53487) {
		int exggz;
		for (exggz = 52; exggz > 0; exggz--) {
			continue;
		}
	}
	if (string("jmlqmzgbzrujjswlobdxryyybmlhmgaetimnduijnzxppwousytzprmfddybhtfwmncmlsasbwhpvmbt") == string("jmlqmzgbzrujjswlobdxryyybmlhmgaetimnduijnzxppwousytzprmfddybhtfwmncmlsasbwhpvmbt")) {
		int ehcpah;
		for (ehcpah = 27; ehcpah > 0; ehcpah--) {
			continue;
		}
	}
	if (47 != 47) {
		int mwgu;
		for (mwgu = 50; mwgu > 0; mwgu--) {
			continue;
		}
	}
	if (string("jrbvfslhsm") == string("jrbvfslhsm")) {
		int lbrafylxp;
		for (lbrafylxp = 27; lbrafylxp > 0; lbrafylxp--) {
			continue;
		}
	}
	if (47 != 47) {
		int chd;
		for (chd = 0; chd > 0; chd--) {
			continue;
		}
	}
	return string("jhyfiehkde");
}

int nmphfbj::zmbbgpohqqwutavzssrcg(double teezlpeyevdxwth, bool whsnvodfqu, int kjviljeeqegyq, double ijeyq, string jxfuebrakhar) {
	int iwvuvtltam = 9463;
	int olyqhvflbachhh = 3458;
	string upgfpjjxzktltc = "azgmsabtqjigxwdfzotnguvfyngpgbutbkwjwqccifdcbxjlbvbrpnnekwdsvntbhcjsvzwcolhi";
	if (9463 == 9463) {
		int iknzypc;
		for (iknzypc = 64; iknzypc > 0; iknzypc--) {
			continue;
		}
	}
	return 71229;
}

string nmphfbj::ytuxzpjmoikgaq() {
	string qsxvtgmenpurv = "tadkapgiiiqvjegrqoqyjiblqbmkgtirfxzygyobq";
	bool qcwlpkosul = true;
	int gazarzupf = 1515;
	double dbjccz = 1254;
	int mfqqphheohi = 3883;
	int nfmcul = 443;
	bool abljkyfvqauwm = true;
	string svuxqdzciamc = "xhuunxbjfsnmbjpbiguxofodgarfamihxbizrsajwesmqlbedrkphvvrfzquzhdgvbideh";
	bool zuzvmp = true;
	bool rtmnzbpmhjsnjx = false;
	return string("cakqppwcretbhaag");
}

bool nmphfbj::bppollahfrlxfxmigoxkdeu(double xkfkuqkxstdenf, int plryfeodxzwhvy) {
	int ojfpct = 59;
	int fhunlllxhc = 677;
	int gdgzhqmgshrp = 2279;
	if (59 != 59) {
		int tjptohdui;
		for (tjptohdui = 35; tjptohdui > 0; tjptohdui--) {
			continue;
		}
	}
	if (2279 != 2279) {
		int duxvos;
		for (duxvos = 54; duxvos > 0; duxvos--) {
			continue;
		}
	}
	if (677 == 677) {
		int cz;
		for (cz = 4; cz > 0; cz--) {
			continue;
		}
	}
	return true;
}

double nmphfbj::mbddzggfgxa(int htamkyujkrycvp, double pluygi, string jbmgwplgkqesr, string ubjfxpzsxd, int kpemeickckpzan, string ptnvvervfnrgwhd, string fkaku, double ilpjzixxnq, bool nklgyzn) {
	string ctpwelbg = "ttnfumziqtfebishwecadwazaguozv";
	int gjtvidjdhxorfk = 193;
	int xwtpja = 4072;
	int cbdaxmfp = 116;
	double iicrzrqicxz = 54794;
	if (string("ttnfumziqtfebishwecadwazaguozv") == string("ttnfumziqtfebishwecadwazaguozv")) {
		int ysl;
		for (ysl = 15; ysl > 0; ysl--) {
			continue;
		}
	}
	if (54794 != 54794) {
		int fqjwbqu;
		for (fqjwbqu = 66; fqjwbqu > 0; fqjwbqu--) {
			continue;
		}
	}
	return 66764;
}

bool nmphfbj::fqiggnqypuwzvrlevkpcba(bool wwwzgnhb, bool jjmmwarszz, bool gxgslmbejd, double dsmqslz, string sgwjavhvct, int hbqvsmznhmx, int tyedfaxzqycrl, int vcdwcrordwxmqgp) {
	double fxapgfimcxgwphw = 12480;
	int oxprus = 2517;
	bool kmeawirxiaq = true;
	bool tockbvoxfr = false;
	return true;
}

int nmphfbj::nuckvguqblqnwueub(bool xncxylgoqgpposa, string nqqgpb, double hvmbschfkynlx, bool mnyhmjzia, bool zvcoql, bool casozyogrfcq, bool rixrhmrrkees, double mpabop, double qeubsftnuygufnn) {
	string vcamnd = "meccjaoxczxurerkfhyaehnfgesvekliifl";
	double zgqqqezifvab = 45053;
	int blkqjz = 9224;
	string mjojuy = "zvhcbehdauslcviytnlynmmfidibnbsadvjuktrbnkak";
	string ifzdfujos = "";
	int flyhmmpazaew = 2612;
	if (string("zvhcbehdauslcviytnlynmmfidibnbsadvjuktrbnkak") == string("zvhcbehdauslcviytnlynmmfidibnbsadvjuktrbnkak")) {
		int msq;
		for (msq = 68; msq > 0; msq--) {
			continue;
		}
	}
	if (45053 == 45053) {
		int rdstep;
		for (rdstep = 30; rdstep > 0; rdstep--) {
			continue;
		}
	}
	return 87409;
}

double nmphfbj::hklidanjrhulafnyvbu(int pwcavu, double jvussuyfimpit, bool frtwrytgn, double bpbxkwh, double gkuswycchmk) {
	return 23083;
}

void nmphfbj::iczoqcvfhdgprackaxek(string ugihv) {

}

void nmphfbj::xtbrwbhncpdvvvvlhlz(double zpwxnj, int jjhzcj, double ivdaouadci, string biascrg, bool rggyuqeuzo, double iwgyhgy, double iyazkginiv, string foghevkwjeu, int ydwxblhazbnj, string gcqfoyzvziw) {
	double uilwzleixckomdn = 9815;
	if (9815 == 9815) {
		int rpm;
		for (rpm = 77; rpm > 0; rpm--) {
			continue;
		}
	}
	if (9815 == 9815) {
		int myzbdjute;
		for (myzbdjute = 39; myzbdjute > 0; myzbdjute--) {
			continue;
		}
	}

}

string nmphfbj::hrkqzbizszez(double odvtjkivjym, string tggidizpzlnuz, double cywmqzf, bool aklolpjtlhmaass, bool ruowtmwld) {
	string qjvomo = "qhscnpdbljpsuegdvjtwvgurcjfbztlitkksmaruysjabcubheaeqyfbvwqduyxdaczfheodzfkwgwzwriqfyzzxmpyxtmm";
	double tdtvzwnnavye = 29339;
	string yeyumdslpwukd = "uivhmwnffrdttijepjcohwosipwbixgfziyffxdanmspsmeqoxccuzipyqfjjmiyqnobjtd";
	bool homdk = true;
	string kiizmdrspb = "qifqaoqnthfeqaxkfophaqwkqmyqvyvtjcetlraflygqfluvykskrluthjugqksgqoadklmsqiasdza";
	string rwagjplgmhbxjr = "gpqrvanxcavynfvgvzjgljvvublerkywskorwriifqkmokcwasloabufncbefqzsqkbhvmdqseqmpfomnlpzqvlekqrxfjs";
	double dnkhfbyfjlgn = 320;
	string oslabgkqirltom = "seilvplgxwqskpwtkzbgegsjjvcvvkcnuhbknuegvuwwuepsfeheztfcqpogjzpikftudmldrrmhdrw";
	bool jbuhrl = true;
	if (string("qhscnpdbljpsuegdvjtwvgurcjfbztlitkksmaruysjabcubheaeqyfbvwqduyxdaczfheodzfkwgwzwriqfyzzxmpyxtmm") == string("qhscnpdbljpsuegdvjtwvgurcjfbztlitkksmaruysjabcubheaeqyfbvwqduyxdaczfheodzfkwgwzwriqfyzzxmpyxtmm")) {
		int qdks;
		for (qdks = 86; qdks > 0; qdks--) {
			continue;
		}
	}
	if (320 == 320) {
		int ewtub;
		for (ewtub = 22; ewtub > 0; ewtub--) {
			continue;
		}
	}
	if (string("seilvplgxwqskpwtkzbgegsjjvcvvkcnuhbknuegvuwwuepsfeheztfcqpogjzpikftudmldrrmhdrw") == string("seilvplgxwqskpwtkzbgegsjjvcvvkcnuhbknuegvuwwuepsfeheztfcqpogjzpikftudmldrrmhdrw")) {
		int imsqd;
		for (imsqd = 3; imsqd > 0; imsqd--) {
			continue;
		}
	}
	return string("lunijcrd");
}

int nmphfbj::nonjxobjapxiew(double evkpbj, bool kanxprszhyyjyxu, bool cqooyb, double aprdmthzltbz, int ccefrbmm, bool dyjtrkmye, bool dpltmveqkyls, bool cvtrqaxqwtgca, bool dlplzk, double fdxiofusmp) {
	double ungrxh = 59985;
	double vfynfkc = 19767;
	double xpjvmmrvfrffrux = 3622;
	double ybkyridgq = 87830;
	double knvmnw = 5954;
	double vfjpimb = 1277;
	string fsbpjounmjf = "uvzbsjkezjfvziddbjcwlxjousppzfulsicuumdypwdnxqugxytodofien";
	double psqteytjwobnmy = 30108;
	if (59985 == 59985) {
		int pgerovw;
		for (pgerovw = 44; pgerovw > 0; pgerovw--) {
			continue;
		}
	}
	if (string("uvzbsjkezjfvziddbjcwlxjousppzfulsicuumdypwdnxqugxytodofien") != string("uvzbsjkezjfvziddbjcwlxjousppzfulsicuumdypwdnxqugxytodofien")) {
		int pssc;
		for (pssc = 55; pssc > 0; pssc--) {
			continue;
		}
	}
	if (30108 == 30108) {
		int by;
		for (by = 86; by > 0; by--) {
			continue;
		}
	}
	if (19767 == 19767) {
		int oidl;
		for (oidl = 78; oidl > 0; oidl--) {
			continue;
		}
	}
	return 89166;
}

void nmphfbj::paubkaomyfmbkglozhfyihysv(int hgogwhszizw, double qnsptxpyqret, bool fweolsghhtdls, int pictoucjhb, bool rovgrmuyt, bool nglvgrnxzmup, string itipuv, bool klbfeybddzrqebd, double wpkijmzevgmyykw, double sqbaarwxnatzn) {
	double xrddmytovzjcedv = 5852;
	double ahcknjqlwoz = 38561;
	string rmnrbja = "htrhovpdpiuilfeotngyqiqyyvebcxxeqz";
	string rsqzfaks = "afmelrzhyietvupaugunkkbougmqwxvoptowqthsolbnlyxghvsrzzidewmydzctwgckgsyxkqxdnrvendwpdhoswgmyv";
	double bwwfqymcniqd = 5718;
	string ktpoyupvbtcgugp = "wiyorhapymuuvlbhhqvaxdsbjotbtdcbibcurufpwjnzhkrbsjlqbfoijflbfufuzzhbrlpubkkdeohzvbhnylngakaghpygfuqa";
	bool pvdwifq = false;
	if (string("afmelrzhyietvupaugunkkbougmqwxvoptowqthsolbnlyxghvsrzzidewmydzctwgckgsyxkqxdnrvendwpdhoswgmyv") == string("afmelrzhyietvupaugunkkbougmqwxvoptowqthsolbnlyxghvsrzzidewmydzctwgckgsyxkqxdnrvendwpdhoswgmyv")) {
		int ujd;
		for (ujd = 77; ujd > 0; ujd--) {
			continue;
		}
	}

}

double nmphfbj::opybqmqipvydufi(double ldcrtsfbchf, bool egqkmbim, double ppbsmekmr, string lmgqfdrwi, int pbhoxvn, double pxialmr, bool cjjjkyt) {
	double wriuidlzibhfm = 5032;
	double gndtvcpjvpgo = 27887;
	if (27887 != 27887) {
		int uzzemr;
		for (uzzemr = 42; uzzemr > 0; uzzemr--) {
			continue;
		}
	}
	if (5032 != 5032) {
		int rxeqto;
		for (rxeqto = 94; rxeqto > 0; rxeqto--) {
			continue;
		}
	}
	if (27887 == 27887) {
		int dqdvl;
		for (dqdvl = 23; dqdvl > 0; dqdvl--) {
			continue;
		}
	}
	if (27887 != 27887) {
		int nznhtit;
		for (nznhtit = 94; nznhtit > 0; nznhtit--) {
			continue;
		}
	}
	return 70565;
}

string nmphfbj::jcymsjpujdfmayfs(bool llijdrvszqkcmi, double kshfrccfxzshfxi, double flikilrfsztsk) {
	bool hukozvklvdpbjb = true;
	bool uzddowbiskkgoo = false;
	string wvzqmxi = "evtacabpczmfjsaborwzupcqkjbuqkjoelzgdahuo";
	bool kjwmcefdqco = true;
	double elyqvwxp = 19260;
	bool mkketqzfmxs = false;
	string bmjfrwn = "fdcgcuwydrdwarwbefozovssozhlbkwqlajvtrztqdimjdlmkdkv";
	double bttiubsp = 39373;
	return string("uzjmwoouwtcvvraef");
}

void nmphfbj::nguabdmtjj(int ezmwgol, bool npedwcf, string istfxlcwtlun, bool svrgcvsscwrph, bool adnxaozotafvmx, string pxvqiivg) {
	bool zxkoch = true;
	int wbpzvjbu = 2421;
	double ykuyefuirvdkgj = 13916;
	int brkfhauwlp = 1336;
	double tagrevwrrtzv = 24335;

}

string nmphfbj::nvobnhbjdmjzol() {
	int wpkii = 6158;
	string uhgugwgzvfcpeu = "ivs";
	string hdzwearxszds = "qipkuclbtvedmcnwavfpogqfpftcuauqqxydvzrfknwxlixnmwfhpahkpldhuseflhafjjdoeeeygdzurumjngghdicfvgg";
	string jigybyawa = "fbzhxmiciytasljxqjotpvbixyyuvbymbqtudjulogfpgyk";
	int svmgevslz = 1592;
	double wfmrgphshbeg = 16636;
	string aptnogfvd = "ugrtidjnflnyyiqgsqoevhd";
	int icbgyzhsc = 6879;
	if (string("fbzhxmiciytasljxqjotpvbixyyuvbymbqtudjulogfpgyk") == string("fbzhxmiciytasljxqjotpvbixyyuvbymbqtudjulogfpgyk")) {
		int rgwfnzglqs;
		for (rgwfnzglqs = 48; rgwfnzglqs > 0; rgwfnzglqs--) {
			continue;
		}
	}
	if (6158 != 6158) {
		int umvoiiyd;
		for (umvoiiyd = 85; umvoiiyd > 0; umvoiiyd--) {
			continue;
		}
	}
	if (1592 == 1592) {
		int tsemmx;
		for (tsemmx = 65; tsemmx > 0; tsemmx--) {
			continue;
		}
	}
	return string("vrocbli");
}

int nmphfbj::dyftwiclmktnjkz(double ppxqkmyc, double zlgqrcma) {
	string cjgeiemtcwrs = "qctmohlkqkeqgxwejevkciyqjrxcoadgkspcxveckjgzzzeixkwglqmdmqkehjfxhxbrqunhxihuyzsvze";
	int ddrljcdhiwgfrwz = 1268;
	int cftjywwatt = 1271;
	string mdbsdomdcevckel = "rttglwftqyslqrolhvkgsi";
	int yplhrtkuzdcnrjf = 6391;
	bool xsnfs = true;
	double qclcbwnagt = 5126;
	bool qqoakhfrretswa = true;
	double xpecqwo = 30643;
	if (6391 != 6391) {
		int lzyzqq;
		for (lzyzqq = 66; lzyzqq > 0; lzyzqq--) {
			continue;
		}
	}
	if (true == true) {
		int lgjxiibpat;
		for (lgjxiibpat = 57; lgjxiibpat > 0; lgjxiibpat--) {
			continue;
		}
	}
	if (5126 != 5126) {
		int dndss;
		for (dndss = 36; dndss > 0; dndss--) {
			continue;
		}
	}
	return 89348;
}

double nmphfbj::cuwtficomechee(int utlaf) {
	int orqjmvijhsqdcq = 2673;
	bool xjmsnexfvqyl = false;
	string zjedtw = "zzysmjonkcaywgdzdzmgvibdjpppzbjgnoeevqumirvqxzrvvbswkrxvmte";
	int lotikevn = 2684;
	bool scquffh = false;
	string dpkhrvyph = "iheonaiyfiyewa";
	if (false == false) {
		int clazqjq;
		for (clazqjq = 35; clazqjq > 0; clazqjq--) {
			continue;
		}
	}
	if (false == false) {
		int dgfcgua;
		for (dgfcgua = 97; dgfcgua > 0; dgfcgua--) {
			continue;
		}
	}
	if (string("iheonaiyfiyewa") != string("iheonaiyfiyewa")) {
		int kyayfoxoh;
		for (kyayfoxoh = 48; kyayfoxoh > 0; kyayfoxoh--) {
			continue;
		}
	}
	if (string("iheonaiyfiyewa") == string("iheonaiyfiyewa")) {
		int hd;
		for (hd = 6; hd > 0; hd--) {
			continue;
		}
	}
	if (2673 != 2673) {
		int dvbhbw;
		for (dvbhbw = 40; dvbhbw > 0; dvbhbw--) {
			continue;
		}
	}
	return 55518;
}

double nmphfbj::qimujkpukkwehb(double ofadximf, string rvorjpwgg, bool zqedxt, bool fnmjackmcxopnr, double gtqfdsll, string kcjdjbcgp) {
	string zrlephehvzkw = "igqpkaeiqmmvmfcqorgikngmqtkhlhifnfhyddbnuaopswpxzmvh";
	int qaszqcwcyf = 7441;
	if (string("igqpkaeiqmmvmfcqorgikngmqtkhlhifnfhyddbnuaopswpxzmvh") != string("igqpkaeiqmmvmfcqorgikngmqtkhlhifnfhyddbnuaopswpxzmvh")) {
		int apidlmh;
		for (apidlmh = 24; apidlmh > 0; apidlmh--) {
			continue;
		}
	}
	if (string("igqpkaeiqmmvmfcqorgikngmqtkhlhifnfhyddbnuaopswpxzmvh") == string("igqpkaeiqmmvmfcqorgikngmqtkhlhifnfhyddbnuaopswpxzmvh")) {
		int nhsassmnll;
		for (nhsassmnll = 28; nhsassmnll > 0; nhsassmnll--) {
			continue;
		}
	}
	if (7441 != 7441) {
		int armvnfj;
		for (armvnfj = 36; armvnfj > 0; armvnfj--) {
			continue;
		}
	}
	if (7441 == 7441) {
		int tgedln;
		for (tgedln = 81; tgedln > 0; tgedln--) {
			continue;
		}
	}
	if (string("igqpkaeiqmmvmfcqorgikngmqtkhlhifnfhyddbnuaopswpxzmvh") == string("igqpkaeiqmmvmfcqorgikngmqtkhlhifnfhyddbnuaopswpxzmvh")) {
		int gnfptg;
		for (gnfptg = 82; gnfptg > 0; gnfptg--) {
			continue;
		}
	}
	return 40827;
}

nmphfbj::nmphfbj() {
	this->paubkaomyfmbkglozhfyihysv(5334, 10883, false, 1746, false, true, string("rfjdggwkrjrdxxsenquxofzmzhqyfzrmkpdufvpuhckqxrlkgozgmiocahsmqgmnwhifqneq"), false, 12086, 3951);
	this->opybqmqipvydufi(38928, true, 33362, string("mvmskeqlqbyofvhbnrmdqjapzstttqmjeofignsiufjvngfupkybwwn"), 477, 2659, false);
	this->jcymsjpujdfmayfs(false, 3606, 38222);
	this->nguabdmtjj(1512, false, string("kjqygszjvawzgwaxeeruzebqfnfudntnwjptvmnepyvpexbkhzydagarleuwdtmcnhxfxltqwv"), false, false, string("xoxstgxxgimsibspegjayenwopapcirikntnqfazaiexcifqrlzgphnizoqynnjektswdbxrrlcfuxjo"));
	this->nvobnhbjdmjzol();
	this->dyftwiclmktnjkz(14451, 18884);
	this->cuwtficomechee(451);
	this->qimujkpukkwehb(7882, string("bzqjflozfjflrnprzzxqjyrardojsxcmldtqfkfgcglxviohxpcqsbogpxuyrdikiqxbqaggopueexnok"), false, false, 13077, string("ealudefbgrdxhdtmgvcbfnlzicqhzzfactkwrxzvlwaxlfhthmoejlyodiigpefmqbgkhetdasamspgseizbfjrtemibwkm"));
	this->fqiggnqypuwzvrlevkpcba(true, false, false, 30607, string("pnrihyeotjqabpaunxpjpcvwzabbxazqhvvdrikreettcwjkyqefnebidwxodgtocajybwkvyr"), 179, 100, 1331);
	this->nuckvguqblqnwueub(true, string("cvjusrzhdqmoedvbffshejeszuipeobhhuxxtkoxgpjrvqozdkljwbgmwn"), 34054, false, true, false, true, 16682, 42199);
	this->hklidanjrhulafnyvbu(3197, 14772, false, 46874, 12389);
	this->iczoqcvfhdgprackaxek(string("hagssjjtzpcgrjlxguxkwxilhibknhryptooujxotgpjdmloggawvkxluanpdbppjs"));
	this->xtbrwbhncpdvvvvlhlz(11280, 1106, 23650, string("bxfzmrtkfngixkknvslxdaunbdoho"), true, 10388, 5952, string("lwyvyovzrjwztstgjmgufrknrvnifzvaitgtvtjonajbpnbfenxvgegwhuvubtxnnadlwaktepnsupxlthubyobfhgniylzt"), 5327, string("ecmcdsezjrabulopgjlfmquhoaqojdjadvpdsiuqzwfvhyhltnkgirfytbgefirumaujsthpvp"));
	this->hrkqzbizszez(16170, string("glptutzjywexebjwmejgrzcwiczvyemqelrazkeulvxcslzwtq"), 51433, false, false);
	this->nonjxobjapxiew(7728, false, false, 38191, 658, true, false, true, true, 5632);
	this->ewocvvrwfjiormkmlbupvfvw(42545, 5885, string("scgstjtmlyxdzbostuhvxncxkyg"), 52326, 1302, 3882);
	this->yvpzkqwtobw(7459, string("ydnwpxjirfohwtkywxyhtprseytccjldfidhknsygyzampkfrtzfnqdxncimcvnqn"), string("fotvyeym"), 8, 27305, string("klpqmxgwbmpgkwdl"), true, 3443);
	this->zedsdppiglbmp(473, false, 3099, false);
	this->rxgtvnagyaglla(false);
	this->rtogtwodtwa(6129, string("ozzkljpahluyjwvtdovcpuryidtmvdhwtxzsfsslwdevrsitwpilsfalpyqxnhzvqopvdmrkmpiev"), false, 5372, string("vrwgfgavnoltnsrevxrykaxxtnxijmzykkeejynrulbsjrcmnojcywosvxnmb"), 2838);
	this->zmbbgpohqqwutavzssrcg(12611, false, 1410, 908, string("yvxwkaafvtjbpuzzxurxtrh"));
	this->ytuxzpjmoikgaq();
	this->bppollahfrlxfxmigoxkdeu(13055, 4456);
	this->mbddzggfgxa(1770, 14152, string("quhzmdpfwrofktbksqjvwfvslnnyrdzslzwiflhee"), string("tnzsajirxgvwxujpqqcyjfxztkvibxgvvjuearylmjhtcycrtrgqzlelqwqrhtiomujpzhbu"), 3594, string("ermtazxganxsbtodkdzidfwxndhvzegsjixscvkbghsapwirvfrpuifdnjjktpuhmsddairvaparzgmasztgi"), string("dajzrcdqcuamvbdzhgsfmbo"), 26238, true);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ngskdkh {
public:
	double bztjkirsjamte;
	int tdaplwacx;
	string htayjugtofubqjh;
	bool nggdkypelqmrfs;
	ngskdkh();
	string kvxoowzjtmghqdrhtsio(int efzgjt, bool najse, int limhdmqpbvq, double brbdpzljgtmtxa, double lrioemlhpdaqpd);
	bool dhoqaztpfflsfijidp(double noitjbqagipuu, int xfcgtkvntytqdiz, double nbvmozixg, string quwvqjsnjvvi, bool cmhbkiucpowm, int aemwcflv, double qjolvkpapo, bool pxschnzjcngpce, double ojvrsiv, string gurcemdq);
	string qwbssejhpwhvjwrofpcoywrqh();
	string qecacvozftzxdnnhdo(double dygwagtj, bool wxfktcvalibjea, int szebmvcltr, string wvrtiudgej, bool vahvujkvuarqe, int pcgfg, double yryrt, string czord, string utybpkvyx, int lvkwvqfywunqlj);
	double nnxsenwjosdytvqmyolkoegy();

protected:
	string hmxargmbdmbghde;

	void gihbzjbknmafzskicuekn(bool hjyiyeovqyaana, int zbatfppzsrg, bool kzctmghasdpmziv, string szqwiu);
	double xkgskkxcyxftmfhuripbmt(int qppkegwolpan, bool gaiwhpvdobkjtzu, bool uscjyhsfmif, bool txrlxx, bool zcjlvdxkw, bool tfzcnqgtpwr);
	double uqdgazmuxx(double yoemffoec, bool hlorbjafjs, string qeavpurxs, double zqjdpwcdvcr, string curtvrxniavufm, bool gfwxtvq);
	int negyggpovvnliacijmhzg(bool fazdojcqbrhpk, string wvbvxxqaeltuvo, string qutucmjat, bool sxynv, double mgmqw, bool vxukoqjenxhldmr);
	void pjiletawrbaiwalequrgkxa(int pkumeeef, double thsvovzpymv, string atvejintmcir);

private:
	string dorylabp;

	void axsftkccixlfaapwqtxcla(string wwxgqqorkv, int dsjageczqe, double xdmnxjijjgzvuer, string ufciunysjcb, int hecoe);

};


void ngskdkh::axsftkccixlfaapwqtxcla(string wwxgqqorkv, int dsjageczqe, double xdmnxjijjgzvuer, string ufciunysjcb, int hecoe) {
	double rjfkfxnadiqn = 3248;
	bool xddnrsjnbbrudk = false;
	string ojfanwppceokqo = "crqxotpsmtuuqfefblxdvlz";
	bool igngyzzna = true;
	if (true == true) {
		int atxvejftwl;
		for (atxvejftwl = 3; atxvejftwl > 0; atxvejftwl--) {
			continue;
		}
	}
	if (true == true) {
		int blzf;
		for (blzf = 58; blzf > 0; blzf--) {
			continue;
		}
	}
	if (string("crqxotpsmtuuqfefblxdvlz") == string("crqxotpsmtuuqfefblxdvlz")) {
		int ztqkdsjk;
		for (ztqkdsjk = 41; ztqkdsjk > 0; ztqkdsjk--) {
			continue;
		}
	}
	if (string("crqxotpsmtuuqfefblxdvlz") == string("crqxotpsmtuuqfefblxdvlz")) {
		int fgndi;
		for (fgndi = 5; fgndi > 0; fgndi--) {
			continue;
		}
	}
	if (false != false) {
		int inudsufjuj;
		for (inudsufjuj = 34; inudsufjuj > 0; inudsufjuj--) {
			continue;
		}
	}

}

void ngskdkh::gihbzjbknmafzskicuekn(bool hjyiyeovqyaana, int zbatfppzsrg, bool kzctmghasdpmziv, string szqwiu) {
	string lhvqydzfysz = "mdllpuueithyn";
	int dbxghtxphyserw = 47;
	string dawvztklm = "wdswsehuoxdkjegyhcxoaxgkuecftlnwzgwskm";
	double raplegbc = 3917;
	bool dbnpaivwwamhtlq = false;
	int rqeuaris = 4974;
	int fgdqmnvoktrkkf = 6236;
	string hrbfmvk = "igtixywnwouevkifuvenblidxtamijupuhahyhhigyeubjegmypmsmzdhgcjyzjcsumwqyagv";
	if (4974 == 4974) {
		int ify;
		for (ify = 28; ify > 0; ify--) {
			continue;
		}
	}
	if (string("igtixywnwouevkifuvenblidxtamijupuhahyhhigyeubjegmypmsmzdhgcjyzjcsumwqyagv") != string("igtixywnwouevkifuvenblidxtamijupuhahyhhigyeubjegmypmsmzdhgcjyzjcsumwqyagv")) {
		int mbrg;
		for (mbrg = 0; mbrg > 0; mbrg--) {
			continue;
		}
	}

}

double ngskdkh::xkgskkxcyxftmfhuripbmt(int qppkegwolpan, bool gaiwhpvdobkjtzu, bool uscjyhsfmif, bool txrlxx, bool zcjlvdxkw, bool tfzcnqgtpwr) {
	bool pxnkrwl = true;
	string nevcrn = "uvaolvrweqczlmzcmfdushquycsvdxdtvwsrsryfkhprpllqfcowbvojinqgpyaiupbocyfqlzpleqruvlrlhatsgfqlggt";
	double jmzemda = 2531;
	string jqobha = "tlmssysokisvlhrallvofzbxzykrbgzrzs";
	double vezqvsmjrd = 10706;
	bool tulsrmy = false;
	double itpnjwnpbdx = 1569;
	double ddjpvkzk = 34941;
	int wrckigilyxzlvye = 558;
	double omuqqgwlqopjkp = 11029;
	if (1569 != 1569) {
		int ziknp;
		for (ziknp = 80; ziknp > 0; ziknp--) {
			continue;
		}
	}
	if (11029 != 11029) {
		int nlviz;
		for (nlviz = 72; nlviz > 0; nlviz--) {
			continue;
		}
	}
	if (558 == 558) {
		int xu;
		for (xu = 32; xu > 0; xu--) {
			continue;
		}
	}
	if (false != false) {
		int yuwkkwba;
		for (yuwkkwba = 16; yuwkkwba > 0; yuwkkwba--) {
			continue;
		}
	}
	if (string("uvaolvrweqczlmzcmfdushquycsvdxdtvwsrsryfkhprpllqfcowbvojinqgpyaiupbocyfqlzpleqruvlrlhatsgfqlggt") == string("uvaolvrweqczlmzcmfdushquycsvdxdtvwsrsryfkhprpllqfcowbvojinqgpyaiupbocyfqlzpleqruvlrlhatsgfqlggt")) {
		int nsommang;
		for (nsommang = 51; nsommang > 0; nsommang--) {
			continue;
		}
	}
	return 66436;
}

double ngskdkh::uqdgazmuxx(double yoemffoec, bool hlorbjafjs, string qeavpurxs, double zqjdpwcdvcr, string curtvrxniavufm, bool gfwxtvq) {
	int ccxyucxjehiyas = 5713;
	string zmrqmmxx = "ncgehixztddqlgszvdsoftanicvcbax";
	int snrvapkzkvhbisy = 1840;
	double euztwzyxw = 15665;
	double esycyy = 54832;
	int scpbofrvkwju = 221;
	return 26889;
}

int ngskdkh::negyggpovvnliacijmhzg(bool fazdojcqbrhpk, string wvbvxxqaeltuvo, string qutucmjat, bool sxynv, double mgmqw, bool vxukoqjenxhldmr) {
	bool jasvynoxykc = false;
	int hwhhpztqaks = 93;
	string gqtyyba = "syzkfpnoemqtkhvhmnxemubpnsgfrzqbckmgrnszxmq";
	bool upycl = false;
	bool pecnewi = true;
	if (93 == 93) {
		int zctab;
		for (zctab = 74; zctab > 0; zctab--) {
			continue;
		}
	}
	if (true == true) {
		int xfz;
		for (xfz = 51; xfz > 0; xfz--) {
			continue;
		}
	}
	if (93 != 93) {
		int vxb;
		for (vxb = 75; vxb > 0; vxb--) {
			continue;
		}
	}
	return 27812;
}

void ngskdkh::pjiletawrbaiwalequrgkxa(int pkumeeef, double thsvovzpymv, string atvejintmcir) {
	bool osmchtjuqsjjh = true;
	if (true != true) {
		int opeullj;
		for (opeullj = 55; opeullj > 0; opeullj--) {
			continue;
		}
	}
	if (true != true) {
		int oynsldqcvm;
		for (oynsldqcvm = 89; oynsldqcvm > 0; oynsldqcvm--) {
			continue;
		}
	}

}

string ngskdkh::kvxoowzjtmghqdrhtsio(int efzgjt, bool najse, int limhdmqpbvq, double brbdpzljgtmtxa, double lrioemlhpdaqpd) {
	string nrkexo = "ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv";
	if (string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv") != string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv")) {
		int sf;
		for (sf = 39; sf > 0; sf--) {
			continue;
		}
	}
	if (string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv") != string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv")) {
		int bxdwmc;
		for (bxdwmc = 10; bxdwmc > 0; bxdwmc--) {
			continue;
		}
	}
	if (string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv") != string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv")) {
		int kkhz;
		for (kkhz = 42; kkhz > 0; kkhz--) {
			continue;
		}
	}
	if (string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv") == string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv")) {
		int mhcxvjr;
		for (mhcxvjr = 47; mhcxvjr > 0; mhcxvjr--) {
			continue;
		}
	}
	if (string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv") == string("ldzkddckavczjzgduaybqkskwmtblysnwoqicjskbvexwuliuzeykbuxhijorutpfvmdlkjkmgzkfrnqhucrtyqidyxgv")) {
		int hhedxsh;
		for (hhedxsh = 24; hhedxsh > 0; hhedxsh--) {
			continue;
		}
	}
	return string("qvelrfqzhlrkz");
}

bool ngskdkh::dhoqaztpfflsfijidp(double noitjbqagipuu, int xfcgtkvntytqdiz, double nbvmozixg, string quwvqjsnjvvi, bool cmhbkiucpowm, int aemwcflv, double qjolvkpapo, bool pxschnzjcngpce, double ojvrsiv, string gurcemdq) {
	string eliibmcsyq = "fcxqepibmdcnkhlixlgngyfgpvvhxrlkfpe";
	bool wuiqnmr = true;
	string vlnsfztpmazhmho = "vwffumrjcrqltwr";
	string glywv = "utmdqerlsqxibbamtthcifoyqpbzksmqqnslaewvpndqxalmocmfqjvffx";
	int noibpsfwokdii = 852;
	bool xuldat = false;
	bool vygjjabaaqiofin = false;
	int qatfioedrpe = 578;
	if (578 == 578) {
		int oknc;
		for (oknc = 89; oknc > 0; oknc--) {
			continue;
		}
	}
	if (578 == 578) {
		int fr;
		for (fr = 99; fr > 0; fr--) {
			continue;
		}
	}
	if (true != true) {
		int zxs;
		for (zxs = 66; zxs > 0; zxs--) {
			continue;
		}
	}
	if (578 == 578) {
		int olnkxdewn;
		for (olnkxdewn = 69; olnkxdewn > 0; olnkxdewn--) {
			continue;
		}
	}
	return false;
}

string ngskdkh::qwbssejhpwhvjwrofpcoywrqh() {
	bool voxgkdhybuleif = false;
	bool yxtjfpsw = true;
	return string("myedtfflowbygw");
}

string ngskdkh::qecacvozftzxdnnhdo(double dygwagtj, bool wxfktcvalibjea, int szebmvcltr, string wvrtiudgej, bool vahvujkvuarqe, int pcgfg, double yryrt, string czord, string utybpkvyx, int lvkwvqfywunqlj) {
	int radqdbrplnrmhky = 2092;
	int apprghltevcwfby = 1042;
	string tqisn = "ufxgbmltwnakpua";
	string idnisy = "zzgqgnfwxjsvbykfzeaptiqqhtvbibxtzymkvwjwayuveufkisbhhacccdtckiwoivn";
	string yvgjbfzhvbdcuix = "avqlpfkgssgqiauibvuxnbgdbcywxtkwnndctdevlaoojaqkgcpxiilzzqabujqhpfbomwfarfqgng";
	double ygshkjjt = 15997;
	string pvwfgmqaczfmmbd = "csiejhaaoktpbdmtddnpwsfyymflnaamjuzycwgzaydeahxkhzecieiincpntsslzhkifhizdueyw";
	double yduyj = 16590;
	if (2092 != 2092) {
		int gpcdkqj;
		for (gpcdkqj = 19; gpcdkqj > 0; gpcdkqj--) {
			continue;
		}
	}
	return string("gwxrolnmcvtoyl");
}

double ngskdkh::nnxsenwjosdytvqmyolkoegy() {
	bool loigan = true;
	int dvvolyfif = 1096;
	bool vjbwfrwzbduho = false;
	string fpyvk = "dqdpcoaxydvdalcuntfibufdtaemnewtfruoefveuvfscvbofpwrbcqmmciwsulmrvjmqicqmguiqouonebcoqqoakf";
	double yprazruxhn = 40537;
	double itctbtzpighgfr = 10492;
	string icuzbugte = "tvivyptlrwsnxixekbusvmswzenrdrtpoacurmtjzmovzoyvppzfdlbqcgwpandtmwmeypoknsrthmswjipaundwi";
	string zudccnp = "zwvgumderonidrytowvkhjjhhnrfgrcauzyctebrrxaviiwsbzpodqzucgpqyanqkpzttanvqfvythuirgfwdvphuwnb";
	string illnkv = "gizarffqgangtumsqebuuwfscnjkjlscrsoqqgpjnnycjxofseueqxjdqyusbfewmev";
	bool uhlkpbrvpprba = true;
	return 83243;
}

ngskdkh::ngskdkh() {
	this->kvxoowzjtmghqdrhtsio(676, true, 9738, 44914, 29610);
	this->dhoqaztpfflsfijidp(29589, 2480, 17974, string("ximslkzhistgcgrtjyvmhgnfqknokqqnkzwqrlkvnvoxjvdlyfxjpv"), false, 2566, 15073, true, 762, string("yrdcnxlkkwcxhwfcnjusurqurqgvrnsnrfbltkxncknauvcqlwaftuebnndoclmwhuplxfuglhyouvolfhzzqwsqbqxi"));
	this->qwbssejhpwhvjwrofpcoywrqh();
	this->qecacvozftzxdnnhdo(15476, false, 1780, string("yfetdprkwrlukxkbmhkxryvkoupqejwlfcybkqzfmivwcnmidqberjvmkc"), false, 1041, 28272, string("moyzbjoisdklrqzjxtgjeuhwqefzowpefctsxwtryrrzmbhtsvax"), string("huptqcvuwbaqyfjjnofepdrpwsglmfzjrcdmiiqqkpzxqcoczfsmkcicvcuxhrmjuzaggesbnaqjsurqtybhtvvebzgqtbopgy"), 5652);
	this->nnxsenwjosdytvqmyolkoegy();
	this->gihbzjbknmafzskicuekn(false, 2182, true, string("cxupviyikpovonjloashktpranlkojwbmsrnxvgfulqhhcimjthlxoddajo"));
	this->xkgskkxcyxftmfhuripbmt(6192, true, true, true, true, true);
	this->uqdgazmuxx(28431, true, string("nbtssyabjrqcxrifeitmdekdmfwjbrva"), 14705, string("ecklbvbjbhrxixxhdbkw"), false);
	this->negyggpovvnliacijmhzg(false, string("sozvwfspadlfydbjlagfagrkcmhd"), string("odgqdsfebxzcrwpbyvcimwciezvcndiziotgoxjp"), false, 19986, false);
	this->pjiletawrbaiwalequrgkxa(7553, 44150, string("yqzwkevenlinvryqfnhefkirthtxotcavwcxvqyxkbjnbrezxrmzfp"));
	this->axsftkccixlfaapwqtxcla(string("kuuazqdwmzzacgkitsduoceiknwfnscmggstjhlmdxtsjdffimoqsokupdfduvvbrgjonvxlbtjix"), 5536, 46856, string("fmwvpnfwncxnjagtzbtbzvfhgfnibumkjaoqniwtpicfyja"), 611);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ckrhqei {
public:
	bool khegy;
	int kgenxxrrdnyova;
	bool zliwhrkou;
	bool gupktyb;
	int fifoyj;
	ckrhqei();
	bool kcymghnrqxexxxqrndu(bool evrvgxhronclekv, int uvxdgeosdp, int ojixjstfdtowyut, double wninnkjjocezt, double kypqrstvxm, string ubawavfzrel, double aqbgvnzjpknfj);
	string imrdctnbuwi();
	double dnkcivrtlpvagrqd(string viriffisvcntxh, double icuwu, string ewyfcbaop, bool xxgfdoxqozy, bool xbjwjgftdkrhvqm, int vojbupv, int pmhknqsa);
	string buvowfddlfhjsyc(string gzddllfrrfvuio, bool wotcadfxor, string lvsizremlwkm, string ycvqxqctesbdxk, double lurfzntjahi, int dsfzneic);
	double tdnxatpxysowfxitsatul(string jfzojfek, string fgisesgnjjjvckc, string wyucsgomgk, double sueitmrfmqirx, double ijqxkjggeobosq, string iryhfdnmwvm, string iidantn, int uiinwesomzwu, string hhtkispbbjbm);
	int lqsfdevvllwrblicickwrqsnh(int vccoqjlzda, bool bzzkwqitxbxyuzh, string vjeiraemp, string lyqjwanxkekd, double qmutgmv, string eiqawkuecw, double czmetkq, string tznwmaryx);
	string yxsmeeluhzzddxenjtz(string zmlesru, double hdhtlhedaqtso);
	void qtryrfwgsbyssslnmkc(bool qzaim, double pocnwiri, int qmppwhp, string bukqtokvwzofcx, bool twstlkmrkqauxao, int zejmzeqsbaqcb, int xbuqcfch, int znbdejvb);
	bool xrypvvjsoiwlgixkackcqiddg(bool prgfapdcrs, double cpoebxvu, int nbwvbqgcxecp, bool tpscic, int esmftjgbvp, string gvbicvjjgbum, double zwgoyowkz, int eykqilwnv);

protected:
	int jaxffk;
	double qgemsyosv;
	string yhbyayfsp;
	double poqchalcvoi;

	int lwzhsxaekzxarlg(double yqbdptkffotwak, double ypytukzaer, int pffmgastv, int mfgqkg, bool wmrggcgfif, int dopwuvm, double makwmuf);
	string ggbarhwnpxbakgcnr(double rfkiytupwueqm, string wrbxdlduag, int mkgdkafunwnoor, int ppipbsollvohhb, bool hmior, int svyapctlf, bool mgpbyinoivd);
	int jnnfjjzdspalzj(double npspt, double ppzfxlnpvo, int psqanqmuap, bool caprw, double wwqinij, double lpwdnixjv, string pgrvv);
	int gefizcvdhdzgzrovwpdtim(bool omdasvgwsae, int eyoyixbklvisrn, double pckll, bool pmscyehah, int wfndnvjfedvara, string ilimaeilga, bool soaxajpswydpxls);
	double gabsbyrlkzxhj(int jkgbhsrcokvogi);
	void ungrzhimvjdpezptlphne(int igtxznuvhem, double hfzdp, string pbtahdqvjh, double vjumjpougioc, int tzzuhheotdogb, int hizjt);

private:
	double huzdozsajmt;
	bool tkqvexgeilu;
	double wnbha;

	int pihdblichytuedlforpdvzfmx(double sqwntbyrdxqmyt, int qfextyx, string lgnfu);
	double uvdylgepavxoy();
	string zspsddmozipupjyldxhlwnlgu(double dbwgurlynpjkx, double fndjzpps, double jofwja);
	int tljuszxmnpzku(double thshkukjnbsy, double pjjrjdsbi, bool playgv, double xghrnc, double sfyvdysggr, bool cjgrogwv);
	bool msbbphjgfez(string nwubnleko, string txqynegecn, bool mimgidojnq, double qszfsbpnehyntr, int xbylhx, double adxsaalntfamc, string kkixl, double zthjjqpairnah, bool dvzdzj);
	void spkhwfhhaunpxeeffonccfmva(int gvdcnba, double ftdbfrfhyiywy, int zoxjzsnc);
	string fnudcmenwtvmrblrpdxlxzz(double ktphdqvtyft, double utmzqml, double xzlcpiohfcunj, int bdakl, bool lrehyrzt, string fbdndzcpvez);
	double kiuvjsctzjerngxuxnzlweg(int oskksgdvc, double xexxnfilh, double wptwednke, int hewycv, int ftxkcsh);

};


int ckrhqei::pihdblichytuedlforpdvzfmx(double sqwntbyrdxqmyt, int qfextyx, string lgnfu) {
	int cfmdkc = 5629;
	bool tnrgavjjwrhhkxn = false;
	string gmqju = "nzkbvgxvakbmllvapdwdnygcjolemscoxkdobnxzx";
	if (5629 == 5629) {
		int mzbrbst;
		for (mzbrbst = 100; mzbrbst > 0; mzbrbst--) {
			continue;
		}
	}
	if (false != false) {
		int rg;
		for (rg = 6; rg > 0; rg--) {
			continue;
		}
	}
	if (5629 == 5629) {
		int rsrthzba;
		for (rsrthzba = 85; rsrthzba > 0; rsrthzba--) {
			continue;
		}
	}
	return 74111;
}

double ckrhqei::uvdylgepavxoy() {
	int cmogrut = 352;
	string yrpoyvk = "nqmhuctdodxoymoheripqxvjrgnlubaucpbaixscvplzaerlbobqzpnveviehhjipqeaacjwgsbujwhxmcdiztczwjfaj";
	bool apylozbkfgojl = true;
	int pexyiywbktamoi = 159;
	bool pouoq = false;
	return 14223;
}

string ckrhqei::zspsddmozipupjyldxhlwnlgu(double dbwgurlynpjkx, double fndjzpps, double jofwja) {
	bool pquahrtwut = true;
	bool jwukdcuenoops = true;
	bool uafqckzbbfxidc = true;
	string aggwirailgbbq = "hhssxlcwcjoqubqridtgvrjefulyztcxsmwxvmzpqpawpnunwjpttc";
	bool wptrggku = true;
	string eizwybbnsdodiz = "mwixckepjdoyzggiovbzmgthlbvivtbryd";
	string kmyeqhn = "oummeadxkslqnbxaxzzpobxvgkkvqypjersbpjder";
	return string("lplpucbmugkhvojehh");
}

int ckrhqei::tljuszxmnpzku(double thshkukjnbsy, double pjjrjdsbi, bool playgv, double xghrnc, double sfyvdysggr, bool cjgrogwv) {
	double hbcntippfo = 4822;
	double vlcnuxhvofisds = 19348;
	bool rpjqdjniqxrc = true;
	int icaumcrciumh = 2619;
	if (2619 == 2619) {
		int epxsyy;
		for (epxsyy = 45; epxsyy > 0; epxsyy--) {
			continue;
		}
	}
	if (19348 != 19348) {
		int wsabamv;
		for (wsabamv = 77; wsabamv > 0; wsabamv--) {
			continue;
		}
	}
	if (true == true) {
		int ovt;
		for (ovt = 67; ovt > 0; ovt--) {
			continue;
		}
	}
	return 20321;
}

bool ckrhqei::msbbphjgfez(string nwubnleko, string txqynegecn, bool mimgidojnq, double qszfsbpnehyntr, int xbylhx, double adxsaalntfamc, string kkixl, double zthjjqpairnah, bool dvzdzj) {
	bool nhrvrp = true;
	int rsrmsba = 1675;
	string qptmgafkzuhjz = "bppokdudszmvzrwzmgyayy";
	bool uznrpesolurwtap = true;
	int wjxaajvwbthped = 154;
	double ypdxkggaojahqvz = 1011;
	int myoozjooflqmfl = 6406;
	string wmqppjbmt = "yfkgfgjtonemxusnwxcwutclrihmldkzptfebfkbeanuucwgzrvelewwiunthlwjjol";
	string iopqikjlupm = "wgmlhpxmxruwxengdlsuqmqnadnxsmfoxkuscvtckaftguketqgsuwybxrdcxvzfvfnzcbbc";
	if (1011 == 1011) {
		int uc;
		for (uc = 78; uc > 0; uc--) {
			continue;
		}
	}
	if (1011 == 1011) {
		int itrxut;
		for (itrxut = 69; itrxut > 0; itrxut--) {
			continue;
		}
	}
	if (true != true) {
		int xaewzkdkd;
		for (xaewzkdkd = 52; xaewzkdkd > 0; xaewzkdkd--) {
			continue;
		}
	}
	if (154 != 154) {
		int ahph;
		for (ahph = 64; ahph > 0; ahph--) {
			continue;
		}
	}
	return false;
}

void ckrhqei::spkhwfhhaunpxeeffonccfmva(int gvdcnba, double ftdbfrfhyiywy, int zoxjzsnc) {
	int vveojyflc = 4319;
	double qcgsiiakjovu = 57596;
	double ryccd = 569;
	int lqayzhdekco = 4643;
	double bsiowgtuserd = 87038;
	string rutsktmemfkjor = "vtvgqahnbbazuvpjvciagdcjaqremzhrghjcnkaecczutw";
	int wzalurjlxijbwq = 7679;
	if (4643 != 4643) {
		int nnymi;
		for (nnymi = 54; nnymi > 0; nnymi--) {
			continue;
		}
	}
	if (57596 != 57596) {
		int jaorou;
		for (jaorou = 98; jaorou > 0; jaorou--) {
			continue;
		}
	}
	if (4643 == 4643) {
		int demmmdr;
		for (demmmdr = 44; demmmdr > 0; demmmdr--) {
			continue;
		}
	}
	if (4643 != 4643) {
		int tjjey;
		for (tjjey = 85; tjjey > 0; tjjey--) {
			continue;
		}
	}

}

string ckrhqei::fnudcmenwtvmrblrpdxlxzz(double ktphdqvtyft, double utmzqml, double xzlcpiohfcunj, int bdakl, bool lrehyrzt, string fbdndzcpvez) {
	string vyhhdjynxhcs = "vhbvkbfcnpxzpzyhatkdxiwllyke";
	int phxyrhxznthk = 6632;
	double vqjvkk = 83141;
	if (6632 != 6632) {
		int vyuyzjur;
		for (vyuyzjur = 13; vyuyzjur > 0; vyuyzjur--) {
			continue;
		}
	}
	if (6632 != 6632) {
		int vqfhvcvj;
		for (vqfhvcvj = 89; vqfhvcvj > 0; vqfhvcvj--) {
			continue;
		}
	}
	if (6632 == 6632) {
		int tuxj;
		for (tuxj = 71; tuxj > 0; tuxj--) {
			continue;
		}
	}
	if (string("vhbvkbfcnpxzpzyhatkdxiwllyke") == string("vhbvkbfcnpxzpzyhatkdxiwllyke")) {
		int zvyddfu;
		for (zvyddfu = 99; zvyddfu > 0; zvyddfu--) {
			continue;
		}
	}
	if (83141 != 83141) {
		int mslzqbpyc;
		for (mslzqbpyc = 40; mslzqbpyc > 0; mslzqbpyc--) {
			continue;
		}
	}
	return string("qocvqjqtxeummgqz");
}

double ckrhqei::kiuvjsctzjerngxuxnzlweg(int oskksgdvc, double xexxnfilh, double wptwednke, int hewycv, int ftxkcsh) {
	bool bbhajito = true;
	if (true == true) {
		int ujhoxttse;
		for (ujhoxttse = 21; ujhoxttse > 0; ujhoxttse--) {
			continue;
		}
	}
	if (true == true) {
		int nd;
		for (nd = 71; nd > 0; nd--) {
			continue;
		}
	}
	if (true != true) {
		int gyspfphf;
		for (gyspfphf = 63; gyspfphf > 0; gyspfphf--) {
			continue;
		}
	}
	if (true == true) {
		int lsl;
		for (lsl = 97; lsl > 0; lsl--) {
			continue;
		}
	}
	return 40555;
}

int ckrhqei::lwzhsxaekzxarlg(double yqbdptkffotwak, double ypytukzaer, int pffmgastv, int mfgqkg, bool wmrggcgfif, int dopwuvm, double makwmuf) {
	double brqecqgozxdvj = 9979;
	if (9979 != 9979) {
		int riqkktjfa;
		for (riqkktjfa = 17; riqkktjfa > 0; riqkktjfa--) {
			continue;
		}
	}
	if (9979 == 9979) {
		int fzrhz;
		for (fzrhz = 88; fzrhz > 0; fzrhz--) {
			continue;
		}
	}
	return 27999;
}

string ckrhqei::ggbarhwnpxbakgcnr(double rfkiytupwueqm, string wrbxdlduag, int mkgdkafunwnoor, int ppipbsollvohhb, bool hmior, int svyapctlf, bool mgpbyinoivd) {
	string jdbwmeb = "ckdrisxhaqiavvbkhjimdekomtzurjduyjh";
	string ivrpdzijkiglkx = "scuwbrykwoibzvyszvpcyiojwittxudcrbhctaqcbcwjggloasgavonjsrqikpuxpidxronvheyzifugdqpdrhkodlg";
	if (string("scuwbrykwoibzvyszvpcyiojwittxudcrbhctaqcbcwjggloasgavonjsrqikpuxpidxronvheyzifugdqpdrhkodlg") == string("scuwbrykwoibzvyszvpcyiojwittxudcrbhctaqcbcwjggloasgavonjsrqikpuxpidxronvheyzifugdqpdrhkodlg")) {
		int qmadtv;
		for (qmadtv = 66; qmadtv > 0; qmadtv--) {
			continue;
		}
	}
	return string("ptfqhw");
}

int ckrhqei::jnnfjjzdspalzj(double npspt, double ppzfxlnpvo, int psqanqmuap, bool caprw, double wwqinij, double lpwdnixjv, string pgrvv) {
	double vgfnzkehwg = 12769;
	double otvkq = 33202;
	bool gvqvgi = true;
	int qmfzdiqqwai = 1103;
	double qzthnzgetdkdldm = 586;
	int iffesjv = 4607;
	bool wgbuyffyl = false;
	bool fcfjrcmk = true;
	bool dwotxuofslruox = true;
	double bdpbtnrqxzhabs = 63700;
	if (4607 != 4607) {
		int vycfyrvgh;
		for (vycfyrvgh = 42; vycfyrvgh > 0; vycfyrvgh--) {
			continue;
		}
	}
	return 7023;
}

int ckrhqei::gefizcvdhdzgzrovwpdtim(bool omdasvgwsae, int eyoyixbklvisrn, double pckll, bool pmscyehah, int wfndnvjfedvara, string ilimaeilga, bool soaxajpswydpxls) {
	int qkmawpmqvp = 2434;
	string iampxpufebyxd = "vanttifvoazqcxfqdiiwgyp";
	if (2434 != 2434) {
		int drtuc;
		for (drtuc = 70; drtuc > 0; drtuc--) {
			continue;
		}
	}
	if (2434 != 2434) {
		int riett;
		for (riett = 54; riett > 0; riett--) {
			continue;
		}
	}
	return 25843;
}

double ckrhqei::gabsbyrlkzxhj(int jkgbhsrcokvogi) {
	bool lrrmbhzhvn = true;
	string rmhjcrzkavgeu = "jpmyngdkvtbrmfdpdlmkrbofqhtxrqxinasqasitoybhwuhjfuwgrwxomyguvuknsioui";
	string hhzhhcy = "ryytcgiljdpryfyzqcymlggwwxvkcvhwzqayshabs";
	double nsoigrenl = 1464;
	bool pgsmxfubn = false;
	string pqebrmrapme = "mhmmk";
	return 34510;
}

void ckrhqei::ungrzhimvjdpezptlphne(int igtxznuvhem, double hfzdp, string pbtahdqvjh, double vjumjpougioc, int tzzuhheotdogb, int hizjt) {
	bool gsafdulku = true;
	double fscneyltnbwlgve = 36381;
	bool muweqykofkztm = true;
	double sanas = 71448;
	if (true == true) {
		int fjnxvazt;
		for (fjnxvazt = 83; fjnxvazt > 0; fjnxvazt--) {
			continue;
		}
	}
	if (true != true) {
		int kbidzsf;
		for (kbidzsf = 37; kbidzsf > 0; kbidzsf--) {
			continue;
		}
	}
	if (true == true) {
		int kpjuwqhe;
		for (kpjuwqhe = 96; kpjuwqhe > 0; kpjuwqhe--) {
			continue;
		}
	}

}

bool ckrhqei::kcymghnrqxexxxqrndu(bool evrvgxhronclekv, int uvxdgeosdp, int ojixjstfdtowyut, double wninnkjjocezt, double kypqrstvxm, string ubawavfzrel, double aqbgvnzjpknfj) {
	string lpmuisdnnwnjxn = "asjdwppdnpahzeaolrbvtfkgwmrtitfjwmaluuxbjskaopckhwilaostm";
	bool qdoyy = true;
	double qrwplmooll = 44314;
	double qehmqpz = 10334;
	int rilzrzbu = 7059;
	double uzvjcgnrawffks = 81446;
	double bgenflczkl = 9697;
	double pessfpqypbz = 3353;
	int qyldsurvyzygbfw = 2864;
	if (9697 == 9697) {
		int rjoppyde;
		for (rjoppyde = 29; rjoppyde > 0; rjoppyde--) {
			continue;
		}
	}
	if (44314 != 44314) {
		int invkhgcwe;
		for (invkhgcwe = 80; invkhgcwe > 0; invkhgcwe--) {
			continue;
		}
	}
	if (44314 != 44314) {
		int ocrugn;
		for (ocrugn = 56; ocrugn > 0; ocrugn--) {
			continue;
		}
	}
	if (9697 == 9697) {
		int viw;
		for (viw = 93; viw > 0; viw--) {
			continue;
		}
	}
	if (true == true) {
		int igcvfuinq;
		for (igcvfuinq = 95; igcvfuinq > 0; igcvfuinq--) {
			continue;
		}
	}
	return false;
}

string ckrhqei::imrdctnbuwi() {
	double ilnwjoohohlvfx = 18231;
	string btuix = "pyhbkmxsascvkbpesdlhbmtykgnuxfhhxtpynjfqnzzhxqllcnopkdzrppptfarrrzculaqklckufhnjuqbifbcd";
	string kicrdmjptkmm = "spfppwrpmiphgfeenhzaemnzgtsogufwiordeuouibmnmkwgtxyhusnnxxsuxmutapgdvbjqiwzeyf";
	bool fxzmrqmx = true;
	string tdprtrh = "luxrsrqyebfnpczimexijggtmsvydpomuvptzzqyajufimmfhoqfjnrztthgymvukgvurwsdefirvoevmbyppatxmsqkjtbx";
	double jnvsi = 63475;
	double qwzkeuwkxr = 11837;
	int pborl = 849;
	string odagctizahejpjp = "kkbfhths";
	double bhkarilx = 2893;
	if (string("pyhbkmxsascvkbpesdlhbmtykgnuxfhhxtpynjfqnzzhxqllcnopkdzrppptfarrrzculaqklckufhnjuqbifbcd") == string("pyhbkmxsascvkbpesdlhbmtykgnuxfhhxtpynjfqnzzhxqllcnopkdzrppptfarrrzculaqklckufhnjuqbifbcd")) {
		int sl;
		for (sl = 20; sl > 0; sl--) {
			continue;
		}
	}
	if (string("luxrsrqyebfnpczimexijggtmsvydpomuvptzzqyajufimmfhoqfjnrztthgymvukgvurwsdefirvoevmbyppatxmsqkjtbx") == string("luxrsrqyebfnpczimexijggtmsvydpomuvptzzqyajufimmfhoqfjnrztthgymvukgvurwsdefirvoevmbyppatxmsqkjtbx")) {
		int mtuibkix;
		for (mtuibkix = 8; mtuibkix > 0; mtuibkix--) {
			continue;
		}
	}
	return string("nfy");
}

double ckrhqei::dnkcivrtlpvagrqd(string viriffisvcntxh, double icuwu, string ewyfcbaop, bool xxgfdoxqozy, bool xbjwjgftdkrhvqm, int vojbupv, int pmhknqsa) {
	bool wceimjs = true;
	bool vaujlmodcsxvo = true;
	if (true != true) {
		int kgc;
		for (kgc = 46; kgc > 0; kgc--) {
			continue;
		}
	}
	if (true != true) {
		int pdyqqqow;
		for (pdyqqqow = 25; pdyqqqow > 0; pdyqqqow--) {
			continue;
		}
	}
	return 1231;
}

string ckrhqei::buvowfddlfhjsyc(string gzddllfrrfvuio, bool wotcadfxor, string lvsizremlwkm, string ycvqxqctesbdxk, double lurfzntjahi, int dsfzneic) {
	double sxkiqifrefgtzrq = 14040;
	int vesrepvaxxctsen = 2829;
	if (14040 == 14040) {
		int yrvimndybj;
		for (yrvimndybj = 13; yrvimndybj > 0; yrvimndybj--) {
			continue;
		}
	}
	return string("xpscmkiuh");
}

double ckrhqei::tdnxatpxysowfxitsatul(string jfzojfek, string fgisesgnjjjvckc, string wyucsgomgk, double sueitmrfmqirx, double ijqxkjggeobosq, string iryhfdnmwvm, string iidantn, int uiinwesomzwu, string hhtkispbbjbm) {
	double rcqniahugue = 8850;
	int hhpvqlqaugnch = 232;
	int wsatofxtwqdpgy = 212;
	int tuemtlwzjcev = 82;
	double diqmkxjjipwr = 1203;
	double lcbbedqgvfmwof = 32555;
	int cxvbtdqbgk = 1364;
	return 71483;
}

int ckrhqei::lqsfdevvllwrblicickwrqsnh(int vccoqjlzda, bool bzzkwqitxbxyuzh, string vjeiraemp, string lyqjwanxkekd, double qmutgmv, string eiqawkuecw, double czmetkq, string tznwmaryx) {
	double kaumlcwjncqaha = 27570;
	if (27570 == 27570) {
		int cbvvdmrq;
		for (cbvvdmrq = 64; cbvvdmrq > 0; cbvvdmrq--) {
			continue;
		}
	}
	if (27570 == 27570) {
		int mhb;
		for (mhb = 41; mhb > 0; mhb--) {
			continue;
		}
	}
	if (27570 == 27570) {
		int oc;
		for (oc = 26; oc > 0; oc--) {
			continue;
		}
	}
	if (27570 != 27570) {
		int avsmh;
		for (avsmh = 86; avsmh > 0; avsmh--) {
			continue;
		}
	}
	if (27570 == 27570) {
		int yhasiay;
		for (yhasiay = 19; yhasiay > 0; yhasiay--) {
			continue;
		}
	}
	return 68294;
}

string ckrhqei::yxsmeeluhzzddxenjtz(string zmlesru, double hdhtlhedaqtso) {
	bool ilhrrmawvc = true;
	int vpsqligfgul = 1578;
	string ruudswrtrsi = "apnqzamuewaesjrgjjzggbrxnuqnnnyfmtihbcjsaksjoaurlhbcpxartwosc";
	string ivvaoyu = "enjneqwsgpafqqdmsmqysnohfudmdzedyclqjkeccujrdrvxvfhjibcttmvavmxshvfq";
	string pgnrjrdzq = "endafkeeypvrsqijzhxhgruvlbjldewirzkccblsnfwualxgrghczca";
	if (1578 == 1578) {
		int ctyzp;
		for (ctyzp = 75; ctyzp > 0; ctyzp--) {
			continue;
		}
	}
	if (string("enjneqwsgpafqqdmsmqysnohfudmdzedyclqjkeccujrdrvxvfhjibcttmvavmxshvfq") == string("enjneqwsgpafqqdmsmqysnohfudmdzedyclqjkeccujrdrvxvfhjibcttmvavmxshvfq")) {
		int ocnpjaagvu;
		for (ocnpjaagvu = 89; ocnpjaagvu > 0; ocnpjaagvu--) {
			continue;
		}
	}
	if (string("apnqzamuewaesjrgjjzggbrxnuqnnnyfmtihbcjsaksjoaurlhbcpxartwosc") != string("apnqzamuewaesjrgjjzggbrxnuqnnnyfmtihbcjsaksjoaurlhbcpxartwosc")) {
		int vfla;
		for (vfla = 52; vfla > 0; vfla--) {
			continue;
		}
	}
	if (string("enjneqwsgpafqqdmsmqysnohfudmdzedyclqjkeccujrdrvxvfhjibcttmvavmxshvfq") != string("enjneqwsgpafqqdmsmqysnohfudmdzedyclqjkeccujrdrvxvfhjibcttmvavmxshvfq")) {
		int hiyqw;
		for (hiyqw = 63; hiyqw > 0; hiyqw--) {
			continue;
		}
	}
	if (1578 != 1578) {
		int uffnfqlkt;
		for (uffnfqlkt = 85; uffnfqlkt > 0; uffnfqlkt--) {
			continue;
		}
	}
	return string("lefpgrzqjipvtwwxty");
}

void ckrhqei::qtryrfwgsbyssslnmkc(bool qzaim, double pocnwiri, int qmppwhp, string bukqtokvwzofcx, bool twstlkmrkqauxao, int zejmzeqsbaqcb, int xbuqcfch, int znbdejvb) {
	int ttakgefpjpix = 1595;
	string nrgymidt = "cqwxhjcltqsppworpcuw";
	bool ozzehdqod = true;
	bool nkbzv = false;
	double xbvkxdqtdoabzlf = 26372;
	if (1595 != 1595) {
		int nopspcrc;
		for (nopspcrc = 55; nopspcrc > 0; nopspcrc--) {
			continue;
		}
	}

}

bool ckrhqei::xrypvvjsoiwlgixkackcqiddg(bool prgfapdcrs, double cpoebxvu, int nbwvbqgcxecp, bool tpscic, int esmftjgbvp, string gvbicvjjgbum, double zwgoyowkz, int eykqilwnv) {
	int caidkeyhuns = 5941;
	string eqtvqjbtbw = "fvdwapymhznkmzlkvmcjxorzljryuyjusfjkufumlegtioulvpnczmwkupciaxhlkoprikcmlfsjtrtucsyyctn";
	if (5941 != 5941) {
		int jpjxvfsil;
		for (jpjxvfsil = 2; jpjxvfsil > 0; jpjxvfsil--) {
			continue;
		}
	}
	if (5941 != 5941) {
		int lzln;
		for (lzln = 62; lzln > 0; lzln--) {
			continue;
		}
	}
	if (string("fvdwapymhznkmzlkvmcjxorzljryuyjusfjkufumlegtioulvpnczmwkupciaxhlkoprikcmlfsjtrtucsyyctn") == string("fvdwapymhznkmzlkvmcjxorzljryuyjusfjkufumlegtioulvpnczmwkupciaxhlkoprikcmlfsjtrtucsyyctn")) {
		int mnu;
		for (mnu = 93; mnu > 0; mnu--) {
			continue;
		}
	}
	if (5941 != 5941) {
		int bfzvpzdev;
		for (bfzvpzdev = 15; bfzvpzdev > 0; bfzvpzdev--) {
			continue;
		}
	}
	if (5941 == 5941) {
		int elwo;
		for (elwo = 74; elwo > 0; elwo--) {
			continue;
		}
	}
	return true;
}

ckrhqei::ckrhqei() {
	this->kcymghnrqxexxxqrndu(true, 1912, 4518, 56427, 57842, string("lkgbjwohnijb"), 39242);
	this->imrdctnbuwi();
	this->dnkcivrtlpvagrqd(string("fotexbooqdoturjoimmvvxnuyrnirhkckpiuejzuxwnuoebddjcmhnrwgklw"), 55866, string("lwhwkhuldxrvgycztyjmjaraeihgabmjhznfqrnhjvpkpndbilhsrd"), true, false, 3277, 1345);
	this->buvowfddlfhjsyc(string("nwsejieuycdczhuwnbvhculalqzdtoxptqtxnhkwqpatgspiietufjqte"), false, string("lifnmyrmovs"), string("cjw"), 63288, 7015);
	this->tdnxatpxysowfxitsatul(string("jpfhphcdbqlxoemsetatkcyirwbozaszhpkekwdbgpqzlwiswoxfcehkrmvgoybnekxtrfmqerrkrbjdnwbpevclrld"), string("wqsbnyztmtekvyxixzbyjkisnewcavlkeeqbejbynitrlxhgspgenmpgimgpdddybycapsfsfffcmihqcaiee"), string("clqienarkqnkllwtcxjpaezzggrshbnimgibtpwyfvkdrq"), 73784, 6932, string("vcishfuzxehzspqmfnzzxmasenbeqalfjssctdkzqbqgznighlbclmshxksvqxefipammegzox"), string("syhoqznxojwdstibhfzfpnpgrmomjseppgpzwznovwwpdguoenru"), 6217, string("woshruvcsjeadkqfvilxoswmlbcpkzkzhmfbfde"));
	this->lqsfdevvllwrblicickwrqsnh(1537, false, string("jhjknpwbtfkbulaoguhuyhmmcxhdrcgntbyqvlj"), string("pgmqrgoyimokryjqybudegmawefgimacdnbsscqzuuclhlehjatlzam"), 5706, string("yzvwiezjksrrsmcuahoq"), 73512, string("uua"));
	this->yxsmeeluhzzddxenjtz(string("izrljsywjylloodaoclstr"), 30461);
	this->qtryrfwgsbyssslnmkc(true, 21275, 1405, string("vldpvvbuttifjcqhjdqsjmalpsopuwuozkxzlhsjwojarfpbnnsjsjilqqkfdofhowpknjukberozjineennlsyzalojqpfdd"), false, 4929, 4486, 3950);
	this->xrypvvjsoiwlgixkackcqiddg(false, 31364, 5289, true, 2957, string("fidloiovtqfe"), 20429, 2112);
	this->lwzhsxaekzxarlg(29155, 18984, 3463, 8283, true, 4507, 46513);
	this->ggbarhwnpxbakgcnr(13860, string("etylgjmgxoosgwusezgisybuwhvdkgnwfloldmlwnhgfoonrqmaywksnuxhfvwtzslmnrch"), 6300, 503, false, 6459, true);
	this->jnnfjjzdspalzj(38644, 22381, 1699, false, 2375, 64651, string("sogphhvcxlgvhtryxxletjnkhelbeyfgranmklmwwxugogoyutfnwoaixqsvrrbgfjtlgyxmfhvrljxefatqlkpqicinjop"));
	this->gefizcvdhdzgzrovwpdtim(true, 1220, 14668, false, 2303, string("wymvtjzcauqalinkghrxus"), true);
	this->gabsbyrlkzxhj(2699);
	this->ungrzhimvjdpezptlphne(7519, 2395, string("qrtcdfwgsyjaghuzycibzfdnbcvmyzmbkyeeanrfzjuyrjqmlqhv"), 28874, 1660, 1758);
	this->pihdblichytuedlforpdvzfmx(60430, 2440, string("xbyjrajjjrwwlikufzgchwyogyrmvyleysvzpmysj"));
	this->uvdylgepavxoy();
	this->zspsddmozipupjyldxhlwnlgu(5465, 29551, 847);
	this->tljuszxmnpzku(6432, 74046, false, 17358, 24410, false);
	this->msbbphjgfez(string("xouognfeuiyquumcdkwlbqwh"), string("adznrfbdhytyrogbnesgdmpobqonftztdivmcygmuvcobjenmohnjhyuqhp"), true, 13125, 2599, 10100, string("iinjcbytnxcsnqotrwlkxwsczfdvaqtgklkfxsgbvrrvwqrbxeteco"), 6628, false);
	this->spkhwfhhaunpxeeffonccfmva(3035, 11684, 269);
	this->fnudcmenwtvmrblrpdxlxzz(50977, 43200, 7254, 3864, true, string("vhhurnalziddlgwld"));
	this->kiuvjsctzjerngxuxnzlweg(4282, 2774, 19871, 5777, 1384);
}


















































































































































































































































































