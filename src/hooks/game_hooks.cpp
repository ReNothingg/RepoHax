#include <src/pch.h>

#include <src/cheat.h>
#include <src/game_classes.h>
#include <src/resource.h>
#include <src/overlay/esp/esp.h>
#include "eye_laser_protocol.h"

#include <float.h>
#include <deque>
#include <vector>

using namespace System;
using namespace Unity;

namespace Cheat
{
    extern void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name);
    extern void HookModuleProc(HMODULE module, const char* procName, void* procHook, SafetyHookInline& out);

    static void* Hooked_MonoRuntimeInvoke(void* a1, void* a2, void* a3, void* a4);
    static void Hooked__EventSystem_Update(Unity::EventSystem __this);
    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int damage, bool savingGrace, int enemyIndex, bool hurtByHeal);
    static void Hooked__PlayerAvatar_PlayerDeath(PlayerAvatar __this, int enemyIndex);
    static void Hooked__PlayerController_FixedUpdate(PlayerController __this);
    static void Hooked__PlayerController_Update(PlayerController __this);
    static void Hooked__PlayerTumble_TumbleRequest(PlayerTumble __this, bool isTumbling, bool playerInput);
    static void Hooked__FlashlightController_Update(FlashlightController __this);
    static void Hooked__ItemUpgrade_Update(ItemUpgrade __this);
    static void Hooked__ItemHealthPack_Update(ItemHealthPack __this);
    static void Hooked__UpdateLoop();
    static void Hooked__PhysGrabObject_PhysicsGrabbingManipulation(PhysGrabObject __this);
    static void Hooked__PostProcessLayer_BuildCommandBuffers(Unity::PostProcessLayer __this);
    static void Hooked__PlayerAvatarVisuals_Update(PlayerAvatarVisuals __this);
    static void Hooked__PhysGrabber_PhysGrabLogic(PhysGrabber __this);
    static void Hooked__PhysGrabber_RayCheck(PhysGrabber __this, bool grab);
    static bool Hooked__Physics_Raycast(void* v1, void* v2, void* v3, float maxDistance, int v5, int v6);
    static void Hooked__PhysGrabObjectImpactDetector_Break(PhysGrabObjectImpactDetector __this, float a1, void* a2, int a3, bool a4);
    static void Hooked__PhysGrabObjectImpactDetector_DestroyObject(PhysGrabObjectImpactDetector __this, bool effects, bool forced);
    static bool Hooked__PhotonNetwork_IsMasterClient();
    static void Hooked__ItemBattery_Update(ItemBattery __this);
    static void Hooked__Camera_FireOnPreRender(Unity::Camera __this);
    static void Hooked__ItemGun_Update(ItemGun __this);
    static void Hooked__RunManager_SetRunLevel(RunManager __this);
    static void Hooked__PhysGrabber_PhysGrabOverCharge(PhysGrabber __this, float _amount, float _multiplier);
    static void Hooked__SpectateCamera_HeadEnergyLogic(SpectateCamera __this);
    static void Hooked__PostLateUpdateLoop();
    static void Hooked__PresentAfterDrawLoop();
    static void Hooked__EnemyRigidbody_FixedUpdate(EnemyRigidbody __this);
    static void Hooked__DataDirector_SaveDeleteCheck(DataDirector __this, bool leaveGame);

    static bool IsInGame();
    static void* GetPlayerLoopPtr(System::Type type, System::Type subType);
    static void Heal(PhysGrabObjectImpactDetector obj, float value);
    static void SpawnItem(Item item);
    static void SpawnCosmeticBox(int rarity);
    static void ParseEnemies();
    static void ParseItems();
    static void ParseLevels();
    static void UpdatePlayerUpgradeLevels(PlayerAvatar avatar);
    static void ApplyPlayerUpgradeChange(PlayerAvatar avatar);
    static void UpdateSessionSafety(PlayerAvatar avatar);
    static void ProcessWorldSessionCommands(PlayerAvatar avatar);
    static void ProcessCurrencyCommands();
    static void ProcessQuotaCommands();
    static void ProcessSurplusBagCommand();
    static void ProcessTeleportCommands(PlayerAvatar avatar);
    static void ProcessObjectRemover(PlayerAvatar avatar);
    static void MaintainFriendEffects();
    static void ProcessGodTools(PlayerAvatar avatar);
    static void DeleteGodObject(Unity::GameObject object, bool networked);
    static void UpdateEyeLasers();
    static void Hooked__EyeLaserPhotonNetwork_OnEvent(EyeLaserPhotonEvent photonEvent);
    static void DrawPlayerChams(PlayerAvatar player, Unity::CommandBuffer cb, Unity::Material aliveMat, Unity::Material deadMat);

    static bool s_ForcedPlayerDeath;
    static bool s_ForcedPlayerTumble;
    static bool s_RerollingRunLevel;
    static Level s_RunLevelToAvoid = null;

    void HookMonoRuntimeInvoke()
    {
        HookModuleProc((HMODULE)Hax::Unity::GetUvmHandle(), "mono_runtime_invoke", Hooked_MonoRuntimeInvoke, G->MonoRuntimeInvokeHook);
        ::FlushInstructionCache(::GetCurrentProcess(), nullptr, 0);
        ::Sleep(20);
    }

    void HookGameFuncs()
    {
        Hax::LogDebug(G->Logger, L"Installing game hooks...");

        #define HOOK(ptr, n) Hook(ptr, Hooked__ ## n, G-> ## n ## _Hook, #n)
        HOOK(::EventSystem::s_Update.Address, EventSystem_Update);
        HOOK(PlayerHealth::s_Hurt.Address, PlayerHealth_Hurt);
        HOOK(PlayerAvatar::s_PlayerDeath.Address, PlayerAvatar_PlayerDeath);
        HOOK(PlayerController::s_FixedUpdate.Address, PlayerController_FixedUpdate);
        HOOK(PlayerController::s_Update.Address, PlayerController_Update);
        HOOK(PlayerTumble::s_TumbleRequest.Address, PlayerTumble_TumbleRequest);
        HOOK(FlashlightController::s_Update.Address, FlashlightController_Update);
        HOOK(ItemUpgrade::s_Update.Address, ItemUpgrade_Update);
        HOOK(ItemHealthPack::s_Update.Address, ItemHealthPack_Update);

        HOOK(PhysGrabObject::s_PhysicsGrabbingManipulation.Address, PhysGrabObject_PhysicsGrabbingManipulation);
        HOOK(::PostProcessLayer::s_BuildCommandBuffers.Address, PostProcessLayer_BuildCommandBuffers);
        HOOK(PlayerAvatarVisuals::s_Update.Address, PlayerAvatarVisuals_Update);
        HOOK(PhysGrabber::s_PhysGrabLogic.Address, PhysGrabber_PhysGrabLogic);
        HOOK(PhysGrabber::s_RayCheck.Address, PhysGrabber_RayCheck);
        HOOK(::Physics::s_Raycast.Address, Physics_Raycast);
        HOOK(PhysGrabObjectImpactDetector::s_Break.Address, PhysGrabObjectImpactDetector_Break);
        HOOK(PhysGrabObjectImpactDetector::s_DestroyObject.Address, PhysGrabObjectImpactDetector_DestroyObject);
        //HOOK(::PhotonNetwork::s_IsMasterClient.Address, PhotonNetwork_IsMasterClient);
        HOOK(ItemBattery::s_Update.Address, ItemBattery_Update);
        HOOK(::Camera::s_FireOnPreRender.Address, Camera_FireOnPreRender);
        HOOK(ItemGun::s_Update.Address, ItemGun_Update);
        HOOK(RunManager::s_SetRunLevel.Address, RunManager_SetRunLevel);
        HOOK(PhysGrabber::s_PhysGrabOverCharge.Address, PhysGrabber_PhysGrabOverCharge);
        HOOK(SpectateCamera::s_HeadEnergyLogic.Address, SpectateCamera_HeadEnergyLogic);
        HOOK(EnemyRigidbody::s_FixedUpdate.Address, EnemyRigidbody_FixedUpdate);
        HOOK(DataDirector::s_SaveDeleteCheck.Address, DataDirector_SaveDeleteCheck);
        HOOK(EyeLaserPhotonNetwork::s_OnEvent.Address, EyeLaserPhotonNetwork_OnEvent);
        #undef HOOK

        ::FlushInstructionCache(::GetCurrentProcess(), nullptr, 0);
    }

    void HookPlayerLoop()
    {
        HAX_PANIC(G->PlayerLoop.ReadyToHook, &G->Logger, L"LoopSystem not ready");

        Hook(G->PlayerLoop.UpdatePtr, Hooked__UpdateLoop, G->UpdateHook, "LoopSystem.Update");
        Hook(G->PlayerLoop.PostLateUpdatePtr, Hooked__PostLateUpdateLoop, G->PostLateUpdateHook, "LoopSystem.PostLateUpdate");
        Hook(G->PlayerLoop.PresentAfterDrawPtr, Hooked__PresentAfterDrawLoop, G->PresentAfterDrawHook, "LoopSystem.PresentAfterDrawLoop");
    }

    static void* Hooked_MonoRuntimeInvoke(void* a1, void* a2, void* a3, void* a4)
    {
        void* ret = G->MonoRuntimeInvokeHook.unsafe_call<void*>(a1, a2, a3, a4);

        if (G->UnityLoadedEvent != nullptr)
        {
            using Func = const char*(*)(void*);
            static Func mono_method_get_name = (Func)::GetProcAddress((HMODULE)Hax::Unity::GetUvmHandle(), "mono_method_get_name");

            Hax::StringView methodName = mono_method_get_name(a1);
            if (methodName == "Update")
            {
                ::SetEvent(G->UnityLoadedEvent);
                G->UnityLoadedEvent = nullptr;
            }
        }

        return ret;
    }

    static int ReadPlayerUpgradeLevel(System::Dictionary<System::String, System::Int32> levels, System::String steamId)
    {
        System::Int32 value(0);
        if (levels != null)
            levels.TryGetValue(steamId, &value);
        return Hax::Max(0, (int)value);
    }

    static void UpdatePlayerUpgradeLevels(PlayerAvatar avatar)
    {
        StatsManager stats = StatsManager::instance();
        if (!avatar || !stats)
            return;

        System::String steamId = SemiFunc::PlayerGetSteamID(avatar);
        if (steamId == null)
            return;

        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::Health]         = ReadPlayerUpgradeLevel(stats.playerUpgradeHealth(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::Stamina]        = ReadPlayerUpgradeLevel(stats.playerUpgradeStamina(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::ExtraJump]      = ReadPlayerUpgradeLevel(stats.playerUpgradeExtraJump(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::MapPlayerCount] = ReadPlayerUpgradeLevel(stats.playerUpgradeMapPlayerCount(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::Launch]         = ReadPlayerUpgradeLevel(stats.playerUpgradeLaunch(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::Climb]          = ReadPlayerUpgradeLevel(stats.playerUpgradeTumbleClimb(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::HeadBattery]    = ReadPlayerUpgradeLevel(stats.playerUpgradeDeathHeadBattery(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::Wings]          = ReadPlayerUpgradeLevel(stats.playerUpgradeTumbleWings(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::SprintSpeed]    = ReadPlayerUpgradeLevel(stats.playerUpgradeSpeed(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::CrouchRest]     = ReadPlayerUpgradeLevel(stats.playerUpgradeCrouchRest(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::GrabStrength]   = ReadPlayerUpgradeLevel(stats.playerUpgradeStrength(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::ThrowStrength]  = ReadPlayerUpgradeLevel(stats.playerUpgradeThrow(), steamId);
        G->PlayerUpgradeLevels[(int)PlayerUpgradeType::GrabRange]      = ReadPlayerUpgradeLevel(stats.playerUpgradeRange(), steamId);
    }

    static void ApplyPlayerUpgradeChange(PlayerAvatar avatar)
    {
        PlayerUpgradeType type = G->PlayerUpgradeToChange;
        if (type == PlayerUpgradeType::N)
            return;

        int delta = G->PlayerUpgradeDelta;
        G->PlayerUpgradeToChange = PlayerUpgradeType::N;
        G->PlayerUpgradeDelta = 0;

        PunManager manager = PunManager::instance();
        if (!avatar || !manager || delta == 0)
            return;

        int& current = G->PlayerUpgradeLevels[(int)type];
        if (delta < 0 && current <= 0)
            return;

        System::String steamId = SemiFunc::PlayerGetSteamID(avatar);
        if (steamId == null)
            return;

        switch (type)
        {
            case PlayerUpgradeType::Health:         current = manager.UpgradePlayerHealth(steamId, delta); break;
            case PlayerUpgradeType::Stamina:        current = manager.UpgradePlayerEnergy(steamId, delta); break;
            case PlayerUpgradeType::ExtraJump:      current = manager.UpgradePlayerExtraJump(steamId, delta); break;
            case PlayerUpgradeType::MapPlayerCount: current = manager.UpgradeMapPlayerCount(steamId, delta); break;
            case PlayerUpgradeType::Launch:         current = manager.UpgradePlayerTumbleLaunch(steamId, delta); break;
            case PlayerUpgradeType::Climb:          current = manager.UpgradePlayerTumbleClimb(steamId, delta); break;
            case PlayerUpgradeType::HeadBattery:    current = manager.UpgradeDeathHeadBattery(steamId, delta); break;
            case PlayerUpgradeType::Wings:          current = manager.UpgradePlayerTumbleWings(steamId, delta); break;
            case PlayerUpgradeType::SprintSpeed:    current = manager.UpgradePlayerSprintSpeed(steamId, delta); break;
            case PlayerUpgradeType::CrouchRest:     current = manager.UpgradePlayerCrouchRest(steamId, delta); break;
            case PlayerUpgradeType::GrabStrength:   current = manager.UpgradePlayerGrabStrength(steamId, delta); break;
            case PlayerUpgradeType::ThrowStrength:  current = manager.UpgradePlayerThrowStrength(steamId, delta); break;
            case PlayerUpgradeType::GrabRange:      current = manager.UpgradePlayerGrabRange(steamId, delta); break;
            default: break;
        }
        current = Hax::Max(0, current);
    }

    static void SetSessionAction(const wchar_t* text)
    {
        if (!text)
            text = L"";

        wcsncpy_s(G->SessionLastAction, _countof(G->SessionLastAction), text, _TRUNCATE);
        Hax::Log(G->Logger, L"Session: %ls", G->SessionLastAction);
    }

    static void SetSessionActionF(const wchar_t* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vswprintf_s(G->SessionLastAction, _countof(G->SessionLastAction), fmt, args);
        va_end(args);

        Hax::Log(G->Logger, L"Session: %ls", G->SessionLastAction);
    }

    static void SetSessionAction(Hax::WStringView text)
    {
        wchar_t buf[128]{};
        swprintf_s(buf, _countof(buf), L"%.*ls", (int)text.Size(), text.Data());
        SetSessionAction(buf);
    }

    static bool IsPlayerUsable(PlayerAvatar avatar)
    {
        return avatar && !avatar.deadSet() && !avatar.isDisabled();
    }

    static Unity::Transform GetPlayerTransformSafe(PlayerAvatar avatar)
    {
        if (!avatar)
            return null;

        Unity::Transform transform = avatar.playerTransform();
        if (!transform)
            transform = avatar.GetTransform();

        return transform;
    }

    static bool TeleportAvatar(PlayerAvatar avatar, const Unity::Vector3& position)
    {
        Unity::Transform transform = GetPlayerTransformSafe(avatar);
        if (!avatar || !transform)
            return false;

        transform.SetPosition(position);

        if (avatar.isLocal())
        {
            if (PlayerController controller = PlayerController::instance())
            {
                controller.GetTransform().SetPosition(position);
                if (Unity::Rigidbody body = controller.GetComponent<Unity::Rigidbody>())
                    body.SetVelocity(Unity::Vector3::zero());
            }
        }

        if (Unity::Rigidbody body = avatar.GetComponent<Unity::Rigidbody>())
            body.SetVelocity(Unity::Vector3::zero());

        return true;
    }

    static bool GetTruckPosition(Unity::Vector3& out)
    {
        if (TruckSafetySpawnPoint truck = TruckSafetySpawnPoint::instance())
        {
            out = truck.GetTransform().GetPosition() + Unity::Vector3::up() * 0.35f;
            return true;
        }

        return false;
    }

    static ExtractionPoint GetPreferredExtractionPoint()
    {
        RoundDirector dir = RoundDirector::instance();
        if (!dir)
            return null;

        if (ExtractionPoint current = dir.extractionPointCurrent())
            return current;

        System::List<Unity::GameObject> points = dir.extractionPointList();
        if (points != null)
        {
            for (Unity::GameObject go : points)
            {
                ExtractionPoint point = go ? go.GetComponent<ExtractionPoint>() : null;
                if (point && !point.isLocked())
                    return point;
            }
            for (Unity::GameObject go : points)
            {
                if (go)
                    if (ExtractionPoint point = go.GetComponent<ExtractionPoint>())
                        return point;
            }
        }

        return null;
    }

    static bool GetExtractionPosition(Unity::Vector3& out)
    {
        if (ExtractionPoint point = GetPreferredExtractionPoint())
        {
            out = point.GetTransform().GetPosition() + Unity::Vector3::up() * 0.35f;
            return true;
        }
        return false;
    }

    static bool GetExtractionPackingBounds(Unity::Bounds& out)
    {
        ExtractionPoint point = GetPreferredExtractionPoint();
        if (!point)
            return false;

        Unity::GameObject area = point.extractionArea();
        Unity::Collider collider = area ? area.GetComponent<Unity::Collider>() : null;
        if (!collider && area)
            collider = area.GetTransform().GetComponentInChildren<Unity::Collider>();

        if (collider && collider.GetEnabled())
        {
            out = collider.GetBounds();
            const Unity::Vector3 size = out.GetSize();
            if (size.X >= 0.75f && size.Z >= 0.75f)
                return true;
        }

        // Old maps or unusual prefabs may not expose the area collider. Keep the
        // fallback centered instead of extending every row in one direction.
        out = Unity::Bounds(point.GetTransform().GetPosition() + Unity::Vector3::up() * 1.25f,
            Unity::Vector3(3.6f, 2.5f, 3.6f));
        return true;
    }

    static bool GetNearestValuablePosition(const Unity::Vector3& origin, Unity::Vector3& out)
    {
        ValuableDirector dir = ValuableDirector::instance();
        if (!dir || dir.valuableList() == null)
            return false;

        float bestDistance = FLT_MAX;
        bool found = false;
        for (ValuableObject obj : dir.valuableList())
        {
            if (!obj || !obj.GetEnabled())
                continue;

            Unity::Vector3 pos = obj.GetTransform().GetPosition();
            float dist = Unity::Vector3::Distance(origin, pos);
            if (dist < bestDistance)
            {
                bestDistance = dist;
                out = pos + Unity::Vector3::up() * 0.5f;
                found = true;
            }
        }

        return found;
    }

    static bool GetSafePosition(Unity::Vector3& out)
    {
        if (GetTruckPosition(out))
            return true;

        if (GetExtractionPosition(out))
            return true;

        if (LevelGenerator gen = LevelGenerator::Instance())
        {
            if (LevelPoint truck = gen.LevelPathTruck())
            {
                out = truck.GetTransform().GetPosition() + Unity::Vector3::up() * 0.5f;
                return true;
            }

            System::List<LevelPoint> points = gen.LevelPathPoints();
            if (points != null && points.Count() > 0 && points[0])
            {
                out = points[0].GetTransform().GetPosition() + Unity::Vector3::up() * 0.5f;
                return true;
            }
        }

        return false;
    }

    static void ClearOneShotCommands()
    {
        G->EnemyToSpawn = null;
        G->ItemToSpawn = null;
        G->RarityToSpawn = 0;
        G->ForceNextLevel = false;
        G->SaveWorldNow = false;
        G->ReloadCurrentLevel = false;
        G->UnlockExtractionPoints = false;
        G->TeleportToTruck = false;
        G->TeleportAction = TeleportQuickAction::None;
        G->PlayerToTumble = null;
        G->PlayerToKill = null;
        G->PlayerToRevive = null;
        G->ReviveSelfRequested = false;
        G->PlayerToHeal = null;
        G->SetToZero = false;
        G->SetToMax = false;
        G->ActivateNextPoint = false;
        G->PlayerUpgradeToChange = PlayerUpgradeType::N;
        G->PlayerUpgradeDelta = 0;
        G->SavePositionSlot = -1;
        G->TeleportSavedPositionSlot = -1;
        G->ClearSavedPositionSlot = -1;
        G->CurrencyDeltaPending = 0;
        G->CurrencySetZero = false;
        G->CurrencyRepairOverflow = false;
        G->QuotaApplyRequested = false;
        G->SurplusBagSpawnRequested = false;
        G->DeleteObjectRequested = false;
        G->GodObjectAction = GodObjectCommand::None;
        G->EnemyGodAction = EnemyGodCommand::None;
        G->LootGodAction = LootGodCommand::None;
        G->PlayerGodAction = PlayerGodCommand::None;
        G->DroneLaunchRequested = false;
        G->DroneDetonateRequested = false;
        G->OrbitalRequested = false;
        G->OrbitalAtTargetRequested = false;
        G->BlackHoleRequested = false;
        G->ChaosTriggerNow = false;
    }

    static void ResetSessionState()
    {
        G->FlightEnabled = false;
        G->EyeLasersEnabled = false;
        G->ThirdPerson = false;
        G->DisableOcclusionCulling = false;
        G->NoFog = false;
        G->NoTumble = false;
        G->ObjectRemoverEnabled = false;
        G->ObjectRemoverTargetValid = false;
        G->GodTargetingEnabled = false;
        G->WorldTimeScalePercent = 100;
        G->WorldGravityOverride = false;
        G->WorldFreezePhysics = false;
        G->WorldFreezePhysicsChanged = true;
        G->WorldLightAction = WorldLightCommand::Restore;
        G->GodEnemiesPacified = false;
        G->GodEnemiesPacifiedChanged = true;
        G->GodEnemiesFrozen = false;
        G->GodEnemiesFreezeChanged = true;
        G->GodLootFrozen = false;
        G->GodLootFreezeChanged = true;
        G->GodAllPlayers = false;
        G->GodAllPlayersNoTumble = false;
        G->DisableAllPlayersDeadCheck = false;
        G->ChaosEnabled = false;
        for (PlayerAvatar& player : G->ProtectedPlayers)
            player = null;
        for (PlayerAvatar& player : G->NoTumblePlayers)
            player = null;
        ClearOneShotCommands();
        SetSessionAction(G->Loc[LocKey_ActionSafetyReset]);
    }

    static void UpdateSessionSafety(PlayerAvatar avatar)
    {
        if (G->ResetSessionSafety)
        {
            G->ResetSessionSafety = false;
            ResetSessionState();
        }

        if (!G->SessionSafetyEnabled)
            return;

        if (!G->IsInGame)
        {
            G->FlightEnabled = false;
            G->EyeLasersEnabled = false;
            G->ObjectRemoverTargetValid = false;
            G->DeleteObjectRequested = false;
            // Transitional startup/menu scenes can briefly satisfy IsInGame(). Keep the
            // expensive object scan opt-in and never carry it across a scene boundary.
            G->GodTargetingEnabled = false;
            G->GodObjectAction = GodObjectCommand::None;
            G->EnemyGodAction = EnemyGodCommand::None;
            G->LootGodAction = LootGodCommand::None;
            G->PlayerGodAction = PlayerGodCommand::None;
            for (PlayerAvatar& player : G->ProtectedPlayers)
                player = null;
            for (PlayerAvatar& player : G->NoTumblePlayers)
                player = null;
            G->TeleportAction = TeleportQuickAction::None;
            G->TeleportToTruck = false;
            return;
        }

        if (!IsPlayerUsable(avatar))
        {
            G->FlightEnabled = false;
            G->EyeLasersEnabled = false;
        }

        if (RunManager manager = RunManager::instance())
        {
            manager.allPlayersDeadCheckDisabled() = G->DisableAllPlayersDeadCheck;
            if (G->DisableAllPlayersDeadCheck && manager.allPlayersDead())
                manager.AllPlayersDeadSet(false);
        }

        if (G->AutoCancelClientUnsafe && G->IsClient)
        {
            bool hadUnsafe = false;
            if (G->EnemyToSpawn || G->ItemToSpawn || G->RarityToSpawn || G->ForceNextLevel ||
                G->ReloadCurrentLevel || G->UnlockExtractionPoints || G->PlayerToKill ||
                G->PlayerToRevive || G->PlayerToTumble ||
                G->PlayerToHeal || G->DeleteObjectRequested ||
                G->EnemyGodAction != EnemyGodCommand::None ||
                G->LootGodAction != LootGodCommand::None ||
                G->PlayerGodAction != PlayerGodCommand::None ||
                G->TeleportAction == TeleportQuickAction::SelectedPlayerToMe)
            {
                hadUnsafe = true;
            }

            G->EnemyToSpawn = null;
            G->ItemToSpawn = null;
            G->RarityToSpawn = 0;
            G->ForceNextLevel = false;
            G->ReloadCurrentLevel = false;
            G->UnlockExtractionPoints = false;
            G->PlayerToKill = null;
            G->PlayerToRevive = null;
            G->PlayerToTumble = null;
            G->PlayerToHeal = null;
            G->DeleteObjectRequested = false;
            G->EnemyGodAction = EnemyGodCommand::None;
            G->LootGodAction = LootGodCommand::None;
            G->PlayerGodAction = PlayerGodCommand::None;
            if (G->TeleportAction == TeleportQuickAction::SelectedPlayerToMe)
                G->TeleportAction = TeleportQuickAction::None;

            if (hadUnsafe)
                SetSessionAction(G->Loc[LocKey_ActionUnsafeCancelled]);
        }
    }

    static void ProcessWorldSessionCommands(PlayerAvatar avatar)
    {
        if (G->SaveWorldNow)
        {
            G->SaveWorldNow = false;
            if (StatsManager stats = StatsManager::instance())
            {
                stats.SaveFileSave();
                SetSessionAction(G->Loc[LocKey_ActionSaveDone]);
            }
            else
                SetSessionAction(G->Loc[LocKey_ActionSaveFailedStats]);
        }

        if (G->ReloadCurrentLevel)
        {
            G->ReloadCurrentLevel = false;
            RunManager manager = RunManager::instance();
            if (!G->IsInGame || !manager)
                SetSessionAction(G->Loc[LocKey_ActionReloadNotInGame]);
            else if (G->IsClient)
                SetSessionAction(G->Loc[LocKey_ActionReloadHostOnly]);
            else
            {
                G->FlightEnabled = false;
                // ChangeLevelType::RunLevel (1) runs SetRunLevel, broadcasts the
                // selected level through the game's buffered RPC, then reloads
                // the scene without counting the current level as completed.
                s_RerollingRunLevel = true;
                s_RunLevelToAvoid = manager.levelCurrent();
                manager.ChangeLevel(false, false, 1);
                s_RerollingRunLevel = false;
                s_RunLevelToAvoid = null;
                SetSessionAction(G->Loc[LocKey_ActionReloadRequested]);
            }
        }

        if (G->UnlockExtractionPoints)
        {
            G->UnlockExtractionPoints = false;
            RoundDirector dir = RoundDirector::instance();
            if (!G->IsInGame || !dir)
                SetSessionAction(G->Loc[LocKey_ActionExtractionNotReady]);
            else if (G->IsClient)
                SetSessionAction(G->Loc[LocKey_ActionExtractionHostOnly]);
            else
            {
                dir.ExtractionPointsUnlock();
                SetSessionAction(G->Loc[LocKey_ActionExtractionUnlocked]);
            }
        }
    }

    static void ProcessCurrencyCommands()
    {
        if (!G->IsInGame)
        {
            G->CurrentRunCurrency = 0;
            G->CurrencyDeltaPending = 0;
            G->CurrencySetZero = false;
            G->CurrencyRepairOverflow = false;
            return;
        }

        StatsManager stats = StatsManager::instance();
        if (stats)
            G->CurrentRunCurrency = SemiFunc::StatGetRunCurrency();

        if (G->CurrencyDeltaPending == 0 && !G->CurrencySetZero && !G->CurrencyRepairOverflow)
            return;

        const int delta = G->CurrencyDeltaPending;
        const bool setZero = G->CurrencySetZero;
        const bool repairOverflow = G->CurrencyRepairOverflow;
        G->CurrencyDeltaPending = 0;
        G->CurrencySetZero = false;
        G->CurrencyRepairOverflow = false;

        if (G->IsClient)
        {
            SetSessionAction(G->Loc[LocKey_ActionCurrencyHostOnly]);
            return;
        }

        if (!stats)
        {
            SetSessionAction(G->Loc[LocKey_ActionCurrencyNotReady]);
            return;
        }

        long long target = setZero ? 0LL : (long long)G->CurrentRunCurrency + (long long)delta;
        if (repairOverflow)
            target = Hax::Min<long long>(target, MaxSafeRunCurrency);
        if (target < 0)
            target = 0;
        if (target > MaxSafeRunCurrency)
            target = MaxSafeRunCurrency;

        G->CurrentRunCurrency = SemiFunc::StatSetRunCurrency((int)target);
        stats.SaveFileSave();
        SetSessionActionF(G->Loc[LocKey_ActionCurrencyChangedFmt].Data(), G->CurrentRunCurrency);
    }

    static void ProcessQuotaCommands()
    {
        if (!G->IsInGame)
        {
            G->CurrentQuota = 0;
            G->QuotaApplyRequested = false;
            return;
        }

        RoundDirector director = RoundDirector::instance();
        if (director)
            G->CurrentQuota = director.extractionHaulGoal();

        if (!G->QuotaApplyRequested)
            return;
        G->QuotaApplyRequested = false;

        if (G->IsClient)
        {
            SetSessionAction(G->Loc[LocKey_ActionQuotaHostOnly]);
            return;
        }

        if (!director || director.extractionPointList() == null)
        {
            SetSessionAction(G->Loc[LocKey_ActionQuotaNotReady]);
            return;
        }

        const int target = Hax::Clamp(G->QuotaTarget, 0, 10000000);
        bool applied = false;
        for (Unity::GameObject pointObject : director.extractionPointList())
        {
            if (pointObject)
            {
                if (ExtractionPoint point = pointObject.GetComponent<ExtractionPoint>())
                {
                    point.HaulGoalSet(target);
                    applied = true;
                }
            }
        }

        if (!applied)
        {
            SetSessionAction(G->Loc[LocKey_ActionQuotaNotReady]);
            return;
        }

        G->CurrentQuota = target;
        SetSessionActionF(G->Loc[LocKey_ActionQuotaChangedFmt].Data(), target);
    }

    static void ProcessSurplusBagCommand()
    {
        if (!G->IsInGame)
        {
            G->SurplusBagSpawnRequested = false;
            return;
        }

        if (!G->SurplusBagSpawnRequested)
            return;
        G->SurplusBagSpawnRequested = false;

        if (G->IsClient)
        {
            SetSessionAction(G->Loc[LocKey_ActionSurplusBagHostOnly]);
            return;
        }

        AssetManager assets = AssetManager::instance();
        Unity::Camera camera = SemiFunc::MainCamera();
        if (!assets || !camera)
        {
            SetSessionAction(G->Loc[LocKey_ActionSurplusBagNotReady]);
            return;
        }

        const int amount = Hax::Clamp(G->SurplusBagAmount, 1, 10000000);
        Unity::GameObject prefab = assets.surplusValuableSmall();
        if (amount > 10000)
            prefab = assets.surplusValuableBig();
        else if (amount > 5000)
            prefab = assets.surplusValuableMedium();

        if (!prefab)
        {
            SetSessionAction(G->Loc[LocKey_ActionSurplusBagNotReady]);
            return;
        }

        Unity::Transform cameraTransform = camera.GetTransform();
        const Unity::Vector3 position = cameraTransform.GetPosition() + cameraTransform.GetForward() * 2.f - cameraTransform.GetUp() * 0.45f;
        Unity::GameObject bag = null;
        if (SemiFunc::IsMultiplayer())
        {
            System::String resourcePath = System::String::Concat(System::String::New(L"Valuables/"), prefab.GetName());
            bag = Unity::Photon::PhotonNetwork::InstantiateRoomObject(resourcePath, position, Unity::Quaternion::identity());
        }
        else
            bag = Unity::Object::Instantiate<Unity::GameObject>(prefab, position, Unity::Quaternion::identity());

        ValuableObject valuable = bag ? bag.GetComponent<ValuableObject>() : null;
        if (!valuable)
        {
            SetSessionAction(G->Loc[LocKey_ActionSurplusBagNotReady]);
            return;
        }

        valuable.dollarValueOverride() = amount;
        SetSessionActionF(G->Loc[LocKey_ActionSurplusBagSpawnedFmt].Data(), amount);
    }

    static void ProcessTeleportCommands(PlayerAvatar avatar)
    {
        if (G->TeleportToTruck)
        {
            G->TeleportToTruck = false;
            G->TeleportAction = TeleportQuickAction::ToTruck;
        }

        if (G->ClearSavedPositionSlot >= 0)
        {
            int slot = G->ClearSavedPositionSlot;
            G->ClearSavedPositionSlot = -1;
            if (slot >= 0 && slot < (int)_countof(G->SavedPositions))
            {
                G->SavedPositions[slot].Active = false;
                SetSessionActionF(G->Loc[LocKey_ActionPositionSlotClearedFmt].Data(), slot + 1);
            }
        }

        if (G->SavePositionSlot >= 0)
        {
            int slot = G->SavePositionSlot;
            G->SavePositionSlot = -1;
            Unity::Transform transform = GetPlayerTransformSafe(avatar);
            if (slot >= 0 && slot < (int)_countof(G->SavedPositions) && transform)
            {
                G->SavedPositions[slot].Active = true;
                G->SavedPositions[slot].Position = transform.GetPosition();
                G->SavedPositions[slot].LevelsCompleted = RunManager::instance() ? RunManager::instance().levelsCompleted() : -1;
                SetSessionActionF(G->Loc[LocKey_ActionPositionSlotSavedFmt].Data(), slot + 1);
            }
            else
                SetSessionAction(G->Loc[LocKey_ActionPositionSaveFailed]);
        }

        if (G->TeleportSavedPositionSlot >= 0)
        {
            int slot = G->TeleportSavedPositionSlot;
            G->TeleportSavedPositionSlot = -1;
            if (slot >= 0 && slot < (int)_countof(G->SavedPositions) && G->SavedPositions[slot].Active && TeleportAvatar(avatar, G->SavedPositions[slot].Position))
                SetSessionActionF(G->Loc[LocKey_ActionTeleportedSlotFmt].Data(), slot + 1);
            else
                SetSessionAction(G->Loc[LocKey_ActionTeleportedSlotFailed]);
        }

        TeleportQuickAction action = G->TeleportAction;
        if (action == TeleportQuickAction::None)
            return;
        G->TeleportAction = TeleportQuickAction::None;

        if (!IsPlayerUsable(avatar))
        {
            SetSessionAction(G->Loc[LocKey_ActionTeleportPlayerNotReady]);
            return;
        }

        Unity::Transform avatarTransform = GetPlayerTransformSafe(avatar);
        Unity::Vector3 target{};

        switch (action)
        {
            case TeleportQuickAction::ToTruck:
                if (GetTruckPosition(target) && TeleportAvatar(avatar, target))
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedTruck]);
                else
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedTruckFailed]);
                break;

            case TeleportQuickAction::ToExtraction:
                if (GetExtractionPosition(target) && TeleportAvatar(avatar, target))
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedExtraction]);
                else
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedExtractionFailed]);
                break;

            case TeleportQuickAction::ToNearestValuable:
                if (avatarTransform && GetNearestValuablePosition(avatarTransform.GetPosition(), target) && TeleportAvatar(avatar, target))
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedValuable]);
                else
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedValuableFailed]);
                break;

            case TeleportQuickAction::ToSelectedPlayer:
                if (G->SelectedTeleportPlayer && GetPlayerTransformSafe(G->SelectedTeleportPlayer) &&
                    TeleportAvatar(avatar, GetPlayerTransformSafe(G->SelectedTeleportPlayer).GetPosition() + Unity::Vector3::up() * 0.35f))
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedPlayer]);
                else
                    SetSessionAction(G->Loc[LocKey_ActionTeleportedPlayerFailed]);
                break;

            case TeleportQuickAction::SelectedPlayerToMe:
                if (G->IsClient)
                    SetSessionAction(G->Loc[LocKey_ActionBringPlayerHostOnly]);
                else if (G->SelectedTeleportPlayer && avatarTransform &&
                    TeleportAvatar(G->SelectedTeleportPlayer, avatarTransform.GetPosition() + Unity::Vector3::up() * 0.35f))
                    SetSessionAction(G->Loc[LocKey_ActionBringPlayerDone]);
                else
                    SetSessionAction(G->Loc[LocKey_ActionBringPlayerFailed]);
                break;

            case TeleportQuickAction::PanicSafe:
                if (GetSafePosition(target) && TeleportAvatar(avatar, target))
                    SetSessionAction(G->Loc[LocKey_ActionPanicDone]);
                else
                    SetSessionAction(G->Loc[LocKey_ActionPanicFailed]);
                break;

            case TeleportQuickAction::PlayerToCamera:
                if (Unity::Camera camera = SemiFunc::MainCamera())
                {
                    target = camera.GetTransform().GetPosition() + camera.GetTransform().GetForward() * 0.75f;
                    if (TeleportAvatar(avatar, target))
                        SetSessionAction(G->Loc[LocKey_ActionPlayerToCameraDone]);
                    else
                        SetSessionAction(G->Loc[LocKey_ActionCameraTeleportFailed]);
                }
                else
                    SetSessionAction(G->Loc[LocKey_ActionCameraNotReady]);
                break;

            default:
                break;
        }
    }

    static bool IsReviveSelfSessionReady()
    {
        RunManager manager = RunManager::instance();
        GameDirector director = GameDirector::instance();
        return manager && director && manager.runStarted() && !manager.gameOver() &&
            !manager.restarting() && manager.levelCurrent() != manager.levelLobbyMenu() &&
            manager.levelCurrent() != manager.levelMainMenu() &&
            (director.currentState() == GameDirector_gameState::Main() ||
             director.currentState() == GameDirector_gameState::Death());
    }

    static void ProcessReviveSelfCommand(PlayerAvatar avatar)
    {
        if (!G->ReviveSelfRequested)
            return;

        // Consume once, even if the player or session is no longer ready.
        G->ReviveSelfRequested = false;
        if (!G->ReviveSelfSessionReady || !avatar)
        {
            SetSessionAction(G->Loc[LocKey_ActionReviveSelfNotReady]);
            return;
        }
        if (G->IsClient)
        {
            SetSessionAction(G->Loc[LocKey_ActionReviveSelfHostOnly]);
            return;
        }
        if (!avatar.deadSet())
        {
            SetSessionAction(G->Loc[LocKey_ActionReviveSelfAlive]);
            return;
        }

        if (!avatar.playerDeathHead())
        {
            SetSessionAction(G->Loc[LocKey_ActionReviveSelfNotReady]);
            return;
        }

        avatar.Revive(false);
        SetSessionAction(G->Loc[LocKey_ActionReviveSelfDone]);
    }

    static void Hooked__EventSystem_Update(Unity::EventSystem __this)
    {
        try
        {
            if (__this != Unity::EventSystem::GetCurrent())
                return;

            PlayerAvatar avatar = PlayerAvatar::instance();
            PlayerHealth health = avatar ? avatar.playerHealth() : nullptr;

            G->IsClient = !SemiFunc::IsMasterClientOrSingleplayer();
            G->IsInGame = IsInGame();
            // Death is a brief camera-animation state before spectating returns to Main.
            G->ReviveSelfSessionReady = IsReviveSelfSessionReady();

            UpdateSessionSafety(avatar);
            ProcessReviveSelfCommand(avatar);
            ProcessWorldSessionCommands(avatar);
            ProcessCurrencyCommands();
            ProcessQuotaCommands();
            ProcessSurplusBagCommand();
            ProcessTeleportCommands(avatar);
            ProcessObjectRemover(avatar);
            ProcessGodTools(avatar);
            MaintainFriendEffects();

            if (G->IsInGame)
            {
                ApplyPlayerUpgradeChange(avatar);
                UpdatePlayerUpgradeLevels(avatar);
            }

            if (!G->PlayerLoop.ReadyToHook)
            {
                G->PlayerLoop.UpdatePtr = GetPlayerLoopPtr(Unity::PlayerLoop::Update::typeof(), Unity::PlayerLoop::ScriptRunBehaviourUpdate::typeof());
                Hax::LogDebug(G->Logger, L"m_UpdatePtr ptr = %p", G->PlayerLoop.UpdatePtr);

                G->PlayerLoop.PostLateUpdatePtr = GetPlayerLoopPtr(Unity::PlayerLoop::PostLateUpdate::typeof(), Unity::PlayerLoop::PlayerSendFrameStarted::typeof());
                Hax::LogDebug(G->Logger, L"m_PostLateUpdatePtr ptr = %p", G->PlayerLoop.PostLateUpdatePtr);

                G->PlayerLoop.PresentAfterDrawPtr = GetPlayerLoopPtr(Unity::PlayerLoop::PostLateUpdate::typeof(), Unity::PlayerLoop::PresentAfterDraw::typeof());
                Hax::LogDebug(G->Logger, L"m_PresentAfterDrawPtr ptr = %p", G->PlayerLoop.PresentAfterDrawPtr);

                G->PlayerLoop.ReadyToHook = true;
            }

            if (G->HealToMax)
            {
                G->HealToMax = false;
                if (health)
                    health.HealOther(99999, true);
            }

            if (G->PlayerToHeal)
            {
                PlayerAvatar player = G->PlayerToHeal;
                G->PlayerToHeal = null;
                if (!G->IsClient && player && !player.deadSet())
                {
                    if (PlayerHealth targetHealth = player.playerHealth())
                    {
                        targetHealth.HealOther(99999, true);
                        SetSessionAction(G->Loc[LocKey_ActionPlayerHealed]);
                    }
                }
            }

            if (G->UpgradeToToggle)
            {
                ItemUpgrade item = G->UpgradeToToggle;
                G->UpgradeToToggle = nullptr;

                ItemToggle toggle = item.itemToggle();
                if (toggle)
                {
                    toggle.photonView() = item.photonView();
                    toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                }
            }

            if (G->AidToToggle)
            {
                ItemHealthPack item = G->AidToToggle;
                G->AidToToggle = nullptr;

                ItemToggle toggle = item.itemToggle();
                if (toggle)
                {
                    toggle.photonView() = item.photonView();
                    toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                }
            }

            if (G->EnemiesPool.Empty())
                ParseEnemies();

            if (G->ItemsPool.Empty())
                ParseItems();

            if (G->LevelBans.Empty())
                ParseLevels();

            if (G->EnemyToSpawn)
            {
                EnemySetup enemy = G->EnemyToSpawn;
                G->EnemyToSpawn = null;

                if (LevelGenerator gen = LevelGenerator::Instance())
                {
                    System::List<LevelPoint> points = gen.LevelPathPoints();
                    if (points != null && points.Count() > 0)
                    {
                        LevelPoint point = points[0];
                        if (point)
                            gen.EnemySpawn(enemy, point.GetTransform().GetPosition());
                    }
                }
            }

            if (G->CommandBuffer == null)
            {
                G->CommandBuffer = CommandBuffer::New();
                System::GCHandle::Alloc(G->CommandBuffer, true);
            }

            if (G->Bundle == null)
            {
                auto ptr = Hax::Gui::GetResourceData((Hax::Handle)G->Handle, IDR_BUNDLE1, L"BUNDLE");

                auto bundleRaw = Array<Char>::CreateInstance(ptr.Size());

                memcpy(bundleRaw.begin(), ptr.Data(), ptr.Size());
                G->Bundle = AssetBundle::LoadFromMemory(bundleRaw);
                G->Bundle.SetHideFlags(HideFlags::DontUnloadUnusedAsset);

                Hax::LogDebug(G->Logger, L"Bundle loaded");
            }

            if (!G->ItemsChamsMat && G->Bundle)
            {
                Shader shader = As<Shader>(G->Bundle.LoadAsset(String::New("assets/myshader.shader"), Shader::typeof()));

                G->ItemsChamsMat = Material::New(shader);
                G->ItemsChamsMat.SetHideFlags(HideFlags::DontUnloadUnusedAsset);
                G->ItemsChamsMat.SetColor(Color(1.f, 1.f, 0.9f, 1.f));
                GCHandle::Alloc(G->ItemsChamsMat, true);
                Hax::LogDebug(G->Logger, L"Items chams mat loaded");

                G->PlayerChamsMat = Material::New(shader);
                G->PlayerChamsMat.SetHideFlags(HideFlags::DontUnloadUnusedAsset);
                G->PlayerChamsMat.SetColor(Color::green());
                GCHandle::Alloc(G->PlayerChamsMat, true);
                Hax::LogDebug(G->Logger, L"Player chams mat loaded");

                G->HeadChamsMat = Material::New(shader);
                G->HeadChamsMat.SetHideFlags(HideFlags::DontUnloadUnusedAsset);
                G->HeadChamsMat.SetColor(Color(0x54 / 255.f, 0x54 / 255.f, 0x54 / 255.f, 1.f));
                GCHandle::Alloc(G->HeadChamsMat, true);
                Hax::LogDebug(G->Logger, L"Head chams mat loaded");
            }

            if (G->SetToZero)
            {
                G->SetToZero = false;
                if (ValuableDirector dir = ValuableDirector::instance())
                {
                    for (ValuableObject obj : dir.valuableList())
                    {
                        if (obj)
                            Heal(obj.physGrabObject().impactDetector(), -999999.f);
                    }
                }
            }

            if (G->SetToMax)
            {
                G->SetToMax = false;
                if (ValuableDirector dir = ValuableDirector::instance())
                {
                    for (ValuableObject obj : dir.valuableList())
                    {
                        if (obj)
                            Heal(obj.physGrabObject().impactDetector(), 999999.f);
                    }
                }
            }

            if (G->ItemToSpawn)
            {
                Item item = G->ItemToSpawn;
                G->ItemToSpawn = nullptr;
                SpawnItem(item);
            }

            if (G->ForceNextLevel)
            {
                G->ForceNextLevel = false;
                if (RunManager manager = RunManager::instance())
                    manager.ChangeLevel(true, false);
            }

            if (G->ActivateNextPoint)
            {
                G->ActivateNextPoint = false;
                if (RoundDirector dir = RoundDirector::instance())
                {
                    for (Unity::GameObject go : dir.extractionPointList())
                    {
                        ExtractionPoint point = go.GetComponent<ExtractionPoint>();
                        if (point && point.currentState() == ExtractionPoint_State::Idle() && !point.isLocked())
                        {
                            point.OnClick();
                            break;
                        }
                    }
                }
            }

            if (G->PlayerToTumble)
            {
                PlayerAvatar player = G->PlayerToTumble;
                G->PlayerToTumble = nullptr;
                if (player && !player.isDisabled())
                {
                    s_ForcedPlayerTumble = true;
                    player.tumble().TumbleRequest(true, false);
                    s_ForcedPlayerTumble = false;
                }
            }

            if (G->PlayerToKill)
            {
                PlayerAvatar player = G->PlayerToKill;
                G->PlayerToKill = nullptr;
                if (player && !player.deadSet() && !G->IsClient)
                {
                    s_ForcedPlayerDeath = true;
                    player.PlayerDeath(-1);
                    s_ForcedPlayerDeath = false;
                }
            }

            if (G->PlayerToRevive)
            {
                PlayerAvatar player = G->PlayerToRevive;
                G->PlayerToRevive = nullptr;
                if (player && player.deadSet() && !G->IsClient)
                    player.Revive(false);
            }

            PlayerVoiceChat chat = avatar ? avatar.voiceChat() : nullptr;
            if (chat && avatar.voiceChatFetched() && G->ChatPrefered != ChatPref::None)
            {
                bool isDeadPrefered = G->ChatPrefered == ChatPref::Dead;
                if (chat.inLobbyMixer() != isDeadPrefered)
                {
                    chat.ToggleMixer(isDeadPrefered, false);
                    Hax::LogDebug(G->Logger, L"Chat changed");
                }
                G->ChatPrefered = ChatPref::None;
            }

            if (G->UnlockAllCosmetic && MetaManager::instance())
            {
                G->UnlockAllCosmetic = false;
                MetaManager::instance().CosmeticUnlockAll();
            }

            if (G->ResetAllCosmetic && MetaManager::instance())
            {
                G->ResetAllCosmetic = false;
                MetaManager::instance().Reset();
            }

            if (G->RarityToSpawn > 0)
            {
                int rarity = G->RarityToSpawn;
                G->RarityToSpawn = 0;
                SpawnCosmeticBox(rarity - 1);
            }
        }
        catch (Exception& ex)
        {
            String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->EventSystem_Update_Hook.unsafe_call<void, Unity::EventSystem>(__this);
    }

    static bool IsPlayerInRuntimeSlots(PlayerAvatar player, PlayerAvatar (&slots)[16])
    {
        if (!player)
            return false;

        for (PlayerAvatar entry : slots)
            if (entry == player)
                return true;
        return false;
    }

    static bool IsProtectedHealth(PlayerHealth health)
    {
        if (!health)
            return false;

        GameDirector director = GameDirector::instance();
        System::List<PlayerAvatar> players{};
        if (director)
            players = director.PlayerList();
        if (players == null)
            return false;

        for (PlayerAvatar player : players)
        {
            if (player && player.playerHealth() == health && IsPlayerInRuntimeSlots(player, G->ProtectedPlayers))
                return true;
        }
        return false;
    }

    static bool IsPlayerPresent(PlayerAvatar player, System::List<PlayerAvatar> players)
    {
        if (!player || players == null)
            return false;

        for (PlayerAvatar current : players)
            if (current == player)
                return true;
        return false;
    }

    static void MaintainFriendEffects()
    {
        if (!G->IsInGame || G->IsClient)
            return;

        GameDirector director = GameDirector::instance();
        System::List<PlayerAvatar> players{};
        if (director)
            players = director.PlayerList();
        if (players == null)
            return;

        for (PlayerAvatar& player : G->ProtectedPlayers)
        {
            if (!IsPlayerPresent(player, players))
            {
                player = null;
                continue;
            }

            if (!player.deadSet())
            {
                PlayerHealth health = player.playerHealth();
                if (health && health.health() < health.maxHealth())
                    health.HealOther(health.maxHealth() - health.health(), false);
            }
        }

        for (PlayerAvatar& player : G->NoTumblePlayers)
            if (!IsPlayerPresent(player, players))
                player = null;

        if (G->GodAllPlayers)
        {
            for (PlayerAvatar player : players)
            {
                if (!player || player.deadSet())
                    continue;
                PlayerHealth health = player.playerHealth();
                if (health && health.health() < health.maxHealth())
                    health.HealOther(health.maxHealth() - health.health(), false);
            }
        }
    }

    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int damage, bool savingGrace, int enemyIndex, bool hurtByHeal)
    {
        PlayerAvatar localPlayer = PlayerAvatar::instance();
        if (G->GodAllPlayers || (G->Godmode && localPlayer && localPlayer.playerHealth() == __this) || IsProtectedHealth(__this))
            return;

        G->PlayerHealth_Hurt_Hook.unsafe_call<void, PlayerHealth, int, bool, int, bool>(__this, damage, savingGrace, enemyIndex, hurtByHeal);
    }

    static void Hooked__PlayerAvatar_PlayerDeath(PlayerAvatar __this, int enemyIndex)
    {
        if (!s_ForcedPlayerDeath &&
            (G->GodAllPlayers || (G->Godmode && __this.isLocal()) || IsPlayerInRuntimeSlots(__this, G->ProtectedPlayers)))
            return;

        G->PlayerAvatar_PlayerDeath_Hook.unsafe_call<void, PlayerAvatar, int>(__this, enemyIndex);
    }

    struct NoclipColliderState
    {
        Unity::Collider Value;
        bool WasEnabled;
    };

    struct NoclipRuntimeState
    {
        Hax::Vector<NoclipColliderState> Colliders;
        Unity::Rigidbody Body = null;
        Unity::Vector3 OriginPosition{};
        Unity::Quaternion OriginRotation = Unity::Quaternion::identity();
        Unity::Vector3 Position{};
        float CurrentSpeed{};
        bool BodyUsedGravity{true};
        bool BodyDetectedCollisions{true};
        bool BodyWasKinematic{};
        bool PoseCaptured{};
        bool WasFlying{};
    };

    static NoclipRuntimeState s_Noclip;

    static void RestoreNoclipPhysics()
    {
        for (NoclipColliderState& state : s_Noclip.Colliders)
        {
            try
            {
                if (state.Value)
                    state.Value.SetEnabled(state.WasEnabled);
            }
            catch (System::Exception&)
            {
            }
        }
        s_Noclip.Colliders.Clear();

        try
        {
            if (s_Noclip.Body)
            {
                s_Noclip.Body.SetIsKinematic(s_Noclip.BodyWasKinematic);
                s_Noclip.Body.SetUseGravity(s_Noclip.BodyUsedGravity);
                s_Noclip.Body.SetDetectCollisions(s_Noclip.BodyDetectedCollisions);
                s_Noclip.Body.SetVelocity(Unity::Vector3::zero());
            }
        }
        catch (System::Exception&)
        {
        }
        s_Noclip.Body = null;
    }

    static void StopNoclip(PlayerController controller, PlayerAvatar avatar, bool forceStanding)
    {
        Unity::Vector3 exitPosition = s_Noclip.Position;
        try
        {
            if (controller && controller.GetTransform())
                exitPosition = controller.GetTransform().GetPosition();
        }
        catch (System::Exception&)
        {
        }

        RestoreNoclipPhysics();

        try
        {
            if (forceStanding && avatar && avatar.isTumbling() && avatar.tumble())
                avatar.tumble().TumbleSet(false, false);

            if (s_Noclip.PoseCaptured)
            {
                if (controller && controller.GetTransform())
                {
                    // Leaving noclip must keep the place reached in flight. The
                    // old origin restore caused an unexpected teleport all the
                    // way back to where noclip had been enabled.
                    controller.GetTransform().SetPosition(exitPosition);
                    if (Unity::Rigidbody body = controller.GetComponent<Unity::Rigidbody>())
                        body.SetVelocity(Unity::Vector3::zero());
                }

                if (avatar && avatar.GetTransform())
                {
                    avatar.GetTransform().SetPosition(exitPosition);
                    if (Unity::Rigidbody body = avatar.GetComponent<Unity::Rigidbody>())
                        body.SetVelocity(Unity::Vector3::zero());
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"Noclip restore: %ls",
                message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        s_Noclip.Position = {};
        s_Noclip.CurrentSpeed = 0.f;
        s_Noclip.PoseCaptured = false;
        s_Noclip.WasFlying = false;
    }

    static void Hooked__PlayerController_FixedUpdate(PlayerController __this)
    {
        try
        {
            float& walkSpeed = __this.MoveSpeed();
            float& sprintSpeed = __this.SprintSpeed();
            float& crouchSpeed = __this.CrouchSpeed();

            float backup[3] = { walkSpeed, sprintSpeed, crouchSpeed };

            if (G->InfStamina)
                __this.EnergyCurrent() = __this.EnergyStart();

            walkSpeed *= (float)G->Accel.Walking;
            sprintSpeed *= (float)G->Accel.Sprinting;
            crouchSpeed *= (float)G->Accel.Crouching;
            if (G->FpvPiloting)
            {
                // Piloting an FPV drone: W/S are the throttle, the body stays put.
                walkSpeed = 0.f;
                sprintSpeed = 0.f;
                crouchSpeed = 0.f;
            }

            G->PlayerController_FixedUpdate_Hook.unsafe_call<void, PlayerController>(__this);

            walkSpeed = backup[0];
            sprintSpeed = backup[1];
            crouchSpeed = backup[2];
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");

            G->PlayerController_FixedUpdate_Hook.unsafe_call<void, PlayerController>(__this);
        }

        try
        {
            if (!G->FlightEnabled || !G->IsInGame)
            {
                if (s_Noclip.WasFlying || s_Noclip.PoseCaptured)
                    StopNoclip(__this, PlayerAvatar::instance(), true);
                return;
            }

            PlayerAvatar avatar = PlayerAvatar::instance();
            Unity::Camera camera = SemiFunc::MainCamera();
            Unity::Rigidbody body = __this.GetComponent<Unity::Rigidbody>();
            if (!avatar || avatar.deadSet() || avatar.isDisabled() || !camera || !body)
            {
                StopNoclip(__this, avatar, true);
                G->FlightEnabled = false;
                return;
            }

            if (!s_Noclip.PoseCaptured)
            {
                if (avatar.isTumbling() && avatar.tumble())
                    avatar.tumble().TumbleSet(false, false);

                Unity::Transform transform = __this.GetTransform();
                s_Noclip.OriginPosition = transform.GetPosition();
                s_Noclip.OriginRotation = transform.GetRotation();
                s_Noclip.Position = s_Noclip.OriginPosition;
                s_Noclip.PoseCaptured = true;
            }

            if (!s_Noclip.Body)
            {
                s_Noclip.Body = body;
                s_Noclip.BodyUsedGravity = body.GetUseGravity();
                s_Noclip.BodyDetectedCollisions = body.GetDetectCollisions();
                s_Noclip.BodyWasKinematic = body.GetIsKinematic();
            }
            body.SetUseGravity(false);
            body.SetDetectCollisions(false);
            body.SetIsKinematic(true);

            auto disableColliders = [&](auto root)
            {
                for (Unity::Collider collider : root.template GetComponentsInChildren<Unity::Collider>(true))
                {
                    if (!collider)
                        continue;

                    bool alreadyTracked = false;
                    for (const NoclipColliderState& state : s_Noclip.Colliders)
                    {
                        if (state.Value == collider)
                        {
                            alreadyTracked = true;
                            break;
                        }
                    }

                    if (alreadyTracked)
                    {
                        if (collider.GetEnabled())
                            collider.SetEnabled(false);
                    }
                    else
                    {
                        bool wasEnabled = collider.GetEnabled();
                        s_Noclip.Colliders.PushBack({collider, wasEnabled});
                        if (wasEnabled)
                            collider.SetEnabled(false);
                    }
                }
            };
            disableColliders(__this);
            disableColliders(avatar);

            s_Noclip.WasFlying = true;
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"Noclip: %ls", message != null ? message.ToString().GetRawStringData() : L"Exception without message");
            StopNoclip(__this, PlayerAvatar::instance(), true);
            G->FlightEnabled = false;
        }
    }

    static void Hooked__PlayerController_Update(PlayerController __this)
    {
        int& jumps = __this.JumpExtra();
        int cachedJumps = jumps;
        if (G->InfJumps)
            jumps = 9999;

        G->PlayerController_Update_Hook.unsafe_call<void, PlayerController>(__this);

        jumps = cachedJumps;

        if (!G->FlightEnabled || !G->IsInGame)
        {
            if (s_Noclip.WasFlying || s_Noclip.PoseCaptured)
                StopNoclip(__this, PlayerAvatar::instance(), true);
            return;
        }

        try
        {
            PlayerAvatar avatar = PlayerAvatar::instance();
            Unity::Camera camera = SemiFunc::MainCamera();
            Unity::Transform playerTransform = __this.GetTransform();
            if (!avatar || avatar.deadSet() || avatar.isDisabled() || !camera || !playerTransform)
            {
                G->FlightEnabled = false;
                StopNoclip(__this, avatar, true);
                return;
            }

            if (!s_Noclip.PoseCaptured)
            {
                s_Noclip.OriginPosition = playerTransform.GetPosition();
                s_Noclip.OriginRotation = playerTransform.GetRotation();
                s_Noclip.Position = s_Noclip.OriginPosition;
                s_Noclip.PoseCaptured = true;
            }

            Unity::Vector3 direction = Unity::Vector3::zero();
            if (!G->MenuVisible)
            {
                Unity::Quaternion cameraRotation = camera.GetTransform().GetRotation();
                Unity::Vector3 forward = cameraRotation * Unity::Vector3::forward();
                Unity::Vector3 right = cameraRotation * Unity::Vector3::right();

                if (Hax::Gui::IsKeyDown('W')) direction = direction + forward;
                if (Hax::Gui::IsKeyDown('S')) direction = direction - forward;
                if (Hax::Gui::IsKeyDown('D')) direction = direction + right;
                if (Hax::Gui::IsKeyDown('A')) direction = direction - right;
                if (Hax::Gui::IsKeyDown(VK_SPACE)) direction = direction + Unity::Vector3::up();
                if (Hax::Gui::IsKeyDown(VK_CONTROL)) direction = direction + Unity::Vector3::down();
            }

            if (direction.GetMagnitude() > 0.001f)
            {
                float targetSpeed = (float)G->FlightSpeed;
                if (__this.sprinting())
                    targetSpeed *= (float)G->FlightSprintBoost;

                float dt = Unity::Time::GetDeltaTime();
                s_Noclip.CurrentSpeed = Hax::Lerp(s_Noclip.CurrentSpeed, targetSpeed, Hax::Clamp(dt * 8.f, 0.f, 1.f));
                s_Noclip.Position = s_Noclip.Position + direction.GetNormalized() * s_Noclip.CurrentSpeed * dt;
            }
            else
            {
                float dt = Unity::Time::GetDeltaTime();
                s_Noclip.CurrentSpeed = Hax::Lerp(s_Noclip.CurrentSpeed, 0.f, Hax::Clamp(dt * 12.f, 0.f, 1.f));
            }

            playerTransform.SetPosition(s_Noclip.Position);
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"Noclip position: %ls", message != null ? message.ToString().GetRawStringData() : L"Exception without message");
            G->FlightEnabled = false;
            StopNoclip(__this, PlayerAvatar::instance(), true);
        }
    }

    static void Hooked__PlayerTumble_TumbleRequest(PlayerTumble __this, bool isTumbling, bool playerInput)
    {
        PlayerAvatar player = __this.playerAvatar();

        if (G->FlightEnabled && player && player.isLocal())
        {
            if (!isTumbling)
                return;

            PlayerController controller = PlayerController::instance();
            G->FlightEnabled = false;
            StopNoclip(controller, player, true);
            G->PlayerTumble_TumbleRequest_Hook.unsafe_call<void, PlayerTumble, bool, bool>(
                __this, true, playerInput);
            return;
        }

        if (!s_ForcedPlayerTumble && !playerInput &&
            (G->GodAllPlayersNoTumble || (G->NoTumble && player.isLocal()) || IsPlayerInRuntimeSlots(player, G->NoTumblePlayers)))
            return;

        G->PlayerTumble_TumbleRequest_Hook.unsafe_call<void, PlayerTumble, bool, bool>(__this, isTumbling, playerInput);
    }

    static void Hooked__FlashlightController_Update(FlashlightController __this)
    {
        try
        {
            PlayerAvatar avatar = PlayerAvatar::instance();

            bool wasCrouching = false;
            if (G->Flashlight.InCrouch && avatar)
            {
                wasCrouching = avatar.isCrouching();
                avatar.isCrouching() = false;
            }

            G->FlashlightController_Update_Hook.unsafe_call<void, FlashlightController>(__this);

            if (G->Flashlight.InCrouch && avatar)
                avatar.isCrouching() = wasCrouching;

            Unity::Light spotLight = __this.spotlight();
            if ((float)G->Flashlight.Angle != spotLight.GetSpotAngle())
            {
                spotLight.SetSpotAngle((float)G->Flashlight.Angle);
            }

            spotLight.SetIntensity(spotLight.GetIntensity() * (G->Flashlight.Intensity / 10.f));
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");

            G->FlashlightController_Update_Hook.unsafe_call<void, FlashlightController>(__this);
        }
    }

    static UpgradeData* GetUpgradeInfoFromName(System::String name)
    {
        for (int i = 0; i < (int)UpgradeType::N; ++i)
        {
            if (name.ToHaxView().EndsWith(G->UpgradesData[i].InternalNameEnd))
                return &G->UpgradesData[i];
        }
        return nullptr;
    }

    static void Hooked__ItemUpgrade_Update(ItemUpgrade __this)
    {
        try
        {
            G->UpgradesPool.GetBack().PushBack(__this);

            if (G->AutoUseUpgrs)
            {
                UpgradeData* data = GetUpgradeInfoFromName(__this.itemAttributes().itemAssetName());
                if (data != nullptr && data->AutoUse)
                {
                    ItemToggle toggle = __this.itemToggle();
                    if (toggle)
                    {
                        toggle.photonView() = __this.photonView();
                        toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->ItemUpgrade_Update_Hook.unsafe_call<void, ItemUpgrade>(__this);
    }

    static void Hooked__ItemHealthPack_Update(ItemHealthPack __this)
    {
        if (!RunManager::instance().levelIsShop())
            G->AidsPool.GetBack().PushBack(__this);

        G->ItemHealthPack_Update_Hook.unsafe_call<void, ItemHealthPack>(__this);
    }

    static void Hooked__UpdateLoop()
    {
        G->UpgradesPool.GetBack().Clear();
        G->AidsPool.GetBack().Clear();

        G->UpdateHook.unsafe_call<void>();

        G->UpgradesPool.RefreshSpare();
        G->AidsPool.RefreshSpare();
    }

    static void SetThirdPerson()
    {
        static String playerTag = null;
        if (playerTag == null)
        {
            playerTag = System::String::New("Player");
            GCHandle::Alloc(playerTag, true);
        }

        PlayerAvatar avatar = PlayerAvatar::instance();

        float dist = 3.f;
        Transform normalTransformPivot = avatar.spectatePoint();
        static int layerMask = SemiFunc::LayerMaskGetVisionObstruct();
        auto hits = Unity::Physics::SphereCastAll(normalTransformPivot.GetPosition(), 0.1f, normalTransformPivot.GetForward() * -1.f, 3.f, layerMask);
        bool isMapOpen = avatar.mapToolController() && avatar.mapToolController().Active();
        if (hits != null && !isMapOpen)
        {
            for (RaycastHit& hit : hits)
            {
                Transform transform = hit.GetTransform();
                if (!transform.GetComponent<PlayerHealthGrab>() && !transform.GetGameObject().CompareTag(playerTag) && !transform.GetComponent<PlayerTumble>() && !transform.GetComponent<EnemyRigidbody>())
                    dist = Hax::Min(Hax::Max(1.f, hit.m_Distance), 3.f);
            }
        }
        else if (isMapOpen)
            dist = 1.f;

        SemiFunc::MainCamera().GetTransform().SetLocalPosition(Unity::Vector3(0.f, 0.f, -dist));
    }

    static bool ShouldDrawEsp()
    {
        return G->IsInGame && MenuManager::instance() && !MenuManager::instance().currentMenuPage();
    }

    static bool IsTransformBelow(Unity::Transform child, Unity::Transform root)
    {
        for (int depth = 0; child && depth < 24; ++depth)
        {
            if (child == root)
                return true;
            child = child.GetParent();
        }
        return false;
    }

    static bool IsTriggerCollider(Unity::Collider collider)
    {
        if (!collider)
            return false;

        static auto s_GetIsTrigger = Unity::Collider::typeof().GetMethod("get_isTrigger", nullptr, true).Wrap();
        return s_GetIsTrigger.Call<bool>(collider);
    }

    struct GameObjectRemovalTarget
    {
        Unity::GameObject Object = null;
        bool Networked = false;
    };

    static GameObjectRemovalTarget FindGameObjectRemovalTarget(Unity::Transform transform)
    {
        GameObjectRemovalTarget target{};
        if (!transform)
            return target;

        // For an ordinary scene collider, delete the exact GameObject that was hit.
        target.Object = transform.GetGameObject();

        for (int depth = 0; transform && depth < 20; ++depth)
        {
            // Never allow a parent hierarchy containing a player or enemy to become a target.
            if (transform.GetComponent<PlayerAvatar>() ||
                transform.GetComponent<PlayerTumble>() ||
                transform.GetComponent<EnemyRigidbody>())
            {
                return {};
            }

            // Interactive props often keep their actual object root on the impact detector.
            if (PhysGrabObjectImpactDetector detector = transform.GetComponent<PhysGrabObjectImpactDetector>())
                target.Object = detector.GetGameObject();

            // The closest Photon root identifies the complete network object. Stop here so a
            // held item is not rejected merely because it is temporarily parented to a player.
            if (Unity::Photon::PhotonView view = transform.GetComponent<Unity::Photon::PhotonView>())
            {
                target.Object = view.GetGameObject();
                target.Networked = true;
                return target;
            }

            transform = transform.GetParent();
        }

        return target;
    }

    static void ProcessObjectRemover(PlayerAvatar avatar)
    {
        G->ObjectRemoverTargetValid = false;
        G->ObjectRemoverTargetDistance = 0.f;
        G->GodAimedObject = null;
        G->GodAimedObjectValid = false;
        G->GodAimedObjectNetworked = false;
        G->GodAimedObjectDistance = 0.f;
        wcscpy_s(G->GodAimedObjectName, L"-");

        if ((!G->ObjectRemoverEnabled && !G->GodTargetingEnabled) || !G->IsInGame)
        {
            G->DeleteObjectRequested = false;
            return;
        }

        Unity::Camera camera = SemiFunc::MainCamera();
        if (!camera || !avatar)
        {
            G->DeleteObjectRequested = false;
            return;
        }

        Unity::Transform cameraTransform = camera.GetTransform();
        const Unity::Vector3 origin = cameraTransform.GetPosition();
        const Unity::Vector3 forward = cameraTransform.GetForward().GetNormalized();
        System::Array<Unity::RaycastHit> hits = Unity::Physics::SphereCastAll(
            origin, 0.035f, forward, (float)G->ObjectRemoverRange, -1);

        GameObjectRemovalTarget target{};
        float closestDistance = (float)G->ObjectRemoverRange;
        if (hits != null)
        {
            Unity::Transform avatarRoot = avatar.GetTransform();
            Unity::Transform controllerRoot = PlayerController::instance() ? PlayerController::instance().GetTransform() : null;

            for (Unity::RaycastHit& hit : hits)
            {
                if (hit.m_Distance < 0.05f || hit.m_Distance >= closestDistance)
                    continue;

                Unity::Transform hitTransform = hit.GetTransform();
                if (!hitTransform || IsTransformBelow(hitTransform, avatarRoot) ||
                    (controllerRoot && IsTransformBelow(hitTransform, controllerRoot)))
                {
                    continue;
                }

                Unity::Collider collider = hitTransform.GetComponent<Unity::Collider>();
                if (collider && IsTriggerCollider(collider))
                    continue;

                GameObjectRemovalTarget candidate = FindGameObjectRemovalTarget(hitTransform);
                if (!candidate.Object)
                    continue;

                target = candidate;
                closestDistance = hit.m_Distance;
            }
        }

        G->ObjectRemoverTargetValid = target.Object != null;
        G->ObjectRemoverTargetDistance = target.Object ? closestDistance : 0.f;
        G->GodAimedObject = target.Object;
        G->GodAimedObjectValid = target.Object != null;
        G->GodAimedObjectNetworked = target.Networked;
        G->GodAimedObjectDistance = target.Object ? closestDistance : 0.f;
        if (target.Object)
        {
            System::String name = target.Object.GetName();
            if (name != null && name.GetRawStringData())
                wcsncpy_s(G->GodAimedObjectName, name.GetRawStringData(), _TRUNCATE);
        }

        if (!G->DeleteObjectRequested)
            return;
        G->DeleteObjectRequested = false;

        if (G->IsClient)
        {
            SetSessionAction(G->Loc[LocKey_ActionObjectHostOnly]);
            return;
        }

        if (!target.Object)
        {
            SetSessionAction(G->Loc[LocKey_ActionObjectNoTarget]);
            return;
        }

        DeleteGodObject(target.Object, target.Networked);
        G->ObjectRemoverTargetValid = false;
        SetSessionAction(G->Loc[LocKey_ActionObjectDeleted]);
    }

    enum class GodUndoType { Transform, Reactivate, DestroyClone };

    struct GodUndoEntry
    {
        GodUndoType Type;
        Unity::GameObject Object;
        System::GCHandle Handle{nullptr};
        Unity::Vector3 Position;
        Unity::Quaternion Rotation;
        Unity::Vector3 Scale;
    };

    struct BodyState
    {
        Unity::Rigidbody Body;
        bool Kinematic;
        bool UseGravity;
    };

    struct LightState
    {
        Unity::Light Light;
        bool Enabled;
        float Intensity;
    };

    static Hax::Vector<GodUndoEntry> s_GodUndo;
    static Hax::Vector<BodyState> s_WorldFrozenBodies;
    static Hax::Vector<BodyState> s_EnemyFrozenBodies;
    static Hax::Vector<BodyState> s_LootFrozenBodies;
    static Hax::Vector<LightState> s_WorldLights;
    static Unity::GameObject s_HeldObject;
    static Unity::Rigidbody s_HeldBody;
    static System::GCHandle s_GodTargetHandle{nullptr};
    static bool s_HeldBodyKinematic;
    static bool s_HeldBodyGravity;
    static bool s_TimeScaleWasOverridden;
    static bool s_GravityWasOverridden;
    static Unity::Vector3 s_OriginalGravity = Unity::Vector3(0.f, -9.81f, 0.f);
    static bool s_GizmoDragActive;
    static GodGizmoMode s_GizmoDragMode;
    static GodGizmoSpace s_GizmoDragSpace;
    static int s_GizmoDragAxis = -1;
    static float s_GizmoDragAmount;
    static Unity::Vector3 s_GizmoStartPosition;
    static Unity::Quaternion s_GizmoStartRotation;
    static Unity::Vector3 s_GizmoStartScale;
    static Unity::Vector3 s_GizmoDragWorldAxis;
    static Unity::Rigidbody s_GizmoDragBody;
    static bool s_GizmoDragBodyKinematic;
    static bool s_GizmoDragBodyGravity;

    static void RefreshGodUndoCount()
    {
        G->GodUndoCount = (int)s_GodUndo.Size();
    }

    static void PushGodUndo(GodUndoType type, Unity::GameObject object)
    {
        if (!object)
            return;

        if (s_GodUndo.Size() >= 24)
        {
            s_GodUndo[0].Handle.Free();
            s_GodUndo.Erase((size_t)0);
        }

        Unity::Transform transform = object.GetTransform();
        GodUndoEntry entry{};
        entry.Type = type;
        entry.Object = object;
        entry.Handle = System::GCHandle::Alloc(object, false);
        if (transform)
        {
            entry.Position = transform.GetPosition();
            entry.Rotation = transform.GetRotation();
            entry.Scale = transform.GetLocalScale();
        }
        s_GodUndo.PushBack(entry);
        RefreshGodUndoCount();
    }

    static void UndoLastGodAction()
    {
        while (!s_GodUndo.Empty())
        {
            GodUndoEntry entry = s_GodUndo.Last();
            s_GodUndo.PopBack();
            RefreshGodUndoCount();
            if (!entry.Object)
            {
                entry.Handle.Free();
                continue;
            }

            if (entry.Type == GodUndoType::DestroyClone)
            {
                Unity::Object::Destroy(entry.Object);
                entry.Handle.Free();
                return;
            }

            if (entry.Type == GodUndoType::Reactivate)
                entry.Object.SetActive(true);

            Unity::Transform transform = entry.Object.GetTransform();
            if (transform)
            {
                transform.SetPosition(entry.Position);
                transform.SetRotation(entry.Rotation);
                transform.SetLocalScale(entry.Scale);
            }
            entry.Handle.Free();
            return;
        }
    }

    static void DeleteGodObject(Unity::GameObject object, bool networked)
    {
        if (!object)
            return;

        if (networked && SemiFunc::IsMultiplayer())
        {
            Unity::Photon::PhotonNetwork::Destroy(object);
            return;
        }

        // Scene objects are soft-deleted so the operation can be undone safely.
        PushGodUndo(GodUndoType::Reactivate, object);
        object.SetActive(false);
    }

    static void SetGodTarget(Unity::GameObject object, bool networked)
    {
        s_GodTargetHandle.Free();
        G->GodTargetObject = object;
        G->GodTargetObjectValid = object != null;
        G->GodTargetNetworked = object && networked;
        wcscpy_s(G->GodTargetName, L"-");
        G->GodTargetPosition = Unity::Vector3::zero();
        G->GodTargetLayer = 0;
        if (!object)
        {
            G->GodGizmoEditMode = false;
            G->GodGizmoProjectionValid = false;
            return;
        }

        s_GodTargetHandle = System::GCHandle::Alloc(object, false);
        if (networked && !G->IsClient)
        {
            // REPO serializes network-object position/rotation from the owning PhotonView.
            // Ask for ownership before editing so another client cannot immediately
            // overwrite the host's gizmo movement.
            if (Unity::Photon::PhotonView view = object.GetComponent<Unity::Photon::PhotonView>();
                view && !view.IsMine())
            {
                view.RequestOwnership();
            }
        }
        System::String name = object.GetName();
        if (name != null && name.GetRawStringData())
            wcsncpy_s(G->GodTargetName, name.GetRawStringData(), _TRUNCATE);

        Unity::Transform transform = object.GetTransform();
        if (transform)
        {
            G->GodTargetPosition = transform.GetPosition();
            Unity::Vector3 scale = transform.GetLocalScale();
            G->GodScaleXPercent = (int)Hax::Round(scale.X * 100.f);
            G->GodScaleYPercent = (int)Hax::Round(scale.Y * 100.f);
            G->GodScaleZPercent = (int)Hax::Round(scale.Z * 100.f);
            G->GodScalePercent = (G->GodScaleXPercent + G->GodScaleYPercent + G->GodScaleZPercent) / 3;
        }
        G->GodTargetLayer = object.GetLayer();
    }

    static void RefreshGodTargetSnapshot(Unity::GameObject object)
    {
        if (!object)
        {
            SetGodTarget(null, false);
            return;
        }

        Unity::Transform transform = object.GetTransform();
        G->GodTargetObjectValid = transform != null;
        if (transform)
            G->GodTargetPosition = transform.GetPosition();
        G->GodTargetLayer = object.GetLayer();
    }

    static void DiscardGodSceneReferences()
    {
        for (GodUndoEntry& entry : s_GodUndo)
            entry.Handle.Free();
        s_GodUndo.Clear();
        RefreshGodUndoCount();

        s_WorldFrozenBodies.Clear();
        s_EnemyFrozenBodies.Clear();
        s_LootFrozenBodies.Clear();
        s_WorldLights.Clear();
        s_HeldBody = null;
        s_HeldObject = null;
        G->GodTelekinesisActive = false;

        s_GodTargetHandle.Free();
        G->GodTargetObject = null;
        G->GodTargetObjectValid = false;
        G->GodTargetNetworked = false;
        G->GodTargetPosition = Unity::Vector3::zero();
        G->GodTargetLayer = 0;
        wcscpy_s(G->GodTargetName, L"-");

        G->GodAimedObject = null;
        G->GodAimedObjectValid = false;
        G->GodAimedObjectNetworked = false;
        G->GodAimedObjectDistance = 0.f;
        wcscpy_s(G->GodAimedObjectName, L"-");

        s_GizmoDragActive = false;
        s_GizmoDragAxis = -1;
        s_GizmoDragBody = null;
        G->GodGizmoEditMode = false;
        G->GodGizmoProjectionValid = false;
        G->GodGizmoHoveredAxis = -1;
        G->GodGizmoActiveAxis = -1;
        G->GodGizmoDragPixelsPending = 0.f;
        G->GodGizmoDragBeginRequested = false;
        G->GodGizmoDragEndRequested = false;
    }

    static bool ProjectGizmoPoint(Unity::Camera camera, const Unity::Vector3& world, Hax::Vector2& out)
    {
        if (!camera || G->PixelWidth <= 0.f || G->PixelHeight <= 0.f ||
            G->ScreenWidth <= 0.f || G->ScreenHeight <= 0.f)
        {
            return false;
        }

        Unity::Vector3 screen = camera.WorldToScreenPoint(world);
        if (screen.Z <= 0.f)
            return false;

        out.X = screen.X * (G->ScreenWidth / G->PixelWidth);
        out.Y = G->ScreenHeight - screen.Y * (G->ScreenHeight / G->PixelHeight);
        return out.X >= -64.f && out.Y >= -64.f &&
            out.X <= G->ScreenWidth + 64.f && out.Y <= G->ScreenHeight + 64.f;
    }

    static Unity::Vector3 GetGizmoAxis(int axis, const Unity::Quaternion& rotation, GodGizmoSpace space)
    {
        Unity::Vector3 direction = axis == 0 ? Unity::Vector3::right() :
            (axis == 1 ? Unity::Vector3::up() : Unity::Vector3::forward());
        return space == GodGizmoSpace::Local ? rotation * direction : direction;
    }

    static void EndGodGizmoDrag()
    {
        if (s_GizmoDragBody)
        {
            s_GizmoDragBody.SetIsKinematic(s_GizmoDragBodyKinematic);
            s_GizmoDragBody.SetUseGravity(s_GizmoDragBodyGravity);
        }
        s_GizmoDragBody = null;
        s_GizmoDragActive = false;
        s_GizmoDragAxis = -1;
        G->GodGizmoActiveAxis = -1;
    }

    static void SyncGodTargetPose(Unity::GameObject target, Unity::Transform transform)
    {
        if (!target || !transform || G->IsClient || !G->GodTargetNetworked)
            return;

        // Use REPO's own RPC/PhotonTransformView path for the final pose. Directly
        // assigning Transform only changes the current machine for non-owned objects.
        if (PhysGrabObject physGrab = target.GetComponent<PhysGrabObject>())
            physGrab.Teleport(transform.GetPosition(), transform.GetRotation());
    }

    static void ProcessGodGizmoDrag(Unity::GameObject target, Unity::Transform transform)
    {
        if (!target || !transform || !G->GodGizmoVisible)
        {
            if (s_GizmoDragActive)
                EndGodGizmoDrag();
            G->GodGizmoDragPixelsPending = 0.f;
            G->GodGizmoDragBeginRequested = false;
            G->GodGizmoDragEndRequested = false;
            return;
        }

        if (G->GodGizmoDragBeginRequested)
        {
            G->GodGizmoDragBeginRequested = false;
            const int axis = G->GodGizmoActiveAxis;
            if (axis >= 0 && axis < 3)
            {
                PushGodUndo(GodUndoType::Transform, target);
                s_GizmoDragActive = true;
                s_GizmoDragAxis = axis;
                s_GizmoDragMode = G->GodGizmoModeCurrent;
                s_GizmoDragSpace = G->GodGizmoSpaceCurrent;
                s_GizmoDragAmount = 0.f;
                s_GizmoStartPosition = transform.GetPosition();
                s_GizmoStartRotation = transform.GetRotation();
                s_GizmoStartScale = transform.GetLocalScale();
                s_GizmoDragWorldAxis = GetGizmoAxis(axis, s_GizmoStartRotation, s_GizmoDragSpace);
                s_GizmoDragBody = target.GetComponent<Unity::Rigidbody>();
                if (s_GizmoDragBody)
                {
                    s_GizmoDragBodyKinematic = s_GizmoDragBody.GetIsKinematic();
                    s_GizmoDragBodyGravity = s_GizmoDragBody.GetUseGravity();
                    s_GizmoDragBody.SetVelocity(Unity::Vector3::zero());
                    s_GizmoDragBody.SetUseGravity(false);
                    s_GizmoDragBody.SetIsKinematic(true);
                }
            }
        }

        const float pixels = G->GodGizmoDragPixelsPending;
        G->GodGizmoDragPixelsPending = 0.f;
        if (s_GizmoDragActive && pixels != 0.f)
        {
            Hax::Vector2 screenAxis = G->GodGizmoScreenAxes[s_GizmoDragAxis] - G->GodGizmoScreenOrigin;
            const float screenLength = Hax::Max(screenAxis.Length(), 40.f);

            if (s_GizmoDragMode == GodGizmoMode::Move)
            {
                s_GizmoDragAmount += pixels * G->GodGizmoWorldSize / screenLength;
                transform.SetPosition(s_GizmoStartPosition + s_GizmoDragWorldAxis * s_GizmoDragAmount);
            }
            else if (s_GizmoDragMode == GodGizmoMode::Scale)
            {
                const float startAxis = s_GizmoDragAxis == 0 ? s_GizmoStartScale.X :
                    (s_GizmoDragAxis == 1 ? s_GizmoStartScale.Y : s_GizmoStartScale.Z);
                s_GizmoDragAmount += pixels * Hax::Max(Hax::Abs(startAxis), 0.25f) / screenLength;
                Unity::Vector3 scale = s_GizmoStartScale;
                float value = Hax::Clamp(startAxis + s_GizmoDragAmount, 0.01f, 50.f);
                if (s_GizmoDragAxis == 0) scale.X = value;
                else if (s_GizmoDragAxis == 1) scale.Y = value;
                else scale.Z = value;
                transform.SetLocalScale(scale);
                G->GodScaleXPercent = (int)Hax::Round(scale.X * 100.f);
                G->GodScaleYPercent = (int)Hax::Round(scale.Y * 100.f);
                G->GodScaleZPercent = (int)Hax::Round(scale.Z * 100.f);
            }
            else
            {
                s_GizmoDragAmount += pixels * 0.8f;
                Unity::Quaternion delta = Unity::Quaternion::AngleAxis(s_GizmoDragAmount,
                    s_GizmoDragSpace == GodGizmoSpace::Local
                        ? (s_GizmoDragAxis == 0 ? Unity::Vector3::right() :
                            (s_GizmoDragAxis == 1 ? Unity::Vector3::up() : Unity::Vector3::forward()))
                        : s_GizmoDragWorldAxis);
                transform.SetRotation(s_GizmoDragSpace == GodGizmoSpace::Local
                    ? s_GizmoStartRotation * delta
                    : delta * s_GizmoStartRotation);
            }

        }

        if (G->GodGizmoDragEndRequested)
        {
            G->GodGizmoDragEndRequested = false;
            SyncGodTargetPose(target, transform);
            EndGodGizmoDrag();
        }
    }

    static void UpdateGodGizmoProjection(Unity::GameObject target, Unity::Transform transform)
    {
        G->GodGizmoProjectionValid = false;
        if (!G->GodGizmoVisible || !target || !transform)
            return;

        Unity::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        const Unity::Vector3 origin = transform.GetPosition();
        const Unity::Quaternion rotation = transform.GetRotation();
        const float distance = camera.GetTransform().GetPosition().Distance(origin);
        const float worldSize = Hax::Clamp(distance * 0.12f, 0.6f, 7.f);
        G->GodGizmoWorldSize = worldSize;

        if (!ProjectGizmoPoint(camera, origin, G->GodGizmoScreenOrigin))
            return;

        for (int axis = 0; axis < 3; ++axis)
        {
            const Unity::Vector3 direction = GetGizmoAxis(axis, rotation,
                G->GodGizmoModeCurrent == GodGizmoMode::Scale ? GodGizmoSpace::Local : G->GodGizmoSpaceCurrent);
            G->GodGizmoScreenAxisValid[axis] = ProjectGizmoPoint(camera,
                origin + direction * worldSize, G->GodGizmoScreenAxes[axis]);

            Unity::Vector3 basisA = axis == 0 ? Unity::Vector3::up() : Unity::Vector3::right();
            Unity::Vector3 basisB = axis == 2 ? Unity::Vector3::up() : Unity::Vector3::forward();
            if (axis == 1)
                basisB = Unity::Vector3::forward();
            const GodGizmoSpace ringSpace = G->GodGizmoSpaceCurrent;
            if (ringSpace == GodGizmoSpace::Local)
            {
                basisA = rotation * basisA;
                basisB = rotation * basisB;
            }

            for (int point = 0; point <= GodGizmoRingSegments; ++point)
            {
                const float angle = (float)point / (float)GodGizmoRingSegments * Hax::kPi * 2.f;
                const Unity::Vector3 ringWorld = origin +
                    (basisA * Hax::Cos(angle) + basisB * Hax::Sin(angle)) * (worldSize * 0.82f);
                G->GodGizmoRingPointValid[axis][point] = ProjectGizmoPoint(camera,
                    ringWorld, G->GodGizmoRingPoints[axis][point]);
            }
        }
        G->GodGizmoProjectionValid = true;
    }

    static void ReleaseGodTelekinesis()
    {
        if (s_HeldBody)
        {
            s_HeldBody.SetIsKinematic(s_HeldBodyKinematic);
            s_HeldBody.SetUseGravity(s_HeldBodyGravity);
        }
        s_HeldBody = null;
        s_HeldObject = null;
        G->GodTelekinesisActive = false;
    }

    static bool IsPlayerHierarchy(Unity::Transform transform)
    {
        GameDirector director = GameDirector::instance();
        System::List<PlayerAvatar> players{};
        if (director)
            players = director.PlayerList();
        if (players == null)
            return false;

        for (PlayerAvatar player : players)
            if (player && IsTransformBelow(transform, player.GetTransform()))
                return true;
        return false;
    }

    static void FreezeBodies(Hax::Vector<BodyState>& states, System::Array<Unity::Rigidbody> bodies, bool skipPlayers)
    {
        states.Clear();
        if (bodies == null)
            return;

        for (Unity::Rigidbody body : bodies)
        {
            if (!body || (skipPlayers && IsPlayerHierarchy(body.GetTransform())))
                continue;

            states.PushBack({body, body.GetIsKinematic(), body.GetUseGravity()});
            body.SetVelocity(Unity::Vector3::zero());
            body.SetUseGravity(false);
            body.SetIsKinematic(true);
        }
    }

    static void RestoreBodies(Hax::Vector<BodyState>& states)
    {
        for (BodyState& state : states)
        {
            if (!state.Body)
                continue;
            state.Body.SetIsKinematic(state.Kinematic);
            state.Body.SetUseGravity(state.UseGravity);
        }
        states.Clear();
    }

    static void ApplyRemotePlayerGravity()
    {
        static uint64_t s_LastUpdate;

        if (!G->IsInGame || G->IsClient || !G->WorldGravityOverride || !SemiFunc::IsMultiplayer())
        {
            s_LastUpdate = 0;
            return;
        }

        const uint64_t now = ::GetTickCount64();
        if (!s_LastUpdate)
        {
            s_LastUpdate = now;
            return;
        }

        const uint64_t elapsedMs = now - s_LastUpdate;
        if (elapsedMs < 75)
            return;

        s_LastUpdate = now;
        const float elapsed = Hax::Clamp((float)elapsedMs / 1000.f, 0.f, 0.25f);
        const float originalGravityY = s_GravityWasOverridden ? s_OriginalGravity.Y : Unity::Physics::GetGravity().Y;
        const Unity::Vector3 correction(0.f, ((float)G->WorldGravityY - originalGravityY) * elapsed, 0.f);

        GameDirector director = GameDirector::instance();
        if (!director || director.PlayerList() == null)
            return;

        for (PlayerAvatar player : director.PlayerList())
        {
            if (!IsPlayerUsable(player) || player.isLocal())
                continue;

            player.ForceImpulse(correction);
            if (player.isTumbling() && player.tumble())
                player.tumble().TumbleForce(correction);
        }
    }

    static void ProcessWorldGodTools()
    {
        if (G->IsInGame && G->WorldTimeScalePercent != 100)
        {
            Unity::Time::SetTimeScale(G->WorldTimeScalePercent / 100.f);
            s_TimeScaleWasOverridden = true;
        }
        else if (s_TimeScaleWasOverridden)
        {
            Unity::Time::SetTimeScale(1.f);
            s_TimeScaleWasOverridden = false;
        }

        if (G->WorldGravityOverride)
        {
            if (!s_GravityWasOverridden)
            {
                s_OriginalGravity = Unity::Physics::GetGravity();
                s_GravityWasOverridden = true;
            }
            Unity::Physics::SetGravity(Unity::Vector3(0.f, (float)G->WorldGravityY, 0.f));
        }
        else if (s_GravityWasOverridden)
        {
            Unity::Physics::SetGravity(s_OriginalGravity);
            s_GravityWasOverridden = false;
        }

        ApplyRemotePlayerGravity();

        // Scene objects are already being destroyed once IsInGame turns false.
        // Never inspect or restore cached Components during that unload window.
        if (!G->IsInGame)
        {
            G->WorldFreezePhysicsChanged = false;
            G->WorldLightAction = WorldLightCommand::None;
            return;
        }

        if (G->WorldFreezePhysicsChanged)
        {
            G->WorldFreezePhysicsChanged = false;
            if (G->WorldFreezePhysics)
                FreezeBodies(s_WorldFrozenBodies, Unity::Object::FindObjectsOfType<Unity::Rigidbody>(), true);
            else
                RestoreBodies(s_WorldFrozenBodies);
        }

        WorldLightCommand lightAction = G->WorldLightAction;
        G->WorldLightAction = WorldLightCommand::None;
        if (lightAction != WorldLightCommand::None)
        {
            if (lightAction == WorldLightCommand::Restore)
            {
                for (LightState& state : s_WorldLights)
                {
                    if (state.Light)
                    {
                        state.Light.SetEnabled(state.Enabled);
                        state.Light.SetIntensity(state.Intensity);
                    }
                }
                s_WorldLights.Clear();
            }
            else
            {
                System::Array<Unity::Light> lights = Unity::Object::FindObjectsOfType<Unity::Light>();
                if (s_WorldLights.Empty() && lights != null)
                    for (Unity::Light light : lights)
                        if (light)
                            s_WorldLights.PushBack({light, light.GetEnabled(), light.GetIntensity()});

                if (lights != null)
                {
                    for (Unity::Light light : lights)
                    {
                        if (!light)
                            continue;
                        if (lightAction == WorldLightCommand::Blackout)
                            light.SetEnabled(false);
                        else
                        {
                            light.SetEnabled(true);
                            light.SetIntensity(Hax::Max(light.GetIntensity(), 8.f));
                        }
                    }
                }
            }
        }
    }

    static void ProcessEnemyGodTools()
    {
        EnemyDirector director = EnemyDirector::instance();
        if (!director)
            return;

        if (G->GodEnemiesPacifiedChanged)
        {
            G->GodEnemiesPacifiedChanged = false;
            director.debugNoVision() = G->GodEnemiesPacified;
        }
        System::List<EnemyParent> enemies = director.enemiesSpawned();
        if (enemies == null)
            return;

        if (G->GodEnemiesFreezeChanged)
        {
            G->GodEnemiesFreezeChanged = false;
            if (!G->GodEnemiesFrozen)
                RestoreBodies(s_EnemyFrozenBodies);
            else
            {
                s_EnemyFrozenBodies.Clear();
                for (EnemyParent parent : enemies)
                {
                    Enemy enemy{};
                    if (parent)
                        enemy = parent.enemy();
                    EnemyRigidbody enemyBody = enemy ? enemy.Rigidbody() : null;
                    Unity::Rigidbody body = enemyBody ? enemyBody.GetComponent<Unity::Rigidbody>() : null;
                    if (body)
                    {
                        s_EnemyFrozenBodies.PushBack({body, body.GetIsKinematic(), body.GetUseGravity()});
                        body.SetVelocity(Unity::Vector3::zero());
                        body.SetUseGravity(false);
                        body.SetIsKinematic(true);
                    }
                }
            }
        }

        EnemyGodCommand action = G->EnemyGodAction;
        G->EnemyGodAction = EnemyGodCommand::None;
        if (action == EnemyGodCommand::None)
            return;

        Unity::Camera camera = SemiFunc::MainCamera();
        Unity::Vector3 gatherBase{};
        Unity::Vector3 right = Unity::Vector3::right();
        if (camera)
        {
            Unity::Transform ct = camera.GetTransform();
            gatherBase = ct.GetPosition() + ct.GetForward() * 6.f;
            right = ct.GetRotation() * Unity::Vector3::right();
        }

        int index = 0;
        for (EnemyParent parent : enemies)
        {
            Enemy enemy{};
            if (parent)
                enemy = parent.enemy();
            if (!enemy)
                continue;

            if (action == EnemyGodCommand::KillAll)
            {
                if (EnemyHealth health = enemy.Health(); health && !health.dead())
                    health.Hurt(999999, Unity::Vector3::zero());
            }
            else if (action == EnemyGodCommand::Gather && camera)
            {
                enemy.GetTransform().SetPosition(gatherBase + right * ((float)(index++ % 7) - 3.f) * 1.2f);
            }
            else if (action == EnemyGodCommand::DeleteAll && !G->IsClient)
            {
                Unity::GameObject object = parent.GetGameObject();
                Unity::Photon::PhotonView view = object ? object.GetComponent<Unity::Photon::PhotonView>() : null;
                if (view && SemiFunc::IsMultiplayer())
                    Unity::Photon::PhotonNetwork::Destroy(object);
                else if (object)
                    Unity::Object::Destroy(object);
            }
        }
    }

    struct ExtractionPackingJob
    {
        Hax::Vector<ValuableObject> Items;
        Hax::Vector<BodyState> Bodies;
        Unity::Bounds Area{};
        size_t Next{};
        int Columns{1};
        int Rows{1};
        int Layers{1};
        size_t Moved{};
        int Failures{};
        bool Active{};
    };

    static ExtractionPackingJob s_ExtractionPacking;

    static void ResetExtractionPacking(bool restoreBodies)
    {
        if (restoreBodies)
        {
            for (BodyState& state : s_ExtractionPacking.Bodies)
            {
                try
                {
                    if (!state.Body)
                        continue;
                    state.Body.SetIsKinematic(state.Kinematic);
                    state.Body.SetUseGravity(state.UseGravity);
                }
                catch (System::Exception&)
                {
                    // Extraction may destroy a valuable before the delayed
                    // restore pass reaches its cached rigidbody.
                }
            }
        }

        s_ExtractionPacking.Items.Clear();
        s_ExtractionPacking.Bodies.Clear();
        s_ExtractionPacking = {};
    }

    static bool StartExtractionPacking(System::List<ValuableObject> valuables)
    {
        ResetExtractionPacking(true);
        if (!GetExtractionPackingBounds(s_ExtractionPacking.Area))
            return false;

        for (ValuableObject valuable : valuables)
        {
            try
            {
                if (valuable && valuable.GetEnabled())
                    s_ExtractionPacking.Items.PushBack(valuable);
            }
            catch (System::Exception&)
            {
                ++s_ExtractionPacking.Failures;
            }
        }

        const int count = (int)s_ExtractionPacking.Items.Size();
        if (count == 0)
            return false;

        const Unity::Vector3 size = s_ExtractionPacking.Area.GetSize();
        const float width = Hax::Max(0.25f, size.X - 0.16f);
        const float height = Hax::Max(0.25f, size.Y - 0.16f);
        const float depth = Hax::Max(0.25f, size.Z - 0.16f);
        const float density = cbrtf((float)count / Hax::Max(0.015625f, width * height * depth));

        s_ExtractionPacking.Columns = Hax::Max(1, (int)ceilf(width * density));
        s_ExtractionPacking.Rows = Hax::Max(1, (int)ceilf(depth * density));
        const int perLayer = Hax::Max(1, s_ExtractionPacking.Columns * s_ExtractionPacking.Rows);
        s_ExtractionPacking.Layers = Hax::Max(1, (count + perLayer - 1) / perLayer);
        s_ExtractionPacking.Active = true;

        Hax::Log(G->Logger, L"Extraction packing: %d valuables into %dx%dx%d", count,
            s_ExtractionPacking.Columns, s_ExtractionPacking.Layers, s_ExtractionPacking.Rows);
        return true;
    }

    static void ProcessExtractionPacking()
    {
        if (!s_ExtractionPacking.Active)
            return;

        constexpr size_t BatchSize = 32;
        const Unity::Vector3 areaMin = s_ExtractionPacking.Area.GetMin();
        const Unity::Vector3 size = s_ExtractionPacking.Area.GetSize();
        const float padding = 0.12f;
        const float usableWidth = Hax::Max(0.08f, size.X - padding * 2.f);
        const float usableHeight = Hax::Max(0.08f, size.Y - padding * 2.f);
        const float usableDepth = Hax::Max(0.08f, size.Z - padding * 2.f);
        const int perLayer = Hax::Max(1, s_ExtractionPacking.Columns * s_ExtractionPacking.Rows);
        const size_t end = Hax::Min(s_ExtractionPacking.Items.Size(), s_ExtractionPacking.Next + BatchSize);

        while (s_ExtractionPacking.Next < end)
        {
            const size_t index = s_ExtractionPacking.Next++;
            ValuableObject valuable = s_ExtractionPacking.Items[index];
            try
            {
                if (!valuable || !valuable.GetEnabled())
                    continue;

                Unity::Transform transform = valuable.GetTransform();
                PhysGrabObject phys = valuable.physGrabObject();
                if (!transform)
                    continue;

                const int layer = (int)index / perLayer;
                const int slot = (int)index % perLayer;
                const int row = slot / s_ExtractionPacking.Columns;
                const int column = slot % s_ExtractionPacking.Columns;

                // Put each collider centre at a unique point strictly inside the
                // extraction trigger. The grid becomes denser as loot count grows
                // instead of collapsing excess layers onto one coordinate.
                Unity::Vector3 colliderCenter(
                    areaMin.X + padding + ((float)column + 0.5f) * usableWidth / (float)s_ExtractionPacking.Columns,
                    areaMin.Y + padding + ((float)layer + 0.5f) * usableHeight / (float)s_ExtractionPacking.Layers,
                    areaMin.Z + padding + ((float)row + 0.5f) * usableDepth / (float)s_ExtractionPacking.Rows);

                Unity::Vector3 colliderOffset{};
                Unity::Vector3 colliderExtents{};
                bool hasColliderBounds = false;
                Unity::Bounds colliderBounds{};
                if (phys)
                {
                    for (Unity::Collider collider : phys.GetComponentsInChildren<Unity::Collider>(true))
                    {
                        if (!collider || !collider.GetEnabled())
                            continue;

                        Unity::Bounds bounds = collider.GetBounds();
                        if (!hasColliderBounds)
                        {
                            colliderBounds = bounds;
                            hasColliderBounds = true;
                        }
                        else
                            colliderBounds.Encapsulate(bounds);
                    }
                }

                if (hasColliderBounds)
                {
                    colliderOffset = colliderBounds.Center - transform.GetPosition();
                    colliderExtents = colliderBounds.Extents;

                    const Unity::Vector3 areaMax = s_ExtractionPacking.Area.GetMax();
                    const float minX = areaMin.X + padding + colliderExtents.X;
                    const float maxX = areaMax.X - padding - colliderExtents.X;
                    const float minY = areaMin.Y + padding + colliderExtents.Y;
                    const float maxY = areaMax.Y - padding - colliderExtents.Y;
                    const float minZ = areaMin.Z + padding + colliderExtents.Z;
                    const float maxZ = areaMax.Z - padding - colliderExtents.Z;

                    colliderCenter.X = minX <= maxX ? Hax::Clamp(colliderCenter.X, minX, maxX) : s_ExtractionPacking.Area.Center.X;
                    colliderCenter.Y = minY <= maxY ? Hax::Clamp(colliderCenter.Y, minY, maxY) : s_ExtractionPacking.Area.Center.Y;
                    colliderCenter.Z = minZ <= maxZ ? Hax::Clamp(colliderCenter.Z, minZ, maxZ) : s_ExtractionPacking.Area.Center.Z;
                }
                const Unity::Vector3 position = colliderCenter - colliderOffset;

                Unity::Rigidbody body = phys ? phys.GetComponent<Unity::Rigidbody>() : null;
                if (body)
                {
                    s_ExtractionPacking.Bodies.PushBack({body, body.GetIsKinematic(), body.GetUseGravity()});
                    body.SetVelocity(Unity::Vector3::zero());
                    body.SetUseGravity(false);
                    body.SetIsKinematic(true);
                }

                if (phys)
                    phys.Teleport(position, transform.GetRotation());
                else
                    transform.SetPosition(position);
                ++s_ExtractionPacking.Moved;
            }
            catch (System::Exception&)
            {
                ++s_ExtractionPacking.Failures;
            }
        }

        if (s_ExtractionPacking.Next >= s_ExtractionPacking.Items.Size())
        {
            s_ExtractionPacking.Active = false;
            Hax::Log(G->Logger, L"Extraction packing complete: %zu moved, %d skipped",
                s_ExtractionPacking.Moved, s_ExtractionPacking.Failures);
        }
    }

    static void ProcessLootGodTools(PlayerAvatar avatar)
    {
        ValuableDirector director = ValuableDirector::instance();
        if (!director || director.valuableList() == null)
            return;

        System::List<ValuableObject> valuables = director.valuableList();
        if (G->GodLootFreezeChanged)
        {
            G->GodLootFreezeChanged = false;
            if (!G->GodLootFrozen)
                RestoreBodies(s_LootFrozenBodies);
            else
            {
                s_LootFrozenBodies.Clear();
                for (ValuableObject valuable : valuables)
                {
                    PhysGrabObject phys = valuable ? valuable.physGrabObject() : null;
                    Unity::Rigidbody body = phys ? phys.GetComponent<Unity::Rigidbody>() : null;
                    if (body)
                    {
                        s_LootFrozenBodies.PushBack({body, body.GetIsKinematic(), body.GetUseGravity()});
                        body.SetVelocity(Unity::Vector3::zero());
                        body.SetUseGravity(false);
                        body.SetIsKinematic(true);
                    }
                }
            }
        }

        LootGodCommand action = G->LootGodAction;
        G->LootGodAction = LootGodCommand::None;

        if (action == LootGodCommand::BringToExtraction)
        {
            StartExtractionPacking(valuables);
            ProcessExtractionPacking();
            return;
        }

        if (s_ExtractionPacking.Active && action != LootGodCommand::None)
            ResetExtractionPacking(true);
        ProcessExtractionPacking();
        if (action == LootGodCommand::None)
            return;

        Unity::Vector3 target{};
        bool hasTarget = false;
        if (action == LootGodCommand::BringToPlayer && avatar)
        {
            target = avatar.GetTransform().GetPosition() + avatar.GetTransform().GetForward() * 2.f;
            hasTarget = true;
        }
        else if (action == LootGodCommand::BringToTruck)
            hasTarget = GetTruckPosition(target);

        int index = 0;
        for (ValuableObject valuable : valuables)
        {
            if (!valuable)
                continue;

            if (hasTarget)
            {
                const float x = ((float)(index % 6) - 2.5f) * 0.65f;
                const float z = (float)(index / 6) * 0.65f;
                valuable.GetTransform().SetPosition(target + Unity::Vector3(x, 0.5f, z));
                ++index;
            }
            else if (action == LootGodCommand::DiscoverAll)
                valuable.Discover(1);
            else if (action == LootGodCommand::ApplyValueMultiplier)
                valuable.dollarValueCurrent() = Hax::Clamp(valuable.dollarValueCurrent() * G->GodLootValuePercent / 100.f, 0.f, 10000000.f);
        }
    }

    static void ProcessPlayerGodTools(PlayerAvatar localPlayer)
    {
        PlayerGodCommand action = G->PlayerGodAction;
        G->PlayerGodAction = PlayerGodCommand::None;
        if (action == PlayerGodCommand::None || G->IsClient)
            return;

        GameDirector director = GameDirector::instance();
        System::List<PlayerAvatar> players{};
        if (director)
            players = director.PlayerList();
        if (players == null)
            return;

        Unity::Vector3 gatherPos = localPlayer ? localPlayer.GetTransform().GetPosition() : Unity::Vector3::zero();
        int index = 0;
        for (PlayerAvatar player : players)
        {
            if (!player)
                continue;

            if (action == PlayerGodCommand::HealAll && !player.deadSet())
            {
                if (PlayerHealth health = player.playerHealth())
                    health.HealOther(99999, true);
            }
            else if (action == PlayerGodCommand::ReviveAll && player.deadSet())
                player.Revive(false);
            else if (action == PlayerGodCommand::GatherAll && localPlayer && player != localPlayer)
                player.GetTransform().SetPosition(gatherPos + Unity::Vector3((float)(++index) * 0.8f, 0.25f, 1.5f));
            else if (action == PlayerGodCommand::ApplyScaleAll)
            {
                float scale = G->GodAllPlayerScalePercent / 100.f;
                Unity::Transform transform = player.playerTransform() ? player.playerTransform() : player.GetTransform();
                transform.SetLocalScale(Unity::Vector3(scale, scale, scale));
            }
        }
    }

    static void ProcessGodObjectCommands()
    {
        GodObjectCommand action = G->GodObjectAction;
        G->GodObjectAction = GodObjectCommand::None;

        if (action == GodObjectCommand::LockAimed)
            SetGodTarget(G->GodAimedObject, G->GodAimedObjectNetworked);
        else if (action == GodObjectCommand::ClearTarget)
        {
            ReleaseGodTelekinesis();
            SetGodTarget(null, false);
        }
        else if (action == GodObjectCommand::Undo)
            UndoLastGodAction();

        Unity::GameObject target = G->GodTargetObject;
        if (!target)
        {
            if (G->GodTelekinesisActive)
                ReleaseGodTelekinesis();
            SetGodTarget(null, false);
            return;
        }

        Unity::Transform transform = target.GetTransform();
        ProcessGodGizmoDrag(target, transform);
        Unity::Rigidbody body = target.GetComponent<Unity::Rigidbody>();
        if (action == GodObjectCommand::ToggleTelekinesis)
        {
            if (G->GodTelekinesisActive)
                ReleaseGodTelekinesis();
            else
            {
                PushGodUndo(GodUndoType::Transform, target);
                s_HeldObject = target;
                s_HeldBody = body;
                if (body)
                {
                    s_HeldBodyKinematic = body.GetIsKinematic();
                    s_HeldBodyGravity = body.GetUseGravity();
                    body.SetVelocity(Unity::Vector3::zero());
                    body.SetUseGravity(false);
                    body.SetIsKinematic(true);
                }
                G->GodTelekinesisActive = true;
            }
        }
        else if (action == GodObjectCommand::Pull && transform)
        {
            PushGodUndo(GodUndoType::Transform, target);
            if (Unity::Camera camera = SemiFunc::MainCamera())
                transform.SetPosition(camera.GetTransform().GetPosition() + camera.GetTransform().GetForward() * 2.f);
        }
        else if (action == GodObjectCommand::Push && transform)
        {
            PushGodUndo(GodUndoType::Transform, target);
            if (Unity::Camera camera = SemiFunc::MainCamera())
            {
                Unity::Vector3 force = camera.GetTransform().GetForward() * (float)G->GodThrowForce;
                if (body)
                {
                    body.SetIsKinematic(false);
                    body.SetUseGravity(true);
                    body.AddForce(force, Unity::ForceMode::VelocityChange);
                }
                else
                    transform.SetPosition(transform.GetPosition() + force * 0.15f);
            }
        }
        else if (action == GodObjectCommand::FreezeToggle && body)
        {
            const bool freeze = !body.GetIsKinematic();
            body.SetVelocity(Unity::Vector3::zero());
            body.SetUseGravity(!freeze);
            body.SetIsKinematic(freeze);
        }
        else if (action == GodObjectCommand::Duplicate && transform)
        {
            Unity::Vector3 pos = transform.GetPosition();
            if (Unity::Camera camera = SemiFunc::MainCamera())
                pos = pos + camera.GetTransform().GetRotation() * Unity::Vector3::right();
            Unity::GameObject clone = Unity::Object::Instantiate<Unity::GameObject>(target, pos, transform.GetRotation());
            if (clone)
            {
                clone.SetActive(true);
                PushGodUndo(GodUndoType::DestroyClone, clone);
                SetGodTarget(clone, false);
            }
        }
        else if (action == GodObjectCommand::ApplyTransform && transform)
        {
            PushGodUndo(GodUndoType::Transform, target);
            transform.SetLocalScale(Unity::Vector3(
                G->GodScaleXPercent / 100.f,
                G->GodScaleYPercent / 100.f,
                G->GodScaleZPercent / 100.f));
            transform.SetRotation(Unity::Quaternion::Euler((float)G->GodRotationX, (float)G->GodRotationY, (float)G->GodRotationZ));
            SyncGodTargetPose(target, transform);
        }
        else if (action == GodObjectCommand::DeleteTarget && !G->IsClient)
        {
            ReleaseGodTelekinesis();
            DeleteGodObject(target, G->GodTargetNetworked);
            SetGodTarget(null, false);
        }

        if (G->GodTelekinesisActive && s_HeldObject)
        {
            Unity::Camera camera = SemiFunc::MainCamera();
            Unity::Transform heldTransform = s_HeldObject.GetTransform();
            if (camera && heldTransform)
            {
                Unity::Transform cameraTransform = camera.GetTransform();
                heldTransform.SetPosition(cameraTransform.GetPosition() + cameraTransform.GetForward() * (float)G->GodHoldDistance);
                if (s_HeldBody)
                    s_HeldBody.SetVelocity(Unity::Vector3::zero());
            }
        }

        RefreshGodTargetSnapshot(G->GodTargetObject);
        UpdateGodGizmoProjection(G->GodTargetObject, G->GodTargetObject ? G->GodTargetObject.GetTransform() : null);
    }

    static void ProcessGodTools(PlayerAvatar avatar)
    {
        if (!G->IsInGame)
        {
            ResetExtractionPacking(false);
            DiscardGodSceneReferences();
            G->GodTargetingEnabled = false;
            G->WorldFreezePhysics = false;
            G->GodEnemiesFrozen = false;
            G->GodLootFrozen = false;
            G->GodEnemiesPacified = false;
            G->GodEnemiesPacifiedChanged = false;
            G->GodAllPlayers = false;
            G->GodAllPlayersNoTumble = false;
            G->WorldGravityOverride = false;
            G->WorldLightAction = WorldLightCommand::None;
        }

        ProcessWorldGodTools();
        if (!G->IsInGame)
            return;

        ProcessGodObjectCommands();
        ProcessEnemyGodTools();
        ProcessLootGodTools(avatar);
        ProcessPlayerGodTools(avatar);
    }

    struct EyeLaserPrefabState
    {
        SemiLaser Template = null;
        SemiLaser Left = null;
        SemiLaser Right = null;
        Item NetworkItem = null;
        bool SearchFailureLogged;
    };

    static EyeLaserPrefabState s_EyeLaserPrefab;

    static SemiLaser FindEyeLaserTemplate()
    {
        for (const auto& entry : G->ItemsPool)
        {
            Item item = entry.value;
            if (!item || item.prefab() == null)
                continue;

            Unity::GameObject prefab = item.prefab().Prefab();
            if (!prefab)
                continue;

            ItemGunLaser gunLaser = prefab.GetComponent<ItemGunLaser>();
            if (!gunLaser)
                gunLaser = prefab.GetTransform().GetComponentInChildren<ItemGunLaser>();

            if (gunLaser && gunLaser.semiLaser())
            {
                Hax::Log(G->Logger, L"Eye lasers: using game prefab from %ls", item.itemName().GetRawStringData());
                s_EyeLaserPrefab.NetworkItem = item;
                return gunLaser.semiLaser();
            }
        }

        return null;
    }

    static SemiLaser CreateEyeLaserFromPrefab(SemiLaser source, const Unity::Vector3& position)
    {
        if (!source)
            return null;

        Unity::Object cloned = Unity::Object::Instantiate((Unity::Object)source.GetGameObject());
        Unity::GameObject instance = std::bit_cast<Unity::GameObject>(cloned);
        if (!instance)
            return null;

        instance.SetActive(true);
        instance.GetTransform().SetPosition(position);

        SemiLaser laser = instance.GetComponent<SemiLaser>();
        if (laser)
        {
            laser.hurtColliderBeamThickness() = 0.32f;
            laser.beamThickness() = 1.2f;
            laser.beamHitSize() = 1.35f;
            laser.wobbleAmount() = 1.15f;
        }
        return laser;
    }

    static EyeLaserProtocol::Settings LocalEyeLaserSettings()
    {
        return {G->EyeLaserRange, G->EyeLaserDamage, G->EyeLaserInstantKill, G->EyeLaserDestroyObjects};
    }

    static void ConfigureEyeLaserDamage(SemiLaser laser, PlayerAvatar avatar, const EyeLaserProtocol::Settings& settings)
    {
        if (!laser)
            return;

        HurtCollider hurt = laser.hurtCollider();
        if (!hurt)
            hurt = laser.GetComponentInChildren<HurtCollider>();
        if (!hurt)
            return;

        hurt.playerLogic() = false;
        hurt.physLogic() = !G->IsClient;
        hurt.physDestroy() = settings.DestroyObjects;
        hurt.physHingeDestroy() = settings.DestroyObjects;
        hurt.physReleaseGrab() = true;
        hurt.physImpact() = 3; // HurtCollider.BreakImpact.Heavy
        hurt.physDamageCooldown() = 0.05f;
        hurt.enemyLogic() = !G->IsClient;
        hurt.enemyKill() = settings.InstantKill;
        hurt.enemyDamage() = settings.Damage;
        hurt.enemyDamageCooldown() = 0.05f;
        hurt.enemyHitTriggers() = true;
        hurt.playerCausingHurtOverride() = avatar;
    }

    struct NetworkEyeLaserCarrier
    {
        Unity::GameObject Object = null;
        PhysGrabObject Phys = null;
        ItemGun Gun = null;
        ItemGunLaser GunLaser = null;
        ItemBattery Battery = null;
        Unity::Vector3 MuzzleOffset{};
        Unity::Quaternion MuzzleRotationOffset = Unity::Quaternion::identity();
        uint64_t ReadyAt{};
    };

    static NetworkEyeLaserCarrier s_LeftNetworkEyeLaser;
    static NetworkEyeLaserCarrier s_RightNetworkEyeLaser;

    struct RemoteEyeLaserSession
    {
        int Actor = 0;
        EyeLaserProtocol::Request Request{};
        uint64_t LastReceivedAt = 0;
        uint32_t LastAcknowledgedSequence = 0;
        bool HasAcknowledged = false;
        NetworkEyeLaserCarrier Left;
        NetworkEyeLaserCarrier Right;
    };

    static std::array<RemoteEyeLaserSession, 16> s_RemoteEyeLasers;
    static EyeLaserProtocol::Request s_ClientEyeLaserRequest;
    static uint64_t s_NextEyeLaserRequestAt;
    static uint64_t s_LastEyeLaserAckAt;
    static uint32_t s_LastEyeLaserAckSequence;
    static int s_EyeLaserMasterActor;
    static bool s_ClientEyeLasersActive;

    static void DestroyLocalEyeLasers()
    {
        if (s_EyeLaserPrefab.Left)
            Unity::Object::Destroy(s_EyeLaserPrefab.Left.GetGameObject());
        if (s_EyeLaserPrefab.Right)
            Unity::Object::Destroy(s_EyeLaserPrefab.Right.GetGameObject());
        s_EyeLaserPrefab.Left = null;
        s_EyeLaserPrefab.Right = null;
    }

    static void DestroyNetworkEyeLaser(NetworkEyeLaserCarrier& carrier)
    {
        if (carrier.Object && G->IsInGame && SemiFunc::IsMultiplayer() && SemiFunc::IsMasterClient())
            Unity::Photon::PhotonNetwork::Destroy(carrier.Object);
        carrier = {};
    }

    static void DestroyNetworkEyeLasers()
    {
        DestroyNetworkEyeLaser(s_LeftNetworkEyeLaser);
        DestroyNetworkEyeLaser(s_RightNetworkEyeLaser);
    }

    static bool CreateNetworkEyeLaser(NetworkEyeLaserCarrier& carrier, const Unity::Vector3& position)
    {
        if (!s_EyeLaserPrefab.NetworkItem || s_EyeLaserPrefab.NetworkItem.prefab() == null)
            return false;

        carrier.Object = Unity::Photon::PhotonNetwork::InstantiateRoomObject(
            s_EyeLaserPrefab.NetworkItem.prefab().resourcePath(), position, Unity::Quaternion::identity());
        if (!carrier.Object)
            return false;

        carrier.Phys = carrier.Object.GetComponent<PhysGrabObject>();
        if (!carrier.Phys)
            carrier.Phys = carrier.Object.GetTransform().GetComponentInChildren<PhysGrabObject>();
        carrier.Gun = carrier.Object.GetComponent<ItemGun>();
        if (!carrier.Gun)
            carrier.Gun = carrier.Object.GetTransform().GetComponentInChildren<ItemGun>();
        carrier.GunLaser = carrier.Object.GetComponent<ItemGunLaser>();
        if (!carrier.GunLaser)
            carrier.GunLaser = carrier.Object.GetTransform().GetComponentInChildren<ItemGunLaser>();

        if (!carrier.Phys || !carrier.Gun || !carrier.GunLaser || !carrier.GunLaser.muzzleTransform())
        {
            DestroyNetworkEyeLaser(carrier);
            return false;
        }

        carrier.Battery = carrier.Gun.itemBattery();
        Unity::Transform root = carrier.Object.GetTransform();
        Unity::Transform muzzle = carrier.GunLaser.muzzleTransform();
        carrier.MuzzleOffset = muzzle.GetPosition() - root.GetPosition();
        carrier.MuzzleRotationOffset = muzzle.GetRotation();
        carrier.ReadyAt = ::GetTickCount64() + 500;
        return true;
    }

    static bool EnsureNetworkEyeLasers(const Unity::Vector3& leftEye, const Unity::Vector3& rightEye)
    {
        if (!s_LeftNetworkEyeLaser.Object && !CreateNetworkEyeLaser(s_LeftNetworkEyeLaser, leftEye))
            return false;
        if (!s_RightNetworkEyeLaser.Object && !CreateNetworkEyeLaser(s_RightNetworkEyeLaser, rightEye))
        {
            DestroyNetworkEyeLaser(s_LeftNetworkEyeLaser);
            return false;
        }
        return true;
    }

    static Unity::Quaternion InverseUnitQuaternion(const Unity::Quaternion& value)
    {
        const float squaredLength = value.X * value.X + value.Y * value.Y + value.Z * value.Z + value.W * value.W;
        if (squaredLength <= 0.000001f)
            return Unity::Quaternion::identity();
        const float inverseLength = 1.f / squaredLength;
        return Unity::Quaternion(-value.X * inverseLength, -value.Y * inverseLength,
            -value.Z * inverseLength, value.W * inverseLength);
    }

    static void UpdateNetworkEyeLaser(NetworkEyeLaserCarrier& carrier, const Unity::Vector3& eye,
        const Unity::Quaternion& muzzleRotation, PlayerAvatar avatar, const EyeLaserProtocol::Settings& settings)
    {
        if (!carrier.Object || !carrier.Phys || !carrier.Gun || !carrier.GunLaser)
            return;
        if (::GetTickCount64() < carrier.ReadyAt)
            return;

        const Unity::Quaternion rootRotation = muzzleRotation * InverseUnitQuaternion(carrier.MuzzleRotationOffset);
        const Unity::Vector3 rootPosition = eye - rootRotation * carrier.MuzzleOffset;
        carrier.Phys.Teleport(rootPosition, rootRotation);

        carrier.Gun.gunRange() = (float)settings.Range;
        ConfigureEyeLaserDamage(carrier.GunLaser.semiLaser(), avatar, settings);

        if (carrier.Battery && carrier.Battery.batteryLifeInt() <= 1)
            carrier.Battery.BatteryFullPercentChange(carrier.Battery.batteryBars(), true, false);

        if (carrier.Gun.stateCurrent() != 3 && carrier.Gun.photonView())
        {
            System::Array<System::Object> parameters = { System::Int32(3) };
            carrier.Gun.photonView().RPC(System::String::New("StateSetRPC"), Unity::Photon::RpcTarget::All, parameters);
        }
    }

    static int EyeLaserActorNumber(Unity::Photon::Player player)
    {
        return player != null ? std::bit_cast<EyeLaserPhotonPlayer>(player).ActorNumber() : 0;
    }

    static int EyeLaserViewId(Unity::Photon::PhotonView view)
    {
        return view ? std::bit_cast<EyeLaserPhotonView>(view).ViewId() : 0;
    }

    static PlayerAvatar FindRemoteEyeLaserPlayer(int viewId, int actor)
    {
        GameDirector director = GameDirector::instance();
        if (!director)
            return null;
        for (PlayerAvatar player : director.PlayerList())
        {
            if (!player || player.isLocal())
                continue;
            Unity::Photon::PhotonView view = player.photonView();
            if (EyeLaserViewId(view) == viewId && EyeLaserActorNumber(view.GetOwner()) == actor)
                return player;
        }
        return null;
    }

    static bool SendEyeLaserPacket(std::span<const int32_t> packet, int targetActor = 0)
    {
        System::Array<System::Int32> payload = System::Array<System::Int32>::CreateInstance(packet.size());
        for (size_t index = 0; index < packet.size(); ++index)
            payload[index] = System::Int32(packet[index]);
        EyeLaserRaiseEventOptions options = System::Activator::CreateInstanceDefaultCtor<EyeLaserRaiseEventOptions>();
        if (targetActor > 0)
        {
            const System::Array<System::Int32> targets{System::Int32(targetActor)};
            Hax::Unity::Internal::GCSetField(options, &options.TargetActors(), targets);
        }
        else
            options.Receivers() = 2; // ReceiverGroup.MasterClient; never broadcast client requests.
        return EyeLaserPhotonNetwork::RaiseEvent(EyeLaserProtocol::EventCode, payload, options);
    }

    static void Hooked__EyeLaserPhotonNetwork_OnEvent(EyeLaserPhotonEvent photonEvent)
    {
        try
        {
            if (photonEvent != null && photonEvent.Code() == EyeLaserProtocol::EventCode && G->IsInGame)
            {
                System::Object data = photonEvent.CustomData();
                if (data != null && System::Array<System::Int32>::typeof().IsInstanceOfType(data))
                {
                    auto payload = std::bit_cast<System::Array<System::Int32>>(data);
                    if (payload.Length() >= (int)EyeLaserProtocol::AckSize && payload.Length() <= (int)EyeLaserProtocol::RequestSize)
                    {
                        std::array<int32_t, EyeLaserProtocol::RequestSize> values{};
                        for (int index = 0; index < payload.Length(); ++index)
                            values[index] = payload[index].Value;
                        const std::span<const int32_t> packet(values.data(), payload.Length());
                        const int sender = photonEvent.Sender();
                        const uint64_t now = ::GetTickCount64();
                        if (SemiFunc::IsMultiplayer() && SemiFunc::IsMasterClient())
                        {
                            EyeLaserProtocol::Request request;
                            if (G->EyeLaserHostRelayEnabled && sender > 0 && EyeLaserProtocol::Decode(packet, request) &&
                                FindRemoteEyeLaserPlayer(request.ViewId, sender))
                            {
                                RemoteEyeLaserSession* slot = nullptr;
                                for (auto& session : s_RemoteEyeLasers)
                                {
                                    if (session.Actor == sender) { slot = &session; break; }
                                    if (!slot && session.Actor == 0) slot = &session;
                                }
                                if (slot && (slot->Actor == 0 || slot->Request.ViewId != request.ViewId ||
                                    EyeLaserProtocol::IsNewer(request.Sequence, slot->Request.Sequence)))
                                {
                                    slot->Actor = sender;
                                    slot->Request = request;
                                    slot->LastReceivedAt = now;
                                }
                            }
                        }
                        else if (sender > 0 && sender == s_EyeLaserMasterActor &&
                            sender == EyeLaserActorNumber(Unity::Photon::PhotonNetwork::GetMasterClient()) &&
                            EyeLaserProtocol::IsAcknowledgement(packet, s_ClientEyeLaserRequest.ViewId))
                        {
                            const uint32_t sequence = std::bit_cast<uint32_t>(packet[3]);
                            if (!EyeLaserProtocol::IsNewer(sequence, s_ClientEyeLaserRequest.Sequence) &&
                                (s_LastEyeLaserAckAt == 0 || EyeLaserProtocol::IsNewer(sequence, s_LastEyeLaserAckSequence)))
                            {
                                s_LastEyeLaserAckAt = now;
                                s_LastEyeLaserAckSequence = sequence;
                            }
                        }
                    }
                }
            }
        }
        catch (System::Exception&)
        {
            // A malformed custom event must not interrupt Photon's normal dispatch.
        }
        G->EyeLaserPhotonNetwork_OnEvent_Hook.unsafe_call<void, EyeLaserPhotonEvent>(photonEvent);
    }

    static void DestroyRemoteEyeLaserSession(RemoteEyeLaserSession& session)
    {
        DestroyNetworkEyeLaser(session.Left);
        DestroyNetworkEyeLaser(session.Right);
        session = {};
    }

    static void UpdateEyeLaserRelay()
    {
        const uint64_t now = ::GetTickCount64();
        const bool multiplayer = G->IsInGame && SemiFunc::IsMultiplayer();
        const bool host = multiplayer && SemiFunc::IsMasterClient();
        if (!host)
        {
            for (auto& session : s_RemoteEyeLasers)
                DestroyRemoteEyeLaserSession(session);
        }

        if (!multiplayer || host)
        {
            s_ClientEyeLasersActive = false;
            s_LastEyeLaserAckAt = 0;
            s_EyeLaserMasterActor = 0;
            G->EyeLaserHostConnected = false;
        }
        else
        {
            const int master = EyeLaserActorNumber(Unity::Photon::PhotonNetwork::GetMasterClient());
            PlayerAvatar avatar = PlayerAvatar::instance();
            const int viewId = avatar ? EyeLaserViewId(avatar.photonView()) : 0;
            if (master != s_EyeLaserMasterActor || viewId != s_ClientEyeLaserRequest.ViewId)
            {
                s_LastEyeLaserAckAt = 0;
                s_NextEyeLaserRequestAt = 0;
                s_ClientEyeLasersActive = false;
                s_EyeLaserMasterActor = master;
                s_ClientEyeLaserRequest.ViewId = viewId;
            }
            Unity::Camera camera = SemiFunc::MainCamera();
            const bool active = G->EyeLasersEnabled && !G->MenuVisible && IsPlayerUsable(avatar) && camera && viewId > 0;
            if (master > 0 && viewId > 0 && (active || s_ClientEyeLasersActive) &&
                (active != s_ClientEyeLasersActive || now >= s_NextEyeLaserRequestAt))
            {
                s_ClientEyeLaserRequest.Active = active;
                ++s_ClientEyeLaserRequest.Sequence;
                s_ClientEyeLaserRequest.Options = LocalEyeLaserSettings();
                if (camera)
                {
                    const Unity::Vector3 direction = camera.GetTransform().GetForward().GetNormalized();
                    s_ClientEyeLaserRequest.Direction = {direction.X, direction.Y, direction.Z};
                }
                if (SendEyeLaserPacket(EyeLaserProtocol::Encode(s_ClientEyeLaserRequest)))
                {
                    s_ClientEyeLasersActive = active;
                    s_NextEyeLaserRequestAt = now + EyeLaserProtocol::SendIntervalMs;
                }
            }
            G->EyeLaserHostConnected = s_LastEyeLaserAckAt != 0 && !EyeLaserProtocol::HasExpired(now, s_LastEyeLaserAckAt);
        }

        if (!host)
            return;

        for (auto& session : s_RemoteEyeLasers)
        {
            if (session.Actor == 0)
                continue;
            try
            {
                PlayerAvatar avatar = FindRemoteEyeLaserPlayer(session.Request.ViewId, session.Actor);
                if (!G->EyeLaserHostRelayEnabled || EyeLaserProtocol::HasExpired(now, session.LastReceivedAt) || !IsPlayerUsable(avatar))
                {
                    DestroyRemoteEyeLaserSession(session);
                    continue;
                }
                if (session.Request.Active)
                {
                    if (!s_EyeLaserPrefab.Template && !G->ItemsPool.Empty())
                        s_EyeLaserPrefab.Template = FindEyeLaserTemplate();
                    if (!s_EyeLaserPrefab.Template || !avatar.spectatePoint())
                        continue;

                    const auto& direction = session.Request.Direction;
                    constexpr float degrees = 57.2957795131f;
                    const Unity::Quaternion rotation = Unity::Quaternion::Euler(
                        -std::asin(std::clamp(direction[1], -1.f, 1.f)) * degrees,
                        std::atan2(direction[0], direction[2]) * degrees, 0.f);
                    const Unity::Vector3 forward = rotation * Unity::Vector3::forward();
                    const Unity::Vector3 right = rotation * Unity::Vector3::right();
                    const Unity::Vector3 up = rotation * Unity::Vector3::up();
                    const Unity::Vector3 center = avatar.spectatePoint().GetPosition() + forward * 0.16f - up * 0.025f;
                    const Unity::Vector3 leftEye = center - right * 0.055f;
                    const Unity::Vector3 rightEye = center + right * 0.055f;
                    if (!session.Left.Object && !CreateNetworkEyeLaser(session.Left, leftEye))
                        continue;
                    if (!session.Right.Object && !CreateNetworkEyeLaser(session.Right, rightEye))
                    {
                        DestroyNetworkEyeLaser(session.Left);
                        continue;
                    }
                    UpdateNetworkEyeLaser(session.Left, leftEye, rotation, avatar, session.Request.Options);
                    UpdateNetworkEyeLaser(session.Right, rightEye, rotation, avatar, session.Request.Options);
                    if (now < session.Left.ReadyAt || now < session.Right.ReadyAt)
                        continue;
                }
                else
                {
                    DestroyNetworkEyeLaser(session.Left);
                    DestroyNetworkEyeLaser(session.Right);
                }

                // Acknowledge only after the host has actually processed the request.
                if (!session.HasAcknowledged || session.LastAcknowledgedSequence != session.Request.Sequence)
                {
                    if (SendEyeLaserPacket(EyeLaserProtocol::Acknowledge(session.Request), session.Actor))
                    {
                        session.HasAcknowledged = true;
                        session.LastAcknowledgedSequence = session.Request.Sequence;
                    }
                }
            }
            catch (System::Exception& ex)
            {
                System::String message = ex.Message();
                Hax::LogError(G->Logger, L"Eye laser relay for actor %d: %ls", session.Actor,
                    message != null ? message.GetRawStringData() : L"Exception without message");
                DestroyRemoteEyeLaserSession(session);
            }
        }
    }

    static void UpdateEyeLasers()
    {
        const wchar_t* stage = L"startup";
        try
        {
            stage = L"host relay";
            UpdateEyeLaserRelay();

            PlayerAvatar avatar = PlayerAvatar::instance();
            if (!G->EyeLasersEnabled || !G->IsInGame || G->MenuVisible)
            {
                stage = L"laser cleanup";
                DestroyNetworkEyeLasers();
                DestroyLocalEyeLasers();
                return;
            }

            stage = L"prefab lookup";
            if (!s_EyeLaserPrefab.Template && !G->ItemsPool.Empty())
                s_EyeLaserPrefab.Template = FindEyeLaserTemplate();

            if (!s_EyeLaserPrefab.Template)
            {
                if (!G->ItemsPool.Empty() && !s_EyeLaserPrefab.SearchFailureLogged)
                {
                    Hax::LogError(G->Logger, L"Eye lasers: ItemGunLaser prefab was not found");
                    s_EyeLaserPrefab.SearchFailureLogged = true;
                }
                return;
            }

            stage = L"camera pose";
            Unity::Camera camera = SemiFunc::MainCamera();
            if (!IsPlayerUsable(avatar) || !camera)
            {
                DestroyNetworkEyeLasers();
                DestroyLocalEyeLasers();
                return;
            }

            Unity::Transform cameraTransform = camera.GetTransform();
            Unity::Quaternion rotation = cameraTransform.GetRotation();
            Unity::Vector3 forward = (rotation * Unity::Vector3::forward()).GetNormalized();
            Unity::Vector3 right = (rotation * Unity::Vector3::right()).GetNormalized();
            Unity::Vector3 up = (rotation * Unity::Vector3::up()).GetNormalized();

            Unity::Transform eyeAnchor = avatar.spectatePoint();
            Unity::Vector3 rayOrigin = eyeAnchor ? eyeAnchor.GetPosition() : cameraTransform.GetPosition();
            Unity::Vector3 eyeCenter = rayOrigin + forward * 0.16f - up * 0.025f;
            Unity::Vector3 leftEye = eyeCenter - right * 0.055f;
            Unity::Vector3 rightEye = eyeCenter + right * 0.055f;

            // A room-owned Photon Blaster is understood by stock clients: its
            // native transform stream and ItemGun state RPC make the game itself
            // draw the beams. No RepoHax receiver is needed on other players.
            if (SemiFunc::IsMultiplayer() && !G->IsClient)
            {
                stage = L"local cleanup";
                DestroyLocalEyeLasers();
                stage = L"network spawn";
                if (!EnsureNetworkEyeLasers(leftEye, rightEye))
                    return;

                stage = L"left network update";
                UpdateNetworkEyeLaser(s_LeftNetworkEyeLaser, leftEye, rotation, avatar, LocalEyeLaserSettings());
                stage = L"right network update";
                UpdateNetworkEyeLaser(s_RightNetworkEyeLaser, rightEye, rotation, avatar, LocalEyeLaserSettings());
                return;
            }

            DestroyNetworkEyeLasers();

            if (G->IsClient && G->EyeLaserHostConnected)
            {
                DestroyLocalEyeLasers();
                return;
            }

            if (!s_EyeLaserPrefab.Left)
                s_EyeLaserPrefab.Left = CreateEyeLaserFromPrefab(s_EyeLaserPrefab.Template, leftEye);
            if (!s_EyeLaserPrefab.Right)
                s_EyeLaserPrefab.Right = CreateEyeLaserFromPrefab(s_EyeLaserPrefab.Template, rightEye);
            if (!s_EyeLaserPrefab.Left || !s_EyeLaserPrefab.Right)
                return;

            Unity::Vector3 endPoint = rayOrigin + forward * (float)G->EyeLaserRange;
            bool hitSomething = false;
            float closestDistance = (float)G->EyeLaserRange;
            const int layerMask = SemiFunc::LayerMaskGetVisionObstruct();
            System::Array<Unity::RaycastHit> hits = Unity::Physics::SphereCastAll(rayOrigin, 0.025f, forward, (float)G->EyeLaserRange, layerMask);
            if (hits != null)
            {
                Unity::Transform avatarRoot = avatar.GetTransform();
                Unity::Transform playerRoot = avatar.playerTransform();
                Unity::Transform controllerRoot = PlayerController::instance() ? PlayerController::instance().GetTransform() : null;

                for (Unity::RaycastHit& hit : hits)
                {
                    if (hit.m_Distance < 0.05f || hit.m_Distance >= closestDistance)
                        continue;

                    Unity::Transform hitTransform = hit.GetTransform();
                    if (!hitTransform || IsTransformBelow(hitTransform, avatarRoot) ||
                        (playerRoot && IsTransformBelow(hitTransform, playerRoot)) ||
                        (controllerRoot && IsTransformBelow(hitTransform, controllerRoot)))
                    {
                        continue;
                    }

                    Unity::Collider collider = hitTransform.GetComponent<Unity::Collider>();
                    if (collider && IsTriggerCollider(collider))
                        continue;

                    closestDistance = hit.m_Distance;
                    endPoint = hit.m_Point;
                    hitSomething = true;
                }
            }

            ConfigureEyeLaserDamage(s_EyeLaserPrefab.Left, avatar, LocalEyeLaserSettings());
            ConfigureEyeLaserDamage(s_EyeLaserPrefab.Right, avatar, LocalEyeLaserSettings());
            s_EyeLaserPrefab.Left.LaserActive(leftEye, endPoint, hitSomething);
            s_EyeLaserPrefab.Right.LaserActive(rightEye, endPoint, hitSomething);
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"Eye lasers (%ls): %ls", stage,
                message != null ? message.ToString().GetRawStringData() : L"Exception without message");
            G->EyeLasersEnabled = false;
        }
    }

    // ---------------------------------------------------------------- Fun: drones, FPV, orbital, black hole, chaos
    // Everything here is host-side and made of stock room objects, so stock
    // clients see and hear it natively:
    //  * a drone is a stock drone item (model + buzzing loop) steered through
    //    rigidbody velocity, i.e. the game's own physics stream, no per-frame RPCs;
    //  * blasts are stock explosive grenades detonated through the game's own
    //    TickEnd RPC, so every client plays the explosion and takes the damage.
    // Grenades are only spawned at the moment of impact, so a flying drone costs
    // exactly one networked object. That is what makes "endless" swarms possible:
    // the only hard ceiling left is Photon's pool of room view IDs, and when it
    // runs dry the launch queue simply waits instead of failing.

    constexpr int      DroneMaxWarheads = 3;
    constexpr int      DroneMaxSwarm = 100;
    constexpr int      DroneSpawnPerFrame = 4;
    constexpr float    DroneLaunchTime = 0.45f;
    constexpr float    DroneArmTime = 0.3f;        // hover so grenades run Start() and clients catch up
    constexpr float    DroneLostTargetGrace = 0.6f;
    constexpr float    DroneTargetFuse = 1.25f;
    constexpr float    DroneBystanderFuse = 1.0f;
    constexpr float    DroneCleanupDelay = 1.f;
    constexpr float    DroneFlightHeight = 1.1f;
    constexpr uint64_t DroneSpawnRetryMs = 1500;
    constexpr uint64_t FpvLostScreenMs = 1300;

    enum class DronePhase { Idle, Launch, Cruise, Loiter, Arming, Done };

    struct DroneWarhead
    {
        Unity::GameObject Object = null;
        Unity::Rigidbody Body = null;
        ItemGrenade Grenade = null;
        Unity::Vector3 Offset{};
    };

    struct DroneSlot
    {
        DronePhase Phase = DronePhase::Idle;
        uint32_t Serial = 0;
        Unity::GameObject Object = null;
        Unity::Rigidbody Body = null;
        DroneWarhead Warheads[DroneMaxWarheads]{};
        int WarheadCount = 0;
        int WarheadsWanted = 1;
        bool Activated = false;
        bool Manual = false;
        bool Lethal = false;
        bool ThroughWalls = true;
        bool Retarget = true;
        float Speed = 14.f;
        float Age = 0.f;
        float PhaseTime = 0.f;
        float LostTime = 0.f;
        float Bob = 0.f;
        float RetargetTimer = 0.f;
        float LastYaw = 1e9f;
        float GoalDistance = 0.f;
        Unity::Vector3 Position{};
        Unity::Vector3 Velocity{};
        Unity::Vector3 LaunchDir{};
        Unity::Vector3 LastGoal{};
        Unity::Vector3 LoiterCenter{};
        bool HasLastGoal = false;
        // NavMesh route (walls mode): corners lifted to flight height.
        Unity::Vector3 Path[24]{};
        int PathCount = 0;
        int PathIndex = 0;
        Unity::Vector3 PathGoal{};
        float PathTimer = 0.f;
        float LosTimer = 0.f;
        bool LosClear = false;
        PlayerAvatar TargetPlayer = null;
        EnemyParent TargetEnemy = null;
        PlayerAvatar HitPlayer = null;
        EnemyParent HitEnemy = null;
    };

    struct DronePrefabs
    {
        Item Drone = null;
        Item Grenade = null;
        bool FailureLogged = false;
    };

    struct DroneLaunchOrder
    {
        int Total = 0;
        int Next = 0;
        int Launched = 0;
        int Warheads = 1;
        float Speed = 14.f;
        bool Manual = false;
        bool Lethal = false;
        bool ThroughWalls = true;
        bool Retarget = true;
        bool Quiet = false;
        PlayerAvatar TargetPlayer = null;
        EnemyParent TargetEnemy = null;
    };

    struct FunFrameTarget
    {
        PlayerAvatar Player = null;
        EnemyParent Enemy = null;
        Unity::Vector3 Point{};
    };

    static std::vector<DroneSlot> s_Drones;
    static std::deque<DroneLaunchOrder> s_DroneOrders;
    static DronePrefabs s_DronePrefabs;
    static uint64_t s_DroneSpawnBlockedUntil;
    static uint32_t s_DroneSerial;

    static std::vector<FunFrameTarget> s_FramePlayers;
    static std::vector<FunFrameTarget> s_FrameEnemies;
    static uint64_t s_FunFrame;
    static uint64_t s_FrameTargetsBuiltAt;
    static uint32_t s_FunRng = 0x9E3779B9u;
    static bool s_FunReleasing; // our own detonation is destroying a grenade: let it through

    static int FunRandom(int count)
    {
        if (count <= 0)
            return 0;
        s_FunRng ^= s_FunRng << 13;
        s_FunRng ^= s_FunRng >> 17;
        s_FunRng ^= s_FunRng << 5;
        return (int)(s_FunRng % (uint32_t)count);
    }

    static float FunRandom01()
    {
        return (float)FunRandom(100000) / 100000.f;
    }

    static void LogFunException(const wchar_t* where, System::Exception& ex)
    {
        System::String message = ex.Message();
        Hax::LogError(G->Logger, L"%ls: %ls", where, message != null ? message.GetRawStringData() : L"?");
    }

    static bool DroneNameContains(Hax::WStringView name, const wchar_t* needle)
    {
        const size_t n = wcslen(needle);
        if (!name.Data() || name.Size() < n)
            return false;
        for (size_t i = 0; i + n <= name.Size(); ++i)
            if (_wcsnicmp(name.Data() + i, needle, n) == 0)
                return true;
        return false;
    }

    template <typename T>
    static T DroneFindComponent(Unity::GameObject object)
    {
        if (!object)
            return null;
        T component = object.GetComponent<T>();
        if (!component)
            component = object.GetTransform().GetComponentInChildren<T>();
        return component;
    }

    static bool FindDronePrefabs()
    {
        if (s_DronePrefabs.Drone && s_DronePrefabs.Grenade)
            return true;
        if (G->ItemsPool.Empty())
            return false;

        Item grenadeFallback = null;
        // First pass only loads prefabs whose names look right; the second one is a
        // full scan in case the item names ever change.
        for (int pass = 0; pass < 2 && !(s_DronePrefabs.Drone && s_DronePrefabs.Grenade); ++pass)
        {
            for (const auto& entry : G->ItemsPool)
            {
                const bool wantDrone = !s_DronePrefabs.Drone && (pass == 1 || DroneNameContains(entry.key, L"drone"));
                const bool wantGrenade = !s_DronePrefabs.Grenade && (pass == 1 || DroneNameContains(entry.key, L"grenade"));
                if (!wantDrone && !wantGrenade)
                    continue;

                Item item = entry.value;
                if (!item || item.prefab() == null)
                    continue;
                Unity::GameObject prefab = item.prefab().Prefab();
                if (!prefab)
                    continue;

                if (wantDrone && DroneFindComponent<ItemDrone>(prefab))
                    s_DronePrefabs.Drone = item;

                if (wantGrenade && DroneFindComponent<ItemGrenadeExplosive>(prefab) && DroneFindComponent<ItemGrenade>(prefab))
                {
                    // Prefer the plain explosive grenade over taped or cluster variants.
                    if (DroneNameContains(entry.key, L"explosive") || entry.key == Hax::WStringView(L"Grenade"))
                        s_DronePrefabs.Grenade = item;
                    else if (!grenadeFallback)
                        grenadeFallback = item;
                }
            }
            if (!s_DronePrefabs.Grenade && grenadeFallback)
                s_DronePrefabs.Grenade = grenadeFallback;
        }

        if (s_DronePrefabs.Drone && s_DronePrefabs.Grenade)
        {
            Hax::Log(G->Logger, L"Drone strike: drone = %ls, warhead = %ls",
                s_DronePrefabs.Drone.itemName().GetRawStringData(), s_DronePrefabs.Grenade.itemName().GetRawStringData());
            return true;
        }

        if (!s_DronePrefabs.FailureLogged)
        {
            Hax::LogError(G->Logger, L"Drone strike: prefab not found (drone %d, grenade %d)",
                (int)(bool)s_DronePrefabs.Drone, (int)(bool)s_DronePrefabs.Grenade);
            s_DronePrefabs.FailureLogged = true;
        }
        return false;
    }

    // Throws when Photon has no free room view ID left; callers decide what to do.
    static Unity::GameObject SpawnDroneObject(Item item, const Unity::Vector3& position, const Unity::Quaternion& rotation)
    {
        if (SemiFunc::IsMultiplayer())
        {
            if (!SemiFunc::IsMasterClient())
                return null;
            return Unity::Photon::PhotonNetwork::InstantiateRoomObject(item.prefab().resourcePath(), position, rotation);
        }
        return Unity::Object::Instantiate<Unity::GameObject>(item.prefab().Prefab(), position, rotation);
    }

    static void DestroyDroneObject(Unity::GameObject& object)
    {
        try
        {
            if (object)
            {
                if (!SemiFunc::IsMultiplayer())
                    Unity::Object::Destroy(object);
                else if (G->IsInGame && SemiFunc::IsMasterClient())
                    Unity::Photon::PhotonNetwork::Destroy(object);
            }
        }
        catch (System::Exception& ex)
        {
            LogFunException(L"Fun destroy", ex);
        }
        object = null;
    }

    // The host owns the physics. With colliders off the item glides along our path
    // without snagging on doorframes or shoving players before it goes off.
    static Unity::Rigidbody PrepareDroneBody(Unity::GameObject object)
    {
        System::Array<Unity::Collider> colliders = object.GetTransform().GetComponentsInChildren<Unity::Collider>(true);
        if (colliders != null)
        {
            for (Unity::Collider collider : colliders)
                if (collider)
                    collider.SetEnabled(false);
        }

        Unity::Rigidbody body = DroneFindComponent<Unity::Rigidbody>(object);
        if (body)
            body.SetUseGravity(false);
        return body;
    }

    static void DriveDroneBody(Unity::GameObject object, Unity::Rigidbody body, const Unity::Vector3& goal,
        const Unity::Vector3& velocity, const Unity::Quaternion* rotation)
    {
        if (!object)
            return;

        Unity::Transform transform = object.GetTransform();
        Unity::Vector3 error = goal - transform.GetPosition();
        if (error.GetMagnitude() > 6.f)
        {
            // Way off (spawn hiccup, physics pop): snap instead of rubber-banding.
            transform.SetPosition(goal);
            error = Unity::Vector3::zero();
        }
        if (rotation)
            transform.SetRotation(*rotation);
        if (body)
            body.SetVelocity(velocity + error * 10.f);
    }

    static Unity::Vector3 DronePlayerPoint(PlayerAvatar player)
    {
        Unity::Transform root = GetPlayerTransformSafe(player);
        Unity::Vector3 feet = root ? root.GetPosition() : player.GetTransform().GetPosition();
        if (Unity::Transform head = player.spectatePoint())
            return (feet + head.GetPosition()) * 0.5f;
        return feet + Unity::Vector3::up() * 0.85f;
    }

    static bool DroneEnemyPoint(EnemyParent parent, Unity::Vector3& out)
    {
        if (!parent || !parent.Spawned())
            return false;
        Enemy enemy = parent.enemy();
        if (!enemy)
            return false;
        if (EnemyHealth health = enemy.Health(); health && health.dead())
            return false;
        Unity::Transform center = enemy.CenterTransform();
        out = center ? center.GetPosition() : enemy.GetTransform().GetPosition();
        return true;
    }

    // One snapshot of living players and enemies per frame, shared by every drone,
    // fuse and effect. This keeps big swarms from hammering the game every frame.
    static void EnsureFrameTargets()
    {
        if (s_FrameTargetsBuiltAt == s_FunFrame)
            return;
        s_FrameTargetsBuiltAt = s_FunFrame;
        s_FramePlayers.clear();
        s_FrameEnemies.clear();

        if (GameDirector director = GameDirector::instance())
        {
            for (PlayerAvatar player : director.PlayerList())
                if (IsPlayerUsable(player))
                    s_FramePlayers.push_back({player, null, DronePlayerPoint(player)});
        }
        if (EnemyDirector enemies = EnemyDirector::instance(); enemies && enemies.enemiesSpawned() != null)
        {
            for (EnemyParent parent : enemies.enemiesSpawned())
            {
                Unity::Vector3 point;
                if (DroneEnemyPoint(parent, point))
                    s_FrameEnemies.push_back({null, parent, point});
            }
        }
    }

    static bool FunFindPlayerPoint(PlayerAvatar player, Unity::Vector3& out)
    {
        EnsureFrameTargets();
        for (const FunFrameTarget& entry : s_FramePlayers)
        {
            if (entry.Player == player)
            {
                out = entry.Point;
                return true;
            }
        }
        return false;
    }

    static bool FunFindEnemyPoint(EnemyParent enemy, Unity::Vector3& out)
    {
        EnsureFrameTargets();
        for (const FunFrameTarget& entry : s_FrameEnemies)
        {
            if (entry.Enemy == enemy)
            {
                out = entry.Point;
                return true;
            }
        }
        return false;
    }

    static bool FunTargetPoint(PlayerAvatar player, EnemyParent enemy, Unity::Vector3& out)
    {
        if (player)
            return FunFindPlayerPoint(player, out);
        if (enemy)
            return FunFindEnemyPoint(enemy, out);
        return false;
    }

    // Random living target for chaos events. `spareMe` keeps the host out of it.
    static bool FunPickRandomTarget(bool spareMe, PlayerAvatar& player, EnemyParent& enemy, Unity::Vector3& point)
    {
        EnsureFrameTargets();
        PlayerAvatar local = PlayerAvatar::instance();
        int players = 0;
        for (const FunFrameTarget& entry : s_FramePlayers)
            if (!(spareMe && entry.Player == local))
                ++players;
        const int total = players + (int)s_FrameEnemies.size();
        if (total == 0)
            return false;

        int pick = FunRandom(total);
        player = null;
        enemy = null;
        for (const FunFrameTarget& entry : s_FramePlayers)
        {
            if (spareMe && entry.Player == local)
                continue;
            if (pick-- == 0)
            {
                player = entry.Player;
                point = entry.Point;
                return true;
            }
        }
        const FunFrameTarget& entry = s_FrameEnemies[(size_t)pick];
        enemy = entry.Enemy;
        point = entry.Point;
        return true;
    }

    static Unity::Quaternion DroneRotation(const Unity::Vector3& velocity)
    {
        constexpr float degrees = 57.2957795131f;
        const float yaw = velocity.GetMagnitude() > 0.1f ? std::atan2(velocity.X, velocity.Z) * degrees : 0.f;
        return Unity::Quaternion::Euler(12.f, yaw, 0.f); // nose slightly down, like a real quad in forward flight
    }

    static Unity::Quaternion FunLookRotation(const Unity::Vector3& direction)
    {
        constexpr float degrees = 57.2957795131f;
        const Unity::Vector3 dir = direction.GetNormalized();
        return Unity::Quaternion::Euler(-std::asin(std::clamp(dir.Y, -1.f, 1.f)) * degrees, std::atan2(dir.X, dir.Z) * degrees, 0.f);
    }

    static bool FunIgnoreHit(Unity::RaycastHit& hit, Unity::Transform ignoreA, Unity::Transform ignoreB, Unity::Transform ignoreC)
    {
        Unity::Transform hitTransform = hit.GetTransform();
        if (!hitTransform)
            return true;
        if ((ignoreA && IsTransformBelow(hitTransform, ignoreA)) || (ignoreB && IsTransformBelow(hitTransform, ignoreB)) ||
            (ignoreC && IsTransformBelow(hitTransform, ignoreC)))
            return true;
        return IsTriggerCollider(hitTransform.GetComponent<Unity::Collider>());
    }

    // Where the crosshair points. Manual drones chase this point; orbital strikes and
    // black holes are placed on it.
    static Unity::Vector3 DroneCrosshairPoint(Unity::Camera camera, PlayerAvatar local)
    {
        Unity::Transform camTransform = camera.GetTransform();
        const Unity::Vector3 origin = camTransform.GetPosition();
        const Unity::Vector3 forward = camTransform.GetForward().GetNormalized();
        constexpr float maxRange = 250.f;
        Unity::Vector3 point = origin + forward * maxRange;
        float closest = maxRange;

        System::Array<Unity::RaycastHit> hits = Unity::Physics::SphereCastAll(origin, 0.05f, forward, maxRange,
            SemiFunc::LayerMaskGetVisionObstruct());
        if (hits == null)
            return point;

        Unity::Transform avatarRoot = local ? local.GetTransform() : null;
        Unity::Transform playerRoot = local ? local.playerTransform() : null;
        Unity::Transform controllerRoot = PlayerController::instance() ? PlayerController::instance().GetTransform() : null;
        for (Unity::RaycastHit& hit : hits)
        {
            if (hit.m_Distance < 0.3f || hit.m_Distance >= closest)
                continue;
            if (FunIgnoreHit(hit, avatarRoot, playerRoot, controllerRoot))
                continue;
            closest = hit.m_Distance;
            point = hit.m_Point;
        }
        return point;
    }

    // First solid surface along `direction` (floor, ceiling). Returns false if none.
    static bool FunCastSolid(const Unity::Vector3& origin, const Unity::Vector3& direction, float range, Unity::Transform ignore,
        Unity::Vector3& hitPoint, float& hitDistance)
    {
        System::Array<Unity::RaycastHit> hits = Unity::Physics::SphereCastAll(origin, 0.12f, direction, range,
            SemiFunc::LayerMaskGetVisionObstruct());
        if (hits == null)
            return false;
        bool found = false;
        hitDistance = range;
        for (Unity::RaycastHit& hit : hits)
        {
            if (hit.m_Distance <= 0.f || hit.m_Distance >= hitDistance)
                continue;
            if (FunIgnoreHit(hit, ignore, null, null) || IsPlayerHierarchy(hit.GetTransform()))
                continue;
            hitDistance = hit.m_Distance;
            hitPoint = hit.m_Point;
            found = true;
        }
        return found;
    }

    static Unity::Vector3 FunFloorBelow(const Unity::Vector3& point, Unity::Transform ignore = null)
    {
        Unity::Vector3 hitPoint;
        float distance;
        if (FunCastSolid(point + Unity::Vector3::up() * 0.3f, Unity::Vector3::down(), 30.f, ignore, hitPoint, distance))
            return hitPoint;
        return point;
    }

    static Unity::Vector3 FunCeilingAbove(const Unity::Vector3& ground)
    {
        Unity::Vector3 hitPoint;
        float distance;
        if (FunCastSolid(ground + Unity::Vector3::up() * 0.5f, Unity::Vector3::up(), 40.f, null, hitPoint, distance))
            return ground + Unity::Vector3::up() * std::max(2.5f, 0.5f + distance - 0.4f);
        return ground + Unity::Vector3::up() * 30.f;
    }

    // Proximity fuse. Auto drones only care about their target; manual and piloted
    // drones go off on anybody they touch except the pilot.
    static bool DroneCheckFuse(DroneSlot& slot, PlayerAvatar local)
    {
        Unity::Vector3 point;
        if (!slot.Manual)
        {
            if (FunTargetPoint(slot.TargetPlayer, slot.TargetEnemy, point) &&
                Unity::Vector3::Distance(slot.Position, point) < DroneTargetFuse)
            {
                slot.HitPlayer = slot.TargetPlayer;
                slot.HitEnemy = slot.TargetEnemy;
                return true;
            }
            return false;
        }

        EnsureFrameTargets();
        for (const FunFrameTarget& entry : s_FramePlayers)
        {
            if (entry.Player != local && Unity::Vector3::Distance(slot.Position, entry.Point) < DroneBystanderFuse)
            {
                slot.HitPlayer = entry.Player;
                return true;
            }
        }
        for (const FunFrameTarget& entry : s_FrameEnemies)
        {
            if (Unity::Vector3::Distance(slot.Position, entry.Point) < DroneTargetFuse)
            {
                slot.HitEnemy = entry.Enemy;
                return true;
            }
        }
        return false;
    }

    // Wall handling when the drone is not allowed to ghost through geometry:
    // slide along surfaces that are close ahead, crash into the ones it flies straight into.
    static bool DroneAvoidWalls(DroneSlot& slot, PlayerAvatar local, float step, float dt)
    {
        const float speed = slot.Velocity.GetMagnitude();
        if (speed < 0.1f)
            return false;

        const Unity::Vector3 direction = slot.Velocity * (1.f / speed);
        const float lookAhead = step + 1.8f;
        System::Array<Unity::RaycastHit> hits = Unity::Physics::SphereCastAll(slot.Position, 0.22f, direction, lookAhead,
            SemiFunc::LayerMaskGetVisionObstruct());
        if (hits == null)
            return false;

        Unity::Transform avatarRoot = local ? local.GetTransform() : null;
        Unity::Transform playerRoot = local ? local.playerTransform() : null;
        float closest = lookAhead;
        Unity::Vector3 normal{};
        bool found = false;
        for (Unity::RaycastHit& hit : hits)
        {
            // Distance 0 means the cast started inside a collider (just spawned
            // in a wall, scraping a corner): that is not something ahead of us.
            if (hit.m_Distance <= 0.f || hit.m_Distance >= closest)
                continue;
            if (FunIgnoreHit(hit, avatarRoot, playerRoot, null))
                continue;
            closest = hit.m_Distance;
            normal = hit.m_Normal;
            found = true;
        }
        if (!found)
            return false;

        // Crash only when flying straight into it, and never right after launch;
        // a glancing hit is handled by sliding along the surface.
        if (closest <= step + 0.05f && slot.Age > 0.6f && direction.Dot(normal) < -0.6f)
            return true;

        const float into = slot.Velocity.Dot(normal);
        if (into < 0.f)
            slot.Velocity = slot.Velocity - normal * into;
        const float urgency = 1.f - std::clamp(closest / lookAhead, 0.f, 1.f);
        slot.Velocity = slot.Velocity + normal * (speed * (1.f + 5.f * urgency) * dt);
        return false;
    }

    // Clear flight line between two points? `slack` skips the last stretch,
    // which is usually the target's own collider.
    static bool DroneLineClear(const Unity::Vector3& from, const Unity::Vector3& to, PlayerAvatar local, float slack)
    {
        const Unity::Vector3 delta = to - from;
        const float distance = delta.GetMagnitude();
        const float length = distance - slack;
        if (length <= 0.05f)
            return true;

        System::Array<Unity::RaycastHit> hits = Unity::Physics::SphereCastAll(from, 0.2f, delta * (1.f / distance), length,
            SemiFunc::LayerMaskGetVisionObstruct());
        if (hits == null)
            return true;

        Unity::Transform avatarRoot = local ? local.GetTransform() : null;
        Unity::Transform playerRoot = local ? local.playerTransform() : null;
        for (Unity::RaycastHit& hit : hits)
        {
            if (hit.m_Distance <= 0.f)
                continue;
            if (FunIgnoreHit(hit, avatarRoot, playerRoot, null))
                continue;
            return false;
        }
        return true;
    }

    static void DroneBuildPath(DroneSlot& slot, const Unity::Vector3& aim)
    {
        slot.PathCount = 0;
        slot.PathIndex = 0;
        slot.PathGoal = aim;

        Unity::NavMeshHit start{}, end{};
        if (!Unity::NavMesh::SamplePosition(slot.Position, &start, 5.f, -1) ||
            !Unity::NavMesh::SamplePosition(aim, &end, 5.f, -1))
            return;

        DroneNavMeshPath path = System::Activator::CreateInstanceDefaultCtor<DroneNavMeshPath>();
        if (path == null || !DroneNavMesh::CalculatePath(start.m_Position, end.m_Position, -1, path))
            return;

        System::Array<Unity::Vector3> corners = path.Corners();
        if (corners == null)
            return;

        // Corner 0 is where we already are. The last one is replaced with the
        // real aim point so the final leg goes for the chest, not the floor.
        const int count = corners.Length();
        for (int i = 1; i < count && slot.PathCount < (int)std::size(slot.Path); ++i)
            slot.Path[slot.PathCount++] = corners[i] + Unity::Vector3::up() * DroneFlightHeight;
        if (slot.PathCount > 0 && count - 1 <= (int)std::size(slot.Path))
            slot.Path[slot.PathCount - 1] = aim;
    }

    // Walls mode: fly straight when the aim is in sight, otherwise follow the
    // level's NavMesh corridor by corridor, cutting corners it can already see past.
    static Unity::Vector3 DroneRoute(DroneSlot& slot, const Unity::Vector3& aim, PlayerAvatar local, float dt)
    {
        slot.LosTimer -= dt;
        slot.PathTimer -= dt;
        const bool refresh = slot.LosTimer <= 0.f;
        if (refresh)
        {
            // Spread the checks of a big swarm over several frames.
            slot.LosTimer = 0.12f + FunRandom01() * 0.06f;
            slot.LosClear = DroneLineClear(slot.Position, aim, local, 1.f);
        }
        if (slot.LosClear)
        {
            slot.PathCount = 0;
            return aim;
        }

        if (slot.PathTimer <= 0.f || slot.PathIndex >= slot.PathCount ||
            Unity::Vector3::Distance(aim, slot.PathGoal) > 2.f)
        {
            slot.PathTimer = 0.6f;
            DroneBuildPath(slot, aim);
            if (slot.PathCount == 0)
            {
                static bool s_NoPathLogged = false;
                if (!s_NoPathLogged)
                {
                    Hax::LogError(G->Logger, L"Drone strike: no NavMesh path, falling back to wall sliding");
                    s_NoPathLogged = true;
                }
            }
        }
        if (slot.PathCount == 0)
            return aim;

        while (slot.PathIndex < slot.PathCount - 1 &&
            Unity::Vector3::Distance(slot.Position, slot.Path[slot.PathIndex]) < 1.2f)
            ++slot.PathIndex;

        if (refresh)
        {
            for (int j = std::min(slot.PathIndex + 3, slot.PathCount - 1); j > slot.PathIndex; --j)
            {
                if (DroneLineClear(slot.Position, slot.Path[j], local, 0.3f))
                {
                    slot.PathIndex = j;
                    break;
                }
            }
        }
        return slot.Path[slot.PathIndex];
    }

    static void DroneSteer(DroneSlot& slot, const Unity::Vector3& goal, PlayerAvatar local, float dt)
    {
        Unity::Vector3 aim = goal;
        // Lead moving targets a little so the drone does not just trail behind them.
        if (slot.HasLastGoal && dt > 0.f)
        {
            Unity::Vector3 goalVelocity = (goal - slot.LastGoal) * (1.f / dt);
            if (goalVelocity.GetMagnitude() < 15.f)
            {
                const float lead = std::clamp(Unity::Vector3::Distance(slot.Position, goal) / std::max(slot.Speed, 1.f), 0.f, 0.8f);
                aim = goal + goalVelocity * lead;
            }
        }
        slot.LastGoal = goal;
        slot.HasLastGoal = true;
        slot.GoalDistance = Unity::Vector3::Distance(slot.Position, goal);

        const Unity::Vector3 steerPoint = slot.ThroughWalls ? aim : DroneRoute(slot, aim, local, dt);
        const bool routed = !slot.ThroughWalls && slot.PathCount > 0 && !slot.LosClear;

        const Unity::Vector3 toAim = steerPoint - slot.Position;
        const float distance = toAim.GetMagnitude();
        const Unity::Vector3 desired = distance > 0.001f ? toAim * (slot.Speed / distance) : Unity::Vector3::zero();
        // Turn harder on the final approach so it dives in instead of orbiting;
        // corridor corners need a quick turn too.
        const float agility = distance < 5.f ? 14.f : (routed ? 9.f : (slot.Manual ? 6.f : 3.5f));
        const float blend = 1.f - std::exp(-agility * dt);
        slot.Velocity = slot.Velocity + (desired - slot.Velocity) * blend;
    }

    static void DroneSpawnWarheads(DroneSlot& slot)
    {
        if (!s_DronePrefabs.Grenade)
            return;
        const int wanted = std::clamp(slot.WarheadsWanted, 1, DroneMaxWarheads);
        for (int w = slot.WarheadCount; w < wanted; ++w)
        {
            DroneWarhead& warhead = slot.Warheads[slot.WarheadCount];
            warhead.Offset = Unity::Vector3(((float)w - (float)(wanted - 1) * 0.5f) * 0.2f, -0.32f, 0.f);
            try
            {
                warhead.Object = SpawnDroneObject(s_DronePrefabs.Grenade, slot.Position + warhead.Offset, Unity::Quaternion::identity());
            }
            catch (System::Exception& ex)
            {
                LogFunException(L"Drone warhead spawn", ex);
                warhead.Object = null;
            }
            if (!warhead.Object)
                break;
            warhead.Body = PrepareDroneBody(warhead.Object);
            warhead.Grenade = DroneFindComponent<ItemGrenade>(warhead.Object);
            ++slot.WarheadCount;
        }
    }

    static void DroneArm(DroneSlot& slot)
    {
        if (slot.Phase == DronePhase::Launch || slot.Phase == DronePhase::Cruise || slot.Phase == DronePhase::Loiter)
        {
            slot.Phase = DronePhase::Arming;
            slot.PhaseTime = 0.f;
            DroneSpawnWarheads(slot);
        }
    }

    // Detonate grenades through the game. A detonated grenade destroys itself;
    // touching it again (a second PhotonNetwork.Destroy) crashes Unity, so it is
    // forgotten right away. Duds are removed now, while they are known to be alive.
    static int DetonateGrenade(Unity::GameObject& object, ItemGrenade grenade)
    {
        bool detonated = false;
        if (object && grenade)
        {
            try
            {
                grenade.isSpawnedGrenade() = true; // destroy after the blast, no inventory bookkeeping
                s_FunReleasing = true;
                grenade.TickEnd();
                s_FunReleasing = false;
                detonated = true;
            }
            catch (System::Exception& ex)
            {
                s_FunReleasing = false;
                LogFunException(L"Grenade detonation", ex);
            }
        }
        if (detonated)
            object = null;
        else
            DestroyDroneObject(object);
        return detonated ? 1 : 0;
    }

    static void DroneExplode(DroneSlot& slot)
    {
        int fired = 0;
        for (int i = 0; i < slot.WarheadCount; ++i)
        {
            DroneWarhead& warhead = slot.Warheads[i];
            fired += DetonateGrenade(warhead.Object, warhead.Grenade);
            warhead = {};
        }
        slot.WarheadCount = 0;

        DestroyDroneObject(slot.Object);
        slot.Body = null;

        if (slot.Lethal)
        {
            if (slot.HitPlayer && !slot.HitPlayer.deadSet())
            {
                s_ForcedPlayerDeath = true;
                slot.HitPlayer.PlayerDeath(-1);
                s_ForcedPlayerDeath = false;
            }
            else if (slot.HitEnemy && slot.HitEnemy.enemy())
            {
                if (EnemyHealth health = slot.HitEnemy.enemy().Health(); health && !health.dead())
                    health.Hurt(999999, slot.Velocity.GetNormalized());
            }
        }

        if (fired == 0)
        {
            static bool s_DudLogged = false;
            if (!s_DudLogged)
            {
                Hax::LogError(G->Logger, L"Drone strike: no warhead went off (out of network IDs?)");
                s_DudLogged = true;
            }
        }

        slot.Phase = DronePhase::Done;
        slot.PhaseTime = 0.f;
    }

    static void ResetDroneSlot(DroneSlot& slot, bool destroyObjects)
    {
        if (destroyObjects)
        {
            DestroyDroneObject(slot.Object);
            for (DroneWarhead& warhead : slot.Warheads)
                DestroyDroneObject(warhead.Object);
        }
        slot = {};
    }

    // Lost the target: pick the nearest living one of the same kind (enemies for
    // enemy hunters, other players for player hunters, never the host).
    static bool DroneRetargetSlot(DroneSlot& slot, PlayerAvatar local)
    {
        const bool huntPlayers = (bool)slot.TargetPlayer;
        EnsureFrameTargets();
        float best = FLT_MAX;
        const FunFrameTarget* pick = nullptr;
        const std::vector<FunFrameTarget>& pool = huntPlayers ? s_FramePlayers : s_FrameEnemies;
        for (const FunFrameTarget& entry : pool)
        {
            if (entry.Player && entry.Player == local)
                continue;
            const float distance = Unity::Vector3::Distance(slot.Position, entry.Point);
            if (distance < best)
            {
                best = distance;
                pick = &entry;
            }
        }
        if (!pick)
            return false;
        slot.TargetPlayer = pick->Player;
        slot.TargetEnemy = pick->Enemy;
        slot.HasLastGoal = false;
        slot.PathCount = 0;
        return true;
    }

    static DroneLaunchOrder MakeDroneOrderFromSettings()
    {
        DroneLaunchOrder order;
        order.Total = std::clamp(G->DroneSwarmSize, 1, DroneMaxSwarm);
        order.Warheads = std::clamp(G->DroneWarheads, 1, DroneMaxWarheads);
        order.Speed = (float)std::clamp(G->DroneSpeed, 4, 40);
        order.Manual = G->DroneManualMode;
        order.Lethal = G->DroneLethal;
        order.ThroughWalls = G->DroneThroughWalls;
        order.Retarget = G->DroneRetarget;
        return order;
    }

    static bool FunHostReady()
    {
        return G->IsInGame && !G->IsClient;
    }

    static bool EnsureDronePrefabsOrReport()
    {
        if (FindDronePrefabs())
        {
            G->DronePrefabsMissing = false;
            return true;
        }
        G->DronePrefabsMissing = true;
        SetSessionAction(G->Loc[LocKey_ActionDronePrefabMissing]);
        return false;
    }

    static void LaunchDrones()
    {
        if (!FunHostReady() || !EnsureDronePrefabsOrReport())
            return;

        DroneLaunchOrder order = MakeDroneOrderFromSettings();
        if (!order.Manual)
        {
            Unity::Vector3 unused;
            if (G->DroneTargetPlayer && FunFindPlayerPoint(G->DroneTargetPlayer, unused))
                order.TargetPlayer = G->DroneTargetPlayer;
            else if (G->DroneTargetEnemy && FunFindEnemyPoint(G->DroneTargetEnemy, unused))
                order.TargetEnemy = G->DroneTargetEnemy;
            else
            {
                SetSessionAction(G->Loc[LocKey_ActionDroneNoTarget]);
                return;
            }
        }
        // Every press adds another wave, so spamming the key keeps the sky busy.
        s_DroneOrders.push_back(order);
    }

    // Small swarms fan out in front of you; big ones form rings around you
    // that grow wider and higher with every ring.
    static void DroneSwarmLayout(int index, int total, const Unity::Vector3& origin, const Unity::Vector3& forward,
        const Unity::Vector3& right, Unity::Vector3& spawn, Unity::Vector3& launchDir)
    {
        const Unity::Vector3 up = Unity::Vector3::up();
        if (total <= 5)
        {
            const float spread = ((float)index - (float)(total - 1) * 0.5f) * 0.9f;
            spawn = origin + forward * 1.6f + right * spread + up * 0.35f;
            launchDir = (forward + up * 0.45f + right * (spread * 0.25f)).GetNormalized();
            return;
        }

        constexpr int perRing = 8;
        const int ring = index / perRing;
        const float angle = ((float)(index % perRing) / (float)perRing) * 6.2831853f + (float)ring * 0.39f;
        const Unity::Vector3 outward = forward * std::cos(angle) + right * std::sin(angle);
        spawn = origin + outward * (1.6f + (float)(ring % 6) * 0.8f) + up * (0.35f + (float)(ring % 6) * 0.3f);
        launchDir = (outward * 0.6f + forward * 0.4f + up * 0.7f).GetNormalized();
    }

    static DroneSlot* AcquireDroneSlot()
    {
        for (DroneSlot& slot : s_Drones)
            if (slot.Phase == DronePhase::Idle)
                return &slot;
        s_Drones.emplace_back();
        return &s_Drones.back();
    }

    static void SpawnQueuedDrones(uint64_t now)
    {
        if (s_DroneOrders.empty() || now < s_DroneSpawnBlockedUntil)
            return;

        Unity::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        Unity::Transform camTransform = camera.GetTransform();
        const Unity::Vector3 forward = camTransform.GetForward().GetNormalized();
        const Unity::Vector3 right = (camTransform.GetRotation() * Unity::Vector3::right()).GetNormalized();
        const Unity::Vector3 origin = camTransform.GetPosition();

        int spawned = 0;
        while (spawned < DroneSpawnPerFrame && !s_DroneOrders.empty())
        {
            DroneLaunchOrder& order = s_DroneOrders.front();
            if (order.Next >= order.Total)
            {
                if (order.Launched > 0 && !order.Quiet)
                    SetSessionActionF(G->Loc[LocKey_ActionDroneLaunchedFmt].Data(), order.Launched);
                s_DroneOrders.pop_front();
                continue;
            }

            const int index = order.Next;
            Unity::Vector3 spawn, launchDir;
            DroneSwarmLayout(index, order.Total, origin, forward, right, spawn, launchDir);
            if (!order.ThroughWalls)
            {
                // Indoors a steep climb just hits the ceiling.
                launchDir.Y *= 0.2f;
                launchDir = launchDir.GetNormalized();
            }

            Unity::GameObject object = null;
            try
            {
                object = SpawnDroneObject(s_DronePrefabs.Drone, spawn, DroneRotation(launchDir));
            }
            catch (System::Exception& ex)
            {
                // Out of Photon room view IDs: wait for blasts to free some.
                static bool s_FullLogged = false;
                if (!s_FullLogged)
                {
                    LogFunException(L"Drone spawn (queue paused)", ex);
                    s_FullLogged = true;
                }
                object = null;
            }
            if (!object)
            {
                s_DroneSpawnBlockedUntil = now + DroneSpawnRetryMs;
                break;
            }

            ++order.Next;
            DroneSlot* slot = AcquireDroneSlot();
            *slot = {};
            slot->Serial = ++s_DroneSerial;
            slot->Object = object;
            slot->Body = PrepareDroneBody(object);
            slot->Position = spawn;
            slot->LaunchDir = launchDir;
            slot->Speed = order.Speed;
            slot->Velocity = launchDir * (order.Speed * 0.5f);
            slot->Manual = order.Manual;
            slot->Lethal = order.Lethal;
            slot->ThroughWalls = order.ThroughWalls;
            slot->Retarget = order.Retarget;
            slot->WarheadsWanted = order.Warheads;
            slot->TargetPlayer = order.TargetPlayer;
            slot->TargetEnemy = order.TargetEnemy;
            slot->Bob = (float)index * 1.7f;
            slot->Phase = DronePhase::Launch;
            ++order.Launched;
            ++spawned;
        }
    }

    // ---------------------------------------------------------------- FPV
    static uint32_t s_FpvSerial;
    static uint64_t s_FpvLostAt;
    static bool s_FpvHasLast;
    static Unity::Vector3 s_FpvLastPos{};
    static Unity::Quaternion s_FpvLastRot = Unity::Quaternion::identity();

    struct FpvCameraBackup
    {
        bool Valid = false;
        Unity::Camera Camera = null;
        Unity::Transform Transform = null;
        Unity::Vector3 LocalPosition{};
        Unity::Quaternion Rotation = Unity::Quaternion::identity();
        float FieldOfView = 60.f;
    };
    static FpvCameraBackup s_FpvBackup;

    static bool IsDroneFlying(const DroneSlot& slot)
    {
        return slot.Phase == DronePhase::Launch || slot.Phase == DronePhase::Cruise ||
            slot.Phase == DronePhase::Loiter || slot.Phase == DronePhase::Arming;
    }

    static DroneSlot* FindDroneBySerial(uint32_t serial)
    {
        if (serial == 0)
            return nullptr;
        for (DroneSlot& slot : s_Drones)
            if (slot.Serial == serial && IsDroneFlying(slot))
                return &slot;
        return nullptr;
    }

    // Which drone the FPV feed shows: the current one until it blows up, then a
    // short "signal lost" static, then the newest drone still in the air.
    static void UpdateFpvSelection(uint64_t now)
    {
        if (!G->DroneFpvEnabled || !FunHostReady())
        {
            s_FpvSerial = 0;
            s_FpvLostAt = 0;
            s_FpvHasLast = false;
            G->FpvActive = false;
            G->FpvPiloting = false;
            G->FpvSignalLost = false;
            return;
        }

        if (s_FpvSerial && !FindDroneBySerial(s_FpvSerial))
        {
            s_FpvSerial = 0;
            if (s_FpvHasLast)
                s_FpvLostAt = now;
        }

        if (s_FpvLostAt && now - s_FpvLostAt < FpvLostScreenMs)
        {
            G->FpvSignalLost = true;
            G->FpvActive = true;
            G->FpvPiloting = false;
            return;
        }
        s_FpvLostAt = 0;
        G->FpvSignalLost = false;

        if (!s_FpvSerial)
        {
            for (const DroneSlot& slot : s_Drones)
                if (IsDroneFlying(slot) && slot.Phase != DronePhase::Arming && slot.Serial > s_FpvSerial)
                    s_FpvSerial = slot.Serial;
        }

        DroneSlot* watched = FindDroneBySerial(s_FpvSerial);
        G->FpvActive = watched != nullptr;
        G->FpvPiloting = watched && watched->Manual && !G->MenuVisible;
        if (!watched)
            s_FpvHasLast = false;
    }

    static void ApplyDroneFpvCamera(PlayerAvatar local)
    {
        if (!G->FpvActive)
            return;

        Unity::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;
        Unity::Transform transform = camera.GetTransform();
        if (!transform)
            return;

        Unity::Vector3 position = s_FpvLastPos;
        Unity::Quaternion rotation = s_FpvLastRot;
        if (!G->FpvSignalLost)
        {
            DroneSlot* slot = FindDroneBySerial(s_FpvSerial);
            if (!slot)
                return;

            Unity::Vector3 forward;
            if (G->FpvPiloting)
            {
                // The pilot looks around with the mouse as usual; the drone flies there.
                rotation = transform.GetRotation();
                forward = transform.GetForward().GetNormalized();
            }
            else
            {
                const float speed = slot->Velocity.GetMagnitude();
                forward = speed > 0.5f ? slot->Velocity * (1.f / speed) : (s_FpvLastRot * Unity::Vector3::forward());
                rotation = FunLookRotation(forward);
            }
            position = slot->Position + forward * 0.4f + Unity::Vector3::up() * 0.06f;

            const float pilotDistance = local ? Unity::Vector3::Distance(slot->Position, DronePlayerPoint(local)) : 0.f;
            G->FpvSpeed = slot->Velocity.GetMagnitude();
            G->FpvDistance = slot->GoalDistance;
            G->FpvRange = pilotDistance;
            G->FpvSignal = std::clamp(1.f - pilotDistance / 140.f, 0.08f, 1.f);
            G->FpvFlightTime = slot->Age;
            G->FpvArming = slot->Phase == DronePhase::Arming;
            s_FpvLastPos = position;
            s_FpvLastRot = rotation;
            s_FpvHasLast = true;
        }

        // Only for this frame's render: Hooked__PresentAfterDrawLoop puts the
        // camera back, so the game's own logic never sees the drone camera.
        s_FpvBackup = {true, camera, transform, transform.GetLocalPosition(), transform.GetRotation(), camera.GetFieldOfView()};
        transform.SetPosition(position);
        transform.SetRotation(rotation);
        camera.SetFieldOfView(100.f);
    }

    static void RestoreDroneFpvCamera()
    {
        if (!s_FpvBackup.Valid)
            return;
        s_FpvBackup.Valid = false;
        try
        {
            if (s_FpvBackup.Transform)
            {
                s_FpvBackup.Transform.SetLocalPosition(s_FpvBackup.LocalPosition);
                s_FpvBackup.Transform.SetRotation(s_FpvBackup.Rotation);
            }
            if (s_FpvBackup.Camera)
                s_FpvBackup.Camera.SetFieldOfView(s_FpvBackup.FieldOfView);
        }
        catch (System::Exception& ex)
        {
            LogFunException(L"FPV restore", ex);
        }
        s_FpvBackup.Transform = null;
        s_FpvBackup.Camera = null;
    }

    static void UpdateDroneSlot(DroneSlot& slot, PlayerAvatar local, const Unity::Vector3& crosshair, bool hasCrosshair,
        const Unity::Vector3& pilotForward, float dt)
    {
        slot.Age += dt;
        slot.PhaseTime += dt;

        if (slot.Phase == DronePhase::Done)
        {
            if (slot.PhaseTime >= DroneCleanupDelay)
                slot = {};
            return;
        }

        // Somebody deleted or broke the drone mid-flight: blow up where it was.
        if (!slot.Object && slot.Phase != DronePhase::Arming)
            DroneArm(slot);

        // Turn the drone on once it has run Start(): unfolds the rotors and starts
        // the buzzing loop on every client. Its own target search stays disabled.
        if (!slot.Activated && slot.Object && slot.Age >= 0.2f)
        {
            slot.Activated = true;
            if (ItemDrone drone = DroneFindComponent<ItemDrone>(slot.Object))
            {
                drone.targetValuables() = false;
                drone.targetPlayers() = false;
                drone.targetEnemies() = false;
                drone.targetNonValuables() = false;
            }
            if (ItemToggle toggle = DroneFindComponent<ItemToggle>(slot.Object))
                toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
        }

        const bool piloted = G->FpvPiloting && slot.Serial == s_FpvSerial;

        switch (slot.Phase)
        {
        case DronePhase::Launch:
        {
            const Unity::Vector3 desired = slot.LaunchDir * (slot.Speed * 0.6f);
            slot.Velocity = slot.Velocity + (desired - slot.Velocity) * (1.f - std::exp(-6.f * dt));
            if (slot.PhaseTime >= DroneLaunchTime || piloted)
            {
                slot.Phase = DronePhase::Cruise;
                slot.PhaseTime = 0.f;
            }
            break;
        }
        case DronePhase::Cruise:
        {
            if (piloted)
            {
                // W boosts, S brakes; the nose follows the mouse.
                float throttle = 1.f;
                if (Hax::Gui::IsKeyDown('W')) throttle = 1.8f;
                if (Hax::Gui::IsKeyDown('S')) throttle = 0.3f;
                const Unity::Vector3 desired = pilotForward * (slot.Speed * throttle);
                slot.Velocity = slot.Velocity + (desired - slot.Velocity) * (1.f - std::exp(-7.f * dt));
                slot.GoalDistance = hasCrosshair ? Unity::Vector3::Distance(slot.Position, crosshair) : 0.f;
                break;
            }

            Unity::Vector3 goal;
            bool haveGoal = false;
            if (slot.Manual)
            {
                goal = crosshair;
                haveGoal = hasCrosshair;
            }
            else
                haveGoal = FunTargetPoint(slot.TargetPlayer, slot.TargetEnemy, goal);

            if (haveGoal)
            {
                slot.LostTime = 0.f;
                DroneSteer(slot, goal, local, dt);
                // A manual drone that reaches the crosshair hits whatever is there.
                if (slot.Manual && Unity::Vector3::Distance(slot.Position, goal) < 0.7f)
                    DroneArm(slot);
            }
            else
            {
                slot.LostTime += dt;
                slot.Velocity = slot.Velocity * std::exp(-4.f * dt);
                if (slot.LostTime >= DroneLostTargetGrace && !(slot.Retarget && DroneRetargetSlot(slot, local)))
                {
                    // Nobody left to hunt: circle here and wait.
                    slot.Phase = DronePhase::Loiter;
                    slot.PhaseTime = 0.f;
                    slot.LoiterCenter = slot.Position;
                }
                if (slot.LostTime >= DroneLostTargetGrace)
                    slot.LostTime = 0.f;
            }
            break;
        }
        case DronePhase::Loiter:
        {
            const float angle = slot.Age * 1.1f + slot.Bob;
            const Unity::Vector3 goal = slot.LoiterCenter + Unity::Vector3(std::cos(angle), 0.f, std::sin(angle)) * 2.f;
            const Unity::Vector3 toGoal = goal - slot.Position;
            slot.Velocity = slot.Velocity + (toGoal * 2.5f - slot.Velocity) * (1.f - std::exp(-3.f * dt));

            slot.RetargetTimer -= dt;
            if (slot.RetargetTimer <= 0.f)
            {
                slot.RetargetTimer = 0.5f + FunRandom01() * 0.3f;
                if (slot.Retarget && DroneRetargetSlot(slot, local))
                {
                    slot.Phase = DronePhase::Cruise;
                    slot.PhaseTime = 0.f;
                }
            }
            break;
        }
        case DronePhase::Arming:
            slot.Velocity = slot.Velocity * std::exp(-14.f * dt);
            break;
        default:
            break;
        }

        const float step = slot.Velocity.GetMagnitude() * dt;
        if (slot.Phase != DronePhase::Arming && !slot.ThroughWalls && DroneAvoidWalls(slot, local, step, dt))
        {
            slot.HitPlayer = null;
            slot.HitEnemy = null;
            DroneArm(slot);
        }

        slot.Position = slot.Position + slot.Velocity * dt;

        if (slot.Phase == DronePhase::Cruise)
        {
            const bool manualRules = slot.Manual || piloted;
            const bool savedManual = slot.Manual;
            slot.Manual = manualRules;
            if (DroneCheckFuse(slot, local))
                DroneArm(slot);
            slot.Manual = savedManual;
        }

        // Small hover bob so it reads as a drone, not a thrown brick.
        slot.Bob += dt;
        const Unity::Vector3 shown = slot.Position + Unity::Vector3::up() * (std::sin(slot.Bob * 7.f) * 0.06f);
        const Unity::Quaternion rotation = DroneRotation(slot.Velocity);
        // Rotation only goes out when the heading really changed: one fewer call per
        // drone per frame adds up in big swarms.
        constexpr float degrees = 57.2957795131f;
        const float yaw = slot.Velocity.GetMagnitude() > 0.1f ? std::atan2(slot.Velocity.X, slot.Velocity.Z) * degrees : slot.LastYaw;
        const bool turn = std::fabs(yaw - slot.LastYaw) > 2.f;
        if (turn)
            slot.LastYaw = yaw;
        DriveDroneBody(slot.Object, slot.Body, shown, slot.Velocity, turn ? &rotation : nullptr);
        for (int i = 0; i < slot.WarheadCount; ++i)
        {
            DroneWarhead& warhead = slot.Warheads[i];
            DriveDroneBody(warhead.Object, warhead.Body, shown + rotation * warhead.Offset, slot.Velocity, nullptr);
        }

        if (slot.Phase == DronePhase::Arming && slot.PhaseTime >= DroneArmTime)
            DroneExplode(slot);
    }

    static void ResetAllDrones()
    {
        s_Drones.clear();
        s_DroneOrders.clear();
        s_DroneSpawnBlockedUntil = 0;
        G->DronesInFlight = 0;
        G->DronesQueued = 0;
    }

    static void UpdateDroneStrikes(PlayerAvatar local, uint64_t now, float dt)
    {
        if (G->DroneLaunchRequested)
        {
            G->DroneLaunchRequested = false;
            LaunchDrones();
        }
        SpawnQueuedDrones(now);

        if (G->DroneDetonateRequested)
        {
            G->DroneDetonateRequested = false;
            s_DroneOrders.clear(); // stop any wave still coming
            for (DroneSlot& slot : s_Drones)
            {
                slot.HitPlayer = null;
                slot.HitEnemy = null;
                DroneArm(slot);
            }
        }

        int inFlight = 0;
        int queued = 0;
        bool anyBusy = false;
        bool needCrosshair = G->FpvPiloting;
        for (const DroneSlot& slot : s_Drones)
        {
            if (IsDroneFlying(slot))
                ++inFlight;
            anyBusy |= slot.Phase != DronePhase::Idle;
            needCrosshair |= slot.Manual && slot.Phase == DronePhase::Cruise;
        }
        for (const DroneLaunchOrder& order : s_DroneOrders)
            queued += order.Total - order.Next;
        G->DronesInFlight = inFlight;
        G->DronesQueued = queued;

        UpdateFpvSelection(now);
        if (!anyBusy)
            return;

        Unity::Vector3 crosshair{};
        Unity::Vector3 pilotForward = Unity::Vector3::forward();
        bool hasCrosshair = false;
        if (Unity::Camera camera = SemiFunc::MainCamera())
        {
            pilotForward = camera.GetTransform().GetForward().GetNormalized();
            if (needCrosshair)
            {
                crosshair = DroneCrosshairPoint(camera, local);
                hasCrosshair = true;
            }
        }

        for (DroneSlot& slot : s_Drones)
        {
            if (slot.Phase == DronePhase::Idle)
                continue;
            try
            {
                UpdateDroneSlot(slot, local, crosshair, hasCrosshair, pilotForward, dt);
            }
            catch (System::Exception& ex)
            {
                LogFunException(L"Drone strike slot", ex);
                ResetDroneSlot(slot, true);
            }
        }

        // Keep the pool from growing forever after a huge swarm is gone.
        while (!s_Drones.empty() && s_Drones.back().Phase == DronePhase::Idle)
            s_Drones.pop_back();
    }

    // ---------------------------------------------------------------- Orbital strike
    // A Photon Blaster beam (the same stock item the eye lasers use) burns down
    // from the ceiling onto the mark, then a column of grenades goes off from the
    // top down and a ring of them closes the show at ground level.

    constexpr float OrbitalLockTime = 2.2f;
    constexpr float OrbitalSpawnTime = 2.3f;
    constexpr float OrbitalFireTime = 2.65f;
    constexpr float OrbitalShellGap = 0.06f;
    constexpr int   OrbitalMaxShells = 24;

    struct OrbitalShell
    {
        Unity::GameObject Object = null;
        Unity::Rigidbody Body = null;
        ItemGrenade Grenade = null;
        Unity::Vector3 Point{};
        float FireAt = 0.f;
        bool Done = false;
    };

    struct OrbitalStrike
    {
        float Time = 0.f;
        Unity::Vector3 Ground{};
        Unity::Vector3 Top{};
        PlayerAvatar TrackPlayer = null;
        EnemyParent TrackEnemy = null;
        NetworkEyeLaserCarrier Laser{};
        bool LaserFailed = false;
        OrbitalShell Shells[OrbitalMaxShells]{};
        int ShellCount = 0;
        bool ShellsSpawned = false;
        float EndTime = 0.f;
    };

    static std::vector<OrbitalStrike> s_Orbitals;

    static Unity::Vector3 FunGroundOfTarget(PlayerAvatar player, EnemyParent enemy, const Unity::Vector3& point)
    {
        if (player)
        {
            if (Unity::Transform root = GetPlayerTransformSafe(player))
                return root.GetPosition();
        }
        return FunFloorBelow(point, enemy ? enemy.GetTransform() : null);
    }

    static bool CreateOrbitalLaser(NetworkEyeLaserCarrier& carrier, const Unity::Vector3& position)
    {
        if (!s_EyeLaserPrefab.NetworkItem && !G->ItemsPool.Empty())
            s_EyeLaserPrefab.Template = FindEyeLaserTemplate();
        if (!s_EyeLaserPrefab.NetworkItem || s_EyeLaserPrefab.NetworkItem.prefab() == null)
            return false;

        carrier.Object = SpawnDroneObject(s_EyeLaserPrefab.NetworkItem, position, Unity::Quaternion::identity());
        if (!carrier.Object)
            return false;

        carrier.Phys = DroneFindComponent<PhysGrabObject>(carrier.Object);
        carrier.Gun = DroneFindComponent<ItemGun>(carrier.Object);
        carrier.GunLaser = DroneFindComponent<ItemGunLaser>(carrier.Object);
        if (!carrier.Phys || !carrier.Gun || !carrier.GunLaser || !carrier.GunLaser.muzzleTransform())
        {
            DestroyDroneObject(carrier.Object);
            carrier = {};
            return false;
        }

        carrier.Battery = carrier.Gun.itemBattery();
        Unity::Transform root = carrier.Object.GetTransform();
        Unity::Transform muzzle = carrier.GunLaser.muzzleTransform();
        carrier.MuzzleOffset = muzzle.GetPosition() - root.GetPosition();
        carrier.MuzzleRotationOffset = muzzle.GetRotation();
        carrier.ReadyAt = ::GetTickCount64() + 400;
        PrepareDroneBody(carrier.Object);
        return true;
    }

    static void DestroyOrbitalLaser(OrbitalStrike& strike)
    {
        DestroyDroneObject(strike.Laser.Object);
        strike.Laser = {};
    }

    static void StartOrbitalStrike(const Unity::Vector3& ground, PlayerAvatar trackPlayer, EnemyParent trackEnemy)
    {
        if (!FunHostReady() || !EnsureDronePrefabsOrReport())
            return;
        OrbitalStrike strike;
        strike.Ground = ground;
        strike.Top = FunCeilingAbove(ground);
        strike.TrackPlayer = trackPlayer;
        strike.TrackEnemy = trackEnemy;
        s_Orbitals.push_back(strike);
        SetSessionAction(G->Loc[LocKey_ActionOrbitalIncoming]);
    }

    static void SpawnOrbitalShells(OrbitalStrike& strike)
    {
        strike.ShellsSpawned = true;
        const int total = std::clamp(G->OrbitalPower, 3, OrbitalMaxShells);
        const int ring = total / 3;
        const int column = total - ring;
        const Unity::Vector3 up = Unity::Vector3::up();
        const float height = std::max(0.5f, strike.Top.Y - strike.Ground.Y - 0.3f);

        for (int i = 0; i < total; ++i)
        {
            OrbitalShell& shell = strike.Shells[strike.ShellCount];
            if (i < column)
            {
                const float t = column > 1 ? (float)i / (float)(column - 1) : 1.f;
                shell.Point = strike.Ground + up * (height - (height - 0.5f) * t);
                shell.FireAt = OrbitalFireTime + OrbitalShellGap * (float)i;
            }
            else
            {
                const float angle = (float)(i - column) / (float)std::max(ring, 1) * 6.2831853f;
                shell.Point = strike.Ground + Unity::Vector3(std::cos(angle), 0.f, std::sin(angle)) * 2.2f + up * 0.4f;
                shell.FireAt = OrbitalFireTime + OrbitalShellGap * (float)column + 0.1f;
            }

            try
            {
                shell.Object = SpawnDroneObject(s_DronePrefabs.Grenade, shell.Point, Unity::Quaternion::identity());
            }
            catch (System::Exception& ex)
            {
                LogFunException(L"Orbital shell spawn", ex);
                shell.Object = null;
            }
            if (!shell.Object)
                break;
            shell.Body = PrepareDroneBody(shell.Object);
            shell.Grenade = DroneFindComponent<ItemGrenade>(shell.Object);
            ++strike.ShellCount;
        }
        strike.EndTime = OrbitalFireTime + OrbitalShellGap * (float)total + 1.f;
    }

    static bool UpdateOrbitalStrike(OrbitalStrike& strike, PlayerAvatar local, float dt)
    {
        strike.Time += dt;

        // Follow a moving target until the lock, then the mark is final.
        if (strike.Time < OrbitalLockTime && (strike.TrackPlayer || strike.TrackEnemy))
        {
            Unity::Vector3 point;
            if (FunTargetPoint(strike.TrackPlayer, strike.TrackEnemy, point))
            {
                strike.Ground = FunGroundOfTarget(strike.TrackPlayer, strike.TrackEnemy, point);
                strike.Top.X = strike.Ground.X;
                strike.Top.Z = strike.Ground.Z;
            }
        }

        if (strike.Time < OrbitalFireTime && !strike.LaserFailed)
        {
            try
            {
                if (!strike.Laser.Object && !CreateOrbitalLaser(strike.Laser, strike.Top))
                    strike.LaserFailed = true;
                if (strike.Laser.Object)
                {
                    EyeLaserProtocol::Settings settings;
                    settings.Range = (int)std::ceil(strike.Top.Y - strike.Ground.Y) + 3;
                    settings.Damage = 40;
                    settings.InstantKill = false;
                    settings.DestroyObjects = true;
                    UpdateNetworkEyeLaser(strike.Laser, strike.Top, Unity::Quaternion::Euler(90.f, 0.f, 0.f), local, settings);
                }
            }
            catch (System::Exception& ex)
            {
                LogFunException(L"Orbital laser", ex);
                strike.LaserFailed = true;
                DestroyOrbitalLaser(strike);
            }
        }
        else if (strike.Laser.Object)
            DestroyOrbitalLaser(strike);

        if (!strike.ShellsSpawned && strike.Time >= OrbitalSpawnTime)
            SpawnOrbitalShells(strike);

        for (int i = 0; i < strike.ShellCount; ++i)
        {
            OrbitalShell& shell = strike.Shells[i];
            if (shell.Done)
                continue;
            if (strike.Time >= shell.FireAt)
            {
                DetonateGrenade(shell.Object, shell.Grenade);
                shell = {};
                shell.Done = true;
                continue;
            }
            DriveDroneBody(shell.Object, shell.Body, shell.Point, Unity::Vector3::zero(), nullptr);
        }

        return !(strike.ShellsSpawned && strike.Time >= strike.EndTime);
    }

    static void UpdateOrbitalStrikes(PlayerAvatar local, float dt)
    {
        if (G->OrbitalRequested)
        {
            G->OrbitalRequested = false;
            if (Unity::Camera camera = SemiFunc::MainCamera())
                StartOrbitalStrike(FunFloorBelow(DroneCrosshairPoint(camera, local) - camera.GetTransform().GetForward() * 0.3f),
                    null, null);
        }
        if (G->OrbitalAtTargetRequested)
        {
            G->OrbitalAtTargetRequested = false;
            Unity::Vector3 point;
            if (FunTargetPoint(G->DroneTargetPlayer, G->DroneTargetEnemy, point))
                StartOrbitalStrike(FunGroundOfTarget(G->DroneTargetPlayer, G->DroneTargetEnemy, point),
                    G->DroneTargetPlayer, G->DroneTargetEnemy);
            else
                SetSessionAction(G->Loc[LocKey_ActionDroneNoTarget]);
        }

        for (size_t i = 0; i < s_Orbitals.size();)
        {
            bool keep = false;
            try
            {
                keep = UpdateOrbitalStrike(s_Orbitals[i], local, dt);
            }
            catch (System::Exception& ex)
            {
                LogFunException(L"Orbital strike", ex);
            }
            if (keep)
            {
                ++i;
                continue;
            }
            OrbitalStrike& strike = s_Orbitals[i];
            DestroyOrbitalLaser(strike);
            for (int s = 0; s < strike.ShellCount; ++s)
                DestroyDroneObject(strike.Shells[s].Object);
            s_Orbitals.erase(s_Orbitals.begin() + (ptrdiff_t)i);
        }
        G->OrbitalsActive = (int)s_Orbitals.size();
    }

    // ---------------------------------------------------------------- Black hole
    // Every physics object in range (loot, items, enemy bodies) gets sucked into a
    // spiral; players are knocked over and dragged in with the game's own tumble
    // force RPC. At the end the core grenades go off and everything is flung out.

    // Look: a black core with a purple halo, a tilted glowing accretion disc,
    // spiral arms being sucked in and a pulsing light. These are local engine
    // objects (only the host sees them); everybody sees the networked part: a
    // ring of glowing orbs spiralling in, everything in range flying around and
    // the final ring of blasts.

    constexpr int   BlackHoleOrbCount = 6;
    constexpr int   BlackHoleFinaleShells = 6;
    constexpr int   BlackHoleRingCount = 3;
    constexpr int   BlackHoleArmCount = 6;
    constexpr int   BlackHoleRingPoints = 48;
    constexpr int   BlackHoleArmPoints = 28;
    constexpr float BlackHolePullInterval = 0.08f;
    constexpr float BlackHoleImplosionTime = 0.6f;   // everything rushes in before the bang
    constexpr float BlackHoleFinaleSpawnLead = 0.45f; // finale grenades need Start() before TickEnd
    constexpr float BlackHoleFinaleTime = 0.8f;      // shockwave + flash after the bang

    struct BlackHoleVisuals
    {
        Unity::GameObject Root = null;
        Unity::GameObject Core = null;
        Unity::GameObject Halo = null;
        Unity::LineRenderer Rings[BlackHoleRingCount]{};
        Unity::LineRenderer Arms[BlackHoleArmCount]{};
        Unity::LineRenderer Shock{};
        FunLight Light{};
        Unity::Material Materials[16]{};
        int MaterialCount = 0;
    };

    struct BlackHoleState
    {
        bool Active = false;
        bool Finale = false;
        bool FinaleSpawned = false;
        float Time = 0.f;
        float FinaleTime = 0.f;
        float Duration = 6.f;
        float Radius = 14.f;
        float Strength = 12.f;
        bool IncludeSelf = false;
        Unity::Vector3 Center{};
        float PullTimer = 0.f;
        float PullDt = 0.f;
        float PlayerTimer = 0.f;
        float ShakeTimer = 0.f;
        Unity::GameObject Core[2]{null, null};
        Unity::Rigidbody CoreBodies[2]{null, null};
        ItemGrenade CoreGrenades[2]{null, null};
        Unity::GameObject Shells[BlackHoleFinaleShells]{};
        Unity::Rigidbody ShellBodies[BlackHoleFinaleShells]{};
        ItemGrenade ShellGrenades[BlackHoleFinaleShells]{};
        Unity::GameObject Orbs[BlackHoleOrbCount]{};
        Unity::Rigidbody OrbBodies[BlackHoleOrbCount]{};
        BlackHoleVisuals Visuals{};
    };

    static BlackHoleState s_BlackHole;
    static Item s_BlackHoleOrbItem = null;
    static bool s_BlackHoleOrbSearched;

    static bool BlackHoleOwns(Unity::Transform transform)
    {
        BlackHoleState& hole = s_BlackHole;
        auto owns = [&](Unity::GameObject object) { return object && IsTransformBelow(transform, object.GetTransform()); };
        for (Unity::GameObject object : hole.Core)
            if (owns(object))
                return true;
        for (Unity::GameObject object : hole.Shells)
            if (owns(object))
                return true;
        for (Unity::GameObject object : hole.Orbs)
            if (owns(object))
                return true;
        return false;
    }

    // Any glowing orb item works as a "planet" circling the hole.
    static Item FindBlackHoleOrbItem()
    {
        if (s_BlackHoleOrbSearched || G->ItemsPool.Empty())
            return s_BlackHoleOrbItem;
        s_BlackHoleOrbSearched = true;
        for (const auto& entry : G->ItemsPool)
        {
            if (!DroneNameContains(entry.key, L"orb"))
                continue;
            Item item = entry.value;
            if (item && item.prefab() != null && item.prefab().Prefab())
            {
                s_BlackHoleOrbItem = item;
                Hax::Log(G->Logger, L"Black hole: orbiting %ls", item.itemName().GetRawStringData());
                break;
            }
        }
        return s_BlackHoleOrbItem;
    }

    // ----- local visuals
    static Unity::Material MakeBlackHoleMaterial(BlackHoleVisuals& visuals, const Unity::Color& color)
    {
        static Unity::Shader s_Shader = null;
        static bool s_Searched = false;
        if (!s_Searched)
        {
            s_Searched = true;
            s_Shader = Unity::Shader::Find(System::String::New("Sprites/Default"));
            if (!s_Shader)
                s_Shader = Unity::Shader::Find(System::String::New("Hidden/Internal-Colored"));
            if (!s_Shader)
                Hax::LogError(G->Logger, L"Black hole: no unlit shader, visuals disabled");
        }
        if (!s_Shader || visuals.MaterialCount >= (int)std::size(visuals.Materials))
            return null;
        Unity::Material material = Unity::Material::New(s_Shader);
        material.SetColor(color);
        visuals.Materials[visuals.MaterialCount++] = material;
        return material;
    }

    static Unity::GameObject MakeBlackHoleSphere(BlackHoleVisuals& visuals, const Unity::Color& color)
    {
        Unity::Material material = MakeBlackHoleMaterial(visuals, color);
        if (!material)
            return null;
        Unity::GameObject sphere = FunPrimitive::Create(FunPrimitive::Sphere);
        if (!sphere)
            return null;
        if (Unity::Collider collider = sphere.GetComponent<Unity::Collider>())
            collider.SetEnabled(false);
        if (Unity::MeshRenderer renderer = sphere.GetComponent<Unity::MeshRenderer>())
            renderer.SetMaterial(material);
        sphere.GetTransform().SetParent(visuals.Root.GetTransform());
        return sphere;
    }

    static Unity::LineRenderer MakeBlackHoleLine(BlackHoleVisuals& visuals, const Unity::Color& color, float width, int points)
    {
        Unity::Material material = MakeBlackHoleMaterial(visuals, color);
        if (!material)
            return {};
        Unity::GameObject object = Unity::GameObject::New("RepoHaxBlackHoleLine");
        object.GetTransform().SetParent(visuals.Root.GetTransform());
        Unity::LineRenderer line = object.AddComponent<Unity::LineRenderer>();
        line.SetMaterial(material);
        line.SetStartWidth(width);
        line.SetEndWidth(width);
        line.SetPositionCount(points);
        return line;
    }

    static void BuildBlackHoleVisuals(BlackHoleState& hole)
    {
        BlackHoleVisuals& visuals = hole.Visuals;
        visuals.Root = Unity::GameObject::New("RepoHaxBlackHole");
        visuals.Root.GetTransform().SetPosition(hole.Center);

        visuals.Halo = MakeBlackHoleSphere(visuals, Unity::Color(0.55f, 0.15f, 1.f, 0.28f));
        visuals.Core = MakeBlackHoleSphere(visuals, Unity::Color(0.f, 0.f, 0.f, 1.f));

        const Unity::Color ringColors[BlackHoleRingCount] =
        {
            Unity::Color(1.f, 0.85f, 0.35f, 0.95f),
            Unity::Color(1.f, 0.45f, 0.1f, 0.8f),
            Unity::Color(0.7f, 0.25f, 1.f, 0.6f),
        };
        for (int i = 0; i < BlackHoleRingCount; ++i)
            visuals.Rings[i] = MakeBlackHoleLine(visuals, ringColors[i], 0.14f - 0.03f * (float)i, BlackHoleRingPoints + 1);
        for (int i = 0; i < BlackHoleArmCount; ++i)
        {
            visuals.Arms[i] = MakeBlackHoleLine(visuals, i % 2 ? Unity::Color(0.75f, 0.3f, 1.f, 0.7f) : Unity::Color(1.f, 0.55f, 0.2f, 0.7f),
                0.1f, BlackHoleArmPoints);
            if (visuals.Arms[i] != null)
            {
                visuals.Arms[i].SetStartWidth(0.16f); // outer end
                visuals.Arms[i].SetEndWidth(0.02f);   // vanishing into the core
            }
        }
        visuals.Shock = MakeBlackHoleLine(visuals, Unity::Color(1.f, 0.9f, 0.7f, 0.9f), 0.35f, BlackHoleRingPoints + 1);
        if (visuals.Shock != null)
            visuals.Shock.GetGameObject().SetActive(false);

        visuals.Light = visuals.Root.AddComponent<FunLight>();
        if (visuals.Light)
        {
            visuals.Light.SetColor(Unity::Color(0.6f, 0.2f, 1.f, 1.f));
            visuals.Light.SetRange(hole.Radius);
            visuals.Light.SetIntensity(3.f);
        }
    }

    static void DestroyBlackHoleVisuals(BlackHoleVisuals& visuals)
    {
        try
        {
            if (visuals.Root)
                Unity::Object::Destroy(visuals.Root);
            for (int i = 0; i < visuals.MaterialCount; ++i)
                if (visuals.Materials[i])
                    Unity::Object::Destroy(visuals.Materials[i]);
        }
        catch (System::Exception& ex)
        {
            LogFunException(L"Black hole visuals cleanup", ex);
        }
        visuals = {};
    }

    static float BlackHoleCoreRadius(const BlackHoleState& hole)
    {
        const float grow = std::clamp(hole.Time / std::max(hole.Duration, 0.1f), 0.f, 1.f);
        float radius = (0.45f + 0.65f * grow) * (1.f + 0.07f * std::sin(hole.Time * 8.f));
        // Shrinks to a point right before the bang.
        const float left = hole.Duration - hole.Time;
        if (left < BlackHoleImplosionTime)
            radius *= std::clamp(left / BlackHoleImplosionTime, 0.05f, 1.f);
        return radius;
    }

    static void UpdateBlackHoleVisuals(BlackHoleState& hole)
    {
        BlackHoleVisuals& visuals = hole.Visuals;
        if (!visuals.Root)
            return;

        const Unity::Vector3 center = hole.Center;
        if (hole.Finale)
        {
            // Shockwave ring racing outwards and a fading flash.
            const float f = std::clamp(hole.FinaleTime / BlackHoleFinaleTime, 0.f, 1.f);
            if (visuals.Core)
                visuals.Core.SetActive(false);
            if (visuals.Halo)
                visuals.Halo.SetActive(false);
            for (Unity::LineRenderer& ring : visuals.Rings)
                if (ring != null)
                    ring.GetGameObject().SetActive(false);
            for (Unity::LineRenderer& arm : visuals.Arms)
                if (arm != null)
                    arm.GetGameObject().SetActive(false);
            if (visuals.Shock != null)
            {
                visuals.Shock.GetGameObject().SetActive(true);
                const float radius = 0.5f + hole.Radius * 1.2f * (1.f - (1.f - f) * (1.f - f));
                visuals.Shock.SetStartWidth(0.6f * (1.f - f) + 0.05f);
                visuals.Shock.SetEndWidth(0.6f * (1.f - f) + 0.05f);
                for (int p = 0; p <= BlackHoleRingPoints; ++p)
                {
                    const float a = (float)p / (float)BlackHoleRingPoints * 6.2831853f;
                    visuals.Shock.SetPosition(p, center + Unity::Vector3(std::cos(a), 0.f, std::sin(a)) * radius);
                }
            }
            if (visuals.Light)
            {
                visuals.Light.SetColor(Unity::Color(1.f, 0.75f, 0.45f, 1.f));
                visuals.Light.SetIntensity(28.f * (1.f - f));
                visuals.Light.SetRange(hole.Radius * 2.f);
            }
            return;
        }

        const float coreRadius = BlackHoleCoreRadius(hole);
        if (visuals.Core)
            visuals.Core.GetTransform().SetLocalScale(Unity::Vector3(1.f, 1.f, 1.f) * (coreRadius * 2.f));
        if (visuals.Halo)
            visuals.Halo.GetTransform().SetLocalScale(Unity::Vector3(1.f, 1.f, 1.f) * (coreRadius * 3.4f));

        // Accretion disc: tilted rings spinning at different speeds with a plasma wobble.
        for (int i = 0; i < BlackHoleRingCount; ++i)
        {
            Unity::LineRenderer ring = visuals.Rings[i];
            if (ring == null)
                continue;
            const float radius = coreRadius * (2.1f + 0.95f * (float)i);
            const float spin = hole.Time * (3.2f - 0.8f * (float)i);
            const Unity::Quaternion tilt = Unity::Quaternion::Euler(16.f + 7.f * (float)i, 25.f * (float)i, 0.f);
            for (int p = 0; p <= BlackHoleRingPoints; ++p)
            {
                const float a = (float)p / (float)BlackHoleRingPoints * 6.2831853f + spin;
                const float wobble = 1.f + 0.06f * std::sin(a * 3.f + hole.Time * 6.f + (float)i);
                ring.SetPosition(p, center + tilt * (Unity::Vector3(std::cos(a), 0.f, std::sin(a)) * (radius * wobble)));
            }
        }

        // Spiral arms winding into the core.
        const float outer = std::max(coreRadius * 4.f, hole.Radius * 0.55f);
        for (int i = 0; i < BlackHoleArmCount; ++i)
        {
            Unity::LineRenderer arm = visuals.Arms[i];
            if (arm == null)
                continue;
            const float base = (float)i / (float)BlackHoleArmCount * 6.2831853f + hole.Time * 2.4f;
            for (int p = 0; p < BlackHoleArmPoints; ++p)
            {
                const float s = (float)p / (float)(BlackHoleArmPoints - 1); // 0 outer .. 1 core
                const float radius = outer + (coreRadius - outer) * s;
                const float a = base + s * 7.5f;
                const float y = std::sin(a * 0.5f + hole.Time) * 0.25f * (1.f - s);
                arm.SetPosition(p, center + Unity::Vector3(std::cos(a) * radius, y, std::sin(a) * radius));
            }
        }

        if (visuals.Light)
        {
            const float left = hole.Duration - hole.Time;
            const float boost = left < BlackHoleImplosionTime ? 3.f : 1.f;
            visuals.Light.SetIntensity((3.f + 1.5f * std::sin(hole.Time * 5.f)) * boost);
        }
    }

    // ----- networked part
    static Unity::GameObject SpawnBlackHolePart(Item item, const Unity::Vector3& position, Unity::Rigidbody& body)
    {
        Unity::GameObject object = null;
        try
        {
            object = SpawnDroneObject(item, position, Unity::Quaternion::identity());
        }
        catch (System::Exception& ex)
        {
            LogFunException(L"Black hole spawn", ex);
            object = null;
        }
        body = object ? PrepareDroneBody(object) : Unity::Rigidbody{};
        return object;
    }

    // Bodies are found and pushed in the same frame and never kept: a valuable
    // can break, get sold or despawn at any moment.
    static void PushBlackHoleBodies(float dt, bool collapse, float strengthScale)
    {
        BlackHoleState& hole = s_BlackHole;
        System::Array<Unity::RaycastHit> hits = Unity::Physics::SphereCastAll(hole.Center, hole.Radius, Unity::Vector3::up(), 0.01f, -1);
        if (hits == null)
            return;

        const float strength = hole.Strength * strengthScale;
        const float ramp = std::clamp(hole.Time, 0.f, 1.f);
        const float blend = 1.f - std::exp(-3.f * dt);
        const Unity::Vector3 up = Unity::Vector3::up();
        std::vector<Unity::Rigidbody> seen;
        seen.reserve(64);

        for (Unity::RaycastHit& hit : hits)
        {
            try
            {
                Unity::Rigidbody body = hit.GetRigidbody();
                if (!body)
                    continue; // walls, floors: nothing to move

                bool duplicate = false;
                for (const Unity::Rigidbody& other : seen)
                {
                    if (other == body)
                    {
                        duplicate = true;
                        break;
                    }
                }
                if (duplicate)
                    continue;
                seen.push_back(body);

                PhysGrabObject grab = body.GetComponent<PhysGrabObject>();
                if (!grab || grab.isPlayer() || grab.dead())
                    continue;

                const Unity::Vector3 position = body.GetTransform().GetPosition();
                Unity::Vector3 inward = hole.Center - position;
                const float distance = inward.GetMagnitude();
                inward = distance > 0.01f ? inward * (1.f / distance) : up;

                if (collapse)
                {
                    body.SetVelocity(inward * -(strength * 2.5f) + up * 7.f);
                    continue;
                }

                const Unity::Vector3 tangent = up.Cross(inward).GetNormalized();
                const float closeness = 1.f - std::clamp(distance / hole.Radius, 0.f, 1.f);
                Unity::Vector3 desired = distance < 1.2f
                    ? tangent * (strength * 0.8f) + inward * distance * 2.f
                    : inward * (strength * (0.6f + 1.4f * closeness)) + tangent * (strength * 0.9f * closeness) + up * 0.6f;
                desired = desired * ramp;
                const Unity::Vector3 velocity = body.GetVelocity();
                body.SetVelocity(velocity + (desired - velocity) * blend);
            }
            catch (System::Exception&)
            {
            }
        }
    }

    static void PushBlackHolePlayers(PlayerAvatar local, bool collapse, float strengthScale)
    {
        BlackHoleState& hole = s_BlackHole;
        const float ramp = std::clamp(hole.Time, 0.f, 1.f);
        const Unity::Vector3 up = Unity::Vector3::up();
        EnsureFrameTargets();
        for (const FunFrameTarget& entry : s_FramePlayers)
        {
            if (entry.Player == local && !hole.IncludeSelf)
                continue;
            const float distance = Unity::Vector3::Distance(entry.Point, hole.Center);
            if (distance > hole.Radius || (!collapse && distance < 0.8f))
                continue;
            PlayerTumble tumble = entry.Player.tumble();
            if (!tumble)
                continue;
            try
            {
                const Unity::Vector3 inward = distance > 0.01f ? (hole.Center - entry.Point) * (1.f / distance) : up;
                if (collapse)
                {
                    tumble.TumbleForce(inward * -30.f + up * 12.f);
                    continue;
                }
                if (!entry.Player.isTumbling())
                {
                    s_ForcedPlayerTumble = true;
                    tumble.TumbleRequest(true, false);
                    s_ForcedPlayerTumble = false;
                }
                const float closeness = 1.f - distance / hole.Radius;
                tumble.TumbleForce(inward * (4.f + hole.Strength * strengthScale * 0.6f * closeness) * ramp + up * 1.5f);
            }
            catch (System::Exception& ex)
            {
                s_ForcedPlayerTumble = false;
                LogFunException(L"Black hole player", ex);
            }
        }
    }

    static void ShakeBlackHoleCamera(float strength, float time)
    {
        if (GameDirector director = GameDirector::instance())
        {
            if (FunCameraShake shake = director.cameraShake())
                shake.ShakeDistance(strength, 3.f, s_BlackHole.Radius * 2.5f, s_BlackHole.Center, time);
        }
    }

    static void StopBlackHole(bool destroyObjects)
    {
        BlackHoleState& hole = s_BlackHole;
        if (destroyObjects)
        {
            for (Unity::GameObject& object : hole.Core)
                DestroyDroneObject(object);
            for (Unity::GameObject& object : hole.Shells)
                DestroyDroneObject(object);
            for (Unity::GameObject& object : hole.Orbs)
                DestroyDroneObject(object);
            DestroyBlackHoleVisuals(hole.Visuals);
        }
        hole = {};
        G->BlackHoleActive = false;
    }

    static void StartBlackHole(const Unity::Vector3& center, float radius, float duration, float strength, bool includeSelf)
    {
        if (!FunHostReady() || !EnsureDronePrefabsOrReport())
            return;
        if (s_BlackHole.Active)
            StopBlackHole(true);

        BlackHoleState& hole = s_BlackHole;
        hole.Active = true;
        hole.Center = center + Unity::Vector3::up() * 1.2f;
        hole.Radius = radius;
        hole.Duration = std::max(duration, 1.f);
        hole.Strength = strength;
        hole.IncludeSelf = includeSelf;

        for (int i = 0; i < 2; ++i)
        {
            hole.Core[i] = SpawnBlackHolePart(s_DronePrefabs.Grenade, hole.Center, hole.CoreBodies[i]);
            hole.CoreGrenades[i] = DroneFindComponent<ItemGrenade>(hole.Core[i]);
        }

        if (Item orb = FindBlackHoleOrbItem())
        {
            for (int i = 0; i < BlackHoleOrbCount; ++i)
            {
                const float a = (float)i / (float)BlackHoleOrbCount * 6.2831853f;
                hole.Orbs[i] = SpawnBlackHolePart(orb, hole.Center + Unity::Vector3(std::cos(a), 0.f, std::sin(a)) * 3.5f, hole.OrbBodies[i]);
                if (!hole.Orbs[i])
                    break;
            }
        }

        try
        {
            BuildBlackHoleVisuals(hole);
        }
        catch (System::Exception& ex)
        {
            LogFunException(L"Black hole visuals", ex);
            DestroyBlackHoleVisuals(hole.Visuals);
        }

        G->BlackHoleActive = true;
        SetSessionAction(G->Loc[LocKey_ActionBlackHoleOpened]);
    }

    static void CollapseBlackHole(PlayerAvatar local)
    {
        BlackHoleState& hole = s_BlackHole;
        for (int i = 0; i < 2; ++i)
        {
            DetonateGrenade(hole.Core[i], hole.CoreGrenades[i]);
            hole.Core[i] = null;
        }
        for (int i = 0; i < BlackHoleFinaleShells; ++i)
        {
            DetonateGrenade(hole.Shells[i], hole.ShellGrenades[i]);
            hole.Shells[i] = null;
        }
        PushBlackHoleBodies(0.f, true, 1.f);
        PushBlackHolePlayers(local, true, 1.f);
        ShakeBlackHoleCamera(14.f, 1.2f);

        hole.Finale = true;
        hole.FinaleTime = 0.f;
    }

    static void UpdateBlackHole(PlayerAvatar local, float dt)
    {
        if (G->BlackHoleRequested)
        {
            G->BlackHoleRequested = false;
            if (Unity::Camera camera = SemiFunc::MainCamera())
                StartBlackHole(DroneCrosshairPoint(camera, local) - camera.GetTransform().GetForward() * 0.5f,
                    (float)G->BlackHoleRadius, (float)G->BlackHoleDuration, (float)G->BlackHoleStrength, G->BlackHoleIncludeSelf);
        }
        if (G->BlackHoleStopRequested)
        {
            G->BlackHoleStopRequested = false;
            if (s_BlackHole.Active && !s_BlackHole.Finale)
                s_BlackHole.Duration = std::min(s_BlackHole.Duration, s_BlackHole.Time + BlackHoleImplosionTime); // implode now
        }

        BlackHoleState& hole = s_BlackHole;
        if (!hole.Active)
            return;

        if (hole.Finale)
        {
            hole.FinaleTime += dt;
            UpdateBlackHoleVisuals(hole);
            if (hole.FinaleTime >= BlackHoleFinaleTime)
                StopBlackHole(true); // orbs and visuals; the grenades are already gone
            return;
        }

        hole.Time += dt;
        const float left = hole.Duration - hole.Time;
        const bool imploding = left < BlackHoleImplosionTime;

        if (!hole.FinaleSpawned && left < BlackHoleFinaleSpawnLead)
        {
            hole.FinaleSpawned = true;
            for (int i = 0; i < BlackHoleFinaleShells; ++i)
            {
                const float a = (float)i / (float)BlackHoleFinaleShells * 6.2831853f;
                hole.Shells[i] = SpawnBlackHolePart(s_DronePrefabs.Grenade,
                    hole.Center + Unity::Vector3(std::cos(a), 0.f, std::sin(a)) * 2.5f, hole.ShellBodies[i]);
                hole.ShellGrenades[i] = DroneFindComponent<ItemGrenade>(hole.Shells[i]);
            }
        }

        if (left <= 0.f)
        {
            CollapseBlackHole(local);
            UpdateBlackHoleVisuals(hole);
            return;
        }

        const float coreRadius = BlackHoleCoreRadius(hole);

        // Core grenades spin in the middle.
        for (int i = 0; i < 2; ++i)
        {
            const float angle = hole.Time * 9.f + (float)i * 3.1415926f;
            DriveDroneBody(hole.Core[i], hole.CoreBodies[i],
                hole.Center + Unity::Vector3(std::cos(angle), 0.f, std::sin(angle)) * 0.3f, Unity::Vector3::zero(), nullptr);
        }

        // Glowing orbs spiral in: visible to every player.
        const float progress = std::clamp(hole.Time / hole.Duration, 0.f, 1.f);
        for (int i = 0; i < BlackHoleOrbCount; ++i)
        {
            const float angle = hole.Time * (1.6f + 2.4f * progress) + (float)i / (float)BlackHoleOrbCount * 6.2831853f;
            const float radius = 3.5f + (coreRadius * 1.6f - 3.5f) * progress;
            const Unity::Vector3 point = hole.Center + Unity::Vector3(std::cos(angle) * radius,
                std::sin(hole.Time * 2.f + (float)i) * 0.4f, std::sin(angle) * radius);
            DriveDroneBody(hole.Orbs[i], hole.OrbBodies[i], point, Unity::Vector3::zero(), nullptr);
        }

        // Finale grenades close in on the core with everything else.
        for (int i = 0; i < BlackHoleFinaleShells; ++i)
        {
            const float angle = (float)i / (float)BlackHoleFinaleShells * 6.2831853f + hole.Time * 6.f;
            const float radius = std::max(0.4f, 2.5f * std::clamp(left / BlackHoleFinaleSpawnLead, 0.f, 1.f));
            DriveDroneBody(hole.Shells[i], hole.ShellBodies[i],
                hole.Center + Unity::Vector3(std::cos(angle), 0.f, std::sin(angle)) * radius, Unity::Vector3::zero(), nullptr);
        }

        // Velocity carries over between pushes, so ~12 pushes a second look smooth.
        hole.PullTimer -= dt;
        hole.PullDt += dt;
        if (hole.PullTimer <= 0.f)
        {
            hole.PullTimer = BlackHolePullInterval;
            PushBlackHoleBodies(hole.PullDt, false, imploding ? 3.f : 1.f);
            hole.PullDt = 0.f;
        }

        hole.PlayerTimer -= dt;
        if (hole.PlayerTimer <= 0.f)
        {
            hole.PlayerTimer = 0.25f;
            PushBlackHolePlayers(local, false, imploding ? 2.5f : 1.f);
        }

        hole.ShakeTimer -= dt;
        if (hole.ShakeTimer <= 0.f)
        {
            hole.ShakeTimer = 0.2f;
            ShakeBlackHoleCamera((imploding ? 5.f : 1.5f) * std::clamp(hole.Time, 0.f, 1.f), 0.3f);
        }

        UpdateBlackHoleVisuals(hole);
    }

    // ---------------------------------------------------------------- Chaos mode
    enum class ChaosEvent { GravityFlip, ZeroGravity, Blackout, SlowMotion, Turbo, TumbleAll, DroneRaid, Orbital, BlackHole,
        SpawnEnemy, LaunchEveryone, Count };

    struct ChaosState
    {
        uint64_t NextAt = 0;
        uint64_t RevertAt = 0;
        bool Reverting = false;
        ChaosEvent Timed = ChaosEvent::Count;
        ChaosEvent Last = ChaosEvent::Count;
        bool SavedGravityOverride = false;
        int SavedGravityY = -10;
        int SavedTimeScale = 100;
    };

    static ChaosState s_Chaos;

    static void ChaosRevert()
    {
        ChaosState& chaos = s_Chaos;
        switch (chaos.Timed)
        {
        case ChaosEvent::GravityFlip:
        case ChaosEvent::ZeroGravity:
            G->WorldGravityOverride = chaos.SavedGravityOverride;
            G->WorldGravityY = chaos.SavedGravityY;
            break;
        case ChaosEvent::Blackout:
            G->WorldLightAction = WorldLightCommand::Restore;
            break;
        case ChaosEvent::SlowMotion:
        case ChaosEvent::Turbo:
            G->WorldTimeScalePercent = chaos.SavedTimeScale;
            break;
        default:
            break;
        }
        chaos.Timed = ChaosEvent::Count;
        chaos.RevertAt = 0;
    }

    static void ChaosAnnounce(LocKey key, uint64_t now)
    {
        swprintf_s(G->ChaosBanner, L"%ls", G->Loc[key].Data());
        G->ChaosBannerUntil = now + 3500;
        SetSessionAction(G->Loc[key]);
    }

    static bool ChaosRun(ChaosEvent event, PlayerAvatar local, uint64_t now)
    {
        ChaosState& chaos = s_Chaos;
        PlayerAvatar player = null;
        EnemyParent enemy = null;
        Unity::Vector3 point;

        auto startTimed = [&](uint64_t durationMs)
        {
            chaos.Timed = event;
            chaos.RevertAt = now + durationMs;
        };

        switch (event)
        {
        case ChaosEvent::GravityFlip:
        case ChaosEvent::ZeroGravity:
            chaos.SavedGravityOverride = G->WorldGravityOverride;
            chaos.SavedGravityY = G->WorldGravityY;
            G->WorldGravityOverride = true;
            G->WorldGravityY = event == ChaosEvent::GravityFlip ? 6 : 0;
            startTimed(event == ChaosEvent::GravityFlip ? 6000 : 9000);
            ChaosAnnounce(event == ChaosEvent::GravityFlip ? LocKey_ChaosGravityFlip : LocKey_ChaosZeroGravity, now);
            return true;
        case ChaosEvent::Blackout:
            G->WorldLightAction = WorldLightCommand::Blackout;
            startTimed(10000);
            ChaosAnnounce(LocKey_ChaosBlackout, now);
            return true;
        case ChaosEvent::SlowMotion:
        case ChaosEvent::Turbo:
            chaos.SavedTimeScale = G->WorldTimeScalePercent;
            G->WorldTimeScalePercent = event == ChaosEvent::SlowMotion ? 35 : 200;
            startTimed(6000);
            ChaosAnnounce(event == ChaosEvent::SlowMotion ? LocKey_ChaosSlowMotion : LocKey_ChaosTurbo, now);
            return true;
        case ChaosEvent::TumbleAll:
        case ChaosEvent::LaunchEveryone:
        {
            EnsureFrameTargets();
            int hit = 0;
            for (const FunFrameTarget& entry : s_FramePlayers)
            {
                if (G->ChaosSpareMe && entry.Player == local)
                    continue;
                PlayerTumble tumble = entry.Player.tumble();
                if (!tumble)
                    continue;
                s_ForcedPlayerTumble = true;
                tumble.TumbleRequest(true, false);
                s_ForcedPlayerTumble = false;
                if (event == ChaosEvent::LaunchEveryone)
                    tumble.TumbleForce(Unity::Vector3((FunRandom01() - 0.5f) * 8.f, 28.f, (FunRandom01() - 0.5f) * 8.f));
                ++hit;
            }
            if (hit == 0)
                return false;
            ChaosAnnounce(event == ChaosEvent::TumbleAll ? LocKey_ChaosTumbleAll : LocKey_ChaosLaunchEveryone, now);
            return true;
        }
        case ChaosEvent::DroneRaid:
        {
            if (!FunPickRandomTarget(G->ChaosSpareMe, player, enemy, point) || !FindDronePrefabs())
                return false;
            DroneLaunchOrder order = MakeDroneOrderFromSettings();
            order.Total = 5;
            order.Manual = false;
            order.Retarget = false;
            order.Quiet = true;
            order.TargetPlayer = player;
            order.TargetEnemy = enemy;
            s_DroneOrders.push_back(order);
            ChaosAnnounce(LocKey_ChaosDroneRaid, now);
            return true;
        }
        case ChaosEvent::Orbital:
            if (!FunPickRandomTarget(G->ChaosSpareMe, player, enemy, point))
                return false;
            StartOrbitalStrike(FunGroundOfTarget(player, enemy, point), player, enemy);
            ChaosAnnounce(LocKey_ChaosOrbital, now);
            return true;
        case ChaosEvent::BlackHole:
            if (s_BlackHole.Active || !FunPickRandomTarget(G->ChaosSpareMe, player, enemy, point))
                return false;
            StartBlackHole(point + Unity::Vector3((FunRandom01() - 0.5f) * 4.f, 0.f, (FunRandom01() - 0.5f) * 4.f),
                10.f, 5.f, 11.f, !G->ChaosSpareMe);
            ChaosAnnounce(LocKey_ChaosBlackHole, now);
            return true;
        case ChaosEvent::SpawnEnemy:
            if (G->EnemiesPool.Size() == 0)
                return false;
            G->EnemyToSpawn = G->EnemiesPool.begin()[(size_t)FunRandom((int)G->EnemiesPool.Size())].value;
            ChaosAnnounce(LocKey_ChaosSpawnEnemy, now);
            return true;
        default:
            return false;
        }
    }

    static void UpdateChaos(PlayerAvatar local, uint64_t now)
    {
        ChaosState& chaos = s_Chaos;
        if (!G->ChaosEnabled || !FunHostReady())
        {
            if (chaos.Timed != ChaosEvent::Count)
                ChaosRevert();
            chaos.NextAt = 0;
            G->ChaosNextIn = 0;
            return;
        }

        if (chaos.Timed != ChaosEvent::Count && now >= chaos.RevertAt)
            ChaosRevert();

        const uint64_t interval = (uint64_t)std::clamp(G->ChaosInterval, 5, 300) * 1000;
        if (chaos.NextAt == 0 || chaos.NextAt > now + interval)
            chaos.NextAt = now + std::min<uint64_t>(interval, 3000); // first event soon after enabling
        if (G->ChaosTriggerNow)
        {
            G->ChaosTriggerNow = false;
            chaos.NextAt = now;
        }

        if (now >= chaos.NextAt)
        {
            if (chaos.Timed != ChaosEvent::Count)
                ChaosRevert();
            // A few tries: some events need a target or a free black hole.
            for (int attempt = 0; attempt < 8; ++attempt)
            {
                ChaosEvent event = (ChaosEvent)FunRandom((int)ChaosEvent::Count);
                if (event == chaos.Last)
                    continue;
                if (ChaosRun(event, local, now))
                {
                    chaos.Last = event;
                    break;
                }
            }
            chaos.NextAt = now + interval;
        }
        G->ChaosNextIn = (int)((chaos.NextAt - std::min(now, chaos.NextAt) + 999) / 1000);
    }

    // ---------------------------------------------------------------- Protection
    static bool FunOwns(Unity::Transform transform, Unity::GameObject object)
    {
        return object && IsTransformBelow(transform, object.GetTransform());
    }

    static bool IsFunObjectProtected(Unity::Transform transform)
    {
        if (s_FunReleasing || !transform)
            return false;
        for (DroneSlot& slot : s_Drones)
        {
            if (slot.Phase == DronePhase::Idle)
                continue;
            if (FunOwns(transform, slot.Object))
                return true;
            for (int i = 0; i < slot.WarheadCount; ++i)
                if (FunOwns(transform, slot.Warheads[i].Object))
                    return true;
        }
        for (OrbitalStrike& strike : s_Orbitals)
        {
            if (FunOwns(transform, strike.Laser.Object))
                return true;
            for (int i = 0; i < strike.ShellCount; ++i)
                if (FunOwns(transform, strike.Shells[i].Object))
                    return true;
        }
        return BlackHoleOwns(transform);
    }

    // ---------------------------------------------------------------- Driver
    static void UpdateFunFeatures()
    {
        try
        {
            ++s_FunFrame;
            if (!FunHostReady())
            {
                // Scene change or lost host: the objects are gone or no longer ours.
                ResetAllDrones();
                s_Orbitals.clear();
                s_BlackHole = {};
                G->BlackHoleActive = false;
                G->OrbitalsActive = 0;
                G->DroneLaunchRequested = false;
                G->DroneDetonateRequested = false;
                G->OrbitalRequested = false;
                G->OrbitalAtTargetRequested = false;
                G->BlackHoleRequested = false;
                UpdateFpvSelection(0);
                UpdateChaos(null, ::GetTickCount64());
                return;
            }

            const uint64_t now = ::GetTickCount64();
            const float dt = std::clamp(Unity::Time::GetDeltaTime(), 0.f, 0.1f);
            PlayerAvatar local = PlayerAvatar::instance();

            const struct { const wchar_t* Name; void (*Fn)(PlayerAvatar, uint64_t, float); } steps[] =
            {
                {L"Chaos", [](PlayerAvatar l, uint64_t n, float) { UpdateChaos(l, n); }},
                {L"Drone strike", [](PlayerAvatar l, uint64_t n, float d) { UpdateDroneStrikes(l, n, d); }},
                {L"Orbital strike", [](PlayerAvatar l, uint64_t, float d) { UpdateOrbitalStrikes(l, d); }},
                {L"Black hole", [](PlayerAvatar l, uint64_t, float d) { UpdateBlackHole(l, d); }},
                {L"FPV camera", [](PlayerAvatar l, uint64_t, float) { ApplyDroneFpvCamera(l); }},
            };
            for (const auto& step : steps)
            {
                try
                {
                    step.Fn(local, now, dt);
                }
                catch (System::Exception& ex)
                {
                    LogFunException(step.Name, ex);
                }
            }
        }
        catch (System::Exception& ex)
        {
            LogFunException(L"Fun features", ex);
        }
    }

    // ---------------------------------------------------------------- Spectator
    // While dead the game runs its own SpectateCamera (death cam, then an orbit around a
    // teammate that LMB/RMB/Space already switch). On top of it we offer a CS-style view
    // from the teammate's eyes and a free-flying camera. The camera is moved only for this
    // frame's render and put back in Hooked__PresentAfterDrawLoop, like the FPV feed.
    struct SpectatorCameraBackup
    {
        bool Valid = false;
        Unity::Camera Camera = null;
        Unity::Transform Transform = null;
        Unity::Vector3 LocalPosition{};
        Unity::Quaternion Rotation = Unity::Quaternion::identity();
        float NearClip = 0.01f;
    };
    static SpectatorCameraBackup s_SpecBackup;
    static Unity::Vector3 s_SpecFreePos{};
    static int s_SpecFreeReinitFrames = 1;   // > 0: copy the game camera position before flying
    static bool s_SpecKeyWasDown[3];

    enum { SpecKey_Prev, SpecKey_Next, SpecKey_View };

    static bool SpectatorKeyDown(int vk)
    {
        return vk > 0 && vk < 256 && (G->GetAsyncKeyStateHook.unsafe_stdcall<SHORT, int>(vk) & 0x8000) != 0;
    }

    static bool SpectatorKeyPressed(int slot, int vk, bool inputAllowed)
    {
        const bool down = SpectatorKeyDown(vk);
        const bool pressed = down && !s_SpecKeyWasDown[slot];
        s_SpecKeyWasDown[slot] = down;
        return pressed && inputAllowed;
    }

    static void SpectatorFillTarget(PlayerAvatar target)
    {
        G->SpectatorTargetName[0] = 0;
        G->SpectatorTargetIndex = 0;
        G->SpectatorTargetCount = 0;
        GameDirector director = GameDirector::instance();
        if (!director)
            return;
        for (PlayerAvatar player : director.PlayerList())
        {
            if (!player || player.isDisabled())
                continue;
            ++G->SpectatorTargetCount;
            if (target && player == target)
                G->SpectatorTargetIndex = G->SpectatorTargetCount;
        }
        if (target)
        {
            System::String name = target.playerName();
            if (name != null && name.GetRawStringData())
                wcsncpy_s(G->SpectatorTargetName, name.GetRawStringData(), _TRUNCATE);
        }
    }

    static bool SpectatorEyes(PlayerAvatar target, PlayerAvatar local, Unity::Vector3& position, Unity::Quaternion& rotation)
    {
        if (!target || target == local || target.isDisabled() || target.deadSet())
            return false;
        PlayerLocalCamera eyes = target.localCamera();
        if (!eyes)
            return false;
        Unity::Transform eyesTransform = eyes.GetTransform();
        if (!eyesTransform)
            return false;
        rotation = eyesTransform.GetRotation();
        // A little forward so the camera sits in front of the teammate's eyes; the raised
        // near plane (see ApplySpectatorCamera) culls the rest of their head.
        position = eyesTransform.GetPosition() + (rotation * Unity::Vector3::forward()) * 0.08f;
        return true;
    }

    static void SpectatorFreeFly(Unity::Transform cameraTransform, Unity::Vector3& position, Unity::Quaternion& rotation, bool inputAllowed)
    {
        if (s_SpecFreeReinitFrames > 0)
        {
            --s_SpecFreeReinitFrames;
            s_SpecFreePos = cameraTransform.GetPosition();
        }

        // The game's orbit camera already turns with the mouse (with the player's
        // sensitivity settings), so the free camera borrows its rotation.
        rotation = cameraTransform.GetRotation();

        if (inputAllowed)
        {
            const float dt = std::clamp(Unity::Time::GetDeltaTime(), 0.f, 0.1f);
            float speed = (float)G->SpectatorFreeSpeed;
            if (SpectatorKeyDown(VK_SHIFT))
                speed *= 3.f;
            if (SpectatorKeyDown(VK_MENU))
                speed *= 0.3f;

            const Unity::Vector3 forward = rotation * Unity::Vector3::forward();
            const Unity::Vector3 right = rotation * Unity::Vector3::right();
            Unity::Vector3 move = Unity::Vector3::zero();
            if (SpectatorKeyDown('W')) move = move + forward;
            if (SpectatorKeyDown('S')) move = move - forward;
            if (SpectatorKeyDown('D')) move = move + right;
            if (SpectatorKeyDown('A')) move = move - right;
            if (SpectatorKeyDown(VK_SPACE)) move = move + Unity::Vector3::up();
            if (SpectatorKeyDown(VK_CONTROL)) move = move - Unity::Vector3::up();
            if (move.GetMagnitude() > 0.001f)
                s_SpecFreePos = s_SpecFreePos + move.GetNormalized() * (speed * dt);
        }
        position = s_SpecFreePos;
    }

    static void ApplySpectatorCamera()
    {
        G->SpectatorActive = false;
        G->SpectatorDeathCam = false;

        const bool inputAllowed = !G->MenuVisible && G->KeyListenerId == 0 &&
            ::GetForegroundWindow() == G->GameWndHandle &&
            !(MenuManager::instance() && MenuManager::instance().currentMenuPage());

        // Always sample the keys so a press made before death does not fire later.
        const bool prevPressed = SpectatorKeyPressed(SpecKey_Prev, VK_LBUTTON, inputAllowed);
        const bool nextPressed = SpectatorKeyPressed(SpecKey_Next, VK_RBUTTON, inputAllowed);
        const bool viewPressed = SpectatorKeyPressed(SpecKey_View, G->VkSpectatorView, inputAllowed);

        PlayerAvatar local = PlayerAvatar::instance();
        SpectateCamera spectate = G->IsInGame && G->SpectatorEnabled ? SpectateCamera::instance() : null;
        if (!spectate || !local || !local.deadSet() || G->FpvActive)
        {
            s_SpecFreeReinitFrames = 1;
            return;
        }

        const int state = spectate.currentState();
        if (state == 0)
        {
            // Death cam: any mouse button jumps straight to watching a teammate.
            G->SpectatorDeathCam = true;
            if (G->SpectatorSkipDeathCam && (prevPressed || nextPressed))
            {
                spectate.stateDeathDelayTimer() = 0.f;
                spectate.stateTimer() = 0.f;
            }
            return;
        }
        if (state != 1)
            return; // Head state: the player is steering their own dead head.

        if (viewPressed)
        {
            G->SpectatorViewMode = (G->SpectatorViewMode + 1) % SpectatorView_Count;
            s_SpecFreeReinitFrames = 1;
        }
        // The game switches the target itself on LMB/RMB this frame; the free camera then
        // jumps to the new teammate once the orbit camera has moved there.
        if ((prevPressed || nextPressed) && G->SpectatorViewMode == SpectatorView_Free)
            s_SpecFreeReinitFrames = 2;

        PlayerAvatar target = spectate.player();
        SpectatorFillTarget(target);
        G->SpectatorActive = true;

        if (G->SpectatorViewMode == SpectatorView_Game)
            return;

        Unity::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;
        Unity::Transform cameraTransform = camera.GetTransform();
        if (!cameraTransform)
            return;

        Unity::Vector3 position{};
        Unity::Quaternion rotation = Unity::Quaternion::identity();
        float nearClip = -1.f;
        if (G->SpectatorViewMode == SpectatorView_Eyes)
        {
            if (!SpectatorEyes(target, local, position, rotation))
                return; // nobody to look through: keep the game's orbit
            nearClip = 0.18f;
        }
        else
        {
            SpectatorFreeFly(cameraTransform, position, rotation, inputAllowed);
        }

        s_SpecBackup = {true, camera, cameraTransform, cameraTransform.GetLocalPosition(), cameraTransform.GetRotation(), camera.GetNearClipPlane()};
        cameraTransform.SetPosition(position);
        cameraTransform.SetRotation(rotation);
        if (nearClip > 0.f && s_SpecBackup.NearClip < nearClip)
            camera.SetNearClipPlane(nearClip);
    }

    static void RestoreSpectatorCamera()
    {
        if (!s_SpecBackup.Valid)
            return;
        s_SpecBackup.Valid = false;
        try
        {
            if (s_SpecBackup.Transform)
            {
                s_SpecBackup.Transform.SetLocalPosition(s_SpecBackup.LocalPosition);
                s_SpecBackup.Transform.SetRotation(s_SpecBackup.Rotation);
            }
            if (s_SpecBackup.Camera)
                s_SpecBackup.Camera.SetNearClipPlane(s_SpecBackup.NearClip);
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"Spectator restore: %ls", message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }
        s_SpecBackup.Transform = null;
        s_SpecBackup.Camera = null;
    }

    static float prevPlane;
    static float prevFov;
    static bool prevOcclusionCulling;
    static bool restoreClipPlane;
    static bool restoreOcclusionCulling;
    static void Hooked__PostLateUpdateLoop()
    {
        // Runs first: it may move the camera onto an FPV drone, and the ESP below
        // must project from the view that is actually rendered.
        UpdateFunFeatures();

        try
        {
            ApplySpectatorCamera();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"Spectator: %ls", message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        try
        {
            Unity::Camera mainCam = SemiFunc::MainCamera();
            PlayerAvatar avatar = PlayerAvatar::instance();
            if (mainCam && G->IsInGame && avatar)
            {
                restoreClipPlane = false;
                restoreOcclusionCulling = false;
                if (G->BetterVision || G->DisableOcclusionCulling)
                {
                    prevPlane = mainCam.GetFarClipPlane();
                    restoreClipPlane = true;
                }

                if (G->BetterVision)
                {
                    if (prevPlane < 32.f)
                        mainCam.SetFarClipPlane(32.f);
                }

                if (G->DisableOcclusionCulling)
                {
                    prevOcclusionCulling = mainCam.GetUseOcclusionCulling();
                    restoreOcclusionCulling = true;
                    mainCam.SetUseOcclusionCulling(false);
                    mainCam.SetFarClipPlane((float)G->RenderDistance);
                }

                prevFov = mainCam.GetFieldOfView();
                if (prevFov < (float)G->Fov)
                    mainCam.SetFieldOfView((float)G->Fov);

                if (G->ThirdPerson && !SpectateCamera::instance())
                    if (!G->FpvActive)
                        SetThirdPerson();

                if (ShouldDrawEsp())
                {
                    G->PixelHeight = (float)mainCam.GetPixelHeight();
                    G->PixelWidth = (float)mainCam.GetPixelWidth();
                    G->ScreenHeight = (float)Unity::Screen::GetHeight();
                    G->ScreenWidth = (float)Unity::Screen::GetWidth();

                    if (G->EnemiesEsp)
                    {
                        auto& back = G->EnemiesEspBuffer.GetBack();
                        back.Clear();
                        if (EnemyDirector dir = EnemyDirector::instance())
                        {
                            for (EnemyParent parent : dir.enemiesSpawned())
                            {
                                if (!parent || !parent.Spawned())
                                    continue;
                                Enemy enemy = parent.enemy();
                                if (enemy)
                                {
                                    Hax::Optional<EnemyEspData> data = ParseEnemyEspData(enemy);
                                    if (data.HasValue())
                                        back.PushBack(*data);
                                }
                            }
                        }
                        G->EnemiesEspBuffer.RefreshSpare();
                    }

                    if (G->ValuablesEsp)
                    {
                        auto& back = G->ValuablesEspBuffer.GetBack();
                        back.Clear();
                        if (ValuableDirector dir = ValuableDirector::instance())
                        {
                            for (ValuableObject obj : dir.valuableList())
                            {
                                Hax::Optional<ValuableEspData> data = ParseValuableEspData(obj);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->ValuablesEspBuffer.RefreshSpare();
                    }

                    if (G->ExtrPointsEsp)
                    {
                        auto& back = G->ExtrPointsEspBuffer.GetBack();
                        back.Clear();
                        if (RoundDirector dir = RoundDirector::instance())
                        {
                            for (Unity::GameObject go : dir.extractionPointList())
                            {
                                Hax::Optional<ExtrPointEspData> data = ParseExtrPointEspData(go);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->ExtrPointsEspBuffer.RefreshSpare();
                    }

                    if (G->TruckEsp)
                    {
                        auto& back = G->TruckEspBuffer.GetBack();
                        back.Clear();
                        if (TruckSafetySpawnPoint truck = TruckSafetySpawnPoint::instance())
                        {
                            Hax::Optional<TruckEspData> data = ParseTruckEspData(truck);
                            if (data.HasValue())
                                back.PushBack(*data);
                        }
                        G->TruckEspBuffer.RefreshSpare();
                    }

                    if (G->PlayersEsp)
                    {
                        auto& back = G->PlayersEspBuffer.GetBack();
                        back.Clear();
                        if (GameDirector dir = GameDirector::instance())
                        {
                            for (PlayerAvatar avatar : dir.PlayerList())
                            {
                                Hax::Optional<PlayerEspData> data = ParsePlayerEspData(avatar);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->PlayersEspBuffer.RefreshSpare();
                    }

                    if (G->CosmeticBoxesEsp)
                    {
                        auto& back = G->CosmeticBoxesEspBuffer.GetBack();
                        back.Clear();
                        if (RoundDirector dir = RoundDirector::instance())
                        {
                            for (CosmeticWorldObject box : dir.cosmeticWorldObjects())
                            {
                                Hax::Optional<CosmeticBoxEspData> data = ParseCosmeticBoxEspData(box);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->CosmeticBoxesEspBuffer.RefreshSpare();
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->PostLateUpdateHook.unsafe_call<void>();
        UpdateEyeLasers();

        static bool s_FogOverridden = false;
        static bool s_PreviousFog = true;
        try
        {
            if (G->NoFog && G->IsInGame)
            {
                if (!s_FogOverridden)
                {
                    s_PreviousFog = Unity::RenderSettings::GetFog();
                    s_FogOverridden = true;
                }
                Unity::RenderSettings::SetFog(false);
            }
            else if (s_FogOverridden)
            {
                Unity::RenderSettings::SetFog(s_PreviousFog);
                s_FogOverridden = false;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"Fog override: %ls", message != null ? message.ToString().GetRawStringData() : L"Exception without message");
            s_FogOverridden = false;
        }
    }

    static void Hooked__PresentAfterDrawLoop()
    {
        try
        {
            Unity::Camera mainCam = SemiFunc::MainCamera();
            if (mainCam && G->IsInGame)
            {
                if (restoreClipPlane)
                {
                    mainCam.SetFarClipPlane(prevPlane);
                    restoreClipPlane = false;
                }

                if (restoreOcclusionCulling)
                {
                    mainCam.SetUseOcclusionCulling(prevOcclusionCulling);
                    restoreOcclusionCulling = false;
                }

                mainCam.SetFieldOfView(prevFov);

                if (G->ThirdPerson && !SpectateCamera::instance())
                    mainCam.GetTransform().SetLocalPosition(Unity::Vector3::zero());
            }
            RestoreDroneFpvCamera();
            RestoreSpectatorCamera();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->PresentAfterDrawHook.unsafe_call<void>();
    }

    static void Hooked__PhysGrabObject_PhysicsGrabbingManipulation(PhysGrabObject __this)
    {
        try
        {
            if (PlayerAvatar avatar = PlayerAvatar::instance())
            {
                if (PhysGrabber grabber = avatar.physGrabber())
                {
                    float backup = grabber.grabStrength();
                    if (G->EasyGrab && backup < 20.f)
                        grabber.grabStrength() = 20.f;

                    G->PhysGrabObject_PhysicsGrabbingManipulation_Hook.unsafe_call<void, PhysGrabObject>(__this);

                    grabber.grabStrength() = backup;
                    return;
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->PhysGrabObject_PhysicsGrabbingManipulation_Hook.unsafe_call<void, PhysGrabObject>(__this);
    }

    static void Hooked__PostProcessLayer_BuildCommandBuffers(Unity::PostProcessLayer __this)
    {
        try
        {
            if (G->BetterVision && G->IsInGame)
            {
                __this.m_LegacyCmdBufferBeforeReflections().Clear();
                __this.m_LegacyCmdBufferBeforeLighting().Clear();
                __this.m_LegacyCmdBufferOpaque().Clear();
                __this.m_LegacyCmdBuffer().Clear();
                return;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->PostProcessLayer_BuildCommandBuffers_Hook.unsafe_call<void, Unity::PostProcessLayer>(__this);
    }

    void Hooked__PlayerAvatarVisuals_Update(PlayerAvatarVisuals __this)
    {
        try
        {
            if (GameManager gm = GameManager::instance())
            {
                PlayerAvatar avatar = __this.playerAvatar();
                if (G->ThirdPerson && avatar && !__this.isMenuAvatar() && avatar.isLocal())
                {
                    __this.showSelfOverrideTimer() = Hax::Max(0.1f, __this.showSelfOverrideTimer());
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->PlayerAvatarVisuals_Update_Hook.unsafe_call<void, PlayerAvatarVisuals>(__this);
    }

    static void Hooked__PhysGrabber_PhysGrabLogic(PhysGrabber __this)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (G->UnlimGrabRange && playerAvatar && __this == playerAvatar.physGrabber())
            range = 999999.f;

        G->PhysGrabber_PhysGrabLogic_Hook.unsafe_call<void, PhysGrabber>(__this);

        range = cached;
    }

    static bool g_RaycastMaxDist;
    static void Hooked__PhysGrabber_RayCheck(PhysGrabber __this, bool grab)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (G->UnlimGrabRange && playerAvatar && __this == playerAvatar.physGrabber()) 
        {
            range = 999999.f;
            g_RaycastMaxDist = true;
        }

        G->PhysGrabber_RayCheck_Hook.unsafe_call<void, PhysGrabber, bool>(__this, grab);

        range = cached;
        g_RaycastMaxDist = false;
    }

    static bool Hooked__Physics_Raycast(void* v1, void* v2, void* v3, float maxDistance, int v5, int v6)
    {
        if (g_RaycastMaxDist)
            maxDistance = 999999.f;

        return G->Physics_Raycast_Hook.unsafe_call<bool, void*, void*, void*, float, int, int>(v1, v2, v3, maxDistance, v5, v6);
    }

    static void Hooked__PhysGrabObjectImpactDetector_Break(PhysGrabObjectImpactDetector __this, float a1, void* a2, int a3, bool a4)
    {
        if (G->Unbreakable && __this.isValuable())
            return;

        G->PhysGrabObjectImpactDetector_Break_Hook.unsafe_call<void, PhysGrabObjectImpactDetector, float, void*, int, bool>(__this, a1, a2, a3, a4);
    }

    static void Hooked__PhysGrabObjectImpactDetector_DestroyObject(PhysGrabObjectImpactDetector __this, bool effects, bool forced)
    {
        // Forced cleanup must still remove extracted valuables and spent objects.
        if (G->Unbreakable && __this.isValuable() && !forced)
            return;

        // Our drones and not-yet-fired grenades must not be broken by the game
        // (a neighbour's blast, a hard hit): we destroy them ourselves later, and
        // destroying an object twice crashes Unity.
        try
        {
            if (!forced && IsFunObjectProtected(__this.GetTransform()))
                return;
        }
        catch (System::Exception&)
        {
        }

        G->PhysGrabObjectImpactDetector_DestroyObject_Hook.unsafe_call<void, PhysGrabObjectImpactDetector, bool, bool>(__this, effects, forced);
    }

    static bool g_PretendMaster;
    static bool Hooked__PhotonNetwork_IsMasterClient()
    {
        if (g_PretendMaster)
        {
            g_PretendMaster = false;
            return true;
        }
        return G->PhotonNetwork_IsMasterClient_Hook.unsafe_call<bool>();
    }

    static void Hooked__ItemBattery_Update(ItemBattery __this)
    {
        try
        {
            if (G->InfBattery && __this.batteryLifeInt() < __this.batteryBars() && __this.physGrabObject().grabbedLocal())
            {
                //g_PretendMaster = true;
                __this.BatteryFullPercentChange(__this.batteryBars(), false, false);
                //g_PretendMaster = false;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->ItemBattery_Update_Hook.unsafe_call<void, ItemBattery>(__this);
    }

    static void Hooked__Camera_FireOnPreRender(Unity::Camera __this)
    {
        try
        {
            if (IsInGame() && __this == GameDirector::instance().MainCamera() && G->ItemsChamsMat != null && G->CommandBuffer != null)
            {
                G->CommandBuffer.Clear();

                static Unity::Camera s_PrevCam = null;
                if (__this && __this != s_PrevCam)
                {
                    s_PrevCam = __this;
                    __this.AddCommandBuffer(Unity::CameraEvent::AfterEverything, G->CommandBuffer);
                }

                if (G->ValuablesChams && ValuableDirector::instance())
                {
                    for (ValuableObject obj : ValuableDirector::instance().valuableList())
                    {
                        if (obj && obj.GetEnabled())
                        {
                            for (Unity::MeshRenderer renderer : obj.GetTransform().GetChild(0).GetComponentsInChildren<Unity::MeshRenderer>(true))
                            {
                                G->CommandBuffer.DrawRenderer(renderer, G->ItemsChamsMat);
                            }
                        }
                    }
                }

                if (G->PlayersChams && GameDirector::instance())
                {
                    for (PlayerAvatar player : GameDirector::instance().PlayerList())
                    {
                        DrawPlayerChams(player, G->CommandBuffer, G->PlayerChamsMat, G->HeadChamsMat);
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->Camera_FireOnPreRender_Hook.unsafe_call<void, Unity::Camera>(__this);
    }

    static void Hooked__ItemGun_Update(ItemGun __this)
    {
        try
        {
            Unity::LineRenderer laser = __this.GetComponent<Unity::LineRenderer>();

            if (__this.physGrabObject().grabbedLocal() && G->UseLaser)
            {
                if (!laser)
                {
                    laser = __this.GetGameObject().AddComponent<Unity::LineRenderer>();
                    laser.SetStartWidth(0.02f);
                    laser.SetEndWidth(0.02f);
                    laser.SetSortingOrder(1);
                    Unity::Material mat = Unity::Material::New(Unity::Shader::Find(System::String::New("Sprites/Default")));
                    mat.SetColor(Unity::Color::red());
                    laser.SetMaterial(mat);
                    laser.SetPositionCount(2);
                }

                Unity::Transform transform = __this.gunMuzzle();
                laser.SetEnabled(true);
                laser.SetPosition(0, transform.GetPosition());
                laser.SetPosition(1, transform.GetPosition() + transform.GetForward() * __this.gunRange());
            }
            else if (laser)
            {
                laser.SetEnabled(false);
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        G->ItemGun_Update_Hook.unsafe_call<void, ItemGun>(__this);
    }

    static void Hooked__RunManager_SetRunLevel(RunManager __this)
    {
        try
        {
            if (G->LevelBans.Size() > 0)
            {
                List<Level> levels = __this.levels();
                Level levelToAvoid = s_RerollingRunLevel ? s_RunLevelToAvoid : __this.previousRunLevel();
                s_RerollingRunLevel = false;
                s_RunLevelToAvoid = null;

                int candidateCount = 0;
                for (const LevelBan& ban : G->LevelBans)
                {
                    if (!ban.Allowed || ban.Index < 0 || ban.Index >= levels.Count())
                        continue;

                    Level level = levels[ban.Index];
                    if (level && (!levelToAvoid || level != levelToAvoid))
                        ++candidateCount;
                }

                // If the filter leaves only the current map enabled, using it is
                // preferable to bypassing the filter or looping forever.
                if (candidateCount == 0)
                {
                    levelToAvoid = null;
                    for (const LevelBan& ban : G->LevelBans)
                    {
                        if (ban.Allowed && ban.Index >= 0 && ban.Index < levels.Count() && levels[ban.Index])
                            ++candidateCount;
                    }
                }

                if (candidateCount > 0)
                {
                    int selectedCandidate = rand() % candidateCount;
                    for (const LevelBan& ban : G->LevelBans)
                    {
                        if (!ban.Allowed || ban.Index < 0 || ban.Index >= levels.Count())
                            continue;

                        Level level = levels[ban.Index];
                        if (!level || (levelToAvoid && level == levelToAvoid))
                            continue;

                        if (selectedCandidate-- == 0)
                        {
                            __this.levelCurrent() = level;
                            Hax::Log(G->Logger, L"Map filter: picked %ls (%d candidates, %zu banned)",
                                ban.Name.Data(), candidateCount, G->TotalBans);
                            return;
                        }
                    }
                }
            }
        }
        catch (Exception& ex)
        {
            String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

        s_RerollingRunLevel = false;
        s_RunLevelToAvoid = null;

        Hax::Log(G->Logger, L"Map filter: fell back to the game's random map (%zu levels parsed)", G->LevelBans.Size());
        G->RunManager_SetRunLevel_Hook.unsafe_call<void, RunManager>(__this);
    }

    static void Hooked__PhysGrabber_PhysGrabOverCharge(PhysGrabber __this, float _amount, float _multiplier)
    {
        if (G->NoOvercharge)
            return;

        G->PhysGrabber_PhysGrabOverCharge_Hook.unsafe_call<void, PhysGrabber, float, float>(__this, _amount, _multiplier);
    }

    static void Hooked__SpectateCamera_HeadEnergyLogic(SpectateCamera __this)
    {
        if (G->MaxHeadBattery)
        {
            __this.headEnergy() = 9999.f;
            __this.headEnergyEnough() = true;
        }

        G->SpectateCamera_HeadEnergyLogic_Hook.unsafe_call<void, SpectateCamera>(__this);
    }

    static void Hooked__EnemyRigidbody_FixedUpdate(EnemyRigidbody __this)
    {
        if (G->NoGrabMaxTime && SemiFunc::IsMasterClientOrSingleplayer())
        {
            __this.grabShakeReleaseTimer() = 0.f;
            __this.grabTimeCurrent() = 0.f;
        }

        G->EnemyRigidbody_FixedUpdate_Hook.unsafe_call<void, EnemyRigidbody>(__this);
    }

    static void Hooked__DataDirector_SaveDeleteCheck(DataDirector __this, bool leaveGame)
    {
        if (G->PreserveSaveOnDeath)
        {
            if (RunManager manager = RunManager::instance(); manager && manager.allPlayersDead())
            {
                Hax::Log(G->Logger, L"Save deletion after player death was prevented");
                return;
            }
        }

        G->DataDirector_SaveDeleteCheck_Hook.unsafe_call<void, DataDirector, bool>(__this, leaveGame);
    }

    static bool IsInGame()
    {
        RunManager runManager = RunManager::instance();
        GameDirector gameDirector = GameDirector::instance();
        return runManager && PlayerAvatar::instance() && gameDirector
            && runManager.runStarted()
            && runManager.levelCurrent() != runManager.levelLobbyMenu()
            && runManager.levelCurrent() != runManager.levelMainMenu()
            && gameDirector.currentState() == GameDirector_gameState::Main();
    }

    static void* GetPlayerLoopPtr(System::Type type, System::Type subType)
    {
        auto mainLoop = LowLevel::PlayerLoop::GetDefaultPlayerLoop();
        for (PlayerLoopSystem& loop : mainLoop.m_SubSystemList)
        {
            if (loop.m_Type != type)
                continue;

            for (PlayerLoopSystem& subLoop : loop.m_SubSystemList)
                if (subLoop.m_Type == subType)
                    return *subLoop.m_UpdateFunction;
        }
        return nullptr;
    }

    static void Heal(PhysGrabObjectImpactDetector obj, float value)
    {
        if (SemiFunc::IsMultiplayer())
        {
            Array<System::Object> params = { Single(value), Vector3() };
            obj.photonView().RPC(String::New("HealRPC"), Photon::RpcTarget::All, params);
        }
        else
        {
            obj.HealLogic(value, Vector3::zero());
        }
    }

    static void SpawnItem(Item item)
    {
        Unity::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        if (!item)
            return;

        Unity::Transform transform = camera.GetTransform();
        Unity::Vector3 pos = transform.GetPosition() + transform.GetForward() * 2.f - transform.GetUp();

        if (SemiFunc::IsMasterClient())
            Photon::PhotonNetwork::InstantiateRoomObject(item.prefab().resourcePath(), pos, Unity::Quaternion::identity());
        if (!SemiFunc::IsMultiplayer())
            Unity::Object::Instantiate<Unity::GameObject>(item.prefab().Prefab(), pos, Unity::Quaternion::identity());
    }

    static void SpawnCosmeticBox(int rarity)
    {
        Unity::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        ValuableDirector dir = ValuableDirector::instance();
        if (!dir)
            return;

        auto setups = dir.cosmeticWorldObjectSetups();
        if (setups == null || rarity < 1 || rarity >= setups.Count())
            return;

        CosmeticWorldObjectSetup setup = setups[rarity];
        if (setup == null || setup.prefab() == null)
            return;

        Transform transform = camera.GetTransform();
        Vector3 pos = transform.GetPosition() + transform.GetForward() * 2.f - transform.GetUp();

        if (GameManager::instance().gameMode() == 0)
            Unity::Object::Instantiate<Unity::GameObject>(setup.prefab().Prefab(), pos, Quaternion::identity());
        else
            Photon::PhotonNetwork::InstantiateRoomObject(setup.prefab().resourcePath(), pos, Quaternion::identity());
    }

    static void ParseEnemies()
    {
        EnemyDirector dir = EnemyDirector::instance();
        if (!G->EnemiesPool.Empty() || !dir)
            return;

        Hax::LogDebug(G->Logger, L"Parsing enemies");
        //G->LoadRequests.reserve(50);

        List<EnemySetup> setups[3] = {dir.enemiesDifficulty1(), dir.enemiesDifficulty2(), dir.enemiesDifficulty3()};
        for (size_t i = 0; i < 3; ++i)
        {
            for (EnemySetup setup : setups[i])
            {
                if (!setup)
                    continue;

                String name = setup.GetName();
                if (name.Contains(L"Bang") || name.Contains(L"Gnome"))
                    continue;

                for (PrefabRef ref : setup.spawnObjects())
                {
                    auto req = Unity::Resources::LoadAsync(ref.resourcePath());
                    //G->LoadRequests.push_back(new LoadRequest(setup, req));

                    Hax::WStringView enemyName = ref.prefabName().begin() + 8;

                    EnemySetup& entry = G->EnemiesPool.FindOrAdd(enemyName);
                    if (!entry)
                    {
                        entry = setup;
                        GCHandle::Alloc(name, true);
                        Hax::LogDebug(G->Logger, L"Enemy parsed %ls", enemyName.begin());
                    }
                }
            }
        }
    }

    static void ParseItems()
    {
        if (StatsManager manager = StatsManager::instance())
        {
            auto itemDict = manager.itemDictionary();
            int nItems = itemDict.Count();

            if (itemDict.Count() == 0)
                manager.LoadItemsFromFolder();

            for (auto& entry : itemDict)
            {
                if (!entry.Value)
                    continue;

                Hax::WStringView name = entry.Value.itemName().ToHaxView();

                if (!G->ItemsPool.Contains(name))
                {
                    G->ItemsPool.Insert(name, entry.Value);
                    auto req = Resources::LoadAsync(entry.Value.prefab().resourcePath());

                    Hax::LogDebug(G->Logger, L"Item parsed %s", name.begin());
                }
            }
        }
    }

    static void ParseLevels()
    {
        if (RunManager manager = RunManager::instance())
        {
            Hax::LogDebug(G->Logger, L"Parsing levels");

            System::List<Level> levels = manager.levels();
            G->LevelBans.Reserve(levels.Count());

            for (int i = 0; i < levels.Count(); ++i)
            {
                System::String levelName = levels[i].NarrativeName();

                LevelBan ban = {levelName.ToHaxView(), i, true};
                G->LevelBans.PushBack(ban);

                GCHandle::Alloc(levelName, true);
                Hax::LogDebug(G->Logger, L"Parsed level %s", levelName.begin());
            }

            std::sort(G->LevelBans.begin(), G->LevelBans.end(), [](const LevelBan& o1, const LevelBan& o2) { return o1.Name < o2.Name; });
        }
    }

    static void DrawPlayerChams(PlayerAvatar player, Unity::CommandBuffer cb, Unity::Material aliveMat, Unity::Material deadMat)
    {
        if (cb == null || !aliveMat || !deadMat || !player || player.isLocal())
            return;

        if (player.deadSet())
        {
            PlayerDeathHead head = player.playerDeathHead();
            if (head && head.GetEnabled())
            {
                for (Unity::MeshRenderer r : head.meshRenderers())
                    cb.DrawRenderer(r, deadMat);
            }
            return;
        }

        if (!player.isDisabled())
        {
            for (Unity::MeshRenderer renderer : player.playerHealth().renderers())
            {
                if (renderer && renderer.GetEnabled())
                    cb.DrawRenderer(renderer, aliveMat);
            }
        }
    }
}
