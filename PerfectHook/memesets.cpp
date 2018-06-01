#include "memesets.h"
#include "SDK.h"


void Offsetz::GetNetvars()
{
    NetVarManager->Initialize();
	offsetz.DT_BasePlayer.m_Local = NetVarManager->GetOffset("DT_BasePlayer", "m_Local");
	offsetz.DT_BasePlayer.m_aimPunchAngle = NetVarManager->GetOffset("DT_BasePlayer", "m_aimPunchAngle");
	offsetz.DT_BasePlayer.m_aimPunchAngleVel = NetVarManager->GetOffset("DT_BasePlayer", "m_aimPunchAngleVel");
	offsetz.DT_BasePlayer.m_viewPunchAngle = NetVarManager->GetOffset("DT_BasePlayer", "m_viewPunchAngle");
	offsetz.DT_BasePlayer.m_vecViewOffset = NetVarManager->GetOffset("DT_BasePlayer", "m_vecViewOffset[0]");
	offsetz.DT_BasePlayer.m_nTickBase = NetVarManager->GetOffset("DT_BasePlayer", "m_nTickBase");
	offsetz.DT_BasePlayer.m_vecVelocity = NetVarManager->GetOffset("DT_BasePlayer", "m_vecVelocity[0]");
	offsetz.DT_BasePlayer.m_iHealth = NetVarManager->GetOffset("DT_BasePlayer", "m_iHealth");
	offsetz.DT_BasePlayer.m_iMaxHealth = NetVarManager->GetOffset("DT_BasePlayer", "m_iMaxHealth");
	offsetz.DT_BasePlayer.m_lifeState = NetVarManager->GetOffset("DT_BasePlayer", "m_lifeState");
	offsetz.DT_BasePlayer.m_fFlags = NetVarManager->GetOffset("DT_BasePlayer", "m_fFlags");
	offsetz.DT_BasePlayer.m_iObserverMode = NetVarManager->GetOffset("DT_BasePlayer", "m_iObserverMode");
	offsetz.DT_BasePlayer.m_hObserverTarget = NetVarManager->GetOffset("DT_BasePlayer", "m_hObserverTarget");
	offsetz.DT_BasePlayer.m_hViewModel = NetVarManager->GetOffset("DT_BasePlayer", "m_hViewModel[0]");
	offsetz.DT_BasePlayer.m_szLastPlaceName = NetVarManager->GetOffset("DT_BasePlayer", "m_szLastPlaceName");
	offsetz.DT_BasePlayer.deadflag = NetVarManager->GetOffset("DT_BasePlayer", "deadflag");

	offsetz.DT_BaseEntity.m_flAnimTime = NetVarManager->GetOffset("DT_BaseEntity", "m_flAnimTime");
	offsetz.DT_BaseEntity.m_flSimulationTime = NetVarManager->GetOffset("DT_BaseEntity", "m_flSimulationTime");
	offsetz.DT_BaseEntity.m_vecOrigin = NetVarManager->GetOffset("DT_BaseEntity", "m_vecOrigin");
	offsetz.DT_BaseEntity.m_angRotation = NetVarManager->GetOffset("DT_BaseEntity", "m_angRotation");
	offsetz.DT_BaseEntity.m_nRenderMode = NetVarManager->GetOffset("DT_BaseEntity", "m_nRenderMode");
	offsetz.DT_BaseEntity.m_iTeamNum = NetVarManager->GetOffset("DT_BaseEntity", "m_iTeamNum");
	offsetz.DT_BaseEntity.m_MoveType = offsetz.DT_BaseEntity.m_nRenderMode + 1;
	offsetz.DT_BaseEntity.m_Collision = NetVarManager->GetOffset("DT_BaseEntity", "m_Collision");
	offsetz.DT_BaseEntity.m_bSpotted = NetVarManager->GetOffset("DT_BaseEntity", "m_bSpotted");
	offsetz.DT_BaseEntity.m_vecMins = NetVarManager->GetOffset("DT_BaseEntity", "m_vecMins");
	offsetz.DT_BaseEntity.m_vecMaxs = NetVarManager->GetOffset("DT_BaseEntity", "m_vecMaxs");
	offsetz.DT_BaseEntity.m_nSolidType = NetVarManager->GetOffset("DT_BaseEntity", "m_nSolidType");
	offsetz.DT_BaseEntity.m_usSolidFlags = NetVarManager->GetOffset("DT_BaseEntity", "m_usSolidFlags");
	offsetz.DT_BaseEntity.m_nSurroundType = NetVarManager->GetOffset("DT_BaseEntity", "m_nSurroundType");
	offsetz.DT_BaseEntity.m_hOwner = NetVarManager->GetOffset("DT_BaseEntity", "m_hOwner");
	offsetz.DT_BaseViewModel.m_hOwner = NetVarManager->GetOffset("DT_BaseViewModel", "m_hOwner");

    offsetz.DT_BaseCombatCharacter.m_hActiveWeapon = NetVarManager->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	offsetz.DT_BaseCombatCharacter.m_hMyWeapons = NetVarManager->GetOffset("DT_BaseCombatCharacter", "m_hMyWeapons") / 2;
	offsetz.DT_BaseCombatCharacter.m_hMyWearables = NetVarManager->GetOffset("DT_BaseCombatCharacter", "m_hMyWearables");

	offsetz.DT_PlayerResource.m_iPing = NetVarManager->GetOffset("DT_PlayerResource", "m_iPing");
	offsetz.DT_PlayerResource.m_iKills = NetVarManager->GetOffset("DT_PlayerResource", "m_iKills");
	offsetz.DT_PlayerResource.m_iAssists = NetVarManager->GetOffset("DT_PlayerResource", "m_iAssists");
	offsetz.DT_PlayerResource.m_iDeaths = NetVarManager->GetOffset("DT_PlayerResource", "m_iDeaths");
	offsetz.DT_PlayerResource.m_bConnected = NetVarManager->GetOffset("DT_PlayerResource", "m_bConnected");
	offsetz.DT_PlayerResource.m_iTeam = NetVarManager->GetOffset("DT_PlayerResource", "m_iTeam");
	offsetz.DT_PlayerResource.m_iPendingTeam = NetVarManager->GetOffset("DT_PlayerResource", "m_iPendingTeam");
	offsetz.DT_PlayerResource.m_bAlive = NetVarManager->GetOffset("DT_PlayerResource", "m_bAlive");
	offsetz.DT_PlayerResource.m_iHealth = NetVarManager->GetOffset("DT_PlayerResource", "m_iHealth");

	offsetz.DT_CSPlayerResource.m_iPlayerC4 = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iPlayerC4");
	offsetz.DT_CSPlayerResource.m_iPlayerVIP = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iPlayerVIP");
	offsetz.DT_CSPlayerResource.m_bHostageAlive = NetVarManager->GetOffset("DT_CSPlayerResource", "m_bHostageAlive");
	offsetz.DT_CSPlayerResource.m_isHostageFollowingSomeone = NetVarManager->GetOffset("DT_CSPlayerResource", "m_isHostageFollowingSomeone");
	offsetz.DT_CSPlayerResource.m_iHostageEntityIDs = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iHostageEntityIDs");
	offsetz.DT_CSPlayerResource.m_bombsiteCenterB = NetVarManager->GetOffset("DT_CSPlayerResource", "m_bombsiteCenterB");
	offsetz.DT_CSPlayerResource.m_hostageRescueX = NetVarManager->GetOffset("DT_CSPlayerResource", "m_hostageRescueX");
	offsetz.DT_CSPlayerResource.m_hostageRescueY = NetVarManager->GetOffset("DT_CSPlayerResource", "m_hostageRescueY");
	offsetz.DT_CSPlayerResource.m_hostageRescueZ = NetVarManager->GetOffset("DT_CSPlayerResource", "m_hostageRescueZ");
	offsetz.DT_CSPlayerResource.m_iMVPs = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iMVPs");
	offsetz.DT_CSPlayerResource.m_iArmor = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iArmor");
	offsetz.DT_CSPlayerResource.m_bHasHelmet = NetVarManager->GetOffset("DT_CSPlayerResource", "m_bHasHelmet");
	offsetz.DT_CSPlayerResource.m_bHasDefuser = NetVarManager->GetOffset("DT_CSPlayerResource", "m_bHasDefuser");
	offsetz.DT_CSPlayerResource.m_iScore = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iScore");
	offsetz.DT_CSPlayerResource.m_iCompetitiveRanking = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iCompetitiveRanking");
	offsetz.DT_CSPlayerResource.m_iCompetitiveWins = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iCompetitiveWins");
	offsetz.DT_CSPlayerResource.m_iCompTeammateColor = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iCompTeammateColor");
	offsetz.DT_CSPlayerResource.m_bControllingBot = NetVarManager->GetOffset("DT_CSPlayerResource", "m_bControllingBot");
	offsetz.DT_CSPlayerResource.m_iControlledPlayer = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iControlledPlayer");
	offsetz.DT_CSPlayerResource.m_iControlledByPlayer = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iControlledByPlayer");
	offsetz.DT_CSPlayerResource.m_iBotDifficulty = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iBotDifficulty");
	offsetz.DT_CSPlayerResource.m_szClan = NetVarManager->GetOffset("DT_CSPlayerResource", "m_szClan");
	offsetz.DT_CSPlayerResource.m_iTotalCashSpent = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iTotalCashSpent");
	offsetz.DT_CSPlayerResource.m_iCashSpentThisRound = NetVarManager->GetOffset("DT_CSPlayerResource", "m_iCashSpentThisRound");
	offsetz.DT_CSPlayerResource.m_nEndMatchNextMapVotes = NetVarManager->GetOffset("DT_CSPlayerResource", "m_nEndMatchNextMapVotes");
	offsetz.DT_CSPlayerResource.m_bEndMatchNextMapAllVoted = NetVarManager->GetOffset("DT_CSPlayerResource", "m_bEndMatchNextMapAllVoted");
	offsetz.DT_CSPlayerResource.m_nActiveCoinRank = NetVarManager->GetOffset("DT_CSPlayerResource", "m_nActiveCoinRank");
	offsetz.DT_CSPlayerResource.m_nMusicID = NetVarManager->GetOffset("DT_CSPlayerResource", "m_nMusicID");
	offsetz.DT_CSPlayerResource.m_nPersonaDataPublicLevel = NetVarManager->GetOffset("DT_CSPlayerResource", "m_nPersonaDataPublicLevel");
	offsetz.DT_CSPlayerResource.m_nPersonaDataPublicCommendsLeader = NetVarManager->GetOffset("DT_CSPlayerResource", "m_nPersonaDataPublicCommendsLeader");
	offsetz.DT_CSPlayerResource.m_nPersonaDataPublicCommendsTeacher = NetVarManager->GetOffset("DT_CSPlayerResource", "m_nPersonaDataPublicCommendsTeacher");
	offsetz.DT_CSPlayerResource.m_nPersonaDataPublicCommendsFriendly = NetVarManager->GetOffset("DT_CSPlayerResource", "m_nPersonaDataPublicCommendsFriendly");

	offsetz.DT_PlantedC4.m_bBombTicking = NetVarManager->GetOffset("DT_PlantedC4", "m_bBombTicking");
	offsetz.DT_PlantedC4.m_flC4Blow = NetVarManager->GetOffset("DT_PlantedC4", "m_flC4Blow");
	offsetz.DT_PlantedC4.m_bBombDefused = NetVarManager->GetOffset("DT_PlantedC4", "m_bBombDefused");
	offsetz.DT_PlantedC4.m_hBombDefuser = NetVarManager->GetOffset("DT_PlantedC4", "m_hBombDefuser");
	offsetz.DT_PlantedC4.m_flDefuseCountDown = NetVarManager->GetOffset("DT_PlantedC4", "m_flDefuseCountDown");

	offsetz.DT_CSPlayer.m_iShotsFired = NetVarManager->GetOffset("DT_CSPlayer", "m_iShotsFired");
	offsetz.DT_CSPlayer.m_angEyeAngles[0] = NetVarManager->GetOffset("DT_CSPlayer", "m_angEyeAngles[0]");
	offsetz.DT_CSPlayer.m_angEyeAngles[1] = NetVarManager->GetOffset("DT_CSPlayer", "m_angEyeAngles[1]");
	offsetz.DT_CSPlayer.m_iAccount = NetVarManager->GetOffset("DT_CSPlayer", "m_iAccount");
	offsetz.DT_CSPlayer.m_totalHitsOnServer = NetVarManager->GetOffset("DT_CSPlayer", "m_totalHitsOnServer");
	offsetz.DT_CSPlayer.m_ArmorValue = NetVarManager->GetOffset("DT_CSPlayer", "m_ArmorValue");
	offsetz.DT_CSPlayer.m_bIsDefusing = NetVarManager->GetOffset("DT_CSPlayer", "m_bIsDefusing");
	offsetz.DT_CSPlayer.m_bIsGrabbingHostage = NetVarManager->GetOffset("DT_CSPlayer", "m_bIsGrabbingHostage");
	offsetz.DT_CSPlayer.m_bIsScoped = NetVarManager->GetOffset("DT_CSPlayer", "m_bIsScoped");
	offsetz.DT_CSPlayer.m_bGunGameImmunity = NetVarManager->GetOffset("DT_CSPlayer", "m_bGunGameImmunity");
	offsetz.DT_CSPlayer.m_bIsRescuing = NetVarManager->GetOffset("DT_CSPlayer", "m_bIsRescuing");
	offsetz.DT_CSPlayer.m_bHasHelmet = NetVarManager->GetOffset("DT_CSPlayer", "m_bHasHelmet");
	offsetz.DT_CSPlayer.m_bHasDefuser = NetVarManager->GetOffset("DT_CSPlayer", "m_bHasDefuser");
	offsetz.DT_CSPlayer.m_flFlashDuration = NetVarManager->GetOffset("DT_CSPlayer", "m_flFlashDuration");
	offsetz.DT_CSPlayer.m_flFlashMaxAlpha = NetVarManager->GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
	offsetz.DT_CSPlayer.m_flLowerBodyYawTarget = NetVarManager->GetOffset("DT_CSPlayer", "m_flLowerBodyYawTarget");
    offsetz.DT_CSPlayer.m_bHasHeavyArmor = NetVarManager->GetOffset("DT_CSPlayer", "m_bHasHeavyArmor");
	offsetz.DT_CSPlayer.m_iGunGameProgressiveWeaponIndex = NetVarManager->GetOffset("DT_CSPlayer", "m_iGunGameProgressiveWeaponIndex");

	offsetz.DT_BaseAttributableItem.m_iItemDefinitionIndex = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	offsetz.DT_BaseAttributableItem.m_iAccountID = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_iAccountID");
	offsetz.DT_BaseAttributableItem.m_iEntityQuality = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_iEntityQuality");
	offsetz.DT_BaseAttributableItem.m_szCustomName = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_szCustomName");
	offsetz.DT_BaseAttributableItem.m_nFallbackPaintKit = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_nFallbackPaintKit");
	offsetz.DT_BaseAttributableItem.m_nFallbackSeed = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_nFallbackSeed");
	offsetz.DT_BaseAttributableItem.m_flFallbackWear = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_flFallbackWear");
	offsetz.DT_BaseAttributableItem.m_nFallbackStatTrak = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_nFallbackStatTrak");
	offsetz.DT_BaseAttributableItem.m_OriginalOwnerXuidLow = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	offsetz.DT_BaseAttributableItem.m_OriginalOwnerXuidHigh = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");

	offsetz.DT_BaseViewModel.m_nModelIndex = NetVarManager->GetOffset("DT_BaseViewModel", "m_nModelIndex");
	offsetz.DT_BaseViewModel.m_hWeapon = NetVarManager->GetOffset("DT_BaseViewModel", "m_hWeapon");
	offsetz.DT_BaseViewModel.m_hOwner = NetVarManager->GetOffset("DT_BaseViewModel", "m_hOwner");

	offsetz.DT_WeaponCSBase.m_fAccuracyPenalty = NetVarManager->GetOffset("DT_WeaponCSBase", "m_fAccuracyPenalty");
    offsetz.DT_WeaponCSBase.m_flPostponeFireReadyTime = NetVarManager->GetOffset("DT_WeaponCSBase", "m_flPostponeFireReadyTime");

	offsetz.DT_WeaponC4.m_bStartedArming = NetVarManager->GetOffset("DT_WeaponC4", "m_bStartedArming");

	offsetz.DT_BaseCombatWeapon.m_flNextPrimaryAttack = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	offsetz.DT_BaseCombatWeapon.m_iViewModelIndex = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_iViewModelIndex");
	offsetz.DT_BaseCombatWeapon.m_iWorldModelIndex = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_iWorldModelIndex");
	offsetz.DT_BaseCombatWeapon.m_hOwner = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_hOwner");
	offsetz.DT_BaseCombatWeapon.m_iClip1 = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_iClip1");
	offsetz.DT_BaseCombatWeapon.m_bInReload = offsetz.DT_BaseCombatWeapon.m_flNextPrimaryAttack + 113;
    offsetz.DT_BaseCombatWeapon.m_hWeaponWorldModel = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_hWeaponWorldModel");
	offsetz.DT_BaseCombatWeapon.m_iReserve = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount");
	offsetz.DT_BaseCombatWeapon.m_hOwner = NetVarManager->GetOffset("DT_BaseCombatWeapon", "m_hOwner");


	offsetz.DT_BaseCSGrenade.m_bRedraw = NetVarManager->GetOffset("DT_BaseCSGrenade", "m_bRedraw");
	offsetz.DT_BaseCSGrenade.m_bIsHeldByPlayer = NetVarManager->GetOffset("DT_BaseCSGrenade", "m_bIsHeldByPlayer");
	offsetz.DT_BaseCSGrenade.m_bPinPulled = NetVarManager->GetOffset("DT_BaseCSGrenade", "m_bPinPulled");
	offsetz.DT_BaseCSGrenade.m_fThrowTime = NetVarManager->GetOffset("DT_BaseCSGrenade", "m_fThrowTime");
	offsetz.DT_BaseCSGrenade.m_bLoopingSoundPlaying = NetVarManager->GetOffset("DT_BaseCSGrenade", "m_bLoopingSoundPlaying");
	offsetz.DT_BaseCSGrenade.m_flThrowStrength = NetVarManager->GetOffset("DT_BaseCSGrenade", "m_flThrowStrength");

	offsetz.DT_DynamicProp.m_bShouldGlow = NetVarManager->GetOffset("DT_DynamicProp", "m_bShouldGlow");

	offsetz.DT_CSGameRulesProxy.m_bBombPlanted = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bBombPlanted");
	offsetz.DT_CSGameRulesProxy.m_bIsValveDS = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bIsValveDS");

	offsetz.DT_CSGameRulesProxy.m_bFreezePeriod = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bFreezePeriod");
	offsetz.DT_CSGameRulesProxy.m_bMatchWaitingForResume = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bMatchWaitingForResume");
	offsetz.DT_CSGameRulesProxy.m_bWarmupPeriod = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bWarmupPeriod");
	offsetz.DT_CSGameRulesProxy.m_fWarmupPeriodEnd = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_fWarmupPeriodEnd");
	offsetz.DT_CSGameRulesProxy.m_fWarmupPeriodStart = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_fWarmupPeriodStart");
	offsetz.DT_CSGameRulesProxy.m_bTerroristTimeOutActive = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bTerroristTimeOutActive");
	offsetz.DT_CSGameRulesProxy.m_bCTTimeOutActive = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bCTTimeOutActive");
	offsetz.DT_CSGameRulesProxy.m_flTerroristTimeOutRemaining = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flTerroristTimeOutRemaining");
	offsetz.DT_CSGameRulesProxy.m_flCTTimeOutRemaining = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flCTTimeOutRemaining");
	offsetz.DT_CSGameRulesProxy.m_nTerroristTimeOuts = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nTerroristTimeOuts");
	offsetz.DT_CSGameRulesProxy.m_nCTTimeOuts = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nCTTimeOuts");
	offsetz.DT_CSGameRulesProxy.m_iRoundTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iRoundTime");
	offsetz.DT_CSGameRulesProxy.m_gamePhase = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_gamePhase");
	offsetz.DT_CSGameRulesProxy.m_totalRoundsPlayed = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_totalRoundsPlayed");
	offsetz.DT_CSGameRulesProxy.m_nOvertimePlaying = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nOvertimePlaying");
	offsetz.DT_CSGameRulesProxy.m_timeUntilNextPhaseStarts = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_timeUntilNextPhaseStarts");
	offsetz.DT_CSGameRulesProxy.m_flCMMItemDropRevealStartTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flCMMItemDropRevealStartTime");
	offsetz.DT_CSGameRulesProxy.m_flCMMItemDropRevealEndTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flCMMItemDropRevealEndTime");
	offsetz.DT_CSGameRulesProxy.m_fRoundStartTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_fRoundStartTime");
	offsetz.DT_CSGameRulesProxy.m_bGameRestart = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bGameRestart");
	offsetz.DT_CSGameRulesProxy.m_flRestartRoundTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flRestartRoundTime");
	offsetz.DT_CSGameRulesProxy.m_flGameStartTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flGameStartTime");
	offsetz.DT_CSGameRulesProxy.m_iHostagesRemaining = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iHostagesRemaining");
	offsetz.DT_CSGameRulesProxy.m_bAnyHostageReached = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bAnyHostageReached");
	offsetz.DT_CSGameRulesProxy.m_bMapHasBombTarget = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bMapHasBombTarget");
	offsetz.DT_CSGameRulesProxy.m_bMapHasRescueZone = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bMapHasRescueZone");
	offsetz.DT_CSGameRulesProxy.m_bMapHasBuyZone = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bMapHasBuyZone");
	offsetz.DT_CSGameRulesProxy.m_bIsQueuedMatchmaking = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bIsQueuedMatchmaking");
	offsetz.DT_CSGameRulesProxy.m_bIsValveDS = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bIsValveDS");
	offsetz.DT_CSGameRulesProxy.m_bIsQuestEligible = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bIsQuestEligible");
	offsetz.DT_CSGameRulesProxy.m_bLogoMap = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bLogoMap");
	offsetz.DT_CSGameRulesProxy.m_iNumGunGameProgressiveWeaponsCT = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iNumGunGameProgressiveWeaponsCT");
	offsetz.DT_CSGameRulesProxy.m_iNumGunGameProgressiveWeaponsT = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iNumGunGameProgressiveWeaponsT");
	offsetz.DT_CSGameRulesProxy.m_iSpectatorSlotCount = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iSpectatorSlotCount");
	offsetz.DT_CSGameRulesProxy.m_bBombDropped = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bBombDropped");
	offsetz.DT_CSGameRulesProxy.m_bBombPlanted = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bBombPlanted");
	offsetz.DT_CSGameRulesProxy.m_iRoundWinStatus = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iRoundWinStatus");
	offsetz.DT_CSGameRulesProxy.m_eRoundWinReason = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_eRoundWinReason");
	offsetz.DT_CSGameRulesProxy.m_flDMBonusStartTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flDMBonusStartTime");
	offsetz.DT_CSGameRulesProxy.m_flDMBonusTimeLength = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flDMBonusTimeLength");
	offsetz.DT_CSGameRulesProxy.m_unDMBonusWeaponLoadoutSlot = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_unDMBonusWeaponLoadoutSlot");
	offsetz.DT_CSGameRulesProxy.m_bDMBonusActive = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bDMBonusActive");
	offsetz.DT_CSGameRulesProxy.m_bTCantBuy = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bTCantBuy");
	offsetz.DT_CSGameRulesProxy.m_bCTCantBuy = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bCTCantBuy");
	offsetz.DT_CSGameRulesProxy.m_flGuardianBuyUntilTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flGuardianBuyUntilTime");
	offsetz.DT_CSGameRulesProxy.m_iMatchStats_RoundResults = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iMatchStats_RoundResults");
	offsetz.DT_CSGameRulesProxy.m_iMatchStats_PlayersAlive_T = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iMatchStats_PlayersAlive_T");
	offsetz.DT_CSGameRulesProxy.m_iMatchStats_PlayersAlive_CT = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iMatchStats_PlayersAlive_CT");
	offsetz.DT_CSGameRulesProxy.m_GGProgressiveWeaponOrderC = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_GGProgressiveWeaponOrderC");
	offsetz.DT_CSGameRulesProxy.m_GGProgressiveWeaponOrderT = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_GGProgressiveWeaponOrderT");
	offsetz.DT_CSGameRulesProxy.m_GGProgressiveWeaponKillUpgradeOrderCT = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_GGProgressiveWeaponKillUpgradeOrderCT");
	offsetz.DT_CSGameRulesProxy.m_GGProgressiveWeaponKillUpgradeOrderT = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_GGProgressiveWeaponKillUpgradeOrderT");
	offsetz.DT_CSGameRulesProxy.m_MatchDevice = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_MatchDevice");
	offsetz.DT_CSGameRulesProxy.m_bHasMatchStarted = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bHasMatchStarted");
	offsetz.DT_CSGameRulesProxy.m_TeamRespawnWaveTimes = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_TeamRespawnWaveTimes");
	offsetz.DT_CSGameRulesProxy.m_flNextRespawnWave = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_flNextRespawnWave");
	offsetz.DT_CSGameRulesProxy.m_nNextMapInMapgroup = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nNextMapInMapgroup");
	offsetz.DT_CSGameRulesProxy.m_nEndMatchMapGroupVoteOptions = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nEndMatchMapGroupVoteOptions");
	offsetz.DT_CSGameRulesProxy.m_bIsDroppingItems = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_bIsDroppingItems");
	offsetz.DT_CSGameRulesProxy.m_iActiveAssassinationTargetMissionID = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_iActiveAssassinationTargetMissionID");
	offsetz.DT_CSGameRulesProxy.m_fMatchStartTime = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_fMatchStartTime");
	offsetz.DT_CSGameRulesProxy.m_szTournamentEventName = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_szTournamentEventName");
	offsetz.DT_CSGameRulesProxy.m_szTournamentEventStage = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_szTournamentEventStage");
	offsetz.DT_CSGameRulesProxy.m_szTournamentPredictionsTxt = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_szTournamentPredictionsTxt");
	offsetz.DT_CSGameRulesProxy.m_nTournamentPredictionsPct = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nTournamentPredictionsPct");
	offsetz.DT_CSGameRulesProxy.m_szMatchStatTxt = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_szMatchStatTxt");
	offsetz.DT_CSGameRulesProxy.m_nGuardianModeWaveNumber = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nGuardianModeWaveNumber");
	offsetz.DT_CSGameRulesProxy.m_nGuardianModeSpecialKillsRemaining = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nGuardianModeSpecialKillsRemaining");
	offsetz.DT_CSGameRulesProxy.m_nGuardianModeSpecialWeaponNeeded = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nGuardianModeSpecialWeaponNeeded");
	offsetz.DT_CSGameRulesProxy.m_nHalloweenMaskListSeed = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_nHalloweenMaskListSeed");
	offsetz.DT_CSGameRulesProxy.m_numGlobalGiftsGiven = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_numGlobalGiftsGiven");
	offsetz.DT_CSGameRulesProxy.m_numGlobalGifters = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_numGlobalGifters");
	offsetz.DT_CSGameRulesProxy.m_numGlobalGiftsPeriodSeconds = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_numGlobalGiftsPeriodSeconds");
	offsetz.DT_CSGameRulesProxy.m_arrFeaturedGiftersAccounts = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_arrFeaturedGiftersAccounts");
	offsetz.DT_CSGameRulesProxy.m_arrFeaturedGiftersGifts = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_arrFeaturedGiftersGifts");
	offsetz.DT_CSGameRulesProxy.m_arrTournamentActiveCasterAccounts = NetVarManager->GetOffset("DT_CSGameRulesProxy", "m_arrTournamentActiveCasterAccounts");

    offsetz.DT_BaseAnimating.m_nHitboxSet = NetVarManager->GetOffset("DT_BaseAnimating", "m_nHitboxSet");

}

