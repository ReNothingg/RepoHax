#include <Windows.h>

#include "overlay/menu/localization.h"
#include "overlay/esp/esp.h"
#include "game_classes.h"

#include <third_party/safetyhook/safetyhook.hpp>

namespace Cheat
{
    void Initialize(HMODULE hCheatDll);

    inline constexpr int MaxSafeRunCurrency = 2147483; // 32-bit int.

    enum class UpgradeType : int { Rest, Jump, Range, Speed, Stamina, Strength, Tumble, Wings, Health, Map, Climb, Battery, N };
    enum class AidType { Small, Medium, Large, N };
    enum class ChatPref { None = -1, Alive = 0, Dead = 1 };
    enum class CosmeticRarity { Common, Uncommon, Rare, UltraRare, N };
    enum class PlayerUpgradeType
    {
        Health, Stamina, ExtraJump, MapPlayerCount, Launch, Climb, HeadBattery,
        Wings, SprintSpeed, CrouchRest, GrabStrength, ThrowStrength, GrabRange, N
    };

    enum class TeleportQuickAction
    {
        None,
        ToTruck,
        ToExtraction,
        ToNearestValuable,
        ToSelectedPlayer,
        SelectedPlayerToMe,
        PanicSafe,
        PlayerToCamera
    };

    enum class GodObjectCommand
    {
        None, LockAimed, ClearTarget, ToggleTelekinesis, Pull, Push, FreezeToggle,
        Duplicate, ApplyTransform, DeleteTarget, Undo
    };

    enum class EnemyGodCommand { None, Gather, KillAll, DeleteAll };
    enum class LootGodCommand { None, BringToPlayer, BringToTruck, BringToExtraction, DiscoverAll, ApplyValueMultiplier };
    enum class WorldLightCommand { None, Blackout, FullBright, Restore };
    enum class PlayerGodCommand { None, HealAll, ReviveAll, GatherAll, ApplyScaleAll };
    enum class GodGizmoMode { Move, Rotate, Scale };
    enum class GodGizmoSpace { World, Local };
    inline constexpr int GodGizmoRingSegments = 32;

    struct SavedPositionSlot
    {
        bool                                Active;
        Unity::Vector3                      Position;
        int                                 LevelsCompleted;
    };

    struct UpgradeData
    {
        Hax::Gui::TextureHandle Icon;
        Hax::WStringView InternalNameEnd;
        LocKey LocKey;
        bool AutoUse;
    };

    struct AidData
    {
        Hax::Gui::TextureHandle Icon;
        Hax::WStringView InternalNameEnd;
        LocKey LocKey;
    };

    struct LevelBan
    {
        Hax::WStringView Name;
        int Index;
        bool Allowed = true;
    };

    struct Context
    {
        HMODULE                             Handle;
        HWND                                GameWndHandle;

        Hax::IniFile                        Config{L"haxsdk.ini"};
        Hax::LogFile                        Logger;

        bool                                MenuVisible;

        // Settings
        bool                                UseConsole;
        bool                                DarkenBackground;
        int                                 VkOpenClose = VK_OEM_3;
        Lang                                Language;

        // Stats
        bool                                Godmode;
        bool                                InfStamina;
        bool                                InfJumps;
        bool                                NoTumble;
        bool                                FlightEnabled;
        int                                 VkToggleFlight = VK_F7;
        int                                 FlightSpeed = 10;
        int                                 FlightSprintBoost = 3;
        int                                 VkTeleportPlayerToCamera = VK_F9;
        bool                                EyeLasersEnabled;
        int                                 VkToggleEyeLasers = VK_F8;
        int                                 EyeLaserRange = 80;
        int                                 EyeLaserDamage = 250;
        bool                                EyeLaserInstantKill = true;
        bool                                EyeLaserDestroyObjects = true;
        bool                                EasyGrab;
        bool                                UnlimGrabRange;
        bool                                NoOvercharge;
        bool                                AutoUseUpgrs;
        bool                                HealToMax;
        struct 
        {
            int                             Walking = 1;
            int                             Sprinting = 1;
            int                             Crouching = 1;
        } Accel;
        ItemUpgrade                         UpgradeToToggle = null;
        ItemHealthPack                      AidToToggle = null;
        Hax::TripleBuffer<ItemHealthPack>   AidsPool{24};
        Hax::TripleBuffer<ItemUpgrade>      UpgradesPool{24};
        int                                 PlayerUpgradeLevels[(int)PlayerUpgradeType::N]{};
        PlayerUpgradeType                   PlayerUpgradeToChange = PlayerUpgradeType::N;
        int                                 PlayerUpgradeDelta;

