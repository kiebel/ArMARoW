/*******************************************************************************
 *
 * Copyright (c) 2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/
#pragma once
/* === includes ============================================================= */
#include <stdint.h>

#define CPU_FREQUENCY F_CPU
#include "avr-halib/avr/portmap.h"
#include "avr-halib/avr/spi.h"
#include "avr-halib/share/freq.h"

UseInterrupt(SIG_INPUT_CAPTURE1);
/* === types ================================================================ */
using avr_halib::config::Frequency;
typedef Frequency< F_CPU > CPUClock;

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

struct SPI		// portmap for atmega1281
{
    union
    {
        struct		// pin ss: b 0;
        {
            uint8_t __pad0 [0x23];
            bool pin  : 1;		// PINB (0x23), bit 0
            uint8_t   : 7;
            bool ddr  : 1;		// DDRB (0x24), bit 0
            uint8_t   : 7;
            bool port : 1;		// PORTB (0x25), bit 0
        } ss;
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

// atmega1281 ist mit 16MHz getaktet
// USB befindet sich an UART 1
struct AT86RF230		// portmap for atmega1281
{
    typedef InterruptRC irq_t;
    typedef SpiMaster<SpiCfg> spi_t;
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
