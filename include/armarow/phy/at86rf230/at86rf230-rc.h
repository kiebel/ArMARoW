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
#ifndef __ARMAROW_AT86RF230_h__
#define __ARMAROW_AT86RF230_h__
#define ARMAROW_DEBUG_DISABLE

#include "armarow/phy/at86rf230/at86rf230-spec.h"
#include "armarow/debug.h"
#include "avr-halib/share/delay.h"
#include "avr-halib/share/delegate.h"

namespace armarow {
    namespace phy {
        /*! \brief  Default configuration of the AT86RF230 radio controller.
         *  \ingroup RcConf
         */
        struct At86Rf230CFG {
            enum {
                enabledCCA           = false,
                /*!< indicates whether a CCA is performed before transmitting*/
                enabledCRC           = true,
                /*!< indicates whether CRC checking of frames is enabled*/
                enabledAutoAck       = false,
                /*!< indicates whether the automatic acknowledgement is enabled*/
                enabledAddressDecode = false,
                /*!< indicates whether the automatic address decode is enabled*/
                enableConcurrence    = false,
                /*!< indicates whether interrupts can be used concurrently*/
                rxOnIdle             = true
                /*!< indicates whether the TX mode is turned off after a transmit*/
            };
        };
        /*! \class  At86Rf230 at86rf230-rc.h "armarow/phy/at86rf230/at86rf230-rc.h"
         *  \brief  Class %At86Rf230 represents the interface for the AT86RF230
         *          radio controller being part of ArMARoW's physical layer.
         *
         *  \tparam HW  hardware configuration of the radio controller
         *              (<em>usually given by a portmap</em>)
         *  \tparam CFG configuration parameters of the radio controller
         *              (\link At86Rf230CFG\endlink)
         *  \ingroup PhyL RcImpl
         */
        template < class HW, class CFG = struct At86Rf230CFG >
        class At86Rf230 {
            public:
                /*! \brief  definition of the class type*/
                typedef At86Rf230< HW, CFG > type;
                /*! \brief  definition of the radio controller %specification*/
                typedef armarow::phy::specification::At86Rf230 spec_t;
                /*! \brief  definition of layer specific information*/
                typedef struct {
                    enum {
                        frame    = armarow::PHY::aMaxPHYPacketSize,
                        /*!< size of a frame (note that this parameter
                         *  coresponses to the <code>aMaxPHYPacketSize</code>
                         *  of the IEEE 802.15.4)
                         */
                        overhead = ((CFG::enabledCRC) ? 0x02 : 0x00),
                        /*!< overhead caused by the radio controller
                         *   configuration (CRC, aso.)
                         */
                        payload  = frame - overhead
                        /*!< available payload considering frame size and overhead */
                    };
                } info;
                /*! \brief  definition of a layer specific message object*/
                typedef struct {
                    uint8_t size;                   /*!< number of bytes*/
                    uint8_t payload[info::frame];   /*!< frame data     */
                } mob_t;
                /*! \brief  definition of layer specific message information*/
                typedef struct {
                    uint8_t rxLqi;  /*!< last measured LQI value*/
                } minfo_t;
          private:
                /*! \brief  Interface between the microcontroller and
                 *          the radio controller.
                 *  \tparam BC  class the interface is based on (e.g. SPI)
                 */
                template< class BC >
                class ControllerInterface : public BC {
                    public:
                        /*! \brief  Data Type for transceiver SRAM address*/
                        typedef uint8_t ramaddr_t;
                        /*! \brief  Data Type for transceiver register address*/
                        typedef spec_t::registerDefault::address regaddr_t;
                        /*! \brief  Data Type for transceiver register value*/
                        typedef spec_t::registerMap regval_t;
                        /*! \brief  data type for interface status*/
                        typedef spec_t::SPI::status_t status_t;
                    private:
                        /*! \brief  internal status of the interface*/
                        status_t status;

                        void read(regaddr_t pAddress, void* pValue)  {}
                        void write(regaddr_t pAddress, void* pValue) {}
                    public:
                        /*! \brief  (Re)Initializes the interface.*/
                        void init() {
                            status.value = 0;   // resets internal status
                        }