        UpgradeData UpgradesData[(int)UpgradeType::N + 1] =
        {
            { .InternalNameEnd = L"Rest",     .LocKey = LocKey_CrouchRest },
            { .InternalNameEnd = L"Jump",     .LocKey = LocKey_ExtraJump },
            { .InternalNameEnd = L"Range",    .LocKey = LocKey_Range },
            { .InternalNameEnd = L"Speed",    .LocKey = LocKey_SprintSpeed },
            { .InternalNameEnd = L"Energy",   .LocKey = LocKey_Stamina },
            { .InternalNameEnd = L"Strength", .LocKey = LocKey_Strength },
            { .InternalNameEnd = L"Launch",   .LocKey = LocKey_Launch },
            { .InternalNameEnd = L"Wings",    .LocKey = LocKey_Wings },
            { .InternalNameEnd = L"Health",   .LocKey = LocKey_Health },
            { .InternalNameEnd = L"Count",    .LocKey = LocKey_PlayersCount },
            { .InternalNameEnd = L"Climb",    .LocKey = LocKey_Climb },
            { .InternalNameEnd = L"Battery",  .LocKey = LocKey_HeadBattery },
            { .InternalNameEnd = L"",         .LocKey = LocKey_Unknown }
        };

        AidData AidsData[(int)AidType::N + 1] =
        {
            { .InternalNameEnd = L"Small",    .LocKey = LocKey_Small },
            { .InternalNameEnd = L"Medium",   .LocKey = LocKey_Medium },
            { .InternalNameEnd = L"Large",    .LocKey = LocKey_Large },
            { .InternalNameEnd = L"",         .LocKey = LocKey_Unknown }
        };

        // Entities
        bool                                NoGrabMaxTime;
        bool                                EnemiesEsp;
        bool                                PlayersEsp;
        bool                                PlayersChams;
        EnemySetup                          EnemyToSpawn = null;
        PlayerAvatar                        PlayerToTumble = null;
        PlayerAvatar                        PlayerToRevive = null;
        PlayerAvatar                        PlayerToKill = null;
        PlayerAvatar                        PlayerToHeal = null;
        PlayerAvatar                        ProtectedPlayers[16]{};
        PlayerAvatar                        NoTumblePlayers[16]{};
        ChatPref                            ChatPrefered = ChatPref::None;
        size_t SelectedEnemySetup;

        // Vision
        bool                                BetterVision;
        bool                                NoFog;
        bool                                DisableOcclusionCulling;
        int                                 RenderDistance = 200;
        bool                                ThirdPerson;
        bool                                MaxHeadBattery;
        int                                 Fov = 60;
        struct
        {
            int                             Intensity = 10;
            int                             Angle = 60;
            bool                            InCrouch;
        } Flashlight;
        Unity::CommandBuffer          CommandBuffer = null;
        Unity::AssetBundle            Bundle = null;
        Unity::Material               ItemsChamsMat = null;
        Unity::Material               PlayerChamsMat = null;
        Unity::Material             HeadChamsMat = null;

