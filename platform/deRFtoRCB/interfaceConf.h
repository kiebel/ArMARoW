#pragma once

#include "stdint.h"
#if 0
UseInterrupt(SIG_INPUT_CAPTURE1);

class InterruptRC {
    public:
        InterruptRC() {}
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
#endif
/* TRX24 - Receive start interrupt */
UseInterrupt(TRX24_RX_START_vect);
/*TRX24 - RX_END interrupt */
UseInterrupt(TRX24_RX_END_vect);
/* TRX24 - TX_END interrupt */
UseInterrupt(TRX24_TX_END_vect);
