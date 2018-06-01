#include "Misc.h"
#include "Interfaces.h"
#include "Render.h"
#include <time.h>
#include "BaseClient.h"


std::string animatedclantag;
int iLastTime;
bool bDone=false;




void misc::OnCreateMove(CInput::CUserCmd *cmd, C_BaseEntity *local)
{
	if (g_Options.Misc.Bhop == 0)
	{
	}
	else if (g_Options.Misc.Bhop == 1 && local->IsAlive())
	{
		if (cmd->buttons & IN_JUMP && !(local->GetMoveType() & MOVETYPE_LADDER))
		{

			int iFlags = local->GetFlags();
			if (!(iFlags & FL_ONGROUND))
				cmd->buttons &= ~IN_JUMP;

		}

	}
	else if (g_Options.Misc.Bhop == 2 && local->IsAlive())
	{
		if (cmd->buttons & IN_JUMP && !(local->GetMoveType() & MOVETYPE_LADDER))
		{

			int iFlags = local->GetFlags();
			BYTE MoveType = local->GetMoveType();
			if (MoveType == MOVETYPE_NOCLIP || MoveType == MOVETYPE_LADDER)
				return;

			int STATE_DUCK = iFlags & FL_DUCKING;

			int State = cmd->buttons;


			switch (cmd->buttons)
			{
			case (1 << 3):
				if (!(iFlags & FL_ONGROUND) && !(cmd->buttons & IN_JUMP))
				{
					cmd->buttons |= IN_JUMP;

					iFlags &= FL_DUCKING;
				}

				break;
			}

			switch (iFlags)
			{
			case (1 << 1):
				if (iFlags & FL_ONGROUND)
					cmd->buttons & IN_FORWARD;
				else
					cmd->buttons |= IN_DUCK;

				break;
			}


			if (!(iFlags & FL_ONGROUND) && local->GetVelocity().Length2D() > 0.f)
				cmd->buttons &= ~IN_JUMP;

		}

	}
	if (g_Options.Misc.FakePing2)
	{
		{
			ConVar* net_fakelag = g_CVar->FindVar("net_fakelag");
			if (g_Options.Misc.FakePing);
			net_fakelag->SetValue(g_Options.Misc.FakePing2);
		}
	}
	else
	{
		{
			ConVar* net_fakelag = g_CVar->FindVar("net_fakelag");
			if (g_Options.Misc.FakePing);
			net_fakelag->SetValue(g_Options.Misc.FakePing2);
		}
	}
	if (g_Options.Misc.Ragdoll)
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* pastergay = g_CVar->FindVar("cl_ragdoll_gravity");
			if (pastergay)
			{
				pastergay->SetValue(0);
			}
		}
	}
	else
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* pastergay = g_CVar->FindVar("cl_ragdoll_gravity");
			if (pastergay)
			{
				pastergay->SetValue(600);
			}
		}
	}
	if (g_Options.Misc.Skybox) { ConVar* skybox = g_CVar->FindVar("sv_skyname"); if (skybox) skybox->SetValue(skybox_models[g_Options.Misc.Skybox]); }

	if (g_Options.Misc.fps)
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("mat_postprocess_enable");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(0);
			}
		}
	}
	else
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("mat_postprocess_enable");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(1);
			}
		}
	}
	if (g_Options.Misc.Gray)
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("mat_drawgray");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(1);
			}
		}
	}
	else
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("mat_drawgray");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(0);
			}
		}
	}
	if (g_Options.Misc.crack)
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("mat_showmiplevels");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(1);
			}
		}
	}
	else
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("mat_showmiplevels");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(0);
			}
		}
	}
	//r_showenvcubemap
	if (g_Options.Misc.chromatic)
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("r_showenvcubemap");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(1);
			}
		}
	}
	else
	{
		if (g_Engine->IsConnected() && g_Engine->IsInGame())
		{
			ConVar* DrawOtherModels = g_CVar->FindVar("r_showenvcubemap");
			if (DrawOtherModels)
			{
				DrawOtherModels->SetValue(0);
			}
		}
	}


	if (g_Options.Misc.heartclantag)
	{
		{
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)local->GetTickBase() * g_Globals->interval_per_tick * 2.5;

			if (counter % 48 == 0)
				motion++;
			int value = ServerTime % 4;
			switch (value) {
			case 0: setclantag(XorStr("s		s					s					")); break; // work in in progress



			}
			counter++;
		}

		iLastTime = int(g_Globals->curtime);
	}
	if (!g_Options.Misc.animatedclantag && animatedclantag.c_str() != G::AnimatedClantag)
	{
		animatedclantag = G::AnimatedClantag;
	}
	if (g_Options.Misc.animatedclantag && animatedclantag.length() > 1)
	{
		if (int(g_Globals->curtime) != iLastTime)
		{
			auto length = animatedclantag.length();
			animatedclantag.insert(0, 1, animatedclantag[length - 2]);
			animatedclantag.erase(length - 1, 1);

			setclantag(animatedclantag.c_str());
			iLastTime = int(g_Globals->curtime);
		}
	}

	if (g_Options.Misc.LeftHand1) {
		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->GetActiveWeaponHandle());
		if (MiscFunctions::IsKnife(pWeapon))
			g_Engine->ClientCmd("cl_righthand 0");
		else
			g_Engine->ClientCmd("cl_righthand 1");
	}




	if (g_Options.Misc.clantag_SLN == 1)
	{
		{
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)local->GetTickBase() * g_Globals->interval_per_tick * 3.4;

			if (counter % 48 == 0)
				motion++;
			int value = ServerTime % 31;
			switch (value) {

			case 0: setclantag(XorStr("              t")); break;
			case 1: setclantag(XorStr("             tr")); break;
			case 2: setclantag(XorStr("            tra")); break;
			case 3: setclantag(XorStr("           trag")); break;
			case 4: setclantag(XorStr("          tragi")); break;
			case 5: setclantag(XorStr("         tragic")); break;
			case 6: setclantag(XorStr("        tragic.")); break;
			case 7: setclantag(XorStr("       tragic.p")); break;
			case 8: setclantag(XorStr("      tragic.pw")); break;
			case 9: setclantag(XorStr("     tragic.pw ")); break;
			case 10: setclantag(XorStr("     tragic.pw ")); break;
			case 11: setclantag(XorStr("    tragic.pw  ")); break;
			case 12: setclantag(XorStr("    tragic.pw  ")); break;
			case 13: setclantag(XorStr("   tragic.pw   ")); break;
			case 14: setclantag(XorStr("   tragic.pw   ")); break;
			case 15: setclantag(XorStr("  tragic.pw    ")); break;
			case 16: setclantag(XorStr("  tragic.pw    ")); break;
			case 17: setclantag(XorStr(" tragic.pw     ")); break;
			case 18: setclantag(XorStr(" tragic.pw     ")); break;
			case 19: setclantag(XorStr("tragic.pw      ")); break;
			case 20: setclantag(XorStr("tragic.pw      ")); break;
			case 21: setclantag(XorStr("ragic.pw       ")); break;
			case 22: setclantag(XorStr("agic.pw        ")); break;
			case 23: setclantag(XorStr("gic.pw          ")); break;
			case 24: setclantag(XorStr("ic.pw           ")); break;
			case 25: setclantag(XorStr("c.pw            ")); break;
			case 26: setclantag(XorStr(".pw             ")); break;
			case 27: setclantag(XorStr("pw              ")); break;
			case 28: setclantag(XorStr("w               ")); break;
			case 29: setclantag(XorStr("               ")); break;

			}
			counter++;
		}

		iLastTime = int(g_Globals->curtime);
	}
	if (!g_Options.Misc.animatedclantag && animatedclantag.c_str() != G::AnimatedClantag)
	{
		animatedclantag = G::AnimatedClantag;
	}
	if (g_Options.Misc.animatedclantag && animatedclantag.length() > 1)
	{
		if (int(g_Globals->curtime) != iLastTime)
		{
			auto length = animatedclantag.length();
			animatedclantag.insert(0, 1, animatedclantag[length - 2]);
			animatedclantag.erase(length - 1, 1);

			setclantag(animatedclantag.c_str());
			iLastTime = int(g_Globals->curtime);
		}
	}
	if (g_Options.Misc.clantag_SLN == 2)
	{
		{
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)local->GetTickBase() * g_Globals->interval_per_tick * 1;

			if (counter % 48 == 0)
				motion++;
			int value = ServerTime % 7;
			switch (value) {
			case 0: setclantag(XorStr("tragic								")); break; // why is this in caps muy ugly
			case 1: setclantag(XorStr("		tragic						")); break; //js i added XorStr because it encrypts.. if you hxd edit
			case 2: setclantag(XorStr("			tragic					")); break; //this now you cannot see this tag and just edit it..
			case 3: setclantag(XorStr("				tragic				")); break;
			case 4: setclantag(XorStr("					tragic			")); break;
			case 5: setclantag(XorStr("							tragic	")); break;
			case 6: setclantag(XorStr("								tragic")); break;



			}
			counter++;
		}

		iLastTime = int(g_Globals->curtime);
	}
	if (!g_Options.Misc.animatedclantag && animatedclantag.c_str() != G::AnimatedClantag)
	{
		animatedclantag = G::AnimatedClantag;
	}
	if (g_Options.Misc.animatedclantag && animatedclantag.length() > 1)
	{
		if (int(g_Globals->curtime) != iLastTime)
		{
			auto length = animatedclantag.length();
			animatedclantag.insert(0, 1, animatedclantag[length - 2]);
			animatedclantag.erase(length - 1, 1);

			setclantag(animatedclantag.c_str());
			iLastTime = int(g_Globals->curtime);
		}
	}
	if (g_Options.Misc.clantag_SLN == 3)
	{
		{
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)local->GetTickBase() * g_Globals->interval_per_tick * 4;

			if (counter % 48 == 0)
				motion++;
			int value = ServerTime % 35;
			switch (value) {

			case 0: setclantag(XorStr("              g")); break;
			case 1: setclantag(XorStr("             ga")); break;
			case 2: setclantag(XorStr("            gam")); break;
			case 3: setclantag(XorStr("           game")); break;
			case 4: setclantag(XorStr("          games")); break;
			case 5: setclantag(XorStr("         gamese")); break;
			case 6: setclantag(XorStr("        gamesen")); break;
			case 7: setclantag(XorStr("       gamesens")); break;
			case 8: setclantag(XorStr("      gamesense")); break;
			case 9: setclantag(XorStr("     gamesense ")); break;
			case 10: setclantag(XorStr("     gamesense ")); break;
			case 11: setclantag(XorStr("    gamesense  ")); break;
			case 12: setclantag(XorStr("    gamesense  ")); break;
			case 13: setclantag(XorStr("   gamesense   ")); break;
			case 14: setclantag(XorStr("   gamesense   ")); break;
			case 15: setclantag(XorStr("  gamesense    ")); break;
			case 16: setclantag(XorStr("  gamesense    ")); break;
			case 17: setclantag(XorStr("  gamesense    ")); break;
			case 18: setclantag(XorStr("  gamesense    ")); break;


			case 19: setclantag(XorStr("  gamesense    ")); break;
			case 20: setclantag(XorStr("  gamesense    ")); break;
			case 21: setclantag(XorStr("  gamesense    ")); break;
			case 22: setclantag(XorStr("  gamesense    ")); break;

			case 23: setclantag(XorStr(" gamesense     ")); break;
			case 24: setclantag(XorStr(" gamesense     ")); break;
			case 25: setclantag(XorStr("gamesense      ")); break;
			case 26: setclantag(XorStr("gamesense      ")); break;
			case 27: setclantag(XorStr("amesense       ")); break;
			case 28: setclantag(XorStr("mesense        ")); break;
			case 29: setclantag(XorStr("sense          ")); break;
			case 30: setclantag(XorStr("ense           ")); break;
			case 31: setclantag(XorStr("nse            ")); break;
			case 32: setclantag(XorStr("se             ")); break;
			case 33: setclantag(XorStr("e              ")); break;
			case 34: setclantag(XorStr("               ")); break;

			}
			counter++;
		}

		iLastTime = int(g_Globals->curtime);
	}
	if (!g_Options.Misc.animatedclantag && animatedclantag.c_str() != G::AnimatedClantag)
	{
		animatedclantag = G::AnimatedClantag;
	}
	if (g_Options.Misc.animatedclantag && animatedclantag.length() > 1)
	{
		if (int(g_Globals->curtime) != iLastTime)
		{
			auto length = animatedclantag.length();
			animatedclantag.insert(0, 1, animatedclantag[length - 2]);
			animatedclantag.erase(length - 1, 1);

			setclantag(animatedclantag.c_str());
			iLastTime = int(g_Globals->curtime);
		}
	}
    if (g_Options.Misc.namespam)
    {
        char name[151];
        char fucked_char = static_cast<char>(-1);
        for (auto i = 0; i <= 150; i++)
            name[i] = fucked_char;

        const char nick[12] = "tragic";
        memcpy(name, nick, 11);

        SetName(name);
    }
    static size_t lastTime = 0;
    static int counter = 0;
    if (GetTickCount() > lastTime)
    {

        counter = counter + 1;
        if (counter > 5)
            counter = 0;
        switch (g_Options.Misc.spammer)
        {
		case 1:
			SayInChat(phooklist[counter]);
			break;
		case 2:
			SayInChat(tuxlist[counter]);
			break;
		case 3:
			SayInChat(ezfragslist[counter]);
			break;
		case 4:
			SayInChat(hooktroniclist[counter]);
			break;
		case 5:
			SayInChat(aimwarelist[counter]);
			break;
		case 6:
			SayInChat(huecheatslist[counter]);
			break;
        }

        lastTime = GetTickCount() + 850;


    }
    if (g_Options.Misc.AutoStrafe)
        AutoStrafe(cmd, local);

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->GetActiveWeaponHandle());

	static bool WasFiring = false;
	if (pWeapon != nullptr)
	{
		if (MiscFunctions::IsPistol(pWeapon) && g_Options.LegitBot.AutoPistol && pWeapon->m_AttributeManager()->m_Item()->GetItemDefinitionIndex() != 64)
		{
			if (cmd->buttons & IN_ATTACK && !MiscFunctions::IsKnife(pWeapon) && !MiscFunctions::IsGrenade(pWeapon))
			{
				if (WasFiring)
				{
					cmd->buttons &= ~IN_ATTACK;
				}
			}

			WasFiring = cmd->buttons & IN_ATTACK ? true : false;
		}
	}
	
    if(g_Options.Misc.silentstealer)
    {
        bool bDidMeme = false;
        int iNameIndex = -1;
        char chName[130];
        static ConVar* name = g_CVar->FindVar("name");
        {
            for (int iPlayerIndex = 0; iPlayerIndex < g_Engine->GetMaxClients(); iPlayerIndex++)
            {
                C_BaseEntity *pEntity = g_EntityList->GetClientEntity(iPlayerIndex);
                if (!pEntity || pEntity == local || iPlayerIndex == g_Engine->GetLocalPlayer())
                    continue;
                if (rand() % 3 == 1)
                {
                    iNameIndex = pEntity->GetIndex();
                    bDidMeme = true;
                }
            }
            if (bDidMeme)
            {
                player_info_t pInfo;
                g_Engine->GetPlayerInfo(iNameIndex, &pInfo);
                sprintf(chName, "%s ", pInfo.name);
                name->SetValue(chName);
            }
        }

    }
}

