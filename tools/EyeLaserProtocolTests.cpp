#include "src/hooks/eye_laser_protocol.h"

#include <cstdlib>
#include <iostream>
#include <limits>

using namespace Cheat::EyeLaserProtocol;

static int checks;
static void Require(bool condition, const char* message)
{
    ++checks;
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

int main()
{
    Request original{1001, 42, true, {0.6f, 0.f, 0.8f}, {200, 2000, false, true}};
    const auto valid = Encode(original);
    Request decoded;
    Require(Decode(valid, decoded), "valid request rejected");
    Require(decoded.ViewId == 1001 && decoded.Sequence == 42 && decoded.Active, "identity or state lost");
    Require(decoded.Options.Range == 200 && decoded.Options.Damage == 2000 &&
        !decoded.Options.InstantKill && decoded.Options.DestroyObjects, "settings lost");
    Require(std::abs(decoded.Direction[0] - 0.6f) < 0.00001f, "direction lost");
    Require(!Decode({}, decoded), "empty packet accepted");
    Require(!Decode(std::span(valid).first(10), decoded), "truncated packet accepted");
    std::array<int32_t, RequestSize + 1> oversized{};
    Require(!Decode(oversized, decoded), "oversized packet accepted");

    const auto rejectField = [&](size_t field, int32_t value, const char* message)
    {
        auto packet = valid;
        packet[field] = value;
        Require(!Decode(packet, decoded), message);
    };
    rejectField(0, 0, "unrelated event accepted");
    rejectField(1, 2, "acknowledgement treated as request");
    rejectField(2, 0, "missing avatar accepted");
    rejectField(2, -1, "negative avatar accepted");
    rejectField(4, 2, "invalid active flag accepted");
    rejectField(8, 9, "short range accepted");
    rejectField(8, 201, "excessive range accepted");
    rejectField(9, 9, "invalid damage accepted");
    rejectField(9, 2001, "excessive damage accepted");
    rejectField(10, 4, "unknown options accepted");
    rejectField(5, std::bit_cast<int32_t>(std::numeric_limits<float>::quiet_NaN()), "NaN accepted");
    rejectField(5, std::bit_cast<int32_t>(std::numeric_limits<float>::infinity()), "infinity accepted");
    rejectField(5, std::bit_cast<int32_t>(2.f), "unbounded direction accepted");
    auto zeroDirection = valid;
    zeroDirection[5] = zeroDirection[6] = zeroDirection[7] = 0;
    Require(!Decode(zeroDirection, decoded), "zero direction accepted");

    auto stop = original;
    stop.Active = false;
    Require(Decode(Encode(stop), decoded) && !decoded.Active, "stop request lost");
    const auto ack = Acknowledge(original);
    Require(IsAcknowledgement(ack, 1001), "valid acknowledgement rejected");
    Require(!IsAcknowledgement(ack, 1002), "another avatar's acknowledgement accepted");
    Require(!IsAcknowledgement(valid, 1001), "request treated as acknowledgement");
    Require(IsNewer(43, 42) && !IsNewer(42, 42) && !IsNewer(41, 42), "replay ordering incorrect");
    Require(IsNewer(0, 0xffffffffu) && !IsNewer(0xffffffffu, 0), "sequence wrap incorrect");
    Require(!HasExpired(2499, 1000) && HasExpired(2500, 1000), "disconnect timeout boundary incorrect");
    std::cout << "PASS: " << checks << " eye-laser protocol checks\n";
}
