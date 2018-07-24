#pragma once
#include "Configuration.hpp"
#include "dropboxes.h"
#include "Variables.h"
#include "Themes.h"
#include <cctype>
#include "Logging.h"
#include "Listener.h"
#include "IDirect3DDevice9.h"
#include "Themes.h"
#include "Hooks.h"
#include <ShlObj_core.h>
typedef void(*CL_FullUpdate_t) (void);
CL_FullUpdate_t CL_FullUpdate = nullptr;
std::vector<std::string> configs;



void GetConfigMassive()
{
	//get all files on folder

	configs.clear();

	static char path[MAX_PATH];
	std::string szPath1;

	if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path)))
		return;

	szPath1 = std::string(path) + "\\tragicpw-configs\\*";


	WIN32_FIND_DATA FindFileData;
	HANDLE hf;

	configs.push_back("Config");

	hf = FindFirstFile(szPath1.c_str(), &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		do {
			std::string filename = FindFileData.cFileName;

			if (filename == ".")
				continue;

			if (filename == "..")
				continue;

			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (filename.find(".ini") != std::string::npos)
				{
					configs.push_back(std::string(filename));
				}
			}
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
	}
}
void KnifeApplyCallbk()
{
	static auto CL_FullUpdate = reinterpret_cast<CL_FullUpdate_t>(U::FindPattern("engine.dll", reinterpret_cast<PBYTE>("\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56\xFF\x50\x14\x8B\x34\x85"), "x????x????xxxxxxx"));
	CL_FullUpdate();
}

void UpdateModelChanger()
{
	
}


void dankestSetClanTag(const char* tag, const char* name)
{
	static auto ClanTagOffset = U::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");
	if (ClanTagOffset)
	{
		if (strlen(name) > 0) {
			auto name_ = std::string(name);
			auto newline = name_.find("\\n");
			auto tab = name_.find("\\t");
			if (newline != std::string::npos) {
				name_.replace(newline, newline + 2, "\n");
			}
			if (tab != std::string::npos) {
				name_.replace(tab, tab + 2, "\t");
			}
		}
		auto tag_ = std::string(tag);
		if (strlen(tag) > 0) {
			auto newline = tag_.find("\\n");
			auto tab = tag_.find("\\t");
			if (newline != std::string::npos) {
				tag_.replace(newline, newline + 2, "\n");
			}
			if (tab != std::string::npos) {
				tag_.replace(tab, tab + 2, "\t");
			}
		}
		static auto dankesttSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(ClanTagOffset);
		dankesttSetClanTag(tag_.data(), tag_.data());
	}
}

namespace ImGui
{
	class Tab
	{
	private:

		std::vector<std::string> labels;

	public:

		void add(std::string name)
		{
			labels.push_back(name);
		}

		void draw(int *selected)
		{
			ImGuiStyle &style = GetStyle();
			ImVec4 color = style.Colors[ImGuiCol_Button];
			ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
			ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
			ImVec2 max = GetContentRegionMax();
			float size_x = max.x / labels.size() - 0.f;
			float size_y = max.y / labels.size() - 30.f;

			for (size_t i = 0; i < labels.size(); i++)
			{
				if (i == *selected)
				{
					style.Colors[ImGuiCol_Button] = colorActive;
					style.Colors[ImGuiCol_ButtonActive] = colorActive;
					style.Colors[ImGuiCol_ButtonHovered] = colorActive;
				}
				else
				{
					style.Colors[ImGuiCol_Button] = color;
					style.Colors[ImGuiCol_ButtonActive] = colorActive;
					style.Colors[ImGuiCol_ButtonHovered] = colorHover;
				}

				if (Button(labels.at(i).c_str(), { size_x, size_y }))
					*selected = i;
			}

			style.Colors[ImGuiCol_Button] = color;
			style.Colors[ImGuiCol_ButtonActive] = colorActive;
			style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		}
	};
}

const char* hitsound[] =
{
	"Disabled",
	"Default",
	"tragic.pw",
	"Skeet"
};



#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define BUILDSTAMP ( __DATE__ )


void FullUpdate()
{
	static auto CL_FullUpdate = reinterpret_cast<CL_FullUpdate_t>(U::FindPattern("engine.dll", reinterpret_cast<PBYTE>("\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56\xFF\x50\x14\x8B\x34\x85"), "x????x????xxxxxxx"));
	CL_FullUpdate();
}



bool Contains(const std::string &word, const std::string &sentence) {
	if (word == "" || sentence == "")
		return true;

	return sentence.find(word) != std::string::npos;
}

std::string ToLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))std::tolower);

	return str;
}
//--------------------------------------------


