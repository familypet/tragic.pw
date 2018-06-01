#pragma once
#include <set>
#include <map>
#include <unordered_map>

extern void RenderInterface();
struct skinInfo
{
	int seed = -1;
	int paintkit;
	std::string tagName;
};

struct Variables
{
	Variables()
	{

	}
	bool HWIDPassed;
	bool Unload;
	bool Exploit;
	bool clantag;
	bool obs;
	bool Dark = true;

	char ConfigName[52];
	char Tag[32] = "";
	char Name[32] = "";

	struct Ragebot_s
	{
		bool	MainSwitch;
		bool 	Enabled;
		bool 	AutoFire;
		float 	FOV;
		bool 	Silent;
		bool	AutoPistol;

		int LegitChoiceList;
		bool LegitAA;
		int		KeyPress;
		bool	AimStep;

		//AA Builder 
		//THESE NOW BECOME WHAT I WILL SET DEPENDING ON THE MVOEMENT TYPE EXACMPLE: IF WERE WALKING { JITTER = WALK_JITTER; } //LESS CODE!
		bool	BuilderAAs;
		bool	Jitter;
		bool	FJitter;
		bool	LBYBreaker;
		float	BuilderReal;
		float	BuilderFake;
		float	BuilderPitch;
		float	JitterRange;
		float	FJitterRange;


		//walking builder aa's
		bool	walk_BuilderAAs;
		bool	walk_Jitter;
		bool	walk_FJitter;
		bool	walk_LBYBreaker;
		float	walk_BuilderReal;
		float	walk_BuilderFake;
		float	walk_BuilderPitch;
		float	walk_JitterRange;
		float	walk_FJitterRange;

		//running builder aa's
		bool	run_BuilderAAs;
		bool	run_Jitter;
		bool	run_FJitter;
		bool	run_LBYBreaker;
		float	run_BuilderReal;
		float	run_BuilderFake;
		float	run_BuilderPitch;
		float	run_JitterRange;
		float	run_FJitterRange;


		//crouching builder aa's
		bool	crouch_BuilderAAs;
		bool	crouch_Jitter;
		bool	crouch_FJitter;
		bool	crouch_LBYBreaker;
		float	crouch_BuilderReal;
		float	crouch_BuilderFake;
		float	crouch_BuilderPitch;
		float	crouch_JitterRange;
		float	crouch_FJitterRange;


		//fakewalk builder aa's
		bool	fwalk_BuilderAAs;
		bool	fwalk_Jitter;
		bool	fwalk_FJitter;
		bool	fwalk_LBYBreaker;
		float	fwalk_BuilderReal;
		float	fwalk_BuilderFake;
		float	fwalk_BuilderPitch;
		float	fwalk_JitterRange;
		float	fwalk_FJitterRange;


		//standing builder aa's
		bool	stand_BuilderAAs;
		bool	stand_Jitter;
		bool	stand_FJitter;
		bool	stand_LBYBreaker;
		float	stand_BuilderReal;
		float	stand_BuilderFake;
		float	stand_BuilderPitch;
		float	stand_JitterRange;
		float	stand_FJitterRange;


		int flip_aa;

		bool ayywarecrasher = false;

		bool	EnabledAntiAim;
		int		SubAATabs;
		bool	PreAAs;
		int		Pitch;
		int		YawTrue;
		int		YawFake;
		int		YawTrueMove;
		float	PitchAdder;
		float	YawTrueAdder;
		float	YawFakeAdder;
		float	YawFakeMove;
		bool	AtTarget;
		bool	Edge;
		bool	KnifeAA;
		bool	FakeLag;
		int		FakeLagAmt;

		// Pitch  PitchAdder  YawTrue  YawFakeAdder  YawFake  YawFakeAdder

		//walking prebuilt aa
		bool	walk_PreAAs;
		int		walk_Pitch;
		float	walk_PitchAdder;
		int		walk_YawTrue;
		float	walk_YawTrueAdder;
		int		walk_YawFake;
		float	walk_YawFakeAdder;


