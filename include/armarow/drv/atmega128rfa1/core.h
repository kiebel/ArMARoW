#pragma once

#include <armarow/common/message.h>
#include <armarow/common/error.h>
#include <avr-halib/avr/InterruptManager/InterruptBinding.h>
#include <avr-halib/avr/InterruptManager/Slot.h>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/if.hpp>
#include "interruptMap.h"
#include <avr-halib/common/singleton.h>
#include <avr-halib/common/delegate.h>

#include "spec.h"

namespace armarow {
namespace drv {
namespace atmega128rfa1 {

    using common::Error;
    using ::boost::mpl::vector;
    using ::Interrupt::Binding;
    using ::Interrupt::Slot;
    using ::boost::mpl::vector;
    using ::boost::mpl::if_c;
    using ::avr_halib::object::Singleton;

    /**  \brief  The core component of the atmega128rfa1 driver
     *   \tparam Config the configuration structure for this driver
     *
     *   this component provides basic send receive and initialization functionality.
     *
     *   \see DefaultConfig for a descriptio of all configuration parameters
     **/
    struct Core
    {
        template< typename Config >
        class configure
        {
            private:
                struct Implementation
                {
                    public:
                        typedef specification::Constants        Constants;
                        /** \brief  definition of a layer specific message object**/
                        typedef common::Message< Constants::maxPayload > MessageType;

                        struct Events : public InterruptMap
                        {
                            typedef InterruptMap::Interrupts EventType;
                        };
                        typedef typename Events::EventType EventType;
                        typedef Singleton< Implementation >     Base;
                        typedef Delegate< MessageType& >        CallbackType;
                        typedef specification::RegMap           RegMap;
                        typedef specification::FrameBufferMap   FrameBufferMap;
                        typedef specification::States           States;
                        typedef specification::StateType        StateType;
                        
                        static const StateType idleState = (Config::rxOnIdle)?(States::rx_on):(States::tx_on);


                    private:
                        CallbackType callUpper;

                    protected:
                        MessageType* currentMsg;

                    public:

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

                        const CallbackType& getCallback() const
                        {
                            return callUpper;
                        }

                        void setCallback(const CallbackType& cb)
                        {
                            callUpper=cb;
                        }
                    private:

                        /** \brief pll locked interrupt handler**/
                        static void pllLock(){}
                        /** \brief pll unlocked interrupt handler**/
                        static void pllUnlock(){}
                        /** \brief start of reception interrupt handler**/
                        static void rxStart(){}
                        /** \brief end of reception interrupt handler**/
                        static void rxEnd()
                        {
                            log::emit<log::Trace>() << PROGMEMSTRING("got message") << log::endl;
                            Base& base=Base::getInstance();
                            if(base.currentMsg)
                            {
                                UseRegMap(frameRM, FrameBufferMap);
                                SyncRegMap(frameRM);

                                log::emit<log::Trace>() << "fetch " << (uint16_t)frameRM.length
                                                       << " message bytes from chip" << log::endl;

                                base.currentMsg->header.size = frameRM.length;
                                if(base.currentMsg->header.size > MessageType::maxSize)
                                {
                                    log::emit<log::Error>() << "impossible size in message: " << (uint16_t)base.currentMsg->header.size << log::endl;
                                    return;
                                }

                                for( uint8_t i = 0; i < base.currentMsg->header.size; i++)
                                    base.currentMsg->payload[i] = frameRM.frame[i];

                                base.currentMsg->properties.state = common::RX_DONE;
                               
                                base.callUpper(*base.currentMsg);
                            }
                            else
                                log::emit<log::Error>() << PROGMEMSTRING("dropped unexpected message") << log::endl;
                        }
                        /** \brief end of cca or ed interrupt handler**/
                        static void ccaDone(){}
                        /** \brief address matched interrupt handler**/
                        static void addressMatch(){}
                        /** \brief transmission ended interrupt handler**/
                        static void txEnd()
                        {
                            log::emit<log::Trace>() << PROGMEMSTRING("tx ended") << log::endl;
                            Base& base=Base::getInstance();
                            base.setState( idleState );
                            if(base.currentMsg)
                            {
                                base.currentMsg->properties.state=common::TX_DONE;
                                base.callUpper(*base.currentMsg);
                            }
                            else
                                log::emit<log::Error>() << PROGMEMSTRING("finished tx of unknown message") << log::endl;
                            
                        }
                        /** \brief radio awoken from sleep interrupt handler**/
                        static void awake()
                        {
                            log::emit<log::Trace>() << PROGMEMSTRING("awoken from sleep") << log::endl;
                            Base::getInstance().setState( idleState );
                        }

                        typedef Slot< Events::rxEnd       ,  Binding::FixedPlainFunction > RxEndSlot;
                        
                        typedef Slot< Events::txEnd       ,  Binding::FixedPlainFunction > TxEndSlot;
                        typedef Slot< Events::awake       ,  Binding::FixedPlainFunction > AwakeSlot;
                        typedef Slot< Events::pllLock     ,  Binding::FixedPlainFunction > PllLockSlot;
                        typedef Slot< Events::pllUnlock   ,  Binding::FixedPlainFunction > PllUnlockSlot;
                        typedef Slot< Events::rxStart     ,  Binding::FixedPlainFunction > RxStartSlot;
                        typedef Slot< Events::ccaDone     ,  Binding::FixedPlainFunction > CcaDoneSlot;
                        typedef Slot< Events::addressMatch,  Binding::FixedPlainFunction > AddressMatchSlot;

