#pragma once

#include "atmega128rfa1/attr.h"

namespace armarow {
namespace drv {
    struct Atmega128RFA1
    {   
        /** \brief Default configuration
         *
         *  default configuration of atmega128rfa1 driver
         **/
        struct DefaultConfig{
            /** \brief defines if the radio will be ready to receive message directlyy after transmitting
             *
             *  default value: true
             **/
            static const bool rxOnIdle      = true;
            /** \brief defines if the radio driver will use interrupts
             *
             *  default value: true
             *  if interrupts are disabled the poll function must be called periodically.
             **/
            static const bool useInterrupts = true;

            static const bool fetchLQI = true;

            static const bool fetchRSSI = true;
        };
        
        template<typename Config = DefaultConfig>
        struct configure
        {
            typedef typename atmega128rfa1::AttributeHandler::configure< Config >::type type;
        };
    };
}
}