Offsetz* g_Netvars = new(Offsetz);
COffsetz offsetz;

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class mjzxybv {
public:
	string bfuhutwpjvqvoie;
	double wvzfnztaspjy;
	string vlakfcupxplu;
	int vayhkxypqrvk;
	mjzxybv();
	bool hqztxxeeebbfgjpbuxoa(double rscjxjqelaaxoi, bool yqthhkhygb, string woxcfdhikoxbd, string gkubbstg, int qnotrhrjz, bool gstxpmbspu, string vejyhbxbcwwy);
	string appzyamwpnjojlaigohg(int vuqizqj, double hmlly, int uigfoozvfr, string xilypiodoexefw, int pkazbmjpilyebd, bool tiijrn, string khpfxfjpzqvlqeq);
	void hbmhmlxcab();
	int kjqufmrjvgh(double xqckgmpyawgxb, double whytyebmstbxwjd, bool pkbnwdfwhn, double xctvaxz, bool mhtouofwifua, bool xdjli, bool iatrk);
	int deypvzvkdqkhad(string gqivibctatodyn, bool xtgojrdylmhdgqe, string qnppoofowmwdz, string zqjqwqxqccwqw, int rdwqddmqsy, string nroxjp);
	double xytcbpssoirabhiwhliu(string lmywylpiefii, double lemsckhvarcfwag, double lffjd);
	bool xlvvjqhovbkrqgyetycgjsdzl(int jknxuyyp, double ziakfi, int tsfqcebmvyeg, bool buoyub, double weceg, string btiyqrgfnw, double lebruhrjs, string memswmoczdn, bool kunedquoch);
	double chsxzmluccemjugoiopidet(int xnmuoiiwbmk, double lnciplkxbgappgh, string vojvwk, int ffhtiyyxeaoyn, double jjxmntfhbm, bool yszgzdkn, string pxtmcdzmopi);

protected:
	double cwkbtr;
	int ypgjq;
	bool vfyotjptdk;
	double imddr;
	bool loypvzuqxpjfeh;

	string rvyustcuwjsfputusxxccexcz(bool hfnnajeiqqrmi, bool jrwlcbptx);

private:
	string tdcmkush;
	bool xqzwbzp;

	void youkljnucvvmzzweevstegjzq();
	bool hhzjchxslxaqtg(string rwuofdlyh, int tyorftimiqbydso);

};


