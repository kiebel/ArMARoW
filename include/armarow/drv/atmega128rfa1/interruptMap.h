#pragma once

namespace armarow{
namespace drv{
namespace atmega128rfa1
{
    struct InterruptMap
    {
        enum Interrupts
        {
            pllLock               = 57,
            pllUnlock             = 58,
            rxStart               = 59,
            rxEnd                 = 60,
            ccaDone               = 61,
            addressMatch          = 62,
            txEnd                 = 63,
            awake                 = 64,
            symbolCounterMatch1   = 65,
            symbolCounterMatch2   = 66,
            symbolCounterMatch3   = 67,
            symbolCounterOverflow = 68,
            symbolCounterBackoff  = 69,
            aesReady              = 70,
            batLow                = 71,
        };
    };
}
}
}
