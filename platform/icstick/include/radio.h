#pragma once

#include <armarow/phy/at86rf230/at86rf230-rc.h>
#include <avr-halib/avr/newSpi.h>

namespace platform {
namespace icstick {
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
    };

    template<typename Config=At86Rf230CFG>
    struct RadioDriver : public At86Rf230< RadioInterface, Config >{};
}
}
