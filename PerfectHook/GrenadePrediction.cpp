#include "GrenadePrediction.h"
#include "Render.h"
void grenade_prediction::Tick(int buttons)
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	act = (in_attack && in_attack2) ? ACT_LOB :
		(in_attack2) ? ACT_DROP :
		(in_attack) ? ACT_THROW :
		ACT_NONE;
}
void grenade_prediction::View(CViewSetup* setup)
{

	auto local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	if (local && local->IsAlive())
	{
		CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->GetActiveWeaponHandle());
		if (weapon && MiscFunctions::IsGrenade(weapon) && act != ACT_NONE)
		{
			type = weapon->m_AttributeManager()->m_Item()->GetItemDefinitionIndex();
			Simulate(setup);
		}
		else
		{
			type = 0;
		}
	}
}

void grenade_prediction::Paint()
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	if ((type) && path.size()>1)
	{
		Vector nadeStart, nadeEnd;

		Color lineColor(int(g_Options.Colors.tracer_color[0] * 255), int(g_Options.Colors.tracer_color[1] * 255), int(g_Options.Colors.tracer_color[2] * 255), 255);
		Vector prev = path[0];
		for (auto it = path.begin(), end = path.end(); it != end; ++it)
		{
			if (g_Render->WorldToScreen(prev, nadeStart) && g_Render->WorldToScreen(*it, nadeEnd))
			{
				g_Surface->DrawSetColor(lineColor);
				g_Surface->DrawLine((int)nadeStart.x, (int)nadeStart.y, (int)nadeEnd.x, (int)nadeEnd.y);
			}
			prev = *it;
		}

		if (g_Render->WorldToScreen(prev, nadeEnd))
		{
			g_Surface->DrawSetColor(Color(255, 0, 0, 255));
			g_Surface->DrawOutlinedCircle((int)nadeEnd.x, (int)nadeEnd.y, 10, 25);
		}
	}
}
static const constexpr auto PIRAD = 0.01745329251f;
void angle_vectors2(const Vector &angles, Vector *forward, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	sp = static_cast<float>(sin(double(angles.x) * PIRAD));
	cp = static_cast<float>(cos(double(angles.x) * PIRAD));
	sy = static_cast<float>(sin(double(angles.y) * PIRAD));
	cy = static_cast<float>(cos(double(angles.y) * PIRAD));
	sr = static_cast<float>(sin(double(angles.z) * PIRAD));
	cr = static_cast<float>(cos(double(angles.z) * PIRAD));

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
void grenade_prediction::Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles)
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	Vector angThrow = viewangles;
	auto local = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
	float pitch = angThrow.x;

	if (pitch <= 90.0f)
	{
		if (pitch<-90.0f)
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	angle_vectors2(angThrow, &vForward, &vRight, &vUp); //angThrow.ToVector(vForward, vRight, vUp);

	vecSrc = local->GetEyePosition();
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest += vForward * 22.0f; //vecDest.MultAdd(vForward, 22.0f);

	TraceHull(vecSrc, vecDest, tr);

	// After the hull trace it moves 6 units back along vForward
	// vecSrc = tr.endpos - vForward * 6
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = local->GetVelocity(); vecThrow *= 1.25f;
	vecThrow += vForward * flVel; //	vecThrow.MultAdd(vForward, flVel);
}

void grenade_prediction::Simulate(CViewSetup* setup)
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	Vector vecSrc, vecThrow;
	Vector angles; g_Engine->GetViewAngles(angles);
	Setup(vecSrc, vecThrow, angles);

	float interval = g_Globals->interval_per_tick;

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;


	path.clear();
	for (unsigned int i = 0; i<path.max_size() - 1; ++i)
	{
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);
		if ((s & 1)) break;

		// Reset the log timer every logstep OR we bounced
		if ((s & 2) || logtimer >= logstep) logtimer = 0;
		else ++logtimer;
	}
	path.push_back(vecSrc);
}

int grenade_prediction::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{

	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval))
	{
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	// Set new position
	vecSrc = tr.endpos;

	return result;
}


bool grenade_prediction::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	switch (type)
	{
	case WEAPON_SMOKE:
	case WEAPON_DECOY:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length2D()<0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick%det_tick_mod);
		}
		return false;

	case WEAPON_MOLOTOV:
	case WEAPON_INC:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z>0.7f)
			return true;
		// OR we've been flying for too long

	case WEAPON_FLASH:
	case WEAPON_HE:
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		return static_cast<float>(tick)*interval>1.5f && !(tick%static_cast<int>(0.2f / interval));

	default:
		assert(false);
		return false;
	}
}

void grenade_prediction::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	Ray_t ray;
	ray.Init(src, end, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f));

	CTraceFilterWorldAndPropsOnly filter;
	//filter.SetIgnoreClass("BaseCSGrenadeProjectile");
	//filter.bShouldHitPlayers = false;

	g_EngineTrace->TraceRay(ray, 0x200400B, &filter, &tr);
}

void grenade_prediction::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void grenade_prediction::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	// Trace through world
	TraceHull(src, vecAbsEnd, tr);
}

void grenade_prediction::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	if (!g_Options.Visuals.GrenadePrediction)
		return;
	// Calculate elasticity
	float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity>0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity<0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr<flMinSpeedSqr)
	{
		//vecAbsVelocity.Zero();
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	// Stop if on ground
	if (tr.plane.normal.z>0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval); //vecAbsVelocity.Mult((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
	{
		vecVelocity = vecAbsVelocity;
	}
}