        // Valuables
        bool                                ValuablesEsp;
        bool                                ValuablesChams;
        bool                                Unbreakable; // TODO: rename
        bool                                ExtrPointsEsp;
        bool                                SetToZero;
        bool                                SetToMax;
        bool                                ActivateNextPoint;
        int                                 ValuablesEspRange = 50;

        // Items
        bool                                InfBattery;
        bool                                UseLaser;
        int                                 VkSpawnItem = VK_F6;
        Item                                SelectedItem = null;
        Item                                ItemToSpawn = null;
        bool                                ObjectRemoverEnabled;
        int                                 VkDeleteObject = VK_F10;
        int                                 ObjectRemoverRange = 25;
        bool                                DeleteObjectRequested;
        bool                                ObjectRemoverTargetValid;
        float                               ObjectRemoverTargetDistance;

        // God tools / runtime object editor
        bool                                GodTargetingEnabled;
        int                                 VkLockGodTarget = VK_F11;
        Unity::GameObject                   GodAimedObject = null;
        bool                                GodAimedObjectValid;
        bool                                GodAimedObjectNetworked;
        float                               GodAimedObjectDistance;
        Hax::char16                         GodAimedObjectName[96] = L"-";
        Unity::GameObject                   GodTargetObject = null;
        bool                                GodTargetObjectValid;
        bool                                GodTargetNetworked;
        Hax::char16                         GodTargetName[96] = L"-";
        Unity::Vector3                      GodTargetPosition;
        int                                 GodTargetLayer;
        GodObjectCommand                    GodObjectAction = GodObjectCommand::None;
        bool                                GodTelekinesisActive;
        int                                 GodHoldDistance = 5;
        int                                 GodThrowForce = 35;
        int                                 GodScalePercent = 100;
        int                                 GodScaleXPercent = 100;
        int                                 GodScaleYPercent = 100;
        int                                 GodScaleZPercent = 100;
        int                                 GodRotationX;
        int                                 GodRotationY;
        int                                 GodRotationZ;
        int                                 GodUndoCount;

        // Unity-style transform gizmo. Unity calls stay on the game thread; the overlay
        // consumes only these projected POD snapshots and submits drag deltas back.
        bool                                GodGizmoVisible = true;
        bool                                GodGizmoEditMode;
        GodGizmoMode                        GodGizmoModeCurrent = GodGizmoMode::Move;
        GodGizmoSpace                       GodGizmoSpaceCurrent = GodGizmoSpace::Local;
        bool                                GodGizmoProjectionValid;
        Hax::Vector2                        GodGizmoScreenOrigin{};
        Hax::Vector2                        GodGizmoScreenAxes[3]{};
        bool                                GodGizmoScreenAxisValid[3]{};
        Hax::Vector2                        GodGizmoRingPoints[3][GodGizmoRingSegments + 1]{};
        bool                                GodGizmoRingPointValid[3][GodGizmoRingSegments + 1]{};
        float                               GodGizmoWorldSize;
        int                                 GodGizmoHoveredAxis = -1;
        int                                 GodGizmoActiveAxis = -1;
        Hax::Vector2                        GodGizmoDragScreenDirection{};
        float                               GodGizmoDragPixelsPending;
        bool                                GodGizmoDragBeginRequested;
        bool                                GodGizmoDragEndRequested;

        // World control
        int                                 WorldTimeScalePercent = 100;
        bool                                WorldGravityOverride;
        int                                 WorldGravityY = -10;
        bool                                WorldFreezePhysics;
        bool                                WorldFreezePhysicsChanged;
        WorldLightCommand                   WorldLightAction = WorldLightCommand::None;

        // Enemy director
        bool                                GodEnemiesPacified;
        bool                                GodEnemiesPacifiedChanged;
        bool                                GodEnemiesFrozen;
        bool                                GodEnemiesFreezeChanged;
        EnemyGodCommand                     EnemyGodAction = EnemyGodCommand::None;

