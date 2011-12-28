#pragma once

#include <avr-halib/avr/interrupt.h>
#include <armarow/drv/atmega128rfa1.h>

UseInterrupt(TRX24_RX_START_vect);
UseInterrupt(TRX24_RX_END_vect);
UseInterrupt(TRX24_TX_END_vect);

namespace platform {
namespace config {
    using armarow::drv::Atmega128RFA1;
    using armarow::drv::atmega128rfa1::DefaultConfig;

    template<typename Config=DefaultConfig>
    struct RadioDriver : public Atmega128RFA1< Config >{};
}
}