                        /*! \brief  Invokes a command of the radio controller.
                         *  \param[in] pCommand command to be executed
                         *  \return Returns the interface state.
                         */
                        status_t invoke(regaddr_t pCommand = spec_t::registerDefault::SNOP) {
                            return status;
                        }

                        /*! \brief  Read from a radio controller register.
                         *  \param[in]  pAddress address of the source register
                         *  \param[out] pValue variable to write result to
                         */
                        void readRegister(regaddr_t pAddress, regval_t &pValue) {
                            read(pAddress, &(pValue.value));
                        }
                        /*! \brief  Writes to a radio controller register.
                         *  \param[in] pAddress address of the destination register
                         *  \param[in] pValue new value of the register
                         */
                        void writeRegister(regaddr_t pAddress, regval_t pValue) {
                            write(pAddress, &(pValue.value));
                        }

                        /*! \brief  Read data from the RXFIFO.
                         *  \param[in]  pSize available size of the buffer
                         *  \param[out] pData pointer to the data buffer
                         *  \param[out] pLqi  pointer to the lqi value
                         *  \return Returns the number of bytes read from
                         *          the RXFIFO (including the LQI byte)
                         */
                        uint8_t readRxFifo(uint8_t pSize, uint8_t *pData, uint8_t *pLqi) {
                            if ( this->frame.size > pSize ) return 0;
                            *pLqi = 255;
                            for (uint8_t index = 0; index < this->frame.size; index++) {
                                pData[index] = this->frame.payload[index];
                            }
                            return this->frame.size;
                        }
                        /*! \brief  Read data from RXFIFO and do a CRC16 check
                         *          (CRC value is suppressed).
                         *
                         *  While transfering the frame into the provided buffer
                         *  the CRC value is calculated. This value is then
                         *  compared against the last two byte which are not
                         *  copied into the buffer.
                         *
                         *  \param[in]  pSize available size of the buffer
                         *  \param[out] pData pointer to the data buffer
                         *  \param[out] pLqi  pointer to the variable storing the LQI
                         *  \param[out] pCrc  result of the CRC check
                         *  \return Returns the number of bytes read from the
                         *          RXFIFO (including the LQI byte)
                         */
                        uint8_t readRxFifoCrc(uint8_t pSize, uint8_t *pData, uint8_t *pLqi, bool &pCrc) {
                            //TODO read from RXFIFO with CRC
                            return 0;
                        }
                        /*! \brief  Writes data into the TXFIFO.
                         *  \param[in] pSize size of the data buffer
                         *  \param[in] pData pointer to the buffer
                         */
                        void writeTxFifo(uint8_t pSize, const uint8_t *pData) {
                            ::logging::log::emit() << "\t[[" << (uint16_t)pSize << "] ['";
                            this->frame.size = pSize;
                            for (uint8_t index = 0; index < pSize; index++) {
                                this->frame.payload[index] = pData[index];
                                ::logging::log::emit() << (char)pData[index];
                            }
                            ::logging::log::emit() << "']]" << ::logging::log::endl;
                        }
                        /*! \brief  Writes data into the TXFIFO and calculate
                         *          CRC (if not supported by the radio
                         *          controller).
                         *  \param[in] pSize size of the data buffer
                         *  \param[in] pData pointer to the buffer
                         */
                        void writeTxFifoCRC(uint8_t pSize, const uint8_t *pData) {
                            //TODO write to TXFIFO with CRC
                        }

                        /*! \brief  Read from the SRAM.
                         *  \param[in]  pAddress address in the SRAM
                         *  \param[in]  pSize number of bytes to be read
                         *  \param[out] pData pointer to the buffer
                         */
                        void readSRAM(ramaddr_t pAddress, uint8_t pSize, uint8_t *pData) {
                        }
                        /*! \brief  Write to the SRAM.
                         *  \param[in] pAddress address in the SRAM
                         *  \param[in] pSize number of bytes to write
                         *  \param[in] pData pointer to the buffer
                         */
                        void writeSRAM(ramaddr_t pAddress, uint8_t pSize, uint8_t *pData) {
                        }
                };
                typedef struct {
                    uint8_t channel;           // current channel
                    uint8_t transmitPower;     // current transmission power
                    uint8_t ccaMode;           // current CCA mode
                    mob_t   frame;             // current frame (send/recv)
                    armarow::PHY::State state; // current state of transceiver
                } chip_t; //TODO remove this typedefinition
                /*! \brief  definition of interface between %MC and %RC.*/
                typedef ControllerInterface< chip_t > ControllerInterface_t;

