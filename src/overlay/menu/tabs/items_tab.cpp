#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"
#include "../theme.h"

#define LINE_ID (HAX_LINE * 234)

namespace Cheat
{
    static void DrawSpawnCard()
    {
        const bool host = G->IsInGame && !G->IsClient;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_SPAWN], {}, {.Icon = Icon::Box, .HostOnly = true});
        {
            const float gap = 8_px;
            const Hax::Vector2 spawnSize = CalcButtonSize(G->Loc[LocKey_Spawn]);
            const size_t nItems = G->ItemsPool.Size();
            constexpr size_t dropListId = Hax::Hash(L"ItemSelect");
            const float dropW = Hax::Gui::GetContentRegionAvail().X - gap - spawnSize.X;

            Hax::Gui::BeginHorizontal(gap);
            {
                Hax::WStringView preview = L"—";
                if (G->SelectedItem != null)
                    preview = G->SelectedItem.itemName().ToHaxView();

                const Hax::Vector2 pos = Hax::Gui::GetCursorPos();
                if (DropdownBtn(LINE_ID, preview, dropW))
                    OpenPopup(dropListId, {pos.X, pos.Y + CalcButtonHeight()});

                if (Button(LINE_ID, G->Loc[LocKey_Spawn], {}, {.Enabled = host && G->SelectedItem, .Style = ButtonStyle::Primary}))
                    G->ItemToSpawn = G->SelectedItem;
            }
            Hax::Gui::EndHorizontal();

            const float selectableH = CalcButtonHeight();
            const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(nItems, 8ULL)) + 8_px + 1.f;
            if (BeginDropList(dropListId, {dropW, dropListH}))
            {
                const float w = Hax::Gui::GetContentRegionAvail().X;
                for (size_t i = 0; i < nItems; ++i)
                {
                    const auto& p = G->ItemsPool.begin()[i];
                    if (Selectable(LINE_ID + i * 10000, p.key, G->SelectedItem == p.value, {.MinW = w}))
                    {
                        G->SelectedItem = p.value;
                        ClosePopup(dropListId);
                    }
                }
                EndDropList();
            }

            HorizontalLine(1_px);
            HotkeyEx(Hax::Hash(L"SpawnItemHotkeyEditor"), G->VkSpawnItem, G->Loc[LocKey_Hotkey], G->Loc[LocKey_Spawn]);
        }
        EndPanel();
    }

    static void DrawRemoverCard()
    {
        const bool canDelete = G->IsInGame && !G->IsClient && G->ObjectRemoverEnabled && G->ObjectRemoverTargetValid;

        BeginPanel(LINE_ID);
        PanelHeader(G->Loc[LocKey_OBJECT_REMOVER], G->Loc[LocKey_ObjectRemoverDesc],
            {.Icon = Icon::Trash, .HostOnly = true, .Toggle = &G->ObjectRemoverEnabled, .ToggleDisabled = G->IsClient});
        {
            Hax::char16 range[32]{};
            swprintf_s(range, _countof(range), G->Loc[LocKey_Meters].Data(), G->ObjectRemoverRange);
            SliderEx(LINE_ID, G->Loc[LocKey_ObjectRemoverRange], range, &G->ObjectRemoverRange, 3, 100, SliderConvertInt);
            HorizontalLine(1_px);

            if (ActionRow(LINE_ID, G->Loc[LocKey_DeleteTarget],
                G->ObjectRemoverTargetValid ? G->Loc[LocKey_TargetReady] : G->Loc[LocKey_NoTarget],
                G->Loc[LocKey_DeleteTarget],
                {.Enabled = canDelete, .Style = ButtonStyle::Danger, .Hotkey = &G->VkDeleteObject, .HotkeyId = Hax::Hash(L"DeleteObjectHotkeyEditor")}))
                G->DeleteObjectRequested = true;
        }
        EndPanel();
    }

    void DrawItemsTab()
    {
        BeginColumns(Hax::Hash("ItemsColumns"), 3);
        {
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_GUNS], {}, {.Icon = Icon::Gun});
            {
                ToggleEx(LINE_ID, G->InfBattery, G->Loc[LocKey_MaxBatteryGun], {}, {.Disabled = G->IsClient, .HostOnly = true});
                HorizontalLine(1_px);
                ToggleEx(LINE_ID, G->UseLaser, G->Loc[LocKey_LaserCrosshair]);
            }
            EndPanel();
        }
        NextColumn();
        DrawSpawnCard();
        NextColumn();
        DrawRemoverCard();
        EndColumns();
    }
}