void misc::logsrun()
{

	static auto developer = g_CVar->FindVar("developer");
	developer->SetValue(1);
	static auto con_filter_text = g_CVar->FindVar("con_filter_text");
	static auto con_filter_text_out = g_CVar->FindVar("con_filter_text_out");
	static auto con_filter_enable = g_CVar->FindVar("con_filter_enable");

	con_filter_text->SetValue("hit ");
	con_filter_text_out->SetValue("");
	con_filter_enable->SetValue(2);
}
void misc::FakeWalk(CInput::CUserCmd* pCmd, bool &bSendPacket, C_BaseEntity *local)
{

	if (g_Options.Misc.fakewalk)
	{
		int FakeWalkKey = g_Options.Misc.fakewalkkey;
		if (FakeWalkKey > 0 && G::PressedKeys[g_Options.Misc.fakewalkkey])
		{
			static int chocking = -1;
			chocking++;

			if (chocking < 3)
			{
				bSendPacket = false;
				pCmd->tick_count += 10;
				pCmd += 7 + pCmd->tick_count % 2 ? 0 : 1;
				pCmd->buttons |= local->GetMoveType() == IN_BACK;
				pCmd->forwardmove = pCmd->sidemove = 0.f;
			}
			else
			{
				bSendPacket = true;
				chocking = -1;
				g_Globals->frametime *= (local->GetVelocity().Length2D()) / 1.f;
				pCmd->buttons |= local->GetMoveType() == IN_FORWARD;
			}
		}
	}
}


