#pragma once

#include "haxsdk/gui/hax_gui_dx11.h"

namespace Cheat::Visuals
{
    enum Language
    {
        Lang_Eng,
        Lang_Ru,
        Lang_Count
    };

    struct EnemyEspData
    {
        Hax::Rect Box;
        Hax::WStringView Name;
        float Distance;
        int CurHp;
        int MaxHp;
    };

    struct PlayerEspData
    {
        Hax::Rect Box;
        Hax::WStringView Name;
        float Distance;
        int CurHp;
        int MaxHp;
        bool Dead;
    };

    struct ValuableEspData
    {
        Hax::WStringView Name;
        Hax::Vector2 Pos;
        float Distance;
        float Value;
    };

    struct ExtrPointEspData
    {
        Hax::Vector2 Pos;
        float Distance;
        bool Completed;
        bool Active;
    };

    struct TruckEspData
    {
        Hax::Vector2 Pos;
        float Distance;
    };

    void RegisterConfig(Hax::IniFile& iniFile);
    void InitializeMenu(Hax::Handle hDirectX11);
    bool IsUpgrAutouse(void* assetName);
}
