#pragma once

#include "config.h"

namespace platform
{
    using namespace deRFmega128;

    struct RadioHal{};
};

#include <armarow/platform/avr/logging.h>
#include <armarow/platform/avr/clock.h>
#include <armarow/platform/avr/power.h>

namespace platform {
namespace power {
    using avr::power::Idler;
}
namespace logging {
    using avr::logging::log;
}
}

using platform::logging::log;
using platform::config::CPUClock;
using platform::power::Idler;
