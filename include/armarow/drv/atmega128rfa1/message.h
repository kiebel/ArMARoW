#pragma once

namespace armarow {
namespace drv {
namespace atmega128rfa1
{
    struct MessageHeader{};

    template<bool useLQI, bool useRSSI>
    struct MessageProperties;

    template<>
    struct MessageProperties<true, true>
    {
        uint8_t lqi;
        uint8_t rssi;
    };

    template<>
    struct MessageProperties<true, false>
    {
                uint8_t lqi;
    };

    template<>
    struct MessageProperties<false, true>
    {
        uint8_t rssi;
    };

    template<>
    struct MessageProperties<false, false>
    {

    };
}
}
}
