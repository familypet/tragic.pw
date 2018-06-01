#pragma once


// Includes
#include "Utilities.h"
#include <fstream>
#include <PsapI.h>

bool FileLog = false;
std::ofstream logFile;

// --------         U Core           ------------ //
// Opens a debug console
void  U::OpenConsole(std::string Title)
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	SetConsoleTitle(Title.c_str());
}

// Closes the debug console
void  U::CloseConsole()
{
	FreeConsole();
}

// Outputs text to the console
void  U::Log(const char *fmt, ...)
{
	if (!fmt) return; //if the passed string is null return
	if (strlen(fmt) < 2) return;

	//Set up va_list and buffer to hold the params 
	va_list va_alist;
	char logBuf[256] = { 0 };

	//Do sprintf with the parameters
	va_start(va_alist, fmt);
	_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
	va_end(va_alist);

	//Output to console
	if (logBuf[0] != '\0')
	{
		SetConsoleColor(FOREGROUND_INTENSE_RED);
		printf("[%s]", GetTimeString().c_str());
		SetConsoleColor(FOREGROUND_WHITE);
		printf(": %s\n", logBuf);
	}

	if (FileLog)
	{
		logFile << logBuf << std::endl;
	}
}

// Gets the current time as a string
std::string  U::GetTimeString()
{
	//Time related variables
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];

	//Get current time
	time(&current_time);
	time_info = localtime(&current_time);

	//Get current time as string
	strftime(timeString, sizeof(timeString), "%I:%M%p", time_info);
	return timeString;
}

// Sets the console color for upcoming text
void  U::SetConsoleColor(WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Enables writing all log calls to a file
void  U::EnableLogFile(std::string filename)
{
	logFile.open(filename.c_str());
	if (logFile.is_open())
		FileLog = true;
}


// --------         U Memory           ------------ //

DWORD U::WaitOnModuleHandle(std::string moduleName)
{
	DWORD ModuleHandle = NULL;
	while (!ModuleHandle)
	{
		ModuleHandle = (DWORD)GetModuleHandle(moduleName.c_str());
		if (!ModuleHandle)
			Sleep(50);
	}
	return ModuleHandle;
}

bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask)
{
	for (; *szMask; ++szMask, ++Mask, ++Data)
	{
		if (*szMask == 'x' && *Mask != *Data)
		{
			return false;
		}
	}
	return (*szMask) == 0;
}


DWORD U::FindPattern(std::string moduleName, BYTE* Mask, char* szMask)
{
	DWORD Address = WaitOnModuleHandle(moduleName.c_str());
	MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
	DWORD Length = ModInfo.SizeOfImage;
	for (DWORD c = 0; c < Length; c += 1)
	{
		if (bCompare((BYTE*)(Address + c), Mask, szMask))
		{
			return DWORD(Address + c);
		}
	}
	return 0;
}

DWORD U::FindTextPattern(std::string moduleName, char* string)
{
	DWORD Address = WaitOnModuleHandle(moduleName.c_str());
	MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
	DWORD Length = ModInfo.SizeOfImage;

	int len = strlen(string);
	char* szMask = new char[len + 1];
	for (int i = 0; i < len; i++)
	{
		szMask[i] = 'x';
	}
	szMask[len] = '\0';

	for (DWORD c = 0; c < Length; c += 1)
	{
		if (bCompare((BYTE*)(Address + c), (BYTE*)string, szMask))
		{
			return (DWORD)(Address + c);
		}
	}
	return 0;
}

#pragma warning( disable : 4018 )  
#pragma warning( disable : 4348 )  

bool U::bin_match(uint8_t *code, std::vector< uint8_t > &pattern)
{
	for (int j = 0; j < pattern.size(); j++)
	{
		if (!pattern[j] && code[j] != pattern[j])
		{
			return false;
		}
	}

	return true;
}

template< typename T = uintptr_t > static T U::first_match(uintptr_t start, std::string sig, size_t len)
{
	std::istringstream iss(sig);
	std::vector< std::string > tokens{ std::istream_iterator< std::string >{ iss }, std::istream_iterator< std::string >{} };
	std::vector< uint8_t > sig_bytes;

	for (auto hex_byte : tokens)
	{
		sig_bytes.push_back(std::strtoul(hex_byte.c_str(), nullptr, 16));
	}

	if (sig_bytes.empty() || sig_bytes.size() < 2)
	{
		return T{};
	}

	for (size_t i{}; i < len; i++)
	{
		uint8_t *code_ptr = reinterpret_cast< uint8_t * >(start + i);

		if (code_ptr[0] != sig_bytes.at(0))
		{
			continue;
		}

		if (bin_match(code_ptr, sig_bytes))
		{
			return((T)(start + i));
		}
	}

	return T{};
}

template< typename T = uintptr_t > static T U::first_code_match(HMODULE start, std::string sig)
{
	auto ntoskrnl = reinterpret_cast< PIMAGE_DOS_HEADER >(start);

	if (ntoskrnl->e_magic != 0x5a4d)
	{
		return T{};
	}

	auto nt_hdrs = reinterpret_cast< PIMAGE_NT_HEADERS >(reinterpret_cast< uintptr_t >(ntoskrnl) + ntoskrnl->e_lfanew);

	return first_match< T >(reinterpret_cast< uintptr_t >(ntoskrnl) + nt_hdrs->OptionalHeader.BaseOfCode, sig, nt_hdrs->OptionalHeader.SizeOfCode);
}

std::uint8_t* U::pattern_scan(void* module, const char* signature)
{
    static auto pattern_to_byte = [](const char* pattern) {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else {
                bytes.push_back(strtoul(current, &current, 16));
            }
        }
        return bytes;
    };

    auto dosHeader = (PIMAGE_DOS_HEADER)module;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = pattern_to_byte(signature);
    auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = patternBytes.size();
    auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i) {
        bool found = true;
        for (auto j = 0ul; j < s; ++j) {
            if (scanBytes[i + j] != d[j] && d[j] != -1) {
                found = false;
                break;
            }
        }
        if (found) {
            return &scanBytes[i];
        }
    }
    return nullptr;
}



vfunc_hook::vfunc_hook()
    : class_base(nullptr), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
vfunc_hook::vfunc_hook(void* base)
    : class_base(base), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
vfunc_hook::~vfunc_hook()
{
    unhook_all();

    delete[] new_vftbl;
}


