namespace armarow {
namespace mac {
    namespace ieee802_15_4 {
        /*! \brief Defines a data structure storeing the data of the last received message.*/
        struct MessageFilter {
            uint8_t lastSequenceNumber;
            uint8_t lastSourceID;
            uint8_t lastSourcePANId;

            messageFilter() {
                lastSequenceNumber = 0;
                lastSourceID       = 255; //FIXME what does the value stand for (broadcast)?
                lastSourcePANId    = 255; //FIXME what does the value stand for (broadcast)?
            }

            /*! \brief  Checks whether a provided message is a broadcast or not.
             *  \param  messageObject message to be checked
             *  \return TRUE if message is a broadcast, FALSE otherwise.
             */
            bool isBroadcast(MessageFrameMAC& messageObject) {
                return (messageObject.header.dest_adress == macBroadcastAddress);
            }

            /*! \brief  Checks wheter a provided message is addressed to this node.
             *  \param  messageObject message to be checked
             *  \return TRUE if message destination is this node, FALSE otherwise.
             */
            bool isDestination(MessageFrameMAC& messageObject) {
                return (messageObject.header.dest_adress == MACCFG::nodeAddress);
            }

            /*! \brief  Checks whether a provided message is a duplicate of the filter data.
             *  \param  messageObject message to be checked
             *  \return TRUE if message is a duplicate, FALSE otherwise.
             *  \todo check and refactor logic for duplicat checking
             */
            bool isDuplicate(MessageFrameMAC& messageObject) {
                bool result = false;

                //FIXME quick fix don't think that will do!!!
                result = (messageObject.header.controlfield.frametype == Data) true : result;
                result = (lastSequenceNumber == messageObject.header.sequencenumber) true : result;
                result = (lastSourceID == messageObject.header.source_adress) true : result;
                result = (lastSourcePANId == messageObject.header.source_pan) true : result;
                return result;
            }

            /*! \brief  Sets data for duplicat filtering.
             *  \param  messageObject message used for duplicate filtering
             */
            void setFilteringMessage(MessageFrameMAC& messageObject) {
                lastSequenceNumber = messageObject.header.sequencenumber;
                lastSourceID       = messageObject.header.source_adress;
                lastSourcePANId    = messageObject.header.source_pan;
            }
        };
    }
}
}