        // Loot control
        bool                                GodLootFrozen;
        bool                                GodLootFreezeChanged;
        int                                 GodLootValuePercent = 100;
        LootGodCommand                      LootGodAction = LootGodCommand::None;

        // Team-wide player control
        bool                                GodAllPlayers;
        bool                                GodAllPlayersNoTumble;
        int                                 GodAllPlayerScalePercent = 100;
        PlayerGodCommand                    PlayerGodAction = PlayerGodCommand::None;

        // Levels
        Hax::Vector<LevelBan>               LevelBans;
        size_t                              TotalBans;
        bool                                ForceNextLevel;

        // Cosmetics
        bool                                UnlockAllCosmetic;
        bool                                ResetAllCosmetic;
        bool                                CosmeticBoxesEsp;
        int                                 RarityToSpawn;

        // Misc
        bool                                TruckEsp;
        bool                                TeleportToTruck;
        bool                                PreserveSaveOnDeath = true;
        bool                                SessionSafetyEnabled = true;
        bool                                AutoCancelClientUnsafe = true;
        bool                                DisableAllPlayersDeadCheck;
        bool                                ResetSessionSafety;
        bool                                SaveWorldNow;
        bool                                ReloadCurrentLevel;
        bool                                UnlockExtractionPoints;
        int                                 CurrentRunCurrency;
        int                                 CurrencyChangeAmount = 1000;
        int                                 CurrencyDeltaPending;
        bool                                CurrencySetZero;
        bool                                CurrencyRepairOverflow;
        int                                 CurrentQuota;
        int                                 QuotaTarget = 10000;
        bool                                QuotaApplyRequested;
        TeleportQuickAction                 TeleportAction = TeleportQuickAction::None;
        PlayerAvatar                        SelectedTeleportPlayer = null;
        int                                 SavePositionSlot = -1;
        int                                 TeleportSavedPositionSlot = -1;
        int                                 ClearSavedPositionSlot = -1;
        SavedPositionSlot                   SavedPositions[8]{};
        Hax::char16                         SessionLastAction[128] = L"Idle";

        // Events
        HANDLE                              UnityLoadedEvent;

        // WinHooks
        bool                                ForceCursorVisible;
        bool                                OrigCursorVisible;
        bool                                ForceCursorUnclip;
        bool                                OrigCursorUnclip;
        bool                                GameInputPrevented;
        bool                                ForceCustomCursorTex;
        Hax::Optional<POINT>                LockedCursorPos;
        Hax::Optional<RECT>                 OrigClipArea;
        HCURSOR                             OrigMouseTexture;

        // Graphics
        void*                               PresentOrig;
        void*                               ResizeBuffersOrig;
        ID3D11RenderTargetView*             RenderTarget;
        ID3D11Device*                       Device;
        ID3D11DeviceContext*                DeviceContext;

        // Overlay
        Hax::Gui::FontHandle                NunitoSans_SemiBold;
        Hax::Gui::FontHandle                NunitoSans_Bold;
        Hax::Gui::FontHandle                NunitoSans_ExtraBold;
        Hax::Gui::FontHandle                Icons_Solid;
        Hax::Gui::TextureHandle             Logo;
        size_t                              KeyListenerId;

        struct
        {
            bool                            ReadyToHook;
            void*                           UpdatePtr;
            void*                           PostLateUpdatePtr;
            void*                           PresentAfterDrawPtr;
        } PlayerLoop;

        // Esp
        Hax::TripleBuffer<EnemyEspData>     EnemiesEspBuffer{20};
        Hax::TripleBuffer<ValuableEspData>  ValuablesEspBuffer{50};
        Hax::TripleBuffer<ExtrPointEspData> ExtrPointsEspBuffer{10};
        Hax::TripleBuffer<TruckEspData>     TruckEspBuffer{2};
        Hax::TripleBuffer<PlayerEspData>    PlayersEspBuffer{6};
        Hax::TripleBuffer<CosmeticBoxEspData> CosmeticBoxesEspBuffer{20};


