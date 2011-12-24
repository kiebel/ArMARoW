#pragma once

#include <stdint.h>
#include "registers.h"
#include "frameBuffer.h"

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
		};

        /** \brief radio operation states **/
        enum State
        {
            NOP           = 0x00,   /**< do nothing only useful for setState**/
            BUSY_RX       = 0x01,   /**< radio busy receiving**/
            BUSY_TX       = 0x02,   /**< radio busy transmitting**/
            FORCE_TRX_OFF = 0x03,   /**< force radio to enter TRX_OFF**/
            FORCE_TX_ON   = 0x04,   /**< force radio to enter TX_ON**/
            RX_ON         = 0x06,   /**< ready for receiving**/
            TRX_OFF       = 0x08,   /**< \todo see documentation**/
            TX_ON         = 0x09,   /**< ready for transmission**/
            SLEEP         = 0x0f,   /**< deactivate radio**/
            CHANGING      = 0x1f    /**< state change in progress**/
        };

        typedef avr_halib::regmaps::local::atmega128rfa1::Registers   RegMap;
        typedef avr_halib::regmaps::local::atmega128rfa1::FrameBuffer FrameBufferMap;
}
}
}
