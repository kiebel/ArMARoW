#pragma once

#include <avr-halib/ext/loggingDevice.h>
#include <avr-halib/ext/uartLogging.h>
#include <avr-halib/avr/uartSync.h>
#include <avr-halib/avr/regmaps.h>
#include <avr-halib/avr/sleep.h>

namespace platform
{
    typedef Uart0<CPUClock,19200> logConf;
}

setLoggingConfig(platform::logConf);
setLoggingDevice(::avr_halib::logExt::devices::Uart);

typedef avr_halib::power::UartSleepSynchronizer<platform::logConf> LogSync;

#include <avr-halib/ext/logging.h>

using avr_halib::logExt::log;
