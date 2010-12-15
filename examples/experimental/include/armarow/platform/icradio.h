/*******************************************************************************
 *
 * Copyright (c) 2010-2011 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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
#ifndef __ARMAROW_PLATFORM_ICRADIO_h__
#define __ARMAROW_PLATFORM_ICRADIO_h__
#define UARTLOGDEVICE Uart1
/* === includes ============================================================= */
#include <stdint.h>

#define CPU_FREQUENCY F_CPU
#include <avr-halib/avr/portmap.h>
#include <avr-halib/share/freq.h>
#include <armarow/common/spi.h>
/* === types ================================================================ */
using avr_halib::config::Frequency;
typedef Frequency< F_CPU > CPUClock;

namespace armarow {
    namespace platform {
        namespace icradio {
            struct PortmapRC {  // portmap for atmega1281
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
            struct SpiCfg : public Spi<CPUClock> {
                typedef CPUClock Controller_Configuration;
                enum {
                    useInterupt    = false,
                    dataDirection  = msb,
                    leadingEdge    = rising,
                    sampleEdge     = leading,
                    clockPrescaler = ps2,
                    busywaitput    = true
                };
            };
            typedef typename armarow::common::SPI<PortmapRC,SpiCfg> SPI;
        }
    }
}
#endif  // __ARMAROW_PLATFORM_ICRADIO_h__