		//running prebuilt aa
		bool	run_PreAAs;
		int		run_Pitch;
		float	run_PitchAdder;
		int		run_YawTrue;
		float	run_YawTrueAdder;
		int		run_YawFake;
		float	run_YawFakeAdder;


		//crouching prebuilt aa
		bool	crouch_PreAAs;
		int		crouch_Pitch;
		float	crouch_PitchAdder;
		int		crouch_YawTrue;
		float	crouch_YawTrueAdder;
		int		crouch_YawFake;
		float	crouch_YawFakeAdder;


		//fakewalk prebuilt aa fwalk
		bool	fwalk_PreAAs;
		int		fwalk_Pitch;
		float	fwalk_PitchAdder;
		int		fwalk_YawTrue;
		float	fwalk_YawTrueAdder;
		int		fwalk_YawFake;
		float	fwalk_YawFakeAdder;


		//standing prebuilt aa's 
		bool	stand_PreAAs;
		int		stand_Pitch;
		float	stand_PitchAdder;
		int		stand_YawTrue;
		float	stand_YawTrueAdder;
		int		stand_YawFake;
		float	stand_YawFakeAdder;


		bool stand_allowflip;
		bool fwalk_allowflip;
		bool crouch_allowflip;
		bool walk_allowflip;
		bool run_allowflip;

		int flipkey;

		bool fakehead;

		bool walk_fakehead;
		bool run_fakehead;
		bool crouch_fakehead;
		bool fwalk_fakehead;
		bool stand_fakehead;

		int error_type;  //aa warning variable

		bool	FriendlyFire;
		int		Hitbox;
		int		Hitscan;
		float	Pointscale;
		bool	Multipoint;
		float	Multipoints;

		bool	AntiRecoil;
		bool	AutoWall;
		bool	AutoStop;
		int		autostopkey;
		bool	AutoCrouch;
		bool	AutoScope;
		float	MinimumDamageSniper;
		float	MinimumDamageRifle;
		float	MinimumDamagePistol;
		float	MinimumDamageHeavy;
		float	MinimumDamageSmg;
		float	MinimumDamageRevolver;
		bool	Hitchance;
		float	HitchanceSniper;
		float	HitchancePistol;
		float	HitchanceRifle;
		float	HitchanceHeavy;
		float	HitchanceSmgs;
		float	HitchanceRevolver;
		int		Resolver;
		bool	Resolver3;

		bool	FakeLagFix;
		bool	PosAdjust;
		bool	playerlist;
		int		BAIMkey;
		float	bruteAfterX;


		bool	MovingAA; //eventually get rid of this

						  //and replace with this
		bool	AA_onRun;
		bool	AA_onWalk;
		bool	AA_onCrouch;
		bool	AA_onFakeWalk;
		bool	AA_onStand;
	} Ragebot;
	struct
	{
		bool Enabled;

		int knifemodel;

		int glove;
		int gloeskin;


		struct
		{
			bool ChangerEnabled = false;
			int ChangerSkin = 0;
			char ChangerName[32] = "";
			bool stat;
			int statamount;
			bool customstat;
			int customstatamount;
			char Name;
		} W[519];

		std::unordered_map<std::string, std::set<std::string>> weaponSkins;
		std::unordered_map<std::string, skinInfo> skinMap;
		std::unordered_map<std::string, std::string> skinNames;
		std::vector<std::string> weaponNames;
	} Skinchanger2;
	struct
	{
		bool legitresolver;
		bool MainSwitch;
		bool backtrack;
		float backtrackTicks;
		bool AutoPistol;

		bool showBacktrack;
		int LegitChoiceList;
		bool LegitAA;

		struct
		{
			bool 	Enabled;
			bool AutoPistol;
			bool	Resolver;

		} Aimbot;

		int MainKey = 1;
		float MainSmooth = 1;
		float Mainfov;
		float main_random_smooth;
		float main_recoil_min;
		float main_recoil_max;
		float main_randomized_angle;
		bool main_silent;



