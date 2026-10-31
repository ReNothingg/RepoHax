#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>

namespace Cheat::EyeLaserProtocol
{
    inline constexpr uint8_t EventCode = 173;
    inline constexpr int32_t Magic = 0x52484c31; // RHL1, versioned independently of Photon RPCs.
    inline constexpr uint64_t SendIntervalMs = 100;
    inline constexpr uint64_t TimeoutMs = 1500;
    inline constexpr size_t RequestSize = 11;
    inline constexpr size_t AckSize = 4;

    struct Settings
    {
        int Range = 80;
        int Damage = 250;
        bool InstantKill = true;
        bool DestroyObjects = true;
    };

    struct Request
    {
        int ViewId = 0;
        uint32_t Sequence = 0;
        bool Active = false;
        std::array<float, 3> Direction{0.f, 0.f, 1.f};
        Settings Options{};
    };

    inline bool IsNewer(uint32_t sequence, uint32_t previous)
    {
        const uint32_t delta = sequence - previous;
        return delta != 0 && delta < 0x80000000u;
    }

    inline bool HasExpired(uint64_t now, uint64_t lastReceived)
    {
        return now - lastReceived >= TimeoutMs;
    }

    inline std::array<int32_t, RequestSize> Encode(const Request& request)
    {
        return {Magic, 1, request.ViewId, std::bit_cast<int32_t>(request.Sequence), request.Active ? 1 : 0,
            std::bit_cast<int32_t>(request.Direction[0]), std::bit_cast<int32_t>(request.Direction[1]),
            std::bit_cast<int32_t>(request.Direction[2]), request.Options.Range, request.Options.Damage,
            (request.Options.InstantKill ? 1 : 0) | (request.Options.DestroyObjects ? 2 : 0)};
    }

    inline bool Decode(std::span<const int32_t> packet, Request& result)
    {
        if (packet.size() != RequestSize || packet[0] != Magic || packet[1] != 1 || packet[2] <= 0 ||
            (packet[4] != 0 && packet[4] != 1) || packet[8] < 10 || packet[8] > 200 ||
            packet[9] < 10 || packet[9] > 2000 || (packet[10] & ~3) != 0)
            return false;

        std::array<float, 3> direction{std::bit_cast<float>(packet[5]), std::bit_cast<float>(packet[6]),
            std::bit_cast<float>(packet[7])};
        float lengthSquared = 0.f;
        for (float value : direction)
        {
            if (!std::isfinite(value) || std::abs(value) > 1.01f)
                return false;
            lengthSquared += value * value;
        }
        if (lengthSquared < 0.9f || lengthSquared > 1.1f)
            return false;
        const float inverseLength = 1.f / std::sqrt(lengthSquared);
        for (float& value : direction)
            value *= inverseLength;

        result = {packet[2], std::bit_cast<uint32_t>(packet[3]), packet[4] != 0, direction,
            {packet[8], packet[9], (packet[10] & 1) != 0, (packet[10] & 2) != 0}};
        return true;
    }

    inline std::array<int32_t, AckSize> Acknowledge(const Request& request)
    {
        return {Magic, 2, request.ViewId, std::bit_cast<int32_t>(request.Sequence)};
    }

    inline bool IsAcknowledgement(std::span<const int32_t> packet, int viewId)
    {
        return packet.size() == AckSize && packet[0] == Magic && packet[1] == 2 && packet[2] == viewId;
    }
}
