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
                    enum command   {
                        FRAMEBUFFER_READ = 0x20,
                        /*!< command to read the framebuffer of the transceiver*/
                        FRAMEBUFFER_WRITE = 0x60,
                        /*!< command to write the framebuffer of the transceiver*/
                        REGISTER_READ     = 0x80,
                        /*!< command to read a register*/
                        REGISTER_WRITE    = 0xC0,
                        /*!< command to write a register*/
                        SRAM_READ         = 0x00,
                        /*!< command to read the SRAM of the transceiver*/
                        SRAM_WRITE        = 0x40
                        /*!< command to write the SRAM of the transceiver*/
                    };
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
                    enum address {
                        SNOP        = 0x00,
                        /*!< FIXME*/
                        TRXSTATUS   = 0x01,
                        /*!< signals the current state of the radio
                         *   transceiver/CCA measurement (a read access to
                         *   this register clears bits CCA_DONE and CCA_STATUS)*/
                        TRXSTATE    = 0x02,
                        /*!< controls the radio transceiver states*/
                        TRXCTRL0    = 0x03,
                        /*!< controls the drive current of the digital output
                         *   pads and the CLKM clock rate*/
                        PHYTXPWR    = 0x05,
                        /*!< sets the transmit power and controls the FCS
                         *   algorithm for TX aperations*/
                        PHYRSSI     = 0x06,
                        /*!< indicates the current received signal strength
                         *   (RSSI) and the FCS validity of a received frame*/
                        PHYED       = 0x07,
                        /*!< contains the result after an ED measurement*/
                        PHYCCCCA    = 0x08,
                        /*!< contains bits to initiate and control the CCA
                         *   measurement as well as to set the channel center 
                         *   frequency */
                        CCATHRES    = 0x09,
                        /*!< contains the threshold level for CCA-ED
                         * measurements*/
                        IRQMASK     = 0x0E,
                        /*!< used to enable/disable interrupt events*/
                        IRQSTATUS   = 0x0F,
                        /*!< contains the status of the individual interrupts
                         *   (a read access to this register resets all
                         *   interrupt bits)*/
                        VREGCTRL    = 0x10,
                        /*!< controls the use of the voltage regulators and
                         *   indicates their status*/
                        BATMON      = 0x11,
                        /*!< configures the battery monitor*/
                        XOSCCTRL    = 0x12,
                        /*!< controls the operation of the crystal oscillator*/
                        PLLCF       = 0x1A,
                        /*!< controls the operation of the center frequency
                         *   calibration loop */
                        PLLDCU      = 0x1B,
                        /*!< controls the operation of the delay cell
                         *   calibration loop */
                        PARTNUM     = 0x1C,
                        /*!< contains the radio transceiver part number*/
                        VERSIONNUM  = 0x1D,
                        /*!< contains the radio transceiver version number*/
                        MANID0      = 0x1E,
                        /*!< contains 1st part of stored JEDEC manufacturer ID*/
                        MANID1      = 0x1F,
                        /*!< contains 2nd part of stored JEDEC manufacturer ID*/
                        SHORTADDR0  = 0x20,
                        /*!< contains 1st part of 16 bit short address*/
                        SHORTADDR1  = 0x21,
                        /*!< contains 2nd part of 16 bit short address*/
                        PANID0      = 0x22,
                        /*!< contains 1st part of 16 bit PAN ID*/
                        PANID1      = 0x23,
                        /*!< contains 2nd part of 16 bit PAN ID*/
                        IEEEADDR0   = 0x24,
                        /*!< contains 1st part of 64 bit IEEE address*/
                        IEEEADDR1   = 0x25,
                        /*!< contains 2nd part of 64 bit IEEE address*/
                        IEEEADDR2   = 0x26,
                        /*!< contains 3rd part of 64 bit IEEE address*/
                        IEEEADDR3   = 0x27,
                        /*!< contains 4th part of 64 bit IEEE address*/
                        IEEEADDR4   = 0x28,
                        /*!< contains 5th part of 64 bit IEEE address*/
                        IEEEADDR5   = 0x29,
                        /*!< contains 6th part of 64 bit IEEE address*/
                        IEEEADDR6   = 0x2A,
                        /*!< contains 7th part of 64 bit IEEE address*/
                        IEEEADDR7   = 0x2B,
                        /*!< contains 8th part of 64 bit IEEE address*/
                        XAHCTRL     = 0x2C,
                        /*!< controls the TX_ARET transaction in the Extended
                         *   Operating Mode*/
                        CSMASEED0   = 0x2D,
                        /*!< contains a fraction of the CSMA_SEED value for the
                         *   CSMA-CA algorithm*/
                        CSMASEED1   = 0x2E,
                        /*!< contains a fraction of the CSMA_SEED value for the
                         *   CSMA-CA algorithm*/
                    };
                    // --- default register values ----------------------------
                    enum value   {
                        DEFAULT_TRXSTATUS  = 0x00,
                        DEFAULT_TRXSTATE   = 0x00,
                        DEFAULT_TRXCTRL0   = 0x19,
                        DEFAULT_PHYTXPWR   = 0x00,
                        DEFAULT_PHYCCCCA   = 0x2B,
                        DEFAULT_CCATHRES   = 0xC7,
                        DEFAULT_IRQMASK    = 0xFF,
                        DEFAULT_IRQSTATUS  = 0x00,
                        DEFAULT_BATMON     = 0x02,
                        DEFAULT_XOSCCTRL   = 0xF0,
                        DEFAULT_PLLCF      = 0x5F,
                        DEFAULT_PLLDCU     = 0x20,
                        DEFAULT_PARTNUM    = 0x02,
                        DEFAULT_VERSIONNUM = 0x02,/*!< FIXME \todo AT86RF230 Revision B*/
                        DEFAULT_MANID0     = 0x1F,
                        DEFAULT_MANID1     = 0x00,
                        DEFAULT_XAHCTRL    = 0x38,
                        DEFAULT_CSMASEED0  = 0xEA,
                        DEFAULT_CSMASEED1  = 0xC2,
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
                        uint8_t TRX_STATUS          : 5;
                        /*!< current radio transceiver status*/
                        uint8_t Reserved            : 1;
                        bool    CCA_STATUS          : 1;
                        /*!< indicates the result of a CCA request*/
                        bool    CCA_DONE            : 1;
                        /*!< indicates if a CCA request is completed*/
                    } TRXSTATUS;
                    struct {
                        uint8_t TRX_CMD             : 5;
                        /*!< write access initiates a radio transceiver state
                         *   transistion */
                        uint8_t TRAC_STATUS         : 3;
                        /*!< FIXME*/
                    } TRXSTATE;
                    struct {
                        uint8_t CLKM_CTRL           : 3;
                        /*!< sets clock rate of pin CLKM*/
                        bool    CLKM_SHA_SEL        : 1;
                        /*!< defines the commencement of the CLKM clock rate
                         *   modifications*/
                        uint8_t PAD_IO_CLKM         : 2;
                        /*!< set the output driver current of pin CLKM*/
                        uint8_t PAD_IO              : 2;
                        /*!< set the output driver current of digital ouput
                         *   pads MISO and IRQ*/
                    } TRXCTRL0;
                    struct {
                        uint8_t TX_PWR              : 4;
                        /*!< FIXME*/
                        uint8_t Reserved            : 3;
                        bool    TX_AUTO_CRC_ON      : 1;
                        /*!< controls the automatic FCS generation for TX
                         *   operations*/
                    } PHYTXPWR;
                    struct {
                        uint8_t RSSI                : 5;
                        /*!< contains the result of the automated RSSI
                         * measurement*/
                        uint8_t Reserved            : 2;
                        bool    RX_CRC_VALID        : 1;
                        /*!< indicates whether a received frame has a valid FCS*/
                    } PHYRSSI;
                    struct {
                        uint8_t CHANNEL             : 5;
                        /*!< defines the RX/TX channel according to
                         *   IEEE 802.15.4*/
                        uint8_t CCA_MODE            : 2;
                        /*!< selected CCA_MODE*/
                        bool    CCA_REQUEST         : 1;
                        /*!< initiates a manual CCA measurement*/
                    } PHYCCCCA;
                    struct {
                        uint8_t CCA_ED_THRES        : 4;
                        /*!< defines the threshold value of the CCA-ED
                         *   measurements*/
                        uint8_t Reserved            : 4;
                    } CCATHRES;
                    struct {
                        bool    PLL_LOCK            : 1;
                        /*!< indicates PLL lock*/
                        bool    PLL_UNLOCK          : 1;
                        /*!< indicates PLL unlock*/
                        bool    RX_START            : 1;
                        /*!< indicates a SFD detection (the TRX_STATE changes
                         *   to BUSY_RX)*/
                        bool    TRX_END             : 1;
                        /*!< indicates the completion of a frame
                         *   reception/transmission*/
                        uint8_t Reserverd           : 2;
                        bool    TRX_UR              : 1;
                        /*!< indicates a frame buffer access violation
                         *   (underrun)*/
                        bool    BAT_LOW             : 1;
                        /*!< indicates a supply voltage below the programmed
                         *   threshold*/
                    } IRQ;
                    struct {
                        uint8_t Reserved1           : 2;
                        bool    DVDD_OK             : 1;
                        /*!< indicates if the internal 1.8V regulated supply
                         *   voltage DVDD has settled*/
                        bool    DVREG_EXT           : 1;
                        /*!< defines whether the internal digital voltage
                         *   regulator or an external regualator is used*/
                        uint8_t Reserved2           : 2;
                        bool    AVDD_OK             : 1;
                        /*!< indicates if the internal 1.8 regulated supply
                         *   voltage AVDD has settled*/
                        bool    AVREG_EXT           : 1;
                        /*!< defines whether the internal analog voltage
                         * regulator or an external regulator is used*/
                    } VREGCTRL;
                    struct {
                        uint8_t BATMON_VTH          : 4;
                        /*!< defines the threshold value for the battery
                         * monitor*/
                        bool    BATMON_HR           : 1;
                        /*!< selects the range and resolution of the battery
                         *   monitor*/
                        bool    BATMON_OK           : 1;
                        /*!< indicates the level of the external supply woltage
                         *   with respect to the programmed threshold*/
                        uint8_t Reserved            : 2;
                    } BATMON;
                    struct {
                        uint8_t XTAL_TRIM           : 4;
                        /*!< page 70 at86rf230 manual*/
                        uint8_t XTAL_MODE           : 4;
                        /*!< sets the operating mode of the crystal oscillator*/
                    } XOSCCTRL;
                    struct {
                        uint8_t Reserved            : 7;
                        bool    PLL_CF_START        : 1;
                        /*!< initiates the center frequency calibration*/
                    } PLLCF;
                    struct {
                        uint8_t Reserved            : 7;
                        bool    PLL_DCU_START       : 1;
                        /*!< initiates the delay cell calibration*/
                    } PLLDCU;
                    struct {
                        uint8_t Reserved            : 1;
                        uint8_t MAX_CSMA_RETRIES    : 3;
                        /*!< specifies the maximum number of retries to repeat
                         *   the random back-off/CCA procedure before a TX_ARET
                         *   transaction is cancelled*/
                        uint8_t MAX_FRAME_RETRIES   : 3;
                        /*!< specifies the maximum number of frame
                         *   retransmission in a TX_ARET transaction*/
                    } XAHCTRL;
                    struct {
                        uint8_t CSMA_SEED_1         : 3;
                        /*!< contains the upper 3 bit of the CSMA_SEED*/
                        bool    I_AM_COORD          : 1;
                        /*!< set if node is PAN coordinator*/
                        uint8_t Reserved            : 1;
                        bool    AACK_SET_PD         : 1;
                        /*!< defines the content of the frame pending subfield
                         *   for acknowledgement frames*/
                        uint8_t MIN_BE              : 2;
                        /*!< defines the minimal back-off exponent used in the
                         *   CSMA-CA algorithm */
                    } CSMASEED1;
                    uint8_t value;
                } __attribute__ ((packed));
                // --- default values -----------------------------------------
                /*! \brief  %Specification of default values of the AT86RF230
                 *          radio controller.
                 */
                struct defaultValue {
                    enum {
                        NOP                          = 0x00,
                        P_ON                         = 0x00,
                        BUSY_RX                      = 0x01,
                        BUSY_TX                      = 0x02,
                        TX_START                     = 0x02,
                        FORCE_TRX_OFF                = 0x03,
                        RX_ON                        = 0x06,
                        TRX_OFF                      = 0x08,
                        PLL_ON                       = 0x09,
                        SLEEP                        = 0x0F,
                        BUSY_RX_AACK                 = 0x11,
                        BUSY_TX_ARET                 = 0x12,
                        RX_AACK_ON                   = 0x16,
                        TX_ARET_ON                   = 0x19,
                        RX_ON_NOCLK                  = 0x1c,
                        RX_AACK_ON_NOCLK             = 0x1d,
                        BUSY_RX_AACK_NOCLK           = 0x1e,
                        STATE_TRANSITION_IN_PROGRESS = 0x1f,
                        SUCCESS                      = 0x00,
                        SUCCESS_DATA_PENDING         = 0x01,
                        CHANNEL_ACCESS_FAILURE       = 0x03,
                        NO_ACK                       = 0x05,
                        INVALID                      = 0x07,
                    };
                };
                // --- timings ------------------------------------------------
                /*! \brief  Time %specification of the AT86RF230 radio controller.
                 *
                 *  Defines how long operations or waiting intervalls of this
                 *  radio controller last.
                 */
                struct Duration {
                    enum {
                        TRX_RESET_TIME_US      = 6,
                        /*!< duration while reset=low is asserted*/
                        TRX_INIT_TIME_US       = 510,
                        /*!< duration for transceiver to reach TRX_OFF for the
                         *   first time*/
                        TRX_CCA_TIME_US        = 140,
                        /*!< duration of a CCA measurement*/
                        TRX_CHIP_RESET_TIME_US = 625
                        /*!< duration for transceiver reset*/
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
