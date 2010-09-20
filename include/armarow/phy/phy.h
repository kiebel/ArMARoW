/*******************************************************************************
 *
 * Copyright (c) 2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 *               2010 Marcus Foerster <marcus.foerster@student.ovgu.de>
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
#ifndef __ARMAROW_LAYER_PHY_h__
#define __ARMAROW_LAYER_PHY_h__

/*! \defgroup PhyL Physical Layer
 *
 *  The physical layer (%PHY) ultimately provides the data transmission service,
 *  as well as the interface to the physical layer management entity, which
 *  offers access to every layer management function and maintains a database
 *  of information on related personal area networks.
 *  Thus, %PHY manages the physical RF transceiver and performs channel
 *  selection as well as energy and signal management functions.
 *
 *  \ingroup ArMARoW
 */
/*! \defgroup RcImpl Radio Controller Implementation
 *  \brief  Implementations of different radio controller chips
 *  \todo documentation of Group 'Radio Controller Implementation'
 *  \sa     \link PhyL\endlink
 *
 *  Based on this class the physical layer for any given hardware
 *  (<em>radio controller</em>) can be implemented.
 *  Since ArMARoW is template based this class should no be extended
 *  but rather copied.
 */
/*! \defgroup RcConf Radio Controller Configuration
 *  \ingroup RcImpl
 *  \todo documentation of Group 'Radio Controller Configuration'
 */
/*! \defgroup RcSpec Radio Controller Specification
 *  \ingroup RcImpl
 *  \todo documentation Group Radio Controller Specification
 */
/*! \brief Physical Layer (%PHY)
 *
 *  This namespace represents the lowest layer of the OSI reference modell.
 *  It contains layer specific information, interfaces and data as well as the
 *  actual chip implementations.
 *
 *  \ingroup PhyL
 *  \namespace armarow::phy
 */
namespace armarow {
    /*! \brief  Definition of physical layer specific informations
     *          (<em>constants, PIB attribute identifiers and states</em>).
     *
     *  These informations include layer specific constants, states that are
     *  used for communication between layers and identifiers to access the
     *  PAN Information Base Attributes.
     *
     *  \note   The definitions are consistent with the IEEE 802.15.4 standard.
     *  \ingroup PhyL
     */
    struct PHY {
        /*! \brief  Identifiers to access the PAN Information Base Attributes
         *          requiered to manage the physical layer as defined in
         *          IEEE 802.15.4.
         */
        enum PIBAttribute {
            phyCurrentChannel       = 0x00,
            /*!< RF channel used for transmissions*/
            phyChannelsSupported    = 0x01,
            /*!< bitmap of supported channels per channel page*/
            phyTransmitPower        = 0x02,
            /*!< tolerance on, and nominal transmit power*/
            phyCCAMode              = 0x03,
            /*!< mode used for Clear Channel Assesment*/
            phyCurrentPage          = 0x04,
            /*!< channel page to use for transmission (<em>used in conjunction
             *   with <code>phyCurrentChannel</code></em>)*/
            phyMaxFrameDuration     = 0x05,
            /*!< maximal numer of symbols in a frame <code>:= phySHRDuration +
             *   ceiling([aMaxPHYPacketSize + 1] x phySumbolsPerOctet)</code>
             */
            phySHRDuration          = 0x06,
            /*!< duration of the synchronization header (SHR) in symbols
             *   (<em>for the current layer</em>)
             */
            phySymbolsPerOctet      = 0x07
            /*!< number of symbols per octet for the current layer*/
        };
        /*! \brief  Constants specific to the physical layer as defined in
         *          IEEE 802.15.4.
         */
        enum Constant {
            aMaxPHYPacketSize       = 127,
            /*!< maximum number bytes contained in a PHY-layer packet*/
            aTurnaroundTime         = 12
            /*!< number of symbol periods the transceiver needs to switch
             *   between RX and TX states*/
        };
        /*! \brief  States specific to the physical layer as defined in
         *          IEEE 802.15.4.
         */
        enum State {
            BUSY                    = 0x00,
            /*!< radio controller is busy*/
            BUSY_RX                 = 0x01,
            /*!< radio controller is currently busy receiving data*/
            BUSY_TX                 = 0x02,
            /*!< radio controller is currently busy transmitting data*/
            FORCE_TRX_OFF           = 0x03,
            /*!< state change to TRX_OFF is forced*/
            IDLE                    = 0x04,
            /*!< channel is currently idle*/
            INVALID_PARAMETER       = 0x05,
            /*!< provided parameter is invalid or not supported*/
            RX_ON                   = 0x06,
            /*!< radio controller is waiting to receive data*/
            SUCCESS                 = 0x07,
            /*!< operation was successfully performed*/
            TRX_OFF                 = 0x08,
            /*!< radio controller clock state oscillator is enable*/
            TX_ON                   = 0x09,
            /*!< radio controller is waiting to transmit data*/
            UNSUPPORTED_ATTRIBUTE   = 0x0A,
            /*!< specified attribute is not supported or defined*/
            READ_ONLY               = 0x0B
            /*!< specified attribute is read-only*/
        };
        /*! \brief Modes for Clear Channel Assesment (%CCA) */
        enum ModeCCA {
            threshold               = 1,
            /*!< Mode 1, Energy is above a given threshold */
            carrierSense            = 2,
            /*!< Mode 2, Carrier sense only */
            carrierSenseThreshold   = 3
            /*!< Mode 3, Carrier sense and energy is above a given threshold */
        };
    };
}; // namespace armarow

#endif  //__ARMAROW_LAYER_PHY_h__