int grenade_prediction::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float    backoff;
	float    change;
	float    angle;
	int        i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class oleprms {
public:
	int kzxrerjxjbbc;
	string ocygrhrqkfyzi;
	int ygots;
	bool biyyufwrkolnb;
	oleprms();
	bool eamxxbctrehgzoyo(bool uttynd, double xlfesrc, int vkfjq, string htbbyzqoz, bool kcmvpnvrwe, int icxwavltteunpx, string grncwhfbxt, string jmyejdnmcrp, bool ajzhubn, bool wqdyd);
	double wdgzhxrhxwvbtmz(double spfyny, string hfvct, bool gdjaumllwvm, bool dezmoyrducgj, string rwqqdipwyofsfo, bool glkdem, bool szecfii, bool exoiqctnlyehq, double gbgkeyvvrconicz);
	bool sgajlnyoxcuiiki(bool gkylpwxhlcm, string ieoenja, bool ghsvnhcesmas, int oisiolcyyxtv, bool zwzce, string rntjeqydvcbpax);
	int vfetdhfguotsxvptlnakcg(double xlzdgiwfomstw, double ixupejk);

protected:
	string liknnk;

	double kmipguvtehvbjewhtnj(bool mdjvzoosmjbml, string isvmciivzyayaw, string tozxwmxt, int ttdbzkvhjyjdtz, string zeraogjrsbypk, int opkigtfriwtjc, string pqzjwz);
	bool pqqmngvdhjoakw(double jxfzeunwcethy, string ytzozmsntqq, double utfbmhcpr, string lxmjo);
	void crczergxlkika(int nnpyueqohpgq, bool fznzbilhtjjgjco, string kwcotxuxcetir, double voldzlyszrpdgv, string gtwilbwy, string rgiwolrgrykcbcb, string obriokzyborgwe, string zjvbtdqrx, bool jvyxgqzjcqaczi, int qkyexyxgkqikbjj);
	bool ycncdubnihxttbldylquzbs(int dpimupl, int mqxmmantqsv, string losidddiwca, double egvelvmfdh, bool huedbnbxaln);
	int dhmtlcctkeodapnmctbxsjow(double fxuneaazqbyul, double ychapfizfwro, string lbtxmkw, bool emnpnugkglmjdf, int aypnmkeje, bool qodpqopboqsl, double xuogtykvg, string hmmjwlpwmctg, bool yinqizmwtfog);
	double dizpevghjznczxaaijrwvsgz(string yadzjwv, bool cktlbtqbbs, bool jeyvbaejpj, string yhtkyrso);
	void xcfujphitnm(string xymmqlooawzoot, double ilbiokzcidh);

private:
	double nwzyyyr;
	double rhsef;
	int nfjdft;
	double lovnvjwh;
	int fdyxwstnuu;

	void cgyswnuckvorynoyhllneyu(bool ntscgxpli, bool ndakfr, double eoarznjvglybq, bool cfbru);
	bool poadooljubzaxqxifwpq(string kueqbovnrrmggnj, string iaockufg, bool bcsyzxvfadaie, string flxqzzewisghizs, double gldnhmprpki, int rlmrpjnmbcqdsm);
	double dbwdouckmsjqbpfp(bool wkugpotefmhtbr, double anclgsxnlt, bool cjoactw, int mshvkh, bool lgblhaftunf, double wqwmiws, string nttkgeufo, double ykfwiahjient);

};


void oleprms::cgyswnuckvorynoyhllneyu(bool ntscgxpli, bool ndakfr, double eoarznjvglybq, bool cfbru) {
	bool ezthf = true;
	int qshpsrelc = 744;
	int oducbk = 2403;
	int pzfaknxjuq = 5386;
	string ixzlywooahnl = "jybhgtoqxzreljtvnkpiwrsjuigeqpcdg";
	if (5386 != 5386) {
		int rqpltsoy;
		for (rqpltsoy = 77; rqpltsoy > 0; rqpltsoy--) {
			continue;
		}
	}

}

bool oleprms::poadooljubzaxqxifwpq(string kueqbovnrrmggnj, string iaockufg, bool bcsyzxvfadaie, string flxqzzewisghizs, double gldnhmprpki, int rlmrpjnmbcqdsm) {
	int rmkakzxixptz = 3586;
	int takfwhpanvmyz = 2046;
	double esaucvntgndj = 57812;
	int yzzqufnxierpx = 3354;
	int zemnj = 2857;
	bool gtftjqcknfegk = true;
	bool bpdlypineu = true;
	bool ewhlu = true;
	int xnvnktiuwo = 654;
	int oaxhowzavcpjrn = 6444;
	if (2857 == 2857) {
		int rtqihsust;
		for (rtqihsust = 93; rtqihsust > 0; rtqihsust--) {
			continue;
		}
	}
	if (57812 == 57812) {
		int pdxwpxgsdm;
		for (pdxwpxgsdm = 72; pdxwpxgsdm > 0; pdxwpxgsdm--) {
			continue;
		}
	}
	if (2046 != 2046) {
		int lkmwebr;
		for (lkmwebr = 58; lkmwebr > 0; lkmwebr--) {
			continue;
		}
	}
	if (true != true) {
		int unqhqunkuq;
		for (unqhqunkuq = 19; unqhqunkuq > 0; unqhqunkuq--) {
			continue;
		}
	}
	if (true != true) {
		int zyz;
		for (zyz = 93; zyz > 0; zyz--) {
			continue;
		}
	}
	return false;
}

double oleprms::dbwdouckmsjqbpfp(bool wkugpotefmhtbr, double anclgsxnlt, bool cjoactw, int mshvkh, bool lgblhaftunf, double wqwmiws, string nttkgeufo, double ykfwiahjient) {
	bool igunlolejpq = true;
	if (true != true) {
		int kf;
		for (kf = 14; kf > 0; kf--) {
			continue;
		}
	}
	if (true != true) {
		int glspcvv;
		for (glspcvv = 30; glspcvv > 0; glspcvv--) {
			continue;
		}
	}
	if (true != true) {
		int tnggoxq;
		for (tnggoxq = 55; tnggoxq > 0; tnggoxq--) {
			continue;
		}
	}
	return 35103;
}

double oleprms::kmipguvtehvbjewhtnj(bool mdjvzoosmjbml, string isvmciivzyayaw, string tozxwmxt, int ttdbzkvhjyjdtz, string zeraogjrsbypk, int opkigtfriwtjc, string pqzjwz) {
	return 35009;
}

bool oleprms::pqqmngvdhjoakw(double jxfzeunwcethy, string ytzozmsntqq, double utfbmhcpr, string lxmjo) {
	bool meqapfp = false;
	double haagandb = 87;
	bool jexsluvznwpuo = true;
	string bfjsljexfuz = "gphlspndgdgjfkvjshwccvyokqvvftuqbdjywlwsaztugvkzpxmigjgmlomfhvppawqrxdzbxdeyxdljaruvosdntptnlcziqnxq";
	double tenvkhyewbygtl = 49236;
	bool hwixoy = false;
	string sduvd = "eqpthghxvnzscwwetednicyibctgifnbks";
	bool vexikm = false;
	double vrwrgufqgdb = 6808;
	double leoqsfwj = 7631;
	if (string("eqpthghxvnzscwwetednicyibctgifnbks") != string("eqpthghxvnzscwwetednicyibctgifnbks")) {
		int vzed;
		for (vzed = 60; vzed > 0; vzed--) {
			continue;
		}
	}
	if (87 == 87) {
		int hpmqj;
		for (hpmqj = 65; hpmqj > 0; hpmqj--) {
			continue;
		}
	}
	return false;
}

void oleprms::crczergxlkika(int nnpyueqohpgq, bool fznzbilhtjjgjco, string kwcotxuxcetir, double voldzlyszrpdgv, string gtwilbwy, string rgiwolrgrykcbcb, string obriokzyborgwe, string zjvbtdqrx, bool jvyxgqzjcqaczi, int qkyexyxgkqikbjj) {

}

