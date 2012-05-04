#pragma once

#include <platform.h>
#include <armarow/debug.h>
#include <radio.h>
#include <avr-halib/avr/InterruptManager/InterruptManager.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/common/frequency.h>
#include <boost/mpl/joint_view.hpp>

using platform::RadioDriver;
using armarow::common::Error;
using avr_halib::config::Frequency;

struct Config : public RadioDriver::DefaultConfig
{
    static const bool rxOnIdle = false;
    static const bool useInterrupt = false;
};

struct TriggerConfig : public platform::Timer1BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint8_t TickValueType;
};

struct ClockConfig : public platform::Timer3BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint32_t TickValueType;
};

typedef RadioDriver::configure< Config >::type Radio;
typedef avr_halib::drivers::Clock< TriggerConfig > Trigger;
typedef avr_halib::drivers::Clock< ClockConfig > Clock;

typedef boost::mpl::joint_view< Trigger::InterruptSlotList, Clock::InterruptSlotList > TimersSlotList;
typedef boost::mpl::joint_view< Radio::InterruptSlotList, TimersSlotList > InterruptSlotList;

typedef Interrupt::InterruptManager< InterruptSlotList > IM;
