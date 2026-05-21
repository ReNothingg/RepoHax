#pragma once

#include <src/game_classes.h>
#include <third_party/haxsdk/hax.h>

namespace Cheat
{
    struct EnemyEspData
    {
        Hax::Rect           Box;
        Hax::WStringView    Name;
        float               Distance;
        int                 CurHp;
        int                 MaxHp;
    };

    struct PlayerEspData
    {
        Hax::Rect           Box;
        Hax::WStringView    Name;
        float               Distance;
        int                 CurHp;
        int                 MaxHp;
        bool                Dead;
    };

    struct ValuableEspData
    {
        Hax::WStringView    Name;
        Hax::Vector2        Pos;
        float               Distance;
        float               Value;
    };

    struct ExtrPointEspData
    {
        Hax::Vector2        Pos;
        float               Distance;
        bool                Completed;
        bool                Active;
    };

    struct TruckEspData
    {
        Hax::Vector2        Pos;
        float               Distance;
    };

    void                            RenderEsp();
    Hax::Optional<EnemyEspData>     ParseEnemyEspData(Enemy enemy);
    Hax::Optional<ValuableEspData>  ParseValuableEspData(ValuableObject obj);
    Hax::Optional<ExtrPointEspData> ParseExtrPointEspData(UnityEngine::GameObject obj);
    Hax::Optional<TruckEspData>     ParseTruckEspData(TruckSafetySpawnPoint truck);
    Hax::Optional<PlayerEspData>    ParsePlayerEspData(PlayerAvatar avatar);
}