                ControllerInterface_t  rc;
                minfo_t minfo;

                /*! \brief  Determins the cause of an interrupt and calls the
                 *          appropriated interrupt handler.
                 *  \note   Method is triggered by an external interrupt.
                 *  \sa     \link handleIRQ()\endlink
                 */
                void onIRQ() {
                    typename ControllerInterface_t::regval_t registerValue;

                    if ( CFG::enableConcurrence ) {
                        //TODO disable RadioControllerIRQ
                        sei();
                    }
                    registerValue.value = 1;
                    handleIRQ( registerValue );
                    if ( CFG::enableConcurrence ) {
                        cli();
                        //TODO enable RadioControllerIRQ
                    }
                }

                /*! \brief  Handles any interrupt caused by the
                 *          radio controller.
                 *  \note   Since no interrupts from the radio controller
                 *          are accepted while this method is running, keep
                 *          the implementation of the callback functions
                 *          short and efficient.
                 *  \sa     \link onIRQ()\endlink
                 *  \sa     \link interruptRX()\endlink
                 *  \sa     \link receive(mob_t&)\endlink
                 *  \param[in] pCause interrupt status
                 */
                void handlerIRQ( typename ControllerInterface_t::regval_t pCause) {
                    //TODO implement all cases that trigger an interrupt
                    minfo.rxLqi  = 15;
                    if ( pCause == 1 ) onReceive();
                }

                /*! \brief  Checks the radio controller for pending operations.
                 *  \returns  Returns TRUE if radio controller finished
                 *            transmitting or receiving otherwise FALSE.
                 */
                bool ready() {
                    switch ( getStateTRX() ) {
                        case armarow::PHY::BUSY_TX:
                        case armarow::PHY::TX_ON:
                            setStateTRX( ((CFG::rxOnIdle) ? armarow::PHY::RX_ON : armarow::PHY::TRX_OFF) );
                        case armarow::PHY::RX_ON:
                            return true;
                        default:
                            return false;
                    }
                }
            public:
                /*! \brief  Interrupt service routine that is triggered
                 *          if data for the next layer is available.
                 *  \note   A call to this method is similar to an invocation
                 *          of the <code>PD-DATA.indication-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 */
                Delegate<> onReceive;

                /*! \brief Constructor of the AT86RF230 radio controller.
                 *
                 *  Initializes and resets the radio controller hardware using
                 *  \link reset()\endlink, as well as maps interrupts of the
                 *  radio controller to <code>onIRQ()</code>.
                 */
                At86Rf230() {
                    // configure radio controller hardware
                    //TODO UseRegmap(rm, HW);
                    //TODO hardware initialization
                    //TODO SyncRegmap(rm);
                    delay_us( spec_t::Duration::TRX_CHIP_RESET_TIME_US );
                    // map interrupt to method onIRQ()
                    //TODO HW::irq_t::template init<type,&type::onIRQ>(this);
                    //------------------------------------------------------
                    rc.channel       = 0;
                    rc.transmitPower = 0;
                    rc.ccaMode       = 0;
                    rc.frame.size    = 0;
                    //------------------------------------------------------
                    rc.state = armarow::PHY::TRX_OFF;
                }
                ~At86Rf230() {}

                /*! \brief  (Re)Initializes the AT86RF230 radio controller by
                 *          initializing the hardware used.
                 */
                void init() {
                    minfo.rxLqi  = 0;
                    // transceiver initialization
                    // disable IRQ and clear any pending IRQs
                    // check current state
                    rc.state = armarow::PHY::TRX_OFF;
                }

                /*! \brief  Resets the AT86RF230 radio controller.
                 *  \note   After reset was called the radio controller is
                 *          in state <code>TRX_OFF</code>.
                 */
                void reset() {
                    //TODO do a chip reset
                    rc.state = armarow::PHY::TRX_OFF;
                }

