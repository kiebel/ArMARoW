#pragma once

#include "platform.h"

#include <avr-halib/avr/interrupt.h>
#include <armarow/phy/at86rf230/at86rf230-rc.h>
#include <avr-halib/avr/newSpi.h>

#include "radio_portmap.h"

UseInterrupt(SIG_INPUT_CAPTURE1);

namespace platform {
namespace config {
    using armarow::phy::At86Rf230;
    using armarow::phy::At86Rf230CFG;

    struct RadioInterface
    {
        typedef RadioPortMap PortMap;

        struct SpiBaseConfig
        {
            typedef avr_halib::regmaps::local::Spi RegMap;
            enum CommunicationParameters
            {
                clockPrescaler=RegMap::ps2
            };
            template<typename Config>
            struct Spi : public avr_halib::drivers::Spi<Config> {};
        };

        class Interrupts {
        public:
            Interrupts() {}
            template< typename T, void(T::*Fxn)() > static void init(T* obj) {
                TCCR1B |= (_BV(ICNC1) | _BV(ICES1));
                TIFR1  |= _BV(ICF1);
                DDRC |= (0x01 << 2);
                //-----------------------------------------------------------------
                redirectISRM(SIG_INPUT_CAPTURE1, Fxn, *obj);
                enable();
            }
            static void enable()  { TIMSK1 |= _BV(ICIE1); PORTC |=  (0x01 << 2); }
            static void disable() { TIMSK1 &= ~_BV(ICIE1);PORTC &= ~(0x01 << 2); }
        };
    };

    template<typename Config=At86Rf230CFG>
    struct RadioDriver : public At86Rf230< RadioInterface, Config >{};
}
}
