#include <src/pch.h>

#include "esp.h"

#include <src/cheat.h>

#include "../menu/theme.h"

namespace Cheat
{
    enum VerticalAlignment : int 
    {
        VerticalAlignment_Bottom,
        VerticalAlignment_Top,
        VerticalAlignment_Center
    };

    enum HorizontalAlignment : int
    {
        HorizontalAlignment_Right,
        HorizontalAlignment_Left,
        HorizontalAlignment_Center
    };

    static void DrawEnemyEsp(EnemyEspData& data);
    static void DrawValuableEsp(ValuableEspData& data);
    static void DrawExtrPointEsp(ExtrPointEspData& data);
    static void DrawTruckEsp(TruckEspData& data);
    static void DrawPlayerEsp(PlayerEspData& data);
    static void DrawCosmeticBoxEsp(CosmeticBoxEspData& data);
    static void Box(const Hax::Rect& rect, const Hax::Gui::Color& col);
    static void Text(Hax::Gui::FontHandle hFont,
                     Hax::WStringView text,
                     Hax::Vector2 pos,
                     Hax::Gui::Color col,
                     float fontSize,
                     VerticalAlignment vAlign = VerticalAlignment_Bottom,
                     HorizontalAlignment hAlign = HorizontalAlignment_Right);
    static float EspFontSize() { return 10.5_px; }
    static float IconFontSize() { return 14_px; }
    static void VertHealthBar(float x, float y, float width, float height, float cur, float max);
    static void VertHealthBar(float x, float y, float width, float height, int cur, int max);
    static Unity::Rect CalcBoundsInScreenSpace(const Unity::Bounds& bigBounds, Unity::Camera cam);