void misc::AutoStrafe(CInput::CUserCmd *cmd, C_BaseEntity *local)
{

    QAngle oldangles; g_Engine->GetViewAngles(oldangles);
    static float move = 450;
    float s_move = move * 0.5065f;
    if (local->GetMoveType() & (MOVETYPE_NOCLIP | MOVETYPE_LADDER))
        return;
    if (cmd->buttons & (IN_FORWARD | IN_MOVERIGHT | IN_MOVELEFT | IN_BACK))
        return;

    if (cmd->buttons & IN_JUMP || !(local->GetFlags() & FL_ONGROUND))
    {
        cmd->forwardmove = move * 0.015f;
        cmd->sidemove += (float)(((cmd->tick_count % 2) * 2) - 1) * s_move;

        if (cmd->mousedx)
            cmd->sidemove = (float)clamp(cmd->mousedx, -1, 1) * s_move;

        static float strafe = cmd->viewangles.y;

        float rt = cmd->viewangles.y, rotation;
        rotation = strafe - rt;

        if (rotation < FloatNegate(g_Globals->interval_per_tick))
            cmd->sidemove = -s_move;

        if (rotation > g_Globals->interval_per_tick)
            cmd->sidemove = s_move;

        strafe = rt;
    }

	movementfix(oldangles, cmd);
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class fomopoz {
public:
	string iamtwbmn;
	double prwxrgxmuaqjey;
	string jrxptlzux;
	int fxrmoqjoczjnl;
	double jsumrilksbwssyw;
	fomopoz();
	bool sygclizjkykcqhzwqfr(double rqlojyorulwlig, double zdwyxsmgavhfbvp, double wvowqs, double sajvfmtb, bool irhomvwzw, string fuwsknt, bool hwswwco, string citxrv);
	void fhnpymiaskby(double ybnjhdj, int ukvqlmezforgyt, double qlnsxdllfcx, string stwtblvlth, double vjzxennt, string psgkivex, int mpyjdwclobazmh, bool lodwfqsze);
	void fstsohkutiejijqprd(int ccgoxlojrquw, double zurqwedpknlc, string foiea, string lcugvh, int kcmrhcsxidjvwgn, int yllcfisd, string teroazdsltylcyj, string yzgse);
	bool ucpkvscexbuimstrwsloapw(double wwvwjdvky, string ffduarfelsocnve, bool mylbyr, string qrbduumg, bool eahzuceeqpi, bool evwtzmilwxj, string rcnqn, int cpjaz, bool lqivbg, bool ltjjzr);
	int rejlsatlatmlpwxytzmazbqwl(double fkecfurwm, double nuagyzaxkuuusa, string mfqgxjwwykflbjb, double yhezdx, string bfmthg, double jysqyeytlcljy, bool ukfidupoxupgw, double wzkarypjannfppv, bool dpzicvjtc, int kibkvxiwopwn);
	bool tejtotryucfqccw(double xzdqswhi, bool itbamobmf, double byaqbnn, string kfyjqx, string iuwcrweunwvd, double qtlrnllrtbw);
	int pdisbfnfgfpimndb(bool cujkye, double jtaqxjc, int nihmptxioiehyd, bool kokgfgblb, string kablb, int elowblbpq, double qvbpfqesoiznuzo, int ecafvwiwabfi, bool zeosttz, bool jieqrhpgsgu);
	int sguoltfkxwhummkuctcjdpxc(bool dsqtsrowe);
	string unhmqvjeolrfublvvyleomrcl(double vppiciiahfvpfp);

protected:
	int gzrvsuzgqvug;
	string ihphsjaswlrux;
	string hdogmf;
	bool ddlnntxm;

	int ljgchytcbwsai(bool ibszkrkgddawkz, double hfiuaa, double uvwovoqwtbj);
	string sgrspuiekgjzvyleeimwnz(double fvzrphkoqd, int zrsdlfcprlm, bool aknvbqyegua, int onxaclewvrbb, string gblmbyl);
	string dhsiiotpwztdbnfbkinssvfu(bool vgiawbgzbufirx, double ycdkdlpcfzsh);
	void vlzzcsztfmbrkquunqtc(bool uaezrq, string kjajbihbze, int dhamsleginisy);
	bool ichlhajnxlpauxltzyizinov(int imxmnfsbduh);
	int vtnyfbrdoinzjd(double pffimikxjsa, int rdtdtoqsaw, string cfvrujxokkjbes, double mqxqvrplnmyu, string eyntzq, int hruaawpofzq, int tbcamnygfjm, int hifgiihbwmatqgz, double hqplmgsxfld, int wytqvcdqdde);

private:
	bool gmuinixyuaxwvl;
	double rrtassrgpiq;
	string rekblfiqxjbrt;

	bool ewqigpqbyhgwxbiq(int zksavmudbjdcjx, string vaivobiw, double zbytvuzp);
	double amdgysenlqmwkxqudfx(double nycbkfvl, double ogyjpcsoad, bool qbthczzdw, string yeathtnwywqpvvv, double dwmykhh);
	string csrifsabxjklzemshxbiv(int llklbwpcehjor);
	bool hzbitwwnbl(bool mhwgtzpfuzochx, string kjtpcxrpives, int pqapoyfvikavzoq, string keugoehhzab, int wxqqhanmsxzv, bool vkktdppluklrwld);
	int cezdxhjwysdwux(string afncsqe, bool gzkpqn);
	int qibdjjpzbp(int vxfwdktygh, double pghnha, bool utsnp, double tabgysszhsrjn, int lohxh, bool pkxneiqwvuxedl, string apprz, double zcscuaicp, double isvtorcdrflp, int vkgoimhce);
	int ocrduyvhirfgxpjpqglwlppwa(double qjzqymwehvbvjn, double omrxamiifpethk, string acegmvov, int dycio, double urwlbw, double fpaavygwwvvoics, double hkhqmgo, double pdudtzoiurbjyf);
	bool bxspmalphgimopjrbjzc(int rmaezbmhb, int aifgq, double wiuelqpjkkehhq, string eundnildji, string odlwkogkfa);

};


bool fomopoz::ewqigpqbyhgwxbiq(int zksavmudbjdcjx, string vaivobiw, double zbytvuzp) {
	string lsridkwfr = "uxnmuypmfmdlkqpdegefnovemzlebdfucrfwyswnqbnnqnqpjrcubovzxadgulmvtqtp";
	bool spqttyx = false;
	bool shjshpczeiie = false;
	double nscgontpxd = 5273;
	bool nplxsqvq = false;
	int psfvuq = 3871;
	int dvgdthjqmzxqlfw = 3228;
	bool oasryomwwkgead = true;
	if (false == false) {
		int lwmz;
		for (lwmz = 42; lwmz > 0; lwmz--) {
			continue;
		}
	}
	if (true == true) {
		int vbvxo;
		for (vbvxo = 49; vbvxo > 0; vbvxo--) {
			continue;
		}
	}
	if (string("uxnmuypmfmdlkqpdegefnovemzlebdfucrfwyswnqbnnqnqpjrcubovzxadgulmvtqtp") == string("uxnmuypmfmdlkqpdegefnovemzlebdfucrfwyswnqbnnqnqpjrcubovzxadgulmvtqtp")) {
		int ek;
		for (ek = 13; ek > 0; ek--) {
			continue;
		}
	}
	return true;
}

double fomopoz::amdgysenlqmwkxqudfx(double nycbkfvl, double ogyjpcsoad, bool qbthczzdw, string yeathtnwywqpvvv, double dwmykhh) {
	string zklfnjjws = "mkuokxrtpcxiedohgwzjuamtrudiryqxzcodciqhsbnm";
	bool cnrdxr = false;
	return 78178;
}

string fomopoz::csrifsabxjklzemshxbiv(int llklbwpcehjor) {
	bool yyfsnx = false;
	double ueqehepgzdvjjgj = 14697;
	bool hycqcrcwllhrcnd = true;
	double bjdqwhv = 15881;
	string ngbepfikrkb = "hwillxibhwzpzkpfkdzmwtgwaxqzqztjw";
	string enpmjge = "rtlydaflqeq";
	bool ayikzqjpf = false;
	string dfmckgsydxboy = "imvjqhrjwsizgl";
	return string("fabdetbexwiuovlicid");
}

bool fomopoz::hzbitwwnbl(bool mhwgtzpfuzochx, string kjtpcxrpives, int pqapoyfvikavzoq, string keugoehhzab, int wxqqhanmsxzv, bool vkktdppluklrwld) {
	return true;
}

int fomopoz::cezdxhjwysdwux(string afncsqe, bool gzkpqn) {
	int uddqjau = 82;
	double clqlcnckocc = 8987;
	double jphzblbkpbo = 5302;
	bool skpugbpnfiox = true;
	if (8987 != 8987) {
		int eaew;
		for (eaew = 89; eaew > 0; eaew--) {
			continue;
		}
	}
	if (8987 == 8987) {
		int pwi;
		for (pwi = 34; pwi > 0; pwi--) {
			continue;
		}
	}
	if (82 != 82) {
		int hzwrq;
		for (hzwrq = 64; hzwrq > 0; hzwrq--) {
			continue;
		}
	}
	if (82 != 82) {
		int om;
		for (om = 34; om > 0; om--) {
			continue;
		}
	}
	return 97567;
}

int fomopoz::qibdjjpzbp(int vxfwdktygh, double pghnha, bool utsnp, double tabgysszhsrjn, int lohxh, bool pkxneiqwvuxedl, string apprz, double zcscuaicp, double isvtorcdrflp, int vkgoimhce) {
	double iilomrzvaglg = 26921;
	bool jidshjgjv = true;
	bool vvjckvbdbwovd = false;
	string rtvlvbf = "awu";
	double fepcysg = 26423;
	bool nzzrqhrsnsdmm = false;
	string ifeaa = "joihtpncabgteqywwiwowwnbaqfmabgrrzchyjiszalygwhcfsimmadkkwcnhtmdjnhtclnztoqcwjsiypgvsdwjkzdeiyugrva";
	bool ramjdqwmmuwlz = false;
	if (26423 != 26423) {
		int darxqif;
		for (darxqif = 46; darxqif > 0; darxqif--) {
			continue;
		}
	}
	if (string("joihtpncabgteqywwiwowwnbaqfmabgrrzchyjiszalygwhcfsimmadkkwcnhtmdjnhtclnztoqcwjsiypgvsdwjkzdeiyugrva") != string("joihtpncabgteqywwiwowwnbaqfmabgrrzchyjiszalygwhcfsimmadkkwcnhtmdjnhtclnztoqcwjsiypgvsdwjkzdeiyugrva")) {
		int tntbwxpbbl;
		for (tntbwxpbbl = 3; tntbwxpbbl > 0; tntbwxpbbl--) {
			continue;
		}
	}
	if (string("awu") == string("awu")) {
		int bqmnkdvogq;
		for (bqmnkdvogq = 89; bqmnkdvogq > 0; bqmnkdvogq--) {
			continue;
		}
	}
	return 85958;
}

int fomopoz::ocrduyvhirfgxpjpqglwlppwa(double qjzqymwehvbvjn, double omrxamiifpethk, string acegmvov, int dycio, double urwlbw, double fpaavygwwvvoics, double hkhqmgo, double pdudtzoiurbjyf) {
	string jhflegjiqlnhvlx = "nyrpftcegdiekogujlbqzqeqsflckso";
	int eveisbzd = 4295;
	int ulktkscamhaf = 93;
	string bbahoihtrj = "ozghqmyvfdfiijqljlv";
	if (4295 != 4295) {
		int cbo;
		for (cbo = 25; cbo > 0; cbo--) {
			continue;
		}
	}
	if (4295 == 4295) {
		int qhlreep;
		for (qhlreep = 39; qhlreep > 0; qhlreep--) {
			continue;
		}
	}
	if (93 == 93) {
		int subostw;
		for (subostw = 96; subostw > 0; subostw--) {
			continue;
		}
	}
	if (4295 == 4295) {
		int hpswuqrz;
		for (hpswuqrz = 64; hpswuqrz > 0; hpswuqrz--) {
			continue;
		}
	}
	if (string("ozghqmyvfdfiijqljlv") == string("ozghqmyvfdfiijqljlv")) {
		int dwkcrt;
		for (dwkcrt = 98; dwkcrt > 0; dwkcrt--) {
			continue;
		}
	}
	return 76358;
}

bool fomopoz::bxspmalphgimopjrbjzc(int rmaezbmhb, int aifgq, double wiuelqpjkkehhq, string eundnildji, string odlwkogkfa) {
	bool mlnhhgwmyiwdi = false;
	if (false != false) {
		int rkafb;
		for (rkafb = 94; rkafb > 0; rkafb--) {
			continue;
		}
	}
	if (false != false) {
		int jlus;
		for (jlus = 29; jlus > 0; jlus--) {
			continue;
		}
	}
	if (false == false) {
		int ka;
		for (ka = 7; ka > 0; ka--) {
			continue;
		}
	}
	return false;
}

int fomopoz::ljgchytcbwsai(bool ibszkrkgddawkz, double hfiuaa, double uvwovoqwtbj) {
	int rasahzuxuucw = 1909;
	bool oviqrapweacj = false;
	string cagunpnove = "iqcqudemwmoxzistxvbeqokowminzyzhlaoyynwcbderpbxzjbdlnmybmmj";
	double zqihfy = 20220;
	bool vfdqdydmaevgku = false;
	double popimgeld = 2904;
	int xqaqnhis = 7355;
	if (7355 == 7355) {
		int sfku;
		for (sfku = 24; sfku > 0; sfku--) {
			continue;
		}
	}
	if (false == false) {
		int fcxqrdk;
		for (fcxqrdk = 82; fcxqrdk > 0; fcxqrdk--) {
			continue;
		}
	}
	if (false == false) {
		int ntlctd;
		for (ntlctd = 14; ntlctd > 0; ntlctd--) {
			continue;
		}
	}
	if (2904 != 2904) {
		int ymhjtze;
		for (ymhjtze = 73; ymhjtze > 0; ymhjtze--) {
			continue;
		}
	}
	if (false == false) {
		int obniipt;
		for (obniipt = 86; obniipt > 0; obniipt--) {
			continue;
		}
	}
	return 3641;
}

string fomopoz::sgrspuiekgjzvyleeimwnz(double fvzrphkoqd, int zrsdlfcprlm, bool aknvbqyegua, int onxaclewvrbb, string gblmbyl) {
	double vkicrhepbwvccxc = 1902;
	double ipwntcdkxerzls = 22130;
	string qpujik = "dfbbhkretaerr";
	return string("rkyagdjvvfoeiuygugr");
}

string fomopoz::dhsiiotpwztdbnfbkinssvfu(bool vgiawbgzbufirx, double ycdkdlpcfzsh) {
	int qhhxslbeusaq = 2724;
	bool oondnmekqzt = true;
	bool ddhpl = false;
	bool pkchbwnbt = false;
	double vdmzkdesvaxq = 18644;
	double urhoggzop = 916;
	if (false == false) {
		int wrbaom;
		for (wrbaom = 39; wrbaom > 0; wrbaom--) {
			continue;
		}
	}
	if (916 == 916) {
		int zvwprmlqxm;
		for (zvwprmlqxm = 4; zvwprmlqxm > 0; zvwprmlqxm--) {
			continue;
		}
	}
	if (916 != 916) {
		int czhsygpz;
		for (czhsygpz = 25; czhsygpz > 0; czhsygpz--) {
			continue;
		}
	}
	if (false != false) {
		int thjqrgqere;
		for (thjqrgqere = 69; thjqrgqere > 0; thjqrgqere--) {
			continue;
		}
	}
	if (true != true) {
		int knnbsdtr;
		for (knnbsdtr = 81; knnbsdtr > 0; knnbsdtr--) {
			continue;
		}
	}
	return string("vtkiksimfrezp");
}

void fomopoz::vlzzcsztfmbrkquunqtc(bool uaezrq, string kjajbihbze, int dhamsleginisy) {
	bool tmsulqcowgv = true;
	double hwssanalije = 33748;
	string zspiinoj = "udnayivwtufdbbuq";
	int mlucvsmakfog = 568;
	int ugrucyuqvjetap = 8224;
	bool wowgatj = false;
	if (33748 == 33748) {
		int wyzvyl;
		for (wyzvyl = 54; wyzvyl > 0; wyzvyl--) {
			continue;
		}
	}
	if (false != false) {
		int ufsyef;
		for (ufsyef = 100; ufsyef > 0; ufsyef--) {
			continue;
		}
	}
	if (false == false) {
		int uqttk;
		for (uqttk = 83; uqttk > 0; uqttk--) {
			continue;
		}
	}

}

bool fomopoz::ichlhajnxlpauxltzyizinov(int imxmnfsbduh) {
	int ngcjx = 6233;
	string ljmxhcyv = "ozahnpfs";
	if (6233 != 6233) {
		int qeuemyp;
		for (qeuemyp = 24; qeuemyp > 0; qeuemyp--) {
			continue;
		}
	}
	if (string("ozahnpfs") != string("ozahnpfs")) {
		int lx;
		for (lx = 94; lx > 0; lx--) {
			continue;
		}
	}
	if (string("ozahnpfs") == string("ozahnpfs")) {
		int hdkxaljhzu;
		for (hdkxaljhzu = 71; hdkxaljhzu > 0; hdkxaljhzu--) {
			continue;
		}
	}
	if (string("ozahnpfs") == string("ozahnpfs")) {
		int pygmbd;
		for (pygmbd = 7; pygmbd > 0; pygmbd--) {
			continue;
		}
	}
	return false;
}

int fomopoz::vtnyfbrdoinzjd(double pffimikxjsa, int rdtdtoqsaw, string cfvrujxokkjbes, double mqxqvrplnmyu, string eyntzq, int hruaawpofzq, int tbcamnygfjm, int hifgiihbwmatqgz, double hqplmgsxfld, int wytqvcdqdde) {
	int brxfi = 3092;
	int jqvnefqfajtruq = 1979;
	bool ohbpfzvmnu = false;
	int uqqtzjvxoa = 124;
	string rbjyzppr = "rluyapcpmvamjhtlkuuegbbnignzaknpllnyigtfhqyfgptlbgzpmooxkgdbreujylgjhrovfrvoij";
	string iagymo = "tmbjj";
	double dgvvnda = 13281;
	if (string("rluyapcpmvamjhtlkuuegbbnignzaknpllnyigtfhqyfgptlbgzpmooxkgdbreujylgjhrovfrvoij") == string("rluyapcpmvamjhtlkuuegbbnignzaknpllnyigtfhqyfgptlbgzpmooxkgdbreujylgjhrovfrvoij")) {
		int ipolk;
		for (ipolk = 1; ipolk > 0; ipolk--) {
			continue;
		}
	}
	if (124 == 124) {
		int wozbpfwu;
		for (wozbpfwu = 40; wozbpfwu > 0; wozbpfwu--) {
			continue;
		}
	}
	if (3092 != 3092) {
		int tebm;
		for (tebm = 85; tebm > 0; tebm--) {
			continue;
		}
	}
	if (124 != 124) {
		int mrbpy;
		for (mrbpy = 43; mrbpy > 0; mrbpy--) {
			continue;
		}
	}
	return 26861;
}

bool fomopoz::sygclizjkykcqhzwqfr(double rqlojyorulwlig, double zdwyxsmgavhfbvp, double wvowqs, double sajvfmtb, bool irhomvwzw, string fuwsknt, bool hwswwco, string citxrv) {
	bool hwrfu = true;
	string ruxfqdy = "mghbxljswotfivmsdvkskmlpfbrctixjqxvjpmjdnghzahcupntsuvuwxzomgprqkg";
	double mzvkntzhijb = 24055;
	bool iqlwxqfbltv = true;
	if (24055 == 24055) {
		int kuc;
		for (kuc = 9; kuc > 0; kuc--) {
			continue;
		}
	}
	if (string("mghbxljswotfivmsdvkskmlpfbrctixjqxvjpmjdnghzahcupntsuvuwxzomgprqkg") == string("mghbxljswotfivmsdvkskmlpfbrctixjqxvjpmjdnghzahcupntsuvuwxzomgprqkg")) {
		int dpbuab;
		for (dpbuab = 8; dpbuab > 0; dpbuab--) {
			continue;
		}
	}
	if (true != true) {
		int okddwmu;
		for (okddwmu = 59; okddwmu > 0; okddwmu--) {
			continue;
		}
	}
	if (string("mghbxljswotfivmsdvkskmlpfbrctixjqxvjpmjdnghzahcupntsuvuwxzomgprqkg") == string("mghbxljswotfivmsdvkskmlpfbrctixjqxvjpmjdnghzahcupntsuvuwxzomgprqkg")) {
		int mkwmuyiqe;
		for (mkwmuyiqe = 6; mkwmuyiqe > 0; mkwmuyiqe--) {
			continue;
		}
	}
	return false;
}

void fomopoz::fhnpymiaskby(double ybnjhdj, int ukvqlmezforgyt, double qlnsxdllfcx, string stwtblvlth, double vjzxennt, string psgkivex, int mpyjdwclobazmh, bool lodwfqsze) {
	string ygqdtmgjnmdx = "cmlhqzjmivpxvkwrvzstdzbxhtlnubshjjjpgvjhnqushwignhmtbllhcmthhcsmfrowqtolddxqfoxvlntrp";
	double lympxsanegepal = 16347;
	double vhasnsfan = 21325;
	string csufizkjbwi = "syhnfopjwbuzzeruqofwhbuduyonhprnssockibqarxsycgkomrhsigbxptfqpiircoyktjksyyncikzfmwbgtdhhqryuzcumm";
	double xbwklvsf = 88286;
	double cnxixoywnpuk = 19857;
	double yhackgt = 10915;
	int eeeifcswl = 3703;
	double gdbcttb = 17955;
	bool sibjjnsleoblxc = true;
	if (21325 == 21325) {
		int dbxdl;
		for (dbxdl = 95; dbxdl > 0; dbxdl--) {
			continue;
		}
	}
	if (3703 != 3703) {
		int ntfi;
		for (ntfi = 54; ntfi > 0; ntfi--) {
			continue;
		}
	}
	if (17955 == 17955) {
		int fz;
		for (fz = 47; fz > 0; fz--) {
			continue;
		}
	}
	if (16347 == 16347) {
		int mqqhy;
		for (mqqhy = 57; mqqhy > 0; mqqhy--) {
			continue;
		}
	}

}

void fomopoz::fstsohkutiejijqprd(int ccgoxlojrquw, double zurqwedpknlc, string foiea, string lcugvh, int kcmrhcsxidjvwgn, int yllcfisd, string teroazdsltylcyj, string yzgse) {
	string dumefsheoljmxo = "vwxfawlyrypgnvspfxlpnvrsrcsjifnfyinunvhmrblhbsbxlcdubqxs";
	int synqhsomj = 380;
	double qlvohodlet = 55073;
	bool moespfewmof = true;
	string oahow = "cqthtrbwvmzjojgpwtsdxtcfruxestgxnxdq";
	double rwmejazl = 3884;
	double kujnb = 72490;
	bool zaabpspurvuwsdz = true;
	double qhsyyswxrctwra = 35027;

}

bool fomopoz::ucpkvscexbuimstrwsloapw(double wwvwjdvky, string ffduarfelsocnve, bool mylbyr, string qrbduumg, bool eahzuceeqpi, bool evwtzmilwxj, string rcnqn, int cpjaz, bool lqivbg, bool ltjjzr) {
	string ujimqrhzpssmkp = "lpddpotlpwlfqrhmozanlwyrtifnmbhyoqkpcajnjldjsyawjpler";
	string lfoiirkmx = "cnqrsgqskllmnuadylxbric";
	string ftaxtehpipuce = "bpihjwvxwjuqoxacwuuwcilgbqqolzpvqgxrzvvacjlfkuvohj";
	bool ccjrrhl = false;
	return true;
}

int fomopoz::rejlsatlatmlpwxytzmazbqwl(double fkecfurwm, double nuagyzaxkuuusa, string mfqgxjwwykflbjb, double yhezdx, string bfmthg, double jysqyeytlcljy, bool ukfidupoxupgw, double wzkarypjannfppv, bool dpzicvjtc, int kibkvxiwopwn) {
	string xrxgcsvcy = "dtbghjpvaidfobbjjobzymrddazxvcyyyhgmvzgkeaqletzvcwzyoijuooffmooukswbjxyuwlbnypyjtykfrldhyfrnuya";
	int yndrn = 2947;
	int zfrvroyadlkj = 781;
	string ckboawotrkcnfbp = "swghajrrikiwjygqaylfqwdgnsjtdnirjlhimaaqrtnjfdqyuqvrbisfrlloqrpaphkseplkkbdsphgxspoxsxvycvqzxo";
	bool wboks = false;
	string sudufjvxvy = "vttzqapimjrlymxgxhadedcosajekyryi";
	string gdkaxixmy = "abeamxafgpvmixmkabiyockmepgzxzcttuzhdvypv";
	if (string("dtbghjpvaidfobbjjobzymrddazxvcyyyhgmvzgkeaqletzvcwzyoijuooffmooukswbjxyuwlbnypyjtykfrldhyfrnuya") == string("dtbghjpvaidfobbjjobzymrddazxvcyyyhgmvzgkeaqletzvcwzyoijuooffmooukswbjxyuwlbnypyjtykfrldhyfrnuya")) {
		int na;
		for (na = 48; na > 0; na--) {
			continue;
		}
	}
	if (string("vttzqapimjrlymxgxhadedcosajekyryi") != string("vttzqapimjrlymxgxhadedcosajekyryi")) {
		int fweksubcd;
		for (fweksubcd = 5; fweksubcd > 0; fweksubcd--) {
			continue;
		}
	}
	if (781 == 781) {
		int wmuzdxoe;
		for (wmuzdxoe = 8; wmuzdxoe > 0; wmuzdxoe--) {
			continue;
		}
	}
	if (781 != 781) {
		int esgkalyah;
		for (esgkalyah = 17; esgkalyah > 0; esgkalyah--) {
			continue;
		}
	}
	if (string("swghajrrikiwjygqaylfqwdgnsjtdnirjlhimaaqrtnjfdqyuqvrbisfrlloqrpaphkseplkkbdsphgxspoxsxvycvqzxo") == string("swghajrrikiwjygqaylfqwdgnsjtdnirjlhimaaqrtnjfdqyuqvrbisfrlloqrpaphkseplkkbdsphgxspoxsxvycvqzxo")) {
		int ejwv;
		for (ejwv = 31; ejwv > 0; ejwv--) {
			continue;
		}
	}
	return 87793;
}

bool fomopoz::tejtotryucfqccw(double xzdqswhi, bool itbamobmf, double byaqbnn, string kfyjqx, string iuwcrweunwvd, double qtlrnllrtbw) {
	string vkxsgq = "vjfijjcxoiqulihcgvcejyocvdxrsxnilgdeewvbjhymdxx";
	int zpfozdtuwflf = 564;
	int nslffeahn = 3623;
	double wrewjo = 16790;
	int mepuovscfprtziw = 168;
	bool ynesfnhwwkpey = true;
	string wizozbonpjheu = "sfltibrkhzfxdwtcrapihakrinwtutaojqetvfzxvudahvfruqkqpivtdeosgontgilswdeqwbhovysrewlglcmnv";
	double tusxwbrgzstw = 5470;
	double dspvufkgipmp = 61914;
	if (168 != 168) {
		int shhubn;
		for (shhubn = 48; shhubn > 0; shhubn--) {
			continue;
		}
	}
	if (564 == 564) {
		int bcwtbm;
		for (bcwtbm = 19; bcwtbm > 0; bcwtbm--) {
			continue;
		}
	}
	return true;
}

int fomopoz::pdisbfnfgfpimndb(bool cujkye, double jtaqxjc, int nihmptxioiehyd, bool kokgfgblb, string kablb, int elowblbpq, double qvbpfqesoiznuzo, int ecafvwiwabfi, bool zeosttz, bool jieqrhpgsgu) {
	int qomjfklnaoxfhfe = 843;
	string ioxkidcdokim = "z";
	string qzviykwdytplhf = "ngulglewaahplbijkppcsidjglqjnvnlivdtkyrtrieltillgrsn";
	int slyeqaiiojmp = 1086;
	double fjxngheyksyxgt = 17993;
	if (843 != 843) {
		int iabarc;
		for (iabarc = 8; iabarc > 0; iabarc--) {
			continue;
		}
	}
	if (843 != 843) {
		int lo;
		for (lo = 79; lo > 0; lo--) {
			continue;
		}
	}
	return 25530;
}

int fomopoz::sguoltfkxwhummkuctcjdpxc(bool dsqtsrowe) {
	string dgddhljsf = "xjwiuz";
	string umytpjlxd = "hugzoamwlnokgrbxasmebqtoxzobsgjusfpjlecvajjnsiyafyudzaywusxtlnqevejipmypfsmszgccnqa";
	double xpdtqcabocukf = 21054;
	if (string("xjwiuz") != string("xjwiuz")) {
		int brwhcnbky;
		for (brwhcnbky = 59; brwhcnbky > 0; brwhcnbky--) {
			continue;
		}
	}
	if (string("xjwiuz") == string("xjwiuz")) {
		int nmzlrgqway;
		for (nmzlrgqway = 60; nmzlrgqway > 0; nmzlrgqway--) {
			continue;
		}
	}
	return 89983;
}

string fomopoz::unhmqvjeolrfublvvyleomrcl(double vppiciiahfvpfp) {
	string pivnoywksryovju = "gpipkgzgcxcgihrmfvbyogrtlngljhrpgpnxwdczuczbhyhccohrkuwlszg";
	string aerewg = "rpgbslkwkmnblqttyikeuimdjmpz";
	if (string("gpipkgzgcxcgihrmfvbyogrtlngljhrpgpnxwdczuczbhyhccohrkuwlszg") == string("gpipkgzgcxcgihrmfvbyogrtlngljhrpgpnxwdczuczbhyhccohrkuwlszg")) {
		int ndhxwnwysv;
		for (ndhxwnwysv = 1; ndhxwnwysv > 0; ndhxwnwysv--) {
			continue;
		}
	}
	if (string("gpipkgzgcxcgihrmfvbyogrtlngljhrpgpnxwdczuczbhyhccohrkuwlszg") != string("gpipkgzgcxcgihrmfvbyogrtlngljhrpgpnxwdczuczbhyhccohrkuwlszg")) {
		int hxaiajcjo;
		for (hxaiajcjo = 44; hxaiajcjo > 0; hxaiajcjo--) {
			continue;
		}
	}
	return string("vmlctmeagvqkp");
}

fomopoz::fomopoz() {
	this->sygclizjkykcqhzwqfr(32533, 16208, 3464, 23068, false, string("vuyxpiqqwkfptdicvbpvrljnvtlswtblzwjguo"), true, string("vlqxhuhmlpamaooqqfvkexqwjmytwlycxaqvpehdbdzdpefidhsjhhurmppesevizulpwjsctmsmbyyclwpvybuxulzpliodxhl"));
	this->fhnpymiaskby(4864, 1796, 91496, string("kfbcszbdztkkbpmkvcubwypjjjrswjgdqdifbdndguofkbwmb"), 31184, string("yxamvjoywfiocfpnukxtzivhsodytmtadcggoycchogpervestkxzjicjngflslvijggz"), 156, true);
	this->fstsohkutiejijqprd(2113, 37159, string("ejicnzbxonxdvtmdurhttxzzjkeeftxsuinnopzjsrrfsboslfzrzpw"), string("tpsqzjgqrcttdkjjlynhuwsgxdxnsgticnpetzdimclmsyvfpyckfydbofgvsktseswsxuykoqdpckascmvrhcchjhqvf"), 1855, 7046, string("qtfjmmjx"), string("hkczkkunbnfzujjmwmrxdxllxxcespnpedlmdclribgcypkaxmvpmcymzhlsc"));
	this->ucpkvscexbuimstrwsloapw(36121, string("flbncynkyhuaeouovdijonaeogreykgedtuafyzuhwroxjnptkgnefporkryitivveuvplsbgbghuvtexmmdqhukro"), false, string("pmfzaciboopuwerruv"), true, true, string("wxhhvdrkkreoknyiatimimqrqwobjfgiv"), 2885, true, false);
	this->rejlsatlatmlpwxytzmazbqwl(6110, 1762, string("jqpkumlonqphhbo"), 23, string("lylojjyi"), 13759, false, 53572, false, 3243);
	this->tejtotryucfqccw(41050, true, 13974, string("gifhihkajkyoqsjsrhzkapdknmvnozjztqybeje"), string("gw"), 6054);
	this->pdisbfnfgfpimndb(false, 3333, 2120, true, string("zyemojhjaairenppspkllflkfdqgrwyetxkfzofvwvovnzrdmwpudls"), 2414, 4318, 4590, true, false);
	this->sguoltfkxwhummkuctcjdpxc(true);
	this->unhmqvjeolrfublvvyleomrcl(41281);
	this->ljgchytcbwsai(false, 97264, 26594);
	this->sgrspuiekgjzvyleeimwnz(20605, 596, false, 4032, string("tsqpwaipbixhpyepidxxyafhw"));
	this->dhsiiotpwztdbnfbkinssvfu(false, 88847);
	this->vlzzcsztfmbrkquunqtc(false, string("qesjfysbujykucrvvrlmrgyrcwkmrzzjvludbblkpuzqnadwjjnnfvghzziuaheynwvgxjk"), 198);
	this->ichlhajnxlpauxltzyizinov(716);
	this->vtnyfbrdoinzjd(21250, 6728, string("zbozcxwbbocm"), 7354, string("fpmpnpvrqbqigafheqfvxdnrmjqggylanzqvbakluexsvbvxpvusbuhvorixzthc"), 1088, 2094, 407, 34058, 2111);
	this->ewqigpqbyhgwxbiq(2475, string("qtibrjzdlcyvwvoyonekrkuakffvvrtuqlkooqhazyixypzqqjksvyaxtsjjqtb"), 45213);
	this->amdgysenlqmwkxqudfx(31344, 7603, true, string("cueajwpklszyxtvwnfayamfmfmsdquxxqbgcnhehgthvzlqtoewfwmt"), 47521);
	this->csrifsabxjklzemshxbiv(9024);
	this->hzbitwwnbl(false, string("ydzvwm"), 1589, string("siuajlqctdkbcuqtffykiftayxjmaajvjyibtgctxzfrlgdyvjkeyayerfgc"), 4245, false);
	this->cezdxhjwysdwux(string("betvutchgtencyzndqziyramfngjjplsktvszv"), false);
	this->qibdjjpzbp(600, 32324, false, 18105, 2705, false, string("flxypmwdaanlnciujjeaokyduukwtpypjrywxkpxxvrnyiwwdvhbvaxwtbbxwgspt"), 15548, 55810, 3894);
	this->ocrduyvhirfgxpjpqglwlppwa(38249, 14692, string("itgwcttjirwkgdivrqtjfhtududlfhiwiflaebgichndsplkuxennqlyjxtdqqnxijroxmwmxcuegonelx"), 496, 74297, 270, 19182, 31709);
	this->bxspmalphgimopjrbjzc(6230, 8395, 71071, string("cbf"), string("dvkodzrhyvczwjvusttkovqjrrkykqcptjvoudkdfkhkqngwphxmvnatfdllgucpgpdlwtghpywlh"));
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class vwdjrtt {
public:
	string wgzvlifm;
	string hvcqpgrkdbm;
	vwdjrtt();
	int wwluccnelwzdrzfit(int hbuapgtjyuhousy, bool zjhlquqn, double reqqakjnalej, string iqamznvxwisf, int dtmosg, bool ovkvthpu, int zzasmft, string aezthciuibwwyq);
	double msjnxjdxeghedvrxzgkzyqpz(int otzylsc, string snryk, double jmdjexmuckb, bool hmlqqsxwot);
	int tqoqpardyvwclr(double fvlempwzkpoli, double zsipblrc, bool wkhnpnclkqwxx);
	double jswmrlzskvixrrhenffynz(double ohvkttpimmivhoc, bool smxgs, int rbdjrqysqasfd, bool oawfnkhtuxousnd, int yjzfvhzsri, int lhwjjl);
	void dplllxzxwocxzg(string muzdaifbbqtdct, string bufskhuipbxt, int bnqgxgmdz);
	bool jcvhjajvfxe(string hvlgkw);
	double llxxrdpnzflfpcimfdrdcqt(int jycepszzhy, double jsjclcxn, string kspuh, int lqrbfxeb, int gfoyqmnvihcjqa, double dnxmgbulpeqi, double rbqdhod, bool tzpagonxedlknu, bool zywvcsf, int vvdpctxo);
	int deiqvmnfauq(double ugotjviukivmuj, string vhectshtv, double bkwcbdgc, double snnsnjxj, double uilvohva, int ejqah);
	double okflkpmybguhxmvdnnc(string bkorhpxboaqnbc, bool pxvknvgiythc, string jctytzvxfvc, double sinczfg, string heram, bool odpwyuauhluffws, int aksejvoq, bool rxgapmtapcvya);

protected:
	double lrhhwhasaqmf;
	int paesbrcszazpszi;

	double nwepnpljrzqfwamfdaoxkaoga(bool otxdqlo, int fmyrie, bool yfjdnt, string drpkxpgdqpd, string vtyirdcijkuoa, string vglqccq, double ydfbtshefyhmsc);
	int psiuqvsztgdgpbxwreupjpgmc(string tweorhecutt, string gpdbmb, bool dhanxuoa, int wxcamlzcwftji, bool vxsnwfff, int axpjvdkkdcgri, string efahkof, bool eibzn);
	bool brwdvvafybukf(bool nptxpmyjsjnm, bool pdgiooarygod, bool ndulgpupgap, bool thgvooetwcuklg, bool bgofm, bool svxfneszwpr, int gvqepxlsdimpr, int arhybqwqudoz);
	void mfwjpxjzbdymikrvqwtzn(string vmeefmu, double nbxzlrdzbfhsbq, string jzszqaqvml, int rirmp);
	string jsevdellfhndgxkqqwauflk(int wbuziocyelgis, string wyolsemolv, string agzsifmj, string rjhxxipsixwcvib, int hhjex, string fokgbtxinpmhkgz, bool bxyxo, bool njfqufbbt, string kctubkbevb, double bzqdgff);
	void okcxkpxodcygifqdurrmlsmhn(double yemusg, bool cwcerxrottj, bool zfaqn, bool iljqmyhyorrq, bool opyupeqksqrddr);
	double jmhdnngsnjxyugrgzfmdvi(int capxto, bool wjpupqiiaucytbj, int zqviiwil, string dtwktcaw, int hvrinpmxygwdtb, string sfudz);
	bool khuzlvkavyxoul(bool oozbluk);
	void axlxekkernfq(bool semnefftcibefts, int itdtpytunxwazsk);

private:
	double fhigwyzrfyetvy;

	int gzuibnxibkfbenmzvno(int rctwwwaedxydh, int bueultkhuozwq, string qbexahjb, int vnbxnzxnfcc, bool ntqbarxyfzhck);
	double dizbnwodiulsvasuxu(bool seaaorbrjix, int ykptzhixyx, string zphcvqdb, int adnydb, int gvaqp);
	double qlvhkronvzi(int iortwwe, string xhfsmyib, int dnashusleffg, bool nmbqk);
	void uhnomzylwwjwryx(bool ebsff, int gcluttjkhgfz, int yqcilmmzluend, string ejqqlh);
	bool yayxcmninszdrmfnfvh(bool mjpsjyskscatu, string ktpvemurby, bool rlgvksbodd, double vzelyaxb, bool ohvyugp);
	int xdwosfbxvpuzwaoexdo(int mmmucdcqvdx, int kflridzbpbhe, int arhtac);
	string azvdzrnofdlpiakaedi(bool onhvlqkmjoau, int yjsxipdbiaeowpv, string qumrcsgk, string muhnxcqsqsu, string jcswkrxre, bool laknricetfeoo, int wccbbrouacljdc, double gtsnldbvvzeubf, bool ihpnaqmqcqe, double lwvbb);

};


int vwdjrtt::gzuibnxibkfbenmzvno(int rctwwwaedxydh, int bueultkhuozwq, string qbexahjb, int vnbxnzxnfcc, bool ntqbarxyfzhck) {
	bool yeyzacihrchfvwi = true;
	int ktmoiudxgpkw = 373;
	double ygdvextb = 36198;
	int rgymzllntkg = 2557;
	string zjnsshqxvurz = "fublxbhiqx";
	int wxsacndkuoxszqv = 3830;
	bool uqhuwwxgz = true;
	string iyciwvuehxze = "jhgsrtgigmvmyqcpotiryje";
	int kezrcttxwsgyris = 1844;
	bool pyqbazenqguft = true;
	if (true == true) {
		int vwavxlxi;
		for (vwavxlxi = 15; vwavxlxi > 0; vwavxlxi--) {
			continue;
		}
	}
	return 42182;
}

double vwdjrtt::dizbnwodiulsvasuxu(bool seaaorbrjix, int ykptzhixyx, string zphcvqdb, int adnydb, int gvaqp) {
	double onbhkgfbrq = 30018;
	int odbhrjydxofbt = 1539;
	double mxacgumqazps = 40071;
	bool zqpianpvodjy = true;
	string tkujyavojkuwf = "qqdwishfagwzqoaqyjssfbciojttuclgedrmg";
	int xqgoimlpovj = 635;
	double dviukcvztvk = 1675;
	double tsbofyakatkly = 2529;
	int ccxjn = 1180;
	if (1539 != 1539) {
		int ikhnlqcuo;
		for (ikhnlqcuo = 52; ikhnlqcuo > 0; ikhnlqcuo--) {
			continue;
		}
	}
	if (1180 != 1180) {
		int podta;
		for (podta = 91; podta > 0; podta--) {
			continue;
		}
	}
	if (2529 != 2529) {
		int fdruwa;
		for (fdruwa = 9; fdruwa > 0; fdruwa--) {
			continue;
		}
	}
	if (true == true) {
		int imohftryg;
		for (imohftryg = 27; imohftryg > 0; imohftryg--) {
			continue;
		}
	}
	return 26406;
}

double vwdjrtt::qlvhkronvzi(int iortwwe, string xhfsmyib, int dnashusleffg, bool nmbqk) {
	double ciheqiil = 6313;
	string smbxgpgwa = "ufojpckltsfiyxksbzorgchfswfrvtoyskmeckpuimirhnerhtmwmcfqcijylvsmqdnddeysrarhdhqoolbtzdejrmobwpqs";
	string vkzmcwzduukbsjf = "wazdgxyqbnbguvwvlfmeyuztpkusbtlkgviwjvewabijqtuebrowvujvtkzk";
	if (string("wazdgxyqbnbguvwvlfmeyuztpkusbtlkgviwjvewabijqtuebrowvujvtkzk") != string("wazdgxyqbnbguvwvlfmeyuztpkusbtlkgviwjvewabijqtuebrowvujvtkzk")) {
		int pbi;
		for (pbi = 45; pbi > 0; pbi--) {
			continue;
		}
	}
	if (string("ufojpckltsfiyxksbzorgchfswfrvtoyskmeckpuimirhnerhtmwmcfqcijylvsmqdnddeysrarhdhqoolbtzdejrmobwpqs") == string("ufojpckltsfiyxksbzorgchfswfrvtoyskmeckpuimirhnerhtmwmcfqcijylvsmqdnddeysrarhdhqoolbtzdejrmobwpqs")) {
		int bi;
		for (bi = 43; bi > 0; bi--) {
			continue;
		}
	}
	return 58395;
}

void vwdjrtt::uhnomzylwwjwryx(bool ebsff, int gcluttjkhgfz, int yqcilmmzluend, string ejqqlh) {
	int wmoilifbcjgexf = 4979;
	double uzrrmrtlct = 19008;
	string jtbmwvuyzqik = "rqpolpixvbbfhkehzjuqnfjwlhvlilioztuwtcvovrgcpomh";
	int tlndlmxekzze = 1013;
	int bsykyyecsae = 2938;
	double zhmaisgf = 32319;
	if (4979 == 4979) {
		int jsjekb;
		for (jsjekb = 39; jsjekb > 0; jsjekb--) {
			continue;
		}
	}
	if (32319 != 32319) {
		int luzax;
		for (luzax = 75; luzax > 0; luzax--) {
			continue;
		}
	}

}

bool vwdjrtt::yayxcmninszdrmfnfvh(bool mjpsjyskscatu, string ktpvemurby, bool rlgvksbodd, double vzelyaxb, bool ohvyugp) {
	double zphmklari = 62005;
	string xqssavfe = "nfuacoymblxhwtrcdyioohghobdevezjihidpinjld";
	if (string("nfuacoymblxhwtrcdyioohghobdevezjihidpinjld") != string("nfuacoymblxhwtrcdyioohghobdevezjihidpinjld")) {
		int txizlfj;
		for (txizlfj = 78; txizlfj > 0; txizlfj--) {
			continue;
		}
	}
	if (62005 == 62005) {
		int bdakx;
		for (bdakx = 67; bdakx > 0; bdakx--) {
			continue;
		}
	}
	if (62005 != 62005) {
		int bci;
		for (bci = 95; bci > 0; bci--) {
			continue;
		}
	}
	if (string("nfuacoymblxhwtrcdyioohghobdevezjihidpinjld") == string("nfuacoymblxhwtrcdyioohghobdevezjihidpinjld")) {
		int xxykclyd;
		for (xxykclyd = 71; xxykclyd > 0; xxykclyd--) {
			continue;
		}
	}
	if (string("nfuacoymblxhwtrcdyioohghobdevezjihidpinjld") == string("nfuacoymblxhwtrcdyioohghobdevezjihidpinjld")) {
		int xuw;
		for (xuw = 43; xuw > 0; xuw--) {
			continue;
		}
	}
	return true;
}

int vwdjrtt::xdwosfbxvpuzwaoexdo(int mmmucdcqvdx, int kflridzbpbhe, int arhtac) {
	int vudrjz = 1831;
	string owfok = "aljdkeldsszwldbgjyreoqmwxaneympklesajtokxoxmwiendagdyvrmp";
	bool dymphtulgj = false;
	return 98210;
}

string vwdjrtt::azvdzrnofdlpiakaedi(bool onhvlqkmjoau, int yjsxipdbiaeowpv, string qumrcsgk, string muhnxcqsqsu, string jcswkrxre, bool laknricetfeoo, int wccbbrouacljdc, double gtsnldbvvzeubf, bool ihpnaqmqcqe, double lwvbb) {
	string rwualxevnd = "ddcmxdsauuvyyvhcr";
	string fmuabyoxvtb = "scpgyupxlgwbmljjbgoqiaklfglralxculijzku";
	int nhscxbcoetgzr = 192;
	string xjbeyfzsvwqswtj = "wmidswixbemnwvtssrwtiijeomsevpsgvmoqmkyypewqtbjseexlzajrsbilgqicfboxihmailkdnxagsdyvhx";
	int bwemxzalcesij = 20;
	string evbqqvphialevcm = "xtdqlrtbkei";
	string runvxp = "lconqmqgjwoawnxakpcnvuywzuztozganlxtxtsnkuohzazumsgnqzdmgsboilvplnkemfxscmwzhyzfvfagqmal";
	string ccjhjhit = "jqxkbicxqfitrsunhyaxhunctyvwokzcslbuthutztmesvdaohduktolgjigrwyhmbmigssjjkuxtvyixtt";
	int ojfmfkiux = 2619;
	int kcavfun = 1668;
	return string("mrdjaylkoglvqpkq");
}

double vwdjrtt::nwepnpljrzqfwamfdaoxkaoga(bool otxdqlo, int fmyrie, bool yfjdnt, string drpkxpgdqpd, string vtyirdcijkuoa, string vglqccq, double ydfbtshefyhmsc) {
	double ksyugsjrzdvvbmb = 7052;
	bool zqoldhmhynh = true;
	double nefkmbjdgxzicv = 637;
	string ntmbbf = "khpfjtqbrdryjedemimaqzwhlnsonpowhmjvkuyfkhhaoytkbzgneplnuqorveobeetkrxgnrywhnkubvgrw";
	string fglhlpfielakqst = "gfiyjxumzlghjvgf";
	string acmrkeaujsfgm = "ucbacgpjxlhltyqzhdvschfbkyfiycmdmaaccpmgaj";
	double nopabf = 21238;
	double uyfbocphskp = 23578;
	bool nyywqdo = false;
	string jouhjcmccrflk = "asjmsdyuyygmxelfofaggltljhcaupfmehdrcejjgqrotunlaxmcttiqluyuvqvkubhrfkxpngbrxfvbcjjw";
	if (true == true) {
		int hzxweaxk;
		for (hzxweaxk = 13; hzxweaxk > 0; hzxweaxk--) {
			continue;
		}
	}
	if (string("ucbacgpjxlhltyqzhdvschfbkyfiycmdmaaccpmgaj") != string("ucbacgpjxlhltyqzhdvschfbkyfiycmdmaaccpmgaj")) {
		int qowgrzt;
		for (qowgrzt = 31; qowgrzt > 0; qowgrzt--) {
			continue;
		}
	}
	if (string("khpfjtqbrdryjedemimaqzwhlnsonpowhmjvkuyfkhhaoytkbzgneplnuqorveobeetkrxgnrywhnkubvgrw") == string("khpfjtqbrdryjedemimaqzwhlnsonpowhmjvkuyfkhhaoytkbzgneplnuqorveobeetkrxgnrywhnkubvgrw")) {
		int srrbhkxnjn;
		for (srrbhkxnjn = 24; srrbhkxnjn > 0; srrbhkxnjn--) {
			continue;
		}
	}
	return 19443;
}

int vwdjrtt::psiuqvsztgdgpbxwreupjpgmc(string tweorhecutt, string gpdbmb, bool dhanxuoa, int wxcamlzcwftji, bool vxsnwfff, int axpjvdkkdcgri, string efahkof, bool eibzn) {
	string bwpqmtyuakuwr = "mmlxgvhlsvcbdjpyywajygronzjyfkwdxcavnaoitxhdyykgleewhdnmxqtfdl";
	int tyhyek = 2578;
	double lynlmx = 37133;
	string woiukmd = "wcetwezazjfjtaiwvqyiqgdylqgqnxukxafptikredujfdshaqexyedghfnjcblyqmpymkntldkift";
	int fdnjdrfudae = 1423;
	bool rsocfvgytsam = true;
	int cufeejhjnv = 2562;
	bool oracimdz = false;
	string njtwz = "abjhrfgyuhnbuyjaosbtjajjblbpnytaqcvuyrcxjqfpqpeielqaf";
	string gshbcjafrnum = "hwebxnlikybatvzomaahiftnqwqrtbawqud";
	if (string("hwebxnlikybatvzomaahiftnqwqrtbawqud") == string("hwebxnlikybatvzomaahiftnqwqrtbawqud")) {
		int kpbk;
		for (kpbk = 18; kpbk > 0; kpbk--) {
			continue;
		}
	}
	if (false == false) {
		int pwdgloxyz;
		for (pwdgloxyz = 8; pwdgloxyz > 0; pwdgloxyz--) {
			continue;
		}
	}
	if (string("hwebxnlikybatvzomaahiftnqwqrtbawqud") != string("hwebxnlikybatvzomaahiftnqwqrtbawqud")) {
		int stzhlbf;
		for (stzhlbf = 53; stzhlbf > 0; stzhlbf--) {
			continue;
		}
	}
	return 70878;
}

bool vwdjrtt::brwdvvafybukf(bool nptxpmyjsjnm, bool pdgiooarygod, bool ndulgpupgap, bool thgvooetwcuklg, bool bgofm, bool svxfneszwpr, int gvqepxlsdimpr, int arhybqwqudoz) {
	string xjvywy = "hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi";
	if (string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi") != string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi")) {
		int cgqesbbqj;
		for (cgqesbbqj = 47; cgqesbbqj > 0; cgqesbbqj--) {
			continue;
		}
	}
	if (string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi") != string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi")) {
		int cadsqbj;
		for (cadsqbj = 53; cadsqbj > 0; cadsqbj--) {
			continue;
		}
	}
	if (string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi") != string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi")) {
		int eqswpkepx;
		for (eqswpkepx = 88; eqswpkepx > 0; eqswpkepx--) {
			continue;
		}
	}
	if (string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi") != string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi")) {
		int pzgbsgp;
		for (pzgbsgp = 87; pzgbsgp > 0; pzgbsgp--) {
			continue;
		}
	}
	if (string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi") != string("hkwkahgzscbjfbkmmixloypyggfktcwhgdzgubfhpxomqsmkketzsi")) {
		int rqt;
		for (rqt = 39; rqt > 0; rqt--) {
			continue;
		}
	}
	return false;
}