		int PistolKey = 1;
		float Pistolfov;
		float PistolSmooth = 1;;
		float pistol_random_smooth;
		float pistol_recoil_min;
		float pistol_recoil_max;
		float pistol_randomized_angle;
		bool pistol_autoshoot;
		bool pistol_silent;

		int SniperKey = 1;
		float Sniperfov;
		float SniperSmooth = 1;
		float sniper_random_smooth;
		float sniper_recoil_min;
		float sniper_recoil_max;
		float sniper_randomized_angle;


		int DeagleKey = 1;
		float Deaglefov;
		float DeagleSmooth = 1;
		float Deagle_random_smooth;
		float Deagle_recoil_min;
		float Deagle_recoil_max;
		float Deagle_randomized_angle;

		int smg_Key = 1;
		float smg_fov;
		float smg_Smooth = 1;
		float smg_random_smooth;
		float smg_recoil_min;
		float smg_recoil_max;
		float smg_randomized_angle;

		int heavy_wp_Key = 1;
		float heavy_wp_fov;
		float heavy_wp_Smooth;
		float heavy_wp_random_smooth;
		float heavy_wp_recoil_min;
		float heavy_wp_recoil_max;
		float heavy_wp_randomized_angle;


		struct
		{
			bool Headwpmain;
			bool Neckwpmain;
			bool Chestwpmain;
			bool Stomachwpmain;
			bool multiboneswpmain;
		}FilterMainWeapons;

		struct
		{
			bool Headwppis;
			bool Neckwppis;
			bool Chestwppis;
			bool Stomachwppis;
			bool multiboneswppis;
		}FilterPistolWeapons;


		struct
		{
			bool HeadwpSnip;
			bool NeckwpSnip;
			bool ChestwpSnip;
			bool StomachwpSnip;
			bool multiboneswpSnip;

		}FilterSniperWeapons;


		struct
		{
			bool Headwpsmg;
			bool Neckwpsmg;
			bool Chestwpsmg;
			bool Stomachwpsmg;
			bool multiboneswpsmg;

		}FiltersmgWeapons;


		struct
		{
			bool Headwphv;
			bool Neckwphv;
			bool Chestwphv;
			bool Stomachwphv;
			bool multibonewphv;

		}FilterheavyWeapons;
		
		struct
		{
			bool HeadwpDeagle;
			bool NeckwpDeagle;
			bool ChestwpDeagle;
			bool StomachwpDeagle;
			bool multiboneswpDeagle;

		}FilterDeagleWeapons;

	

		bool SmokeCheck;
		bool magnetaim;
		struct
		{
			bool Head;
			bool Arms;
			bool Chest;
			bool Stomach;
			bool Legs;
		}Filter;


		int scoutKey = 1;
		float scoutfov;
		float scoutSmooth;
		float scout_random_smooth;
		float scout_recoil_min;
		float scout_recoil_max;
		float scout_randomized_angle;

		struct
		{
			bool HeadwpScout;
			bool NeckwpScout;
			bool ChestwpScout;
			bool StomachwpScout;
			bool multiboneswpScout;

		}FilterScoutWeapons;


		struct
		{
			bool	Enabled;
			float Delay;
			int Key = 6;
			float hitchance;
			struct
			{
				bool Head;
				bool Arms;
				bool Chest;
				bool Stomach;
				bool Legs;
			} Filter;

		} Triggerbot;

	} LegitBot;
	struct
	{
		bool backtrackenable;
		bool backtrackhistory;
		int  backtrackticks = 1;


	}Backtrack;
	struct 
	{
		bool 	Enabled;
		bool TimeClanTag;
		int ToggleKey;
		bool ToggleCheck;
		bool Box;
		bool Name;
		bool HP;
		bool Weapon;
		bool Info;
		bool Chams;
		bool Skeleton;
		bool DrawSkeleton;
		bool AimSpot;
		bool chamsHp;
		bool SniperCrosshair;
		bool FakeAngleChams;

