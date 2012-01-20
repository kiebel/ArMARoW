#pragma once

#include <stdint.h>
#include <platform.h>

#include "register.h"
#include "framebuffer.h"

namespace armarow {
namespace drv {
namespace atmega128rfa1 {
    namespace specification {
        struct Channels{
            enum Channel{
                minChannel = 11,
                Channel11  = 11,
                Channel12  = 12,
                Channel13  = 13,
                Channel14  = 14,
                Channel15  = 15,
                Channel16  = 16,
                Channel17  = 17,
                Channel18  = 18,
                Channel19  = 19,
                Channel20  = 20,
                Channel21  = 21,
                Channel22  = 22,
                Channel23  = 23,
                Channel24  = 24,
                Channel25  = 25,
                Channel26  = 26,
                MaxChannel = 26
            };
        };

        struct CCAModes{
            enum CCAMode{
                CarrierSenseOrEnergy  = 0,
                Energy                = 1,
                CarrierSense          = 2,
                CarrierSenseAndEnergy = 3
            };
        };

        struct States{
            /** \brief radio operation states **/
            enum State{
                nop           = 0x00,   /**< do nothing only useful for setState()**/
                busy_rx       = 0x01,   /**< radio busy receiving**/
                busy_tx       = 0x02,   /**< radio busy transmitting**/
                force_trx_off = 0x03,   /**< force radio to enter trx_off**/
                force_tx_on   = 0x04,   /**< force radio to enter tx_on**/
                rx_on         = 0x06,   /**< ready for receiving**/
                trx_off       = 0x08,   /**< transceiver deactivated**/
                tx_on         = 0x09,   /**< ready for transmission**/
                sleep         = 0x0f,   /**< deactivate radio**/
                changing      = 0x1f    /**< state change in progress**/
            };
        };

        typedef States::State     StateType;
        typedef Channels::Channel ChannelType;
        typedef int8_t            RSSIType;

        typedef CCAModes::CCAMode CCAModeType;
        typedef uint8_t           CCAThresholdType ;
        typedef bool              CCAType;
        typedef bool              SleepType;


		struct Constants
		{
            private:
                static const RSSIType rssi_base_val = -90;
                static const uint8_t ccaThresholdSize = 4;
                static const uint8_t ccaThresholdModifier = 2; 

            public:

			/** \brief the amount of symbols transmitted in one second **/
			static const uint32_t symbolRate = 62500;
            /** \brief maximum bytes in message payload **/
            static const uint8_t maxPayload = 128;
            static const CCAThresholdType minCCAThreshold = rssi_base_val;
            static const CCAThresholdType maxCCAThreshold = rssi_base_val + ccaThresholdModifier*((1<<ccaThresholdSize)-1);
		};

        typedef avr_halib::regmaps::local::atmega128rfa1::Registers   RegMap;
        typedef avr_halib::regmaps::local::atmega128rfa1::FrameBuffer FrameBufferMap;
}
}
}
}

LoggingOutput& operator<<(LoggingOutput& out, const armarow::drv::atmega128rfa1::specification::StateType& state)
{
    typedef armarow::drv::atmega128rfa1::specification::States States;
    switch(state)
    {
        case(States::nop)          : return out << PROGMEMSTRING("nothing");
        case(States::busy_rx)      : return out << PROGMEMSTRING("busy-rx");
        case(States::busy_tx)      : return out << PROGMEMSTRING("busy-tx");
        case(States::force_trx_off): return out << PROGMEMSTRING("force-off");
        case(States::force_tx_on)  : return out << PROGMEMSTRING("force-rx");
        case(States::rx_on)        : return out << PROGMEMSTRING("rx-ready");
        case(States::trx_off)      : return out << PROGMEMSTRING("trx-off");
        case(States::tx_on)        : return out << PROGMEMSTRING("tx-ready");
        case(States::sleep)        : return out << PROGMEMSTRING("sleep");
        case(States::changing)     : return out << PROGMEMSTRING("state-changing");
        default                    : return out << PROGMEMSTRING("unknown driver state");
    };

    return out;
}

LoggingOutput& operator<<(LoggingOutput& out, const armarow::drv::atmega128rfa1::specification::CCAModeType& mode)
{
    using armarow::drv::atmega128rfa1::specification::CCAModes;

    switch(mode)
    {
        case(CCAModes::CarrierSenseOrEnergy)  : return out << PROGMEMSTRING("carrier sensed or energy detected");
        case(CCAModes::CarrierSense)          : return out << PROGMEMSTRING("carrier sensed");
        case(CCAModes::Energy)                : return out << PROGMEMSTRING("energy detected");
        case(CCAModes::CarrierSenseAndEnergy) : return out << PROGMEMSTRING("carrier sensed and energy detected");
        default                              : return out << PROGMEMSTRING("unknown cca mode");
    };

    return out;
}

LoggingOutput& operator<<(LoggingOutput& out, const armarow::drv::atmega128rfa1::specification::ChannelType& channel)
{
    return out << uint16_t(channel);
}