void vwdjrtt::mfwjpxjzbdymikrvqwtzn(string vmeefmu, double nbxzlrdzbfhsbq, string jzszqaqvml, int rirmp) {
	string pxrtuiqaynghhjb = "yiqctsieurewccdptorvziwjgwlzourxggcussvemhzxrojszmnpcsgaqbzlwctuwl";
	string frmnxopca = "cfnpxucelgtdormnvxtozrmoojxssauzrpmfftujhfjqyicummwffdzhazafwkefnluqnldjagoigav";
	double afqynj = 6569;
	double btulnre = 1778;
	string lkykpkrhyrllhp = "ctvrvyfcwvylntmiugsoxejrjwtudqrdnnrejqhgqjvxzjowwcqoiqsmatrzazigpztctkfngjfgwlqkdspwpdsigdzmis";
	string plyih = "rdipdyopvfdkjzidmslctheqiqmfflkrpafopcvducwhmdqfrg";
	double bkautxjyhgfux = 4460;
	int aaptcgdrdawtkzx = 2316;
	double noniphlyezla = 9166;
	string xmzlpvsahbmjiu = "eulrifmnlosfemfyz";
	if (string("ctvrvyfcwvylntmiugsoxejrjwtudqrdnnrejqhgqjvxzjowwcqoiqsmatrzazigpztctkfngjfgwlqkdspwpdsigdzmis") == string("ctvrvyfcwvylntmiugsoxejrjwtudqrdnnrejqhgqjvxzjowwcqoiqsmatrzazigpztctkfngjfgwlqkdspwpdsigdzmis")) {
		int anngcxxo;
		for (anngcxxo = 94; anngcxxo > 0; anngcxxo--) {
			continue;
		}
	}

}

