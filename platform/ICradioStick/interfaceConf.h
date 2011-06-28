#pragma once

#include "avr-halib/avr/spi.h"
#include "stdint.h"

UseInterrupt(SIG_INPUT_CAPTURE1);

struct Portmap		// portmap for atmega1281
{
    union
    {
        struct		// pin interrupt: d 4;
        {
            uint8_t __pad0 [0x29];
            uint8_t   : 4;
            bool pin  : 1;		// PIND (0x29), bit 4
            uint8_t   : 7;
            bool ddr  : 1;		// DDRD (0x2a), bit 4
            uint8_t   : 7;
            bool port : 1;		// PORTD (0x2b), bit 4
        } interrupt;
        struct		// pin sleep: b 4;
        {
            uint8_t __pad0 [0x23];
            uint8_t   : 4;
            bool pin  : 1;		// PINB (0x23), bit 4
            uint8_t   : 7;
            bool ddr  : 1;		// DDRB (0x24), bit 4
            uint8_t   : 7;
            bool port : 1;		// PORTB (0x25), bit 4
        } sleep;
        struct		// pin reset: b 5;
        {
            uint8_t __pad0 [0x23];
            uint8_t   : 5;
            bool pin  : 1;		// PINB (0x23), bit 5
            uint8_t   : 7;
            bool ddr  : 1;		// DDRB (0x24), bit 5
            uint8_t   : 7;
            bool port : 1;		// PORTB (0x25), bit 5
        } reset;
        struct		// pin cs: b 0;
        {
            uint8_t __pad0 [0x23];
            bool pin  : 1;		// PINB (0x23), bit 0
            uint8_t   : 7;
            bool ddr  : 1;		// DDRB (0x24), bit 0
            uint8_t   : 7;
            bool port : 1;		// PORTB (0x25), bit 0
        } cs;
        struct		// pin sck: b 1;
        {
            uint8_t __pad0 [0x23];
            uint8_t   : 1;
            bool pin  : 1;		// PINB (0x23), bit 1
            uint8_t   : 7;
            bool ddr  : 1;		// DDRB (0x24), bit 1
            uint8_t   : 7;
            bool port : 1;		// PORTB (0x25), bit 1
        } sck;
        struct		// pin mosi: b 2;
        {
            uint8_t __pad0 [0x23];
            uint8_t   : 2;
            bool pin  : 1;		// PINB (0x23), bit 2
            uint8_t   : 7;
            bool ddr  : 1;		// DDRB (0x24), bit 2
            uint8_t   : 7;
            bool port : 1;		// PORTB (0x25), bit 2
        } mosi;
        struct		// pin miso: b 3;
        {
            uint8_t __pad0 [0x23];
            uint8_t   : 3;
            bool pin  : 1;		// PINB (0x23), bit 3
            uint8_t   : 7;
            bool ddr  : 1;		// DDRB (0x24), bit 3
            uint8_t   : 7;
            bool port : 1;		// PORTB (0x25), bit 3
        } miso;
    };
};

struct SpiCfg : public Spi<CPUClock>
{
    typedef CPUClock Controller_Configuration;
    enum {
        useInterupt=false,
        dataDirection=msb,
        leadingEdge=rising,
        sampleEdge=leading,
        clockPrescaler=ps2,
        busywaitput=true
    };
};

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
