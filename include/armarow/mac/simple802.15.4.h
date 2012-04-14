#include <armarow/common/interface.h>
#include "simple802.15.4/csma_ca.h"
#include "simple802.15.4/message.h"

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
    template<typename config, class PhysicalLayer>
    class Simple802_15_4 : public common::Interface< simple802_15_4::Message<config,typename PhysicalLayer::Message> >, public PhysicalLayer
    {
        public:
            typedef typename common::Interface< simple802_15_4::Message<config,typename PhysicalLayer::Message> >::Message Message;
        private:
            /**\brief the CSMA_CA implementation**/
            simple802_15_4::NonBeaconCsmaCa<config, PhysicalLayer, Message> mac;
            /**\brief internal variable for sequence number generation**/
            typename Message::SequenceNumberType seqNumber;

            void txComplete(Message& msg)
            {
                this->txCompleteCallback(msg);
            }

            /** \brief check a message against the protocol specification
             *
             *  \param msg a reference to the message to be checked
             *  \return boolean indicating the validity of the message
             **/
            bool isValid(Message& msg) const
            {
                return true;
            }

        public:
            Simple802_15_4(): mac(*this){}
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
            common::Error send(Message& msg)
            {
                if(mac.isReady())
                {
                    msg.header.seqNumber      = seqNumber++;
                    msg.header.source.id      = config::address;
                    msg.header.source.pan     = config::pan;
                    mac.sendMessage(msg);
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
            common::Error receive(Message& msg)
            {
                common::Error error = this->receive(msg);
                if(error)
                    return error;
                if(!isValid(msg))
                    return common::NO_MESSAGE;
                msg.state=common::RX_DONE;
                return common::SUCCESS;
            }

            void reset()
            {
                this->PhysicalLayer::reset();
                mac.reset();
                seqNumber=0;
            }
    };
}
}
