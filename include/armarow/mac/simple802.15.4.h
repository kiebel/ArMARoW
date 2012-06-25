#include <armarow/common/interface.h>
#include "simple802.15.4/csma_ca.h"
#include "simple802.15.4/message.h"
#include <armarow/common/attributeContainer.h>

namespace armarow {
namespace mac {

    /** \brief A simplified IEEE 802.15.4 MAC-Protocol
     *
     *  \tparam config structure containing all the configuration parameters of this mac protocol
     *  \tparam PhysicalLayer the basic layer of this protocol
     *  
     *  This MAC-Protocol only support the non-beacon mode.
     *
     *  Currently implemented are the fallowing features:
     *      - CSMA/CA based transmission
     ***/
    struct Simple802_15_4
    {

        struct DefaultConfig : public platform::Timer1BaseConfig
        {
            typedef platform::RadioDriver BaseLayer;
            typedef platform::RadioDriver::DefaultConfig BaseLayerConfig;
            static const uint16_t pan     = 0;
            static const uint16_t address = 1;
        };

        template<typename config = DefaultConfig>
        struct configure
        {
            private:
                typedef simple802_15_4::FrameHeader< config > Header;
                typedef simple802_15_4::Properties< config > Properties;
                
                typedef typename config::BaseLayerConfig BaseConfig;
                typedef typename config::BaseLayer BaseTemplate;
                typedef typename BaseTemplate::template configure< BaseConfig >::type BaseLayer;
                typedef typename BaseLayer::MessageType BaseMessageType;
                typedef typename BaseMessageType::template extend< Header, Properties >::type MessageType;

            public:
                struct type : public common::Interface< MessageType >, 
                              public BaseLayer
                {
                    public:
                        typedef configure::MessageType MessageType;
                        typedef Delegate<MessageType&> CallbackType;

                        struct CallbackTag{};
                        struct Attributes : public BaseLayer::Attributes
                        {
                            typedef common::AttributeContainer< CallbackTag,
                                                        CallbackType > Callback;
                        };



                    private:
                        /**\brief the CSMA_CA implementation**/
                        typedef simple802_15_4::NonBeaconCsmaCa<config, BaseLayer, MessageType> CSMA_CA;
                        CSMA_CA csma_ca;
                        CallbackType callback;
                        MessageType* rxBuffer;

                        /**\brief internal variable for sequence number generation**/
                        typename Header::SequenceNumberType seqNumber;

                        void rxDone(MessageType& msg)
                        {
                            callback(msg);
                        }

                        void txDone(MessageType& msg)
                        {
                            csma_ca.txDone();
                            if(BaseLayer::config::rxOnIdle && rxBuffer)
                                this->BaseLayer::receive(MessageType::down(*rxBuffer));
                            callback(msg);
                        }

                        void handlePhyEvent(BaseMessageType& phyMsg)
                        {
                            log::emit<log::Trace>() << "phy event" << log::endl;

                            MessageType& msg = MessageType::up(phyMsg);
                            switch(msg.properties.state)
                            {
                                case(common::TX_DONE): return txDone(msg);
                                case(common::RX_DONE): return rxDone(msg);
                                default: break;
                            }
                        }

                        /** \brief check a message against the protocol specification
                         *
                         *  \param msg a reference to the message to be checked
                         *  \return boolean indicating the validity of the message
                         **/
                        bool isValid(MessageType& msg) const
                        {
                            return true;
                        }

                    public:
                        typedef typename CSMA_CA::InterruptSlotList InterruptSlotList;


                        type(): csma_ca(*this)
                        {
                            typename BaseLayer::CallbackType cb;
                            cb.template bind< type, &type::handlePhyEvent >( this );
                            BaseLayer::setCallback( cb );
                        }

                        template<typename CustomMsg>
                        common::Error send(CustomMsg& msg)
                        {
                            return send(CustomMsg::down(msg));
                        }


                        /**\brief transmit a message
                         *
                         * \param msg a reference to the message being transmitted
                         *
                         * This function uses the user supplied messsage as a
                         * buffer for storing the message as well as further
                         * information. The user has to make sure to not alter
                         * or delete the buffer during the transmission or
                         * inconsistencies are quite possible. Read access
                         * however is no problem. If the user did not register
                         * the txCompleteCallback, it is possible to infer the
                         * end of the transmission by polling the state
                         * variable of the message. The operation is finished
                         * when the variable changes from BUSY to SUCCESS or
                         * FAILURE.
                         *
                         **/
                        common::Error send(MessageType& msg)
                        {
                            if(csma_ca.isReady())
                            {
                                msg.properties.state  = common::WORKING;
                                msg.header.seqNumber  = seqNumber++;
                                msg.header.source.id  = config::address;
                                msg.header.source.pan = config::pan;
                                csma_ca.sendMessage(msg);
                                return common::SUCCESS;
                            }
                            else
                                return common::BUSY;
                        }

                        /**\brief receive a message
                         *
                         * \param msg a reference to the message buffer to be filled with the received message
                         *
                         * This function uses the user supplied messsage as a
                         * buffer for storing the message as well as further
                         * information. The implementation will overwrite any
                         * content in the buffer during the reception.  This
                         * function returns with NO_MESSAGE if there is no
                         * message to be received. To support polling usage of
                         * the mac protocol.  If the user want`s to be notified
                         * on arrival of a new message, he should register the
                         * rxCallback.
                         *
                         **/
                        common::Error receive(MessageType& msg)
                        {
                            rxBuffer=&msg;
                            return this->BaseLayer::receive(MessageType::down(msg));
                        }

                        void reset()
                        {
                            this->BaseLayer::reset();
                            csma_ca.reset();
                            seqNumber=0;
                        }

                        common::Error setAttribute(const typename Attributes::Callback& cb)
                        {
                            callback=cb.value;
                            return common::SUCCESS;
                        }

                        common::Error getAttribute(typename Attributes::Callback& cb) const
                        {
                            cb.value = callback;
                            return common::SUCCESS;
                        }

                        template<typename T>
                        common::Error setAttribute(const T& attr)
                        {
                            return this->BaseLayer::setAttribute(attr);
                        }

                        template<typename T>
                        common::Error getAttribute(T& attr) const
                        {
                            return this->BaseLayer::getAttribute(attr);
                        }
                };
        };
    };
}
}
