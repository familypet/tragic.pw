/*#pragma once

#include <Windows.h>
#pragma comment(lib, "Winmm.lib") 

#define g_GameEventManager g_EventManager
#define EVENT_DEBUG_ID_INIT 0x2A;

class hitmarker
{
	class player_hurt_listener
		: public IGameEventListener2
	{
	public:
		void start()
		{
			if (!g_GameEventManager->AddListener(this, "player_hurt", false)) {
				throw std::exception("Failed to register the event");
			}
		}
		void stop()
		{
			g_GameEventManager->RemoveListener(this);
		}
		void FireGameEvent(IGameEvent *event) override
		{
			hitmarker::singleton()->on_fire_event(event);
		}
		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT /*0x2A*/;
		/*}
	};
public:
	static hitmarker* singleton()
	{
		static hitmarker* instance = new hitmarker;
		return instance;
	}

	void initialize()
	{
		_listener.start();
	}

	void on_fire_event(IGameEvent* event)
	{

		if (!strcmp(event->GetName(), "player_hurt"))
		{
			int attacker = event->GetInt("attacker");
			if (g_Engine->GetPlayerForUserID(attacker) == g_Engine->GetLocalPlayer())
			{
				if (g_Options.Misc.Hitmarker)
				_flHurtTime = g_Globals->curtime;
			}
		}
	}

	void on_paint()
	{
		auto curtime = g_Globals->curtime;
		auto lineSize = 8;
		if (_flHurtTime + 0.25f >= curtime) {
			int screenSizeX, screenCenterX;
			int screenSizeY, screenCenterY;
			g_Engine->GetScreenSize(screenSizeX, screenSizeY);

			screenCenterX = screenSizeX / 2;
			screenCenterY = screenSizeY / 2;

			if (g_Options.Misc.HitMarkerColor == 1)
			{
				g_Surface->DrawSetColor(255, 0, 0, 255);

			}

			if (g_Options.Misc.HitMarkerColor == 2)
			{
				g_Surface->DrawSetColor(200, 200, 200, 255);

			}

			if (g_Options.Misc.HitMarkerColor == 3)
			{
				g_Surface->DrawSetColor(0, 0, 255, 255);

			}

			if (g_Options.Misc.HitMarkerColor == 4)
			{
				g_Surface->DrawSetColor(255, 0, 255, 255);

			}

			if (g_Options.Misc.HitMarkerColor == 5)
			{
				g_Surface->DrawSetColor(255, 255, 0, 255);

			}

			if (g_Options.Misc.HitMarkerColor == 6)
			{
				g_Surface->DrawSetColor(255, 128, 0, 255);

			}


			g_Surface->DrawLine(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4));
			g_Surface->DrawLine(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4));
			g_Surface->DrawLine(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4));
			g_Surface->DrawLine(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4));

		}

	}

private:
	player_hurt_listener    _listener;
	float                   _flHurtTime;
};
*/