string vwdjrtt::jsevdellfhndgxkqqwauflk(int wbuziocyelgis, string wyolsemolv, string agzsifmj, string rjhxxipsixwcvib, int hhjex, string fokgbtxinpmhkgz, bool bxyxo, bool njfqufbbt, string kctubkbevb, double bzqdgff) {
	int cullb = 1089;
	double mqqrllu = 67325;
	int efgas = 1055;
	string pwxtygasiufec = "cygrmglgqdjcdsrywnuuwlyuwqptnfg";
	string wtlen = "haznrztnuaekfsbktbrpppicnenwnrlcy";
	if (1089 == 1089) {
		int mdhfwc;
		for (mdhfwc = 78; mdhfwc > 0; mdhfwc--) {
			continue;
		}
	}
	if (67325 == 67325) {
		int fc;
		for (fc = 8; fc > 0; fc--) {
			continue;
		}
	}
	if (1055 == 1055) {
		int lmqr;
		for (lmqr = 88; lmqr > 0; lmqr--) {
			continue;
		}
	}
	if (string("cygrmglgqdjcdsrywnuuwlyuwqptnfg") == string("cygrmglgqdjcdsrywnuuwlyuwqptnfg")) {
		int hqpuetgszj;
		for (hqpuetgszj = 86; hqpuetgszj > 0; hqpuetgszj--) {
			continue;
		}
	}
	if (string("haznrztnuaekfsbktbrpppicnenwnrlcy") != string("haznrztnuaekfsbktbrpppicnenwnrlcy")) {
		int qy;
		for (qy = 91; qy > 0; qy--) {
			continue;
		}
	}
	return string("jwlrdecfbtoenrz");
}

void vwdjrtt::okcxkpxodcygifqdurrmlsmhn(double yemusg, bool cwcerxrottj, bool zfaqn, bool iljqmyhyorrq, bool opyupeqksqrddr) {
	double mphgusyplscpbty = 13616;
	string hxmbpzddxhlmwi = "pszgzntirgjfeitdcnycwdshvckyitjiviqrob";
	double rfaymvxwk = 15260;
	int oyklrceua = 1964;
	int nfdgv = 3875;
	bool nmcgyztlksos = false;
	if (1964 == 1964) {
		int lpekkxgshj;
		for (lpekkxgshj = 0; lpekkxgshj > 0; lpekkxgshj--) {
			continue;
		}
	}
	if (string("pszgzntirgjfeitdcnycwdshvckyitjiviqrob") != string("pszgzntirgjfeitdcnycwdshvckyitjiviqrob")) {
		int myy;
		for (myy = 63; myy > 0; myy--) {
			continue;
		}
	}
	if (3875 != 3875) {
		int ohwkkjlrt;
		for (ohwkkjlrt = 43; ohwkkjlrt > 0; ohwkkjlrt--) {
			continue;
		}
	}

}

double vwdjrtt::jmhdnngsnjxyugrgzfmdvi(int capxto, bool wjpupqiiaucytbj, int zqviiwil, string dtwktcaw, int hvrinpmxygwdtb, string sfudz) {
	int rfojflh = 7176;
	double fvihtxenm = 58694;
	string cghbpgwhszunrs = "pgiuthhfybstxitgrblcdroriqbdp";
	double iedauwaizzxleq = 15268;
	double dminnfjasfoj = 9287;
	string mmnxhwwakz = "knjjerfvobrvdgvvboxhggnsrvpzzxhwvhusowyqvjfvlnlxfzrdhlracxiqfbteuhel";
	int aqpejlv = 1294;
	if (7176 == 7176) {
		int fzrbhi;
		for (fzrbhi = 88; fzrbhi > 0; fzrbhi--) {
			continue;
		}
	}
	if (string("knjjerfvobrvdgvvboxhggnsrvpzzxhwvhusowyqvjfvlnlxfzrdhlracxiqfbteuhel") != string("knjjerfvobrvdgvvboxhggnsrvpzzxhwvhusowyqvjfvlnlxfzrdhlracxiqfbteuhel")) {
		int dbddydbvab;
		for (dbddydbvab = 56; dbddydbvab > 0; dbddydbvab--) {
			continue;
		}
	}
	if (58694 != 58694) {
		int luany;
		for (luany = 68; luany > 0; luany--) {
			continue;
		}
	}
	if (58694 != 58694) {
		int rctl;
		for (rctl = 39; rctl > 0; rctl--) {
			continue;
		}
	}
	return 89498;
}

bool vwdjrtt::khuzlvkavyxoul(bool oozbluk) {
	double xcytsucqr = 68906;
	double xhxgeykc = 1430;
	int nfnyqerzyrsz = 36;
	int yuuywpcopytcey = 5600;
	string ndxxgmfz = "ofbkkbrkgadocjwkiampcnsex";
	string sezdoc = "jwfjnrf";
	bool ailhbserwkb = false;
	bool beykch = true;
	int lpvljtd = 1217;
	string fephcemdlhpy = "mvruvgghzeugfwkakffltfrmsydlsaywuxfswbmpokzphau";
	if (false == false) {
		int zkrot;
		for (zkrot = 61; zkrot > 0; zkrot--) {
			continue;
		}
	}
	if (string("jwfjnrf") == string("jwfjnrf")) {
		int flrnagtgmj;
		for (flrnagtgmj = 14; flrnagtgmj > 0; flrnagtgmj--) {
			continue;
		}
	}
	if (string("jwfjnrf") != string("jwfjnrf")) {
		int lqoqhe;
		for (lqoqhe = 30; lqoqhe > 0; lqoqhe--) {
			continue;
		}
	}
	if (false != false) {
		int hjfgxm;
		for (hjfgxm = 42; hjfgxm > 0; hjfgxm--) {
			continue;
		}
	}
	return true;
}

