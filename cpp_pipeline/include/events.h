#pragma once
#include <cstdint>

struct alignas(64) Event {
    const int64_t instrument_id;
    const double price;
    const int64_t timestamp;

    Event(int64_t id, double p, int64_t ts)
        : instrument_id(id), price(p), timestamp(ts) {}
};
