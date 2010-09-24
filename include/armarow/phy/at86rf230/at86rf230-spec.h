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
#ifndef __ARMAROW_AT86RF230_SPEC_h__
#define __ARMAROW_AT86RF230_SPEC_h__

#include "armarow/phy/phy.h"

namespace armarow {
    namespace phy {
        namespace specification {
            /*! \brief  Hard- and software %specification of the AT86RF230
             *          radio controller.
             *
             *          Definition of constants and values for:
             *              - %SPI commands
             *              - registers (register maps, register addresses,
             *                register values)
             *              - supported PIB Attributes for Medium Access Control
             *  \ingroup PhyL RcSpec
             */
            struct At86Rf230 {
                // --- channels -----------------------------------------------
                /*! \brief  %Specification of the channels usable by the
                 *          radio controller.
                 *  \todo use of available channels with all channel pages
                 */
                struct Channel {
                    enum {
                        minChannel    = 11,
                        /*!< lowest available channel for the 2.4GHz band */
                        maxChannel    = 26,
                        /*!< highest available channel for the 2.4GHz band */
                        channelNumber = maxChannel - minChannel
                        /*!< number of available channels*/
                    };
                    static uint32_t supportedChannel() {
                        return (((1UL << maxChannel) - 1) & ~((1UL << minChannel) - 1));
                    }
                };
                // --- SPI ----------------------------------------------------
                /*! \brief  %SPI %specification of the AT86RF230 radio controller.
                 *
                 *  Defines commands and default values for the SPI connection.
                 */
                struct SPI {
                    enum command   {};
                    enum value     {};
                    union status_t {
                        uint8_t value;
                    } __attribute((packed));
                };
                // --- register defaults --------------------------------------
                /*! \brief  Register %specification of the AT86RF230
                 *          radio controller.
                 *
                 *  Defines addresses and default values for the register of
                 *  the radio controller.
                 */
                struct registerDefault {
                    // --- default register addresses -------------------------
                    enum address { };
                    // --- default register values ----------------------------
                    enum value   {
                        SNOP  = 0x00
                    };
                };
                // --- register maps ------------------------------------------
                /*! \brief  Register map %specification of the AT86RF230
                 *          radio controller.
                 *
                 *  Defines how values are mapped to the structure of the
                 *  registers of the AT86RF230 radio controller.
                 *  \note The type of <code>value</code> depends on the width of
                 *        the registers the default radio controller uses.
                 *        An overview of the available registers and their
                 *        structure can be found in the datasheet of the
                 *        radio controller.
                 */
                union registerMap {
                    struct {
                        uint8_t channel : 3;
                        uint8_t mode    : 3;
                        bool            : 1;
                        bool    state   : 1;
                    } REGMASK;
                    uint8_t value;
                } __attribute__ ((packed));
                // --- default values -----------------------------------------
                /*! \brief  %Specification of default values of the AT86RF230
                 *          radio controller.
                 */
                struct defaultValue {
                    enum { };
                };
                // --- timings ------------------------------------------------
                /*! \brief  Time %specification of the AT86RF230 radio controller.
                 *
                 *  Defines how long operations or waiting intervalls of this
                 *  radio controller last.
                 */
                struct Duration {
                    enum {
                        TRX_CHIP_RESET_TIME_US = 625
                        /*!< duration for transceiver reset */
                    };
                };
                // --- sizes chip specific ------------------------------------
                /*! \brief  %Specification of available memory of the AT86RF230
                 *          radio controller.
                 *
                 *  Defines sizes and numbers of available memory and queues.
                 */
                enum chipSize {
                    SIZE_FIFO    = 128,
                    SIZE_RAM     = 512
                };
                // --- supported PIB Attributes for Medium Access Control -----
                //TODO struct PIBAttributeMAC {enum {};};

                //TODO RSSI for ED is good, but find a better way for LQI calculation using the correlator value of the FCS!
                static uint8_t rssiToED(const int8_t pRssi) {
                    return ( (pRssi < -38) ? 0   : (pRssi - -38));
                }
                //TODO
                static uint8_t edToLQI(const uint8_t pEd) {
                    return ( (pEd > 63)    ? 255 : (pEd << 2));
                }
                /*! \brief  Maps a PA level value to overall transmission power.
                 *  \param[in]  pLevel 5-bit PA level
                 *  \return     Returns overall transmission power in dB (relative to 1mW).
                 */
                static int8_t paLevelTodBm(const uint8_t pLevel) {
                    if (pLevel >= 31) return (0);
                    switch (pLevel) {
                        case 30: case 29: case 28: case 27: return (-1);
                        case 26: case 25: case 24: case 23: return (-3);
                        case 22: case 21: case 20: case 19: return (-5);
                        case 18: case 17: case 16: case 15: return (-7);
                        case 14: case 13: case 12: case 11: return (-10);
                        case 10: case 9:  case 8:  case 7:  return (-15);
                        case 6:  case 5:  case 4:  case 3:  return (-25);
                        default: return (-32);
                    }
                }
                /*! \brief  Maps an overall transmission power to a PA Level value.
                 *  \param[in]  pDBm overall transmission power in dB
                 *  \return     Returns a 5-bit PA level.
                 */
                static uint8_t dBmToPaLevel(const int8_t pDBm) {
                    if (pDBm <= -32) return (0);
                    switch (pDBm) {
                        case -31: case -30: case -29: case -28: case -27:
                        case -26: case -25:
                            return (3);
                        case -24: case -23: case -22: case -21: case -20:
                        case -19: case -18: case -17: case -16: case -15:
                            return (7);
                        case -14: case -13: case -12: case -11: case -10:
                            return (11);
                        case  -9: case  -8: case  -7:
                            return (15);
                        case  -6: case  -5:
                            return (19);
                        case  -4: case  -3:
                            return (23);
                        default: return (31);
                    }
                }
            };
        } // namespace specification
    } // namespace phy
} // namespace armarow

#endif  //__ARMAROW_AT86RF230_SPEC_h__