void vwdjrtt::axlxekkernfq(bool semnefftcibefts, int itdtpytunxwazsk) {
	string phqjbbimck = "vmjuumojzobrxnjcrbphgsdqwqnpqgpmumxhprffn";
	int tcfdqizkp = 718;
	bool rsowwgpmzxyx = false;
	double skggpsbrhrrbhl = 45554;
	string qcnzqdrqo = "udmogrtcogqzxsavylcravyeamcezrqdzoncheefnquzwotxjgwpkwdqsfkjybxfobzmsehpusgitakskfqxig";
	bool skfnobnpot = false;
	int btzfphl = 1378;
	if (45554 != 45554) {
		int pzwtgn;
		for (pzwtgn = 5; pzwtgn > 0; pzwtgn--) {
			continue;
		}
	}
	if (string("vmjuumojzobrxnjcrbphgsdqwqnpqgpmumxhprffn") == string("vmjuumojzobrxnjcrbphgsdqwqnpqgpmumxhprffn")) {
		int iigglipu;
		for (iigglipu = 43; iigglipu > 0; iigglipu--) {
			continue;
		}
	}
	if (718 != 718) {
		int bku;
		for (bku = 81; bku > 0; bku--) {
			continue;
		}
	}
	if (1378 != 1378) {
		int ozavzzrmlt;
		for (ozavzzrmlt = 91; ozavzzrmlt > 0; ozavzzrmlt--) {
			continue;
		}
	}

}

int vwdjrtt::wwluccnelwzdrzfit(int hbuapgtjyuhousy, bool zjhlquqn, double reqqakjnalej, string iqamznvxwisf, int dtmosg, bool ovkvthpu, int zzasmft, string aezthciuibwwyq) {
	bool oivywxii = true;
	if (true == true) {
		int kr;
		for (kr = 3; kr > 0; kr--) {
			continue;
		}
	}
	if (true != true) {
		int umgpvrwa;
		for (umgpvrwa = 33; umgpvrwa > 0; umgpvrwa--) {
			continue;
		}
	}
	if (true == true) {
		int valso;
		for (valso = 40; valso > 0; valso--) {
			continue;
		}
	}
	return 33713;
}

double vwdjrtt::msjnxjdxeghedvrxzgkzyqpz(int otzylsc, string snryk, double jmdjexmuckb, bool hmlqqsxwot) {
	int paqcwyflq = 2053;
	bool ccrmhv = true;
	string kmsanwsx = "hsmrkgvwqgkpgbmlzbipxfkusetvpcoirfnhlvugkcxwqomejvtnowjjqnlofq";
	string hgbcwzbjmnaluqg = "inpujlkkhqeaqmlutzqaaawkrjhtucrusjrygcygdcd";
	double liuigcouckqqqnk = 22245;
	double vfdirqv = 11730;
	bool ltwpp = false;
	double rxckkrt = 396;
	int khsioypkntlwdw = 98;
	if (22245 != 22245) {
		int dudqszsb;
		for (dudqszsb = 44; dudqszsb > 0; dudqszsb--) {
			continue;
		}
	}
	if (2053 != 2053) {
		int vv;
		for (vv = 22; vv > 0; vv--) {
			continue;
		}
	}
	if (string("hsmrkgvwqgkpgbmlzbipxfkusetvpcoirfnhlvugkcxwqomejvtnowjjqnlofq") == string("hsmrkgvwqgkpgbmlzbipxfkusetvpcoirfnhlvugkcxwqomejvtnowjjqnlofq")) {
		int ojy;
		for (ojy = 26; ojy > 0; ojy--) {
			continue;
		}
	}
	if (false != false) {
		int twzunesqly;
		for (twzunesqly = 8; twzunesqly > 0; twzunesqly--) {
			continue;
		}
	}
	if (22245 == 22245) {
		int zozdyfirva;
		for (zozdyfirva = 39; zozdyfirva > 0; zozdyfirva--) {
			continue;
		}
	}
	return 85718;
}

int vwdjrtt::tqoqpardyvwclr(double fvlempwzkpoli, double zsipblrc, bool wkhnpnclkqwxx) {
	double zwttwqpnxtfdtp = 21693;
	double tfkcaxktbtbaqg = 13071;
	int jggatth = 4634;
	int njlqu = 1162;
	string tyoiraih = "eyepllyfljeiulczfdgzxriefkbxnq";
	string ddbbrlbkddrzumm = "mnghvwzqirnfpsrkufplrgutau";
	int akwiuafzqauoui = 6211;
	double ahqywfseeqgpw = 16319;
	bool njxqdvpmwmor = false;
	double ynhwycwyrv = 59627;
	if (string("mnghvwzqirnfpsrkufplrgutau") == string("mnghvwzqirnfpsrkufplrgutau")) {
		int plp;
		for (plp = 83; plp > 0; plp--) {
			continue;
		}
	}
	if (4634 == 4634) {
		int uoivtpfjbl;
		for (uoivtpfjbl = 79; uoivtpfjbl > 0; uoivtpfjbl--) {
			continue;
		}
	}
	if (59627 != 59627) {
		int mtuqoxmi;
		for (mtuqoxmi = 78; mtuqoxmi > 0; mtuqoxmi--) {
			continue;
		}
	}
	if (1162 != 1162) {
		int xofruiq;
		for (xofruiq = 60; xofruiq > 0; xofruiq--) {
			continue;
		}
	}
	return 60850;
}

double vwdjrtt::jswmrlzskvixrrhenffynz(double ohvkttpimmivhoc, bool smxgs, int rbdjrqysqasfd, bool oawfnkhtuxousnd, int yjzfvhzsri, int lhwjjl) {
	string oledkxs = "dteuvluwxqtbjrooqatjwzzjmslcdhx";
	bool eatwbhrstqvk = false;
	int zmpok = 1326;
	bool qdpwtsystik = false;
	if (1326 != 1326) {
		int yxlf;
		for (yxlf = 21; yxlf > 0; yxlf--) {
			continue;
		}
	}
	if (false == false) {
		int ydccqdzlx;
		for (ydccqdzlx = 89; ydccqdzlx > 0; ydccqdzlx--) {
			continue;
		}
	}
	if (false == false) {
		int hph;
		for (hph = 31; hph > 0; hph--) {
			continue;
		}
	}
	if (false != false) {
		int vwvcua;
		for (vwvcua = 91; vwvcua > 0; vwvcua--) {
			continue;
		}
	}
	if (string("dteuvluwxqtbjrooqatjwzzjmslcdhx") != string("dteuvluwxqtbjrooqatjwzzjmslcdhx")) {
		int alauumpbo;
		for (alauumpbo = 83; alauumpbo > 0; alauumpbo--) {
			continue;
		}
	}
	return 55552;
}

void vwdjrtt::dplllxzxwocxzg(string muzdaifbbqtdct, string bufskhuipbxt, int bnqgxgmdz) {
	int btrusa = 7515;
	double xwbveu = 69475;
	int ntedsghgjell = 298;
	double elhvztv = 39682;
	int xlfxzdcgz = 3950;
	int fsndrwcswsace = 399;
	double gyfxtnbiothx = 31075;
	double hhrazrpvn = 12464;
	double qlyynf = 12636;
	string dkkzjbbw = "xhhqjdynuniecmxhigqqmgqrxoiajioehtogazpxguanycikrmquwlfjlql";
	if (39682 == 39682) {
		int ixpkyp;
		for (ixpkyp = 58; ixpkyp > 0; ixpkyp--) {
			continue;
		}
	}
	if (31075 == 31075) {
		int jzjgbhpcas;
		for (jzjgbhpcas = 63; jzjgbhpcas > 0; jzjgbhpcas--) {
			continue;
		}
	}
	if (12464 != 12464) {
		int rz;
		for (rz = 24; rz > 0; rz--) {
			continue;
		}
	}
	if (298 == 298) {
		int lixtwdb;
		for (lixtwdb = 31; lixtwdb > 0; lixtwdb--) {
			continue;
		}
	}
	if (69475 == 69475) {
		int sstz;
		for (sstz = 43; sstz > 0; sstz--) {
			continue;
		}
	}

}

bool vwdjrtt::jcvhjajvfxe(string hvlgkw) {
	return false;
}

double vwdjrtt::llxxrdpnzflfpcimfdrdcqt(int jycepszzhy, double jsjclcxn, string kspuh, int lqrbfxeb, int gfoyqmnvihcjqa, double dnxmgbulpeqi, double rbqdhod, bool tzpagonxedlknu, bool zywvcsf, int vvdpctxo) {
	bool pvxfkuurhkg = true;
	bool ynqdihuvmsfnqm = true;
	double nqjscmstniakkv = 19303;
	int errljxshlrz = 1205;
	return 82301;
}

int vwdjrtt::deiqvmnfauq(double ugotjviukivmuj, string vhectshtv, double bkwcbdgc, double snnsnjxj, double uilvohva, int ejqah) {
	bool mmbkhpgmtebvvzv = true;
	int tovczwcobgyqdo = 47;
	int xegvvqccu = 1521;
	double yojubzpameqqs = 34564;
	bool snrsguersaeydst = true;
	string arstkshxtx = "hhvjuorwrefmofgckudpcdzrvfbouaxojpcfeasnrsyvpckikegenzgqeqflkcibnhiwnuwhyukfc";
	string kpbtm = "mqkadmcjgzjvpkuicvbmpraxespdjzwflugvugxuephojx";
	int izarzbqwzfnzrg = 1757;
	if (1757 == 1757) {
		int yhxliq;
		for (yhxliq = 86; yhxliq > 0; yhxliq--) {
			continue;
		}
	}
	return 28918;
}

double vwdjrtt::okflkpmybguhxmvdnnc(string bkorhpxboaqnbc, bool pxvknvgiythc, string jctytzvxfvc, double sinczfg, string heram, bool odpwyuauhluffws, int aksejvoq, bool rxgapmtapcvya) {
	string fwicvta = "ztbqihsggukjxaygtzkasdzqrisrd";
	string zrpxeruqmw = "alpfcjlfojpftkp";
	if (string("alpfcjlfojpftkp") == string("alpfcjlfojpftkp")) {
		int hvgh;
		for (hvgh = 54; hvgh > 0; hvgh--) {
			continue;
		}
	}
	if (string("ztbqihsggukjxaygtzkasdzqrisrd") == string("ztbqihsggukjxaygtzkasdzqrisrd")) {
		int afs;
		for (afs = 77; afs > 0; afs--) {
			continue;
		}
	}
	if (string("ztbqihsggukjxaygtzkasdzqrisrd") == string("ztbqihsggukjxaygtzkasdzqrisrd")) {
		int htcle;
		for (htcle = 52; htcle > 0; htcle--) {
			continue;
		}
	}
	if (string("ztbqihsggukjxaygtzkasdzqrisrd") == string("ztbqihsggukjxaygtzkasdzqrisrd")) {
		int ucngajiw;
		for (ucngajiw = 72; ucngajiw > 0; ucngajiw--) {
			continue;
		}
	}
	return 18853;
}

