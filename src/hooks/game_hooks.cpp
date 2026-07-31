#include <src/pch.h>

#include <src/cheat.h>
#include <src/game_classes.h>
#include <src/resource.h>
#include <src/overlay/esp/esp.h>

#include <float.h>

using namespace System;
using namespace Unity;

namespace Cheat
{
    extern void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name);
    extern void HookModuleProc(HMODULE module, const char* procName, void* procHook, SafetyHookInline& out);

    static void* Hooked_MonoRuntimeInvoke(void* a1, void* a2, void* a3, void* a4);
    static void Hooked__EventSystem_Update(Unity::EventSystem __this);
    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int damage, bool savingGrace, int enemyIndex);
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
    static void Hooked__PhysGrabObjectImpactDetector_DestroyObject(PhysGrabObjectImpactDetector __this, bool a1);
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
    static void ProcessTeleportCommands(PlayerAvatar avatar);
    static void ProcessObjectRemover(PlayerAvatar avatar);
    static void MaintainFriendEffects();
    static void ProcessGodTools(PlayerAvatar avatar);
    static void DeleteGodObject(Unity::GameObject object, bool networked);
    static void UpdateEyeLasers();
    static void DrawPlayerChams(PlayerAvatar player, Unity::CommandBuffer cb, Unity::Material aliveMat, Unity::Material deadMat);

    static bool s_ForcedPlayerDeath;
    static bool s_ForcedPlayerTumble;

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
        if (!avatar || !manager || (delta != -1 && delta != 1))
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
        G->DeleteObjectRequested = false;
        G->GodObjectAction = GodObjectCommand::None;
        G->EnemyGodAction = EnemyGodCommand::None;
        G->LootGodAction = LootGodCommand::None;
        G->PlayerGodAction = PlayerGodCommand::None;
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
                manager.RestartScene();
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

            UpdateSessionSafety(avatar);
            ProcessWorldSessionCommands(avatar);
            ProcessCurrencyCommands();
            ProcessQuotaCommands();
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

    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int damage, bool savingGrace, int enemyIndex)
    {
        PlayerAvatar localPlayer = PlayerAvatar::instance();
        if (G->GodAllPlayers || (G->Godmode && localPlayer && localPlayer.playerHealth() == __this) || IsProtectedHealth(__this))
            return;

        G->PlayerHealth_Hurt_Hook.unsafe_call<void, PlayerHealth, int, bool, int>(__this, damage, savingGrace, enemyIndex);
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
        RestoreNoclipPhysics();

        try
        {
            if (forceStanding && avatar && avatar.isTumbling() && avatar.tumble())
                avatar.tumble().TumbleSet(false, false);

            if (s_Noclip.PoseCaptured)
            {
                if (controller && controller.GetTransform())
                {
                    controller.GetTransform().SetPosition(s_Noclip.OriginPosition);
                    controller.GetTransform().SetRotation(s_Noclip.OriginRotation);
                    if (Unity::Rigidbody body = controller.GetComponent<Unity::Rigidbody>())
                        body.SetVelocity(Unity::Vector3::zero());
                }

                if (avatar && avatar.GetTransform())
                {
                    avatar.GetTransform().SetPosition(s_Noclip.OriginPosition);
                    avatar.GetTransform().SetRotation(s_Noclip.OriginRotation);
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
        const float padding = 0.08f;
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
                const Unity::Vector3 colliderCenter(
                    areaMin.X + padding + ((float)column + 0.5f) * usableWidth / (float)s_ExtractionPacking.Columns,
                    areaMin.Y + padding + ((float)layer + 0.5f) * usableHeight / (float)s_ExtractionPacking.Layers,
                    areaMin.Z + padding + ((float)row + 0.5f) * usableDepth / (float)s_ExtractionPacking.Rows);

                Unity::Vector3 colliderOffset{};
                Unity::Collider collider = phys ? phys.GetComponentInChildren<Unity::Collider>() : null;
                if (collider && collider.GetEnabled())
                    colliderOffset = collider.GetBounds().Center - transform.GetPosition();
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

    static void ConfigureEyeLaserDamage(SemiLaser laser, PlayerAvatar avatar)
    {
        if (!laser)
            return;

        HurtCollider hurt = laser.hurtCollider();
        if (!hurt)
            hurt = laser.GetComponentInChildren<HurtCollider>();
        if (!hurt)
            return;

        hurt.playerLogic() = false;
        hurt.physLogic() = true;
        hurt.physDestroy() = G->EyeLaserDestroyObjects;
        hurt.physHingeDestroy() = G->EyeLaserDestroyObjects;
        hurt.physReleaseGrab() = true;
        hurt.physImpact() = 3; // HurtCollider.BreakImpact.Heavy
        hurt.physDamageCooldown() = 0.05f;
        hurt.enemyLogic() = true;
        hurt.enemyKill() = G->EyeLaserInstantKill;
        hurt.enemyDamage() = G->EyeLaserDamage;
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
        if (carrier.Object && G->IsInGame && !G->IsClient && SemiFunc::IsMultiplayer())
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
        const Unity::Quaternion& muzzleRotation, PlayerAvatar avatar)
    {
        if (!carrier.Object || !carrier.Phys || !carrier.Gun || !carrier.GunLaser)
            return;
        if (::GetTickCount64() < carrier.ReadyAt)
            return;

        const Unity::Quaternion rootRotation = muzzleRotation * InverseUnitQuaternion(carrier.MuzzleRotationOffset);
        const Unity::Vector3 rootPosition = eye - rootRotation * carrier.MuzzleOffset;
        carrier.Phys.Teleport(rootPosition, rootRotation);

        carrier.Gun.gunRange() = (float)G->EyeLaserRange;
        ConfigureEyeLaserDamage(carrier.GunLaser.semiLaser(), avatar);

        if (carrier.Battery && carrier.Battery.batteryLifeInt() <= 1)
            carrier.Battery.BatteryFullPercentChange(carrier.Battery.batteryBars(), true);

        if (carrier.Gun.stateCurrent() != 3 && carrier.Gun.photonView())
        {
            System::Array<System::Object> parameters = { System::Int32(3) };
            carrier.Gun.photonView().RPC(System::String::New("StateSetRPC"), Unity::Photon::RpcTarget::All, parameters);
        }
    }

    static void UpdateEyeLasers()
    {
        const wchar_t* stage = L"startup";
        try
        {
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

            PlayerAvatar avatar = PlayerAvatar::instance();
            if (!G->EyeLasersEnabled || !G->IsInGame || G->MenuVisible)
            {
                stage = L"network cleanup";
                DestroyNetworkEyeLasers();
                return;
            }

            stage = L"camera pose";
            Unity::Camera camera = SemiFunc::MainCamera();
            if (!IsPlayerUsable(avatar) || !camera)
                return;

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
                UpdateNetworkEyeLaser(s_LeftNetworkEyeLaser, leftEye, rotation, avatar);
                stage = L"right network update";
                UpdateNetworkEyeLaser(s_RightNetworkEyeLaser, rightEye, rotation, avatar);
                return;
            }

            DestroyNetworkEyeLasers();

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

            ConfigureEyeLaserDamage(s_EyeLaserPrefab.Left, avatar);
            ConfigureEyeLaserDamage(s_EyeLaserPrefab.Right, avatar);
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

    static float prevPlane;
    static float prevFov;
    static bool prevOcclusionCulling;
    static bool restoreClipPlane;
    static bool restoreOcclusionCulling;
    static void Hooked__PostLateUpdateLoop()
    {
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
                                Enemy enemy = parent.enemy();
                                if (enemy && parent.Spawned())
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

    static void Hooked__PhysGrabObjectImpactDetector_DestroyObject(PhysGrabObjectImpactDetector __this, bool a1)
    {
        if (G->Unbreakable && __this.isValuable())
            return;

        G->PhysGrabObjectImpactDetector_DestroyObject_Hook.unsafe_call<void, PhysGrabObjectImpactDetector, bool>(__this, a1);
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
                __this.BatteryFullPercentChange(__this.batteryBars(), false);
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
                List<Level> levels = RunManager::instance().levels();

                LevelBan* levelBan = &G->LevelBans[0];
                Level prevLevel = __this.previousRunLevel();

                while (!levelBan->Allowed || ((G->LevelBans.Size() - G->TotalBans) > 1 && prevLevel && levels[levelBan->Index] == prevLevel))
                    levelBan = &G->LevelBans[rand() % G->LevelBans.Size()];

                __this.levelCurrent() = levels[levelBan->Index];
                return;
            }
        }
        catch (Exception& ex)
        {
            String message = ex.Message();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message != null ? message.ToString().GetRawStringData() : L"Exception without message");
        }

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
