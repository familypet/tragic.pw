#pragma once
#include "ImGui\imgui.h"
#include "Logging.h"

namespace G // Global Stuff
{
	extern bool			Aimbotting;
	extern bool			InAntiAim;
	extern bool			Return;
	extern HMODULE		Dll;
	extern HWND			Window;
	extern bool			PressedKeys[256];
	extern bool			d3dinit;
	extern float		FOV;
	extern int			ChamMode;
	extern char			AnimatedClantag[16];
	extern bool			SendPacket;
	extern int			BestTarget;
	extern char			buf3[128];
	extern char			extra_flags;
	extern bool			menuon;

	using msg_t = void(__cdecl*)(const char*, ...);
	extern msg_t		Msg;
	extern WNDPROC  OldWindow;
	extern float		hitmarkeralpha;

}