        // Misc
        Hax::Handle                         UvmThread;
        const Hax::WStringView*             Loc = g_LocDict[0];
        bool                                IsClient;
        bool                                IsInGame;
        Hax::Map<Hax::WStringView, EnemySetup> EnemiesPool;
        Hax::Map<Hax::WStringView, Item>     ItemsPool;
        float PixelHeight;
        float PixelWidth;
        float ScreenHeight;
        float ScreenWidth;


        // Hooks
        SafetyHookInline                    MonoRuntimeInvokeHook;

        SafetyHookInline                    SetCursorHook;
        SafetyHookInline                    GetRawInputDataHook;
        SafetyHookInline                    GetCursorPosHook;
        SafetyHookInline                    ShowCursorHook;
        SafetyHookInline                    ClipCursorHook;
        SafetyHookInline                    SetCursorPosHook;
        SafetyHookInline                    GetAsyncKeyStateHook;
        SafetyHookInline                    GetKeyStateHook;
        SafetyHookInline                    GetKeyboardStateHook;
        SafetyHookInline                    PeekMessageAHook;
        SafetyHookInline                    PeekMessageWHook;
        SafetyHookInline                    GetMessageAHook;
        SafetyHookInline                    GetMessageWHook;
        SafetyHookInline                    TerminateProcessHook;

        SafetyHookInline                    Application_Quit_Hook;
        SafetyHookInline                    EventSystem_Update_Hook;
        SafetyHookInline                    PlayerHealth_Hurt_Hook;
        SafetyHookInline                    PlayerAvatar_PlayerDeath_Hook;
        SafetyHookInline                    PlayerController_FixedUpdate_Hook;
        SafetyHookInline                    PlayerController_Update_Hook;
        SafetyHookInline                    PlayerTumble_TumbleRequest_Hook;
        SafetyHookInline                    FlashlightController_Update_Hook;
        SafetyHookInline                    ItemUpgrade_Update_Hook;
        SafetyHookInline                    ItemHealthPack_Update_Hook;
        SafetyHookInline                    UpdateLoop_Hook;
        SafetyHookInline                    PostLateUpdateLoop_Hook;
        SafetyHookInline                    PresentAfterDrawLoop_Hook;
        SafetyHookInline                    PhysGrabObject_PhysicsGrabbingManipulation_Hook;
        SafetyHookInline                    PostProcessLayer_BuildCommandBuffers_Hook;
        SafetyHookInline                    PlayerAvatarVisuals_Update_Hook;
        SafetyHookInline                    PhysGrabber_PhysGrabLogic_Hook;
        SafetyHookInline                    PhysGrabber_RayCheck_Hook;
        SafetyHookInline                    Physics_Raycast_Hook;
        SafetyHookInline                    PhysGrabObjectImpactDetector_Break_Hook;
        SafetyHookInline                    PhysGrabObjectImpactDetector_DestroyObject_Hook;
        SafetyHookInline                    PhotonNetwork_IsMasterClient_Hook;
        SafetyHookInline                    ItemBattery_Update_Hook;
        SafetyHookInline                    Camera_FireOnPreRender_Hook;
        SafetyHookInline                    ItemGun_Update_Hook;
        SafetyHookInline                    RunManager_SetRunLevel_Hook;
        SafetyHookInline                    PhysGrabber_PhysGrabOverCharge_Hook;
        SafetyHookInline                    SpectateCamera_HeadEnergyLogic_Hook;
        SafetyHookInline                    EnemyRigidbody_FixedUpdate_Hook;
        SafetyHookInline                    DataDirector_SaveDeleteCheck_Hook;

        SafetyHookInline                    UpdateHook;
        SafetyHookInline                    PostLateUpdateHook;
        SafetyHookInline                    PresentAfterDrawHook;
    };

    extern Context* G;
}