void mjzxybv::youkljnucvvmzzweevstegjzq() {
	bool ckvdrycwdgzx = false;
	double tujealmfe = 12618;
	int uhgjqdzy = 748;
	double opghybrsrsqoxup = 6385;
	int ifjoctuddrizrh = 2414;
	int nscheyab = 2114;
	double ahelqumjlyri = 58870;

}

bool mjzxybv::hhzjchxslxaqtg(string rwuofdlyh, int tyorftimiqbydso) {
	double ddkhrzhrmefi = 37363;
	bool ajczuyfbbfnbwe = true;
	string bvnukxoamvcba = "vqmxmuicnwmxivxufxwynghvfyntxtnghpsritclwmpqqkuuzbfddzmdfpywxbucnmilvhovlbcqnlsvzomnhguivlqgwm";
	string vijpjrlvtpm = "fnjixwshbqgnaxnbcmkosccpnvlrnzwsppukbqbpvkfauqggstlhujxmuleomrocqofieuvwnrerf";
	bool oqqcd = true;
	string jodrkbsnzbaja = "rdauuxloejnctbntsiivkbxicbsanirdvrvdjdchykpzlgmhavtuwksuldywsrlgksaztgusatd";
	if (string("vqmxmuicnwmxivxufxwynghvfyntxtnghpsritclwmpqqkuuzbfddzmdfpywxbucnmilvhovlbcqnlsvzomnhguivlqgwm") == string("vqmxmuicnwmxivxufxwynghvfyntxtnghpsritclwmpqqkuuzbfddzmdfpywxbucnmilvhovlbcqnlsvzomnhguivlqgwm")) {
		int ie;
		for (ie = 64; ie > 0; ie--) {
			continue;
		}
	}
	if (string("vqmxmuicnwmxivxufxwynghvfyntxtnghpsritclwmpqqkuuzbfddzmdfpywxbucnmilvhovlbcqnlsvzomnhguivlqgwm") == string("vqmxmuicnwmxivxufxwynghvfyntxtnghpsritclwmpqqkuuzbfddzmdfpywxbucnmilvhovlbcqnlsvzomnhguivlqgwm")) {
		int jxcrtykppj;
		for (jxcrtykppj = 18; jxcrtykppj > 0; jxcrtykppj--) {
			continue;
		}
	}
	return true;
}

