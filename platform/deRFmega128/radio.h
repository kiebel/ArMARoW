#pragma once

#include "platform.h"

#include <avr-halib/avr/interrupt.h>
#include <armarow/phy/atmega128rfa1/atmega128rfa1-rc.h>

UseInterrupt(TRX24_RX_START_vect);
UseInterrupt(TRX24_RX_END_vect);
UseInterrupt(TRX24_TX_END_vect);

namespace platform {
namespace config {
    using armarow::phy::ATmega128RfA1;
    using armarow::phy::ATmega128RfA1CFG;

    template<typename Config=ATmega128RfA1CFG>
    struct RadioDriver : public ATmega128RfA1< RadioHal, Config >{};
}
}
