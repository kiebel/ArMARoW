#pragma once

#include "config.h"

namespace platform
{
    using namespace deRCB128RFA1;
};

#include <armarow/platform/avr/logging.h>
#include <armarow/platform/avr/clock.h>
#include <armarow/platform/avr/power.h>

namespace platform {
namespace power
{
    using avr::power::Idler;
}
namespace logging
{
    using avr::logging::log;
}
}

using platform::logging::log;
using platform::config::CPUClock;
using platform::power::Idler;
using platform::config::Clock1;
using platform::config::Clock2;