string mjzxybv::rvyustcuwjsfputusxxccexcz(bool hfnnajeiqqrmi, bool jrwlcbptx) {
	double jvyqb = 7436;
	double iqfhqnfkrpqmcc = 39367;
	bool okqfcu = true;
	string iwdtkwvn = "ikcxowobauftdhpgeuqzzqpmeyqjvkosuorbnfzppxhqksh";
	string yoojssuntdbkhq = "exrnptjzhsknhjoefzqinsallqaistjhcsbfwbleaklwcpfozxicoypuxquqszbvttwvy";
	int ylbthfpkgidd = 1440;
	bool iyetgkobonfeca = true;
	double elmetwt = 3590;
	double ktwptmyzqtfl = 10124;
	string mnftzqflpz = "kincithnyaitnotlimsgjzciywdyoxnwsa";
	if (7436 != 7436) {
		int vulypgjsrb;
		for (vulypgjsrb = 93; vulypgjsrb > 0; vulypgjsrb--) {
			continue;
		}
	}
	if (3590 != 3590) {
		int nozfl;
		for (nozfl = 37; nozfl > 0; nozfl--) {
			continue;
		}
	}
	return string("roixdlzicridmfp");
}

bool mjzxybv::hqztxxeeebbfgjpbuxoa(double rscjxjqelaaxoi, bool yqthhkhygb, string woxcfdhikoxbd, string gkubbstg, int qnotrhrjz, bool gstxpmbspu, string vejyhbxbcwwy) {
	string dnktqjswokzxyo = "hwjesxlctewqzm";
	double gpycn = 35631;
	double bpvkmpkw = 62105;
	int ltslfrkxkynqhmc = 4439;
	string gkriiabtfgrr = "ypnrdaaikdrxtuggvkzhemwhspranbzhexqrayfjrbwziqxaufbvlqzledcqrctsedfvucdolbnihihuorjwbvkhrafnevuc";
	bool embabzclvieltv = true;
	if (string("hwjesxlctewqzm") != string("hwjesxlctewqzm")) {
		int yofqhzjn;
		for (yofqhzjn = 17; yofqhzjn > 0; yofqhzjn--) {
			continue;
		}
	}
	if (string("hwjesxlctewqzm") != string("hwjesxlctewqzm")) {
		int vr;
		for (vr = 99; vr > 0; vr--) {
			continue;
		}
	}
	if (string("ypnrdaaikdrxtuggvkzhemwhspranbzhexqrayfjrbwziqxaufbvlqzledcqrctsedfvucdolbnihihuorjwbvkhrafnevuc") != string("ypnrdaaikdrxtuggvkzhemwhspranbzhexqrayfjrbwziqxaufbvlqzledcqrctsedfvucdolbnihihuorjwbvkhrafnevuc")) {
		int zirisffy;
		for (zirisffy = 67; zirisffy > 0; zirisffy--) {
			continue;
		}
	}
	if (62105 != 62105) {
		int rcpr;
		for (rcpr = 4; rcpr > 0; rcpr--) {
			continue;
		}
	}
	return true;
}

string mjzxybv::appzyamwpnjojlaigohg(int vuqizqj, double hmlly, int uigfoozvfr, string xilypiodoexefw, int pkazbmjpilyebd, bool tiijrn, string khpfxfjpzqvlqeq) {
	double vbruyqnkvhwllr = 13650;
	string lhhxmw = "pfiwkbkmmemzoztumrgmxqufvhywbmv";
	int hrkvlnppyb = 1571;
	string dnrikyl = "recqeyrusiegeqybjtyezgumjpvlnfycilnmvm";
	bool iovjxdoohaqsr = false;
	bool bxvlycxdqwsmbh = true;
	if (true != true) {
		int cuqimkuce;
		for (cuqimkuce = 49; cuqimkuce > 0; cuqimkuce--) {
			continue;
		}
	}
	if (13650 != 13650) {
		int jkaqtbia;
		for (jkaqtbia = 30; jkaqtbia > 0; jkaqtbia--) {
			continue;
		}
	}
	if (string("pfiwkbkmmemzoztumrgmxqufvhywbmv") == string("pfiwkbkmmemzoztumrgmxqufvhywbmv")) {
		int llshkvsvx;
		for (llshkvsvx = 26; llshkvsvx > 0; llshkvsvx--) {
			continue;
		}
	}
	return string("aolnozx");
}

void mjzxybv::hbmhmlxcab() {
	double vuarfihxrmkkx = 13697;
	bool mayfyzb = true;
	bool nzpukcdbruy = true;
	double ywbtiwqklvouv = 4194;
	double ykpogtzlzz = 34405;
	if (4194 == 4194) {
		int rtgftio;
		for (rtgftio = 27; rtgftio > 0; rtgftio--) {
			continue;
		}
	}
	if (true != true) {
		int yddiha;
		for (yddiha = 100; yddiha > 0; yddiha--) {
			continue;
		}
	}

}

int mjzxybv::kjqufmrjvgh(double xqckgmpyawgxb, double whytyebmstbxwjd, bool pkbnwdfwhn, double xctvaxz, bool mhtouofwifua, bool xdjli, bool iatrk) {
	bool mbfnencwq = true;
	return 6037;
}

int mjzxybv::deypvzvkdqkhad(string gqivibctatodyn, bool xtgojrdylmhdgqe, string qnppoofowmwdz, string zqjqwqxqccwqw, int rdwqddmqsy, string nroxjp) {
	bool xtrwkoeepscilrd = true;
	double acpbsqnbwa = 15983;
	bool vcsybrhgkmazmup = true;
	int tqykbotnj = 634;
	if (true == true) {
		int dzltjo;
		for (dzltjo = 42; dzltjo > 0; dzltjo--) {
			continue;
		}
	}
	return 42104;
}

double mjzxybv::xytcbpssoirabhiwhliu(string lmywylpiefii, double lemsckhvarcfwag, double lffjd) {
	bool iokjyxdia = false;
	bool xewctijgflyu = false;
	double zazksd = 39972;
	double qtokpmpg = 36344;
	int lilckt = 163;
	string ykmszfblyb = "ttzwzzbsjeslskvjvymrsazdxygayrvfwcdsprhjmkfxwtdqjldjlugmrskgmfqurymfejpgjqtirftqbeqknaansgly";
	int mdzmfjdcvl = 1614;
	bool scbsc = true;
	string aoimtwytywef = "hqosnoiijwtbfcyvnirofnamnd";
	double gnjlcrfwrxbsh = 19276;
	if (19276 == 19276) {
		int bg;
		for (bg = 91; bg > 0; bg--) {
			continue;
		}
	}
	if (39972 == 39972) {
		int kg;
		for (kg = 68; kg > 0; kg--) {
			continue;
		}
	}
	if (36344 == 36344) {
		int arzlfflhwl;
		for (arzlfflhwl = 86; arzlfflhwl > 0; arzlfflhwl--) {
			continue;
		}
	}
	if (163 != 163) {
		int pgaygpod;
		for (pgaygpod = 50; pgaygpod > 0; pgaygpod--) {
			continue;
		}
	}
	if (true != true) {
		int skbws;
		for (skbws = 23; skbws > 0; skbws--) {
			continue;
		}
	}
	return 74971;
}

bool mjzxybv::xlvvjqhovbkrqgyetycgjsdzl(int jknxuyyp, double ziakfi, int tsfqcebmvyeg, bool buoyub, double weceg, string btiyqrgfnw, double lebruhrjs, string memswmoczdn, bool kunedquoch) {
	double shkyafwnqq = 51612;
	if (51612 == 51612) {
		int pptc;
		for (pptc = 96; pptc > 0; pptc--) {
			continue;
		}
	}
	if (51612 == 51612) {
		int rh;
		for (rh = 81; rh > 0; rh--) {
			continue;
		}
	}
	if (51612 == 51612) {
		int jsnqvvdk;
		for (jsnqvvdk = 42; jsnqvvdk > 0; jsnqvvdk--) {
			continue;
		}
	}
	if (51612 != 51612) {
		int ymoarbii;
		for (ymoarbii = 63; ymoarbii > 0; ymoarbii--) {
			continue;
		}
	}
	if (51612 != 51612) {
		int wz;
		for (wz = 47; wz > 0; wz--) {
			continue;
		}
	}
	return true;
}

double mjzxybv::chsxzmluccemjugoiopidet(int xnmuoiiwbmk, double lnciplkxbgappgh, string vojvwk, int ffhtiyyxeaoyn, double jjxmntfhbm, bool yszgzdkn, string pxtmcdzmopi) {
	bool ctbez = false;
	int wjwfq = 1753;
	string vwwqahzu = "rewwlzsxxokowbmhxgztshxtcnelzxlbmhmhzszfsmehk";
	double ngrpiyk = 15221;
	int aefzxgsgziahl = 490;
	bool wimotcjgi = true;
	string huzsvputcizv = "dicbqyuxouaearihmhhpuujrpehnislerlsz";
	string ljixxxbcynzigc = "dpbzdxrihsdayxcqwnylzykrkopnpagbmdnuozsoaogjndgbsnnwvltclyiwilprvukwotmxbyx";
	int gsiybau = 971;
	if (false == false) {
		int cigctocuz;
		for (cigctocuz = 24; cigctocuz > 0; cigctocuz--) {
			continue;
		}
	}
	if (490 == 490) {
		int zykgkvsjlk;
		for (zykgkvsjlk = 84; zykgkvsjlk > 0; zykgkvsjlk--) {
			continue;
		}
	}
	if (string("rewwlzsxxokowbmhxgztshxtcnelzxlbmhmhzszfsmehk") != string("rewwlzsxxokowbmhxgztshxtcnelzxlbmhmhzszfsmehk")) {
		int emo;
		for (emo = 25; emo > 0; emo--) {
			continue;
		}
	}
	if (false != false) {
		int htpn;
		for (htpn = 82; htpn > 0; htpn--) {
			continue;
		}
	}
	if (false == false) {
		int fk;
		for (fk = 82; fk > 0; fk--) {
			continue;
		}
	}
	return 66035;
}

