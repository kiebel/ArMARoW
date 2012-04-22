#pragma once

#include <armarow/phy/atmega128rfa1/atmega128rfa1-rc.h>

namespace platform {
namespace config {
    using armarow::phy::ATmega128RfA1;
    using armarow::phy::ATmega128RfA1CFG;

    template<typename Config=ATmega128RfA1CFG>
    struct RadioDriver : public ATmega128RfA1< Config >{};
}
}
