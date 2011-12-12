#pragma once

#include <config.h>

namespace platform
{
namespace avr
{
namespace clock
{
    struct Clock1BaseConfig
    {
        typedef platform::config::CPUClock TimerFrequency;
        typedef platform::config::Clock1 Timer;
    };

    struct Clock2BaseConfig
    {
        typedef platform::config::CPUClock TimerFrequency;
        typedef platform::config::Clock2 Timer;
    };
}
}
}