bool oleprms::ycncdubnihxttbldylquzbs(int dpimupl, int mqxmmantqsv, string losidddiwca, double egvelvmfdh, bool huedbnbxaln) {
	bool kgfpjxbfsuozfih = true;
	int lslxh = 706;
	bool ivftgcso = true;
	int aiithvuzxlosbb = 1013;
	int szowlfzpz = 1215;
	string pkejc = "wrrwfgasp";
	double tkagewqub = 3646;
	double uilqzwebsozbg = 19077;
	bool dpofstrkq = false;
	bool flkwhowwoh = false;
	if (1013 == 1013) {
		int pfu;
		for (pfu = 42; pfu > 0; pfu--) {
			continue;
		}
	}
	if (1215 == 1215) {
		int iuvq;
		for (iuvq = 2; iuvq > 0; iuvq--) {
			continue;
		}
	}
	if (string("wrrwfgasp") == string("wrrwfgasp")) {
		int zvciu;
		for (zvciu = 22; zvciu > 0; zvciu--) {
			continue;
		}
	}
	return false;
}

int oleprms::dhmtlcctkeodapnmctbxsjow(double fxuneaazqbyul, double ychapfizfwro, string lbtxmkw, bool emnpnugkglmjdf, int aypnmkeje, bool qodpqopboqsl, double xuogtykvg, string hmmjwlpwmctg, bool yinqizmwtfog) {
	bool abqqfs = true;
	double hxnyflulh = 21576;
	int cetilmwvxak = 197;
	int rrpmr = 511;
	string ikwqvepqz = "onsypvoemevznsxaekhairijmvygulbelbklxpjfaoa";
	int epyoa = 896;
	string xzziectentxx = "qydsnunwyrbcvziwwtfaeoaxbkjqzihiewellfxuqgllbasfbthbrjmcwuzpixbwefja";
	return 33123;
}

double oleprms::dizpevghjznczxaaijrwvsgz(string yadzjwv, bool cktlbtqbbs, bool jeyvbaejpj, string yhtkyrso) {
	bool kgxkhqzgfx = false;
	int ayvabyjs = 6804;
	bool ejzjxxach = true;
	int munuracyiwn = 1856;
	int ioifnijwks = 51;
	double xnhlzogiule = 63585;
	int fvezj = 840;
	if (6804 != 6804) {
		int atogfycf;
		for (atogfycf = 46; atogfycf > 0; atogfycf--) {
			continue;
		}
	}
	if (true != true) {
		int twjos;
		for (twjos = 79; twjos > 0; twjos--) {
			continue;
		}
	}
	if (1856 != 1856) {
		int vdqprx;
		for (vdqprx = 74; vdqprx > 0; vdqprx--) {
			continue;
		}
	}
	if (63585 == 63585) {
		int nzscrseh;
		for (nzscrseh = 31; nzscrseh > 0; nzscrseh--) {
			continue;
		}
	}
	if (6804 != 6804) {
		int zgq;
		for (zgq = 16; zgq > 0; zgq--) {
			continue;
		}
	}
	return 24059;
}

void oleprms::xcfujphitnm(string xymmqlooawzoot, double ilbiokzcidh) {
	bool ktulhkgrfvbcqq = false;
	double umhns = 6009;
	bool pavdmx = true;
	string jbfeasvekt = "qpnygfxjmtcskiqqptehvieszjqvlaxattwxtfeimfpkdrchhudtwjlskcsvebah";
	double wmlqxh = 52314;
	bool aurhdmm = false;
	bool nsckzjenyudy = true;
	string eyrzbymhpbz = "jqoysmitvgawkhqgtphgubpueaafsorpkicdlksdnjxbalqbqqrxtcwewpluwwy";
	double junlisjd = 21902;

}

bool oleprms::eamxxbctrehgzoyo(bool uttynd, double xlfesrc, int vkfjq, string htbbyzqoz, bool kcmvpnvrwe, int icxwavltteunpx, string grncwhfbxt, string jmyejdnmcrp, bool ajzhubn, bool wqdyd) {
	double upzhlyjj = 8776;
	double wgjbnjgxfbkhu = 9424;
	string kbnsizyl = "bhpujjge";
	bool pofdytneskstkt = true;
	int hpdlf = 623;
	if (623 == 623) {
		int ypaovhdn;
		for (ypaovhdn = 87; ypaovhdn > 0; ypaovhdn--) {
			continue;
		}
	}
	if (true == true) {
		int cgollgsfoy;
		for (cgollgsfoy = 20; cgollgsfoy > 0; cgollgsfoy--) {
			continue;
		}
	}
	if (8776 == 8776) {
		int ykqxmmiryn;
		for (ykqxmmiryn = 89; ykqxmmiryn > 0; ykqxmmiryn--) {
			continue;
		}
	}
	return false;
}

double oleprms::wdgzhxrhxwvbtmz(double spfyny, string hfvct, bool gdjaumllwvm, bool dezmoyrducgj, string rwqqdipwyofsfo, bool glkdem, bool szecfii, bool exoiqctnlyehq, double gbgkeyvvrconicz) {
	int whjlofqupnb = 1985;
	double pnuthcnreuv = 10441;
	int mbjqmcfbsqywy = 4701;
	double hxlzbvplkex = 46723;
	string xplqefz = "swrrrfedxxyeiuqzlridskyleczxcnqfedzbiauwblhtvyhcgoohlayhwfsbghfyhxmweazkldnrbexehwwgxycob";
	int hqwyosvbipwcxum = 2225;
	int kogfpyfwnhqo = 1742;
	int welwzk = 188;
	string zhoumztee = "kasrhlqlajcphyumohifvrcnaiskcrj";
	bool hxsjzrehehbexb = true;
	return 41744;
}

bool oleprms::sgajlnyoxcuiiki(bool gkylpwxhlcm, string ieoenja, bool ghsvnhcesmas, int oisiolcyyxtv, bool zwzce, string rntjeqydvcbpax) {
	string bsmklbuif = "rywqumlmyzipozkmuizzffoayyp";
	int zqkkmsjtbvqhok = 5925;
	double wilifaycf = 24775;
	bool ptdgml = false;
	if (string("rywqumlmyzipozkmuizzffoayyp") != string("rywqumlmyzipozkmuizzffoayyp")) {
		int wky;
		for (wky = 77; wky > 0; wky--) {
			continue;
		}
	}
	if (string("rywqumlmyzipozkmuizzffoayyp") != string("rywqumlmyzipozkmuizzffoayyp")) {
		int rplafnuccj;
		for (rplafnuccj = 19; rplafnuccj > 0; rplafnuccj--) {
			continue;
		}
	}
	if (false != false) {
		int kxeotqiva;
		for (kxeotqiva = 54; kxeotqiva > 0; kxeotqiva--) {
			continue;
		}
	}
	if (5925 == 5925) {
		int ld;
		for (ld = 4; ld > 0; ld--) {
			continue;
		}
	}
	return false;
}

int oleprms::vfetdhfguotsxvptlnakcg(double xlzdgiwfomstw, double ixupejk) {
	double mpxigxg = 6907;
	int hlohxusajvi = 1126;
	double llbveogzrjkjjo = 602;
	bool qcsfgvoc = true;
	double eeqmjravby = 31970;
	string dvhsmd = "misltfgrtatztqmvzqaefurvhnhttgggomdegnrwzdgskuewhpihbjvdxwvluiiqffkmuh";
	string dmafsef = "srttlcviuekfjsnzzvwuwottw";
	if (string("srttlcviuekfjsnzzvwuwottw") != string("srttlcviuekfjsnzzvwuwottw")) {
		int ekicnkn;
		for (ekicnkn = 14; ekicnkn > 0; ekicnkn--) {
			continue;
		}
	}
	return 63669;
}

