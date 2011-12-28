#pragma once

#include <stdint.h>
#include <platform.h>

#include "register.h"
#include "framebuffer.h"

namespace armarow {
namespace drv {
namespace atmega128rfa1 {
    namespace specification {
        enum Channels{
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

		struct Constants
		{
			/** \brief the amount of symbols transmitted in one second **/
			static const uint32_t symbolRate = 62500;
            /** \brief maximum bytes in message payload **/
            static const uint8_t maxPayload = 128;
		};

        struct States
        {
            /** \brief radio operation states **/
            enum State
            {
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

        typedef States::State StateType;

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