		bool InverseView;
		bool armor;
		bool Radar;
		bool DLight;
		bool SpreadCrosshair;
		bool GrenadeESP;
		bool C4;
		bool backtrackline;
		bool RecoilCrosshair;
		bool Glow;
		int Smoke;
		//DISSECTED FROM FROSTY
		//Variables.h
		bool demageidicator;
		bool bulletshow;
		float flTracersDuration;
		float flTracersWidth;
		float BulletTraceColor[3] = { 0.f, 0.f, 0.f };

		bool IsScoped;
		bool Reloading;
		bool HasDefuser;
		bool NoVisualRecoil;
		int Hands;
		int Weapons;
		float FOVChanger;
		float viewmodelChanger;
		bool NoFlash;
		bool NoSmoke;
		int TPKey;
		float tpdist = 200;
		bool ThirdPerson;
		int antiaim_thirdperson_angle;

		bool Time;
		bool Flashed;
		bool money;
		bool C4World;
		bool WeaponsWorld;
		bool noscopeborder;
		bool GrenadePrediction;
		bool Snaplines;

		bool handcham;
		bool weaponviewcham;
		int chamhandalpha;

		int weaponmat;
		int handmat;
		int weaponhandalpha;
		bool GhostChams;


		bool ArrowAA;
		int SceneChams;

		int matierial;
		bool outline;
		bool Teamchams;
		int champlayeralpha;
		bool LBYIndicator;
		bool AAindicator;
		bool Chamweapon;
		bool XQZ;
		float wallalpha = 1.f;
		float propalpha = 1.f;
		float modelalpha = 1.f;

		int HitmarkerSound;
		bool backtrack;
		float backtrackTicks;
		bool AutoPistol;
		bool showBacktrack;
		int LegitChoiceList;
		bool LegitAA;
		bool DrawAwall;
		bool radar;
		int BoxHotkey;;
		int SkeleHotkey;
		int RadarHotkey;
		bool bBlockbot;
		int iBlockbotKey;
		bool angleLines;

		bool inventoryalwayson;
		int SkyboxChanger;
		bool Hitbox;
		float HitboxDuration;
		struct
		{
			bool Players;
			bool EnemyOnly;
		} Filter;

		struct
		{
			bool Players;
			bool EnemyOnly;
			bool Weapons;
			bool DynamicBox;
		} GlowFilter;

	} Visuals;

	struct misc_s
	{

		bool Miscrun;
		bool TimeClanTag;
		int customodelst;
		int customodelsct;
		bool PurchaseInfo;
		bool DmgInfo;
		bool inventoryalwayson;
		bool antiuntrusted = true;
		bool Radar;
		bool Ragdoll;

		bool silentstealer;
		int ragequit;
		int 	Bhop;
		bool 	AutoStrafe;
		float MinVel;
		int		AirStuckKey;
		int lagexploit;
		int lagexploitmultiplier = 3;
		float lagexploitvalue;
		float FakeLag;
		bool AdaptiveFakeLag;
		bool nightMode;
		int NameSpammer;
		bool NameChangerFix;
		bool NoName;
		int		ChatSpamMode;
		bool ClantagChanger;
		int ClanTagSpeed;
		bool syncclantag;

		int fakewalkkey;
		bool fakewalk = false;
		int flashlight;
		bool familyclantag;
		bool familyclantag2;
		bool heartclantag;
		bool Clantags;
		int clantag_SLN;
		int clantags;
		bool Snaplines;
		bool SpecList;
		bool FPSBooster;
		int SkyBoxChanger;
		bool namespam;
		int spammer;
		int AutoDefuse;
		bool Spam;
		bool isRecording;
		bool isReplaying;
		bool RecordPath;
		bool AutoAccept;
		bool mmprediciton;
		bool afkbot;
		bool    LeftHand1;
		bool bFreeCam;
		int iFreeCamKey;
		float flFreeCamSpeed;
		bool SpoofConfirmation = false;
		bool animatedclantag = false;
		int customtab;
		bool fps;
		bool ServerRankRevealAll;
		bool niggatest;
		bool Hitmarker;
		int hitsound;
		int HitMarkerColor;
		bool Watermark;
		int Skybox;
		int CurrentSky;
		bool bought;
		bool FakePing;
		int FakePing2;
		bool Gray;
		bool crack;
		bool chromatic;
		bool eventlogs;
		bool eventlogs2;
		bool bBlockbot;
		int iBlockbotKey;

	} Misc;
	struct
	{