                        typedef typename RxEndSlot       ::template Bind< &Implementation::rxEnd        > RxEnd;
                        typedef typename TxEndSlot       ::template Bind< &Implementation::txEnd        > TxEnd;
                        typedef typename AwakeSlot       ::template Bind< &Implementation::awake        > Awake;
                        typedef typename PllLockSlot     ::template Bind< &Implementation::pllLock      > PllLock;
                        typedef typename PllUnlockSlot   ::template Bind< &Implementation::pllUnlock    > PllUnlock;
                        typedef typename RxStartSlot     ::template Bind< &Implementation::rxStart      > RxStart;
                        typedef typename CcaDoneSlot     ::template Bind< &Implementation::ccaDone      > CcaDone;
                        typedef typename AddressMatchSlot::template Bind< &Implementation::addressMatch > AddressMatch;

                        typedef typename vector< RxEnd, TxEnd, Awake >::type InternalSlotList;
                        typedef typename vector< >::type                         EmptySlotList;

                    public:
                        typedef typename if_c<Config::useInterrupts , InternalSlotList, EmptySlotList >::type InterruptSlotList;

                        /** \brief Default constructor of the ATmega128RFA1 radio controller.
                         *
                         *  calls reset()
                         **/
                        Implementation(){
                            reset();
                        }
                        /** \brief Default destructor
                         *
                         *  does nothing yet
                         **/
                        ~Implementation() {}

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
                                awake();
                            }
                            if( rm.irqStatus.pllLock )
                            {
                                rm.irqStatus.pllLock = true;
                                pllLock();
                            }
                            if( rm.irqStatus.pllUnlock )
                            {
                                rm.irqStatus.pllUnlock = true;
                                pllUnlock();
                            }
                            if( rm.irqStatus.rxStart )
                            {
                                rm.irqStatus.rxStart = true;
                                rxStart();
                            }
                            if( rm.irqStatus.rxEnd )
                            {
                                rm.irqStatus.rxEnd = true;
                                rxEnd();
                            }
                            if( rm.irqStatus.ccaEdDone )
                            {
                                rm.irqStatus.ccaEdDone = true;
                                ccaDone();
                            }
                            if( rm.irqStatus.ami )
                            {
                                rm.irqStatus.ami = true;
                                addressMatch();
                            }
                            if( rm.irqStatus.txEnd )
                            {
                                rm.irqStatus.txEnd = true;
                                txEnd();
                            }
                            SyncRegMap(rm);
                        }

                        /** \brief  Reset the ATmega128RFA1
                         *
                         *  \todo register interrupts depending on the configuration value
                         *  \see Specification
                         **/
                        void reset() {
                            currentMsg = 0;
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
                        Error send(MessageType& msg)
                        {
                            if( msg.header.size > MessageType::maxSize )
                                return common::OUT_OF_RANGE;

                            if( msg.header.size == 0 )
                                return common::NO_MESSAGE;

                            if( !setState(States::tx_on) )
                                return common::BUSY;

                            currentMsg = &msg;

                            UseRegMap(frameRM, FrameBufferMap);

                            frameRM.frame[0] = msg.header.size;

                            log::emit<log::Trace>() << "put " << (uint16_t)frameRM.frame[0] << " message bytes to chip" << log::endl;

                            for(uint8_t i = 0; i< msg.header.size; i++)
                              frameRM.frame[i+1] = msg.payload[i];

                            SyncRegMap(frameRM);

                            log::emit<log::Trace>() << "starting transmission of message: " << log::hex << (void*)&msg << log::endl;

                            if( !setState(States::busy_tx))
                                return common::FAILURE;

                            currentMsg->properties.state=common::WORKING;

                            return common::SUCCESS;
                        }

                        /** \brief  fetch the last received message from the radio
                         *  \param  msg the message used as buffer to contain the received message
                         *  \return an error code indicating the result of the operation
                         **/
                        Error receive(MessageType& msg)
                        {
                            currentMsg=&msg;

                            if( !setState(States::rx_on))
                                return common::BUSY;

                            currentMsg->properties.state=common::WORKING;

                            return common::SUCCESS;
                        }
                };

                struct Proxy
                {
                    private:
                        typedef typename Implementation::Base Base;

                    protected:
                        typedef typename Base::CallbackType CallbackType;
                        typedef typename Base::RegMap       RegMap;
                        typedef typename Base::States       States;
                        typedef typename Base::StateType    StateType;
                        static const StateType idleState = Base::idleState;

                        void                setCallback(const CallbackType& cb) {       Base::getInstance().setCallback(cb);  }
                        const CallbackType& getCallback() const                 { return Base::getInstance().getCallback();   }
                        bool                setState(const StateType state)     { return Base::getInstance().setState(state); }
                        StateType           getState() const                    { return Base::getInstance().getState();      }
                        
                    public:
                        typedef typename Base::Constants         Constants;
                        typedef typename Base::MessageType       MessageType;
                        typedef typename Base::Events            Events;
                        typedef typename Base::EventType         EventType;
                        typedef typename Base::InterruptSlotList InterruptSlotList;

                    public:
                        void poll()                     {        Base::getInstance().poll();       }
                        void reset()                    {        Base::getInstance().reset();      }
                        Error send(MessageType& msg)    { return Base::getInstance().send(msg);    }
                        Error receive(MessageType& msg) { return Base::getInstance().receive(msg); }
                };

            public:
                typedef Proxy type;
        };
    };
}
}
}