void RenderInterface() {

	//g_Render->DrawFillBox(0, 0, iScreenWidth, iScreenHeight, Color(0, 0, 0, 170));
	bool is_renderer_active = renderer->IsActive();

	if (is_renderer_active)
	{
		g_Options.Menu.bShowTabs = true;
	}
	else
	{
		g_Options.Menu.bShowTabs = false;
	}

	if (is_renderer_active)
	{

		if (Globals::error)
		{
			ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(200, 50)); //200 50
			if (ImGui::Begin(XorStr("Warning!"), &g_Options.Menu.Opened, ImVec2(400, 200), 1.f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders))
			{
				ImGui::Text(XorStr("ah ah ahhh! one at a time please!"));
				if (ImGui::Button(XorStr("OK!")))
				{
					if (g_Options.Ragebot.error_type == 1)
					{
						g_Options.Ragebot.run_PreAAs = false;
						g_Options.Ragebot.run_BuilderAAs = false;
						Globals::error = false;
						g_Options.Ragebot.error_type == 0;
					}
					else if (g_Options.Ragebot.error_type == 2)
					{
						g_Options.Ragebot.walk_PreAAs = false;
						g_Options.Ragebot.walk_BuilderAAs = false;
						Globals::error = false;
						g_Options.Ragebot.error_type == 0;
					}
					else if (g_Options.Ragebot.error_type == 3)
					{
						g_Options.Ragebot.crouch_PreAAs = false;
						g_Options.Ragebot.crouch_BuilderAAs = false;
						Globals::error = false;
						g_Options.Ragebot.error_type == 0;
					}
					else if (g_Options.Ragebot.error_type == 4)
					{
						g_Options.Ragebot.fwalk_PreAAs = false;
						g_Options.Ragebot.fwalk_BuilderAAs = false;
						Globals::error = false;
						g_Options.Ragebot.error_type == 0;
					}
					else if (g_Options.Ragebot.error_type == 5)
					{
						g_Options.Ragebot.stand_PreAAs = false;
						g_Options.Ragebot.stand_BuilderAAs = false;
						Globals::error = false;
						g_Options.Ragebot.error_type == 0;
					}
				}
			}ImGui::End();
		}
		if (!g_Options.Misc.antiuntrusted)
		{
			ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(200, 50)); //200 50
			if (ImGui::Begin(XorStr("Warning!"), &g_Options.Menu.Opened, ImVec2(20, 30), 1.f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders))
			{
				ImGui::Text(XorStr("Anti-Untrusted Disabled!"));
			}ImGui::End();
		}


		if (ImGui::Begin("##chet", &is_renderer_active, ImVec2(780, 613), 1.f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{													//1040		//613

			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
				ImGui::Text(XorStr("TRAGIC.PW"));
				ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
			}
			/*	static float r = 1.0f;
			static float g = 0.f;
			static float b = 0.f;

			if (r == 1.f && g >= 0.f && b <= 0.f)
			{
			g += 0.005f;
			b = 0.f;
			}
			if (r <= 1.f && g >= 1.f && b == 0.f)
			{
			g = 1.f;
			r -= 0.005f;
			}
			if (r <= 0.f && g == 1.f && b >= 0.f)
			{
			r = 0.f;
			b += 0.005f;
			}
			if (r == 0.f && g <= 1.f && b >= 1.f)
			{
			b = 1.f;
			g -= 0.005f;
			}
			if (r >= 0.f && g <= 0.f && b == 1.f)
			{
			g = 0.f;
			r += 0.005f;
			}
			if (r >= 1.f && g >= 0.f && b <= 1.f)
			{
			r = 1.f;
			b -= 0.005f;
			}

			ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(r, g, b, 1.f));

			ImGui::BeginChild("##rainbotcrapnignog", ImVec2(1600.f, 3.f));
			ImGui::EndChild();
			ImGui::PopStyleColor();
			*/
			auto color_grey = ImGui::GetColorU32(ImVec4(0.4, 0.4, 0.4, 0.30));
			auto color_black = ImGui::GetColorU32(ImVec4(0.0, 0.0, 0.0, 0.20));
			ImVec2 p = ImGui::GetCursorScreenPos();

			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 2), ImColor(55, 177, 218), ImColor(201, 84, 192), ImColor(201, 84, 192), ImColor(55, 177, 218));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), ImColor(201, 84, 192), ImColor(204, 227, 54), ImColor(204, 227, 54), ImColor(201, 84, 192));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 2), color_black, color_black, color_black, color_black);
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), color_black, color_black, color_black, color_black);
			ImGui::Spacing();

			static int tabselected = 0;
			ImGui::BeginChild("##tabshit", ImVec2(100.f, 580.f)); //100 580 lef
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.07, 0.07, 0.07, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04, 0.04, 0.04, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.01, 0.01, 0.01, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.07f, 0.07f, 0.07f, 1.f));
				ImGui::PushFont(fDefault);
				if (ImGui::Button("Changelog", ImVec2(100, 25))) //rage
					tabselected = 0;
				ImGui::PushFont(fDefault);
				if (ImGui::Button("Experimental", ImVec2(100, 25))) //rage
					tabselected = 8;
				//	if (ImGui::Button("Configs", ImVec2(120, 25))) //rage
				//		tabselected = 6;

				ImGui::PushFont(fbadcache);
				if (ImGui::Button("A", ImVec2(100, 75.5))) //legitbot
					tabselected = 1;
				if (ImGui::Button("I", ImVec2(100, 75.5))) //visuals
					tabselected = 2;
				if (ImGui::Button("D", ImVec2(100, 75.5)))  //misc
					tabselected = 3;
				if (ImGui::Button("G", ImVec2(100, 75.5))) //skinchanger
					tabselected = 4;
				if (ImGui::Button("B", ImVec2(100, 75.5))) //skinchanger
					tabselected = 5;

				ImGui::PopFont();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17f, 0.17f, 0.17f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.07f, 0.07f, 0.07f, 1.f));
				ImGui::PushFont(fDefault);
				ImGui::PushItemWidth(120);
				/*	static float r = 1.0f;
				static float g = 0.f;
				static float b = 0.f;

				if (r == 1.f && g >= 0.f && b <= 0.f)
				{
				g += 0.005f;
				b = 0.f;
				}
				if (r <= 1.f && g >= 1.f && b == 0.f)
				{
				g = 1.f;
				r -= 0.005f;
				}
				if (r <= 0.f && g == 1.f && b >= 0.f)
				{
				r = 0.f;
				b += 0.005f;
				}
				if (r == 0.f && g <= 1.f && b >= 1.f)
				{
				b = 1.f;
				g -= 0.005f;
				}
				if (r >= 0.f && g <= 0.f && b == 1.f)
				{
				g = 0.f;
				r += 0.005f;
				}
				if (r >= 1.f && g >= 0.f && b <= 1.f)
				{
				r = 1.f;
				b -= 0.005f;
				}

				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(r, g, b, 1.f));

				ImGui::BeginChild("##rainbotcrapnignog", ImVec2(1600.f, 3.f));
				ImGui::EndChild();
				ImGui::PopStyleColor();
				*/


				// CONFIGS HERE
				/*ImGui::PopFont(); ImGui::Combo((""), &g_Options.Menu.ConfigFile, configFiles, ARRAYSIZE(configFiles));
				if (ImGui::Button("     Save Config         ")) Config->Save();
				if (ImGui::Button("     Load Config       ")) Config->Load();
				if (ImGui::Button(XorStr("        Unhook          ")))
				{
				g_Engine->ClientCmd_Unrestricted("cl_mouseenable 1");
				unload = true;
				}


				*/

				//	ImGui::Spacing();

				ImGui::PushItemWidth(100);

				static int selectedcfg = 0;
				static std::string cfgname = "default";

				GetConfigMassive();

				if (ImGui::Combo("", &selectedcfg, [](void* data, int idx, const char** out_text)
				{
					*out_text = configs[idx].c_str();
					return true;
				}, nullptr, configs.size(), 10))
				{
					cfgname = configs[selectedcfg];
					cfgname.erase(cfgname.length() - 4, 4);
					strcpy(g_Options.ConfigName, cfgname.c_str());
				}

			}

			static bool create;
			if (ImGui::Button("Save", ImVec2(-55, 23))) Config->Save();
			ImGui::SameLine();
			if (ImGui::Button("Load", ImVec2(-1, 23)))
			{
				Config->Load();
				FullUpdate();
			}

			if (ImGui::Button("New Config", ImVec2(-1, 23))) create = !create;
			if (ImGui::Button("Reset Settings", ImVec2(-1, 23))) Config->Setup();

			if (create)
			{
				ImGui::Begin("##Create", &create, ImVec2(200, 300), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
				auto color_grey = ImGui::GetColorU32(ImVec4(0.4, 0.4, 0.4, 0.30));
				auto color_black = ImGui::GetColorU32(ImVec4(0.0, 0.0, 0.0, 0.20));
				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 2), ImColor(55, 177, 218), ImColor(201, 84, 192), ImColor(201, 84, 192), ImColor(55, 177, 218));
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), ImColor(201, 84, 192), ImColor(204, 227, 54), ImColor(204, 227, 54), ImColor(201, 84, 192));
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 2), color_black, color_black, color_black, color_black);
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), color_black, color_black, color_black, color_black);
				ImGui::Spacing();


				ImGui::Text("Config Name:");
				ImGui::InputText("Name", g_Options.ConfigName, 52);
				if (ImGui::Button("Create and save"))
				{
					Config->Save();
					create = !create;
				}
				if (ImGui::Button("Cancel"))
					create = !create;
				ImGui::End();
			}


			if (ImGui::Button(XorStr("      Unhook         ")))
			{
				g_Engine->ClientCmd_Unrestricted("cl_mouseenable 1");
				unload = true;
			}
			ImGui::EndChild();

			ImGui::SameLine();
			if (g_Options.Visuals.Enabled)
			{

				ImDrawList* draw = ImGui::GetWindowDrawList();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
				auto& style = ImGui::GetStyle();
				style.WindowMinSize = ImVec2(190, 296);
				style.WindowPadding = ImVec2(8, 8);
				style.ItemSpacing = ImVec2(4, 4);

				if (ImGui::Begin("###esppreview", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse)) {

					auto color_grey = ImGui::GetColorU32(ImVec4(0.4, 0.4, 0.4, 0.30));
					auto color_black = ImGui::GetColorU32(ImVec4(0.0, 0.0, 0.0, 0.20));
					ImVec2 p = ImGui::GetCursorScreenPos();
					ImGui::Text("                        esp preview");

					ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 2), ImColor(55, 177, 218), ImColor(201, 84, 192), ImColor(201, 84, 192), ImColor(55, 177, 218));
					ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), ImColor(201, 84, 192), ImColor(204, 227, 54), ImColor(204, 227, 54), ImColor(201, 84, 192));
					ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 2), color_black, color_black, color_black, color_black);
					ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 1), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), color_black, color_black, color_black, color_black);
					ImGui::Spacing();

					ImGui::BeginChild("##esppreview22", ImVec2(255, 257), true);
					{
						int alpha = 0;
						const char* name = "";
						ImGui::Dummy(ImVec2(32, 0));
						ImGui::SameLine();
						if (g_Options.Visuals.Name) {
							name = "name";
						}
						ImGui::Text(name);
						ImVec2 p = ImGui::GetCursorScreenPos();
						ImGui::Spacing();
						ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
						if (g_Options.Visuals.HP) {
							alpha = 255;
						}
						else
						{
							alpha = 0;
						}
						ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x, p.y), ImVec2(p.x + 2, p.y + 175), ImColor(0, 255, 0, alpha), 0.0f, 15, 1.f);
						ImGui::SameLine(12);
						if (g_Options.Visuals.Box) {
							ImVec2 p1 = ImGui::GetCursorScreenPos();
							ImGui::GetWindowDrawList()->AddRect(ImVec2(p1.x, p1.y), ImVec2(p1.x + 100, p1.y + 175), ImColor(255, 0, 0, 255), 0.0f, 15, 1.f);
						}

						if (g_Options.Visuals.money) {
							ImGui::Text("                              $0");
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 0, 0, 255));
						}

						if (g_Options.Visuals.Weapon) {
							//ImGui::SameLine(116);
							ImGui::Text("");
							ImGui::Text("");
							ImGui::Text("");
							ImGui::Text("");
							ImGui::Text("");
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Text("         weapon");
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
						}
					}ImGui::EndChild();
				} ImGui::End();
			}

			ImGui::BeginChild("##b1g main area", ImVec2(900.f, 580.f)); //730 580
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17f, 0.17f, 0.17f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.07f, 0.07f, 0.07f, 1.f));
				ImGui::PushFont(fDefault);

				if (tabselected == 1)
				{

					{

						{


							//btndefault();
							{
								//ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0, 255, 0, 255));
								//ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
							}
							ImGui::Text(" ");
							ImGui::Text(" ");
							ImGui::SameLine();
							ImGui::BeginChild("##ragepage1", ImVec2(310, 545), true);
							{
								{
									{
										ImGui::PushFont(fDefault);
										ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
										ImGui::Text("Aimbot");
										ImGui::PushFont(fDefault);
										ImGui::SameLine(260);
										ImGui::Checkbox(XorStr(""), &g_Options.Ragebot.MainSwitch);//
									
										ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									}
									ImGui::Separator();
									ImGui::Spacing();
									{
										ImGui::Checkbox(XorStr("Enabled"), &g_Options.Ragebot.Enabled);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Enable Rage Mod");
										ImGui::SliderFloat(XorStr("FOV"), &g_Options.Ragebot.FOV, 1.f, 180.f, "%.0f");//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Field for aimbot");
										ImGui::Checkbox(XorStr("Silent Aim"), &g_Options.Ragebot.Silent);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Aimbot will not lock on, client side.");
										ImGui::Checkbox(XorStr("Auto Pistol"), &g_Options.Ragebot.AutoPistol);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Pistols automatically fire");
										ImGui::Checkbox(XorStr("Recoil Control"), &g_Options.Ragebot.AntiRecoil);
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Controls recoil");
										ImGui::Checkbox(XorStr("RageBackTrack"), &g_Options.Ragebot.PosAdjust);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Shoots previous positions");
										ImGui::Checkbox(XorStr("Auto Shoot"), &g_Options.Ragebot.AutoFire);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Automatically fire");
										ImGui::Checkbox(XorStr("Engine prediction"), &g_Options.Ragebot.EnginePred);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Some Prediction");
										ImGui::Checkbox(XorStr("Friendly Fire"), &g_Options.Ragebot.FriendlyFire);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Automatic shoot friends");
										ImGui::Checkbox(XorStr("Auto zeus"), &g_Options.Ragebot.AutoZeus);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Automatic shoot with zeus.Maybe crash.");
										//ImGui::Checkbox(XorStr("Player List"), &g_Options.Ragebot.playerlist);
										ImGui::Separator();
										{
											ImGui::PushFont(fDefault);
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
											ImGui::Text("Min AutoWall Damage");
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
											ImGui::PushFont(fDefault);
										}
										ImGui::Separator();

										ImGui::PushItemWidth(190);
										ImGui::SliderFloat(("Snipers##gay"), &g_Options.Ragebot.MinimumDamageSniper, 1.f, 100.f, "%.2f");//
										ImGui::SliderFloat(("Rifles##gay"), &g_Options.Ragebot.MinimumDamageRifle, 1.f, 100.f, "%.2f");//
										ImGui::SliderFloat(("Pistols##gay"), &g_Options.Ragebot.MinimumDamagePistol, 1.f, 100.f, "%.2f");//
										ImGui::SliderFloat(("Heavies##gay"), &g_Options.Ragebot.MinimumDamageHeavy, 1.f, 100.f, "%.2f");//
										ImGui::SliderFloat(("SMGs##gay"), &g_Options.Ragebot.MinimumDamageSmg, 1.f, 100.f, "%.2f");//
										ImGui::SliderFloat(("R8 / Deagle##gay"), &g_Options.Ragebot.MinimumDamageRevolver, 1.f, 100.f, "%.2f");//

										ImGui::Separator();
										{
											ImGui::PushFont(fDefault);
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
											ImGui::Text("Other");
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
											ImGui::PushFont(fDefault);
										}
										ImGui::Separator();

										ImGui::Text(XorStr("Rage On Key"));
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Toggle ragebot on key press");
										ImGui::SameLine(160);
										ImGui::PushItemWidth(150);
										ImGui::Hotkey(XorStr("##gay 1"), &g_Options.Ragebot.KeyPress);//
										ImGui::Text(XorStr("BAIM Key   "));
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Only body aim on key press");
										ImGui::SameLine(160);
										ImGui::PushItemWidth(150);
										ImGui::Hotkey(XorStr("##biggest gay"), &g_Options.Ragebot.BAIMkey);//


										ImGui::Text(XorStr("Flip AA"));
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Flip anti-aim 180 degrees");

										ImGui::SameLine(160);
										ImGui::PushItemWidth(150);
										ImGui::Hotkey(XorStr("##dawjbdkajwbwd"), &g_Options.Ragebot.flipkey);//
										ImGui::Checkbox(XorStr("Fakewalk"), &g_Options.Misc.fakewalk);//
										ImGui::SameLine(160);
										ImGui::PushItemWidth(150);
										ImGui::Hotkey(XorStr("##fakewalk key123"), &g_Options.Misc.fakewalkkey);//
									
										if (g_Options.Misc.fakewalk == true)
										{
											ImGui::Text(XorStr("Glitch FakeWalk")); ImGui::SameLine(160); ImGui::PushItemWidth(150);
											ImGui::Hotkey(XorStr("##glitch key123"), &g_Options.Misc.glitchfakewalk);//
										}
										ImGui::Checkbox(XorStr("Fakelag Fix"), &g_Options.Ragebot.FakeLagFix);//
										if (ImGui::IsItemHovered())
											ImGui::SetTooltip("Deactivates client sided interpolation");
										ImGui::PushItemWidth(190);
										ImGui::Checkbox(XorStr("Resolver Enable"), &g_Options.Ragebot.resenable);//
										if (g_Options.Ragebot.resenable)
										{
											ImGui::Combo(XorStr("Pitch Adjustment for mirrors"), &g_Options.Ragebot.PitchAdjustment, resolverspitch, ARRAYSIZE(resolverspitch));//
											ImGui::Combo(XorStr("Resolvers"), &g_Options.Ragebot.Resolver, resolvers, ARRAYSIZE(resolvers));//
											ImGui::Text(XorStr("Override[miror]")); ImGui::SameLine(160); ImGui::PushItemWidth(150);
											ImGui::Hotkey(XorStr("##overridekey"), &g_Options.Misc.overridekey);//
											ImGui::Text(XorStr("Bruteforce after X shots:"));
											ImGui::PushItemWidth(190);
											ImGui::SliderFloat(XorStr("##shots123"), &g_Options.Ragebot.bruteAfterX, 0, 10, "%1.f");//
										}
										
									}
								}
							}
						}



					}ImGui::EndChild();

					ImGui::SameLine();
					ImGui::Text(" ");
					ImGui::SameLine();

					ImGui::BeginChild("##ragepag2", ImVec2(310, 545), true);
					{
						{
							ImGui::PushFont(fDefault);
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
							ImGui::Text("Accuracy");
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
							ImGui::PushFont(fDefault);
						}
						ImGui::Separator();
						ImGui::Spacing();
						{

							ImGui::Checkbox(XorStr("Auto Stop"), &g_Options.Ragebot.AutoStop);//
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip("Automatically stop beore shooting, to increase accuracy ");
							ImGui::SameLine(160);
							ImGui::PushItemWidth(150);
							ImGui::Hotkey(XorStr("##autostop key"), &g_Options.Ragebot.autostopkey);//
							ImGui::Checkbox(XorStr("Auto Scope"), &g_Options.Ragebot.AutoScope);//
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip("Automatically scopes weapon before shooting");
							ImGui::Checkbox(XorStr("Auto Crouch"), &g_Options.Ragebot.AutoCrouch);//
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip("Automatically crouch beore shooting, to increase accuracy ");
							ImGui::Checkbox(XorStr("Hitchance"), &g_Options.Ragebot.Hitchance);//
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip("Speed of which the aimbot will shoot at");

							ImGui::PushItemWidth(190);

							ImGui::SliderFloat(XorStr("Snipers"), &g_Options.Ragebot.HitchanceSniper, 0.f, 100.f, "%.1f");//
							ImGui::SliderFloat(XorStr("Rifles"), &g_Options.Ragebot.HitchanceRifle, 0.f, 100.f, "%.1f");//
							ImGui::SliderFloat(XorStr("Pistols"), &g_Options.Ragebot.HitchancePistol, 0.f, 100.f, "%.1f");//
							ImGui::SliderFloat(XorStr("SMGs"), &g_Options.Ragebot.HitchanceSmgs, 0.f, 100.f, "%.1f");//
							ImGui::SliderFloat(XorStr("Heavies"), &g_Options.Ragebot.HitchanceHeavy, 0.f, 100.f, "%.1f");//
							ImGui::SliderFloat(XorStr("R8 / Deagle"), &g_Options.Ragebot.HitchanceRevolver, 0.f, 100.f, "%.1f");//
							ImGui::PushItemWidth(190);

							ImGui::Text("Hitbox");
							ImGui::Combo(XorStr("##hitscan123"), &g_Options.Ragebot.Hitbox, aimBones, ARRAYSIZE(aimBones));//
							ImGui::Text("Hitscan");
							ImGui::Combo(XorStr("##hitbox123"), &g_Options.Ragebot.Hitscan, hitscan, ARRAYSIZE(hitscan));//
							ImGui::Separator();
							{
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("Anti-Aim");
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
							}
							ImGui::Separator();
							ImGui::BeginChild("##aa's", ImVec2(310.f, 288.f));
							{
								static int movementtype = 0;
								ImGui::Checkbox(XorStr("Enable Anti-Aim"), &g_Options.Ragebot.EnabledAntiAim);
								ImGui::Checkbox(XorStr("Broke Anti-Aim"), &g_Options.Visuals.GhostChams);
								if (ImGui::Button("Run", ImVec2(50, 25)))
									movementtype = 0;
								ImGui::SameLine();
								if (ImGui::Button("Walk", ImVec2(50, 25)))
									movementtype = 1;
								ImGui::SameLine();
								if (ImGui::Button("Stand", ImVec2(50, 25)))
									movementtype = 2;
								ImGui::SameLine();
								if (ImGui::Button("Fake", ImVec2(50, 25)))
									movementtype = 3;
								ImGui::SameLine();
								if (ImGui::Button("Crouch", ImVec2(50, 25)))
									movementtype = 4;

								static int aatabtype = 0;
								static int aatabtype1 = 0;
								static int aatabtype2 = 0;
								static int aatabtype3 = 0;
								static int aatabtype4 = 0;

								switch (movementtype)
                                          case 0:
								{

									ImGui::Checkbox(XorStr("AA On Run"), &g_Options.Ragebot.AA_onRun);
									ImGui::SameLine();

									ImGui::Checkbox(XorStr("AA on knife"), &g_Options.Ragebot.KnifeAA);
									ImGui::SameLine();

									ImGui::Checkbox(XorStr("Edge"), &g_Options.Ragebot.Edge);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Anti-aim will be angled towards walls ");

									if (ImGui::Button("Presets", ImVec2(150, 25))) aatabtype = 0;
									ImGui::SameLine();
									if (ImGui::Button("Self Made", ImVec2(150, 25))) aatabtype = 1;

									if (aatabtype == 0) //run_
									{
										ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.run_PreAAs);
										ImGui::SameLine();
										ImGui::Checkbox("fakehead", &g_Options.Ragebot.run_fakehead);
										ImGui::Combo(XorStr("Pitch"), &g_Options.Ragebot.run_Pitch, antiaimpitch, ARRAYSIZE(antiaimpitch));
										ImGui::SliderFloat(XorStr("Pitch Adder: "), &g_Options.Ragebot.run_PitchAdder, -180, 180, "%1.f");
										ImGui::Separator();
										ImGui::Combo(XorStr("Yaw"), &g_Options.Ragebot.run_YawTrue, antiaimyawtrue, ARRAYSIZE(antiaimyawtrue));
										ImGui::SliderFloat(XorStr("Real Adder: "), &g_Options.Ragebot.run_YawTrueAdder, -180, 180, "%1.f");
										ImGui::Separator();
										ImGui::Combo(XorStr("Fake-Yaw"), &g_Options.Ragebot.run_YawFake, antiaimyawfake, ARRAYSIZE(antiaimyawfake));
										ImGui::SliderFloat(XorStr("Fake Adder: "), &g_Options.Ragebot.run_YawFakeAdder, -180, 180, "%1.f");
									}
									else if (aatabtype == 1)
									{
										ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.run_BuilderAAs);
										ImGui::SliderFloat(XorStr("Pitch Angle: "), &g_Options.Ragebot.run_BuilderPitch, -89, 89, "%1.f");
										ImGui::SliderFloat(XorStr("Real Angle: "), &g_Options.Ragebot.run_BuilderReal, -180, 180, "%1.f");
										ImGui::SliderFloat(XorStr("Fake Angle: "), &g_Options.Ragebot.run_BuilderFake, -180, 180, "%1.f");
										ImGui::Checkbox(XorStr("Enable Real Jitter"), &g_Options.Ragebot.run_Jitter);
										ImGui::SliderFloat(XorStr("Jitter Range: "), &g_Options.Ragebot.run_JitterRange, -90, 90, "%1.f");
										ImGui::Checkbox(XorStr("Enable Fake Jitter"), &g_Options.Ragebot.run_FJitter);
										ImGui::SliderFloat(XorStr("FJitter Range: "), &g_Options.Ragebot.run_FJitterRange, -90, 90, "%1.f");
										ImGui::Checkbox(XorStr("LBY Breaker"), &g_Options.Ragebot.run_LBYBreaker);
									}
									break;
										  case 1:
											  ImGui::Checkbox(XorStr("AA On Walk"), &g_Options.Ragebot.AA_onWalk);


											  if (ImGui::Button("Presets", ImVec2(150, 25))) aatabtype1 = 0;
											  ImGui::SameLine();
											  if (ImGui::Button("Self Made", ImVec2(150, 25))) aatabtype1 = 1;

											  if (aatabtype1 == 0) //walk_
											  {
												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.walk_PreAAs);
												  ImGui::SameLine();
												  ImGui::Checkbox("fakehead", &g_Options.Ragebot.walk_fakehead);
												  ImGui::Combo(XorStr("Pitch"), &g_Options.Ragebot.walk_Pitch, antiaimpitch, ARRAYSIZE(antiaimpitch));
												  ImGui::SliderFloat(XorStr("Pitch Adder: "), &g_Options.Ragebot.walk_PitchAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Yaw"), &g_Options.Ragebot.walk_YawTrue, antiaimyawtrue, ARRAYSIZE(antiaimyawtrue));
												  ImGui::SliderFloat(XorStr("Real Adder: "), &g_Options.Ragebot.walk_YawTrueAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Fake-Yaw"), &g_Options.Ragebot.walk_YawFake, antiaimyawfake, ARRAYSIZE(antiaimyawfake));
												  ImGui::SliderFloat(XorStr("Fake Adder: "), &g_Options.Ragebot.walk_YawFakeAdder, -180, 180, "%1.f");
											  }
											  else if (aatabtype1 == 1)
											  {
												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.walk_BuilderAAs);
												  ImGui::SliderFloat(XorStr("Pitch Angle: "), &g_Options.Ragebot.walk_BuilderPitch, -89, 89, "%1.f");
												  ImGui::SliderFloat(XorStr("Real Angle: "), &g_Options.Ragebot.walk_BuilderReal, -180, 180, "%1.f");
												  ImGui::SliderFloat(XorStr("Fake Angle: "), &g_Options.Ragebot.walk_BuilderFake, -180, 180, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Real Jitter"), &g_Options.Ragebot.walk_Jitter);
												  ImGui::SliderFloat(XorStr("Jitter Range: "), &g_Options.Ragebot.walk_JitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Fake Jitter"), &g_Options.Ragebot.walk_FJitter);
												  ImGui::SliderFloat(XorStr("FJitter Range: "), &g_Options.Ragebot.walk_FJitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("LBY Breaker"), &g_Options.Ragebot.walk_LBYBreaker);
											  }
											  break;
										  case 2:
											  ImGui::Checkbox(XorStr("AA On Stand"), &g_Options.Ragebot.AA_onStand);



											  if (ImGui::Button("Presets", ImVec2(150, 25))) aatabtype2 = 0;
											  ImGui::SameLine();
											  if (ImGui::Button("Self Made", ImVec2(150, 25))) aatabtype2 = 1;

											  if (aatabtype2 == 0) //stand_
											  {
												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.stand_PreAAs);
												  ImGui::SameLine();
												  ImGui::Checkbox("fakehead", &g_Options.Ragebot.stand_fakehead);
												  ImGui::Combo(XorStr("Pitch"), &g_Options.Ragebot.stand_Pitch, antiaimpitch, ARRAYSIZE(antiaimpitch));
												  ImGui::SliderFloat(XorStr("Pitch Adder: "), &g_Options.Ragebot.stand_PitchAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Yaw"), &g_Options.Ragebot.stand_YawTrue, antiaimyawtrue, ARRAYSIZE(antiaimyawtrue));
												  ImGui::SliderFloat(XorStr("Real Adder: "), &g_Options.Ragebot.stand_YawTrueAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Fake-Yaw"), &g_Options.Ragebot.stand_YawFake, antiaimyawfake, ARRAYSIZE(antiaimyawfake));
												  ImGui::SliderFloat(XorStr("Fake Adder: "), &g_Options.Ragebot.stand_YawFakeAdder, -180, 180, "%1.f");

											  }
											  else if (aatabtype2 == 1)
											  {

												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.stand_BuilderAAs);
												  ImGui::SliderFloat(XorStr("Pitch Angle: "), &g_Options.Ragebot.stand_BuilderPitch, -89, 89, "%1.f");
												  ImGui::SliderFloat(XorStr("Real Angle: "), &g_Options.Ragebot.stand_BuilderReal, -180, 180, "%1.f");
												  ImGui::SliderFloat(XorStr("Fake Angle: "), &g_Options.Ragebot.stand_BuilderFake, -180, 180, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Real Jitter"), &g_Options.Ragebot.stand_Jitter);
												  ImGui::SliderFloat(XorStr("Jitter Range: "), &g_Options.Ragebot.stand_JitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Fake Jitter"), &g_Options.Ragebot.stand_FJitter);
												  ImGui::SliderFloat(XorStr("FJitter Range: "), &g_Options.Ragebot.stand_FJitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("LBY Breaker"), &g_Options.Ragebot.stand_LBYBreaker);
											  }
											  break;
										  case 3:
											  ImGui::Checkbox(XorStr("AA On FakeWalk"), &g_Options.Ragebot.AA_onFakeWalk);



											  if (ImGui::Button("Presets", ImVec2(150, 25))) aatabtype3 = 0;
											  ImGui::SameLine();
											  if (ImGui::Button("Self Made", ImVec2(150, 25))) aatabtype3 = 1;

											  if (aatabtype3 == 0) //fwalk_
											  {
												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.fwalk_PreAAs);
												  ImGui::SameLine();
												  ImGui::Checkbox("fakehead", &g_Options.Ragebot.fwalk_fakehead);
												  ImGui::Combo(XorStr("Pitch"), &g_Options.Ragebot.fwalk_Pitch, antiaimpitch, ARRAYSIZE(antiaimpitch));
												  ImGui::SliderFloat(XorStr("Pitch Adder: "), &g_Options.Ragebot.fwalk_PitchAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Yaw"), &g_Options.Ragebot.fwalk_YawTrue, antiaimyawtrue, ARRAYSIZE(antiaimyawtrue));
												  ImGui::SliderFloat(XorStr("Real Adder: "), &g_Options.Ragebot.fwalk_YawTrueAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Fake-Yaw"), &g_Options.Ragebot.fwalk_YawFake, antiaimyawfake, ARRAYSIZE(antiaimyawfake));
												  ImGui::SliderFloat(XorStr("Fake Adder: "), &g_Options.Ragebot.fwalk_YawFakeAdder, -180, 180, "%1.f");
											  }
											  else if (aatabtype3 == 1)
											  {
												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.fwalk_BuilderAAs);
												  ImGui::SliderFloat(XorStr("Pitch Angle: "), &g_Options.Ragebot.fwalk_BuilderPitch, -89, 89, "%1.f");
												  ImGui::SliderFloat(XorStr("Real Angle: "), &g_Options.Ragebot.fwalk_BuilderReal, -180, 180, "%1.f");
												  ImGui::SliderFloat(XorStr("Fake Angle: "), &g_Options.Ragebot.fwalk_BuilderFake, -180, 180, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Real Jitter"), &g_Options.Ragebot.fwalk_Jitter);
												  ImGui::SliderFloat(XorStr("Jitter Range: "), &g_Options.Ragebot.fwalk_JitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Fake Jitter"), &g_Options.Ragebot.fwalk_FJitter);
												  ImGui::SliderFloat(XorStr("FJitter Range: "), &g_Options.Ragebot.fwalk_FJitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("LBY Breaker"), &g_Options.Ragebot.fwalk_LBYBreaker);
											  }
											  break;
										  case 4:
											  ImGui::Checkbox(XorStr("AA On Crouch"), &g_Options.Ragebot.AA_onCrouch);



											  if (ImGui::Button("Presets", ImVec2(150, 25))) aatabtype4 = 0;
											  ImGui::SameLine();
											  if (ImGui::Button("Self Made", ImVec2(150, 25))) aatabtype4 = 1;

											  if (aatabtype4 == 0) //crouch_
											  {
												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.crouch_PreAAs);
												  ImGui::SameLine();
												  ImGui::Checkbox("fakehead", &g_Options.Ragebot.crouch_fakehead);
												  ImGui::Combo(XorStr("Pitch"), &g_Options.Ragebot.crouch_Pitch, antiaimpitch, ARRAYSIZE(antiaimpitch));
												  ImGui::SliderFloat(XorStr("Pitch Adder: "), &g_Options.Ragebot.crouch_PitchAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Yaw"), &g_Options.Ragebot.crouch_YawTrue, antiaimyawtrue, ARRAYSIZE(antiaimyawtrue));
												  ImGui::SliderFloat(XorStr("Real Adder: "), &g_Options.Ragebot.crouch_YawTrueAdder, -180, 180, "%1.f");
												  ImGui::Separator();
												  ImGui::Combo(XorStr("Fake-Yaw"), &g_Options.Ragebot.crouch_YawFake, antiaimyawfake, ARRAYSIZE(antiaimyawfake));
												  ImGui::SliderFloat(XorStr("Fake Adder: "), &g_Options.Ragebot.crouch_YawFakeAdder, -180, 180, "%1.f");
											  }
											  else if (aatabtype4 == 1)
											  {
												  ImGui::Checkbox(XorStr("Enable"), &g_Options.Ragebot.crouch_BuilderAAs);
												  ImGui::SliderFloat(XorStr("Pitch Angle: "), &g_Options.Ragebot.crouch_BuilderPitch, -89, 89, "%1.f");
												  ImGui::SliderFloat(XorStr("Real Angle: "), &g_Options.Ragebot.crouch_BuilderReal, -180, 180, "%1.f");
												  ImGui::SliderFloat(XorStr("Fake Angle: "), &g_Options.Ragebot.crouch_BuilderFake, -180, 180, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Real Jitter"), &g_Options.Ragebot.crouch_Jitter);
												  ImGui::SliderFloat(XorStr("Jitter Range: "), &g_Options.Ragebot.crouch_JitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("Enable Fake Jitter"), &g_Options.Ragebot.crouch_FJitter);
												  ImGui::SliderFloat(XorStr("FJitter Range: "), &g_Options.Ragebot.crouch_FJitterRange, -90, 90, "%1.f");
												  ImGui::Checkbox(XorStr("LBY Breaker"), &g_Options.Ragebot.crouch_LBYBreaker);
											  }
											  break;
								}

							}ImGui::EndChild();
						}ImGui::EndChild();
					}
				}
				else if (tabselected == 2)
				{
					static int pagesshit = 1;
					static int legitbotpages = 1;
					if (ImGui::Button("Legitbot", ImVec2(330.0f, 30.0f)))
					{
						legitbotpages = 1;
					}
					ImGui::SameLine();
					if (ImGui::Button("Triggerbot", ImVec2(330.0f, 30.0f)))
					{
						legitbotpages = 2;
					}
					if (legitbotpages == 1)
					{


						ImGui::BeginChild("#weaponssets", ImVec2(815, 50), false, ImGuiWindowFlags_NoScrollbar);
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.07, 0.07, 0.07, 1.f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.04, 0.04, 0.04, 1.f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.01, 0.01, 0.01, 1.f));
							ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.07f, 0.07f, 0.07f, 1.f));
							//		ImGui::Spacing();

							ImGui::PushFont(guns);
							ImGui::Separator();
							//	if (ImGui::Button("A", ImVec2(120, 76.5))) //legitbot

							if (ImGui::Button("W", ImVec2(105, 40))) //RIFLES
							{
								pagesshit = 1;
							}

							ImGui::SameLine();

							if (ImGui::Button("A", ImVec2(105, 40)))  //PISTOLS
							{
								pagesshit = 2;

							}
							ImGui::SameLine();
							if (ImGui::Button("Z", ImVec2(105, 40))) //SNIPERS
							{
								pagesshit = 3;
							}
							ImGui::SameLine();
							if (ImGui::Button("N", ImVec2(105, 40))) // SMG
							{
								pagesshit = 4;
							}
							ImGui::SameLine();
							if (ImGui::Button("f", ImVec2(105, 40))) //HEAVY
							{
								pagesshit = 5;
							}
							ImGui::SameLine();
							if (ImGui::Button("d", ImVec2(105, 40))) //SHOTTY
							{
								pagesshit = 6;
							}
							ImGui::Separator();
							ImGui::PopFont();

						}ImGui::EndChild();


						ImGui::Spacing();


						{
							ImGui::BeginChild("##LegitAA1", ImVec2(660, 210), true);
							ImGui::Columns(2, NULL, false);
							{
								{
									{
										ImGui::PushFont(fDefault);
										ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
										ImGui::Text("Miscellaneous"); ImGui::NextColumn(); ImGui::Text("Backtrack"); ImGui::NextColumn();
										ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
										ImGui::PushFont(fDefault);
									}
									ImGui::PushItemWidth(200);


									ImGui::Separator();
									ImGui::Spacing();
									ImGui::Checkbox(("Auto Pistol"), &g_Options.LegitBot.AutoPistol);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Pistols automotically fire if mouse1 is held.");
									
									ImGui::Checkbox(("Smoke Check"), &g_Options.LegitBot.SmokeCheck);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Aimbot will not lock on through a smoke.");
									ImGui::Checkbox(XorStr("pSilent"), &g_Options.Ragebot.Silent);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Crosshair wont snap to players, clientside. Will only work if smooth is set to 0.");
									ImGui::Checkbox(("Legit Anti-Aim"), &g_Options.LegitBot.LegitAA);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Moves your hitboxes away from your player model.");
									if (g_Options.LegitBot.LegitAA)
										ImGui::Combo((""), &g_Options.LegitBot.LegitChoiceList, LegitChoices, _ARRAYSIZE(LegitChoices));
									ImGui::Checkbox(("Legit-AA Resolver"), &g_Options.LegitBot.legitresolver);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Corrects players hitbox using legit anti-aim");
									


									ImGui::NextColumn();


									ImGui::Checkbox(("Legit Backtrack"), &g_Options.LegitBot.backtrack);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Abusing lag compensation, causing a players previous postion to teleport to your crosshair when shot.");
									ImGui::SliderFloat(("Ticks"), &g_Options.LegitBot.backtrackTicks, 0, 12, "%1.f");
									ImGui::Checkbox(("Backtrack line"), &g_Options.Visuals.backtrackline);
									if (ImGui::IsItemHovered())
										ImGui::SetTooltip("Visual representation of backtrack.");
									ImGui::SameLine();
									ImGui::MyColorEdit3(XorStr("##232gd"), (float*)&g_Options.Colors.backtrackdots_color, 1 << 7);
									ImGui::Text("Fake Latency");
									ImGui::SliderInt(XorStr("##fakeping"), &g_Options.Misc.FakePing2, 0.f, 900.f, "%.0f"); //pFakePing
								}ImGui::EndChild();
							}
						}


						if (pagesshit == 1)

						{

							ImGui::BeginChild("#Child1", ImVec2(320, 260), true);
							{
								//	btndefault();
								ImGui::PushItemWidth(300);

								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Rifles");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}
								ImGui::PushItemWidth(200);

								ImGui::Separator();

								ImGui::Spacing();

								ImGui::Hotkey(("##mainkeyKEY"), &g_Options.LegitBot.MainKey);
								ImGui::SameLine();
								ImGui::Text("Key");

								ImGui::Spacing();

								ImGui::SliderFloat(("Smooth"), &g_Options.LegitBot.MainSmooth, 1.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("FOV"), &g_Options.LegitBot.Mainfov, 0.00f, 30.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-X"), &g_Options.LegitBot.main_recoil_min, 0.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("RCS-Y"), &g_Options.LegitBot.main_recoil_max, 0.00f, 100.00f, "%.2f");





							}ImGui::EndChild();


							ImGui::SameLine(0.0, 15.0f);


							ImGui::BeginChild("##Hiboxes", ImVec2(325, 260), true);
							{
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Hitbox");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();
								ImGui::Checkbox(("Aim at Head"), &g_Options.LegitBot.FilterMainWeapons.Headwpmain);
								ImGui::Checkbox(("Aim at Neck"), &g_Options.LegitBot.FilterMainWeapons.Neckwpmain);
								ImGui::Checkbox(("Aim at Chest"), &g_Options.LegitBot.FilterMainWeapons.Chestwpmain);
								ImGui::Checkbox(("Aim at Stomach"), &g_Options.LegitBot.FilterMainWeapons.Stomachwpmain);
								ImGui::Checkbox(("Aim at MultiBone"), &g_Options.LegitBot.FilterMainWeapons.multiboneswpmain);

							}ImGui::EndChild();

						}

						if (pagesshit == 2)

						{

							ImGui::BeginChild("#Child2", ImVec2(320, 260), true);
							{
								//		btndefault();

								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Pistols");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}
								ImGui::PushItemWidth(200);


								ImGui::Separator();

								ImGui::Spacing();



								ImGui::Hotkey(("##pistolKEY"), &g_Options.LegitBot.PistolKey);
								ImGui::SameLine();
								ImGui::Text("Key");



								ImGui::Spacing();

								ImGui::SliderFloat(("Smooth"), &g_Options.LegitBot.PistolSmooth, 1.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("FOV"), &g_Options.LegitBot.Pistolfov, 0.00f, 30.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-X"), &g_Options.LegitBot.pistol_recoil_min, 0.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("RCS-Y"), &g_Options.LegitBot.pistol_recoil_max, 0.00f, 100.00f, "%.2f");
								ImGui::Separator();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("DEAGLE");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}
								ImGui::Separator();
								ImGui::Hotkey(("###pistolKEY"), &g_Options.LegitBot.DeagleKey, ImVec2(300, 25));
								ImGui::SameLine();
								ImGui::Text("Key");



								ImGui::Spacing();

								ImGui::SliderFloat(("Smooth "), &g_Options.LegitBot.DeagleSmooth, 1.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("FOV "), &g_Options.LegitBot.Deaglefov, 0.00f, 30.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-X "), &g_Options.LegitBot.Deagle_recoil_min, 0.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("RCS-Y "), &g_Options.LegitBot.Deagle_recoil_max, 0.00f, 100.00f, "%.2f");
							}ImGui::EndChild();


							ImGui::SameLine(0.0, 15.0f);

							ImGui::BeginChild("##Hiboxes2", ImVec2(325, 260), true);
							{
								//		btndefault();

								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Hitbox Pistols");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();

								ImGui::Checkbox(("Aim at Head"), &g_Options.LegitBot.FilterPistolWeapons.Headwppis);
								ImGui::Checkbox(("Aim at Neck"), &g_Options.LegitBot.FilterPistolWeapons.Neckwppis);
								ImGui::Checkbox(("Aim at Chest"), &g_Options.LegitBot.FilterPistolWeapons.Chestwppis);
								ImGui::Checkbox(("Aim at Stomach"), &g_Options.LegitBot.FilterPistolWeapons.Stomachwppis);
								ImGui::Checkbox(("Aim at MultiBone"), &g_Options.LegitBot.FilterPistolWeapons.multiboneswppis);

								ImGui::Separator();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Hitbox Deagle");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();

								ImGui::Checkbox(("Aim at Head "), &g_Options.LegitBot.FilterDeagleWeapons.HeadwpDeagle);
								ImGui::Checkbox(("Aim at Neck "), &g_Options.LegitBot.FilterDeagleWeapons.NeckwpDeagle);
								ImGui::Checkbox(("Aim at Chest "), &g_Options.LegitBot.FilterDeagleWeapons.ChestwpDeagle);
								ImGui::Checkbox(("Aim at Stomach "), &g_Options.LegitBot.FilterDeagleWeapons.StomachwpDeagle);
								ImGui::Checkbox(("Aim at MultiBone "), &g_Options.LegitBot.FilterDeagleWeapons.multiboneswpDeagle);
							}ImGui::EndChild();
						}

						if (pagesshit == 3)
						{
							ImGui::BeginChild("#Child3", ImVec2(320, 260), true);
							{
								//	btndefault();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Snipers");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::PushItemWidth(200);

								ImGui::Separator();

								ImGui::Spacing();


								ImGui::Hotkey(("##sniperkeyKEY"), &g_Options.LegitBot.SniperKey);
								ImGui::SameLine();
								ImGui::Text("Key");

								ImGui::Spacing();

								ImGui::SliderFloat(("Smooth"), &g_Options.LegitBot.SniperSmooth, 1.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("FOV"), &g_Options.LegitBot.Sniperfov, 0.00f, 30.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-X"), &g_Options.LegitBot.sniper_recoil_min, 0.00f, 100.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-Y"), &g_Options.LegitBot.sniper_recoil_max, 0.00f, 100.00f, "%.2f");


								ImGui::Separator();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Scout");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}
								ImGui::Separator();
								ImGui::Spacing();
								ImGui::Hotkey("Key##88", &g_Options.LegitBot.scoutKey);
								ImGui::Spacing();
								//ImGui::Combo(("Key##2"), &menu.Legitbot.scoutKey, keyNames, ARRAYSIZE(keyNames));
								ImGui::SliderFloat("Smooth##388", &g_Options.LegitBot.scoutSmooth, 1.00f, 50.00f, "%.2f");
								ImGui::Spacing();
								ImGui::SliderFloat("FOV##838", &g_Options.LegitBot.scoutfov, 0.00f, 30.00f, "%.2f");


							}ImGui::EndChild();


							ImGui::SameLine(0.0, 15.0f);

							ImGui::BeginChild("##Hiboxes2", ImVec2(325, 260), true);
							{
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Hitbox");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();


								ImGui::Checkbox(("Aim at Head "), &g_Options.LegitBot.FilterSniperWeapons.HeadwpSnip);
								ImGui::Checkbox(("Aim at Neck "), &g_Options.LegitBot.FilterSniperWeapons.NeckwpSnip);
								ImGui::Checkbox(("Aim at Chest "), &g_Options.LegitBot.FilterSniperWeapons.ChestwpSnip);
								ImGui::Checkbox(("Aim at Stomach "), &g_Options.LegitBot.FilterSniperWeapons.StomachwpSnip);
								ImGui::Checkbox(("Aim at MultiBone "), &g_Options.LegitBot.FilterSniperWeapons.multiboneswpSnip);





								ImGui::Separator();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Hitbox Scout");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}
								ImGui::Separator();
								ImGui::Checkbox(("Aim at Head  "), &g_Options.LegitBot.FilterScoutWeapons.HeadwpScout);
								ImGui::Checkbox(("Aim at Neck  "), &g_Options.LegitBot.FilterScoutWeapons.NeckwpScout);
								ImGui::Checkbox(("Aim at Chest  "), &g_Options.LegitBot.FilterScoutWeapons.ChestwpScout);
								ImGui::Checkbox(("Aim at Stomach  "), &g_Options.LegitBot.FilterScoutWeapons.StomachwpScout);
								ImGui::Checkbox(("Aim at MultiBone  "), &g_Options.LegitBot.FilterScoutWeapons.multiboneswpScout);
							}ImGui::EndChild();
						}

						if (pagesshit == 4)
						{

							ImGui::BeginChild("#Child4", ImVec2(320, 260), true);
							{
								//	btndefault();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("SMG");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();

								ImGui::Spacing();

								ImGui::PushItemWidth(200);



								ImGui::Hotkey(("##smgKEY"), &g_Options.LegitBot.smg_Key);
								ImGui::SameLine();
								ImGui::Text("Key");


								ImGui::Spacing();

								ImGui::SliderFloat(("Smooth"), &g_Options.LegitBot.smg_Smooth, 1.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("FOV"), &g_Options.LegitBot.smg_fov, 0.00f, 30.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-X"), &g_Options.LegitBot.smg_recoil_min, 0.00f, 100.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-Y"), &g_Options.LegitBot.smg_recoil_max, 0.00f, 100.00f, "%.2f");


							}ImGui::EndChild();


							ImGui::SameLine(0.0, 15.0f);


							ImGui::BeginChild("##Hiboxes3", ImVec2(325, 260), true);
							{
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Hitbox");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();

								ImGui::Checkbox(("Aim at Head"), &g_Options.LegitBot.FiltersmgWeapons.Headwpsmg);
								ImGui::Checkbox(("Aim at Neck"), &g_Options.LegitBot.FiltersmgWeapons.Neckwpsmg);
								ImGui::Checkbox(("Aim at Chest"), &g_Options.LegitBot.FiltersmgWeapons.Chestwpsmg);
								ImGui::Checkbox(("Aim at Stomach"), &g_Options.LegitBot.FiltersmgWeapons.Stomachwpsmg);
								ImGui::Checkbox(("Aim at MultiBone"), &g_Options.LegitBot.FiltersmgWeapons.multiboneswpsmg);

							}ImGui::EndChild();

						}

						if (pagesshit == 5)
						{
							ImGui::BeginChild("#Child5", ImVec2(320, 260), true);
							{
								//		btndefault();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Heavy");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();

								ImGui::Spacing();

								ImGui::PushItemWidth(200);

								ImGui::Hotkey(("##heavyKEY"), &g_Options.LegitBot.heavy_wp_Key);
								ImGui::SameLine();
								ImGui::Text("Key");


								ImGui::Spacing();

								ImGui::SliderFloat(("Smooth"), &g_Options.LegitBot.heavy_wp_Smooth, 1.00f, 100.00f, "%.2f");

								ImGui::Spacing();

								ImGui::SliderFloat(("FOV"), &g_Options.LegitBot.heavy_wp_fov, 0.00f, 30.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-X"), &g_Options.LegitBot.heavy_wp_recoil_min, 0.00f, 100.00f, "%.2f");

								ImGui::Spacing();


								ImGui::SliderFloat(("RCS-Y"), &g_Options.LegitBot.heavy_wp_recoil_max, 0.00f, 100.00f, "%.2f");



							}ImGui::EndChild();


							ImGui::SameLine(0.0, 15.0f);


							ImGui::BeginChild("##Hiboxes4", ImVec2(325, 260), true);
							{
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Hitbox");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}

								ImGui::Separator();



								ImGui::Checkbox(("Aim at Head"), &g_Options.LegitBot.FilterheavyWeapons.Headwphv);
								ImGui::Checkbox(("Aim at Neck"), &g_Options.LegitBot.FilterheavyWeapons.Neckwphv);
								ImGui::Checkbox(("Aim at Chest"), &g_Options.LegitBot.FilterheavyWeapons.Chestwphv);
								ImGui::Checkbox(("Aim at Stomach"), &g_Options.LegitBot.FilterheavyWeapons.Stomachwphv);
								ImGui::Checkbox(("Aim at MultiBone"), &g_Options.LegitBot.FilterheavyWeapons.multibonewphv);


							}ImGui::EndChild();
						}



					}
					if (pagesshit == 5)
					{
					}
					if (legitbotpages == 2)
					{

						ImGui::BeginChild("#triggerbot", ImVec2(320, 260), true);
						{
							//		btndefault();

							ImGui::Text(("Triggerbot"));
							ImGui::Separator();

							ImGui::PushItemWidth(200);
							ImGui::Checkbox(("Enable    "), &g_Options.LegitBot.Triggerbot.Enabled);


							ImGui::Hotkey("##triggerbotKey", &g_Options.LegitBot.Triggerbot.Key);
							ImGui::SameLine();
							ImGui::Text("Key");


							ImGui::Spacing();

							ImGui::SliderFloat(("Delay"), &g_Options.LegitBot.Triggerbot.Delay, 1.f, 150.f, "%.0f");

							ImGui::Spacing();

							ImGui::SliderFloat(("Hitchance"), &g_Options.LegitBot.Triggerbot.hitchance, 1.f, 100.f, "%.0f");

							ImGui::PopItemWidth();
						}ImGui::EndChild();


						ImGui::SameLine(0.0, 15.0f);

						ImGui::BeginChild("#triggerbot2", ImVec2(320, 260), true);
						{
							ImGui::Text(("Filter"));
							ImGui::Separator();

							ImGui::Checkbox(("Aim at Head"), &g_Options.LegitBot.Triggerbot.Filter.Head);
							ImGui::Checkbox(("Aim at Arms"), &g_Options.LegitBot.Triggerbot.Filter.Arms);
							ImGui::Checkbox(("Aim at Chest"), &g_Options.LegitBot.Triggerbot.Filter.Chest);
							ImGui::Checkbox(("Aim at Stomach"), &g_Options.LegitBot.Triggerbot.Filter.Stomach);
							ImGui::Checkbox(("Aim at Legs"), &g_Options.LegitBot.Triggerbot.Filter.Legs);
						} ImGui::EndChild();

					}

				}
				else if (tabselected == 3)
				{

					{
						ImGui::Text(" ");
						ImGui::Text(" ");
						ImGui::SameLine();
						ImGui::BeginChild("##esppage1", ImVec2(310, 545), true);
						{
							{
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("Player ESP");
								
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::SameLine(260);
								ImGui::Checkbox((" "), &g_Options.Visuals.Enabled);//
								ImGui::PushFont(fDefault);
								
							}
							ImGui::PushItemWidth(80);
							ImGui::Spacing();
							{
								ImGui::Separator();
								ImGui::Checkbox(("Players"), &g_Options.Visuals.Filter.Players);//
								ImGui::Checkbox(("Enemy Only"), &g_Options.Visuals.Filter.EnemyOnly);//
								ImGui::Separator();

								ImGui::Text("Toggle Key");
								ImGui::SameLine(160);
								ImGui::PushItemWidth(150);

								ImGui::Hotkey("##toggleket1", &g_Options.Visuals.ToggleKey); //
								ImGui::PushItemWidth(80);
								ImGui::Checkbox(("Box"), &g_Options.Visuals.Box);//
								if (g_Options.Visuals.Box)
								{
									ImGui::Separator();
									ImGui::Text(("box visible ct"));
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##1eravdcgrhtngfbdv"), (float*)&g_Options.Colors.boxvisiblect, 1 << 7);//
									ImGui::Text(("box visible t"));
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##2wevcwaefdcgads"), (float*)&g_Options.Colors.boxvisiblet, 1 << 7);//
									ImGui::Text(("box invisible ct"));
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##eravdcgrhtngfbdv"), (float*)&g_Options.Colors.box_color_ct, 1 << 7);//
									ImGui::Text(("box invisible t"));
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##wevcwaefdcgads"), (float*)&g_Options.Colors.box_color_t, 1 << 7);//
									ImGui::Separator();
									
								}
								ImGui::Checkbox(("Name"), &g_Options.Visuals.Name);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(XorStr("##bxcvhcmnb"), (float*)&g_Options.Colors.colornamexd, 1 << 7);//
								
								
								
								ImGui::Checkbox(("HP"), &g_Options.Visuals.HP);//
								if (g_Options.Visuals.HP)
								{
									ImGui::Separator();
									ImGui::MyColorEdit3(XorStr("Board HP"), (float*)&g_Options.Colors.healthboard, 1 << 7);//
									ImGui::MyColorEdit3(XorStr("HP Line"), (float*)&g_Options.Colors.healthpline, 1 << 7);//
									ImGui::Separator();
								}

								ImGui::Checkbox(("Skeleton"), &g_Options.Visuals.Skeleton);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(XorStr("##04593"), (float*)&g_Options.Colors.color_skeleton, 1 << 7);//
								ImGui::Checkbox("Glow", &g_Options.Visuals.Glow);//
								if (g_Options.Visuals.Glow)
								{
									ImGui::Separator();
									ImGui::Text(("glow visible t"));
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##35w4gercdwe"), (float*)&g_Options.Colors.EnemyGlow, 1 << 7);//
									ImGui::Text(("glow visible ct"));
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##11asdqedascsd"), (float*)&g_Options.Colors.TeamGlow, 1 << 7);//
									ImGui::SliderFloat(XorStr("Glow Alpha"), &g_Options.Colors.Glowalpha, 0.0f, 1.0f);//
									ImGui::Checkbox(("Weapon"), &g_Options.Visuals.Weapon);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##wreacesrsgdvc"), (float*)&g_Options.Colors.WeaponGlow, 1 << 7);//
									if (g_Options.Visuals.Weapon)
									{
										ImGui::SliderFloat(XorStr("Weapon Alpha"), &g_Options.Colors.Weaponalpha, 0.0f, 1.0f);//
									}
									ImGui::Separator();
								}
								ImGui::Checkbox(("Weapon"), &g_Options.Visuals.Weapon);
								ImGui::Checkbox(("Money"), &g_Options.Visuals.money);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(XorStr("##04593zxvrte"), (float*)&g_Options.Colors.moneycolorxd, 1 << 7);//


								ImGui::Checkbox(XorStr("Snap Line"), &g_Options.Visuals.Snaplines);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(XorStr("##44wgerccweacdsc"), (float*)&g_Options.Colors.snaplinexd, 1 << 7);//
								ImGui::Checkbox(("Defuser"), &g_Options.Visuals.HasDefuser);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(XorStr("##44sfgdsgfwgerccweacdsc"), (float*)&g_Options.Colors.defuserxd, 1 << 7);//
								ImGui::Checkbox(("Scoped"), &g_Options.Visuals.IsScoped);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(XorStr("##ggcvbacdsc"), (float*)&g_Options.Colors.skopedxd, 1 << 7);//
								ImGui::Checkbox(("Flashed"), &g_Options.Visuals.Flashed);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(XorStr("##ggcvbacdsc123r"), (float*)&g_Options.Colors.flashedxd, 1 << 7);//
								ImGui::Checkbox(("Hitmarker"), &g_Options.Misc.Hitmarker);//
								ImGui::SameLine(267);
								ImGui::MyColorEdit3(("##fd543asfxzc2"), g_Options.Colors.hitmark_color, 1 << 7);//
								ImGui::PushItemWidth(240);
								ImGui::Combo(XorStr(""), &g_Options.Misc.hitsound, hitsound, _ARRAYSIZE(hitsound));//
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Other ESP");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);

								}
								ImGui::PushItemWidth(80);
								ImGui::Separator();
								ImGui::Spacing();
								{

									ImGui::Checkbox(("Time"), &g_Options.Visuals.Time);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##ggcvbacdsc123rfdvxcb"), (float*)&g_Options.Colors.timexd, 1 << 7);//
									ImGui::Checkbox(("Spread Indicator"), &g_Options.Visuals.SpreadCrosshair);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##23452"), (float*)&g_Options.Colors.color_spread, 1 << 7);//
									ImGui::Checkbox(("Recoil Indicator"), &g_Options.Visuals.RecoilCrosshair);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##95340"), (float*)&g_Options.Colors.color_recoil, 1 << 7);//
									ImGui::Checkbox(("Dynamic Light"), &g_Options.Visuals.DLight);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##63456edsce"), (float*)&g_Options.Colors.dlight_color, 1 << 7);//
									ImGui::Checkbox(("GrenadeESP"), &g_Options.Visuals.GrenadeESP);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##63456edxcvxcsce"), (float*)&g_Options.Colors.frenadexd, 1 << 7);//
									ImGui::Checkbox(("Grenade Prediction"), &g_Options.Visuals.GrenadePrediction);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##1234rewrvds56474574532"), (float*)&g_Options.Colors.tracer_color, 1 << 7);//
									ImGui::Checkbox(("Dropped weapons"), &g_Options.Visuals.WeaponsWorld);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##1234rewrvds"), (float*)&g_Options.Colors.dropwepnamexd, 1 << 7);//
									ImGui::Checkbox(("C4 World"), &g_Options.Visuals.C4World);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##1234rewzxczxcvfbrvds"), (float*)&g_Options.Colors.bombxd, 1 << 7);//
									ImGui::Checkbox(("Sky color changer"), &g_Options.Visuals.skycolorchanger);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##1234567zxcva"), (float*)&g_Options.Colors.colorchanger, 1 << 7);//
									ImGui::Checkbox(("World color changer"), &g_Options.Visuals.worldcolorchanger);//
									ImGui::SameLine(267);
									ImGui::MyColorEdit3(XorStr("##1234567rt3wqef"), (float*)&g_Options.Colors.wcolorchanger, 1 << 7);//
								}ImGui::EndChild();

								ImGui::SameLine();
								ImGui::Text(" ");
								ImGui::SameLine();

								ImGui::BeginChild("##esppage2", ImVec2(310, 545), true);
								{
									{
										ImGui::PushFont(fDefault);
										ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
										ImGui::Text("HVH ESP");
										ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
										ImGui::PushFont(fDefault);
									}
									ImGui::Separator();
									ImGui::Spacing();
									{
										ImGui::Checkbox(XorStr("Angle Lines"), &g_Options.Visuals.angleLines);//
										ImGui::Checkbox(XorStr("LBY Indicator"), &g_Options.Visuals.LBYIndicator);//
										ImGui::Checkbox(XorStr("Thirdperson"), &g_Options.Visuals.ThirdPerson);//
										if (g_Options.Visuals.ThirdPerson)
									{
										
										ImGui::SameLine(160);
										ImGui::PushItemWidth(150);
										ImGui::Hotkey(XorStr("     "), &g_Options.Visuals.TPKey);//
										ImGui::PushItemWidth(240);
										ImGui::Combo("", &g_Options.Visuals.antiaim_thirdperson_angle, AntiAimThirdperson, ARRAYSIZE(AntiAimThirdperson));//
										ImGui::SliderFloat(XorStr("##tpdist"), &g_Options.Visuals.tpdist, 50.f, 450.f, "%.0f");//
									}
										//ImGui::Checkbox(XorStr("r_showenvcubemap"), &g_Options.Visuals.r_showenvcubemap);
										ImGui::Separator();
										{
											ImGui::PushFont(fDefault);
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
											ImGui::Text("Chams / Models"); 
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
											ImGui::PushItemWidth(80);

											ImGui::SameLine(); ImGui::Combo("##sdgl", &g_Options.Visuals.matierial, "Normal\0\rFlat\0\rWireframe\0\rGlass\0\rGold\0\rPlatinum\0\rPlastic\0\rCrystal\0\0", -1);//
											ImGui::PushItemWidth(120);

											ImGui::PushFont(fDefault);

										}
										ImGui::Separator();
										ImGui::SliderInt(XorStr("Chams Alpha"), &g_Options.Visuals.champlayeralpha, 0, 100);//

										ImGui::Checkbox(XorStr("Chams Players"), &g_Options.Visuals.Chams);//
										ImGui::SameLine(267); 
										ImGui::MyColorEdit3("##42ESP", g_Options.Colors.EnemyChamsVis, 1 << 7);//
										ImGui::Checkbox(XorStr("Chams Team"), &g_Options.Visuals.Teamchams);
										ImGui::SameLine(267);
										ImGui::MyColorEdit3("##E772SP", g_Options.Colors.TeamChamsVis, 1 << 7);//
										ImGui::Checkbox(("Chams HP Based"), &g_Options.Visuals.chamsHp);//
										ImGui::Checkbox(XorStr("Chams Invisible"), &g_Options.Visuals.XQZ);//
										ImGui::SameLine(267);
										ImGui::MyColorEdit3("##43ESP", g_Options.Colors.EnemyChamsNVis, 1 << 7);//
										ImGui::Combo(XorStr("Hands"), &g_Options.Visuals.Hands, handd, ARRAYSIZE(handd));//
										ImGui::SameLine(267);
										ImGui::MyColorEdit3(("##47ESP"), g_Options.Colors.hands_color, 1 << 7);//
										ImGui::SliderFloat(XorStr("Hands Alpha"), &g_Options.Colors.hands_alpha, 0.f, 1.f);//
										ImGui::Separator();
										{
											ImGui::PushFont(fDefault);
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
											ImGui::Text("Effects");
											ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
											ImGui::PushFont(fDefault);

										}
										ImGui::Separator();
										ImGui::Checkbox(("Disable Crosshair"), &g_Options.Visuals.InverseView);//
										ImGui::Combo(("Smoke"), &g_Options.Visuals.Smoke, smoke, ARRAYSIZE(smoke));
										ImGui::Checkbox(("No Visual Recoil"), &g_Options.Visuals.NoVisualRecoil);//
										ImGui::Checkbox(("No Flash"), &g_Options.Visuals.NoFlash);//
										ImGui::Checkbox(("Remove scope"), &g_Options.Visuals.noscopeborder);//

										ImGui::Checkbox(("Gravity Ragdoll"), &g_Options.Misc.Ragdoll);//
										ImGui::Checkbox(("Night Mode"), &g_Options.Misc.nightMode);//
										ImGui::Checkbox("Sniper Crosshair", &g_Options.Visuals.SniperCrosshair);//
										ImGui::Checkbox(("Disable post processing"), &g_Options.Misc.fps);//
										ImGui::Text("Skybox");
										ImGui::Combo(("     "), &g_Options.Visuals.SkyboxChanger, skyboxchanger, ARRAYSIZE(skyboxchanger));//
										ImGui::PushItemWidth(180);
										ImGui::SliderFloat(("##wallalpha"), &g_Options.Visuals.wallalpha, 0.f, 1.f, "Wall Alpha: %.2f"); ImGui::SameLine(); ImGui::Text("Wall Alpha");//
										ImGui::SliderFloat(("##proppalpha"), &g_Options.Visuals.propalpha, 0.f, 1.f, "Prop Alpha: %.2f");  ImGui::SameLine(); ImGui::Text("Prop Alpha ");//
										ImGui::SliderFloat(("##modelpalpha"), &g_Options.Visuals.modelalpha, 0.f, 1.f, "Model Alpha: %.2f");  ImGui::SameLine(); ImGui::Text("Model Alpha");//
										ImGui::SliderFloat(("View model FOV"), &g_Options.Visuals.viewmodelChanger, 0.f, 120.f, "%.0f");//

										ImGui::Checkbox(XorStr("Override Scope FOV"), &g_Options.Visuals.OverrideScopedFOV);

										ImGui::SliderFloat(("FOV"), &g_Options.Visuals.FOVChanger, 0.f, 60.f, "%.0f");//
									}ImGui::EndChild();
								}
							}
						}
					}
				}
				else if (tabselected == 4)
				{
					{
						ImGui::Text(" ");
						ImGui::Text(" ");
						ImGui::SameLine();
						ImGui::BeginChild("##miscpage1", ImVec2(310, 545), true);
						{
							{
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("Miscellaneous");
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
							}
							ImGui::Separator();

							ImGui::Spacing();
							{
								ImGui::Checkbox("Anti-Untrusted", &g_Options.Misc.antiuntrusted);//
								ImGui::Checkbox(("Watermark"), &g_Options.Misc.Watermark);

								ImGui::Checkbox("Fakelag", &g_Options.Misc.fakelag);//
								if (g_Options.Misc.fakelag)
								{
									ImGui::SliderInt("Fakelag Choke", &g_Options.Misc.fakelagchoke, 0, 20, "%.0f");//
									ImGui::SliderInt("Fakelag Send", &g_Options.Misc.fakelagsend, 0, 20, "%.0f");//
								}
								ImGui::Checkbox("Inventory Always On", &g_Options.Misc.inventoryalwayson);//
								ImGui::Checkbox("Left-Hand on Knife", &g_Options.Misc.LeftHand1);

								ImGui::Checkbox("Show enemies purchases", &g_Options.Misc.bought);//
								ImGui::Checkbox("mat_showmiplevels", &g_Options.Misc.crack);//
						//		ImGui::Checkbox("Fake Ping", &g_Options.Misc.FakePing);//
								ImGui::Checkbox("mat_drawgray", &g_Options.Misc.Gray);//
								ImGui::Checkbox("Replay", &g_Options.Misc.isReplaying);//
								ImGui::Checkbox("Spectator List", &g_Options.Misc.SpecList);//
								ImGui::Checkbox(("Damage Log [console]"), &g_Options.Misc.eventlogs);//
								ImGui::Checkbox(("Buy Log [console]"), &g_Options.Misc.eventlogs2);//
								ImGui::Checkbox("Rank Revealer", &g_Options.Misc.ServerRankRevealAll);//
								ImGui::Checkbox(("Auto-Accept"), &g_Options.Misc.AutoAccept);//
								ImGui::Checkbox("Moonwalk", &g_Options.Misc.moonwalk);//
								if (ImGui::Checkbox(("AFK Bot"), &g_Options.Misc.afkbot))//
								{
									if (g_Options.Misc.afkbot)
									{

										g_Engine->ClientCmd_Unrestricted("+forward;+moveleft;+left");

									}
									else if (!g_Options.Misc.afkbot)
									{

										g_Engine->ClientCmd_Unrestricted("-forward;-moveleft;-left");
									}
								}
								if (ImGui::Checkbox(("MM Map Prediction"), &g_Options.Misc.mmprediciton))//
									if (g_Options.Misc.mmprediciton)
									{

										g_Engine->ClientCmd_Unrestricted("developer 1");
										g_Engine->ClientCmd_Unrestricted("con_filter_enable 2");
										g_Engine->ClientCmd_Unrestricted("con_filter_text_out game:mapgroupname");
										g_Engine->ClientCmd_Unrestricted("con_filter_text game:map");
									}
									else if (!g_Options.Misc.mmprediciton)
									{
										g_Engine->ClientCmd_Unrestricted("developer 0");
										g_Engine->ClientCmd_Unrestricted("con_filter_enable 0");
									}
								ImGui::Checkbox("WalkBot", &g_Options.Misc.WalkBot);//
								ImGui::Checkbox("No Name", &g_Options.Misc.NameHide);//
								ImGui::Checkbox("Crash Pasta", &g_Options.Misc.crashpaste);//
								ImGui::Checkbox("Bunnyhop", &g_Options.Misc.bunnyhop);//
								if (g_Options.Misc.bunnyhop)
								{
									ImGui::Checkbox("AutoStrafe", &g_Options.Misc.autostrafenew);//
									if (g_Options.Misc.autostrafenew)
										g_Options.Misc.rageautostrafe = false;
									ImGui::Checkbox("RageAutoStrafe[bad]", &g_Options.Misc.rageautostrafe);//
									if (g_Options.Misc.rageautostrafe)
										g_Options.Misc.autostrafenew = false;
								}
								ImGui::Checkbox("Freecam", &g_Options.Misc.bFreeCam);//
								ImGui::SameLine(160);
								ImGui::PushItemWidth(150);
								ImGui::Hotkey("##freecamkey", &g_Options.Misc.iFreeCamKey);//
								ImGui::PushItemWidth(146);
								ImGui::SliderFloat("Speed", &g_Options.Misc.flFreeCamSpeed, 5, 25, "%.1f", (1.0F));//
							}
							

						}ImGui::EndChild();

						ImGui::SameLine();
						ImGui::Text(" ");
						ImGui::SameLine();

						ImGui::BeginChild("##miscpage2", ImVec2(310, 545), true);
						{


							{
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("Spammers");
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
							}
							ImGui::Separator();
							ImGui::Spacing();
							{
								ImGui::PushItemWidth(130);
								ImGui::Checkbox("Name Stealer", &g_Options.Misc.silentstealer);//
								ImGui::SameLine();
								if (ImGui::Button("Silent exploit"))
								{
									static ConVar* name = g_CVar->FindVar("name");
									if (name)
									{
										*(int*)((DWORD)&name->fnChangeCallback + 0xC) = NULL;
										name->SetValue("\n\xAD\xAD\xAD");
									}
								}


								ImGui::Checkbox("NameSpammer", &g_Options.Misc.namespam);//
								ImGui::Text(("ClanTags"));
								ImGui::PushItemWidth(146);

								ImGui::Combo(("##2"), &g_Options.Misc.clantag_SLN, clantags, _ARRAYSIZE(clantags));//


								ImGui::Text(("Custom Name"));
								ImGui::PushItemWidth(150);
								static char buf1[128] = ""; ImGui::InputText(("##Name"), buf1, 128, ImGuiInputTextFlags_AllowTabInput);
								ImGui::SameLine();
								ImGui::PushItemWidth(50);
								if (ImGui::Button(("Change Name")))
								{
									ConVar* name = g_CVar->FindVar("name");
									if (name)
									{
										*(int*)((DWORD)&name->fnChangeCallback + 0xC) = NULL;
										name->SetValue(buf1);
									}
								}
								ImGui::Text(("Custom ClanTag"));
								ImGui::PushItemWidth(150);
								static char buf2[128] = ""; ImGui::InputText(("##Clantag"), buf2, 16, ImGuiInputTextFlags_AllowTabInput);
								ImGui::SameLine();
								ImGui::PushItemWidth(50);

								if (ImGui::Button(("Change Clantag")))
								{
									dankestSetClanTag(buf2, buf2);
								}
								ImGui::Text(("Animated ClanTag"));
								ImGui::PushItemWidth(150);
								ImGui::InputText(("##AnimatedClantag"), G::AnimatedClantag, 16, ImGuiInputTextFlags_AllowTabInput);
								ImGui::SameLine();
								ImGui::PushItemWidth(50);
								if (ImGui::Button(("Start")))
								{
									g_Options.Misc.animatedclantag = true;
								}
								ImGui::SameLine();
								ImGui::PushItemWidth(85);
								if (ImGui::Button(("Stop")))
								{
									g_Options.Misc.animatedclantag = false;
								}

								ImGui::Combo(("ChatSpammer"), &g_Options.Misc.spammer, spammers, ARRAYSIZE(spammers));//
								ImGui::Separator();
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("Models changer"); 
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
								ImGui::Separator();
									ImGui::Text("Knfie Model"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##stopuse"), &g_Options.Modelchanger.knife_model, knifemodels, ARRAYSIZE(knifemodels));//
									ImGui::Text("Player Models"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##stopuse22"), &g_Options.Modelchanger.playermodels, playermodels, ARRAYSIZE(playermodels));//
									ImGui::Text("Awp Models"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##stopuse2234"), &g_Options.Modelchanger.awpmodels, awpmodels, ARRAYSIZE(awpmodels));//
									ImGui::Text("Ak-47"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##afdsvcwdsfcawsdc"), &g_Options.Modelchanger.ak47, ak47new, ARRAYSIZE(ak47new));//
									ImGui::Text("M4A1 Models"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##stopuse223456"), &g_Options.Modelchanger.m4a1models, m4a1models, ARRAYSIZE(m4a1models));//
									ImGui::Text("Hegrenade"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##stopuse223456234"), &g_Options.Modelchanger.granademodels, granademodels, ARRAYSIZE(granademodels));//
									ImGui::Text("Molotov other"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##s456234"), &g_Options.Modelchanger.incgrenademodels, incgrenademodels, ARRAYSIZE(incgrenademodels));//
									ImGui::Text("Mp7 Models"); ImGui::SameLine(); ImGui::PushItemWidth(150);
									ImGui::Combo(("##rf456234"), &g_Options.Modelchanger.mp7models, mp7models, ARRAYSIZE(mp7models));//	
							}ImGui::EndChild();
						}
					}
				}
				else if (tabselected == 5)
				{

					{
						ImGui::Text(" ");
						ImGui::Text(" ");
						ImGui::SameLine();
						ImGui::BeginChild("##skinpage1", ImVec2(310, 545), true);
						{ImGui::PushFont(fDefault);
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
							ImGui::Text("General");
							{
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
							}
							ImGui::SameLine();
							if (ImGui::Button(("Force Update")))
							{
								KnifeApplyCallbk();
							}
							ImGui::SameLine(260);

							ImGui::Checkbox(("##enables312"), &g_Options.Skinchanger.Enabled);

							ImGui::Separator();
							ImGui::Spacing();
							{

								ImGui::Combo(("Knife Model"), &g_Options.Skinchanger.Knife, knives, ARRAYSIZE(knives));
								ImGui::Combo(("Knife Skin"), &g_Options.Skinchanger.KnifeSkin, knifeskins, ARRAYSIZE(knifeskins));
								ImGui::Combo(("Gloves"), &g_Options.Skinchanger.gloves, gloves, _ARRAYSIZE(gloves));


								ImGui::Columns(1);
								ImGui::Separator();
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("Rifles");
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
								ImGui::Columns(2, NULL, false);
								ImGui::Combo(("AK-47"), &g_Options.Skinchanger.AK47Skin, ak47, ARRAYSIZE(ak47));
								ImGui::Combo(("M4A1-S"), &g_Options.Skinchanger.M4A1SSkin, m4a1s, ARRAYSIZE(m4a1s));
								ImGui::Combo(("M4A4"), &g_Options.Skinchanger.M4A4Skin, m4a4, ARRAYSIZE(m4a4));
								ImGui::Combo(("Galil AR"), &g_Options.Skinchanger.GalilSkin, galil, ARRAYSIZE(galil));
								ImGui::NextColumn();
								ImGui::Combo(("AUG"), &g_Options.Skinchanger.AUGSkin, aug, ARRAYSIZE(aug));
								ImGui::Combo(("FAMAS"), &g_Options.Skinchanger.FAMASSkin, famas, ARRAYSIZE(famas));
								ImGui::Combo(("SG553"), &g_Options.Skinchanger.Sg553Skin, sg553, ARRAYSIZE(sg553));

								ImGui::Columns(1);
								ImGui::Separator();
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("Snipers");
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
								ImGui::Columns(2, NULL, false);
								ImGui::Combo(("AWP"), &g_Options.Skinchanger.AWPSkin, awp, ARRAYSIZE(awp));
								ImGui::Combo(("SSG08"), &g_Options.Skinchanger.SSG08Skin, ssg08, ARRAYSIZE(ssg08));
								ImGui::NextColumn();
								ImGui::Combo(("SCAR"), &g_Options.Skinchanger.SCAR20Skin, scar20, ARRAYSIZE(scar20));
								ImGui::Combo(("G3SG1"), &g_Options.Skinchanger.G3sg1Skin, g3sg1, ARRAYSIZE(g3sg1));

								ImGui::Columns(1);
								ImGui::Separator();
								ImGui::PushFont(fDefault);
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
								ImGui::Text("SMG's");
								ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
								ImGui::PushFont(fDefault);
								ImGui::Columns(2, NULL, false);
								ImGui::Combo(("P90"), &g_Options.Skinchanger.P90Skin, p90, ARRAYSIZE(p90));
								ImGui::Combo(("MP7"), &g_Options.Skinchanger.Mp7Skin, mp7, ARRAYSIZE(mp7));
								ImGui::Combo(("MP9"), &g_Options.Skinchanger.Mp9Skin, mp9, ARRAYSIZE(mp9));
								ImGui::NextColumn();
								ImGui::Combo(("UMP45"), &g_Options.Skinchanger.UMP45Skin, ump45, ARRAYSIZE(ump45));
								ImGui::Combo(("MAC10"), &g_Options.Skinchanger.Mac10Skin, mac10, ARRAYSIZE(mac10));
								ImGui::Combo(("Bizon"), &g_Options.Skinchanger.BizonSkin, bizon, ARRAYSIZE(bizon));





							}ImGui::EndChild();

							ImGui::SameLine();
							ImGui::Text(" ");
							ImGui::SameLine();

							ImGui::BeginChild("##skinpage2", ImVec2(310, 545), true);
							{
								//  btndefault();
								{
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Pistols");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
								}
								ImGui::Spacing();
								{
									ImGui::Columns(1);

									ImGui::Columns(2, NULL, false);
									ImGui::Combo(("Glock"), &g_Options.Skinchanger.GlockSkin, glock, ARRAYSIZE(glock));
									ImGui::Combo(("USP-S"), &g_Options.Skinchanger.USPSkin, usp, ARRAYSIZE(usp));
									ImGui::Combo(("Deagle"), &g_Options.Skinchanger.DeagleSkin, deagle, ARRAYSIZE(deagle));
									ImGui::Combo(("Five-Seven"), &g_Options.Skinchanger.FiveSkin, five, ARRAYSIZE(five));
									ImGui::Combo(("R8"), &g_Options.Skinchanger.RevolverSkin, revolver, ARRAYSIZE(revolver));
									ImGui::NextColumn();
									ImGui::Combo(("TEC-9"), &g_Options.Skinchanger.tec9Skin, tec9, ARRAYSIZE(tec9));
									ImGui::Combo(("P2000"), &g_Options.Skinchanger.P2000Skin, p2000, ARRAYSIZE(p2000));
									ImGui::Combo(("P250"), &g_Options.Skinchanger.P250Skin, p250, ARRAYSIZE(p250));
									ImGui::Combo(("Dual-Barettas"), &g_Options.Skinchanger.DualSkin, dual, ARRAYSIZE(dual));
									ImGui::Combo(("CZ75"), &g_Options.Skinchanger.Cz75Skin, cz75, ARRAYSIZE(cz75));

									ImGui::Columns(1);
									ImGui::Separator();
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Shotguns");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
									ImGui::Columns(2, NULL, false);
									ImGui::Combo(("Nova"), &g_Options.Skinchanger.NovaSkin, nova, ARRAYSIZE(nova));
									ImGui::Combo(("SawedOff"), &g_Options.Skinchanger.SawedSkin, sawed, ARRAYSIZE(sawed));

									ImGui::NextColumn();
									ImGui::Combo(("Mag7"), &g_Options.Skinchanger.MagSkin, mag, ARRAYSIZE(mag));
									ImGui::Combo(("XM1014"), &g_Options.Skinchanger.XmSkin, xm, ARRAYSIZE(xm));

									ImGui::Columns(1);
									ImGui::Separator();
									ImGui::PushFont(fDefault);
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
									ImGui::Text("Machine Guns");
									ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
									ImGui::PushFont(fDefault);
									ImGui::Columns(2, NULL, false);
									ImGui::Combo(("Negev"), &g_Options.Skinchanger.NegevSkin, negev, ARRAYSIZE(negev));
									ImGui::NextColumn();
									ImGui::Combo(("M249"), &g_Options.Skinchanger.M249Skin, m249, ARRAYSIZE(m249));
								
									

								}

							}
						}ImGui::EndChild();
					}
				}
				else if (tabselected == 6)
				{


					using namespace ImGui;

					ImGui::Spacing();
					static int selectedcfg = 0;
					static std::string cfgname = "default";
					{
						ImGui::Columns(2, 0, false);

						GetConfigMassive();
						ImGui::BeginChild(1, ImVec2(-1, 200), true, ImGuiWindowFlags_AlwaysAutoResize);
						ImGui::Separator();
						ImGui::Spacing();
						if (ImGui::Combo("File", &selectedcfg, [](void* data, int idx, const char** out_text)
						{
							*out_text = configs[idx].c_str();
							return true;
						}, nullptr, configs.size(), 10))
						{
							cfgname = configs[selectedcfg];
							cfgname.erase(cfgname.length() - 4, 4);
							strcpy(g_Options.ConfigName, cfgname.c_str());
						}

						static bool create;
						if (ImGui::Button("Save", ImVec2(150, 50))) Config->Save(); 	ImGui::SameLine();
						if (ImGui::Button("Load", ImVec2(144, 50)))
						{
							Config->Load();
							FullUpdate();
						}

						if (ImGui::Button("New Config", ImVec2(-1, 23))) create = !create;
						if (ImGui::Button("Reset Settings", ImVec2(-1, 23))) Config->Setup();

						if (create)
						{
							ImGui::Begin("##Create", &create, ImVec2(200, 300), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
							ImGui::Text("Config Name:");
							ImGui::InputText("Name", g_Options.ConfigName, 52);
							if (ImGui::Button("Create and save"))
							{
								Config->Save();
								create = !create;
							}
							if (ImGui::Button("Cancel"))
								create = !create;
							ImGui::End();
						}
					}
				}
				else if (tabselected == 8)
				{
					ImGui::Checkbox(XorStr("Masterlooser GUI"), &g_Options.Visuals.Warnings);
					ImGui::Checkbox(XorStr("Legit AA"), &g_Options.Ragebot.LegitAA);
					ImGui::Combo(XorStr("Type"), &g_Options.Ragebot.LegitAAReal, LegitAACombo, ARRAYSIZE(LegitAACombo));

				}
				else if (tabselected == 0)
				{

					ImGui::BeginChild("Changelog", ImVec2(660, 575), true);
					{
						ImGui::GetStyle().Colors[ImGuiCol_Button] = ImColor(27, 40, 56, 255);
						ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImColor(49, 61, 75, 255);
						ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImColor(72, 82, 95, 255);
						if (ImGui::Button(("Website")))
						{
							ShellExecute(NULL, "open", "www.tragic.pw", NULL, NULL, SW_SHOWNORMAL);
						}
						ImGui::SameLine();
						if (ImGui::Button(("Contact")))
						{
							ShellExecute(NULL, "open", "www.steamcommunity.com/id/familypet", NULL, NULL, SW_SHOWNORMAL);
						}
						ImGui::SameLine();

						if (ImGui::Button(("Steam Group")))
						{
							ShellExecute(NULL, "open", "www.steamcommunity.com/groups/tragicpw", NULL, NULL, SW_SHOWNORMAL);
						}
						ImGui::SameLine();
						ImGui::GetStyle().Colors[ImGuiCol_Button] = ImColor(114, 137, 218, 255);
						ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImColor(128, 148, 221, 255);
						ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImColor(142, 160, 225, 255);
						if (ImGui::Button(("Discord")))
						{
							ShellExecute(NULL, "open", "www.discord.me/familypet", NULL, NULL, SW_SHOWNORMAL);
						}
						ImGui::Text(XorStr("Current version: "));
						ImGui::SameLine();
						{
							ImGui::PushFont(fDefault);
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(253, 246, 146, 255));
							ImGui::Text("4.0");
							ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255));
							ImGui::PushFont(fDefault);
						}

						ImGui::Text(XorStr("Build date: 12/07/2018"));
						ImGui::Text(XorStr("Changelog:"));
						ImGui::Text(XorStr("   - Update date: 24/03/2018"));
						ImGui::Text(XorStr("		+ Skeet menu bar"));
						ImGui::Text(XorStr("		+ Fixed triggerbot from crashing"));
						ImGui::Text(XorStr("		+ Fixed aimbot"));
						ImGui::Text(XorStr("		+ New menu colours"));
						ImGui::Text(XorStr("		+ Added Legit AA - adaptive sideways, backwards"));
						ImGui::Text(XorStr("		+ Added SMG sub catergory in legitbot"));
						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("	- Update date: 27/03/2018"));
						ImGui::Text(XorStr("		+ Icons added"));
						ImGui::Text(XorStr("		+ Added FPS counter"));
						ImGui::Text(XorStr("		+ Cleaned up clantags, added dropboxes"));
						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("	- Update date: 30/03/2018"));
						ImGui::Text(XorStr("		+ Added scout and deagle sub category in legitbot"));
						ImGui::Text(XorStr("		+ Ragebot recode"));
						ImGui::Text(XorStr("		+ Added Snaplines"));

						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("	- Update date: 01/04/2018"));
						ImGui::Text(XorStr("		+ Updated indexes which were causing crashes"));
						ImGui::Text(XorStr("		+ Fixed chams"));
						ImGui::Text(XorStr("		+ Temporarily removed GrenadeESP which was known to cause crashes"));

						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("	- Update date: 02/04/2018"));
						ImGui::Text(XorStr("		+ New menu"));
						ImGui::Text(XorStr("		+ Fixed GrenadeESP from causing crashes"));
						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("	- Update date: 03/04/2018"));
						ImGui::Text(XorStr("		+ Fixed up ragebot (P)"));
						ImGui::Text(XorStr("		+ Added Anti-Untrusted"));
						ImGui::Text(XorStr("		+ Menu tweaks (Changed icons, properly aligned config buttons, tragic.pw header)"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 04/04/2018"));
						ImGui::Text(XorStr("		+ Added 'home' tab"));
						ImGui::Text(XorStr("		+ Added legit bhop"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 05/04/2018"));
						ImGui::Text(XorStr("		+ Added purchase info listener"));
						ImGui::Text(XorStr("		+ Added damage info listener"));
						ImGui::Text(XorStr("		+ Added smoke check (legitbot)"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 13/04/2018"));
						ImGui::Text(XorStr("		+ Added thirdperson real and fake angles"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 22/04/2018"));
						ImGui::Text(XorStr("		+ Legitbot, visuals and misc tab redesign"));
						ImGui::Text(XorStr("		+ Added custom bone hitboxes (Legitbot)"));
						ImGui::Text(XorStr("		+ Fixed silent name exploit"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 23/04/2018"));
						ImGui::Text(XorStr("		+ Fixed white bodies death ragdoll"));
						ImGui::Text(XorStr("		+ Added AFK Bot"));
						ImGui::Text(XorStr("		+ Added matchmaking map prediction"));
						ImGui::Text(XorStr("		+ Added health based chams"));
						ImGui::Text(XorStr("		+ Added fixed event logs causing extreme lag"));
						ImGui::Text(XorStr("		+ Added freecam"));
						ImGui::Text(XorStr("		- Removed fake ping"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 24/04/2018"));
						ImGui::Text(XorStr("		+ Fixed scout not autoshooting in ragebot"));
						ImGui::Text(XorStr("		+ Fixed scout scopeborder"));
						ImGui::Text(XorStr("		+ Fixed MM map prediction"));
						ImGui::Text(XorStr("		+ Ragebot tab redesign"));

						ImGui::Text(XorStr(""));



						ImGui::Text(XorStr("	- Update date: 01/05/2018"));
						ImGui::Text(XorStr("		+ Added animated rainbow watermark"));
						ImGui::Text(XorStr("		+ Fixed delayed hitmarker sound"));
						ImGui::Text(XorStr("		+ Added matchmaking map prediction"));
						ImGui::Text(XorStr("		+ Added tooltips when hovering over a button"));

						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("	- Update date: 02/05/2018"));
						ImGui::Text(XorStr("		+ Added night mode"));

						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 13/05/2018"));
						ImGui::Text(XorStr("		+ Added ragdoll gravity"));
						ImGui::Text(XorStr("		+ Added wall, prop and momdel opacity changer"));
						ImGui::Text(XorStr("		+ Added wireframe smoke option"));
						ImGui::Text(XorStr("		+ Added anti-aim angles line"));
						ImGui::Text(XorStr("		+ Added LBY Indicator"));
						ImGui::Text(XorStr("		+ Added Visuals toggle key"));
						ImGui::Text(XorStr("		+ Fixed ragebot position adjustment (backtracking)"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 14/05/2018"));
						ImGui::Text(XorStr("		+ Added faked synced ping; extends backtracking"));
						ImGui::Text(XorStr("		+ Added sniper crosshair"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 19/05/2018"));
						ImGui::Text(XorStr("		+ Added new config system, ability to name and create as many configs as you want"));
						ImGui::Text(XorStr("		+ Added ESP preview"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 20/05/2018"));
						ImGui::Text(XorStr("		+ Added weapon icons"));
						ImGui::Text(XorStr("		+ Changed style of sliders"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 23/05/2018"));
						ImGui::Text(XorStr("		+ New ragebot features: Hitscan, hitbox"));
						ImGui::Text(XorStr("		+ Added pSilent for Legitbot"));
						ImGui::Text(XorStr(""));

						ImGui::Text(XorStr("	- Update date: 12/06/2018"));
						ImGui::Text(XorStr("		+ Now uses undetected vmt hooking function"));
						ImGui::Text(XorStr("		+ Fixed smoke check"));
						ImGui::Text(XorStr("		+ Updated for panorama "));
						ImGui::Text(XorStr("		+ Override scope FOV"));
						ImGui::Text(XorStr(		"- Experimental Features:"));
						ImGui::Text(XorStr("		+ Masterlooser GUI (WIP)"));
						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("	- Credits to Azu for the following"));
						ImGui::Text(XorStr("		+ Fixes with CFG saving issues"));
						ImGui::Text(XorStr("		+ Colour changers for: Glow, Money, Snaplines, IsScoped, IsFlashed, Time, Spread, Recoil, Grenade ESP, Dropped weapons, C4 World"));
						ImGui::Text(XorStr("		+ Sky Colour Changer"));
						ImGui::Text(XorStr("		+ World Colour Changer"));
						ImGui::Text(XorStr("		+ Disable Crosshair"));

						ImGui::Text(XorStr("		+ Fakelag"));
							
						ImGui::Text(XorStr("		+ Moonwalk"));
						ImGui::Text(XorStr("		+ WalkBot"));
						ImGui::Text(XorStr("		+ Bhop Auto Strafe fix"));
						ImGui::Text(XorStr("		+ Models Changer (?)"));
						ImGui::Text(XorStr("		+ Replay"));
						ImGui::Text(XorStr("		+ No name	"));
						ImGui::Text(XorStr("		+ Auto Zeus"));
						ImGui::Text(XorStr("		+ More Resolvers and pitch adjustment for no-spread"));
						ImGui::Text(XorStr(""));
						ImGui::Text(XorStr("		Notes:"));
						ImGui::Text(XorStr("		The following features are broken due to the panorama update: "));
						ImGui::Text(XorStr("		- Skinchanger broken"));
						ImGui::Text(XorStr("		- Grenade ESP causes crashes"));


						// yea ye
					}

				}	


				ImGui::EndChild();
			}



		}ImGui::End();
	}
}