                /*! \brief  Transmits provided data via the radio connection.
                 *  \note   A call to this method is similar to an invocation
                 *          of the <code>PD-DATA.request-primitive</code> as
                 *          defined in IEEE 802.15.4.
                 *  \sa     \link send_blocking \endlink
                 *  \param[in] pData layer specific message object
                 *  \returns  Returns the status of the operation
                 *            (<em>note that the status is similar to an
                 *            invocation of the
                 *            <code>PD-DATA.confirm-primitive</code> as
                 *            defined in IEEE 802.15.4</em>)
                 */
                armarow::PHY::State send(mob_t& pData) {
                    TRACE_FUNCTION;
                    // check if TX is enabled and no transmission is taking place
                    armarow::PHY::State cState = getStateTRX();
                    if ( cState != armarow::PHY::TX_ON ) return cState;
                    // do nothing if size equals zero
                    if ( pData.size == 0) {
                        setStateTRX( ((CFG::rxOnIdle) ? armarow::PHY::RX_ON : armarow::PHY::TRX_OFF) );
                        return armarow::PHY::SUCCESS;
                    }

                    // alter size if CRC is enabled
                    uint8_t size = pData.size + info::overhead;
                    // check if size is within allowed range
                    if ( size > armarow::PHY::aMaxPHYPacketSize )
                        return armarow::PHY::INVALID_PARAMETER;

                    rc.writeTxFifo(size, pData.payload);

                    // if CCA is enabled wait for idle medium
                    if (CFG::enabledCCA) {
                        while ( doCCA() != armarow::PHY::IDLE);
                    }

                    //TODO send data
                    rc.state = armarow::PHY::BUSY_TX;

                    return armarow::PHY::SUCCESS;
                }

                /*! \brief  Transmits provided data via the radio connection and
                 *          waits until transmission has finished.
                 *  \note   Method blocks until transmission finished or failed.
                 *  \note   A call to this method is similar to an invocation
                 *          of the <code>PD-DATA.request-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 *  \sa     \link send() \endlink
                 *  \param[in] pData layer specific message object
                 *  \returns  Returns the status of the operation
                 *            (<em>note that the status is similar to an
                 *            invocation of the
                 *            <code>PD-DATA.confirm-primitive</code> as
                 *            defined in IEEE 802.15.4</em>)
                 */
                armarow::PHY::State send_blocking(mob_t& pData) {
                    TRACE_FUNCTION;
                    armarow::PHY::State result = send(pData);
                    while( ( result == armarow::PHY::SUCCESS ) && !ready() );
                    return result;
                }

                /*! \brief  Gets the last received message and fills it into the
                 *          provided data structure.
                 *  \sa     \link receive_blocking()\endlink
                 *  \param[in] pData layer specific message object
                 *  \returns  Returns the size of the message received.
                 */
                uint8_t receive(mob_t& pData) {
                    TRACE_FUNCTION;
                    // read data from RXFIXO and set lqi value
                    pData.size  = rc.readRxFifo(info::frame, pData.payload, &(minfo.rxLqi));
                    pData.size -= info::overhead;
                    //TODO get information for layerStatus
                    return pData.size;
                }

                /*! \brief  Blocks until a message was received and fill this
                 *          message into the provided data structure.
                 *  \sa     \link receive()\endlink
                 *  \note   Method blocks until transmission finished or failed.
                 *  \param[in] pData layer specific message object
                 *  \return Returns the size of the message received.
                 */
                uint8_t receive_blocking(mob_t& pData) {
                    TRACE_FUNCTION;
                    while( !ready() );
                    return receive(pData);
                }

                /*! \brief  Checks if the medium is available by performing
                 *          a CCA (<em>Clear Channel Assesment</em>).
                 *  \sa     \link armarow::phy::RadioControllerCFG::enabledCCA\endlink
                 *  \note   A call to this method is similar to an invocation
                 *          of the <code>PLME-CCA.request-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 *
                 *  \returns  The result of the CCA which is similar to an
                 *            invocation of the
                 *            <code>PLME-CCA.confirm-primitive</code>
                 *            as defined in IEEE 802.15.4 (possible values
                 *            are <code>TRX_OFF</code>,
                 *            <code>BUSY, IDLE</code>)
                 */
                armarow::PHY::State doCCA() {
                    TRACE_FUNCTION;
                    // a CCA check is possible only if we are in RX state
                    if ( getStateTRX() == armarow::PHY::TRX_OFF )
                        return armarow::PHY::TRX_OFF;
                    if ( getStateTRX() != armarow::PHY::RX_ON )
                        return armarow::PHY::BUSY;
                    //TODO do actual check
                    return armarow::PHY::IDLE;
                }