mjzxybv::mjzxybv() {
	this->hqztxxeeebbfgjpbuxoa(62351, false, string("huigrqjibrqdqjgnktrxdsfyxgaqjlslhzkoribydbseiwwtxiwvwjalsmzddcoeifjo"), string("iemoqcsdzibkcucnvlmcllhopxqzmaklhzuutawpltjgab"), 811, false, string("ajyzwrqvmlgvrlbfsymuorwvffpqdnxxjgdaxiddqssshnugznzltzlvbf"));
	this->appzyamwpnjojlaigohg(4488, 34021, 4430, string("ffuqmzrzpslsulqkrudtlwuuthzdqw"), 2241, true, string("spowwxnsefbllycbgcyywbqwiwdhpgyifqkbenhxtxmogkpxhzyciywzptqzkflgyuaepllupxaidpycntubzpbsvgtuukybw"));
	this->hbmhmlxcab();
	this->kjqufmrjvgh(2824, 13443, true, 9577, true, true, true);
	this->deypvzvkdqkhad(string("swsrdzzgytrzjlfbhodcszrgoegcuacsgyghbrs"), false, string("ogwtaccbedxcpfdpdmxgosfnujcsriskdoplhsqrdcukgetwiyajpidpgx"), string("awqboqnrhknlaixsoxihrrodifhovgslrvyvmygtlcuulcfdtjouejlizxtlypbcuadnudp"), 863, string("snbx"));
	this->xytcbpssoirabhiwhliu(string("qbkvzqhnldzsrwftdbchszkebrh"), 42337, 18245);
	this->xlvvjqhovbkrqgyetycgjsdzl(2614, 84427, 749, false, 9402, string("eohopydetrdh"), 71846, string("cavazuehqdymzvdkdxlilhcxyfvpoawozdlpsgizhjpvxk"), true);
	this->chsxzmluccemjugoiopidet(2784, 8484, string("vuefhtrbtjsjo"), 1124, 22871, true, string("pvjlliiwgnjewoaqym"));
	this->rvyustcuwjsfputusxxccexcz(true, true);
	this->youkljnucvvmzzweevstegjzq();
	this->hhzjchxslxaqtg(string("dchldmsgoiugauyemroqllwqzkbcvxysrjypoihimtpdnwdtofqurw"), 4025);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class uoistjb {
public:
	int wrnjyvarkkqi;
	double khhcpnmbkaduahk;
	uoistjb();
	double bppbbdmowpxswaogfz(string qrhxeosrqe, bool zmasaqvuydtzr, double rbrpyisaarqgfqn, int lozzzovsdvvhhul, double ocjxszsuirvjc, bool gcpdvqptrqhfbbv, double xzwzmkenfyr, bool xdeluaynkvu);
	double upfxkozcmhvcamprfkap(int bsziyaiakhv, int qhfegayhbis, int zgilgnuhvp, int ribsaireieslwd, string kehjfnl, double omamwjlqprpaca, bool fqiglrljwlpvo, double mxxeqvmuhuarwf);
	bool byyaajrhyawtkorrfnzh(string pxbwwenhpjfi, bool angkagesvoi, double qcsfjv, string olraeqp, double itpkinnkth, string tnbbpf);
	int rfdjurhyqnsrftutqcjk(double zkowfhkycdpzrwb, string ntxgpldzfpsuc, bool bjiqgawl, int xccuwalupebwwnj);
	bool mpatcsjevbgmz(int tsfhagnypqq, bool xnajkoutqffyog, double clsxdtxvvpalrb);
	void ylymdqvhncio();
	void vmmerhjply(int oanzrgwovfzpmv, string jimon, double zepou, double iytlnyej, string ttcnvadehtu, double vhiqrucjn, int kqeocehzmds);

protected:
	string jooudyqkilnxm;
	int xyrlpivpxunlyl;

	double fkgovzoyutlk(string royhnqvkdgeg, double xwhenbambxgoy, string sachi, string jqiex, int ddrwxhn);
	double gmsqhhjshtstzfrqi(double qfpjfqqceekmn, double hjqnj, string lknqh, int khupukmrwud, string zfuawvtb, int bgismpmudztky, int rvzxae, bool kczrmgy, string keyhqokcparufa);
	string nojpzuiybokefzth(int wuwldugdpckda, int ivaqejnneqyh, double qtbfq, int tylchi, int vxamkwuxkveip, int ppheisqdudb, int vcdavbpupxz, bool hogngsgticpdzb, bool nofvdttoigvszv);
	string lzxhufqrorxnfzhmgqv(bool vqwienkrgmnahd, string xcqtnf, bool rxmrubpgncpdi, bool dztjgord);
	bool gxomxjjtpmsiymetkekqqbrp(double cwrvey, int xqharqvug);

private:
	string fjdcfhvvzdz;
	bool fvlynbbseif;

	void sccyhbhvodbfhdzdhlwnbsokz(string zimnimdgmezet, bool yrodlftb, bool mnvwoitewrsvyi, bool mvszvhlwvsb, bool qwadsudymxhfpv, bool xwvahv);
	string rfwuoifxikb(bool sbttwrnl, double yyrpvntxvwwa, int cqtwfkuvr);

};


void uoistjb::sccyhbhvodbfhdzdhlwnbsokz(string zimnimdgmezet, bool yrodlftb, bool mnvwoitewrsvyi, bool mvszvhlwvsb, bool qwadsudymxhfpv, bool xwvahv) {
	string agnpphfkqunlmkr = "mhpgdxtllhrsklsfznru";
	double uququttazcpkzr = 37503;
	string bnlna = "zxjztvgkibwhjtazjxsgivntqlcvtsiyvkdadpzerztwggcgppqflomtglpghpvbluiolvbxtbenumjqfkjp";
	string rxzwoypi = "idalppownvyhghournovqztjrjfvevmhkuclyldsrbvfuzcslpnprkaubhtxufbcuhgtvi";
	int bkiznztbijzsq = 361;
	double lfhlzbay = 70043;
	string zszqoe = "o";
	int ajwgprvzycbpv = 3320;
	bool vthiczr = false;

}

string uoistjb::rfwuoifxikb(bool sbttwrnl, double yyrpvntxvwwa, int cqtwfkuvr) {
	int shatutegtbwert = 1184;
	string kgjiurxyjw = "rcyngorgqcioiojzjhpvtsfxayafezmhsmraczrwuoizqazrypvvneqrrwsffwrdyhvmevcpmmexfvopdqyv";
	if (string("rcyngorgqcioiojzjhpvtsfxayafezmhsmraczrwuoizqazrypvvneqrrwsffwrdyhvmevcpmmexfvopdqyv") == string("rcyngorgqcioiojzjhpvtsfxayafezmhsmraczrwuoizqazrypvvneqrrwsffwrdyhvmevcpmmexfvopdqyv")) {
		int xjseqpb;
		for (xjseqpb = 97; xjseqpb > 0; xjseqpb--) {
			continue;
		}
	}
	if (string("rcyngorgqcioiojzjhpvtsfxayafezmhsmraczrwuoizqazrypvvneqrrwsffwrdyhvmevcpmmexfvopdqyv") == string("rcyngorgqcioiojzjhpvtsfxayafezmhsmraczrwuoizqazrypvvneqrrwsffwrdyhvmevcpmmexfvopdqyv")) {
		int ehitfxnost;
		for (ehitfxnost = 64; ehitfxnost > 0; ehitfxnost--) {
			continue;
		}
	}
	if (1184 == 1184) {
		int fehhfpid;
		for (fehhfpid = 40; fehhfpid > 0; fehhfpid--) {
			continue;
		}
	}
	return string("uo");
}

double uoistjb::fkgovzoyutlk(string royhnqvkdgeg, double xwhenbambxgoy, string sachi, string jqiex, int ddrwxhn) {
	int nqiit = 3249;
	if (3249 != 3249) {
		int bircfxzmyf;
		for (bircfxzmyf = 14; bircfxzmyf > 0; bircfxzmyf--) {
			continue;
		}
	}
	if (3249 != 3249) {
		int omjvqgy;
		for (omjvqgy = 55; omjvqgy > 0; omjvqgy--) {
			continue;
		}
	}
	if (3249 == 3249) {
		int gr;
		for (gr = 68; gr > 0; gr--) {
			continue;
		}
	}
	if (3249 != 3249) {
		int vrqiewbctd;
		for (vrqiewbctd = 41; vrqiewbctd > 0; vrqiewbctd--) {
			continue;
		}
	}
	if (3249 != 3249) {
		int dnvqbxfsl;
		for (dnvqbxfsl = 83; dnvqbxfsl > 0; dnvqbxfsl--) {
			continue;
		}
	}
	return 77008;
}

double uoistjb::gmsqhhjshtstzfrqi(double qfpjfqqceekmn, double hjqnj, string lknqh, int khupukmrwud, string zfuawvtb, int bgismpmudztky, int rvzxae, bool kczrmgy, string keyhqokcparufa) {
	string sgjfgyoxinczfso = "qnhdopwqrexhegtpbnpgejehlmgocovcnmiavjtvzpuspkncviwizbw";
	int rhljfparaacnw = 7006;
	double lpimacdpmfqdoc = 53163;
	double jpdsqqhfeq = 968;
	int cgzbtpx = 5361;
	if (968 != 968) {
		int bcgkadrwqz;
		for (bcgkadrwqz = 56; bcgkadrwqz > 0; bcgkadrwqz--) {
			continue;
		}
	}
	if (string("qnhdopwqrexhegtpbnpgejehlmgocovcnmiavjtvzpuspkncviwizbw") != string("qnhdopwqrexhegtpbnpgejehlmgocovcnmiavjtvzpuspkncviwizbw")) {
		int ox;
		for (ox = 17; ox > 0; ox--) {
			continue;
		}
	}
	if (53163 != 53163) {
		int kbcxhgt;
		for (kbcxhgt = 1; kbcxhgt > 0; kbcxhgt--) {
			continue;
		}
	}
	if (5361 != 5361) {
		int kmpzvggmbx;
		for (kmpzvggmbx = 63; kmpzvggmbx > 0; kmpzvggmbx--) {
			continue;
		}
	}
	if (53163 == 53163) {
		int cdijcgjmf;
		for (cdijcgjmf = 87; cdijcgjmf > 0; cdijcgjmf--) {
			continue;
		}
	}
	return 89440;
}

string uoistjb::nojpzuiybokefzth(int wuwldugdpckda, int ivaqejnneqyh, double qtbfq, int tylchi, int vxamkwuxkveip, int ppheisqdudb, int vcdavbpupxz, bool hogngsgticpdzb, bool nofvdttoigvszv) {
	bool gxhpeahxg = true;
	double ewxjsnw = 48077;
	if (48077 == 48077) {
		int wqeydoub;
		for (wqeydoub = 27; wqeydoub > 0; wqeydoub--) {
			continue;
		}
	}
	if (true == true) {
		int cjwexffk;
		for (cjwexffk = 25; cjwexffk > 0; cjwexffk--) {
			continue;
		}
	}
	if (48077 != 48077) {
		int pucuo;
		for (pucuo = 48; pucuo > 0; pucuo--) {
			continue;
		}
	}
	return string("qla");
}

string uoistjb::lzxhufqrorxnfzhmgqv(bool vqwienkrgmnahd, string xcqtnf, bool rxmrubpgncpdi, bool dztjgord) {
	int adfllw = 1397;
	bool fiahrokaqdptkp = true;
	bool slzcccobkpvszni = false;
	int yvfvnjukw = 3323;
	bool ygzibhxqru = true;
	int uwcsflf = 601;
	double qmfmjmpkigcih = 30208;
	string yyjvdin = "jqzrnzgwdnburdyxbtqgnbvkyhfoelslyaugtldjgxybmtgcreqehmrvqoptxejglnomjlxldkyj";
	double chxecussxxmwvgp = 85109;
	string ozuavlz = "iqwhvzfkzqgpeknbrxovntgtvexbranfhyqxfrgussslmsvbagkipnhibalczdslffzltt";
	if (601 != 601) {
		int hhsz;
		for (hhsz = 52; hhsz > 0; hhsz--) {
			continue;
		}
	}
	if (601 == 601) {
		int eaxipl;
		for (eaxipl = 0; eaxipl > 0; eaxipl--) {
			continue;
		}
	}
	return string("polyjdfpd");
}

bool uoistjb::gxomxjjtpmsiymetkekqqbrp(double cwrvey, int xqharqvug) {
	double xmdfxxyrxksix = 71168;
	double rzfkypdvspbmbqo = 31838;
	double mawmmaivvmewd = 16865;
	bool kpftzjfm = true;
	int cfbsmxthpden = 6206;
	bool wczardwkidthcbe = false;
	bool oqzdskb = true;
	if (false != false) {
		int payxlt;
		for (payxlt = 1; payxlt > 0; payxlt--) {
			continue;
		}
	}
	if (true == true) {
		int ztoo;
		for (ztoo = 16; ztoo > 0; ztoo--) {
			continue;
		}
	}
	if (false != false) {
		int gproqut;
		for (gproqut = 26; gproqut > 0; gproqut--) {
			continue;
		}
	}
	return true;
}

double uoistjb::bppbbdmowpxswaogfz(string qrhxeosrqe, bool zmasaqvuydtzr, double rbrpyisaarqgfqn, int lozzzovsdvvhhul, double ocjxszsuirvjc, bool gcpdvqptrqhfbbv, double xzwzmkenfyr, bool xdeluaynkvu) {
	string nqeesfu = "fluebeftmjjslnwmunexvos";
	int fkqzwzhnz = 1461;
	double tgpgfvd = 2500;
	int lkeqjackpedfc = 2029;
	string eqrsopgmoa = "xdxpgvalxnzfufcwtdikmyrukclnypxakxtpudynpotdqamguvinzwtdoncowani";
	int xgbmaprnbrxljqn = 2065;
	bool pbhkfgwtafkjp = true;
	bool deytnfhmyty = false;
	if (false != false) {
		int ly;
		for (ly = 57; ly > 0; ly--) {
			continue;
		}
	}
	if (2500 != 2500) {
		int wktunci;
		for (wktunci = 28; wktunci > 0; wktunci--) {
			continue;
		}
	}
	if (true == true) {
		int ovmeyvmilq;
		for (ovmeyvmilq = 22; ovmeyvmilq > 0; ovmeyvmilq--) {
			continue;
		}
	}
	if (1461 != 1461) {
		int ycurk;
		for (ycurk = 27; ycurk > 0; ycurk--) {
			continue;
		}
	}
	if (1461 == 1461) {
		int zcqlgcil;
		for (zcqlgcil = 6; zcqlgcil > 0; zcqlgcil--) {
			continue;
		}
	}
	return 53031;
}

double uoistjb::upfxkozcmhvcamprfkap(int bsziyaiakhv, int qhfegayhbis, int zgilgnuhvp, int ribsaireieslwd, string kehjfnl, double omamwjlqprpaca, bool fqiglrljwlpvo, double mxxeqvmuhuarwf) {
	int hftqornjmmy = 839;
	double wllmk = 29650;
	double eiovc = 12431;
	if (29650 != 29650) {
		int rguylug;
		for (rguylug = 99; rguylug > 0; rguylug--) {
			continue;
		}
	}
	if (12431 == 12431) {
		int mnpkakzo;
		for (mnpkakzo = 99; mnpkakzo > 0; mnpkakzo--) {
			continue;
		}
	}
	if (12431 != 12431) {
		int ikyeeqe;
		for (ikyeeqe = 16; ikyeeqe > 0; ikyeeqe--) {
			continue;
		}
	}
	if (29650 != 29650) {
		int xzrf;
		for (xzrf = 40; xzrf > 0; xzrf--) {
			continue;
		}
	}
	return 94566;
}

bool uoistjb::byyaajrhyawtkorrfnzh(string pxbwwenhpjfi, bool angkagesvoi, double qcsfjv, string olraeqp, double itpkinnkth, string tnbbpf) {
	string ktlzfrnoaypcek = "uqqanbljhouwodcgpazkeajbhhbnrydqzyxvi";
	int esviblqq = 150;
	int jvtkhznsakf = 8602;
	if (8602 != 8602) {
		int pcts;
		for (pcts = 39; pcts > 0; pcts--) {
			continue;
		}
	}
	if (8602 != 8602) {
		int ngfzavi;
		for (ngfzavi = 34; ngfzavi > 0; ngfzavi--) {
			continue;
		}
	}
	return false;
}

int uoistjb::rfdjurhyqnsrftutqcjk(double zkowfhkycdpzrwb, string ntxgpldzfpsuc, bool bjiqgawl, int xccuwalupebwwnj) {
	double ucoldadbhskhavk = 908;
	double qzibewe = 58593;
	int zqcqmqb = 975;
	int leyampjqsl = 1137;
	if (975 != 975) {
		int xuiwib;
		for (xuiwib = 62; xuiwib > 0; xuiwib--) {
			continue;
		}
	}
	if (1137 != 1137) {
		int hbokiydg;
		for (hbokiydg = 94; hbokiydg > 0; hbokiydg--) {
			continue;
		}
	}
	if (58593 == 58593) {
		int su;
		for (su = 37; su > 0; su--) {
			continue;
		}
	}
	return 14030;
}

bool uoistjb::mpatcsjevbgmz(int tsfhagnypqq, bool xnajkoutqffyog, double clsxdtxvvpalrb) {
	double mkzysubdjdw = 39878;
	string snbvzzbmtrvw = "pzjdrinrflvgkqatpfuackmawninfgbdvbxigacbyrpebcsgaullgbthwvulukxtqualmkgpjqohwwdqjd";
	string dnpsjh = "mdaaerksgrtizcrcclpfjoirkfilnyifgowxfekwongphmyuqmcbmxvjhjgqde";
	string dswwxzqtsczkuw = "iznnbgtungikdeprafkc";
	bool hjxgwragtfy = true;
	string ktyhckramc = "jlhludmkyxaawmjorhztfbrohnaqwezqnrvtntuypwwziireqsboaefosgnxhfvrovtdyxkzbjzera";
	return false;
}

void uoistjb::ylymdqvhncio() {
	bool vhrrjqtrjdlxf = false;
	double qpiioj = 54491;
	if (false == false) {
		int td;
		for (td = 57; td > 0; td--) {
			continue;
		}
	}
	if (54491 == 54491) {
		int uava;
		for (uava = 5; uava > 0; uava--) {
			continue;
		}
	}
	if (false != false) {
		int ttz;
		for (ttz = 7; ttz > 0; ttz--) {
			continue;
		}
	}

}

void uoistjb::vmmerhjply(int oanzrgwovfzpmv, string jimon, double zepou, double iytlnyej, string ttcnvadehtu, double vhiqrucjn, int kqeocehzmds) {
	double mrokdt = 61214;
	double tarxjb = 42363;
	string utvouaylzb = "hfacfigoasbqgkvfyf";
	double kkmqeib = 75343;
	double hdhxxndmi = 11160;
	if (11160 == 11160) {
		int sikbmbav;
		for (sikbmbav = 61; sikbmbav > 0; sikbmbav--) {
			continue;
		}
	}
	if (75343 != 75343) {
		int eflr;
		for (eflr = 77; eflr > 0; eflr--) {
			continue;
		}
	}

}

uoistjb::uoistjb() {
	this->bppbbdmowpxswaogfz(string("gtsaieznmvknadsfjlzewfedunqlsdnxhschhjbpmjbkeaqisjbyrqjfrglspcrenxzsvpdulxdevmsadsnjyeqfhuojlcdt"), true, 39736, 59, 32186, false, 6223, true);
	this->upfxkozcmhvcamprfkap(337, 3434, 1639, 2255, string("gklkxpvdlwmolsilsuqppxxbpouwjyxpvtgh"), 13024, true, 18712);
	this->byyaajrhyawtkorrfnzh(string("f"), true, 29612, string("tz"), 24387, string("covlluctbfaqrdylzbgilqengipdpowjxfjuqxnwkty"));
	this->rfdjurhyqnsrftutqcjk(10100, string("vtnwvdgcuqgjsrygmqjrifbuebgdfbbvqexpmhaehrdrqsrpzifvxqxgpvbpcarsekysjthgwriggxmvpcipwxmjtkiambqgsqxw"), true, 2507);
	this->mpatcsjevbgmz(1123, true, 6450);
	this->ylymdqvhncio();
	this->vmmerhjply(3779, string("vhwnuvgvdcw"), 5334, 21403, string("ndbgmcdtajwqodjfeshxjwikandnyjxifjhqdpctbhezoqkshkvoojpxtykc"), 13558, 794);
	this->fkgovzoyutlk(string("hmmdjgzvzqe"), 72013, string("dldsdwcltpzeklfadpidvrmesssnwljgbggqtxfltpqbtnsdufddmgvlhwsrmfzkrspzjczedqowiuhrcyachqmqebebyvjb"), string("oesoibfqxtsfqowc"), 1681);
	this->gmsqhhjshtstzfrqi(81156, 23906, string("njpehgtakymmpxfjvkcqtadrdkovmonroqdysy"), 744, string("zzmnkuafrdqxbbrvnqtmweyqkkpijyobrpjtjmrjsvelqmcyrdgeavypokejcnbbhjurawwyuejjuvxrpgushlzrlbnbpwcrgulg"), 1442, 443, false, string("xxgxwhhjnuvbzeraaxsy"));
	this->nojpzuiybokefzth(1628, 142, 51580, 1534, 1563, 8367, 2731, false, false);
	this->lzxhufqrorxnfzhmgqv(false, string("ibbubzkqoybeesc"), true, true);
	this->gxomxjjtpmsiymetkekqqbrp(8995, 1920);
	this->sccyhbhvodbfhdzdhlwnbsokz(string("wyl"), true, true, false, true, true);
	this->rfwuoifxikb(true, 5163, 1232);
}



#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class zfdrfpv {
public:
	bool izbhf;
	string doelggitm;
	zfdrfpv();
	double oacxbmieegh(string wasvrgpvmwkwqi, string snazaftfaj, string umblanckgcscx);
	bool yemxaiyivkfxwsdr(bool qvthjdinc, int rmoqtattzyquyxa, bool meiezfribnnzxj, double jrhdusxxav, bool ayeclozpwk, int gigcvtohopzsdk, string mieojmulrtba, int vtynjqrmuopw, int dugetvyppjn);
	string xprflexlmyebonxsvsjcp(string cvwfmjwdwbbcomk, bool xbxxqmtqxdxnnx, bool prruzdo, string mxncyvzeprush, string ajpfqtezte, int ikanubwybfg);
	string tnbsbklrrwwpywh(double fabfjrikb, int ggkft, int ybixcbomtf, bool wrysaecuyousoqz, double coxdqj);
	string tdbwmbpykpnnzy(string hpadpqjllee, bool finrpwqsrlhmf, int tqgfvsdh, double jdmzxwvonppc, string ouhrrhsgtjtx, double ifqhyzy);
	void lekueoeihmtltospta(int xrpme, string gvwgyqpkgymncon, int tejmfcwbzskvrhr, string grufa);
	string aagawcgnxlmshfsfpozill(double icuolyeziyfvd, double haeqavgkrgaeo, bool okknjjapaj, string cnritdwo, bool ufysripspfbnste, bool yucxyoedikhuwz, string yimnv);
	bool jixcgqnorizjletqbptkfe(int pgnziyf);

protected:
	double hjhkgxglnabggnj;
	double xqubmmjl;
	bool yrzbgjpoukyr;
	int oazayojlvwswpyu;

	bool rshwkxpqjilvx(int xryejhgtvdgh, double fgxrobnxdhlr, string txbovw);
	string dabtwmecirxeliszqgypka();
	double eggyaflqmrkbbqtjogsito(string ehnrlqc, bool mvvfjpryyk, bool piqhc, string vxhpgkxgedlkez, double sdcycgmnhbxttje);

private:
	int pdnlfcrkc;
	string exqwymmvajuuvb;
	int evzpx;

	void sxxzkvbkyzdlkl(string ijacgknk, double ixpsgxxyz, string faohesbcxwctbw, int choanhteqzjs, bool wvtmvafspkimi, bool qegrjh, double fowudwzmct, string oqiyfuuccx, bool hvoxjjb);
	bool vocktjxoizkxnpqmm(string kbhxwu, int picnubwgdvddo, double axddibjnqiwojk, int mbyhuztfmzgtpn, string rvrjcprh);
	string nnbzkplgbwh();
	string fxdyfsdiseuq(bool ldyomezxw, string dvvpu, bool kbrvbcrmpfp, int surmi, int tcdurjskvso, string fffxgogiubrooi, bool wvlbkoicvfukks, string kjcqhikspm, bool ejaobuspq);

};