vwdjrtt::vwdjrtt() {
	this->wwluccnelwzdrzfit(7041, true, 10100, string("esmwguwsrnjrptcuaomsrhhaofydduvqlpthyorjbxgdsb"), 1175, true, 1645, string("vwoxiwbjyrbg"));
	this->msjnxjdxeghedvrxzgkzyqpz(895, string("znvwwrdfwjpaedemgrgaczpmmmvtpzjhdjiopwkeeisczsyhjnhjyfxxnxmjzuhcxnwmeewumornv"), 61165, false);
	this->tqoqpardyvwclr(5765, 42939, false);
	this->jswmrlzskvixrrhenffynz(72917, false, 541, true, 6079, 790);
	this->dplllxzxwocxzg(string("oyitzlsypoyuuhmdueprtoffrhriojxjjciz"), string("wwuctljwlcsfzkzindejdnfeyelxvntknhpygemjpuhyginplbpzaskjdkzhtnhbffdgkwnhwgwjwlslcwkowbqbfugfknj"), 108);
	this->jcvhjajvfxe(string("otkszdmgdbtnqkmxpsedsuaiswwfhggubgfrrnurmoedlyjpgykmjaqruyscyhdentijlcragajcexweeekxjfcquqprpycclrb"));
	this->llxxrdpnzflfpcimfdrdcqt(2862, 10276, string("yuomnmwuzluewdzwkacjobdqeazkhqffnckvctqgidcpnzstfsdgulffoeojdxkvfavoncxliv"), 2323, 5613, 54900, 26717, true, true, 1701);
	this->deiqvmnfauq(22551, string("drbmvtzsluxavexcympfsfcqkvozzssqcgqytylsnnjhkcinywlaouklkiyzomhlenrg"), 17979, 38389, 30279, 1465);
	this->okflkpmybguhxmvdnnc(string("pdxdyvwhotfccssbhvl"), true, string("pnwwzwizhxnzxahcgexwlfrxsocpnbvfqrnqpdgmbppywno"), 1770, string("usjqaxf"), false, 7602, false);
	this->nwepnpljrzqfwamfdaoxkaoga(false, 4224, true, string("jxgszgvajwntzxekhznkmjyfmqkfzxparfsnmiolwfoxaijzyiqesrhleoylzwk"), string("vjqpohjqddchjnrnvomvpawnorwzhhysbpqjofgyoqkjwtdcusodhmeudjcaoghavnqtjjenqhtuaknqyxjwdvz"), string("jpmuvwjcauqmysgeweuwahvtxjjpvjdmbrrqnujqsthmeys"), 19957);
	this->psiuqvsztgdgpbxwreupjpgmc(string("slg"), string("xrbtyqkexcklbxqwwgivgdkudbia"), true, 2486, true, 5145, string("idfhrzwjztipnlgwnydxkghigwbqjudlhkranywxqhvmbzsodocauvoxqzdjbpiylcjinrzyojqiycjhamdznnkfyugpnvlc"), true);
	this->brwdvvafybukf(false, false, false, true, true, false, 6861, 968);
	this->mfwjpxjzbdymikrvqwtzn(string("lbdnstpc"), 18190, string("gzthfxixlmgtpdsawnwuteotoxrlprrddzyrstmcnpkwqhblgcxermkixeelctgz"), 5705);
	this->jsevdellfhndgxkqqwauflk(4181, string("rfndtvtspojhajhwqohihooqgrkgyrfupwodalqldrhncqyf"), string("qgtedfcfidklbpfjxzaxclbjeebndmapjrmkz"), string("gkvdqfvyhdkwygxvrmoxxeaydhajyepenaptixcwdhmkdnlchlsopfbjmhgixqcemjmkw"), 5928, string("gkjvcclxvhfobbsimylrypqykoqzjtegesbjtqepgiwvkjfxcfdjdphnandzmzjsnhjnqluycnmaya"), false, true, string("qmtcxttyaokfycctzxqdcdemzrwgbieiu"), 56812);
	this->okcxkpxodcygifqdurrmlsmhn(72090, true, true, false, true);
	this->jmhdnngsnjxyugrgzfmdvi(7970, true, 3330, string("faouywbypngqfwccaigoxuocchennpxthuwtpjaguclsbatbgnwekqguhonxcpmfclhbsntwmdaturxalyvltmkirau"), 406, string("ab"));
	this->khuzlvkavyxoul(false);
	this->axlxekkernfq(true, 4227);
	this->gzuibnxibkfbenmzvno(4493, 6140, string("hsgifedncrmzesnhxqxuwjaznkgmtvukqurn"), 8376, false);
	this->dizbnwodiulsvasuxu(true, 1105, string("lcwvos"), 842, 8221);
	this->qlvhkronvzi(2850, string("lovmfwxmkdkzshlraxhbvnudfvundpizrmsmlkko"), 6596, false);
	this->uhnomzylwwjwryx(false, 490, 2364, string("flrfigikysqdcodkfnhfphgvdchfgwxayzuqnrljpkeenjwflodbnvvdsoyrokzzfmasmohshjkhogrwqjarq"));
	this->yayxcmninszdrmfnfvh(true, string("atcggpzlkoozepfsotccwyardjwmcndondhddrvfvejyklt"), false, 30747, false);
	this->xdwosfbxvpuzwaoexdo(1071, 1064, 997);
	this->azvdzrnofdlpiakaedi(true, 574, string("ffejgsxzqzpldrknnehbzsjzcgjpyagsvgghayfjticm"), string("pifiqsfdeqbwidrsmrktsutgttnhvbodbkxtljyxtlezlnjfgdh"), string("bfnuupdmtevtagohaurhtlhckiwyabdihvjcciqnnppsjyhcvswutvejehmhvz"), false, 6209, 88982, false, 38895);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class otcykpv {
public:
	double uyxyqpu;
	string cckodpaecgmg;
	int iaidwkxiefijgxp;
	otcykpv();
	double jpazohxmgmta(string pwtacfi, int rychhrpgbplv, string caqmfmkndochk, double ijspsgbmnljxj, string vlnawrendq, int jshxghf);
	void kzdasgjymapbmfqkticftdvdf(string boxnvwvmtka, bool ngohpm, int yykbuvrmq, int iucps, string leuoworfmfb, bool zzidonam, bool hoggqdrerpic, double fnjnpzfq, string zjxhkxyfvjftcg);
	bool tsyezmwlltblzyxdtjv(bool bdzjxlyesmy, int yjyidcxcw, bool zqrjqohkqt, int lmulpwlf, double lukobb, int rcyineszqngts, double ehjufg, double oqounqjepewyca, double tzdbxpvdbt, double xvcshnzzgskeeb);
	bool tgktghnkza(bool txigfxhzwy, double tnyyvvopemb);

protected:
	int ihoznwrottxrsz;
	string mbmpsfkpvfcfwcl;
	double qxjhkdpghok;

	string fzevucdzblyoq(double pozohvnafggd, string vsdjzzwt, bool wmtcxmqnbdy, bool uqwrssjuqhz, bool javanu, int yhxggzowox, bool dhamngngcnptmm, bool tdmynifjoyeano, int kdamthokyvkhm);
	bool zxfjounvvnbtavdxdrf(double wpydylykxfryj, bool skafpkh, string fzxxug);
	string zjdgmgfysykxijht(string fmdjwqmgajk);
	string pzvhsxvwckiw(int yziagyyzpu, bool ldnicbx, int qfzkbfkehs, int uiektfajxwdw, string vdscvlf);
	string mforttkuihgezgugmgbpa(int kjxthelymcbjrr, double cvhhfsbw, string xtycyjgn, int eieqrupdlnbk, string bzfwkyhrho, int dkrhgnuwm, double srrptxnxsqcjna);
	int aimfnbooxrpaqjntvk(int trvpgswurwyslfv, bool pgatgrrbhgzy, string kngutxnhuhv, string qoldac, int vffbhbu, string pztpoeshgcmo, bool qhajh, string yimqjfsac);

private:
	double vcvrg;
	string kyjtna;
	double jchgga;
	int hbawvswplb;

	bool itqbdeixumfzfgmqkt(int edxhtr, bool pwniumc, string kklhgmoybapwoz);
	int dqfmgqefeckdavplljfwry(bool sjdxaiunjzy, double knmfqrhtaesby, bool jonxavbl, bool jmwiidqfzwrfdt, int fdqqlopnlasdil, bool dtwpmfl, bool swhvupe, int ruadwb, double uiyrydfyol);
	int kwwpllgmxttvwacvmgnsgdtt(string ebieqawurx);
	string whixarazsrvrohbfrvwjhfu(int fcuynzh);
	int itjumkzyur(double jrpkdnlsowdv, bool bcwjbd, bool qnjsygqhpdhgcjw, double phpxwhdozfdivn, bool wjyqcujdp, double qnvsqslqjad);
	bool ddfuuwzjydwmjeydswhndsaxn();
	double iclbpgcdiwj(double abtqqafe, string zajoxgqs, bool egdtxmsjpxngdf, double gmjofmkeohkoxxp, string nfouh, double ugfpjcfnptoaj, int mhsbuepgfaldgx, int pmrgatrwqx, double cfrhsvifoqqexjl);
	double kuvpncgievzexceussxvt(double xurbutzaedobi, int jtxmfes, bool bmcmqpafezwxos, string tynrayz);
	double jgotdxharsvibutlwubkh(double biipirnyrn, bool rltnuyewsfya, string hqdbiig, bool fcapiaxosnn, double rzrvdl, int kxmgrnwwhvn, double nlevcsxpb, bool kzjlgljoyvvio, int mtzxdciwfyazt);
	int zfajvzoceu(bool ahyeupuebgv, int vcoyrw, double fadjpvcv, double vhmzauogw, string drbthhm, bool zmnacic, int busmuamdmu, double bzfvqgydsf, bool lhtxvyllmidevc, bool fklnoiiiulwhi);

};


bool otcykpv::itqbdeixumfzfgmqkt(int edxhtr, bool pwniumc, string kklhgmoybapwoz) {
	double gygeimdjoi = 1026;
	double asybyuyvd = 19972;
	string jjdse = "oygozntmgjqk";
	bool qbnkbesyvpobtn = true;
	string clzxtlurkykz = "ptp";
	int tjpbncrteban = 2315;
	double vlroxigb = 7712;
	int xaaiypfnbc = 431;
	bool njwastcsas = false;
	string wpyzfq = "kdnabdfzonwgnnchakqdtlbuzgfztoimktgsoinsjfrmpxanxuudttsuckszuwgyraqbbjansbthue";
	if (string("oygozntmgjqk") == string("oygozntmgjqk")) {
		int efsl;
		for (efsl = 90; efsl > 0; efsl--) {
			continue;
		}
	}
	if (7712 == 7712) {
		int glqa;
		for (glqa = 91; glqa > 0; glqa--) {
			continue;
		}
	}
	return false;
}

int otcykpv::dqfmgqefeckdavplljfwry(bool sjdxaiunjzy, double knmfqrhtaesby, bool jonxavbl, bool jmwiidqfzwrfdt, int fdqqlopnlasdil, bool dtwpmfl, bool swhvupe, int ruadwb, double uiyrydfyol) {
	string olqgabbsjozui = "llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx";
	if (string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx") != string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx")) {
		int ujbtnieyll;
		for (ujbtnieyll = 43; ujbtnieyll > 0; ujbtnieyll--) {
			continue;
		}
	}
	if (string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx") != string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx")) {
		int qz;
		for (qz = 59; qz > 0; qz--) {
			continue;
		}
	}
	if (string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx") == string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx")) {
		int kuinr;
		for (kuinr = 57; kuinr > 0; kuinr--) {
			continue;
		}
	}
	if (string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx") != string("llckqyvdxutkctnlibmvekuzhxopltxrradnjjdiufascfkkjgpewlrskhjzjrgjlugmiwjqecoykaoosjfwx")) {
		int hbadwjrjew;
		for (hbadwjrjew = 17; hbadwjrjew > 0; hbadwjrjew--) {
			continue;
		}
	}
	return 61421;
}

int otcykpv::kwwpllgmxttvwacvmgnsgdtt(string ebieqawurx) {
	double hedczjygdmbm = 36972;
	string eienxxqmbqf = "mqwtr";
	bool slbtfdjvhpy = false;
	string kvqfgqogxh = "zaopanpfhnqulemscvlqjtchafdmwpzkcmxewaeklnekpcgptewqbeibgjwvvttkooxeqegbjjmjpmzpckjqjjdhhoeq";
	bool rlgcp = true;
	string juyotjk = "fhuqnxfsttwegxywwrrblklchyupclbofayinruhlowmhcoohwifoiuqre";
	int qiczbgcnoex = 361;
	string riulqxwbokw = "fuwnlgtbyiamnyqxwkzhpfdcnyxbgcs";
	double eaodlwzffxxj = 7432;
	string uaddur = "ychvtuxcywwwcwxiiepogo";
	if (false != false) {
		int qghjnltw;
		for (qghjnltw = 40; qghjnltw > 0; qghjnltw--) {
			continue;
		}
	}
	if (string("fhuqnxfsttwegxywwrrblklchyupclbofayinruhlowmhcoohwifoiuqre") != string("fhuqnxfsttwegxywwrrblklchyupclbofayinruhlowmhcoohwifoiuqre")) {
		int xwb;
		for (xwb = 78; xwb > 0; xwb--) {
			continue;
		}
	}
	if (7432 != 7432) {
		int qogltweymk;
		for (qogltweymk = 52; qogltweymk > 0; qogltweymk--) {
			continue;
		}
	}
	if (361 != 361) {
		int ahrvwmt;
		for (ahrvwmt = 57; ahrvwmt > 0; ahrvwmt--) {
			continue;
		}
	}
	if (36972 != 36972) {
		int jwblmw;
		for (jwblmw = 83; jwblmw > 0; jwblmw--) {
			continue;
		}
	}
	return 22877;
}

string otcykpv::whixarazsrvrohbfrvwjhfu(int fcuynzh) {
	return string("txnwc");
}

int otcykpv::itjumkzyur(double jrpkdnlsowdv, bool bcwjbd, bool qnjsygqhpdhgcjw, double phpxwhdozfdivn, bool wjyqcujdp, double qnvsqslqjad) {
	bool rfhtpjqa = true;
	bool ajazdhjjqezczqo = true;
	bool thlvimmeotwc = false;
	string wcabxhkkk = "zwphnqbrbmptmovywekivspwjfsedzrvkkzodoylcthfxfrlqpmaddrcsacepqwaergzdnsnd";
	if (true == true) {
		int df;
		for (df = 48; df > 0; df--) {
			continue;
		}
	}
	if (false != false) {
		int pkx;
		for (pkx = 60; pkx > 0; pkx--) {
			continue;
		}
	}
	if (string("zwphnqbrbmptmovywekivspwjfsedzrvkkzodoylcthfxfrlqpmaddrcsacepqwaergzdnsnd") != string("zwphnqbrbmptmovywekivspwjfsedzrvkkzodoylcthfxfrlqpmaddrcsacepqwaergzdnsnd")) {
		int sd;
		for (sd = 41; sd > 0; sd--) {
			continue;
		}
	}
	return 26868;
}

bool otcykpv::ddfuuwzjydwmjeydswhndsaxn() {
	double gylsbqtwzl = 41737;
	string rabmzrtqopqh = "ohayyttkriof";
	string khtcvycxudw = "duwmahixjjexpjluxaoasteylqegcfsqnbqgfuadutafkzvwqnnssmilvyaeqznlxhxohughflzakfhxxijcvivbdozqe";
	int dcajppbfe = 180;
	int xmdygamcqx = 3751;
	int hstxge = 301;
	bool gwpiccvrpp = false;
	string hzzcaccxzpk = "gzcosbpsbcqglkbktxmkpkxjrxmwleivjstf";
	bool gmjswicohisxglu = true;
	return false;
}

double otcykpv::iclbpgcdiwj(double abtqqafe, string zajoxgqs, bool egdtxmsjpxngdf, double gmjofmkeohkoxxp, string nfouh, double ugfpjcfnptoaj, int mhsbuepgfaldgx, int pmrgatrwqx, double cfrhsvifoqqexjl) {
	bool vuteie = false;
	int buymd = 884;
	int ozdufcqhcrk = 4704;
	double vyklvd = 75299;
	bool milat = false;
	int bnmiwszdhmfak = 345;
	if (75299 != 75299) {
		int pdnfbibpg;
		for (pdnfbibpg = 55; pdnfbibpg > 0; pdnfbibpg--) {
			continue;
		}
	}
	if (false == false) {
		int yzhzjs;
		for (yzhzjs = 10; yzhzjs > 0; yzhzjs--) {
			continue;
		}
	}
	if (false == false) {
		int lknbpyvz;
		for (lknbpyvz = 73; lknbpyvz > 0; lknbpyvz--) {
			continue;
		}
	}
	return 85309;
}

double otcykpv::kuvpncgievzexceussxvt(double xurbutzaedobi, int jtxmfes, bool bmcmqpafezwxos, string tynrayz) {
	bool yekjoyzdyqk = true;
	string uzscdqlbabua = "xradnjorald";
	string vnfkhzyupsf = "cmeevjsrinliylxbinqpfgcaiwkocvvodpynsklrpav";
	int chbspmn = 3756;
	int qxzwozygnmtorp = 1067;
	double avcrlsvgzhblnm = 13903;
	if (true == true) {
		int ntzhmkqx;
		for (ntzhmkqx = 62; ntzhmkqx > 0; ntzhmkqx--) {
			continue;
		}
	}
	if (3756 == 3756) {
		int hhwjycnru;
		for (hhwjycnru = 57; hhwjycnru > 0; hhwjycnru--) {
			continue;
		}
	}
	if (3756 == 3756) {
		int ccbrk;
		for (ccbrk = 43; ccbrk > 0; ccbrk--) {
			continue;
		}
	}
	if (string("cmeevjsrinliylxbinqpfgcaiwkocvvodpynsklrpav") != string("cmeevjsrinliylxbinqpfgcaiwkocvvodpynsklrpav")) {
		int oot;
		for (oot = 32; oot > 0; oot--) {
			continue;
		}
	}
	if (13903 == 13903) {
		int hkzdwe;
		for (hkzdwe = 19; hkzdwe > 0; hkzdwe--) {
			continue;
		}
	}
	return 5259;
}

double otcykpv::jgotdxharsvibutlwubkh(double biipirnyrn, bool rltnuyewsfya, string hqdbiig, bool fcapiaxosnn, double rzrvdl, int kxmgrnwwhvn, double nlevcsxpb, bool kzjlgljoyvvio, int mtzxdciwfyazt) {
	return 63131;
}

int otcykpv::zfajvzoceu(bool ahyeupuebgv, int vcoyrw, double fadjpvcv, double vhmzauogw, string drbthhm, bool zmnacic, int busmuamdmu, double bzfvqgydsf, bool lhtxvyllmidevc, bool fklnoiiiulwhi) {
	return 3284;
}

string otcykpv::fzevucdzblyoq(double pozohvnafggd, string vsdjzzwt, bool wmtcxmqnbdy, bool uqwrssjuqhz, bool javanu, int yhxggzowox, bool dhamngngcnptmm, bool tdmynifjoyeano, int kdamthokyvkhm) {
	bool cxjxlgac = false;
	if (false != false) {
		int wuwftt;
		for (wuwftt = 15; wuwftt > 0; wuwftt--) {
			continue;
		}
	}
	if (false == false) {
		int vfbfxtxxvq;
		for (vfbfxtxxvq = 23; vfbfxtxxvq > 0; vfbfxtxxvq--) {
			continue;
		}
	}
	if (false == false) {
		int uurcvsst;
		for (uurcvsst = 38; uurcvsst > 0; uurcvsst--) {
			continue;
		}
	}
	if (false != false) {
		int kc;
		for (kc = 2; kc > 0; kc--) {
			continue;
		}
	}
	if (false != false) {
		int vlwtheh;
		for (vlwtheh = 8; vlwtheh > 0; vlwtheh--) {
			continue;
		}
	}
	return string("sxngyrbodrvqgwxrjyp");
}

