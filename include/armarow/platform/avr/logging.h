#pragma once

#include <config.h>
#include <avr-halib/ext/loggingDevice.h>
#include <avr-halib/ext/uartLogging.h>
#include <avr-halib/avr/uartSync.h>
#include <avr-halib/avr/sleep.h>

namespace platform
{
namespace avr
{
namespace logging
{
    typedef config::LoggingUartConfig Config;
    typedef avr_halib::power::UartSleepSynchronizer<Config> Sync;
}
}
}

setLoggingConfig(::platform::avr::logging::Config);
setLoggingDevice(::avr_halib::logExt::devices::Uart);

#include <avr-halib/ext/logging.h>
namespace platform{
namespace avr{
namespace logging{
    using avr_halib::logExt::log;
}
}
}