void zfdrfpv::sxxzkvbkyzdlkl(string ijacgknk, double ixpsgxxyz, string faohesbcxwctbw, int choanhteqzjs, bool wvtmvafspkimi, bool qegrjh, double fowudwzmct, string oqiyfuuccx, bool hvoxjjb) {
	bool hiobwlysnebfi = true;
	string iqubcfofumbnhn = "uljyefdocjgegniekmwgoxjbdmzciircqdjumgharnfostzfedp";
	int xramkyrpggs = 607;
	bool gggkm = false;
	double ilpxahusc = 39997;
	double nugappdgtj = 16046;
	double jgteqo = 1340;
	double ojljl = 9004;
	int inpkw = 145;
	if (1340 == 1340) {
		int mcotoji;
		for (mcotoji = 19; mcotoji > 0; mcotoji--) {
			continue;
		}
	}
	if (16046 == 16046) {
		int tw;
		for (tw = 78; tw > 0; tw--) {
			continue;
		}
	}
	if (16046 != 16046) {
		int fzsnr;
		for (fzsnr = 44; fzsnr > 0; fzsnr--) {
			continue;
		}
	}

}

bool zfdrfpv::vocktjxoizkxnpqmm(string kbhxwu, int picnubwgdvddo, double axddibjnqiwojk, int mbyhuztfmzgtpn, string rvrjcprh) {
	string lefakgergsx = "iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd";
	if (string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd") != string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd")) {
		int vcbbeck;
		for (vcbbeck = 84; vcbbeck > 0; vcbbeck--) {
			continue;
		}
	}
	if (string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd") == string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd")) {
		int dirjhtz;
		for (dirjhtz = 40; dirjhtz > 0; dirjhtz--) {
			continue;
		}
	}
	if (string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd") == string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd")) {
		int tbtt;
		for (tbtt = 97; tbtt > 0; tbtt--) {
			continue;
		}
	}
	if (string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd") != string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd")) {
		int ltlmyeh;
		for (ltlmyeh = 74; ltlmyeh > 0; ltlmyeh--) {
			continue;
		}
	}
	if (string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd") == string("iikqcmrvduozswclhgydovdkdqmpndbnmrjgtjmhtspucrijd")) {
		int pn;
		for (pn = 86; pn > 0; pn--) {
			continue;
		}
	}
	return true;
}

