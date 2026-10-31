#pragma once

#include <third_party/haxsdk/hax.h>

#define LOC_LIST(X) \
X(LocKey_Stats, L"Stats", L"Статы") \
X(LocKey_Entities, L"Entities & team", L"Существа и команда") \
X(LocKey_Vision, L"Vision", L"Зрение") \
X(LocKey_Valuables, L"Economy", L"Экономика") \
X(LocKey_Items, L"Items & spawn", L"Предметы и спавн") \
X(LocKey_Level, L"Session & level", L"Сессия и уровень") \
X(LocKey_Misc, L"Teleport", L"Телепорт")\
X(LocKey_Settings, L"Settings", L"Настройки")\
X(LocKey_ControlCenter, L"CONTROL CENTER", L"ЦЕНТР УПРАВЛЕНИЯ") \
X(LocKey_PlayerCategory, L"Player", L"Игрок") \
X(LocKey_WorldCategory, L"World", L"Мир") \
X(LocKey_SessionCategory, L"Session", L"Сессия") \
X(LocKey_ToolsCategory, L"Tools", L"Инструменты") \
X(LocKey_EconomyCategory, L"Economy", L"Экономика") \
X(LocKey_GodCategory, L"GOD MODE", L"РЕЖИМ БОГА") \
X(LocKey_GodTools, L"Object editor", L"Редактор объектов") \
X(LocKey_StatsPageDesc, L"Health, movement, abilities and upgrades", L"Здоровье, передвижение, способности и улучшения") \
X(LocKey_EntitiesPageDesc, L"Enemies, players, friends and team-wide actions", L"Враги, игроки, друзья и массовые действия команды") \
X(LocKey_VisionPageDesc, L"ESP, visibility, lighting and camera helpers", L"ESP, видимость, освещение и инструменты камеры") \
X(LocKey_EconomyPageDesc, L"Quota, balance, valuables, extraction and loot", L"Квота, баланс, ценности, эвакуация и добыча") \
X(LocKey_ItemsPageDesc, L"Weapons, item spawning and raycast deletion", L"Оружие, создание предметов и удаление по прицелу") \
X(LocKey_LevelPageDesc, L"Session safety, progression, maps and world state", L"Безопасность сессии, прогресс, карты и состояние мира") \
X(LocKey_CosmeticPageDesc, L"Local appearance and cosmetic spawning", L"Локальный внешний вид и создание косметики") \
X(LocKey_TeleportPageDesc, L"Quick destinations, players and saved positions", L"Быстрые точки, игроки и сохранённые позиции") \
X(LocKey_SettingsPageDesc, L"Interface, language and hotkeys", L"Интерфейс, язык и горячие клавиши") \
X(LocKey_EditorPageDesc, L"Select, move, rotate, scale and manipulate scene objects", L"Выбор, перемещение, вращение и изменение объектов сцены") \
X(LocKey_SearchHint, L"Search features", L"Поиск функций") \
X(LocKey_NothingFound, L"Nothing found", L"Ничего не найдено") \
X(LocKey_HideMenu, L"Hide menu", L"Скрыть меню") \
X(LocKey_CloseMenu, L"Close menu", L"Закрыть меню") \
X(LocKey_SettingsAutosave, L"Settings are saved to haxsdk.ini", L"Настройки сохраняются в haxsdk.ini") \
X(LocKey_PlayersLabel, L"Players", L"Игроки") \
X(LocKey_HostBadge, L"HOST", L"ХОСТ") \
X(LocKey_ClientBadge, L"CLIENT", L"КЛИЕНТ") \
X(LocKey_ResetRiskyShort, L"Reset risky", L"Сбросить рискованное") \
X(LocKey_NOCLIP, L"NOCLIP", L"НОКЛИП") \
X(LocKey_GodmodeHint, L"Incoming damage is ignored", L"Урон по вам не проходит") \
X(LocKey_HealShort, L"Heal", L"Исцелить") \
X(LocKey_MoveShort, L"Move", L"Переместить") \
X(LocKey_ApplyShort, L"Apply", L"Применить") \
X(LocKey_GoShort, L"Go", L"Перейти") \
X(LocKey_SaveShort, L"Save", L"Сохранить") \
X(LocKey_ActivateShort, L"Activate", L"Активировать") \
X(LocKey_FixShort, L"Fix", L"Исправить") \
X(LocKey_DontTumbleHint, L"Stay upright while flying", L"Не опрокидываться в полёте") \
X(LocKey_AidKitsHint, L"Health packs on the map", L"Аптечки на карте") \
X(LocKey_NoneAvailable, L"None available", L"Нет доступных") \
X(LocKey_UpgradesLocal, L"local", L"локально") \
X(LocKey_AvailableNowFmt, L"%d available", L"Доступно: %d") \
X(LocKey_PROGRESS, L"PROGRESS", L"ПРОГРЕСС") \
X(LocKey_SAFETY, L"SAFETY", L"БЕЗОПАСНОСТЬ") \
X(LocKey_LEVEL, L"LEVEL", L"УРОВЕНЬ") \
X(LocKey_LevelsDoneFmt, L"Levels completed: %d", L"Пройдено уровней: %d") \
X(LocKey_OnMapHint, L"active on the map", L"активны на карте") \
X(LocKey_ValuablesPointsTile, L"Valuables · extraction", L"Ценности · извлечение") \
X(LocKey_PointsOpenHint, L"extraction points", L"точек извлечения") \
X(LocKey_LevelsCompletedShort2, L"Levels completed", L"Пройдено уровней") \
X(LocKey_LIGHTING, L"Lighting", L"Освещение") \
X(LocKey_YOU, L"YOU", L"ВЫ") \
X(LocKey_SelectedTag, L"selected", L"выбран") \
X(LocKey_YouTag, L"you", L"вы") \
X(LocKey_AliveStatus, L"ALIVE", L"ЖИВ") \
X(LocKey_DeadStatus, L"DEAD", L"МЁРТВ") \
X(LocKey_NoPlayers, L"No players yet", L"Игроков пока нет") \
X(LocKey_SelectPlayerHint, L"Pick a player in the list above", L"Выберите игрока в списке выше") \
X(LocKey_TeleportToShort, L"Teleport to", L"Телепорт к нему") \
X(LocKey_BringShort, L"Bring to me", L"Ко мне") \
X(LocKey_OnMapFmt, L"%d on the map", L"%d на карте") \
X(LocKey_RunBalance, L"Run balance", L"Баланс забега") \
X(LocKey_QuotaTile, L"Quota", L"Квота") \
X(LocKey_ValuablesOnMap, L"Valuables on the map", L"Ценности на карте") \
X(LocKey_BalanceSyncHint, L"Synced and saved immediately", L"Синхронизируется и сохраняется сразу") \
X(LocKey_QuotaTileHint, L"Target for extraction points", L"Цель для точек извлечения") \
X(LocKey_PricesAll, L"All valuable prices", L"Цены всех ценностей") \
X(LocKey_HOTKEYS, L"HOTKEYS", L"ГОРЯЧИЕ КЛАВИШИ") \
X(LocKey_INTERFACE, L"INTERFACE", L"ИНТЕРФЕЙС") \
X(LocKey_QUICK_TELEPORT, L"QUICK TELEPORT", L"БЫСТРЫЙ ТЕЛЕПОРТ") \
X(LocKey_PLAYER_TELEPORT, L"PLAYER", L"ИГРОК") \
X(LocKey_Gizmo, L"Gizmo", L"Гизмо") \
X(LocKey_LightNormal, L"Normal", L"Обычное") \
X(LocKey_LightDark, L"Dark", L"Темнота") \
X(LocKey_LightBright, L"Bright", L"Яркое") \
X(LocKey_TargetLockShort, L"Lock", L"Зафиксировать") \
X(LocKey_GizmoModeLabel, L"Mode", L"Режим") \
X(LocKey_GizmoSpaceLabel, L"Axes", L"Оси") \
X(LocKey_QUOTA, L"QUOTA", L"КВОТА") \
X(LocKey_CurrentQuota, L"Current quota", L"Текущая квота") \
X(LocKey_QuotaTarget, L"New quota", L"Новая квота") \
X(LocKey_ApplyQuota, L"Apply quota", L"Применить квоту") \
X(LocKey_QuotaDesc, L"Updates every extraction point; host only", L"Обновляет все точки эвакуации; только хост") \
X(LocKey_ActionQuotaChangedFmt, L"Quota changed: $%dK", L"Квота изменена: $%dK") \
X(LocKey_ActionQuotaHostOnly, L"Quota change skipped: host only", L"Изменение квоты пропущено: только хост") \
X(LocKey_ActionQuotaNotReady, L"Quota change failed: extraction is not ready", L"Квота не изменена: эвакуация не готова") \
X(LocKey_SURPLUS_BAG, L"SURPLUS PAYOUT", L"МЕШОЧЕК С ОСТАТКОМ") \
X(LocKey_SurplusBagDesc, L"Creates the same money bag returned after meeting quota", L"Создаёт такой же мешочек, который выдаётся за остаток после квоты") \
X(LocKey_SpawnSurplusBag, L"DISPENSE MONEY BAG", L"ВЫДАТЬ МЕШОЧЕК") \
X(LocKey_SurplusBagHostHint, L"Drops in front of you; host only", L"Появится перед вами; только хост") \
X(LocKey_ActionSurplusBagSpawnedFmt, L"Money bag created: $%dK", L"Мешочек создан: $%dK") \
X(LocKey_ActionSurplusBagHostOnly, L"Money bag creation skipped: host only", L"Создание мешочка пропущено: только хост") \
X(LocKey_ActionSurplusBagNotReady, L"Money bag creation failed: assets are not ready", L"Мешочек не создан: игровые ресурсы не готовы") \
X(LocKey_TARGET_INSPECTOR, L"TARGET INSPECTOR", L"ИНСПЕКТОР ЦЕЛИ") \
X(LocKey_TargetingEnabled, L"Aim targeting", L"Наведение на объекты") \
X(LocKey_AimedObject, L"Aimed object", L"Объект под прицелом") \
X(LocKey_LockedObject, L"Locked object", L"Зафиксированный объект") \
X(LocKey_ObjectAuthority, L"Object authority", L"Тип объекта") \
X(LocKey_ObjectPosition, L"Position", L"Позиция") \
X(LocKey_ObjectLayer, L"Layer", L"Слой") \
X(LocKey_NetworkObject, L"Photon / synchronized", L"Photon / синхронизируемый") \
X(LocKey_LocalObject, L"Scene / local", L"Объект сцены / локальный") \
X(LocKey_LockTarget, L"Lock aimed target", L"Зафиксировать цель") \
X(LocKey_ClearTarget, L"Clear target", L"Очистить цель") \
X(LocKey_TRANSFORM_GIZMO, L"TRANSFORM GIZMO", L"ГИЗМО ТРАНСФОРМАЦИИ") \
X(LocKey_GizmoVisible, L"Show 3D gizmo", L"Показывать 3D-гизмо") \
X(LocKey_GizmoEditMode, L"Scene edit mode", L"Режим редактирования сцены") \
X(LocKey_GizmoEditModeDesc, L"Close the menu to drag axes; Esc returns to the menu", L"Закройте меню и тяните оси; Esc возвращает в меню") \
X(LocKey_GizmoMove, L"Move (W)", L"Перемещение (W)") \
X(LocKey_GizmoRotate, L"Rotate (E)", L"Вращение (E)") \
X(LocKey_GizmoScale, L"Scale (R)", L"Масштаб (R)") \
X(LocKey_GizmoWorld, L"World", L"Мировые оси") \
X(LocKey_GizmoLocal, L"Local", L"Локальные оси") \
X(LocKey_GizmoControls, L"X red · Y green · Z blue · drag with LMB", L"X красная · Y зелёная · Z синяя · тяните ЛКМ") \
X(LocKey_TELEKINESIS, L"TELEKINESIS / BUILDER", L"ТЕЛЕКИНЕЗ / РЕДАКТОР") \
X(LocKey_HoldRelease, L"Hold / release", L"Удерживать / отпустить") \
X(LocKey_HoldDistance, L"Hold distance", L"Дистанция удержания") \
X(LocKey_ThrowForce, L"Throw force", L"Сила броска") \
X(LocKey_PullObject, L"Pull", L"Притянуть") \
X(LocKey_PushObject, L"Throw", L"Бросить") \
X(LocKey_FreezeObject, L"Freeze / unfreeze", L"Заморозить / разморозить") \
X(LocKey_DuplicateObject, L"Duplicate locally", L"Дублировать локально") \
X(LocKey_ObjectScale, L"Uniform scale", L"Масштаб") \
X(LocKey_ScaleX, L"Scale X", L"Масштаб X") \
X(LocKey_ScaleY, L"Scale Y", L"Масштаб Y") \
X(LocKey_ScaleZ, L"Scale Z", L"Масштаб Z") \
X(LocKey_RotationX, L"Rotation X", L"Поворот X") \
X(LocKey_RotationY, L"Rotation Y", L"Поворот Y") \
X(LocKey_RotationZ, L"Rotation Z", L"Поворот Z") \
X(LocKey_ApplyTransform, L"Apply transform", L"Применить трансформацию") \
X(LocKey_UndoActionFmt, L"Undo last action (%d)", L"Отменить действие (%d)") \
X(LocKey_WORLD_CONTROL, L"WORLD CONTROL", L"УПРАВЛЕНИЕ МИРОМ") \
X(LocKey_TimeScale, L"Time scale", L"Скорость времени") \
X(LocKey_GravityOverride, L"Override gravity", L"Изменить гравитацию") \
X(LocKey_GravityY, L"Vertical gravity", L"Вертикальная гравитация") \
X(LocKey_FreezePhysics, L"Freeze world physics", L"Заморозить физику мира") \
X(LocKey_Blackout, L"Blackout", L"Выключить свет") \
X(LocKey_FullBrightWorld, L"Full bright", L"Максимальный свет") \
X(LocKey_RestoreLighting, L"Restore lighting", L"Восстановить освещение") \
X(LocKey_ENEMY_DIRECTOR, L"ENEMY DIRECTOR", L"УПРАВЛЕНИЕ ВРАГАМИ") \
X(LocKey_PacifyEnemies, L"Pacify all enemies", L"Сделать всех врагов пассивными") \
X(LocKey_FreezeEnemies, L"Freeze all enemies", L"Заморозить всех врагов") \
X(LocKey_GatherEnemies, L"Gather in front of me", L"Собрать перед собой") \
X(LocKey_KillAllEnemies, L"Kill all", L"Убить всех") \
X(LocKey_DeleteAllEnemies, L"Delete all", L"Удалить всех") \
X(LocKey_LOOT_CONTROL, L"LOOT CONTROL", L"УПРАВЛЕНИЕ ДОБЫЧЕЙ") \
X(LocKey_FreezeLoot, L"Freeze all valuables", L"Заморозить все ценности") \
X(LocKey_LootToPlayer, L"Bring to me", L"Притянуть ко мне") \
X(LocKey_LootToTruck, L"Send to truck", L"Отправить в фургон") \
X(LocKey_LootToExtraction, L"Send to extraction", L"Отправить на точку сбора") \
X(LocKey_DiscoverLoot, L"Discover all", L"Обнаружить всё") \
X(LocKey_ValueMultiplier, L"Value multiplier", L"Множитель стоимости") \
X(LocKey_ApplyValueMultiplier, L"Apply to all valuables", L"Применить ко всем ценностям") \
X(LocKey_TEAM_CONTROL, L"TEAM CONTROL", L"УПРАВЛЕНИЕ КОМАНДОЙ") \
X(LocKey_GodmodeAll, L"Godmode for everyone", L"Бессмертие для всех") \
X(LocKey_AntiTumbleAll, L"Anti-tumble for everyone", L"Защита от падения для всех") \
X(LocKey_HealAllPlayers, L"Heal everyone", L"Вылечить всех") \
X(LocKey_ReviveAllPlayers, L"Revive everyone", L"Воскресить всех") \
X(LocKey_GatherAllPlayers, L"Gather team", L"Собрать команду") \
X(LocKey_PlayerScaleAll, L"Team scale", L"Размер команды") \
X(LocKey_ApplyPlayerScaleAll, L"Apply scale to everyone", L"Изменить размер всех") \
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
X(LocKey_EyeLasersDesc, L"Networked beams and damage are processed by the host", L"Сетевые лучи и урон обрабатывает хост") \
X(LocKey_EyeLaserHostRelay, L"Allow client eye lasers", L"Разрешить лазеры клиентам") \
X(LocKey_EyeLaserHostRelayDesc, L"Process laser requests from players with RepoHax", L"Обрабатывать запросы лазеров от игроков с RepoHax") \
X(LocKey_EyeLaserHostConnected, L"Host confirmed network lasers", L"Хост подтвердил сетевые лазеры") \
X(LocKey_EyeLaserHostRequired, L"Waiting for a host with RepoHax; until confirmed, beams are local visuals", L"Ожидание хоста с RepoHax; до подтверждения лучи видны только вам") \
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
X(LocKey_ReviveSelf, L"Revive myself", L"Возродиться") \
X(LocKey_ReviveSelfHotkey, L"Quick revive hotkey", L"Бинд возрождения") \
X(LocKey_ReviveSelfDesc, L"Revives you without opening the menu or selecting a player; host only. Esc clears the binding", L"Возрождает вас без открытия меню и выбора игрока; только хост. Esc сбрасывает бинд") \
X(LocKey_ActionReviveSelfDone, L"Quick revive requested", L"Запрошено быстрое возрождение") \
X(LocKey_ActionReviveSelfAlive, L"Quick revive skipped: you are alive", L"Возрождение пропущено: вы живы") \
X(LocKey_ActionReviveSelfHostOnly, L"Quick revive skipped: host only", L"Возрождение пропущено: только хост") \
X(LocKey_ActionReviveSelfNotReady, L"Quick revive unavailable: death head or session is not ready", L"Возрождение недоступно: голова или сессия не готова") \
X(LocKey_FriendTools, L"FRIEND TOOLS", L"ИНСТРУМЕНТЫ ДЛЯ ДРУЗЕЙ") \
X(LocKey_FriendToolsDesc, L"Host-side effects for the selected player", L"Эффекты хоста для выбранного игрока") \
X(LocKey_ProtectPlayer, L"Godmode", L"Бессмертие") \
X(LocKey_ProtectPlayerDesc, L"Block damage and death for this player", L"Блокировать урон и смерть для этого игрока") \
X(LocKey_ProtectPlayerTumble, L"Anti-tumble", L"Защита от падения") \
X(LocKey_ProtectPlayerTumbleDesc, L"Block forced tumbles for this player", L"Блокировать принудительные падения этого игрока") \
X(LocKey_HealPlayer, L"Heal to max", L"Вылечить полностью") \
X(LocKey_ActionPlayerHealed, L"Selected player healed", L"Выбранный игрок полностью вылечен") \
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
X(LocKey_OBJECT_REMOVER, L"OBJECT REMOVER", L"УДАЛЕНИЕ ОБЪЕКТОВ") \
X(LocKey_ObjectRemover, L"Aim remover", L"Удаление по прицелу") \
X(LocKey_ObjectRemoverDesc, L"Delete the aimed GameObject; Photon objects are removed for everyone", L"Удаляет GameObject под прицелом; Photon-объекты удаляются у всех") \
X(LocKey_ObjectRemoverRange, L"Targeting range", L"Дальность наведения") \
X(LocKey_DeleteTarget, L"Delete aimed object", L"Удалить объект под прицелом") \
X(LocKey_TargetReady, L"Target acquired", L"Цель найдена") \
X(LocKey_NoTarget, L"No GameObject target", L"Нет цели GameObject") \
X(LocKey_ActionObjectDeleted, L"Aimed object deleted", L"Объект под прицелом удалён") \
X(LocKey_ActionObjectNoTarget, L"Delete skipped: no removable target", L"Удаление пропущено: подходящая цель не найдена") \
X(LocKey_ActionObjectHostOnly, L"Delete skipped: host only", L"Удаление пропущено: только для хоста") \
X(LocKey_ActionDronePrefabMissing, L"Drone launch skipped: drone or grenade prefab not found", L"Запуск дрона отменён: префаб дрона или гранаты не найден") \
X(LocKey_ActionDroneNoTarget, L"Drone launch skipped: select a living target", L"Запуск дрона отменён: выберите живую цель") \
X(LocKey_ActionDroneLaunchedFmt, L"Drones launched: %d", L"Запущено дронов: %d") \
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
X(LocKey_MainMenuStatus, L"Main menu", L"Главное меню") \
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
X(LocKey_RepairBalance, L"Repair overflowed balance", L"Исправить переполненный баланс") \
X(LocKey_RepairBalanceDesc, L"Clamp the balance to the maximum value safe for the shop", L"Ограничить баланс максимальным безопасным для магазина значением") \
X(LocKey_ActionCurrencyChangedFmt, L"Balance changed: $%dK", L"Баланс изменён: $%dK") \
X(LocKey_ActionCurrencyHostOnly, L"Balance change skipped: host only", L"Изменение баланса пропущено: только хост") \
X(LocKey_ActionCurrencyNotReady, L"Balance change failed: run stats are not ready", L"Баланс не изменён: статистика забега не готова") \
X(LocKey_SaveNow, L"Save now", L"Сохранить сейчас") \
X(LocKey_ReloadLevel, L"Restart with a new map", L"Перезапустить с новой картой") \
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
X(LocKey_ActionReloadNotInGame, L"Map reroll skipped: not in game", L"Смена карты пропущена: вы не в игре") \
X(LocKey_ActionReloadHostOnly, L"Map reroll skipped: host only", L"Смена карты пропущена: только хост") \
X(LocKey_ActionReloadRequested, L"Restart with a new map requested", L"Перезапуск с новой картой запрошен") \
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
X(LocKey_DRONESTRIKE, L"DRONE STRIKE", L"УДАР БПЛА") \
X(LocKey_DroneStrikeDesc, L"Kamikaze drone with grenades: flies at the target and blows up", L"Дрон-камикадзе с гранатами: летит в цель и взрывается") \
X(LocKey_DroneGuidance, L"Guidance", L"Наведение") \
X(LocKey_DroneModeAuto, L"Auto", L"Авто") \
X(LocKey_DroneModeManual, L"Crosshair", L"По прицелу") \
X(LocKey_DroneManualHint, L"Flies where you aim and blows up on anyone it touches", L"Летит туда, куда вы смотрите, и взрывается о любого") \
X(LocKey_DroneTarget, L"Target", L"Цель") \
X(LocKey_DronePlayerTag, L"Player", L"Игрок") \
X(LocKey_DroneEnemyTag, L"Enemy", L"Враг") \
X(LocKey_DroneSpeed, L"Drone speed", L"Скорость дрона") \
X(LocKey_DroneSpeedFmt, L"%d m/s", L"%d м/с") \
X(LocKey_DroneSwarm, L"Drones per launch", L"Дронов за запуск") \
X(LocKey_DroneWarheads, L"Grenades per drone", L"Гранат на дрон") \
X(LocKey_DroneThroughWalls, L"Fly through walls", L"Сквозь стены") \
X(LocKey_DroneThroughWallsDesc, L"Off: dodges walls and blows up if it crashes", L"Выкл: облетает стены и взрывается, если врежется") \
X(LocKey_DroneLethal, L"Guaranteed kill", L"Гарантированное убийство") \
X(LocKey_DroneLethalDesc, L"The hit target dies even if the blast was not enough", L"Цель умрёт, даже если взрыва не хватило") \
X(LocKey_DroneLaunch, L"Launch", L"Запустить") \
X(LocKey_DroneDetonate, L"Detonate all", L"Подорвать все") \
X(LocKey_DroneLaunchHotkey, L"Drone launch key", L"Клавиша запуска дрона") \
X(LocKey_DroneDetonateHotkey, L"Drone detonate key", L"Клавиша подрыва дронов") \
X(LocKey_DronesInFlight, L"In the air", L"В воздухе") \
X(LocKey_DronesQueued, L"queued", L"в очереди") \
X(LocKey_DroneFpv, L"FPV camera", L"FPV-камера") \
X(LocKey_DroneFpvDesc, L"Watch through the newest drone. In Crosshair mode you fly it: mouse steers, W/S throttle", L"Вид с последнего дрона. В режиме «По прицелу» им управляешь ты: мышь — руль, W/S — газ") \
X(LocKey_DroneFpvHotkey, L"FPV camera key", L"Клавиша FPV-камеры") \
X(LocKey_DroneRetarget, L"Hunt the next target", L"Искать следующую цель") \
X(LocKey_DroneRetargetDesc, L"Off: when the target is gone the drone circles in place until detonated", L"Выкл: без цели дрон кружит на месте, пока не подорвёшь") \
X(LocKey_FunTab, L"Fun & chaos", L"Приколы и хаос") \
X(LocKey_FunCategory, L"Fun", L"Приколы") \
X(LocKey_FunPageDesc, L"Drones, orbital strikes, black holes and chaos mode", L"Дроны, орбитальные удары, чёрные дыры и хаос-режим") \
X(LocKey_ORBITAL, L"ORBITAL STRIKE", L"ОРБИТАЛЬНЫЙ УДАР") \
X(LocKey_OrbitalDesc, L"A laser burns down from the ceiling, then a column of blasts hits the mark", L"С потолка бьёт лазер, затем по метке проходит столб взрывов") \
X(LocKey_OrbitalPower, L"Shells per strike", L"Снарядов за удар") \
X(LocKey_OrbitalAtCrosshair, L"At crosshair", L"По прицелу") \
X(LocKey_OrbitalAtTarget, L"At drone target", L"По цели дрона") \
X(LocKey_OrbitalHotkey, L"Orbital strike key", L"Клавиша орбитального удара") \
X(LocKey_BLACKHOLE, L"BLACK HOLE", L"ЧЁРНАЯ ДЫРА") \
X(LocKey_BlackHoleDesc, L"Sucks loot, items, enemies and players into a spiral, then blows it all apart", L"Затягивает лут, предметы, врагов и игроков в воронку, а потом всё разносит") \
X(LocKey_BlackHoleRadius, L"Radius", L"Радиус") \
X(LocKey_BlackHoleDuration, L"Duration", L"Длительность") \
X(LocKey_BlackHoleStrength, L"Pull strength", L"Сила притяжения") \
X(LocKey_BlackHoleIncludeSelf, L"Pull me in too", L"Затягивать и меня") \
X(LocKey_BlackHoleOpen, L"Open at crosshair", L"Открыть по прицелу") \
X(LocKey_BlackHoleCollapse, L"Collapse now", L"Схлопнуть") \
X(LocKey_BlackHoleHotkey, L"Black hole key", L"Клавиша чёрной дыры") \
X(LocKey_SecondsFmt, L"%d s", L"%d с") \
X(LocKey_CHAOS, L"CHAOS MODE", L"ХАОС-РЕЖИМ") \
X(LocKey_ChaosDesc, L"A random event every few seconds: gravity, darkness, drones, black holes...", L"Случайное событие каждые N секунд: гравитация, темнота, дроны, чёрные дыры...") \
X(LocKey_ChaosInterval, L"Event every", L"Событие каждые") \
X(LocKey_ChaosSpareMe, L"Spare me", L"Не трогать меня") \
X(LocKey_ChaosNow, L"Chaos now", L"Хаос сейчас") \
X(LocKey_ChaosNextFmt, L"next in %d s", L"следующее через %d с") \
X(LocKey_ChaosGravityFlip, L"CHAOS: gravity flipped!", L"ХАОС: гравитация вверх!") \
X(LocKey_ChaosZeroGravity, L"CHAOS: zero gravity", L"ХАОС: невесомость") \
X(LocKey_ChaosBlackout, L"CHAOS: lights out", L"ХАОС: свет вырубили") \
X(LocKey_ChaosSlowMotion, L"CHAOS: slow motion", L"ХАОС: замедление времени") \
X(LocKey_ChaosTurbo, L"CHAOS: turbo time", L"ХАОС: турбо-время") \
X(LocKey_ChaosTumbleAll, L"CHAOS: everybody falls over", L"ХАОС: все падают") \
X(LocKey_ChaosDroneRaid, L"CHAOS: drone raid!", L"ХАОС: налёт дронов!") \
X(LocKey_ChaosOrbital, L"CHAOS: orbital strike incoming", L"ХАОС: орбитальный удар!") \
X(LocKey_ChaosBlackHole, L"CHAOS: a black hole opened", L"ХАОС: открылась чёрная дыра") \
X(LocKey_ChaosSpawnEnemy, L"CHAOS: someone new is here...", L"ХАОС: кто-то пришёл...") \
X(LocKey_ChaosLaunchEveryone, L"CHAOS: everybody to the ceiling!", L"ХАОС: все к потолку!") \
X(LocKey_ActionOrbitalIncoming, L"Orbital strike incoming", L"Орбитальный удар запущен") \
X(LocKey_ActionBlackHoleOpened, L"Black hole opened", L"Чёрная дыра открыта") \
X(LocKey_FpvSignalLost, L"SIGNAL LOST", L"СИГНАЛ ПОТЕРЯН") \
X(LocKey_FpvThrottleHint, L"W/S throttle · mouse steers", L"W/S — газ · мышь — руль") \
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