    void RenderEsp()
    {
        bool shouldDrawEsp = G->IsInGame && MenuManager::instance() && !MenuManager::instance().currentMenuPage();
        if (!shouldDrawEsp)
            return;

        size_t i1 = 9999;
        size_t i2 = 9999;
        size_t i3 = 9999;
        size_t i4 = 9999;
        size_t i5 = 9999;
        size_t i6 = 9999;

        Hax::Vector<EnemyEspData>& buf1 = G->EnemiesEspBuffer.GetFront();
        Hax::Vector<ValuableEspData>& buf2 = G->ValuablesEspBuffer.GetFront();
        Hax::Vector<ExtrPointEspData>& buf3 = G->ExtrPointsEspBuffer.GetFront();
        Hax::Vector<TruckEspData>& buf4 = G->TruckEspBuffer.GetFront();
        Hax::Vector<PlayerEspData>& buf5 = G->PlayersEspBuffer.GetFront();
        Hax::Vector<CosmeticBoxEspData>& buf6 = G->CosmeticBoxesEspBuffer.GetFront();

        if (G->EnemiesEsp)
        {
            i1 = 0;
            std::sort(buf1.begin(), buf1.end(), [](const EnemyEspData& d1, const EnemyEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->ValuablesEsp)
        {
            i2 = 0;
            std::sort(buf2.begin(), buf2.end(), [](const ValuableEspData& d1, const ValuableEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->ExtrPointsEsp)
        {
            i3 = 0;
            std::sort(buf3.begin(), buf3.end(), [](const ExtrPointEspData& d1, const ExtrPointEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->PlayersEsp)
        {
            i5 = 0;
            std::sort(buf5.begin(), buf5.end(), [](const PlayerEspData& d1, const PlayerEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->TruckEsp)
        {
            i4 = 0;
        }
        if (G->CosmeticBoxesEsp)
        {
            i6 = 0;
            std::sort(buf6.begin(), buf6.end(), [](const CosmeticBoxEspData& d1, const CosmeticBoxEspData& d2) { return d1.Distance > d2.Distance; });
        }

        while (i1 < buf1.Size() || i2 < buf2.Size() || i3 < buf3.Size() || i4 < buf4.Size() || i5 < buf5.Size() || i6 < buf6.Size())
        {
            float d1 = i1 < buf1.Size() ? buf1[i1].Distance : FLT_MIN;
            float d2 = i2 < buf2.Size() ? buf2[i2].Distance : FLT_MIN;
            float d3 = i3 < buf3.Size() ? buf3[i3].Distance : FLT_MIN;
            float d4 = i4 < buf4.Size() ? buf4[i4].Distance : FLT_MIN;
            float d5 = i5 < buf5.Size() ? buf5[i5].Distance : FLT_MIN;
            float d6 = i6 < buf6.Size() ? buf6[i6].Distance : FLT_MIN;

            if (d1 >= d2 && d1 >= d3 && d1 >= d4 && d1 >= d5 && d1 >= d6)       { DrawEnemyEsp(buf1[i1++]);     }
            else if (d2 >= d1 && d2 >= d3 && d2 >= d4 && d2 >= d5 && d2 >= d6)  { DrawValuableEsp(buf2[i2++]);  }
            else if (d3 >= d1 && d3 >= d2 && d3 >= d4 && d3 >= d5 && d3 >= d6)  { DrawExtrPointEsp(buf3[i3++]); }
            else if (d4 >= d1 && d4 >= d2 && d4 >= d3 && d4 >= d5 && d4 >= d6)  { DrawTruckEsp(buf4[i4++]);     }
            else if (d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d6)  { DrawPlayerEsp(buf5[i5++]);    }
            else                                                                { DrawCosmeticBoxEsp(buf6[i6++]); }
        }
    }

    Hax::Optional<EnemyEspData> ParseEnemyEspData(Enemy enemy)
    {
        GameDirector director = GameDirector::instance();
        if (!director)
            return {};
        Unity::Camera mainCam = director.MainCamera();
        if (!mainCam || !enemy)
            return {};

        Unity::Transform cameraTransform = mainCam.GetTransform();
        Unity::Transform enemyTransform = enemy.GetTransform();
        if (!cameraTransform || !enemyTransform)
            return {};

        float dist = cameraTransform.GetPosition().Distance(enemyTransform.GetPosition());
        if (dist < 2.f)
            return {};

        EnemyParent parent = enemy.enemyParent();
        EnemyHealth health = enemy.Health();
        if (!parent || !health)
            return {};
        if (parent.enemyName() == L"Spewer")
        {
            EnemySlowMouth comp = enemy.GetComponent<EnemySlowMouth>();
            if (comp && comp.attachedTimer() > 0.f)
                return {};
        }

        Unity::Bounds enemyBounds{};
        bool hasBounds = false;
        if (EnemyRigidbody rb = enemy.Rigidbody())
        {
            PhysGrabObject grabObj = rb.physGrabObject();
            if (grabObj)
            {
                enemyBounds = Unity::Bounds(grabObj.centerPoint(), grabObj.boundingBox());
                hasBounds = true;
            }
        }
        if (!hasBounds)
        {
            // Enemy prefab hierarchies can change between updates.
            for (Unity::Collider collider : enemy.GetComponentsInChildren<Unity::Collider>())
            {
                if (!collider || !collider.GetEnabled())
                    continue;
                if (!hasBounds)
                    enemyBounds = collider.GetBounds();
                else
                    enemyBounds.Encapsulate(collider.GetBounds());
                hasBounds = true;
            }
        }
        if (!hasBounds)
            return {};

        Unity::Rect bounds = CalcBoundsInScreenSpace(enemyBounds, mainCam);
        Unity::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};

        if (!bounds.Overlaps(screenRect))
            return {};

        return EnemyEspData
        {
            .Box = bounds,
            .Name = parent.enemyName().ToHaxView(),
            .Distance = dist,
            .CurHp = health.healthCurrent(),
            .MaxHp = health.health(),
        };
    }

    Hax::Optional<ValuableEspData> ParseValuableEspData(ValuableObject obj)
    {
        if (!obj || !obj.GetEnabled())
            return {};

        Unity::Camera cam = GameDirector::instance().MainCamera();
        float dist = cam.GetTransform().GetPosition().Distance(obj.GetTransform().GetPosition());
        if (dist < 2.f || dist > (float)G->ValuablesEspRange)
            return {};

        PhysGrabObject physObject = obj.physGrabObject();
        if (!physObject)
            return {};

        Unity::Vector3 worldPos = physObject.midPoint();
        Unity::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        Unity::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.Z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.X *= scaleX;
        screenPos.Y = G->ScreenHeight - screenPos.Y * scaleY;

        return ValuableEspData
        {
            .Name = obj.GetName().ToHaxView(),
            .Pos = Unity::Vector2(screenPos),
            .Distance = dist,
            .Value = obj.dollarValueCurrent()
        };
    }

    Hax::Optional<ExtrPointEspData> ParseExtrPointEspData(Unity::GameObject obj)
    {
        Unity::Camera cam = GameDirector::instance().MainCamera();

        if (!obj || !cam)
            return {};

        ExtractionPoint point = obj.GetComponent<ExtractionPoint>();
        if (!point)
            return {};

        float dist = cam.GetTransform().GetPosition().Distance(obj.GetTransform().GetPosition());
        if (dist < 1.f)
            return {};

        Unity::Vector3 screenPos = cam.WorldToScreenPoint(obj.GetTransform().GetPosition());
        Unity::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.Z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.X *= scaleX;
        screenPos.Y = G->ScreenHeight - screenPos.Y * scaleY;

        return ExtrPointEspData 
        {
            .Pos = Unity::Vector2(screenPos),
            .Distance = dist,
            .Completed = (point.currentState() == ExtractionPoint_State::Complete()),
            .Active = (point == RoundDirector::instance().extractionPointCurrent())
        };
    }

    Hax::Optional<TruckEspData> ParseTruckEspData(TruckSafetySpawnPoint truck)
    {
        Unity::Camera cam = GameDirector::instance().MainCamera();

        if (!truck || !cam)
            return {};

        Unity::Vector3 worldPos = truck.GetTransform().GetPosition();

        float dist = cam.GetTransform().GetPosition().Distance(worldPos);
        if (dist < 1.f)
            return {};

        Unity::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        Unity::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.Z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.X *= scaleX;
        screenPos.Y = G->ScreenHeight - screenPos.Y * scaleY;

        return TruckEspData
        {
            .Pos = Unity::Vector2(screenPos),
            .Distance = dist
        };
    }

    Hax::Optional<PlayerEspData> ParsePlayerEspData(PlayerAvatar avatar)
    {
        Unity::Camera mainCam = GameDirector::instance().MainCamera();
        if (!mainCam || !avatar || avatar.isLocal())
            return {};

        float dist = mainCam.GetTransform().GetPosition().Distance(avatar.GetTransform().GetPosition());
        if (dist < 1.f)
            return {};

        if (avatar.deadSet())
        {
            PlayerDeathHead head = avatar.playerDeathHead();
            if (head && head.GetEnabled())
            {
                System::Array<Unity::Collider> colliders = head.colliders();
                Unity::Bounds bounds = colliders[0].GetBounds();
                for (auto col : colliders)
                    bounds.Encapsulate(col.GetBounds());

                if (bounds.Extents == Unity::Vector3::zero())
                    return {};

                Unity::Rect rect = CalcBoundsInScreenSpace(bounds, mainCam);
                Unity::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};

                if (!rect.Overlaps(screenRect))
                    return {};

                PlayerEspData espData{};
                espData.Box = rect;
                espData.Distance = dist;
                espData.Name = SemiFunc::PlayerGetName(avatar).ToHaxView();
                espData.CurHp = 0;
                espData.MaxHp = 0;
                espData.Dead = true;

                return espData;
            }

            return {};
        }

        if (avatar.isDisabled())
            return {};

        PlayerAvatarVisuals visuals = avatar.playerAvatarVisuals();
        Unity::Bounds bounds2 = visuals.headSideTransform().GetComponent<Unity::Collider>().GetBounds();
        Unity::Vector3 ext = bounds2.Extents;
        for (Unity::Collider col2 : visuals.legTwistTransform().GetComponentsInChildren<Unity::Collider>())
        {
            if (col2)
                bounds2.Encapsulate(col2.GetBounds());
        }
        if (bounds2.Extents == Unity::Vector3::zero())
        {
            return {};
        }

        ext.Y = bounds2.Extents.Y * 1.2f;
        ext.Z *= 1.5f;
        ext.X *= 1.5f;
        bounds2.Extents = ext;
        Unity::Rect rect = CalcBoundsInScreenSpace(bounds2, mainCam);

        Unity::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (!rect.Overlaps(screenRect))
            return {};

        PlayerHealth health = avatar.playerHealth();

        return PlayerEspData
        {
            .Box = rect,
            .Name = SemiFunc::PlayerGetName(avatar).ToHaxView(),
            .Distance = dist,
            .CurHp = health.health(),
            .MaxHp = health.maxHealth(),
            .Dead = false,
        };
    }

    Hax::Optional<CosmeticBoxEspData> ParseCosmeticBoxEspData(CosmeticWorldObject box)
    {
        Unity::Camera cam = GameDirector::instance().MainCamera();

        if (!box || !cam)
            return {};

        Unity::Vector3 worldPos = box.GetTransform().GetPosition();

        float dist = cam.GetTransform().GetPosition().Distance(worldPos);
        if (dist < 1.f)
            return {};

        Unity::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        Unity::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.Z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.X *= scaleX;
        screenPos.Y = G->ScreenHeight - screenPos.Y * scaleY;

        return CosmeticBoxEspData
        {
            .Pos = Unity::Vector2(screenPos),
            .Distance = dist,
            .Rarity = box.rarity()
        };
    }

    // ESP palette, matching the menu theme.
    namespace EspColors
    {
        constexpr Hax::Gui::Color Enemy    = Theme::Danger;
        constexpr Hax::Gui::Color Player   = Theme::Host;
        constexpr Hax::Gui::Color Dead     = Theme::Faint;
        constexpr Hax::Gui::Color Valuable = Theme::Ok;
        constexpr Hax::Gui::Color Marker   = Theme::Accent;
        constexpr Hax::Gui::Color TagBg    = 0x0A0B0DD6;
        constexpr Hax::Gui::Color Outline  = 0x00000099;
    }

    // Text label on a dark rounded plate, centred on `anchor.X`.
    static void Tag(Hax::WStringView text, Hax::Vector2 anchor, Hax::Gui::Color col, bool above)
    {
        const float fontH = EspFontSize();
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(G->NunitoSans_Bold, text, fontH);
        const Hax::Vector2 pad = {6_px, 2_px};
        const Hax::Vector2 size = textSize + pad * 2.f;
        const Hax::Vector2 min = {anchor.X - size.X / 2.f, above ? anchor.Y - 4_px - size.Y : anchor.Y + 4_px};

        Hax::Gui::DrawRect(min, min + size, {.FillColor = EspColors::TagBg, .Rounding = 5_px});
        Hax::Gui::DrawString(G->NunitoSans_Bold, text, min + pad, fontH, {.Color = col});
    }

    static Hax::Gui::Color HealthColor(float cur, float max)
    {
        const float t = max > 0.f ? Hax::Clamp(cur / max, 0.f, 1.f) : 0.f;
        return Hax::Lerp(Hax::Gui::LinearColor(Theme::Danger), Hax::Gui::LinearColor(Theme::Ok), t).ToColor();
    }

    static void DrawEnemyEsp(EnemyEspData& data)
    {
        Box(data.Box, EspColors::Enemy);
        Tag(data.Name, {data.Box.GetCenter().X, data.Box.Min.Y}, EspColors::Enemy, true);
        VertHealthBar(data.Box.Min.X - 7_px, data.Box.Min.Y, 3_px, data.Box.GetSize().Y, data.CurHp, data.MaxHp);

        Hax::WStringBuilder<16> sb;
        sb.AppendF(L"%d HP", data.CurHp);
        Tag(sb.View(), {data.Box.GetCenter().X, data.Box.Max.Y}, HealthColor((float)data.CurHp, (float)data.MaxHp), false);
    }

    static void DrawValuableEsp(ValuableEspData& data)
    {
        Hax::WStringView s = data.Name;
        if (s.EndsWith(L"(Clone)"))
            s.RemoveSuffix(7);
        if (data.Name.StartsWith(L"Valuable "))
            s.RemovePrefix(9);

        wchar_t buf[64] = {0};
        if (swprintf_s(buf, L"%.*ls \u00B7 $%.0f", (int)s.Length(), s.begin(), data.Value) > 0)
        {
            Tag(buf, data.Pos, EspColors::Valuable, false);
            return;
        }

        Tag(s, data.Pos, EspColors::Valuable, false);
    }

    static void DrawExtrPointEsp(ExtrPointEspData& data)
    {
        Hax::Gui::Color color = data.Active ? EspColors::Marker : data.Completed ? EspColors::Dead : EspColors::Enemy;
        Text(G->Icons_Solid, L"\uF468", data.Pos, color, IconFontSize());
    }

    static void DrawTruckEsp(TruckEspData& data)
    {
        Text(G->Icons_Solid, L"\uF0D1", data.Pos, EspColors::Marker, IconFontSize());
    }

    static void DrawPlayerEsp(PlayerEspData& data)
    {
        const Hax::Gui::Color col = data.Dead ? EspColors::Dead : EspColors::Player;
        Box(data.Box, col);
        Tag(data.Name, {data.Box.GetCenter().X, data.Box.Min.Y}, col, true);
        if (!data.Dead)
        {
            VertHealthBar(data.Box.Min.X - 7_px, data.Box.Min.Y, 3_px, data.Box.GetSize().Y, data.CurHp, data.MaxHp);

            Hax::WStringBuilder<16> sb;
            sb.AppendF(L"%d HP", data.CurHp);
            Tag(sb.View(), {data.Box.GetCenter().X, data.Box.Max.Y}, HealthColor((float)data.CurHp, (float)data.MaxHp), false);
        }
    }

    static void DrawCosmeticBoxEsp(CosmeticBoxEspData& data)
    {
        static Hax::Array<Hax::Gui::Color, 4> s_RarityToColor = {Theme::Ok, Theme::Host, 0xC9A7F5FF, Theme::Accent};

        Text(G->Icons_Solid, L"\uF553", data.Pos, s_RarityToColor[data.Rarity], IconFontSize());
    }

    static void Box(const Hax::Rect& rect, const Hax::Gui::Color& col)
    {
        const float outline = 3.5_px;
        const float line = 1.5_px;

        Hax::Vector2 tl = rect.GetTL();
        Hax::Vector2 tr = rect.GetTR();
        Hax::Vector2 br = rect.GetBR();
        Hax::Vector2 bl = rect.GetBL();

        Hax::Gui::DrawLine(tl, tr, {.FillColor = EspColors::Outline, .Th = outline});
        Hax::Gui::DrawLine(tr, br, {.FillColor = EspColors::Outline, .Th = outline});
        Hax::Gui::DrawLine(bl, br, {.FillColor = EspColors::Outline, .Th = outline});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = EspColors::Outline, .Th = outline});

        Hax::Gui::DrawLine(tl, tr, {.FillColor = col, .Th = line});
        Hax::Gui::DrawLine(tr, br, {.FillColor = col, .Th = line});
        Hax::Gui::DrawLine(bl, br, {.FillColor = col, .Th = line});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = col, .Th = line});
    }

    static void Text(Hax::Gui::FontHandle hFont,
                     Hax::WStringView text,
                     Hax::Vector2 pos,
                     Hax::Gui::Color col,
                     float fontSize,
                     VerticalAlignment vAlign,
                     HorizontalAlignment hAlign)
    {
        Hax::Vector2 textSize = Hax::Gui::CalcTextSize(hFont, text, fontSize);
        float xShift = hAlign == HorizontalAlignment_Right ? 0.f : textSize.X / (float)hAlign;
        float yShift = vAlign == VerticalAlignment_Bottom ? 0.f : textSize.Y / (float)vAlign;

        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift + 1.F, pos.Y - yShift - 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift + 1.F, pos.Y - yShift + 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift - 1.F, pos.Y - yShift + 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift - 1.F, pos.Y - yShift - 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift, pos.Y - yShift), fontSize, {.Color = col});
    }