string zfdrfpv::nnbzkplgbwh() {
	double hmfkvcxmctd = 17178;
	bool rrnpjczicddxx = true;
	double jnzhzwv = 104;
	string velgnmixh = "ophfcaodykhf";
	bool xvzhebf = true;
	double ozialqljmgcf = 32714;
	return string("iehpunb");
}

string zfdrfpv::fxdyfsdiseuq(bool ldyomezxw, string dvvpu, bool kbrvbcrmpfp, int surmi, int tcdurjskvso, string fffxgogiubrooi, bool wvlbkoicvfukks, string kjcqhikspm, bool ejaobuspq) {
	double oxfmc = 12777;
	string hfmttlnsgs = "ygkadmdmcrlymetnpgknocthhzsmdkxehhkysdzefx";
	bool yzzjwxbxxb = false;
	int urhyjxkczgzul = 2479;
	double xuqzcjcd = 9057;
	bool yxaohstsuz = true;
	double vrswtiztuuqp = 4593;
	bool awerrveawsx = false;
	bool otwqqrozyo = false;
	if (false != false) {
		int cavmrcaz;
		for (cavmrcaz = 38; cavmrcaz > 0; cavmrcaz--) {
			continue;
		}
	}
	if (9057 == 9057) {
		int vogy;
		for (vogy = 98; vogy > 0; vogy--) {
			continue;
		}
	}
	if (false == false) {
		int igfehfklgz;
		for (igfehfklgz = 71; igfehfklgz > 0; igfehfklgz--) {
			continue;
		}
	}
	if (false != false) {
		int dugygwoddt;
		for (dugygwoddt = 10; dugygwoddt > 0; dugygwoddt--) {
			continue;
		}
	}
	return string("fzxfzqftyrsxx");
}

bool zfdrfpv::rshwkxpqjilvx(int xryejhgtvdgh, double fgxrobnxdhlr, string txbovw) {
	int kdujdujapqc = 1032;
	int esjenpgoon = 6362;
	double oqzjtnmqdjchvgl = 429;
	double dxgghiahrptt = 6129;
	string biszievdkbmud = "rjevbhleltbceqmlutznqzldmwqrdxwwxnbtbpaakkdmikeqfnwwyggecimjtzvbcpiybkpjozeihuokrdymppbseonwuquxwqty";
	bool xhqfnstwjmkkzyq = true;
	if (true != true) {
		int zjrfpar;
		for (zjrfpar = 11; zjrfpar > 0; zjrfpar--) {
			continue;
		}
	}
	if (1032 == 1032) {
		int pxyexuge;
		for (pxyexuge = 74; pxyexuge > 0; pxyexuge--) {
			continue;
		}
	}
	if (6129 == 6129) {
		int lgg;
		for (lgg = 43; lgg > 0; lgg--) {
			continue;
		}
	}
	if (true == true) {
		int qrdpmao;
		for (qrdpmao = 70; qrdpmao > 0; qrdpmao--) {
			continue;
		}
	}
	if (true != true) {
		int gowfhbrcs;
		for (gowfhbrcs = 34; gowfhbrcs > 0; gowfhbrcs--) {
			continue;
		}
	}
	return false;
}

string zfdrfpv::dabtwmecirxeliszqgypka() {
	bool ryqbfrfrkns = false;
	int izxhtuzrvsvdbwo = 916;
	string hhwlfbh = "ikecxejccxnkhmfinzxupzklkvkijulozhmbfhhtnybfosrzqkwvklmcnoadsdupcsqxnosoe";
	double mozdoyotxv = 39343;
	int elxlcyu = 2727;
	if (string("ikecxejccxnkhmfinzxupzklkvkijulozhmbfhhtnybfosrzqkwvklmcnoadsdupcsqxnosoe") != string("ikecxejccxnkhmfinzxupzklkvkijulozhmbfhhtnybfosrzqkwvklmcnoadsdupcsqxnosoe")) {
		int qydllic;
		for (qydllic = 94; qydllic > 0; qydllic--) {
			continue;
		}
	}
	return string("bbwe");
}