oleprms::oleprms() {
	this->eamxxbctrehgzoyo(true, 36902, 3594, string("btpoybzobimvicjquxxygyzrnojwvulea"), true, 691, string("ijqu"), string("ykmjmhgspopfbdpfddprfdrplfxsvmdubetssldlkbwigzcmbomwupqglehvreyalisgb"), false, false);
	this->wdgzhxrhxwvbtmz(16650, string("twopbvwxiusnmjmnsoepwvkawgfoolymgvk"), true, false, string("bfeuzeahohqxvhatzcjujycicedweaaxeltrsftqsdewzvwdqoafovwyamrwlx"), false, false, false, 44934);
	this->sgajlnyoxcuiiki(false, string("fccvdtmefmqzsyvkzziphrxckvurectau"), true, 669, true, string("hayawkxbvtqwvddzpkckogzhyjnwysozbmbxcxtnigbudgmsrnk"));
	this->vfetdhfguotsxvptlnakcg(18132, 70839);
	this->kmipguvtehvbjewhtnj(false, string("qulyjkfixuzkajftynjkmywsdhscfjdtrjyldriwmheiudmmalknnxrnfsjgkadbd"), string("lqnwgfgmlpdljyavnwlnlnmsutqmnuqxungpbttnmvnfsnvltrywpvln"), 9, string("qsakscfzxekrqoqfglczdicneojargoforjndmnrjfhgoeutzqpciigq"), 1317, string("cnoiuahvfyimqrjhhxtkwyrwihgdp"));
	this->pqqmngvdhjoakw(774, string("ykbajnvtkmpiygeslmlvnsumfncepoavomgxbfzkefygifls"), 87681, string("cqbpfjgzqx"));
	this->crczergxlkika(1739, true, string("bdoeeydbsnvfcnbywrjptpmtyktedfmwh"), 10209, string("zkbdwuqqfilkbvnhxmbgefzupdyhdzcklwxxecrdxgvrupvpfshvvcqkctnrbdkyfcwreuiqmflhqpjdksjgcdvqfiaripwu"), string("xejatkyqjlsaympefskcuuugelqcrfiwgvldilupqmiljwmjwhboy"), string("gpsmsohxxoqwgevqrktmqjuheiszqdcfpbmwzuvrxwvbuztadalupbkcdydcnlxobzyaxtpxaspuunjhponcskwjotquxlzu"), string("dninnjkeqgusoqlxdqqwyakmofrqmbbatqehkamdyeaawirjpydbunuiyjyopdwqithzho"), true, 500);
	this->ycncdubnihxttbldylquzbs(728, 953, string("hjjovtmzshfazrwoujsqghlrpxbcwcxdbrwibbqcmzugplkbcnttzspxoqqzcaoymkuicl"), 9264, false);
	this->dhmtlcctkeodapnmctbxsjow(43915, 16551, string("aowynoofosrti"), false, 3836, false, 31953, string("rxxozfwsukjycltwkwxxqckkzytxpgrzstnkppghkuzoma"), true);
	this->dizpevghjznczxaaijrwvsgz(string("tpurxzjlwdxlfxjfoqbxklpsrzekxlcytijqrjdqunpyjtwxwnagfnvawnrhltswguaoaadnihxtmfstaxskgwhtc"), true, true, string("rksqqpzpjwwhtmgbxkgogqjyzlsscegxiwodwzcfppauyvqeogvyrsokqzqdghialcuttqdkmerfkodgkpfziiez"));
	this->xcfujphitnm(string("holebontvzwdhbzlueyahdsppdlztxir"), 46757);
	this->cgyswnuckvorynoyhllneyu(true, false, 31786, false);
	this->poadooljubzaxqxifwpq(string("qlvpjaqnszyiyvemailktipmsqejzhbgnnpgcxebegfehkqbariqnutxiccgdaziitkrfdixfoswetkzexvada"), string("vkluvuqexbazztkjofrxbxoidtfegnbnyvyexlosemrsytcrcjvjybhrfa"), true, string("mmbitvkezyxfb"), 57571, 8042);
	this->dbwdouckmsjqbpfp(true, 30495, false, 1296, false, 56931, string("lpxtvcgfqtcpwcnjupeespejs"), 76184);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class pzxtmwp {
public:
	int wwundobf;
	bool dkhuemo;
	string yudqntcrau;
	pzxtmwp();
	void jjmhuivcytqgv(string nkugppi, string zkigga, int zzvmqtvokoajcz, bool mhagkazquwgz, bool nppmjsu, string jdpvayfyx, int tyioxcs, int tcbiuw, string ipielwczuloixj);
	string cqcivvdvwcragrmyyvdob(int pynwhkqjt, string qcxbp, string wmntc, double ckyft, double atdhse, int wbvfrcxqkag);
	int lkkumiticrzfkdkye(int ylpsavducmgzgr, int ehchynn, string dmeotzj, int bddngttxupot, bool bpnbufxuc, string tvzxz, bool hxjwxlmg, bool zayvdbjypkln);
	double mqcbelgbbggksdo(string kjodjuqnxth, bool dfoyzxnavxevyjk, double gulbedyqw, string qjitaswyjpbve);
	string bhdueoravehp(int xjxdlsfhyif, bool kvmgigisrnft, string mjcggztjkprtj);
	double ueeaegltyyuxmvj(int qgvrjagifldlhq, string jksmy, int zfbupwhwxalefj, int carriwjupfdula, double vechicbjnst);
	bool oilgrxtxtwhicnysqx(double ncvrtwxgrwd, string nqopzcmozitt);
	bool zeinbdqwaywxnmgbmdzzs(double koqmkfqka);

protected:
	int fkgjdzcrosjfo;
	double cqyrptlep;

	double zvyaauzmvdnakwrilnu(string qnkhmpzlyups, int swdrlnfzxnsdcwh, double rgebpnfft, int oxeykuhiybdzhyr, string cjjanwnptasq, double cgwfctjyfu, bool nbhpidgdqr, double qgbvnozya, int bbmyvepa, int awqiq);
	bool urkyyulucdez(bool binffuxmm, bool lymcxs, bool snmubhvaq, bool gbejmznds);
	int hauswtsvmdayacuo();
	bool hqagsklypvwyvrgudyu(int ixzhhys, bool kbckpq, int mhpqtgqgsj, string rscsrbu, bool ipxxlampssce);
	int otzztwleygpykjehormeqhwzk(string ouoljnlpazik, int wgcnaolgifkowih, int ayompkg, double vkpxneqruv, int uutcndrr, bool gmhfq, bool plositoemscqga);

private:
	double ascivhttqfd;
	int vuycpdhczbhwmm;
	double vtgbfcvtxojhudi;
	int axyeekrz;
	int syfvnfgts;

	void sbfmtardpw(bool srrlszgfgeu, string qsiylhkokaqwn, int krlli, string tjgoxoathsvvkir, double laqzex);
	bool rumrmtrcqxbsrxth(string thqdqajogjlwu, string ibsvixsesite, double qazqjfsowtk, double abfxax, string rbeefmpitkl, double ssbsta, int ehapihcwmloas, int qyhnju);
	string mrdysetxerxlgslcqsjrxrcg(string pjkygdkmtqwqrpp, int pvhisy, int dozjonnssvjwll, string ofgwm, double aktuwomlorxuc, string vtfbeldshcdkwn, string cuahn, int ztobmaxyblxlhn, double uqoimbg, string uudbx);
	bool azzqojkhlrdxvdsknyfgrfdl(int yjmusxkumgiphs, int cbaejunwafr, double dzrgqeaw, string midwzdkbsa, string xqorckzlmn, double flfjgpvzvbgxpce, string kctmzbqbysuk, double bolfphdiahnhq);
	int fyczjzhxwfngkifuapzxgmbfo(double ykwmz, bool thpgindzz, string exsyypxylq, bool zvlniinyi);

};


void pzxtmwp::sbfmtardpw(bool srrlszgfgeu, string qsiylhkokaqwn, int krlli, string tjgoxoathsvvkir, double laqzex) {
	string oigfnvoowbfagn = "mpbbiwthzphmeidlhkpaawhpdcfexddobcycoctezxogmksqchqgfrxojyowrytsbxfnqwcypbnjfltlxewrcubrayj";
	string phlioectinajt = "zhwmqhkcywpednm";
	double cshxcoumkftmb = 5335;
	int sxapteebwce = 647;
	double tgweotrvebfrxj = 5677;
	int euxgxwazsiwy = 1455;
	string lzxraoqbxandu = "pdvpvknkqxmalnqmjkzgiv";
	string hbhpdkba = "kgbdqrfwppcimvvozlxweesymermyrb";
	if (string("zhwmqhkcywpednm") == string("zhwmqhkcywpednm")) {
		int xhm;
		for (xhm = 60; xhm > 0; xhm--) {
			continue;
		}
	}
	if (647 != 647) {
		int tyl;
		for (tyl = 86; tyl > 0; tyl--) {
			continue;
		}
	}
	if (5335 == 5335) {
		int uyrfobkola;
		for (uyrfobkola = 26; uyrfobkola > 0; uyrfobkola--) {
			continue;
		}
	}

}

bool pzxtmwp::rumrmtrcqxbsrxth(string thqdqajogjlwu, string ibsvixsesite, double qazqjfsowtk, double abfxax, string rbeefmpitkl, double ssbsta, int ehapihcwmloas, int qyhnju) {
	int hoxlufsltz = 4326;
	string dxgme = "qkludmhxennwyjdzusxsjkqldwbbfoz";
	string njhpt = "tisoefxizroelerdw";
	double izgbgymrdnkmzud = 49732;
	if (string("qkludmhxennwyjdzusxsjkqldwbbfoz") == string("qkludmhxennwyjdzusxsjkqldwbbfoz")) {
		int ttiq;
		for (ttiq = 52; ttiq > 0; ttiq--) {
			continue;
		}
	}
	if (string("tisoefxizroelerdw") == string("tisoefxizroelerdw")) {
		int xnceu;
		for (xnceu = 51; xnceu > 0; xnceu--) {
			continue;
		}
	}
	if (4326 == 4326) {
		int dfywtu;
		for (dfywtu = 48; dfywtu > 0; dfywtu--) {
			continue;
		}
	}
	return false;
}

string pzxtmwp::mrdysetxerxlgslcqsjrxrcg(string pjkygdkmtqwqrpp, int pvhisy, int dozjonnssvjwll, string ofgwm, double aktuwomlorxuc, string vtfbeldshcdkwn, string cuahn, int ztobmaxyblxlhn, double uqoimbg, string uudbx) {
	string pttegnzsehdy = "ffbgymywiqkqvhzgnqovfxuidcsfxdgexutyslhhypvekgmqjewzlmozoei";
	int mwvngjdlyumifo = 775;
	double cofvwbomedphnw = 40036;
	if (string("ffbgymywiqkqvhzgnqovfxuidcsfxdgexutyslhhypvekgmqjewzlmozoei") != string("ffbgymywiqkqvhzgnqovfxuidcsfxdgexutyslhhypvekgmqjewzlmozoei")) {
		int zn;
		for (zn = 60; zn > 0; zn--) {
			continue;
		}
	}
	if (40036 != 40036) {
		int qlkl;
		for (qlkl = 17; qlkl > 0; qlkl--) {
			continue;
		}
	}
	return string("rkwcbqnuohawh");
}

bool pzxtmwp::azzqojkhlrdxvdsknyfgrfdl(int yjmusxkumgiphs, int cbaejunwafr, double dzrgqeaw, string midwzdkbsa, string xqorckzlmn, double flfjgpvzvbgxpce, string kctmzbqbysuk, double bolfphdiahnhq) {
	string vuihgi = "dbqevjgpyklvblcytyrfdluwuvzgcaxnwpxzbsplnfvwezwxgeqvwycjebvwfkefinjsddgbwwla";
	int nxmmnoeygjtvpj = 6504;
	string jyojjjzkhgvzixf = "fjxogsnnskbtuihmaxwxhjxmainmscfsiwddjhhwxisrpovtwupn";
	double ntwfrtgxwnjl = 19973;
	string ltnxjit = "lmitcbgfrgsroghrxjtiwtdggqpchqetybxiajadgnxzgzzdeooeviafvfchspetpthsykha";
	double jmdwktagooghsnh = 8828;
	bool erwwjhhxauxthv = false;
	if (false == false) {
		int zk;
		for (zk = 19; zk > 0; zk--) {
			continue;
		}
	}
	if (6504 == 6504) {
		int nysdwtqqqr;
		for (nysdwtqqqr = 84; nysdwtqqqr > 0; nysdwtqqqr--) {
			continue;
		}
	}
	if (6504 == 6504) {
		int lzeq;
		for (lzeq = 29; lzeq > 0; lzeq--) {
			continue;
		}
	}
	if (string("dbqevjgpyklvblcytyrfdluwuvzgcaxnwpxzbsplnfvwezwxgeqvwycjebvwfkefinjsddgbwwla") == string("dbqevjgpyklvblcytyrfdluwuvzgcaxnwpxzbsplnfvwezwxgeqvwycjebvwfkefinjsddgbwwla")) {
		int npfsto;
		for (npfsto = 44; npfsto > 0; npfsto--) {
			continue;
		}
	}
	return false;
}

int pzxtmwp::fyczjzhxwfngkifuapzxgmbfo(double ykwmz, bool thpgindzz, string exsyypxylq, bool zvlniinyi) {
	int nulvvcvpwvlcy = 4882;
	double vkwlxvyia = 24914;
	int pjfgrlbsbsps = 4466;
	int yntejpa = 137;
	string iwuiasiymiwqq = "bkywmeuiarutzxxqacjsvtjpu";
	int dhlsfbopp = 523;
	double fhnqa = 52245;
	if (string("bkywmeuiarutzxxqacjsvtjpu") != string("bkywmeuiarutzxxqacjsvtjpu")) {
		int zkoxxazcj;
		for (zkoxxazcj = 23; zkoxxazcj > 0; zkoxxazcj--) {
			continue;
		}
	}
	if (4466 != 4466) {
		int jxiwjlfstk;
		for (jxiwjlfstk = 20; jxiwjlfstk > 0; jxiwjlfstk--) {
			continue;
		}
	}
	return 4725;
}

double pzxtmwp::zvyaauzmvdnakwrilnu(string qnkhmpzlyups, int swdrlnfzxnsdcwh, double rgebpnfft, int oxeykuhiybdzhyr, string cjjanwnptasq, double cgwfctjyfu, bool nbhpidgdqr, double qgbvnozya, int bbmyvepa, int awqiq) {
	string fldrihvbeg = "iasktssy";
	if (string("iasktssy") == string("iasktssy")) {
		int tbl;
		for (tbl = 60; tbl > 0; tbl--) {
			continue;
		}
	}
	return 65941;
}

bool pzxtmwp::urkyyulucdez(bool binffuxmm, bool lymcxs, bool snmubhvaq, bool gbejmznds) {
	bool jgztdtvutivrt = false;
	if (false != false) {
		int fcakno;
		for (fcakno = 13; fcakno > 0; fcakno--) {
			continue;
		}
	}
	if (false != false) {
		int lbw;
		for (lbw = 51; lbw > 0; lbw--) {
			continue;
		}
	}
	if (false != false) {
		int edlfwwtp;
		for (edlfwwtp = 99; edlfwwtp > 0; edlfwwtp--) {
			continue;
		}
	}
	if (false == false) {
		int zveln;
		for (zveln = 35; zveln > 0; zveln--) {
			continue;
		}
	}
	if (false != false) {
		int fld;
		for (fld = 5; fld > 0; fld--) {
			continue;
		}
	}
	return false;
}

int pzxtmwp::hauswtsvmdayacuo() {
	double pditdsgm = 30204;
	string jdzql = "nvzppxccxdotviclhjkyrcotdamvrisxsyyfhujbeqxoggnulkqzagcuodxf";
	bool zdawrqbdfnesrnf = true;
	int kqsmnqvpks = 3610;
	bool vjctavkzlyc = false;
	bool hrplqwcgwiz = true;
	double cjpjthdgyeqxudi = 8380;
	double yexoefm = 23554;
	string mqtgrg = "euhoalngjgaq";
	if (8380 != 8380) {
		int wujsu;
		for (wujsu = 82; wujsu > 0; wujsu--) {
			continue;
		}
	}
	if (true != true) {
		int epsjccfq;
		for (epsjccfq = 38; epsjccfq > 0; epsjccfq--) {
			continue;
		}
	}
	if (3610 == 3610) {
		int wrryjueq;
		for (wrryjueq = 16; wrryjueq > 0; wrryjueq--) {
			continue;
		}
	}
	return 34772;
}

bool pzxtmwp::hqagsklypvwyvrgudyu(int ixzhhys, bool kbckpq, int mhpqtgqgsj, string rscsrbu, bool ipxxlampssce) {
	bool dhfdfmkxnnj = false;
	double kyhrlnrfkf = 16686;
	string ywjznjdmegc = "jesrekyovokhwqyywmjosdlzytkmawdgxtpymbvengjodns";
	string hrpxnesbr = "kab";
	int qiijkyflj = 1681;
	int pwnygz = 6622;
	double bkqku = 90638;
	string baavp = "xplsmzbwwbsfzeafqxp";
	int kikejrzbhqow = 1526;
	bool mylrry = true;
	return false;
}

int pzxtmwp::otzztwleygpykjehormeqhwzk(string ouoljnlpazik, int wgcnaolgifkowih, int ayompkg, double vkpxneqruv, int uutcndrr, bool gmhfq, bool plositoemscqga) {
	string sxreylidokz = "yjwdfksrysrajrawcfuygbqcdjh";
	int roypvwhzvlogas = 3413;
	double taeoyvnr = 3348;
	string oruttfvrzb = "sgjmuwsqxgsxhfdktlznhaglxttdmurfstdxnvarayeozeejcorotfnu";
	string lkgflyfhcmiqqin = "ewkoydcsopafoeaoxtjjyhsjntetqeqzdbmrbblxetgfgqmzpczougqasxmswlfbyhkhpfbkhdmznz";
	bool onyhpafkce = false;
	bool xkbmmr = true;
	bool xplecekjdmmf = false;
	if (string("yjwdfksrysrajrawcfuygbqcdjh") == string("yjwdfksrysrajrawcfuygbqcdjh")) {
		int yryjvi;
		for (yryjvi = 86; yryjvi > 0; yryjvi--) {
			continue;
		}
	}
	if (string("yjwdfksrysrajrawcfuygbqcdjh") != string("yjwdfksrysrajrawcfuygbqcdjh")) {
		int dkyiznma;
		for (dkyiznma = 7; dkyiznma > 0; dkyiznma--) {
			continue;
		}
	}
	if (false == false) {
		int jdzcelh;
		for (jdzcelh = 87; jdzcelh > 0; jdzcelh--) {
			continue;
		}
	}
	return 85550;
}

void pzxtmwp::jjmhuivcytqgv(string nkugppi, string zkigga, int zzvmqtvokoajcz, bool mhagkazquwgz, bool nppmjsu, string jdpvayfyx, int tyioxcs, int tcbiuw, string ipielwczuloixj) {
	double rfwkcidqjbxrg = 17321;
	bool swjjhihehnymav = false;
	double hotozjatln = 38444;
	int fxpdjhyjsuxcm = 152;
	double kopiffsfiidmxu = 20763;
	string dmieswg = "fafbpqlzsxhmuzg";
	string laxgmyr = "jgwqgztdcrrizqlwudcoffhxbsftswfvomsvnjminjaztgvxxalrodcpzshyeddqnyrzblzpfqbkq";
	double mpbauxxpt = 48052;
	double ovovcew = 20332;
	if (17321 == 17321) {
		int fupzecnhfz;
		for (fupzecnhfz = 8; fupzecnhfz > 0; fupzecnhfz--) {
			continue;
		}
	}
	if (38444 == 38444) {
		int lwgluaviuf;
		for (lwgluaviuf = 30; lwgluaviuf > 0; lwgluaviuf--) {
			continue;
		}
	}
	if (string("jgwqgztdcrrizqlwudcoffhxbsftswfvomsvnjminjaztgvxxalrodcpzshyeddqnyrzblzpfqbkq") == string("jgwqgztdcrrizqlwudcoffhxbsftswfvomsvnjminjaztgvxxalrodcpzshyeddqnyrzblzpfqbkq")) {
		int zwmi;
		for (zwmi = 35; zwmi > 0; zwmi--) {
			continue;
		}
	}
	if (152 != 152) {
		int psmirgxa;
		for (psmirgxa = 44; psmirgxa > 0; psmirgxa--) {
			continue;
		}
	}

}

string pzxtmwp::cqcivvdvwcragrmyyvdob(int pynwhkqjt, string qcxbp, string wmntc, double ckyft, double atdhse, int wbvfrcxqkag) {
	int bksfadusrghwb = 3650;
	bool emkex = false;
	bool cehgnyjughlarra = false;
	string mkedwbxnz = "kfcjnhfwpbjaaqwueykwtfqpiwnvfqhvhezsdyglufeyklczlahi";
	bool ajrzuwieney = false;
	if (false != false) {
		int xa;
		for (xa = 14; xa > 0; xa--) {
			continue;
		}
	}
	if (3650 != 3650) {
		int udlz;
		for (udlz = 30; udlz > 0; udlz--) {
			continue;
		}
	}
	if (3650 != 3650) {
		int horbrh;
		for (horbrh = 4; horbrh > 0; horbrh--) {
			continue;
		}
	}
	if (false == false) {
		int gqs;
		for (gqs = 79; gqs > 0; gqs--) {
			continue;
		}
	}
	if (false == false) {
		int dnmbycvbgr;
		for (dnmbycvbgr = 79; dnmbycvbgr > 0; dnmbycvbgr--) {
			continue;
		}
	}
	return string("atkhqbvyjwtymaz");
}

int pzxtmwp::lkkumiticrzfkdkye(int ylpsavducmgzgr, int ehchynn, string dmeotzj, int bddngttxupot, bool bpnbufxuc, string tvzxz, bool hxjwxlmg, bool zayvdbjypkln) {
	double tjmbtnrifz = 9351;
	if (9351 == 9351) {
		int pvnzot;
		for (pvnzot = 54; pvnzot > 0; pvnzot--) {
			continue;
		}
	}
	if (9351 == 9351) {
		int ipjbwal;
		for (ipjbwal = 79; ipjbwal > 0; ipjbwal--) {
			continue;
		}
	}
	if (9351 == 9351) {
		int rhleve;
		for (rhleve = 24; rhleve > 0; rhleve--) {
			continue;
		}
	}
	if (9351 == 9351) {
		int ojhudvcth;
		for (ojhudvcth = 4; ojhudvcth > 0; ojhudvcth--) {
			continue;
		}
	}
	if (9351 != 9351) {
		int smyji;
		for (smyji = 28; smyji > 0; smyji--) {
			continue;
		}
	}
	return 30280;
}

double pzxtmwp::mqcbelgbbggksdo(string kjodjuqnxth, bool dfoyzxnavxevyjk, double gulbedyqw, string qjitaswyjpbve) {
	int uguqzf = 3690;
	string fuxwnquhzutxeh = "wttospfghlgbbzjxeywbcfcmaxchafkfxbxqmgbzmdqxxwmhtzhhomgzjpiqwysrfgkqlufalgmwmgyjmweyolctvdve";
	int bwtpjufdu = 3852;
	string rciiyshwcs = "rndslwxwdcszsfzmwdjxfrwqyezvagzwzhlawmjmcidxxwsi";
	int kntaetkdcampxb = 1966;
	int tydkzalk = 2066;
	bool wnwouh = true;
	bool qsxgvh = true;
	int hkhxjiogobe = 9416;
	bool loadpbuq = true;
	if (3690 != 3690) {
		int yspck;
		for (yspck = 60; yspck > 0; yspck--) {
			continue;
		}
	}
	if (true != true) {
		int qrkeppcltq;
		for (qrkeppcltq = 99; qrkeppcltq > 0; qrkeppcltq--) {
			continue;
		}
	}
	if (true != true) {
		int kzmoaabc;
		for (kzmoaabc = 15; kzmoaabc > 0; kzmoaabc--) {
			continue;
		}
	}
	if (2066 == 2066) {
		int oclopwdzlf;
		for (oclopwdzlf = 84; oclopwdzlf > 0; oclopwdzlf--) {
			continue;
		}
	}
	return 53327;
}

string pzxtmwp::bhdueoravehp(int xjxdlsfhyif, bool kvmgigisrnft, string mjcggztjkprtj) {
	string auuznbxjq = "wjsrimlpivtejzsi";
	double mzboovpdhau = 9273;
	double cxwwzxxhmz = 6686;
	bool xefhiagpayzca = false;
	double bqtvhhasrd = 5610;
	string bmqxyypbjgaaui = "qugllpmugklkqstpopvfbsyjnokregzfl";
	int vzgtdcoeg = 323;
	bool zooodxtdtvza = true;
	bool eknvmxwy = true;
	int kxwbzgzetlp = 1202;
	if (true == true) {
		int ecpj;
		for (ecpj = 92; ecpj > 0; ecpj--) {
			continue;
		}
	}
	if (6686 != 6686) {
		int fdvho;
		for (fdvho = 2; fdvho > 0; fdvho--) {
			continue;
		}
	}
	if (6686 != 6686) {
		int yodtqmn;
		for (yodtqmn = 33; yodtqmn > 0; yodtqmn--) {
			continue;
		}
	}
	if (false == false) {
		int lxfm;
		for (lxfm = 31; lxfm > 0; lxfm--) {
			continue;
		}
	}
	return string("jaejbdflirnxzxrcf");
}

double pzxtmwp::ueeaegltyyuxmvj(int qgvrjagifldlhq, string jksmy, int zfbupwhwxalefj, int carriwjupfdula, double vechicbjnst) {
	string ofqhlsgntgavi = "cqjltwyaij";
	string nlelbjtp = "nsfxcgn";
	string ebnhabbacncivy = "qrmvvxafvcejjalnmtixhfvkbvjthbncpsmfhrliohwsokwvhdmyhbwxpmblbsazkikmrovfmmkmnjpazzi";
	bool zhzvjlooqzwhv = true;
	int nsnusxwesiioeso = 399;
	bool zpkwcfduhhg = true;
	int zrtid = 5903;
	if (true != true) {
		int elzmwka;
		for (elzmwka = 97; elzmwka > 0; elzmwka--) {
			continue;
		}
	}
	if (string("nsfxcgn") == string("nsfxcgn")) {
		int pkffxuzbpq;
		for (pkffxuzbpq = 45; pkffxuzbpq > 0; pkffxuzbpq--) {
			continue;
		}
	}
	if (string("nsfxcgn") == string("nsfxcgn")) {
		int sjlctf;
		for (sjlctf = 68; sjlctf > 0; sjlctf--) {
			continue;
		}
	}
	if (399 != 399) {
		int yni;
		for (yni = 97; yni > 0; yni--) {
			continue;
		}
	}
	return 60318;
}

bool pzxtmwp::oilgrxtxtwhicnysqx(double ncvrtwxgrwd, string nqopzcmozitt) {
	return false;
}

bool pzxtmwp::zeinbdqwaywxnmgbmdzzs(double koqmkfqka) {
	double drguziltj = 517;
	double nlfhok = 8191;
	string huxvsiyluqhwnk = "isepinzggkqigoqchbzhubglbrmvcevnoacwoxnvqhskcjuynhzewfvohidmixjxotwjzkcnezrrykoisjcweofdigtaha";
	int trqpllwnvh = 2261;
	bool nweypqvzbw = false;
	int iltxamdexcjv = 1415;
	int cqcloyxayimz = 4740;
	if (4740 == 4740) {
		int dsccuppsda;
		for (dsccuppsda = 63; dsccuppsda > 0; dsccuppsda--) {
			continue;
		}
	}
	if (string("isepinzggkqigoqchbzhubglbrmvcevnoacwoxnvqhskcjuynhzewfvohidmixjxotwjzkcnezrrykoisjcweofdigtaha") != string("isepinzggkqigoqchbzhubglbrmvcevnoacwoxnvqhskcjuynhzewfvohidmixjxotwjzkcnezrrykoisjcweofdigtaha")) {
		int ck;
		for (ck = 63; ck > 0; ck--) {
			continue;
		}
	}
	if (string("isepinzggkqigoqchbzhubglbrmvcevnoacwoxnvqhskcjuynhzewfvohidmixjxotwjzkcnezrrykoisjcweofdigtaha") == string("isepinzggkqigoqchbzhubglbrmvcevnoacwoxnvqhskcjuynhzewfvohidmixjxotwjzkcnezrrykoisjcweofdigtaha")) {
		int asoo;
		for (asoo = 63; asoo > 0; asoo--) {
			continue;
		}
	}
	if (8191 == 8191) {
		int gazizrt;
		for (gazizrt = 19; gazizrt > 0; gazizrt--) {
			continue;
		}
	}
	if (2261 != 2261) {
		int ecmgah;
		for (ecmgah = 15; ecmgah > 0; ecmgah--) {
			continue;
		}
	}
	return true;
}

pzxtmwp::pzxtmwp() {
	this->jjmhuivcytqgv(string("vgqplvkipocyrglzueyydcwpeibdaxrfyiffwejlsakscujdjkgoczmeoqsadzyephghagqmtprtebhpkshx"), string("zszjqdezffdfrjiornvbpvdrapdjpebymatgqhrt"), 2016, true, false, string("exhayeoeunhghdihsmvqlflynqogsqsvykapnmvvxpyqycinmjoidhjgoxttajcyieyfnktpksxcfevikeufbagjmt"), 2371, 5821, string("ifwzxgpkuunkagirmflwbgusoxmcqtgptzjdbjluraweqivoxxtastixqxqcbxaiulaaihryrvitlvgndlccloamrhic"));
	this->cqcivvdvwcragrmyyvdob(1534, string("oemxeoonymbysaccjnracqizhgsdxolredsmjqhcoremhrafpudfnnvyfdavmrnoaxugqutlalgqrduybmjajcfsqkjdhznb"), string("irslifgtyuletwtfzgvnuuurbsylew"), 21078, 20097, 430);
	this->lkkumiticrzfkdkye(4436, 259, string("eckdrwtbiperxqwkvbnhdtxks"), 447, true, string("azcwsqtwkpcmtlbilwbnlvespnbgdraxlxzauypifwfjtpytatnnqdoqwitsmvoxzko"), true, false);
	this->mqcbelgbbggksdo(string("nrsaiigogiflxtubloveubn"), true, 6949, string("icgtuyejdjrbp"));
	this->bhdueoravehp(3621, true, string("xxpjkhkonjfizdfpzaznbodjslmyaxqajufttrbexitrbdnfrbuhvloekdzkrrfvwvtsojpgpfrn"));
	this->ueeaegltyyuxmvj(1698, string("vgcijrpbrenfdduqihiatoigm"), 2160, 601, 10607);
	this->oilgrxtxtwhicnysqx(703, string("wjcumcsahtbjacvvcoqbbezuckrkdpcdldkozopik"));
	this->zeinbdqwaywxnmgbmdzzs(1192);
	this->zvyaauzmvdnakwrilnu(string("ertuxjohfedojsofauws"), 5699, 16523, 1308, string("pfydjrcyweiwbiedbhgmduukurdfdpadetqfpqnwc"), 19634, false, 21249, 1664, 404);
	this->urkyyulucdez(true, false, true, false);
	this->hauswtsvmdayacuo();
	this->hqagsklypvwyvrgudyu(2591, true, 1741, string("dmfbywtquuwuezsspcgbkdlyjqergxpkgqbysllglsgsdocddlnddxywfhtqp"), false);
	this->otzztwleygpykjehormeqhwzk(string("gbfwtkwtltpwvxzouicmzvvhywgieywvwscnybwdbytzqzgtrosnafqjuejdhkznyumbullbtx"), 1302, 3727, 5142, 1431, true, false);
	this->sbfmtardpw(false, string("tnpjodqoeuhgizjmrkkytzafhtwfgrofuhe"), 2566, string("gfqroylbjwiosnumxy"), 28796);
	this->rumrmtrcqxbsrxth(string("ybadldllebfomtjlohmpwgnpvmzbnljysfuoxfhgnzxvxsixfjpuvbqgshfdhxqxfbmtpayhywsl"), string("sqdadfcefbzxyrrsruravkwfii"), 13765, 47335, string("vabgkpllnxcvdsgnabzqbknqwkgojorflhhnjrxeqrqkgjqdvujenlkeqgxkokuu"), 41779, 1688, 1744);
	this->mrdysetxerxlgslcqsjrxrcg(string("bgseblritdlqgvdmwiqdifhjevewgsujfpmduxalwrjreehagnzlhdbxxfxwqxjvgnsnxnx"), 842, 725, string("xsdjezteciualzwrpazfmfbkplgygnmsjxutqoimhegmhndqcupeycczlilucidpuqj"), 17266, string("otqomywaxxkxjhhjvxsnvbespzzdygeaem"), string("iymbntvikzmvmugnupfplcnitkgwakuwoagltibvjsgimfbxlrhfrrzjlzayaxxqocntnvgkvapmdxtbvlvkp"), 5456, 47093, string("gfyzzeasdlajfcgcmdhgfnvcqqh"));
	this->azzqojkhlrdxvdsknyfgrfdl(1304, 4486, 30006, string("rmalxoywuorpowabsshbknpcplgygwxdwrtobdtmbt"), string("sjoynncmsddihqebmdlmws"), 10437, string("viinzrjywfgxmwftrodvccicsyvcqaaeatutwiqdatzyilsgkokpmtnkgpb"), 34873);
	this->fyczjzhxwfngkifuapzxgmbfo(67061, false, string("bdvalmhcqwzurxqbthgzqoggxsyatbgdygngozt"), true);
}

                                               

















































































































































































































































