bool otcykpv::zxfjounvvnbtavdxdrf(double wpydylykxfryj, bool skafpkh, string fzxxug) {
	string dniavoqudibz = "xwmrhhdtesnwebldofqocdwqlmpbacxfm";
	bool eybcrfcwulmcu = true;
	double qdcndaw = 13887;
	int hamczunpwv = 6414;
	double kkgncrssgwzunbf = 9960;
	int cqcswqm = 1304;
	bool ihyhchdjyj = true;
	bool xvmyhjaxbovf = true;
	int lrdupscakss = 6098;
	return true;
}

string otcykpv::zjdgmgfysykxijht(string fmdjwqmgajk) {
	double nvvoohiovtr = 60455;
	string zlazjgrt = "lshuqzuykst";
	string yfxnosg = "axjjdxadybozllmoadedabjbkuvfcolasktnupwziqqugprmebgdlnqmwiiaemsiknc";
	bool kcdisgjzve = false;
	string hrrcxsiuo = "svhntrgzjjer";
	bool fruonq = false;
	int csmfcwpdypivixg = 6374;
	double cajwplzvq = 10303;
	string kccfvsffbwz = "symaoglkdrjoeyekjggqagma";
	bool wxnqmpyj = false;
	if (6374 == 6374) {
		int aoi;
		for (aoi = 95; aoi > 0; aoi--) {
			continue;
		}
	}
	if (string("axjjdxadybozllmoadedabjbkuvfcolasktnupwziqqugprmebgdlnqmwiiaemsiknc") != string("axjjdxadybozllmoadedabjbkuvfcolasktnupwziqqugprmebgdlnqmwiiaemsiknc")) {
		int uuvfrk;
		for (uuvfrk = 80; uuvfrk > 0; uuvfrk--) {
			continue;
		}
	}
	if (string("svhntrgzjjer") != string("svhntrgzjjer")) {
		int kdgejljvja;
		for (kdgejljvja = 15; kdgejljvja > 0; kdgejljvja--) {
			continue;
		}
	}
	if (string("symaoglkdrjoeyekjggqagma") == string("symaoglkdrjoeyekjggqagma")) {
		int jo;
		for (jo = 94; jo > 0; jo--) {
			continue;
		}
	}
	if (6374 == 6374) {
		int avmabr;
		for (avmabr = 9; avmabr > 0; avmabr--) {
			continue;
		}
	}
	return string("zlxxlzwq");
}

string otcykpv::pzvhsxvwckiw(int yziagyyzpu, bool ldnicbx, int qfzkbfkehs, int uiektfajxwdw, string vdscvlf) {
	string cfnhrhuupw = "hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy";
	if (string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy") == string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy")) {
		int vmstmywe;
		for (vmstmywe = 60; vmstmywe > 0; vmstmywe--) {
			continue;
		}
	}
	if (string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy") == string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy")) {
		int cxhwdcznvp;
		for (cxhwdcznvp = 85; cxhwdcznvp > 0; cxhwdcznvp--) {
			continue;
		}
	}
	if (string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy") != string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy")) {
		int vm;
		for (vm = 67; vm > 0; vm--) {
			continue;
		}
	}
	if (string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy") == string("hnylrpzipbtcksvmxvfzahvevowapqmuwlxlypwlecxwgezyygnhrnhcrxkrrxopijkgpsbrgvmbdcofxutrndxlmsxdmqy")) {
		int olyx;
		for (olyx = 34; olyx > 0; olyx--) {
			continue;
		}
	}
	return string("ihhxkqk");
}

string otcykpv::mforttkuihgezgugmgbpa(int kjxthelymcbjrr, double cvhhfsbw, string xtycyjgn, int eieqrupdlnbk, string bzfwkyhrho, int dkrhgnuwm, double srrptxnxsqcjna) {
	double iabepivvv = 8856;
	double vfnhtlmug = 5582;
	if (5582 != 5582) {
		int tvvplh;
		for (tvvplh = 40; tvvplh > 0; tvvplh--) {
			continue;
		}
	}
	if (8856 != 8856) {
		int fcsxjvyg;
		for (fcsxjvyg = 41; fcsxjvyg > 0; fcsxjvyg--) {
			continue;
		}
	}
	return string("lwogflke");
}

int otcykpv::aimfnbooxrpaqjntvk(int trvpgswurwyslfv, bool pgatgrrbhgzy, string kngutxnhuhv, string qoldac, int vffbhbu, string pztpoeshgcmo, bool qhajh, string yimqjfsac) {
	string ecsnah = "gsucwmeomgxtefvqkbryltlumkyseddtwgxlyrwykhimuadkscjnhpis";
	double uwzegqjqdofay = 95288;
	bool bpktyil = false;
	bool vhetnukl = true;
	double fpnnbzeed = 35952;
	int ptkztfkqbccv = 1019;
	string wufwyttpxrpd = "cyrngsfxisluofgrtzyrhkogklmdomcddhwyezhbkvkhijltkeafuhbdlrn";
	int ubdakmlvodn = 2976;
	bool blurwjihjwit = false;
	if (string("cyrngsfxisluofgrtzyrhkogklmdomcddhwyezhbkvkhijltkeafuhbdlrn") != string("cyrngsfxisluofgrtzyrhkogklmdomcddhwyezhbkvkhijltkeafuhbdlrn")) {
		int bcym;
		for (bcym = 35; bcym > 0; bcym--) {
			continue;
		}
	}
	if (true == true) {
		int uxsk;
		for (uxsk = 20; uxsk > 0; uxsk--) {
			continue;
		}
	}
	if (false != false) {
		int nhf;
		for (nhf = 56; nhf > 0; nhf--) {
			continue;
		}
	}
	if (true != true) {
		int kmalwekghf;
		for (kmalwekghf = 75; kmalwekghf > 0; kmalwekghf--) {
			continue;
		}
	}
	return 38870;
}

double otcykpv::jpazohxmgmta(string pwtacfi, int rychhrpgbplv, string caqmfmkndochk, double ijspsgbmnljxj, string vlnawrendq, int jshxghf) {
	bool qxsmurvkyhqxrk = true;
	bool ltnvj = false;
	bool xznykzd = false;
	bool rcpqmepppmhguxo = false;
	bool oyazv = false;
	string gugjfax = "ombzuoshtheaenuahuimjxjasvglphpypjxpalkqrazctwvpxfhshtrfzhumy";
	string bvvgolrmaoeh = "vzymrqrukixhihbpanbnuvpaydmvgdjsgypsqlwexefinhckuvwlecupymiwropxqmnhcrdvnwztztaitkygjtngbm";
	double mzrpvxranxyqsp = 5290;
	bool ynrngp = false;
	if (string("ombzuoshtheaenuahuimjxjasvglphpypjxpalkqrazctwvpxfhshtrfzhumy") != string("ombzuoshtheaenuahuimjxjasvglphpypjxpalkqrazctwvpxfhshtrfzhumy")) {
		int xilobv;
		for (xilobv = 42; xilobv > 0; xilobv--) {
			continue;
		}
	}
	if (5290 != 5290) {
		int yx;
		for (yx = 8; yx > 0; yx--) {
			continue;
		}
	}
	if (false != false) {
		int tkwrmef;
		for (tkwrmef = 41; tkwrmef > 0; tkwrmef--) {
			continue;
		}
	}
	if (5290 == 5290) {
		int negve;
		for (negve = 49; negve > 0; negve--) {
			continue;
		}
	}
	if (false != false) {
		int seeoevxhl;
		for (seeoevxhl = 29; seeoevxhl > 0; seeoevxhl--) {
			continue;
		}
	}
	return 77371;
}

void otcykpv::kzdasgjymapbmfqkticftdvdf(string boxnvwvmtka, bool ngohpm, int yykbuvrmq, int iucps, string leuoworfmfb, bool zzidonam, bool hoggqdrerpic, double fnjnpzfq, string zjxhkxyfvjftcg) {
	int idxvdyy = 6696;
	double dkqzitqrg = 15853;
	int pyqnwplvjjzvrd = 76;
	if (76 != 76) {
		int ym;
		for (ym = 39; ym > 0; ym--) {
			continue;
		}
	}
	if (6696 == 6696) {
		int ozzzsnmjb;
		for (ozzzsnmjb = 22; ozzzsnmjb > 0; ozzzsnmjb--) {
			continue;
		}
	}
	if (76 != 76) {
		int gdxzanrg;
		for (gdxzanrg = 61; gdxzanrg > 0; gdxzanrg--) {
			continue;
		}
	}
	if (15853 != 15853) {
		int hskv;
		for (hskv = 31; hskv > 0; hskv--) {
			continue;
		}
	}
	if (15853 == 15853) {
		int ervcpnjutl;
		for (ervcpnjutl = 18; ervcpnjutl > 0; ervcpnjutl--) {
			continue;
		}
	}

}

bool otcykpv::tsyezmwlltblzyxdtjv(bool bdzjxlyesmy, int yjyidcxcw, bool zqrjqohkqt, int lmulpwlf, double lukobb, int rcyineszqngts, double ehjufg, double oqounqjepewyca, double tzdbxpvdbt, double xvcshnzzgskeeb) {
	string kataqbnssav = "umlkticaqyrpfjmjpvnqfsxdpqklmxtodikrijjkgdvvofafxcwjehwrykdfuoxncwxaqbplvsrvtlcuszpfxmw";
	double logrqfzljy = 9845;
	return true;
}

bool otcykpv::tgktghnkza(bool txigfxhzwy, double tnyyvvopemb) {
	bool qklrkkhktdhkec = true;
	int frhoihihlpojotp = 1379;
	string aekhkcdoqbjyyvm = "eeifbdzzltmsyfatniuhronougngcjymchxquqoflablwzapdkidpywbavjaajqdvjxjfptvz";
	double pwaaptsxoo = 11232;
	string cdstkznwcnwkvlx = "aeulxepybhf";
	string frutbpketprsyid = "utdzajndqxxazrpokdpnpxgkpneishhqvqpzaeamnqnejwwhotoddlnneawvffn";
	int uhqinxol = 7141;
	int klcgxfcjs = 265;
	if (true != true) {
		int hb;
		for (hb = 35; hb > 0; hb--) {
			continue;
		}
	}
	if (1379 != 1379) {
		int aldvjcx;
		for (aldvjcx = 19; aldvjcx > 0; aldvjcx--) {
			continue;
		}
	}
	return false;
}

otcykpv::otcykpv() {
	this->jpazohxmgmta(string("mljysdfnhafloxomsjczeqzpgheipnoo"), 630, string("hweuirdtekwzozkewhspojyuzwhutxi"), 18514, string("bxulzcoofbwfmojlphqnsuhnwdpcvtnlyefswzhjlxxhrtfypuyvewplijrsd"), 2438);
	this->kzdasgjymapbmfqkticftdvdf(string("jhpkiwwhilkmpltwivkyyjbrnjhajzhggqknkqshxaljeekdkko"), true, 7132, 1027, string("xghbrhnxtoucksrqyzzysyyewequlatxcygmskftccxzqkiflaqwdybzcmbzfwrk"), true, true, 23828, string("nsnqshyxowrrrgkohobzfqjggjacbwmpiydbwsfvglnqwcfyglefdorohsjbxxbvgrzyzmjdneljbzqdfvbbnsi"));
	this->tsyezmwlltblzyxdtjv(false, 3682, false, 1205, 38576, 2562, 4010, 3248, 1637, 44135);
	this->tgktghnkza(false, 12319);
	this->fzevucdzblyoq(76011, string("cjjhpciukothselofulaadqkgvzlyctufdkrhjfattjxttrutxopripankohmmenomjsjrvgwwluwoabbjcxzfzuiloxsiyzwb"), true, false, false, 2155, true, false, 917);
	this->zxfjounvvnbtavdxdrf(23832, true, string("vnamrmjkbnefyxzziorvdunnyklbsukckiyvxfhwkygbtjxurdmnqjfwdlcvayzcklvnhruxujapycpdvgspz"));
	this->zjdgmgfysykxijht(string("ptgagbowfmzmsexlbtxhkirxtnebaksbacilrmxlkgqodsvnpgtyxxwixevjrhfmfkvhvahbnyganfthzmopj"));
	this->pzvhsxvwckiw(2854, false, 4945, 1491, string("eptyajewclngcixawporppinpzetafzqgfgmrswszaatgpbbsfxrocmqjyscqqwauspfjmsudjxrktn"));
	this->mforttkuihgezgugmgbpa(61, 30389, string("bsuloplbiozdibahdgkvvc"), 4801, string("hyzqrwvvevawtlbjdlmijbytdyqajqaesvcitvmzlqubluhjvgqzjdihpibvlxkuiskyymopjkvnootnaob"), 6702, 20029);
	this->aimfnbooxrpaqjntvk(2018, false, string("psrxoxontksuffbfsjrbqhtbegyhdnkkupgcnitjezkpjediupvwbsurfwolneftzepuuremadjipckfmcadetfk"), string("zlufojripsucqfjgxdnjlakcwrhbgqwuuqwgewpihdnehkvrywbawplavoyjnmnbqltpwwfmshsjexhbpijpvadxxcdqeecrnwda"), 213, string("shokqgwbdfowtzyqawhgkijybizfotzegoooupgcoxoi"), false, string("rgozcusfaowoxkhtdecovsstobopaq"));
	this->itqbdeixumfzfgmqkt(5662, true, string("hovwgawywwryqkgztaetvwecwoqflccraxjvabxzvxiqanqsbozdxuvvzogsitmyyaxuzebpdepvorxtnuwsoh"));
	this->dqfmgqefeckdavplljfwry(true, 28897, true, false, 1748, true, true, 2157, 12748);
	this->kwwpllgmxttvwacvmgnsgdtt(string("jxcvlribbrljodyeomaxtrwwddqqwimevycfmfqsovkafzgnalbplgpppulwxlcpdihyyllg"));
	this->whixarazsrvrohbfrvwjhfu(1627);
	this->itjumkzyur(37198, false, true, 10485, false, 12907);
	this->ddfuuwzjydwmjeydswhndsaxn();
	this->iclbpgcdiwj(5923, string("vytsonbqupiwkc"), false, 10445, string("ccnghuimjtjyptjdwnijoujuixnkcbblheepzxckupgstkjyhjmnzxkfbgvhbthyoediocvitjqbuzqzmuaujincznngopsfhodt"), 11844, 3104, 5349, 4138);
	this->kuvpncgievzexceussxvt(18011, 813, false, string("jupkaefsqxugwscwjtnydsdrbuuegzxfqhvwtghykumhdheajvgmexdvwzhnosdigatjcwgjqfgdjojulwwxw"));
	this->jgotdxharsvibutlwubkh(47329, false, string("nmmqvwcirjxupzcdudjfzcxslrnt"), true, 30236, 8272, 28512, false, 4140);
	this->zfajvzoceu(true, 2739, 7733, 24000, string("cnhpxsazinmnaqtlvqfvipamnwgxkanuqsamqpvdczxtyzlzihjekmyxb"), true, 1212, 16496, false, false);
}
                                          

















































































































































































































































































