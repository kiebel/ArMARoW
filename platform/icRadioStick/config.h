#pragma once

#include <avr-halib/avr/interrupt.h>
#include <avr-halib/avr/regmaps.h>
#include <avr-halib/regmaps/local.h>
#include <avr-halib/share/freq.h>
#include <avr-halib/avr/sleep.h>

UseInterrupt(SIG_OUTPUT_COMPARE1A);
UseInterrupt(SIG_OUTPUT_COMPARE3A);

namespace platform
{
namespace icRadioStick
{
namespace config
{
    typedef avr_halib::config::Frequency<F_CPU> CPUClock;
    typedef avr_halib::regmaps::local::Timer1 Clock1;
    typedef avr_halib::regmaps::local::Timer3 Clock2;
    typedef Uart1<CPUClock, 19200> LoggingUartConfig;
    static const avr_halib::power::SleepModes sleepMode=avr_halib::power::idle;
}
}
}
