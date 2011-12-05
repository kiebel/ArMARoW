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
#pragma once

#include <armarow/phy/phy.h>

namespace armarow {
    namespace phy {
        namespace specification {
            /*! \brief  Hard- and software %specification of the ATmega128RFA1
             *          radio controller.
             *
             *          Definition of constants and values for:
             *              - %SPI commands
             *              - registers (register maps, register addresses,
             *                register values)
             *              - supported PIB Attributes for Medium Access Control
             *  \ingroup PhyL RcSpec
             */
            struct ATmega128RfA1 {
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

		struct SignalParameters
		{
			/** \brief the amount of symbols transmitted in one second **/
			static const uint32_t symbolRate = 62500;
		};

                // --- register defaults --------------------------------------
                /*! \brief  Register %specification of the ATmega128RFA1
                 *          radio controller.
                 *
                 *  Defines addresses and default values for the register of
                 *  the radio controller.
                 */
                struct registerDefault {
                    // --- default register addresses -------------------------
                    enum address {
                        snop        = 0x00,
                        /*!< FIXME*/
                        trxStatus   = 0x01,
                        /*!< signals the current state of the radio
                         *   transceiver/CCA measurement (a read access to
                         *   this register clears bits CCA_DONE and CCA_STATUS)*/
                        trxState    = 0x02,
                        /*!< controls the radio transceiver states*/
                        trxCtrl0    = 0x03,
                        /*!< controls the drive current of the digital output
                         *   pads and the CLKM clock rate*/
                        phyTxPwr    = 0x05,
                        /*!< sets the transmit power and controls the FCS
                         *   algorithm for TX aperations*/
                        phyRssi     = 0x06,
                        /*!< indicates the current received signal strength
                         *   (RSSI) and the FCS validity of a received frame*/
                        phyEd       = 0x07,
                        /*!< contains the result after an ED measurement*/
                        phyCccca    = 0x08,
                        /*!< contains bits to initiate and control the CCA
                         *   measurement as well as to set the channel center 
                         *   frequency */
                        ccaThres    = 0x09,
                        /*!< contains the threshold level for CCA-ED
                         * measurements*/
                        irqMask     = 0x0E,
                        /*!< used to enable/disable interrupt events*/
                        irqStatus   = 0x0F,
                        /*!< contains the status of the individual interrupts
                         *   (a read access to this register resets all
                         *   interrupt bits)*/
                        vregCtrl    = 0x10,
                        /*!< controls the use of the voltage regulators and
                         *   indicates their status*/
                        batMon      = 0x11,
                        /*!< configures the battery monitor*/
                        xoscCtrl    = 0x12,
                        /*!< controls the operation of the crystal oscillator*/
                        pllCf       = 0x1A,
                        /*!< controls the operation of the center frequency
                         *   calibration loop */
                        pllDcu      = 0x1B,
                        /*!< controls the operation of the delay cell
                         *   calibration loop */
                        partNum     = 0x1C,
                        /*!< contains the radio transceiver part number*/
                        versionNum  = 0x1D,
                        /*!< contains the radio transceiver version number*/
                        manId0      = 0x1E,
                        /*!< contains 1st part of stored JEDEC manufacturer ID*/
                        manId1      = 0x1F,
                        /*!< contains 2nd part of stored JEDEC manufacturer ID*/
                        shortAddr0  = 0x20,
                        /*!< contains 1st part of 16 bit short address*/
                        shortAddr1  = 0x21,
                        /*!< contains 2nd part of 16 bit short address*/
                        panId0      = 0x22,
                        /*!< contains 1st part of 16 bit PAN ID*/
                        panId1      = 0x23,
                        /*!< contains 2nd part of 16 bit PAN ID*/
                        ieeeAddr0   = 0x24,
                        /*!< contains 1st part of 64 bit IEEE address*/
                        ieeeAddr1   = 0x25,
                        /*!< contains 2nd part of 64 bit IEEE address*/
                        ieeeAddr2   = 0x26,
                        /*!< contains 3rd part of 64 bit IEEE address*/
                        ieeeAddr3   = 0x27,
                        /*!< contains 4th part of 64 bit IEEE address*/
                        ieeeAddr4   = 0x28,
                        /*!< contains 5th part of 64 bit IEEE address*/
                        ieeeAddr5   = 0x29,
                        /*!< contains 6th part of 64 bit IEEE address*/
                        ieeeAddr6   = 0x2A,
                        /*!< contains 7th part of 64 bit IEEE address*/
                        ieeeAddr7   = 0x2B,
                        /*!< contains 8th part of 64 bit IEEE address*/
                        xahCtrl     = 0x2C,
                        /*!< controls the TX_ARET transaction in the Extended
                         *   Operating Mode*/
                        csmaSeed0   = 0x2D,
                        /*!< contains a fraction of the CSMA_SEED value for the
                         *   CSMA-CA algorithm*/
                        csmaSeed1   = 0x2E,
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
                        DEFAULT_VERSIONNUM = 0x02,
                        DEFAULT_MANID0     = 0x1F,
                        DEFAULT_MANID1     = 0x00,
                        DEFAULT_XAHCTRL    = 0x38,
                        DEFAULT_CSMASEED0  = 0xEA,
                        DEFAULT_CSMASEED1  = 0xC2,
                    };
                };
                // --- register maps ------------------------------------------
                /*! \brief  Register map %specification of the ATmega128RFA1
                 *          radio controller.
                 *
                 *  Defines how values are mapped to the structure of the
                 *  registers of the ATmega128RFA1 radio controller.
                 *  \note The type of <code>value</code> depends on the width of
                 *        the registers the default radio controller uses.
                 *        An overview of the available registers and their
                 *        structure can be found in the datasheet of the
                 *        radio controller.
                 */
                union registerMap {
                    struct {
                        uint8_t trx_status          : 5;
                        /*!< current radio transceiver status*/
                        uint8_t reserved            : 1;
                        bool    cca_status          : 1;
                        /*!< indicates the result of a CCA request*/
                        bool    cca_done            : 1;
                        /*!< indicates if a CCA request is completed*/
                    } trxStatus;
                    struct {
                        uint8_t trx_cmd             : 5;
                        /*!< write access initiates a radio transceiver state
                         *   transistion */
                        uint8_t trac_status         : 3;
                        /*!< FIXME*/
                    } trxState;
                    struct {
                        uint8_t clkm_ctrl           : 3;
                        /*!< sets clock rate of pin CLKM*/
                        bool    clkm_sha_sel        : 1;
                        /*!< defines the commencement of the CLKM clock rate
                         *   modifications*/
                        uint8_t pad_io_clkm         : 2;
                        /*!< set the output driver current of pin CLKM*/
                        uint8_t pad_io              : 2;
                        /*!< set the output driver current of digital ouput
                         *   pads MISO and IRQ*/
                    } trxCtrl0;
                    struct {
                        uint8_t tx_pwr              : 4;
                        /*!< FIXME*/
                        uint8_t reserved            : 3;
                        bool    tx_auto_crc_on      : 1;
                        /*!< controls the automatic FCS generation for TX
                         *   operations*/
                    } phyTxPwr;
                    struct {
                        uint8_t rssi                : 5;
                        /*!< contains the result of the automated RSSI
                         * measurement*/
                        uint8_t reserved            : 2;
                        bool    rx_crc_valid        : 1;
                        /*!< indicates whether a received frame has a valid FCS*/
                    } phyRssi;
                    struct {
                        uint8_t channel             : 5;
                        /*!< defines the RX/TX channel according to
                         *   IEEE 802.15.4*/
                        uint8_t cca_mode            : 2;
                        /*!< selected CCA_MODE*/
                        bool    cca_request         : 1;
                        /*!< initiates a manual CCA measurement*/
                    } phyCccca;
                    struct {
                        uint8_t cca_ed_thres        : 4;
                        /*!< defines the threshold value of the CCA-ED
                         *   measurements*/
                        uint8_t reserved            : 4;
                    } ccaThres;
                    struct {
                        bool    pll_lock            : 1;
                        /*!< indicates PLL lock*/
                        bool    pll_unlock          : 1;
                        /*!< indicates PLL unlock*/
                        bool    rx_start            : 1;
                        /*!< indicates a SFD detection (the TRX_STATE changes
                         *   to BUSY_RX)*/
                        bool    trx_end             : 1;
                        /*!< indicates the completion of a frame
                         *   reception/transmission*/
                        uint8_t reserverd           : 2;
                        bool    trx_ur              : 1;
                        /*!< indicates a frame buffer access violation
                         *   (underrun)*/
                        bool    bat_low             : 1;
                        /*!< indicates a supply voltage below the programmed
                         *   threshold*/
                    } irq;
                    struct {
                        uint8_t reserved1           : 2;
                        bool    dvdd_ok             : 1;
                        /*!< indicates if the internal 1.8V regulated supply
                         *   voltage DVDD has settled*/
                        bool    dvreg_ext           : 1;
                        /*!< defines whether the internal digital voltage
                         *   regulator or an external regualator is used*/
                        uint8_t reserved2           : 2;
                        bool    avdd_ok             : 1;
                        /*!< indicates if the internal 1.8 regulated supply
                         *   voltage AVDD has settled*/
                        bool    avreg_ext           : 1;
                        /*!< defines whether the internal analog voltage
                         * regulator or an external regulator is used*/
                    } vregCtrl;
                    struct {
                        uint8_t batmon_vth          : 4;
                        /*!< defines the threshold value for the battery
                         * monitor*/
                        bool    batmon_hr           : 1;
                        /*!< selects the range and resolution of the battery
                         *   monitor*/
                        bool    batmon_ok           : 1;
                        /*!< indicates the level of the external supply woltage
                         *   with respect to the programmed threshold*/
                        uint8_t reserved            : 2;
                    } batMon;
                    struct {
                        uint8_t xtal_trim           : 4;
                        /*!< page 70 atmega128rfa1 manual*/
                        uint8_t xtal_mode           : 4;
                        /*!< sets the operating mode of the crystal oscillator*/
                    } xoscCtrl;
                    struct {
                        uint8_t reserved            : 7;
                        bool    pll_cf_start        : 1;
                        /*!< initiates the center frequency calibration*/
                    } pllCf;
                    struct {
                        uint8_t reserved            : 7;
                        bool    pll_dcu_start       : 1;
                        /*!< initiates the delay cell calibration*/
                    } pllDcu;
                    struct {
                        uint8_t reserved            : 1;
                        uint8_t max_csma_retries    : 3;
                        /*!< specifies the maximum number of retries to repeat
                         *   the random back-off/CCA procedure before a TX_ARET
                         *   transaction is cancelled*/
                        uint8_t max_frame_retries   : 3;
                        /*!< specifies the maximum number of frame
                         *   retransmission in a TX_ARET transaction*/
                    } xahCtrl;
                    struct {
                        uint8_t csma_seed_1         : 3;
                        /*!< contains the upper 3 bit of the CSMA_SEED*/
                        bool    i_am_coord          : 1;
                        /*!< set if node is PAN coordinator*/
                        uint8_t reserved            : 1;
                        bool    aack_set_pd         : 1;
                        /*!< defines the content of the frame pending subfield
                         *   for acknowledgement frames*/
                        uint8_t min_be              : 2;
                        /*!< defines the minimal back-off exponent used in the
                         *   CSMA-CA algorithm */
                    } csmaSeed1;
                    uint8_t value;
                } __attribute__ ((packed));
                // --- default values -----------------------------------------
                /*! \brief  %Specification of default values of the ATmega128RFA1
                 *          radio controller.
                 */
                struct defaultValue {
                    enum {
                        nop                          = 0x00,
                        p_on                         = 0x00,
                        busy_rx                      = 0x01,
                        busy_tx                      = 0x02,
                        tx_start                     = 0x02,
                        force_trx_off                = 0x03,
                        rx_on                        = 0x06,
                        trx_off                      = 0x08,
                        pll_on                       = 0x09,
                        sleep                        = 0x0F,
                        busy_rx_aack                 = 0x11,
                        busy_tx_aret                 = 0x12,
                        rx_aack_on                   = 0x16,
                        tx_aret_on                   = 0x19,
                        rx_on_noclk                  = 0x1c,
                        rx_aack_on_noclk             = 0x1d,
                        busy_rx_aack_noclk           = 0x1e,
                        state_transition_in_progress = 0x1f,
                        success                      = 0x00,
                        success_data_pending         = 0x01,
                        channel_access_failure       = 0x03,
                        no_ack                       = 0x05,
                        invalid                      = 0x07,
                    };
                };
                // --- timings ------------------------------------------------
                /*! \brief  Time %specification of the ATmega128RFA1 radio controller.
                 *
                 *  Defines how long operations or waiting intervalls of this
                 *  radio controller last.
                 */
                struct Duration {
                    enum {
                        trx_reset_time_us      = 6,
                        /*!< duration while reset=low is asserted*/
                        trx_init_time_us       = 510,
                        /*!< duration for transceiver to reach TRX_OFF for the
                         *   first time*/
                        trx_cca_time_us        = 140,
                        /*!< duration of a CCA measurement*/
                        trx_chip_reset_time_us = 625
                        /*!< duration for transceiver reset*/
                    };
                };
                // --- sizes chip specific ------------------------------------
                 /*! \brief  %Specification of available memory of the ATmega128RFA1
                 *          radio controller.
                 *
                 *  Defines sizes and numbers of available memory and queues.
                 */
                enum chipSize {
                    size_fifo    = 128,
                    size_ram     = 512
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
