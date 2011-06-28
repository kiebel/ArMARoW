#pragma once

#include <avr-halib/ext/loggingDevice.h>
#include <avr-halib/ext/uartLogging.h>
#include <avr-halib/avr/regmaps.h>
#include <boost/mpl/list.hpp>
#include <avr-halib/avr/sleep.h>

typedef Uart1<CPUClock,115200> logConf;

setLoggingConfig(logConf);

setLoggingDevice(avr_halib::logging::devices::Uart);

typedef avr_halib::power::UartSleepSynchronizer<logConf> LogSync;

#include <avr-halib/ext/logging.h>

using avr_halib::logging::log;

