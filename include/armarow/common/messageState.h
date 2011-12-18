#pragma once

namespace armarow {
namespace common {
    /** \brief Possible message states of all layers **/
    enum MessageState
    {
        NOTHING,    /**< Nothing happended yet              **/
        WORKING,    /**< currently working on it            **/
        TX_DONE,    /**< transmssion complete               **/
        RX_DONE,    /**< message received                   **/
        ACK_DONE,   /**< acknowledgement received           **/
        TX_FAILED,  /**< transmission failed                **/
        RX_FAILED,  /**< received message invalid           **/
        ACK_FAILED  /**< no acknowledgement received in time**/
    };
}
}