                /*! \brief  Performs an energy detection on the medium without
                 *          trying to decode any data.
                 *  \note   A call to this method is similar to an invocation
                 *          of the <code>PLME-ED.request-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 *
                 *  \param[out] pEnergyLevel detected level of energy
                 *  \returns  The status of the energy detection which is
                 *            similar to an invocation of the
                 *            <code>PLME-ED.confirm-primitive</code> as
                 *            defined in IEEE 802.15.4 (possible values are
                 *            <code>SUCCESS</code>, <code>TRX_OFF</code>,
                 *            <code>TX_ON</code>)
                 */
                armarow::PHY::State doED(uint8_t& pEnergyLevel) {
                    TRACE_FUNCTION;
                    if ( (getStateTRX() == armarow::PHY::RX_ON) ||
                            (getStateTRX() == armarow::PHY::BUSY_RX) ) {
                        pEnergyLevel = spec_t::dBmToPaLevel(0);
                        return armarow::PHY::SUCCESS;
                    }
                    return getStateTRX();
                }

                /*! \brief  Requests the value of a PAN Information Attribute.
                 *  \sa     \link setAttribute()\endlink
                 *  \note   A call to this method is similar to an invocation
                 *          of the <code>PLME-GET.request-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 *
                 *  \param[in]  pAttribute specifies attribute requested
                 *              (for possible values see
                 *              \link PHY::PIBAttribute\endlink)
                 *  \param[out] pAttrValue pointer to the value of the
                 *              attribute (note that the caller has to
                 *              provide an appropriated buffer for the
                 *              attribute)
                 *  \returns  The status of the operation similar to an
                 *            invocation of the
                 *            <code>PLME-GET.confirm-primitive</code> as
                 *            defined in IEEE 802.15.4 (possible values are
                 *            <code>SUCCESS</code>,
                 *            <code>UNSUPPORTED_ATTRIBUTE</code>)
                 *  \todo add phyMaxFrameDuration, phySHRDuration, phySymbolsPerOctet
                 */
                armarow::PHY::State getAttribute(armarow::PHY::PIBAttribute pAttribute, void* pAttrValue) {
                    switch (pAttribute) {
                        case armarow::PHY::phyCurrentChannel:
                            *((uint8_t*)pAttrValue) = rc.channel;           //TODO
                            break;
                        case armarow::PHY::phyChannelsSupported:
                            *((uint32_t*)pAttrValue) = spec_t::Channel::supportedChannel();
                            break;
                        case armarow::PHY::phyTransmitPower:
                            *((uint8_t*)pAttrValue) = rc.transmitPower;     //TODO
                            break;
                        case armarow::PHY::phyCCAMode:
                            *((uint8_t*)pAttrValue) = rc.ccaMode;           //TODO
                            break;
                        case armarow::PHY::phyCurrentPage:
                            // note that only page 0x00 is curently supported
                            *((uint8_t*)pAttrValue) = 0x00;
                            break;
                        case armarow::PHY::phyMaxFrameDuration:             //FIXME
                        case armarow::PHY::phySHRDuration:                  //FIXME
                        case armarow::PHY::phySymbolsPerOctet:              //FIXME
                            break;
                        default:
                            return armarow::PHY::UNSUPPORTED_ATTRIBUTE;
                    }
                    return armarow::PHY::SUCCESS;
                }

