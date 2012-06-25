#pragma once

#include <platform.h>
#include <radio.h>
#include <armarow/mac/simple802.15.4.h>
#include <avr-halib/avr/InterruptManager/InterruptManager.h>
#include <avr-halib/avr/oneShotTimer.h>
#include <avr-halib/common/frequency.h>
#include <boost/mpl/joint_view.hpp>
#include <idler.h>

using platform::RadioDriver;
using armarow::common::Error;
using avr_halib::config::Frequency;
using armarow::mac::Simple802_15_4;

struct RadioConfig : public RadioDriver::DefaultConfig
{
    static const bool rxOnIdle = true;
    static const bool useInterrupt = true;
};

struct TriggerConfig : public platform::Timer3BaseConfig
{
    static const bool overflowInt = false;
    static const bool async       = false;
    typedef Frequency<1000> BaseFrequency;
};

struct MacConfig : public Simple802_15_4::DefaultConfig
{
    typedef RadioDriver BaseLayer;
    typedef RadioConfig BaseLayerConfig;
    static const uint16_t pan     = 0;
    static const uint16_t address = ADDRESS;
};

typedef Simple802_15_4::configure< MacConfig >::type Mac;
typedef avr_halib::drivers::OneShotTimer::configure< TriggerConfig >::type Trigger;


typedef Mac::MessageType MessageType;

struct RelayContent
{
    armarow::mac::simple802_15_4::ShortAddress source;
    uint8_t seqNr;
    int8_t rssi;
    uint8_t lqi;
} __attribute__((packed));

typedef MessageType::customize<RelayContent>::type RelayMessage;