double zfdrfpv::eggyaflqmrkbbqtjogsito(string ehnrlqc, bool mvvfjpryyk, bool piqhc, string vxhpgkxgedlkez, double sdcycgmnhbxttje) {
	return 94915;
}

double zfdrfpv::oacxbmieegh(string wasvrgpvmwkwqi, string snazaftfaj, string umblanckgcscx) {
	double zynbvgtsbkojrp = 13547;
	int pieyqzzek = 363;
	bool vbklzksgmtdfu = false;
	bool oibzacn = false;
	string cfthlkww = "cthvswymupocmaqbgacfivrdfzi";
	string dfvtddoocbloxn = "dyfm";
	int pwfeap = 2041;
	int ilpmoeaoauejzd = 1634;
	int ypieolikjcmg = 4159;
	bool iyggxeusw = true;
	if (string("cthvswymupocmaqbgacfivrdfzi") != string("cthvswymupocmaqbgacfivrdfzi")) {
		int almt;
		for (almt = 88; almt > 0; almt--) {
			continue;
		}
	}
	if (4159 == 4159) {
		int ms;
		for (ms = 50; ms > 0; ms--) {
			continue;
		}
	}
	if (true != true) {
		int zlflvlxedl;
		for (zlflvlxedl = 19; zlflvlxedl > 0; zlflvlxedl--) {
			continue;
		}
	}
	if (2041 == 2041) {
		int fzii;
		for (fzii = 17; fzii > 0; fzii--) {
			continue;
		}
	}
	return 17074;
}

bool zfdrfpv::yemxaiyivkfxwsdr(bool qvthjdinc, int rmoqtattzyquyxa, bool meiezfribnnzxj, double jrhdusxxav, bool ayeclozpwk, int gigcvtohopzsdk, string mieojmulrtba, int vtynjqrmuopw, int dugetvyppjn) {
	return true;
}

string zfdrfpv::xprflexlmyebonxsvsjcp(string cvwfmjwdwbbcomk, bool xbxxqmtqxdxnnx, bool prruzdo, string mxncyvzeprush, string ajpfqtezte, int ikanubwybfg) {
	bool bnqlkqzn = false;
	bool pvxuo = false;
	bool pkzntuclgcwwuzr = false;
	int xxqdcqlvlr = 60;
	double secln = 44138;
	bool sohkudwanr = true;
	string doklptfuhxaazfi = "neelemratwhibfamnpdiduuuukzgawgletwbyqnc";
	int pyruirqhgswjov = 4612;
	if (string("neelemratwhibfamnpdiduuuukzgawgletwbyqnc") != string("neelemratwhibfamnpdiduuuukzgawgletwbyqnc")) {
		int wszgaxots;
		for (wszgaxots = 9; wszgaxots > 0; wszgaxots--) {
			continue;
		}
	}
	if (true != true) {
		int yup;
		for (yup = 27; yup > 0; yup--) {
			continue;
		}
	}
	if (44138 != 44138) {
		int zkamvk;
		for (zkamvk = 5; zkamvk > 0; zkamvk--) {
			continue;
		}
	}
	if (4612 == 4612) {
		int tsvj;
		for (tsvj = 43; tsvj > 0; tsvj--) {
			continue;
		}
	}
	return string("xvohhugvbbgsz");
}

string zfdrfpv::tnbsbklrrwwpywh(double fabfjrikb, int ggkft, int ybixcbomtf, bool wrysaecuyousoqz, double coxdqj) {
	bool ejrle = true;
	string yeaycgh = "jxvhfmfqzdehshuhbwiwmttnrpctlpgydxwcpxfpfezmxwzcqzjlrtabeqommenuzehazysvrrdurrvjtkisprze";
	double wbgdrtjupacnit = 61568;
	bool rubyrij = true;
	int ajlmijbifxld = 2962;
	int hhmhd = 251;
	string omntjl = "fofeacnfwhkjwjbfyqrtlua";
	int rbdihuqg = 1580;
	string pepkkex = "liympcnclayvyftgclinannwllqysfhnymvygpzqgrzvsmyneqcaslbreskqechituhkscwjdmjnxwdethmsvothnexfg";
	if (61568 == 61568) {
		int tnninhzrei;
		for (tnninhzrei = 25; tnninhzrei > 0; tnninhzrei--) {
			continue;
		}
	}
	if (251 != 251) {
		int bq;
		for (bq = 100; bq > 0; bq--) {
			continue;
		}
	}
	if (2962 != 2962) {
		int ldd;
		for (ldd = 28; ldd > 0; ldd--) {
			continue;
		}
	}
	return string("lwvtrqnmtnchezqterob");
}

string zfdrfpv::tdbwmbpykpnnzy(string hpadpqjllee, bool finrpwqsrlhmf, int tqgfvsdh, double jdmzxwvonppc, string ouhrrhsgtjtx, double ifqhyzy) {
	string udoiwxdygwfyzs = "zyxlxylbmdiorapghvdeeyzreypzbzsaohezfbnzquisarlzxvkilhyhxoljadaxfxmbmuqxidmlbibtpxyhomadsuyllalsr";
	double nvjxs = 35782;
	bool yvhnkox = true;
	return string("hpojruaiipcgqe");
}

void zfdrfpv::lekueoeihmtltospta(int xrpme, string gvwgyqpkgymncon, int tejmfcwbzskvrhr, string grufa) {
	string zncyfe = "pcqizegqqjwrrsqbbikvhkyviywwpb";

}

string zfdrfpv::aagawcgnxlmshfsfpozill(double icuolyeziyfvd, double haeqavgkrgaeo, bool okknjjapaj, string cnritdwo, bool ufysripspfbnste, bool yucxyoedikhuwz, string yimnv) {
	int lywpkpvxjjve = 6925;
	return string("aubfdyae");
}

bool zfdrfpv::jixcgqnorizjletqbptkfe(int pgnziyf) {
	double dixkuxnmjew = 49577;
	string zdzywta = "vgdmghbhawjiyjakabnwhjlbwfrunywyjrgvrgfzgndhtwnbtpzqynoxknyvstzobvuajbsplztvf";
	string sseyfpvaowrik = "udqstihwdcfuurhcpvkrsgmyldrjeisbmdjzpguipluynzmguudu";
	double diohivwgppl = 28291;
	double mscplcyfzoituu = 10802;
	if (10802 == 10802) {
		int rzwl;
		for (rzwl = 8; rzwl > 0; rzwl--) {
			continue;
		}
	}
	if (49577 == 49577) {
		int onp;
		for (onp = 86; onp > 0; onp--) {
			continue;
		}
	}
	if (49577 != 49577) {
		int lfwprhr;
		for (lfwprhr = 42; lfwprhr > 0; lfwprhr--) {
			continue;
		}
	}
	return false;
}

zfdrfpv::zfdrfpv() {
	this->oacxbmieegh(string("gsbbqcooyumyruqpspjoaxgxjsmxhxhvczzzszhykxowftsfrdqdpwidnsxtobsjmk"), string("aipglepehpeeqjjrvjhxprjdokkmoztuwslnxrskmqpfeyeafqccjdmnhszrissmfoyjwghzovfozezkurnosvsxqdy"), string("csvtpltksjdzdajptkjrgvokuphusfifunzsovhnopfjghavsfxygwveknfrtrtklsxwuy"));
	this->yemxaiyivkfxwsdr(true, 4694, true, 18397, false, 3412, string("ytlmulmtkycielhgijtvmrlwogyffqwmzwxsebr"), 7274, 1750);
	this->xprflexlmyebonxsvsjcp(string("zbjbglbizriqtoksvngwdfifjcfthsphjryedoveswiuegbmmqpyspyx"), false, false, string("aexbxnozkjbzrrzfrelsevx"), string("ptvhwwufriq"), 3438);
	this->tnbsbklrrwwpywh(37426, 3346, 929, false, 11290);
	this->tdbwmbpykpnnzy(string("ioqowjugmbtfsvvwkzaetccmdhnxlokcgsvreekwvctavrrmbootqemja"), false, 5289, 16760, string("nsogjslhpmoiewcrvevftcqlzkdcnfobxthtfvxhlhhvyjggityfelcnlhxgrsvmafpphmytpbdtplwdwvrbmplppffpxw"), 9565);
	this->lekueoeihmtltospta(4509, string("zilfhscaoxlmzdpfcdwpylzfrgeihpujutrxrdmd"), 1372, string("qewhlbmoynoilpumtkyacmmcmojmgueoykvkwqllhmfznqbxwrypgqnbtdcdcqaljkzkbfuqajerzul"));
	this->aagawcgnxlmshfsfpozill(86065, 6241, true, string("czcoxpajhjhnvp"), false, true, string("uwkhyobfpycrogtshqcogxkhbckdftqiwlqxobrsbigfpmqpds"));
	this->jixcgqnorizjletqbptkfe(1910);
	this->rshwkxpqjilvx(1696, 29340, string("ysrbvzufsbmwegvrajuppwg"));
	this->dabtwmecirxeliszqgypka();
	this->eggyaflqmrkbbqtjogsito(string("ixsrebytpbpvtqvgzadvybdqbhuvggeoyvhvoqzaspygtlzkfbqb"), false, true, string("klupgpjgvzlikhowslxqlcydqtickyakmojbaaftieomhezfxmmkvzzdwrrobqwdcd"), 20498);
	this->sxxzkvbkyzdlkl(string("qqwilmmfcrunyrmgfcqw"), 15060, string("opouxzdtmaptpylpk"), 7492, false, false, 29597, string("htcfottxrdhbnazjfcwijzvorszefzfeeuhmypcfdkrdgqfczzdhexqwgfawparxrqiolqg"), false);
	this->vocktjxoizkxnpqmm(string("sznxygndaafzebejgxuubrz"), 4779, 18160, 3790, string("wzxxtnsvkrjzbymraeszvanupywkfvyojfjneswhhnuusstqenoaprkethcwjofmjpxxbgfbvrjzapzeffelsyqykghxfayeyox"));
	this->nnbzkplgbwh();
	this->fxdyfsdiseuq(false, string("stgldzxfrrovcspacalriafmhhybgsxyjeduv"), false, 895, 512, string("jwhyagfwrerhiqqwolvqujpsxqhkclpyzsmrgfrbguxsfvvkbupvmmzhphhcmmyyifrtmxylueug"), true, string("ypsuofxtwyhozdlrgfqbslcjofmalhng"), false);
}
                                       

















































































































































































































































































