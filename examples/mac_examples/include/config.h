#pragma once

#include <platform.h>
#include <armarow/debug.h>
#include <radio.h>
#include <armarow/mac/simple802.15.4.h>
#include <avr-halib/avr/InterruptManager/InterruptManager.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/common/frequency.h>
#include <boost/mpl/joint_view.hpp>

using platform::RadioDriver;
using armarow::common::Error;
using avr_halib::config::Frequency;
using armarow::mac::Simple802_15_4;

struct RadioConfig : public RadioDriver::DefaultConfig
{
    static const bool rxOnIdle = true;
    static const bool useInterrupt = true;
};

struct ClockConfig : public platform::Timer3BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint32_t TickValueType;
};

struct MacConfig : public Simple802_15_4::DefaultConfig
{
    typedef RadioDriver BaseLayer;
    typedef RadioConfig BaseLayerConfig;
    static const uint16_t pan     = 0;
    static const uint16_t address = 1;
};

typedef Simple802_15_4::configure< MacConfig >::type Mac;
typedef Interrupt::InterruptManager< Mac::InterruptSlotList > IM;