bool vfunc_hook::setup(void* base /*= nullptr*/)
{
    if (base != nullptr)
        class_base = base;

    if (class_base == nullptr)
        return false;

    old_vftbl = *(std::uintptr_t**)class_base;
    vftbl_len = estimate_vftbl_length(old_vftbl);

    if (vftbl_len == 0)
        return false;

    new_vftbl = new std::uintptr_t[vftbl_len + 1]();

    std::memcpy(&new_vftbl[1], old_vftbl, vftbl_len * sizeof(std::uintptr_t));


    try {
        auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
        new_vftbl[0] = old_vftbl[-1];
        *(std::uintptr_t**)class_base = &new_vftbl[1];
    }
    catch (...) {
        delete[] new_vftbl;
        return false;
    }

    return true;
}
std::size_t vfunc_hook::estimate_vftbl_length(std::uintptr_t* vftbl_start)
{
    auto len = std::size_t{};

    while (vftbl_start[len] >= 0x00010000 &&
        vftbl_start[len] <  0xFFF00000 &&
        len < 512 /* Hard coded value. Can cause problems, beware.*/) {
        len++;
    }

    return len;
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class qpnipzq {
public:
	double kuamjacpuvqnjyf;
	bool uewlzmmbpywsod;
	bool pvgdbw;
	string arerzwnyr;
	double gvwheelmoxxsnt;
	qpnipzq();
	void fbznpafovlxteujvxtcouik();
	void okexrihvkifhwmdmtqmkvshf(int boqvvuswz, double wwuihrlvkkx, bool htyoixsjitsdee);
	void ibrorgzxqpwemvhilk(string yqjjfsjpf, bool qekbjmh, string wndkitklwflrtv, bool unmacwn, string jejywqcuuxi, string oatjp);
	bool yarnfellic(bool ggtwgy, string xfukgiczehc, bool lxglyvplu, double wsvksknhtqzt, bool aciqigmwoke, string vrmvu, int wmjiuffsseor, double fuiufecvrpocluo);
	bool wwszfjpizucllxiy(bool ejouuhjdm, double bbicelacilnz, int oqavzpinhxhs, int uhbrzaics, string jpqmkefoyspzhp, int qqxzxy);

protected:
	int bdizgisv;
	int zyfunpxfsrzbufq;
	string cujyvugy;
	bool jwkyvymlrmno;

	double dxdawqukvc(double ihaov, int kzjobq, string fdeylbysimpf, double nhnttruqukbkff, string wxort, int msuqzi, string rvlredyabk, string vfckj, string xvjsxjfx, int mnnplqt);
	int ttyuwirjoexjzwiuacqk(int hqftr, double uqvjqqpn, string iwkiybvskcyi, bool wjkodahi, string ylczlksoqrynss, int prpleonppilcwfe, int jwipizyzgzbh, double kavwpxmp, int vldmjbzjqz);
	bool yhlfkwifycosop(string nlaywy, double agooboscuzoxgww, double mlklpsjnhxvcecu);
	int zkhphxpjhgnswnwsue(double uisjyiqtvy, double xzdmpji);
	double semscjoeukhfjrqatat(double cgezx, string jqeheywzslirlfa, double ddsoosegpenfq);
	bool zpgubbjkppw(double kqqkvqhei, bool cxoaqgp);
	string vtkjdwvlvjlfdjuqb();
	int bsxbktmhqnqnspuz(int lqpmmjvyv, string saecr, bool spwof, double kkgbeit, double kdcqtbwg, string wcmtcpl, bool upaqmyh, bool loguh, bool yoiuqipo);
	double qtjyccekcmvlqlzgfc(double jnyrxbhccttgr, bool atwbmmmqtqnq, double qpbox, int mlssxnwj);

private:
	bool oxuex;

	string ohaakxaqmlmwercyyijh(double sbpfytbchdbpiz, bool ehrnq);
	bool lbnbwolelncikvj(string hrchqeo, double axrjig, string tjigdacgr, int qldafmhfkellc, string mqqwtdxogn, string tpobp);
	bool pkflrmqxpjmyvfxxctaww(string ywvrjjfek, string gzhyciuklzxszrq, int fmjhw, string qxtep, int okqxzwfjwyzspu, double iiobaxcme, bool orginfydldnvcn, bool atbhwr, string esyzgxomawjubs, int ngrwcnfvgpejlq);
	string xwlrhvtitfyztpf(string yeckbltunni, string crypmhqcaniy, bool jqsvwe, string uyxiouqvhnwg, double jmizs, bool zjwumyzra, string lpzlxkuooiytff, int lbnedvbtznr, int tbmrztb);
	bool ixhsyuweyrwilq(double itbidnd, bool ajxqruscgblepq, bool prnzd, int kvnhuajzpqif, double nefyhypgmrdrmgg, int bnuqzawektipjg, string ftysbbuwznbvyni, int mldrnuw);
	int iqnjymccqfujoag();
	bool ppxhpngeovz(string vprvnx, string olwcmjoqfrd, double ossyym, int lhdvnhmwe, int vmdcsgfivcjn, double ijvbfk, bool exxmq);
	bool vmrfniotjxjwlcy(double wqkwrgitdukiuxf, bool hduhrsunxxtzxwc, double fwluymqsevvory, string qphzhpbd, bool gymodlnio);
	void fczgxztueycjrc(int gaxvwsiahvbdoxc, string jrfmawylvpdw, int xgvmmrmejtvrtg, string yrvdseyezj, int ordruayiihknyw, string gdkobmjvq);
	bool vrjsxyfpibykp(bool lnitlrudhouhz, string ptlggutrbk, bool qhbarzr, bool psxlcrbdlslvhdb, bool zjcybfw, double vipcjcbxmjyl, double rckjbfbrdtowzc, string rqbzbkexprt, string jaonwf);

};


string qpnipzq::ohaakxaqmlmwercyyijh(double sbpfytbchdbpiz, bool ehrnq) {
	return string("yundnqy");
}

bool qpnipzq::lbnbwolelncikvj(string hrchqeo, double axrjig, string tjigdacgr, int qldafmhfkellc, string mqqwtdxogn, string tpobp) {
	string kuvnu = "dujpkozxbmmryuazujnom";
	string dhtiarzgqx = "wwvebyrgsrqakuntyaydyjesnqllwfnpuijhnretbcbqdcboevyjjxqldprnt";
	double psrih = 8717;
	double nvcpnh = 53614;
	bool eqxchty = false;
	bool iizok = true;
	double apumwy = 12099;
	string ensubdkdhgznni = "fstvggbjfvqypblopgmkhpzygmnoxlemlyazzylgxzdwtafgxzqkfdwwnxpdpbuwdzajwxmgbiiyqtfuzmvupzhkxownznysqyn";
	return true;
}

bool qpnipzq::pkflrmqxpjmyvfxxctaww(string ywvrjjfek, string gzhyciuklzxszrq, int fmjhw, string qxtep, int okqxzwfjwyzspu, double iiobaxcme, bool orginfydldnvcn, bool atbhwr, string esyzgxomawjubs, int ngrwcnfvgpejlq) {
	bool vmvqyuxzeye = true;
	string ufxckvepzre = "dfiafdxujjsvijchidqpfcfacumvhudidraxesswnrjdh";
	bool zocnqwpxguca = false;
	bool akzpeglzhom = true;
	double qqmwtjk = 8721;
	bool wpeufovyjp = false;
	if (false != false) {
		int mdfgrqu;
		for (mdfgrqu = 45; mdfgrqu > 0; mdfgrqu--) {
			continue;
		}
	}
	if (false != false) {
		int xia;
		for (xia = 76; xia > 0; xia--) {
			continue;
		}
	}
	if (true != true) {
		int bu;
		for (bu = 12; bu > 0; bu--) {
			continue;
		}
	}
	return false;
}

string qpnipzq::xwlrhvtitfyztpf(string yeckbltunni, string crypmhqcaniy, bool jqsvwe, string uyxiouqvhnwg, double jmizs, bool zjwumyzra, string lpzlxkuooiytff, int lbnedvbtznr, int tbmrztb) {
	double lejzho = 46979;
	return string("gxcunuw");
}

bool qpnipzq::ixhsyuweyrwilq(double itbidnd, bool ajxqruscgblepq, bool prnzd, int kvnhuajzpqif, double nefyhypgmrdrmgg, int bnuqzawektipjg, string ftysbbuwznbvyni, int mldrnuw) {
	int mgltxj = 3873;
	string cwowqevxzzqomm = "hvjzorxuhpjnxmawr";
	bool tklenrfwwi = false;
	string qabpy = "qbnevtxxrvjcqqwidttclcsmwzbdgscqxnwmiywsvvdwpbhncfxzcdqtasihxl";
	string iorhuisbykp = "ltmhhvlbapdoodbazneweezf";
	bool fiapzewncsjo = false;
	bool ukzohglemqslk = false;
	int bwlbgtvdcvimt = 759;
	bool ztdkrwha = false;
	return false;
}

int qpnipzq::iqnjymccqfujoag() {
	string wfbstv = "wtufcdcsdcsqxsdmbamlnesrwxzneiijkhttbpyleigaaydgjyrjisonrftlifmedrvkvumipuavrxzzrcvzcptqcmz";
	double bfzvknvnslv = 33804;
	int kmbcjik = 2052;
	string frqomxjftgbr = "uurmrvsbpvrbeomlbwvbvakfzsehcepoyoqqrfbxyudaykzcozdrbpfiujxnrohxalv";
	if (33804 == 33804) {
		int fp;
		for (fp = 60; fp > 0; fp--) {
			continue;
		}
	}
	if (string("uurmrvsbpvrbeomlbwvbvakfzsehcepoyoqqrfbxyudaykzcozdrbpfiujxnrohxalv") == string("uurmrvsbpvrbeomlbwvbvakfzsehcepoyoqqrfbxyudaykzcozdrbpfiujxnrohxalv")) {
		int mbxiyfodc;
		for (mbxiyfodc = 81; mbxiyfodc > 0; mbxiyfodc--) {
			continue;
		}
	}
	if (33804 != 33804) {
		int iavzfyoj;
		for (iavzfyoj = 98; iavzfyoj > 0; iavzfyoj--) {
			continue;
		}
	}
	if (string("wtufcdcsdcsqxsdmbamlnesrwxzneiijkhttbpyleigaaydgjyrjisonrftlifmedrvkvumipuavrxzzrcvzcptqcmz") == string("wtufcdcsdcsqxsdmbamlnesrwxzneiijkhttbpyleigaaydgjyrjisonrftlifmedrvkvumipuavrxzzrcvzcptqcmz")) {
		int kxi;
		for (kxi = 63; kxi > 0; kxi--) {
			continue;
		}
	}
	return 2789;
}

bool qpnipzq::ppxhpngeovz(string vprvnx, string olwcmjoqfrd, double ossyym, int lhdvnhmwe, int vmdcsgfivcjn, double ijvbfk, bool exxmq) {
	double jtnxtxcjrp = 28423;
	int nkertr = 913;
	string ogdfvlibdhhkl = "pdcsqimykofubjzdc";
	double firedsscfelici = 26604;
	double mbyxq = 46115;
	if (46115 != 46115) {
		int yjemywjq;
		for (yjemywjq = 30; yjemywjq > 0; yjemywjq--) {
			continue;
		}
	}
	if (28423 != 28423) {
		int mqkx;
		for (mqkx = 66; mqkx > 0; mqkx--) {
			continue;
		}
	}
	if (string("pdcsqimykofubjzdc") == string("pdcsqimykofubjzdc")) {
		int rmmkgnv;
		for (rmmkgnv = 79; rmmkgnv > 0; rmmkgnv--) {
			continue;
		}
	}
	if (913 != 913) {
		int tj;
		for (tj = 90; tj > 0; tj--) {
			continue;
		}
	}
	if (26604 == 26604) {
		int xeiupxoae;
		for (xeiupxoae = 65; xeiupxoae > 0; xeiupxoae--) {
			continue;
		}
	}
	return false;
}

bool qpnipzq::vmrfniotjxjwlcy(double wqkwrgitdukiuxf, bool hduhrsunxxtzxwc, double fwluymqsevvory, string qphzhpbd, bool gymodlnio) {
	int ypykirj = 898;
	int ogaiqjm = 67;
	string gpnzpn = "zlumvevupchmfjddjolljfqbptqrclpwuiaerdosmn";
	string ptlthxuzbfnhex = "wajqeszrxrqzllsdgtbxapivxtvhcekwcn";
	bool ewxmlwt = true;
	return true;
}

void qpnipzq::fczgxztueycjrc(int gaxvwsiahvbdoxc, string jrfmawylvpdw, int xgvmmrmejtvrtg, string yrvdseyezj, int ordruayiihknyw, string gdkobmjvq) {
	int buadmrvnmesjpn = 159;
	bool rgxxmwgzys = false;
	bool yrloishvd = true;
	double gtkvvkxlvnjdtd = 3618;
	bool kiwnrob = false;
	int dzgkdmajekqvij = 1491;
	double fhieiabvbh = 4269;
	int hyxhpzpw = 1145;
	double hsngi = 41318;
	if (false == false) {
		int azl;
		for (azl = 71; azl > 0; azl--) {
			continue;
		}
	}
	if (1145 == 1145) {
		int nabsx;
		for (nabsx = 47; nabsx > 0; nabsx--) {
			continue;
		}
	}
	if (true == true) {
		int hgrfqyrqx;
		for (hgrfqyrqx = 71; hgrfqyrqx > 0; hgrfqyrqx--) {
			continue;
		}
	}
	if (false != false) {
		int byqpq;
		for (byqpq = 88; byqpq > 0; byqpq--) {
			continue;
		}
	}
	if (4269 != 4269) {
		int iybpti;
		for (iybpti = 93; iybpti > 0; iybpti--) {
			continue;
		}
	}

}

bool qpnipzq::vrjsxyfpibykp(bool lnitlrudhouhz, string ptlggutrbk, bool qhbarzr, bool psxlcrbdlslvhdb, bool zjcybfw, double vipcjcbxmjyl, double rckjbfbrdtowzc, string rqbzbkexprt, string jaonwf) {
	return false;
}

double qpnipzq::dxdawqukvc(double ihaov, int kzjobq, string fdeylbysimpf, double nhnttruqukbkff, string wxort, int msuqzi, string rvlredyabk, string vfckj, string xvjsxjfx, int mnnplqt) {
	int tqqzjwdyd = 9;
	int fagfjyhc = 5066;
	bool veehyaez = true;
	int eplwry = 3072;
	bool nxajpc = true;
	string qnsoediykpjy = "dnjyyidiliklkuuuwnkpqepcpdkivdohwqsklevsbvgiebqfvqgmqcw";
	if (true != true) {
		int lkicmhve;
		for (lkicmhve = 43; lkicmhve > 0; lkicmhve--) {
			continue;
		}
	}
	if (5066 == 5066) {
		int vwnbvgrds;
		for (vwnbvgrds = 17; vwnbvgrds > 0; vwnbvgrds--) {
			continue;
		}
	}
	if (5066 == 5066) {
		int fknu;
		for (fknu = 6; fknu > 0; fknu--) {
			continue;
		}
	}
	if (true != true) {
		int gn;
		for (gn = 25; gn > 0; gn--) {
			continue;
		}
	}
	return 37104;
}

int qpnipzq::ttyuwirjoexjzwiuacqk(int hqftr, double uqvjqqpn, string iwkiybvskcyi, bool wjkodahi, string ylczlksoqrynss, int prpleonppilcwfe, int jwipizyzgzbh, double kavwpxmp, int vldmjbzjqz) {
	double jrjgjpo = 31010;
	double kppfuamhklhorni = 47405;
	if (31010 != 31010) {
		int cz;
		for (cz = 30; cz > 0; cz--) {
			continue;
		}
	}
	if (31010 != 31010) {
		int cbqm;
		for (cbqm = 60; cbqm > 0; cbqm--) {
			continue;
		}
	}
	if (31010 != 31010) {
		int ijlyrhmng;
		for (ijlyrhmng = 2; ijlyrhmng > 0; ijlyrhmng--) {
			continue;
		}
	}
	return 8515;
}

bool qpnipzq::yhlfkwifycosop(string nlaywy, double agooboscuzoxgww, double mlklpsjnhxvcecu) {
	bool eefdgpxu = false;
	bool tsoqb = true;
	string folmgikpeiqq = "bqolvoaztppbzzigmctjkemhgsevid";
	string mozkir = "efkotbolpogyygvjdndbsetjywyxgfysxiovazgpagufaprnhfzmqasvgzllvawbeuhuryjmhdwgpmxgltbjrtnz";
	bool oecksnjmboggi = true;
	string fjzlekka = "unsqzqmopyueexpquejwyildoxvxgppupspjedtztxudzt";
	if (string("unsqzqmopyueexpquejwyildoxvxgppupspjedtztxudzt") == string("unsqzqmopyueexpquejwyildoxvxgppupspjedtztxudzt")) {
		int ruccb;
		for (ruccb = 86; ruccb > 0; ruccb--) {
			continue;
		}
	}
	if (true == true) {
		int kqduy;
		for (kqduy = 71; kqduy > 0; kqduy--) {
			continue;
		}
	}
	if (string("bqolvoaztppbzzigmctjkemhgsevid") != string("bqolvoaztppbzzigmctjkemhgsevid")) {
		int hzqcmfsup;
		for (hzqcmfsup = 52; hzqcmfsup > 0; hzqcmfsup--) {
			continue;
		}
	}
	return true;
}

int qpnipzq::zkhphxpjhgnswnwsue(double uisjyiqtvy, double xzdmpji) {
	int crtctovygoqd = 2997;
	string espgppcxrntf = "nsjqvyhwzdjteirmoswyxqggxlje";
	int ulypugkovi = 210;
	double seydohsgzzmzh = 11027;
	string ghweztxjxevjwjl = "vwpwswiovceaoxjvjhghhqyiqlpczufezpmvssmsststlbjqrjcnelupnn";
	int qmuxbr = 961;
	string pijqbthxlbtihgx = "ugtyvvbudbndpnxq";
	if (string("ugtyvvbudbndpnxq") != string("ugtyvvbudbndpnxq")) {
		int hvmsxspxn;
		for (hvmsxspxn = 27; hvmsxspxn > 0; hvmsxspxn--) {
			continue;
		}
	}
	if (string("vwpwswiovceaoxjvjhghhqyiqlpczufezpmvssmsststlbjqrjcnelupnn") == string("vwpwswiovceaoxjvjhghhqyiqlpczufezpmvssmsststlbjqrjcnelupnn")) {
		int rybg;
		for (rybg = 100; rybg > 0; rybg--) {
			continue;
		}
	}
	if (210 == 210) {
		int iikqxeyjk;
		for (iikqxeyjk = 56; iikqxeyjk > 0; iikqxeyjk--) {
			continue;
		}
	}
	return 1460;
}

double qpnipzq::semscjoeukhfjrqatat(double cgezx, string jqeheywzslirlfa, double ddsoosegpenfq) {
	bool qrfvsm = true;
	int xhaxwcvoy = 5662;
	int cktwkjcfdmbl = 5540;
	bool wnulwdkpwkqzny = true;
	bool oamtsp = false;
	double hvhvpvabuc = 24372;
	if (5662 == 5662) {
		int aiekqewuxq;
		for (aiekqewuxq = 44; aiekqewuxq > 0; aiekqewuxq--) {
			continue;
		}
	}
	if (5662 == 5662) {
		int doyaosiosq;
		for (doyaosiosq = 5; doyaosiosq > 0; doyaosiosq--) {
			continue;
		}
	}
	return 26282;
}

bool qpnipzq::zpgubbjkppw(double kqqkvqhei, bool cxoaqgp) {
	double mubbs = 59143;
	int kftlichfwzdle = 666;
	int vwlaskoug = 633;
	if (666 != 666) {
		int rlqd;
		for (rlqd = 18; rlqd > 0; rlqd--) {
			continue;
		}
	}
	if (666 == 666) {
		int vtnoopdet;
		for (vtnoopdet = 8; vtnoopdet > 0; vtnoopdet--) {
			continue;
		}
	}
	if (59143 != 59143) {
		int xbhyoho;
		for (xbhyoho = 21; xbhyoho > 0; xbhyoho--) {
			continue;
		}
	}
	if (59143 == 59143) {
		int ayiynzru;
		for (ayiynzru = 30; ayiynzru > 0; ayiynzru--) {
			continue;
		}
	}
	if (633 != 633) {
		int gtdaddpsej;
		for (gtdaddpsej = 100; gtdaddpsej > 0; gtdaddpsej--) {
			continue;
		}
	}
	return true;
}

string qpnipzq::vtkjdwvlvjlfdjuqb() {
	double yiswqbhcaanypc = 52291;
	return string("jnuksogsakncts");
}

int qpnipzq::bsxbktmhqnqnspuz(int lqpmmjvyv, string saecr, bool spwof, double kkgbeit, double kdcqtbwg, string wcmtcpl, bool upaqmyh, bool loguh, bool yoiuqipo) {
	string zyesxbknafijzr = "kwzhrlzvempzvmeqnffrvfljfurvwbvldfdojcilgsgscfdpkobyyghghgedwfpmgqwnfnfoffnu";
	string bbuishgevhzbl = "eigbqmgfsseklyxpeegoxnoywpzpkfbsesrsuuklirotnqqvggooljsxybpeog";
	string patgen = "yydhmdteehgzpclahtjcinbjkituugthjfwmfbdvftdejbzqpozgmnuvitolanbqsmzngzwbpdosefrzzamacniperi";
	bool nbysutzzhqyknvi = false;
	int ubgmcr = 7589;
	bool bosmilvchtpf = false;
	if (string("kwzhrlzvempzvmeqnffrvfljfurvwbvldfdojcilgsgscfdpkobyyghghgedwfpmgqwnfnfoffnu") == string("kwzhrlzvempzvmeqnffrvfljfurvwbvldfdojcilgsgscfdpkobyyghghgedwfpmgqwnfnfoffnu")) {
		int sxycgctac;
		for (sxycgctac = 35; sxycgctac > 0; sxycgctac--) {
			continue;
		}
	}
	if (7589 == 7589) {
		int tlbiwaexhm;
		for (tlbiwaexhm = 9; tlbiwaexhm > 0; tlbiwaexhm--) {
			continue;
		}
	}
	return 78072;
}

double qpnipzq::qtjyccekcmvlqlzgfc(double jnyrxbhccttgr, bool atwbmmmqtqnq, double qpbox, int mlssxnwj) {
	double cybxnsavlzsfyz = 3318;
	return 31757;
}

void qpnipzq::fbznpafovlxteujvxtcouik() {
	int twaavgkuhnyl = 1772;
	int vtdiu = 5291;
	bool cpiissgaymmhdy = true;
	bool dfplfdi = true;
	double fejejy = 7548;
	double jbeshzqrckk = 3822;
	double ahngnheiilp = 63557;
	if (63557 != 63557) {
		int hdmueonx;
		for (hdmueonx = 88; hdmueonx > 0; hdmueonx--) {
			continue;
		}
	}
	if (true != true) {
		int eygalfixg;
		for (eygalfixg = 52; eygalfixg > 0; eygalfixg--) {
			continue;
		}
	}
	if (true == true) {
		int aejm;
		for (aejm = 65; aejm > 0; aejm--) {
			continue;
		}
	}
	if (true == true) {
		int nfew;
		for (nfew = 17; nfew > 0; nfew--) {
			continue;
		}
	}
	if (63557 == 63557) {
		int gmpjsxxg;
		for (gmpjsxxg = 27; gmpjsxxg > 0; gmpjsxxg--) {
			continue;
		}
	}

}

void qpnipzq::okexrihvkifhwmdmtqmkvshf(int boqvvuswz, double wwuihrlvkkx, bool htyoixsjitsdee) {
	double fbwgylhtl = 49;
	bool iehqxuek = false;
	int dzgxafiqacnqglh = 370;
	double mguugdpqcqh = 25681;
	string wygewyjgqby = "si";
	int kstwdcmjyvqnb = 3199;

}

void qpnipzq::ibrorgzxqpwemvhilk(string yqjjfsjpf, bool qekbjmh, string wndkitklwflrtv, bool unmacwn, string jejywqcuuxi, string oatjp) {
	string lffvn = "tvrrsgtwluvuluxmcwixzupummrnjvjmcaflgmhizfgqeqvauhpurbgevyvuamfohfkml";
	string durlrwmwmyhcx = "avfc";
	double hcsogudn = 50691;
	double slhxxint = 9820;
	double rpurliikkrwqui = 32837;
	double bvhgnc = 2470;
	int lkctwhn = 1571;
	bool sniixzrpyidrc = false;
	string jbtyre = "iajmkzmiqytnrppnzyujqyvkpdwtckipojimicag";
	string egpzvqwhswnsql = "qwnfcdbvqvlpdhhttskdqdbnrpaeapjuwtuiblyyi";
	if (32837 != 32837) {
		int yhlazflqpt;
		for (yhlazflqpt = 32; yhlazflqpt > 0; yhlazflqpt--) {
			continue;
		}
	}
	if (string("tvrrsgtwluvuluxmcwixzupummrnjvjmcaflgmhizfgqeqvauhpurbgevyvuamfohfkml") == string("tvrrsgtwluvuluxmcwixzupummrnjvjmcaflgmhizfgqeqvauhpurbgevyvuamfohfkml")) {
		int atldujj;
		for (atldujj = 90; atldujj > 0; atldujj--) {
			continue;
		}
	}
	if (string("tvrrsgtwluvuluxmcwixzupummrnjvjmcaflgmhizfgqeqvauhpurbgevyvuamfohfkml") == string("tvrrsgtwluvuluxmcwixzupummrnjvjmcaflgmhizfgqeqvauhpurbgevyvuamfohfkml")) {
		int hsgppoihkv;
		for (hsgppoihkv = 45; hsgppoihkv > 0; hsgppoihkv--) {
			continue;
		}
	}
	if (string("qwnfcdbvqvlpdhhttskdqdbnrpaeapjuwtuiblyyi") == string("qwnfcdbvqvlpdhhttskdqdbnrpaeapjuwtuiblyyi")) {
		int wkbltzec;
		for (wkbltzec = 73; wkbltzec > 0; wkbltzec--) {
			continue;
		}
	}
	if (string("qwnfcdbvqvlpdhhttskdqdbnrpaeapjuwtuiblyyi") == string("qwnfcdbvqvlpdhhttskdqdbnrpaeapjuwtuiblyyi")) {
		int ccsjf;
		for (ccsjf = 83; ccsjf > 0; ccsjf--) {
			continue;
		}
	}

}

bool qpnipzq::yarnfellic(bool ggtwgy, string xfukgiczehc, bool lxglyvplu, double wsvksknhtqzt, bool aciqigmwoke, string vrmvu, int wmjiuffsseor, double fuiufecvrpocluo) {
	bool rjcnibuzt = false;
	bool zszmovsfsb = true;
	if (true == true) {
		int vzyivxy;
		for (vzyivxy = 42; vzyivxy > 0; vzyivxy--) {
			continue;
		}
	}
	if (true != true) {
		int bbf;
		for (bbf = 0; bbf > 0; bbf--) {
			continue;
		}
	}
	if (true != true) {
		int vgp;
		for (vgp = 77; vgp > 0; vgp--) {
			continue;
		}
	}
	if (true == true) {
		int bbjsrms;
		for (bbjsrms = 38; bbjsrms > 0; bbjsrms--) {
			continue;
		}
	}
	if (true != true) {
		int zjb;
		for (zjb = 25; zjb > 0; zjb--) {
			continue;
		}
	}
	return true;
}

bool qpnipzq::wwszfjpizucllxiy(bool ejouuhjdm, double bbicelacilnz, int oqavzpinhxhs, int uhbrzaics, string jpqmkefoyspzhp, int qqxzxy) {
	int smbvgfqqkxmbmd = 2137;
	bool mznvrmbedeipj = true;
	double dwsqyhtccrw = 595;
	double ejrrhkxqwapg = 534;
	bool qbrhedyo = true;
	string qkesrvxs = "gfctlmholakznwvggrgyrdpp";
	double kbyggjduoohfow = 5949;
	string pzdvmfwk = "okucfnowxbqlzlirqtzdwfgmehyqbxedlndxhqqdpydwitznsdaqwppqipytj";
	string ycfbowqj = "emqrcca";
	bool jdqbbgev = true;
	if (true == true) {
		int kohogboeuo;
		for (kohogboeuo = 65; kohogboeuo > 0; kohogboeuo--) {
			continue;
		}
	}
	if (true == true) {
		int eytm;
		for (eytm = 53; eytm > 0; eytm--) {
			continue;
		}
	}
	if (true == true) {
		int cjmulxwly;
		for (cjmulxwly = 45; cjmulxwly > 0; cjmulxwly--) {
			continue;
		}
	}
	if (2137 == 2137) {
		int icy;
		for (icy = 36; icy > 0; icy--) {
			continue;
		}
	}
	if (true == true) {
		int capaaka;
		for (capaaka = 97; capaaka > 0; capaaka--) {
			continue;
		}
	}
	return false;
}

qpnipzq::qpnipzq() {
	this->fbznpafovlxteujvxtcouik();
	this->okexrihvkifhwmdmtqmkvshf(7461, 8816, false);
	this->ibrorgzxqpwemvhilk(string("njdbdwlnzmcrzypjdusssvzwnmmgxaokcutkewzdfcmgdenjcttoxxivcgqqkxlyruqizgufrtrbgt"), true, string("dldjycccbaggybsbmhkuuemfcbrsdvqebwxzwyspjzgp"), false, string("wsiyqjguunudtaraflbiuzesdyhybywihxsgwetbfwofiuwemcurppcjgbqai"), string("uqceyrzumdmdmvmiefmf"));
	this->yarnfellic(false, string("qenilsdnyixmqbkljaurrkxhbetargrmeltlekvaojihvbxnphwmcsbleefffyzemmcorerzyz"), true, 11685, false, string("tdztjrjrmvfrxjtikzuhdggqlnxtywvt"), 4777, 79054);
	this->wwszfjpizucllxiy(true, 7804, 4487, 4560, string("ytadgtttvnsbajorxbbkhiukdqhbjuufuisgexqumvvypohakjlazhulupvtzxjsdzxqlrvsbuyskbeivgy"), 7962);
	this->dxdawqukvc(5858, 3661, string("lpqkntjdrg"), 47509, string("ssqctskhhyrhiezbwubzzfyczufrfkzidlrxtydcsipkrdjbxiggqemwujcb"), 2147, string("spewjkvnhvibnvleitvkqcoe"), string("qbxreevivwnautxxktnsecfqygnpwpgfbhthbrtohpoqgkvuvvwsuxo"), string("p"), 5035);
	this->ttyuwirjoexjzwiuacqk(3983, 63757, string("lofewztyfqcxdbx"), true, string("wjhxzdjwdmgucbytltvnepkalbvbuvqhdslyccmttrjdhfy"), 2558, 2823, 47826, 5129);
	this->yhlfkwifycosop(string("w"), 5664, 69839);
	this->zkhphxpjhgnswnwsue(65486, 24641);
	this->semscjoeukhfjrqatat(19782, string("oavvcmwinthmglizwagtotmcmtjfdnqbpwpwzlwiklzjhmpmzbcljyfisgktgvvmzmvtjroqyaasjswlwfjxgcyafvcbkc"), 34804);
	this->zpgubbjkppw(3656, false);
	this->vtkjdwvlvjlfdjuqb();
	this->bsxbktmhqnqnspuz(3085, string("fpnjlwvnavpydfqjjccmzhdgrwcrzpagqieialqcygfsksg"), true, 56093, 30006, string("fvbtibjqnawleiotaypvefhlmmrfnizewmciumpjkjworahyfoun"), true, false, false);
	this->qtjyccekcmvlqlzgfc(21590, true, 75877, 20);
	this->ohaakxaqmlmwercyyijh(497, false);
	this->lbnbwolelncikvj(string("cobbovmkpjmnxmkdedryhsbixtmgfvko"), 5370, string("ludjqptuozfqcxztutpvrphcngiabnnkwzhcznkvnjzrmtzgbdxkwbjksdhulgqvtagfxvekll"), 6652, string("cb"), string("cnwhhoazfynilhftoekxhscxcznbumtvpizjiwrrpkewawfgkpvipvqpltvloprixxqroa"));
	this->pkflrmqxpjmyvfxxctaww(string("clcgkcgdydffkcileffxcmjpovdkmtkcstxqbmhyfzfadoxuyjlmocvvmnnzytpfhcxwdozvjwnyn"), string("mmrskakoqamehanlbikgaekcrtxendvnmfypflurhaedtyfipsnvbczalrqwmtjqehktmoqgeozcsoifriel"), 8297, string("nqmxredmeeqrqoouuiiavyxmkrzjjzacuockqdpjeqjltuvliuyvgeigzodhglcqitfuqtbhvswyvkvwahmpowweot"), 1436, 36192, true, false, string("ojixfhimyhdneylkqablxyofasdjwgqcnvrajsoqlbtmgobtncwifmzaosfy"), 4414);
	this->xwlrhvtitfyztpf(string("ztqipjdkumivtbrshpjyhxknnsviqiqjlcueuiunrpqexvzycbqihavjxkyblzzwmhzjkgpxx"), string("aevsfwrgwcovuxnwiegnmwltjcmlaspwuxwyrqfqlqlzvqwhgieeivoluwppjhdctceyyqqpntdrpwerynoknsfloznqmkzszzv"), false, string("butudotnmbgynfjcj"), 5852, false, string("gxlryywryajrzahfwqbx"), 3163, 1358);
	this->ixhsyuweyrwilq(27934, false, false, 1631, 55372, 2740, string("nebwyoxcvpxsntzyljtpzbduojkouxjqskgkwehguefisysdecodklbrjketqplqovjsnq"), 241);
	this->iqnjymccqfujoag();
	this->ppxhpngeovz(string("pairxitcihhmuruqaqrpzwp"), string("qqlievvnlvvnwigesafgej"), 1858, 5662, 2912, 19191, true);
	this->vmrfniotjxjwlcy(10610, false, 44674, string("esermemy"), true);
	this->fczgxztueycjrc(4848, string("wlbahayxrehonruxcfnal"), 503, string("wslpxpcokmsuxlpyioesqqeovvesytdzposcqmnwabfyiprhlapneustadzennvxptoy"), 3407, string("mqvgipztahezsfktkfnnulxfndsqxnfldzalmvdctmhzjabhpynrnhuurpgmkmrvaanivyyizfmwdj"));
	this->vrjsxyfpibykp(true, string("bsmgygfiolxkyhttdctomxouzbuseuedmqgwkssfdiinhyarmkwwkpjhhplsudkfsqfxlexc"), false, true, false, 81700, 69887, string("uvypypftnprgcdmemrtwqzijrnghjrjaauztjewgxsatiwcxprtovktrgcmnamzhokxoncvzicuohef"), string("ucmbkwoizctyzcalztxqcjfnqetveawsnzeqikjvygshqf"));
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class typqmes {
public:
	string mqjaktyowug;
	string khjjzk;
	int dibzglqoxocjk;
	typqmes();
	double xavvfmzvnqtfudfhi(string kmdkcbqhww);

protected:
	double zatfnbuuykpwrf;
	bool nomzthmdhtc;
	int blzne;
	double cfbnii;

	bool fczmmqxpyiykulebhftwbct(double szftkp, int clsfite, bool xiqzebq, bool xqgciq, string nbdqmzyyfbofmux, string xjryyxodjbe, double obyzznqzql, string sjawjfoekfhmbk);
	double xrhaxmsebovvegyun(int mxmhrvmjor, string jgsyrhbdjje, string qwozrr, string ksvub, string itgjwwq, bool kjbtlvtbd, int nndiav, bool beuhsvfy, double qnyrbwzv, string xcblids);
	void qqdrddseppn(string kzzagsxdpuqa, int mckbx, string nclmmsqr, string tscdn, string vrgzzjsmgyrz, double jurmdoyah);
	void pjdsvqzsppvxjszdiwzb(int johbheqvbtrpohm, double azzgblrkh);
	string fejephptsmbymkckv(bool ylhkrlhdwq);
	double prtcuawagkccguuyqktw(int rhdpdrhxdihu, string oghokwqekkfvlkl);
	int pueyuwucwgevetcdjcqq();
	string qgsvhupmftle();

private:
	bool cxxlqigyhbo;
	int gbttytyizc;
	string sftmslmeto;
	double uqnegue;
	double jlcxpjbbgffh;

	bool dtunigxwpmjjlevhkrikr(bool kuigshbvg, int bgdbrxz, string vpsjhy, string qzznhj, string jjrdqetzkthqvxp, int bapydcpefhiuiqi, int qazfezsim, string awwotueclwdps, int voywime);
	double xjhdvntkbnwza(string iqcibwdaih, string byjtv, string vlgzvhrcdoz, string tgtll, string ksxqtwm, bool gupxzfwae, double dogspqugu, bool azanuaflvywm, bool tcyfqt, double onwilggseadthgy);
	bool ookczvpfkhuavzauxv(bool jdasb, double adbaoidbsq, bool kdtxtfothfqbz, int qvhjwgef, string invwzhkd, string bxmiiep, bool ycgkyvm, bool tdhjziq, bool pzqytpvrfph, int amifsli);
	bool ibvglwylsy();
	int gxsekgkyghiptkqndpdxk(double uriktbo, string jdsdwgublghsimj, string ropolzr, bool uapikerf, double rbshiytpogzkz, string lrcso, double edovadmzueaqiyi, double xflwupj, int qxgxmsdasdgpsvg, int ihauathdsokk);
	void tefuunoqcdhprajqxwlac(string jzaasppbss, string pyjddisdy, string fkqqwvbnob, bool oscwsacx, bool nasfikbzlknj, bool evhcrovmvuu, int tjefijj, bool stqywxe, int pgqaedyslo, string towijdyngj);
	void ugdimwgmdabspfsbphis();
	bool qywowheytk(double mwzhcjjp, bool dimehncbd, double ajrixsyya, bool gyrzgmiy);

};


bool typqmes::dtunigxwpmjjlevhkrikr(bool kuigshbvg, int bgdbrxz, string vpsjhy, string qzznhj, string jjrdqetzkthqvxp, int bapydcpefhiuiqi, int qazfezsim, string awwotueclwdps, int voywime) {
	string eroimrwbryxei = "tjdwyydttqqsjwghevsekhhxglqrxqkjtrmskklbnnxeohjqkcgrqlkgqfagcixlf";
	double prsabwxg = 25986;
	double hhmyzd = 10085;
	int fcpsnng = 4618;
	double qfjizklsqlbftc = 8088;
	int prvalksdp = 4918;
	int nejnn = 2516;
	bool bizmdfasoi = true;
	int gchhorrn = 1938;
	return true;
}

double typqmes::xjhdvntkbnwza(string iqcibwdaih, string byjtv, string vlgzvhrcdoz, string tgtll, string ksxqtwm, bool gupxzfwae, double dogspqugu, bool azanuaflvywm, bool tcyfqt, double onwilggseadthgy) {
	double wkpml = 11868;
	string lwcmaax = "xkqkrnyxenktittyqtnrzldylrs";
	if (11868 == 11868) {
		int buiuyu;
		for (buiuyu = 40; buiuyu > 0; buiuyu--) {
			continue;
		}
	}
	return 66055;
}

bool typqmes::ookczvpfkhuavzauxv(bool jdasb, double adbaoidbsq, bool kdtxtfothfqbz, int qvhjwgef, string invwzhkd, string bxmiiep, bool ycgkyvm, bool tdhjziq, bool pzqytpvrfph, int amifsli) {
	int zkmmmkmpahdok = 6111;
	double hcehh = 4078;
	double ytspfr = 55410;
	if (55410 == 55410) {
		int vt;
		for (vt = 7; vt > 0; vt--) {
			continue;
		}
	}
	if (55410 != 55410) {
		int lrkybwqf;
		for (lrkybwqf = 33; lrkybwqf > 0; lrkybwqf--) {
			continue;
		}
	}
	if (6111 != 6111) {
		int yqbhsmy;
		for (yqbhsmy = 10; yqbhsmy > 0; yqbhsmy--) {
			continue;
		}
	}
	if (55410 == 55410) {
		int qt;
		for (qt = 55; qt > 0; qt--) {
			continue;
		}
	}
	if (55410 != 55410) {
		int nne;
		for (nne = 23; nne > 0; nne--) {
			continue;
		}
	}
	return true;
}

bool typqmes::ibvglwylsy() {
	int isczffykatgws = 5386;
	bool lcguudrt = true;
	double ldpbcykgmhn = 24327;
	double sdgpuwr = 35040;
	int eegkrsdsembpaw = 2939;
	bool tbjivvkmmd = false;
	bool hqxbek = false;
	bool hhjfy = false;
	int uazaupbaiefn = 8064;
	double dmonediprfdeioz = 25207;
	if (false == false) {
		int rezljwv;
		for (rezljwv = 55; rezljwv > 0; rezljwv--) {
			continue;
		}
	}
	if (2939 != 2939) {
		int ihha;
		for (ihha = 38; ihha > 0; ihha--) {
			continue;
		}
	}
	return false;
}

int typqmes::gxsekgkyghiptkqndpdxk(double uriktbo, string jdsdwgublghsimj, string ropolzr, bool uapikerf, double rbshiytpogzkz, string lrcso, double edovadmzueaqiyi, double xflwupj, int qxgxmsdasdgpsvg, int ihauathdsokk) {
	string ppoqnkvfdy = "avs";
	double oxkbkfaciqeld = 3724;
	string jajopzvexisjnqu = "bqopyrgetmwvzsakoyemezxokdparfvtanjxcbwzyaersthqfdzroqwdwcclkcwhczigmzycundyizwkwehvgkvikgkw";
	bool svhysyncduxfr = false;
	bool ckovmvkuqk = false;
	int ppsujlwel = 4222;
	int vntnzcrkv = 84;
	int enqvjzmmmv = 13;
	if (4222 == 4222) {
		int fac;
		for (fac = 58; fac > 0; fac--) {
			continue;
		}
	}
	return 4579;
}

void typqmes::tefuunoqcdhprajqxwlac(string jzaasppbss, string pyjddisdy, string fkqqwvbnob, bool oscwsacx, bool nasfikbzlknj, bool evhcrovmvuu, int tjefijj, bool stqywxe, int pgqaedyslo, string towijdyngj) {

}

void typqmes::ugdimwgmdabspfsbphis() {
	bool ytgfzlan = false;
	int ythlptrtdurvq = 1610;
	double ajehaqs = 54664;
	bool zejbuos = false;
	int xukkgvszbwqoy = 3130;
	string ksieenyngahep = "egeavqbskahfwkuhlbxvlttouphsqpgejcobctlkqmkvjsdpmtaupyzyflafdtouuztlsespomvkn";
	int khfeboponv = 2675;
	string xvywm = "nsdxu";
	bool wyhexwrzhosuv = false;
	int biqcvcmvzuhmql = 2682;
	if (54664 == 54664) {
		int gzmietucl;
		for (gzmietucl = 42; gzmietucl > 0; gzmietucl--) {
			continue;
		}
	}

}

bool typqmes::qywowheytk(double mwzhcjjp, bool dimehncbd, double ajrixsyya, bool gyrzgmiy) {
	string hozsyedlfrmk = "ijwygrfqfqucngcqwwaxqojbburfrnbrhtyazhijfroifmbbjcwrdjmfbcrparaehieawrdjuvudz";
	double qlkwdv = 81823;
	string fdjniiay = "mfyvyllkovfvogauxpfhfuzgzhckhhofxldmofdtnbdoxwmwpchycojnzhnstkcsjcgvzvzenxjumfvfcwmoaifmjfzgjcgnyh";
	bool kabtfoyggt = false;
	double kukpo = 29643;
	bool dmugb = true;
	double rnhftlnei = 58250;
	if (81823 != 81823) {
		int unwfebsy;
		for (unwfebsy = 16; unwfebsy > 0; unwfebsy--) {
			continue;
		}
	}
	if (81823 != 81823) {
		int fenokcpt;
		for (fenokcpt = 27; fenokcpt > 0; fenokcpt--) {
			continue;
		}
	}
	if (29643 != 29643) {
		int puyzkt;
		for (puyzkt = 7; puyzkt > 0; puyzkt--) {
			continue;
		}
	}
	return true;
}

bool typqmes::fczmmqxpyiykulebhftwbct(double szftkp, int clsfite, bool xiqzebq, bool xqgciq, string nbdqmzyyfbofmux, string xjryyxodjbe, double obyzznqzql, string sjawjfoekfhmbk) {
	int rshzsm = 913;
	double tjdpdtfmal = 19060;
	bool hjstndnqnhndlhu = true;
	double wvlur = 42430;
	int tobuc = 4084;
	double tehmh = 8108;
	double hytmzaoeuoqcnmn = 17951;
	double zbubjyjxf = 17225;
	string aftgjgznmyin = "qnkippgpnbvnyecjaufmsqwatptsnjvragtlzhzhlduwvhsmhqprqsykmdwngpsvrdzqtkiuluatqjecljotneymcbcxezqptbsl";
	int yveriuprqrmggvf = 6617;
	if (string("qnkippgpnbvnyecjaufmsqwatptsnjvragtlzhzhlduwvhsmhqprqsykmdwngpsvrdzqtkiuluatqjecljotneymcbcxezqptbsl") == string("qnkippgpnbvnyecjaufmsqwatptsnjvragtlzhzhlduwvhsmhqprqsykmdwngpsvrdzqtkiuluatqjecljotneymcbcxezqptbsl")) {
		int yxxqctd;
		for (yxxqctd = 6; yxxqctd > 0; yxxqctd--) {
			continue;
		}
	}
	if (42430 != 42430) {
		int iz;
		for (iz = 39; iz > 0; iz--) {
			continue;
		}
	}
	if (true != true) {
		int metzlhcdnc;
		for (metzlhcdnc = 59; metzlhcdnc > 0; metzlhcdnc--) {
			continue;
		}
	}
	if (string("qnkippgpnbvnyecjaufmsqwatptsnjvragtlzhzhlduwvhsmhqprqsykmdwngpsvrdzqtkiuluatqjecljotneymcbcxezqptbsl") != string("qnkippgpnbvnyecjaufmsqwatptsnjvragtlzhzhlduwvhsmhqprqsykmdwngpsvrdzqtkiuluatqjecljotneymcbcxezqptbsl")) {
		int vzuuvna;
		for (vzuuvna = 74; vzuuvna > 0; vzuuvna--) {
			continue;
		}
	}
	return true;
}

double typqmes::xrhaxmsebovvegyun(int mxmhrvmjor, string jgsyrhbdjje, string qwozrr, string ksvub, string itgjwwq, bool kjbtlvtbd, int nndiav, bool beuhsvfy, double qnyrbwzv, string xcblids) {
	return 57422;
}

void typqmes::qqdrddseppn(string kzzagsxdpuqa, int mckbx, string nclmmsqr, string tscdn, string vrgzzjsmgyrz, double jurmdoyah) {
	bool mgtztzlbvbfepof = true;
	double xhnnqtpyctqowr = 33217;
	bool wrtfedwfq = true;
	string zugxcywkmm = "yvjxalbozaetspenpjssaabdzrjqngaugbxumpixgyhsiqypqbrlfkzpslpacqunpkkhpltrjrcnvjcevsrntdjcrcujxvscitro";
	string vazjah = "gsnxtasqjjnhqbhi";
	double ezbneluzmtmobs = 21779;
	if (21779 != 21779) {
		int smonesbs;
		for (smonesbs = 62; smonesbs > 0; smonesbs--) {
			continue;
		}
	}
	if (string("gsnxtasqjjnhqbhi") == string("gsnxtasqjjnhqbhi")) {
		int wcj;
		for (wcj = 100; wcj > 0; wcj--) {
			continue;
		}
	}
	if (21779 != 21779) {
		int vattxh;
		for (vattxh = 25; vattxh > 0; vattxh--) {
			continue;
		}
	}
	if (true == true) {
		int gmgbrtjs;
		for (gmgbrtjs = 95; gmgbrtjs > 0; gmgbrtjs--) {
			continue;
		}
	}

}

void typqmes::pjdsvqzsppvxjszdiwzb(int johbheqvbtrpohm, double azzgblrkh) {
	double eervpouj = 21869;
	string ehrcbtfljgkbgn = "yntjdxhmwtqxqqxmmfkknlwxvv";
	if (string("yntjdxhmwtqxqqxmmfkknlwxvv") == string("yntjdxhmwtqxqqxmmfkknlwxvv")) {
		int ghmtmvzi;
		for (ghmtmvzi = 82; ghmtmvzi > 0; ghmtmvzi--) {
			continue;
		}
	}
	if (string("yntjdxhmwtqxqqxmmfkknlwxvv") == string("yntjdxhmwtqxqqxmmfkknlwxvv")) {
		int hlmapnvwr;
		for (hlmapnvwr = 11; hlmapnvwr > 0; hlmapnvwr--) {
			continue;
		}
	}
	if (21869 == 21869) {
		int wxmd;
		for (wxmd = 35; wxmd > 0; wxmd--) {
			continue;
		}
	}
	if (string("yntjdxhmwtqxqqxmmfkknlwxvv") == string("yntjdxhmwtqxqqxmmfkknlwxvv")) {
		int ecgtebxhg;
		for (ecgtebxhg = 22; ecgtebxhg > 0; ecgtebxhg--) {
			continue;
		}
	}

}

string typqmes::fejephptsmbymkckv(bool ylhkrlhdwq) {
	double qlwby = 15467;
	bool tciwwkclvlbi = false;
	double pugexa = 74955;
	string vtcqkbvjco = "swfrmh";
	string rxagarne = "yatngbikhallhxbegqyzgwfkzug";
	int ctmxbcwibg = 689;
	if (false != false) {
		int loqqmhr;
		for (loqqmhr = 73; loqqmhr > 0; loqqmhr--) {
			continue;
		}
	}
	if (689 == 689) {
		int atk;
		for (atk = 20; atk > 0; atk--) {
			continue;
		}
	}
	if (15467 == 15467) {
		int koh;
		for (koh = 81; koh > 0; koh--) {
			continue;
		}
	}
	if (689 != 689) {
		int coleibkmpn;
		for (coleibkmpn = 31; coleibkmpn > 0; coleibkmpn--) {
			continue;
		}
	}
	if (string("swfrmh") == string("swfrmh")) {
		int laj;
		for (laj = 49; laj > 0; laj--) {
			continue;
		}
	}
	return string("skwfw");
}

double typqmes::prtcuawagkccguuyqktw(int rhdpdrhxdihu, string oghokwqekkfvlkl) {
	string drkkheejezit = "mbqcauywbwfugmggopwbzulretfzydtfxlzumnffbierpxxpmgiwsrnbbrqjcrqsqrnwxjvdgdthfvfjzdlacbjwtdvafqlnlkq";
	double nmgiwtx = 40114;
	double uogwzvjpdzwovqc = 33541;
	string tmlresfjnfkcplv = "nkjoboxaawcnasbsfwaucwhblzjwvpmkezgvampfdplzuqntufjzfhnesmkdpvygjkmyaapwscxjcsnfjgzzxzz";
	string tttisgk = "pzcdljlppwzjybgqoxxajzgoxolahqhklxizpgpgovrx";
	double dzqmqqptaqgxeg = 9916;
	double mdhgghfsmpaim = 30256;
	double fblzcigw = 15206;
	double pikebupvcdqwas = 34268;
	if (30256 != 30256) {
		int ncgiufcvsp;
		for (ncgiufcvsp = 75; ncgiufcvsp > 0; ncgiufcvsp--) {
			continue;
		}
	}
	if (33541 != 33541) {
		int goyh;
		for (goyh = 93; goyh > 0; goyh--) {
			continue;
		}
	}
	if (15206 == 15206) {
		int iyyhlquop;
		for (iyyhlquop = 97; iyyhlquop > 0; iyyhlquop--) {
			continue;
		}
	}
	if (30256 != 30256) {
		int pgofjztbpj;
		for (pgofjztbpj = 70; pgofjztbpj > 0; pgofjztbpj--) {
			continue;
		}
	}
	if (33541 != 33541) {
		int sq;
		for (sq = 16; sq > 0; sq--) {
			continue;
		}
	}
	return 64036;
}

int typqmes::pueyuwucwgevetcdjcqq() {
	double hmpqkwhb = 37552;
	string prbonmz = "dildlxnbeeviqnjxcntqec";
	bool ozwfvvwqc = false;
	int ztthgrsvbq = 446;
	double yupaforuj = 9904;
	int mkyxgl = 2729;
	int eolywcrrride = 3255;
	int nhfjffquzfq = 1855;
	if (9904 != 9904) {
		int vkab;
		for (vkab = 64; vkab > 0; vkab--) {
			continue;
		}
	}
	if (2729 != 2729) {
		int tq;
		for (tq = 1; tq > 0; tq--) {
			continue;
		}
	}
	if (string("dildlxnbeeviqnjxcntqec") != string("dildlxnbeeviqnjxcntqec")) {
		int jxtyuev;
		for (jxtyuev = 15; jxtyuev > 0; jxtyuev--) {
			continue;
		}
	}
	return 52597;
}

string typqmes::qgsvhupmftle() {
	bool qcrzche = true;
	int ybslvpruqzpgx = 2131;
	double qgtffhwpmvbvml = 4241;
	bool fzbszedrmdzp = false;
	string bylthpknrjlx = "mwewhqyengpzmhfydydmlsnidprwjuntujydhzpw";
	double oefjx = 58435;
	return string("vbasqdvrtum");
}

double typqmes::xavvfmzvnqtfudfhi(string kmdkcbqhww) {
	int abwrsjlmhpiyic = 132;
	double xzpbjn = 28320;
	int ayrfriq = 95;
	double kzxtcjwvvjbx = 6273;
	string nvswyrcbcqmm = "liadcjdgyiyrlzbpwfudgzchvkrevwtxrcsqwlfaqdmuwvpenvhrstfcjjixxtfcoulecttmvuisbhafxnsikcxkyiaubwl";
	double qubefta = 35886;
	bool inyasmkphdyml = false;
	if (95 != 95) {
		int jafhz;
		for (jafhz = 33; jafhz > 0; jafhz--) {
			continue;
		}
	}
	if (28320 != 28320) {
		int dsfsvmaoqn;
		for (dsfsvmaoqn = 46; dsfsvmaoqn > 0; dsfsvmaoqn--) {
			continue;
		}
	}
	if (false == false) {
		int qljrvcl;
		for (qljrvcl = 97; qljrvcl > 0; qljrvcl--) {
			continue;
		}
	}
	if (false == false) {
		int ki;
		for (ki = 61; ki > 0; ki--) {
			continue;
		}
	}
	return 16713;
}

typqmes::typqmes() {
	this->xavvfmzvnqtfudfhi(string("gulsfgzriksswfenikarpivlmxmlxoihieplqwrtoyqfqhyyodmnjtugqoessjgcnjdujjmdusbxv"));
	this->fczmmqxpyiykulebhftwbct(32971, 3575, true, true, string("avlfnxcrkzcywxvfmmbup"), string("aywocmakekdtaadbaeroev"), 68677, string("ngklyedlzwvdeywsoyjfkkzrsjnnsnthqwzrkriesnydhjlkvthvlawnngolpojfnkmsiabjjdvamjrgttzigndazkbg"));
	this->xrhaxmsebovvegyun(1020, string(""), string("zfzlzizwnpjekfpmvempociakeyttkmjqqwr"), string("ffzgfqbyyrbqnpdcwjtihspnnhfqhjwrqkqgmqizylyraksucrsnbfytjqmjuujqeomclxnuziurzhsdiojfcitgugdefhsamvum"), string("acyufecbdazad"), true, 685, false, 23254, string("pghjrvvgifqrrqtlekpvaqpatwszvelxaegoohmyoqbw"));
	this->qqdrddseppn(string("oswveacvtqmdulvrxnratfuwuzetrdazrmpb"), 4477, string("eeltqxzxmjgcrtcyqcjjirtawizrpabjqtwzgrkliwxcotbscjiivuscujqdgckoo"), string("hmk"), string("klbaveoqqjadfdfqluomjrjmlpnrhgnheossizgqmkfvmxpcyabnmgpupefynervpjsnvpusbkpkoodgewgeiky"), 39762);
	this->pjdsvqzsppvxjszdiwzb(4441, 54481);
	this->fejephptsmbymkckv(false);
	this->prtcuawagkccguuyqktw(2295, string("ucwozzbyiqcoywacoymninwzvbymwyyfkcvgcyrphcitbptrbbqdigxwndjwckmwghdxxkklylrfqkpxukfihbprwxzmtxa"));
	this->pueyuwucwgevetcdjcqq();
	this->qgsvhupmftle();
	this->dtunigxwpmjjlevhkrikr(true, 183, string("nynxslnomhmsowgjvbfmbw"), string("thacieumizhnmfrrlghslaueidycqeuryfugkozpzjtqultglgqebeywxekcfemqyryml"), string("vmgrbjhkmqfrcnhmgtcyyzjnffluacjgfypordksirulivbrprnxxygxflerfwrhpflxrnrmfjajghgfkhyaubdqihw"), 490, 2935, string("mjggrxuvfaqjrdzoyryxhsnhvdtyqhijcgdiqigeuvjajy"), 3268);
	this->xjhdvntkbnwza(string("ftkkbsduoavlatqoazsscslowzwmsfvappxlwdljausulvbtzwsclgfiedhwhtaqyavsvxpjvqmpsmxgfctjaidnxn"), string("zsdnipzrwlemslyuoqvzjbjrctsaz"), string("sfpjwuhniopdztzqymc"), string("artrubgwysrrxxglskduhomjfmywhkoonekkbbdxs"), string("qmdzpuicpzfvqlsnkjcvzxrvurducqitolklvfrpv"), true, 6034, false, true, 6159);
	this->ookczvpfkhuavzauxv(false, 7008, false, 1721, string("eedckrspfkfnkktqaprfbqkcm"), string(""), false, true, true, 367);
	this->ibvglwylsy();
	this->gxsekgkyghiptkqndpdxk(78629, string("nyfxbvfcuvlpsi"), string("cmyugoifsdwvvviwofycqgflb"), true, 41814, string("jnwlzsvdhzzmqspiyffrflnttacryssvmbaexaupuzppcwpzrybekpdftsnqswbnkokwcgfcxkdxfe"), 31023, 8882, 1166, 2187);
	this->tefuunoqcdhprajqxwlac(string("nbixvypgirvcozfsenzmzaxnslmzwefckdsmfnvesolocfpmmbijtbigaeoreehjrbjndepgebnlijymv"), string("xkymbtbxalymzvgptzaqgrbyqm"), string("jidouaqyvbumtkczrthlmvxufmmryqcplujgguyhuynjmigznwnbuwhpp"), false, true, true, 537, false, 988, string("bjnicjbtaqtdfxyrocnrknrmjkiahtridbneuxxhjcumxefgmofrbpmkfhmdgszrwkp"));
	this->ugdimwgmdabspfsbphis();
	this->qywowheytk(22789, true, 7042, false);
}
                                             

















































































































































































































































































