#pragma once

#include <avr-halib/share/delay.h>
#include <avr-halib/share/delegate.h>

#include "spec.h"
#include "attr.h"

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
    class Core : public Attributes< spec::RegMap >, spec::Constants {
        public:
            /** \brief  forward declaration of this driver**/
            typedef Core< Config > type;

            /** \brief  definition of a layer specific message object**/
            typedef common::Message Message;

            typedef spec::State State;
            typedef spec::RegMap RegMap;
            typedef spec::FrameBufferMap FrameBufferMap;

        private:
            /** \brief delegate to contain callback to upper layer **/
            Delegate<> callUpper;

            /** \brief  fetch the current operation state from the radio
             *  \return The current operation state
             **/
            State getState() const{
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                return reinterpret_cast<State>(rm.trx_status);
            }

            /** \brief  change the current operation state of the radio
             *  \param  newState The new operation state
             *  \return true if the state transition was successfull
             **/
            bool setState(const StateType newState) {

                StateType oldState = getState();
                bool stateTransitionPossible = true;

                switch(newState)
                {
                    case(FORCE_TRX_OFF): if(oldState == TRX_OFF)
                                            return true;
                    case(FORCE_TX_ON)  : if(oldState == TX_ON)
                                             return true;
                    default            : if(oldState == newState)
                                             return true;
                }

                switch(newState)
                {
                    case(RX_ON)  : if( !( oldState == TRX_OFF || oldState = PLL_ON ))
                                    stateTransitionPossible = false;
                                   break;
                    case(TRX_OFF): if( !( oldState == RX_ON || oldState == TX_ON ))
                                    stateTransitionPossible = false;
                                   break;
                    case(TX_ON)  : if( !(oldState == TRX_OFF || oldState == RX_ON ))
                                    stateTransitionPossible = false;
                                   break;
                }

                if( !stateTransitionPossible )
                {
                    log::emit<log::Error>() <<
                        PROGMEMSTRING("State transition impossible: ") <<
                        oldState << PROGMEMSTRING(" -> ") <<
                        newState << log::endl;
                    return false;
                }

                while(getState() == CHANGING);

                UseRegMap(rm, RegMap);

                rm.trx_cmd = newState;
                SyncRegMap(rm);

                while(getState() == CHANGING);

                switch(newState)
                {
                    case(FORCE_TRX_OFF): return getState() == TRX_OFF;
                    case(FORCE_TX_ON)  : return getState() == TX_ON;
                    default            : return getState() == newState;
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
                callUpper();
            }
            /** \brief end of cca or ed interrupt handler**/
            void ccaEdDone(){}
            /** \brief address matched interrupt handler**/
            void adressMatched(){}
            /** \brief transmission ended interrupt handler**/
            void txEnded()
            {
                if( rxOnIdle )
                    setState( RX_ON );
                callUpper();
            }
            /** \brief radio awoken from sleep interrupt handler**/
            void awoken()
            {
                if( rxOnIdle )
                    setState( RX_ON );
                else
                    setState( TX_ON );
            }

        public:
            /** \brief Default constructor of the ATmega128RFA1 radio controller.
             *
             *  Initializes and resets the radio controller hardware using
             *  \link reset()\endlink, as well as maps interrupts of the
             *  radio controller to <code>onIRQ()</code>.
             **/
            Core() {
                reset()
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
                UseRegMap(rm, registers);
                rm.sleep  = false;
                rm.reset = true;

                do
                {
                    SyncRegMap(rm);
                }while(rm.reset == true);

                rm.irqMask.value   = 0x00;
                rm.irqStatus.value = 0xff;
                SyncRegMap(rm);

                setState(FORCE_TRX_OFF);

                if(getState != TRX_OFF)
                  log::emit<log::Error>() << 
                      PROGMEMSTRING("Initialization failed! Current state is not " << 
                      TRX_OFF << log::endl;

                redirectISRM(TRX24_RX_START_vect, &type::rxStarted, *this);
                redirectISRM(TRX24_RX_END_vect,   &type::rxEnded,   *this);
                redirectISRM(TRX24_TX_END_vect,   &type::txEnded,   *this);

                rm.irqMask.txEnd   = useInterrupt;
                rm.irqMask.rxEnd   = useInterrupt;
                rm.irqMask.rxStart = useInterrupt;
                SyncRegMap(rm);
            }

            /** \brief  Transmit a message.
             *  \param  msg the message to be transmitted
             *  \return an error code indicating the result of the operation
             **/
            Error send(Message& msg) {
                if( !setState(TX_ON) )
                        return common::BUSY;

                if( msg.size == 0 )
                {
                    if( rxOnIdle )
                        setState( RX_ON );

                    return common::SUCCESS;
                }

                UseRegMap(frameRM, FrameBufferMap);

                frameRM.frame[0] = size;
                for(uint8_t i = 0; i< msg.size; i++)
                  frameRM.frame[i+1] = msg.payload[i];

                SyncRegMap(frameRM);

                setState(BUSY_TX);

                return common::SUCCESS;
            }

            /** \brief  fetch the last received message from the radio
             *  \param  msg the message used as buffer to contain the received message
             *  \return an error code indicating the result of the operation
             **/
            Error receive(Message& msg) {
                UseRegMap(frameRM, FrameBufferMap);
                SyncRegMap(frameRM);

                msg.size = frameRM.length;
                for( uint8_t i = 0; i < msg.size; i++)
                    msg.payload[i]=frameRM.frame[i];

                return common::SUCCESS;
            }
    };
}
}
}