                /*! \brief  Sets the value of a PAN Information Attribute.
                 *  \sa     \link getAttribute()\endlink
                 *  \note   A call to this method is similar to an invocation
                 *          of the <code>PLME-SET.request-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 *
                 *  \param[in]  pAttribute defines attribute to be changed
                 *              (for possible values see
                 *              \link PHY::PIBAttribute\endlink)
                 *  \param[in]  pAttrValue value of the attribute
                 *
                 *  \returns  The status of the operation similar to an
                 *            invocation of the
                 *            <code>PLME-SET.confirm-primitive</code> as
                 *            defined in IEEE 802.15.4 (possible values are
                 *            <code>SUCCESS</code>,
                 *            <code>UNSUPPORTED_ATTRIBUTE</code>,
                 *            <code>INVALID_PARAMETER</code>,
                 *            <code>READ_ONLY</code>)
                 */
                armarow::PHY::State setAttribute(armarow::PHY::PIBAttribute pAttribute, void* pAttrValue) {
                    switch (pAttribute) {
                        case armarow::PHY::phyCurrentChannel:
                            if ( ( *((uint8_t*)pAttrValue) >= spec_t::Channel::minChannel) &&
                                    ( *((uint8_t*)pAttrValue) <= spec_t::Channel::maxChannel) ) {
                                rc.channel = *((uint8_t*)pAttrValue);       //TODO
                            } else {
                                return armarow::PHY::INVALID_PARAMETER;
                            }
                            break;
                        case armarow::PHY::phyTransmitPower:
                            if ( *((uint8_t*)pAttrValue) <= 15 ) {
                                rc.transmitPower = *((uint8_t*)pAttrValue); //TODO
                            } else {
                                return armarow::PHY::INVALID_PARAMETER;
                            }
                            break;
                        case armarow::PHY::phyCCAMode:
                            if ( *((uint8_t*)pAttrValue) <= 3 ) {
                                rc.ccaMode = *((uint8_t*)pAttrValue);       //TODO
                            } else {
                                return armarow::PHY::INVALID_PARAMETER;
                            }
                            break;
                        case armarow::PHY::phyCurrentPage:
                            if ( *((uint8_t*)pAttrValue) != 0x00)
                                return armarow::PHY::INVALID_PARAMETER;
                            break;
                        case armarow::PHY::phyChannelsSupported:
                        case armarow::PHY::phyMaxFrameDuration:
                        case armarow::PHY::phySHRDuration:
                        case armarow::PHY::phySymbolsPerOctet:
                            return armarow::PHY::READ_ONLY;
                        default:
                            return armarow::PHY::UNSUPPORTED_ATTRIBUTE;
                    }
                    return  armarow::PHY::SUCCESS;
                }

                /*! \brief  Gets the state (<em>working mode</em>) of the
                 *          transceiver.
                 *  \sa     \link setStateTRX() \endlink
                 *  \note   A call to this method is similar to an invocation
                 *          of the
                 *          <code>PLME-GET-TRX-STATE.request-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 *  \return Returns current state of the radio controller
                 *          (available modes are <code>RX_ON</code>,
                 *          <code>TRX_OFF</code>, <code>BUSY_RX</code>,
                 *          <code>BUSY_TX</code>, <code>TX_ON</code>).
                 */
                armarow::PHY::State getStateTRX() {
                    return rc.state;
                }

                /*! \brief  Sets the state (<em>working mode</em>) of the
                 *          transceiver.
                 *  \sa     \link getStateTRX() \endlink
                 *  \note   A call to this method is similar to an invocation
                 *          of the
                 *          <code>PLME-SET-TRX-STATE.request-primitive</code>
                 *          as defined in IEEE 802.15.4.
                 *  \param[in]  pState new working mode (available modes are
                 *              <code>RX_ON,</code> <code>TRX_OFF</code>,
                 *              <code>FORCE_TRX_OFF</code>,
                 *              <code>TX_ON</code>)
                 */
                void setStateTRX(const armarow::PHY::State pState) {
                    switch (pState) {
                        case armarow::PHY::RX_ON:
                            rc.state = armarow::PHY::RX_ON;
                            break;
                        case armarow::PHY::TRX_OFF:
                            rc.state = armarow::PHY::TRX_OFF;
                            break;
                        case armarow::PHY::TX_ON:
                            rc.state = armarow::PHY::TX_ON;
                            break;
                        case armarow::PHY::FORCE_TRX_OFF:
                            reset();
                            break;
                        default:
                            break;
                    }
                }
        };
    }; // end namespace phy
}; // end namespace armarow

#endif  //__ARMAROW_AT86RF230_h__