    static void VertHealthBar(float x, float y, float width, float height, float cur, float max)
    {
        const float coef = max > 0.f ? Hax::Clamp(cur / max, 0.f, 1.f) : 0.f;
        Hax::Gui::DrawLine({x, y}, {x, y + height}, {.FillColor = EspColors::Outline, .Th = width + 2_px});
        Hax::Gui::DrawLine({x, y + height - height * coef}, {x, y + height}, {.FillColor = HealthColor(cur, max), .Th = width});
    }

    static void VertHealthBar(float x, float y, float width, float height, int cur, int max)
    {
        VertHealthBar(x, y, width, height, (float)cur, (float)max);
    }

    static Unity::Rect CalcBoundsInScreenSpace(const Unity::Bounds& bigBounds, Unity::Camera cam)
    {
        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        Unity::Vector3 screenSpaceCorners[8] =
        {
            {bigBounds.Center.X + bigBounds.Extents.X, bigBounds.Center.Y + bigBounds.Extents.Y, bigBounds.Center.Z + bigBounds.Extents.Z},
            {bigBounds.Center.X + bigBounds.Extents.X, bigBounds.Center.Y + bigBounds.Extents.Y, bigBounds.Center.Z - bigBounds.Extents.Z},
            {bigBounds.Center.X + bigBounds.Extents.X, bigBounds.Center.Y - bigBounds.Extents.Y, bigBounds.Center.Z + bigBounds.Extents.Z},
            {bigBounds.Center.X + bigBounds.Extents.X, bigBounds.Center.Y - bigBounds.Extents.Y, bigBounds.Center.Z - bigBounds.Extents.Z},
            {bigBounds.Center.X - bigBounds.Extents.X, bigBounds.Center.Y + bigBounds.Extents.Y, bigBounds.Center.Z + bigBounds.Extents.Z},
            {bigBounds.Center.X - bigBounds.Extents.X, bigBounds.Center.Y + bigBounds.Extents.Y, bigBounds.Center.Z - bigBounds.Extents.Z},
            {bigBounds.Center.X - bigBounds.Extents.X, bigBounds.Center.Y - bigBounds.Extents.Y, bigBounds.Center.Z + bigBounds.Extents.Z},
            {bigBounds.Center.X - bigBounds.Extents.X, bigBounds.Center.Y - bigBounds.Extents.Y, bigBounds.Center.Z - bigBounds.Extents.Z}
        };

        for (int i = 0; i < 8; ++i)
        {
            screenSpaceCorners[i] = cam.WorldToScreenPoint(screenSpaceCorners[i]);
            screenSpaceCorners[i].X *= scaleX;
            screenSpaceCorners[i].Y = G->ScreenHeight - screenSpaceCorners[i].Y * scaleY;
        }

        float x = FLT_MAX;
        float y = FLT_MAX;
        float x2 = FLT_MIN;
        float y2 = FLT_MIN;

        for (const Unity::Vector3& v : screenSpaceCorners)
        {
            if (v.Z <= 0.f)
                continue;

            x = Hax::Min(v.X, x);
            y = Hax::Min(v.Y, y);
            x2 = Hax::Max(v.X, x2);
            y2 = Hax::Max(v.Y, y2);
        }

        if (y2 - y < 15.f)
            y2 = y + 15.f;

        return Unity::Rect::MinMaxRect(x, y, x2, y2);
    }
}
