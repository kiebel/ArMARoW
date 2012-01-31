#pragma once

#include <armarow/common/message.h>
#include <armarow/common/error.h>

#include "spec.h"

namespace armarow {
namespace drv {
namespace atmega128rfa1 {

    using common::Error;

    /** \brief Default configuration
     *
     *  default configuration of atmega128rfa1 driver
     **/
    struct DefaultConfig{
        /** \brief defines if the radio will be ready to receive message directlyy after transmitting
         *
         *  default value: true
         **/
        static const bool rxOnIdle      = true;
        /** \brief defines if the radio driver will use interrupts
         *
         *  default value: true
         *  if interrupts are disabled the poll function must be called periodically.
         **/
        static const bool useInterrupts = true;
    };

    /**  \brief  The core component of the atmega128rfa1 driver
     *   \tparam Config the configuration structure for this driver
     *
     *   this component provides basic send receive and initialization functionality.
     *
     *   \see DefaultConfig for a descriptio of all configuration parameters
     **/
    template< typename Config = DefaultConfig >
    class Core : public specification::Constants {
        private:
            typedef specification::States           States;
            typedef specification::StateType        StateType;
            typedef specification::ChannelType      ChannelType;
            typedef specification::CCAModeType      CCAModeType;
            typedef specification::CCAThresholdType CCAThresholdType;
            typedef specification::CCAType          CCAType;

            typedef specification::RegMap           RegMap;
            typedef specification::FrameBufferMap   FrameBufferMap;

            static const StateType idleState = (Config::rxOnIdle)?(States::rx_on):(States::tx_on);
            
        public:
            /** \brief  forward declaration of this driver**/
            typedef Core< Config > type;

            /** \brief  definition of a layer specific message object**/
            typedef common::Message<maxPayload> Message;

        protected:
            typedef Delegate<void>                  CallbackType;

            CallbackType callUpper;

        private:

            /** \brief  fetch the current operation state from the radio
             *  \return The current operation state
             **/
            StateType getState() const{
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                return (StateType)(rm.trx_status);
            }

            /** \brief  change the current operation state of the radio
             *  \param  newState The new operation state
             *  \return true if the state transition was successfull
             **/
            bool setState(const StateType newState) {

                while(getState() == States::changing);

                StateType oldState = getState();

                switch(newState)
                {
                    case(States::force_trx_off): if(oldState == States::trx_off)
                                                    return true;
                    case(States::force_tx_on)  : if(oldState == States::tx_on)
                                                    return true;
                    default                    : if(oldState == newState)
                                                    return true;
                }

                UseRegMap(rm, RegMap);

                rm.trx_cmd = newState;
                SyncRegMap(rm);

                while(getState() == States::changing);

                switch(newState)
                {
                    case(States::force_trx_off): return getState() == States::trx_off;
                    case(States::force_tx_on)  : return getState() == States::tx_on;
                    default                    : return getState() == newState;
                }
            }

            /** \brief pll locked interrupt handler**/
            void pllLocked(){}
            /** \brief pll unlocked interrupt handler**/
            void pllUnlocked(){}
            /** \brief start of reception interrupt handler**/
            void rxStarted(){}
            /** \brief end of reception interrupt handler**/
            void rxEnded()
            {
                log::emit<log::Info>() << PROGMEMSTRING("got message") << log::endl;
                callUpper();
            }
            /** \brief end of cca or ed interrupt handler**/
            void ccaEdDone(){}
            /** \brief address matched interrupt handler**/
            void addressMatched(){}
            /** \brief transmission ended interrupt handler**/
            void txEnded()
            {
                setState( idleState );
                log::emit<log::Info>() << PROGMEMSTRING("tx ended") << log::endl;
                callUpper();
                
            }
            /** \brief radio awoken from sleep interrupt handler**/
            void awoken()
            {
                log::emit<log::Info>() << PROGMEMSTRING("awoken from sleep") << log::endl;
                setState( idleState );
            }

        public:
            /** \brief Default constructor of the ATmega128RFA1 radio controller.
             *
             *  calls reset()
             **/
            Core() : callUpper(){
                reset();
            }
            /** \brief Default destructor
             *
             *  does nothing yet
             **/
            ~Core() {}

            /** \brief poll the current state of the radio
             *
             *  interrupt free access to the radio controller, 
             *  checking current operation state and acting acordingly
             **/
            void poll() {
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                if( rm.irqStatus.awake )
                {
                    rm.irqStatus.awake = true;
                    awoken();
                }
                if( rm.irqStatus.pllLock )
                {
                    rm.irqStatus.pllLock = true;
                    pllLocked();
                }
                if( rm.irqStatus.pllUnlock )
                {
                    rm.irqStatus.pllUnlock = true;
                    pllUnlocked();
                }
                if( rm.irqStatus.rxStart )
                {
                    rm.irqStatus.rxStart = true;
                    rxStarted();
                }
                if( rm.irqStatus.rxEnd )
                {
                    rm.irqStatus.rxEnd = true;
                    rxEnded();
                }
                if( rm.irqStatus.ccaEdDone )
                {
                    rm.irqStatus.ccaEdDone = true;
                    ccaEdDone();
                }
                if( rm.irqStatus.ami )
                {
                    rm.irqStatus.ami = true;
                    addressMatched();
                }
                if( rm.irqStatus.txEnd )
                {
                    rm.irqStatus.txEnd = true;
                    txEnded();
                }
                SyncRegMap(rm);
            }

