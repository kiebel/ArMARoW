#pragma once

namespace armarow {
namespace mac {
namespace ieee802_15_4 {
    namespace debug {
        /** \brief log the memory layout of a message of type MessageFrame to log level TRACE**/
        void logFormat(MessageFrame &msg) {
            log::emit<TRACE>()
                << "size  : "    << sizeof(msg.size)               << log::endl
                << "header: "    << sizeof(msg.header)             << log::endl
                << "  control: " << sizeof(msg.header.control)     << log::endl
                << "  seqNr  : " << sizeof(msg.header.seqNr)       << log::endl
                << "  dstAdr : " << sizeof(msg.header.destination) << log::endl
                << "  srcAdr : " << sizeof(msg.header.source)      << log::endl;
        }
    }
}
}
}