		bool Enabled;
		int Knife;
		int gloves;
		int KnifeSkin;
		int AK47Skin;
		int GalilSkin;
		int M4A1SSkin;
		int M4A4Skin;
		int AUGSkin;
		int FAMASSkin;
		int AWPSkin;
		int SSG08Skin;
		int SCAR20Skin;
		int P90Skin;
		int Mp7Skin;
		int NovaSkin;
		int UMP45Skin;
		int GlockSkin;
		int SawedSkin;
		int USPSkin;
		int MagSkin;
		int XmSkin;
		int DeagleSkin;
		int DualSkin;
		int FiveSkin;
		int RevolverSkin;
		int Mac10Skin;
		int tec9Skin;
		int Cz75Skin;
		int NegevSkin;
		int M249Skin;
		int Mp9Skin;
		int P2000Skin;
		int BizonSkin;
		int Sg553Skin;
		int P250Skin;
		int G3sg1Skin;
		bool activeknife;


		int knifemodel;

		int glove;
		int gloeskin;


		struct
		{
			bool ChangerEnabled = false;
			int ChangerSkin = 0;
			char ChangerName[32] = "";
			bool stat;
			int statamount;
			bool customstat;
			int customstatamount;
			char Name;
		} W[519];

		std::unordered_map<std::string, std::set<std::string>> weaponSkins;
		std::unordered_map<std::string, skinInfo> skinMap;
		std::unordered_map<std::string, std::string> skinNames;
		std::vector<std::string> weaponNames;
	} Skinchanger;

	struct
	{
		bool	Opened = true;
		int 	Key;
		bool	Ragebot = false;
		bool	Legitbot = false;
		bool	Visual = false;
		bool	Misc = false;
		int		ConfigFile = 0;
		int		Theme = 0;
		bool	Colors = false;
		int     menu_hitbox_select;
		int		iTab;
		bool	bShowTabs;
	} Menu;

	struct
	{
		float TeamESP[3] = { 0, 1.f, 0 };
		float EnemyESP[3] = { 1.f, 0, 0 };
		float TeamDLights[3] = { 0, 1.f, 0 };
		float EnemyDLights[3] = { 1.f, 0, 0 };
		float EnemyGlow[3] = { 1.f, 0, 0 };
		float TeamGlow[3] = { 0, 1.f, 0 };
		float WeaponGlow[3] = { 1.f, 0, 0 };
		float C4glow[3] = { 1.f, 0, 0 };
		float EnemyChamsVis[3] = { 1.f, 0, 0 };
		float EnemyChamsNVis[3] = { 1.f, 0, 0 };
		float TeamChamsVis[3] = { 0, 1.f, 0 };
		float TeamChamsNVis[3] = { 0, 1.f, 0 };
		float WeaponCham[3] = { 1.f, 0, 0 };
		float HandCham[3] = { 1.f, 0, 0 };
		float WeaponViewCham[3] = { 1.f, 0, 0 };
		float hands_color[3] = { 1.f, 0, 0 };
		float hands_alpha;
		float tracer_color[3] = { 1.f, 0, 0 };
		float box_color_t[3] = { 1.f, 0, 0 };
		float box_color_ct[3] = { 1.f, 0, 0 };
		float color_skeleton[3] = { 1.f, 1.f, 1.f };
		float color_spread[3] = { 0, 1.f, 0 };
		float color_recoil[3] = { 0, 0, 1.f };
		float dlight_color[3] = { 0, 1.f, 0 };
		float hitmark_color[3] = { 0, 0, 1.f };
		float backtrackdots_color[3] = { 1.f, 0, 0 };
		float window_bg[3] = { 0.15f, 0.14f, 0.15f };
	}Colors;
};

extern Variables g_Options;