            /** \brief  Reset the ATmega128RFA1
             *
             *  \todo register interrupts depending on the configuration value
             *  \see Specification
             **/
            void reset() {
                UseRegMap(rm, RegMap);
                rm.sleep  = false;
                rm.reset = true;

                do
                {
                    SyncRegMap(rm);
                }while(rm.reset == true);

                rm.irqMask.value   = 0x00;
                rm.irqStatus.value = 0xff;
                SyncRegMap(rm);

                setState(States::force_trx_off);

                StateType temp;
                do{
                    temp=getState();
                }while(temp==States::nop || temp==States::changing);

                if(temp != States::trx_off)
                  log::emit<log::Error>() << 
                      PROGMEMSTRING("Initialization failed! Current state is ") << 
                      temp << PROGMEMSTRING(" but should be ") << 
                      States::trx_off << log::endl;

                redirectISRM(TRX24_RX_START_vect, &type::rxStarted, *this);
                redirectISRM(TRX24_RX_END_vect,   &type::rxEnded,   *this);
                redirectISRM(TRX24_TX_END_vect,   &type::txEnded,   *this);

                rm.irqMask.txEnd   = Config::useInterrupts;
                rm.irqMask.rxEnd   = Config::useInterrupts;
                rm.irqMask.rxStart = Config::useInterrupts;
                rm.tx_auto_crc_on=false;
                SyncRegMap(rm);

                setState( idleState );
            }

            /** \brief  Transmit a message.
             *  \param  msg the message to be transmitted
             *  \return an error code indicating the result of the operation
             **/
            Error send(Message& msg) {
                if( !setState(States::tx_on) )
                        return common::BUSY;

                if( msg.header.size == 0 )
                {
                    setState( idleState );
                    return common::NO_MESSAGE;
                }

                UseRegMap(frameRM, FrameBufferMap);

                frameRM.frame[0] = msg.header.size;

                log::emit<log::Info>() << "put " << 
                    (uint16_t)frameRM.frame[0] << 
                    " message bytes to chip" << log::endl;

                for(uint8_t i = 0; i< msg.header.size; i++)
                  frameRM.frame[i+1] = msg.payload[i];

                SyncRegMap(frameRM);

                setState(States::busy_tx);

                return common::SUCCESS;
            }

            /** \brief  fetch the last received message from the radio
             *  \param  msg the message used as buffer to contain the received message
             *  \return an error code indicating the result of the operation
             **/
            Error receive(Message& msg) {
                UseRegMap(frameRM, FrameBufferMap);
                SyncRegMap(frameRM);

                log::emit<log::Info>() << "fetch " << 
                    (uint16_t)frameRM.length << 
                    " message bytes from chip" << log::endl;

                msg.header.size = frameRM.length;
                for( uint8_t i = 0; i < msg.header.size; i++)
                    msg.payload[i]=frameRM.frame[i];

                return common::SUCCESS;
            }

        protected:
            /** \brief set trx channel
             *  \param channel the new channel
             **/
            void setChannel(ChannelType channel)
            {
                UseRegMap(rm, RegMap);
                rm.channel=channel;
                SyncRegMap(rm);
                while(!rm.irqStatus.pllLock)
                    SyncRegMap(rm);
                rm.irqStatus.pllLock=true;
                SyncRegMap(rm);
            }

            /** \brief get trx channel
             *  \return the currently used channel
             **/
            ChannelType getChannel() const
            {
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                return rm.channel;
            }

            bool sleep()
            {
                if(!setState(States::trx_off))
                    return false;
                UseRegMap(rm, RegMap);
                rm.sleep=true;
                SyncRegMap(rm);
                return true;
            }

            bool wakeup()
            {
                UseRegMap(rm, RegMap);
                rm.sleep=false;
                do
                    SyncRegMap(rm);
                while(!rm.irqStatus.pllLock);
                rm.irqStatus.pllLock=true;
                SyncRegMap(rm);
                return setState(idleState);
            }

            bool isSleeping() const
            {
                UseRegMap(rm ,RegMap);
                SyncRegMap(rm);
                return rm.sleep && getState()==States::trx_off;
            }

            bool startCCA()
            {
                UseRegMap(rm, RegMap);
                if(!setState(States::rx_on))
                    return false;
                rm.cca_request=true;
                SyncRegMap(rm);
                return true;
            }

            bool ccaDone()
            {
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                if(rm.cca_done)
                {
                    setState(idleState);
                    return true;
                }
                else
                    return false;
            }

            bool getCCAValue() const
            {
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                return rm.cca_status;
            }

            void getCCAParams(CCAModeType& mode, CCAThresholdType& threshold) const
            {
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                mode=rm.cca_mode;
                threshold=rm.cca_threshold;
            }

            bool setCCAParams(CCAModeType mode, CCAThresholdType threshold)
            {
                if(threshold < minCCAThreshold || threshold > maxCCAThreshold)
                    return false;
                UseRegMap(rm, RegMap);
                rm.cca_mode=mode;
                rm.cca_threshold=threshold / ccaThresholdModifier;
                SyncRegMap(rm);
                return true;
            }
    };
}
}
}
