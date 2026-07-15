#pragma once

#include <third_party/haxsdk/hax.h>

#define LOC_LIST(X) \
X(LocKey_Stats, L"Stats", L"Статы") \
X(LocKey_Entities, L"Entities", L"Существа") \
X(LocKey_Vision, L"Vision", L"Зрение") \
X(LocKey_Valuables, L"Valuables", L"Ценности") \
X(LocKey_Items, L"Items", L"Предметы") \
X(LocKey_Level, L"Level", L"Уровень") \
X(LocKey_Misc, L"Miscellaneous", L"Разное")\
X(LocKey_Settings, L"Settings", L"Настройки")\
X(LocKey_PlayerCategory, L"Player", L"Игрок") \
X(LocKey_WorldCategory, L"World", L"Мир") \
X(LocKey_ToolsCategory, L"Tools", L"Инструменты") \
X(LocKey_Navigation, L"NAVIGATION", L"НАВИГАЦИЯ") \
X(LocKey_HEALTH, L"HEALTH", L"ЗДОРОВЬЕ") \
X(LocKey_Godmode, L"Godmode", L"Бессмертие") \
X(LocKey_HealToMax, L"Heal to max", L"Исцелиться на максимум") \
X(LocKey_MOVEMENT, L"MOVEMENT", L"ПЕРЕДВИЖЕНИЕ") \
X(LocKey_InfiniteStamina, L"Infinite stamina", L"Стамина не кончается") \
X(LocKey_Default, L"Default", L"Обычная") \
X(LocKey_WalkingSpeed, L"Walking speed", L"Скорость ходьбы") \
X(LocKey_RunningSpeed, L"Running speed", L"Скорость бега") \
X(LocKey_CrouchingSpeed, L"Crouching speed", L"Скорость ползания") \
X(LocKey_InfiniteJumps, L"Infinite jumps", L"Бесконечные прыжки") \
X(LocKey_Flight, L"Noclip", L"Ноклип") \
X(LocKey_FlightSpeed, L"Noclip speed", L"Скорость ноклипа") \
X(LocKey_ToggleFlight, L"Toggle noclip", L"Включить / выключить ноклип") \
X(LocKey_FlightControls, L"WASD, Space / Ctrl; hold Sprint for 3x speed", L"WASD, Пробел / Ctrl; удерживайте бег для скорости x3") \
X(LocKey_DontTumble, L"Dont tumble", L"Не падать") \
X(LocKey_DisableTumbling, L"Disable tumbling from external sources", L"Отключает падение от внешних источников") \
X(LocKey_EYELASERS, L"EYE LASERS", L"ЛАЗЕРЫ ИЗ ГЛАЗ") \
X(LocKey_EyeLasers, L"Deadly eye lasers", L"Смертельные лазеры") \
X(LocKey_EyeLasersDesc, L"Eye lasers; damage is host-only", L"Лазеры из глаз; урон только у хоста") \
X(LocKey_EyeLaserRange, L"Laser range", L"Дальность лазеров") \
X(LocKey_EyeLaserDamage, L"Damage without instant kill", L"Урон без мгновенного убийства") \
X(LocKey_EyeLaserInstantKill, L"Instantly kill enemies", L"Мгновенно убивать врагов") \
X(LocKey_EyeLaserDestroyObjects, L"Destroy objects", L"Разрушать предметы") \
X(LocKey_ToggleEyeLasers, L"Toggle eye lasers", L"Включить / выключить лазеры") \
X(LocKey_GRAB, L"GRAB", L"ЗАХВАТ") \
X(LocKey_HighStrength, L"High strength", L"Большая сила") \
X(LocKey_AvailableIfHost, L"Available if you are host", L"Доступно если вы хост") \
X(LocKey_UnlimitedRange, L"Unlimited range", L"Неограниченная дальность") \
X(LocKey_NoOvercharge, L"No overcharge", L"Нет перегрузки") \
X(LocKey_POWERUPS, L"POWERUPS", L"УЛУЧШЕНИЯ") \
X(LocKey_PlayerUpgrades, L"PLAYER UPGRADE LEVELS", L"УРОВНИ УЛУЧШЕНИЙ ИГРОКА") \
X(LocKey_AutoApplyUpgrades, L"Automatically apply upgrades", L"Автоматически применять улучшения") \
X(LocKey_ManualApplication, L"Manual application", L"Ручное применение") \
X(LocKey_HEALTHPACKS, L"HEALTH PACKS", L"АПТЕЧКИ") \
X(LocKey_ApplyAidKit, L"Apply an available aid kit to yourself", L"Применить на себя доступную аптечку") \
X(LocKey_VIEW, L"VIEW", L"ОБЗОР") \
X(LocKey_BetterVision, L"Better vision", L"Улучшенное зрение") \
X(LocKey_NoFog, L"Disable fog", L"Отключить туман") \
X(LocKey_NoFogDesc, L"Remove environmental distance fog", L"Убрать окружающий туман, ограничивающий видимость") \
X(LocKey_DisableOcclusion, L"Disable occlusion culling", L"Отключить occlusion culling") \
X(LocKey_DisableOcclusionDesc, L"Render geometry hidden by camera occlusion", L"Отображать геометрию, скрываемую камерой") \
X(LocKey_RenderDistance, L"Render distance", L"Дальность отрисовки") \
X(LocKey_Meters, L"%d m", L"%d м") \
X(LocKey_ThirdPerson, L"Third person", L"Третье лицо") \
X(LocKey_DefaultView, L"Default", L"Обычное") \
X(LocKey_Degrees, L"%d degrees", L"%d градусов") \
X(LocKey_FieldOfView, L"Field of view", L"Поле зрения") \
X(LocKey_FLASHLIGHT, L"FLASHLIGHT", L"ФОНАРИК") \
X(LocKey_LightIntensity, L"Light intensity", L"Яркость света") \
X(LocKey_IlluminationAngle, L"Illumination angle", L"Угол освещения") \
X(LocKey_WorksWhileCrouching, L"Works while crouching", L"Работает в приседе") \
X(LocKey_DEADHEAD, L"DEAD HEAD", L"МЕРТВАЯ ГОЛОВА") \
X(LocKey_MaxBatteryHead, L"Max battery", L"Полный заряд") \
X(LocKey_VISUALS, L"VISUALS", L"ВИЗУАЛ") \
X(LocKey_DisplayThroughWalls, L"Display through walls", L"Отображать через стены") \
X(LocKey_DisplayBoxesThroughWalls, L"Display boxes through walls", L"Отображать ящики через стены") \
X(LocKey_WithinSelectedDistance, L"Within selected distance", L"В пределах выбранной дистанции") \
X(LocKey_DisplayDistance, L"Display distance", L"Дистанция отображения") \
X(LocKey_XrayHighlight, L"X-ray hightlight", L"Рентген подсветка") \
X(LocKey_NotAllHighlighted, L"Some objects are not highlighted", L"Некоторые предметы не подсвечиваются") \
X(LocKey_VALUE, L"VALUE", L"ЦЕНА") \
X(LocKey_NoDamageValuables, L"Valuables dont get damage", L"Ценности не получают повреждений") \
X(LocKey_ChangeValue, L"Change value of all valuables:", L"Изменить цену всех ценностей:") \
X(LocKey_ToZero, L"To zero", L"На ноль") \
X(LocKey_ToMax, L"To max", L"На максимальную") \
X(LocKey_EXTRACTION, L"EXTRACTION POINTS", L"ТОЧКИ СБОРА") \
X(LocKey_ActivateNext, L"Activate next", L"Активировать следующую точку") \
X(LocKey_ENEMIES, L"ENEMIES", L"ВРАГИ") \
X(LocKey_Spawn, L"Spawn", L"Заспавить") \
X(LocKey_SpawnAlt, L"Spawn", L"Заспавнить") \
X(LocKey_CantSeePlayers, L"Cant see players", L"Не видят игроков") \
X(LocKey_NoGrabLimit, L"No grab time limitation", L"Можно держать вечно") \
X(LocKey_PLAYERS, L"PLAYERS", L"ИГРОКИ") \
X(LocKey_Tumble, L"Tumble", L"Уронить") \
X(LocKey_Kill, L"Kill", L"Убить") \
X(LocKey_HostOnly, L"Host only", L"Только хост") \
X(LocKey_Revive, L"Revive", L"Воскресить") \
X(LocKey_VoiceChat, L"Voice chat", L"Голосовой чат") \
X(LocKey_Alive, L"Alive", L"Живых") \
X(LocKey_Dead, L"Dead", L"Мертвых") \
X(LocKey_GENERAL, L"GENERAL", L"ОБЩЕЕ") \
X(LocKey_LevelsCompleted, L"Number of levels completed", L"Количество пройденных уровней") \
X(LocKey_NextLevel, L"Go next level", L"Перейти на следующий уровень") \
X(LocKey_MAPFILTER, L"MAP FILTER", L"ФИЛЬТР КАРТ") \
X(LocKey_MapFilterDesc, L"Select maps that will not be set. Available if you are host", L"Выберите карты которые не будут выпадать. Доступно если вы хост") \
X(LocKey_GUNS, L"GUNS", L"ОРУЖИЕ") \
X(LocKey_MaxBatteryGun, L"Max battery", L"Максимальная батарея") \
X(LocKey_LaserCrosshair, L"Laser crosshair", L"Лазерный прицел") \
X(LocKey_SPAWN, L"SPAWN", L"СПАВН") \
X(LocKey_TRUCK, L"TRUCK", L"ФУРГОН") \
X(LocKey_TeleportTruck, L"Teleport to truck", L"Телепортироваться в фургон") \
X(LocKey_SAVE, L"SAVE PROTECTION", L"ЗАЩИТА СОХРАНЕНИЯ") \
X(LocKey_PreserveSaveOnDeath, L"Keep save after death", L"Сохранять save после смерти") \
X(LocKey_PreserveSaveOnDeathDesc, L"Prevent automatic save deletion after the whole team dies", L"Не удалять save после гибели всей команды") \
X(LocKey_TeleportPosition, L"TELEPORT / POSITION", L"ТЕЛЕПОРТ / ПОЗИЦИЯ") \
X(LocKey_TeleportPositionDesc, L"Quick movement tools and saved slots", L"Быстрые телепорты и сохранённые точки") \
X(LocKey_TruckShort, L"Truck", L"Фургон") \
X(LocKey_ExtractionShort, L"Extraction", L"Точка сбора") \
X(LocKey_NearestValuable, L"Nearest valuable", L"Ближайшая ценность") \
X(LocKey_PanicSafe, L"Panic safe", L"Безопасная точка") \
X(LocKey_PlayerTarget, L"Player target", L"Целевой игрок") \
X(LocKey_GoToPlayer, L"Go to player", L"К игроку") \
X(LocKey_BringToMe, L"Bring to me", L"Ко мне") \
X(LocKey_PlayerToCamera, L"Player to camera", L"Игрок к камере") \
X(LocKey_SavedPositions, L"SAVED POSITIONS", L"СОХРАНЁННЫЕ ПОЗИЦИИ") \
X(LocKey_SavedPositionsDesc, L"Runtime slots; cleared after game restart", L"Временные слоты; очищаются после перезапуска игры") \
X(LocKey_PositionSlotFmt, L"Slot %d  [L%d]  %.1f, %.1f, %.1f", L"Слот %d  [У%d]  %.1f, %.1f, %.1f") \
X(LocKey_PositionSlotEmptyFmt, L"Slot %d  empty", L"Слот %d  пусто") \
X(LocKey_SaveButton, L"Save", L"Сохранить") \
X(LocKey_GoButton, L"Go", L"Перейти") \
X(LocKey_ClearButton, L"Clear", L"Очистить") \
X(LocKey_SessionStatus, L"SESSION STATUS", L"СТАТУС СЕССИИ") \
X(LocKey_InGame, L"In game", L"В игре") \
X(LocKey_Authority, L"Authority", L"Права") \
X(LocKey_LevelName, L"Level", L"Уровень") \
X(LocKey_LastAction, L"Last action", L"Последнее действие") \
X(LocKey_Yes, L"yes", L"да") \
X(LocKey_No, L"no", L"нет") \
X(LocKey_Client, L"client", L"клиент") \
X(LocKey_HostSingleplayer, L"host / singleplayer", L"хост / одиночная игра") \
X(LocKey_Idle, L"Idle", L"Ожидание") \
X(LocKey_SafetyGuards, L"SAFETY GUARDS", L"ЗАЩИТА СЕССИИ") \
X(LocKey_SafetyGuardsDesc, L"Guards that prevent stuck sessions and unsafe client-side calls", L"Защита от зависших состояний и опасных клиентских действий") \
X(LocKey_SessionSafety, L"Session safety", L"Защита сессии") \
X(LocKey_SessionSafetyDesc, L"Auto-disables movement tools when player/game is not ready", L"Автоматически отключает инструменты движения, если игрок или игра не готовы") \
X(LocKey_CancelHostOnlyClient, L"Cancel host-only actions on client", L"Отменять host-only действия на клиенте") \
X(LocKey_CancelHostOnlyClientDesc, L"Prevents queued host-only actions from firing without authority", L"Не даёт выполнять host-only действия без нужных прав") \
X(LocKey_BlockAllDeadGameover, L"Block all-dead gameover", L"Блокировать gameover при смерти всех") \
X(LocKey_BlockAllDeadGameoverDesc, L"Host/singleplayer safety switch; reset it after testing", L"Защитный переключатель для хоста/одиночной игры; сбросьте после теста") \
X(LocKey_ResetRiskyToggles, L"Reset risky toggles", L"Сбросить рискованные функции") \
X(LocKey_ResetRiskyTogglesDesc, L"Turns off noclip/visual overrides and clears queued commands", L"Отключает noclip/визуальные переопределения и очищает очередь команд") \
X(LocKey_QuickSessionActions, L"QUICK SESSION ACTIONS", L"БЫСТРЫЕ ДЕЙСТВИЯ СЕССИИ") \
X(LocKey_BALANCE, L"RUN BALANCE", L"БАЛАНС ЗАБЕГА") \
X(LocKey_BalanceDesc, L"Host changes are synchronized and saved immediately", L"Изменения хоста синхронизируются и сразу сохраняются") \
X(LocKey_CurrentBalance, L"Current balance", L"Текущий баланс") \
X(LocKey_CurrencyChangeAmount, L"Change amount", L"Сумма изменения") \
X(LocKey_SubtractMoney, L"Subtract", L"Убавить") \
X(LocKey_AddMoney, L"Add", L"Добавить") \
X(LocKey_SetBalanceZero, L"Set balance to zero", L"Обнулить баланс") \
X(LocKey_ActionCurrencyChangedFmt, L"Balance changed: $%dK", L"Баланс изменён: $%dK") \
X(LocKey_ActionCurrencyHostOnly, L"Balance change skipped: host only", L"Изменение баланса пропущено: только хост") \
X(LocKey_ActionCurrencyNotReady, L"Balance change failed: run stats are not ready", L"Баланс не изменён: статистика забега не готова") \
X(LocKey_SaveNow, L"Save now", L"Сохранить сейчас") \
X(LocKey_ReloadLevel, L"Reload level", L"Перезагрузить уровень") \
X(LocKey_UnlockExtractionPoints, L"Unlock extraction points", L"Открыть точки сбора") \
X(LocKey_WorldSessionStatus, L"WORLD / SESSION STATUS", L"СТАТУС МИРА / СЕССИИ") \
X(LocKey_WorldSessionTools, L"WORLD / SESSION TOOLS", L"ИНСТРУМЕНТЫ МИРА / СЕССИИ") \
X(LocKey_WorldSessionToolsDesc, L"Host-only actions are disabled for clients", L"Host-only действия отключены для клиентов") \
X(LocKey_LevelsCompletedShort, L"Levels completed", L"Пройдено уровней") \
X(LocKey_Modules, L"Modules", L"Модули") \
X(LocKey_ActiveEnemies, L"Active enemies", L"Активные враги") \
X(LocKey_ValuablesCount, L"Valuables", L"Ценности") \
X(LocKey_ExtractionPointsCount, L"Extraction points", L"Точки сбора") \
X(LocKey_SaveFile, L"Save file", L"Файл сохранения") \
X(LocKey_SprintBoost, L"Sprint boost", L"Ускорение при беге") \
X(LocKey_ActionSafetyReset, L"Safety reset applied", L"Защитный сброс применён") \
X(LocKey_ActionUnsafeCancelled, L"Unsafe host-only action cancelled on client", L"Опасное host-only действие отменено на клиенте") \
X(LocKey_ActionSaveDone, L"Save completed", L"Сохранение выполнено") \
X(LocKey_ActionSaveFailedStats, L"Save failed: StatsManager is not ready", L"Сохранение не выполнено: StatsManager не готов") \
X(LocKey_ActionReloadNotInGame, L"Reload skipped: not in game", L"Перезагрузка пропущена: вы не в игре") \
X(LocKey_ActionReloadHostOnly, L"Reload skipped: host only", L"Перезагрузка пропущена: только хост") \
X(LocKey_ActionReloadRequested, L"Level reload requested", L"Перезагрузка уровня запрошена") \
X(LocKey_ActionExtractionNotReady, L"Extraction unlock skipped: round is not ready", L"Открытие точек сбора пропущено: раунд не готов") \
X(LocKey_ActionExtractionHostOnly, L"Extraction unlock skipped: host only", L"Открытие точек сбора пропущено: только хост") \
X(LocKey_ActionExtractionUnlocked, L"Extraction points unlocked", L"Точки сбора открыты") \
X(LocKey_ActionPositionSlotClearedFmt, L"Position slot %d cleared", L"Слот позиции %d очищен") \
X(LocKey_ActionPositionSlotSavedFmt, L"Position saved to slot %d", L"Позиция сохранена в слот %d") \
X(LocKey_ActionPositionSaveFailed, L"Position save failed: player is not ready", L"Позиция не сохранена: игрок не готов") \
X(LocKey_ActionTeleportedSlotFmt, L"Teleported to saved slot %d", L"Телепорт в сохранённый слот %d") \
X(LocKey_ActionTeleportedSlotFailed, L"Saved slot teleport failed", L"Телепорт в сохранённый слот не выполнен") \
X(LocKey_ActionTeleportPlayerNotReady, L"Teleport skipped: local player is not ready", L"Телепорт пропущен: локальный игрок не готов") \
X(LocKey_ActionTeleportedTruck, L"Teleported to truck", L"Телепорт в фургон выполнен") \
X(LocKey_ActionTeleportedTruckFailed, L"Truck teleport failed", L"Телепорт в фургон не выполнен") \
X(LocKey_ActionTeleportedExtraction, L"Teleported to extraction point", L"Телепорт к точке сбора выполнен") \
X(LocKey_ActionTeleportedExtractionFailed, L"Extraction teleport failed", L"Телепорт к точке сбора не выполнен") \
X(LocKey_ActionTeleportedValuable, L"Teleported to nearest valuable", L"Телепорт к ближайшей ценности выполнен") \
X(LocKey_ActionTeleportedValuableFailed, L"Valuable teleport failed", L"Телепорт к ценности не выполнен") \
X(LocKey_ActionTeleportedPlayer, L"Teleported to selected player", L"Телепорт к выбранному игроку выполнен") \
X(LocKey_ActionTeleportedPlayerFailed, L"Player teleport failed", L"Телепорт к игроку не выполнен") \
X(LocKey_ActionBringPlayerHostOnly, L"Player-to-me skipped: host only", L"Перемещение игрока к вам пропущено: только хост") \
X(LocKey_ActionBringPlayerDone, L"Selected player teleported to you", L"Выбранный игрок телепортирован к вам") \
X(LocKey_ActionBringPlayerFailed, L"Player-to-me failed", L"Перемещение игрока к вам не выполнено") \
X(LocKey_ActionPanicDone, L"Panic teleport completed", L"Аварийный телепорт выполнен") \
X(LocKey_ActionPanicFailed, L"Panic teleport failed", L"Аварийный телепорт не выполнен") \
X(LocKey_ActionPlayerToCameraDone, L"Player moved to camera", L"Игрок перемещён к камере") \
X(LocKey_ActionCameraTeleportFailed, L"Camera teleport failed", L"Телепорт к камере не выполнен") \
X(LocKey_ActionCameraNotReady, L"Camera teleport failed: camera is not ready", L"Телепорт к камере не выполнен: камера не готова") \
X(LocKey_Cosmetic, L"Cosmetic", L"Косметика") \
X(LocKey_Common, L"Common", L"Обычная") \
X(LocKey_Uncommon, L"Uncommon", L"Необычная") \
X(LocKey_Rare, L"Rare", L"Редкая") \
X(LocKey_UltraRare, L"Ultra rare", L"Очень редкая") \
X(LocKey_SETTINGS, L"SETTINGS", L"НАСТРОЙКИ") \
X(LocKey_Language, L"Language", L"Язык") \
X(LocKey_Hotkey, L"Hotkey", L"Горячая клавиша") \
X(LocKey_OpenCloseMenu, L"Open / close menu", L"Открыть / закрыть меню") \
X(LocKey_DarkenBackground, L"Darken the screen behind the menu", L"Затемнять экрана позади меню")\
X(LocKey_CrouchRest, L"Crouch rest", L"Восст. в приседе") \
X(LocKey_ExtraJump, L"Extra jump", L"Доп. прыжок") \
X(LocKey_Range, L"Range", L"Дальность") \
X(LocKey_SprintSpeed, L"Sprint speed", L"Скорость бега") \
X(LocKey_Stamina, L"Stamina", L"Выносливость") \
X(LocKey_Strength, L"Strength", L"Сила") \
X(LocKey_Launch, L"Launch", L"Запуск") \
X(LocKey_Wings, L"Wings", L"Крылья") \
X(LocKey_Health, L"Health", L"Здоровье") \
X(LocKey_PlayersCount, L"Players count", L"Счетчик игроков") \
X(LocKey_Climb, L"Climb", L"Карабканье") \
X(LocKey_HeadBattery, L"Head battery", L"Заряд головы") \
X(LocKey_Small, L"Small", L"Маленькая") \
X(LocKey_Medium, L"Medium", L"Средняя") \
X(LocKey_Large, L"Large", L"Большая") \
X(LocKey_Unknown, L"Unknown", L"Неизвестное") \
X(LocKey_UnlockAll, L"Unlock all", L"Открыть все")\
X(LocKey_ResetAll, L"Reset all", L"Сбросить все")

namespace Cheat
{
    enum LocKey
    {
        #define X(key, en, ru) key,
        LOC_LIST(X)
        #undef X
        LocKey_Count
    };

    enum Lang
    {
        Lang_Eng,
        Lang_Ru,
        Lang_Count
    };

    inline constexpr Hax::WStringView g_LocDict[Lang_Count][LocKey_Count] = 
    {
        {
            #define X(key, en, ru) en,
            LOC_LIST(X)
            #undef X
        },
        {
            #define X(key, en, ru) ru,
            LOC_LIST(X)
            #undef X
        }
    };
}
