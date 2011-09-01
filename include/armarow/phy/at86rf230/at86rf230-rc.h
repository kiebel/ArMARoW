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

#include "armarow/common/crc.h"
#include "armarow/phy/at86rf230/at86rf230-spec.h"
#include "armarow/phy/at86rf230/at86rf230-controllerInterface.h"
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
                rxOnIdle             = true,
                /*!< indicates whether the TX mode is turned off after transmits*/
				autoCRC				 = true
				/*!< inicated whether autmatic crc generation for send frames will be used*/
            };

			typedef common::CRC::Polynomials::ITU_T CRCPolynomial;
        };
        /*! \class  At86Rf230 at86rf230-rc.h "armarow/phy/at86rf230/at86rf230-rc.h"
         *  \brief  Class %At86Rf230 represents the interface for the AT86RF230
         *          radio controller being part of ArMARoW's physical layer.
         *
         *  \tparam Hal  hardware configuration of the radio controller
         *              (<em>usually given by a portmap</em>)
         *  \tparam CFG configuration parameters of the radio controller
         *              (\link At86Rf230CFG\endlink)
         *  \ingroup PhyL RcImpl
         */
        template < class Hal, class CFG = struct At86Rf230CFG >
        class At86Rf230 {
            public:
				template<class NewCFG>
				struct reconfigure{typedef At86Rf230<Hal, NewCFG> type;};
                /*! \brief  definition of the class type*/
                typedef At86Rf230< Hal, CFG > type;
                /*! \brief  definition of the radio controller %specification*/
                typedef armarow::phy::specification::At86Rf230 spec_t;
                /*! \brief  definition of layer specific information*/
                struct info{
                    enum {
                        frame    = armarow::PHY::aMaxPHYPacketSize,
                        /*!< size of a frame (note that this parameter
                         *  coresponses to the <code>aMaxPHYPacketSize</code>
                         *  of the IEEE 802.15.4)
                         */
                        overhead = ((CFG::enabledCRC) ? 0x02 : 0x00), //FIXME check if enabledCRC is used correctly
                        /*!< overhead caused by the radio controller
                         *   configuration (CRC, aso.)
                         */
                        payload  = frame - overhead
                        /*!< available payload considering frame size and overhead */
                    };
                };
				
                /*! \brief  definition of a layer specific message object*/
                struct mob_t{
                    uint8_t size;                   /*!< number of bytes*/
                    uint8_t payload[info::frame];   /*!< frame data     */
					struct {
                    	uint8_t lqi;  /*!< last measured LQI value*/
						uint8_t ed;
						bool    crc  : 1;
                	} minfo;
                };
                /*! \brief  definition of layer specific message information
                 *  \todo add information such as RSSI, CRC validity aso.
                 */
                
          private:

                /*! \brief  definition of interface between %MC and %RC.*/
                typedef ControllerInterface< typename Hal::spi_t, typename Hal::portmap_t, spec_t > ControllerInterface_t;

                ControllerInterface_t  rc;

                /*! \brief  Determins the cause of an interrupt and calls the
                 *          appropriated interrupt handler.
                 *  \note   Method is triggered by an external interrupt.
                 *  \sa     \link handleIRQ()\endlink
                 */
                void onIRQ() {
                    typename ControllerInterface_t::regval_t registerValue;

                    if ( CFG::enableConcurrence ) {
                        Hal::irq_t::disable();
                        sei();
                    }
                    rc.readRegister( spec_t::registerDefault::IRQSTATUS, registerValue);
                    handlerIRQ( registerValue );
                    if ( CFG::enableConcurrence ) {
                        cli();
                        Hal::irq_t::enable();
                    }
                }

                /*! \brief  Handles any interrupt caused by the
                 *          radio controller.
                 *  \note   Since no interrupts from the radio controller
                 *          are accepted while this method is running, keep
                 *          the implementation of the callback functions
                 *          short and efficient.
                 *  \sa     \link onIRQ()\endlink
                 *  \sa     \link onReceive()\endlink
                 *  \sa     \link receive(mob_t&)\endlink
                 *  \param[in] pCause interrupt status
                 */
                void handlerIRQ( typename ControllerInterface_t::regval_t pCause) {
                    if ( pCause.IRQ.BAT_LOW  ) {} //TODO power supply low
                    if ( pCause.IRQ.TRX_UR   ) {} //TODO buffer underrun
                    if ( pCause.IRQ.RX_START ) {
                        //rc.readRegister(  spec_t::registerDefault::PHYRSSI, registerValue );
                        //FIXME minfo.rxRssi = registerValue.PHYRSSI.RSSI;
                    }
                    if ( pCause.IRQ.TRX_END ) {
                        armarow::PHY::State cState = (armarow::PHY::State)this->getStateTRX();
                        switch(cState)
						{
							case(armarow::PHY::RX_ON ): this->onReceive();
														break;

                            case(armarow::PHY::TX_ON ): if(CFG::rxOnIdle)
															this->setStateTRX( armarow::PHY::RX_ON );
														break;
							default:					break;
                        }
					}
                }

                /*! \brief  Checks the radio controller for pending operations.
                 *  \returns  Returns TRUE if radio controller finished
                 *            transmitting or receiving otherwise FALSE.
                 *  \todo rework the way an blocking send/recv works (use of ready??)
                 */
                bool ready() {
                    switch ( getStateTRX() ) {
                        case armarow::PHY::BUSY_RX:
                        case armarow::PHY::BUSY_TX:
                            return false;
                        case armarow::PHY::TX_ON:
                            if ( CFG::rxOnIdle ) setStateTRX( armarow::PHY::RX_ON );
                        case armarow::PHY::RX_ON:
                            //FIXME do something here ....!!! what happens after succesfull recv?
                        case armarow::PHY::TRX_OFF:
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
                    UseRegmap(rm, Portmap);
                    rm.reset.ddr  = true;   // reset pin is output
                    rm.reset.port = false;  // pin set to LOW
                    rm.sleep.ddr  = true;   // sleep pin is output
                    rm.sleep.port = false;  // pin set to LOW
                    SyncRegmap(rm);
                    delay_us( spec_t::Duration::TRX_CHIP_RESET_TIME_US );
                    // map interrupt to method onIRQ()
                    Hal::irq_t::template init<type,&type::onIRQ>(this);
                }
                ~At86Rf230() {}

                /*! \brief  (Re)Initializes the AT86RF230 radio controller by
                 *          initializing the hardware used.
                 */
                void init() {
                    typename ControllerInterface_t::regval_t registerValue;

                    // transceiver initialization
                    UseRegmap(rm, Portmap);
                    rc.readRegister(spec_t::registerDefault::TRXSTATUS, registerValue);
                    if ( registerValue.TRXSTATUS.TRX_STATUS != spec_t::defaultValue::TRX_OFF ) {
                        if ( registerValue.TRXSTATUS.TRX_STATUS !=  spec_t::defaultValue::P_ON ) {
                            reset();                    // reset chip and set TRX_OFF
                        }
                        rm.sleep.port = false;          // sleep pin to LOW
                        rm.reset.port = true;           // reset pin to HIGH
                        SyncRegmap(rm);
                    }

                    // disable IRQ and clear any pending IRQs
                    registerValue.value = 0x00;
                    rc.writeRegister( spec_t::registerDefault::IRQMASK,   registerValue );
                    rc.readRegister(  spec_t::registerDefault::IRQSTATUS, registerValue );
                    rc.readRegister(  spec_t::registerDefault::TRXSTATE,  registerValue );
                    registerValue.TRXSTATE.TRX_CMD = spec_t::defaultValue::TRX_OFF;
                    rc.writeRegister( spec_t::registerDefault::TRXSTATE,  registerValue );
                    delay_us( spec_t::Duration::TRX_INIT_TIME_US );

                    // check current state and stop if state is not TRX_OFF
                    // since something must be wrong!!!
                    rc.readRegister( spec_t::registerDefault::TRXSTATUS, registerValue);
                    while ( registerValue.TRXSTATUS.TRX_STATUS != spec_t::defaultValue::TRX_OFF );

                    rc.readRegister(  spec_t::registerDefault::PHYTXPWR,  registerValue );
                    registerValue.PHYTXPWR.TX_AUTO_CRC_ON = CFG::autoCRC;
                    rc.writeRegister( spec_t::registerDefault::PHYTXPWR,  registerValue );
					rc.readRegister( spec_t::registerDefault::PHYTXPWR, registerValue);
                    registerValue.value = 0x00;
                    registerValue.IRQ.TRX_END  = true;
                    registerValue.IRQ.RX_START = true;
                    rc.writeRegister( spec_t::registerDefault::IRQMASK,  registerValue );
                }

                /*! \brief  Resets the AT86RF230 radio controller.
                 *  \note   After reset was called the radio controller is
                 *          in state <code>TRX_OFF</code>.
                 */
                void reset() {
                    UseRegmap(rm, Portmap);
                    rm.reset.port = false;  // reset pin LOW
                    SyncRegmap(rm);
                    delay_us( spec_t::Duration::TRX_RESET_TIME_US );
                    rm.reset.port = true;   // reset pin HIGH
                    SyncRegmap(rm);
                    delay_us( spec_t::Duration::TRX_CHIP_RESET_TIME_US );
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
                        if ( CFG::rxOnIdle ) setStateTRX( armarow::PHY::RX_ON );
                        return armarow::PHY::SUCCESS;
                    }

                    // alter size if CRC is enabled
					
                    uint8_t size = pData.size+info::overhead;
					
					if ( size > armarow::PHY::aMaxPHYPacketSize )
                        return armarow::PHY::INVALID_PARAMETER;

					if(CFG::enabledCRC && !CFG::autoCRC)
						*((uint16_t*)(pData.payload+pData.size))=common::CRC::calculateCRC<typename CFG::CRCPolynomial>(pData.payload, size);

                    rc.writeTxFifo(size, pData.payload, CFG::autoCRC);

                    typename ControllerInterface_t::regval_t registerValue;

                    // start sending
                    rc.readRegister(spec_t::registerDefault::TRXSTATE, registerValue);
                    registerValue.TRXSTATE.TRX_CMD = spec_t::defaultValue::TX_START;
                    rc.writeRegister(spec_t::registerDefault::TRXSTATE,registerValue);

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
                    pData.size  = rc.readRxFifo(info::frame, pData.payload, &(pData.minfo.lqi)) - info::overhead;
					typename ControllerInterface_t::regval_t registerValue;
					rc.readRegister(spec_t::registerDefault::PHYED, registerValue);
					pData.minfo.ed=registerValue.value;
					registerValue.value=0;
					if(CFG::autoCRC)
					{
						rc.readRegister(spec_t::registerDefault::PHYRSSI, registerValue);
						pData.minfo.crc=registerValue.PHYRSSI.RX_CRC_VALID;
					}
					else
					{
						uint16_t recvCrc[2]={pData.payload[pData.size], pData.payload[pData.size+1]};
						common::CRC::calculateCRC<typename CFG::CRCPolynomial>(pData.payload, pData.size+2);
						if(recvCrc[0]==pData.payload[pData.size] && recvCrc[1]==pData.payload[pData.size+1])
							pData.minfo.crc=true;
						else
							pData.minfo.crc=false;
					}
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
                    while( !ready() );//FIXME does not work as predicted rethink
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
                 *            <code>busy, IDLE</code>)
                 */
                armarow::PHY::State doCCA(uint8_t& value) { //FIXME still untested
                    TRACE_FUNCTION;
                    typename ControllerInterface_t::regval_t registerValue;

                    armarow::PHY::State cState = (armarow::PHY::State)getStateTRX();
                    if ( cState == armarow::PHY::TRX_OFF ) return cState;
                    if ( cState != armarow::PHY::RX_ON ) return armarow::PHY::BUSY;

                    // do the actual check of the medium
                    rc.readRegister(spec_t::registerDefault::PHYCCCCA, registerValue);
                    registerValue.PHYCCCCA.CCA_REQUEST = true;
                    rc.writeRegister(spec_t::registerDefault::PHYCCCCA,registerValue);
                    //delay_us( spec_t::Duration::TRX_CCA_TIME_US );
					
                    // cca_done and cca_status are valid only for one read
		    delay_us(140);
		    rc.readRegister(spec_t::registerDefault::trxStatus, registerValue);
                    if(registerValue.trxStatus.cca_done)
		    {
			value=registerValue.trxStatus.cca_status;
			return armarow::PHY::success;
		    }
		    else
		    	return armarow::PHY::busy;
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
                 *            <code>success</code>, <code>TRX_OFF</code>,
                 *            <code>TX_ON</code>)
                 */
                armarow::PHY::State doED(uint8_t& pEnergyLevel) {//FIXME still untested
                    TRACE_FUNCTION;
                    typename ControllerInterface_t::regval_t registerValue;

                    armarow::PHY::State cState = (armarow::PHY::State)getStateTRX();
                    if ( ( cState == armarow::PHY::RX_ON) ||
                            ( cState == armarow::PHY::BUSY_RX) ) {
                        registerValue.value = 0x00;
                        rc.writeRegister(spec_t::registerDefault::PHYED, registerValue);
                        delay_us( spec_t::Duration::TRX_CCA_TIME_US );
                        rc.readRegister(spec_t::registerDefault::PHYED, registerValue);
                        pEnergyLevel = spec_t::dBmToPaLevel(registerValue.value);//FIXME check conversion
                        return armarow::PHY::SUCCESS;
                    }
                    return cState;
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
                 *            <code>success</code>,
                 *            <code>UNSUPPORTED_ATTRIBUTE</code>)
                 *  \todo add phyMaxFrameDuration, phySHRDuration, phySymbolsPerOctet
                 */
                armarow::PHY::State getAttribute(armarow::PHY::PIBAttribute pAttribute, void* pAttrValue) {
                    typename ControllerInterface_t::regval_t registerValue;

                    switch (pAttribute) {
                        case armarow::PHY::phyCurrentChannel:
                            rc.readRegister(spec_t::registerDefault::PHYCCCCA, registerValue);
                            *((uint8_t*)pAttrValue) = registerValue.PHYCCCCA.CHANNEL;
                            break;
                        case armarow::PHY::phyChannelsSupported:
                            *((uint32_t*)pAttrValue) = spec_t::Channel::supportedChannel();
                            break;
                        case armarow::PHY::phyTransmitPower:
                            rc.readRegister(spec_t::registerDefault::PHYTXPWR, registerValue);
                            *((uint8_t*)pAttrValue) = registerValue.PHYTXPWR.TX_PWR;
                            break;
                        case armarow::PHY::phyCCAMode:
                            rc.readRegister(spec_t::registerDefault::PHYCCCCA, registerValue);
                            *((uint8_t*)pAttrValue) = registerValue.PHYCCCCA.CCA_MODE;
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
                 *            <code>success</code>,
                 *            <code>UNSUPPORTED_ATTRIBUTE</code>,
                 *            <code>INVALID_PARAMETER</code>,
                 *            <code>READ_ONLY</code>)
                 */
                armarow::PHY::State setAttribute(armarow::PHY::PIBAttribute pAttribute, void* pAttrValue) {
                    typename ControllerInterface_t::regval_t registerValue;

                    switch (pAttribute) {
                        case armarow::PHY::phyCurrentChannel:
                            if ( ( *((uint8_t*)pAttrValue) >= spec_t::Channel::minChannel) &&
                                    ( *((uint8_t*)pAttrValue) <= spec_t::Channel::maxChannel) )
                            {
                                rc.readRegister(spec_t::registerDefault::PHYCCCCA, registerValue);
                                registerValue.PHYCCCCA.CHANNEL = *((uint8_t*)pAttrValue);
                                rc.writeRegister(spec_t::registerDefault::PHYCCCCA,registerValue);
                            } else {
                                return armarow::PHY::INVALID_PARAMETER;
                            }
                            break;
                        case armarow::PHY::phyTransmitPower:
                            if ( *((uint8_t*)pAttrValue) <= 15 )
                            {
                                rc.readRegister(spec_t::registerDefault::PHYTXPWR, registerValue);
                                registerValue.PHYTXPWR.TX_PWR = *((uint8_t*)pAttrValue);
                                rc.writeRegister(spec_t::registerDefault::PHYTXPWR,registerValue);
                            } else {
                                return armarow::PHY::INVALID_PARAMETER;
                            }
                            break;
                        case armarow::PHY::phyCCAMode:
                            if ( *((uint8_t*)pAttrValue) <= 3 )
                            {
                                rc.readRegister(spec_t::registerDefault::PHYCCCCA, registerValue);
                                registerValue.PHYCCCCA.CCA_MODE = *((uint8_t*)pAttrValue);
                                rc.writeRegister(spec_t::registerDefault::PHYCCCCA,registerValue);
                            } else {
                                return armarow::PHY::INVALID_PARAMETER;
                            }
                            break;
						case armarow::PHY::phyCCAThres:
                            if ( *((uint8_t*)pAttrValue) <= 15 )
                            {
                                registerValue.CCATHRES.CCA_ED_THRES = *((uint8_t*)pAttrValue);
                                rc.writeRegister(spec_t::registerDefault::CCATHRES,registerValue);
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
                 *          <code>TRX_OFF</code>, <code>busy_RX</code>,
                 *          <code>busy_TX</code>, <code>TX_ON</code>).
                 */
                armarow::PHY::State getStateTRX() {
                    typename ControllerInterface_t::regval_t registerValue;
                    rc.readRegister( spec_t::registerDefault::TRXSTATUS, registerValue);
                    return (armarow::PHY::State)registerValue.TRXSTATUS.TRX_STATUS;
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
                    typename ControllerInterface_t::regval_t registerValue;

                    rc.readRegister(spec_t::registerDefault::TRXSTATUS, registerValue);
                    switch(pState) {
                        case armarow::PHY::RX_ON:
                            if ( (registerValue.TRXSTATUS.TRX_STATUS == spec_t::defaultValue::TRX_OFF) ||
                                    (registerValue.TRXSTATUS.TRX_STATUS == spec_t::defaultValue::PLL_ON) ) {
                                rc.readRegister(spec_t::registerDefault::TRXSTATE, registerValue);
                                registerValue.TRXSTATE.TRX_CMD = spec_t::defaultValue::RX_ON;
                                rc.writeRegister(spec_t::registerDefault::TRXSTATE, registerValue);
                                do {
                                    rc.readRegister( spec_t::registerDefault::TRXSTATUS, registerValue);
                                } while ( registerValue.TRXSTATUS.TRX_STATUS != spec_t::defaultValue::RX_ON );
                            }
                           break;
                        case armarow::PHY::TRX_OFF:
                            if ( (registerValue.TRXSTATUS.TRX_STATUS == spec_t::defaultValue::RX_ON) ||
                                    (registerValue.TRXSTATUS.TRX_STATUS == spec_t::defaultValue::PLL_ON) ) {
                                rc.readRegister(spec_t::registerDefault::TRXSTATE, registerValue);
                                registerValue.TRXSTATE.TRX_CMD = spec_t::defaultValue::TRX_OFF;
                                rc.writeRegister(spec_t::registerDefault::TRXSTATE, registerValue);
                            }
                            break;
                        case armarow::PHY::TX_ON:
                            if ( (registerValue.TRXSTATUS.TRX_STATUS == spec_t::defaultValue::TRX_OFF) ||
                                    (registerValue.TRXSTATUS.TRX_STATUS == spec_t::defaultValue::RX_ON) ) {
                                rc.readRegister(spec_t::registerDefault::TRXSTATE, registerValue);
                                registerValue.TRXSTATE.TRX_CMD = spec_t::defaultValue::PLL_ON;
                                rc.writeRegister(spec_t::registerDefault::TRXSTATE, registerValue);
                                do {
                                    rc.readRegister( spec_t::registerDefault::TRXSTATUS, registerValue);
                                } while ( registerValue.TRXSTATUS.TRX_STATUS != spec_t::defaultValue::PLL_ON );
                            }
                            break;
                        case armarow::PHY::FORCE_TRX_OFF:
                            reset();
                            break;
                        default:
                            break;
                    }
                }

				void sleep()
				{
					setStateTRX(PHY::TRX_OFF);
					UseRegmap(rm, Portmap);
					rm.sleep.port=true;
					SyncRegmap(rm);
				}

				void wakeup()
				{
					UseRegmap(rm, Portmap);
					rm.sleep.port=true;
					SyncRegmap(rm);
				}
        };
    }; // end namespace phy
}; // end namespace armarow

#endif  //__ARMAROW_AT86RF230_h__
